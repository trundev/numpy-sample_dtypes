"""Test basic module import"""


def test_import():
  import sample_dtypes

  print(
      f'sample_dtypes ({sample_dtypes.__name__}): {sample_dtypes.__version__}'
  )
  print('* ' + '\n* '.join(sample_dtypes.__path__))


def test_import_ext():
  from sample_dtypes import _sample_dtypes_ext as pyd

  print(f'sample_dtypes._sample_dtypes_ext ({pyd.__name__}):')
  print('*', pyd.__file__)
