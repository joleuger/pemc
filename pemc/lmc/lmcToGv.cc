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

#include <gsl/gsl_util>
#include <string>

#include "pemc/basic/probability.h"
#include "pemc/lmc/lmcToGv.h"


namespace {

  using namespace pemc;

  void exportDistribution(Lmc& lmc, std::ostream& out,
    const std::string& sourceStateName, gsl::span<LmcTransitionEntry> transitions)
  {
    auto labels = lmc.getLabelIdentifier();

  	for (auto& transition : transitions)
  	{
  		out << sourceStateName
          << " -> "
          << transition.state
          << "[label=\""
          << prettyPrint(transition.probability)
          << std::endl;

  		for (gsl::index i = 0; i < labels.size(); ++i)
  		{
  			if (i==0)
  				out << "\\n";
  			else
  				out << ",";
  			if (transition.label[i])
  				out << "t";
  			else
  				out << "f";
  		}
  		out << "\"];" << std::endl;
  	}
  }
}

namespace pemc {

  void exportLmcToGv(Lmc& lmc, std::ostream& out)
  {
  	out << "digraph S {" << std::endl;
  	//out << "size = \"8,5\"");
  	out << "node [shape=box];" << std::endl;

  	auto initialStateName = "initialState";
  	out << " "
        << initialStateName
        << " [shape=point,width=0.0,height=0.0,label=\"\"];"
        << std::endl;
  	auto initialDistribution = lmc.getInitialTransitions();
  	exportDistribution(lmc, out, initialStateName, initialDistribution);

    auto states = lmc.getStates();

    for (gsl::index state = 0; state < states.size(); ++state)
    {
      auto stateString = std::to_string(state);
  		out << " "
          << stateString
          << "[label=\""
          << stateString
          << "\"];"
          << std::endl;

  		auto distribution = lmc.getTransitionsOfState(state);
  		exportDistribution(lmc, out, stateString, distribution);

  	}
  	out << "}" << std::endl;
  }

}
