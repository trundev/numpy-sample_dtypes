#ifndef _NPY_DTYPE_H
#define _NPY_DTYPE_H

typedef struct {
  PyArray_Descr base;
  PyObject *sample_scalar; // Pointer to scalar.SampleScalar
} SampleDTypeObject;

typedef struct {
  PyArray_DTypeMeta base;
} SampleDType_type;

extern PyArray_DTypeMeta SampleDType;
extern PyTypeObject *SampleScalar_Type;

// TODO: Pick the correct element based on ScalarType
typedef long double ScalarElementType;

int init_sample_dtype(void);

#endif /*_NPY_DTYPE_H*/
