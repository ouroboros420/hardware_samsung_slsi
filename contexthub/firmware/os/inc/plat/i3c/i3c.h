/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I3C
 *----------------------------------------------------------------------------
 *      Name:    i3c.h
 *      Purpose: To expose I3C APIs and define macros
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2020 Sukmin Kang
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

#ifndef __EXYNOS_I3C_H__
#define __EXYNOS_I3C_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <csp_common.h>
#include <i3c/ccc.h>

#define I3C_BUS_MAX_DEVS                1           // we use only one slave for i3c
#define I3C_BUS_MAX_I3C_SCL_RATE        12900000
#define I3C_BUS_TYP_I3C_SCL_RATE        12500000
#define I3C_BUS_I3C_OPEN_DRAIN_RATE     1500000     // this value is not according to spec.
#define I3C_BUS_I2C_FM_PLUS_SCL_RATE    1000000
#define I3C_BUS_I2C_FM_SCL_RATE         400000
#define I3C_BUS_TLOW_FM                 1300
#define I3C_BUS_TLOW_FM_PLUS            500
#define I3C_BUS_TLOW_OD                 200
#define I3C_BUS_THIGH_MAX_NS            41
#define I3C_BUS_THIGH_MIN_NS            24
#define I3C_BUS_TSU_STO_FM              600
#define I3C_BUS_TSU_STA_FM              600
#define I3C_BUS_TSU_STA_FM_PLUS         260
#define I3C_BUS_TCAS_MIN                40          // 38.4ns
#define I3C_BUS_TCBS_MIN                (I3C_BUS_TCAS_MIN / 2)
#define I3C_BUS_TCBP_MIN                (I3C_BUS_TCAS_MIN / 2)
#define I3C_BUS_TBUF_FM                 1300
#define I3C_BUS_TBUF_MIXED_MIN_NS       1300        // it is 1.3us

#if defined(EMBOS)
    #define EINVAL 	    22
    #define ENXIO       6
    #define EBUSY       16
#endif

#define I3C_BROADCAST_ADDR              0x7E

typedef void (*I3cCallbackF)(void *cookie, size_t tx, size_t rx, int err);

enum i3cXferMode {
    I3C_MODE_POLLING,
    I3C_MODE_INTERRUPT,
    I3C_MODE_DMA,
};

struct I3cPid {
    uint8_t pid[6];
};

struct I3cCfg {
    uint32_t sfrBase;
    uint32_t clkOpenDrain;
    uint32_t clkPushPull;
    uint32_t xferMode;
    IRQn_Type irq;
    uint8_t busId;
};

#define DECLARE_IRQ_HANDLERS(_n)        \
    extern void I3C##_n##_IRQHandler(); \
                                        \
    extern void I3C##_n##_IRQHandler()  \
    {                                   \
        i3cIsrEvent(_n);                \
    }

void i3cIsrEvent(uint32_t busId);
int i3cInit(void);
int i3cCccEnecDisec(uint32_t busId, uint32_t index, bool enable, uint8_t evts);
int i3cCccSetmrl(uint32_t busId, uint32_t index, struct I3cMrl *mrl);
int i3cMasterRequest(uint32_t busId, struct I3cPid *pid);
int i3cMasterTxRx(uint32_t busId, uint32_t index, const void *txBuf, size_t txSize,
        void *rxBuf, size_t rxSize, I3cCallbackF callback, void *cookie);
static inline int i3cMasterTx(uint32_t busId, uint32_t addr,
        const void *txBuf, size_t txSize, I3cCallbackF callback, void *cookie) {
    return i3cMasterTxRx(busId, addr, txBuf, txSize, NULL, 0, callback, cookie);
}
static inline int i3cMasterRx(uint32_t busId, uint32_t addr,
        void *rxBuf, size_t rxSize, I3cCallbackF callback, void *cookie) {
    return i3cMasterTxRx(busId, addr, NULL, 0, rxBuf, rxSize, callback, cookie);
}

extern const struct I3cCfg *getI3cCfg(uint8_t busId);

int i3cWrite(uint32_t busId, uint32_t handle, uint32_t regAddr, void *txBuf, size_t txSize);
int i3cRead(uint32_t busId, uint32_t handle, uint32_t regAddr, void *rxBuf, size_t rxSize);
int i3cReadLarge(uint32_t busId, uint32_t handle, uint32_t regAddr, void *rxBuf, size_t rxSize, bool inc);

void i3cRestoreState(void);
void i3cSaveState(void);
#endif  //  __EXYNOS_I3C_H__
