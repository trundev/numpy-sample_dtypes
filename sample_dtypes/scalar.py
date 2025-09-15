"""Scalar types needed by the dtype machinery."""

import ctypes
import numpy as np


DEFAULT_SHAPE = tuple()


class ScalarType:
  shape: tuple[int, ...]
  dtype: np.dtype

  def __init__(self, *, shape: tuple[int, ...] = DEFAULT_SHAPE, dtype=None):
    self.shape = shape
    self.dtype = np.dtype(dtype)

  def __repr__(self) -> str:
    return f'{type(self).__name__}(shape={self.shape}, dtype={self.dtype})'

  @property
  def elsize(self) -> int:
    """Overall size of DType item (to initialize `PyArray_Descr::elsize`)"""
    # Consider @cashed_property
    return np.empty(self.shape, self.dtype).nbytes

  @property
  def alignment(self) -> int:
    """Alignment of DType item (to initialize `PyArray_Descr::alignment`)"""
    return self.dtype.alignment

  @property
  def hasobject(self) -> bool:
    """Whether DType contain pointers to Python objects

    (adds `PyArray_Descr::flags` like `NPY_ITEM_HASOBJECT` and `NPY_NEEDS_INIT`)
    """
    return self.dtype.hasobject

  def is_compatible(self, other: 'ScalarType') -> bool:
    """Check if both objects are compatible for set/cast operations"""
    return self.shape == other.shape

  def _create(self) -> 'Scalar':
    """Instantiate Scalar object, that includes actual data storage"""
    return Scalar(shape=self.shape, dtype=self.dtype)

  def _get_np_view(self, dataptr: int) -> np.ndarray:
    """Create numpy array that uses `dataptr` memory block"""
    ct_array = ctypes.cast(
        dataptr, ctypes.POINTER(custom_as_ctypes_type(self.dtype))
    )
    return np.ctypeslib.as_array(ct_array, shape=self.shape)

  def _get_strided_np_view(
      self, dataptr: int, size: int, stride: int
  ) -> np.ndarray:
    """Create numpy array that uses strided `dataptr` memory block"""
    # Start with ctypes object to cover single SampleScalar
    ct_base = custom_as_ctypes_type(self.dtype)
    for dim in reversed(self.shape):
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

  def setitem(self, src: 'Scalar', dataptr: int) -> int:
    """Python `NPY_DT_setitem` implementation

    NOTE:
      `dataptr` is a buffer address, valid during execution of the function only
    """
    if not src.is_compatible(self):
      raise ValueError('Incompatible item value')

    # TODO: Avoid private member access
    self._get_np_view(dataptr)[...] = src._ndarr
    return 0  # -1 on failure

  def getitem(self, dataptr: int) -> 'Scalar':
    """Python `NPY_DT_getitem` implementation

    NOTE:
      `dataptr` is a buffer address, valid during execution of the function only
    """
    new = self._create()
    # TODO: Avoid private member access
    new._ndarr[...] = self._get_np_view(dataptr)
    return new

  def clear_loop(self, data: int, size: int, stride: int) -> int:
    """Python implementation for `out_loop` returned by `NPY_DT_get_clear_loop`

    NOTE: See setitem()
    """
    view = self._get_strided_np_view(data, size, stride)
    view[...] = 0  # Force dereference of all object-entries
    return 0

  def cast_loop(
      self,
      other: 'ScalarType',
      data: tuple[int, int],
      dimensions: tuple[int],
      strides: tuple[int, int],
  ) -> int:
    """Python implementation for `NPY_METH_unaligned_strided_loop` callback

    NOTE: See setitem()
    """
    in_view = other._get_strided_np_view(data[0], dimensions[0], strides[0])
    out_view = self._get_strided_np_view(data[1], dimensions[0], strides[1])
    out_view[...] = in_view
    return 0


class Scalar(ScalarType):
  _ndarr: np.ndarray

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    self._ndarr = np.empty(self.shape, dtype=self.dtype)


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
