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
  if (state == 0) {
    // either 1,2,3 or 3,4,5
    state = choose({123, 456});
  } else if (state == 123) {
    // either 1,2 or 3+rethrow
    state = choose({12, 3123});
  } else if (state == 12) {
    // either 1 or 2
    state = choose({1, 2});
  } else if (state == 3123) {
    // either 3 or rethrow (1,2,3)
    state = choose({3, 123});
  } else if (state == 456) {
    // either 4,5 or 6+rethrow
    state = choose({45, 6456});
  } else if (state == 45) {
    // either 4 or 5
    state = choose({4, 5});
  } else if (state == 6456) {
    // either 6 or rethrow (4,5,6)
    state = choose({6, 456});
  }
}

int32_t TestModel::getStateVectorSize() {
  return sizeof(int32_t);
}

auto f1 = std::make_shared<CppFormula>(
    [](CppModel* model) {
      auto cppModel = static_cast<TestModel*>(model);
      return cppModel->state == 3;
    },
    "f1");

auto f2 = std::make_shared<CppFormula>(
    [](CppModel* model) {
      auto cppModel = static_cast<TestModel*>(model);
      return cppModel->state == 7;
    },
    "f2");

auto formulas = std::vector<std::shared_ptr<Formula>>({f1});

}  // namespace

int main() {
  auto configuration = Configuration();

  auto modelCreator = []() { return std::make_unique<TestModel>(); };

  auto pemc = Pemc(configuration);
  auto lmc = pemc.buildLmcFromExecutableModel(modelCreator, formulas);

  auto probability_2steps =
      pemc.calculateProbabilityToReachStateWithinBound(*lmc, f1, 2);
  auto probability_4steps =
      pemc.calculateProbabilityToReachStateWithinBound(*lmc, f1, 4);
  auto probability_10steps =
      pemc.calculateProbabilityToReachStateWithinBound(*lmc, f1, 10);
  auto probability_20steps =
      pemc.calculateProbabilityToReachStateWithinBound(*lmc, f1, 20);

  lmc->validate();

  std::cout << "The probability in 2 steps is " << probability_2steps.value
            << std::endl;
  std::cout << "The probability in 4 steps is " << probability_4steps.value
            << std::endl;
  std::cout << "The probability in 10 steps is " << probability_10steps.value
            << std::endl;
  std::cout << "The probability in 20 steps is " << probability_20steps.value
            << std::endl;

  auto number_3_is_reachable =
      pemc.checkReachabilityInExecutableModel(modelCreator, f1);
  auto number_7_is_reachable =
      pemc.checkReachabilityInExecutableModel(modelCreator, f2);

  std::cout << "3 is reachable " << number_3_is_reachable << std::endl;
  std::cout << "7 is reachable  " << number_7_is_reachable << std::endl;
}
