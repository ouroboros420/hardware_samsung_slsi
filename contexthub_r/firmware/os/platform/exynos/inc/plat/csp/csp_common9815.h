/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common9815.h
 *      Purpose: To define common definitions for S5E9815
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

#ifndef __CSP_COMMON9815_H__
#define __CSP_COMMON9815_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CMU_BASE_ADDRESS              0x40100000
#define SYSREG_BASE_ADDRESS           0x40120000
#define WDT_BASE_ADDRESS              0x40130000
#define TIMER_BASE_ADDRESS            0x40140000
#define PWM_BASE_ADDRESS              0x40150000
#define DMAILBOX_BASE_ADDRESS         0x40160000
#define MAILBOX_NPU_BASE_ADDRESS      0x40170000
#define MAILBOX_DSP_BASE_ADDRESS      0x40180000
#define SERIAL_LIF_BASE_ADDRESS       0x40190000
#define BUSIF_HPM_BASE_ADDRESS        0x401A0000
#define COMB_WAKEUP_SRC_BASE_ADDRESS  0x401E0000

#define SYSREG_CHUB2WLBT_BASE_ADDRESS 0x40940000
#define SYSREG_CHUB2AP_BASE_ADDRESS   0x40950000
#define SYSREG_CHUB2APM_BASE_ADDRESS  0x40960000
#define USI0_CHUB_BASE_ADDRESS        0x40970000
#define I2C0_CHUB_BASE_ADDRESS        (USI0_CHUB_BASE_ADDRESS)
#define SPI0_CHUB_BASE_ADDRESS        (USI0_CHUB_BASE_ADDRESS)
#define UART0_CHUB_BASE_ADDRESS       (USI0_CHUB_BASE_ADDRESS)
#define I2C1_CHUB_BASE_ADDRESS        0x40980000
#define USI1_CHUB_BASE_ADDRESS        0x40990000
#define I2C2_CHUB_BASE_ADDRESS        (USI1_CHUB_BASE_ADDRESS)
#define SPI1_CHUB_BASE_ADDRESS        (USI1_CHUB_BASE_ADDRESS)
#define UART1_CHUB_BASE_ADDRESS       (USI1_CHUB_BASE_ADDRESS)
#define I2C3_CHUB_BASE_ADDRESS        0x409A0000
#define USI2_CHUB_BASE_ADDRESS        0x409B0000
#define I2C4_CHUB_BASE_ADDRESS        (USI2_CHUB_BASE_ADDRESS)
#define SPI2_CHUB_BASE_ADDRESS        (USI2_CHUB_BASE_ADDRESS)
#define UART2_CHUB_BASE_ADDRESS       (USI2_CHUB_BASE_ADDRESS)
#define I2C5_CHUB_BASE_ADDRESS        0x409C0000
#define GPIO_BASE_ADDRESS             0x409D0000

#define APM_BASE    0x40300000
#define RTC_BASE_ADDRESS            (APM_BASE + 0x0C0000)
#define MAILBOX_WLBT_BASE_ADDRESS   (APM_BASE + 0x0D0000)
#define MAILBOX_CP_BASE_ADDRESS     (APM_BASE + 0x130000)
#define MAILBOX_VTS_BASE_ADDRESS    (APM_BASE + 0x140000)
#define MAILBOX_GNSS_BASE_ADDRESS   (APM_BASE + 0x180000)
#define MAILBOX_APM_BASE_ADDRESS    (APM_BASE + 0x1A0000)
#define MAILBOX_ABOX_BASE_ADDRESS   (APM_BASE + 0x1B0000)
#define MAILBOX_AP_BASE_ADDRESS     (APM_BASE + 0x1C0000)


#define CMGP_BASE   0x40500000
#define SYSREG_CMGP_BASE_ADDRESS    (CMGP_BASE + 0x20000)
#define GPIO_CMGP_BASE_ADDRESS      (CMGP_BASE + 0x30000)
#define SYSREG_CMGP2CHUB_BASE_ADDRESS (CMGP_BASE + 0x90000)

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
#define USI_CMGP0                   USI_CHUB_CHANNEL_MAX
#define USI_I2C_CMGP0               7
#define USI_CMGP1                   8
#define USI_I2C_CMGP1               9
#define USI_CMGP2                   10
#define USI_I2C_CMGP2               11
#define USI_CHANNEL_MAX             12

