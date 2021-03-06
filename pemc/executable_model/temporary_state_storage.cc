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

#include "pemc/executable_model/temporary_state_storage.h"

#include <algorithm>
#include <limits>

#include "pemc/basic/ThrowAssert.hpp"
#include "pemc/basic/exceptions.h"

namespace pemc {

TemporaryStateStorage::TemporaryStateStorage(StateIndex _capacity) {
  throw_assert(
      _capacity >= 1024 && _capacity <= std::numeric_limits<StateIndex>::max(),
      "capacity invalid");

  totalCapacity = _capacity;

  throw_assert(sizeof(StateIndex) == 4,
               "Used StateStorage not compatible with size of StateIndex");
}

gsl::span<gsl::byte> TemporaryStateStorage::operator[](size_t idx) {
  throw_assert(idx >= 0 && idx < totalCapacity, "idx not in range");
  return gsl::span<gsl::byte>(stateMemory.data() + idx * stateVectorSize,
                              stateVectorSize);
}

StateIndex TemporaryStateStorage::getNumberOfSavedStates() {
  return savedStates;
}

StateIndex TemporaryStateStorage::getFreshStateIndex() {
  auto stateIndex = savedStates;
  savedStates++;
  return stateIndex;
}

void TemporaryStateStorage::resizeStateBuffer() {
  stateVectorSize =
      modelStateVectorSize + preStateStorageModifierStateVectorSize;
  stateMemory.resize(totalCapacity * stateVectorSize);
}

void TemporaryStateStorage::setStateVectorSize(
    int32_t _modelStateVectorSize,
    int32_t _preStateStorageModifierStateVectorSize) {
  modelStateVectorSize = _modelStateVectorSize;
  preStateStorageModifierStateVectorSize =
      _preStateStorageModifierStateVectorSize;
  resizeStateBuffer();
}

void TemporaryStateStorage::clear() {
  savedStates = 0;
}

}  // namespace pemc
