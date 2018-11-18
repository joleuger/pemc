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

#ifndef PEMC_LMCTRAVERSER_LMCCHOICERESOLVER_H_
#define PEMC_LMCTRAVERSER_LMCCHOICERESOLVER_H_

#include <vector>

#include "pemc/basic/probability.h"
#include "pemc/executableModel/IChoiceResolver.h"

namespace pemc {
  struct LmcChoiceStackEntry {
    Probability probability;
    size_t currentOption;
    size_t noOfOptions;
  };

  class LmcChoiceResolver : public IChoiceResolver {
  private:
      std::vector<LmcChoiceStackEntry> choiceStack;
      bool firstExecutionOfMacroStep;
      size_t choiceDepth = -1;
  public:
      LmcChoiceResolver();
      virtual ~LmcChoiceResolver() = default;

      std::vector<Probability> probabilities;

      virtual void beginMacroStepExecution();

      virtual bool prepareNextPath();

      virtual size_t choose(const gsl::span<Probability>& choices);

      virtual size_t choose(size_t numberOfChoices);

      virtual void stepFinished();

      virtual void endMacroStepExecution();

      virtual void* getCustomPayloadOfLastCalculation();
  };

}

#endif  // PEMC_LMCTRAVERSER_LMCCHOICERESOLVER_H_
