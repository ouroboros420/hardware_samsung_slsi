/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C9110
 *----------------------------------------------------------------------------
 *      Name:    i2c9110.c
 *      Purpose: To implement I2C driver functions
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
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

struct I2cInfoType mI2cInfo[I2C_CHANNEL_MAX] = {
    DEC_I2C(I2C0_BASE_ADDRESS, GPM00_0, GPM01_0, CMGP_USI0_IRQ, I2C_MODE_POLLING),    // I2C_CHUB0
    DEC_I2C(I2C1_BASE_ADDRESS, GPM02_0, GPM03_0, CMGP_I2C0_IRQ, I2C_MODE_POLLING),    // I2C_CHUB1
    DEC_I2C(I2C2_BASE_ADDRESS, GPM20_0, GPM21_0, CMGP_I2C6_IRQ, I2C_MODE_POLLING),    // I2C_CHUB2
    DEC_I2C(I2C3_BASE_ADDRESS, GPM04_0, GPM05_0, CMGP_USI1_IRQ, I2C_MODE_POLLING),    // I2C_CMGP2
    DEC_I2C(I2C4_BASE_ADDRESS, GPM06_0, GPM07_0, CMGP_I2C1_IRQ, I2C_MODE_POLLING),    // I2C_CMGP3
    DEC_I2C(I2C5_BASE_ADDRESS, GPM08_0, GPM09_0, CMGP_USI2_IRQ, I2C_MODE_POLLING),    // I2C_CMGP4
    DEC_I2C(I2C6_BASE_ADDRESS, GPM10_0, GPM11_0, CMGP_I2C2_IRQ, I2C_MODE_POLLING),    // I2C_CMGP5
    DEC_I2C(I2C7_BASE_ADDRESS, GPM12_0, GPM13_0, CMGP_USI3_IRQ, I2C_MODE_POLLING),    // I2C_CMGP6
    DEC_I2C(I2C8_BASE_ADDRESS, GPM14_0, GPM15_0, CMGP_I2C3_IRQ, I2C_MODE_POLLING),    // I2C_CMGP7
    DEC_I2C(I2C9_BASE_ADDRESS, GPM16_0, GPM17_0, CMGP_I2C4_IRQ, I2C_MODE_POLLING),    // I2C_CMGP8
    DEC_I2C(I2C10_BASE_ADDRESS, GPM18_0, GPM19_0, CMGP_I2C5_IRQ, I2C_MODE_POLLING),   // I2C_CMGP9
};

void i2cSetOperationMode(uint32_t busId, uint8_t mode) {
    mI2cInfo[busId].operationMode = mode;
}
