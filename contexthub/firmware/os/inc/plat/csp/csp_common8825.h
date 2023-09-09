/*----------------------------------------------------------------------------
 * Exynos SoC  -  CSP
 *
 * Copyright (C) 2021 Samsung Electronics Co., Ltd.
 *
 * Shinkyu Park <shinkyu.park@samsung.com>
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

#ifndef __CSP_COMMON8825_H__
#define __CSP_COMMON8825_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

/*******************************************
 * CHUB Memory Map
 ******************************************/
#define CMU_BASE_ADDRESS              0x40100000     // 0x11000000
#define SYSREG_BASE_ADDRESS           0x40120000     // 0x11020000
#define WDT_BASE_ADDRESS              0x40130000     // 0x11030000
#define TIMER_BASE_ADDRESS            0x40140000     // 0x11040000
#define PWM_BASE_ADDRESS              0x40160000     // 0x11060000
#define COMB_WAKEUP_SRC_BASE_ADDRESS  0x401E0000     // 0x110E0000
#define DUMP_GPR_BASE_ADDRESS         0x401F0000     // 0x110F0000
#define USI3_CHUB_BASE_ADDRESS        0x40200000     // 0x11100000
#define I2C3_CHUB_BASE_ADDRESS        0x40210000     // 0x11110000
#define COMB_CHUB2WLBT_BASE_ADDRESS   0x40240000     // 0x11140000
#define COMB_CHUB2AP_BASE_ADDRESS     0x40250000     // 0x11150000
#define COMB_CHUB2APM_BASE_ADDRESS    0x40260000     // 0x11160000
#define USI0_CHUB_BASE_ADDRESS        0x40270000     // 0x11170000
#define USI1_CHUB_BASE_ADDRESS        0x40290000     // 0x11190000
#define I2C1_CHUB_BASE_ADDRESS        0x402A0000     // 0x111A0000
#define USI2_CHUB_BASE_ADDRESS        0x402B0000     // 0x111B0000
#define GPIO_BASE_ADDRESS             0x402D0000     // 0x111D0000
#define GPIO_CHUBEINT_BASE_ADDRESS    0x402E0000     // 0x111E0000


/*******************************************
 * CMGP Memory Map
 * 0x40C00000 ~ 0x40DFFFFF is mapped to 0x11400000 ~ 0x115FFFFF
 ******************************************/
#define CMGP_BASE_ADDRESS             0x40C00000     // 0x11400000
#define CMU_CMGP_BASE_ADDRESS         0x40C00000     // 0x11400000
#define SYSREG_CMGP_BASE_ADDRESS      0x40C20000     // 0x11420000
#define GPIO_CMGP_BASE_ADDRESS        0x40C30000     // 0x11430000
#define SYSREG_CMGP2CHUB_BASE_ADDRESS 0x40CA0000     // 0x114A0000
#define USI0_CMGP_BASE_ADDRESS        0x40D00000     // 0x11500000
#define I2C0_CMGP_BASE_ADDRESS        0x40D10000     // 0x11510000
#define USI1_CMGP_BASE_ADDRESS        0x40D20000     // 0x11520000
#define I2C1_CMGP_BASE_ADDRESS        0x40D30000     // 0x11530000
#define USI2_CMGP_BASE_ADDRESS        0x40D40000     // 0x11540000
#define I2C2_CMGP_BASE_ADDRESS        0x40D50000     // 0x11550000
#define USI3_CMGP_BASE_ADDRESS        0x40D60000     // 0x11560000
#define I2C3_CMGP_BASE_ADDRESS        0x40D70000     // 0x11570000
#define USI4_CMGP_BASE_ADDRESS        0x40D80000     // 0x11580000
#define I2C4_CMGP_BASE_ADDRESS        0x40D90000     // 0x11590000
#define I3C_CMGP_BASE_ADDRESS         0x40DA0000     // 0x115A0000

/*******************************************
 * ALIVE Memory Map
 * 0x40900000 ~ 0x40BFFFFF is mapped to 0x11800000 ~ 0x11AFFFFF
 * 0x41100000 ~ 0x411FFFFF is mapped to 0x13C00000 ~ 0x13CFFFFF
 ******************************************/
