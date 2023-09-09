/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spiDrv.c
 *      Purpose: To implement SPI driver functions
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

#include <plat/spi/spi.h>
#include <spi_priv.h>
#include <spiSfrBase.h>
#include <spiDrv.h>
#include <cmu.h>
#include <string.h>
#include <rtc.h>
#include <cmsis.h>

#define REG_CH_CFG             (0X00)
#define REG_MODE_CFG           (0x08)
#define REG_CS_REG             (0x0C)
#define REG_SPI_INT_EN         (0x10)
#define REG_SPI_STATUS         (0x14)
#define REG_SPI_TX_DATA        (0x18)
#define REG_SPI_RX_DATA        (0x1C)
#define REG_PACKET_CNT_REG     (0x20)
#define REG_PENDING_CLR_REG    (0x24)
#define REG_SWAP_CFG           (0x28)
#define REG_FB_CLK_SEL         (0x2C)

/* REG_CH_CFG */
#define EXYNOS_SPI_HIGH_SPEED     (1 << 6)
#define EXYNOS_SPI_FIFO_FLUSH     (1 << 5)
#define EXYNOS_SPI_SLAVE          (1 << 4)
#define EXYNOS_SPI_CPOL_ACTLOW    (1 << 3)
#define EXYNOS_SPI_CPHA_FORMB     (1 << 2)
#define EXYNOS_SPI_RX_ON          (1 << 1)
#define EXYNOS_SPI_TX_ON          (1 << 0)

/* REG_MODE_CFG */
#define EXYNOS_SPI_CH_WIDTH_MASK        0x3
#define EXYNOS_SPI_CH_WIDTH_OFFSET      29
#define EXYNOS_SPI_TRAILING_CNT_MAX     0x3ff
#define EXYNOS_SPI_TRAILING_CNT_MASK    EXYNOS_SPI_TRAILING_CNT_MAX
#define EXYNOS_SPI_TRAILING_CNT_OFFSET  19
#define EXYNOS_SPI_BUS_WIDTH_MASK       0x3
#define EXYNOS_SPI_BUS_WIDTH_OFFSET     17
#define EXYNOS_SPI_RXRDYLVL_MASK        0x3F
#define EXYNOS_SPI_RXRDYLVL_OFFSET      11
#define EXYNOS_SPI_TXRDYLVL_MASK        0x3F
#define EXYNOS_SPI_TXRDYLVL_OFFSET      5
#define EXYSNOS_SPI_SELF_LOOPBACK_ON    (1<<3)
#define EXYNOS_SPI_RX_DMA_ON            (1 << 2)
#define EXYNOS_SPI_TX_DMA_ON            (1 << 1)
#define EXYNOS_SPI_DMA_4BURST           (1 << 0)

/* REG_CS_REG */
#define EXYNOS_SPI_NCSCOUNT_MASK        0x3F
#define EXYNOS_SPI_NCSCOUNT_OFFSET      4
#define EXYNOS_AUTO_N_MANUAL            (1<<1)
#define EXYNOS_NSSOUT                   (1<<0)

/* REG_PACKET_CNT_REG */
#define EXYNOS_SPI_PACKET_COUNT_EN      (1 << 16)
#define EXYNOS_SPI_COUNT_MASK           (0xFFFF)


#define TRAILING        (1 << 24)
#define RX_OVERRUN      (1 << 5)
#define RX_UNDERRUN     (1 << 4)
#define TX_OVERRUN      (1 << 3)
#define TX_UNDERRUN     (1 << 2)
#define RX_FIFO_RDY     (1 << 1)
#define TX_FIFO_RDY     (1 << 0)
#define TX_DONE         (1 << 25)

#define TX_FIFO_MAX     (0x3F)
#define RX_FIFO_MAX     (0x3F)

#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif

#define SRC_VA(channel)        (0x10000000 + 0x10000000 * channel)
#define DST_VA(channel)        (0x60000000 + 0x10000000 * channel)
#define resource_dma_req(r) (r)->start
#define resource_dma_name(r) (char *)((r)->node_name)

struct spi_device mSpiDev[SPI_CHANNEL_MAX];

static void __wait_until_tx_done(uint32_t base);
static uint32_t mSpiTempBuf[SPI_FIFO_LENGTH];
static void dumpSpi(uint32_t base);

static void spiDrvDumpSFR(uint32_t portNum)
{
#if defined(I2CSPI_SFRDUMP_REQUIRED)
    struct spi_device *spi;
    unsigned int regValue;

    spi =  &mSpiDev[portNum];
    CSP_PRINTF_INFO("\nspiDrvDumpSFR\n#############################################\n");

    regValue = __raw_read32(spi->base + REG_CH_CFG);
    CSP_PRINTF_INFO("SPI REG_CH_CFG(0x%x) : %x\n", (unsigned int)(spi->base + REG_CH_CFG), regValue);

    regValue = __raw_read32(spi->base + REG_MODE_CFG);
    CSP_PRINTF_INFO("SPI REG_MODE_CFG(0x%x) : %x\n", (unsigned int)(spi->base + REG_MODE_CFG), regValue);

     regValue = __raw_read32(spi->base + REG_CS_REG);
    CSP_PRINTF_INFO("SPI REG_CS_REG(0x%x) : %x\n", (unsigned int)(spi->base + REG_CS_REG), regValue);

     regValue = __raw_read32(spi->base + REG_SPI_INT_EN);
    CSP_PRINTF_INFO("SPI REG_SPI_INT_EN(0x%x) : %x\n", (unsigned int)(spi->base + REG_SPI_INT_EN), regValue);

     regValue = __raw_read32(spi->base + REG_SPI_STATUS);
    CSP_PRINTF_INFO("SPI REG_SPI_STATUS(0x%x) : %x\n", (unsigned int)(spi->base + REG_SPI_STATUS), regValue);

     regValue = __raw_read32(spi->base + REG_SPI_TX_DATA);
    CSP_PRINTF_INFO("SPI REG_SPI_TX_DATA(0x%x) : %x\n", (unsigned int)(spi->base + REG_SPI_TX_DATA), regValue);

     regValue = __raw_read32(spi->base + REG_SPI_RX_DATA);
    CSP_PRINTF_INFO("SPI REG_SPI_RX_DATA(0x%x) : %x\n", (unsigned int)(spi->base + REG_SPI_RX_DATA), regValue);

     regValue = __raw_read32(spi->base + REG_PACKET_CNT_REG);
    CSP_PRINTF_INFO("SPI REG_PACKET_CNT_REG(0x%x) : %x\n", (unsigned int)(spi->base + REG_PACKET_CNT_REG), regValue);

     regValue = __raw_read32(spi->base + REG_PENDING_CLR_REG);
    CSP_PRINTF_INFO("SPI REG_PENDING_CLR_REG(0x%x) : %x\n", (unsigned int)(spi->base + REG_PENDING_CLR_REG), regValue);

     regValue = __raw_read32(spi->base + REG_SWAP_CFG);
    CSP_PRINTF_INFO("SPI REG_SWAP_CFG(0x%x) : %x\n", (unsigned int)(spi->base + REG_SWAP_CFG), regValue);

     regValue = __raw_read32(spi->base + REG_FB_CLK_SEL);
    CSP_PRINTF_INFO("SPI REG_FB_CLK_SEL(0x%x) : %x\n", (unsigned int)(spi->base + REG_FB_CLK_SEL), regValue);

    CSP_PRINTF_INFO("\n#############################################\n");

    CSP_PRINTF_INFO("SPI base : %x\n", (unsigned int)(spi->base));
    CSP_PRINTF_INFO("SPI m_portId : %x\n", (unsigned int)(spi->m_portId));
    CSP_PRINTF_INFO("SPI m_EnHighSpeed : %x\n", (unsigned int)(spi->m_EnHighSpeed));
    CSP_PRINTF_INFO("SPI m_OpMode : %x\n", (unsigned int)(spi->m_OpMode));
    CSP_PRINTF_INFO("SPI m_CPOL : %x\n", (unsigned int)(spi->m_CPOL));
    CSP_PRINTF_INFO("SPI m_CPHA : %x\n", (unsigned int)(spi->m_CPHA));
    CSP_PRINTF_INFO("SPI m_EnRxChannel : %x\n", (unsigned int)(spi->m_EnRxChannel));
    CSP_PRINTF_INFO("SPI m_EnTxChannel : %x\n", (unsigned int)(spi->m_EnTxChannel));
    CSP_PRINTF_INFO("SPI m_EnClk : %x\n", (unsigned int)(spi->m_EnClk));
    CSP_PRINTF_INFO("SPI m_OpClock : %x\n", (unsigned int)(spi->m_OpClock));
    CSP_PRINTF_INFO("SPI m_TargetSPICLK : %x\n", (unsigned int)(spi->m_TargetSPICLK));
    CSP_PRINTF_INFO("SPI m_ChWidth : %x\n", (unsigned int)(spi->m_ChWidth));
    CSP_PRINTF_INFO("SPI m_BusWidth : %x\n", (unsigned int)(spi->m_BusWidth));
    CSP_PRINTF_INFO("SPI m_TraillingCnt : %x\n", (unsigned int)(spi->m_TraillingCnt));
    CSP_PRINTF_INFO("SPI m_RxReadyLevel : %x\n", (unsigned int)(spi->m_RxReadyLevel));
    CSP_PRINTF_INFO("SPI m_TxReadyLevel : %x\n", (unsigned int)(spi->m_TxReadyLevel));
    CSP_PRINTF_INFO("SPI m_RxTransferMode : %x\n", (unsigned int)(spi->m_RxTransferMode));
    CSP_PRINTF_INFO("SPI m_TxTransferMode : %x\n", (unsigned int)(spi->m_TxTransferMode));
    CSP_PRINTF_INFO("SPI m_MaxFIFOSize : %x\n", (unsigned int)(spi->m_MaxFIFOSize));
    CSP_PRINTF_INFO("SPI m_FifoLevelGrid : %x\n", (unsigned int)(spi->m_FifoLevelGrid));
#endif
}


