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
 * Bt_adp_bip.c
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
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 * 09 01 2010 sh.lai
 * NULL
 * Integration change into 1036OF.
 *
 * Sep 18 2009 mtk80017
 * [MAUI_01898639] adp code modify
 * 
 *
 * Sep 14 2009 mtk80017
 * [MAUI_01897314] remove bt_adp_bip.c  in command rsp , if rsp_code is not success, it not send abort_
 * 
 *
 * Sep 2 2009 mtk80029
 * [MAUI_01596143] Loading displays in file manager when Cancel sending.
 * 
 *
 * Jun 26 2009 mtk80029
 * [MAUI_01872828] [BT]Can't receive file after sender remind Done
 * When push image file, if FS read error, need not send ABORT request, for OBEX will send ABORT for the error
 *
 * Jun 9 2009 mtk80029
 * [MAUI_01866671] [BT] OBEX, GOEP,PBAP,BIP,HFP: replace "Report" trace with kal_trace
 * 
 *
 * Mar 12 2009 mtk80029
 * [MAUI_01406911] [Bluetooth] Btadp folder restruct activity
 * 
 *
 * Mar 12 2009 mtk80029
 * [MAUI_01406911] [Bluetooth] Btadp folder restruct activity
 * 
 *
 * Jan 12 2009 mtk80029
 * [MAUI_01496803] MTK:Bluetooth_it handset always show "Connecting" screen
 * let responder cntx struct has its own xml parsing result member
 *
 * Jan 9 2009 mtk80029
 * [MAUI_01496380] MTK:Bluetooth_[1] Assert fail: bt_adp_bip.c 1208 - BT
 * 
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
 * Nov 10 2008 mbj06032
 * [MAUI_01222538] MTK:Bluetooth_handset popup "fail to open file"
 * see fs error FS_MSDC_READ_SECTOR_ERROR as FS_MEDIA_CHANGED
 *
 * Oct 24 2008 mbj06032
 * [MAUI_01327697] [BT]can' t refresh file in BIP profile
 * delete the tmp xml file after parsing it
 *
 * Oct 23 2008 mbj06032
 * [MAUI_01327004] [BIP ADP] remove build warning
 * 
 *
 * Oct 16 2008 mbj06032
 * [MAUI_01123702] [BT]Remind "Service unavailable"
 * If empty, then need not check the imagelist file
 *
 * Oct 6 2008 mbj06032
 * [MAUI_01117427] [BT BIP] code revisement
 * 
 *
 * Sep 24 2008 mbj06032
 * [MAUI_01116233] [BT] Check received DCFs: if no Rights info, delete it
 * check __DRM_SUPPORT__ macro
 * 
 *
 * Sep 24 2008 mbj06032
 * [MAUI_00789020] BT-BIP problem
 * modify trace
 *
 * Sep 23 2008 mbj06032
 * [MAUI_00789020] BT-BIP problem
 * to save fs quota, composing image list xml is moved to MMI
 *
 * Sep 23 2008 mbj06032
 * [MAUI_01116233] [BT] Check received DCFs: if no Rights info, delete it
 * 
 *
 * Sep 22 2008 mbj06032
 * [MAUI_01115888] [BT BIP] Modify image descripter XML composing when sending image file:  WBMP type s
 * 
 *
 * Sep 17 2008 mbj06032
 * [MAUI_01113995] [bt]File size display error
 * 
 *
 * Sep 8 2008 mbj06032
 * [MAUI_01111918] [BT]The prompt is abnormal.
 * check FS_WRITE_PROTECTION fs error
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106048] [BT BIP] send pushed file detail info to MMI
 * 
 *
 * Aug 15 2008 mbj06032
 * [MAUI_01101632] [BT BIP] revise thumbnail request code of BIP PUSH server
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_01091714] [BT BIP] server show connect confirm when RFCOMM Open Indication
 * notify server when  RFCOMM Open Ind to let user confirm:
 *
 * Jun 27 2008 mbj06032
 * [MAUI_01081533] [BT]Should not display 0% when file not found.
 * 
 *
 * Jun 20 2008 mbj06032
 * [MAUI_01079270] [BT]Prompt "Quota usage warning" when power on handset.
 * delete the tmp file when disconnected
 *
 * Jun 6 2008 mbj06032
 * [MAUI_01066439] [BT BIP] revise code
 * 
 *
 * Jun 3 2008 mbj06032
 * [MAUI_01062984] [BT BIP] fix send-image error
 * 
 *
 * Jun 2 2008 mbj06032
 * [MAUI_01144797] MTK:Bluetooth_the popup message not play tone
 * 
 *
 * May 29 2008 mbj06032
 * [MAUI_01145940] MTK:Bluetooth_can't connect BIP
 * when fs quota full, feedback MMI fs quota full error code
 *
 * May 29 2008 mbj06032
 * [MAUI_01144776] MTK:Bluetooth _image browser screen can't view image
 * 
 *
 * May 29 2008 mbj06032
 * [MAUI_01144776] MTK:Bluetooth _image browser screen can't view image
 * When xml parsing failed, close the file handle
 *
 * May 26 2008 mbj06032
 * [MAUI_01047712] [BT]Should not popup "Unfinished."
 * When initiator receive ABORTED event, check if the initiator send the ABORT request itself. If so, use the i_err_code. It may be fs error.
 *
 * May 26 2008 mbj06032
 * [MAUI_01047668] [BT BIP] code revise
 * add "data_len" member to bip_get_image_cnf msg
 *
 * May 22 2008 mbj06032
 * [MAUI_00991010] MTK:Bluetooth BIP_can't disconnect BIP
 * BIP: add session role member(RESPONDER or INITIATOR) to DISCONNECT_IND msg & DISCONNECT_REQ msg
 *
 * May 15 2008 mbj06032
 * [MAUI_00259549] [BIP]_29 handset connect M8223 BIP handset popup "unfinished"
 * support Partial Content rsp code
 *
 * May 14 2008 mbj06032
 * [MAUI_01038940] [BT]Should show the same prompt.
 * FS_MSDC_MOUNT_ERROR, FS_MSDC_NOT_PRESENT & FS_MSDC_PRESNET_NOT_READY are also considered as "drive not present"
 *
 * May 6 2008 mbj06032
 * [MAUI_01035146] [BT][1]Assert Fail: 0 bt_adp_bip.c 880 -BT.
 * call BIP_TpDisconnect instead of GOEP_TpDisconnect
 *
 * Apr 29 2008 mbj06032
 * [MAUI_00678106] [BT BIP] fix code error
 * 
 *
 * Apr 25 2008 mbj06032
 * [MAUI_00669575] [BT BIP]Revise BIP code and modify trace info
 * 
 *
 * Apr 8 2008 Mbj06038
 * [MAUI_00652170] Remove compile warning of BIP adp
 * 
 *
 * Apr 3 2008 mbj06032
 * [MAUI_00291669] MTK:Bluetooth BIP_the both popup message different
 * for fs error code: also check fs error code between FS_MSDC_MOUNT_ERROR and FS_MSDC_NOT_PRESENT
 *
 * Mar 31 2008 mbj06032
 * [MAUI_00102208] Failed to open file pop up appears.
 * assign initial value to a variable
 *
 * Mar 25 2008 mbj06032
 * [MAUI_00285864] the both popup message different
 * modify bip_adp_check_fs_error_code func
 *
 * Feb 25 2008 mbj06032
 * [MAUI_00084141] Unable to delete JPEG files from Memory card.
 * add BIP trace info
 *
 * Feb 22 2008 mbj06032
 * [MAUI_00084141] Unable to delete JPEG files from Memory card.
 * if btmtk_fs_close return FS_DEVICE_BUSY, then start timer to close the file
 *
 * Feb 22 2008 mbj06032
 * [MAUI_00514127] [BT]Should popup "Disk full"  after display receiving.
 * When the first data packet is received, if btmtk_fs_open return BTMTK_FS_PATH_NOT_FOUND, then create the folder first.
 *
 * Jan 24 2008 mtk00511
 * [MAUI_00258835] MTK BT profile supported on MT6611_ [1] Assert fail: 0 bt_adp_bip.c 820 - BT
 * 
 *
 * Jan 7 2008 mtk00511
 * [MAUI_00594672] [BT_ADP][BIP]Remove redudant trace and global variables
 * 
 *
 * Dec 7 2007 mtk00511
 * [MAUI_00498866] [BT]Should delete the bip_i.tmp file
 * 
 *
 * Dec 3 2007 mtk00511
 * [MAUI_00497159] [BT]Should transform image from unsupported format to the supported format
 * 
 *
 * Dec 3 2007 mtk00511
 * [MAUI_00481330] [BT]Should remind "Disk Full"
 * 
 *
 * Nov 27 2007 mtk00511
 * [MAUI_00241898] MTK:Bluetooth BIP_[1] Assert fail: hFile >= 0 btmmibipscr.c 3530 - MMI
 * 
 *
 * Nov 27 2007 mtk00511
 * [MAUI_00495498] [BT]Should delete the bip_r.tmp file
 * 
 *
 * Nov 15 2007 mtk00511
 * [MAUI_00492171] [bt] the file size display error.
 * 
 *
 * Nov 5 2007 mtk00511
 * [MAUI_00489760] [1]Assert fail:0 bt_adp_bip.c 4419 -BT
 * 
 *
 * Nov 5 2007 mtk00511
 * [MAUI_00486278] [BT]Can't get the detail from 0B iamges.
 * 
 *
 * Nov 5 2007 mtk00511
 * [MAUI_00485323] [BT]Rename long name file error.
 * 
 *
 * Nov 1 2007 mtk00511
 * [MAUI_00487823] [1] Assert Fail: 0 bt_adp_bip.c 1641 - BT
 * 
 *
 * Oct 30 2007 mtk00511
 * [MAUI_00484512] [BT]Can't get the GIF file.
 * 
 *
 * Oct 29 2007 mtk00511
 * [MAUI_00239666] [MTK:Bluetooth BIP]Fatal Error (4): Data Abort (4e5cfc) - BT
 * 
 *
 * Oct 27 2007 mtk00511
 * [MAUI_00566141] BT-Always stay "Sending" screen
 * 
 *
 * Oct 25 2007 mtk00511
 * [MAUI_00239570] MTK:Bluetooth BIP_[1] Fatal Error (804): Buffer not available (80) - L1
 * 
 *
 * Oct 22 2007 mtk00511
 * [MAUI_00483634] [BT]It always display "Send (0%)"
 * 
 *
 * Oct 22 2007 mtk00511
 * [MAUI_00483026] [1]Assert fail:0 btbm.c 2396-BT
 * 
 *
 * Oct 16 2007 mtk00511
 * [MAUI_00561157] [Need Patch] for 07BW0742OF for BIP ADP feature
 * 
 *
 * Oct 16 2007 mtk00511
 * [MAUI_00561157] [Need Patch] for 07BW0742OF for BIP ADP feature
 * 
 *
 * Oct 15 2007 mtk00511
 * [MAUI_00480548] [1]Assert Fail rServer->object bip_sm.c 269 -BT
 * 
 *
 * Oct 15 2007 mtk00511
 * [MAUI_00559539] BT-Screen always in "Connecting "
 * 
 *
 * Oct 15 2007 mtk00511
 * [MAUI_00239162] MTK:Bluetooth BIP_Assert Fail:0 bt_adp_bip.c 899-BT
 * 
 *
 * Oct 15 2007 mtk00511
 * [MAUI_00480600] [1]Assert fail:0 bt_adp_bip.c 1214 -BT
 * 
 *
 * Oct 15 2007 mtk00511
 * [MAUI_00559613] [1]Assert fail:0 btbm.c 2396-BT
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
/* Keep this header file outside of the compile option to avoid compile warning */
// #include "kal_release.h"        /* Basic data type */
#ifdef __BT_BIP_PROFILE__
/***************************************************************************** 
* Include
*****************************************************************************/
//#include "bt_mmi.h"             /* message and module IDs */
//#include "bt_message.h"         /* enum for message IDs */
#include "stdio.h"              /* Basic C I/O functions */
#include "stdlib.h"             /* standard lib */
#include "bt_common.h"         /* Task message communiction(ILM) */
//#include "app_buff_alloc.h"     /* get_ctel_buffer */
//#include "kal_trace.h"          /* Trace Header Files */
//#include "bluetooth_trc.h"      /* bt trace definition */
//#include "uart_sw.h"            /* UART port for bluetooth_struct */
#include "bluetooth_struct.h"   /* BIP SAP */
#include "bluetooth_bipi_struct.h"
#include "bluetooth_bipr_struct.h"
#include "goep.h"               /* GOEP operations */
#include "bipi.h"                /* BIP operations */
#include "bip.h"                /* BIP operations */
//#include "bluetooth_bm_struct.h"/* CONVERT_BDADDR2ARRAY */
#include "btbm_adp.h"           /* CONVERT_BDADDR2ARRAY */
#include "bt_osal.h" //"fat_fs.h"             /* FS */
#include "bt_adp_fs.h"
#include "bt_adp_fs_ucs2.h"
#include "bt_adp_msg.h"
/* for xml_def.h */
#include "xml_def.h"            /* XML_PARSER */
#include "bt.h"
#include "btbm_adp.h"

#include "bt_adp_bip.h"         /* Basic data type */
//#include "bt_dm_struct.h"            /* BT_OBEX_BIPS_PROFILE_UUID */
#include "bluetooth_gap_struct.h" /* btbm_bd_addr_t */
#include "bluetooth_sdp_struct.h"

/***************************************************************************** 
* External Function
*****************************************************************************/
extern void CONVERT_BDADDR2ARRAY(U8 *dest, U32 lap, U8 uap, U16 nap);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);
/*
extern int btmtk_charset_utf8_to_ucs2_length_in_bytes(const kal_uint8 *raw);
extern int btmtk_charset_ucs2_to_utf8_length_in_bytes(const kal_uint8 *raw);
extern kal_int32 btmtk_chset_ucs2_to_utf8_string(kal_uint8 *dest, kal_int32 dest_size, kal_uint8 *src);
extern kal_int32 btmtk_chset_utf8_to_ucs2_string(kal_uint8 *dest, kal_int32 dest_size, kal_uint8 *src);
*/

/***************************************************************************** 
* MACROS
*****************************************************************************/
#define BIP_ADP_GET_FLAG(flag, a)   ((flag & a) == 0 ? KAL_FALSE : KAL_TRUE)
#define BIP_ADP_SET_FLAG(flag, a)   (flag |= a)

#define BIP_ADP_I_STATE_TRANS(x, y)   \
{                                   \
    bip_cntx_p->iState = (y);       \
}

#define BIP_ADP_R_STATE_TRANS(x, y)   \
{                                   \
    bip_cntx_p->rState = (y);       \
}

#define BIP_ADP_FS_DEVICE_BUSY(ret) (ret == BTMTK_FS_ERR_DEVICE_BUSY || ret == BTMTK_FS_ERR_LOCK_MUTEX_FAIL)

/***************************************************************************** 
* Globol Variable
*****************************************************************************/
bip_adp_cntx_struct bip_cntx;
bip_adp_cntx_struct *bip_cntx_p = &bip_cntx;

static const char* const bip_img_trans[] = 
{
    "",
    "strech",
    "crop",
    "fill"
};

typedef enum
{
    BT_BIP_ADP_FS_OPEN = 1,
    BT_BIP_ADP_FS_READ,
    BT_BIP_ADP_FS_WRITE,
    BT_BIP_ADP_FS_SEEK,
    BT_BIP_ADP_FS_MOVE,
    BT_BIP_ADP_FS_OTHER
}bt_bip_adp_fs_oper_enum;

#ifdef BTMTK_ON_WISESDK

static U16 BIP_ADP_ROOT_FOLDER[260];
static U16 BIP_ADP_WORK_FOLDER[260]; //     (L".\\bip\\")           //working folder
static U16 BIP_ADP_SND_FILE[260]; //        (L".\\bip\\snd.tmp")     //for sending object
static U16 BIP_ADP_RCV_FILE[260]; //        (L".\\bip\\rcv.tmp")     //for receiving object
static U16 BIP_ADP_IMG_LIST_FILE[260];  //  (L".\\bip\\img_list.tmp")     //for image list
static U16 BIP_ADP_DESCPTR_FILE[260]; //    (L".\\bip\\descptr.tmp")     //for descriptor xml file
static U16 BIP_ADP_PUSH_TMP_FILE[260];//   (L".\\bip\\bip_push.tmp")     //for incoming push tmp file

static void bip_adp_get_cur_path(void)
{
    S32 ret;

    ret = btmtk_fs_get_current_dir_ucs2(230, BIP_ADP_ROOT_FOLDER);
    if (ret < BTMTK_FS_OK)
        return;
    if (BIP_ADP_ROOT_FOLDER[ret - 1] != '\\' && BIP_ADP_ROOT_FOLDER[ret - 1] != '/')
    {
        BIP_ADP_ROOT_FOLDER[ret++] = '\\';
    }
    btmtk_wstrcpy(BIP_ADP_ROOT_FOLDER + ret, L"@btmtk\\");
    
    btmtk_wstrcpy(BIP_ADP_WORK_FOLDER, BIP_ADP_ROOT_FOLDER);
    ret += 7;

    btmtk_wstrcpy(BIP_ADP_WORK_FOLDER + ret, L"profile\\");
    ret += 8;

    btmtk_wstrcpy(BIP_ADP_SND_FILE, BIP_ADP_WORK_FOLDER);
    btmtk_wstrcpy(BIP_ADP_SND_FILE + ret, L"bip_snd.tmp");

    btmtk_wstrcpy(BIP_ADP_RCV_FILE, BIP_ADP_WORK_FOLDER);
    btmtk_wstrcpy(BIP_ADP_RCV_FILE + ret, L"bip_rcv.tmp");

    btmtk_wstrcpy(BIP_ADP_IMG_LIST_FILE, BIP_ADP_WORK_FOLDER);
    btmtk_wstrcpy(BIP_ADP_IMG_LIST_FILE + ret, L"bip_img_list.tmp");

    btmtk_wstrcpy(BIP_ADP_DESCPTR_FILE, BIP_ADP_WORK_FOLDER);
    btmtk_wstrcpy(BIP_ADP_DESCPTR_FILE + ret, L"bip_descptr.tmp");

    btmtk_wstrcpy(BIP_ADP_PUSH_TMP_FILE, BIP_ADP_WORK_FOLDER);
    btmtk_wstrcpy(BIP_ADP_PUSH_TMP_FILE + ret, L"bip_push.tmp");
}
#endif
/***************************************************************************** 
* Static Declaration
*****************************************************************************/

/* init/deinit functions */
static void bip_adp_cntx_init(void);
static kal_bool bip_adp_work_folder_init(void);
static void bip_adp_cntx_deinit(void);

/* callback functions */
static void bip_adp_initiator_event_callback(BipCallbackParms *parms);
static void bip_adp_responder_event_callback(BipCallbackParms *parms);

/* handler functions for BIP initiator */
static void bip_adp_connect_req_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_abort_req_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_command_req_hdlr(ilm_struct *ilm_ptr);

/* handler functions for BIP responder */
static void bip_adp_activate_req_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_deactivate_req_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_connect_rsp_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_command_rsp_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_authorize_rsp_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_send_final_packet_push_ind(kal_uint32 cm_conn_id, kal_uint32 obj_len, kal_uint32 data_len, U8 final_flag);


/* handler functions for both BIP initiator and responder */
static void bip_adp_continue_rsp_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_disconnect_req_hdlr(ilm_struct *ilm_ptr);

/* utility functions for BIP initiator */
static void bip_adp_deregister_initiator(void);
static void bip_adp_send_connect_cnf(kal_uint8 req_id, kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_get_capabilities_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_put_img_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code, kal_uint8* img_handler);
static void bip_adp_send_put_linked_thumbnail_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_get_img_list_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_get_img_prop_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_get_img_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code, kal_uint32 data_len);
static void bip_adp_send_get_linked_thumbnail_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_get_monitoring_img_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);
static void bip_adp_send_abort_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code);

/* utility functions for BIP responder */
static void bip_adp_send_activate_cnf(kal_uint8 req_id, kal_uint8 cnf_code);
static void bip_adp_send_deactivate_cnf(kal_uint8 req_id, kal_uint8 cnf_code);
static void bip_adp_send_authorize_ind(bt_bip_bd_addr_struct* dev_bd_addr, kal_uint8* dev_name);
static void bip_adp_send_connect_ind(kal_uint32 cm_conn_id, bt_bip_bd_addr_struct* dev_bd_addr, kal_uint8* dev_name);
static void bip_adp_send_get_capabilities_ind(kal_uint32 cm_conn_id);
static void bip_adp_send_put_img_ind(kal_uint32 cm_conn_id, BipImageReq* img_req, bt_bip_img_desc_struct* p_imginfo, U8 final_pkt_flag);
static void bip_adp_send_put_linked_thumbnail_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle, U8 final_pkt_flag);
static void bip_adp_send_get_img_list_ind(kal_uint32 cm_conn_id, BipImgListingReq* img_list_req);
static void bip_adp_send_get_img_prop_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle);
static void bip_adp_send_get_img_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle);
static void bip_adp_send_get_linked_thumbnail_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle);
static void bip_adp_send_get_monitoring_img_ind(kal_uint32 cm_conn_id, kal_uint8 store_flag);

/* utility functions for both BIP initiator and responder */
static void bip_adp_enter_usb_mode(void);
static kal_uint8 bip_adp_check_fs_error_code(int fs_error, bt_bip_adp_fs_oper_enum fs_oper);

static void bip_adp_send_continue_ind(kal_uint32 cm_conn_id, kal_uint32 obj_len, kal_uint32 data_len);
static void bip_adp_send_abort_ind(kal_uint32 cm_conn_id, kal_uint8 ind_code);
static void bip_adp_send_complete_ind(kal_uint32 cm_conn_id);
static void bip_adp_send_disconnect_ind(kal_uint32 cm_conn_id, bt_bip_session_role_enum ses_role);
static void bip_adp_send_msg(kal_uint16 msg_id, void *p_local_para);
static void bip_adp_generate_xml_capabilities(bt_bip_xml_composer_struct *composer, void* local_para);
static void bip_adp_generate_xml_img_list(bt_bip_xml_composer_struct *composer, void* local_para);
static void bip_adp_generate_xml_img_prop(bt_bip_xml_composer_struct *composer, void* local_para);
static bt_bip_session_role_enum bip_adp_find_session_with_cm_conn_id(kal_uint32 cm_conn_id);
static void bip_adp_write_buf2file(bt_bip_xml_composer_struct *composer);
static void bip_adp_xml_new_composer(bt_bip_xml_composer_struct **composer);
static void bip_adp_xml_close_composer(bt_bip_xml_composer_struct *composer);

static void bip_adp_compose_startElement(bt_bip_xml_composer_struct *composer, const char *el, const char **attr);
static void bip_adp_compose_endElement(bt_bip_xml_composer_struct *composer, const char *el);
static void bip_adp_compose_inline_attribute_element(bt_bip_xml_composer_struct *composer, const char *el, const char **attr);
static int bip_adp_generate_xml_file(int xml_type, void* rsp);
static int bip_adp_parse_xml_file(kal_uint32 xml_response_type, kal_wchar* filename, XML_PARSER_STRUCT ** p_parser);

void bt_bip_ignore_parse_dataElement(void *data, const char *el, const char *value, kal_int32 len, kal_int32 err);
static void bt_bip_parse_img_list_startElement(void *data, const char *el, const char **attr, kal_int32 err);
static void bt_bip_parse_img_list_endElement(void *data, const char *el, kal_int32 err);
static void bt_bip_parse_img_cap_startElement(void *data, const char *el, const char **attr, kal_int32 err);
static void bt_bip_parse_img_cap_endElement(void *data, const char *el, kal_int32 err);
static void bt_bip_parse_img_prop_startElement(void *data, const char *el, const char **attr, kal_int32 err);
static void bt_bip_parse_img_prop_endElement(void *data, const char *el, kal_int32 err);
static void bt_bip_parse_img_descriptor_startElement(void *data, const char *el, const char **attr, kal_int32 err);
static void bt_bip_parse_img_descriptor_endElement(void *data, const char *el, kal_int32 err);

static int bt_adp_create_folder(WCHAR* filepath);

static void bip_adp_start_close_file_continually(int file_handle);
void bip_adp_start_timer(eventid *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr);

void bt_adp_parsing_imglist_end(void);

void bip_adp_resume_xml_parse(ilm_struct *ilm_ptr);

static int bip_adp_parse_imagelist_xml_file_start(XML_PARSER_STRUCT ** p_xml_parser);
static void bip_adp_parse_imagelist_xml_file_end(bip_adp_xmlparse_end_enum result);

#ifdef BIP_CHECK_DRM_RIGHTS
kal_bool bt_bip_adp_check_DRM_rights(kal_uint16 *file_path);
#endif

static void bip_adp_send_challenge_ind(kal_uint32 cm_id, ObexAuthChallengeInfo* challenge, kal_uint16 msg);
static void bipr_adp_obex_auth_rsp(bt_bip_obex_auth_rsp_struct* rsp);
static void bipr_adp_obex_auth_req(bt_bip_obex_auth_req_struct* req);
static void bipi_adp_obex_auth_rsp(bt_bip_obex_auth_rsp_struct* rsp);
static void bip_adp_send_challenge_cnf(kal_uint32 cm_id, kal_uint8 result, kal_uint16 msg);
/*
static const U8 BipImagePushUUID[] = {
    0xE3, 0x3D, 0x95, 0x45, 0x83, 0x74, 0x4A, 0xD7,
    0x9E, 0xC5, 0xC1, 0x6B, 0xE3, 0x1E, 0xDE, 0x8E
};

static const U8 BipImagePullUUID[] = {
    0x8E, 0xE9, 0xB3, 0xD0, 0x46, 0x08, 0x11, 0xD5,
    0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E
};

static const U8 BipRemoteCameraUUID[] = {
    0x94, 0x7E, 0x74, 0x20, 0x46, 0x08, 0x11, 0xD5,
    0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E
};
*/

