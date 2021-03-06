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

#ifndef TESTS_PEMC_LMC_LMC_H_
#define TESTS_PEMC_LMC_LMC_H_

#include "pemc/lmc/lmc.h"
#include "pemc/basic/label.h"
#include "pemc/formula/adapted_formula.h"
#include "pemc/formula/binary_formula.h"

using namespace pemc;

class LmcExample1 {
public:
  LmcExample1();

  Lmc lmc;
  std::shared_ptr<Formula> f1;
  std::shared_ptr<Formula> f2;

};

class LmcExample2 {
public:
  LmcExample2();

  Lmc lmc;
  std::shared_ptr<Formula> f1;
  std::shared_ptr<Formula> f2;

};


#endif  // TESTS_PEMC_LMC_LMC_H_
