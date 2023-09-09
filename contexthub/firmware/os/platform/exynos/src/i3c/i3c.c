/*----------------------------------------------------------------------------
 *      Exynos SoC  -  I3C
 *----------------------------------------------------------------------------
 *      Name:    i3c.c
 *      Purpose: To implement I3C APIs
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

#include <csp_common.h>
#include <csp_printf.h>
#if defined(SEOS)
    #include <errno.h>
    #include <seos.h>
    #include <atomic.h>
#elif defined(EMBOS)
    #include <RTOS.h>
    #include <Device.h>
#endif
#include <cmsis.h>
#include <plat/i3c/i3c.h>
#include <plat/i3c/ccc.h>
#include <rtc.h>
#include <platform.h>
#include <cmu.h>
#include <util.h>

#define PRINTF_DEBUG CSP_PRINTF_DEBUG
#define PRINTF_INFO  CSP_PRINTF_INFO
#define PRINTF_ERROR CSP_PRINTF_ERROR

/******************************************************************************
 * I3C SFR
 ******************************************************************************/
/* Capabilities and Operation Registers */
#define REG_I3C_HCI_VERSION                 0x0
#define REG_I3C_HC_CONTROL                  0x4
    #define BUS_ENABLE                      (1u << 31)
#define REG_I3C_MASTER_DEVICE_ADDR          0x8
#define REG_I3C_HC_CAPABILITIES             0xc
#define REG_I3C_RESET_CONTROL               0x10
#define REG_I3C_PRESENT_STATE               0x14
#define REG_I3C_DAT_SECTION_OFFSET          0x30
#define REG_I3C_DCT_SECTION_OFFSET          0x34
#define REG_I3C_RING_HEADERS_SECTION_OFFSET 0x38
#define REG_I3C_PIO_SECTION_OFFSET          0x3c
#define REG_I3C_EXT_CAPS_SECTION_OFFSET     0x40
#define REG_I3C_IBI_NOTIFY_CTRL             0x58
#define REG_I3C_DEV_CTX_BASE_LO             0x60
#define REG_I3C_DEV_CTX_BASE_HI             0x64

/* Device Address Table */
#define REG_I3C_DEVICE_ADDRESS_TABLE(n)     (0x100 + (n * 4))

/* Device Characteristic Table */
#define REG_I3C_DEVICE_ADDRESS_TABLE_0(n)   (0x200 + (n * 16))
#define REG_I3C_DEVICE_ADDRESS_TABLE_1(n)   (0x204 + (n * 16))
#define REG_I3C_DEVICE_ADDRESS_TABLE_2(n)   (0x208 + (n * 16))
#define REG_I3C_DEVICE_ADDRESS_TABLE_3(n)   (0x20C + (n * 16))

/* PIO Access Area */
#define REG_I3C_CMD_QUEUE                   0x300
    #define CMD_QUEUE_TOC                   (1u << 31)
    #define CMD_QUEUE_ROC                   (1u << 30)
    #define CMD_QUEUE_RNW(n)                ((n & MASK_1BITS) << 29)
    #define CMD_QUEUE_DEV_COUNT(n)          ((n & MASK_4BITS) << 26)
    #define CMD_QUEUE_MODE(n)               ((n & MASK_3BITS) << 26)
    #define CMD_QUEUE_BC(n)                 ((n & MASK_3BITS) << 23)
    #define CMD_QUEUE_DEV_INDEX(n)          ((n & MASK_5BITS) << 16)
    #define CMD_QUEUE_CP(n)                 ((n & MASK_1BITS) << 15)
    #define CMD_QUEUE_CMD(n)                ((n & MASK_8BITS) << 7)
    #define CMD_QUEUE_TID(n)                ((n & MASK_4BITS) << 3)
    #define CMD_QUEUE_CMD_ATTR(n)           ((n & MASK_3BITS) << 0)
#define REG_I3C_RESP_QUEUE                  0x304
#define REG_I3C_XFER_DATA_PORT              0x308
#define REG_I3C_IBI_PORT                    0x30c
#define REG_I3C_QUEUE_THLD_CTRL             0x310
#define REG_I3C_DATA_BUFFER_THLD_CTRL       0x314
#define REG_I3C_QUEUE_SIZE                  0x318
#define REG_I3C_PIO_INTR_STATUS             0x320
#define REG_I3C_PIO_INTR_STATUS_ENABLE      0x324
#define REG_I3C_PIO_INTR_SIG_ENABLE         0x328
#define REG_I3C_PIO_INTR_FORCE              0x32c

