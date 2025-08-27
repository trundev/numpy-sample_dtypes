"""Test DType creation/instantiation"""

import numpy as np
import pytest


def test_instantiate():
  """Test PyTypeObject.tp_new (sampledtype_new)"""
  import sample_dtypes

  print('* no args')
  print('  *', sample_dtypes.SampleDType())

  # Not allowed arguments types
  print('* string arg')
  with pytest.raises(TypeError):
    print('  *', sample_dtypes.SampleDType('123'))
  print('* string SampleScalar class')
  with pytest.raises(TypeError):
    print('  *', sample_dtypes.SampleDType(sample_dtypes.SampleScalar))

  print('* string SampleScalar object')
  print('  *', sample_dtypes.SampleDType(sample_dtypes.SampleScalar()))


def test_setitem():
  """Test NPY_DT_setitem (sampledtype_setitem)"""
  import sample_dtypes

  print('* create empty array')
  scalar = sample_dtypes.SampleScalar(dtype=int)

  print('  * set compatible value')
  arr: np.ndarray = np.empty(3, sample_dtypes.SampleDType(scalar))
  scalar._ndarr.flat[...] = np.arange(scalar._ndarr.size) + 10
  arr[1] = scalar
  np.testing.assert_equal(arr[1]._ndarr, scalar._ndarr)

  print('  * set incompatible value')
  arr = np.empty(3, sample_dtypes.SampleDType)
  with pytest.raises(ValueError):
    arr[0] = scalar
