#include <Python.h>

#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"

#include "casts.h"
#include "dtype.h"

static NPY_CASTING sample_to_sample_resolve_descriptors(
    PyObject *NPY_UNUSED(self), PyArray_DTypeMeta *NPY_UNUSED(dtypes[2]),
    PyArray_Descr *given_descrs[2], PyArray_Descr *loop_descrs[2],
    npy_intp *view_offset) {
  py::print(__func__);

  if (given_descrs[0] == NULL) {
    return _NPY_ERROR_OCCURRED_IN_CAST;
  } else if (given_descrs[1] == NULL) {
    Py_INCREF(given_descrs[0]);
    loop_descrs[1] = given_descrs[0];
  } else {
    auto descr_in = reinterpret_cast<SampleDTypeObject *>(given_descrs[0]);
    auto descr_out = reinterpret_cast<SampleDTypeObject *>(given_descrs[1]);
#if __DEBUG
    py::print("  descr_in", descr_in->py_scalar, ", descr_out",
              descr_out->py_scalar);
#endif

    bool is_compatible;
    try {
      is_compatible =
          descr_out->py_scalar.attr("is_compatible")(descr_in->py_scalar)
              .cast<bool>();
    } catch (const py::error_already_set &e) {
      // Handle exceptions from Python
      PyErr_SetObject(e.type().ptr(), e.value().ptr());
      return _NPY_ERROR_OCCURRED_IN_CAST;
    }
    if (!is_compatible) {
      return NPY_UNSAFE_CASTING;
    }

    Py_INCREF(given_descrs[1]);
    loop_descrs[1] = given_descrs[1];
  }

  Py_INCREF(given_descrs[0]);
  loop_descrs[0] = given_descrs[0];

  *view_offset = 0;
  return NPY_NO_CASTING;
}

static int sample_to_sample_loop(PyArrayMethod_Context *context,
                                 char *const data[],
                                 npy_intp const dimensions[],
                                 npy_intp const strides[],
                                 void *NPY_UNUSED(auxdata)) {
  auto descr_in =
      reinterpret_cast<SampleDTypeObject *>(context->descriptors[0]);
  auto descr_out =
      reinterpret_cast<SampleDTypeObject *>(context->descriptors[1]);
#if __DEBUG
  py::print(__func__, "descr_in", descr_in->py_scalar, ", descr_out",
            descr_out->py_scalar);
#endif

  int ret;
  try {
    ret = descr_out->py_scalar
              .attr("cast_loop")(
                  // `in` scalar object
                  descr_in->py_scalar,
                  // tuple of `in` and `out` data addresses
                  py::make_tuple((Py_ssize_t)data[0], (Py_ssize_t)data[1]),
                  // tuple of single N element
                  py::make_tuple(dimensions[0]),
                  // tuple of `in` and `out` strides
                  py::make_tuple(strides[0], strides[1]))
              .cast<int>();
  } catch (const py::error_already_set &e) {
    // Handle exceptions from Python
    PyErr_SetObject(e.type().ptr(), e.value().ptr());
    return -1;
  }
  return ret;
}

static PyArray_DTypeMeta *s2s_dtypes[] = {nullptr, nullptr};

static PyType_Slot s2s_slots[] = {
    {NPY_METH_resolve_descriptors,
     (void *)&sample_to_sample_resolve_descriptors},
    // CHECKME: Try to specify single/generic loop function only
    //{NPY_METH_strided_loop, (void *)&sample_to_sample_loop},
    {NPY_METH_unaligned_strided_loop, (void *)&sample_to_sample_loop},
    {0, nullptr}};

PyArrayMethod_Spec **dtype_casts::init(py::object scalar_type) {
  // Add sample-to-sample cast
  specs.push_back(new PyArrayMethod_Spec{
      .name = "cast_SampleDType_to_SampleDType",
      .nin = 1,
      .nout = 1,
      .casting = NPY_SAFE_CASTING,
      .flags = NPY_METH_SUPPORTS_UNALIGNED,
      .dtypes = s2s_dtypes,
      .slots = s2s_slots,
  });

  // Finally NULL terminator
  specs.push_back(nullptr);
  return specs.data();
};

dtype_casts::~dtype_casts() {
  for (auto spec : specs) {
    if (spec) { /*All the specs, dtypes and slots are currently static
       delete[] spec->dtypes;
       delete[] spec->slots;*/
      delete spec;
    }
  }
  specs.clear();
}
