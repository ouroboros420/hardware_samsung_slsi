### CSP : Chip Specific Package

csp_common<I>**xxxx**</I>.h contains chip specific information.<br>

(xxxx is product code. For example: csp_common9630.h) <br>

The file is in <i>firmware/os/platform/exynos/inc/plat/csp/</I>
- SFR Base Address
- Channel Information of USI/SPI/I2C
- Interrupt
- Other information those are different from other product


Here is the contents of header file for S5E9630
<br>

```c
#define CMU_BASE_ADDRESS            0x4010000
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

#define GPIO_CHANNEL_MAX            33

#define RTC_SUPPORT_TICK0           1
#define RTC_TIMESTAMP_TICK          1

#define I2C_INTERRUPT_MASK          0x00000021

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
    EXTINT_GPH0_0               = 29,
    EXTINT_GPH0_1               = 30,
    EXTINT_GPH0_2               = 31,
    EXTINT_GPH0_3               = 32,
    EXTINT_GPH0_4               = 33,
    EXTINT_GPH0_5               = 34,
    EXTINT_GPH0_6               = 35,
    EXTINT_GPH0_7               = 36,
    //NA                        = 37,
    EXTINT_GPH1_0               = 38,
    EXTINT_GPH1_1               = 39,
    EXTINT_GPH1_2               = 40,
    EXTINT_GPH1_3               = 41,
    EXTINT_GPH2_0               = 42,
    EXTINT_GPH2_1               = 43,
    EXTINT_GPH2_2               = 44,
    EXTINT_GPH2_3               = 45,
    EXTINT_GPH2_4               = 46,
    EXTINT_GPH2_5               = 47,

    NUM_INTERRUPTS
}IRQn_Type;

```
