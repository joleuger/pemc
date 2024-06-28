// The MIT License (MIT)
//
// Copyright (c) 2023, Johannes Leupolz
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

#include <stdio.h>
#include <stdlib.h>

#include "pemc_c/pemc_c.h"

pemc_functions pemc_function_accessor;

typedef struct {
  int32_t state_serialized;
  int64_t state_temporary;
  pemc_model_specific_interface* pemc_interface;
} DiceModel;

void dice_model_create(unsigned char** model,
                       pemc_model_specific_interface* _pemc_interface,
                       unsigned char* optional_value_for_model_create) {
  DiceModel* dicemodel = (DiceModel*)malloc(sizeof(DiceModel));
  dicemodel->pemc_interface = _pemc_interface;
  *model = (unsigned char*)dicemodel;
  printf("Dice model created\n");
}

void dice_model_free(unsigned char* model) {
  free(model);
  printf("Dice model destructed\n");
}

void dice_model_serialize(unsigned char* model,
                          unsigned char* position,
                          size_t size) {
  // retrieve model
  DiceModel* dicemodel = (DiceModel*)model;

  *(int32_t*)position = dicemodel->state_serialized;  // copy value to position

  printf("Dice model serialized: %d\n", dicemodel->state_serialized);
}

void dice_model_deserialize(unsigned char* model,
                            unsigned char* position,
                            size_t size) {
  // retrieve model
  DiceModel* dicemodel = (DiceModel*)model;

  int32_t state =
      *(int32_t*)position;  // copy value in position to a temporary variable

  dicemodel->state_serialized = state;  // copy value to model

  printf("Dice model deserialized: %d\n", dicemodel->state_serialized);
}

void dice_model_reset_to_initial_state(unsigned char* model) {
  // retrieve model
  DiceModel* dicemodel = (DiceModel*)model;

  dicemodel->state_serialized = 0;  // set initial value
  printf("Dice model reset\n");
}

void dice_model_step(unsigned char* model) {
  // retrieve model
  printf("Dice model step\n");
  DiceModel* dicemodel = (DiceModel*)model;
  printf("Before step %d\n", dicemodel->state_serialized);

  int32_t state = dicemodel->state_serialized;
  if (state == 0) {
    // either 1,2,3 or 3,4,5
    int32_t options[] = {123, 456};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  } else if (state == 123) {
    // either 1,2 or 3+rethrow
    int32_t options[] = {12, 3123};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  } else if (state == 12) {
    // either 1 or 2
    int32_t options[] = {1, 2};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  } else if (state == 3123) {
    // either 3 or rethrow (1,2,3)
    int32_t options[] = {3, 123};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  } else if (state == 456) {
    // either 4,5 or 6+rethrow
    int32_t options[] = {45, 6456};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  } else if (state == 45) {
    // either 4 or 5
    int32_t options[] = {4, 5};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  } else if (state == 6456) {
    // either 6 or rethrow (4,5,6)
    int32_t options[] = {6, 456};
    int numOptions = sizeof(options) / sizeof(options[0]);
    state = pemc_function_accessor.pemc_choose_int_option(
        dicemodel->pemc_interface, options, numOptions);
  }
  dicemodel->state_serialized = state;
}

int32_t dice_model_get_state_vector_size(unsigned char* model) {
  return sizeof(int32_t);
}

int32_t formula_f1(unsigned char* model) {
  // retrieve model
  DiceModel* dicemodel = (DiceModel*)model;
  return dicemodel->state_serialized == 3;
}

int32_t formula_f2(unsigned char* model) {
  // retrieve model
  DiceModel* dicemodel = (DiceModel*)model;
  return dicemodel->state_serialized == 7;
}

int main() {
  // setup model functions
  pemc_model_functions model_functions;
  model_functions.model_create = (pemc_model_create)dice_model_create;
  model_functions.model_free = (pemc_model_free)dice_model_free;
  model_functions.serialize =
      (pemc_serialize_function_type)dice_model_serialize;
  model_functions.deserialize =
      (pemc_deserialize_function_type)dice_model_deserialize;
  model_functions.reset_to_initial_state =
      (pemc_reset_to_initial_state_function_type)
          dice_model_reset_to_initial_state;
  model_functions.step = (pemc_step_function_type)dice_model_step;
  model_functions.get_state_vector_size =
      (pemc_get_state_vector_size_function_type)
          dice_model_get_state_vector_size;

  // get pemc functions
  assign_pemc_functions_from_dll(&pemc_function_accessor);

  pemc_formula_ref* f1 =
      pemc_function_accessor.pemc_register_basic_formula(formula_f1);

  pemc_formula_ref* f2 =
      pemc_function_accessor.pemc_register_basic_formula(formula_f2);

  int32_t number_3_is_reachable =
      pemc_function_accessor.check_reachability_in_executable_model(
          model_functions, NULL, f1);

  int32_t number_7_is_reachable =
      pemc_function_accessor.check_reachability_in_executable_model(
          model_functions, NULL, f2);

  pemc_formula_ref* formulas[] = {f1, f2};
  int formula_num = sizeof(formulas) / sizeof(formulas[0]);

  pemc_lmc_ref* lmc = pemc_function_accessor.build_lmc_from_executable_model(
      model_functions, NULL, formulas, formula_num);

  double probability_2steps =
      pemc_function_accessor.calculate_probability_to_reach_state_within_bound(
          lmc, f1, 2);
  double probability_4steps =
      pemc_function_accessor.calculate_probability_to_reach_state_within_bound(
          lmc, f1, 4);
  double probability_10steps =
      pemc_function_accessor.calculate_probability_to_reach_state_within_bound(
          lmc, f1, 10);
  double probability_20steps =
      pemc_function_accessor.calculate_probability_to_reach_state_within_bound(
          lmc, f1, 20);
  printf("The probability in 2 steps is %f\n", probability_2steps);
  printf("The probability in 4 steps is %f\n", probability_4steps);
  printf("The probability in 10 steps is %f\n", probability_10steps);
  printf("The probability in 20 steps is %f\n", probability_20steps);

  printf("3 is reachable %d\n", number_3_is_reachable);
  printf("7 is reachable %d\n", number_7_is_reachable);

  pemc_function_accessor.pemc_unref_formula(f1);
  pemc_function_accessor.pemc_unref_formula(f2);
}
