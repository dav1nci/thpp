/**
 * Copyright 2015 Facebook
 * @author Tudor Bosman (tudorb@fb.com)
 */

#ifndef THPP_CUDA_TENSOR_H_
#define THPP_CUDA_TENSOR_H_

#include <thpp/cuda/Storage.h>
#include <thpp/Tensor.h>
#include <thpp/cuda/detail/Tensor.h>

namespace thpp {

template <class T>
class CudaTensor : public TensorBase<T, CudaStorage<T>, CudaTensor<T>> {
  typedef TensorBase<T, CudaStorage<T>, CudaTensor<T>> Base;
  typedef typename Base::Ops Ops;
  friend class TensorPtr<CudaTensor>;
 public:
  typedef typename Base::StorageType StorageType;
  typedef typename Base::offset_type offset_type;
  typedef typename Base::size_type size_type;
  typedef typename Base::THType THType;
  typedef typename Base::Ptr Ptr;

  // Default constructor; construct an empty, zero-dimensional Tensor.
  CudaTensor();

  CudaTensor(StorageType storage, offset_type storageOffset,
             LongStorage sizes, LongStorage strides = LongStorage());

  // Constructors from a list of sizes and a list of strides.
  // If specified, the list of strides must have the same size as the
  // list of sizes.
  explicit CudaTensor(LongStorage sizes, LongStorage strides = LongStorage());
  explicit CudaTensor(LongRange sizes, LongRange strides = LongRange());
  explicit CudaTensor(const std::vector<size_type>& sizes,
                      const std::vector<size_type>& strides =
                        std::vector<size_type>());
  explicit CudaTensor(
      std::initializer_list<size_type> sizes,
      std::initializer_list<size_type> strides =
        std::initializer_list<size_type>());

  explicit CudaTensor(const Tensor<T>& cpuTensor);

  // Deserialize from Thrift. Throws if wrong type.
  explicit CudaTensor(const ThriftTensor& thriftTensor,
                      SharingMode sharing = SHARE_IOBUF_MANAGED);

  // Do not alias other, create separate object (with separate metadata);
  // might still share data with other, unless UNIQUE requested in
  // cloneMode.
  explicit CudaTensor(const THType* other, unsigned cloneMode = 0);

  // Move/copy constructors. Enforce requested mode.
  /* implicit */ CudaTensor(const CudaTensor& other, unsigned cloneMode = 0);
  /* implicit */ /* may throw */ CudaTensor(
      CudaTensor&& other, unsigned cloneMode = 0);

  // Move/copy assignment operators. Will share memory with "other".
  CudaTensor& operator=(const CudaTensor& other);
  /* noexcept override */ CudaTensor& operator=(CudaTensor&& other);

  T at(offset_type idx) const { return at({idx}); }
  T at(std::initializer_list<offset_type> indices) const;

  // Copy from another tensor
  void copy(const CudaTensor& src);

  template <class U>
  void copy(const Tensor<U>& src);

  // Copy to another (non-CUDA) tensor
  template <class U>
  void copyTo(Tensor<U>& dest) const;

  // <max, argmax>
  std::pair<CudaTensor, CudaTensor> max(int dim) const;

  // <min, argmin>
  std::pair<CudaTensor, CudaTensor> min(int dim) const;

  // Return the CUDA device that this tensor is based on
  int getDevice() const;

  // Serialize to Thrift. Won't ever share CUDA memory.
  void serialize(ThriftTensor& out,
                 ThriftTensorEndianness endianness =
                     ThriftTensorEndianness::NATIVE,
                 SharingMode sharing = SHARE_IOBUF_MANAGED) const;

  // Copy to CPU
  typename Tensor<T>::Ptr toCPU() const;

  // Copy to given CUDA device, unless already there.
  Ptr toDevice(int device) const;

 private:
  CudaTensor(detail::SetTH, THType* t, bool incRef);
};

template <class D, class S>
void copyTensor(Tensor<D>& dest, const CudaTensor<S>& src) {
  src.copyTo(dest);
}

template <class D, class S>
void copyTensor(CudaTensor<D>& dest, const Tensor<S>& src) {
  dest.copy(src);
}

template <class D, class S>
void copyTensor(CudaTensor<D>& dest, const CudaTensor<S>& src) {
  dest.copy(src);
}

}  // namespaces

#include <thpp/cuda/Tensor-inl.h>

#endif /* THPP_CUDA_TENSOR_H_ */
