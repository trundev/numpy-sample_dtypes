"""Setuptool-based build for sample_dtypes."""

import platform
import numpy as np
from setuptools import Extension
from setuptools import setup
from setuptools.command.build_py import build_py

if platform.system() == "Windows":
  COMPILE_ARGS = [
      "/std:c++17",
  ]
else:
  COMPILE_ARGS = [
      "-std=c++17",
  ]


setup(
    ext_modules=[
        Extension(
            "sample_dtypes._sample_dtypes_ext",
            [
                "src/dtypes.cc",
                "src/numpy.cc",
            ],
            include_dirs=[
                ".",
                np.get_include(),
            ],
            extra_compile_args=COMPILE_ARGS,
        )
    ],
    cmdclass={"build_py": build_py},
)
