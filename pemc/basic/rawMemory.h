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

#ifndef PEMC_BASIC_RAWMEMORY_H_
#define PEMC_BASIC_RAWMEMORY_H_

#include <gsl/gsl_byte>

namespace pemc {

  using pbyte = gsl::byte; // pemc byte, use until std::byte is included

  using deleter_t = std::function<void(void *)>;

  using unique_void_ptr = std::unique_ptr<void, deleter_t>;

  unique_void_ptr unique_void(void* ptr);


  /// <summary>
  ///   Compares the two buffers <paramref name="buffer1" /> and <paramref name="buffer2" />, returning <c>true</c> when the
  ///   buffers are equivalent.
  /// </summary>
  /// <param name="buffer1">The first buffer of memory to compare.</param>
  /// <param name="buffer2">The second buffer of memory to compare.</param>
  /// <param name="sizeInBytes">The size of the buffers in bytes.</param>
  bool areBuffersEqual(pbyte* buffer1, pbyte* buffer2, size_t sizeInBytes);

  /// <summary>
  ///   Copies <paramref name="sizeInBytes" />-many bytes from <paramref name="source" /> to <see cref="destination" />.
  /// </summary>
  /// <param name="source">The first buffer of memory to compare.</param>
  /// <param name="destination">The second buffer of memory to compare.</param>
  /// <param name="sizeInBytes">The size of the buffers in bytes.</param>
  void copyBuffers(pbyte* source, pbyte* destination, size_t sizeInBytes);

  /// <summary>
  ///   Hashes the <paramref name="buffer" />.
  /// </summary>
  /// <param name="buffer">The buffer of memory that should be hashed.</param>
  /// <param name="sizeInBytes">The size of the buffer in bytes.</param>
  /// <param name="seed">The seed value for the hash.</param>
  /// <remarks>See also https://en.wikipedia.org/wiki/MurmurHash (MurmurHash3 implementation)</remarks>
  uint32_t hashBuffer(pbyte* buffer, size_t sizeInBytes, int32_t seed);

}

#endif  // PEMC_BASIC_RAWMEMORY_H_
