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

#ifndef _MAILBOX_CHUB_AP_IPC_H
#define _MAILBOX_CHUB_AP_IPC_H

#define AP_IPC

#define IPC_VERSION (20200831)

#include "comms.h"

#define DEBUG_LEVEL (KERN_ERR)

enum DfsGovernor {
	DFS_GOVERNOR_OFF,
	DFS_GOVERNOR_SIMPLE,
	DFS_GOVERNOR_POWER,
	DFS_GVERNOR_MAX,
};

int ipc_logbuf_outprint(void *data, u32 loop);
bool ipc_logbuf_filled(void);
void ipc_logbuf_flush_on(bool on);
u32 ipc_get_chub_mem_size(void);
void ipc_set_chub_clk(u32 clk);
void ipc_reset_map(void);
u8 ipc_get_sensor_info(u8 type);
#endif
