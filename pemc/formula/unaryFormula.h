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

#ifndef PEMC_FORMULA_UNARYFORMULA_H_
#define PEMC_FORMULA_UNARYFORMULA_H_

#include "pemc/formula/formula.h"

namespace pemc {

  enum UnaryOperator {
    // Non-temporal operators
		Not,

		// Temporal Future operators
		Next,
		Finally,
		Globally,

		// Temporal Past operator (see dx.doi.org/10.1007/3-540-15648-8_16)
		Once,

		// Path operators
		All,
		Exists
  };

  class UnaryFormula : Formula {
  private:
    std::shared_ptr<Formula> operand;
    UnaryOperator unaryOperator;
  public:
    UnaryFormula(std::shared_ptr<Formula> _operand, UnaryOperator _unaryOperator, const std::string& _identifier = nullptr);
    virtual ~UnaryFormula() = default;

    Formula* getOperand();
    UnaryOperator getOperator();

    virtual void Visit(FormulaVisitor& visitor);

  };

}
#endif  // PEMC_FORMULA_UNARYFORMULA_H_
