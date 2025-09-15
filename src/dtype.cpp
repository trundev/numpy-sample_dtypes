#include <Python.h>
#include <format>
#include <structmember.h>

#define NO_IMPORT_ARRAY
#define NO_IMPORT_UFUNC
#include "numpy/arrayobject.h"
#include "numpy/dtype_api.h"
#include "numpy/ndarraytypes.h"

#include "casts.h"
#include "dtype.h"

/*
 * Internal helper to create new instances
 */
SampleDTypeObject *new_sampledtype_instance(SampleDType_type *dtype_obj,
                                            py::object py_scalar) {
#if __DEBUG
  py::print(__func__, py_scalar);
#endif

  // Take element size and alignment from SampleTypeScalar object
  int elsize, alignment;
  bool hasobject;
  try {
    elsize = py_scalar.attr("elsize").cast<int>();
    alignment = py_scalar.attr("alignment").cast<int>();
    hasobject = py_scalar.attr("hasobject").cast<bool>();
  } catch (const py::error_already_set &e) {
    // Handle "AttributeError: object has no attribute" and other Python
    // exceptions
    PyErr_SetObject(e.type().ptr(), e.value().ptr());
    return nullptr;
  } catch (const py::cast_error &e) {
    // Handle "Unable to cast Python instance of type" exception
    PyErr_SetString(PyExc_TypeError, e.what());
    return nullptr;
  }

  // Allocate final DType object
  SampleDTypeObject *new_obj = (SampleDTypeObject *)PyArrayDescr_Type.tp_new(
      &dtype_obj->base.super.ht_type, nullptr, nullptr);
  if (new_obj == nullptr) {
    return nullptr;
  }

  new_obj->py_scalar = py_scalar;
  if (hasobject) {
    // TODO: Other flags from NPY_OBJECT_DTYPE_FLAGS
    new_obj->base.flags |= NPY_ITEM_HASOBJECT | NPY_NEEDS_INIT;
  }
  new_obj->base.elsize = elsize;
  new_obj->base.alignment = alignment;

  return new_obj;
}

/*
 * Callbacks for SampleDType_Slots
 */
static PyArray_Descr *
sampledtype_discover_descriptor_from_pyobject(PyArray_DTypeMeta *cls,
                                              PyObject *obj) {
  auto dtype_cls = reinterpret_cast<SampleDType_type *>(cls);
  assert(&dtype_cls->base.scalar_type->ob_base.ob_base ==
         dtype_cls->scalar_type.ptr());

  auto py_obj = py::reinterpret_borrow<py::object>(obj);
#if __DEBUG
  py::print(__func__, py_obj);
#endif

  if (!py::isinstance(py_obj, dtype_cls->scalar_type)) {
    PyErr_SetObject(PyExc_TypeError,
                    py::str("Can only store object of type {} in a SampleDType "
                            "array, got {} instead")
                        .format(dtype_cls->scalar_type, py_obj)
                        .ptr());
    return nullptr;
  }

  SampleDTypeObject *new_obj = new_sampledtype_instance(dtype_cls, py_obj);
  if (new_obj == nullptr) {
    return nullptr;
  }
  return &new_obj->base;
}

static PyArray_DTypeMeta *common_dtype(PyArray_DTypeMeta *cls,
                                       PyArray_DTypeMeta *other) {
#if __DEBUG
  py::print("TODO:", __func__);
#endif

  Py_INCREF(Py_NotImplemented);
  return (PyArray_DTypeMeta *)Py_NotImplemented;
}

static SampleDTypeObject *common_instance(SampleDTypeObject *dtype1,
                                          SampleDTypeObject *dtype2) {
#if __DEBUG
  py::print("TODO:", __func__);
#endif

  // TODO: on mismatch, just fail
  if (dtype1->py_scalar != dtype2->py_scalar) {
    return nullptr;
  }
  // TODO: For now just return the first one.
  Py_INCREF(dtype1);
  return dtype1;
}

static SampleDTypeObject *
sampledtype_ensure_canonical(SampleDTypeObject *self) {
#if __DEBUG
  py::print("TODO:", __func__);
#endif

  Py_INCREF(self);
  return self;
}

static int sampledtype_setitem(SampleDTypeObject *descr, PyObject *obj,
                               char *dataptr) {
  auto py_obj = py::reinterpret_borrow<py::object>(obj);
#if __DEBUG
  py::print(__func__, ", elsise", descr->base.elsize, ", type_num",
            descr->base.type_num, ", py_obj", py_obj);
#endif

  // scalar_type is also in PyArray_Descr::typeobj
  auto scalar_type =
      py::reinterpret_borrow<py::type>(&descr->base.typeobj->ob_base.ob_base);
  if (!py::isinstance(py_obj, scalar_type)) {
    PyErr_SetObject(PyExc_TypeError,
                    py::str("Can only store object of type {} in a SampleDType "
                            "array, got {} instead")
                        .format(descr->py_scalar, py_obj)
                        .ptr());
    return -1;
  }

  int res;
  try {
    res = descr->py_scalar.attr("setitem")(py_obj, (Py_ssize_t)dataptr)
              .cast<int>();
  } catch (const py::error_already_set &e) {
    // Handle exceptions from Python
    PyErr_SetObject(e.type().ptr(), e.value().ptr());
    return -1;
  }
  return res;
}

