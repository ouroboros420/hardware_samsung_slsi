///
/// @file linear_layout_manager.cpp
/// @copyright 2020-2021 Samsung Electronics
///

#include "debug_helper.h"
#include "format_info.h"
#include "format_manager.h"
#include "linear_layout_manager.h"
#include "util/util.h"

namespace android {
namespace samsung {
namespace gralloc {

///
/// @brief Get stride and vstride with alignment requirement
///
/// @param[in]  alloc_extent      Image extent for allocation
/// @param[in]  alloc_align       Allocation information
/// @param[in]  sampling_info     Sampling information
/// @param[in]  sample_bits       Bits for a sample
/// @param[in]  format            Current format
/// @param[in]  plane_index       Currnet Plane index
/// @param[out] stride_in_bytes   Stride in bytes
/// @param[out] vstride_in_pixels VStride in pixels
///
static inline void get_stride(const sgr_extent_2d *alloc_extent,
                              const align_info &alloc_align,
                              const sub_sampling_info *sampling_info,
                              uint32_t sample_bits,
                              const PixelFormat format,
                              const uint32_t *plane_index,
                              uint32_t *stride_in_bytes,
                              uint32_t *vstride_in_pixels)
{
        const sgr_extent_2d sub_sampled_extent = {
                .width  = (alloc_extent->width / sampling_info->hsub),
                .height = (alloc_extent->height / sampling_info->vsub)
        };

        *stride_in_bytes = SGR_ALIGN(convert_bits_to_bytes(sub_sampled_extent.width * sample_bits),
                                     alloc_align.stride_in_bytes);

        // stride_in_bytes are for U,V planes, double it for Y plane
        //
        // launch_api_level == 31 : double stride_in_byte both YV12 & YV12_M (i.e., pamir)
        // launch_api_level >= 33 : double stride_in_byte only YV12 (i.e., from quadra)
        //

        bool is_yv12 = (format == PixelFormat::YV12);
        bool is_yv12m_and_pamir = (format == PixelFormat::PRIVATE_YV12_M) && (!is_launch_api_level_gt_s());

        if ((is_yv12 || is_yv12m_and_pamir) && (*plane_index == 0)) {
                *stride_in_bytes = SGR_ALIGN(convert_bits_to_bytes(sub_sampled_extent.width * sample_bits),
                                             alloc_align.stride_in_bytes * 2);
        }

        *vstride_in_pixels = SGR_ALIGN(sub_sampled_extent.height, alloc_align.vstride_in_pixels);
}

///
/// @brief Compute plane layout and return size of plane for YCBCR_422_I format
///
/// @param[in]  alloc_align            Allocation information
/// @param[in]  alloc_extent           Image extent
/// @param[out] cur_plane_layout_index Number of sgr_plane_layout array
/// @param[out] plane_layouts          Pointer to sgr_plane_layout array
///
/// @return alloc size in byte
///
uint32_t LinearLayoutManager::compute_plane_layout_for_YCBCR_422_I(const align_info &alloc_align,
                                                                   const sgr_extent_2d *alloc_extent,
                                                                   uint32_t *cur_plane_layout_index,
                                                                   sgr_plane_layout *plane_layouts) const
{
        SGR_ASSERT(plane_layouts != nullptr);
        SGR_ASSERT((alloc_extent->width % 2) == 0);

        PixelFormat format = PixelFormat::YCBCR_422_I;
        uint32_t num_components = get_num_components(format);
        const component_info *comp_info = get_component_info(format);

        for (uint32_t i = 0; i < num_components; i++)
        {
                plane_layouts[0].components[i].component_type = comp_info[i].type;
                plane_layouts[0].components[i].size_in_bits   = comp_info[i].bits;
        }

        // Each sample consists of 2 pixels. Correct offset for each components:
        // Y offset  = 0; Cb offset = 8; Cr offset = 24
        plane_layouts[0].components[0].offset_in_bits = 0;
        plane_layouts[0].components[1].offset_in_bits = 8;
        plane_layouts[0].components[2].offset_in_bits = 24;

        static constexpr uint32_t sample_bits     = 16;
        plane_layouts[0].num_components           = num_components;
        plane_layouts[0].offset_in_bytes          = 0;
        plane_layouts[0].sample_increment_in_bits = sample_bits;

        const uint32_t stride_in_bytes = SGR_ALIGN(convert_bits_to_bytes(plane_layouts[0].sample_increment_in_bits) *
                                                   alloc_extent->width,
                                                   alloc_align.stride_in_bytes);

        plane_layouts[0].width_in_samples    = (convert_bytes_to_bits(stride_in_bytes) / sample_bits);
        plane_layouts[0].height_in_samples   = SGR_ALIGN(alloc_extent->height, alloc_align.vstride_in_pixels);
        plane_layouts[0].stride_in_bytes     = stride_in_bytes;
        plane_layouts[0].total_size_in_bytes = stride_in_bytes * plane_layouts[0].height_in_samples;

        *cur_plane_layout_index = 1;

        return plane_layouts[0].total_size_in_bytes;
}

///
/// @brief Compute plane layout and return size of plane.
///        Each call computes for only 1 plane
///
/// @param[in]     format                 Format
/// @param[in]     alloc_align            Allocation information
/// @param[in]     num_planes_per_alloc   Number of planes for currnet alloc
/// @param[in/out] alloc_extent           Image extent
/// @param[out]    cur_plane_layout_index Number of sgr_plane_layout array
/// @param[out]    plane_layouts          Pointer to sgr_plane_layout array
///
/// @return alloc size in byte
///
uint32_t LinearLayoutManager::compute_plane_layout_for_alloc(PixelFormat format,
                                                             const align_info &alloc_align,
                                                             uint32_t num_planes_per_alloc,
                                                             const sgr_extent_2d *alloc_extent,
                                                             uint32_t *cur_plane_layout_index,
                                                             sgr_plane_layout *plane_layouts,
                                                             uint64_t usage) const
{
        SGR_ASSERT(plane_layouts != nullptr);
        SGR_ASSERT(cur_plane_layout_index != nullptr);
        SGR_ASSERT(*cur_plane_layout_index < SGR_MAX_NUM_PLANES);

        ///@todo GFXSW-3940 - support planar format
        uint32_t num_components = get_num_components(format);
        const component_info *comp_info = get_component_info(format);
        uint32_t alloc_size = 0;

        // Plane layout information for S10 formats are different from actual layout.
        for (uint32_t plane = 0; plane < num_planes_per_alloc; plane++) {
                sgr_plane_layout *plane_layout = &plane_layouts[*cur_plane_layout_index];
                uint32_t idx = 0;
                uint32_t sample_bits = 0;
                for (uint i = 0; i < num_components; i++) {
                        if (comp_info[i].plane_index == *cur_plane_layout_index) {
                                plane_layout->components[idx].component_type = comp_info[i].type;
                                plane_layout->components[idx].offset_in_bits = sample_bits;
                                plane_layout->components[idx].size_in_bits   = comp_info[i].bits;
                                sample_bits += comp_info[i].bits;
                                idx++;
                        }
                }

                SGR_ASSERT(idx > 0);
                plane_layout->num_components           = idx;
                plane_layout->offset_in_bytes          = alloc_size;
                plane_layout->sample_increment_in_bits = sample_bits;

                const sub_sampling_info *sub_info = get_sampling_info(format, *cur_plane_layout_index);
                plane_layout->horizontal_subsampling   = sub_info->hsub;
                plane_layout->vertical_subsampling     = sub_info->vsub;

                uint32_t stride_in_bytes = 0;
                uint32_t vstride_in_pixel = 0;

                get_stride(alloc_extent, alloc_align, sub_info, sample_bits, format, cur_plane_layout_index,
                          &stride_in_bytes, &vstride_in_pixel);

                plane_layout->width_in_samples  = (convert_bytes_to_bits(stride_in_bytes) / sample_bits);
                plane_layout->height_in_samples = static_cast<int64_t>(vstride_in_pixel);
                plane_layout->stride_in_bytes = stride_in_bytes;
                plane_layout->total_size_in_bytes =
                        stride_in_bytes * vstride_in_pixel + alloc_align.plane_padding_in_bytes;

                if (is_any_bitmask_set_64(usage, static_cast<uint64_t>(BufferUsage::GPU_MIPMAP_COMPLETE))) {
                        uint32_t mip_level_count = 32 - __builtin_clz(SGR_MAX(alloc_extent->width, alloc_extent->height));

                        for (int i = 1; i < mip_level_count; i++) {
                            // Samsung hardware expects that mip level sizes are rounded up.
                            uint32_t level_width  = (alloc_extent->width >> i) + (((alloc_extent->width & ((1 << i) - 1)) != 0) ? 1 : 0);
                            uint32_t level_height = (alloc_extent->height >> i) + (((alloc_extent->height & ((1 << i) - 1)) != 0) ? 1 : 0);
                            // In linear mipmaps stride alignment is applied to each mip level.
                            plane_layout->total_size_in_bytes += SGR_ALIGN(convert_bits_to_bytes(level_width * sample_bits), alloc_align.stride_in_bytes) * level_height;
                        }
                }

                alloc_size += plane_layout->total_size_in_bytes;
                (*cur_plane_layout_index)++;
        }

        return alloc_size;
}

///
/// @brief Get common alignment for power of two alignment values
///
/// @param[in]     align        alignment structure
/// @param[in/out] common_align alignment structure
///
static inline void get_common_alignment(const align_info &align, align_info *common_align)
{
        SGR_ASSERT(is_power_of_two(common_align->stride_in_bytes));
        SGR_ASSERT(is_power_of_two(align.stride_in_bytes));
        common_align->stride_in_bytes = SGR_MAX(align.stride_in_bytes, common_align->stride_in_bytes);

        SGR_ASSERT(is_power_of_two(common_align->vstride_in_pixels));
        SGR_ASSERT(is_power_of_two(align.vstride_in_pixels));
        common_align->vstride_in_pixels = SGR_MAX(align.vstride_in_pixels, common_align->vstride_in_pixels);

        common_align->plane_padding_in_bytes =
                SGR_MAX(align.plane_padding_in_bytes, common_align->plane_padding_in_bytes);

        common_align->alloc_padding_in_bytes =
                SGR_MAX(align.alloc_padding_in_bytes, common_align->alloc_padding_in_bytes);
}

///
/// @brief Get alloc alignment
///
/// @param[in]  format   Target format
/// @param[in]  ip_flags IP list
///
/// @return alloc_align
///
align_info LinearLayoutManager::get_alloc_align(PixelFormat format, uint32_t ip_flags) const
{
        align_info alloc_align = {
                .stride_in_bytes = 1,
                .vstride_in_pixels = 1,
                .plane_padding_in_bytes = 0,
                .alloc_padding_in_bytes = 0
        };

        for (uint32_t pos = 0; pos < static_cast<uint32_t>(Ip::COUNT) && ip_flags != 0; pos++) {
                if ((ip_flags & 0x1) == 0x1) {
                        align_info ip_align = m_ip_format_manager[pos]->get_linear_alignment(format);
                        get_common_alignment(ip_align, &alloc_align);
                }
                ip_flags >>= 1;
        }

        SGR_LOGV("alloc_align.stride_in_bytes  = %u\n"
                 "alloc_align.vstride_in_pixels = %u\n"
                 "alloc_align.plane_padding_in_bytes = %u\n",
                 alloc_align.stride_in_bytes, alloc_align.vstride_in_pixels, alloc_align.plane_padding_in_bytes);

        return alloc_align;
}

///
/// @brief Get allocation information
///
/// @param[in]     format        Format
/// @param[in]     layer_count   Number of layers
/// @param[in]     ip_flags      IP list
/// @param[in/out] alloc_extent  Image extent
/// @param[out]    allocs        Pointer to sgr_alloc array. If nullptr, then ignored
/// @param[out]    num_planes    Number of sgr_plane_layout array
/// @param[out]    plane_layouts Pointer to sgr_plane_layout array
///
/// @return the number of alloc_info which are updated
///
uint32_t LinearLayoutManager::get_alloc_info(PixelFormat format, uint32_t layer_count, uint32_t ip_flags,
                                           sgr_extent_2d *alloc_extent, sgr_alloc *allocs,
                                           uint32_t *num_planes, sgr_plane_layout *plane_layouts, uint64_t usage) const
{
        SGR_ASSERT(allocs != nullptr);
        SGR_LOGV("Alloc_infos = %p\n"
                 "\t\tformat = %u [%s]\n",
                 allocs, format, get_pixel_format_string(format).c_str());

        const align_info alloc_align = get_alloc_align(format, ip_flags);

        const uint32_t num_allocs = get_num_allocs(format);
        const uint32_t *num_planes_per_alloc = get_num_planes_per_alloc(format);

        for(uint32_t i = 0; i < num_allocs; i++) {
                uint32_t data_size = 0;
                if (format == PixelFormat::YCBCR_422_I) {
                        data_size = compute_plane_layout_for_YCBCR_422_I(alloc_align, alloc_extent,
                                                                         num_planes, plane_layouts);
                } else {
                        data_size = compute_plane_layout_for_alloc(format, alloc_align,
                                                                   num_planes_per_alloc[i], alloc_extent,
                                                                   num_planes, plane_layouts, usage);
                }

                sgr_alloc *alloc = &allocs[i];

                alloc->alignment   = 4 * 1024; // 4 KB

                alloc->data.offset = 0;
                alloc->data.size   = layer_count * data_size;

                alloc->key.offset  = alloc->data.size;
                alloc->key.size    = 0;

                alloc->total_size  = alloc->data.size + alloc_align.alloc_padding_in_bytes;
        }

        alloc_extent->width  = plane_layouts[0].width_in_samples;
        alloc_extent->height = plane_layouts[0].height_in_samples;

        return num_allocs;
}

} // gralloc
} // samsung
} // android
