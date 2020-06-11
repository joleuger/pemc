// SPDX-License-Identifier: MIT
// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2019, Johannes Leupolz
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

#include "pemc/lmc/lmc.h"

#include <algorithm>
#include <limits>

#include "pemc/basic/ThrowAssert.hpp"
#include "pemc/basic/exceptions.h"
#include "pemc/formula/generate_label_based_formula_evaluator.h"

namespace pemc {

Lmc::Lmc(){};

gsl::span<LmcStateEntry> Lmc::getStates() {
  return gsl::span<LmcStateEntry>(states.data(), stateCount);
}

gsl::span<LmcTransitionEntry> Lmc::getTransitions() {
  return gsl::span<LmcTransitionEntry>(transitions.data(),
                                       transitionCount.load());
}

gsl::span<LmcTransitionEntry> Lmc::getInitialTransitions() {
  auto tspan = getTransitions();
  return tspan.subspan(initialTransitionFrom, initialTransitionElements);
}

std::tuple<TransitionIndex, TransitionIndex>
Lmc::getInitialTransitionIndexes() {
  // returns begin(inclusive) and end(exclusive)
  // use std::tie(begin, end) = lmc.getInitialTransitionIndexes();
  // or auto& [a,b,c] = C++17
  // (https://en.cppreference.com/w/cpp/language/structured_binding) (Easier)
  // alternatives:
  //    * https://en.wikipedia.org/wiki/Generator_(computer_programming)#C++
  //    * Boost range with irange(1, 10)
  auto from = initialTransitionFrom;
  auto to = initialTransitionFrom + initialTransitionElements;
  return std::make_tuple(from, to);
}

gsl::span<LmcTransitionEntry> Lmc::getTransitionsOfState(StateIndex state) {
  auto tspan = getTransitions();
  auto from = states[state].from;
  auto elements = states[state].elements;
  return tspan.subspan(from, elements);
}

std::tuple<TransitionIndex, TransitionIndex> Lmc::getTransitionIndexesOfState(
    StateIndex state) {
  // returns begin(inclusive) and end(exclusive)
  // use std::tie(begin, end) = lmc.getTransitionIndexesOfState(index);
  auto from = states[state].from;
  auto to = states[state].from + states[state].elements;
  return std::make_tuple(from, to);
}

gsl::span<std::string> Lmc::getLabelIdentifier() {
  return gsl::span<std::string>(labelIdentifier);
}

void Lmc::setLabelIdentifier(const std::vector<std::string>& _labelIdentifier) {
  labelIdentifier = _labelIdentifier;
}

// generates a lambda, which takes a transitionIndex as input and returns the
// result of evaluating formula on this label.
std::function<bool(TransitionIndex)> Lmc::createLabelBasedFormulaEvaluator(
    Formula* formula) {
  auto labelEvaluator =
      generateLabelBasedFormulaEvaluator(getLabelIdentifier(), formula);
  std::function<bool(TransitionIndex)> evaluator =
      [this, labelEvaluator](TransitionIndex transitionIndex) {
        auto transitions = this->getTransitions();
        auto labelOfInputTransition = transitions[transitionIndex].label;
        return labelEvaluator(labelOfInputTransition);
      };
  return evaluator;
}

void Lmc::initialize(ModelCapacity& modelCapacity) {
  maxNumberOfStates = modelCapacity.getMaximalStates();
  maxNumberOfStates =
      std::min(std::numeric_limits<StateIndex>::max(), maxNumberOfStates);
  states.resize(maxNumberOfStates);

#ifdef DEBUG
  // For debugging: be able to check if entries have already been written to
  // (indicated by -1)
  initialTransitionFrom = -1;
  for (auto& stateEntry : states) {
    stateEntry.from = -1;
    stateEntry.elements = 0;
  }
#endif

  // number of transitions is equal to number of targets
  maxNumberOfTransitions = modelCapacity.getMaximalTargets();
  maxNumberOfTransitions = std::min(std::numeric_limits<TransitionIndex>::max(),
                                    maxNumberOfTransitions);
  transitions.resize(maxNumberOfTransitions);

  transitionCount = 0;
  stateCount = 0;

  // TODO: to omit initialization, either use malloc and free, or use custom
  // allocator (see stackoverflow)
  // https://stackoverflow.com/questions/35551254/filling-a-vector-with-multiple-threads
  // https://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container/21028912#21028912
}

TransitionIndex Lmc::getPlaceForNewTransitionEntries(NoOfElements number) {
  auto locationOfFirstNewEntry =
      std::atomic_fetch_add(&transitionCount, number);
  if (locationOfFirstNewEntry + number >= maxNumberOfTransitions ||
      locationOfFirstNewEntry < 0)
    throw OutOfMemoryException(
        "Unable to store transitions. Try increasing the transition capacity.");
  return locationOfFirstNewEntry;
}

TransitionIndex Lmc::getPlaceForNewTransitionEntriesOfState(
    StateIndex stateIndex,
    NoOfElements number) {
  auto locationOfFirstNewEntry = getPlaceForNewTransitionEntries(number);

  auto& stateEntry = states[stateIndex];
  stateEntry.from = locationOfFirstNewEntry;
  stateEntry.elements = number;
  return locationOfFirstNewEntry;
}

TransitionIndex Lmc::getPlaceForNewInitialTransitionEntries(
    NoOfElements number) {
  auto locationOfFirstNewEntry = getPlaceForNewTransitionEntries(number);
  initialTransitionFrom = locationOfFirstNewEntry;
  initialTransitionElements = number;
  return locationOfFirstNewEntry;
}

void Lmc::setLmcTransitionEntry(TransitionIndex index,
                                const LmcTransitionEntry& entry) {
  transitions[index] = entry;
}

void Lmc::createStutteringState(StateIndex stutteringStateIndex) {
  // The stuttering state might not be reached at all.
  // Make sure, that all used algorithms to not require a connected state graph.
  auto& stateEntry = states[stutteringStateIndex];

#ifdef DEBUG
  // For debugging: check if entries have already been written to (indicated by
  // -1)
  throw_assert(stateEntry.from == -1 && stateEntry.elements == 0,
               "Stuttering state has already been created");
#endif

  auto locationOfNewEntry = getPlaceForNewTransitionEntries(1);
  auto& transitionEntry = transitions[locationOfNewEntry];
  transitionEntry.label = Label();
  transitionEntry.probability = Probability(1.0);
  transitionEntry.state = stutteringStateIndex;

  stateEntry.from = locationOfNewEntry;
  stateEntry.elements = 1;
}

void Lmc::finishCreation(StateIndex _stateCount) {
  // Note: Do not miss to count the optional stuttering state!
  stateCount = _stateCount;
  states.resize(stateCount);
  transitions.resize(transitionCount);
}

void Lmc::validate() {
#ifdef DEBUG
  throw_assert(initialTransitionFrom != -1, "Initial transitions not set");
  throw_assert(stateCount > 0, "At least one state needed");
  for (auto& stateEntry : getStates()) {
    throw_assert(stateEntry.from != -1, "Transitions not set");
    throw_assert(stateEntry.elements > 0, "At least one transition needed");
  }
  auto transitionSum = 0.0;
  for (auto& transitionEntry : getTransitions()) {
    throw_assert(
        transitionEntry.state >= 0 && transitionEntry.state < stateCount,
        "State invalid");
    throw_assert(probabilityIsValid(transitionEntry.probability),
                 "Probability invalid");
  }
  for (StateIndex i = 0; i < stateCount; i++) {
    transitionSum = 0.0;
    for (auto& transitionEntry : getTransitionsOfState(i)) {
      transitionSum += transitionEntry.probability.value;
    }
    throw_assert(probabilityIsOne(transitionSum, 0.00000001),
                 "The sum of probabilities of the state " + std::to_string(i) +
                     " is invalid");
  }
  transitionSum = 0.0;
  for (auto& transitionEntry : getInitialTransitions()) {
    throw_assert(
        transitionEntry.state >= 0 && transitionEntry.state < stateCount,
        "State invalid");
    throw_assert(probabilityIsValid(transitionEntry.probability),
                 "Probability invalid");
    transitionSum += transitionEntry.probability.value;
  }
  throw_assert(
      probabilityIsOne(transitionSum, 0.00000001),
      "The sum of probabilities of the initial transitions is invalid");
#endif
}
}  // namespace pemc
