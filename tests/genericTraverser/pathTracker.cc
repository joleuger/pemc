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

#include "pemc/genericTraverser/pathTracker.h"

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


TEST(genericTraverser_test, pathTracker_gets_empty) {
    auto pathTracker = PathTracker(5000);
    // initially, we find 3 states (from root)
    pathTracker.pushFrame();
    pathTracker.pushStateIndex(1);
    pathTracker.pushStateIndex(5);
    pathTracker.pushStateIndex(8);
    // then no state (from 8)
    pathTracker.pushFrame();
    StateIndex get1;
    auto get1success = pathTracker.tryGetStateIndex(get1);
    // then no state (from 5)
    pathTracker.pushFrame();
    StateIndex get2;
    auto get2success = pathTracker.tryGetStateIndex(get2);
    // then no state (from 1)
    pathTracker.pushFrame();
    StateIndex get3;
    auto get3success = pathTracker.tryGetStateIndex(get3);

    ASSERT_EQ(get1, 5) << "FAIL";
    ASSERT_EQ(get1success, true) << "FAIL";
    ASSERT_EQ(get2, 1) << "FAIL";
    ASSERT_EQ(get2success, true) << "FAIL";
    ASSERT_EQ(get3, 0) << "FAIL";
    ASSERT_EQ(get3success, false) << "FAIL";
}
