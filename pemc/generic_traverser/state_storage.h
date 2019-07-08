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

#ifndef PEMC_GENERIC_TRAVERSER_STATE_STORAGE_H_
#define PEMC_GENERIC_TRAVERSER_STATE_STORAGE_H_

#include <vector>
#include <gsl/span>
#include <cstdint>
#include <atomic>
#include <boost/align/aligned_allocator.hpp>

#include "pemc/basic/tsc_index.h"
#include "pemc/basic/probability.h"
#include "pemc/basic/label.h"
#include "pemc/basic/model_capacity.h"
#include "pemc/basic/raw_memory.h"
#include "pemc/formula/formula.h"

namespace pemc {

  ///   We store states in a contiguous array, indexed by a continuous variable.
  ///   Therefore, we use state hashes and one level of indirection.
  ///   The hashes are stored in a separate array, using open addressing,
  ///   see Laarman, "Scalable Multi-Core Model Checking", Algorithm 2.3.
  ///   The method addState can be used simultaneously by multiple threads.
  ///   Note: Must be cleared with clear() before used.
  class StateStorage {
  private:

      // The assumed size of a cache line in bytes.
      static const size_t CacheLineSize = 64;

      // The number of attempts that are made to find an empty bucket.
      static const size_t ProbeThreshold = 1000;

      // The number of buckets that can be stored in a cache line.
      static const size_t BucketsPerCacheLine = CacheLineSize / sizeof(StateIndex);

      // special std::vector that is aligned for more speed
      using alignedStateIndexVector = std::vector<std::atomic<StateIndex>, boost::alignment::aligned_allocator<std::atomic<StateIndex>, CacheLineSize> >;

      // The length in bytes of a state vector required for the analysis model.
      int32_t modelStateVectorSize = 0;
      // Extra bytes in state vector for preStateStorage modifiers.
      int32_t preStateStorageModifierStateVectorSize = 0;
      // The length in bytes of the state vector of the analysis model with the extra bytes
		  // required for the preStateStorage modifiers
      int32_t stateVectorSize = 0;

      // The number of saved states
      std::atomic<StateIndex> savedStates;
  	  // The number of states that can be cached and the number of reserved states.
      StateIndex totalCapacity;
      // The number of states that can be cached.
  		StateIndex cachedStatesCapacity;
      // The number of reserved states
      StateIndex reservedStatesCapacity;

      // the memory that contains the serialized states
      std::vector<gsl::byte> stateMemory;
      // maps the hashed based index to the index in the stateMemory and.
      std::unique_ptr<std::vector<std::atomic<StateIndex>>> indexMapper;
      // hashes in next line should be aligned for more speed
      std::unique_ptr<alignedStateIndexVector> hashes;

      void resizeStateBuffer();

  public:
      StateStorage(StateIndex _capacity);

      gsl::span<gsl::byte> operator [](size_t idx);

      StateIndex getNumberOfSavedStates();

      StateIndex reserveStateIndex();

      bool addState(gsl::byte* state, StateIndex& index);

      void setStateVectorSize(int32_t _modelStateVectorSize, int32_t _preStateStorageModifierStateVectorSize);

      void clear();
  };

}

#endif  // PEMC_GENERIC_TRAVERSER_STATE_STORAGE_H_
