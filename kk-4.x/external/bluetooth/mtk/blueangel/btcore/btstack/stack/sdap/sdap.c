/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * sdap.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions provide to each profile to get SDP Data base service.
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 04 25 2012 huirong.liao
 * [ALPS00270892] [BlueTooth][HFP]DUT Connected with PC "phone audio" will disconnected itself after a while "This issue also exist on MT6577"
 * .
 *
 * 10 01 2011 dlight.ting
 * [ALPS00076918] [FDD2_MPV3 Regression Test][CTSVerifier] Bluetooth Test - Fail
 * .
 *
 * 09 30 2011 dlight.ting
 * [ALPS00076918] [FDD2_MPV3 Regression Test][CTSVerifier] Bluetooth Test - Fail
 * .
 *
 * 09 30 2011 dlight.ting
 * [ALPS00076918] [FDD2_MPV3 Regression Test][CTSVerifier] Bluetooth Test - Fail
 * .
 *
 * 09 30 2011 dlight.ting
 * [ALPS00076918] [FDD2_MPV3 Regression Test][CTSVerifier] Bluetooth Test - Fail
 * .
 *
 * 09 30 2011 dlight.ting
 * [ALPS00076918] [FDD2_MPV3 Regression Test][CTSVerifier] Bluetooth Test - Fail
 * .
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 * 03 17 2011 dlight.ting
 * [ALPS00034625] Bluetooth BNEP and PAN profile
 * .
 *
 * Mar 12 2009 mtk01411
 * [MAUI_01645202] [Bluetooth] Btadp folder restruct
 * 
 *
 * Feb 18 2009 mtk01239
 * [MAUI_01631727] [Bluetooth] Checkin HCI Retransmission design
 * 
 *
 * Dec 2 2008 mtk02126
 * [MAUI_01288486] chech in SPP/DUN new architecture
 * 
 *
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Aug 12 2008 mtk01411
 * [MAUI_00820444] [Bluetooth] SyncML feature supported in BT
 * 
 *
 * Aug 5 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 10 2008 mtk01239
 * [MAUI_01177672] MTK:Bluetooth_can't bond
 * 
 *
 * Jun 26 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * Apr 6 2008 mtk01411
 * [MAUI_00289416] [BPP]_handset always show "connecting" screen
 * 
 *
 * Feb 28 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Feb 25 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Feb 19 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Feb 19 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Oct 8 2007 mtk01239
 * [MAUI_00556508] [BT]Shrinking BT code size
 * 
 *
 * Sep 1 2007 mtk01239
 * [MAUI_00541015] [Bluetooth] Fix SDAP complier error
 * 
 *
 * Aug 20 2007 mtk01239
 * [MAUI_00535716] [Bluetooth] fix complier warming.
 * 
 *
 * Jul 30 2007 mtk01411
 * [MAUI_00527847] [Bluetooth] Modify discFlag to FALSE when doing sdp query if ACL link is already exi
 * 
 *
 * Jul 30 2007 mtk01411
 * [MAUI_00527346] [Bluetooth] Sdap search single attribute remove sdapMeHandler when receiving rsp
 * 
 *
 * Jul 16 2007 mtk01411
 * [MAUI_00416972] [Bluetooth] SPP with SCO functionality
 * 
 *
 * Jul 15 2007 mtk01411
 * [MAUI_00416972] [Bluetooth] SPP with SCO functionality
 * 
 *
 * Jun 22 2007 mtk01411
 * [MAUI_00407614] [Bluetooth UPF27] A2DP PTS testing failed on PTS version 2.2.0.0 (PTS only responds
 * 
 *
 * Jun 21 2007 mtk01411
 * [MAUI_00407614] [Bluetooth UPF27] A2DP PTS testing failed on PTS version 2.2.0.0 (PTS only responds
 * 
 *
 * May 7 2007 mtk01239
 * [MAUI_00389758] [Bluetooth] Checkin JSR82 code of BTStack
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Apr 16 2007 mtk01239
 * [MAUI_00377236] [Bluetooth] Security level 2
 * 
 *
 * Feb 16 2007 mtk01239
 * [MAUI_00367691] [BT][HID] HID new feature check-in
 * 
 *
 * Jan 11 2007 mtk01239
 * [MAUI_00358010] [1]Fatal Error(4):Data Abort lr=0*55768e-BT
 * 
 *
 * Jan 5 2007 mtk01239
 * [MAUI_00356742] [Bluetooth] add SDP record for BIP
 * 
 *
 * Jan 3 2007 mtk01239
 * [MAUI_00355740] [1] Assert Fail: SDAP(sdap_current_multi_req_index) <= SDAP_MAX_MULTIPLE_REQ sdap.c
 * 
 *
 * Dec 31 2006 mtk01239
 * [MAUI_00355146] Video Player-Select the video and select "Play", it will always display  the BT's pr
 * 
 *
 * Dec 27 2006 mtk01239
 * [MAUI_00354182] [1] Assert Fail: SDAP(sdap_current_multi_req_index) <= SDAP_MAX_MULTIPLE_REQ sdap.c
 * 
 *
 * Dec 14 2006 mtk01239
 * [MAUI_00350555] [1]Fatal Error:Data Abort lr=0*4ca1d2-BT
 * 
 *
 * Dec 8 2006 mtk01239
 * [MAUI_00349361] [1]Assert fail:attribute_num<=SDAP_MAX_ATTRIBUTE_NUM sdap.c 592-BT
 * 
 *
 * Dec 7 2006 mtk01239
 * [MAUI_00349361] [1]Assert fail:attribute_num<=SDAP_MAX_ATTRIBUTE_NUM sdap.c 592-BT
 * 
 *
 * Dec 7 2006 mtk01239
 * [MAUI_00349361] [1]Assert fail:attribute_num<=SDAP_MAX_ATTRIBUTE_NUM sdap.c 592-BT
 * 
 *
 * Dec 7 2006 mtk01239
 * [MAUI_00349361] [1]Assert fail:attribute_num<=SDAP_MAX_ATTRIBUTE_NUM sdap.c 592-BT
 * 
 *
 * Nov 27 2006 mtk01239
 * [MAUI_00346134] [1] Assert fail: 0 bt_adp_bm.c 1708 - BT
 * 
 *
 * Nov 27 2006 mtk01239
 * [MAUI_00346296] [Bluetooth] Sync BT Host protocol code.
 * 
 *
 * Nov 24 2006 mtk01239
 * [MAUI_00340384] Bluetooth_SE T630 cannot send VCalendar to Superman29 successfully
 * 
 *
 * Nov 20 2006 mtk01239
 * [MAUI_00344139] [1] Fatal Error (3): Prefetch Abort 1 - BT
 * 
 *
 * Nov 20 2006 mtk01239
 * [MAUI_00344165] Bluetooth-Press play key then the audio will not play
 * 
 *
 * Nov 14 2006 mtk01239
 * [MAUI_00226549] bt_the device service list is abnormal
 * 
 *
 * Oct 18 2006 mtk01239
 * [MAUI_00336057] Bluetooth_The handsfree profile device is HT820 on active device list after conn
 * 
 *
 * Oct 16 2006 mtk01239
 * [MAUI_00336126] [1] Assert fail: 0 BTMMICm.c 1188 - MMI
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334713] [Bluetooth]Remove Report to bt_trace
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334713] [Bluetooth]Remove Report to bt_trace
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334643] [Bluetooth] fix complier warning
 * 
 *
 * Sep 30 2006 mtk01239
 * [MAUI_00333506] [1]Fatal Error:Data Abort Ir=0*3db208-BT
 * 
 *
 * Aug 21 2006 mtk01239
 * [MAUI_00322918] [BT] Interaction between BT-earphone and earphone, Assert Fail: meif.c 1051 -BT occu
 * 
 *
 * Aug 7 2006 mtk01239
 * [MAUI_00215228] [BT] update Bluetooth code
 * 
 *
 * Aug 4 2006 mtk01239
 * [MAUI_00215228] [BT] update Bluetooth code
 * 
 *
 * Jul 16 2006 mtk01239
 * [MAUI_00210782] [Bluetooth][ESI]update Bluetooth codes
 * 
 *
 * Jun 5 2006 mtk01239
 * [MAUI_00199721] [Bluetooth][ESI] update bt code update
 * 
 *
 * Apr 3 2006 mtk00758
 * [MAUI_00184485] [New feature] Add task to simulate UART driver for unitest
 * 
 *
 * Mar 11 2006 MTK00303
 * [MAUI_00178684] [Bluetooth]ESI bluetooth protocol stack
 * add bluetooth files
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

#include "bt_common.h"
#include "bt_os_api.h"
#include "bluetooth_gap_struct.h"
#include "btbm_adp.h"
#include "l2cap.h"
#include "sdp.h"
#include "sdap.h"
#include "medev.h"
#include "bt_maui_btbm.h"
#include "sdpi.h"
#include "btalloc.h"
#include "bt_jsr82_setting.h"
#include "utils.h"
#include "sdap_devdb.h"

#if BT_JSR82_ENABLED
extern void BTBMAdpSearchAttributeRawResult(U16 len, U8 *data, btbm_bd_addr_t addr);
extern void BTBMAdpSearchAttributeRawComplete(U8 result, btbm_bd_addr_t addr);
#endif /* BT_JSR82_ENABLED */ 

extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);
extern void btbm_dummy_callback(BMCallbackParms *Parms);
extern void btbmSdpSearchAttributePendingInquiry(U8 enabled);

/***************
Internal Globals 
****************/
/* MTK SDAP Manager Context */
static BtHandler sdapMeHandler;
static sdp_context_struct SDAPController = {0};

#ifdef __SDAP_REQUEST_QUEUE__
static ListEntry sdapRequestFreeList;
/* Structure pool for SDAP requests */
static SdapQueuedRequest _sdapRequestStructPool[SDAP_MAX_MULTIPLE_REQ - 1];
#endif

/*********************
Macros With Parameters 
**********************/

#define SDAP(f)  (SDAPController.f)

/***************************
Internal function prototypes
****************************/

static BtStatus sdap_service_search_request_all(U16 uuid, SdpQueryMode mode);
static BtStatus sdap_service_search_request_all_2(SdpQueryMode mode);

#if BT_JSR82_ENABLED
static BtStatus sdap_service_search_attribute_raw_request(SdpQueryMode mode);
#endif 
static BtStatus sdap_service_search_attribute_request(U8 uuid_len, U8 *uuid_128, U16 uuid, U16 attribute_id, SdpQueryMode mode);

static void query_sdap_callback(const BtEvent *event);
static void query_sdap_multiple_callback(const BtEvent *event);
static void query_sdap_multiple_2_handle_callback(const BtEvent *event);
static void query_sdap_multiple_2_attribute_callback(const BtEvent *event);
static void query_sdap_service_search_all_callback(const BtEvent *Event);
static void query_sdap_service_search_all_2_callback(const BtEvent *Event);
static void query_sdap_search_attribute_callback(const BtEvent *Event);
static void sdapSendServiceSearchCompleted(U8 result, btbm_bd_addr_t addr, U8 no);
static void sdapSendServiceSearchCancelled(U8 result, btbm_bd_addr_t addr, U8 no);
static void sdapSendAttributeSearchResult(U16 len, U8 *data, btbm_bd_addr_t addr);
#if defined(BTMTK_ON_LINUX)
static void sdapSendAttributeSearchCompleted(U8 result, btbm_bd_addr_t addr, U16 len, U8 *data);
#else
static void sdapSendAttributeSearchCompleted(U8 result, btbm_bd_addr_t addr);
#endif

#if BT_JSR82_ENABLED
static void query_sdap_service_attribute_raw_callback(const BtEvent *Event);
#endif 
static U8 sdap_util_allocate_free_devices(BD_ADDR bdAddr);
static void sdap_util_setup_query_token(
                SdpQueryToken *cur_sqt,
                const U8 *parms,
                U16 plen,
                SdpQueryType type,
                BtCallBack callback,
                BtRemoteDevice *rm);

static BtStatus sdap_util_create_acl_link(BtDeviceContext *bdc);
static void sdap_restart_multiple_req(BtRemoteDevice *remDev);
static void sdap_mutiple_req_timeout(EvmTimer *Timer);
static void sdap_process_next_request(BtRemoteDevice * remDev);

#ifdef __SDAP_REQUEST_QUEUE__
static void sdapInitRequestFreeList();
static SdapQueuedRequest *sdapGetAvailableRequestStruct();
static void sdapReturnRequestStruct(SdapQueuedRequest *sdap_req);
static void sdapQueuedRequestTimeout(EvmTimer *timer);
static BtStatus sdapEnqueueRequestAll(sdap_service_search_all_struct search_pattern);
static BtStatus sdapEnqueueRequestAll2(const BD_ADDR bdAddr);
static BtStatus sdapEnqueueRequestMultipleAttribute(sdap_service_search_multi_attribute_struct search_pattern);
static BtStatus sdapEnqueueRequestMultipleAttribute2(sdap_service_search_multi_attribute_struct search_pattern);
static SdapQueuedRequest *sdapDequeueRequestFromHead();
static SdapQueuedRequest *sdapDequeueRequestSameAddr(BD_ADDR *bdaddr);
static void sdapRemoveRequestFromQueue(SdapQueuedRequest *req);
static void sdapClearRequestSameAddr(BD_ADDR *bdaddr);
#endif

static BtStatus sdap_search_pnp_info_request(SdpQueryMode mode); 

/* Add 2007-1219 */
void SDAP_ServiceSearchAttributeRaw(bt_bm_search_raw_req_struct *search_pattern);

/******************************
Definitions Of Exported Globals 
*******************************/
#ifdef __BT_A2DP_PROFILE__
extern U8 A2DP_GetSdpQueryCount(void);
#endif 
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);

/*-------------------------------------------------------------------------
 *
 * SDP RFCOMM GROUP query info
 */
U8 SdapRfcommGroupServiceSearchReq[] = 
{
    /* 
     * Data Element Sequence of Services 
     */
    DETD_SEQ + DESD_ADD_8BITS,  /* Header for attribute ID list */
    9,                          /* Size of the list */

    /* Serial Port */
    DETD_UUID + DESD_2BYTES,                    /* type & size index 0x19 */
    (U8) (((SC_SERIAL_PORT) & 0xff00) >> 8),    /* Bits[15:8] of UUID */
    (U8) ((SC_SERIAL_PORT) & 0x00ff),           /* Bits[7:0] of UUID */

    /* RFCOMM */
    DETD_UUID + DESD_2BYTES,    /* Header for RFCOMM UUID */
    0x00, 0x03,                 /* UUID for RFCOMM in Big Endian */

    /* L2CAP */
    DETD_UUID + DESD_2BYTES,    /* UUID header of size 2 bytes */
    0x01, 0x00,                 /* L2CAP UUID */

    /* 
     * Max number of service record handles to return 
     */
    0x00, 0x64,

    /* 
     * Data Element Sequence of Attribute List 
     */
    DETD_SEQ + DESD_ADD_8BITS,  /* Header for attribute ID list */
    3,                          /* Size of the list */
    DETD_UINT + DESD_2BYTES,    /* Header for Protocol descriptor list ID */
    0x00, 0x04                  /* Value of protocol descriptor list ID */
};

U8 SdapRfcommTesterServiceSearchReq[] = 
{
    /* 
     * Data Element Sequence of Services 
     */
    DETD_SEQ + DESD_ADD_8BITS,  /* Header for attribute ID list */
    3,                          /* Size of the list */

    /* Serial Port */
    DETD_UUID + DESD_2BYTES,                    /* type & size index 0x19 */
    (U8) (((SC_SERIAL_PORT) & 0xff00) >> 8),    /* Bits[15:8] of UUID */
    (U8) ((SC_SERIAL_PORT) & 0x00ff),           /* Bits[7:0] of UUID */

    /* 
     * Max number of service record handles to return 
     */
    0x00, 0x64,

    /* 
     * Data Element Sequence of Attribute List 
     */
    DETD_SEQ + DESD_ADD_8BITS,  /* Header for attribute ID list */
    3,                          /* Size of the list */
    DETD_UINT + DESD_2BYTES,    /* Header for Protocol descriptor list ID */
    0x00, 0x04                  /* Value of protocol descriptor list ID */
};

/*-------------------------------------------------------------------------
 *
 * SDP query info
 *
 * Service search attribute request.
 */
U8 SdapAvrcpGroupServiceSearchReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 4 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* DES, 6 bytes */
    /* The third UUID is AVCTP */
    SDP_UUID_16BIT(PROT_AVCTP), /* AVCTP UUID   */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x02, 0xA0, /* Max number of bytes for attribute is 672 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),                  /* DES, 3 bytes      */
    SDP_UINT_16BIT(AID_SERVICE_CLASS_ID_LIST)   /* Prot Desc List ID */
};

/*-------------------------------------------------------------------------
 *
 * SDP query info
 */
U8 SdapMultipleRfcommGroupServiceSearchAttribReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data Element Sequence, 9 bytes */

    SDP_UUID_16BIT(SC_HANDSFREE),   /* UUID for Hands-Free AG in Big Endian */
    SDP_UUID_16BIT(PROT_L2CAP),     /* L2CAP UUID in Big Endian */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* UUID for RFCOMM in Big Endian */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST)
};


U8 SdapMultipleAVRCPCTGroupServiceSearchAttribReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(6),  /* Data Element Sequence, 9 bytes */

    SDP_UUID_16BIT(SC_AV_REMOTE_CONTROL),   /* UUID for Hands-Free AG in Big Endian */
    SDP_UUID_16BIT(PROT_L2CAP),     /* L2CAP UUID in Big Endian */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST)
};



U8 SdapUUID128MultipleRfcommGroupServiceSearchAttribReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(23),  /* Data Element Sequence, 23 bytes */

    SDP_UUID_128BIT(SC_BT_BASE_128),   /* UUID-128 bits for SyncML BT Server in Big Endian */
    SDP_UUID_16BIT(PROT_L2CAP),     /* L2CAP UUID in Big Endian */
    SDP_UUID_16BIT(PROT_RFCOMM),    /* UUID for RFCOMM in Big Endian */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST)
};


/*-------------------------------------------------------------------------
 *
 * SDP query info
 */
U8 SdapMultipleA2dpGroupServiceSearchReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */

    SDP_UUID_16BIT(SC_AUDIO_DISTRIBUTION),  /* L2CAP UUID                     */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),                  /* Data Element Sequence, 12 bytes */
    SDP_UINT_16BIT(AID_BT_PROFILE_DESC_LIST)    /* Protocol descriptor list ID */
};

U8 SdapMultipleHidGroupServiceSearchReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */

    SDP_UUID_16BIT(SC_HUMAN_INTERFACE_DEVICE),  /* L2CAP UUID                     */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),                  /* Data Element Sequence, 12 bytes */
    SDP_UINT_16BIT(AID_BT_PROFILE_DESC_LIST)    /* Protocol descriptor list ID */
};

U8 SdapSearchHandleReq[] = {
	SDP_ATTRIB_HEADER_8BIT(3),
	SDP_UUID_16BIT(0x0000),
	0x00, 0x01
};

U8 SdapSearchAttributeReq[] = {
	/* Service Record Handle */
	0x00, 0x00, 0x00, 0x00,

	/* Maximum byte count is 100 */
	0x00, 0x64,

	/* Attribute ID list */
	SDP_ATTRIB_HEADER_8BIT(3),
	SDP_UINT_16BIT(0x0000),
};

static U8 SdapServiceSearchArrributePattern[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */

    SDP_UUID_16BIT(SC_HEADSET_AUDIO_GATEWAY),   /* UUID for Headset          */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */
    SDP_UINT_16BIT(0x0004),     /* Value of protocol descriptor list ID */
};

static U8 SdapServiceSearchArrributePattern128[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(17),  /* Data Element Sequence, 3 bytes */

    SDP_UUID_128BIT(SC_BT_BASE_128),   /* UUID for BT base UUID */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */
    SDP_UINT_16BIT(0x0004),     /* Value of protocol descriptor list ID */
};

#ifdef __SDAP_SERVICE_SEARCH_ALL_2__
static U8 SdapServiceSearchAll2Pattern[] = 
{
	/*
	 * First parameter is the search pattern in data element format. It
	 * is a list of 3 UUIDs. 
	 */
	SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */

	SDP_UUID_16BIT(PROT_L2CAP),   /* UUID for Headset          */

	/*
	 * The second parameter is the maximum number of bytes that can be
	 * be received for the attribute list
	 */
	0xFF, 0xFF, /* Max number of bytes for attribute is 255 */

	/*
	 * The third parameter is the actual attribute ID list which is a data
	 * element sequence. We only want one attribute which is the protocol
	 * descriptor
	 */
	SDP_ATTRIB_HEADER_8BIT(5),  /* Data Element Sequence, 3 bytes */
	SDP_UINT_32BIT(0x0000FFFF),     /* Value of protocol descriptor list ID */
};
#endif

