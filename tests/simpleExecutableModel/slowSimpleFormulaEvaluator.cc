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
#include <vector>

#include "pemc/basic/label.h"
#include "pemc/formula/adapted_formula.h"
#include "pemc/formula/binary_formula.h"

#include "tests/simpleExecutableModel/simpleFormula.h"
#include "tests/simpleExecutableModel/simpleModel.h"
#include "tests/simpleExecutableModel/generateSlowSimpleFormulaEvaluator.h"

using namespace pemc;
using namespace pemc::simple;

namespace {

  auto model = SimpleModel();

  auto f1 = std::make_shared<SimpleFormula>([](SimpleModel* model) { return model->getState() >= 0; }, "f1" );
  auto f2 = std::make_shared<SimpleFormula>([](SimpleModel* model) { return model->getState() == 1; }, "f2" );

  auto f1_and_f2 = std::make_shared<BinaryFormula>(f1,BinaryOperator::And,f2);

  auto labelIdentifier = std::vector<std::string> {"f1", "f2"};
}

TEST(formula_test, labelBasedFormulaEvaluatorOfBinaryFormula_SimpleModel) {

    auto modelEvaluator = generateSlowSimpleFormulaEvaluator(&model, f1_and_f2.get());

    auto result1 = modelEvaluator();
    model.setState(1);
    auto result2 = modelEvaluator();
    model.setState(2);
    auto result3 = modelEvaluator();
    model.setState(0);
    auto result4 = modelEvaluator();

    ASSERT_EQ(result1, false) << "FAIL";
    ASSERT_EQ(result2, true) << "FAIL";
    ASSERT_EQ(result3, false) << "FAIL";
    ASSERT_EQ(result4, false) << "FAIL";
}
