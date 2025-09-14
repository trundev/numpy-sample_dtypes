#include <Python.h>

#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC

extern "C" {
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"
}

#include "casts.h"
#include "dtype.h"
#include "vector"

#if 1 //_DEBUG only
#include <stdarg.h>
static void print_fromformat(const char *func, const char *format, ...) {
  va_list vargs;
  va_start(vargs, format);

  PyObject *str_obj = PyUnicode_FromFormatV(format, vargs);
  if (str_obj != NULL) {
    printf("%s: %s", func, PyUnicode_AsUTF8(str_obj));
    Py_DECREF(str_obj);
  }

  va_end(vargs);
}
#define PRINT_FROMFORMAT(fmt, ...) print_fromformat(__func__, fmt, __VA_ARGS__)
#else
#define PRINT_FROMFORMAT(...)
#endif

static NPY_CASTING sample_to_sample_resolve_descriptors(
    PyObject *NPY_UNUSED(self), PyArray_DTypeMeta *NPY_UNUSED(dtypes[2]),
    PyArray_Descr *given_descrs[2], PyArray_Descr *loop_descrs[2],
    npy_intp *view_offset) {
  printf("TODO: %s, given_descrs=[%p, %p]\n", __func__, given_descrs[0],
         given_descrs[1]);

  if (given_descrs[0] == NULL) {
    return _NPY_ERROR_OCCURRED_IN_CAST;
  } else if (given_descrs[1] == NULL) {
    Py_INCREF(given_descrs[0]);
    loop_descrs[1] = given_descrs[0];
  } else {
    SampleDTypeObject *descr_in = (SampleDTypeObject *)given_descrs[0];
    SampleDTypeObject *descr_out = (SampleDTypeObject *)given_descrs[1];
    PRINT_FROMFORMAT("descr_in scalar=%R, descr_out scalar=%R\n",
                     descr_in->sample_scalar, descr_out->sample_scalar);

    PyObject *res =
        PyObject_CallMethod(descr_out->sample_scalar, "is_compatible", "O",
                            descr_in->sample_scalar);
    if (res == NULL) {
      return _NPY_ERROR_OCCURRED_IN_CAST;
    }
    int is_compatible = PyObject_IsTrue(res);
    Py_DECREF(res);
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
  SampleDTypeObject *descr_in = (SampleDTypeObject *)context->descriptors[0];
  SampleDTypeObject *descr_out = (SampleDTypeObject *)context->descriptors[1];
  PRINT_FROMFORMAT("descr_in scalar=%R, descr_out scalar=%R\n",
                   descr_in->sample_scalar, descr_out->sample_scalar);

  PyObject *res = PyObject_CallMethod(
      descr_out->sample_scalar, "cast_loop", "O (n,n) (n) (n,n)",
      // `in` scalar object
      descr_in->sample_scalar,
      // tuple of `in` and `out` data addresses
      (Py_ssize_t)data[0], (Py_ssize_t)data[1],
      // tuple of single N element
      (Py_ssize_t)dimensions[0],
      // tuple of `in` and `out` strides
      (Py_ssize_t)strides[0], (Py_ssize_t)strides[1]);
  if (res == NULL) {
    return -1;
  }
  int ret = PyLong_AsLong(res);
  Py_DECREF(res);
  if (ret == -1 && PyErr_Occurred()) {
    return -1;
  }

  return ret;
}

/*
 * NumPy currently allows NULL for the own DType/"cls".
 */
static PyArray_DTypeMeta *s2s_dtypes[] = {nullptr, nullptr};

static PyType_Slot s2s_slots[] = {
    {NPY_METH_resolve_descriptors,
     (void *)&sample_to_sample_resolve_descriptors},
    // CHECKME: Try to specify single/generic loop function only
    //{NPY_METH_strided_loop, (void *)&sample_to_sample_loop},
    {NPY_METH_unaligned_strided_loop, (void *)&sample_to_sample_loop},
    {0, nullptr}};

PyArrayMethod_Spec SampleToFloat64CastSpec = {
    // TODO: See
    // https://github.com/numpy/numpy-user-dtypes/blob/1111a86b99ce12633cf8eac8795d4ddafd45541a/unytdtype/unytdtype/src/casts.c#L475C25-L475C46
};

static std::vector<PyArrayMethod_Spec *> specs;

PyArrayMethod_Spec **init_casts_internal(void) {
  // TODO: ....
  printf("TODO: %s\n", __func__);

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
}

PyArrayMethod_Spec **init_casts(void) {
  try {
    return init_casts_internal();
  } catch (int e) {
    PyErr_NoMemory();
    return nullptr;
  }
}

void free_casts(void) {
  for (auto spec : specs) {
    if (spec) { /*All the specs, dtypes and slots are currently static
       delete[] spec->dtypes;
       delete[] spec->slots;*/
      delete spec;
    }
  }
  specs.clear();
}
