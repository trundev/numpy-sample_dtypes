"""NumPy sample Dtypes"""

__version__ = "0.0.0"  # Keep in sync with pyproject.toml:version
__all__ = [
    "__version__",
    "register_dtype",
]

import sample_dtypes
from sample_dtypes._sample_dtypes_ext import _register_dtype_ext


def register_dtype(name: str, data_type: type, count: int):
  dtype = _register_dtype_ext(name, data_type, count)
  setattr(sample_dtypes, name, dtype)
  setattr(dtype, "__module__", sample_dtypes)
  # Support dtype(type_name)
  # setattr(dtype, 'dtype', npy_descr)
  return dtype
