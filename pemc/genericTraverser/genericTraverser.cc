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

    // instantiate an instance of ITransitionsCalculator, which can calculate
    // the successor transitions of a given state (ModelExecutor is the most common
    // example of an ITransitionsCalculator).
    auto transitionsCalculator = transitionsCalculatorCreator();

    // Now the state state storage is initialized with the stateVectorSize
    auto modelStateVectorSize = transitionsCalculator->getStateVectorSize();
    stateStorage = std::make_unique<StateStorage>(modelStateVectorSize, conf.modelCapacity->getMaximalStates());

    // create an instance of each IPreStateStorageModifier
    auto preStateStorageModifiers = std::vector<std::unique_ptr<IPreStateStorageModifier>>(preStateStorageModifierCreators.size());
    std::transform(preStateStorageModifierCreators.begin(), preStateStorageModifierCreators.end(), std::back_inserter(preStateStorageModifiers),
      [](std::function<std::unique_ptr<IPreStateStorageModifier>()> creator){ return creator();} );

    // create an instance of each IPostStateStorageModifier
    auto postStateStorageModifiers = std::vector<std::unique_ptr<IPostStateStorageModifier>>(postStateStorageModifierCreators.size());
    std::transform(postStateStorageModifierCreators.begin(), postStateStorageModifierCreators.end(), std::back_inserter(postStateStorageModifiers),
      [](std::function<std::unique_ptr<IPostStateStorageModifier>()> creator){ return creator();} );

    auto pathTracker = PathTracker(conf.maximalSearchDepth);

    // now start with the actual traversal
    auto initialTransitions = transitionsCalculator->calculateInitialTransitions();

  }

}
