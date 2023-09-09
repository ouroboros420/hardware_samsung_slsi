/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Boojin Kim <boojin.kim@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <seos.h>
#include <hostIntf.h>
#include <hostIntf_priv.h>
#include <nanohubPacket.h>
#include <mailbox.h>

static int hostIntfMailboxRequestRelease()
{
    return 0;
}

static void hostIntfMailboxRxCallback(void *cookie, void *rxbuf, int err)
{
    struct NanohubPacket *packet = rxbuf;
    HostIntfCommCallbackF callback = cookie;

    callback(NANOHUB_PACKET_SIZE(packet->len), err);
}

static int hostIntfMailboxRxPacket(void *rxBuf, size_t rxSize,
                                   HostIntfCommCallbackF callback)
{
    int err =
        mailboxDrvRead(rxBuf, rxSize, hostIntfMailboxRxCallback, callback);

    if (err)
        callback(NANOHUB_PACKET_SIZE(rxSize), err);
    return 0;
}

static int hostIntfMailboxTxPacket(const void *txBuf, size_t txSize,
                                   HostIntfCommCallbackF callback)
{
    int ret;

    ((uint8_t *) txBuf)[txSize] = NANOHUB_PREAMBLE_BYTE;

    ret = mailboxDrvWrite(txBuf, txSize + 1);

    callback(txSize, ret);

    return 0;
}

static const struct HostIntfComm gMailboxComm = {
    .request = hostIntfMailboxRequestRelease,
    .rxPacket = hostIntfMailboxRxPacket,
    .txPacket = hostIntfMailboxTxPacket,
    .release = hostIntfMailboxRequestRelease,
};

const struct HostIntfComm *hostIntfMailboxInit()
{
    return &gMailboxComm;
}
