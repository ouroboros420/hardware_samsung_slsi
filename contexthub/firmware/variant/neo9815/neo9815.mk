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

#variant makefile for ASB9815


#ACC = NULL
#MAG = NULL
#ALS = NULL
#BARO = NULL
ACC = BMI160
MAG = AK09918
ALS = RPR0521
BARO = BMP280
#MGR = NULL

ACC_SENSOR = $(shell echo $(ACC) | tr a-z A-Z)
MAG_SENSOR = $(shell echo $(MAG) | tr a-z A-Z)
ALS_SENSOR = $(shell echo $(ALS) | tr a-z A-Z)
BARO_SENSOR = $(shell echo $(BARO) | tr a-z A-Z)
MANAGER = $(shell echo $(MGR) | tr a-z A-Z)


FLAGS += -DPLATFORM_HW_TYPE=0x9630
FLAGS += -DPLATFORM_HW_VER=0
FLAGS += -DPLATFORM_BL_VER=0x1126
FLAGS += -DSUPPORT_MULTIOS
#FLAGS += -DDEBUG_UART_PIN=16         #GPIOB0 is debug uart at 2MBps
FLAGS += -DNO_FLASH
FLAGS += -Wno-unused-variable -Wno-unused-but-set-variable

ifeq ($(EXYNOS_CONTEXTHUB), YES)
FLAGS += -DEXYNOS_CONTEXTHUB
endif

FLAGS += -I$(VARIANT_PATH)/inc

FLAGS += -ldhry -L$(VARIANT_PATH)/src

#If APM handshaking is implemented, turn it on
#FLAGS += -DAPM_HANDSHAKING

#If you want to test firmware only, turn it on
#FLAGS += -DFIRMWARE_ONLY

#board configuration shims
#SRCS_os += $(VARIANT_PATH)/src/i2c.c
#SRCS_os += $(VARIANT_PATH)/src/spi.c
#SRCS_os += $(VARIANT_PATH)/src/cmgpErd9630.c
#SRCS_os += $(VARIANT_PATH)/src/gpioErd9630.c
#SRCS_os += $(VARIANT_PATH)/src/spiErd9630.c
SRCS_os += $(VARIANT_PATH)/src/boardInit.c
SRCS_os += $(VARIANT_PATH)/src/crt_$(PLATFORM)$(CHIP).c
SRCS_os += $(VARIANT_PATH)/src/Main.c

FLAGS += -D "BASE_COMMIT=\"$(COMMIT_LOG)\""
FLAGS += -D "BUILDER=\"$(BUILDER)\""
FLAGS += -D "SOCNAME=\"$(SOCNAME)\""
FLAGS += -DEXYNOS_LOG_TO_KERNEL
FLAGS += -D$(BUILD_VARIANT_NAME)
#keys
$(infoincluded NEO9815 variant)
