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

#include "pemc_cpp/cpp_formula.h"
#include "pemc_cpp/cpp_model.h"
#include "pemc_cpp/generate_slow_cpp_formula_evaluator.h"

using namespace pemc;
using namespace pemc::cpp;

namespace {

  class TestModel : public CppModel {
  public:
    int32_t state;

    virtual void serialize(gsl::span<gsl::byte> position);

    virtual void deserialize(gsl::span<gsl::byte> position);

    virtual void resetToInitialState();

    virtual void step();

    virtual int32_t getStateVectorSize();
  };

  void TestModel::serialize(gsl::span<gsl::byte> position) {
    auto ptrToState = reinterpret_cast<int32_t*>(position.data());
    *ptrToState = state;
  }

  void TestModel::deserialize(gsl::span<gsl::byte> position) {
    auto ptrToState = reinterpret_cast<int32_t*>(position.data());
    state = *ptrToState;
  }

  void TestModel::resetToInitialState() {
    state = 0;
  }

  void TestModel::step() {
    if (state == 0 ) {
      state =  choose( {3, 1} );
    } else if (state == 1 ) {
      state = 3;
    }
  }

  int32_t TestModel::getStateVectorSize() {
    return sizeof(int32_t);
  }

  auto f1 = std::make_shared<CppFormula>([](CppModel* model) {
      auto cppModel = static_cast<TestModel*>(model);
      return cppModel->state == 3;
    }, "f1" );

  auto formulas = std::vector<std::shared_ptr<Formula>>( {f1} );

}

TEST(pemcCpp_test, pemcCpp_test) {
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
