/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spiDrv.h
 *      Purpose: To expose SPI driver functions
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

#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__

#include <csp_common.h>
#include <sysreg.h>

typedef enum {
    SLAVE_RX,
    SLAVE_TX,
} SPI_SlaveMode;

//#define SPI_TRANS_SIZE         8         // for debugging
//#define SPI_BUFFER_SIZE        32        // for debugging
#define SPI_INTERRUPT_MODE_EN    1
#define SPI_FIFO_LENGTH          64

/* SPI Master/Slave Selection */
typedef enum {
    SPI_MASTER = 0,
    SPI_SLAVE,
} SPI_OpMode;

/* SPI Clock Polarity */
typedef enum {
    SPI_ACTIVE_HIGH = 0,
    SPI_ACTIVE_LOW
} SPI_CPOL;

/* SPI Clock Phase */
typedef enum {
    SPI_FORMAT_A = 0,
    SPI_FORMAT_B
} SPI_CPHA;

/* SPI DMA mode Structure. */
typedef enum {
    SPI_DMA_SINGLE = 0,
    SPI_DMA_4_BURST,
} SPI_DMA_type;

/*SPI Interrupt Condition */
typedef enum {
    SPI_INT_TRAILING = (1 << 6),
    SPI_INT_RX_OVERRUN = (1 << 5),
    SPI_INT_RX_UNDERRUN = (1 << 4),
    SPI_INT_TX_OVERRUN = (1 << 3),
    SPI_INT_TX_UNDERRUN = (1 << 2),
    SPI_INT_RX_FIFORDY = (1 << 1),
    SPI_INT_TX_FIFORDY = (1 << 0),
    SPI_INT_ALL = 0x7F
} SPI_IntrModeBit_et;

typedef enum {
    SPI_CLR_PND_TX_UNDERRUN = (1 << 4),
    SPI_CLR_PND_TX_OVERRUN = (1 << 3),
    SPI_CLR_PND_RX_UNDERRUN = (1 << 2),
    SPI_CLR_PND_RX_OVERRUN = (1 << 1),
    SPI_CLR_PND_TRAILING = (1 << 0),
    SPI_CLR_PND_ALL = 0x1F,
} SPI_IntClrModeBit_et;

typedef enum {
    SPI_STATUS_TX_DONE = (1 << 25),
    SPI_STATUS_TRAILING_BYTE = (1 << 24),
    SPI_STATUS_RX_OVERRUN = (1 << 5),
    SPI_STATUS_RX_UNDERRUN = (1 << 4),
    SPI_STATUS_TX_OVERRUN = (1 << 3),
    SPI_STATUS_TX_UNDERRUN = (1 << 2),
    SPI_STATUS_RX_FIFORDY = (1 << 1),
    SPI_STATUS_TX_FIFORDY = (1 << 0)
} SPI_StatusBit_et;


/* SPI Transfer Data Type. */
typedef enum {
    SPI_BYTE = 0,
    SPI_HWORD = 1,
    SPI_WORD = 2
} SPI_transfer_data_type;


/* SPI Feedback Clock Delay Value. */
typedef enum {
    //New CLock Feedback (for RX) Schem
    SPI_PHASE_DELAY0 = 0x00,
    SPI_PHASE_DELAY90 = 0x01,
    SPI_PHASE_DELAY180 = 0x02,
    SPI_PHASE_DELAY270 = 0x03,
    SPI_PHASE_DELAY = 0x04,

} SPI_feedbackClock;


/* SPI Transfer mode */
typedef enum {
    SPI_POLLING_MODE = 0x0,
    SPI_DMA_MODE = 0x1,
    SPI_INTERRUPT_MODE = 0x4,
} SPI_transfer_mode;

typedef enum {
    SPI_NSSOUT_ACTIVE = 0,
    SPI_NSSOUT_INACTIVE = 1,
    SPI_NSSOUT_USER_MODE = 2,
} SPI_NSSOUT_et;


