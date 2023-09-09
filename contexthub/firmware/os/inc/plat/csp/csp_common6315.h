/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common6315.h
 *      Purpose: To define common definitions for S5E6315
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

#ifndef __CSP_COMMON6315_H__
#define __CSP_COMMON6315_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CMU_BASE_ADDRESS                0x40000000 //0x1480_0000
#define SYSREG_BASE_ADDRESS             0x40020000 //0x1482_0000
#define WDT_BASE_ADDRESS                0x40030000 //0x1483_0000
#define TIMER_BASE_ADDRESS              0x40040000 //0x1484_0000
#define PWM_BASE_ADDRESS                0x40060000 //0x1486_0000
#define RTC_BASE_ADDRESS                0x40070000 //0x1487_0000
#define COMB_WAKEUP_SRC_BASE_ADDRESS    0x400E0000 //0x148E_0000
#define DUMP_GPR_CHUB_BASE_ADDRESS      0x400F0000 //0x148F_0000

#define SYSREG_CHUB2APM_BASE_ADDRESS    0x40100000 //0x14A0_0000
/* USI_CHUB0_USI */
#define USI0_CHUB_BASE_ADDRESS          0x40150000 //0x14A5_0000
/* USI_CHUB1_USI */
#define USI1_CHUB_BASE_ADDRESS          0x40160000 //0x14A6_0000
/* USI_CHUB2_USI */
#define USI2_CHUB_BASE_ADDRESS          0x40170000 //0x14A7_0000
/* SPI_I2C_CHUB1_USI */
#define USI3_CHUB_BASE_ADDRESS          0x40180000 //0x14A8_0000
/* I2C_CHUB0_USI */
#define USI4_CHUB_BASE_ADDRESS          0x40190000 //0x14A9_0000
/* I3C_CHUB0 */
#define I3C0_CHUB_BASE_ADDRESS          0x401A0000 //0x14AA_0000
/* SPI_I2C_CHUB0_USI */
#define USI5_CHUB_BASE_ADDRESS          0x401B0000 //0x14AB_0000
/* USI_CHUB3_USI */
#define USI6_CHUB_BASE_ADDRESS          0x401C0000 //0x14AC_0000
/* GPIO_CHUB */
#define GPIO_BASE_ADDRESS               0x401D0000 //0x14AD_0000
/* I2C_CHUB1_USI */
#define USI7_CHUB_BASE_ADDRESS          0x401E0000 //0x14AE_0000
/* I3C_CHUB1 */
#define I3C1_CHUB_BASE_ADDRESS          0x401F0000 //0x14AF_0000

#define ALIVE_BASE                      0x40200000 //0x15800000

#define CORE_BASE                       0x40400000 //0x14E00000
#define OTP_CON_TOP_BASE_ADDRESS        (CORE_BASE)
#define ADC1_BASE_ADDRESS               (CORE_BASE + 0x10000)
#define ADC2_BASE_ADDRESS               (CORE_BASE + 0x20000)
#define USI0_GNSS_BASE_ADDRESS          (CORE_BASE + 0x30000)
#define USI1_GNSS_BASE_ADDRESS          (CORE_BASE + 0x40000)
#define USI2_GNSS_BASE_ADDRESS          (CORE_BASE + 0x50000)
#define PDMA_BASE_ADDRESS               (CORE_BASE + 0x60000)
#define CMU_CORE_BASE_ADDRESS           (CORE_BASE + 0x70000)
#define MAILBOX_GNSS_BASE_ADDRESS       (CORE_BASE + 0x80000)
#define MAILBOX_APM_BASE_ADDRESS        (CORE_BASE + 0X90000)
#define MAILBOX_SHARED_SRAM             (CORE_BASE + 0xB0000)
#define SYSREG_CORE_BASE_ADDRESS        (CORE_BASE + 0xC0000)
#define GPIO_CORE_BASE_ADDRESS          (CORE_BASE + 0xD0000)

#define GNSS_BASE                       0x40500000 //0x14600000
#define SRAM_GNSS_BASE                  0x40800000 //0x01800000
#define SRAM_ALIVE_BASE                 0x40900000 //0x02060000


#define I2C0_BASE_ADDRESS               USI0_CHUB_BASE_ADDRESS
#define I2C1_BASE_ADDRESS               USI1_CHUB_BASE_ADDRESS
#define I2C2_BASE_ADDRESS               USI2_CHUB_BASE_ADDRESS
#define I2C3_BASE_ADDRESS               USI3_CHUB_BASE_ADDRESS
#define I2C4_BASE_ADDRESS               USI4_CHUB_BASE_ADDRESS
#define I2C5_BASE_ADDRESS               USI5_CHUB_BASE_ADDRESS
#define I2C6_BASE_ADDRESS               USI6_CHUB_BASE_ADDRESS
#define I2C7_BASE_ADDRESS               USI7_CHUB_BASE_ADDRESS

#define SPI0_BASE_ADDRESS               USI0_CHUB_BASE_ADDRESS
#define SPI1_BASE_ADDRESS               USI1_CHUB_BASE_ADDRESS
#define SPI2_BASE_ADDRESS               USI2_CHUB_BASE_ADDRESS
#define SPI3_BASE_ADDRESS               USI3_CHUB_BASE_ADDRESS
#define SPI4_BASE_ADDRESS               USI5_CHUB_BASE_ADDRESS
#define SPI5_BASE_ADDRESS               USI6_CHUB_BASE_ADDRESS

