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

#ifndef PEMC_PEMC_H_
#define PEMC_PEMC_H_

#include <functional>

#include "pemc/basic/configuration.h"
#include "pemc/basic/dll_defines.h"
#include "pemc/basic/probability.h"
#include "pemc/executable_model/abstract_model.h"
#include "pemc/formula/formula.h"
#include "pemc/lmc/lmc.h"

namespace pemc {

// Pemc is the facade, which facilitates calculating probabilities of formulas
// in executable models.
class Pemc {
 private:
  Configuration conf;

 public:
  Pemc();
  Pemc(const Configuration& _conf);

  // The modelCreator creates an instance of an executable model.
  // The formulas are the formulas to include as labels.
  std::unique_ptr<Lmc> buildLmcFromExecutableModel(
      const std::function<std::unique_ptr<AbstractModel>()>& modelCreator,
      std::vector<std::shared_ptr<Formula>> formulas);

  // Lmc is a Labeled Markov chain. This method calculates the probability to
  // reach
  Probability calculateProbabilityToReachStateWithinBound(
      Lmc& lmc,
      std::shared_ptr<Formula> formula,
      int32_t bound);
};

}  // namespace pemc

#endif  // PEMC_PEMC_H_