/*
static const char *pBipOp(BipOp bipOp)
{
    switch (bipOp) {
    case BIPOP_GET_CAPABILITIES:
        return "GetCapabilities XML";

    case BIPOP_GET_IMAGES_LIST:
        return "GetImagesList XML";

    case BIPOP_GET_IMAGE_PROPERTIES:
        return "GetImageProperties XML";

    case BIPOP_PUT_IMAGE:
        return "PutImage Op";

    case BIPOP_PUT_LINKED_ATTACHMENT:
        return "PutLinkedAttachment Op";

    case BIPOP_PUT_LINKED_THUMBNAIL:
        return "PutLinkedThumbnail Op";

    case BIPOP_GET_IMAGE:
        return "GetImage Op";

    case BIPOP_GET_LINKED_THUMBNAIL:
        return "GetLinkedThumbnail Op";

    case BIPOP_GET_LINKED_ATTACHMENT:
        return "GetLinkedAttachment Op";

    case BIPOP_GET_MONITORING_IMAGE:
        return "GetMonitoringImage Op";

    case BIPOP_REMOTE_DISPLAY:
    case BIPOP_DELETE_IMAGE:
    case BIPOP_START_PRINT:
    case BIPOP_START_ARCHIVE:
    case BIPOP_GETSTATUS:
    default:
        return "UNKNOWN";
        break;
    }
}


static const char *pBipInd(BipDataType indType)
{
    switch (indType) {
    case BIPIND_HANDLES_DESCRIPTOR:
        return "XML-HandlesDescriptor";

    case BIPIND_IMAGE_DESCRIPTOR:
        return "XML-ImageDescriptor";

    case BIPIND_IMG_CAPABILITIES_XML:
        return "GetCapabilities XML";

    case BIPIND_IMG_LISTING_XML:
        return "GetImagesList XML";

    case BIPIND_IMG_PROPERTIES_XML:
        return "GetImageProperties XML";

    default:
        return "UNKNOWN";
        break;
    }
}
*/

static void bip_adp_register_req_hdlr(ilm_struct *ilm_ptr);
static void bip_adp_deregister_req_hdlr(ilm_struct *ilm_ptr);
extern U8 ObexSdpDB_DeRegistration(U32 service_type);
extern U8 ObexSdpDB_Registration(U32 service_type);


/*
 * build challenge string
 */
static void bip_build_challenge_nonce(U8* nonce, U8 size)
{
    U32 i;

    for (i = 0; i < size; i++)
    {
        *nonce = 'a' + rand()%26;
        nonce++;
    }
}

/*****************************************************************************
*
* BIP ADP init function
*
*****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  bip_adp_init
 * DESCRIPTION
 *  This function is to init bip adp
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_bool ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_INIT);
    bip_adp_cntx_init();
    bip_adp_work_folder_init();
    ret = BIP_Init();
    ASSERT(ret);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_deinit
 * DESCRIPTION
 *  This function is to deinit bip adp
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_DEINIT);
    BIP_Deinit();

    /* should add all ctrl buffer free operation !!!!! */
    bip_adp_cntx_deinit();   
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_initiator_cntx_init
 * DESCRIPTION
 *  This function is to init bip adaptation initiator context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_initiator_cntx_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_CNTX_INIT);

    /* BIP ADP STATE Init */
    bip_cntx_p->iState = BT_BIP_I_STATE_IDLE;
    
    /* BIP ADP Structure Init */
    memset(&(bip_cntx_p->i_bd_addr), 0, sizeof(bt_bip_bd_addr_struct));

    memset(&(bip_cntx_p->iData), 0, sizeof(BipData));
    memset(&(bip_cntx_p->BipInitiator), 0, sizeof(BipInitiatorSession));
    memset(&(bip_cntx_p->i_xml_parsed_result), 0, sizeof(bt_bip_i_xml_parsed_info_union));

    memset(bip_cntx_p->file_path, 0, BT_BIP_MAX_PATH_LEN*BT_BIP_ENCODING_LENGTH);

    /* ID */
    bip_cntx_p->i_req_id = 0xFF;
    bip_cntx_p->i_cm_conn_id = 0xFFFFFFFF;
    bip_cntx_p->i_data_len = 0;
    bip_cntx_p->i_err_code = BT_BIP_CNF_FAILED;

    /* Pointers*/
    bip_cntx_p->i_xml_parser_p = NULL;

    /* Others */
    bip_cntx_p->i_chal_info_p = NULL;
    bip_cntx_p->i_chal_status = 0;
    memset(&bip_cntx_p->i_dig_chal, 0, sizeof(bt_bip_dig_challenge_struct));
    memset(&bip_cntx_p->i_dig_rsp, 0, sizeof(bt_bip_dig_response_struct));
    memset(&bip_cntx_p->i_obex_chal, 0, sizeof(ObexAuthChallenge));
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_responder_cntx_init
 * DESCRIPTION
 *  This function is to init bip adaptation responder context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_responder_cntx_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//  BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_CNTX_INIT);

    /* BIP ADP STATE Init */ 
    bip_cntx_p->rState = BT_BIP_R_STATE_IDLE;
    
    /* BIP ADP Structure Init */ 
    memset(&(bip_cntx_p->r_bd_addr), 0, sizeof(bt_bip_bd_addr_struct));

    memset(&(bip_cntx_p->rData), 0, sizeof(BipData));
    memset(&(bip_cntx_p->BipResponder), 0, sizeof(BipResponderSession));
    memset(&(bip_cntx_p->r_xml_parsed_result), 0, sizeof(bt_bip_r_xml_parsed_info_union));

    /* ID */
    bip_cntx_p->r_cm_conn_id = 0xFFFFFFFF;
    bip_cntx_p->r_data_len = 0;
    bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
	bip_cntx_p->r_fs_cont_timer_id = 0;

    /* Pointers*/
    bip_cntx_p->r_xml_parser_p = NULL;

    /* Others */
    bip_cntx_p->r_chal_info_p = NULL; 
    bip_cntx_p->r_chal_status = 0;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_cntx_init
 * DESCRIPTION
 *  This function is to init bip adaptation context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_cntx_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_CNTX_INIT);

    bip_adp_initiator_cntx_init();
    bip_adp_responder_cntx_init();
    bip_cntx_p->xml_composer_p = NULL;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_work_folder_init
 * DESCRIPTION
 *  This function is to initialize BIP ADP work folder
 * PARAMETERS
 *  void
 * RETURNS
 *  kal_bool
 *****************************************************************************/
kal_bool bip_adp_work_folder_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BTMTK_FS_HANDLE   h;
    kal_int32   ret;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_WORK_FOLDER_INIT);

#ifdef __NVRAM_IN_USB_MS__
    if (USBMS_BOOT == stack_query_boot_mode())
    {
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_WORK_FOLDER_INIT_USB_MS);
        return KAL_FALSE;
    }
#endif /* __NVRAM_IN_USB_MS__ */ 

#ifdef BTMTK_ON_WISESDK
    bip_adp_get_cur_path();
#endif

#if defined BTMTK_ON_WISESDK || defined BTMTK_ON_WISE
    h = btmtk_fs_open_ucs2((kal_uint16*) BIP_ADP_ROOT_FOLDER, BTMTK_FS_READ_ONLY | BTMTK_FS_OPEN_DIR);
    if (h >= BTMTK_FS_OK)
    {
        btmtk_fs_close_ucs2(h);
    }
    else
    {
        h = btmtk_fs_create_dir_ucs2((kal_uint16*)BIP_ADP_ROOT_FOLDER);
        if (h < 0)
            return KAL_FALSE;
    }
#endif

    h = btmtk_fs_open_ucs2((kal_uint16*) BIP_ADP_WORK_FOLDER, BTMTK_FS_READ_ONLY | BTMTK_FS_OPEN_DIR);
    if (h >= BTMTK_FS_OK)
    {
        btmtk_fs_close_ucs2(h);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_WORK_FOLDER_INIT_OPEN_NO_ERROR);
        return KAL_TRUE;
    }
    else    /* Error handling */
    {
        ret = btmtk_fs_create_dir_ucs2((kal_uint16*) BIP_ADP_WORK_FOLDER);
        if (ret < 0)
        {
            BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_WORK_FOLDER_INIT_OPEN_ERROR);
            return KAL_FALSE;            
        }
        return KAL_TRUE;
    }    
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_initiator_cntx_deinit
 * DESCRIPTION
 *  This function is to deinit bip adaptation initiator context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_initiator_cntx_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_I_CNTX_DEINIT);

    /* ctrl_buffer reset */
    if (bip_cntx_p->i_xml_parser_p)
    {
        free_ctrl_buffer(bip_cntx_p->i_xml_parser_p);
        bip_cntx_p->i_xml_parser_p = NULL;
    }
   
    bip_adp_initiator_cntx_init();
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_responder_cntx_deinit
 * DESCRIPTION
 *  This function is to deinit bip adaptation responder context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_responder_cntx_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//  BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_CNTX_DEINIT);

    /* ctrl_buffer reset */
    if (bip_cntx_p->r_xml_parser_p)
    {
        free_ctrl_buffer(bip_cntx_p->r_xml_parser_p);
        bip_cntx_p->r_xml_parser_p = NULL;
    }

    bip_adp_responder_cntx_init();
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_cntx_deinit
 * DESCRIPTION
 *  This function is to deinit bip adaptation context
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_cntx_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int result = 0;
       
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_CNTX_DEINIT);

    result = btmtk_fs_delete_ucs2(BIP_ADP_SND_FILE);
    if (result < BTMTK_FS_OK)
    {
        BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CNTX_DEINIT_FS_DELETE_FAILED, result);
    } 
    result = btmtk_fs_delete_ucs2(BIP_ADP_RCV_FILE);        
    if (result < BTMTK_FS_OK)
    {
        BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CNTX_DEINIT_FS_DELETE_FAILED, result);
    }
    
    bip_adp_initiator_cntx_deinit();
    bip_adp_responder_cntx_deinit();
    bip_cntx_p->xml_composer_p = NULL;
}


/*****************************************************************************
*
* BIP ADP msg handler function
*
*****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  bip_adp_msg_hdlr
 * DESCRIPTION
 *  This function is bip adp message handler 
 * PARAMETERS
 *  ilm_srtuct*
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_msg_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 msg_id = ilm_ptr->msg_id;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_MSG_HDLR, msg_id);

    switch (msg_id)
    {
        case MSG_ID_BT_BIP_CONNECT_REQ:
            bip_adp_connect_req_hdlr(ilm_ptr);
            break;
        case MSG_ID_BT_BIP_DISCONNECT_REQ:
            bip_adp_disconnect_req_hdlr(ilm_ptr);
            break;
        case MSG_ID_BT_BIP_ABORT_REQ:
            bip_adp_abort_req_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_BIP_GET_CAPABILITIES_REQ:
        case MSG_ID_BT_BIP_PUT_IMG_REQ:
        case MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_REQ:
        case MSG_ID_BT_BIP_GET_IMG_LIST_REQ:
        case MSG_ID_BT_BIP_GET_IMG_PROP_REQ:
        case MSG_ID_BT_BIP_GET_IMG_REQ:
        case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_REQ:
        case MSG_ID_BT_BIP_GET_MONITORING_IMG_REQ:
            bip_adp_command_req_hdlr(ilm_ptr);
            break;
        case MSG_ID_BT_BIP_ACTIVATE_REQ:
            bip_adp_activate_req_hdlr(ilm_ptr);
            break;
        case MSG_ID_BT_BIP_DEACTIVATE_REQ:
            bip_adp_deactivate_req_hdlr(ilm_ptr);
            break;
//20100812 mtk03036
        case MSG_ID_BT_BIPR_REGISTER_REQ:
            bip_adp_register_req_hdlr(ilm_ptr);
            break;
        case MSG_ID_BT_BIPR_DEREGISTER_REQ:
            bip_adp_deregister_req_hdlr(ilm_ptr);
            break;
//20100812 mtk03036 end 
        case MSG_ID_BT_BIP_ENTER_USB_MODE_REQ:
            bip_adp_enter_usb_mode();
            break;
        case MSG_ID_BT_BIP_CONNECT_RSP:
            bip_adp_connect_rsp_hdlr(ilm_ptr);
            break;
        case MSG_ID_BT_BIP_CONTINUE_RSP:
            bip_adp_continue_rsp_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_BIP_GET_CAPABILITIES_RSP:
        case MSG_ID_BT_BIP_PUT_IMG_RSP:
        case MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_RSP:
        case MSG_ID_BT_BIP_GET_IMG_LIST_RSP:
        case MSG_ID_BT_BIP_GET_IMG_PROP_RSP:
        case MSG_ID_BT_BIP_GET_IMG_RSP:
        case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_RSP:
        case MSG_ID_BT_BIP_GET_MONITORING_IMG_RSP:
            bip_adp_command_rsp_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_BIP_AUTHORIZE_RSP:
            bip_adp_authorize_rsp_hdlr(ilm_ptr);
            break;

        case MSG_ID_BT_BIP_RESUME_XMLPARSE_IND:
            bip_adp_resume_xml_parse(ilm_ptr);
            break;
            
        case MSG_ID_BT_BIPR_OBEX_AUTH_RSP:
            bipr_adp_obex_auth_rsp((bt_bip_obex_auth_rsp_struct*)ilm_ptr->local_para_ptr);
            break;
            
        case MSG_ID_BT_BIPI_OBEX_AUTH_RSP:
            bipi_adp_obex_auth_rsp((bt_bip_obex_auth_rsp_struct*)ilm_ptr->local_para_ptr);
            break;

        case MSG_ID_BT_BIPR_OBEX_AUTH_REQ:
            bipr_adp_obex_auth_req((bt_bip_obex_auth_req_struct*)ilm_ptr->local_para_ptr);
            break;
        default:
            ASSERT(0);
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bip_adp_connect_req_hdlr
 * DESCRIPTION
 *  This function is to handle connect request from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_connect_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    ObexTpAddr addr;
    bt_bip_connect_req_struct *msg_p = (bt_bip_connect_req_struct*) ilm_ptr->local_para_ptr;
    //BipService bip_service = 0;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY2(BT_BIP_FUNC_CONNECT_REQ_HDLR, bip_cntx_p->iState, msg_p->bip_service);

    /* check state first */
    if (bip_cntx_p->iState != BT_BIP_I_STATE_IDLE)
    {
        /* wrong state */
        BT_BIP_I_STATE(bip_cntx_p->iState);
        //Report(("Initiator Error: Initiator Connect in NOT-IDLE state.\n"));
        //ASSERT(0);
        bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);	
    }

    /* Register initiator first */
    /* initialize BIP initiaitor, and set bip_adp_initiator_event_callback as callback function) */
    status = BIP_RegisterInitiator(&(bip_cntx_p->BipInitiator), bip_adp_initiator_event_callback);
    if (status != OB_STATUS_SUCCESS) 
    {
		BT_BIP_LOG1(BT_BIP_REGINITIATOR_FAILED, status);
        //EXT_ASSERT(0, status, 0, 0);
        bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);	
    }
    else
    {
        //Report(("Initiator registered.\n"));
    }   

    /* connect printer */
    bip_cntx_p->i_bd_addr = msg_p->bd_addr;
    bip_cntx_p->i_req_id = msg_p->req_id;
    //bip_cntx_p->iService = msg_p->bip_service;

    /* call BIP connect function to establish job channel connection (DPS) */
    CONVERT_BDADDR2ARRAY((U8*)&(addr.proto.bt.addr), bip_cntx_p->i_bd_addr.lap, bip_cntx_p->i_bd_addr.uap, bip_cntx_p->i_bd_addr.nap);
    addr.type = OBEX_TP_BLUETOOTH;

    if(msg_p->bip_service == BT_BIP_IMAGE_PUSH)
    {
        bip_cntx_p->iService = BIPSVC_IMAGE_PUSH;
    }
    else if(msg_p->bip_service == BT_BIP_IMAGE_PULL)
    {
        bip_cntx_p->iService = BIPSVC_IMAGE_PULL;
    }
    else if(msg_p->bip_service == BT_BIP_REMOTE_CAMERA)
    {
        bip_cntx_p->iService = BIPSVC_REMOTE_CAMERA;
    }
    else
    {
        ASSERT(0);
    }
    
    if (msg_p->challenge.pwd_len)
    {
        memcpy(&bip_cntx_p->i_dig_chal, &msg_p->challenge, sizeof(bt_bip_dig_challenge_struct));
        if (msg_p->challenge.uid_len)
        {
            bip_cntx_p->i_obex_chal.options = AUTH_OPT_SEND_USERID;
        }
        else
        {
            bip_cntx_p->i_obex_chal.options = AUTH_OPT_DEFAULT;
        }
        if (msg_p->challenge.realm_len)
        {
            bip_cntx_p->i_obex_chal.realmLen = msg_p->challenge.realm_len;
            bip_cntx_p->i_obex_chal.realm = bip_cntx_p->i_dig_chal.realm;
        }
        bip_build_challenge_nonce(bip_cntx_p->i_nonce, 16);
        bip_cntx_p->i_obex_chal.challenge = bip_cntx_p->i_nonce;
        bip_cntx_p->i_obex_chal.challengeLen = 16;
        BIP_ClientAuthenticate(&bip_cntx_p->BipInitiator.primary, NULL, &bip_cntx_p->i_obex_chal);
    }
    else /* challenge information maybe keep, here should be clear */
    {
        memset(&bip_cntx_p->i_obex_chal, 0, sizeof(ObexAuthChallenge));
    }
    
    status = BIP_Connect(&(bip_cntx_p->BipInitiator), &addr, bip_cntx_p->iService);
		
    if (status == OB_STATUS_SUCCESS)
    {
        bip_cntx_p->i_cm_conn_id = (U32) BTBMProfileAllocateConnIndex(SC_IMAGING, addr.proto.bt.addr);
        
        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
        bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
    }
    else if (status != OB_STATUS_PENDING)
    {         
        BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CONN_REQ_HDLR_CONN_FAILED, status);

        /* send connect failed confirm to MMI */
        bip_cntx_p->i_cm_conn_id = 0xFFFFFFFF;
        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_IDLE);
        bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);

        /* deregister initiator */
        BIP_DeregisterInitiator(&(bip_cntx_p->BipInitiator));
    }
    else
    {
        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTING);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_abort_req_hdlr
 * DESCRIPTION
 *  This function is to handle abort request from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_abort_req_hdlr(ilm_struct *ilm_ptr)
{        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_ABORT_REQ_HDLR,bip_cntx_p->iState);

    /* check state first */
    switch (bip_cntx_p->iState)
    {
        /* wrong state */
        case BT_BIP_I_STATE_IDLE:
        case BT_BIP_I_STATE_CONNECTING:
        case BT_BIP_I_STATE_CONNECTED: 
            bip_adp_send_abort_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);
            return;

        case BT_BIP_I_STATE_XMLPARSING:
            /* clear data */
            bip_adp_parse_imagelist_xml_file_end(BIP_ADP_XMLPARSE_USERCANCELED); 
            
            bip_adp_send_abort_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);

            /* transfer status to CONNECTED */
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);

            return;

        /* transferring states */
        case BT_BIP_I_STATE_REQUESTING:
             memset(&(bip_cntx_p->i_xml_parsed_result), 0, sizeof(bt_bip_i_xml_parsed_info_union));
             if (bip_cntx_p->iData.ocx != 0) 
             {
                 btmtk_fs_close_ucs2(bip_cntx_p->iData.ocx);
                 bip_cntx_p->iData.ocx = 0;
             }
             BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_ABORTING);
             BIP_Abort(&(bip_cntx_p->BipInitiator), BIPCH_INITIATOR_PRIMARY, 0);
            return;

        /* ignore */
        case BT_BIP_I_STATE_ABORTING:
            bip_adp_send_abort_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);
            break;
        case BT_BIP_I_STATE_DISCONNECTING:
            bip_adp_send_abort_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);
            break;
        /* should never happens */    
        default:
            ASSERT(0);
            return;
    }    
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_command_req_hdlr
 * DESCRIPTION
 *  This function is to handle BIP command request from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_command_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 msg_id = ilm_ptr->msg_id;
    ObStatus    status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
     BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_COMMAND_REQ_HDLR, bip_cntx_p->iState);

    /* check state first */
    if ((bip_cntx_p->iState != BT_BIP_I_STATE_CONNECTED) &&
        (bip_cntx_p->iState != BT_BIP_I_STATE_IDLE))
    {
        /* wrong state */
        BT_BIP_I_STATE(bip_cntx_p->iState);
        ASSERT(0);
    }
    /* send the BIP request to BIP responder */
    else
    {    
        if (bip_cntx_p->iState == BT_BIP_I_STATE_IDLE)
        {
            return;
        }
        
        /* build BIP initiator request */
        switch(msg_id)
        {
            case MSG_ID_BT_BIP_GET_CAPABILITIES_REQ:
                bip_cntx_p->iData.bipOp = BIPOP_GET_CAPABILITIES;
                break;
            case MSG_ID_BT_BIP_PUT_IMG_REQ:
                {
                    int i =0;
                    int ret, fh, len;

                    bt_bip_put_img_req_struct *msg_p = (bt_bip_put_img_req_struct*) ilm_ptr->local_para_ptr;

                    while ((msg_p->img_name[i] != 0) || (msg_p->img_name[i+1] != 0))
                    {
						  bip_cntx_p->iData.r.rImg.name[i] = msg_p->img_name[i+1];
						  bip_cntx_p->iData.r.rImg.name[i+1] = msg_p->img_name[i];
                        i += 2;
                    }

                    ret = bip_adp_generate_xml_file(BIPOP_PUT_IMAGE, msg_p);
                    if (ret < 0)
                    {
                        bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_OPEN_FAIL, NULL);
                        return;
                    }
                    
                    fh = btmtk_fs_open_ucs2((WCHAR*)BIP_ADP_SND_FILE, BTMTK_FS_READ_ONLY);

                    if (fh < 0)
                    {
						BT_BIP_LOG1(BT_BIP_FS_OPEN_SNDFILE_FAILED, fh);	
                        bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_OPEN_FAIL, NULL);
                        return;
                    }

                    btmtk_fs_get_filesize_ucs2(fh, (kal_uint32*)&bip_cntx_p->iData.r.rImg.imageDescriptorLen);
                    bip_cntx_p->iData.r.rImg.imageDescriptor = get_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptorLen);
                    ret = btmtk_fs_read_ucs2(fh, bip_cntx_p->iData.r.rImg.imageDescriptor, bip_cntx_p->iData.r.rImg.imageDescriptorLen, &len);
                    if(ret < 0)
                    {
                        if(bip_cntx_p->iData.r.rImg.imageDescriptor != NULL)
                            free_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptor);
                        bip_cntx_p->iData.r.rImg.imageDescriptor = 0;
                        bip_cntx_p->iData.r.rImg.imageDescriptorLen = 0;

						BT_BIP_LOG1(BT_BIP_FS_READ_SNDFILE_FAILED, ret);	
                        btmtk_fs_close_ucs2(fh);
                        bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_READ_FAIL, NULL);
                        return;
                    }
                    btmtk_fs_close_ucs2(fh);

                    bip_cntx_p->iData.totLen = msg_p->img_size;
                    bip_cntx_p->iData.ocx = btmtk_fs_open_ucs2((WCHAR*)msg_p->img_path, BTMTK_FS_READ_ONLY);
                    if (bip_cntx_p->iData.ocx < 0)
                    {
                        if(bip_cntx_p->iData.r.rImg.imageDescriptor != NULL)
                            free_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptor);
                        bip_cntx_p->iData.r.rImg.imageDescriptor = 0;
                        bip_cntx_p->iData.r.rImg.imageDescriptorLen = 0;
                        bip_cntx_p->iData.ocx = 0;
						BT_BIP_LOG1(BT_BIP_FS_OPEN_PUTIMG_FAILED, ret);	
                        bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_OPEN_FAIL, NULL);
                        return;
                    }
                    bip_cntx_p->iData.bipOp = BIPOP_PUT_IMAGE;
                }
                break;
            case MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_REQ:
                {
                    bt_bip_put_linked_thumbnail_req_struct *msg_p = (bt_bip_put_linked_thumbnail_req_struct*) ilm_ptr->local_para_ptr;

                    memcpy(bip_cntx_p->iData.r.rThm.imgHandle, msg_p->img_handle, BT_BIP_IMG_HANDLE_LEN);
                    bip_cntx_p->iData.ocx = btmtk_fs_open_ucs2((WCHAR*)msg_p->img_path, BTMTK_FS_READ_ONLY);
                    if (bip_cntx_p->iData.ocx < 0)
                    {
						BT_BIP_LOG1(BT_BIP_FS_OPEN_PUTTHM_FAILED,bip_cntx_p->iData.ocx);
                        bip_cntx_p->iData.ocx = 0;
                        bip_adp_send_put_linked_thumbnail_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_OPEN_FAIL);
                        return;
                    }
                    btmtk_fs_get_filesize_ucs2(bip_cntx_p->iData.ocx, &bip_cntx_p->iData.totLen);
                    bip_cntx_p->iData.bipOp = BIPOP_PUT_LINKED_THUMBNAIL;
                }
                break;
            case MSG_ID_BT_BIP_GET_IMG_LIST_REQ:
                {
                    bt_bip_get_img_list_req_struct *msg_p = (bt_bip_get_img_list_req_struct*) ilm_ptr->local_para_ptr;

                    /* Create empty image handle descriptor */
                    bip_cntx_p->iData.r.rList.handlesDescriptor = (BipImgDescription)1; /* set to non-NULL */
                    bip_cntx_p->iData.r.rList.handlesDescriptorLen = 0; /* set to 0 to create empty image description header */
                    
                    bip_cntx_p->iData.r.rList.nbReturnedHandles = msg_p->max_img_handle_number;
                    bip_cntx_p->iData.r.rList.listStartOffset = msg_p->start_index;
                    bip_cntx_p->iData.r.rList.latestCapturedImages = (U8) msg_p->latest_captured;
                    bip_cntx_p->iData.bipOp = BIPOP_GET_IMAGES_LIST;
                }
                break;
            case MSG_ID_BT_BIP_GET_IMG_PROP_REQ:
                {
                    bt_bip_get_img_prop_req_struct *msg_p = (bt_bip_get_img_prop_req_struct*) ilm_ptr->local_para_ptr;

                    memcpy(bip_cntx_p->iData.r.rProp.imgHandle, msg_p->img_handle, BT_BIP_IMG_HANDLE_LEN);
                    bip_cntx_p->iData.bipOp = BIPOP_GET_IMAGE_PROPERTIES;
                }
                break;
            case MSG_ID_BT_BIP_GET_IMG_REQ:
                {
                    int ret, fh, len;                    
                    bt_bip_get_img_req_struct *msg_p = (bt_bip_get_img_req_struct*) ilm_ptr->local_para_ptr;

                    if(msg_p->img_descriptor.encoding != BT_BIP_IMG_TYPE_UNSUPPORT)
                    {
                        ret = bip_adp_generate_xml_file(BIPOP_GET_IMAGE, msg_p);
                        if (ret < 0)
                        {
                            bip_adp_send_get_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_XML_GEN_FAIL, 0);
                            return;
                        }
                    
                        fh = btmtk_fs_open_ucs2((WCHAR*)BIP_ADP_SND_FILE, BTMTK_FS_READ_ONLY);

                        if (fh < 0)
                        {
							BT_BIP_LOG1(BT_BIP_GETIMGREQ_FS_OPEN_FAILED,fh);	
                            bip_adp_send_get_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_OPEN_FAIL, 0);
                            return;
                        }

                        btmtk_fs_get_filesize_ucs2(fh, (kal_uint32*)&bip_cntx_p->iData.r.rImg.imageDescriptorLen);
                        bip_cntx_p->iData.r.rImg.imageDescriptor = get_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptorLen);
                        ret = btmtk_fs_read_ucs2(fh, bip_cntx_p->iData.r.rImg.imageDescriptor, bip_cntx_p->iData.r.rImg.imageDescriptorLen, &len);
                        if(ret < 0)
                        {
                            if(bip_cntx_p->iData.r.rImg.imageDescriptor != NULL)
                                free_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptor);
                            bip_cntx_p->iData.r.rImg.imageDescriptor = 0;
                            bip_cntx_p->iData.r.rImg.imageDescriptorLen = 0;
							BT_BIP_LOG1(BT_BIP_GETIMGREQ_FS_READ_FAILED,ret);	
                            btmtk_fs_close_ucs2(fh);
                            bip_adp_send_get_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_FILE_READ_FAIL, 0);
                            return;
                        }
                        btmtk_fs_close_ucs2(fh);
                    }
                    else
                    {
                        /* Create empty image descriptor */
                        bip_cntx_p->iData.r.rImg.imageDescriptor = (BipImgDescription)1; /* set to non-NULL */
                        bip_cntx_p->iData.r.rImg.imageDescriptorLen = 0; /* set to 0 to create empty image description header */
                    }
                    ASSERT((msg_p->img_path[0] != 0) || (msg_p->img_path[1] != 0));
                    btmtk_wstrcpy((WCHAR*)bip_cntx_p->file_path, (WCHAR*)msg_p->img_path);
                    memcpy(bip_cntx_p->iData.r.rImg.imgHandle, msg_p->img_handle, BT_BIP_IMG_HANDLE_LEN);
                    bip_cntx_p->iData.bipOp = BIPOP_GET_IMAGE;
                }
                break;
            case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_REQ:
                {
                    bt_bip_get_linked_thumbnail_req_struct *msg_p = (bt_bip_get_linked_thumbnail_req_struct*) ilm_ptr->local_para_ptr;

                    ASSERT((msg_p->img_path[0] != 0) || (msg_p->img_path[1] != 0));
                    btmtk_wstrcpy((WCHAR*)bip_cntx_p->file_path, (WCHAR*)msg_p->img_path);
                    memcpy(bip_cntx_p->iData.r.rThm.imgHandle, msg_p->img_handle, BT_BIP_IMG_HANDLE_LEN);
                    bip_cntx_p->iData.bipOp = BIPOP_GET_LINKED_THUMBNAIL;
                }
                break;
            case MSG_ID_BT_BIP_GET_MONITORING_IMG_REQ:
                {
                    bt_bip_get_monitoring_img_req_struct *msg_p = (bt_bip_get_monitoring_img_req_struct*) ilm_ptr->local_para_ptr;

                    ASSERT((msg_p->img_path[0] != 0) || (msg_p->img_path[1] != 0));
                    btmtk_wstrcpy((WCHAR*)bip_cntx_p->file_path, (WCHAR*)msg_p->img_path);
                    bip_cntx_p->iData.bipOp = BIPOP_GET_MONITORING_IMAGE;
                    bip_cntx_p->iData.r.rMon.storeFlag = (U8) msg_p->store_flag;
                }
                break;
            default:
                ASSERT(0);
        }
        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_REQUESTING);
        status = BIP_SendRequest(&(bip_cntx_p->BipInitiator), &(bip_cntx_p->iData));
        if(status != OB_STATUS_PENDING)
        {            
            BT_BIP_LOG1(BT_BIP_SENDBIPREQ_FAILD, status);
            EXT_ASSERT(0, status, 0, 0);
        }               
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_activate_req_hdlr
 * DESCRIPTION
 *  This function is to handle activate request from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_activate_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    bt_bip_activate_req_struct *msg_p = (bt_bip_activate_req_struct*) ilm_ptr->local_para_ptr;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_ACTIVATE_REQ_HDLR, bip_cntx_p->rState);

    /* check state first */
    if (bip_cntx_p->rState != BT_BIP_R_STATE_IDLE)
    {
        /* wrong state */
        BT_BIP_R_STATE(bip_cntx_p->rState);
        ASSERT(0);
    }        
    
    /* Register responder first */
    /* initialize BIP responder, and set bip_adp_responder_event_callback as callback function) */

    status = BIP_RegisterResponder(&(bip_cntx_p->BipResponder), bip_adp_responder_event_callback);

    if (status != OB_STATUS_SUCCESS) 
    {
        bip_adp_send_activate_cnf(msg_p->req_id, BT_BIP_CNF_FAILED);
        EXT_ASSERT(0, status, 0, 0);
    }
    else
    {
        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACTIVATED);
        bip_adp_send_activate_cnf(msg_p->req_id, BT_BIP_CNF_SUCCESS);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_deactivate_req_hdlr
 * DESCRIPTION
 *  This function is to handle deactivate request from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_deactivate_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BtStatus status;
    bt_bip_deactivate_req_struct *msg_p = (bt_bip_deactivate_req_struct*) ilm_ptr->local_para_ptr;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_DEACTIVATE_REQ_HDLR,bip_cntx_p->rState);

    /* check state first */
    if (bip_cntx_p->rState == BT_BIP_R_STATE_IDLE)
    {
        /* wrong state */
        BT_BIP_R_STATE(bip_cntx_p->rState);
        //Report(("Responder Error: Responder is deactivated in IDLE state.\n"));
        ASSERT(0);
    }

    /* Deregister responder */
    status = BIP_DeregisterResponder(&(bip_cntx_p->BipResponder));
    if (status != OB_STATUS_SUCCESS)
    {
        bip_adp_send_deactivate_cnf(msg_p->req_id, BT_BIP_CNF_FAILED);
        //Report(("Responder Error: Could not deinitiaze Responder."));
        EXT_ASSERT(0, status, 0, 0);
    }
    else
    {
        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_IDLE);
        bip_adp_send_deactivate_cnf(msg_p->req_id, BT_BIP_CNF_SUCCESS);
    }
}
//20100812 mtk03036
void bip_adp_register_req_hdlr(ilm_struct *ilm_ptr)
{
    U8 status;
    bt_sdpdb_register_cnf_struct *rsp_msg;

    status = ObexSdpDB_Registration(((bt_sdpdb_register_req_struct*) ilm_ptr->local_para_ptr)->uuid);
    kal_trace(TRACE_GROUP_1, REGISTRATION_RESULTx02X, status);
    rsp_msg =  (bt_sdpdb_register_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_register_cnf_struct), TD_UL);
    rsp_msg->uuid = ((bt_sdpdb_register_req_struct*) ilm_ptr->local_para_ptr)->uuid;
    rsp_msg->result = status;

    BT_SendMessage(MSG_ID_BT_BIPR_REGISTER_CNF, MOD_EXTERNAL, rsp_msg, ((local_para_struct*)rsp_msg)->msg_len);
}

