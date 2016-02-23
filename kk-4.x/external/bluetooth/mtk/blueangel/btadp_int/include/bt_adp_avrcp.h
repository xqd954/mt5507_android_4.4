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
 *	bt_adp_avrcp.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions which provide the service to be the adaption layer 
 *   between Bluetooth and MMI
 *
 * Author:
 * -------
 *	Wilson Lien (mtk00676)
 *      Create 2006/4/17
 *
 *==============================================================================
 * 				HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Dec 26 2007 mtk00676
 * [MAUI_00596125] [AVRCP] Change naming for AVRCP variable
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Jul 17 2006 mtk00676
 * [MAUI_00210994] [AVRCP] Modification for ESI AVRCP
 * 
 *
 * May 22 2006 mtk01239
 * [MAUI_00195772] [Bluetooth] ESI Bluetooth path for A2DP and AVDTP
 * 
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "avrcp.h"
#include "assert.h"
#include "bluetooth_avrcp_struct.h"


/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/
 
#define ASSERT assert

#define AVRCP_CONVERT_BDADDR2ARRAY(dest, lap, uap, nap) \
        ((U8 *) dest)[0] = ((U8)(((U32)lap) & 0x000000FF));\
        ((U8 *) dest)[1] = ((U8)((((U32)lap) & 0x0000FF00) >> 8));\
        ((U8 *) dest)[2] = ((U8)((((U32)lap) & 0x00FF0000) >> 16));\
        ((U8 *) dest)[3] = ((U8)uap);\
        ((U8 *) dest)[4] = ((U8)(((U16)nap) & 0x00FF));\
        ((U8 *) dest)[5] = ((U8)((((U16)nap) & 0xFF00)>>8)); 

#define AVRCP_CONVERT_ARRAY2BDADDR(dest, src) \
     ((bt_avrcp_dev_addr_struct *)dest)->nap = (((U16)((U8 *) src)[4])) | ((U16)((U8 *) src)[5]<<8);\
    ((bt_avrcp_dev_addr_struct *)dest)->uap = (U8)((U8 *) src)[3];\
    ((bt_avrcp_dev_addr_struct *)dest)->lap =  (((U32)((U8 *) src)[2]) << 16) | ((U32)((U8 *) src)[1] <<8)|((U32)((U8 *) src)[0]); 

#define AVRCP_CONVERT_UUID_FROM_ROLE(_adp_role_ , _uuid_role_) \
    do {                                    \
        if (_adp_role_ == BT_AVRCP_TG)      \
        {                                   \
            _uuid_role_ = AVRCP_TG;         \
        }                                   \
        else if (_adp_role_ == BT_AVRCP_CT) \
        {                                   \
            _uuid_role_ = AVRCP_CT;         \
        }                                   \
        else                                \
        {                                   \
            ASSERT(0);                      \
        }                                   \
    } while(0);                              \

typedef U8 BTAVRCPEvent;

#define BTAVRCP_EVENT_ACTIVATE_CNF                  0x01
#define BTAVRCP_EVENT_DEACTIVATE_CNF                0x02
#define BTAVRCP_EVENT_CONNECT_CNF                   0x03
#define BTAVRCP_EVENT_CONNECT_IND                   0x04
#define BTAVRCP_EVENT_DISCONNECT_CNF                0x05
#define BTAVRCP_EVENT_DISCONNECT_IND                0x06
#define BTAVRCP_EVENT_COMMAND_FRAME_CNF             0x07
#define BTAVRCP_EVENT_COMMAND_FRAME_IND             0x08
#define BTAVRCP_EVENT_BROWSE_CONNECT_CNF            0x09
#define BTAVRCP_EVENT_BROWSE_CONNECT_IND            0x0a
#define BTAVRCP_EVENT_BROWSE_DISCONNECT_IND         0x0b

