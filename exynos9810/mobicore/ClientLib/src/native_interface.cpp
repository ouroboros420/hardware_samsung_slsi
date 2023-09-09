/*
 * Copyright (c) 2016 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <vector>

#include <sys/mman.h>

#include "dynamic_log.h"

#include "common.h"

#define BUFFER_LENGTH_MAX 0x100000

using namespace trustonic;

// Open/close count
static std::mutex open_close_lock;
static int open_count;

static Common& common = Common::getInstance();

// Proprietary

void TEEC_TT_RegisterPlatformContext(
    void*                   globalContext,
    void*                   localContext) {
    ENTER();
    if (!globalContext || !localContext) {
        return;
    }
    common.TEEC_TT_RegisterPlatformContext(globalContext, localContext);
    EXIT_NORETURN();
}


// Global Platform

// Keep internal track of allocated shared memories
class _TEECAllocatedSharedMemories {
    std::mutex shared_mems_mutex_;
    std::vector<TEEC_SharedMemory> shared_mems_;
public:
    bool allocate(TEEC_SharedMemory* shared_mem) {
        // Allocate memory (use size 1 if 0 given)
        size_t size = shared_mem->size;
        if (!size) {
            size++;
        }
        shared_mem->buffer = ::mmap(0, size, PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (shared_mem->buffer == MAP_FAILED) {
            shared_mem->buffer = nullptr;
            return false;
        }
        std::lock_guard<std::mutex> lock(shared_mems_mutex_);
        shared_mems_.push_back(*shared_mem);
        LOG_D("allocated shared mem, size %zu", shared_mems_.size());
        return true;
    }

    void free(TEEC_SharedMemory* shared_mem) {
        std::lock_guard<std::mutex> lock(shared_mems_mutex_);
        auto it = std::find_if(shared_mems_.begin(), shared_mems_.end(),
                               [shared_mem](const TEEC_SharedMemory& other) {
            return shared_mem->buffer == other.buffer;
        });
        if (it != shared_mems_.end()) {
            // Free allocated memory (use size 1 if 0 given)
            size_t size = shared_mem->size;
            if (!size) {
                size++;
            }
            ::munmap(shared_mem->buffer, size);
            shared_mem->buffer = nullptr;
            shared_mem->size = 0;
            shared_mems_.erase(it);
            LOG_D("freed shared mem, size %zu", shared_mems_.size());
        }
    }
};

static _TEECAllocatedSharedMemories allocated_shared_mems;

// Keep internal track of operations, to properly deal with cancellation
class _TEECOngoingOperations {
    static const uint32_t ENDED = 0xf;
    std::mutex operation_id_counter_mutex_;
    uint32_t operation_id_counter_ = 0x421;
    // List management
    std::mutex mutex_;
    std::vector<TEEC_Operation*> operations_;
    // Manage a 28-bit counter for operation IDs
    uint32_t getOperationId() {
        std::lock_guard<std::mutex> lock(operation_id_counter_mutex_);
        operation_id_counter_++;
        if (!(operation_id_counter_ & 0xfffffff)) {
            operation_id_counter_ = 1;
        }
        return operation_id_counter_;
    }

public:
    bool start(TEEC_Operation* operation) {
        LOG_D("operation %p start", operation);
        std::lock_guard<std::mutex> lock(mutex_);
        // Check for early cancellation
        auto it = std::find(operations_.begin(), operations_.end(), operation);
        if (it != operations_.end()) {
            LOG_I("operation cancelled before start");
            operations_.erase(it);
            return false;
        }
        auto operation_id = getOperationId();
        // Leave space for status bits below operation ID
        operation->started = operation_id << 4;
        LOG_D("operation %p (ID %u) started", operation, operation_id);
        return true;
    }

    void end(TEEC_Operation* operation) {
        LOG_D("operation %p end", operation);
        std::lock_guard<std::mutex> lock(mutex_);
        operation->started = ENDED;
        LOG_D("operation %p ended", operation);
    }

    void cancel(IClient& client, TEEC_Operation* operation) {
        LOG_D("operation %p cancel", operation);
        std::lock_guard<std::mutex> lock(mutex_);
        // Operation not started yet, store pointer
        if (operation->started == 0) {
            if (std::find(operations_.begin(), operations_.end(), operation) == operations_.end()) {
                operations_.push_back(operation);
                LOG_D("operation %p cancellation recorded", operation);
            } else {
                LOG_D("operation %p cancellation was already recorded", operation);
            }
            return;
        }
        // Get status to be returned
        uint32_t operation_id = operation->started >> 4;
        LOG_D("operation %p (ID %u) cancelled, was %songoing", operation, operation_id,
              operation_id ? "" : "not");
        if (operation_id) {
            // Call under lock so state/operation ID cannot change behind our back
            client.TEEC_RequestCancellation(operation);
        } else {
            LOG_I("operation ended before cancellation");
        }
    }
};

static _TEECOngoingOperations operations;

// For test purpose
static bool set_open_mode(uint32_t mode) {
    IClient::OpenMode open_mode;
    bool device_id_ok = true;
    switch (mode) {
        case static_cast<int>(IClient::OpenMode::AUTO):
            open_mode = IClient::OpenMode::AUTO;
            break;
        case static_cast<int>(IClient::OpenMode::DRIVER):
            open_mode = IClient::OpenMode::DRIVER;
            break;
        case static_cast<int>(IClient::OpenMode::NONE):
            open_mode = IClient::OpenMode::NONE;
            break;
        default:
            open_mode = IClient::OpenMode::AUTO;
            device_id_ok = false;
    }
    common.setOpenMode(open_mode);
    return device_id_ok;
}

TEEC_Result TEEC_InitializeContext(
    const char*             name,
    TEEC_Context*           context) {
    ENTER();
    std::lock_guard<std::mutex> lock(open_close_lock);
    // Check parameters
    if (!context) {
        LOG_E("context is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    set_open_mode(context->imp.reserved);
    TEEC_Result teec_result = common.TEEC_InitializeContext(name, context);
    if (teec_result == TEEC_SUCCESS) {
        open_count++;
    }
    EXIT(teec_result);
}

void TEEC_FinalizeContext(
    TEEC_Context*           context) {
    ENTER();
    std::lock_guard<std::mutex> lock(open_close_lock);
    // Check parameters
    if (!context) {
        LOG_E("context is null");
        EXIT_NORETURN();
    }

    common.TEEC_FinalizeContext(context);
    open_count--;
    EXIT_NORETURN();
}

TEEC_Result TEEC_RegisterSharedMemory(
    TEEC_Context*           context,
    TEEC_SharedMemory*      sharedMem) {
    ENTER();
    // Check parameters
    if (!context) {
        LOG_E("context is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (!sharedMem) {
        LOG_E("sharedMem is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (!sharedMem->buffer) {
        LOG_E("sharedMem->buffer is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if ((static_cast<int>(sharedMem->flags) & ~TEEC_MEM_INOUT)) {
        LOG_E("sharedMem->flags is incorrect");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (sharedMem->flags == 0) {
        LOG_E("sharedMem->flags is incorrect");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }

    EXIT(common.TEEC_RegisterSharedMemory(context, sharedMem));
}

TEEC_Result TEEC_AllocateSharedMemory(
    TEEC_Context*           context,
    TEEC_SharedMemory*      sharedMem) {
    ENTER();
    // Check parameters
    if (!context) {
        LOG_E("context is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (!sharedMem) {
        LOG_E("sharedMem is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (static_cast<int>(sharedMem->flags) & ~TEEC_MEM_INOUT) {
        LOG_E("sharedMem->flags is incorrect");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (sharedMem->flags == 0) {
        LOG_E("sharedMem->flags is incorrect");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    // Allocate shared memory
    if (!allocated_shared_mems.allocate(sharedMem)) {
        LOG_E("mmap failed");
        EXIT(TEEC_ERROR_OUT_OF_MEMORY);
    }
    auto res = common.TEEC_RegisterSharedMemory(context, sharedMem);
    if (res != TEEC_SUCCESS) {
        allocated_shared_mems.free(sharedMem);
    }
    EXIT(res);
}

void TEEC_ReleaseSharedMemory(
    TEEC_SharedMemory*      sharedMem) {
    ENTER();
    // Check parameters
    if (!sharedMem) {
        LOG_E("sharedMem is null");
        EXIT_NORETURN();
    }
    if (!sharedMem->buffer) {
        LOG_E("sharedMem->buffer is null");
        EXIT_NORETURN();
    }
    // Release shared memory
    common.TEEC_ReleaseSharedMemory(sharedMem);
    allocated_shared_mems.free(sharedMem);
    EXIT_NORETURN();
}

TEEC_Result TEEC_OpenSession(
    TEEC_Context*           context,
    TEEC_Session*           session,
    const TEEC_UUID*        destination,
    uint32_t                connectionMethod,
    const void*             connectionData,
    TEEC_Operation*         operation,
    uint32_t*               returnOrigin) {
    ENTER();
    // Set default origin
    if (returnOrigin) {
        *returnOrigin = TEEC_ORIGIN_API;
    }
    // Check parameters
    if (!context) {
        LOG_E("context is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if (!session) {
        LOG_E("session is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }
    if ((connectionMethod != TEEC_LOGIN_PUBLIC) &&
            (connectionMethod != TEEC_LOGIN_USER) &&
            (connectionMethod != TEEC_LOGIN_GROUP) &&
            (connectionMethod != TEEC_LOGIN_APPLICATION) &&
            (connectionMethod != TEEC_LOGIN_USER_APPLICATION) &&
            (connectionMethod != TEEC_LOGIN_GROUP_APPLICATION)) {
        LOG_E("connectionMethod not supported");
        return TEEC_ERROR_NOT_IMPLEMENTED;
    }
    if ((TEEC_LOGIN_GROUP == connectionMethod) ||
            (TEEC_LOGIN_GROUP_APPLICATION == connectionMethod)) {
        if (!connectionData) {
            LOG_E("connectionData is null");
            EXIT(TEEC_ERROR_BAD_PARAMETERS);
        }
    }

    if (operation) {
        if (!operations.start(operation)) {
            if (returnOrigin) {
                *returnOrigin = TEEC_ORIGIN_COMMS;
            }
            EXIT(TEEC_ERROR_CANCEL);
        }
    }
    TEEC_Result res = common.TEEC_OpenSession(context, session, destination,
                      connectionMethod, connectionData,
                      operation, returnOrigin);
    if (operation) {
        operations.end(operation);
    }
    EXIT(res);
}

void TEEC_CloseSession(
    TEEC_Session*           session) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session is null");
        EXIT_NORETURN();
    }

    common.TEEC_CloseSession(session);
    EXIT_NORETURN();
}

TEEC_Result TEEC_InvokeCommand(
    TEEC_Session*           session,
    uint32_t                commandID,
    TEEC_Operation*         operation,
    uint32_t*               returnOrigin) {
    ENTER();
    // Set default origin
    if (returnOrigin) {
        *returnOrigin = TEEC_ORIGIN_API;
    }
    // Check parameters
    if (!session) {
        LOG_E("session is null");
        EXIT(TEEC_ERROR_BAD_PARAMETERS);
    }

    if (operation) {
        if (!operations.start(operation)) {
            if (returnOrigin) {
                *returnOrigin = TEEC_ORIGIN_COMMS;
            }
            EXIT(TEEC_ERROR_CANCEL);
        }
    }
    TEEC_Result res = common.TEEC_InvokeCommand(session, commandID, operation,
                      returnOrigin);
    if (operation) {
        operations.end(operation);
    }
    EXIT(res);
}

/*
 * This function ensures that an operation is on the way before proceeding, so
 * the layers below can block waiting for the call to either TEEC_OpenSession or
 * TEEC_InvokeCommand.
 */
