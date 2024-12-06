project(
  'sample_dtypes',
  'c', 'cpp', default_options : ['cpp_std=c++20', 'b_pie=true']
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

c_args = [
  '-DPY_ARRAY_UNIQUE_SYMBOL=sample_dtypes_ARRAY_API',
  '-DPY_UFUNC_UNIQUE_SYMBOL=sample_dtypes_UFUNC_API',
  '-DNPY_NO_DEPRECATED_API=NPY_2_0_API_VERSION',
  '-DNPY_TARGET_VERSION=NPY_2_0_API_VERSION',
]

srcs = [
  'src/casts.cpp',
  'src/casts.h',
  'src/dtype.c',
  'src/dtype.h',
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
  include_directories: includes,
  c_args: c_args,
  cpp_args: c_args,
)
