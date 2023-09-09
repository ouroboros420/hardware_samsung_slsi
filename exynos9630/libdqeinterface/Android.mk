LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -Wno-unused-function -DLOG_TAG=\"libdqeinterface\"
LOCAL_SRC_FILES := \
    dqeInterface.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libutils \
    libcutils \
    libxml2

LOCAL_C_INCLUDES := \
    external/libxml2/include \

LOCAL_MODULE := libdqeInterface
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libdqeInterface_headers
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDE := \
    $(LOCAL_PATH)/include

LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/include

include $(BUILD_HEADER_LIBRARY)
