/*
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
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

#include <stdint.h>
#include <string.h>
#include <seos.h>
#include <variant.h>

#define VEC_(nm, pfx)    void nm##pfx(void) __attribute__ ((weak, alias ("IntDefaultHandler")))
#define VEC(nm)        VEC_(nm, _Handler)
#define VECI(nm)    VEC_(nm, _IRQHandler)

#ifndef OS_STACK_SIZE
#define OS_STACK_SIZE 8192
#endif

void __attribute__ ((weak)) IntDefaultHandler(void);
VEC(NMI);
VEC(HardFault);
VEC(MemoryManagemntFault);
VEC(BusFault);
VEC(UsageFault);
VEC(SVC);
VEC(DebugMonitor);
VEC(PendSV);
VEC(SysTick);

VECI(mailboxAP);
VECI(mailboxApm2Chub);
VECI(mailboxABOX);
#if defined(TIMER_REQUIRED)
VECI(timer);
#endif
VECI(wdt);
VECI(gpio);
VECI(pwm0);
#if defined(PWM_REQUIRED)
VECI(pwm1);
VECI(pwm2);
VECI(pwm3);
#endif
VECI(cmgp);

#if defined(EXYNOS_VSYNC)
VECI(csis0);
VECI(csis1);
VECI(csis2);
VECI(csis3);
#endif
#if defined(SLIF_REQUIRED)
VECI(slif);
#endif

//stack top (provided by linker)
extern uint32_t __bl_start[];
extern uint32_t __bl_end[];
extern uint32_t __stack_top[];
extern uint32_t __data_data[];
extern uint32_t __data_start[];
extern uint32_t __data_end[];
extern uint32_t __bss_start[];
extern uint32_t __bss_end[];

extern uint8_t __code_start[];
extern uint8_t __code_end[];
extern uint8_t __ram_start[];
extern uint8_t __ram_end[];
extern uint8_t __ipc_start[];
extern uint8_t __ipc_end[];
extern uint8_t __cipc_args_start[];

#if defined(STACK_OVERFLOW_CHECK)
extern bool check_stack_overflow(void);
extern uint32_t __stack_check_top[];
extern uint32_t __stack_check_bottom[];
#define OS_STACK_CHECK_SIZE 128
#define MAGIC_VALUE 0xBABAFEFE
uint64_t __attribute__ ((section (".stack_check"))) _STACK_CHECK[OS_STACK_CHECK_SIZE / sizeof(uint64_t)];
#endif
uint64_t __attribute__ ((section (".stack"))) _STACK[OS_STACK_SIZE / sizeof(uint64_t)];

void __attribute__((noreturn)) IntDefaultHandler(void)
{
    while (1) {
        asm("cpsid i");

        //spin/sleep/whatever forever
        asm("wfi":::"memory");
    }
}

static void init_data(void)
{
    uint32_t *dst, *src, *end;
    //copy data
    dst = __data_start;
    src = __data_data;
    end = __data_end;
    while(dst != end)
        *dst++ = *src++;

    //init bss
    dst = __bss_start;
    end = __bss_end;
    while(dst != end)
        *dst++ = 0;

#if defined(STACK_OVERFLOW_CHECK)
    //set magic value to check stack overflow
    dst = __stack_check_bottom;
    end = __stack_check_top;
    while(dst != end)
        *dst++ = MAGIC_VALUE;
#endif
}

#if defined(STACK_OVERFLOW_CHECK)
bool check_stack_overflow(void)
{
    uint32_t *dst, *end;

#define MAGIC_VALUE 0xBABAFEFE
    dst = __stack_check_bottom;
    end = __stack_check_top;
    while(dst != end)
        if(*dst++ != MAGIC_VALUE) return false;

    return true;
}
#endif

struct chub_bootargs __attribute__ ((section(".bootargs"))) map = {
    .magic = OS_UPDT_MAGIC,
    .ipc_version = IPC_VERSION,
    //.bl_start = (uint32_t)0,
    //.bl_end = (uint32_t)0,
    .code_start = (uint32_t) __code_start,
    .code_end = (uint32_t) __code_end,
    .ipc_start = (uint32_t) __ipc_start,
    .ipc_end = (uint32_t) __ipc_end,
    .cipc_init_map = (uint32_t) __cipc_args_start,
    .ram_start = (uint32_t) __ram_start,
    .ram_end = (uint32_t) __ram_end,
    /* in case of 9610, whole sram is used only by shub */
    .dump_start = (uint32_t) 0,
    .dump_end = (uint32_t) __ram_end,
#ifdef SUPPORT_MULTIOS
    .bootmode = 1,
#endif
};

void __defaultHandler(void);
void __defaultHandler(void)
{
    while(1);
}

void __attribute__((noreturn)) ResetISR(void);
void __attribute__((noreturn)) ResetISR(void)
{
#if defined (LOCAL_POWERGATE)
    if (map.bootmode == BOOTMODE_PWRGATING) {
        pwrDrvPwrGatingRestoreCPU(map.msp, map.psp);
    }
#endif
    init_data();
    osMain();

    //if main returns => bad
    while(1);
}

