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
#include "pemc/formula/labelFormula.h"
#include "pemc/formula/unaryFormula.h"
#include "pemc/formula/binaryFormula.h"
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/formula/boundedBinaryFormula.h"

namespace {

  #include <string>
  #include <sstream>

  class SlowFormulaCompilationVisitor : public FormulaVisitor {
  private:
    std::function<bool()> result; // todo: vielleicht unique pointer
  public:
    SlowFormulaCompilationVisitor();
    virtual ~SlowFormulaCompilationVisitor() = default;

    std::string getResult();

    virtual void VisitLabelFormula(LabelFormula* formula);
    virtual void VisitUnaryFormula(UnaryFormula* formula);
    virtual void VisitBinaryFormula(BinaryFormula* formula);
    virtual void VisitBoundedUnaryFormula(BoundedUnaryFormula* formula);
    virtual void VisitBoundedBinaryFormula(BoundedBinaryFormula* formula);
  };

  SlowFormulaCompilationVisitor::SlowFormulaCompilationVisitor(){
  }

  std::function<bool()> FormulaToStringVisitor::getResult(){
    return result;
  }

  void SlowFormulaCompilationVisitor::VisitLabelFormula(LabelFormula* formula) {
    // TODO: cast to CppFormula and set result
  }

  void SlowFormulaCompilationVisitor::VisitUnaryFormula(UnaryFormula* formula){
		switch (formula->getOperator())
		{
			case UnaryOperator::Next:
			case UnaryOperator::Finally:
			case UnaryOperator::Globally:
      case UnaryOperator::All:
      case UnaryOperator::Exists:
      case UnaryOperator::Once:
        auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
        throw_assert(false, error);
        break;
			case UnaryOperator::Not:
				// TODO:  set result
				break;
			default:
        auto error = "Unknown or unsupported unary operator '"+std::to_string(formula->getOperator())+"'.";
        throw_assert(false, error);
				break;
		}

		formula->getOperand()->Visit(this);
  }

  void SlowFormulaCompilationVisitor::VisitBinaryFormula(BinaryFormula* formula){

		formula->getLeftOperand()->Visit(this);

		switch (formula->getOperator())
		{
			case BinaryOperator::And:
				// TODO:  set result
				break;
			case BinaryOperator::Or:
				// TODO:  set result
				break;
			case BinaryOperator::Implication:
				// TODO:  set result
				break;
			case BinaryOperator::Equivalence:
				// TODO:  set result
				break;
			case BinaryOperator::Until:
        auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
        throw_assert(false, error);
				break;
			default:
        auto error = "Unknown or unsupported binary operator '"+std::to_string(formula->getOperator())+"'.";
				throw_assert(false, error);
				break;
		}

		formula->getRightOperand()->Visit(this);
  }

  void SlowFormulaCompilationVisitor::VisitBoundedUnaryFormula(BoundedUnaryFormula* formula){
      auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
      throw_assert(false, error);
  }

  void SlowFormulaCompilationVisitor::VisitBoundedBinaryFormula(BoundedBinaryFormula* formula){
    auto error = "Could not compile '"+std::to_string(formula->getOperator())+"'.";
    throw_assert(false, error);
  }
}
namespace pemc { namespace cpp {
  /*
  std::function<bool()> generateSlowCppFormulaEvaluator(Model* model) {

  }
  */

} }
