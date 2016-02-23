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
 * goep_profile.c
 *
 * Project:
 * --------
 *   SUPERMAN6229
 *
 * Description:
 * ------------
 *   This is main() function of GOEP module.
 *   This file is intends for processing inter-layer messages
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
 * Integration Bluetooth solution.
 *
 * Feb 13 2009 mtk80029
 * [MAUI_01393811] [BT GOEP] Sync CR MAUI_01481941 to MP branch
 *
 *
 * Dec 19 2008 mbj06032
 * [MAUI_01481941] MTK:Bluetooth_it the FTP can't disconenct
 * modify send_disconnect_ind func: use the parameter first
 *
 * Nov 26 2008 mbj06038
 * [MAUI_01342148] [New feature][SyncML] Device sync support
 *
 *
 * Oct 6 2008 mbj06032
 * [MAUI_01207978] MTK:Bluetooth_GOEP_ASSERT fail btbmProfilesCtrl.c 117 -BT
 * goep_send_connect_ind_msg func check if Service->cm_conn_id has valid value before alloc a value.
 *
 * Sep 2 2008 mbj06032
 * [MAUI_01106205] [BT GOEP] Send OPP Client SDP result (OPP server's supported format list) to MMI
 *
 *
 * Aug 20 2008 mbj06032
 * [MAUI_01196499] MTK:Bluetooth OPP_[1] Fatal Error (431, 5c) - BT
 * call allocate_ilm just before use it
 *
 * Aug 15 2008 mbj06032
 * [MAUI_01101632] [BT BIP] revise thumbnail request code of BIP PUSH server
 * add trace
 *
 * Jul 11 2008 mbj06032
 * [MAUI_01171562] MTK:Bluetooth OBEX_it can't connect FTP succeed
 *
 *
 * Jul 9 2008 mbj06032
 * [MAUI_01171562] MTK:Bluetooth OBEX_it can't connect FTP succeed
 *
 *
 * Jul 2 2008 mbj06032
 * [MAUI_01171562] MTK:Bluetooth OBEX_it can't connect FTP succeed
 *
 *
 * Mar 13 2008 mbj06032
 * [MAUI_00281507] MTK OPP: it is abanormal when receive a OPP file
 * clear server.pullpush.name info only after using it
 *
 * Mar 10 2008 mbj06032
 * [MAUI_00630752] MTK BT profile supported on MT6611 download file from PC by FTP speed abnormal
 * assign the correct value to the "type" member
 *
 * Nov 21 2007 mbj06032
 * [MAUI_00241054] MTK:Bluetooth OPP_[1] Fatal Error (80a): Buffer foot print is corrupted (e6680) - MM
 *
 *
 * Oct 26 2007 mbj06032
 * [MAUI_00239564] MTK:Bluetooth BIP_[1] Fatal Error (809): kal buffer pool ID is invalid (60f1b8) - BT
 * goep_deinit need only deregister FTP & OPP server/client
 *
 * Sep 28 2007 mbj06032
 * [MAUI_00470068] [BT][GOEP] when GET/PUT failed, tell application the response code from server
 * tell the application the detail failed rsp code from server
 *
 * Sep 24 2007 mbj06032
 * [MAUI_00473476] [BT GOEP] modify goep_find_xxx_with_goep_conn_id func
 * in "find xxx" func, check "j < GOEP_MAX_PROFILES" in the loop
 *
 * Sep 24 2007 mbj06032
 * [MAUI_00238399] MTK:Bluetooth OPP_[1] Fatal Error (4): Data Abort (50a0) - BT
 * in goep_disconnect_req_hdlr func, when call goep_send_disconnect_ind_msg, check if target is NULL
 *
 * Sep 14 2007 mbj06032
 * [MAUI_00469575] [BT] [BIP] ( For MBJ ) Check in
 * BIP MMI feature check in
 *
 * Sep 6 2007 mbj06032
 * [MAUI_00462785] [BT]Screen always display '0%'
 * goep_pull_rsp_hdlr process GOEP_SINGLE_PKT as GOEP_FIRST_PKT
 *
 * Aug 27 2007 mbj06032
 * [MAUI_00461457] replace "Report" with "kal_trace" in GOEP & OBEX
 * replace "Report" with "kal_trace"
 *
 * Jun 14 2007 mbj06032
 * [MAUI_00403526] BT_HFP profile is disappear in Active Device but HFP profile still connect with BT d
 * To avoid release service's un-allocated cm_conn_id, initialize service->cm_conn_id to INVALID value, and when release the service->cm_conn_id in goep_send_disconnect_ind_msg, set its value to INVALID too.
 *
 * Jun 1 2007 mbj06032
 * [MAUI_00434949] [MTK_BT] [1]Assert fail:0 btmmiopp.c 2699-MMI
 * don't set connState to CS_DISCONNECTING if goep_conn_id is valid
 *
 * Jun 1 2007 mbj06032
 * [MAUI_00434976] FTP Server doesn't send the correct "length" header
 * file-base's modification: build "length" header  with server->currOp.info.pushpull.objectLen. so assign value to server->currOp.info.pushpull.objectLen in goep_pull_res_hdlr
 *
 * May 29 2007 mbj06032
 * [MAUI_00397020] BT-Screen always in Aborting
 * If RF_CloseChannel return fail, then send disconnect_ind msg
 *
 * May 28 2007 mbj06032
 * [MAUI_00397347] [1]Fatal Error(809):kal buffer pool ID is invalid(161634)-BT
 * memset Service struct to 0 when registering
 *
 * May 21 2007 mbj06032
 * [MAUI_00431668] GOEP authetication practice
 * add code for GOEP practice. Compile option is GOEP_AUTH
 *
 * May 21 2007 mbj06032
 * [MAUI_00431654] memory leak in GOEP authentication
 * Free "auth_ream" , "auth_passwd" and "Challenge " memory malloced in goep_auth_req_hdlr func
 *
 * May 19 2007 mbj06032
 * [MAUI_00393865] BT_HFP profile is disappear in Active Device but HFP profile still connect with BT d
 * before calling BTBMProfileReleaseConnIndex, check  cm_conn_id 's validity
 *
 * May 18 2007 mbj06032
 * [MAUI_00393864] [1] Assert fail: IsListCircular(head) utils.c 250 - BT
 * When GOEP_Disconnect and GOEP_TpDisconnect fails, call RF_CloseChannel directly to disconnect the transport connection
 *
 * May 11 2007 mbj06032
 * [MAUI_00427650] Need more GOEP and OBEX trace info
 * add some traces
 *
 * May 9 2007 mbj06032
 * [MAUI_00232824] [1] Assert fail: AppHndl obstack.c 1334 - BT
 * modify goep_abort_req_hdlr func for CR maui_00232824
 *
 * Apr 20 2007 mtk00560
 * [MAUI_00384677] [MMI][BT] to check-in for CR423159
 *
 *
 * Apr 16 2007 MTK00511
 * [MAUI_00381908] [New Feature]Bluetooth Basic Printing Profile check in
 *
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Dec 17 2006 mtk00511
 * [MAUI_00227613] Bluetooth_Some files icon displays unknown file icon in BT File Browser.
 *
 *
 * Nov 17 2006 mtk00511
 * [MAUI_00343978] [1] Assert fail: 0 BTMMIFtp.c 4545 - MMI
 *
 *
 * Nov 13 2006 mtk00511
 * [MAUI_00343002] [GOEP] Add OBEX authentication support
 *
 *
 * Oct 23 2006 mtk00511
 * [MAUI_00336114] [1] Fatal Error (305): msg_send_ext_queue() failed - SYSTEM H
 *
 *
 * Oct 18 2006 mtk00511
 * [MAUI_00336475] [1] Assert fail: AppHndl obstack.c 1331 - BT
 *
 *
 * Oct 16 2006 mtk00511
 * [MAUI_00336405] [OBEX] Modification for BQB test and IOT issues
 *
 *
 * Oct 13 2006 mtk00511
 * [MAUI_00334977] [1] Assert fail: 0 goep_main.c 2091 - BT
 *
 *
 * Oct 11 2006 mtk00511
 * [MAUI_00017409] [1] Assert fail: 0 goep_main.c 2007 - BT
 *
 *
 * Oct 11 2006 mtk00511
 * [MAUI_00335038] [1] Fatal Error (807): Release Buffer Failed - BT
 *
 *
 * Oct 3 2006 mtk00511
 * [MAUI_00332583] [1]Assert fail:0 BTMMIFtp.c 1514-MMI
 *
 *
 * Oct 3 2006 mtk00511
 * [MAUI_00334315] BT_ Use PC send one vCard to handset then my device will display wrong device name
 *
 *
 * Sep 19 2006 mtk00511
 * [MAUI_00330815] BT_ FTP access confirm dialog display mess message
 *
 *
 * Sep 17 2006 mtk00511
 * [MAUI_00330296] [GOEP]Fix OPP IOT issue with Widcomm
 *
 *
 * Sep 13 2006 mtk00511
 * [MAUI_00329160] [GOEP]Check in GOEP profile and OBEX stack
 *
 *
 * Sep 13 2006 mtk00511
 * [MAUI_00329160] [GOEP]Check in GOEP profile and OBEX stack
 *
 *
 * Sep 12 2006 mtk00511
 * [MAUI_00329160] [GOEP]Check in GOEP profile and OBEX stack
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_GOEP_PROFILE__

#define _GOEP_MAIN_C_

#include <sys/obxalloc.h>
#include <sys/obex_utl.h>


#include "goep.h"
#include "goepext.h"
#include "bluetooth_struct.h"
#include "goep_main.h"
#include "obexauth.h"
#include "bt_os_api.h"
#include "bt_ucs2.h"

#include "sdpdb.h"

//#include <cutils/log.h>

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#define GOEP_INVALID_CMID  0xFFFF

/* Global context in GOEP */
kal_uint8* global_goep_connection[GOEP_CONNECTION_NUM];
kal_int8 global_goep_conn_count;

#if GOEP_AUTH == XA_ENABLED

auth_chal_struct  auth_chal_profiles[MAX_AUTHCHAL_PROFILES_NUM];
kal_bool  auth_chal_profiles_inited = KAL_FALSE;

#define FTP_PWD_STR   "1234"

kal_bool goep_need_obex_auth_challenge(kal_uint8 *uuid, kal_uint8 uuid_len, int *pIndex);

#endif

/*****************************************************************************
* External functions
*****************************************************************************/
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern U8 *BTBMGetLocalStoredName(U8 *bdAddr);
extern BOOL BTBMProfileReleaseConnIndex(U8 index);

/*-------------------------------------------------------------------------
 *
 * OBEX File Transfer UUID for Directed connections
 */
static const U8 FileXferUuid[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };
// 090820 daylong: copy from goep.c
static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };

/*-------------------------------------------------------------------------
 *
 * Dispatch to OPP or FTP profile
 */
 // #if defined (__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__) || defined(__BT_OPP_PROFILE__)
typedef struct
{
    U8 ref_count;
    U16 msg_len;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
} obex_msg_struct;
//#endif /* (__BT_FTS_PROFILE__) || (__BT_FTC_PROFILE__) || (__BT_OPP_PROFILE__) */
void goep_approfile_handler(U32 msg_id, void* data, U32 size);

#ifndef GOEP_APPROFILE_DISPATCH
#define GOEP_APPROFILE_DISPATCH(x,y,z) goep_approfile_handler(x, y, z);
#endif

//typedef void (*GOEP_EVENT_CALLBAC)(U32 msg_id, void* data, U32 u4Size);

void goep_approfile_handler(U32 msg_id, void* data, U32 size){

    obex_msg_struct *obex_msg = (obex_msg_struct *) data;

    /* OPP */
    if (obex_msg->uuid_len == 0 && obex_msg->uuid[0] == '\0')
    {
#if defined(__BT_OPP_PROFILE__)

        // make sure buffer is ready


		/// Should Move. goep_main (opp & ftp's intadp)
        bt_opp_profile_event_hdlr(msg_id, data, size);
        return;
#endif /* defined(__BT_OPP_PROFILE__) */
    }
    /* FTP */
#if defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__)
    else if ( bt_strncmp( (const char*)OBEX_FTP_UUID, (const char*)obex_msg->uuid, 16 ) == 0 )
    {
        
        OS_Report("[ERR] goep_approfile_handler no handle msg_id:%d ", msg_id);
		/// Should Move. goep_main (opp & ftp's intadp)
        bt_ftp_profile_event_hdlr(msg_id, data, size);
		return;
    }
#endif //#endif /* defined(__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__) */
    else{
        /// error msg. catch it
    }
    ASSERT(0);
}

void free_clientapp_ctrl_buffer(GoepClientApp *client){
	// free the 
	U8 idx = 0;
	ASSERT(client != NULL);
	if( client != NULL ){
#if BT_GOEP_V2 == XA_ENABLED
		if( 0 != client->l2capRawDataNum ){
			for( idx = 0; idx < client->l2capRawDataNum ; idx++ ){
				ASSERT( client->pl2capRawData[idx] );
				free_ctrl_buffer( client->pl2capRawData[idx] );
				client->pl2capRawData[idx] = NULL;
			}
			client->l2capRawDataNum = 0;
		}
#endif
		free_ctrl_buffer(client);
	}
}

void free_serverapp_ctrl_buffer(GoepServerApp *server){
	// free the 
	U8 idx = 0;
	ASSERT(server != NULL);
	if( server != NULL ){
#if BT_GOEP_V2 == XA_ENABLED		
		if( 0 != server->l2data.l2capRawDataNum ){
			for( idx = 0; idx < server->l2data.l2capRawDataNum ; idx++ ){
				ASSERT( server->l2data.pl2capRawData[idx] );
				free_ctrl_buffer( server->l2data.pl2capRawData[idx] );
				server->l2data.pl2capRawData[idx] = NULL;
			}
			server->l2data.l2capRawDataNum = 0;
		}
#endif		
		free_ctrl_buffer(server);
	}
}

