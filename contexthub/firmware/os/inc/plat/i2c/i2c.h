/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C
 *----------------------------------------------------------------------------
 *      Name:    i2c.h
 *      Purpose: To expose I2C APIs and define macros
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

#ifndef __EXYNOS_I2C_H__
#define __EXYNOS_I2C_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#include <stdbool.h>
#include <stddef.h>
#include <i2c.h>

#define I2C_MODE_MASTER         (0x0)
#define I2C_MODE_SLAVE          (0x1)
#define I2C_MODE_POLLING        (0x0)
#define I2C_MODE_INTERRUPT      (0x1)
#define I2C_MODE_FS             (0x0)
#define I2C_MODE_HS             (0x1)

#define I2C_HS_CLOCK            2500000
#define I2C_FS_CLOCK            400000

#define I2C_M_TENBIT            0x0010  /* this is a ten bit chip address */
#define I2C_M_RD                0x0001  /* read data, from slave to master */
#define I2C_M_STOP              0x8000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART           0x4000  /* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR      0x2000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK        0x1000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK         0x0800  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN          0x0400  /* length will be first received byte */

#define I2C_DEBUG               0

#if (I2C_DEBUG)
    #define I2C_DEBUG_PRINT(fmt, ... ) \
        osLog(LOG_DEBUG, fmt, ##__VA_ARGS__);
#else
    #define I2C_DEBUG_PRINT(fmt, ...)
#endif

#define I2C_ERR_INVALID_VALUE 	0x8001
#define I2C_ERR_CONFIG 			0x8002
#define I2C_ERR_TIMEOUT 		0x8003
#define I2C_ERR_IO 				0x8004

#ifndef SEOS
	typedef void (*I2cCallbackF)(void *cookie, size_t tx, size_t rx, int err);
#endif

struct ExynosI2cBoardCfg {
    uint32_t xferSpeed;
    uint32_t master;
    uint32_t mode;
    uint32_t slaveAddr;
};

void i2cInit(void);
void i2c_IRQHandler(uint32_t busId);
int i2cMasterRequest(uint32_t busId, uint32_t speedInHz);
int i2cMasterRelease(uint32_t busId);
int i2cMasterTxRx(uint32_t busId, uint32_t addr, const void *txBuf, size_t txSize,
        void *rxBuf, size_t rxSize, I2cCallbackF callback, void *cookie);
int i2cSlaveRequest(uint32_t busId, uint32_t addr);
int i2cSlaveRelease(uint32_t busId);
void i2cSlaveEnableRx(uint32_t busId, void *rxBuf, size_t rxSize,
        I2cCallbackF callback, void *cookie);
int i2cSlaveTxPreamble(uint32_t busId, uint8_t byte,
        I2cCallbackF callback, void *cookie);
int i2cSlaveTxPacket(uint32_t busId, const void *txBuf, size_t txSize,
        I2cCallbackF callback, void *cookie);
void i2cSetOperationMode(uint32_t busId, uint8_t mode);
void i2cCmgpIrqHandler(uint32_t busId);
int i2cOpen(uint32_t busId, uint32_t speedInHz);
int i2cRead(uint32_t busId, uint32_t slaveAddr, uint32_t regAddr, uint32_t length, uint8_t *rxBuf);
int i2cWrite(uint32_t busId, uint32_t slaveAddr, uint32_t regAddr, uint32_t length, uint8_t *txBuf);
void i2cSaveState(void);
void i2cRestoreState(void);
void i2cCmgpSaveState(void);
void i2cCmgpRestoreState(void);
#endif

