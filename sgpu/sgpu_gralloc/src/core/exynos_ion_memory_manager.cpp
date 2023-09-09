///
/// @file exynos_ion_memory_manager.cpp
/// @copyright 2020 Samsung Electronics
///

#include <cerrno>
#include <cstring>
#include <hardware/exynos/ion.h>
#include <sys/mman.h>
#include <unistd.h>

#include "exynos_ion_memory_manager.h"
#include "hidl_defines.h"
#include "util/util.h"

namespace android {
namespace samsung {
namespace gralloc {

static inline int convert_to_direction(uint32_t cpu_usage_flag)
{
        static_assert(ION_SYNC_READ  == MemoryManager::CPU_USAGE_READ_BIT, "Memory Manager cpu read usage mismatch");
        static_assert(ION_SYNC_WRITE == MemoryManager::CPU_USAGE_WRITE_BIT, "Memory Manager cpu write usage mismatch");

        int direction = 0;

        if (is_any_bitmask_set(cpu_usage_flag, static_cast<uint32_t>(BufferUsage::CPU_READ_MASK))){
                direction |= ION_SYNC_READ;
        }

        if (is_any_bitmask_set(cpu_usage_flag, static_cast<uint32_t>(BufferUsage::CPU_WRITE_MASK))){
                direction |= ION_SYNC_WRITE;
        }

        return direction;
}

static inline int convert_to_heap_mask(uint64_t buffer_usage)
{
	uint64_t heap_mask = EXYNOS_ION_HEAP_SYSTEM_MASK;

        if (is_all_bitmask_set_64(buffer_usage,
                                  static_cast<uint64_t>(BufferUsage::PRIVATE_SECURE_CAMERA_RESERVED))) {
                heap_mask = EXYNOS_ION_HEAP_SECURE_CAMERA_MASK;
        } else if (is_all_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::PROTECTED))) {
                heap_mask = EXYNOS_ION_HEAP_VIDEO_FRAME_MASK;

                constexpr uint64_t hwc_mask = static_cast<uint64_t>(BufferUsage::COMPOSER_OVERLAY);
                constexpr uint64_t gpu_mask = static_cast<uint64_t>(BufferUsage::GPU_TEXTURE) |
                        static_cast<uint64_t>(BufferUsage::GPU_RENDER_TARGET);
                if (is_all_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::PRIVATE_PRIVATE_NONSECURE)) ||
                    /* W/A to Android R Camera vts_r5. (W/A requested by Visual S/W group MCD) */
                    is_all_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::PRIVATE_CAMERA_RESERVED))) {
                        heap_mask = EXYNOS_ION_HEAP_SYSTEM_MASK;
                } else if (is_all_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::PRIVATE_VIDEO_EXT))) {
                        heap_mask = EXYNOS_ION_HEAP_VIDEO_STREAM_MASK;
                } else if (is_all_bitmask_set_64(buffer_usage, hwc_mask) == true &&
                           is_any_bitmask_set_64(buffer_usage, gpu_mask) == false) {
                        heap_mask = EXYNOS_ION_HEAP_VIDEO_SCALER_MASK;
                }
        } else if (is_all_bitmask_set_64(buffer_usage,
                                         static_cast<uint64_t>(BufferUsage::PRIVATE_CAMERA_RESERVED))) {
                heap_mask = EXYNOS_ION_HEAP_CAMERA_MASK;
        }

        return heap_mask;
}

static inline int convert_to_flags(uint64_t buffer_usage)
{
        int flags = 0;

        if (is_any_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::CPU_READ_OFTEN))) {
                flags |= (ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC);

                if (is_any_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::GPU_RENDER_TARGET))) {
                        flags |= ION_FLAG_SYNC_FORCE;
                }
        }

        constexpr uint64_t gpu_usage = (static_cast<uint64_t>(BufferUsage::GPU_TEXTURE) |
                                        static_cast<uint64_t>(BufferUsage::GPU_RENDER_TARGET));
        if (is_any_bitmask_set_64(buffer_usage, gpu_usage)) {
                flags |= ION_FLAG_MAY_HWRENDER;
        }

        if (is_any_bitmask_set_64(buffer_usage, static_cast<uint64_t>(BufferUsage::PRIVATE_NOZEROED))) {
                flags |= ION_FLAG_NOZEROED;
        }

        constexpr uint64_t protected_usage = (static_cast<uint64_t>(BufferUsage::PROTECTED) |
                                              static_cast<uint64_t>(BufferUsage::PRIVATE_SECURE_CAMERA_RESERVED));
        if (is_any_bitmask_set_64(buffer_usage, protected_usage)) {
                /* W/A to Android R Camera vts_r5. (W/A requested by Visual S/W group MCD) */
                if (is_any_bitmask_set_64(buffer_usage,
                                          static_cast<uint64_t>(BufferUsage::PRIVATE_CAMERA_RESERVED)) == false) {
                        flags |= ION_FLAG_PROTECTED;
                }
        }

        return flags;
}

