///
/// @file ion_memory_manager.h
/// @copyright 2020 Samsung Electronics
///

#pragma once

#include "memory_manager.h"

namespace android {
namespace samsung {
namespace gralloc {

class IonMemoryManager : public MemoryManager {
public:
        IonMemoryManager();
        ~IonMemoryManager();
        IonMemoryManager(const MemoryManager&) = delete;
        IonMemoryManager(const MemoryManager&&) = delete;
        IonMemoryManager& operator=(const MemoryManager&) = delete;
        IonMemoryManager& operator=(const MemoryManager&&) = delete;

        virtual int  alloc(size_t length, size_t alignment, uint64_t buffer_usage) override;
        virtual void free(int fd) override;
        virtual bool free_handle(int fd, int *handle) override;
        virtual bool import_handle(int fd, int *handle) override;
        virtual void *map(void *addr, size_t length, int prot, int flags, int fd,
                          off_t offset) override;
        virtual bool unmap(void *addr, size_t length) override;
        virtual bool sync(int fd) override;
        virtual bool sync_partial(int fd, off_t offset, size_t length) override;
        virtual bool sync_start(int fd, uint32_t cpu_usage_flag) override;
        virtual bool sync_end(int fd, uint32_t cpu_usage_flag) override;

private:
        int m_ion_fd;
        bool open();
        void close();
};

} // gralloc
} // samsung
} // android
