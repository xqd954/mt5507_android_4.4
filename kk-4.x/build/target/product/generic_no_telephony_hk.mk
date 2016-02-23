#
# Copyright (C) 2007 The Android Open Source Project
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

# This is a generic phone product that isn't specialized for a specific device.
# It includes the base Android platform.

PRODUCT_POLICY := android.policy_phone

PRODUCT_PACKAGES := \
    DeskClock \
    Bluetooth \
    Calculator \
    Calendar \
    Camera2 \
    Email \
    Exchange2 \
    Gallery2 \
    InputDevices \
    Music \
    MusicFX \
    TeleService \
    CalendarProvider \
    bluetooth-health \
    hostapd \
    wpa_supplicant.conf \
    TCL_HK_CyberUI \
    TCL_HK_CyberUIRes \
    HK_SmartTVHelp \
    TCL_HK_Browser3.0 \
    TCL_HK_TV \
    TCL_HK_MediaBrowser \
    TCL_HK_mtkVersionUpdate \
    TCL_HK_TCL_HK_MessageBox \
    TCL_HK_Tweeter \
    TCL_HK_MenuHelpFor5Key \
    TCL_HK_Factory \
    TCL_HK_common_UserCenter \
    TCL_HK_common_DeviceAuthention \
    TCL_HK_TvSymbolRecognize \
    TCL_HK_AdService \
    TCL_HK_Bootsetwizard_s \
    TCL_HK_ImageViewer \
    TCL_HK_UsageStats \
    TCL_HK_ComMediaPlayer \
    TCL_HK_TVOS_TVManager_FastBoot \
    TCL_HK_TVOS_TVManager_NetManager \
    TCL_HK_TVOS_TVManager_SafeProtection \
    TCL_HK_TVOS_TVManager_SoftwareManager \
    TCL_HK_TVOS_TVManager_SystemInfo \
    TCL_HK_TVOS_TVManager_TaskManager \
    TCL_HK_TVOS_TVManager_TVExamination \
    TCL_HK_TVOS_TVManager_TVManager \
    TCL_HK_MT5507_IQIYI_Video \
    TCL_HK_Settings_DTV \
    TCL_HK_Pinyin_IME \
    TCL_HK_PackageInstallerRenew \
    TCL_HK_AppStore3.2 \
    TCL_HK_AppStoreUpdate \
    TCL_HK_PackageInstallerHIS2V3.2_CN \
    TCL_HK_ShowMode \
    TCL_HK_3Ddemo \
    TCL_HK_SnapshotScreen \
    TCL_HK_AirplayPlayerActivity \
    TCL_HK_AudioPlayerActivity \
    TCL_HK_Multiscreen_mDNSProtocolStack_TV \
    TCL_HK_WelcomeActivity \
    TCL_HK_WeiXin \
    TCL_HK_nScreen2.0 \
    TCL_HK_youku_tv \
    TCL_HK_IflytekTlrService \
    TCL_HK_TvWeather \
    TCL_HK_TVHealthCheck_MT5507_signed \
    TCL_HK_MediaCenter \
    TCL_HK_TestFlashSpace \
    TCL_TV_MARKET \
    TVGamepad \
    TCL_MT5507_iCast \
    TCL_DongleDetectServer \
    TCL_HK_ALL_TvLife \
    TCL_HK_imgoTV \
    TCL_HK_YouTube_10_12_53 \
    TCL_HK_YouTube \

ifeq "$(SYSTEMUI_EXISTED)" "true"
PRODUCT_PACKAGES += SystemUI
endif

PRODUCT_PACKAGES += \
    audio \
    clatd \
    clatd.conf \
    dhcpcd.conf \
    network \
    pand \
    pppd \
    sdptool \
    wpa_supplicant

PRODUCT_PACKAGES += \
    librs_jni \
    libvideoeditor_jni \
    libvideoeditor_core \
    libvideoeditor_osal \
    libvideoeditor_videofilters \
    libvideoeditorplayer \

PRODUCT_PACKAGES += \
    audio_policy.default \
    local_time.default \
    power.default

PRODUCT_PACKAGES += \
    local_time.default

