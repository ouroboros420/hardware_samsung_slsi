#include <gpio.h>
#include <i2c.h>
#include CSP_HEADER(gpio)
#include CSP_HEADER(i2cSfrBase)

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
#ifdef DEF_IF_INTERRUPT_MODE
    DEC_I2C(I2C2_BASE_ADDRESS, GPH0_4, GPH0_5, USI_CHUB01_IRQn, I2C_MODE_INTERRUPT),    // I2C_CHUB2
    DEC_I2C(I2C3_BASE_ADDRESS, GPH0_6, GPH0_7, I2C_CHUB01_IRQn, I2C_MODE_INTERRUPT),    // I2C_CHUB3
    DEC_I2C(I2C4_BASE_ADDRESS, GPH1_0, GPH1_1, USI_CHUB02_IRQn, I2C_MODE_INTERRUPT),    // I2C_CHUB4
#else
    DEC_I2C(I2C2_BASE_ADDRESS, GPH0_4, GPH0_5, USI_CHUB01_IRQn, I2C_MODE_POLLING),    // I2C_CHUB2
    DEC_I2C(I2C3_BASE_ADDRESS, GPH0_6, GPH0_7, I2C_CHUB01_IRQn, I2C_MODE_POLLING),    // I2C_CHUB3
    DEC_I2C(I2C4_BASE_ADDRESS, GPH1_0, GPH1_1, USI_CHUB02_IRQn, I2C_MODE_POLLING),    // I2C_CHUB4
#endif
    DEC_I2C(I2C5_BASE_ADDRESS, GPH1_2, GPH1_3, I2C_CHUB02_IRQn, I2C_MODE_POLLING),    // I2C_CHUB5
    DEC_I2C(I2C6_BASE_ADDRESS, GPM00_0, GPM01_0, CMGP_USI0_IRQ, I2C_MODE_POLLING),    // I2C_CMGP0
    DEC_I2C(I2C7_BASE_ADDRESS, GPM02_0, GPM03_0, CMGP_I2C0_IRQ, I2C_MODE_POLLING),    // I2C_CMGP1
    DEC_I2C(I2C8_BASE_ADDRESS, GPM04_0, GPM05_0, CMGP_USI1_IRQ, I2C_MODE_POLLING),    // I2C_CMGP2
    DEC_I2C(I2C9_BASE_ADDRESS, GPM06_0, GPM07_0, CMGP_I2C1_IRQ, I2C_MODE_POLLING),    // I2C_CMGP3
    DEC_I2C(I2C10_BASE_ADDRESS, GPM08_0, GPM09_0, CMGP_USI2_IRQ, I2C_MODE_POLLING),   // I2C_CMGP4
    DEC_I2C(I2C11_BASE_ADDRESS, GPM10_0, GPM11_0, CMGP_I2C2_IRQ, I2C_MODE_POLLING),   // I2C_CMGP5
    DEC_I2C(I2C12_BASE_ADDRESS, GPM12_0, GPM13_0, CMGP_USI3_IRQ, I2C_MODE_POLLING),   // I2C_CMGP6
    DEC_I2C(I2C13_BASE_ADDRESS, GPM14_0, GPM15_0, CMGP_I2C3_IRQ, I2C_MODE_POLLING)    // I2C_CMGP7
};

void i2cSetOperationMode(uint32_t busId, uint8_t mode) {
    mI2cInfo[busId].operationMode = mode;
}

void i2c0_IRQHandler(void) { i2c_IRQHandler(I2C_CHUB0); }
void i2c1_IRQHandler(void) { i2c_IRQHandler(I2C_CHUB1); }
void i2c2_IRQHandler(void) { i2c_IRQHandler(I2C_CHUB2); }
void i2c3_IRQHandler(void) { i2c_IRQHandler(I2C_CHUB3); }
void i2c4_IRQHandler(void) { i2c_IRQHandler(I2C_CHUB4); }
void i2c5_IRQHandler(void) { i2c_IRQHandler(I2C_CHUB5); }
