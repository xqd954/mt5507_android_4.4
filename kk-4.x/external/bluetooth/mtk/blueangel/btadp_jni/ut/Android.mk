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

ifeq ($(strip $(BT_UT)), yes)

#BT_PATH := $(MTK_PATH_SOURCE)/protect/external/bluetooth/blueangel
CUR_PATH := $(call my-dir)

#############################################
# Build JNI - PAN UT
#############################################

LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	extpan_ut.cpp

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \
        $(BT_PATH)/include \
        $(BT_PATH)/include/profiles \
        $(BT_PATH)/include/common \
        $(BT_PATH)/include/common/default \
        $(BT_PATH)/btcore/inc \
	$(BT_PATH)/test/ut_simulator

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_STATIC_LIBRARIES += \
        libmtkbtextsys \

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libutils \
        libbinder \
        libmtkbtextpan \
	libextut_simulator

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libextpan_ut

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


#############################################
# Build JNI - AVRCP UT
#############################################

LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	extavrcp_ut.cpp

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \
        $(BT_PATH)/include \
        $(BT_PATH)/include/profiles \
        $(BT_PATH)/include/common \
        $(BT_PATH)/include/common/default \
        $(BT_PATH)/btcore/inc \
	$(BT_PATH)/test/ut_simulator

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_STATIC_LIBRARIES += \
        libmtkbtextsys \

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libutils \
        libbinder \
	libextut_simulator

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libextavrcp_ut

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#############################################
# Build JNI - HID UT
#############################################

LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	exthid_ut.cpp

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \
        $(BT_PATH)/include \
        $(BT_PATH)/include/profiles \
        $(BT_PATH)/include/common \
        $(BT_PATH)/include/common/default \
        $(BT_PATH)/btcore/inc \
	$(BT_PATH)/test/ut_simulator

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_STATIC_LIBRARIES += \
        libmtkbtextsys \

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libutils \
        libbinder \
        libmtkbtextpan \
	libextut_simulator

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libexthid_ut

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#############################################
# PBAP
#############################################

LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	extpbap_ut.cpp

LOCAL_C_INCLUDES += \
	$(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \
        $(BT_PATH)/include \
        $(BT_PATH)/include/profiles \
        $(BT_PATH)/include/common \
        $(BT_PATH)/include/common/default \
        $(BT_PATH)/btcore/inc \
	$(BT_PATH)/test/ut_simulator

#LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_STATIC_LIBRARIES += \
        libmtkbtextsys \

LOCAL_SHARED_LIBRARIES += \
	libcutils \
        libutils \
        libbinder \
        libextpbap \
	libextut_simulator

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libextpbap_ut

LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)

#############################################
# PBAP END
#############################################
endif
#############################################
# End of file
#############################################
