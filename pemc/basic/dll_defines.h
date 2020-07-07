// SPDX-License-Identifier: MIT
// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2019, Johannes Leupolz
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

#ifndef PEMC_BASIC_DLL_DEFINES_H_
#define PEMC_BASIC_DLL_DEFINES_H_

// see https://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define PEMC_HELPER_DLL_IMPORT __declspec(dllimport)
#define PEMC_HELPER_DLL_EXPORT __declspec(dllexport)
#define PEMC_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define PEMC_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#define PEMC_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#define PEMC_HELPER_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define PEMC_HELPER_DLL_IMPORT
#define PEMC_HELPER_DLL_EXPORT
#define PEMC_HELPER_DLL_LOCAL
#endif
#endif

// Now we use the generic helper definitions above to define PEMC_API and
// PEMC_LOCAL. PEMC_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) PEMC_LOCAL is used
// for non-api symbols.

#ifdef PEMC_DLL          // defined if PEMC is compiled as a shared DLL
#ifdef PEMC_DLL_EXPORTS  // defined if we are building the PEMC DLL (instead of
                         // using it)
#define PEMC_API PEMC_HELPER_DLL_EXPORT
#else
#define PEMC_API PEMC_HELPER_DLL_IMPORT
#endif  // PEMC_DLL_EXPORTS
#define PEMC_LOCAL PEMC_HELPER_DLL_LOCAL
#else  // PEMC_DLL is not defined: this means PEMC is a static lib.
#define PEMC_API
#define PEMC_LOCAL
#endif  // PEMC_DLL

#endif