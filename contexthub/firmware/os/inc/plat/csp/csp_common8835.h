/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common8835.h
 *      Purpose: To define common definitions for S5E8835
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

#ifndef __CSP_COMMON8835_H__
#define __CSP_COMMON8835_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CHUB_BASE_ADDRESS                   0x40100000                      //0x1100_0000
#define CMU_BASE_ADDRESS                    (CHUB_BASE_ADDRESS)             //0x1100_0000
#define SYSREG_BASE_ADDRESS                 (CHUB_BASE_ADDRESS + 0x20000)   //0x1102_0000
#define WDT_BASE_ADDRESS                    (CHUB_BASE_ADDRESS + 0x30000)   //0x1103_0000
#define TIMER_BASE_ADDRESS                  (CHUB_BASE_ADDRESS + 0x40000)   //0x1104_0000
#define PWM_BASE_ADDRESS                    (CHUB_BASE_ADDRESS + 0x60000)   //0x1106_0000
#define COMB_WAKEUP_SRC_BASE_ADDRESS        (CHUB_BASE_ADDRESS + 0xE0000)   //0x110E_0000
#define DUMP_GPR_CHUB_BASE_ADDRESS          (CHUB_BASE_ADDRESS + 0xF0000)   //0x110F_0000
#define SYSREG_CHUB2WLBT_BASE_ADDRESS       (CHUB_BASE_ADDRESS + 0x140000)  //0x1114_0000
#define SYSREG_CHUB2AP_BASE_ADDRESS         (CHUB_BASE_ADDRESS + 0x150000)  //0x1115_0000
#define SYSREG_CHUB2APM_BASE_ADDRESS        (CHUB_BASE_ADDRESS + 0x160000)  //0x1116_0000
#define USI_CHUB0_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x170000)  //0x1117_0000
#define I3C_CHUB_BASE_ADDRESS               (CHUB_BASE_ADDRESS + 0x180000)  //0x1118_0000
#define USI_CHUB1_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x190000)  //0x1119_0000
#define I2C_CHUB1_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x1A0000)  //0x111A_0000
#define USI_CHUB2_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x1B0000)  //0x111B_0000
#define USI_CHUB3_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x100000)  //0x1110_0000
#define I2C_CHUB3_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x110000)  //0x1111_0000
#define GPIO_BASE_ADDRESS                   (CHUB_BASE_ADDRESS + 0x1D0000)  //0x111D_0000
#define GPIO_CHUBEINT_BASE_ADDRESS          (CHUB_BASE_ADDRESS + 0x1E0000)  //0x111E_0000

#define CMGP_BASE_ADDRESS                   0x41000000                      //0x1140_0000
#define CMU_CMGP_BASE_ADDRESS               (CMGP_BASE_ADDRESS)             //0x1140_0000
#define SYSREG_CMGP_BASE_ADDRESS            (CMGP_BASE_ADDRESS + 0x20000)   //0x1142_0000
#define GPIO_CMGP_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x30000)   //0x1143_0000
#define SYSREG_CMGP2CHUB_BASE_ADDRESS       (CMGP_BASE_ADDRESS + 0xA0000)   //0x114A_0000
#define USI_CMGP0_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x100000)  //0x1150_0000
#define I2C_CMGP0_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x110000)  //0x1151_0000
#define USI_CMGP1_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x120000)  //0x1152_0000
#define I2C_CMGP1_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x130000)  //0x1153_0000
#define USI_CMGP2_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x140000)  //0x1154_0000
#define I2C_CMGP2_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x150000)  //0x1155_0000
#define USI_CMGP3_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x160000)  //0x1156_0000
#define I2C_CMGP3_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x170000)  //0x1157_0000
#define USI_CMGP4_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x180000)  //0x1158_0000
#define I2C_CMGP4_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x190000)  //0x1159_0000
#define I3C_CMGP0_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1A0000)  //0x115A_0000
#define I3C_CMGP1_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1B0000)  //0x115B_0000
#define USI_CMGP5_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1C0000)  //0x115C_0000
#define I2C_CMGP5_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1D0000)  //0x115D_0000
#define USI_CMGP6_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1E0000)  //0x115E_0000
#define I2C_CMGP6_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1F0000)  //0x115F_0000

