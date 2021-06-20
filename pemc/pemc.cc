// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018, Johannes Leupolz
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

#include "pemc/pemc.h"

#include <atomic>

#include "pemc/executable_model/model_executor.h"
#include "pemc/formula/bounded_unary_formula.h"
#include "pemc/generic_traverser/generic_traverser.h"
#include "pemc/lmc/lmc_model_checker.h"
#include "pemc/lmc_traverser/add_transitions_to_lmc_modifier.h"
#include "pemc/lmc_traverser/lmc_choice_resolver.h"
#include "pemc/reachability_traverser/reachability_choice_resolver.h"
#include "pemc/reachability_traverser/reachability_modifier.h"

namespace pemc {
Pemc::Pemc() {
  conf = Configuration();
};

Pemc::Pemc(const Configuration& _conf) {
  conf = _conf;
};

std::unique_ptr<Lmc> Pemc::buildLmcFromExecutableModel(
    const std::function<std::unique_ptr<AbstractModel>()>& modelCreator,
    std::vector<std::shared_ptr<Formula>> formulas) {
  // initialize an empty Lmc, which will contain the resulting model.
  auto lmc = std::make_unique<Lmc>();
  lmc->initialize(*conf.modelCapacity);

  // Set the labels of the Lmc.
  auto labelIdentifier = std::vector<std::string>();
  labelIdentifier.reserve(formulas.size());
  std::transform(formulas.begin(), formulas.end(),
                 std::back_inserter(labelIdentifier),
                 [](std::shared_ptr<Formula>& formula) {
                   return formula->getIdentifier();
                 });
  lmc->setLabelIdentifier(labelIdentifier);

  auto traverser = GenericTraverser(conf);

  // Declare a creator for a ModelExecutor that has an instance of the model
  // that should be executed.
  auto transitionsCalculatorCreator =
      [&modelCreator, &conf = this->conf,
       &formulas]() -> std::unique_ptr<ModelExecutor> {
    auto modelExecutor = std::make_unique<ModelExecutor>(conf);
    auto model = modelCreator();
    model->setFormulasForLabel(formulas);
    modelExecutor->setModel(std::move(model));
    modelExecutor->setChoiceResolver(std::make_unique<LmcChoiceResolver>());
    return modelExecutor;
  };
  traverser.transitionsCalculatorCreator = transitionsCalculatorCreator;

  // Declare a creator for a modifier that adds states to the Lmc.
  auto addTransitionsToLmcModifierCreator =
      [p_lmc = lmc.get()]() -> std::unique_ptr<IPostStateStorageModifier> {
    auto modifier = std::make_unique<AddTransitionsToLmcModifier>(p_lmc);
    return modifier;
  };
  traverser.postStateStorageModifierCreators.push_back(
      addTransitionsToLmcModifierCreator);

  // Traverse the model.
  traverser.traverse();

  // Finish the creation of the Lmc and return it.
  auto getNoOfStates = traverser.getNoOfStates();
  lmc->finishCreation(getNoOfStates);
  return lmc;
}

bool Pemc::checkReachabilityInExecutableModel(
    const std::function<std::unique_ptr<AbstractModel>()>& modelCreator,
    std::shared_ptr<Formula> formula) {
  // initialize an empty Lmc, which will contain the resulting model.
  auto lmc = std::make_unique<Lmc>();
  lmc->initialize(*conf.modelCapacity);

  // Because there is only one formula there is only one label
  std::vector<std::shared_ptr<Formula>> formulas;
  formulas.push_back(formula);

  auto traverser = GenericTraverser(conf);

  // Declare a creator for a ModelExecutor that has an instance of the model
  // that should be executed.
  auto transitionsCalculatorCreator =
      [&modelCreator, &conf = this->conf,
       &formulas]() -> std::unique_ptr<ModelExecutor> {
    auto modelExecutor = std::make_unique<ModelExecutor>(conf);
    auto model = modelCreator();
    model->setFormulasForLabel(formulas);
    modelExecutor->setModel(std::move(model));
    modelExecutor->setChoiceResolver(
        std::make_unique<ReachabilityChoiceResolver>());
    return modelExecutor;
  };
  traverser.transitionsCalculatorCreator = transitionsCalculatorCreator;

  std::atomic<bool> reached(false);

  // Declare a creator for a modifier that adds states to the Lmc.
  auto reachabilityModifierCreator =
      [&reached]() -> std::unique_ptr<IPostStateStorageModifier> {
    auto modifier = std::make_unique<ReachabilityModifier>(&reached);
    return modifier;
  };
  traverser.postStateStorageModifierCreators.push_back(
      reachabilityModifierCreator);

  // Traverse the model.
  traverser.traverse();

  return reached;
}

Probability Pemc::calculateProbabilityToReachStateWithinBound(
    Lmc& lmc,
    std::shared_ptr<Formula> formula,
    int32_t bound) {
  auto finally_formula = std::make_shared<BoundedUnaryFormula>(
      formula, UnaryOperator::Finally, bound);

  auto mc = LmcModelChecker(lmc, conf);
  auto probability = mc.calculateProbability(*finally_formula);
  return probability;
}
}  // namespace pemc
