"""Test internal object reference counting"""

import sys
import numpy as np


def test_dtype_refcnt():
  """Test SampleScalar internal dtype reference counts

  NOTE: This fails when debugging as of extra ownership of local object
  """
  import sample_dtypes

  # Use non-singleton (non-immortal) dtypes to have valid ref-counts
  dtype = np.dtype([('a', np.int16)])
  assert (
      sys.getrefcount(dtype) == 2
  ), 'Unexpected initial ref-count for non-immortal object'

  scalar = sample_dtypes.SampleScalar(dtype=dtype)
  arr: np.ndarray = np.empty(3, sample_dtypes.SampleDType(scalar))
  arr[1] = scalar

  del arr, scalar
  assert sys.getrefcount(dtype) == 2, 'Unexpected dtype ref-count'
