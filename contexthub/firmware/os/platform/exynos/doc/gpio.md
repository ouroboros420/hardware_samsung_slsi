### GPIO Device Driver
### Table of Contents
1. [Defines](#defines)
2. [Types](#types)
3. [Functions](#functions)

<br>
### Defines<a name="defines"></a>

Defines for GPIO Function selection
<br>

```c
#define     GPIO_CON_INPUT              0x0     //input function setting
#define     GPIO_CON_OUTPUT             0x1     //output function setting
#define     GPIO_CON_FUNC_1             0x2     //function 1
#define     GPIO_CON_FUNC_2             0x3     //function 2
#define     GPIO_CON_FUNC_3             0x4     //function 3
#define     GPIO_CON_FUNC_4             0x5     //function 4
#define     GPIO_CON_FUNC_5             0x6     //function 5
#define     GPIO_CON_FUNC_6             0x7     //function 6
#define     GPIO_CON_EINT               0xF     //function 7
```

<br>
Defines for GPIO Data control

```c
#define     GPIO_DAT_LOW                0x0     //low level ouput
#define     GPIO_DAT_HIGH               0x1     //high lvel output
```

<br>
Defines for Pull-Up / Pull-Down Resistor Control

```c
#define     GPIO_PUD_DISABLE            0x0     //pull-up & pull-down disable
#define     GPIO_PUD_PULLDOWN           0x1     //pull-down enable
#define     GPIO_PUD_RESERVED           0x2     //Reserved
#define     GPIO_PUD_PULLUP             0x3     //pull-up enable
```

<br>
Defines for GPIO Drive Strength Control

```c
#define     GPIO_DRV_1X                 0x0
#define     GPIO_DRV_2X                 0x1
#define     GPIO_DRV_3X                 0x2
#define     GPIO_DRV_4X                 0x3
```


---

### Types<a name="types"></a>

```c
typedef enum {
    Low_Level    = 0x0,
    High_Level   = 0x1,
    Falling_Edge = 0x2,
    Rising_Edge  = 0x3,
    Both_Edge    = 0x4
} IntTriggerType;

typedef enum {
    eDisFLT      = (uint32_t) 0x0,
    eEnFLT       = (uint32_t) 0x1,
} IntFilterEnType;

typedef enum {
    eNoFLT       = (uint32_t) 0x0,
    eDelayFLT    = (uint32_t) 0x1,
    eDigitalFLT  = (uint32_t) 0x2,
} IntFilterSel;

typedef enum {
    GPIO_CHUB_START_PIN_NUM = 0,
    GPH0_0 = GPIO_CHUB_START_PIN_NUM,
    GPH0_1,
    GPH0_2,
    GPH0_3,
    GPH0_4,
    GPH0_5,
    GPH0_6,
    GPH0_7,

    GPH1_0,
    GPH1_1,
    GPH1_2,
    GPH1_3,
 
    GPH2_0,
    GPH2_1,
    GPH2_2,
    GPH2_3,
    GPH2_4,
    GPH2_5,
 } GpioPinNumType;
```
* Note : The value of GpioPinNumType is different for each product.<br>
         It is defined in firmware/os/platform/exynos/inc/plat/gpio/gpio**xxxx**.h (xxxx is the product code). <br>
         For example, gpio9630.h is for S5E9630.

---

### Functions<a name="functions"></a>
```c
void gpioSetFunction(GpioPinNumType gpioPinNum, uint32_t gpioFunction)
```
**DESCRIPTION**<br>
Select Function for each GPIO pin.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO Pin |
| gpioFunction | GPIO Function <br>(GPIO_CON_INPUT, GPIO_CON_OUTPUT, ..., GPIO_CON_EINT) |

<br>
<br>

```c
void gpioSetPud(GpioPinNumType gpioPinNum, uint32_t gpioPudValue);
```
**DESCRIPTION**<br>
Config pull-up / pull-down resister for each GPIO pin.<br>

**PARAMETERS**<br>


| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO Pin |
| gpioPudValue | Pull-Up, Pull-Down resistor option.<br>(GPIO_PUD_DISABLE, GPIO_PUD_PULLDOWN, GPIO_PUD_PULLUP) |

<br>
<br>

```c
void gpioSetDrvStrength(GpioPinNumType gpioPinNum, uint32_t gpioDrvValue)
```
**DESCRIPTION**<br>
Set drive strength for specified GPIO pin<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO pin |
| gpioDrvValue | Drive Strength <br> (GPIO_DRV_1X, GPIO_DRV_2X, GPIO_DRV_3X, GPIO_DRV_4X) |

<br>
<br>


```c
void gpioSetData(GpioPinNumType gpioPinNum, uint32_t gpioDataValue)
```
**DESCRIPTION**<br>
Set Data for specified GPIO pin.(GPIO Output control) <br>
This function works only when the function of the GPIO pin is set as OUTPUT. <br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO pin |
| gpioDataValue | GPIO Output state <br> (GPIO_DAT_LOW, GPIO_DAT_HIGH) |

<br>
<br>


```c
uint32_t gpioGetData(GpioPinNumType gpioPinNum)
```
**DESCRIPTION**<br>
Read the status of GPIO pin.<br>
This function returns current status of GPIO regardless of function. <br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO pin |

**RETURN VALUE**<br>
Returns current state of GPIO pin.<br>
    0 : Low <br>
    1 : High 

<br>
<br>


```c
void gpioEnableInterrupt(GpioPinNumType gpioPinNum, uint32_t en)
```
**DESCRIPTION**<br>
Enable or Disable interrupt of the GPIO pin.<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO pin |
| en | 0 : Disable <br> 1 : Enable |

<br>
<br>


```c
void gpioClearPending(GpioPinNumType gpioPinNum)
```
**DESCRIPTION**<br>
Clear interrupt pending register<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO pin |

<br>
<br>

```c
void gpioSetExtInterrupt(GpioPinNumType gpioPinNum,
                         IntTriggerType intTrigger,
                         IntFilterSel   intFilter,
                         uint32_t       filterWidth, 
                         void (*callbackFunction) (void))
```

**DESCRIPTION**<br>
Config external interrupt<br>

**PARAMETERS**<br>

| Parameter | Description |
| --- | ---|
| gpioPinNum | Number of GPIO pin |
| intTrigger | Interrupt trig type : <br>Low_Level, High_Level, Falling_Edge, Rising_Edge, Both_Edge |
| intFilter | Finter selection : <br>eNoFLT : No Filter<br>eDelayFLT : Delay Filter<br>eDigitalFLT : Digital Filter |
| filterWidth | Filter Width :  |
| callbackFunction | Callback function of this GPIO interrupt |


---
