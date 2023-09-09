/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common9925.h
 *      Purpose: To define common definitions for S5E9925
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

#ifndef __CSP_COMMON9925_H__
#define __CSP_COMMON9925_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CMU_BASE_ADDRESS              0x40100000 // 0x14800000
#define WDT_BASE_ADDRESS              0x40130000
#define TIMER_BASE_ADDRESS            0x40140000
#define PWM_BASE_ADDRESS              0x40160000
#define MAILBOX_DNC_BASE_ADDRESS      0x40180000
#define COMB_WAKEUP_SRC_BASE_ADDRESS  0x401E0000

#define APM_BASE_ADDRESS              0x40300000 // 0x14C00000
#define RTC_BASE_ADDRESS              (APM_BASE_ADDRESS)
#define MAILBOX_CP_BASE_ADDRESS       (APM_BASE_ADDRESS + 0x030000)
#define MAILBOX_GNSS_BASE_ADDRESS     (APM_BASE_ADDRESS + 0x080000)
#define MAILBOX_APM_BASE_ADDRESS      (APM_BASE_ADDRESS + 0x0A0000)
#define MAILBOX_AP_BASE_ADDRESS       (APM_BASE_ADDRESS + 0x0C0000)

#define UFD_BASE_ADDRESS              0x40400000 // 0x15100000

#if defined (EVT0)
    #define SYSREG_BASE_ADDRESS           0x40120000
    #define DMAILBOX_BASE_ADDRESS         0x40170000
    #define UPMU_BASE_ADDRESS             0x40250000
    #define MCT_ALIVE_BASE_ADDRESS        0x40790000
    #define SYSREG_CHUB2AP_BASE_ADDRESS   0x40920000
    #define SYSREG_CHUB2APM_BASE_ADDRESS  0x40930000
    #define USI_CHUB0_BASE_ADDRESS        0x40950000
    #define USI_CHUB1_BASE_ADDRESS        0x40960000
    #define USI_CHUB2_BASE_ADDRESS        0x40970000
    #define USI_CHUB3_BASE_ADDRESS        0x409C0000
    #define SPI_I2C_CHUB0_BASE_ADDRESS    0x409B0000
    #define SPI_I2C_CHUB1_BASE_ADDRESS    0x40980000
    #define USI_I2C_CHUB_BASE_ADDRESS     0x40990000
    #define I3C_CHUB_BASE_ADDRESS         0x409A0000
    #define GPIO_BASE_ADDRESS             0x409D0000
    #define MAILBOX_VTS_BASE_ADDRESS      (APM_BASE_ADDRESS + 0x040000)
    #define CMU_CMGP_BASE_ADDRESS         0x40500000 // 0x14E00000
    #define SYSREG_CMGP_BASE_ADDRESS      0x40520000
    #define GPIO_CMGP_BASE_ADDRESS        0x40530000
    #define SYSREG_CMGP2CHUB_BASE_ADDRESS 0x40590000

#elif defined (EVT1)
    #define SYSREG_BASE_ADDRESS           0x402E0000
    #define DMAILBOX_BASE_ADDRESS         0x40AC0000
    #define MCT_ALIVE_BASE_ADDRESS        0x40890000
    #define SYSREG_CHUB2AP_BASE_ADDRESS   0x40220000
    #define SYSREG_CHUB2APM_BASE_ADDRESS  0x40230000
    #define USI_CHUB0_BASE_ADDRESS        0x40250000
    #define USI_CHUB1_BASE_ADDRESS        0x40260000
    #define USI_CHUB2_BASE_ADDRESS        0x40270000
    #define USI_CHUB3_BASE_ADDRESS        0x402C0000
    #define SPI_I2C_CHUB0_BASE_ADDRESS    0x402B0000
    #define SPI_I2C_CHUB1_BASE_ADDRESS    0x40280000
    #define USI_I2C_CHUB_BASE_ADDRESS     0x40290000
    #define I3C_CHUB_BASE_ADDRESS         0x402A0000
    #define GPIO_BASE_ADDRESS             0x402D0000
    #define UPMU_BASE_ADDRESS             0x40A60000 // 0x15560000
    #define MAILBOX_VTS_BASE_ADDRESS      0x40AD0000 // 0x155D0000
    #define CMU_CHUBVTS_BASE_ADDRESS      0x40A00000 // 0x15500000
    #define CMU_CMGP_BASE_ADDRESS         0x40600000 // 0x14E00000
    #define SYSREG_CMGP_BASE_ADDRESS      0x40620000
    #define GPIO_CMGP_BASE_ADDRESS        0x40630000
    #define SYSREG_CMGP2CHUB_BASE_ADDRESS 0x40690000

