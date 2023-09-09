/*
 * Copyright@ SAMSUNG Electronics Co., Ltd.
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

#ifndef EXYNOS_FUSION_H
#define EXYNOS_FUSION_H

#include <seos.h>

#define EXYNOS_FUSION_APP_ID      APP_ID_MAKE(NANOHUB_VENDOR_EXYNOS, 3)
#define EXYNOS_FUSION_APP_VERSION 1

#define USE_ANY_MOTION
#define USE_SIG_MOTION
#define USE_STEP_DETECT
#define USE_GRAVITY
#define USE_WIN_ORIENTATION
#define USE_ROTATION_VECTOR
#define USE_GAME_ROTATION_VECTOR
#define USE_GEOMAGNETIC_ROTATION
#define USE_LINEAR_ACCELERATION
#define USE_TILT
#define USE_ORIENTATION
#define USE_PEDOMETER
#define USE_STATIONARY_DETECTOR

#ifdef s5e8535
#undef USE_GRAVITY
#undef USE_ROTATION_VECTOR
#undef USE_GAME_ROTATION_VECTOR
#undef USE_GEOMAGNETIC_ROTATION
#undef USE_LINEAR_ACCELERATION
#undef USE_TILT
#undef USE_ORIENTATION
#undef USE_PEDOMETER
#endif

enum exynosFusionSensorType {
#ifdef USE_ANY_MOTION
    ANYMOTION,
#endif
#ifdef USE_SIG_MOTION
    SIGMOTION,
#endif
#ifdef USE_STEP_DETECT
    STEPDETECT,
    STEPCOUNT,
#endif
#ifdef USE_GRAVITY
    GRAVITY,
#endif
#ifdef USE_WIN_ORIENTATION
    WIN_ORIENTATION,
#endif
#ifdef USE_ROTATION_VECTOR
    ROTATION_VECTOR,
#endif
#ifdef USE_GAME_ROTATION_VECTOR
    GAME_ROTATION_VECTOR,
#endif
#ifdef USE_GEOMAGNETIC_ROTATION
    GEOMAGNETIC_ROTATION,
#endif
#ifdef USE_LINEAR_ACCELERATION
    LINEAR_ACCELERATION,
#endif
#ifdef USE_TILT
    TILT,
#endif
#ifdef USE_ORIENTATION
    ORIENTATION,
#endif
#ifdef USE_PEDOMETER
    PEDOMETER,
#endif
#ifdef USE_STATIONARY_DETECTOR
    STATIONARY_DETECTOR,
#endif
    NUM_OF_FUSION_SENSOR
};

#include "parameters.h"

struct tEnableModule {
    bool enableModule[NUM_OF_FUSION_SENSOR];
};

bool exynosFusionStart(uint32_t tid);
void exynosFusionEnd(void);
void exynosFusionHandleEvent(uint32_t evtType, const void *evtData);
void exynosFusionGetParams(void *params[]);
void exynosFusionEnableModules(struct tEnableModule *enM);

#endif
