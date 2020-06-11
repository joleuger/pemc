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

#include "pemc/lmc_traverser/lmc_choice_resolver.h"

#include "pemc/basic/ThrowAssert.hpp"
#include "pemc/basic/exceptions.h"

// For the description, see chapter 7.3 of the thesis "Safety Analysis of
// Executable Models".

namespace pemc {

LmcChoiceResolver::LmcChoiceResolver() {}

void LmcChoiceResolver::beginMacroStepExecution() {
  firstExecutionOfMacroStep = true;
  probabilities.clear();
}

bool LmcChoiceResolver::prepareNextPath() {
  throw_assert(choiceDepth == choiceStack.size() - 1,
               "Something strange occured somewhere");

  choiceDepth = -1;

  if (firstExecutionOfMacroStep) {
    firstExecutionOfMacroStep = false;
    return true;
  }

  while (choiceStack.size() > 0) {
    auto lastEntry = choiceStack.back();
    choiceStack.pop_back();
    if (lastEntry.noOfOptions > lastEntry.currentOption + 1) {
      lastEntry.currentOption++;
      choiceStack.push_back(lastEntry);
      return true;
    }
  }
  return false;
}

Probability LmcChoiceResolver::getPreviousProbability() {
  auto previousProbability = Probability::One();
  if (choiceDepth >= 0 && choiceStack.size() >= choiceDepth + 1) {
    previousProbability = choiceStack[choiceDepth].probability;
  }
  return previousProbability;
}

size_t LmcChoiceResolver::choose(const gsl::span<Probability>& choices) {
  auto previousProbability = getPreviousProbability();
  choiceDepth++;
  if (choiceDepth < choiceStack.size()) {
    auto idx = choiceStack[choiceDepth].currentOption;
    if (choiceDepth == choiceStack.size() - 1) {
      choiceStack[choiceDepth].probability = choices[idx] * previousProbability;
    }
    return idx;
  }
  auto valueCount = choices.size();
  auto newChoiceEntry = LmcChoiceStackEntry();
  newChoiceEntry.probability = choices[0] * previousProbability;
  newChoiceEntry.currentOption = 0;
  newChoiceEntry.noOfOptions = valueCount;
  choiceStack.push_back(newChoiceEntry);
  return 0;
}

size_t LmcChoiceResolver::choose(size_t numberOfChoices) {
  auto previousProbability = getPreviousProbability();
  choiceDepth++;
  if (choiceDepth < choiceStack.size()) {
    auto idx = choiceStack[choiceDepth].currentOption;
    if (choiceDepth == choiceStack.size() - 1) {
      choiceStack[choiceDepth].probability =
          Probability(1.0 / numberOfChoices) * previousProbability;
    }
    return idx;
  }
  auto valueCount = numberOfChoices;
  auto newChoiceEntry = LmcChoiceStackEntry();
  newChoiceEntry.probability =
      Probability(1.0 / numberOfChoices) * previousProbability;
  newChoiceEntry.currentOption = 0;
  newChoiceEntry.noOfOptions = valueCount;
  choiceStack.push_back(newChoiceEntry);
  return 0;
}

void LmcChoiceResolver::stepFinished() {
  auto probability = Probability::One();
  if (choiceStack.size() > 0) {
    probability = choiceStack.back().probability;
  }

  probabilities.push_back(probability);
}

void LmcChoiceResolver::endMacroStepExecution() {}

void* LmcChoiceResolver::getCustomPayloadOfLastCalculation() {
  return static_cast<void*>(probabilities.data());
}

}  // namespace pemc