#define ALIVE_BASE_ADDRESS            0x40900000     // 0x11800000
#define MAILBOX_CP_BASE_ADDRESS       0x40A40000     // 0x11940000
#define MAILBOX_VTS_BASE_ADDRESS      0x40A50000     // 0x11950000
#define MAILBOX_GNSS_BASE_ADDRESS     0x40A90000     // 0x11990000
#define MAILBOX_WLBT_BASE_ADDRESS     0x40AE0000     // 0x119E0000
#define MAILBOX_APM_BASE_ADDRESS      0x40AF0000     // 0x119F0000
#define MAILBOX_AP_BASE_ADDRESS       0x40B10000     // 0x11A10000
#define RTC_BASE_ADDRESS              0x40BB0000     // 0x11AB0000


/*******************************************
 * USI/SPI/I2C Address Map
 ******************************************/
/* USI */
#define USI0_BASE_ADDRESS             (USI0_CHUB_BASE_ADDRESS)  // 0x40270000
#define USI1_BASE_ADDRESS             (USI1_CHUB_BASE_ADDRESS)  // 0x40290000
#define USI2_BASE_ADDRESS             (I2C1_CHUB_BASE_ADDRESS)  // 0x402A0000
#define USI3_BASE_ADDRESS             (USI2_CHUB_BASE_ADDRESS)  // 0x402B0000
#define USI4_BASE_ADDRESS             (USI3_CHUB_BASE_ADDRESS)  // 0x40200000
#define USI5_BASE_ADDRESS             (I2C3_CHUB_BASE_ADDRESS)  // 0x40210000
#define USI6_BASE_ADDRESS             (USI0_CMGP_BASE_ADDRESS)  // 0x40D00000
#define USI7_BASE_ADDRESS             (I2C0_CMGP_BASE_ADDRESS)  // 0x40D10000
#define USI8_BASE_ADDRESS             (USI1_CMGP_BASE_ADDRESS)  // 0x40D20000
#define USI9_BASE_ADDRESS             (I2C1_CMGP_BASE_ADDRESS)  // 0x40D30000
#define USI10_BASE_ADDRESS            (USI2_CMGP_BASE_ADDRESS)  // 0x40D40000
#define USI11_BASE_ADDRESS            (I2C2_CMGP_BASE_ADDRESS)  // 0x40D50000
#define USI12_BASE_ADDRESS            (USI3_CMGP_BASE_ADDRESS)  // 0x40D60000
#define USI13_BASE_ADDRESS            (I2C3_CMGP_BASE_ADDRESS)  // 0x40D70000
#define USI14_BASE_ADDRESS            (USI4_CMGP_BASE_ADDRESS)  // 0x40D80000
#define USI15_BASE_ADDRESS            (I2C4_CMGP_BASE_ADDRESS)  // 0x40D90000

/* I2C */
#define I2C0_BASE_ADDRESS             (USI0_CHUB_BASE_ADDRESS)  // 0x40270000
#define I2C1_BASE_ADDRESS             (USI1_CHUB_BASE_ADDRESS)  // 0x40290000
#define I2C2_BASE_ADDRESS             (I2C1_CHUB_BASE_ADDRESS)  // 0x402A0000
#define I2C3_BASE_ADDRESS             (USI2_CHUB_BASE_ADDRESS)  // 0x402B0000
#define I2C4_BASE_ADDRESS             (USI3_CHUB_BASE_ADDRESS)  // 0x40200000
#define I2C5_BASE_ADDRESS             (I2C3_CHUB_BASE_ADDRESS)  // 0x40210000
#define I2C6_BASE_ADDRESS             (USI0_CMGP_BASE_ADDRESS)  // 0x40D00000
#define I2C7_BASE_ADDRESS             (I2C0_CMGP_BASE_ADDRESS)  // 0x40D10000
#define I2C8_BASE_ADDRESS             (USI1_CMGP_BASE_ADDRESS)  // 0x40D20000
#define I2C9_BASE_ADDRESS             (I2C1_CMGP_BASE_ADDRESS)  // 0x40D30000
#define I2C10_BASE_ADDRESS            (USI2_CMGP_BASE_ADDRESS)  // 0x40D40000
#define I2C11_BASE_ADDRESS            (I2C2_CMGP_BASE_ADDRESS)  // 0x40D50000
#define I2C12_BASE_ADDRESS            (USI3_CMGP_BASE_ADDRESS)  // 0x40D60000
#define I2C13_BASE_ADDRESS            (I2C3_CMGP_BASE_ADDRESS)  // 0x40D70000
#define I2C14_BASE_ADDRESS            (USI4_CMGP_BASE_ADDRESS)  // 0x40D80000
#define I2C15_BASE_ADDRESS            (I2C4_CMGP_BASE_ADDRESS)  // 0x40D90000

