"""Scalar types needed by the dtype machinery."""

import ctypes
import numpy as np


DEFAULT_SHAPE = (2,) * 2
BYTES_BUF_ORDER = 'A'


def custom_as_ctypes_type(dtype: np.dtype):
  """Wrapper around `numpy.ctypeslib.as_ctypes_type` that maps 'O' to py_object"""
  if not dtype.hasobject:
    return np.ctypeslib.as_ctypes_type(dtype)

  if dtype.fields is None:
    # Simple dtype or array
    if dtype.subdtype:
      # Handle sub-array types like ('O', 3)
      base, shape = dtype.subdtype
      new_base = custom_as_ctypes_type(base)
      for dim in reversed(shape):
        new_base = new_base * dim
      return new_base
    else:
      # Simple object (non-objects are already handled)
      assert dtype == np.dtype(object)
      return ctypes.py_object

  # Structured dtype
  new_fields = []
  for name, (field_dtype, *_) in dtype.fields.items():
    new_field_dtype = custom_as_ctypes_type(field_dtype)
    new_fields.append((name, new_field_dtype))

  class CustomStruct(ctypes.Structure):
    _pack_ = dtype.alignment
    _fields_ = new_fields

  return CustomStruct


class SampleScalar:
  _ndarr: np.ndarray

  def __init__(self, *, shape: tuple[int, ...] = DEFAULT_SHAPE, dtype=None):
    print(f'TODO: scalar.py: SampleScalar.__init__({shape=}, {dtype=})')
    self._ndarr = np.empty(shape, dtype=dtype)

  def copy(self) -> 'SampleScalar':
    return type(self)(shape=self._ndarr.shape, dtype=self._ndarr.dtype)

  @property
  def elsize(self) -> int:
    return self._ndarr.nbytes

  @property
  def alignment(self) -> int:
    return self._ndarr.dtype.alignment

  @property
  def hasobject(self) -> bool:
    return self._ndarr.dtype.hasobject

  def is_compatible(self, other: 'SampleScalar') -> bool:
    print(f'scalar.py: SampleScalar.is_compatible({other})')
    return self._ndarr.shape == other._ndarr.shape

  def __repr__(self) -> str:
    return f'{type(self).__name__}(id={hex(id(self))}, shape={self._ndarr.shape}, dtype={self._ndarr.dtype})'

  def _get_np_view(self, dataptr: int) -> np.ndarray:
    """Create numpy array that uses `dataptr` memory block"""
    ct_array = ctypes.cast(
        dataptr, ctypes.POINTER(custom_as_ctypes_type(self._ndarr.dtype))
    )
    return np.ctypeslib.as_array(ct_array, shape=self._ndarr.shape)

  def _get_strided_np_view(
      self, dataptr: int, size: int, stride: int
  ) -> np.ndarray:
    """Create numpy array that uses strided `dataptr` memory block"""
    # Start with ctypes object to cover single SampleScalar
    ct_base = custom_as_ctypes_type(self._ndarr.dtype)
    for dim in reversed(self._ndarr.shape):
      ct_base = ct_base * dim

    # Single ctypes object to cover whole strided data-block
    # CHECKME: `stride` may also be zero (?), but `size` must be 1
    assert stride >= ctypes.sizeof(ct_base), (
        'Insufficient stride size ' f'{stride} / {ctypes.sizeof(ct_base)}'
    )

    class PaddedStruct(ctypes.Structure):
      _align_ = stride
      _fields_ = [('_', ct_base)]

    ct_base = PaddedStruct * size

    # Create ndarray view from this ctypes object
    ct_array = ctypes.cast(dataptr, ctypes.POINTER(ct_base))
    return np.ctypeslib.as_array(ct_array, shape=())

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

  def clear_loop(self, data: int, size: int, stride: int) -> int:
    """Python implementation for `out_loop` returned by `NPY_DT_get_clear_loop`

    NOTE: See setitem()
    """
    print(f'scalar.py: SampleScalar.clear_loop {data=}, {size=}, {stride=}')
    view = self._get_strided_np_view(data, size, stride)
    view[...] = 0  # Force dereference of all object-entries
    return 0

  def cast_loop(
      self,
      other: 'SampleScalar',
      data: tuple[int, int],
      dimensions: tuple[int],
      strides: tuple[int, int],
  ) -> int:
    """Python implementation for `NPY_METH_unaligned_strided_loop` callback

    NOTE: See setitem()
    """
    print(
        f'scalar.py: SampleScalar.cast_loop {other=}, {data=}, {dimensions=}, {strides=}'
    )
    in_view = other._get_strided_np_view(data[0], dimensions[0], strides[0])
    out_view = self._get_strided_np_view(data[1], dimensions[0], strides[1])
    out_view[...] = in_view
    return 0
