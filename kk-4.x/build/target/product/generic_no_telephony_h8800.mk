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
    TCL_H8800_AppStore3.3 \
    TCL_H8800_AppStoreUpdate \
    TCL_H8800_TCLPackageInstallerMaster \
    TCL_H8800_TV_MARKET \
    TCL_H8800_TVGamepad \
    TCL_H8800_Browser3.0 \
    TCL_H8800_ComMediaPlayer \
    TCL_H8800_common_DeviceAuthention \
    TCL_H8800_common_UserCenter \
    TCL_H8800_CyberUI \
    TCL_H8800_CyberUIRes \
    TCL_H8800_PackageInstallerRenew \
    TCL_H8800_SmartTVHelp \
    TCL_H8800_TV \
    TCL_H8800_MediaBrowser \
    TCL_H8800_mtkVersionUpdate \
    TCL_H8800_Message_Box_Service_3.1plus \
    TCL_H8800_Message_Box_3.1plus \
    TCL_H8800_MenuHelpFor5Key \
    TCL_H8800_Factory \
    TCL_H8800_Bootsetwizard_s \
    TCL_H8800_ImageViewer \
    TCL_H8800_UsageStats \
    TCL_H8800_MT5507_IQIYI_Video \
    TCL_H8800_All_HuanAndroidPay \
    TCL_H8800_ALL_Lexue3.0 \
    TCL_H8800_Settings_DTV \
    TCL_H8800_XiriIME \
    TCL_H8800_Pinyin_IME \
    TCL_H8800_Miracast \
    TCL_H8800_ESticker \
    TCL_H8800_A71S_ShortCutMenu \
    TCL_H8800_nScreen2 \
    TCL_H8800_ShowMode \
    TCL_H8800_3Ddemo \
    TCL_H8800_SnapshotScreen \
    TCL_H8800_TVOS_TVManager_FastBoot \
    TCL_H8800_TVOS_TVManager_NetManager \
    TCL_H8800_TVOS_TVManager_SafeProtection \
    TCL_H8800_TVOS_TVManager_SoftwareManager \
    TCL_H8800_TVOS_TVManager_SystemInfo \
    TCL_H8800_TVOS_TVManager_TaskManager \
    TCL_H8800_TVOS_TVManager_TVExamination \
    TCL_H8800_TVOS_TVManager_TVManager \
    TCL_H8800_WeiXin \
    TCL_H8800_ALL_Rainbow \
    TCL_H8800_MT5507_Angelkaraoke \
    TCL_H8800_youku_tv \
    TCL_H8800_IflytekTlrService \
    TCL_H8800_TvWeather \
    TCL_H8800_MT5507_iCast \
    TCL_H8800_DongleDetectServer \
    TCL_H8800_GoLiveCinema \
    TCL_H8800_All_GoLiveAdvert \
    TCL_H8800_MediaCenter \
    TCL_H8800_TVHealthCheck_MT5507_signed \
    TCL_H8800_TestFlashSpace \
    TCL_H8800_HuanFitness \
    TCL_H8800_HuanTVLove \
    TCL_H8800_sofago \
    TCL_H8800_TvLife \
    TCL_H8800_qqlivetvICNAPP \

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
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_AppStore/blacklist_TCL-CN-MT07H-H8800F.xml:system/etc/appinfo/blacklist_TCL-CN-MT07H-H8800F.xml \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_AppStore/blacklist_TCL-CN-MT07H-A920C.xml:system/etc/appinfo/blacklist_TCL-CN-MT07H-A920C.xml \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/libBaiduMapSDK_v3_1_0.so:system/lib/libBaiduMapSDK_v3_1_0.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/libiomx-ics.so:system/lib/libiomx-ics.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/liblocSDK4d.so:system/lib/liblocSDK4d.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Messager/libvlcjni.so:system/lib/libvlcjni.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MenuFor5Keysigned/libsim-key.so:system/lib/libsim-key.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libiqiyi_media_player.so:system/lib/libiqiyi_media_player.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libiqiyi_player_memalloc.so:system/lib/libiqiyi_player_memalloc.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libkernel.so:system/lib/libkernel.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libKey.so:system/lib/libKey.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libnetdoc.so:system/lib/libnetdoc.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libSpeedEngine.so:system/lib/libSpeedEngine.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_MT5507_IQIYI_Video/libSysInput.so:system/lib/libSysInput.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_lexuefangzhou/libarm.so:system/lib/libarm.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_lexuefangzhou/libvinit.so:system/lib/libvinit.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_xunfeiIME/libsmartaiwrite-jni-v5.so:system/lib/libsmartaiwrite-jni-v5.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_xunfeiIME/libvitvimemsc.so:system/lib/libvitvimemsc.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Pinyin_IME/libjni_pinyinime.so:system/lib/libjni_pinyinime.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/lib_All_ATET_Market_Crypto.so:system/lib/lib_All_ATET_Market_Crypto.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/lib_All_ATET_Market_Cryptoex.so:system/lib/lib_All_ATET_Market_Cryptoex.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/libatetime.so:system/lib/libatetime.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/libAPKProtect.so:system/lib/libAPKProtect.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/lib_All_ATET_Ime.so:system/lib/lib_All_ATET_Ime.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ATET/libcrypt.so:system/lib/libcrypt.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_CyberUI/black_list.json:system/etc/black_list.json \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_CyberUI/cyberui_config.json:system/etc/cyberui_config.json \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ShowMode/libdatasystemdemo.so:system/lib/libdatasystemdemo.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ShowMode/libstlport_shared.so:system/lib/libstlport_shared.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ShowMode/libtcl3d.so:system/lib/libtcl3d.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ShowMode/libtcl3dM.so:system/lib/libtcl3dM.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_nScreen_srv/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudiocnAudioUtil.so:system/lib/libTCL_MT5507_AngelkaraokeAudiocnAudioUtil.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudiocnkaraoke.so:system/lib/libTCL_MT5507_AngelkaraokeAudiocnkaraoke.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudiocnloop.so:system/lib/libTCL_MT5507_AngelkaraokeAudiocnloop.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudiocnMP4codec.so:system/lib/libTCL_MT5507_AngelkaraokeAudiocnMP4codec.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudiocnReverb.so:system/lib/libTCL_MT5507_AngelkaraokeAudiocnReverb.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudiocnSoundtouch.so:system/lib/libTCL_MT5507_AngelkaraokeAudiocnSoundtouch.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeAudioresample.so:system/lib/libTCL_MT5507_AngelkaraokeAudioresample.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeDirectpcm.so:system/lib/libTCL_MT5507_AngelkaraokeDirectpcm.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeKRoom.so:system/lib/libTCL_MT5507_AngelkaraokeKRoom.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokeKRoomJni.so:system/lib/libTCL_MT5507_AngelkaraokeKRoomJni.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_Angelkaraokemixed_AV.so:system/lib/libTCL_MT5507_Angelkaraokemixed_AV.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_Angelkaraokemtk.so:system/lib/libTCL_MT5507_Angelkaraokemtk.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokesingScoreso.so:system/lib/libTCL_MT5507_AngelkaraokesingScoreso.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_Angelkaraoke/libTCL_MT5507_AngelkaraokesingScoreso.so:system/lib/libTCL_MT5507_AngelkaraokeSpeexresampler.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libalgms.so:system/lib/libalgms.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libckeygenerator.so:system/lib/libckeygenerator.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libDES.so:system/lib/libDES.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libgif.so:system/lib/libgif.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libHWDec14.so:system/lib/libHWDec14.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libHWDec16.so:system/lib/libHWDec16.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libHWDec17.so:system/lib/libHWDec17.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libHWDec9.so:system/lib/libHWDec9.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libluajava.so:system/lib/libluajava.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libMtaNativeCrash.so:system/lib/libMtaNativeCrash.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libMultiScreenServer.so:system/lib/libMultiScreenServer.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libNativeRQD.so:system/lib/libNativeRQD.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libottlogin.so:system/lib/libottlogin.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libpilog.so:system/lib/libpilog.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libPlayerCore_neon.so:system/lib/libPlayerCore_neon.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libqqlivetv.so:system/lib/libqqlivetv.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libQQMMANDKSignature.so:system/lib/libQQMMANDKSignature.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libsta_jni.so:system/lib/libsta_jni.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libTLR15.so:system/lib/libTLR15.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libtvaccount.so:system/lib/libtvaccount.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libtvsubp.so:system/lib/libtvsubp.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libtv-ticket-tool.so:system/lib/libtv-ticket-tool.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_WeiXin/libTxCodec_neon.so:system/lib/libTxCodec_neon.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libbasicphrasegrm.so:system/lib/libbasicphrasegrm.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libem_aisound5.so:system/lib/libem_aisound5.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libEventInjector.so:system/lib/libEventInjector.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/liblsopen.so:system/lib/liblsopen.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/liblsopencfg.so:system/lib/liblsopencfg.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libmaindict.so:system/lib/libmaindict.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libmspk.so:system/lib/libmspk.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libmspkcfg.so:system/lib/libmspkcfg.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libphonecost.so:system/lib/libphonecost.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libpinyin.so:system/lib/libpinyin.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libsystemcommongrm.so:system/lib/libsystemcommongrm.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libvitvmsc.so:system/lib/libvitvmsc.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libvitvpinyin.so:system/lib/libvitvpinyin.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/libvitvvad.so:system/lib/libvitvvad.so \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/iflytekdeamon:system/bin/iflytekdeamon \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/addtails.pcm:system/media/addtails.pcm \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/tclxirihelp.dat:system/media/tclxirihelp.dat \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/IflytekTVService.apk:system/app/IflytekTVService.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/iqiyivideo.apk:system/app/iqiyivideo.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/plugin_tcl.apk:system/app/plugin_tcl.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/QiyiVoiceManager4.1.apk:system/app/QiyiVoiceManager4.1.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/Showcome.apk:system/app/Showcome.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/ShowcomeSettings.apk:system/app/ShowcomeSettings.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/speehservices.apk:system/app/speehservices.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/Xiri.apk:system/app/Xiri.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/XiriHAL.apk:system/app/XiriHAL.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_VoiceAssistance/XiriRecorder.apk:system/app/XiriRecorder.apk \
        vendor/tcl/sita/X_packages/CN_H8800_APP/TCL_H8800_ronghedianshi/libicast_hal_tsplayer.so:system/lib/libicast_hal_tsplayer.so \
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
