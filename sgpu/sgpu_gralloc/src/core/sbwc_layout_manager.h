///
/// @file sbwc_layout_manager.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include "hidl_defines.h"
#include "interface/metadata_gralloc.h"
#include "layout_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

constexpr uint32_t sbwc_blk_dimension_width = 32;                // In Pixels
constexpr uint32_t sbwc_blk_dimension_height = 4;                // In Pixels
constexpr uint32_t sbwc_vstride_lossless_alignment = 16;         // In Pixels
constexpr uint32_t sbwc_vstride_lossy_alignment = 16;            // In Pixels
constexpr uint32_t sbwc_header_stride_alignment = 16;            // In Pixels
constexpr uint32_t sbwc_payload_base_addr_alignment = 32;        // In Bytes

///
/// @brief Check whether format is valid lossless SBWC format
///
/// @param[in] format Format
///
/// @return true if valid lossless SBWC format
///
static inline bool is_lossless_format(PixelFormat format)
{
        SGR_ASSERT(format <= PixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC_L80);

        bool ret = false;
        switch (format) {
        case PixelFormat::PRIVATE_YCBCR_420_SP_M_SBWC:
        case PixelFormat::PRIVATE_YCBCR_420_SPN_SBWC:
        case PixelFormat::PRIVATE_YCBCR_420_SP_M_10B_SBWC:
        case PixelFormat::PRIVATE_YCBCR_420_SPN_10B_SBWC:
        case PixelFormat::PRIVATE_YCRCB_420_SP_M_SBWC:
        case PixelFormat::PRIVATE_YCRCB_420_SP_M_10B_SBWC:
        case PixelFormat::PRIVATE_YCBCR_420_SPN_256_SBWC:
        case PixelFormat::PRIVATE_YCBCR_420_SPN_10B_256_SBWC:
                ret = true;
                break;
        default:
                break;
        }

        return ret;
}

class SbwcLayoutManager : public LayoutManager {
public:
        SbwcLayoutManager() = default;
        ~SbwcLayoutManager() = default;
        SbwcLayoutManager(const SbwcLayoutManager&) = delete;
        SbwcLayoutManager(const SbwcLayoutManager&&) = delete;
        SbwcLayoutManager& operator=(const SbwcLayoutManager&) = delete;
        SbwcLayoutManager& operator=(const SbwcLayoutManager&&) = delete;

        uint32_t get_alloc_info(PixelFormat format, uint32_t layer_count, uint32_t ip_flags,
                                sgr_extent_2d *alloc_extent, sgr_alloc *allocs, uint32_t *num_planes,
                                sgr_plane_layout *plane_layouts, uint64_t usage) const override;

private:
        uint32_t compute_plane_layout_for_alloc(PixelFormat format, const sgr_extent_2d &alloc_extent,
                                                uint32_t num_planes_per_alloc,
                                                uint32_t *cur_plane_layout_index,
                                                sgr_plane_layout *plane_layouts,
                                                uint32_t luma_size, uint32_t chroma_size) const;
        void get_block_extent(PixelFormat format, sgr_extent_2d *extent) const;
        void get_alloc_extent(PixelFormat format, sgr_extent_2d *alloc_extent) const;
};

} // gralloc
} // samsung
} // android
