///
/// @file format_manager.h
/// @copyright 2020-2021 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "hidl_defines.h"
#include "ip_format_manager.h"
#include "layout_manager.h"
#include "util/util.h"

namespace android {
namespace samsung {
namespace gralloc {

class FormatManager {
public:
        FormatManager();
        ~FormatManager();
        FormatManager(const FormatManager&) = delete;
        FormatManager(const FormatManager&&) = delete;
        FormatManager& operator=(const FormatManager&) = delete;
        FormatManager& operator=(const FormatManager&&) = delete;

        PixelFormat get_format(const BufferDescriptor &descriptor, FormatLayout *format_layout,
                               uint32_t *ip_flags) const;
        uint32_t get_allocation_info(PixelFormat internal_format, FormatLayout format_layout, uint64_t usage,
                                     uint32_t ip_flags, uint32_t layer_count, sgr_extent_2d *alloc_extent,
                                     sgr_alloc *alloc_infos, uint32_t *num_planes,
                                     sgr_plane_layout *plane_layouts) const;
private:
        IpFormatManager *m_ip_format_manager[static_cast<uint32_t>(Ip::COUNT)];
        LayoutManager   *m_layout_manager[static_cast<uint32_t>(FormatLayout::COUNT)];
};

} // gralloc
} // samsung
} // android
