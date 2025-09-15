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
incdir_pybind11 = run_command(py,
  [
    '-c',
    'import pybind11; print(pybind11.get_include())'
  ],
  check: true
).stdout().strip()

includes = include_directories(
  [
    incdir_numpy,
    incdir_pybind11,
    'src'
  ]
)

c_args = [
  '-DPY_ARRAY_UNIQUE_SYMBOL=sample_dtypes_ARRAY_API',
  '-DPY_UFUNC_UNIQUE_SYMBOL=sample_dtypes_UFUNC_API',
  '-DNPY_NO_DEPRECATED_API=NPY_2_0_API_VERSION',
  '-DNPY_TARGET_VERSION=NPY_2_0_API_VERSION',
]
if get_option('buildtype') == 'debug'
  # Avoid '_DEBUG' as it links to 'python*_d.lib'
  c_args += ['-D__DEBUG']
endif

srcs = [
  'src/casts.cpp',
  'src/casts.h',
  'src/dtype.cpp',
  'src/dtype.h',
  'src/main.cpp',
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
  '_dtypes_ext',
  srcs,
  install: true,
  subdir: 'sample_dtypes',
  include_directories: includes,
  c_args: c_args,
  cpp_args: c_args,
)