/* Extended Capability Mandatory Registers */
#define REG_I3C_EXTCAP_HEADER1              0x2000
#define REG_I3C_COMP_MANUFACTURER           0x2004
#define REG_I3C_COMP_VERSION                0x2008
#define REG_I3C_COMP_TYPE                   0x200c
#define REG_I3C_EXTCAP_HEADER2              0x2010
#define REG_I3C_MASTER_CONFIG               0x2014
#define REG_I3C_EXTCAP_HEADER3              0x2018
#define REG_I3C_BUS_INSTANCE_COUNT          0x201c
#define REG_I3C_BUS_INSTANCE_OFFSET         0x2020
#define REG_I3C_EXTCAP_HEADER4              0x2024
#define REG_I3C_QUEUE_STATUS_LEVEL          0x2028
#define REG_I3C_DATA_BUFFER_STATUS_LEVEL    0x202c
#define REG_I3C_PRESENT_STATE_DEBUG         0x2030
#define REG_I3C_MX_ERROR_COUNTERS           0x2034
#define REG_I3C_EXTCAP_HEADER5              0x20fc
#define REG_I3C_EXT_DEVICE_ADDRESS_TABLE(n) (0x2100 + (n * 4))
#define REG_I3C_EXTCAP_HEADER6              0x2180
#define REG_I3C_EXT_PIO_INTR_STATUS         0x2184
#define REG_I3C_EXT_PIO_INTR_STATUS_ENABLE  0x2188
#define REG_I3C_EXT_PIO_INTR_SIG_ENABLE     0x218c
#define REG_I3C_EXT_PIO_INTR_FORCE          0x2190
#define REG_I3C_EXTCAP_HEADER7              0x2194
#define REG_I3C_EXT_DEVICE_CTRL             0x2198
#define REG_I3C_EXTCAP_HEADER8              0x219c
#define REG_I3C_EXT_RESET_CTRL              0x21a0
#define REG_I3C_EXTCAP_HEADER9              0x21a4
#define REG_I3C_TRAILING_CYCLE              0x21a8
#define REG_I3C_SCL_QUARTER_PERIOD          0x21ac
#define REG_I3C_SCL_CONTROL                 0x21b0
#define REG_I3C_TSU_STA                     0x21b4
#define REG_I3C_TCBP                        0x21b8
#define REG_I3C_TLOW                        0x21bc
#define REG_I3C_THD_STA                     0x21c0
#define REG_I3C_TBUF                        0x21c4
#define REG_I3C_BUS_CONDITION_CYCLE         0x21c8
#define REG_I3C_SAMPLE_CYCLE                0x21cc
#define REG_I3C_EXTCAP_HEADER10             0x21d0
#define REG_I3C_SCL_LOW_FORCE_STEP          0x21d4
#define REG_I3C_EXTCAP_HEADER11             0x21d8
#define REG_I3C_FILTER_CTRL                 0x21dc
#define REG_I3C_EXTCAP_HEADER12             0x21e0
#define REG_I3C_VGPIO_TX_CONFIG             0x21e4
#define REG_I3C_VGPIO_TX_RESP               0x21e8
#define REG_I3C_EXTCAP_HEADER13             0x21ec
#define REG_I3C_VGPIO_RX_MASK               0x21f0
#define REG_I3C_EXTCAP_HEADER14             0x2210
#define REG_I3C_HDR_DDR_RD_ABORT            0x2214
#define REG_I3C_EXTCAP_HEADER15             0x2218
#define REG_I3C_EVENT_COMMAND               0x221c
#define REG_I3C_ENTER_ACTIVITY_STATE        0x2220
#define REG_I3C_MAX_WRITE_LENGTH            0x2224
#define REG_I3C_MAX_READ_LENGTH             0x2228
#define REG_I3C_DEVICE_COUNT                0x222c
#define REG_I3C_CURRENT_MASTER              0x2230
#define REG_I3C_TEST_MODE_BYTE              0x2234
#define REG_I3C_HDR_MODE                    0x2238
#define REG_I3C_CHARACTERISTIC_REGISTER_0   0x223c
#define REG_I3C_CHARACTERISTIC_REGISTER_1   0x2240
#define REG_I3C_MAX_SPEED                   0x2244
#define REG_I3C_MAX_RD_TURN                 0x2248
#define REG_I3C_HDRCAP                      0x224c
#define REG_I3C_DATA_LENGTH_SLAVE_HDR       0x2250
#define REG_I3C_SLAVE_HDR_TS_TX_PERIOD      0x2254
#define REG_I3C_EXTCAP_HEADER16             0x22fc
#define REG_I3C_IBI_TX_DATA_PORT            0x2300
#define REG_I3C_IBI_TX_BUF_LEVEL            0x2304
#define REG_I3C_EXTCAP_HEADER17             0x230c
#define REG_I3C_DEBUG_CMD_QUEUE_0           0x2310
#define REG_I3C_DEBUG_CMD_QUEUE_1           0x2314
#define REG_I3C_DEBUG_RESP_STAT_QUEUE       0x2330
#define REG_I3C_DEBUG_TX_DATA_PORT          0x2340
#define REG_I3C_DEBUG_RX_DATA_PORT          0x2380
#define REG_I3C_DEBUG_IBI_STAT_QUEUE        0x23c0
#define REG_I3C_DEBUG_IBI_TX_DATA_PORT      0x2400
#define REG_I3C_VGPIO_TX_MONITOR            0x2410
#define REG_I3C_VGPIO_RX_MONITOR            0x2414
#define REG_I3C_MASTER_STAT_0               0x2418
#define REG_I3C_MASTER_STAT_1               0x241c
#define REG_I3C_SLAVE_STAT                  0x2420
#define REG_I3C_EXTCAP_HEADER18             0x2424
#define REG_I3C_OPTIONS                     0x2428
#define REG_I3C_CHICKEN                     0x242c

/* DEVICE_ADDRESS_TABLE */
#define DEVICE_ADDRESS_TABLE_SIR_REJECT     (1 << 13)

/* RESET_CONTROL */
#define RESET_CONTROL_CMD_QUEUE_RST         (1 << 1)
#define RESET_CONTROL_SOFT_RST              (1 << 0)

/* EXT DEVICE CTRL */
#define EXT_DEVICE_CTRL_HWACG_DISABLE_S     (1 << 31)
#define EXT_DEVICE_CTRL_RX_STALL_EN         (1 << 7)
#define EXT_DEVICE_CTRL_TX_STALL_EN         (1 << 6)
   
/* PIO INTERRUPT */
#define PIO_INTR_TRANSFER_ERR               (1 << 9)
#define PIO_INTR_TRANSFER_ABORT             (1 << 5)
#define PIO_INTR_RESP_READY                 (1 << 4)
#define PIO_INTR_CMD_QUEUE_READY            (1 << 3)
#define PIO_INTR_IBI_STATUS_THLD            (1 << 2)
#define PIO_INTR_RX_THLD                    (1 << 1)
#define PIO_INTR_TX_THLD                    (1 << 0)

/* EXT PIO INTERRUPT */
#define EXT_PIO_INTR_SCL_LOW_DONE           (1 << 31)
#define EXT_PIO_INTR_VGPIO_TX_PARITY_ERR    (1 << 22)
#define EXT_PIO_INTR_VGPIO_RX               (1 << 21)
#define EXT_PIO_INTR_VGPIO_TX               (1 << 20)
#define EXT_PIO_INTR_SLAVE_INT_ACKED        (1 << 18)
#define EXT_PIO_INTR_GETACCMST              (1 << 17)
#define EXT_PIO_INTR_BUS_AVAIL              (1 << 16)
#define EXT_PIO_INTR_BUS_IDLE               (1 << 15)
#define EXT_PIO_INTR_TRAILING               (1 << 12)
#define EXT_PIO_INTR_BUS_AVAIL_VIOLATION    (1 << 11)
#define EXT_PIO_INTR_BUS_IDLE_VIOLATION     (1 << 10)


//#define I3C_BROADCAST_ADDR                  0x7E
#define I3C_MAX_QUEUE_DEPTH                 5
#define I3C_TIMEOUT                         1000
#define I3C_MAX_READ_LEN                    127

#define INFO_PRINT(fmt, ...) do { \
        osLog(LOG_INFO, "[I3C] " fmt, ##__VA_ARGS__); \
    } while (0);

#define ERROR_PRINT(fmt, ...) do { \
        osLog(LOG_ERROR, "[I3C] ERROR:" fmt, ##__VA_ARGS__); \
    } while (0);

