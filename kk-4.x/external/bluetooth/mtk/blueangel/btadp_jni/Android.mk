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

#BT_PATH := external/bluetooth/mtk/blueangel
CUR_PATH := $(call my-dir)

#############################################
# Build JNI - FTP
#############################################
LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        extftp_jni.cpp \

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_STATIC_LIBRARIES += \
        libmtkbtextsys \

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libutils \
        libbinder \
        libextftp \


LOCAL_MODULE := libextftp_jni

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#############################################
# BPP START
#############################################
#bpp java
bpp_module_tags := user
# ----------------------------------------------

#include $(CLEAR_VARS)

#LOCAL_PACKAGE_NAME := BPP

#LOCAL_SRC_FILES := $(call all-subdir-java-files)

#LOCAL_JNI_SHARED_LIBRARIES := libextbpp_jni

#LOCAL_CERTIFICATE := shared

#include $(BUILD_PACKAGE)


#bpp jni part:
#-----------------------------------------------

include $(CLEAR_VARS)


LOCAL_MODULE := libextbpp_jni

LOCAL_SRC_FILES := \
    extbpp_jni.cpp

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/native/include \
    $(BT_PATH)/btadp_ext/include \
    system/core/libcutils \
    $(JNI_H_INCLUDE)

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libextbpp \

#LOCAL_STATIC_LIBRARIES := \
     libbtbpp \

LOCAL_CFLAGS +=

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#-----------------------------------------------
#include $(call all-makefiles-under, $(LOCAL_PATH))
#############################################
# BPP END
#############################################

#############################################
# BIP
#############################################
#bip jni part:
#-----------------------------------------------
include $(CLEAR_VARS)

LOCAL_MODULE := libextbip_jni

LOCAL_SRC_FILES := \
	extbip_jni.cpp \

LOCAL_C_INCLUDES += \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include \
        system/core/libcutils \
	$(JNI_H_INCLUDE) \

#LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libextbip \


LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)
#############################################
# BIP END
#############################################

#############################################
# Build JNI - SIMAP
#############################################
ifeq (1, 0)
LOCAL_PATH := $(CUR_PATH)
RIL_PATH := $(MTK_PATH_SOURCE)/hardware/ril/utilrilmtk
RIL_H_PATH := hardware/ril/include/telephony
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	extsimap_jni.cpp \
	BtSimapAfAdapter.cpp \

#
#	BtSimapRILAdapter.cpp \
#

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include \
	$(JNI_H_INCLUDE) \
	$(RIL_PATH) \
	$(RIL_H_PATH) \

ifeq ($(GEMINI),yes)	
  LOCAL_CFLAGS += -DMTK_DUAL_SIM_SUPPORT
endif

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libextsimap \
	libutilrilmtk \


LOCAL_MODULE := libextsimap_jni

LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)
endif
#############################################
# SIMAP JNI END
#############################################

#############################################
# Build JNI - DUN
#############################################
LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        extdun_jni.cpp \

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \
        system/core/libcutils \

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libmtkbtextspp \
        

LOCAL_MODULE := libextdun_jni

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#############################################
# DUN JNI END
#############################################

#############################################
# Build JNI - MAP
#############################################
LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        extmap_jni.cpp \

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(JNI_H_INCLUDE) \
        system/core/libcutils \

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += \
        libcutils \
        libextmap \
        

LOCAL_MODULE := libextmap_jni

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
#############################################
# MAP JNI END
#############################################

#############################################
# Build JNI - Advanced Service
#############################################
LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	extadvanced_jni.cpp \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include \
	$(JNI_H_INCLUDE) \
    system/core/libcutils \
    
LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += \
	libcutils \


LOCAL_MODULE := libextadvanced_jni

LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)

#############################################
# Advanced JNI END
#############################################

#############################################
# Build JNI - SYS
#############################################
LOCAL_PATH := $(CUR_PATH)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := extsys_jni.c

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/sys/include \
	$(JNI_H_INCLUDE) \

LOCAL_CFLAGS += -DPLATFORM_ANDROID

include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libextsys \

LOCAL_MODULE := libextsys_jni
LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

#############################################
# SYS JNI END
#############################################

#############################################
# End of file
#############################################
