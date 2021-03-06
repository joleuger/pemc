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

#include "pemc/lmc/lmc.h"

#include "tests/lmc/lmcExamples.h"

using namespace pemc;

namespace {
  using namespace pemc;

}

TEST(lmc_test, lmc_is_constructed) {
    Lmc lmc;

    ASSERT_EQ(lmc.getStates().size(), 0) << "FAIL";
    ASSERT_EQ(lmc.getTransitions().size(), 0) << "FAIL";
}

TEST(lmc_test, lmc_createLabelBasedFormulaEvaluator_works) {
    LmcExample1 example{};
    auto& lmc = example.lmc;

    auto f1_and_f2 = std::make_shared<BinaryFormula>(example.f1,BinaryOperator::And,example.f2);
    auto evaluator = lmc.createLabelBasedFormulaEvaluator(f1_and_f2.get());

    TransitionIndex beginOf2, endOf2 = 0;
    std::tie(beginOf2, endOf2) = lmc.getTransitionIndexesOfState(2);

    auto resultOfFirstTransitionOfState3 = evaluator(beginOf2);

    ASSERT_EQ(resultOfFirstTransitionOfState3, true) << "FAIL";
}
