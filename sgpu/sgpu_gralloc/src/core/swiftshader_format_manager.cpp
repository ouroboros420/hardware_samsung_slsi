///
/// @file swiftshader_format_manager.cpp
/// @copyright 2020 Samsung Electronics
///

#include "debug_helper.h"
#include "format_info.h"
#include "hidl_defines.h"
#include "layout_manager.h"
#include "swiftshader_format_manager.h"
#include "util/util.h"

#undef LOG_TAG
#define LOG_TAG "SwiftshaderFormatManager"

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
uint32_t SwiftshaderFormatManager::get_layout(PixelFormat format, const BufferDescriptor &descriptor) const
{
        SGR_UNUSED(descriptor);

        uint32_t layout = 0;
        switch (format) {
                case PixelFormat::RGBA_8888:
                case PixelFormat::RGBX_8888:
                case PixelFormat::RGB_565:
                case PixelFormat::BGRA_8888:
                case PixelFormat::RGBA_1010102:
                        layout |= (static_cast<uint32_t>(FormatLayoutBitMask::LINEAR));
                        break;
                case PixelFormat::UNDEFINED:
                ///@todo GFXSW-3833 - Update RGB8 according to UMD solution
                case PixelFormat::RGB_888:
                default:
                        layout |= static_cast<uint32_t>(FormatLayoutBitMask::NONE);
                        break;
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
align_info SwiftshaderFormatManager::get_linear_alignment(PixelFormat format) const
{
        // 2x2 pixel alignment + 4 byte padding for swiftshader
        align_info align = {
                .stride_in_bytes        = 1,
                .vstride_in_pixels      = 2,
                .plane_padding_in_bytes = 4,
                .alloc_padding_in_bytes = 0
        };

        align.stride_in_bytes = get_bps(format) * 2;

        dump_alignment(align);

        return align;
}

} // gralloc
} // samsung
} // android
