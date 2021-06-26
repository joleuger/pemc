// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2020, Johannes Leupolz
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

#ifndef PEMC_C_API_C_API_H_
#define PEMC_C_API_C_API_H_

// This file is designed to be standalone and does not depend on the rest of
// pemc. Thus, it can be included in external programs without introducing any
// direct dependency on pemc.
// CApi is build as part of the shared library.

#include <stddef.h>
#include <stdint.h>

#include "pemc/basic/dll_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

// function pointer for model functions

typedef void (*pemc_serialize_function_type)(unsigned char*,  // position
                                             size_t);         // size

typedef void (*pemc_deserialize_function_type)(unsigned char*,  // position
                                               size_t);         // size

typedef void (*pemc_reset_to_initial_state_function_type)(unsigned char*);

typedef void (*pemc_step_function_type)(unsigned char*);

typedef int32_t (*pemc_get_state_vector_size_function_type)(unsigned char*);

typedef int32_t (*pemc_get_state_vector_size_function_type)(unsigned char*);

typedef int32_t (*pemc_model_create)(unsigned char**);

typedef int32_t (*pemc_model_free)(unsigned char*);

typedef struct {
  pemc_model_create model_create;
  pemc_model_free model_free;
  pemc_serialize_function_type serialize;
  pemc_deserialize_function_type deserialize;
  pemc_reset_to_initial_state_function_type reset_to_initial_state;
  pemc_step_function_type step;
  pemc_get_state_vector_size_function_type get_state_vector_size;
} pemc_model_functions;

// function pointer for formulas

typedef struct {
  const char* identifier;
  unsigned char* formula;
} formula;

// function pointer for entry points

typedef int32_t (*check_reachability_in_executable_model_function_type)(
    pemc_model_functions model_functions);

typedef int32_t (*test_function_type)(void);

typedef struct {
  check_reachability_in_executable_model_function_type
      check_reachability_in_executable_model;
  test_function_type test;
} pemc_functions;

PEMC_API int32_t assign_pemc_functions(pemc_functions* target);

#ifdef __cplusplus
}
#endif

#endif