void bip_adp_deregister_req_hdlr(ilm_struct *ilm_ptr)
{
    U8 status;
    bt_sdpdb_register_cnf_struct *rsp_msg;

    status = ObexSdpDB_DeRegistration(((bt_sdpdb_deregister_req_struct*) ilm_ptr->local_para_ptr)->uuid);
    kal_trace(TRACE_GROUP_1, DEREGISTRATION_RESULTx02X, status);
    rsp_msg = (bt_sdpdb_register_cnf_struct*) construct_local_para(sizeof(bt_sdpdb_deregister_cnf_struct), TD_UL);
    rsp_msg->uuid = ((bt_sdpdb_deregister_req_struct*) ilm_ptr->local_para_ptr)->uuid;
    rsp_msg->result = status;

    BT_SendMessage(MSG_ID_BT_BIPR_DEREGISTER_CNF, MOD_EXTERNAL, rsp_msg, ((local_para_struct*)rsp_msg)->msg_len);
}
//20100812 mtk03036 end

/*****************************************************************************
 * FUNCTION
 *  bip_adp_authorize_rsp_hdlr
 * DESCRIPTION
 *  This function is to handle connect command response from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_authorize_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_authorize_rsp_struct *rsp = (bt_bip_authorize_rsp_struct*) ilm_ptr->local_para_ptr;
    kal_uint8 rsp_code;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CONNECT_RSP_HDLR,bip_cntx_p->rState);

    /* check state first */
    if (bip_cntx_p->rState != BT_BIP_R_STATE_AUTHORIZING)
    {
        /* wrong state */
        BT_BIP_R_STATE(bip_cntx_p->rState);
        /* This is a possible scenario */
        //Report(("Responder Error: Responder send connect response in NOT-RESPONDER_ACCEPTING state.\n"));
    }
    else
    {
        if(rsp->cnf_code == BT_BIP_CNF_SUCCESS)
        {
            rsp_code = GOEP_STATUS_SUCCESS;
            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_AUTHORIZED);
        }
        else
        {
            rsp_code = GOEP_STATUS_FAILED;
			BT_BIP_LOG1(BT_BIP_MMI_CONNECT_RSP_FAILED,rsp->cnf_code);	
            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACTIVATED);
        }
        BIP_ServerAuthorizeResp(&(bip_cntx_p->BipResponder.primary), rsp_code); 
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_connect_rsp_hdlr
 * DESCRIPTION
 *  This function is to handle connect command response from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_connect_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_connect_rsp_struct *rsp = (bt_bip_connect_rsp_struct*) ilm_ptr->local_para_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CONNECT_RSP_HDLR,bip_cntx_p->rState);

    /* check state first */
    if (bip_cntx_p->rState != BT_BIP_R_STATE_ACCEPTING)
    {
        /* wrong state */
        BT_BIP_R_STATE(bip_cntx_p->rState);
        /* This is a possible scenario */
        //Report(("Responder Error: Responder send connect response in NOT-RESPONDER_ACCEPTING state.\n"));
    }
    else
    {
        if(rsp->cnf_code == BT_BIP_CNF_SUCCESS)
        {
            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
        }
        else
        {
			BT_BIP_LOG1(BT_BIP_MMI_CONNECT_RSP_FAILED,rsp->cnf_code);	
            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACTIVATED);
            BIP_Abort(&(bip_cntx_p->BipResponder), BIPCH_RESPONDER_PRIMARY, rsp->cnf_code);
        }
        BIP_ResponderContinue(&(bip_cntx_p->BipResponder.primary)); 
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_command_rsp_hdlr
 * DESCRIPTION
 *  This function is to handle BIP command response from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_command_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint32 msg_id = ilm_ptr->msg_id;
    BipData *rData = bip_cntx_p->rData_p;
    BipObexServer* rServer = &(bip_cntx_p->BipResponder.primary);
    kal_uint16 img_path[BT_BIP_MAX_PATH_LEN];
    int     ret;
    kal_uint8 abort_reason = GOEP_SERVICE_UNAVAILABLE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_COMMAND_RSP_HDLR,bip_cntx_p->rState);

    /* check state first */
    BT_BIP_R_STATE(bip_cntx_p->rState);
    if (bip_cntx_p->rState == BT_BIP_R_STATE_INDICATING)
    {
        /* build BIP responder response */
        switch(msg_id)
        {
            case MSG_ID_BT_BIP_PUT_IMG_RSP:
                {
                    bt_bip_put_img_rsp_struct* rsp = (bt_bip_put_img_rsp_struct*) ilm_ptr->local_para_ptr;

                    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS || rsp->cnf_code == BT_BIP_PARTIAL_CONTENT)
                    {
                        btmtk_wstrcpy((WCHAR*)img_path, (const WCHAR*)rsp->img_path);
                        
                        if(rData->ocx != 0)
                        {
                            int result;
                            btmtk_fs_close_ucs2(rData->ocx);
                            rData->ocx = 0;
                            result = btmtk_fs_move_file_ucs2(BIP_ADP_PUSH_TMP_FILE, (const WCHAR*)img_path,  BTMTK_FS_MOVE_KILL | BTMTK_FS_MOVE_OVERWRITE);
                            if(result < 0)
                            {
                                bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(result, BT_BIP_ADP_FS_MOVE);
                                goto BIP_ABORT;
                            }
                            rData->ocx = btmtk_fs_open_ucs2((WCHAR*)img_path, BTMTK_FS_READ_WRITE);
							if(rData->ocx >= 0)
							{
                                btmtk_fs_seek_ucs2(rData->ocx, 0, BTMTK_FS_SEEK_END);
                            }
                        }
                        else
                        {
                            rData->ocx = btmtk_fs_open_ucs2((WCHAR*)img_path, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
                        }
                        if(rData->ocx < 0)
                        {
                            rData->ocx = 0;
                            bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(rData->ocx, BT_BIP_ADP_FS_OPEN);
                            goto BIP_ABORT;
                        }
                    
#ifdef BIP_CHECK_DRM_RIGHTS   
                        btmtk_wstrcpy((WCHAR*)bip_cntx_p->r_recv_path, (const WCHAR*)img_path);
#endif
                    
                        memcpy(rData->r.rImg.imgHandle, rsp->img_handle, BT_BIP_IMG_HANDLE_LEN);

                        if (rsp->cnf_code == BT_BIP_PARTIAL_CONTENT)
                        {
                            BIP_ResponderAccept(rServer, OBRC_PARTIAL_CONTENT, rData);
                        }
                        else if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
                        {
                            BIP_ResponderAccept(rServer, OBRC_ACCEPTED, rData);
                        }
                    }
                    else
                    {
                        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
                        abort_reason = rsp->cnf_code;
                        goto BIP_ABORT;
                    }

                }
                break;
            case MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_RSP:
                {
                    bt_bip_put_linked_thumbnail_rsp_struct* rsp = (bt_bip_put_linked_thumbnail_rsp_struct*) ilm_ptr->local_para_ptr;
                    /* Convert handle to a filename and open file */
                    if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
                    {
                        if (rData->r.rThm.imgHandle[0] != 0)
                        {
                            btmtk_wstrcpy((WCHAR*)img_path, (const WCHAR*)rsp->img_path);
							if(rData->ocx != 0)
							{
								int result;
								btmtk_fs_close_ucs2(rData->ocx);
								result = btmtk_fs_move_file_ucs2(BIP_ADP_PUSH_TMP_FILE, (const WCHAR*)img_path,  BTMTK_FS_MOVE_KILL | BTMTK_FS_MOVE_OVERWRITE);
								if(result < 0)
								{
									bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(result, BT_BIP_ADP_FS_MOVE);
									
									goto BIP_ABORT;
								}
								rData->ocx = btmtk_fs_open_ucs2((WCHAR*)img_path, BTMTK_FS_READ_WRITE);
								if(rData->ocx >= 0)
								{
									btmtk_fs_seek_ucs2(rData->ocx, 0, BTMTK_FS_SEEK_END);
								}
							}
							else
							{
                                rData->ocx = btmtk_fs_open_ucs2((WCHAR*)img_path, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
							}

                            if (rData->ocx < 0) 
                            {
                                rData->ocx = 0;
                                bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(rData->ocx, BT_BIP_ADP_FS_OPEN);
								
                                goto BIP_ABORT;
                            }
                            else
                            {
                                BIP_ResponderAccept(rServer, OBRC_ACCEPTED, rData);
                            }
                        }
                        else
                        {
                            bip_cntx_p->r_err_code = BT_BIP_INVALID_PARM;
                            goto BIP_ABORT;
                        }
                    }   
                    else
                    {
                        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
                        goto BIP_ABORT;
                    }
                }
                break;
            case MSG_ID_BT_BIP_GET_CAPABILITIES_RSP:
                {
                    bt_bip_get_capabilities_rsp_struct* rsp = (bt_bip_get_capabilities_rsp_struct*) ilm_ptr->local_para_ptr;

                    if(rsp->cnf_code == BT_BIP_CNF_SUCCESS)
                    {
                        ret = bip_adp_generate_xml_file(BIPOP_GET_CAPABILITIES, rsp);
                        if (ret < 0)
                        {
							kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_GEN_XML_FAILED,ret);
                            goto BIP_ABORT;
                        }
                        rData->ocx = btmtk_fs_open_ucs2(BIP_ADP_SND_FILE, BTMTK_FS_READ_ONLY);
                        if (rData->ocx <= 0)
                        {
                            bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(rData->ocx, BT_BIP_ADP_FS_OPEN);
							rData->ocx = 0;
                            goto BIP_ABORT;
                        }
                    }
                    else
                    {
                        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
                        abort_reason = rsp->cnf_code;
                        goto BIP_ABORT;
                    }
                }
                break;
            case MSG_ID_BT_BIP_GET_IMG_LIST_RSP:
                {
                    bt_bip_get_img_list_rsp_struct* rsp = (bt_bip_get_img_list_rsp_struct*) ilm_ptr->local_para_ptr;

                    rData->r.rList.nbReturnedHandles = rsp->img_count;
                    /* Create empty image handle descriptor */
                    rData->r.rList.handlesDescriptor = (BipImgDescription)1; /* set to non-NULL */
                    rData->r.rList.handlesDescriptorLen = 0; /* set to 0 to create empty image description header */

                    if(rsp->cnf_code == BT_BIP_CNF_SUCCESS || rsp->cnf_code == BT_BIP_PARTIAL_CONTENT)
                    {
                        
#ifdef __COMPOSE_IMGLIST_XML_IN_ADP__                        
                        ret = bip_adp_generate_xml_file(BIPOP_GET_IMAGES_LIST, rsp);
                        if (ret < 0)
                        {
							kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_GEN_XML_FAILED,ret);
							
                            goto BIP_ABORT;
                        }
                        rData->ocx = btmtk_fs_open_ucs2(BIP_ADP_SND_FILE, BTMTK_FS_READ_ONLY);
#else
                        rData->ocx = btmtk_fs_open_ucs2((WCHAR*)rsp->img_list_path, BTMTK_FS_READ_ONLY);
#endif
                        /* if img_count == 0, then need not check the imagelist file */
                        if ((rsp->img_count > 0) && (rData->ocx <= 0))
                        {
                            if (rData->ocx < 0)
                                rData->ocx = 0;
                            bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(rData->ocx, BT_BIP_ADP_FS_OPEN);

                            goto BIP_ABORT;
                        }

                        if (rsp->cnf_code == BT_BIP_PARTIAL_CONTENT)
                        {
                            BIP_ResponderAccept(rServer, OBRC_PARTIAL_CONTENT, rData);
                        }
                        else if (rsp->cnf_code == BT_BIP_CNF_SUCCESS)
                        {
                            BIP_ResponderAccept(rServer, OBRC_ACCEPTED, rData);
                        }
                        
                    }
                    else
                    {
                        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
                        abort_reason = rsp->cnf_code;
                        goto BIP_ABORT;
                    }
                }
                break;
            case MSG_ID_BT_BIP_GET_IMG_PROP_RSP:
                {
                    bt_bip_get_img_prop_rsp_struct* rsp = (bt_bip_get_img_prop_rsp_struct*) ilm_ptr->local_para_ptr;

                    if(rsp->cnf_code == BT_BIP_CNF_SUCCESS)
                    {
                        ret = bip_adp_generate_xml_file(BIPOP_GET_IMAGE_PROPERTIES, rsp);
                        if (ret < 0)
                        {
							kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_GEN_XML_FAILED,ret);
							
                            goto BIP_ABORT;
                        }
                        rData->ocx = btmtk_fs_open_ucs2(BIP_ADP_SND_FILE, BTMTK_FS_READ_ONLY);
                        if (rData->ocx < 0)
                        {
                            rData->ocx = 0;
                            bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(rData->ocx, BT_BIP_ADP_FS_OPEN);
                            goto BIP_ABORT;
                        }
                    }
                    else
                    {
                        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
                        abort_reason = rsp->cnf_code;
                        goto BIP_ABORT;
                    }
                }
                break;
            case MSG_ID_BT_BIP_GET_IMG_RSP:
            case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_RSP:
            case MSG_ID_BT_BIP_GET_MONITORING_IMG_RSP:
                {
                    bt_bip_get_img_rsp_struct* rsp = (bt_bip_get_img_rsp_struct*) ilm_ptr->local_para_ptr;
                    if (rsp->img_handle[0] != 0)
                    {
                        memcpy(rData->r.rImg.imgHandle, rsp->img_handle, BT_BIP_IMG_HANDLE_LEN);
                    }
                    if(rsp->cnf_code == BT_BIP_CNF_SUCCESS)
                    {
                        rData->ocx = btmtk_fs_open_ucs2((WCHAR*) rsp->img_path, BTMTK_FS_READ_ONLY);
                        if (rData->ocx <= 0)
                        {
                            rData->ocx = 0;
                            bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(rData->ocx, BT_BIP_ADP_FS_OPEN);
                            goto BIP_ABORT;
                        }
                    }
                    else
                    {
                        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
                        abort_reason = rsp->cnf_code;

                        goto BIP_ABORT;
                    }
                }
                break;

            default:
                ASSERT(0);
        } 
        btmtk_fs_get_filesize_ucs2(rData->ocx, &rData->totLen);
        rData->bipOp = rServer->request.bipOp;
        BIP_SendResponse(rServer, rData);
        switch(msg_id)
        {
            case MSG_ID_BT_BIP_GET_IMG_RSP:
            case MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_RSP:
            case MSG_ID_BT_BIP_GET_MONITORING_IMG_RSP:
                GOEP_SetPullObjectLength(&rServer->sApp, rData->totLen);
        }
        GOEP_ServerAccept(&rServer->sApp, rServer->object);
        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_RESPONDING);        
        BIP_ResponderContinue(rServer);
    }
    return;
BIP_ABORT:
    //bip_adp_send_abort_ind(bip_cntx_p->r_cm_conn_id, bip_cntx_p->r_err_code);
    //bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED; /* Reset the error code to failed */
    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
    BIP_Abort(&(bip_cntx_p->BipResponder), BIPCH_RESPONDER_PRIMARY, abort_reason);
    BIP_ResponderContinue(&(bip_cntx_p->BipResponder.primary));
    if(rData->ocx != 0)
    {
        btmtk_fs_close_ucs2(rData->ocx);
        rData->ocx = 0;
    }
    if(rServer->object)
    {
        BIPOBS_Close(&(rServer->object));
    }
    btmtk_fs_delete_ucs2(BIP_ADP_PUSH_TMP_FILE); 
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_disconnect_req_hdlr
 * DESCRIPTION
 *  This function is to handle disconnect request from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_disconnect_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/   
    bt_bip_disconnect_req_struct *msg_p = (bt_bip_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    bt_bip_session_role_enum session_role;
    ObStatus status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_DISCONNECT_REQ_HDLR);

    /* To check the disconnect request comes from initiator or responder */
    //session_role = bip_adp_find_session_with_cm_conn_id(msg_p->cm_conn_id);
    session_role = msg_p->session_role;

    switch(session_role)
    {
        case BT_BIP_INITIATOR_ROLE:
            BT_BIP_I_STATE(bip_cntx_p->iState);
            /* check state first */
            switch (bip_cntx_p->iState)
            {
                /* wrong state */
                case BT_BIP_I_STATE_CONNECTING:
                    if(msg_p->disconnect_tp_directly == KAL_TRUE)
                    {
                        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_DISCONNECTING);
                	goto BIP_I_DISCONNECT;
                    }
                    else
                    {
						kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_I_DISCONNECT_REQ_IN_WRONG_STATE,bip_cntx_p->iState);
                        return;
                    }

                /* race condition */
                case BT_BIP_I_STATE_IDLE:
                    /* sent disconnect indication to MMI */
                    return;

                /* connected states */
                case BT_BIP_I_STATE_XMLPARSING:
                    /* clear data */
                    bip_adp_parse_imagelist_xml_file_end(BIP_ADP_XMLPARSE_USERCANCELED); 
                    /* fall through to the following */                    
                case BT_BIP_I_STATE_CONNECTED:
                    BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_DISCONNECTING);
                    goto BIP_I_DISCONNECT;

                /* transferring states */
                case BT_BIP_I_STATE_REQUESTING:
                /* receive disconnect req from MMI -> error */
                case BT_BIP_I_STATE_ABORTING:
                case BT_BIP_I_STATE_DISCONNECTING:
                    if(msg_p->disconnect_tp_directly == KAL_TRUE)
                	{
                        goto BIP_I_DISCONNECT;
                    }
                    else
                    {
						kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_I_DISCONNECT_REQ_IN_WRONG_STATE,bip_cntx_p->iState);
                        return;
                    }
                /* should never happens */    
                default:
					kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_I_DISCONNECT_REQ_IN_WRONG_STATE,bip_cntx_p->iState);
                    ASSERT(0);
                    return;
            }    
            break;        
        case BT_BIP_RESPONDER_ROLE:
            BT_BIP_R_STATE(bip_cntx_p->rState);
            /* check state first */
            switch (bip_cntx_p->rState)
            {
                /* wrong state */
                case BT_BIP_R_STATE_IDLE:
                case BT_BIP_R_STATE_ACTIVATING:
					kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_R_DISCONNECT_REQ_IN_WRONG_STATE,bip_cntx_p->rState);
                    return;

                /* race condition */
                case BT_BIP_R_STATE_ACTIVATED:
                    return;

                /* connected states */
                case BT_BIP_R_STATE_AUTHORIZING:
                case BT_BIP_R_STATE_AUTHORIZED:
                case BT_BIP_R_STATE_ACCEPTING:
                case BT_BIP_R_STATE_ACCEPTED:
                case BT_BIP_R_STATE_INDICATING:
                case BT_BIP_R_STATE_RESPONDING:
                case BT_BIP_R_STATE_ABORTING:
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_DISCONNECTING);
                    goto BIP_R_DISCONNECT;

                /* transferring states */
                case BT_BIP_R_STATE_DISCONNECTING:
					kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_R_DISCONNECT_REQ_IN_WRONG_STATE,bip_cntx_p->rState);
                    return;
                /* should never happens */    
                default:
					kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_R_DISCONNECT_REQ_IN_WRONG_STATE,bip_cntx_p->rState);
                    ASSERT(0);
                    return;
            }    
            break;
        default:
            /* race condition */
			kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_ERROR_SESSION_ROLE);	
            return;
    }
    return;

BIP_R_DISCONNECT:
    status = GOEP_ServerTpDisconnect(&(bip_cntx_p->BipResponder.primary.sApp));
    if ((status == OB_STATUS_DISCONNECT) || (status == OB_STATUS_NO_CONNECT))
    {   
        /* send disconnect confirm to MMI */
        bip_adp_send_disconnect_ind(msg_p->cm_conn_id, BT_BIP_RESPONDER_ROLE);
    }
    return;
