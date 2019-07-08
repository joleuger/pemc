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

#ifndef PEMC_GENERIC_TRAVERSER_PATH_TRACKER_H_
#define PEMC_GENERIC_TRAVERSER_PATH_TRACKER_H_

#include <vector>
#include <gsl/span>
#include <cstdint>
#include <atomic>
#include <stack>
#include <limits>

#include "pemc/basic/tsc_index.h"
#include "pemc/basic/label.h"
#include "pemc/basic/model_capacity.h"
#include "pemc/basic/raw_memory.h"
#include "pemc/formula/formula.h"

namespace pemc {
  struct PathFrame {
    // the offset into the states array where the index of the frame's first state is stored.
    int32_t indexOfFirstStateIndexEntry; //offset
    // the number of states the frame consists of.
    int32_t count;
  };

  ///   When enumerating all states of a model in a depth-first fashion, we have to store the next states (that are computed all
	///   at once) somewhere while also being able to generate the counter example. When check a new state, a new frame is allocated
	///   on the stack and all unknown successor states are stored in that frame. We then take the topmost state, compute its
	///   successor states, and so on. When a formula violation is detected, the counter example consists of the last states of each
	///   frame on the stack. When a frame has been fully enumerated without detecting a formula violation, the stack frame is
	///   removed, the topmost state of the topmost frame is removed, and the new topmost state is checked.
  class PathTracker {
  private:
      std::vector<PathFrame> pathFrames;
      std::vector<StateIndex> stateIndexEntries;

      ///   The lowest index of a splittable frame. -1 if no frame is splittable.
      int32_t lowestSplittableFrame = -1;

      //   Maximal number of stateIndexEntries.
      int32_t capacity = 1 << 20;

      ///   Finds the next splittable frame, if any.
      void updateLowestSplittableFrame();

  public:
      PathTracker(int32_t _capacity);

      ///   Indicates whether the pathFrames can be split.
      bool canSplit();

      ///   Gets the number of path frames.
      int32_t getPathFrameCount();

      ///   Clears all pathFrames and its stateIndexEntries.
      void clear();

      ///   Pushes a new frame onto the pathFrames stack.
      void pushFrame();

      /// Adds the stateIndex to the current path frame.
      void pushStateIndex(StateIndex stateIndex);

  		///   Tries to get the topmost stateIndex if there is one.
      ///   Returns false to indicate that the path tracker was empty and no stateIndex was returned.
      bool tryGetStateIndex(StateIndex& stateIndex);

      bool splitWork(PathTracker& other);

      ///   Gets the path the path tracker currently represents, i.e.,
      ///   returns the sequence of topmost states of each frame, starting with
      ///   the oldest one.
      std::vector<StateIndex> getCurrentPath();
  };

}

#endif  // PEMC_GENERIC_TRAVERSER_PATH_TRACKER_H_
