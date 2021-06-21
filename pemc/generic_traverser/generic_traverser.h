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

#ifndef PEMC_GENERIC_TRAVERSER_GENERIC_TRAVERSER_H_
#define PEMC_GENERIC_TRAVERSER_GENERIC_TRAVERSER_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <gsl/span>
#include <limits>
#include <stack>
#include <vector>

#include "pemc/basic/cancellation_token.h"
#include "pemc/basic/configuration.h"
#include "pemc/basic/label.h"
#include "pemc/basic/model_capacity.h"
#include "pemc/basic/raw_memory.h"
#include "pemc/basic/tsc_index.h"
#include "pemc/formula/formula.h"
#include "pemc/generic_traverser/i_post_state_storage_modifier.h"
#include "pemc/generic_traverser/i_pre_state_storage_modifier.h"
#include "pemc/generic_traverser/i_transitions_calculator.h"
#include "pemc/generic_traverser/state_storage.h"

namespace pemc {

class GenericTraverser {
 private:
  const Configuration& conf;
  bool createStutteringState = false;

 public:
  GenericTraverser(const Configuration& _conf);

  // a transitionsCalculator calculates the successors of a given state.
  std::function<std::unique_ptr<ITransitionsCalculator>()>
      transitionsCalculatorCreator;

  // preStateStorageModifier can access and modify the transistions returned by
  // the transitionsOfStateCalculator before the corresponding states are added
  // to the StateStorage. The states in the array are given as byte vectors.
  // Note that order matters.
  std::vector<std::function<std::unique_ptr<IPreStateStorageModifier>()>>
      preStateStorageModifierCreators;

  // postStateStorageModifier can access and modify the transistions returned by
  // the transitionsOfStateCalculator after the corresponding states are added
  // to the StateStorage. The states in the array are given as indexes into the
  // stateStorage. Note that order matters.
  std::vector<std::function<std::unique_ptr<IPostStateStorageModifier>()>>
      postStateStorageModifierCreators;

  // stores all encoutered states and maps each state to a unique index.
  std::unique_ptr<StateStorage> stateStorage;

  StateIndex stutteringStateIndex;

  StateIndex getNoOfStates();

  void traverse(cancellation_token cancellationToken);
};

}  // namespace pemc

#endif  // PEMC_GENERIC_TRAVERSER_GENERIC_TRAVERSER_H_
