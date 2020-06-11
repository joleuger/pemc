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

#include "pemc_cpp/generate_slow_cpp_formula_evaluator.h"

#include "pemc/basic/ThrowAssert.hpp"
#include "pemc/formula/adapted_formula.h"
#include "pemc/formula/binary_formula.h"
#include "pemc/formula/bounded_binary_formula.h"
#include "pemc/formula/bounded_unary_formula.h"
#include "pemc/formula/formula.h"
#include "pemc/formula/formula_to_string_visitor.h"
#include "pemc/formula/unary_formula.h"
#include "pemc_cpp/cpp_formula.h"

namespace {
using namespace pemc;
using namespace pemc::cpp;

#include <sstream>
#include <string>

class SlowFormulaCompilationVisitor : public FormulaVisitor {
 private:
  CppModel* model;

 public:
  std::function<bool()> result;

  SlowFormulaCompilationVisitor(CppModel* _model);
  virtual ~SlowFormulaCompilationVisitor() = default;

  virtual void visitAdaptedFormula(AdaptedFormula* formula);
  virtual void visitUnaryFormula(UnaryFormula* formula);
  virtual void visitBinaryFormula(BinaryFormula* formula);
  virtual void visitBoundedUnaryFormula(BoundedUnaryFormula* formula);
  virtual void visitBoundedBinaryFormula(BoundedBinaryFormula* formula);
};

SlowFormulaCompilationVisitor::SlowFormulaCompilationVisitor(CppModel* _model)
    : FormulaVisitor(), model(_model) {}

void SlowFormulaCompilationVisitor::visitAdaptedFormula(
    AdaptedFormula* formula) {
  auto cppFormula = dynamic_cast<CppFormula*>(formula);
  throw_assert(cppFormula != nullptr,
               "Could not cast AdaptedFormula into a CppFormula");
  result = std::bind(cppFormula->getEvaluator(), model);
}

void SlowFormulaCompilationVisitor::visitUnaryFormula(UnaryFormula* formula) {
  formula->getOperand()->visit(this);

  switch (formula->getOperator()) {
    case UnaryOperator::Next:
    case UnaryOperator::Finally:
    case UnaryOperator::Globally:
    case UnaryOperator::All:
    case UnaryOperator::Exists:
    case UnaryOperator::Once: {
      auto error =
          "Could not compile '" + std::to_string(formula->getOperator()) + "'.";
      throw_assert(false, error);
      break;
    }
    case UnaryOperator::Not: {
      result = [operand{std::move(result)}] { return !operand(); };
      break;
    }
    default: {
      auto error = "Unknown or unsupported unary operator '" +
                   std::to_string(formula->getOperator()) + "'.";
      throw_assert(false, error);
      break;
    }
  }
}

void SlowFormulaCompilationVisitor::visitBinaryFormula(BinaryFormula* formula) {
  // https://stackoverflow.com/questions/8640393/move-capture-in-lambda
  formula->getLeftOperand()->visit(this);
  auto leftOperand = std::move(result);
  formula->getRightOperand()->visit(this);
  auto rightOperand = std::move(result);

  switch (formula->getOperator()) {
    case BinaryOperator::And:
      result = [leftOperand{std::move(leftOperand)},
                rightOperand{std::move(rightOperand)}] {
        if (!leftOperand()) {
          return false;
        }
        return rightOperand();
      };
      break;
    case BinaryOperator::Or:
      result = [leftOperand{std::move(leftOperand)},
                rightOperand{std::move(rightOperand)}] {
        if (leftOperand()) {
          return true;
        }
        return rightOperand();
      };
      break;
    case BinaryOperator::Implication:
      result = [leftOperand{std::move(leftOperand)},
                rightOperand{std::move(rightOperand)}] {
        if (!leftOperand()) {
          return true;
        }
        return !rightOperand();
      };
      break;
    case BinaryOperator::Equivalence:
      result = [leftOperand{std::move(leftOperand)},
                rightOperand{std::move(rightOperand)}] {
        return leftOperand() == rightOperand();
      };
      break;
    case BinaryOperator::Until: {
      auto errorUntil =
          "Could not compile '" + std::to_string(formula->getOperator()) + "'.";
      throw_assert(false, errorUntil);
      break;
    }
    default: {
      auto errorDefault = "Unknown or unsupported binary operator '" +
                          std::to_string(formula->getOperator()) + "'.";
      throw_assert(false, errorDefault);
      break;
    }
  }
}

void SlowFormulaCompilationVisitor::visitBoundedUnaryFormula(
    BoundedUnaryFormula* formula) {
  auto error =
      "Could not compile '" + std::to_string(formula->getOperator()) + "'.";
  throw_assert(false, error);
}

void SlowFormulaCompilationVisitor::visitBoundedBinaryFormula(
    BoundedBinaryFormula* formula) {
  auto error =
      "Could not compile '" + std::to_string(formula->getOperator()) + "'.";
  throw_assert(false, error);
}
}  // namespace
namespace pemc {
namespace cpp {
std::function<bool()> generateSlowCppFormulaEvaluator(CppModel* model,
                                                      Formula* formula) {
  auto compiler = SlowFormulaCompilationVisitor(model);
  formula->visit(&compiler);
  return std::move(compiler.result);
}

}  // namespace cpp
}  // namespace pemc
