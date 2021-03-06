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

#ifndef PEMC_REACHABILITY_TRAVERSER_REACHABILITY_MODIFIER_H_
#define PEMC_REACHABILITY_TRAVERSER_REACHABILITY_MODIFIER_H_

#include <atomic>

#include "pemc/basic/cancellation_token.h"
#include "pemc/generic_traverser/i_post_state_storage_modifier.h"
#include "pemc/lmc/lmc.h"

namespace pemc {
class ReachabilityModifier : public IPostStateStorageModifier {
 private:
  std::atomic<bool>* reached;
  // the cancellationTokenSource is used to cancel the token when a state
  // satisfying the condition is reached
  cancellation_token_source cancellationTokenSource;

 public:
  ReachabilityModifier(std::atomic<bool>* reached,
                       cancellation_token_source cancellationTokenSource);

  virtual void applyOnTransitions(std::optional<StateIndex> stateIndexOfSource,
                                  gsl::span<TraversalTransition> transitions,
                                  void* customPayLoad);
};

}  // namespace pemc

#endif  // PEMC_REACHABILITY_TRAVERSER_REACHABILITY_MODIFIER_H_
