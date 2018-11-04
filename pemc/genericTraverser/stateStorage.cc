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


namespace pemc {

  StateStorage::StateStorage(StateIndex _capacity) {
    throw_assert(_capacity > 1024 && _capacity<=std::numeric_limits<StateIndex>::max(), "capacity invalid");

	  totalCapacity = _capacity;

    indexMapper = std::make_unique<std::vector<std::atomic<StateIndex>>>(totalCapacity);

    hashes = std::make_unique<alignedStateIndexVector>(totalCapacity);

    throw_assert(sizeof(StateIndex)==4, "Used StateStorage not compatible with size of StateIndex");
  }


  gsl::span<gsl::byte> StateStorage::operator [](size_t idx) {
    throw_assert(idx >= 0 && idx < totalCapacity, "idx not in range");
    return gsl::span<gsl::byte>(stateMemory.data() + idx * stateVectorSize, stateVectorSize);
  }

  StateIndex StateStorage::getNumberOfSavedStates() {
    return savedStates;
  }

  StateIndex StateStorage::reserveStateIndex(){
    auto freshCompactIndex = savedStates.fetch_add(1); //returns old value

    // Use the index pointing at the last possible element in the buffers and decrease the size.
    reservedStatesCapacity++;
    cachedStatesCapacity--;

    // Add BucketsPerCacheLine so returnIndex does not interfere with the maximal possible index returned by addState
    // which is _cachedStatesCapacity+BucketsPerCacheLine-1.
    // returnIndex is in range of capacityToReserve, so this is save.
    auto hashBasedIndex = cachedStatesCapacity + BucketsPerCacheLine;
    (*indexMapper)[hashBasedIndex].store(freshCompactIndex);

    throw_assert(hashBasedIndex >= 0 && hashBasedIndex <= totalCapacity + BucketsPerCacheLine, "idx not in range");

    return freshCompactIndex;
  }

  bool StateStorage::addState(gsl::byte* state, StateIndex& index){

			// We don't have to do any out of bounds checks here
			auto hash = hashBuffer(state, stateVectorSize, 0);
			for (auto i = 1; i < ProbeThreshold; ++i) {
				// We store 30 bit hash values as 32 bit integers, with the most significant bit #31 being set
				// indicating the 'written' state and bit #30 indicating whether writing is not yet finished
				// 'empty' is represented by 0
				// We ignore two most significant bits of the original hash, which has no influence on the
				// correctness of the algorithm, but might result in more state comparisons
				auto hashedIndex = hashBuffer(reinterpret_cast<gsl::byte*>(&hash), sizeof(StateIndex), i * 8345723) % cachedStatesCapacity;
				auto memoizedHash = hashedIndex & 0x3FFFFFFF;
				auto cacheLineStart = (hashedIndex / BucketsPerCacheLine) * BucketsPerCacheLine;

				for (auto j = 0; j < BucketsPerCacheLine; ++j)
				{
					auto offset = static_cast<int32_t>(cacheLineStart + (hashedIndex + j) % BucketsPerCacheLine);
					auto currentValue = (*hashes)[offset].load();

          auto expected = 0;
          auto desired = (StateIndex)memoizedHash | (1 << 30);
          auto successFullyChanged = (*hashes)[offset].compare_exchange_strong(expected, desired);

					if (currentValue == 0 && successFullyChanged) {
						auto freshCompactIndex = savedStates.fetch_add(1); //returns old value
						(*indexMapper)[offset].store(freshCompactIndex);

						copyBuffers(state, this->operator[](freshCompactIndex).data(), stateVectorSize);
            // add memory fence to ensure the buffer was copied completely
            // memory_order_release should be enough (could change to sequential consistency)
            std::atomic_thread_fence(std::memory_order_release);

						(*hashes)[offset].store((StateIndex)memoizedHash | (1 << 31));


						index = freshCompactIndex;
						return true;
					}

					// We have to read the hash value again as it might have been written now where it previously was not
					currentValue = (*hashes)[offset].load();
					if ((currentValue & 0x3FFFFFFF) == memoizedHash) {
						while ((currentValue & 1 << 31) == 0)
							currentValue = (*hashes)[offset].load();

						auto compactIndex = (*indexMapper)[offset].load();

            // add memory fence to ensure the buffer was copied to completely (in another thread)
            // memory_order_acquire should be enough (could change to sequential consistency)
            std::atomic_thread_fence(std::memory_order_acquire);
            // now compare
						if (compactIndex!=-1 && areBuffersEqual(state, this->operator[](compactIndex).data(), stateVectorSize)) {
							index = compactIndex;
							return false;
						}
					}
				}
			}

			throw OutOfMemoryException(
				"Failed to find an empty hash table slot within a reasonable amount of time. Try increasing the state capacity.");
	}

  void StateStorage::resizeStateBuffer(){
    stateVectorSize = modelStateVectorSize + preStateStorageModifierStateVectorSize;
    stateMemory.resize(totalCapacity * stateVectorSize);
  }

  void StateStorage::setStateVectorSize(void setStateVectorSize(int32_t _modelStateVectorSize, int32_t _preStateStorageModifierStateVectorSize);){
    modelStateVectorSize = _modelStateVectorSize;
    preStateStorageModifierStateVectorSize = _preStateStorageModifierStateVectorSize;
		resizeStateBuffer();
  }

  void StateStorage::clear(){
     savedStates = 0;

     reservedStatesCapacity = 0;

     // capacity is reduced, because offset returned by this.add() may be up to
     // BucketsPerCacheLine-1 positions bigger than cachedStatesCapacity
     cachedStatesCapacity = totalCapacity - BucketsPerCacheLine;

     //zeros memory of hashes
     for (auto& entry : *hashes) {
       entry.store(0);
     }
     // set -1 to memory of _indexMapper
     for (auto& entry : *indexMapper) {
       entry.store(-1);
     }
  }
}