typedef enum {
    SPI_NO_SWAP = 0,
    SPI_BIT_SWAP = (1 << 1),        // Bit swap.
    SPI_BYTE_SWAP = (1 << 2),        // Byte swap.
    SPI_HWORD_SWAP = (1 << 3),        // Half Word swap.
} SPI_SwapMode;

typedef enum {
    SPI_Manual,
    SPI_Auto,
} SPI_CsMode;

#define MAX_NAME      16
#define MAX_SLAVE_NUM 4

typedef void (*SPI_IntHandler_t)(void);
typedef void (*SPI_Callback_t)(uint32_t Value);

struct spi_device {
    struct SpiDevice        *dev;
    uint32_t                base;
    uint32_t                m_portId;
    uint32_t                m_usiPort;
    int32_t                 m_IRQ;
    uint8_t                 m_CS[MAX_SLAVE_NUM];
    uint32_t                m_GPIO;
    uint32_t                m_slave;
    uint32_t                m_cs;
    bool                    m_multiSlave;
    bool                    m_RWBit;
    /* CH_CFG */
    bool                    m_EnHighSpeed;        // Enable High Speed in Salve Tx
    SPI_OpMode              m_OpMode;            // Master or Slave Mode
    SPI_CPOL                m_CPOL;                // CPOL
    SPI_CPHA                m_CPHA;                // CPHA
    bool                    m_EnRxChannel;        // Enable Tx Channel
    bool                    m_EnTxChannel;        // Enalbe Rx Channel
    bool                    m_EnClk;            // Clock Enable/Disable
    uint32_t                m_OpClock;            // Opeation clock
    uint32_t                m_TargetSPICLK;        // Target Clock

    /*MODE_CFG */
    SPI_transfer_data_type  m_ChWidth;            // BYTE, HWORD, WORD
    uint32_t                m_TraillingCnt;        // Trailing Bytes threshold value in Rx FIFO
    SPI_transfer_data_type  m_BusWidth;            // BYTE, HWORD, WORD
    uint32_t                m_RxReadyLevel;        // Rx FIFO trigger level
    uint32_t                m_TxReadyLevel;        // Tx FIFO trigger level
    SPI_transfer_mode       m_RxTransferMode;    // Interrupt mode, DMA mode
    SPI_transfer_mode       m_TxTransferMode;    // Interrupt mode, DMA mode
    uint32_t                m_MaxFIFOSize;        // Port0 : 256 bytes, Port1/2 : 64 bytes
    uint32_t                m_FifoLevelGrid;    // Port0 : 4 bytes, Port1/2 : 1 byte

    uint32_t                m_TxTransferSize;
    uint32_t                m_RxTransferSize;
    SPI_DMA_type            m_DMAType;            // Single or 4Burst

    /* CS_REG */
    uint32_t                m_NCSTimeCnt;        // NCS Time Count
    bool                    m_EnAutoCS;            // Manual or Auto Chip Selection
    SPI_NSSOUT_et           m_NSSOut;            // Active or Inactive in Manual mode Chip Selection

    /* INT_EN */
    bool                    m_EnTrailingInt;            // Enable Trailing Interrupt
    bool                    m_EnRxOverrunInt;        // Enable Rx Overrun Interrupt
    bool                    m_EnRxUnderrunInt;        // Enable Rx Underrun Interrupt
    bool                    m_EnTxOverrunInt;        // Enable Tx Overrun Interrupt
    bool                    m_EnTxUnderrunInt;        // Enable Tx Underrun Interrupt
    bool                    m_EnRxFifoReadyInt;    // Enable Rx FIFO Ready Interrupt
    bool                    m_EnTxFifoReadyInt;    // Enable Tx FIFO Ready Interrupt

    /* PACKET_CNT_REG */
    bool                    m_EnPacketCnt;
    uint32_t                m_PacketCntValue;

