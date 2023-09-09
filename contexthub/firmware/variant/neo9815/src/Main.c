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
#include <pwm.h>
#include <wdt.h>
#include <slif.h>

#define CHUB_CMD_INT   (0)
#define CHUB_ALIVE_INT (15)

#define DEFAULT_DHRY_LOOP 1000

#define SR_CMD      SR_0
#define SR_PARAM0   SR_1
#define SR_PARAM1   SR_2
#define SR_RESULT   SR_3

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
void Main(void);

struct func_list_t {
    uint32_t no;
    int (*func)(void);
};

struct func_list_t testFunc_AP[] = {
    {0, doTestCmd}
};

struct audiodata_t {
    uint16_t ch1;
    uint16_t ch0;
};

struct audiodata_t rxBuf[SLIF_RX_BUF_SIZE];
struct audiodata_t txBuf[SLIF_TX_BUF_SIZE];

int slifTxComplete ;
int slifRxComplete ; 
uint32_t mAPCommand;

void mailbox_AP_Handler(void);
void slifTxDone(void);
void slifRxDone(void);
int doSlifTest(void);
int doTestCmd(void);


void slifTxDone(void)
{
    slifTxComplete = 1;
}

void slifRxDone(void)
{
    slifRxComplete = 1;
}

int doSlifTest(void)
{
    int i, timeout;
    int ret;

    ret = slifInit((uint32_t)txBuf, SLIF_TX_BUF_SIZE * 4, (uint32_t)rxBuf, SLIF_RX_BUF_SIZE * 4);
    if (ret)
        return ret;

    slifDisableWriteAddrWrap();
    slifDisableReadAddrWrap();
    slifEnableLoopback();

    slifSetHandler(SLIF_AXI_READ_DONE, slifTxDone);
    slifSetHandler(SLIF_AXI_WRITE_DONE, slifRxDone);

    for (i = 0 ; i < SLIF_TX_BUF_SIZE/2 ; i++)
    {
        txBuf[i].ch0 = i;
        txBuf[i].ch1 = (SLIF_TX_BUF_SIZE/2) - i;
    }
    for (i = SLIF_TX_BUF_SIZE/2  ; i < SLIF_TX_BUF_SIZE ; i++)
    {
        txBuf[i].ch0 = SLIF_TX_BUF_SIZE - i;
        txBuf[i].ch1 = i - (SLIF_TX_BUF_SIZE/2);
    }

    slifStartAll();

    timeout = 0x1000000;
    while (timeout > 0) {
        if (slifTxComplete && slifRxComplete)
            break;
        timeout--;
    }

    if (timeout == 0) {
        CSP_PRINTF_ERROR("SLIF Test : TIMEOUT!!\n");
        return -1;
    }

    for (i = 0 ; i < SLIF_TX_BUF_SIZE ; i++)
    {
        if (*(uint32_t *)&txBuf[i] != *(uint32_t *)&rxBuf[i]) {
            CSP_PRINTF_ERROR("   Data mismatch : %d %x %x\n",
                    i, *(unsigned int*)&txBuf[i], *(unsigned int*)&rxBuf[i]);
            return -1;
        }
    }
    
    return 0;
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
        case 2: // Serial LIF Test
            CSP_PRINTF_INFO("Serial-LIF Test Start\n");
            ret = doSlifTest();
            CSP_PRINTF_INFO("Serial-LIF Test End\n");
            break;
        default:
            break;
  }
  return ret;
}

void mailbox_AP_Handler(void)
{
    uint32_t irq;
    uint32_t i, j;


    irq = ipc_hw_read_int_status_reg(AP);
    irq = irq >> ipc_hw_read_int_start_index(AP);


    mAPCommand = irq;

    for (i = 0 ; i < 16 ; i++) {
	if ((irq & (0x1 << i)) == 0)
            continue;

        ipc_hw_clear_int_pend_reg(AP, i);
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
                mbCmd.cmd = ipc_hw_read_shared_reg(AP, SR_CMD);
                mbCmd.param1 = ipc_hw_read_shared_reg(AP, SR_PARAM0);
                mbCmd.param2 = ipc_hw_read_shared_reg(AP, SR_PARAM1);
                res = testFunc_AP[j].func();
                ipc_hw_write_shared_reg(AP, res, SR_RESULT);
                break;
            }
        }
        ipc_hw_gen_interrupt(AP, i);
    }
}

void sendAlive(void)
{
    ipc_hw_gen_interrupt(AP, CHUB_ALIVE_INT);
}

void Main(void)
{
    int res;

    SCB->VTOR = 0;
    cpuInit();

    wdtDisable();
    pwmInit(TS_PWM_ID, 100000000, PWM_TIMER_MODE_INTERVAL, 0);
    pwmStartTimer(TS_PWM_ID);
    mailboxEnable();

    cpuRegisterHandler(MB_AP_IRQn, mailbox_AP_Handler);
    NVIC_EnableIRQ(MB_AP_IRQn);

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
