LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wno-unused-function -DLOG_TAG=\"libhdrinterface\"
LOCAL_SRC_FILES := \
    libhdr_plugin_exynos9630.cpp

LOCAL_HEADER_LIBRARIES := libhdrinterface_header libsystem_headers libhdr10p_meta_interface_header
LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    libxml2

LOCAL_C_INCLUDES := \
    hardware/samsung_slsi/exynos/include \

LOCAL_MODULE := libhdr_plugin_exynos9630
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
