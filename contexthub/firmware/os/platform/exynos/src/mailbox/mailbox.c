/*----------------------------------------------------------------------------
 *      Exynos SoC  -  Mailbox
 *----------------------------------------------------------------------------
 *      Name:    mailbox.c
 *      Purpose: To implement Mailbox APIs
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

#include <mailbox.h>
#include <mailboxOS.h>
#include <mailboxDrv.h>
#include <mailboxApm.h>
#include <mailboxGnssDrv.h>
#if defined(MBCP_REQUIRED)
  #include <mailboxCp.h>
#endif
#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

#ifdef LOCAL_POWERGATE
#include <sysreg.h>
#endif

void mailboxEnable(void)
{
    mailboxDrvEnable();
    mailboxOsEnable();
#if defined(MBAPM_REQUIRED)
    mailboxApmEnable();
#endif
#if defined(MBGNSS_REQUIRED)
    mailboxGnssDrvEnable();
#endif
#if defined(MBCP_REQUIRED)
    mailboxCpInit();
#endif
}

int32_t mailboxWrite(const void *buf, uint32_t size)
{
    if (buf == NULL) {
        CSP_PRINTF_ERROR("mailboxWrite() - buf is NULL\n");
        CSP_ASSERT(0);
    }

    if (size == 0) {
        CSP_PRINTF_ERROR("mailboxWrite() - size is 0\n");
        CSP_ASSERT(0);
    }

    return mailboxDrvWrite(buf, size);
}

int32_t mailboxRead(void *buf, uint32_t size, mailboxRxCB callback,
                    void *cookie)
{
    if (buf == NULL) {
        CSP_PRINTF_ERROR("mailboxRead() - buf is NULL\n");
        CSP_ASSERT(0);
    }

    if (size == 0) {
        CSP_PRINTF_ERROR("mailboxRead() - size is 0\n");
        CSP_ASSERT(0);
    }

    if (callback == NULL) {
        CSP_PRINTF_ERROR("mailboxRead() - callback is NULL\n");
        CSP_ASSERT(0);
    }

    return mailboxDrvRead(buf, size, callback, cookie);
}

void mailboxWriteEvent(enum mailbox_event event, uint32_t val)
{
    if (!(event >= 0 && event < MAILBOX_EVT_MAX)) {
        CSP_PRINTF_ERROR("mailboxWriteEvent() - event(%d) is invalid\n", event);
        CSP_ASSERT(0);
    }

    if (event & MAILBOX_EVT_AP_OWN)
        mailboxDrvWriteEvent(event, val);
#if defined(MBAPM_REQUIRED)
    else if (event & MAILBOX_EVT_APM_OWN)
  #if defined(s5e9110)
    {
        if (sysregDrvGetAPMUpStatus) {
            mailboxApmWriteEvent(event);
        } else {
            CSP_ERROR(sysregEnableAPM());
            mailboxApmWriteEvent(event);
        }
    }
  #else
        mailboxApmWriteEvent(event);
  #endif
#endif
}

#if defined(MBAPM_REQUIRED)
int32_t mailboxGetPowerMode(void)
{
    return mailboxApmGetSleepReady();
}

void mailboxSetPowerMode(bool bEn)
{
#if defined(SUPPORT_RCO_DIVIDING)
    if(bEn) {
        mailboxApmWriteEvent(APM_EVT_RCO_DIVIDING);
        mailboxApmWaitAck(APM_EVT_RCO_DIVIDING);
    }
#else
    if (bEn)
        mailboxApmWriteEvent(MAILBOX_EVT_SLEEP);
#endif
}
#endif
