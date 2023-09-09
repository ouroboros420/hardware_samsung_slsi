///
/// @file format_info.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include <stdint.h>

#include "hidl_defines.h"
#include "interface/metadata_gralloc.h"
#include "interface/private_handle.h"

namespace android {
namespace samsung {
namespace gralloc {

enum class SamplingType : uint32_t {
        RGB     = 0,
        YUV_444 = 1,
        YUV_422 = 2,
        YUV_420 = 3
};

struct sub_sampling_info {
        uint32_t hsub; /*Horizontal Sub sampling factor*/
        uint32_t vsub; /*Vertical Sub sampling factor*/
};

struct component_info {
        uint32_t plane_index;
        SGR_PLANE_LAYOUT_COMPONENT_TYPE type;
        uint32_t bits;
};

struct format_info {
        PixelFormat id;                                           /* Format ID */
        uint32_t num_allocs;                                      /* Number of allocations (Fds) for this format */
        uint32_t num_planes_per_alloc[SGR_MAX_NUM_ALLOCATIONS];   /* Num planes for each allocation, sum of planes for
                                                                     all allocs cannot exceed SGR_MAX_NUM_PLANES*/
        uint32_t num_components;                                  /* Number of Components in the format */
        component_info info[SGR_MAX_NUM_PLANE_LAYOUT_COMPONENTS]; /* Plane index, format type and bits per component */
        SamplingType format_sampling;                             /* Index for sub_sampling_info_array[] */
        uint32_t format_fourcc;                                   /* drm fourcc format */
};

uint32_t get_num_components(PixelFormat format);
uint32_t get_num_allocs(PixelFormat format);
const uint32_t *get_num_planes_per_alloc(PixelFormat format);
const component_info *get_component_info(PixelFormat format);
const sub_sampling_info *get_sampling_info(PixelFormat format, uint32_t plane_index);
bool is_yuv_format(PixelFormat format);
uint32_t get_alloc_index(PixelFormat format, uint32_t plane_index);
uint32_t get_pixel_format_fourcc(PixelFormat format);
bool is_sbwc_format(PixelFormat format);

uint32_t get_bps(PixelFormat format);

} // gralloc
} // samsung
} // android
