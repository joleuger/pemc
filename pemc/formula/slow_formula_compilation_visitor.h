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

#ifndef PEMC_FORMULA_SLOW_FORMULA_COMPILATION_VISITOR_H_
#define PEMC_FORMULA_SLOW_FORMULA_COMPILATION_VISITOR_H_

#include <functional>

#include "pemc/formula/adapted_formula.h"
#include "pemc/formula/binary_formula.h"
#include "pemc/formula/bounded_binary_formula.h"
#include "pemc/formula/bounded_unary_formula.h"
#include "pemc/formula/formula.h"
#include "pemc/formula/formula_to_string_visitor.h"
#include "pemc/formula/unary_formula.h"

namespace pemc {
using namespace pemc;

#include <sstream>
#include <string>

class SlowFormulaCompilationVisitor : public FormulaVisitor {
 public:
  std::function<bool()> result;

  virtual ~SlowFormulaCompilationVisitor() = default;

  virtual void visitUnaryFormula(UnaryFormula* formula);
  virtual void visitBinaryFormula(BinaryFormula* formula);
  virtual void visitBoundedUnaryFormula(BoundedUnaryFormula* formula);
  virtual void visitBoundedBinaryFormula(BoundedBinaryFormula* formula);
};
}  // namespace pemc
#endif  // PEMC_FORMULA_SLOW_FORMULA_COMPILATION_VISITOR_H_
