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
SRCS_os += $(SRCS_platform)

FLAGS += -D "BASE_COMMIT=\"$(COMMIT_LOG)\""
FLAGS += -D "BUILDER=\"$(BUILDER)\""
FLAGS += -D "SOCNAME=\"$(SOCNAME)\""
FLAGS += -DEXYNOS_LOG_TO_KERNEL
FLAGS += -D$(BUILD_VARIANT_NAME)
FLAGS += -DPACKET_SIZE_MAX=$(PACKET_SIZE_MAX)
FLAGS += -DCIPC_DATA_AP2CHUB_SIZE=$(CIPC_DATA_AP2CHUB_SIZE)
FLAGS += -DLOGBUF_NUM=$(LOGBUF_NUM)
FLAGS += -DCIPC_START_OFFSET=$(CIPC_START_OFFSET)

ifeq ($(MANAGER), EXYNOS)
SRCS_os += $(VARIANT_PATH)/src/sensormanager.c
endif

ifeq ($(ALS_SENSOR), RPR0521)
SRCS_os += os/drivers/rohm_rpr0521/rohm_rpr0521.c
endif

ifeq ($(ALS_SENSOR), TMD3725)
SRCS_os += os/drivers/ams_tmd3725/ams_tmd3725.c
endif

ifeq ($(MAG_SENSOR), LIS3MDL)
SRCS_os += os/drivers/st_lis3mdl/st_lis3mdl.c
endif

ifeq ($(MAG_SENSOR), AK09918)
SRCS_os += os/drivers/akm_ak09918/akm_ak09918.c
ifeq ($(MAG_CALIBRATION), YES)
FLAGS += -DAK09918_CAL_ENABLED -lm
SRCS_os += os/algos/calibration/magnetometer/mag_cal/mag_cal.c
SRCS_os += os/algos/calibration/diversity_checker/diversity_checker.c
SRCS_os += os/algos/common/math/kasa.c
endif
endif

# Fusion algorithms
SRCS_os += os/algos/fusion.c \
	os/algos/common/math/mat.c \
	os/algos/common/math/quat.c \
	os/algos/common/math/vec.c \
	os/algos/time_sync.c

ifeq ($(ACC_SENSOR), LSM6DSM)
SRCS_os += os/drivers/st_lsm6dsm/st_lsm6dsm.c
endif

ifeq ($(ACC_SENSOR), BMI160)
  ifeq ($(HIFI_SENSOR_SUPPORT), YES)
    FLAGS += -DSUPPORT_1600HZ
    FLAGS += -DSUPPORT_HIFI_SENSORS
  endif
  SRCS_os += \
      os/algos/time_sync.c \
      os/drivers/bosch_bmi160/bosch_bmi160.c
endif

ifeq ($(ACC_SENSOR), BMI26X)
SRCS_os += os/drivers/bosch_bmi26x/bosch_bmi26x.c
endif

# To enable calibration routines of bmi160
# if you choose YES, have SENS_TYPE_ACCEL_BIAS defined in sensType.h
ifeq ($(ACCEL_CALIBRATION), YES)
    FLAGS += -DACCEL_CAL_ENABLED
    FLAGS += -DGYRO_CAL_ENABLED
    FLAGS += -DOVERTEMPCAL_ENABLED
    SRCS_os += \
      os/algos/calibration/accelerometer/accel_cal.c \
      os/algos/calibration/gyroscope/gyro_cal.c \
      os/algos/calibration/gyroscope/gyro_stillness_detect.c \
#      os/algos/calibration/magnetometer/mag_cal.c
#      os/algos/calibration/over_temp/over_temp_cal.c \
#      os/algos/calibration/common/calibration_data.c \
#      os/algos/calibration/common/diversity_checker.c
#      os/algos/calibration/common/sphere_fit_calibration.c
endif

#ifeq ($(DEBUG_MODE), YES)
#SRCS_os += os/drivers/wakeup_test/wakeup_test.c
#endif

ifeq ($(BARO_SENSOR), BMP280)
SRCS_os += os/drivers/bosch_bmp280/bosch_bmp280.c
endif

ifeq ($(BARO_SENSOR), BMP380)
SRCS_os += os/drivers/bosch_bmp380/bosch_bmp380.c
endif

ifeq ($(EXYNOS_VSYNC), YES)
  ifeq ($(EXYNOS_VSYNC_WORKAROUND), YES)
    SRCS_os += os/drivers/vsync/vsync_eint.c
  else
    SRCS_os += os/drivers/vsync/vsync.c
  endif
endif

ifeq ($(EXYNOS_FUSION), YES)
    EXYNOS_FUSION_BUILD := REL
    FLAGS += -Ios/drivers/exynos_fusion/inc
    FLAGS += -DTARGET_BOARD_$(shell echo $(VARIANT) | tr a-z A-Z)
ifeq ($(EXYNOS_FUSION_BUILD), DEV)
    include os/drivers/exynos_fusion/src/build.mk
else
    FLAGS += -lexynos_fusion -lm -L$(VARIANT_PATH)/src
endif
    SRCS_os += os/drivers/exynos_fusion/exynos_fusion_interface.c
endif

$(info build variant = $(BUILD_VARIANT_NAME))
$(info included Exynos variant:$(VARIANT) base:$(VARIANT_BASE) path:$(VARIANT_PATH))
