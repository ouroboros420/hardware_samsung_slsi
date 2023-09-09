/*
* @file metadata_gralloc.h
* @copyright 2020 Samsung Electronics
*/

#ifndef SGR_METADATA_GRALLOC_H_
#define SGR_METADATA_GRALLOC_H_

#include <stdint.h>

#include "private_handle.h"

/*
 * Android definition
 * These are from Android Gralloc4 definition, but could be slightly different
 */

#define SGR_MAX_LENGTH_NAME 256
#define SGR_MAX_NUM_PLANE_LAYOUT_COMPONENTS 4
#define SGR_MAX_LENGTH_SMPTE2094_40 1024 ///@todo GFXSW-4145 - Figure out the correct value

enum SGR_PLANE_LAYOUT_COMPONENT_TYPE {
        Y   = (1 << 0),
        CB  = (1 << 1),
        CR  = (1 << 2),
        R   = (1 << 10),
        G   = (1 << 11),
        B   = (1 << 12),
        RAW = (1 << 20),
        A   = (1 << 30),
};

struct sgr_rect {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
};

struct sgr_plane_layout_component {
        int64_t component_type;  // ExtendableType with SGR_PLANE_LAYOUT_COMPONENT_TYPE
        int64_t offset_in_bits;
        int64_t size_in_bits;
};

struct sgr_plane_layout {
        int64_t num_components;
        struct sgr_plane_layout_component components[SGR_MAX_NUM_PLANE_LAYOUT_COMPONENTS];
        int64_t offset_in_bytes;
        int64_t sample_increment_in_bits;
        int64_t stride_in_bytes;
        int64_t width_in_samples;
        int64_t height_in_samples;
        int64_t total_size_in_bytes;
        int64_t horizontal_subsampling;
        int64_t vertical_subsampling;
};

struct sgr_xy_color {
        float x;
        float y;
};

struct sgr_smpte2086 {
        sgr_xy_color primary_red;
        sgr_xy_color primary_green;
        sgr_xy_color primary_blue;
        sgr_xy_color white_point;
        float max_luminance;
        float min_luminance;
};

struct sgr_cta861_3 {
        float max_content_light_level;
        float max_frame_average_light_level;
};

enum SGR_ALLOC_LAYOUT {
        SGR_ALLOC_LAYOUT_NONE = 0,
        SGR_ALLOC_LAYOUT_LINEAR,
        SGR_ALLOC_LAYOUT_DCC,
        SGR_ALLOC_LAYOUT_SBWC,
        SGR_ALLOC_LAYOUT_COUNT,
};

struct sgr_extent_2d {
        uint32_t width;
        uint32_t height;
};

struct sgr_section {
        uint32_t offset;
        uint32_t size;
};

/* Memory layout
 * - A buffer can have one or more allocations
 * - An allocation corresponds one fd and consists of data and key as follows
 *   - A data contains uncompressed/compressed data and can have multiple planes
 *   - A key contains opaque compressed keys of the data if any. For uncompressed buffers,
 *     its size is 0
 * - A buffer can have one or more planes which reside in one of allocations
 * - For layered buffers, the above structure is repeated as the number of layers
 */

struct sgr_alloc {
        uint32_t    alignment;
        uint32_t    total_size;
        sgr_section data;
        sgr_section key;

        sgr_section payload[2];
        sgr_section header[2];
};

///@todo GFXSW-4148 will be replaced by sgr_plane_layout
struct sgr_plane {
        uint32_t alloc_index;
        uint32_t offset;
        uint32_t pixel_stride;
        uint32_t row_stride;
};

struct sgr_metadata {
        // Standard Metadata
        uint64_t         buffer_id;
        uint64_t         length_name;
        char             name[SGR_MAX_LENGTH_NAME];
        uint64_t         width;
        uint64_t         height;
        uint64_t         layer_count;
        int32_t          pixel_format_requested;
        uint32_t         pixel_format_fourcc;
        uint64_t         pixel_format_modifier;
        uint64_t         usage;
        uint64_t         allocation_size;
        uint64_t         protected_content;
        uint32_t         compression;           // ExtendableType
        uint32_t         interlaced;            // ExtendableType
        uint32_t         chroma_siting;         // ExtendableType
        uint64_t         num_plane_layouts;
        struct sgr_plane_layout plane_layouts[SGR_MAX_NUM_PLANES];
        uint64_t         num_crops;
        struct sgr_rect  crops[SGR_MAX_NUM_PLANES];
        int32_t          dataspace;
        int32_t          blend_mode;
        bool             is_smpte2086_set;
        struct sgr_smpte2086 smpte2086;         // android.hardware.graphics.common.Smpte2086
        bool             is_cta861_3_set;
        struct sgr_cta861_3 cta861_3;           // android.hardware.graphics.common.Cta861_3
        uint64_t         length_smpte2094_40;
        uint8_t          smpte2094_40[SGR_MAX_LENGTH_SMPTE2094_40];

        // Allocation info
        uint32_t         alloc_width;
        uint32_t         alloc_height;
        uint32_t         alloc_format; // HAL_PIXEL_FORMAT
        uint32_t         alloc_layout; // SGR_ALLOC_LAYOUT
        uint32_t         num_allocs;
        struct sgr_alloc allocs[SGR_MAX_NUM_ALLOCATIONS];

        // Sub Allocation info
        bool             sub_valid;
};

inline struct sgr_metadata *sgr_get_metadata(const struct native_handle *handle)
{
        return (struct sgr_metadata *) sgr_get_metadata_gralloc(handle);
}

#endif /*  SGR_METADATA_GRALLOC_H_ */
