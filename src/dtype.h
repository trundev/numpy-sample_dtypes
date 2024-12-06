#ifndef _NPY_DTYPE_H
#define _NPY_DTYPE_H

typedef struct {
  PyArray_Descr base;

  // TODO: Add extra members
  int _reserved[2];
} SampleDTypeObject;

typedef struct {
  PyArray_DTypeMeta base;
} SampleDType_type;

extern PyArray_DTypeMeta SampleDType;
extern PyTypeObject *SampleScalar_Type;

SampleDTypeObject *new_sampledtype_instance(PyObject *unit);

int init_sample_dtype(void);

#endif /*_NPY_DTYPE_H*/
