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

#ifndef PEMC_GENERICTRAVERSER_GENERICTRAVERSER_H_
#define PEMC_GENERICTRAVERSER_GENERICTRAVERSER_H_

#include <vector>
#include <gsl/span>
#include <cstdint>
#include <atomic>
#include <stack>
#include <limits>
#include <functional>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/label.h"
#include "pemc/basic/modelCapacity.h"
#include "pemc/basic/rawMemory.h"
#include "pemc/formula/formula.h"
#include "pemc/genericTraverser/ITransitionsOfStateCalculator.h"
#include "pemc/genericTraverser/IPreStateStorageModifier.h"
#include "pemc/genericTraverser/IPostStateStorageModifier.h"

namespace pemc {

  class GenericTraverser {
  protected:
      // a transitionsOfStateCalculator calculates the successors of a given state.
      std::function<ITransitionsOfStateCalculator()> transitionsOfStateCalculatorCreator();

      // preStateStorageModifier can access and modify the transistions returned by
      // the transitionsOfStateCalculator before the corresponding states are added to the StateStorage.
      // The states in the array are given as byte vectors.
      std::vector<std::function<IPreStateStorageModifier()>> preStateStorageModifierCreators();

      // postStateStorageModifier can access and modify the transistions returned by
      // the transitionsOfStateCalculator after the corresponding states are added to the StateStorage.
      // The states in the array are given as indexes into the stateStorage.
      std::vector<std::function<IPostStateStorageModifier()>> postStateStorageModifierCreators();
  public:
      GenericTraverser();
  };

}

#endif  // PEMC_GENERICTRAVERSER_GENERICTRAVERSER_H_