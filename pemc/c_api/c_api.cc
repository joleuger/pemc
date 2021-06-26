// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2020, Johannes Leupolz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "pemc/c_api/c_api.h"

#include <pemc/formula/adapted_formula.h>
#include "pemc/basic/ThrowAssert.hpp"
#include "pemc/executable_model/abstract_model.h"
#include "pemc/formula/slow_formula_compilation_visitor.h"
#include "pemc/pemc.h"

#include <spdlog/spdlog.h>

using namespace pemc;

namespace {

class CApiModel;

class SlowCApiFormulaCompilationVisitor : public SlowFormulaCompilationVisitor {
 private:
  CApiModel* model;

 public:
  std::function<bool()> result;

  SlowCApiFormulaCompilationVisitor(CApiModel* _model);
  virtual ~SlowCApiFormulaCompilationVisitor() = default;

  virtual void visitAdaptedFormula(AdaptedFormula* formula);
};

int32_t pemc_choose_by_no_of_options(unsigned char* _model_internals,
                                     int32_t no_of_options);

class CApiModel : public AbstractModel {
 private:
  pemc_model_functions model_functions;
  pemc_choice_resolver pemc_choice_resolver;

 public:
  unsigned char* model;

  CApiModel(pemc_model_functions _model_functions) {
    model_functions = _model_functions;

    pemc_choice_resolver.pemc_choose_by_no_of_options =
        (pemc_choose_by_no_of_options_function_type)
            pemc_choose_by_no_of_options;

    pemc_choice_resolver.model_internals =
        reinterpret_cast<unsigned char*>(this);

    model_functions.model_create(&model, &pemc_choice_resolver);

    spdlog::info("C-Api: Model instance created.");
  }

  virtual ~CApiModel() {
    model_functions.model_free(model);
    spdlog::info("C-Api: Model instance freed.");
  }

  virtual void serialize(gsl::span<gsl::byte> position) {
    model_functions.serialize(model,
                              reinterpret_cast<unsigned char*>(position.data()),
                              position.size_bytes());
  }

  virtual void deserialize(gsl::span<gsl::byte> position) {
    model_functions.deserialize(
        model, reinterpret_cast<unsigned char*>(position.data()),
        position.size_bytes());
  }

  virtual void setFormulasForLabel(
      const std::vector<std::shared_ptr<Formula>>& _formulas) {
    spdlog::info("C-Api: Formulas.");
    formulaEvaluators.clear();
    formulaEvaluators.reserve(_formulas.size());
    std::transform(_formulas.begin(), _formulas.end(),
                   std::back_inserter(formulaEvaluators),
                   [this](auto& formula) {
                     auto compiler = SlowCApiFormulaCompilationVisitor(this);
                     formula->visit(&compiler);
                     return std::move(compiler.result);
                   });
  }

  virtual void resetToInitialState() {
    model_functions.reset_to_initial_state(model);
  }

  virtual void step() {
    spdlog::info("C-Api: Step.");
    model_functions.step(model);
  }

  virtual int32_t getStateVectorSize() {
    return model_functions.get_state_vector_size(model);
  }
};

// such formulas should be static in the model classes
class CApiFormula : public AdaptedFormula {
 private:
  pemc_basic_formula_function_type evaluator;

 public:
  CApiFormula(pemc_basic_formula_function_type _evaluator,
              const std::string& _identifier = "")
      : AdaptedFormula(_identifier), evaluator(_evaluator) {}
  virtual ~CApiFormula() = default;

  pemc_basic_formula_function_type getEvaluator() { return evaluator; }
};

SlowCApiFormulaCompilationVisitor::SlowCApiFormulaCompilationVisitor(
    CApiModel* _model)
    : SlowFormulaCompilationVisitor(), model(_model) {}

void SlowCApiFormulaCompilationVisitor::visitAdaptedFormula(
    AdaptedFormula* formula) {
  auto capiFormula = dynamic_cast<CApiFormula*>(formula);
  CApiModel* cmodel = this->model;
  throw_assert(capiFormula != nullptr,
               "Could not cast AdaptedFormula into a CApiFormula");
  result = [capiFormula, cmodel]() {
    spdlog::info("C-Api: Evaluate. Needs to be fixed");
    pemc_basic_formula_function_type evaluator = capiFormula->getEvaluator();
    auto evaluatedFormula = evaluator(cmodel->model);
    spdlog::info("C-Api: Evaluated.");
    return evaluatedFormula;
  };
  spdlog::info("C-Api: Formulas finished.");
}

// formulas
pemc_formula_ref* pemc_register_basic_formula(
    pemc_basic_formula_function_type c_function) {
  spdlog::info("C-Api: Register formula.");
  pemc_formula_ref* pemc_formula =
      (pemc_formula_ref*)malloc(sizeof(pemc_formula_ref));
  if (!pemc_formula)
    return nullptr;

  auto cApiFormula = std::make_shared<CApiFormula>(c_function);
  auto ptr = std::make_unique<std::shared_ptr<CApiFormula>>(cApiFormula);

  pemc_formula->refs = 1;
  pemc_formula->formula = reinterpret_cast<unsigned char*>(ptr.release());

  return pemc_formula;
}

void pemc_ref_formula(pemc_formula_ref* formula_ref) {
  if (!formula_ref)
    return;

  formula_ref->refs++;
}

void pemc_unref_formula(pemc_formula_ref* formula_ref) {
  if (!formula_ref)
    return;

  formula_ref->formula--;

  if (formula_ref->refs <= 0) {
    // revive unique_ptr, which destroys everything inside it, when the program
    // leaves the scope.
    auto ptr = std::unique_ptr<std::shared_ptr<Formula>>(
        reinterpret_cast<std::shared_ptr<Formula>*>(formula_ref->formula));
  }
};

// choice resolvers

int32_t pemc_choose_by_no_of_options(unsigned char* _model_internals,
                                     int32_t no_of_options) {
  spdlog::info("C-Api: Choice Resolver.");
  CApiModel* model_internals = reinterpret_cast<CApiModel*>(_model_internals);
  return model_internals->choose(no_of_options);
};

// main functionality

static int32_t check_reachability_in_executable_model(
    pemc_model_functions model_functions,
    pemc_formula_ref* formula_ref) {
  auto modelCreator = [&model_functions]() {
    return std::make_unique<CApiModel>(model_functions);
  };

  pemc_ref_formula(formula_ref);
  auto formula =
      *reinterpret_cast<std::shared_ptr<Formula>*>(formula_ref->formula);

  auto configuration = Configuration();
  auto pemc = Pemc(configuration);

  auto result = pemc.checkReachabilityInExecutableModel(modelCreator, formula);
  pemc_unref_formula(formula_ref);

  return result;
}

static int32_t test() {
  return sizeof(int);
}
}  // namespace

extern "C" int32_t assign_pemc_functions(pemc_functions* target) {
  // main functionality

  target->check_reachability_in_executable_model =
      (check_reachability_in_executable_model_function_type)
          check_reachability_in_executable_model;

  // formulas
  target->pemc_register_basic_formula =
      (pemc_register_basic_formula_function_type)pemc_register_basic_formula;
  target->pemc_ref_formula = (pemc_ref_formula_function_type)pemc_ref_formula;
  target->pemc_unref_formula =
      (pemc_ref_formula_function_type)pemc_unref_formula;

  // debugging
  target->test = (test_function_type)test;
  return 0;
}