## Variant
This directory has the board specific options and codes.

### Table of Contents
- [Variant](#variant)
  - [Table of Contents](#table-of-contents)
  - [Makefile <a name="makefile"></a>](#makefile-)
  - [Board Specific Codes <a name="board_specific_code"></a>](#board-specific-codes-)
  - [Multiple Sensor Combination <a name="sensor_combination"></a>](#multiple-sensor-combination-)

<br>

### Makefile <a name="makefile"></a>
The board specific Makefile is firmware/variant/*variant_name*/***variant_name.mk***.
For example, when the variant name is erd9815, the Makefile is firmware/variant/*erd9815/erd9815.mk*.
<br>
There is another option file for make in the same directory with the name of ***\<variant_name\>_conf.mk***. (erd9815_conf.mk for the variant of erd9815).

The following variables are important

|Key|Value|
|---|---|
|VARIANT|erd9815|
|CPU|cortexm4*|
|CHIP|9815|
|PLATFORM|exynos*|
|SOCNAME|s5e9815|
|BOARD|erd|

Note : (*) Do not change the value<br>


---

### Board Specific Codes <a name="board_specific_code"></a>
Header File

Directory : firmware/variant/*\<variant_name\>*/inc/variant/

|File|Description|
|---|---|
|variant.h| Contains board specific options|
|senstype.h| Contains information of sensors currently being used on the board|
|cmgpBoard.h | Contains information of USI channel configuration |
|gpioBoard.h | Contains information of GPIO port configuration |
|me*\>.h| Function declaration for source file |

<br>

1. **GPIO Initialization** <br>
   *gpioBoardInit* function initilize the these kinds of configuration for each GPIO.
   - GPIO Function : Inout, Output, or other function.
   - Status : High or Low
   - Pull-up/down control : Pull-up, Pull-down, or none
   - Drive strength

   *mGpioPortInitValue* has the configuration table.
<br>

|Field No.| Description |
|---|---|
|1| GPIO Number.|
|2| Function.<br>Input = GPIO_CON_INPUT<br> Output=GPIO_CON_OUTPUT<br>or other values|
|3| GPIO Status<br> High = GPIO_DAT_HIGH<br>Low = GPIO_DAT_LOW|
|4| Pull-Up/Down selection<br>Pull-up = GPIO_PUD_PULLUP_ENABLE<br>Pull-down = GPIO_PUD_PULLDOWN_ENABLE<br>Disable both = GPIO_PUD_DISABLE|
|5| GPIO Status in Power-down mode<br>GPIO_CONPDN_OUTPUT_LOW<br>GPIO_CONPDN_OUTPUT_HIGH|
|6| Pull-Up/Down selection in Power-down mode<br>GPIO_CONPDN_OUTPUT_LOW<br>GPIO_CONPDN_OUTPUT_HIGH<br>GPIO_CONPDN_INPUT<br>GPIO_CONPDN_PREVIOUS_STATE|
|7| Drive Strength<br>GPIO_DRV_1X<br>GPIO_DRV_2X<br>GPIO_DRV_3X<br>GPIO_DRV_4X|

<br>

```c
  static const uint8_t mGpioPortInitValue[][7] = {
    // BMI160 Accelation-Gyro Sensor:: SPI_CHUB0 @ XUSI_CHUB0 as SPI
    /* SPI CLK, CS, MISO, MOSI*/
    {GPIO_H0_0, GPH0_0_USI00_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_1, GPH0_1_USI00_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_2, GPH0_2_USI00_CHUB_USI_RTS_SPIDI_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_3, GPH0_3_USI00_CHUB_USI_CTS_SPICS_NA, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    /*  Interrupt : */
    //Interrupt A_INT_1 @ CHUB
    {GPIO_H2_0, GPH2_0_NWEINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    //Interrupt A_INT_2 @ CHUB
    {GPIO_H2_1, GPH2_1_NWEINT, GPIO_DAT_HIGH, GPIO_PUD_DISABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // AK09918 MAGNETIC Sensor:: I2C2_CHUB @ XUSI_CHUB1 as I2C
    // I2C SCL / SDA
    {GPIO_H0_4, GPH0_4_USI01_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_5, GPH0_5_USI01_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},

    // BMI280 PRESSURE:: I2C3_CHUB @ XUSI_CHUB1 as I2C
    /* I2C SCL / SDA */
    {GPIO_H0_6, GPH0_6_I2C_CHUB01_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    {GPIO_H0_7, GPH0_7_I2C_CHUB01_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
    /*  Interrupt : */

    // RPR-0521RS Light Sensor:: I2C4_CHUB @ XUSI_CHUB2 as I2C
  /* I2C SCL / SDA */
  {GPIO_H1_0, GPH1_0_USI02_CHUB_USI_RXD_SPICLK_SCL, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
  {GPIO_H1_1, GPH1_1_USI02_CHUB_USI_TXD_SPIDO_SDA, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X},
  /*  Interrupt : */
  {GPIO_H2_2, GPH2_2_NWEINT, GPIO_DAT_HIGH, GPIO_PUD_PULLUP_ENABLE, GPIO_CONPDN_OUTPUT_LOW, GPIO_PUDPDN_DISABLE, GPIO_DRV_1X}, 
  #endif
   }; 
```
<br>


2. **USI Function** <br>
*cmgpBoardInit* function initializes the serial protocol of USI.<br>
USI has three serial-protocol<br>
   - UART : USI_PROTOCOL_UART
   - SPI : USI_PROTOCOL_SPI
   - I2C : USI_PROTOCOL_I2C


   *mUsiBoardConfig* has the configuration table.

Example code : variant/erd9815/inc/variant/cmgpBoard.h
<br>
```c
UsiBoardConfigType mUsiBoardConfig[] = {
    {USI_CHUB0,         USI_PROTOCOL_I2C},
    {USI_I2C_CHUB0,     USI_PROTOCOL_I2C},
    {USI_CHUB1,         USI_PROTOCOL_SPI},
    {USI_I2C_CHUB2,     USI_PROTOCOL_I2C},
};
```

---
<br>

2. **SPI Channel Configuration** <br>
*spiBoardInit* function in firmware/variant/erd9815/src/spi.c initializes SPI channels.

   *mUsiBoardConfig* has the configuration table.
<br>
```c
static const spi_device_t mSpiDevConf[] = {
    [0] = {
        .m_portId = USI_CHUB1,
        .m_EnAutoCS = 0,
        .m_OpMode = SPI_MASTER,
        .m_TargetSPICLK = 8000000,
        .m_OpClock=8000000,
        .m_ChWidth=SPI_BYTE,
        .m_BusWidth=SPI_BYTE,
#ifdef DEF_IF_INTERRUPT_MODE
        .m_RxTransferMode=SPI_INTERRUPT_MODE,
        .m_TxTransferMode=SPI_INTERRUPT_MODE,
#else
       .m_RxTransferMode=SPI_POLLING_MODE,
       .m_TxTransferMode=SPI_POLLING_MODE,
#endif
        .m_loopback=0,
        .m_FBClkSel = SPI_PHASE_DELAY90,
        .m_CPOL = SPI_ACTIVE_LOW,//Active low
        .m_CPHA = SPI_FORMAT_B,//Format B
    },
};
```
<br>
<br>

### Multiple Sensor Combination <a name="sensor_combination"></a>
Most of vendors use several sensor combination in the same product. 
That feature could be supported by building several os images for each sensor combination. <br>
The configuratin file for all combination is specified by the value of $SENS_CONF_FILE in [build.sh](../README.md/build.sh) and the file is in the folder of firmware/variant/\<variant_name\>.

build.sh: <br>

```bash
    ...
    $SENS_CONF_FILE="sensor_conf"
    ...
```
<br>

sensor_conf:
<br>
```bash
USE_MULTI_SENSOR_COMBINATION=1

MULTI_COMB_OPTION_NAME=("ACC" "MAG" "ALS" "BARO" "MGR")

  ACC_OPT=("NULL"  "BMI160")
  MAG_OPT=("NULL"  "AK09918")
  ALS_OPT=("NULL"  "RPR0521")
  BARO_OPT=("NULL" "BMP280")
  MGR_OPT=("EXYNOS" "NULL")

MULTI_COMB_OPTION=(
    ${ACC_OPT[@]}
    ${MAG_OPT[@]}
    ${ALS_OPT[@]}
    ${BARO_OPT[@]}
    ${MGR_OPT[@]}
)

OTHER_OPTIONS=''
```

<br>

**USE_MULTI_SENSOR_COMBINATION** indicates if it support multiple sensor combination or not.
If the value is 0, it will generate only one os image and all other options in that file are ignored.

**MULTI_COMB_OPTION_NAME** shows that which kinds of sensors have different combinations. Above example show that Accelerometer, Magnetometer, Ambient light sensor, Barometer could have different sensors. MGR is not a physical sensor and is a kind of sensor manager that searches which sensors are used in a board. 

**MULTI_COMB_OPTION** shows the combination of each sensor.(XXX means the sesnor name described in USE_MULTI_SENSOR_COMBINATION).
The example code shows that there are two options, the first option has only MGR(the name is EXYNOS) and the second option has the specific sensor model for each physical sensor except MGR. <br>
Two os image will be generated by this option, *os_checked_0.bin*(with the first option) and *os_checked_1.bin*(with the second option).

**OTHER_OPTIONS** : any other build option could be set with this field.<BR>
The option could be set with the form of KEY=VALUE.<br>
<br>

Each combination is passed to the input parameter of makefile.<br>
The following parameters are passed to the makefile for the second option:

```
    ACC=BMI160 MAG=AK09918 ALS=RPR0521 BARO=BMP280 MGR=NULL $OTHER_OPTIONS
```
