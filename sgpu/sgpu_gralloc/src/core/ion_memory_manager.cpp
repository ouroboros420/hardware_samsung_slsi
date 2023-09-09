///
/// @file ion_memory_manager.cpp
/// @copyright 2020 Samsung Electronics
///

#include <cerrno>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

#include <ion/ion.h>
#include "ion_memory_manager.h"
#include <util/util.h>

#define __STDC_FORMAT_MACROS
#include <cinttypes>

namespace android {
namespace samsung {
namespace gralloc {

static inline int convert_to_heap_mask(uint64_t /* buffer_usage */)
{
        return ION_HEAP_SYSTEM_MASK;
}

static inline int convert_to_flags(uint64_t /* buffer_usage */)
{
        return 0;
}

///
/// @brief IonMemoryManager constructor
///
IonMemoryManager::IonMemoryManager()
{
        bool ret = open();
        SGR_ASSERT_MSG(ret == true, "Failed to open IonMemoryManager");
}

///
/// @brief IonMemoryManager destructor
///
IonMemoryManager::~IonMemoryManager()
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
bool IonMemoryManager::open()
{
        int ion_fd = ion_open();
        if (ion_fd >= 0) {
                m_ion_fd = ion_fd;
                return true;
        }

        return false;
}

///
/// @brief This closes ion node
///
void IonMemoryManager::close()
{
        if (m_ion_fd != -1) {
                ion_close(m_ion_fd);
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
int IonMemoryManager::alloc(size_t length, size_t alignment, uint64_t buffer_usage)
{
        uint32_t heap_mask = convert_to_heap_mask(buffer_usage);
        uint32_t flags = convert_to_flags(buffer_usage);

        SGR_ASSERT(m_ion_fd >= 0);

        int fd = -1;
        int err = ion_alloc_fd(m_ion_fd, length, alignment, heap_mask, flags, &fd);
        if (err == 0) {
                SGR_LOGV("Alloc success, fd: %d, length %zu usages 0x%" PRIx64,
                             fd, length, buffer_usage);
        } else {
                SGR_LOGE("Failed to alloc fd: %d, length %zu, usages 0x%" PRIx64,
                             fd, length, buffer_usage);
        }
        return fd;
}

///
/// @brief Free dma_buf
///
/// @param fd Dma_buf fd
///
void IonMemoryManager::free(int fd)
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
bool IonMemoryManager::free_handle(int fd, int *handle)
{
        SGR_ASSERT(m_ion_fd >= 0);

        /// Does not suport memtracking for ion buffers
        SGR_UNUSED(fd);
        SGR_UNUSED(handle);

        return (fd >= 0);
}

///
/// @brief Import ION handle to let ION driver know who's using the buffer
///
/// @param fd Dma_buf fd
/// @param handle ION handle
///
/// @return True if it succeeds, otherwise false
///
bool IonMemoryManager::import_handle(int fd, int *handle)
{
        SGR_ASSERT(m_ion_fd >= 0);

        /// Does not suport memtracking for ion buffers
        SGR_UNUSED(fd);
        SGR_UNUSED(handle);

        return (fd >= 0);
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
void *IonMemoryManager::map(void *addr, size_t length, int prot, int flags,
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
bool IonMemoryManager::unmap(void *addr, size_t length)
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
bool IonMemoryManager::sync(int fd)
{
        SGR_ASSERT(m_ion_fd >= 0);

        int ret = ion_sync_fd(m_ion_fd, fd);

        return (ret >= 0);
}

///
/// @brief Synchronize the file descriptor with partial memory
///
/// @param fd Dma_buf fd
///
/// @return True if it succeeds, otherwise false
///
bool IonMemoryManager::sync_partial(int fd, off_t offset, size_t length)
{
        SGR_ASSERT(m_ion_fd >= 0);

        SGR_UNUSED(offset);
        SGR_UNUSED(length);

        return (fd >= 0);
}

///
/// @brief Inform of sync start of the file descriptor with cpu usage
///
/// @param fd Dma_buf fd
/// @param cpu_usage_flag Cpu usage flag
///
/// @return True if it succeeds, otherwise false
///
bool IonMemoryManager::sync_start(int fd, uint32_t cpu_usage_flag)
{
        SGR_ASSERT(m_ion_fd >= 0);

        SGR_UNUSED(cpu_usage_flag);

        return (fd >= 0);
}

///
/// @brief Inform of end start of the file descriptor with cpu usage
///
/// @param fd Dma_buf fd
/// @param cpu_usage_flag Cpu usage flag
///
/// @return True if it succeeds, otherwise false
///
bool IonMemoryManager::sync_end(int fd, uint32_t cpu_usage_flag)
{
        SGR_ASSERT(m_ion_fd >= 0);

        SGR_UNUSED(cpu_usage_flag);

        return (fd >= 0);
}

} // gralloc
} // samsung
} // android
