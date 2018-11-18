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

#include <limits>
#include <algorithm>
#include <ThrowAssert.hpp>

#include "pemc/basic/exceptions.h"
#include "pemc/executableModel/modelExecutor.h"

namespace {
  using namespace pemc;
}

namespace pemc {

  ModelExecutor::ModelExecutor(const Configuration& conf)
  : temporaryStateStorage(conf.successorCapacity)
  {
  }

  void ModelExecutor::setModel(std::unique_ptr<AbstractModel> _model) {
    model = std::move(_model);
    auto modelStateVectorSize = model->getStateVectorSize();
    temporaryStateStorage.setStateVectorSize(modelStateVectorSize, preStateStorageModifierStateVectorSize);
  }

  void ModelExecutor::setChoiceResolver(std::unique_ptr<IChoiceResolver> _choiceResolver) {
    choiceResolver = std::move(_choiceResolver);
    model->setChoiceResolver(choiceResolver.get());
  }

  int32_t ModelExecutor::getStateVectorSize() {
    return model->getStateVectorSize();
  }

  void ModelExecutor::setPreStateStorageModifierStateVectorSize(int32_t _preStateStorageModifierStateVectorSize){
    preStateStorageModifierStateVectorSize = _preStateStorageModifierStateVectorSize;
    if (model) {
      auto modelStateVectorSize = model->getStateVectorSize();
      temporaryStateStorage.setStateVectorSize(modelStateVectorSize, preStateStorageModifierStateVectorSize);
    }
  }

  void ModelExecutor::addTransition() {
    auto tempStateIndex = temporaryStateStorage.getFreshStateIndex();
    auto tempStateSpan = temporaryStateStorage[tempStateIndex];
    model->serialize(tempStateSpan);
    auto label = model->calculateLabel();
    auto newTransition = TraversalTransition(tempStateSpan.data(), label);
    transitions.push_back(newTransition);
  }

  gsl::span<TraversalTransition> ModelExecutor::calculateInitialTransitions() {
    transitions.clear();
		temporaryStateStorage.clear();

    choiceResolver->beginMacroStepExecution();

		while (choiceResolver->prepareNextPath()) {
			model->resetToInitialState();
			model->step();
			choiceResolver->stepFinished();
      addTransition();
		}

		choiceResolver->endMacroStepExecution();
    return gsl::span<TraversalTransition>(transitions);
  }

  gsl::span<TraversalTransition> ModelExecutor::calculateTransitionsOfState(gsl::span<gsl::byte> state) {
    transitions.clear();
		temporaryStateStorage.clear();

    choiceResolver->beginMacroStepExecution();

		while (choiceResolver->prepareNextPath()) {
			model->deserialize(state);
			model->step();
			choiceResolver->stepFinished();
      addTransition();
		}

		choiceResolver->endMacroStepExecution();
    return gsl::span<TraversalTransition>(transitions);
  }

  void* ModelExecutor::getCustomPayloadOfLastCalculation() {
    return choiceResolver->getCustomPayloadOfLastCalculation();
  }
}
