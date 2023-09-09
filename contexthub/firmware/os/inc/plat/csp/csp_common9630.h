/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common9630.h
 *      Purpose: To define common definitions for s5e9630
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

#ifndef __CSP_COMMON9630_H__
#define __CSP_COMMON9630_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CMU_BASE_ADDRESS            0x40100000
#define SYSREG_BASE_ADDRESS         0x40120000
#define WDT_BASE_ADDRESS            0x40130000
#define TIMER_BASE_ADDRESS          0x40140000
#define PDMA_BASE_ADDRESS           0x40150000
#define PWM_BASE_ADDRESS            0x40160000
#define USI0_CHUB_BASE_ADDRESS      0x40970000
#define I2C0_CHUB_BASE_ADDRESS      (USI0_CHUB_BASE_ADDRESS)
#define SPI0_CHUB_BASE_ADDRESS      (USI0_CHUB_BASE_ADDRESS)
#define UART0_CHUB_BASE_ADDRESS     (USI0_CHUB_BASE_ADDRESS)
#define I2C1_CHUB_BASE_ADDRESS      0x40980000
#define USI1_CHUB_BASE_ADDRESS      0x40990000
#define I2C2_CHUB_BASE_ADDRESS      (USI1_CHUB_BASE_ADDRESS)
#define SPI1_CHUB_BASE_ADDRESS      (USI1_CHUB_BASE_ADDRESS)
#define UART1_CHUB_BASE_ADDRESS     (USI1_CHUB_BASE_ADDRESS)
#define I2C3_CHUB_BASE_ADDRESS      0x409A0000
#define USI2_CHUB_BASE_ADDRESS      0x409B0000
#define I2C4_CHUB_BASE_ADDRESS      (USI2_CHUB_BASE_ADDRESS)
#define SPI2_CHUB_BASE_ADDRESS      (USI2_CHUB_BASE_ADDRESS)
#define UART2_CHUB_BASE_ADDRESS     (USI2_CHUB_BASE_ADDRESS)
#define I2C5_CHUB_BASE_ADDRESS      0x409C0000
#define GPIO_BASE_ADDRESS           0x409D0000

#define APM_BASE    0x40300000
#define MAILBOX_CP_BASE_ADDRESS     (APM_BASE + 0x040000)
#define MAILBOX_GNSS_BASE_ADDRESS   (APM_BASE + 0x090000)
#define MAILBOX_WLBT_BASE_ADDRESS   (APM_BASE + 0x0E0000)
#define MAILBOX_APM_BASE_ADDRESS    (APM_BASE + 0x0F0000)
#define MAILBOX_VTS_BASE_ADDRESS    (APM_BASE + 0x100000)
#define MAILBOX_AP_BASE_ADDRESS     (APM_BASE + 0x120000)
#define RTC_APM_BASE_ADDRESS        (APM_BASE + 0x1A0000)

#define RTC_BASE_ADDRESS            (RTC_APM_BASE_ADDRESS)

#define CMGP_BASE   0x40500000
#define CMU_CMGP_BASE_ADDRESS       (CMGP_BASE)
#define SYSREG_CMGP_BASE_ADDRESS    (CMGP_BASE + 0x20000)
#define GPIO_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x30000)
#define ADC0_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x40000)
#define ADC1_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x50000)
#define SYSREG_CMGP2CHUB_BASE_ADDRESS       (CMGP_BASE + 0xA0000)

#define USI0_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x100000)
#define I2C0_CMGP_BASE_ADDRESS      USI0_CMGP_BASE_ADDRESS
#define SPI0_CMGP_BASE_ADDRESS      USI0_CMGP_BASE_ADDRESS
#define UART0_CMGP_BASE_ADDRESS     USI0_CMGP_BASE_ADDRESS
#define I2C1_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x110000)

