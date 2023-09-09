/*----------------------------------------------------------------------------
 *      Exynos SoC  -  SERIAL-LIF
 *----------------------------------------------------------------------------
 *      Name:    slif.c
 *      Purpose: To implement SERIAL_LIF APIs
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

#include <csp_common.h>
#include <cpu/cpu.h>
#if defined(SEOS)
    #include <errno.h>
    #include <cmsis.h>
#endif
#include <slif.h>
#include <variant.h>

#define REG_SLIF_INT_EN_SET           (SERIAL_LIF_BASE_ADDRESS + 0x040)
#define REG_SLIF_INT_EN_CLR           (SERIAL_LIF_BASE_ADDRESS + 0x044)
#define REG_SLIF_INT_PEND             (SERIAL_LIF_BASE_ADDRESS + 0x048)
#define REG_SLIF_INT_CLR              (SERIAL_LIF_BASE_ADDRESS + 0x04C)
#define REG_SLIF_CTRL                 (SERIAL_LIF_BASE_ADDRESS + 0x100)
#define REG_SLIF_CONFIG_TX            (SERIAL_LIF_BASE_ADDRESS + 0x104)
#define REG_SLIF_CONFIG_RX            (SERIAL_LIF_BASE_ADDRESS + 0x108)
#define REG_SLIF_CONFIG_DONE          (SERIAL_LIF_BASE_ADDRESS + 0x10C)
#define REG_SLIF_SAMPLE_PCNT          (SERIAL_LIF_BASE_ADDRESS + 0x110)
#define REG_SLIF_INPUT_EN             (SERIAL_LIF_BASE_ADDRESS + 0x114)
#define REG_SLIF_CHANNEL_MAP          (SERIAL_LIF_BASE_ADDRESS + 0x138)
#define REG_SLIF_CONTROL_AXI_WMASTER  (SERIAL_LIF_BASE_ADDRESS + 0x300)
#define REG_SLIF_W_STARTADDR          (SERIAL_LIF_BASE_ADDRESS + 0x304)
#define REG_SLIF_W_ENDADDR            (SERIAL_LIF_BASE_ADDRESS + 0x308)
#define REG_SLIF_READ_POINTER0        (SERIAL_LIF_BASE_ADDRESS + 0x30C)
#define REG_SLIF_READ_POINTER1        (SERIAL_LIF_BASE_ADDRESS + 0x310)
#define REG_SLIF_READ_POINTER2        (SERIAL_LIF_BASE_ADDRESS + 0x314)
#define REG_SLIF_READ_POINTER3        (SERIAL_LIF_BASE_ADDRESS + 0x318)
#define REG_SLIF_CONTROL_AXI_RMASTER  (SERIAL_LIF_BASE_ADDRESS + 0x400)
#define REG_SLIF_R_STARTADDR          (SERIAL_LIF_BASE_ADDRESS + 0x404)
#define REG_SLIF_R_ENDADDR            (SERIAL_LIF_BASE_ADDRESS + 0x408)
#define REG_SLIF_WRITE_POINTER0       (SERIAL_LIF_BASE_ADDRESS + 0x40C)
#define REG_SLIF_WRITE_POINTER1       (SERIAL_LIF_BASE_ADDRESS + 0x410)
#define REG_SLIF_WRITE_POINTER2       (SERIAL_LIF_BASE_ADDRESS + 0x414)
#define REG_SLIF_WRITE_POINTER3       (SERIAL_LIF_BASE_ADDRESS + 0x418)

// CTRL
#define SLIF_AXI_QCH_DISABLE  (0x1 << 20)
#define SLIF_APB_QCH_DISABLE  (0x1 << 16)
#define SLIF_LOOPBACK_ENABLE  (0x1 << 12)
#define SLIF_RX_ENABLE        (0x1 << 4)
#define SLIF_TX_ENABLE        (0x1 << 0)

// CONTROL_TX , CONTROL_RX
#define SLIF_BCLK_POLAR_BIT           24
#define SLIF_WS_MODE_BIT              20
#define SLIF_BCLK_SEL_BIT             16
#define SLIF_DIFF_MSIF_STR_BIT        12
#define SLIF_WS_POLAR_BIT             8
#define SLIF_OPMODE_BIT               0
#define SLIF_OPMODE_MASK              0x1F

// CONTROL_AXI_WMASTER
#define SLIF_ENABLE_AXI_MASTER        (0x1 << 8)
#define SLIF_TRIG_POINTER_EN3         (0x1 << 7)
#define SLIF_TRIG_POINTER_EN2         (0x1 << 6)
#define SLIF_TRIG_POINTER_EN1         (0x1 << 5)
#define SLIF_TRIG_POINTER_EN0         (0x1 << 4)
#define SLIF_ADDR_WRAP_EN             (0x1 << 0)

// CONFIG_DONE
#define RX_CONFIG_DONE       (0x1 << 8)
#define TX_CONFIG_DONE       (0x1 << 4)
#define ALL_CONFIG_DONE      (0x1 << 0)

#define SLIF_DEFAULT_CTRL  (SLIF_RX_ENABLE | SLIF_TX_ENABLE)

#define SLIF_TX_BCLK_POLAR         BCLK_RISING_EDGE
#define SLIF_TX_WS_MODE            WS_MODE_LRCK
#define SLIF_TX_BCLK               BCLK_INTERNAL
#define SLIF_TX_WS_POLAR           WS_ACTIVE_HIGH
#define SLIF_TX_OPMODE             ((MODE_PACKED_16BIT << 3)|(USE_2CH))

#define SLIF_RX_BCLK_POLAR         BCLK_RISING_EDGE
#define SLIF_RX_WS_MODE            WS_MODE_LRCK
#define SLIF_RX_BCLK               BCLK_EXTERNAL
#define SLIF_RX_WS_POLAR           WS_ACTIVE_HIGH
#define SLIF_RX_OPMODE             ((MODE_PACKED_16BIT << 3)|(USE_2CH))

void (*mSlifHandler[SLIF_INTERRUPT_NUM])(void);


int slifInit(uint32_t txBufAddr, uint32_t txBufSize, uint32_t rxBufAddr, uint32_t rxBufSize)
{
    uint32_t regValue;

    if ((txBufAddr & 0x3) || txBufSize == 0 || (rxBufAddr & 0x3) || rxBufSize ==0) {
        CSP_PRINTF_ERROR("SLIF Init Error : Tx(%x %d), Rx(%x %d\n",
                    (unsigned int)txBufAddr, (unsigned int)txBufSize,
                    (unsigned int)rxBufAddr, (unsigned int)rxBufSize);
        return -1;
    }

    slifStopAll();

    NVIC_DisableIRQ(SERIAL_LIF_IRQn);
    slifDisableInterrupt(0xFFFFFFFF);
    regValue =  slifGetPendingStatus();
    slifClearPending(regValue);

    __raw_writel(SLIF_DEFAULT_CTRL, REG_SLIF_CTRL);

    slifSetTxMode(SLIF_TX_BCLK_POLAR, SLIF_TX_WS_MODE, SLIF_TX_BCLK, SLIF_TX_WS_POLAR, SLIF_TX_OPMODE);
    slifSetRxMode(SLIF_RX_BCLK_POLAR, SLIF_RX_WS_MODE, SLIF_RX_BCLK, SLIF_RX_WS_POLAR, SLIF_RX_OPMODE);

    slifSetWriteArea(rxBufAddr, rxBufAddr + rxBufSize);
//    slifSetWriteTrigPoint(0, rxBufAddr + rxBufSize - 4);
    regValue = __raw_readl(REG_SLIF_CONTROL_AXI_WMASTER);
    regValue |= SLIF_ENABLE_AXI_MASTER | SLIF_ADDR_WRAP_EN;
    __raw_writel(regValue, REG_SLIF_CONTROL_AXI_WMASTER);

    slifSetReadArea(txBufAddr, txBufAddr + txBufSize);
 //   slifSetReadTrigPoint(0, txBufAddr + txBufSize - 4);
    regValue = __raw_readl(REG_SLIF_CONTROL_AXI_RMASTER);
    regValue |= SLIF_ENABLE_AXI_MASTER | SLIF_ADDR_WRAP_EN;
    __raw_writel(regValue, REG_SLIF_CONTROL_AXI_RMASTER);

    slifEnableInterrupt(0x3FFC0);
    NVIC_EnableIRQ(SERIAL_LIF_IRQn);
    return 0;
}

void slifEnableInterrupt(uint32_t irq)
{
    __raw_writel(irq, REG_SLIF_INT_EN_SET);
}

void slifDisableInterrupt(uint32_t irq)
{
    __raw_writel(irq, REG_SLIF_INT_EN_CLR);
}

uint32_t slifGetPendingStatus(void)
{
    return __raw_readl(REG_SLIF_INT_PEND);
}

void slifClearPending(uint32_t pend)
{
    __raw_writel(pend, REG_SLIF_INT_CLR);
}

void slifEnableRx(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CTRL);
    regValue |= SLIF_RX_ENABLE;
    __raw_writel(regValue, REG_SLIF_CTRL);
}

void slifDisableRx(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CTRL);
    regValue &= ~SLIF_RX_ENABLE;
    __raw_writel(regValue, REG_SLIF_CTRL);
}

void slifEnalbeTx(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CTRL);
    regValue |= SLIF_TX_ENABLE;
    __raw_writel(regValue, REG_SLIF_CTRL);
}

void slifDisableTX(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CTRL);
    regValue &= ~SLIF_TX_ENABLE;
    __raw_writel(regValue, REG_SLIF_CTRL);
}

void slifEnableLoopback(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CTRL);
    regValue |= SLIF_LOOPBACK_ENABLE;
    __raw_writel(regValue, REG_SLIF_CTRL);
}

void slifDisableLoopback(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CTRL);
    regValue &= ~SLIF_LOOPBACK_ENABLE;
    __raw_writel(regValue, REG_SLIF_CTRL);
}

void slifSetBclkPolar(uint32_t dir, uint32_t polar)
{
    uint32_t regValue;

    if (dir == SLIF_TX) {
        regValue = __raw_readl(REG_SLIF_CONFIG_TX);
    } else {
        regValue = __raw_readl(REG_SLIF_CONFIG_RX);
    }

    if (polar)
        regValue |= (0x1 << SLIF_BCLK_POLAR_BIT);
    else
        regValue &= ~(0x1 << SLIF_BCLK_POLAR_BIT);

    if (dir == SLIF_TX) {
        __raw_writel(regValue, REG_SLIF_CONFIG_TX);
    } else {
        __raw_writel(regValue, REG_SLIF_CONFIG_RX);
    }
}

void slifSetWSMode(uint32_t dir, uint32_t mode)
{
    uint32_t regValue;

    if (dir == SLIF_TX) {
        regValue = __raw_readl(REG_SLIF_CONFIG_TX);
    } else {
        regValue = __raw_readl(REG_SLIF_CONFIG_RX);
    }

    if (mode)
        regValue |= (0x1 << SLIF_WS_MODE_BIT);
    else
        regValue &= ~(0x1 << SLIF_WS_MODE_BIT);

    if (dir == SLIF_TX) {
        __raw_writel(regValue, REG_SLIF_CONFIG_TX);
    } else {
        __raw_writel(regValue, REG_SLIF_CONFIG_RX);
    }
}

void slifSelectBclk(uint32_t dir, uint32_t bclk)
{
    uint32_t regValue;

    if (dir == SLIF_TX) {
        regValue = __raw_readl(REG_SLIF_CONFIG_TX);
    } else {
        regValue = __raw_readl(REG_SLIF_CONFIG_RX);
    }

    if (bclk)
        regValue |= (0x1 << SLIF_BCLK_SEL_BIT);
    else
        regValue &= ~(0x1 << SLIF_BCLK_SEL_BIT);

    if (dir == SLIF_TX) {
        __raw_writel(regValue, REG_SLIF_CONFIG_TX);
    } else {
        __raw_writel(regValue, REG_SLIF_CONFIG_RX);
    }
}

void slifSetWSPolar(uint32_t dir, uint32_t polar)
{
    uint32_t regValue;

    if (dir == SLIF_TX) {
        regValue = __raw_readl(REG_SLIF_CONFIG_TX);
    } else {
        regValue = __raw_readl(REG_SLIF_CONFIG_RX);
    }

    if (polar)
        regValue |= (0x1 << SLIF_WS_POLAR_BIT);
    else
        regValue &= ~(0x1 << SLIF_WS_POLAR_BIT);

    if (dir == SLIF_TX) {
        __raw_writel(regValue, REG_SLIF_CONFIG_TX);
    } else {
        __raw_writel(regValue, REG_SLIF_CONFIG_RX);
    }
}

void slifSetOpMode(uint32_t dir, uint32_t mode)
{
    uint32_t regValue;

    if (dir == SLIF_TX) {
        regValue = __raw_readl(REG_SLIF_CONFIG_TX);
    } else {
        regValue = __raw_readl(REG_SLIF_CONFIG_RX);
    }

    regValue &= ~(SLIF_OPMODE_MASK);
    regValue |= (mode & SLIF_OPMODE_MASK);

    if (dir == SLIF_TX) {
        __raw_writel(regValue, REG_SLIF_CONFIG_TX);
    } else {
        __raw_writel(regValue, REG_SLIF_CONFIG_RX);
    }
}

void slifSetTxMode(uint32_t bclkpol, uint32_t wsmode, uint32_t bclk, uint32_t wspolar, uint32_t opmode)
{
    slifSetBclkPolar(SLIF_TX, bclkpol);
    slifSetWSMode(SLIF_TX, wsmode);
    slifSelectBclk(SLIF_TX, bclk);
    slifSetWSPolar(SLIF_TX, wspolar);
    slifSetOpMode(SLIF_TX, opmode);
}

void slifSetRxMode(uint32_t bclkpol, uint32_t wsmode, uint32_t bclk, uint32_t wspolar, uint32_t opmode)
{
    slifSetBclkPolar(SLIF_RX, bclkpol);
    slifSetWSMode(SLIF_RX, wsmode);
    slifSelectBclk(SLIF_RX, bclk);
    slifSetWSPolar(SLIF_RX, wspolar);
    slifSetOpMode(SLIF_RX, opmode);
}


void slifStartRx()
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONFIG_DONE);
    regValue |= RX_CONFIG_DONE;
    __raw_writel(regValue, REG_SLIF_CONFIG_DONE);
}

void slifStopRx()
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONFIG_DONE);
    regValue &= ~RX_CONFIG_DONE;
    __raw_writel(regValue, REG_SLIF_CONFIG_DONE);
}

void slifStartTx()
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONFIG_DONE);
    regValue |= TX_CONFIG_DONE;
    __raw_writel(regValue, REG_SLIF_CONFIG_DONE);
}

void slifStopTx()
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONFIG_DONE);
    regValue &= ~TX_CONFIG_DONE;
    __raw_writel(regValue, REG_SLIF_CONFIG_DONE);
}

void slifStartAll()
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONFIG_DONE);
    regValue |= ALL_CONFIG_DONE;
    __raw_writel(regValue, REG_SLIF_CONFIG_DONE);
}

void slifStopAll()
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONFIG_DONE);
    regValue &= ~(ALL_CONFIG_DONE | TX_CONFIG_DONE | RX_CONFIG_DONE);
    __raw_writel(regValue, REG_SLIF_CONFIG_DONE);
}

int slifSetWriteArea(uint32_t start, uint32_t end)
{
    if (start & 0x3 || end & 0x3) {
        CSP_PRINTF_ERROR("SLIF Set W_Address FAIL: start(%x), end(%x)\n", (unsigned int)start, (unsigned int)end);
        return -1;
    }

    __raw_writel(start, REG_SLIF_W_STARTADDR);
    __raw_writel(end, REG_SLIF_W_ENDADDR);

    return 0;
}

int slifSetWriteTrigPoint(uint32_t no, uint32_t addr)
{
    if (no > 3) {
        CSP_PRINTF_ERROR("SLIF set W_TRIG_POINT %d FAIL!\n", (unsigned int)no);
        return -1;
    }

    addr &= ~0x3;
    __raw_writel(addr, REG_SLIF_READ_POINTER0 + (no * 4));

    return 0;
}

void slifEnableWriteAddrWrap(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONTROL_AXI_WMASTER);
    regValue |= SLIF_ADDR_WRAP_EN;
    __raw_writel(regValue, REG_SLIF_CONTROL_AXI_WMASTER);
}

void slifDisableWriteAddrWrap(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONTROL_AXI_WMASTER);
    regValue &= ~SLIF_ADDR_WRAP_EN;
    __raw_writel(regValue, REG_SLIF_CONTROL_AXI_WMASTER);
}

int slifSetReadArea(uint32_t start, uint32_t end)
{
    if (start & 0x3 || end & 0x3) {
        CSP_PRINTF_ERROR("SLIF Set R_Address FAIL: start(%x), end(%x)\n", (unsigned int)start, (unsigned int)end);
        return -1;
    }

    __raw_writel(start, REG_SLIF_R_STARTADDR);
    __raw_writel(end, REG_SLIF_R_ENDADDR);

    return 0;
}

int slifSetReadTrigPoint(uint32_t no, uint32_t addr)
{
    if (no > 3) {
        CSP_PRINTF_ERROR("SLIF set R_TRIG_POINT %d FAIL!\n", (unsigned int)no);
        return -1;
    }

    addr &= ~0x3;
    __raw_writel(addr, REG_SLIF_WRITE_POINTER0 + (no * 4));

    return 0;
}

void slifEnableReadAddrWrap(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONTROL_AXI_RMASTER);
    regValue |= SLIF_ADDR_WRAP_EN;
    __raw_writel(regValue, REG_SLIF_CONTROL_AXI_RMASTER);
}

void slifDisableReadAddrWrap(void)
{
    uint32_t regValue;

    regValue = __raw_readl(REG_SLIF_CONTROL_AXI_RMASTER);
    regValue &= ~SLIF_ADDR_WRAP_EN;
    __raw_writel(regValue, REG_SLIF_CONTROL_AXI_RMASTER);
}

void slif_IRQHandler(void)
{
    uint32_t irq;
    uint32_t i;

    irq = slifGetPendingStatus();

    for (i = 0 ; i < SLIF_INTERRUPT_NUM ; i++) {
        if ((irq & (0x1 << i)) && mSlifHandler[i] != NULL)
            mSlifHandler[i]();
    }

    slifClearPending(irq);
}

void slifSetHandler(int irq, void (*handler)(void))
{
    int i;

    for (i = 0 ; i < SLIF_INTERRUPT_NUM ; i++) {
        if (irq & (0x1 << i))
            mSlifHandler[i] = handler;
    }
}