/* SPI */
#define SPI0_BASE_ADDRESS             (USI0_CHUB_BASE_ADDRESS)  // 0x40270000
#define SPI1_BASE_ADDRESS             (USI1_CHUB_BASE_ADDRESS)  // 0x40290000
#define SPI2_BASE_ADDRESS             (USI2_CHUB_BASE_ADDRESS)  // 0x402B0000
#define SPI3_BASE_ADDRESS             (USI3_CHUB_BASE_ADDRESS)  // 0x40200000
#define SPI4_BASE_ADDRESS             (USI0_CMGP_BASE_ADDRESS)  // 0x40D00000
#define SPI5_BASE_ADDRESS             (USI1_CMGP_BASE_ADDRESS)  // 0x40D20000
#define SPI6_BASE_ADDRESS             (USI2_CMGP_BASE_ADDRESS)  // 0x40D40000
#define SPI7_BASE_ADDRESS             (USI3_CMGP_BASE_ADDRESS)  // 0x40D60000
#define SPI8_BASE_ADDRESS             (USI4_CMGP_BASE_ADDRESS)  // 0x40D80000

#define USI_CHUB0                   0
#define USI_CHUB1                   1
#define USI_I2C_CHUB1               2
#define USI_CHUB2                   3
#define USI_CHUB3                   4
#define USI_I2C_CHUB3               5
#define USI_CHUB_CHANNEL_MAX        6
#define USI_CMGP0                   USI_CHUB_CHANNEL_MAX
#define USI_I2C_CMGP0               7
#define USI_CMGP1                   8
#define USI_I2C_CMGP1               9
#define USI_CMGP2                   10
#define USI_I2C_CMGP2               11
#define USI_CMGP3                   12
#define USI_I2C_CMGP3               13
#define USI_CMGP4                   14
#define USI_I2C_CMGP4               15
#define USI_CHANNEL_MAX             16

#define I2C_CHUB0                   0   // USI_CHUB0
#define I2C_CHUB1                   1   // USI_CHUB1
#define I2C_CHUB2                   2   // I2C_CHUB1
#define I2C_CHUB3                   3   // USI_CHUB2
#define I2C_CHUB4                   4   // USI_CHUB3
#define I2C_CHUB5                   5   // I2C_CHUB3
#define I2C_CHUB_CHANNEL_MAX        6
#define I2C_CMGP0                   I2C_CHUB_CHANNEL_MAX  // USI_CMGP0
#define I2C_CMGP1                   7   // I2C_CMGP0
#define I2C_CMGP2                   8   // USI_CMGP1
#define I2C_CMGP3                   9   // I2C_CMGP1
#define I2C_CMGP4                   10  // USI_CMGP2
#define I2C_CMGP5                   11  // I2C_CMGP2
#define I2C_CMGP6                   12  // USI_CMGP3
#define I2C_CMGP7                   13  // I2C_CMGP3
#define I2C_CMGP8                   14  // USI_CMGP4
#define I2C_CMGP9                   15  // I2C_CMGP4
#define I2C_CHANNEL_MAX             16

