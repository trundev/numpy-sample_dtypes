"""Scalar types needed by the dtype machinery."""

import ctypes
import numpy as np


DEFAULT_SHAPE = (2,) * 2
BYTES_BUF_ORDER = 'A'


class SampleScalar:
  _ndarr: np.ndarray

  def __init__(self, *, shape: tuple[int, ...] = DEFAULT_SHAPE, dtype=None):
    print(f'TODO: scalar.py: SampleScalar.__init__({shape=}, {dtype=})')
    self._ndarr = np.zeros(shape, dtype=dtype)

  def copy(self) -> 'SampleScalar':
    return type(self)(shape=self._ndarr.shape, dtype=self._ndarr.dtype)

  @property
  def elsize(self) -> int:
    return self._ndarr.nbytes

  @property
  def alignment(self) -> int:
    return self._ndarr.dtype.alignment

  def is_compatible(self, other: 'SampleScalar') -> bool:
    print(f'scalar.py: SampleScalar.is_compatible({other})')
    return self._ndarr.shape == other._ndarr.shape

  def __repr__(self) -> str:
    return f'{type(self).__name__}(id={hex(id(self))}, shape={self._ndarr.shape}, dtype={self._ndarr.dtype})'

  def _get_np_view(self, dataptr: int) -> np.ndarray:
    """Create numpy array that uses `dataptr` memory block"""
    ct_array = ctypes.cast(
        dataptr, ctypes.POINTER(np.ctypeslib.as_ctypes_type(self._ndarr.dtype))
    )
    return np.ctypeslib.as_array(ct_array, shape=self._ndarr.shape)

  def setitem(self, src: 'SampleScalar', dataptr: int) -> None:
    """Python `NPY_DT_setitem` implementation

    NOTE:
      `dataptr` is a buffer address, valid during execution of the function only
    """
    if not self.is_compatible(src):
      raise ValueError('Incompatible item value')

    self._get_np_view(dataptr)[...] = src._ndarr

  def getitem(self, dataptr: int) -> 'SampleScalar':
    """Python `NPY_DT_getitem` implementation

    NOTE: See setitem()
    """
    new = self.copy()
    new._ndarr[...] = self._get_np_view(dataptr)
    return new
