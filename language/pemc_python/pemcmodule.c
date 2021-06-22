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

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pemc_c/pemc_c.h"

// see https://docs.python.org/3/extending/extending.html

pemc_functions pemc_functions_from_dll;

static PyObject* pemc_check_lmc(PyObject* self, PyObject* args) {
  const char* command;
  int sts;

  if (!PyArg_ParseTuple(args, "s", &command))
    return NULL;

  // calculate
  sts = 1234;

  return PyLong_FromLong(sts);
}

static PyObject* pemc_test(PyObject* self, PyObject* args) {
  const char* command;
  int sts;

  // calculate
  sts = pemc_functions_from_dll.test();

  return PyLong_FromLong(sts);
}

static PyMethodDef PemcMethods[] = {
    {"check_lmc", pemc_check_lmc, METH_VARARGS,
     "Check the data structure using a labeled Markov chain."},
    {"get_state_vector_size", pemc_test, METH_VARARGS,
     "Get State Vector Sized."},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef pemcmodule = {
    PyModuleDef_HEAD_INIT, "pemc", /* name of module */
    NULL,                          /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module,
           or -1 if the module keeps state in global variables. */
    PemcMethods};

PyMODINIT_FUNC PyInit_pypemc(void) {
  assign_pemc_functions_from_dll(&pemc_functions_from_dll);
  return PyModule_Create(&pemcmodule);
}
