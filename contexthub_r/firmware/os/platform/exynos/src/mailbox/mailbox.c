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
#include <mailboxApmDrv.h>
#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>

void mailboxEnable()
{
    mailboxDrvEnable();
    mailboxOsEnable();
#if defined(MBAPM_REQUIRED)
    mailboxApmDrvEnable();
#endif
#if defined(MBGNSS_REQUIRED)
    mailboxGnssDrvEnable();
#endif
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
        mailboxApmDrvWriteEvent(event);
#endif
}

#if defined(MBAPM_REQUIRED)
int32_t mailboxGetPowerMode(void)
{
    return mailboxApmDrvGetSleepReady();
}

void mailboxSetPowerMode(bool bEn)
{
    if (bEn)
        mailboxApmDrvWriteEvent(MAILBOX_EVT_SLEEP);
}
#endif