#if GOEP_AUTH == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  goep_need_obex_auth_challenge
* DESCRIPTION
*  This function is to check if the uuid need OBEX authentication challenge
* PARAMETERS
*  uuid: target uuid
*  uuid_len: length of uuid
* RETURNS
*   TRUE:  the target uuid need OBEX authentication challenge
*   FALSE: need not
* GLOBALS AFFECTED
*  None
*****************************************************************************/
kal_bool goep_need_obex_auth_challenge(kal_uint8 *obex_uuid, kal_uint8 uuid_len, int *pIndex)
{
    int i;


    if (!auth_chal_profiles_inited)
    {
        btmtk_os_memset(auth_chal_profiles, 0, sizeof(auth_chal_profiles));

	    auth_chal_profiles[0].uuid_len = sizeof(FileXferUuid);
		btmtk_os_memcpy(auth_chal_profiles[0].uuid, FileXferUuid, sizeof(FileXferUuid));

		auth_chal_profiles[0].passwd_len = strlen(FTP_PWD_STR);
	    btmtk_os_memcpy(auth_chal_profiles[0].passwd, FTP_PWD_STR,
						auth_chal_profiles[0].passwd_len);

	    auth_chal_profiles_inited = KAL_TRUE;
    }

    for (i = 0; i < MAX_AUTHCHAL_PROFILES_NUM; i++)
    {
        if(auth_chal_profiles[i].uuid
			&& (auth_chal_profiles[i].uuid[0] != 0)
			&& (bt_strncmp((const char*)obex_uuid, (const char*)auth_chal_profiles[i].uuid,
					auth_chal_profiles[i].uuid_len) == 0))
        {
            *pIndex = i;
            return KAL_TRUE;
        }
    }
    return KAL_FALSE;
}

extern int rand(void);

ObexAuthChallenge* goep_build_auth_challenge_data(int index)
{
    ObexAuthChallenge* Challenge;
    U8 rand_str[16];

    if (index < 0 || index >= MAX_AUTHCHAL_PROFILES_NUM)
		return NULL;

    Challenge = get_ctrl_buffer(sizeof(ObexAuthChallenge));
    btmtk_os_memset(Challenge, 0, sizeof(ObexAuthChallenge));

    kal_sprintf((char*)rand_str, "%12d", rand());

    Challenge->challengeLen = strlen((const char*)rand_str) + 1;
    Challenge->challenge = get_ctrl_buffer(Challenge->challengeLen);
    btmtk_os_memcpy(Challenge->challenge, rand_str, Challenge->challengeLen);

    return Challenge;
}


/*****************************************************************************
* FUNCTION
*  goep_find_ServerObexCons_with_goep_conn_id
* DESCRIPTION
*  This function is to find out the ServerObexCons with goep_conn_id
* PARAMETERS
*  goep_conn_id     IN       The goep connection id of the wanted server
* RETURNS
*  The GOEP ServerObexCons with the input goep_conn_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepServerObexCons * goep_find_ServerObexCons_with_goep_conn_id(int goep_conn_id)
{
    int i;

    for(i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        if(&(GOES(servers)[i]) != NULL)
        {
            if(GOES(servers)[i].obs.goep_conn_id == goep_conn_id)
                return (GoepServerObexCons *)(&(GOES(servers)[i]));
        }
    }
    return NULL;
}

#endif //end of GOEP_AUTH

/*****************************************************************************
* FUNCTION
*  goep_find_free_goep_id
* DESCRIPTION
*  This function is to find out the free goep connection id to use
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
int goep_find_free_goep_id(int isclient, U8 *uuid, U8 uuid_len)
{
    int i = -1;
    U32 l_uuid32 = 0;

    if( 0 != uuid && uuid_len >= 4){
        l_uuid32 = BEtoHost32(uuid);
    }
    //OS_Report("goep: find_free param isclient:%d uuid_len:%d l_uuid32:0x%x", isclient, uuid_len, l_uuid32);
    
    if( 0 == isclient  ){
        // return the fixed server position or -1
        // select the fixed position i by first 4 byte
        switch(l_uuid32){
            case 0xF9EC7BC4://FTP 0xF9, 0xEC, 0x7B, 0xC4
            i = 1;
            break;
            case 0xE33D9545://BIP Push 0xE3, 0x3D, 0x95, 0x45
            i = 2;
            break;
            case 0x8Ee9b3d0://BIP Pull 0x8E, 0xE9, 0xB3, 0xD0
            i = 3;
            break;
            case 0x947E7420://BIP Camera 0x94, 0x7E, 0x74, 0x20 
            i = 4;
            break;
            case 0xBB582B40://MAP NAS 0xBB, 0x58, 0x2B, 0x40
            i = 5; // NAS has more then one. Others use the remain index
            break;
            case 0xBB582B41://MAP MNS 0xBB, 0x58, 0x2B, 0x41
            i = 6; 
            break;
            case 0: //OPP or others
            i = 7;
            break;
        }
        
        if( i >=  0 && global_goep_connection[i] == NULL ){
            OS_Report("goep: find_free isclient:%d uuid_len:%d l_uuid32:0x%x ret:%d", isclient, uuid_len, l_uuid32, i);
            return i;
        }else{
            // select from half part
           
        }
    }

    for(i = (int)(GOEP_CONNECTION_NUM/2); i < GOEP_CONNECTION_NUM; i++)
    {
        if(global_goep_connection[i] == NULL){
            OS_Report("goep: find_free isclient:%d ret2:%d", isclient,  i);            
            return i;
    }
    }
    OS_Report("goep: find_free not available i:%d", i);
    return -1; /* Can not find out free goep connection id */
}

/*****************************************************************************
* FUNCTION
*  goep_find_server_with_goep_conn_id
* DESCRIPTION
*  This function is to find out the server with goep_conn_id
* PARAMETERS
*  goep_conn_id     IN       The goep connection id of the wanted server
* RETURNS
*  The GOEP server context with the input goep_conn_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepServerApp * goep_find_server_with_goep_conn_id(int goep_conn_id)
{
    int i;
    int j = 0;

    for(i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        if(&(GOES(servers)[i]) != NULL)
        {
            if(GOES(servers)[i].obs.goep_conn_id == goep_conn_id)
            {
                while (j < GOEP_MAX_PROFILES && GOES(servers)[i].profiles[j] == NULL)
                    j++;

                if (j < GOEP_MAX_PROFILES)
                    return GOES(servers)[i].profiles[j];
                else
                    return NULL;
            }
        }
    }
    return NULL;
}

/*****************************************************************************
* FUNCTION
*  goep_find_client_with_goep_conn_id
* DESCRIPTION
*  This function is to find out the client with goep_conn_id
* PARAMETERS
*  goep_conn_id     IN       The goep connection id of the wanted client
* RETURNS
*  The GOEP server context with the input goep_conn_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepClientApp * goep_find_client_with_goep_conn_id(int goep_conn_id)
{
    int i;
    int j = 0;

    for(i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++)
    {
        if(&(GOEC(clients)[i]) != NULL)
        {
            if(GOEC(clients)[i].obc.goep_conn_id == goep_conn_id)
            {
                while (j < GOEP_MAX_PROFILES && GOEC(clients)[i].profiles[j] == NULL)
                    j++;

                if (j < GOEP_MAX_PROFILES)
                    return GOEC(clients)[i].profiles[j];
                else
                    return NULL;
            }
        }
    }
    return NULL;
}

/*****************************************************************************
* FUNCTION
*  goep_find_client_with_req_id
* DESCRIPTION
*  This function is to find out the client with req_id
* PARAMETERS
*  req_id     IN       The id of the connect request
* RETURNS
*  The GOEP server context with the goep req_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepClientApp * goep_find_client_with_req_id(int req_id)
{
    int i;
    int j = 0;

    for(i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++)
    {
        if(&(GOEC(clients)[i]) != NULL)
        {
            if(GOEC(clients)[i].obc.req_id == req_id)
            {
                while (j < GOEP_MAX_PROFILES && GOEC(clients)[i].profiles[j] == NULL)
                    j++;

                if (j < GOEP_MAX_PROFILES)
                    return GOEC(clients)[i].profiles[j];
                else
                    return NULL;
            }
        }
    }
    return NULL;
}

/*****************************************************************************
* FUNCTION
*  goep_bd_addr_stuct2array
* DESCRIPTION
*  This function is to transfer the format of BD address from structure to array
* PARAMETERS
*  src     IN       The source of the address translation
*  dst     IN       The destinatio of the address translation
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_bd_addr_stuct2array(goep_bd_addr_struct *src, BD_ADDR *dst)
{
    btmtk_os_memcpy( (U8 *) &dst->addr[0], (U8 *) &src->lap, 3);
    dst->addr[3] = src->uap;
    btmtk_os_memcpy( (U8 *) &dst->addr[4], (U8 *) &src->nap, 2);
}

/*****************************************************************************
* FUNCTION
*  goep_bd_addr_array2struct
* DESCRIPTION
*  This function is to transfer the format of BD address from array to structure
* PARAMETERS
*  src     IN       The source of the address translation
*  dst     IN       The destinatio of the address translation
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_bd_addr_array2struct(BD_ADDR *src, goep_bd_addr_struct *dst)
{
    dst->lap = 0;
    btmtk_os_memcpy( (U8 *) &dst->lap, &src->addr[0], 3);
    dst->uap = src->addr[3];
    btmtk_os_memcpy( (U8 *) &dst->nap, &src->addr[4], 2);
}

/*****************************************************************************
* FUNCTION
*  goep_find_bt_uuid_with_obex_uuid
* DESCRIPTION
*  This function is to find out the Bluetooth uuid with Obex uuid
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
int goep_find_bt_uuid_with_obex_uuid(U32* bt_uuid_p, U8* obex_uuid_str)
{
    if(bt_strncmp((const char*)obex_uuid_str, (const char*)FileXferUuid, GOEP_MAX_UUID_SIZE) == 0)
    {
        *bt_uuid_p = SC_OBEX_FILE_TRANSFER;
    }
    else if(*obex_uuid_str == 0)
    {
        *bt_uuid_p = SC_OBEX_OBJECT_PUSH;
    }
    else
    {
        *bt_uuid_p = 0;
        return -1; /* Can not find out free goep connection id */
    }
    return GOEP_STATUS_SUCCESS;
}

/*****************************************************************************
* FUNCTION
*  goep_find_obex_uuid_with_bt_uuid
* DESCRIPTION
*  This function is to find out the Obex uuid with Bluetooth uuid
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
int goep_find_obex_uuid_with_bt_uuid(U32* bt_uuid_p, U8* obex_uuid_str, U8* obex_uuid_len)
{
	ASSERT(obex_uuid_str && obex_uuid_len);

    if(bt_uuid_p && *bt_uuid_p == SC_OBEX_FILE_TRANSFER)
    {
        *obex_uuid_len = sizeof(FileXferUuid);
        btmtk_os_memcpy(obex_uuid_str, FileXferUuid, sizeof(FileXferUuid));
        return GOEP_STATUS_SUCCESS;
    }
    else if(bt_uuid_p == NULL)
    {
        *obex_uuid_len = 0;
        *bt_uuid_p = 0;
        return GOEP_STATUS_SUCCESS;
    }
    else
    {
        *obex_uuid_len = 0;
        *bt_uuid_p = 0;
        return -1; /* Can not find out free goep connection id */
    }
}

/*****************************************************************************
* FUNCTION
*  goep_main
*
* DESCRIPTION
*   This is main() function of GOEP module.
*
* PARAMETERS
*  ilm_ptr     IN       The primitives
*
* RETURNS
*  none.
*
* GLOBALS AFFECTED
*  none.
*****************************************************************************/
void goep_event_handler(U32 msg_id, void* data)
{
    /// data is 'ilm_ptr->local_para_ptr' in MAUI platform
	if ( NULL != data)
    {
		switch(msg_id)
		{
			case MSG_ID_BT_GOEPS_REGISTER_SERVER_REQ:
                goep_register_server_req_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_DEREGISTER_SERVER_REQ:
			    goep_deregister_server_req_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_CONNECT_RSP:
			    goep_connect_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_AUTHORIZE_RSP:
			    goep_authorize_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_PUSH_RSP:
                goep_push_rsp_hdlr(data);
                break;
  			case MSG_ID_BT_GOEPS_PULL_RSP:
                goep_pull_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_SET_FOLDER_RSP:
			    goep_set_folder_rsp_hdlr(data);
                break;
                case MSG_ID_BT_GOEPS_ACTION_RSP:
	      	    goep_action_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_ABORT_RSP:
			    goep_abort_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_AUTH_REQ:
			    goep_auth_req_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_AUTH_RSP:
				goep_auth_rsp_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_CONNECT_REQ:
				goep_connect_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_PUSH_REQ:
				goep_push_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_PULL_REQ:
				goep_pull_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_SET_FOLDER_REQ:
				goep_set_folder_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_ACTION_REQ:
				goep_action_req_hdlr(data);
				break;				
  			case MSG_ID_BT_GOEPC_ABORT_REQ:
				goep_abort_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_DISCONNECT_REQ:
				goep_disconnect_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_AUTH_REQ:
				goep_auth_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_AUTH_RSP:
				goep_auth_rsp_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPS_TPDISCONNECT_REQ:
				// The function is useless but can't terminate the connection.
				// goep_TpDisconnect_req_hdlr(data);
				goep_disconnect_req_hdlr(data);
				break;
			default:
				kal_trace(BT_TRACE_G6_OBEX, GOEP_INVALID_MSG,msg_id,
					      MSG_ID_BT_GOEPS_REGISTER_SERVER_REQ,MSG_ID_BT_GOEPS_AUTH_RSP);
				// ASSERT(0);
		}
	}else{
	    ASSERT(0);
	}
} /* end of module main function */