#if BT_JSR82_ENABLED
U8 SdapServiceSearchRawPattern[200] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(30), /* Data Element Sequence, 3 bytes; size:30bytes=5(0x1A + 4bytes UUID)*6UUIDs */

    SDP_UUID_32BIT(0x0000), /* UUID 1   */
    SDP_UUID_32BIT(0x0000), /* UUID 2   */
    SDP_UUID_32BIT(0x0000), /* UUID 3   */
    SDP_UUID_32BIT(0x0000), /* UUID 4   */
    SDP_UUID_32BIT(0x0000), /* UUID 5   */
    SDP_UUID_32BIT(0x0000), /* UUID 6   */
    0x00, 0x64,             /* Max number of bytes for attribute is 100 */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. We only want one attribute which is the protocol
     * descriptor
     */
    SDP_ATTRIB_HEADER_8BIT(18), /* Data Element Sequence, 3 bytes; size:18bytes=3(0x09 + 2bytes UUID)*6UUIDs */
    SDP_UINT_16BIT(0x0000),     /* Value of attribute list ID 1 */
    SDP_UINT_16BIT(0x0000),     /* Value of attribute list ID 2 */
    SDP_UINT_16BIT(0x0000),     /* Value of attribute list ID 3 */
    SDP_UINT_16BIT(0x0000),     /* Value of attribute list ID 4 */
    SDP_UINT_16BIT(0x0000),     /* Value of attribute list ID 5 */
    SDP_UINT_16BIT(0x0000),     /* Value of attribute list ID 6 */

};
#endif /* BT_JSR82_ENABLED */ 

static U8 SdapMultipleHdpGroupServiceSearchReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */

    SDP_UUID_16BIT(SC_HDP),   /* UUID for HDP, which has to be replaced as actual sink or source UUID*/
//    SDP_UUID_16BIT(PROT_L2CAP),   /* UUID for HDP, which has to be replaced as actual sink or source UUID*/
//    SDP_UUID_16BIT(PROT_MCAP_CONTROL_CHANNEL),   /* UUID for HDP, which has to be replaced as actual sink or source UUID*/

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x02, 0xA0, /* Max number of bytes for attribute is  */

    /*
     * The third parameter is the actual attribute ID list which is a data
     * element sequence. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 3 bytes */
    SDP_UINT_16BIT(AID_PROTOCOL_DESC_LIST),     /* Value of protocol descriptor list ID */
};

static U8 SdapSearchPnPInfoReq[] =
{
	/* Service Record Handle */
	0x00, 0x00, 0x00, 0x00,

	/* Maximum byte count is 100 */
	0x00, 0x64,

	/* Attribute ID list */
    SDP_ATTRIB_HEADER_8BIT(6),
    SDP_UINT_16BIT(AID_PNP_INFO_VENDOR_ID),
    SDP_UINT_16BIT(AID_PNP_INFO_PRODUCT_ID),
};

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  SDAP_Init
 * DESCRIPTION
 *  This function is used to call by initialize procedure. This function will initial the context of SDAP
 * PARAMETERS
 *  void
 * RETURNS
 *  init result
 *****************************************************************************/
BOOL SDAP_Init(void)
{
    /* Reset all the SDAP context */
    OS_MemSet((U8*) & SDAPController, 0, sizeof(SDAPController));
    SDAP(bmCB) = btbm_dummy_callback;

#ifdef __SDAP_REQUEST_QUEUE__
	sdapInitRequestFreeList();
	InitializeListHead(&SDAP(sdap_request_queue));
#endif

    SDAP_OpenDB();

    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  SdapRemoveLinkCallback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  link        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SdapRemoveLinkCallback(BtRemoteDevice *link)
{
    bt_trace(TRACE_GROUP_1, SDAP_REMOVE_LINKCALLBACK_SDAPMEHANDLER);
    if (link != 0)
    {
        if (IsNodeOnList(&(link->handlers), &(sdapMeHandler.node)))
        {
            bt_trace(TRACE_GROUP_1, BT_SDAP_REMOVE_LINK_HDLER);
            RemoveEntryList(&(sdapMeHandler.node));
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearch
 * DESCRIPTION
 *  This function is used to search peer device service by SdpQueryToken and uuid
 *  The HSP, DUN, SPP, LAP, FAX, AVRCP can use the function to request the service.
 * PARAMETERS
 *  search_pattern      [IN]        Include the information such as:
 *  bdAddr : The Bluetooth Address of remote device
 *  sqt  : SdpQueryToken contains information such as remote device information. uuid...
 *  uuid  : Connect peer device with Profiles == uuid.
 *  callback: Profile callback function of search result
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearch(sdap_service_search_struct search_pattern)
{
    BtStatus status = BT_STATUS_FAILED;
    BtRemoteDevice *rm;
    SdpQueryToken *sqt;
    U16 uuid;
    SdapCallBack callback;

    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(progress));

    /* Check  Previous SDAP operation finished or not */
    if (SDAP(progress) == SDAP_PROGRESS_NONE)
    {
        SDAP(progress) = SDAP_PROGRESS_ONGOING;
        rm = search_pattern.rm;
        sqt = search_pattern.sqt;
        uuid = search_pattern.uuid;
        callback = search_pattern.callback;

        /* Setting the security token */
        SDAP(uuid) = uuid;
        switch (uuid)
        {
            case SC_HEADSET:
            case SC_DIALUP_NETWORKING:
            case SC_SERIAL_PORT:
            case SC_LAN_ACCESS_PPP:
            case SC_FAX:
                /* Replace the string in the service search reqest by the requested uuid */
                StoreBE16(SdapRfcommGroupServiceSearchReq+3,uuid);
                sdap_util_setup_query_token(
                    sqt,
                    SdapRfcommGroupServiceSearchReq,
                    sizeof(SdapRfcommGroupServiceSearchReq),
                    BSQT_SERVICE_SEARCH_ATTRIB_REQ,
                    query_sdap_callback,
                    0x00);
                sqt->uuid = PROT_RFCOMM;
                break;
	    case BT_UUID_GATT_SERVICE_LINKLOSS: // proximity test 
		case BT_UUID_GATT_SERVICE_TXPOWER:
		case BT_UUID_GATT_SERVICE_ALERT:
            case BT_UUID_GATT_SERVICE_TIME:
            case BT_UUID_GATT_SERVICE_TIMEUPDATE:
            case BT_UUID_GATT_SERVICE_DST:
            case SC_AV_REMOTE_CONTROL:
            case SC_AV_REMOTE_CONTROL_TARGET:
            case SC_PANU:
            case SC_GN:
            case SC_NAP:
                StoreBE16(SdapAvrcpGroupServiceSearchReq+3,uuid);
                /* Replace the string in the service search reqest by the requested uuid */
                sdap_util_setup_query_token(
                    sqt,
                    SdapAvrcpGroupServiceSearchReq,
                    sizeof(SdapAvrcpGroupServiceSearchReq),
                    BSQT_SERVICE_SEARCH_ATTRIB_REQ,
                    query_sdap_callback,
                    0x00);
                sqt->uuid = uuid;
                break;
            case SC_RFCOMM_TESTER:
                /* Replace the string in the service search reqest by the requested uuid */
                sdap_util_setup_query_token(
                    sqt,
                    SdapRfcommTesterServiceSearchReq,
                    sizeof(SdapRfcommTesterServiceSearchReq),
                    BSQT_SERVICE_SEARCH_ATTRIB_REQ,
                    query_sdap_callback,
                    0x00);
                sqt->uuid = PROT_RFCOMM;
                break;
            default:
                /* A2DP shall use the multiple attribute request way */
                return BT_STATUS_FAILED;
        }
        sqt->attribId = AID_PROTOCOL_DESC_LIST;
        SDAP(singlecallback) = callback;
        SDAP(cur_sqt) = sqt;
        sqt->rm = rm;
        status = SDP_Query(sqt, BSQM_FIRST);
        if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDP_QUERY_QUERY_BT_STATUS_PENDING);
            status = BT_STATUS_PENDING;
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDP_QUERY_QUERY_BT_STATUS_FAILED_xD, status);
            SDAP(progress) = SDAP_PROGRESS_NONE;
            status = BT_STATUS_FAILED;
        }
    }
    else
    {
        status = BT_STATUS_FAILED;
    }

    return status;
}

#ifdef __BT_GOEP_V2__
U8 SdapMultipleL2capGroupServiceSearchAttribReq[] = 
{
    /*
     * First parameter is the search pattern in data element format. It
     * is a list of 3 UUIDs. 
     */
    SDP_ATTRIB_HEADER_8BIT(3),  /* Data Element Sequence, 9 bytes */

    SDP_UUID_16BIT(SC_OBEX_OBJECT_PUSH),   /* UUID for SC_OBEX_OBJECT_PUSH in Big Endian */
//    SDP_UUID_16BIT(PROT_L2CAP),     /* L2CAP UUID in Big Endian */

    /*
     * The second parameter is the maximum number of bytes that can be
     * be received for the attribute list
     */
    0x00, 0x64, /* Max number of bytes for attribute is 100 */

    SDP_ATTRIB_HEADER_8BIT(3),
    SDP_UINT_16BIT(AID_GOEPL2CAP_PSM)
};
#endif

/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchMultipleAttributeReq
 * DESCRIPTION
 *  Cancel the ServiceSearchMultipleAttribute request if still queued.
 *  If the request is proceeding, just let it complete and return BT_STATUS_FAILED
 * PARAMETERS
 *  rm : remote device
 *  sqt : sdp query toke
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchMultipleAttributeCancel(BtRemoteDevice *rm, SdpQueryToken *sqt)
{
	BtStatus status = BT_STATUS_FAILED;

#ifdef __SDAP_REQUEST_QUEUE__
	SdapQueuedRequest *req;
	for (req = (SdapQueuedRequest *) SDAP(sdap_request_queue).Flink;
			(ListEntry *) req != &SDAP(sdap_request_queue);
			req = (SdapQueuedRequest *) req->node.Flink) {

		if ((req->req_type == SDAP_REQUEST_MULTIPLE_ATTRIBUTE) &&
			(req->params.multi_attr.rm == rm) &&
			(req->params.multi_attr.sqt == sqt)) {

			req->node.Flink->Blink = req->node.Blink;
			req->node.Blink->Flink = req->node.Flink;
			EVM_ResetTimer(&req->req_timer);

			status = BT_STATUS_SUCCESS;
		}
	}
#else
#endif
	return status;
}


/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchMultipleAttribute
 * DESCRIPTION
 *  The function provides to the profiles  multiple attribute request.
 *  The HFP, A2DP can use the function to request the service.
 * PARAMETERS
 *  search_pattern      [IN]        Include the information such as:
 *  bdAddr : The Bluetooth Address of remote device
 *  sqt  : SdpQueryToken contains information such as remote device information. uuid...
 *  uuid  : Connect peer device with Profiles == uuid.
 *  callback: Profile callback function of search result
 *  attribute_num: The list number of profile interesting
 *  attribute_id: The list of attribute id which profile interesting.
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchMultipleAttribute(sdap_service_search_multi_attribute_struct search_pattern)
{
    U8 i = 0;
    BtRemoteDevice *rm;
    SdpQueryToken *sqt;
    U16 uuid;
    SdapMultiAttributeCallBack callback;
    U8 attribute_num;
    U8 uuid_offset = 0;
    U16 *attribute_id;
    BtStatus status = BT_STATUS_FAILED;
    
    /* Check  Previous SDAP operation finished or not */
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(progress));
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(sdap_multi_req_index));
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(sdap_current_multi_req_index));

    if (SDAP(progress) == SDAP_PROGRESS_NONE)
    {
        /* This is do sdp query for a2dp again, reset counter */
#ifdef __BT_A2DP_PROFILE__
        Report(("A2dpSdpQuery  GetSdpQueryCount %d", A2DP_GetSdpQueryCount()));
        if (A2DP_GetSdpQueryCount() == 1)
        {

#ifdef __SDAP_REQUEST_QUEUE__
#else
            bt_trace(TRACE_GROUP_1, RESET_SDAP_MULTI_REQ_INDEX_FOR_A2DP_SDP_QUERY_AGAIN);
            if (SDAP(sdap_current_multi_req_index) == SDAP(sdap_multi_req_index))
            {
                SDAP(sdap_current_multi_req_index) = 0;
                SDAP(sdap_multi_req_index) = 0;
            }
            else
            {
                /* In this case: Existing one pending sdp request from other profile, reject this 2nd a2dp sdp request */
                bt_trace(TRACE_GROUP_1, BT_LOG_SDAP_REJECT_SECOND_A2DP_SDP_DUE_TO_ONE_PENDING);
                return BT_STATUS_FAILED;
            }
#endif

        }
#endif /* __BT_A2DP_PROFILE__ */ 

        SDAP(progress) = SDAP_PROGRESS_MULTIPLE_ONGOING;

#ifdef __SDAP_REQUEST_QUEUE__
#else
        if (SDAP(sdap_multi_req_index) == 0)    /* First call the function */
        {
            SDAP(sdap_multi_req_index)++;
            SDAP(sdap_current_multi_req_index) = 0;
        }
        SDAP(sdap_current_multi_req_index)++;
        Assert(SDAP(sdap_current_multi_req_index) <= SDAP(sdap_multi_req_index));
#endif

        bt_trace(
            TRACE_GROUP_1,
            ABT_DEBUGx02XBBT_DEBUGx02X,
            SDAP(sdap_multi_req_index),
            SDAP(sdap_current_multi_req_index));
        Assert(SDAP(sdap_current_multi_req_index) <= SDAP_MAX_MULTIPLE_REQ);
        sqt = search_pattern.sqt;
        rm = search_pattern.rm;
        uuid = search_pattern.uuid;
        callback = search_pattern.callback;
        attribute_num = search_pattern.attribute_num;
        attribute_id = (search_pattern.attribute_id);

        SDAP(uuid) = uuid;
        SDAP(attribute_num) = attribute_num;
        Assert(attribute_num <= SDAP_MAX_ATTRIBUTE_NUM);
        for (i = 0; i < attribute_num; i++)
        {
            SDAP(attribute_id)[i] = attribute_id[i];
        }

        Report(("A2dpSdpQuery  ServiceSearchMultipleAttribute uuid %04x", uuid));

        switch (uuid)
        {
            case SC_OBEX_FILE_TRANSFER:
            case SC_OBEX_OBJECT_PUSH:
            case SC_MAP_SERVER:
            case SC_MAP_MNS_SERVER:				
#ifdef __BT_GOEP_V2__
            if( NULL != attribute_id && AID_GOEPL2CAP_PSM == attribute_id[0] ){
                // GoepL2CAP Searcp
                SDAP(multiple_attribute_group) = SdapMultipleL2capGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleL2capGroupServiceSearchAttribReq);
                sqt->uuid = uuid;
                SDAP(attribute_offset) = 9;
                uuid_offset = 3;
            }else{
                // RFComm Search
                SDAP(multiple_attribute_group) = SdapMultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = uuid;
                SDAP(attribute_offset) = 15;
                uuid_offset = 3;
            }
                break;
#endif                
            case SC_IMAGING_RESPONDER:
            case SC_DIRECT_PRINTING:
            case SC_PRINTING_STATUS:
                SDAP(multiple_attribute_group) = SdapMultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = uuid;
                SDAP(attribute_offset) = 15;
                uuid_offset = 3;
                break;
            case SC_HANDSFREE:
	     case SC_HANDSFREE_AUDIO_GATEWAY:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = SC_HANDSFREE;
                SDAP(attribute_offset) = 15;
                uuid_offset = 3;
                break;
            case SC_HEADSET:
            case SC_HEADSET_AUDIO_GATEWAY:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = SC_HEADSET;
                SDAP(attribute_offset) = 15;
                uuid_offset = 3;
                break;
            case SC_AUDIO_SINK:
            case SC_AUDIO_SOURCE:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleA2dpGroupServiceSearchReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleA2dpGroupServiceSearchReq);

        #ifdef __BT_A2DP_PROFILE__
                if (A2DP_GetSdpQueryCount() == 0)
        #endif 
                {
                    uuid = SC_AUDIO_DISTRIBUTION;
                    sqt->uuid = SC_AUDIO_DISTRIBUTION;

                }
            #ifdef __BT_A2DP_PROFILE__
                else
                {
                    //uuid = SC_AUDIO_DISTRIBUTION;
                    //using uuid = SC_AUDIO_SINK or SC_AUDIO_SOURCE to do sdp query
                    sqt->uuid = uuid;
                    bt_trace(TRACE_GROUP_1, FOR_A2DP_AGAIN_UUIDx4X, sqt->uuid);
                }
            #endif /* __BT_A2DP_PROFILE__ */ 

                SDAP(attribute_offset) = 9;
                uuid_offset = 3;
                break;
            case SC_HUMAN_INTERFACE_DEVICE:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleHidGroupServiceSearchReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleHidGroupServiceSearchReq);
                uuid = SC_HUMAN_INTERFACE_DEVICE;
                sqt->uuid = SC_HUMAN_INTERFACE_DEVICE;
                SDAP(attribute_offset) = 9;
                uuid_offset = 3;
                break;
	    case BT_UUID_GATT_SERVICE_LINKLOSS: // proximity test 
		case BT_UUID_GATT_SERVICE_TXPOWER:
		case BT_UUID_GATT_SERVICE_ALERT:
            case BT_UUID_GATT_SERVICE_TIME:
            case BT_UUID_GATT_SERVICE_TIMEUPDATE:
            case BT_UUID_GATT_SERVICE_DST:
            case SC_AV_REMOTE_CONTROL:
            case SC_AV_REMOTE_CONTROL_CONTROL:
            case SC_AV_REMOTE_CONTROL_TARGET:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleAVRCPCTGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleAVRCPCTGroupServiceSearchAttribReq);
                sqt->uuid = uuid;
                SDAP(attribute_offset) = 12;
                uuid_offset = 3;
                break;
            case SC_SERIAL_PORT:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = SC_SERIAL_PORT;
                SDAP(attribute_offset) = 15;
                uuid_offset = 3;
                break;
				
			case SC_JSR82:
				SDAP(multiple_attribute_group) = SdapUUID128MultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapUUID128MultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = SC_JSR82;
                SDAP(attribute_offset) = 29;
                uuid_offset = 3;
				break;
				
#ifdef __BT_SUPPORT_SYNCML__                
            /* Add case SC_SYNCML_BT_SERVER */    
            case SC_SYNCML_BT_SERVER:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapUUID128MultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapUUID128MultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = SC_SYNCML_BT_SERVER;
                SDAP(attribute_offset) = 29;
                uuid_offset = 3;
                break;
            case SC_SYNCML_BT_CLIENT:
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapUUID128MultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapUUID128MultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = SC_SYNCML_BT_CLIENT;
                SDAP(attribute_offset) = 29;
                uuid_offset = 3;    
                break;
#endif /* __BT_SUPPORT_SYNCML__ */               
		case SC_HDP_SINK:
		case SC_HDP_SOURCE:
		    SDP_PutU16(SdapMultipleHdpGroupServiceSearchReq + 3, uuid);
                    SDAP(multiple_attribute_group) = SdapMultipleHdpGroupServiceSearchReq;
                    SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleHdpGroupServiceSearchReq);
                    sqt->uuid = uuid;
                    SDAP(attribute_offset) = 9;
                    uuid_offset = 3;
		    break;				
				
            default:
#if defined (BTMTK_ON_WISE) || defined (BTMTK_ON_WISESDK) || defined (BTMTK_ON_WIN32)
            	  /* Bingyi: Customization for user-specific SPP UUID */
                /* Replace the string in the service search reqest by the requested uuid */
                SDAP(multiple_attribute_group) = SdapMultipleRfcommGroupServiceSearchAttribReq;
                SDAP(multiple_attribute_group_size) = sizeof(SdapMultipleRfcommGroupServiceSearchAttribReq);
                sqt->uuid = uuid;
                SDAP(attribute_offset) = 15;
                uuid_offset = 3;
#endif
                break;
        }
#ifdef __BT_SUPPORT_SYNCML__        
        if (sqt->uuid != SC_SYNCML_BT_SERVER && 
			sqt->uuid != SC_SYNCML_BT_CLIENT &&
			sqt->uuid != SC_JSR82)
#else
		if (sqt->uuid != SC_JSR82)
#endif
        {
            StoreBE16(SDAP(multiple_attribute_group)+uuid_offset,uuid);
        }
        else
        {
            OS_MemCopy(SDAP(multiple_attribute_group)+uuid_offset, search_pattern.uuid128_array, 16);
        }
        
        SDAP(multiple_attribute_group)[SDAP(attribute_offset)] = DETD_UINT + DESD_2BYTES;
        StoreBE16(SDAP(multiple_attribute_group)+ SDAP(attribute_offset) + 1, attribute_id[0]);

#if 0  /* old code */      
        SDAP(multiple_attribute_group)[uuid_offset] = (U8) (((uuid) & 0xff00) >> 8);    /* Bits[15:8] of UUID */
        SDAP(multiple_attribute_group)[uuid_offset + 1] = (U8) ((uuid) & 0x00ff),       /* Bits[7:0] of UUID */
        SDAP(multiple_attribute_group)[attribute_offset] = DETD_UINT + DESD_2BYTES;
        SDAP(multiple_attribute_group)[attribute_offset + 1] = (U8) (((attribute_id[0]) & 0xff00) >> 8);
        SDAP(multiple_attribute_group)[attribute_offset + 2] = (U8) ((attribute_id[0]) & 0x00ff);
