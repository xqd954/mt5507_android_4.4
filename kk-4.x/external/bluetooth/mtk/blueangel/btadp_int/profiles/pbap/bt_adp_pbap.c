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
*  bt_adp_pbap.c
*
* Project:
* -------- 
*  
*
* Description:
* ------------
*  phonebook access profile
*
* Author:
* -------
*  Xueling Li
*
*============================================================================
*             HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Log$
 *
 * 09 22 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integrate bluetooth code from //ALPS_SW_PERSONAL/sh.lai/10YW1040OF_CB/ into //ALPS_SW/TRUNK/ALPS/.
*
* 
*
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
****************************************************************************/

#ifdef __BT_PBAP_PROFILE__
/***************************************************************************** 
* Include
*****************************************************************************/
#ifdef BTMTK_ON_WIN32
#include "windows.h"
#endif
#include "string.h"
#include "stdio.h"      /* Basic C I/O functions */
#include "bttypes.h"
#include "btconfig.h"
#include "bt_common.h"
#include "bt_feature.h"
#include "bt.h"
#include "bt_adp_fs.h"
#include "bt_adp_fs_ucs2.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_pbap_struct.h"
#include "bluetooth_pbap_message.h"
#include "bluetooth_struct.h"
#include "eventmgr.h"
#include "goep.h"               /* GOEP operations */
#include "obex.h"
#include "pbap.h"
#include "pbapobs.h"            /* pbap operations */
#include "bt_adp_pbap.h"        /* Basic data type */
#include "bt_adp_msg.h"

#ifdef BTMTK_ON_WISE
#define PBAP_ADP_ROOT_FOLDER     (L"c:\\@btmtk\\")
#define PBAP_ADP_WORK_FOLDER     (L"c:\\@btmtk\\profile")
#elif defined(BTMTK_ON_LINUX)
#define PBAP_ADP_ROOT_FOLDER     (L"@btmtk/")
#define PBAP_ADP_WORK_FOLDER     (L"@btmtk/profile")
#endif

typedef struct
{
    bt_pbap_bd_addr_struct bd_addr;
    U32 cm_conn_id;
    U8 pbap_dev_name[BT_PBAP_MAX_DEV_NAME_LEN];
    U8 err_code;
    U8 store_mod;     /*buffer or file*/
    U8 data_file[BT_PBAP_MAX_FILEPATH_NAME_LEN];
    U8* data_buffer;
    PbStatus current_op_status;
    U32 object_size;
    U16 newMissedCalls;
    U16 phonebookSize;
    U32 fs_offset;
    FHANDLE fh;
    U8 flag;
    U32 send_size;
    U32 lastFileSize;
} bt_pbap_context;

typedef struct /*_PbaServerData*/
{
    PbapServerSession server;   /* PBAP server */
    U8 state;
}PbaServerData;

typedef struct
{
    U8 * buf;
    U16  buf_size;
    kal_uint32 pbap_mru;
}
pbap_send_obj_struct;/*Xin add*/
pbap_send_obj_struct psend_obj;


/***************************************************************************** 
* Static Declaration
*****************************************************************************/
static U8 *pbap_ucs2_strcpy(U8 *strDestination, const U8 *strSource);
static U16 pbap_ucs2_swapcpy(U8 *strDestination, const U8 *strSource);
static void BTPBAPAdpActivateCnf(U8 cnf_result);
static void BTPBAPAdpDeactivateCnf(U8 cnf_result);
static void BTPBAPAdpSetPathInd(const U8 *name, U8 flags, BT_BOOL reset);
static void BTPBAPAdpReadPbListInd(U8 order,
								   U8 searchAttrib,
								   U8 *searchValue,
								   U8 searchLength,
								   U16 listStartOffset,
								   U16 maxListCount,
								   U8 *name);
static void BTPBAPAdpReadFolderInd(PbapVcardFilter filter,
								   U8 formate,
								   U16 maxListCount,
								   U16 listStartOffset,
								   U8 *name);
static void BTPBAPADPReadPBEntryInd(PbapVcardFilter filter, U8 format, U8 *name);
static void BTPBAPADPDisconnectInd(void);
static void BTPBAPAdpClientAuthInd(U8 auth_option);
static void BTPBAPAdpAbortInd(void);
static void BTPBAPAdpOperSuccessInd(void);
static void BTPBAPAdpConnectInd(U32 cm_conn_id, bt_pbap_bd_addr_struct *dev_bd_addr, U8 *dev_name);
static void BTPBAPAdpAuthorizeInd(bt_pbap_bd_addr_struct* dev_bd_addr, U8* dev_name);

static void bt_pbap_activate_req_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_deactivate_req_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_disconnect_req_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_read_pb_entry_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_read_pb_folder_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_read_pb_list_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_read_pb_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_set_pb_path_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_disconnect_req_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_connect_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_auth_rsp_hdlr(ilm_struct *ilm_ptr);
static void bt_pbap_authrize_rsp_hdlr(ilm_struct *ilm_ptr);

static BT_BOOL  bt_pbap_folder_init(void);

BT_BOOL PBA_ServerInit(BtSecurityMode secMode, U8  support_rep);
BT_BOOL PBA_ServerDeinit(void);

PbStatus bt_pbap_read_data_from_file(U8 *buff, U16 len);
PbStatus bt_pbap_read_data_from_buffer(U8 *buff, U16 len);

static void bt_adp_pbap_send_data_continue(void);


bt_pbap_context pbap_adp_cntx;
PbaServerData PbaServer;

#if OBEX_AUTHENTICATION == XA_ENABLED
PbapAuthInfo PBAP_AuthInfo;
U8 PBAP_Password[20];
U8 PBAP_UserId[20];
BT_BOOL PBAP_PasswordSet;
BT_BOOL PBAP_UserIdSet;
BT_BOOL PBPA_AuthInitialized = FALSE;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/***************************************************************************** 
* External Function
*****************************************************************************/

#define BT_ADP_PBAP_INIT
/*****************************************************************************
* FUNCTION
*  pbap_adp_init
* DESCRIPTION
*  
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void pbap_adp_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL ret;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG(BT_PBAP_INIT);	
    bt_pbap_cntx_init();
    ret =  bt_pbap_folder_init();
    ASSERT(ret);
    ret = PBAP_Init();
    ASSERT(ret);
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_deinit
* DESCRIPTION
*  
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void bt_pbap_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG(BT_PBAP_DEINIT);	
    bt_pbap_cntx_deinit();
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_cntx_init
* DESCRIPTION
*  
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void bt_pbap_cntx_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pbap_adp_cntx.err_code = FALSE;
    pbap_adp_cntx.cm_conn_id = PBAP_INVALID_COUNT;
    pbap_adp_cntx.fs_offset = 0;
    pbap_adp_cntx.current_op_status = PB_STATUS_FAILED;
    pbap_adp_cntx.object_size = 0;
    pbap_adp_cntx.data_buffer = NULL;  
    pbap_adp_cntx.store_mod  = PBAP_NA_MOD;
    OS_MemSet(pbap_adp_cntx.data_file, 0, BT_PBAP_MAX_FILEPATH_NAME_LEN);
    pbap_adp_cntx.newMissedCalls = PBAP_INVALID_COUNT;
    pbap_adp_cntx.phonebookSize = PBAP_INVALID_COUNT;
    pbap_adp_cntx.fh = FHANDLE_INVALID_VALUE;
    pbap_adp_cntx.flag = 0;
    pbap_adp_cntx.send_size = 0;
    pbap_adp_cntx.lastFileSize = 0;
    BT_PBAP_STATE_TRANS(PBA_IDLE);
}

/*****************************************************************************
 * FUNCTION
 *  bt_pbap_folder_init
 * DESCRIPTION
 *  This function is to initialize PBAP  ADP work folder
 * PARAMETERS
 *  void
 * RETURNS
 *  kal_bool
 *****************************************************************************/
BT_BOOL bt_pbap_folder_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifdef BTMTK_ON_WISE
    FS_STATUS ret;
    BT_BOOL  status;
        
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   status = btmtk_fs_is_dir_exist_ucs2((U16*) PBAP_ADP_ROOT_FOLDER);
   if(status == FALSE)
   {
   	ret = btmtk_fs_create_dir_ucs2((U16*) PBAP_ADP_ROOT_FOLDER);
	if(ret < 0)
		return FALSE;
   }

  status = btmtk_fs_is_dir_exist_ucs2((U16*) PBAP_ADP_WORK_FOLDER);
   if(status == FALSE)
   {
   	ret = btmtk_fs_create_dir_ucs2((U16*) PBAP_ADP_WORK_FOLDER);
	if(ret < 0)
		return FALSE;
   }