#define USI1_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x120000)
#define I2C2_CMGP_BASE_ADDRESS      USI1_CMGP_BASE_ADDRESS
#define SPI1_CMGP_BASE_ADDRESS      USI1_CMGP_BASE_ADDRESS
#define UART1_CMGP_BASE_ADDRESS     USI1_CMGP_BASE_ADDRESS
#define I2C3_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x130000)

#define USI2_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x140000)
#define I2C4_CMGP_BASE_ADDRESS      USI2_CMGP_BASE_ADDRESS
#define SPI2_CMGP_BASE_ADDRESS      USI2_CMGP_BASE_ADDRESS
#define UART2_CMGP_BASE_ADDRESS     USI2_CMGP_BASE_ADDRESS
#define I2C5_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x150000)

#define USI3_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x160000)
#define I2C6_CMGP_BASE_ADDRESS      USI3_CMGP_BASE_ADDRESS
#define SPI3_CMGP_BASE_ADDRESS      USI3_CMGP_BASE_ADDRESS
#define UART3_CMGP_BASE_ADDRESS     USI3_CMGP_BASE_ADDRESS
#define I2C7_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x170000)

#define I2C0_BASE_ADDRESS           I2C0_CHUB_BASE_ADDRESS
#define I2C1_BASE_ADDRESS           I2C1_CHUB_BASE_ADDRESS
#define I2C2_BASE_ADDRESS           I2C2_CHUB_BASE_ADDRESS
#define I2C3_BASE_ADDRESS           I2C3_CHUB_BASE_ADDRESS
#define I2C4_BASE_ADDRESS           I2C4_CHUB_BASE_ADDRESS
#define I2C5_BASE_ADDRESS           I2C5_CHUB_BASE_ADDRESS
#define I2C6_BASE_ADDRESS           I2C0_CMGP_BASE_ADDRESS
#define I2C7_BASE_ADDRESS           I2C1_CMGP_BASE_ADDRESS
#define I2C8_BASE_ADDRESS           I2C2_CMGP_BASE_ADDRESS
#define I2C9_BASE_ADDRESS           I2C3_CMGP_BASE_ADDRESS
#define I2C10_BASE_ADDRESS          I2C4_CMGP_BASE_ADDRESS
#define I2C11_BASE_ADDRESS          I2C5_CMGP_BASE_ADDRESS
#define I2C12_BASE_ADDRESS          I2C6_CMGP_BASE_ADDRESS
#define I2C13_BASE_ADDRESS          I2C7_CMGP_BASE_ADDRESS

#define SPI0_BASE_ADDRESS           SPI0_CHUB_BASE_ADDRESS
#define SPI1_BASE_ADDRESS           SPI1_CHUB_BASE_ADDRESS
#define SPI2_BASE_ADDRESS           SPI2_CHUB_BASE_ADDRESS
#define SPI3_BASE_ADDRESS           SPI0_CMGP_BASE_ADDRESS
#define SPI4_BASE_ADDRESS           SPI1_CMGP_BASE_ADDRESS
#define SPI5_BASE_ADDRESS           SPI2_CMGP_BASE_ADDRESS

#define UART0_BASE_ADDRESS          UART0_CHUB_BASE_ADDRESS
#define UART1_BASE_ADDRESS          UART1_CHUB_BASE_ADDRESS
#define UART2_BASE_ADDRESS          UART2_CHUB_BASE_ADDRESS
#define UART3_BASE_ADDRESS          UART0_CMGP_BASE_ADDRESS
#define UART4_BASE_ADDRESS          UART1_CMGP_BASE_ADDRESS
#define UART5_BASE_ADDRESS          UART2_CMGP_BASE_ADDRESS

#define USI_CHUB0                   0
#define USI_I2C_CHUB0               1
#define USI_CHUB1                   2
#define USI_I2C_CHUB1               3
#define USI_CHUB2                   4
#define USI_I2C_CHUB2               5
#define USI_CHUB_CHANNEL_MAX        6
#define USI_CMGP0                   6
#define USI_I2C_CMGP0               7
#define USI_CMGP1                   8
#define USI_I2C_CMGP1               9
#define USI_CMGP2                   10
#define USI_I2C_CMGP2               11
#define USI_CMGP3                   12
#define USI_I2C_CMGP3               13
#define USI_CHANNEL_MAX             14

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