#define I3C_DEBUG                           0
#define DEBUG_PRINT(fmt, ...) do { \
        if (I3C_DEBUG) {  \
            osLog(LOG_DEBUG, "[I3C] " fmt, ##__VA_ARGS__); \
        } \
    } while (0);

enum I3cMasterState {
    I3C_MASTER_IDLE,
    I3C_MASTER_ERROR,
    I3C_MASTER_START,
    I3C_MASTER_TX,
    I3C_MASTER_RX,
};

enum I3cRnW {
    I3C_WRITE = 0,
    I3C_READ = 1,
};

enum I3cAttr {
    I3C_MODE_R = 0x0,
    I3C_MODE_I = 0x1,
    I3C_MODE_A = 0x2,
};

struct I3cState {
    struct {
        union {
            uint8_t *buf;
            const uint8_t *cbuf;
            uint8_t byte;
        };
        size_t size;
        size_t offset;                  // offset for data bigger than 64bytes
    } rx, tx;

    I3cCallbackF callback;
    void *cookie;

    struct {
        uint8_t cmd;
        uint8_t data;
        uint8_t num;
        uint8_t err;
    } ccc;

    uint8_t masterState;
    uint8_t index;
    uint16_t tid;
};

struct I3cDev {
    const struct I3cCfg *cfg;
    struct I3cState state;

    uint32_t next;
    uint32_t last;

    struct I3cPid pid[I3C_BUS_MAX_DEVS];
};

static struct I3cDev mI3cDev[I3C_CHANNEL_MAX];

static uint8_t mI3cAddrPool[I3C_BUS_MAX_DEVS] = {
    0x8,
    //0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12,
};

struct I3cXfer {
    uint32_t        id;
    const void      *txBuf;
    size_t          txSize;
    void            *rxBuf;
    size_t          rxSize;
    I3cCallbackF    callback;
    void            *cookie;
    uint8_t         busId;
    uint8_t         index;      // max 11 slaves
    uint16_t        tid;
};

#if defined(SEOS)
    ATOMIC_BITSET_DECL(mXfersValid, I3C_MAX_QUEUE_DEPTH, static);
#elif defined(EMBOS)
    bool mXfersValid[I3C_MAX_QUEUE_DEPTH];
#endif

static struct I3cXfer mXfers[I3C_MAX_QUEUE_DEPTH];

static inline void i3cReset(struct I3cDev *pdev) {
    __raw_writel(RESET_CONTROL_SOFT_RST, pdev->cfg->sfrBase + REG_I3C_RESET_CONTROL);
    while(__raw_readl(pdev->cfg->sfrBase + REG_I3C_RESET_CONTROL) & RESET_CONTROL_SOFT_RST);
}

static inline void i3cCmdQueueReset(struct I3cDev *pdev) {
    __raw_writel(RESET_CONTROL_CMD_QUEUE_RST, pdev->cfg->sfrBase + REG_I3C_RESET_CONTROL);
    while(__raw_readl(pdev->cfg->sfrBase + REG_I3C_RESET_CONTROL) & RESET_CONTROL_CMD_QUEUE_RST);
}

static inline void i3cInvokeCallback(struct I3cState *state, size_t tx, size_t rx, int err) {
    if (!state->callback)
        return;
#if defined(SEOS)
    uint16_t oldTid = osSetCurrentTid(state->tid);
    state->callback(state->cookie, tx, rx, err);
    osSetCurrentTid(oldTid);
#else
    state->callback(state->cookie, tx, rx, err);
#endif
}

static inline void i3cSetDynamicAddr(struct I3cDev *pdev, uint32_t index, uint32_t addr) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_DEVICE_ADDRESS_TABLE(index));
    regValue &= ~(MASK_8BITS << 16);
    regValue |= (addr & MASK_8BITS) << 16;
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_DEVICE_ADDRESS_TABLE(index));
}

static inline void i3cSetSirReject(struct I3cDev *pdev, uint32_t index) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_DEVICE_ADDRESS_TABLE(index));
    regValue |= DEVICE_ADDRESS_TABLE_SIR_REJECT;
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_DEVICE_ADDRESS_TABLE(index));
}

static inline void i3cHcCtrlBusEnable(struct I3cDev *pdev) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_HC_CONTROL);
    regValue |= BUS_ENABLE;
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_HC_CONTROL);
}

static inline void i3cHcCtrlBusDisable(struct I3cDev *pdev) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_HC_CONTROL);
    regValue &= ~BUS_ENABLE;
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_HC_CONTROL);
}

static inline void i3cPioIntrEnable(struct I3cDev *pdev, uint32_t interrupt) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_PIO_INTR_STATUS_ENABLE);
    regValue |= interrupt;
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_PIO_INTR_STATUS_ENABLE);
}

static inline void i3cPioIntrEnableAll(struct I3cDev *pdev) {
    i3cPioIntrEnable(pdev,  PIO_INTR_TRANSFER_ERR   |
                            PIO_INTR_TRANSFER_ABORT |
                            PIO_INTR_RESP_READY     |
                            PIO_INTR_CMD_QUEUE_READY|
                            PIO_INTR_IBI_STATUS_THLD|
                            PIO_INTR_RX_THLD        |
                            PIO_INTR_TX_THLD        );
}

static inline void i3cExtPioIntrEnable(struct I3cDev *pdev, uint32_t interrupt) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_EXT_PIO_INTR_STATUS_ENABLE);
    regValue |= interrupt;
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_EXT_PIO_INTR_STATUS_ENABLE);
}

static inline void i3cExtPioIntrEnableAll(struct I3cDev *pdev) {
    i3cExtPioIntrEnable(pdev,   EXT_PIO_INTR_SCL_LOW_DONE       |
                                //EXT_PIO_INTR_VGPIO_TX_PARITY_ERR|
                                //EXT_PIO_INTR_VGPIO_RX           |
                                //EXT_PIO_INTR_VGPIO_TX           |
                                EXT_PIO_INTR_SLAVE_INT_ACKED    |
                                EXT_PIO_INTR_GETACCMST          |
                                EXT_PIO_INTR_BUS_AVAIL          |
                                EXT_PIO_INTR_BUS_IDLE           |
                                EXT_PIO_INTR_TRAILING           |
                                EXT_PIO_INTR_BUS_AVAIL_VIOLATION|
                                EXT_PIO_INTR_BUS_IDLE_VIOLATION );
}

static inline void i3cExtDeviceCtrl(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_EXT_DEVICE_CTRL);
}

static inline uint32_t i3cGetPioIntrStatus(struct I3cDev *pdev) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_PIO_INTR_STATUS);
    return regValue;
}

static inline void i3cSetPioIntrStatus(struct I3cDev *pdev, uint32_t interrupt) {
    __raw_writel(interrupt, pdev->cfg->sfrBase + REG_I3C_PIO_INTR_STATUS);
}

static inline uint32_t i3cGetExtPioIntrStatus(struct I3cDev *pdev) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_EXT_PIO_INTR_STATUS);
    return regValue;
}

static inline void i3cExtSetPioIntrStatus(struct I3cDev *pdev, uint32_t interrupt) {
    __raw_writel(interrupt, pdev->cfg->sfrBase + REG_I3C_EXT_PIO_INTR_STATUS);
}

static inline void i3cSetSclQuarterPeriod(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_SCL_QUARTER_PERIOD);
}

static inline void i3cSetSclControl(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_SCL_CONTROL);
}

static inline void i3cSetTsuSta(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_TSU_STA);
}

static inline void i3cSetTCBP(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_TCBP);
}

static inline void i3cSetTlow(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_TLOW);
}

static inline void i3cSetThdSta(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_THD_STA);
}

static inline void i3cSetTbuf(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_TBUF);
}

static inline void i3cSetCmdQueue(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_CMD_QUEUE);
}

static inline void i3cSetDataBuffer(struct I3cDev *pdev, uint32_t regValue) {
    __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_XFER_DATA_PORT);
}

static inline uint32_t i3cGetDataBuffer(struct I3cDev *pdev) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_XFER_DATA_PORT);
    return regValue;
}

static inline uint32_t i3cGetRespQueue(struct I3cDev *pdev) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_RESP_QUEUE);
    return regValue;
}

static inline struct I3cXfer *i3cGetXfer(void) {
#if defined(SEOS)
    int32_t idx = atomicBitsetFindClearAndSet(mXfersValid);
#elif defined(EMBOS)
    int32_t idx  = -1;
    int i;
    
    for (i = 0; i < I3C_MAX_QUEUE_DEPTH; i++) {
        if (!mXfersValid[i]) {
            idx = i;
            mXfersValid[i] = true;
            break;
      }
    }
#endif
    
    if (idx < 0)
        return NULL;
    else
        return mXfers + idx;
}

static inline void i3cPutXfer(struct I3cXfer *xfer) {

    if (xfer)
#if defined(SEOS)
        atomicBitsetClearBit(mXfersValid, xfer - mXfers);
#elif defined(EMBOS)
        mXfersValid[xfer - mXfers] = false;
#endif
}

static inline uint32_t i3cGetDataLevel(struct I3cDev *pdev, enum I3cRnW rw) {
    uint32_t regValue;

    regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_DATA_BUFFER_STATUS_LEVEL);
    if (rw == I3C_WRITE)
        regValue = regValue & MASK_8BITS;
    else if (rw == I3C_READ)
        regValue = (regValue >> 8) & MASK_8BITS;

    return regValue;
}

static int i3cClockConfig(struct I3cDev *pdev) {
    uint32_t coreRate, corePeriod, regValue;
    uint32_t cnt;
    
    return 0;

    coreRate = cmuGetSpeed(CMU_CLK_I3C);
    if (!coreRate)
        return -1;

    corePeriod = DIV_ROUND_UP(1000000000, coreRate);

    // REG_I3C_TRAILING_CYCLE
    __raw_writel(0xF, pdev->cfg->sfrBase + REG_I3C_TRAILING_CYCLE);

    // REG_I3C_SCL_QUARTER_PERIOD
    regValue = 0;
    cnt = DIV_ROUND_UP(coreRate, I3C_BUS_I2C_FM_SCL_RATE) / 4;
    regValue |= ((cnt & MASK_8BITS) << 24);
    cnt = DIV_ROUND_UP(coreRate, I3C_BUS_I2C_FM_PLUS_SCL_RATE) / 4;
    regValue |= ((cnt & MASK_8BITS) << 16);
    //cnt = DIV_ROUND_UP(coreRate, I3C_BUS_I3C_OPEN_DRAIN_RATE) / 4;
    cnt = DIV_ROUND_UP(coreRate, pdev->cfg->clkOpenDrain) / 4;
    regValue |= ((cnt & MASK_8BITS) << 8);
    //cnt = DIV_ROUND_UP(coreRate, I3C_BUS_TYP_I3C_SCL_RATE) / 4;
    cnt = DIV_ROUND_UP(coreRate, pdev->cfg->clkPushPull) / 4;
    if (cnt < 1) cnt = 1;
    regValue |= (cnt & MASK_8BITS);
    i3cSetSclQuarterPeriod(pdev, regValue);

    // REG_I3C_SCL_CONTROL
    cnt = I3C_BUS_THIGH_MAX_NS / corePeriod;
    if (cnt * corePeriod < I3C_BUS_THIGH_MIN_NS)
        cnt = DIV_ROUND_UP(I3C_BUS_THIGH_MIN_NS, corePeriod);
    regValue = (cnt & MASK_8BITS);
    i3cSetSclControl(pdev, regValue);

    // REG_I3C_TSU_STA
    regValue = 0;
    cnt = DIV_ROUND_UP(I3C_BUS_TSU_STA_FM, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 16);
    cnt = DIV_ROUND_UP(I3C_BUS_TSU_STA_FM_PLUS, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 8);
    cnt = DIV_ROUND_UP(I3C_BUS_TCBS_MIN, corePeriod);
    regValue |= (cnt & MASK_8BITS);
    i3cSetTsuSta(pdev, regValue);

    // REG_I3C_TCBP
    regValue = 0;
    cnt = DIV_ROUND_UP(I3C_BUS_TSU_STO_FM, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 16);
    cnt = DIV_ROUND_UP(I3C_BUS_TCBP_MIN, corePeriod);
    regValue |= (cnt & MASK_8BITS);
    i3cSetTCBP(pdev, regValue);

    // REG_I3C_TLOW
    regValue = 0;
    cnt = DIV_ROUND_UP(I3C_BUS_TLOW_FM, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 16);
    cnt = DIV_ROUND_UP(I3C_BUS_TLOW_FM_PLUS, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 8);
    cnt = DIV_ROUND_UP(I3C_BUS_TLOW_OD, corePeriod);
    regValue |= (cnt & MASK_8BITS);
    i3cSetTlow(pdev, regValue);

    // REG_I3C_THD_STA
    regValue = 0;
    cnt = DIV_ROUND_UP(I3C_BUS_TSU_STA_FM, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 16);
    cnt = DIV_ROUND_UP(I3C_BUS_TSU_STA_FM_PLUS, corePeriod);
    regValue |= ((cnt & MASK_8BITS) << 8);
    cnt = DIV_ROUND_UP(I3C_BUS_TCAS_MIN, corePeriod);
    regValue |= (cnt & MASK_8BITS);
    i3cSetThdSta(pdev, regValue);

    // REG_I3C_TBUF
    regValue = 0;
    cnt = DIV_ROUND_UP(I3C_BUS_TBUF_FM, corePeriod);
    regValue |= ((cnt & MASK_9BITS) << 16);;
    cnt = DIV_ROUND_UP(I3C_BUS_TCAS_MIN, corePeriod);
    regValue |= (cnt & MASK_8BITS);
    i3cSetTbuf(pdev, regValue);

    // REG_I3C_SAMPLE_CYCLE
    __raw_writel(0x31, pdev->cfg->sfrBase + REG_I3C_SAMPLE_CYCLE);

    return 0;
}

