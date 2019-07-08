// SPDX-License-Identifier: MIT
// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2019, Johannes Leupolz
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

#ifndef PEMC_EXECUTABLE_MODEL_I_CHOICE_RESOLVER_H_
#define PEMC_EXECUTABLE_MODEL_I_CHOICE_RESOLVER_H_

#include <gsl/span>

#include "pemc/basic/probability.h"


namespace pemc {
  class IChoiceResolver {
  private:
  public:
      IChoiceResolver() = default;
      virtual ~IChoiceResolver() = default;

      virtual void beginMacroStepExecution() {}

      virtual bool prepareNextPath() {return false;}

      virtual size_t choose(const gsl::span<Probability>& choices) { return 0;}

      virtual size_t choose(size_t numberOfChoices) { return 0;}

      virtual void stepFinished() {}

      virtual void endMacroStepExecution() {}

      virtual void* getCustomPayloadOfLastCalculation() {return nullptr;}
  };

}

#endif  // PEMC_EXECUTABLE_MODEL_I_CHOICE_RESOLVER_H_