#define BTAVRCP_EVENT_GET_CAPABILITIES_IND                    0x20
#define BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_IND     0x21
#define BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_IND         0x22
#define BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_IND       0x23
#define BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_IND          0x24
#define BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_IND  0x25
#define BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_IND      0x26
#define BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_IND              0x27
#define BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_IND           0x28
#define BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_IND              0x29
#define BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_IND                0x2a
#define BTAVRCP_EVENT_REGISTER_NOFITICATION_IND               0x2b
#define BTAVRCP_EVENT_REQUEST_CONTINUING_IND                  0x2c
#define BTAVRCP_EVENT_ABORT_CONTINUING_IND                    0x2d
#define BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_IND                 0x2e
#define BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_IND                 0x2f
#define BTAVRCP_EVENT_SET_BROWSEDPLAYER_IND                   0x30
#define BTAVRCP_EVENT_GET_FOLDERITEMS_IND                     0x31
#define BTAVRCP_EVENT_CHANGE_PATH_IND                         0x32
#define BTAVRCP_EVENT_GET_ITEMATTRIBUTES_IND                  0x33
#define BTAVRCP_EVENT_PLAY_ITEMS_IND                          0x34
#define BTAVRCP_EVENT_SEARCH_IND                              0x35
#define BTAVRCP_EVENT_ADD_TONOWPLAYING_IND                    0x36

#define BTAVRCP_EVENT_GET_CAPABILITIES_CNF                    0x50
#define BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGATTRIBUTE_CNF     0x51
#define BTAVRCP_EVENT_LIST_PLAYERAPP_SETTINGVALUE_CNF         0x52
#define BTAVRCP_EVENT_GET_CURPLAYERAPP_SETTINGVALUE_CNF       0x53
#define BTAVRCP_EVENT_SET_PLAYERAPP_SETTINGVALUE_CNF          0x54
#define BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGATTRIBUTETEXT_CNF  0x55
#define BTAVRCP_EVENT_GET_PLAYERAPP_SETTINGVALUETEXT_CNF      0x56
#define BTAVRCP_EVENT_INFORM_DISPLAY_CHARSET_CNF              0x57
#define BTAVRCP_EVENT_INFORM_BATTERY_STATUSOFCT_CNF           0x58
#define BTAVRCP_EVENT_GET_ELEMENT_ATTRIBUTES_CNF              0x59
#define BTAVRCP_EVENT_GET_PLAYERAPP_STATUS_CNF                0x5a
#define BTAVRCP_EVENT_REGISTER_NOFITICATION_CNF               0x5b
#define BTAVRCP_EVENT_REQUEST_CONTINUING_CNF                  0x5c
#define BTAVRCP_EVENT_ABORT_CONTINUING_CNF                    0x5d
#define BTAVRCP_EVENT_SET_ABSOLUTE_VOLUME_CNF                 0x5e
#define BTAVRCP_EVENT_SET_ADDRESSEDPLAYER_CNF                 0x5f
#define BTAVRCP_EVENT_SET_BROWSEDPLAYER_CNF                   0x60
#define BTAVRCP_EVENT_GET_FOLDERITEMS_CNF                     0x61
#define BTAVRCP_EVENT_CHANGE_PATH_CNF                         0x62
#define BTAVRCP_EVENT_GET_ITEMATTRIBUTES_CNF                  0x63
#define BTAVRCP_EVENT_PLAY_ITEMS_CNF                          0x64
#define BTAVRCP_EVENT_SEARCH_CNF                              0x65
#define BTAVRCP_EVENT_ADD_TONOWPLAYING_CNF                    0x66

typedef struct _BTAVRCPEventActivateCnf{
    U8 chnl_num;
    U8 version;
}BTAVRCPEventActivateCnf;

typedef struct _BTAVRCPEventDeactivateCnf{
    U8 chnl_num;
}BTAVRCPEventDeactivateCnf;

typedef struct _BTAVRCPEventConnectCnf {
    U8 chnl_num;
    U8 connection_id;
    bt_avrcp_dev_addr_struct dev_addr;
    U16 browse_channel;
    U16 remote_tg_support_feature; 
    U16 remote_ct_support_feature; 
}BTAVRCPEventConnectCnf;