PRODUCT_COPY_FILES := \
        frameworks/av/media/libeffects/data/audio_effects.conf:system/etc/audio_effects.conf \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ACR/libChannelSwitchDetector.so:system/lib/libChannelSwitchDetector.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ACR/libFingerprintGenerator.so:system/lib/libFingerprintGenerator.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ACR/libopencv_java.so:system/lib/libopencv_java.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/libtclappstore.so:system/lib/libtclappstore.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/libtclappstoredata.so:system/lib/libtclappstoredata.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/appconfig.xml:system/etc/appinfo/appconfig.xml \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/appconfig_TCL-CN-MT5507-F3800A.xml:system/etc/appinfo/appconfig_TCL-CN-MT5507-F3800A.xml \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/appconfig_TCL-CN-MT5507-A710.xml:system/etc/appinfo/appconfig_TCL-CN-MT5507-A710.xml \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/appconfig_TCL-CN-MT5507-A810.xml:system/etc/appinfo/appconfig_TCL-CN-MT5507-A810.xml \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/appconfig_TCL-CN-MT5507-B385A.xml:system/etc/appinfo/appconfig_TCL-CN-MT5507-B385A.xml \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_AppStore/appconfig_TCL-CN-MT5507-HK.xml:system/etc/appinfo/appconfig_TCL-CN-MT5507-HK.xml \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MenuFor5Keysigned/libsim-key.so:system/lib/libsim-key.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libiqiyi_media_player.so:system/lib/libiqiyi_media_player.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libiqiyi_player_memalloc.so:system/lib/libiqiyi_player_memalloc.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libkernel.so:system/lib/libkernel.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libKey.so:system/lib/libKey.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libSpeedEngine.so:system/lib/libSpeedEngine.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libSysInput.so:system/lib/libSysInput.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_MT5507_IQIYI_Video/libnetdoc.so:system/lib/libnetdoc.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_Pinyin_IME/libjni_pinyinime.so:system/lib/libjni_pinyinime.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_CyberUI/black_list.json:system/etc/black_list.json \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_CyberUI/cyberui_config.json:system/etc/cyberui_config.json \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_nScreen_srv/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_WeiXin/libalgms.so:system/lib/libalgms.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_WeiXin/libluajava.so:system/lib/libluajava.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_WeiXin/libTLR15.so:system/lib/libTLR15.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ShowMode/libdatasystemdemo.so:system/lib/libdatasystemdemo.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ShowMode/libstlport_shared.so:system/lib/libstlport_shared.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ShowMode/libtcl3d.so:system/lib/libtcl3d.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_ShowMode/libtcl3dM.so:system/lib/libtcl3dM.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_TvLife/libamapv304.so:system/lib/libamapv304.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_TvLife/libamapv304ex.so:system/lib/libamapv304ex.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_imgoTV/lib_All_imgoTV_bitmaps.so:system/lib/lib_All_imgoTV_bitmaps.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_imgoTV/lib_All_imgoTV_nn_tv_air_control.so:system/lib/lib_All_imgoTV_nn_tv_air_control.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_imgoTV/lib_All_imgoTV_nn_tv_client.so:system/lib/lib_All_imgoTV_nn_tv_client.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_imgoTV/lib_All_imgoTV_starcor_xul.so:system/lib/lib_All_imgoTV_starcor_xul.so \
        vendor/tcl/sita/X_packages/CN_HK_APP/TCL_HK_imgoTV/lib_All_imgoTV_xul.so:system/lib/lib_All_imgoTV_xul.so \
		
PRODUCT_PROPERTY_OVERRIDES += \
    ro.carrier=unknown

$(call inherit-product-if-exists, frameworks/base/data/fonts/fonts.mk)
$(call inherit-product-if-exists, external/noto-fonts/fonts.mk)
$(call inherit-product-if-exists, external/naver-fonts/fonts.mk)
$(call inherit-product-if-exists, external/sil-fonts/fonts.mk)
$(call inherit-product-if-exists, frameworks/base/data/keyboards/keyboards.mk)
$(call inherit-product-if-exists, frameworks/webview/chromium/chromium.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/core.mk)

# Overrides
PRODUCT_BRAND := generic
PRODUCT_DEVICE := generic
PRODUCT_NAME := generic_no_telephony
