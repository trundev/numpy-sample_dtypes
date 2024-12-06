#include <Python.h>
#include <structmember.h>

#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"

#include "casts.h"
#include "dtype.h"

PyTypeObject *SampleScalar_Type = NULL;

/*
 * Internal helper to create new instances
 */
SampleDTypeObject *new_sampledtype_instance(PyObject *sample_scalar) {
  SampleDTypeObject *new = (SampleDTypeObject *)PyArrayDescr_Type.tp_new(
      (PyTypeObject *)&SampleDType, NULL, NULL);
  if (new == NULL) {
    return NULL;
  }

  // TODO: Pick correct element size and alignment
  printf("TODO: %s\n", __func__);
  Py_INCREF(sample_scalar);
  new->sample_scalar = sample_scalar;
  new->base.elsize = sizeof(ScalarElementType);
  new->base.alignment = _Alignof(ScalarElementType);

  return new;
}

/*
 * Callbacks for SampleDType_Slots
 */
static PyArray_Descr *sampledtype_discover_descriptor_from_pyobject(
    PyArray_DTypeMeta *NPY_UNUSED(cls), PyObject *obj) {
  printf("TODO: %s\n", __func__);

  if (Py_TYPE(obj) != SampleScalar_Type) {
    PyErr_SetString(PyExc_TypeError,
                    "Can only store SampleScalar in a SampleDType array.");
    return NULL;
  }

  SampleDTypeObject *new = NULL; // TODO: new_sampledtype_instance(...);
  if (new == NULL) {
    return NULL;
  }
  return &new->base;
}

static PyArray_DTypeMeta *common_dtype(PyArray_DTypeMeta *cls,
                                       PyArray_DTypeMeta *other) {
  printf("TODO: %s\n", __func__);

  Py_INCREF(Py_NotImplemented);
  return (PyArray_DTypeMeta *)Py_NotImplemented;
}

static SampleDTypeObject *common_instance(SampleDTypeObject *dtype1,
                                          SampleDTypeObject *dtype2) {
  printf("TODO: %s\n", __func__);

  // TODO: on mismatch, just fail
  if (dtype1->sample_scalar != dtype2->sample_scalar) {
    return NULL;
  }
  // TODO: For now just return the first one.
  Py_INCREF(dtype1);
  return dtype1;
}

static SampleDTypeObject *
sampledtype_ensure_canonical(SampleDTypeObject *self) {
  printf("TODO: %s\n", __func__);

  Py_INCREF(self);
  return self;
}

static int sampledtype_setitem(SampleDTypeObject *descr, PyObject *obj,
                               char *dataptr) {
  printf("TODO: %s, target elsise %lld\n", __func__, descr->base.elsize);

  if (Py_TYPE(obj) != SampleScalar_Type) {
    PyErr_SetString(PyExc_TypeError,
                    "Can only store SampleScalar in a SampleDType array.");
    return -1;
  }

  // TODO: ...
  npy_intp elsize = descr->base.elsize;
  npy_intp alignment = descr->base.alignment;

  PyErr_Format(PyExc_NotImplementedError, "TODO: %s", __func__);

  return -1;
}

static PyObject *sampledtype_getitem(SampleDTypeObject *descr, char *dataptr) {
  printf("TODO: %s, source elsize %lld\n", __func__, descr->base.elsize);

  PyErr_Format(PyExc_NotImplementedError, "TODO: %s", __func__);

  return NULL;
}

static PyType_Slot SampleDType_Slots[] = {
    {NPY_DT_discover_descr_from_pyobject,
     &sampledtype_discover_descriptor_from_pyobject},
    {NPY_DT_common_dtype, &common_dtype},
    {NPY_DT_common_instance, &common_instance},
    {NPY_DT_ensure_canonical, &sampledtype_ensure_canonical},
    {NPY_DT_setitem, &sampledtype_setitem},
    {NPY_DT_getitem, &sampledtype_getitem},
    {0, NULL}};

static PyObject *sampledtype_new(PyTypeObject *NPY_UNUSED(cls), PyObject *args,
                                 PyObject *kwds) {
  static char *kwlist[] = {"scalar", NULL};
  PyObject *sample_scalar = NULL;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!", kwlist, SampleScalar_Type,
                                   &sample_scalar)) {
    return NULL;
  }

  if (sample_scalar == NULL) {
    sample_scalar =
        PyObject_CallObject(&SampleScalar_Type->ob_base.ob_base, NULL);
    if (sample_scalar == NULL) {
      return NULL;
    }
  } else {
    Py_INCREF(sample_scalar);
  }
  // TODO: Parse scalar_str, also check unytdtype/src/dtype.c: unytdtype_new()

  SampleDTypeObject *new = new_sampledtype_instance(sample_scalar);
  Py_DECREF(sample_scalar);
  if (new == NULL) {
    return NULL;
  }
  return &new->base.ob_base;
}

static void sampledtype_dealloc(PyObject *self) {
  printf("%s\n", __func__);

  // TODO: Deallocate what is needed
  SampleDTypeObject *obj = (SampleDTypeObject *)self;
  Py_CLEAR(obj->sample_scalar);

  PyArrayDescr_Type.tp_dealloc(self);
}

static PyObject *sampledtype_repr(SampleDTypeObject *self) {
  printf("TODO: %s, sample_scalar type '%s'\n", __func__,
         Py_TYPE(self->sample_scalar)->tp_name);

  // TODO: Use '%R', self->obj
  PyObject *res = PyUnicode_FromFormat("SampleDType(<TODO>)");
  return res;
}

static PyMemberDef SampleDType_members[] = {
    {"sample_scalar", T_OBJECT_EX, offsetof(SampleDTypeObject, sample_scalar),
     READONLY, "the scalar"},
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
        PyVarObject_HEAD_INIT(NULL, 0).tp_name = "sample_dtypes.SampleDType",
        .tp_basicsize = sizeof(SampleDTypeObject),
        .tp_new = sampledtype_new,
        .tp_dealloc = sampledtype_dealloc,
        .tp_repr = (reprfunc)sampledtype_repr,
        .tp_str = (reprfunc)sampledtype_repr,
        .tp_members = SampleDType_members,
    }},
    /* rest, filled in during DTypeMeta initialization */
};

int init_sample_dtype(void) {
  /*
   Code based on `numpy_quaddtype` sample
   https://github.com/numpy/numpy-user-dtypes/blob/116a605/quaddtype/numpy_quaddtype/src/dtype.c#L238
   */
  PyArrayMethod_Spec **casts = init_casts();
  if (!casts)
    return -1;

  PyArrayDTypeMeta_Spec SampleDType_DTypeSpec = {
      .flags = (NPY_DT_PARAMETRIC | NPY_DT_NUMERIC),
      .casts = casts,
      .typeobj = SampleScalar_Type,
      .slots = SampleDType_Slots,
  };

  ((PyObject *)&SampleDType)->ob_type = &PyArrayDTypeMeta_Type;

  ((PyTypeObject *)&SampleDType)->tp_base = &PyArrayDescr_Type;

  int res = PyType_Ready((PyTypeObject *)&SampleDType);

  if (res == 0) {
    res = PyArrayInitDTypeMeta_FromSpec(&SampleDType, &SampleDType_DTypeSpec);
  }

  free_casts();

  return res;
}
