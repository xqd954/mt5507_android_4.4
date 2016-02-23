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

###############################
# build xfile lib
###############################
LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH := $(LOCAL_PATH)
include $(CLEAR_VARS)
ifeq (1,0)
have_xfile_src := $(wildcard $(LOCAL_PATH)/xfile/Android.mk)
have_xfile_lib := $(wildcard $(LOCAL_PATH)/libmtkbtstack2.a)

## include $(have_xfile_src)
$(warning ${have_xfile_src})

ifeq ($(strip $(have_xfile_src)),)
ifeq ($(strip $(have_xfile_lib)),)
$(error "$(LOCAL_PATH)/libmtkbtstack2.a not found")
else
LOCAL_PREBUILT_LIBS := \
		libmtkbtstack2.a
include $(BUILD_MULTI_PREBUILT)
endif
else
include $(have_xfile_src)
endif

endif
########################################
# build bt stack
########################################
LOCAL_PATH := $(MY_LOCAL_PATH)
#BT_PATH := external/bluetooth/mtk/blueangel
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	hcitrans/bt_parseopts.c \
	hcitrans/transprt.c \
	hcitrans/modinit/radiomod.c \
	hcitrans/modinit/test_bt_patch.c \
	hcitrans/uart/uarttran.c \
	hcitrans/usb/usbtrans.c \
	stack/Bt_Virtual_Sniffer.c \
	stack/bt_wifi_coexist_setting.c \
	stack/btalloc.c \
	stack/btrom.c \
	stack/btutils.c \
	stack/debug.c \
	stack/x_file.c \
	stack/eventmgr.c \
	stack/radiomgr.c \
	stack/rxbuff.c \
	stack/utils.c \
	stack/avctp/avc.c \
	stack/avctp/avctp.c \
	stack/avctp/avctpcon.c \
	stack/avctp/avctpmsg.c \
	stack/avdtp/avalloc.c \
	stack/avdtp/avdtp.c \
	stack/avdtp/avdtpcon.c \
	stack/avdtp/avsigmgr.c \
	stack/bm/BtAccCtrl.c \
	stack/bm/Btbm.c \
	stack/bm/BtbmBlocklist.c \
	stack/bm/BtbmCon.c \
	stack/bm/BtbmDev.c \
	stack/bm/BtbmProfilesCtrl.c \
	stack/bm/BtbmSdp.c \
	stack/bm/BtbmSec.c \
	stack/bm/BtbmSetting.c \
	stack/bm/BtbmGatt.c \
	stack/bm/BtbmLEAdv.c \
    stack/bnep/bnep.c \
    stack/bnep/bnep_sm.c \
    stack/bnep/bneputil.c \
    stack/mcap/mcap.c \
    stack/mcap/mcap_sm.c \
    stack/mcap/mcap_util.c \
	stack/common/avtp.c \
	stack/common/conmgr.c \
	stack/common/bt_wmt.c \
	stack/hci/hci.c \
	stack/hci/hci_evnt.c \
	stack/hci/hci_proc.c \
	stack/hci/hci_util.c \
	stack/hci/hci_meta.c \
	stack/hci/hci_amp.c \
	stack/l2cap/l2cap.c \
	stack/l2cap/l2cap_if.c \
	stack/l2cap/l2cap_sm.c \
	stack/l2cap/l2cap_utl.c \
	stack/l2cap/l2cap_amp.c \
	stack/l2cap/l2cap_ampchan.c \
	stack/l2cap/l2cap_enhanced.c \
	stack/me/me.c \
	stack/me/meacc.c \
	stack/me/mecon.c \
	stack/me/medev.c \
	stack/me/Meeir.c \
	stack/me/meif.c \
	stack/me/MeRadio.c \
	stack/me/mesco.c \
	stack/me/mescodat.c \
	stack/me/mescoif.c \
	stack/me/mesec.c \
	stack/me/meamp.c \
	stack/me/mele.c \
	stack/rfcomm/rfc_ctl.c \
	stack/rfcomm/rfc_fcs.c \
	stack/rfcomm/rfc_sm.c \
	stack/rfcomm/rfc_util.c \
	stack/rfcomm/rfcomm.c \
	stack/sdap/sdap.c \
	stack/sdap/sdap_devdb.c \
	stack/sdp/sdp.c \
	stack/sdp/sdpclient.c \
	stack/sdp/sdpserv.c \
	stack/sdp/sdputil.c \
	stack/a2mp/a2mp_ctl.c \
	stack/a2mp/a2mp_sec.c \
	stack/a2mp/a2mp_sm.c \
	stack/a2mp/a2mp_util.c \
	stack/sha256/mtkbt_hmac_sha2.c \
	stack/att/att.c \
	stack/att/att_utl.c \
	stack/att/att_state.c \
	stack/gatt/gatt.c \
	stack/gatt/gatt_hdl.c \
	stack/gatt/gatt_state.c \
	stack/gatt/gatt_utl.c \
	stack/sm/sm.c \
	stack/sm/smp.c \
        stack/gavdp/gavdp.c 

LOCAL_C_INCLUDES := \
	$(BT_PATH)/btcore/btstack/inc \
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
	$(BT_PATH)/bwcs/inc \
	
include $(BT_PATH)/mtkbt.inc

LOCAL_SHARED_LIBRARIES += libcutils
#LOCAL_WHOLE_STATIC_LIBRARIES += libmtkbtstack2

#LOCAL_PREBUILT_OBJ_FILES += 

LOCAL_MODULE := libmtkbtstack
LOCAL_PRELINK_MODULE:= false
include $(BUILD_STATIC_LIBRARY)

#ROOT_PATH := $(shell pwd)
#$(info "ROOT_PATH=$(ROOT_PATH))

#$(shell test -e ./$(TARGET_OUT_INTERMEDIATES)/STATIC_LIBRARIES/libmtkbtstack_intermediates/libmtkbtstack.a && cp ./$(TARGET_OUT_INTERMEDIATES)/STATIC_LIBRARIES/libmtkbtstack_intermediates/libmtkbtstack.a $(LOCAL_PATH)/../libmtkbtstack.a)
