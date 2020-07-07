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

#include "pemc/c_api/c_api.h"

static void serialize(unsigned char* position, size_t size) {}

static void deserialize(unsigned char* position, size_t size) {}

static void resetToInitialState() {}

static void step() {}

static int32_t getStateVectorSize() {
  return sizeof(int);
}

extern "C" int32_t assign_pemc_functions(pemc_functions* target) {
  target->serialize = (pemc_serialize_function_type)serialize;
  target->deserialize = (pemc_deserialize_function_type)deserialize;
  target->reset_to_initial_state =
      (pemc_reset_to_initial_state_function_type)resetToInitialState;
  target->step = (pemc_step_function_type)step;
  target->get_state_vector_size =
      (pemc_get_state_vector_size_function_type)getStateVectorSize;
  return 0;
}