/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SPI
 *----------------------------------------------------------------------------
 *      Name:    spi.c
 *      Purpose: To implement SPI APIs
 *      Rev.:    V1.00
 *----------------------------------------------------------------------------
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http:www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <csp_common.h>
#if defined(SEOS)
  #include <errno.h>
  //#include <atomicBitset.h>
  //#include <atomic.h>
  #include <heap.h>
  #include <platform.h>
  #include <spi.h>
  #include <spi_priv.h>
#elif defined(EMBOS)
  #include <Device.h>
  #include <RTOS.h>
#endif
#include <spiSfrBase.h>
#include <plat/spi/spi.h>
#include <spiDrv.h>
#include <debug.h>
#include <usi.h>
#include <rtc.h>
#include <cmu.h>
#include <string.h>
#include <cmsis.h>
#include <plat/gpio/gpio.h>

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
#define SPI_MAX_BUF 100

/* save the enabled spi channel */
static struct spi_device mSpiDev[SPI_CHANNEL_MAX];
static uint8_t mSpiChannelStatus[SPI_CHANNEL_MAX];
static uint8_t m_portNum[16] = {0,};

static int __wait_until_tx_done(uint32_t base);
static uint32_t mSpiTempBuf[SPI_MAX_BUF];
static void dumpSpi(uint32_t base);

void spiSetTransferMode(uint32_t portNum, SPI_transfer_mode mode) {

    mSpiDev[portNum].m_RxTransferMode = mode;
    mSpiDev[portNum].m_TxTransferMode = mode;
}

static void spiDumpSFR(uint32_t portNum)
{
#if defined(I2CSPI_SFRDUMP_REQUIRED)
    struct spi_device *spi;
    unsigned int regValue;

    spi =  &mSpiDev[portNum];
    CSP_PRINTF_INFO("\nspiDumpSFR\n#############################################\n");

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

static void spiInitCtrl(uint32_t base, int on, uint32_t src)
{
    uint32_t val;

    /* Enable interrupt */
    val = __raw_readl(base + REG_SPI_INT_EN);
    val &= ~src;
    if (on)
        val |= src;
    __raw_writel(val, base + REG_SPI_INT_EN);
}

static void spiSetChannel(struct spi_device *spi)
{
    uint32_t val;

    val = __raw_readl(spi->base + REG_CH_CFG);

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

static void spiRxTxChannelOn(uint32_t base, bool RxOn, bool TxOn)
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

static void spiFifoFlush(uint32_t base)
{
    volatile int val;

    /* RxTx channel should be disabled before flushing fifo */
    spiRxTxChannelOn(base, false, false);
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

static void spiEnablePktcount(uint32_t base, bool on, uint16_t size)
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


static void spiSetMode(struct spi_device *spi)
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

static void spiSetSelfloopback(uint32_t base, bool on)
{
    uint32_t val;

    SPI_DEBUG_PRINT("spiSetSelfloopback base: 0x%x, on: %d\n", (unsigned int)base, (unsigned int)on);
    val =  __raw_readl(base + REG_MODE_CFG);

    if(on){
        val |= EXYSNOS_SPI_SELF_LOOPBACK_ON;
    }
    else{
        val &= (uint32_t)~(EXYSNOS_SPI_SELF_LOOPBACK_ON);
    }

    __raw_writel(val, base + REG_MODE_CFG);
}

static void spiSetCs(struct spi_device *spi)
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


static void spiSetSwap(struct spi_device *spi)
{
    uint32_t val;

    val = (spi->m_EnRxSwap << 4) | (spi->m_EnTxSwap);

    __raw_writel(val, spi->base + REG_SWAP_CFG);
}


static void spiSetFbclk(struct spi_device *spi)
{
    uint32_t val;

    val = spi->m_FBClkSel;

    __raw_writel(val, spi->base + REG_FB_CLK_SEL);
}


static void spiSetErrInt(uint32_t base)
{
    uint32_t val;

    val =  __raw_readl(base + REG_SPI_INT_EN);
    val |= 0x68;

    __raw_writel(val, base + REG_SPI_INT_EN);
}



static void spiClearPending(uint32_t base)
{
    uint32_t val;

    val = (uint32_t)0x1F;

    __raw_writel(val, base + REG_PENDING_CLR_REG);
}




static int spiEnableCs(struct spi_device *spi, bool is_enable)
{
    uint32_t val;
    uint32_t gpio;

    val =  __raw_readl(spi->base + REG_CS_REG);

    if (is_enable) {
        val &= (uint32_t)~EXYNOS_NSSOUT;
        gpio = GPIO_DAT_LOW;
    }
    else {
        val |= (uint32_t)EXYNOS_NSSOUT;
        gpio = GPIO_DAT_HIGH;
    }

    if (spi->m_multiSlave)
        gpioSetData((GpioPinNumType)spi->m_cs, gpio);
    __raw_writel(val, spi->base + REG_CS_REG);

    return 0;
}

/* function ready for interrupt mode */
static void spiIntPendingClear(struct spi_device *spi, uint32_t clear_int)
{
    __raw_writel(clear_int, spi->base + REG_PENDING_CLR_REG);
}

static uint32_t spiIntRRxOverrun(struct spi_device *spi);
static uint32_t spiIntRRxOverrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Rx Fifo Over Run!!\n", spi->m_portId);
    spi->m_RxOverrunINT_Value = true;//TRUE;

    spiIntPendingClear(spi, SPI_CLR_PND_RX_OVERRUN);
    return 0;
}

static uint32_t spiIntRxUnderrun(struct spi_device *spi);
static uint32_t spiIntRxUnderrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Rx Fifo Under Run!!\n", spi->m_portId);
    spi->m_RxUnderrunINT_Value = true;//TRUE;

    spiIntPendingClear(spi, SPI_CLR_PND_RX_UNDERRUN);

    return 0;
}

static uint32_t spiIntTxOverrun(struct spi_device *spi);
static uint32_t spiIntTxOverrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Tx Fifo Over Run!!\n", spi->m_portId);
    spi->m_TxOverrunINT_Value = true;//TRUE;

    spiIntPendingClear(spi, SPI_CLR_PND_TX_OVERRUN);
    return 0;
}

static uint32_t spiIntTxUnderrun(struct spi_device *spi);
static uint32_t spiIntTxUnderrun(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Tx Fifo Under Run!!\n", spi->m_portId);
    spi->m_TxUnderrunINT_Value = true;//TRUE;

    spiIntPendingClear(spi, SPI_CLR_PND_TX_UNDERRUN);

    return 0;
}

static uint32_t spiIntTrailByte(struct spi_device *spi);
static uint32_t spiIntTrailByte(struct spi_device *spi)
{
    osLog(LOG_ERROR, "\n[ISR]SPI Port%lu Tx Fifo Under Run!!\n", spi->m_portId);
    spi->m_TrailingINT_Value = true;//TRUE;

    spiIntPendingClear(spi, SPI_CLR_PND_TRAILING);

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

static uint32_t spiIntWrite(struct spi_device *spi);
static uint32_t spiIntWrite(struct spi_device *spi)
{
    void *buf;
    int size, cnt;
    //uint32_t regVal;
    buf = spi->txbuf;
    size = spi->tx_size;
    cnt = 0;

    //osLog(LOG_INFO, "spiIntWrite");
    //
    /* Disable Interrupt */
    spiInitCtrl(spi->base, DISABLE, TX_FIFO_RDY);

    size = size - spi->tx_cur_ptr;
    if (size > TX_FIFO_MAX)
        size = TX_FIFO_MAX;

    spiEnablePktcount(spi->base, true, size);
    spiRxTxChannelOn(spi->base, true, true);
    spiEnableCs(spi, true);

    if (spi->m_BusWidth == SPI_BYTE) {
        while (cnt < size) {
            __raw_writel(((uint8_t *)buf)[spi->tx_cur_ptr++], spi->base + REG_SPI_TX_DATA);
            cnt++;
        }
    } else if (spi->m_BusWidth == SPI_HWORD) {
        if (size % sizeof(uint16_t) != 0) {

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0x01) { // Tx FIFO Ready
                __raw_writel(*(uint16_t *)buf, spi->base + REG_SPI_TX_DATA);
                buf = (uint8_t *)buf + sizeof(uint16_t);
                size -= 2;
            }

        }
    } else if (spi->m_BusWidth == SPI_WORD) {
        if (size % sizeof(uint32_t) != 0) {

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0x01) { // Tx FIFO Ready
                __raw_writel(*(uint32_t *)buf, spi->base + REG_SPI_TX_DATA);

                buf = (uint8_t *)buf + sizeof(uint32_t);
                size -= 4;
            }
        }
    }

    //if (spi->m_OpMode == SPI_MASTER && size != spi->tx_cur_ptr)   // eun00 : No check TX done if size is 0.
    //    __wait_until_tx_done(spi->base);

    if (spi->tx_cur_ptr == size) {
        //osLog(LOG_INFO, "spiIntWrite size%d ptr%d 0x%X\n", size, spi->tx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));
        spi->m_TxIntCompleteCallback_Value = true;//TRUE;

    } else {
        //osLog(LOG_INFO, "spiIntWrite size%d ptr%d 0x%X\n", size, spi->tx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));
    }

    return 0;
}