#endif
   return TRUE;
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_cntx_deinit
* DESCRIPTION
*  pbap server context deinit
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void bt_pbap_cntx_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	bt_pbap_cntx_init();
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_activate_req_hdlr
* DESCRIPTION
*  pbap server activate request handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_activate_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL status;
    bt_pbap_register_req_struct *req = (bt_pbap_register_req_struct*) ilm_ptr->local_para_ptr;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check state first */
    if (PbaServer.state != PBA_IDLE)
    {
        /* wrong state */
       BT_PBAP_LOG1(BT_PBAP_ACTIVATE_WITH_WRONG_STATE, PbaServer.state);
       return;
    }
	
    status = PBA_ServerInit((BtSecurityMode)req->security_level, req->support_repositories);
	
    if (status == TRUE)
    {
        BT_PBAP_STATE_TRANS(PBA_ACTIVED);
        BTPBAPAdpActivateCnf(PBAP_CNF_SUCCESS);
        #if defined(BTMTK_ON_LINUX)
        bt_setProfileAddress(PROFILE_PBAP, &req->addr, req->addrlen);
        #endif        
    }
    else
    {
        BT_PBAP_STATE_TRANS(PBA_IDLE);
        BTPBAPAdpActivateCnf(PBAP_CNF_FAILED);		
        BT_PBAP_LOG(BT_PBAP_SERVER_ACTIVATE_FAIL);
    }
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_deactivate_req_hdlr
* DESCRIPTION
*  deactivate request handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_deactivate_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus  discnt_status;
    BT_BOOL  deacitve_status;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check state first */
     switch (PbaServer.state)
    {
    	case PBA_DEACTIVING:
		 return;
	case PBA_ACTIVED:
		/*deinitial derictly*/
		discnt_status = OB_STATUS_NO_CONNECT;
		break;	
	case PBA_AUTHRIZING:
		GOEP_ServerAuthorizeResponse(&(PbaServer.server.sApp), GOEP_STATUS_FAILED); 
		 /*pending*/
		discnt_status = OB_STATUS_PENDING;		 
		break;
	case PBA_AUTHRIZED:
	case PBA_CONNECTING:
	case PBA_CONNECTED:
	case PBA_BUILDING:
	case PBA_SENDING:
		/*disconect first*/
		 discnt_status = PBAP_ServerTpDisconnect(&(PbaServer.server));
		break;
	case PBA_DISCONNECTING:
              /*pending*/
		discnt_status = OB_STATUS_PENDING;
		break;

	case PBA_IDLE:
	default:
		discnt_status = OB_STATUS_INVALID_PARM;
		break;
    }
	

    if ((discnt_status == OB_STATUS_DISCONNECT) || (discnt_status == OB_STATUS_NO_CONNECT))
    {  
    	BT_PBAP_STATE_TRANS(PBA_ACTIVED);
    	deacitve_status = PBA_ServerDeinit();
	if (!deacitve_status)
	{
		BTPBAPAdpDeactivateCnf(PBAP_CNF_FAILED);
		BT_PBAP_LOG(BT_PBAP_SERVER_DEACTIVATE_FAIL);
	}
	else
	{
        	BT_PBAP_STATE_TRANS(PBA_IDLE);
        	BTPBAPAdpDeactivateCnf(PBAP_CNF_SUCCESS);
	}
    }
    else if(discnt_status == OB_STATUS_PENDING)
    {
    	 BT_PBAP_STATE_TRANS(PBA_DEACTIVING);
    }
    else if(discnt_status == OB_STATUS_INVALID_PARM)
    {
    	BTPBAPAdpDeactivateCnf(PBAP_CNF_SUCCESS);
    }  
	
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_connect_rsp_hdlr
* DESCRIPTION
*  connect rsponse handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_connect_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_client_connect_rsp_struct *rsp = (bt_pbap_client_connect_rsp_struct*) ilm_ptr->local_para_ptr;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG(BT_PBAP_FUNC_CONNECT_RSP_HDLR);
	
    /* check state first */
    if (PbaServer.state != PBA_CONNECTING)
    {
        /* wrong state */
        BT_PBAP_STATE_LOG(PbaServer.state);
    }
    else
    {
        if (rsp->cnf_code == PBAP_CNF_FAILED)
        {
            BT_PBAP_STATE_TRANS(PBA_ACTIVED);
            PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        }
        else
        {
            BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        }		
        PBAP_ServerContinue(&(PbaServer.server));
    }
	
}

#define BT_ADP_PBAP_HANDLER
/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_handle_message
* DESCRIPTION
*  This function is pbap adp message handler
* PARAMETERS
*  ilm_ptr     [?]     
*  ilm_srtuct*(?)
* RETURNS
*  void
*****************************************************************************/
void bt_adp_pbap_handle_message(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 msg_id = ilm_ptr->msg_id;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (msg_id)
    {
	case MSG_ID_BT_PBAP_REGISTER_REQ:   /* active request */
		bt_pbap_activate_req_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_DEREGISTER_REQ: /* deactive request */
		bt_pbap_deactivate_req_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_DISCONNECT_REQ:
		bt_pbap_disconnect_req_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_CLIENT_CONNECT_RSP:
		bt_pbap_connect_rsp_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_SET_PATH_RSP:
		bt_pbap_set_pb_path_rsp_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_READ_ENTRY_RSP:
		bt_pbap_read_pb_entry_rsp_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_READ_FOLDER_RSP:
		bt_pbap_read_pb_folder_rsp_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_READ_LIST_RSP:
		bt_pbap_read_pb_list_rsp_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_RSP:
		bt_pbap_auth_rsp_hdlr(ilm_ptr);
		break;
	case MSG_ID_BT_PBAP_AUTHORIZE_RSP:
		bt_pbap_authrize_rsp_hdlr(ilm_ptr);
		break;
	default:
		break;
    }
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_pb_entry_rsp_hdlr
* DESCRIPTION
*  read phonebook contact response handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_read_pb_entry_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_read_entry_rsp_struct *rsp = (bt_pbap_read_entry_rsp_struct*) ilm_ptr->local_para_ptr;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_LOG(PbaServer.state);
	
    if (rsp->result == PB_STATUS_SUCCESS && PbaServer.state == PBA_BUILDING)
    {
        BT_PBAP_STATE_TRANS(PBA_SENDING);
		
        pbap_adp_cntx.current_op_status = rsp->result;
	 pbap_adp_cntx.store_mod = PBAP_FILE_MOD;
       memcpy(pbap_adp_cntx.data_file, rsp->pbap_data_file, BT_PBAP_MAX_FILEPATH_NAME_LEN);
        pbap_adp_cntx.flag = 0;
		
       PBAP(serverParms).event = PBAP_EVENT_PRE_DATA_REQ;
       PBAP(serverCallback)(&PBAP(serverParms));
    }
    else
    {
        bt_adp_pbap_release_object();            
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        PBAP_ServerAbort(&(PbaServer.server), PBRC_NOT_FOUND);
        PBAP_ServerContinue(&(PbaServer.server));
    }
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_pb_folder_rsp_hdlr
* DESCRIPTION
*  read phonebook folder response handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_read_pb_folder_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_pbap_read_pb_rsp_hdlr(ilm_ptr);
	
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_pb_list_rsp_hdlr
* DESCRIPTION
*  read phonebook list rsp handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_read_pb_list_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_pbap_read_pb_rsp_hdlr(ilm_ptr);
}



/*****************************************************************************
* FUNCTION
*  
* DESCRIPTION
*  
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_read_pb_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_read_rsp_struct *rsp = (bt_pbap_read_rsp_struct*) ilm_ptr->local_para_ptr;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_LOG(PbaServer.state);
	
    if (rsp->result == PB_STATUS_SUCCESS && PbaServer.state == PBA_BUILDING)
    {
        BT_PBAP_STATE_TRANS(PBA_SENDING);
		
        pbap_adp_cntx.current_op_status = rsp->result;
        pbap_adp_cntx.newMissedCalls = rsp->newMissedCalls;   /* 0xffff, if not mch.vcf */
        pbap_adp_cntx.phonebookSize =  rsp->phoneBookSize;  /* 0xffff, if not request phonebook size */
        pbap_adp_cntx.flag = rsp->cont;
		
        if (rsp->phoneBookSize != PBAP_INVALID_COUNT)
        {
            PBAP(serverParms).event = PBAP_EVENT_PROVIDE_PARAMS;
            PBAP(serverCallback)(&PBAP(serverParms));
        }
        else
        {
            if (pbap_adp_cntx.newMissedCalls != PBAP_INVALID_COUNT)
            {
                bt_adp_pbap_provide_mcalls();
            }
	     pbap_adp_cntx.store_mod = PBAP_FILE_MOD;
            memcpy(pbap_adp_cntx.data_file, rsp->pbap_data_file, BT_PBAP_MAX_FILEPATH_NAME_LEN);
            PBAP(serverParms).event = PBAP_EVENT_PRE_DATA_REQ;
            PBAP(serverCallback)(&PBAP(serverParms));
        }
    }
    else
    {
	bt_adp_pbap_release_object();            
	BT_PBAP_STATE_TRANS(PBA_CONNECTED);
	PBAP_ServerAbort(&(PbaServer.server), PBRC_NOT_FOUND);
	PBAP_ServerContinue(&(PbaServer.server));
    }
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_set_pb_path_rsp_hdlr
* DESCRIPTION
*  set path rsponse handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_set_pb_path_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_set_path_rsp_struct *rsp = (bt_pbap_set_path_rsp_struct*) ilm_ptr->local_para_ptr;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_LOG(PbaServer.state);
    if (PbaServer.state == PBA_BUILDING)
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        if (rsp->result !=  PBAP_CNF_SUCCESS)
        {    
            PBAP_ServerAbort(&(PbaServer.server), PBRC_NOT_FOUND);   
            PBAP_ServerContinue(&(PbaServer.server));
        }
        else
        {            
            PBAP_ServerContinue(&(PbaServer.server));
        }
    }    
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_disconnect_req_hdlr
* DESCRIPTION
*  disconnect request handler
* PARAMETERS
*  ilm_ptr     [?]     
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_disconnect_req_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_LOG(PbaServer.state);
    switch (PbaServer.state)
    {
	case PBA_ACTIVED:
		return;
		/* wrong state */
	case PBA_IDLE:
	case PBA_AUTHRIZING:
	case PBA_AUTHRIZED:
	case PBA_CONNECTING:
	case PBA_DISCONNECTING:
		BT_PBAP_LOG1(BT_PBAP_RECEIVE_DISCONNECT_IN_WRONG_STATE, PbaServer.state);
		return;
		
	case PBA_CONNECTED:
	case PBA_BUILDING:
	case PBA_SENDING:
		BT_PBAP_STATE_TRANS(PBA_DISCONNECTING);
		break;
	default:
		//ASSERT(0);
		return;
    }
	
    status = PBAP_ServerTpDisconnect(&(PbaServer.server));
    if ((status == OB_STATUS_DISCONNECT) || (status == OB_STATUS_NO_CONNECT))
    {   
        /* send disconnect confirm to MMI */
        BTPBAPADPDisconnectInd();
	 BT_PBAP_STATE_TRANS(PBA_ACTIVED);
    }   
    return;
	
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_auth_rsp_hdlr
* DESCRIPTION
*  auth confirm
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_pbap_auth_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_client_auth_challenge_rsp_struct *rsp = (bt_pbap_client_auth_challenge_rsp_struct*) ilm_ptr->local_para_ptr;
    ObStatus result;
    U16 len;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!rsp->cancel)
    {
	len = OS_StrLen((const char *)rsp->password);
	 OS_MemSet(PbaServer.server.password, 0, 20);
	if(len>0)	  
        {
	     memcpy(PbaServer.server.password, rsp->password, len);
	} 
		
	len = OS_StrLen((const char *)rsp->userId);
	 OS_MemSet( PbaServer.server.userId, 0, 20);
	if(len>0)
        {
  
            memcpy( PbaServer.server.userId, rsp->userId, len);
        }
				
	 PbaServer.server.resp.password = PbaServer.server.password;
        PbaServer.server.resp.passwordLen =(U8)OS_StrLen((const char *)(PbaServer.server.password));		
		
        PbaServer.server.resp.userId = PbaServer.server.userId;
        PbaServer.server.resp.userIdLen = (U8)OS_StrLen((const char *)(PbaServer.server.userId));
        result = PBAP_ServerAuthCheck(&(PbaServer.server), 0);
    }
    else
    {
        result = PBAP_ServerAuthCheck(&(PbaServer.server), 1);
    }
	
    if (result == OB_STATUS_SUCCESS)
    {
     	BTPBAPAdpConnectInd(pbap_adp_cntx.cm_conn_id, &(pbap_adp_cntx.bd_addr), pbap_adp_cntx.pbap_dev_name);
	BT_PBAP_STATE_TRANS(PBA_CONNECTING);
    }
    else
    {
        PBAP_ServerContinue(&(PbaServer.server));
	 BT_PBAP_STATE_TRANS(PBA_ACTIVED);
    }
}

static void bt_pbap_authrize_rsp_hdlr(ilm_struct *ilm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_authorize_rsp_struct *rsp = (bt_pbap_authorize_rsp_struct*) ilm_ptr->local_para_ptr;
    U8 rsp_code;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/	
    /* check state first */
    if (PbaServer.state != PBA_AUTHRIZING)
    {
        /* wrong state */
        BT_PBAP_STATE_LOG(PbaServer.state);
        /* This is a possible scenario */
	kal_trace(BT_TRACE_G6_OBEX,BT_PBAP_ADP_AUTHORIZE_ERROR_STATE);
    }
    else
    {
        if(rsp->cnf_code == PBAP_CNF_SUCCESS)
        {
            rsp_code = GOEP_STATUS_SUCCESS;
            BT_PBAP_STATE_TRANS(PBA_AUTHRIZED);
        }
        else
        {
            rsp_code = GOEP_STATUS_FAILED;
            BT_PBAP_STATE_TRANS(PBA_ACTIVED);
        }
        GOEP_ServerAuthorizeResponse(&(PbaServer.server.sApp), rsp_code); 
    }
	
}

#define BT_ADP_PBAP_ACTIVE
/*****************************************************************************
* FUNCTION
*  BTPBAPAdpActivateCnf
* DESCRIPTION
*  pbap server activate confirm handler
* PARAMETERS
*  cnf_result      [IN]        
* RETURNS
*  void
*****************************************************************************/
static void BTPBAPAdpActivateCnf(U8 cnf_result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_register_cnf_struct *cnf =
        (bt_pbap_register_cnf_struct*) construct_local_para(sizeof(bt_pbap_register_cnf_struct), TD_RESET);
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cnf->register_result = cnf_result;
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_REGISTER_CNF, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)cnf, NULL);
}


/*****************************************************************************
* FUNCTION
*  BTPBAPAdpDeactivateCnf
* DESCRIPTION
*  pbap server deactivate confirm function
* PARAMETERS
*  cnf_result      [IN]        
*  confirm result(?)
* RETURNS
*  void
*****************************************************************************/
static void BTPBAPAdpDeactivateCnf(U8 cnf_result)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_deregister_cnf_struct *cnf =
        (bt_pbap_deregister_cnf_struct*) construct_local_para(sizeof(bt_pbap_deregister_cnf_struct), TD_RESET);
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    cnf->deregister_result = cnf_result;    
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_DEREGISTER_CNF, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)cnf, NULL);
}

#define BT_ADP_PBAP_IND
/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_abort_ind  BTPBAPAdpAbortInd
* DESCRIPTION
*  auth indication
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpClientAuthInd(U8 auth_option)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
   U16 len;
    bt_pbap_client_auth_challenge_ind_struct *ind =
        (bt_pbap_client_auth_challenge_ind_struct*) construct_local_para(sizeof(bt_pbap_client_auth_challenge_ind_struct), TD_RESET);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    len = OS_StrLen((const char *)pbap_adp_cntx.pbap_dev_name);
    if(len >  BT_PBAP_MAX_DEV_NAME_LEN)
    {
    	len = BT_PBAP_MAX_DEV_NAME_LEN;
    }
	
    memcpy((char *)ind->dev_name, (U8 *)pbap_adp_cntx.pbap_dev_name, len);
    ind->auth_option = auth_option;
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind, NULL);
}


