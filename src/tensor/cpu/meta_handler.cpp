#include "lamppp/tensor/cpu/meta_handler.hpp"
#include "lamppp/common/assert.hpp"
#include "lamppp/tensor/cuda/offset_util.cuh"
#include "lamppp/tensor/device_type.hpp"

namespace lmp::tensor::detail {

template <>
UnaryMetaHandler::TensorMetaHandler(const TensorImpl* a)
    : inTens({a}),
      outDtype_(a->type()),
      outSize_(a->numel()),
      outShape_(a->shape()) {
  LMP_DISPATCH_ALL_TYPES(outDtype_, [&] {
    using out_dtype_t = scalar_t;
    LMP_DISPATCH_ALL_TYPES(a->type(), [&] {
      using arg_dtype_t = scalar_t;
      Storage out_st(outSize_ * sizeof(out_dtype_t), a->device());
      outTen = std::make_unique<TensorImpl>(out_st, outShape_, outDtype_);
    });
  });
}

template <>
ExpandMetaHandler::TensorMetaHandler(const TensorImpl* a, const TensorImpl* b)
    : inTens({a, b}),
      outDtype_(type_upcast(a->type(), b->type())),
      outSize_(a->numel()),
      outShape_(a->shape()) {
  LMP_INTERNAL_ASSERT(a->device() == b->device(),
                      "Should have asserted already");
  detail::AlignUtil expand_dims(a->shape(), b->shape());
  outSize_ = expand_dims.aligned_size_;
  outShape_ = expand_dims.aligned_shape_;
  LMP_DISPATCH_ALL_TYPES(outDtype_, [&] {
    using out_dtype_t = scalar_t;
    LMP_DISPATCH_ALL_TYPES(a->type(), [&] {
      using arg1_dtype_t = scalar_t;
      LMP_DISPATCH_ALL_TYPES(b->type(), [&] {
        using arg2_dtype_t = scalar_t;
        Storage out_st(outSize_ * sizeof(out_dtype_t), a->device());
        outTen = std::make_unique<TensorImpl>(out_st, outShape_, outDtype_);

        if (a->device() == DeviceType::CPU) {
          outOffset =
              std::make_unique<cpu::CPUOffsetUtil<ExpandMetaHandler::NumElem>>(
                  ::std::array<const TensorImpl*, ExpandMetaHandler::NumElem>{
                      a, b},
                  *outTen.get());
        } else if (a->device() == DeviceType::CUDA) {
          outOffset = std::make_unique<
              cuda::CUDAOffsetUtil<ExpandMetaHandler::NumElem>>(
              ::std::array<const TensorImpl*, ExpandMetaHandler::NumElem>{a, b},
              *outTen.get());
        } else {  // TODO: this works better as a dispatch, need to fix sometime TODO TODO THIS IS BAD BAD BAD
          LMP_INTERNAL_ASSERT(false, "Cannot support device");
        }
      });
    });
  });
}

template <>
ReductMetaHandler::TensorMetaHandler(const TensorImpl* a, size_t axis)
    : inTens({a}),
      outDtype_(a->type()),
      outSize_(a->numel()),
      outShape_(a->shape()) {
  outSize_ /= outShape_[axis];
  outShape_[axis] = 1;
  LMP_DISPATCH_ALL_TYPES(outDtype_, [&] {
    using out_dtype_t = scalar_t;
    LMP_DISPATCH_ALL_TYPES(a->type(), [&] {
      using arg_dtype_t = scalar_t;
      Storage out_st(outSize_ * sizeof(out_dtype_t), a->device());
      outTen = std::make_unique<TensorImpl>(out_st, outShape_, outDtype_);
    });
  });
}

}  // namespace lmp::tensor::detail