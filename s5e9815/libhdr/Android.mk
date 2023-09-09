LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_SOC_BASE), s5e9815)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -Werror -Wno-narrowing
LOCAL_CFLAGS += -Wno-unused-function -DLOG_TAG=\"libhdrinterface\"
ifeq ($(BOARD_USE_FULL_ST2094_40), true)
LOCAL_CFLAGS += -DUSE_FULL_ST2094_40
endif

LOCAL_SRC_FILES := \
    libhdr10p.cpp \
    libhdr_plugin_exynos9815.cpp

LOCAL_HEADER_LIBRARIES := \
    libsystem_headers \
    libhdrinterface_header \
    libhdr10p_meta_interface_header

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    libxml2 \

LOCAL_C_INCLUDES := \
    hardware/samsung_slsi/exynos/include

LOCAL_MODULE := libhdr_plugin_exynos9815
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
endif
