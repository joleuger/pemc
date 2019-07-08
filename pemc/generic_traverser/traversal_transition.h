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

#ifndef PEMC_GENERIC_TRAVERSER_TRAVERSAL_TRANSITION_H_
#define PEMC_GENERIC_TRAVERSER_TRAVERSAL_TRANSITION_H_

#include <gsl/gsl_byte>
#include <cstdint>

#include "pemc/basic/tsc_index.h"
#include "pemc/basic/label.h"

namespace pemc {

  enum TraversalTransitionFlags : uint32_t {
    NoFlag,

		/// If set, the value of targetStateIndex is set and targetState is invalid.
		/// If unset, the value of targetState is set and targetStateIndex is invalid.
    IsTargetStateTransformedToIndex = 1,

    /// If set, the transition is invalid and should be ignored
    IsTransitionInvalid = 2,

    /// If set, the transition leads to the stuttering state and the state in targetState
    /// shall be ignored.
    IsToStutteringState = 4
  };

  struct TraversalTransition {
    // 4 bytes / 8 bytes
    union {
      gsl::byte* targetState;  // is set prior to adding  the state to state storage
      StateIndex targetStateIndex; // is set after having added the state to state storage
    };

    // 4 bytes
    TraversalTransitionFlags flags;

    // 4 bytes
    Label label;

    TraversalTransition()
      : targetState(nullptr), flags(TraversalTransitionFlags::NoFlag), label(Label()){
    }

    TraversalTransition(gsl::byte* _targetState, Label _label)
      : targetState(_targetState), flags(TraversalTransitionFlags::NoFlag), label(_label) {
    }
  };


  inline TraversalTransitionFlags operator|(TraversalTransitionFlags a, TraversalTransitionFlags b) {
    return static_cast<TraversalTransitionFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
  }

  inline TraversalTransitionFlags operator|=(TraversalTransitionFlags a, const TraversalTransitionFlags b) {
    a =  static_cast<TraversalTransitionFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    return a;
  }

}

#endif  // PEMC_GENERIC_TRAVERSER_TRAVERSAL_TRANSITION_H_
