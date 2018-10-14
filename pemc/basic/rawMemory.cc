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

#include <string>
#include <stdio.h>
#include <functional>

#include "pemc/basic/rawMemory.h"

namespace pemc {

    void deleter(void *data) {
        ::operator delete(data);
    }

    // idea from
    // https://stackoverflow.com/questions/39288891/why-is-shared-ptrvoid-legal-while-unique-ptrvoid-is-ill-formed
    unique_void_ptr unique_void(void* ptr) {
        return unique_void_ptr(ptr, &deleter);
    }



    /// <summary>
    ///   Compares the two buffers <paramref name="buffer1" /> and <paramref name="buffer2" />, returning <c>true</c> when the
    ///   buffers are equivalent.
    /// </summary>
    /// <param name="buffer1">The first buffer of memory to compare.</param>
    /// <param name="buffer2">The second buffer of memory to compare.</param>
    /// <param name="sizeInBytes">The size of the buffers in bytes.</param>
    bool areBuffersEqual(pbyte* buffer1, pbyte* buffer2, size_t sizeInBytes) {
      if (buffer1 == buffer2)
        return true;

      for (auto i = sizeInBytes / 8; i > 0; --i) {
        if (* reinterpret_cast<int64_t*>(buffer1) != * reinterpret_cast<int64_t*>(buffer2))
          return false;

        buffer1 += 8;
        buffer2 += 8;
      }

      auto remaining = sizeInBytes % 8;
      if (remaining >= 4) {
        if (* reinterpret_cast<int32_t*>(buffer1) != * reinterpret_cast<int32_t*>(buffer2))
          return false;

        buffer1 += 4;
        buffer2 += 4;
        remaining -= 4;
      }

      for (auto i = 0; i < remaining; ++i) {
        if (*buffer1 != *buffer2)
          return false;

        buffer1 += 1;
        buffer2 += 1;
      }

      return true;
    }

    /// <summary>
    ///   Copies <paramref name="sizeInBytes" />-many bytes from <paramref name="source" /> to <see cref="destination" />.
    /// </summary>
    /// <param name="source">The first buffer of memory to compare.</param>
    /// <param name="destination">The second buffer of memory to compare.</param>
    /// <param name="sizeInBytes">The size of the buffers in bytes.</param>
    void copyBuffers(pbyte* source, pbyte* destination, size_t sizeInBytes) {
      for (auto i = sizeInBytes / 8; i > 0; --i) {
        * reinterpret_cast<int64_t*>(destination) = * reinterpret_cast<int64_t*>(source);

        source += 8;
        destination += 8;
      }

      auto remaining = sizeInBytes % 8;
      if (remaining >= 4) {
        * reinterpret_cast<int32_t*>(destination) = * reinterpret_cast<int32_t*>(source);

        source += 4;
        destination += 4;
        remaining -= 4;
      }

      for (auto i = 0; i < remaining; ++i) {
        *destination = *source;

        source += 1;
        destination += 1;
      }
    }

    /// <summary>
    ///   Hashes the <paramref name="buffer" />.
    /// </summary>
    /// <param name="buffer">The buffer of memory that should be hashed.</param>
    /// <param name="sizeInBytes">The size of the buffer in bytes.</param>
    /// <param name="seed">The seed value for the hash.</param>
    /// <remarks>See also https://en.wikipedia.org/wiki/MurmurHash (MurmurHash3 implementation)</remarks>
    uint32_t hashBuffer(pbyte* buffer, size_t sizeInBytes, int32_t seed) {
      const uint32_t c1 = 0xcc9e2d51;
      const uint32_t c2 = 0x1b873593;
      const int32_t r1 = 15;
      const int32_t r2 = 13;
      const uint32_t m = 5;
      const uint32_t n = 0xe6546b64;

      auto hash = (uint32_t)seed;
      auto numBlocks = sizeInBytes / 4;
      auto blocks = reinterpret_cast<uint32_t*>(buffer);

      for (auto i = 0; i < numBlocks; i++) {
        auto k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
      }

      auto tail = buffer + numBlocks * 4;
      auto k1 = 0u;

      switch (sizeInBytes & 3) {
        case 3:
          k1 ^= (uint32_t)tail[2] << 16;
        case 2:
          k1 ^= (uint32_t)tail[1] << 8;
        case 1:
          k1 ^= tail[0];
          k1 *= c1;
          k1 = (k1 << r1) | (k1 >> (32 - r1));
          k1 *= c2;
          hash ^= k1;
      }

      hash ^= (uint32_t)sizeInBytes;
      hash ^= hash >> 16;
      hash *= (0x85ebca6b);
      hash ^= hash >> 13;
      hash *= (0xc2b2ae35);
      hash ^= hash >> 16;

      return hash;
    }
}