static PyObject *sampledtype_getitem(SampleDTypeObject *descr, char *dataptr) {
#if __DEBUG
  py::print(__func__, ", elsise", descr->base.elsize, ", type_num",
            descr->base.type_num);
#endif

  py::object py_scalar;
  try {
    py_scalar = descr->py_scalar.attr("getitem")((Py_ssize_t)dataptr);
  } catch (const py::error_already_set &e) {
    // Handle exceptions from Python
    PyErr_SetObject(e.type().ptr(), e.value().ptr());
    return nullptr;
  }
  return py_scalar.release().ptr();
}

static int sampledtype_clear_loop(void *NPY_UNUSED(traverse_context),
                                  const PyArray_Descr *_descr, char *data,
                                  npy_intp size, npy_intp stride,
                                  NpyAuxData *NPY_UNUSED(auxdata)) {
  const SampleDTypeObject *descr = (const SampleDTypeObject *)_descr;
#if __DEBUG
  py::print(__func__, ", elsise", descr->base.elsize, ", type_num",
            descr->base.type_num, ", data", (Py_ssize_t)data, ", size", size,
            ", stride", stride);
#endif

  int res;
  try {
    res = descr->py_scalar.attr("clear_loop")((Py_ssize_t)data, size, stride)
              .cast<int>();
  } catch (const py::error_already_set &e) {
    // Handle exceptions from Python
    PyErr_SetObject(e.type().ptr(), e.value().ptr());
    return -1;
  }
  return res;
}

static int sampledtype_get_clear_loop(void *NPY_UNUSED(traverse_context),
                                      const SampleDTypeObject *descr,
                                      int aligned, npy_intp fixed_stride,
                                      PyArrayMethod_TraverseLoop **out_loop,
                                      NpyAuxData **NPY_UNUSED(out_auxdata),
                                      NPY_ARRAYMETHOD_FLAGS *flags) {
#if __DEBUG
  py::print(__func__, ", elsize", descr->base.elsize, ", type_num",
            descr->base.type_num, ", aligned", aligned, ", fixed_stride",
            fixed_stride);
#endif

  *out_loop = &sampledtype_clear_loop;
  *flags = NPY_METH_NO_FLOATINGPOINT_ERRORS;
  return 0;
}

static PyType_Slot SampleDType_Slots[] = {
    {NPY_DT_discover_descr_from_pyobject,
     (void *)&sampledtype_discover_descriptor_from_pyobject},
    {NPY_DT_common_dtype, (void *)&common_dtype},
    {NPY_DT_common_instance, (void *)&common_instance},
    {NPY_DT_ensure_canonical, (void *)&sampledtype_ensure_canonical},
    {NPY_DT_setitem, (void *)&sampledtype_setitem},
    {NPY_DT_getitem, (void *)&sampledtype_getitem},
    {NPY_DT_get_clear_loop, (void *)&sampledtype_get_clear_loop},
    {0, nullptr}};

static PyObject *sampledtype_new(PyTypeObject *cls, PyObject *args,
                                 PyObject *kwds) {
  py::gil_scoped_acquire gil; // Ensure GIL is held

  auto dtype_cls = reinterpret_cast<SampleDType_type *>(cls);
  assert(&dtype_cls->base.scalar_type->ob_base.ob_base ==
         dtype_cls->scalar_type.ptr());

  // TODO: More consistent argument parsing
  auto py_args = py::reinterpret_borrow<py::tuple>(args);
  // auto py_kwds = py::reinterpret_borrow<py::dict>(kwds);
  py::object py_scalar;
  if (py_args.size() > 0) {
    py_scalar = py_args[0];
    if (!py::isinstance(py_scalar, dtype_cls->scalar_type)) {
      PyErr_SetObject(PyExc_TypeError, py::str("Expected object of type {}")
                                           .format(dtype_cls->scalar_type)
                                           .ptr());
      return nullptr;
    }
  }

  if (!py_scalar || py_scalar.is_none()) {
    py_scalar = dtype_cls->scalar_type();
    if (py_scalar.is_none()) {
      PyErr_SetObject(PyExc_TypeError, py::str("Invalid object created by {}")
                                           .format(dtype_cls->scalar_type)
                                           .ptr());
      return nullptr;
    }
  }

  SampleDTypeObject *new_obj = new_sampledtype_instance(dtype_cls, py_scalar);
  if (new_obj == nullptr) {
    return nullptr;
  }
  return &new_obj->base.ob_base;
}

