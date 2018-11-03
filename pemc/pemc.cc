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
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/lmc/lmcModelChecker.h"
#include "pemc/genericTraverser/genericTraverser.h"
#include "pemc/executableModel/modelExecutor.h"
#include "pemc/lmcTraverser/addTransitionsToLmcModifier.h"

namespace pemc {
  Pemc::Pemc(){
    conf = Configuration();
  };

  Pemc::Pemc(const Configuration& _conf){
    conf = _conf;
  };


  std::unique_ptr<Lmc> Pemc::buildLmcFromExecutableModel(std::function<AbstractModel()> modelCreator, std::vector<Formula> formulas) {
    // initialize an empty Lmc, which will contain the resulting model.
    auto lmc = std::make_unique<Lmc>();
    //lmc->initialize(capacity);

    // Set the labels of the Lmc.
    auto labelIdentifier = std::vector<std::string> {"f1", "f2"};
    //lmc.setLabelIdentifier(labelIdentifier);

    auto traverser = GenericTraverser();

    // Declare a creator for a ModelExecutor that has an instance of the model that should be executed.
    auto transitionsOfStateCalculatorCreator = []() -> ITransitionsOfStateCalculator {
      auto modelExecutor = ModelExecutor();
      // modelExecutor.model = modelCreator();
      return modelExecutor;
    };
    traverser.transitionsOfStateCalculatorCreator = transitionsOfStateCalculatorCreator;

    // Declare a creator for a modifier that adds states to the Lmc.
    auto addTransitionsToLmcModifierCreator = []() -> IPostStateStorageModifier {
      auto modifier = AddTransitionsToLmcModifier();
      //modifier.lmc = lmc;
      return modifier;
    };
    traverser.postStateStorageModifierCreators.push_back(addTransitionsToLmcModifierCreator);

    // Traverse the model.
    //traverser.traverse();

    // Finish the creation of the Lmc and return it.
    //lmc.finishCreation(noOfStates);
    return lmc;
  }

  Probability Pemc::calculateProbabilityToReachStateWithinBound(Lmc& lmc, std::shared_ptr<Formula> formula, int32_t bound) {
    auto finally_formula = std::make_shared<BoundedUnaryFormula>(formula, UnaryOperator::Finally, bound);

    auto mc = LmcModelChecker(lmc, conf);
    auto probability = mc.calculateProbability(*finally_formula);
    return probability;
  }
}
