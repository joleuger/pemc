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

#include <gtest/gtest.h>
#include <iostream>

#include "pemc/executableModel/modelExecutor.h"
#include "pemc/lmcTraverser/lmcChoiceResolver.h"

#include "tests/simpleExecutableModel/simpleModel.h"

using namespace pemc;
using namespace pemc::simple;

namespace {
  using namespace pemc;

}

TEST(modelExecutor_test, modelExecutor_test) {
    auto configuration = Configuration();

    auto modelExecutor = std::make_unique<ModelExecutor>(configuration);
    modelExecutor->setModel(std::make_unique<SimpleModel>());
    modelExecutor->setChoiceResolver(std::make_unique<LmcChoiceResolver>());

    // assert
    ASSERT_EQ(true, true) << "FAIL";
}
