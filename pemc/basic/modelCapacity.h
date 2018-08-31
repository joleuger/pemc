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

#ifndef PEMC_BASIC_MODELCAPACITY_H_
#define PEMC_BASIC_MODELCAPACITY_H_

#include <functional>
#include <iostream>
#include <tuple>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/label.h"

namespace pemc {

  class ModelCapacity {
  private:
    std::size_t stateSize = sizeof(StateIndex);
    std::size_t targetSize = sizeof(Label) + sizeof(StateIndex);
    std::size_t choiceSize = 0;
  public:
    virtual ~ModelCapacity() = default;

    void setStateSize(std::size_t _stateSize) {
      stateSize = _stateSize;
    }

    void setTargetSize(std::size_t _targetSize) {
      targetSize = _targetSize;
    }

    void setChoiceSize(std::size_t _choiceSize) {
      choiceSize = _choiceSize;
    }

    virtual StateIndex getMaximalStates() = 0;

    virtual TargetIndex getMaximalTargets() = 0;

    virtual ChoiceIndex getMaximalChoices () = 0;
  };

  class ModelCapacityByModelSize : public ModelCapacity {
  private:
      StateIndex maximalStates = 0;
      TargetIndex maximalTargets = 0;
      ChoiceIndex maximalChoices = 0;

  public:
    virtual ~ModelCapacityByModelSize() = default;

    void setMaximalStates(StateIndex _maximalStates) {
      maximalStates = _maximalStates;
    }

    void setMaximalTargets(TargetIndex _maximalTargets) {
      maximalTargets = _maximalTargets;
    }

    void setMaximalChoices(ChoiceIndex _maximalChoices) {
      maximalChoices = _maximalChoices;
    }

    virtual StateIndex getMaximalStates() {
      return maximalStates;
    };

    virtual TargetIndex getMaximalTargets() {
      // Remind: Transition = Target + Probability
      return maximalTargets;
    };

    virtual ChoiceIndex getMaximalChoices () {
      return maximalChoices;
    };

    static ModelCapacityByModelSize Small() {
      auto modelCapacity = ModelCapacityByModelSize();
      modelCapacity.setMaximalStates(1024);
      modelCapacity.setMaximalTargets(1024 << 2);
      modelCapacity.setMaximalChoices(1024 << 4);
      return modelCapacity;
    }

    static ModelCapacityByModelSize Normal() {
      auto modelCapacity = ModelCapacityByModelSize();
      modelCapacity.setMaximalStates(1 << 21);
      modelCapacity.setMaximalTargets(1 << 23);
      modelCapacity.setMaximalChoices(1 << 25);
      return modelCapacity;
    }

    static ModelCapacityByModelSize Large() {
      auto modelCapacity = ModelCapacityByModelSize();
      modelCapacity.setMaximalStates(1 << 26);
      modelCapacity.setMaximalTargets(1 << 28);
      modelCapacity.setMaximalChoices(1 << 30);
      return modelCapacity;
    }
  };


}
#endif  // PEMC_BASIC_MODELCAPACITY_H_
