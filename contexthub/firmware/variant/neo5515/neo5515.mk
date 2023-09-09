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

#variant makefile for ERD9815


FLAGS += -DNO_FLASH
FLAGS += -Wno-unused-variable -Wno-unused-but-set-variable

ifeq ($(EXYNOS_CONTEXTHUB), YES)
FLAGS += -DEXYNOS_CONTEXTHUB
endif

FLAGS += -I$(VARIANT_PATH)/inc
FLAGS += -I$(VARIANT_PATH)/../common/inc

#If APM handshaking is implemented, turn it on
#FLAGS += -DAPM_HANDSHAKING

#If you want to test firmware only, turn it on
#FLAGS += -DFIRMWARE_ONLY

#board configuration shims
SRCS_os += $(VARIANT_PATH)/../common/src/i2c.c
SRCS_os += $(VARIANT_PATH)/../common/src/boardInit.c
SRCS_os += $(VARIANT_PATH)/../common/src/cmgp.c
SRCS_os += $(VARIANT_PATH)/../common/src/gpio.c
SRCS_os += $(VARIANT_PATH)/src/spi.c
SRCS_os += $(VARIANT_PATH)/src/crt_$(PLATFORM)$(CHIP).c
SRCS_os += $(VARIANT_PATH)/src/Main.c
SRCS_os += $(VARIANT_PATH)/src/sram_test.c

FLAGS += -D "BASE_COMMIT=\"$(COMMIT_LOG)\""
FLAGS += -D "BUILDER=\"$(BUILDER)\""
FLAGS += -D "SOCNAME=\"$(SOCNAME)\""
FLAGS += -DEXYNOS_LOG_TO_KERNEL
FLAGS += -D$(BUILD_VARIANT_NAME)
FLAGS += -DDEF_SUPPORT_CPU_UTILIZATION
FLAGS += -ldhry -L$(VARIANT_PATH)/src

$(info build variant = $(BUILD_VARIANT_NAME))
$(info included Exynos variant:$(VARIANT) base:$(VARIANT_BASE) path:$(VARIANT_PATH))
