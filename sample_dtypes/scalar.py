"""Scalar types needed by the dtype machinery."""

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
    print(f'TODO: scalar.py: SampleScalar.is_compatible({other})')
    # This implies `self.elsize == other.elsize and self.alignment == other.alignment`
    return (
        self._ndarr.shape == other._ndarr.shape
        and self._ndarr.dtype == other._ndarr.dtype
    )

  def __repr__(self) -> str:
    return f'{type(self).__name__}(id={hex(id(self))}, shape={self._ndarr.shape}, dtype={self._ndarr.dtype})'

  @property
  def data_buffer(self) -> bytes:
    """Get raw-data"""
    return self._ndarr.tobytes(order=BYTES_BUF_ORDER)

  def from_data_buffer(self, buf: bytes) -> 'SampleScalar':
    """Create new object using raw-data"""
    assert (
        len(buf) == self._ndarr.nbytes
    ), f'Buffer size must be {self._ndarr.nbytes}'
    new = self.copy()
    new._ndarr.flat[:] = np.frombuffer(buf, dtype=self._ndarr.dtype)
    return new
