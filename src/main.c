#include <Python.h>

#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ufuncobject.h"

#include "dtype.h"

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_sample_dtypes_main",
    .m_doc = "Experimental/sample NumPy Custom Data Type module, see:\n"
             " https://numpy.org/doc/stable/reference/c-api/"
             "array.html#custom-data-types",
    .m_size = -1,
    .m_methods = NULL,
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

  if (PyModule_AddObject(m, "SampleDType",
                         &SampleDType.super.ht_type.ob_base.ob_base) < 0) {
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
  Py_XDECREF(SampleScalar_Type);
  return NULL;
}
