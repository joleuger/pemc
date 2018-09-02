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

#include <functional>

#include "pemc/lmc/lmcModelChecker.h"

namespace {
    using namespace pemc;

    enum PrecalculatedTransitions : int8_t
    {
      Nothing = 0,
      SatisfiedDirect = 1,
      ExcludedDirect = 2,
      Satisfied = 4, // Satisfied for the current run
      Excluded = 8, // Excluded for the current run
      Mark = 16,
    };

    void LmcIteration(Lmc& lmc,
                      gsl::span<PrecalculatedTransitions> &precalculations,
                      gsl::span<Probability> &xold,
                      gsl::span<Probability> &xnew,
                      Probability startValue,
                      std::function<bool(Probability,Probability)> order) {
        auto stateCount = lmc.getStates().size();
        for (StateIndex s = 0; s < stateCount; ++s) {
        	auto transitions = lmc.getTransitions();
        	auto sum = Probability::Zero();
          TransitionIndex begin, end = 0;
          std::tie(begin, end) = lmc.getTransitionIndexesOfState(s);

          for (TransitionIndex t=begin; t<end; t++) {
        		auto& transition = transitions[t];
            auto& precalulated = precalculations[t];
        		if (precalulated & PrecalculatedTransitions::Satisfied)
        		{
        			sum += transition.probability;
        		}
        		else if (precalulated & PrecalculatedTransitions::Excluded)
        		{
        		}
        		else
        		{
        			sum += transition.probability * xold[transition.state];
        		}
        	}
        	xnew[s] = sum;
        }
    }
}

namespace pemc {

  LmcModelChecker::LmcModelChecker(){
  };

}
