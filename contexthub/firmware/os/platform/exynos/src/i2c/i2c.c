/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C
 *----------------------------------------------------------------------------
 *      Name:    i2c.c
 *      Purpose: To implement I2C APIs
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
#include <csp_printf.h>
#if defined(SEOS)
    #include <seos.h>
    #include <errno.h>
    #include <heap.h>
#elif defined(EMBOS)
    #include <RTOS.h>
    #include <Device.h>
#endif
#include <cmsis.h>
#include <plat/i2c/i2c.h>
#include <plat/cmu/cmuDrv.h>
#include <string.h>
#include <stdlib.h>
#include <sysreg.h>
#include <cmgp.h>
#include <plat/gpio/gpio.h>
#include <debug.h>

// SFR ADDRESS OFFSETs
#define REG_I2C_CTL             0x00
#define REG_I2C_FIFO_CTL        0x04
#define REG_I2C_TRAILING_CTL    0x08
#define REG_I2C_INT_EN          0x20
#define REG_I2C_INT_STAT        0x24
#define REG_I2C_FIFO_STAT       0x30
#define REG_I2C_TXDATA          0x34
#define REG_I2C_RXDATA          0x38
#define REG_I2C_CONF            0x40
#define REG_I2C_CONF2           0x44
#define REG_I2C_TIMEOUT         0x48
#define REG_I2C_CMD             0x4C
#define REG_I2C_TRANS_STATUS    0x50
#define REG_I2C_TIMING_HS1      0x54
#define REG_I2C_TIMING_HS2      0x58
#define REG_I2C_TIMING_HS3      0x5C
#define REG_I2C_TIMING_FS1      0x60
#define REG_I2C_TIMING_FS2      0x64
#define REG_I2C_TIMING_FS3      0x68
#define REG_I2C_TIMING_SLA      0x6C
#define REG_I2C_ADDR            0x70

#define I2C_MODE                    I2C_MODE_POLLING

#define I2C_RESET                   (0x1<<31)

#define I2C_RXCH_ON                 (0x1<<6)

#define I2C_TX_FIFO_RESET           (0x1<<3)
#define I2C_RX_FIFO_RESET           (0x1<<2)
#define I2C_RXFIFO_EN               (0x1<<0)
#define I2C_TXFIFO_EN               (0x1<<1)
#define I2C_RXFIFO_TRIGGER_LEVEL    (0x8 << 4)
#define I2C_TXFIFO_TRIGGER_LEVEL    (0x8 << 16)

#define I2C_DEFAULT_FILTER_SDA      7

// I2C_CONF2 Register
#define I2C_READ_WRITE              (0x1<<16)
#define I2C_STOP_AFTER_TRANS        (0x1<<17)
#define I2C_MASTER_RUN              (0x1<<31)



#define I2C_AUTO_MODE               (0x1<<31)
#define I2C_HS_MODE                 (0x1<<29)
#define I2C_FAST_SPEED              0
#define I2C_HIGH_SPEED              1

// I2C_TRAILING_CTL Register bits
#define I2C_TRAILING_COUNT          (0x400)

// I2C_INT_EN Register bits
#define I2C_INT_TX_ALMOSTEMPTY_EN   (1u << 0)
#define I2C_INT_RX_ALMOSTFULL_EN    (1u << 1)
#define I2C_INT_TRAILING_EN         (1u << 6)
#define I2C_INT_TRANSFER_DONE       (1u << 7)
#define I2C_INT_NO_DEV_ACK_EN       (1u << 9)
#define I2C_INT_CHK_TRANS_STATE     (0xf << 8)

// FIFO_STAT Register bits
#define I2C_TX_FIFO_FULL                (0x1 << 7)
#define I2C_TX_FIFO_EMPTY               (0x1 << 8)
#define I2C_RX_FIFO_FULL                (0x1 << 23)
#define I2C_RX_FIFO_EMPTY               (0x1 << 24)


#define I2C_TIMEOUT_EN              (0x1<<31)

