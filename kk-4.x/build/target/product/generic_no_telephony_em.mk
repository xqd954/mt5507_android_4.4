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
    CertInstaller \
    Email \
    Exchange2 \
    Gallery2 \
    InputDevices \
    Music \
    MusicFX \
    OneTimeInitializer \
    PrintSpooler \
    Provision \
    TeleService \
    CalendarProvider \
    bluetooth-health \
    hostapd \
    wpa_supplicant.conf \
    TCL_CrossUI \
    AdService \
    AbroadPackageInstallerHIS2V3.2 \
    AppStoreAbroadV3.2_FOTA \
    ComMediaPlayer \
    Deezer_v1.0.6 \
    TCL_common_DeviceAuthention \
    TCL_All_GoLive \
    MediaBrowser \
    MessageBox \
    MultiScreenInteraction_TV_4.0 \
    SmartTVHelp \
    SystemSettings \
    versionUpdate \
    TCL_TV \
    TCLBrowser3.0 \
    TclSystemUI \
    TCL_All_Twitter \
    UsageStats \
    TCL_ALL_ZingTV \
    ImageViewer \
    TCL_Factory \
    MenuHelpFor5Key \
    inputmethodrs_version2.0.13 \
    InitSetup \
    ZingMP3 \
    mytv101 \
    T_Cast \
    VirtualKey \
    FileExplorer \
    ESticker3.0 \
    PvrPlayer

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
        frameworks/av/media/libeffects/data/audio_effects.conf:system/etc/audio_effects.conf\
        vendor/tcl_em/sita/X_packages/EM_APP/AppStore/libtclappstore.so:system/lib/libtclappstore.so \
        vendor/tcl_em/sita/X_packages/EM_APP/AppStore/libtclappstoredata.so:system/lib/libtclappstoredata.so \
        vendor/tcl_em/sita/X_packages/EM_APP/MultiScreenInteraction/libKeyAndMouse.so:system/lib/libKeyAndMouse.so \
        vendor/tcl_em/sita/X_packages/EM_APP/Deezer/libdeezer.so:system/lib/libdeezer.so \
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/lib_All_golive_forcetv.so:system/lib/lib_All_golive_forcetv.so \
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/libDCF_JNI.so:system/lib/libDCF_JNI.so \
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/libDPSStreamEngine.so:system/lib/libDPSStreamEngine.so\
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/libDSAS.so:system/lib/libDSAS.so \
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/libGoliveHelper.so:system/lib/libGoliveHelper.so \
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/libICPDownload.so:system/lib/libICPDownload.so \
        vendor/tcl_em/sita/X_packages/EM_APP/GoLive/libMD5.so:system/lib/libMD5.so \
	vendor/tcl_em/sita/X_packages/EM_APP/InputMethod/libmacroimepinyin.so:system/lib/libmacroimepinyin.so \
	vendor/tcl_em/sita/X_packages/EM_APP/5in1Keys/libsim-key.so:system/lib/libsim-key.so \
	vendor/tcl_em/sita/X_packages/EM_APP/MyTV/libvinit.so:system/lib/libvinit.so
        

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
