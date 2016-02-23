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
*  permission of MediaTek Inc. (C) 2005
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * Bt_adp_bip.h
 *
 * Project:
 * --------
 *   BT BIP
 *
 * Description:
 * ------------
 *   This file is for BT BIP ADP service.
 *
 * Author:
 * -------
 * Attar Hsu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Jan 12 2009 mtk80029
 * [MAUI_01496803] MTK:Bluetooth_it handset always show "Connecting" screen
 * let responder cntx struct has its own xml parsing result member
 *
 * Jan 8 2009 mbj06032
 * [MAUI_01362135] [BT BIP ADP] revise code related to parsing imagelist XML in segment
 * 
 *
 * Dec 26 2008 mbj06032
 * [MAUI_01483740] MTK:Bluetooth_[1] Fatal Error (305, 8888005d) - MMI
 * 
 *
 * Dec 26 2008 mbj06032
 * [MAUI_01483740] MTK:Bluetooth_[1] Fatal Error (305, 8888005d) - MMI
 * parse the imagelist intermittently
 *
 * Nov 3 2008 mbj06032
 * [MAUI_01331519] [BT BIP] BIP server check received DRM file's RIGHTS
 * 
 *
 * Oct 6 2008 mbj06032
 * [MAUI_01117427] [BT BIP] code revisement
 * 
 *
 * Sep 23 2008 mbj06032
 * [MAUI_00789020] BT-BIP problem
 * to save fs quota, composing image list xml is moved to MMI
 *
 * Sep 23 2008 mbj06032
 * [MAUI_01116233] [BT] Check received DCFs: if no Rights info, delete it
 * 
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106048] [BT BIP] send pushed file detail info to MMI
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_01091714] [BT BIP] server show connect confirm when RFCOMM Open Indication
 * notify server when  RFCOMM Open Ind to let user confirm:
 *
 * May 22 2008 mbj06032
 * [MAUI_00991010] MTK:Bluetooth BIP_can't disconnect BIP
 * BIP: add session role member(RESPONDER or INITIATOR) to DISCONNECT_IND msg & DISCONNECT_REQ msg
 *
 * Feb 22 2008 mbj06032
 * [MAUI_00084141] Unable to delete JPEG files from Memory card.
 * if FS_Close return FS_DEVICE_BUSY, then start timer to close the file
 *
 * Jan 7 2008 mtk00511
 * [MAUI_00594672] [BT_ADP][BIP]Remove redudant trace and global variables
 * 
 *
 * Dec 3 2007 mtk00511
 * [MAUI_00481330] [BT]Should remind "Disk Full"
 * 
 *
 * Oct 29 2007 mtk00511
 * [MAUI_00239666] [MTK:Bluetooth BIP]Fatal Error (4): Data Abort (4e5cfc) - BT
 * 
 *
 * Oct 22 2007 mtk00511
 * [MAUI_00481867] [BT]The sum of all files display error
 * 
 *
 * Oct 12 2007 mtk00511
 * [MAUI_00479797] [Need Patch] for 07BW0741OF for BIP feature function
 * 
 *
 * Oct 8 2007 mtk00511
 * [MAUI_00556452] [Bluetooth][BT_ADP]Update BIP profile
 * 
 *
 * Sep 14 2007 mtk00511
 * [MAUI_00469578] [BT] [BIP] ( For TW ) Check in
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifndef BT_ADP_BIP_H
#define BT_ADP_BIP_H

/***************************************************************************** 
 * Macro
 *****************************************************************************/


/***************************************************************************** 
 * Definations
 ****************************************************************************/
 /* L for wide char */
#ifdef BTMTK_ON_WISESDK

#if 0
#define BIP_ADP_WORK_FOLDER     (L".\\bip\\")           //working folder
#define BIP_ADP_SND_FILE        (L".\\bip\\snd.tmp")     //for sending object
#define BIP_ADP_RCV_FILE        (L".\\bip\\rcv.tmp")     //for receiving object
#define BIP_ADP_IMG_LIST_FILE   (L".\\bip\\img_list.tmp")     //for image list
#define BIP_ADP_DESCPTR_FILE    (L".\\bip\\descptr.tmp")     //for descriptor xml file
#define BIP_ADP_PUSH_TMP_FILE   (L".\\bip\\bip_push.tmp")     //for incoming push tmp file
#endif

