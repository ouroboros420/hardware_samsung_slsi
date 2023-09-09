/*
* @file private_handle.h
* @copyright 2020 Samsung Electronics
*/

#ifndef SGR_PRIVATE_HANDLE_H_
#define SGR_PRIVATE_HANDLE_H_

#include <cutils/native_handle.h>
#include <stdint.h>

#define SGR_NUM_DEFAULT_ALLOCATIONS     1

/*
 * NUM_ALLOCATION(default + private) can grow up to 4 when there is DOWNSCALE BufferUsage
 * With this usage, num_allocs will be doubled
 * And the downscale scenario supports only for YCbCr_420_SP_M & YCbCr_P010_M,
 *      ex) 2 fds for origin image, 2 fds for downscaled image
 */
#define SGR_MAX_NUM_PRIVATE_ALLOCATIONS 3

#define SGR_MAX_NUM_ALLOCATIONS                                         \
        (SGR_NUM_DEFAULT_ALLOCATIONS + SGR_MAX_NUM_PRIVATE_ALLOCATIONS)

#define SGR_NUM_METADATA 1
#define SGR_MAX_NUM_FDS (SGR_MAX_NUM_ALLOCATIONS + SGR_NUM_METADATA)

#define SGR_NUM_INTS_IN_PRIVATE_HANDLE(num_fds)                         \
        ((sizeof(struct private_handle_t) - sizeof(struct native_handle)) / sizeof(int) - \
         num_fds)

/*
 * NUM_PLANES can grow up to 4 when there is DOWNSCALE BufferUsage
 * With this usage, num_plane_layouts will be doubled
 *      ex) 2 planes for origin image, 2 planes for downscaled image
 */
#define SGR_MAX_NUM_PLANES 4

#define SGR_PRIVATE_HANDLE_MAGIC 0x59700795

#define PRIVATE_HANDLE_INTERFACE_VERSION_MAJOR 3
#define PRIVATE_HANDLE_INTERFACE_VERSION_MINOR 0

struct plane_info {
    int64_t size;
    int64_t offset;
    int64_t stride_in_bytes;
    int64_t width_in_samples;
    int64_t height_in_samples;
};

struct alloc_info {
    uint32_t size;
    uint32_t key_offset;
};

#ifdef __cplusplus
struct private_handle_t : public native_handle
{
#else
struct private_handle_t
{
        struct native_handle nativeHandle;
#endif
        // Invariant: native_handle should be followed by fds
        //            only numFds are dupped at import
        //            metadata_fd is located in the valid index, i.e., fds[num_allocs]
        int       fds[SGR_MAX_NUM_FDS];
        uint32_t  magic;

        uint64_t     reserved_region_size;

        // exynos ion handles for tracking who's using the memory
        int       handles[SGR_MAX_NUM_FDS];

        // Process local information
        uint64_t     alloc_bases[SGR_MAX_NUM_ALLOCATIONS];
        uint64_t     metadata_base;
        int       pid;
        uint64_t  lock_cpu_usage;
        int       lock_count;

        // cache buffer metadata for libexynosgraphicbuffer
        uint32_t width;
        uint32_t height;
        uint32_t format;
        uint64_t usage;

        uint32_t alloc_width;
        uint32_t alloc_height;
        uint32_t alloc_format;
        uint32_t alloc_layout;

        uint32_t num_planes;
        uint32_t num_allocs;
        struct plane_info plane_infos[SGR_MAX_NUM_ALLOCATIONS];
        struct alloc_info alloc_infos[SGR_MAX_NUM_PLANES];

        uint32_t major_version;
        uint32_t minor_version;
};

/*
 * Metadata memory is splited into multiple sections for IPs and a user
 * The size should be negotiated with clients and clients should check the sizes are correct
 *
 * Invariant - Metadata size and offset should be 64 byte aligned for simplicity
 * Invariant - VIDEO metadata should be located at first for compatibility
 */
enum SGR_METADATA_SIZE {
        SGR_METADATA_SIZE_VIDEO     = 12288, // ExynosVideoMeta (< 512B)
        SGR_METADATA_SIZE_GRALLOC   = 3072, // sgr_metadata (< 1024B)
        SGR_METADATA_SIZE_GPU       = 1024, // sgr_metadata_gpu (< 64B)
        /* SGR_METADATA_SIZE_USER_RESERVED - defined by user at runtime */
        SGR_METADATA_SIZE_SUB_TOTAL = SGR_METADATA_SIZE_VIDEO + SGR_METADATA_SIZE_GRALLOC + SGR_METADATA_SIZE_GPU,
};

enum SGR_METADATA_OFFSET {
        SGR_METADATA_OFFSET_VIDEO         = 0,
        SGR_METADATA_OFFSET_GRALLOC       = SGR_METADATA_OFFSET_VIDEO   + SGR_METADATA_SIZE_VIDEO,
        SGR_METADATA_OFFSET_GPU           = SGR_METADATA_OFFSET_GRALLOC + SGR_METADATA_SIZE_GRALLOC,
        SGR_METADATA_OFFSET_USER_RESERVED = SGR_METADATA_OFFSET_GPU     + SGR_METADATA_SIZE_GPU,
        SGR_METADATA_OFFSET_COUNT,
};

inline void *sgr_get_metadata_video(const struct native_handle *handle)
{
        const struct private_handle_t *phandle = (const struct private_handle_t*) handle;
        return (phandle->metadata_base == 0) ? NULL :
                (void *)(phandle->metadata_base + SGR_METADATA_OFFSET_VIDEO);
}

inline void *sgr_get_metadata_gralloc(const struct native_handle *handle)
{
        const struct private_handle_t *phandle = (const struct private_handle_t*) handle;
        return (phandle->metadata_base == 0) ? NULL :
                (void *)(phandle->metadata_base + SGR_METADATA_OFFSET_GRALLOC);
}

inline void *sgr_get_metadata_gpu(const struct native_handle *handle)
{
        const struct private_handle_t *phandle = (const struct private_handle_t*) handle;
        return (phandle->metadata_base == 0) ? NULL :
                (void *)(phandle->metadata_base + SGR_METADATA_OFFSET_GPU);
}

inline void *sgr_get_metadata_user_reserved(const struct native_handle *handle)
{
        const struct private_handle_t *phandle = (const struct private_handle_t*) handle;
        return (phandle->metadata_base == 0) ? NULL :
                (void *)(phandle->metadata_base + SGR_METADATA_OFFSET_USER_RESERVED);
}

#endif /*  SGR_PRIVATE_HANDLE_H_ */
