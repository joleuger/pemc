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

#include <limits>
#include <algorithm>
#include <ThrowAssert.hpp>

#include "pemc/basic/exceptions.h"
#include "pemc/genericTraverser/genericTraverser.h"
#include "pemc/genericTraverser/pathTracker.h"
#include "pemc/genericTraverser/traversalTransition.h"
#include "pemc/genericTraverser/temporaryStateStorage.h"

namespace {
  using namespace pemc;

  class Worker {
  public:
    std::unique_ptr<ITransitionsCalculator> transitionsCalculator;
    std::vector<std::unique_ptr<IPreStateStorageModifier>> preStateStorageModifiers;
    std::vector<std::unique_ptr<IPostStateStorageModifier>> postStateStorageModifiers;
    PathTracker pathTracker;
    GenericTraverser& traverser;

    Worker(const Configuration& conf, GenericTraverser& _traverser)
      : pathTracker(PathTracker(conf.maximalSearchDepth)),
        traverser(_traverser)
      {
      // currently only single core traversal implemented.

      // instantiate an instance of ITransitionsCalculator, which can calculate
      // the successor transitions of a given state (ModelExecutor is the most common
      // example of an ITransitionsCalculator).
      transitionsCalculator = traverser.transitionsCalculatorCreator();

      // create an instance of each IPreStateStorageModifier
      preStateStorageModifiers = std::vector<std::unique_ptr<IPreStateStorageModifier>>(traverser.preStateStorageModifierCreators.size());
      std::transform(traverser.preStateStorageModifierCreators.begin(),
                     traverser.preStateStorageModifierCreators.end(),
                     std::back_inserter(preStateStorageModifiers),
        [](std::function<std::unique_ptr<IPreStateStorageModifier>()> creator){ return creator();} );

      // After all preStateStorage modifiers have been initialized, the transitionsCalculator can be
      // aware the size all preStateStorage modifiers require.
      auto preStateStorageModifierStateVectorSize = getPreStateStorageModifierStateVectorSize();
      transitionsCalculator->setPreStateStorageModifierStateVectorSize(preStateStorageModifierStateVectorSize);

      // create an instance of each IPostStateStorageModifier
      postStateStorageModifiers = std::vector<std::unique_ptr<IPostStateStorageModifier>>(traverser.postStateStorageModifierCreators.size());
      std::transform(traverser.postStateStorageModifierCreators.begin(),
                     traverser.postStateStorageModifierCreators.end(),
                     std::back_inserter(postStateStorageModifiers),
        [](std::function<std::unique_ptr<IPostStateStorageModifier>()> creator){ return creator();} );

    }

    int32_t getPreStateStorageModifierStateVectorSize() {
      auto stateVectorSize = 0;
      for (auto& preStateStorageModifier : preStateStorageModifiers) {
        stateVectorSize += preStateStorageModifier->getModifierStateVectorSize();
      }
    }

    void handleTransitions(gsl::span<TraversalTransition> transitions) {
      // handle transitions

      // new states and transitions calculated during this method call
      auto newCalculatedTransitionCount = 0;
      auto newStatesCount = 0;

      for (auto& modifier : preStateStorageModifiers) {
      }

      pathTracker.pushFrame();
      for (auto& transition : transitions) {

        StateIndex targetStateIndex;
        bool isNewState;

        if (transition.flags & TraversalTransitionFlags::IsToStutteringState) {
          isNewState = false;
          targetStateIndex = traverser.stutteringStateIndex;
        } else {
          isNewState = traverser.stateStorage->addState(transition.targetState, targetStateIndex);
        }

        // Replace the targetState pointer with the unique indexes of the transition's target state.
        transition.targetStateIndex = targetStateIndex;
        transition.flags |= TraversalTransitionFlags::IsTargetStateTransformedToIndex;

        if (isNewState) {
          ++newStatesCount;
          pathTracker.pushStateIndex(targetStateIndex);
        }

        ++newCalculatedTransitionCount;
      }

      for (auto& modifier : postStateStorageModifiers) {
      }
    }

    void traverse() {
      // traverse initial transitions
      auto initialTransitions = transitionsCalculator->calculateInitialTransitions();
      handleTransitions(initialTransitions);

      StateIndex stateIndexToTraverse;
      while (pathTracker.tryGetStateIndex(stateIndexToTraverse) ) {
        auto stateToTraverse = (*traverser.stateStorage)[stateIndexToTraverse];
        auto transitions = transitionsCalculator->calculateTransitionsOfState(stateToTraverse);
        handleTransitions(transitions);
      }
    }
  };
}

namespace pemc {

  GenericTraverser::GenericTraverser(const Configuration& _conf)
    : conf(_conf){
  }

  StateIndex GenericTraverser::getNoOfStates() {
    throw_assert(stateStorage, "traverse() has not been called, yet.");
    return stateStorage->getNumberOfSavedStates();
  }

  void GenericTraverser::traverse() {
    // currently only single core traversal implemented. Therefore, instantiate a single worker.
    auto worker = Worker(conf,*this);

    // After the first worker has been initialized, it can be used to derive the stateVectorSize
    // and the traversalModifierStateVectorSize.
    auto modelStateVectorSize = worker.transitionsCalculator->getStateVectorSize();
    auto preStateStorageModifierStateVectorSize = worker.getPreStateStorageModifierStateVectorSize();

    // Now the state state storage is initialized with the stateVectorSize
    stateStorage = std::make_unique<StateStorage>(conf.modelCapacity->getMaximalStates());
    stateStorage->setStateVectorSize(modelStateVectorSize, preStateStorageModifierStateVectorSize);
    stateStorage->clear();

    // create a stuttering state if demanded.
    if (createStutteringState) {
      stutteringStateIndex = stateStorage->reserveStateIndex();
    }

    // conduct the actual traversal
    worker.traverse();
  }

}
