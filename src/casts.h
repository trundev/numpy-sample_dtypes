#ifndef _NPY_CASTS_H
#define _NPY_CASTS_H

#include "numpy/dtype_api.h"
#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

PyArrayMethod_Spec **init_casts(void);

void free_casts(void);

#ifdef __cplusplus
}
#endif

#endif /* _NPY_CASTS_H */