#endif 
        /* Setting the security token */
        sdap_util_setup_query_token(
            sqt,
            SDAP(multiple_attribute_group),
            SDAP(multiple_attribute_group_size),
            BSQT_SERVICE_SEARCH_ATTRIB_REQ,
            query_sdap_multiple_callback,
            0x00);
        sqt->attribId = attribute_id[0];
        SDAP(current_attribute_index) = 0;
        SDAP(multicallback) = callback;
        SDAP(cur_sqt) = sqt;
        sqt->rm = rm;
        status = SDP_Query(sqt, BSQM_FIRST);
        if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING))
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDP_QUERY_QUERY_BT_STATUS_PENDING);
            status = BT_STATUS_PENDING;
        }
        else
        {
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDP_QUERY_QUERY_BT_STATUS_FAILED_xD, status);
            SDAP(progress) = SDAP_PROGRESS_NONE;
            if (SDAP(sdap_multi_req_index) == (SDAP(sdap_current_multi_req_index)))
            {
                SDAP(sdap_current_multi_req_index) = 0;
                SDAP(sdap_multi_req_index) = 0;
                SDAP(current_attribute_index) = 0;
            }
            status = BT_STATUS_FAILED;
        }
    }
    else if (SDAP(progress) == SDAP_PROGRESS_MULTIPLE_ONGOING)
    {
#ifdef __SDAP_REQUEST_QUEUE__
		if (sdapEnqueueRequestMultipleAttribute(search_pattern) == BT_STATUS_SUCCESS) {
			status = BT_STATUS_PENDING;
		}

#else
        if (SDAP(sdap_multi_req_index) < SDAP_MAX_MULTIPLE_REQ)
        {
            status = BT_STATUS_PENDING;
            SDAP(sdap_multi_req[SDAP(sdap_multi_req_index) - 1]) = search_pattern;
            SDAP(sdap_multi_req_index)++;
        }
        bt_trace(
            TRACE_GROUP_1,
            CBT_DEBUGx02XDBT_DEBUGx02X,
            SDAP(sdap_multi_req_index),
            SDAP(sdap_current_multi_req_index));
#endif
    }
    else
    {
        bt_trace(TRACE_GROUP_1, NOT_IN_MULTIPLE_SDAP_REQUEST);
        status = BT_STATUS_BUSY;
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchMultipleAttribute2
 * DESCRIPTION
 *  The function retrieve SDP attribute by asking the service record handle first.
 * PARAMETERS
 *  search_pattern      [IN]        Include the information such as:
 *  bdAddr : The Bluetooth Address of remote device
 *  sqt  : SdpQueryToken contains information such as remote device information. uuid...
 *  uuid  : Connect peer device with Profiles == uuid.
 *  callback: Profile callback function of search result
 *  attribute_num: The list number of profile interesting
 *  attribute_id: The list of attribute id which profile interesting.
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchMultipleAttribute2(sdap_service_search_multi_attribute_struct search_pattern)
{
	SdpQueryToken *sqt;
	BtRemoteDevice *rm;
	SdapMultiAttributeCallBack callback;
	BtStatus status = BT_STATUS_FAILED;

	U8 i = 0;
	U8 attribute_num;
	U8 uuid_offset = 0;
	U16 uuid;
	U16 *attribute_id;

  
	OS_Report("[SDAP] %s(), progress: %d", __FUNCTION__, SDAP(progress));

    if (SDAP(progress) == SDAP_PROGRESS_NONE) {
        SDAP(progress) = SDAP_PROGRESS_MULTIPLE_ONGOING;

        sqt = search_pattern.sqt;
        rm = search_pattern.rm;
        uuid = search_pattern.uuid;
        callback = search_pattern.callback;
        attribute_num = search_pattern.attribute_num;
        attribute_id = (search_pattern.attribute_id);

        SDAP(uuid) = uuid;
        SDAP(attribute_num) = attribute_num;
        Assert(attribute_num <= SDAP_MAX_ATTRIBUTE_NUM);
        for (i = 0; i < attribute_num; i++) {
            SDAP(attribute_id)[i] = attribute_id[i];
        }

        OS_Report("[SDAP] %s(), uuid: 0x%02x attr count: %d", __FUNCTION__, uuid, attribute_num);

		// For now, this function is crafted for HID only.
        switch (uuid) {
			case SC_HUMAN_INTERFACE_DEVICE:
				/* Replace the string in the service search reqest by the requested uuid */
				SDAP(multiple_attribute_group) = SdapSearchHandleReq;
				SDAP(multiple_attribute_group_size) = sizeof(SdapSearchHandleReq);
				sqt->uuid = SC_HUMAN_INTERFACE_DEVICE;
				uuid_offset = 3;
				break;
				
            default:
                break;
        }

		StoreBE16(SDAP(multiple_attribute_group) + uuid_offset, uuid);

		/* Setting the security token */
		sdap_util_setup_query_token(
				sqt,
				SDAP(multiple_attribute_group),
				SDAP(multiple_attribute_group_size),
				BSQT_SERVICE_SEARCH_REQ,
				query_sdap_multiple_2_handle_callback,
				rm);

		SDAP(multicallback) = callback;
		SDAP(current_attribute_index) = 0;
		SDAP(cur_sqt) = sqt;

		status = SDP_Query(sqt, BSQM_FIRST);
		if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING))	{
			OS_Report("[SDAP] %s(), SDP_Query() returns %d", __FUNCTION__, status);
			status = BT_STATUS_PENDING;

		} else {
			bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDP_QUERY_QUERY_BT_STATUS_FAILED_xD, status);
			SDAP(progress) = SDAP_PROGRESS_NONE;
			if (SDAP(sdap_multi_req_index) == (SDAP(sdap_current_multi_req_index)))
			{
				SDAP(sdap_current_multi_req_index) = 0;
				SDAP(sdap_multi_req_index) = 0;
				SDAP(current_attribute_index) = 0;
			}
			status = BT_STATUS_FAILED;
		}
	}
	else if (SDAP(progress) == SDAP_PROGRESS_MULTIPLE_ONGOING)
	{
#ifdef __SDAP_REQUEST_QUEUE__
		if (sdapEnqueueRequestMultipleAttribute2(search_pattern) == BT_STATUS_SUCCESS) {
			status = BT_STATUS_PENDING;
		}
#else
		OS_Report("[SDAP] Request Queue is not enabled. Just return busy.");
		status = BT_STATUS_BUSY;
#endif
	}
	else
	{
		OS_Report("[SDAP] Not in multiple sdap request.");
		status = BT_STATUS_BUSY;
	}
	return status;
}


/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchAllCancel
 * DESCRIPTION
 *  The function provides to the MMI to cancel current service search
 * PARAMETERS
 *  void
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchAllCancel(BD_ADDR *bdAddr)
{
    BtStatus status = BT_STATUS_FAILED;
    btbm_bd_addr_t temp_bd_addr;
    U8 searched = 0;

    CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdAddr->addr);

#ifdef __SDAP_REQUEST_QUEUE__
	SdapQueuedRequest *req;
	for (req = (SdapQueuedRequest *) SDAP(sdap_request_queue).Flink;
			(ListEntry *) req != &SDAP(sdap_request_queue);
			req = (SdapQueuedRequest *) req->node.Flink) {
		if ((req->req_type == SDAP_REQUEST_ALL) &&
			(OS_MemCmp(req->params.all.bdAddr->addr, 6, bdAddr->addr, 6) == TRUE)) {
			req->node.Flink->Blink = req->node.Blink;
			req->node.Blink->Flink = req->node.Flink;
			EVM_ResetTimer(&req->req_timer);
	        sdapSendServiceSearchCancelled(BTBM_ADP_SUCCESS, temp_bd_addr, searched);
			return BT_STATUS_SUCCESS;
		}
	}

	if (SDAP(progress) == SDAP_PROGRESS_MULTIPLE_ONGOING)
#else
    if (SDAP(progress) == SDAP_PROGRESS_ONGOING)
#endif
    {
        BtDeviceContext *bdc = DS_FindDevice(bdAddr);
        if (bdc != NULL && bdc == SDAP(bdc))
        {
            if (SDP_QueryCancel(&SDAP(local_sqt)) == BT_STATUS_SUCCESS)
            {
                /* To Disconnect Link */
                bdc->link->discFlag = TRUE;
                SdapRemoveLinkCallback(bdc->link);
            }
            else
            {
                /* not yet connected, wait til receiving BTEVENT_LINK_CONNECT_CNF  */
            }
            SDAP(progress) = SDAP_PROGRESS_NONE;
            searched = SDAP(searched_uuid);
        }
        status = BT_STATUS_SUCCESS;
    }

    if (status == BT_STATUS_SUCCESS)
    {
        sdapSendServiceSearchCancelled(BTBM_ADP_SUCCESS, temp_bd_addr, searched);
    }
    else
    {
        sdapSendServiceSearchCancelled(BTBM_ADP_FAILED, temp_bd_addr, searched);
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchAll
 * DESCRIPTION
 *  The function provides to the MMI to search the remote device supported services.
 * PARAMETERS
 *  search_pattern      [IN]        Include the information such as:
 *  bdAddr : The Bluetooth Address of remote device
 *  sdap_len  : the total uuid number
 *  sdp_uuid  : The search service uuid.
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchAll(sdap_service_search_all_struct search_pattern)
{
    BtDeviceContext *bdc = 0;
    BD_ADDR bdAddr;
    U8 sdap_len;
    U32 *sdp_uuid;
    btbm_bd_addr_t temp_bd_addr;

    BtStatus status = BT_STATUS_FAILED;

    /* Check  Previous SDAP operation finished or not */
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(progress));

    if (SDAP(progress) == SDAP_PROGRESS_NONE)
    {
#ifdef __SDAP_REQUEST_QUEUE__
		SDAP(progress) = SDAP_PROGRESS_MULTIPLE_ONGOING;
#else
        SDAP(progress) = SDAP_PROGRESS_ONGOING;
#endif

#ifdef __SDAP_MODIFIED_SEARCH_ALL__
		SDAP(sdap_cur) = 0;
#endif /* __SDAP_MODIFIED_SEARCH_ALL__ */

        OS_MemCopy((U8*) & bdAddr, (U8*) (search_pattern.bdAddr), 6);
        sdap_len = search_pattern.sdap_len;
        sdp_uuid = search_pattern.sdp_uuid;
        SDAP(searched_uuid) = 0;
        SDAP(sdap_len) = sdap_len;
        Assert(sdap_len > 0);
        if (sdap_len != 0)
        {
            OS_MemSet((U8*) SDAP(sdap_uuid), 0, sizeof(SDAP(sdap_uuid)));
            if (sdap_len > SDAP_NUM_RECORD_SEARCH)
            {
                sdap_len = SDAP_NUM_RECORD_SEARCH;
            }
            OS_MemCopy((U8*) SDAP(sdap_uuid), (U8*) sdp_uuid, sdap_len * 4);
        }
        sdapMeHandler.callback = query_sdap_service_search_all_callback;
        if ((bdc = DS_FindDevice(&bdAddr)) == 0)
        {
            if (sdap_util_allocate_free_devices(bdAddr) != BT_STATUS_SUCCESS)
            {
                SDAP(progress) = SDAP_PROGRESS_NONE;
                status = BT_STATUS_FAILED;
                return status;
            }
            bdc = DS_FindDevice(&bdAddr);
        }
        SDAP(bdc) = bdc;
        if (bdc->link == 0)
        {
            /* Acl Link not exist, create the connection */
            status = sdap_util_create_acl_link(bdc);
            if (bdc->link == 0)
            {
                CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdc->addr.addr);
                SDAP(progress) = SDAP_PROGRESS_NONE;
                sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, temp_bd_addr, 0);
                return BT_STATUS_FAILED;
            }
            bdc->link->discFlag = TRUE;
        }
        else
        {
            /* ACL link connection exists, do service search directly. */
            bt_trace(TRACE_GROUP_1, ACL_LINK_CONNECTION_EXISTS_DO_SERVICE_SEARCH_DIRECTLY);
            /* Do SDAP  */
            bt_trace(TRACE_GROUP_1, DO_SDAP_SERVICE);

#ifdef __SDAP_MODIFIED_SEARCH_ALL__
			if (sdap_service_search_request_all((U16) (SDAP(sdap_uuid))[SDAP(sdap_cur)], BSQM_FIRST) ==
				BT_STATUS_PENDING)
#else
            SDAP(sdap_len)--;
            if (sdap_service_search_request_all((U16) (SDAP(sdap_uuid))[SDAP(sdap_len)], BSQM_FIRST) ==
                BT_STATUS_PENDING)
#endif
            {
                status = BT_STATUS_PENDING;
            }
            else
            {
                CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdc->addr.addr);
                SDAP(progress) = SDAP_PROGRESS_NONE;
                sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, temp_bd_addr, 0);
            }
        }
    }
#ifdef __SDAP_REQUEST_QUEUE__
	else if (SDAP(progress) == SDAP_PROGRESS_MULTIPLE_ONGOING) {
		if (sdapEnqueueRequestAll(search_pattern) == BT_STATUS_SUCCESS) {
			return BT_STATUS_PENDING;
		}
		OS_Report("[SDAP] Failed to queue sdap service search all request.");
		return BT_STATUS_FAILED;
	}
#endif
    else
    {
        bt_trace(TRACE_GROUP_1, SDAP_NOT_FINSIHED_REJECT_THE_OPEATION);
        OS_MemCopy((U8*) & bdAddr, (U8*) (search_pattern.bdAddr), 6);
        CONVERT_ARRAY2BDADDR(&temp_bd_addr, (U8*) & bdAddr.addr);
        sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, temp_bd_addr, 0);
    }
    return status;

}

#ifdef __SDAP_SERVICE_SEARCH_ALL_2__
/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchAll_2
 * DESCRIPTION
 *  Search all service by requesting L2CAP UUID
 * PARAMETERS
 *  search_pattern      [IN]        Include the information such as:
 *  bdAddr : The Bluetooth Address of remote device
 *  sdap_len  : the total uuid number
 *  sdp_uuid  : The search service uuid.
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchAll_2(const BD_ADDR bdAddr)
{
	BtDeviceContext *bdc = 0;
	btbm_bd_addr_t temp_bd_addr;
	
	BtStatus status = BT_STATUS_FAILED;
	
	/* Check  Previous SDAP operation finished or not */
	OS_Report("[SDAP] %s(): progress %d", __FUNCTION__, SDAP(progress));

	if (SDAP(progress) == SDAP_PROGRESS_NONE)
	{
#ifdef __SDAP_REQUEST_QUEUE__
		SDAP(progress) = SDAP_PROGRESS_MULTIPLE_ONGOING;
#else
		SDAP(progress) = SDAP_PROGRESS_ONGOING;
#endif

		sdapMeHandler.callback = query_sdap_service_search_all_2_callback;
		if ((bdc = DS_FindDevice(&bdAddr)) == 0)
		{
			if (sdap_util_allocate_free_devices(bdAddr) != BT_STATUS_SUCCESS)
			{
				SDAP(progress) = SDAP_PROGRESS_NONE;
				status = BT_STATUS_FAILED;
				return status;
			}
			bdc = DS_FindDevice(&bdAddr);
		}
		SDAP(searched_uuid) = 0;
		SDAP(bdc) = bdc;
		if (bdc->link == 0)
		{
			/* Acl Link not exist, create the connection */
			status = sdap_util_create_acl_link(bdc);
			if (bdc->link == 0)
			{
				CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdc->addr.addr);
				SDAP(progress) = SDAP_PROGRESS_NONE;
				sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, temp_bd_addr, 0);
				return BT_STATUS_FAILED;
			}
			bdc->link->discFlag = TRUE;
		}
		else
		{
			/* ACL link connection exists, do service search directly. */
			bt_trace(TRACE_GROUP_1, ACL_LINK_CONNECTION_EXISTS_DO_SERVICE_SEARCH_DIRECTLY);
			/* Do SDAP	*/
			bt_trace(TRACE_GROUP_1, DO_SDAP_SERVICE);

			if (sdap_service_search_request_all_2(BSQM_FIRST) == BT_STATUS_PENDING) {
				status = BT_STATUS_PENDING;

			} else {
				CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdc->addr.addr);
				SDAP(progress) = SDAP_PROGRESS_NONE;
				sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, temp_bd_addr, 0);
			}
		}
	}
#ifdef __SDAP_REQUEST_QUEUE__
	else if (SDAP(progress) == SDAP_PROGRESS_MULTIPLE_ONGOING) {
		if (sdapEnqueueRequestAll2(bdAddr) == BT_STATUS_SUCCESS) {
			return BT_STATUS_PENDING;
		}
		OS_Report("[SDAP] Failed to queue sdap service search all2 request.");
		return BT_STATUS_FAILED;
	}
#endif
	else
	{
		bt_trace(TRACE_GROUP_1, SDAP_NOT_FINSIHED_REJECT_THE_OPEATION);
		CONVERT_ARRAY2BDADDR(&temp_bd_addr, (U8*) &bdAddr.addr);
		sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, temp_bd_addr, 0);
	}
	return status;

}
#endif

/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchSingleAttribute
 * DESCRIPTION
 *  The function provides to the MMI or Java to search the remote device supported services by attribute.
 * PARAMETERS
 *  search_pattern      [IN]        Include the information such as:
 *  bdAddr : The Bluetooth Address of remote device
 *  sdp_uuid  : The service uuid
 *  attribute_id  : The service search attribute.
 * RETURNS
 *  U8 : The result status
 *****************************************************************************/
BtStatus SDAP_ServiceSearchSingleAttribute(sdap_service_search_single_attribute_struct search_pattern)
{
    BtDeviceContext *bdc = 0;
    BtStatus status = BT_STATUS_FAILED;
    BD_ADDR bdAddr;

    /* Check  Previous SDAP operation finished or not */
    if (SDAP(progress) == SDAP_PROGRESS_NONE)
    {
        SDAP(progress) = SDAP_PROGRESS_ONGOING;

        OS_MemCopy((U8*) & bdAddr, (U8*) (search_pattern.bdAddr), 6);
        SDAP(sdap_len) = 0x01;
        OS_MemSet((U8*) SDAP(sdap_uuid), 0, sizeof(SDAP(sdap_uuid)));
        if (search_pattern.uuid_len == 16)
        {
            OS_MemCopy(SDAP(sdap_uuid_128), search_pattern.sdp_uuid_128, search_pattern.uuid_len);
        }
        else
        {
            SDAP(sdap_uuid)[0] = search_pattern.sdp_uuid;
        }
        SDAP(sdap_uuid_len) = search_pattern.uuid_len;
        SDAP(attribute_id)[0] = search_pattern.attribute_id;
        sdapMeHandler.callback = query_sdap_search_attribute_callback;

        if ((bdc = DS_FindDevice(&bdAddr)) == 0)
        {
            if (sdap_util_allocate_free_devices(bdAddr) != BT_STATUS_SUCCESS)
            {
                SDAP(progress) = SDAP_PROGRESS_NONE;
                status = BT_STATUS_FAILED;
                return status;
            }
            bdc = DS_FindDevice(&bdAddr);
        }
        SDAP(bdc) = bdc;
        if (bdc->link == 0)
        {
            /* Acl Link not exist, create the connection */
            status = sdap_util_create_acl_link(bdc);
            Assert(status != BT_STATUS_SUCCESS);
            if (bdc->link != 0)
            {
                bt_trace(TRACE_GROUP_1, IN_SDAP_SERVICESEARCHSINGLEATTRIBUTE_SET_DISCFLAG_AS_FALSE);
                bdc->link->discFlag = FALSE;
            }
        }
        else
        {
            /* ACL link connection exists, do service search directly. */
            bt_trace(TRACE_GROUP_1, ACL_LINK_CONNECTION_EXISTS_DO_SERVICE_SEARCH_ATTRIBUTE_DIRECTLY);
            bt_trace(TRACE_GROUP_1, DO_SDAP_SERVICE);
            SDAP(sdap_len)--;
            if (sdap_service_search_attribute_request(SDAP(sdap_uuid_len), SDAP(sdap_uuid_128), (U16)(SDAP(sdap_uuid))[SDAP(sdap_len)], SDAP(attribute_id)[0], BSQM_FIRST) ==
                BT_STATUS_PENDING)
            {
                btbmSdpSearchAttributePendingInquiry(1);
                status = BT_STATUS_PENDING;
                bt_trace(
                    TRACE_GROUP_1,
                    ACL_LINK_EXISTS_MAY_DUE_TO_AFTER_BONDING_PROCEDURE_THEN_CONNECT_SPP_IMMEDIATELY_SET_DISCFLAG_AS_FALSE);
                /* Because the bonding procedure involves sdp query, the link may be disconnected due to L2capDiscACL */
                /* Set discFlag = FLASE, when BT Stack receives BT_SPP_CONNECT_REQ, this flag will be reset TRUE via call CMGR_CreateDataLink() */
                bdc->link->discFlag = FALSE;
            }
        }
        if (status == BT_STATUS_FAILED)
        {
            btbm_bd_addr_t result_addr;
            btbmSdpSearchAttributePendingInquiry(0);
            SDAP(progress) = SDAP_PROGRESS_NONE;
            CONVERT_ARRAY2BDADDR(&result_addr, search_pattern.bdAddr->addr);
            BTBMAdpAttributeSearchComplete(BTBM_ADP_SUCCESS, 
                                           result_addr,
                                           0,
                                           NULL);            
        }
    }
    else
    {
        btbm_bd_addr_t result_addr;
    
        status = BT_STATUS_FAILED;
        btbmSdpSearchAttributePendingInquiry(0);
        CONVERT_ARRAY2BDADDR(&result_addr, search_pattern.bdAddr->addr);
        BTBMAdpAttributeSearchComplete(BTBM_ADP_SUCCESS, 
                                       result_addr,
                                       0,
                                       NULL);         

    }
    return status;
}

#if BT_JSR82_ENABLED


/*****************************************************************************
 * FUNCTION
 *  Sdap_Search_Raw_Retry_Wait_for_Disc_Complete_Timeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void Sdap_Search_Raw_Retry_Wait_for_Disc_Complete_Timeout(EvmTimer *Timer)
{
    bt_bm_search_raw_req_struct *search_pattern = NULL;

    SDAP(sdap_search_raw_retry_timer).func = 0;
    search_pattern = (bt_bm_search_raw_req_struct*) Timer->context;
    /* Report(("Sdap_Search_Raw_Retry_Wait_for_Disc_Complete_Timeout():Retry again, check if disconnecting link is already terminated")); */
    bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_SDAP_SEARCH_RAW_RETRY_WAIT_FOR_DISC_COMPLETE_TIMEOUT);
    SDAP_ServiceSearchAttributeRaw(search_pattern);
}


/*****************************************************************************
 * FUNCTION
 *  SDAP_ServiceSearchAttributeRaw
 * DESCRIPTION
 *  
 * PARAMETERS
 *  search_pattern      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SDAP_ServiceSearchAttributeRaw(bt_bm_search_raw_req_struct *search_pattern)
{
    BtDeviceContext *bdc = 0;
    BD_ADDR bdAddr;
    btbm_bd_addr_t temp_bd_addr;
    BtStatus status = BT_STATUS_FAILED;
    btbm_bd_addr_t *remoteaddr;

    /* Check  Previous SDAP operation finished or not */
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(progress));

    remoteaddr = &(search_pattern->bd_addr);
    CONVERT_BDADDR2ARRAY(bdAddr.addr, remoteaddr->lap, remoteaddr->uap, remoteaddr->nap);
    CONVERT_ARRAY2BDADDR(&temp_bd_addr, (U8*) & bdAddr.addr);

    if ((SDAP(progress) == SDAP_PROGRESS_NONE) &&
        (search_pattern->search_pattern_size < sizeof(SdapServiceSearchRawPattern)))
    {
        SDAP(search_pattern_size) = search_pattern->search_pattern_size;
        OS_MemSet((U8*) SdapServiceSearchRawPattern, 0, sizeof(SdapServiceSearchRawPattern));
        OS_MemCopy((U8*) SdapServiceSearchRawPattern, search_pattern->search_pattern, SDAP(search_pattern_size));
        switch (search_pattern->search_type)
        {
            case BT_JSR82_SERVICE_SEARCH_REQ:
                SDAP(search_type) = BSQT_SERVICE_SEARCH_REQ;
                /* The last two bytes 0x0010: Maximum Service Record Count */
                SdapServiceSearchRawPattern[SDAP(search_pattern_size)] = 0x00;
                SdapServiceSearchRawPattern[SDAP(search_pattern_size) + 1] = 0x10;
                SDAP(search_pattern_size) += 2;
                break;
            case BT_JSR82_SERVICE_ATTRIBUTE_REQ:
                SDAP(search_type) = BSQT_ATTRIB_REQ;
                break;
            case BT_JSR82_SERVICE_SEARCH_ATTRIBUTE_REQ:
                SDAP(search_type) = BSQT_SERVICE_SEARCH_ATTRIB_REQ;
                break;
        }
        SDAP(progress) = SDAP_PROGRESS_ONGOING;
        sdapMeHandler.callback = query_sdap_service_attribute_raw_callback;
        if ((bdc = DS_FindDevice(&bdAddr)) == 0)
        {
            if (sdap_util_allocate_free_devices(bdAddr) != BT_STATUS_SUCCESS)
            {
                SDAP(progress) = SDAP_PROGRESS_NONE;
                status = BT_STATUS_FAILED;
                CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdAddr.addr);
                BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, temp_bd_addr);
                return;
            }
            bdc = DS_FindDevice(&bdAddr);
        }
        SDAP(bdc) = bdc;
        if (bdc->link == 0)
        {
            /* Acl Link not exist, create the connection */
            status = sdap_util_create_acl_link(bdc);
            if (bdc->link == 0)
            {
                SDAP(progress) = SDAP_PROGRESS_NONE;
                CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdAddr.addr);
                BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, temp_bd_addr);
                return;
            }
            bdc->link->discFlag = TRUE;
            SDAP(sdap_search_raw_retry_count) = 0;
        }
        else
        {
            /* ACL link connection exists, do service search directly. */
            bt_trace(TRACE_GROUP_1, ACL_LINK_CONNECTION_EXISTS_DO_SERVICE_SEARCH_DIRECTLY);
            /* Do SDAP  */
            bt_trace(TRACE_GROUP_1, DO_SDAP_SERVICE);
            if ((status = sdap_service_search_attribute_raw_request(BSQM_FIRST)) == BT_STATUS_PENDING)
            {
                status = BT_STATUS_PENDING;
                SDAP(sdap_search_raw_retry_count) = 0;
            }
            else
            {   /* To process this sdp request encounters the ACL link is disconnecting */
                if ((status == BT_STATUS_NO_CONNECTION) && (SDAP(sdap_search_raw_retry_count) < 3))
                {
                    /* Reset SDAP(progress) to NONE */
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    /* Report(("SDAP_SSA_Raw():Wait for Link disconnected,count=%d",SDAP(sdap_search_raw_retry_count))); */
                    bt_trace(TRACE_GROUP_1, BT_LOG_SDAP_SSA_RAW_RETRY_COUNT, SDAP(sdap_search_raw_retry_count));
                    SDAP(sdap_search_raw_retry_count)++;
                    /* Start a timer and wait for timer timeout to enter this function SDAP_ServiceSearchAttributeRaw() again */
                    if (SDAP(sdap_search_raw_retry_timer).func != 0)
                    {
                        EVM_CancelTimer(&SDAP(sdap_search_raw_retry_timer));
                    }

                    SDAP(sdap_search_raw_retry_timer).func = Sdap_Search_Raw_Retry_Wait_for_Disc_Complete_Timeout;
                    SDAP(sdap_search_raw_retry_timer).context = (void*)SdapServiceSearchRawPattern;
                    /* After 50ms, check this link is disconnected or not */
                    EVM_StartTimer(&SDAP(sdap_search_raw_retry_timer), 50);

                }
                else
                {
                    SDAP(sdap_search_raw_retry_count) = 0;
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdAddr.addr);
                    BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, temp_bd_addr);
                    return;
                }
            }
        }
    }
    else
    {
        bt_trace(TRACE_GROUP_1, SDAP_NOT_FINSIHED_REJECT_THE_OPEATION);
        CONVERT_ARRAY2BDADDR(&temp_bd_addr, bdAddr.addr);
        BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, temp_bd_addr);
    }
    return;
}

#endif /* BT_JSR82_ENABLED */ 


/*****************************************************************************
 * FUNCTION
 *  sdap_service_search_request_all
 * DESCRIPTION
 *  
 * PARAMETERS
 *  uuid        [IN]        
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus sdap_service_search_request_all(U16 uuid, SdpQueryMode mode)
{
    BtStatus status = BT_STATUS_FAILED;
    BtRemoteDevice *remDev;
    U8 SDP_INSECURE[]  = {0x30,0x1c, 0x21, 0x4f, 0x91, 0xa2, 0x43, 0xbf,0xa7, 0x95, 0x09, 0xd1, 0x19, 0x8a, 0x81, 0xa7 };
    
    U8 SDP_SECURE[] ={0x85, 0x91, 0xd7, 0x57, 0x18, 0xee, 0x45, 0xe1, 0x9b, 0x12, 0x92, 0x87, 0x5d, 0x06, 0xba, 0x23 };


    Assert(SDAP(bdc)->link != 0x00);

    remDev = SDAP(bdc)->link;

    bt_trace(TRACE_GROUP_1, PSI_INFORMATION_STORED_IN_DB_NOW);
    if((uuid == 0x2001) || 
    (uuid == 0x2002))
    {
        if(uuid == 0x2001)
        {
            OS_MemCopy(SdapServiceSearchArrributePattern128 + 3, SDP_INSECURE, 16);
        }
        if(uuid == 0x2002)
        {
            OS_MemCopy(SdapServiceSearchArrributePattern128 + 3, SDP_SECURE, 16);
        }         
        StoreBE16(SdapServiceSearchArrributePattern128 + 24, AID_SERVICE_CLASS_ID_LIST);
        sdap_util_setup_query_token(
            &(SDAP(local_sqt)),
            SdapServiceSearchArrributePattern128,
            sizeof(SdapServiceSearchArrributePattern128),
            BSQT_SERVICE_SEARCH_ATTRIB_REQ,
            query_sdap_service_search_all_callback,
            remDev);
    }
	else if (uuid == SC_HUMAN_INTERFACE_DEVICE)
	{
		StoreBE16(SdapSearchHandleReq + 3, uuid);
		sdap_util_setup_query_token(
			&(SDAP(local_sqt)),
			SdapSearchHandleReq,
			sizeof(SdapSearchHandleReq),
			BSQT_SERVICE_SEARCH_REQ,
			query_sdap_service_search_all_callback,
			remDev);
	}
    else
    {
    SdapServiceSearchArrributePattern[2] = DETD_UUID + DESD_2BYTES;
    StoreBE16(SdapServiceSearchArrributePattern+3, uuid);
    SdapServiceSearchArrributePattern[9] = DETD_UINT + DESD_2BYTES;
    StoreBE16(SdapServiceSearchArrributePattern+10, AID_SERVICE_CLASS_ID_LIST);

#if 0
    SdapServiceSearchArrributePattern[3] = (U8) (((uuid) & 0xff00) >> 8);
    SdapServiceSearchArrributePattern[4] = (U8) ((uuid) & 0x00ff);
    SdapServiceSearchArrributePattern[10] = (U8) (((AID_SERVICE_CLASS_ID_LIST) & 0xff00) >> 8);
    SdapServiceSearchArrributePattern[11] = (U8) ((AID_SERVICE_CLASS_ID_LIST) & 0x00ff);
#endif

    /* Setup the SDAP token  */
    sdap_util_setup_query_token(
        &(SDAP(local_sqt)),
        SdapServiceSearchArrributePattern,
        sizeof(SdapServiceSearchArrributePattern),
        BSQT_SERVICE_SEARCH_ATTRIB_REQ,
        query_sdap_service_search_all_callback,
        remDev);
    }
    SDAP(local_sqt).mode = mode;
    status = SDP_Query(&SDAP(local_sqt), mode);

    return status;
}

#if BT_JSR82_ENABLED


/*****************************************************************************
 * FUNCTION
 *  sdap_service_search_attribute_raw_request
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus sdap_service_search_attribute_raw_request(SdpQueryMode mode)
{
    BtStatus status = BT_STATUS_FAILED;
    BtRemoteDevice *remDev;

    Assert(SDAP(bdc)->link != 0x00);
    remDev = SDAP(bdc)->link;
    bt_trace(TRACE_GROUP_1, PSI_INFORMATION_STORED_IN_DB_NOW);

    /* Setup the SDAP token  */
    sdap_util_setup_query_token(
        &(SDAP(local_sqt)),
        SdapServiceSearchRawPattern,
        SDAP(search_pattern_size),
        SDAP(search_type),
        query_sdap_service_attribute_raw_callback,
        remDev);
    SDAP(local_sqt).mode = mode;
    status = SDP_Query(&SDAP(local_sqt), mode);

    return status;
}
#endif /* BT_JSR82_ENABLED */ 


/*****************************************************************************
 * FUNCTION
 *  sdap_service_search_attribute_request
 * DESCRIPTION
 *  
 * PARAMETERS
 *  uuid                [IN]        
 *  attribute_id        [IN]        
 *  mode                [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus sdap_service_search_attribute_request(U8 uuid_len, U8 *uuid_128, U16 uuid, U16 attribute_id, SdpQueryMode mode)
{

    BtStatus status = BT_STATUS_FAILED;
    BtRemoteDevice *remDev;

    Assert(SDAP(bdc)->link != 0x00);
    remDev = SDAP(bdc)->link;

    /* Setup the SDAP token  */
    OS_MemSet((U8*) & SDAP(local_sqt), 0, sizeof(SDAP(local_sqt)));

    /* Create the service search attribute request pattern  */
    if (uuid_len == 2)
    {
        StoreBE16(SdapServiceSearchArrributePattern + 3, uuid);
        SdapServiceSearchArrributePattern[2] = DETD_UUID + DESD_2BYTES;
        SdapServiceSearchArrributePattern[9] = DETD_UINT + DESD_2BYTES;
        StoreBE16(SdapServiceSearchArrributePattern + 10, attribute_id);
        sdap_util_setup_query_token(
            &(SDAP(local_sqt)),
            SdapServiceSearchArrributePattern,
            sizeof(SdapServiceSearchArrributePattern),
            BSQT_SERVICE_SEARCH_ATTRIB_REQ,
            query_sdap_search_attribute_callback,
            remDev);
    }
    else
    {
        OS_MemCopy(SdapServiceSearchArrributePattern128 + 3, uuid_128, 16);
        StoreBE16(SdapServiceSearchArrributePattern128 + 24, attribute_id);
        sdap_util_setup_query_token(
            &(SDAP(local_sqt)),
            SdapServiceSearchArrributePattern128,
            sizeof(SdapServiceSearchArrributePattern128),
            BSQT_SERVICE_SEARCH_ATTRIB_REQ,
            query_sdap_search_attribute_callback,
            remDev);
    }
#if 0
    SdapServiceSearchArrributePattern[3] = (U8) (((uuid) & 0xff00) >> 8);
    SdapServiceSearchArrributePattern[4] = (U8) ((uuid) & 0x00ff);
    SdapServiceSearchArrributePattern[10] = (U8) (((attribute_id) & 0xff00) >> 8);
    SdapServiceSearchArrributePattern[11] = (U8) ((attribute_id) & 0x00ff);
#endif

    SDAP(local_sqt).mode = mode;
    status = SDP_Query(&SDAP(local_sqt), mode);
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  query_sdap_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  event       [IN]        event of result
 * RETURNS
 *  void
 * LOCAL AFFECTED
 *  
 *****************************************************************************/
static void query_sdap_callback(const BtEvent *event)
{
    BtStatus status;
    SdpQueryToken *token;
    U16 uuid;

    switch (event->eType)
    {
        case SDEVENT_QUERY_RSP:
            /* SDAP operation success */
            token = event->p.token;
            uuid = token->uuid;
            if ((uuid == SC_AV_REMOTE_CONTROL) || 
                (uuid == SC_AV_REMOTE_CONTROL_TARGET) ||
                (uuid == SC_PANU) ||
                (uuid == SC_NAP) ||
                (uuid == SC_GN) ||
				(uuid == BT_UUID_GATT_SERVICE_TXPOWER)  || 
				(uuid == BT_UUID_GATT_SERVICE_LINKLOSS)  || 
				(uuid == BT_UUID_GATT_SERVICE_ALERT) ||
                (uuid == BT_UUID_GATT_SERVICE_TIME) ||
                (uuid == BT_UUID_GATT_SERVICE_TIMEUPDATE) ||
                (uuid == BT_UUID_GATT_SERVICE_DST)
                )
            {
                if (event->p.token->rLen > 5)
                {
                    status = BT_STATUS_SUCCESS;
                }
                else
                {
                    status = BT_STATUS_FAILED;
                }
            }
            else
            {
                status = SDP_ParseAttributes(token);
            }
            /* SDAP Parse the result */
            if (status == BT_STATUS_SUCCESS)
            {
                bt_trace(
                    TRACE_GROUP_1,
                    SERVICE_SEARCH_COMPLETE_STATUS__BT_DEBUGx02X_TOKEN_VALUE_LENBT_DEBUGx02X,
                    status,
                    token->totalValueLen);
                if ((uuid == SC_AV_REMOTE_CONTROL) || (uuid == SC_AV_REMOTE_CONTROL_TARGET) || (uuid == PROT_AVDTP) ||
                    (uuid == SC_PANU) || (uuid == SC_NAP) || (uuid == SC_GN) ||  (uuid == BT_UUID_GATT_SERVICE_LINKLOSS) ||
                    (uuid == BT_UUID_GATT_SERVICE_TXPOWER) || (uuid == BT_UUID_GATT_SERVICE_ALERT) ||
                    (uuid == BT_UUID_GATT_SERVICE_TIME) || (uuid == BT_UUID_GATT_SERVICE_TIMEUPDATE) ||
                    (uuid == BT_UUID_GATT_SERVICE_DST)

                    )
                {
                    SDAP(singlecallback) (token, BT_STATUS_SUCCESS, 0x00);
                }
                else
                {
                    /* RFCOMM GROUP, found server channel */
                    /* Value returned should be U8 for channel. */
                    Assert(token->totalValueLen == 1);
                    SDAP(singlecallback) (token, BT_STATUS_SUCCESS, token->valueBuff[0]);
                }
                SDAP(progress) = SDAP_PROGRESS_NONE;

            }
            else if (status == BT_STATUS_SDP_CONT_STATE)
            {
                /* We need to continue the query. */
                token->mode = BSPM_CONT_STATE;
                if (SDP_Query(token, BSQM_CONTINUE) != BT_STATUS_PENDING)
                {
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    SDAP(singlecallback) (SDAP(cur_sqt), BT_STATUS_FAILED, 0x00);
                }
            }
            else
            {
                /* SDAP operation Failed */
                bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED_NO_SERVICES);
                SDAP(progress) = SDAP_PROGRESS_NONE;
                SDAP(singlecallback) (token, BT_STATUS_NOSERVICES, 0x00);
            }
            break;
        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            /* SDAP operation query failed, maybe no service */
            token = event->p.token;
            bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
            SDAP(progress) = SDAP_PROGRESS_NONE;
            SDAP(singlecallback) (token, BT_STATUS_FAILED, 0x00);
            /* Do we not indicate this back somehow? */
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  query_sdap_multiple_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  event       [IN]        event of result
 * RETURNS
 *  void
 * LOCAL AFFECTED
 *  
 *****************************************************************************/
static void query_sdap_multiple_callback(const BtEvent *event)
{
    BtStatus status;
    SdpQueryToken *token;

    switch (event->eType)
    {
        case SDEVENT_QUERY_RSP:
            token = event->p.token;
            switch (SDAP(attribute_id)[SDAP(current_attribute_index)])
            {
                case AID_PROTOCOL_DESC_LIST:
                    switch (SDAP(uuid))
                    {
                        case SC_OBEX_FILE_TRANSFER:
                        case SC_OBEX_OBJECT_PUSH:
                        case SC_HANDSFREE:
                        case SC_HANDSFREE_AUDIO_GATEWAY:
                        case SC_HEADSET:
                        case SC_HEADSET_AUDIO_GATEWAY:
                        case SC_IMAGING_RESPONDER:
                        case SC_DIRECT_PRINTING:
                            /*
                             * Add case SC_SERIAL_PORT 
                             * Must to replace token's uuid to RFCOMM's uuid:
                             * Because in SDP_ParseAttributes(): parsing_stage = SDP_PARSE_ATTRIB_VALUE
                             * the last UUID of theis rsp is RFCOMM's uuid, if we want to get servchnl number,
                             * token->uuid sets to PORT_RFCOMM here, so that in SDP_PARSE_ATTRIB_VALUE stage,
                             * Atfer parsing the last UUID and is equal to token's uuid, the parsing stage will change to SDP_PARSE_RESULT_HEADER
                             * And  the server channel number can be obtained successfully
                             */
                        case SC_SERIAL_PORT:
			case SC_JSR82:
#ifdef __BT_SUPPORT_SYNCML__
                        case SC_SYNCML_BT_SERVER:
                        case SC_SYNCML_BT_CLIENT:
#endif /* __BT_SUPPORT_SYNCML__ */                           
                        case SC_MAP_SERVER:
                        case SC_MAP_MNS_SERVER:
                            token->uuid = PROT_RFCOMM;
                            break;
                        case SC_AUDIO_SINK:
                        case SC_AUDIO_SOURCE:
                            token->uuid = PROT_AVDTP;
                            break;
                        case SC_HUMAN_INTERFACE_DEVICE:
                            token->uuid = SC_HUMAN_INTERFACE_DEVICE;
                            break;
						case SC_AV_REMOTE_CONTROL:
						case SC_AV_REMOTE_CONTROL_CONTROL:
						case SC_AV_REMOTE_CONTROL_TARGET:
							token->uuid = PROT_AVCTP;
                            break;
						case SC_HDP_SINK:
						case SC_HDP_SOURCE:
							token->uuid = PROT_L2CAP;
							break;
							
#if defined (BTMTK_ON_WISE) || defined (BTMTK_ON_WISESDK) || defined (BTMTK_ON_WIN32)
                        /* Bingyi: Customization for user-specific SPP UUID */
                        default:
                        	token->uuid = PROT_RFCOMM;
                        	break;
#endif                        	
                    }
                    token->attribId = AID_PROTOCOL_DESC_LIST;
                    break;
                case AID_BT_PROFILE_DESC_LIST:
                    switch (SDAP(uuid))
                    {
                        case SC_HEADSET:
                            token->uuid = SC_HEADSET;
                            break;
                        case SC_HANDSFREE:
                            token->uuid = SC_HANDSFREE;
                            break;
                        case SC_AUDIO_SINK:
                        case SC_AUDIO_SOURCE:

                    #ifdef __BT_A2DP_PROFILE__
                            if (A2DP_GetSdpQueryCount() == 0)
                    #endif 
                                token->uuid = SC_AUDIO_DISTRIBUTION;
                    #ifdef __BT_A2DP_PROFILE__
                            else
                            {
                                token->uuid = SDAP(uuid);
                                bt_trace(TRACE_GROUP_1, FOR_A2DP_AGAIN_RSP_BACKUUIDBT_DEBUGx04X, token->uuid);
                            }
                    #endif /* __BT_A2DP_PROFILE__ */ 
                            break;
					
		    case SC_HDP_SINK:
		    case SC_HDP_SOURCE:
		        token->uuid = SC_HDP;
		        break;
                    }
                    token->attribId = AID_BT_PROFILE_DESC_LIST;
                    break;
                case AID_SUPPORTED_FEATURES:
                    token->uuid = 0;
                    token->attribId = AID_SUPPORTED_FEATURES;
                    break;
                case AID_SUPPORTED_FORMATS_LIST:
                    token->uuid = 0;
                    token->attribId = AID_SUPPORTED_FORMATS_LIST;
                    break;
                case AID_HID_VIRTUAL_CABLE:
                    token->uuid = 0;
                    token->attribId = AID_HID_VIRTUAL_CABLE;
                    break;
                case AID_HID_RECONNECT_INIT:
                    token->uuid = 0;
                    token->attribId = AID_HID_RECONNECT_INIT;
                    break;
                case AID_HID_DESCRIPTOR_LIST:
                    token->uuid = 0;
                    token->attribId = AID_HID_DESCRIPTOR_LIST;
                    break;
                case AID_HID_SDP_DISABLE:
                    token->uuid = 0;
                    token->attribId = AID_HID_SDP_DISABLE;
                    break;                    
                case AID_HID_NORM_CONNECTABLE:
                    token->uuid = 0;
                    token->attribId = AID_HID_NORM_CONNECTABLE;
                    break;                        
                case AID_HID_BOOT_DEVICE:
                    token->uuid = 0;
                    token->attribId = AID_HID_BOOT_DEVICE;
                    break;
                case AID_SERVICE_CLASS_ID_LIST:
                    switch (SDAP(uuid))
                    {
                        case SC_HEADSET:
                        case SC_HANDSFREE:
                            token->uuid = 0;
                            break;
                        default:
                            token->uuid = 0;
                            break;
                    }
                    token->attribId = AID_SERVICE_CLASS_ID_LIST;
                    break;
                case AID_ADDITIONAL_PROT_DESC_LISTS:
                    switch (SDAP(uuid))
                    {
                        case SC_PRINTING_STATUS:
                            token->uuid = PROT_RFCOMM;
                            break;
						case SC_AV_REMOTE_CONTROL:
						case SC_AV_REMOTE_CONTROL_CONTROL:
						case SC_AV_REMOTE_CONTROL_TARGET:
							token->uuid = PROT_AVCTP;
							break;
						case SC_HDP_SINK:
						case SC_HDP_SOURCE:
							token->uuid = PROT_L2CAP;
							break;							
                        default:
                            Assert(0);
                            break;
                    }
                    token->attribId = AID_ADDITIONAL_PROT_DESC_LISTS;
                    break;
		case AID_HDP_SUPPORTED_FEATURES:
		case AID_HDP_MCAP_SUPPORTED_PROC:
                    token->uuid = 0;
                    token->attribId = SDAP(attribute_id)[SDAP(current_attribute_index)];
					OS_Report("[HDP] in sdap, uuid is 0x%x", SDAP(attribute_id)[SDAP(current_attribute_index)]);
		    break;
					
#ifdef __BT_GOEP_V2__
		default:
			token->uuid = 0;
			break;
#endif
            }

            status = SDP_ParseAttributes(token);
            if (status == BT_STATUS_SUCCESS)
            {
                /* Value returned should be U8 for channel. */
                bt_trace(
                    TRACE_GROUP_1,
                    SERVICE_SEARCH_COMPLETEBT_DEBUGx02X_TOTAL_VALUE_LENBT_DEBUGx02X,
                    status,
                    token->totalValueLen);
                bt_trace(
                    BT_TRACE_G1_PROTOCOL,
                    BTLOG_BTVALUE0BT_DEBUGx02XVALUE1BT_DEBUGx02X,
                    token->valueBuff[0],
                    token->valueBuff[1]);
                SDAP(multicallback) (token, BT_STATUS_SUCCESS, SDAP(current_attribute_index), token->valueBuff);
                SDAP(current_attribute_index)++;
                OS_Report("query_sdap_multiple_callback: proceed to attrib[%d]", SDAP(current_attribute_index));
                if (SDAP(current_attribute_index) == SDAP(attribute_num))
                {
					sdap_process_next_request(token->rm);
                }
                else
                {
                    SDAP(multiple_attribute_group)[SDAP(attribute_offset)] = DETD_UINT + DESD_2BYTES;
                    SDAP(multiple_attribute_group)[SDAP(attribute_offset) + 1] =
                        (U8) (((SDAP(attribute_id)[SDAP(current_attribute_index)]) & 0xff00) >> 8);
                    SDAP(multiple_attribute_group)[SDAP(attribute_offset) + 2] =
                        (U8) ((SDAP(attribute_id)[SDAP(current_attribute_index)]) & 0x00ff);
                    sdap_util_setup_query_token(
                        token,
                        SDAP(multiple_attribute_group),
                        SDAP(multiple_attribute_group_size),
                        BSQT_SERVICE_SEARCH_ATTRIB_REQ,
                        query_sdap_multiple_callback,
                        token->rm);
                    if (SDP_Query(token, BSQM_FIRST) == BT_STATUS_PENDING)
                    {
                        return;
                    }
                    else
                    {
                        SDAP(multicallback) (token, BT_STATUS_FAILED, SDAP(current_attribute_index), 0x00);
						sdap_process_next_request(token->rm);
                    }
                }
            }
            else if (status == BT_STATUS_SDP_CONT_STATE)
            {
                /* We need to continue the query. */
                token->mode = BSPM_CONT_STATE;
                SDAP(multicallback) (token, BT_STATUS_SDP_CONT_STATE, SDAP(current_attribute_index), token->valueBuff);
                if (SDP_Query(token, BSQM_CONTINUE) != BT_STATUS_PENDING)
                {
#ifdef __SDAP_REQUEST_QUEUE__
					/* If the queue mechanism is activated, don't change the progress state before callback. */
#else
                    SDAP(progress) = SDAP_PROGRESS_NONE;
#endif
                    SDAP(multicallback) (token, BT_STATUS_FAILED, SDAP(current_attribute_index), 0x00);

					sdap_process_next_request(token->rm);
                }
            }
            else
            {
                bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
                bt_trace(
                    TRACE_GROUP_1,
                    SERVICE_ATTRIBUTE_IDBT_DEBUGx04X,
                    SDAP(attribute_id)[SDAP(current_attribute_index)]);
                /* Can not findout the services */
                SDAP(progress) = SDAP_PROGRESS_NONE;
                SDAP(multicallback) (token, BT_STATUS_NOSERVICES, SDAP(current_attribute_index), 0x00);

                /*
                 * In this case (BT_STATUS_NOSERVICES) and if this sdp request is issued by a2dp:
                 * The SDAP(multicallback)() will be callback to bt_a2dp_sdp_query_callback(), then use another UUID to ask peer device,
                 *  If SDAP(sdap_multi_req_index) != (SDAP(sdap_current_multi_req_index):
                 * -> This means that existing one pending sdp request from other profile is issued during waiting for a2dp's sdp rsp
                 * In this way, 2nd a2dp should not be allowed. 
                 * Otherwise, when it returns from SDAP_ServiceSearchMultipleAttribute() with PENDING and returns from bt_a2dp_sdp_query_callback to here:
                 * Continue to the following check, sdap_restart_multiple_req() will be rejected due to 
                 * -> SDAP(sdap_multi_req_index) > SDAP_MAX_MULTIPLE_REQ
                 */

            #ifdef __BT_A2DP_PROFILE__
                if (A2DP_GetSdpQueryCount() == 0)
            #endif 
                {
					sdap_process_next_request(token->rm);
                }
            }
            break;
        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            token = event->p.token;
            bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
#ifdef __SDAP_REQUEST_QUEUE__
			/* If the queue mechanism is activated, don't change the progress state before callback. */
#else
            SDAP(progress) = SDAP_PROGRESS_NONE;
#endif
            SDAP(multicallback) (token, BT_STATUS_FAILED, SDAP(current_attribute_index), 0x00);
            /* In this cases (SDEVENT_QUERY_ERR/FAILED), the SDAP(multicallback)() to a2dp will not casue to ask again */

#ifdef __SDAP_REQUEST_QUEUE__
			if (event->p.token->rm->state != BDS_CONNECTED) {
				OS_Report("%s(): ACL link is gone.", __FUNCTION__);
				sdapClearRequestSameAddr(&(event->p.token->rm->bdAddr));
			}
#endif

			sdap_process_next_request(token->rm);

            /* Do we not indicate this back somehow? */
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  query_sdap_multiple_2_handle_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  event       [IN]        event of result
 * RETURNS
 *  void
 * LOCAL AFFECTED
 *  
 *****************************************************************************/
static void query_sdap_multiple_2_handle_callback(const BtEvent *event)
{
	BtStatus status = BT_STATUS_FAILED;
	SdpQueryToken* token;
	U16 total_record_count = 0, cur_record_count = 0;
	U8* results;

	token = event->p.token;
	if (token == NULL) {
		OS_Report("[SDAP][ERR] %s(), token is null.", __FUNCTION__);
		return;
	}

	results = token->results;

    switch (event->eType) {
		case SDEVENT_QUERY_RSP:
			OS_Report("[SDAP] %s(), rLen: %d", __FUNCTION__, token->rLen);
			OS_Report("[SDAP] result dump 0x%02x 0x%02x 0x%02x 0x%02x",
					results[0],	results[1],	results[2],	results[3]);

			if (token->rLen < 5) {
				status = BT_STATUS_NOSERVICES;
				goto report_fail;
			}

			total_record_count = SDP_GetU16(results);
			cur_record_count = SDP_GetU16(results + 2);

			if (total_record_count < 1 || cur_record_count < 1) {
				status = BT_STATUS_NOSERVICES;
				goto report_fail;
			}

			SDAP(record_handle) = SDP_GetU32(results + 4);
			OS_Report("[SDAP] service record handle: 0x%08x", SDAP(record_handle));

			StoreBE32(SdapSearchAttributeReq, SDAP(record_handle));
			StoreBE16(SdapSearchAttributeReq + 9, SDAP(attribute_id)[0]);

			sdap_util_setup_query_token(
					token,
					SdapSearchAttributeReq,
					sizeof(SdapSearchAttributeReq),
					BSQT_ATTRIB_REQ,
					query_sdap_multiple_2_attribute_callback,
					token->rm);

			status = SDP_Query(token, BSQM_FIRST);
			if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
				OS_Report("[SDAP] %s(), SDP_Query() returns %d.", __FUNCTION__, status);
				status = BT_STATUS_FAILED;
				goto report_fail;
			}
			return;

		case SDEVENT_QUERY_ERR:
			/* CONTINUE, SDAP operation query error */
		case SDEVENT_QUERY_FAILED:
			bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
			OS_Report("[SDAP] %s(), event: %d", __FUNCTION__, event->eType);
			break;
	}

report_fail:

#ifdef __SDAP_REQUEST_QUEUE__
	SDAP(multicallback) (token, status, SDAP(current_attribute_index), 0x00);
	if (event->p.token->rm->state != BDS_CONNECTED) {
		OS_Report("[SDAP] %s(): ACL link is gone.", __FUNCTION__);
		sdapClearRequestSameAddr(&(event->p.token->rm->bdAddr));
	}
#else
	SDAP(progress) = SDAP_PROGRESS_NONE;
	SDAP(multicallback) (token, status, SDAP(current_attribute_index), 0x00);
#endif

	sdap_process_next_request(token->rm);
	return;
}

/*****************************************************************************
 * FUNCTION
 *  query_sdap_multiple_2_attribute_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  event       [IN]        event of result
 * RETURNS
 *  void
 * LOCAL AFFECTED
 *  
 *****************************************************************************/
static void query_sdap_multiple_2_attribute_callback(const BtEvent *event)
{
	BtStatus status = BT_STATUS_FAILED;
	SdpQueryToken* token;
	U32 len = 0;
	U16 att_bytes = 0, offset = 0;
	U8* results;

	token = event->p.token;
	if (token == NULL) {
		OS_Report("[SDAP][ERR] %s(), token is null.", __FUNCTION__);
		return;
	}

	results = event->p.token->results;

	switch (event->eType) {
		case SDEVENT_QUERY_RSP:
			OS_Report("[SDAP] %s(), rLen: %d", __FUNCTION__, token->rLen);
			OS_Report("[SDAP] result dump 0x%02x 0x%02x", results[0], results[1]);

			att_bytes = SDP_GetU16(results);
			len = SdpParseElement(results + 2, &offset);

			// For some IOT device, we need to check if the parameters are consistent.
			if (att_bytes <= offset) {
				OS_Report("[SDAP] %s(), Invalid parameters. att_bytes: %d, offset: %d, len: %d",
						__FUNCTION__, att_bytes, offset, len);
				status = BT_STATUS_NOSERVICES;
				goto report_fail;
			}

			if (token->rLen < 5) {
				status = BT_STATUS_NOSERVICES;
				goto report_fail;
			}

			if (token->mode == BSQM_CONTINUE) {
				token->mode = BSPM_CONT_STATE;
			}

			token->uuid = 0;
			token->attribId = SDAP(attribute_id)[SDAP(current_attribute_index)];
			OS_Report("[SDAP] attribute id: 0x%04x", token->attribId);

			status = SDP_ParseAttributes(token);
			if (status == BT_STATUS_SUCCESS) {
				SDAP(multicallback) (token, BT_STATUS_SUCCESS, SDAP(current_attribute_index), token->valueBuff);
				SDAP(current_attribute_index)++;

				OS_Report("[SDAP] %s(), proceed to attrib[%d]", __FUNCTION__, SDAP(current_attribute_index));
				if (SDAP(current_attribute_index) == SDAP(attribute_num)) {
					sdap_process_next_request(token->rm);
					
				} else {
					StoreBE16(SdapSearchAttributeReq + 9, SDAP(attribute_id)[SDAP(current_attribute_index)]);

					sdap_util_setup_query_token(
							token,
							SdapSearchAttributeReq,
							sizeof(SdapSearchAttributeReq),
							BSQT_ATTRIB_REQ,
							query_sdap_multiple_2_attribute_callback,
							token->rm);
						
					status = SDP_Query(token, BSQM_FIRST);
					if (status != BT_STATUS_SUCCESS || status != BT_STATUS_PENDING) {
						OS_Report("[SDAP] %s(), SDP_Query() returns %d.", __FUNCTION__, status);
						goto report_fail;
					}
				}
			}
			else if (status == BT_STATUS_SDP_CONT_STATE)
			{
				/* We need to continue the query. */
				token->mode = BSPM_CONT_STATE;
				SDAP(multicallback) (token, BT_STATUS_SDP_CONT_STATE, SDAP(current_attribute_index), token->valueBuff);
				status = SDP_Query(token, BSQM_CONTINUE);
				if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
					OS_Report("[SDAP] %s(), SDP_Query() returns %d.", __FUNCTION__, status);
					goto report_fail;
				}
			}
			else
			{
				bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
				OS_Report("[SDAP] %s(), Cannot find the attribute 0x%02x", __FUNCTION__, token->attribId);
				status = BT_STATUS_NOSERVICES;
				goto report_fail;

			}
			return;

		case SDEVENT_QUERY_ERR:
			/* CONTINUE, SDAP operation query error */
		case SDEVENT_QUERY_FAILED:
			bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
			OS_Report("[SDAP] %s(), event: %d", __FUNCTION__, event->eType);
			break;
	}

report_fail:

#ifdef __SDAP_REQUEST_QUEUE__
	SDAP(multicallback) (token, status, SDAP(current_attribute_index), 0x00);
	if (event->p.token->rm->state != BDS_CONNECTED) {
		OS_Report("[SDAP] %s(): ACL link is gone.", __FUNCTION__);
		sdapClearRequestSameAddr(&(event->p.token->rm->bdAddr));
	}
#else
	SDAP(progress) = SDAP_PROGRESS_NONE;
	SDAP(multicallback) (token, status, SDAP(current_attribute_index), 0x00);
#endif

	sdap_process_next_request(token->rm);
	return;
}

/*****************************************************************************
 * FUNCTION
 *  query_sdap_service_search_all_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  Event       [IN]        
 *  event  : event of result
 * RETURNS
 *  void
 *****************************************************************************/
static void query_sdap_service_search_all_callback(const BtEvent *Event)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtRemoteDevice *remDev;
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t bd_addr;
    SdpQueryToken *token;
    BMCallbackParms parms;    
    U16 *support_uuid;
    U8 i;
	
    CONVERT_ARRAY2BDADDR(&bd_addr, SDAP(bdc)->addr.addr);
    switch (Event->eType)
    {
        case SDEVENT_QUERY_RSP:
            bt_trace(
                TRACE_GROUP_1,
                RESULTBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
                Event->p.token->results[0],
                Event->p.token->results[1],
                Event->p.token->results[2],
                Event->p.token->results[3],
                Event->p.token->results[4],
                Event->p.token->results[5]);
            if (Event->p.token->rLen > 5)
            {
                status = BT_STATUS_SUCCESS;

#ifdef __SDAP_MODIFIED_SEARCH_ALL__
				Event->p.token->uuid = (U16) SDAP(sdap_uuid)[SDAP(sdap_cur)];
#else
                Event->p.token->uuid = (U16) SDAP(sdap_uuid)[SDAP(sdap_len)];
#endif
                if (Event->p.token->uuid == SC_AV_REMOTE_CONTROL_TARGET)
                {
                    Event->p.token->attribId = AID_SERVICE_CLASS_ID_LIST;
                    status = SDP_ParseAttributes(Event->p.token);
                    if (status != BT_STATUS_SUCCESS)
                    {
                        Event->p.token->uuid = SC_AV_REMOTE_CONTROL;
                        status = SDP_ParseAttributes(Event->p.token);
                    }
                }
                else if (Event->p.token->uuid == SC_SERIAL_PORT)
                {
                    token = Event->p.token;
                    Event->p.token->attribId = AID_SERVICE_CLASS_ID_LIST;
                    status = SDP_ParseAttributes(token);
                    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAP_STATUSBT_DEBUGx02X, status);
                    if (status == BT_STATUS_SUCCESS)
                    {
                        token = Event->p.token;
                        token->uuid = SC_GENERIC_AUDIO;
                        status = SDP_ParseAttributes(token);
                        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAP_STATUSBT_DEBUGx02X, status);

                        if (status == BT_STATUS_FAILED)
                        {
                            status = BT_STATUS_SUCCESS;
                        }
                        else
                        {
                            status = BT_STATUS_FAILED;
                        }
                    }
                }
				else if (Event->p.token->uuid == SC_HUMAN_INTERFACE_DEVICE)
				{
					// Get TotalServiceRecordCount from SDP_ServiceSearchResponse PDU
					U16 totalRecordCount = SDP_GetU16(Event->p.token->results);
					OS_Report("[SDAP] %s(), uuid: %02x, record count: %d",
							__FUNCTION__, Event->p.token->uuid, totalRecordCount);

					if (totalRecordCount > 0) {
						status = BT_STATUS_SUCCESS;
					} else {
						status = BT_STATUS_FAILED;
					}

					U32 len = 0, handle = 0;
					U16 offset = 0, attBytes = 0;
					U8 *data;

					data = Event->p.token->results + 4;
					len = SdpParseElement(data, &offset);
					handle = SDP_GetU32(data + offset);
					OS_Report("[SDAP] %s(), HID service record handle: %d", __FUNCTION__, handle);

					attBytes = SDP_GetU16(Event->p.token->results);
					OS_Report("[SDAP] %s(), HID attribute bytes: %d", __FUNCTION__, attBytes);
				}
                else
                {
                    if(((SDAP(sdap_uuid))[SDAP(sdap_len)] == 0x2001) ||
                      ((SDAP(sdap_uuid))[SDAP(sdap_len)] == 0x2002))  
                    {
                        Event->p.token->attribId = AID_SERVICE_CLASS_ID_LIST;                        
                        status = BT_STATUS_SUCCESS;
                    }
                    else
                    {
                    Event->p.token->attribId = AID_SERVICE_CLASS_ID_LIST;
                    status = SDP_ParseAttributes(Event->p.token);
                	}
                    Report(("SDP parse status:%d", status));
                }
                if (status == BT_STATUS_SUCCESS)
                {
                    parms.event = BTBM_EVENT_SERVICE_SEARCH_RESULT;

#ifdef __SDAP_MODIFIED_SEARCH_ALL__
					parms.p.ReportServiceSearchResult.uuid = SDAP(sdap_uuid)[SDAP(sdap_cur)];
#else
                    parms.p.ReportServiceSearchResult.uuid = SDAP(sdap_uuid)[SDAP(sdap_len)];
#endif
                        parms.p.ReportServiceSearchResult.bd_addr = bd_addr;
                        SDAP(bmCB)(&parms);                    
                        SDAP(searched_uuid)++;
                }
            }

			if (Event->p.token->rLen == 5 || status != BT_STATUS_SUCCESS) {
#ifdef __SDAP_MODIFIED_SEARCH_ALL__
				Event->p.token->uuid = (U16) SDAP(sdap_uuid)[SDAP(sdap_cur)];
#else
				Event->p.token->uuid = (U16) SDAP(sdap_uuid)[SDAP(sdap_len)];
#endif                
				OS_Report("[SDAP] %s() rLen == 5.", __FUNCTION__);
				support_uuid = btGetDevCustvalueById(Event->p.token->rm->devId, CUSTID_DEV_SUPPORT_SERVICES);
                           //OS_Report("[SDAP] Event->p.token->uuid=%u",Event->p.token->uuid);
				if (support_uuid != NULL) {
					for (i = 0; support_uuid[i] != 0; i++) {
                                        //OS_Report("[SDAP] support_uuid[%u])=%u",i,support_uuid[i]);
						if (Event->p.token->uuid == support_uuid[i]) {
                                                parms.event = BTBM_EVENT_SERVICE_SEARCH_RESULT;                            
							OS_Report("[SDAP] %s() Supported cust UUID: 0x%04x", __FUNCTION__,
								support_uuid[i]);
							parms.p.ReportServiceSearchResult.uuid = Event->p.token->uuid;
							parms.p.ReportServiceSearchResult.bd_addr = bd_addr;
							SDAP(bmCB)(&parms);
							SDAP(searched_uuid)++;
							break;
						}
					}
				}
            }

