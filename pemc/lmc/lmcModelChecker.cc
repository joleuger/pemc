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

#include <boost/timer/timer.hpp>
#include <vector>
#include <utility>

#include "pemc/basic/exceptions.h"
#include "pemc/formula/formulaUtils.h"
#include "pemc/lmc/lmcModelChecker.h"

namespace {
    using namespace pemc;
    using boost::timer::cpu_timer;

    enum PrecalculatedTransition : uint8_t
    {
      Nothing = 0,
      SatisfiedDirect = 1,
      ExcludedDirect = 2,
      Satisfied = 4, // Satisfied for the current run
      Excluded = 8, // Excluded for the current run
      Mark = 16,
    };

    void calculateIteration(Lmc& lmc,
                      gsl::span<PrecalculatedTransition> precalculations,
                      gsl::span<Probability> xold,
                      gsl::span<Probability> xnew) {
        auto stateCount = lmc.getStates().size();
        for (StateIndex s = 0; s < stateCount; ++s) {
        	auto transitions = lmc.getTransitions();
        	auto sum = Probability::Zero();
          TransitionIndex begin, end = 0;
          std::tie(begin, end) = lmc.getTransitionIndexesOfState(s);

          for (TransitionIndex t=begin; t<end; t++) {
        		auto& transition = transitions[t];
            auto& precalulated = precalculations[t];
        		if (precalulated & PrecalculatedTransition::Satisfied)
        		{
        			sum += transition.probability;

        		}
        		else if (precalulated & PrecalculatedTransition::Excluded)
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

    Probability calculateInitialProbability(Lmc& lmc,
                      gsl::span<PrecalculatedTransition> precalculations,
                      gsl::span<Probability> x) {
      	auto transitions = lmc.getTransitions();
      	auto sum = Probability::Zero();
        TransitionIndex begin, end = 0;
        std::tie(begin, end) = lmc.getInitialTransitionIndexes();

        for (TransitionIndex t=begin; t<end; t++) {
      		auto& transition = transitions[t];
          auto& precalulated = precalculations[t];
      		if (precalulated & PrecalculatedTransition::Satisfied)
      		{
      			sum += transition.probability;
      		}
      		else if (precalulated & PrecalculatedTransition::Excluded)
      		{
      		}
      		else
      		{
      			sum += transition.probability * x[transition.state];
      		}
      	}
      	return sum;

    }

    void precalculateDirectSatisfactionAndExclusion(Lmc& lmc,
                    gsl::span<PrecalculatedTransition> precalculatedTransitions,
                    Formula* phi,
                    Formula* psi,
                    std::ostream& cout) {
        cout << "Precalculate transitions that are directly satisfied or excluded. " <<std::endl;
        cpu_timer timer;

        auto psiEvaluator = lmc.createLabelBasedFormulaEvaluator(psi);
        std::function<bool(TransitionIndex)> returnFalse = [](TransitionIndex t) {return false;};
        auto phiEvaluator = phi != nullptr ? lmc.createLabelBasedFormulaEvaluator(psi) : returnFalse;

        //bitwise or casts uint8_t implicitly to int
        auto satisfied = (PrecalculatedTransition) (PrecalculatedTransition::SatisfiedDirect | PrecalculatedTransition::Satisfied);
        auto excluded = (PrecalculatedTransition) (PrecalculatedTransition::ExcludedDirect | PrecalculatedTransition::Excluded);

        for (TransitionIndex t = 0; t < precalculatedTransitions.size(); t++) {
          if (psiEvaluator(t)) {
            precalculatedTransitions[t] = satisfied;
          } else if (phiEvaluator(t)) {
            precalculatedTransitions[t] = excluded;
          } else {
            precalculatedTransitions[t] = PrecalculatedTransition::Nothing;
          }
        }

        timer.stop();
        auto elapsedTime = timer.elapsed();
        auto elapsedTimeStr = format(elapsedTime);
        cout << "\t\tFinished in " << elapsedTimeStr << "." <<std::endl;
    }


    Probability calculateBoundedUntil(Lmc& lmc, Formula* phi, Formula* psi, int bound, std::ostream& cout) {

        cpu_timer timer;

        auto transitions = lmc.getTransitions();
        std::vector<PrecalculatedTransition> precalculations(transitions.size());
        precalculateDirectSatisfactionAndExclusion(lmc, precalculations, phi, psi, cout);

        auto stateCount = lmc.getStates().size();
        auto probablityVector1 = std::vector<Probability>(stateCount);
        auto probablityVector2 = std::vector<Probability>(stateCount);
        auto xold = gsl::span<Probability>(probablityVector1);
        auto xnew = gsl::span<Probability>(probablityVector2);

        for (auto i = 0; i < bound; i++) {
          calculateIteration(lmc, precalculations, xold, xnew);
          std::swap(xold, xnew);

          if (i%10==0) {
            cout << "Calculated " << i << " iterations" << std::endl;
          }
        }

        auto result = calculateInitialProbability(lmc, precalculations, xold);

        timer.stop();
        auto elapsedTime = timer.elapsed();
        auto elapsedTimeStr = format(elapsedTime);

        return result;
    }
}

namespace pemc {

  using boost::timer::cpu_timer;
  namespace stde = std::experimental;

  LmcModelChecker::LmcModelChecker(Lmc& _lmc, const Configuration& _conf)
    :lmc(_lmc),
     conf(_conf)
  {
  };

  Probability LmcModelChecker::calculateProbability(Formula& formulaToCheck) {
    auto matchFormula = tryExtractPhiUntilPsiWithBound(formulaToCheck);
    if (matchFormula== stde::nullopt)
      return Probability::Error();
    Formula* phi;
    Formula* psi;
    stde::optional<int> bound;
    std::tie(phi, psi, bound) = *matchFormula;

    *conf.cout << "Checking formula: " << formulaToString(formulaToCheck) << std::endl;

		if (bound!=stde::nullopt)
		{
			return calculateBoundedUntil(lmc, phi, psi, *bound, *conf.cout);
		}
		else
		{
      // CalculateUnboundUntil
      throw NotImplementedYetException();
		}

    return Probability::Error();
  }

}
