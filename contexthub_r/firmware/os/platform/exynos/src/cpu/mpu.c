/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CPU MPU
 *----------------------------------------------------------------------------
 *      Name:    mpu.c
 *      Purpose: To implement MPU driver
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

#include <csp_common.h>
#include <mpu.h>
#if defined(SEOS)
#include <cmsis.h>
#endif
#include <cpu.h>
#include <splitMpuRegion.h>

extern uint32_t __code_end;
extern uint32_t __guard_area_start;

//Note:CODE_AREA_START must be zero in this implement.
#define CODE_AREA_START     0x0ul
#define CODE_AREA_END       ((uint32_t)&__code_end -0x1ul)
#define CODE_AREA_SIZE      (((uint32_t)&__code_end)/1024)

// region id - Cortex-M4 MPU has 8 regions and a background region. The highest number prioritize over lower one
#define MPU_REGION_0    0
#define MPU_REGION_1    1
#define MPU_REGION_2    2
#define MPU_REGION_3    3
#define MPU_REGION_4    4
#define MPU_REGION_5    5
#define MPU_REGION_6    6
#define MPU_REGION_7    7

// region executable atrribute
#define MPU_NOT_EXECUTABLE (1UL << 28) /* no execute */
#define MPU_EXECUTABLE     (0UL << 28) /* execute */

// region accessible attribute
#define MPU_AP_NA          (0UL << 24) /* Privileged: no access   Unprivileged: no access */
#define MPU_AP_U_NA_P_RW   (1UL << 24) /* Privileged: RW          Unprivileged: no access */
#define MPU_AP_U_RO_P_RW   (2UL << 24) /* Privileged: RW          Unprivileged RO        */
#define MPU_AP_RW          (3UL << 24) /* Privileged: RW          Unprivileged: RW        */
#define MPU_AP_U_NA_P_RO   (5UL << 24) /* Privileged: RO          Unprivileged: no access */
#define MPU_AP_U_RO_P_RO   (6UL << 24) /* Privileged: RO          Unprivileged: RO        */

//This len must equal the size of SRD in RASA.
#define SRD_LEN         8

// region size attribute
#define MPU_SIZE_32B    (4UL << 1)
#define MPU_SIZE_1KB    (9UL << 1)
#define MPU_SIZE_2KB    (10UL << 1)
#define MPU_SIZE_4KB    (11UL << 1)
#define MPU_SIZE_8KB    (12UL << 1)
#define MPU_SIZE_16KB   (13UL << 1)
#define MPU_SIZE_32KB   (14UL << 1)
#define MPU_SIZE_64KB   (15UL << 1)
#define MPU_SIZE_128KB  (16UL << 1)
#define MPU_SIZE_256KB  (17UL << 1)
#define MPU_SIZE_512KB  (18UL << 1)
#define MPU_SIZE_1MB    (19UL << 1)
#define MPU_SIZE_1GB    (29UL << 1)
#define MPU_SIZE_4GB    (31UL << 1)
//The param(x) is from MPU_SIZE_XXX
#define MPU_GET_LENGTH(x)	(0x1ul << ((x>>0x1ul)+0x1ul))
#define MPU_GET_SUBREGION_SIZE(x)	(MPU_GET_LENGTH(x) / SRD_LEN)

// region enable bit
#define MPU_REGION_ENABLE  1UL

// subregion disabled bits
#define MPU_SUBREGION_DISABLE    0xFF00UL

// TEX(19), C(17), B(16), S(18)
#define MPU_SO_TEX_C_B_NS       ((0UL << 19) | (0UL << 17) | (0UL << 16) | (0 << 18)) /* Strongly ordered */
#define MPU_DEVICE_TEX_C_B_NS   ((2UL << 19) | (0UL << 17) | (0UL << 16) | (0 << 18)) /* Device */
#define MPU_DEVICE_TEX_C_B_S   ((0UL << 19) | (0UL << 17) | (1UL << 16) | (1 << 18)) /* Device, shareable */
#define MPU_NORMAL_WT_NOWA_TEX_C_B_NS   ((0UL << 19) | (1UL << 17) | (0UL << 16) | (0 << 18)) /* Normal, Write-through, No write-alloc */
#define MPU_NORMAL_WT_NOWA_TEX_C_B_S   ((0UL << 19) | (1UL << 17) | (0UL << 16) | (1 << 18)) /* Normal, Write-through, No write-alloc, shareable */
#define MPU_NORMAL_WB_NOWA_TEX_C_B_NS   ((0UL << 19) | (1UL << 17) | (1UL << 16) | (0 << 18)) /* Normal, Write-back, No write-alloc */
#define MPU_NORMAL_NC_TEX_C_B_NS   ((1UL << 19) | (0UL << 17) | (0UL << 16) | (0 << 18)) /* Normal, noncacheable */
#define MPU_NORMAL_WB_WA_TEX_C_B_NS   ((1UL << 19) | (1UL << 17) | (1UL << 16) | (0 << 18)) /* Normal, Write-back, Write-alloc */
#define MPU_NORMAL_WB_WA_TEX_C_B_S   ((1UL << 19) | (1UL << 17) | (1UL << 16) | (1 << 18)) /* Normal, Write-back, Write-alloc, shareable */