#define I2C_MASTER_STATUS_MASK              (0xF << 0)
#define I2C_MASTER_STATUS_IDLE              (0x0)
#define I2C_MASTER_STATUS_INIT              (0x1)
#define I2C_MASTER_STATUS_START             (0x2)
#define I2C_MASTER_STATUS_SLA_ADDR          (0x3)
#define I2C_MASTER_STATUS_MASTERCODE        (0x4)
#define I2C_MASTER_STATUS_RELEASE_TERM      (0x5)
#define I2C_MASTER_STATUS_LOSE              (0x6)
#define I2C_MASTER_STATUS_10BIT_SLA_ADDR    (0x7)
#define I2C_MASTER_STATUS_READFIFO          (0x8)
#define I2C_MASTER_STATUS_WRITE             (0x9)
#define I2C_MASTER_STATUS_READ              (0xA)
#define I2C_MASTER_STATUS_WRITEFIFO         (0xB)
#define I2C_MASTER_STATUS_STOP              (0xC)
#define I2C_MASTER_STATUS_WAIT              (0xD)

#define I2C_STATE_CLOSE             (0x0)
#define I2C_STATE_OPEN              (0x1<<0)
#define I2C_STATE_BUSY              (0x1<<1)
#define I2C_STATE_ERROR             (0x1<<2)
#define I2C_STATE_REOPEN            (0x1<<3)
#define I2C_STATE_DONE              (0x1<<7)

#define I2C_TIMEOUT                 100000
#define I2C_FIFO_SIZE               16

#define I2C_TIMING_COFF_HS          7
#define I2C_TIMING_COFF_FS          9

#define PRINTF_DEBUG CSP_PRINTF_DEBUG
#define PRINTF_INFO  CSP_PRINTF_INFO
#define PRINTF_ERROR CSP_PRINTF_ERROR

static int i2cCheckChannel(uint32_t busid);
static void i2cReset(uint32_t busId);
static void i2cInitTiming(uint32_t busId);
static void i2cMasterReset(uint32_t busId);
static int i2cCheckIdle(uint32_t busId);
static int i2cMasterTransferPolling(uint32_t busId);
static int i2cMasterTransfer(uint32_t busId);
static void i2cDumpSFR(uint32_t busId);

#if defined(SEOS)
  ATOMIC_BITSET_DECL(mI2cFlag, I2C_CHANNEL_MAX, static);
#elif defined(EMBOS)
  OS_RSEMA mI2cSema[I2C_CHANNEL_MAX];
#endif

#include CSP_SOURCE(i2c)

static inline void i2cStop(uint32_t busId)
{
    __raw_writel(0, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);
}

void i2c_IRQHandler(uint32_t busId)
{
    uint32_t intStat;
    uint32_t intEn;
    //uint32_t regValue;
    uint8_t data;
    int err;
#if defined(SEOS)
    uint16_t oldTid;
#endif

    perfMonitorSetEvt(chub_prof_i2c_irq);

    err = 0;

    intEn = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);
    __raw_writel(0, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);

    intStat = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
    __raw_writel(intStat, mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);

    //osLog(LOG_DEBUG, "REG_I2C_INT_STAT 0X%X\n", (unsigned int)intStat);

    if ((mI2cInfo[busId].msg.flags & I2C_M_RD) &&
        (intStat & (I2C_INT_RX_ALMOSTFULL_EN | I2C_INT_TRAILING_EN))) {
        //osLog(LOG_DEBUG, "i2c_IRQ READ 0x%X\n", __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_STAT));
        while(!(__raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_STAT) &
                I2C_RX_FIFO_EMPTY)) {
            data = (uint8_t)__raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_RXDATA);
            mI2cInfo[busId].msg.rxBuf[mI2cInfo[busId].msg.rxPtr++] = data;
        }

        if (mI2cInfo[busId].msg.rxPtr >= mI2cInfo[busId].msg.rxSize) {
            //regValue = __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);
            intEn &= ~I2C_INT_RX_ALMOSTFULL_EN;
            //__raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);
            mI2cInfo[busId].state |= I2C_STATE_DONE;

        }
    } else if (intStat & I2C_INT_TX_ALMOSTEMPTY_EN){
        //osLog(LOG_DEBUG, "i2c_IRQ WRITE\n");
        while(!(__raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_STAT) &
                I2C_TX_FIFO_FULL)) {
            if (mI2cInfo[busId].msg.txPtr >= mI2cInfo[busId].msg.txSize) {
                //intEn = __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);
                intEn &= ~I2C_INT_TX_ALMOSTEMPTY_EN;
                //__raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);

                if (mI2cInfo[busId].stop == 1)
                    mI2cInfo[busId].state |= I2C_STATE_DONE;
                break;
            }
            data = mI2cInfo[busId].msg.txBuf[mI2cInfo[busId].msg.txPtr++];
            __raw_writel(data, mI2cInfo[busId].sfrBase + REG_I2C_TXDATA);
        }
    }

    //Checking Error State in INT_STATUS register
    if (intStat & I2C_INT_CHK_TRANS_STATE) {
       osLog(LOG_ERROR, "I2C Error Interrupt occurred(IS:0x%08x, TR:0x%08x)\n",
               (unsigned int)intStat, __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TRANS_STATUS));

       err = -1;
       goto callback;
    }


    __raw_writel(intEn, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);

    if (mI2cInfo[busId].msg.flags & I2C_M_RD) {
        if (mI2cInfo[busId].state & I2C_STATE_DONE) {
            goto callback;
        }
    } else if (intStat & I2C_INT_TRANSFER_DONE) {
        if (mI2cInfo[busId].stop == 0) {
            if (busId < I2C_CHUB_CHANNEL_MAX) {
                NVIC_DisableIRQ((IRQn_Type)mI2cInfo[busId].irqNum);
            } else if (busId < I2C_CHANNEL_MAX) {
                cmgpDisableInterrupt(mI2cInfo[busId].irqNum);
            }
            mI2cInfo[busId].msg.flags |= I2C_M_RD;
            mI2cInfo[busId].stop = 1;
            i2cMasterTransfer(busId);
        } else if (mI2cInfo[busId].state & I2C_STATE_DONE) {
            goto callback;
        }
    }

    perfMonitorSetEvt(chub_prof_i2c_irq_out);

    return;