void TEEC_RequestCancellation(
    TEEC_Operation*         operation) {
    ENTER();
    if (!operation) {
        LOG_E("operation is null");
        EXIT_NORETURN();
    }
    operations.cancel(common, operation);
    EXIT_NORETURN();
}


// MobiCore

/*
 * On mcCloseDevice(), all mappings need to be removed, which means we need to
 * keep them somewhere.  We keep them in an array, resized when needed.
 * Both map and length must be kept in user-space, as after a fork another
 * mapping with same address may have appeared from the other process(es).
 */
class WSMs {
    struct Map {
        uint8_t* wsm;
        uint32_t len;
    };
    std::vector<Map> maps_;
    std::mutex mutex_;
public:
    void add(uint8_t* wsm, uint32_t len) {
        LOG_D("WSMs::add %p %d", wsm, len);
        std::lock_guard<std::mutex> lock(mutex_);
        maps_.push_back({ wsm, len });
    }
    void remove(uint8_t* wsm) {
        std::lock_guard<std::mutex> lock(mutex_);
        /* Find position of WSM */
        for (auto i = maps_.begin(); i != maps_.end(); i++) {
            if (i->wsm == wsm) {
                LOG_D("WSMs::remove %p %d", wsm, i->len);
                maps_.erase(i);
                break;
            }
        }
    }
    int len(uint8_t* wsm) {
        std::lock_guard<std::mutex> lock(mutex_);
        int len = -1;
        for (auto i = maps_.begin(); i != maps_.end(); i++) {
            if (i->wsm == wsm) {
                len = i->len;
                LOG_D("WSMs::len %p %d", wsm, len);
                break;
            }
        }
        return len;
    }
    void flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!maps_.empty()) {
            Map wsm = maps_.back();
            LOG_D("WSMs::flush %p %d", wsm.wsm, wsm.len);
            common.mcFreeWsm(MC_DEVICE_ID_DEFAULT, wsm.wsm, wsm.len);
            maps_.pop_back();
        }
    }
};

