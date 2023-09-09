/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include <apInt.h>
#include <gpio.h>
#include <variant/variant.h>
#include <seos.h>
#include <platform.h>
#include <plat/plat.h>
#include <mailbox.h>

#ifdef EXYNOS_CONTEXTHUB_EXT
#include <plat/gpio/gpio.h>

static struct Gpio *apIntWkup;
#ifdef AP_INT_NONWAKEUP
static struct Gpio *apIntNonWkup;
#endif

#define apIntSleepWakeup (0xbb00)

void apIntInit()
{
    apIntWkup = gpioRequest(AP_INT_WAKEUP);
    gpioConfigOutput(apIntWkup, GPIO_SPEED_LOW, GPIO_PULL_NONE, GPIO_OUT_PUSH_PULL, 1);
#ifdef AP_INT_NONWAKEUP
    apIntNonWkup = gpioRequest(AP_INT_NONWAKEUP);
    gpioConfigOutput(apIntNonWkup, GPIO_SPEED_LOW, GPIO_PULL_NONE, GPIO_OUT_PUSH_PULL, 1);
#endif
}
void apIntSet(bool wakeup)
{
    if (wakeup) {
		platRequestDevInSleepMode(apIntSleepWakeup, 12);
        gpioSet(apIntWkup, 0);
    }
#ifdef AP_INT_NONWAKEUP
    else
        gpioSet(apIntNonWkup, 0);
#endif
}
void apIntClear(bool wakeup)
{
    if (wakeup) {
		platReleaseDevInSleepMode(apIntSleepWakeup);
        gpioSet(apIntWkup, 1);
    }
#ifdef AP_INT_NONWAKEUP
    else
        gpioSet(apIntNonWkup, 1);
#endif
}
#else
void apIntInit()
{
}

void apIntSet(bool wakeup)
{
    if (wakeup) {
        if (wakeup && (ipc_get_ap_wake() == AP_SLEEP))
            osLog(LOG_INFO, "%s: gen irq on ap sleep\n", __func__);
        mailboxWriteEvent(MAILBOX_EVT_WAKEUP, 1);
    }
}

void apIntClear(bool wakeup)
{
    if (wakeup) {
        if (wakeup && (ipc_get_ap_wake() == AP_SLEEP))
            osLog(LOG_INFO, "%s: gen irq on ap sleep\n", __func__);
        mailboxWriteEvent(MAILBOX_EVT_WAKEUP, 0);
    }
}
#endif
