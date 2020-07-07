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

// https://en.wikipedia.org/wiki/Dynamic_loading

#include <stdio.h>

#define PEMC_LIBNAME

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HMODULE dlhandle;
#define DLLEXT "dll"
#define dlopen(filename, flag) LoadLibrary(filename)
#define dlsym(handle, symbol) (void*)GetProcAddress(handle, symbol)
#define dlclose(handle) FreeLibrary(handle)

#else
#include <dlfcn.h>

#ifdef __APPLE__
#define DLLEXT "dylib"
#else
#define DLLEXT "so"
#endif

#endif

#include "pemc_c/pemc_c.h"

int32_t assign_pemc_functions_from_dll(pemc_functions* target) {
  char dll_name[256];

  snprintf(dll_name, sizeof dll_name, "pemc_shared.%s", DLLEXT);

  void* pemc_library = dlopen(dll_name, RTLD_LAZY);
  if (pemc_library == NULL) {
    // report error ...
    printf("%s%s", "pemc_c.c dlopen: Could not open ", dll_name);
    return -1;
  }

  // use the result in a call to dlsym
  void* assign_pemc_functions_uncasted =
      dlsym(pemc_library, "assign_pemc_functions");
  if (assign_pemc_functions_uncasted == NULL) {
    // report error ...
    return -1;
  }

  // int32_t assign_pemc_functions(pemc_functions* target)
  // cast get_pemc_functions_uncasted to its proper type and use
  typedef int32_t (*assign_pemc_functions_uncasted_type)(pemc_functions*);
  assign_pemc_functions_uncasted_type assign_pemc_functions =
      (assign_pemc_functions_uncasted_type)assign_pemc_functions_uncasted;

  assign_pemc_functions(target);

  return 0;
}
