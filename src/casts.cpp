#include <Python.h>

#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC

extern "C" {
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"
}

#include "casts.h"
#include "vector"

static NPY_CASTING sample_to_sample_resolve_descriptors(
    PyObject *NPY_UNUSED(self), PyArray_DTypeMeta *NPY_UNUSED(dtypes[2]),
    PyArray_Descr *given_descrs[2], PyArray_Descr *loop_descrs[2],
    npy_intp *view_offset) {
  // TODO: See
  // https://github.com/numpy/numpy-user-dtypes/blob/1111a86/unytdtype/unytdtype/src/casts.c#L134
  printf("TODO: %s\n", __func__);
  return _NPY_ERROR_OCCURRED_IN_CAST;
}

static int sample_to_sample_get_loop(PyArrayMethod_Context *context,
                                     int aligned,
                                     int NPY_UNUSED(move_references),
                                     const npy_intp *strides,
                                     PyArrayMethod_StridedLoop **out_loop,
                                     NpyAuxData **out_transferdata,
                                     NPY_ARRAYMETHOD_FLAGS *flags) {
  // TODO: See
  // https://github.com/numpy/numpy-user-dtypes/blob/1111a86/unytdtype/unytdtype/src/casts.c#L314
  printf("TODO: %s\n", __func__);
  return -1;
}

/*
 * NumPy currently allows NULL for the own DType/"cls".
 */
static PyArray_DTypeMeta *s2s_dtypes[] = {nullptr, nullptr};

static PyType_Slot s2s_slots[] = {
    {NPY_METH_resolve_descriptors,
     (void *)&sample_to_sample_resolve_descriptors},
    {NPY_METH_get_loop, (void *)&sample_to_sample_get_loop},
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