/*****************************************************************************
* FUNCTION
*  goep_send_rsp_msg
* DESCRIPTION
*  This function is to send response message with status code to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN       The result of the register server process
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_cnf_msg(kal_uint32 cnf_msg_id, kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_cnf_struct* goep_cnf = NULL;

    goep_cnf = (goep_cnf_struct*)
        construct_local_para(sizeof(goep_cnf_struct), TD_CTRL);
    ASSERT(goep_cnf != NULL);

    btmtk_os_memset(goep_cnf->uuid, 0, sizeof(goep_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_cnf->uuid, uuid_p, uuid_len);

    goep_cnf->uuid_len = uuid_len;
    goep_cnf->goep_conn_id = goep_conn_id;
    goep_cnf->rsp_code = (U8)rsp_code;

    GOEP_APPROFILE_DISPATCH(cnf_msg_id, goep_cnf , sizeof(goep_cnf_struct) );

}


/*****************************************************************************
* FUNCTION
*  goep_send_disconnect_ind_msg
* DESCRIPTION
*  This function is to send connect indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN       The result of the disconnect server process
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_disconnect_ind_msg(kal_uint8 req_id, kal_uint8 goep_conn_id, kal_uint8* uuid_p, kal_uint8 uuid_len, kal_uint8 reason)
{
    goep_disconnect_ind_struct* goep_disconnect_ind;
    GoepServerApp *Service;
    GoepClientApp* client;

    /* Attention:
     * uuid_p may be member of client that will be deregistered as following.
     * so record the data here.
     */
    goep_disconnect_ind = (goep_disconnect_ind_struct*)
        construct_local_para(sizeof(goep_disconnect_ind_struct), TD_CTRL);
    ASSERT(goep_disconnect_ind != NULL);

    goep_disconnect_ind->goep_conn_id = goep_conn_id;
    goep_disconnect_ind->req_id = req_id;

    btmtk_os_memset(goep_disconnect_ind->uuid, 0, sizeof(goep_disconnect_ind->uuid));
    goep_disconnect_ind->reason = (U8) reason;
    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_disconnect_ind->uuid, uuid_p, uuid_len);

    goep_disconnect_ind->uuid_len = uuid_len;


    /***** search the server first *****/
    Service = goep_find_server_with_goep_conn_id(goep_conn_id);

    if(Service == NULL)
    {
        if(goep_conn_id == 0xFF)
        {
            client = goep_find_client_with_req_id(req_id);
            if(client != NULL)
            {
                if (client->cm_conn_id != GOEP_INVALID_CMID)
                {
                    OS_Report("[GOEP]client ReleaseConnID: first" ); //ALPS00360017
                    kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_RELEASE_CONNID, client->cm_conn_id);
                    BTBMProfileReleaseConnIndex((U8)client->cm_conn_id);
                    client->cm_conn_id = GOEP_INVALID_CMID;
                }

                GOEP_DeregisterClient(client);
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                /* free password memory -- added by yfchu */
                if (client->connect_req.password)
                    free_ctrl_buffer(client->connect_req.password);

                free_clientapp_ctrl_buffer(client);
            }
        }
        else
        {
            client = goep_find_client_with_goep_conn_id(goep_conn_id);
            if(client != NULL)
            {
                if (client->cm_conn_id != GOEP_INVALID_CMID)
                {
                    kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_RELEASE_CONNID, client->cm_conn_id);
                    BTBMProfileReleaseConnIndex((U8)client->cm_conn_id);
                    client->cm_conn_id = GOEP_INVALID_CMID;
                }

                GOEP_DeregisterClient(client);
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                    /* free password memory -- added by yfchu */
                    if (client->connect_req.password)
                        free_ctrl_buffer(client->connect_req.password);
                free_clientapp_ctrl_buffer(client);
            }
        }
		GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_TPDISCONNECT_IND, goep_disconnect_ind , sizeof(goep_disconnect_ind_struct) );
    }
    else
    {
        if (Service->cm_conn_id != GOEP_INVALID_CMID)
        {
            kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVICE_RELEASE_CONNID, Service->cm_conn_id);
            BTBMProfileReleaseConnIndex((U8)Service->cm_conn_id);
            Service->cm_conn_id = GOEP_INVALID_CMID;
        }

        /* free password memory -- add by yfchu */
        if (Service->password)
        {
            free_ctrl_buffer(Service->password);
            Service->password = NULL;
        }
		GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_TPDISCONNECT_IND, goep_disconnect_ind , sizeof(goep_disconnect_ind_struct) );
    }



}

/*****************************************************************************
* FUNCTION
*  goep_send_connect_ind_msg
* DESCRIPTION
*  This function is to send connect indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  bd_addr     IN       The address of the disconnected BT device
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_connect_ind_msg(U8 goep_conn_id, U16 peer_mru, U8* uuid_p, U8 uuid_len, U8 challenge, U8 response)
{
    goep_connect_ind_struct* goep_connect_ind = NULL;
    U32 profile_id;
    ObexTpConnInfo      tpInfo;
    kal_bool success;
    ObexAppHandle* obexapphdl_p;
    goep_bd_addr_struct goep_bd_addr;
    char* dev_name = 0;
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_conn_id);

    ASSERT(Service != NULL);

    goep_connect_ind = (goep_connect_ind_struct*)
        construct_local_para(sizeof(goep_connect_ind_struct), TD_CTRL);
    ASSERT(goep_connect_ind != NULL);

    goep_connect_ind->goep_conn_id = goep_conn_id;
    obexapphdl_p = &(GOES(servers)[Service->connId].obs.handle);

    success = OBEX_GetTpConnInfo(obexapphdl_p, &tpInfo);
    ASSERT(success);
    goep_bd_addr_array2struct(&(tpInfo.remDev->bdAddr), &goep_bd_addr);
    goep_connect_ind->bd_addr = goep_bd_addr;

    dev_name = (char *)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr)) ;

    btmtk_os_memset(goep_connect_ind->dev_name, 0, sizeof(goep_connect_ind->dev_name));

    if(dev_name != NULL)
        bt_strncpy((char*)goep_connect_ind->dev_name, (char*)dev_name, GOEP_MAX_DEV_NAME);

    btmtk_os_memset(goep_connect_ind->uuid, 0, sizeof(goep_connect_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_connect_ind->uuid, uuid_p, uuid_len);

    goep_connect_ind->uuid_len = uuid_len;

    goep_find_bt_uuid_with_obex_uuid((U32*)&profile_id, goep_connect_ind->uuid);

    if (Service->cm_conn_id == GOEP_INVALID_CMID)
    {
        Service->cm_conn_id = (U32) BTBMProfileAllocateConnIndex(profile_id, tpInfo.remDev->bdAddr);
        kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVICE_ALLOC_CONNID, Service->cm_conn_id);
    }
    else
    {
        OS_Report(("[GOEP] Service->cm_conn_id has alloced before!"));
		kal_trace(BT_TRACE_G6_OBEX, GOEP_CMCONNID_HAS_ALLOCED,Service->cm_conn_id);
    }

    goep_connect_ind->cm_conn_id = Service->cm_conn_id;
    goep_connect_ind->peer_mru = peer_mru;

    goep_connect_ind->client_auth = challenge;
    goep_connect_ind->server_auth = response;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_CONNECT_IND, goep_connect_ind  , sizeof(goep_connect_ind_struct) );

}

#ifdef __BT_GOEPS_AUTO_ACK__
static void goep_switch_server_buffer( ObexServerApp* ObexAppHandle ){

    if( ObexAppHandle->goep_buff_index == 0 )
    {
        ObexAppHandle->goep_buff_index = 1;
        ObexAppHandle->goep_buff_ptr += OPPS_MAX_OBEX_PACKET_LENGTH;
    }
    else
    {
        ObexAppHandle->goep_buff_index = 0;
        ObexAppHandle->goep_buff_ptr -= OPPS_MAX_OBEX_PACKET_LENGTH;
    }
}
#endif
#ifdef __BT_GOEPC_AUTO_ACK__
static void goep_switch_client_buffer( ObexClientApp* ObexAppHandle ){

    if( ObexAppHandle->goep_buff_index == 0 )
    {
        ObexAppHandle->goep_buff_index = 1;
    }
    else
    {
        ObexAppHandle->goep_buff_index = 0;
    }
}
#endif

/*****************************************************************************
* FUNCTION
*  goep_send_push_ind_msg
* DESCRIPTION
*  This function is to send push indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  pkt_type     IN   The received packet type
*  put_type     IN   The received put command type
*  total_obj_len     IN   The size of the receiving object
*  obj_name     IN   The name of the receiving object
*  frag_ptr     IN   The pointer of the receiving buffer
*  frag_len     IN   The length of the receiving buffer
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_push_ind_msg(kal_uint8 goep_conn_id, goep_pkt_type_enum pkt_type,
                            goep_put_type_enum put_type, kal_uint32 total_obj_len,
                            kal_uint8* obj_name, kal_uint16 obj_name_len,
                            kal_uint8* obj_mime_type, kal_uint16 obj_mime_type_len,
                            kal_uint8* frag_ptr, kal_uint16 frag_len,
                            kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_push_ind_struct* goep_push_ind = NULL;
    GoepServerApp *Service;

    Service = goep_find_server_with_goep_conn_id(goep_conn_id);
    ASSERT(Service != NULL);

    goep_push_ind = (goep_push_ind_struct*)construct_local_para(sizeof(goep_push_ind_struct), TD_CTRL);
    ASSERT(goep_push_ind != NULL);

#ifdef __BT_GOEPS_AUTO_ACK__

    // default: use the first buffer
    goep_push_ind->buff_index = 0;

    // get app context
    ObexServerApp* ObexAppHandle = &(GOES(servers)[Service->connId].obs);
//LOGW("[opps] PUHS IND: state[%i], index[%i], type[%i]", ObexAppHandle->goep_buff_state, ObexAppHandle->goep_buff_index, pkt_type );
    if( pkt_type == GOEP_FIRST_PKT )
    {
        // reset context variables and make sure first buffer is ready
            ObexAppHandle->goep_buff_state = 0;
        if( ObexAppHandle->goep_buff_index == 1 )
        {
            goep_switch_server_buffer( ObexAppHandle );
        }
        }
    else if( pkt_type == GOEP_NORMAL_PKT )
    {
        // before switch, keep the data buffer (filled by stack)
        goep_push_ind->buff_index = ObexAppHandle->goep_buff_index;

            // state == 0: have free buffer and can ack first
            if( ObexAppHandle->goep_buff_state == 0 ){

                // lock buffer
                ObexAppHandle->goep_buff_state = 1;

                // swap buffer
                goep_switch_server_buffer( ObexAppHandle );
//LOGW("[opps] PUHS RSP: state[%i], index[%i], type[EARLY]", ObexAppHandle->goep_buff_state, ObexAppHandle->goep_buff_index );
                // buffer is ready for next block => ack first
                ObexAppHandle->goep_buff_wrote_len = 0;
                GOEP_ServerContinue(Service);
            }
            else {

            // wait for RSP from external and free the buffer
                ObexAppHandle->goep_buff_state = 2; // blocked
            }
        }
    else if( pkt_type == GOEP_FINAL_PKT )
    {
        // keep the data buffer pointer
        goep_push_ind->buff_index = ObexAppHandle->goep_buff_index;

        // reset buffer state ( make sure first buffer is ready for next request )
        if( ObexAppHandle->goep_buff_index == 1 )
        {
            goep_switch_server_buffer( ObexAppHandle );
        }
    }

#endif

    goep_push_ind->goep_conn_id = goep_conn_id;
    goep_push_ind->pkt_type = pkt_type;

    goep_push_ind->put_type = put_type;
    goep_push_ind->total_obj_len = total_obj_len;

    btmtk_os_memset(goep_push_ind->obj_name, 0, sizeof(goep_push_ind->obj_name));

    if(obj_name != NULL)
    {
        if(obj_name_len >= GOEP_MAX_OBJ_NAME - 1)
        {
            btmtk_os_memcpy(goep_push_ind->obj_name, obj_name, (GOEP_MAX_OBJ_NAME-2));
            goep_push_ind->obj_name[GOEP_MAX_OBJ_NAME-2] = 0;
            goep_push_ind->obj_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }
        else
        {
            btmtk_os_memcpy(goep_push_ind->obj_name, obj_name, obj_name_len);
            goep_push_ind->obj_name[obj_name_len] = 0;
            goep_push_ind->obj_name[obj_name_len+1] = 0;
        }
    }

    btmtk_os_memset(goep_push_ind->obj_mime_type, 0, sizeof(goep_push_ind->obj_mime_type));

    if(obj_mime_type != NULL)
    {
        if( (obj_mime_type_len+2) >= GOEP_MAX_MIME_TYPE)
        {
            btmtk_os_memcpy(goep_push_ind->obj_mime_type, obj_mime_type, (GOEP_MAX_MIME_TYPE-2));
            goep_push_ind->obj_mime_type[GOEP_MAX_MIME_TYPE-2] = 0;
            goep_push_ind->obj_mime_type[GOEP_MAX_MIME_TYPE-1] = 0;
        }
        else
        {
            btmtk_os_memcpy(goep_push_ind->obj_mime_type, obj_mime_type, obj_mime_type_len);
            goep_push_ind->obj_mime_type[obj_mime_type_len] = 0;
            goep_push_ind->obj_mime_type[obj_mime_type_len+1] = 0;
        }
    }

    goep_push_ind->frag_ptr = frag_ptr;
    goep_push_ind->frag_len = frag_len;

    btmtk_os_memset(goep_push_ind->uuid, 0, sizeof(goep_push_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_push_ind->uuid, uuid_p, uuid_len);

    goep_push_ind->uuid_len = uuid_len;

    //Service = goep_find_server_with_goep_conn_id(goep_conn_id);
    GOEP_ServerResetPushNameInfo(Service);

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_PUSH_IND, goep_push_ind , sizeof(goep_push_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_pull_ind_msg
* DESCRIPTION
*  This function is to send pull indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  obj_mime_type     IN   The mime_type of the receiving object
*  obj_name     IN   The file name of the receiving object
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_pull_ind_msg(kal_uint8 goep_conn_id, goep_pkt_type_enum pkt_type,
                            kal_uint8* obj_mime_type, kal_uint8* obj_name, kal_uint16 obj_name_len,
                            kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_pull_ind_struct* goep_pull_ind = NULL;

    goep_pull_ind = (goep_pull_ind_struct*)
        construct_local_para(sizeof(goep_pull_ind_struct), TD_CTRL);
    ASSERT(goep_pull_ind != NULL);

    goep_pull_ind->goep_conn_id = goep_conn_id;
    goep_pull_ind->pkt_type = pkt_type;
    if(obj_mime_type != NULL)
        bt_strncpy((char*)goep_pull_ind->obj_mime_type, (char*)obj_mime_type, GOEP_MAX_MIME_TYPE);
    else
        goep_pull_ind->obj_mime_type[0] = 0;

    btmtk_os_memset(goep_pull_ind->obj_name, 0, sizeof(goep_pull_ind->obj_name));

    if(obj_name!= NULL)
    {
        if(obj_name_len >= GOEP_MAX_OBJ_NAME - 1)
        {
            btmtk_os_memcpy(goep_pull_ind->obj_name, obj_name, (GOEP_MAX_OBJ_NAME-2));
            goep_pull_ind->obj_name[GOEP_MAX_OBJ_NAME-2] = 0;
            goep_pull_ind->obj_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }
        else
        {
            btmtk_os_memcpy(goep_pull_ind->obj_name, obj_name, obj_name_len);
            goep_pull_ind->obj_name[obj_name_len] = 0;
            goep_pull_ind->obj_name[obj_name_len+1] = 0;
        }
    }

    btmtk_os_memset(goep_pull_ind->uuid, 0, sizeof(goep_pull_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_pull_ind->uuid, uuid_p, uuid_len);

    goep_pull_ind->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_PULL_IND, goep_pull_ind , sizeof(goep_pull_ind_struct) );

}


/*****************************************************************************
* FUNCTION
*  goep_send_setfolder_ind_msg
* DESCRIPTION
*  This function is to send setfolder indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  setpath_flag     IN   Indicate the setpath command is to forward, back to parent,
*                        or back to root
*  folder_name     IN   The folder name of the setting path
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_set_folder_ind_msg(kal_uint8 goep_conn_id, goep_set_folder_type_enum setpath_flag,
                            kal_uint8* folder_name, kal_uint16 folder_name_len, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_set_folder_ind_struct* goep_set_folder_ind = NULL;

    goep_set_folder_ind = (goep_set_folder_ind_struct*)
        construct_local_para(sizeof(goep_set_folder_ind_struct), TD_CTRL);
    ASSERT(goep_set_folder_ind != NULL);

    goep_set_folder_ind->goep_conn_id = goep_conn_id;
    goep_set_folder_ind->setpath_flag = setpath_flag;

    btmtk_os_memset(goep_set_folder_ind->folder_name, 0, sizeof(goep_set_folder_ind->folder_name));

    if(folder_name!= NULL)
    {
        if(folder_name_len >= GOEP_MAX_OBJ_NAME - 1)
        {
            btmtk_os_memcpy(goep_set_folder_ind->folder_name, folder_name, (GOEP_MAX_OBJ_NAME-2));
            goep_set_folder_ind->folder_name[GOEP_MAX_OBJ_NAME-2] = 0;
            goep_set_folder_ind->folder_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }
        else
        {
            btmtk_os_memcpy(goep_set_folder_ind->folder_name, folder_name, folder_name_len);
            goep_set_folder_ind->folder_name[folder_name_len] = 0;
            goep_set_folder_ind->folder_name[folder_name_len+1] = 0;
        }
    }

    btmtk_os_memset(goep_set_folder_ind->uuid, 0, sizeof(goep_set_folder_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_set_folder_ind->uuid, uuid_p, uuid_len);

    goep_set_folder_ind->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_SET_FOLDER_IND, goep_set_folder_ind , sizeof(goep_set_folder_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_action_ind_msg
* DESCRIPTION
*  This function is to send setfolder indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  setpath_flag     IN   Indicate the setpath command is to forward, back to parent,
*                        or back to root
*  folder_name     IN   The folder name of the setting path
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_action_ind_msg(U8 goep_conn_id, U8 action, U8 permission, 
                            U8* folder_name, U16 folder_name_len, U8* dst_name, U16 dst_name_len, U8* uuid_p, U8 uuid_len)
{
    goep_action_ind_struct* goep_action_ind = NULL;

    goep_action_ind = (goep_action_ind_struct*)
        construct_local_para(sizeof(goep_action_ind_struct), TD_CTRL);
    ASSERT(goep_action_ind != NULL);

    goep_action_ind->goep_conn_id = goep_conn_id;
	goep_action_ind->action_id = action;
	goep_action_ind->permission = permission; 
    //goep_action_ind->setpath_flag = setpath_flag;

    btmtk_os_memset(goep_action_ind->folder_name, 0, sizeof(goep_action_ind->folder_name));

    if(folder_name!= NULL)
    {
        if(folder_name_len >= (GOEP_MAX_OBJ_NAME-2))
        {
            btmtk_os_memcpy(goep_action_ind->folder_name, folder_name, (GOEP_MAX_OBJ_NAME-2));
            goep_action_ind->folder_name[GOEP_MAX_OBJ_NAME-2] = 0;
            goep_action_ind->folder_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(goep_action_ind->folder_name, folder_name, folder_name_len);
            goep_action_ind->folder_name[folder_name_len] = 0;
            goep_action_ind->folder_name[folder_name_len+1] = 0;
        }
    }
    if(dst_name!= NULL)
    {
        if(dst_name_len >= (GOEP_MAX_OBJ_NAME-2) )
        {
            btmtk_os_memcpy(goep_action_ind->dst_name, dst_name, (GOEP_MAX_OBJ_NAME-2));
            goep_action_ind->dst_name[GOEP_MAX_OBJ_NAME-2] = 0;
            goep_action_ind->dst_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(goep_action_ind->dst_name, dst_name, dst_name_len);
            goep_action_ind->dst_name[dst_name_len] = 0;
            goep_action_ind->dst_name[dst_name_len+1] = 0;
        }
    }    

    btmtk_os_memset(goep_action_ind->uuid, 0, sizeof(goep_action_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_action_ind->uuid, uuid_p, uuid_len);

    goep_action_ind->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_ACTION_IND, goep_action_ind , sizeof(goep_action_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_abort_ind_msg
* DESCRIPTION
*  This function is to send abort indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_abort_ind_msg(kal_uint8 goep_conn_id, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_abort_ind_struct* goep_abort_ind = NULL;

    goep_abort_ind = (goep_abort_ind_struct*)
        construct_local_para(sizeof(goep_abort_ind_struct), TD_CTRL);
    ASSERT(goep_abort_ind != NULL);

    goep_abort_ind->goep_conn_id = goep_conn_id;

    btmtk_os_memset(goep_abort_ind->uuid, 0, sizeof(goep_abort_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_abort_ind->uuid, uuid_p, uuid_len);

    goep_abort_ind->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_ABORT_IND, goep_abort_ind , sizeof(goep_abort_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_register_cnf_msg
* DESCRIPTION
*  This function is to send register response message with status code to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN       The result of the register server process
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_register_cnf_msg(kal_uint32 cnf_msg_id, kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8* uuid_p, kal_uint8 uuid_len, kal_uint8 req_id)
{
    goep_register_server_cnf_struct* register_server_cnf = NULL;

    register_server_cnf = (goep_register_server_cnf_struct*)
        construct_local_para(sizeof(goep_register_server_cnf_struct), TD_CTRL);
    ASSERT(register_server_cnf != NULL);

    btmtk_os_memset(register_server_cnf->uuid, 0, sizeof(register_server_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(register_server_cnf->uuid, uuid_p, uuid_len);

    register_server_cnf->uuid_len = uuid_len;
    register_server_cnf->req_id = req_id;
    register_server_cnf->goep_conn_id = goep_conn_id;
    register_server_cnf->rsp_code = (U8)rsp_code;

    GOEP_APPROFILE_DISPATCH( cnf_msg_id, register_server_cnf , sizeof(goep_register_server_cnf_struct) );

}

/*****************************************************************************
* FUNCTION
*  goep_send_connect_cnf_msg
* DESCRIPTION
*  This function is to send connect response message to the application
* PARAMETERS
*  req_id     IN   The requesting id from application
*  goep_conn_id     IN   The assigned GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
*  mtu     IN   The maximum receive size sent from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_connect_cnf_msg(kal_uint8 req_id, kal_uint8 goep_conn_id, ObStatus rsp_code, U16 peer_mru, U8* uuid_p, U8 uuid_len)
{
    goep_connect_cnf_struct* goep_connect_cnf = NULL;
    GoepClientApp* client;
    ObexClientApp* ObexApp;
#if BT_GOEP_V2 == XA_ENABLED
	U8 idx;
#endif

    goep_connect_cnf = (goep_connect_cnf_struct*)
        construct_local_para(sizeof(goep_connect_cnf_struct), TD_CTRL);
    ASSERT(goep_connect_cnf != NULL);

    goep_connect_cnf->req_id = req_id;

    btmtk_os_memset(goep_connect_cnf->uuid, 0, sizeof(goep_connect_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_connect_cnf->uuid, uuid_p, uuid_len);

    goep_connect_cnf->uuid_len = uuid_len;
    goep_connect_cnf->rsp_code = (U8)rsp_code;

    kal_trace(BT_TRACE_G6_OBEX,GOEP_SEND_CONNECT_RSP, rsp_code);

    if((rsp_code == OB_STATUS_SUCCESS) || (rsp_code == OB_STATUS_PENDING))
    {
        client = (GoepClientApp *)goep_find_client_with_goep_conn_id(goep_conn_id);
        ASSERT(client != NULL);
        ObexApp = &(GOEC(clients)[client->connId].obc);
        goep_connect_cnf->goep_conn_id = goep_conn_id;
        client->obex_conn_is_connected = KAL_TRUE;
        client->cm_conn_id = (U32) BTBMProfileAllocateConnIndex(ObexApp->trans.ObexClientBtTrans.target_uuid, ObexApp->trans.ObexClientBtTrans.remoteDevice->bdAddr);
        goep_connect_cnf->cm_conn_id = client->cm_conn_id;
        goep_connect_cnf->peer_mru = peer_mru;
        if( goep_connect_cnf->uuid_len > 0 ){
            goep_connect_cnf->peer_mru -= 5;
        }
        kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_ALLOC_CONNID, client->cm_conn_id);
    }
    else
    {
        client = (GoepClientApp *)goep_find_client_with_req_id(req_id);
        if (client == NULL)
        {
            goto SND_MSG;
        }
        ASSERT(client != NULL);

#if GOEP_AUTH == XA_ENABLED
        if (client->goep_auth_verify_fail)
        {
            /* if authentication fail,  goep.c call GOEP_TpDisconnect() */
        }
        else
