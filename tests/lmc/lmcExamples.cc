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


#include "pemc/formula/adaptedFormula.h"
#include "pemc/formula/binaryFormula.h"

#include "tests/lmc/lmcExamples.h"

using namespace pemc;

namespace {
  using namespace pemc;

  auto false_false = std::vector<bool> { false, false };
  auto false_true = std::vector<bool> { false, true };
  auto true_false = std::vector<bool> { true, false };
  auto true_true = std::vector<bool> { true, true };

}

LmcExample1::LmcExample1()
  : lmc(Lmc()){
    // 0----> 1 ----> 2⟲ f2

  f1 = std::make_shared<AdaptedFormula>("f1");
  f2 = std::make_shared<AdaptedFormula>("f2");

  auto capacity = ModelCapacityByModelSize::Small();
  lmc.initialize(capacity);

  auto labelIdentifier = std::vector<std::string> {"f1", "f2"};
  lmc.setLabelIdentifier(labelIdentifier);

  auto locationOfFirstInitialEntry = lmc.getPlaceForNewInitialTransitionEntries(1);
  lmc.setLmcTransitionEntry(locationOfFirstInitialEntry,
    LmcTransitionEntry(Probability::One(), false_false, 0) );

  auto locationOfFirstEntryOfState0 = lmc.getPlaceForNewTransitionEntriesOfState(0,1);
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState0,
    LmcTransitionEntry(Probability::One(), false_false, 1) );

  auto locationOfFirstEntryOfState1 = lmc.getPlaceForNewTransitionEntriesOfState(1,1);
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState1,
    LmcTransitionEntry(Probability::One(), false_false, 2) );

  auto locationOfFirstEntryOfState2 = lmc.getPlaceForNewTransitionEntriesOfState(2,1);
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState2,
    LmcTransitionEntry(Probability::One(), true_true, 2) );

  auto noOfStates = 3;
  lmc.finishCreation(noOfStates);

}


LmcExample2::LmcExample2()
  : lmc(Lmc()){
  // 0---->
  //  -f1-> 1⟲
  //           ----> 2⟲
  //           -f2-> 2
  // 0-f2->          2

  f1 = std::make_shared<AdaptedFormula>("f1");
  f2 = std::make_shared<AdaptedFormula>("f2");

  auto capacity = ModelCapacityByModelSize::Small();
  lmc.initialize(capacity);

  auto labelIdentifier = std::vector<std::string> {"f1", "f2"};
  lmc.setLabelIdentifier(labelIdentifier);

  auto locationOfFirstInitialEntry = lmc.getPlaceForNewInitialTransitionEntries(1);
  lmc.setLmcTransitionEntry(locationOfFirstInitialEntry,
    LmcTransitionEntry(Probability::One(), false_false, 0) );

  auto locationOfFirstEntryOfState0 = lmc.getPlaceForNewTransitionEntriesOfState(0,3);
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState0,
    LmcTransitionEntry(Probability(0.6), false_false, 1) );
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState0+1,
    LmcTransitionEntry(Probability(0.3), true_false, 1) );
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState0+2,
    LmcTransitionEntry(Probability(0.1), false_true, 2) );

  auto locationOfFirstEntryOfState1 = lmc.getPlaceForNewTransitionEntriesOfState(1,3);
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState1,
    LmcTransitionEntry(Probability(0.9), false_false, 1) );
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState1+1,
    LmcTransitionEntry(Probability(0.01), false_false, 2) );
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState1+2,
    LmcTransitionEntry(Probability(0.09), false_true, 2) );

  auto locationOfFirstEntryOfState2 = lmc.getPlaceForNewTransitionEntriesOfState(2,1);
  lmc.setLmcTransitionEntry(locationOfFirstEntryOfState2,
    LmcTransitionEntry(Probability::One(), false_false, 2) );

  auto noOfStates = 3;
  lmc.finishCreation(noOfStates);

}
