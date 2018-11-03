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

namespace {
  using namespace pemc;

  class Worker {
  private:
    std::unique_ptr<ITransitionsCalculator> transitionsCalculator;
    std::vector<std::unique_ptr<IPreStateStorageModifier>> preStateStorageModifiers;
    std::vector<std::unique_ptr<IPostStateStorageModifier>> postStateStorageModifiers;
    PathTracker pathTracker;

  public:
    Worker(const Configuration& conf, GenericTraverser& traverser)
      : pathTracker(PathTracker(conf.maximalSearchDepth))
      {
      // currently only single core traversal implemented.

      // instantiate an instance of ITransitionsCalculator, which can calculate
      // the successor transitions of a given state (ModelExecutor is the most common
      // example of an ITransitionsCalculator).
      transitionsCalculator = traverser.transitionsCalculatorCreator();

      // Now the state state storage is initialized with the stateVectorSize
      auto modelStateVectorSize = transitionsCalculator->getStateVectorSize();
      if (!traverser.stateStorage) {
        // the first worker has to create the state storage.
        // Note that the workers must not be created at the same time.
        traverser.stateStorage = std::make_unique<StateStorage>(modelStateVectorSize, conf.modelCapacity->getMaximalStates());
      }

      // create an instance of each IPreStateStorageModifier
      preStateStorageModifiers = std::vector<std::unique_ptr<IPreStateStorageModifier>>(traverser.preStateStorageModifierCreators.size());
      std::transform(traverser.preStateStorageModifierCreators.begin(),
                     traverser.preStateStorageModifierCreators.end(),
                     std::back_inserter(preStateStorageModifiers),
        [](std::function<std::unique_ptr<IPreStateStorageModifier>()> creator){ return creator();} );

      // create an instance of each IPostStateStorageModifier
      postStateStorageModifiers = std::vector<std::unique_ptr<IPostStateStorageModifier>>(traverser.postStateStorageModifierCreators.size());
      std::transform(traverser.postStateStorageModifierCreators.begin(),
                     traverser.postStateStorageModifierCreators.end(),
                     std::back_inserter(postStateStorageModifiers),
        [](std::function<std::unique_ptr<IPostStateStorageModifier>()> creator){ return creator();} );

    }

    void handleTransitions(gsl::span<TraversalTransition> transitions) {
      // traverse initial transitions
      for (auto& transition : transitions) {
      }
    }

    void traverse() {
          // traverse initial transitions
      auto initialTransitions = transitionsCalculator->calculateInitialTransitions();
      handleTransitions(initialTransitions);
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
    // currently only single core traversal implemented.
    auto worker = Worker(conf,*this);
    worker.traverse();

  }

}
