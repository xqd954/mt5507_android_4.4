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


################################################
# build start
################################################
LOCAL_PATH := $(call my-dir)
CURR_PATH := $(LOCAL_PATH)
#BT_PATH := external/bluetooth/mtk/blueangel

################################################
# build log mask tool
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	logmask.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include
	
include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils libutils

LOCAL_MODULE := btlogmask

LOCAL_PRELINK_MODULE:= false
include $(BUILD_EXECUTABLE)
################################################
# build log mask tool
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	btconfig.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include \
	$(BT_PATH)/bt_cust/inc \
	$(BT_PATH)/btcore/inc
	
include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils libutils libbtcust libbtcusttable

LOCAL_MODULE := btconfig

LOCAL_PRELINK_MODULE:= false
include $(BUILD_EXECUTABLE)
################################################
# build pcm switch tool
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	btpcm.c

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils libutils

#LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libbtpcm

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build sniff tool
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	virtual_sniffs.c

LOCAL_SHARED_LIBRARIES += libcutils libutils

#LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libbtsniff

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
################################################
# build teste mode verification tool
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	bttm.cpp \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include
	
include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils libutils libexttestmode libmedia

LOCAL_C_INCLUDES += \
   frameworks/base/services/audioflinger \
   frameworks/base/services/audioflinger/LAD \
   frameworks/base/include/media \
   frameworks/av/include/media \
   frameworks/base/include \
   system/core/include/cutils
   
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := bttm

LOCAL_PRELINK_MODULE:= false
include $(BUILD_EXECUTABLE)

################################################
# build em command lib
################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	btem.c

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils libutils

LOCAL_MODULE := libbtem

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
################################################
# End of File
################################################