static void spiDrvInitCtrl(uint32_t base, int on, uint32_t src)
{
    uint32_t val;

    /* Enable interrupt */
    val = __raw_readl(base + REG_SPI_INT_EN);
    val &= ~src;
    if (on)
        val |= src;
    __raw_writel(val, base + REG_SPI_INT_EN);
}

static void spiDrvSetChannel(struct spi_device *spi)
{
    uint32_t val;

    val = __raw_readl(spi->base + REG_CH_CFG);

    if (spi->m_OpMode == SPI_MASTER)
        val &= (uint32_t)~EXYNOS_SPI_SLAVE;
    else
        val |= EXYNOS_SPI_SLAVE;

    if (spi->m_CPOL == SPI_ACTIVE_HIGH)
        val &= (uint32_t)~EXYNOS_SPI_CPOL_ACTLOW;
    else
        val |= EXYNOS_SPI_CPOL_ACTLOW;

    if (spi->m_CPHA == SPI_FORMAT_A)
        val &= (uint32_t)~EXYNOS_SPI_CPHA_FORMB;
    else
        val |= (uint32_t)EXYNOS_SPI_CPHA_FORMB;

    __raw_writel(val, spi->base + REG_CH_CFG);
    while ((__raw_readl(spi->base + REG_CH_CFG) & 1) != (val & 1));

}

static void spiDrvRxTxChannelOn(uint32_t base, bool RxOn, bool TxOn)
{
    uint32_t val;

    val = __raw_readl(base + REG_CH_CFG);

    val &= (uint32_t)(~(EXYNOS_SPI_RX_ON | EXYNOS_SPI_TX_ON));
    if(RxOn)
        val |= EXYNOS_SPI_RX_ON;
    if(TxOn)
        val |= EXYNOS_SPI_TX_ON;

    __raw_writel(val, base + REG_CH_CFG);
}

static void spiDrvFifoFlush(uint32_t base)
{
    volatile int val;

    /* RxTx channel should be disabled before flushing fifo */
    spiDrvRxTxChannelOn(base, false, false);
    __raw_writel(0x0, base + REG_PACKET_CNT_REG);

    /* fifo flush(SW reset)  enable */
    val = __raw_readl(base + REG_CH_CFG);
    val |= EXYNOS_SPI_FIFO_FLUSH;
    __raw_writel(val, base + REG_CH_CFG);

    do{
        val = __raw_readl(base + REG_SPI_STATUS);
    }while(val&(EXYNOS_SPI_RXRDYLVL_MASK << EXYNOS_SPI_RXRDYLVL_OFFSET));

    do{
        val = __raw_readl(base + REG_SPI_STATUS);
    }while(val&(EXYNOS_SPI_TXRDYLVL_MASK << EXYNOS_SPI_TXRDYLVL_OFFSET));

    /* fifo flush(SW reset) disable */
    val = __raw_readl(base + REG_CH_CFG);
    val &= (uint32_t)~EXYNOS_SPI_FIFO_FLUSH;
    __raw_writel(val, base + REG_CH_CFG);

    do {
        val = __raw_readl(base + REG_CH_CFG);
    } while (val & EXYNOS_SPI_FIFO_FLUSH);
}

static void spiDrvEnablePktcount(uint32_t base, bool on, uint16_t size)
{
    uint32_t val;

    /* Disable PACKET_CNT_REG before setting COUNT_VALUE */
    val =  __raw_readl(base + REG_PACKET_CNT_REG);
    val &= ~EXYNOS_SPI_PACKET_COUNT_EN;
    __raw_writel(val, base + REG_PACKET_CNT_REG);

    val &= ~EXYNOS_SPI_COUNT_MASK;
    val |= (size&EXYNOS_SPI_COUNT_MASK);
    if (on) val |= EXYNOS_SPI_PACKET_COUNT_EN ;
    __raw_writel(val, base + REG_PACKET_CNT_REG);
}


