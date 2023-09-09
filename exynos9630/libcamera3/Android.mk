# Copyright (C) 2017 The Android Open Source Project
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

LOCAL_PATH:= $(call my-dir)

#################
# libexynoscamera3

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_PROPRIETARY_MODULE := true

LOCAL_STATIC_LIBRARIES := android.hardware.camera.common@1.0-helper
LOCAL_SHARED_LIBRARIES:= libutils libcutils liblog libui libcamera_metadata
LOCAL_SHARED_LIBRARIES += libexynosutils libhwjpeg libexynosv4l2 libion_exynos libsync libcsc libacryl
LOCAL_SHARED_LIBRARIES += libdl libexynoscamera_plugin
LOCAL_SHARED_LIBRARIES += android.hardware.graphics.allocator@2.0 android.hardware.graphics.mapper@2.0 libGrallocWrapper

LOCAL_CFLAGS += -Wno-error=date-time
LOCAL_CFLAGS += -Wno-overloaded-virtual
LOCAL_CFLAGS += -DGAIA_FW_BETA
LOCAL_CFLAGS += -DMAIN_CAMERA_SENSOR_NAME=$(BOARD_BACK_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_CAMERA_SENSOR_NAME=$(BOARD_FRONT_CAMERA_SENSOR)
LOCAL_CFLAGS += -DSECURE_CAMERA_SENSOR_NAME=$(BOARD_SECURE_CAMERA_SENSOR)
LOCAL_CFLAGS += -DUSE_CAMERA_ESD_RESET
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)
LOCAL_CFLAGS += -DSECURE_ROTATION=$(BOARD_SECURE_CAMERA_ROTATION)

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_CFLAGS += -DUSE_DUAL_CAMERA
LOCAL_CFLAGS += -DMAIN_1_CAMERA_SENSOR_NAME=$(BOARD_BACK_1_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_1_CAMERA_SENSOR_NAME=$(BOARD_FRONT_1_CAMERA_SENSOR)
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA_SOLUTION_FAKE), true)
LOCAL_CFLAGS += -DUSES_DUAL_CAMERA_SOLUTION_FAKE
LOCAL_SHARED_LIBRARIES += libexynoscamera_fakefusion_plugin
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA_SOLUTION_ARCSOFT), true)
LOCAL_CFLAGS += -DUSES_DUAL_CAMERA_SOLUTION_ARCSOFT
LOCAL_SHARED_LIBRARIES += libexynoscamera_arcsoftfusion_plugin
endif

ifeq ($(BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS), true)
LOCAL_CFLAGS += -DUSES_SW_VDIS
LOCAL_SHARED_LIBRARIES += libexynoscamera_vdis_plugin
endif

ifeq ($(BOARD_CAMERA_USES_HIFI_LLS_CAPTURE), true)
LOCAL_CFLAGS += -DUSES_HIFI_LLS
LOCAL_SHARED_LIBRARIES += libexynoscamera_hifills_plugin
endif

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libcamera3 \
	$(LOCAL_PATH)/../libcamera3/SensorInfos \
	$(TOP)/system/media/camera/include \
	$(TOP)/system/core/libsync/include \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/9xxx \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Activities \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Buffers \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/MCPipes \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Pipes2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PostProcessing \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/SensorInfos \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/SizeTables \
	$(TOP)/hardware/samsung_slsi/exynos/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/include \
	$(TOP)/hardware/libhardware_legacy/include/hardware_legacy \
	$(TOP)/bionic \
	$(TOP)/external/expat/lib \
	$(TOP)/external/libcxx/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/frameworks/native/libs/arect/include \
	$(TOP)/frameworks/native/libs/nativebase/include \
	$(TOP)/frameworks/av/include \
	$(TOP)/hardware/interfaces/camera/common/1.0/default/include \
	$(TOP)/hardware/samsung_slsi/exynos/libion/include

ifeq ($(BOARD_CAMERA_USES_SLSI_PLUGIN), true)
LOCAL_CFLAGS += -DUSE_SLSI_PLUGIN
LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PlugIn \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PlugIn/include \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PlugIn/converter
endif

ifeq ($(BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS), true)
LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/9xxx/Sec/ExynosCameraSolution
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/Vendor/$(LOCAL_PROJECT_DIR)
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/SensorInfos
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_C_INCLUDES += \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Fusion \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Fusion/DofLut
endif

LOCAL_SRC_FILES = \
	../../exynos/libcamera3/common_v2/ExynosCameraFrame.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraMemory.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraFrameManager.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraUtils.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraNode.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraNodeJpegHAL.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraFrameSelector.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraFrameFactoryBase.cpp \
	../../exynos/libcamera3/common_v2/SensorInfos/ExynosCameraSensorInfoBase.cpp \
	../../exynos/libcamera3/common_v2/PostProcessing/ExynosCameraPP.cpp \
	../../exynos/libcamera3/common_v2/PostProcessing/ExynosCameraPPLibcsc.cpp \
	../../exynos/libcamera3/common_v2/PostProcessing/ExynosCameraPPJPEG.cpp \
	../../exynos/libcamera3/common_v2/PostProcessing/ExynosCameraPPGDC.cpp \
	../../exynos/libcamera3/common_v2/PostProcessing/ExynosCameraPPFactory.cpp \
	../../exynos/libcamera3/common_v2/MCPipes/ExynosCameraMCPipe.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipe.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraSWPipe.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipeFlite.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipeGSC.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipeJpeg.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipeVRA.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipeHFD.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipePP.cpp \
	../../exynos/libcamera3/common_v2/Buffers/ExynosCameraBufferManager.cpp \
	../../exynos/libcamera3/common_v2/Buffers/ExynosCameraBufferSupplier.cpp \
	../../exynos/libcamera3/common_v2/Activities/ExynosCameraActivityBase.cpp \
	../../exynos/libcamera3/common_v2/Activities/ExynosCameraActivityAutofocus.cpp \
	../../exynos/libcamera3/common_v2/Activities/ExynosCameraActivityFlash.cpp \
	../../exynos/libcamera3/common_v2/Activities/ExynosCameraActivitySpecialCapture.cpp \
	../../exynos/libcamera3/common_v2/Activities/ExynosCameraActivityUCTL.cpp \
	../../exynos/libcamera3/common_v2/Sec/Activities/ExynosCameraActivityAutofocusVendor.cpp \
	../../exynos/libcamera3/common_v2/Sec/Activities/ExynosCameraActivityFlashVendor.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraRequestManager.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraStreamManager.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraMetadataConverter.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraTimeLogger.cpp \
	../../exynos/libcamera3/common_v2/Sec/ExynosCameraFrameSelectorVendor.cpp \
	../../exynos/libcamera3/common_v2/Sec/ExynosCameraMetadataConverterVendor.cpp \
	../../exynos/libcamera3/common_v2/Sec/ExynosCameraRequestManagerVendor.cpp \
	../../exynos/libcamera3/common_v2/Sec/ExynosCameraStreamManagerVendor.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraActivityControl.cpp\
	../../exynos/libcamera3/9xxx/ExynosCamera.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraParameters.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraConfigurations.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraSizeControl.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFrameFactory.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFrameFactoryPreview.cpp \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraFrameFactoryPreviewVendor.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFrameFactoryPreviewFrontPIP.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFrameFactoryVision.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFrameReprocessingFactory.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFrameJpegReprocessingFactory.cpp \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraFrameReprocessingFactoryVendor.cpp \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraFrameJpegReprocessingFactoryVendor.cpp \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraVendor.cpp \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraConfigurationsVendor.cpp \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraParametersVendor.cpp

ifeq ($(BOARD_CAMERA_USES_SLSI_PLUGIN), true)
LOCAL_SHARED_LIBRARIES += libdl libexynoscamera_plugin
LOCAL_SRC_FILES += \
	../../exynos/libcamera3/common_v2/PlugIn/converter/ExynosCameraPlugInConverter.cpp \
	../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipePlugIn.cpp \
	../../exynos/libcamera3/9xxx/ExynosCameraFactoryPlugIn.cpp
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_SRC_FILES += \
    ../../exynos/libcamera3/common_v2/Pipes2/ExynosCameraPipeSync.cpp \
    ../../exynos/libcamera3/9xxx/ExynosCameraFrameFactoryPreviewDual.cpp \
    ../../exynos/libcamera3/9xxx/ExynosCameraFrameReprocessingFactoryDual.cpp
endif

ifneq ($(filter eng userdebug, $(TARGET_BUILD_VARIANT)),)
LOCAL_CFLAGS += -DUSE_DEBUG_PROPERTY
LOCAL_SRC_FILES += \
	../../exynos/libcamera3/common_v2/ExynosCameraProperty.cpp \
	../../exynos/libcamera3/common_v2/ExynosCameraLogManager.cpp
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_SRC_FILES += ../libcamera3/Vendor/$(LOCAL_PROJECT_DIR)/ExynosCameraSensorInfo.cpp
else
LOCAL_SRC_FILES += ../libcamera3/SensorInfos/ExynosCameraSensorInfo.cpp
endif


