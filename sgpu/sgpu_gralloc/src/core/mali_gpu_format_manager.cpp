///
/// @file mali_gpu_format_manager.cpp
/// @copyright 2020-2021 Samsung Electronics
///

#include "debug_helper.h"
#include "format_info.h"
#include "mali_gpu_format_manager.h"
#include "hidl_defines.h"
#include "layout_manager.h"
#include "private_handle_helper.h"
#include "util/util.h"

#undef LOG_TAG
#define LOG_TAG "MaliGpuFormatManager"

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
uint32_t MaliGpuFormatManager::get_layout(PixelFormat format, const BufferDescriptor &descriptor) const
{
        SGR_UNUSED(descriptor);

        uint32_t layout = 0;
        switch (format) {
                case PixelFormat::RGBA_8888:
                case PixelFormat::RGBX_8888:
                case PixelFormat::RGB_565:
                case PixelFormat::RGB_888:
                case PixelFormat::BGRA_8888:
                case PixelFormat::YCBCR_422_SP:
                case PixelFormat::YCRCB_420_SP:
                case PixelFormat::YCBCR_422_I:
                case PixelFormat::RGBA_FP16:
                case PixelFormat::BLOB:
                case PixelFormat::RAW16:
                case PixelFormat::RAW10:
                case PixelFormat::RAW12:
                case PixelFormat::RGBA_1010102:
                case PixelFormat::YCBCR_P010:
                case PixelFormat::YV12:
                case PixelFormat::PRIVATE_YCBCR_420_P_M:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_TILED:
                case PixelFormat::PRIVATE_YV12_M:
                case PixelFormat::PRIVATE_YCRCB_420_SP_M:
                case PixelFormat::PRIVATE_YCRCB_420_SP_M_FULL:
                case PixelFormat::PRIVATE_YCBCR_420_P:
                case PixelFormat::PRIVATE_YCBCR_420_SP:
                case PixelFormat::PRIVATE_YCBCR_420_SPN:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_S10B:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_S10B:
                case PixelFormat::PRIVATE_YCBCR_P010_M:
                        layout = static_cast<uint32_t>(FormatLayoutBitMask::LINEAR);
                        break;
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_SBWC:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC:
                case PixelFormat::PRIVATE_YCRCB_420_SP_M_SBWC:
                case PixelFormat::PRIVATE_YCRCB_420_SP_M_10B_SBWC:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L50:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC_L75:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L50:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_SBWC_L75:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L40:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L60:
                case PixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC_L80:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L40:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L60:
                case PixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L80:
                        layout = static_cast<uint32_t>(FormatLayoutBitMask::SBWC);
                        break;
                default:
                        layout = static_cast<uint32_t>(FormatLayoutBitMask::NONE);
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
align_info MaliGpuFormatManager::get_linear_alignment(PixelFormat format) const
{
        align_info rgb_align = {
                .stride_in_bytes        = 64,
                .vstride_in_pixels      = 1,
                .plane_padding_in_bytes = 0,
                .alloc_padding_in_bytes = 0
        };

        if (format == PixelFormat::PRIVATE_YCBCR_420_SP_M_S10B ||
            format == PixelFormat::PRIVATE_YCBCR_420_SPN_S10B){
                rgb_align.stride_in_bytes = 64;
                rgb_align.vstride_in_pixels = 16;
        } else if (is_yuv_format(format)) {
                rgb_align.stride_in_bytes = 16;
        }

        dump_alignment(rgb_align);

        return rgb_align;
}

} // gralloc
} // samsung
} // android