#define SPI_CHUB0                   0
#define SPI_CHUB1                   1
#define SPI_CHUB2                   2
#define SPI_CHUB_CHANNEL_MAX        3
#define SPI_CMGP0                   3
#define SPI_CMGP1                   4
#define SPI_CMGP2                   5
#define SPI_CMGP3                   6
#define SPI_CHANNEL_MAX             7

#define UART_CHUG0                  0
#define UART_CHUG1                  1
#define UART_CHUG2                  2
#define UART_CHUB_CHANNEL_MAX       3
#define UART_CMGP0                  3
#define UART_CMGP1                  4
#define UART_CMGP2                  5
#define UART_CHANNEL_MAX            6

#define UART_CHANNEL_DEBUG          USI_CMGP3

#define ADC_CHANNEL_MAX             0

#define GPIO_CHANNEL_MAX            33

#define RTC_SUPPORT_TICK0	          1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK	        0x00000021

#define OSC_CLOCK                   24576000

#define RTC_INTERRUPT_NO            RTC_IRQn
#define SUPPORT_PWM_TIMER4          0
#define USE_PWM_AS_TIMESTAMP        1
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)

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

    PDMA_CHUB00_IRQn            = 0,
    TIMER_CHUB_IRQn             = 1,
    WDT_CHUB_IRQn               = 2,
    CSIS0_VVALID_IRQn           = 3,
    CSIS1_VVALID_IRQn           = 4,
    CSIS2_VVALID_IRQn           = 5,
    CSIS3_VVALID_IRQn           = 6,
    CSIS4_VVALID_IRQn           = 7,
    MB_WLBT_IRQn                = 8,
    MB_AP_IRQn                  = 9,
    MB_CP_IRQn                  = 10,
    MB_APM_IRQn                 = 11,
    MB_GNSS_IRQn                = 12,
    SYSREG0_CMGP_IRQn           = 13,
    SYSREG1_CMGP_IRQn           = 14,
    MB_VTS_IRQn                 = 15,
    USI_CHUB00_IRQn             = 16,
    I2C_CHUB00_IRQn             = 17,
    USI_CHUB01_IRQn             = 18,
    I2C_CHUB01_IRQn             = 19,
    USI_CHUB02_IRQn             = 20,
    I2C_CHUB02_IRQn             = 21,
    CMGP_REQ_IRQn               = 22,
    MIF_REQ_IRQn                = 23,
    PWM0_CHUB_IRQn              = 24,
    PWM1_CHUB_IRQn              = 25,
    PWM2_CHUB_IRQn              = 26,
    PWM3_CHUB_IRQn              = 27,
    RTC_IRQn                    = 28,
    CHUB_RTC_TICK0_IRQn         = 28,
    CHUB_RTC_TICK1_IRQn         = 28,
    EXTINT_GPH0_0                 = 29,
    EXTINT_GPH0_1                 = 30,
    EXTINT_GPH0_2                 = 31,
    EXTINT_GPH0_3                 = 32,
    EXTINT_GPH0_4                 = 33,
    EXTINT_GPH0_5                 = 34,
    EXTINT_GPH0_6                 = 35,
    EXTINT_GPH0_7                 = 36,
    //NA                        = 37,
    EXTINT_GPH1_0                 = 38,
    EXTINT_GPH1_1                 = 39,
    EXTINT_GPH1_2                 = 40,
    EXTINT_GPH1_3                 = 41,
    EXTINT_GPH2_0                 = 42,
    EXTINT_GPH2_1                 = 43,
    EXTINT_GPH2_2                 = 44,
    EXTINT_GPH2_3                 = 45,
    EXTINT_GPH2_4                 = 46,
    EXTINT_GPH2_5                 = 47,


    NUM_INTERRUPTS

}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif

