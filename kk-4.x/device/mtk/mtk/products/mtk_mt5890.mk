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
80211_BUILD_IN := true
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

PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.$(MODEL).pre_dtv_driver.rc:root/init.pre_dtv_driver.rc


#3D GPU Library

# Include System Property config file
$(call inherit-product, $(DEVICE_STRING)/system.mk)

