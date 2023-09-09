/*----------------------------------------------------------------------------
 *      Exynos SoC  -  CSP
 *----------------------------------------------------------------------------
 *      Name:    csp_common9935.h
 *      Purpose: To define common definitions for S5E9935
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

#ifndef __CSP_COMMON9935_H__
#define __CSP_COMMON9935_H__

#if defined(__GNUC__) || defined(__ICCARM__)
#include <stdint.h>
#endif

#define CHUB_BASE_ADDRESS                   0x40100000
#define CMU_BASE_ADDRESS                    (CHUB_BASE_ADDRESS)             //0x1480_0000
#define SERIAL_LIF_US_PROX                  (CHUB_BASE_ADDRESS + 0x10000)   //0x1481_0000
#define WDT_BASE_ADDRESS                    (CHUB_BASE_ADDRESS + 0x30000)   //0x1483_0000
#define TIMER_BASE_ADDRESS                  (CHUB_BASE_ADDRESS + 0x40000)   //0x1484_0000
#define PWM_BASE_ADDRESS                    (CHUB_BASE_ADDRESS + 0x60000)   //0x1486_0000
#define MAILBOX_DNC_BASE_ADDRESS            (CHUB_BASE_ADDRESS + 0x80000)   //0x1488_0000
#define MAILBOX_ABOX_BASE_ADDRESS           (CHUB_BASE_ADDRESS + 0x90000)   //0x1489_0000
#define COMB_WAKEUP_SRC_BASE_ADDRESS        (CHUB_BASE_ADDRESS + 0xE0000)   //0x148E_0000
#define DUMP_GPR_CHUB_BASE_ADDRESS          (CHUB_BASE_ADDRESS + 0xF0000)   //0x148F_0000
#define SYSREG_CHUB2AP_BASE_ADDRESS         (CHUB_BASE_ADDRESS + 0x120000)  //0x1492_0000
#define SYSREG_CHUB2APM_BASE_ADDRESS        (CHUB_BASE_ADDRESS + 0x130000)  //0x1493_0000
#define USI_CHUB0_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x150000)  //0x1495_0000
#define SPI_I2C_CHUB0_BASE_ADDRESS          (CHUB_BASE_ADDRESS + 0x1B0000)  //0x149B_0000
#define USI_CHUB1_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x160000)  //0x1496_0000
#define SPI_I2C_CHUB1_BASE_ADDRESS          (CHUB_BASE_ADDRESS + 0x180000)  //0x1498_0000
#define USI_CHUB2_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x170000)  //0x1497_0000
#define USI_CHUB3_BASE_ADDRESS              (CHUB_BASE_ADDRESS + 0x1C0000)  //0x149C_0000
#define I2C_CHUB_BASE_ADDRESS               (CHUB_BASE_ADDRESS + 0x190000)  //0x1499_0000
#define I3C_CHUB_BASE_ADDRESS               (CHUB_BASE_ADDRESS + 0x1A0000)  //0x149A_0000
#define GPIO_BASE_ADDRESS                   (CHUB_BASE_ADDRESS + 0x1D0000)  //0x149D_0000
#define SYSREG_BASE_ADDRESS                 (CHUB_BASE_ADDRESS + 0x1E0000)  //0x149E_0000

#define APM_BASE_ADDRESS                    0x40300000
/*
DBGCORE_UART_usi    0x14C0_0000
DBGCORE_UART_uart   0x14C0_0000        
SPC_ALIVE           0x14C1_0000
APM_DTA             0x14C2_0000
MAILBOX_APM_AUD     0x14C3_0000
MAILBOX_APM_GNSS    0x14C4_0000
MAILBOX_AP_GNSS     0x14C5_0000
TIMER1_ALIVE        0x14C6_0000
MAILBOX_APM_VTS     0x14C7_0000
WDT_ASM             0x14C8_0000
TIMER_ASM           0x14C9_0000
DUMP_GPR_ASM        0x14CA_0000
MAILBOX_AP_ASM      0x14CB_0000
MAILBOX_APM_ASM     0x14CC_0000
MAILBOX_ASM_CP      0x14CD_0000
BAAW_ASM_1          0x14CE_0000
BAAW_ASM_2          0x14CE_0010
BAAW_ASM_3          0x14CE_0020
BAAW_ASM_4          0x14CE_0030
BAAW_ASM_5          0x14CE_0040
BAAW_ASM_6          0x14CE_0050
BAAW_ASM_7          0x14CE_0060
ECU_ALIVE           0x14CF_0000
*/
                
#define CMGP_BASE_ADDRESS                   0x40600000
#define CMU_CMGP_BASE_ADDRESS               (CMGP_BASE_ADDRESS)             //0x14e0_0000
#define SYSREG_CMGP_BASE_ADDRESS            (CMGP_BASE_ADDRESS + 0x20000)   //0x14e2_0000
#define GPIO_CMGP_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x30000)   //0x14e3_0000
#define SYSREG_CMGP2CHUB_BASE_ADDRESS       (CMGP_BASE_ADDRESS + 0x90000)   //0x14e9_0000
#define USI_CMGP0_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x100000)  //0x14f0_0000
#define SPI_I2C_CMGP0_BASE_ADDRESS          (CMGP_BASE_ADDRESS + 0x110000)  //0x14f1_0000
#define USI_CMGP1_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x120000)  //0x14f2_0000
#define SPI_I2C_CMGP1_BASE_ADDRESS          (CMGP_BASE_ADDRESS + 0x130000)  //0x14f3_0000
#define USI_CMGP2_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x140000)  //0x14f4_0000
#define I2C_CMGP2_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x150000)  //0x14f5_0000
#define USI_CMGP3_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x160000)  //0x14f6_0000
#define I2C_CMGP3_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x170000)  //0x14f7_0000
#define USI_CMGP4_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x180000)  //0x14f8_0000
#define I2C_CMGP4_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x190000)  //0x14f9_0000
#define USI_CMGP5_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1A0000)  //0x14fA_0000
#define I2C_CMGP5_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1B0000)  //0x14fB_0000
#define USI_CMGP6_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1C0000)  //0x14fC_0000
#define I2C_CMGP6_BASE_ADDRESS              (CMGP_BASE_ADDRESS + 0x1D0000)  //0x14fD_0000

#define UFD_BASE_ADDRESS                    0x40400000
/*
CMU_UFD             0x1500_0000
D_TZPC_UFD          0x1501_0000
SYSREG_UFD          0x1502_0000
S2PC_UFD            0x1504_0000
VGEN_LITE_D_UFD_NS  0x1505_0000
VGEN_LITE_D_UFD_S   0x1505_0000
SYSMMU_UFD_S0_S1_NS 0x1506_0000
SYSMMU_UFD_S0_S1_S  0x1507_0000
SYSMMU_UFD_S0_S2_NS 0x1508_0000
SYSMMU_UFD_S0_S2_S  0x1508_0000
PPMU_D_UFD          0x1509_0000
UFD                 0x1510_0000
PDMA_UFD            0x1511_0000
I2C_UFD_USI         0x1512_0000
I2C_UFD_I2C         0x1512_0000
I3C_UFD             0x1513_0000
BAAW1_D_UFDDNC      0x1514_0000
BAAW2_D_UFDDNC      0x1514_0010
BAAW3_D_UFDDNC      0x1514_0020
BAAW4_D_UFDDNC      0x1514_0030
SYSREG_UFD_SECURE   0x1515_0000
*/
    
#define CHUBVTS_BASE_ADDRESS                0x40A00000
#define DMAILBOX_BASE_ADDRESS               (CHUBVTS_BASE_ADDRESS + 0xC0000) //0x155C_0000
#define MAILBOX_VTS_BASE_ADDRESS            (CHUBVTS_BASE_ADDRESS + 0xD0000) //0x155D_0000

#define ALIVE_BASE_ADDRESS                  0x40800000
#define MCT_ALIVE_BASE_ADDRESS              (ALIVE_BASE_ADDRESS + 0x90000)  //0x1589_0000
#define RTC_0_BASE_ADDRESS                  (ALIVE_BASE_ADDRESS + 0x110000) //0x1591_0000
#define RTC_1_BASE_ADDRESS                  (ALIVE_BASE_ADDRESS + 0x120000) //0x1592_0000
#define RTC_BASE_ADDRESS                    RTC_0_BASE_ADDRESS
    #define RTC_TIMESTAMP_BASE_ADDRESS      RTC_0_BASE_ADDRESS
    #define RTC_INTERRUPT_BASE_ADDRESS      RTC_1_BASE_ADDRESS
