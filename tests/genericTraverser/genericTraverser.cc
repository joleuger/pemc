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

#include <gtest/gtest.h>
#include <iostream>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/configuration.h"
#include "pemc/basic/label.h"
#include "pemc/basic/modelCapacity.h"
#include "pemc/basic/rawMemory.h"
#include "pemc/formula/formula.h"
#include "pemc/genericTraverser/genericTraverser.h"
#include "pemc/genericTraverser/ITransitionsCalculator.h"
#include "pemc/genericTraverser/temporaryStateStorage.h"
#include "pemc/genericTraverser/traversalTransition.h"

using namespace pemc;

namespace {
  using namespace pemc;

  class HardCodedTransitionsCalculator : public ITransitionsCalculator {
  private:
      size_t modelStateVectorSize = sizeof(int32_t);

      TemporaryStateStorage temporaryStateStorage;

      std::vector<TraversalTransition> transitions;

      int32_t preStateStorageModifierStateVectorSize = 0;

      StateIndex stateIndex0;
      StateIndex stateIndex1;
      StateIndex stateIndex2;
      StateIndex stateIndex3;
      StateIndex stateIndex4;

      void initializeSomeStates() {
        stateIndex0 = temporaryStateStorage.getFreshStateIndex();
        *((int32_t*) temporaryStateStorage[stateIndex0].data()) = 112;

        stateIndex1 = temporaryStateStorage.getFreshStateIndex();
        *((int32_t*) temporaryStateStorage[stateIndex1].data()) = 1;

        stateIndex2 = temporaryStateStorage.getFreshStateIndex();
        *((int32_t*) temporaryStateStorage[stateIndex2].data()) = 5;

        stateIndex3 = temporaryStateStorage.getFreshStateIndex();
        *((int32_t*) temporaryStateStorage[stateIndex3].data()) = 3;

        stateIndex4 = temporaryStateStorage.getFreshStateIndex();
        *((int32_t*) temporaryStateStorage[stateIndex4].data()) = 1;
      }
  public:

      HardCodedTransitionsCalculator(const Configuration& conf)
          : temporaryStateStorage(conf.successorCapacity) {
        temporaryStateStorage.setStateVectorSize(modelStateVectorSize, preStateStorageModifierStateVectorSize);
        initializeSomeStates();
      }

      virtual int32_t getStateVectorSize() {
        return modelStateVectorSize;
      }

      virtual void setPreStateStorageModifierStateVectorSize(int32_t _preStateStorageModifierStateVectorSize) {
        std::cout << "Resizing temporary state storage. "
                  << "preStateStorageModifierStateVectorSize " << _preStateStorageModifierStateVectorSize << std::endl;
        preStateStorageModifierStateVectorSize = _preStateStorageModifierStateVectorSize;
        temporaryStateStorage.setStateVectorSize(modelStateVectorSize, preStateStorageModifierStateVectorSize);
      }

      virtual gsl::span<TraversalTransition> calculateInitialTransitions() {
        transitions.clear();
        // set initial state "122"
        std::cout << "Calculating initial transitions." << std::endl;
        auto transition0 = TraversalTransition(temporaryStateStorage[stateIndex0].data(), Label());
        transitions.push_back(transition0);
        return transitions;
      }

      virtual gsl::span<TraversalTransition> calculateTransitionsOfState(gsl::span<gsl::byte> state) {
        transitions.clear();
        auto currentState = *((int32_t*) state.data());
        std::cout << "Calculating successors of state " << currentState << "." << std::endl;
        if (currentState == 112) {
          // set successor states "1" and "5"
          auto transition0 = TraversalTransition(temporaryStateStorage[stateIndex1].data(), Label());
          transitions.push_back(transition0);
          auto transition1 = TraversalTransition(temporaryStateStorage[stateIndex2].data(), Label());
          transitions.push_back(transition1);
        } else if (currentState == 1) {
          // set successor states "1"
          auto transition0 = TraversalTransition(temporaryStateStorage[stateIndex1].data(), Label());
          transitions.push_back(transition0);
          auto transition1 = TraversalTransition(temporaryStateStorage[stateIndex4].data(), Label());
          transitions.push_back(transition1);
        } else {
          // set successor state "3"
          auto transition0 = TraversalTransition(temporaryStateStorage[stateIndex3].data(), Label());
          transitions.push_back(transition0);
        }
        return transitions;
      }
  };
}

TEST(genericTraverser_test, genericTraverser_works) {
    auto configuration = Configuration();
    auto traverser = GenericTraverser(configuration);

    // Declare a creator for a ModelExecutor that has an instance of the model that should be executed.
    auto transitionsCalculatorCreator = [&configuration]() -> std::unique_ptr<HardCodedTransitionsCalculator> {
      return std::make_unique<HardCodedTransitionsCalculator>(configuration);
    };
    traverser.transitionsCalculatorCreator = transitionsCalculatorCreator;

    // Traverse the model.
    traverser.traverse();

    // get the number of the states.
    auto getNoOfStates = traverser.getNoOfStates();

    ASSERT_EQ(getNoOfStates, 4) << "FAIL";
}
