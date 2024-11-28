"""Test register_dtype()"""


def test_register_dtype():
  from sample_dtypes import _sample_dtypes_ext as pyd

  print(
      f'_register_dtype_ext: {type(pyd._register_dtype_ext)}, {type(pyd.__doc__)}'
  )

  import sample_dtypes

  dtype = sample_dtypes.register_dtype('type_name', int, 4)
  assert dtype.__module__ is sample_dtypes
