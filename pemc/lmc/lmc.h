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

#ifndef PEMC_LMC_LMC_H_
#define PEMC_LMC_LMC_H_

#include <string>
#include <vector>
#include <gsl/span>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/probability.h"
#include "pemc/basic/label.h"
#include "pemc/basic/modelCapacity.h"

namespace pemc {

  struct LmcStateEntry { TransitionIndex from; int elements; };

  // Transition = Target + Probability
  struct LmcTransitionEntry { Probability probability; Label label; StateIndex state; };

  class Lmc {
  private:
      TransitionIndex transitionCount = 0;
      std::vector<LmcTransitionEntry> transitions;

      int initialTransitionFrom = 0;
      int initialTransitionElements = 0;
      StateIndex stateCount = 0;
      std::vector<LmcStateEntry> states;

      std::vector<std::string> labels;
  public:
      Lmc();

      gsl::span<LmcStateEntry> getStates();

      gsl::span<std::string> getLabels();

      gsl::span<LmcTransitionEntry> getTransitions();
      gsl::span<LmcTransitionEntry> getInitialTransitions();
      gsl::span<LmcTransitionEntry> getTransitionsOfState(StateIndex state);

      void reserveSpace(ModelCapacity& modelCapacity);
      void shrinkToFit();
  };

}

#endif  // PEMC_LMC_LMC_H_