static void i3cWriteFifo(struct I3cDev *pdev, const uint8_t *txBuf, int nbytes) {
    uint32_t regValue = 0;
    int i;

    for (i = 0; i < nbytes / 4; i ++) {
        memcpy(&regValue, txBuf + (i * 4), 4);
        i3cSetDataBuffer(pdev, regValue);
    }
    if (nbytes & 3) {
        regValue = 0;
        memcpy(&regValue, txBuf + (nbytes & ~3), nbytes & 3);
        i3cSetDataBuffer(pdev, regValue);
    }
}

static void i3cReadFifo(struct I3cDev *pdev, uint8_t *rxBuf, int nbytes) {
    uint32_t regValue = 0;
    int i;

    for (i = 0; i < nbytes / 4; i ++) {
        regValue = i3cGetDataBuffer(pdev);
        memcpy(rxBuf + (i * 4), &regValue, 4);
    }
    if (nbytes & 3) {
        regValue = i3cGetDataBuffer(pdev);
        memcpy(rxBuf + (nbytes & ~3), &regValue, nbytes & 3);
    }
}

static void i3cDumpStatus(struct I3cDev *pdev) {
	PRINTF_INFO("[I3C STATUS DUMP] ");
	PRINTF_INFO("PIO_INTR_STATUS      0x%X", (unsigned int)i3cGetPioIntrStatus(pdev));
	PRINTF_INFO("EXT_PIO_INTR_STATUS  0x%X", (unsigned int)i3cGetExtPioIntrStatus(pdev));

	PRINTF_INFO("RESPONSE_QUEUE_PORT  0x%X", (unsigned int)i3cGetRespQueue(pdev));
}

static int i3cWaitForTransDone(struct I3cDev *pdev) {
    uint64_t timeout;

    timeout = getTimeStampUS() + I3C_TIMEOUT;

    while (getTimeStampUS() < timeout) {
        if (i3cGetPioIntrStatus(pdev) & PIO_INTR_RESP_READY) {
            return 0;
        }
    }
	ERROR_PRINT("i3cWaitForTransDone TIMEOUT\n");
	i3cDumpStatus(pdev);
	i3cCmdQueueReset(pdev);
	return -1;
}

static int i3cWriteData(struct I3cDev *pdev) {
    uint32_t regValue;
    struct I3cState *state = &pdev->state;
    uint16_t dataOffset, size;
    int ret;

    size = (state->tx.size > 64) ? 64 : state->tx.size;
    i3cWriteFifo(pdev, state->tx.cbuf, size);
    dataOffset = size;

    regValue =
        CMD_QUEUE_ROC                           |
        CMD_QUEUE_RNW(I3C_WRITE)                |
        CMD_QUEUE_MODE(0)                       | // TODO: need to check specific mode configuration
        CMD_QUEUE_DEV_INDEX(state->index)       |
        CMD_QUEUE_CP(PRIVATE)                   |
        CMD_QUEUE_TID(0)                        |
        CMD_QUEUE_CMD_ATTR(I3C_MODE_R);

    if (!state->rx.size)
        regValue |= CMD_QUEUE_TOC;


    i3cSetCmdQueue(pdev, regValue);
    regValue = state->tx.size;
    i3cSetCmdQueue(pdev, regValue << 16);

    while (dataOffset < state->tx.size) {
        if (i3cGetPioIntrStatus(pdev) & PIO_INTR_TX_THLD) {
            size = (state->tx.size - dataOffset) > 32 ? 32 : (state->tx.size - dataOffset);
            i3cWriteFifo(pdev, state->tx.cbuf + dataOffset, size);
            dataOffset += size;
        }
    }

    ret = i3cWaitForTransDone(pdev);
    if (ret)
        return ret;

    regValue = i3cGetRespQueue(pdev);

    return regValue >> 28;
}

static int i3cReadData(struct I3cDev *pdev) {
    uint32_t regValue;
    struct I3cState * state = &pdev->state;
    uint16_t dataOffset, size;
    int ret;

    dataOffset = 0;

    regValue =
        CMD_QUEUE_TOC                           |
        CMD_QUEUE_ROC                           |
        CMD_QUEUE_RNW(I3C_READ)                 |
        CMD_QUEUE_MODE(0)                       | // TODO: need to check specific mode configuration
        CMD_QUEUE_DEV_INDEX(state->index)        |
        CMD_QUEUE_CP(PRIVATE)                   |
        CMD_QUEUE_TID(0)                        |
        CMD_QUEUE_CMD_ATTR(I3C_MODE_R);

    i3cSetCmdQueue(pdev, regValue);
    regValue = state->rx.size;
    i3cSetCmdQueue(pdev, regValue << 16);

    while (dataOffset < state->rx.size) {
        if (state->rx.size - dataOffset < 16)
            break;
        if (i3cGetPioIntrStatus(pdev) & PIO_INTR_RX_THLD) {
            size = (state->rx.size - dataOffset) > 16 ? 16 : (state->rx.size - dataOffset);
            i3cReadFifo(pdev, state->rx.buf + dataOffset, size);
            dataOffset += size;
        }
    }

    ret = i3cWaitForTransDone(pdev);
    if (ret)
        return ret;

    if (dataOffset < state->rx.size)
        i3cReadFifo(pdev, state->rx.buf + dataOffset, state->rx.size - dataOffset);

    regValue = i3cGetRespQueue(pdev);

    return regValue >> 28;
}

static int i3cCccSet(struct I3cDev *pdev, struct I3cCccCmd *ccc) {
    uint32_t regValue;
    uint8_t index = 0;

    if (ccc->cmd & I3C_CCC_DIRECT)
        index = ccc->dests->index;

    if (ccc->dests->payload.len)
        i3cWriteFifo(pdev, ccc->dests->payload.data, ccc->dests->payload.len);

    regValue =
        CMD_QUEUE_TOC                           |
        CMD_QUEUE_ROC                           |
        CMD_QUEUE_RNW(I3C_WRITE)                |
        CMD_QUEUE_MODE(0)                       | // TODO: need to check specific mode configuration
        //CMD_QUEUE_BC(ccc->dests->payload.len)   |
        CMD_QUEUE_DEV_INDEX(index)              |
        CMD_QUEUE_CP(CCC)                       |
        CMD_QUEUE_CMD(ccc->cmd)                 |
        CMD_QUEUE_TID(0)                        |
        //CMD_QUEUE_CMD_ATTR(I3C_MODE_I);
        CMD_QUEUE_CMD_ATTR(I3C_MODE_R);

    i3cSetCmdQueue(pdev, regValue);
    regValue = 0;
    if (ccc->dests->payload.len) {
        regValue = ccc->dests->payload.len;
    }
    i3cSetCmdQueue(pdev, regValue << 16);
    //if (ccc->dests->payload.len)
    //    memcpy(&regValue, ccc->dests->payload.data, ccc->dests->payload.len);
    //i3cSetCmdQueue(pdev, regValue);

    while (!(i3cGetPioIntrStatus(pdev) & PIO_INTR_RESP_READY));
    i3cSetPioIntrStatus(pdev, PIO_INTR_RESP_READY);

    regValue = i3cGetRespQueue(pdev);
    //regValue &= (MASK_4BITS << 28);

    return regValue >> 28;
}

static int i3cCccGet(struct I3cDev *pdev, struct I3cCccCmd *ccc) {
    uint32_t regValue;
    uint8_t index = 0;

    if (ccc->cmd & I3C_CCC_DIRECT)
        index = ccc->dests->index;

    regValue =
        CMD_QUEUE_TOC                           |
        CMD_QUEUE_ROC                           |
        CMD_QUEUE_RNW(I3C_READ)                 |
        CMD_QUEUE_MODE(0)                       | // TODO: need to check specific mode configuration
        CMD_QUEUE_DEV_INDEX(index)              |
        CMD_QUEUE_CP(CCC)                       |
        CMD_QUEUE_CMD(ccc->cmd)                 |
        CMD_QUEUE_TID(0)                        |
        CMD_QUEUE_CMD_ATTR(I3C_MODE_R);

    i3cSetCmdQueue(pdev, regValue);
    regValue = ccc->dests->payload.len;
    i3cSetCmdQueue(pdev, regValue << 16);

    while (!(i3cGetPioIntrStatus(pdev) & PIO_INTR_RESP_READY));
    i3cSetPioIntrStatus(pdev, PIO_INTR_RESP_READY);

    regValue = i3cGetRespQueue(pdev);
    if (regValue &= (MASK_4BITS << 28))
        return regValue >> 28;

    i3cReadFifo(pdev, ccc->dests->payload.data, ccc->dests->payload.len);
    
    return 0;
}

static int i3cSendCcc(struct I3cDev *pdev, struct I3cCccCmd *ccc) {
    int ret = 0;

    if (ccc->cmd == I3C_CCC_BROADCAST_ENTDAA)
        return -1;

    if (ccc->rnw)
        ret = i3cCccGet(pdev, ccc);
    else
        ret = i3cCccSet(pdev, ccc);

    return ret;
}

static void i3cCccCmdDestInit(struct I3cCccCmdDest *dest, uint8_t index, uint16_t len, void* data) {
    dest->index = index;
    dest->payload.len = len;
    if (len)
        dest->payload.data = data;
    else
        dest->payload.data = NULL;
}

static void i3cCccCmdInit(struct I3cCccCmd *ccc, enum I3cRnW rnw, uint8_t cmd,
        struct I3cCccCmdDest *dests, uint32_t ndests) {
    ccc->rnw = rnw;
    ccc->cmd = cmd;
    ccc->dests = dests;
    ccc->ndests = ndests;
    ccc->err = 0;
}

static int i3cCccGetpid(struct I3cDev *pdev, uint8_t index) {
    struct I3cPid *pid = &pdev->pid[index];
    struct I3cCccCmdDest dest;
    struct I3cCccCmd ccc;
    int ret = 0;

    // init dest
    i3cCccCmdDestInit(&dest, index, sizeof(struct I3cPid), pid);

    // init cmd
    i3cCccCmdInit(&ccc, I3C_READ, I3C_CCC_DIRECT_GETPID, &dest, 1);

    ret = i3cSendCcc(pdev, &ccc);

    return ret;
}

int i3cCccSetmrl(uint32_t busId, uint32_t index, struct I3cMrl *mrl) {
    struct I3cCccCmdDest dest;
    struct I3cCccCmd ccc;
    struct I3cDev *pdev = &mI3cDev[busId];
    int ret;
    uint8_t cmd;

    if (index == I3C_BROADCAST_ADDR)
        cmd = I3C_CCC_BROADCAST_SETMRL;
    else
        cmd = I3C_CCC_DIRECT_SETMRL;

    i3cCccCmdDestInit(&dest, index, sizeof(struct I3cMrl), mrl);
    i3cCccCmdInit(&ccc, I3C_WRITE, cmd, &dest, 1);

    ret = i3cSendCcc(pdev, &ccc);

    return ret;
}

int i3cCccEnecDisec(uint32_t busId, uint32_t index, bool enable, uint8_t evts) {
    struct I3cEvents events;
    struct I3cCccCmdDest dest;
    struct I3cCccCmd ccc;
    struct I3cDev *pdev = &mI3cDev[busId];
    int ret;
    uint8_t cmd;

    if (enable) {
        if (index == I3C_BROADCAST_ADDR)
            cmd = I3C_CCC_BROADCAST_ENEC;
        else
            cmd = I3C_CCC_DIRECT_ENEC;
    } else {
        if (index == I3C_BROADCAST_ADDR)
            cmd = I3C_CCC_BROADCAST_DISEC;
        else
            cmd = I3C_CCC_DIRECT_DISEC;
    }

    events.events = evts;
    i3cCccCmdDestInit(&dest, index, sizeof(struct I3cEvents), &events);
    //i3cCccCmdDestInit(&dest, index, 0, NULL);
    i3cCccCmdInit(&ccc, I3C_WRITE, cmd, &dest, 1);

    ret = i3cSendCcc(pdev, &ccc);

    return ret;
}

