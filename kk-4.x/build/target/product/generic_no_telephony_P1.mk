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
    TCL_H8800_TvSymbolRecognize \
    TCL_H8800_AdService \
    TCL_H8800_AirplayPlayerActivity \
    TCL_H8800_AudioPlayerActivity \
    TCL_H8800_Multiscreen_mDNSProtocolStack_TV \
    TCL_H8800_WelcomeActivity \
    TCL_P1_AppStore3.3 \
    TCL_P1_AppStoreUpdate \
    TCL_P1_PackageInstallerHIS2V3.2_CN \
    TCL_H8800_Browser3.0 \
    TCL_H8800_ComMediaPlayer \
    TCL_H8800_common_DeviceAuthention \
    TCL_H8800_common_UserCenter \
    TCL_P1_CyberUI \
    TCL_P1_PackageInstallerRenew \
    TCL_P1_SmartTVHelp \
    TCL_H8800_TV \
    TCL_P1_MediaBrowser \
    TCL_P1_mtkVersionUpdate \
    TCL_H8800_Message_Box_Service_3.1plus \
    TCL_H8800_Message_Box_3.1plus \
    TCL_H8800_MenuHelpFor5Key \
    TCL_H8800_Factory \
    TCL_H8800_Bootsetwizard_s \
    TCL_H8800_ImageViewer \
    TCL_H8800_UsageStats \
    TCL_H8800_All_HuanAndroidPay \
    TCL_H8800_ALL_Lexue3.0 \
    TCL_P1_Settings \
    TCL_H8800_Pinyin_IME \
    TCL_P1_Miracast \
    TCL_H8800_ESticker \
    TCL_P1_nScreen2 \
    TCL_H8800_SnapshotScreen \
    TCL_P1_TVOS_TVManager_FastBoot \
    TCL_P1_TVOS_TVManager_NetManager \
    TCL_P1_TVOS_TVManager_SafeProtection \
    TCL_P1_TVOS_TVManager_SoftwareManager \
    TCL_P1_TVOS_TVManager_SystemInfo \
    TCL_P1_TVOS_TVManager_TaskManager \
    TCL_P1_TVOS_TVManager_TVExamination \
    TCL_P1_TVOS_TVManager_TVManager \
    TCL_P1_WeiXin \
    TCL_H8800_ALL_Rainbow \
    TCL_P1_youku_tv \
    TCL_P1_IflytekTlrService \
    TCL_P1_TvWeather \
    TCL_P1_MT5507_iCast \
    TCL_P1_DongleDetectServer \
    TCL_H8800_GoLiveCinema \
    TCL_H8800_All_GoLiveAdvert \
    TCL_P1_MediaCenter \
    TCL_H8800_TVHealthCheck_MT5507_signed \
    TCL_H8800_TestFlashSpace \
    TCL_H8800_HuanFitness \
    TCL_H8800_HuanTVLove \
    TCL_H8800_TvLife \
    TCL_P1_QQLiveTV \
    TCL_P1_TheK_Video \
    TCL_P1_T2_GameHall \
    TCL_P1_QQ_GameHall \
    TCL_P1_ScreenSaver \
    TCL_H8800_TV_MARKET \
    TCL_H8800_TVGamepad \
		TCL_BAMA_BamaLauncher \
    TCL_BAMA_XmppManager \
    TCL_BAMA_DouguoRecipes \
    TCL_BAMA_HomeEmailBox \
    TCL_BAMA_imgoTV \
    TCL_BAMA_MHSFortuneTV \
    TCL_BAMA_WuUp \
    TCL_BAMA_zhwnltv \
    TCL_P1_ShowMode \
    TCL_P1_3Ddemo \
    TCL_P1_AudioShare_TV \

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
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ACR/libChannelSwitchDetector.so:system/lib/libChannelSwitchDetector.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ACR/libFingerprintGenerator.so:system/lib/libFingerprintGenerator.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ACR/libopencv_java.so:system/lib/libopencv_java.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/libBaiduMapSDK_v3_1_0.so:system/lib/libBaiduMapSDK_v3_1_0.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/libiomx-ics.so:system/lib/libiomx-ics.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/liblocSDK4d.so:system/lib/liblocSDK4d.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/libvlcjni.so:system/lib/libvlcjni.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MenuFor5Keysigned/libsim-key.so:system/lib/libsim-key.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_lexuefangzhou/libarm.so:system/lib/libarm.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_lexuefangzhou/libvinit.so:system/lib/libvinit.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Pinyin_IME/libjni_pinyinime.so:system/lib/libjni_pinyinime.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_Appstore/blacklist_TCL-CN-MT07B-P1-F.xml:system/etc/appinfo/blacklist_TCL-CN-MT07B-P1-F.xml \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_CyberUI/black_list.json:system/etc/black_list.json \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_CyberUI/cyberui_config.json:system/etc/cyberui_config.json \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_nScreen_srv/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_WeiXin/libalgms.so:system/lib/libalgms.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_WeiXin/libluajava.so:system/lib/libluajava.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_WeiXin/libTLR15.so:system/lib/libTLR15.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_ronghedianshi/libicast_hal_tsplayer.so:system/lib/libicast_hal_tsplayer.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/lib_All_golive_forcetv.so:system/lib/lib_All_golive_forcetv.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libDCF_JNI.so:system/lib/libDCF_JNI.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libDPSStreamEngine.so:system/lib/libDPSStreamEngine.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libDSAS.so:system/lib/libDSAS.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libGoliveHelper.so:system/lib/libGoliveHelper.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libICPDownload.so:system/lib/libICPDownload.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libMD5.so:system/lib/libMD5.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libtsplayer.so:system/lib/libtsplayer.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/libvooleglib.so:system/lib/libvooleglib.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/kdmServer:system/bin/kdmServer \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_GoLiveCinema/kdmStart.sh:system/bin/kdmStart.sh \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_HuanFitness/libamapv304.so:system/lib/libamapv304.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_HuanFitness/libamapv304ex.so:system/lib/libamapv304ex.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libckeygenerator.so:system/lib/libckeygenerator.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libDES.so:system/lib/libDES.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libgif.so:system/lib/libgif.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libHWDec14.so:system/lib/libHWDec14.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libHWDec16.so:system/lib/libHWDec16.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libHWDec17.so:system/lib/libHWDec17.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libHWDec9.so:system/lib/libHWDec9.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libMtaNativeCrash.so:system/lib/libMtaNativeCrash.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libMultiScreenServer.so:system/lib/libMultiScreenServer.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libNativeRQD.so:system/lib/libNativeRQD.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libottlogin.so:system/lib/libottlogin.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libp2pproxy.so:system/lib/libp2pproxy.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libpilog.so:system/lib/libpilog.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libPlayerCore_neon.so:system/lib/libPlayerCore_neon.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libqqlivetv.so:system/lib/libqqlivetv.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libQQMMANDKSignature.so:system/lib/libQQMMANDKSignature.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libsta_jni.so:system/lib/libsta_jni.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libtvaccount.so:system/lib/libtvaccount.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libtvsubp.so:system/lib/libtvsubp.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libtv-ticket-tool.so:system/lib/libtv-ticket-tool.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_Video/libTxCodec_neon.so:system/lib/libTxCodec_neon.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_GameHall/libNativeRQD.so:system/lib/libNativeRQD.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_GameHall/libtpnsSecurity.so:system/lib/libtpnsSecurity.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_QQ_GameHall/libuinputsupport.so:system/lib/libuinputsupport.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/lib_All_ATET_Market_Crypto.so:system/lib/lib_All_ATET_Market_Crypto.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/lib_All_ATET_Market_Cryptoex.so:system/lib/lib_All_ATET_Market_Cryptoex.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/libatetime.so:system/lib/libatetime.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/libAPKProtect.so:system/lib/libAPKProtect.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/lib_All_ATET_Ime.so:system/lib/lib_All_ATET_Ime.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/libcrypt.so:system/lib/libcrypt.so \
	    	vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/lib_All_imgoTV_bitmaps.so:system/lib/lib_All_imgoTV_bitmaps.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/lib_All_imgoTV_nn_tv_air_control.so:system/lib/lib_All_imgoTV_nn_tv_air_control.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/lib_All_imgoTV_nn_tv_client.so:system/lib/lib_All_imgoTV_nn_tv_client.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/lib_All_imgoTV_starcor_xul.so:system/lib/lib_All_imgoTV_starcor_xul.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/lib_All_imgoTV_xul.so:system/lib/lib_All_imgoTV_xul.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/lib_All_imgoTV_xul-v8.so:system/lib/lib_All_imgoTV_xul-v8.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_imogoTV/libpl_All_imgoTV_droidsonroids_gif.so:system/lib/libpl_All_imgoTV_droidsonroids_gif.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libBaiduMapSDK_v3_3_0_15.so:system/lib/libBaiduMapSDK_v3_3_0_15.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libbdpush_V2_2.so:system/lib/libbdpush_V2_2.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libBDTTSDecoder_V1.so:system/lib/libBDTTSDecoder_V1.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libbspatch.so:system/lib/libbspatch.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libhello.so:system/lib/libhello.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/liblocSDK5.so:system/lib/liblocSDK5.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libusb_bak.so:system/lib/libusb_bak.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_MHSFortuneTV/libusbpd.so:system/lib/libusbpd.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_WuUp/libarm.so:system/lib/libarm.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_WuUp/libvinit.so:system/lib/libvinit.so \
        vendor/tcl/sita/X_packages/CN_APP/TCL_BAMA_BamaLauncher/liblocSDK4d.so:system/lib/liblocSDK4d.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_ShowMode/libdatasystemdemo.so:system/lib/libdatasystemdemo.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_ShowMode/libstlport_shared.so:system/lib/libstlport_shared.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_ShowMode/libtcl3d.so:system/lib/libtcl3d.so \
        vendor/tcl/sita/X_packages/CN_P1_APP/TCL_P1_ShowMode/libtcl3dM.so:system/lib/libtcl3dM.so \

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