/*****************************************************************************
* FUNCTION
*  BTPBAPAdpAuthorizeInd
* DESCRIPTION
*  This function is to send authorize indication to MMI
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpAuthorizeInd(bt_pbap_bd_addr_struct* dev_bd_addr, U8* dev_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/   
    U16 len;
    bt_pbap_authorize_ind_struct* ind = (bt_pbap_authorize_ind_struct*) construct_local_para(sizeof(bt_pbap_authorize_ind_struct), TD_RESET); 
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	
    ind->bd_addr.lap =  dev_bd_addr->lap;
    ind->bd_addr.nap =  dev_bd_addr->nap;
    ind->bd_addr.uap =  dev_bd_addr->uap;	
    len = OS_StrLen((const char *)dev_name);
    if(len >  BT_PBAP_MAX_DEV_NAME_LEN)
    {
    	len = BT_PBAP_MAX_DEV_NAME_LEN;
    }
    memcpy(ind->dev_name, dev_name, len);    
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_AUTHORIZE_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind, NULL);
}


/*****************************************************************************
* FUNCTION
*  BTPBAPAdpAbortInd
* DESCRIPTION
*  abort indication
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpAbortInd(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_abort_ind_struct *ind =
		(bt_pbap_abort_ind_struct*) construct_local_para(sizeof(bt_pbap_abort_ind_struct), TD_RESET);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_TRANS(PBA_CONNECTED);
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_ABORT_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind,NULL);
}
/*****************************************************************************
* FUNCTION
*  BTPBAPAdpAbortInd
* DESCRIPTION
*  abort indication
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpOperSuccessInd(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_oper_success_ind_struct *ind =
		(bt_pbap_oper_success_ind_struct*) construct_local_para(sizeof(bt_pbap_oper_success_ind_struct), TD_RESET);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_OPER_SUCCESS_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind,NULL);
}


/*****************************************************************************
* FUNCTION
*  BTPBAPADPDisconnectInd
* DESCRIPTION
*  disconnect indication
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void BTPBAPADPDisconnectInd(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_disconnect_ind_struct *ind =
        (bt_pbap_disconnect_ind_struct*) construct_local_para(sizeof(bt_pbap_disconnect_ind_struct), TD_RESET);
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/	
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_DISCONNECT_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind,NULL);
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_connect_ind  BTPBAPAdpConnectInd
* DESCRIPTION
*  connect indication
* PARAMETERS
*  cm_conn_id      [IN]        
*  dev_bd_addr     [?]         
*  dev_name        [?]         
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpConnectInd(U32 cm_conn_id, bt_pbap_bd_addr_struct *dev_bd_addr, U8 *dev_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
   U16 len;
    bt_pbap_client_connect_ind_struct *ind =
        (bt_pbap_client_connect_ind_struct*) construct_local_para(sizeof(bt_pbap_client_connect_ind_struct), TD_RESET);
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_TRANS(PBA_CONNECTING);
	
    ind->cm_conn_id = cm_conn_id;
    ind->bd_addr.lap =  dev_bd_addr->lap;
    ind->bd_addr.nap =  dev_bd_addr->nap;
    ind->bd_addr.uap =  dev_bd_addr->uap;
    len = OS_StrLen((const char *)dev_name);
    if(len >  BT_PBAP_MAX_DEV_NAME_LEN)
    {
    	len = BT_PBAP_MAX_DEV_NAME_LEN;
    }
    OS_MemCopy(ind->dev_name, dev_name, (U32)len);    
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_CLIENT_CONNECT_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind,NULL);
	
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_set_pb_path_ind BTPBAPAdpSetPathInd
* DESCRIPTION
*  set phonebook path indication
* PARAMETERS
*  name        [IN]        
*  flags       [IN]        
*  reset       [IN]        
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpSetPathInd(const U8 *name, U8 flags, BT_BOOL reset)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_set_path_ind_struct *ind;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG1(BT_ADP_PBAP_SET_PATH_IND, PbaServer.state);
    if (PbaServer.state == PBA_CONNECTED)
    {
        PbaServer.state = PBA_BUILDING;
    }
    else
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        PBAP_ServerContinue(&(PbaServer.server));
        return;
    }
	
    ind = (bt_pbap_set_path_ind_struct*) construct_local_para(sizeof(bt_pbap_set_path_ind_struct), TD_RESET);
	
    if (!(flags & OSPF_DONT_CREATE))
    {
        //error report
    }
	
    /*set the coresponding flag according to the info provided by obex*/
    ind->name[0] = 0;
    ind->name[1] = 0;
	
    if (reset)
    {
        ind->type = PBAP_ROOT_FOLDER;
    }
    else if (flags & OSPF_BACKUP)
    {
        ind->type = PBAP_BACK_FOLDER;
    }
    else 
    {
        if (name[0] == 0 && name[1] == 0)
        {
            ind->type = PBAP_ROOT_FOLDER;
        }
        else
        {
            ind->type = PBAP_FORWARD_FOLDER;
            pbap_ucs2_swapcpy((U8 *) ind->name, (U8 *) name);
            //pbap_ucs2_strcpy((U8 *) ind->name, (U8 *) name);	   
        }
    }
    bt_prompt_trace(MOD_BT, "[PBAP] Set path ind : %s", name);
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_SET_PATH_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind, NULL);
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_pb_list_ind BTPBAPAdpReadPbListInd
* DESCRIPTION
*  read phonebook list indication
* PARAMETERS
*  order               [IN]        
*  searchAttrib        [IN]        
*  searchValue         [?]         
*  searchLength        [IN]        
*  listStartOffset     [IN]        
*  maxListCount        [IN]        
*  name                [?]         
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpReadPbListInd(U8 order,
							U8 searchAttrib,
							U8 *searchValue,
							U8 searchLength,
							U16 listStartOffset,
							U16 maxListCount,
							U8 *name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_read_list_ind_struct *ind;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG1(BT_ADP_PBAP_READ_LIST_IND, PbaServer.state);
    if (PbaServer.state == PBA_CONNECTED)
    {
        PbaServer.state = PBA_BUILDING;
    }
    else if (PbaServer.state == PBA_SENDING)
    {
        if(psend_obj.buf)
        {
	      free_ctrl_buffer(psend_obj.buf);
		  psend_obj.buf = NULL;
        }
        bt_adp_pbap_send_data_continue();

        return;
    }
    else        
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);        
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        PBAP_ServerContinue(&(PbaServer.server));
        return;
    }
	
    ind = (bt_pbap_read_list_ind_struct*) construct_local_para(sizeof(bt_pbap_read_list_ind_struct), TD_RESET);
	
    ind->order = order;
    ind->searchValueLength = searchLength;
    ind->listStartOffset = listStartOffset;
    ind->searchAttribute = searchAttrib;
    ind->maxListCount = maxListCount;
    pbap_ucs2_swapcpy((U8 *) ind->folderName, (U8 *) name);
    //pbap_ucs2_strcpy((U8 *) ind->folderName, (U8 *) name);
    OS_MemSet(ind->searchValue, 0, MAX_PBAP_SEARCH_VALUE_LENGTH + 1);
    if(searchValue != NULL)
    {
	memcpy((char*)ind->searchValue, (char*)searchValue, ind->searchValueLength);
     }
    bt_prompt_trace(MOD_BT, "[PBAP] list folder : %s", name);
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_READ_LIST_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind, NULL);
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_pb_folder_ind BTPBAPAdpReadFolderInd
* DESCRIPTION
*  read phonebook folder indication
* PARAMETERS
*  filter              [IN]        
*  formate             [IN]        
*  maxListCount        [IN]        
*  listStartOffset     [IN]        
*  name                [?]         
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpReadFolderInd(PbapVcardFilter filter,
							U8 formate,
							U16 maxListCount,
							U16 listStartOffset,
							U8 *name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_read_folder_ind_struct *ind;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(name != 0);
    BT_PBAP_LOG1(BT_ADP_PBAP_READ_FOLDER_IND, PbaServer.state);
    if (PbaServer.state == PBA_CONNECTED)
    {
        PbaServer.state = PBA_BUILDING;
    }
    else if (PbaServer.state == PBA_SENDING)
    {
        if(psend_obj.buf)
        {
	        free_ctrl_buffer(psend_obj.buf);
	        psend_obj.buf = NULL;
        }
        bt_adp_pbap_send_data_continue();
        return;
    }
    else
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        PBAP_ServerContinue(&(PbaServer.server));
        return;
    }
	
    ind = (bt_pbap_read_folder_ind_struct*) construct_local_para(sizeof(bt_pbap_read_folder_ind_struct), TD_RESET);
	
    memcpy((char*)ind->filter.byte, (char*)filter.byte, PBAP_FILTER_SIZE);
    ind->format = formate;
    ind->maxListCount = maxListCount;
    ind->listStartOffset = listStartOffset;

    pbap_ucs2_swapcpy((U8 *) ind->pbName, (U8 *) name);
    //pbap_ucs2_strcpy((U8 *) ind->pbName, (U8 *) name);
    bt_prompt_trace(MOD_BT, "[PBAP] read folder : %s", name);
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_READ_FOLDER_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind, NULL);
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_pb_entry_ind
* DESCRIPTION
*  read phonebook contact indication
* PARAMETERS
*  filter      [IN]        
*  format      [IN]        
*  name        [?]         
* RETURNS
*  void
*****************************************************************************/
void BTPBAPADPReadPBEntryInd(PbapVcardFilter filter, U8 format, U8 *name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_read_entry_ind_struct *ind;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(name != 0);
    BT_PBAP_LOG1(BT_ADP_PBAP_READ_ENTRY_IND, PbaServer.state);
    if (PbaServer.state == PBA_CONNECTED)
    {
        PbaServer.state = PBA_BUILDING;
    }
    else if (PbaServer.state == PBA_SENDING)
    {
        if(psend_obj.buf)
        {
	       free_ctrl_buffer(psend_obj.buf);
		   psend_obj.buf = NULL;
        }
        bt_adp_pbap_send_data_continue();
        return;
    }
    else
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        PBAP_ServerContinue(&(PbaServer.server));
        return;
    }
	
    ind = (bt_pbap_read_entry_ind_struct*) construct_local_para(sizeof(bt_pbap_read_entry_ind_struct), TD_RESET);
    ind->format = format;

    pbap_ucs2_swapcpy((U8 *) ind->objectName, (U8 *) name);
    //pbap_ucs2_strcpy((U8 *)ind->objectName, (U8 *)name);
    memcpy((char*)ind->filter.byte, (char*)filter.byte, PBAP_FILTER_SIZE);
    bt_prompt_trace(MOD_BT, "[PBAP] read entry : %s", name);
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_READ_ENTRY_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind,NULL);
}

