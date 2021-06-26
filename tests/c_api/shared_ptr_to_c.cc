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

#include <gtest/gtest.h>

#include <iostream>
#include <memory>

// https://stackoverflow.com/questions/31063055/passing-a-shared-ptr-through-a-c-interface-that-takes-void

struct Formula {
  int somedata;
  Formula() { std::cout << "Formula::Formula() called\n"; }
  ~Formula() { std::cout << "Formula::~Formula() called\n"; }
};

typedef struct {
  size_t refs;
  unsigned char* formula;
} pemc_formula_ref;

TEST(c_api_test, struct_in_shared_ptr_shared_in_scope) {
  {
    std::shared_ptr<Formula> formula = std::make_shared<Formula>();
    std::cout << "Shared Formula created\n";
  }
  std::cout << "Scope closed\n";

  ASSERT_EQ(true, true) << "FAIL";
}

TEST(c_api_test, struct_in_shared_ptr_nested_in_scope) {
  std::unique_ptr<std::shared_ptr<Formula>> ptr;
  {
    std::shared_ptr<Formula> formula = std::make_shared<Formula>();
    std::cout << "Shared Formula created\n";
    ptr = std::make_unique<std::shared_ptr<Formula>>(formula);
    std::cout << "Unique pointer set\n";
  }
  std::cout << "Scope closed\n";

  ASSERT_EQ(true, true) << "FAIL";
}

TEST(c_api_test, struct_in_shared_ptr_released_unique_ptr) {
  std::unique_ptr<std::shared_ptr<Formula>> ptr;
  {
    std::shared_ptr<Formula> formula = std::make_shared<Formula>();
    std::cout << "Shared Formula created\n";
    ptr = std::make_unique<std::shared_ptr<Formula>>(formula);
    std::cout << "Unique pointer set\n";
    ptr.release();
    std::cout << "Unique pointer released\n";
  }
  std::cout << "Scope closed\n";

  // memory leak: Formula::~Formula() is never called

  ASSERT_EQ(true, true) << "FAIL";
}

TEST(c_api_test, struct_in_shared_ptr_dangling_ptr) {
  unsigned int* ptr;
  {
    std::shared_ptr<Formula> formula = std::make_shared<Formula>();
    std::cout << "Shared Formula created\n";
    ptr = reinterpret_cast<unsigned int*>(&formula);
    std::cout << "C pointer set\n";
  }
  std::cout << "Scope closed\n";

  // memory failure: ptr is dangling

  ASSERT_EQ(true, true) << "FAIL";
}

TEST(c_api_test, struct_in_shared_ptr_how_to_do_it) {
  std::unique_ptr<std::shared_ptr<Formula>> ptr;
  unsigned int* cptr;
  {
    std::shared_ptr<Formula> formula = std::make_shared<Formula>();
    std::cout << "Shared Formula created\n";
    ptr = std::make_unique<std::shared_ptr<Formula>>(formula);
    std::cout << "Unique pointer set\n";
    cptr = reinterpret_cast<unsigned int*>(ptr.release());
    std::cout << "Unique pointer released\n";
  }
  std::cout << "Scope closed\n";
  ptr = std::unique_ptr<std::shared_ptr<Formula>>(
      reinterpret_cast<std::shared_ptr<Formula>*>(cptr));

  std::cout << "Unique pointer set\n";

  ASSERT_EQ(true, true) << "FAIL";
}