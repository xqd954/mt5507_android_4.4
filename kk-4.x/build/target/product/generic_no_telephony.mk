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
    TCL_CyberUI \
    TCL_CyberUIRes \
    SmartTVHelp \
    TCLBrowser3.0 \
    TCL_TV \
    TCL_MediaBrowser \
    TCL_mtkVersionUpdate \
    TCL_Message_Box_Service_3.1plus \
    TCL_Message_Box_3.1plus \
    TCL_MenuHelpFor5Key \
    TCL_Factory \
    TCL_common_UserCenter \
    TCL_common_DeviceAuthention \
    TCL_AdService \
    TCL_Bootsetwizard_s \
    TCL_ImageViewer \
    TCL_UsageStats \
    TCL_ComMediaPlayer \
    TCL_TVOS_TVManager_FastBoot \
    TCL_TVOS_TVManager_NetManager \
    TCL_TVOS_TVManager_SafeProtection \
    TCL_TVOS_TVManager_SoftwareManager \
    TCL_TVOS_TVManager_SystemInfo \
    TCL_TVOS_TVManager_TaskManager \
    TCL_TVOS_TVManager_TVExamination \
    TCL_TVOS_TVManager_TVManager \
    TCL_MT5507_IQIYI_Video \
    TCL_All_HuanAndroidPay \
    TCL_ALL_Lexue3.0 \
    TCL_Settings_DTV \
    TCL_Pinyin_IME \
    TCL_PackageInstallerRenew \
    TCL_AppStore3.3 \
    TCL_AppStoreUpdate \
    TCL_TCLPackageInstallerMaster \
    TCL_ShowMode \
    TCL_3Ddemo \
    TCL_SnapshotScreen \
    TCL_AirplayPlayerActivity \
    TCL_AudioPlayerActivity \
    TCL_Multiscreen_mDNSProtocolStack_TV \
    TCL_WelcomeActivity \
    TCL_WeiXin \
    TCL_nScreen2.0 \
    TCL_ALL_Rainbow \
    TCL_youku_tv \
    TCL_IflytekTlrService \
    TCL_TvWeather \
    TCL_All_GoLiveCinema \
    TCL_All_GoLiveAdvert \
    TCL_TVHealthCheck_MT5507_signed \
    TCL_MediaCenter \
    TCL_TestFlashSpace \
    TCL_TV_MARKET \
    TVGamepad \
    TCL_MT5507_iCast \
    TCL_DongleDetectServer \
    TCL_All_HuanTVLove \
    TCL_ALL_TvLife \
    TCL_ALL_HuanFitness \
    TCL_All_sofago \

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
        vendor/tcl/sita/X_packages/CN_APP/TCL_AppStore/blacklist_TCL-CN-MT5507-F3800A.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-F3800A.xml \
        vendor/tcl/sita/X_packages/CN_APP/TCL_AppStore/blacklist_TCL-CN-MT5507-A710.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-A710.xml \
        vendor/tcl/sita/X_packages/CN_APP/TCL_AppStore/blacklist_TCL-CN-MT5507-A810.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-A810.xml \
        vendor/tcl/sita/X_packages/CN_APP/TCL_AppStore/blacklist_TCL-CN-MT5507-B385A.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-B385A.xml \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/libBaiduMapSDK_v3_1_0.so:system/lib/libBaiduMapSDK_v3_1_0.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/libiomx-ics.so:system/lib/libiomx-ics.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/liblocSDK4d.so:system/lib/liblocSDK4d.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/libvlcjni.so:system/lib/libvlcjni.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MenuFor5Keysigned/libsim-key.so:system/lib/libsim-key.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libiqiyi_media_player.so:system/lib/libiqiyi_media_player.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libiqiyi_player_memalloc.so:system/lib/libiqiyi_player_memalloc.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libkernel.so:system/lib/libkernel.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libKey.so:system/lib/libKey.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libSpeedEngine.so:system/lib/libSpeedEngine.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libSysInput.so:system/lib/libSysInput.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_MT5507_IQIYI_Video/libnetdoc.so:system/lib/libnetdoc.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_lexuefangzhou/libarm.so:system/lib/libarm.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_lexuefangzhou/libvinit.so:system/lib/libvinit.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Pinyin_IME/libjni_pinyinime.so:system/lib/libjni_pinyinime.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_CyberUI/black_list.json:system/etc/black_list.json \
        vendor/tcl/sita/X_packages/CN_APP/TCL_CyberUI/cyberui_config.json:system/etc/cyberui_config.json \
        vendor/tcl/sita/X_packages/CN_APP/TCL_nScreen_srv/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_WeiXin/libalgms.so:system/lib/libalgms.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_WeiXin/libluajava.so:system/lib/libluajava.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_WeiXin/libTLR15.so:system/lib/libTLR15.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ShowMode/libdatasystemdemo.so:system/lib/libdatasystemdemo.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ShowMode/libstlport_shared.so:system/lib/libstlport_shared.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ShowMode/libtcl3d.so:system/lib/libtcl3d.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ShowMode/libtcl3dM.so:system/lib/libtcl3dM.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/lib_All_golive_forcetv.so:system/lib/lib_All_golive_forcetv.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libDCF_JNI.so:system/lib/libDCF_JNI.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libDPSStreamEngine.so:system/lib/libDPSStreamEngine.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libDSAS.so:system/lib/libDSAS.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libGoliveHelper.so:system/lib/libGoliveHelper.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libICPDownload.so:system/lib/libICPDownload.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libMD5.so:system/lib/libMD5.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libtsplayer.so:system/lib/libtsplayer.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/libvooleglib.so:system/lib/libvooleglib.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/kdmServer:system/bin/kdmServer \
        vendor/tcl/sita/X_packages/CN_APP/TCL_GoLiveCinema/kdmStart.sh:system/bin/kdmStart.sh \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ATET/lib_All_ATET_Ime.so:system/lib/lib_All_ATET_Ime.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ATET/lib_All_ATET_Market_Crypto.so:system/lib/lib_All_ATET_Market_Crypto.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ATET/lib_All_ATET_Market_Cryptoex.so:system/lib/lib_All_ATET_Market_Cryptoex.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ATET/libAPKProtect.so:system/lib/libAPKProtect.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ATET/libatetime.so:system/lib/libatetime.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ATET/libcrypt.so:system/lib/libcrypt.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ronghedianshi/libicast_hal_tsplayer.so:system/lib/libicast_hal_tsplayer.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_TvLife/libamapv304.so:system/lib/libamapv304.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_TvLife/libamapv304ex.so:system/lib/libamapv304ex.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ALL_Rainbow/libarm_rainbow.so:system/lib/libarm_rainbow.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_ALL_Rainbow/libvinit_rainbow.so:system/lib/libvinit_rainbow.so \

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
