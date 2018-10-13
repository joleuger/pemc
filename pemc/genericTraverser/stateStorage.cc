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

#include <limits>
#include <algorithm>
#include <ThrowAssert.hpp>

#include "pemc/basic/exceptions.h"
#include "pemc/genericTraverser/stateStorage.h"

namespace {
  using namespace pemc;

	// The number of attempts that are made to find an empty bucket.
	const size_t ProbeThreshold = 1000;

	// The assumed size of a cache line in bytes.
	const size_t CacheLineSize = 64;

	// The number of buckets that can be stored in a cache line.
	const size_t BucketsPerCacheLine = CacheLineSize / sizeof(StateIndex);

  void zeroMemory();

  void minusOneMemory();
}

namespace pemc {

  StateStorage::StateStorage(int32_t _modelStateVectorSize, StateIndex _capacity) {
    // auto x = ::operator new(sz);
    throw_assert(_capacity > 1024 && _capacity<=std::numeric_limits<StateIndex>::max(), "capacity invalid");

		modelStateVectorSize = _modelStateVectorSize;
	  totalCapacity = _capacity;

    // capacity is reduced, because offset returned by this.add() may be up to
    // BucketsPerCacheLine-1 positions bigger than cachedStatesCapacity
		cachedStatesCapacity = totalCapacity - BucketsPerCacheLine;

		resizeStateBuffer();

		indexMapper = unique_void(::operator new(totalCapacity * sizeof(StateIndex)) );
		pIndexMapper = reinterpret_cast<StateIndex*>(indexMapper.get());

		// We allocate enough space so that we can align the returned pointer such that index 0 is the start of a cache line
    hashes = unique_void(::operator new(totalCapacity * sizeof(StateIndex) + CacheLineSize) );
	  pHashes = reinterpret_cast<StateIndex*>(hashes.get());

    // Improve memory allignment of pHashes, because it is used very often
		if ((uintptr_t)pHashes % CacheLineSize != 0) {
      auto allignedAddress = reinterpret_cast<uintptr_t>(pHashes) + (CacheLineSize - (uintptr_t)pHashes % CacheLineSize);
      pHashes = reinterpret_cast<StateIndex*>(allignedAddress);
    }

		//throw_assert((ulong)_hashMemory - (ulong)_hashBuffer.Pointer, 0ul, (ulong)CacheLineSize);
		//throw_assert((ulong)_hashMemory % CacheLineSize == 0, "Invalid buffer alignment.");
  }
}
