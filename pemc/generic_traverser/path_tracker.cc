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

#include "pemc/generic_traverser/path_tracker.h"

#include <algorithm>
#include <limits>

#include "pemc/basic/ThrowAssert.hpp"
#include "pemc/basic/exceptions.h"

namespace pemc {

PathTracker::PathTracker(int32_t _capacity) {
  capacity = _capacity;
  pathFrames.reserve(_capacity);
  stateIndexEntries.reserve(_capacity);
}

void PathTracker::updateLowestSplittableFrame() {
  auto pathFrameCount = getPathFrameCount();
  for (auto i = std::max(0, lowestSplittableFrame); i < pathFrameCount; ++i) {
    if (pathFrames[i].count > 1) {
      lowestSplittableFrame = i;
      return;
    }
  }
  lowestSplittableFrame = -1;
}

bool PathTracker::canSplit() {
  return lowestSplittableFrame != -1;
}

int32_t PathTracker::getPathFrameCount() {
  return pathFrames.size();
}

void PathTracker::clear() {
  pathFrames.clear();
  stateIndexEntries.clear();
}

void PathTracker::pushFrame() {
  auto indexOfFirstStateIndexEntry = 0;
  if (pathFrames.size() != 0) {
    auto& lastFrame = pathFrames.back();
    indexOfFirstStateIndexEntry =
        lastFrame.indexOfFirstStateIndexEntry + lastFrame.count;
  }
  auto newFrame = PathFrame();
  newFrame.indexOfFirstStateIndexEntry = indexOfFirstStateIndexEntry;
  newFrame.count = 0;
  pathFrames.push_back(newFrame);
}

void PathTracker::pushStateIndex(StateIndex stateIndex) {
  auto& pathFrame = pathFrames.back();

#ifdef DEBUG
  auto indexOfStateIndexEntry =
      pathFrame.indexOfFirstStateIndexEntry + pathFrame.count;
  throw_assert(stateIndexEntries.size() == indexOfStateIndexEntry,
               "BUG! " + std::to_string(stateIndexEntries.size()) +
                   "!=" + std::to_string(indexOfStateIndexEntry));
  if (indexOfStateIndexEntry >= capacity) {
    throw OutOfMemoryException(
        "Unable to allocate an additional depth first search state. Try "
        "increasing the size of the path tracker.");
  }
#endif

  stateIndexEntries.push_back(stateIndex);
  pathFrame.count += 1;

  if (lowestSplittableFrame == -1 && pathFrame.count > 1)
    lowestSplittableFrame = pathFrames.size() - 1;
}

bool PathTracker::tryGetStateIndex(StateIndex& stateIndex) {
  while (pathFrames.size() != 0) {
    auto& lastFrame = pathFrames.back();
    if (lastFrame.count > 0) {
      // Return the frame's topmost state but do not yet remove the state as it
      // might be needed later when constructing the counter example

      auto indexOfStateIndexEntry =
          lastFrame.indexOfFirstStateIndexEntry + lastFrame.count - 1;
      stateIndex = stateIndexEntries[indexOfStateIndexEntry];
      return true;
    }

    // We're done with the frame and we can now remove the topmost state of the
    // previous frame as we no longer need it to construct a counter example
    pathFrames.pop_back();

    if (pathFrames.size() > 0) {
      auto& newLastFrame = pathFrames.back();
      // the last state was erased
      newLastFrame.count -= 1;
      // Erase all remaining stateIndexEntries of the previous frames.
      // Note that because of dangling entries that resulted from stack
      // splitting, starting the erasure after
      // lastFrame.indexOfFirstStateIndexEntry is not enough.
      auto firstStateIndexEntryToDelete =
          newLastFrame.indexOfFirstStateIndexEntry + newLastFrame.count;
      stateIndexEntries.erase(
          stateIndexEntries.begin() + firstStateIndexEntryToDelete,
          stateIndexEntries.end());
    } else {
      // Erase all remaining stateIndexEntries
      stateIndexEntries.erase(stateIndexEntries.begin(),
                              stateIndexEntries.end());
    }
  }

  stateIndex = 0;
  return false;
}

bool PathTracker::splitWork(PathTracker& other) {
  throw NotTestedYetException();  // "multi core not supported, yet"
  throw_assert(canSplit(), "Cannot split the state stack.");
  throw_assert(other.pathFrames.size() == 0, "Expected an empty state stack.");

  // We go through each frame and split the first frame with more than two
  // states in half
  for (auto i = 0; i < pathFrames.size(); ++i) {
    other.pushFrame();

    switch (pathFrames[i].count) {
      // The stack is in an invalid state; clear the other worker's stack and
      // let this worker continue; it will clean up its stack and try to split
      // work later
      case 0:
        other.clear();
        return false;
      // We can't split work here, so just push the state to the other worker's
      // stack
      case 1:
        other.pushStateIndex(
            stateIndexEntries[pathFrames[i].indexOfFirstStateIndexEntry]);
        break;
      // We've encountered a frame where we can actually split work; we always
      // split work as early as possible, that is, as low on the stack as
      // possible, to hopefully maximize the amount of work passed to the other
      // worker
      default:
        // Split the states of the frame
        auto otherCount = pathFrames[i].count / 2;
        auto thisCount = pathFrames[i].count - otherCount;

        // Add the first otherCount states to the other stack
        for (auto j = 0; j < otherCount; ++j) {
          other.pushStateIndex(
              stateIndexEntries[pathFrames[i].indexOfFirstStateIndexEntry + j]);
        }

        // Adjust the count and offset of the frame.
        // This leads to dangling entries.
        pathFrames[i].indexOfFirstStateIndexEntry += otherCount;
        pathFrames[i].count = thisCount;
        // Note that the stateIndexEntries in the range [old
        // indexOfFirstStateIndexEntry, new indexOfFirstStateIndexEntry) did not
        // get removed, but they get removed

        // Find the next splittable frame if this one no longer is
        if (thisCount == 1)
          updateLowestSplittableFrame();

        return true;
    }
  }

  // This stack could not be split, so we clear the other's stack and let some
  // other worker try again
  other.clear();
  return false;
}

std::vector<StateIndex> PathTracker::getCurrentPath() {
  // We have to explicitly allocate and fill a list here, as pointers are not
  // allowed in iterators
  auto steps = pathFrames.size();
  auto path = std::vector<StateIndex>(steps);

  for (size_t i = 0; i < steps; ++i) {
    auto& currentPathFrame = pathFrames[i];
    if (currentPathFrame.count > 0) {
      auto indexOfStateIndexEntry =
          currentPathFrame.indexOfFirstStateIndexEntry +
          currentPathFrame.count - 1;
      path.push_back(stateIndexEntries[indexOfStateIndexEntry]);
    }
  }
  return path;
}
}  // namespace pemc
