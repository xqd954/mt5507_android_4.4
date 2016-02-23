#
# Copyright (C) 2009 The Android Open Source Project
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

# This is a build configuration for a full-featured build of the
# Open-Source part of the tree. It's geared toward a US-centric
# build of the emulator, but all those aspects can be overridden
# in inherited configurations.

PRODUCT_PACKAGES := \
    libfwdlockengine \
    OpenWnn \
    PinyinIME \
    libWnnEngDic \
    libWnnJpnDic \
    libwnndict 

PRODUCT_PROPERTY_OVERRIDES := \
    ro.com.android.dateformat=MM-dd-yyyy \
    ro.config.ringtone=Ring_Synth_04.ogg \
    ro.config.notification_sound=pixiedust.ogg

# Put en_US first in the list, so make it default.
PRODUCT_LOCALES := en_US

# Include drawables for all densities
PRODUCT_AAPT_CONFIG := normal hdpi xhdpi xxhdpi

# Get some sounds
$(call inherit-product-if-exists, frameworks/base/data/sounds/AllAudio.mk)

# Get the TTS language packs
$(call inherit-product-if-exists, external/svox/pico/lang/all_pico_languages.mk)

# Get a list of languages.
$(call inherit-product, $(SRC_TARGET_DIR)/product/locales_full.mk)

# Get everything else from the parent package
ifeq ($(TCL_PRODUCT_BRANCH),EM)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_em.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_H8800)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h8800.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),ROWA_CN_X1000)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_x1000.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_H5800)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h5800.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_C1)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_C1.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_F3850A)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_f3850A.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_HK)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_hk.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_TEN)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_tencent.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_F3800_CMP)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_f3800_compete.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_H5800_CMP)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h5800_compete.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_P1)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_P1.mk)
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_H8800_CMP)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h8800_compete.mk)
else
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony.mk)
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif
