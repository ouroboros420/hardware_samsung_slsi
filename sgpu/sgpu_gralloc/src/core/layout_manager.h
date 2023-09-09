///
/// @file layout_manager.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include "hidl_defines.h"
#include "interface/metadata_gralloc.h"

namespace android {
namespace samsung {
namespace gralloc {

enum class FormatLayout : uint32_t {
        NONE = 0,
        LINEAR,
        DCC,
        SBWC,
        COUNT
};

enum class FormatLayoutBitMask : uint32_t {
        NONE    = (0x0 << static_cast<uint32_t>(FormatLayout::NONE)), /* Format unsupported */
        LINEAR  = (0x1 << static_cast<uint32_t>(FormatLayout::LINEAR)), /* Format supports uncompressed */
        DCC     = (0x1 << static_cast<uint32_t>(FormatLayout::DCC)), /* DCC compressed */
        SBWC    = (0x1 << static_cast<uint32_t>(FormatLayout::SBWC)), /* SWBC compressed */
        ALL     = ((0x1 << static_cast<uint32_t>(FormatLayout::COUNT)) - 1),
};

class LayoutManager {
public:
        virtual ~LayoutManager() = default;
        LayoutManager(const LayoutManager&) = delete;
        LayoutManager(const LayoutManager&&) = delete;
        LayoutManager& operator=(const LayoutManager&) = delete;
        LayoutManager& operator=(const LayoutManager&&) = delete;

        virtual uint32_t get_alloc_info(PixelFormat format, uint32_t layer_count, uint32_t ip_flags,
                                        sgr_extent_2d *alloc_extent, sgr_alloc *allocs, uint32_t *num_planes,
                                        sgr_plane_layout *plane_layouts, uint64_t usage) const = 0;

protected:
        LayoutManager() = default;
};

} // gralloc
} // samsung
} // android
