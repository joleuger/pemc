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

#include "pemc/formula/formula.h"
#include "pemc/formula/formulaToStringVisitor.h"
#include "pemc/formula/labelFormula.h"
#include "pemc/formula/unaryFormula.h"
#include "pemc/formula/binaryFormula.h"
#include "pemc/formula/boundedUnaryFormula.h"
#include "pemc/formula/boundedBinaryFormula.h"

namespace pemc {

  FormulaToStringVisitor::FormulaToStringVisitor(){
  }

  void FormulaToStringVisitor::VisitLabelFormula(LabelFormula* formula) {

  }

  void FormulaToStringVisitor::VisitUnaryFormula(UnaryFormula* formula){

  }

  void FormulaToStringVisitor::VisitBinaryFormula(BinaryFormula* formula){

  }

  void FormulaToStringVisitor::VisitBoundedUnaryFormula(BoundedUnaryFormula* formula){

  }

  void FormulaToStringVisitor::VisitBoundedBinaryFormula(BoundedBinaryFormula* formula){

  }
}