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
#include "ipc_common.h"
#include "ipc_chub.h"
#include "ipc_other.h"
#include "mailboxOS.h"
#include <cmsis.h>
#include <heap.h>

#ifdef IPC_DEF_IPC_TEST
int enableLoopbackTest;
#endif
void mailboxABOXHandleIRQ(int evt, void *priv)
{
	struct cipc_info *cipc = cipc_get_info();
	unsigned int len;
	char *ipc_buf;
	char *local_buf;

#ifdef IPC_DEF_IPC_TEST
	if (evt == CIPC_LOOPBACK_EVT) {
		CSP_PRINTF_INFO("%s: enter: evt:%x\n", __func__, evt);
		if (enableLoopbackTest == 0)
			enableLoopbackTest = 1;
		else
			enableLoopbackTest = 0;
		CSP_PRINTF_INFO("%s: enableLoopbackTest:%d\n", __func__, enableLoopbackTest);
		return;
	}
	if (enableLoopbackTest) {
		CSP_PRINTF_INFO("%s: evt:0x%x\n", __func__, evt);
		cipc_loopback_test(evt, 0);
		return;
	}
#endif

	if (cipc->cb) {
		if (evt == CIPC_REG_DATA_ABOX2CHUB || evt == CIPC_REG_DATA_ABOX2CHUB_AUD || evt == CIPC_REG_DATA_CHUB2ABOX) {
			ipc_buf = cipc_read_data(evt, &len);
			local_buf = heapAlloc(len);
			if (!local_buf) {
				CSP_PRINTF_INFO("%s: fails to allocate local_buf\n", __func__);
				return;
			}
			memcpy(local_buf, ipc_buf, len);
			cipc->cb(evt, local_buf, len);
			heapFree(local_buf);
		} else {
			cipc->cb(evt, NULL, 0);
		}
		return;
	}

	switch (evt) {
	case CIPC_REG_DATA_ABOX2CHUB:
		/* handle Gyro data */
		break;
	case CIPC_REG_DATA_ABOX2CHUB_AUD:
		/* handle Gyro data */
		break;
	case CIPC_REG_DATA_CHUB2ABOX:
		/* handle Gyro data */
		break;
	default:
		break;
	}
	return;
}

void mailboxABOX_IRQHandler(void)
{
#ifdef DEBUG_VER
	CSP_PRINF_INFO("+++%s: chub2abox!!!: sr:%x", __func__,
		ipc_hw_read_int_status_reg_all((void *)MAILBOX_ABOX_BASE_ADDRESS, IPC_SRC_MB0));
#endif
	cipc_irqhandler(CIPC_USER_ABOX2CHUB, 0);
}

static void chubSleep(unsigned int ms)
{
	msleep(ms);
}

static void chubMemcpy(void *dst, void *src, int size, int dst_io, int src_io)
{
	memcpy(dst, src, size);
}

static void chubMemset(void *buf, int val, int size, int io)
{
	memset(buf, val, size);
}

static void chubGetLock(unsigned long *flag)
{
	__disable_irq();
}

static void chubPutLock(unsigned long *flag)
{
	__enable_irq();
}

static int chubStrncmp(char *dst, char *src, int size)
{
	return strncmp(dst, src, size);
}

static void chubStrncpy(char *dst, char *src, int size, int dst_io, int src_io)
{
	strncpy(dst, (const char *)src, (size_t) size);
}

static void chubLogOut(const char *str, ...)
{
	va_list vl;

	va_start(vl, str);
	osLogv(LOG_INFO, 0, str, vl);
	va_end(vl);
}

struct cipc_funcs cipc_func = {
	.mbusywait = chubSleep,
	.memcpy = chubMemcpy,
	.memset = chubMemset,
	.getlock_evt = chubGetLock,
	.putlock_evt = chubPutLock,
	.getlock_data = chubGetLock,
	.putlock_data = chubPutLock,
	.strncmp = chubStrncmp,
	.strncpy = chubStrncpy,
	.print = chubLogOut,
	.priv = NULL,
};

#ifdef IPC_DEF_IPC_TEST_CB
static void client_callback(int evt, char *buf, unsigned int len)
{
	CSP_PRINTF_INFO("%s: evt:%d is called, len: %d, buf:%s\n",
		__func__, evt, len, buf ? buf : NULL);
}
#endif

struct cipc_info *chub_cipc_init(enum ipc_owner owner, void *sram_base)
{
	u32 cipc_start_offset;
	int cipc_size;
	struct cipc_info *cipc;
	int ret;

	cipc = cipc_init(owner, sram_base, &cipc_func);
	if (!cipc) {
		CSP_PRINTF_ERROR("%s: cipc_init fails\n", __func__);
		return NULL;
	}

	ret =
	    cipc_register((void *)MAILBOX_AP_BASE_ADDRESS, CIPC_USER_CHUB2AP, CIPC_USER_AP2CHUB,
			  mailboxAPHandleIRQ, NULL, &cipc_start_offset, &cipc_size);
	if (ret) {
		CSP_PRINTF_ERROR("%s: chub2ap cipc_request fails\n", __func__);
		return NULL;
	}
	CSP_PRINTF_INFO("%s: chub2ap register: offset:+%x, size:%d\n", __func__,
			cipc_start_offset, cipc_size);

#ifdef CIPC_DEF_USER_ABOX
	ret = cipc_register((void *)MAILBOX_ABOX_BASE_ADDRESS, CIPC_USER_CHUB2ABOX, CIPC_USER_ABOX2CHUB,
			  mailboxABOXHandleIRQ, NULL, &cipc_start_offset, &cipc_size);
	if (ret) {
		CSP_PRINTF_ERROR("%s: chub2abox register fails: offset:+%x, size:%d\n", __func__,
			cipc_start_offset, cipc_size);
		return NULL;
	}

	CSP_PRINTF_INFO("%s: chub2abox register: offset:+%x, size:%d\n", __func__,
			cipc_start_offset, cipc_size);
#ifdef IPC_DEF_IPC_TEST_CB
	cipc_register_callback(client_callback);
#endif
#endif
	return cipc;
}