typedef struct {
    uint32_t region_id;
    uint32_t region_baddr;
    uint32_t region_attr;
    uint32_t region_size;
    uint32_t valid;
} MpuRegionType;

#ifndef MPU_SUPPORT_NUMBER
#define MPU_SUPPORT_NUMBER      6
#endif
//0:disable protection 1: enable protection
#define MPU_PROTECT_CODE_AREA_STA       1
static MpuRegionType codeRegions[MPU_SUPPORT_NUMBER];
//0:disable protection 1: enable protection
#define MPU_PROTECT_SRAM_AREA_STA       1
static MpuRegionType sramRegion = {
    .region_id = 0,
    .region_baddr = 0x0,
    .region_attr = MPU_AP_RW|MPU_NORMAL_WT_NOWA_TEX_C_B_S|MPU_REGION_ENABLE|MPU_NOT_EXECUTABLE,
    .region_size = MPU_SIZE_1MB,
    .valid = MPU_PROTECT_SRAM_AREA_STA,
};

//0:disable protection 1: enable protection
#define MPU_PROTECT_GUARD_AREA_STA       1
static MpuRegionType guardRegion = {
    .region_id = 0,//Will be modified after the number of code confirmed.
    .region_baddr = (uint32_t)&__guard_area_start,
    .region_attr = MPU_AP_NA|MPU_NORMAL_WT_NOWA_TEX_C_B_S|MPU_REGION_ENABLE|MPU_NOT_EXECUTABLE,
    .region_size = MPU_SIZE_32B,
    .valid = MPU_PROTECT_GUARD_AREA_STA,
};
#define CODE_REGION_SIZE    (guardRegion.region_id -1)
/*
*Respect the status of Splitting region.
*   true:success, area of code and guard will be protected.
*   false:failed, only area of guard will be protected. The area of code can be written at anytime.
*/
static bool mpuInitSta = false;
//Used to translate from usedBlock to srd bit.
static uint32_t usedBlock2SrdTable[] = {
    0x0000, 0xfe00, 0xfc00, 0xf800, 0xf000,
    0xe000, 0xc000, 0x8000, 0x0000,
};

// MPU control
#define MPU_ENABLE                    (1UL << 0)
#define MPU_ENABLE_DURING_HF_NMI      (1UL << 1)
#define MPU_ENABLE_DEFAULT_MEM_MAP    (1UL << 2)
#define MPU_CONTROL_ENABLE            MPU_ENABLE | MPU_ENABLE_DURING_HF_NMI | MPU_ENABLE_DEFAULT_MEM_MAP /* MPU enabled during hard fault */
#define MPU_CONTROL_DISABLE           0

static void mpuSetupRegion(MpuRegionType*);
static void mpuSetupRegions(void);
static void mpuEnable(void);
static void mpuDisable(void);
static bool belongToCodeRegion(uint32_t);
static enum mpuErrType \
    mpuWriteDataToCodeArea(uint32_t,uint8_t*,uint32_t);
static uint32_t getCodeRegionsAttrs(MpuRegion*);
static uint32_t getCodeBaddr(uint32_t);
static uint32_t getCodeRegionSize(MpuRegion*);
static void setCodeAtt(MpuRegionType*, MpuRegionTable*);
static void setGuardAtt(MpuRegionTable*);
static void setCodeRegionFromRegionTable(MpuRegionType*, MpuRegionTable*);
static void showMpuAllArea();



void MemoryManagemntFault_Handler(void);

void __attribute__((naked)) MemoryManagemntFault_Handler(void)
{
    asm volatile(
        "mov    r0, #3                    \n"
        "b      cpuCommonFaultCode        \n"
    );
}

static bool belongToCodeRegion(uint32_t address)
{
    return (address >= CODE_AREA_START && address <= CODE_AREA_END);
}

/**
    Note:
        1. You must disable irq before call this function.
*/
static void mpuSetupRegion(MpuRegionType* region)
{
    uint32_t attr;

    //Something is need to watching.
    MPU->RNR = 0xFF & region->region_id;

    attr = region->region_attr | region->region_size;
    // Disable region
    MPU->RASR = attr & 0xFFFFFFFE;

    // Set base address
    MPU->RBAR = 0xFFFFFFE0 & region->region_baddr;

    // Enable region
    MPU->RASR = attr | region->valid;
}

static void mpuSetupRegions(void)
{
    uint32_t i;
    mpuSetupRegion(&sramRegion);

    for(i = 0; i < CODE_REGION_SIZE; i++) {

        mpuSetupRegion(&codeRegions[i]);
    }
    mpuSetupRegion(&guardRegion);
}

static void mpuEnable(void)
{
    MPU->CTRL = MPU_CONTROL_ENABLE;
    //Ensure MPU settings take effects.
    __DSB();
    //Sequence instruction fetches using update settings.
    __ISB();
}


static void mpuDisable(void)
{
    //Make sure outstanding transfers are done.
    __DMB();

    MPU->CTRL = MPU_CONTROL_DISABLE;
}

static bool mpuIsOpen()
{
    return (MPU->CTRL&MPU_ENABLE);
}

static void mpuAllowAreaWrite(bool allowSvcWrite)
{
    if(!getMpuInitSta()){
        /*
        *false:
        *   failed, only area of guard will be protected.
        *   The area of code can be written at anytime.
        */
        MPU_LOG_PRINT("The area of code can be written at anytime.");
        return;
    }
    /**
    *allowSvcWrite:
    *   false:mpu needs open.
    *   true :mpu needs close.
    */
    if(!mpuIsOpen()&&allowSvcWrite){
        MPU_LOG_PRINT("MPU has closed!! So this operation is ignored.");
        return;
    }

    if(mpuIsOpen()&& !allowSvcWrite){
        MPU_LOG_PRINT("MPU has open!! So this operation is ignored.");
        return;
    }

    uint64_t intSta = cpuIntsOff();

    allowSvcWrite?mpuDisable():mpuEnable();

    cpuIntsRestore (intSta);
}
static uint32_t getCodeRegionsAttrs(MpuRegion* region)
{
    uint32_t attr = 0x0ul;
    attr = MPU_AP_U_RO_P_RO | MPU_NORMAL_WT_NOWA_TEX_C_B_S;
    attr |= MPU_REGION_ENABLE;//Enable.
    attr |= usedBlock2SrdTable[region->usedBlock];
    return attr;
}
static uint32_t getCodeBaddr(uint32_t curAddress)
{
    return (curAddress*1024);
}
static uint32_t getCodeRegionSize(MpuRegion* region)
{
    switch(region->regionSize){
        case SPLIT_MPU_SIZE_1KB:

            return MPU_SIZE_1KB;
        case SPLIT_MPU_SIZE_2KB:

            return MPU_SIZE_2KB;
        case SPLIT_MPU_SIZE_4KB:

            return MPU_SIZE_4KB;
        case SPLIT_MPU_SIZE_8KB:

            return MPU_SIZE_8KB;
        case SPLIT_MPU_SIZE_16KB:

            return MPU_SIZE_16KB;
        case SPLIT_MPU_SIZE_32KB:

            return MPU_SIZE_32KB;
        case SPLIT_MPU_SIZE_64KB:

            return MPU_SIZE_64KB;
        case SPLIT_MPU_SIZE_128KB:

            return MPU_SIZE_128KB;
        case SPLIT_MPU_SIZE_256KB:

            return MPU_SIZE_256KB;
        case SPLIT_MPU_SIZE_512KB:

            return MPU_SIZE_512KB;
    }
    return 0;
}
static void setCodeAtt(MpuRegionType* regions, MpuRegionTable* regionTable)
{
    if(regions == NULL || regionTable == NULL){
        return;
    }

    uint8_t i;
    uint32_t curAddress = 0x0ul;
    for(i=0; i < regionTable->position; ++i){
        MpuRegion* region = &regionTable->mpuRegions[i];
        if(region == NULL){
            MPU_ERR_PRINT("region is NULL.Next.");
            continue;
        }
        //The MPU of region[0] is used to protect the sram.
        regions[i].region_id = region->regionId + 1;
        regions[i].region_attr = getCodeRegionsAttrs(region);
        regions[i].region_baddr = getCodeBaddr(curAddress);
        regions[i].region_size = getCodeRegionSize(region);
        regions[i].valid = MPU_PROTECT_CODE_AREA_STA;

        curAddress += region->consumedSize;
    }
}
static void setGuardAtt(MpuRegionTable* regionTable)
{
    guardRegion.region_id = regionTable->position+1;
}

