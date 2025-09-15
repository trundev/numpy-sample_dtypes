"""Test internal object reference counting

NOTE:
  Most of these tests fail when debugging, because "ms-python.debugpy" takes
  extra local object ownership.
"""

import sys
import numpy as np
import pytest
import sample_dtypes


def test_dtype_refcnt():
  """Test SampleScalar internal dtype reference counts"""
  # Use non-singleton (non-immortal) dtypes to have valid ref-counts
  dtype = np.dtype([('a', np.int16)])
  assert (
      sys.getrefcount(dtype) == 2
  ), 'Unexpected initial ref-count for non-immortal object'

  scalar = sample_dtypes.scalar.Scalar(dtype=dtype)
  arr: np.ndarray = np.empty(3, sample_dtypes.SampleDType(scalar))
  arr[1] = scalar

  del arr, scalar
  assert sys.getrefcount(dtype) == 2, 'Unexpected dtype ref-count'


@pytest.mark.parametrize(
    'dtype',
    [
        # Simple object dtype
        np.dtype(object),
        # Array of objects
        np.dtype((object, 3)),
        # Structured dtype to include object (alignment issue)
        np.dtype([('i', np.int16), ('obj', object)]),
        # Complex/compound dtype with Structured and Array elements
        np.dtype(
            ([('i', np.uint16), ('obj_arr', [('obj', object)], (2, 3))], 4)
        ),
    ],
)
def test_custom_as_ctypes_type(dtype):
  """Test scalar.custom_as_ctypes_type conversion, see np.ctypeslib.as_ctypes_type"""
  import ctypes
  from sample_dtypes.scalar import custom_as_ctypes_type

  ct_type = custom_as_ctypes_type(dtype)
  assert (
      ctypes.sizeof(ct_type) == dtype.itemsize
  ), 'Converted size does NOT match'

  shape = (3, 2)
  src = np.empty(shape, dtype=dtype)
  tgt = np.empty(shape, dtype=dtype)
  # Generate some different source values
  src.flat[...] = (np.arange(src.size) + 10).astype(src.dtype)

  # Create view that uses `tgt` data-buffer
  ct_array = ctypes.cast(
      tgt.__array_interface__['data'][0], ctypes.POINTER(ct_type)
  )
  tgt_view = np.ctypeslib.as_array(ct_array, shape=shape)
  # Replace view content, expect same content in `tgt`
  tgt_view[...] = src
  np.testing.assert_array_equal(tgt, src, 'Mismatch between array and its view')


def test_sub_dtype_refcnt():
  """Test SampleScalar internal dtype reference counts"""
  # Use generic-object dtype to test sub-object ownership transfer
  scalar = sample_dtypes.ScalarType(shape=(2, 2), dtype=object)
  arr: np.ndarray = np.empty(3, sample_dtypes.SampleDType(scalar))
  scalar = scalar._create()

  obj = f'string {1}'  # Dynamic (non-immortal) string object
  scalar._ndarr[0, 1] = obj
  # Expect 3 references to the string: `obj`, `scalar._ndarr` and `getrefcount`
  assert (
      sys.getrefcount(obj) == 3
  ), 'Unexpected ref-count for non-immortal object'

  # Do NPY_DT_setitem
  arr[1] = scalar
  # Expect 4 references to the string: ones from above and `arr._ndarr`
  assert sys.getrefcount(obj) == 4, 'Unexpected ref-count after assignment'
  assert obj is arr[1]._ndarr[0, 1], 'Same object must be referenced by arr'
  arr[2] = scalar
  # Expect 5 references to the string: ones from above and extra `arr._ndarr`
  assert sys.getrefcount(obj) == 5, 'Unexpected ref-count after assignment'
  assert obj is arr[2]._ndarr[0, 1], 'Same object must be referenced by arr'

  # Drop references one by one
  del scalar
  # Expect `scalar._ndarr` reference to be dropped
  assert (
      sys.getrefcount(obj) == 4
  ), 'Unexpected ref-count after scalar reference deletion'
  del arr
  # Expect two `arr._ndarr` references to be dropped
  assert (
      sys.getrefcount(obj) == 2
  ), 'Unexpected ref-count after ndarray reference deletion'