typedef struct _BTAVRCPEventConnectInd {
    U8 chnl_num;
    U8 connection_id;
    bt_avrcp_dev_addr_struct dev_addr;
    U8 device_name_size;
    U8 *device_name;
}BTAVRCPEventConnectInd;

typedef struct _BTAVRCPEventDisConnectCnf {
    U8 chnl_num;
    U8 connection_id;
   
}BTAVRCPEventDisconnectCnf;

typedef struct _BTAVRCPEventDisConnectInd {
    U8 chnl_num;
    U8 connection_id;
    
}BTAVRCPEventDisconnectInd;

typedef struct _BTAVRCPCmdFrameCnf {
    U8 chnl_num;
    U8 seq_id;
    U8 ctype;
    U8 subunit_type;
    U8 subunit_id;
    U16 data_len;
    U8* frame_data;

}BTAVRCPCmdFrameCnf;

typedef struct _BTAVRCPCmdFrameInd {
    U8 chnl_num;
    U8 seq_id;
    U8 profile_id;
    U8 ctype;
    U8 subunit_type;
    U8 subunit_id;
    U16 data_len;
    U8* frame_data;

}BTAVRCPCmdFrameInd;

/* command indication */
        
typedef bt_avrcp_browse_connect_cnf_struct                      BTAVRCPEventBrowseConnectCnf;
typedef bt_avrcp_browse_connect_ind_struct                      BTAVRCPEventBrowseConnectInd;  
typedef bt_avrcp_browse_disconnect_ind_struct                   BTAVRCPEventBrowseDisconnectInd;
        
typedef bt_avrcp_get_capabilities_ind_struct                 BTAVRCPGetCapabilitiesInd;
typedef bt_avrcp_list_playerapp_attribute_ind_struct         BTAVRCPListPlayerappAttributeInd;
typedef bt_avrcp_list_playerapp_value_ind_struct             BTAVRCPListPlayerappValueInd;
typedef bt_avrcp_get_curplayerapp_value_ind_struct           BTAVRCPGetCURPlayerValueInd;
typedef bt_avrcp_set_playerapp_value_ind_struct              BTAVRCPSetPlayerValueInd; 
typedef bt_avrcp_get_playerapp_attributetext_ind_struct      BTAVRCPGetPlayerAttrTextInd;
typedef bt_avrcp_get_playerapp_valuetext_ind_struct          BTAVRCPGetPlayerValueTextInd;
typedef bt_avrcp_inform_display_charset_ind_struct           BTAVRCPInformCharsetInd;
typedef bt_avrcp_inform_battery_statusofct_ind_struct        BTAVRCPInformBattOfCTInd;
typedef bt_avrcp_get_element_attributes_ind_struct           BTAVRCPGetElementAttrInd;
typedef bt_avrcp_get_playerstatus_ind_struct                 BTAVRCPGetPlayerStatusInd;
typedef bt_avrcp_register_notification_ind_struct             BTAVRCPRegisterNotifyInd;
typedef bt_avrcp_abort_continueresponse_ind_struct           BTAVRCPAbortContinueInd;
typedef bt_avrcp_set_absolute_volume_ind_struct              BTAVRCPSetAbsoluteVolumeInd;
typedef bt_avrcp_set_addressedplayer_ind_struct              BTAVRCPSetAddrPlayerInd;
typedef bt_avrcp_get_playeritems_ind_struct                  BTAVRCPGetPlayItemsInd;
typedef bt_avrcp_get_folderitems_ind_struct                  BTAVRCPGetFolderItemsInd;
typedef bt_avrcp_set_browsedplayer_ind_struct                BTAVRCPSetBrowsePlayerInd;
typedef bt_avrcp_change_path_ind_struct                      BTAVRCPChangePathInd;
typedef bt_avrcp_get_itemattributes_ind_struct               BTAVRCPGetItemAttrInd;
typedef bt_avrcp_play_items_ind_struct                       BTAVRCPPlayItemsInd;
typedef bt_avrcp_search_ind_struct                           BTAVRCPSearchInd;
typedef bt_avrcp_add_tonowplaying_ind_struct                 BTAVRCPAddNowPlayingInd;