BIP_I_DISCONNECT:
    if (msg_p->disconnect_tp_directly == KAL_TRUE)
    {
        status = BIP_TpDisconnect(&(bip_cntx_p->BipInitiator));
    }
    else
    {
        status = BIP_Disconnect(&(bip_cntx_p->BipInitiator));
    }
    if ((status == OB_STATUS_DISCONNECT) || (status == OB_STATUS_NO_CONNECT))
    {   
        /* send disconnect confirm to MMI */
        bip_adp_send_disconnect_ind(msg_p->cm_conn_id, BT_BIP_INITIATOR_ROLE);
        /* No need to deregister sender */
    }
    return;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_continue_rsp_hdlr
 * DESCRIPTION
 *  This function is to handle continue command response from MMI
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
void bip_adp_continue_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_continue_rsp_struct *rsp = (bt_bip_continue_rsp_struct*) ilm_ptr->local_para_ptr;
    bt_bip_session_role_enum session_role;
    kal_uint8 abort_reason = GOEP_METHOD_NOT_ALLOWED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_CONTINUE_RSP_HDLR);

    /* To check the continue indication comes from initiator or responder */
    session_role = bip_adp_find_session_with_cm_conn_id(rsp->cm_conn_id);

    switch(session_role)
    {
        case BT_BIP_INITIATOR_ROLE:
            /* check state first */
            if (bip_cntx_p->iState != BT_BIP_I_STATE_REQUESTING)
            {
                /* wrong state */
                BT_BIP_I_STATE(bip_cntx_p->iState);
				kal_trace(BT_TRACE_G6_OBEX, BT_BIP_ADP_I_INVALID_CONTINUE_RSP,bip_cntx_p->iState);
            }
            else
            {
                if(rsp->cnf_code == BT_BIP_CNF_FAILED)
                {
                    memset(&(bip_cntx_p->i_xml_parsed_result), 0, sizeof(bt_bip_i_xml_parsed_info_union));
                    BIP_Abort(&(bip_cntx_p->BipInitiator), BIPCH_INITIATOR_PRIMARY, 0);
                }
                BIP_InitiatorContinue(&(bip_cntx_p->BipInitiator.primary)); 
            }

            break;        
        case BT_BIP_RESPONDER_ROLE:
            /* check state first */
            if (bip_cntx_p->rState != BT_BIP_R_STATE_INDICATING)
            {
                /* wrong state */
                BT_BIP_R_STATE(bip_cntx_p->rState);
				kal_trace(BT_TRACE_G6_OBEX, BT_BIP_ADP_R_INVALID_CONTINUE_RSP,bip_cntx_p->rState);
            }
            else
            {
                if(rsp->cnf_code != BT_BIP_CNF_SUCCESS
                   && rsp->cnf_code != BT_BIP_PARTIAL_CONTENT)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                    BIP_Abort(&(bip_cntx_p->BipResponder), BIPCH_RESPONDER_PRIMARY, abort_reason);
                }
                else
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_RESPONDING);        
                }
                BIP_ResponderContinue(&(bip_cntx_p->BipResponder.primary)); 
            }
            break;
        default:
			kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_ERROR_SESSION_ROLE);	
			
            ASSERT(0);
    }
}


/*****************************************************************************
*
* BIP ADP callback function
*
*****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  bip_adp_initiator_event_callback
 * DESCRIPTION
 *  This function is to handle initiator events
 * PARAMETERS
 *  *parms
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_initiator_event_callback(BipCallbackParms *parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8     *p = NULL;
    ObexTpAddr addr;
    int     ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//  BT_BIP_FUNC_ENTRY3(BT_BIP_FUNC_INITIATOR_EVENT_CALLBACK, parms->event, parms->data->bipOp, parms->status);

    switch (parms->event) 
    {
        case BIPINITIATOR_DATA_REQ:
            if (parms->data->ocx != 0) 
            {
                /* Set the buff pointer */
                switch (parms->data->bipOp) 
                {
                    case BIPOP_PUT_IMAGE:
                        p = parms->data->r.rImg.buff;
                        break;

                    case BIPOP_PUT_LINKED_THUMBNAIL:
                        p = parms->data->r.rThm.buff;
                        break;
                    default:
                        bip_cntx_p->i_err_code = BT_BIP_INVALID_PARM;
                        goto BIP_ABORT;
                }
                /* Copy the file into memory */
                ret = btmtk_fs_read_ucs2(parms->data->ocx, p, parms->data->len, (S32*)&parms->data->len);
                if (ret < 0)
                {
                    bip_cntx_p->i_err_code = bip_adp_check_fs_error_code(ret, BT_BIP_ADP_FS_READ);
                    parms->data->len = 0;

				    if (parms->data->ocx != 0) 
				    {
				        btmtk_fs_close_ucs2(parms->data->ocx);
				        parms->data->ocx = 0;
				    }

					/* here need not send ABORT request: OBEX will send ABORT if it check out FS read error */
                    return;
                }
                bip_cntx_p->i_data_len += parms->data->len; 
            }
            else
            {
                parms->data->len = 0;
            }
            break;

        case BIPINITIATOR_DATA_IND:

            /* Set the buff pointer */
            switch (parms->data->dataType) 
            {
                case BIPIND_IMAGE:
                case BIPOP_GET_MONITORING_IMAGE:
                    p = parms->data->i.iImg.buff;
                    break;

                case BIPIND_THUMBNAIL:
                    p = parms->data->i.iThm.buff;
                    break;

                case BIPIND_HANDLES_DESCRIPTOR:
                    p = parms->data->i.iHDsc.buff;
                    break;

                case BIPIND_IMAGE_DESCRIPTOR:
                    p = parms->data->i.iIDsc.buff;
                    break;

                case BIPIND_IMG_LISTING_XML:
                    p = parms->data->i.iList.buff;
                    /* GetImagesList has an Application Parameter also */
                    //App_Report("NbReturnedHandles = %d",
                    //parms->data->i.iList.nbReturnedHandles);
                    break;

                case BIPIND_IMG_CAPABILITIES_XML:
                    p = parms->data->i.iCapa.buff;
                    break;

                case BIPIND_IMG_PROPERTIES_XML:
                    p = parms->data->i.iProp.buff;
                    break;

                case BIPIND_IMAGE_HANDLE:
                    /* PutImage gets an image handle back from the responder */
                    p = parms->data->i.iHndl.imgHandle;
                    break; 

                default:
                    bip_cntx_p->i_err_code = BT_BIP_INVALID_PARM;
                    break;
            }

            switch (parms->data->dataType) 
            {
                case BIPIND_HANDLES_DESCRIPTOR:
                case BIPIND_IMAGE_DESCRIPTOR:
                case BIPIND_IMG_CAPABILITIES_XML:
                case BIPIND_IMG_LISTING_XML:
                case BIPIND_IMG_PROPERTIES_XML:
                case BIPIND_IMAGE:
                case BIPIND_THUMBNAIL:
                    /* Write the image */
                    if (parms->data->ocx != 0) 
                    {
                        /* Write the image */
                        ret =   btmtk_fs_write_ucs2( parms->data->ocx,
                                p, parms->data->len,
                                (S32*)&parms->data->len);
                        if (ret < 0)
                        {
                            bip_cntx_p->i_err_code = bip_adp_check_fs_error_code(ret, BT_BIP_ADP_FS_WRITE);
                            parms->data->len = 0;
                            goto BIP_ABORT;
                        }
                        bip_cntx_p->i_data_len += parms->data->len; 
                    }
                    else 
                    {
                        //App_Report("Initiator error writing image, channel = %x", parms->channel);
                        parms->data->len = 0;
                    }
                    break;

                case BIPIND_IMAGE_HANDLE:
                    /* Received an image handle */
                    //App_Report("Img-Handle = %s", p);
                    break; 

                default:
                    bip_cntx_p->i_err_code = BT_BIP_INVALID_PARM;
                    goto BIP_ABORT;
                    break;
            }
            break;

        case BIPINITIATOR_RESPONSE:
            /* Parse the response */
            switch (parms->data->bipOp) 
            {
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_MONITORING_IMAGE:
                    ASSERT((bip_cntx_p->file_path[0] != 0) || (bip_cntx_p->file_path[1] != 0));
					
                    parms->data->ocx = btmtk_fs_open_ucs2((WCHAR*)bip_cntx_p->file_path, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));

                    if (parms->data->ocx <= 0) 
                    {
						if (parms->data->ocx == BTMTK_FS_ERR_PATH)
						{
					        /* folder NOT exist, create it */
							int ret = bt_adp_create_folder((WCHAR*)bip_cntx_p->file_path);
	                        if (ret >= 0)
	                        {
	                            /* open the file again */
	                            parms->data->ocx = btmtk_fs_open_ucs2((WCHAR*)bip_cntx_p->file_path, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
								if (parms->data->ocx > 0)
								{
								    /* success */
								    break;
								}
                                else
                                {
                                    parms->data->ocx = 0;
                                }
	                        }
							else
							{
							    /* maybe disk full or root dir full */
							    parms->data->ocx = 0; /* ret; */
							}
						}
						
                        bip_cntx_p->i_err_code = bip_adp_check_fs_error_code(parms->data->ocx, BT_BIP_ADP_FS_OPEN);
                        goto BIP_ABORT;
                    }
                    break;
                case BIPOP_GET_IMAGES_LIST:
                case BIPOP_GET_CAPABILITIES:
                case BIPOP_GET_IMAGE_PROPERTIES:
                    parms->data->ocx = btmtk_fs_open_ucs2((WCHAR*)BIP_ADP_RCV_FILE, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
                    if (parms->data->ocx <= 0) 
                    {
                        parms->data->ocx = 0;
                        bip_cntx_p->i_err_code = bip_adp_check_fs_error_code(parms->data->ocx, BT_BIP_ADP_FS_OPEN);
                        goto BIP_ABORT;
                    }
                    break;
            }
            /* Move the provide object request to when send continue */ 
            break;

        case BIPINITIATOR_CONTINUE:
            switch (parms->data->bipOp) 
            {
                case BIPOP_PUT_IMAGE:
                case BIPOP_PUT_LINKED_THUMBNAIL:
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_MONITORING_IMAGE:
                case BIPOP_GET_IMAGES_LIST:
                case BIPOP_GET_CAPABILITIES:
                case BIPOP_GET_IMAGE_PROPERTIES:
                    /* Received an XML response, the data will be indicated
                    * in  BIPINITIATOR_DATA_IND event
                    */
                    //App_Report("Initiator received %s response", pBipOp(parms->data->bipOp));                    
                    bip_adp_send_continue_ind(bip_cntx_p->i_cm_conn_id, BIPOBS_GetObjectLen(parms->obex.client->pushPull.object), bip_cntx_p->i_data_len);
                    bip_cntx_p->i_data_len = 0;
                    break;

                default:
                    bip_cntx_p->i_err_code = BT_BIP_INVALID_PARM;
                    goto BIP_ABORT;
                    break;
            }
            break;

        case BIPINITIATOR_COMPLETE:
            switch (parms->data->bipOp) 
            {
                case BIPOP_CONNECT:
                    if (parms->status == OB_STATUS_SUCCESS) 
                    {
                        //App_Report("Initiator Connect Complete, channel = %x", parms->channel);
                        CONVERT_BDADDR2ARRAY((U8*)&(addr.proto.bt.addr), bip_cntx_p->i_bd_addr.lap, bip_cntx_p->i_bd_addr.uap, bip_cntx_p->i_bd_addr.nap);
                        addr.type = OBEX_TP_BLUETOOTH;
                        bip_cntx_p->i_cm_conn_id = (U32) BTBMProfileAllocateConnIndex(SC_IMAGING, addr.proto.bt.addr);
                        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
                        if (bip_cntx_p->i_chal_status == BIP_ADP_CHAL_STATUS_VERIFY_OK)
                        {
                            bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_CHALLENGE_OK);
                        }
                        else if (bip_cntx_p->i_chal_status == BIP_ADP_CHAL_STATUS_VERIFY_FAILED)
                        {
                            bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_CHALLENGE_FAILED);
                        }
                        else
                        {
                            bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
                        }
                    }
                    else if (parms->status == OB_STATUS_DISCONNECT) 
                    {
                        //App_Report("Initiator Disconnect Complete, channel = %x", parms->channel);
                        /* send connect failed confirm to MMI */
                        if(bip_cntx_p->iState == BT_BIP_I_STATE_CONNECTING)
                        {
                            bip_cntx_p->i_cm_conn_id = 0xFFFFFFFF;
                            bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);
                        }
                        else
                        {
                            if (parms->data->ocx != 0) 
                            {
                                btmtk_fs_close_ucs2(parms->data->ocx);
                                parms->data->ocx = 0;
                            }

                            if (bip_cntx_p->iState == BT_BIP_I_STATE_XMLPARSING)
                            {
                                /* notify mmi the parsing result, and clear data */
                                bip_adp_parse_imagelist_xml_file_end(BIP_ADP_XMLPARSE_TPDISCONNECTED);                            
                            }
                            
                            /* notify MMI */
                            bip_adp_send_disconnect_ind(bip_cntx_p->i_cm_conn_id, BT_BIP_INITIATOR_ROLE);
                            /* release BTBM conn ID*/
                            BTBMProfileReleaseConnIndex((U8)bip_cntx_p->i_cm_conn_id);
                        }
                        /* deregister initiator */
                        bip_adp_deregister_initiator(); 
                        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_IDLE);
                    }
                    else
                    {
                        ASSERT(0);
                    }
                    break;

                case BIPOP_PUT_IMAGE:
                case BIPOP_PUT_LINKED_THUMBNAIL:
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_MONITORING_IMAGE:
                case BIPOP_GET_IMAGES_LIST:
                case BIPOP_GET_CAPABILITIES:
                case BIPOP_GET_IMAGE_PROPERTIES:
                    if (parms->data->ocx != 0) 
                    {
                        btmtk_fs_close_ucs2(parms->data->ocx);
                        parms->data->ocx = 0;
                    }
                    //App_Report("%s Complete status = %d", pBipOp(parms->data->bipOp), parms->status);
                    if (parms->data->bipOp == BIPOP_PUT_IMAGE)
                    {
                        if (parms->status == OBRC_PARTIAL_CONTENT)
                        {
                            bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_PARTIAL_CONTENT, parms->data->i.iHndl.imgHandle);
                        }
                        else
                        {
                            bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS, NULL);
                        }
                    }
                    else if (parms->data->bipOp == BIPOP_PUT_LINKED_THUMBNAIL)
                    {
                        bip_adp_send_put_linked_thumbnail_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
                    }
                    else if (parms->data->bipOp == BIPOP_GET_IMAGE)
                    {
                        bip_adp_send_get_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS, bip_cntx_p->i_data_len);                    
                    }
                    else if (parms->data->bipOp == BIPOP_GET_LINKED_THUMBNAIL)
                    {
                        bip_adp_send_get_linked_thumbnail_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
                    }         
                    else if (parms->data->bipOp == BIPOP_GET_MONITORING_IMAGE)
                    {
                        bip_adp_send_get_monitoring_img_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
                    }
                    else if (parms->data->bipOp == BIPOP_GET_CAPABILITIES)
                    {
               		    /* It's a GET response, parse it: */
                        bip_adp_parse_xml_file(BIPOP_GET_CAPABILITIES, BIP_ADP_RCV_FILE, &(bip_cntx_p->i_xml_parser_p));
                        /* delete the parsed tmp xml file */
                        btmtk_fs_delete_ucs2(BIP_ADP_RCV_FILE);

                        bip_adp_send_get_capabilities_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
                    }
                    else if (parms->data->bipOp == BIPOP_GET_IMAGES_LIST)
                    {
                        //kal_int8 cnf_code;

                        bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num = 0;
                        bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_num_interval = 0;
                        bip_cntx_p->i_rspcode = parms->status;
                        
                        ret = bip_adp_parse_imagelist_xml_file_start(&(bip_cntx_p->i_xml_parser_p));
#if 0
                        /* delete the parsed tmp xml file */
                        btmtk_fs_delete(BIP_ADP_RCV_FILE);

                        if(ret == 0 || parms->data->i.iList.nbReturnedHandles == 0) /* parse succeed */
                        {
                            if(parms->status == OBRC_PARTIAL_CONTENT)
                            {
                                 cnf_code = BT_BIP_PARTIAL_CONTENT;
                            }
                            else
                            {
                                cnf_code = BT_BIP_CNF_SUCCESS;
                            }
                            /*
                            if(bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num == 0)
                            {
                                bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num = parms->data->i.iList.nbReturnedHandles;
                            }
                            */
                        }                        
                        else  /* parse has some error */
                        {
                            if (bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num > 0)
                            {
                                cnf_code = BT_BIP_PARTIAL_CONTENT;
                            }
                            else
                            {
                                 cnf_code = BT_BIP_XML_PARSE_FAIL;
                            }
                        }

                        bip_adp_send_get_img_list_cnf(bip_cntx_p->i_cm_conn_id, cnf_code);
#endif                        
                    }
                    else if (parms->data->bipOp == BIPOP_GET_IMAGE_PROPERTIES)
                    {
                        bip_adp_parse_xml_file(BIPOP_GET_IMAGE_PROPERTIES, BIP_ADP_RCV_FILE, &(bip_cntx_p->i_xml_parser_p));
                        /* delete the parsed tmp xml file */
                        btmtk_fs_delete_ucs2(BIP_ADP_RCV_FILE);
                        
                        bip_adp_send_get_img_prop_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
                    }
                    
                    /* BIPOP_GET_IMAGES_LIST: state is PARSING */
                    if (parms->data->bipOp != BIPOP_GET_IMAGES_LIST)
                    {
                        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
                    }
                    /* Reset the progress bar */
                    bip_cntx_p->i_data_len = 0;

                    break;

                default:
                    //App_Report("Initiator Operation Complete status = %d", parms->status);
                    break;
            }

            /* 
             * For image-list xml parsing is interval, so wait the parsing finished
             */
            if (parms->data->bipOp == BIPOP_GET_IMAGES_LIST)
            {
               /* maybe to be continue */
               break;
            }
         
            /* Free Img-Description header memory */
            switch (parms->data->bipOp) 
            {
                case BIPOP_GET_IMAGES_LIST:
                    if (bip_cntx_p->iData.r.rList.handlesDescriptor != 0) 
                    {
                        if(bip_cntx_p->iData.r.rList.handlesDescriptor != (BipImgDescription)1)
                            free_ctrl_buffer(bip_cntx_p->iData.r.rList.handlesDescriptor);
                        bip_cntx_p->iData.r.rList.handlesDescriptor = 0;
                        bip_cntx_p->iData.r.rList.handlesDescriptorLen = 0;
                    }
                    break;

                case BIPOP_PUT_IMAGE:
                case BIPOP_GET_IMAGE:
                    if (bip_cntx_p->iData.r.rImg.imageDescriptor != 0) 
                    {
                        if(bip_cntx_p->iData.r.rImg.imageDescriptor != (BipImgDescription)1)
                            free_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptor);
                        bip_cntx_p->iData.r.rImg.imageDescriptor = 0;
                        bip_cntx_p->iData.r.rImg.imageDescriptorLen = 0;
                    }
                    break;

                default:
                    break;
            }
            /* clear the data info */
            bip_cntx_p->i_data_len = 0;
            memset(&(bip_cntx_p->i_xml_parsed_result), 0, sizeof(bt_bip_i_xml_parsed_info_union));
            memset(&(bip_cntx_p->iData), 0, sizeof(BipData));
            break;

        case BIPINITIATOR_FAILED:
            switch(parms->status)
            {
                case BIP_NO_SERVICE_FOUND:
                case BIP_SCO_REJECT:
                case BIP_BTCHIP_REJECT:
                    bip_cntx_p->i_cm_conn_id = 0xFFFFFFFF;
                    bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);
                    /* deregister initiator */
                    bip_adp_deregister_initiator();
                    BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_IDLE);
                    break;
                default:
                    break;
            }
            break;

        case BIPINITIATOR_ABORT:
            
            bip_cntx_p->i_data_len = 0;
            //App_Report("Initiator Aborted status = %d, channel = %d",
            //                            parms->status, parms->channel);
            if(bip_cntx_p->iState == BT_BIP_I_STATE_ABORTING)
            {
                bip_adp_send_abort_cnf(bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
            }
            else
            {
                kal_uint8 reason = (kal_uint8)parms->status;

                if (reason == 0x20)
                {
                   /* Initiator sent out the ABORT request: maybe FS error */
                   reason = bip_cntx_p->i_err_code;
                }
                else if (reason == GOEP_STATUS_DISCONNECT || reason == 0x80)
                {
                    /* disconnected */
                    reason = BT_BIP_DISCONNECTED;
                    bip_cntx_p->i_chal_info_p = NULL;
                }

                kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_ABORT_REASON, reason);
                if (bip_cntx_p->i_chal_info_p)
                {
                    bip_adp_send_challenge_ind(
                        bip_cntx_p->i_cm_conn_id,
                        bip_cntx_p->i_chal_info_p,
                        MSG_ID_BT_BIPI_OBEX_AUTH_IND);
                    bip_cntx_p->i_chal_info_p = NULL;

                    BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_AUTHENTICATING);                    
                    return;
                }
                
                if (parms->data->bipOp == BIPOP_PUT_IMAGE)
                {
                    bip_adp_send_put_img_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/, NULL);
                }
                else if (parms->data->bipOp == BIPOP_PUT_LINKED_THUMBNAIL)
                {
                    bip_adp_send_put_linked_thumbnail_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/);
                }
                else if (parms->data->bipOp == BIPOP_GET_IMAGE)
                {
                    bip_adp_send_get_img_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/, 0);                    
                }
                else if (parms->data->bipOp == BIPOP_GET_LINKED_THUMBNAIL)
                {
                    bip_adp_send_get_linked_thumbnail_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/);
                }         
                else if (parms->data->bipOp == BIPOP_GET_MONITORING_IMAGE)
                {
                    bip_adp_send_get_monitoring_img_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/);
                }
                else if (parms->data->bipOp == BIPOP_GET_IMAGE_PROPERTIES)
                {
                    bip_adp_send_get_img_prop_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/);
                }
                else if (parms->data->bipOp == BIPOP_GET_IMAGES_LIST)
                {
                    /* kal_uint8 cnf_code;
                    cnf_code = reason; */
                    
                    if (parms->status == 0x20 && 
                        (reason == BT_BIP_FS_QUOTA_FULL || reason == BT_BIP_FILE_WRITE_FAIL))
                    {
                        bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num = 0;
                        bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_num_interval = 0;
                        bip_cntx_p->i_rspcode = BT_BIP_PARTIAL_CONTENT;//parms->status;
                        
                        ret = bip_adp_parse_imagelist_xml_file_start(&(bip_cntx_p->i_xml_parser_p));
#if 0					
						/* Maybe we ABORT because of FS error, then parse the content with our best */
						ret = bip_adp_parse_xml_file(BIPOP_GET_IMAGES_LIST, BIP_ADP_RCV_FILE, &(bip_cntx_p->i_xml_parser_p));
                        /* delete the parsed tmp xml file */
                        btmtk_fs_delete(BIP_ADP_RCV_FILE);
                        
						if(bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num > 0)
						{
							cnf_code = BT_BIP_PARTIAL_CONTENT;
						}

				        bip_adp_send_get_img_list_cnf(bip_cntx_p->i_cm_conn_id, cnf_code);
#endif
                    }
                    else
                    {
				        bip_adp_send_get_img_list_cnf(bip_cntx_p->i_cm_conn_id, reason);
                    }
                }
                else if (parms->data->bipOp == BIPOP_GET_CAPABILITIES)
                {
                    bip_adp_send_get_capabilities_cnf(bip_cntx_p->i_cm_conn_id, reason/*bip_cntx_p->i_err_code*/);
                }            
                else if (parms->data->bipOp == BIPOP_CONNECT)
                {
                    bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, reason);
                }
                bip_cntx_p->i_err_code = BT_BIP_CNF_FAILED; /* Reset the error code to failed */
            }
            if (parms->data->ocx != 0) 
            {
                btmtk_fs_close_ucs2(parms->data->ocx);
                parms->data->ocx = 0;
            }
            /* Free Img-Description header memory */
            switch (parms->data->bipOp) 
            {
                case BIPOP_GET_IMAGES_LIST:
                    if (bip_cntx_p->iData.r.rList.handlesDescriptor != 0) 
                    {
                        if(bip_cntx_p->iData.r.rList.handlesDescriptor != (BipImgDescription)1)
                            free_ctrl_buffer(bip_cntx_p->iData.r.rList.handlesDescriptor);
                        bip_cntx_p->iData.r.rList.handlesDescriptor = 0;
                        bip_cntx_p->iData.r.rList.handlesDescriptorLen = 0;
                    }
                    break;

                case BIPOP_PUT_IMAGE:
                case BIPOP_GET_IMAGE:
                    if (bip_cntx_p->iData.r.rImg.imageDescriptor != 0) 
                    {
                        if(bip_cntx_p->iData.r.rImg.imageDescriptor != (BipImgDescription)1)
                            free_ctrl_buffer(bip_cntx_p->iData.r.rImg.imageDescriptor);
                        bip_cntx_p->iData.r.rImg.imageDescriptor = 0;
                        bip_cntx_p->iData.r.rImg.imageDescriptorLen = 0;
                    }
                    break;
                    
                default:
                    break;
            }
            memset(&(bip_cntx_p->iData), 0, sizeof(BipData));
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
            break;
            
        case BIP_OBAUTH_RSP:
            bip_cntx_p->i_chal_status = BIP_ADP_CHAL_STATUS_VERIFY_FAILED;

            if (BIP_ClientVerifyAuthResponse(
                &(bip_cntx_p->BipInitiator.primary),
                bip_cntx_p->i_dig_chal.pwd, bip_cntx_p->i_dig_chal.pwd_len) &&
                parms->response->userIdLen == bip_cntx_p->i_dig_chal.uid_len)
            {
                if (bip_cntx_p->i_dig_chal.uid_len == 0 ||
                    !memcmp(bip_cntx_p->i_dig_chal.uid, parms->response->userId, bip_cntx_p->i_dig_chal.uid_len))
                    bip_cntx_p->i_chal_status = BIP_ADP_CHAL_STATUS_VERIFY_OK;
            }
            break;

        case BIP_OBAUTH_CHAL:
            memcpy(&bip_cntx_p->i_chal_info, parms->challenge, sizeof(ObexAuthChallengeInfo));
            bip_cntx_p->i_chal_info_p = &bip_cntx_p->i_chal_info;
            break;
                
        default:
            break;
    }
    return;
BIP_ABORT:
    if (parms->data->ocx != 0) 
    {
        btmtk_fs_close_ucs2(parms->data->ocx);
        parms->data->ocx = 0;
    }
    bip_cntx_p->i_data_len = 0;
    memset(&(bip_cntx_p->i_xml_parsed_result), 0, sizeof(bt_bip_i_xml_parsed_info_union));
    BIP_Abort(&(bip_cntx_p->BipInitiator), parms->channel, 0);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_responder_event_callback
 * DESCRIPTION
 *  This function is to handle responder events
 * PARAMETERS
 *  *parms
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_responder_event_callback(BipCallbackParms *parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8     *p = NULL;
    int     ret, fh;
    kal_uint8 abort_reason = GOEP_SERVICE_UNAVAILABLE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//  BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_SENDER_EVENT_BIPCLIENT_DATA_REQ, bip_cntx_p->state);

    bip_cntx_p->rData_p = parms->data;
    switch (parms->event)
    {
        case BIPRESPONDER_DATA_REQ:
            /* Read file */
            /* Set up the buff pointer */
            switch (parms->data->bipOp)
            {
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_MONITORING_IMAGE:
                    p = parms->data->r.rImg.buff;
                    break;

                case BIPOP_GET_LINKED_THUMBNAIL:
                    p = parms->data->r.rThm.buff;
                    break;

                case BIPOP_GET_CAPABILITIES:
                    p = parms->data->r.rCapa.buff;
                    break;

                case BIPOP_GET_IMAGES_LIST:
                    p = parms->data->r.rList.buff;
                    break;

                case BIPOP_GET_IMAGE_PROPERTIES:
                    p = parms->data->r.rProp.buff;
                    break;

                default:
                    bip_cntx_p->r_err_code = BT_BIP_INVALID_PARM;
                    parms->data->len = 0;
                    goto BIP_ABORT;
            }

            /* Copy file into memory buff */
            if (parms->data->ocx != 0)
            {
                ret = btmtk_fs_read_ucs2(parms->data->ocx, p, parms->data->len, (S32*)&parms->data->len );
                if (ret < 0)
                {
                    bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(ret, BT_BIP_ADP_FS_READ);
                    parms->data->len = 0;
                    goto BIP_ABORT;
                }
                bip_cntx_p->r_data_len += parms->data->len;
            }
            else
            {
                //App_Report("Responder error reading file, channel = %x", parms->channel);
                parms->data->len = 0;
                /* Need to add error handling here */
            }
            break;

        case BIPRESPONDER_DATA_IND:
            /* Write file */
            /* Set the buff pointer */
            switch (parms->data->dataType)
            {
                case BIPIND_IMAGE:
                    p = parms->data->i.iImg.buff;
                    break;

                case BIPIND_THUMBNAIL:
                    p = parms->data->i.iThm.buff;
                    break;

                case BIPIND_HANDLES_DESCRIPTOR:
                    p = parms->data->i.iHDsc.buff;
                    break;

                case BIPIND_IMAGE_DESCRIPTOR:
                    p = parms->data->i.iIDsc.buff;
                    break;

                default:
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ABORTING);
                    parms->data->len = 0;
                    goto BIP_ABORT;
            }

            switch (parms->data->dataType)
            {
                case BIPIND_IMAGE:
                case BIPIND_THUMBNAIL:
                    /* Write the image */
                    if(parms->data->ocx == 0)
                    {
                        parms->data->ocx = btmtk_fs_open_ucs2((WCHAR*)BIP_ADP_PUSH_TMP_FILE, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
                        if(parms->data->ocx < 0)
                        {
                            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ABORTING);
                            bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(parms->data->ocx, BT_BIP_ADP_FS_OPEN);
                            parms->data->len = 0;
                            parms->data->ocx = 0;
                            goto BIP_ABORT;
                        }
                    }
                    ret = btmtk_fs_write_ucs2( parms->data->ocx,
                    p, parms->data->len,
                    (S32*)&parms->data->len);
                    if (ret < 0)
                    {
                        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ABORTING);
                        bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(ret, BT_BIP_ADP_FS_WRITE);
                        parms->data->len = 0;
                        goto BIP_ABORT;
                    }
                    bip_cntx_p->r_data_len += parms->data->len;
                    break;

                case BIPIND_HANDLES_DESCRIPTOR:
                case BIPIND_IMAGE_DESCRIPTOR:

                    fh = btmtk_fs_open_ucs2(BIP_ADP_DESCPTR_FILE, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));

                    if (fh < 0)
                    {
                        parms->data->len = 0;
                        bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(fh, BT_BIP_ADP_FS_OPEN);
                        goto BIP_ABORT;
                    }
                    /* Write the descriptor */
                    ret = btmtk_fs_write_ucs2( fh, p, parms->data->len, (S32*)&parms->data->len);
                    if (ret < 0)
                    {
                        bip_cntx_p->r_err_code = bip_adp_check_fs_error_code(ret, BT_BIP_ADP_FS_WRITE);
                        parms->data->len = 0;
                        goto BIP_ABORT;
                    }
                    btmtk_fs_close_ucs2(fh);
                    /* Received an XML response, parse it here */
                    //App_Report("Responder received %s", pBipInd(parms->data->dataType));
                    break;

                default:
                    parms->data->len = 0;
                    bip_cntx_p->r_err_code = BT_BIP_INVALID_PARM;
                    goto BIP_ABORT;
            }
            break;

        case BIPRESPONDER_REQUEST:
            switch (parms->data->bipOp) 
            {
                case BIPOP_PUT_LINKED_THUMBNAIL:
                //App_Report("Img-Handle = %s", parms->data->r.rThm.imgHandle);
                case BIPOP_PUT_IMAGE:
                //App_Report("Created Img-Handle = %s", parms->data->r.rImg.imgHandle);
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_CAPABILITIES:
                case BIPOP_GET_IMAGES_LIST:
                case BIPOP_GET_IMAGE_PROPERTIES:
                case BIPOP_GET_MONITORING_IMAGE:
                break;

                default:
                    bip_cntx_p->r_err_code = BT_BIP_INVALID_PARM;
                    parms->data->len = 0;
                    goto BIP_ABORT;
            }
            break;

        case BIPRESPONDER_COMPLETE:
            /* Close the file hanlde and reset the file handle */
            bip_cntx_p->r_data_len = 0;
            switch (parms->data->bipOp)
            {
                case BIPOP_CONNECT:
                    if (parms->status == OB_STATUS_SUCCESS)
                    {
                        //App_Report("Responder Connect Complete, channel = %x", parms->channel);
                    }
                    else if (parms->status == OB_STATUS_DISCONNECT)
                    {
                        if (parms->data->ocx != 0)
                        {
                            ret = btmtk_fs_close_ucs2(parms->data->ocx);
                            if (ret >= BTMTK_FS_OK)
                            {
                                parms->data->ocx = 0;
                            }
                            else if (BIP_ADP_FS_DEVICE_BUSY(ret))
                            {
                                bip_adp_start_close_file_continually(parms->data->ocx);
                            }
                        }
                        bip_cntx_p->r_data_len = 0;
                        //App_Report("Responder Disconnect Complete, channel = %x", parms->channel);
                        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACTIVATED);
                        bip_adp_send_disconnect_ind(bip_cntx_p->r_cm_conn_id, BT_BIP_RESPONDER_ROLE);
                        BTBMProfileReleaseConnIndex((U8)bip_cntx_p->r_cm_conn_id);
                        bip_cntx_p->r_cm_conn_id = 0xFFFFFFFF;
                        return;
                    }
                    break;

                case BIPOP_PUT_IMAGE:
                case BIPOP_PUT_LINKED_THUMBNAIL:
                case BIPOP_GET_IMAGE:
                case BIPOP_GET_LINKED_THUMBNAIL:
                case BIPOP_GET_CAPABILITIES:
                case BIPOP_GET_IMAGES_LIST:
                case BIPOP_GET_IMAGE_PROPERTIES:
                    if (parms->data->ocx != 0)
                    {
                        ret = btmtk_fs_close_ucs2(parms->data->ocx);
                        if (ret >= BTMTK_FS_OK)
                        {
                            parms->data->ocx = 0;
                        }
                        else if (BIP_ADP_FS_DEVICE_BUSY(ret))
                        {
                            bip_adp_start_close_file_continually(parms->data->ocx);
                        }
                    }
                    //App_Report("%s Complete status = %d", pBipOp(parms->data->bipOp), parms->status);

#ifdef BIP_CHECK_DRM_RIGHTS
                    /* check the received file */
                    if (parms->data->bipOp == BIPOP_PUT_IMAGE)
                    {
                       kal_bool b_succ = KAL_TRUE;
                       b_succ = bt_bip_adp_check_DRM_rights(bip_cntx_p->r_recv_path);

                       if (b_succ == KAL_FALSE)
                       {
                            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                            bip_adp_send_abort_ind(bip_cntx_p->r_cm_conn_id, BT_BIP_DRM_NO_RIGHTS);
                            return;
                       }
                    }
#endif
                        
                    break;

                default:
                    //App_Report("Responder Operation Complete status = %d", parms->status);
                    break;
            }
            /* Free Img-Description header memory */
            switch (parms->data->bipOp)
            {
                case BIPOP_GET_IMAGES_LIST:
                    if (bip_cntx_p->rData.r.rList.handlesDescriptor != 0)
                    {
                        if(bip_cntx_p->rData.r.rList.handlesDescriptor != (BipImgDescription)1)
                            free_ctrl_buffer(bip_cntx_p->rData.r.rList.handlesDescriptor);
                        bip_cntx_p->rData.r.rList.handlesDescriptor = 0;
                        bip_cntx_p->rData.r.rList.handlesDescriptorLen = 0;
                    }
                    break;

                default:
                    break;
            }
            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
            bip_adp_send_complete_ind(bip_cntx_p->r_cm_conn_id);
            break;

        case BIPRESPONDER_FAILED:
            //App_Report("Print Session Failed status = %d", parms->status);
            break;

        case BIPRESPONDER_ABORT:
            if (parms->data->ocx != 0)
            {
                int ret = btmtk_fs_close_ucs2(parms->data->ocx);
                if (ret >= BTMTK_FS_OK)
                {
                    parms->data->ocx = 0;
                }
                else if (BIP_ADP_FS_DEVICE_BUSY(ret))
                {
                    bip_adp_start_close_file_continually(parms->data->ocx);
                }
            }
            bip_cntx_p->r_data_len = 0;
            
            if (parms->obex.server->object) 
            {
                /* close any objects associated with the aborted operation */
                BIPOBS_Close(&(parms->obex.server->object));
            }

            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
            bip_adp_send_abort_ind(bip_cntx_p->r_cm_conn_id, bip_cntx_p->r_err_code);
            //App_Report("Print Session Aborted status = %d", parms->status);
            break;

        case BIPRESPONDER_CONTINUE:
            
        if (bip_cntx_p->rState == BT_BIP_R_STATE_ABORTING)
        {
            bip_adp_send_abort_ind(bip_cntx_p->r_cm_conn_id, bip_cntx_p->r_err_code);
            bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
            
            BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
            BIP_Abort(&(bip_cntx_p->BipResponder), BIPCH_RESPONDER_PRIMARY, abort_reason);
            BIP_ResponderContinue(&(bip_cntx_p->BipResponder.primary));
            return;
        }
        if (bip_cntx_p->r_chal_info_p)
        {
            /* initiator challenge responder case */
            bip_adp_send_challenge_ind(
                bip_cntx_p->r_cm_conn_id,
                bip_cntx_p->r_chal_info_p,
                MSG_ID_BT_BIPR_OBEX_AUTH_IND);
            /* clear challenge info */
            bip_cntx_p->r_chal_info_p = NULL;

            /* maybe we should change status to ACCEPTING or INDICATING
             * althoug we think external ADP should charge the correct status!
             */
            if (parms->data->bipOp == BIPOP_CONNECT)
            {
                BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTING);
            }
            else
            {
                BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
            }
            return;
        }    
        switch (parms->data->bipOp)
        {            
            case BIPOP_CONNECT:
                {
                    ObexTpConnInfo      tpInfo;
                    kal_uint8 *        dev_name = NULL;

                    GOEP_ServerGetTpConnInfo(&(parms->obex.server->sApp), &tpInfo);
                    dev_name = (kal_uint8*)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr));
                    CONVERT_ARRAY2BDADDR((btbm_bd_addr_t *)&(bip_cntx_p->r_bd_addr), tpInfo.remDev->bdAddr.addr);
                    bip_cntx_p->r_cm_conn_id = BTBMProfileAllocateConnIndex(SC_IMAGING_RESPONDER, tpInfo.remDev->bdAddr);
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTING);
                    bip_adp_send_connect_ind(bip_cntx_p->r_cm_conn_id, &(bip_cntx_p->r_bd_addr), dev_name);
                }
                break;
            case BIPOP_PUT_LINKED_THUMBNAIL:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    /* check parms.FinalPacket */
                    if (parms->FinalPacket)
                    {
                        if (parms->data->ocx != 0)
                        {
                            ret = btmtk_fs_close_ucs2(parms->data->ocx);
                            if (ret >= BTMTK_FS_OK)
                            {
                                parms->data->ocx = 0;
                            }
                            else if (BIP_ADP_FS_DEVICE_BUSY(ret))
                            {
                                bip_adp_start_close_file_continually(parms->data->ocx);
                            }
                        }
                        bip_adp_send_final_packet_push_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len, parms->FinalPacket);
                    }
                    else
                    {
                        bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    }
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    
                    /* check parms.FinalPacket */
                    bip_adp_send_put_linked_thumbnail_ind(bip_cntx_p->r_cm_conn_id, parms->data->r.rThm.imgHandle, parms->FinalPacket);
                    
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                }
                break;
                
            case BIPOP_PUT_IMAGE:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    /* check parms.FinalPacket */
                    if (parms->FinalPacket)
                    {
                        if (parms->data->ocx != 0)
                        {
                            ret = btmtk_fs_close_ucs2(parms->data->ocx);
                            if (ret >= BTMTK_FS_OK)
                            {
                                parms->data->ocx = 0;
                            }
                            else if (BIP_ADP_FS_DEVICE_BUSY(ret))
                            {
                                bip_adp_start_close_file_continually(parms->data->ocx);
                            }
                        }
                        //zgyu bip_adp_send_final_packet_push_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len, parms->FinalPacket);
                        bip_adp_send_final_packet_push_ind(
                            bip_cntx_p->r_cm_conn_id,
                            bip_cntx_p->rData_p->r.rImg.img_len, 
                            bip_cntx_p->r_data_len,
                            parms->FinalPacket);
                        
                    }
                    else
                    {
                        // zgyu bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                        bip_adp_send_continue_ind(
                            bip_cntx_p->r_cm_conn_id, 
                            bip_cntx_p->rData_p->r.rImg.img_len,
                            bip_cntx_p->r_data_len);
                    }
                    
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_parse_xml_file(BIPOP_PUT_IMAGE, BIP_ADP_DESCPTR_FILE, &(bip_cntx_p->r_xml_parser_p));					
                    
                    /* check parms.FinalPacket */
                    bip_adp_send_put_img_ind(bip_cntx_p->r_cm_conn_id, &(parms->data->r.rImg), &(bip_cntx_p->r_xml_parsed_result.parsed_img_desc), parms->FinalPacket);
                    
                    memset(&(bip_cntx_p->r_xml_parsed_result), 0, sizeof(bt_bip_r_xml_parsed_info_union));
                    btmtk_fs_delete_ucs2(BIP_ADP_DESCPTR_FILE); 
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                }
                break;
                
            case BIPOP_GET_LINKED_THUMBNAIL:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_get_linked_thumbnail_ind(bip_cntx_p->r_cm_conn_id, parms->data->r.rThm.imgHandle);
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                }
                break;

            case BIPOP_GET_CAPABILITIES:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_get_capabilities_ind(bip_cntx_p->r_cm_conn_id);
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                }
                break;

            case BIPOP_GET_IMAGE:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_parse_xml_file(BIPOP_GET_IMAGE, BIP_ADP_DESCPTR_FILE, &(bip_cntx_p->r_xml_parser_p));
                    bip_adp_send_get_img_ind(bip_cntx_p->r_cm_conn_id, parms->data->r.rImg.imgHandle);
                    memset(&(bip_cntx_p->r_xml_parsed_result), 0, sizeof(bt_bip_r_xml_parsed_info_union));
                    btmtk_fs_delete_ucs2(BIP_ADP_DESCPTR_FILE); 
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                }
                break;

            case BIPOP_GET_IMAGES_LIST:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_get_img_list_ind(bip_cntx_p->r_cm_conn_id, &(parms->data->r.rList));
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                }                
                break;

            case BIPOP_GET_IMAGE_PROPERTIES:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_get_img_prop_ind(bip_cntx_p->r_cm_conn_id, parms->data->r.rProp.imgHandle);
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                } 
                break;

            case BIPOP_GET_MONITORING_IMAGE:
                if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_continue_ind(bip_cntx_p->r_cm_conn_id, 0, bip_cntx_p->r_data_len);
                    bip_cntx_p->r_data_len = 0;
                }
                else if (bip_cntx_p->rState == BT_BIP_R_STATE_ACCEPTED)
                {
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_INDICATING);
                    bip_adp_send_get_monitoring_img_ind(bip_cntx_p->r_cm_conn_id, parms->data->r.rMon.storeFlag);
                }
                else
                {
                    // error handling
                    BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
                } 
                break;

            default:
                bip_cntx_p->r_err_code = BT_BIP_INVALID_PARM;
                parms->data->len = 0;
                goto BIP_ABORT;
            }
            break;

        case BIPRESPONDER_AUTHORIZE_IND:
            {
                ObexTpConnInfo      tpInfo;
                kal_uint8 *        dev_name = NULL;

                GOEP_ServerGetTpConnInfo(&(parms->obex.server->sApp), &tpInfo);
                dev_name = (kal_uint8*)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr));
                CONVERT_ARRAY2BDADDR((btbm_bd_addr_t *)&(bip_cntx_p->r_bd_addr), tpInfo.remDev->bdAddr.addr);

                BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_AUTHORIZING);
                bip_adp_send_authorize_ind(&(bip_cntx_p->r_bd_addr), dev_name);
            }
            break;
            
        case BIP_OBAUTH_CHAL:
            memcpy(&bip_cntx_p->r_chal_info, parms->challenge, sizeof(ObexAuthChallengeInfo));
            bip_cntx_p->r_chal_info_p = &bip_cntx_p->r_chal_info;
            break;

        case BIP_OBAUTH_RSP:
            if (bip_cntx_p->r_chal_status == BIP_ADP_CHAL_STATUS_REQUEST)
            {
                bip_cntx_p->r_chal_status = BIP_ADP_CHAL_STATUS_VERIFY_FAILED;
                
                if (BIP_ServerVerifyAuthResponse(
                    &(bip_cntx_p->BipResponder.primary),
                    bip_cntx_p->r_dig_chal.pwd, bip_cntx_p->r_dig_chal.pwd_len) &&
                    parms->response->userIdLen == bip_cntx_p->r_dig_chal.uid_len)
                {
                    if (bip_cntx_p->r_dig_chal.uid_len == 0 ||
                        !memcmp(bip_cntx_p->r_dig_chal.uid, parms->response->userId, bip_cntx_p->r_dig_chal.uid_len))
                        bip_cntx_p->r_chal_status = BIP_ADP_CHAL_STATUS_VERIFY_OK;
                }                

                if (bip_cntx_p->r_chal_status == BIP_ADP_CHAL_STATUS_VERIFY_OK)
                    bip_adp_send_challenge_cnf(bip_cntx_p->r_cm_conn_id, BT_BIP_CNF_SUCCESS,  MSG_ID_BT_BIPR_OBEX_AUTH_CNF);
                else
                    bip_adp_send_challenge_cnf(bip_cntx_p->r_cm_conn_id, BT_BIP_CNF_FAILED,  MSG_ID_BT_BIPR_OBEX_AUTH_CNF);
            }
            break;
            
        default:
            break;
    }
    
    return;
    
BIP_ABORT:
#if 0
    if (parms->data->ocx != 0)
    {
        int ret = btmtk_fs_close(parms->data->ocx);
        if (ret >= BTMTK_FS_NO_ERROR)
        {
            parms->data->ocx = 0;
        }
        else if (BIP_ADP_FS_DEVICE_BUSY(ret))
        {
            bip_adp_start_close_file_continually(parms->data->ocx);
        }
    }
    if (parms->obex.server->object) 
    {
        /* close any objects associated with the aborted operation */
        BIPOBS_Close(&(parms->obex.server->object));
    }
    bip_cntx_p->r_data_len = 0;
    if (bip_cntx_p->rState == BT_BIP_R_STATE_RESPONDING)
    {
        bip_adp_send_abort_ind(bip_cntx_p->r_cm_conn_id, bip_cntx_p->r_err_code);
        bip_cntx_p->r_err_code = BT_BIP_CNF_FAILED;
        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ACCEPTED);
    }
    else
    {
        BIP_ADP_R_STATE_TRANS(bip_cntx_p->rState, BT_BIP_R_STATE_ABORTING);
    }
#endif
    return;
}


