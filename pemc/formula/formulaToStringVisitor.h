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

#ifndef PEMC_FORMULA_FORMULATOSTRINGVISITOR_H_
#define PEMC_FORMULA_FORMULATOSTRINGVISITOR_H_

#include <string>
#include <sstream>

#include "pemc/formula/formulaVisitor.h"

namespace pemc {

  class FormulaToStringVisitor : public FormulaVisitor {
  private:
    std::ostringstream output;
  public:
    FormulaToStringVisitor();
    virtual ~FormulaToStringVisitor() = default;

    std::string getResult();

    virtual void visitLabelFormula(LabelFormula* formula);
    virtual void visitUnaryFormula(UnaryFormula* formula);
    virtual void visitBinaryFormula(BinaryFormula* formula);
    virtual void visitBoundedUnaryFormula(BoundedUnaryFormula* formula);
    virtual void visitBoundedBinaryFormula(BoundedBinaryFormula* formula);
  };

}
#endif  // PEMC_FORMULA_FORMULATOSTRINGVISITOR_H_