#endif
        {
			 OS_Report("[OBEX] client->connState is %d", client->connState);
			 switch(client->connState){
				 case CS_CONNECTED:
					 // will be disconnect by obex layer
				 break;
			     case CS_DISCONNECTED:
				 case CS_DISCONNECTING:
				 default:
				 {
        GOEP_DeregisterClient(client);
                                  if(client->connect_req.target){
            free_ctrl_buffer(client->connect_req.target);
                                  }
                                
#if BT_GOEP_V2 == XA_ENABLED
                                    if( NULL != client->pl2capRawData[0] && 0 != client->l2capRawDataNum ){
                                        for( idx = 0; idx < client->l2capRawDataNum; idx++){
                                            ASSERT( client->pl2capRawData[i] );
                                            free_ctrl_buffer(client->pl2capRawData[idx]);
                                            client->pl2capRawData[idx] = NULL;
                                         }
                            	    }else{
                                         client->l2capRawDataNum = 0;
                                    }
#endif

                                 free_clientapp_ctrl_buffer(client);
                  } // default
				 break;
           } //switch
        }
    }

SND_MSG:
    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_CONNECT_CNF, goep_connect_cnf , sizeof(goep_connect_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_push_cnf_msg
* DESCRIPTION
*  This function is to send push response message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_push_cnf_msg(kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8 reason, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
#ifdef __BT_GOEPC_AUTO_ACK__

    GoepClientApp* client = goep_find_client_with_goep_conn_id(goep_conn_id);
    ObexClientApp* ObexApp = &(GOEC(clients)[client->connId].obc);
    ASSERT(ObexApp != NULL);

    // 1: real CNF is received before next REQ => wait for next REQ
        if( ObexApp->goep_buff_state == 1 )
        {
            ObexApp->goep_buff_state = 0;

        if( ObexApp->req_pkt_type == GOEP_NORMAL_PKT )
        {
            return;
        }
    }
    // 2: real CNF is received after next REQ => send REQ
        else if( ObexApp->goep_buff_state == 2 )
        {
        // send REQ (push data)
            ObexApp->goep_buff_ptr = ObexApp->goep_buff_next_ptr + (OPPC_MAX_OBEX_PACKET_LENGTH*ObexApp->goep_buff_index);
            ObexApp->goep_buff_size = ObexApp->goep_buff_next_size;
            ObexApp->goep_buff_wrote_len = 0;
            GOEP_ClientContinue(client);

            // send cnf to external-layer => fill buffer
        ObexApp->goep_buff_state = 1;

        // FINAL PTK arrived
        if( ObexApp->req_pkt_type == GOEP_FINAL_PKT ){

            return;
        }
        }

    goep_send_push_cnf_msg2(ObexApp, goep_conn_id, rsp_code, reason, uuid_p, uuid_len);
    }

void goep_send_push_cnf_msg2(ObexClientApp* ObexApp, kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8 reason, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    //LOGW( "[oppc] PUSH_CNF: auto[%i], state[%i], index[%i]", ObexApp->goep_auto_ack, ObexApp->goep_buff_state, ObexApp->goep_buff_index );
    goep_switch_client_buffer( ObexApp );
#endif

    goep_push_cnf_struct* goep_push_cnf = NULL;
    goep_push_cnf = (goep_push_cnf_struct*)construct_local_para(sizeof(goep_push_cnf_struct), TD_CTRL);
    ASSERT(goep_push_cnf != NULL);

#ifdef __BT_GOEPC_AUTO_ACK__
    goep_push_cnf->buff_index = ObexApp->goep_buff_index;
#endif
    goep_push_cnf->goep_conn_id = goep_conn_id;
    goep_push_cnf->rsp_code = (U8)rsp_code;
    goep_push_cnf->reason = reason;

    btmtk_os_memset(goep_push_cnf->uuid, 0, sizeof(goep_push_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_push_cnf->uuid, uuid_p, uuid_len);

    goep_push_cnf->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_PUSH_CNF, goep_push_cnf , sizeof(goep_push_cnf_struct) );
}


/*****************************************************************************
* FUNCTION
*  goep_send_pull_cnf_msg
* DESCRIPTION
*  This function is to send pull response message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_pull_cnf_msg(kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8 reason, goep_pkt_type_enum pkt_type,
                            kal_uint8* obj_mime_type, kal_uint32 total_obj_len, kal_uint8* frag_ptr,
                            kal_uint16 frag_len, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_pull_cnf_struct* goep_pull_cnf = NULL;

    goep_pull_cnf = (goep_pull_cnf_struct*)
        construct_local_para(sizeof(goep_pull_cnf_struct), TD_CTRL);
    ASSERT(goep_pull_cnf != NULL);

    goep_pull_cnf->rsp_code = (U8)rsp_code;
    goep_pull_cnf->reason = reason;

    btmtk_os_memset(goep_pull_cnf->uuid, 0, sizeof(goep_pull_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_pull_cnf->uuid, uuid_p, uuid_len);

    goep_pull_cnf->uuid_len = uuid_len;
    goep_pull_cnf->goep_conn_id = goep_conn_id;
    goep_pull_cnf->pkt_type = pkt_type;


    if((pkt_type == GOEP_FIRST_PKT)||(pkt_type == GOEP_SINGLE_PKT))
    {
        goep_pull_cnf->total_obj_len = total_obj_len;
    }
    goep_pull_cnf->frag_ptr = frag_ptr;
    goep_pull_cnf->frag_len = frag_len;

    /* added for continue parsing large folder content */
    goep_pull_cnf->to_self = FALSE;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_PULL_CNF, goep_pull_cnf , sizeof(goep_pull_cnf_struct) );
}


