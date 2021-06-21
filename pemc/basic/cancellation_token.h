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

// Interface ripped off and simplified from the MIT-licensed Parallel Patterns
// Library. See
// https://github.com/mghazaryan/pplx/blob/master/include/pplx/pplxcancellation_token.h

#ifndef PEMC_BASIC_CANCELLATION_TOKEN_H_
#define PEMC_BASIC_CANCELLATION_TOKEN_H_

#include <cstdint>
#include <functional>
#include <vector>

namespace pemc {

class cancellation_token_source;

/// <summary>
///     The <c>cancellation_token</c> class represents the ability to determine
///     whether some operation has been requested to cancel. It can also be
///     polled for cancellation or have a callback registered for if and when
///     the associated <c>cancellation_token_source</c> is canceled.
/// </summary>
class cancellation_token {
 private:
  friend class cancellation_token_source;

  // weak_ptr is not enought because cancellation should be permanent even when
  // the source cancellation_token_source is gone.
  std::shared_ptr<std::atomic<long>> state;

  cancellation_token() {}

  cancellation_token(std::shared_ptr<std::atomic<long>> _state) {
    state = _state;
  }

 public:
  /// <summary>
  ///     Returns a cancellation token which can never be subject to
  ///     cancellation.
  /// </summary>
  /// <returns>
  ///     A cancellation token that cannot be canceled.
  /// </returns>
  static cancellation_token none() { return cancellation_token(); }

  cancellation_token(const cancellation_token& _Src) = default;

  cancellation_token(cancellation_token&& _Src) = default;

  cancellation_token& operator=(const cancellation_token& _Src) = default;

  cancellation_token& operator=(cancellation_token&& _Src) = default;

  bool operator==(const cancellation_token& _Src) const {
    return state == _Src.state;
  }

  bool operator!=(const cancellation_token& _Src) const {
    return !(operator==(_Src));
  }

  /// <summary>
  ///     Returns an indication of whether this token can be canceled or not.
  /// </summary>
  /// <returns>
  ///     An indication of whether this token can be canceled or not.
  /// </returns>
  bool is_cancelable() const;

  /// <summary>
  /// Returns <c>true</c> if the token has been canceled.
  /// </summary>
  /// <returns>
  /// The value <c>true</c> if the token has been canceled; otherwise, the value
  /// <c>false</c>.
  /// </returns>
  bool is_canceled() const;
};

/// <summary>
///     The <c>cancellation_token_source</c> class represents the ability to
///     cancel some cancelable operation.
/// </summary>
class cancellation_token_source {
 private:
  std::shared_ptr<std::atomic<long>> state;

 public:
  /// <summary>
  ///     Constructs a new <c>cancellation_token_source</c>.  The source can be
  ///     used to flag cancellation of some cancelable operation.
  /// </summary>
  cancellation_token_source();

  cancellation_token_source(const cancellation_token_source& _Src) = default;

  cancellation_token_source(cancellation_token_source&& _Src) = default;

  cancellation_token_source& operator=(const cancellation_token_source& _Src) =
      default;

  cancellation_token_source& operator=(cancellation_token_source&& _Src) =
      default;

  bool operator==(const cancellation_token_source& _Src) const {
    return state == _Src.state;
  }

  bool operator!=(const cancellation_token_source& _Src) const {
    return !(operator==(_Src));
  }

  /// <summary>
  ///     Returns a cancellation token associated with this source.  The
  ///     returned token can be polled for cancellation or provide a callback if
  ///     and when cancellation occurs.
  /// </summary>
  /// <returns>
  ///     A cancellation token associated with this source.
  /// </returns>
  cancellation_token get_token() const;

  /// <summary>
  ///     Cancels the token.  Any <c>task_group</c>,
  ///     <c>structured_task_group</c>, or <c>task</c> which utilizes the token
  ///     will be canceled upon this call and throw an exception at the next
  ///     interruption point.
  /// </summary>
  void cancel() const;
};

}  // namespace pemc

#endif