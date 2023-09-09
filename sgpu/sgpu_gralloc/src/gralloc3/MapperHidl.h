///
/// @file MapperHidl.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include <android/hardware/graphics/mapper/3.0/IMapper.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "core/mapper.h"

namespace android {
namespace hardware {
namespace graphics {
namespace mapper {
namespace V3_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class Mapper : public IMapper {
public:
        // Methods from ::android::hardware::graphics::mapper::V3_0::IMapper follow.
        Return<void> createDescriptor(const ::android::hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo& description, createDescriptor_cb _hidl_cb) override;
        Return<void> importBuffer(const hidl_handle& rawHandle, importBuffer_cb _hidl_cb) override;
        Return<::android::hardware::graphics::mapper::V3_0::Error> freeBuffer(void* buffer) override;
        Return<::android::hardware::graphics::mapper::V3_0::Error> validateBufferSize(void* buffer, const ::android::hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo& description, uint32_t stride) override;
        Return<void> getTransportSize(void* buffer, getTransportSize_cb _hidl_cb) override;
        Return<void> lock(void* buffer, uint64_t cpuUsage, const ::android::hardware::graphics::mapper::V3_0::IMapper::Rect& accessRegion, const hidl_handle& acquireFence, lock_cb _hidl_cb) override;
        Return<void> lockYCbCr(void* buffer, uint64_t cpuUsage, const ::android::hardware::graphics::mapper::V3_0::IMapper::Rect& accessRegion, const hidl_handle& acquireFence, lockYCbCr_cb _hidl_cb) override;
        Return<void> unlock(void* buffer, unlock_cb _hidl_cb) override;
        Return<void> isSupported(const ::android::hardware::graphics::mapper::V3_0::IMapper::BufferDescriptorInfo& description, isSupported_cb _hidl_cb) override;

        // Methods from ::android::hidl::base::V1_0::IBase follow.

private:
        samsung::gralloc::Mapper m_mapper;
};

extern "C" IMapper* HIDL_FETCH_IMapper(const char* name);

}  // namespace implementation
}  // namespace V3_0
}  // namespace mapper
}  // namespace graphics
}  // namespace hardware
}  // namespace android
