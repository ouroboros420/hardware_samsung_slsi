///
/// @file AllocatorHidl.cpp
/// @copyright 2020 Samsung Electronics
///

#include "converter.h"
#include "AllocatorHidl.h"

namespace android {
namespace hardware {
namespace graphics {
namespace allocator {
namespace V4_0 {
namespace implementation {

// Methods from ::android::hardware::graphics::allocator::V4_0::IAllocator follow.
Return<void> Allocator::allocate(const hidl_vec<uint8_t>& descriptor, uint32_t count, allocate_cb _hidl_cb) {
        using mapper::V4_0::Error;

        Error error = Error::NONE;
        uint32_t stride = 0;

        hidl_vec<hidl_handle> hidl_handles(count);
        if (count == 0) {
                _hidl_cb(Error::NONE, stride, hidl_handles);
                return Void();
        }

        if (descriptor.size() == 0) {
                hidl_handles.resize(0);
                _hidl_cb(Error::BAD_DESCRIPTOR, stride, hidl_handles);
                return Void();
        }

        std::vector<native_handle_t*> handles(count);
        const auto buffer_descriptor =
                reinterpret_cast<const samsung::gralloc::BufferDescriptor*>(descriptor.data());
        error = convert_to_hidl_error(m_allocator.allocate(*buffer_descriptor, count, handles, &stride));

        if (error == Error::NONE) {
                for (uint32_t i = 0; i < count; i++) {
                        hidl_handles[i] = hidl_handle(handles[i]);
                }
        } else {
                hidl_handles.resize(0);
                handles.resize(0);
        }

        _hidl_cb(error, stride, hidl_handles);

        for (auto handle : handles) {
                m_allocator.free_handle(handle);
        }

        return Void();
}

}  // namespace implementation
}  // namespace V4_0
}  // namespace allocator
}  // namespace graphics
}  // namespace hardware
}  // namespace android