#endif

/* USI_CHUB0 */
#define USI0_CHUB_BASE_ADDRESS        (USI_CHUB0_BASE_ADDRESS)
#define I2C0_CHUB_BASE_ADDRESS        (USI_CHUB0_BASE_ADDRESS)
#define SPI0_CHUB_BASE_ADDRESS        (USI_CHUB0_BASE_ADDRESS)

/* SPI_I2C_CHUB0 */
#define USI1_CHUB_BASE_ADDRESS        (SPI_I2C_CHUB0_BASE_ADDRESS)
#define I2C1_CHUB_BASE_ADDRESS        (SPI_I2C_CHUB0_BASE_ADDRESS)

/* USI_CHUB1 */
#define USI2_CHUB_BASE_ADDRESS        (USI_CHUB1_BASE_ADDRESS)
#define I2C2_CHUB_BASE_ADDRESS        (USI_CHUB1_BASE_ADDRESS)
#define SPI1_CHUB_BASE_ADDRESS        (USI_CHUB1_BASE_ADDRESS)

/* SPI_I2C_CHUB1 */
#define USI3_CHUB_BASE_ADDRESS        (SPI_I2C_CHUB1_BASE_ADDRESS)
#define I2C3_CHUB_BASE_ADDRESS        (SPI_I2C_CHUB1_BASE_ADDRESS)

/* USI_CHUB2 */
#define USI4_CHUB_BASE_ADDRESS        (USI_CHUB2_BASE_ADDRESS)
#define I2C4_CHUB_BASE_ADDRESS        (USI_CHUB2_BASE_ADDRESS)
#define SPI2_CHUB_BASE_ADDRESS        (USI_CHUB2_BASE_ADDRESS)

/* USI_CHUB3 */
#define USI5_CHUB_BASE_ADDRESS        (USI_CHUB3_BASE_ADDRESS)
#define I2C5_CHUB_BASE_ADDRESS        (USI_CHUB3_BASE_ADDRESS)
#define SPI3_CHUB_BASE_ADDRESS        (USI_CHUB3_BASE_ADDRESS)

/* I2C_CHUB */
#define USI6_CHUB_BASE_ADDRESS        (USI_I2C_CHUB_BASE_ADDRESS)
#define I2C6_CHUB_BASE_ADDRESS        (USI_I2C_CHUB_BASE_ADDRESS)



/* USI_CMGP0 */
#define USI_CMGP0_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x100000)
#define USI0_CMGP_BASE_ADDRESS      (USI_CMGP0_BASE_ADDRESS)
#define I2C0_CMGP_BASE_ADDRESS      (USI_CMGP0_BASE_ADDRESS)
#define SPI0_CMGP_BASE_ADDRESS      (USI_CMGP0_BASE_ADDRESS)

/* SPI_I2C_CMGP0 */
#define SPI_I2C_CMGP0_BASE_ADDRESS  (CMU_CMGP_BASE_ADDRESS + 0x110000)
#define USI1_CMGP_BASE_ADDRESS      (SPI_I2C_CMGP0_BASE_ADDRESS)
#define I2C1_CMGP_BASE_ADDRESS      (SPI_I2C_CMGP0_BASE_ADDRESS)

