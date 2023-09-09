LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wno-unused-function -DLOG_TAG=\"libhdrinterface\"
LOCAL_SRC_FILES := \
    hdrInterface.cpp

LOCAL_HEADER_LIBRARIES := libhdrInterface_headers
LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    libxml2

LOCAL_C_INCLUDES := \
    external/libxml2/include \

LOCAL_MODULE := libhdrInterface
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)
