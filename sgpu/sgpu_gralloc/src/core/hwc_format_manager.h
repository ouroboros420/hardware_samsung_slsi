///
/// @file hwc_format_manager.h
/// @copyright 2020-2021 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "hidl_defines.h"
#include "ip_format_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class HwcFormatManager : public IpFormatManager {
public:
        HwcFormatManager() = default;
        ~HwcFormatManager() = default;
        HwcFormatManager(const IpFormatManager&) = delete;
        HwcFormatManager(const IpFormatManager&&) = delete;
        HwcFormatManager& operator=(const IpFormatManager&) = delete;
        HwcFormatManager& operator=(const IpFormatManager&&) = delete;

        uint32_t      get_layout(PixelFormat format, const BufferDescriptor &descriptor) const override;
        align_info    get_linear_alignment(PixelFormat format) const override;
};

} // gralloc
} // samsung
} // android
