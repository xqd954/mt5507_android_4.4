# Copyright (C) 2011 The Android Open Source Project
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
# Product-specific compile-time definitions.
#

USE_CAMERA_STUB := true
BOARD_USES_GENERIC_AUDIO := false

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_CPU_VARIANT := cortex-a9

TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := false
TARGET_NO_RECOVERY := false

HAVE_HTC_AUDIO_DRIVER := false
BOARD_USES_ALSA_AUDIO := true
BUILD_WITH_ALSA_UTILS := true
BOARD_USES_MT53XX_AUDIO := true

BOARD_HAVE_BLUETOOTH := true

ifeq "$(SYS_IMG_FS)" "ext4"
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_FLASH_BLOCK_SIZE := 512
TARGET_USERIMAGES_SPARSE_EXT_DISABLED := true
#boot.image, 20M
BOARD_BOOTIMAGE_MAX_SIZE := 20971520
BOARD_BOOTIMAGE_PARTITION_SIZE := 20971520

#recovery.image, 20MB=>48MB
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 50331648

ifeq "$(H8800_C1)" "true"
#system.image, 270M==>300M==>330M==>360M=>410M=>750MB=>788MB=>1088(1100-12)=>1288(1300-12)
#    system image size: 1288MB
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1383071744
else
#system.image, 270M==>300M==>330M==>360M=>410M=>750MB=>788MB=>1088(1100-12)
#    system image size: 1088MB
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1140850688
endif
#    system image spare size: 12MB (for dmverity + recovery_from_boot.p(~= 1MB))
BOARD_SYSTEMIMAGE_PARTITION_SPARE_SIZE := 12582912

ifeq "$(f3800_1G)" "true"
#userdata.image 371M==>341M==>336M==>512M==>1024M=>2.1G=>5.6G
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 16777216
BOARD_USERDATAIMAGE_PARTITION_SIZE := 6006243328
else
ifeq "$(H8800_C1)" "true"
#userdata.image 371M==>341M==>336M==>512M==>1024M=>2.1G=>1.9G
BOARD_USERDATAIMAGE_PARTITION_SIZE := 2030043136
else
#userdata.image 371M==>341M==>336M==>512M==>1024M=>2.1G
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 16777216
BOARD_USERDATAIMAGE_PARTITION_SIZE := 2252341248
endif
endif

#cache.image 100M=>120MB=>32MB
BOARD_CACHEIMAGE_PARTITION_SIZE := 33554432
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4

#impdata.image,8MB
BOARD_IMPDATAIMAGE_PARTITION_SIZE := 8388608
BOARD_IMPDATAIMAGE_FILE_SYSTEM_TYPE := ext4

#tclconfig.image, 32MB
BOARD_TCLCONFIGIMAGE_PARTITION_SIZE := 33554432
BOARD_TCLCONFIGIMAGE_FILE_SYSTEM_TYPE := ext4

#tvos.image, 256MB
BOARD_TVOSIMAGE_PARTITION_SIZE := 268435456
BOARD_TVOSIMAGE_FILE_SYSTEM_TYPE := ext4

#tcldata.image, 16MB
BOARD_TCLDATAIMAGE_PARTITION_SIZE := 16777216
BOARD_TCLDATAIMAGE_FILE_SYSTEM_TYPE := ext4

#eMMC
BOARD_NAND_PAGE_SIZE  := 512
BOARD_FLASH_BLOCK_SIZE := 512
BOARD_NAND_SPARE_SIZE := 0
else
TARGET_USERIMAGES_USE_UBIFS := true
# NAND Flash Page/Block information for otapackage
BOARD_NAND_PAGE_SIZE := 2048
BOARD_FLASH_BLOCK_SIZE := $(BOARD_NAND_PAGE_SIZE) * 64
BOARD_NAND_SPARE_SIZE := 0
endif

# FOTA:Begin added by wujy@tcl.com
ifeq "${TCL_PRODUCT_BRANCH}" "ROWA_CN_X1000"
TCL_BUILD_ID := V8-0MT0704-LF1V014
endif

# FOTA:Begin added by w.y@tcl.com
ifeq "${TCL_PRODUCT_BRANCH}" "CN_C1"
TCL_BUILD_ID := V8-0MT0705-LF1V012
endif

# FOTA:Begin added by pengpeng01@tcl.com
ifeq "${TCL_PRODUCT_BRANCH}" "CN_H8800"
TCL_BUILD_ID := V8-0MT0702-LF1V042
endif


ifeq "${TCL_PRODUCT_BRANCH}" "CN_H5800"
TCL_BUILD_ID := V8-0MT0703-LF1V027
endif

ifeq "${TCL_PRODUCT_BRANCH}" "CN_F3850A"
TCL_BUILD_ID := V8-0MT0706-LF1V007
endif

ifeq "${TCL_PRODUCT_BRANCH}" "CN_F3800"
TCL_BUILD_ID := V8-0MT0701-LF1V050
endif

ifeq "${TCL_PRODUCT_BRANCH}" "CN_HK"
TCL_BUILD_ID := V8-0MT0707-LF1V001
endif

ifeq "${TCL_PRODUCT_BRANCH}" "CN_P1"
TCL_BUILD_ID := V8-0MT0708-LF1V006
endif

ifeq "${TCL_PRODUCT_BRANCH}" "CN_TEN"
TCL_BUILD_ID := V8-0MT0701-LF1V125
endif

