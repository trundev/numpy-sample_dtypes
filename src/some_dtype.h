#ifndef SAMPLE_DTYPES_SOME_DTYPE_H_
#define SAMPLE_DTYPES_SOME_DTYPE_H_

namespace sample_dtypes {

template <int N, typename T> struct some_dtype {
  T v[N];
};

} // namespace sample_dtypes

#endif // SAMPLE_DTYPES_SOME_DTYPE_H_