/*****************************************************************************
* FUNCTION
*  goep_send_set_folder_cnf_msg
* DESCRIPTION
*  This function is to send push response message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_set_folder_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len)
{
    goep_set_folder_cnf_struct* goep_setfolder_cnf = NULL;

    goep_setfolder_cnf = (goep_set_folder_cnf_struct*)
        construct_local_para(sizeof(goep_set_folder_cnf_struct), TD_CTRL);
    ASSERT(goep_setfolder_cnf != NULL);

    goep_setfolder_cnf->goep_conn_id = goep_conn_id;
    goep_setfolder_cnf->rsp_code = (U8)rsp_code;
    goep_setfolder_cnf->reason = reason;

    btmtk_os_memset(goep_setfolder_cnf->uuid, 0, sizeof(goep_setfolder_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_setfolder_cnf->uuid, uuid_p, uuid_len);

    goep_setfolder_cnf->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_SET_FOLDER_CNF, goep_setfolder_cnf , sizeof(goep_set_folder_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_action_cnf_msg
* DESCRIPTION
*  This function is to send action response message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_action_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len)
{
    goep_action_cnf_struct* goep_action_cnf = NULL;

    goep_action_cnf = (goep_action_cnf_struct*)
        construct_local_para(sizeof(goep_action_cnf_struct), TD_CTRL);
    ASSERT(goep_action_cnf != NULL);

    goep_action_cnf->goep_conn_id = goep_conn_id;
    goep_action_cnf->rsp_code = (U8)rsp_code;
    goep_action_cnf->reason = reason;

    btmtk_os_memset(goep_action_cnf->uuid, 0, sizeof(goep_action_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_action_cnf->uuid, uuid_p, uuid_len);

    goep_action_cnf->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_ACTION_CNF, goep_action_cnf , sizeof(goep_action_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_abort_cnf_msg
* DESCRIPTION
*  This function is to send abort response message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_abort_cnf_msg(kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8* uuid_p, kal_uint8 uuid_len)
{
    goep_abort_cnf_struct* goep_abort_cnf = NULL;

    goep_abort_cnf = (goep_abort_cnf_struct*)
        construct_local_para(sizeof(goep_abort_cnf_struct), TD_CTRL);
    ASSERT(goep_abort_cnf != NULL);

    goep_abort_cnf->goep_conn_id = goep_conn_id;
    goep_abort_cnf->rsp_code = (U8)rsp_code;

    btmtk_os_memset(goep_abort_cnf->uuid, 0, sizeof(goep_abort_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_abort_cnf->uuid, uuid_p, uuid_len);

    goep_abort_cnf->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH ( MSG_ID_BT_GOEPC_ABORT_CNF, goep_abort_cnf , sizeof(goep_abort_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_auth_cnf_msg
* DESCRIPTION
*  This function is to send authentication response message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX authentication
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_auth_cnf_msg(kal_uint8 goep_conn_id, ObStatus rsp_code, kal_uint8* uuid_p, U8 uuid_len)
{
    goep_auth_cnf_struct* goep_auth_cnf = NULL;

    goep_auth_cnf = (goep_auth_cnf_struct*)
        construct_local_para(sizeof(goep_auth_cnf_struct), TD_CTRL);
    ASSERT(goep_auth_cnf != NULL);

    goep_auth_cnf->goep_conn_id = goep_conn_id;
    goep_auth_cnf->rsp_code = (U8)rsp_code;

    btmtk_os_memset(goep_auth_cnf->uuid, 0, sizeof(goep_auth_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_auth_cnf->uuid, uuid_p, uuid_len);

    goep_auth_cnf->uuid_len = uuid_len;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_AUTH_CNF, goep_auth_cnf , sizeof(goep_auth_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_auth_ind_msg
* DESCRIPTION
*  This function is to send OBEX authentication indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  realm     IN   The hint of the requested password
*  realm_len IN   The length of the inputed realm
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_auth_ind_msg(U8 goep_conn_id, U8* realm, U8 realm_len, U8* uuid_p, U8 uuid_len,U8 options)
{
    goep_auth_ind_struct* goep_auth_ind = NULL;

    ObexTpConnInfo      tpInfo;
    kal_bool success;
    ObexAppHandle* obexapphdl_p;
    char* dev_name = 0;
    GoepServerApp *Service = NULL;
	GoepClientApp *Client = NULL;
    goep_bd_addr_struct goep_bd_addr;

	Service = goep_find_server_with_goep_conn_id(goep_conn_id);
    if (Service == NULL)
	{
		Client = goep_find_client_with_goep_conn_id(goep_conn_id);

		ASSERT(Client != NULL);
	}

    goep_auth_ind = (goep_auth_ind_struct*)
        construct_local_para(sizeof(goep_auth_ind_struct), TD_CTRL);
    ASSERT(goep_auth_ind != NULL);

    goep_auth_ind->goep_conn_id = goep_conn_id;

    btmtk_os_memset(goep_auth_ind->realm, 0, sizeof(goep_auth_ind->realm));

    if((realm != NULL)&&(realm_len != 0))
        btmtk_os_memcpy(goep_auth_ind->realm, realm, realm_len);

    goep_auth_ind->realm_len = realm_len;

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(goep_auth_ind->uuid, uuid_p, uuid_len);

    goep_auth_ind->uuid_len = uuid_len;
    goep_auth_ind->options = options;


	if (Service != NULL)
	{
		obexapphdl_p = &(GOES(servers)[Service->connId].obs.handle);
	}
	else
	{
		obexapphdl_p = &(GOEC(clients)[Client->connId].obc.handle);
	}

    success = OBEX_GetTpConnInfo(obexapphdl_p, &tpInfo);
    ASSERT(success);

    goep_bd_addr_array2struct(&(tpInfo.remDev->bdAddr), &goep_bd_addr);

    dev_name = (char *)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr)) ;

    btmtk_os_memset(goep_auth_ind->dev_name, 0, sizeof(goep_auth_ind->dev_name));

    if(dev_name != NULL)
        bt_strncpy((char*)goep_auth_ind->dev_name, (char*)dev_name, GOEP_MAX_DEV_NAME);

	if( NULL != Service  ){
    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_AUTH_IND, goep_auth_ind , sizeof(goep_auth_ind_struct) );
	}else if( NULL != Client ){
		GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_AUTH_IND, goep_auth_ind , sizeof(goep_auth_ind_struct) );
	}else{
 		ASSERT(0);
	}
}

/*****************************************************************************
* FUNCTION
*  goep_send_authorize_ind_msg
* DESCRIPTION
*  This function is to send OBEX authorization indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  bd_addr     IN   The requesting bluetooth device address
*  dev_name    IN   The name of the requesting bluetooth device
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_authorize_ind_msg(U8 goep_conn_id, goep_bd_addr_struct* bd_addr_p, U8* dev_name)
{
    goep_authorize_ind_struct* goep_authorize_ind = NULL;
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_conn_id);

    ASSERT(Service != NULL);

    goep_authorize_ind = (goep_authorize_ind_struct*)
        construct_local_para(sizeof(goep_authorize_ind_struct), TD_CTRL);
    ASSERT(goep_authorize_ind != NULL);

    if(Service->target[0])
    {
        goep_authorize_ind->uuid_len = Service->target[0]->targetLen;

        btmtk_os_memset(goep_authorize_ind->uuid, 0, sizeof(goep_authorize_ind->uuid));

        if((Service->target[0]->target != NULL)&&(goep_authorize_ind->uuid_len != 0))
            btmtk_os_memcpy(goep_authorize_ind->uuid, Service->target[0]->target, Service->target[0]->targetLen);

        goep_authorize_ind->uuid_len = Service->target[0]->targetLen;
    }
    else
    {
        goep_authorize_ind->uuid_len = 0;
        goep_authorize_ind->uuid[0] = 0;
    }

    goep_authorize_ind->goep_conn_id = goep_conn_id;
    goep_authorize_ind->bd_addr = *bd_addr_p;

    btmtk_os_memset(goep_authorize_ind->dev_name, 0, sizeof(goep_authorize_ind->dev_name));

    if(dev_name != NULL)
        bt_strncpy((char*)goep_authorize_ind->dev_name, (char*)dev_name, sizeof(goep_authorize_ind->dev_name) );

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPS_AUTHORIZE_IND, goep_authorize_ind , sizeof(goep_authorize_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_send_opp_supported_formats_ind_msg
* DESCRIPTION
*  This function is to send OPP client OPP server supported format list of SDP result indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_send_opp_supported_formats_ind_msg(kal_uint8 goep_conn_id, kal_uint8 supported_list)
{
    goep_opp_supported_formats_ind_struct* goep_opp_supported_formats_ind = NULL;
    GoepClientApp *Client = goep_find_client_with_goep_conn_id(goep_conn_id);

    ASSERT(Client != NULL);

    goep_opp_supported_formats_ind = (goep_opp_supported_formats_ind_struct*)
        construct_local_para(sizeof(goep_opp_supported_formats_ind_struct), TD_CTRL);
    ASSERT(goep_opp_supported_formats_ind != NULL);

    goep_opp_supported_formats_ind->uuid_len = 0;
    goep_opp_supported_formats_ind->uuid[0] = 0;

    goep_opp_supported_formats_ind->goep_conn_id = goep_conn_id;
	goep_opp_supported_formats_ind->server_supported_formats = supported_list;

    GOEP_APPROFILE_DISPATCH( MSG_ID_BT_GOEPC_SUPPORTED_FORMATS_IND , goep_opp_supported_formats_ind , sizeof(goep_opp_supported_formats_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  goep_register_server_req_hdlr
* DESCRIPTION
*  This function is to handle the OBEX server registration
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_register_server_req_hdlr(goep_register_server_req_struct* register_server_req)
{
    // support duplicated registration case: for external layer terminated abnormally
    // 1. find existing server info
    int i;
    GoepProfile req_profile = (register_server_req->uuid_len != 0) ? GOEP_PROFILE_FTP : GOEP_PROFILE_OPUSH;
    for(i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        if(&(GOES(servers)[i]) != NULL)
        {
            //while (j < GOEP_MAX_PROFILES && GOES(servers)[i].profiles[j] == NULL)
            if(GOES(servers)[i].profiles[req_profile] != NULL)
            {
                // 2. disconnet current connection
                if ((GOES(servers)[i].flags & GOEF_ACTIVE) == 1) {

                    ObStatus rsp_code = GOEP_ServerTpDisconnect(GOES(servers)[i].profiles[req_profile]);
                    // TODO call goep_send_disconnect_ind_msg (ref: goep_disconnect_req_hdlr() )
                }
                // 3. send cnf msg (success)
                goep_send_register_cnf_msg( MSG_ID_BT_GOEPS_REGISTER_SERVER_CNF, GOES(servers)[i].obs.goep_conn_id, OB_STATUS_SUCCESS, register_server_req->uuid, register_server_req->uuid_len, register_server_req->req_id);
                return;
            }
        }
    }

    //GoepServerApp *Service = get_ctrl_buffer(sizeof(GoepServerApp));
    GoepServerApp *Service = (GoepServerApp *)get_ctrl_buffer(sizeof(GoepServerApp));
    ObStatus rsp_code;
    ObexAppHandle* obexapphdlr_p;
    GoepTpType tptype = GOEP_TP_RFCOMM_ONLY;
#if BT_GOEP_V2 == XA_ENABLED
	U8 idx;
#endif

    ASSERT(Service != NULL);

    btmtk_os_memset( (U8 *) Service, 0, sizeof(GoepServerApp));

#if BT_GOEP_V2 == XA_ENABLED
    Service->l2data.tpType = 0x0; /* OPP & FTP update to GOEP v2 version */
    tptype = Service->l2data.tpType;
