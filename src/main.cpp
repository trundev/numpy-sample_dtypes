#include "numpy/arrayobject.h"
#include "numpy/ufuncobject.h"

#include "dtype.h"

static void *init_numpy() {
  import_array();
  // TODO: import_umath();
  return nullptr;
}

PYBIND11_MODULE(_dtypes_ext, mod) {
#if __DEBUG
  py::print(__func__, ": " __DATE__ ", " __TIME__ ": ", mod);
#endif

  init_numpy();

  mod.doc() = "Experimental NumPy Custom Data Type module, see:\n"
              " https://numpy.org/doc/stable/reference/c-api/"
              "array.html#custom-data-types";
  mod.def("init_dtype", &init_dtype,
          "A function to create NumPy Custom Data Type");
}
