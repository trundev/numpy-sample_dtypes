#ifndef _NPY_CASTS_H
#define _NPY_CASTS_H

#include <pybind11/pybind11.h>
#include <vector>

#include "numpy/dtype_api.h"

namespace py = pybind11;

class dtype_casts {
  std::vector<PyArrayMethod_Spec *> specs;

public:
  ~dtype_casts();
  PyArrayMethod_Spec **init(py::object scalar_type);
};

#endif /* _NPY_CASTS_H */