/* USI_CMGP1 */
#define USI_CMGP1_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x120000)
#define USI2_CMGP_BASE_ADDRESS      (USI_CMGP1_BASE_ADDRESS)
#define I2C2_CMGP_BASE_ADDRESS      (USI_CMGP1_BASE_ADDRESS)
#define SPI1_CMGP_BASE_ADDRESS      (USI_CMGP1_BASE_ADDRESS)

/* SPI_I2C_CMGP1 */
#define SPI_I2C_CMGP1_BASE_ADDRESS  (CMU_CMGP_BASE_ADDRESS + 0x130000)
#define USI3_CMGP_BASE_ADDRESS      (SPI_I2C_CMGP1_BASE_ADDRESS)
#define I2C3_CMGP_BASE_ADDRESS      (SPI_I2C_CMGP1_BASE_ADDRESS)

/* USI_CMGP2 */
#define USI_CMGP2_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x140000)
#define USI4_CMGP_BASE_ADDRESS      (USI_CMGP2_BASE_ADDRESS)
#define I2C4_CMGP_BASE_ADDRESS      (USI_CMGP2_BASE_ADDRESS)
#define SPI2_CMGP_BASE_ADDRESS      (USI_CMGP2_BASE_ADDRESS)

/* I2C_CMGP2 */
#define I2C_CMGP2_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x150000)
#define USI5_CMGP_BASE_ADDRESS      (I2C_CMGP2_BASE_ADDRESS)
#define I2C5_CMGP_BASE_ADDRESS      (I2C_CMGP2_BASE_ADDRESS)

/* USI_CMGP3 */
#define USI_CMGP3_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x160000)
#define USI6_CMGP_BASE_ADDRESS      (USI_CMGP3_BASE_ADDRESS)
#define I2C6_CMGP_BASE_ADDRESS      (USI_CMGP3_BASE_ADDRESS)
#define SPI3_CMGP_BASE_ADDRESS      (USI_CMGP3_BASE_ADDRESS)

/* I2C_CMGP3 */
#define I2C_CMGP3_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x170000)
#define USI7_CMGP_BASE_ADDRESS      (I2C_CMGP3_BASE_ADDRESS)
#define I2C7_CMGP_BASE_ADDRESS      (I2C_CMGP3_BASE_ADDRESS)

/* USI_CMGP4 */
#define USI_CMGP4_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x180000)
#define USI8_CMGP_BASE_ADDRESS      (USI_CMGP4_BASE_ADDRESS)
#define I2C8_CMGP_BASE_ADDRESS      (USI_CMGP4_BASE_ADDRESS)
#define SPI4_CMGP_BASE_ADDRESS      (USI_CMGP4_BASE_ADDRESS)

/* I2C_CMGP4 */
#define I2C_CMGP4_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x190000)
#define USI9_CMGP_BASE_ADDRESS      (I2C_CMGP4_BASE_ADDRESS)
#define I2C9_CMGP_BASE_ADDRESS      (I2C_CMGP4_BASE_ADDRESS)

/* USI_CMGP5 */
#define USI_CMGP5_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x1a0000)
#define USI10_CMGP_BASE_ADDRESS     (USI_CMGP5_BASE_ADDRESS)
#define I2C10_CMGP_BASE_ADDRESS     (USI_CMGP5_BASE_ADDRESS)
#define SPI5_CMGP_BASE_ADDRESS      (USI_CMGP5_BASE_ADDRESS)

/* I2C_CMGP5 */
#define I2C_CMGP5_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x1b0000)
#define USI11_CMGP_BASE_ADDRESS     (I2C_CMGP5_BASE_ADDRESS)
#define I2C11_CMGP_BASE_ADDRESS     (I2C_CMGP5_BASE_ADDRESS)

/* USI_CMGP6 */
#define USI_CMGP6_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x1c0000)
#define USI12_CMGP_BASE_ADDRESS     (USI_CMGP6_BASE_ADDRESS)
#define I2C12_CMGP_BASE_ADDRESS     (USI_CMGP6_BASE_ADDRESS)
#define SPI6_CMGP_BASE_ADDRESS      (USI_CMGP6_BASE_ADDRESS)

