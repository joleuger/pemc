// SPDX-License-Identifier: MIT
// The MIT License (MIT)
//
// Copyright (c) 2018, Microsoft
// Copyright (c) 2021, Johannes Leupolz
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

#include "pemc/basic/cancellation_token.h"

namespace {
static const long _STATE_CLEAR = 0;
static const long _STATE_CANCELED = 1;
}  // namespace

namespace pemc {
bool cancellation_token::is_cancelable() const {
  return state != nullptr;
}

bool cancellation_token::is_canceled() const {
  return (state != nullptr && state.get()->load() != _STATE_CLEAR);
}

cancellation_token_source::cancellation_token_source() {
  state = std::make_shared<std::atomic<long>>(_STATE_CLEAR);
}

cancellation_token cancellation_token_source::get_token() const {
  return cancellation_token(state);
}

void cancellation_token_source::cancel() const {
  state.get()->store(_STATE_CANCELED);
}
}  // namespace pemc