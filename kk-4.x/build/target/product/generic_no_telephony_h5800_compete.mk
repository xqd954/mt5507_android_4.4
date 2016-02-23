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
    TCL_CMP_CyberUI_IQIYI \
    TCL_CMP_CyberUIRes_IQIYI \
    TCL_CMP_PackageInstallerRenew_IQIYI \
    TCL_F3800_CMP_SmartTVHelp \
    TCLBrowser3.0 \
    TCL_CMP_LIVE_TV \
    TCL_CMP_SOURCE_MANAGER \
    TCL_MediaBrowser \
    TCL_CMP_mtkVersionUpdate \
    TCL_Message_Box_Service_3.1plus \
    TCL_Message_Box_3.1plus \
    TCL_H8800_MenuHelpFor7Key \
    TCL_Factory \
    TCL_CMP_common_UserCenter \
    TCL_common_DeviceAuthention \
    TCL_AdService \
    TCL_Bootsetwizard_s \
    TCL_ImageViewer \
    TCL_UsageStats \
    TCL_ComMediaPlayer \
    TCL_CMP_TVOS_TVManager_FastBoot \
    TCL_CMP_TVOS_TVManager_NetManager \
    TCL_CMP_TVOS_TVManager_SafeProtection \
    TCL_CMP_TVOS_TVManager_SoftwareManager \
    TCL_CMP_TVOS_TVManager_SystemInfo \
    TCL_CMP_TVOS_TVManager_TaskManager \
    TCL_CMP_TVOS_TVManager_TVExamination \
    TCL_CMP_TVOS_TVManager_TVManager \
    TCL_MT5507_IQIYI_Video \
    TCL_All_HuanAndroidPay \
    TCL_ALL_Lexue3.0 \
    TCL_CMP_Settings_DTV \
    TCL_Pinyin_IME \
    TCL_CMP_AppStore3.3 \
    TCL_CMP_AppStoreUpdate \
    TCL_CMP_TCLPackageInstallerMaster \
    TCL_CMP_ShowMode \
    TCL_CMP_3Ddemo \
    TCL_SnapshotScreen \
    TCL_AirplayPlayerActivity \
    TCL_AudioPlayerActivity \
    TCL_Multiscreen_mDNSProtocolStack_TV \
    TCL_WelcomeActivity \
    TCL_CMP_WeiXin \
    TCL_CMP_nScreen2 \
    TCL_CMP_AudioShare_TV \
    TCL_ALL_Rainbow \
    TCL_CMP_youku_tv \
    TCL_CMP_IflytekTlrService \
    TCL_CMP_WeChat_QQLIVETV \
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
    TCL_F3800_CMP_A71S_ShortCutMenu \
    TCL_CMP_History \

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
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_AppStore/libtclappstore.so:system/lib/libtclappstore.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_AppStore/libtclappstoredata.so:system/lib/libtclappstoredata.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_AppStore/blacklist_TCL-CN-MT5507-F3800T.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-F3800T.xml \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_AppStore/blacklist_TCL-CN-MT5507-A810T.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-A810T.xml \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_AppStore/blacklist_TCL-CN-MT5507-B385AT.xml:system/etc/appinfo/blacklist_TCL-CN-MT5507-B385AT.xml \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/libBaiduMapSDK_v3_1_0.so:system/lib/libBaiduMapSDK_v3_1_0.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/libiomx-ics.so:system/lib/libiomx-ics.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/liblocSDK4d.so:system/lib/liblocSDK4d.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_Messager/libvlcjni.so:system/lib/libvlcjni.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MenuFor7Keysigned/libsim-key.so:system/lib/libsim-key.so \
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
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_CyberUI_iqiyi/black_list.json:system/etc/black_list.json \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_CyberUI_iqiyi/cyberui_config.json:system/etc/cyberui_config.json \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_nScreen_srv/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_nScreen_srv/libaaccodec.so:system/lib/libaaccodec.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libalgms.so:system/lib/libalgms.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libluajava.so:system/lib/libluajava.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libTLR15.so:system/lib/libTLR15.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libckeygenerator.so:system/lib/libckeygenerator.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libDES.so:system/lib/libDES.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libgif.so:system/lib/libgif.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libHWDec9.so:system/lib/libHWDec9.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libHWDec14.so:system/lib/libHWDec14.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libHWDec16.so:system/lib/libHWDec16.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libHWDec17.so:system/lib/libHWDec17.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libMMANDKSignature.so:system/lib/libMMANDKSignature.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libMtaNativeCrash.so:system/lib/libMtaNativeCrash.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libMultiScreenServer.so:system/lib/libMultiScreenServer.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libNativeRQD.so:system/lib/libNativeRQD.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libottlogin.so:system/lib/libottlogin.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libp2pproxy.so:system/lib/libp2pproxy.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libpilog.so:system/lib/libpilog.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libPlayerCore_neon.so:system/lib/libPlayerCore_neon.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libqqlivetv.so:system/lib/libqqlivetv.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libQQMMANDKSignature.so:system/lib/libQQMMANDKSignature.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libsta_jni.so:system/lib/libsta_jni.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libtvaccount.so:system/lib/libtvaccount.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libtvsubp.so:system/lib/libtvsubp.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libtv-ticket-tool.so:system/lib/libtv-ticket-tool.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_WeiXin/libTxCodec_neon.so:system/lib/libTxCodec_neon.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_ShowMode/lib/libdatasystemdemo.so:system/lib/libdatasystemdemo.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_ShowMode/lib/libstlport_shared.so:system/lib/libstlport_shared.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_ShowMode/lib/libtcl3d.so:system/lib/libtcl3d.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_ShowMode/lib/libtcl3dM.so:system/lib/libtcl3dM.so \
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
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libbasicphrasegrm.so:system/lib/libbasicphrasegrm.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libem_aisound5.so:system/lib/libem_aisound5.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libEventInjector.so:system/lib/libEventInjector.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/liblsopen.so:system/lib/liblsopen.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/liblsopencfg.so:system/lib/liblsopencfg.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libmaindict.so:system/lib/libmaindict.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libmspk.so:system/lib/libmspk.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libmspkcfg.so:system/lib/libmspkcfg.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libphonecost.so:system/lib/libphonecost.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libshowcome_xiri_d.so:system/lib/libshowcome_xiri_d.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libsmartaiwrite-jni-v5.so:system/lib/libsmartaiwrite-jni-v5.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libsystemcommongrm.so:system/lib/libsystemcommongrm.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libvitvmsc.so:system/lib/libvitvmsc.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libvitvpinyin.so:system/lib/libvitvpinyin.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libvitvvad.so:system/lib/libvitvvad.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/libxdriver_xiri_d.so:system/lib/libxdriver_xiri_d.so \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/xiriservice_tcl:system/bin/xiriservice_tcl \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/addtails.pcm:system/media/addtails.pcm \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/tclxirihelp.dat:system/media/tclxirihelp.dat \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/IflytekTVService.apk:system/app/IflytekTVService.apk \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/QiyiVoiceManager4.1.apk:system/app/QiyiVoiceManager4.1.apk \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/SpeechService.apk:system/app/SpeechService.apk \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/Xiri.apk:system/app/Xiri.apk \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/SystemXiri.apk:system/app/SystemXiri.apk \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/XiriHAL.apk:system/app/XiriHAL.apk \
        vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_F3800_CMP_VoiceAssistance/XiriRecorder.apk:system/app/XiriRecorder.apk \

PRODUCT_PROPERTY_OVERRIDES += \
    ro.carrier=unknown

#copy file for showmode app
$(shell rm -rf $(ANDROID_PRODUCT_OUT)/system/media/ShowMode_Res)
$(shell cp -rf vendor/tcl/sita/X_packages/CN_CMP_APP/TCL_CMP_ShowMode/ShowMode_Res_MT5507 $(ANDROID_PRODUCT_OUT)/system/media/ShowMode_Res)
$(shell chmod -R 777 $(ANDROID_PRODUCT_OUT)/system/media/ShowMode_Res)
    
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
