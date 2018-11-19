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

#ifndef PEMC_SIMPLEEXECUTABLEMODEL_SIMPLEFMODEL_H_
#define PEMC_SIMPLEEXECUTABLEMODEL_SIMPLEFMODEL_H_

#include "pemc/executableModel/abstractModel.h"

namespace pemc { namespace simple {

  using namespace pemc;

  class SimpleModel : public AbstractModel {
  private:
    int32_t state;
  public:

    virtual void serialize(gsl::span<gsl::byte> position);

    virtual void deserialize(gsl::span<gsl::byte> position);

    virtual void setFormulasForLabel(const std::vector<std::shared_ptr<Formula>>& _formulas);

    virtual void resetToInitialState();

    virtual void step();

    virtual int32_t getStateVectorSize();

    int32_t getState();

    void setState(int32_t _state);

    template<typename T>
    std::tuple<Probability,T> choose(std::initializer_list<std::tuple<Probability,T>> choices) {
      // This is a Member template and the implementation must stay therefore in the header.
      auto probabilities = std::vector<Probability>();
      probabilities.reserve(choices.size());
      std::transform(choices.begin(), choices.end(), std::back_inserter(probabilities),
        [](auto& choice){ return std::get<0>(choice);} );
      auto chosenIdx = choiceResolver->choose(probabilities);
      return choices.begin()[chosenIdx];
    }

    template<typename T>
    T choose(std::initializer_list<T> choices) {
      // This is a Member template and the implementation must stay therefore in the header.
      auto chosenIdx = choiceResolver->choose(choices.size());
      return choices.begin()[chosenIdx];
    }
  };

} }
#endif  // PEMC_SIMPLEEXECUTABLEMODEL_SIMPLEFMODEL_H_
