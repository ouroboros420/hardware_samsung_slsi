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
        ".word timerIRQHandler + 1              \n" // TIMER_CHUB ISR[0]
#else
        ".word 0                                \n" // TIMER_CHUB ISR[0]
#endif
        ".word wdt_IRQHandler + 1               \n" // WDT_CHUB ISR[1]
#if defined(EXYNOS_VSYNC)
        ".word csis0_IRQHandler + 1             \n" // CSIS0_VVALID ISR[2]
        ".word csis1_IRQHandler + 1             \n" // CSIS1_VVALID ISR[3]
        ".word csis2_IRQHandler + 1             \n" // CSIS2_VVALID ISR[4]
        ".word csis3_IRQHandler + 1             \n" // CSIS3_VVALID ISR[5]
        ".word csis4_IRQHandler + 1             \n" // CSIS4_VVALID ISR[6]
        ".word csis5_IRQHandler + 1             \n" // CSIS4_VVALID ISR[7]
#else
        ".word 0                                \n" // CSIS4_VVALID ISR[2]
        ".word 0                                \n" // CSIS0_VVALID ISR[3]
        ".word 0                                \n" // CSIS1_VVALID ISR[4]
        ".word 0                                \n" // CSIS2_VVALID ISR[5]
        ".word 0                                \n" // CSIS3_VVALID ISR[6]
        ".word 0                                \n" // CSIS4_VVALID ISR[7]
#endif
        ".word 0                                \n" // MB with WLBT ISR[8]
        ".word mailboxAP_IRQHandler + 1         \n" // MB with AP ISR[9]
        ".word 0                                \n" // MB with CP ISR[10]
        ".word mailboxApm2Chub_IRQHandler +1    \n" // MB with APM ISR ISR[11]
        ".word 0                                \n" // MB with GNSS ISR[12]
        ".word 0                                \n" // MB with VTS ISR[13]
        ".word mailboxABOX_IRQHandler + 1       \n" // MB with AUD ISR[14]
        ".word 0                                \n" // MB with NPU0 ISR[15]
        ".word 0                                \n" // MB with NPU1 ISR[16]
        ".word 0                                \n" // Rsvd [17]
        ".word 0                                \n" // SYSREG_CMGP_S ISR[18]
        ".word cmgp_IRQHandler + 1              \n" // SYSREG_CMGP_NS ISR[19]
        ".word rtc_IRQHandler + 1               \n" // RTC ISR[20]
        ".word 0                                \n" // Rsvd [21]
        ".word 0                                \n" // Rsvd [22]
        ".word slif_IRQHandler + 1              \n" // Rsvd [23]
        ".word usi0_IRQHandler + 1              \n" // USI_CHUB00 ISR[24]
        ".word usi1_IRQHandler + 1              \n" // I2C_CHUB00 ISR[25]
        ".word usi2_IRQHandler + 1              \n" // USI_CHUB01 ISR[26]
        ".word usi3_IRQHandler + 1              \n" // I2C_CHUB01 ISR[27]
        ".word usi4_IRQHandler + 1              \n" // USI_CHUB02 ISR[28]
        ".word usi5_IRQHandler + 1              \n" // I2C_CHUB02 ISR[29]
        ".word 0                                \n" // CMGP_REQ ISR[30]
        ".word 0                                \n" // MIF_REQ ISR[31]
        ".word pwm0_IRQHandler + 1              \n" // PWM_CHUB ISR[32]
        ".word pwm1_IRQHandler + 1              \n" // PWM_CHUB ISR[33]
        ".word pwm2_IRQHandler + 1              \n" // PWM_CHUB ISR[34]
        ".word pwm3_IRQHandler + 1              \n" // PWM_CHUB ISR[35]
        ".word pwm4_IRQHandler + 1              \n" // PWM_CHUB ISR[36]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH0_0 ISR[37]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH0_1 ISR[38]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH0_2 ISR[39]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH0_3 ISR[40]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH1_0 ISR[41]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH1_1 ISR[42]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH1_2 ISR[43]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH1_3 ISR[44]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH2_0 ISR[45]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH2_1 ISR[46]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH2_2 ISR[47]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH2_3 ISR[48]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH3_0 ISR[49]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH3_1 ISR[50]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH3_2 ISR[51]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH3_3 ISR[52]
        ".word gpio_IRQHandler + 1              \n" // EXT_INTH3_4 ISR[53]
        ".word 0                                \n" // HPM ISR[54]
    );
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
