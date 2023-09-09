#include <splitMpuRegion.h>

static void clearDetectRegionSta(DetectRegionStatus* );
static bool setDetectRegionSta(DetectRegionStatus* , enum subRegionUseStatus , uint32_t , uint32_t , uint32_t );
static bool setDetectRegionStaNoSub(DetectRegionStatus* , uint32_t );
static bool setDetectRegionStaSub(DetectRegionStatus* , uint32_t , uint32_t , uint32_t );
static void clearRegionTable(MpuRegionTable* );

static void showRegionStatus(DetectRegionStatus* regionStatus)
{
    SPLIT_MPU_DEBUG_PRINT("=======================");
    if(regionStatus == NULL){
        SPLIT_MPU_DEBUG_PRINT("regionStatus is %p.", regionStatus);
        return;
    }

    SPLIT_MPU_DEBUG_PRINT("consumedSize(%lu) regionSize(%lu) subRegionSta(%d) usedBlock(%lu).",\
                    regionStatus->consumedSize,\
                    regionStatus->regionSize,\
                    regionStatus->subRegionSta,\
                    regionStatus->usedBlock);
    SPLIT_MPU_DEBUG_PRINT("=======================");
}
static bool recordOneRegionToTable(MpuRegionTable* table, DetectRegionStatus* regionSta)
{
    if(table->position >= MPU_SUPPORT_NUMBER){
        SPLIT_MPU_DEBUG_PRINT("ERR:The MPU support number is %d.", MPU_SUPPORT_NUMBER);
        return false;
    }
    MpuRegion* regionItem = &table->mpuRegions[table->position];

    regionItem->regionId = table->position;
    regionItem->regionSize = regionSta->regionSize;
    regionItem->subRegionSta = regionSta->subRegionSta;
    regionItem->usedBlock = regionSta->usedBlock;
    regionItem->consumedSize = regionSta->consumedSize;

    table->position++;
    return true;
}

static uint32_t getCurAddress(MpuRegionTable* regionTable)
{
    uint8_t i;
    uint32_t curAddress = 0;

    for(i=0; i < regionTable->position; ++i){

        curAddress += regionTable->mpuRegions[i].consumedSize;
    }

    return curAddress;
}

static bool addressIsRegionSizeOfMultiple(uint32_t address, uint32_t regionSize)
{
    return ((address%regionSize) == 0);
}
static void clearSubRegionSize(SubRegionSizeStore* subRegionSize)
{
    if(subRegionSize == NULL){
        SPLIT_MPU_ERR_PRINT("subRegionSize is NULL.");
        return;
    }
    subRegionSize->isBytes = false;
    subRegionSize->size = 0;
}

static SubRegionSizeStore getSubRegionSize(uint32_t supportSize)
{
    SubRegionSizeStore subregionSize;
    clearSubRegionSize(&subregionSize);

    if(supportSize > SPLIT_MPU_SIZE_8KB){
        subregionSize.isBytes = false;
        subregionSize.size = supportSize/MPU_SUB_MAX_SIZE;
    }else{
        subregionSize.isBytes = true;
        subregionSize.size = (supportSize*1024)/MPU_SUB_MAX_SIZE;
    }

    return subregionSize;
}

static uint32_t getMultipleNumber(uint32_t member, SubRegionSizeStore* denominator)
{
    if(denominator == NULL){
        SPLIT_MPU_ERR_PRINT("denominator can't be NULL.");
        return 0;
    }

    return(member*(denominator->isBytes?1024:1))/denominator->size;
}

static bool subRegionEqualRestSize(uint32_t restRegionSize, SubRegionSizeStore* subregionSize, uint32_t N)
{
    if(subregionSize == NULL){
        SPLIT_MPU_ERR_PRINT("subregionSize can't be NULL.");
        return 0;
    }
    return (restRegionSize*(subregionSize->isBytes ? 1024:1) == (N*subregionSize->size));
}
static bool isSupportSize(uint32_t N)
{
    return (N >= 1 && N<= MPU_SUB_MAX_SIZE);
}
static DetectRegionStatus findSubRegionDetectRestSize(uint32_t address, uint32_t restRegionSize)
{
    DetectRegionStatus detectRegionSta;
    uint8_t i;

    clearDetectRegionSta(&detectRegionSta);

    for(i=0; i< MPU_SUPPORT_SIZE; ++i){

        if(addressIsRegionSizeOfMultiple(address, mpuSupportSizeTables[i])){

            if(mpuSupportSizeTables[i] == restRegionSize ){
                setDetectRegionStaNoSub(&detectRegionSta, mpuSupportSizeTables[i]);
                break;
            }
            //How the subRegionSize to support the mini value which is 1KB/8 = 128byte
            SubRegionSizeStore subRegionSize = getSubRegionSize(mpuSupportSizeTables[i]);
            uint32_t N = getMultipleNumber(restRegionSize, &subRegionSize);

            SPLIT_MPU_DEBUG_PRINT("i(%d):isBytes %d size %lu  N %lu restRegionSize%lu %d.", i, subRegionSize.isBytes, subRegionSize.size, N,\
                restRegionSize*(subRegionSize.isBytes?1024:1), \
                    (N >= 1 && N<= MPU_SUB_MAX_SIZE&& subRegionEqualRestSize(restRegionSize, &subRegionSize, N)));

            if(isSupportSize(N) && subRegionEqualRestSize(restRegionSize, &subRegionSize, N)){
                setDetectRegionStaSub(&detectRegionSta,N,mpuSupportSizeTables[i],\
                        (N*subRegionSize.size)/(subRegionSize.isBytes?1024:1));
                break;
            }

        }
    }
    return detectRegionSta;
}

static bool setDetectRegionSta(DetectRegionStatus* detectRegionSta, \
    enum subRegionUseStatus subRegionSta, uint32_t usedBlock, uint32_t regionSize, uint32_t consumedSize)
{
    if(detectRegionSta == NULL){
        return false;
    }
    clearDetectRegionSta(detectRegionSta);

    detectRegionSta->subRegionSta = subRegionSta;
    detectRegionSta->usedBlock = usedBlock;
    detectRegionSta->regionSize = regionSize;
    detectRegionSta->consumedSize = consumedSize;
    return true;
}

static bool setDetectRegionStaNoSub(DetectRegionStatus* detectRegionSta, uint32_t regionSize)
{
    return setDetectRegionSta(detectRegionSta, SUB_REGION_NOT_USED, 0, regionSize, regionSize);
}
static bool setDetectRegionStaSub(DetectRegionStatus* detectRegionSta, uint32_t usedBlock, uint32_t regionSize, uint32_t consumedSize)
{
    return setDetectRegionSta(detectRegionSta, SUB_REGION_USED, usedBlock, regionSize, consumedSize);
}

static DetectRegionStatus findDetectRegionByMaxIndex(uint32_t address, uint32_t restRegionSize, uint32_t maxRegionIndex)
{
    int i;
    uint8_t miniIndex = 0;
    uint32_t miniDelta = SPLIT_MPU_SIZE_MAX;
    uint32_t curDelta = 0;
    DetectRegionStatus detectRegionSta;

    SPLIT_MPU_DEBUG_PRINT("address %lu restRegionSize %lu maxRegionIndex %lu.", address, restRegionSize, maxRegionIndex);

    for (i = 0 ; i < maxRegionIndex; i++){
        SPLIT_MPU_DEBUG_PRINT("i %d:curDelta %lu miniDelta %lu miniIndex %d %d.", \
            i, curDelta, miniDelta, miniIndex, addressIsRegionSizeOfMultiple(address, mpuSupportSizeTables[i]));

        //Check size is valid.
        if(addressIsRegionSizeOfMultiple(address, mpuSupportSizeTables[i])){
            //The MPU of support table includes the restRegionSize.
            if( restRegionSize == mpuSupportSizeTables[i] ){

                setDetectRegionStaNoSub(&detectRegionSta, restRegionSize);
                return detectRegionSta;
            }
            //In this implement can't supports the delta of regionSize which greater than the restRegionSize.
            if(mpuSupportSizeTables[i] > restRegionSize){
                continue;
            }

            curDelta = restRegionSize - mpuSupportSizeTables[i];
            if(miniDelta >= curDelta){
                miniDelta = curDelta;
                miniIndex = i;
            }
        }
    }

    SPLIT_MPU_DEBUG_PRINT("address %lu restRegionSize %lu miniIndex %d %lu.", \
            address, restRegionSize, miniIndex, mpuSupportSizeTables[miniIndex]);
    setDetectRegionStaNoSub(&detectRegionSta, mpuSupportSizeTables[miniIndex]);
    return detectRegionSta;
}
static void clearDetectRegionSta(DetectRegionStatus* detectRegionSta)
{
    detectRegionSta->consumedSize = 0;
    detectRegionSta->regionSize = 0;
    detectRegionSta->subRegionSta = SUB_REGION_NOT_USED;
    detectRegionSta->usedBlock = 0;
}
static uint32_t getMaxSizeFromSupportTable(uint32_t address, uint8_t * maxRegionIndex)
{
    uint8_t i;
    //get the max region size for this address
    for (i=0; i < MPU_SUPPORT_SIZE; i++){
        if(((address%mpuSupportSizeTables[i]) == 0) && \
                (mpuSupportSizeTables[i] > mpuSupportSizeTables[*maxRegionIndex])){
            *maxRegionIndex = i;
        }
    }
    return mpuSupportSizeTables[*maxRegionIndex];;
}
static DetectRegionStatus getOneMpuRegionConfiguration(uint32_t address, uint32_t restRegionSize)
{
    uint8_t maxSupportSizeIndex = 0;
    DetectRegionStatus detectRegionSta;

    detectRegionSta = findSubRegionDetectRestSize(address, restRegionSize);
    if(0 != detectRegionSta.regionSize){
        SPLIT_MPU_DEBUG_PRINT("Find a detectRegionSta in subRegionDectect success.");
        return detectRegionSta;
    }

    uint32_t maxSupportSize = getMaxSizeFromSupportTable(address, &maxSupportSizeIndex);

    SPLIT_MPU_DEBUG_PRINT("%d %lu.", maxSupportSizeIndex, maxSupportSize);
    if(maxSupportSize > restRegionSize){
        detectRegionSta = findDetectRegionByMaxIndex(address, restRegionSize, maxSupportSizeIndex);
    }else{
        setDetectRegionStaNoSub(&detectRegionSta,mpuSupportSizeTables[maxSupportSizeIndex]);
    }
    showRegionStatus(&detectRegionSta);
    return detectRegionSta;
}

static void showOneRegion(MpuRegion* mpuRegion)
{
    if(mpuRegion == NULL){
        SPLIT_MPU_LOG_PRINT("mpuRegion(%p) can't be NULL.", mpuRegion);
        return;
    }
    if(mpuRegion->subRegionSta == SUB_REGION_USED){
        SPLIT_MPU_LOG_PRINT("regionId:%lu regionSize:%lu subRegionSta:%d usedBlock:%lu consumedSize:%lu.",\
            mpuRegion->regionId,\
            mpuRegion->regionSize,\
            mpuRegion->subRegionSta,\
            mpuRegion->usedBlock,\
            mpuRegion->consumedSize);
    }else if(mpuRegion->subRegionSta == SUB_REGION_NOT_USED) {
        SPLIT_MPU_LOG_PRINT("regionId:%lu regionSize:%lu subRegionSta:%d usedBlock:%lu.",\
            mpuRegion->regionId,\
            mpuRegion->regionSize,\
            mpuRegion->subRegionSta,\
            mpuRegion->usedBlock);
    }
}

#if defined(MPU_REGION_VAIL_CHECK)
static bool isBelongToSupportTable(MpuRegion* region)
{
    bool ret = false;
    uint8_t i;
    for(i=0; i< MPU_SUPPORT_SIZE; ++i){

        if(region->regionSize == mpuSupportSizeTables[i]){
            ret = true;
            break;
        }
    }
    return ret;
}

static uint32_t getMpuTableSize(MpuRegionTable* regionTable)
{
    if(regionTable == NULL){
        SPLIT_MPU_ERR_PRINT("regionTable is NULL.");
        return 0;
    }
    uint8_t i;
    uint32_t regionTableSize = 0;
    for(i=0; i< regionTable->position; ++i){
        MpuRegion * region = &regionTable->mpuRegions[i];
        if(region ==  NULL){
            SPLIT_MPU_ERR_PRINT("region[%d] is NULL.", i);
            continue;
        }
        //Check the regionSize belong to mpuSupportSizeTables or not.
        if(! isBelongToSupportTable(region)){
            SPLIT_MPU_ERR_PRINT("This regionSize(%lu) doesn't belong to the table of MPU support.", region->regionSize);
            break;
        }
        regionTableSize += region->consumedSize;
    }
    return regionTableSize;
}

static bool checkOneRegion(uint32_t curAddress, MpuRegion* mpuRegion)
{
    SPLIT_MPU_DEBUG_PRINT("%d, %d", curAddress, mpuRegion->regionSize);
    //Check the address equal the region size or not.
    if(!addressIsRegionSizeOfMultiple(curAddress, mpuRegion->regionSize)){
        SPLIT_MPU_ERR_PRINT("The address doesn't multiple the size of this region.");
        return false;
    }
    return true;
}

/**
*The function used to check the validity of this MpuRegionTable which calculated with the size of the area(protectedSize).
*/
static bool checkMpuRegionsIsValid(MpuRegionTable* regionTable, uint32_t protectedSize)
{
    uint8_t i;
    if(regionTable == NULL){
        SPLIT_MPU_LOG_PRINT("regionTable(%p).", regionTable);
        return false;
    }
    MpuRegion *mpuRegion = NULL;
    uint32_t curAddress = 0;
    uint32_t mpuTableSize = getMpuTableSize(regionTable);
    if(mpuTableSize != protectedSize){
        SPLIT_MPU_LOG_PRINT("The mpuTableSize(%lu) doesn't detects protectedSize(%lu).", mpuTableSize, protectedSize);
        return false;
    }
    SPLIT_MPU_LOG_PRINT("position(%u).", regionTable->position);

    for(i = 0; i < regionTable->position; ++i){
        mpuRegion = &regionTable->mpuRegions[i];
        if(!checkOneRegion(curAddress,mpuRegion)){
            SPLIT_MPU_LOG_PRINT("protectedSize %lu:The checking of region[%d] is failed.", protectedSize, i);
            return false;
        }
        curAddress += mpuRegion->consumedSize;
    }
    SPLIT_MPU_LOG_PRINT("protectedSize %lu:Check is valid. success!!", protectedSize);
    return true;
}
#endif//MPU_REGION_VAIL_CHECK
void  __attribute__((used))  showMpuRegions(MpuRegionTable* regionTable)
{
    uint8_t i;
    if(regionTable == NULL){
        SPLIT_MPU_ERR_PRINT("regionTable(%p).", regionTable);
        return;
    }
    MpuRegion *mpuRegion = NULL;

    SPLIT_MPU_LOG_PRINT("position(%u).", regionTable->position);

    for(i = 0; i < regionTable->position; ++i){
        mpuRegion = &regionTable->mpuRegions[i];
        showOneRegion(mpuRegion);
    }
}
bool __attribute__((used)) splitProtectArea(uint32_t address_start, uint32_t regionSize/*uint:KB*/, MpuRegionTable* storeTable)
{
    if(storeTable == NULL){
        SPLIT_MPU_ERR_PRINT("The storeTable can't is NULL.");
        return false;
    }
    uint8_t i;
    uint32_t restRegionSize = regionSize;

    clearRegionTable(storeTable);

    for (i=0; i < MPU_SUPPORT_NUMBER; i++){

        uint32_t curAddress = getCurAddress(storeTable);
        SPLIT_MPU_DEBUG_PRINT("%lu %lu", curAddress, restRegionSize);
        DetectRegionStatus regionSta = getOneMpuRegionConfiguration(curAddress, restRegionSize);
        recordOneRegionToTable(storeTable, &regionSta);
        restRegionSize -= regionSta.consumedSize;

        if(restRegionSize == 0){
            SPLIT_MPU_LOG_PRINT("The operation of splitting is success!!");
            break;
        }

    }

    if(restRegionSize != 0){
        SPLIT_MPU_LOG_PRINT("The operation of splitting is failed! Its start address and size are (%08lx,%lu).",\
            address_start, regionSize);
        return false;
    }

    return true;
}

static void clearRegionTable(MpuRegionTable* table )
{
    uint8_t i;
    if(table == NULL){
        SPLIT_MPU_ERR_PRINT("The table can't is NULL.");
        return;
    }
    table->position = 0;
    for(i=0; i < MPU_SUPPORT_NUMBER; ++i){
        MpuRegion* region = &table->mpuRegions[i];
        region->regionId = 0;
        region->regionSize = 0;
        region->subRegionSta = SUB_REGION_NOT_USED;
        region->usedBlock  = 0;
    }
}

#ifdef ENABLE_MPU_TEST
void recordFailedItemInfo(TestResult* result, uint32_t protectedAreaSize)
{
    if(result == NULL){
        SPLIT_MPU_ERR_PRINT("result is null");
        return;
    }
    if(result->failedItemCount > (FAILED_PROTECT_ITEM_MAX_SIZE - 1)){
        SPLIT_MPU_ERR_PRINT("failedProtectItemList hadn't enough space to store the item of protectedAreaSize.");
        return;
    }
    SPLIT_MPU_LOG_PRINT("result->failedItemCount(%lu), protectedAreaSize(%lu)", result->failedItemCount, protectedAreaSize);
    result->failedProtectItemList[result->failedItemCount] = protectedAreaSize;
    result->failedItemCount++;
}

void showTestReport(TestResult* result)
{
    SPLIT_MPU_LOG_PRINT("=============START===========");
    if(result == NULL){
        SPLIT_MPU_ERR_PRINT("result is null");
        return;
    }
    SPLIT_MPU_LOG_PRINT("testItemCount(%lu) susscessItemCount(%lu) failedItemCount(%lu)", \
        result->testItemCount, result->susscessItemCount, result->failedItemCount);
    uint32_t i;
    for(i = 0; i< result->failedItemCount; ++i){

        SPLIT_MPU_LOG_PRINT("fialed area size:%lu", \
                result->failedProtectItemList[i]);

    }
    SPLIT_MPU_LOG_PRINT("============END============");
}

void clearTestResult(TestResult* testResult)
{
    if(testResult == NULL){
        SPLIT_MPU_ERR_PRINT("result is null");
        return;
    }

    testResult->testItemCount = 0;
    testResult->susscessItemCount= 0;
    testResult->failedItemCount = 0;
    testResult->testItemCount = 0;
    memset(testResult->failedProtectItemList, 0, FAILED_PROTECT_ITEM_MAX_SIZE);
}
#endif//ENABLE_MPU_TEST
