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

#include <seos.h>
#include "exynos_fusion.h"

#ifdef USE_ANY_MOTION
#define DERIV_THR_AMD   0.15f
#define DELAY_LEVEL_AMD 25
#define MODE_AMD        7  // Mode

// MODE_AMD Description; [1, 7]
// Which axis to be activated is determined depending on the MODE_AMD as follows,
//
// MODE_AMD   |   z_axis   |   y_axis   |   x_axis
//     1      |     X      |     X      |     O
//     2      |     X      |     O      |     X
//     3      |     X      |     O      |     O
//     4      |     O      |     X      |     X
//     5      |     O      |     X      |     O
//     6      |     O      |     O      |     X
//     7      |     O      |     O      |     O
#endif

void exynosFusionGetParams(void *params[]) {
#ifdef USE_ANY_MOTION
    if (params[ANYMOTION]) {
        struct EF_anymotionParams *p = (struct EF_anymotionParams *)params[ANYMOTION];
        p->deriv_thr = DERIV_THR_AMD;
        p->delay_level_amd = DELAY_LEVEL_AMD;
        p->axis_on_flags = MODE_AMD;
    }
#endif

#ifdef USE_WIN_ORIENTATION
    if (params[WIN_ORIENTATION]) {
        struct EF_winOrientParams *p = (struct EF_winOrientParams *)params[WIN_ORIENTATION];
        p->enable_screen_updown = 0;
        p->screen_up_value = -1;
        p->screen_down_value = -1;
    }
#endif

#ifdef USE_GRAVITY
    if (params[GRAVITY]) {
        struct EF_gravityParams *p = (struct EF_gravityParams *)params[GRAVITY];
        p->tau = 0.0;
    }
#endif
}

void exynosFusionEnableModules(struct tEnableModule *enM) {
    if (enM) {
#ifdef USE_ANY_MOTION
        enM->enableModule[ANYMOTION] = true;
#endif
#ifdef USE_SIG_MOTION
        enM->enableModule[SIGMOTION] = true;
#endif
#ifdef USE_STEP_DETECT
        enM->enableModule[STEPDETECT] = true;
        enM->enableModule[STEPCOUNT] = true;
#endif
#ifdef USE_GRAVITY
        enM->enableModule[GRAVITY] = true;
#endif
#ifdef USE_WIN_ORIENTATION
        enM->enableModule[WIN_ORIENTATION] = true;
#endif
#ifdef USE_ROTATION_VECTOR
        enM->enableModule[ROTATION_VECTOR] = true;
#endif
#ifdef USE_GAME_ROTATION_VECTOR
        enM->enableModule[GAME_ROTATION_VECTOR] = true;
#endif
#ifdef USE_GEOMAGNETIC_ROTATION
        enM->enableModule[GEOMAGNETIC_ROTATION] = true;
#endif
#ifdef USE_LINEAR_ACCELERATION
        enM->enableModule[LINEAR_ACCELERATION] = true;
#endif
#ifdef USE_TILT
        enM->enableModule[TILT] = true;
#endif
#ifdef USE_ORIENTATION
        enM->enableModule[ORIENTATION] = true;
#endif
#ifdef USE_PEDOMETER
        enM->enableModule[PEDOMETER] = true;
#endif
#ifdef USE_STATIONARY_DETECTOR
        enM->enableModule[STATIONARY_DETECTOR] = true;
#endif
    }
}

INTERNAL_APP_INIT(
        EXYNOS_FUSION_APP_ID,
        EXYNOS_FUSION_APP_VERSION,
        exynosFusionStart,
        exynosFusionEnd,
        exynosFusionHandleEvent);
