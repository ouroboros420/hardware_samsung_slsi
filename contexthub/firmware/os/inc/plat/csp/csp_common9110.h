/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common9110.h
 *      Purpose: To define common definitions for s5e9110
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

#ifndef __CSP_COMMON9110_H__
#define __CSP_COMMON9110_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif


#define CMU_BASE_ADDRESS                    0x40100000
#define SYSREG_BASE_ADDRESS                 0x40110000
#define TIMER_BASE_ADDRESS                  0x40120000
#define WDT_BASE_ADDRESS                    0x40130000
#define PDMA_BASE_ADDRESS                   0x40150000
#define PWM_BASE_ADDRESS                    0x40160000
#define USI0_CHUB_BASE_ADDRESS              0x40170000
#define SPI0_CHUB_BASE_ADDRESS              USI0_CHUB_BASE_ADDRESS
#define I2C0_CHUB_BASE_ADDRESS              0x40180000
#define I2C1_CHUB_BASE_ADDRESS              0x40190000
#define GPIO_CMGPALV_APBIF_BASE_ADDRESS     0x401A0000
#define CHUB_RTC_BASE_ADDRESS               0x401B0000
#define GPIO_CMGP_ALV_BASE_ADDRESS          GPIO_CMGPALV_APBIF_BASE_ADDRESS

#define APM_BASE    0x40300000 //APM base address is 0x1190_0000 @ AP side and will be mapped to this by BAAW
#define MAILBOX_CP_BASE_ADDRESS             (APM_BASE + 0x040000)
#define MAILBOX_GNSS_BASE_ADDRESS           (APM_BASE + 0x090000)
#define MAILBOX_WLBT_BASE_ADDRESS           (APM_BASE + 0x0E0000)
#define MAILBOX_APM_BASE_ADDRESS            (APM_BASE + 0x0F0000)
#define MAILBOX_AP_BASE_ADDRESS             (APM_BASE + 0x100000)

#define RTC_BASE_ADDRESS                    CHUB_RTC_BASE_ADDRESS


#define CMGP_BASE                           0x40600000
#define CMU_CMGP_BASE_ADDRESS               CMGP_BASE
#define SYSREG_CMGP_BASE_ADDRESS            (CMGP_BASE + 0x20000)
#define GPIO_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x30000)
#define ADC_CMGP_BASE_ADDRESS               (CMGP_BASE + 0x40000)
#define SYSREG_CMGP2CHUB_BASE_ADDRESS       (CMGP_BASE + 0x90000)
#define SYSREG_CMGP2PMU_CHUB_BASE_ADDRESS   (CMGP_BASE + 0xB0000)

#define USI0_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x100000)
#define I2C0_CMGP_BASE_ADDRESS              USI0_CMGP_BASE_ADDRESS
#define SPI0_CMGP_BASE_ADDRESS              USI0_CMGP_BASE_ADDRESS
#define UART0_CMGP_BASE_ADDRESS             USI0_CMGP_BASE_ADDRESS
#define I2C1_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x110000)

#define USI1_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x120000)
#define I2C2_CMGP_BASE_ADDRESS              USI1_CMGP_BASE_ADDRESS
#define SPI1_CMGP_BASE_ADDRESS              USI1_CMGP_BASE_ADDRESS
#define UART1_CMGP_BASE_ADDRESS             USI1_CMGP_BASE_ADDRESS
#define I2C3_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x130000)

#define USI2_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x140000)
#define I2C4_CMGP_BASE_ADDRESS              USI2_CMGP_BASE_ADDRESS
#define SPI2_CMGP_BASE_ADDRESS              USI2_CMGP_BASE_ADDRESS
#define UART2_CMGP_BASE_ADDRESS             USI2_CMGP_BASE_ADDRESS
#define I2C5_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x150000)

#define USI3_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x160000)
#define I2C6_CMGP_BASE_ADDRESS              USI3_CMGP_BASE_ADDRESS
#define SPI3_CMGP_BASE_ADDRESS              USI3_CMGP_BASE_ADDRESS
#define UART3_CMGP_BASE_ADDRESS             USI3_CMGP_BASE_ADDRESS
#define I2C7_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x170000)

#define I2C8_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x180000)
#define I2C9_CMGP_BASE_ADDRESS              (CMGP_BASE + 0x190000)
#define I2C10_CMGP_BASE_ADDRESS             (CMGP_BASE + 0x1A0000)