#define UART0_BASE_ADDRESS              USI0_CHUB_BASE_ADDRESS
#define UART1_BASE_ADDRESS              USI1_CHUB_BASE_ADDRESS
#define UART2_BASE_ADDRESS              USI2_CHUB_BASE_ADDRESS
#define UART3_BASE_ADDRESS              USI6_CHUB_BASE_ADDRESS

#define I3C0_BASE_ADDRESS               I3C0_CHUB_BASE_ADDRESS
#define I3C1_BASE_ADDRESS               I3C1_CHUB_BASE_ADDRESS

#define USI_CHUB0                       0
#define USI_CHUB1                       1
#define USI_CHUB2                       2
#define USI_CHUB3                       3
#define USI_CHUB4                       4
#define USI_CHUB5                       5
#define USI_CHUB6                       6
#define USI_CHUB7                       7
#define USI_CHUB_CHANNEL_MAX            8
#define USI_CHANNEL_MAX                 USI_CHUB_CHANNEL_MAX

#define I2C_CHUB0                       0
#define I2C_CHUB1                       1
#define I2C_CHUB2                       2
#define I2C_CHUB3                       3
#define I2C_CHUB4                       4
#define I2C_CHUB5                       5
#define I2C_CHUB6                       6
#define I2C_CHUB7                       7
#define I2C_CHUB_CHANNEL_MAX            8
#define I2C_CHANNEL_MAX                 I2C_CHUB_CHANNEL_MAX

#define SPI_CHUB0                       0
#define SPI_CHUB1                       1
#define SPI_CHUB2                       2
#define SPI_CHUB3                       3
#define SPI_CHUB4                       4
#define SPI_CHUB5                       5
#define SPI_CHUB_CHANNEL_MAX            6
#define SPI_CHANNEL_MAX                 SPI_CHUB_CHANNEL_MAX

#define UART_CHUB0                      0
#define UART_CHUB1                      1
#define UART_CHUB2                      2
#define UART_CHUB3                      3
#define UART_CHUB_CHANNEL_MAX           4
#define UART_CHANNEL_MAX                UART_CHUB_CHANNEL_MAX

#define UART_CHANNEL_DEBUG              UART_CHUB0

#define ADC_CHANNEL_MAX                 2

#define GPIO_CHANNEL_MAX                30

#define RTC_SUPPORT_TICK0               1
#define RTC_TIMESTAMP_TICK              1

#define I2C_INTERRUPT_MASK              0x00000021

#define OSC_CLOCK                       24576000

#define RTC_INTERRUPT_NO                RTC_IRQn
#define SUPPORT_PWM_TIMER4              1
#define USE_PWM_AS_TIMESTAMP            0
#define TS_PWM_ID                       PWM_TIMER4
#define MAILBOX_VERSION                 (0x1)
#define MAILBOX_ABOX_SUPPORT            0
#define MAILBOX_NPU_SUPPORT             0
#define MAILBOX_DSP_SUPPORT             0
#define CMGP_INTERRUPT_NUN              0


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

    TIMER_CHUB_IRQn             = 0,
    WDT_CHUB_IRQn               = 1,

    MB_GNSS_IRQn                = 8,
    MB_APM_IRQn                 = 9,
    MB_CP_IRQn                  = 10,
    SPI_CHUB_IRQn               = 11,
    SPI_GNSS_IRQn               = 12,
    UART_CP_IRQn                = 13,

    RTC_IRQn                    = 15,
    I2C_CHUB01_IRQn             = 16,
    I3C_CHUB01_IRQn             = 17,
    USI_CHUB00_IRQn             = 18,
    USI_CHUB01_IRQn             = 19,
    USI_CHUB02_IRQn             = 20,
    USI_CHUB03_IRQn             = 21,
    I2C_CHUB00_IRQn             = 22,
    I3C_CHUB00_IRQn             = 23,
    CMGP_REQ_ACK_IRQn           = 24,

    PWM0_CHUB_IRQn              = 26,
    PWM1_CHUB_IRQn              = 27,
    PWM2_CHUB_IRQn              = 28,
    PWM3_CHUB_IRQn              = 29,
    PWM4_CHUB_IRQn              = 30,
    EXTINT_GPH0_0               = 31,
    EXTINT_GPH0_1               = 32,
    EXTINT_GPH0_2               = 33,
    EXTINT_GPH0_3               = 34,
    EXTINT_GPH0_4               = 35,
    EXTINT_GPH0_5               = 36,
    EXTINT_GPH0_6               = 37,
    EXTINT_GPH0_7               = 38,
    EXTINT_GPH1_0               = 39,
    EXTINT_GPH1_1               = 40,
    EXTINT_GPH1_2               = 41,
    EXTINT_GPH1_3               = 42,
    EXTINT_GPH1_4               = 43,
    EXTINT_GPH1_5               = 44,
    EXTINT_GPH1_6               = 45,
    EXTINT_GPH1_7               = 46,
    EXTINT_GPH2_0               = 47,
    EXTINT_GPH2_1               = 48,
    EXTINT_GPH2_2               = 49,
    EXTINT_GPH2_3               = 50,
    EXTINT_GPH3_0               = 51,
    EXTINT_GPH3_1               = 52,
    EXTINT_GPH3_2               = 53,
    EXTINT_GPH3_3               = 54,
    EXTINT_GPH3_4               = 55,
    EXTINT_GPH3_5               = 56,
    EXTINT_GPH3_6               = 57,
    EXTINT_GPH3_7               = 58,
    EXTINT_GPH4_0               = 59,
    EXTINT_GPH4_1               = 60,
    
    SPI_I2C_CHUB00_IRQn         = 67,
    SPI_I2C_CHUB01_IRQn         = 68,

    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif

