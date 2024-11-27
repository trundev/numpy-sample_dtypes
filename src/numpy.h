#ifndef SAMPLE_DTYPES__NUMPY_H_
#define SAMPLE_DTYPES__NUMPY_H_

#ifdef PyArray_Type
#error "Numpy cannot be included before numpy.h."
#endif

// We import_array in the sample_dtypes init function only.
#define PY_ARRAY_UNIQUE_SYMBOL _sample_dtypes_numpy_api
#ifndef SAMPLE_DTYPES_IMPORT_NUMPY
#define NO_IMPORT_ARRAY
#endif

// Place `<locale>` before <Python.h> to avoid build failure in macOS.
#include <locale>

#include <Python.h>

#include "numpy/arrayobject.h"
#include "numpy/arrayscalars.h"
#include "numpy/ufuncobject.h"

namespace sample_dtypes {

// Import numpy.  This wrapper function exists so that the
// PY_ARRAY_UNIQUE_SYMBOL can be safely defined in a .cc file to
// avoid weird linking issues.  Should be called only from our
// module initialization function.
void ImportNumpy();

} // namespace sample_dtypes

#endif // SAMPLE_DTYPES__NUMPY_H_