/*****************************************************************************
* FUNCTION
*  BTPBAPAdpContinueInd
* DESCRIPTION
*  abort indication
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void BTPBAPAdpContinueInd(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_pbap_continue_ind_struct *ind =
		(bt_pbap_continue_ind_struct*) construct_local_para(sizeof(bt_pbap_continue_ind_struct), TD_RESET);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (PbaServer.state == PBA_SENDING)
    {
        BT_PBAP_STATE_TRANS(PBA_BUILDING); 
    }
    else        
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);        
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        PBAP_ServerContinue(&(PbaServer.server));
        return;
    }
    bt_adp_pbap_send_msg(MSG_ID_BT_PBAP_READ_CONTINUE_IND, MOD_EXTERNAL,
        BT_APP_SAP, (local_para_struct*)ind,NULL);
}

#define BT_ADP_PBAP_CORECODE

/*****************************************************************************
* FUNCTION
*  bt_pbap_read_vcard_entry
* DESCRIPTION
*  get vcard entry to obex buffer
* PARAMETERS
*  buff        [?]         
*  len         [IN]        
* RETURNS
*  void
*****************************************************************************/
PbStatus bt_pbap_read_data_from_file(U8 *buff, U16 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    //FHANDLE fh;
    S32 ret;
    S32 read_len;
    PbStatus result;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pbap_adp_cntx.current_op_status != PB_STATUS_SUCCESS)
    {
		return pbap_adp_cntx.current_op_status;
    }
    
    if ( pbap_adp_cntx.fh == FHANDLE_INVALID_VALUE )
    {
        pbap_adp_cntx.fh = btmtk_fs_open_ucs2((U16*)pbap_adp_cntx.data_file, BTMTK_FS_READ_ONLY);
    }
    
    if (pbap_adp_cntx.fh != FHANDLE_INVALID_VALUE)
    {
        btmtk_fs_seek_ucs2(pbap_adp_cntx.fh,  (S32)pbap_adp_cntx.fs_offset, BTMTK_FS_SEEK_BEGIN);
        ret = btmtk_fs_read_ucs2(pbap_adp_cntx.fh,  buff,  (S32)len,  (S32*)&read_len);
		if (ret < 0)
		{
			len = 0;
			result = PB_STATUS_FAILED;
		}
		else
		{
            pbap_adp_cntx.fs_offset += read_len;
            pbap_adp_cntx.send_size= pbap_adp_cntx.send_size - read_len;
			result = PB_STATUS_SUCCESS;
		}
        //btmtk_fs_close_ucs2(pbap_adp_cntx.fh);
	}
	else
	{
        len = 0;
        result = PB_STATUS_FAILED;
	}
    return result;
}


/*****************************************************************************
* FUNCTION
*  bt_pbap_read_vcard_entry
* DESCRIPTION
*  get vcard entry to obex buffer
* PARAMETERS
*  buff        [?]         
*  len         [IN]        
* RETURNS
*  void
*****************************************************************************/
PbStatus bt_pbap_read_data_from_buffer(U8 *buff, U16 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    PbStatus result;
    U32  left_len,read_len;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pbap_adp_cntx.current_op_status != PB_STATUS_SUCCESS)
    {
		return pbap_adp_cntx.current_op_status;
    }

   if(pbap_adp_cntx.fs_offset >  pbap_adp_cntx.object_size)
   {
   	result = PB_STATUS_FAILED;
	return result;
   }
   
  left_len = pbap_adp_cntx.object_size -pbap_adp_cntx.fs_offset ;
  if(len > left_len)
  	{
  		read_len = left_len;
  	}
  else
  	{
  		read_len = len;  	
  	}
  memcpy(buff,  pbap_adp_cntx.data_buffer  + pbap_adp_cntx.fs_offset , read_len);

  pbap_adp_cntx.fs_offset += read_len;
  result = PB_STATUS_SUCCESS;
  return result;
}

/*****************************************************************************
* FUNCTION
*  bt_pbap_get_object_length
* DESCRIPTION
*  object length of request
* PARAMETERS
*  name        [?]         
*  curr_op     [IN]        
* RETURNS
*  U32
*****************************************************************************/
U32 bt_pbap_get_object_length(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	FHANDLE fh;
       U32 size = 0;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if(pbap_adp_cntx.store_mod == PBAP_BUFFER_MOD)
    {
	size  =pbap_adp_cntx.object_size;
    }
    else 
    {
    	fh = btmtk_fs_open_ucs2((U16*)pbap_adp_cntx.data_file, BTMTK_FS_READ_ONLY);
	if (fh <= 0)
	{
		size = 0;
	}
	else
	{
            btmtk_fs_get_filesize_ucs2(fh, (U32*) &size);
            btmtk_fs_close_ucs2(fh);
	}
    }
    BT_PBAP_LOG1(BT_ADP_PBAP_GET_OBJECT_LENGTH, size);
    return size;
}

#define BT_ADP_PBAP_UTILS
/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_check_state
* DESCRIPTION
*  msg sender
* PARAMETERS
*      
* RETURNS
*  void
*****************************************************************************/
BT_BOOL bt_adp_pbap_check_state(PbapOp op)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BT_BOOL is_state_valid = TRUE;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG1(BT_ADP_PBAP_CHECK_STATE_VALID, PbaServer.state);
    switch (op)
    {
	case PBAPOP_NONE:
		break;
	case PBAPOP_CONNECT:
		if (PbaServer.state != PBA_AUTHRIZED /* PBA_ACTIVED */)
		{
			is_state_valid = FALSE;
		}
		break;
	case PBAPOP_DISCONNECT:
		break;
	case PBAPOP_SET_PHONEBOOK:
	case PBAPOP_PULL:
	case PBAPOP_PULL_PHONEBOOK:
	case PBAPOP_PULL_VCARD_ENTRY:
	case PBAPOP_PULL_VCARD_LISTING:
		if (PbaServer.state != PBA_CONNECTED)
		{
			is_state_valid = FALSE;
		}
		break;
	default:
		break;
    }
	
    return is_state_valid;
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_release_object
* DESCRIPTION
*  msg sender
* PARAMETERS
*      
* RETURNS
*  void
*****************************************************************************/
void bt_adp_pbap_release_object(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG(BT_ADP_PBAP_RELEASE_OBJECT);
    if (PbaServer.server.object)
    {
        PBAPOBS_Close(&PbaServer.server.object);
    }    
    pbap_adp_cntx.fs_offset = 0;
    if ( pbap_adp_cntx.fh != FHANDLE_INVALID_VALUE )
    {
        btmtk_fs_close_ucs2( pbap_adp_cntx.fh );
        pbap_adp_cntx.fh = FHANDLE_INVALID_VALUE;
    }
    pbap_adp_cntx.flag = 0;
    pbap_adp_cntx.send_size= 0;
    pbap_adp_cntx.lastFileSize = 0;
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_send_msg
* DESCRIPTION
*  msg sender
* PARAMETERS
*  msg_id              [IN]    msg id    
*  p_local_para        [IN]    msg local struct         
* RETURNS
*  void
*****************************************************************************/
void bt_adp_pbap_send_msg(msg_type msg,
						  module_type dstMod,
						  sap_type sap,
						  local_para_struct *local_para,
						  peer_buff_struct *peer_buff)
{	
    if (dstMod == 0)
    {
#ifdef MTK_BT_DEBUG_TRACE
        //kal_trace(BT_TRACE_G1_PROTOCOL, BTLOG_NOT_READY);
#endif 
        return;
    }
	
    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}


/****************************************************************************
* Internal Function prototypes
***************************************************************************/
static void  PbaServerCallback(PbapServerCallbackParms *Parms);
static void bt_adp_pbap_oper_start(PbapServerCallbackParms *Parms);
static void bt_adp_pbap_oper_continue(PbapServerCallbackParms *Parms);
#if OBEX_AUTHENTICATION == XA_ENABLED
static void bt_adp_pbap_auth_result(PbapServerCallbackParms *Parms);
#endif
static void bt_adp_pbap_provide_app_params(PbapServerCallbackParms *Parms);
static void bt_adp_pbap_send_data(PbapServerCallbackParms *Parms);
static void bt_adp_pbap_tp_disconnect(void);
static void bt_adp_pbap_oper_complete(PbapServerCallbackParms *Parms);

static void bt_adp_pbap_authrize_ind(PbapServerCallbackParms *Parms);
static void bt_adp_pbap_pre_send_data(PbapServerCallbackParms *Parms);
static void bt_adp_pbap_oper_abort(PbapServerCallbackParms *Parms);
#if OBEX_AUTHENTICATION == XA_ENABLED
static void AuthInit(void);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/***************************************************************************** 
* External Function
*****************************************************************************/
extern U8 BTBMProfileAllocateConnIndex(U32 uuid, BD_ADDR addr);
extern BT_BOOL BTBMProfileReleaseConnIndex(U8 index);
extern U8 *BTBMGetLocalStoredName(U8 *bdAddr);
extern void CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src);

