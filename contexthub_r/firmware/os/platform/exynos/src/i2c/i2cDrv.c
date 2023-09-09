/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I2C
 *----------------------------------------------------------------------------
 *      Name:    i2cDrv.c
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

#include <csp_common.h>
#include <i2c.h>
#include <i2cDrv.h>
#include <i2cSfrBase.h>
#include <sysreg.h>
#include <cmu.h>
#include <cmuDrv.h>

#if defined(SEOS)
	#include <errno.h>
  #include <cmsis.h>
#elif defined(EMBOS)
	#include <DEVICE.h>
#endif


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
#define I2C_RXFIFO_TRIGGER_LEVEL    (0x8<<4)
#define I2C_TXFIFO_TRIGGER_LEVEL    (0x8<<16)

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
#define I2C_TRAILING_COUNT          (0xF)

// I2C_INT_EN Register bits
#define I2C_INT_TX_ALMOSTEMPTY_EN   (1u << 0)
#define I2C_INT_RX_ALMOSTFULL_EN    (1u << 1)
#define I2C_INT_TRAILING_EN         (1u << 6)
#define I2C_INT_TRANSFER_DONE       (1u << 7)
#define I2C_INT_NO_DEV_ACK_EN       (1u << 9)
#define I2C_INT_CHK_TRANS_STATE     (0xf << 8)



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

#define I2C_TIMEOUT                 100000
#define I2C_FIFO_SIZE               16

#define I2C_TIMING_COFF_HS          7
#define I2C_TIMING_COFF_FS          9


typedef struct {
    uint32_t        busFreeTimeout;
    uint32_t        txTimeout;
    uint32_t        rxTimeout;
    uint32_t        txNACK;
    uint32_t        rxNACK;
    uint32_t        stopErrInOk;
    uint32_t        stopErrInErr;
}I2cErrorType;

typedef struct {
    uint32_t        base;
    uint32_t        speed;
    uint32_t        mode;
    uint32_t        state;
    uint32_t        hwInit;
    I2cErrorType    errorCount;
}I2cInfoType;


I2cInfoType    mI2cInfo[I2C_CHANNEL_MAX];

/*
 * Static Function List
 */
static int i2cDrvSetTiming(uint32_t busId, uint32_t mode);
static int i2cDrvInitTiming(uint32_t busId);
static int i2cDrvMasterTransfer( uint32_t busId, I2cMsgType *i2cMsg, uint32_t stop);
static void i2cDrvDumpSFR(uint32_t busId);
static bool i2cDrvCheckIdle(uint32_t busId);

/*
 * Function Body
 */
void i2cDrvReset(uint32_t busId)
{
    uint32_t regValue;

    regValue = __raw_read32( mI2cInfo[busId].base + REG_I2C_CTL);
    regValue |= (uint32_t)I2C_RESET;
    __raw_write32( regValue, mI2cInfo[busId].base + REG_I2C_CTL); // Reset

    regValue = __raw_read32( mI2cInfo[busId].base + REG_I2C_CTL);
    regValue &= ~I2C_RESET;
    __raw_write32( regValue, mI2cInfo[busId].base + REG_I2C_CTL); // Reset Release
}

