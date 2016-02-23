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

ifeq ($(TCL_PRODUCT_BRANCH),EM)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_em.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_H8800)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h8800.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),ROWA_CN_X1000)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h8800.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_C1)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_C1.mk)
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_H5800)
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic_no_telephony_h5800.mk)
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
$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony.mk)

# Overrides
PRODUCT_BRAND := generic_mips
PRODUCT_DEVICE := generic_mips
PRODUCT_NAME := generic_mips
