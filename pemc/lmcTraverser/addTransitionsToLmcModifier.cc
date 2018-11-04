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

#include "pemc/lmcTraverser/addTransitionsToLmcModifier.h"

namespace pemc {

  AddTransitionsToLmcModifier::AddTransitionsToLmcModifier(Lmc* _lmc) {
    lmc = _lmc;
  };

  void AddTransitionsToLmcModifier::applyOnTransitions(
      stde::optional<StateIndex> stateIndexOfSource,
      gsl::span<TraversalTransition> transitions,
      void* customPayLoad) {
    auto p_transitions = transitions.data(); // for more speed

    TransitionIndex locationOfFirstEntry;
    auto transitionCount = transitions.size();

    if (stateIndexOfSource==stde::nullopt) {
      locationOfFirstEntry = lmc->getPlaceForNewInitialTransitionEntries(transitionCount);
    } else {
      locationOfFirstEntry = lmc->getPlaceForNewTransitionEntriesOfState(*stateIndexOfSource,transitionCount);
    }
    for (auto i = 0; i < transitionCount; i++) {
      auto newEntry = LmcTransitionEntry(Probability::One(), p_transitions[i].label, p_transitions[i].targetStateIndex);
      lmc->setLmcTransitionEntry(locationOfFirstEntry + i, newEntry);
    }
  }
}
