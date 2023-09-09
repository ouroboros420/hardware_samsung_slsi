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

#define DERIV_THR_AMD       0.15f
#define DELAY_LEVEL_AMD     25
#define MODE_AMD            7       // Mode

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

void exynosFusionGetParams(void* params[])
{
    if (params[ANYMOTION]) {
        struct EF_anymotionParams* p = (struct EF_anymotionParams*)params[ANYMOTION];
        p->deriv_thr = DERIV_THR_AMD;
        p->delay_level_amd = DELAY_LEVEL_AMD;
        p->axis_on_flags = MODE_AMD;
    }

    if (params[WIN_ORIENTATION]) {
        struct EF_winOrientParams* p = (struct EF_winOrientParams*)params[WIN_ORIENTATION];
        p->enable_screen_updown = 0;
        p->screen_up_value = -1;
        p->screen_down_value = -1;
    }

    if (params[GRAVITY]) {
        struct EF_gravityParams* p = (struct EF_gravityParams*)params[GRAVITY];
        p->tau = 0.0;
    }

}

void exynosFusionEnableModules(struct tEnableModule *enM)
{
    if (enM) {
        enM->enableModule[ANYMOTION] = true;
        enM->enableModule[SIGMOTION] = true;
        enM->enableModule[STEPDETECT] = true;
        enM->enableModule[STEPCOUNT] = true;
        enM->enableModule[GRAVITY] = true;
        enM->enableModule[WIN_ORIENTATION] = true;
        enM->enableModule[ROTATION_VECTOR] = true;
        enM->enableModule[GAME_ROTATION_VECTOR] = true;
        enM->enableModule[GEOMAGNETIC_ROTATION] = true;
        enM->enableModule[LINEAR_ACCELERATION] = true;
        enM->enableModule[TILT] = true;
        enM->enableModule[ORIENTATION] = true;
    }
}

INTERNAL_APP_INIT(
    EXYNOS_FUSION_APP_ID,
    EXYNOS_FUSION_APP_VERSION,
    exynosFusionStart,
    exynosFusionEnd,
    exynosFusionHandleEvent);
