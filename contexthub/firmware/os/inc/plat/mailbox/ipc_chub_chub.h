/*
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
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

#ifndef _MAILBOX_CHUB_CHUB_IPC_H
#define _MAILBOX_CHUB_CHUB_IPC_H

#define CHUB_IPC

#include <plat/dfs/dfs.h>
#if defined(SEOS)
  #include <nanohubPacket.h>
  #include <csp_common.h>
#elif defined(EMBOS)
  #include <Device.h>
  #include <typedef.h>
  #include <string.h>
  #define SUPPORT_LOOPBACKTEST
#endif

#define DEBUG_LEVEL (LOG_ERROR)

/* ipc size and count */
#ifndef CIPC_DATA_AP2CHUB_SIZE
  #define CIPC_DATA_AP2CHUB_SIZE (128)
#endif
#define CIPC_DATA_AP2CHUB_CNT (8)

#define CIPC_DATA_CHUB2AP_SIZE (PACKET_SIZE_MAX)
#define CIPC_DATA_CHUB2AP_CNT (16)

#define CIPC_DATA_CHUB2AP_BATCH_SIZE (128)
#define CIPC_DATA_CHUB2AP_BATCH_CNT (1)

#define CIPC_DATA_CHUB2ABOX_SIZE (128)
#define CIPC_DATA_CHUB2ABOX_CNT (8)

#define CIPC_DATA_ABOX2CHUB_SIZE (96 * 1024)
#define CIPC_DATA_ABOX2CHUB_CNT (1)

#define CIPC_DATA_ABOX2CHUB_PROX_SIZE (128)
#define CIPC_DATA_ABOX2CHUB_PROX_CNT (8)

#ifndef HOSTINTF_SENSOR_DATA_MAX
#define HOSTINTF_SENSOR_DATA_MAX    240
#endif

struct logbuf_content *ipc_logbuf_get_curlogbuf(struct logbuf_content *log);
void ipc_logbuf_set_req_num(struct logbuf_content *log);
void *ipc_logbuf_inbase(bool force);
void ipc_logbuf_req_flush(struct logbuf_content *log);
void ipc_set_sensor_info(u8 type, const char *name, const char *vendor,
			 u8 senstype, u8 id);
u32 ipc_get_chub_clk(void);
struct sampleTimeTable *ipc_get_dfs_sampleTime(void);
struct cipc_info *chub_cipc_init(enum ipc_owner owner, void *sram_bas, struct cipc_funcs *func);
void mailboxABOXHandleIRQ(int evt, void *priv);

/* LOCAL_POWERGATE */
u32 *ipc_get_chub_psp(void);
u32 *ipc_get_chub_msp(void);
#endif
