/*----------------------------------------------------------------------------
 * Exynos SoC  -  CSP
 *
 * Copyright (C) 2020 Samsung Electronics Co., Ltd.
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

#ifndef __CSP_COMMON9110_H__
#define __CSP_COMMON9110_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

// Memory Map on CHUB Block
#define CMU_BASE_ADDRESS                    0x40100000  // 0x10C00000
#define TZPC_BASE_ADDRESS                   0x40110000
#define BAAW_C_BASE_ADDRESS                 0x40120000
#define BAAW_D_BASE_ADDRESS                 0x40130000
#define SYSREG_BASE_ADDRESS                 0x40140000
#define PPMU_BASE_ADDRESS                   0x40150000
#define TIMER_BASE_ADDRESS                  0x40160000
#define WDT_BASE_ADDRESS                    0x40170000
#define PDMA_BASE_ADDRESS                   0x40180000
#define PWM_BASE_ADDRESS                    0x40190000
#define USI0_CHUB_BASE_ADDRESS              0x401A0000
#define SPI0_CHUB_BASE_ADDRESS              USI0_CHUB_BASE_ADDRESS
#define I2C0_CHUB_BASE_ADDRESS              USI0_CHUB_BASE_ADDRESS
#define I2C1_CHUB_BASE_ADDRESS              0x401B0000
#define I2C2_CHUB_BASE_ADDRESS              0x401C0000
#define DUMP_GPR0_BASE_ADDRESS              0x401D0000  // C-M4
#define DUMP_GPR1_BASE_ADDRESS              0x401F0000  // C-M55

// Memory Map on CMGP Block
#define MAILBOX_CP_BASE_ADDRESS             0x40340000  // 0x12940000
#define MAILBOX_GNSS_BASE_ADDRESS           0x403A0000
#define MAILBOX_WLBT0_BASE_ADDRESS          0x40400000
#define MAILBOX_WLBT1_BASE_ADDRESS          0x40410000
#define MAILBOX_APM_BASE_ADDRESS            0x40420000
#define MAILBOX_AP_BASE_ADDRESS             0x40430000

#define RTC_BASE_ADDRESS                    0x40470000

#define CMU_CMGP_BASE_ADDRESS               0x40500000

#define SYSREG_CMGP_BASE_ADDRESS            0x40520000
#define GPIO_CMGP_BASE_ADDRESS              0x40530000
#define ADC_CMGP_BASE_ADDRESS               0x40540000

#define SYSREG_CMGP2CHUB_BASE_ADDRESS       0x40590000

#define SYSREG_CMGP2PMU_CHUB_BASE_ADDRESS   0x405B0000

#define USI0_CMGP_BASE_ADDRESS              0x405C0000
#define I2C0_CMGP_BASE_ADDRESS              USI0_CMGP_BASE_ADDRESS
#define SPI0_CMGP_BASE_ADDRESS              USI0_CMGP_BASE_ADDRESS
#define UART0_CMGP_BASE_ADDRESS             USI0_CMGP_BASE_ADDRESS
#define I2C1_CMGP_BASE_ADDRESS              0x405D0000

#define USI1_CMGP_BASE_ADDRESS              0x405E0000
#define I2C2_CMGP_BASE_ADDRESS              USI1_CMGP_BASE_ADDRESS
#define SPI1_CMGP_BASE_ADDRESS              USI1_CMGP_BASE_ADDRESS
#define UART1_CMGP_BASE_ADDRESS             USI1_CMGP_BASE_ADDRESS
#define I2C3_CMGP_BASE_ADDRESS              0x405F0000

#define USI2_CMGP_BASE_ADDRESS              0x40600000
#define I2C4_CMGP_BASE_ADDRESS              USI2_CMGP_BASE_ADDRESS
#define SPI2_CMGP_BASE_ADDRESS              USI2_CMGP_BASE_ADDRESS
#define UART2_CMGP_BASE_ADDRESS             USI2_CMGP_BASE_ADDRESS
#define I2C5_CMGP_BASE_ADDRESS              0x40610000

#define USI3_CMGP_BASE_ADDRESS              0x40620000
#define I2C6_CMGP_BASE_ADDRESS              USI3_CMGP_BASE_ADDRESS
#define SPI3_CMGP_BASE_ADDRESS              USI3_CMGP_BASE_ADDRESS
#define UART3_CMGP_BASE_ADDRESS             USI3_CMGP_BASE_ADDRESS
#define I2C7_CMGP_BASE_ADDRESS              0x40630000

#define I2C8_CMGP_BASE_ADDRESS              0x40640000
#define I2C9_CMGP_BASE_ADDRESS              0x40650000
#define I2C10_CMGP_BASE_ADDRESS             0x40660000
#define TIMER_CMGP_BASE_ADDRESS             0x40670000

#define I2C0_BASE_ADDRESS                   I2C0_CHUB_BASE_ADDRESS
#define I2C1_BASE_ADDRESS                   I2C1_CHUB_BASE_ADDRESS
#define I2C2_BASE_ADDRESS                   I2C2_CHUB_BASE_ADDRESS
#define I2C3_BASE_ADDRESS                   I2C0_CMGP_BASE_ADDRESS
#define I2C4_BASE_ADDRESS                   I2C1_CMGP_BASE_ADDRESS
#define I2C5_BASE_ADDRESS                   I2C2_CMGP_BASE_ADDRESS
#define I2C6_BASE_ADDRESS                   I2C3_CMGP_BASE_ADDRESS
#define I2C7_BASE_ADDRESS                   I2C4_CMGP_BASE_ADDRESS
#define I2C8_BASE_ADDRESS                   I2C5_CMGP_BASE_ADDRESS
#define I2C9_BASE_ADDRESS                   I2C6_CMGP_BASE_ADDRESS
#define I2C10_BASE_ADDRESS                  I2C7_CMGP_BASE_ADDRESS
#define I2C11_BASE_ADDRESS                  I2C8_CMGP_BASE_ADDRESS
#define I2C12_BASE_ADDRESS                  I2C9_CMGP_BASE_ADDRESS
#define I2C13_BASE_ADDRESS                  I2C10_CMGP_BASE_ADDRESS

#define SPI0_BASE_ADDRESS                   SPI0_CHUB_BASE_ADDRESS
#define SPI1_BASE_ADDRESS                   SPI1_CMGP_BASE_ADDRESS
#define SPI2_BASE_ADDRESS                   SPI2_CMGP_BASE_ADDRESS
#define SPI3_BASE_ADDRESS                   SPI3_CMGP_BASE_ADDRESS

#define UART0_BASE_ADDRESS                  UART1_CMGP_BASE_ADDRESS
#define UART1_BASE_ADDRESS                  UART2_CMGP_BASE_ADDRESS
#define UART2_BASE_ADDRESS                  UART3_CMGP_BASE_ADDRESS

#define USI_CHUB0                   0
#define USI_I2C_CHUB0               1
#define USI_I2C_CHUB1               2
#define USI_CHUB_CHANNEL_MAX        3
#define USI_CMGP0                   USI_CHUB_CHANNEL_MAX
#define USI_I2C_CMGP0               4
#define USI_CMGP1                   5
#define USI_I2C_CMGP1               6
#define USI_CMGP2                   7
#define USI_I2C_CMGP2               8
#define USI_CMGP3                   9
#define USI_I2C_CMGP3               10
#define USI_I2C_CMGP4               11
#define USI_I2C_CMGP5               12
#define USI_I2C_CMGP6               13
#define USI_CHANNEL_MAX             14

#define I2C_CHUB0                   0
#define I2C_CHUB1                   1
#define I2C_CHUB2                   2
#define I2C_CHUB_CHANNEL_MAX        3
#define I2C_CMGP0                   I2C_CHUB_CHANNEL_MAX
#define I2C_CMGP1                   4
#define I2C_CMGP2                   5
#define I2C_CMGP3                   6
#define I2C_CMGP4                   7
#define I2C_CMGP5                   8
#define I2C_CMGP6                   9
#define I2C_CMGP7                   10
#define I2C_CMGP8                   11
#define I2C_CMGP9                   12
#define I2C_CMGP10                  13
#define I2C_CHANNEL_MAX             14

#define SPI_CHUB0                   0
#define SPI_CHUB_CHANNEL_MAX        1
#define SPI_CMGP0                   SPI_CHUB_CHANNEL_MAX
#define SPI_CMGP1                   2
#define SPI_CMGP2                   3
#define SPI_CMGP3                   4
#define SPI_CHANNEL_MAX             5

#define UART_CHUB0                  0
#define UART_CHUB_CHANNEL_MAX       1
#define UART_CMGP0                  UART_CHUB_CHANNEL_MAX
#define UART_CMGP1                  2
#define UART_CMGP2                  3
#define UART_CMGP3                  4
#define UART_CHANNEL_MAX            6

#define UART_CHANNEL_DEBUG          USI_CMGP2

#define ADC_CHANNEL_MAX             7

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK	        0x07000000

#define OSC_CLOCK                   (24576000)

#define RTC_INTERRUPT_NO            RTC_CHUB_IRQn
#define SUPPORT_PWM_TIMER4          0
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)
#define CMGP_INTERRUPT_NUN          24

typedef enum IRQn
{
    /* -------------------  Cortex    Processor Exceptions Numbers  ------------------ */
    NonMaskableInt_IRQn          = -14,      /*!<  2 Non Maskable Interrupt          */
    HardFault_IRQn               = -13,      /*!<  3 HardFault Interrupt             */
    MemoryManagement_IRQn        = -12,      /*!<  4 Memory Management Interrupt     */
    BusFault_IRQn                = -11,      /*!<  5 Bus Fault Interrupt             */
    UsageFault_IRQn              = -10,      /*!<  6 Usage Fault Interrupt           */
    SVCall_IRQn                  =  -5,      /*!< 11 SV Call Interrupt               */
    DebugMonitor_IRQn            =  -4,      /*!< 12 Debug Monitor Interrupt         */
    PendSV_IRQn                  =  -2,      /*!< 14 Pend SV Interrupt               */
    SysTick_IRQn                 =  -1,      /*!< 15 System Tick Interrupt           */

    PDMA_CHUB_IRQn               = 0,
    DPU_DPP_VG_IRQn              = 1,
    WDT_CHUB_IRQn                = 2,
    MB_WLBT_IRQn                 = 3,
    PWM0_CHUB_IRQn               = 4,
    PWM1_CHUB_IRQn               = 5,
    PWM2_CHUB_IRQn               = 6,
    PWM3_CHUB_IRQn               = 7,
    DPU_DSIM0_IRQn               = 8,
    MB_AP_IRQn                   = 9,
    MB_CP_IRQn                   = 10,
    MB_APM_IRQn                  = 11,
    MB_GNSS_IRQn                 = 12,
    DPU_FRAME_START_IRQn         = 13,
    SYSREG1_CMGP_IRQn            = 14,
    CM55_IRQn                    = 15,
    MIF_REQ_ACK_IRQn             = 16,
    ADC_CMGP_IRQn                = 17,
    DPU_DMA_VG_IRQn              = 18,
    DPU_DMA_G0_IRQn              = 19,
    USI_CMGP2_IRQn               = 20,
    USI_CMGP3_IRQn               = 21,
    GPIO_PERI_IRQn               = 22,
    I2C_CMGP1_IRQn               = 23,
    USI_CHUB0_IRQn               = 24,
    I2C_CHUB0_IRQn               = 25,
    I2C_CHUB1_IRQn               = 26,
    I2C_CMGP2_IRQn               = 27,
    I2C_CMGP3_IRQn               = 28,
    I2C_CMGP4_IRQn               = 29,
    I2C_CMGP5_IRQn               = 30,
    I2C_CMGP6_IRQn               = 31,

    GPM00_IRQn                   = 32,
    GPM01_IRQn                   = 33,
    GPM02_IRQn                   = 34,
    GPM03_IRQn                   = 35,
    GPM04_IRQn                   = 36,
    GPM05_IRQn                   = 37,
    GPM06_IRQn                   = 38,
    GPM07_IRQn                   = 39,
    GPM08_IRQn                   = 40,
    GPM09_IRQn                   = 41,
    GPM10_IRQn                   = 42,
    GPM11_IRQn                   = 43,
    GPM12_IRQn                   = 44,
    GPM13_IRQn                   = 45,
    GPM14_IRQn                   = 46,
    GPM15_IRQn                   = 47,
    GPM16_IRQn                   = 48,
    GPM17_IRQn                   = 49,
    GPM18_IRQn                   = 50,
    GPM19_IRQn                   = 51,
    GPM20_IRQn                   = 52,
    GPM21_IRQn                   = 53,
    GPM22_IRQn                   = 54,
    GPM23_IRQn                   = 55,
    GPM24_IRQn                   = 56,
    GPM25_IRQn                   = 57,

    RTC_CHUB_IRQn                = 58,
    DPU_FRAME_DONE_IRQn          = 59,
    CMGP_REQ_ACK_IRQn            = 60,
    WDT1_CHUB_IRQn               = 61,
    NUM_INTERRUPTS               = 62
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1,
    RtcTickIdMax,
};

#endif