#ifdef __SDAP_MODIFIED_SEARCH_ALL__
			SDAP(sdap_cur)++;
			if (SDAP(sdap_cur) < SDAP(sdap_len))
			{
				bt_trace(TRACE_GROUP_1, PARSE_NEXT);
				status = sdap_service_search_request_all((U16) (SDAP(sdap_uuid))[SDAP(sdap_cur)], BSQM_FIRST);

#else
            if (SDAP(sdap_len) > 0)
            {
                bt_trace(TRACE_GROUP_1, PARSE_NEXT);
                SDAP(sdap_len)--;
                status = sdap_service_search_request_all((U16) (SDAP(sdap_uuid))[SDAP(sdap_len)], BSQM_FIRST);
#endif

                if (status != BT_STATUS_PENDING)
                {
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    if (Event->p.token->rm != 0)
                    {
                        Event->p.token->rm->discFlag = TRUE;
                        Event->p.token->rm->discTimeoutLevel = BT_L2CAP_DISCONNECT_TIMEOUT_50S;
                        bt_trace(
                            TRACE_GROUP_1,
                            IN_QUERY_SDAP_SERVICE_SEARCH_ALL_CALLBACK_SET_DISCLEVEL_BT_L2CAP_DISCONNECT_TIMEOUT_50S);
                    }

                    SdapRemoveLinkCallback(Event->p.token->rm);
                    sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, SDAP(searched_uuid));

					sdap_process_next_request(Event->p.token->rm);
                }
            }
            else
            {
                if (Event->p.token->rm != 0)
                {
                    Event->p.token->rm->discFlag = TRUE;
                }
                SdapRemoveLinkCallback(Event->p.token->rm);
                sdapSendServiceSearchCompleted(BTBM_ADP_SUCCESS, bd_addr, SDAP(searched_uuid));
                bt_trace(TRACE_GROUP_1, PARSE_COMPLETE);

                sdap_search_pnp_info_request(BSQM_FIRST);
                // SDAP(progress) = SDAP_PROGRESS_NONE;
				// sdap_process_next_request(Event->p.token->rm);
            }
            break;
        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            if (Event->p.token->rm != 0)
            {
                Event->p.token->rm->discFlag = TRUE;
            }
            SDAP(progress) = SDAP_PROGRESS_NONE;
            bt_trace(TRACE_GROUP_1, PARSE_COMPLETE_WITH_ERROR);
            SdapRemoveLinkCallback(Event->p.token->rm);
            sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, SDAP(searched_uuid));

#ifdef __SDAP_REQUEST_QUEUE__
			if (Event->p.token->rm->state != BDS_CONNECTED) {
				OS_Report("[SDAP] %s(): ACL link is gone.", __FUNCTION__);
				sdapClearRequestSameAddr(&(Event->p.token->rm->bdAddr));
			}
#endif

			sdap_process_next_request(Event->p.token->rm);
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            if (Event->errCode == BEC_NO_ERROR)
            {
                remDev = Event->p.remDev;
                bdc = DS_FindDevice(&(remDev->bdAddr));
                if (bdc != 0)
                {
                    bdc->link = remDev;
                }

                if (SDAP(progress) == SDAP_PROGRESS_NONE)  /* cancelled */
                {
                    bdc->link->discFlag = TRUE;
                    /* sdapMeHandler will be removed from remDev handlerList in ME_ForceDisconnectLinkWithReason() */
                    ME_ForceDisconnectLinkWithReason(&sdapMeHandler, bdc->link, BEC_USER_TERMINATED, TRUE);
                }
                else
                {

#ifdef __SDAP_MODIFIED_SEARCH_ALL__
					if (bdc == SDAP(bdc))
					{
						status = sdap_service_search_request_all((U16) (SDAP(sdap_uuid))[SDAP(sdap_cur)], BSQM_FIRST);
						if (status != BT_STATUS_PENDING)
						{
							SDAP(progress) = SDAP_PROGRESS_NONE;
						}
					}
#else
                    SDAP(sdap_len)--;
                    if (bdc == SDAP(bdc))
                    {
                        status = sdap_service_search_request_all((U16) (SDAP(sdap_uuid))[SDAP(sdap_len)], BSQM_FIRST);
                        if (status != BT_STATUS_PENDING)
                        {
                            SDAP(progress) = SDAP_PROGRESS_NONE;
                        }
    
                    }
#endif
                }
            }
            else
            {
                if (Event->p.token->rm != 0)
                {
                    Event->p.token->rm->discFlag = TRUE;
                }
                SDAP(progress) = SDAP_PROGRESS_NONE;
                sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, SDAP(searched_uuid));
            }
            break;

    }
}

#if BT_JSR82_ENABLED


/*****************************************************************************
 * FUNCTION
 *  check_continute_query
 * DESCRIPTION
 *  
 * PARAMETERS
 *  token           [IN]        
 *  start_pos       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void check_continute_query(SdpQueryToken *token, U16 start_pos)
{
    if (token->contStateLen > 1)
    {
        int i = 0;

        memset(token->parseData.storedBuff, 0, 17);
        /* first 1 byte (storedBuff[0]): length field of contstate data information: its value will be 1-17 bytes */
        token->parseData.storedBuff[0] = token->contStateLen - 1;
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CHECK_CONSTATE_VALUE, token->parseData.storedBuff[0]);
        /* From second bytes and its later byte, the actual contstate information */
        memcpy(token->parseData.storedBuff + 1, token->results + start_pos, token->contStateLen - 1);
        for (i = 0; i < token->parseData.storedBuff[0]; i++)
        {
            bt_trace(
                BT_TRACE_JSR82_GROUP,
                BT_JSR82_CHECK_DUMP_CONSTATE_DATA_INX,
                i,
                token->parseData.storedBuff[1 + i]);
        }
        token->contState = token->parseData.storedBuff;
    }
    else
    {
        token->contState[0] = 0x00;
        memset(token->parseData.storedBuff, 0, 17);
        SDPC(contState) = 0;
        /* Report(("check_continute_query():End of sdp request, reset continute state to zero")); */
        bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_CHECK_CONTINUE_QUERY);
    }

}


/*****************************************************************************
 * FUNCTION
 *  query_sdap_service_attribute_raw_callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void query_sdap_service_attribute_raw_callback(const BtEvent *Event)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtRemoteDevice *remDev;
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t bd_addr;

    /* SdpQueryToken    *token; */

    CONVERT_ARRAY2BDADDR(&bd_addr, SDAP(bdc)->addr.addr);
    switch (Event->eType)
    {
        case SDEVENT_QUERY_RSP:
            bt_trace(
                TRACE_GROUP_1,
                RESULTBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02XBT_DEBUGx02X,
                Event->p.token->results[0],
                Event->p.token->results[1],
                Event->p.token->results[2],
                Event->p.token->results[3],
                Event->p.token->results[4],
                Event->p.token->results[5]);
            if (Event->p.token->rLen > 0)
            {
                switch (SDAP(search_type))
                {
                    case BSQT_SERVICE_SEARCH_REQ:
                    {
                        /*
                         * rLen: length of result; rLen = 2bytes TotalServiceRecordCount + 2 bytes CurrentServiceRecordCount + (CurrentServiceRecordCount*4) + ContinuationState Length 
                         * * results+4: points to ServiceRecordHandleList
                         */
                        U16 current_record_count = BEtoHost16(Event->p.token->results + 2);
                        U16 start_pos = 0;

                        /* Report(("query_sa_raw_cb:currentServiceRecordCount=%d",current_record_count)); */
                        bt_trace(TRACE_GROUP_1, BT_LOG_QUERY_SA_RAW_CB_RECORD_COUNT, current_record_count);
                        Event->p.token->contStateLen = Event->p.token->rLen - 2 - 2 - 4 * current_record_count;
                        /* Report(("query_sa_raw_cb:ContStateLen from ss rsp =%d",Event->p.token->contStateLen)); */
                        bt_trace(TRACE_GROUP_1, BT_LOG_QUERY_SA_RAW_CB_CONT_STATE_LEN, Event->p.token->contStateLen);
                        /* 2bytes (TotalServiceRecordCount) + 2bytes (CurrentServoceRecordCount) + 4*current_record_count bytes + 1 byte (ContinuationState) */
                        start_pos = 2 + 2 + 4 * current_record_count + 1;
                        check_continute_query(Event->p.token, start_pos);

                        BTBMAdpSearchAttributeRawResult(
                            Event->p.token->rLen - Event->p.token->contStateLen - 4,
                            Event->p.token->results + 4,
                            bd_addr);
                    }
                        break;
                    case BSQT_ATTRIB_REQ:
                    {
                        /*
                         * rLen: length of result; rLen = 2 bytes AttributeListByteCount + AttributeList + ContinuationState Length
                         * * results+2: points to AttributeList
                         */
                        U16 attributelist_bytecount = BEtoHost16(Event->p.token->results);
                        U16 start_pos = 0;

                        /* Report(("query_sa_raw_cb:attributelist count=%d,contStateLen=%d,case=%d",attributelist_bytecount,Event->p.token->contStateLen)); */
                        bt_trace(
                            TRACE_GROUP_1,
                            BT_LOG_QUERY_SA_RAW_CB_ATTRLIST_COUNT_CONTSTATE_LEN_CASE,
                            attributelist_bytecount,
                            Event->p.token->contStateLen,
                            1);
                        Event->p.token->contStateLen = Event->p.token->rLen - 2 - attributelist_bytecount;
                        /* Report(("query_sa_raw_cb:ContStateLen from sa rsp =%d,case=%d",Event->p.token->contStateLen)); */
                        bt_trace(
                            TRACE_GROUP_1,
                            BT_LOG_QUERY_SA_RAW_CB_CONSTATE_LEN_FROM_SA_RSP_CASE,
                            Event->p.token->contStateLen,
                            1);
                        /* 2bytes (AttributeListsByteCount) + attributelist_bytecount + 1 byte (ContinuationState) */
                        start_pos = 2 + attributelist_bytecount + 1;
                        check_continute_query(Event->p.token, start_pos);
                        BTBMAdpSearchAttributeRawResult(
                            Event->p.token->rLen - Event->p.token->contStateLen - 2,
                            Event->p.token->results + 2,
                            bd_addr);
                    }
                        break;
                    case BSQT_SERVICE_SEARCH_ATTRIB_REQ:
                    {
                        /*
                         * rLen: length of result; rLen = 2 bytes AttributeListsByteCount + AttributeLists Length + ContinuationState Length 
                         * * results+2: points AttributeLists
                         */
                        U16 attributelists_bytecount = BEtoHost16(Event->p.token->results);
                        U16 start_pos = 0;

                        /* Report(("query_sa_raw_cb:attributelists count=%d,contStateLen=%d",attributelists_bytecount,Event->p.token->contStateLen)); */
                        bt_trace(
                            TRACE_GROUP_1,
                            BT_LOG_QUERY_SA_RAW_CB_ATTRLIST_COUNT_CONTSTATE_LEN_CASE,
                            attributelists_bytecount,
                            Event->p.token->contStateLen,
                            2);
                        Event->p.token->contStateLen = Event->p.token->rLen - 2 - attributelists_bytecount;
                        /* Report(("query_sa_raw_cb:ContStateLen from ssa rsp =%d",Event->p.token->contStateLen)); */
                        bt_trace(
                            TRACE_GROUP_1,
                            BT_LOG_QUERY_SA_RAW_CB_CONSTATE_LEN_FROM_SA_RSP_CASE,
                            Event->p.token->contStateLen,
                            2);
                        /* 2bytes (AttributeListsByteCount) + attributelists_bytecount + 1 byte (ContinuationState) */
                        start_pos = 2 + attributelists_bytecount + 1;
                        check_continute_query(Event->p.token, start_pos);
                        BTBMAdpSearchAttributeRawResult(
                            Event->p.token->rLen - Event->p.token->contStateLen - 2,
                            Event->p.token->results + 2,
                            bd_addr);
                    }
                        break;
                }
            }
            if ((Event->p.token->contStateLen == 1) && (Event->p.token->contState[0] == 0x00))
            {
                SDAP(progress) = SDAP_PROGRESS_NONE;
                SdapRemoveLinkCallback(Event->p.token->rm);
                BTBMAdpSearchAttributeRawComplete(BTBM_ADP_SUCCESS, bd_addr);
            }
            else
            {
                BtStatus status = 0;
                BtRemoteDevice *remDev;
                int i = 0;

                Assert(SDAP(bdc)->link != 0x00);
                remDev = SDAP(bdc)->link;
                bt_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_TRY_CONTINUE_SDP_TO_REMDEV, remDev);
                bt_trace(TRACE_GROUP_1, CONTINUATION_STATE);
                memset((U8*) SDAP(jsr82_sdp_client_contState_Data), 0, 17);
                sdap_util_setup_query_token(
                    &(SDAP(local_sqt)),
                    SdapServiceSearchRawPattern,
                    SDAP(search_pattern_size),
                    SDAP(search_type),
                    query_sdap_service_attribute_raw_callback,
                    remDev);

                SDAP(local_sqt).mode = BSPM_CONT_STATE;
                SDAP(jsr82_sdp_client_contState_Len) = Event->p.token->contStateLen;
                memcpy(
                    SDAP(jsr82_sdp_client_contState_Data),
                    Event->p.token->parseData.storedBuff,
                    Event->p.token->contStateLen);

                SDAP(local_sqt).contStateLen = SDAP(jsr82_sdp_client_contState_Len);
                SDAP(local_sqt).contState = SDAP(jsr82_sdp_client_contState_Data);

                bt_trace(
                    BT_TRACE_JSR82_GROUP,
                    BT_JSR82_TRY_CONTINUE_LEN_CONTSTATE_PTR,
                    SDAP(local_sqt).contStateLen,
                    SDAP(local_sqt).contState);
                for (i = 0; i < SDAP(local_sqt).contStateLen; i++)
                {
                    bt_trace(
                        BT_TRACE_JSR82_GROUP,
                        BT_JSR82_CHECK_DUMP_CONSTATE_DATA_INX,
                        i,
                        SDAP(local_sqt).contState[i]);
                }

                if ((status = SDP_Query(&SDAP(local_sqt), BSQM_CONTINUE)) != BT_STATUS_PENDING)
                {
                    /* Report(("query_sa_raw_cb:Continue sdp query failed, stauts=%d",status)); */
                    bt_trace(TRACE_GROUP_1, BT_LOG_QUERY_SA_RAW_CB_CONTINUE_SDP_FAILED_STATUS, status);
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    SdapRemoveLinkCallback(Event->p.token->rm);
                    BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, bd_addr);
                }
            }
            break;
        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            if (Event->p.token->rm != 0)
            {
                Event->p.token->rm->discFlag = TRUE;
            }
            SDAP(progress) = SDAP_PROGRESS_NONE;
            bt_trace(TRACE_GROUP_1, PARSE_COMPLETE_WITH_ERROR);

            SdapRemoveLinkCallback(Event->p.token->rm);
            BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, bd_addr);
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            if (Event->errCode == BEC_NO_ERROR)
            {
                remDev = Event->p.remDev;
                bdc = DS_FindDevice(&(remDev->bdAddr));
                if (bdc != 0)
                {
                    bdc->link = remDev;
                }
                if (bdc == SDAP(bdc))
                {
                    status = sdap_service_search_attribute_raw_request(BSQM_FIRST);
                    if (status != BT_STATUS_PENDING)
                    {
                        SDAP(progress) = SDAP_PROGRESS_NONE;
                        /* To Disconnect Link */
                        bdc->link->discFlag = TRUE;
                        /* sdapMeHandler will be removed from remDev handlerList in ME_ForceDisconnectLinkWithReason() */
                        ME_ForceDisconnectLinkWithReason(&sdapMeHandler, bdc->link, BEC_USER_TERMINATED, TRUE);
                        /* If the link is disconencted, the MeReportLinkUp()->CallAndRemoveHandlers() -> remDev->state = BDS_DISCONNECTED and remove handler */
                        if (bdc->link->state != BDS_DISCONNECTED)
                        {
                            SdapRemoveLinkCallback(bdc->link);
                        }

                        BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, bd_addr);
                    }
                }
            }
            else
            {
                if (Event->p.token->rm != 0)
                {
                    Event->p.token->rm->discFlag = TRUE;
                }
                SDAP(progress) = SDAP_PROGRESS_NONE;
                remDev = Event->p.remDev;
                /* If the link is disconencted, the MeReportLinkUp()->CallAndRemoveHandlers() -> remDev->state = BDS_DISCONNECTED and remove handler */
                /* Report(("query_ssa_raw_cb:Link connect cnf with error")); */
                bt_trace(TRACE_GROUP_1, BT_LOG_QUERY_SA_RAW_CB_LINK_CON_CNF_FAILED);
                SdapRemoveLinkCallback(remDev);
                BTBMAdpSearchAttributeRawComplete(BTBM_ADP_FAILED, bd_addr);
            }
            break;
    }
}
#endif /* BT_JSR82_ENABLED */ 


/*****************************************************************************
 * FUNCTION
 *  query_sdap_search_attribute_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  Event       [IN]        
 *  event  : event of result
 * RETURNS
 *  void
 * LOCAL AFFECTED
 *  
 *****************************************************************************/
