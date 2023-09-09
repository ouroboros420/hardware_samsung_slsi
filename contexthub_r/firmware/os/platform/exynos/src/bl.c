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

#include <cmsis.h>
#include <bl.h>
#include <string.h>

#ifdef SUPPORT_EXT_NANO
#include <nanohub/sha2.h>
#include <nanohub/aes.h>
#include <nanohub/rsa.h>
#endif
#include <csp_common.h>
#include <ipc_chub.h>
#include <pwrDrvPwrGating.h>

#undef NON_NANO
//linker provides these
extern uint32_t __pubkeys_start[];
extern uint32_t __pubkeys_end[];
extern uint8_t __stack_top[];
#ifdef NON_NANO
#define __bl_start 0
#define __bl_end 0x3800
#define __ram_start 0x29000
#define __ram_end 0x3f000
#define __code_start 0x3800
#define __code_end 0x24000
#define __shared_start 0x3f000
#define __shared_end 0x40000
#else
extern uint8_t __bl_start[];
extern uint8_t __bl_end[];
extern uint8_t __ram_start[];
extern uint8_t __ram_end[];
extern uint8_t __code_start[];
extern uint8_t __code_end[];
extern uint8_t __shared_start[];
extern uint8_t __shared_end[];
#endif
extern uint8_t __bss_end[];
extern uint8_t __bss_start[];
extern uint8_t __data_start[];
extern uint8_t __data_end[];
extern uint8_t __data_data[];
extern uint8_t __ipc_start[];
extern uint8_t __ipc_end[];
extern uint8_t __cipc_args_start[];

//BL stack
uint64_t
    __attribute__ ((section(".stack"))) _STACK[BL_STACK_SIZE /
                                               sizeof(uint64_t)];

// Default IRQ Handler, which reset system
static void defaultHandler(void)
{
    //SCB->AIRCR = 0x05FA0004;

    //we never get here
    while (1) ;
}

// AP Mailbox IRQ Handler
static void mailboxApHandler(void)
{

}

#if 0 // not for cipc def FULL_BL
// APM Mailbox IRQ Handler
static void mailboxApmHandler(void)
{
#if defined(APM_HANDSHAKING)
    uint32_t i, num;
    uint32_t sr0, sr2;

    num = IPC_HW_READ_STATUS1(MAILBOX_APM_BASE_ADDRESS);
    for (i = 0; i <= 15; i++) {
        if (num & (1 << i)) {
            if (i == IRQ_CH2) {
                IPC_HW_CLEAR_PEND1(MAILBOX_APM_BASE_ADDRESS, IRQ_CH2);

                sr2 = IPC_HW_READ_SHARED_REG(MAILBOX_APM_BASE_ADDRESS, SR_2);

                IPC_HW_WRITE_SHARED_REG(MAILBOX_APM_BASE_ADDRESS, SR_2, 0);
                IPC_HW_WRITE_SHARED_REG(MAILBOX_APM_BASE_ADDRESS, SR_3, 0);

                if ((sr2 & 0xf) == 1) {
                    sr0 = 1;
                    IPC_HW_WRITE_SHARED_REG(MAILBOX_APM_BASE_ADDRESS, SR_0,
                                            sr0);
                    IPC_HW_GEN_INTERRUPT_GR0(MAILBOX_APM_BASE_ADDRESS, IRQ_CH2);
                }
            } else {
                IPC_HW_CLEAR_PEND1(MAILBOX_APM_BASE_ADDRESS, i);
            }
        }
    }
#endif
}
#endif

#ifdef SUPPORT_EXT_APP
static void blExtApiReboot(void)
{
    SCB->AIRCR = 0x05FA0004;
    //we never get here
    while (1) ;
}

#ifdef SUPPORT_EXT_NANO
static const uint32_t *blExtApiGetRsaKeyInfo(uint32_t * numKeys)
{
    uint32_t numWords = __pubkeys_end - __pubkeys_start;

    if (numWords % RSA_WORDS)   // something is wrong
        return NULL;

    *numKeys = numWords / RSA_WORDS;
    return __pubkeys_start;
}

static const uint32_t *blExtApiSigPaddingVerify(const uint32_t * rsaResult)
{
    uint32_t i;

    //all but first and last word of padding MUST have no zero bytes
    for (i = SHA2_HASH_WORDS + 1; i < RSA_WORDS - 1; i++) {
        if (!(uint8_t) (rsaResult[i] >> 0))
            return NULL;
        if (!(uint8_t) (rsaResult[i] >> 8))
            return NULL;
        if (!(uint8_t) (rsaResult[i] >> 16))
            return NULL;
        if (!(uint8_t) (rsaResult[i] >> 24))
            return NULL;
    }

    //first padding word must have all nonzero bytes except low byte
    if ((rsaResult[SHA2_HASH_WORDS] & 0xff)
        || !(rsaResult[SHA2_HASH_WORDS] & 0xff00)
        || !(rsaResult[SHA2_HASH_WORDS] & 0xff0000)
        || !(rsaResult[SHA2_HASH_WORDS] & 0xff000000))
        return NULL;

    //last padding word must have 0x0002 in top 16 bits and nonzero random bytes in lower bytes
    if ((rsaResult[RSA_WORDS - 1] >> 16) != 2)
        return NULL;
    if (!(rsaResult[RSA_WORDS - 1] & 0xff00)
        || !(rsaResult[RSA_WORDS - 1] & 0xff))
        return NULL;

    return rsaResult;
}
#endif
static uint32_t blExtApiGetVersion(void)
{
    return BL_VERSION_CUR;
}
#endif // SUPPORT_EXT_APP


// Avoid compiler complaint
void __blEntry(void);

// Boot vector
#include CSP_HEADER(blBootVec)

#ifdef SUPPORT_EXT_APP
static bool blExtApiProgramSharedArea(uint8_t *dst, const uint8_t *src, uint32_t length, uint32_t key1, uint32_t key2)
{
    memcpy(dst, src, length);
    return 1;
}

static bool blExtApiEraseSharedArea(uint32_t key1, uint32_t key2) // TODO: erase whole area??
{
    if (__shared_end - __shared_start)
        memset((void *)__shared_start, 0, __shared_end - __shared_start); // TODO: 0 is write??
    return 1;
}

static bool blExtApiProgramEe(uint8_t *dst, const uint8_t *src, uint32_t length, uint32_t key1, uint32_t key2)
{
    memcpy(dst, src, length);
    return 1;
}
#endif // SUPPROT_EXT_APP

static void blExtApiNotifyOs(void)
{
    __disable_irq();
    __raw_writel(1<<(IRQ_NUM_CHUB_ALIVE + IPC_HW_IRQ_MAX), MAILBOX_AP_BASE_ADDRESS + REG_MAILBOX_INTGR0);

    do{}
    while(__raw_readl(MAILBOX_AP_BASE_ADDRESS + REG_MAILBOX_ISSR0 + SR_DEBUG_VAL_LOW * 4) != READY_TO_GO);

	 SCB->VTOR = ((uint32_t) __bl_start) & ~1;
	 asm volatile ("MOV R0, #0\n" "LDR SP, [R0]\n");

     /*
      * The address(0) is protected with MPU. So,you should close MPU before writing it.
      */
     if(MPU->CTRL & (1UL << 0)/*Check the status of MPU is openned or not*/){
         MPU->CTRL = 0;
         __DSB();
         __ISB();
     }

    __blEntry();
}

const struct BlApiTable __attribute__((section(".blapi"))) __BL_API =
{
    .blNotifyOS = &blExtApiNotifyOs,
#ifdef SUPPORT_EXT_APP
    /* api */
    .blGetVersion = &blExtApiGetVersion,
    .blReboot = &blExtApiReboot,
    .blGetSnum = NULL,
    .blProgramShared = &blExtApiProgramSharedArea,
    .blEraseShared = &blExtApiEraseSharedArea,
    .blProgramEe = &blExtApiProgramEe,
#ifdef SUPPORT_EXT_NANO
    .blGetPubKeysInfo = &blExtApiGetRsaKeyInfo,
    .blRsaPubOpIterative = &rsaPubOpIterative,
    .blSha2init = &sha2init,
    .blSha2processBytes = &sha2processBytes,
    .blSha2finish = &sha2finish,
    .blAesInitForEncr = &aesInitForEncr,
    .blAesInitForDecr = &aesInitForDecr,
    .blAesEncr = &aesEncr,
    .blAesDecr = &aesDecr,
    .blAesCbcInitForEncr = &aesCbcInitForEncr,
    .blAesCbcInitForDecr = &aesCbcInitForDecr,
    .blAesCbcEncr = &aesCbcEncr,
    .blAesCbcDecr = &aesCbcDecr,
    .blSigPaddingVerify = &blExtApiSigPaddingVerify,
#endif
    .blVerifyOsUpdate = NULL,
#endif
};

#ifdef FULL_BL
// Cut off unnecessary clock and power
static inline void minPwr(void)
{

}
#endif

static inline void wait_for_interrupt(void)
{
#if defined ( __GNUC__ )
    __WFI();
#else
    asm volatile ("wfi\n" "nop":::"memory");
#endif
}