static WSMs wsms;

/* Only 1 device is supported */
static inline bool isValidDevice(uint32_t deviceId) {
    return (MC_DEVICE_ID_DEFAULT == deviceId);
}

mcResult_t mcOpenDevice(
    uint32_t                deviceId) {
    ENTER();
    std::lock_guard<std::mutex> lock(open_close_lock);
    // Check parameters
    if (!set_open_mode(deviceId) && !isValidDevice(deviceId)) {
        LOG_E("%s invalid device ID %x", __FUNCTION__, deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }

    mcResult_t mc_result = common.mcOpenDevice(deviceId);
    if (mc_result == MC_DRV_OK) {
        open_count++;
    }
    EXIT(mc_result);
}

mcResult_t mcCloseDevice(
    uint32_t                deviceId) {
    ENTER();
    std::lock_guard<std::mutex> lock(open_close_lock);
    // Check parameters
    if (!isValidDevice(deviceId)) {
        LOG_E("invalid device ID %x", deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }

    auto mc_result = common.mcCloseDevice(deviceId);
    if (mc_result == MC_DRV_OK) {
        if (!--open_count) {
            // Free all remaining WSMs
            wsms.flush();
        }
    }
    EXIT(mc_result);
}

mcResult_t mcOpenSession(
    mcSessionHandle_t*      session,
    const mcUuid_t*         uuid,
    uint8_t*                tci,
    uint32_t                tciLen) {
    ENTER();
    // Check parameters
    if (!session) {
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!uuid) {
        LOG_E("uuid is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    LOG_D("tci %p len %u", tci, tciLen);
    if ((tci && !tciLen) || (!tci && tciLen)) {
        LOG_E("TCI and its length are inconsistent");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    // So we return the correct error code
    if (tciLen > BUFFER_LENGTH_MAX) {
        LOG_E("TCI length too big: %u", tciLen);
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    EXIT(common.mcOpenSession(session, uuid, tci, tciLen));
}

mcResult_t mcOpenTrustlet(
    mcSessionHandle_t*      session,
    mcSpid_t                spid,
    uint8_t*                trustedapp,
    uint32_t                tLen,
    uint8_t*                tci,
    uint32_t                tciLen) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    LOG_D("tci %p len %u", tci, tciLen);
    if ((tci && !tciLen) || (!tci && tciLen)) {
        LOG_E("TCI and its length are inconsistent");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    // So we return the correct error code
    if (tciLen > BUFFER_LENGTH_MAX) {
        LOG_E("TCI length too big: %u", tciLen);
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    EXIT(common.mcOpenTrustlet(session, spid, trustedapp, tLen, tci, tciLen));
}

mcResult_t mcCloseSession(
    mcSessionHandle_t*      session) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }

    EXIT(common.mcCloseSession(session));
}

mcResult_t mcNotify(
    mcSessionHandle_t*      session) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }

    EXIT(common.mcNotify(session));
}

mcResult_t mcWaitNotification(
    mcSessionHandle_t*      session,
    int32_t                 timeout) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }

    int mc_result;
    do {
        mc_result = common.mcWaitNotification(session, timeout);
    } while ((mc_result == MC_DRV_ERR_INTERRUPTED_BY_SIGNAL) &&
             (timeout == MC_INFINITE_TIMEOUT));

    EXIT(mc_result);
}

mcResult_t mcMallocWsm(
    uint32_t                deviceId,
    uint32_t                /*align*/,
    uint32_t                len,
    uint8_t**               wsm,
    uint32_t                /*wsmFlags*/) {
    ENTER();
    // Check parameters
    if (!isValidDevice(deviceId)) {
        LOG_E("invalid device ID %x", deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!wsm) {
        LOG_E("wsm pointer is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    // Check length here to make sure we are consistent
    if ((len == 0) || (len > BUFFER_LENGTH_MAX)) {
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    auto mc_result = common.mcMallocWsm(deviceId, len, wsm);
    if (mc_result == MC_DRV_OK) {
        wsms.add(*wsm, len);
    }
    EXIT(mc_result);
}

mcResult_t mcFreeWsm(
    uint32_t                deviceId,
    uint8_t*                wsm) {
    ENTER();
    // Check parameters
    if (!isValidDevice(deviceId)) {
        LOG_E("invalid device ID %x", deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!wsm) {
        LOG_E("wsm pointer is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    int len = wsms.len(wsm);
    if (len == -1) {
        LOG_E("wsm %p buffer not available", wsm);
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    auto mc_result = common.mcFreeWsm(deviceId, wsm, len);
    if (mc_result == MC_DRV_OK) {
        wsms.remove(wsm);
    }
    EXIT(mc_result);
}

mcResult_t mcMap(
    mcSessionHandle_t*      session,
    void*                   buf,
    uint32_t                len,
    mcBulkMap_t*            mapInfo) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!mapInfo) {
        LOG_E("mapInfo pointer is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (len > BUFFER_LENGTH_MAX) {
        LOG_E("Incorrect len: %u", len);
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    EXIT(common.mcMap(session, buf, len, mapInfo));
}

mcResult_t mcUnmap(
    mcSessionHandle_t*      session,
    void*                   buf,
    mcBulkMap_t*            mapInfo) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!buf) {
        LOG_E("buf pointer is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!mapInfo) {
        LOG_E("mapInfo pointer is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!mapInfo->sVirtualAddr) {
        LOG_E("mapInfo SVA is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    EXIT(common.mcUnmap(session, buf, mapInfo));
}

mcResult_t mcGetSessionErrorCode(
    mcSessionHandle_t*      session,
    int32_t*                lastErr) {
    ENTER();
    // Check parameters
    if (!session) {
        LOG_E("session handle is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }
    if (!isValidDevice(session->deviceId)) {
        LOG_E("invalid device ID %x", session->deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!lastErr) {
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    EXIT(common.mcGetSessionErrorCode(session, lastErr));
}

mcResult_t mcGetMobiCoreVersion(
    uint32_t                deviceId,
    mcVersionInfo_t*        versionInfo) {
    ENTER();
    // Check parameters
    if (!isValidDevice(deviceId)) {
        LOG_E("invalid device ID %x", deviceId);
        EXIT(MC_DRV_ERR_UNKNOWN_DEVICE);
    }
    if (!versionInfo) {
        LOG_E("versionInfo pointer is null");
        EXIT(MC_DRV_ERR_INVALID_PARAMETER);
    }

    EXIT(common.mcGetMobiCoreVersion(deviceId, versionInfo));
}
