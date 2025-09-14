"""Test object casting"""

import itertools
import numpy as np
import pytest
import sample_dtypes


def test_sample_to_sample():
  """Same type cast (copy operation)"""
  scalar = sample_dtypes.SampleScalar(dtype=np.uint16)
  src_arr: np.ndarray = np.empty((3, 4), sample_dtypes.SampleDType(scalar))
  dst_arr: np.ndarray = np.empty_like(src_arr)
  test_data = np.arange(scalar._ndarr.size) + 10
  scalar._ndarr.flat[...] = test_data

  print('* Cast with implicit broadcast of scalar to whole array')
  src_arr.flat[...] = scalar

  print('* Cast between arrays of same shape')
  dst_arr[...] = src_arr
  np.testing.assert_equal(dst_arr[1, 1]._ndarr.flat, test_data)

  print('* Cast between array slices of same shape and stride')
  dst_arr: np.ndarray = np.empty_like(src_arr, shape=(4, 5))
  dst_arr[:-1, :-1] = src_arr
  np.testing.assert_equal(dst_arr[1, 1]._ndarr.flat, test_data)

  print(
      '* Cast between arrays slices with different `in_stride` vs. `out_stride`'
  )
  dst_arr: np.ndarray = np.empty_like(src_arr, shape=(4, 5))
  dst_arr.T[:-2, :] = src_arr
  dst_arr[:, :-2] = src_arr.T
  np.testing.assert_equal(dst_arr[1, 1]._ndarr.flat, test_data)


@pytest.mark.parametrize(
    'in_dtype, out_dtype',
    itertools.permutations([np.uint8, np.float32, object], 2),
)
def test_covert_internal_dtype(in_dtype: np.dtype, out_dtype: np.dtype):
  """Same type cast between different scalar-internal dtypes"""
  in_scalar = sample_dtypes.SampleScalar(dtype=in_dtype)
  in_arr: np.ndarray = np.empty(3, sample_dtypes.SampleDType(in_scalar))
  out_arr: np.ndarray = np.empty_like(
      in_arr,
      dtype=sample_dtypes.SampleDType(
          sample_dtypes.SampleScalar(dtype=out_dtype)
      ),
  )

  in_scalar._ndarr.flat[...] = np.arange(in_scalar._ndarr.size) * 5 + 3
  in_arr.flat[...] = in_scalar
  out_arr = in_arr
  # TODO: This needs NPY_ITER_REFS_OK
  # for out_item in np.nditer(out_arr):
  for idx in np.ndindex(out_arr.shape):
    np.testing.assert_equal(out_arr[idx]._ndarr, in_scalar._ndarr)
