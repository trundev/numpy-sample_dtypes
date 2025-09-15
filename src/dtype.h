#ifndef _NPY_DTYPE_H
#define _NPY_DTYPE_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

typedef struct {
  PyArray_Descr base;
  py::object py_scalar;
} SampleDTypeObject;

typedef struct {
  PyArray_DTypeMeta base;
  py::object scalar_type; // TODO: Use py::type instead
} SampleDType_type;

py::object init_dtype(py::type scalar_type);

#endif /*_NPY_DTYPE_H*/