#define PBAP_SERVER_ACTIVE_AND_DEACTIVE
/*****************************************************************************
* FUNCTION
*  PBA_ServerInit
* DESCRIPTION
* Synopsis:  Initialize the PBAP Server.
* PARAMETERS
*  
* RETURNS
*  BT_BOOL
*****************************************************************************/
BT_BOOL PBA_ServerInit(BtSecurityMode secMode, U8  support_rep)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    PbStatus            status;
#if BT_SECURITY == XA_ENABLED
    BtSecurityLevel     secLevel = BSL_AUTHENTICATION_IN;
#endif /* BT_SECURITY == XA_ENABLED */
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_MemSet((U8 *)&PbaServer, 0, sizeof(PbaServer));
	
#if BT_SECURITY == XA_ENABLED
    switch (secMode)
    {
	case BSM_SEC_DISABLED: /* 0x01 */
		/* No Security */
		secLevel = BSL_NO_SECURITY;
		break;
	case BSM_SEC_LEVEL_1:  /* 0x02 */
		/* Level 1 */
		secLevel = (BSL_AUTHENTICATION_IN | BSL_AUTHORIZATION_IN);
		break;
	case BSM_SEC_LEVEL_2:  /* 0x03 */
	case BSM_SEC_LEVEL_3:  /* 0x04 */
		/* Level 2 or 3 */
		secLevel = (BSL_AUTHENTICATION_IN | BSL_AUTHORIZATION_IN |\
			BSL_AUTHENTICATION_OUT | BSL_AUTHORIZATION_OUT);
		break;
	case BSM_SEC_ENCRYPT:  /* 0x05 */
		/* Level 3 + Encryption */
		secLevel = (BSL_AUTHENTICATION_IN | BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN|\
			BSL_AUTHENTICATION_OUT | BSL_AUTHORIZATION_OUT | BSL_ENCRYPTION_OUT);
		break;
    }    
    PbaServer.server.secLevel = secLevel;
    PbaServer.server.suppored_repositories = support_rep;
#endif /* BT_SECURITY == XA_ENABLED */
	
    status = (PbStatus)PBAP_RegisterServer(&(PbaServer.server), PbaServerCallback);
    ASSERT(status == OB_STATUS_SUCCESS);
	
#if OBEX_AUTHENTICATION == XA_ENABLED
    AuthInit();
    status = (PbStatus)PBAP_ServerSetAuthInfo(&(PbaServer.server), &PBAP_AuthInfo);
    ASSERT(status == OB_STATUS_SUCCESS);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
	
    return TRUE;
}


/*****************************************************************************
* FUNCTION
*  PBA_ServerDeinit
* DESCRIPTION
* Synopsis:  Deinitialize the PBAP Server.
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
BT_BOOL PBA_ServerDeinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if OBEX_DEINIT_FUNCS == XA_ENABLED
	PbStatus    status;
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if OBEX_DEINIT_FUNCS == XA_ENABLED
    if ((PbaServer.state >  PBA_ACTIVED) && (PbaServer.state !=  PBA_DEACTIVING) )
        return FALSE;
	
    /* Pbap Server */
    status = (PbStatus) PBAP_DeregisterServer(&(PbaServer.server));
    assert(status == OB_STATUS_SUCCESS);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */
	
    BT_PBAP_STATE_TRANS(PBA_IDLE);
	
    return TRUE;
}


#define PBAP_SERVER_PROCESS_HANDLE
#define PBAP_SERVER_AUTH_HANDLE
#if OBEX_AUTHENTICATION == XA_ENABLED

/*****************************************************************************
* FUNCTION
*  AuthInit
* DESCRIPTION
* Synopsis:  Initialize the OBEX Authentication information for the PBAP
*            Client and Server.
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
static void AuthInit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Only initialize these parameters once */
    if (PBPA_AuthInitialized == TRUE)
        return;
	
    /* No user-defined password, just a default */
    PBAP_PasswordSet = FALSE;
    /* No user-defined userId, just a default*/
    PBAP_UserIdSet = FALSE;
    /* Set default password*/
  
    PBAP_Password[0] = 'M'; 
    PBAP_Password[1] = 'y';
    PBAP_Password[2] = 'P'; 
    PBAP_Password[3] = 'a';
    PBAP_Password[4] = 's'; 
    PBAP_Password[5] = 's';
    PBAP_Password[6] = 'w'; 
    PBAP_Password[7] = 'o';
    PBAP_Password[8] = 'r'; 
    PBAP_Password[9] = 'd';
    PBAP_Password[10] = 0;  

    /* Set default userId */
    PBAP_UserId[0] = 'M'; 
    PBAP_UserId[1] = 'y'; 
    PBAP_UserId[2] = 'U'; 
    PBAP_UserId[3] = 's';
    PBAP_UserId[4] = 'e'; 
    PBAP_UserId[5] = 'r';
    PBAP_UserId[6] = 'I'; 
    PBAP_UserId[7] = 'd';
	
    for (i = 8; i < 20; i++)
    {
        PBAP_UserId[i] = 0;
    }
    PBAP_AuthInfo.password = PBAP_Password;
    PBAP_AuthInfo.passwordLen = 11;
    /* Set default userId */
    PBAP_AuthInfo.userId = PBAP_UserId;
    PBAP_AuthInfo.userIdLen = 9;
    /* Set default realm */
    PBAP_AuthInfo.realm = (const U8 *)"\0mtk-csj";
    PBAP_AuthInfo.realmLen = 8;
    
    /* Authentication parameters have been initialized */
    PBPA_AuthInitialized = TRUE;
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */


#define PBAP_SERVER_ADP_CALLBACK
/*****************************************************************************
* FUNCTION
*  PbaServerCallback
* DESCRIPTION
* Synopsis:  This function processes PBAP Server events.
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void PbaServerCallback(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_LOG3(BT_ADP_PBAP_CALL_BACK, Parms->event, Parms->oper, PbaServer.state);
    switch ( Parms->event ) 
    {
	case PBAP_EVENT_TP_CONNECTED:
		break;
        
	case PBAP_EVENT_TP_DISCONNECTED:
		bt_adp_pbap_tp_disconnect();
		break;
		
	case PBAP_EVENT_START:
		bt_adp_pbap_oper_start(Parms);
		break;
		
	case PBAP_EVENT_CONTINUE:
		bt_adp_pbap_oper_continue(Parms);
		break;
		
	case PBAP_EVENT_ABORTED:
		bt_adp_pbap_oper_abort(Parms);
		break;
		
	case PBAP_EVENT_PRECOMPLETE:
		break;
        
	case PBAP_EVENT_COMPLETE:
		bt_adp_pbap_oper_complete(Parms);
		break;
		
#if OBEX_AUTHENTICATION == XA_ENABLED
	case PBAP_EVENT_AUTH_RESULT:
		bt_adp_pbap_auth_result(Parms);
		break;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
		
	case PBAP_EVENT_PROVIDE_PARAMS:
		bt_adp_pbap_provide_app_params(Parms);
		break;
		
	case PBAP_EVENT_PRE_DATA_REQ:
		bt_adp_pbap_pre_send_data(Parms);
		break;
		
	case PBAP_EVENT_DATA_REQ:
		bt_adp_pbap_send_data(Parms);
		break;
	case PBAP_EVENT_AUTHRIZE_IND:
		bt_adp_pbap_authrize_ind(Parms);
		break;
		
	case PBAP_EVENT_PARAMS_RX:
	case PBAP_EVENT_OBJECT_LENGTH_REQ:
		break;
		
	default:
		break;
    }
}


#define PBAP_SERVER_CORN_FUNC
/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_oper_start
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_oper_start(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (Parms->oper)
    {
	case PBAPOP_PULL:
	case PBAPOP_PULL_PHONEBOOK:
	case PBAPOP_PULL_VCARD_ENTRY:
	case PBAPOP_PULL_VCARD_LISTING:
		if (!bt_adp_pbap_check_state(PBAPOP_PULL))
		{
			BT_PBAP_STATE_TRANS(PBA_CONNECTED);			
			bt_adp_pbap_release_object();
			PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
			PBAP_ServerContinue(&(PbaServer.server));
		}
		else
		{
			pbap_adp_cntx.fs_offset = 0;
            pbap_adp_cntx.send_size = 0;
            pbap_adp_cntx.lastFileSize = 0;
            pbap_adp_cntx.flag = 0;
            if ( pbap_adp_cntx.fh != FHANDLE_INVALID_VALUE )
            {
                btmtk_fs_close_ucs2( pbap_adp_cntx.fh );
                pbap_adp_cntx.fh = FHANDLE_INVALID_VALUE;
            }
		}
		break;
	default:
		break;
    }
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_oper_continue
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_oper_continue(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (Parms->oper)
    {
	case PBAPOP_CONNECT:
		if (bt_adp_pbap_check_state(PBAPOP_CONNECT))
		{
			ObexTpConnInfo      tpInfo;
			BT_BOOL            status;
			U8 *   dev_name = NULL;
			U16 len;
			
			status = GOEP_ServerGetTpConnInfo(&(PbaServer.server.sApp), &tpInfo);

				dev_name = (U8*)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr));
				CONVERT_ARRAY2BDADDR((btbm_bd_addr_t *)&(pbap_adp_cntx.bd_addr), tpInfo.remDev->bdAddr.addr);
				len = OS_StrLen((const char*)dev_name);
				if(len > BT_PBAP_MAX_DEV_NAME_LEN)
				{
					len = BT_PBAP_MAX_DEV_NAME_LEN;
				}
				OS_MemSet((U8*) pbap_adp_cntx.pbap_dev_name, 0, BT_PBAP_MAX_DEV_NAME_LEN);  
				memcpy((U8*) pbap_adp_cntx.pbap_dev_name, dev_name, len);
				pbap_adp_cntx.cm_conn_id = BTBMProfileAllocateConnIndex(SC_PBAP_SERVER, tpInfo.remDev->bdAddr);
			
#if OBEX_AUTHENTICATION == XA_ENABLED
				if (PbaServer.server.flags & PBAP_FLAGS_CHAL_RCVD)
				{
					BT_PBAP_STATE_TRANS(PBA_AUTH_CHAL_RECEIVED);
					BTPBAPAdpClientAuthInd(PbaServer.server.challengeInfo.options);
				}
				else
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
				{
                     	                 BTPBAPAdpConnectInd(pbap_adp_cntx.cm_conn_id, &(pbap_adp_cntx.bd_addr), pbap_adp_cntx.pbap_dev_name);
				}

		}
		else
		{
			BT_PBAP_STATE_TRANS(PBA_ACTIVED);
			PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
			PBAP_ServerContinue(&(PbaServer.server));
		}
		break;
		
	case PBAPOP_SET_PHONEBOOK:
		BTPBAPAdpSetPathInd(Parms->u.info.setPb.name,
							Parms->u.info.setPb.flags,
							Parms->u.info.setPb.reset);
		break;
		
	case PBAPOP_PULL_PHONEBOOK:
		BTPBAPAdpReadFolderInd(
			Parms->u.paramsRx.filter,
			Parms->u.paramsRx.format,
			Parms->u.paramsRx.maxListCount,
			Parms->u.paramsRx.listStartOffset,
			Parms->u.paramsRx.name);
		break;
		
	case PBAPOP_PULL_VCARD_LISTING:
		BTPBAPAdpReadPbListInd(
			Parms->u.paramsRx.order,
			Parms->u.paramsRx.searchAttrib,
			Parms->u.paramsRx.searchValue,
			Parms->u.paramsRx.searchValueLen,
			Parms->u.paramsRx.listStartOffset,
			Parms->u.paramsRx.maxListCount,
			Parms->u.paramsRx.name);
		break;
		
	case PBAPOP_PULL_VCARD_ENTRY:
		BTPBAPADPReadPBEntryInd(
			Parms->u.paramsRx.filter,
			Parms->u.paramsRx.format,
			Parms->u.paramsRx.name );
		break;
		
	default:
		PBAP_ServerContinue(&(PbaServer.server));
		break;
    }
}

static void bt_adp_pbap_oper_abort(PbapServerCallbackParms *Parms)
{
	if(PbaServer.state != PBA_DEACTIVING )
	{
        if(psend_obj.buf)
        {
            free_ctrl_buffer(psend_obj.buf);
            psend_obj.buf = NULL;
        }

        GOEP_ServerInitSendBuf(&PbaServer.server.sApp,NULL,0); 

		BTPBAPAdpAbortInd();
		pbap_adp_cntx.fs_offset = 0;
        pbap_adp_cntx.flag = 0;
        pbap_adp_cntx.send_size= 0;
        pbap_adp_cntx.lastFileSize = 0;
        if ( pbap_adp_cntx.fh != FHANDLE_INVALID_VALUE )
        {
            btmtk_fs_close_ucs2( pbap_adp_cntx.fh );
            pbap_adp_cntx.fh = FHANDLE_INVALID_VALUE;
        }
	}
}

#if OBEX_AUTHENTICATION == XA_ENABLED
/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_auth_result
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
void bt_adp_pbap_auth_result(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (Parms->u.auth.result)
    {
        ASSERT(Parms->u.auth.reason == PBAP_AUTH_NO_FAILURE);
    }
    else
    {
        if (Parms->u.auth.reason == PBAP_AUTH_NO_RCVD_AUTH_RESP)
        {   
        }
        else if (Parms->u.auth.reason == PBAP_AUTH_INVALID_CREDENTIALS)
        {    
        }
        else
        {
            ASSERT(0);
        }
    }
}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_provide_app_params
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_provide_app_params(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 index;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (Parms->oper)
    {
	case PBAPOP_PULL_PHONEBOOK:
	case PBAPOP_PULL_VCARD_LISTING:
		Parms->u.provideParams.phonebookSize = pbap_adp_cntx.phonebookSize;
		break;
	default:
		ASSERT(0);
		break;
    }
    
    Parms->u.provideParams.newMissedCalls =  pbap_adp_cntx.newMissedCalls;
    
    PbaServer.server.appParms[0] = 0;
    
    /* Skip past the length value */
    index = 1;
    
    if (PBAP(serverParms).u.paramsRx.maxListCount == 0)
    {
        PbaServer.server.appParms[index++] = PBAP_TAG_PHONEBOOK_SIZE;
        PbaServer.server.appParms[index++] = 2;
        StoreBE16(PbaServer.server.appParms + index, 
	  PBAP(serverParms).u.provideParams.phonebookSize);
        index += 2;
    }
    /* 0xffff is a invalid value */
    if (Parms->u.provideParams.newMissedCalls != PBAP_INVALID_COUNT)
    {
        PbaServer.server.appParms[index++] = PBAP_TAG_MISSED_CALLS;
        PbaServer.server.appParms[index++] = 1;
        PbaServer.server.appParms[index++] = (U8)PBAP(serverParms).u.provideParams.newMissedCalls;
    }
    
    if (index > 1) 
    {
        if (GOEP_ServerQueueHeader(&PbaServer.server.sApp, OBEXH_APP_PARAMS, 
			PbaServer.server.appParms + 1, (U16)(index - 1)) )
        {
            GOEP_ServerAcceptNoObject(&PbaServer.server.sApp);
        }
        else
        {
            BT_PBAP_STATE_TRANS(PBA_CONNECTED);
            bt_adp_pbap_release_object();
            PBAP_ServerAbort(&(PbaServer.server), PBRC_INTERNAL_SERVER_ERR);
        }
        PBAP_ServerContinue(&(PbaServer.server));
    }
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_provide_app_params
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
void bt_adp_pbap_provide_mcalls(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint16 index = 0;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	
    PbaServer.server.appParms[0] = 0;
    index = 1;
    PbaServer.server.appParms[index++] = PBAP_TAG_MISSED_CALLS;
    PbaServer.server.appParms[index++] = 1;
    PbaServer.server.appParms[index++] = (U8) pbap_adp_cntx.newMissedCalls;
	
    if (GOEP_ServerQueueHeader(&PbaServer.server.sApp, OBEXH_APP_PARAMS, 
		PbaServer.server.appParms + 1, (U16)(index - 1)) == FALSE)
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
		//    kal_printf("Server QH abort\n");
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(&(PbaServer.server), PBRC_INTERNAL_SERVER_ERR);
        PBAP_ServerContinue(&(PbaServer.server));
    }
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_send_data
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_pre_send_data(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 objectLen = 0;
    U16 plen;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if((Parms->u.paramsRx.name)!=NULL)
    {    
    PBAPOBS_AppendName( PbaServer.server.object,
        (const U8*)Parms->u.paramsRx.name, 
        OS_StrLen((char*)(Parms->u.paramsRx.name)) );
    }
    
     if((Parms->u.paramsRx.type)!=NULL)
    {   
    PBAPOBS_AppendType(PbaServer.server.object,
        (const U8*)Parms->u.paramsRx.type, 
        OS_StrLen((char*)(Parms->u.paramsRx.type)) );
     }
	
    /*set the object length*/
    objectLen =  bt_pbap_get_object_length();
    /*set data size*/
    pbap_adp_cntx.send_size= objectLen - pbap_adp_cntx.lastFileSize;
    pbap_adp_cntx.lastFileSize = objectLen;
    
    PBAPOBS_SetObjectLen(PbaServer.server.object, pbap_adp_cntx.send_size);
	
    GOEP_ServerAccept(&PbaServer.server.sApp, PbaServer.server.object);
    if(pbap_adp_cntx.newMissedCalls == PBAP_INVALID_COUNT )
    {
        bt_adp_pbap_send_data_continue();
    }
    else
    {
        kal_prompt_trace(MOD_BT,"[PBAP_ADP][header] init buffer,send empty data first");
        psend_obj.buf = NULL;
        psend_obj.pbap_mru = GOEP_ServerGetRemoteMRU(&PbaServer.server.sApp);
        psend_obj.buf_size = (U16)(psend_obj.pbap_mru > PBAP_MAX_PUSH_BUFF_LEN ? PBAP_MAX_PUSH_BUFF_LEN : psend_obj.pbap_mru);
        plen = (U16)(pbap_adp_cntx.send_size > psend_obj.buf_size ? psend_obj.buf_size : pbap_adp_cntx.send_size);
        psend_obj.buf_size = plen;

        if(psend_obj.buf_size)
        {
            psend_obj.buf = get_ctrl_buffer(psend_obj.buf_size);
        }

        GOEP_ServerInitSendBuf(&PbaServer.server.sApp,(kal_char*)psend_obj.buf,plen);
        PBAP_ServerContinue(&PbaServer.server);/*send MCH header first.*/
    }

}

