///
/// @file cpu_format_manager.h
/// @copyright 2020-2021 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "ip_format_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class CpuFormatManager : public IpFormatManager {
public:
        CpuFormatManager() = default;
        ~CpuFormatManager() = default;
        CpuFormatManager(const CpuFormatManager&) = delete;
        CpuFormatManager(const CpuFormatManager&&) = delete;
        CpuFormatManager& operator=(const CpuFormatManager&) = delete;
        CpuFormatManager& operator=(const CpuFormatManager&&) = delete;

        uint32_t      get_layout(PixelFormat format, const BufferDescriptor &descriptor) const override;
        align_info    get_linear_alignment(PixelFormat format) const override;
};

} // gralloc
} // samsung
} // android
