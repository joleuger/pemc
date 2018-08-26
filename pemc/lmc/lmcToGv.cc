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

#include "pemc/lmc/lmcToGv.h"


namespace {

  using namespace pemc;

  void ExportDistribution(const Lmc& lmc, const std::ostream& out,
    const std::string& sourceStateName,
    LabeledTransitionMarkovChain.LabeledTransitionEnumerator distribution)
  {
  	while (distribution.MoveNext())
  	{
  		out << $"{sourceStateName} -> {distribution.CurrentTargetState} [label=\"{Probability.PrettyPrint(distribution.CurrentProbability)}" << std::endl;

  		for (int i = 0; i < markovChain.StateFormulaLabels.Length; i++)
  		{
  			if (i==0)
  				out << "\\n";
  			else
  				out << ",";
  			if (distribution.CurrentFormulas[i])
  				out << "t";
  			else
  				out << "f";
  		}
  		out << "\"];" << std::endl;
  	}
  }
}

namespace pemc {

  public void ExportToGv(const Lmc& lmc, const std::ostream& out)
  {
  	out << "digraph S {" << std::endl;
  	//out << "size = \"8,5\"");
  	out << "node [shape=box];" << std::endl;

  	var initialStateName = "initialState";
  	out << " {initialStateName} [shape=point,width=0.0,height=0.0,label=\"\"];" << std::endl;
  	var initialDistribution = markovChain.GetInitialDistributionEnumerator();
  	ExportDistribution(markovChain, sb, initialStateName, initialDistribution);

  	foreach (var state in markovChain.SourceStates)
  	{

  		out << " {state} [label=\"{state}";
  		out << "\"];" << std::endl;

  		var distribution = markovChain.GetTransitionEnumerator(state);
  		ExportDistribution(markovChain, sb, state.ToString(), distribution);

  	}
  	out << "}");
  }

}
