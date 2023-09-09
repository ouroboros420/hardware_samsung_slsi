///
/// @file AllocatorHidl.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include <android/hardware/graphics/allocator/4.0/IAllocator.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "core/allocator.h"

namespace android {
namespace hardware {
namespace graphics {
namespace allocator {
namespace V4_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct Allocator : public V4_0::IAllocator {
        // Methods from ::android::hardware::graphics::allocator::V4_0::IAllocator follow.
        Return<void> allocate(const hidl_vec<uint8_t>& descriptor, uint32_t count, allocate_cb _hidl_cb) override;

        // Methods from ::android::hidl::base::V1_0::IBase follow.
        private:
            samsung::gralloc::Allocator m_allocator;
};

}  // namespace implementation
}  // namespace V4_0
}  // namespace allocator
}  // namespace graphics
}  // namespace hardware
}  // namespace android