static int i2cDrvSetTiming(uint32_t busId, uint32_t mode)
{
    uint32_t PClk;
    uint32_t OpClk;
    uint32_t CLK_DIV, TSCL_H, TSTART_HD;
    uint32_t regValue;

    PClk = cmuDrvGetSpeed(CMU_CLK_I2C + busId);
    OpClk = mI2cInfo[busId].speed;

    if ( mode == I2C_HIGH_SPEED ) {
        CLK_DIV = PClk / (OpClk * 16);
        CLK_DIV &= 0xFF;
        regValue = __raw_readl( mI2cInfo[busId].base + REG_I2C_TIMING_HS3 ) & (uint32_t)(~0x00FF0000);
        __raw_write32( regValue | (CLK_DIV << 16), mI2cInfo[busId].base + REG_I2C_TIMING_HS3);

        TSCL_H = ((I2C_TIMING_COFF_HS * PClk) / (1000 * 1000)) / ((CLK_DIV + 1) * 100);
        TSCL_H = (0xFFFFFFFF >> TSCL_H) << TSCL_H;
        TSCL_H &= 0xFF;
        regValue = __raw_readl( mI2cInfo[busId].base + REG_I2C_TIMING_HS2 ) & (uint32_t)(~0x000000FF);
        __raw_write32( regValue | (TSCL_H << 0), mI2cInfo[busId].base + REG_I2C_TIMING_HS2);

        TSTART_HD = (I2C_TIMING_COFF_HS * PClk / (1000 * 1000)) / ((CLK_DIV + 1) * 100) - 1;
        TSTART_HD = (0xFFFFFFFF >> TSTART_HD) << TSTART_HD;
        TSTART_HD &= 0xFF;
        regValue = __raw_readl( mI2cInfo[busId].base + REG_I2C_TIMING_HS1 ) & (uint32_t)(~0x00FF0000);
        __raw_write32( regValue | (TSTART_HD << 16), mI2cInfo[busId].base + REG_I2C_TIMING_HS1);
    } else {
        CLK_DIV = PClk / (OpClk * 16);
        CLK_DIV &= 0xFF;
        regValue = __raw_readl( mI2cInfo[busId].base + REG_I2C_TIMING_FS3 ) & (uint32_t)(~0x00FF0000);
        __raw_write32( regValue | (CLK_DIV << 16), mI2cInfo[busId].base + REG_I2C_TIMING_FS3);

        TSCL_H = ((I2C_TIMING_COFF_FS * PClk) / (1000 * 1000)) / ((CLK_DIV + 1) * 10);
        TSCL_H = (0xFFFFFFFF >> TSCL_H) << TSCL_H;
        TSCL_H &= 0xFF;
        regValue = __raw_readl( mI2cInfo[busId].base + REG_I2C_TIMING_FS2 ) & (uint32_t)(~0x000000FF);
        __raw_write32( regValue | (TSCL_H << 0), mI2cInfo[busId].base + REG_I2C_TIMING_FS2);

        TSTART_HD = (I2C_TIMING_COFF_FS * PClk / (1000 * 1000)) / ((CLK_DIV + 1) * 10) - 1;
        TSTART_HD = (0xFFFFFFFF >> TSTART_HD) << TSTART_HD;
        TSTART_HD &= 0xFF;
        regValue = __raw_readl( mI2cInfo[busId].base + REG_I2C_TIMING_FS1 ) & (uint32_t)(~0x00FF0000);
        __raw_write32( regValue | (TSTART_HD << 16), mI2cInfo[busId].base + REG_I2C_TIMING_FS1);
    }

    return 0;
}

static int i2cDrvInitTiming(uint32_t busId)
{
    // First, Configure timing values for Fast Speed.
    // Even in High Speed, the initianl mode is Fast Speed.
    if( i2cDrvSetTiming(busId, I2C_FAST_SPEED) ) {
        return -I2C_ERR_CONFIG;
    }

    // Configure timing values for High Speed.
    if( mI2cInfo[busId].speed > I2C_FS_CLOCK) {
        if( i2cDrvSetTiming(busId, I2C_HIGH_SPEED) )
            return -I2C_ERR_CONFIG;
    }

    return 0;
}

