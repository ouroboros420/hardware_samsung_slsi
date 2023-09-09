/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_os.c
 *      Purpose: To implement os api
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
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

#include <csp_common.h>
#if defined(SEOS)
  #include <heap.h>
  #include <timer.h>
#elif defined(EMBOS)
  #include <RTOS.h>
#endif
#include <mailbox.h>
#include <csp_os.h>
#include <cmu.h>

void *cspHeapAlloc(uint32_t sz)
{
#ifdef SEOS
    return heapAlloc(sz);
#else
    return OS_malloc(sz);
#endif
}

void *cspHeapRealloc(void *ptr, uint32_t newSz)
{
#ifdef SEOS
    // Not supported.
    // Caller should allocate new sized memory chunk, move contents to new memory chunk, and then free old memory
    return NULL;
#else
    return OS_realloc(ptr, newSz);
#endif
}

void cspHeapFree(void *ptr)
{
#ifdef SEOS
    heapFree(ptr);
#else
    OS_free(ptr);
#endif
}

void uSleep(uint32_t us)
{
    uint32_t unit = cmuGetSpeed(CMU_CLK_OUTPUT_CPU) / 5000000;
    volatile uint32_t time = us * unit;
    while(time--);
}

void mSleep(uint32_t ms)
{
    volatile uint32_t time = ms * (cmuGetSpeed(CMU_CLK_OUTPUT_CPU) / 5000) /*unit */ ;
    while (time--) ;
}

static void cspMakePanic(void)
{
    int *hanger = NULL;

    CSP_PRINTF_INFO("%s: killing chub...\n", __func__);
    *hanger = -1;
    CSP_PRINTF_INFO("%s: not killed chub...\n", __func__);
}

void cspNotify(enum error_type err)
{
    switch (err) {
    case ERR_ASSERT:
        mailboxDrvWriteEvent(MAILBOX_EVT_DEBUG, IPC_DEBUG_CHUB_ASSERT);
        break;
    case ERR_FAULT:
        mailboxDrvWriteEvent(MAILBOX_EVT_DEBUG, IPC_DEBUG_CHUB_FAULT);
        break;
    case ERR_ERROR:
        mailboxDrvWriteEvent(MAILBOX_EVT_DEBUG, IPC_DEBUG_CHUB_ERROR);
        break;
    case ERR_REBOOT:
        mailboxDrvWriteEvent(MAILBOX_EVT_REBOOT, 0);
        break;
    case ERR_PANIC:
        cspMakePanic();
        break;
    default:
        break;
    };
}
