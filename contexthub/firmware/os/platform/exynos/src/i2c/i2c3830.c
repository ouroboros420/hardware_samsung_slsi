/*----------------------------------------------------------------------------
 * Exynos SoC  -  I2C
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Huiling Wu <huiling.wu@samsung.com>
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

#include <gpio.h>
#include <i2c.h>
#include CSP_HEADER(gpio)
#include CSP_HEADER(cmgp)

typedef struct {
    GpioPinNumType pinSCL;
    GpioPinNumType pinSDA;
}I2cGpioInfoType;

struct I2cMsgType {
    uint16_t flags;
    uint16_t addr;
    uint16_t rxSize;
    uint16_t txSize;
    uint16_t rxPtr;
    uint16_t txPtr;
    uint8_t *rxBuf;
    uint8_t *txBuf;
    void *cookie;
    I2cCallbackF callback;
};

struct I2cInfoType {
    uint8_t mode;
    uint8_t speedMode;
    uint8_t operationMode;
    uint8_t state;
    uint8_t gpioSCL;
    uint8_t gpioSDA;
    uint8_t irqNum;
    uint8_t stop;
    uint16_t tid;
    uint32_t sfrBase;
    uint32_t speed;
    uint32_t errorState;
    struct I2cMsgType msg;
};

#define DEC_I2C(base, scl, sda, irq, opmode) \
    {.sfrBase = base, .gpioSCL = scl, .gpioSDA = sda, .irqNum = irq, .operationMode = opmode}
//p-239
struct I2cInfoType mI2cInfo[I2C_CHANNEL_MAX] = {
    DEC_I2C(I2C0_CMGP_BASE_ADDRESS, GPM00_0, GPM01_0, SYSREG1_CMGP_IRQn, I2C_MODE_POLLING),   // I2C_CMGP0
    DEC_I2C(I2C1_CMGP_BASE_ADDRESS, GPM04_0, GPM05_0, SYSREG1_CMGP_IRQn, I2C_MODE_POLLING),   // I2C_CMGP1
};

void i2cSetOperationMode(uint32_t busId, uint8_t mode) {
    mI2cInfo[busId].operationMode = mode;
}