ifeq "${TCL_PRODUCT_BRANCH}" "EM"
TCL_BUILD_ID := V8-MT56551-LF1V004
endif

FOTA_OUT := $(TOP)/../../fota
OTA_INCREMENTAL_FROM := $(TOP)/../../fota/mid_zip
INC_NUM = 3
OTA_WITH_TCLDATA := false
OTA_WITH_TCLCONFIG := true
OTA_WITH_TVOS := true
TARGET_RELEASETOOLS_EXTENSIONS := device/mtk/mtk/$(IC_SETTING)
#FOTA:end

# Set /system/bin/sh to mksh, not ash, to test the transition.
TARGET_SHELL := mksh

DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)

ifeq "true" "$(ANDROID_SW_RENDERING)"
BOARD_EGL_CFG := $(DEVICE_STRING)/egl_sw.cfg
USE_OPENGL_RENDERER := false
else
ifeq "${GPU_SETTING}" "IMG"
BOARD_EGL_CFG := $(DEVICE_STRING)/egl_sgx.cfg
USE_OPENGL_RENDERER := true
else
BOARD_EGL_CFG := $(DEVICE_STRING)/egl.cfg
USE_OPENGL_RENDERER := true
endif
endif

ifeq "${IC_SETTING}" "mt5395"
    TARGET_BOARD_PLATFORM := mt5395
    TARGET_ARCH_VARIANT := armv6-vfp
endif

ifeq "${IC_SETTING}" "mt5396"
    TARGET_BOARD_PLATFORM := mt5396
    TARGET_ARCH_VARIANT := armv7-a
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

ifeq "${IC_SETTING}" "mt5880"
    TARGET_BOARD_PLATFORM := mt5880
    TARGET_ARCH := arm
    TARGET_ARCH_VARIANT := armv7-a
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

ifeq "${IC_SETTING}" "mt5398"
    TARGET_BOARD_PLATFORM := mt5398
    TARGET_ARCH := arm
    TARGET_ARCH_VARIANT := armv7-a
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

ifeq "${IC_SETTING}" "mt5399"
    TARGET_BOARD_PLATFORM := mt5399
    TARGET_ARCH := arm
    TARGET_ARCH_VARIANT := armv7-a-neon
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

ifeq "${IC_SETTING}" "mt5890"
    TARGET_BOARD_PLATFORM := mt5890
    TARGET_ARCH := arm
    TARGET_ARCH_VARIANT := armv7-a-neon
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

ifeq "${IC_SETTING}" "mt5861"
    TARGET_BOARD_PLATFORM := mt5861
    TARGET_ARCH := arm
    TARGET_ARCH_VARIANT := armv7-a-neon
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

ifeq "${IC_SETTING}" "mt5882"
    TARGET_BOARD_PLATFORM := mt5882
    TARGET_ARCH := arm
    TARGET_ARCH_VARIANT := armv7-a-neon
    ARCH_ARM_HAVE_TLS_REGISTER := true
endif

TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK := true

# For WiFi Setting
WPA_SUPPLICANT_VERSION ?= VER_0_8_X
BOARD_WIFI_VENDOR := mtk

ifeq ($(BOARD_WIFI_VENDOR), mtk)
	WPA_SUPPLICANT_VERSION := VER_0_8_X

	BOARD_WPA_SUPPLICANT_DRIVER := NL80211
	#BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_mtk

	BOARD_HOSTAPD_DRIVER        := NL80211
	#BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_mtk

	BOARD_WLAN_DEVICE := mt76xx

else ifeq ($(BOARD_WIFI_VENDOR), realtek)
    WPA_SUPPLICANT_VERSION := VER_0_8_X
    #BOARD_WPA_SUPPLICANT_DRIVER := WEXT
    BOARD_WPA_SUPPLICANT_DRIVER := NL80211
    BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_rtl
    BOARD_HOSTAPD_DRIVER        := NL80211
    BOARD_HOSTAPD_PRIVATE_LIB   := lib_driver_cmd_rtl

    BOARD_WLAN_DEVICE := rtl8192cu
    #BOARD_WLAN_DEVICE := rtl8192du
    #BOARD_WLAN_DEVICE := rtl8192ce
    #BOARD_WLAN_DEVICE := rtl8192de
    #BOARD_WLAN_DEVICE := rtl8723as
    #BOARD_WLAN_DEVICE := rtl8723au
    #BOARD_WLAN_DEVICE := rtl8188es

    WIFI_DRIVER_MODULE_NAME   := 8192cu
    #WIFI_DRIVER_MODULE_PATH   := "/system/lib/modules/8192cu.ko"

    WIFI_DRIVER_MODULE_ARG    := ""
    WIFI_FIRMWARE_LOADER      := ""
    WIFI_DRIVER_FW_PATH_STA   := ""
    WIFI_DRIVER_FW_PATH_AP    := ""
    WIFI_DRIVER_FW_PATH_P2P   := ""
    WIFI_DRIVER_FW_PATH_PARAM := ""
endif

#TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"
#TARGET_RECOVERY_UI_LIB :=librecovery_ui_mtk
#TARGET_RELEASETOOLS_EXTENSIONS := device/mtk/$(IC_SETTING)

# SELINUX custom: start
BOARD_SEPOLICY_DIRS := \
    device/$(BRAND)/$(ODM)/sepolicy

BOARD_SEPOLICY_REPLACE := \
     installd.te \
     netd.te \
     vold.te \
	 zygote.te \
	 file.te
# SELINUX custom: end

