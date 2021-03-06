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

#include "pemc/formula/adapted_formula.h"
#include "pemc/formula/binary_formula.h"
#include "pemc/formula/formula_utils.h"

using namespace pemc;


TEST(formula_test, formulaToStringOfBinaryFormula) {
    auto f1 = std::make_shared<AdaptedFormula>("f1");
    auto f2 = std::make_shared<AdaptedFormula>("f2");

    auto f1_and_f2 = std::make_shared<BinaryFormula>(f1,BinaryOperator::And,f2);
    auto formulaString = formulaToString(*f1_and_f2);

    std::cout << "Formula: " << formulaString << std::endl;

    ASSERT_EQ(formulaString, "(f1 && f2)") << "FAIL";
}