#endif

    if(register_server_req->uuid_len != 0)
    {
        Service->target[0] = get_ctrl_buffer(sizeof(ObexConnection));
        ASSERT(Service->target[0] != NULL);
        Service->numTargets = 1;
        Service->target[0]->target = get_ctrl_buffer(register_server_req->uuid_len);
        ASSERT(Service->target[0]->target != NULL);
        Service->target[0]->targetLen = register_server_req->uuid_len;
        btmtk_os_memcpy(Service->target[0]->target, register_server_req->uuid, register_server_req->uuid_len);
        Service->bt_service = GOEP_PROFILE_FTP;
        Service->type = GOEP_PROFILE_FTP; /* We allow only one service to be registered in one OBEX channel */
        GOEP_Report("[goep] Register FTP Profile SDP Record ");
        ObexSdpDB_RegistrationEx(SC_OBEX_FILE_TRANSFER, tptype); //will casue connect fail
    }
    else
    {
        Service->numTargets = 0;
        Service->target[0] = NULL;
        Service->bt_service = GOEP_PROFILE_OPUSH;
        Service->type = GOEP_PROFILE_OPUSH; /* We allow only one service to be registered in one OBEX channel */
        GOEP_Report("[goep] Register OPP Profile SDP Record ");
	 ObexSdpDB_RegistrationEx(SC_OBEX_OBJECT_PUSH, tptype); //will casue connect fail
    }

    /* We do not allow existing connection to be re-used */
    /* the exact connection id will be assigned in GOEP_RegisterServer() */
    Service->connFlags = GOEP_NEW_CONN;
    Service->connId = 0;
    Service->callback = NULL;
    Service->cm_conn_id = GOEP_INVALID_CMID;

#if BT_GOEP_V2 == XA_ENABLED
    /* enable the GOEP/L2cap */
    GOEP_Report("[GOEP][SERV] prepare the l2data buffer--");
    Service->l2data.bSRM = FALSE;
    Service->l2data.tpType = OBEX_TP_RFCOMM_ONLY;
    Service->l2data.l2capRawDataNum = 0;
#endif

    rsp_code = GOEP_RegisterServer(Service, NULL);    /* service connection id must be assigned after here */

    /* Registration failed => return the error code to application */
    if(rsp_code)
    {
        if(Service)
        {
            if(Service->target[0])
            {
                if(Service->target[0]->target)
                    free_ctrl_buffer(Service->target[0]->target);
                free_ctrl_buffer(Service->target[0]);
            }
            free_ctrl_buffer(Service);
        }
        GOEP_Report("[goep] RegisterServer Fail !");
        /// send register result back
        goep_send_register_cnf_msg( MSG_ID_BT_GOEPS_REGISTER_SERVER_CNF, 0, rsp_code, register_server_req->uuid, register_server_req->uuid_len, register_server_req->req_id);
        return;
    }
    obexapphdlr_p = &(GOES(servers)[Service->connId].obs.handle);
    GOES(servers)[Service->connId].obs.goep_buff_ptr = register_server_req->buf_ptr;
    if( 'R' == register_server_req->rawheader && 0 != register_server_req->uuid_len ){
        GOES(servers)[Service->connId].obs.goep_buff_rawheader = KAL_TRUE;
        GOEP_Report("[goep] enable rawheader");
    }
    obexapphdlr_p->mru = register_server_req->buf_size;
    if(register_server_req->need_auth)
    {
#if BT_SECURITY == XA_ENABLED
        GOEP_RegisterServerSecurityRecord(Service, BSL_AUTHENTICATION_IN);
#endif
    }
    /// send register result back
    goep_send_register_cnf_msg( MSG_ID_BT_GOEPS_REGISTER_SERVER_CNF, GOES(servers)[Service->connId].obs.goep_conn_id, OB_STATUS_SUCCESS, register_server_req->uuid, register_server_req->uuid_len, register_server_req->req_id);
}

/*****************************************************************************
* FUNCTION
*  goep_deregister_server_req_hdlr
* DESCRIPTION
*  This function is to deregister the OBEX server application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_deregister_server_req_hdlr(goep_deregister_server_req_struct* deregister_server_req) //ilm_struct* ilm_ptr)
{
    ObStatus rsp_code;
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(deregister_server_req->goep_conn_id);

    if(Service == NULL)
    {
        //ASSERT(0);
        OS_Report("goep: goep_main ignore the wrong goep_conn_id %d", deregister_server_req->goep_conn_id);
        return;
    }
	rsp_code = GOEP_DeregisterServer(Service);
	if(rsp_code != OB_STATUS_SUCCESS)
	{
	    /* The failed result is not allowed */
	    //ASSERT(0);
	    OS_Report("goep: goep_main deregister %d", rsp_code);
	}
	else
	{
        if(Service->target[0]){
	       /// send deregister result back
	       ObexSdpDB_DeRegistration(SC_OBEX_FILE_TRANSFER);
	       goep_send_cnf_msg( MSG_ID_BT_GOEPS_DEREGISTER_SERVER_CNF, deregister_server_req->goep_conn_id, rsp_code, Service->target[0]->target, Service->target[0]->targetLen);
        }else{
           /// send deregister result back
           ObexSdpDB_DeRegistration(SC_OBEX_OBJECT_PUSH);
           goep_send_cnf_msg( MSG_ID_BT_GOEPS_DEREGISTER_SERVER_CNF, deregister_server_req->goep_conn_id, rsp_code, NULL, 0);
        }

        if(Service)
        {
#if BT_SECURITY == XA_ENABLED
            GOEP_UnregisterServerSecurityRecord(Service);
#endif
            if(Service->target[0])
            {
                if(Service->target[0]->target){
                    free_ctrl_buffer(Service->target[0]->target);
                    Service->target[0]->target = 0;
                }
                free_ctrl_buffer(Service->target[0]);
                Service->target[0] = 0;
            }
            free_ctrl_buffer(Service);
        }
	}
}
/*****************************************************************************
* FUNCTION
*  goep_connect_rsp_hdlr
* DESCRIPTION
*  This function is to handle the connect response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_connect_rsp_hdlr(goep_connect_rsp_struct* goep_connect_rsp)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_connect_rsp->goep_conn_id);
#if GOEP_AUTH == XA_ENABLED
    GoepServerObexCons *server;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
#endif

    if(Service != NULL)
    {
        /* Note that lack of MRU interface */
        if(goep_connect_rsp->rsp_code == GOEP_STATUS_SUCCESS)
        {
            GOEP_ServerAccept(Service, NULL);

#if GOEP_AUTH == XA_ENABLED

            if (Service->bAuthChallenged)
            {
                if(Service->target[0])
                {
                    uuid_len = Service->target[0]->targetLen;
                    btmtk_os_memcpy(uuid, Service->target[0]->target, uuid_len);
                }
                else
                {
                    uuid[0]=0;
                    uuid_len =0;
                }

                server = goep_find_ServerObexCons_with_goep_conn_id(goep_connect_rsp->goep_conn_id);

                /* send auth_ind msg */
                goep_send_auth_ind_msg(goep_connect_rsp->goep_conn_id,
					             server->currOp.challenge.realm,
								 server->currOp.challenge.realmLen,
								 uuid,
								 uuid_len);
		        return; /* connect response will be sent in auth_rsp hdlr */
            }
#endif
        }
        else
            GOEP_ServerAbort(Service, goep_connect_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_authorize_rsp_hdlr
* DESCRIPTION
*  This function is to handle the authorization response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_authorize_rsp_hdlr(goep_authorize_rsp_struct* goep_authorize_rsp ) //ilm_struct* ilm_ptr)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_authorize_rsp->goep_conn_id);

    if(Service != NULL)
    {
        GOEP_ServerAuthorizeResponse(Service, goep_authorize_rsp->rsp_code);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_push_rsp_hdlr
* DESCRIPTION
*  This function is to handle the push response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_push_rsp_hdlr(goep_push_rsp_struct* goep_push_rsp )//ilm_struct* ilm_ptr)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_push_rsp->goep_conn_id);
    ObexServerApp* ObexAppHandle;

    if(Service != NULL)
    {
        ObexAppHandle = &(GOES(servers)[Service->connId].obs);
        ObexAppHandle->goep_buff_wrote_len = 0;

#ifdef __BT_GOEPS_AUTO_ACK__
//LOGW("[opps] PUHS RSP: state[%i], index[%i]", ObexAppHandle->goep_buff_state, ObexAppHandle->goep_buff_index );
        // state == 1: internal acked => only need to free buffer
        if( ObexAppHandle->goep_buff_state == 1 ){

            ObexAppHandle->goep_buff_state = 0;
            return;
        }
        // state == 2: internal blocked => need to swap buffer and do ack
        else if( ObexAppHandle->goep_buff_state == 2 ){

            // swap buffer
            goep_switch_server_buffer( ObexAppHandle );
            ObexAppHandle->goep_buff_state = 1;

            // do ack
        }
#endif

        // send push response
        ObexAppHandle->goep_buff_wrote_len = 0;
        if(goep_push_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, goep_push_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_pull_rsp_hdlr
* DESCRIPTION
*  This function is to handle the pull response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_pull_rsp_hdlr(goep_pull_rsp_struct* goep_pull_rsp) //ilm_struct* ilm_ptr)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_pull_rsp->goep_conn_id);
    ObexServerApp* ObexAppHandle;

    if(Service != NULL)
    {
        ObexAppHandle = &(GOES(servers)[Service->connId].obs);
        ObexAppHandle->goep_buff_wrote_len = 0;
        ObexAppHandle->goep_buff_size = goep_pull_rsp->frag_len;

        if(goep_pull_rsp->pkt_type == GOEP_FIRST_PKT || goep_pull_rsp->pkt_type == GOEP_SINGLE_PKT)
        {
			GoepServerObexCons *server = &GOES(servers)[Service->connId];

            ObexAppHandle->server.objLen = goep_pull_rsp->total_obj_len;
			server->currOp.info.pushpull.objectLen = ObexAppHandle->server.objLen;

            if(goep_pull_rsp->rsp_code == GOEP_STATUS_SUCCESS)
                GOEP_ServerAccept(Service, NULL);

        }

        if(goep_pull_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, goep_pull_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_set_folder_rsp_hdlr
* DESCRIPTION
*  This function is to handle the set folder response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_set_folder_rsp_hdlr(goep_set_folder_rsp_struct* goep_set_folder_rsp) //ilm_struct* ilm_ptr)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_set_folder_rsp->goep_conn_id);

    if(Service != NULL)
    {
        if(goep_set_folder_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, goep_set_folder_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}


/*****************************************************************************
* FUNCTION
*  goep_action_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the set folder response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_action_rsp_hdlr(goep_action_rsp_struct* goep_action_rsp)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_action_rsp->goep_conn_id);

    if(Service != NULL)
    {
        if(goep_action_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, goep_action_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}


/*****************************************************************************
* FUNCTION
*  goep_abort_rsp_hdlr
* DESCRIPTION
*  This function is to handle the abort response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_abort_rsp_hdlr(goep_abort_rsp_struct* goep_abort_rsp) //ilm_struct* ilm_ptr)
{
    GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_abort_rsp->goep_conn_id);

    if(Service != NULL)
    {
        if(goep_abort_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, goep_abort_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_auth_req_hdlr
* DESCRIPTION
*  This function is to handle the authentication request from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_auth_req_hdlr(goep_auth_req_struct* goep_auth_req )
{
    ObexAuthChallenge* Challenge = get_ctrl_buffer(sizeof(ObexAuthChallenge));
    U8 *auth_passwd = get_ctrl_buffer(GOEP_MAX_PASSWD_SIZE);
    U8 *auth_realm = get_ctrl_buffer(GOEP_MAX_REALM_SIZE);
    ObStatus rsp_code;

    ASSERT((auth_passwd != NULL)&&(auth_realm != NULL)&&(Challenge != NULL));

    /* Initialize the dynamically allocated memory */
    btmtk_os_memset( (U8 *) Challenge, 0, sizeof(ObexAuthChallenge));
    btmtk_os_memset( (U8 *) auth_passwd, 0, GOEP_MAX_PASSWD_SIZE);
    btmtk_os_memset( (U8 *) auth_realm, 0, GOEP_MAX_REALM_SIZE);

    if((goep_auth_req->passwd[0] != 0) && (goep_auth_req->passwd_len != 0))
        btmtk_os_memcpy( auth_passwd, goep_auth_req->passwd, GOEP_MAX_PASSWD_SIZE);

    if((goep_auth_req->realm_str[0] != 0) && (goep_auth_req->realm_len != 0))
        btmtk_os_memcpy( auth_realm, goep_auth_req->realm_str, GOEP_MAX_REALM_SIZE);

    Challenge->options = 0;
    Challenge->realm = auth_realm;
    Challenge->realmLen = goep_auth_req->realm_len;

    if(goep_auth_req->goep_role == GOEP_CLIENT_ROLE)
    {
        GoepClientApp *Client = goep_find_client_with_goep_conn_id(goep_auth_req->goep_conn_id);
        if(Client != NULL)
        {
            Client->connect_req.challenge = Challenge;
            Client->connect_req.password = auth_passwd;
            Client->connect_req.passwordLen = goep_auth_req->passwd_len;
            rsp_code = GOEP_Connect(Client, &(Client->connect_req));
            Client->connect_req.challenge = NULL;
        }
    }
    else
    {
        GoepServerApp *Service = goep_find_server_with_goep_conn_id(goep_auth_req->goep_conn_id);

        if(Service != NULL)
        {
            Service->challenge = Challenge;
            Service->password = auth_passwd;
            Service->passwordLen = goep_auth_req->passwd_len;
            rsp_code = GOEP_ServerAuthenticate(Service, NULL, Challenge);
            //OS_Report(("goep_auth_req_hdlr: GOEP_ServerAuthenticate status %d\n", rsp_code));
            rsp_code = GOEP_ServerContinue(Service);
            //OS_Report(("goep_auth_req_hdlr: GOEP_ServerContinue status %d\n", rsp_code));
            Service->challenge = NULL;
        }
        else
        {
            /* return failed result */
            goep_send_auth_cnf_msg(goep_auth_req->goep_conn_id, OB_STATUS_FAILED, goep_auth_req->uuid, goep_auth_req->uuid_len);
        }
     }
     /* free memory */
     free_ctrl_buffer(auth_realm);
     free_ctrl_buffer(Challenge);
}

/*****************************************************************************
* FUNCTION
*  goep_connect_req_hdlr
* DESCRIPTION
*  This function is to handle the connect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_connect_req_hdlr(goep_connect_req_struct* goep_connect_req)
{
    GoepClientApp *Client = get_ctrl_buffer(sizeof(GoepClientApp));
    ObexClientApp* ObexApp;
    ObexTpAddr  obtpaddr;
    ObStatus rsp_code;
    int result;
#if BT_GOEP_V2 == XA_ENABLED 
    U8 i;
#endif 
    ASSERT(Client != NULL);

    btmtk_os_memset( (U8 *) Client, 0, sizeof(GoepClientApp));

    if(goep_connect_req->tp_type == GOEP_TP_BT)
    {
#if BT_GOEP_V2 == XA_ENABLED
        /* Init the L2cap enhance buffer */
        Client->target_type = goep_connect_req->tptype;
        // TODO: remove this
        Client->target_type = GOEP_TP_RFCOMM_ONLY; // for Debugging
        switch( Client->target_type ){
            case GOEP_TP_L2CAP_ONLY:
            case GOEP_TP_BOTH:
                {
                    // construct l2cap enhance buffer
                    for(i = 0; i< GOEPL2_MAX_RETRANS_NUM; i++){
                        Client->pl2capRawData[i] = get_ctrl_buffer(L2CAP_MAXIMUM_MTU+20);
                        ASSERT( Client->pl2capRawData[i] != NULL );
						OS_Report("[GOEP] create Client:0x%x pl2capRawData:0x%x", &Client, Client->pl2capRawData[i]);
                    }
                    Client->l2capRawDataNum = GOEPL2_MAX_RETRANS_NUM;
                }
                break;
            default:
                Client->target_type = GOEP_TP_RFCOMM_ONLY;
            break;

        }
#endif

        /* Client Registration */
        /* No existing connection can be re-used */
        /* the exact connection id will be assigned in GOEP_RegisterServer() */

        if(goep_connect_req->uuid_len != 0)
        {
            Client->type = GOEP_PROFILE_FTP;
        }
		else
		{
            Client->type = GOEP_PROFILE_OPUSH;
		}

        Client->connFlags = GOEP_NEW_CONN;
        Client->connId = 0;
        Client->cm_conn_id = GOEP_INVALID_CMID;

        rsp_code = GOEP_RegisterClient(Client, NULL);

        if(rsp_code != OB_STATUS_SUCCESS)
        {
            goto RETURN_ERROR_CODE;
        }

        /* Client Connect */

        obtpaddr.type = OBEX_TP_BLUETOOTH;
        result = goep_find_bt_uuid_with_obex_uuid( &(obtpaddr.proto.bt.uuid), goep_connect_req->uuid);
        if(result < 0)
        {
            rsp_code = XA_STATUS_INVALID_PARM;
            goto RETURN_ERROR_CODE;
        }
        goep_bd_addr_stuct2array(&(goep_connect_req->bd_addr), &(obtpaddr.proto.bt.addr));

        if(goep_connect_req->uuid_len != 0)
        {
            Client->connect_req.target = get_ctrl_buffer(goep_connect_req->uuid_len + 1);
            btmtk_os_memcpy(Client->connect_req.target, goep_connect_req->uuid, goep_connect_req->uuid_len);
            Client->connect_req.target[goep_connect_req->uuid_len] = 0; /* NULL terminated */
            Client->connect_req.targetLen = goep_connect_req->uuid_len;
        }
        else
        {
            Client->connect_req.target = NULL;
            Client->connect_req.targetLen = 0;
        }

        ObexApp = &(GOEC(clients)[Client->connId].obc);
        ObexApp->goep_buff_ptr = goep_connect_req->buf_ptr;
        ObexApp->req_id = goep_connect_req->req_id;
        ObexApp->goep_auth_use = goep_connect_req->auth_use;
        Client->connect_req.mru = goep_connect_req->buf_size;

        /* Support continous CONNID */
        if(goep_connect_req->uuid_len != 0){
          	ObexApp->client.idFlag = 1;
	    }

        /* Connect to transport layer first */
        rsp_code = GOEP_TpConnect(Client, &obtpaddr);

        if(rsp_code == OB_STATUS_SUCCESS)
        {
            /* Connect to OBEX layer */
 #if OBEX_AUTHENTICATION == XA_ENABLED
            if(goep_connect_req->auth_use)
            {
                /* Since the client needs authentication,
                wait for auth request to give authentication information */
                goep_send_connect_cnf_msg(ObexApp->req_id, ObexApp->goep_conn_id, OB_STATUS_PENDING, 0, goep_connect_req->uuid, goep_connect_req->uuid_len);
                //OS_Report(("goep_connect_req_hdlr: Pending to wait for authenticate request\n"));
                return;
            }
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

            rsp_code = GOEP_Connect(Client, &(Client->connect_req));

            if (rsp_code != OB_STATUS_PENDING)
            {
                goto RETURN_ERROR_CODE;
            }
        }
        else if(rsp_code == OB_STATUS_PENDING)
        {
            /* Wait for transport layer establishment */
        }
        else
        {
            goto RETURN_ERROR_CODE;
        }
    }
    else
    {
        /* error handling */
        ASSERT(0);
    }
    return;

RETURN_ERROR_CODE:

    goep_send_connect_cnf_msg(goep_connect_req->req_id, 0xFF, rsp_code, 0, goep_connect_req->uuid, goep_connect_req->uuid_len);
    return;
}

/*****************************************************************************
* FUNCTION
*  goep_push_req_hdlr
* DESCRIPTION
*  This function is to handle the push request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_push_req_hdlr(goep_push_req_struct* goep_push_req ) //ilm_struct* ilm_ptr)
{
    GoepClientApp* client = goep_find_client_with_goep_conn_id(goep_push_req->goep_conn_id);
    ObexClientApp* ObexApp;
    GoepObjectReq object;
    ObStatus rsp_code;
    kal_uint8 uuid[GOEP_MAX_UUID_SIZE];
    kal_uint8 uuid_len;

    if(client != NULL)
    {
        ObexApp = &(GOEC(clients)[client->connId].obc);
        btmtk_os_memset( (U8 *) &object, 0, sizeof(GoepObjectReq));

#ifdef __BT_GOEPC_AUTO_ACK__

        //LOGW( "[oppc] PUSH_REQ: pkt_type[%i], state[%i], index[%i]", goep_push_req->pkt_type, ObexApp->goep_buff_state, ObexApp->goep_buff_index );

        ObexApp->req_pkt_type = goep_push_req->pkt_type;
        if( goep_push_req->pkt_type == GOEP_FIRST_PKT )
        {
            // init parameters
            ObexApp->goep_buff_state = 0;
            ObexApp->goep_buff_index = 0;
            ObexApp->goep_buff_next_ptr = 0;
            ObexApp->goep_buff_next_size = 0;
        }
        else if( goep_push_req->pkt_type == GOEP_NORMAL_PKT || goep_push_req->pkt_type == GOEP_FINAL_PKT )
        {
            // keep data buffer pointer and size from external
            ObexApp->goep_buff_next_ptr = goep_push_req->frag_ptr;  // pointer to the start point of shared memory
            ObexApp->goep_buff_next_size = goep_push_req->frag_len; // data size from external layer

            // update data pointer (data filled by external)
            goep_push_req->frag_ptr += (OPPC_MAX_OBEX_PACKET_LENGTH * ObexApp->goep_buff_index);

            // 0: buffer is available and can send CNF to external and let it prepare next block of data
            if( goep_push_req->pkt_type == GOEP_NORMAL_PKT && ObexApp->goep_buff_state == 0 )
            {
                // auto ack => send CNF to external and let it prepare next data block
                goep_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
                goep_send_push_cnf_msg2(ObexApp, goep_push_req->goep_conn_id, OB_STATUS_SUCCESS, (U8)OB_STATUS_SUCCESS, uuid, uuid_len);
            }

            // 1: early CNF is sent to external but next REQ is not received
            // 2: early CNF is sent and next REQ is received => no real CNF received => REQ and CNF cannot be sent.
            ObexApp->goep_buff_state++;

            // CNF for previous REQ is not received => cannot send current REQ ( it should be true in most case )
            if( ObexApp->goep_buff_state != 1 )
            {
                return;
            }
        }
#endif

        if(goep_push_req->pkt_type == GOEP_FIRST_PKT || GOEP_SINGLE_PKT == goep_push_req->pkt_type)
        {
            if(goep_push_req->put_type == GOEP_PUT_DELETE || GOEP_SINGLE_PKT == goep_push_req->pkt_type )
            {
                object.object = NULL;
            }
            else
            {
                object.object = (void*)1;
            }

            if((goep_push_req->obj_name[0] != 0)||(goep_push_req->obj_name[1] != 0))
            {
                /* This packet is the firsk packet of the  */
                /* Transfer to utf-8 first */
                object.name = get_ctrl_buffer(GOEP_MAX_OBJ_NAME);
                bt_ucs2ncpy( (S8 *)object.name, (const S8 *) goep_push_req->obj_name, GOEP_MAX_OBJ_NAME/sizeof(U16));
            }
            else
            {
                object.name = NULL;
            }

            if(goep_push_req->obj_mime_type[0] != 0)
            {
                object.type = get_ctrl_buffer(GOEP_MAX_MIME_TYPE);
                bt_strncpy((S8 *)object.type, (const S8 *)goep_push_req->obj_mime_type, GOEP_MAX_MIME_TYPE);
            }
            else
                object.type = NULL;

            if(goep_push_req->total_obj_len)
            {
                object.objectLen = goep_push_req->total_obj_len;
            }
            else
            {
                object.objectLen = 0;
            }
            ObexApp->goep_buff_wrote_len = 0;
            rsp_code = GOEP_Push(client, &object);

            if(object.name)
                free_ctrl_buffer(object.name);

            if(object.type)
                free_ctrl_buffer(object.type);

            if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
            {
                goep_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
                goep_send_push_cnf_msg(goep_push_req->goep_conn_id, rsp_code, (U8)rsp_code, uuid, uuid_len);
                return;
            }
        }
        else    // GOEP_NORMAL_PKT or GOEP_FINAL_PKT
        {
            ObexApp->goep_buff_ptr = goep_push_req->frag_ptr;
            ObexApp->goep_buff_size = goep_push_req->frag_len;
            ObexApp->goep_buff_wrote_len = 0;
            GOEP_ClientContinue(client);
        }
    }
}

