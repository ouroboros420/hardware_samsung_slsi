///
/// @file MapperHidl.cpp
/// @copyright 2020 Samsung Electronics
///

#include <cutils/native_handle.h>

#include "converter.h"
#include "MapperHidl.h"
#include "util/util.h"

namespace android {
namespace hardware {
namespace graphics {
namespace mapper {
namespace V3_0 {
namespace implementation {

static int32_t get_fence_fd(const hidl_handle& fence)
{
        const native_handle_t* handle = fence.getNativeHandle();
        return (handle != nullptr && handle->numFds == 1) ? handle->data[0] : -1;
}

// Methods from ::android::hardware::graphics::mapper::V3_0::IMapper follow.
Return<void> Mapper::createDescriptor(const ::android::hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo& description, createDescriptor_cb _hidl_cb) {
        hidl_vec<uint32_t> descriptor(sizeof(samsung::gralloc::BufferDescriptor) / sizeof(uint32_t));
        auto buffer_descriptor = reinterpret_cast<samsung::gralloc::BufferDescriptor*>(descriptor.data());
        Error error = convert_to_hidl_error(m_mapper.create_descriptor(samsung::gralloc::convert_to_buffer_descriptor_info(description),
                                                                       buffer_descriptor));
        if (error != Error::NONE) {
                descriptor.resize(0);
        }

        _hidl_cb(error, descriptor);

        return Void();
}

Return<void> Mapper::importBuffer(const hidl_handle& rawHandle, importBuffer_cb _hidl_cb) {
        if (rawHandle.getNativeHandle() == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr);
                return Void();
        }

        native_handle_t *handle = nullptr;
        Error error = convert_to_hidl_error(m_mapper.import_buffer(rawHandle.getNativeHandle(), &handle));

        _hidl_cb(error, reinterpret_cast<void*>(handle));

        return Void();
}

Return<::android::hardware::graphics::mapper::V3_0::Error> Mapper::freeBuffer(void* buffer) {
        if (buffer == nullptr) {
                return Error::BAD_BUFFER;
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        return convert_to_hidl_error(m_mapper.free_buffer(handle));
}

Return<::android::hardware::graphics::mapper::V3_0::Error> Mapper::validateBufferSize(void* buffer, const ::android::hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo& description, uint32_t stride) {
        if (buffer == nullptr) {
                return Error::BAD_BUFFER;
        }

        auto handle = reinterpret_cast<const native_handle_t*>(buffer);
        auto buffer_descriptor_info = samsung::gralloc::convert_to_buffer_descriptor_info(description);

        return convert_to_hidl_error(m_mapper.validate_buffer_size(handle, buffer_descriptor_info, stride));
}

Return<void> Mapper::getTransportSize(void* buffer, getTransportSize_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, 0, 0);
                return Void();
        }

        auto handle = reinterpret_cast<const native_handle_t*>(buffer);
        int32_t num_fds = 0;
        int32_t num_ints = 0;

        Error error = convert_to_hidl_error(m_mapper.get_transport_size(handle, &num_fds, &num_ints));

        _hidl_cb(error, num_fds, num_ints);

        return Void();
}

Return<void> Mapper::lock(void* buffer, uint64_t cpuUsage, const ::android::hardware::graphics::mapper::V3_0::IMapper::Rect& accessRegion, const hidl_handle& acquireFence, lock_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr, -1, -1);
                return Void();
        }

        if (cpuUsage == 0) {
                _hidl_cb(Error::BAD_VALUE, nullptr, -1, -1);
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        auto access_region = samsung::gralloc::convert_to_rect(accessRegion);
        auto acquire_fence = get_fence_fd(acquireFence);
        void *data = nullptr;
        int32_t bytesPerPixel = -1;
        int32_t bytesPerStride = -1;

        Error error = convert_to_hidl_error(m_mapper.lock(handle, cpuUsage, access_region, acquire_fence,
                                                          &data, &bytesPerPixel, &bytesPerStride));

        _hidl_cb(error, data, bytesPerPixel, bytesPerStride);

        return Void();
}

Return<void> Mapper::lockYCbCr(void* buffer, uint64_t cpuUsage, const ::android::hardware::graphics::mapper::V3_0::IMapper::Rect& accessRegion, const hidl_handle& acquireFence, lockYCbCr_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, YCbCrLayout());
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        auto access_region = samsung::gralloc::convert_to_rect(accessRegion);
        auto acquire_fence = get_fence_fd(acquireFence);
        YCbCrLayout layout = {};

        Error error = convert_to_hidl_error(m_mapper.lock_ycbcr(handle, cpuUsage, access_region, acquire_fence,
                                                             reinterpret_cast<samsung::gralloc::YCbCrLayout*>(&layout)));

        _hidl_cb(error, layout);

        return Void();
}

Return<void> Mapper::unlock(void* buffer, unlock_cb _hidl_cb) {
        if (buffer == nullptr) {
                _hidl_cb(Error::BAD_BUFFER, nullptr);
                return Void();
        }

        auto handle = reinterpret_cast<native_handle_t*>(buffer);
        int fence = -1;

        Error error = convert_to_hidl_error(m_mapper.unlock(handle, &fence));

        native_handle_t *fence_handle = nullptr;
        if (fence >= 0) {
                NATIVE_HANDLE_DECLARE_STORAGE(storage, 1, 0);
                fence_handle = native_handle_init(storage, 1, 0);
                fence_handle->data[0] = fence;
        }

        _hidl_cb(error, fence_handle);

        return Void();
}

Return<void> Mapper::isSupported(const ::android::hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo& description, isSupported_cb _hidl_cb) {
        bool supported = m_mapper.is_supported(samsung::gralloc::convert_to_buffer_descriptor_info(description));

        _hidl_cb(Error::NONE, supported);

        return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

IMapper* HIDL_FETCH_IMapper(const char*  name) {
        SGR_UNUSED(name);
        SGR_LOGI("android.hardware.graphics.mapper@3.0: Loaded Mapper successfully.");

        return new Mapper();
}

}  // namespace implementation
}  // namespace V3_0
}  // namespace mapper
}  // namespace graphics
}  // namespace hardware
}  // namespace android