#define MAILBOX_CP_BASE_ADDRESS             (ALIVE_BASE_ADDRESS + 0x1D0000) //0x159D_0000
#define MAILBOX_APM_BASE_ADDRESS            (ALIVE_BASE_ADDRESS + 0x1E0000) //0x159E_0000
#define MAILBOX_AP_BASE_ADDRESS             (ALIVE_BASE_ADDRESS + 0x1F0000) //0x159F_0000
#define MAILBOX_GNSS_BASE_ADDRESS           (ALIVE_BASE_ADDRESS + 0x0E0000) //0x158E_0000
#define GNSS_SHMIPC_BASE_ADDRESS            0x0017F000

/*
CMU_ALIVE               0x1580_0000
D_TZPC_ALIVE            0x1581_0000
SYSREG_ALIVE            0x1582_0000
TIMER0_ALIVE            0x1583_0000
WDT_APM0                0x1584_0000
GPIO_ALIVE              0x1585_0000
PMU_ALIVE               0x1586_0000
PMU_INTR_GEN            0x1587_0000
INTMEM                  0x1588_0000
VGEN_LITE_ALIVE_NS      0x158A_0000
VGEN_LITE_ALIVE_S       0x158A_0000
CLKMON                  0x158B_0000
DUMP_GPR_APM0           0x158C_0000
MAILBOX_AP_DBGCORE      0x158D_0000
MAILBOX_GNSS_CHUB       0x158E_0000
MAILBOX_SHARED_SRAM     0x158F_0000
RTC_S                   0x1590_0000
INTCOMB_VGPIO2APACHE    0x1593_0000
INTCOMB_VGPIO2APM       0x1594_0000
INTCOMB_VGPIO2PMU       0x1595_0000
SPMI_MASTER_PMIC        0x1596_0000
MAILBOX_APM_CP          0x1597_0000
MAILBOX_APM_CP_1        0x1598_0000
MAILBOX_APM_AP          0x1599_0000
MAILBOX_AP_CP_S         0x159A_0000
MAILBOX_AP_CP           0x159B_0000
MAILBOX_CP_GNSS         0x159C_0000
*/

#define SRAM_MIU_UFD_BASE_ADDRESS           0x41B30000

#define CSIS_BASE_ADDRESS                   0x40B00000

/* USI_CHUB0 */
#define USI0_CHUB_BASE_ADDRESS              (USI_CHUB0_BASE_ADDRESS)
#define I2C0_CHUB_BASE_ADDRESS              (USI_CHUB0_BASE_ADDRESS)
#define SPI0_CHUB_BASE_ADDRESS              (USI_CHUB0_BASE_ADDRESS)

/* SPI_I2C_CHUB0 */
#define USI1_CHUB_BASE_ADDRESS              (SPI_I2C_CHUB0_BASE_ADDRESS)
#define I2C1_CHUB_BASE_ADDRESS              (SPI_I2C_CHUB0_BASE_ADDRESS)

/* USI_CHUB1 */
#define USI2_CHUB_BASE_ADDRESS              (USI_CHUB1_BASE_ADDRESS)
#define I2C2_CHUB_BASE_ADDRESS              (USI_CHUB1_BASE_ADDRESS)
#define SPI1_CHUB_BASE_ADDRESS              (USI_CHUB1_BASE_ADDRESS)

/* SPI_I2C_CHUB1 */
#define USI3_CHUB_BASE_ADDRESS              (SPI_I2C_CHUB1_BASE_ADDRESS)
#define I2C3_CHUB_BASE_ADDRESS              (SPI_I2C_CHUB1_BASE_ADDRESS)

/* USI_CHUB2 */
#define USI4_CHUB_BASE_ADDRESS              (USI_CHUB2_BASE_ADDRESS)
#define I2C4_CHUB_BASE_ADDRESS              (USI_CHUB2_BASE_ADDRESS)
#define SPI2_CHUB_BASE_ADDRESS              (USI_CHUB2_BASE_ADDRESS)

/* USI_CHUB3 */
#define USI5_CHUB_BASE_ADDRESS              (USI_CHUB3_BASE_ADDRESS)
#define I2C5_CHUB_BASE_ADDRESS              (USI_CHUB3_BASE_ADDRESS)
#define SPI3_CHUB_BASE_ADDRESS              (USI_CHUB3_BASE_ADDRESS)

/* I2C_CHUB */
#define USI6_CHUB_BASE_ADDRESS              (I2C_CHUB_BASE_ADDRESS)
#define I2C6_CHUB_BASE_ADDRESS              (I2C_CHUB_BASE_ADDRESS)


/* USI_CMGP0 */
#define USI0_CMGP_BASE_ADDRESS              (USI_CMGP0_BASE_ADDRESS)
#define I2C0_CMGP_BASE_ADDRESS              (USI_CMGP0_BASE_ADDRESS)
#define SPI0_CMGP_BASE_ADDRESS              (USI_CMGP0_BASE_ADDRESS)

/* SPI_I2C_CMGP0 */
#define USI1_CMGP_BASE_ADDRESS              (SPI_I2C_CMGP0_BASE_ADDRESS)
#define I2C1_CMGP_BASE_ADDRESS              (SPI_I2C_CMGP0_BASE_ADDRESS)

/* USI_CMGP1 */
#define USI2_CMGP_BASE_ADDRESS              (USI_CMGP1_BASE_ADDRESS)
#define I2C2_CMGP_BASE_ADDRESS              (USI_CMGP1_BASE_ADDRESS)
#define SPI1_CMGP_BASE_ADDRESS              (USI_CMGP1_BASE_ADDRESS)

/* SPI_I2C_CMGP1 */
#define USI3_CMGP_BASE_ADDRESS              (SPI_I2C_CMGP1_BASE_ADDRESS)
#define I2C3_CMGP_BASE_ADDRESS              (SPI_I2C_CMGP1_BASE_ADDRESS)

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
#define I3C1_BASE_ADDRESS           I3C_UFD_BASE_ADDRESS

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
#define USI_CHANNEL_MAX             21

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
#define I2C_CHANNEL_MAX             21

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
#define I3C_UFD0                    1
#define I3C_CHANNEL_MAX             2

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          0
#define RTC_INTERRUPT_TICK          0

#define I2C_INTERRUPT_MASK	        0x07000000

#define OSC_CLOCK                   (24576000)

#define RTC_INTERRUPT_NO            RTC_CHUB_IRQn
#define SUPPORT_PWM_TIMER4          0
#define USE_PWM_AS_TIMESTAMP        0
#define TS_PWM_ID                   PWM_TIMER3
#define MAILBOX_VERSION             (0x2)
#define CMGP_INTERRUPT_NUN          54
#define MCT_USE_RTC                 1
#define MCT_COMPENSATE_CNT          100
#define MCT_FREQ                    25600000
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
    DNC_WAKEUP_REQ_IRQn         = 17,
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
    UFD_PMDA_IRQn               = 56,
    UFD0_IRQn                   = 57,
    UFD1_IRQn                   = 58,
    UFD2_IRQn                   = 59,
    EXTINT_GPH5_5               = 60,
    MB_DNC_IRQn                 = 61,
    MB_ABOX_IRQn                = 62,
    SPI_I2C_CHUB00_IRQn         = 63,
    SPI_I2C_CHUB01_IRQn         = 64,
    UFD_I3C_IRQn                = 65,
    CSIS0_IRQn                  = 66,
    CSIS1_IRQn                  = 67,
    CSIS2_IRQn                  = 68,
    CSIS3_IRQn                  = 69,
    CSIS4_IRQn                  = 70,
    CSIS5_IRQn                  = 71,
    CSIS6_IRQn                  = 72,
    EXTINT_GPH5_6               = 73,
    AUD_WAKEUP_REQ_IRQn         = 74,
    INTREQ_DMAILBOX_SEMA_IRQn   = 75,
    INTREQ_M_DMAILBOX_SEMA_IRQn = 76,
    INT_RX_REQ0_DMAILBOX_IRQn   = 77,
    INT_RX_REQ1_DMAILBOX_IRQn   = 78,
    EXTINT_GPB5_0               = 79,
    EXTINT_GPB5_1               = 80,
    EXTINT_GPB5_2               = 81,
    EXTINT_GPB5_3               = 82,
    SLIF_US_PROX_IRQn           = 83,
    UFD_I2C_IRQn                = 84,
    SYSREG_TCXO_IRQn            = 85,
    NUM_INTERRUPTS
}IRQn_Type;

enum RtcTickId {
    RtcTickId0,
    RtcTickId1 = RtcTickId0,
    RtcTickIdMax,
};

#endif
