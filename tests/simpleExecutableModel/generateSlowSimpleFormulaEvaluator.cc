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

#include <ThrowAssert.hpp>

#include "pemc/formula/formula.h"
#include "pemc/formula/formulaToStringVisitor.h"
#include "pemc/formula/adaptedFormula.h"
#include "pemc/formula/unaryFormula.h"
#include "pemc/formula/binaryFormula.h"
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/formula/boundedBinaryFormula.h"

#include "tests/simpleExecutableModel/simpleFormula.h"
#include "tests/simpleExecutableModel/generateSlowSimpleFormulaEvaluator.h"

namespace {
  using namespace pemc;
  using namespace pemc::simple;

  #include <string>
  #include <sstream>

  class SlowFormulaCompilationVisitor : public FormulaVisitor {
  private:
    SimpleModel* model;
  public:
    std::function<bool()> result;

    SlowFormulaCompilationVisitor(SimpleModel* _model);
    virtual ~SlowFormulaCompilationVisitor() = default;

    virtual void visitAdaptedFormula(AdaptedFormula* formula);
    virtual void visitUnaryFormula(UnaryFormula* formula);
    virtual void visitBinaryFormula(BinaryFormula* formula);
    virtual void visitBoundedUnaryFormula(BoundedUnaryFormula* formula);
    virtual void visitBoundedBinaryFormula(BoundedBinaryFormula* formula);
  };

  SlowFormulaCompilationVisitor::SlowFormulaCompilationVisitor(SimpleModel* _model)
    : FormulaVisitor(), model(_model) {
  }

  void SlowFormulaCompilationVisitor::visitAdaptedFormula(AdaptedFormula* formula) {
    auto simpleFormula = dynamic_cast<SimpleFormula*>(formula);
    throw_assert(simpleFormula!=nullptr, "Could not cast AdaptedFormula into a CppFormula");
    result = std::bind(simpleFormula->getEvaluator(), model);
  }

  void SlowFormulaCompilationVisitor::visitUnaryFormula(UnaryFormula* formula){
  	formula->getOperand()->visit(this);

		switch (formula->getOperator())
		{
			case UnaryOperator::Next:
			case UnaryOperator::Finally:
			case UnaryOperator::Globally:
      case UnaryOperator::All:
      case UnaryOperator::Exists:
      case UnaryOperator::Once: {
          auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
          throw_assert(false, error);
          break;
        }
			case UnaryOperator::Not: {
  				result = [ operand{std::move(result)} ] { return !operand(); };
  				break;
        }
			default: {
          auto error = "Unknown or unsupported unary operator '"+std::to_string(formula->getOperator())+"'.";
          throw_assert(false, error);
          break;
        }
		}
  }

  void SlowFormulaCompilationVisitor::visitBinaryFormula(BinaryFormula* formula){
    // https://stackoverflow.com/questions/8640393/move-capture-in-lambda
		formula->getLeftOperand()->visit(this);
    auto leftOperand = std::move(result);
    formula->getRightOperand()->visit(this);
    auto rightOperand = std::move(result);

		switch (formula->getOperator())
		{
			case BinaryOperator::And:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] {
           if (!leftOperand()) {
             return false;
           }
           return rightOperand();
        };
				break;
			case BinaryOperator::Or:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] {
           if (leftOperand()) {
             return true;
           }
           return rightOperand();
        };
				break;
			case BinaryOperator::Implication:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] {
           if (!leftOperand()) {
             return true;
           }
           return !rightOperand();
        };
				break;
			case BinaryOperator::Equivalence:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] {
           return leftOperand() == rightOperand();
        };
				break;
			case BinaryOperator::Until: {
          auto errorUntil = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
          throw_assert(false, errorUntil);
  				break;
        }
			default: {
          auto errorDefault = "Unknown or unsupported binary operator '"+std::to_string(formula->getOperator())+"'.";
  				throw_assert(false, errorDefault);
  				break;
        }
		}

  }

  void SlowFormulaCompilationVisitor::visitBoundedUnaryFormula(BoundedUnaryFormula* formula){
      auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
      throw_assert(false, error);
  }

  void SlowFormulaCompilationVisitor::visitBoundedBinaryFormula(BoundedBinaryFormula* formula){
    auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
    throw_assert(false, error);
  }
}
namespace pemc { namespace simple {
  std::function<bool()> generateSlowSimpleFormulaEvaluator(SimpleModel* model, Formula& formula) {
      auto compiler = SlowFormulaCompilationVisitor(model);
      formula.visit(&compiler);
      return std::move(compiler.result);
  }

} }