static void spiDrvSetMode(struct spi_device *spi)
{
    uint32_t val;

    val =  __raw_readl(spi->base + REG_MODE_CFG);

    if (spi->m_RxTransferMode == SPI_DMA_MODE)
        val |= EXYNOS_SPI_RX_DMA_ON;
    else
        val &= (uint32_t)~EXYNOS_SPI_RX_DMA_ON;

    if (spi->m_TxTransferMode == SPI_DMA_MODE)
        val |= EXYNOS_SPI_TX_DMA_ON;
    else
        val &= (uint32_t)~EXYNOS_SPI_TX_DMA_ON;

    if (spi->m_DMAType == SPI_DMA_4_BURST)
        val |= EXYNOS_SPI_DMA_4BURST;
    else
        val &= (uint32_t)~EXYNOS_SPI_DMA_4BURST;

    val &= (uint32_t)~(EXYNOS_SPI_CH_WIDTH_MASK << EXYNOS_SPI_CH_WIDTH_OFFSET);
    val |= spi->m_ChWidth << EXYNOS_SPI_CH_WIDTH_OFFSET;

    val &= (uint32_t)(EXYNOS_SPI_TRAILING_CNT_MASK << EXYNOS_SPI_TRAILING_CNT_OFFSET);
    if(spi->m_TraillingCnt)
        val |= spi->m_TraillingCnt << EXYNOS_SPI_TRAILING_CNT_OFFSET;
    else
        val |= EXYNOS_SPI_TRAILING_CNT_MAX << EXYNOS_SPI_TRAILING_CNT_OFFSET;

    val &= (uint32_t)~(EXYNOS_SPI_BUS_WIDTH_MASK << EXYNOS_SPI_BUS_WIDTH_OFFSET);
    val |= spi->m_BusWidth << EXYNOS_SPI_BUS_WIDTH_OFFSET;

    val &= (uint32_t)~(0x3F << EXYNOS_SPI_TXRDYLVL_OFFSET);
    val |= ((spi->m_TxReadyLevel & 0x3F) << EXYNOS_SPI_TXRDYLVL_OFFSET);
    val &= (uint32_t)~(0x3F << EXYNOS_SPI_RXRDYLVL_OFFSET);
    val |= ((spi->m_RxReadyLevel & 0x3F) << EXYNOS_SPI_RXRDYLVL_OFFSET);

    if (spi->m_loopback == true)
        val |= (uint32_t)(1<<3);
    else
        val &= (uint32_t)~(1<<3);


    __raw_writel(val, spi->base + REG_MODE_CFG);
}

static void spiDrvSetSelfloopback(uint32_t base, bool on)
{
    uint32_t val;

    SPI_DEBUG_PRINT("spiDrvSetSelfloopback base: 0x%x, on: %d\n", (unsigned int)base, (unsigned int)on);
    val =  __raw_readl(base + REG_MODE_CFG);

    if(on){
        val |= EXYSNOS_SPI_SELF_LOOPBACK_ON;
    }
    else{
        val &= (uint32_t)~(EXYSNOS_SPI_SELF_LOOPBACK_ON);
    }

    __raw_writel(val, base + REG_MODE_CFG);
}

static void spiDrvSetCs(struct spi_device *spi)
{
    uint32_t val;

    val =  __raw_readl(spi->base + REG_CS_REG);

    if (spi->m_EnAutoCS)
        val |= EXYNOS_AUTO_N_MANUAL;
    else
        val &= ~EXYNOS_AUTO_N_MANUAL;

    val &= (uint32_t)~(EXYNOS_SPI_NCSCOUNT_MASK<<EXYNOS_SPI_NCSCOUNT_OFFSET);
    val |= (uint32_t)(spi->m_NCSTimeCnt << EXYNOS_SPI_NCSCOUNT_OFFSET);

    __raw_writel(val, spi->base + REG_CS_REG);
}


static void spiDrvSetSwap(struct spi_device *spi)
{
    uint32_t val;

    val = (spi->m_EnRxSwap << 4) | (spi->m_EnTxSwap);

    __raw_writel(val, spi->base + REG_SWAP_CFG);
}


static void spiDrvSetFbclk(struct spi_device *spi)
{
    uint32_t val;

    val = spi->m_FBClkSel;

    __raw_writel(val, spi->base + REG_FB_CLK_SEL);
}


static void spiDrvSetErrInt(uint32_t base)
{
    uint32_t val;

    val =  __raw_readl(base + REG_SPI_INT_EN);

    val |= 0x68;

    __raw_writel(val, base + REG_SPI_INT_EN);
}



static void spiDrvClearPending(uint32_t base)
{
    uint32_t val;

    val = (uint32_t)0x1F;

    __raw_writel(val, base + REG_PENDING_CLR_REG);
}




static int spiDrvEnableCs(uint32_t base, bool is_enable)
{
    uint32_t val;

    val =  __raw_readl(base + REG_CS_REG);

    if (is_enable)
        val &= (uint32_t)~EXYNOS_NSSOUT;
    else
        val |= (uint32_t)EXYNOS_NSSOUT;

    __raw_writel(val, base + REG_CS_REG);

    return 0;
}

/* function ready for interrupt mode */
static void spiDrvIntPendingClear(struct spi_device *spi, uint32_t clear_int)
{
    __raw_writel(clear_int, spi->base + REG_PENDING_CLR_REG);
}

static uint32_t spiDrvIntRRxOverrun(struct spi_device *spi);
static uint32_t spiDrvIntRRxOverrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Rx Fifo Over Run!!\n", spi->m_portId);
    spi->m_RxOverrunINT_Value = true;//TRUE;

    spiDrvIntPendingClear(spi, SPI_CLR_PND_RX_OVERRUN);
    return 0;
}

static uint32_t spiDrvIntRxUnderrun(struct spi_device *spi);
static uint32_t spiDrvIntRxUnderrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Rx Fifo Under Run!!\n", spi->m_portId);
    spi->m_RxUnderrunINT_Value = true;//TRUE;

    spiDrvIntPendingClear(spi, SPI_CLR_PND_RX_UNDERRUN);

    return 0;
}

static uint32_t spiDrvIntTxOverrun(struct spi_device *spi);
static uint32_t spiDrvIntTxOverrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Tx Fifo Over Run!!\n", spi->m_portId);
    spi->m_TxOverrunINT_Value = true;//TRUE;

    spiDrvIntPendingClear(spi, SPI_CLR_PND_TX_OVERRUN);
    return 0;
}

static uint32_t spiDrvIntTxUnderrun(struct spi_device *spi);
static uint32_t spiDrvIntTxUnderrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Tx Fifo Under Run!!\n", spi->m_portId);
    spi->m_TxUnderrunINT_Value = true;//TRUE;

    spiDrvIntPendingClear(spi, SPI_CLR_PND_TX_UNDERRUN);

    return 0;
}

static uint32_t spiDrvIntTrailByte(struct spi_device *spi);
static uint32_t spiDrvIntTrailByte(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Tx Fifo Under Run!!\n", spi->m_portId);
    spi->m_TrailingINT_Value = true;//TRUE;

    spiDrvIntPendingClear(spi, SPI_CLR_PND_TRAILING);

    return 0;
}

/*
void __exynos_spi_txfifo_rdy(void *var0)
{
    struct spi_device *spi = (struct spi_device *)var0; // Get only Port number!

    DBG_MSG("\n[ISR]SPI Port%d Tx Fifo Ready!!\n", spi->m_portId);

}


void __exynos_spi_rxfifo_rdy(void *var0)
{
    struct spi_device *spi = (struct spi_device *)var0; // Get only Port number!

    DBG_MSG("\n[ISR]SPI Port%d Rx Fifo Ready!!\n", spi->m_portId);

}
*/

static uint32_t spiDrvIntWrite(struct spi_device *spi);
static uint32_t spiDrvIntWrite(struct spi_device *spi)
{
    void *buf;
    int size, cnt;
    //uint32_t regVal;
    buf = spi->txbuf;
    size = spi->tx_size;
    cnt = 0;

    //osLog(LOG_INFO, "spiDrvIntWrite");
    //
    /* Disable Interrupt */
    spiDrvInitCtrl(spi->base, DISABLE, TX_FIFO_RDY);

    size = size - spi->tx_cur_ptr;
    if (size > TX_FIFO_MAX)
        size = TX_FIFO_MAX;

    spiDrvEnablePktcount(spi->base, true, size);
    spiDrvRxTxChannelOn(spi->base, true, true);
    spiDrvEnableCs(spi->base, true);

    if (spi->m_BusWidth == SPI_BYTE) {
        while (cnt < size) {
            __raw_writel(((uint8_t *)buf)[spi->tx_cur_ptr++], spi->base + REG_SPI_TX_DATA);
            cnt++;
        }
    } else if (spi->m_BusWidth == SPI_HWORD) {
        if (size % sizeof(uint16_t) != 0) {
            //cprintf("\nTx Size(%d) isn't aligned by 2", size);

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0x01) { // Tx FIFO Ready
                __raw_writel(*(uint16_t *)buf, spi->base + REG_SPI_TX_DATA);
                buf += sizeof(uint16_t);
                size -= 2;
            }

        }
    } else if (spi->m_BusWidth == SPI_WORD) {
        if (size % sizeof(uint32_t) != 0) {
            //cprintf("\nTx Size(%d) isn't aligned by 4", size);

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0x01) { // Tx FIFO Ready
                __raw_writel(*(uint32_t *)buf, spi->base + REG_SPI_TX_DATA);

                buf += sizeof(uint32_t);
                size -= 4;
            }
        }
    }

    //if (spi->m_OpMode == SPI_MASTER && size != spi->tx_cur_ptr)   // eun00 : No check TX done if size is 0.
    //    __wait_until_tx_done(spi->base);

    if (spi->tx_cur_ptr == size) {
        //osLog(LOG_INFO, "spiDrvIntWrite size%d ptr%d 0x%X\n", size, spi->tx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));
        spi->m_TxIntCompleteCallback_Value = true;//TRUE;

    } else {
        //osLog(LOG_INFO, "spiDrvIntWrite size%d ptr%d 0x%X\n", size, spi->tx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));
    }

    return 0;
}

static uint32_t spiDrvIntRead(struct spi_device *spi);
static uint32_t spiDrvIntRead(struct spi_device *spi)
{
    void *buf;
    int size, cnt;
    uint32_t regVal;

    //osLog(LOG_INFO, "spiDrvIntRead\n");

    buf = spi->rxbuf;
    size = spi->rx_size;
    cnt = 0;

    if (spi->m_BusWidth == SPI_BYTE) {
        while (cnt < spi->m_RxReadyLevel) {
            ((uint8_t *)buf)[spi->rx_cur_ptr++] = __raw_readl(spi->base + REG_SPI_RX_DATA);
            cnt++;
        }
    } else if (spi->m_BusWidth == SPI_HWORD) {
        if (size % sizeof(uint16_t) != 0) {
            //cprintf("\nTx Size(%d) isn't aligned by 2", size);

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0xFF8000) { // RX_FIFO_LVL[23:15]
                *(uint16_t *)buf = __raw_readl(spi->base + REG_SPI_RX_DATA);

                buf += sizeof(uint16_t);
                size -= 2;
            }
        }
    } else if (spi->m_BusWidth == SPI_WORD) {
        if (size % sizeof(uint32_t) != 0) {
            //cprintf("\nTx Size(%d) isn't aligned by 4", size);

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0xFF8000) { // RX_FIFO_LVL[23:15]
                *(uint32_t *)buf = __raw_readl(spi->base + REG_SPI_RX_DATA);

                buf += sizeof(uint32_t);
                size -= 4;
            }
        }
    }

    if (spi->rx_cur_ptr == size) {
        //osLog(LOG_INFO, "spiDrvIntRead size%d ptr%d, 0x%X\n", size, spi->rx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));

        /* Disable Interrupt */
        spiDrvInitCtrl(spi->base, DISABLE, RX_FIFO_RDY);
        spiDrvEnableCs(spi->base, false);

        //__wait_until_tx_done(spi->base);
        //spiDrvFifoFlush(spi->base);

        spi->m_RxIntCompleteCallback_Value = true;//TRUE;

        spiMasterRxTxDone(spi->dev,0);
    } else {
        //osLog(LOG_INFO, "spiDrvIntRead size%d ptr%d, 0x%X\n", size, spi->rx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));

        //__wait_until_tx_done(spi->base);
        spi->m_RxReadyLevel = size - spi->rx_cur_ptr;
        if (spi->m_RxReadyLevel > RX_FIFO_MAX)
            spi->m_RxReadyLevel = RX_FIFO_MAX;
        regVal = __raw_readl(spi->base + REG_MODE_CFG);
        regVal &= (uint32_t)~(0x3F << EXYNOS_SPI_RXRDYLVL_OFFSET);
        regVal |= ((spi->m_RxReadyLevel & 0x3F) << EXYNOS_SPI_RXRDYLVL_OFFSET);
        __raw_writel(regVal, spi->base + REG_MODE_CFG);

        /* Enable Interrupt */
        spiDrvInitCtrl(spi->base, ENABLE, TX_FIFO_RDY);
    }
        
    return 0;
}

void spiDrvInterruptHandler(uint32_t portNum)
{
    struct spi_device *spi = &mSpiDev[portNum];
    uint32_t spiStatus, spiIntEn, val;

    spiStatus = __raw_readl(spi->base + REG_SPI_STATUS);
    spiIntEn = __raw_readl(spi->base + REG_SPI_INT_EN);

    //osLog(LOG_INFO, "STATUS 0x%X INT_EN 0x%X MOD 0x%X\n", (unsigned int)spiStatus, (unsigned int)spiIntEn, __raw_readl(spi->base + REG_MODE_CFG));
    val = spiStatus & spiIntEn;

    if (val & RX_FIFO_RDY)
        spiDrvIntRead(spi);

    if (val & TX_FIFO_RDY)
        spiDrvIntWrite(spi);

    if (val & RX_OVERRUN)
        spiDrvIntRRxOverrun(spi);

    if (val & RX_UNDERRUN)
        spiDrvIntRxUnderrun(spi);

    if (val & TX_UNDERRUN)
        spiDrvIntTxUnderrun(spi);

    if (val & TX_OVERRUN)
        spiDrvIntTxOverrun(spi);

    /* Trailing Byte interrupt always occur. so this interrupt source needs to be checked after RX_FIFO_READY. */
    if (val & TRAILING)
        spiDrvIntTrailByte(spi);

}

static void dumpSpi(uint32_t base)
{
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_CH_CFG, __raw_readl(base + REG_CH_CFG));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_MODE_CFG, __raw_readl(base + REG_MODE_CFG));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_CS_REG, __raw_readl(base + REG_CS_REG));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_SPI_INT_EN, __raw_readl(base + REG_SPI_INT_EN));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_SPI_STATUS, __raw_readl(base + REG_SPI_STATUS));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_SPI_TX_DATA, __raw_readl(base + REG_SPI_TX_DATA));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_SPI_RX_DATA, __raw_readl(base + REG_SPI_RX_DATA));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_PACKET_CNT_REG, __raw_readl(base + REG_PACKET_CNT_REG));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_PENDING_CLR_REG, __raw_readl(base + REG_PENDING_CLR_REG));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_SWAP_CFG, __raw_readl(base + REG_SWAP_CFG));
    osLog(LOG_INFO, "%s: +%d: 0x%x\n", __func__, REG_FB_CLK_SEL, __raw_readl(base + REG_FB_CLK_SEL));
}

