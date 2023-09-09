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
#include <cmsis.h>
#include <csp_common.h>
#include <variant/dhry.h>
#include <cpu.h>
#include <plat/cpu/cpu.h>
#include <mailbox.h>
#include <ipc_chub.h>
#include <ipc_hw.h>
#include <pwm.h>
#include <wdt.h>
#include <plat/wdt/wdt.h>
#include <timer.h>
#include <rtc.h>


#define CHUB_CMD_INT   (0)
#define CHUB_ALIVE_INT (15)

#define DEFAULT_DHRY_LOOP 1000

#define SR_CMD      SR_0
#define SR_PARAM0   SR_1
#define SR_PARAM1   SR_2
#define SR_RESULT   SR_3

#define PWM_TEST_ID 2

#define CHUB_SRAM_SIZE (2 * 1024 * 1024) // 2MB

typedef struct {
  uint32_t cmd;
  uint32_t param1;
  uint32_t param2;
  uint32_t resp;
} mbCmd_t;

mbCmd_t mbCmd;

int doTestCmd(void);
void mailbox_handler(void);
void doCommand(unsigned int cmd);
void sendAlive(void);
int sram_test(unsigned int start, unsigned int end);
void Main(void);

struct func_list_t {
    uint32_t no;
    int (*func)(void);
};

struct func_list_t testFunc_AP[] = {
    {0, doTestCmd}
};

extern uint8_t __ipc_start[];
extern uint8_t __ipc_end[];

uint32_t mAPCommand;
uint32_t mPwmInt;
uint32_t mWdtInt;
uint32_t mRtcInt;

void mailbox_AP_Handler(void);
int doTestCmd(void);
//int doPwmTest(void);
//int doWdtTest(void);
//int doRtcTest(void);
//void pwmTestHandler(void);
//void wdtTestHandler(void);
//void rtcTestHandler(void);

static void pwmTestHandler(void)
{
    mPwmInt = 1;
}

static void wdtTestHandler(void)
{
    mWdtInt = 1;
    wdtClearPend();
    NVIC_DisableIRQ(WDT_CHUB_IRQn);
}

static void rtcTestHandler(void)
{
    mRtcInt = 1;
}


static int doPwmTest(void)
{
    uint64_t start = rtcGetTimeStampUS();

    while((rtcGetTimeStampUS() - start) < 1100) {
        if (mPwmInt == 1)
           return 0;
    }

    return -1;
}

static int doWdtTest(void)
{
    uint64_t start = rtcGetTimeStampUS();

    while((rtcGetTimeStampUS() - start) < 1100) {
        if (mWdtInt == 1)
           return 0;
    }

    return -1;
}

static int doRtcTest(void)
{
    uint64_t start = rtcGetTimeStampUS();

    while((rtcGetTimeStampUS() - start) < 1100) {
        if (mRtcInt == 1)
           return 0;
    }

    return -1;
}

int doTestCmd(void)
{
    int loop = DEFAULT_DHRY_LOOP;
    int ret = 0;

    switch(mbCmd.cmd) {
        case 1:  // CPU Test
            CSP_PRINTF_INFO("Dhry Start\n");
            if (mbCmd.param1 != 0)
                loop = mbCmd.param1 * 500;
            dhry1(loop, 393);
            CSP_PRINTF_INFO("Dhry End\n");
            break;
        case 2: // PWM Test
            CSP_PRINTF_INFO("PWM Test Start\n");
            ret = doPwmTest();
            CSP_PRINTF_INFO("PWM Test End\n");
            break;
        case 3: // RTC Test
            CSP_PRINTF_INFO("RTC Test Start\n");
            ret = doRtcTest();
            CSP_PRINTF_INFO("RTC Test End\n");
            break;
        case 4: // SRAM Test
            CSP_PRINTF_INFO("SRAM Test Start\n");
            ret = sram_test((unsigned int)__ipc_end, CHUB_SRAM_SIZE - (unsigned int)__ipc_end);
            CSP_PRINTF_INFO("SRAM Test End\n");
            break;
        case 5: // WDT Test
            CSP_PRINTF_INFO("WDT Test Start\n");
            ret = doWdtTest();
            CSP_PRINTF_INFO("WDT Test End\n");
            break;
        default:
            break;
  }
  CSP_PRINTF_INFO("TEST Result: %d\n", ret);
  return ret;
}

void mailbox_AP_Handler(void)
{
    uint32_t irq;
    uint32_t i, j;


    irq = ipc_hw_read_int_status_reg_all((void *)MAILBOX_AP_BASE_ADDRESS, IPC_DST_MB1);


    mAPCommand = irq;

    for (i = 0 ; i < 16 ; i++) {
        if ((irq & (0x1 << i)) == 0)
            continue;

        ipc_hw_clear_int_pend_reg((void *)MAILBOX_AP_BASE_ADDRESS, IPC_DST_MB1, i);
    }
}

void doCommand(unsigned int cmd)
{
    int res;
    int i, j;

    for (i = 0 ; i < 16 ; i++) {
        if ((cmd & (0x1 << i)) == 0)
            continue;

        for (j = 0 ; j < sizeof(testFunc_AP)/sizeof(struct func_list_t) ; j++) {
            if (testFunc_AP[j].no == i) {
                mbCmd.cmd    = ipc_hw_read_shared_reg((void *)MAILBOX_AP_BASE_ADDRESS, SR_CMD);
                mbCmd.param1 = ipc_hw_read_shared_reg((void *)MAILBOX_AP_BASE_ADDRESS, SR_PARAM0);
                mbCmd.param2 = ipc_hw_read_shared_reg((void *)MAILBOX_AP_BASE_ADDRESS, SR_PARAM1);
                res = testFunc_AP[j].func();
                ipc_hw_write_shared_reg((void *)MAILBOX_AP_BASE_ADDRESS, res, SR_RESULT);
                break;
            }
        }
        ipc_hw_gen_interrupt((void *)MAILBOX_AP_BASE_ADDRESS, IPC_SRC_MB0, i);
    }
}

void sendAlive(void)
{
    ipc_hw_gen_interrupt((void *)MAILBOX_AP_BASE_ADDRESS, IPC_SRC_MB0, CHUB_ALIVE_INT);
}

static void clear_ipc_area(void)
{
    unsigned int *pIpc = (unsigned int *)__ipc_start;

    while(pIpc < (unsigned int*)__ipc_end)
        *pIpc++ = 0;
}

void Main(void)
{
    int res;

    SCB->VTOR = 0;
    cpuInit();

    wdtDisable();
    rtcInit();
    clear_ipc_area();
    mailboxEnable();

    cpuRegisterHandler(MB_AP_IRQn, mailbox_AP_Handler);
    NVIC_EnableIRQ(MB_AP_IRQn);

    pwmSetHandler(PWM_TEST_ID, pwmTestHandler);
    pwmInit(PWM_TEST_ID, 1000, PWM_TIMER_MODE_ONESHOT, 1);
    pwmStartTimer(PWM_TEST_ID);
    rtcSetTickTime(0, 1, 0, rtcTestHandler, 0);
    //wdtSetTime(1);
    //wdtEnableInterrupt();
    //wdtEnable();
    cpuRegisterHandler(WDT_CHUB_IRQn, wdtTestHandler);
    NVIC_EnableIRQ(WDT_CHUB_IRQn);

    sendAlive();

    while(1) {
        if (mAPCommand) {
            NVIC_DisableIRQ(MB_AP_IRQn);
            doCommand(mAPCommand);
            mAPCommand = 0;
            NVIC_EnableIRQ(MB_AP_IRQn);
        }
        __asm("wfi");
    }
}