    /* SWAP_CFG */
    SPI_SwapMode            m_EnRxSwap;
    SPI_SwapMode            m_EnTxSwap;

    /* FB_CLK_SEL */
    SPI_feedbackClock       m_FBClkSel;            // Select Feedback Clock

    /*  Drive Strength */
    uint32_t                m_DrvStrength;        // Drive Strength
    bool                    m_EnSWReset;        // Reset in InitializeREG()

    bool                    m_loopback;        // self loopback mode

    /*  DMA */
    uint32_t                m_pdma_ch;
    uint32_t                m_DmaTxReqSrc;
    uint32_t                m_DmaRxReqSrc;

    /*  ISR */
    SPI_Callback_t          m_RxDMACompleteCallback;
    uint32_t                m_RxDMACompleteCallback_Value;

    SPI_Callback_t          m_TxDMACompleteCallback;
    uint32_t                m_TxDMACompleteCallback_Value;

    SPI_Callback_t          m_TrailingINT_Callback;
    uint32_t                m_TrailingINT_Value;

    SPI_Callback_t          m_RxOverrunINT_Callback;
    uint32_t                m_RxOverrunINT_Value;

    SPI_Callback_t          m_RxUnderrunINT_Callback;
    uint32_t                m_RxUnderrunINT_Value;

    SPI_Callback_t          m_TxOverrunINT_Callback;
    uint32_t                m_TxOverrunINT_Value;

    SPI_Callback_t          m_TxUnderrunINT_Callback;
    uint32_t                m_TxUnderrunINT_Value;

    SPI_Callback_t          m_RxFIFORdyINT_Callback;
    uint32_t                m_RxFIFORdyINT_Value;

    SPI_Callback_t          m_TxFIFORdyINT_Callback;
    uint32_t                m_TxFIFORdyINT_Value;

    SPI_Callback_t          m_RxIntCompleteCallback;
    uint32_t                m_RxIntCompleteCallback_Value;

    SPI_Callback_t          m_TxIntCompleteCallback;
    uint32_t                m_TxIntCompleteCallback_Value;

    void    *txbuf;
    int     tx_size;
    int     tx_cur_ptr;
    void    *rxbuf;
    int     rx_size;
    int     rx_cur_ptr;
};
typedef struct spi_device spi_device_t;

struct spi_board_info {
    /* the device name and module name are coupled, like platform_bus;
     * "modalias" is normally the driver name.
     *
     * platform_data goes to spi_device.dev.platform_data,
     * controller_data goes to spi_device.controller_data,
     * irq is copied too
     */
    const char  *name;
    /* slower signaling on noisy or low voltage boards */
    uint32_t        max_speed_hz;
    uint32_t        mode;
    uint32_t        xfer_mode;
    uint32_t        bit_width;
    uint32_t        burst_size;
    /* bus_num is board specific and matches the bus_num of some
     * spi_master that will probably be registered later.
     *
     * chip_select reflects how this chip is wired to that master;
     * it's less than num_chipselect.
     */
    uint16_t     bus_num;
    uint16_t     chip_select;

    /* mode becomes spi_device.mode, and is essential for chips
     * where the default of SPI_CS_HIGH = 0 is wrong.
     */

};

void spiDrvInterruptHandler(uint32_t portNum);
int spiOpen(uint32_t portNum, struct spi_device *spiCfg);
void spiSetDeviceInfo(uint32_t portNum, const struct spi_device *spi_dev);
int spiDataRead(uint32_t portNum, uint32_t *buf, uint32_t size);
int spiDataWrite(uint32_t portNum, uint32_t *buf, uint32_t size);
int spiDataWriteRead(uint32_t portNum, uint32_t *txbuf, uint32_t *rxbuf, uint32_t size);
void spiDrvGetInfo(uint32_t portNum, const struct spi_device *spi_dev);
#endif

