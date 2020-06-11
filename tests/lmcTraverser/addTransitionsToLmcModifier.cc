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

#include "pemc/basic/configuration.h"
#include "pemc/basic/label.h"
#include "pemc/basic/model_capacity.h"
#include "pemc/basic/raw_memory.h"
#include "pemc/basic/tsc_index.h"
#include "pemc/executable_model/temporary_state_storage.h"
#include "pemc/formula/formula.h"
#include "pemc/generic_traverser/generic_traverser.h"
#include "pemc/generic_traverser/i_transitions_calculator.h"
#include "pemc/generic_traverser/traversal_transition.h"
#include "pemc/lmc/lmc.h"
#include "pemc/lmc_traverser/add_transitions_to_lmc_modifier.h"

using namespace pemc;

namespace {
using namespace pemc;

}

TEST(lmcTraverser_test, lmcTraverser_works) {
  auto configuration = Configuration();
  auto traverser = GenericTraverser(configuration);

  auto lmc = std::make_unique<Lmc>();
  lmc->initialize(*configuration.modelCapacity);

  // Set the labels of the Lmc.
  auto labelIdentifier = std::vector<std::string>{"f1", "f2"};
  lmc->setLabelIdentifier(labelIdentifier);

  auto modifier = std::make_unique<AddTransitionsToLmcModifier>(lmc.get());

  auto noState = std::optional<StateIndex>();
  auto state0 = std::make_optional(0);
  auto state1 = std::make_optional(1);
  auto transitions = std::vector<TraversalTransition>();
  auto transitionProbabilities = std::vector<Probability>();

  // act
  // add initial transitions
  transitions.clear();
  transitionProbabilities.clear();
  auto transition = TraversalTransition();
  transition.label = Label(std::vector<bool>{false, true});
  transition.targetStateIndex = 0;
  transitions.push_back(transition);
  transitionProbabilities.push_back(Probability::One());
  modifier->applyOnTransitions(
      noState, gsl::span<TraversalTransition>(transitions),
      static_cast<void*>(transitionProbabilities.data()));
  // add transitions of state 0
  transitions.clear();
  transitionProbabilities.clear();
  transition.targetStateIndex = 0;
  transitions.push_back(transition);
  transitionProbabilities.push_back(Probability(0.4));
  transition.targetStateIndex = 1;
  transitions.push_back(transition);
  transitionProbabilities.push_back(Probability(0.6));
  modifier->applyOnTransitions(
      state0, gsl::span<TraversalTransition>(transitions),
      static_cast<void*>(transitionProbabilities.data()));
  // add transitions of state 1
  transitions.clear();
  transitionProbabilities.clear();
  transition.targetStateIndex = 1;
  transitions.push_back(transition);
  transitionProbabilities.push_back(Probability::One());
  modifier->applyOnTransitions(
      state1, gsl::span<TraversalTransition>(transitions),
      static_cast<void*>(transitionProbabilities.data()));
  // finish
  lmc->finishCreation(2);

  // assert
  lmc->validate();
  ASSERT_EQ(lmc->getTransitions().size(), 4) << "FAIL";
  ASSERT_EQ(lmc->getInitialTransitions().size(), 1) << "FAIL";
}
