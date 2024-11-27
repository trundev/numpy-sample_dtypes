// Must be included first
// clang-format off
#include "src/numpy.h"
// clang-format on

#include <iostream>
#include <locale>

// Place `<locale>` before <Python.h> to avoid a build failure in macOS.
#include <Python.h>

#include "src/common.h"

namespace sample_dtypes {

/*
 *TODO: Implement Dtype casts registration
 */

// Initialize type attribute in the module object.

/*
 *TODO: Implement Dtype module initialization
 */

// Initializes the module.
bool Initialize() {
  sample_dtypes::ImportNumpy();
  import_umath1(false);

  Safe_PyObjectPtr numpy_str = make_safe(PyUnicode_FromString("numpy"));
  if (!numpy_str) {
    return false;
  }
  Safe_PyObjectPtr numpy = make_safe(PyImport_Import(numpy_str.get()));
  if (!numpy) {
    return false;
  }

  /*
   *TODO: Register Dtype-s
   */

  // Register casts between pairs of custom float dtypes.
  bool success = true;

  /*
   *TODO: Register Dtype casts
   */

  return success;
}

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "_sample_dtypes_ext",
};

PyMODINIT_FUNC PyInit__sample_dtypes_ext() {
  std::cout << "DEBUG: " << __func__ << std::endl;

  Safe_PyObjectPtr m = make_safe(PyModule_Create(&module_def));
  if (!m) {
    return nullptr;
  }
  if (!Initialize()) {
    if (!PyErr_Occurred()) {
      PyErr_SetString(PyExc_RuntimeError,
                      "cannot load _sample_dtypes_ext module.");
    }
    return nullptr;
  }

  /*
   * TODO: Initialize Dtype modules
   */

#ifdef Py_GIL_DISABLED
  PyUnstable_Module_SetGIL(m.get(), Py_MOD_GIL_NOT_USED);
#endif

  return m.release();
}
} // namespace sample_dtypes
