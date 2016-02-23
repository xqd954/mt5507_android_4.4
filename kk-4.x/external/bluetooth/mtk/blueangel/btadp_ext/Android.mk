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

include $(LOCAL_PATH)/sys/Android.mk

#BT_PATH := external/bluetooth/mtk/blueangel

################################################
# build Bluetooth.default.so
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)
	
# HAL layer
LOCAL_SRC_FILES += \
	  ../btadp_if/src/bluetooth.c
	
# Utils
LOCAL_SRC_FILES += \
    ../btutils/bt_utils.c 
		
# BTIF
LOCAL_SRC_FILES += \
    ../btadp_if/src/btif_core.c \
    ../btadp_if/src/btif_av.c \
    ../btadp_if/src/btif_rc.c \
    ../btadp_if/src/btif_hh.c \
    ../btadp_if/src/btif_hf.c \
    ../btadp_if/src/btif_pan.c \
    ../btadp_if/src/btif_sock.c \
    ../btadp_if/src/btif_hl.c \
    ../btadp_if/src/btif_gatt.c \
    ../btadp_if/src/btif_gatt_client.c \
    ../btadp_if/src/btif_gatt_server.c
    
# platform specific
LOCAL_SRC_FILES += \
	  common/bt_ext_timer.c \
	  common/bt_ext_apis.c \
	  profiles/gap/bt_gap_api.c \
	  profiles/gap/bt_gap_hdl.c \
	  profiles/gap/bt_profile_manager.c \
	  profiles/hid/bt_hid_api.c \
	  profiles/hid/bt_hid_hdl.c \
	  profiles/avrcp/bt_avrcp_key.c \
	  profiles/avrcp/bt_avrcp_api.c \
	  profiles/avrcp/bt_avrcp_hdl.c \
	  profiles/a2dp/bt_a2dp_api.c \
	  profiles/a2dp/bt_a2dp_hdl.c \
	  profiles/hfg/bt_hfg_api.c \
		profiles/pan/bt_pan_api.c \
		profiles/pan/bt_pan_hdl.c \
	  profiles/jsr82/bt_jsr82_jbt_session.c \
	  profiles/jsr82/bt_jsr82_l2rf_api.c \
	  profiles/jsr82/bt_jsr82_l2rf_hdl.c \
	  profiles/jsr82/bt_jsr82_sdp.c \
	  profiles/hdp/bt_hdp_api.c \
	  profiles/hdp/bt_hdp_common.c \
	  profiles/hdp/bt_hdp_hdl.c \

LOCAL_C_INCLUDES+= \
  $(BT_PATH)/btadp_ext/include \
  $(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btutils/include \
	$(BT_PATH)/btadp_if/include \
	$(BT_PATH)/btcore/inc \
	$(BT_PATH)/btadp_int/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/include/common \
	$(BT_PATH)/btadp_int/include \
	$(BT_PATH)/session/include \
	$(BT_PATH)/bt_cust/inc \
	$(MTK_ROOT)/frameworks/base/custom/inc

include $(BT_PATH)/mtkbt.inc

LOCAL_CFLAGS += -DBUILDCFG

ifeq ($(strip $(MTK_BSP_PACKAGE)), no)
LOCAL_STATIC_LIBRARIES := libcustom_prop libmtkbtextsys
else
LOCAL_STATIC_LIBRARIES := libmtkbtextsys
endif

LOCAL_SHARED_LIBRARIES += \
    libcutils \
    libbtsession \
    libbtcust \
    libbtcusttable
    

LOCAL_MODULE := bluetooth.default
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib/hw
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

include $(BUILD_SHARED_LIBRARY)

################################################
# build SYS external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils libutils libbinder

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
	sys/ext_interlayer.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextsys

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build FTP external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libc \
	libcutils \
	libutils \
	libbinder

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
	common/bt_ext_utility.c \
	profiles/goep/bt_goep_fs.c \
	profiles/goep/bt_goep_porting.c \
	profiles/goep/bt_goep_util.c \
	profiles/ftp/bt_ftp_ashm.cpp \
	profiles/ftp/bt_ftp_porting.c \
	profiles/ftp/bt_ftps_api.c \
	profiles/ftp/bt_ftps_hdl.c \
	profiles/ftp/bt_ftps_genxml.c \
	profiles/ftp/bt_ftpc_api.c \
	profiles/ftp/bt_ftpc_hdl.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include
	
include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextftp

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build BIP external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
        profiles/bip/bt_bip_hdl.c \
        profiles/bip/bt_bipi_api.c \
        profiles/bip/bt_bipi_hdl.c \
        profiles/bip/bt_bipr_api.c \
        profiles/bip/bt_bipr_hdl.c \
        profiles/bip/bt_bip_porting.c \
	#profiles/bip/bt_bip_int.c 
        #profiles/bip/bt_bipi_comm.c 
        #profiles/bip/bt_bipr_comm.c

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(BT_PATH)/btcore/inc
	
include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextbip

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
################################################
# build BPP external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
        profiles/bpp/bt_bpp_porting.c \
        profiles/bpp/bt_bpp_hdl.c 

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(BT_PATH)/btcore/inc

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextbpp

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build SIMAP external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
        profiles/simap/bt_simap_api.c \
        profiles/simap/bt_simap_hdl.c \
        profiles/simap/bt_simap_porting.c \

LOCAL_C_INCLUDES := \
        $(BT_PATH)/btadp_ext/include \
        $(BT_PATH)/btadp_ext/sys/include \
        $(BT_PATH)/btadp_int/include \
        $(BT_PATH)/btcore/inc

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextsimap

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build MAP external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils 

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
	common/bt_ext_utility.c \
	profiles/map/bt_map_api.c \
	profiles/map/bt_map_hdl.c \
	profiles/map/bt_map_util.c \
	profiles/map/bt_map_obj.c \

#LOCAL_CFLAGS += -D__DUN_FOR_GSM__

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
  $(BT_PATH)/btadp_ext/sys/include \
  $(BT_PATH)/btadp_int/include \
  $(BT_PATH)/include/common

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextmap

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build SPP external ADP start
################################################
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder

LOCAL_STATIC_LIBRARIES := libmtkbtextsys

LOCAL_SRC_FILES := \
	profiles/spp/bt_spp_api.c \
	profiles/spp/bt_spp_hdl.c \
	profiles/spp/bt_spp_ashm.cpp \
	profiles/spp/bt_dun_at.c

LOCAL_CFLAGS += -D__DUN_FOR_GSM__

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
  $(BT_PATH)/btadp_ext/sys/include \
  $(BT_PATH)/btadp_int/include \
  $(BT_PATH)/include/common

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libmtkbtextspp

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

include $(CURR_PATH)/profiles/spp/libpppbtdun/Android.mk

################################################
# build BT test mode lib start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils libutils libextsys
ifeq ($(MTK_TC1_FEATURE),yes)
#LOCAL_SHARED_LIBRARIES += libtc1part
endif

LOCAL_SRC_FILES := \
	sys/ext_testmode.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include \
	$(BT_PATH)/btadp_ext/sys/include \
	$(BT_PATH)/btadp_int/include

ifeq ($(MTK_TC1_FEATURE),yes)
#LOCAL_C_INCLUDES += \
#	$(MTK_PATH_SOURCE)/external/tc1_interface
endif

include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libexttestmode

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# build EM external ADP start
################################################
LOCAL_PATH := $(CURR_PATH)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libcutils

LOCAL_SRC_FILES := \
	common/bt_ext_em.c \

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btadp_ext/include 
	
include $(BT_PATH)/mtkbt.inc

LOCAL_MODULE := libextem

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

################################################
# End of File
################################################
