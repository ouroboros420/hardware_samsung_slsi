### I2C Device Driver

### Defines
```c
#define I2C_CHUB0                   0
#define I2C_CHUB1                   1
#define I2C_CHUB2                   2
#define I2C_CHUB3                   3
#define I2C_CHUB4                   4
#define I2C_CHUB5                   5
#define I2C_CHUB_CHANNEL_MAX        6
#define I2C_CMGP0                   6
#define I2C_CMGP1                   7
#define I2C_CMGP2                   8
#define I2C_CMGP3                   9
#define I2C_CMGP4                   10
#define I2C_CMGP5                   11
#define I2C_CMGP6                   12
#define I2C_CMGP7                   13
#define I2C_CHANNEL_MAX             14
```
---

### Types
```c
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
```
<br>

```c
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
```
---

### Functions
```c
int i2cMasterRequest(uint32_t busId, uint32_t speedInHz)
```
**DESCRIPTION**<br>
Request I2C Master channel.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| busId | Spi channel number. |
| speedInHz | Speed of i2c operation clock in Hz |

<br>
<br>


```c
int i2cMasterRelease(uint32_t busId)
```
**DESCRIPTION**<br>
Read and Transfer SPI message.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| busId | Spi channel number. |

<br>
<br>

```c
int i2cMasterTxRx(uint32_t busId,
                  uint32_t addr,
                  const void *txBuf,
                  size_t txSize,
                  void *rxBuf,
                  size_t rxSize,
                  I2cCallbackF callback,
                  void *cookie)
```
**DESCRIPTION**<br>
Read and Transfer SPI message.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| busId | Spi channel number. |
| addr | i2c address |
| txBuf | Pointer to a transfer buffer of type void |
| txSize | Size of transfer data |
| rxBuf | Pointer to a read buffer of type void |
| rxSize | Size of read data |
| callback | Pointer to the callback routing to be called after i2c is done |
| cookie | A void pointer which is used as parameter for the callback function |

<br>
<br>
---
