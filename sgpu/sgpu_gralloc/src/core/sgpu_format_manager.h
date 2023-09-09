///
/// @file sgpu_format_manager.h
/// @copyright 2020-2021 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "hidl_defines.h"
#include "ip_format_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class SgpuFormatManager : public IpFormatManager {
private:
        struct format_layout_info {
                PixelFormat format;
                uint64_t    usage;
                uint32_t    layout;
        };

public:
        SgpuFormatManager() = default;
        ~SgpuFormatManager() = default;
        SgpuFormatManager(const IpFormatManager&) = delete;
        SgpuFormatManager(const IpFormatManager&&) = delete;
        SgpuFormatManager& operator=(const IpFormatManager&) = delete;
        SgpuFormatManager& operator=(const IpFormatManager&&) = delete;

        uint32_t      get_layout(PixelFormat format, const BufferDescriptor &descriptor) const override;
        align_info    get_linear_alignment(PixelFormat format) const override;
};

} // gralloc
} // samsung
} // android