//vector table
__attribute__ ((section(".vectors"))) __attribute__((naked)) void __VECTORS(void);
__attribute__ ((section(".vectors"))) __attribute__((naked)) void __VECTORS(void)
{
    asm volatile (
        ".word __stack_top                      \n"
        ".word ResetISR + 1                     \n"
        ".word NMI_Handler + 1                  \n"
        ".word HardFault_Handler + 1            \n"
        ".word MemoryManagemntFault_Handler + 1 \n"
        ".word BusFault_Handler + 1             \n"
        ".word UsageFault_Handler + 1           \n"
        ".word 0                                \n"
        ".word 0                                \n"
        ".word 0                                \n"
        ".word 0                                \n"
        ".word SVC_Handler + 1                  \n"
        ".word DebugMonitor_Handler + 1         \n"
        ".word 0                                \n"
        ".word PendSV_Handler + 1               \n"
        ".word SysTick_Handler + 1              \n"


        ".word __defaultHandler                 \n" //ISR[0]  PDMA_CHUB
#if defined(TIMER_REQUIRED)
        ".word timerIRQHandler + 1              \n" //ISR[1]  TIMER_CHUB
#else
        ".word __defaultHandler                 \n" //ISR[1]  TIMER_CHUB
#endif
        ".word __defaultHandler                 \n" //ISR[2]  WDT_CHUB
#if defined(EXYNOS_VSYNC)
        ".word csis0_IRQHandler + 1             \n" //ISR[3]  CSIS_VVALID
#else
        ".word __defaultHandler                 \n" //ISR[3]  CSIS_VVALID
#endif
        ".word pwm0_IRQHandler + 1              \n" //ISR[4]  PWM0_CHUB
#if defined(PWM_REQUIRED)
        ".word pwm1_IRQHandler + 1              \n" //ISR[5]  PWM1_CHUB
        ".word pwm2_IRQHandler + 1              \n" //ISR[6]  PWM2_CHUB
        ".word pwm3_IRQHandler + 1              \n" //ISR[7]  PWM3_CHUB
#else
        ".word __defaultHandler                 \n" //ISR[5]  PWM1_CHUB
        ".word __defaultHandler                 \n" //ISR[6]  PWM2_CHUB
        ".word __defaultHandler                 \n" //ISR[7]  PWM3_CHUB
#endif
        ".word __defaultHandler                 \n" //ISR[8]  MB with WLBT
        ".word mailboxAP_IRQHandler + 1         \n" //ISR[9]  MB with AP
        ".word __defaultHandler                 \n" //ISR[10]  MB with CP
#if defined(MBAPM_REQUIRED)
        ".word mailboxApm2Chub_IRQHandler +1    \n" //ISR[11]  MB with APM
#else
        ".word __defaultHandler                 \n" //ISR[11]  MB with APM
#endif
        //".word MailboxGnss2Chub_IRQHandler      \n" //ISR[12]  MB with GNSS
        ".word __defaultHandler                 \n" //ISR[13]  SYSREG_CMGP 0
        ".word __defaultHandler                 \n" //ISR[13]  SYSREG_CMGP 0
        ".word cmgp_IRQHandler + 1              \n" //ISR[14]  SYSREG_CMGP 1
        ".word rtc_IRQHandler + 1          \n" //ISR[15]  CHUB_RTC_TICK_0
        ".word __defaultHandler                 \n" //ISR[16]  MIF ACK
        ".word __defaultHandler                 \n" //ISR[17]  CMGP_EINT[0]
        ".word __defaultHandler                 \n" //ISR[18]  CMGP_EINT[1]
        ".word __defaultHandler                 \n" //ISR[19]  CMGP_EINT[2]
        ".word __defaultHandler                 \n" //ISR[20]  CMGP_EINT[3]
        ".word __defaultHandler                 \n" //ISR[21]  CMGP_EINT[4]
        ".word __defaultHandler                 \n" //ISR[22]  CMGP_EINT[5]
        ".word rtc_IRQHandler + 1          \n" //ISR[23]  CHUB_RTC_TICK_1
        ".word __defaultHandler                 \n" //ISR[24]  USI_CHUB00
        ".word __defaultHandler                 \n" //ISR[25]  I2C_CHUB00
        ".word __defaultHandler                 \n" //ISR[26]  I2C_CHUB01
        ".word sysregApmReqAck_IRQHandler       \n" //ISR[27]  APM_REQ_ACK
    );
};


struct cipc_user_map_info __attribute__ ((section(".cipc_args"))) cipc_map_info[CIPC_USER_MAX] = {
    {"AP2CHUB", CIPC_REG_AP2CHUB, CIPC_REG_EVT_AP2CHUB,
        {CIPC_REG_DATA_AP2CHUB, 0},
        {CIPC_DATA_AP2CHUB_SIZE, 0},
        {CIPC_DATA_AP2CHUB_CNT, 0},
        IPC_OWN_AP, {IPC_OWN_AP, IPC_SRC_MB0}, {IPC_OWN_CHUB, IPC_DST_MB1}, 1},
    {"CHUB2AP", CIPC_REG_CHUB2AP, CIPC_REG_EVT_CHUB2AP,
        {CIPC_REG_DATA_CHUB2AP, CIPC_REG_DATA_CHUB2AP_BATCH},
        {CIPC_DATA_CHUB2AP_SIZE, CIPC_DATA_CHUB2AP_BATCH_SIZE},
        {CIPC_DATA_CHUB2AP_CNT, CIPC_DATA_CHUB2AP_BATCH_CNT},
        IPC_OWN_AP, {IPC_OWN_CHUB, IPC_DST_MB1}, {IPC_OWN_AP, IPC_SRC_MB0}, 2},
};

struct image_info {
  char soc[8];
  unsigned int ipc_version;
  char time[21];
  char name[19];
  char commit[200];
};

static const struct image_info __attribute__ ((section(".info"))) build_info = {
  .soc = SOCNAME,
  .ipc_version = IPC_VERSION,
  .time = __DATE__" "__TIME__,
  .name = BUILDER,
  .commit = BASE_COMMIT
};