static uint32_t spiIntRead(struct spi_device *spi);
static uint32_t spiIntRead(struct spi_device *spi)
{
    void *buf;
    int size, cnt;
    uint32_t regVal;

    //osLog(LOG_INFO, "spiIntRead\n");

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

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0xFF8000) { // RX_FIFO_LVL[23:15]
                *(uint16_t *)buf = __raw_readl(spi->base + REG_SPI_RX_DATA);

                buf = (uint8_t *)buf + sizeof(uint16_t);
                size -= 2;
            }
        }
    } else if (spi->m_BusWidth == SPI_WORD) {
        if (size % sizeof(uint32_t) != 0) {

            return (-1);//ERROR;
        }

        while (size > 0) {
                  if (__raw_readl(spi->base + REG_SPI_STATUS) & 0xFF8000) { // RX_FIFO_LVL[23:15]
                *(uint32_t *)buf = __raw_readl(spi->base + REG_SPI_RX_DATA);

                buf = (uint8_t *)buf + sizeof(uint32_t);
                size -= 4;
            }
        }
    }

    if (spi->rx_cur_ptr == size) {
        //osLog(LOG_INFO, "spiIntRead size%d ptr%d, 0x%X\n", size, spi->rx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));

        /* Disable Interrupt */
        spiInitCtrl(spi->base, DISABLE, RX_FIFO_RDY);
        spiEnableCs(spi, false);

        //__wait_until_tx_done(spi->base);
        //spiFifoFlush(spi->base);

        spi->m_RxIntCompleteCallback_Value = true;//TRUE;

#if defined(SEOS)
        spiMasterRxTxDone(spi->dev,0);
#endif
    } else {
        //osLog(LOG_INFO, "spiIntRead size%d ptr%d, 0x%X\n", size, spi->rx_cur_ptr, __raw_readl(spi->base + REG_SPI_STATUS));

        //__wait_until_tx_done(spi->base);
        spi->m_RxReadyLevel = size - spi->rx_cur_ptr;
        if (spi->m_RxReadyLevel > RX_FIFO_MAX)
            spi->m_RxReadyLevel = RX_FIFO_MAX;
        regVal = __raw_readl(spi->base + REG_MODE_CFG);
        regVal &= (uint32_t)~(0x3F << EXYNOS_SPI_RXRDYLVL_OFFSET);
        regVal |= ((spi->m_RxReadyLevel & 0x3F) << EXYNOS_SPI_RXRDYLVL_OFFSET);
        __raw_writel(regVal, spi->base + REG_MODE_CFG);

        /* Enable Interrupt */
        spiInitCtrl(spi->base, ENABLE, TX_FIFO_RDY);
    }

    return 0;
}

#define SPI_RETRY_MAX    (10000)
#define SPI_TIMEOUT_US   (10000)

static int __wait_until_tx_done(uint32_t base)
{
    uint32_t val;
    u64 time = getTimeStampUS();

    //Delay(1);
    do {
        val = __raw_readl(base + REG_SPI_STATUS);

        if (val & TX_DONE)
            break;

        if (getTimeStampUS() - time > SPI_TIMEOUT_US) {
            osLog(LOG_ERROR, "%s: out! inTime:%llu, outTime:%llu\n", __func__, time, getTimeStampUS());
            dumpSpi(base);
            return -1;
        }

    } while (1);

    return 0;
}

static int spiSetup(uint32_t portNum)
{
    struct spi_device *spi;
    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiSetup\n");

    if ((spi->m_TxTransferMode == SPI_DMA_MODE) || (spi->m_RxTransferMode == SPI_DMA_MODE)) {
    }

    //NEED TO BE DOUBLE CHECK!!! HOW TO CONFIG TxReadyLevel
    spi->m_TxReadyLevel = 1;

    spiSetChannel(spi);

    spiSetMode(spi);
    spiSetCs(spi);
    spiSetSwap(spi);
    spiSetFbclk(spi);

    if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE || spi->m_RxTransferMode == SPI_INTERRUPT_MODE) {
        spiSetErrInt(spi->base);
    }

    spiClearPending(spi->base);

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

    spiFifoFlush(spi->base);

    return 0;
}

// this is for only loopback test
static int spiChannelOn(uint32_t portNum)
{
    uint32_t val;
    struct spi_device *spi;

    SPI_DEBUG_PRINT("spiChannelOn portNum: %d\n", (unsigned int)portNum);
    spi =  &mSpiDev[portNum];

    val = __raw_readl(spi->base + REG_CH_CFG);

    val &= (uint32_t)~0x03;
    val |= (spi->m_EnRxChannel << 1) | (spi->m_EnTxChannel << 0);

    __raw_writel(val, spi->base + REG_CH_CFG);
    while ((__raw_readl(spi->base + REG_CH_CFG) & 1) != (val & 1));
    return 0;
}

