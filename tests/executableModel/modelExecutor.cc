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

#include "pemc/executable_model/model_executor.h"
#include "pemc/lmc_traverser/lmc_choice_resolver.h"

#include "tests/simpleExecutableModel/simpleFormula.h"
#include "tests/simpleExecutableModel/simpleModel.h"

using namespace pemc;
using namespace pemc::simple;

namespace {
  using namespace pemc;

  class TestModel : public SimpleModel {
    virtual void step() {
      if (getState() == 0 ) {
        setState( choose( {3, 1} ) );
      } else if (getState() == 1 ) {
        setState(3);
      }
    }
  };

  auto f1 = std::make_shared<SimpleFormula>([](SimpleModel* model) { return model->getState() >= 2; }, "f1" );
  auto formulas = std::vector<std::shared_ptr<Formula>>( {f1} );

  auto sourceState = std::vector<gsl::byte>(4);
  auto sourceStatePtr = reinterpret_cast<int32_t*>(sourceState.data());
}

TEST(modelExecutor_test, modelExecutor_test) {
    auto configuration = Configuration();

    auto modelExecutor = std::make_unique<ModelExecutor>(configuration);
    auto model = std::make_unique<TestModel>();
    model->setFormulasForLabel(formulas);
    modelExecutor->setModel(std::move(model));
    modelExecutor->setChoiceResolver(std::make_unique<LmcChoiceResolver>());

    // act
    auto resultTransitions = modelExecutor->calculateInitialTransitions();
    auto resultPayload = reinterpret_cast<Probability*>(modelExecutor->getCustomPayloadOfLastCalculation());
    auto transition0 = resultTransitions[0];
    auto itarget_0 = *reinterpret_cast<int32_t*>(transition0.targetState);
    auto ilabel_0 = transition0.label;
    auto iprob_0 = resultPayload[0];
    auto transition1 = resultTransitions[1];
    auto itarget_1 = *reinterpret_cast<int32_t*>(transition1.targetState);
    auto ilabel_1 = transition1.label;
    auto iprob_1 = resultPayload[1];

    *sourceStatePtr = 1;
    resultTransitions = modelExecutor->calculateTransitionsOfState(sourceState);
    resultPayload = reinterpret_cast<Probability*>(modelExecutor->getCustomPayloadOfLastCalculation());
    transition0 = resultTransitions[0];
    auto target1 = *reinterpret_cast<int32_t*>(transition0.targetState);
    auto label1 = transition0.label;
    auto prob1 = resultPayload[0];

    *sourceStatePtr = 3;
    resultTransitions = modelExecutor->calculateTransitionsOfState(sourceState);
    resultPayload = reinterpret_cast<Probability*>(modelExecutor->getCustomPayloadOfLastCalculation());
    transition0 = resultTransitions[0];
    auto target3 = *reinterpret_cast<int32_t*>(transition0.targetState);
    auto label3 = transition0.label;
    auto prob3 = resultPayload[0];

    // assert
    ASSERT_EQ(itarget_0, 3) << "FAIL";
    ASSERT_EQ(ilabel_0[0], true) << "FAIL";
    ASSERT_EQ(probabilityIsAround(iprob_0, 0.5, 0.0001), true) << "FAIL";
    ASSERT_EQ(itarget_1, 1) << "FAIL";
    ASSERT_EQ(ilabel_1[0], false) << "FAIL";
    ASSERT_EQ(probabilityIsAround(iprob_1, 0.5, 0.0001), true) << "FAIL";

    ASSERT_EQ(target1, 3) << "FAIL";
    ASSERT_EQ(label1[0], true) << "FAIL";
    ASSERT_EQ(probabilityIsOne(prob1, 0.0001), true) << "FAIL";

    ASSERT_EQ(target3, 3) << "FAIL";
    ASSERT_EQ(label3[0], true) << "FAIL";
    ASSERT_EQ(probabilityIsOne(prob3, 0.0001), true) << "FAIL";
}
