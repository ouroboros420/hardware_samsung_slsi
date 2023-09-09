/*
 * Copyright (C) 2016 The Android Open Source Project
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

#ifndef __PLAT_I2C_H
#define __PLAT_I2C_H

#include <gpio.h>
#include <platform.h>
#include <cmsis.h>
#include <gpio.h>
#include <plat.h>
#include <typedef.h>

struct i2c_algorithm;
struct i2c_adapter;

#define CTL                     0x0000
#define FIFO_CTL                0x0004
#define TRAILING_CTL            0x0008
#define INT_EN                  0x0020
#define INT_STAT                0x0024
#define FIFO_STAT               0x0030
#define TXDATA                  0x0034
#define RXDATA                  0x0038
#define I2C_CONF                0x0040
#define I2C_AUTO_CONF           0x0044
#define I2C_TIMEOUT             0x0048
#define I2C_MANUAL_CMD          0x004C
#define I2C_TRANS_STATUS        0x0050
#define I2C_TIMING_HS1          0x0054
#define I2C_TIMING_HS2          0x0058
#define I2C_TIMING_HS3          0x005C
#define I2C_TIMING_FS1          0x0060
#define I2C_TIMING_FS2          0x0064
#define I2C_TIMING_FS3          0x0068
#define I2C_TIMING_SLA          0x006C

#define I2C_ADDR                0x0070

#define I2C_SPEED_400KHZ        400000

#define I2C_MASTER              0
#define I2C_SLAVE               1
#define I2C_POLLING             0
#define I2C_INTERRUPT           1
#define I2C_BATCHER             2

#define HSI2C_INT_ALL           0xFFFF


/* Register Map */
#define HSI2C_CTL               0x00
#define HSI2C_FIFO_CTL          0x04
#define HSI2C_TRAILIG_CTL       0x08
#define HSI2C_CLK_CTL           0x0C
#define HSI2C_CLK_SLOT          0x10
#define HSI2C_INT_ENABLE        0x20
#define HSI2C_INT_STATUS        0x24
#define HSI2C_ERR_STATUS        0x2C
#define HSI2C_FIFO_STATUS       0x30
#define HSI2C_TX_DATA           0x34
#define HSI2C_RX_DATA           0x38
#define HSI2C_CONF              0x40
#define HSI2C_AUTO_CONF         0x44
#define HSI2C_TIMEOUT           0x48
#define HSI2C_MANUAL_CMD        0x4C
#define HSI2C_TRANS_STATUS      0x50
#define HSI2C_TIMING_HS1        0x54
#define HSI2C_TIMING_HS2        0x58
#define HSI2C_TIMING_HS3        0x5C
#define HSI2C_TIMING_FS1        0x60
#define HSI2C_TIMING_FS2        0x64
#define HSI2C_TIMING_FS3        0x68
#define HSI2C_TIMING_SLA        0x6C
#define HSI2C_ADDR              0x70

#define HSI2C_SMRelease         0x200

/* HSI2C Batcher Register Map */
#define HSI2C_BATCHER_CON               0x500
#define HSI2C_BATCHER_STATE             0x504
#define HSI2C_BATCHER_INT_EN            0x508
#define HSI2C_BATCHER_FIFO_STATUS       0x50C
#define HSI2C_BATCHER_INT_STATUS        0x510
#define HSI2C_BATCHER_OPCODE            0x600

#define HSI2C_START_PAYLOAD             0x1000
#define HSI2C_END_PAYLOAD               0x1060

/* I2C_CTL Register bits */
#define HSI2C_FUNC_MODE_I2C                     (1u << 0)
#define HSI2C_CS_ENB                            (1u << 0)
#define HSI2C_MASTER                            (1u << 3)
#define HSI2C_RXCHON                            (1u << 6)
#define HSI2C_TXCHON                            (1u << 7)
#define HSI2C_EXT_MSB                           (1u << 29)
#define HSI2C_EXT_ADDR                          (1u << 30)
#define HSI2C_SW_RST                            (1u << 31)

