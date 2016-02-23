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
# build for mt5399 hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#

# Include Generic System config
$(call inherit-product, device/$(BRAND)/$(ODM)/products/generic.mk)

DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)

PREBUIT_PATH := ../mediatek/device/prebuilt
DTV_OUT_PATH := $(DEVICE_STRING)/DTV_OUT
IMGSGX_OUT_PATH := $(DTV_OUT_PATH)/imgsgxout

# wifi 80211
80211_BUILD_IN := false
ifneq "$(80211_BUILD_IN)" "true"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/cfg80211.ko:system/lib/modules/cfg80211.ko \
    $(DTV_OUT_PATH)/mac80211.ko:system/lib/modules/mac80211.ko
endif

#usb 2.0
ifeq "$(USB20_BUILD_IN)" "true"
else
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/mtk-hcd.ko:system/lib/modules/mtk-hcd.ko
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/usb-storage.ko:system/lib/modules/usb-storage.ko
endif

#SSUSB
ifeq "$(USB30_BUILD_MOD)" "true"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/xhci-hcd.ko:system/lib/modules/xhci-hcd.ko
endif
  
#sw hevc
ifeq ($(HEVC_SUPPORT),true)
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/sw_hevc/libHEVCdec_sa.ca9.android.so:system/lib/libHEVCdec_sa.ca9.android.so \
    $(PREBUIT_PATH)/sw_hevc/libvcodec_oal.so:system/lib/libvcodec_oal.so
endif

#ethernet
ifneq "$(ETHERNET_DRV_BUILTIN)" "true"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/star_mac.ko:system/lib/modules/star_mac.ko
endif

#3D GPU Library
ifeq "${GPU_SETTING}" "IMG"
ifeq ($(RLS_CUSTOM_BUILD),true)
PRODUCT_COPY_FILES += \
      $(PREBUIT_PATH)/lib/egl.cfg:system/lib/egl/egl.cfg \
		 $(PREBUIT_PATH)/dc_cobra.ko:system/modules/dc_cobra.ko \
		 $(PREBUIT_PATH)/pvrsrvkm.ko:system/modules/pvrsrvkm.ko \
		 $(PREBUIT_PATH)/pvrsrvctl:system/bin/pvrsrvctl \
      $(PREBUIT_PATH)/lib/libEGL_POWERVR_SGX543_303.so:system/lib/egl/libEGL_POWERVR_SGX543_303.so \
      $(PREBUIT_PATH)/lib/libGLESv1_CM_POWERVR_SGX543_303.so:system/lib/egl/libGLESv1_CM_POWERVR_SGX543_303.so \
      $(PREBUIT_PATH)/lib/libGLESv2_POWERVR_SGX543_303.so:system/lib/egl/libGLESv2_POWERVR_SGX543_303.so \
      $(PREBUIT_PATH)/lib/libglslcompiler.so:system/lib/libglslcompiler.so \
      $(PREBUIT_PATH)/lib/libIMGegl.so:system/lib/libIMGegl.so \
      $(PREBUIT_PATH)/lib/libpvr2d.so:system/lib/libpvr2d.so \
      $(PREBUIT_PATH)/lib/libpvrANDROID_WSEGL.so:system/lib/libpvrANDROID_WSEGL.so \
      $(PREBUIT_PATH)/lib/libPVRScopeServices.so:system/lib/libPVRScopeServices.so \
      $(PREBUIT_PATH)/lib/libsrv_init.so:system/lib/libsrv_init.so \
      $(PREBUIT_PATH)/lib/libsrv_um.so:system/lib/libsrv_um.so \
      $(PREBUIT_PATH)/lib/libusc.so:system/lib/libusc.so \
      $(PREBUIT_PATH)/lib/gralloc.$(ANDROID_IC_SETTING).so:system/lib/hw/gralloc.$(ANDROID_IC_SETTING).so \
      $(PREBUIT_PATH)/lib/hwcomposer.$(ANDROID_IC_SETTING).so:system/lib/hw/hwcomposer.$(ANDROID_IC_SETTING).so
else
PRODUCT_COPY_FILES += \
		 $(PREBUIT_PATH)/img_sgx_5399/lib/egl.cfg:system/lib/egl/egl.cfg \
		 $(IMGSGX_OUT_PATH)/pvrsrvctl:system/bin/pvrsrvctl \
         $(DTV_OUT_PATH)/dc_cobra.ko:system/modules/dc_cobra.ko \
		 $(DTV_OUT_PATH)/pvrsrvkm.ko:system/modules/pvrsrvkm.ko \
		 $(IMGSGX_OUT_PATH)/lib/libEGL_POWERVR_SGX543_303.so:system/lib/egl/libEGL_POWERVR_SGX543_303.so \
		 $(IMGSGX_OUT_PATH)/lib/libGLESv1_CM_POWERVR_SGX543_303.so:system/lib/egl/libGLESv1_CM_POWERVR_SGX543_303.so \
		 $(IMGSGX_OUT_PATH)/lib/libGLESv2_POWERVR_SGX543_303.so:system/lib/egl/libGLESv2_POWERVR_SGX543_303.so \
		 $(IMGSGX_OUT_PATH)/lib/libglslcompiler.so:system/lib/libglslcompiler.so \
		 $(IMGSGX_OUT_PATH)/lib/libIMGegl.so:system/lib/libIMGegl.so \
		 $(IMGSGX_OUT_PATH)/lib/libpvr2d.so:system/lib/libpvr2d.so \
		 $(IMGSGX_OUT_PATH)/lib/libpvrANDROID_WSEGL.so:system/lib/libpvrANDROID_WSEGL.so \
		 $(IMGSGX_OUT_PATH)/lib/libPVRScopeServices.so:system/lib/libPVRScopeServices.so \
		 $(IMGSGX_OUT_PATH)/lib/libsrv_init.so:system/lib/libsrv_init.so \
		 $(IMGSGX_OUT_PATH)/lib/libsrv_um.so:system/lib/libsrv_um.so \
		 $(IMGSGX_OUT_PATH)/lib/libusc.so:system/lib/libusc.so \
		 $(IMGSGX_OUT_PATH)/lib/gralloc.$(ANDROID_IC_SETTING).so:system/lib/hw/gralloc.$(ANDROID_IC_SETTING).so \
		 $(IMGSGX_OUT_PATH)/lib/hwcomposer.$(ANDROID_IC_SETTING).so:system/lib/hw/hwcomposer.$(ANDROID_IC_SETTING).so 
endif
endif
# Include System Property config file
$(call inherit-product, $(DEVICE_STRING)/system.mk)