static void __attribute__((used)) setCodeRegionFromRegionTable(MpuRegionType* regions, MpuRegionTable* regionTable)
{

    setCodeAtt(regions, regionTable);
    setGuardAtt(regionTable);
}
static void showOneRegionType(MpuRegionType* region)
{
    MPU_LOG_PRINT("region_id:%ld, region_baddr:%lx, region_attr:%lx, region_size:%ld, valid:%ld", \
        region->region_id, region->region_baddr, region->region_attr, region->region_size, region->valid);
}
static void __attribute__((used)) showMpuAllArea()
{
    uint8_t i;
    //show sram
    showOneRegionType(&sramRegion);
    //show code
    for(i = 0;i < CODE_REGION_SIZE; ++i){
        showOneRegionType(&codeRegions[i]);
    }
    //show guard
    showOneRegionType(&guardRegion);
}
// Setup regions and start MPU
void mpuStart(void)
{
    MpuRegionTable regionTable;
    if(!splitProtectArea(CODE_AREA_START, CODE_AREA_SIZE, &regionTable))
    {
        MPU_LOG_PRINT("Current area of code isn't supported. So the area of guard is protected.");
        setMpuInitSta(false);
        regionTable.position = 0;
        sramRegion.region_attr &= ~(MPU_NOT_EXECUTABLE);
    }else{
        setMpuInitSta(true);
        showMpuRegions(&regionTable);
    }
#ifdef  MPU_HACK_CODE
    /**
    *HACK:
    *   Putting the following code at here for simulating the protected area of code can't be supported.
    */
    setMpuInitSta(false);
    regionTable.position = 0;
    sramRegion.region_attr &= ~(MPU_NOT_EXECUTABLE);
#endif
    setCodeRegionFromRegionTable(codeRegions, &regionTable);
    showMpuAllArea();

    uint64_t intSta = cpuIntsOff();
    mpuDisable();

    mpuSetupRegions();

    mpuEnable();
    cpuIntsRestore (intSta);
}

void mpuAllowRomWrite(bool allowSvcWrite)
{
    mpuAllowAreaWrite(allowSvcWrite);
}

/**
    This function must be called after the initialization of mailbox done.
*/
void mpuShow(void)
{
    int i;
    uint32_t addr, rasr;
    uint8_t ap;
    bool xn;
    char *s, *u;

    osLog(LOG_INFO, "MPU: %d HFNMIENA: %d PRIVDEFENA: %d\n",
        !!(MPU->CTRL & MPU_CTRL_ENABLE_Msk),
        !!(MPU->CTRL & MPU_CTRL_HFNMIENA_Msk),
        !!(MPU->CTRL & MPU_CTRL_PRIVDEFENA_Msk));

    for (i=0; i < CODE_REGION_SIZE+2; i++) {
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
            "%d: %c %08lx-%08lx S:%s U:%s TEX:%ld %c%c%c %02lx\n",
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

static enum mpuErrType \
    mpuWriteDataToCodeArea(uint32_t address, \
                                   uint8_t* start, \
                                   uint32_t size)
{
    MPU_DEBUG_PRINT("%08lx, %02x, %lx", address, *start, size);
    bool isVail = belongToCodeRegion(address);
    if(!isVail){
        return MPU_ERR_ERROR_ADDRESS;
    }

    if(size > 1){
        isVail = belongToCodeRegion(address+size-1);
        if(!isVail){
            return MPU_ERR_ERROR_ADDRESS;
        }
    }

    mpuAllowRomWrite(true);

    uint32_t count;
    for(count = 0; count < size; ++count){
        __raw_write8(*(start + count), address + count);
    }

    mpuAllowRomWrite(false);

    return MPU_ERR_NO_ERROR;
}

//The API used to user.So it must keep stable.
enum mpuErrType mpuWriteByteToCode(uint32_t address, uint8_t data)
{
   return mpuWriteDataToCodeArea(address, &data, 1);
}

//The API used to user.So it must keep stable.
enum mpuErrType mpuWriteWordToCode(uint32_t address, uint32_t word)
{
   return mpuWriteDataToCodeArea(address, (uint8_t*)&word, 4);
}

//The API used to user.So it must keep stable.
enum mpuErrType mpuWriteMulDataToCode(uint32_t address, uint8_t* data, uint32_t size)
{
    return mpuWriteDataToCodeArea(address, data, size);
}

void setMpuInitSta(bool newSta)
{
    mpuInitSta = newSta;
}
bool getMpuInitSta(void)
{
    return mpuInitSta;
}
void mpuAllowRamExecution(bool allowSvcExecute)
{

}