/* I2C_FIFO_CTL Register bits */
#define HSI2C_RXFIFO_EN                         (1u << 0)
#define HSI2C_TXFIFO_EN                         (1u << 1)
#define HSI2C_FIFO_MAX                          (0x40)
#define HSI2C_RXFIFO_TRIGGER_LEVEL              (0x8 << 4)
#define HSI2C_TXFIFO_TRIGGER_LEVEL              (0x8 << 16)

/* I2C_TRAILING_CTL Register bits */
#define HSI2C_TRAILING_COUNT                    (0xf)
#define BATCHER_TRAILING_COUNT                  (0x2ff)

/* I2C_INT_EN Register bits */
#define HSI2C_INT_TX_ALMOSTEMPTY_EN             (1u << 0)
#define HSI2C_INT_RX_ALMOSTFULL_EN              (1u << 1)
#define HSI2C_INT_TRAILING_EN                   (1u << 6)
#define HSI2C_INT_TRANSFER_DONE                 (1u << 7)
#define HSI2C_INT_I2C_EN                        (1u << 9)
#define HSI2C_INT_CHK_TRANS_STATE               (0xf << 8)

/* I2C_INT_STAT Register bits */
#define HSI2C_INT_TX_ALMOSTEMPTY                (1u << 0)
#define HSI2C_INT_RX_ALMOSTFULL                 (1u << 1)
#define HSI2C_INT_TX_UNDERRUN                   (1u << 2)
#define HSI2C_INT_TX_OVERRUN                    (1u << 3)
#define HSI2C_INT_RX_UNDERRUN                   (1u << 4)
#define HSI2C_INT_RX_OVERRUN                    (1u << 5)
#define HSI2C_INT_TRAILING                      (1u << 6)
#define HSI2C_INT_TRANSFER_DONE                 (1u << 7)
#define HSI2C_INT_TRANSFER_ABORT                (1u << 8)
#define HSI2C_INT_NO_DEV_ACK                    (1u << 9)
#define HSI2C_INT_NO_DEV                        (1u << 10)
#define HSI2C_INT_TIMEOUT                       (1u << 11)
#define HSI2C_INT_SLAVE_ADDR_MATCH              (1u << 15)
#define HSI2C_RX_INT                            (HSI2C_INT_RX_ALMOSTFULL | \
                                                 HSI2C_INT_RX_UNDERRUN | \
                                                 HSI2C_INT_RX_OVERRUN | \
                                                 HSI2C_INT_TRAILING)

/* I2C_FIFO_STAT Register bits */
#define HSI2C_RX_FIFO_EMPTY                     (1u << 24)
#define HSI2C_RX_FIFO_FULL                      (1u << 23)
#define HSI2C_RX_FIFO_LVL(x)                    ((x >> 16) & 0x7f)
#define HSI2C_RX_FIFO_LVL_MASK                  (0x7F << 16)
#define HSI2C_TX_FIFO_EMPTY                     (1u << 8)
#define HSI2C_TX_FIFO_FULL                      (1u << 7)
#define HSI2C_TX_FIFO_LVL(x)                    ((x >> 0) & 0x7f)
#define HSI2C_TX_FIFO_LVL_MASK                  (0x7F << 0)
#define HSI2C_FIFO_EMPTY                        (HSI2C_RX_FIFO_EMPTY |  \
                                                 HSI2C_TX_FIFO_EMPTY)

/* I2C_CONF Register bits */
#define HSI2C_AUTO_MODE                         (1u << 31)
#define HSI2C_10BIT_ADDR_MODE                   (1u << 30)
#define HSI2C_HS_MODE                           (1u << 29)
#define HSI2C_FILTER_EN_SCL                     (1u << 28)
#define HSI2C_FILTER_EN_SDA                     (1u << 27)
#define HSI2C_FTL_CYCLE_SCL_MASK                (0x7 << 16)
#define HSI2C_FTL_CYCLE_SDA_MASK                (0x7 << 13)

/* I2C_AUTO_CONF Register bits */
#define HSI2C_READ_WRITE                        (1u << 16)
#define HSI2C_STOP_AFTER_TRANS                  (1u << 17)
#define HSI2C_MASTER_RUN                        (1u << 31)

/* I2C_TIMEOUT Register bits */
#define HSI2C_TIMEOUT_EN                        (1u << 31)