#elif defined(BTMTK_ON_LINUX)
//mtk03036
#define BIP_ADP_ROOT_FOLDER     (L"/data/@btmtk/")
#define BIP_ADP_WORK_FOLDER     (L"/data/@btmtk/profile/")           //working folder
#define BIP_ADP_SND_FILE        (L"/data/@btmtk/profile/bip_snd.tmp")     //for sending object
#define BIP_ADP_RCV_FILE        (L"/data/@btmtk/profile/bip_rcv.tmp")     //for receiving object
#define BIP_ADP_IMG_LIST_FILE   (L"/data/@btmtk/profile/bip_img_list.tmp")     //for image list
#define BIP_ADP_DESCPTR_FILE    (L"/data/@btmtk/profile/bip_descptr.tmp")     //for descriptor xml file
#define BIP_ADP_PUSH_TMP_FILE   (L"/data/@btmtk/profile/bip_push.tmp")     //for incoming push tmp file

#elif WIN32

#define BIP_ADP_WORK_FOLDER     (L"D:\\bip\\")           //working folder
#define BIP_ADP_SND_FILE        (L"D:\\bip\\snd.tmp")     //for sending object
#define BIP_ADP_RCV_FILE        (L"D:\\bip\\rcv.tmp")     //for receiving object
#define BIP_ADP_IMG_LIST_FILE   (L"D:\\bip\\img_list.tmp")     //for image list
#define BIP_ADP_DESCPTR_FILE     (L"D:\\bip\\descptr.tmp")     //for descriptor xml file
#define BIP_ADP_PUSH_TMP_FILE   (L"D:\\bip\\bip_push.tmp")     //for incoming push tmp file

#elif BTMTK_ON_WISE

#define BIP_ADP_ROOT_FOLDER     (L"c:\\@btmtk\\")
#define BIP_ADP_WORK_FOLDER     (L"c:\\@btmtk\\profile")           //working folder
#define BIP_ADP_SND_FILE        (L"c:\\@btmtk\\profile\\bip_snd.tmp")     //for sending object
#define BIP_ADP_RCV_FILE        (L"c:\\@btmtk\\profile\\bip_rcv.tmp")     //for receiving object
#define BIP_ADP_IMG_LIST_FILE   (L"c:\\@btmtk\\profile\\bip_img_list.tmp")     //for image list
#define BIP_ADP_DESCPTR_FILE    (L"c:\\@btmtk\\profile\\bip_descptr.tmp")     //for descriptor xml file
#define BIP_ADP_PUSH_TMP_FILE   (L"c:\\@btmtk\\profile\\bip_push.tmp")     //for incoming push tmp file

#else

#define BIP_ADP_WORK_FOLDER     (L"Z:\\@bip\\")           //working folder
#define BIP_ADP_SND_FILE        (L"Z:\\@bip\\snd.tmp")     //for sending object
#define BIP_ADP_RCV_FILE        (L"Z:\\@bip\\rcv.tmp")     //for receiving object
#define BIP_ADP_IMG_LIST_FILE   (L"Z:\\@bip\\img_list.tmp")     //for image list
#define BIP_ADP_DESCPTR_FILE     (L"Z:\\@bip\\descptr.tmp")     //for descriptor xml file
#define BIP_ADP_PUSH_TMP_FILE   (L"Z:\\@bip\\bip_push.tmp")     //for incoming push tmp file

#endif

