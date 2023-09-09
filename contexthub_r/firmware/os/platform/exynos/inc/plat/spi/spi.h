/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spi.h
 *      Purpose: To expose SPI APIs and define macros
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

#ifndef __SPI_H__
#define __SPI_H__

#include <csp_common.h>
#include <csp_assert.h>
#include <csp_printf.h>
#include <spi.h>
#include <spiDrv.h>
#include <sysreg.h>

#define SPI_DEBUG              0

#if (SPI_DEBUG)
    #define SPI_DEBUG_PRINT(fmt, ... ) \
        osLog(LOG_DEBUG, fmt, ##__VA_ARGS__);
#else
    #define SPI_DEBUG_PRINT(fmt, ...)
#endif

#if !defined(SEOS)
    #define ENOMEM 	12
    #define EINVAL 	22
    #define EOPNOTSUPP 	95

    typedef uint8_t spi_cs_t;
    typedef uint32_t SpiSpeed;

    typedef void (*SpiCbkF)(void *cookie, int err);

    struct SpiMode {
        enum {
            SPI_CPOL_IDLE_LO,
            SPI_CPOL_IDLE_HI,
        } cpol;

        enum {
            SPI_CPHA_LEADING_EDGE,
            SPI_CPHA_TRAILING_EDGE,
        } cpha;

        uint8_t bitsPerWord;
        enum {
            SPI_FORMAT_LSB_FIRST,
            SPI_FORMAT_MSB_FIRST,
        } format;

        uint16_t txWord;

        uint32_t speed;

        bool nssChange;
    };

    struct SpiPacket {
        void *rxBuf;
        const void *txBuf;
        size_t size;
        uint32_t delay;
    };

    struct SpiDevice {
        const struct SpiDevice_ops *ops;
        void *pdata;
    };

    struct SpiDevice_ops {
        int (*masterStartSync)(struct SpiDevice *dev, spi_cs_t cs,
                const struct SpiMode *mode);
        int (*masterStartAsync)(struct SpiDevice *dev, spi_cs_t cs,
                const struct SpiMode *mode);

        int (*masterRxTx)(struct SpiDevice *dev, void *rxBuf, const void *txBuf,
                size_t size, const struct SpiMode *mode);

        int (*masterStopSync)(struct SpiDevice *dev);
        int (*masterStopAsync)(struct SpiDevice *dev);

        int (*slaveStartSync)(struct SpiDevice *dev, const struct SpiMode *mode);
        int (*slaveStartAsync)(struct SpiDevice *dev, const struct SpiMode *mode);

        int (*slaveIdle)(struct SpiDevice *dev, const struct SpiMode *mode);
        int (*slaveRxTx)(struct SpiDevice *dev, void *rxBuf, const void *txBuf,
                size_t size, const struct SpiMode *mode);

        void (*slaveSetCsInterrupt)(struct SpiDevice *dev, bool enabled);
        bool (*slaveCsIsActive)(struct SpiDevice *dev);

        int (*slaveStopSync)(struct SpiDevice *dev);
        int (*slaveStopAsync)(struct SpiDevice *dev);

        int (*release)(struct SpiDevice *dev);
    };

#endif

void spiSetTransferMode(uint32_t portNum, SPI_transfer_mode mode);
void spi_IRQHandler(uint32_t busId);
uint32_t spiMasterInit(uint8_t *port);
void spiMasterTransfer(struct SpiDevice *dev, uint32_t port, uint32_t *txBuf, uint32_t *rxBuf, uint32_t size, const struct SpiMode *mode );
int spiMasterDeinit(uint8_t *portNum);
void spiSetDeviceInfo(uint32_t portNum, const struct spi_device *spi_dev);
int spiOpen(uint32_t portNum, struct spi_device *spiCfg);
int spiRead(uint32_t port, uint32_t addr, void* rxBuf, size_t size);
int spiWrite(uint32_t port, uint32_t addr, void* txBuf, size_t size);
void spiSaveState(void);
void spiRestoreState(void);
void spiCmgpSaveState(void);
void spiCmgpRestoreState(void);
#endif