/*****************************************************************************
*
* BIP ADP core function
*
*****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  bip_adp_deregister_initiator
 * DESCRIPTION
 *  This function is to deregister initiator
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_deregister_initiator(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/           
    ObStatus status;         
            
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//  BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_DEREGISTER_INITIATOR);

    /* deregister initiator */
    status = BIP_DeregisterInitiator(&(bip_cntx_p->BipInitiator));
    if (status != OB_STATUS_SUCCESS) 
    {
        EXT_ASSERT(0, status, 0, 0);
    }
    //Report(("Initiator Deregistered.\n"));
    
    /* reset context */
    bip_adp_initiator_cntx_deinit();
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_connect_cnf
 * DESCRIPTION
 *  This function is to send connect confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_connect_cnf(kal_uint8 req_id, kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_connect_cnf_struct* cnf = (bt_bip_connect_cnf_struct*) construct_local_para(sizeof(bt_bip_connect_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->req_id = req_id;
    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_CONNECT_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_capabilities_cnf
 * DESCRIPTION
 *  This function is to send get capabilities confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_capabilities_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_capabilities_cnf_struct* cnf = (bt_bip_get_capabilities_cnf_struct*) construct_local_para(sizeof(bt_bip_get_capabilities_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    cnf->preferred_format = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_format;
    cnf->preferred_maxsize = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_maxsize;
    cnf->preferred_trans = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_trans;
    cnf->preferred_pixel = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_pixel;
    memcpy(cnf->supported_img_formats, bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_formats, (BT_BIP_MAX_IMG_FORMATS * sizeof(bt_bip_img_format_enum)));
    memcpy(cnf->supported_img_size, bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_size, BT_BIP_MAX_IMG_FORMATS * sizeof(bt_bip_pixel_struct));
    memcpy(cnf->supported_maxsize, bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_maxsize, BT_BIP_MAX_IMG_FORMATS * sizeof(U32));
    
    cnf->created_time_filter = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.created_time_filter;
    cnf->modified_time_filter = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.modified_time_filter;
    cnf->encoding_filter = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.encoding_filter;
    cnf->pixel_filter = bip_cntx_p->i_xml_parsed_result.parsed_img_cap.pixel_filter;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_CAPABILITIES_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_img_list_cnf
 * DESCRIPTION
 *  This function is to send get image list confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_img_list_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_img_list_cnf_struct* cnf = (bt_bip_get_img_list_cnf_struct*) construct_local_para(sizeof(bt_bip_get_img_list_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    cnf->img_count = bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num;
    btmtk_wstrcpy((WCHAR*)cnf->img_list_path, BIP_ADP_IMG_LIST_FILE);
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_LIST_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_img_prop_cnf
 * DESCRIPTION
 *  This function is to send get image property confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_img_prop_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_img_prop_cnf_struct* cnf = (bt_bip_get_img_prop_cnf_struct*) construct_local_para(sizeof(bt_bip_get_img_prop_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    cnf->native_img = bip_cntx_p->i_xml_parsed_result.parsed_img_prop;
    cnf->variant_img_p = NULL;
    cnf->attachment_p = NULL;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_PROP_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_put_img_cnf
 * DESCRIPTION
 *  This function is to send put image confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_put_img_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code, kal_uint8* img_handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_put_img_cnf_struct* cnf = (bt_bip_put_img_cnf_struct*) construct_local_para(sizeof(bt_bip_put_img_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    if(img_handler != NULL)
    {
        strncpy((char*)cnf->img_handle, (char*)img_handler, BT_BIP_IMG_HANDLE_LEN);    
    }
    else
    {
        cnf->img_handle[0] = 0;
    }
    bip_adp_send_msg(MSG_ID_BT_BIP_PUT_IMG_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_put_linked_thumbnail_cnf
 * DESCRIPTION
 *  This function is to send put linked thumbnail confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_put_linked_thumbnail_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_put_linked_thumbnail_cnf_struct* cnf = (bt_bip_put_linked_thumbnail_cnf_struct*) construct_local_para(sizeof(bt_bip_put_linked_thumbnail_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_img_cnf
 * DESCRIPTION
 *  This function is to send get image confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_img_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code, kal_uint32 data_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_img_cnf_struct* cnf = (bt_bip_get_img_cnf_struct*) construct_local_para(sizeof(bt_bip_get_img_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    cnf->data_len = data_len;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_linked_thumbnail_cnf
 * DESCRIPTION
 *  This function is to send get linked thumbnail confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_linked_thumbnail_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_linked_thumbnail_cnf_struct* cnf = (bt_bip_get_linked_thumbnail_cnf_struct*) construct_local_para(sizeof(bt_bip_get_linked_thumbnail_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_monitoring_img_cnf
 * DESCRIPTION
 *  This function is to send get monitoring image confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_monitoring_img_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_monitoring_img_cnf_struct* cnf = (bt_bip_get_monitoring_img_cnf_struct*) construct_local_para(sizeof(bt_bip_get_monitoring_img_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_MONITORING_IMG_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_abot_cnf
 * DESCRIPTION
 *  This function is to send abort confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_abort_cnf(kal_uint32 cm_conn_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_abort_cnf_struct* cnf = (bt_bip_abort_cnf_struct*) construct_local_para(sizeof(bt_bip_abort_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->cm_conn_id = cm_conn_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_ABORT_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_activate_cnf
 * DESCRIPTION
 *  This function is to send activate confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_activate_cnf(kal_uint8 req_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_activate_cnf_struct* cnf = (bt_bip_activate_cnf_struct*) construct_local_para(sizeof(bt_bip_activate_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->req_id = req_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_ACTIVATE_CNF, cnf);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_deactivate_cnf
 * DESCRIPTION
 *  This function is to send deactivate confirm to MMI
 * PARAMETERS
 *  cm_conn_id
 *  cnf_code
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_deactivate_cnf(kal_uint8 req_id, kal_uint8 cnf_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_deactivate_cnf_struct* cnf = (bt_bip_deactivate_cnf_struct*) construct_local_para(sizeof(bt_bip_deactivate_cnf_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    cnf->req_id = req_id;
    cnf->cnf_code = cnf_code;
    bip_adp_send_msg(MSG_ID_BT_BIP_DEACTIVATE_CNF, cnf);
}


/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_authorize_ind
 * DESCRIPTION
 *  This function is to send authorize indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_authorize_ind(bt_bip_bd_addr_struct* dev_bd_addr, kal_uint8* dev_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_authorize_ind_struct* ind = (bt_bip_authorize_ind_struct*) construct_local_para(sizeof(bt_bip_authorize_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->bd_addr = *dev_bd_addr;
    memcpy(ind->dev_name, dev_name, BT_BIP_MAX_DEV_NAME_LEN);    
    bip_adp_send_msg(MSG_ID_BT_BIP_AUTHORIZE_IND, ind);
}


/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_connect_ind
 * DESCRIPTION
 *  This function is to send connect indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_connect_ind(kal_uint32 cm_conn_id, bt_bip_bd_addr_struct* dev_bd_addr, kal_uint8* dev_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_connect_ind_struct* ind = (bt_bip_connect_ind_struct*) construct_local_para(sizeof(bt_bip_connect_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    ind->bd_addr = *dev_bd_addr;
    memcpy(ind->dev_name, dev_name, BT_BIP_MAX_DEV_NAME_LEN);    
    bip_adp_send_msg(MSG_ID_BT_BIP_CONNECT_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_capabilities_ind
 * DESCRIPTION
 *  This function is to send get capability indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_capabilities_ind(kal_uint32 cm_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_capabilities_ind_struct* ind = (bt_bip_get_capabilities_ind_struct*) construct_local_para(sizeof(bt_bip_get_capabilities_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_CAPABILITIES_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_put_img_ind
 * DESCRIPTION
 *  This function is to send put image indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_put_img_ind(kal_uint32 cm_conn_id, BipImageReq* img_req, bt_bip_img_desc_struct* p_imginfo, U8 final_pkt_flag)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_put_img_ind_struct* ind = (bt_bip_put_img_ind_struct*) construct_local_para(sizeof(bt_bip_put_img_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    ASSERT(sizeof(ind->img_name) >= GOEP_MAX_UNICODE_LEN*2);
    memcpy(ind->img_name, img_req->name, GOEP_MAX_UNICODE_LEN*2);
    ind->img_size = p_imginfo->size;
    ind->img_descriptor = *p_imginfo;
    ind->r_last_pkt = final_pkt_flag;
    bip_adp_send_msg(MSG_ID_BT_BIP_PUT_IMG_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_put_linked_thumbnail_ind
 * DESCRIPTION
 *  This function is to send put linked thumbnail indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_put_linked_thumbnail_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle, U8 final_pkt_flag)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_put_linked_thumbnail_ind_struct* ind = (bt_bip_put_linked_thumbnail_ind_struct*) construct_local_para(sizeof(bt_bip_put_linked_thumbnail_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    memcpy(ind->img_handle, img_handle, BT_BIP_IMG_HANDLE_LEN);
	ind->r_last_pkt = final_pkt_flag;
    bip_adp_send_msg(MSG_ID_BT_BIP_PUT_LINKED_THUMBNAIL_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_img_list_ind
 * DESCRIPTION
 *  This function is to send get image indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_img_list_ind(kal_uint32 cm_conn_id, BipImgListingReq* img_list_req)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_img_list_ind_struct* ind = (bt_bip_get_img_list_ind_struct*) construct_local_para(sizeof(bt_bip_get_img_list_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    memset(&(ind->img_list_descriptor), 0, sizeof(bt_bip_img_info_struct));
    ind->max_img_handle_number = img_list_req->nbReturnedHandles;
    ind->start_index = img_list_req->listStartOffset;
    ind->latest_captured = (kal_bool) img_list_req->latestCapturedImages;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_LIST_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_img_prop_ind
 * DESCRIPTION
 *  This function is to send get image property indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_img_prop_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_img_prop_ind_struct* ind = (bt_bip_get_img_prop_ind_struct*) construct_local_para(sizeof(bt_bip_get_img_prop_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    memcpy(ind->img_handle, img_handle, BT_BIP_IMG_HANDLE_LEN);
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_PROP_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_img_ind
 * DESCRIPTION
 *  This function is to send get image indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_img_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_img_ind_struct* ind = (bt_bip_get_img_ind_struct*) construct_local_para(sizeof(bt_bip_get_img_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    memcpy(ind->img_handle, img_handle, BT_BIP_IMG_HANDLE_LEN);
    ind->img_descriptor = bip_cntx_p->r_xml_parsed_result.parsed_img_prop;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_linked_thumbnail_ind
 * DESCRIPTION
 *  This function is to send get linked thumbnail indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_linked_thumbnail_ind(kal_uint32 cm_conn_id, kal_uint8* img_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_linked_thumbnail_ind_struct* ind = (bt_bip_get_linked_thumbnail_ind_struct*) construct_local_para(sizeof(bt_bip_get_linked_thumbnail_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    memcpy(ind->img_handle, img_handle, BT_BIP_IMG_HANDLE_LEN);
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_LINKED_THUMBNAIL_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_get_monitoring_img_ind
 * DESCRIPTION
 *  This function is to send get monitoring image indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_get_monitoring_img_ind(kal_uint32 cm_conn_id, kal_uint8 store_flag)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_get_monitoring_img_ind_struct* ind = (bt_bip_get_monitoring_img_ind_struct*) construct_local_para(sizeof(bt_bip_get_monitoring_img_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    ind ->store_flag = store_flag;
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_MONITORING_IMG_IND, ind);
}


/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_final_packet_push_ind
 * DESCRIPTION
 *  This function is to send continue indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_final_packet_push_ind(kal_uint32 cm_conn_id, kal_uint32 obj_len, kal_uint32 data_len, U8 final_flag)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_continue_ind_struct* ind = (bt_bip_continue_ind_struct*) construct_local_para(sizeof(bt_bip_continue_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    ind->obj_len = obj_len;
    ind->data_len = data_len;
	ind->r_last_pkt = final_flag;
    bip_adp_send_msg(MSG_ID_BT_BIP_CONTINUE_IND, ind);
}


/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_continue_ind
 * DESCRIPTION
 *  This function is to send continue indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_continue_ind(kal_uint32 cm_conn_id, kal_uint32 obj_len, kal_uint32 data_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_continue_ind_struct* ind = (bt_bip_continue_ind_struct*) construct_local_para(sizeof(bt_bip_continue_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    ind->obj_len = obj_len;
    ind->data_len = data_len;
    bip_adp_send_msg(MSG_ID_BT_BIP_CONTINUE_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_abort_ind
 * DESCRIPTION
 *  This function is to send abort indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_abort_ind(kal_uint32 cm_conn_id, kal_uint8 ind_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_abort_ind_struct* ind = (bt_bip_abort_ind_struct*) construct_local_para(sizeof(bt_bip_abort_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->ind_code = ind_code;
    ind->cm_conn_id = cm_conn_id;
    bip_adp_send_msg(MSG_ID_BT_BIP_ABORT_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_complete_ind
 * DESCRIPTION
 *  This function is to send complete indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_complete_ind(kal_uint32 cm_conn_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_complete_ind_struct* ind = (bt_bip_complete_ind_struct*) construct_local_para(sizeof(bt_bip_complete_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    bip_adp_send_msg(MSG_ID_BT_BIP_COMPLETE_IND, ind);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_disconnect_ind
 * DESCRIPTION
 *  This function is to send disconnect indication to MMI
 * PARAMETERS
 *  cm_conn_id
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_disconnect_ind(kal_uint32 cm_conn_id, bt_bip_session_role_enum ses_role)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/       
    bt_bip_disconnect_ind_struct* ind = (bt_bip_disconnect_ind_struct*) construct_local_para(sizeof(bt_bip_disconnect_ind_struct), TD_RESET); 
    int result = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_conn_id;
    ind->session_role = ses_role;
    
    bip_adp_send_msg(MSG_ID_BT_BIP_DISCONNECT_IND, ind);

    result = btmtk_fs_delete_ucs2(BIP_ADP_SND_FILE);
    if (result < BTMTK_FS_OK)
    {
        BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CNTX_DEINIT_FS_DELETE_FAILED, result);
    } 
    result = btmtk_fs_delete_ucs2(BIP_ADP_RCV_FILE);        
    if (result < BTMTK_FS_OK)
    {
        BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CNTX_DEINIT_FS_DELETE_FAILED, result);
    }
    
}


/*****************************************************************************
*
* BIP ADP utility function
*
*****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  bip_adp_enter_usb_mode
 * DESCRIPTION
 *  This function is to process usb mode related fs termination
 * PARAMETERS
 *  *ilm_ptr
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_enter_usb_mode(void)
{
    if (bip_cntx_p->rData_p)
    {
        if (bip_cntx_p->rData_p->ocx != 0) 
        {
            btmtk_fs_close_ucs2(bip_cntx_p->rData_p->ocx);
            bip_cntx_p->rData_p->ocx = 0;
        }
    }
    if (bip_cntx_p->iData.ocx != 0)
    {
        btmtk_fs_close_ucs2(bip_cntx_p->iData.ocx);
        bip_cntx_p->iData.ocx = 0;
    }
    bip_adp_send_msg(MSG_ID_BT_BIP_ENTER_USB_MODE_CNF, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_check_fs_error_code
 * DESCRIPTION
 *  This function is to return disk full/root dir full if occurs
 * PARAMETERS
 *  fs_error
 *  fs_oper
 * RETURNS
 *  kal_uint8
 *****************************************************************************/
static kal_uint8 bip_adp_check_fs_error_code(int fs_error, bt_bip_adp_fs_oper_enum fs_oper)
{
   kal_uint8 bip_err;

   BT_BIP_FUNC_ENTRY2(BT_BIP_FUNC_CHECK_FS_ERROR, fs_error, fs_oper);	
   
    if(fs_error == BTMTK_FS_ERR_FULL)
    {
        bip_err = BT_BIP_DISK_FULL;
    }
    else if(fs_error == BTMTK_FS_ERR_ROOT_DIR_FULL)
    {
        bip_err = BT_BIP_ROOT_DIR_FULL;
    }
    else if(fs_error == BTMTK_FS_ERR_APP_QUOTA_FULL)
    {
        bip_err = BT_BIP_FS_QUOTA_FULL;
    }
    else if (fs_error == BTMTK_FS_ERR_WRITE_PROTECTION)
    {
        bip_err = BT_BIP_FS_WRITE_PROTECTION;
    }
    else if ((fs_error == BTMTK_FS_ERR_MEDIA_CHANGED) 
		      || (fs_error <= BTMTK_FS_ERR_MSDC_MOUNT_ERROR 
		          && fs_error >= BTMTK_FS_ERR_MSDC_NOT_PRESENT))
    {
		bip_err = BT_BIP_FS_MEDIA_CHANGED;
    }
    else
	{
	    switch (fs_oper)
	    {
	        case BT_BIP_ADP_FS_OPEN:
				bip_err = BT_BIP_FILE_OPEN_FAIL;
				break;
	        case BT_BIP_ADP_FS_READ:
				bip_err = BT_BIP_FILE_READ_FAIL;
				break;
	        case BT_BIP_ADP_FS_WRITE:
				bip_err = BT_BIP_FILE_WRITE_FAIL;
				break;
	        case BT_BIP_ADP_FS_MOVE:
				bip_err = BT_BIP_FILE_MOVE_FAIL;
				break;
			default:
				bip_err = BT_BIP_FILE_OPEN_FAIL;
				break;
	    }
	}

	
    return bip_err;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_ucs2_to_utf8
 * DESCRIPTION
 *  This function is to convert USC2 string to a UTF8 string
 * PARAMETERS
 *  ucs2_str        [IN]        The UCS2 string
 * RETURNS
 *  new UTF8 string
 *****************************************************************************/
static kal_uint8 *bip_adp_ucs2_to_utf8(kal_uint8 *ucs2_str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 *utf8_str;
    int utf8_str_len = btmtk_charset_ucs2_to_utf8_length_in_bytes((const kal_uint8*)ucs2_str);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    utf8_str = (kal_uint8*) get_ctrl_buffer((kal_uint16) (utf8_str_len + 1));
    btmtk_chset_ucs2_to_utf8_string(utf8_str, utf8_str_len + 1, ucs2_str);
    utf8_str[utf8_str_len] = '\0';
    return utf8_str;
}


/*****************************************************************************
 * FUNCTION
 *  bip_adp_utf8_to_ucs2
 * DESCRIPTION
 *  This function is to convert UTF8 string to a UCS2 string
 * PARAMETERS
 *  utf8_str        [IN]        The UTF8 string
 * RETURNS
 *  new UCS2 string
 *****************************************************************************/
static kal_wchar *bip_adp_utf8_to_ucs2(kal_uint8 *utf8_str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 *ucs2_str;
    int ucs2_str_len = btmtk_chset_utf8_to_ucs2_length_in_bytes((const kal_uint8*)utf8_str);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ucs2_str = (kal_uint8*) get_ctrl_buffer((kal_uint16) (ucs2_str_len + 2));
    btmtk_chset_utf8_to_ucs2_string(ucs2_str, ucs2_str_len + 2, utf8_str);
    ucs2_str[ucs2_str_len] = '\0';
    ucs2_str[ucs2_str_len + 1] = '\0';
    return (kal_wchar*) ucs2_str;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_send_msg
 * DESCRIPTION
 *  This function is to send confirm message to MMI
 * PARAMETERS
 *  msg_id, *p_local_para
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_send_msg(kal_uint16 msg_id, void *p_local_para)
{
#ifdef __ON_MAUI__
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_send;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
//  BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_SEND_MSG);

    ilm_send = allocate_ilm(MOD_BT);   
    ilm_send->src_mod_id = MOD_BT;
    ilm_send->dest_mod_id = MOD_MMI;
    ilm_send->sap_id = BT_BIP_SAP;
    ilm_send->msg_id = (kal_uint16) msg_id;
    ilm_send->local_para_ptr = (local_para_struct*) p_local_para;
    ilm_send->peer_buff_ptr = (peer_buff_struct*) NULL;    
    msg_send_ext_queue(ilm_send);
#else
    BT_SendMessage(
        msg_id, 
        MOD_EXTERNAL,
        p_local_para,
        ((local_para_struct*)p_local_para)->msg_len);
#endif

}

void bt_bip_adp_send_continue_xmlparse_msg_to_self(void)
{
#ifdef __ON_MAUI__
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ilm_struct *ilm_send;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ilm_send = allocate_ilm(MOD_BT);   
    ilm_send->src_mod_id = MOD_BT;
    ilm_send->dest_mod_id = MOD_BT;
    ilm_send->sap_id = BT_BIP_SAP;
    ilm_send->msg_id = (kal_uint16) MSG_ID_BT_BIP_RESUME_XMLPARSE_IND;
    ilm_send->local_para_ptr = (local_para_struct*) NULL;
    ilm_send->peer_buff_ptr = (peer_buff_struct*) NULL;    
    msg_send_ext_queue(ilm_send);
#else
    BT_SendMessage(
        MSG_ID_BT_BIP_RESUME_XMLPARSE_IND, 
        MOD_BT,
        NULL,
        0);
#endif
}


void bip_adp_resume_xml_parse(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int result;
    bip_adp_xmlparse_end_enum parse_rst = BIP_ADP_XMLPARSE_FAILED;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bip_cntx_p->i_xml_parser_p == NULL)
    {
		kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_RESUME_PARSE_NULL,bip_cntx_p->iState);	
        if (bip_cntx_p->iState == BT_BIP_I_STATE_XMLPARSING)
        {
            /* notify mmi the parsing result: something error */
            bip_adp_parse_imagelist_xml_file_end(parse_rst);

            /* transfer status to CONNECTED */
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
        }
        
        return;
    }
    
    BT_BIP_I_STATE(bip_cntx_p->iState);
    /* check state first */
    if (bip_cntx_p->iState != BT_BIP_I_STATE_XMLPARSING)
    {
        /* maybe parsing is cancelled by user */
        //Report(("[BIP ADP] bip_adp_resume_xml_parse: iState changed!"));
 
        bip_adp_parse_imagelist_xml_file_end(BIP_ADP_XMLPARSE_USERCANCELED);

        return;
    }

	/* clear the flag */
    bip_cntx_p->i_parse_paused = FALSE;
    
	/* resume parsing */
	result = xml_resume_parse(bip_cntx_p->i_xml_parser_p);

	/* check the parsing result: paused or finished */
    if (bip_cntx_p->i_parse_paused)
    {
        /* to be continue */
		kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_PARSE_PAUSED);	
        return;
    }
    else /* parsing finished */
    {
        //Report(("[BIP ADP] bip_adp_resume_xml_parse: finished"));

        parse_rst = (result == XML_RESULT_OK ? BIP_ADP_XMLPARSE_SUCCESS : BIP_ADP_XMLPARSE_FAILED);
        /* notify mmi the parsing result */
        bip_adp_parse_imagelist_xml_file_end(parse_rst);

        /* transfer status to CONNECTED */
        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
    }

}

/* now we thinke buffer will always enough */
static void bip_adp_generate_pixel(bt_bip_pixel_struct* pixel, kal_uint8* buf)
{
    if (!pixel->height2 && !pixel->width2)
    {
        sprintf((char*)buf, "%d*%d", pixel->width, pixel->height);
    }
    else if (pixel->height2 && pixel->width2)
    {
        sprintf((char*)buf, "%d*%d-%d*%d", pixel->width, pixel->height, pixel->width2, pixel->height2);
    }
    else
    {
        sprintf((char*)buf, "%d**-%d*%d", pixel->width, pixel->width2, pixel->height2);
    }
}
/*****************************************************************************
 * FUNCTION
 *  bip_adp_generate_xml_capabilities
 * DESCRIPTION
 *  This function is to generate capabilities xml
 * PARAMETERS
 *  msg_id, *p_local_para
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_generate_xml_capabilities(bt_bip_xml_composer_struct *composer, void* local_para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_get_capabilities_rsp_struct* rsp = (bt_bip_get_capabilities_rsp_struct*) local_para;
    kal_uint8 attr[10][BT_BIP_MAX_ATTR_NAME_LEN];
    kal_uint8* attr_p[10];
    int i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_GENERATE_XML_CAPABILITIES);


    for(i = 0; i < 10; i++)
    {
        attr_p[i] = attr[i];
    }

    strcpy((char*)attr[0], BIP_ADP_STR_VERSION);
    strcpy((char*)attr[1], BIP_ADP_STR_1_0);
    attr[2][0] = 0;
    attr[3][0] = 0;
    bip_adp_compose_startElement(composer, (const char *)BIP_ADP_STR_IMG_CAP, (const char **) attr_p);

    if(rsp->preferred_format)
    {
        strcpy((char*)attr[0], BIP_ADP_STR_ENC);
        attr_p[0] = attr[0];
        switch(rsp->preferred_format)
        {
            case BT_BIP_IMG_TYPE_JPEG:
                strcpy((char*)attr[1], BIP_ADP_STR_JPEG);
                break;
            case BT_BIP_IMG_TYPE_BMP:
                strcpy((char*)attr[1], BIP_ADP_STR_BMP);
                break;
            case BT_BIP_IMG_TYPE_GIF:
                strcpy((char*)attr[1], BIP_ADP_STR_GIF);
                break;
            case BT_BIP_IMG_TYPE_WBMP:
                strcpy((char*)attr[1], BIP_ADP_STR_WBMP);
                break;
            case BT_BIP_IMG_TYPE_PNG:
                strcpy((char*)attr[1], BIP_ADP_STR_PNG);
                break;
            default:
                ASSERT(0);
        }
        i = 2;
        if (rsp->preferred_pixel.width)
        {
            strcpy((char*)attr[i++], BIP_ADP_STR_PIXEL);
            bip_adp_generate_pixel(&rsp->preferred_pixel, attr[i++]);
        }
        if (rsp->preferred_trans && rsp->preferred_trans < BT_BIP_TRANS_MAX)
        {
            strcpy((char*)attr[i++], BIP_ADP_STR_TRANSFORMATION);
            strcpy((char*)attr[i++], bip_img_trans[rsp->preferred_trans]);
        }
        if (rsp->preferred_maxsize)
        {
            strcpy((char*)attr[i++], BIP_ADP_STR_MAXSIZE);
            sprintf((char*)attr[i++], "%lu", rsp->preferred_maxsize);
        }
            
        attr[i++][0] = 0;
        attr[i++][0] = 0;
        bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_PREFER_FORMAT, (const char **)attr_p);
    }

    for(i = 0; i < 10 && rsp->supported_img_formats[i] != BT_BIP_IMG_TYPE_UNSUPPORT; i++)
    {
        j = 0;
        strcpy((char*)attr[0], BIP_ADP_STR_ENC);
        switch(rsp->supported_img_formats[i])
        {
            case BT_BIP_IMG_TYPE_JPEG:
                strcpy((char*)attr[1], BIP_ADP_STR_JPEG);
                break;
            case BT_BIP_IMG_TYPE_BMP:
                strcpy((char*)attr[1], BIP_ADP_STR_BMP);
                break;
            case BT_BIP_IMG_TYPE_GIF:
                strcpy((char*)attr[1], BIP_ADP_STR_GIF);
                break;
            case BT_BIP_IMG_TYPE_WBMP:
                strcpy((char*)attr[1], BIP_ADP_STR_WBMP);
                break;
            case BT_BIP_IMG_TYPE_PNG:
                strcpy((char*)attr[1], BIP_ADP_STR_PNG);
                break;
            default:
                ASSERT(0);
        }
        j = 2;
        if (rsp->supported_img_size[i].width)
        {
            strcpy((char*)attr[j++], BIP_ADP_STR_PIXEL);
            //strcpy((char*)attr[Ii++], "1*1-65535*65535");
            bip_adp_generate_pixel(&rsp->supported_img_size[i], attr[j++]);
        }
        if (rsp->supported_maxsize[i])
        {
            strcpy((char*)attr[j++], BIP_ADP_STR_MAXSIZE);
            sprintf((char*)attr[j++], "%lu", rsp->supported_maxsize[i]);
        }
        attr[j++][0] = 0;
        attr[j++][0] = 0;
        bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_IMG_FORMAT, (const char **)attr_p);
    }

    strcpy((char*)attr[0], BIP_ADP_STR_CREATED);
    if(rsp->created_time_filter)
    {
        strcpy((char*)attr[1], BIP_ADP_STR_1);
    }
    else
    {
        strcpy((char*)attr[1], BIP_ADP_STR_0);
    }

    strcpy((char*)attr[2], BIP_ADP_STR_MODIFIED);
    if(rsp->modified_time_filter)
    {
        strcpy((char*)attr[3], BIP_ADP_STR_1);
    }
    else
    {
        strcpy((char*)attr[3], BIP_ADP_STR_0);
    }
    strcpy((char*)attr[4], BIP_ADP_STR_ENC);
    if(rsp->encoding_filter)
    {
        strcpy((char*)attr[5], BIP_ADP_STR_1);
    }
    else
    {
        strcpy((char*)attr[5], BIP_ADP_STR_0);
    }

    strcpy((char*)attr[6], BIP_ADP_STR_PIXEL);
    if(rsp->pixel_filter)
    {
        strcpy((char*)attr[7], BIP_ADP_STR_1);
    }
    else
    {
        strcpy((char*)attr[7], BIP_ADP_STR_0);
    }

    attr[8][0] = 0;
    attr[9][0] = 0;
    bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_FILTER_PARA, (const char **)attr_p);
    bip_adp_compose_endElement(composer, BIP_ADP_STR_IMG_CAP);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_generate_xml_img_list
 * DESCRIPTION
 *  This function is to generate image list xml
 * PARAMETERS
 *  msg_id, *p_local_para
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_generate_xml_img_list(bt_bip_xml_composer_struct *composer, void* local_para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_get_img_list_rsp_struct* rsp = (bt_bip_get_img_list_rsp_struct*) local_para;
    kal_uint8 attr[10][BT_BIP_MAX_ATTR_NAME_LEN];
    int i, len, ret, bytes_read = 0;
    BipOcx bip_fh;
    bt_bip_img_min_info_struct img_info;
    kal_uint8* attr_p[10];
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_GENERATE_XML_IMG_LIST);
    for(i = 0; i < 10; i++)
    {
        attr_p[i] = attr[i];
    }
    strcpy((char*)attr[0], BIP_ADP_STR_VERSION);
    strcpy((char*)attr[1], BIP_ADP_STR_1_0);
    attr[2][0] = 0;
    attr[3][0] = 0;
    bip_adp_compose_startElement(composer, (const char *)BIP_ADP_STR_IMG_LIST, (const char **)attr_p);

    len = sizeof(bt_bip_img_min_info_struct);

    bip_fh = btmtk_fs_open_ucs2((WCHAR*)rsp->img_list_path, BTMTK_FS_READ_ONLY);

    if(bip_fh >= 0)
    {
        for (i = 0; i < rsp->img_count; i++)
        {
            ret = btmtk_fs_read_ucs2(bip_fh, (void*)&img_info, len,  &bytes_read);
            if (ret != BTMTK_FS_OK || bytes_read != len)
            {
                btmtk_fs_close_ucs2(bip_fh);
                return;
            }
            strcpy((char*)attr[0], BIP_ADP_STR_HENDLE);
            memcpy((char*)attr[1], img_info.img_handle, BT_BIP_IMG_HANDLE_LEN);
            strcpy((char*)attr[2], BIP_ADP_STR_CREATED);
            memcpy((char*)attr[3], img_info.created, BT_BIP_MAX_TIME_LEN);
            attr[4][0] = 0;
            attr[5][0] = 0;
            /*            
            strcpy((char*)attr[4], BIP_ADP_STR_MODIFIED);
            memcpy((char*)attr[5], img_info.modified, BT_BIP_MAX_TIME_LEN);
            attr[6][0] = 0;
            attr[7][0] = 0;
            */
            bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_IMG, (const char **)attr_p);
        }
        btmtk_fs_close_ucs2(bip_fh);
    }
    bip_adp_compose_endElement(composer, BIP_ADP_STR_IMG_LIST);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_generate_xml_img_prop
 * DESCRIPTION
 *  This function is to generate image list xml
 * PARAMETERS
 *  msg_id, *p_local_para
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_generate_xml_img_prop(bt_bip_xml_composer_struct *composer, void* local_para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_get_img_prop_rsp_struct* rsp = (bt_bip_get_img_prop_rsp_struct*) local_para;
    kal_uint8 attr[10][BT_BIP_MAX_ATTR_NAME_LEN];
    kal_uint8* attr_p[10];
    int i, j, k;
 
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_GENERATE_XML_IMG_PROP);
    for(i = 0; i < 10; i++)
    {
        attr_p[i] = attr[i];
    }
    strcpy((char*)attr[0], BIP_ADP_STR_VERSION);
    strcpy((char*)attr[1], BIP_ADP_STR_1_0);
    strcpy((char*)attr[2], BIP_ADP_STR_HENDLE);
    strcpy((char*)attr[3], (char*)rsp->img_handle);
    attr[4][0] = 0;
    attr[5][0] = 0;
    bip_adp_compose_startElement(composer, (const char *)BIP_ADP_STR_IMG_PROP, (const char **)attr_p);
    j = 0;
    if(rsp->native_img.encoding)
    {
        strcpy((char*)attr[j++], BIP_ADP_STR_ENC);
        switch(rsp->native_img.encoding)
        {
            case BT_BIP_IMG_TYPE_JPEG:
                strcpy((char*)attr[j++], BIP_ADP_STR_JPEG);
                break;
            case BT_BIP_IMG_TYPE_BMP:
                strcpy((char*)attr[j++], BIP_ADP_STR_BMP);
                break;
            case BT_BIP_IMG_TYPE_GIF:
                strcpy((char*)attr[j++], BIP_ADP_STR_GIF);
                break;
            case BT_BIP_IMG_TYPE_WBMP:
                strcpy((char*)attr[j++], BIP_ADP_STR_WBMP);
                break;
            case BT_BIP_IMG_TYPE_PNG:
                strcpy((char*)attr[j++], BIP_ADP_STR_PNG);
                break;
            default:
                ASSERT(0);
        }
    }
    if ((rsp->native_img.specified_pixel_width) && (rsp->native_img.specified_pixel_height))
    {
        strcpy((char*)attr[j++], BIP_ADP_STR_PIXEL);    
        kal_sprintf((char*)attr[j++], "%d*%d" ,rsp->native_img.specified_pixel_width, rsp->native_img.specified_pixel_height);
    }
    
    if (rsp->native_img.size)
    {
        strcpy((char*)attr[j++], BIP_ADP_STR_SIZE);
        kal_sprintf((char*)attr[j++], "%lu", rsp->native_img.size);
    }
    strcpy((char*)attr[j++], BIP_ADP_STR_FRIENDLY_NAME);
    k = j;
    attr_p[j++] = bip_adp_ucs2_to_utf8((kal_uint8*) rsp->native_img.friendly_name);

    attr[j++][0] = 0;
    attr[j++][0] = 0;
    bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_NATIVE, (const char **)attr_p);
    free_ctrl_buffer(attr_p[k]);
    bip_adp_compose_endElement(composer, BIP_ADP_STR_IMG_PROP);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_generate_xml_get_img_descriptor
 * DESCRIPTION
 *  This function is to generate image descriptor xml for get request
 * PARAMETERS
 *  msg_id, *p_local_para
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_generate_xml_get_img_descriptor(bt_bip_xml_composer_struct *composer, void* local_para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_get_img_req_struct* rsp = (bt_bip_get_img_req_struct*) local_para;
    kal_uint8 attr[10][BT_BIP_MAX_ATTR_NAME_LEN];
    kal_uint8* attr_p[10];
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_GENERATE_XML_CAPABILITIES);

    for(i = 0; i < 10; i++)
    {
        attr_p[i] = attr[i];
    }

    strcpy((char*)attr[0], BIP_ADP_STR_VERSION);
    strcpy((char*)attr[1], BIP_ADP_STR_1_0);
    attr[2][0] = 0;
    attr[3][0] = 0;
    bip_adp_compose_startElement(composer, (const char *)BIP_ADP_STR_IMG_DESC, (const char **) attr_p);

    if(rsp->img_descriptor.encoding)
    {
        strcpy((char*)attr[0], BIP_ADP_STR_ENC);
        attr_p[0] = attr[0];
        switch(rsp->img_descriptor.encoding)
        {
            case BT_BIP_IMG_TYPE_JPEG:
                strcpy((char*)attr[1], BIP_ADP_STR_JPEG);
                break;
            case BT_BIP_IMG_TYPE_BMP:
                strcpy((char*)attr[1], BIP_ADP_STR_BMP);
                break;
            case BT_BIP_IMG_TYPE_GIF:
                strcpy((char*)attr[1], BIP_ADP_STR_GIF);
                break;
            case BT_BIP_IMG_TYPE_WBMP:
                strcpy((char*)attr[1], BIP_ADP_STR_WBMP);
                break;
            case BT_BIP_IMG_TYPE_PNG:
                strcpy((char*)attr[1], BIP_ADP_STR_PNG);
                break;
            default:
                ASSERT(0);
        }
        strcpy((char*)attr[2], BIP_ADP_STR_PIXEL);
        if(rsp->img_descriptor.specified_pixel_width && rsp->img_descriptor.specified_pixel_height)
        {
            kal_sprintf((char*)attr[3], "%d*%d", rsp->img_descriptor.specified_pixel_width, rsp->img_descriptor.specified_pixel_height);
        }
        else
        {
            strcpy((char*)attr[3], "160*120");
        }
        attr[4][0] = 0;
        attr[5][0] = 0;
        bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_IMG, (const char **)attr_p);
    }
    bip_adp_compose_endElement(composer, BIP_ADP_STR_IMG_DESC);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_generate_xml_img_descriptor
 * DESCRIPTION
 *  This function is to generate image descriptor xml
 * PARAMETERS
 *  msg_id, *p_local_para
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_generate_xml_img_descriptor(bt_bip_xml_composer_struct *composer, void* local_para)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_bip_put_img_req_struct* rsp = (bt_bip_put_img_req_struct*) local_para;
    kal_uint8 attr[10][BT_BIP_MAX_ATTR_NAME_LEN];
    kal_uint8* attr_p[10];
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_GENERATE_XML_CAPABILITIES);

    for(i = 0; i < 10; i++)
    {
        attr_p[i] = attr[i];
    }

    strcpy((char*)attr[0], BIP_ADP_STR_VERSION);
    strcpy((char*)attr[1], BIP_ADP_STR_1_0);
    attr[2][0] = 0;
    attr[3][0] = 0;
    bip_adp_compose_startElement(composer, (const char *)BIP_ADP_STR_IMG_DESC, (const char **) attr_p);

    if(rsp->img_descriptor.encoding)
    {
        strcpy((char*)attr[0], BIP_ADP_STR_ENC);
        attr_p[0] = attr[0];
        switch(rsp->img_descriptor.encoding)
        {
            case BT_BIP_IMG_TYPE_JPEG:
                strcpy((char*)attr[1], BIP_ADP_STR_JPEG);
                break;
            case BT_BIP_IMG_TYPE_BMP:
                strcpy((char*)attr[1], BIP_ADP_STR_BMP);
                break;
            case BT_BIP_IMG_TYPE_GIF:
                strcpy((char*)attr[1], BIP_ADP_STR_GIF);
                break;
            case BT_BIP_IMG_TYPE_WBMP:
                strcpy((char*)attr[1], BIP_ADP_STR_WBMP);
                break;
            case BT_BIP_IMG_TYPE_PNG:
                strcpy((char*)attr[1], BIP_ADP_STR_PNG);
                break;
            default:
                ASSERT(0);
        }
    }
    strcpy((char*)attr[2], BIP_ADP_STR_PIXEL);
    if(rsp->img_descriptor.pixel.width)
    {
        /*kal_sprintf((char*)attr[3], "%d*%d", rsp->img_descriptor.specified_pixel_width, rsp->img_descriptor.specified_pixel_height); */
        bip_adp_generate_pixel(&rsp->img_descriptor.pixel, attr[3]);
    }
    else
    {
        strcpy((char*)attr[3], "320*240");
    }
    strcpy((char*)attr[4], BIP_ADP_STR_SIZE);
    kal_sprintf((char*)attr[5], "%lu", rsp->img_descriptor.size);
    i = 6;
    if (rsp->img_descriptor.transformation && rsp->img_descriptor.transformation < BT_BIP_TRANS_MAX)
    {
        strcpy((char*)attr[i++], BIP_ADP_STR_TRANSFORMATION);
        strcpy((char*)attr[i++], bip_img_trans[rsp->img_descriptor.transformation]);
    }
        
    attr[i++][0] = 0;
    attr[i++][0] = 0;
    bip_adp_compose_inline_attribute_element(composer, (const char *)BIP_ADP_STR_IMG, (const char **)attr_p);

    bip_adp_compose_endElement(composer, BIP_ADP_STR_IMG_DESC);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_find_session_with_cm_conn_id
 * DESCRIPTION
 *  This function is to find out responding session with cm_conn_id
 * PARAMETERS
 *  kal_uint32 cm_conn_id
 * RETURNS
 *  bt_bip_session_role_enum
 *****************************************************************************/
static bt_bip_session_role_enum bip_adp_find_session_with_cm_conn_id(kal_uint32 cm_conn_id)
{   
    if (bip_cntx_p->r_cm_conn_id == cm_conn_id)
    {
        return BT_BIP_RESPONDER_ROLE;
    }
    else //attar : when in no cm_conn_id scenario if (bip_cntx_p->i_cm_conn_id == cm_conn_id)
    {
        return BT_BIP_INITIATOR_ROLE;        
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_write_buf2file
 * DESCRIPTION
 *  
 * PARAMETERS
 *  composer        [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_write_buf2file(bt_bip_xml_composer_struct *composer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (composer->buflen >= BIP_ADP_WRITE_BUF_THRESHOLD)
    {
        ret = btmtk_fs_write_ucs2(composer->fd, composer->buf, composer->buflen, (S32*)&(composer->written));

        if (ret >= 0)
        {
            memset(composer->buf, 0, BIP_ADP_MAX_COMPOSE_BUF_SIZE);
            composer->buflen = 0;
            composer->written = 0;
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_xml_new_composer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  XML_Composer        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_xml_new_composer(bt_bip_xml_composer_struct **composer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *composer = get_ctrl_buffer(sizeof(bt_bip_xml_composer_struct));
    memset(*composer, 0, sizeof(bt_bip_xml_composer_struct));
    (*composer)->buflen = 0;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_xml_close_composer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  XML_Composer        [?]     
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_xml_close_composer(bt_bip_xml_composer_struct *composer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    free_ctrl_buffer(composer);
    return;
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_compose_startElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  composer        [?]         
 *  el              [IN]        
 *  attr            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_compose_startElement(bt_bip_xml_composer_struct *composer, const char *el, const char **attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    composer->buflen += sprintf((char*)(composer->buf + composer->buflen), "<%s", el);
    if (attr[0] != NULL)
    {
        while (attr[i][0] != 0)
        {
            composer->buflen += sprintf((char*)(composer->buf + composer->buflen), " %s=", attr[i]);
            composer->buflen += sprintf((char*)(composer->buf + composer->buflen), "\"%s\"", attr[i + 1]);
            i += 2;
        }
    }
    composer->buflen += sprintf((char*)(composer->buf + composer->buflen), ">\r\n");
    bip_adp_write_buf2file(composer);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_compose_endElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  composer        [?]         
 *  el              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_compose_endElement(bt_bip_xml_composer_struct *composer, const char *el)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    composer->buflen += sprintf((char*)(composer->buf + composer->buflen), "</%s>\r\n", el);
    bip_adp_write_buf2file(composer);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_compose_inline_attribute_element
 * DESCRIPTION
 *  
 * PARAMETERS
 *  composer        [?]         
 *  el              [IN]        
 *  attr            [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bip_adp_compose_inline_attribute_element(bt_bip_xml_composer_struct *composer, const char *el, const char **attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    composer->buflen += sprintf((char*)(composer->buf + composer->buflen), "<%s", el);
    if (attr[0] != NULL)
    {
        while (attr[i][0] != 0)
        {
            composer->buflen += sprintf((char*)(composer->buf + composer->buflen), " %s=", attr[i]);
            composer->buflen += sprintf((char*)(composer->buf + composer->buflen), "\"%s\"", attr[i + 1]);
            i += 2;
        }
    }
    composer->buflen += sprintf((char*)(composer->buf + composer->buflen), "/>\r\n");
    bip_adp_write_buf2file(composer);
}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_generate_xml_file
 * DESCRIPTION
 *  
 * PARAMETERS
 *  imps_msg_type           [IN]        
 *  transaction_content     [?]         
 * RETURNS
 *  
 *****************************************************************************/
static int bip_adp_generate_xml_file(int xml_type, void* rsp)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 ret = 0;
    bt_bip_xml_composer_struct *composer = bip_cntx_p->xml_composer_p;    

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    bip_adp_xml_new_composer(&composer);
    //BIPOP_GET_IMAGE

    ret = btmtk_fs_open_ucs2(BIP_ADP_SND_FILE, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));

    if (ret < 0)
    {
        BT_BIP_LOG1(BT_BIP_GEN_XML_FS_OPEN_FAILED, ret);
        bip_adp_xml_close_composer(composer);
        composer = NULL;
        return ret;
    }
    composer->fd = ret;

    switch(xml_type)
    {
        case BIPOP_GET_CAPABILITIES:
            bip_adp_generate_xml_capabilities(composer, rsp);
            break;
        case BIPOP_GET_IMAGES_LIST:
            bip_adp_generate_xml_img_list(composer, rsp);
            break;
        case BIPOP_GET_IMAGE_PROPERTIES:
            bip_adp_generate_xml_img_prop(composer, rsp);
            break;
        case BIPOP_PUT_IMAGE:
            bip_adp_generate_xml_img_descriptor(composer, rsp);
            break;
        case BIPOP_GET_IMAGE:
            bip_adp_generate_xml_get_img_descriptor(composer, rsp);
            break;
        default: 
            ASSERT(0);
    }

    ret = btmtk_fs_write_ucs2(composer->fd, composer->buf, composer->buflen, (S32*)&(composer->written));

    if (ret < 0)
    {
        btmtk_fs_close_ucs2(composer->fd);
        bip_adp_xml_close_composer(composer);
        composer = NULL;
        BT_BIP_LOG1(BT_BIP_GEN_XML_FS_WRITE_FALED,ret);
        return ret;
    }

    btmtk_fs_close_ucs2(composer->fd);
    bip_adp_xml_close_composer(composer);
    composer = NULL;
    return 0;
}

static int bip_adp_parse_imagelist_xml_file_start(XML_PARSER_STRUCT ** p_xml_parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int result = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/   
    /* notify MMI: adp start parsing */
    bip_adp_send_msg(MSG_ID_BT_BIP_GET_IMG_LIST_PARSING_IND, NULL);

    BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_XMLPARSING);

    /* clear the flag */
    bip_cntx_p->i_parse_paused = FALSE;

    /* start parsing */
    result = bip_adp_parse_xml_file(BIPOP_GET_IMAGES_LIST, BIP_ADP_RCV_FILE, p_xml_parser);

    return result;
}

static void bip_adp_parse_imagelist_xml_file_end(bip_adp_xmlparse_end_enum result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 cnf_code;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/   
	kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_PARSE_IMGLIST_END, bip_cntx_p->iState, result);
    
    /* 1. free BIP buffer & close file */
    bt_adp_parsing_imglist_end();

    /* 2. close parser and free parser buffer */
    if (bip_cntx_p->i_xml_parser_p != NULL)
    {
        xml_close_parser(bip_cntx_p->i_xml_parser_p);
        free_ctrl_buffer(bip_cntx_p->i_xml_parser_p);
        bip_cntx_p->i_xml_parser_p = NULL;
    }

    /* 3. precess according parsing result */
    if (result == BIP_ADP_XMLPARSE_USERCANCELED)
	{
		/* canceled by user, don't notify MMI */

        /* delete parsed image-list file */
        btmtk_fs_delete_ucs2(BIP_ADP_IMG_LIST_FILE);        
	}
	else
	{   
		/* notify the MMI the parsing result */

		if (result == BIP_ADP_XMLPARSE_TPDISCONNECTED)
		{
			/* TP disconnected */
			cnf_code = (kal_int8)BT_BIP_DISCONNECTED;
 
			/* delete parsed image-list file */
			btmtk_fs_delete_ucs2(BIP_ADP_IMG_LIST_FILE);        
		}
		else if(result == BIP_ADP_XMLPARSE_SUCCESS || 
			bip_cntx_p->iData.i.iList.nbReturnedHandles == 0) /* parse succeed */
		{
			if(bip_cntx_p->i_rspcode == OBRC_PARTIAL_CONTENT)
			{
				cnf_code = BT_BIP_PARTIAL_CONTENT;
			}
			else
			{
				cnf_code = BT_BIP_CNF_SUCCESS;
			}
			/*
			if(bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num == 0)
			{
				bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num = parms->data->i.iList.nbReturnedHandles;
			}
			*/
		}                        
		else  /* parse has some error */
		{
			if (bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num > 0)
			{
				cnf_code = BT_BIP_PARTIAL_CONTENT;
			}
			else
			{
				 cnf_code = BT_BIP_XML_PARSE_FAIL;
			}
		}

        /* send msg to MMI */
        bip_adp_send_get_img_list_cnf(bip_cntx_p->i_cm_conn_id, cnf_code);

	}

    /* delete the parsed tmp xml file */
    btmtk_fs_delete_ucs2(BIP_ADP_RCV_FILE);

    /* clear the data info */
    bip_cntx_p->i_data_len = 0;
    memset(&(bip_cntx_p->i_xml_parsed_result), 0, sizeof(bt_bip_i_xml_parsed_info_union));
    memset(&(bip_cntx_p->iData), 0, sizeof(BipData));

    bip_cntx_p->i_parse_paused = FALSE;  

}

/*****************************************************************************
 * FUNCTION
 *  bip_adp_parse_xml_file
 * DESCRIPTION
 *  
 * PARAMETERS
 *  filename        [?]     
 * RETURNS
 *  
 *****************************************************************************/
static int bip_adp_parse_xml_file(kal_uint32 xml_response_type, kal_wchar* filename, XML_PARSER_STRUCT ** p_xml_parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int result = 0;
    XML_PARSER_STRUCT * parser;
    int ret = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (*p_xml_parser == NULL)
    {
        *p_xml_parser = get_ctrl_buffer(sizeof(XML_PARSER_STRUCT));
    }

    parser = *p_xml_parser;
    memset(parser, 0, sizeof(XML_PARSER_STRUCT));
    result = xml_new_parser(parser);
    if (result)
    {
        xml_close_parser(parser);
        free_ctrl_buffer(parser);
        *p_xml_parser = NULL;
        return -1;
    }

    xml_register_data_handler(parser, bt_bip_ignore_parse_dataElement);
    switch(xml_response_type)
    {
        case BIPOP_GET_CAPABILITIES:
            {
                xml_register_element_handler(parser, bt_bip_parse_img_cap_startElement, bt_bip_parse_img_cap_endElement);
            }
            break;

        case BIPOP_GET_IMAGE_PROPERTIES:
            {
                xml_register_element_handler(parser, bt_bip_parse_img_prop_startElement, bt_bip_parse_img_prop_endElement);
            }
        	break;

        case BIPOP_GET_IMAGES_LIST:
            {
                xml_register_element_handler(parser, bt_bip_parse_img_list_startElement, bt_bip_parse_img_list_endElement);
            }
        	break;

        case BIPOP_PUT_IMAGE:
        case BIPOP_GET_IMAGE:
            {
                xml_register_element_handler(parser, bt_bip_parse_img_descriptor_startElement, bt_bip_parse_img_descriptor_endElement);
            }
        	break;

        default:
        	ASSERT(0);
    }

    result = xml_parse(parser, (U8*)filename);

    if (result)
    {
	    kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_XMLPARSE_RETURN,result);
        ret = -1;
    }
    else
    {
        ret = 0;
    }

	if (xml_response_type == BIPOP_GET_IMAGES_LIST)
    {   
        if (bip_cntx_p->i_parse_paused)
        {
            /* to be continue */
            return 0;
    }
        else /* finished */
        {
            bip_adp_xmlparse_end_enum parse_rst;
            
            parse_rst = (result == XML_RESULT_OK ? BIP_ADP_XMLPARSE_SUCCESS : BIP_ADP_XMLPARSE_FAILED);

            /* free resources, and notify mmi the parsing result */
            bip_adp_parse_imagelist_xml_file_end(parse_rst);

            /* transfer status to CONNECTED */
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
        }
    }
    else /* other type */
    {
        xml_close_parser(parser);
        free_ctrl_buffer(parser);
        
        /* clear the pointer */
        *p_xml_parser = NULL;
    }

    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_cap_dataElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 *  value       [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_bip_ignore_parse_dataElement(void *data, const char *el, const char *value, kal_int32 len, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Do nothing */
    return;
}

int bip_adp_write_parsed_imglist_buf2file(kal_uint8 *p_buf, kal_uint32 buflen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int result = 0;
    kal_uint32 written_bytes = 0;
    kal_uint32 written_item = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    result = btmtk_fs_write_ucs2(bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh, 
               p_buf, 
               buflen, (S32*)&written_bytes);

    if (result < 0)
        BT_BIP_LOG2(BT_BIP_ADP_FS_WRITE_RET, result, 10);

    written_item = written_bytes / sizeof(bt_bip_img_min_info_struct);
    bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_item_num += (U16)written_item;
    bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_num_interval += (U16)written_item;

    return result;
}

int bip_adp_write_parsed_imglist_item(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int result = 0;
    kal_uint8 *p_buf = bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer;
    kal_uint32 buflen = bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.buflen;
    kal_uint32 item_size = sizeof(bt_bip_img_min_info_struct);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* obj -> buffer */
    ASSERT(p_buf);
    
    memcpy(
        &(p_buf[buflen]),
        &(bip_cntx_p->i_xml_parsed_result.parsed_img_list.parsed_img_item), 
        item_size
        );
    buflen += item_size;

    /* buffer -> file, if need */
    if (buflen > (BIP_ADP_MAX_COMPOSE_BUF_SIZE - item_size))
    {
        result = bip_adp_write_parsed_imglist_buf2file(p_buf, buflen);
 
        memset(p_buf, 0, BIP_ADP_MAX_COMPOSE_BUF_SIZE);
        buflen = 0;
    }

    bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.buflen = buflen;
    
    return result;
}

void bt_adp_parsing_imglist_end(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* write the data in buffer to the file, if any */
    if (bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer &&
        bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.buflen > 0)
    {
        bip_adp_write_parsed_imglist_buf2file(
                    bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer,
                    bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.buflen);
    }

    /* free the buffer, if need */
    if (bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer)
	{
        free_ctrl_buffer(bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer);
        bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer = NULL;  
		bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.buflen = 0;
	}
    
    /* close the file, if need */
	if (bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh > 0)
	{
		ret = btmtk_fs_close_ucs2(bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh);
		bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh = 0;
		if (ret < 0)
		{
	        kal_trace(BT_TRACE_G6_OBEX,BT_BIP_ADP_PARSE_END_FSCLOSED_FAILED,ret);
			ASSERT(0);
		}
	}
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_list_startElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 *  attr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_list_startElement(void *data, const char *el, const char **attr, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!strcmp(BIP_ADP_STR_IMG_LIST, el))
    {
        ASSERT(bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer == NULL);
        bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.p_buffer = get_ctrl_buffer(BIP_ADP_MAX_COMPOSE_BUF_SIZE);
        bip_cntx_p->i_xml_parsed_result.parsed_img_list.writer.buflen = 0;

        /* open file */
        bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh = btmtk_fs_open_ucs2(BIP_ADP_IMG_LIST_FILE, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
        if ( bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh < 0)
        {
            bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh = 0;
			BT_BIP_LOG1(BT_BIP_XMLPARSE_IMGLIST_FS_OPEN_FAILED,bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh);	
        }
        return;
    }
    else if (!strcmp(BIP_ADP_STR_IMG, el))
    {
        /* save the pased image handle and image info */
        while (attr[i] != 0)
        {
            if (!strcmp(BIP_ADP_STR_HENDLE, attr[i]))
                memcpy(bip_cntx_p->i_xml_parsed_result.parsed_img_list.parsed_img_item.img_handle, attr[i+1], BT_BIP_IMG_HANDLE_LEN);
            else if (!strcmp(BIP_ADP_STR_CREATED, attr[i]))
                memcpy(bip_cntx_p->i_xml_parsed_result.parsed_img_list.parsed_img_item.created, attr[i+1], BT_BIP_MAX_TIME_LEN);                
            else if (!strcmp(BIP_ADP_STR_MODIFIED, attr[i]))
                memcpy(bip_cntx_p->i_xml_parsed_result.parsed_img_list.parsed_img_item.modified, attr[i+1], BT_BIP_MAX_TIME_LEN);
            i += 2;
        }
        return;
    }
    else
    {
        //kal_wap_trace(MOD_BT,BT_TRACE_G6_OBEX,"[BIP ADP]----ERROR-- bt_bip_parse_img_list_startElement: el=%s, err=%d", el, err);
        xml_stop_parse(bip_cntx_p->i_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_LIST_START_FAIL);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_list_endElement
 * DESCRIPTION
 *  initiator parse image list EndElement
 *  Only INITIATOR will use this func!
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_list_endElement(void *data, const char *el, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!strcmp(BIP_ADP_STR_IMG_LIST, el)) 
    {
        /* The end of this document */
        bt_adp_parsing_imglist_end();
        return;
    }
    else if (!strcmp(BIP_ADP_STR_IMG, el))
    {
        ret = bip_adp_write_parsed_imglist_item();
        if (ret < 0)
        {
            btmtk_fs_close_ucs2(bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh);
            bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh = 0;
            xml_stop_parse(bip_cntx_p->i_xml_parser_p);
            BT_BIP_LOG1(BT_BIP_PARSE_XML_IMGLIST_FS_WRITE_FAILED, ret);
            //Report(("[BIP ADP]- bt_bip_parse_img_list_endElement: btmtk_fs_write return %d, fh=%d", ret,bip_cntx_p->i_xml_parsed_result.parsed_img_list.fh));
            return;
        }
        if (bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_num_interval >= BIP_ADP_XMLPARSE_NUM_THRESHOLD)
        {
            //Report(("[BIP ADP]---bt_bip_parse_img_list_endElement: xml_pause_parse: wrote_img_num_interval=%d", 
            //        bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_num_interval));
            /* pause parseing */ 
            xml_pause_parse(bip_cntx_p->i_xml_parser_p);
            
            bip_cntx_p->i_xml_parsed_result.parsed_img_list.wrote_img_num_interval = 0;
            bip_cntx_p->i_parse_paused = TRUE;

            /* send a msg to itself */
            bt_bip_adp_send_continue_xmlparse_msg_to_self();
        }
    }
    else
    {
        //kal_wap_trace(MOD_BT,BT_TRACE_G6_OBEX,"[BIP ADP]----ERROR-- bt_bip_parse_img_list_endElement: el=%s, err=%d", el, err);
        xml_stop_parse(bip_cntx_p->i_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_LIST_END_FAIL);
    }
}

static U32 bip_get_trans_val(const char* s)
{
    U32 i ;

    for (i = 1; i < BT_BIP_TRANS_MAX; i++)
    {
        if (!strcmp(s, bip_img_trans[i]))
            return i;
    }
    return 0;
            
}
/*
  * W*H /W1*H1-W2*H2 / W1**-W2*H2
  */

enum
{
    SEEK_VALUE,
    SET_VALUE,
    SEEK_STAR,
    SEEK_SUB
};
static void bip_parse_pixel(const U8* s, bt_bip_pixel_struct* pixel)
{
    const U8* p = s;
    U16* v = &pixel->width;
    U32 state = 0;

    memset(pixel, 0, sizeof(bt_bip_pixel_struct));    

    while(*p && v <= &pixel->height2)
    {
        switch(state)
        {
        case SEEK_VALUE:
            while(*p && (*p <  '0' || *p >  '9'))
            {
                if (*p == '*')
                    v++;
                p++;
            }
            state = SET_VALUE;
            break;
            
        case SET_VALUE:
            *v = atoi((const char*)p);            
            v++;
            if ((v - &pixel->width) & 1)
                state = SEEK_STAR;
            else
                state = SEEK_SUB;
            break;
            
        case SEEK_STAR:
            while(*p && *p != '*')
            {
                p++;
            }
            if (*p == '*')
            {
                p++;
                while(*p && *p == ' ')
                    p++;
                if (*p == '*')
                {
                    if (v != &pixel->height)
                        return; /* error case */
                    else
                    {
                        p++;
                        v++;
                        state = SEEK_SUB;
                        break;
                    }
                }
            }
            state = SEEK_VALUE;
            break;

        case SEEK_SUB:
            while(*p && *p != '-')
            {
                if (*p == '*')
                    v++;
                p++;
            }
            state = SEEK_VALUE;
            break;
        }
    }  
}
/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_cap_startElement
 * DESCRIPTION
 *  Only INITIATOR will use this func
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 *  attr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_cap_startElement(void *data, const char *el, const char **attr, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;
    static int j = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!strcmp(BIP_ADP_STR_IMG_CAP, el))
    {
        /* do nothing, just reset the value of j */
        j = 0;
        return;
    }
    else if (!strcmp(BIP_ADP_STR_ATTACH_FORMAT, el))
    {
        /* do nothing */
        return;
    }
    else if (!strcmp(BIP_ADP_STR_PREFER_FORMAT, el))
    {
        while (attr[i] != 0)
        {
            if (!strcmp(BIP_ADP_STR_ENC, attr[i]))
            {
                if (!strncmp(BIP_ADP_STR_JPEG, attr[i+1], 4))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_format = BT_BIP_IMG_TYPE_JPEG;
                }
                else if (!strncmp(BIP_ADP_STR_BMP, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_format = BT_BIP_IMG_TYPE_BMP;
                }
                else if (!strncmp(BIP_ADP_STR_GIF, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_format = BT_BIP_IMG_TYPE_GIF;
                }
                else if (!strncmp(BIP_ADP_STR_PNG, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_format = BT_BIP_IMG_TYPE_PNG;
                }
                else if (!strncmp(BIP_ADP_STR_WBMP, attr[i+1], 4))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_format = BT_BIP_IMG_TYPE_WBMP;
                }
            }
            else if(!strcmp(BIP_ADP_STR_PIXEL, attr[i]))
            {
                bip_parse_pixel((const U8*)attr[i + 1], &bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_pixel);
            }
            else if (!strcmp(BIP_ADP_STR_TRANSFORMATION, attr[i]))
            {
                bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_trans = bip_get_trans_val(attr[i + 1]);
            }
            else if (!strcmp(BIP_ADP_STR_MAXSIZE, attr[i]))
            {
                bip_cntx_p->i_xml_parsed_result.parsed_img_cap.preferred_maxsize = atoi(attr[i + 1]);
            }
            i += 2;
        }
        	
        /* save the pased image handle and image info */
        return;
    }
    else if (!strcmp(BIP_ADP_STR_IMG_FORMAT, el))
    {
        while (attr[i] != 0 && j < BT_BIP_MAX_IMG_FORMATS)
        {
            if (!strcmp(BIP_ADP_STR_ENC, attr[i]))
            {
                if (!strncmp(BIP_ADP_STR_JPEG, attr[i+1], 4))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_formats[j] = BT_BIP_IMG_TYPE_JPEG;
                }
                else if (!strncmp(BIP_ADP_STR_BMP, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_formats[j] = BT_BIP_IMG_TYPE_BMP;
                }
                else if (!strncmp(BIP_ADP_STR_GIF, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_formats[j] = BT_BIP_IMG_TYPE_GIF;
                }
                else if (!strncmp(BIP_ADP_STR_PNG, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_formats[j] = BT_BIP_IMG_TYPE_PNG;
                }
                else if (!strncmp(BIP_ADP_STR_WBMP, attr[i+1], 4))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_formats[j] = BT_BIP_IMG_TYPE_WBMP;
                }
            }    
            else if (!strcmp(BIP_ADP_STR_PIXEL, attr[i]))
            {
                bip_parse_pixel((const U8*)attr[i + 1], &bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_img_size[j]);
            }
            else if (!strcmp(BIP_ADP_STR_MAXSIZE, attr[i]))
            {
                bip_cntx_p->i_xml_parsed_result.parsed_img_cap.supported_maxsize[j] = atoi(attr[i + 1]);
            }
            i += 2;
        }
        j = j + 1;
        /* save the pased image handle and image info */
        return;
    }
    else if (!strcmp(BIP_ADP_STR_FILTER_PARA, el))
    {
        while (attr[i] != 0)
        {
            if (!strcmp(BIP_ADP_STR_CREATED, attr[i]))
            {
                if (attr[i+1][0] == '1')
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.created_time_filter = KAL_TRUE;
                }
                else
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.created_time_filter = KAL_FALSE;
                }
            }     
            else if (!strcmp(BIP_ADP_STR_MODIFIED, attr[i]))
            {
                if (attr[i+1][0] == '1')
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.modified_time_filter = KAL_TRUE;
                }
                else
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.modified_time_filter = KAL_FALSE;
                }
            }
            else if (!strcmp(BIP_ADP_STR_ENC, attr[i]))
            {
                if (attr[i+1][0] == '1')
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.encoding_filter = KAL_TRUE;
                }
                else
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.encoding_filter = KAL_FALSE;
                }
            }
            else if (!strcmp(BIP_ADP_STR_PIXEL, attr[i]))
            {
                if (attr[i+1][0] == '1')
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.pixel_filter= KAL_TRUE;
                }
                else
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_cap.pixel_filter = KAL_FALSE;
                }
            }
            i += 2;
        }
        	
        /* save the pased image handle and image info */
        return;
    }
    else
    {
		/* only INITIATOR will use this func */
        xml_stop_parse(bip_cntx_p->i_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_CAP_START_FAIL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_cap_endElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_cap_endElement(void *data, const char *el, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((!strcmp(BIP_ADP_STR_IMG_CAP, el)) ||
        (!strcmp(BIP_ADP_STR_IMG_FORMAT, el)) ||
        (!strcmp(BIP_ADP_STR_PREFER_FORMAT, el)) ||
        (!strcmp(BIP_ADP_STR_ATTACH_FORMAT, el)) ||
        (!strcmp(BIP_ADP_STR_FILTER_PARA, el))
        )
    {
        /* The end of this document */
        return;
    }
    else
    {
		/* only INITIATOR will use this func */
        xml_stop_parse(bip_cntx_p->i_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_CAP_END_FAIL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_prop_startElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 *  attr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_prop_startElement(void *data, const char *el, const char **attr, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	int i = 0, pos, len;
	kal_char index_string[10];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((!strcmp(BIP_ADP_STR_IMG_PROP, el)) ||
        (!strcmp(BIP_ADP_STR_VARIANT, el)) || 
        (!strcmp(BIP_ADP_STR_ATTACHMENT, el)))
    {
        /* do nothing */
        return;
    }
    else if (!strcmp(BIP_ADP_STR_NATIVE, el))
    {
        /* save the pased image handle and image info */
        while (attr[i] != 0)
        {
            if (!strcmp(BIP_ADP_STR_ENC, attr[i]))
            {
                if (!strncmp(BIP_ADP_STR_JPEG, attr[i+1], 4))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.encoding = BT_BIP_IMG_TYPE_JPEG;
                }
                else if (!strncmp(BIP_ADP_STR_BMP, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.encoding = BT_BIP_IMG_TYPE_BMP;
                }
                else if (!strncmp(BIP_ADP_STR_GIF, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.encoding = BT_BIP_IMG_TYPE_GIF;
                }
                else if (!strncmp(BIP_ADP_STR_PNG, attr[i+1], 3))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.encoding = BT_BIP_IMG_TYPE_PNG;
                }
                else if (!strncmp(BIP_ADP_STR_WBMP, attr[i+1], 4))
                {
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.encoding = BT_BIP_IMG_TYPE_WBMP;
                }
            }  
            else if (!strcmp(BIP_ADP_STR_PIXEL, attr[i]))
            {
                pos = (int)strchr(attr[i+1], '*');
                if(pos)
                {
                    len = pos - (int)attr[i+1];
                    ASSERT(len < 10);
                    memcpy(index_string, attr[i+1], len);
                    index_string[len] = 0;
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.specified_pixel_width = atoi((const char*) index_string);
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.specified_pixel_height = atoi((const char*) (pos+1));
                }
            } 
            else if (!strcmp(BIP_ADP_STR_SIZE, attr[i]))
            {
                bip_cntx_p->i_xml_parsed_result.parsed_img_prop.size = atoi((const char*) attr[i+1]);
            } 
            else if (!strcmp(BIP_ADP_STR_FRIENDLY_NAME, attr[i]))
            {
            	kal_wchar* tmp_wchar_p = NULL;
            	int wchar_len, accept_len;

                tmp_wchar_p = bip_adp_utf8_to_ucs2((kal_uint8*) attr[i+1]);
                wchar_len = btmtk_wstrlen(tmp_wchar_p);
                accept_len = BT_BIP_MAX_IMG_NAME_LEN/2 - 1;

                if (wchar_len > accept_len)
                {
                    btmtk_wstrcpy((kal_uint16*) bip_cntx_p->i_xml_parsed_result.parsed_img_prop.friendly_name, L"~");
                    memcpy(
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.friendly_name + 2,
                    (kal_uint8*) tmp_wchar_p + (btmtk_wstrlen(tmp_wchar_p) * 2) - (accept_len*2 - 2),
                    accept_len*2 - 2);
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.friendly_name[BT_BIP_MAX_IMG_NAME_LEN - 2] = 0;
                    bip_cntx_p->i_xml_parsed_result.parsed_img_prop.friendly_name[BT_BIP_MAX_IMG_NAME_LEN - 1] = 0;
                }
                else
                {
                    btmtk_wstrcpy((kal_wchar*)bip_cntx_p->i_xml_parsed_result.parsed_img_prop.friendly_name, tmp_wchar_p);
                }
                free_ctrl_buffer(tmp_wchar_p);
            }
            i += 2;
        }
        return;
    }
    else
    {
		/* only INITIATOR will use this func */
        xml_stop_parse(bip_cntx_p->i_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_PROP_START_FAIL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_prop_endElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_prop_endElement(void *data, const char *el, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((!strcmp(BIP_ADP_STR_IMG_PROP, el)) ||
        (!strcmp(BIP_ADP_STR_NATIVE, el)) ||
        (!strcmp(BIP_ADP_STR_VARIANT, el)) ||
        (!strcmp(BIP_ADP_STR_ATTACHMENT, el)))
    {
        /* The end of this document */
        return;
    }
    else
    {
		/* only INITIATOR will use this func */
        xml_stop_parse(bip_cntx_p->i_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_PROP_END_FAIL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_descriptor_startElement
 * DESCRIPTION
 *  Only RESPONDER will use this func
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 *  attr        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_descriptor_startElement(void *data, const char *el, const char **attr, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	int i = 0;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!strcmp(BIP_ADP_STR_IMG_DESC, el))
    {
        /* do nothing */
        return;
    }
    else if (!strcmp(BIP_ADP_STR_IMG, el))
    {
        /* save the pased image handle and image info */
        while (attr[i] != 0)
        {
            if (!strcmp(BIP_ADP_STR_ENC, attr[i]))
            {
                if (!strncmp(BIP_ADP_STR_JPEG, attr[i+1], 4))
                {
                    bip_cntx_p->r_xml_parsed_result.parsed_img_desc.encoding = BT_BIP_IMG_TYPE_JPEG;
                }
                else if (!strncmp(BIP_ADP_STR_BMP, attr[i+1], 3))
                {
                    bip_cntx_p->r_xml_parsed_result.parsed_img_desc.encoding = BT_BIP_IMG_TYPE_BMP;
                }
                else if (!strncmp(BIP_ADP_STR_GIF, attr[i+1], 3))
                {
                    bip_cntx_p->r_xml_parsed_result.parsed_img_desc.encoding = BT_BIP_IMG_TYPE_GIF;
                }
                else if (!strncmp(BIP_ADP_STR_PNG, attr[i+1], 3))
                {
                    bip_cntx_p->r_xml_parsed_result.parsed_img_desc.encoding  = BT_BIP_IMG_TYPE_PNG;
                }
                else if (!strncmp(BIP_ADP_STR_WBMP, attr[i+1], 4))
                {
                    bip_cntx_p->r_xml_parsed_result.parsed_img_desc.encoding  = BT_BIP_IMG_TYPE_WBMP;
                }
            }  
            else if (!strcmp(BIP_ADP_STR_PIXEL, attr[i]))
            {
                bip_parse_pixel((const U8*)attr[i + 1], &bip_cntx_p->r_xml_parsed_result.parsed_img_desc.pixel);
                /*
                pos = (int)strchr(attr[i+1], '*');
                if(pos)
                {
                    len = pos - (int)attr[i+1];
                    ASSERT(len < 10);
                    memcpy(index_string, attr[i+1], len);
                    index_string[len] = 0;
                    bip_cntx_p->r_xml_parsed_result.parsed_img_prop.specified_pixel_width = atoi((const char*) index_string);
                    bip_cntx_p->r_xml_parsed_result.parsed_img_prop.specified_pixel_height = atoi((const char*) (pos+1));
                }
                */
            } 
            /* size and maxsize should be exclusive! */
            else if (!strcmp(BIP_ADP_STR_SIZE, attr[i]) ||!strcmp(BIP_ADP_STR_MAXSIZE, attr[i]))
            {
                bip_cntx_p->r_xml_parsed_result.parsed_img_desc.size = atoi((const char*) attr[i+1]);
            }
            else if (!strcmp(BIP_ADP_STR_TRANSFORMATION, attr[i]))
            {
                bip_cntx_p->r_xml_parsed_result.parsed_img_desc.transformation = bip_get_trans_val(attr[i + 1]);
            }
            i += 2;
        }
        return;
    }
    else
    {
		/* only RESPONDER will use this func */
        xml_stop_parse(bip_cntx_p->r_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_PROP_START_FAIL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_bip_parse_img_descriptor_endElement
 * DESCRIPTION
 *  
 * PARAMETERS
 *  data        [?]         
 *  el          [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_bip_parse_img_descriptor_endElement(void *data, const char *el, kal_int32 err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((!strcmp(BIP_ADP_STR_IMG_DESC, el)) ||
        (!strcmp(BIP_ADP_STR_IMG, el)))
    {
        /* The end of this document */
        return;
    }
    else
    {
		/* only RESPONDER will use this func */
        xml_stop_parse(bip_cntx_p->r_xml_parser_p);
        BT_BIP_FUNC_ENTRY(BT_BIP_FUNC_PARSE_XML_IMG_PROP_END_FAIL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_adp_create_folder
 * DESCRIPTION
 *  
 * PARAMETERS
 *  filepath      [IN] file path with path and filename       
 * RETURNS
 *  fs result
 *****************************************************************************/
static int bt_adp_create_folder(WCHAR* filepath)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint16 pathlen = (kal_uint16) btmtk_wstrlen(filepath);
//mtk03036
#ifdef BTMTK_ON_LINUX 
    WCHAR deter = '/';
#else
    WCHAR deter = '\\';
#endif
//mtk03036 end
	WCHAR tmpchar = 0;
	int ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	/* find the last '\' */
    while (pathlen > 0)
    {
        pathlen--;
        if (memcmp(filepath + pathlen, &deter, 2) == 0)
        {
            break;
        }
    }

	/* truncate the filepath to get only folder string */
	if (pathlen > 0)
	{
	    tmpchar = filepath[pathlen + 1];
	    filepath[pathlen + 1] = '\0'; 
	}

    ret = btmtk_fs_create_dir_ucs2(filepath);
    BT_BIP_LOG1(BT_BIP_ADP_CREATE_DIR_RET, ret);

	/* restore the filepath with filename*/
	if (pathlen > 0)
	{
  	    filepath[pathlen + 1] = tmpchar; 
	}

	return ret;
}

void bip_adp_close_file_continually(void *p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret = BTMTK_FS_OK;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CLOSE_FILE_CONTINUALLY, bip_cntx_p->r_fshandle_to_close);	

    if (bip_cntx_p->r_fshandle_to_close >= 0)
    {
	    ret = btmtk_fs_close_ucs2(bip_cntx_p->r_fshandle_to_close);
		if (ret >= BTMTK_FS_OK)
		{
		    bip_cntx_p->r_fshandle_to_close = -1;
		}
    }

	if (ret >= BTMTK_FS_OK)
	{
        /* reset timer id to be zero */
		bip_cntx_p->r_fs_cont_timer_id = 0;
	}
	else if (BIP_ADP_FS_DEVICE_BUSY(ret))
	{
	    /* start timer again */
	    bip_adp_start_timer(&(bip_cntx_p->r_fs_cont_timer_id), 
		                 BIP_ADP_FS_CONT_DUR, 
		                 bip_adp_close_file_continually);
	}
}

static void bip_adp_start_close_file_continually(int file_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_START_CLOSE_FILE_CONTINUALLY, bip_cntx_p->r_fshandle_to_close);	

	bip_cntx_p->r_fshandle_to_close = file_handle;

	bip_adp_start_timer(&(bip_cntx_p->r_fs_cont_timer_id), 
		                 BIP_ADP_FS_CONT_DUR, 
		                 bip_adp_close_file_continually);
}
	
kal_uint32 bip_adp_time_msec_2_tick(kal_uint32 msec)
{
   kal_uint32 num_ticks; 

   num_ticks = (msec*1000)/4615;

   while(num_ticks*4615 < (msec*1000))
      num_ticks++;

   return num_ticks;
}
void bip_adp_start_timer(eventid *timer_id_p, kal_uint32 timer_duration, kal_timer_func_ptr funct_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    // do nothing now....
#if 0 
    eventid timer_evt_id = *timer_id_p;
    if(bt_p->os_init == TRUE)
    {
        if (timer_evt_id != 0)
        {
            evshed_cancel_event (bt_p->evsh_sched, timer_id_p);
            *timer_id_p = 0;
        }

        *timer_id_p = evshed_set_event (bt_p->evsh_sched,
    		                            funct_ptr, NULL, 
    		                            bip_adp_time_msec_2_tick(timer_duration));
    }
#endif
}

#ifdef BIP_CHECK_DRM_RIGHTS

#include "drm_gprot.h"

kal_bool bt_bip_adp_check_DRM_rights(kal_uint16 *file_path)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_bool ret = KAL_TRUE;
#ifdef __DRM_SUPPORT__
    kal_int8 url[DRM_MAX_URL_LENGTH];
    BTMTK_FS_HANDLE handle;
    kal_int32 size = DRM_MAX_URL_LENGTH;
    kal_uint8 method;
    kal_int32 drm_get_rst;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    handle = DRM_open_file((kal_wchar*)file_path, BTMTK_FS_READ_ONLY, DRM_PERMISSION_NONE);
    BT_BIP_LOG1(BT_BIP_ADP_DRM_OPEN_RET, handle);

    if (handle < DRM_RESULT_OK)
    {
        return ret; /* not a valid file */
    }

    /* get DRM method */
    method = DRM_get_object_method(handle, NULL);
    BT_BIP_LOG1(BT_BIP_ADP_DRM_GET_OBJ_METHOD_RET, method);
    
    if((method & DRM_METHOD_SEPARATE_DELIVERY ) || 
        method == DRM_METHOD_V2 || 
        method == DRM_METHOD_PDCF)
    {
        drm_get_rst = DRM_get_rights_issuer(handle, (kal_char*)url, (kal_uint32 *)&size);
        BT_BIP_LOG2(BT_BIP_ADP_DRM_GET_RIGHTS_ISSUER_RET, drm_get_rst, size);

        if(drm_get_rst < DRM_RESULT_OK)
        {
            size = 0;
        }
    }

    DRM_close_file(handle);

    /* if no DRM rights info, delete the file */    
    if (size == 0)   
    {
        int rst = btmtk_fs_delete_ucs2((kal_wchar*)file_path);
        ret = KAL_FALSE;
        BT_BIP_LOG1(BT_BIP_ADP_FS_DELETE_RET, rst);
    }
#endif /* __DRM_SUPPORT__ */

    BT_BIP_LOG1(BT_BIP_ADP_CHECK_DRM_RET, ret);
    return ret;
}
#endif /* BIP_CHECK_DRM_RIGHTS */

void bip_adp_send_challenge_ind(kal_uint32 cm_id, ObexAuthChallengeInfo* chal, kal_uint16 msg)
{
    bt_bip_obex_auth_ind_struct* ind = (bt_bip_obex_auth_ind_struct*) construct_local_para(sizeof(bt_bip_obex_auth_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ind->cm_conn_id = cm_id;
    ind->option = chal->options;    
    ind->realm_len= chal->realmLen;
    if (ind->realm_len)
        memcpy(ind->realm, chal->realm, ind->realm_len);    
    bip_adp_send_msg(msg, ind);
}

void bip_adp_send_challenge_cnf(kal_uint32 cm_id, kal_uint8 result, kal_uint16 msg)
{
    bt_bip_obex_auth_cnf_struct* cnf = 
        (bt_bip_obex_auth_cnf_struct*)construct_local_para(sizeof(bt_bip_obex_auth_cnf_struct), TD_RESET);

    cnf->cm_conn_id = cm_id;
    cnf->cnf_code = result;
    bip_adp_send_msg(msg, cnf);
}
void bipr_adp_obex_auth_rsp(bt_bip_obex_auth_rsp_struct* rsp)
{
    if (rsp->result != BT_BIP_CNF_SUCCESS)
    {
        BIP_Abort(&(bip_cntx_p->BipResponder), BIPCH_RESPONDER_PRIMARY, GOEP_UNAUTHORIZED);
    }
    else
    {
        memcpy(&bip_cntx_p->r_dig_rsp, &rsp->dig_rsp, sizeof(bt_bip_dig_response_struct));        
            
        bip_cntx_p->r_obex_rsp.password = bip_cntx_p->r_dig_rsp.pwd;
        bip_cntx_p->r_obex_rsp.passwordLen = bip_cntx_p->r_dig_rsp.pwd_len;
        bip_cntx_p->r_obex_rsp.userId = bip_cntx_p->r_dig_rsp.uid;
        bip_cntx_p->r_obex_rsp.userIdLen = bip_cntx_p->r_dig_rsp.uid_len;
        BIP_ServerAuthenticate(&bip_cntx_p->BipResponder.primary, &bip_cntx_p->r_obex_rsp, NULL);
    }
    
    BIP_ResponderContinue(&(bip_cntx_p->BipResponder.primary)); 
}

void bipr_adp_obex_auth_req(bt_bip_obex_auth_req_struct* req)
{
    if (bip_cntx_p->rState != BT_BIP_R_STATE_ACCEPTING &&
        bip_cntx_p->rState != BT_BIP_R_STATE_INDICATING)
    {
        /* add trace */
        BT_BIP_R_STATE(bip_cntx_p->rState);
        return;
    }
    
    memcpy(&bip_cntx_p->r_dig_chal, &req->dig_chal, sizeof(bt_bip_dig_challenge_struct));

    if (req->dig_chal.uid_len)
    {
        bip_cntx_p->r_obex_chal.options = AUTH_OPT_SEND_USERID;
    }
    else
    {
        bip_cntx_p->r_obex_chal.options = AUTH_OPT_DEFAULT;
    }
    bip_build_challenge_nonce(bip_cntx_p->r_nonce, 16);
    bip_cntx_p->r_obex_chal.challenge = bip_cntx_p->r_nonce;
    bip_cntx_p->r_obex_chal.challengeLen = 16;
    BIP_ServerAuthenticate(&bip_cntx_p->BipResponder.primary, NULL, &bip_cntx_p->r_obex_chal);
    BIP_ResponderContinue(&(bip_cntx_p->BipResponder.primary)); 

    bip_cntx_p->r_chal_status = BIP_ADP_CHAL_STATUS_REQUEST;
}

void bipi_adp_obex_auth_rsp(bt_bip_obex_auth_rsp_struct* rsp)
{
    BtStatus status;
    ObexTpAddr addr;
    ObexAuthChallenge* pchallenge = NULL;

    if (rsp->result == BT_BIP_CNF_SUCCESS && rsp->dig_rsp.pwd_len)
    {
        /* response challenge */
        if (bip_cntx_p->iState != BT_BIP_I_STATE_AUTHENTICATING)
        {
            /* add trace */
            BT_BIP_R_STATE(bip_cntx_p->rState);
            return;
        }
        /* now we only support CONNECT */
        if (bip_cntx_p->i_obex_chal.challengeLen)
            pchallenge = &bip_cntx_p->i_obex_chal;
        
        memcpy(&bip_cntx_p->i_dig_rsp, &rsp->dig_rsp, sizeof(bt_bip_dig_response_struct));
        bip_cntx_p->i_obex_rsp.password = bip_cntx_p->i_dig_rsp.pwd;
        bip_cntx_p->i_obex_rsp.passwordLen = bip_cntx_p->i_dig_rsp.pwd_len;
        bip_cntx_p->i_obex_rsp.userId = bip_cntx_p->i_dig_rsp.uid;
        bip_cntx_p->i_obex_rsp.userIdLen = bip_cntx_p->i_dig_rsp.uid_len;
        BIP_ClientAuthenticate(&bip_cntx_p->BipInitiator.primary, &bip_cntx_p->i_obex_rsp, pchallenge);
        
        CONVERT_BDADDR2ARRAY((U8*)&(addr.proto.bt.addr), bip_cntx_p->i_bd_addr.lap, bip_cntx_p->i_bd_addr.uap, bip_cntx_p->i_bd_addr.nap);
        addr.type = OBEX_TP_BLUETOOTH;

        status = BIP_Connect(&(bip_cntx_p->BipInitiator), &addr, bip_cntx_p->iService);
        
        if (status == OB_STATUS_SUCCESS)
        {
            bip_cntx_p->i_cm_conn_id = (U32) BTBMProfileAllocateConnIndex(SC_IMAGING, addr.proto.bt.addr);
            
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTED);
            bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_SUCCESS);
        }
        else if (status != OB_STATUS_PENDING)
        {         
            BT_BIP_FUNC_ENTRY1(BT_BIP_FUNC_CONN_REQ_HDLR_CONN_FAILED, status);
            
            /* send connect failed confirm to MMI */
            bip_cntx_p->i_cm_conn_id = 0xFFFFFFFF;
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_IDLE);
            bip_adp_send_connect_cnf(bip_cntx_p->i_req_id, bip_cntx_p->i_cm_conn_id, BT_BIP_CNF_FAILED);
            
            /* deregister initiator */
            BIP_DeregisterInitiator(&(bip_cntx_p->BipInitiator));
        }
        else
        {
            BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_CONNECTING);
        }        
    }
    else
    {
        /* disconnect, since nothing can be done now */
        BIP_ADP_I_STATE_TRANS(bip_cntx_p->iState, BT_BIP_I_STATE_DISCONNECTING);
        status = BIP_TpDisconnect(&(bip_cntx_p->BipInitiator));
    }
}

#endif /* __BT_BIP_PROFILE__ */

