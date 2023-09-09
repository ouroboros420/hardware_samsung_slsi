
#ifndef __DFS_H__
#define __DFS_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#if defined(SEOS)
#include <halIntf.h>
#endif

typedef enum {
    DFS_FREQ_360MHz,
    DFS_FREQ_180MHz,
    DFS_FREQ_120MHz,
    DFS_FREQ_90MHz,
    DFS_FREQ_60MHz,
    DFS_FREQ_MAX,
} DfsFreq;

typedef enum {
    DFS_GOVERNOR_OFF,
    DFS_GOVERNOR_SIMPLE,
    DFS_GOVERNOR_POWER,
    DFS_GOVERNOR_MAX,
} DfsGovernor;

struct sampleTimeTable {
    uint32_t sensType;
    uint32_t sampleTime[DFS_FREQ_MAX];
};

#define MAX_SENS_NUM (20)

void dfsSetFreqLock(uint8_t sensorType, IN DfsFreq freq);
void dfsClearFreqLock(uint8_t sensorType);
void dfsInit(uint8_t numSensors, uint8_t * sensorList);
void dfsSensorConfig(uint8_t sensorType, uint32_t rate);
void dfsDebugPrint(void);
void dfsSelectGovernor(IN DfsGovernor);
#if defined(SEOS)
void dfsLoadSampleTime(struct DfsToSensorHalDataPayload* data);
#endif
IN DfsGovernor dfsGetCurGovernor();
#endif
