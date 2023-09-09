#include <gpio.h>
#include <i2c.h>
#include CSP_HEADER(gpio)

typedef struct {
    GpioPinNumType pinSCL;
    GpioPinNumType pinSDA;
}I2cGpioInfoType;

struct I2cMsgType {
    uint16_t flags;
    uint16_t addr;
    uint16_t rxSize;
    uint16_t txSize;
    uint16_t rxPtr;
    uint16_t txPtr;
    uint8_t *rxBuf;
    uint8_t *txBuf;
    void *cookie;
    I2cCallbackF callback;
};

struct I2cInfoType {
    uint8_t mode;
    uint8_t speedMode;
    uint8_t operationMode;
    uint8_t state;
    uint8_t gpioSCL;
    uint8_t gpioSDA;
    uint8_t irqNum;
    uint8_t stop;
    uint16_t tid;
    uint32_t sfrBase;
    uint32_t speed;
    uint32_t errorState;
    struct I2cMsgType msg;
};

#define DEC_I2C(base, scl, sda, irq, opmode) \
    {.sfrBase = base, .gpioSCL = scl, .gpioSDA = sda, .irqNum = irq, .operationMode = opmode}

struct I2cInfoType mI2cInfo[I2C_CHANNEL_MAX] = {
    DEC_I2C(I2C0_BASE_ADDRESS, GPH0_0, GPH0_1, USI_CHUB00_IRQn, I2C_MODE_POLLING),    // I2C_CHUB0
    DEC_I2C(I2C1_BASE_ADDRESS, GPH0_2, GPH0_3, I2C_CHUB00_IRQn, I2C_MODE_POLLING),    // I2C_CHUB1
    DEC_I2C(I2C2_BASE_ADDRESS, GPH1_0, GPH1_1, USI_CHUB01_IRQn, I2C_MODE_POLLING),    // I2C_CHUB2
    DEC_I2C(I2C3_BASE_ADDRESS, GPH1_2, GPH1_3, I2C_CHUB01_IRQn, I2C_MODE_POLLING),    // I2C_CHUB3
    DEC_I2C(I2C4_BASE_ADDRESS, GPH2_0, GPH2_1, USI_CHUB02_IRQn, I2C_MODE_POLLING),    // I2C_CHUB4
    DEC_I2C(I2C5_BASE_ADDRESS, GPH2_2, GPH2_3, I2C_CHUB02_IRQn, I2C_MODE_POLLING),    // I2C_CHUB5
};

void i2cSetOperationMode(uint32_t busId, uint8_t mode) {
    mI2cInfo[busId].operationMode = mode;
}
