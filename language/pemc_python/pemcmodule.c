// The MIT License (MIT)
//
// Copyright (c) 2014-2018, Institute for Software & Systems Engineering
// Copyright (c) 2018-2024, Johannes Leupolz
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
#define CHOOSE_MAX_OPTIONS 8
#include <Python.h>

#include "pemc_c/pemc_c.h"

// see https://docs.python.org/3/extending/extending.html

// Currently, the python API uses a static "state" element in the class
// pypemc.PemcModel, which has basically always the same size. To access the
// element, the user can use the set_state and get_state methods of the class.
// Also, only
// - a simple "evaluate_formula" method is existing, and
// - a simple "choose" method is existing.
// - no parallelizm, because we don't really create model instances, yet

pemc_functions pemc_function_accessor;

typedef struct {
  // Fields of the C struct
  int32_t value1;
} PemcModelState;

typedef struct {
  PyObject_HEAD  // PyObject_HEAD
      pemc_model_specific_interface* pemc_interface;
  PemcModelState model_state;
} PemcModelObject;

// Adapter methods

static void pyadapter_model_create(
    unsigned char** model,
    pemc_model_specific_interface* _pemc_interface,
    unsigned char* optional_value_for_model_create) {
  // optional_value_for_model_create is the python class that needs to be
  // instantiated

  // Create instance of the model
  PyObject* py_class_of_model = (PyObject*)optional_value_for_model_create;

  PyObject* model_instance_py = PyObject_CallObject(py_class_of_model, NULL);
  if (model_instance_py == NULL)
    return;

  *model = (unsigned char*)model_instance_py;
  PemcModelObject* model_instance = (PemcModelObject*)model_instance_py;

  Py_XINCREF(model_instance_py);

  // now assign the values to the object

  model_instance->pemc_interface = _pemc_interface;

  printf("pyadapter model created\n");
}

static void pyadapter_model_free(unsigned char* model) {
  // retrieve model
  PyObject* model_instance_py = (PyObject*)model;

  Py_DECREF(model_instance_py);

  printf("pyadapter model destructed\n");
}

static void pyadapter_model_serialize(unsigned char* model,
                                      unsigned char* position,
                                      size_t size) {
  // retrieve model
  PemcModelObject* model_instance = (PemcModelObject*)model;

  *(int32_t*)position =
      model_instance->model_state.value1;  // copy value to position

  printf("pyadapter model serialized: %d\n",
         model_instance->model_state.value1);
}

static void pyadapter_model_deserialize(unsigned char* model,
                                        unsigned char* position,
                                        size_t size) {
  // retrieve model
  PemcModelObject* model_instance = (PemcModelObject*)model;

  int32_t state =
      *(int32_t*)position;  // copy value in position to a temporary variable

  model_instance->model_state.value1 = state;  // copy value to model

  printf("pyadapter model deserialized: %d\n",
         model_instance->model_state.value1);
}

static void pyadapter_model_reset_to_initial_state(unsigned char* model) {
  // retrieve model
  PyObject* model_instance_py = (PyObject*)model;
  PemcModelObject* model_instance = (PemcModelObject*)model;

  // Todo: Get value from the model_instance_py.step()
  model_instance->model_state.value1 = 0;  // set initial value
  printf("pyadapter model reset\n");
}

static void pyadapter_model_step(unsigned char* model) {
  // retrieve model
  printf("pyadapter model step\n");
  PyObject* model_instance_py = (PyObject*)model;
  PemcModelObject* model_instance = (PemcModelObject*)model;

  printf("Before step %d\n", model_instance->model_state.value1);
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* result = PyObject_CallMethod(model_instance_py, "step", NULL);
  if (result == NULL) {
    printf("Evaluate step failed\n");
    PyErr_Print();
  }
  PyGILState_Release(gstate);
}

int32_t pyadapter_model_get_state_vector_size(unsigned char* model) {
  return sizeof(int32_t);
}

