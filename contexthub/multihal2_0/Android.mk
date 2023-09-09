LOCAL_PATH := $(call my-dir)

ifeq ($(NANOHUB_SENSORHAL2), true)

COMMON_CFLAGS := -Wall -Werror -Wextra

################################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := android.hardware.sensors@2.0-exynossubhal

LOCAL_CFLAGS += $(COMMON_CFLAGS)
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES += \
	hardware/samsung_slsi/contexthub/multihal2_0/exynos_subhal \
	hardware/samsung_slsi/contexthub/multihal2_0/hubconnection2

LOCAL_SRC_FILES := \
	$(call all-cpp-files-under, exynos_subhal) \
	../../../../$(NANOHUB_SENSORHAL_SENSORLIST)

LOCAL_HEADER_LIBRARIES := \
	android.hardware.sensors@2.0-multihal.header \
	android.hardware.sensors@2.X-shared-utils

LOCAL_SHARED_LIBRARIES := \
	android.hardware.sensors@1.0 \
	android.hardware.sensors@2.0 \
	android.hardware.sensors@2.0-ScopedWakelock \
	android.hardware.sensors@2.1 \
	libcutils \
	libfmq \
	libhardware \
	libhidlbase \
	libbase \
	liblog \
	libpower \
	libutils \
	libstagefright_foundation \
	libhubconnection2

LOCAL_STATIC_LIBRARIES := \
	android.hardware.sensors@2.X-multihal \
	android.hardware.sensors@1.0-convert \
	libaidlcommonsupport

LOCAL_CFLAGS += \
	-DLOG_TAG="\"ExynosSensorHal\"" \
	-DEXYNOS_CONTEXTHUB \
	-DSUB_HAL_NAME="\"ExynosSubHal\""

LOCAL_CFLAGS += -coverage
LOCAL_CFLAGS += -fprofile-dir="/data/coverage"
LOCAL_LDFLAGS := -coverage
LOCAL_NATIVE_COVERAGE :=true
GCOV_PREFIX := /data/coverage

include $(BUILD_SHARED_LIBRARY)

################################################################################

include $(CLEAR_VARS)

LOCAL_MODULE := libhubconnection2

LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES += \
	hardware/samsung_slsi/contexthub/multihal2_0/exynos_subhal

LOCAL_SRC_FILES := $(call all-cpp-files-under, hubconnection2)

LOCAL_HEADER_LIBRARIES := \
	android.hardware.sensors@2.0-multihal.header \
	android.hardware.sensors@2.X-shared-utils

LOCAL_SHARED_LIBRARIES := \
	android.hardware.sensors@1.0 \
	android.hardware.sensors@2.0 \
	android.hardware.sensors@2.0-ScopedWakelock \
	android.hardware.sensors@2.1 \
	libcutils \
	libfmq \
	libhardware \
	libhardware_legacy \
	liblog \
	libstagefright_foundation \
	libutils \
	libhidlbase \
	libbase

LOCAL_STATIC_LIBRARIES := \
	libhubutilcommon \
	android.hardware.sensors@1.0-convert \
	android.hardware.sensors@2.X-multihal


LOCAL_CFLAGS := \
	-DLOG_TAG="\"ExynosSensorHal\"" \
	-DEXYNOS_CONTEXTHUB

LOCAL_CFLAGS += -coverage
LOCAL_CFLAGS += -fprofile-dir="/data/coverage"
LOCAL_LDFLAGS := -coverage
LOCAL_NATIVE_COVERAGE :=true
GCOV_PREFIX := /data/coverage

include $(BUILD_SHARED_LIBRARY)

################################################################################

endif
