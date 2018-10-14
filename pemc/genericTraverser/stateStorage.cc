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


  void zeroMemory();

  void minusOneMemory();

  //memory compare und hashen
  //bei comparen parameter nehmen, sequential consistency beachten mit "atomic_thread_fence"
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

    indexMapper = std::make_unique<std::vector<std::atomic<StateIndex>>>(totalCapacity);
    hashes = std::make_unique<std::vector<std::atomic<StateIndex>, boost::alignment::aligned_allocator<std::atomic<StateIndex>, CacheLineSize> >>(totalCapacity);
  }
}
