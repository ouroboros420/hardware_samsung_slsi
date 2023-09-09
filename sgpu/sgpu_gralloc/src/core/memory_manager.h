///
/// @file memory_manager.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include<cstdint>
#include<cstdlib>

namespace android {
namespace samsung {
namespace gralloc {

class MemoryManager {
public:
        enum CpuUsageFlagBits {
                CPU_USAGE_READ_BIT  = (0x1 << 0),
                CPU_USAGE_WRITE_BIT = (0x1 << 1),
        };

public:
        virtual ~MemoryManager() = default;
        MemoryManager(const MemoryManager&) = delete;
        MemoryManager(const MemoryManager&&) = delete;
        MemoryManager& operator=(const MemoryManager&) = delete;
        MemoryManager& operator=(const MemoryManager&&) = delete;

        virtual int  alloc(size_t length, size_t alignment, uint64_t buffer_usage) = 0;
        virtual void free(int fd) = 0;
        virtual bool free_handle(int fd, int *handle) = 0;
        virtual bool import_handle(int fd, int *handle) = 0;
        virtual void *map(void *addr, size_t length, int prot, int flags, int fd,
                          off_t offset) = 0;
        virtual bool unmap(void *addr, size_t length) = 0;
        virtual bool sync(int fd) = 0;
        virtual bool sync_partial(int fd, off_t offset, size_t length) = 0;
        virtual bool sync_start(int fd, uint32_t cpu_usage_flag) = 0;
        virtual bool sync_end(int fd, uint32_t cpu_usage_flag) = 0;

protected:
        MemoryManager() = default;
};

} // gralloc
} // samsung
} // android
