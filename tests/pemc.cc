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
#include "pemc/pemc.h"

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

TEST(pemc_test, pemc_test) {
    auto configuration = Configuration();

    auto modelCreator = [](){ return std::make_unique<TestModel>(); };

    auto pemc = Pemc(configuration);
    auto lmc = pemc.buildLmcFromExecutableModel(modelCreator, formulas);

    auto probability1 = pemc.calculateProbabilityToReachStateWithinBound(*lmc, f1, 0);
    auto probability2 = pemc.calculateProbabilityToReachStateWithinBound(*lmc, f1, 1);

    lmc->validate();

    ASSERT_EQ(lmc->getStates().size(), 2) << "FAIL";
    ASSERT_EQ(probabilityIsAround(probability1, 0.5, 0.0001), true) << "FAIL";
    ASSERT_EQ(probabilityIsOne(probability2, 0.0001), true) << "FAIL";

}
