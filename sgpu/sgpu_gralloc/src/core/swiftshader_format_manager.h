///
/// @file swiftshader_format_manager.h
/// @copyright 2020-2021 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "hidl_defines.h"
#include "ip_format_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class SwiftshaderFormatManager : public IpFormatManager {
public:
        SwiftshaderFormatManager() = default;
        ~SwiftshaderFormatManager() = default;
        SwiftshaderFormatManager(const IpFormatManager&) = delete;
        SwiftshaderFormatManager(const IpFormatManager&&) = delete;
        SwiftshaderFormatManager& operator=(const IpFormatManager&) = delete;
        SwiftshaderFormatManager& operator=(const IpFormatManager&&) = delete;

        uint32_t      get_layout(PixelFormat format, const BufferDescriptor &descriptor) const override;
        align_info    get_linear_alignment(PixelFormat format) const override;
};

} // gralloc
} // samsung
} // android
