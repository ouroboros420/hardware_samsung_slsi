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
extern uint32_t __code_start[];
extern uint32_t __stack_top[];
extern uint32_t __data_data[];
extern uint32_t __data_start[];
extern uint32_t __data_end[];
extern uint32_t __bss_start[];
extern uint32_t __bss_end[];

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

void __attribute__((noreturn)) ResetISR(void);
void __attribute__((noreturn)) ResetISR(void)
{
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


#if defined(TIMER_REQUIRED)
        ".word timerIRQHandler + 1              \n" // [00] TIMER_CHUB ISR
#else
        ".word 0                                \n" // [00] TIMER_CHUB ISR
#endif
        ".word wdt_IRQHandler + 1               \n" // [01] WDT_CHUB ISR
        ".word 0                                \n" // [02] Rsvd
        ".word 0                                \n" // [03] Rsvd
        ".word 0                                \n" // [04] Rsvd
        ".word 0                                \n" // [05] Rsvd
        ".word 0                                \n" // [06] Rsvd
        ".word 0                                \n" // [07] Rsvd
        ".word 0                                \n" // [08] Rsvd
        ".word 0       \n" // [09] MB with GNSS ISR
        //kang
        //".word mailboxGnss_IRQHandler + 1       \n" // [09] MB with GNSS ISR
        ".word mailboxApm2Chub_IRQHandler +1    \n" // [10] MB with APM ISR
        ".word 0                                \n" // [11] SPI_CHUB ISR
        ".word 0                                \n" // [12] SPI_GNSS ISR
        ".word 0                                \n" // [13] UART_CP ISR
        ".word mailboxABOX_IRQHandler + 1       \n" // [14] Rsvd
        ".word 0                                \n" // [15] CHUB_RTC ISR
        ".word 0                                \n" // [16] I2C_CHUB1 ISR
        ".word 0                                \n" // [17] I3C_CHUB1 ISR
        ".word 0                                \n" // [18] USI_CHUB0 ISR
        ".word 0                                \n" // [19] USI_CHUB1 ISR
        ".word 0                                \n" // [20] USI_CHUB2 ISR
        ".word 0                                \n" // [21] USI_CHUB3 ISR
        ".word 0                                \n" // [22] I2C_CHUB0 ISR
        ".word 0                                \n" // [23] I3C_CHUB0 ISR
        ".word 0                                \n" // [24] CMGP_REQ_ACK ISR
        ".word 0                                \n" // [25] Rsvd
        ".word 0                                \n" // [26] PWM_CHUB0 ISR
        ".word 0                                \n" // [27] PWM_CHUB1 ISR
        ".word 0                                \n" // [28] PWM_CHUB2 ISR
        ".word 0                                \n" // [29] PWM_CHUB3 ISR
        ".word 0                                \n" // [30] PWM_CHUB4 ISR
        ".word 0                                \n" // [31] EXT_INTH0_0 ISR
        ".word 0                                \n" // [32] EXT_INTH0_1 ISR
        ".word 0                                \n" // [33] EXT_INTH0_2 ISR
        ".word 0                                \n" // [34] EXT_INTH0_3 ISR
        ".word 0                                \n" // [35] EXT_INTH0_4 ISR
        ".word 0                                \n" // [36] EXT_INTH0_5 ISR
        ".word 0                                \n" // [37] EXT_INTH0_6 ISR
        ".word 0                                \n" // [38] EXT_INTH0_7 ISR
        ".word 0                                \n" // [39] EXT_INTH1_0 ISR
        ".word 0                                \n" // [40] EXT_INTH1_1 ISR
        ".word 0                                \n" // [41] EXT_INTH1_2 ISR
        ".word 0                                \n" // [42] EXT_INTH1_3 ISR
        ".word 0                                \n" // [43] EXT_INTH1_4 ISR
        ".word 0                                \n" // [44] EXT_INTH1_5 ISR
        ".word 0                                \n" // [45] EXT_INTH1_6 ISR
        ".word 0                                \n" // [46] EXT_INTH1_7 ISR
        ".word 0                                \n" // [47] EXT_INTH2_0 ISR
        ".word 0                                \n" // [48] EXT_INTH2_1 ISR
        ".word 0                                \n" // [49] EXT_INTH2_2 ISR
        ".word 0                                \n" // [50] EXT_INTH2_3 ISR
        ".word 0                                \n" // [51] EXT_INTH3_0 ISR
        ".word 0                                \n" // [52] EXT_INTH3_1 ISR
        ".word 0                                \n" // [53] EXT_INTH3_2 ISR
        ".word 0                                \n" // [54] EXT_INTH3_3 ISR
        ".word 0                                \n" // [55] EXT_INTH3_4 ISR
        ".word 0                                \n" // [56] EXT_INTH3_5 ISR
        ".word 0                                \n" // [57] EXT_INTH3_6 ISR
        ".word 0                                \n" // [58] EXT_INTH3_7 ISR
        ".word 0                                \n" // [59] EXT_INTH4_0 ISR
        ".word 0                                \n" // [60] EXT_INTH4_1 ISR
        ".word 0                                \n" // [61]
        ".word 0                                \n" // [62]
        ".word 0                                \n" // [63]
        ".word 0                                \n" // [64]
        ".word 0                                \n" // [65]
        ".word 0                                \n" // [66]
        ".word 0                                \n" // [67] SPI_I2C_CHUB0 ISR
        ".word 0                                \n" // [68] SPI_I2C_CHUB1 ISR
        ".word 0                                \n" // [69]
    );
};

struct image_info {
  char soc[8];
  unsigned int ipc_version;
  char time[21];
  char name[19];
  char commit[200];
};

const struct image_info __attribute__ ((section(".info"))) build_info = {
  .soc = SOCNAME,
  .ipc_version = IPC_VERSION,
  .time = __DATE__" "__TIME__,
  .name = BUILDER,
  .commit = BASE_COMMIT
};
