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

#ifndef PEMC_LCMDP_LCMDP_H_
#define PEMC_LCMDP_LCMDP_H_

#include <vector>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/probability.h"
#include "pemc/basic/label.h"

namespace pemc {

  struct LcmdpStateEntry { TransitionIndex from; int elements; };

  struct LcmdpTransitionEntry { Probability probability; Label label; StateIndex state; };

  class Lcmdp {
  public:
      int initialTransitionFrom = 0;
      int initialTransitionCount = 0;
      std::vector<LcmdpStateEntry> states;
      std::vector<LcmdpTransitionEntry> transitions;

      Lcmdp();
  };

}

#endif  // PEMC_LCMDP_LCMDP_H_