ifeq ($(BOARD_CAMERA_USES_CAMERA_SOLUTION_VDIS), true)
LOCAL_SRC_FILES += \
	../../exynos/libcamera3/9xxx/Sec/ExynosCameraSolution/ExynosCameraSolutionSWVdis.cpp
endif

$(foreach file,$(LOCAL_SRC_FILES),$(shell touch '$(LOCAL_PATH)/$(file)'))

LOCAL_LDFLAGS :=  -Wl,-Bsymbolic

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libexynoscamera3

include $(TOP)/hardware/samsung_slsi/exynos/BoardConfigCFlags.mk

ifeq ($(BOARD_CAMERA_USES_SLSI_PLUGIN), true)
include $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PlugIn/converter/libs/Android.mk
endif

include $(BUILD_SHARED_LIBRARY)


#################
# camera.exynos9630.so

include $(CLEAR_VARS)

# HAL module implemenation stored in
# hw/<COPYPIX_HARDWARE_MODULE_ID>.<ro.product.board>.so
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/libcamera3 \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/libcamera3/SensorInfos \
	$(TOP)/system/media/camera/include \
	$(TOP)/system/core/libsync/include \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/9xxx \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Activities \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Buffers \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/MCPipes \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Pipes2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PostProcessing \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/SensorInfos \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/SizeTables \
	$(TOP)/hardware/samsung_slsi/exynos/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(TOP)/external/libcxx/include \
	$(TOP)/bionic \
	$(TOP)/frameworks/native/include \
	$(TOP)/frameworks/native/libs/nativebase/include \
	$(TOP)/frameworks/native/libs/arect/include \
	$(TOP)/hardware/samsung_slsi/exynos/libion/include \
	$(TOP)/hardware/interfaces/camera/common/1.0/default/include

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_C_INCLUDES += \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Fusion \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/Fusion/DofLut
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/Vendor/$(LOCAL_PROJECT_DIR)
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/SensorInfos
endif

LOCAL_SRC_FILES:= \
	../../exynos/libcamera3/common_v2/ExynosCameraInterface.cpp

LOCAL_CFLAGS += -Wno-error=date-time
LOCAL_CFLAGS += -Wno-overloaded-virtual
LOCAL_CFLAGS += -DMAIN_CAMERA_SENSOR_NAME=$(BOARD_BACK_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_CAMERA_SENSOR_NAME=$(BOARD_FRONT_CAMERA_SENSOR)
LOCAL_CFLAGS += -DSECURE_CAMERA_SENSOR_NAME=$(BOARD_SECURE_CAMERA_SENSOR)
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)
LOCAL_CFLAGS += -DSECURE_ROTATION=$(BOARD_SECURE_CAMERA_ROTATION)

ifneq ($(filter eng userdebug, $(TARGET_BUILD_VARIANT)),)
LOCAL_CFLAGS += -DUSE_DEBUG_PROPERTY
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_CFLAGS += -DUSE_DUAL_CAMERA
LOCAL_CFLAGS += -DMAIN_1_CAMERA_SENSOR_NAME=$(BOARD_BACK_1_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_1_CAMERA_SENSOR_NAME=$(BOARD_FRONT_1_CAMERA_SENSOR)
endif

LOCAL_SHARED_LIBRARIES:= liblog libhardware libion_exynos libhwjpeg
LOCAL_SHARED_LIBRARIES += libexynoscamera3
LOCAL_SHARED_LIBRARIES += android.hardware.graphics.allocator@2.0

$(foreach file,$(LOCAL_SRC_FILES),$(shell touch '$(LOCAL_PATH)/$(file)'))

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := camera.$(TARGET_SOC)

include $(TOP)/hardware/samsung_slsi/exynos/BoardConfigCFlags.mk
include $(BUILD_SHARED_LIBRARY)

$(warning #############################################)
$(warning ########       libcamera3       #############)
$(warning #############################################)

include $(call all-makefiles-under, $(LOCAL_PATH))

ifeq ($(BOARD_CAMERA_USES_SLSI_PLUGIN), true)
#################
# plugIn
include $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common_v2/PlugIn/Android.mk
endif