#define I2C0_BASE_ADDRESS           USI0_CHUB_BASE_ADDRESS
#define I2C1_BASE_ADDRESS           I2C0_CHUB_BASE_ADDRESS
#define I2C2_BASE_ADDRESS           I2C1_CHUB_BASE_ADDRESS
#define I2C3_BASE_ADDRESS           I2C2_CMGP_BASE_ADDRESS
#define I2C4_BASE_ADDRESS           I2C3_CMGP_BASE_ADDRESS
#define I2C5_BASE_ADDRESS           I2C4_CMGP_BASE_ADDRESS
#define I2C6_BASE_ADDRESS           I2C5_CMGP_BASE_ADDRESS
#define I2C7_BASE_ADDRESS           I2C6_CMGP_BASE_ADDRESS
#define I2C8_BASE_ADDRESS           I2C7_CMGP_BASE_ADDRESS
#define I2C9_BASE_ADDRESS           I2C8_CMGP_BASE_ADDRESS
#define I2C10_BASE_ADDRESS          I2C9_CMGP_BASE_ADDRESS

#define SPI0_BASE_ADDRESS           SPI0_CHUB_BASE_ADDRESS
#define SPI1_BASE_ADDRESS           SPI1_CMGP_BASE_ADDRESS
#define SPI2_BASE_ADDRESS           SPI2_CMGP_BASE_ADDRESS
#define SPI3_BASE_ADDRESS           SPI3_CMGP_BASE_ADDRESS

#define UART0_BASE_ADDRESS          UART1_CMGP_BASE_ADDRESS
#define UART1_BASE_ADDRESS          UART2_CMGP_BASE_ADDRESS
#define UART2_BASE_ADDRESS          UART3_CMGP_BASE_ADDRESS

#define USI_CHUB0                   0
#define USI_I2C_CHUB0               1
#define USI_I2C_CHUB1               2
#define USI_CHUB_CHANNEL_MAX        3
//#define USI_CMGP0                   3
//#define USI_I2C_CMGP0               4
#define USI_CMGP1                   3
#define USI_I2C_CMGP1               4
#define USI_CMGP2                   5
#define USI_I2C_CMGP2               6
#define USI_CMGP3                   7
#define USI_I2C_CMGP3               8
#define USI_CHANNEL_MAX             9

#define I2C_CHUB0                   0
#define I2C_CHUB1                   1
#define I2C_CHUB2                   2
#define I2C_CHUB_CHANNEL_MAX        3
#define I2C_CMGP2                   3
#define I2C_CMGP3                   4
#define I2C_CMGP4                   5
#define I2C_CMGP5                   6
#define I2C_CMGP6                   7
#define I2C_CMGP7                   8
#define I2C_CMGP8                   9
#define I2C_CMGP9                   10
#define I2C_CHANNEL_MAX             11

#define SPI_CHUB0                   0
#define SPI_CHUB_CHANNEL_MAX        1
#define SPI_CMGP1                   1
#define SPI_CMGP2                   2
#define SPI_CMGP3                   3
#define SPI_CHANNEL_MAX             4

#define UART_CMGP0                  0
#define UART_CMGP1                  1
#define UART_CMGP2                  2
#define UART_CHANNEL_MAX            3

#define UART_CHANNEL_DEBUG          USI_CMGP2

#define ADC_CHANNEL_MAX             7

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          0

#define I2C_INTERRUPT_MASK	    0x07000000

#define OSC_CLOCK                   (49152000)

#define RTC_INTERRUPT_NO            CHUB_RTC_TICK1_IRQn
#define SUPPORT_PWM_TIMER4          0
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)
#define CMGP_INTERRUPT_NUN          14

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
    TIMER_CHUB_IRQn              = 1,
    WDT_CHUB_IRQn                = 2,
    CSIS0_VVALID_IRQn            = 3,
    PWM0_CHUB_IRQn               = 4,
    PWM1_CHUB_IRQn               = 5,
    PWM2_CHUB_IRQn               = 6,
    PWM3_CHUB_IRQn               = 7,
    MB_WLBT_IRQn                 = 8,
    MB_AP_IRQn                   = 9,
    MB_CP_IRQn                   = 10,
    MB_APM_IRQn                  = 11,
    MB_GNSS_IRQn                 = 12,
    SYSREG0_CMGP_IRQn            = 13,
    SYSREG1_CMGP_IRQn            = 14,
    CHUB_RTC_TICK0_IRQn          = 15,
    MIF_ACK_IRQn                 = 16,
    CMGP_EINT0_IRQn              = 17,
    CMGP_EINT1_IRQn              = 18,
    CMGP_EINT2_IRQn              = 19,
    CMGP_EINT3_IRQn              = 20,
    CMGP_EINT4_IRQn              = 21,
    CMGP_EINT5_IRQn              = 22,
    CHUB_RTC_TICK1_IRQn          = 23,
    USI_CHUB00_IRQn              = 24,
    I2C_CHUB00_IRQn              = 25,
    I2C_CHUB01_IRQn              = 26,
    APM_ACK_IRQn                 = 27,
    NUM_INTERRUPTS

}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1,
    RtcTickIdMax,
};

#endif