callback:
    i2cStop(busId);
    if (busId < I2C_CHUB_CHANNEL_MAX) {
        NVIC_DisableIRQ((IRQn_Type)mI2cInfo[busId].irqNum);
    } else if (busId < I2C_CHANNEL_MAX) {
        cmgpDisableInterrupt(mI2cInfo[busId].irqNum);
    }
    mI2cInfo[busId].state &= ~I2C_STATE_DONE;
    //osLog(LOG_DEBUG, "I2C Callback 0X%X cookie %x\n", (unsigned int)(mI2cInfo[busId].msg.callback), (unsigned int)(mI2cInfo[busId].msg.cookie));
#if defined(SEOS)
    atomicBitsetClearBit(mI2cFlag, busId);
#elif defined(EMBOS)
    OS_Unuse(&mI2cSema[busId]);
#endif
    if (mI2cInfo[busId].msg.callback) {
      #if defined(SEOS)
        oldTid = osSetCurrentTid(mI2cInfo[busId].tid);
        mI2cInfo[busId].msg.callback( mI2cInfo[busId].msg.cookie, mI2cInfo[busId].msg.txSize, mI2cInfo[busId].msg.rxSize, err);
        osSetCurrentTid(oldTid);
      #elif defined(EMBOS)
        mI2cInfo[busId].msg.callback( mI2cInfo[busId].msg.cookie, mI2cInfo[busId].msg.txSize, mI2cInfo[busId].msg.rxSize, err);
      #endif
    }

    perfMonitorSetEvt(chub_prof_i2c_irq_out);
}

static int i2cCheckChannel(uint32_t busId)
{
    if ( busId >= I2C_CHANNEL_MAX) {
        PRINTF_ERROR("I2C Invalid channel : %d\n", (int)busId);
        return -I2C_ERR_INVALID_VALUE;
    }

    return 0;
}

static void i2cDumpSFR(uint32_t busId)
{
    PRINTF_INFO("I2C state=%d, speed=%d, addr=%x, rxSize=%d, txSize=%d\n",
        (int)mI2cInfo[busId].state, (int)mI2cInfo[busId].speed, (unsigned int)mI2cInfo[busId].msg.addr,
        (int)mI2cInfo[busId].msg.rxSize, (int)mI2cInfo[busId].msg.txSize);
    PRINTF_INFO("I2C(%d) SFR DUMP\n", (int)busId);
    PRINTF_INFO("I2C CTL : %x\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_CTL));
    PRINTF_INFO("I2C FIFO_CTL : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_CTL));
    PRINTF_INFO("I2C TRAILING_CTL : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TRAILING_CTL));
    PRINTF_INFO("I2C INT_EN : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_INT_EN));
    PRINTF_INFO("I2C INT_STAT : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT));
    PRINTF_INFO("I2C FIFO_STAT : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_STAT));
    PRINTF_INFO("I2C CONF : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_CONF));
    PRINTF_INFO("I2C CONF2 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_CONF2));
    PRINTF_INFO("I2C TRANS STATUS : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TRANS_STATUS));
    PRINTF_INFO("I2C TIMING HS1 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS1));
    PRINTF_INFO("I2C TIMING HS2 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS2));
    PRINTF_INFO("I2C TIMING HS3 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS3));
    PRINTF_INFO("I2C TIMING FS1 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS1));
    PRINTF_INFO("I2C TIMING FS2 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS2));
    PRINTF_INFO("I2C TIMING FS3 : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS3));
    PRINTF_INFO("I2C ADDR : %X\n", __raw_read32(mI2cInfo[busId].sfrBase + REG_I2C_ADDR));
}

static void i2cReset(uint32_t busId)
{
    uint32_t regValue;

    // Reset Assert
    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CTL);
    regValue |= (uint32_t)I2C_RESET;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CTL);

    // Reset Release
    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CTL);
    regValue &= ~I2C_RESET;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CTL);
}

static void i2cInitTiming(uint32_t busId)
{
    uint32_t pClk;
    uint32_t opClk;
    uint32_t clk_div, tscl_h, tstart_hd;
    uint32_t regValue;

    pClk = cmuDrvGetSpeed((CmuIpType)(CMU_CLK_I2C + busId));
    opClk = mI2cInfo[busId].speed;

    if (mI2cInfo[busId].speedMode == I2C_MODE_HS) {
        clk_div = pClk / (opClk * 16);
        clk_div &= 0xFF;
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS3) & (~0x00FF0000);
        regValue |= (clk_div << 16);
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS3);

        tscl_h = ((I2C_TIMING_COFF_HS * pClk) / (1000 * 1000)) / ((clk_div + 1) * 100);
        tscl_h = (0xFFFFFFFF >> tscl_h) << tscl_h;
        tscl_h &= 0xFF;
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS2) & (~0x000000FF);
        regValue |= tscl_h;
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS2);

        tstart_hd = ((I2C_TIMING_COFF_HS * pClk) / (1000 * 1000)) / ((clk_div + 1) * 100) - 1;
        tstart_hd = (0xFFFFFFFF >> tstart_hd) << tstart_hd;
        tstart_hd &= 0xFF;
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS1) & (~0x00FF0000);
        regValue |= (tstart_hd << 16);
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMING_HS1);
    } else {
        clk_div = pClk / (opClk * 16);
        clk_div &= 0xFF;
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS3) & (~0x00FF0000);
        regValue |= (clk_div << 16);
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS3);

        tscl_h = ((I2C_TIMING_COFF_FS * pClk) / (1000 * 1000)) / ((clk_div + 1) * 10);
        tscl_h = (0xFFFFFFFF >> tscl_h) << tscl_h;
        tscl_h &= 0xFF;
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS2) & (~0x000000FF);
        regValue |= tscl_h;
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS2);

        tstart_hd = ((I2C_TIMING_COFF_FS * pClk) / (1000 * 1000)) / ((clk_div + 1) * 10) - 1;
        tstart_hd = (0xFFFFFFFF >> tstart_hd) << tstart_hd;
        tstart_hd &= 0xFF;
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS1) & (~0x00FF0000);
        regValue |= (tstart_hd << 16);
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMING_FS1);

        // Set STOP to IDLE time >= 1.3us (I2C_CONF)
        /*
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CONF);
        regValue &= ~0xFF;
        stop2idle = 1300*1000000/pClk - (clk_div+1)*8 + 1;
        regValue |= stop2idle;
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CONF);
        */
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CONF);
        regValue |= 0xFF;
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CONF);
    }
}

void i2cInit(void)
{
#if defined(SEOS)
    atomicBitsetInit(mI2cFlag, I2C_CHANNEL_MAX);
#elif defined(EMBOS)
    int i;
    
    for (i = 0; i < I2C_CHANNEL_MAX ; i++)
        OS_CREATERSEMA(&mI2cSema[i]); 
#endif
}

int i2cMasterRequest(uint32_t busId, uint32_t speedInHz)
{
    uint32_t regValue;

    I2C_DEBUG_PRINT("i2cMasterRequest : busId = %d, speed = %d\n", (int)busId, (int)speedInHz);

    if (i2cCheckChannel(busId))
        return -1;

    if (mI2cInfo[busId].state & I2C_STATE_OPEN &&
            !(mI2cInfo[busId].state & I2C_STATE_REOPEN)) {
        // PRINTF_INFO("I2C(%d) already opened\n", (int)busId);
        return 0;
    }

    mI2cInfo[busId].speed = speedInHz;
    mI2cInfo[busId].mode = I2C_MODE_MASTER;
    mI2cInfo[busId].state = 0;

    if (speedInHz > I2C_FS_CLOCK)
        mI2cInfo[busId].speedMode = I2C_MODE_HS;
    else
        mI2cInfo[busId].speedMode = I2C_MODE_FS;

    i2cReset(busId);

    i2cInitTiming(busId);

    __raw_writel(I2C_TRAILING_COUNT, mI2cInfo[busId].sfrBase + REG_I2C_TRAILING_CTL);

    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CONF);
    if (mI2cInfo[busId].speedMode == I2C_MODE_HS) {
        regValue |= I2C_HS_MODE;
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CONF);
    }
    regValue |= (unsigned int)I2C_AUTO_MODE;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CONF);

    mI2cInfo[busId].state = I2C_STATE_OPEN;
    mI2cInfo[busId].errorState = 0;

    return 0;
}

int i2cMasterRelease(uint32_t busId)
{
    if (i2cCheckChannel(busId))
        return -1;

    mI2cInfo[busId].state = 0;

    return 0;
}

static void i2cMasterReset(uint32_t busId)
{
    uint32_t speed;
    uint32_t gpioFuncScl, gpioFuncSda;
    uint32_t timeout;
    int sclVal, sdaVal;
    int clkCnt;

    GpioPinNumType pinSCL, pinSDA;

    if (i2cCheckChannel(busId))
        return;

    pinSCL = (GpioPinNumType)mI2cInfo[busId].gpioSCL;
    pinSDA = (GpioPinNumType)mI2cInfo[busId].gpioSDA;

    gpioFuncScl =  gpioGetConfig(pinSCL);
    gpioFuncSda =  gpioGetConfig(pinSDA);

    speed = mI2cInfo[busId].speed;

    i2cMasterRelease(busId);

    sdaVal = gpioGetData(pinSDA);
    sclVal = gpioGetData(pinSCL);

    PRINTF_INFO("busId %lu SDA line:%d, SCL line:%d\n", busId, sdaVal, sclVal);

    /*
    if (sdaVal == 1 && sclVal == 1)
        return;
    */

    /* Wait for SCL as high */
    if (sclVal == 0) {
        timeout = 0;
        while(timeout < 500) { // for 500ms
            sclVal = gpioGetData(pinSCL);
            if(sclVal == 0) {
                timeout = 0;
                break;
              }
            mSleep(10);
            timeout += 10;
        }

        if (timeout)
            PRINTF_ERROR("SCL line is still LOW!!!\n");
    }

    sdaVal = gpioGetData(pinSDA);

    if (sdaVal == 0) {
        gpioSetFunction(pinSCL, GPIO_CON_OUTPUT);
        gpioSetFunction(pinSDA, GPIO_CON_INPUT);

        for (clkCnt = 0 ; clkCnt < 100 ; clkCnt++) {
            gpioSetData(pinSCL, 0);
            uSleep(5);
            gpioSetData(pinSCL, 1);
            uSleep(5);
            if (gpioGetData(pinSDA) == 1) {
                PRINTF_ERROR("SDA line is recovered\n");
                break;
            }
        }
        if (clkCnt == 100)
            PRINTF_ERROR("SDA line is not recovered\n");
    }

    gpioSetFunction(pinSCL, gpioFuncScl);
    gpioSetFunction(pinSDA, gpioFuncSda);

    i2cReset(busId);
    i2cMasterRequest(busId, speed);
}

static int i2cCheckIdle(uint32_t busId)
{
    uint32_t regValue;
    int i;

    regValue = 0;

    for (i = 0 ; i < 50 ; i++) {
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TRANS_STATUS);
        if ((regValue & I2C_MASTER_STATUS_MASK) == I2C_MASTER_STATUS_INIT) {
            return 0;
        }
    }
    PRINTF_INFO("I2C Bus(%lu) is not IDLE: TRANS_STATUS = %x\n", busId, (unsigned int)(regValue));

    return -1;
}

static int i2cMasterTransferPolling(uint32_t busId)
{
    uint32_t regValue = 0;
    uint8_t  data;
    int timeout;
    int ret=1;


    ret = -I2C_ERR_TIMEOUT;

    if (mI2cInfo[busId].msg.flags & I2C_M_RD) {
        __raw_writel(I2C_INT_TRANSFER_DONE | I2C_INT_CHK_TRANS_STATE, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);
        timeout = I2C_TIMEOUT;

        while (timeout-- > 0) {
            if ((__raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_STAT) & I2C_RX_FIFO_EMPTY) == 0) {
                data = (uint8_t)__raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_RXDATA);
                mI2cInfo[busId].msg.rxBuf[mI2cInfo[busId].msg.rxPtr++] = data;
                timeout = I2C_TIMEOUT;
            }

            if (mI2cInfo[busId].msg.rxPtr >= mI2cInfo[busId].msg.rxSize) {
                timeout = I2C_TIMEOUT;
                while (timeout-- > 0) {
                    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
                    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
                    if (regValue & I2C_INT_CHK_TRANS_STATE)
                        break;
                    if (regValue & I2C_INT_TRANSFER_DONE) {
                        ret = 0;
                        break;
                    }
                }
                break;
            }
        }

        if (ret == -I2C_ERR_TIMEOUT) {
            PRINTF_DEBUG("I2C ERROR: RX Timeout, Rx count = %d\n", mI2cInfo[busId].msg.rxPtr);
            i2cDumpSFR(busId);
            return ret;
        }
    } else {
        __raw_writel(I2C_INT_TRANSFER_DONE, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);

        timeout = I2C_TIMEOUT;
        while ((timeout-- > 0) && (mI2cInfo[busId].msg.txPtr < mI2cInfo[busId].msg.txSize)) {
            if ((__raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_FIFO_STAT) & 0x7F) < I2C_FIFO_SIZE) {
                data = mI2cInfo[busId].msg.txBuf[mI2cInfo[busId].msg.txPtr++];
                __raw_writel(data, mI2cInfo[busId].sfrBase + REG_I2C_TXDATA);
            }
        }

        timeout = I2C_TIMEOUT;
        while (timeout-- > 0) {
            regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
            __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
            if (regValue & I2C_INT_CHK_TRANS_STATE)
                break;
            if (regValue & I2C_INT_TRANSFER_DONE) {
                ret = 0;
                break;
            }
        }

        if (ret == -I2C_ERR_TIMEOUT) {
            PRINTF_ERROR("I2C ERROR: TX Timeout, Tx count = %d\n", mI2cInfo[busId].msg.txPtr);
            PRINTF_ERROR("INT STAT: %x\n", (unsigned int)regValue);
            i2cDumpSFR(busId);
            return ret;
        }
    }

    return ret;
}

static int i2cMasterTransfer(uint32_t busId)
{
    uint32_t regValue;
    int ret = 0;

    // I2C_TIMEOUT
    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_TIMEOUT);
    regValue &= ~I2C_TIMEOUT_EN;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_TIMEOUT);

    // I2C_FIFO_CTL
    regValue = I2C_RXFIFO_EN | I2C_TXFIFO_EN | I2C_TXFIFO_TRIGGER_LEVEL;
    if (mI2cInfo[busId].msg.rxSize >= 0x8)
        regValue |= I2C_RXFIFO_TRIGGER_LEVEL;
    else
        regValue |= (mI2cInfo[busId].msg.rxSize << 4);
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_FIFO_CTL);

    if (mI2cInfo[busId].operationMode == I2C_MODE_INTERRUPT) {
        // I2C_INT_EN
        regValue = 0;
        if (mI2cInfo[busId].msg.flags & I2C_M_RD) {
            regValue |= I2C_INT_RX_ALMOSTFULL_EN |
                        I2C_INT_TRAILING_EN;
        } else {
            regValue |= I2C_INT_TX_ALMOSTEMPTY_EN;
        }
        regValue |= I2C_INT_CHK_TRANS_STATE |
                    I2C_INT_TRANSFER_DONE;
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_INT_EN);

        // Clear Pending
        regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
        __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_INT_STAT);
    }

    // I2C_ADDR
    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_ADDR);
    regValue &= (0x00F00000);
    regValue |= ((mI2cInfo[busId].msg.addr & 0x7F) << 10);
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_ADDR);

    // I2C_CTL configuration
    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CTL);
    if (mI2cInfo[busId].msg.flags & I2C_M_RD)
        regValue |= I2C_RXCH_ON;
    else
        regValue &= ~I2C_RXCH_ON;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CTL);

    // I2C_CONF2
    regValue = __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CONF2);
    regValue &= ~0xFFFF;
    if (mI2cInfo[busId].msg.flags & I2C_M_RD) {
        regValue |= I2C_READ_WRITE;
        regValue |= mI2cInfo[busId].msg.rxSize;
    } else {
        regValue &= ~I2C_READ_WRITE;
        regValue |= mI2cInfo[busId].msg.txSize;
    }
    if (mI2cInfo[busId].stop == 1)
        regValue |= I2C_STOP_AFTER_TRANS;
    else
        regValue &= ~I2C_STOP_AFTER_TRANS;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CONF2);

    regValue =  __raw_readl(mI2cInfo[busId].sfrBase + REG_I2C_CONF2);
    regValue |= (unsigned int)I2C_MASTER_RUN;
    __raw_writel(regValue, mI2cInfo[busId].sfrBase + REG_I2C_CONF2);

    if (mI2cInfo[busId].operationMode == I2C_MODE_INTERRUPT) {
        // Enable IRQ
        if (busId < I2C_CHUB_CHANNEL_MAX) {
            NVIC_EnableIRQ((IRQn_Type)mI2cInfo[busId].irqNum);
        } else if (busId < I2C_CHANNEL_MAX) {
            cmgpEnableInterrupt(mI2cInfo[busId].irqNum);
        }
    } else {
        ret = i2cMasterTransferPolling(busId);
    }

    return ret;
}