#if 0
#if !defined(FIRMWARE_ONLY)
static int checkOsImage(void)
{
    /* if set BOOT_MODE, load OS image */
    if (IPC_HW_READ_SHARED_REG(MAILBOX_AP_BASE_ADDRESS, SR_BOOT_MODE))
        return 1;
    else
        return 0;
}
#endif
#endif

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
	{"ABOX2CHUB", CIPC_REG_ABOX2CHUB, CIPC_REG_EVT_ABOX2CHUB,
		{CIPC_REG_DATA_ABOX2CHUB_AUD, CIPC_REG_DATA_ABOX2CHUB},
		{CIPC_DATA_ABOX2CHUB_SIZE, CIPC_DATA_ABOX2CHUB_PROX_SIZE},
		{CIPC_DATA_ABOX2CHUB_CNT,  CIPC_DATA_ABOX2CHUB_PROX_CNT},
		IPC_OWN_ABOX, {IPC_OWN_ABOX, IPC_DST_MB1}, {IPC_OWN_CHUB, IPC_SRC_MB0}, 2},
	{"CHUB2ABOX", CIPC_REG_CHUB2ABOX, CIPC_REG_EVT_CHUB2ABOX,
		{CIPC_REG_DATA_CHUB2ABOX, 0},
		{CIPC_DATA_CHUB2ABOX_SIZE, 0},
		{CIPC_DATA_CHUB2ABOX_CNT, 0},
		IPC_OWN_ABOX, {IPC_OWN_CHUB, IPC_SRC_MB0}, {IPC_OWN_ABOX, IPC_DST_MB1}, 1},
};

struct chub_bootargs __attribute__ ((section(".bootargs"))) map = {
    .magic = OS_UPDT_MAGIC,
    .ipc_version = IPC_VERSION,
    .bl_start = (uint32_t) __bl_start,
    .bl_end = (uint32_t) __bl_end,
    .code_start = (uint32_t) __code_start,
    .code_end = (uint32_t) __code_end,
    .ipc_start = (uint32_t) __ipc_start,
    .ipc_end = (uint32_t) __ipc_end,
    .cipc_init_map = (uint32_t) __cipc_args_start,
    .ram_start = (uint32_t) __ram_start,
    .ram_end = (uint32_t) __ram_end,
    /* in case of 9610, whole sram is used only by shub */
    .dump_start = (uint32_t) __bl_start,
    .dump_end = (uint32_t) __ram_end,
#ifdef SUPPORT_MULTIOS
    .bootmode = 1,
#endif
};

void __blEntry(void)
{
#ifdef NON_NANO
    uint32_t appBase = ((uint32_t) __code_start) & ~1;
#else
    uint32_t appBase = ((uint32_t) & __code_start) & ~1;
#endif
    uint32_t i;
    uint32_t *bootCnt;

#if defined(LOCAL_POWERGATE)
    // Check whether or not system wakes up from local-power gating
    if(map.bootmode == BOOTMODE_PWRGATING) {
        // Restore CPU and go back directly to OS
        pwrDrvPwrGatingRestoreCPU(map.msp, map.psp);
    }
#endif

    // Disable irq
    __disable_irq();

    memset((void *)map.ram_start, 0, map.ram_end - map.ram_start);
    //init things a little for the higher levels
    memset(__bss_start, 0, __bss_end - __bss_start);
    memcpy(__data_start, __data_data, __data_end - __data_start);

    // Setup Vector Table
    SCB->VTOR = (uint32_t) & _BL;

#ifdef FULL_BL
    // Cut off unnecessary clock and power
    minPwr();
#endif

    // Disable System Timer
    SysTick->CTRL = 0;

    bootCnt = (uint32_t *)(map.code_start-4);
    if(*bootCnt == 0xffffffff)
        *bootCnt=1;
    else if(*bootCnt !=0)
        *bootCnt += 1;

    // Initialize NVIC
    NVIC_SetPriorityGrouping(7);

    for (i = 0; i < NUM_INTERRUPTS; i++) {
        NVIC_SetPriority(i, i);
        NVIC_DisableIRQ(i);
        NVIC_ClearPendingIRQ(i);
    }

#if defined(APM_HANDSHAKING)
    // Enable irqs
    NVIC_EnableIRQ(MB_APM_IRQn);
#endif

#if 0
    // Enable irq
    __enable_irq();

#if 0//!defined(FIRMWARE_ONLY)
    // Infinite loop
    while (1) {
        if (checkOsImage())
            break;
        wait_for_interrupt();
    }
#endif
#endif

    //Jump to OS with irq disabled
    __disable_irq();
    SCB->VTOR = appBase;

    asm volatile ("LDR SP, [%0] \n" "LDR PC, [%0, #4] \n"::"r" (appBase)
                  :"memory", "cc");

    // Never reach here
    while (1) ;
}
