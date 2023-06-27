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
int32_t pemc_choose_by_no_of_options(
    pemc_model_specific_interface* _pemc_interface,
    int32_t no_of_options);

class CApiModel : public AbstractModel {
 private:
  pemc_model_functions model_functions;
  pemc_model_specific_interface pemc_interface;

 public:
  unsigned char* model;

  CApiModel(pemc_model_functions _model_functions) {
    model_functions = _model_functions;

    pemc_interface.pemc_choose_by_no_of_options =
        (pemc_choose_by_no_of_options_function_type)
            pemc_choose_by_no_of_options;

    pemc_interface.model_internals = reinterpret_cast<unsigned char*>(this);

    model_functions.model_create(&model, &pemc_interface);

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

  formula_ref->refs--;

  if (formula_ref->refs <= 0) {
    // revive unique_ptr, which destroys everything inside it, when the program
    // leaves the scope.
    auto ptr = std::unique_ptr<std::shared_ptr<Formula>>(
        reinterpret_cast<std::shared_ptr<Formula>*>(formula_ref->formula));
  }
};

// models

void pemc_ref_lmc(pemc_lmc_ref* lmc_ref) {
  if (!lmc_ref)
    return;

  lmc_ref->refs++;
}

void pemc_unref_lmc(pemc_lmc_ref* lmc_ref) {
  if (!lmc_ref)
    return;

  lmc_ref->refs--;

  if (lmc_ref->refs <= 0) {
    // revive unique_ptr, which destroys everything inside it, when the program
    // leaves the scope.
    auto ptr = std::unique_ptr<std::shared_ptr<Lmc>>(
        reinterpret_cast<std::shared_ptr<Lmc>*>(lmc_ref->lmc));
  }
};

// choice resolvers

int32_t pemc_choose_by_no_of_options(
    pemc_model_specific_interface* _pemc_interface,
    int32_t no_of_options) {
  spdlog::info("C-Api: Choice Resolver.");
  CApiModel* model_internals =
      reinterpret_cast<CApiModel*>(_pemc_interface->model_internals);
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

static pemc_lmc_ref* build_lmc_from_executable_model(
    pemc_model_functions model_functions,
    const pemc_formula_ref** formulas,
    int32_t num_formulas) {
  auto modelCreator = [&model_functions]() {
    return std::make_unique<CApiModel>(model_functions);
  };

  // create a c++ array with the formulas
  std::vector<std::shared_ptr<Formula>> formulasAsVec;
  std::transform(
      formulas, formulas + num_formulas, std::back_inserter(formulasAsVec),
      [](const pemc_formula_ref* element) {
        auto formula =
            *reinterpret_cast<std::shared_ptr<Formula>*>(element->formula);
        return formula;
      });

  auto configuration = Configuration();
  auto pemc = Pemc(configuration);

  auto result = pemc.buildLmcFromExecutableModel(modelCreator, formulasAsVec);

  // wrap the result into pemc_lmc_ref
  pemc_lmc_ref* pemc_lmc = (pemc_lmc_ref*)malloc(sizeof(pemc_lmc_ref));
  if (!pemc_lmc)
    return nullptr;
  pemc_lmc->refs = 1;
  pemc_lmc->lmc = reinterpret_cast<unsigned char*>(result.release());

  return pemc_lmc;
}

static double calculate_probability_to_reach_state_within_bound(
    pemc_lmc_ref* lmc_ref,
    pemc_formula_ref* formula_ref,
    int32_t bound) {
  pemc_ref_lmc(lmc_ref);
  pemc_ref_formula(formula_ref);

  auto lmc = reinterpret_cast<Lmc*>(lmc_ref->lmc);

  auto formula =
      *reinterpret_cast<std::shared_ptr<Formula>*>(formula_ref->formula);

  auto configuration = Configuration();
  auto pemc = Pemc(configuration);

  auto result =
      pemc.calculateProbabilityToReachStateWithinBound(*lmc, formula, bound);

  pemc_unref_formula(formula_ref);
  pemc_unref_lmc(lmc_ref);

  return result.value;
}

static int32_t test() {
  return sizeof(int);
}

static int32_t pemc_choose_int_option(
    pemc_model_specific_interface* pemc_interface,
    const int32_t* options,
    int32_t numOptions) {
  int32_t selected_option =
      pemc_interface->pemc_choose_by_no_of_options(pemc_interface, numOptions);
  return options[selected_option];
}
}  // namespace

extern "C" int32_t assign_pemc_functions(pemc_functions* target) {
  // formulas
  target->pemc_register_basic_formula =
      (pemc_register_basic_formula_function_type)pemc_register_basic_formula;
  target->pemc_ref_formula = (pemc_ref_formula_function_type)pemc_ref_formula;
  target->pemc_unref_formula =
      (pemc_ref_formula_function_type)pemc_unref_formula;

  // models
  target->pemc_ref_lmc = (pemc_ref_lmc_function_type)pemc_ref_lmc;
  target->pemc_unref_lmc = (pemc_unref_lmc_function_type)pemc_unref_lmc;

  // main functionality
  target->check_reachability_in_executable_model =
      (check_reachability_in_executable_model_function_type)
          check_reachability_in_executable_model;
  target->build_lmc_from_executable_model =
      (build_lmc_from_executable_model_function_type)
          build_lmc_from_executable_model;
  target->calculate_probability_to_reach_state_within_bound =
      (calculate_probability_to_reach_state_within_bound_function_type)
          calculate_probability_to_reach_state_within_bound;

  // convenience functions
  target->pemc_choose_int_option =
      (pemc_choose_int_option_function_type)pemc_choose_int_option;

  // debugging
  target->test = (test_function_type)test;
  return 0;
}