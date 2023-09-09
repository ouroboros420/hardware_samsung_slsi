LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CLFLAGS += -Wno-unused-function
LOCAL_SRC_FILES := \
	usage.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libion \
	libsync \
	libion_exynos \

LOCAL_STATIC_LIBRARIES := \
	libsbwc-dpu \

LOCAL_HEADER_LIBRARIES := \
	libsystem_headers \

LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/exynos/include \
	$(TOP)/hardware/samsung_slsi/graphics/exynos2100/libhwc2.1 \
	$(TOP)/hardware/samsung_slsi/graphics/base/libhwc2.1/driver_header \

LOCAL_MODULE := sbwc_dpu_usage
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
