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
#include "pemc/formula/slow_formula_compilation_visitor.h"
#include "pemc/formula/unary_formula.h"
#include "pemc_cpp/cpp_formula.h"

namespace {
using namespace pemc;
using namespace pemc::cpp;

#include <sstream>
#include <string>

class SlowCppFormulaCompilationVisitor : public SlowFormulaCompilationVisitor {
 private:
  CppModel* model;

 public:
  std::function<bool()> result;

  SlowCppFormulaCompilationVisitor(CppModel* _model);
  virtual ~SlowCppFormulaCompilationVisitor() = default;

  virtual void visitAdaptedFormula(AdaptedFormula* formula);
};

SlowCppFormulaCompilationVisitor::SlowCppFormulaCompilationVisitor(
    CppModel* _model)
    : SlowFormulaCompilationVisitor(), model(_model) {}

void SlowCppFormulaCompilationVisitor::visitAdaptedFormula(
    AdaptedFormula* formula) {
  auto cppFormula = dynamic_cast<CppFormula*>(formula);
  throw_assert(cppFormula != nullptr,
               "Could not cast AdaptedFormula into a CppFormula");
  result = std::bind(cppFormula->getEvaluator(), model);
}

}  // namespace
namespace pemc {
namespace cpp {
std::function<bool()> generateSlowCppFormulaEvaluator(CppModel* model,
                                                      Formula* formula) {
  auto compiler = SlowCppFormulaCompilationVisitor(model);
  formula->visit(&compiler);
  return std::move(compiler.result);
}

}  // namespace cpp
}  // namespace pemc
