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

#ifndef EXYNOS_CAMERA_SENSOR_INFO_H
#define EXYNOS_CAMERA_SENSOR_INFO_H

#include "ExynosCameraSensorInfoBase.h"

const float AVAILABLE_APERTURE_VALUE[] = {1.5f, 2.4f};

namespace android {

struct ExynosCameraSensor2L3 : public ExynosCameraSensor2L3Base {
public:
    ExynosCameraSensor2L3(int sensorId);
};

struct ExynosCameraSensorIMX320_3H1 : public ExynosCameraSensorIMX320_3H1Base {
public:
    ExynosCameraSensorIMX320_3H1(int sensorId);
};

struct ExynosCameraSensor3M3 : public ExynosCameraSensor3M3Base {
public:
    ExynosCameraSensor3M3(int sensorId);
};

struct ExynosCameraSensorS5K5F1 : public ExynosCameraSensorS5K5F1Base {
public:
    ExynosCameraSensorS5K5F1(int sensorId);
};

/* Helpper functions */
struct ExynosCameraSensorInfoBase *createExynosCameraSensorInfo(int sensorName);

#ifdef SUPPORT_DEPTH_MAP
static int SAK2L3_AVAILABLE_DEPTH_SIZE_LIST[][2] =
{
    { 504, 378},
    { 504, 282},
    { 376, 376},
};

static int SAK2L3_AVAILABLE_DEPTH_FORMATS_LIST[] =
{
    HAL_PIXEL_FORMAT_RAW16,
};

static int DEPTH_MAP_SIZE_LUT_SAK2L3[][3] =
{
    /* { ratioId, width, height } */
    { SIZE_RATIO_16_9,
        504      ,  282      ,   /* [vci    ] */
    },
    { SIZE_RATIO_4_3,
        504      ,  378      ,   /* [vci    ] */
    },
    { SIZE_RATIO_1_1,
        376      ,  376      ,   /* [vci    ] */
    },
    { SIZE_RATIO_3_2,
        504      ,  378      ,   /* [vci    ] */
    },
    { SIZE_RATIO_5_4,
        504      ,  378      ,   /* [vci    ] */
    },
    { SIZE_RATIO_5_3,
        504      ,  378      ,   /* [vci    ] */
    },
    { SIZE_RATIO_11_9,
        504      ,  378      ,   /* [vci    ] */
    },
};
#endif

static int SAK2L3_AVAILABLE_THUMBNAIL_CALLBACK_SIZE_LIST[][2] =
{
    {  512,  384},
    {  512,  288},
    {  384,  384},
};

static int SAK2L3_AVAILABLE_THUMBNAIL_CALLBACK_FORMATS_LIST[] =
{
    HAL_PIXEL_FORMAT_YCbCr_420_888,
};

static int IMX320_3H1_AVAILABLE_THUMBNAIL_CALLBACK_SIZE_LIST[][2] =
{
    {  512,  384},
    {  512,  288},
    {  384,  384},
};

static int IMX320_3H1_AVAILABLE_THUMBNAIL_CALLBACK_FORMATS_LIST[] =
{
    HAL_PIXEL_FORMAT_YCbCr_420_888,
};

}; /* namespace android */
#endif
