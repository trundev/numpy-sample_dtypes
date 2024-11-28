// Must be included first
#include "src/numpy.h"

#include <iostream>
#include <locale>

// Place `<locale>` before <Python.h> to avoid a build failure in macOS.
#include <Python.h>

#include "src/common.h"
#include "src/some_dtype.h"

namespace sample_dtypes {

static PyObject *RegisterDtype(PyObject *self, PyObject *args) {
  std::cout << "DEBUG: " << __func__ << std::endl;

  const char *name = nullptr;
  unsigned int count = 0;
  PyTypeObject *data_type = nullptr;
  if (!PyArg_ParseTuple(args, "sO!I", &name, &PyType_Type, &data_type,
                        &count)) {
    std::cout << "DEBUG: " << "PyArg_ParseTuple failed" << std::endl;
    return nullptr;
  }

  // Get the size of type
  unsigned data_type_size = 0;
  if (PyType_IsSubtype(data_type, &PyLong_Type)) {
    data_type_size = sizeof(uint64_t);
    std::cout << "DEBUG: data_type(int) specified" << std::endl;
  } else if (PyType_IsSubtype(data_type, &PyFloat_Type)) {
    data_type_size = sizeof(float);
    std::cout << "DEBUG: data_type(float) specified" << std::endl;
  } else {
    // Not a supported python data-type, check numpy types
    // PyObject *type_dict = PyType_GetDict(type);
    PyObject *type_nbytes = PyObject_GetAttrString(
        reinterpret_cast<PyObject *>(data_type), "nbytes");
    if (type_nbytes == nullptr) {
      std::cout << "DEBUG: PyObject_GetAttrString 'nbytes' failed" << std::endl;
      PyErr_Format(PyExc_TypeError, "Cannot interpret '%R' as a data type",
                   data_type);
      return nullptr;
    }
    if (!PyArg_Parse(type_nbytes, "I", &data_type_size)) {
      std::cout << "DEBUG: PyArg_Parse failed" << std::endl;
      return nullptr;
    }
    std::cout << "DEBUG: numpy.dtype specified, nbytes=" << data_type_size
              << std::endl;
  }

  PyObject *type_name = PyType_GetQualName(data_type);
  const char *type_name_str = nullptr;
  if (!PyArg_Parse(type_name, "s", &type_name_str)) {
    std::cout << "DEBUG: PyArg_Parse failed" << std::endl;
    return nullptr;
  }
  std::cout << "DEBUG: _register_dtype_ext(name=" << name
            << ", data_type=" << type_name_str << ", count=" << count
            << ") -> data_type_size=" << data_type_size << std::endl;

  // Create python type object
  PyType_Slot type_slots[]{
      {0, nullptr},
  };
  typedef struct {
    PyObject_HEAD
    // int value[0];
  } type_struct;
  PyType_Spec type_spec = {
      .name = name,
      .basicsize =
          static_cast<int>(sizeof(type_struct) + data_type_size * count),
      .itemsize = 0,
      .flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
      .slots = type_slots,
  };
  PyObject *type = PyType_FromSpecWithBases(
      &type_spec, reinterpret_cast<PyObject *>(&PyGenericArrType_Type));
  if (!type) {
    return nullptr;
  }

  return type;
}

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

static PyMethodDef module_methods[] = {{"_register_dtype_ext", RegisterDtype,
                                        METH_VARARGS,
                                        "Create/register a dtype class"},
                                       {nullptr, nullptr, 0}};

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "_sample_dtypes_ext",
    "sample_dtypes extension module",
    -1,
    module_methods,
};

PyMODINIT_FUNC PyInit__sample_dtypes_ext() {
  std::cout << "DEBUG: " << __DATE__ << " " << __TIME__ << ": " << __func__
            << std::endl;
#ifdef _HYBRID
  std::cout << "HYBRID build" << std::endl;
#endif

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