#define I2C_CHUB0                   0
#define I2C_CHUB1                   1
#define I2C_CHUB2                   2
#define I2C_CHUB3                   3
#define I2C_CHUB4                   4
#define I2C_CHUB5                   5
#define I2C_CHUB_CHANNEL_MAX        6
#define I2C_CMGP0                   I2C_CHUB_CHANNEL_MAX
#define I2C_CMGP1                   7
#define I2C_CMGP2                   8
#define I2C_CMGP3                   9
#define I2C_CMGP4                   10
#define I2C_CMGP5                   11
#define I2C_CHANNEL_MAX             12

#define SPI_CHUB0                   0
#define SPI_CHUB1                   1
#define SPI_CHUB2                   2
#define SPI_CHUB_CHANNEL_MAX        3
#define SPI_CMGP0                   SPI_CHUB_CHANNEL_MAX
#define SPI_CMGP1                   4
#define SPI_CMGP2                   5
#define SPI_CHANNEL_MAX             6

#define UART_CHUG0                  0
#define UART_CHUG1                  1
#define UART_CHUG2                  2
#define UART_CHUB_CHANNEL_MAX       3
#define UART_CMGP0                  UART_CHUB_CHANNEL_MAX
#define UART_CMGP1                  4
#define UART_CMGP2                  5
#define UART_CHANNEL_MAX            6

#define UART_CHANNEL_DEBUG          USI_CMGP3

#define ADC_CHANNEL_MAX             0

#define GPIO_CHANNEL_MAX            17

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK          0x00000021

#define OSC_CLOCK                   24576000

#define RTC_INTERRUPT_NO            RTC_IRQn
#define SUPPORT_PWM_TIMER4          1
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER4
#define MAILBOX_VERSION             (0x1)
#define MAILBOX_ABOX_SUPPORT        1
#define MAILBOX_NPU_SUPPORT         1
#define MAILBOX_DSP_SUPPORT         1

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
    MB_ABOX_IRQn                = 14,
    MB_NPU0_IRQn                = 15,
    MP_NPU1_IRQn                = 16,
    DMAILBOX_IRQn               = 17,
    SYSREG0_CMGP_IRQn           = 18,
    SYSREG1_CMGP_IRQn           = 19,
    RTC_IRQn                    = 20,
    CHUB_RTC_TICK0_IRQn         = 20,
    CHUB_RTC_TICK1_IRQn         = 20,
    SERIAL_LIF_US_IRQn          = 21,
    SERIAL_LIF_VT_IRQn          = 22,
    SERIAL_LIF_IRQn             = 23,
    USI_CHUB00_IRQn             = 24,
    I2C_CHUB00_IRQn             = 25,
    USI_CHUB01_IRQn             = 26,
    I2C_CHUB01_IRQn             = 27,
    USI_CHUB02_IRQn             = 28,
    I2C_CHUB02_IRQn             = 29,
    CMGP_REQ_IRQn               = 30,
    MIF_REQ_IRQn                = 31,
    PWM0_CHUB_IRQn              = 32,
    PWM1_CHUB_IRQn              = 33,
    PWM2_CHUB_IRQn              = 34,
    PWM3_CHUB_IRQn              = 35,
    PWM4_CHUB_IRQn              = 36,
    EXTINT_GPH0_0               = 37,
    EXTINT_GPH0_1               = 38,
    EXTINT_GPH0_2               = 39,
    EXTINT_GPH0_3               = 40,
    EXTINT_GPH1_0               = 41,
    EXTINT_GPH1_1               = 42,
    EXTINT_GPH1_2               = 43,
    EXTINT_GPH1_3               = 44,
    EXTINT_GPH2_0               = 45,
    EXTINT_GPH2_1               = 46,
    EXTINT_GPH2_2               = 47,
    EXTINT_GPH2_3               = 48,
    EXTINT_GPH3_0               = 49,
    EXTINT_GPH3_1               = 50,
    EXTINT_GPH3_2               = 51,
    EXTINT_GPH3_3               = 52,
    EXTINT_GPH3_4               = 53,
    HPM_IRQn                    = 54,
    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif

