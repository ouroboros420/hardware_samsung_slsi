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

#include <stdbool.h>
#include <stdint.h>

#include <cpu.h>
#include <mpu.h>
#include <platform.h>

#include <plat/cmsis.h>

#define MPU_CODE_REGION_BASE    0
#define MPU_CODE_REGION_CNT     4
#define MPU_RAM_REGION_BASE     4
#define MPU_RAM_REGION_CNT      4

#define MPU_RASR_S          0x00040000
#define MPU_RASR_C          0x00020000
#define MPU_RASR_B          0x00010000

/* region type */
#define MPU_TYPE_SRAM       (MPU_RASR_S | MPU_RASR_C)
#define MPU_TYPE_FLASH      (MPU_RASR_C)

/* region execute priviledges */
#define MPU_BIT_XN          (1UL << 28) /* no execute */

/* region access priviledges */
#define MPU_NA              (0UL << 24) /* S: no access   U: no access */
#define MPU_U_NA_S_RW       (1UL << 24) /* S: RW          U: no access */
#define MPU_U_RO_S_RW       (2UL << 24) /* S: RW          U: RO        */
#define MPU_RW              (3UL << 24) /* S: RW          U: RW        */
#define MPU_U_NA_S_RO       (5UL << 24) /* S: RO          U: no access */
#define MPU_U_RO_S_RO       (6UL << 24) /* S: RO          U: RO        */

/* subregion disable (not used so all zeroes) */
#define MPU_SRD_BITS        0x0000UL
#define MPU_BIT_ENABLE      1UL

/* these define rom */
extern uint8_t __code_start[];
extern uint8_t __code_end[];
extern uint8_t __ram_start[];
extern uint8_t __ram_end[];
extern uint8_t __shared_end[];

void MemoryManagemntFault_Handler(void);
void __attribute__((naked)) MemoryManagemntFault_Handler(void)
{
    asm volatile(
        "mov    r0, #3                    \n"
        "b      cpuCommonFaultCode        \n"
    );
}

static bool mpuRegionCfg(uint32_t regionNo, uint32_t start, uint32_t size, uint32_t attrs) /* region will be rounded to acceptable boundaries (32B minimum, self-aligned) by GROWTH */
{
    uint64_t intState;

    if (start & 0x1F) {
        osLog(LOG_ERROR, "%s start addr invaild(%lu)\n", __func__, start);
        return false;
    }

    if (size < 5) {
        osLog(LOG_ERROR, "%s size too small %lu\n", __func__, size);
        return false;
    }

    intState = cpuIntsOff();
    asm volatile("dsb\nisb");

    MPU->RNR = regionNo;
    MPU->RASR = 0; /* disable region before changing it */
    MPU->RBAR = start;
    MPU->RASR = MPU_SRD_BITS | MPU_BIT_ENABLE | attrs | ((size-1) << 1);

    asm volatile("dsb\nisb");
    cpuIntsRestore(intState);

    return true;
}

static void mpuCfgCode(bool allowSvcWrite)
{
    uint32_t codeAddr, codeSize;
    int size, region;

    codeAddr = (uint32_t)__code_start;
    codeSize = __code_end - __code_start;
    region = 0;

    for (size = 31; size > 4; size--) {
        if (codeSize & (1 << size)) {
            mpuRegionCfg(MPU_CODE_REGION_BASE + region, codeAddr,
                         size, MPU_TYPE_FLASH | (allowSvcWrite ? MPU_U_RO_S_RW : MPU_U_RO_S_RO));
            region++;
            codeAddr += (1 << size);
            if (region == MPU_CODE_REGION_CNT)
                break;
        }
    }

}

static void mpuCfgRam(bool allowSvcExecute)
{
    uint32_t ramAddr, ramSize;
    int size, region;

    ramAddr = (uint32_t)__code_end;
    ramSize = __shared_end - __code_end;
    region = 0;

    for (size = 31; size > 4; size--) {
        if (ramSize & (1 << size)) {
            mpuRegionCfg(MPU_RAM_REGION_BASE + region, ramAddr,
                         size, MPU_TYPE_SRAM | MPU_RW | (allowSvcExecute ? 0 : MPU_BIT_XN));
            region++;
            ramAddr += (1 << size);
            if (region == MPU_RAM_REGION_CNT)
                break;
        }
    }
}


void mpuStart(void)
{
    MPU->CTRL = 0x00; // disable MPU

    mpuCfgCode(false);
    mpuCfgRam(false);

    //MPU on, even during faults, supervisor default: allow, user default: default deny
    MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_HFNMIENA_Msk | MPU_CTRL_PRIVDEFENA_Msk;
    //SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    
    mpuShow();
}

void mpuAllowRamExecution(bool allowSvcExecute)
{
    mpuCfgRam(allowSvcExecute);
}

void mpuAllowRomWrite(bool allowSvcWrite)
{
    mpuCfgCode(allowSvcWrite);
}

void mpuShow()
{
    int i, regions = (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos;
    uint32_t addr, rasr;
    uint8_t ap;
    bool xn;
    char *s, *u;

    osLog(LOG_INFO, "MPU: %d HFNMIENA: %d PRIVDEFENA: %d\n",
        !!(MPU->CTRL & MPU_CTRL_ENABLE_Msk),
        !!(MPU->CTRL & MPU_CTRL_HFNMIENA_Msk),
        !!(MPU->CTRL & MPU_CTRL_PRIVDEFENA_Msk));
    for (i=0; i<regions; i++) {
        MPU->RNR = i;
        addr = MPU->RBAR & MPU_RBAR_ADDR_Msk;
        rasr = MPU->RASR;
        xn = rasr & MPU_RASR_XN_Msk;
        ap = (rasr & MPU_RASR_AP_Msk) >> MPU_RASR_AP_Pos;
        if (ap == 0) {
            s = "---";
        } else if (ap == 1 || ap == 2 || ap == 3) {
            if (xn)
                s = "RW-";
            else
                s = "RWX";
        } else if (ap == 5 || ap == 6 || ap == 7) {
            if (xn)
                s = "R--";
            else
                s = "R-X";
        } else {
            s = "???";
        }
        if (ap == 0 || ap == 1 || ap == 5) {
            u = "---";
        } else if (ap == 3) {
            if (xn)
                u = "RW-";
            else
                u = "RWX";
        } else if (ap == 2 || ap == 6 || ap == 7) {
            if (xn)
                u = "R--";
            else
                u = "R-X";
        } else {
            u = "???";
        }
        osLog(LOG_INFO,
            "%d: %c %08lx-%08lx S: %s U: %s TEX: %ld %c%c%c %02lx\n",
            i, (rasr & MPU_RASR_ENABLE_Msk) ? 'E' : 'D',
            addr,
            addr + (1 << (((rasr & MPU_RASR_SIZE_Msk) >> MPU_RASR_SIZE_Pos) + 1))-1,
            s, u,
            (rasr & MPU_RASR_TEX_Msk) >> MPU_RASR_TEX_Pos,
            (rasr & MPU_RASR_S_Msk) ? 'S' : ' ',
            (rasr & MPU_RASR_C_Msk) ? 'C' : ' ',
            (rasr & MPU_RASR_B_Msk) ? 'B' : ' ',
            (rasr & MPU_RASR_SRD_Msk) >> MPU_RASR_SRD_Pos);
    }
}
