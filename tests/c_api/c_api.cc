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

#include "pemc/c_api/c_api.h"

#include <gtest/gtest.h>

#include <iostream>
#include <thread>

typedef struct TestModel {
  int32_t state_serialized;
  int64_t state_temporary;
  pemc_model_specific_interface* pemc_interface;
} TestModel;

pemc_functions pemc_function_accessor;

void test_model_create(unsigned char** model,
                       pemc_model_specific_interface* _pemc_interface,
                       unsigned char* optional_value_for_model_create) {
  TestModel* testmodel = (TestModel*)malloc(sizeof(TestModel));
  testmodel->pemc_interface = _pemc_interface;
  *model = (unsigned char*)testmodel;
  bool optional_value_is_3 = (*optional_value_for_model_create) == 3;
  std::cout << "Optional parameter is 3? " << optional_value_is_3 << "\n";
  std::cout << "Test model created\n";
}

void test_model_free(unsigned char* model) {
  free(model);
  std::cout << "Test model destructed\n";
}

void test_model_serialize(unsigned char* model,
                          unsigned char* position,
                          size_t size) {
  // retrieve model
  TestModel* testmodel = (TestModel*)model;

  *(int32_t*)position = testmodel->state_serialized;  // copy value to position

  std::cout << "Test model serialized\n";
}

void test_model_deserialize(unsigned char* model,
                            unsigned char* position,
                            size_t size) {
  // retrieve model
  TestModel* testmodel = (TestModel*)model;

  int32_t state =
      *(int32_t*)position;  // copy value in postition to a temporary variable

  testmodel->state_serialized = state;  // copy value to model

  std::cout << "Test model deserialized\n";
}

void test_model_reset_to_initial_state(unsigned char* model) {
  // retrieve model
  TestModel* testmodel = (TestModel*)model;

  testmodel->state_serialized = 0;  // set initial value
  std::cout << "Test model reset\n";
}

void test_model_step(unsigned char* model) {
  // retrieve model
  std::cout << "Test model step\n";
  TestModel* testmodel = (TestModel*)model;

  if (testmodel->state_serialized == 0) {
    int32_t option = 0;
    option = testmodel->pemc_interface->pemc_choose_by_no_of_options(
        testmodel->pemc_interface, 2);
    if (option == 0) {
      int32_t options[] = {1};
      int numOptions = sizeof(options) / sizeof(options[0]);
      int32_t selected_option = pemc_function_accessor.pemc_choose_int_option(
          testmodel->pemc_interface, options, numOptions);
      testmodel->state_serialized = selected_option;
    }
    if (option == 1) {
      testmodel->state_serialized = 3;
    }
  } else if (testmodel->state_serialized == 1) {
    testmodel->state_serialized = 3;
  }
}

int32_t formula_f1(unsigned char* model) {
  // retrieve model
  TestModel* testmodel = (TestModel*)model;
  return testmodel->state_serialized == 3;
}

int32_t formula_f2(unsigned char* model) {
  // retrieve model
  TestModel* testmodel = (TestModel*)model;
  return testmodel->state_serialized == 4;
}

int32_t test_model_get_state_vector_size(unsigned char* model) {
  return sizeof(int32_t);
}

TEST(c_api_test, c_api_function_accessor_works) {
  // get pemc functions
  assign_pemc_functions(&pemc_function_accessor);

  ASSERT_EQ(
      pemc_function_accessor.check_reachability_in_executable_model != nullptr,
      true)
      << "FAIL";
}

TEST(c_api_test, c_api_check_reachability_in_executable_model_works) {
  // setup model functions
  pemc_model_functions model_functions;
  model_functions.model_create = (pemc_model_create)test_model_create;
  model_functions.model_free = (pemc_model_free)test_model_create;
  model_functions.serialize =
      (pemc_serialize_function_type)test_model_serialize;
  model_functions.deserialize =
      (pemc_deserialize_function_type)test_model_deserialize;
  model_functions.reset_to_initial_state =
      (pemc_reset_to_initial_state_function_type)
          test_model_reset_to_initial_state;
  model_functions.step = (pemc_step_function_type)test_model_step;
  model_functions.get_state_vector_size =
      (pemc_get_state_vector_size_function_type)
          test_model_get_state_vector_size;

  // get pemc functions
  assign_pemc_functions(&pemc_function_accessor);

  pemc_formula_ref* f1 =
      pemc_function_accessor.pemc_register_basic_formula(formula_f1);

  pemc_formula_ref* f2 =
      pemc_function_accessor.pemc_register_basic_formula(formula_f2);

  unsigned char optional_parameter = 3;

  auto result_f1 =
      pemc_function_accessor.check_reachability_in_executable_model(
          model_functions, &optional_parameter, f1);

  auto result_f2 =
      pemc_function_accessor.check_reachability_in_executable_model(
          model_functions, &optional_parameter, f2);

  pemc_function_accessor.pemc_unref_formula(f1);
  pemc_function_accessor.pemc_unref_formula(f2);

  ASSERT_EQ(result_f1, true) << "FAIL";
  ASSERT_EQ(result_f2, false) << "FAIL";
}

static int32_t dummy_choose_by_no_of_options(
    pemc_model_specific_interface* pemc_interface,
    int32_t no_of_options) {
  return *pemc_interface->model_internals;
}

TEST(c_api_test, c_api_check_pemc_choose_int_option_works) {
  pemc_model_specific_interface_struct pemc_interface;
  unsigned char selectedOption = 0;
  pemc_interface.model_internals = &selectedOption;
  pemc_interface.pemc_choose_by_no_of_options = &dummy_choose_by_no_of_options;

  int32_t options[] = {6, 456};
  int numOptions = sizeof(options) / sizeof(options[0]);
  auto result_option0 = pemc_function_accessor.pemc_choose_int_option(
      &pemc_interface, options, numOptions);
  selectedOption = 1;
  auto result_option1 = pemc_function_accessor.pemc_choose_int_option(
      &pemc_interface, options, numOptions);

  ASSERT_EQ(result_option0, 6) << "FAIL";
  ASSERT_EQ(result_option1, 456) << "FAIL";
}
