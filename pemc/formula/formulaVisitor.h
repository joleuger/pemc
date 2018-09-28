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

#ifndef PEMC_FORMULA_FORMULAVISITOR_H_
#define PEMC_FORMULA_FORMULAVISITOR_H_

#include "pemc/formula/formulaVisitor.h"

namespace pemc {

  class LabelFormula;
  class UnaryFormula;
  class BinaryFormula;
  class BoundedUnaryFormula;
  class BoundedBinaryFormula;

  class FormulaVisitor {
  public:
    virtual ~FormulaVisitor() = default;

    virtual void visitLabelFormula(LabelFormula* formula) = 0;
    virtual void visitUnaryFormula(UnaryFormula* formula) = 0;
    virtual void visitBinaryFormula(BinaryFormula* formula) = 0;
    virtual void visitBoundedUnaryFormula(BoundedUnaryFormula* formula) = 0;
    virtual void visitBoundedBinaryFormula(BoundedBinaryFormula* formula) = 0;
  };

}
#endif  // PEMC_FORMULA_FORMULAVISITOR_H_
