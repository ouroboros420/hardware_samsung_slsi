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

#define EXYNOS_FUSION_APP_ID APP_ID_MAKE(NANOHUB_VENDOR_EXYNOS, 3)

#define EXYNOS_FUSION_APP_VERSION  1


enum exynosFusionSensorType {
    ANYMOTION,
    SIGMOTION,
    STEPDETECT,
    STEPCOUNT,
    GRAVITY,
    WIN_ORIENTATION,
    ROTATION_VECTOR,
    GAME_ROTATION_VECTOR,
    GEOMAGNETIC_ROTATION,
    LINEAR_ACCELERATION,
    TILT,
    ORIENTATION,
    NUM_OF_FUSION_SENSOR
};

#include "parameters.h"

struct tEnableModule {
    bool enableModule[NUM_OF_FUSION_SENSOR];
};

bool exynosFusionStart(uint32_t tid);
void exynosFusionEnd(void);
void exynosFusionHandleEvent(uint32_t evtType, const void *evtData);
void exynosFusionGetParams(void* params[]);
void exynosFusionEnableModules(struct tEnableModule *enM);

#endif
