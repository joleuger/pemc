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

#include <stdint.h>

#include "pemc/basic/dll_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pemc_serialize_function_type)(unsigned char*,  // position
                                             size_t);         // size

typedef void (*pemc_deserialize_function_type)(unsigned char*,  // position
                                               size_t);         // size

typedef void (*pemc_reset_to_initial_state_function_type)(void);

typedef void (*pemc_step_function_type)(void);

typedef int32_t (*pemc_get_state_vector_size_function_type)(void);

typedef struct {
  pemc_serialize_function_type serialize;
  pemc_deserialize_function_type deserialize;
  pemc_reset_to_initial_state_function_type reset_to_initial_state;
  pemc_step_function_type step;
  pemc_get_state_vector_size_function_type get_state_vector_size;
} pemc_functions;

PEMC_API int32_t assign_pemc_functions(pemc_functions* target);

#ifdef __cplusplus
}
#endif

#endif