static int spiPollingRead(uint32_t portNum, void *buf, int size)
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
            *tmpRxBuf8++ = (uint8_t)__raw_readl(spi->base + REG_SPI_RX_DATA);
            tmpSize--;
        }
    }
    else if (spi->m_BusWidth == SPI_HWORD) {
        tmpRxBuf16 = (uint16_t *)buf;
        tmpSize = size;
        while(tmpSize){
            *tmpRxBuf16++ = (uint16_t)__raw_readl(spi->base + REG_SPI_RX_DATA);
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

static int spiInterruptRead(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    spi->rxbuf = buf;
    spi->rx_size = size;
    spi->rx_cur_ptr = 0;

    spiInitCtrl(spi->base, ENABLE, RX_FIFO_RDY);

    return 0;
}

static int spiPollingWrite(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;
    uint32_t *tmpTxBuf32;
    uint16_t *tmpTxBuf16;
    uint8_t *tmpTxBuf8;
    uint32_t tmpSize;
    uint32_t loopDelay;
    int ret = 0;

    spi =  &mSpiDev[portNum];

    if (spi->m_BusWidth == SPI_BYTE) {
        if(spi->m_EnAutoCS){
            tmpTxBuf8 = (uint8_t *)buf;
            tmpSize = size;

            while(tmpSize){
                __raw_writel(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiRxTxChannelOn(spi->base, true, true);
        }
        else{
            tmpTxBuf8 = (uint8_t *)buf;
            tmpSize = size;

            spiEnablePktcount(spi->base, false, 0);

            spiRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiEnableCs(spi, true);
                __raw_writel(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiEnableCs(spi, false);
            }
        }
    }
    else if (spi->m_BusWidth == SPI_HWORD) {
        if(spi->m_EnAutoCS){
            tmpTxBuf16 = (uint16_t *)buf;
            tmpSize = size;

            while(tmpSize){
                __raw_writel(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiRxTxChannelOn(spi->base, true, true);
        }
        else{
            tmpTxBuf16 = (uint16_t *)buf;
            tmpSize = size;

            spiEnablePktcount(spi->base, false, 0);

            spiRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiEnableCs(spi, true);
                __raw_writel(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiEnableCs(spi, false);
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

            spiRxTxChannelOn(spi->base, true, true);
        }
        else{
            tmpTxBuf32 = (uint32_t *)buf;
            tmpSize = size;

            spiEnablePktcount(spi->base, false, 0);

            spiRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiEnableCs(spi, true);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiEnableCs(spi, false);
            }
        }
    }

    if (spi->m_OpMode == SPI_MASTER)
        ret = __wait_until_tx_done(spi->base);

    return ret;
}

static int spiPollingWriteRead(uint32_t portNum, void *txbuf, void *rxbuf, int size)
{
    struct spi_device *spi;
    uint32_t *tmpTxBuf32, *tmpRxBuf32;
    uint16_t *tmpTxBuf16, *tmpRxBuf16;
    uint8_t *tmpTxBuf8, *tmpRxBuf8;
    uint32_t tmpSize;
    uint32_t loopDelay;
    int ret = 0;

    SPI_DEBUG_PRINT("\n\nspiPollingWriteRead \nportNum: %d, txbuf: 0x%x , rxbuf: 0x%x, size: %d\n", (unsigned int)portNum, (unsigned int)txbuf, (unsigned int)rxbuf, (unsigned int)size);
    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiPollingWriteRead spi->m_BusWidth: 0x%x\n", (unsigned int)spi->m_BusWidth);
    if (spi->m_BusWidth == SPI_BYTE) {
        if(spi->m_EnAutoCS){
            tmpTxBuf8 = (uint8_t *)txbuf;
            tmpRxBuf8 = (uint8_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("--> [m_EnAutoCs true] tmpTxBuf8(0x%x): 0x%x, tmpRxBuf8(0x%x): 0x%x, size: %d \n",\
                    (int)tmpTxBuf8, (int)*tmpTxBuf8, (int)tmpRxBuf8, (int)*tmpRxBuf8, (int)size);

            while(tmpSize){
                __raw_writel(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiRxTxChannelOn(spi->base, true, true);

            SPI_DEBUG_PRINT("--> [m_EnAutoCs true] tmpTxBuf8(0x%x): 0x%x, tmpRxBuf8(0x%x): 0x%x, size: %d \n",\
                    (int)tmpTxBuf8, (int)*tmpTxBuf8, (int)tmpRxBuf8, (int)*tmpRxBuf8, (int)size);


            tmpSize = size;
            while(tmpSize){
                *tmpRxBuf8++ = (uint8_t)__raw_readl(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
        }
        else{
            tmpTxBuf8 = (uint8_t *)txbuf;
            tmpRxBuf8 = (uint8_t *)rxbuf;
            tmpSize = size;

            spiEnablePktcount(spi->base, false, 0);

            spiRxTxChannelOn(spi->base, true, true);

            spiEnableCs(spi, true);
            while(tmpSize>0){
                SPI_DEBUG_PRINT("--> [m_EnAutoCs false] tmpTxBuf8(0x%x): 0x%x, tmpRxBuf8(0x%x): 0x%x, size: %d \n",\
                        (int)tmpTxBuf8, (int)*tmpTxBuf8, (int)tmpRxBuf8, (int)*tmpRxBuf8, (int)size);

                __raw_writel(*tmpTxBuf8++, spi->base + REG_SPI_TX_DATA);
                __wait_until_tx_done(spi->base);

                if (tmpRxBuf8)
                    *tmpRxBuf8++ = (uint8_t)__raw_readl(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
            spiEnableCs(spi, false);
        }
    }
    else if (spi->m_BusWidth == SPI_HWORD) {
        if(spi->m_EnAutoCS){
            tmpTxBuf16 = (uint16_t *)txbuf;
            tmpRxBuf16 = (uint16_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("tmpTxBuf16(0x%x): 0x%x, size: %d \n",(int)tmpTxBuf16, (int)*tmpTxBuf16, (int)size);

            while(tmpSize){
                __raw_writel(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                tmpSize--;
            }

            spiRxTxChannelOn(spi->base, true, true);


            tmpSize = size;
            while(tmpSize){
                *tmpRxBuf16++ = (uint16_t)__raw_readl(spi->base + REG_SPI_RX_DATA);
                tmpSize--;
            }
        }
        else{
            tmpTxBuf16 = (uint16_t *)txbuf;
            tmpRxBuf16 = (uint16_t *)rxbuf;
            tmpSize = size;

            SPI_DEBUG_PRINT("tmpTxBuf16(0x%x): 0x%x, size: %d \n",(int)tmpTxBuf16, (int)*tmpTxBuf16, (int)size);

            spiEnablePktcount(spi->base, false, 0);

            spiRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiEnableCs(spi, true);
                __raw_writel(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf16++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiEnableCs(spi, false);
                *tmpRxBuf16++ = (uint16_t)__raw_readl(spi->base + REG_SPI_RX_DATA);
                *tmpRxBuf16++ = (uint16_t)__raw_readl(spi->base + REG_SPI_RX_DATA);

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

            spiRxTxChannelOn(spi->base, true, true);


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

            spiEnablePktcount(spi->base, false, 0);

            spiRxTxChannelOn(spi->base, true, true);
            while(tmpSize>0){
                spiEnableCs(spi, true);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);
                __raw_writel(*tmpTxBuf32++, spi->base + REG_SPI_TX_DATA);

                loopDelay = 150;
                while(loopDelay>0)
                    loopDelay--;

                spiEnableCs(spi, false);

                *tmpRxBuf32++ = __raw_readl(spi->base + REG_SPI_RX_DATA);
                *tmpRxBuf32++ = __raw_readl(spi->base + REG_SPI_RX_DATA);

                tmpSize--;
            }
        }
    }

    ret = __wait_until_tx_done(spi->base);
    spiFifoFlush(spi->base);

    return ret;
}

static int spiInterruptWriteRead(uint32_t portNum, void *txbuf, void *rxbuf, int size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    spi->txbuf = txbuf;
    spi->tx_size = size;
    spi->tx_cur_ptr = 0;

    spi->rxbuf = rxbuf;
    spi->rx_size = size;
    spi->rx_cur_ptr = 0;

    spiInitCtrl(spi->base, ENABLE, RX_FIFO_RDY);
    spiInitCtrl(spi->base, ENABLE, TX_FIFO_RDY);

    if (spi->m_IRQ >= 0)
        NVIC_EnableIRQ((IRQn_Type)spi->m_IRQ);

    return 0;
}


static int spiInterruptWrite(uint32_t portNum, void *buf, int size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    spi->txbuf = buf;
    spi->tx_size = size;
    spi->tx_cur_ptr = 0;

    //irqchip_set_prioritylevel(data->irq, 0xC0); // This function is not acceptable for ISP SPI
    spiInitCtrl(spi->base, ENABLE, TX_FIFO_RDY);

    return 0;
}

int spiDataRead(uint32_t portNum, uint32_t *buf, uint32_t size)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDataRead\n");
    spiDumpSFR(portNum);

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiEnableCs(spi, true);

    /* enable rx channel for master mode */
    if (spi->m_OpMode == SPI_MASTER)
        spiEnablePktcount(spi->base, ENABLE, size);    // Enable when Master Rx not

    spiChannelOn(portNum);

/*-----------------------------------------------------*/
    if (spi->m_RxTransferMode == SPI_POLLING_MODE)
        spiPollingRead(portNum, buf, size);
    else if (spi->m_RxTransferMode == SPI_INTERRUPT_MODE)
        spiInterruptRead(portNum, buf, size);
/*-----------------------------------------------------*/

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiEnableCs(spi, false);

    /* enable rx channel for master mode */
    if (spi->m_OpMode == SPI_MASTER)
        spiEnablePktcount(spi->base, DISABLE, 0);    // Enable when Master Rx

    return 0;
}

int spiDataWrite(uint32_t portNum, uint32_t *buf, uint32_t size)
{
    int write_result = -1;
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDataWrite\n");
    spiDumpSFR(portNum);

    if (spi->m_EnSWReset)
        spiFifoFlush(spi->base);

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiEnableCs(spi, true);

    spiChannelOn(portNum);

/*-----------------------------------------------------*/
    if (spi->m_TxTransferMode == SPI_POLLING_MODE)
        write_result = spiPollingWrite(portNum, buf, size);
    else if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE)
        write_result = spiInterruptWrite(portNum, buf, size);
/*-----------------------------------------------------*/

    if ((spi->m_OpMode == SPI_MASTER) && (spi->m_EnAutoCS == false/*FALSE*/) && (spi->m_NSSOut != SPI_NSSOUT_USER_MODE))
        spiEnableCs(spi, false);

    return write_result;
}

int spiDataWriteRead(uint32_t portNum, uint32_t *txbuf, uint32_t *rxbuf, uint32_t size)
{
    struct spi_device *spi;
    int ret=0;

    spi =  &mSpiDev[portNum];

    SPI_DEBUG_PRINT("spiDataWriteRead portNum: %d, txbuf: 0x%x , rxbuf: 0x%x, size: %d\n", (int)portNum, (int)txbuf, (int)rxbuf, (int)size);

    if (spi->m_EnSWReset)
        spiFifoFlush(spi->base);

    if(spi->m_loopback)
        spiSetSelfloopback(spi->base, true);

    if (spi->m_TxTransferMode == SPI_POLLING_MODE)
        ret= spiPollingWriteRead(portNum, txbuf, rxbuf, size);
    else if (spi->m_TxTransferMode == SPI_INTERRUPT_MODE)
        ret= spiInterruptWriteRead(portNum, txbuf, rxbuf, size);


    if(spi->m_loopback)
        spiSetSelfloopback(spi->base, false);

    return ret;
}

void spi_IRQHandler(uint32_t busId)
{
    struct spi_device *spi = &mSpiDev[busId];
    uint32_t spiStatus, spiIntEn, val;

    perfMonitorSetEvt(chub_prof_spi_irq);
    spiStatus = __raw_readl(spi->base + REG_SPI_STATUS);
    spiIntEn = __raw_readl(spi->base + REG_SPI_INT_EN);

    //osLog(LOG_INFO, "STATUS 0x%X INT_EN 0x%X MOD 0x%X\n", (unsigned int)spiStatus, (unsigned int)spiIntEn, __raw_readl(spi->base + REG_MODE_CFG));
    val = spiStatus & spiIntEn;

    if (val & RX_FIFO_RDY)
        spiIntRead(spi);

    if (val & TX_FIFO_RDY)
        spiIntWrite(spi);

    if (val & RX_OVERRUN)
        spiIntRRxOverrun(spi);

    if (val & RX_UNDERRUN)
        spiIntRxUnderrun(spi);

    if (val & TX_UNDERRUN)
        spiIntTxUnderrun(spi);

    if (val & TX_OVERRUN)
        spiIntTxOverrun(spi);

    /* Trailing Byte interrupt always occur. so this interrupt source needs to be checked after RX_FIFO_READY. */
    if (val & TRAILING)
        spiIntTrailByte(spi);

    perfMonitorSetEvt(chub_prof_spi_irq_out);
}

/* Public API to initialize SPI. This should be called when OS starts */

uint32_t spiMasterInit(uint8_t *port)
{
    SPI_DEBUG_PRINT("\n\nspiInit port: %x\n", (unsigned int)*port);

    if(*port >= SPI_CHANNEL_MAX)
        return (uint32_t)-1;

    //spiOpen(port);
    return 0;
}

void spiMasterTransfer(struct SpiDevice *dev, uint32_t port, uint32_t *txBuf, uint32_t *rxBuf, uint32_t size, const struct SpiMode *mode )
{
    int ret = 0;
    struct spi_device *spi;

    spi = &mSpiDev[port];

    spi->dev = dev;
    spi->tx_size = size;
    spi->rx_size = size;

    SPI_DEBUG_PRINT("spiTransfer\n");

    spiOpen(port, spi);

    if(txBuf && rxBuf)
        ret = spiDataWriteRead(port, txBuf, rxBuf, size);
    else if(txBuf)
        ret = spiDataWrite(port, txBuf, size);
    else
        ret = spiDataRead(port, rxBuf, size);

    (void)ret;
#if defined(SEOS)
    if (mSpiDev[port].m_RxTransferMode == SPI_POLLING_MODE)
        spiMasterRxTxDone(dev,ret);
#endif
}

int spiMasterDeinit(uint8_t *portNum)
{
    return 0;
}

void spiSetDeviceInfo(uint32_t portNum, const struct spi_device *spi_dev)
{
    struct spi_device *spi;

    spi =  &mSpiDev[portNum];
    SPI_DEBUG_PRINT("spiSetDeviceInfo\n");

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
    spi->m_multiSlave = spi_dev->m_multiSlave;

    if (spi_dev->m_slave < MAX_SLAVE_NUM) {
        spi->m_CS[spi_dev->m_slave] = spi_dev->m_GPIO;
        mSpiChannelStatus[portNum]=true;
    } else {
        CSP_PRINTF_ERROR("%s : Wrong slave number (%lu < %d)\n", __func__, spi_dev->m_slave, MAX_SLAVE_NUM);
        mSpiChannelStatus[portNum]=false;
    }
}

static int exynosSpiRxTx(struct SpiDevice *dev, void *rxBuf, const void *txBuf,
        size_t size, const struct SpiMode *mode)
{
    uint8_t *portNum = (uint8_t *)dev->pdata;

    SPI_DEBUG_PRINT("%s: piTransfer - mode info\n",__func__);
    SPI_DEBUG_PRINT("%s: bitsPerWord: %d\n",__func__,  mode->bitsPerWord);
    SPI_DEBUG_PRINT("%s: cpha: %d\n",__func__,  mode->cpha);
    SPI_DEBUG_PRINT("%s: cpol: %d\n",__func__, mode->cpol);
    SPI_DEBUG_PRINT("%s: format: %d\n",__func__, mode->format);
    SPI_DEBUG_PRINT("%s: nssChange: %d\n",__func__,  mode->nssChange);
    SPI_DEBUG_PRINT("%s: speed: %d\n",__func__, (int)mode->speed);
    SPI_DEBUG_PRINT("%s: size: %d\n",__func__, (int)size);

    spiMasterTransfer(dev, *portNum, (uint32_t*)txBuf, (uint32_t*)rxBuf, (uint32_t)size, mode);

    return 0;
}

static int exynosSpiRelease(struct SpiDevice *dev)
{
    uint8_t portNum = *(uint8_t *)dev->pdata;

    if(spiMasterDeinit(&m_portNum[portNum]))
        return -1;

    m_portNum[portNum] = 0;

    return 0;
}

static int exynosStartSync(struct SpiDevice *dev, spi_cs_t cs,
                           const struct SpiMode *mode)
{
    mSpiDev[*(uint8_t *)(dev->pdata)].m_cs = cs;

    return 0;
}

static const struct SpiDevice_ops mExynosSpiOps = {
    .masterStartSync = exynosStartSync,
    .masterRxTx = exynosSpiRxTx,
    .masterStopSync = NULL,

    .slaveStartSync = NULL,
    .slaveIdle = NULL,
    .slaveRxTx = NULL,
    .slaveStopSync = NULL,

    .slaveSetCsInterrupt = NULL,
    .slaveCsIsActive = NULL,

    .release = exynosSpiRelease,
};

int spiRequest(struct SpiDevice *dev, uint8_t busId)
{
    dev->ops = &mExynosSpiOps;
    m_portNum[busId] = busId;

    if(spiMasterInit(&m_portNum[busId]))
        return -1;

    dev->pdata = (void *)&m_portNum[busId];

    return 0;
}

struct SpiDeviceState {
    struct SpiDevice dev;

    const struct SpiPacket *packets;
    size_t n;
    size_t currentBuf;
    struct SpiMode mode;

    SpiCbkF rxTxCallback;
    void *rxTxCookie;

    SpiCbkF finishCallback;
    void *finishCookie;

    int err;
};
#define SPI_DEVICE_TO_STATE(p) ((struct SpiDeviceState *)p)

int spiOpen(uint32_t portNum, struct spi_device *spiCfg)
{
    SPI_DEBUG_PRINT("spiOpen portNum: %d\n", (unsigned int)portNum);

    mSpiDev[portNum].base = mSpiSfrBase[portNum];
    mSpiDev[portNum].m_portId = portNum;
    if (spiCfg) {
        mSpiDev[portNum].dev = spiCfg->dev;
        mSpiDev[portNum].m_RxReadyLevel = spiCfg->rx_size;
        if ( spiCfg->rx_size >= RX_FIFO_MAX)
            mSpiDev[portNum].m_RxReadyLevel = RX_FIFO_MAX;
        //osLog(LOG_INFO, "spiOpen %d %d\n", spiCfg->tx_size, spiCfg->tx_size);
    }

    if(spiSetup(portNum))
        return -1;

    return 0;
}

int spiRead(uint32_t port, uint32_t addr, void* rxBuf, size_t size)
{
    struct spi_device *spi;
    uint32_t pTempBuf;
    int ret;

    spi = &mSpiDev[port];

    if (size > sizeof(mSpiTempBuf) - 1)
        CSP_PRINTF_ERROR("%s : size %d is larger than max size\n", __func__, size);

    pTempBuf = (uint32_t)mSpiTempBuf;
    if (spi->m_BusWidth == SPI_BYTE){
        if(spi->m_RWBit)
            *(uint8_t *)pTempBuf = (uint8_t)(addr | 0x80);
        else
            *(uint8_t *)pTempBuf = (uint8_t)(addr);
        pTempBuf++;
    }
    else if (spi->m_BusWidth == SPI_HWORD){
        if(spi->m_RWBit)
            *(uint16_t *)pTempBuf = (uint16_t)(addr | 0x8000);
        else
            *(uint16_t *)pTempBuf = (uint16_t)(addr);
        pTempBuf += 2;
    }
    else {
        if(spi->m_RWBit)
            *(uint32_t *)pTempBuf = (uint32_t)(addr | 0x80000000);
        else
            *(uint32_t *)pTempBuf = (uint32_t)(addr);
        pTempBuf += 4;
    }

    ret = spiDataWriteRead(port, mSpiTempBuf, mSpiTempBuf, size + 1);

    if (ret)
        return ret;

    if (spi->m_BusWidth == SPI_BYTE){
        memcpy((uint8_t *)rxBuf, (uint8_t *)mSpiTempBuf + 1, size);
    }
    else if (spi->m_BusWidth == SPI_HWORD){
        memcpy((uint16_t *)rxBuf, (uint16_t *)mSpiTempBuf + 1, size);
    }
    else {
        memcpy((uint32_t *)rxBuf, (uint32_t *)mSpiTempBuf + 1, size);
    }

    return 0;
}

int spiReadMulti(uint32_t port, uint32_t slave, uint32_t addr, void* rxBuf, size_t size)
{
    const spi_device_t *cfg;
    cfg = spiGetBoardConf(port, slave);
    if (cfg == NULL) {
        CSP_PRINTF_ERROR("%s : No corresponding port and slave(%lu, %lu)\n", __func__, port, slave);
        return -1;
    }

    cfg = spiGetBoardConf(port, slave);
    mSpiDev[port].m_cs = cfg->m_GPIO;
    mSpiDev[port].m_RWBit = cfg->m_RWBit;
    mSpiDev[port].m_CPOL = cfg->m_CPOL;
    mSpiDev[port].m_CPHA = cfg->m_CPHA;
    spiSetChannel(&mSpiDev[port]);

    return spiRead(port, addr, rxBuf, size);
}

int spiWrite(uint32_t port, uint32_t addr, void* txBuf, size_t size)
{
    struct spi_device *spi;
    uint32_t pTempBuf = (uint32_t)mSpiTempBuf;
    int ret;

    spi = &mSpiDev[port];

    if (size > sizeof(mSpiTempBuf) - 1)
        CSP_PRINTF_ERROR("%s : size %d is larger than max size\n", __func__, size);

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

    ret = spiDataWriteRead(port, mSpiTempBuf, NULL, size + 1);

    if (ret)
        return ret;

    return 0;
}

int spiWriteMulti(uint32_t port, uint32_t slave, uint32_t addr, void* txBuf, size_t size)
{
    const spi_device_t *cfg;

    cfg = spiGetBoardConf(port, slave);
    if (cfg == NULL) {
        CSP_PRINTF_ERROR("%s : No corresponding port and slave(%lu, %lu)\n", __func__, port, slave);
        return -1;
    }
    mSpiDev[port].m_cs = cfg->m_GPIO;
    mSpiDev[port].m_RWBit = cfg->m_RWBit;
    mSpiDev[port].m_CPOL = cfg->m_CPOL;
    mSpiDev[port].m_CPHA = cfg->m_CPHA;
    spiSetChannel(&mSpiDev[port]);

    return spiWrite(port, addr, txBuf, size);
}

void spiSaveState(void)
{
}

void spiRestoreState(void)
{
    int i;

    for (i = 0 ; i < SPI_CHUB_CHANNEL_MAX ; i++){
        if (mSpiChannelStatus[i])
            spiOpen(i, NULL);
    }
}

void spiCmgpSaveState(void)
{
}

void spiCmgpRestoreState(void)
{
    int i;

    for (i = SPI_CHUB_CHANNEL_MAX ; i < SPI_CHANNEL_MAX ; i++){
        if (mSpiChannelStatus[i])
            spiOpen(i, NULL);
    }
}
