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

#include "pemc/formula/binaryFormula.h"
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/lmc/lmcModelChecker.h"

#include "tests/lmc/lmcExamples.h"

using namespace pemc;


TEST(lmcModelChecker_test, check_simple_formula_1) {
    auto example = LmcExample1();
    auto& lmc = example.lmc;

    auto finally_f2 = std::make_shared<BoundedUnaryFormula>(example.f2,UnaryOperator::Finally,15);

    auto configuration = Configuration();
    auto mc = LmcModelChecker(lmc, configuration);
    auto probability = mc.calculateProbability(*finally_f2);

    std::cout << "Probability is " << prettyPrint(probability) << std::endl;

    ASSERT_EQ(probabilityIsOne(probability,0.000001), true) << "FAIL";
}


TEST(lmcModelChecker_test, check_simple_formula_2) {
    auto example = LmcExample2();
    auto& lmc = example.lmc;

    auto configuration = Configuration();
    auto mc = LmcModelChecker(lmc, configuration);

    auto finally_f2_in_i = [&](int i) {
      auto formula = std::make_shared<BoundedUnaryFormula>(example.f2,UnaryOperator::Finally,i);
      return mc.calculateProbability(*formula);
    };

    auto results = std::vector<Probability>(15);
    for (auto i=0; i < 15; i++) {
      results[i]=finally_f2_in_i(i);
      std::cout << "Probability for " << i << " steps is "
        << prettyPrint(results[i]) << std::endl;
    }

    auto result200=finally_f2_in_i(200);
    std::cout << "Probability for 200 steps is " << prettyPrint(result200) << std::endl;

    ASSERT_EQ(probabilityIsAround(results[0], 0.0, 0.000001), true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(results[1], 0.1, 0.000001), true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(results[2], 0.1 + 0.9*0.09, 0.000001), true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(results[3], 0.1 + 0.9*0.09 + 0.9*0.9*0.09, 0.000001), true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(results[4], 0.1 + 0.9*0.09 + 0.9*0.9*0.09 + 0.9*0.9*0.9*0.09, 0.000001), true) << "FAIL";

    ASSERT_EQ(probabilityIsAround(result200, 0.91, 0.000001), true) << "FAIL";
}
