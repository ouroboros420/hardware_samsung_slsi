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

#include "ipc_chub.h"

#if defined(SEOS)
  #include <seos.h>
  #include <errno.h>
  #include <cmsis.h>
  #include <os/inc/trylock.h>
#elif defined(EMBOS)
  #include <Device.h>
  #include <RTOS.h>
  #define EINVAL 22
#endif
#include <mailboxDrv.h>
#include <csp_common.h>
#include <csp_printf.h>
#include <string.h>
#include <mailboxOS.h>
#include <cmu.h>

#define NAME_PREFIX "ipc"
#define SENSORMAP_MAGIC	"SensorMap"
#define MAX_ACTIVE_SENSOR_NUM (10)

#if defined(SEOS)
static TRYLOCK_DECL_STATIC(ipcLockLog) = TRYLOCK_INIT_STATIC();
#endif

u32 ipc_get_chub_clk(void)
{
	struct chub_bootargs *map = ipc_get_base(IPC_REG_BL_MAP);

	return map->chubclk;
}

void ipc_set_sensor_info(u8 type, const char *name, const char *vendor,
			 u8 senstype, u8 id)
{
	struct sensor_map *ipc_sensor_map =
	    ipc_get_base(IPC_REG_IPC_SENSORINFO);

	if (ipc_have_sensor_info(ipc_sensor_map)) {
		if (ipc_sensor_map->index >= MAX_PHYSENSOR_NUM) {
			CSP_PRINTF_ERROR("%s: invalid index:%d\n", __func__,
					 ipc_sensor_map->index);
			return;
		}
		if (name) {
			ipc_sensor_map->sinfo[ipc_sensor_map->index].sensortype = type;
			strncpy(ipc_sensor_map->sinfo[ipc_sensor_map->index].name, name,
				MAX_SENSOR_NAME);
			strncpy(ipc_sensor_map->sinfo[ipc_sensor_map->index].vendorname, vendor,
				MAX_SENSOR_VENDOR_NAME);
		}
	  #if defined(SEOS)
		if (senstype) {
			int i;

			for (i = 0; i < MAX_SENSOR_NUM; i++)
				if (ipc_sensor_map->sinfo[i].sensortype == type) {
					ipc_sensor_map->sinfo[i].chipid = id;
					ipc_sensor_map->sinfo[i].senstype =
					    senstype;
					return;
				}
		}
	  #endif
		CSP_PRINTF_ERROR("sensortype: %d: u:%d, t:%d, n:%s, v:%s\n",
				 ipc_sensor_map->index, type,
				 ipc_sensor_map->sinfo[ipc_sensor_map->index].sensortype,
				 ipc_sensor_map->sinfo[ipc_sensor_map->index].name,
				 ipc_sensor_map->sinfo[ipc_sensor_map->index].vendorname);
		ipc_sensor_map->index++;
	}
}

void *ipc_logbuf_inbase(bool force)
{
	struct ipc_info *ipc = ipc_get_info();

	if (ipc->ipc_map) {
		struct ipc_logbuf *logbuf = &ipc->ipc_map->logbuf;

		if (logbuf->eq >= LOGBUF_NUM || logbuf->dq >= LOGBUF_NUM)
			return NULL;

		if (force || logbuf->loglevel) {
			struct logbuf_content *log;
			int index;

			/* check the validataion of ipc index */
			if (logbuf->eq >= LOGBUF_NUM || logbuf->dq >= LOGBUF_NUM)
				return NULL;
		  #if defined(SEOS)
			if (!trylockTryTake(&ipcLockLog))
				return NULL;
		  #elif defined(EMBOS)
                        OS_EnterRegion();
		  #endif

			if (logbuf->full)	/* logbuf is full overwirte */
				logbuf->dbg_full_cnt++;

			index = logbuf->eq;
			logbuf->eq = (logbuf->eq + 1) % LOGBUF_NUM;
			if (logbuf->eq == logbuf->dq)
				logbuf->full = 1;
		  #if defined(SEOS)
			trylockRelease(&ipcLockLog);
		  #elif defined(EMBOS)
                        OS_LeaveRegion();
		  #endif

			log = &logbuf->log[index];
			memset(log, 0, sizeof(struct logbuf_content));
			return log;
		}
	}
	return NULL;
}

struct logbuf_content *ipc_logbuf_get_curlogbuf(struct logbuf_content *log)
{
	int i;

	for (i = 0; i < log->newline; i++) {
		if (log->nextaddr)
			log = (struct logbuf_content *)log->nextaddr;
		else
			break;
	}

	return log;
}

void ipc_logbuf_set_req_num(struct logbuf_content *log)
{
	struct ipc_info *ipc = ipc_get_info();
	struct ipc_logbuf *logbuf = &ipc->ipc_map->logbuf;

	log->size = logbuf->fw_num++;
}

void ipc_logbuf_req_flush(struct logbuf_content *log)
{
	if (log) {
		struct ipc_info *ipc = ipc_get_info();
		struct ipc_logbuf *logbuf = &ipc->ipc_map->logbuf;

		if (logbuf->eq >= LOGBUF_NUM || logbuf->dq >= LOGBUF_NUM)
			return;

		/* debug check overwrite */
		if (log->nextaddr && log->newline) {
			struct logbuf_content *nextlog =
			    ipc_logbuf_get_curlogbuf(log);

			nextlog->size = logbuf->fw_num++;
		} else {
			log->size = logbuf->fw_num++;
		}

		if (ipc->ipc_map) {
			if (!logbuf->flush_req && !logbuf->flush_active) {
#ifdef USE_LOG_FLUSH_TRSHOLD
				u32 eq = logbuf->eq;
				u32 dq = logbuf->dq;
				u32 logcnt =
				    (eq >= dq) ? (eq - dq) : (eq + (logbuf->size - dq));

				if (((ipc_get_ap_wake() == AP_WAKE) &&
				    (logcnt > LOGBUF_FLUSH_THRESHOLD)) || log->error) {
					if (!logbuf->flush_req) {
						logbuf->flush_req = 1;
						ipc_hw_gen_interrupt(ipc->mb_base,
								     ipc->opp_mb_id,
								     IRQ_NUM_CHUB_LOG);
					}
				}
#else
				if ((ipc_get_ap_wake() == AP_WAKE) || log->error) {
					if (!logbuf->flush_req) {
						logbuf->flush_req = 1;
						logbuf->reqcnt++;
						ipc_hw_gen_interrupt(ipc->mb_base,
								     ipc->opp_mb_id,
								     IRQ_NUM_CHUB_LOG);
					}
				}
#endif
			}
		}
	}
}

/* LOCAL_POWERGATE */
u32 *ipc_get_chub_psp(void)
{
	struct chub_bootargs *map = ipc_get_base(IPC_REG_BL_MAP);

	return &map->psp;
}

u32 *ipc_get_chub_msp(void)
{
	struct chub_bootargs *map = ipc_get_base(IPC_REG_BL_MAP);

	return &map->msp;
}