/* I2C_TRANS_STATUS register bits */
#define HSI2C_MASTER_BUSY                       (1u << 17)
#define HSI2C_SLAVE_BUSY                        (1u << 16)
#define HSI2C_TIMEOUT_AUTO                      (1u << 4)
#define HSI2C_NO_DEV                            (1u << 3)
#define HSI2C_NO_DEV_ACK                        (1u << 2)
#define HSI2C_TRANS_ABORT                       (1u << 1)
#define HSI2C_TRANS_DONE                        (1u << 0)
#define HSI2C_MAST_ST_MASK                      (0xf << 0)

/* I2C_ADDR register bits */
#define HSI2C_SLV_ADDR_SLV(x)                   ((x & 0x3ff) << 0)
#define HSI2C_SLV_ADDR_MAS(x)                   ((x & 0x3ff) << 10)
#define HSI2C_MASTER_ID(x)                      ((x & 0xff) << 24)
#define MASTER_ID(x)                            ((x & 0x7) + 0x08)

/*
 * Controller operating frequency, timing values for operation
 * are calculated against this frequency
*/

#define HSI2C_HS_TX_CLOCK       2500000
#define HSI2C_FS_TX_CLOCK       400000
#define HSI2C_HIGH_SPD          1
#define HSI2C_FAST_SPD          0

#define HSI2C_POLLING 0
#define HSI2C_INTERRUPT 1

#define EXYNOS5_FIFO_SIZE               16

#define HSI2C_BATCHER_INIT_CMD  0xFFFFFFFF

struct i2c_msg;
/*
 * The following structs are for those who like to implement new bus drivers:
 * i2c_algorithm is the interface to a class of hardware solutions which can
 * be addressed using the same bus algorithms - i.e. bit-banging or the PCF8584
 * to name two of the most common.
 */
struct i2c_algorithm {
    /* If an adapter algorithm can't do I2C-level access, set master_xfer
       to NULL. If an adapter algorithm can do SMBus access, set
       smbus_xfer. If set to NULL, the SMBus protocol is simulated
       using common I2C messages */
    /* master_xfer should return the number of messages successfully
       processed, or a negative value on error */
    int (*master_xfer) (struct i2c_adapter * adap, struct i2c_msg * msgs,
                int num);
    /* To determine what the adapter supports */
    int (*i2c_setup) (struct i2c_adapter * adapter, u32 master, u32 mode,
              u32 speed, u32 slave_addr);
    int (*i2c_cleanup) (struct i2c_adapter * adapter);
};

typedef enum {
    DISABLE_I2C,
    MASTER_TX_MODE,
    MASTER_RX_MODE,
    SLAVE_TX_MODE,
    SLAVE_RX_MODE,
} I2C_MODE;

struct i2c_adapter {
    const struct i2c_algorithm *algo;    /* the algorithm to access the bus */
};

/* flags fort struct i2c_msg.flags */
#define I2C_M_TEN               0x0010    /* this is a ten bit chip address */
#define I2C_M_RD                0x0001    /* read data, from slave to master */
#define I2C_M_STOP              0x8000    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART           0x4000    /* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR      0x2000    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK        0x1000    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK         0x0800    /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN          0x0400    /* length will be first received byte */

struct i2c_msg {
    u16 addr;        /* slave address                        */
    u16 flags;
    u16 len;        /* msg length                           */
    u8 *buf;        /* pointer to msg data                  */
};

struct hsi2c_adapter {
    struct ExynosI2cDev *dev;
    int id;
    struct i2c_adapter adapter;

    struct i2c_msg *msg;

    int state;
    int clock;
    int xfer_speed;
    int vclk_id;

    u32 base;
    int irq;
    int sda;
    int scl;
    int sda_func;
    int scl_func;
    int command;        /* command being executed */
    int need_init;

    u32 master;
    u32 mode;
    u32 slave_addr;
    u8 stop_after_trans;
    unsigned int msg_ptr;
    unsigned int msg_len;
};


struct ExynosI2cBoardCfg {
    int xfer_speed;
    u32 master;
    u32 mode;
    u32 slave_addr;

};

#define I2C_DMA_BUS         0

extern const struct ExynosI2cBoardCfg *boardExynosI2cCfg(uint8_t busId);

#endif                /* __PLAT_I2C_H */