/* response indication */
typedef bt_avrcp_get_capabilities_cnf_struct                 BTAVRCPGetCapabilitiesCnf;
typedef bt_avrcp_list_playerapp_attribute_cnf_struct         BTAVRCPListPlayerappAttributeCnf;
typedef bt_avrcp_list_playerapp_value_cnf_struct             BTAVRCPListPlayerappValueCnf;
typedef bt_avrcp_get_curplayerapp_value_cnf_struct           BTAVRCPGetCURPlayerValueCnf;
typedef bt_avrcp_set_playerapp_value_cnf_struct              BTAVRCPSetPlayerValueCnf; 
typedef bt_avrcp_get_playerapp_attributetext_cnf_struct      BTAVRCPGetPlayerAttrTextCnf;
typedef bt_avrcp_get_playerapp_valuetext_cnf_struct          BTAVRCPGetPlayerValueTextCnf;
typedef bt_avrcp_inform_display_charset_cnf_struct           BTAVRCPInformCharsetCnf;
typedef bt_avrcp_inform_battery_statusofct_cnf_struct        BTAVRCPInformBattOfCTCnf;
typedef bt_avrcp_get_element_attributes_cnf_struct           BTAVRCPGetElementAttrCnf;
typedef bt_avrcp_get_playerstatus_cnf_struct                 BTAVRCPGetPlayerStatusCnf;
typedef bt_avrcp_register_notification_cnf_struct             BTAVRCPRegisterNotifyCnf;
typedef bt_avrcp_abort_continueresponse_cnf_struct           BTAVRCPAbortContinueCnf;
typedef bt_avrcp_set_absolute_volume_cnf_struct              BTAVRCPSetAbsoluteVolumeCnf;
typedef bt_avrcp_set_addressedplayer_cnf_struct              BTAVRCPSetAddrPlayerCnf;
typedef bt_avrcp_get_playeritems_cnf_struct                  BTAVRCPGetPlayItemsCnf;
typedef bt_avrcp_get_folderitems_cnf_struct                  BTAVRCPGetFolderItemsCnf;
typedef bt_avrcp_set_browsedplayer_cnf_struct                BTAVRCPSetBrowsePlayerCnf;
typedef bt_avrcp_change_path_cnf_struct                      BTAVRCPChangePathCnf;
typedef bt_avrcp_get_itemattributes_cnf_struct               BTAVRCPGetItemAttrCnf;
typedef bt_avrcp_play_items_cnf_struct                       BTAVRCPPlayItemsCnf;
typedef bt_avrcp_search_cnf_struct                           BTAVRCPSearchCnf;
typedef bt_avrcp_add_tonowplaying_cnf_struct                 BTAVRCPAddNowPlayingCnf;