/*****************************************************************************
* FUNCTION
*  goep_pull_req_hdlr
* DESCRIPTION
*  This function is to handle the pull request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_pull_req_hdlr(goep_pull_req_struct* goep_pull_req) //ilm_struct* ilm_ptr)
{
    GoepClientApp* client = goep_find_client_with_goep_conn_id(goep_pull_req->goep_conn_id);
    ObexClientApp* ObexApp;
    GoepObjectReq object;
    ObStatus rsp_code;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
	//U8 l_obj_name[GOEP_MAX_OBJ_NAME];
	//U8 l_mime_type[GOEP_MAX_MIME_TYPE];

    if(client != NULL)
    {
        ObexApp = &(GOEC(clients)[client->connId].obc);
        btmtk_os_memset( (U8 *) &object, 0, sizeof(GoepObjectReq));
        ObexApp->goep_buff_wrote_len = 0;
        if(goep_pull_req->pkt_type == GOEP_FIRST_PKT)
        {
            /* This packet is the firsk packet of the  */
            if((goep_pull_req->obj_name[0] != 0)||(goep_pull_req->obj_name[1] != 0))
            {
                /* Transfer to utf-8 first */
                object.name = get_ctrl_buffer(GOEP_MAX_OBJ_NAME);
                bt_ucs2ncpy( (S8*) object.name, (const S8 *) goep_pull_req->obj_name, GOEP_MAX_OBJ_NAME/sizeof(U16));
            }
            else
                object.name = NULL;

            if(goep_pull_req->obj_mime_type[0] != 0)
            {
                object.type = get_ctrl_buffer(GOEP_MAX_MIME_TYPE);
                bt_strncpy((char*)object.type, (char*)goep_pull_req->obj_mime_type, GOEP_MAX_MIME_TYPE);
            }
            else
                object.type = NULL;

            ObexApp->goep_first_packet_received = KAL_FALSE;

            rsp_code = GOEP_Pull(client, &object, KAL_FALSE);

            if(object.name)
                free_ctrl_buffer(object.name);

            if(object.type)
                free_ctrl_buffer(object.type);

            if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
            {
                goep_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
                goep_send_pull_cnf_msg(goep_pull_req->goep_conn_id, rsp_code, (U8)rsp_code, 0, NULL, 0, NULL, 0, uuid, uuid_len);
                return;
            }
        }
        else
        {
            GOEP_ClientContinue(client);
        }
    }else{
        // pull fail cannot find the connection
        goep_send_pull_cnf_msg(goep_pull_req->goep_conn_id, OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, 0, NULL, 0, NULL, 0, goep_pull_req->uuid, goep_pull_req->uuid_len);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_set_folder_req_hdlr
* DESCRIPTION
*  This function is to handle the set folder request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_set_folder_req_hdlr(goep_set_folder_req_struct* goep_set_folder_req) //ilm_struct* ilm_ptr)
{
    GoepClientApp* client = goep_find_client_with_goep_conn_id(goep_set_folder_req->goep_conn_id);
    GoepFolderReq   folder;
    ObStatus rsp_code = OB_STATUS_INVALID_PARM;

    if(client != NULL)
    {
        switch(goep_set_folder_req->setpath_flag)
        {
            case GOEP_FORWARD_FOLDER:
            folder.name = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
            ASSERT(folder.name != NULL);
            bt_ucs2ncpy( (S8*)folder.name, (const S8*)goep_set_folder_req->folder_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
            folder.reset = FALSE;
            folder.flags = OSPF_DONT_CREATE;
            break;

            case GOEP_CREATE_FOLDER:
            folder.name = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
            ASSERT(folder.name != NULL);
            bt_ucs2ncpy( (S8*)folder.name, (const S8*)goep_set_folder_req->folder_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
            folder.reset = FALSE;
            folder.flags = OSPF_NONE;
            break;

            case GOEP_BACK_FOLDER:
            folder.name = 0;
            folder.reset = FALSE;
            folder.flags = (OSPF_DONT_CREATE|OSPF_BACKUP);
            break;

            case GOEP_ROOT_FOLDER:
            folder.name = 0;
            folder.reset = TRUE;
            folder.flags = OSPF_DONT_CREATE;
            break;

        }

        rsp_code = GOEP_SetFolder(client, &folder);

        if(folder.name)
            free_ctrl_buffer(folder.name);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            goep_send_set_folder_cnf_msg(goep_set_folder_req->goep_conn_id, rsp_code, (U8)rsp_code, (U8*)goep_set_folder_req->uuid, goep_set_folder_req->uuid_len);
            return;
        }
    }else{
        // set path but cannot find the connection
        goep_send_set_folder_cnf_msg(goep_set_folder_req->goep_conn_id, (U8)OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, (U8*)goep_set_folder_req->uuid , goep_set_folder_req->uuid_len);
    }

}

/*****************************************************************************
* FUNCTION
*  goep_action_req_hdlr 
* DESCRIPTION
*  This function is to handle the set folder request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_action_req_hdlr(goep_action_req_struct* goep_action_req) 
{
    GoepClientApp* client = goep_find_client_with_goep_conn_id(goep_action_req->goep_conn_id);
    GoepActionReq  action;
    ObStatus rsp_code = OB_STATUS_INVALID_PARM;

    if(client != NULL)
    {
        action.dstname = NULL;            
        action.name = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
        ASSERT(action.name != NULL);
        action.id = goep_action_req->action_id;
        action.permission = goep_action_req->permission;
        bt_ucs2ncpy( (S8*)action.name, (const S8*)goep_action_req->folder_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
        
        if( OBAP_SETPARAMISSION != action.id ){
            action.dstname = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
            bt_ucs2ncpy( (S8*)action.dstname, (const S8*)goep_action_req->dst_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
        }

        rsp_code = GOEP_PerformAction(client, &action);    

        if(action.name)
            free_ctrl_buffer(action.name);
        if(action.dstname)
            free_ctrl_buffer(action.dstname);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            goep_send_action_cnf_msg(goep_action_req->goep_conn_id, rsp_code, (U8)rsp_code, (U8*)goep_action_req->uuid, goep_action_req->uuid_len);
            return;
        }
    }else{
        // set path but cannot find the connection
        //TODO log
        goep_send_action_cnf_msg(goep_action_req->goep_conn_id, (U8)OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, (U8*)goep_action_req->uuid , goep_action_req->uuid_len);
    }

}

/*****************************************************************************
* FUNCTION
*  goep_abort_req_hdlr
* DESCRIPTION
*  This function is to handle the abort request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_abort_req_hdlr(goep_abort_req_struct* goep_abort_req )
{
    GoepClientApp* client = goep_find_client_with_goep_conn_id(goep_abort_req->goep_conn_id);
    ObStatus rsp_code;

    if(client != NULL)
    {
        rsp_code = GOEP_ClientAbort(client);

        if((rsp_code != OB_STATUS_SUCCESS) && (rsp_code != OB_STATUS_PENDING))
        {
            kal_trace(BT_TRACE_G6_OBEX,GOEP_ABORTREQFAIL_SEND_ABORT_RSP);
            goep_send_abort_cnf_msg(goep_abort_req->goep_conn_id, rsp_code, client->connect_req.target, client->connect_req.targetLen);
            return;
        }
    }else{
        goep_send_abort_cnf_msg(goep_abort_req->goep_conn_id, OBRC_INTERNAL_SERVER_ERR, goep_abort_req->uuid, goep_abort_req->uuid_len);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_disconnect_req_hdlr
* DESCRIPTION
*  This function is to handle the disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_disconnect_req_hdlr(goep_disconnect_req_struct* goep_disconnect_req)
{
    GoepServerApp *Service;
    GoepClientApp* client;
    ObStatus rsp_code;
    ObStatus status;

    if(goep_disconnect_req->goep_conn_id == 0xFF)
    {
        client = goep_find_client_with_req_id(goep_disconnect_req->req_id);
        if(client == NULL){
        	OS_Report("[GOEP] goep_disconnect_req_hdlr fail to find GoepclientApp goep_conn_id:%d", goep_disconnect_req->goep_conn_id );
	    goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, goep_disconnect_req->uuid, goep_disconnect_req->uuid_len, OBRC_INTERNAL_SERVER_ERR); //TODO NO SUCH CONNECTION
            return;
	}
		OS_Report("[GOEP] before GOEP API connState:%d", client->connState);
        if(client->connState == CS_CONNECTING)
        {
            rsp_code = GOEP_TpDisconnect(client);
            if(rsp_code == OB_STATUS_PENDING)
            {
               client->connState = CS_DISCONNECTING;
            }
        }
        else
        {
            rsp_code = GOEP_Disconnect(client);
            if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
            {
                rsp_code = GOEP_TpDisconnect(client);
                if(rsp_code == OB_STATUS_PENDING)
                {
                    client->connState = CS_DISCONNECTING;
                }
            }
        }
        OS_Report("[GOEP] after GOEP API connState:%d", client->connState);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            /* if disconnect request fails,  call RF_CloseChannel directly -- added by yfchu */
            GoepClientObexCons *clientObex = &GOEC(clients)[client->connId];
            ObBtClientTransport *btxp = &(clientObex->obc.trans.ObexClientBtTrans);

            kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_RFCLOSECHANNEL);

            status = RF_CloseChannel(&btxp->client.channel);
            btxp->client.state = OCS_DISCONNECTING;

            client->connState = CS_DISCONNECTING;

            if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS))
            {
                kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_RFCLOSECHANNEL_FAILED, status);
                goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, client->connect_req.target, client->connect_req.targetLen, (U8) rsp_code);
            }
            return;
        }
    }
    else
    {
        Service = goep_find_server_with_goep_conn_id(goep_disconnect_req->goep_conn_id);
        if(Service != NULL)
        {
            rsp_code = GOEP_ServerTpDisconnect(Service);

            if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
            {
                if (Service->target[0] != NULL) /* OPP Server's target is NULL */
                {
                    goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, Service->target[0]->target, Service->target[0]->targetLen, (U8)rsp_code);
                }
                else
                {
                    goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, NULL, 0, (U8)rsp_code);
                }
                return;
            }
        }
        else
        {
            client = goep_find_client_with_goep_conn_id(goep_disconnect_req->goep_conn_id);
            if(client != NULL)
            {
                OS_Report("[GOEP] goep_disconnect_req_hdlr to call GOEP_Disconnect ");
                rsp_code = GOEP_Disconnect(client);
                if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
                {
                    /* if disconnect request fails,  call RF_CloseChannel directly -- added by yfchu */
                    GoepClientObexCons *clientObex = &GOEC(clients)[client->connId];
                    ObBtClientTransport *btxp = &(clientObex->obc.trans.ObexClientBtTrans);

					switch( btxp->client.state ){
						case OCS_DISCOVERY:
						case OCS_LINK_CONNECT:
                        case OCS_SERVICE_QUERY:
                        case OCS_CONNECTING: // unable to stop connecting
							{
							/// ignore and set up a flag to disconnect
							ObStatus l_status;
                            l_status = btxp->client.state;
                            kal_trace(BT_TRACE_G6_OBEX, GOEP_DISCONNECT_FLAG_SETUP, l_status);
							btxp->client.state = OCS_DISCONNECTING;
							}
							break;
						case OCS_DISCONNECTING:
                            OS_Report("[GOEP] ignore disconnect twice" );
							break;
					    default:
                            kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_RFCLOSECHANNEL);
                    status = RF_CloseChannel(&btxp->client.channel);

                    btxp->client.state = OCS_DISCONNECTING;

                    if ((status != BT_STATUS_PENDING) && (status != BT_STATUS_IN_PROGRESS))
                    {
                        kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_RFCLOSECHANNEL_FAILED, status);
                        goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, client->connect_req.target, client->connect_req.targetLen, (U8)rsp_code);
                    }
						break;
					}// switch

                    return;
                }
            }
            else
            {
                /* The disconnecting connection does not exist */
                /* It may be race condition, thus ignore it. */
                kal_trace(BT_TRACE_G6_OBEX,GOEP_MAIN_DISCONECT_AN_INEXISTENT_GOEP_CONN_ID_xD , goep_disconnect_req->goep_conn_id);
                goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, goep_disconnect_req->uuid, goep_disconnect_req->uuid_len, OBRC_INTERNAL_SERVER_ERR); //TODO NO SUCH CONNECTION
                return;
            }
        }
    }
}

