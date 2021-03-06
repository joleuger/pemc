// SPDX-License-Identifier: MIT
// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2019, Johannes Leupolz
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

#ifndef PEMC_BASIC_EXCEPTIONS_H_
#define PEMC_BASIC_EXCEPTIONS_H_

#include <exception>
#include <string>

namespace pemc {

class OutOfMemoryException : public std::exception {
 private:
  std::string completeMessage;

 public:
  OutOfMemoryException(const std::string& message) {
    completeMessage = "OutOfMemoryException: " + message;
  }
  ~OutOfMemoryException() = default;

  virtual const char* what() const throw() { return completeMessage.c_str(); }
};

class NotImplementedYetException : public std::exception {
 public:
  NotImplementedYetException() = default;
  ~NotImplementedYetException() = default;

  virtual const char* what() const throw() {
    return "NotImplementedYetException";
  }
};

class NotTestedYetException : public std::exception {
 public:
  NotTestedYetException() = default;
  ~NotTestedYetException() = default;

  virtual const char* what() const throw() { return "NotTestedYetException"; }
};

}  // namespace pemc
#endif  // PEMC_BASIC_EXCEPTIONS_H_
