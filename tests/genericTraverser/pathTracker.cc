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

#include "pemc/generic_traverser/path_tracker.h"

using namespace pemc;

namespace {
  using namespace pemc;

}

TEST(genericTraverser_test, pathTracker_works) {
    auto pathTracker = PathTracker(5000);
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(1);
    pathTracker.pushStateIndex(5);
    pathTracker.pushStateIndex(8);
    StateIndex get1;
    auto get1success = pathTracker.tryGetStateIndex(get1);

    ASSERT_EQ(get1, 8) << "FAIL";
    ASSERT_EQ(get1success, true) << "FAIL";
    ASSERT_EQ(pathTracker.canSplit(), true) << "FAIL";
}


TEST(genericTraverser_test, pathTracker_gets_empty_1) {
    auto pathTracker = PathTracker(5000);
    // iteration 1: initial states
    // initially, we find 3 states (from root)
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(1);
    pathTracker.pushStateIndex(5);
    pathTracker.pushStateIndex(8);

    // then no state (from 8); therefore, the next state is 5
    StateIndex get1;
    auto get1success = pathTracker.tryGetStateIndex(get1);
    pathTracker.pushFrame();

    // then no state (from 5); therefore, the next state is 1
    StateIndex get2;
    auto get2success = pathTracker.tryGetStateIndex(get2);
    pathTracker.pushFrame();

    // then no state (from 1); therefore, no next state
    StateIndex get3;
    auto get3success = pathTracker.tryGetStateIndex(get3);
    pathTracker.pushFrame();

    // finally, we recognize there is nothing to do...
    StateIndex get4;
    auto get4success = pathTracker.tryGetStateIndex(get4);

    ASSERT_EQ(get1, 8) << "FAIL";
    ASSERT_EQ(get1success, true) << "FAIL";
    ASSERT_EQ(get2, 5) << "FAIL";
    ASSERT_EQ(get2success, true) << "FAIL";
    ASSERT_EQ(get3, 1) << "FAIL";
    ASSERT_EQ(get3success, true) << "FAIL";
    ASSERT_EQ(get4, 0) << "FAIL";
    ASSERT_EQ(get4success, false) << "FAIL";
}


TEST(genericTraverser_test, pathTracker_gets_empty_2) {
    auto pathTracker = PathTracker(5000);
    // iteration 1: initial states
    // initially, we find 3 states (from root)
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(1);
    pathTracker.pushStateIndex(5);
    pathTracker.pushStateIndex(8);

    // then no state (from 8); therefore, the next state is 5
    StateIndex get1;
    auto get1success = pathTracker.tryGetStateIndex(get1);
    pathTracker.pushFrame();

    // then one state (from 5); therefore, the next state is 22
    StateIndex get2;
    auto get2success = pathTracker.tryGetStateIndex(get2);
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(22);

    // then no state (from 22); therefore, no next state
    StateIndex get3;
    auto get3success = pathTracker.tryGetStateIndex(get3);
    pathTracker.pushFrame();

    // then no state (from 1); therefore, no next state
    StateIndex get4;
    auto get4success = pathTracker.tryGetStateIndex(get4);
    pathTracker.pushFrame();

    // finally, we recognize there is nothing to do...
    StateIndex get5;
    auto get5success = pathTracker.tryGetStateIndex(get5);

    ASSERT_EQ(get1, 8) << "FAIL";
    ASSERT_EQ(get1success, true) << "FAIL";
    ASSERT_EQ(get2, 5) << "FAIL";
    ASSERT_EQ(get2success, true) << "FAIL";
    ASSERT_EQ(get3, 22) << "FAIL";
    ASSERT_EQ(get3success, true) << "FAIL";
    ASSERT_EQ(get4, 1) << "FAIL";
    ASSERT_EQ(get4success, true) << "FAIL";
    ASSERT_EQ(get5, 0) << "FAIL";
    ASSERT_EQ(get5success, false) << "FAIL";
}


TEST(genericTraverser_test, pathTracker_space_gets_reused) {
    auto pathTracker = PathTracker(3);
    // iteration 1: initial states
    // initially, we find 3 states (from root)
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(1);
    pathTracker.pushStateIndex(5);
    pathTracker.pushStateIndex(8);

    // now 3 states are tracked pathTracker.
    StateIndex get1;
    auto get1success = pathTracker.tryGetStateIndex(get1);
    pathTracker.pushFrame();

    // calling tryGetStateIndex(...) on the empty Frame removes the last state;
    // thus, there is one vacant place to be filled by pushStateIndex(...)
    StateIndex get2;
    auto get2success = pathTracker.tryGetStateIndex(get2);
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(22);
    ASSERT_EQ(get1, 8) << "FAIL";
    ASSERT_EQ(get1success, true) << "FAIL";
    ASSERT_EQ(get2, 5) << "FAIL";
    ASSERT_EQ(get2success, true) << "FAIL";
}
