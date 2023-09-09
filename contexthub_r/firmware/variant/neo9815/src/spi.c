/*
 * Copyright (C) 2016 The Android Open Source Project
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

// Will clean up codes after verification
#if 0

#include <plat/inc/spi.h>
#include <util.h>

static const struct exynosSpiBoardCfg mExynosSpiBoardCfgs[] = {
    [0] = {
        .gpioMiso = GPIO_PD1(4),
        .gpioMosi = GPIO_PD1(5),
        .gpioSclk = GPIO_PD1(6),
        .gpioNss = GPIO_PD1(7),
        .gpioFunc = GPIO_FUNC_ALT,
        .gpioSpeed = GPIO_SPEED_MEDIUM,
    },
};

const struct exynosSpiBoardCfg *boardExynosSpiCfg(uint8_t busId)
{
    if (busId >= ARRAY_SIZE(mExynosSpiBoardCfgs))
        return NULL;

    return &mExynosSpiBoardCfgs[busId];
}
#endif