///
/// @brief ExynosIonMemoryManager constructor
///
ExynosIonMemoryManager::ExynosIonMemoryManager()
{
        bool ret = open();
        SGR_ASSERT_MSG(ret == true, "Failed to open Exynos Ion MemoryManager");
}

///
/// @brief ExynosIonMemoryManager destructor
///
ExynosIonMemoryManager::~ExynosIonMemoryManager()
{
        close();
}

///
/// @brief Open ion node
///
/// @param this_matches What the parameter is used for.
/// @param this_also_matches What the parameter is used for.
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::open()
{
        int ion_fd = exynos_ion_open();
        if (ion_fd >= 0) {
                m_ion_fd = ion_fd;
                return true;
        }

        return false;
}

///
/// @brief This closes ion node
///
void ExynosIonMemoryManager::close()
{
        if (m_ion_fd != -1) {
                exynos_ion_close(m_ion_fd);
                m_ion_fd = -1;
        }
}

///
/// @brief Allocate dma_buf
///
/// @param length Size of a buffer in bytes
/// @param alignment Start address alignment
/// @param buffer_usage Android graphics BufferUsage
///
/// @return dma_buf fd if it succeeds, otherwise -1
///
int ExynosIonMemoryManager::alloc(size_t length, size_t alignment, uint64_t buffer_usage)
{
        SGR_UNUSED(alignment);

        uint32_t heap_mask = convert_to_heap_mask(buffer_usage);
        uint32_t flags = convert_to_flags(buffer_usage);

        SGR_ASSERT(m_ion_fd >= 0);

        return exynos_ion_alloc(m_ion_fd, length, heap_mask, flags);
}

///
/// @brief Free dma_buf
///
/// @param fd Dma_buf fd
///
void ExynosIonMemoryManager::free(int fd)
{
        ::close(fd);
}

///
/// @brief Free ION handle
///
/// @param fd Dma_buf fd
/// @param handle ION handle
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::free_handle(int fd, int *handle)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int ret = 0;
        if (fd)
                ret = exynos_ion_free_handle(m_ion_fd, *handle);
        *handle = -1;

        return (ret >= 0);
}

///
/// @brief Import ION handle to let ION driver know who's using the buffer
///
/// @param fd Dma_buf fd
/// @param handle ION handle
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::import_handle(int fd, int *handle)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int ret = exynos_ion_import_handle(m_ion_fd, fd, handle);

        return (ret >= 0);
}

///
/// @brief Mmap dma_buf. This is a simple wrapper of linux mmap
///
/// @param addr Requested start address of mapping. NULL is accepted
/// @param length Length of mapping
/// @param prot Desired memory protection of the mapping
/// @param flags Accessibility of mapping
/// @param fd Dma_buf fd
/// @param offset Offset from the file in bytes
///
/// @return Mapped address if it succeeds, otherwise NULL
///
void *ExynosIonMemoryManager::map(void *addr, size_t length, int prot, int flags,
                            int fd, off_t offset)
{
        void *ret = mmap(addr, length, prot, flags, fd, offset);
        if (ret == MAP_FAILED) {
                SGR_LOGE("Failed to mmap fd: %d, addr: %p, length %zu, error %s",
                             fd, addr, length, std::strerror(errno));
                return nullptr;
        }
        return ret;
}

///
/// @brief Munmap dma_buf
///
/// @param addr Mapped address
/// @param length Length of mapping
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::unmap(void *addr, size_t length)
{
        int ret = munmap(addr, length);
        if (ret < 0) {
                SGR_LOGE("Failed to unmap addr: %p, length %zu, error %s",
                             addr, length, std::strerror(errno));
                return false;
        }
        return true;
}

///
/// @brief Synchronize the file descriptor with memory
///
/// @param fd Dma_buf fd
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::sync(int fd)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int ret = exynos_ion_sync_fd(m_ion_fd, fd);

        return (ret >= 0);
}

///
/// @brief Synchronize the file descriptor with partial memory
///
/// @param fd Dma_buf fd
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::sync_partial(int fd, off_t offset, size_t length)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int ret = exynos_ion_sync_fd_partial(m_ion_fd, fd, offset, length);

        return (ret >= 0);
}

///
/// @brief Inform of sync start of the file descriptor with cpu usage
///
/// @param fd Dma_buf fd
/// @param cpu_usage_flag Cpu usage flag
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::sync_start(int fd, uint32_t cpu_usage_flag)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int direction = convert_to_direction(cpu_usage_flag);
        int ret = exynos_ion_sync_start(m_ion_fd, fd, direction);

        return (ret >= 0);
}

///
/// @brief Inform of end start of the file descriptor with cpu usage
///
/// @param fd Dma_buf fd
/// @param cpu_usage_flag Cpu usage flag
///
/// @return True if it succeeds, otherwise false
///
bool ExynosIonMemoryManager::sync_end(int fd, uint32_t cpu_usage_flag)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int direction = convert_to_direction(cpu_usage_flag);
        int ret = exynos_ion_sync_end(m_ion_fd, fd, direction);

        return (ret >= 0);
}

} // gralloc
} // samsung
} // android