int32_t pyadapter_evaluate_formula(unsigned char* model) {
  // retrieve model
  printf("Evaluate formula\n");
  PyObject* model_instance_py = (PyObject*)model;

  PyGILState_STATE gstate = PyGILState_Ensure();

  PyObject* result =
      PyObject_CallMethod(model_instance_py, "evaluate_formula", NULL);
  if (result == NULL) {
    printf("Evaluate formula failed\n");
    PyErr_Print();
    PyGILState_Release(gstate);
    return 0;
  }
  int32_t is_true = PyObject_IsTrue(result);
  PyGILState_Release(gstate);

  Py_DECREF(result);
  printf("Evaluate formula %d\n", is_true);

  return is_true;
}
// standard implementations of the class

static PyObject* PemcModel_get_initial_state_value(PyObject* self,
                                                   PyObject* args) {
  return PyLong_FromLong(0);
}

static PyObject* PemcModel_step(PemcModelObject* self, PyObject* args) {
  // Do nothing in the step method. This needs to be overwritten by the user.
  Py_RETURN_NONE;
}

PyObject* get_state(PyObject* self, PyObject* args) {
  return PyLong_FromLong(0);
}

PyObject* set_state(PyObject* self, PyObject* args) {
  long value;

  if (!PyArg_ParseTuple(args, "l", &value))
    return NULL;

  printf("State set to: %ld\n", value);
  Py_RETURN_NONE;
}

static PyObject* PemcModel_choose(PemcModelObject* self, PyObject* args) {
  Py_ssize_t numOptions = PyTuple_Size(args);
  if (numOptions < 1) {
    PyErr_SetString(PyExc_ValueError,
                    "Expected at least one integer argument.");
    return NULL;
  }

  if (numOptions > CHOOSE_MAX_OPTIONS) {
    PyErr_SetString(PyExc_ValueError, "Too many options.");
    return NULL;
  }

  // Allocate the options array on the stack
  int32_t options[CHOOSE_MAX_OPTIONS];

  for (Py_ssize_t i = 0; i < numOptions; i++) {
    PyObject* item = PyTuple_GetItem(args, i);
    if (!PyLong_Check(item)) {
      PyErr_SetString(PyExc_TypeError, "Arguments must be integers.");
      return NULL;
    }
    options[i] = (int32_t)PyLong_AsLong(item);
    if (PyErr_Occurred()) {
      return NULL;
    }
  }

  // Call your function
  int32_t state = pemc_function_accessor.pemc_choose_int_option(
      self->pemc_interface, options, (int)numOptions);

  return PyLong_FromLong((long)state);
}

static PyObject* PemcModel_evaluate_formula(PyObject* self, PyObject* args) {
  Py_RETURN_TRUE;
}

