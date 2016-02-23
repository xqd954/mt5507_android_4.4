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

MTK_BT_SUPPORT := yes
BT_TRANSPORT_DRV := yes

BT_PATH := external/bluetooth/mtk/blueangel

ifeq ($(MTK_BT_SUPPORT),yes)

MTK_BT_AUTOTEST := no
###############################
# build BT stack
###############################
LOCAL_PATH := $(call my-dir)
CUR_PATH := $(LOCAL_PATH)

have_stack_src := $(wildcard $(LOCAL_PATH)/btcore/btstack/Android.mk)
have_stack_lib := $(wildcard $(LOCAL_PATH)/libmtkbtstack.a)

#include $(have_stack_src)
#include $(LOCAL_PATH)/btcore/Android.mk

#$(error "have_stack_src=$(have_stack_src)")

ifeq ($(strip $(have_stack_src)),)
$(info "src not found")
else
include $(have_stack_src)
endif
###############################
# build BT task
###############################
#LOCAL_PATH := $(call my-dir)
LOCAL_PATH = $(CUR_PATH)
MY_LOCAL_PATH := $(LOCAL_PATH)

#
# libkal
#
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	btcore/btprofiles/a2dp/a2dp.c \
	btcore/btprofiles/a2dp/a2dpevent.c \
	btcore/btprofiles/a2dp/a2dputil.c \
	btcore/btprofiles/avrcp/avrcp.c \
	btcore/btprofiles/avrcp/avrcpevent.c \
	btcore/btprofiles/avrcp/avrcpsdp.c \
	btcore/btprofiles/avrcp/avrcputil.c \
	btcore/btprofiles/bip/bip.c \
	btcore/btprofiles/bip/bip_sm.c \
	btcore/btprofiles/bip/bipobs.c \
	btcore/btprofiles/bip/bipparse.c \
	btcore/btprofiles/bip/biputil.c \
	btcore/btprofiles/bpp/bpp.c \
	btcore/btprofiles/bpp/bpp_data.c \
	btcore/btprofiles/bpp/bpp_text.c \
	btcore/btprofiles/bpp/bpp_util.c \
	btcore/btprofiles/hid/hid.c \
	btcore/btprofiles/hid/hid_ctrl.c \
	btcore/btprofiles/hid/hid_intr.c \
	btcore/btprofiles/hid/hid_sdp.c \
	btcore/btprofiles/hid/hid_sm.c \
	btcore/btprofiles/hid/hid_util.c \
	btcore/btprofiles/hid/hidalloc.c \
	btcore/btprofiles/hid/hid_parser.c \
	btcore/btprofiles/hid/hid_gatt.c \
	btcore/btprofiles/hid/hid_blescan.c \
	btcore/btprofiles/ftp/ftp.c \
	btcore/btprofiles/ftp/ftp_sm.c \
	btcore/btprofiles/ftp/ftp_event.c \
	btcore/btprofiles/ftp/ftpobs.c \
	btcore/btprofiles/ftp/ftputil.c \
	btcore/btprofiles/add-ins/at/at_dun.c \
	btcore/btprofiles/add-ins/at/at_hf.c \
	btcore/btprofiles/add-ins/at/at_hs.c \
	btcore/btprofiles/add-ins/at/at_pb.c \
	btcore/btprofiles/add-ins/at/at_sms.c \
	btcore/btprofiles/add-ins/at/atp.c \
	btcore/btprofiles/goep/goep.c \
	btcore/btprofiles/goep/goep_main.c \
	btcore/btprofiles/hfg/hfg.c \
	btcore/btprofiles/hfg/hfg_at.c \
	btcore/btprofiles/hfg/hfg_sdp.c \
	btcore/btprofiles/hfg/hfg_seq.c \
	btcore/btprofiles/hfg/hfg_sm.c \
	btcore/btprofiles/hfg/hfg_util.c \
	btcore/btprofiles/hfg/hfgalloc.c \
	btcore/btprofiles/hfg/hfg_audio.cpp \
	btcore/btprofiles/obex/obclient.c \
	btcore/btprofiles/obex/obdebug.c \
	btcore/btprofiles/obex/obex_utl.c \
	btcore/btprofiles/obex/obexauth.c \
	btcore/btprofiles/obex/obheader.c \
	btcore/btprofiles/obex/obparse.c \
	btcore/btprofiles/obex/observer.c \
	btcore/btprofiles/obex/obstack.c \
	btcore/btprofiles/obex/obxalloc.c \
	btcore/btprofiles/obex/obxbtstk.c \
	btcore/btprofiles/obex/obxirstk.c \
	btcore/btprofiles/obex/obxoemstk.c \
	btcore/btprofiles/obex/obxtcpstk.c \
	btcore/btprofiles/opp/opp.c \
	btcore/btprofiles/pbap/pbap.c \
	btcore/btprofiles/pbap/pbapobs.c \
	btcore/btprofiles/sdpdb/a2dpsdpdb.c \
	btcore/btprofiles/sdpdb/appsdpdb.c \
	btcore/btprofiles/sdpdb/avrcpsdpdb.c \
	btcore/btprofiles/sdpdb/hfagsdpdb.c \
	btcore/btprofiles/sdpdb/hidsdpdb.c \
	btcore/btprofiles/sdpdb/hsagsdpdb.c \
	btcore/btprofiles/sdpdb/obexsdpdb.c \
	btcore/btprofiles/sdpdb/sdpdb.c \
	btcore/btprofiles/sdpdb/sppsdpdb.c \
	btcore/btprofiles/sdpdb/prxsdpdb.c \
	btcore/btprofiles/sdpdb/pansdpdb.c \
	btcore/btprofiles/sdpdb/timesdpdb.c \
	btcore/btprofiles/sdpdb/attsdpdb.c \
	btcore/btprofiles/sdpdb/hdpsdpdb.c \
	btcore/btprofiles/attdb/attdbmain.c \
	btcore/btprofiles/attdb/gapattdb.c \
	btcore/btprofiles/attdb/prxattdb.c \
	btcore/btprofiles/attdb/timeattdb.c \
	btcore/btprofiles/attdb/htpattdb.c \
	btcore/btprofiles/attdb/fmpattdb.c \
	btcore/btprofiles/simap/testing/fake_sim.c \
	btcore/btprofiles/simap/testing/simaptester.c \
	btcore/btprofiles/simap/sim.c \
	btcore/btprofiles/simap/sim_server.c \
	btcore/btprofiles/simap/sim_sm.c \
	btcore/btprofiles/simap/sim_util.c \
	btcore/btprofiles/simap/simap_api.c \
	btcore/btprofiles/spp/bt_spp_os.c \
	btcore/btprofiles/spp/spp.c \
	btcore/btprofiles/spp/spp_api.c \
	btcore/btprofiles/map/map.c \
	btcore/btprofiles/map/map_event.c \
	btcore/btprofiles/map/map_obs.c \
	btcore/btprofiles/jsr82/jsr82_session.c \
	btcore/btprofiles/jsr82/jsr82_ringbuf.c \
	btcore/btprofiles/jsr82/jsr82_ashm.cpp \
	btcore/btprofiles/jsr82/jsr82_sdp.c \
	btcore/btprofiles/prx/prxr.c \
	btcore/btprofiles/prx/prxm.c \
	btcore/btprofiles/prx/prxadapter.c \
	btcore/btprofiles/pan/pan.c \
	btcore/btprofiles/pan/pan_sdp.c \
	btcore/btprofiles/pan/pan_sm.c \
	btcore/btprofiles/pan/pan_util.c \
	btcore/btprofiles/time/timec.c \
	btcore/btprofiles/time/times.c \
	btcore/btprofiles/time/time_gatt.c \
	btcore/btprofiles/htp/htp.c \
	btcore/btprofiles/htp/htp_gatt.c \
	btcore/btprofiles/htp/htp_sm.c \
	btcore/btprofiles/htp/htp_util.c \
	btcore/btprofiles/fmp/fmp.c \
	btcore/btprofiles/fmp/fmp_gatt.c \
	btcore/btprofiles/fmp/fmp_sm.c \
	btcore/btprofiles/fmp/fmp_util.c \
	btcore/btprofiles/bleservice/bleservice_db.c \
	btcore/btprofiles/bleservice/htsservice.c \
	btcore/btprofiles/bleservice/disservice.c \
	btcore/btprofiles/bleservice/ctsservice.c \
	btcore/btprofiles/bleservice/ndcsservice.c \
	btcore/btprofiles/bleservice/rtuservice.c \
	btcore/btprofiles/bleservice/iasservice.c \
	btcore/btprofiles/bleservice/tpsservice.c \
	btcore/btprofiles/bleservice/llsservice.c \
	btcore/btprofiles/hdp/hdp.c \
	btcore/btprofiles/hdp/hdp_event.c \
	btcore/btprofiles/hdp/hdp_util.c \
	btcore/btprofiles/hdp/hdp_sdp.c \
	btcore/btprofiles/bleutils/ble_utils.c \
	btcore/btprofiles/gatt/gattc.c \
	btcore/btprofiles/gatt/gattc_channel.c \
	btcore/btprofiles/gatt/gatts.c \
	btcore/btprofiles/gatt/gatts_svc.c \
	btcore/btprofiles/gatt/gatt_util.c \
	btcore/os/bt_osapi.c \
	btcore/os/bt_profile_init.c \
	btcore/utility/bt_ddb4w.c \
	btcore/utility/Bt_debug.c \
	btcore/utility/bt_utility.c \
	btadp_int/bt_adp_profile_init.c \
	btadp_int/common/ucs2/bt_ucs2.c \
	btadp_int/common/xmlp/xml_main.c \
	btadp_int/driver/uart/bt_uart_drv.c \
	btadp_int/driver/uart/bt_uart_log_drv.c \
	btadp_int/profiles/a2dp/bt_adp_a2dp.c \
	btadp_int/profiles/a2dp/bt_adp_a2dp_event.c \
	btadp_int/profiles/a2dp/bt_adp_a2dpfake.c \
	btadp_int/profiles/avrcp/bt_adp_avrcp.c \
	btadp_int/profiles/bip/bt_adp_bip.c \
	btadp_int/profiles/bpp/bt_adp_bpp.c \
	btadp_int/profiles/hid/bt_adp_hidd.c \
	btadp_int/profiles/hid/bt_adp_hidh.c \
	btadp_int/profiles/hid/bt_adp_uhid.c \
	btadp_int/profiles/ftp/bt_adp_ftp_ashm.cpp \
	btadp_int/profiles/ftp/bt_adp_ftp.c \
	btadp_int/profiles/gap/bt_adp_bm.c \
	btadp_int/profiles/gap/bt_adp_bm_event.c \
	btadp_int/profiles/hfg/bt_adp_hfg.c \
	btadp_int/profiles/hfg/bt_adp_hfg_event.c \
	btadp_int/profiles/opp/bt_adp_opp_ashm.cpp \
	btadp_int/profiles/opp/bt_adp_opp.c \
	btadp_int/profiles/simap/bt_adp_simap.c \
	btadp_int/profiles/simap/bt_adp_simap_event.c \
	btadp_int/profiles/pbap/bt_adp_pbap.c \
	btadp_int/profiles/spp/bt_adp_spp.c \
	btadp_int/profiles/spp/bt_adp_spp_event.c \
	btadp_int/profiles/spp/bt_adp_spp_ashm.cpp \
	btadp_int/profiles/map/bt_adp_map.c \
	btadp_int/profiles/jsr82/bt_adp_jsr82.c \
	btadp_int/profiles/jsr82/bt_adp_jsr82_session.c \
	btadp_int/profiles/jsr82/bt_adp_jsr82_event.c \
	btadp_int/profiles/prx/bt_adp_prx.c \
	btadp_int/profiles/pan/bt_adp_pan.c \
	btadp_int/profiles/pan/bt_adp_pan_ip.c \
	btadp_int/profiles/time/bt_adp_time.c \
	btadp_int/profiles/time/bt_adp_times.c \
	btadp_int/profiles/time/bt_adp_timec.c \
	btadp_int/profiles/htp/bt_adp_htpc.c \
	btadp_int/profiles/htp/bt_adp_htpt.c \
	btadp_int/profiles/htp/bt_adp_htp_common.c \
	btadp_int/profiles/fmp/bt_adp_fmp.c \
	btadp_int/profiles/hdp/bt_adp_hdp.c \
	btadp_int/profiles/hdp/bt_adp_hdp_channel.c \
	btadp_int/profiles/hdp/bt_adp_hdp_util.c \
	btadp_int/profiles/gatt/bt_adp_gattc.c \
	btadp_int/profiles/gatt/bt_adp_gatts.c \
	btadp_int/sys/bt_adp_ashm_impl.cpp \
	btadp_int/sys/bt_adp_ashm.cpp \
	btadp_int/sys/bt_adp_em.c \
	btadp_int/sys/bt_adp_fs.c \
	btadp_int/sys/bt_adp_fs_ucs2.c \
	btadp_int/sys/bt_adp_log.c \
	btadp_int/sys/bt_adp_main.c \
	btadp_int/sys/bt_adp_utility.c \
	btadp_int/sys/bt_adp_msg.c \
	btadp_int/sys/bt_adp_sys_event.c \
	btadp_int/sys/bt_adp_system.c \
	btadp_int/sys/bt_osal.c \
	btadp_int/sys/bt_osal_string.c \
	btadp_int/sys/bt_osal_vcard.c \
	btadp_int/sys/bt_osal_qp.c \
	btadp_int/sys/bt_osal_charset.c \
	btadp_int/sys/bt_adp_mem.c \
	btadp_int/sys/bt_adp_a2mp.c \
	btadp_int/sys/ring.c \
	linuxsystem/src/interlayer.c \
	linuxsystem/src/btmainthread.c \
	btadp_int/sys/bt_adp_log_msg.c \
	linuxsystem/src/cmdqueue.c \
	linuxsystem/src/bttstthread.c \
	btadp_int/common/tstrb/tst_ringbuffer.c \
	linuxsystem/src/eint.c \
	linuxsystem/src/btnet.c \
	bwcs/bwcs.c \
	btadp_int/sys/bt_adp_debug.c \
	btadp_int/test/sdap_tester/bt_adp_sdap_tester.c \
	bt_cust/bt_adp_cust.c \
	
ifeq ($(MTK_BT_AUTOTEST), yes)
LOCAL_SRC_FILES += \
	autotest/auto_test.c \
	autotest/gap/auto_gap_handler.c \
	autotest/hci/auto_hci_handler.c \
	autotest/local/auto_local_handler.c \
	autotest/gatt/auto_gatt_handler.c \
	autotest/util/auto_util.c
endif

LOCAL_C_INCLUDES := \
	$(MY_LOCAL_PATH)/btcore/inc \
	$(MY_LOCAL_PATH)/btcore/inc/sys \
	$(MY_LOCAL_PATH)/btcore/btprofiles/include \
	$(MY_LOCAL_PATH)/btadp_int/driver/include \
	$(MY_LOCAL_PATH)/btadp_int/driver/uart/platform \
	$(MY_LOCAL_PATH)/btadp_int/include \
	$(MY_LOCAL_PATH)/dummyinc \
	$(MY_LOCAL_PATH)/linuxsystem/include \
	$(MY_LOCAL_PATH)/btadp_int/common/sbc \
	$(MY_LOCAL_PATH)/_bt_scripts/include \
	$(MY_LOCAL_PATH)/bwcs/inc \
	$(MY_LOCAL_PATH)/bt_cust/inc \
	$(MY_LOCAL_PATH)/session/include \
       $(MTK_PATH_CUSTOM)/hal/bluetooth
	