static void query_sdap_search_attribute_callback(const BtEvent *Event)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtRemoteDevice *remDev;
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t bd_addr;

    CONVERT_ARRAY2BDADDR(&bd_addr, SDAP(bdc)->addr.addr);

    OS_Report("query_sdap_search_attribute_callback event %d", Event->eType);
    switch (Event->eType)
    {
#ifdef __BT_GOEP_V2__
        case SDEVENT_QUERY_RSP:
            Event->p.token->attribId = SDAP(attribute_id)[0];
            if (SDAP(sdap_uuid_len) == 16)  /* spp-based app: get channel */
            {
                Event->p.token->uuid = PROT_RFCOMM;
                OS_Report("Get RFCOMM channel for 128bit UUID service");
            }
            else
            {
                Event->p.token->uuid = (U16) SDAP(sdap_uuid[0]);
                OS_Report("Event->p.token->uuid=0x%x", Event->p.token->uuid);
            }
        
            switch(Event->p.token->uuid)
            {
            case SC_DIALUP_NETWORKING:
            case SC_SERIAL_PORT:
            case SC_OBEX_OBJECT_PUSH:
            case SC_OBEX_FILE_TRANSFER:
            case SC_HANDSFREE:
            case SC_HANDSFREE_AUDIO_GATEWAY:
            case SC_HEADSET:
            case SC_HEADSET_AUDIO_GATEWAY:
            case SC_IMAGING_RESPONDER:
            case SC_DIRECT_PRINTING:
			case SC_PBAP_SERVER:
                bt_trace(TRACE_GROUP_1, IN_QUERY_SDAP_SEARCH_ATTRIBUTE_CALLBACK_RECV_SPP_QUERY_RSP);
                /* Change uuid for doing attribute parsing to obtain the server channel number */
                Event->p.token->uuid = PROT_RFCOMM;
                break;
            default:
                break;
            }
#else
        case SDEVENT_QUERY_RSP:
            Event->p.token->attribId = SDAP(attribute_id)[0];
            if (SDAP(sdap_uuid_len) == 16)  /* spp-based app: get channel */
            {
                Event->p.token->uuid = PROT_RFCOMM;
                OS_Report("Get RFCOMM channel for 128bit UUID service");
            }
            else
            {
                Event->p.token->uuid = (U16) SDAP(sdap_uuid[0]);
                OS_Report("Event->p.token->uuid=0x%x", Event->p.token->uuid);
            }

            switch(Event->p.token->uuid)
            {
        #if DUN_CLIENT == XA_ENABLED
            case SC_DIALUP_NETWORKING:
        #endif
            case SC_SERIAL_PORT:
            case SC_OBEX_OBJECT_PUSH:
            case SC_OBEX_FILE_TRANSFER:
            case SC_HANDSFREE:
            case SC_HANDSFREE_AUDIO_GATEWAY:
            case SC_HEADSET:
            case SC_HEADSET_AUDIO_GATEWAY:
            case SC_IMAGING_RESPONDER:
            case SC_DIRECT_PRINTING:
			case SC_PBAP_SERVER:
                bt_trace(TRACE_GROUP_1, IN_QUERY_SDAP_SEARCH_ATTRIBUTE_CALLBACK_RECV_SPP_QUERY_RSP);
                /* Change uuid for doing attribute parsing to obtain the server channel number */
                Event->p.token->uuid = PROT_RFCOMM;
                break;
            default:
                break;
            }
#endif

          contiue_parsing:
            status = SDP_ParseAttributes(Event->p.token);
#if 0
            if(Event->p.token->remainLen > 10)
            {
                Report(("Try to parse the second time"));
                Event->p.token->mode = BSPM_RESUME;
                SDP_ParseAttributes(Event->p.token);            

            }
#endif  

            if (status == BT_STATUS_SUCCESS)
            {
                /* Report to upper layer */
                bt_trace(TRACE_GROUP_1, FOUND_SERVICE_RECORD_OF_BT_DEBUGx04X, SDAP(sdap_uuid)[SDAP(sdap_len)]);

#if defined(BTMTK_ON_LINUX)  /* JNI is blocking call to get the result */
                SDAP(progress) = SDAP_PROGRESS_NONE;
                SdapRemoveLinkCallback(Event->p.token->rm);
                OS_Report("availValueLen=%d, storedLen=%d",Event->p.token->availValueLen, Event->p.token->parseData.storedLen);
                sdapSendAttributeSearchCompleted(BTBM_ADP_SUCCESS, bd_addr, Event->p.token->parseData.storedLen, Event->p.token->valueBuff);
#else
                if (Event->p.token->parseData.flags == READY_RESUME)
                {
                    bt_trace(TRACE_GROUP_1, CONTINUE_TO_PARSING_SDP_RSP_RESULT);
                    sdapSendAttributeSearchResult(Event->p.token->availValueLen, Event->p.token->valueBuff, bd_addr);
                    /* Modify the token->mode to BSPM_RESUME for parsing from correct position: 2007-0401 */
                    Event->p.token->mode = BSPM_RESUME;
                    goto contiue_parsing;
                }
                else if (Event->p.token->parseData.flags == READY_PARSE)
                {
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    SdapRemoveLinkCallback(Event->p.token->rm);
                    sdapSendAttributeSearchResult(Event->p.token->availValueLen, Event->p.token->valueBuff, bd_addr);
                    sdapSendAttributeSearchCompleted(BTBM_ADP_SUCCESS, bd_addr);
                }
#endif
                break;
            }
            else if (status == BT_STATUS_SDP_CONT_STATE)
            {
#if defined(BTMTK_ON_LINUX)
                sdapSendAttributeSearchCompleted(BTBM_ADP_SUCCESS, bd_addr, Event->p.token->availValueLen, Event->p.token->valueBuff);
#else
                /* We are going to query again for the rest of the response */
                sdapSendAttributeSearchResult(Event->p.token->availValueLen, Event->p.token->valueBuff, bd_addr);
                status = sdap_service_search_attribute_request(SDAP(sdap_uuid_len), SDAP(sdap_uuid_128), (U16) (SDAP(sdap_uuid))[0], SDAP(attribute_id[0]), BSQM_CONTINUE);
                if (status != BT_STATUS_PENDING)
                {
                    SDAP(progress) = SDAP_PROGRESS_NONE;
                    SdapRemoveLinkCallback(Event->p.token->rm);
                    sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr);
                }
#endif
                break;
            }
            else
            {
                SdapRemoveLinkCallback(Event->p.token->rm);
#if defined(BTMTK_ON_LINUX)
                sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr, 0, NULL);
#else
                sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr);
#endif
                SDAP(progress) = SDAP_PROGRESS_NONE;
            }
            break;
        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            SDAP(progress) = SDAP_PROGRESS_NONE;
            SdapRemoveLinkCallback(Event->p.token->rm);
#if defined(BTMTK_ON_LINUX)
            sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr, 0, NULL);
#else
            sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr);
#endif
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            if (Event->errCode == BEC_NO_ERROR)
            {
                remDev = Event->p.remDev;
                bdc = DS_FindDevice(&(remDev->bdAddr));
                if (bdc != 0)
                {
                    bdc->link = remDev;
                }
                if (bdc == SDAP(bdc))
                {
                    status = sdap_service_search_attribute_request(SDAP(sdap_uuid_len), SDAP(sdap_uuid_128), (U16)(SDAP(sdap_uuid))[0], SDAP(attribute_id[0]), BSQM_FIRST);
                    if (status != BT_STATUS_PENDING)
                    {
                        SDAP(progress) = SDAP_PROGRESS_NONE;
                    }
                    else
                    {
                        btbmSdpSearchAttributePendingInquiry(1);
                    }

                }
            }
            else
            {
                SDAP(progress) = SDAP_PROGRESS_NONE;
#if defined(BTMTK_ON_LINUX)
                sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr, 0, NULL);
#else
                sdapSendAttributeSearchCompleted(BTBM_ADP_FAILED, bd_addr);
#endif
                btbmSdpSearchAttributePendingInquiry(0);

            }
            break;

    }
}


/*****************************************************************************
 * FUNCTION
 *  sdap_util_allocate_free_devices
 * DESCRIPTION
 *  The function is used to allocate a new free devices if the bdaddr does not exist before
 * PARAMETERS
 *  bdAddr      [IN]        The bdaddress of the new device
 * RETURNS
 *  Status of add result
 * LOCAL AFFECTED
 *  
 *****************************************************************************/
static U8 sdap_util_allocate_free_devices(BD_ADDR bdAddr)
{
    BtDeviceContext device, *currDevice;

    OS_MemSet((U8*) & device, 0, (U32) sizeof(BtDeviceContext));
    device.addr = bdAddr;

    if (DS_AddDevice(&device, &currDevice) != BT_STATUS_SUCCESS)
    {
        return BT_STATUS_FAILED;
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }
}


/*****************************************************************************
 * FUNCTION
 *  sdap_util_setup_query_token
 * DESCRIPTION
 *  
 * PARAMETERS
 *  cur_sqt         [IN]        
 *  parms           [IN]        
 *  plen            [IN]        
 *  type            [IN]        
 *  callback        [IN]        
 *  rm              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void sdap_util_setup_query_token(
                SdpQueryToken *cur_sqt,
                const U8 *parms,
                U16 plen,
                SdpQueryType type,
                BtCallBack callback,
                BtRemoteDevice *rm)
{
    cur_sqt->parms = parms;
    cur_sqt->plen = plen;
    cur_sqt->type = type;
    cur_sqt->callback = callback;
    cur_sqt->rm = rm;
    cur_sqt->mode = BSPM_BEGINNING;
    return;
}


/*****************************************************************************
 * FUNCTION
 *  sdap_util_create_acl_link
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bdc     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus sdap_util_create_acl_link(BtDeviceContext *bdc)
{
    BtStatus status;

    status = ME_CreateLink(&sdapMeHandler, &(bdc->addr), &(bdc->psi), &(bdc->link));
    Report(("sdap_util_create_acl_link status=%d", status));
    switch (status)
    {
        case BT_STATUS_PENDING:
        case BT_STATUS_IN_PROGRESS:
            status = BT_STATUS_PENDING;
            break;
        case BT_STATUS_SUCCESS:
            status = BT_STATUS_SUCCESS;
            break;
        default:
            status = BT_STATUS_FAILED;
            break;
    }
    return status;
}


/*****************************************************************************
 * FUNCTION
 *  sdap_restart_multiple_req
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void sdap_restart_multiple_req(BtRemoteDevice *remDev)
{
    BtRemoteDevice *next_remDev;
    BtStatus status;
    SdpQueryToken *token = 0;

    bt_trace(TRACE_GROUP_1, RESTART_SDAP_MULTIPLE_REQ_TIMER_B);
    next_remDev = (BtRemoteDevice*) (SDAP(sdap_multi_req[SDAP(sdap_current_multi_req_index) - 1])).rm;
    EvmResetTimer(&SDAP(sdap_multi_req_timer));
    if (next_remDev != remDev)
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, 0x21);
        SDAP(sdap_multi_req_timer).func = sdap_mutiple_req_timeout;
        EVM_StartTimer(&SDAP(sdap_multi_req_timer), 1);
    }
    else
    {
        bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, 0x22);
        token = (SdpQueryToken*) (SDAP(sdap_multi_req[SDAP(sdap_current_multi_req_index) - 1])).sqt;
        status = SDAP_ServiceSearchMultipleAttribute(SDAP(sdap_multi_req[SDAP(sdap_current_multi_req_index) - 1]));
        if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING))
        {
            SDAP(multicallback) (token, BT_STATUS_FAILED, SDAP(current_attribute_index), 0x00);
            if (SDAP(sdap_multi_req_index) == (SDAP(sdap_current_multi_req_index)))
            {
                SDAP(sdap_current_multi_req_index) = 0;
                SDAP(sdap_multi_req_index) = 0;
                SDAP(current_attribute_index) = 0;
                SDAP(progress) = SDAP_PROGRESS_NONE;
            }
            else
            {
                SDAP(sdap_multi_req_timer).func = sdap_mutiple_req_timeout;
                EVM_StartTimer(&SDAP(sdap_multi_req_timer), 1);
            }

        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  sdap_mutiple_req_timeout
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Timer       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void sdap_mutiple_req_timeout(EvmTimer *Timer)
{
    BtStatus status;
    SdpQueryToken *token = 0;

    bt_trace(TRACE_GROUP_1, SDAP_MULTIPLE_REQUEST_TIMEOUT_A);
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(sdap_current_multi_req_index));
    bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTSDAPPROGRESSxD, SDAP(sdap_multi_req_index));

    SDAP(sdap_multi_req_timer).func = 0;
    token = (SdpQueryToken*) (SDAP(sdap_multi_req[SDAP(sdap_current_multi_req_index) - 1])).sqt;
    status = SDAP_ServiceSearchMultipleAttribute(SDAP(sdap_multi_req[SDAP(sdap_current_multi_req_index) - 1]));
    if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_PENDING))
    {
        SDAP(multicallback) (token, BT_STATUS_FAILED, SDAP(current_attribute_index), 0x00);
        if (SDAP(sdap_multi_req_index) == (SDAP(sdap_current_multi_req_index)))
        {
            SDAP(sdap_current_multi_req_index) = 0;
            SDAP(sdap_multi_req_index) = 0;
            SDAP(current_attribute_index) = 0;
            SDAP(progress) = SDAP_PROGRESS_NONE;
        }
        else
        {
            SDAP(sdap_multi_req_timer).func = sdap_mutiple_req_timeout;
            EVM_StartTimer(&SDAP(sdap_multi_req_timer), 1);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  sdap_process_next_reqeuest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  remDev      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void sdap_process_next_request(BtRemoteDevice *remDev) {

#ifdef __SDAP_REQUEST_QUEUE__
    BtRemoteDevice *next_remDev;
    BtStatus status = BT_STATUS_SUCCESS;

	OS_Report("[SDAP] %s()", __FUNCTION__);
	SdapQueuedRequest *req = sdapDequeueRequestSameAddr(&remDev->bdAddr);
	if (req == NULL) {
		OS_Report("[SDAP] No SDAP request for the same device.");
		req = sdapDequeueRequestFromHead();
	}

	SDAP(progress) = SDAP_PROGRESS_NONE;
	if (req != NULL) {
		sdapReturnRequestStruct(req);
		switch (req->req_type) {
			case SDAP_REQUEST_ALL:
				SDAP_ServiceSearchAll(req->params.all);
				break;

#ifdef __SDAP_SERVICE_SEARCH_ALL_2__
			case SDAP_REQUEST_ALL_2:
				SDAP_ServiceSearchAll_2(req->bdAddr);
				break;
#endif

			case SDAP_REQUEST_MULTIPLE_ATTRIBUTE:
				status = SDAP_ServiceSearchMultipleAttribute(req->params.multi_attr);
				break;
			case SDAP_REQUEST_MULTIPLE_ATTRIBUTE_2:
				status = SDAP_ServiceSearchMultipleAttribute2(req->params.multi_attr);
				break;
			default:
				break;
		}

		OS_Report("[SDAP] Request Type: %d, status: %d");
		if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
			switch (req->req_type) {
				case SDAP_REQUEST_MULTIPLE_ATTRIBUTE:
				case SDAP_REQUEST_MULTIPLE_ATTRIBUTE_2:
					req->params.multi_attr.callback(req->params.multi_attr.sqt, BT_STATUS_FAILED, 0, 0x00);
					break;
				default:
					break;
			}
		}
		
	} else {
		OS_Report("[SDAP] No any queued sdap request.");
	}

#else
	SDAP(progress) = SDAP_PROGRESS_NONE;
	if (SDAP(sdap_multi_req_index) != (SDAP(sdap_current_multi_req_index)))
	{
		bt_trace(TRACE_GROUP_1, SDAP_MULTIPLE_REQUEST_TIMEOUT_C);
		sdap_restart_multiple_req(remDev);
	}
	else
	{
		SDAP(sdap_current_multi_req_index) = 0;
		SDAP(sdap_multi_req_index) = 0;
	}

#endif

}


void SDAP_Register(BMCallback Callback)
{
    SDAP(bmCB) = Callback;
}


static void sdapSendServiceSearchCompleted(U8 result, btbm_bd_addr_t addr, U8 no)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_SERVICE_SEARCH_COMPLETED;
    parms.p.ReportServiceSearchComplete.result = result;
    parms.p.ReportServiceSearchComplete.bd_addr = addr;
    parms.p.ReportServiceSearchComplete.no = no;
    SDAP(bmCB)(&parms);  
}


static void sdapSendServiceSearchCancelled(U8 result, btbm_bd_addr_t addr, U8 no)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_SERVICE_SEARCH_CANCELLED;
    parms.p.ReportServiceSearchComplete.result = result;
    parms.p.ReportServiceSearchComplete.bd_addr = addr;
    parms.p.ReportServiceSearchComplete.no = no;
    SDAP(bmCB)(&parms);  
}


static void sdapSendAttributeSearchResult(U16 len, U8 *data, btbm_bd_addr_t addr)
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_ATTRIBUTE_SEARCH_RESULT;
    parms.p.ReportAttributeSearchResult.len = len;
    parms.p.ReportAttributeSearchResult.data = data;
    parms.p.ReportAttributeSearchResult.bd_addr = addr;
    SDAP(bmCB)(&parms);  
}

#ifdef __SDAP_REQUEST_QUEUE__
static void sdapInitRequestFreeList()
{
	int i;
	InitializeListHead(&sdapRequestFreeList);

	for (i = 0; i < SDAP_MAX_MULTIPLE_REQ - 1; i++) {
		InsertTailList(&sdapRequestFreeList, &(_sdapRequestStructPool[i].node));
	}
}

static SdapQueuedRequest *sdapGetAvailableRequestStruct()
{
	SdapQueuedRequest *ret = NULL;
	
	if (!IsListEmpty(&sdapRequestFreeList)) {
		ret = (SdapQueuedRequest *) RemoveHeadList(&sdapRequestFreeList);
		btmtk_os_memset((U8 *) ret, 0, sizeof(SdapQueuedRequest));
	}

	if (ret == NULL) {
		OS_Report("[SDAP] No available sdap request struct.");
	}
	return ret;
}

static void sdapReturnRequestStruct(SdapQueuedRequest *sdap_req)
{
	InsertTailList(&sdapRequestFreeList, &sdap_req->node);
}

static void sdapQueuedRequestTimeout(EvmTimer *timer)
{
	SdapQueuedRequest *req = (SdapQueuedRequest *) timer->context;
    btbm_bd_addr_t bd_addr;

	OS_Report("[SDAP] %s(), req_type: %d", __FUNCTION__, req->req_type);
	switch (req->req_type) {
		case SDAP_REQUEST_ALL:
			CONVERT_ARRAY2BDADDR(&bd_addr, req->params.all.bdAddr->addr);
			sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, 0);
			break;

		case SDAP_REQUEST_MULTIPLE_ATTRIBUTE:
			req->params.multi_attr.callback(req->params.multi_attr.sqt, BT_STATUS_TIMEOUT, 0, 0x00);
			break;

		default:
			break;
	}

	sdapRemoveRequestFromQueue(req);
	sdapReturnRequestStruct(req);
}

static BtStatus sdapEnqueueRequestAll(sdap_service_search_all_struct search_pattern)
{
	SdapQueuedRequest *req = sdapGetAvailableRequestStruct();

	OS_Report("[SDAP] %s()", __FUNCTION__);
	if (req != NULL) {
		req->req_type = SDAP_REQUEST_ALL;

		OS_MemCopy(req->bdAddr.addr, search_pattern.bdAddr->addr, 6);
		req->params.all.bdAddr = &req->bdAddr;
		req->params.all.sdap_len = search_pattern.sdap_len;
		req->params.all.sdp_uuid = search_pattern.sdp_uuid;
		req->req_timer.func = sdapQueuedRequestTimeout;
		req->req_timer.context = req;
		EVM_StartTimer(&req->req_timer, SDAP_QUEUED_REQUEST_TIMEOUT);
		InsertTailList(&SDAP(sdap_request_queue), &req->node);
		return BT_STATUS_SUCCESS;
	}
	return BT_STATUS_FAILED;
}

static BtStatus sdapEnqueueRequestAll2(const BD_ADDR bdAddr)
{
	SdapQueuedRequest *req = sdapGetAvailableRequestStruct();

	OS_Report("[SDAP] %s()", __FUNCTION__);
	if (req != NULL) {
		req->req_type = SDAP_REQUEST_ALL_2;

		OS_MemCopy(req->bdAddr.addr, bdAddr.addr, 6);
		req->req_timer.func = sdapQueuedRequestTimeout;
		req->req_timer.context = req;
		EVM_StartTimer(&req->req_timer, SDAP_QUEUED_REQUEST_TIMEOUT);
		InsertTailList(&SDAP(sdap_request_queue), &req->node);
		return BT_STATUS_SUCCESS;
	}
	return BT_STATUS_FAILED;
}

static BtStatus sdapEnqueueRequestMultipleAttribute(sdap_service_search_multi_attribute_struct search_pattern)
{
	SdapQueuedRequest *req = sdapGetAvailableRequestStruct();

	OS_Report("[SDAP] %s()", __FUNCTION__);
	if (req != NULL) {
		req->req_type = SDAP_REQUEST_MULTIPLE_ATTRIBUTE;

		OS_MemCopy(req->bdAddr.addr, search_pattern.rm->bdAddr.addr, 6);
		req->params.multi_attr.sqt = search_pattern.sqt;
		req->params.multi_attr.sqt->rm = search_pattern.rm;
		req->params.multi_attr.rm = search_pattern.rm;
		req->params.multi_attr.uuid = search_pattern.uuid;
		req->params.multi_attr.callback = search_pattern.callback;
		req->params.multi_attr.attribute_num = search_pattern.attribute_num;
		req->params.multi_attr.attribute_id = (search_pattern.attribute_id);
		req->req_timer.func = sdapQueuedRequestTimeout;
		req->req_timer.context = req;
		EVM_StartTimer(&req->req_timer, SDAP_QUEUED_REQUEST_TIMEOUT);
		InsertTailList(&SDAP(sdap_request_queue), &req->node);
		return BT_STATUS_SUCCESS;
	}
	return BT_STATUS_FAILED;
}

static BtStatus sdapEnqueueRequestMultipleAttribute2(sdap_service_search_multi_attribute_struct search_pattern)
{
	SdapQueuedRequest *req = sdapGetAvailableRequestStruct();

	OS_Report("[SDAP] %s()", __FUNCTION__);
	if (req != NULL) {
		req->req_type = SDAP_REQUEST_MULTIPLE_ATTRIBUTE_2;

		OS_MemCopy(req->bdAddr.addr, search_pattern.rm->bdAddr.addr, 6);
		req->params.multi_attr.sqt = search_pattern.sqt;
		req->params.multi_attr.sqt->rm = search_pattern.rm;
		req->params.multi_attr.rm = search_pattern.rm;
		req->params.multi_attr.uuid = search_pattern.uuid;
		req->params.multi_attr.callback = search_pattern.callback;
		req->params.multi_attr.attribute_num = search_pattern.attribute_num;
		req->params.multi_attr.attribute_id = (search_pattern.attribute_id);
		req->req_timer.func = sdapQueuedRequestTimeout;
		req->req_timer.context = req;
		EVM_StartTimer(&req->req_timer, SDAP_QUEUED_REQUEST_TIMEOUT);
		InsertTailList(&SDAP(sdap_request_queue), &req->node);
		return BT_STATUS_SUCCESS;
	}
	return BT_STATUS_FAILED;

}

static SdapQueuedRequest *sdapDequeueRequestFromHead()
{
	SdapQueuedRequest *ret = NULL;
	
	if (!IsListEmpty(&SDAP(sdap_request_queue))) {
		ret = (SdapQueuedRequest *) RemoveHeadList(&SDAP(sdap_request_queue));
		OS_Report("[SDAP] Get queued sdap request, type: %d", ret->req_type);

		EVM_ResetTimer(&ret->req_timer);
	}
	return ret;
}

static SdapQueuedRequest *sdapDequeueRequestSameAddr(BD_ADDR *bdaddr)
{
	SdapQueuedRequest *tmp_req;

	for (tmp_req = (SdapQueuedRequest *) SDAP(sdap_request_queue).Flink;
			(ListEntry *) tmp_req != &SDAP(sdap_request_queue);
			tmp_req = (SdapQueuedRequest *) tmp_req->node.Flink) {

		if (OS_MemCmp(bdaddr->addr, 6, tmp_req->bdAddr.addr, 6) == TRUE) {		
			tmp_req->node.Flink->Blink = tmp_req->node.Blink;
			tmp_req->node.Blink->Flink = tmp_req->node.Flink;

			// Request node found, reset its timer.
			EVM_ResetTimer(&tmp_req->req_timer);
			return tmp_req;
		}
	}
	return NULL;
}

static void sdapRemoveRequestFromQueue(SdapQueuedRequest *req)
{
	SdapQueuedRequest *tmp_req;

	for (tmp_req = (SdapQueuedRequest *) SDAP(sdap_request_queue).Flink;
			(ListEntry *) tmp_req != &SDAP(sdap_request_queue);
			tmp_req = (SdapQueuedRequest *) tmp_req->node.Flink) {
		if (tmp_req == req) {
			tmp_req->node.Flink->Blink = tmp_req->node.Blink;
			tmp_req->node.Blink->Flink = tmp_req->node.Flink;
			return;
		}
	}
	OS_Report("[SDAP] Sdap failed to remove the request from queue");
}

static void sdapClearRequestSameAddr(BD_ADDR *bdaddr)
{
	btbm_bd_addr_t bd_addr_t;
	SdapQueuedRequest *cur_req, *next_req = NULL;

	OS_Report("[SDAP] %s()", __FUNCTION__);
	for (cur_req = (SdapQueuedRequest *) SDAP(sdap_request_queue).Flink;
			(ListEntry *) cur_req != &SDAP(sdap_request_queue);
			cur_req = next_req) {

        next_req = (SdapQueuedRequest *) cur_req->node.Flink;

		if (OS_MemCmp(bdaddr->addr, 6, cur_req->bdAddr.addr, 6) == TRUE) {		
			cur_req->node.Flink->Blink = cur_req->node.Blink;
			cur_req->node.Blink->Flink = cur_req->node.Flink;

			// Request node found, reset its timer.
			EVM_ResetTimer(&cur_req->req_timer);

			switch (cur_req->req_type) {
				case SDAP_REQUEST_ALL:
					CONVERT_ARRAY2BDADDR(&bd_addr_t, bdaddr->addr);
					sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr_t, 0);
					break;

				case SDAP_REQUEST_MULTIPLE_ATTRIBUTE:
					cur_req->params.multi_attr.callback(
							cur_req->params.multi_attr.sqt,
							BT_STATUS_FAILED, 0, 0x00);
					break;

				default:
					break;
			}
			sdapReturnRequestStruct(cur_req);
		}
	}
}

#endif

#ifdef __SDAP_SERVICE_SEARCH_ALL_2__
/*****************************************************************************
 * FUNCTION
 *  query_sdap_service_search_all_callback
 * DESCRIPTION
 *  The function is the callback register to SDP layer.
 *  The SDP will callback the function when the SDP results.
 * PARAMETERS
 *  Event       [IN]        
 *  event  : event of result
 * RETURNS
 *  void
 *****************************************************************************/