#define SPI_RETRY_MAX (10000)
static void __wait_until_tx_done(uint32_t base)
{
    uint32_t val;
    int loop = 0;
    u64 time = rtcGetTime();

    //Delay(1);
    do {
        val = __raw_readl(base + REG_SPI_STATUS);

        if (val & TX_DONE)
            break;

        if (loop++ > SPI_RETRY_MAX) {
            if (rtcGetTime() - time > 10000000) {
                osLog(LOG_ERROR, "%s: out! inTime:%llu, outTime:%llu\n", __func__, time, rtcGetTime());
                dumpSpi(base);
                break;
            }
        }

    } while (1);
}

void spiDrvSetDeviceInfo(uint32_t portNum, const struct spi_device *spi_dev)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];
    SPI_DEBUG_PRINT("spiDrvSetDeviceInfo\n");

    spi->m_portId = spi_dev->m_portId;
    spi->m_EnAutoCS = spi_dev->m_EnAutoCS;
    spi->m_TargetSPICLK = spi_dev->m_TargetSPICLK;
    spi->m_OpMode = spi_dev->m_OpMode;
    spi->m_CPOL = spi_dev->m_CPOL;
    spi->m_CPHA = spi_dev->m_CPHA;
    spi->m_OpClock = spi_dev->m_OpClock;
    spi->m_ChWidth = spi_dev->m_ChWidth;
    spi->m_BusWidth = spi_dev->m_BusWidth;
    spi->m_RxTransferMode = spi_dev->m_RxTransferMode;
    spi->m_TxTransferMode = spi_dev->m_TxTransferMode;
    spi->m_FBClkSel = spi_dev->m_FBClkSel;
}

static int spiDrvSetup(uint32_t portNum)
{
    struct spi_device *spi;
    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDrvSetup\n");

    if ((spi->m_TxTransferMode == SPI_DMA_MODE) || (spi->m_RxTransferMode == SPI_DMA_MODE)) {
    }

    //NEED TO BE DOUBLE CHECK!!! HOW TO CONFIG TxReadyLevel
    //spi->m_TxReadyLevel = 1;

    spiDrvSetChannel(spi);

    spiDrvSetMode(spi);
    spiDrvSetCs(spi);
    spiDrvSetSwap(spi);
    spiDrvSetFbclk(spi);

    if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE || spi->m_RxTransferMode == SPI_INTERRUPT_MODE) {
        spiDrvSetErrInt(spi->base);
    }

    spiDrvClearPending(spi->base);

    if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE || spi->m_RxTransferMode == SPI_INTERRUPT_MODE) {
        spi->m_RxOverrunINT_Value = 0;
        spi->m_TxOverrunINT_Value = 0;
        spi->m_RxUnderrunINT_Value = 0;
        spi->m_TxUnderrunINT_Value = 0;
        spi->m_RxFIFORdyINT_Value = 0;
        spi->m_TxFIFORdyINT_Value = 0;
        spi->m_TxIntCompleteCallback_Value = 0;
        spi->m_RxIntCompleteCallback_Value = 0;
    }

#if 0/*HACK: Mask SPI interrupt to avoid unnecessary CM4 clock request*/
    if(spi->m_TxTransferMode == SPI_POLLING_MODE){
        if(portNum==0 || portNum==1)
            sysregSetMaskIrq((uint32_t)(1<<portNum));
        else
            CSP_PRINTF_ERROR("%s: Need further implementation",__FUNCTION__);

    }
#endif

    spiDrvFifoFlush(spi->base);

    return 0;
}

int spiDrvOpen(uint32_t portNum, struct spi_device *spiCfg)
{
    (void)spiCfg;

    SPI_DEBUG_PRINT("spiDrvOpen portNum: %d\n", (unsigned int)portNum);

    mSpiDev[portNum].base = mSpiSfrBase[portNum];
    mSpiDev[portNum].m_portId = portNum;
    mSpiDev[portNum].dev = spiCfg->dev;
    //mSpiDev[portNum].m_TraillingCnt = 0x100;
    mSpiDev[portNum].m_TxReadyLevel = 1;
    //mSpiDev[portNum].m_TxReadyLevel = spiCfg->tx_size;
    mSpiDev[portNum].m_RxReadyLevel = spiCfg->rx_size;
    //if ( spiCfg->tx_size > TX_FIFO_MAX)
        //mSpiDev[portNum].m_TxReadyLevel = TX_FIFO_MAX;
    if ( spiCfg->rx_size >= RX_FIFO_MAX)
        mSpiDev[portNum].m_RxReadyLevel = RX_FIFO_MAX;

    //osLog(LOG_INFO, "spiDrvOpen %d %d\n", spiCfg->tx_size, spiCfg->tx_size);

    if(spiDrvSetup(portNum))
        return -1;

    return 0;
}


// this is for only loopback test
static int spiDrvChannelOn(uint32_t portNum)
{
    uint32_t val;
    struct spi_device *spi;

    SPI_DEBUG_PRINT("spiDrvChannelOn portNum: %d\n", (unsigned int)portNum);
    spi =  &mSpiDev[portNum];

    val = __raw_readl(spi->base + REG_CH_CFG);

    val &= (uint32_t)~0x03;
    val |= (spi->m_EnRxChannel << 1) | (spi->m_EnTxChannel << 0);

    __raw_writel(val, spi->base + REG_CH_CFG);
    while ((__raw_readl(spi->base + REG_CH_CFG) & 1) != (val & 1));
    return 0;
}

static int spiDrvPollingRead(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;
    uint32_t *tmpRxBuf32;
    uint16_t *tmpRxBuf16;
    uint8_t *tmpRxBuf8;
    uint32_t tmpSize;

    spi =  &mSpiDev[portNum];

    if (spi->m_BusWidth == SPI_BYTE) {
        tmpRxBuf8 = (uint8_t *)buf;
        tmpSize = size;
        while(tmpSize){
            *tmpRxBuf8++ = __raw_read8(spi->base + REG_SPI_RX_DATA);
            tmpSize--;
        }
    }
    else if (spi->m_BusWidth == SPI_HWORD) {
        tmpRxBuf16 = (uint16_t *)buf;
        tmpSize = size;
        while(tmpSize){
            *tmpRxBuf16++ = __raw_read16(spi->base + REG_SPI_RX_DATA);
            tmpSize--;
        }
    }
    else if (spi->m_BusWidth == SPI_WORD) {
        tmpRxBuf32 = (uint32_t *)buf;
        tmpSize = size;
        while(tmpSize){
            *tmpRxBuf32++ = __raw_readl(spi->base + REG_SPI_RX_DATA);
            tmpSize--;
        }
    }

    return 0;
}

static int spiDrvInterruptRead(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    spi->rxbuf = buf;
    spi->rx_size = size;
    spi->rx_cur_ptr = 0;

    spiDrvInitCtrl(spi->base, ENABLE, RX_FIFO_RDY);

    return 0;
}

