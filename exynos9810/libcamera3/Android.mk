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

ifeq ($(TARGET_BOARD_PLATFORM), universal9810)
######## System LSI ONLY ########
BOARD_CAMERA_GED_FEATURE := true
#################################
else
BOARD_CAMERA_GED_FEATURE := false
endif

LOCAL_PRELINK_MODULE := false
LOCAL_PROPRIETARY_MODULE := true

LOCAL_STATIC_LIBRARIES := android.hardware.camera.common@1.0-helper
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libui libcamera_metadata libbinder
LOCAL_SHARED_LIBRARIES += libexynosutils libhwjpeg libexynosv4l2 libion_exynos libsync libcsc libacryl
LOCAL_SHARED_LIBRARIES += android.hardware.graphics.allocator@2.0 android.hardware.graphics.mapper@2.0 libGrallocWrapper
#LOCAL_SHARED_LIBRARIES_arm := libhfd

ifeq ($(BOARD_CAMERA_GED_FEATURE), false)
LOCAL_SHARED_LIBRARIES += libsecnativefeature
LOCAL_SHARED_LIBRARIES += libuniplugin
LOCAL_SHARED_LIBRARIES += libsensorlistener
LOCAL_SHARED_LIBRARIES += libuniapi
endif

ifeq ($(BOARD_CAMERA_GED_FEATURE), false)
ifneq ($(filter eng,$(TARGET_BUILD_VARIANT)),)
LOCAL_CFLAGS += -DDEBUG_IQ_OSD
LOCAL_CFLAGS += -DUNI_HAL_TOAST
endif
endif

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
LOCAL_CFLAGS += -Wno-implicit-fallthrough
LOCAL_CFLAGS += -Wno-error=implicit-fallthrough

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_CFLAGS += -DMAIN_1_CAMERA_SENSOR_NAME=$(BOARD_BACK_1_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_1_CAMERA_SENSOR_NAME=$(BOARD_FRONT_1_CAMERA_SENSOR)
endif

ifeq ($(BOARD_CAMERA_GED_FEATURE), true)
LOCAL_CFLAGS += -DCAMERA_GED_FEATURE
else
LOCAL_CFLAGS += -DUSE_CSC_FEATURE
endif

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libcamera3 \
	$(LOCAL_PATH)/../libcamera3/SensorInfos \
	$(TOP)/system/media/camera/include \
	$(TOP)/system/core/libsync/include \
	$(TOP)/system/memory/libion/kernel-headers \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/9810 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Activities \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Buffers \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/MCPipes \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Pipes2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/PostProcessing \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/SensorInfos \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/SizeTables \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Sec \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Sec/PPUniPlugin \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Sec/Pipes2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Sec/Activities \
	$(TOP)/hardware/samsung_slsi/exynos/include \
	$(TOP)/hardware/samsung_slsi/exynos5/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/libhardware_legacy/include/hardware_legacy \
	$(TOP)/vendor/samsung/configs/feature/CscFeature/libsecnativefeature \
	$(TOP)/bionic \
	$(TOP)/external/expat/lib \
	$(TOP)/external/libcxx/include \
	$(TOP)/frameworks/native/include \
	$(TOP)/frameworks/native/libs/arect/include \
	$(TOP)/frameworks/native/libs/nativebase/include \
	$(TOP)/hardware/interfaces/camera/common/1.0/default/include \
	$(TOP)/hardware/camera/SensorListener \
	$(TOP)/hardware/camera/UniPlugin/include \
	$(TOP)/hardware/camera/UniAPI/include

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/Vendor/$(LOCAL_PROJECT_DIR)
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/SensorInfos
endif

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_C_INCLUDES += \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Fusion \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Fusion/DofLut
endif

LOCAL_SRC_FILES = \
	../../exynos/libcamera3/common/ExynosCameraFrame.cpp \
	../../exynos/libcamera3/common/ExynosCameraMemory.cpp \
	../../exynos/libcamera3/common/ExynosCameraFrameManager.cpp \
	../../exynos/libcamera3/common/ExynosCameraUtils.cpp \
	../../exynos/libcamera3/common/ExynosCameraNode.cpp \
	../../exynos/libcamera3/common/ExynosCameraNodeJpegHAL.cpp \
	../../exynos/libcamera3/common/ExynosCameraFrameSelector.cpp \
	../../exynos/libcamera3/common/ExynosCameraFrameFactoryBase.cpp \
	../../exynos/libcamera3/common/SensorInfos/ExynosCameraSensorInfoBase.cpp \
	../../exynos/libcamera3/common/PostProcessing/ExynosCameraPP.cpp \
	../../exynos/libcamera3/common/PostProcessing/ExynosCameraPPLibcsc.cpp \
	../../exynos/libcamera3/common/PostProcessing/ExynosCameraPPJPEG.cpp \
	../../exynos/libcamera3/common/PostProcessing/ExynosCameraPPGDC.cpp \
	../../exynos/libcamera3/common/PostProcessing/ExynosCameraPPFactory.cpp \
	../../exynos/libcamera3/common/MCPipes/ExynosCameraMCPipe.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipe.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraSWPipe.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeFlite.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeGSC.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeJpeg.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeVRA.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeHFD.cpp \
	../../exynos/libcamera3/common/Pipes2/ExynosCameraPipePP.cpp \
	../../exynos/libcamera3/common/Buffers/ExynosCameraBufferManager.cpp \
	../../exynos/libcamera3/common/Buffers/ExynosCameraBufferSupplier.cpp \
	../../exynos/libcamera3/common/Activities/ExynosCameraActivityBase.cpp \
	../../exynos/libcamera3/common/Activities/ExynosCameraActivityAutofocus.cpp \
	../../exynos/libcamera3/common/Activities/ExynosCameraActivityFlash.cpp \
	../../exynos/libcamera3/common/Activities/ExynosCameraActivitySpecialCapture.cpp \
	../../exynos/libcamera3/common/Activities/ExynosCameraActivityUCTL.cpp \
	../../exynos/libcamera3/common/Sec/Activities/ExynosCameraActivityAutofocusVendor.cpp \
	../../exynos/libcamera3/common/Sec/Activities/ExynosCameraActivityFlashVendor.cpp \
	../../exynos/libcamera3/common/Sec/PostProcessing/ExynosCameraPPFactoryVendor.cpp \
	../../exynos/libcamera3/common/ExynosCameraRequestManager.cpp \
	../../exynos/libcamera3/common/ExynosCameraStreamManager.cpp \
	../../exynos/libcamera3/common/ExynosCameraMetadataConverter.cpp \
	../../exynos/libcamera3/common/Sec/ExynosCameraFrameSelectorVendor.cpp \
	../../exynos/libcamera3/common/Sec/ExynosCameraMetadataConverterVendor.cpp \
	../../exynos/libcamera3/common/Sec/ExynosCameraRequestManagerVendor.cpp \
	../../exynos/libcamera3/common/Sec/ExynosCameraStreamManagerVendor.cpp \
	../../exynos/libcamera3/common/Sec/SecAppMarker.cpp \
	../../exynos/libcamera3/common/Sec/SecCameraVendorTags.cpp \
	../../exynos/libcamera3/common/Sec/SecCameraUtil.cpp \
	../../exynos/libcamera3/9810/ExynosCameraActivityControl.cpp\
	../../exynos/libcamera3/9810/ExynosCamera.cpp \
	../../exynos/libcamera3/9810/ExynosCameraParameters.cpp \
	../../exynos/libcamera3/9810/ExynosCameraSizeControl.cpp \
	../../exynos/libcamera3/9810/ExynosCameraFrameFactory.cpp \
	../../exynos/libcamera3/9810/ExynosCameraFrameFactoryPreview.cpp \
	../../exynos/libcamera3/9810/Sec/ExynosCameraFrameFactoryPreviewVendor.cpp \
	../../exynos/libcamera3/9810/ExynosCameraFrameFactoryPreviewFrontPIP.cpp \
	../../exynos/libcamera3/9810/ExynosCameraFrameFactoryVision.cpp \
	../../exynos/libcamera3/9810/ExynosCameraFrameReprocessingFactory.cpp \
	../../exynos/libcamera3/9810/Sec/ExynosCameraFrameReprocessingFactoryVendor.cpp \
	../../exynos/libcamera3/9810/Sec/ExynosCameraVendor.cpp \
	../../exynos/libcamera3/9810/Sec/ExynosCameraParametersVendor.cpp

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_SRC_FILES += \
    ../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeSync.cpp \
    ../../exynos/libcamera3/common/Pipes2/ExynosCameraPipeFusion.cpp \
    ../../exynos/libcamera3/common/Fusion/ExynosCameraFusionMetaDataConverter.cpp \
    ../../exynos/libcamera3/common/Fusion/ExynosCameraFusionWrapper.cpp \
    ../../exynos/libcamera3/9810/ExynosCameraFrameFactoryPreviewDual.cpp \
    ../../exynos/libcamera3/9810/ExynosCameraFrameReprocessingFactoryDual.cpp
endif

ifeq ($(BOARD_CAMERA_GED_FEATURE), false)
LOCAL_SRC_FILES += \
	../../exynos/libcamera3/common/Sec/Pipes2/ExynosCameraPipePPVendor.cpp \
	../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPlugin.cpp \
	../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginLD.cpp \
	../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginOT.cpp \
	../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginHLV.cpp \
	../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginFP.cpp \
	../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginBD.cpp \
    ../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginSTR_Preview.cpp \
    ../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginSTR_Capture.cpp \
    ../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginIDDQD.cpp \
    ../../exynos/libcamera3/common/Sec/PPUniPlugin/ExynosCameraPPUniPluginSWVdis_3_0.cpp
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_SRC_FILES += ../libcamera3/Vendor/$(LOCAL_PROJECT_DIR)/ExynosCameraSensorInfo.cpp
else
LOCAL_SRC_FILES += ../libcamera3/SensorInfos/ExynosCameraSensorInfo.cpp
endif

$(foreach file,$(LOCAL_SRC_FILES),$(shell touch '$(LOCAL_PATH)/$(file)'))

LOCAL_LDFLAGS :=  -Wl,-Bsymbolic

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libexynoscamera3

include $(TOP)/hardware/samsung_slsi/exynos/BoardConfigCFlags.mk
include $(BUILD_SHARED_LIBRARY)


#################
# camera.exynos9810.so

include $(CLEAR_VARS)

ifeq ($(TARGET_BOARD_PLATFORM), universal9810)
######## System LSI ONLY ########
BOARD_CAMERA_GED_FEATURE := true
#################################
else
BOARD_CAMERA_GED_FEATURE := false
endif

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
	$(TOP)/system/memory/libion/kernel-headers \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/9810 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Activities \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Buffers \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/MCPipes \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Pipes2 \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/PostProcessing \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/SensorInfos \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/SizeTables \
	$(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Sec \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Sec/PPUniPlugin \
	$(TOP)/hardware/samsung_slsi/exynos/include \
	$(TOP)/hardware/samsung_slsi/exynos5/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/interfaces/camera/common/1.0/default/include \
	$(TOP)/external/libcxx/include \
	$(TOP)/bionic \
	$(TOP)/frameworks/native/include \
	$(TOP)/frameworks/native/libs/nativebase/include \
	$(TOP)/frameworks/native/libs/nativewindow/include \
    $(TOP)/frameworks/native/libs/arect/include

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_C_INCLUDES += \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Fusion \
    $(TOP)/hardware/samsung_slsi/exynos/libcamera3/common/Fusion/DofLut
endif

ifeq ($(BOARD_CAMERA_GED_FEATURE), false)
LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC_BASE)/libcamera3/Vendor \
	$(TOP)/hardware/camera/SensorListener \
	$(TOP)/hardware/camera/UniPlugin/include \
	$(TOP)/hardware/camera/UniAPI/include
endif

ifneq ($(LOCAL_PROJECT_DIR),)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/Vendor/$(LOCAL_PROJECT_DIR)
else
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcamera3/SensorInfos
endif

LOCAL_SRC_FILES:= \
	../../exynos/libcamera3/common/ExynosCameraInterface.cpp

ifeq ($(BOARD_CAMERA_GED_FEATURE), false)
ifneq ($(filter eng,$(TARGET_BUILD_VARIANT)),)
LOCAL_CFLAGS += -DDEBUG_IQ_OSD
LOCAL_CFLAGS += -DUNI_HAL_TOAST
endif
endif

LOCAL_CFLAGS += -Wno-error=date-time
LOCAL_CFLAGS += -Wno-overloaded-virtual
LOCAL_CFLAGS += -DMAIN_CAMERA_SENSOR_NAME=$(BOARD_BACK_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_CAMERA_SENSOR_NAME=$(BOARD_FRONT_CAMERA_SENSOR)
LOCAL_CFLAGS += -DSECURE_CAMERA_SENSOR_NAME=$(BOARD_SECURE_CAMERA_SENSOR)
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)
LOCAL_CFLAGS += -DSECURE_ROTATION=$(BOARD_SECURE_CAMERA_ROTATION)

ifeq ($(BOARD_CAMERA_USES_DUAL_CAMERA), true)
LOCAL_CFLAGS += -DBOARD_CAMERA_USES_DUAL_CAMERA
LOCAL_CFLAGS += -DMAIN_1_CAMERA_SENSOR_NAME=$(BOARD_BACK_1_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_1_CAMERA_SENSOR_NAME=$(BOARD_FRONT_1_CAMERA_SENSOR)
endif

ifeq ($(BOARD_CAMERA_GED_FEATURE), true)
LOCAL_CFLAGS += -DCAMERA_GED_FEATURE
endif

ifeq ($(BOARD_SECURE_CAMERA_SUPPORT), true)
LOCAL_CFLAGS += -DBOARD_SECURE_CAMERA_SUPPORT
endif

LOCAL_SHARED_LIBRARIES:= liblog libhardware libion_exynos libhwjpeg libbinder libui
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
