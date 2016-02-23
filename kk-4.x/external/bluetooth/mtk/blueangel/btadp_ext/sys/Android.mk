# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.


LOCAL_PATH := $(call my-dir)
#BT_PATH := external/bluetooth/mtk/blueangel

include $(CLEAR_VARS)

#LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_SRC_FILES := \
	ext_ashm.cpp \
	../../btadp_int/sys/bt_adp_ashm.cpp \
	../../btadp_int/sys/bt_adp_ashm_impl.cpp \
	ext_ucs2.c \
	../../btadp_int/common/ucs2/bt_ucs2.c \
	ext_osal.c \
	../../btadp_int/sys/bt_osal.c \
	ext_osal_string.c \
	../../btadp_int/sys/bt_osal_string.c \
	ext_osal_charset.c \
	../../btadp_int/sys/bt_osal_charset.c \
	ext_nbuffer.c \
	ext_dummy.c \
	../../btadp_int/sys/bt_adp_fs_ucs2.c \
	../../btadp_int/sys/bt_adp_fs.c \
	ext_ring.c \
	../../btadp_int/sys/ring.c \

#	ext_fs_ucs2.c \

#	../../btcore/os/bt_osapi.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btcore/inc \
	$(BT_PATH)/btcore/inc/sys \
	$(BT_PATH)/btcore/btprofiles/include \
	$(BT_PATH)/btadp_int/driver/include \
	$(BT_PATH)/btadp_int/driver/uart/platform \
	$(BT_PATH)/btadp_int/include \
	$(BT_PATH)/dummyinc \
	$(BT_PATH)/linuxsystem/include \
	$(BT_PATH)/btadp_int/common/sbc \
	$(BT_PATH)/_bt_scripts/include \

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libmtkbtextsys

include $(BUILD_STATIC_LIBRARY)
