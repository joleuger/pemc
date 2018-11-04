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

#include<gtest/gtest.h>

#include "pemc/genericTraverser/stateStorage.h"

using namespace pemc;

namespace {
  using namespace pemc;

}

TEST(genericTraverser_test, stateStorage_works_with_intSized_model) {
    auto stateVectorSize = sizeof(int32_t);
    auto preStateStorageModifierStateVectorSize = 0;
    auto capacity = 10000;

    auto stateStorage = StateStorage(stateVectorSize, capacity);
    stateStorage.setStateVectorSize(stateVectorSize, preStateStorageModifierStateVectorSize);
    stateStorage.clear();

    auto firstState = int32_t(55);
    auto pFirstState = reinterpret_cast<gsl::byte*>(&firstState);
    StateIndex firstStateIndex = -1;
    auto firstAddSuccess = stateStorage.addState(pFirstState, firstStateIndex);

    auto secondState = int32_t(1337);
    auto pSecondState = reinterpret_cast<gsl::byte*>(&secondState);
    StateIndex secondStateIndex = -1;
    auto secondAddSuccess = stateStorage.addState(pSecondState, secondStateIndex);

    auto firstStateAgain = int32_t(55);
    auto pFirstStateAgain = reinterpret_cast<gsl::byte*>(&firstStateAgain);
    StateIndex firstStateAgainIndex = -1;
    auto firstStateAgainAddSuccess = stateStorage.addState(pFirstStateAgain, firstStateAgainIndex);

    ASSERT_EQ(firstAddSuccess, true) << "FAIL";
    ASSERT_EQ(firstStateIndex, 0) << "FAIL";
    ASSERT_EQ(secondAddSuccess, true) << "FAIL";
    ASSERT_EQ(secondStateIndex, 1) << "FAIL";
    ASSERT_EQ(firstStateAgainAddSuccess, false) << "FAIL";
    ASSERT_EQ(firstStateAgainIndex, 0) << "FAIL";
}