static void i2cDrvDumpSFR(uint32_t busId)
{
#if 0
    CSP_PRINTF_INFO("I2C CTL : %x\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_CTL));
    CSP_PRINTF_INFO("I2C FIFO_CTL : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_FIFO_CTL));
    CSP_PRINTF_INFO("I2C TRAILING_CTL : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_TRAILING_CTL));
    CSP_PRINTF_INFO("I2C INT_EN : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_INT_EN));
    CSP_PRINTF_INFO("I2C INT_STAT : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_INT_STAT));
    CSP_PRINTF_INFO("I2C FIFO_STAT : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_FIFO_STAT));
    CSP_PRINTF_INFO("I2C CONF : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_CONF));
    CSP_PRINTF_INFO("I2C CONF2 : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_CONF2));
    CSP_PRINTF_INFO("I2C TRANS STATUS : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_TRANS_STATUS));
    CSP_PRINTF_INFO("I2C TIMING HS1 : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_TIMING_HS1));
    CSP_PRINTF_INFO("I2C TIMING HS2 : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_TIMING_HS2));
    CSP_PRINTF_INFO("I2C TIMING HS3 : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_TIMING_HS3));
#endif
    CSP_PRINTF_INFO("I2C ADDR : %X\n", __raw_read32(mI2cInfo[busId].base + REG_I2C_ADDR));
}

int i2cDrvMasterOpen(uint32_t busId, uint32_t speedInHz)
{
    uint32_t    regValue;

    if ( mI2cInfo[busId].state & I2C_STATE_OPEN ) // Already Opened
        return -1;

    mI2cInfo[busId].base = mI2cSfrBase[busId];
    mI2cInfo[busId].speed = speedInHz;
    mI2cInfo[busId].mode = I2C_MODE_MASTER;
    mI2cInfo[busId].state = 0;
    mI2cInfo[busId].hwInit = 0;

    i2cDrvReset(busId);

    regValue = __raw_read32( mI2cInfo[busId].base + REG_I2C_CTL);
    regValue |= (0x1<<3);
    __raw_write32( regValue, mI2cInfo[busId].base + REG_I2C_CTL);

    i2cDrvInitTiming(busId);

    __raw_write32( I2C_TRAILING_COUNT, mI2cInfo[busId].base + REG_I2C_TRAILING_CTL);

    if ( speedInHz > I2C_FS_CLOCK )
    {
        regValue = __raw_read32( mI2cInfo[busId].base + REG_I2C_CONF);
        regValue |= I2C_HS_MODE;
        __raw_write32( regValue, mI2cInfo[busId].base + REG_I2C_CONF);
    }

    regValue |= (uint32_t)I2C_AUTO_MODE;
    __raw_write32( regValue, mI2cInfo[busId].base + REG_I2C_CONF);

    mI2cInfo[busId].hwInit = 1;
    mI2cInfo[busId].state = I2C_STATE_OPEN;

    return 0;
}

int i2cDrvClose(uint32_t busId)
{
    mI2cInfo[busId].state = 0;
    mI2cInfo[busId].hwInit = 0;
    return 0;
}

bool i2cDrvCheckIdle(uint32_t busId)
{
    uint32_t regValue;

    regValue = __raw_read32( mI2cInfo[busId].base + REG_I2C_TRANS_STATUS);

    if( (regValue & I2C_MASTER_STATUS_MASK) != I2C_MASTER_STATUS_INIT)
    {
        CSP_PRINTF_ERROR("I2C Bus is not IDLE : TRANS_STATUS = %x\n", (unsigned int)(regValue & I2C_MASTER_STATUS_MASK));
        return false;
    }
    else
        return true;
}

static int i2cDrvMasterTransfer( uint32_t busId, I2cMsgType *i2cMsg, uint32_t stop)
{
    uint32_t timeout;
    uint32_t transStatus;
    uint32_t regCtl;
    uint32_t regConf2;
    uint32_t regTimeout;
    uint32_t regAddr;
    uint32_t regIntEn;
    uint32_t regFifoCtl;
    int ret = 0;
    uint32_t msgPtr = 0;
    uint8_t byte;


    regCtl     = __raw_read32( mI2cInfo[busId].base + REG_I2C_CTL);
    regConf2   = __raw_read32( mI2cInfo[busId].base + REG_I2C_CONF2);
    regTimeout = __raw_read32( mI2cInfo[busId].base + REG_I2C_TIMEOUT);

    regTimeout &= ~I2C_TIMEOUT_EN;
    __raw_write32( regTimeout, mI2cInfo[busId].base + REG_I2C_TIMEOUT);

    regFifoCtl = I2C_RXFIFO_EN | I2C_TXFIFO_EN |
                 I2C_TXFIFO_TRIGGER_LEVEL | I2C_RXFIFO_TRIGGER_LEVEL;
    __raw_write32( regFifoCtl, mI2cInfo[busId].base + REG_I2C_FIFO_CTL);

    regIntEn = 0;

    if ( i2cMsg->flags & I2C_M_RD) {
        regCtl |= I2C_RXCH_ON;
        regConf2 |= I2C_READ_WRITE;
        regIntEn |= ( I2C_INT_RX_ALMOSTFULL_EN | I2C_INT_TRAILING_EN );
    } else {
        regCtl &= (uint32_t)~I2C_RXCH_ON;
        regConf2 &= (uint32_t)~I2C_READ_WRITE;
        regIntEn |= I2C_INT_TX_ALMOSTEMPTY_EN;
    }

    if ( mI2cInfo[busId].mode == I2C_MODE_INTERRUPT ) {
        // Clear Pend
    }

    if ( stop == 1 )
        regConf2 |= I2C_STOP_AFTER_TRANS;
    else
        regConf2 &= (uint32_t)~I2C_STOP_AFTER_TRANS;

    regAddr = __raw_read32( mI2cInfo[busId].base + REG_I2C_ADDR );
    regAddr &= (uint32_t)(~(0x3FF << 10));
    regAddr &= (uint32_t)(~(0x3FF << 0));
    regAddr &= ~(0xFF  << 24);
    regAddr |= ((i2cMsg->addr & 0x7F) << 10);
    __raw_write32( regAddr, mI2cInfo[busId].base + REG_I2C_ADDR);

    __raw_write32( regCtl, mI2cInfo[busId].base + REG_I2C_CTL);

    regConf2 &= (uint32_t)(~(0xFFFF));

    if( i2cMsg->flags & I2C_M_RD)
        regConf2 |= i2cMsg->rxSize;
    else
        regConf2 |= i2cMsg->txSize;
    __raw_write32( regConf2, mI2cInfo[busId].base + REG_I2C_CONF2);

    regConf2  = __raw_read32( mI2cInfo[busId].base + REG_I2C_CONF2 );
    regConf2 |= (uint32_t)I2C_MASTER_RUN;
    __raw_write32( regConf2, mI2cInfo[busId].base + REG_I2C_CONF2);

    if ( mI2cInfo[busId].mode == I2C_MODE_INTERRUPT ) {
        // Enable IRQ
    }

    ret = -I2C_ERR_TIMEOUT;


    msgPtr = 0;
    if ( i2cMsg->flags & I2C_M_RD)
    {
        if ( mI2cInfo[busId].mode == I2C_MODE_POLLING ) {
	    uint32_t mask = sysregGetMaskIrq();
	    mask = mask | I2C_INTERRUPT_MASK;
	    sysregSetMaskIrq(mask);

            __raw_write32( I2C_INT_TRANSFER_DONE, mI2cInfo[busId].base + REG_I2C_INT_EN);
            timeout = I2C_TIMEOUT;

            while( timeout-- > 0 ) {
                if( (__raw_read32( mI2cInfo[busId].base + REG_I2C_FIFO_STAT ) & (0x1<<24)) == 0) {
                    byte =  (uint8_t)__raw_read32( mI2cInfo[busId].base + REG_I2C_RXDATA);
                    i2cMsg->rxBuf[msgPtr++] =  byte;
                    timeout = I2C_TIMEOUT;
                }

                if( msgPtr >= i2cMsg->rxSize ) {
                    timeout = I2C_TIMEOUT;
                    while (timeout-- > 0) {
                        transStatus = __raw_read32( mI2cInfo[busId].base + REG_I2C_INT_STAT );
                        __raw_write32( transStatus, mI2cInfo[busId].base + REG_I2C_INT_STAT);
                        if( transStatus & I2C_INT_TRANSFER_DONE ){
                            ret = 0;
                            break;
                        }
                    }
                    break;
                }
            }

            if ( ret == -I2C_ERR_TIMEOUT ) {
                I2C_DEBUG_PRINT("ERROR : RX Timeout\n");
                I2C_DEBUG_PRINT("Rx count : %d\n", (int)msgPtr);
                i2cDrvDumpSFR(busId);
                return ret;
            }
        }
    } else {
        if ( mI2cInfo[busId].mode == I2C_MODE_POLLING ) {
	    uint32_t mask = sysregGetMaskIrq();
	    mask = mask | I2C_INTERRUPT_MASK;
	    sysregSetMaskIrq(mask);

            __raw_write32( I2C_INT_TRANSFER_DONE, mI2cInfo[busId].base + REG_I2C_INT_EN);
            timeout = I2C_TIMEOUT;
            while( (timeout-- > 0) && (msgPtr < i2cMsg->txSize)) {
                if( (__raw_read32( mI2cInfo[busId].base + REG_I2C_FIFO_STAT ) & 0x7F) < I2C_FIFO_SIZE) {
                    byte = i2cMsg->txBuf[msgPtr++];
                    __raw_write32( byte, mI2cInfo[busId].base + REG_I2C_TXDATA);
                }
            }
        }

        if ( mI2cInfo[busId].mode == I2C_MODE_POLLING ) {
            timeout = I2C_TIMEOUT;
            while (timeout-- > 0) {
                transStatus = __raw_read32( mI2cInfo[busId].base + REG_I2C_INT_STAT );
                __raw_write32( transStatus, mI2cInfo[busId].base + REG_I2C_INT_STAT);
                if( transStatus & I2C_INT_TRANSFER_DONE ){
                    ret = 0;
                    break;
                }
            }
            if ( ret == -I2C_ERR_TIMEOUT){
                I2C_DEBUG_PRINT("ERROR : Tx Timeout !!!\n");
                i2cDrvDumpSFR(busId);

                return ret;
            }
        }
    }

    return ret;
}

int i2cDrvMasterTxRx(uint32_t busId, I2cMsgType *i2cMsg)
{
    uint32_t stop = 1;
    int ret = 0;

    if (i2cMsg == NULL)
        return -I2C_ERR_INVALID_VALUE;

    if ( !(mI2cInfo[busId].state & I2C_STATE_OPEN ) ) // Check if opened
        return -I2C_ERR_IO;

    if( !i2cDrvCheckIdle(busId))
    {
        i2cDrvClose(busId);
        i2cDrvMasterOpen(busId, mI2cInfo[busId].speed);
    }

    i2cMsg->flags = 0;

    if ( i2cMsg->rxSize > 0 ) {
        stop = 0;
    }

    if (i2cMsg->txSize > 0 ) {
        i2cMsg->flags |= I2C_M_NOSTART;

        __disable_irq();
        ret = i2cDrvMasterTransfer( busId, i2cMsg, stop );
        __enable_irq();

        if ( ret) {
            CSP_PRINTF_ERROR("i2cDrvMaster Transmit Fail : %d\n", ret);
            return ret;
        }
        if ( stop == 0 ) {
            i2cMsg->flags |= I2C_M_RD;

            __disable_irq();
            ret = i2cDrvMasterTransfer( busId, i2cMsg, 1 );
            __enable_irq();

            if ( ret) {
                CSP_PRINTF_ERROR("i2cDrvMaster Receive Fail : %d\n", ret);
                return ret;
            }
        }
    }

    return ret;
}

uint32_t i2cDrvGetSpeed(uint32_t busId)
{
    if(busId >= I2C_CHANNEL_MAX)
        return 0;

    return mI2cInfo[busId].speed;
}