#if 0//for future implementation
static int spiDrvDmaRead(uint32_t portNum, void *buf_va, int size)
{
    s16 ch_num;
    uint32_t bs, bl, es;
    char *dma_name;
    DMA_REQ_MAP dma_req;
    uint32_t src_va;
    uint32_t dst_va = *(uint32_t *)buf_va;
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    if (!spi->drvdata->pdma[spi->m_pdma_ch * 2]) {
        ERR_MSG("\n >> ERROR : dma.%d has no channel for SPI %d !!\n", spi->m_pdma_ch, spi->m_portId);
        return -1;
    } else {
        dma_name = resource_dma_name(spi->drvdata->pdma[spi->m_pdma_ch * 2]); //RX
        dma_req = resource_dma_req(spi->drvdata->pdma[spi->m_pdma_ch * 2]);    //RX
    }

    struct platform_device *pdev = platform_device_find_byname(dma_name);
    struct dma_drvdata *dma_drv = platform_get_drvdata(pdev);

    bl = spi->m_DMAType*3; // BL1(0) / BL4(3)
    es = 0; // 0 means NO_SWAP
    bs = spi->m_BusWidth;

    //pm_runtime_get_sync(dma_drv->dev);

    src_va = (uint32_t)(u64)spi->base + REG_SPI_RX_DATA;

    ch_num = dma_transfer_params((struct dma_drvdata *)dma_drv, DMA_P2M, bs, bl, es, dma_req);

    //callback_channel = ch_num;
    //dma_register_callback((struct dma_drvdata *)dma_drv, ch_num, pdma_isr_callback, (void *)&callback_channel);

    dma_transfer_start((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)ch_num, (uint32_t *)(u64)src_va, (uint32_t *)(u64)dst_va, size);

    while (!dma_transfer_done((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)ch_num));

    dma_channel_stop((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)ch_num);

//    dma_unregister_callback((struct dma_drvdata *)dma_drv, ch_num);

    //pm_runtime_put_sync(dma_drv->dev);

    return 0;
}
#endif


static int spiDrvPollingWrite(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;
    uint32_t *tmpTxBuf32;
    uint16_t *tmpTxBuf16;
    uint8_t *tmpTxBuf8;
    uint32_t tmpSize;
    uint32_t loopDelay;

    spi =  &mSpiDev[portNum];

    if (spi->m_BusWidth == SPI_BYTE) {
        if(spi->m_EnAutoCS){
            tmpTxBuf8 = (uint8_t *)buf;
            tmpSize = size;

            while(tmpSize){
                __raw_write8(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiDrvRxTxChannelOn(spi->base, true, true);
        }
        else{
            tmpTxBuf8 = (uint8_t *)buf;
            tmpSize = size;

            spiDrvEnablePktcount(spi->base, false, 0);

            spiDrvRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiDrvEnableCs(spi->base, true);
                __raw_write8(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                __raw_write8(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiDrvEnableCs(spi->base, false);
            }
        }
    }
    else if (spi->m_BusWidth == SPI_HWORD) {
        if(spi->m_EnAutoCS){
            tmpTxBuf16 = (uint16_t *)buf;
            tmpSize = size;

            while(tmpSize){
                __raw_write16(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiDrvRxTxChannelOn(spi->base, true, true);
        }
        else{
            tmpTxBuf16 = (uint16_t *)buf;
            tmpSize = size;

            spiDrvEnablePktcount(spi->base, false, 0);

            spiDrvRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiDrvEnableCs(spi->base, true);
                __raw_write16(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                __raw_write16(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiDrvEnableCs(spi->base, false);
            }
        }
    }
    else if (spi->m_BusWidth == SPI_WORD) {
        if(spi->m_EnAutoCS){
            tmpTxBuf32 = (uint32_t *)buf;
            tmpSize = size;

            while(tmpSize){
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiDrvRxTxChannelOn(spi->base, true, true);
        }
        else{
            tmpTxBuf32 = (uint32_t *)buf;
            tmpSize = size;

            spiDrvEnablePktcount(spi->base, false, 0);

            spiDrvRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiDrvEnableCs(spi->base, true);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiDrvEnableCs(spi->base, false);
            }
        }
    }

    if (spi->m_OpMode == SPI_MASTER)
        __wait_until_tx_done(spi->base);
    return 0;
}

static int spiDrvPollingWriteRead(uint32_t portNum, void *txbuf, void *rxbuf, int size)
{
    struct spi_device *spi;
    uint32_t *tmpTxBuf32, *tmpRxBuf32;
    uint16_t *tmpTxBuf16, *tmpRxBuf16;
    uint8_t *tmpTxBuf8, *tmpRxBuf8;
    uint32_t tmpSize;
    uint32_t loopDelay;

    SPI_DEBUG_PRINT("\n\nspiDrvPollingWriteRead \nportNum: %d, txbuf: 0x%x , rxbuf: 0x%x, size: %d\n", (unsigned int)portNum, (unsigned int)txbuf, (unsigned int)rxbuf, (unsigned int)size);
    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDrvPollingWriteRead spi->m_BusWidth: 0x%x\n", (unsigned int)spi->m_BusWidth);
    if (spi->m_BusWidth == SPI_BYTE) {
        if(spi->m_EnAutoCS){
            tmpTxBuf8 = (uint8_t *)txbuf;
            tmpRxBuf8 = (uint8_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("--> [m_EnAutoCs true] tmpTxBuf8(0x%x): 0x%x, tmpRxBuf8(0x%x): 0x%x, size: %d \n",\
                    (int)tmpTxBuf8, (int)*tmpTxBuf8, (int)tmpRxBuf8, (int)*tmpRxBuf8, (int)size);

            while(tmpSize){
                __raw_write8(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiDrvRxTxChannelOn(spi->base, true, true);

            SPI_DEBUG_PRINT("--> [m_EnAutoCs true] tmpTxBuf8(0x%x): 0x%x, tmpRxBuf8(0x%x): 0x%x, size: %d \n",\
                    (int)tmpTxBuf8, (int)*tmpTxBuf8, (int)tmpRxBuf8, (int)*tmpRxBuf8, (int)size);


            tmpSize = size;
            while(tmpSize){
                *tmpRxBuf8++ = __raw_read8(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
        }
        else{
            tmpTxBuf8 = (uint8_t *)txbuf;
            tmpRxBuf8 = (uint8_t *)rxbuf;
            tmpSize = size;

            spiDrvEnablePktcount(spi->base, false, 0);

            spiDrvRxTxChannelOn(spi->base, true, true);

            spiDrvEnableCs(spi->base, true);
            while(tmpSize>0){
                SPI_DEBUG_PRINT("--> [m_EnAutoCs false] tmpTxBuf8(0x%x): 0x%x, tmpRxBuf8(0x%x): 0x%x, size: %d \n",\
                        (int)tmpTxBuf8, (int)*tmpTxBuf8, (int)tmpRxBuf8, (int)*tmpRxBuf8, (int)size);

                __raw_write8(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                __wait_until_tx_done(spi->base);

#if 0
                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;
#endif

                *tmpRxBuf8++ = __raw_read8(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
            spiDrvEnableCs(spi->base, false);
        }
    }
    else if (spi->m_BusWidth == SPI_HWORD) {
        if(spi->m_EnAutoCS){
            tmpTxBuf16 = (uint16_t *)txbuf;
            tmpRxBuf16 = (uint16_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("tmpTxBuf16(0x%x): 0x%x, size: %d \n",(int)tmpTxBuf16, (int)*tmpTxBuf16, (int)size);

            while(tmpSize){
                __raw_write16(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiDrvRxTxChannelOn(spi->base, true, true);


            tmpSize = size;
            while(tmpSize){
                *tmpRxBuf16++ = __raw_read16(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
        }
        else{
            tmpTxBuf16 = (uint16_t *)txbuf;
            tmpRxBuf16 = (uint16_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("tmpTxBuf16(0x%x): 0x%x, size: %d \n",(int)tmpTxBuf16, (int)*tmpTxBuf16, (int)size);

            spiDrvEnablePktcount(spi->base, false, 0);

            spiDrvRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiDrvEnableCs(spi->base, true);
                __raw_write16(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                __raw_write16(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiDrvEnableCs(spi->base, false);

                *tmpRxBuf16++ = __raw_read16(spi->base + REG_SPI_RX_DATA);
                *tmpRxBuf16++ = __raw_read16(spi->base + REG_SPI_RX_DATA);

                tmpSize--;
            }
        }
    }
    else if (spi->m_BusWidth == SPI_WORD) {
        if(spi->m_EnAutoCS){
            tmpTxBuf32 = (uint32_t *)txbuf;
            tmpRxBuf32 = (uint32_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("tmpTxBuf32(0x%x): 0x%x, size: %d \n",(int)tmpTxBuf32, (int)*tmpTxBuf32, (int)size);

            while(tmpSize){
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiDrvRxTxChannelOn(spi->base, true, true);


            tmpSize = size;
            while(tmpSize){
                *tmpRxBuf32++ = __raw_readl(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
        }
        else{
            tmpTxBuf32 = (uint32_t *)txbuf;
            tmpRxBuf32 = (uint32_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("tmpTxBuf32(0x%x): 0x%x, size: %d \n",(int)tmpTxBuf32, (int)*tmpTxBuf32, (int)size);

            spiDrvEnablePktcount(spi->base, false, 0);

            spiDrvRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiDrvEnableCs(spi->base, true);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiDrvEnableCs(spi->base, false);

                *tmpRxBuf32++ = __raw_readl(spi->base + REG_SPI_RX_DATA);
                *tmpRxBuf32++ = __raw_readl(spi->base + REG_SPI_RX_DATA);

                tmpSize--;
            }
        }
    }

    __wait_until_tx_done(spi->base);
    spiDrvFifoFlush(spi->base);

    return 0;
}

#if 0//for future implementation
static int spiDrvDmaWriteRead(uint32_t portNum, void *tx_va, void *rx_va, int size)
{
    s16 tx_ch_num, rx_ch_num;
    uint32_t bs, bl, es;
    char *dma_name;
    DMA_REQ_MAP tx_dma_req, rx_dma_req;
    uint32_t tx_src_va = *(uint32_t *)tx_va;
    uint32_t tx_dst_va;
    uint32_t rx_src_va;
    uint32_t rx_dst_va = *(uint32_t *)rx_va;
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];


    if (!data->pdma[spi->m_pdma_ch * 2]) {
        ERR_MSG("\n >> ERROR : dma.%d has no channel for SPI %d !!\n", spi->m_pdma_ch, spi->m_portId);
        return -1;
    } else {
        dma_name = resource_dma_name(data->pdma[spi->m_pdma_ch * 2]);
        rx_dma_req = resource_dma_req(data->pdma[spi->m_pdma_ch * 2]);            //RX
        tx_dma_req = resource_dma_req(data->pdma[(spi->m_pdma_ch * 2) + 1]);    //TX
    }

    struct platform_device *pdev = platform_device_find_byname(dma_name);
    struct dma_drvdata *dma_drv = platform_get_drvdata(pdev);

    bl = spi->m_DMAType*3; // BL1(0) / BL4(3)
    es = 0; // 0 means NO_SWAP
    bs = spi->m_BusWidth;

    //pm_runtime_get_sync(dma_drv->dev);

    tx_dst_va = (uint32_t)(u64)spi->base + REG_SPI_TX_DATA;

    rx_src_va = (uint32_t)(u64)spi->base + REG_SPI_RX_DATA;

    tx_ch_num = dma_transfer_params((struct dma_drvdata *)dma_drv, DMA_M2P, bs, bl, es, tx_dma_req);
    rx_ch_num = dma_transfer_params((struct dma_drvdata *)dma_drv, DMA_P2M, bs, bl, es, rx_dma_req);

    dma_transfer_start((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)tx_ch_num, (uint32_t *)(u64)tx_src_va, (uint32_t *)(u64)tx_dst_va, size);

    while (!dma_transfer_done((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)tx_ch_num));

    dma_channel_stop((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)tx_ch_num);

    //callback_channel = ch_num;
    //dma_register_callback((struct dma_drvdata *)dma_drv, ch_num, pdma_isr_callback, (void *)&callback_channel);
    dma_transfer_start((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)rx_ch_num, (uint32_t *)(u64)rx_src_va, (uint32_t *)(u64)rx_dst_va, size);

    while (!dma_transfer_done((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)rx_ch_num));

    dma_channel_stop((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)rx_ch_num);

//    dma_unregister_callback((struct dma_drvdata *)dma_drv, ch_num);

    //pm_runtime_put_sync(dma_drv->dev);
    return 0;
}
#endif

static int spiDrvInterruptWriteRead(uint32_t portNum, void *txbuf, void *rxbuf, int size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    spi->txbuf = txbuf;
    spi->tx_size = size;
    spi->tx_cur_ptr = 0;

    spi->rxbuf = rxbuf;
    spi->rx_size = size;
    spi->rx_cur_ptr = 0;

    spiDrvInitCtrl(spi->base, ENABLE, RX_FIFO_RDY);
    spiDrvInitCtrl(spi->base, ENABLE, TX_FIFO_RDY);

    NVIC_EnableIRQ(USI_CHUB00_IRQn);

    return 0;
}


static int spiDrvInterruptWrite(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    spi->txbuf = buf;
    spi->tx_size = size;
    spi->tx_cur_ptr = 0;

    //irqchip_set_prioritylevel(data->irq, 0xC0); // This function is not acceptable for ISP SPI
    spiDrvInitCtrl(spi->base, ENABLE, TX_FIFO_RDY);

    return 0;
}

#if 0//for future implementation
static int spiDrvDmaWrite(uint32_t portNum, void *buf_va, int size)
{
//    bool bResult = TRUE;
//    uint8_t bs_cnt, bl_cnt;
    s16 ch_num;
    cycle_t elapsed_tick = 0;
    uint32_t bs, bl, es;
    char *dma_name;
    DMA_REQ_MAP dma_req;
    uint32_t src_va = *(uint32_t *)buf_va;
    uint32_t dst_va;
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    if (!data->pdma[(spi->m_pdma_ch * 2) + 1]) {
        ERR_MSG("\n >> ERROR : dma.%d has no channel for SPI %d !!\n", spi->m_pdma_ch, spi->m_portId);
        return -1;
    } else {
        dma_name = resource_dma_name(data->pdma[(spi->m_pdma_ch * 2) + 1]); //TX
        dma_req = resource_dma_req(data->pdma[(spi->m_pdma_ch * 2) + 1]);    //TX
    }

    struct platform_device *pdev = platform_device_find_byname(dma_name);
    struct dma_drvdata *dma_drv = platform_get_drvdata(pdev);

    bl = spi->m_DMAType*3; // BL1(0) / BL4(3)
    es = 0; // 0 means NO_SWAP
    bs = spi->m_BusWidth;

    //pm_runtime_get_sync(dma_drv->dev);

    dst_va = (uint32_t)(u64)spi->base + REG_SPI_TX_DATA;

    ch_num = dma_transfer_params((struct dma_drvdata *)dma_drv, DMA_M2P, bs, bl, es, dma_req);

    //callback_channel = ch_num;
    //dma_register_callback((struct dma_drvdata *)dma_drv, ch_num, pdma_isr_callback, (void *)&callback_channel);

    dma_transfer_start((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)ch_num, (uint32_t *)(u64)src_va, (uint32_t *)(u64)dst_va, size);
    if (spi->m_timeCheck) // right after dma excute
        elapsed_tick = get_timer(0);

    while (!dma_transfer_done((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)ch_num));

    if (spi->m_timeCheck)
        spi->m_tick = get_timer(elapsed_tick);

    dma_channel_stop((struct dma_drvdata *)dma_drv, (DMA_CHANNEL_NUM)ch_num);

//    dma_unregister_callback((struct dma_drvdata *)dma_drv, ch_num);

    //pm_runtime_put_sync(dma_drv->dev);
    return 0;
}
#endif

int spiDrvRead(uint32_t portNum, uint32_t *buf, uint32_t size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDrvRead\n");
    spiDrvDumpSFR(portNum);

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiDrvEnableCs(spi->base, true);

    /* enable rx channel for master mode */
    if (spi->m_OpMode == SPI_MASTER)
        spiDrvEnablePktcount(spi->base, ENABLE, size);    // Enable when Master Rx not

    spiDrvChannelOn(portNum);

/*-----------------------------------------------------*/
    if (spi->m_RxTransferMode == SPI_POLLING_MODE)
        spiDrvPollingRead(portNum, buf, size);
    else if (spi->m_RxTransferMode == SPI_INTERRUPT_MODE)
        spiDrvInterruptRead(portNum, buf, size);
/*-----------------------------------------------------*/

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiDrvEnableCs(spi->base, false);

    /* enable rx channel for master mode */
    if (spi->m_OpMode == SPI_MASTER)
        spiDrvEnablePktcount(spi->base, DISABLE, 0);    // Enable when Master Rx

    return 0;
}

int spiDrvWrite(uint32_t portNum, uint32_t *buf, uint32_t size)
{
    int write_result = -1;
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDrvWrite\n");
    spiDrvDumpSFR(portNum);

    if (spi->m_EnSWReset)
        spiDrvFifoFlush(spi->base);

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiDrvEnableCs(spi->base, true);

    spiDrvChannelOn(portNum);

/*-----------------------------------------------------*/
    if (spi->m_TxTransferMode == SPI_POLLING_MODE)
        write_result = spiDrvPollingWrite(portNum, buf, size);
    else if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE)
        write_result = spiDrvInterruptWrite(portNum, buf, size);
/*-----------------------------------------------------*/

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiDrvEnableCs(spi->base, false);

    return write_result;
}

int spiDrvWriteRead(uint32_t portNum, uint32_t *txbuf, uint32_t *rxbuf, uint32_t size)
{
    struct spi_device *spi;
    int ret=0;

    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDrvWriteRead portNum: %d, txbuf: 0x%x , rxbuf: 0x%x, size: %d\n", (int)portNum, (int)txbuf, (int)rxbuf, (int)size);

    if (spi->m_EnSWReset)
        spiDrvFifoFlush(spi->base);

    if(spi->m_loopback)
        spiDrvSetSelfloopback(spi->base, true);
//-----------------------------------------------------
    if (spi->m_TxTransferMode == SPI_POLLING_MODE)
        ret= spiDrvPollingWriteRead(portNum, txbuf, rxbuf, size);
    else if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE)
        ret= spiDrvInterruptWriteRead(portNum, txbuf, rxbuf, size);
//-----------------------------------------------------

    if(spi->m_loopback)
        spiDrvSetSelfloopback(spi->base, false);

    return ret;
}

int spiDrvReadData(uint32_t port, uint32_t addr, void* rxBuf, size_t size)
{
    struct spi_device *spi;
    uint32_t pTempBuf;
    uint32_t step, cnt;
    int ret;

    spi = &mSpiDev[port];
    cnt = 0;

    if (size > (SPI_FIFO_LENGTH - 1))
        step = SPI_FIFO_LENGTH - 1;
    else
        step = size;

    while (cnt < size) {
        pTempBuf = (uint32_t)mSpiTempBuf;
        if (spi->m_BusWidth == SPI_BYTE){
            *(uint8_t *)pTempBuf = (uint8_t)(addr | 0x80);
	    pTempBuf++;
        }
        else if (spi->m_BusWidth == SPI_HWORD){
            *(uint16_t *)pTempBuf = (uint16_t)(addr | 0x8000);
	    pTempBuf += 2;
        }
        else {
            *(uint32_t *)pTempBuf = (uint32_t)(addr | 0x80000000);
	    pTempBuf += 4;
        }

        ret = spiDrvWriteRead(port, mSpiTempBuf, mSpiTempBuf, step + 1);

        if (ret)
            return ret;

        if (spi->m_BusWidth == SPI_BYTE){
            memcpy((uint8_t *)rxBuf + cnt, (uint8_t *)mSpiTempBuf + 1, step);
        }
        else if (spi->m_BusWidth == SPI_HWORD){
            memcpy((uint16_t *)rxBuf + cnt, (uint16_t *)mSpiTempBuf + 1, step);
        }
        else {
            memcpy((uint32_t *)rxBuf + cnt, (uint32_t *)mSpiTempBuf + 1, step);
        }

        if (cnt + step > size)
            step = size - cnt;

        cnt = cnt + step;
        addr = addr + cnt;
    }

    return 0;
}

int spiDrvWriteData(uint32_t port, uint32_t addr, void* txBuf, size_t size)
{
    struct spi_device *spi;
    uint32_t pTempBuf = (uint32_t)mSpiTempBuf;
    uint32_t step, cnt;
    int ret;

    spi = &mSpiDev[port];
    cnt = 0;

    if (size > (SPI_FIFO_LENGTH - 1))
        step = SPI_FIFO_LENGTH - 1;
    else
        step = size;

    while (cnt < size) {
        pTempBuf = (uint32_t)mSpiTempBuf;
        if (spi->m_BusWidth == SPI_BYTE){
            *(uint8_t *)pTempBuf = (uint8_t)(addr);
	    pTempBuf++;
            memcpy((uint8_t *)pTempBuf, txBuf, size);
        }
        else if (spi->m_BusWidth == SPI_BYTE){
            *(uint16_t *)pTempBuf = (uint16_t)(addr);
	    pTempBuf += 2;
            memcpy((uint8_t *)pTempBuf, txBuf, size*2);
        }
        else {
            *(uint32_t *)pTempBuf = (uint32_t)(addr);
	    pTempBuf += 4;
            memcpy((uint8_t *)pTempBuf, txBuf, size*4);
        }

        ret = spiDrvWriteRead(port, mSpiTempBuf, NULL, step + 1);

        if (ret)
            return ret;

        if (cnt + step > size)
            step = size - cnt;

        cnt = cnt + step;
        addr = addr + cnt;
    }

    return 0;
}