static int i3cCccRstdaa(struct I3cDev *pdev, uint8_t index) {
    struct I3cCccCmdDest dest;
    struct I3cCccCmd ccc;
    int ret, i;
    uint8_t cmd;

    if (index == I3C_BROADCAST_ADDR) {
        for (i = 0; i < I3C_BUS_MAX_DEVS; i++)
            i3cSetDynamicAddr(pdev, i, 0);
        cmd = I3C_CCC_BROADCAST_RSTDAA;
    } else {
        i3cSetDynamicAddr(pdev, index, 0);
        cmd = I3C_CCC_DIRECT_RSTDAA;
    }

    i3cCccCmdDestInit(&dest, I3C_BROADCAST_ADDR, 0, NULL);
    i3cCccCmdInit(&ccc, I3C_WRITE, cmd, &dest, 1);

    ret = i3cSendCcc(pdev, &ccc);

    return ret;
}

static int i3cCccDaa(struct I3cDev *pdev) {
    uint32_t regValue;
    int i;
    uint8_t cmd;

    cmd = I3C_CCC_BROADCAST_ENTDAA;

    for (i = 0; i < I3C_BUS_MAX_DEVS; i++) {
        //clear dynamic address
        regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_EXT_DEVICE_ADDRESS_TABLE(i));
        regValue &= ~(1 << 7);
        __raw_writel(regValue, pdev->cfg->sfrBase + REG_I3C_EXT_DEVICE_ADDRESS_TABLE(i));

        i3cSetDynamicAddr(pdev, i, mI3cAddrPool[i]);
    }

    regValue =
        CMD_QUEUE_TOC                           |
        CMD_QUEUE_ROC                           |
        CMD_QUEUE_DEV_COUNT(I3C_BUS_MAX_DEVS)   |
        CMD_QUEUE_DEV_INDEX(0)                  |
        CMD_QUEUE_CMD(cmd)                      |
        CMD_QUEUE_TID(0)                        |
        CMD_QUEUE_CMD_ATTR(I3C_MODE_A);

    i3cSetCmdQueue(pdev, regValue);

    while (!(i3cGetPioIntrStatus(pdev) & PIO_INTR_RESP_READY));
    i3cSetPioIntrStatus(pdev, PIO_INTR_RESP_READY);

    regValue = i3cGetRespQueue(pdev);
    regValue &= (MASK_4BITS << 28);

    if (regValue)
        return regValue;

    // confirm dynamic address assigned
    for (i = 0; i < I3C_BUS_MAX_DEVS; i++) {
        regValue = __raw_readl(pdev->cfg->sfrBase + REG_I3C_EXT_DEVICE_ADDRESS_TABLE(i));
        if (!(regValue & (1 << 7)))
            return -1;
    }

    return 0;
}

