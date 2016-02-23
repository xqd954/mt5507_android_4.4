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
# build for mt5882 hardware. This cleanly combines a set of
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

#add dropcaches
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/configs/script/drop_caches.sh:system/etc/script/drop_caches.sh 

#add libsim-mouse.so
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/libsim-mouse.so:system/lib/libsim-mouse.so

#add bootanimation by lulin 
ifeq ($(TCL_PRODUCT_BRANCH),EM)		#yanyy seperate em from cn
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/bootanimation.zip:system/media/bootanimation.zip
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/bootanimation_oem.zip:system/media/bootanimation_oem.zip
else ifeq ($(TCL_PRODUCT_BRANCH),ROWA_CN_X1000)		#yanyy seperate rowa_cn from mt07_cn
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/bootanimation_rowa.zip:system/media/bootanimation.zip
else
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/bootanimation.zip:system/media/bootanimation.zip
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/bootanimationvideo.zip:system/media/bootanimationvideo.zip
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/bootvideo/bootvideo.ts:system/media/bootvideo/bootvideo.ts
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/bootvideo/ts.ini:system/media/bootvideo/ts.ini
endif

#add appwhitelist by gaodw 
ifeq ($(TCL_PRODUCT_BRANCH),EM)		#yanyy seperate em from cn
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/appwhitelist.xml:system/etc/appwhitelist.xml
else
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/appwhitelist.xml:system/etc/appwhitelist.xml
endif


#add DB by lulin 
ifeq ($(TCL_PRODUCT_BRANCH),EM)		#yanyy seperate em from cn
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/DeviceInfo.db:system/etc/deviceinfo/DeviceInfo.db
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_TEN)		#wujy add for tencent_project
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/DeviceInfo_tencent.db:system/etc/deviceinfo/DeviceInfo.db
else
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/DeviceInfo.db:system/etc/deviceinfo/DeviceInfo.db
endif
endif

#add by guolz@tcl.com for impdat being ro(read-only)
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/mtab:system/etc/mtab

#add by tank@tcl.com for sim_driver
ifeq ($(TCL_PRODUCT_BRANCH),EM)		#yanyy seperate em from cn
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/tcl_sim_kb.ko:system/lib/modules/tcl_sim_kb.ko
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/Vendor_0019_Product_0001.kl:system/usr/keylayout/Vendor_0019_Product_0001.kl
else
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/tcl_sim_kb.ko:system/lib/modules/tcl_sim_kb.ko
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/Vendor_0019_Product_0001.kl:system/usr/keylayout/Vendor_0019_Product_0001.kl
PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl/keyboardfunctionkey.txt:system/usr/keylayout/keyboardfunctionkey.txt
endif
#end tank@tcl.com

#add appconfig by gaoyunpei for MT5655_EM
ifeq ($(TCL_PRODUCT_BRANCH),EM)
#PRODUCT_COPY_FILES += device/mtk/mtk/mt5882/tcl_em/appconfig/system_settings_config.properties:system/etc/appconfig/system_settings_config.properties
#modify system_settings_config copy path
PRODUCT_COPY_FILES +=vendor/tcl_em/sita/X_packages/EM_APP/SystemSettings/appconfig/system_settings_config.properties:system/etc/appconfig/system_settings_config.properties
endif

#3D GPU Library

# Include System Property config file
$(call inherit-product, $(DEVICE_STRING)/system.mk)

ifeq ($(TCL_PRODUCT_BRANCH),EM)
#add tcl addon by gaodw.
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/UIKit/products/generic_tvos_addon.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/Tvinfo/generic_tvos_Tvinfo.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/RootSeeker/generic_tvos_rootseeker.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/Net/generic_tvos_net.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/Media/generic_tvos_media.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/HuanAccount/generic_tvos_huanaccount.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/Device/generic_tvos_device.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/System/generic_tvos_system.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/adSystem/generic_tvos_adsystem.mk)
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/generic_tvos_voicecontrol.mk)
#end  
#yanyy seperate EM product from CN product.
$(call inherit-product-if-exists, vendor/tcl_em/sita/X_packages/EM_APP/Android.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_H8800)
#add tcl addon by gaodw.
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/UIKit/products/generic_tvos_addon.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Tvinfo/generic_tvos_Tvinfo.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/RootSeeker/generic_tvos_rootseeker.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Net/generic_tvos_net.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Media/generic_tvos_media.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/HuanAccount/generic_tvos_huanaccount.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Device/generic_tvos_device.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/System/generic_tvos_system.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/adSystem/generic_tvos_adsystem.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/voicecontrol/generic_tvos_voicecontrol.mk)
#end  
#add tcl adsystem by w.y.
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/CN_H8800_APP/Android.mk)
else
#add tcl addon by gaodw.
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/UIKit/products/generic_tvos_addon.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Tvinfo/generic_tvos_Tvinfo.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/RootSeeker/generic_tvos_rootseeker.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Net/generic_tvos_net.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Media/generic_tvos_media.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/HuanAccount/generic_tvos_huanaccount.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/Device/generic_tvos_device.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/System/generic_tvos_system.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/adSystem/generic_tvos_adsystem.mk)
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/voicecontrol/generic_tvos_voicecontrol.mk)
#end  
#add tcl adsystem by w.y.
$(call inherit-product-if-exists, vendor/tcl/sita/X_packages/CN_APP/Android.mk)
endif
endif

#add for bcm bluetooth function by yjw
$(call inherit-product-if-exists, device/mtk/mtk/basic/bt_bcm/bt_bcm_build.mk)
#end  

#add for tcl dongle function by yjw
$(call inherit-product-if-exists, device/mtk/mtk/basic/dongle/dongle_build.mk)
#end 

#del target_files_intermediates for FOTA by pengpeng01@tcl.com
$(shell rm -rf $(ANDROID_PRODUCT_OUT)/obj/PACKAGING/target_files_intermediates/*)
#end  