/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_send_data_continue
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
* Xin add for test 2010.01.13
*****************************************************************************/
static void bt_adp_pbap_send_data_continue(void)
{
    U8* pbuf = NULL;
    U16 plen;
    PbStatus ret = PB_STATUS_SUCCESS;
    if(psend_obj.buf)	 
    {
        free_ctrl_buffer(psend_obj.buf);
        psend_obj.buf = NULL;
    }
    psend_obj.pbap_mru = GOEP_ServerGetRemoteMRU(&PbaServer.server.sApp);
    psend_obj.buf_size = (U16)(psend_obj.pbap_mru > PBAP_MAX_PUSH_BUFF_LEN ? PBAP_MAX_PUSH_BUFF_LEN : psend_obj.pbap_mru);
    plen = (U16)(pbap_adp_cntx.send_size > psend_obj.buf_size ? psend_obj.buf_size : pbap_adp_cntx.send_size);
    psend_obj.buf_size = plen;

    kal_prompt_trace(MOD_BT,"[PBAP_ADP]pbap send data size: %d",plen);

    if(psend_obj.buf_size)
    {
        psend_obj.buf = get_ctrl_buffer(psend_obj.buf_size);
        pbuf = psend_obj.buf;
        /*read data from temp file*/
        ret = bt_pbap_read_data_from_file(pbuf,plen);
    }

    if(ret == PB_STATUS_SUCCESS)
    {
        if((!plen)&&(pbap_adp_cntx.flag))	
        {
            //pbap_adp_cntx.fs_offset = 0;
            BTPBAPAdpContinueInd();
        }
        else
        {
            GOEP_ServerInitSendBuf(&PbaServer.server.sApp,(kal_char*)pbuf,plen);  
    PBAP_ServerContinue(&PbaServer.server);
}
    }
    else
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED); 
        if(pbuf)
        {
            free_ctrl_buffer(pbuf);
            pbuf = NULL;
        }
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(&(PbaServer.server), OBRC_CONFLICT);
        PBAP_ServerContinue(&(PbaServer.server));
    }
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_send_data
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_send_data(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObStatus status = PB_STATUS_SUCCESS;
    U8  store_mod  = pbap_adp_cntx.store_mod;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (Parms->oper)
    {
	case PBAPOP_PULL_PHONEBOOK:
	case PBAPOP_PULL_VCARD_LISTING:
	case PBAPOP_PULL_VCARD_ENTRY:
		{
			if(store_mod == PBAP_FILE_MOD)		
			status = bt_pbap_read_data_from_file(Parms->u.dataReq.buffer, Parms->u.dataReq.req.len);
			else
			status = bt_pbap_read_data_from_buffer(Parms->u.dataReq.buffer, Parms->u.dataReq.req.len);
		}
	break;
	default:
		ASSERT(0);
		break;
    }
    
    if (status != PB_STATUS_SUCCESS)
    {
        BT_PBAP_STATE_TRANS(PBA_CONNECTED);
        bt_adp_pbap_release_object();
        PBAP_ServerAbort(Parms->server, PBRC_NOT_FOUND);
    }
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_tp_disconnect
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_tp_disconnect(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   BTBMProfileReleaseConnIndex((U8) pbap_adp_cntx.cm_conn_id);
    pbap_adp_cntx.cm_conn_id = PBAP_INVALID_COUNT;
    pbap_adp_cntx.fs_offset = 0;
   if ( pbap_adp_cntx.fh != FHANDLE_INVALID_VALUE )
   {
       btmtk_fs_close_ucs2( pbap_adp_cntx.fh );
       pbap_adp_cntx.fh = FHANDLE_INVALID_VALUE;
   }
   if(PbaServer.state == PBA_DEACTIVING)
   {
   	BT_BOOL deacitve_status;
   	deacitve_status = PBA_ServerDeinit();
	if (!deacitve_status)
	{
		BTPBAPAdpDeactivateCnf(PBAP_CNF_FAILED);
		BT_PBAP_LOG(BT_PBAP_SERVER_DEACTIVATE_FAIL);
	}
	else
	{
        	BT_PBAP_STATE_TRANS(PBA_IDLE);
        	BTPBAPAdpDeactivateCnf(PBAP_CNF_SUCCESS);
	}
   }
   else
   {
       BTPBAPADPDisconnectInd();
	BT_PBAP_STATE_TRANS(PBA_ACTIVED);
   }
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_oper_complete
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_oper_complete(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_LOG(PbaServer.state);

    if(psend_obj.buf)
    {
        free_ctrl_buffer(psend_obj.buf);
        psend_obj.buf = NULL;
    }
    
    switch (Parms->oper)
    {
	case PBAPOP_DISCONNECT:
		BT_PBAP_STATE_TRANS(PBA_ACTIVED);
		break;
	case PBAPOP_PULL:
	case PBAPOP_PULL_PHONEBOOK:
	case PBAPOP_PULL_VCARD_ENTRY:
	case PBAPOP_PULL_VCARD_LISTING:
        GOEP_ServerInitSendBuf(&PbaServer.server.sApp,NULL,0); 
		BTPBAPAdpOperSuccessInd();
		BT_PBAP_STATE_TRANS(PBA_CONNECTED);
		pbap_adp_cntx.fs_offset = 0;
        if ( pbap_adp_cntx.fh != FHANDLE_INVALID_VALUE )
        {
            btmtk_fs_close_ucs2( pbap_adp_cntx.fh );
            pbap_adp_cntx.fh = FHANDLE_INVALID_VALUE;
        }
		break;
	default:
		break;
    }
    Parms->oper = PBAPOP_NONE;
}


/*****************************************************************************
* FUNCTION
*  bt_adp_pbap_authrize_ind
* DESCRIPTION
* 
* PARAMETERS
*  
* RETURNS
*  void
*****************************************************************************/
static void bt_adp_pbap_authrize_ind(PbapServerCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ObexTpConnInfo tpInfo;
    BT_BOOL status;
    U8 * dev_name = NULL;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_PBAP_STATE_LOG(PbaServer.state);
    status = GOEP_ServerGetTpConnInfo(&(Parms->server->sApp), &tpInfo);
     dev_name = (U8*)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr));
    CONVERT_ARRAY2BDADDR((btbm_bd_addr_t *)&(pbap_adp_cntx.bd_addr), tpInfo.remDev->bdAddr.addr);
    
       BT_PBAP_STATE_TRANS(PBA_AUTHRIZING);
       BTPBAPAdpAuthorizeInd(&(pbap_adp_cntx.bd_addr), dev_name);
  
}

#define PBAP_SERVER_ADP_UTILS
/*****************************************************************************
* FUNCTION
*  pbap_ucs2_strcpy
* DESCRIPTION
*  The function is used for copy UCS2 string. 
* PARAMETERS
*  strDestination  [OUT]   UCS2 destination string
*  strSource       [IN]    UCS2 source string
* RETURNS
*  Return the destination string.
*****************************************************************************/
static U8 *pbap_ucs2_strcpy(U8 *strDestination, const U8 *strSource)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	
    U16 count = 1;
    U8 *temp = strDestination;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (strSource == NULL)
    {
        if (strDestination)
        {
            *(strDestination + count - 1) = '\0';
            *(strDestination + count) = '\0';
        }
        return temp;		
    }
	
    if (strDestination == NULL || strSource == NULL)
    {
        return NULL;
    }
    while (!((*(strSource + count) == 0) && (*(strSource + count - 1) == 0)))
    {
		
        *(strDestination + count - 1) = *(strSource + count - 1);
        *(strDestination + count) = *(strSource + count);
        count += 2;
    }
	
    *(strDestination + count - 1) = '\0';
    *(strDestination + count) = '\0';
	
    return temp;
}

static U16 pbap_ucs2_swapcpy(U8 *strDestination, const U8 *strSource)
{
    U16 i  = 0;
    bt_prompt_trace(MOD_BT, "pbap_ucs2_swapcpy(%p, %p)", strDestination, strSource);
    if(strDestination == NULL)
    {
        return 0;
    }
    if(strSource == NULL)
    {
           strDestination[0] = strDestination[1] = 0;
    }
    else
    {
        while(strSource[i] + strSource[i+1])
        {
            //LOGI("pbap_ucs2_swapcpy loop i=%d", i);
            strDestination[i] = strSource[i+1];
            strDestination[i+1] = strSource[i];
            i+=2;
        }
    }
    bt_prompt_trace(MOD_BT,"pbap_ucs2_swapcpy return %d", i);
    return i;
}
#endif