typedef struct _BTAVRCPCallbackParms {
    BTAVRCPEvent    event;   /* Event associated with the callback       */
    U16            result;
    U8             error_code;
    union{
        BTAVRCPEventActivateCnf    AVRCPEventActivateCnf;
        BTAVRCPEventDeactivateCnf    AVRCPEventDeactivateCnf;
        BTAVRCPEventConnectCnf    AVRCPEventConnectCnf;
        BTAVRCPEventConnectInd    AVRCPEventConnectInd;  
        BTAVRCPEventDisconnectCnf    AVRCPEventDisconnectCnf;
        BTAVRCPEventDisconnectInd    AVRCPEventDisconnectInd;
        BTAVRCPCmdFrameCnf  AVRCPEventCmdFrameCnf;
        BTAVRCPCmdFrameInd  AVRCPEventCmdFrameInd;
        BTAVRCPEventBrowseConnectCnf AVRCPEventBrowseConnectCnf;
        BTAVRCPEventBrowseConnectInd AVRCPEventBrowseConnectInd;         
        BTAVRCPEventBrowseDisconnectInd AVRCPEventBrowseDisconnectInd;
        BTAVRCPGetCapabilitiesInd AVRCPGetCapabilitiesInd;
        BTAVRCPListPlayerappAttributeInd AVRCPListPlayerappAttributeInd;
        BTAVRCPListPlayerappValueInd AVRCPListPlayerappValueInd;
        BTAVRCPGetCURPlayerValueInd ABTAVRCPGetCURPlayerValueInd;
        BTAVRCPSetPlayerValueInd  ABTAVRCPSetPlayerValueInd ;
        BTAVRCPGetPlayerAttrTextInd ABTAVRCPGetPlayerAttrTextInd;
        BTAVRCPGetPlayerValueTextInd ABTAVRCPGetPlayerValueTextInd;
        BTAVRCPInformCharsetInd ABTAVRCPInformCharsetInd;
        BTAVRCPInformBattOfCTInd ABTAVRCPInformBattOfCTInd;
        BTAVRCPGetElementAttrInd ABTAVRCPGetElementAttrInd;
        BTAVRCPGetPlayerStatusInd ABTAVRCPGetPlayerStatusInd;
        BTAVRCPRegisterNotifyInd ABTAVRCPRegisterNotifyInd;
        BTAVRCPAbortContinueInd ABTAVRCPAbortContinueInd;
        BTAVRCPSetAbsoluteVolumeInd ABTAVRCPSetAbsoluteVolumeInd;
        BTAVRCPSetAddrPlayerInd ABTAVRCPSetAddrPlayerInd;
        BTAVRCPGetPlayItemsInd ABTAVRCPGetPlayItemsInd;
        BTAVRCPGetFolderItemsInd ABTAVRCPGetFolderItemsInd;
        BTAVRCPSetBrowsePlayerInd ABTAVRCPSetBrowsePlayerInd;
        BTAVRCPChangePathInd ABTAVRCPChangePathInd;
        BTAVRCPGetItemAttrInd ABTAVRCPGetItemAttrInd;
        BTAVRCPPlayItemsInd ABTAVRCPPlayItemsInd;
        BTAVRCPSearchInd ABTAVRCPSearchInd;
        BTAVRCPAddNowPlayingInd ABTAVRCPAddNowPlayingInd;

        BTAVRCPGetCapabilitiesCnf ABTAVRCPGetCapabilitiesCnf;
        BTAVRCPListPlayerappAttributeCnf ABTAVRCPListPlayerappAttributeCnf;
        BTAVRCPListPlayerappValueCnf ABTAVRCPListPlayerappValueCnf;
        BTAVRCPGetCURPlayerValueCnf ABTAVRCPGetCURPlayerValueCnf;
        BTAVRCPSetPlayerValueCnf ABTAVRCPSetPlayerValueCnf;
        BTAVRCPGetPlayerAttrTextCnf ABTAVRCPGetPlayerAttrTextCnf;
        BTAVRCPGetPlayerValueTextCnf ABTAVRCPGetPlayerValueTextCnf;
        BTAVRCPInformCharsetCnf ABTAVRCPInformCharsetCnf;
        BTAVRCPInformBattOfCTCnf ABTAVRCPInformBattOfCTCnf;
        BTAVRCPGetElementAttrCnf ABTAVRCPGetElementAttrCnf;
        BTAVRCPGetPlayerStatusCnf ABTAVRCPGetPlayerStatusCnf;
        BTAVRCPRegisterNotifyCnf ABTAVRCPRegisterNotifyCnf;
        BTAVRCPAbortContinueCnf ABTAVRCPAbortContinueCnf;
        BTAVRCPSetAbsoluteVolumeCnf ABTAVRCPSetAbsoluteVolumeCnf;
        BTAVRCPSetAddrPlayerCnf ABTAVRCPSetAddrPlayerCnf;
        BTAVRCPGetPlayItemsCnf ABTAVRCPGetPlayItemsCnf;
        BTAVRCPGetFolderItemsCnf ABTAVRCPGetFolderItemsCnf;
        BTAVRCPSetBrowsePlayerCnf ABTAVRCPSetBrowsePlayerCnf;
        BTAVRCPChangePathCnf ABTAVRCPChangePathCnf;
        BTAVRCPGetItemAttrCnf ABTAVRCPGetItemAttrCnf;
        BTAVRCPPlayItemsCnf ABTAVRCPPlayItemsCnf;
        BTAVRCPSearchCnf ABTAVRCPSearchCnf;
        BTAVRCPAddNowPlayingCnf ABTAVRCPAddNowPlayingCnf;
    }p;

}BTAVRCPCallbackParms;

