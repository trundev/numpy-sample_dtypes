#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL sample_dtypes_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL sample_dtypes_UFUNC_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ufuncobject.h"

#include "dtype.h"

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_sample_dtypes_main",
    .m_size = -1,
};

/* Module initialization function */
PyMODINIT_FUNC PyInit__sample_dtypes_main(void) {
  import_array();
  import_umath();

  PyObject *m = PyModule_Create(&moduledef);
  if (m == NULL) {
    return NULL;
  }

  PyObject *mod = PyImport_ImportModule("sample_dtypes");
  if (mod == NULL) {
    goto error;
  }
  SampleScalar_Type =
      (PyTypeObject *)PyObject_GetAttrString(mod, "SampleScalar");
  Py_DECREF(mod);

  if (SampleScalar_Type == NULL) {
    goto error;
  }

  if (init_sample_dtype() < 0) {
    goto error;
  }

#if 0 // TODO:...
    if (init_multiply_ufunc() == -1) {
        goto error;
    }
#endif

  return m;

error:
  Py_DECREF(m);
  return NULL;
}
