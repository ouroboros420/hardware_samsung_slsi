///
/// @file linear_layout_manager.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include "hidl_defines.h"
#include "interface/metadata_gralloc.h"
#include "layout_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class IpFormatManager;

class LinearLayoutManager : public LayoutManager {
public:
        LinearLayoutManager() = delete;
        LinearLayoutManager(IpFormatManager **ip_format_manager)
                : m_ip_format_manager(ip_format_manager) {}
        ~LinearLayoutManager() { m_ip_format_manager = nullptr; }
        LinearLayoutManager(const LinearLayoutManager&) = delete;
        LinearLayoutManager(const LinearLayoutManager&&) = delete;
        LinearLayoutManager& operator=(const LinearLayoutManager&) = delete;
        LinearLayoutManager& operator=(const LinearLayoutManager&&) = delete;

        uint32_t get_alloc_info(PixelFormat format, uint32_t layer_count, uint32_t ip_flags,
                                sgr_extent_2d *alloc_extent, sgr_alloc *allocs, uint32_t *num_planes,
                                sgr_plane_layout *plane_layouts, uint64_t usage) const override;

private:
        uint32_t compute_plane_layout_for_alloc(PixelFormat format, const align_info &alloc_align,
                                                uint32_t num_planes_per_alloc, const sgr_extent_2d *alloc_extent,
                                                uint32_t *cur_plane_layout_index,
                                                sgr_plane_layout *plane_layouts, uint64_t usage) const;
        uint32_t compute_plane_layout_for_YCBCR_422_I(const align_info &alloc_align,
                                                      const sgr_extent_2d *alloc_extent,
                                                      uint32_t *cur_plane_layout_index,
                                                      sgr_plane_layout *plane_layouts) const;
        align_info get_alloc_align(PixelFormat format, uint32_t ip_flags) const;

private:
        const IpFormatManager * const *m_ip_format_manager;
};

} // gralloc
} // samsung
} // android
