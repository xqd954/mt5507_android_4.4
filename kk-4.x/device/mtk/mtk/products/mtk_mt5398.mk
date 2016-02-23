#
# Copyright (C) 2011 The Android Open-Source Project
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

#
# This file is the build configuration for a full Android
# build for mt5398 hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#

# Include Generic System config
$(call inherit-product, device/$(BRAND)/$(ODM)/products/generic.mk)

PREBUIT_PATH := ../mediatek/device/prebuilt
DTV_OUT_PATH := $(DEVICE_STRING)/DTV_OUT

# wifi 80211
80211_BUILD_IN := false
ifneq "$(80211_BUILD_IN)" "true"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/cfg80211.ko:system/lib/modules/cfg80211.ko \
    $(DTV_OUT_PATH)/mac80211.ko:system/lib/modules/mac80211.ko
endif

#3D GPU Library
ifeq "${GPU_SETTING}" "IMG"
ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
PRODUCT_COPY_FILES += \
     $(PREBUIT_PATH)/img_sgx_5398_stagefright/dc_cobra.ko:system/modules/dc_cobra.ko \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/pvrsrvkm.ko:system/modules/pvrsrvkm.ko \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/pvrsrvctl:system/bin/pvrsrvctl \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/egl.cfg:system/lib/egl/egl.cfg \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libEGL_POWERVR_SGX544_115.so:system/lib/egl/libEGL_POWERVR_SGX544_115.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libGLESv1_CM_POWERVR_SGX544_115.so:system/lib/egl/libGLESv1_CM_POWERVR_SGX544_115.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libGLESv2_POWERVR_SGX544_115.so:system/lib/egl/libGLESv2_POWERVR_SGX544_115.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libglslcompiler.so:system/lib/libglslcompiler.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libIMGegl.so:system/lib/libIMGegl.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libpvr2d.so:system/lib/libpvr2d.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libpvrANDROID_WSEGL.so:system/lib/libpvrANDROID_WSEGL.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libPVRScopeServices.so:system/lib/libPVRScopeServices.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libsrv_init.so:system/lib/libsrv_init.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libsrv_um.so:system/lib/libsrv_um.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/libusc.so:system/lib/libusc.so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/gralloc.$(ANDROID_IC_SETTING).so:system/lib/hw/gralloc.$(ANDROID_IC_SETTING).so \
		 $(PREBUIT_PATH)/img_sgx_5398_stagefright/lib/hwcomposer.$(ANDROID_IC_SETTING).so:system/lib/hw/hwcomposer.$(ANDROID_IC_SETTING).so
else
PRODUCT_COPY_FILES += \
		 $(PREBUIT_PATH)/img_sgx_5398/dc_cobra.ko:system/modules/dc_cobra.ko \
		 $(PREBUIT_PATH)/img_sgx_5398/pvrsrvkm.ko:system/modules/pvrsrvkm.ko \
		 $(PREBUIT_PATH)/img_sgx_5398/pvrsrvctl:system/bin/pvrsrvctl \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/egl.cfg:system/lib/egl/egl.cfg \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libEGL_POWERVR_SGX544_115.so:system/lib/egl/libEGL_POWERVR_SGX544_115.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libGLESv1_CM_POWERVR_SGX544_115.so:system/lib/egl/libGLESv1_CM_POWERVR_SGX544_115.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libGLESv2_POWERVR_SGX544_115.so:system/lib/egl/libGLESv2_POWERVR_SGX544_115.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libglslcompiler.so:system/lib/libglslcompiler.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libIMGegl.so:system/lib/libIMGegl.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libpvr2d.so:system/lib/libpvr2d.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libpvrANDROID_WSEGL.so:system/lib/libpvrANDROID_WSEGL.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libPVRScopeServices.so:system/lib/libPVRScopeServices.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libsrv_init.so:system/lib/libsrv_init.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libsrv_um.so:system/lib/libsrv_um.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/libusc.so:system/lib/libusc.so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/gralloc.$(ANDROID_IC_SETTING).so:system/lib/hw/gralloc.$(ANDROID_IC_SETTING).so \
		 $(PREBUIT_PATH)/img_sgx_5398/lib/hwcomposer.$(ANDROID_IC_SETTING).so:system/lib/hw/hwcomposer.$(ANDROID_IC_SETTING).so
endif		 
endif
# Include System Property config file
$(call inherit-product, $(DEVICE_STRING)/system.mk)

