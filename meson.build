project(
  'sample_dtypes',
  'c',
)

py_mod = import('python')
py = py_mod.find_installation()

incdir_numpy = run_command(py,
  [
    '-c',
    'import numpy; print(numpy.get_include())'
  ],
  check: true
).stdout().strip()

includes = include_directories(
  [
    incdir_numpy,
    'src'
  ]
)

srcs = [
  'src/dtype.c',
  'src/main.c',
]

py.install_sources(
  [
    'sample_dtypes/__init__.py',
    'sample_dtypes/scalar.py',
  ],
  subdir: 'sample_dtypes',
  pure: false
)

py.extension_module(
  '_sample_dtypes_main',
  srcs,
  install: true,
  subdir: 'sample_dtypes',
  include_directories: includes
)