#define BIP_ADP_STR_IMG_CAP         "imaging-capabilities"
#define BIP_ADP_STR_PREFER_FORMAT   "preferred-format"
#define BIP_ADP_STR_IMG_LIST        "images-listing"
#define BIP_ADP_STR_IMG             "image"
#define BIP_ADP_STR_HENDLE          "handle"
#define BIP_ADP_STR_CREATED         "created"
#define BIP_ADP_STR_MODIFIED        "modified"
#define BIP_ADP_STR_IMG_DESC        "image-descriptor"
#define BIP_ADP_STR_ENC             "encoding"
#define BIP_ADP_STR_PIXEL           "pixel"
#define BIP_ADP_STR_SIZE            "size"
#define BIP_ADP_STR_VERSION         "version"
#define BIP_ADP_STR_1_0             "1.0"
#define BIP_ADP_STR_1               "1"
#define BIP_ADP_STR_0               "0"
#define BIP_ADP_STR_FILTER_PARA     "filtering-parameters"
#define BIP_ADP_STR_IMG_HANDLE_DESC "image-handles-descriptor"
#define BIP_ADP_STR_IMG_FORMAT      "image-formats"
#define BIP_ADP_STR_ATTACH_FORMAT   "attachment-formats"
#define BIP_ADP_STR_JPEG            "JPEG"
#define BIP_ADP_STR_BMP             "BMP"
#define BIP_ADP_STR_GIF             "GIF"
#define BIP_ADP_STR_PNG             "PNG"
#define BIP_ADP_STR_WBMP            "WBMP"
#define BIP_ADP_STR_IMG_PROP        "image-properties"
#define BIP_ADP_STR_NATIVE          "native"
#define BIP_ADP_STR_VARIANT         "variant"
#define BIP_ADP_STR_ATTACHMENT      "attachment"
#define BIP_ADP_STR_FRIENDLY_NAME   "friendly-name"
#define BIP_ADP_STR_MAXSIZE     "maxsize"
#define BIP_ADP_STR_TRANSFORMATION  "transformation"
#define BIP_ADP_MAX_COMPOSE_BUF_SIZE 1024
#define BIP_ADP_WRITE_BUF_THRESHOLD 512

#define BIP_ADP_XMLPARSE_NUM_THRESHOLD  200

/* fs continue timer duration: mSec */
#define BIP_ADP_FS_CONT_DUR     (1000)

/* ENABLE the received DRM file checking for the server */
//#define BIP_CHECK_DRM_RIGHTS   


/***************************************************************************** 
 * Enum
 *****************************************************************************/

typedef enum
{
    BT_BIP_I_STATE_IDLE,
    BT_BIP_I_STATE_CONNECTING,
    BT_BIP_I_STATE_CONNECTED,
    BT_BIP_I_STATE_REQUESTING,
    BT_BIP_I_STATE_XMLPARSING,
    BT_BIP_I_STATE_ABORTING,
    BT_BIP_I_STATE_DISCONNECTING,
    BT_BIP_I_STATE_AUTHENTICATING /* be challenged and waiting for response */
} bt_bip_initiator_state_enum;

typedef enum
{
    BT_BIP_R_STATE_IDLE,
    BT_BIP_R_STATE_ACTIVATING,
    BT_BIP_R_STATE_ACTIVATED,
    BT_BIP_R_STATE_AUTHORIZING,
    BT_BIP_R_STATE_AUTHORIZED,
    BT_BIP_R_STATE_ACCEPTING,
    BT_BIP_R_STATE_ACCEPTED,
    BT_BIP_R_STATE_INDICATING,
    BT_BIP_R_STATE_RESPONDING,
    BT_BIP_R_STATE_ABORTING,
    BT_BIP_R_STATE_DISCONNECTING
} bt_bip_responder_state_enum;

typedef enum
{
  BIP_ADP_XMLPARSE_FAILED = -1,
  BIP_ADP_XMLPARSE_SUCCESS = 0,
  BIP_ADP_XMLPARSE_TPDISCONNECTED = 1,
  BIP_ADP_XMLPARSE_USERCANCELED,
  BIP_ADP_XMLPARSE_LASTNUM
}bip_adp_xmlparse_end_enum;

typedef enum
{
    BIP_ADP_CHAL_STATUS_NONE,
    BIP_ADP_CHAL_STATUS_REQUEST,
    BIP_ADP_CHAL_STATUS_VERIFY_OK,
    BIP_ADP_CHAL_STATUS_VERIFY_FAILED
} bip_adp_challenge_status_enum;

/***************************************************************************** 
 * Structure
 *****************************************************************************/

typedef struct
{
    kal_int32   fd;
    kal_uint8   buf[BIP_ADP_MAX_COMPOSE_BUF_SIZE];
    kal_uint32  buflen;
    kal_uint32  written;
} bt_bip_xml_composer_struct;

typedef struct
{
    kal_uint8  *p_buffer;
    kal_uint32  buflen;
}bt_bip_buffer_writer;

typedef struct
{
    kal_int32               fh;
    kal_uint16              wrote_img_item_num;
    kal_uint16              wrote_img_num_interval;
    bt_bip_img_min_info_struct parsed_img_item;
    bt_bip_buffer_writer    writer;
} bt_bip_xml_parsed_img_list_struct;