/* I2C_CMGP6 */
#define I2C_CMGP6_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x1d0000)
#define USI13_CMGP_BASE_ADDRESS     (I2C_CMGP6_BASE_ADDRESS)
#define I2C13_CMGP_BASE_ADDRESS     (I2C_CMGP6_BASE_ADDRESS)

/* I2C_CMGP7 */
#define I2C_CMGP7_BASE_ADDRESS      (CMU_CMGP_BASE_ADDRESS + 0x1e0000)
#define USI14_CMGP_BASE_ADDRESS     (I2C_CMGP7_BASE_ADDRESS)
#define I2C14_CMGP_BASE_ADDRESS     (I2C_CMGP7_BASE_ADDRESS)

#define I2C0_BASE_ADDRESS           I2C0_CHUB_BASE_ADDRESS
#define I2C1_BASE_ADDRESS           I2C1_CHUB_BASE_ADDRESS
#define I2C2_BASE_ADDRESS           I2C2_CHUB_BASE_ADDRESS
#define I2C3_BASE_ADDRESS           I2C3_CHUB_BASE_ADDRESS
#define I2C4_BASE_ADDRESS           I2C4_CHUB_BASE_ADDRESS
#define I2C5_BASE_ADDRESS           I2C5_CHUB_BASE_ADDRESS
#define I2C6_BASE_ADDRESS           I2C6_CHUB_BASE_ADDRESS
#define I2C7_BASE_ADDRESS           I2C0_CMGP_BASE_ADDRESS
#define I2C8_BASE_ADDRESS           I2C1_CMGP_BASE_ADDRESS
#define I2C9_BASE_ADDRESS           I2C2_CMGP_BASE_ADDRESS
#define I2C10_BASE_ADDRESS          I2C3_CMGP_BASE_ADDRESS
#define I2C11_BASE_ADDRESS          I2C4_CMGP_BASE_ADDRESS
#define I2C12_BASE_ADDRESS          I2C5_CMGP_BASE_ADDRESS
#define I2C13_BASE_ADDRESS          I2C6_CMGP_BASE_ADDRESS
#define I2C14_BASE_ADDRESS          I2C7_CMGP_BASE_ADDRESS
#define I2C15_BASE_ADDRESS          I2C8_CMGP_BASE_ADDRESS
#define I2C16_BASE_ADDRESS          I2C9_CMGP_BASE_ADDRESS
#define I2C17_BASE_ADDRESS          I2C10_CMGP_BASE_ADDRESS
#define I2C18_BASE_ADDRESS          I2C11_CMGP_BASE_ADDRESS
#define I2C19_BASE_ADDRESS          I2C12_CMGP_BASE_ADDRESS
#define I2C20_BASE_ADDRESS          I2C13_CMGP_BASE_ADDRESS
#define I2C21_BASE_ADDRESS          I2C14_CMGP_BASE_ADDRESS

#define SPI0_BASE_ADDRESS           SPI0_CHUB_BASE_ADDRESS
#define SPI1_BASE_ADDRESS           SPI1_CHUB_BASE_ADDRESS
#define SPI2_BASE_ADDRESS           SPI2_CHUB_BASE_ADDRESS
#define SPI3_BASE_ADDRESS           SPI3_CHUB_BASE_ADDRESS
#define SPI4_BASE_ADDRESS           SPI0_CMGP_BASE_ADDRESS
#define SPI5_BASE_ADDRESS           SPI1_CMGP_BASE_ADDRESS
#define SPI6_BASE_ADDRESS           SPI2_CMGP_BASE_ADDRESS
#define SPI7_BASE_ADDRESS           SPI3_CMGP_BASE_ADDRESS
#define SPI8_BASE_ADDRESS           SPI4_CMGP_BASE_ADDRESS
#define SPI9_BASE_ADDRESS           SPI5_CMGP_BASE_ADDRESS
#define SPI10_BASE_ADDRESS          SPI6_CMGP_BASE_ADDRESS

