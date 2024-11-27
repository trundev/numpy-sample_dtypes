#ifndef SAMPLE_DTYPES_COMMON_H_
#define SAMPLE_DTYPES_COMMON_H_

namespace sample_dtypes {

struct PyDecrefDeleter {
  void operator()(PyObject *p) const { Py_DECREF(p); }
};

// Safe container for an owned PyObject. On destruction, the reference count of
// the contained object will be decremented.
using Safe_PyObjectPtr = std::unique_ptr<PyObject, PyDecrefDeleter>;
inline Safe_PyObjectPtr make_safe(PyObject *object) {
  return Safe_PyObjectPtr(object);
}

} // namespace sample_dtypes

#endif // SAMPLE_DTYPES_COMMON_H_
