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

#ifndef PEMC_BASIC_PROBABILITY_H_
#define PEMC_BASIC_PROBABILITY_H_

namespace pemc {

  struct Probability {
    Probability() = default;

    Probability(double _value) {
      value=_value;
    }

    double value;

    friend bool operator<(const Probability& l, const Probability& r) {
        return l.value < r.value;
    }

    friend bool operator>(const Probability& l, const Probability& r) {
        return l.value > r.value;
    }

    friend bool operator<=(const Probability& l, const Probability& r) {
        return l.value <= r.value;
    }

    friend bool operator>=(const Probability& l, const Probability& r) {
        return l.value >= r.value;
    }

    friend Probability operator+(const Probability& l, const Probability& r) {
        return Probability(l.value + r.value);
    }

    friend Probability& operator+=(Probability& l, const Probability& r) {
        l.value += r.value;
        return l;
    }

    friend Probability operator*(const Probability& l, const Probability& r) {
        return Probability(l.value * r.value);
    }

    static Probability One() {
      return Probability(1.0);
    }

    static Probability Zero() {
      return Probability(0.0);
    }

    static Probability Error() {
      return Probability(std::numeric_limits<float>::quiet_NaN());
    }
  };

  std::string prettyPrint(const Probability& probability);

  bool probabilityIsValid(const Probability& probability);

  bool probabilityIsOne(double value, double tolerance);

  bool probabilityIsAround(double realValue, double desiredValue, double tolerance);

  bool probabilityIsOne(const Probability& value, double tolerance);

  bool probabilityIsAround(const Probability& realValue, double desiredValue, double tolerance);
}

#endif  // PEMC_BASIC_PROBABILITY_H_
