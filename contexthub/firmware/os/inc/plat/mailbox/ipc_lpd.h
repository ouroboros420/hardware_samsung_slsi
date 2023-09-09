/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
 *
 * Shinkyu Park <shinkyu.park@samsung.com>
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


#ifndef __IPC_LPD_H__
#define __IPC_LPD_H__
#include <rtc.h>

#define LPD_PERIOD_1S     (0x1)
#define LPD_PERIOD_1M     (0x2)

#define EVENT_LPD_START   (0x1 << 0)
#define EVENT_LPD_STOP    (0x1 << 1)
#define EVENT_LPD_UPDATE  (0x1 << 2)

int ipc_lpd_reset_release(void);
void ipc_lpd_start(uint32_t period);
void ipc_lpd_run(void);
bool ipc_lpd_isactive(void);
void ipc_lpd_stop(void);
void ipc_lpd_set_tid(void *tid);
void ipc_lpd_send_event(uint32_t event);
void ipc_lpd_check_update(void);
void ipc_lpd_init(void);
void ipc_lpd_powerdown(void);
void ipc_lpd_set_time(union time64_t time);
void ipc_lpd_restore(void);
void cm55_IRQHandler(void);
#endif
