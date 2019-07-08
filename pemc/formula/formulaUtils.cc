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

#include "pemc/formula/unaryFormula.h"
#include "pemc/formula/binaryFormula.h"
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/formula/boundedBinaryFormula.h"
#include "pemc/formula/formulaToStringVisitor.h"

#include "pemc/formula/formulaUtils.h"

namespace pemc {

  stde::optional<std::tuple<Formula*,Formula*,stde::optional<int>>> tryExtractPhiUntilPsiWithBound(Formula& formula) {
    auto asUnaryFormula = dynamic_cast<UnaryFormula*>(&formula);
    if (asUnaryFormula!=nullptr && asUnaryFormula->getOperator()==UnaryOperator::Finally) {
      auto result = std::make_tuple( (Formula*) nullptr,asUnaryFormula->getOperand(), stde::optional<int>());
      return result;
    }

    auto asBinaryFormula = dynamic_cast<BinaryFormula*>(&formula);
    if (asBinaryFormula!=nullptr && asBinaryFormula->getOperator()==BinaryOperator::Until) {
      auto result =  std::make_tuple(asBinaryFormula->getLeftOperand(),asBinaryFormula->getRightOperand(), stde::optional<int>());
      return result;
    }

    auto asBoundedUnaryFormula = dynamic_cast<BoundedUnaryFormula*>(&formula);
    if (asBoundedUnaryFormula!=nullptr && asBoundedUnaryFormula->getOperator()==UnaryOperator::Finally) {
      auto result = std::make_tuple( (Formula*) nullptr,asBoundedUnaryFormula->getOperand(), stde::make_optional(asBoundedUnaryFormula->getBound()));
      return result;
    }

    auto asBoundedBinaryFormula = dynamic_cast<BoundedBinaryFormula*>(&formula);
    if (asBoundedBinaryFormula!=nullptr && asBoundedBinaryFormula->getOperator()==BinaryOperator::Until) {
      auto result =  std::make_tuple(asBoundedBinaryFormula->getLeftOperand(),asBoundedBinaryFormula->getRightOperand(), stde::make_optional(asBoundedBinaryFormula->getBound()));
      return result;
    }

    return stde::nullopt;
  }

  std::string formulaToString(Formula& formula) {
    FormulaToStringVisitor visitor;
    formula.visit(&visitor);
    return visitor.getResult();
  }

}