#define SPI_CHUB0                   0   // USI_CHUB0
#define SPI_CHUB1                   1   // USI_CHUB1
#define SPI_CHUB2                   2   // USI_CHUB2
#define SPI_CHUB3                   3   // USI_CHUB3
#define SPI_CHUB_CHANNEL_MAX        4
#define SPI_CMGP0                   SPI_CHUB_CHANNEL_MAX  // USI_CMGP0
#define SPI_CMGP1                   5   // USI_CMGP1
#define SPI_CMGP2                   6   // USI_CMGP2
#define SPI_CMGP3                   7   // USI_CMGP3
#define SPI_CMGP4                   8   // USI_CMGP4
#define SPI_CHANNEL_MAX             9

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK	        0x07000000

#define OSC_CLOCK                   (24576000)

#define RTC_INTERRUPT_NO            RTC_CHUB_IRQn
#define SUPPORT_PWM_TIMER4          1
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)
#define CMGP_INTERRUPT_NUN          42

typedef enum IRQn
{
    /* -------------------  Cortex    Processor Exceptions Numbers  ------------------ */
    NonMaskableInt_IRQn         = -14,  /*!<  2 Non Maskable Interrupt          */
    HardFault_IRQn              = -13,  /*!<  3 HardFault Interrupt             */
    MemoryManagement_IRQn       = -12,  /*!<  4 Memory Management Interrupt     */
    BusFault_IRQn               = -11,  /*!<  5 Bus Fault Interrupt             */
    UsageFault_IRQn             = -10,  /*!<  6 Usage Fault Interrupt           */
    SVCall_IRQn                 = -5,   /*!< 11 SV Call Interrupt               */
    DebugMonitor_IRQn           = -4,   /*!< 12 Debug Monitor Interrupt         */
    PendSV_IRQn                 = -2,   /*!< 14 Pend SV Interrupt               */
    SysTick_IRQn                = -1,   /*!< 15 System Tick Interrupt           */

    TIMER_CHUBIRQn              = 0,
    WDT_CHUB_IRQn               = 1,
    CSIS0_VVALID_IRQn           = 2,
    CSIS1_VVALID_IRQn           = 3,
    CSIS2_VVALID_IRQn           = 4,
    CSIS3_VVALID_IRQn           = 5,
    CSIS4_VVALID_IRQn           = 6,
    CSIS5_VVALID_IRQn           = 7,
    MB_WLBT_IRQn                = 8,
    MB_AP_IRQn                  = 9,
    MB_CP_IRQn                  = 10,
    MB_APM_IRQn                 = 11,
    MB_GNSS_IRQn                = 12,
    MB_VTS_IRQn                 = 13,
    SYSREG0_CMGP_IRQn           = 14,
    SYSREG1_CMGP_IRQn           = 15,
    RTC_CHUB_IRQn               = 16,
    USI_CHUB0_IRQn              = 17,

    USI_CHUB1_IRQn              = 19,
    I2C_CHUB1_IRQn              = 20,
    USI_CHUB2_IRQn              = 21,
    
    USI_CHUB3_IRQn              = 23,
    I2C_CHUB3_IRQn              = 24,
    CMGP_REQ_IRQn               = 25,
    MIF_REQ_IRQn                = 26,
    PWM0_CHUB_IRQn              = 27,
    PWM1_CHUB_IRQn              = 28,
    PWM2_CHUB_IRQn              = 29,
    PWM3_CHUB_IRQn              = 30,
    PWM4_CHUB_IRQn              = 31,
    EXTINT_GPH00_0_IRQn          = 32,
    EXTINT_GPH00_1_IRQn          = 33,
    EXTINT_GPH00_2_IRQn          = 34,
    EXTINT_GPH00_3_IRQn          = 35,
    EXTINT_GPH00_4_IRQn          = 36,
    EXTINT_GPH00_5_IRQn          = 37,
    EXTINT_GPH00_6_IRQn          = 38,
    EXTINT_GPH00_7_IRQn          = 39,
    EXTINT_GPH01_0_IRQn          = 40,
    EXTINT_GPH01_1_IRQn          = 41,
    EXTINT_GPH01_2_IRQn          = 42,
    EXTINT_GPH01_3_IRQn          = 43,
    INTREQ_EXT_INTE0_IRQn       = 44,
    INTREQ_EXT_INTE1_IRQn       = 45,
    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif
