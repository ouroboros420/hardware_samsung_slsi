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

M3DEBUG ?= m3debug

OBJDUMP := $(CROSS_COMPILE)objdump
CPPFILT := $(CROSS_COMPILE)c++filt

BL_FILE = $(OUT)/bl.unchecked.bin
OS_FILE = $(OUT)/os.checked.bin

TOOLS = tools

DELIVERABLES += showsizes

include os/platform/$(PLATFORM)/$(SOCNAME).mk

ifeq ($(BLOPTION), NOBL)
BL_FILE :=
endif

#extra deps
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/cmgp/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/cmsis/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/cmu/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/cpu/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/csp/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/gpio/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/i2c/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/mailbox/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/pwm/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/pwr/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/rtc/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/spi/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/sysreg/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/usi/*.h)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/wdt/*.h)
ifeq ($(ADC_REQUIRED), YES)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/adc/*.h)
endif
ifeq ($(PDMA_REQUIRED), YES)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/pdma/*.h)
endif
ifeq ($(TIMER_REQUIRED), YES)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/timer/*.h)
endif
ifeq ($(UART_REQUIRED), YES)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/uart/*.h)
endif
ifeq ($(EXYNOS_VSYNC), YES)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/csis/*.h)
endif
ifeq ($(SLIF_REQUIRED), YES)
DEPS += $(wildcard os/platform/$(PLATFORM)/inc/plat/slif/*.h)
endif

#linker script
LKR_os = os/platform/$(PLATFORM)/lkr/$(VARIANT).os.lkr
ifneq ($(BLOPTION), NOBL)
LKR_bl = os/platform/$(PLATFORM)/lkr/$(VARIANT).bl.lkr
OSFLAGS_bl += -Wl,-T $(LKR_bl)
endif
LKR_map = os/platform/$(PLATFORM)/lkr/$(VARIANT).map.lkr
OSFLAGS_os += -Wl,-T $(LKR_os)
DEPS += $(LKR_os) $(LKR_bl) $(LKR_map)

#platform flags
#PLATFORM_HAS_HARDWARE_CRC = false
FLAGS += -D$(SOCNAME)
FLAGS += -DEXYNOS_CONTEXTHUB
FLAGS += -DPLATFORM_HW_VER=0 -DUSE_NON_FLASH -D$(VARIANT) -D$(PLATFORM) -D$(SOCNAME)
#FLAGS += -DSFR_SAVE_RESTORE

#OS flags
FLAGS += -DSEOS

#IPC flags
FLAGS += -DDEF_IPC_DEBUG

ifeq ($(IF_INTERRUPT_MODE), YES)
FLAGS += -DDEF_IF_INTERRUPT_MODE
endif

ifeq ($(DFS_SUPPORT), YES)
FLAGS += -DDEF_IDLE_MONITOR
endif

ifeq ($(DEBUG_MODE), YES)
#FLAGS += -DHEAP_DEBUG
FLAGS += -DDEF_DEBUG_MODE
FLAGS += -DDEF_IPC_TIMESTAMP
FLAGS += -DDEF_IDLE_MONITOR
FLAGS += -DDEF_IPC_MONITOR
FLAGS += -DDEF_PERF_MONITOR
endif

# Default HWACG is off
#FLAGS += -DHWACG_OFF

#platform-specific rules
OBJCOPY_PARAMS = -I elf32-littlearm -O binary

$(OUT)/bl.%.bin : $(OUT)/bl.%.elf
	$(OBJCOPY) -j .bl -j .blrodata -j .eedata $(OBJCOPY_PARAMS) $< $@
	$(TOOLS)/Makepad_528 $@
	$(TOOLS)/cslv/cslv_64 -infile $@ -outfile $@ -sign_type 4 -key_type 2 -rb_count 0 -dynamic_length no

$(OUT)/os.%.bin : $(OUT)/os.%.elf
	$(OBJCOPY) -j .data -j .text $(OBJCOPY_PARAMS) $< $@
	$(OBJDUMP) -t $< | $(CPPFILT) | sort > $(OUT)/os.sym.sorted
	$(TOOLS)/sym2bin.py $(OUT)/os.sym.sorted $(OUT)/os.sym.bin
	cat $(OUT)/os.sym.bin >> $@
	$(TOOLS)/Makepad_528 $@
	$(TOOLS)/cslv/cslv_64 -infile $@ -outfile $@ -sign_type 4 -key_type 2 -rb_count 0 -dynamic_length no


showsizes: $(OUT)/os.unchecked.elf
	os/platform/$(PLATFORM)/misc/showsizes.sh $<

$(info Included EXYNOS platfrom)
