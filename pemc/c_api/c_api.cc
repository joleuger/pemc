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

class CApiModel : public AbstractModel {
 private:
  pemc_model_functions model_functions;
  unsigned char* model;

 public:
  CApiModel(pemc_model_functions _model_functions) {
    model_functions = _model_functions;
    model_functions.model_create(&model);
  }

  virtual ~CApiModel() { model_functions.model_free(model); }

  virtual void serialize(gsl::span<gsl::byte> position) {
    model_functions.serialize(reinterpret_cast<unsigned char*>(position.data()),
                              position.size_bytes());
  }

  virtual void deserialize(gsl::span<gsl::byte> position) {
    model_functions.deserialize(
        reinterpret_cast<unsigned char*>(position.data()),
        position.size_bytes());
  }

  virtual void setFormulasForLabel(
      const std::vector<std::shared_ptr<Formula>>& _formulas) {
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

  virtual void step() { model_functions.step(model); }

  virtual int32_t getStateVectorSize() {
    return model_functions.get_state_vector_size(model);
  }

  template <typename T>
  std::tuple<Probability, T> choose(
      std::initializer_list<std::tuple<Probability, T>> choices) {
    // This is a Member template and the implementation must stay therefore in
    // the header.
    auto probabilities = std::vector<Probability>();
    probabilities.reserve(choices.size());
    std::transform(choices.begin(), choices.end(),
                   std::back_inserter(probabilities),
                   [](auto& choice) { return std::get<0>(choice); });
    auto chosenIdx = choiceResolver->choose(probabilities);
    return choices.begin()[chosenIdx];
  }

  template <typename T>
  T choose(std::initializer_list<T> choices) {
    // This is a Member template and the implementation must stay therefore in
    // the header.
    auto chosenIdx = choiceResolver->choose(choices.size());
    return choices.begin()[chosenIdx];
  }
};

// such formulas should be static in the model classes
class CApiFormula : public AdaptedFormula {
 private:
  std::function<bool(CApiModel*)> evaluator;

 public:
  CApiFormula(const std::function<bool(CApiModel*)>& _evaluator,
              const std::string& _identifier = "")
      : AdaptedFormula(_identifier), evaluator(_evaluator) {}
  virtual ~CApiFormula() = default;

  std::function<bool(CApiModel*)> getEvaluator() { return evaluator; }
};

SlowCApiFormulaCompilationVisitor::SlowCApiFormulaCompilationVisitor(
    CApiModel* _model)
    : SlowFormulaCompilationVisitor(), model(_model) {}

void SlowCApiFormulaCompilationVisitor::visitAdaptedFormula(
    AdaptedFormula* formula) {
  auto capiFormula = dynamic_cast<CApiFormula*>(formula);
  throw_assert(capiFormula != nullptr,
               "Could not cast AdaptedFormula into a CApiFormula");
  result = std::bind(capiFormula->getEvaluator(), model);
}

}  // namespace

static int32_t test() {
  return sizeof(int);
}

static int32_t wrap_checkReachabilityInExecutableModel(
    pemc_model_functions model_functions) {
  auto modelCreator = [&model_functions]() {
    return std::make_unique<CApiModel>(model_functions);
  };
  std::shared_ptr<Formula> formula;

  auto configuration = Configuration();
  auto pemc = Pemc(configuration);

  return pemc.checkReachabilityInExecutableModel(modelCreator, formula);
}

extern "C" int32_t assign_pemc_functions(pemc_functions* target) {
  target->check_reachability_in_executable_model =
      (check_reachability_in_executable_model_function_type)
          wrap_checkReachabilityInExecutableModel;
  target->test = (test_function_type)test;
  return 0;
}