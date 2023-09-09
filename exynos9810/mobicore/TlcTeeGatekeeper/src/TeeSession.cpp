/*
 * Copyright (c) 2015-2016 TRUSTONIC LIMITED
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

#include "buildTag.h"
#include <cstdlib>
#include <time.h>
#include <inttypes.h>
#include <unistd.h>

#include <sstream>
#include <fstream>
#include <utility>
#include <iomanip>
#include <memory>

#include <gatekeeper/password_handle.h>
#include "gatekeeper_version.h"
#include "tci.h"

#include "TeeSession.h"
#include "log.h"

/* Global definitions */
static const __attribute__((used)) char* buildtag = MOBICORE_COMPONENT_BUILD_TAG;
static const uint32_t DEVICE_ID = MC_DEVICE_ID_DEFAULT;
static const mcUuid_t uuid = TEE_GATEKEEPER_TL_UUID;

#define LOG(_msg_, _code_, _level_) \
    LOG_##_level_( "%s:%d %s error=0x%X\n", __func__, __LINE__, _msg_, _code_)

#define END_IF_ERR(expr, _code_)               \
    do {                                       \
        mcRet = (expr);                        \
        if (mcRet != MC_DRV_OK) {              \
            LOG(#expr " returned ", mcRet, D); \
            ret = _code_;                      \
            goto end;                          \
        }                                      \
    } while (false)


namespace {
    template<typename T> struct scoped_data_ptr_t
    {
        scoped_data_ptr_t()
        : size(0) {}

        scoped_data_ptr_t(const scoped_data_ptr_t<T>&) = delete;
        scoped_data_ptr_t<T>& operator=(const scoped_data_ptr_t<T>&) = delete;

        std::unique_ptr<T> buf;
        uint32_t           size;
    };

    typedef scoped_data_ptr_t<uint8_t[]> scoped_buf_ptr_t;

    bool copy_to_scoped_buf(const uint8_t *buf, uint32_t size, scoped_buf_ptr_t& data)
    {
        if(buf+size < buf)
            return false;

        if( (buf!=NULL) && (size!=0) ) {
            data.buf.reset(new (std::nothrow) uint8_t[size]);
            if(data.buf) {
                memcpy(data.buf.get(), buf, size);
                data.size = size;
                return true;
            }
            return false;
        }
        return true;
    }
}


TeeSession::TeeSession()
    : session_handle_({0,0})
    , is_device_opened_(false)
{
}

mcResult_t TeeSession::Open()
{
    if (is_device_opened_)
        return MC_DRV_ERR_INVALID_OPERATION;

    mcResult_t mcRet = mcOpenDevice(DEVICE_ID);

    if ( MC_DRV_OK ==  mcRet ) {
        session_handle_ = {DEVICE_ID, 0};
        mcRet = mcOpenSession(&session_handle_,
                              &uuid,
                              (uint8_t *) &tci_,
                              (uint32_t) sizeof(tciMessage_t));

        if (MC_DRV_OK == mcRet) {
            is_device_opened_ = true;
            return MC_DRV_OK;
        } else {
            LOG("mcOpenSession returned", mcRet, E);
            (void)mcCloseDevice(DEVICE_ID);
            return mcRet;
        }
    }

    LOG("mcOpenDevice returned", mcRet, E);
    return mcRet;
}

mcResult_t TeeSession::Close()
{
    if (!is_device_opened_)
        return MC_DRV_ERR_INVALID_OPERATION;

    mcResult_t mcRet = mcCloseSession(&session_handle_);
    if (MC_DRV_OK == mcRet) {

        // session is not usable now
        is_device_opened_ = false;
        mcRet = mcCloseDevice(DEVICE_ID);
        session_handle_ = {0,0};
        if (MC_DRV_OK == mcRet) {
            return MC_DRV_OK;
        } else {
            LOG("mcCloseDevice returned", mcRet, E);
            return mcRet;
        }
    }
    session_handle_ = {0,0};
    LOG("mcCloseSession returned", mcRet, E);
    return mcRet;
}

/**
 * Map a buffer.
 */
mcResult_t TeeSession::MapBuffer(void *buf, uint32_t buflen, mcBulkMap_t *bufinfo)
{
    if (NULL == buf) {
        return MC_DRV_ERR_INVALID_PARAMETER;
    }

    mcResult_t mcRet = mcMap(   &session_handle_,
                                buf,
                                buflen,
                                bufinfo);
    if (MC_DRV_OK != mcRet) {
        LOG("mcMap() returned", mcRet, E);
    }
    return mcRet;
}

/**
 * Unmap a buffer.
 */
void TeeSession::UnmapBuffer(void *buf, mcBulkMap_t *bufinfo)
{
    if (bufinfo->sVirtualAddr != 0) {
        mcResult_t mcRet = mcUnmap( &session_handle_,
                                    buf,
                                    bufinfo);
        if (mcRet != MC_DRV_OK) {
            LOG("mcUnmap() returned", mcRet, E);
        }
    }
}

/**
 * Notify the trusted application and wait for response.
 */

mcResult_t TeeSession::SecureCall()
{
    if (!is_device_opened_)
        return MC_DRV_ERR_UNKNOWN_SESSION;

    mcResult_t mcRet = mcNotify(&session_handle_);
    if (mcRet != MC_DRV_OK) {
        LOG("mcNotify() returned", mcRet, E);
        return mcRet;
    }

    mcRet = mcWaitNotification( &session_handle_, MC_INFINITE_TIMEOUT);
    if (mcRet != MC_DRV_OK) {
        LOG("mcWaitNotification() returned", mcRet, E);
        if (mcRet == MC_DRV_INFO_NOTIFICATION) {
            int32_t ta_termination_code=0;
            mcGetSessionErrorCode(&session_handle_, &ta_termination_code);
            LOG_E("TA termination code: %d", ta_termination_code);
        }
    }
    return mcRet;
}

gatekeeper_error_t TeeSession::CheckVersion()
{
    gatekeeper_error_t ret = ERROR_NONE;
    mcResult_t mcRet = MC_DRV_OK;
    memset(&tci_, 0, sizeof(tci_));
    /* First thing, check version between NWd/SWd */
    tci_.command.header.commandId = CMD_ID_GET_VERSION;

    END_IF_ERR(SecureCall(), ERROR_UNKNOWN);

end:
    return ret;
}

gatekeeper_error_t TeeSession::Enroll(
    uint32_t uid,
    const uint8_t *current_password_handle, uint32_t current_password_handle_length,
    const uint8_t *current_password, uint32_t current_password_length,
    const uint8_t *desired_password, uint32_t desired_password_length,
    uint8_t **enrolled_password_handle, uint32_t *enrolled_password_handle_length,
    int& throttle_ms)
{
    gatekeeper_error_t ret = ERROR_UNKNOWN;
    mcResult_t mcRet = MC_DRV_OK;

    mcBulkMap_t current_password_mm = {0 /*sVirtualAddr*/, 0 /*sVirtualLen*/};
    mcBulkMap_t desired_password_mm = {0 /*sVirtualAddr*/, 0 /*sVirtualLen*/};
    mcBulkMap_t current_password_handle_mm = {0 /*sVirtualAddr*/,0 /*sVirtualLen*/};
    bool is_untrusted_enroll = false;
    memset(&tci_, 0, sizeof(tci_));

    if ((NULL == desired_password) ||
        (0    == desired_password_length) ||
        (NULL == enrolled_password_handle) ||
        (0    == enrolled_password_handle_length))
    {
        return ERROR_INVALID;
    }

    // Both current password and current_password_handle must be either none
    // or be assigned.
    is_untrusted_enroll = (    (NULL == current_password)
                            && (0 == current_password_length)
                            && (NULL == current_password_handle)
                            && (0 == current_password_handle_length) );

    if ( !is_untrusted_enroll )
    {
        if (   (NULL == current_password)
            || (0    == current_password_length)
            || (NULL == current_password_handle)
            || (0    == current_password_handle_length))
        {
            // In case of trusted enroll all provided values must be assigned
            return ERROR_INVALID;
        }
    }

    // We copy received parametrs, so that const_cast when mapping is not required
    scoped_buf_ptr_t desired_password_buf;
    scoped_buf_ptr_t current_password_buf;
    scoped_buf_ptr_t current_password_handle_buf;
    if(!copy_to_scoped_buf(desired_password, desired_password_length, desired_password_buf))
        return ERROR_UNKNOWN;

    END_IF_ERR(
        MapBuffer(desired_password_buf.buf.get(),
                  desired_password_buf.size,
                  &desired_password_mm), ERROR_UNKNOWN);
    if ( !is_untrusted_enroll )
    {
        if(!copy_to_scoped_buf(current_password, current_password_length, current_password_buf))
        {
            return ERROR_UNKNOWN;
        }
        END_IF_ERR(
            MapBuffer(current_password_buf.buf.get(),
                      current_password_buf.size,
                      &current_password_mm), ERROR_UNKNOWN);

        if(!copy_to_scoped_buf(current_password_handle, current_password_handle_length,
                               current_password_handle_buf))
        {
            return ERROR_UNKNOWN;
        }
        END_IF_ERR(
            MapBuffer(current_password_handle_buf.buf.get(),
                      current_password_handle_buf.size,
                      &current_password_handle_mm), ERROR_UNKNOWN);

        tci_.enroll_request.current_password.data = (uint32_t) current_password_mm.sVirtualAddr;
        tci_.enroll_request.current_password.data_length = current_password_mm.sVirtualLen;
        tci_.enroll_request.current_password_handle.data = (uint32_t) current_password_handle_mm.sVirtualAddr;
        tci_.enroll_request.current_password_handle.data_length = current_password_handle_mm.sVirtualLen;
    }

    tci_.enroll_request.desired_password.data = (uint32_t) desired_password_mm.sVirtualAddr;
    tci_.enroll_request.desired_password.data_length = desired_password_mm.sVirtualLen;
    tci_.enroll_request.uid = uid;
    tci_.command.header.commandId = CMD_ID_ENROLL;

    END_IF_ERR( SecureCall(), ERROR_UNKNOWN );

    // Copy data from SWd
    throttle_ms = tci_.enroll_response.throttle_ms;
    ret = tci_.response.header.returnCode;
    if (ERROR_NONE == ret)
    {
        *enrolled_password_handle = new uint8_t[sizeof(tci_.enroll_response.password_handle)];
        *enrolled_password_handle_length = sizeof(tci_.enroll_response.password_handle);
        memcpy(*enrolled_password_handle,
               reinterpret_cast<uint8_t*>(&tci_.enroll_response.password_handle),
               sizeof(tci_.enroll_response.password_handle));
    }

end:
    UnmapBuffer(desired_password_buf.buf.get(), &desired_password_mm);
    if (!is_untrusted_enroll)
    {
        // Can I unmapp not mapped buffer?
        UnmapBuffer(current_password_buf.buf.get(), &current_password_mm);
        UnmapBuffer(current_password_handle_buf.buf.get(), &current_password_handle_mm);
    }
    return ret;
}

gatekeeper_error_t TeeSession::Verify(
        uint32_t uid, uint64_t challenge,
        const uint8_t *enrolled_password_handle, uint32_t enrolled_password_handle_length,
        const uint8_t *provided_password, uint32_t provided_password_length,
        uint8_t **auth_token, uint32_t *auth_token_length, bool &request_reenroll,
        int& throttle_ms)
{
    gatekeeper_error_t ret = ERROR_UNKNOWN;
    mcResult_t mcRet = MC_DRV_OK;
    mcBulkMap_t enrolled_password_handle_mm = {0 /*sVirtualAddr*/, 0 /*sVirtualLen*/};
    mcBulkMap_t provided_password_mm        = {0 /*sVirtualAddr*/, 0 /*sVirtualLen*/};
    memset(&tci_,0, sizeof(tci_));

    // Validate mandatory parameters
    if (( NULL == enrolled_password_handle ) ||
        ( 0    == enrolled_password_handle_length ) ||
        ( NULL == provided_password ) ||
        ( 0    == provided_password_length ) ||
        ( NULL == auth_token) ||
        ( NULL == auth_token_length ))
    {
        return ERROR_INVALID;
    }

    scoped_buf_ptr_t enrolled_password_handle_buf;
    scoped_buf_ptr_t provided_password_buf;

    if(!copy_to_scoped_buf(enrolled_password_handle, enrolled_password_handle_length, enrolled_password_handle_buf))
        return ERROR_UNKNOWN;
    if(!copy_to_scoped_buf(provided_password, provided_password_length, provided_password_buf))
        return ERROR_UNKNOWN;

    END_IF_ERR(
        MapBuffer(enrolled_password_handle_buf.buf.get(),
                  enrolled_password_handle_buf.size,
                  &enrolled_password_handle_mm), ERROR_UNKNOWN);

    END_IF_ERR(
        MapBuffer(provided_password_buf.buf.get(),
                  provided_password_buf.size,
                  &provided_password_mm), ERROR_UNKNOWN);

    tci_.command.header.commandId = CMD_ID_VERIFY;
    tci_.verify_request.uid = uid;
    tci_.verify_request.challenge = challenge;
    tci_.verify_request.enrolled_password_handle.data = (uint32_t) enrolled_password_handle_mm.sVirtualAddr;
    tci_.verify_request.enrolled_password_handle.data_length = enrolled_password_handle_mm.sVirtualLen;
    tci_.verify_request.provided_password.data = (uint32_t) provided_password_mm.sVirtualAddr;
    tci_.verify_request.provided_password.data_length = provided_password_mm.sVirtualLen;

    END_IF_ERR( SecureCall(), ERROR_UNKNOWN );

    // handle response from SWd
    throttle_ms = tci_.verify_response.throttle_ms;
    request_reenroll = tci_.verify_response.request_reenroll;
    ret = tci_.response.header.returnCode;

    if (ERROR_NONE == ret)
    {
        *auth_token = new uint8_t[sizeof(tci_.verify_response.auth_token)];
        *auth_token_length = sizeof(tci_.verify_response.auth_token);
        memcpy(*auth_token,
               reinterpret_cast<uint8_t*>(&tci_.verify_response.auth_token),
               *auth_token_length);
    }

end:
    UnmapBuffer(enrolled_password_handle_buf.buf.get(), &enrolled_password_handle_mm);
    UnmapBuffer(provided_password_buf.buf.get(), &provided_password_mm);
    return ret;
}

gatekeeper_error_t TeeSession::DeleteUser(user_id_t uid, bool delete_all)
{
    memset(&tci_,0, sizeof(tci_));
    tci_.command.header.commandId = CMD_ID_DELETE_USER;
    tci_.response.header.returnCode = ERROR_UNKNOWN;
    tci_.delete_user_request.uid = uid;
    tci_.delete_user_request.delete_all = delete_all;
    if ( MC_DRV_OK != SecureCall() ) {
        return ERROR_UNKNOWN;
    }
    return tci_.response.header.returnCode;
}