#define I3C0_BASE_ADDRESS           I3C_CHUB_BASE_ADDRESS

#define USI_CHUB0                   0
#define USI_SPI_I2C_CHUB0           1
#define USI_CHUB1                   2
#define USI_SPI_I2C_CHUB1           3
#define USI_CHUB2                   4
#define USI_CHUB3                   5
#define USI_I2C_CHUB                6
#define USI_CHUB_CHANNEL_MAX        7
#define USI_CMGP0                   USI_CHUB_CHANNEL_MAX
#define USI_I2C_CMGP0               8
#define USI_CMGP1                   9
#define USI_I2C_CMGP1               10
#define USI_CMGP2                   11
#define USI_I2C_CMGP2               12
#define USI_CMGP3                   13
#define USI_I2C_CMGP3               14
#define USI_CMGP4                   15
#define USI_I2C_CMGP4               16
#define USI_CMGP5                   17
#define USI_I2C_CMGP5               18
#define USI_CMGP6                   19
#define USI_I2C_CMGP6               20
#define USI_I2C_CMGP7               21
#define USI_CHANNEL_MAX             22

#define I2C_CHUB0                   0 // USI_CHUB0
#define I2C_CHUB1                   1 // USI_CHUB1
#define I2C_CHUB2                   2 // USI_CHUB2
#define I2C_CHUB3                   3 // SPI_I2C_CHUB1
#define I2C_CHUB4                   4 // I2C_CHUB
#define I2C_CHUB5                   5 // SPI_I2C_CHUB0
#define I2C_CHUB6                   6 // USI_CHUB3
#define I2C_CHUB_CHANNEL_MAX        7
#define I2C_CMGP0                   I2C_CHUB_CHANNEL_MAX
#define I2C_CMGP1                   8
#define I2C_CMGP2                   9
#define I2C_CMGP3                   10
#define I2C_CMGP4                   11
#define I2C_CMGP5                   12
#define I2C_CMGP6                   13
#define I2C_CMGP7                   14
#define I2C_CMGP8                   15
#define I2C_CMGP9                   16
#define I2C_CMGP10                  17
#define I2C_CMGP11                  18
#define I2C_CMGP12                  19
#define I2C_CMGP13                  20
#define I2C_CMGP14                  21
#define I2C_CHANNEL_MAX             22

#define SPI_CHUB0                   0
#define SPI_CHUB1                   1
#define SPI_CHUB2                   2
#define SPI_CHUB3                   3
#define SPI_CHUB_CHANNEL_MAX        4
#define SPI_CMGP0                   SPI_CHUB_CHANNEL_MAX
#define SPI_CMGP1                   5
#define SPI_CMGP2                   6
#define SPI_CMGP3                   7
#define SPI_CMGP4                   8
#define SPI_CMGP5                   9
#define SPI_CMGP6                   10
#define SPI_CHANNEL_MAX             11

#define I3C_CHUB0                   0
#define I3C_CHANNEL_MAX             1

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK	        0x07000000

#define OSC_CLOCK                   (24576000)

#define RTC_INTERRUPT_NO            RTC_CHUB_IRQn
#define SUPPORT_PWM_TIMER4          0
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)
#define CMGP_INTERRUPT_NUN          54
#define MCT_USE_RTC                 1
#define MCT_COMPENSATE_CNT          101
#define MCT_FREQ                    25600000

#if defined (EVT1)
#define DMAILBOX_USE_SEMA           1
#endif

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
    CSIS6_VVALID_IRQn           = 8,
    MB_GNSS_IRQn                = 9,
    MB_APM_IRQn                 = 10,
    MB_CP_IRQn                  = 11,
    MB_AP_IRQn                  = 12,
    SYSREG0_CMGP_IRQn           = 13,
    SYSREG1_CMGP_IRQn           = 14,
    RTC_CHUB_IRQn               = 15,
    MB_VTS_IRQn                 = 16,
