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

#include "pemc/basic/cancellation_token.h"

#include <gtest/gtest.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

using namespace pemc;

TEST(basic_test, cancellation_token_works_when_cancelled) {
  std::atomic<bool> success(false);

  std::mutex mutex1;
  std::unique_lock<std::mutex> lock1(mutex1);
  // std::unique_lock<std::mutex> lock2(mutex1,std::defer_lock);

  cancellation_token_source tokenSource;
  auto cancellationToken = tokenSource.get_token();

  auto f1 = std::thread([&mutex1, &success, &cancellationToken]() {
    std::unique_lock<std::mutex> lock1(mutex1);
    success = cancellationToken.is_canceled();
  });
  auto f2 = std::thread([&lock1, &tokenSource]() {
    tokenSource.cancel();
    lock1.unlock();
  });

  f1.join();
  f2.join();

  ASSERT_EQ(success.load(), true) << "FAIL";
}

TEST(basic_test, cancellation_token_works_when_not_cancelled) {
  std::atomic<bool> success(false);

  std::mutex mutex1;
  std::unique_lock<std::mutex> lock1(mutex1);
  // std::unique_lock<std::mutex> lock2(mutex1,std::defer_lock);

  cancellation_token_source tokenSource;
  auto cancellationToken = tokenSource.get_token();

  auto f1 = std::thread([&mutex1, &success, &cancellationToken]() {
    std::unique_lock<std::mutex> lock1(mutex1);
    success = cancellationToken.is_canceled();
  });
  auto f2 = std::thread([&lock1]() { lock1.unlock(); });

  f1.join();
  f2.join();

  ASSERT_EQ(success.load(), false) << "FAIL";
}

TEST(basic_test, cancellation_token_works_in_other_thread) {
  std::atomic<bool> success(false);

  std::mutex mutex1;
  std::unique_lock<std::mutex> lock1(mutex1);

  cancellation_token_source tokenSource;
  auto cancellationToken = tokenSource.get_token();

  auto f1 = std::thread([&lock1, &success, &cancellationToken]() {
    bool unlock_mutex = true;
    while (!cancellationToken.is_canceled()) {
      // loop until cancellationToken is canceled

      // ensure that cancelation_token is initially not cancelled and loop is
      // run at least once
      if (unlock_mutex) {
        unlock_mutex = false;
        lock1.unlock();
      }
    }
    success = cancellationToken.is_canceled();
  });
  auto f2 = std::thread([&mutex1, &tokenSource]() {
    // ensure that cancelation_token is initially not cancelled
    std::unique_lock<std::mutex> lock1(mutex1);

    tokenSource.cancel();
  });

  f1.join();
  f2.join();

  ASSERT_EQ(success.load(), true) << "FAIL";
}

TEST(basic_test, cancellation_token_none) {
  auto cancellationToken = cancellation_token::none();

  ASSERT_EQ(cancellationToken.is_canceled(), false) << "FAIL";
  ASSERT_EQ(cancellationToken.is_cancelable(), false) << "FAIL";
}