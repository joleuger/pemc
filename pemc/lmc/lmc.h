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

#ifndef PEMC_LMC_LMC_H_
#define PEMC_LMC_LMC_H_

#include <atomic>
#include <string>
#include <vector>
#include <functional>
#include <gsl/span>

#include "pemc/basic/tsc_index.h"
#include "pemc/basic/probability.h"
#include "pemc/basic/label.h"
#include "pemc/basic/model_capacity.h"
#include "pemc/formula/formula.h"

namespace pemc {

  struct LmcStateEntry { TransitionIndex from; int32_t elements; };

  // Transition = Target + Probability
  struct LmcTransitionEntry {
    Probability probability; Label label; StateIndex state;

    LmcTransitionEntry() = default;
    LmcTransitionEntry(Probability _probability, Label _label, StateIndex _state)
      : probability(_probability), label(_label), state(_state) {}
  };

  class Lmc {
  private:
      TransitionIndex maxNumberOfTransitions = 0;
      std::atomic<TransitionIndex> transitionCount{0};
      std::vector<LmcTransitionEntry> transitions;
      TransitionIndex initialTransitionFrom = -1; //is uninitialized at first, but may be something else than 0
      int32_t initialTransitionElements = 0;

      StateIndex maxNumberOfStates = 0;
      StateIndex stateCount = 0;
      std::vector<LmcStateEntry> states;

      std::vector<std::string> labelIdentifier;

      TransitionIndex getPlaceForNewTransitionEntries(NoOfElements number);
  public:
      Lmc();

      gsl::span<LmcStateEntry> getStates();

      gsl::span<std::string> getLabelIdentifier();
      void setLabelIdentifier(const std::vector<std::string>& _labelIdentifier);
      std::function<bool(TransitionIndex)> createLabelBasedFormulaEvaluator(Formula* formula);

      gsl::span<LmcTransitionEntry> getTransitions();
      gsl::span<LmcTransitionEntry> getInitialTransitions();
      std::tuple<TransitionIndex,TransitionIndex> getInitialTransitionIndexes();
      gsl::span<LmcTransitionEntry> getTransitionsOfState(StateIndex state);
      std::tuple<TransitionIndex,TransitionIndex> getTransitionIndexesOfState(StateIndex state);

      TransitionIndex getPlaceForNewTransitionEntriesOfState(StateIndex stateIndex, NoOfElements number);
      TransitionIndex getPlaceForNewInitialTransitionEntries(NoOfElements number);
      void setLmcTransitionEntry(TransitionIndex index, const LmcTransitionEntry& entry);
      void createStutteringState(StateIndex stutteringStateIndex);

      void initialize(ModelCapacity& modelCapacity);
      void finishCreation(StateIndex _stateCount);
      void validate();
  };

}

#endif  // PEMC_LMC_LMC_H_