/*****************************************************************************
* FUNCTION
*  goep_auth_rsp_hdlr
* DESCRIPTION
*  This function is to handle the authentication response from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_auth_rsp_hdlr(goep_auth_rsp_struct* goep_auth_rsp) //ilm_struct* ilm_ptr)
{
    ObexAuthResponse* Response = get_ctrl_buffer(sizeof(ObexAuthResponse));
    GoepServerApp *Service;
    GoepClientApp *client;
    ObStatus rsp_code;

    btmtk_os_memset( (U8 *)Response, 0, sizeof(ObexAuthResponse));

    if((goep_auth_rsp->passwd[0] != 0) && (goep_auth_rsp->passwd_len != 0))
        {
        Response->password = get_ctrl_buffer(sizeof(goep_auth_rsp->passwd));
        ASSERT(Response->password != NULL);
        btmtk_os_memcpy(Response->password, goep_auth_rsp->passwd, goep_auth_rsp->passwd_len);
        Response->passwordLen = goep_auth_rsp->passwd_len;
        }

    if((goep_auth_rsp->userid[0] != 0) && (goep_auth_rsp->userid_len != 0))
        {
        Response->userId = get_ctrl_buffer(sizeof(goep_auth_rsp->userid));
        ASSERT(Response->userId != NULL);
        btmtk_os_memcpy(Response->userId, goep_auth_rsp->userid, goep_auth_rsp->userid_len);
        Response->userIdLen = goep_auth_rsp->userid_len;
    }

    Service = goep_find_server_with_goep_conn_id(goep_auth_rsp->goep_conn_id);
    if(Service != NULL)
    {
        rsp_code = GOEP_ServerAuthenticate(Service, Response, NULL);
        OS_Report( "goep_auth_rsp_hdlr: GOEP_ServerAuthenticate status %d\n", rsp_code );
        rsp_code = GOEP_ServerContinue(Service);
        OS_Report( "goep_auth_rsp_hdlr: GOEP_ServerContinue status %d\n", rsp_code );
    }
    else
    {
        client = goep_find_client_with_goep_conn_id(goep_auth_rsp->goep_conn_id);
        if( NULL != goep_auth_rsp ){
            OS_Report( "goep_auth_rsp_hdlr: can not find client by goep_conn_id:%d", goep_auth_rsp->goep_conn_id );
        }else{
            OS_Report( "goep_auth_rsp_hdlr: can not find client by goep_conn_id " );
        }
        if(client != NULL)
        {
            client->connect_req.response = Response;

            rsp_code = GOEP_Connect(client, &(client->connect_req));
            client->connect_req.response = NULL;

        }
        else
        {
            /* No matched goep_conn_id, it shall be race condiction */
        }
    }
    if(Response->password)
    {
        free_ctrl_buffer(Response->password);
    }
    if(Response->userId)
    {
        free_ctrl_buffer(Response->userId);
    }
    if(Response)
    {
        free_ctrl_buffer(Response);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_TpDisconnect_req_hdlr
* DESCRIPTION
*  This function is to handle the TP disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_TpDisconnect_req_hdlr(goep_disconnect_req_struct* goep_disconnect_req) //ilm_struct* ilm_ptr)
{
    GoepClientApp* client;
    ObStatus rsp_code;

    OS_Report("[GOEP] +goep_TpDisconnect_req_hdlr ");
    client = goep_find_client_with_req_id(goep_disconnect_req->req_id);
    if(client == NULL){
		GOEP_Report("[GOEP] client is NULL");
		ASSERT(0);
        return;
	}

	if (client->connState == CS_DISCONNECTING)
	{
	    OS_Report(("[GOEP] client is already CS_DISCONNECTING!"));
	    return;
	}

    rsp_code = GOEP_TpDisconnect(client);

    if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
    {
        goep_send_disconnect_ind_msg(goep_disconnect_req->req_id, goep_disconnect_req->goep_conn_id, client->connect_req.target, client->connect_req.targetLen, (U8)rsp_code);
    }
}

/*****************************************************************************
* FUNCTION
*  goep_deinit
* DESCRIPTION
*  This function is to handle the panic from the lower layer to deinit GOEP context
* PARAMETERS
*  none
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void goep_deinit(void)
{
    GoepServerApp *Service;
    GoepClientApp *client;
    ObStatus rsp_code;
    int i;

    /* here only FTP & OPP server/client need to be deregisted */
    OS_Report("[GOEP] goep_deinit");

    /* Server Deinit */
    for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        Service = goep_find_server_with_goep_conn_id(i);
        if ((Service != NULL) && (Service->callback == NULL))
        {
	        rsp_code = GOEP_DeregisterServer(Service);
	        if(rsp_code != OB_STATUS_SUCCESS)
	        {
	            /* The failed result is not allowed */
                kal_trace(BT_TRACE_G6_OBEX,GOEP_DEINIT_SERVER_FAILED, rsp_code);
	        }
	        else
	        {
	            OS_Report("[GOEP] goep_deinit server i:%d", i);
                if(Service->target[0])
                {
#if 0 
                    // target are ref to global var
                    if(Service->target[0]->target){
                        free_ctrl_buffer(Service->target[0]->target);
                        Service->target[0]->target = 0; // avoid free twice
                    }
#endif                    
                    free_ctrl_buffer(Service->target[0]);
                    Service->target[0] = 0;
                }
                free_ctrl_buffer(Service);
            }
        }
    }
    /* Client Deinit */
    for (i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++)
    {
        client = goep_find_client_with_goep_conn_id(i);
        if ((client != NULL) && (client->callback == NULL))
        {
           rsp_code = GOEP_DeregisterClient(client);
	        if(rsp_code != OB_STATUS_SUCCESS)
	        {
	            /* The failed result is not allowed */
                kal_trace(BT_TRACE_G6_OBEX,GOEP_DEINIT_CLIENT_FAILED, rsp_code);
	        }
	        else
	        {
	            OS_Report("[GOEP] goep_deinit client i:%d", i);	        
                if(client->connect_req.target){
                    free_ctrl_buffer(client->connect_req.target);
                    client->connect_req.target = 0;
                }
                free_clientapp_ctrl_buffer(client);
	        }
        }
    }
}
#endif /* __BT_GOEP_PROFILE__ */