typedef struct
{
    bt_bip_img_format_enum  supported_img_formats[BT_BIP_MAX_IMG_FORMATS];
    bt_bip_pixel_struct supported_img_size[BT_BIP_MAX_IMG_FORMATS];
    U32 supported_maxsize[BT_BIP_MAX_IMG_FORMATS];
    
    bt_bip_img_format_enum  preferred_format;
    bt_bip_pixel_struct preferred_pixel;
    U32 preferred_maxsize;
    U32 preferred_trans;
    
    kal_bool                created_time_filter;
    kal_bool                modified_time_filter;
    kal_bool                encoding_filter;
    kal_bool                pixel_filter;
} bt_bip_xml_parsed_img_cap_struct;

typedef union
{
    bt_bip_xml_parsed_img_list_struct parsed_img_list;
    bt_bip_xml_parsed_img_cap_struct parsed_img_cap;
    bt_bip_img_info_struct parsed_img_prop;
} bt_bip_i_xml_parsed_info_union;

typedef union
{
    bt_bip_xml_parsed_img_cap_struct parsed_img_cap;
    bt_bip_img_info_struct parsed_img_prop;
    bt_bip_img_desc_struct parsed_img_desc;
} bt_bip_r_xml_parsed_info_union;


typedef struct
{
    /* initiator member */
    bt_bip_bd_addr_struct       i_bd_addr;
    U16         iService;
    bt_bip_initiator_state_enum iState;
    BipInitiatorSession         BipInitiator;
    BipData                     iData;
    kal_uint32                  i_cm_conn_id;
    kal_uint8                   i_req_id;
    bt_bip_i_xml_parsed_info_union i_xml_parsed_result;  
    kal_uint16                   file_path[BT_BIP_MAX_PATH_LEN];
    kal_int32                   i_data_len;
    kal_uint8                   i_err_code;
    /* for parse imagelist at interval */
    kal_uint16                  i_rspcode;
    BOOL                        i_parse_paused;
    XML_PARSER_STRUCT           *i_xml_parser_p;

    /* request challenge */
    bt_bip_dig_challenge_struct i_dig_chal;
    ObexAuthChallenge   i_obex_chal;
    kal_uint8 i_nonce[16];
    /* challenge response verify status */
    kal_uint8 i_chal_status;    
    /* receive challenge */
    ObexAuthChallengeInfo i_chal_info;
    ObexAuthChallengeInfo* i_chal_info_p;
    /* response challenge */
    bt_bip_dig_response_struct i_dig_rsp;    
    ObexAuthResponse    i_obex_rsp;
    
    

    /* responder member */
    bt_bip_bd_addr_struct       r_bd_addr;
    bt_bip_responder_state_enum rState;
    BipResponderSession         BipResponder;
    BipData                     rData;
    BipData*                    rData_p;
    kal_uint32                  r_cm_conn_id;
    bt_bip_r_xml_parsed_info_union r_xml_parsed_result;  
    kal_int32                   r_data_len;
    kal_uint8                   r_err_code;

	int                         r_fshandle_to_close;
	eventid                     r_fs_cont_timer_id;
#ifdef BIP_CHECK_DRM_RIGHTS   
    kal_uint16                  r_recv_path[BT_BIP_MAX_PATH_LEN];
#endif
    XML_PARSER_STRUCT           *r_xml_parser_p;

    /* common member */
    bt_bip_xml_composer_struct  *xml_composer_p;

    /* init challenge */
    bt_bip_dig_challenge_struct r_dig_chal;    
    ObexAuthChallenge   r_obex_chal;
    kal_uint8 r_nonce[16];
    kal_uint8 r_chal_status;
    
    /* receive challenge */
    ObexAuthChallengeInfo r_chal_info;
    ObexAuthChallengeInfo* r_chal_info_p;
    /* response challenge */
    bt_bip_dig_response_struct r_dig_rsp;
    ObexAuthResponse    r_obex_rsp;
} bip_adp_cntx_struct;

/***************************************************************************** 
 * Extern Global Variable
 *****************************************************************************/
extern bip_adp_cntx_struct *bip_cntx_p;


/***************************************************************************** 
 * Extern Global Function
 *****************************************************************************/
void bip_adp_init(void);
void bip_adp_deinit(void);

/* callback functions */
void bip_adp_msg_hdlr(ilm_struct *ilm_ptr);

#endif /* BT_ADP_BIP_H */ 

