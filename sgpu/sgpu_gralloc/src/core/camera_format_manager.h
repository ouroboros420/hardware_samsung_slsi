///
/// @file camera_format_manager.h
/// @copyright 2020-2021 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "hidl_defines.h"
#include "ip_format_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class CameraFormatManager : public IpFormatManager {
public:
        CameraFormatManager() = default;
        ~CameraFormatManager() = default;
        CameraFormatManager(const IpFormatManager&) = delete;
        CameraFormatManager(const IpFormatManager&&) = delete;
        CameraFormatManager& operator=(const IpFormatManager&) = delete;
        CameraFormatManager& operator=(const IpFormatManager&&) = delete;

        uint32_t      get_layout(PixelFormat format, const BufferDescriptor &descriptor) const override;
        align_info    get_linear_alignment(PixelFormat format) const override;
};

} // gralloc
} // samsung
} // android