ifeq ($(MTK_BT_AUTOTEST), yes)
LOCAL_C_INCLUDES += \
	$(MY_LOCAL_PATH)/autotest/inc
endif
include $(MY_LOCAL_PATH)/mtkbt.inc

ifneq ($(BT_TRANSPORT_DRV), yes)
LOCAL_SRC_FILES += \
	btadp_int/driver/uart/platform/linux_uart.c
endif

ifeq ($(BT_TRANSPORT_DRV), yes)
LOCAL_SHARED_LIBRARIES += libbluetoothdrv
endif

LOCAL_SHARED_LIBRARIES += libcutils libutils libbinder libhardware_legacy libdl libbtsession libmedia
LOCAL_STATIC_LIBRARIES += libmtkbtstack

#ifeq ($(MTK_BT_30_HS_SUPPORT),yes)
#LOCAL_SHARED_LIBRARIES += libpalsecurity libpalwlan_mtk  libnetutils
#LOCAL_STATIC_LIBRARIES += pal
#endif

LOCAL_SHARED_LIBRARIES += libbtcust

LOCAL_MODULE := mtkbt
LOCAL_PRELINK_MODULE:= false
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_EXECUTABLE)

###############################
# build external libraries
###############################
include $(CUR_PATH)/audio_a2dp_hw/Android.mk
include $(CUR_PATH)/btadp_ext/Android.mk
#include $(CUR_PATH)/driver/Android.mk
ifeq ($(MTK_WLANBT_SINGLEANT), yes)
include $(CUR_PATH)/bwcs/Android.mk
endif
include $(CUR_PATH)/session/Android.mk
include $(CUR_PATH)/bt_cust/Android.mk
include $(CUR_PATH)/tools/Android.mk
include $(CUR_PATH)/btadp_jni/Android.mk
include $(MY_LOCAL_PATH)/test/ut_simulator/Android.mk
include $(MY_LOCAL_PATH)/btadp_jni/ut/Android.mk

# include $(MY_LOCAL_PATH)/test/sdap_tester/Android.mk

endif
