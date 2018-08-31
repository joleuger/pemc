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
#include "pemc/lmc/lmc.h"

namespace pemc {

  Lmc::Lmc(){
  };

  gsl::span<LmcStateEntry> Lmc::getStates() {
     return gsl::span<LmcStateEntry>(states.data(),stateCount);
  }

  gsl::span<LmcTransitionEntry> Lmc::getTransitions() {
     return gsl::span<LmcTransitionEntry>(transitions.data(),transitionCount.load());
  }

  gsl::span<LmcTransitionEntry> Lmc::getInitialTransitions() {
    auto tspan = getTransitions();
    return tspan.subspan(initialTransitionFrom,initialTransitionElements);
  }

  gsl::span<LmcTransitionEntry> Lmc::getTransitionsOfState(StateIndex state){
    auto tspan = getTransitions();
    auto from = states[state].from;
    auto elements = states[state].elements;
    return tspan.subspan(from,elements);
  }

  gsl::span<std::string> Lmc::getLabels() {
    return gsl::span<std::string>(labels);
  }


  void Lmc::reserveSpace(ModelCapacity& modelCapacity) {
    // For Debugging: ClearWithMinus1

    maxNumberOfStates = modelCapacity.getMaximalStates();
    maxNumberOfStates = std::min(std::numeric_limits<StateIndex>::max(),maxNumberOfStates);
    states.resize(maxNumberOfStates);

    #ifdef DEBUG
    initialTransitionFrom = -1;
    for (auto& stateEntry : states) {
      stateEntry.from = -1;
      stateEntry.elements = 0;
    }
    #endif

    // number of transitions is equal to number of targets
    maxNumberOfTransitions = modelCapacity.getMaximalTargets();
    maxNumberOfTransitions = std::min(std::numeric_limits<TransitionIndex>::max(),maxNumberOfTransitions);
    transitions.resize(maxNumberOfTransitions);

    // TODO: to omit initialization, either use malloc and free, or use custom allocator (see stackoverflow)
    // https://stackoverflow.com/questions/35551254/filling-a-vector-with-multiple-threads
    // https://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container/21028912#21028912
  }

  TransitionIndex Lmc::getPlaceForNewTransitionChainElements(NoOfElements number) {
		auto locationOfFirstNewEntry = std::atomic_fetch_add( &transitionCount, number);
		if (locationOfFirstNewEntry + number >= maxNumberOfTransitions || locationOfFirstNewEntry < 0)
			throw OutOfMemoryException("Unable to store transitions. Try increasing the transition capacity.");
		return locationOfFirstNewEntry ;
	}

	void Lmc::createStutteringState(StateIndex stutteringStateIndex) {
		// The stuttering state might not be reached at all.
		// Make sure, that all used algorithms to not require a connected state graph.
    auto &stateEntry = states[stutteringStateIndex];

    // Only for debugging: Check if -1
    #ifdef DEBUG
		throw_assert(stateEntry.from == -1 && stateEntry.elements == 0, "Stuttering state has already been created");
    #endif

		auto locationOfNewEntry = getPlaceForNewTransitionChainElements(1);
    auto &transitionEntry = transitions[locationOfNewEntry];
		transitionEntry.label = Label();
		transitionEntry.probability = Probability(1.0);
		transitionEntry.state = stutteringStateIndex;

    stateEntry.from=locationOfNewEntry;
    stateEntry.elements=1;
	}

  void Lmc::finishCreation(StateIndex _stateCount) {
    // Note: Do not miss to count the optional stuttering state!
    stateCount = _stateCount;
    states.resize(stateCount);
    transitions.resize(transitionCount);
  }
}
