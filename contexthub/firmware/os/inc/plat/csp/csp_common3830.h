/*----------------------------------------------------------------------------
 * Exynos SoC  -  CSP
 *
 * Copyright (C) 2022 Samsung Electronics Co., Ltd.
 *
 * Huiling wu <huiling.wu@samsung.com>
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

#ifndef __CSP_COMMON3830_H__
#define __CSP_COMMON3830_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CMU_BASE_ADDRESS                0x40100000

#define SYSREG_BASE_ADDRESS             0x40110000
#define TIMER_BASE_ADDRESS              0x40120000
#define WDT_BASE_ADDRESS                0x40130000
#define PWM_BASE_ADDRESS                0x40160000

#define ALIVE_BASE_ADDRESS              0x40300000
#define MAILBOX_CP_BASE_ADDRESS         0x40340000
#define MAILBOX_GNSS_BASE_ADDRESS       0x40390000
#define MAILBOX_WLBT_BASE_ADDRESS       0x403E0000
#define MAILBOX_APM_BASE_ADDRESS        0x403F0000
#define MAILBOX_AP_BASE_ADDRESS         0x40400000
#define RTC_BASE_ADDRESS                0x40430000

#define CMGP_BASE                       0x40600000
#define CMU_CMGP_BASE_ADDRESS           (CMGP_BASE)
#define SYSREG_CMGP_BASE_ADDRESS        (CMGP_BASE + 0x20000)
#define GPIO_CMGP_BASE_ADDRESS          (CMGP_BASE + 0x30000)
#define ADC0_CMGP_BASE_ADDRESS          (CMGP_BASE + 0x40000)
#define ADC1_CMGP_BASE_ADDRESS          (CMGP_BASE + 0x50000)
#define SYSREG_CMGP2CHUB_BASE_ADDRESS       (CMGP_BASE + 0x90000)
#define SYSREG_CMGP2PMU_CHUB_BASE_ADDRESS   (CMGP_BASE + 0xB0000)

#define USI0_CMGP_BASE_ADDRESS          (CMGP_BASE + 0x100000)
#define I2C0_CMGP_BASE_ADDRESS          USI0_CMGP_BASE_ADDRESS
#define USI1_CMGP_BASE_ADDRESS          (CMGP_BASE + 0x120000)
#define I2C1_CMGP_BASE_ADDRESS          USI1_CMGP_BASE_ADDRESS

#define USI0_BASE_ADDRESS               (USI0_CMGP_BASE_ADDRESS)
#define USI1_BASE_ADDRESS               (USI1_CMGP_BASE_ADDRESS)

#define I2C0_BASE_ADDRESS               (USI0_CMGP_BASE_ADDRESS)
#define I2C1_BASE_ADDRESS               (USI1_CMGP_BASE_ADDRESS)

#define SPI0_BASE_ADDRESS               (USI0_CMGP_BASE_ADDRESS)
#define SPI1_BASE_ADDRESS               (USI1_CMGP_BASE_ADDRESS)

#define USI_CHUB_CHANNEL_MAX        0
#define USI_CMGP0                   USI_CHUB_CHANNEL_MAX
#define USI_CMGP1                   1
#define USI_CHANNEL_MAX             2

#define I2C_CHUB_CHANNEL_MAX        0
#define I2C_CMGP0                   I2C_CHUB_CHANNEL_MAX
#define I2C_CMGP1                   1
#define I2C_CHANNEL_MAX             2

#define SPI_CHUB_CHANNEL_MAX        0
#define SPI_CMGP0                   SPI_CHUB_CHANNEL_MAX
#define SPI_CMGP1                   1
#define SPI_CHANNEL_MAX             2

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK	        0x07000000

#define OSC_CLOCK                   (24576000)

#define RTC_INTERRUPT_NO            RTC_IRQn
#define SUPPORT_PWM_TIMER4          0
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x1)
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

    DMIC_AHB0_IRQn              = 2,
    CHUB_VVALID_INTR0_IRQn      = 3,
    CHUB_VVALID_INTR1_IRQn      = 4,
    CHUB_VVALID_INTR2_IRQn      = 5,
    PWM0_CHUB_IRQn              = 6,
    PWM1_CHUB_IRQn              = 7,
    PWM2_CHUB_IRQn              = 8,
    PWM3_CHUB_IRQn              = 9,
    MB_WLBT_IRQn                = 10,
    MB_AP_IRQn                  = 11,
    MB_CP_IRQn                  = 12,
    MB_APM_IRQn                 = 13,
    MB_GNSS_IRQn                = 14,
    RTC_IRQn                    = 15,
    CHUB_RTC_TICK0_IRQn         = 15,
    CHUB_RTC_TICK1_IRQn         = 15,
    MIF_REQ_ACK                 = 16,
    SYSREG0_CMGP_IRQn           = 17,
    SYSREG1_CMGP_IRQn           = 18,
    SYSREG0_CMGP2PMU_IRQn       = 19,
    SYSREG1_CMGP2PMU_IRQn       = 20,
    ALIVE_GPIO                  = 21,

    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif
