"""NumPy sample Dtypes"""

from . import _dtypes_ext
from .scalar import ScalarType

__version__ = "0.0.0"  # Keep in sync with pyproject.toml:version
__all__ = [
    "__version__",
    "SampleDType",
    "ScalarType",
]

# At startup create NumPy DType, based on this scalar
SampleDType = _dtypes_ext.init_dtype(ScalarType)
del _dtypes_ext
