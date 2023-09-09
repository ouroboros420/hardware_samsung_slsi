///
/// @file dcc_layout_manager.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include "format_info.h"
#include "hidl_defines.h"
#include "interface/metadata_gralloc.h"
#include "layout_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class DccLayoutManager : public LayoutManager {
public:
        DccLayoutManager() = default;
        ~DccLayoutManager() = default;
        DccLayoutManager(const DccLayoutManager&) = delete;
        DccLayoutManager(const DccLayoutManager&&) = delete;
        DccLayoutManager& operator=(const DccLayoutManager&) = delete;
        DccLayoutManager& operator=(const DccLayoutManager&&) = delete;

        uint32_t get_alloc_info(PixelFormat format, uint32_t layer_count, uint32_t ip_flags,
                                sgr_extent_2d *alloc_extent, sgr_alloc *allocs, uint32_t *num_planes,
                                sgr_plane_layout *plane_layouts, uint64_t usage) const override;

private:
        void get_block_extent(PixelFormat format, sgr_extent_2d *extent) const;
        void get_alloc_extent(PixelFormat format, sgr_extent_2d *alloc_extent) const;
};

} // gralloc
} // samsung
} // android
