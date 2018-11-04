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

#ifndef PEMC_GENERICTRAVERSER_TEMPORARYSTATESTORAGE_H_
#define PEMC_GENERICTRAVERSER_TEMPORARYSTATESTORAGE_H_

#include <vector>
#include <gsl/span>
#include <cstdint>
#include <atomic>
#include <boost/align/aligned_allocator.hpp>

#include "pemc/basic/tscIndex.h"
#include "pemc/basic/probability.h"
#include "pemc/basic/label.h"
#include "pemc/basic/modelCapacity.h"
#include "pemc/basic/rawMemory.h"
#include "pemc/formula/formula.h"

namespace pemc {

  ///   We store states in a contiguous array, indexed by a continuous variable.
  ///   The TemporaryStateStorage is not thread safe.
  class TemporaryStateStorage {
  private:

      // The length in bytes of a state vector required for the analysis model.
      int32_t modelStateVectorSize = 0;
      // Extra bytes in state vector for preStateStorage modifiers.
      int32_t preStateStorageModifierStateVectorSize = 0;
      // The length in bytes of the state vector of the analysis model with the extra bytes
      // required for the preStateStorage modifiers
      int32_t stateVectorSize = 0;

      // The number of saved states
      StateIndex savedStates = 0;
  	  // The number of states that can be cached and the number of reserved states.
      StateIndex totalCapacity;

      // the memory that contains the serialized states
      std::vector<gsl::byte> stateMemory;

      void resizeStateBuffer();

  public:
      TemporaryStateStorage(StateIndex _capacity);

      StateIndex getNumberOfSavedStates();

      gsl::span<gsl::byte> operator [](size_t idx);

      StateIndex getFreshStateIndex();

      void setStateVectorSize(int32_t _modelStateVectorSize, int32_t _preStateStorageModifierStateVectorSize);

      void clear();
  };

}

#endif  // PEMC_GENERICTRAVERSER_TEMPORARYSTATESTORAGE_H_