#if defined (EVT0)
    DMAILBOX_IRQn               = 17,
#elif defined (EVT1)
    DNC_IRQn                    = 17,
#endif
    USI_CHUB0_IRQn              = 18,
    USI_CHUB1_IRQn              = 19,
    USI_CHUB2_IRQn              = 20,
    USI_CHUB3_IRQn              = 21,
    I2C_CHUB0_IRQn              = 22,
    I3C_CHUB0_IRQn              = 23,
    CMGP_REQ_IRQn               = 24,
    MIF_REQ_IRQn                = 25,
    PWM0_CHUB_IRQn              = 26,
    PWM1_CHUB_IRQn              = 27,
    PWM2_CHUB_IRQn              = 28,
    PWM3_CHUB_IRQn              = 29,
    PWM4_CHUB_IRQn              = 30,
    EXTINT_GPH0_0               = 31,
    EXTINT_GPH0_1               = 32,
    EXTINT_GPH0_2               = 33,
    EXTINT_GPH0_3               = 34,
    EXTINT_GPH1_0               = 35,
    EXTINT_GPH1_1               = 36,
    EXTINT_GPH1_2               = 37,
    EXTINT_GPH1_3               = 38,
    EXTINT_GPH2_0               = 39,
    EXTINT_GPH2_1               = 40,
    EXTINT_GPH2_2               = 41,
    EXTINT_GPH2_3               = 42,
    EXTINT_GPH3_0               = 43,
    EXTINT_GPH3_1               = 44,
    EXTINT_GPH3_2               = 45,
    EXTINT_GPH3_3               = 46,
    EXTINT_GPH4_0               = 47,
    EXTINT_GPH4_1               = 48,
    EXTINT_GPH4_2               = 49,
    EXTINT_GPH4_3               = 50,
    EXTINT_GPH5_0               = 51,
    EXTINT_GPH5_1               = 52,
    EXTINT_GPH5_2               = 53,
    EXTINT_GPH5_3               = 54,
    EXTINT_GPH5_4               = 55,
#if defined (EVT0)
    SPI_UFD_IRQn                = 56,
    PDMA_UFD_IRQn               = 57,
    UFD0_IRQn                   = 58,
    UFD1_IRQn                   = 59,
    UFD2_IRQn                   = 60,
    UFD3_IRQn                   = 61,
    UFD4_IRQn                   = 62,
    UFD5_IRQn                   = 63,
    UFD6_IRQn                   = 64,
    UFD7_IRQn                   = 65,
    MB_DNC_IRQn                 = 66,
    SPI_I2C_CHUB00_IRQn         = 67,
    SPI_I2C_CHUB01_IRQn         = 68,
    I3C_UFD_IRQn                = 69,
#elif defined (EVT1)
    PDMA_UFD_IRQn               = 56,
    UFD0_IRQn                   = 57,
    UFD1_IRQn                   = 58,
    UFD2_IRQn                   = 59,

    MB_DNC_IRQn                 = 61,
    MB_ABOX_IRQn                = 62,
    SPI_I2C_CHUB00_IRQn         = 63,
    SPI_I2C_CHUB01_IRQn         = 64,
    I3C_UFD_IRQn                = 65,
    CSIS0_IRQn                  = 66,
    CSIS1_IRQn                  = 67,
    CSIS2_IRQn                  = 68,
    CSIS3_IRQn                  = 69,
    CSIS4_IRQn                  = 70,
    CSIS5_IRQn                  = 71,
    CSIS6_IRQn                  = 72,
    INTREQ_M_DMAILBOX_SEMA      = 73,
    INTREQ_M_PDMA_SEMA          = 74,
    INTREQ_DMAILBOX_SEMA        = 75,
    INTREQ_PDMA_SEMA            = 76,
    INT_RX_REQ0_DMAILBOX        = 77,
    INT_RX_REQ1_DMAILBOX        = 78,
#endif
    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif

