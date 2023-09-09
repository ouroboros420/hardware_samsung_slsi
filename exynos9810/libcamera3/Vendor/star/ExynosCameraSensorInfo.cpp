/*
**
** Copyright 2017, Samsung Electronics Co. LTD
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/*#define LOG_NDEBUG 0 */
#define LOG_TAG "ExynosCameraSensorInfo"
#include <cutils/log.h>

#include "ExynosCameraSensorInfo.h"

namespace android {

struct ExynosCameraSensorInfoBase *createExynosCameraSensorInfo(int camId)
{
    char sensorName[20] = {'\0',};
    struct ExynosCameraSensorInfoBase *sensorInfo = NULL;
    int sensorId = getSensorId(camId);
    if (sensorId < 0) {
        ALOGE("ERR(%s[%d]): Inavalid camId, sensor name is nothing", __FUNCTION__, __LINE__);
        sensorId = SENSOR_NAME_NOTHING;
    }

    switch (sensorId) {
    case SENSOR_NAME_S5K3M3:
        sensorInfo = new ExynosCameraSensor3M3(sensorId);
        snprintf(sensorName, sizeof(sensorName), "S5K3M3");
        break;
    case SENSOR_NAME_SAK2L3:
        sensorInfo = new ExynosCameraSensor2L3(sensorId);
        snprintf(sensorName, sizeof(sensorName), "SAK2L3");
        break;
    case SENSOR_NAME_IMX320:
        sensorInfo = new ExynosCameraSensorIMX320_3H1(sensorId);
        snprintf(sensorName, sizeof(sensorName), "IMX320");
        break;
    case SENSOR_NAME_S5K3H1:
        sensorInfo = new ExynosCameraSensorIMX320_3H1(sensorId);
        snprintf(sensorName, sizeof(sensorName), "S5K3H1");
        break;
    case SENSOR_NAME_S5K5F1:
        sensorInfo = new ExynosCameraSensorS5K5F1(sensorId);
        snprintf(sensorName, sizeof(sensorName), "S5K5F1");
        break;
    default:
        ALOGE("ERR(%s[%d]): Unknown sensor, Return NULL", __FUNCTION__, __LINE__);
        break;
    }

    ALOGI("INFO(%s[%d]):sensor ID %d name %s",__FUNCTION__, __LINE__, sensorId, sensorName);

    return sensorInfo;
}

ExynosCameraSensor2L3::ExynosCameraSensor2L3(int sensorId) : ExynosCameraSensor2L3Base(sensorId)
{
    /* Optional capabilities : vendor feature */
    supportedCapabilities |= (CAPABILITIES_PRIVATE_REPROCESSING | CAPABILITIES_RAW |
                                CAPABILITIES_CONSTRAINED_HIGH_SPEED_VIDEO);

    sceneHDRSupport = true;
    objectTrackingAvailable = true;

    /* for DNG */
    masterRGain = 0.472089f;
    masterBGain = 0.524721f;

#ifdef SUPPORT_DEPTH_MAP
    availableDepthSizeListMax = sizeof(SAK2L3_AVAILABLE_DEPTH_SIZE_LIST) / (sizeof(int) * 2);
    availableDepthSizeList = SAK2L3_AVAILABLE_DEPTH_SIZE_LIST;
    availableDepthFormatListMax = sizeof(SAK2L3_AVAILABLE_DEPTH_FORMATS_LIST) / sizeof(int);
    availableDepthFormatList = SAK2L3_AVAILABLE_DEPTH_FORMATS_LIST;
    depthMapSizeLutMax = sizeof(DEPTH_MAP_SIZE_LUT_SAK2L3) / (sizeof(int) * 3);
    depthMapSizeLut = DEPTH_MAP_SIZE_LUT_SAK2L3;
#endif
#if 0
    availableThumbnailCallbackSizeListMax = sizeof(SAK2L3_AVAILABLE_THUMBNAIL_CALLBACK_SIZE_LIST) / (sizeof(int) * 2);
    availableThumbnailCallbackSizeList = SAK2L3_AVAILABLE_THUMBNAIL_CALLBACK_SIZE_LIST;
    availableThumbnailCallbackFormatListMax = sizeof(SAK2L3_AVAILABLE_THUMBNAIL_CALLBACK_FORMATS_LIST) / sizeof(int);
    availableThumbnailCallbackFormatList = SAK2L3_AVAILABLE_THUMBNAIL_CALLBACK_FORMATS_LIST;
#endif
    fNumber = 2.4f;
    availableApertureValues = (float *)AVAILABLE_APERTURE_VALUE;
    availableApertureValuesLength = ARRAY_LENGTH(AVAILABLE_APERTURE_VALUE);
};

ExynosCameraSensor3M3::ExynosCameraSensor3M3(int sensorId) : ExynosCameraSensor3M3Base(sensorId)
{
    /* Use ExynosCameraSensorS5K3M3Base Constructor */
};

ExynosCameraSensorIMX320_3H1::ExynosCameraSensorIMX320_3H1(int sensorId) : ExynosCameraSensorIMX320_3H1Base(sensorId)
{
    /* Use ExynosCameraSensorIMX320_3H1Base Constructor */
    /* Optional capabilities : vendor feature */
    supportedCapabilities |= (CAPABILITIES_BURST_CAPTURE | CAPABILITIES_PRIVATE_REPROCESSING);
    sceneHDRSupport = true;
    screenFlashAvailable = true;
    fixedFaceFocusAvailable = true;

    availableThumbnailCallbackSizeListMax = sizeof(IMX320_3H1_AVAILABLE_THUMBNAIL_CALLBACK_SIZE_LIST) / (sizeof(int) * 2);
    availableThumbnailCallbackSizeList = IMX320_3H1_AVAILABLE_THUMBNAIL_CALLBACK_SIZE_LIST;
    availableThumbnailCallbackFormatListMax = sizeof(IMX320_3H1_AVAILABLE_THUMBNAIL_CALLBACK_FORMATS_LIST) / sizeof(int);
    availableThumbnailCallbackFormatList = IMX320_3H1_AVAILABLE_THUMBNAIL_CALLBACK_FORMATS_LIST;
};

ExynosCameraSensorS5K5F1::ExynosCameraSensorS5K5F1(int sensorId) : ExynosCameraSensorS5K5F1Base(sensorId)
{
    gain = 20;                      // 2.0;
    exposureTime = 332 * 100000;    // 33.2ms;
    ledCurrent = 5;                 // 450mA
    ledPulseDelay = 0 * 100000;     // 0ms
    ledPulseWidth = 240 * 100000;   // 24ms
    ledMaxTime = 10 * 1000;         // 10s;

    gainRange[MIN] = 1;
    gainRange[MAX] = 160;
    ledCurrentRange[MIN] = 1;                // 0mA
    ledCurrentRange[MAX] = 10;               // 950mA
    ledPulseDelayRange[MIN] = 0 * 100000;    // 0.0ms
    ledPulseDelayRange[MAX] = 1000 * 100000; // 100.0ms
    ledPulseWidthRange[MIN] = 0 * 100000;    // 0.0ms
    ledPulseWidthRange[MAX] = 333 * 100000;  // 33.3ms
    ledMaxTimeRange[MIN] = 1 * 1000;         // 1s
    ledMaxTimeRange[MAX] = 10 * 1000;        // 10s
};

}; /* namespace android */