static void sampledtype_dealloc(PyObject *self) {
  py::gil_scoped_acquire gil; // Ensure GIL is held

  SampleDTypeObject *obj = reinterpret_cast<SampleDTypeObject *>(self);
#if __DEBUG
  printf("%s, py_scalar ", __func__);
  // If this is invoked during Python shutdown, py::print() will crash
  // which messes up pytest results
  try {
    py::print(obj->py_scalar);
  } catch (const py::error_already_set &e) {
  }
#endif

  // Deallocate our extra members
  obj->py_scalar = py::object(); // Like Py_CLEAR

  PyArrayDescr_Type.tp_dealloc(self);
}

static PyObject *sampledtype_repr(SampleDTypeObject *self) {
#if __DEBUG
  py::print(__func__, ", py_scalar", self->py_scalar);
#endif

  auto repr_str = py::str("SampleDType(elsize={}, alignment={}, py_scalar={})")
                      .format(self->base.elsize, self->base.alignment,
                              py::repr(self->py_scalar));
  return repr_str.release().ptr();
}

static PyMemberDef SampleDType_members[] = {
    {"py_scalar", T_OBJECT_EX,
     offsetof(SampleDTypeObject, py_scalar), // TODO: Fix this
     READONLY, "the scalar"},
    {nullptr},
};

/*
 * This is the basic things that you need to create a Python Type/Class in C.
 * However, there is a slight difference here because we create a
 * PyArray_DTypeMeta, which is a larger struct than a typical type.
 * (This should get a bit nicer eventually with Python >3.11.)
 */
SampleDType_type SampleDType = {
    .base =
        {
            // PyArray_DTypeMeta
            .super =
                {
                    // PyHeapTypeObject
                    .ht_type =
                        {
                            // PyTypeObject
                            .ob_base =
                                PyVarObject_HEAD_INIT(nullptr, 0).tp_name =
                                    "sample_dtypes.SampleDType",
                            .tp_basicsize = sizeof(SampleDTypeObject),
                            .tp_dealloc = sampledtype_dealloc,
                            .tp_repr = (reprfunc)sampledtype_repr,
                            .tp_str = (reprfunc)sampledtype_repr,
                            .tp_members = SampleDType_members,
                            .tp_new = sampledtype_new,
                        },
                },
        },
    // CHECKME: This crashes with
    // pybind11::handle::inc_ref() is being called while the GIL is either not
    // held or invalid.
    //.scalar_type = py::none(),
};

py::object init_dtype(py::type scalar_type) {
#if __DEBUG
  py::print("Initializing DType from:", scalar_type);
#endif

  // Ensure scalar_type holds a PyTypeObject
  if (!py::isinstance<py::type>(scalar_type)) {
    throw py::type_error(
        py::str("Expected a type object, got {} instead").format(scalar_type));
  }

  /*
   * Prepare PyArrayDTypeMeta_Spec
   */
  dtype_casts casts;
  PyArrayMethod_Spec **casts_spec = casts.init(scalar_type);
  if (casts_spec == nullptr) {
    throw std::bad_alloc();
  }

  PyArrayDTypeMeta_Spec SampleDType_DTypeSpec = {
      // Note: No need of Py_INCREF() - the structure is in stack,
      // ref-count will be increased by PyArrayInitDTypeMeta_FromSpec()
      .typeobj = reinterpret_cast<PyTypeObject *>(scalar_type.ptr()),
      .flags = (NPY_DT_PARAMETRIC | NPY_DT_NUMERIC),
      .casts = casts_spec,
      .slots = SampleDType_Slots,
  };

#if 1
  /*
   * Use static PyArray_DTypeMeta object (as recommended)
   */
  auto dtype_obj = &SampleDType;
#else
  /*
   * Create dynamic PyArray_DTypeMeta
   */
  PyType_Spec spec = {
      .name = "sample_dtypes.SampleDType",
      .basicsize = sizeof(SampleDTypeObject),
      .flags = Py_TPFLAGS_DEFAULT,
      .slots = my_slots // array of PyType_Slot
  };

  PyObject *result_object = PyType_FromSpec(&spec);
  if (result_object == nullptr) {
    return py::none();
  }
  auto dtype_obj = reinterpret_cast<SampleDType_type *>(result_object);
#endif

  dtype_obj->base.super.ht_type.ob_base.ob_base.ob_type =
      &PyArrayDTypeMeta_Type;
  dtype_obj->base.super.ht_type.tp_base = &PyArrayDescr_Type;
  dtype_obj->scalar_type = scalar_type;

  int res = PyType_Ready(&dtype_obj->base.super.ht_type);
  if (res != 0) {
    return py::none();
  }
  res = PyArrayInitDTypeMeta_FromSpec(&dtype_obj->base, &SampleDType_DTypeSpec);
  if (res != 0) {
    return py::none();
  }

  // Return a pybind11::object instance
  return py::reinterpret_borrow<py::object>(
      &dtype_obj->base.super.ht_type.ob_base.ob_base);
}
