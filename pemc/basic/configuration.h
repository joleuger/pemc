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

#ifndef PEMC_BASIC_CONFIGURATION_H_
#define PEMC_BASIC_CONFIGURATION_H_

#include <functional>
#include <iostream>

#include "pemc/basic/modelCapacity.h"

namespace pemc {

  struct Configuration {

    // Output stream to write output to.
    // Note: Memory of cout is not managed. If memory management is required,
    // then let coutManaged handle the memory management of the class and
    // forward cout to coutManaged.get().
    std::ostream* cout = &std::cout;
    std::shared_ptr<std::ostream> coutManaged = nullptr;

    bool printGraphViz = false;

    int32_t maximalSearchDepth = 1 << 20;

    int32_t successorCapacity = 1 << 14;

    std::shared_ptr<ModelCapacity> modelCapacity = std::make_shared<ModelCapacityByModelSize>(ModelCapacityByModelSize::Small());
  };

}
#endif  // PEMC_BASIC_CONFIGURATION_H_