static int i3cStartPolling(struct I3cDev *pdev) {
    struct I3cState * state = &pdev->state;
    size_t txOffset = state->tx.offset;
    size_t rxOffset = state->rx.offset;
    uint32_t id;
    int ret = 0;

#if defined(SEOS)
    if (atomicReadByte(&state->masterState) == I3C_MASTER_START) {
#elif defined(EMBOS)
    if (state->masterState == I3C_MASTER_START) {
#endif
        if (state->tx.size > 0) {
#if defined(SEOS)
            atomicWriteByte(&state->masterState, I3C_MASTER_TX);
#elif defined(EMBOS)
            state->masterState = I3C_MASTER_TX;
#endif
            ret = i3cWriteData(pdev);
            if (ret) {
                ERROR_PRINT("i3cWrite fail ERROR %d\n", ret);
                goto out;
            }
        }
        if (state->rx.size > 0) {
#if defined(SEOS)
            atomicWriteByte(&state->masterState, I3C_MASTER_RX);
#elif defined(EMBOS)
            state->masterState = I3C_MASTER_RX;
#endif
            ret = i3cReadData(pdev);
            if (ret) {
                ERROR_PRINT("i3cRead fail ERROR %d\n", ret);
                goto out;
            }
        }

out:
        do {
#if defined(SEOS)
            id = atomicAdd32bits(&pdev->next, 1);
#elif defined(EMBOS)
            id = pdev->next++;
#endif
        } while (!id);

#if defined(SEOS)
        atomicWriteByte(&state->masterState, I3C_MASTER_IDLE);
#elif defined(EMBOS)
        state->masterState = I3C_MASTER_IDLE;
#endif
        i3cInvokeCallback(state, txOffset, rxOffset, ret);
    }
    return ret;
}

int i3cInit() {
    int i, j, ret;
    struct I3cDev *pdev;
    struct I3cState *state;

#if defined (SEOS)
    atomicBitsetInit(mXfersValid, I3C_MAX_QUEUE_DEPTH);
#elif defined(EMBOS)
    memset(mXfersValid, 0, sizeof(mXfersValid));
#endif

    for (i = 0; i < I3C_CHANNEL_MAX; i++) {
        // connect cfg from varinant and device
        const struct I3cCfg *cfg = getI3cCfg(i);
        if (!cfg)
            continue;

        pdev = &mI3cDev[i];
        state = &pdev->state;
        pdev->cfg = cfg;
        pdev->next = 2;
        pdev->last = 1;

        // sw reset
        i3cReset(pdev);

        // set clock
        ret = i3cClockConfig(pdev);
        if (ret) {
            ERROR_PRINT("i3cClockConfig %d fail\n", i);
            continue;
        }

        // HC_CONTROL BUS_ENABLE
        i3cHcCtrlBusEnable(pdev);

        // set ext device ctrl if needed
        i3cExtDeviceCtrl(pdev, EXT_DEVICE_CTRL_TX_STALL_EN |
                               EXT_DEVICE_CTRL_RX_STALL_EN);

        // pio interrupt enable all
        i3cPioIntrEnableAll(pdev);

        // ext pio interrupt enable all
        i3cExtPioIntrEnableAll(pdev);

        // queue threshold set if needed
        // data buffer  threshold set if needed

        cmuSetDivider(CMU_CLK_CHUB_I3C0, 0xF);
        // DISEC
        ret = i3cCccEnecDisec(i, I3C_BROADCAST_ADDR, DISABLE, I3C_EVENT_INT);
        if (ret) {
            ERROR_PRINT("i3cCccDisec %d %d fail\n", i, ret);
        }
                
        // RSTDAA
        ret = i3cCccRstdaa(pdev, I3C_BROADCAST_ADDR);
        if (ret) {
            ERROR_PRINT("i3cCccRstdaa %d %d fail\n", i, ret);
            //continue;
        }

        // ENTDAA
        ret = i3cCccDaa(pdev);
        if (ret) {
            ERROR_PRINT("i3cCccDaa %d %d fail\n", i, ret);
            //continue;
        }

        // ENEC
        ret = i3cCccEnecDisec(i, I3C_BROADCAST_ADDR, ENABLE, I3C_EVENT_INT);
        if (ret) {
            ERROR_PRINT("i3cCccEnec %d %d fail\n", i, ret);
            //continue;
        }
        //Disable IBI
        i3cSetSirReject(pdev, i);

        // get PID
        for (j = 0; j < I3C_BUS_MAX_DEVS; j++) {
            ret = i3cCccGetpid(pdev, j);
            if (ret)
                ERROR_PRINT("i3cCccGetpid %d failed\n", j);
        }
        cmuSetDivider(CMU_CLK_CHUB_I3C0, 0x2);

#if defined(SEOS)
        atomicWriteByte(&state->masterState, I3C_MASTER_IDLE);
#elif defined(EMBOS)
        state->masterState = I3C_MASTER_IDLE;
#endif

        if (pdev->cfg->xferMode == I3C_MODE_INTERRUPT) {
            // i3cEnableInterruptAll(pdev);
            // i3cEnableInterruptSigAll(pdev);
            // NVIC_EnableIRQ(pdev->cfg->irq)
        }
    }

    //INFO_PRINT("I3C Init Done\n");
    return 0;
}

int i3cMasterRequest(uint32_t busId, struct I3cPid *pid) {
    int handle, i;
    struct I3cDev *pdev;

    if (busId >= I3C_CHANNEL_MAX)
        return -EINVAL;

    handle = -1;
    pdev = &mI3cDev[busId];

    for (i = 0; i < I3C_BUS_MAX_DEVS; i++) {
        if (!memcmp(pid, pdev->pid + i, sizeof(struct I3cPid))) {
            handle = i;
            break;
        }
    }

    return handle;
}

int i3cMasterTxRx(uint32_t busId, uint32_t handle,
        const void *txBuf, size_t txSize, void *rxBuf, size_t rxSize,
        I3cCallbackF callback, void *cookie) {
    uint32_t id;
    struct I3cDev *pdev;
    struct I3cState *state;
    struct I3cXfer *xfer;
    int ret = 0;

    if (busId >= I3C_CHANNEL_MAX)
        return -EINVAL;

    else if (handle >= I3C_BUS_MAX_DEVS)
        return -ENXIO;

    pdev = &mI3cDev[busId];
    state = &pdev->state;
    xfer = i3cGetXfer();

    if (xfer) {
        xfer->busId = busId;
        xfer->index = handle;
        xfer->txBuf = txBuf;
        xfer->txSize = txSize;
        xfer->rxBuf = rxBuf;
        xfer->rxSize = rxSize;
        xfer->callback = callback;
        xfer->cookie = cookie;
#if defined(SEOS)
        xfer->tid = osGetCurrentTid();
#endif

        do {
#if defined(SEOS)
            id = atomicAdd32bits(&pdev->last, 1);
#elif defined(EMBOS)
            id = pdev->last++;
#endif
        } while (!id);

#if defined(SEOS)
        atomicWrite32bits(&xfer->id, id);
#elif defined(EMBOS)
        xfer->id = id;
#endif

#if defined(SEOS)
        if (atomicCmpXchgByte((uint8_t *)&state->masterState,
                    I3C_MASTER_IDLE, I3C_MASTER_START)) {
            if (atomicCmpXchg32bits(&xfer->id, id, 0)) {
#elif defined(EMBOS)
        if (state->masterState == I3C_MASTER_IDLE) {
            state->masterState = I3C_MASTER_START;
            if (xfer->id == id) {
                xfer->id = 0;
#endif
                state->index = xfer->index;
                state->tx.cbuf = xfer->txBuf;
                state->tx.offset = 0;
                state->tx.size = xfer->txSize;
                state->rx.buf = xfer->rxBuf;
                state->rx.offset = 0;
                state->rx.size = xfer->rxSize;
                state->tid = xfer->tid;
                state->callback = xfer->callback;
                state->cookie = xfer->cookie;

                i3cPutXfer(xfer);
                if (pdev->cfg->xferMode == I3C_MODE_POLLING)
                    ret = i3cStartPolling(pdev);
            }
        }

        return ret;
    } else {
        return -EBUSY;
    }
}

int i3cRead(uint32_t busId, uint32_t handle, uint32_t regAddr, void *rxBuf, size_t rxSize)
{
       if( rxBuf == NULL || rxSize == 0 || rxSize > I3C_MAX_READ_LEN)
            return -1;

        ((uint8_t *)rxBuf)[0] = (uint8_t)regAddr;

        return i3cMasterTxRx(busId, handle, rxBuf, 1, rxBuf, rxSize, NULL, NULL);
}

int i3cReadLarge(uint32_t busId, uint32_t handle, uint32_t regAddr, void *rxBuf, size_t rxSize, bool inc)
{
    int ret, i;
    uint8_t offset, size;

    ret = 0;

    if( rxBuf == NULL || rxSize == 0)
        return -1;

    for (i = 0; i < DIV_ROUND_UP(rxSize, I3C_MAX_READ_LEN); i++) {
        offset = i * I3C_MAX_READ_LEN;
        size = (rxSize - offset) > I3C_MAX_READ_LEN ? I3C_MAX_READ_LEN : (rxSize - offset);

        if (inc)
            ((uint8_t *)rxBuf)[offset] = (uint8_t)(regAddr + (offset)) ;
        else
            ((uint8_t *)rxBuf)[offset] = (uint8_t)regAddr;

        ret = i3cMasterTxRx(busId, handle, (uint8_t *)rxBuf + offset, 1, (uint8_t *)rxBuf + offset, size , NULL, NULL);

        if (ret)
            break;
    }

    return ret;
}

int i3cWrite(uint32_t busId, uint32_t handle, uint32_t regAddr, void *txBuf, size_t txSize)
{
        uint8_t tempBuf[128];
        int ret;

        if( txBuf == NULL || txSize == 0) {
            PRINTF_INFO("i3cWrite: invalid parameter (txBuf=%x, length=%d)\n", (unsigned int)txBuf, (int)txSize);
            return -1;
        }

        memcpy(tempBuf + 1, txBuf, txSize);
        tempBuf[0] = (uint8_t)regAddr;

        ret = i3cMasterTxRx(busId, handle, tempBuf, txSize + 1, NULL, 0, NULL, NULL);
        return ret;
}

void i3cSaveState() {
    return;
}

void i3cRestoreState() {
    i3cInit();
}