static void PemcModel_dealloc(PemcModelObject* self) {
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PemcModel_new(PyTypeObject* type,
                               PyObject* args,
                               PyObject* kwds) {
  PemcModelObject* self;
  self = (PemcModelObject*)type->tp_alloc(type, 0);
  if (self != NULL) {
  }
  return (PyObject*)self;
}

static PyMethodDef PemcModel_methods[] = {
    {"step", (PyCFunction)PemcModel_step, METH_NOARGS,
     "Perform a step operation."},
    {"get_initial_state_value", (PyCFunction)PemcModel_get_initial_state_value,
     METH_NOARGS, "Return initial state value"},
    {"step", (PyCFunction)PemcModel_step, METH_NOARGS,
     "Perform a step operation."},
    {"get_state", (PyCFunction)get_state, METH_NOARGS, "Get state"},
    {"set_state", set_state, METH_VARARGS, "Set state"},
    {"choose", (PyCFunction)PemcModel_choose, METH_VARARGS,
     "Description of pemc_choose_int_option"},
    {"evaluate_formula", (PyCFunction)PemcModel_evaluate_formula, METH_NOARGS,
     "Evaluate formula (return True)"},

    {NULL, NULL, 0, NULL}  // Sentinel
};

static PyTypeObject PemcModelType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "pemc.PemcModel",
    .tp_doc = "PemcModel object",
    .tp_basicsize = sizeof(PemcModelObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  // Enable subclassing
    .tp_new = PemcModel_new,
    .tp_dealloc = (destructor)PemcModel_dealloc,
    .tp_methods = PemcModel_methods,
};

// Formulas
static PyObject* py_pemc_register_basic_formula(PyObject* self,
                                                PyObject* args) {
  // Implementation
  // ...

  Py_RETURN_NONE;
}

static void setup_model_functions(pemc_model_functions* _model_functions) {
  // setup model functions
  _model_functions->model_create = (pemc_model_create)pyadapter_model_create;
  _model_functions->model_free = (pemc_model_free)pyadapter_model_free;
  _model_functions->serialize =
      (pemc_serialize_function_type)pyadapter_model_serialize;
  _model_functions->deserialize =
      (pemc_deserialize_function_type)pyadapter_model_deserialize;
  _model_functions->reset_to_initial_state =
      (pemc_reset_to_initial_state_function_type)
          pyadapter_model_reset_to_initial_state;
  _model_functions->step = (pemc_step_function_type)pyadapter_model_step;
  _model_functions->get_state_vector_size =
      (pemc_get_state_vector_size_function_type)
          pyadapter_model_get_state_vector_size;
}

// Main functionality
static PyObject* py_check_reachability_in_executable_model(PyObject* self,
                                                           PyObject* args) {
  pemc_model_functions model_functions;
  setup_model_functions(&model_functions);

  pemc_formula_ref* f1 = pemc_function_accessor.pemc_register_basic_formula(
      pyadapter_evaluate_formula);

  PyObject* py_class_of_model;

  if (!PyArg_ParseTuple(args, "O", &py_class_of_model))
    return NULL;

  int32_t result =
      pemc_function_accessor.check_reachability_in_executable_model(
          model_functions, py_class_of_model, f1);

  // Create and return a Python integer object with the result
  return PyLong_FromLong(result);
}

static PyObject* py_build_lmc_from_executable_model(PyObject* self,
                                                    PyObject* args) {
  // Implementation
  // ...
  Py_RETURN_NONE;
}

static PyObject* py_calculate_probability_to_reach_state_within_bound(
    PyObject* self,
    PyObject* args) {
  // Implementation
  // ...
  Py_RETURN_NONE;
}

// Method table
static PyMethodDef PemcMethods[] = {
    {"pemc_register_basic_formula", py_pemc_register_basic_formula,
     METH_VARARGS, "Description of pemc_register_basic_formula"},

    {"check_reachability_in_executable_model",
     py_check_reachability_in_executable_model, METH_VARARGS,
     "Description of check_reachability_in_executable_model"},

    {"build_lmc_from_executable_model", py_build_lmc_from_executable_model,
     METH_VARARGS, "Description of build_lmc_from_executable_model"},

    {"calculate_probability_to_reach_state_within_bound",
     py_calculate_probability_to_reach_state_within_bound, METH_VARARGS,
     "Description of calculate_probability_to_reach_state_within_bound"},

    {NULL, NULL, 0, NULL}  // Sentinel
};

static struct PyModuleDef pemcmodule = {
    PyModuleDef_HEAD_INIT, "pemc", /* name of module */
    NULL,                          /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module,
           or -1 if the module keeps state in global variables. */
    PemcMethods};

PyMODINIT_FUNC PyInit_pypemc(void) {
  printf("1\n");

  assign_pemc_functions_from_dll(&pemc_function_accessor);
  printf("2\n");

  if (PyType_Ready(&PemcModelType) < 0) {
    return NULL;
  }

  printf("3\n");

  PyObject* module = PyModule_Create(&pemcmodule);

  printf("4\n");
  if (module == NULL) {
    return NULL;
  }

  printf("5\n");

  Py_INCREF(&PemcModelType);

  printf("6\n");
  if (PyModule_AddObject(module, "PemcModel", (PyObject*)&PemcModelType) < 0) {
    Py_DECREF(&PemcModelType);
    Py_DECREF(module);
    return NULL;
  }

  printf("7\n");

  return module;
}
