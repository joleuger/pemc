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

#include <string>
#include <stdio.h>

#include "pemc/basic/probability.h"

namespace pemc {

  std::string prettyPrint(const Probability& probability) {
    // http://www.cplusplus.com/reference/cstdio/printf/

    char buffer [50];

    if (probability.value * 10.0 >= 1.0)
		{
      auto success = snprintf (buffer, sizeof(buffer), "%0.2f", probability.value);
      if (success>0)
        return std::string(buffer);
      else
        return std::string("probability.cc: failed");
		}
		auto newValue = probability.value;
		for (auto i = 0; i < 10; i++)
		{
			if (newValue >= 1.0)
			{
        auto success = snprintf (buffer, sizeof(buffer), "%0.2f", newValue);
        if (success>0)
          return std::string(buffer)+"×"+" 10E"+std::to_string(i);
  				// \uc397 == ×
        else
          return std::string("probability.cc: failed");
			}
			newValue = newValue * 10.0;
		}

    auto success = snprintf (buffer, sizeof(buffer), "%E", probability.value);
    if (success>0)
      return std::string(buffer);
    else
      return std::string("probability.cc: failed");
  }

  bool probabilityIsValid(const Probability& probability) {
    return probability.value > 0.0 && probability.value <= 1.0;
  }

  bool probabilityIsOne(double value, double tolerance) {
    return value >= 1.0 - tolerance && value <= 1.0 + tolerance;
  }

  bool probabilityIsAround(double realValue, double desiredValue, double tolerance) {
    return realValue >= desiredValue - tolerance && realValue <= desiredValue + tolerance;
  }

  bool probabilityIsOne(const Probability& value, double tolerance) {
    return probabilityIsOne(value.value, tolerance);
  }

  bool probabilityIsAround(const Probability& realValue, double desiredValue, double tolerance) {
    return probabilityIsAround(realValue.value, desiredValue, tolerance);
  }
}
