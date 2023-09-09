#
# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
include variant/$(VARIANT)/$(VARIANT)_conf.mk

#FLAGS += -DTEST_PRINT
# Carve out unnecessary codes. If needed, set it as YES
ifeq ($(PDMA_REQUIRED), YES)
FLAGS += -DPDMA_REQUIRED
endif

ifeq ($(TIMER_REQUIRED), YES)
FLAGS += -DTIMER_REQUIRED
endif

ifeq ($(UART_REQUIRED), YES)
FLAGS += -DUART_REQUIRED
endif

ifeq ($(UTC_REQUIRED), YES)
FLAGS += -DUTC_REQUIRED
endif

ifeq ($(DFS_SUPPORT), YES)
FLAGS += -DDFS_SUPPORT
endif

ifeq ($(DFS_BOOTING), YES)
FLAGS += -DDFS_BOOTING
endif

ifeq ($(SYSTIME_USING_RTC), YES)
FLAGS += -DSYSTICK_RTC
endif

ifeq ($(SYSTIME_USING_DLL), YES)
FLAGS += -DSYSTICK_DLL
endif

ifeq ($(SYSTIME_USING_RTC), YES)
RTC_REQUIRED = YES
endif

ifeq ($(RTC_REQUIRED), YES)
FLAGS += -DRTC_REQUIRED
endif

ifeq ($(PWM_REQUIRED), YES)
FLAGS += -DPWM_REQUIRED
endif

ifeq ($(SWINT_REQUIRED), YES)
FLAGS += -DSWINT_REQUIRED
endif

ifeq ($(I2CSPI_SFRDUMP_REQUIRED), YES)
FLAGS += -DI2CSPI_SFRDUMP_REQUIRED
endif

ifeq ($(HEAP_DEBUG), YES)
FLAGS += -DHEAP_DEBUG
endif

ifeq ($(EXYNOS_VSYNC), YES)
FLAGS += -DEXYNOS_VSYNC
endif

ifeq ($(EXYNOS_VSYNC_WORKAROUND), YES)
FLAGS += -DWORKAROUND_FOR_EXYNOS_VSYNC
endif

ifeq ($(LOCAL_POWERGATE), YES)
FLAGS += -DLOCAL_POWERGATE
endif

ifeq ($(USE_LOG_FLUSH_TRSHOLD), YES)
FLAGS += -DUSE_LOG_FLUSH_TRSHOLD
endif

ifeq ($(ALIVE_DVS_SUPPORT), YES)
FLAGS += -DALIVE_DVS_SUPPORT
endif

ifeq ($(HIFI_SENSOR_SUPPORT), YES)
FLAGS += -DHIFI_SENSOR_SUPPORT
endif

ifeq ($(MAILBOX_REQUIRED), YES)
FLAGS += -DCIPC_DEF_SUPPORT_BAAW_ALIGN
#FLAGS += -DCIPC_DEF_USER_GNSS
endif

FLAGS += -I. -fno-unwind-tables -fstack-reuse=all -ffunction-sections -fdata-sections
FLAGS += -Wl,--gc-sections -nostartfiles

FLAGS_os += -nostdlib

#platform bootloader
SRCS_bl += os/platform/$(PLATFORM)/src/bl.c os/platform/$(PLATFORM)/src/mailbox/mailboxDrv.c

#platform runtime
SRCS_platform +=  os/platform/$(PLATFORM)/src/crt_$(PLATFORM)$(CHIP).c


#platform drivers
SRCS_os += os/platform/$(PLATFORM)/src/platform.c \
	os/platform/$(PLATFORM)/src/mailbox/mailbox.c \
	os/platform/$(PLATFORM)/src/mailbox/mailboxOS.c \
	os/platform/$(PLATFORM)/src/mailbox/mailboxDrv.c \
	os/platform/$(PLATFORM)/src/mailbox/ipc_common.c \
	os/platform/$(PLATFORM)/src/mailbox/ipc_hw.c \
	os/platform/$(PLATFORM)/src/mailbox/ipc_chub.c \
	os/platform/$(PLATFORM)/src/mailbox/ipc_chub_chub.c \
	os/platform/$(PLATFORM)/src/mailbox/ipc_other.c \
	os/platform/$(PLATFORM)/src/csp/debug.c	\
	os/platform/$(PLATFORM)/src/cmu/cmu.c \
	os/platform/$(PLATFORM)/src/gpio/gpio.c \
	os/platform/$(PLATFORM)/src/i2c/i2c.c \
	os/platform/$(PLATFORM)/src/pwr/pwrDrv.c \
	os/platform/$(PLATFORM)/src/pwr/pwrDrvPwrGating.c \
	os/platform/$(PLATFORM)/src/pwr/pwrDrvPwrGatingCpu.s \
	os/platform/$(PLATFORM)/src/rtc/rtc_v2.c \
	os/platform/$(PLATFORM)/src/spi/spi.c \
	os/platform/$(PLATFORM)/src/usi/usi.c \
	os/platform/$(PLATFORM)/src/wdt/wdt.c \
	os/platform/$(PLATFORM)/src/csp/csp_os.c \
	os/platform/$(PLATFORM)/src/cpu/cpu.c \
	os/platform/$(PLATFORM)/src/cpu/swint.c \
	os/platform/$(PLATFORM)/src/apInt.c \
	os/platform/$(PLATFORM)/src/hostIntf.c \
	os/platform/$(PLATFORM)/src/cmgp/cmgp.c \
	os/platform/$(PLATFORM)/src/pwm/pwm.c \
	os/platform/$(PLATFORM)/src/sysreg/sysreg.c \
	os/platform/$(PLATFORM)/src/cpu/mpu.c \
	os/core/hostIntfmailbox.c

ifeq ($(DFS_SUPPORT), YES)
SRCS_os +=	os/platform/$(PLATFORM)/src/dfs/dfs.c
endif

ifeq ($(PDMA_REQUIRED), YES)
SRCS_os +=	os/platform/$(PLATFORM)/src/pdma/pdma.c \
	os/platform/$(PLATFORM)/src/pdma/pdmaOS.c \
	os/platform/$(PLATFORM)/src/pdma/pdmaDrv.c
endif

ifeq ($(TIMER_REQUIRED), YES)
SRCS_os +=	os/platform/$(PLATFORM)/src/timer/timer.c
endif

ifeq ($(UART_REQUIRED), YES)
SRCS_os +=	os/platform/$(PLATFORM)/src/uart/uart.c \
	os/platform/$(PLATFORM)/src/uart/uartOS.c \
	os/platform/$(PLATFORM)/src/uart/uartDrv.c
endif

ifeq ($(MBAPM_REQUIRED), YES)
FLAGS += -DMBAPM_REQUIRED
SRCS_os += os/platform/$(PLATFORM)/src/mailbox/mailboxApm.c
endif


ifeq ($(HEAP_DEBUG), YES)
SRCS_os += os/core/heapDebug.c
endif

ifeq ($(EXYNOS_VSYNC), YES)
SRCS_os += os/platform/$(PLATFORM)/src/csis/csisOS.c
endif

ifeq ($(SLIF_REQUIRED), YES)
SRCS_os += os/platform/$(PLATFORM)/src/slif/slif.c
endif

# CHUB-only UTC
SRCS_os += \
    os/drivers/exynos_test/exynos_test_api.c
FLAGS += -Ios/drivers/exynos_test

ifeq ($(UTC_REQUIRED), YES)
SRCS_os += \
    os/drivers/exynos_test/exynos_test.c \
    os/drivers/exynos_test/gpio_test.c \
    os/drivers/exynos_test/rtc_test.c \
    os/drivers/exynos_test/timer_test.c \
    os/drivers/exynos_test/spi_test.c \
    os/drivers/exynos_test/mem_test.c \
    os/drivers/exynos_test/cmu_test.c \
    os/drivers/exynos_test/wdt_test.c \
    os/drivers/exynos_test/i2c_test.c \
    os/drivers/exynos_test/scenario_test.c \
    os/drivers/exynos_test/aging_test.c
endif

ifeq ($(MCT_SUPPORT), YES)
SRCS_os += os/platform/$(PLATFORM)/src/mct/mct.c
endif

ifeq ($(MCT_SUPPORT), YES)
FLAGS += -DMCT_SUPPORT
endif

ifeq ($(GNSS_REQUIRED), YES)
FLAGS += -DMBGNSS_REQUIRED

SRCS_os += \
    os/platform/$(PLATFORM)/src/mailbox/mailboxGnssDrv.c \
    os/platform/$(PLATFORM)/src/mailbox/gnssIpc.c \
    os/platform/$(PLATFORM)/src/mailbox/gnssProtocol.c \
    os/platform/$(PLATFORM)/src/mailbox/gnssShmipc.c
endif

