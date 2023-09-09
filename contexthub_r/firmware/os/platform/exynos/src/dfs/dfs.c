#include <plat/dfs/dfs.h>
#include <halIntf.h>
#include <csp_common.h>
#include <cmu.h>
#include <rtc.h>
#include <timer.h>
#include <cpu.h>
#include <plat/mailbox/ipc_chub.h>
#include <hostIntf.h>
#if defined(SEOS)
#include <errno.h>
#include <cmsis.h>
#endif
#include <heap.h>
#include <sensors.h>
#include <debug.h>
#include <platform.h>

#define DFS_POWER_60        207
#define DFS_POWER_90        344
#define DFS_POWER_120       480
#define DFS_POWER_180       754
#define DFS_POWER_360       1574

#define DFS_DEBUG           0

#define DFS_FLAG_POWER      (1 << 0)    // Have power information
#define DFS_FLAG_DEFUALT    (1 << 1)    // Use default sampling time
#define DFS_FLAG_LOCK       (1 << 2)    // Have freq lock

#define DFS_THRESHOLD       7 / 10

#if DFS_DEBUG
#define DEBUG_MAX           30

struct DfsDebugInfo {
    uint64_t rtcTime;
    uint64_t activeSensor[2];
    uint32_t accel;
    uint32_t gyro;
    uint32_t mag;
    uint32_t baro;
    uint32_t als;
    uint32_t target;
    uint32_t freqLock;
};
static struct DfsDebugInfo mDfsDebugInfo[DEBUG_MAX];

static struct DfsDebug {
    uint64_t freqTime[DFS_FREQ_MAX];
    struct DfsDebugInfo *info;
    uint8_t idx;
} mDfsDebug;

static uint32_t mDfsAppliedFrequency;
static uint32_t mDfsAppliedTime;
#endif

struct DfsSensor {
    uint32_t rate;
    uint32_t flag;
    uint8_t sensorType;
    struct sampleTimeTable *sample;
    uint8_t freqLock;
    uint8_t updated;
};

// init
static uint8_t mDfsSensorList[SENS_TYPE_LAST_USER];
static struct DfsSensor *mDfsSensorTable;
static struct sampleTimeTable *mDfsActivePerSample;

static uint8_t mDfsFreqLock[DFS_FREQ_MAX];
static uint8_t mDfsNumSensors;

static uint32_t mDfsTargetFrequency;
static uint32_t mDfsMaxFrequencyLock;
uint32_t mDfsCurrFreq;
static uint32_t mDfsGovernor;
static uint32_t mDfsUpdated;
static uint32_t mDfsHandle;

static uint32_t mDfsPower[DFS_FREQ_MAX] = {
    DFS_POWER_360,
    DFS_POWER_180,
    DFS_POWER_120,
    DFS_POWER_90,
    DFS_POWER_60,
};

static uint32_t mDfsFrequency[DFS_FREQ_MAX] = {
    360000000,
    180000000,
    120000000,
    90000000,
    60000000,
};

/*
static uint32_t dfsActivePerSampleUs[][DFS_FREQ_MAX + 1] = {
    // ACCEL
    { SENS_TYPE_ACCEL, 321, 446, 597, 706, 964 },
    // GYRO
    { SENS_TYPE_GYRO, 321, 446, 597, 706, 964 },
    // MAG
    { SENS_TYPE_MAG, 285, 454, 510, 644, 858 },
    // BARO
    { SENS_TYPE_BARO, 237, 378, 515, 650, 859 },
    // ALS
    { SENS_TYPE_ALS, 234, 374, 513, 640, 850 },
};

*/
static uint32_t dfsDefaultSampleUs[DFS_FREQ_MAX + 1] =
    { SENS_TYPE_INVALID, 220, 280, 375, 454, 634 };

#define HZ_TO_MHZ (1000000)
static void dfsIdleMonitorCb(void *data, u64 * idleUs, u64 * totalUs);

#if DFS_DEBUG
void dfsDebugPrint(void)
{
    int i;

    osLog(LOG_DEBUG, "dfsTime      : %llu\n", mDfsDebug.info->rtcTime);
    osLog(LOG_DEBUG, "actvieSensor : 0x%llx\n", mDfsDebug.info->activeSensor[0]);
    osLog(LOG_DEBUG, "actvieSensor : 0x%llx\n", mDfsDebug.info->activeSensor[1]);
    osLog(LOG_DEBUG, "dfsAccelRate : %luHz\n", mDfsDebug.info->accel);
    osLog(LOG_DEBUG, "dfsGyroRate  : %luHz\n", mDfsDebug.info->gyro);
    osLog(LOG_DEBUG, "dfsMagRate   : %luHz\n", mDfsDebug.info->mag);
    osLog(LOG_DEBUG, "dfsBaroRate  : %luHz\n", mDfsDebug.info->baro);
    osLog(LOG_DEBUG, "dfsAlsRate   : %luHz\n", mDfsDebug.info->als);
    osLog(LOG_DEBUG, "dfsTarget    : %luHz\n",
          mDfsFrequency[mDfsDebug.info->target] / HZ_TO_MHZ);
    osLog(LOG_DEBUG, "dfsFreqLock  : %luHz\n",
          mDfsFrequency[mDfsDebug.info->freqLock] / HZ_TO_MHZ);

    for (i = 0; i < DFS_FREQ_MAX - 1; i++) {
        osLog(LOG_DEBUG, "dfsFreq %3dMHz Time : %llu\n", 360 / (i + 1),
              mDfsDebug.freqTime[i]);
    }
    osLog(LOG_DEBUG, "dfsFreq  60MHz Time : %llu\n", mDfsDebug.freqTime[i]);
}
#else
void dfsDebugPrint(void) {}
#endif

#if DFS_DEBUG
static void dfsDebugSaveInfo(void)
{
    int i;

    //idx++
    mDfsDebug.idx = (mDfsDebug.idx + 1) % DEBUG_MAX;
    mDfsDebug.info = mDfsDebugInfo + mDfsDebug.idx;

    //init
    memset(mDfsDebug.info, 0x00, sizeof(struct DfsDebugInfo));

    //save rtc time
    mDfsDebug.info->rtcTime = getTimeStampUS();
    mDfsDebug.freqTime[mDfsAppliedFrequency] +=
        mDfsDebug.info->rtcTime - mDfsAppliedTime;
    mDfsAppliedTime = mDfsDebug.info->rtcTime;

    //save active sensorlist
    for (i = 0; i < mDfsNumSensors; i++) {
        if (mDfsSensorTable[i].rate) {
            if (mDfsSensorTable[i].sensorType < 64)
                mDfsDebug.info->activeSensor[0] |=
                    (1 << mDfsSensorTable[i].sensorType);
            else
                mDfsDebug.info->activeSensor[1] |=
                    (1 << mDfsSensorTable[i].sensorType);
        }
    }

    // save physical sensor information
    mDfsDebug.info->accel =
        mDfsSensorTable[mDfsSensorList[SENS_TYPE_ACCEL - 1]].rate;
    mDfsDebug.info->gyro =
        mDfsSensorTable[mDfsSensorList[SENS_TYPE_GYRO - 1]].rate;
    mDfsDebug.info->mag =
        mDfsSensorTable[mDfsSensorList[SENS_TYPE_MAG - 1]].rate;
    mDfsDebug.info->baro =
        mDfsSensorTable[mDfsSensorList[SENS_TYPE_BARO - 1]].rate;
    mDfsDebug.info->als =
        mDfsSensorTable[mDfsSensorList[SENS_TYPE_ALS - 1]].rate;

    // save target and minlock
    mDfsDebug.info->target = mDfsTargetFrequency;
    mDfsDebug.info->freqLock = mDfsMaxFrequencyLock;

}
#endif

static void sendDfsEvent(IN DfsToHalRequset req)
{
    struct HostIntfDataBuffer *data =
        heapAlloc(sizeof(struct HostIntfDataBuffer));

    osLog(LOG_INFO, "sendDfsEvent %d\n", req);

    if (!data) {
        osLog(LOG_WARN, "Couldn't alloc test result packet\n");
        return;
    }

    data->sensType = SENS_TYPE_INVALID;
    data->length = sizeof(uint32_t);
    data->dataType = HOSTINTF_DATA_TYPE_DFS_TO_SENSOR_HAL;
    data->interrupt = NANOHUB_INT_WAKEUP;

    DfsMsg *pData = (DfsMsg *) data;
    pData->payload.request = (uint32_t) req;

    if (!osEnqueueEvtOrFree(EVT_DFS_TO_SENSOR_HAL, data, heapFree))
        osLog(LOG_WARN, "Couldn't send test result packet\n");
}

static void sendLoadMsgToHal(uint32_t timerId, void *data)
{
    sendDfsEvent(DFS_REQUEST_LOAD);
}

static void dfsGetMaxTime(IN DfsFreq freq, uint32_t * Time, uint32_t * Power)
{
    int i;
    uint32_t maxTime;
    uint32_t estPower;

    maxTime = 0;
    estPower = 0;
    for (i = 0; i < mDfsNumSensors; i++) {
        if (mDfsSensorTable[i].rate) {
            //skip gyro if accel is already on.
            //if (i == mDfsSensorList[SENS_TYPE_GYRO - 1] &&
            //    mDfsSensorTable[mDfsSensorList[SENS_TYPE_ACCEL - 1]].rate)
            //    continue;
            maxTime += mDfsSensorTable[i].sample->sampleTime[freq];
            estPower +=
                mDfsSensorTable[i].sample->sampleTime[freq] *
                mDfsSensorTable[i].rate;
        }
    }

    estPower *= mDfsPower[freq];

    *Time = maxTime;
    *Power = estPower;
}

static uint32_t dfsGetTargetSimple(void)
{
    int i, cnt;
    uint32_t target;

    cnt = 0;
    for (i = 0; i < mDfsNumSensors; i++) {
        if (mDfsSensorTable[i].rate)
            cnt++;
    }

    switch (cnt) {
    case 0:
        target = DFS_FREQ_90MHz;
        break;
    case 1:
        target = DFS_FREQ_120MHz;
        break;
    case 2:
        target = DFS_FREQ_180MHz;
        break;
    default:
        target = DFS_FREQ_360MHz;
        break;
    }

    return target;
}

static uint32_t dfsGetTargetPower(void)
{
    int i;
    uint32_t target, maxRate, minDelay, maxTime;
    uint32_t tmpPower, minPower = 0xFFFFFFFF;

    //get maxRate
    maxRate = 0;
    for (i = 0; i < mDfsNumSensors; i++) {
        if (maxRate < mDfsSensorTable[i].rate)
            maxRate = mDfsSensorTable[i].rate;
    }

    //If all sensor is disabled, target is the lowest.
    if (maxRate == 0)
        return DFS_FREQ_60MHz;

    minDelay = 1000000 / maxRate;

    target = DFS_FREQ_360MHz;
    for (i = 0; i < DFS_FREQ_MAX; i++) {
        //check frequency threshold.
        dfsGetMaxTime(i, &maxTime, &tmpPower);
        if (maxTime > minDelay * DFS_THRESHOLD)
            break;

        if (tmpPower < minPower) {
            minPower = tmpPower;
            target = i;
        }
    }
    osLog(LOG_DEBUG, "[ %s: freq:%lu: %lu < %lu(%lu) ]\n",
          __func__, target, maxTime, minDelay * DFS_THRESHOLD, minDelay);

    return target;
}

static uint32_t(*mDfsGetTargetFreq[DFS_GOVERNOR_MAX]) (void) = {
NULL, dfsGetTargetSimple, dfsGetTargetPower};

static void dfsUpdateFreq(void)
{
    uint32_t freq;

    // compare lock and target
    freq =
        mDfsTargetFrequency >
        mDfsMaxFrequencyLock ? mDfsMaxFrequencyLock : mDfsTargetFrequency;

#if DFS_DEBUG
    dfsDebugSaveInfo();
    mDfsAppliedFrequency = freq;
    dfsDebugPrint();
#endif

    // apply freq
    mDfsCurrFreq = freq;
    //cmuSetCpuClk(mDfsFrequency[freq]);
    if(!cmuSetDivider(CMU_CLK_CHUB_BUS, freq + 1))
        osLog(LOG_ERROR, "Fail to update dfs freq\n");
}

static void dfsUpdateLock(void)
{
    int i;
    uint32_t maxLock;

    //get max lock
    maxLock = DFS_FREQ_60MHz;
    for (i = 0; i < DFS_FREQ_MAX; i++) {
        if (mDfsFreqLock[i]) {
            maxLock = i;
            break;
        }
    }

    osLog(LOG_INFO, "oldLock %luHz newLock %luHz\n",
          mDfsFrequency[mDfsMaxFrequencyLock], mDfsFrequency[maxLock]);
    if (mDfsMaxFrequencyLock == maxLock) {
#if DFS_DEBUG
        dfsDebugSaveInfo();
#endif
        return;
    }

    mDfsMaxFrequencyLock = maxLock;

    //update freq
    dfsUpdateFreq();
}

static void dfsUpdateSensor(void)
{
    uint32_t target;

    //get target freq (governor)
    target = mDfsGetTargetFreq[mDfsGovernor] ();

    osLog(LOG_INFO, "oldTarget %luHz newTarget %luHz\n",
          mDfsFrequency[mDfsCurrFreq] / HZ_TO_MHZ,
          mDfsFrequency[target] / HZ_TO_MHZ);
    if (target == mDfsCurrFreq) {
#if DFS_DEBUG
        dfsDebugSaveInfo();
#endif
        return;
    }

    mDfsTargetFrequency = target;

    //update freq
    dfsUpdateFreq();
}

static void dfsCalSampleTime(uint32_t timerId, void *data)
{
    struct DfsSensor *sensor = (struct DfsSensor *)data;
    int i;

    osLog(LOG_INFO, "> dfsCalSampleTime type %u, cpuFreq:%d\n",
          sensor->sensorType, (int)mDfsCurrFreq);
    if ( mDfsCurrFreq ==  DFS_FREQ_360MHz) {
        if (mDfsHandle == 0) {
            for (i = 0; sensorFind(sensor->sensorType, i, &mDfsHandle) != NULL; i++) {
                if (sensorRequest(0, mDfsHandle, SENSOR_HZ(5), 0))
                    break;
            }
        } else {
            osLog(LOG_ERROR, "dfsCalSampleTime: other sensor is running!\n");
        }
    }
    idleMonitorEnable(1, 1000, dfsIdleMonitorCb, sensor);
}

#define DFS_INVAL_TABLE_INDEX (0xff)
void dfsInit(uint8_t numSensors, uint8_t * sensorList)
{
    int i;

#if DFS_DEBUG
    memset(&mDfsDebug, 0x00, sizeof(struct DfsDebug));
    memset(mDfsDebugInfo, 0x00, sizeof(struct DfsDebugInfo) * DEBUG_MAX);
    mDfsDebug.info = mDfsDebugInfo + mDfsDebug.idx;

    mDfsAppliedFrequency = DFS_FREQ_360MHz;
    mDfsAppliedTime = getTimeStampUS();
    /*
       debug = 0;
       memset(mDfsDebugInfo, 0, sizeof(struct DfsDebugInfo)*DEBUG_MAX);
       memset(, 0, sizeof(struct DfsDebugInfo)*DEBUG_MAX);
     */
#endif

    mDfsHandle = 0;
    mDfsNumSensors = numSensors;
    ipc_set_dfs_numSensor(mDfsNumSensors);
    mDfsActivePerSample = ipc_get_dfs_sampleTime();
    CSP_PRINTF_INFO("%s: dfsTable addr:%p, tableNum:%d\n",
                    __func__, (void *)mDfsActivePerSample, mDfsNumSensors);
    mDfsSensorTable = heapAlloc(numSensors * sizeof(struct DfsSensor));
    if (!mDfsSensorTable)
        osLog(LOG_ERROR, "%s: failed to allocate dfs sensor table!\n",
              __func__);
    memset(mDfsSensorTable, 0x00, numSensors * sizeof(struct DfsSensor));

    //mDfsSensorList initialize
    for (i = SENS_TYPE_INVALID; i < SENS_TYPE_LAST_USER; i++) {
        mDfsSensorList[i] = MAX_REGISTERED_SENSORS;
    }
    memcpy(mDfsSensorList, sensorList, SENS_TYPE_LAST_USER);

    for (i = SENS_TYPE_INVALID; i < SENS_TYPE_LAST_USER; i++) {
        if (mDfsSensorList[i] == MAX_REGISTERED_SENSORS)
            continue;

        mDfsSensorTable[mDfsSensorList[i]].sensorType = i + 1;
        osLog(LOG_INFO, "table[%d] senstype %d\n", mDfsSensorList[i], i + 1);

        if (mDfsSensorList[i] == mDfsNumSensors - 1)
            break;
    }

    memcpy(mDfsActivePerSample, dfsDefaultSampleUs, sizeof(dfsDefaultSampleUs));

    // link sample time from ipc area
    for (i = 0; i < mDfsNumSensors; i++) {      /* 0 is SENS_TYPE_INVALID */
        mDfsSensorTable[i].sample = ipc_get_dfs_sampleTime() + (i + 1);
        mDfsSensorTable[i].sample->sensType = mDfsSensorTable[i].sensorType;
        mDfsSensorTable[i].flag |= DFS_FLAG_POWER;

        CSP_PRINTF_INFO
            ("%s: sensT:%lu,%lu, sampT:%lu,%lu,%lu,%lu,%lu\n",
             __func__, (uint32_t) mDfsSensorTable[i].sensorType,
             mDfsActivePerSample[i].sensType,
             mDfsActivePerSample[i].sampleTime[DFS_FREQ_360MHz],
             mDfsActivePerSample[i].sampleTime[DFS_FREQ_180MHz],
             mDfsActivePerSample[i].sampleTime[DFS_FREQ_120MHz],
             mDfsActivePerSample[i].sampleTime[DFS_FREQ_90MHz],
             mDfsActivePerSample[i].sampleTime[DFS_FREQ_60MHz]);
    }

    //initialize lock
    for (i = 0; i < DFS_FREQ_MAX; i++)
        mDfsFreqLock[i] = 0;

    mDfsTargetFrequency = DFS_FREQ_360MHz;
    mDfsMaxFrequencyLock = DFS_FREQ_60MHz;
    mDfsGovernor = ipc_get_dfs_gov();
    mDfsCurrFreq = DFS_FREQ_360MHz;

    if (mDfsGovernor == DFS_GOVERNOR_MAX) {
        CSP_PRINTF_INFO("No DFS file saved\n");
#if defined(DFS_BOOTING)
        mDfsUpdated = 1;
        timTimerSet(1000000000, 0, 50, dfsCalSampleTime, mDfsSensorTable, true);
        CSP_PRINTF_INFO("timTimerSet %u\n", mDfsSensorTable->sensorType);
#else
        mDfsUpdated = 0;
        ipc_set_dfs_gov(DFS_GOVERNOR_OFF);
#endif
    }
    else {
        mDfsUpdated = 1;
        timTimerSet(1000000000, 0, 50, sendLoadMsgToHal, mDfsSensorTable, true);
        CSP_PRINTF_INFO("timTimerSet %u\n", mDfsSensorTable->sensorType);
    }

}

void dfsSetFreqLock(uint8_t sensorType, IN DfsFreq freq)
{
    uint32_t sensor = mDfsSensorList[sensorType - 1];

    // check already have lock
    if (mDfsSensorTable[sensor].flag & DFS_FLAG_LOCK) {
        dfsClearFreqLock(sensorType);
    }
    //save freq lock info
    mDfsSensorTable[sensor].freqLock = freq;
    mDfsFreqLock[freq]++;

    mDfsSensorTable[sensor].flag |= DFS_FLAG_LOCK;

    if (mDfsGovernor == DFS_GOVERNOR_OFF)
        return;

    //update Freq
    dfsUpdateLock();
}

void dfsClearFreqLock(uint8_t sensorType)
{
    uint32_t freq;
    uint32_t sensor = mDfsSensorList[sensorType - 1];

    if (!(mDfsSensorTable[sensor].flag & DFS_FLAG_LOCK))
        return;

    freq = mDfsSensorTable[sensor].freqLock;
    //remove freq lock info
    if (mDfsFreqLock[freq])
        mDfsFreqLock[freq]--;

    mDfsSensorTable[sensor].flag &= ~DFS_FLAG_LOCK;

    if (mDfsGovernor == DFS_GOVERNOR_OFF)
        return;

    //update Freq
    dfsUpdateLock();
}

#define SAMPLE_TIME_UPDATE_TRESHOLD (20)
#define DFS_MAX_IDLE_CB (1)

static void dfsScanNext(struct DfsSensor *dfs)
{
    uint32_t handle;
    uint64_t state;
    int i,j;

    CSP_PRINTF_INFO("%s(%d), cpuFreq:%d\n", __func__, (u32) dfs->sensorType,
                    (u32) mDfsCurrFreq);

    idleMonitorEnable(0, 1000, NULL, NULL);
    mDfsCurrFreq++;

    if (mDfsCurrFreq == DFS_FREQ_MAX) {
        sensorFind(dfs->sensorType, 0, &handle);
        state = cpuIntsOn();
        sensorRelease(0, mDfsHandle);
        mDfsHandle = 0;
        mDfsCurrFreq = DFS_FREQ_360MHz;
        /*
         * calculate all sensors
         */
        //if (mDfsSensorList[dfs->sensorType - 1] + 1 == mDfsNumSensors) {
        /*
         * calculate physical sensors only
         */
        if (dfs->sensorType == SENS_TYPE_PROX) {
            //done
            CSP_PRINTF_INFO("===> %s(%lu) is done: dfsGov:%d\n", __func__,
                            (uint32_t) dfs->sensorType,
                            (u32) ipc_get_dfs_gov());
            for (i = SENS_TYPE_INVALID; i < mDfsNumSensors; i++) {
                if(mDfsSensorTable[i].sample->sampleTime[0] == 0) {
                    for(j = 0; j < DFS_FREQ_MAX; j++)
                        mDfsSensorTable[i].sample->sampleTime[j] = dfsDefaultSampleUs[j + 1];
                }
            }
            ipc_set_dfs_gov(DFS_GOVERNOR_POWER);
            mDfsGovernor = ipc_get_dfs_gov();
            sendDfsEvent(DFS_REQUEST_SAVE);
            return;
        }
        //cmuSetCpuClk(mDfsFrequency[mDfsCurrFreq]);
        cmuSetDivider(CMU_CLK_CHUB_BUS, mDfsCurrFreq + 1);
        cpuIntsRestore(state);
        dfsCalSampleTime(0, dfs + 1);
    } else {
        dfs->updated = 0;
        //cmuSetCpuClk(mDfsFrequency[mDfsCurrFreq]);
        cmuSetDivider(CMU_CLK_CHUB_BUS, mDfsCurrFreq + 1);
        dfsCalSampleTime(0, dfs);
    }
}

static void dfsIdleMonitorCb(void *data, u64 * idleUs, u64 * totalUS)
{
    struct DfsSensor *dfs = data;
    DfsFreq freqId = mDfsCurrFreq;
    uint32_t perSamplTime;
    uint32_t diff;
    u64 activeUs;
    uint32_t orgSampleTime;
    int i;

    if (!dfs->rate) {
        CSP_PRINTF_INFO
            ("ERROR: dfsCB(%d)(%d/%d): set default table without SampleRate\n",
             dfs->sensorType, dfs->updated, DFS_MAX_IDLE_CB);
        for(i = 0; i < DFS_FREQ_MAX; i++) {
            dfs->sample->sampleTime[i] = dfsDefaultSampleUs[i + 1];
        }
        dfs->updated = DFS_MAX_IDLE_CB;
        mDfsCurrFreq = DFS_FREQ_MAX - 1;
    } else {
        /* get idleMonitor's perSampleTime */
        if (*totalUS < *idleUs) {
            CSP_PRINTF_ERROR("dfsCB(%d): Error! inval total:%lld, idle:%lld\n",
                             (u32) dfs->sensorType, *totalUS, *idleUs);
        } else {
            activeUs = *totalUS - *idleUs;
            perSamplTime = (u32) (activeUs / dfs->rate);
            CSP_PRINTF_INFO
                ("> dfsCB(%d)(%d/%d),f:%d,actT:%lld,Rate:%d,samT:%lu\n",
                 (u32) dfs->sensorType, (u32) dfs->updated, DFS_MAX_IDLE_CB,
                 freqId, activeUs, (u32) dfs->rate, perSamplTime);

            /* get table's perSampleTime */
            orgSampleTime = dfs->sample->sampleTime[freqId];

            /* calc Diff */
            diff = (perSamplTime > orgSampleTime) ?
                (perSamplTime - orgSampleTime) : (orgSampleTime - perSamplTime);

            /* validation of table */
            if (dfs->sample->sensType != dfs->sensorType) {
                CSP_PRINTF_ERROR
                    ("dfsCB: has invalid sensorType:%lu vs %lu\n",
                     dfs->sample->sensType, (uint32_t) dfs->sensorType);
                CSP_ASSERT(0);
            }

            CSP_PRINTF_INFO("=>dfsCB(%lu): new:%lu,org:%lu,diff:%lu\n",
                            (uint32_t) dfs->sensorType, perSamplTime,
                            orgSampleTime, diff);

            /* update Table */
            dfs->sample->sampleTime[freqId] = perSamplTime;
            CSP_PRINTF_INFO("==> dfsCB(%lu): Up t:%lu,%lu,%lu,%lu,%lu\n",
                            (uint32_t) dfs->sensorType,
                            dfs->sample->sampleTime[DFS_FREQ_360MHz],
                            dfs->sample->sampleTime[DFS_FREQ_180MHz],
                            dfs->sample->sampleTime[DFS_FREQ_120MHz],
                            dfs->sample->sampleTime[DFS_FREQ_90MHz],
                            dfs->sample->sampleTime[DFS_FREQ_60MHz]);
        }
    }

    /* disable IdleMonitor's CB */
    dfsScanNext(dfs);
}

void dfsSensorConfig(uint8_t sensorType, uint32_t rate)
{
    uint32_t sensor = mDfsSensorList[sensorType - 1];

    if (!mDfsSensorTable[sensor].sensorType)
        mDfsSensorTable[sensor].sensorType = sensorType;

    if (sensor == MAX_REGISTERED_SENSORS)
        return;

    if (!(mDfsSensorTable[sensor].flag & DFS_FLAG_POWER)) {
        osLog(LOG_ERROR, " Sensor %u No power information\n", sensorType);
        return;
    }

    if (rate == SENSOR_RATE_ONCHANGE)
        mDfsSensorTable[sensor].rate = 5;
    else
        mDfsSensorTable[sensor].rate = rate / 1024;

    if (mDfsGovernor == DFS_GOVERNOR_MAX)
        return;

    if ((mDfsSensorTable[sensor].rate == 0)
        && (mDfsSensorTable[sensor].flag & DFS_FLAG_LOCK))
        dfsClearFreqLock(sensorType);

    if (mDfsGovernor == DFS_GOVERNOR_OFF)
        return;
    // update
    dfsUpdateSensor();
}

void dfsSelectGovernor(IN DfsGovernor gov)
{

    if (gov >= DFS_GOVERNOR_MAX)
        return;

    mDfsGovernor = gov;
    ipc_set_dfs_gov(mDfsGovernor);

    if (mDfsUpdated == 0) {
        mDfsUpdated = 1;
        dfsCalSampleTime(0, mDfsSensorTable);
        return;
    }

    sendDfsEvent(DFS_REQUEST_SAVE);

    if (mDfsGovernor == DFS_GOVERNOR_OFF) {
        mDfsCurrFreq = DFS_FREQ_360MHz;
        //cmuSetCpuClk(mDfsFrequency[DFS_FREQ_360MHz]);
        cmuSetDivider(CMU_CLK_CHUB_BUS, DFS_FREQ_360MHz + 1);

        return;
    }
    dfsUpdateSensor();
}

IN DfsGovernor dfsGetCurGovernor(void)
{

    return mDfsGovernor;
}

void dfsLoadSampleTime(struct DfsToSensorHalDataPayload *data)
{
    uint32_t sensor, i;

    //SENS_TYPE_LAST_USER is for sensNum and governor
    if (data->sensType == SENS_TYPE_LAST_USER) {
        if (data->dfsInfo.sensNum) {
            ipc_set_dfs_numSensor(data->dfsInfo.sensNum);
            ipc_set_dfs_gov(data->dfsInfo.governor);
            mDfsGovernor = ipc_get_dfs_gov();
            osLog(LOG_INFO, "sensNum  %lu\n", data->dfsInfo.sensNum);
            osLog(LOG_INFO, "governor %lu\n", data->dfsInfo.governor);
        } else {
            // there is no file to load
            osLog(LOG_INFO, "dfsLoadSampleTime: No File to Load\n");
        }
    } else {
        if (data->sensType == SENS_TYPE_INVALID) {
            for (i = 0; i < DFS_FREQ_MAX; i++)
                ipc_get_dfs_sampleTime()->sampleTime[i] = data->data[i];
            osLog(LOG_INFO, "sensType = %lu\n",
                  ipc_get_dfs_sampleTime()->sensType);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  ipc_get_dfs_sampleTime()->sampleTime[0]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  ipc_get_dfs_sampleTime()->sampleTime[1]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  ipc_get_dfs_sampleTime()->sampleTime[2]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  ipc_get_dfs_sampleTime()->sampleTime[3]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  ipc_get_dfs_sampleTime()->sampleTime[4]);
        } else {
            sensor = mDfsSensorList[data->sensType - 1];
            if (mDfsSensorTable[sensor].sample->sensType != data->sensType) {
                osLog(LOG_ERROR,
                      "dfsLoadSampleTime not matching sensType %lu %lu\n",
                      mDfsSensorTable[sensor].sample->sensType, data->sensType);
                return;
            }
            for (i = 0; i < DFS_FREQ_MAX; i++)
                mDfsSensorTable[sensor].sample->sampleTime[i] = data->data[i];
            osLog(LOG_INFO, "sensType = %lu\n",
                  mDfsSensorTable[sensor].sample->sensType);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  mDfsSensorTable[sensor].sample->sampleTime[0]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  mDfsSensorTable[sensor].sample->sampleTime[1]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  mDfsSensorTable[sensor].sample->sampleTime[2]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  mDfsSensorTable[sensor].sample->sampleTime[3]);
            osLog(LOG_INFO, "sampleTime %lu\n",
                  mDfsSensorTable[sensor].sample->sampleTime[4]);
        }
    }
}
