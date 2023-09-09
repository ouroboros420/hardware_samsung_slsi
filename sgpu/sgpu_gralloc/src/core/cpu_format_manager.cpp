///
/// @file cpu_format_manager.cpp
/// @copyright 2020-2021 Samsung Electronics
///

#include "cpu_format_manager.h"
#include "debug_helper.h"
#include "format_info.h"
#include "format_manager.h"
#include "util/util.h"

#undef LOG_TAG
#define LOG_TAG "CpuFormatManager"

namespace android {
namespace samsung {
namespace gralloc {

///
/// @brief Get format layout for format.
///
/// @param[in] format           format
/// @param[in] descriptor       Buffer descriptor passed to request allocation
///
/// @return Flag of layouts
///
uint32_t CpuFormatManager::get_layout(PixelFormat format, const BufferDescriptor &descriptor) const
{
        SGR_UNUSED(descriptor);

        uint32_t layout = static_cast<uint32_t>(FormatLayoutBitMask::LINEAR);

        if (is_sbwc_format(format)) {
                layout = static_cast<uint32_t>(FormatLayoutBitMask::SBWC);
        }

        SGR_LOGV("Format_Layout_Bitmask = 0x%x %s", layout, get_format_layout_bitmask_string(layout).c_str());

        return layout;
}

///
/// @brief Get raw alignment rule for linear layout
///
/// @param[in] format Format
///
/// @return ip alignment
///
align_info CpuFormatManager::get_linear_alignment(PixelFormat format) const
{
        align_info align = {
                .stride_in_bytes        = 1,
                .vstride_in_pixels      = 1,
                .plane_padding_in_bytes = 0,
                .alloc_padding_in_bytes = 0
        };

        // Android format requirement - a horizontal stride multiple of 16 pixels
        switch (format) {
        case PixelFormat::Y8:
        case PixelFormat::YV12:
                align.stride_in_bytes = 16;
                break;
        case PixelFormat::RAW16:
        case PixelFormat::Y16:
                align.stride_in_bytes = 32;
                break;
        case PixelFormat::PRIVATE_YCBCR_420_SP_M_S10B:
        case PixelFormat::PRIVATE_YCBCR_420_SPN_S10B:
                align.stride_in_bytes = 64;
                align.vstride_in_pixels = 16;
                break;
        default:
                break;
        }

        dump_alignment(align, static_cast<uint32_t>(IpBitMask::CPU));

        return align;
}

} // gralloc
} // samsung
} // android
