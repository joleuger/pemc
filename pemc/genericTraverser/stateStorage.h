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

#ifndef PEMC_GENERICTRAVERSER_STATESTORAGE_H_
#define PEMC_GENERICTRAVERSER_STATESTORAGE_H_

#include <vector>
#include <gsl/span>
#include <cstdint>
#include <atomic>
#include <boost/align/aligned_allocator.hpp>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/probability.h"
#include "pemc/basic/label.h"
#include "pemc/basic/modelCapacity.h"
#include "pemc/basic/unique_voidptr.h"
#include "pemc/formula/formula.h"

namespace pemc {

  ///   We store states in a contiguous array, indexed by a continuous variable.
  ///   Therefore, we use state hashes and one level of indirection.
  ///   The hashes are stored in a separate array, using open addressing,
  ///   see Laarman, "Scalable Multi-Core Model Checking", Algorithm 2.3.
  class StateStorage {
  private:

      // The assumed size of a cache line in bytes.
      static const size_t CacheLineSize = 64;

      // The number of attempts that are made to find an empty bucket.
      static const size_t ProbeThreshold = 1000;

      // The number of buckets that can be stored in a cache line.
      static const size_t BucketsPerCacheLine = CacheLineSize / sizeof(StateIndex);

      // The length in bytes of a state vector required for the analysis model.
      int32_t modelStateVectorSize;
      // Extra bytes in state vector for traversal modifiers.
      int32_t traversalModifierStateVectorSize;
      // The length in bytes of the state vector of the analysis model with the extra bytes
		  // required for the traversal
      int32_t stateVectorSize;

      // The number of saved states
      std::atomic<StateIndex> savedStates = 0;
  	  // The number of states that can be cached and the number of reserved states.
      StateIndex totalCapacity;
      // The number of states that can be cached.
  		StateIndex cachedStatesCapacity;
      // The number of reserved states
      StateIndex reservedStatesCapacity = 0;

      // The unique_void_ptr keeps track of the memory to avoid leaks.
      // unique_ptr are not used, because they do not support void.
      unique_void_ptr stateMemory;
      pbyte* pStateMemory;
      std::unique_ptr<std::vector<std::atomic<StateIndex>>> indexMapper;
      // hashes in next line should be aligned for more speed
      std::unique_ptr<std::vector<std::atomic<StateIndex>, boost::alignment::aligned_allocator<std::atomic<StateIndex>, CacheLineSize> >> hashes;

      void resizeStateBuffer();

  public:
      StateStorage(int32_t _modelStateVectorSize, StateIndex _capacity);

      gsl::span<pbyte> operator [](size_t idx);

      StateIndex getNumberOfSavedStates();
      StateIndex reserveStateIndex();
      bool addState(pbyte* state, StateIndex& index);
      void clear(int traversalModifierStateVectorSize);
  };

}

#endif  // PEMC_GENERICTRAVERSER_STATESTORAGE_H_
