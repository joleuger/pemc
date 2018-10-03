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
#include "pemc/formula/formulaVisitor.h"
#include "pemc/formula/adaptedFormula.h"
#include "pemc/formula/unaryFormula.h"
#include "pemc/formula/binaryFormula.h"
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/formula/boundedBinaryFormula.h"
#include "pemc/formula/generateLabelBasedFormulaEvaluator.h"

namespace {
  using namespace pemc;

  #include <string>
  #include <sstream>


// Try to find the identifier of a formula in the set of label identifer to be able to evaluate,
// wheter a given label is satisfied.
// Works a bit different than the C# version StateFormulaSetEvaluatorCompilationVisitor.cs.
// This could be optimized by creating an optimized function object or even machine code with LLVM.
  class LabelBasedFormulaCompilationVisitor : public FormulaVisitor {
  private:
    gsl::span<std::string> labelIdentifier;

    bool tryToFindIdentifier(Formula* formula);

  public:
    std::function<bool(Label)> result;

    LabelBasedFormulaCompilationVisitor(gsl::span<std::string> _labelIdentifier);
    virtual ~LabelBasedFormulaCompilationVisitor() = default;

    virtual void visitAdaptedFormula(AdaptedFormula* formula);
    virtual void visitUnaryFormula(UnaryFormula* formula);
    virtual void visitBinaryFormula(BinaryFormula* formula);
    virtual void visitBoundedUnaryFormula(BoundedUnaryFormula* formula);
    virtual void visitBoundedBinaryFormula(BoundedBinaryFormula* formula);
  };

  LabelBasedFormulaCompilationVisitor::LabelBasedFormulaCompilationVisitor(gsl::span<std::string> _labelIdentifier)
    : FormulaVisitor(), labelIdentifier(_labelIdentifier) {
  }

  bool LabelBasedFormulaCompilationVisitor::tryToFindIdentifier(Formula* formula) {
      for (size_t i = 0; i < labelIdentifier.size(); i++) {
        if (labelIdentifier[i] == formula->getIdentifier()) {
			      result = [i](Label label){ return label[i]; };
            return true;
        }
      }
      return false;
		}

  void LabelBasedFormulaCompilationVisitor::visitAdaptedFormula(AdaptedFormula* formula) {
    if (tryToFindIdentifier(formula))
      return;
    throw_assert(false, "Could not compile " + formula->getIdentifier());
  }

  void LabelBasedFormulaCompilationVisitor::visitUnaryFormula(UnaryFormula* formula){
    if (tryToFindIdentifier(formula))
      return;

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
  				result = [ operand{std::move(result)} ] (Label label) { return !operand(label); };
  				break;
        }
			default: {
          auto error = "Unknown or unsupported unary operator '"+std::to_string(formula->getOperator())+"'.";
          throw_assert(false, error);
          break;
        }
		}
  }

  void LabelBasedFormulaCompilationVisitor::visitBinaryFormula(BinaryFormula* formula){
    if (tryToFindIdentifier(formula))
      return;

    // https://stackoverflow.com/questions/8640393/move-capture-in-lambda
		formula->getLeftOperand()->visit(this);
    auto leftOperand = std::move(result);
    formula->getRightOperand()->visit(this);
    auto rightOperand = std::move(result);

		switch (formula->getOperator())
		{
			case BinaryOperator::And:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] (Label label) {
           if (!leftOperand(label)) {
             return false;
           }
           return rightOperand(label);
        };
				break;
			case BinaryOperator::Or:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] (Label label) {
           if (leftOperand(label)) {
             return true;
           }
           return rightOperand(label);
        };
				break;
			case BinaryOperator::Implication:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] (Label label) {
           if (!leftOperand(label)) {
             return true;
           }
           return !rightOperand(label);
        };
				break;
			case BinaryOperator::Equivalence:
				result = [ leftOperand{std::move(leftOperand)}, rightOperand{std::move(rightOperand)} ] (Label label) {
           return leftOperand(label) == rightOperand(label);
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

  void LabelBasedFormulaCompilationVisitor::visitBoundedUnaryFormula(BoundedUnaryFormula* formula){
    if (tryToFindIdentifier(formula))
      return;

    auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
    throw_assert(false, error);
  }

  void LabelBasedFormulaCompilationVisitor::visitBoundedBinaryFormula(BoundedBinaryFormula* formula){
    if (tryToFindIdentifier(formula))
      return;

    auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
    throw_assert(false, error);
  }
}
namespace pemc {
  std::function<bool(Label)> generateLabelBasedFormulaEvaluator(gsl::span<std::string> labelIdentifier, Formula* formula) {
      auto compiler = LabelBasedFormulaCompilationVisitor(labelIdentifier);
      formula->visit(&compiler);
      return std::move(compiler.result);
  }

}