typedef void (*BTAVRCPCallback)(BTAVRCPCallbackParms *Parms);


typedef enum 
{
    BT_AVRCP_STATE_IDLE,
    BT_AVRCP_STATE_ACTIVATED,
    BT_AVRCP_STATE_CONNECTING,
    BT_AVRCP_STATE_ACCEPTING_CONNECT,
    BT_AVRCP_STATE_CONNECTED,
    BT_AVRCP_STATE_DISCONNECTING
} avrcp_state;

typedef enum 
{
    AVRCP_NO_DATA,
    AVRCP_CMD_MORE,
    AVRCP_RSP_MORE
} avrcp_more_data;

typedef struct
{
    avrcp_state     state;
    U8       rcp_role;
    U8       connect_id;
    AvrcpChannel    rcp_channel;
    AvrcpCmdFrame   cmdFrame;
    AvrcpRspFrame   rspFrame;
    avrcp_more_data   cmd_more_data;
    avrcp_more_data   rsp_more_data;
    BTAVRCPCallback avrcpCBfunc;

    /* AV13 available Packets */
    ListEntry freePacketList;
    /* AV13 Packet resources */
    AvrcpQueuePacketFrame txPackets[AVRCP_NUM_TX_PACKETS];
    /* AV13 packet array for queue the incoming data */
    U8 txData[AVRCP_NUM_TX_PACKETS][AVRCP_MAX_PACKET_LEN]; 

    /* AV13 channel type 0 for AVCTP 1 for AVCTP_Browsing*/
} avrcp_adp_context;






void bt_avrcp_send_activate_cnf( module_type dst_mod_id, U8 chnl_num, U16 result, U8 version );

void bt_avrcp_send_deactivate_cnf( module_type dst_mod_id, U8 chnl_num, U16 result );

void bt_avrcp_send_connect_cnf( module_type dst_mod_id, U8 chnl_num, U16 result, bt_avrcp_dev_addr_struct* device_addr, U8 connect_id);
void bt_avrcp_send_connect_ind( module_type dst_mod_id, 
                                        U8 chnl_num, 
                                        bt_avrcp_dev_addr_struct* device_addr, 
                                        U8 connect_id,
                                        U8 device_name_size,
                                        U8 *device_name);

void bt_avrcp_send_disconnect_cnf( module_type dst_mod_id, U8 chnl_num, U16 result, U8 connect_id );
void bt_avrcp_send_disconnect_ind( module_type dst_mod_id, U8 chnl_num, U8 connect_id);

void bt_avrcp_send_cmd_frame_cnf( module_type dst_mod_id, U8 chnl_num, U8 seq_id, U8 c_type, U8	subunit_type, U8 subunit_id,	U16 data_len, U8 *frame_data);
void bt_avrcp_send_cmd_frame_ind( module_type dst_mod_id, U8 chnl_num, U8 seq_id, U16 profile_id,	U8	c_type,	U8	subunit_type, U8 subunit_id,	U16 data_len, U8 *frame_data);
void bt_avrcp_send_cmd_frame_ind_res( module_type src_mod_id, U8 chnl_num, U8 seq_id, U16 profile_id,U8 c_type, U8 subunit_type, U8 subunit_id, U16 data_len, U8* frame_data);