static void query_sdap_service_search_all_2_callback(const BtEvent *Event)
{
    BtStatus status = BT_STATUS_SUCCESS;
    BtRemoteDevice *remDev;
    BtDeviceContext *bdc = 0;
    btbm_bd_addr_t bd_addr;
    SdpQueryToken *token;
    BMCallbackParms parms;
	U16 offset = 0, uuid = 0;
	U32 len = 0;
    
    CONVERT_ARRAY2BDADDR(&bd_addr, SDAP(bdc)->addr.addr);
    switch (Event->eType)
    {
        case SDEVENT_QUERY_RSP:
			OS_Report("[SDAP] %s(): SDEVENT_QUERY_RSP %02x %02x %02x %02x %02x %02x", __FUNCTION__,
					Event->p.token->results[0],
					Event->p.token->results[1],
					Event->p.token->results[2],
					Event->p.token->results[3],
					Event->p.token->results[4],
					Event->p.token->results[5]);

			Event->p.token->attribId = AID_SERVICE_CLASS_ID_LIST;

			if (Event->p.token->mode == BSQM_CONTINUE) {
				Event->p.token->mode = BSPM_CONT_STATE;
			}

			status = SDP_ParseAttributes(Event->p.token);
			OS_Report("[SDAP] SDP_ParseAttributes() returns %d", status);

			while (status == BT_STATUS_SUCCESS) {
				OS_Report("[SDAP] valueBuffer: %02x %02x %02x. len: %d",
						Event->p.token->valueBuff[0],
						Event->p.token->valueBuff[1],
						Event->p.token->valueBuff[2],
						Event->p.token->availValueLen);
				
				len = SdpParseElement(Event->p.token->valueBuff, &offset);
				OS_Report("[SDAP] SdpParseElement() offset: %d, len: %d", offset, len);

				if (len == 2) {
					uuid = SDP_GetU16((Event->p.token->valueBuff + offset));
					OS_Report("[SDAP] Parsed UUID: %4x", uuid);

					parms.event = BTBM_EVENT_SERVICE_SEARCH_RESULT;
					parms.p.ReportServiceSearchResult.uuid = uuid;
					parms.p.ReportServiceSearchResult.bd_addr = bd_addr;
					SDAP(bmCB)(&parms);
					SDAP(searched_uuid)++;
				}

				Event->p.token->mode = BSPM_RESUME;
				status = SDP_ParseAttributes(Event->p.token);
			}

			switch (status) {
				case BT_STATUS_SDP_CONT_STATE:
					OS_Report("[SDAP] Continuation state met.");

					status = sdap_service_search_request_all_2(BSQM_CONTINUE);

					if (status != BT_STATUS_PENDING) {
						SDAP(progress) = SDAP_PROGRESS_NONE;
						if (Event->p.token->rm != 0) {
							Event->p.token->rm->discFlag = TRUE;
							Event->p.token->rm->discTimeoutLevel = BT_L2CAP_DISCONNECT_TIMEOUT_50S;
							bt_trace(TRACE_GROUP_1,	IN_QUERY_SDAP_SERVICE_SEARCH_ALL_CALLBACK_SET_DISCLEVEL_BT_L2CAP_DISCONNECT_TIMEOUT_50S);
						}

						SdapRemoveLinkCallback(Event->p.token->rm);
						sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, SDAP(searched_uuid));

						sdap_process_next_request(Event->p.token->rm);
					}
					break;

				case BT_STATUS_FAILED:
					OS_Report("[SDAP] No more Service Class ID List.");
				default:				
					if (Event->p.token->rm != 0)
					{
						Event->p.token->rm->discFlag = TRUE;
					}
					SdapRemoveLinkCallback(Event->p.token->rm);
					sdapSendServiceSearchCompleted(BTBM_ADP_SUCCESS, bd_addr, SDAP(searched_uuid));
					bt_trace(TRACE_GROUP_1, PARSE_COMPLETE);
					
                    sdap_search_pnp_info_request(BSQM_FIRST);
                    // SDAP(progress) = SDAP_PROGRESS_NONE;
					// sdap_process_next_request(Event->p.token->rm);
			}
            break;

        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            if (Event->p.token->rm != 0)
            {
                Event->p.token->rm->discFlag = TRUE;
            }
            SDAP(progress) = SDAP_PROGRESS_NONE;
            bt_trace(TRACE_GROUP_1, PARSE_COMPLETE_WITH_ERROR);
            SdapRemoveLinkCallback(Event->p.token->rm);
            sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, SDAP(searched_uuid));

#ifdef __SDAP_REQUEST_QUEUE__
			if (Event->p.token->rm->state != BDS_CONNECTED) {
				OS_Report("[SDAP] %s(): ACL link is gone.", __FUNCTION__);
				sdapClearRequestSameAddr(&(Event->p.token->rm->bdAddr));
			}
#endif

			sdap_process_next_request(Event->p.token->rm);
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            if (Event->errCode == BEC_NO_ERROR)
            {
                remDev = Event->p.remDev;
                bdc = DS_FindDevice(&(remDev->bdAddr));
                if (bdc != 0)
                {
                    bdc->link = remDev;
                }

                if (SDAP(progress) == SDAP_PROGRESS_NONE)  /* cancelled */
                {
                    bdc->link->discFlag = TRUE;
                    /* sdapMeHandler will be removed from remDev handlerList in ME_ForceDisconnectLinkWithReason() */
                    ME_ForceDisconnectLinkWithReason(&sdapMeHandler, bdc->link, BEC_USER_TERMINATED, TRUE);
                }
                else
                {
					if (bdc == SDAP(bdc))
					{
						status = sdap_service_search_request_all_2(BSQM_FIRST);
						if (status != BT_STATUS_PENDING)
						{
							SDAP(progress) = SDAP_PROGRESS_NONE;
						}
					}
                }
            }
            else
            {
                if (Event->p.token->rm != 0)
                {
                    Event->p.token->rm->discFlag = TRUE;
                }
                SDAP(progress) = SDAP_PROGRESS_NONE;
                sdapSendServiceSearchCompleted(BTBM_ADP_FAILED, bd_addr, SDAP(searched_uuid));
            }
            break;

    }
}


/*****************************************************************************
 * FUNCTION
 *  sdap_service_search_request_all
 * DESCRIPTION
 *  
 * PARAMETERS
 *  uuid        [IN]        
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus sdap_service_search_request_all_2(SdpQueryMode mode)
{
    BtStatus status = BT_STATUS_FAILED;
    BtRemoteDevice *remDev;

    Assert(SDAP(bdc)->link != 0x00);

    remDev = SDAP(bdc)->link;

	OS_Report("[SDAP] %s(): mode %d", __FUNCTION__, mode);

    /* Setup the SDAP token  */
    sdap_util_setup_query_token(
        &(SDAP(local_sqt)),
        SdapServiceSearchAll2Pattern,
        sizeof(SdapServiceSearchAll2Pattern),
        BSQT_SERVICE_SEARCH_ATTRIB_REQ,
        query_sdap_service_search_all_2_callback,
        remDev);

    SDAP(local_sqt).mode = mode;
    status = SDP_Query(&SDAP(local_sqt), mode);

    return status;
}
#endif

/*****************************************************************************
 * FUNCTION
 *  query_sdap_search_pnp_info_attribute_callback
 * DESCRIPTION
 *
 * PARAMETERS
 *  Event       [IN]        
 *  event  : event of result
 * RETURNS
 *  void
 *****************************************************************************/
static void query_sdap_search_pnp_info_attribute_callback(const BtEvent *event)
{
    BtStatus status = BT_STATUS_FAILED;
    SdpQueryToken* token;
    SdapDevDBRecord record;
    
    U32 len = 0;
    U16 att_bytes = 0, offset = 0;
    U16 vendor_id = 0, product_id = 0;
    U8* results;

    token = event->p.token;
    if (token == NULL) {
        OS_Report("[SDAP][ERR] %s(), token is null.", __FUNCTION__);
        return;
    }

    results = event->p.token->results;
    switch (event->eType) {
        case SDEVENT_QUERY_RSP:
            OS_Report("[SDAP] %s(), rLen: %d", __FUNCTION__, token->rLen);
            OS_Report("[SDAP] result dump 0x%02x 0x%02x", results[0], results[1]);

            att_bytes = SDP_GetU16(results);
            len = SdpParseElement(results + 2, &offset);

            // For some IOT device, we need to check if the parameters are consistent.
            if (att_bytes <= offset) {
                OS_Report("[SDAP] %s(), Invalid parameters. att_bytes: %d, offset: %d, len: %d",
                        __FUNCTION__, att_bytes, offset, len);
                goto done;
            }

            if (token->rLen < 5) {
                goto done;
            }

            if (token->mode == BSQM_CONTINUE) {
                token->mode = BSPM_CONT_STATE;
            }

            token->uuid = 0;
            // Get the value of vendor ID.
            token->attribId = AID_PNP_INFO_VENDOR_ID;
            status = SDP_ParseAttributes(token);
            if (status == BT_STATUS_SUCCESS && token->availValueLen == 2) {
                vendor_id = SDP_GetU16(token->valueBuff);
            }

            // Get the value of product ID
            token->attribId = AID_PNP_INFO_PRODUCT_ID;
            status = SDP_ParseAttributes(token);
            if (status == BT_STATUS_SUCCESS && token->availValueLen == 2) {
                product_id = SDP_GetU16(token->valueBuff);
            }

            OS_MemCopy(&(record.bdaddr), &(token->rm->bdAddr), BD_ADDR_SIZE);
            record.vendorID = vendor_id;
            record.productID = product_id;
            status = SDAP_InsertDevDBRecord(&record);
            
            OS_Report("[SDAP] %s(), Vendor ID: 0x%04x, Product ID: 0x%04x, status: %d",
                __FUNCTION__, vendor_id, product_id, status);

            record.vendorID = 0;
            record.productID = 0;
            status = SDAP_QueryDevDBRecord(&record);
            OS_Report("[SDAP] DB Test. Vendor ID: 0x%04x, Product ID: 0x%04x, status: %d",
                record.vendorID, record.productID, status);

            break;

        case SDEVENT_QUERY_ERR:
        case SDEVENT_QUERY_FAILED:
            bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
            OS_Report("[SDAP] %s(), event: %d", __FUNCTION__, event->eType);
            break;
    }

done:

#ifdef __SDAP_REQUEST_QUEUE__
    if (event->p.token->rm->state != BDS_CONNECTED) {
        OS_Report("[SDAP] %s(): ACL link is gone.", __FUNCTION__);
        sdapClearRequestSameAddr(&(event->p.token->rm->bdAddr));
    }
#else
    SDAP(progress) = SDAP_PROGRESS_NONE;
#endif

    sdap_process_next_request(token->rm);
}


/*****************************************************************************
 * FUNCTION
 *  query_sdap_search_pnp_info_handle_callback
 * DESCRIPTION
 *
 * PARAMETERS
 *  Event       [IN]        
 *  event  : event of result
 * RETURNS
 *  void
 *****************************************************************************/
static void query_sdap_search_pnp_info_handle_callback(const BtEvent *event)
{
    BtStatus status = BT_STATUS_FAILED;
    SdpQueryToken* token;
    U16 total_record_count = 0, cur_record_count = 0;
    U8* results;

    token = event->p.token;
    if (token == NULL) {
        OS_Report("[SDAP][ERR] %s(), token is null.", __FUNCTION__);
        return;
    }

    results = token->results;

    switch (event->eType) {
        case SDEVENT_QUERY_RSP:
            OS_Report("[SDAP] %s(), rLen: %d", __FUNCTION__, token->rLen);
            OS_Report("[SDAP] result dump 0x%02x 0x%02x 0x%02x 0x%02x",
                    results[0], results[1], results[2], results[3]);

            if (token->rLen < 5) {
                status = BT_STATUS_NOSERVICES;
                goto report_fail;
            }

            total_record_count = SDP_GetU16(results);
            cur_record_count = SDP_GetU16(results + 2);

            if (total_record_count < 1 || cur_record_count < 1) {
                status = BT_STATUS_NOSERVICES;
                goto report_fail;
            }

            SDAP(record_handle) = SDP_GetU32(results + 4);
            OS_Report("[SDAP] service record handle: 0x%08x", SDAP(record_handle));

            StoreBE32(SdapSearchPnPInfoReq, SDAP(record_handle));
            sdap_util_setup_query_token(
                    token,
                    SdapSearchPnPInfoReq,
                    sizeof(SdapSearchPnPInfoReq),
                    BSQT_ATTRIB_REQ,
                    query_sdap_search_pnp_info_attribute_callback,
                    token->rm);

            status = SDP_Query(token, BSQM_FIRST);
            if (status != BT_STATUS_SUCCESS && status != BT_STATUS_PENDING) {
                OS_Report("[SDAP] %s(), SDP_Query() returns %d.", __FUNCTION__, status);
                status = BT_STATUS_FAILED;
                goto report_fail;
            }
            return;

        case SDEVENT_QUERY_ERR:
            /* CONTINUE, SDAP operation query error */
        case SDEVENT_QUERY_FAILED:
            bt_trace(TRACE_GROUP_1, SERVICE_SEARCH_FAILED);
            OS_Report("[SDAP] %s(), event: %d", __FUNCTION__, event->eType);
            break;
    }

report_fail:

#ifdef __SDAP_REQUEST_QUEUE__
    if (event->p.token->rm->state != BDS_CONNECTED) {
        OS_Report("[SDAP] %s(): ACL link is gone.", __FUNCTION__);
        sdapClearRequestSameAddr(&(event->p.token->rm->bdAddr));
    }
#else
    SDAP(progress) = SDAP_PROGRESS_NONE;
#endif

    sdap_process_next_request(token->rm);
    return;

}

/*****************************************************************************
 * FUNCTION
 *  sdap_search_pnp_info_request
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus sdap_search_pnp_info_request(SdpQueryMode mode)
{
    BtStatus status = BT_STATUS_FAILED;
    BtRemoteDevice *remDev;

    Assert(SDAP(bdc)->link != 0x00);

    remDev = SDAP(bdc)->link;

	OS_Report("[SDAP] %s(): mode %d", __FUNCTION__, mode);

    // Apply PnPInfo UUID to the request.
    StoreBE16(SdapSearchHandleReq + 3, SC_PNP_INFO);

    /* Setup the SDAP token  */
    sdap_util_setup_query_token(
        &(SDAP(local_sqt)),
        SdapSearchHandleReq,
        sizeof(SdapSearchHandleReq),
        BSQT_SERVICE_SEARCH_REQ,
        query_sdap_search_pnp_info_handle_callback,
        remDev);

    SDAP(local_sqt).mode = mode;
    status = SDP_Query(&SDAP(local_sqt), mode);

    return status;

}

#if defined(BTMTK_ON_LINUX)
static void sdapSendAttributeSearchCompleted(U8 result, btbm_bd_addr_t addr, U16 len, U8 *data)
#else
static void sdapSendAttributeSearchCompleted(U8 result, btbm_bd_addr_t addr)
#endif
{
    BMCallbackParms parms;
    
    parms.event = BTBM_EVENT_ATTRIBUTE_SEARCH_COMPLETED;
    parms.p.ReportAttributeSearchComplete.result = result;
    parms.p.ReportAttributeSearchComplete.bd_addr = addr;
#if defined(BTMTK_ON_LINUX)
    parms.p.ReportAttributeSearchComplete.len = len;
    parms.p.ReportAttributeSearchComplete.data = data;
#endif
    SDAP(bmCB)(&parms);  
}



