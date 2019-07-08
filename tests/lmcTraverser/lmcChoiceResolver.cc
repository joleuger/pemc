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

#include "pemc/lmc_traverser/lmc_choice_resolver.h"

using namespace pemc;

namespace {
  using namespace pemc;

}

TEST(lmcChoiceResolver_test, lmcChoiceResolver_noChoice) {
    auto choiceResolver = LmcChoiceResolver();

    // act
    choiceResolver.beginMacroStepExecution();
    auto prepare1 = choiceResolver.prepareNextPath();
    choiceResolver.stepFinished();
    auto prepare2 = choiceResolver.prepareNextPath();
    choiceResolver.endMacroStepExecution();
    auto customPayLoad = choiceResolver.getCustomPayloadOfLastCalculation();
    auto transitionProbabilities = reinterpret_cast<Probability*>(customPayLoad);

    // assert
    ASSERT_EQ(prepare1, true) << "FAIL";
    ASSERT_EQ(prepare2, false) << "FAIL";
    ASSERT_EQ(probabilityIsOne(transitionProbabilities[0],0.00001),true) << "FAIL";
}

TEST(lmcChoiceResolver_test, lmcChoiceResolver_OneChoice) {
    auto choiceResolver = LmcChoiceResolver();

    // act
    choiceResolver.beginMacroStepExecution();
    auto prepare1 = choiceResolver.prepareNextPath();
    auto chosen1 = choiceResolver.choose(2);
    choiceResolver.stepFinished();
    auto prepare2 = choiceResolver.prepareNextPath();
    auto chosen2 = choiceResolver.choose(2);
    choiceResolver.stepFinished();
    auto prepare3 = choiceResolver.prepareNextPath();
    choiceResolver.endMacroStepExecution();
    auto customPayLoad = choiceResolver.getCustomPayloadOfLastCalculation();
    auto transitionProbabilities = reinterpret_cast<Probability*>(customPayLoad);

    // assert
    ASSERT_EQ(prepare1, true) << "FAIL";
    ASSERT_EQ(chosen1, 0) << "FAIL";
    ASSERT_EQ(prepare2, true) << "FAIL";
    ASSERT_EQ(chosen2, 1) << "FAIL";
    ASSERT_EQ(prepare3, false) << "FAIL";
    ASSERT_EQ(probabilityIsAround(transitionProbabilities[0], 0.5, 0.00001),true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(transitionProbabilities[1], 0.5, 0.00001),true) << "FAIL";
}

TEST(lmcChoiceResolver_test, lmcChoiceResolver_TwoChoices) {
    auto choiceResolver = LmcChoiceResolver();

    // act
    choiceResolver.beginMacroStepExecution();
    auto prepare1 = choiceResolver.prepareNextPath();
    auto chosen1_1 = choiceResolver.choose(2);
    auto chosen1_2 = choiceResolver.choose(3);
    choiceResolver.stepFinished();
    auto prepare2 = choiceResolver.prepareNextPath();
    auto chosen2_1 = choiceResolver.choose(2);
    auto chosen2_2 = choiceResolver.choose(3);
    choiceResolver.stepFinished();
    auto prepare3 = choiceResolver.prepareNextPath();
    auto chosen3_1 = choiceResolver.choose(2);
    auto chosen3_2 = choiceResolver.choose(3);
    choiceResolver.stepFinished();
    auto prepare4 = choiceResolver.prepareNextPath();
    auto chosen4 = choiceResolver.choose(2);
    choiceResolver.stepFinished();
    auto prepare5 = choiceResolver.prepareNextPath();
    choiceResolver.endMacroStepExecution();
    auto customPayLoad = choiceResolver.getCustomPayloadOfLastCalculation();
    auto transitionProbabilities = reinterpret_cast<Probability*>(customPayLoad);

    // assert
    ASSERT_EQ(prepare1, true) << "FAIL";
    ASSERT_EQ(chosen1_1, 0) << "FAIL";
    ASSERT_EQ(chosen1_2, 0) << "FAIL";
    ASSERT_EQ(prepare2, true) << "FAIL";
    ASSERT_EQ(chosen2_1, 0) << "FAIL";
    ASSERT_EQ(chosen2_2, 1) << "FAIL";
    ASSERT_EQ(prepare3, true) << "FAIL";
    ASSERT_EQ(chosen3_1, 0) << "FAIL";
    ASSERT_EQ(chosen3_2, 2) << "FAIL";
    ASSERT_EQ(prepare4, true) << "FAIL";
    ASSERT_EQ(chosen4, 1) << "FAIL";
    ASSERT_EQ(prepare5, false) << "FAIL";
    ASSERT_EQ(probabilityIsAround(transitionProbabilities[0], 0.5 * 0.3333333, 0.00001),true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(transitionProbabilities[1], 0.5 * 0.3333333, 0.00001),true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(transitionProbabilities[2], 0.5 * 0.3333333, 0.00001),true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(transitionProbabilities[3], 0.5, 0.00001),true) << "FAIL";
}
