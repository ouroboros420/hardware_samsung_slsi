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

#include <plat/i2c.h>
#include <util.h>

const struct ExynosI2cBoardCfg *boardExynosI2cCfg(uint8_t busId);

static const struct ExynosI2cBoardCfg mExynosI2cBoardCfgs[] = {
    [0] = {
           .master = I2C_MASTER,
           .mode = I2C_POLLING,
           .xfer_speed = I2C_SPEED_400KHZ,
           .slave_addr = 0x55,
           },
};

const struct ExynosI2cBoardCfg *boardExynosI2cCfg(uint8_t busId)
{
    if (busId >= ARRAY_SIZE(mExynosI2cBoardCfgs))
        return NULL;

    return &mExynosI2cBoardCfgs[busId];
}

