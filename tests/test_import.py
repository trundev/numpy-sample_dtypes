"""Test basic module import"""


def test_import():
  """Test importing of root module"""
  import sample_dtypes

  print(
      f'sample_dtypes ({sample_dtypes.__name__}): {sample_dtypes.__version__}'
  )
  print('* ' + '\n* '.join(sample_dtypes.__path__))


def test_import_ext():
  """Test importing of the extension module"""
  from sample_dtypes import _sample_dtypes_main as pyd

  print(f'sample_dtypes._sample_dtypes_main ({pyd.__name__}):')
  print('*', pyd.__file__)
