///
/// @file private_handle_helper.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include <cstdint>

#include "ip_format_manager.h"
#include "interface/metadata_gralloc.h"
#include "interface/private_handle.h"
#include "layout_manager.h"
#include "util/util.h"

namespace android {
namespace samsung {
namespace gralloc {

#define SIZE_4K  (4 * 1024)

inline uint32_t convert_to_sgr_alloc_format(PixelFormat format)
{
        return static_cast<uint32_t>(format);
}

inline uint32_t convert_to_sgr_alloc_layout(FormatLayout layout)
{
        static_assert(static_cast<uint32_t>(FormatLayout::NONE)   == SGR_ALLOC_LAYOUT_NONE,
                      "Alloc layout mismatch");
        static_assert(static_cast<uint32_t>(FormatLayout::LINEAR) == SGR_ALLOC_LAYOUT_LINEAR,
                      "Alloc layout mismatch");

        return static_cast<uint32_t>(layout);
}

inline private_handle_t *create_private_handle(const BufferDescriptor &descriptor,
                                               const sgr_extent_2d &alloc_extent,
                                               PixelFormat alloc_format,
                                               FormatLayout alloc_layout,
                                               uint32_t num_allocs,
                                               const sgr_alloc *allocs,
                                               uint32_t num_planes,
                                               const sgr_plane_layout *plane_layouts)
{
        SGR_UNUSED(descriptor);
        SGR_UNUSED(alloc_extent);
        SGR_UNUSED(alloc_format);
        SGR_UNUSED(alloc_layout);
        SGR_UNUSED(allocs);
        SGR_UNUSED(num_planes);
        SGR_UNUSED(plane_layouts);

        const uint32_t num_fds = num_allocs + SGR_NUM_METADATA;
        private_handle_t *handle = reinterpret_cast<private_handle_t *>(
                                          native_handle_create(num_fds, SGR_NUM_INTS_IN_PRIVATE_HANDLE(num_fds)));

        if (handle == nullptr) {
                SGR_LOGE("Failed to alloc private_handle_t");
                return nullptr;
        }

        for (uint32_t i = 0; i < SGR_MAX_NUM_FDS; i++) {
                handle->fds[i] = -1;
                handle->handles[i] = -1;
        }

        for (uint32_t i = 0; i < SGR_MAX_NUM_ALLOCATIONS; i++) {
                handle->alloc_bases[i] = 0;
        }

        handle->magic                 = SGR_PRIVATE_HANDLE_MAGIC;
        handle->reserved_region_size  = descriptor.reservedSize;
        handle->metadata_base         = 0;
        handle->pid                   = 0;
        handle->lock_cpu_usage        = 0;
        handle->lock_count            = 0;

        return handle;
}

inline private_handle_t *copy_create_private_handle(const private_handle_t *handle)
{
        SGR_ASSERT(handle != nullptr);
        if (handle == nullptr) {
                SGR_LOGE("Failed to alloc private_handle_t");
                return nullptr;
        }

        auto copy_handle = reinterpret_cast<private_handle_t*>(malloc(sizeof(private_handle_t)));
        memcpy(copy_handle, handle, sizeof(*handle));

        return copy_handle;
}

inline uint32_t get_metdata_fd_index(const private_handle_t *handle)
{
        SGR_ASSERT(handle != nullptr);
        return handle->numFds - SGR_NUM_METADATA;
}


inline bool validate_native_handle(const struct native_handle *handle)
{
        if ((handle == nullptr)   ||
            (handle->numFds <= 0) ||
            (handle->numInts < 0)) {
                return false;
        }

        return true;
}

inline bool is_imported(const private_handle_t *handle) {
        return (handle->metadata_base != 0);
}

inline sgr_metadata *get_metadata(void *metadata_base)
{
        SGR_ASSERT(metadata_base != nullptr);
        return reinterpret_cast<sgr_metadata *>(reinterpret_cast<uint8_t*>(metadata_base) +
                                                SGR_METADATA_OFFSET_GRALLOC);
}

} // gralloc
} // samsung
} // android
