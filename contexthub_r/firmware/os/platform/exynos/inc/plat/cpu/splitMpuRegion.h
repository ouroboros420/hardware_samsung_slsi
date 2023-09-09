#ifndef __SPLITMPUREGION_H
#define __SPLITMPUREGION_H

#include <stdio.h>
#include <stdlib.h>
#include <seos.h>

//#define SPLIT_MPU_DEBUG
#define SPLIT_MPU_SHOW_LOG

#ifdef SPLIT_MPU_DEBUG
#define SPLIT_MPU_DEBUG_PRINT(fmt, x...) \
do{\
	osLog(LOG_DEBUG, "(%s,%d):"fmt"\n",__func__,__LINE__,##x);\
}while(0)
#else
#define SPLIT_MPU_DEBUG_PRINT(fmt, x...)
#endif

#ifdef SPLIT_MPU_SHOW_LOG
#define SPLIT_MPU_LOG_PRINT(fmt, x...) \
do{\
	osLog(LOG_INFO, "(%s,%d):"fmt"\n",__func__,__LINE__,##x);\
}while(0)
#else
#define SPLIT_MPU_LOG_PRINT(fmt, x...)
#endif

#define SPLIT_MPU_ERR_PRINT(fmt, x...) \
do{\
	osLog(LOG_ERROR, "(%s,%d):"fmt"\n",__func__,__LINE__,##x);\
}while(0)

#ifdef ENABLE_MPU_TEST
#define TEST_PERPOSE "This is a test for spliting region of MPU in CORTEX-M4F"
#define START_INDEX     32
#define END_INDEX       1024
typedef struct {
    uint32_t testItemCount;
    uint32_t susscessItemCount;
    uint32_t failedItemCount;
#define FAILED_PROTECT_ITEM_MAX_SIZE    50
    uint32_t failedProtectItemList[FAILED_PROTECT_ITEM_MAX_SIZE];
}TestResult;
#endif

enum subRegionUseStatus{
    SUB_REGION_NOT_USED = 0,
    SUB_REGION_USED = 0x1,
};

typedef struct{
    enum subRegionUseStatus subRegionSta;
    //This param is used in this case which shubRegionSta = SUB_REGION_USED.
    uint32_t usedBlock;
    uint32_t regionSize;
    uint32_t consumedSize;
}DetectRegionStatus;

typedef struct{
    /*
    *   true: the uint of size is byte.
    *   false: the uint of size is KB.
    */
    bool isBytes;
    uint32_t size;
}SubRegionSizeStore;

typedef struct {
    enum subRegionUseStatus subRegionSta;
    uint32_t usedBlock;
    uint32_t regionId;
    uint32_t regionSize;
    uint32_t consumedSize;
}MpuRegion;

//This size refer to the hardwared of MPU.
#ifndef MPU_SUPPORT_NUMBER
#define MPU_SUPPORT_NUMBER      6
#endif
typedef struct{
    uint8_t position;
    MpuRegion mpuRegions[MPU_SUPPORT_NUMBER];
}MpuRegionTable;

//The table of MPU support is used in mpuSupportSizeTables.
#define SPLIT_MPU_SIZE_1KB        (0x1ul << 0)
#define SPLIT_MPU_SIZE_2KB        (SPLIT_MPU_SIZE_1KB << 1)
#define SPLIT_MPU_SIZE_4KB        (SPLIT_MPU_SIZE_1KB << 2)
#define SPLIT_MPU_SIZE_8KB        (SPLIT_MPU_SIZE_1KB << 3)
#define SPLIT_MPU_SIZE_16KB       (SPLIT_MPU_SIZE_1KB << 4)
#define SPLIT_MPU_SIZE_32KB       (SPLIT_MPU_SIZE_1KB << 5)
#define SPLIT_MPU_SIZE_64KB       (SPLIT_MPU_SIZE_1KB << 6)
#define SPLIT_MPU_SIZE_128KB      (SPLIT_MPU_SIZE_1KB << 7)
#define SPLIT_MPU_SIZE_256KB      (SPLIT_MPU_SIZE_1KB << 8)
#define SPLIT_MPU_SIZE_512KB      (SPLIT_MPU_SIZE_1KB << 9)
#define SPLIT_MPU_SIZE_MAX        (SPLIT_MPU_SIZE_1KB << 10)

static uint32_t __attribute__((used)) mpuSupportSizeTables[] = {
    SPLIT_MPU_SIZE_1KB,
    SPLIT_MPU_SIZE_2KB,
    SPLIT_MPU_SIZE_4KB,
    SPLIT_MPU_SIZE_8KB,
    SPLIT_MPU_SIZE_16KB,
    SPLIT_MPU_SIZE_32KB,
    SPLIT_MPU_SIZE_64KB,
    SPLIT_MPU_SIZE_128KB,
    SPLIT_MPU_SIZE_256KB,
    SPLIT_MPU_SIZE_512KB,
    SPLIT_MPU_SIZE_MAX
};
#define MPU_SUPPORT_SIZE    (sizeof(mpuSupportSizeTables)/sizeof(mpuSupportSizeTables[0]))
#define MPU_SUB_MAX_SIZE 8

//Public API
void  __attribute__((used))  showMpuRegions(MpuRegionTable* regionTable);
bool  __attribute__((used))  splitProtectArea(uint32_t, uint32_t/*uint:KB*/, MpuRegionTable*);

#ifdef ENABLE_MPU_TEST
void recordFailedItemInfo(TestResult*, uint32_t);
void showTestReport(TestResult*);
void clearTestResult(TestResult*);
#endif//ENABLE_MPU_TEST

#endif /* __SPLITMPUREGION_H */
