#include "structmember.h"
#include <Python.h>

#define PY_ARRAY_UNIQUE_SYMBOL sample_dtypes_ARRAY_API
#define PY_UFUNC_UNIQUE_SYMBOL sample_dtypes_UFUNC_API
#define NPY_NO_DEPRECATED_API NPY_2_0_API_VERSION
#define NPY_TARGET_VERSION NPY_2_0_API_VERSION
#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"

#include "dtype.h"

PyTypeObject *SampleScalar_Type = NULL;

/*
 * Internal helper to create new instances
 */
SampleDTypeObject *new_sampledtype_instance(PyObject *unit) {
  // TODO:....
  return NULL;
}

static PyType_Slot SampleDType_Slots[] = {
#if 0 // TODO:
        {NPY_DT_common_instance, &common_instance},
        {NPY_DT_common_dtype, &common_dtype},
        {NPY_DT_discover_descr_from_pyobject,
         &unit_discover_descriptor_from_pyobject},
        /* The header is wrong on main :(, so we add 1 */
        {NPY_DT_setitem, &sampledtype_setitem},
        {NPY_DT_getitem, &sampledtype_getitem},
        {NPY_DT_ensure_canonical, &sampledtype_ensure_canonical},
#endif
    {0, NULL}};

static PyMemberDef SampleDType_members[] = {
    {NULL},
};

/*
 * This is the basic things that you need to create a Python Type/Class in C.
 * However, there is a slight difference here because we create a
 * PyArray_DTypeMeta, which is a larger struct than a typical type.
 * (This should get a bit nicer eventually with Python >3.11.)
 */
PyArray_DTypeMeta SampleDType = {
    {{
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "unytdtype.SampleDType",
        .tp_basicsize = sizeof(SampleDTypeObject),
#if 0 // TODO: Implement new/del functions
                .tp_new = sampledtype_new,
                .tp_dealloc = (destructor)sampletype_dealloc,
                .tp_repr = (reprfunc)sampletype_repr,
                .tp_str = (reprfunc)sampledtype_repr,
#endif
        .tp_members = SampleDType_members,
    }},
    /* rest, filled in during DTypeMeta initialization */
};

int init_sample_dtype(void) {
  /*
   * To create our DType, we have to use a "Spec" that tells NumPy how to
   * do it.  You first have to create a static type, but see the note there!
   */
  PyArrayMethod_Spec **casts = NULL; // TODO: Implement get_casts();

  PyArrayDTypeMeta_Spec SampleDType_DTypeSpec = {
      .flags = (NPY_DT_PARAMETRIC | NPY_DT_NUMERIC),
      .casts = casts,
      .typeobj = SampleScalar_Type,
      .slots = SampleDType_Slots,
  };
  /* Loaded dynamically, so may need to be set here: */
  ((PyObject *)&SampleDType)->ob_type = &PyArrayDTypeMeta_Type;
  ((PyTypeObject *)&SampleDType)->tp_base = &PyArrayDescr_Type;
  if (PyType_Ready((PyTypeObject *)&SampleDType) < 0) {
    return -1;
  }

  if (PyArrayInitDTypeMeta_FromSpec(&SampleDType, &SampleDType_DTypeSpec) < 0) {
    return -1;
  }

  return 0;
}