int i2cMasterTxRx(uint32_t busId, uint32_t addr, const void *txBuf, size_t txSize,
        void *rxBuf, size_t rxSize, I2cCallbackF callback, void *cookie)
{
    int ret = 0;
    int i = 0;

    //PRINTF_DEBUG("i2cMasterTxRx : txSize = %d, rxSize = %d\n", txSize, rxSize);
    perfMonitorSetEvt(chub_prof_i2c_in);

    if (i2cCheckChannel(busId))
        return -1;

    if (!(mI2cInfo[busId].state & I2C_STATE_OPEN)) {
        PRINTF_ERROR("i2cMasterTxRx : I2C(%d) is not openend\n", (int)busId);
        return -1;
    }

#if defined(SEOS)
     while(atomicBitsetGetBit(mI2cFlag, busId)) {
       
        PRINTF_ERROR("i2c Channel%lu busy\n", busId);
        i++;
        if ( i >= 50)
          return -1;
        uSleep(100);
    }

    atomicBitsetSetBit(mI2cFlag, busId);
#elif defined(EMBOS)
    while((ret = OS_UseTimed(&mI2cSema[busId], 10)) == 0) {
        PRINTF_ERROR("i2c Channel%lu busy\n", busId);
        i++;
        if ( i >= 50)
          return -1;
    }
#endif


    if (mI2cInfo[busId].errorState)
        i2cMasterReset(busId);

    if ( addr & 0x80 ) {
        PRINTF_ERROR("i2cMasterTxRx : invalid addr (%x)\n", (unsigned int)addr);
        return -I2C_ERR_INVALID_VALUE;
    }

    mI2cInfo[busId].state &= ~I2C_STATE_DONE;
    mI2cInfo[busId].msg.addr = addr;
    mI2cInfo[busId].msg.rxSize = rxSize;
    mI2cInfo[busId].msg.rxBuf = (uint8_t *)rxBuf;
    mI2cInfo[busId].msg.rxPtr = 0;
    mI2cInfo[busId].msg.txSize = txSize;
    mI2cInfo[busId].msg.txBuf = (uint8_t *)txBuf;
    mI2cInfo[busId].msg.txPtr = 0;
    mI2cInfo[busId].msg.cookie = cookie;
    mI2cInfo[busId].msg.callback = callback;
#if defined(SEOS)
    mI2cInfo[busId].tid = osGetCurrentTid();
#endif

    for (i = 0 ; i < 3 ; i++) {
        if (!i2cCheckIdle(busId)) {
            i = 0;
            break;
        }
    }

    if (i == 3) {
        i2cMasterReset(busId);
    }

    mI2cInfo[busId].msg.flags = 0;
    mI2cInfo[busId].stop = 1;

    if (rxSize > 0)
        mI2cInfo[busId].stop = 0;

    if (txSize > 0) {
        mI2cInfo[busId].msg.flags |= I2C_M_NOSTART;
        ret = i2cMasterTransfer(busId);

        if (ret) {
            PRINTF_ERROR("i2cMaster Transmit Fail: %d\n", ret);
            goto end;
        }

        if (mI2cInfo[busId].operationMode == I2C_MODE_POLLING) {
            if (mI2cInfo[busId].stop == 0) {
                mI2cInfo[busId].msg.flags |= I2C_M_RD;
                mI2cInfo[busId].stop = 1;
                ret = i2cMasterTransfer(busId);

                if (ret) {
                    PRINTF_ERROR("i2cMaster Receive Fail: %d\n", ret);
                    goto end;
                }
            }
        }
    }

end:
    if (ret)
        i2cMasterReset(busId);

    if (mI2cInfo[busId].operationMode == I2C_MODE_POLLING) {
      #if defined(SEOS)
        atomicBitsetClearBit(mI2cFlag, busId);
      #elif defined(EMBOS)
        OS_Unuse(&mI2cSema[busId]);
      #endif
        perfMonitorSetEvt(chub_prof_i2c_out);
        if (callback)
            callback( cookie, txSize, rxSize, ret );
    } else {
        perfMonitorSetEvt(chub_prof_i2c_out);
    }

    return ret;
}

int i2cSlaveRequest(uint32_t busId, uint32_t addr)
{
    (void)busId;
    (void)addr;
    return 0;
}

int i2cSlaveRelease(uint32_t busId)
{
    (void)busId;
    return 0;
}

void i2cSlaveEnableRx(uint32_t busId, void *rxBuf, size_t rxSize,
        I2cCallbackF callback, void *cookie)
{
    (void)busId;
    (void)rxBuf;
    (void)rxSize;
    (void)callback;
    (void)cookie;
}

int i2cSlaveTxPreamble(uint32_t busId, uint8_t byte,
        I2cCallbackF callback, void *cookie)
{
    (void)busId;
    (void)byte;
    (void)callback;
    (void)cookie;

    return 0;
}

int i2cSlaveTxPacket(uint32_t busId, const void *txBuf, size_t txSize,
        I2cCallbackF callback, void *cookie)
{
    (void)busId;
    (void)txBuf;
    (void)txSize;
    (void)callback;
    (void)cookie;

    return 0;
}

int i2cOpen(uint32_t busId, uint32_t speedInHz)
{
    return i2cMasterRequest(busId, speedInHz);
}

int i2cRead(uint32_t busId, uint32_t slaveAddr, uint32_t regAddr, uint32_t length, uint8_t *rxBuf)
{
        if( rxBuf == NULL || length == 0)
            return -I2C_ERR_INVALID_VALUE;

        rxBuf[0] = (uint8_t)regAddr;

        return i2cMasterTxRx(busId, slaveAddr, rxBuf, 1, rxBuf, length, NULL, NULL);
}


int i2cWrite(uint32_t busId, uint32_t slaveAddr, uint32_t regAddr, uint32_t length, uint8_t *txBuf)
{
        uint8_t tempBuf[128];
        int ret;

        if( txBuf == NULL || length == 0) {
            PRINTF_ERROR("i2cWrite: invalid parameter (txBuf=%x, length=%d)\n", (unsigned int)txBuf, (int)length);
            return -I2C_ERR_INVALID_VALUE;
        }

        memcpy(tempBuf+1, txBuf, length);
        tempBuf[0] = (uint8_t)regAddr;

        ret = i2cMasterTxRx(busId, slaveAddr, tempBuf, length+1, NULL, 0, NULL, NULL);

        return ret;
}

void i2cSaveState(void)
{
    int i;

    for (i  =0 ; i < I2C_CHUB_CHANNEL_MAX ; i++){
        if(mI2cInfo[i].state)
            mI2cInfo[i].state |= I2C_STATE_REOPEN;
    }
}

void i2cRestoreState(void)
{
    int i;

    for (i  =0 ; i < I2C_CHUB_CHANNEL_MAX ; i++){
        if(mI2cInfo[i].state) {
            i2cMasterRequest(i, mI2cInfo[i].speed);
            mI2cInfo[i].state &= ~I2C_STATE_REOPEN;
        }
    }
}

void i2cCmgpSaveState(void)
{
    int i;

    for (i = I2C_CHUB_CHANNEL_MAX ; i < I2C_CHANNEL_MAX ; i++){
        if(mI2cInfo[i].state)
            i2cMasterRelease(i);
    }
}

void i2cCmgpRestoreState(void)
{
    int i;

    for (i = I2C_CHUB_CHANNEL_MAX ; i < I2C_CHANNEL_MAX ; i++){
        if(mI2cInfo[i].state)
            i2cMasterRequest(i, mI2cInfo[i].speed);
    }
}