#define ALIVE_BASE_ADDRESS                  0x40900000                      //0x1180_0000
#define MCT_ALIVE_BASE_ADDRESS              (ALIVE_BASE_ADDRESS + 0x90000)  //0x1189_0000
#define RTC_S_BASE_ADDRESS                  (ALIVE_BASE_ADDRESS + 0x100000) //0x1190_0000
#define RTC_0_BASE_ADDRESS                  (ALIVE_BASE_ADDRESS + 0x110000) //0x1191_0000
#define RTC_1_BASE_ADDRESS                  (ALIVE_BASE_ADDRESS + 0x120000) //0x1192_0000
#define RTC_BASE_ADDRESS                    RTC_1_BASE_ADDRESS
#define MAILBOX_CP_BASE_ADDRESS             (ALIVE_BASE_ADDRESS + 0x1D0000) //0x119D_0000
#define MAILBOX_APM_BASE_ADDRESS            (ALIVE_BASE_ADDRESS + 0x1E0000) //0x119E_0000
#define MAILBOX_AP_BASE_ADDRESS             (ALIVE_BASE_ADDRESS + 0x1F0000) //0x119F_0000
#define MAILBOX_GNSS_BASE_ADDRESS           (ALIVE_BASE_ADDRESS + 0x0E0000)// 0x118E_0000
#define GNSS_SHMIPC_BASE_ADDRESS            0x00067000

/* USI_CHUB0 */
#define USI0_CHUB_BASE_ADDRESS              (USI_CHUB0_BASE_ADDRESS)
#define I2C0_CHUB_BASE_ADDRESS              (USI_CHUB0_BASE_ADDRESS)
#define SPI0_CHUB_BASE_ADDRESS              (USI_CHUB0_BASE_ADDRESS)

/* USI_CHUB1 */
#define USI1_CHUB_BASE_ADDRESS              (USI_CHUB1_BASE_ADDRESS)
#define I2C1_CHUB_BASE_ADDRESS              (USI_CHUB1_BASE_ADDRESS)
#define SPI1_CHUB_BASE_ADDRESS              (USI_CHUB1_BASE_ADDRESS)

/* I2C_CHUB1 */
#define USI2_CHUB_BASE_ADDRESS              (I2C_CHUB1_BASE_ADDRESS)
#define I2C2_CHUB_BASE_ADDRESS              (I2C_CHUB1_BASE_ADDRESS)

/* USI_CHUB2 */
#define USI3_CHUB_BASE_ADDRESS              (USI_CHUB2_BASE_ADDRESS)
#define I2C3_CHUB_BASE_ADDRESS              (USI_CHUB2_BASE_ADDRESS)
#define SPI2_CHUB_BASE_ADDRESS              (USI_CHUB2_BASE_ADDRESS)

/* USI_CHUB3 */
#define USI4_CHUB_BASE_ADDRESS              (USI_CHUB3_BASE_ADDRESS)
#define I2C4_CHUB_BASE_ADDRESS              (USI_CHUB3_BASE_ADDRESS)
#define SPI3_CHUB_BASE_ADDRESS              (USI_CHUB3_BASE_ADDRESS)

/* I2C_CHUB3 */
#define USI5_CHUB_BASE_ADDRESS              (I2C_CHUB3_BASE_ADDRESS)
#define I2C5_CHUB_BASE_ADDRESS              (I2C_CHUB3_BASE_ADDRESS)

/* USI_CMGP0 */
#define USI0_CMGP_BASE_ADDRESS              (USI_CMGP0_BASE_ADDRESS)
#define I2C0_CMGP_BASE_ADDRESS              (USI_CMGP0_BASE_ADDRESS)
#define SPI0_CMGP_BASE_ADDRESS              (USI_CMGP0_BASE_ADDRESS)

/* I2C_CMGP0 */
#define USI1_CMGP_BASE_ADDRESS              (I2C_CMGP0_BASE_ADDRESS)
#define I2C1_CMGP_BASE_ADDRESS              (I2C_CMGP0_BASE_ADDRESS)

/* USI_CMGP1 */
#define USI2_CMGP_BASE_ADDRESS              (USI_CMGP1_BASE_ADDRESS)
#define I2C2_CMGP_BASE_ADDRESS              (USI_CMGP1_BASE_ADDRESS)
#define SPI1_CMGP_BASE_ADDRESS              (USI_CMGP1_BASE_ADDRESS)

/* I2C_CMGP1 */
#define USI3_CMGP_BASE_ADDRESS              (I2C_CMGP1_BASE_ADDRESS)
#define I2C3_CMGP_BASE_ADDRESS              (I2C_CMGP1_BASE_ADDRESS)

/* USI_CMGP2 */
#define USI4_CMGP_BASE_ADDRESS              (USI_CMGP2_BASE_ADDRESS)
#define I2C4_CMGP_BASE_ADDRESS              (USI_CMGP2_BASE_ADDRESS)
#define SPI2_CMGP_BASE_ADDRESS              (USI_CMGP2_BASE_ADDRESS)

/* I2C_CMGP2 */
#define USI5_CMGP_BASE_ADDRESS              (I2C_CMGP2_BASE_ADDRESS)
#define I2C5_CMGP_BASE_ADDRESS              (I2C_CMGP2_BASE_ADDRESS)

/* USI_CMGP3 */
#define USI6_CMGP_BASE_ADDRESS              (USI_CMGP3_BASE_ADDRESS)
#define I2C6_CMGP_BASE_ADDRESS              (USI_CMGP3_BASE_ADDRESS)
#define SPI3_CMGP_BASE_ADDRESS              (USI_CMGP3_BASE_ADDRESS)

/* I2C_CMGP3 */
#define USI7_CMGP_BASE_ADDRESS              (I2C_CMGP3_BASE_ADDRESS)
#define I2C7_CMGP_BASE_ADDRESS              (I2C_CMGP3_BASE_ADDRESS)

/* USI_CMGP4 */
#define USI8_CMGP_BASE_ADDRESS              (USI_CMGP4_BASE_ADDRESS)
#define I2C8_CMGP_BASE_ADDRESS              (USI_CMGP4_BASE_ADDRESS)
#define SPI4_CMGP_BASE_ADDRESS              (USI_CMGP4_BASE_ADDRESS)

/* I2C_CMGP4 */
#define USI9_CMGP_BASE_ADDRESS              (I2C_CMGP4_BASE_ADDRESS)
#define I2C9_CMGP_BASE_ADDRESS              (I2C_CMGP4_BASE_ADDRESS)

/* USI_CMGP5 */
#define USI10_CMGP_BASE_ADDRESS             (USI_CMGP5_BASE_ADDRESS)
#define I2C10_CMGP_BASE_ADDRESS             (USI_CMGP5_BASE_ADDRESS)
#define SPI5_CMGP_BASE_ADDRESS              (USI_CMGP5_BASE_ADDRESS)

/* I2C_CMGP5 */
#define USI11_CMGP_BASE_ADDRESS             (I2C_CMGP5_BASE_ADDRESS)
#define I2C11_CMGP_BASE_ADDRESS             (I2C_CMGP5_BASE_ADDRESS)

/* USI_CMGP6 */
#define USI12_CMGP_BASE_ADDRESS             (USI_CMGP6_BASE_ADDRESS)
#define I2C12_CMGP_BASE_ADDRESS             (USI_CMGP6_BASE_ADDRESS)
#define SPI6_CMGP_BASE_ADDRESS              (USI_CMGP6_BASE_ADDRESS)

/* I2C_CMGP6 */
#define USI13_CMGP_BASE_ADDRESS             (I2C_CMGP6_BASE_ADDRESS)
#define I2C13_CMGP_BASE_ADDRESS             (I2C_CMGP6_BASE_ADDRESS)

#define USI0_BASE_ADDRESS           USI0_CHUB_BASE_ADDRESS
#define USI1_BASE_ADDRESS           USI1_CHUB_BASE_ADDRESS
#define USI2_BASE_ADDRESS           USI2_CHUB_BASE_ADDRESS
#define USI3_BASE_ADDRESS           USI3_CHUB_BASE_ADDRESS
#define USI4_BASE_ADDRESS           USI4_CHUB_BASE_ADDRESS
#define USI5_BASE_ADDRESS           USI5_CHUB_BASE_ADDRESS
#define USI6_BASE_ADDRESS           USI0_CMGP_BASE_ADDRESS
#define USI7_BASE_ADDRESS           USI1_CMGP_BASE_ADDRESS
#define USI8_BASE_ADDRESS           USI2_CMGP_BASE_ADDRESS
#define USI9_BASE_ADDRESS           USI3_CMGP_BASE_ADDRESS
#define USI10_BASE_ADDRESS          USI4_CMGP_BASE_ADDRESS
#define USI11_BASE_ADDRESS          USI5_CMGP_BASE_ADDRESS
#define USI12_BASE_ADDRESS          USI6_CMGP_BASE_ADDRESS
#define USI13_BASE_ADDRESS          USI7_CMGP_BASE_ADDRESS
#define USI14_BASE_ADDRESS          USI8_CMGP_BASE_ADDRESS
#define USI15_BASE_ADDRESS          USI9_CMGP_BASE_ADDRESS
#define USI16_BASE_ADDRESS          USI10_CMGP_BASE_ADDRESS
#define USI17_BASE_ADDRESS          USI11_CMGP_BASE_ADDRESS
#define USI18_BASE_ADDRESS          USI12_CMGP_BASE_ADDRESS
#define USI19_BASE_ADDRESS          USI13_CMGP_BASE_ADDRESS

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
#define I2C14_BASE_ADDRESS          I2C8_CMGP_BASE_ADDRESS
#define I2C15_BASE_ADDRESS          I2C9_CMGP_BASE_ADDRESS
#define I2C16_BASE_ADDRESS          I2C10_CMGP_BASE_ADDRESS
#define I2C17_BASE_ADDRESS          I2C11_CMGP_BASE_ADDRESS
#define I2C18_BASE_ADDRESS          I2C12_CMGP_BASE_ADDRESS
#define I2C19_BASE_ADDRESS          I2C13_CMGP_BASE_ADDRESS

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
#define USI_CMGP5                   16
#define USI_I2C_CMGP5               17
#define USI_CMGP6                   18
#define USI_I2C_CMGP6               19
#define USI_CHANNEL_MAX             20

#define I2C_CHUB0                   0 // USI_CHUB0
#define I2C_CHUB1                   1 // USI_CHUB1
#define I2C_CHUB2                   2 // I2C_CHUB1
#define I2C_CHUB3                   3 // USI_CHUB2
#define I2C_CHUB4                   4 // USI_CHUB3
#define I2C_CHUB5                   5 // I2C_CHUB3
#define I2C_CHUB_CHANNEL_MAX        6
#define I2C_CMGP0                   I2C_CHUB_CHANNEL_MAX
#define I2C_CMGP1                   7
#define I2C_CMGP2                   8
#define I2C_CMGP3                   9
#define I2C_CMGP4                   10
#define I2C_CMGP5                   11
#define I2C_CMGP6                   12
#define I2C_CMGP7                   13
#define I2C_CMGP8                   14
#define I2C_CMGP9                   15
#define I2C_CMGP10                  16
#define I2C_CMGP11                  17
#define I2C_CMGP12                  18
#define I2C_CMGP13                  19
#define I2C_CHANNEL_MAX             20

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

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK	        0x07000000

#define OSC_CLOCK                   (24576000)

#define RTC_INTERRUPT_NO            RTC_CHUB_IRQn
#define SUPPORT_PWM_TIMER4          1
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)
#define CMGP_INTERRUPT_NUN          56
#define MCT_USE_RTC                 1
#define MCT_COMPENSATE_CNT          100
#define MCT_FREQ                    26000000

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
    CSIS0_VVALID_IRQn           = 2,
    CSIS1_VVALID_IRQn           = 3,
    CSIS2_VVALID_IRQn           = 4,
    CSIS3_VVALID_IRQn           = 5,
    CSIS4_VVALID_IRQn           = 6,

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
    I3C_CHUB_IRQn               = 22,
    USI_CHUB3_IRQn              = 23,
    I2C_CHUB3_IRQn              = 24,
    CMGP_REQ_IRQn               = 25,
    MIF_REQ_IRQn                = 26,
    PWM0_CHUB_IRQn              = 27,
    PWM1_CHUB_IRQn              = 28,
    PWM2_CHUB_IRQn              = 29,
    PWM3_CHUB_IRQn              = 30,
    PWM4_CHUB_IRQn              = 31,
    EXTINT_GPH0_0               = 32,
    EXTINT_GPH0_1               = 33,
    EXTINT_GPH0_2               = 34,
    EXTINT_GPH0_3               = 35,
    EXTINT_GPH0_4               = 36,
    EXTINT_GPH0_5               = 37,
    EXTINT_GPH0_6               = 38,
    EXTINT_GPH0_7               = 39,
    EXTINT_GPH1_0               = 40,
    EXTINT_GPH1_1               = 41,
    EXTINT_GPH1_2               = 42,
    EXTINT_GPH1_3               = 43,
    EXTINT_GPE0_0               = 44,
    EXTINT_GPE1_0               = 45,
    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif
