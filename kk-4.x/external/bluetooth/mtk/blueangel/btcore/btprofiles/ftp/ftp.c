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
 * bt_opps.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth MMI OPP client/server
 *
 * Author:
 * -------
 * Daylong Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *

 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__)

#include "bttypes.h"
#include "bt_os_api.h"
#include "l2cap.h"
#include "bluetooth_struct.h"
#include "ftp.h"
#include "sdpdb.h"
#ifdef __BT_GOEP_V2__
#include "goep_main.h"
#include "goepext.h"
#include "ftpobs.h"
#include "bt_adp_fs.h"
#endif
#include "bt_ucs2.h"

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef GOEP_Report
#undef GOEP_Report
#define GOEP_Report
#endif
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

// Disable SRM mode when a2dp is connected. when using ftp+a2dp, the a2dp doesn't smooth
#define _NO_SRM_WHEN_HEAVY_ 1

static int ftp_find_obex_uuid_with_bt_uuid(U32* bt_uuid_p, U8* obex_uuid_str, U8* obex_uuid_len);
extern void free_clientapp_ctrl_buffer(GoepClientApp *client);

#ifdef __BT_GOEP_V2__
#else
static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };

extern void goep_event_handler(U32 msg_id, void* data);
extern U8 ObexSdpDB_Registration(U32 service_type);

BTFTPCallback g_ftp_callback = NULL;

void btmtk_ftp_register(BTFTPCallback Callback){
    g_ftp_callback = Callback;
}

void ftp_handle_message(U32 msg_id, void* data){
     S32 id = msg_id;
     goep_disconnect_req_struct *disc_req;

     if( MSG_ID_BT_FTPC_DISCONNECT_REQ == id || MSG_ID_BT_GOEPC_DISCONNECT_REQ == id  ){
        disc_req = (goep_disconnect_req_struct *)data ;
        if( (disc_req->uuid_len != sizeof(OBEX_FTP_UUID) ) || (0 != btmtk_os_memcmp( disc_req->uuid, sizeof(OBEX_FTP_UUID), OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID) )) ){
            btmtk_os_memcpy( disc_req->uuid, OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));
            disc_req->uuid_len = sizeof(OBEX_FTP_UUID);
        }
     }
     if( MSG_ID_BT_FTPS_TPDISCONNECT_REQ == id || MSG_ID_BT_GOEPS_TPDISCONNECT_REQ == id ){
        disc_req = (goep_disconnect_req_struct *)data ;
        if( (disc_req->uuid_len != sizeof(OBEX_FTP_UUID) ) || (0 != btmtk_os_memcmp( disc_req->uuid, sizeof(OBEX_FTP_UUID), OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID) )) ){
            btmtk_os_memcpy( disc_req->uuid, OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));
            disc_req->uuid_len = sizeof(OBEX_FTP_UUID);
        }
     }
        
     if( id >= MSG_ID_BT_FTPC_GROUP_START && id <= MSG_ID_BT_FTPC_GROUP_END){
        id = id - (MSG_ID_BT_FTPC_GROUP_START - MSG_ID_BT_GOEPC_GROUP_START);
     }else if( id >= MSG_ID_BT_FTPS_GROUP_START && id <= MSG_ID_BT_FTPS_GROUP_END){
        id = id - (MSG_ID_BT_FTPS_GROUP_START - MSG_ID_BT_GOEPS_GROUP_START);
     }

     goep_event_handler(id, data);
}
 
void bt_ftp_profile_event_hdlr(U32 msg_id, void* data, U32 u4Size){
    /// put data into queue
    U32 u4Id = msg_id + ((U32)MSG_ID_BT_FTPC_GROUP_START - (U32)MSG_ID_BT_GOEPC_GROUP_START);
    BT_SendMessage( (msg_type) u4Id, MOD_EXTERNAL, data, (U16) u4Size);
}
#endif


#ifdef __BT_GOEP_V2__

const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };
#define FTPS_ABORT_TIMEOUT 120000   /* Remove may using srm sending */

extern void goep_event_handler(U32 msg_id, void* data);
extern U8 ObexSdpDB_Registration(U32 service_type);
GoepClientApp * ftp_find_client_with_goep_conn_id(int goep_conn_id);

GoepServerApp * ftp_find_server_with_goep_conn_id(int goep_conn_id);
void ftps_disconnect_req_hdlr(ftps_disconnect_req_struct* ftp_disconnect_req);
void ftps_TpDisconnect_req_hdlr(ftps_disconnect_req_struct* ftp_disconnect_req);
void ftps_send_abortfile_cnf_msg(U8 goep_conn_id, U8 rsp_code);
void ftps_send_pushfile_rsp_ind_msg(U8 goep_conn_id, U8 rsp_code, const U8 *tmppath, const U8 *finalpath, U32 totalsize );

void ftpc_send_abortfile_cnf_msg(U8 goep_conn_id, U8 rsp_code);

void ftpc_client_internal_rw_hdlr(bt_ftpc_client_internal_rw_ind_struct* req);
void ftps_server_internal_rw_hdlr(bt_ftps_server_internal_rw_ind_struct* req);
/***************/
#define GOEP_INVALID_CMID  0xFFFF

extern void FtpGoepServerCallback(GoepServerEvent *event);
extern void FtpGoepClientCallback(GoepClientEvent *event);

void ftpcAdpCallback(GoepClientEvent *event); /* adp callback */
void ftpsAdpCallback(GoepServerEvent *event); /* adp callback */

BTFTPCallback g_ftp_callback = NULL;
static const ObStoreFuncTable FtpStoreFuncTable = {
    FTPOBS_Read,
    FTPOBS_Write,
    FTPOBS_GetObjectLen,
};
FtpContext gFtp; /* ftp client & server's context */

static int ftps_goep_conn_id[GOEP_NUM_OBEX_SERVER_CONS]; /* Record all goep_conn_id of ftp server */

#define FTPC_APPROFILE_DISPATCH(x,y,z) bt_ftp_profile_event_hdlr(x,y,z)

void btmtk_ftp_register(BTFTPCallback Callback){
    g_ftp_callback = Callback;
}

/* New handler for SRM handle */
void ftpc_pushfile_req_hdlr(bt_ftpc_pushfile_req_struct* req){
    /* OBS open a file */
    /* GOEP_PUSH */
    /* handle fail response if GOPE_PUSH fails */

    GoepClientApp* client = ftp_find_client_with_goep_conn_id(req->goep_conn_id);
    ObexClientApp* ObexApp;
    GoepObjectReq object;
    ObStatus rsp_code;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
    BOOL l_useMem = FALSE;
    S32 filesize = 0;
 
	// TODO: check status for pushing

#ifdef _NO_SRM_WHEN_HEAVY_
    if( req->srm_enable != 0 ){
		if( L2CAP_IsHeavyLoading() >= 5 ){
			OS_Report("ftp: wrn disable srm beacuase heavy");
			req->srm_enable = 0;
		}
    }
#endif

    if(client != NULL)
    {
        ObexApp = &(GOEC(clients)[client->connId].obc);
        btmtk_os_memset( (U8 *) &object, 0, sizeof(GoepObjectReq));

        if((req->obj_name[0] != 0)||(req->obj_name[1] != 0))
        {
            /* Transfer to utf-8 first */
            object.name = get_ctrl_buffer(GOEP_MAX_OBJ_NAME);
            bt_ucs2ncpy( (S8*) object.name, (const S8*)req->obj_name, GOEP_MAX_OBJ_NAME/sizeof(U16));
        }
        else
            object.name = NULL;

        if(req->obj_mime_type[0] != 0)
        {
            object.type = get_ctrl_buffer(GOEP_MAX_MIME_TYPE);
            bt_strncpy((char*)object.type, (char*)req->obj_mime_type, GOEP_MAX_MIME_TYPE);
        }
        else
            object.type = NULL;

        /* use srm mechanism */
        if( 0 != req->srm_enable ){
            object.srm_flags |= OBSF_CLIENT_ENABLE;
            GOEP_Report("[SRM] Add client srm_enable:%d", object.srm_flags);
#ifdef GOEP_TEST_SRM            
            gFtp.clientWaitCount = req->srm_waitcount;
            if( gFtp.clientWaitCount > 0 ){
                ObexApp->client.srm_flags |= OBSF_CLIENT_WAITING;
            }
#endif
        }        

		object.object = FTPOBS_New(client, TRUE, l_useMem, req->goep_conn_id);
        if(object.object != NULL){
			FTPOBS_AppendFilePath( object.object, req->filepath, (U16) req->filepath_len);
			((FtpcObStoreHandle)object.object )->ftpOp = GOEP_OPER_PUSH;
			if( OBRC_SUCCESS != FTPOBS_Open( (FtpcObStoreHandle *) &object.object, FALSE) ){
				FTPOBS_Close( (FtpcObStoreHandle *) &object.object, FALSE);
				object.object = NULL;
			}
			
			/* fill the length */
			if( NULL != object.object ){
     			object.objectLen = ((FtpcObStoreHandle)object.object)->objLen;
			}
		}

		if( filesize >= FTP_LARGE_FILE_SIZE ){
			FTPOBS_CreateMemBuffer(object.object, filesize);
		}		

		if(object.object != NULL){
            ObexApp->goep_first_packet_received = KAL_FALSE;
			client->lastprogress = 0;
			client->abortFlag = client->abortReason = 0;

            rsp_code = GOEP_Push(client, &object);
        }else{
	        rsp_code = OB_STATUS_FAILED;
        }

        if(object.name)
            free_ctrl_buffer(object.name);

        if(object.type)
            free_ctrl_buffer(object.type);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            ftp_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
            ftpc_send_pushfile_cnf_msg(req->goep_conn_id, rsp_code, 1, uuid, uuid_len);
            return;
        }

		client->object = object.object; /* obstore handler */

    }else{
        // pull fail cannot find the connection
        ftpc_send_pushfile_cnf_msg(req->goep_conn_id, OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, req->uuid, req->uuid_len);
    }
	
}

void ftpc_pullfile_req_hdlr(bt_ftpc_pullfile_req_struct *req){
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(req->goep_conn_id);
    ObexClientApp* ObexApp;
    GoepObjectReq object;
    ObStatus rsp_code;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
    BOOL l_useMem = FALSE;

#ifdef _NO_SRM_WHEN_HEAVY_
    if( req->srm_enable != 0 ){
        if( L2CAP_IsHeavyLoading() >= 5 ){
            OS_Report("ftp: wrn disable srm beacuase heavy");
            req->srm_enable = 0;
        }
    }
#endif

    if(client != NULL)
    {
        ObexApp = &(GOEC(clients)[client->connId].obc);
        btmtk_os_memset( (U8 *) &object, 0, sizeof(GoepObjectReq));
        ObexApp->goep_buff_wrote_len = 0;
        /* This packet is the firsk packet of the  */
        if((req->obj_name[0] != 0)||(req->obj_name[1] != 0))
        {
            /* Transfer to utf-8 first */
            object.name = get_ctrl_buffer(GOEP_MAX_OBJ_NAME);
            bt_ucs2ncpy( (S8*) object.name, (const S8*)req->obj_name, GOEP_MAX_OBJ_NAME/sizeof(U16));
        }
        else
            object.name = NULL;

        if(req->obj_mime_type[0] != 0)
        {
            object.type = get_ctrl_buffer(GOEP_MAX_MIME_TYPE);
            bt_strncpy((char*)object.type, (char*)req->obj_mime_type, GOEP_MAX_MIME_TYPE);
        }
        else
            object.type = NULL;

	FTPOBS_TestCode((S8 *)object.name, (S8 *)req->filepath);

        /* use srm mechanism */
        if( 0 != req->srm_enable ){
            object.srm_flags = OBSF_CLIENT_ENABLE;
            
#ifdef GOEP_TEST_SRM            
            gFtp.clientWaitCount = req->srm_waitcount;
            if( gFtp.clientWaitCount > 0 ){
                object.srm_flags |= OBSF_CLIENT_WAITING;
            }
#endif
        }
        
		/* reset the information */
        ObexApp->goep_first_packet_received = KAL_FALSE;
		client->lastprogress = 0;
		client->abortFlag = client->abortReason = 0;
        
		object.object = FTPOBS_New(client, TRUE, l_useMem, req->goep_conn_id);
        if(object.object != NULL){
			FTPOBS_AppendFilePath( object.object, (const U8 *)req->filepath, (U16)req->filepath_len);
			((FtpcObStoreHandle)object.object )->ftpOp = GOEP_OPER_PULL;
			if( OBRC_SUCCESS != FTPOBS_Open( (FtpcObStoreHandle *) &object.object, TRUE) ){
				FTPOBS_Close( (FtpcObStoreHandle *) &object.object, TRUE);
				object.object = NULL;
			}
		}

		if(object.object != NULL){
            ObexApp->goep_first_packet_received = KAL_FALSE;

            rsp_code = GOEP_Pull(client, &object, KAL_FALSE);
        }else{
	        rsp_code = OB_STATUS_FAILED;
        }
        
        if(object.name)
            free_ctrl_buffer(object.name);

        if(object.type)
            free_ctrl_buffer(object.type);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            ftp_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
            ftpc_send_pullfile_cnf_msg(req->goep_conn_id, rsp_code, (U8)rsp_code, NULL, 0, uuid, uuid_len);
            return;
        }

        client->object = object.object; /* obstore handler */
    }else{
        // pull fail cannot find the connection
        ftpc_send_pullfile_cnf_msg(req->goep_conn_id, OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, NULL, 0, req->uuid, req->uuid_len);
    }
}

void ftpc_abortfile_timeout(EvmTimer *timer){
	GoepClientApp* client = NULL;
	ObStatus status = 0;
	OS_Report("[FTP] abortfile timeout cntx:0x%x", timer->context);

	client = (GoepClientApp *)timer->context;
	if( client != NULL ){
		status = GOEP_TpDisconnect(client);
		OS_Report("[FTP] status:%d", status);
		client->abortTimerStatus = FALSE;
	}	
}

void ftpc_abortfile_req_hdlr(bt_ftpc_abortfile_req_struct *req){
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(req->goep_conn_id);
    ObexClientApp* ObexApp;
    ObStatus status = GOEP_STATUS_FAILED;

	OS_Report("[FTP] client 0x%x id:%d", client, req->goep_conn_id);
    if( client != NULL ){
        ObexApp = &(GOEC(clients)[client->connId].obc);

		OS_Report("[FTP] srm_client:%d abortstatus:%d", ObexApp->client.srm_client, client->abortTimerStatus);    	
    	if( ObexApp->client.srm_client == OBSH_SRM_ENABLE ){
        	// inside a SRM wait, send the abort req
        	status = GOEP_ClientAbort(client);
			// Wait for next OBEX request and response
			if( FALSE == client->abortTimerStatus ){
				client->abortTimerStatus = TRUE;
				
				OS_Report("[FTP] start a abort timer to disconnect");
				client->abortTimer.func = ftpc_abortfile_timeout;
				client->abortTimer.context = client;	
				EVM_StartTimer(&client->abortTimer, FTPS_ABORT_TIMEOUT);
			}
			
    	}else{
    	    // no SRM, setup the abort flag. wait for abort
            status = GOEP_STATUS_PENDING;
    	    client->abortFlag = TRUE;
    	    if( 0 == req->rsp_code ){
    	    	req->rsp_code = GOEP_USER_CANCEL;
    	    }
    	    client->abortReason = req->rsp_code;

			// Wait for next OBEX request and response
			if( FALSE == client->abortTimerStatus ){
				client->abortTimerStatus = TRUE;
				
				OS_Report("[FTP] start a abort timer to disconnect");
				client->abortTimer.func = ftpc_abortfile_timeout;
				client->abortTimer.context = client;	
				EVM_StartTimer(&client->abortTimer, FTPS_ABORT_TIMEOUT);
			}
			
        }
    }

    if( status != GOEP_STATUS_PENDING && status != GOEP_STATUS_SUCCESS ){
        ftpc_send_abortfile_cnf_msg(req->goep_conn_id, GOEP_INTERNAL_SERVER_ERR);
    }
}

void ftps_pushfile_rsp_hdlr(bt_ftps_pushfile_rsp_struct *rsp){
    ObexServerApp* ObexAppHandle;
	GoepServerObexCons *goepcons;
	U8 local_enable = 1; //TODO: use GOEP API
	U8 l_addOBS = FALSE;
	BOOL l_useMem = FALSE;
	S32 filesize;
	GoepServerApp *goepApp = ftp_find_server_with_goep_conn_id(rsp->goep_conn_id);

	if( rsp->rsp_code != GOEP_STATUS_SUCCESS ){
		GOEP_ServerAbort( goepApp, rsp->rsp_code);
		GOEP_ServerContinue( goepApp );
	}else{
	
#ifdef _NO_SRM_WHEN_HEAVY_
            if( rsp->srm_enable != 0 ){
		if( L2CAP_IsHeavyLoading() >= 5 ){
			OS_Report("ftp: wrn disable srm beacuase heavy");
			rsp->srm_enable = 0;
		}
            }
#endif

		if( NULL == goepApp->object ){
			goepApp->object = FTPOBS_New(goepApp, FALSE, l_useMem, rsp->goep_conn_id);
			FTPOBS_AppendFilePath( goepApp->object, rsp->filepath, rsp->filepath_len );
			if( OBRC_SUCCESS != FTPOBS_Open((FtpcObStoreHandle*)&goepApp->object, TRUE) ){
				GOEP_Report("[FTP][ERR] fail to open file!");
				FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE);
				goepApp->object = NULL;
			}
			l_addOBS = TRUE;
		}else{
		    /* move the object */
            if( rsp->filepath_len == 0 || OBRC_SUCCESS != FTPOBS_MoveOpenFilePath((FtpcObStoreHandle)goepApp->object, rsp->filepath, rsp->filepath_len) ){
				GOEP_Report("[FTP][ERR] fail to move to ");
				FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, TRUE);
				goepApp->object = NULL;
			}
		}
		
		if( NULL != goepApp->object ){
			goepcons = &GOES(servers)[goepApp->connId];
			if( NULL != goepcons ){
			    // check the filesize and 
       			filesize = goepcons->currOp.info.pushpull.objectLen;
			    FTPOBS_CreateMemBuffer(goepApp->object, filesize);
			}
		}
		
        /* fail to open file */
		if( NULL == goepApp->object ){
		    GOEP_Report("[FTP][WRN] Fail to FTPOBS_New");
		    ftps_send_pushfile_rsp_ind_msg(rsp->goep_conn_id, GOEP_INTERNAL_SERVER_ERR, NULL, NULL, 0);
		    GOEP_ServerAbort( goepApp, GOEP_INTERNAL_SERVER_ERR );
		}else{

			ObexAppHandle = &(GOES(servers)[goepApp->connId].obs);
			goepcons = &GOES(servers)[goepApp->connId];
		
			/* reset transmission state */
			goepApp->abortFlag = FALSE;
			goepApp->lastprogress = 0;

                if( rsp->srm_enable ){
                    if( 0 == (goepcons->obs.server.srm_flags & OBSF_CLIENT_ENABLE) ){
                        SRM_Report("[SRM][FTP][ERR] remote srmenable srm_flags:0x%x", goepcons->obs.server.srm_flags); 
                        goepcons->obs.server.srm_flags = 0;
                     }
                }            

	        if( rsp->srm_enable ){
				/* */
				if(  0 != (goepcons->obs.server.srm_flags & OBSF_CLIENT_ENABLE ) ){
	        		GOEP_Report("[SRM][FTP] add srm_server:0x%x to header", &goepcons->obs.server.srm_server);
	                goepcons->obs.server.srm_flags |= OBSF_SERVER_ENABLE;
	        		// goepcons->obs.server.srm_server = OBSH_SRM_ENABLE;
	                GOEP_ServerQueueHeader(goepApp, OBEXH_SRM, &local_enable, 1);
				}else{
					GOEP_Report("[SRM][FTP][WRN] client doesn't send srm header");
				}
	
	#ifdef GOEP_TEST_SRM
	            gFtp.serverWaitCount = rsp->srm_waitcount;
	
	            if( gFtp.serverWaitCount > 0 ){
	                goepcons->obs.server.srm_flags |= OBSF_SERVER_WAITING;
	            }
	#endif
			} // rsp->srm_enable
			
			if( l_addOBS ){
				GOEP_ServerAccept(goepApp, goepApp->object);
			}
				
		}

		GOEP_ServerContinue( goepApp );
	}

}

void ftps_pullfile_rsp_hdlr(bt_ftps_pullfile_rsp_struct *rsp){
	GoepServerApp *goepApp = ftp_find_server_with_goep_conn_id(rsp->goep_conn_id);
    ObexServerApp* ObexAppHandle;
	GoepServerObexCons *goepcons;
	U8 local_enable = 1; //TODO: use GOEP API
	BOOL l_useMem = FALSE;
	ObStatus status = BT_STATUS_FAILED;

	if( rsp->rsp_code != GOEP_STATUS_SUCCESS ){
		GOEP_ServerAbort( goepApp, rsp->rsp_code);
	}else{
	    
#ifdef _NO_SRM_WHEN_HEAVY_
            if( rsp->srm_enable != 0 ){
		if( L2CAP_IsHeavyLoading() >= 5 ){
			OS_Report("ftp: wrn disable srm beacuase heavy");
			rsp->srm_enable = 0;
		}
            }
#endif

		goepApp->object = FTPOBS_New( &goepApp, FALSE, l_useMem, rsp->goep_conn_id);
		FTPOBS_AppendFilePath( goepApp->object, rsp->filepath, rsp->filepath_len );
		if( OBRC_SUCCESS == FTPOBS_Open( (FtpcObStoreHandle*)&goepApp->object, FALSE) ){
			// setup object length
			ObexAppHandle = &(GOES(servers)[goepApp->connId].obs);
			goepcons = &GOES(servers)[goepApp->connId];

			/* reset transmission state */
			goepApp->lastprogress = 0;
			goepApp->abortFlag = FALSE;
            if( rsp->srm_enable ){
                if( 0 == (goepcons->obs.server.srm_flags & OBSF_CLIENT_ENABLE) ){
                    SRM_Report("[SRM][FTP][ERR] remote srmenable srm_flags:0x%x", goepcons->obs.server.srm_flags); 
                    goepcons->obs.server.srm_flags = 0;
                 }
            }

            if( rsp->srm_enable ){
				if( goepcons->obs.server.srm_flags & OBSF_CLIENT_ENABLE ){
					/* */
					if(  0 != (goepcons->obs.server.srm_flags & OBSF_CLIENT_ENABLE ) ){
        				GOEP_Report("[SRM][FTP] add srm_server:0x%x to header", &goepcons->obs.server.srm_server);
						goepcons->obs.server.srm_flags |= OBSF_SERVER_ENABLE;
        				// goepcons->obs.server.srm_server = OBSH_SRM_ENABLE;
						GOEP_ServerQueueHeader(goepApp, OBEXH_SRM, &local_enable, 1);
					}else{
						GOEP_Report("[SRM][FTP][WRN] client doesn't send srm header");
					}

#ifdef GOEP_TEST_SRM
					gFtp.serverWaitCount = rsp->srm_waitcount;

					if( gFtp.serverWaitCount > 0 ){
						goepcons->obs.server.srm_flags |= OBSF_SERVER_WAITING;
					}
#endif                
				}
            }

			ObexAppHandle->server.objLen = ((FtpcObStoreHandle)goepApp->object)->objLen;
			goepcons->currOp.info.pushpull.objectLen = ObexAppHandle->server.objLen;

			FTPOBS_CreateMemBuffer(goepApp->object, ObexAppHandle->server.objLen);			

			GOEP_ServerAccept( goepApp, goepApp->object );
			status = GOEP_ServerContinue( goepApp );

            // Enter SRM mode continue send data
            if( rsp->srm_enable ){
                if( goepcons->obs.server.srm_flags & OBSF_CLIENT_ENABLE ){
                    while( BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status ){
                        status = GOEP_ServerContinue( goepApp );
                        GOEP_Report("[SRM] pullfile_rsp invoke Servercontinue status:%d", status);
                        
                        if( FALSE == GOEP_ServerSRMMode(goepApp) ){ // only loop inside SRM mode
                            break;
                        }
                        if( FALSE == OBSTACK_IsTxBufferAvailable() ){ 
                            break;
                        }
                        if( TRUE == GOEP_ServerInternalPending(goepApp) ){
                            break;
                        }                                
                    }
                }
            }

		}else{
			GOEP_ServerAbort(goepApp, GOEP_NOT_FOUND);
			GOEP_ServerContinue( goepApp );
		}
	}

}

void ftps_abortfile_req_hdlr(bt_ftps_abortfile_req_struct *req){
	GoepServerApp *goepApp = ftp_find_server_with_goep_conn_id(req->goep_conn_id);
    GoepServerObexCons *goepcons;
    
    // check status
    goepcons = &GOES(servers)[goepApp->connId];
    GOEP_Report("[FTP] ftps_abortfile_req_handler currOp:%d", goepcons->currOp);
    if( goepcons->currOp.oper == GOEP_OPER_PUSH || goepcons->currOp.oper == GOEP_OPER_PULL) {
        goepApp->abortFlag = TRUE;
		goepApp->abortReason = req->rsp_code;

        return;
    }

    ftps_send_abortfile_cnf_msg(req->goep_conn_id, GOEP_STATUS_FAILED);
}

void goep_ftp_event_handler(U32 msg_id, void* data)
{
    /// data is 'ilm_ptr->local_para_ptr' in MAUI platform
	if ( NULL != data)
    {
		switch(msg_id)
		{
			case MSG_ID_BT_FTPS_REGISTER_SERVER_REQ:
                ftps_register_server_req_hdlr(data);
                break;
			case MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ:
			    ftps_deregister_server_req_hdlr(data);
                break;
			case MSG_ID_BT_FTPS_CONNECT_RSP:
			    ftps_connect_rsp_hdlr(data);
                break;
			case MSG_ID_BT_FTPS_AUTHORIZE_RSP:
			    ftps_authorize_rsp_hdlr(data);
                break;
			case MSG_ID_BT_FTPS_PUSH_RSP:
                ftps_push_rsp_hdlr(data);
                break;
  			case MSG_ID_BT_FTPS_PULL_RSP:
                ftps_pull_rsp_hdlr(data);
                break;
			case MSG_ID_BT_FTPS_SET_FOLDER_RSP:
			    ftps_set_folder_rsp_hdlr(data);
                break;
			case MSG_ID_BT_FTPS_ABORT_RSP:
			    ftps_abort_rsp_hdlr(data);
                break;
            case MSG_ID_BT_FTPS_AUTH_RSP:
                ftps_auth_rsp_hdlr(data);
                break;
  			case MSG_ID_BT_FTPC_CONNECT_REQ:
				ftpc_connect_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_PUSH_REQ:
				ftpc_push_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_PULL_REQ:
				ftpc_pull_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_SET_FOLDER_REQ:
				ftpc_set_folder_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_ACTION_REQ:
				ftpc_action_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_ABORT_REQ:
				ftpc_abort_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_DISCONNECT_REQ:
				ftpc_disconnect_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_AUTH_REQ:
				goep_auth_req_hdlr(data);
				break;
  			case MSG_ID_BT_FTPC_AUTH_RSP:
				goep_auth_rsp_hdlr(data);
				break;
  			// case MSG_ID_BT_FTPC_TPDISCONNECT_REQ:
				//ftpc_TpDisconnect_req_hdlr(data);
				//break;
  			case MSG_ID_BT_FTPS_TPDISCONNECT_REQ:
				ftps_TpDisconnect_req_hdlr(data);
				break;
			// New
			case MSG_ID_BT_FTPC_PUSHFILE_REQ:
				ftpc_pushfile_req_hdlr(data);
				break;
			case MSG_ID_BT_FTPC_PULLFILE_REQ:
				ftpc_pullfile_req_hdlr(data);
				break;
		    case MSG_ID_BT_FTPC_ABORTFILE_REQ:
				ftpc_abortfile_req_hdlr(data);
				break;				
            case MSG_ID_BT_FTPS_PUSHFILE_RSP:
				ftps_pushfile_rsp_hdlr(data);
				break;
            case MSG_ID_BT_FTPS_PULLFILE_RSP:
				ftps_pullfile_rsp_hdlr(data);
				break;
            case MSG_ID_BT_FTPS_ABORTFILE_REQ:
				ftps_abortfile_req_hdlr(data);
				break;
			case MSG_ID_BT_FTPS_ACTION_RSP:
			    goep_action_rsp_hdlr(data);
			    break;
			case MSG_ID_BT_FTPS_SERVER_INTERNAL_RW:
				ftps_server_internal_rw_hdlr(data);
				break;
			case MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW:
				ftpc_client_internal_rw_hdlr(data);
				break;
			default:
                GOEP_Report("[FTP][ERR] unknown id ilm_ptr->msg_id=%d, MSG_ID_BT_FTPS_REGISTER_SERVER_REQ=%d,", (U32)msg_id, MSG_ID_BT_FTPS_REGISTER_SERVER_REQ);
				// Assert(0);				
		}
	}else{
	    Assert(0);
	}
} /* end of module main function */

void ftp_handle_message(U32 msg_id, void* data){
     S32 id = msg_id;
     goep_disconnect_req_struct *disc_req;

     if( MSG_ID_BT_FTPC_DISCONNECT_REQ == id || MSG_ID_BT_GOEPC_DISCONNECT_REQ == id  ){
        disc_req = (goep_disconnect_req_struct *)data ;
        if( (disc_req->uuid_len != sizeof(OBEX_FTP_UUID) ) || (0 != btmtk_os_memcmp( disc_req->uuid, sizeof(OBEX_FTP_UUID), OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID) )) ){
            btmtk_os_memcpy( disc_req->uuid, OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));
            disc_req->uuid_len = sizeof(OBEX_FTP_UUID);
        }
     }
     if( MSG_ID_BT_FTPS_TPDISCONNECT_REQ == id || MSG_ID_BT_GOEPS_TPDISCONNECT_REQ == id ){
        disc_req = (goep_disconnect_req_struct *)data ;
        if( (disc_req->uuid_len != sizeof(OBEX_FTP_UUID) ) || (0 != btmtk_os_memcmp( disc_req->uuid, sizeof(OBEX_FTP_UUID), OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID) )) ){
            btmtk_os_memcpy( disc_req->uuid, OBEX_FTP_UUID, sizeof(OBEX_FTP_UUID));
            disc_req->uuid_len = sizeof(OBEX_FTP_UUID);
        }
     }
#ifndef __BT_GOEP_V2__        
     if( id >= MSG_ID_BT_FTPC_GROUP_START && id <= MSG_ID_BT_FTPC_GROUP_END){
        id = id - (MSG_ID_BT_FTPC_GROUP_START - MSG_ID_BT_GOEPC_GROUP_START);
     }else if( id >= MSG_ID_BT_FTPS_GROUP_START && id <= MSG_ID_BT_FTPS_GROUP_END){
        id = id - (MSG_ID_BT_FTPS_GROUP_START - MSG_ID_BT_GOEPS_GROUP_START);
     }
#endif
     // goep_event_handler(id, data);
     goep_ftp_event_handler(msg_id, data);
}
 
void bt_ftp_profile_event_hdlr(U32 msg_id, void* data, U32 u4Size){
    /// put data into queue
#if 1
    U32 u4Id;
    if( msg_id >= MSG_ID_BT_FTPC_GROUP_START  && msg_id <= MSG_ID_BT_FTPC_GROUP_END ){
    	u4Id = msg_id;
	}else if( msg_id >= MSG_ID_BT_FTPS_GROUP_START  && msg_id <= MSG_ID_BT_FTPS_GROUP_END ){
		u4Id = msg_id;
    }else{
		Assert(0);
    //	u4Id = msg_id + ((U32)MSG_ID_BT_FTPC_GROUP_START - (U32)MSG_ID_BT_GOEPC_GROUP_START);
        return;
    }
#else
    U32 u4Id = msg_id + ((U32)MSG_ID_BT_FTPC_GROUP_START - (U32)MSG_ID_BT_GOEPC_GROUP_START);
#endif
    BT_SendMessage( (msg_type) u4Id, MOD_EXTERNAL, data, (U16) u4Size);
}

void ftps_init_goep_conn_id()
{
    int i;

    for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        ftps_goep_conn_id[i] = 0xFE;  /* avoid case: When Client Connecting, BT Reset. disconnect_ind_hdlr*/
    }
}

void ftps_save_goep_conn_id(int goep_conn_id)
{
    int i;

    for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        if (ftps_goep_conn_id[i] == 0xFE)
        {
            GOEP_Report("[FTP] ftps_save_goep_conn_id save:0x%x", goep_conn_id);
            ftps_goep_conn_id[i] = goep_conn_id;

            return;
        }
    }
    GOEP_Report("[FTP] ftps_save_goep_conn_id save goep_conn_id error");
}

void ftps_delete_goep_conn_id(int goep_conn_id)
{
    int i;

    for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        if (ftps_goep_conn_id[i] == goep_conn_id)
        {
            GOEP_Report("[FTP] ftps_delete_goep_conn_id delete:0x%x", goep_conn_id);
            ftps_goep_conn_id[i] = 0xFE;

            return;
        }
    }
    GOEP_Report("[FTP] ftps_delete_goep_conn_id delete none");
}

void ftpc_init(){
    FTPOBS_Init();

    gFtp.adpclientCb = ftpcAdpCallback;
}

void ftps_init(){
    //FTPSOBS_Init();
    ftps_init_goep_conn_id();
    gFtp.adpserverCb = ftpsAdpCallback;
}


/****************************************************************************
 *
 * Internal functions
 *
 ****************************************************************************/
void ftpcAdpCallback(GoepClientEvent *event){
	GoepClientApp *goepApp = event->handler;
	SRM_Report("[FTP] ftpcAdpCallback GOEP_Event:%d ", event->event);
	
    switch(event->event){
        case GOEP_EVENT_CONTINUE:
#ifdef GOEP_TEST_SRM
            if( gFtp.clientWaitCount > 0 ){
                SRM_Report("[SRM] adp clientWaitCount:%d", gFtp.clientWaitCount);
                gFtp.clientWaitCount--;
                if( 0 == gFtp.clientWaitCount ){
                    GOEP_ClientDisableWait(goepApp);
                }
            }
#endif            
        break;
    }
}

void ftpsAdpCallback(GoepServerEvent *event){
	GoepServerApp *goepApp = event->handler;

	kal_trace(BT_TRACE_G6_OBEX, FTP_ADP_CALLBACK, event->event);
	
    switch(event->event){
        case GOEP_EVENT_CONTINUE:
#ifdef GOEP_TEST_SRM            
            if( gFtp.serverWaitCount > 0 ){
                SRM_Report("[SRM] adp serverWaitCount:%d", gFtp.serverWaitCount);
                gFtp.serverWaitCount--;
                if( 0 == gFtp.serverWaitCount ){
                    GOEP_ServerDisableWait(goepApp);
                }
            }
#endif            
        break;
    }
}

/**********************************/



/*******************************************************************************/



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
#define TD_CTRL 2
static const U8 FileXferUuid[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };
// 090820 daylong: copy from goep.c
/*
static const unsigned char OBEX_FTP_UUID[] = { 0xF9, 0xEC, 0x7B, 0xC4, 0x95, 0x3C, 0x11, 0xD2,
                                   0x98, 0x4E, 0x52, 0x54, 0x00, 0xDC, 0x9E, 0x09 };
*/
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
void ftp_approfile_handler(U32 msg_id, void* data, U32 size);

#ifndef FTP_APPROFILE_DISPATCH
#define FTP_APPROFILE_DISPATCH(x,y,z) ftp_approfile_handler(x, y, z);
#endif



void ftp_approfile_handler(U32 msg_id, void* data, U32 size){
    
    obex_msg_struct *obex_msg = (obex_msg_struct *) data;
    
//    /* OPP */
//    if (obex_msg->uuid_len == 0 && obex_msg->uuid[0] == '\0')
//    {
//#if defined(__BT_OPP_PROFILE__)
//		/// Should Move. ftp_main (opp & ftp's intadp)
//        bt_opp_profile_event_hdlr(msg_id, data, size);
//        return;
//#endif /* defined(__BT_OPP_PROFILE__) */ 
//    }        
    /* FTP */
#if defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__)        
//    else if ( bt_strncmp( (const char*)OBEX_FTP_UUID, (const char*)obex_msg->uuid, 16 ) == 0 )
    {
		/// Should Move. ftp_main (opp & ftp's intadp)
        bt_ftp_profile_event_hdlr(msg_id, data, size);
		return;
    }
#endif //#endif /* defined(__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__) */     
//    else{
//        /// error msg. catch it
//    }
    ASSERT(0);
}



void ftp_free_clientapp_ctrl_buffer(GoepClientApp *client){
	// free the 
	U8 idx = 0;
	ASSERT(client != NULL);
	if( client != NULL ){
#if BT_GOEP_V2 == XA_ENABLED
		if( 0 != client->l2capRawDataNum ){
			for( idx = 0; idx < client->l2capRawDataNum ; idx++ ){
				// ASSERT( client->pl2capRawData[idx] );
			    if( client->pl2capRawData[idx] != NULL ){
				    free_ctrl_buffer( client->pl2capRawData[idx] );
				    client->pl2capRawData[idx] = NULL;
			    }
			}
			client->l2capRawDataNum = 0;
		}
#endif
		free_ctrl_buffer(client);
	}
}

void ftp_free_serverapp_ctrl_buffer(GoepServerApp *server){
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
*  ftp_need_obex_auth_challenge
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
kal_bool ftp_need_obex_auth_challenge(U8 *obex_uuid, U8 uuid_len, int *pIndex)
{
//    int i;


//    if (!auth_chal_profiles_inited)
//    {
//        btmtk_os_memset(auth_chal_profiles, 0, sizeof(auth_chal_profiles));
//        
//	    auth_chal_profiles[0].uuid_len = sizeof(FileXferUuid);
//		btmtk_os_memcpy(auth_chal_profiles[0].uuid, FileXferUuid, sizeof(FileXferUuid));
//
//		auth_chal_profiles[0].passwd_len = strlen(FTP_PWD_STR);
//	    btmtk_os_memcpy(auth_chal_profiles[0].passwd, FTP_PWD_STR, 
//						auth_chal_profiles[0].passwd_len);
//
//	    auth_chal_profiles_inited = KAL_TRUE;
//    }
//
//    for (i = 0; i < MAX_AUTHCHAL_PROFILES_NUM; i++)
//    {
//        if(auth_chal_profiles[i].uuid 
//			&& (auth_chal_profiles[i].uuid[0] != 0)
//			&& (bt_strncmp((const char*)obex_uuid, (const char*)auth_chal_profiles[i].uuid, 
//					auth_chal_profiles[i].uuid_len) == 0))
//        {
//            *pIndex = i;
//            return KAL_TRUE;
//        }
//    }
    return KAL_FALSE;
}

extern int rand(void);

ObexAuthChallenge* ftp_build_auth_challenge_data(int index)
{
    ObexAuthChallenge* Challenge;
    U8 rand_str[16];
	
    if (index < 0 || index >= MAX_AUTHCHAL_PROFILES_NUM)
		return NULL;

    Challenge = get_ctrl_buffer(sizeof(ObexAuthChallenge));
    btmtk_os_memset((U8 *)Challenge, 0, sizeof(ObexAuthChallenge));

    sprintf((char*)rand_str, "%12d", rand()); // TODO: generate this
    
    Challenge->challengeLen = strlen((const char*)rand_str) + 1;
    Challenge->challenge = get_ctrl_buffer(Challenge->challengeLen);
    btmtk_os_memcpy(Challenge->challenge, rand_str, Challenge->challengeLen);

    return Challenge;	
}


/*****************************************************************************
* FUNCTION
*  ftp_find_ServerObexCons_with_goep_conn_id 
* DESCRIPTION
*  This function is to find out the ServerObexCons with goep_conn_id
* PARAMETERS
*  goep_conn_id     IN       The goep connection id of the wanted server
* RETURNS
*  The GOEP ServerObexCons with the input goep_conn_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepServerObexCons * ftp_find_ServerObexCons_with_goep_conn_id(int goep_conn_id)
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
*  ftp_find_free_ftp_id 
* DESCRIPTION
*  This function is to find out the free goep connection id to use
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
int ftp_find_free_ftp_id(void)
{
	// TODO: not use this
    int i;
    
    for(i = 0; i < GOEP_CONNECTION_NUM; i++)
    {
//        if(global_ftp_connection[i] == NULL)
//           return i;
    }
    return -1; /* Can not find out free goep connection id */
}



/*****************************************************************************
* FUNCTION
*  ftp_find_server_with_goep_conn_id 
* DESCRIPTION
*  This function is to find out the server with goep_conn_id
* PARAMETERS
*  goep_conn_id     IN       The goep connection id of the wanted server
* RETURNS
*  The GOEP server context with the input goep_conn_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepServerApp * ftp_find_server_with_goep_conn_id(int goep_conn_id)
{
    int i;
    int j = 0;
    
    for(i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        if(&(GOES(servers)[i]) != NULL)
        {
            if(GOES(servers)[i].obs.goep_conn_id == goep_conn_id)
            {
                while (j < GOEP_MAX_PROFILES && GOES(servers)[i].profiles[j] == NULL )
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
*  ftp_find_client_with_goep_conn_id 
* DESCRIPTION
*  This function is to find out the client with goep_conn_id
* PARAMETERS
*  goep_conn_id     IN       The goep connection id of the wanted client
* RETURNS
*  The GOEP server context with the input goep_conn_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepClientApp * ftp_find_client_with_goep_conn_id(int goep_conn_id)
{
    int i;
    int j = 0;
    
    for(i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++)
    {
        if(&(GOEC(clients)[i]) != NULL)
        {
            if(GOEC(clients)[i].obc.goep_conn_id == goep_conn_id)
            {
                while ( j < GOEP_MAX_PROFILES && GOEC(clients)[i].profiles[j] == NULL  )
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
*  ftp_find_client_with_req_id 
* DESCRIPTION
*  This function is to find out the client with req_id
* PARAMETERS
*  req_id     IN       The id of the connect request
* RETURNS
*  The GOEP server context with the goep req_id
* GLOBALS AFFECTED
*  None
*****************************************************************************/
GoepClientApp * ftp_find_client_with_req_id(int req_id)
{
    int i;
    int j = 0;
    
    for(i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++)
    {
        if(&(GOEC(clients)[i]) != NULL)
        {
            if(GOEC(clients)[i].obc.req_id == req_id)
            {
                while ( j < GOEP_MAX_PROFILES && GOEC(clients)[i].profiles[j] == NULL )
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
*  ftp_bd_addr_stuct2array 
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
void ftp_bd_addr_stuct2array(goep_bd_addr_struct *src, BD_ADDR *dst)
{
    btmtk_os_memcpy( (U8 *) &dst->addr[0], (U8 *) &src->lap, 3);
    dst->addr[3] = src->uap;
    btmtk_os_memcpy( (U8 *) &dst->addr[4], (U8 *) &src->nap, 2);
}

/*****************************************************************************
* FUNCTION
*  ftp_bd_addr_array2struct 
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
void ftp_bd_addr_array2struct(BD_ADDR *src, goep_bd_addr_struct *dst)
{
    dst->lap = 0;
    btmtk_os_memcpy( (U8 *) &dst->lap, &src->addr[0], 3);
    dst->uap = src->addr[3];
    btmtk_os_memcpy( (U8 *) &dst->nap, &src->addr[4], 2);
}

/*****************************************************************************
* FUNCTION
*  ftp_find_bt_uuid_with_obex_uuid
* DESCRIPTION
*  This function is to find out the Bluetooth uuid with Obex uuid
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
int ftp_find_bt_uuid_with_obex_uuid(U32* bt_uuid_p, U8* obex_uuid_str)
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
*  ftp_find_obex_uuid_with_bt_uuid
* DESCRIPTION
*  This function is to find out the Obex uuid with Bluetooth uuid
* PARAMETERS
*  None
* RETURNS
*  None
* GLOBALS AFFECTED
*  None
*****************************************************************************/
static int ftp_find_obex_uuid_with_bt_uuid(U32* bt_uuid_p, U8* obex_uuid_str, U8* obex_uuid_len)
{
    if(*bt_uuid_p == SC_OBEX_FILE_TRANSFER)
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
#if 0 
/*****************************************************************************
* FUNCTION
*  ftp_main
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
void ftp_event_handler(U32 msg_id, void* data)
{
    /// data is 'ilm_ptr->local_para_ptr' in MAUI platform
    Assert(0);
	if ( NULL != data)
    {
		switch(msg_id)
		{
			case MSG_ID_BT_GOEPS_REGISTER_SERVER_REQ:
                ftps_register_server_req_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_DEREGISTER_SERVER_REQ:
			    ftps_deregister_server_req_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_CONNECT_RSP:
			    ftps_connect_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_AUTHORIZE_RSP:
			    ftps_authorize_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_PUSH_RSP:
                ftps_push_rsp_hdlr(data);
                break;
  			case MSG_ID_BT_GOEPS_PULL_RSP:
                ftps_pull_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_SET_FOLDER_RSP:
			    ftps_set_folder_rsp_hdlr(data);
                break;
            case MSG_ID_BT_GOEPS_ACTION_RSP:
			    ftps_action_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_ABORT_RSP:
			    ftps_abort_rsp_hdlr(data);
                break;
			case MSG_ID_BT_GOEPS_AUTH_REQ:
			    ftps_auth_req_hdlr(data);
                break;
            case MSG_ID_BT_GOEPS_AUTH_RSP:
              	ftps_auth_rsp_hdlr(data);
                break;
  			case MSG_ID_BT_GOEPC_CONNECT_REQ:
				ftpc_connect_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_PUSH_REQ:
				ftpc_push_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_PULL_REQ:
				ftp_pull_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_SET_FOLDER_REQ:
				ftp_set_folder_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_ACTION_REQ:
				ftpc_action_req_hdlr(data);
				break;				
  			case MSG_ID_BT_GOEPC_ABORT_REQ:
				ftp_abort_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_DISCONNECT_REQ:
				ftp_disconnect_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_AUTH_REQ:
				ftpc_auth_req_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPC_AUTH_RSP:
				ftp_auth_rsp_hdlr(data);
				break;
  			case MSG_ID_BT_GOEPS_TPDISCONNECT_REQ:
				ftp_TpDisconnect_req_hdlr(data);
				break;
			default:
                //TODO GOEP_Report(("[GOEP]ftp_main: ilm_ptr->msg_id=%d, GOEP_REGISTER_SERVER_REQ=%d, GOEP_AUTHORIZE_RSP=%d,",
                //(U32)msg_id,MSG_ID_BT_GOEP_REGISTER_SERVER_REQ,MSG_ID_BT_GOEP_AUTHORIZE_RSP));
				ASSERT(0);
		}
	}else{
	    ASSERT(0);
	}
} /* end of module main function */

#endif

void ftpc_start_aborttimer(GoepClientApp* client){
}

void ftpc_stop_aborttimer(GoepClientApp* client){
	if( TRUE == client->abortTimerStatus ){
		client->abortTimerStatus = FALSE;
		EVM_ResetTimer(&client->abortTimer);
		OS_Report("[FTP] stop abortTimer");			
	}	
}

/*****************************************************************************
* FUNCTION
*  ftp_send_rsp_msg
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
void ftp_send_cnf_msg(U32 cnf_msg_id, U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len)
{
    ftp_cnf_struct* ftp_cnf = NULL;

    ftp_cnf = (ftp_cnf_struct*)
        construct_local_para(sizeof(ftp_cnf_struct), TD_CTRL);
    ASSERT(ftp_cnf != NULL);

    btmtk_os_memset(ftp_cnf->uuid, 0, sizeof(ftp_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_cnf->uuid, uuid_p, uuid_len);

    ftp_cnf->uuid_len = uuid_len;
    ftp_cnf->goep_conn_id = goep_conn_id;
    ftp_cnf->rsp_code = (U8)rsp_code;
    
    FTP_APPROFILE_DISPATCH(cnf_msg_id, ftp_cnf , sizeof(ftp_cnf_struct) );  

}


/*****************************************************************************
* FUNCTION
*  ftps_send_disconnect_ind_msg
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
void ftps_send_disconnect_ind_msg(U8 req_id, U8 goep_conn_id, U8* uuid_p, U8 uuid_len, U8 reason)
{
    ftpc_disconnect_ind_struct* ftp_disconnect_ind;
    GoepServerApp *Service;
    GoepClientApp* client;

    /* Attention: 
     * uuid_p may be member of client that will be deregistered as following. 
     * so record the data here.
     */
    ftp_disconnect_ind = (ftpc_disconnect_ind_struct*)
        construct_local_para(sizeof(ftpc_disconnect_ind_struct), TD_CTRL);
    ASSERT(ftp_disconnect_ind != NULL);

    ftp_disconnect_ind->goep_conn_id = goep_conn_id;
    ftp_disconnect_ind->req_id = req_id;

    btmtk_os_memset(ftp_disconnect_ind->uuid, 0, sizeof(ftp_disconnect_ind->uuid));
    ftp_disconnect_ind->reason = (U8) reason;
    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_disconnect_ind->uuid, uuid_p, uuid_len);
    
    ftp_disconnect_ind->uuid_len = uuid_len;


    /***** search the server first *****/
    Service = ftp_find_server_with_goep_conn_id(goep_conn_id);

    if(Service == NULL)
    {
        if(goep_conn_id == 0xFF)
        {
            client = ftp_find_client_with_req_id(req_id);
            if(client != NULL)
            {
                GOEP_DeregisterClient(client);
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                /* free password memory -- added by yfchu */
                if (client->connect_req.password)
                    free_ctrl_buffer(client->connect_req.password);
                
                ftp_free_clientapp_ctrl_buffer(client);
            }
        }
        else
        {
            client = ftp_find_client_with_goep_conn_id(goep_conn_id);
            if(client != NULL)
            {
                if (client->cm_conn_id != GOEP_INVALID_CMID)
                {
                    kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_RELEASE_CONNID, client->cm_conn_id);
                    BTBMProfileReleaseConnIndex((U8)client->cm_conn_id);
                }

                GOEP_DeregisterClient(client);
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                    /* free password memory -- added by yfchu */
                    if (client->connect_req.password)
                        free_ctrl_buffer(client->connect_req.password);
                ftp_free_clientapp_ctrl_buffer(client);
            }
        }
		if(client != NULL){
			ftpc_stop_aborttimer(client);					
		}
		
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_TPDISCONNECT_IND, ftp_disconnect_ind , sizeof(ftpc_disconnect_ind_struct) );
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
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_TPDISCONNECT_IND, ftp_disconnect_ind , sizeof(ftpc_disconnect_ind_struct) );
    }

}

/*****************************************************************************
* FUNCTION
*  ftpc_send_disconnect_ind_msg
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
void ftpc_send_disconnect_ind_msg(U8 req_id, U8 goep_conn_id, U8* uuid_p, U8 uuid_len, U8 reason)
{
    ftpc_disconnect_ind_struct* ftp_disconnect_ind;
    GoepServerApp *Service;
    GoepClientApp* client;

    /* Attention: 
     * uuid_p may be member of client that will be deregistered as following. 
     * so record the data here.
     */
    ftp_disconnect_ind = (ftpc_disconnect_ind_struct*)
        construct_local_para(sizeof(ftpc_disconnect_ind_struct), TD_CTRL);
    ASSERT(ftp_disconnect_ind != NULL);

    ftp_disconnect_ind->goep_conn_id = goep_conn_id;
    ftp_disconnect_ind->req_id = req_id;

    btmtk_os_memset(ftp_disconnect_ind->uuid, 0, sizeof(ftp_disconnect_ind->uuid));
    ftp_disconnect_ind->reason = (U8) reason;
    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_disconnect_ind->uuid, uuid_p, uuid_len);
    
    ftp_disconnect_ind->uuid_len = uuid_len;


    /***** search the server first *****/
    Service = ftp_find_server_with_goep_conn_id(goep_conn_id);

    if(Service == NULL)
    {
        if(goep_conn_id == 0xFF)
        {
            client = ftp_find_client_with_req_id(req_id);
            if(client != NULL)
            {
                GOEP_DeregisterClient(client);
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                /* free password memory -- added by yfchu */
                if (client->connect_req.password)
                    free_ctrl_buffer(client->connect_req.password);
                
                ftp_free_clientapp_ctrl_buffer(client);
            }
        }
        else
        {
            client = ftp_find_client_with_goep_conn_id(goep_conn_id);
            if(client != NULL)
            {
                if (client->cm_conn_id != GOEP_INVALID_CMID)
                {
                    kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_RELEASE_CONNID, client->cm_conn_id);
                    BTBMProfileReleaseConnIndex((U8)client->cm_conn_id);
                }

                GOEP_DeregisterClient(client);
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                    /* free password memory -- added by yfchu */
                    if (client->connect_req.password)
                        free_ctrl_buffer(client->connect_req.password);
                ftp_free_clientapp_ctrl_buffer(client);
            }
        }

		if(client != NULL){
			if( TRUE == client->abortTimerStatus ){
				client->abortTimerStatus = FALSE;
				EVM_ResetTimer(&client->abortTimer);
				OS_Report("[FTP] stop abortTimer");			
			}
		}
		
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_TPDISCONNECT_IND, ftp_disconnect_ind , sizeof(ftpc_disconnect_ind_struct) );
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
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_TPDISCONNECT_IND, ftp_disconnect_ind , sizeof(ftpc_disconnect_ind_struct) );
    }

}




/*****************************************************************************
* FUNCTION
*  ftps_send_connect_ind_msg
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
void ftps_send_connect_ind_msg(U8 goep_conn_id, U16 peer_mru, U8 challenge, U8 response, U8 tptype, U8 obexver)
{
    ftps_connect_ind_struct* ftp_connect_ind = NULL;
    U32 profile_id;
    ObexTpConnInfo      tpInfo;
    kal_bool success;
    ObexAppHandle* obexapphdl_p;
    goep_bd_addr_struct ftp_bd_addr;
    char* dev_name = 0;
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(goep_conn_id);

    ASSERT(Service != NULL);

    ftp_connect_ind = (ftps_connect_ind_struct*)
        construct_local_para(sizeof(ftps_connect_ind_struct), TD_CTRL);
    ASSERT(ftp_connect_ind != NULL);

    ftp_connect_ind->goep_conn_id = goep_conn_id;
    obexapphdl_p = &(GOES(servers)[Service->connId].obs.handle);

    success = OBEX_GetTpConnInfo(obexapphdl_p, &tpInfo);
    ASSERT(success);
    ftp_bd_addr_array2struct(&(tpInfo.remDev->bdAddr), &ftp_bd_addr);
    ftp_connect_ind->bd_addr = ftp_bd_addr;

    dev_name = (char *)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr)) ;

    btmtk_os_memset(ftp_connect_ind->dev_name, 0, sizeof(ftp_connect_ind->dev_name));

    if(dev_name != NULL)
        bt_strncpy((char*)ftp_connect_ind->dev_name, (char*)dev_name, GOEP_MAX_DEV_NAME);

    btmtk_os_memset(ftp_connect_ind->uuid, 0, sizeof(ftp_connect_ind->uuid));

    //if((uuid_p != NULL)&&(uuid_len != 0))
    //    btmtk_os_memcpy(ftp_connect_ind->uuid, uuid_p, uuid_len);

    //ftp_connect_ind->uuid_len = uuid_len;

    ftp_find_bt_uuid_with_obex_uuid((U32*)&profile_id, ftp_connect_ind->uuid);

    if (Service->cm_conn_id == GOEP_INVALID_CMID)
    {
        Service->cm_conn_id = (U32) BTBMProfileAllocateConnIndex(profile_id, tpInfo.remDev->bdAddr);
        kal_trace(BT_TRACE_G6_OBEX,GOEP_SERVICE_ALLOC_CONNID, Service->cm_conn_id);
    }
    else
    {
        GOEP_Report(("[GOEP] Service->cm_conn_id has alloced before!"));
    }

    ftp_connect_ind->cm_conn_id = Service->cm_conn_id;
    ftp_connect_ind->peer_mru = peer_mru;
    ftp_connect_ind->tptype = tptype;
    ftp_connect_ind->obexver = obexver;

    ftp_connect_ind->client_auth = challenge;
    ftp_connect_ind->server_auth = response;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_CONNECT_IND, ftp_connect_ind  , sizeof(ftps_connect_ind_struct) );

}

/*****************************************************************************
* FUNCTION
*  ftps_send_push_ind_msg
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
void ftps_send_push_ind_msg(U8 goep_conn_id, goep_pkt_type_enum pkt_type, 
                            goep_put_type_enum put_type, U32 total_obj_len,
                            U8* obj_name, U16 obj_name_len, 
                            U8* obj_mime_type, U16 obj_mime_type_len, 
                            U8* frag_ptr, U16 frag_len,
                            U8* uuid_p, U8 uuid_len)
{
    ftps_push_ind_struct* ftp_push_ind = NULL;
    GoepServerApp *Service;

    ftp_push_ind = (ftps_push_ind_struct*)
        construct_local_para(sizeof(ftps_push_ind_struct), TD_CTRL);
    ASSERT(ftp_push_ind != NULL);

    ftp_push_ind->goep_conn_id = goep_conn_id;
    ftp_push_ind->pkt_type = pkt_type;

    ftp_push_ind->put_type = put_type;
    ftp_push_ind->total_obj_len = total_obj_len;

    btmtk_os_memset(ftp_push_ind->obj_name, 0, sizeof(ftp_push_ind->obj_name));
    
    if(obj_name != NULL)
    {
        if(obj_name_len >= GOEP_MAX_OBJ_NAME-1)
        {
            btmtk_os_memcpy(ftp_push_ind->obj_name, obj_name, (GOEP_MAX_OBJ_NAME-2));
            ftp_push_ind->obj_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ftp_push_ind->obj_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ftp_push_ind->obj_name, obj_name, obj_name_len);
            ftp_push_ind->obj_name[obj_name_len] = 0;
            ftp_push_ind->obj_name[obj_name_len+1] = 0;
        }
    }

    btmtk_os_memset(ftp_push_ind->obj_mime_type, 0, sizeof(ftp_push_ind->obj_mime_type));

    if(obj_mime_type != NULL)
    {
        if(obj_mime_type_len >= GOEP_MAX_MIME_TYPE-1)
        {
            btmtk_os_memcpy(ftp_push_ind->obj_mime_type, obj_mime_type, (GOEP_MAX_MIME_TYPE-2));
            ftp_push_ind->obj_mime_type[GOEP_MAX_MIME_TYPE-2] = 0;
            ftp_push_ind->obj_mime_type[GOEP_MAX_MIME_TYPE-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ftp_push_ind->obj_mime_type, obj_mime_type, obj_mime_type_len);
            ftp_push_ind->obj_mime_type[obj_mime_type_len] = 0;
            ftp_push_ind->obj_mime_type[obj_mime_type_len+1] = 0;
        }
    }

    ftp_push_ind->frag_ptr = frag_ptr;
    ftp_push_ind->frag_len = frag_len;

    btmtk_os_memset(ftp_push_ind->uuid, 0, sizeof(ftp_push_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_push_ind->uuid, uuid_p, uuid_len);

    ftp_push_ind->uuid_len = uuid_len;
    
    Service = ftp_find_server_with_goep_conn_id(goep_conn_id);
    GOEP_ServerResetPushNameInfo(Service);

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_PUSH_IND, ftp_push_ind , sizeof(ftps_push_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_send_pushfile_ind_msg
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
void ftps_send_pushfile_ind_msg(U8 goep_conn_id, U32 total_obj_len,
                            U8* obj_name, U16 obj_name_len, 
                            U8* obj_mime_type, U16 obj_mime_type_len, 
                            U8 srm_enable)
{
    bt_ftps_pushfile_ind_struct* ind = NULL;
    GoepServerApp *Service;

    ind = (bt_ftps_pushfile_ind_struct*)
        construct_local_para(sizeof(bt_ftps_pushfile_ind_struct), TD_CTRL);
    ASSERT(ftp_push_ind != NULL);

    ind->goep_conn_id = goep_conn_id;
    //ind->pkt_type = pkt_type;

    //ind->put_type = put_type;
    ind->total_obj_len = total_obj_len;
    ind->srm_enable = srm_enable;

    btmtk_os_memset(ind->obj_name, 0, sizeof(ind->obj_name));
    
    if(obj_name != NULL)
    {
        if(obj_name_len >= GOEP_MAX_OBJ_NAME-1)
        {
            btmtk_os_memcpy(ind->obj_name, obj_name, (GOEP_MAX_OBJ_NAME-2));
            ind->obj_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ind->obj_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ind->obj_name, obj_name, obj_name_len);
            ind->obj_name[obj_name_len] = 0;
            ind->obj_name[obj_name_len+1] = 0;
        }
    }

    btmtk_os_memset(ind->obj_mime_type, 0, sizeof(ind->obj_mime_type));

    if(obj_mime_type != NULL)
    {
        if(obj_mime_type_len >= GOEP_MAX_MIME_TYPE-1)
        {
            btmtk_os_memcpy(ind->obj_mime_type, obj_mime_type, (GOEP_MAX_MIME_TYPE-2));
            ind->obj_mime_type[GOEP_MAX_MIME_TYPE-2] = 0;
            ind->obj_mime_type[GOEP_MAX_MIME_TYPE-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ind->obj_mime_type, obj_mime_type, obj_mime_type_len);
            ind->obj_mime_type[obj_mime_type_len] = 0;
            ind->obj_mime_type[obj_mime_type_len+1] = 0;
        }
    }

     btmtk_os_memset(ind->uuid, 0, sizeof(ind->uuid));

    //if((uuid_p != NULL)&&(uuid_len != 0))
    //    btmtk_os_memcpy(ind->uuid, uuid_p, uuid_len);

    //ind->uuid_len = uuid_len;
    
    Service = ftp_find_server_with_goep_conn_id(goep_conn_id);
    GOEP_ServerResetPushNameInfo(Service); /* TODO: check this */

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_PUSHFILE_IND, ind , sizeof(bt_ftps_pushfile_ind_struct) );
}


/*****************************************************************************
* FUNCTION
*  ftps_send_pull_ind_msg
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
void ftps_send_pull_ind_msg(U8 goep_conn_id, goep_pkt_type_enum pkt_type,
                            U8* obj_mime_type, U8* obj_name, U16 obj_name_len,
                            U8* uuid_p, U8 uuid_len)
{
    ftps_pull_ind_struct* ftp_pull_ind = NULL;

    ftp_pull_ind = (ftps_pull_ind_struct*)
        construct_local_para(sizeof(ftps_pull_ind_struct), TD_CTRL);
    ASSERT(ftp_pull_ind != NULL);

    ftp_pull_ind->goep_conn_id = goep_conn_id;
    ftp_pull_ind->pkt_type = pkt_type;
    if(obj_mime_type != NULL)
        bt_strncpy((char*)ftp_pull_ind->obj_mime_type, (char*)obj_mime_type, GOEP_MAX_MIME_TYPE);
    else
        ftp_pull_ind->obj_mime_type[0] = 0;

    btmtk_os_memset(ftp_pull_ind->obj_name, 0, sizeof(ftp_pull_ind->obj_name));

    if(obj_name!= NULL)
    {
        if(obj_name_len >= (GOEP_MAX_OBJ_NAME-1) )
        {
            btmtk_os_memcpy(ftp_pull_ind->obj_name, obj_name, (GOEP_MAX_OBJ_NAME-2));
            ftp_pull_ind->obj_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ftp_pull_ind->obj_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ftp_pull_ind->obj_name, obj_name, obj_name_len);
            ftp_pull_ind->obj_name[obj_name_len] = 0;
            ftp_pull_ind->obj_name[obj_name_len+1] = 0;
        }
    }

    btmtk_os_memset(ftp_pull_ind->uuid, 0, sizeof(ftp_pull_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_pull_ind->uuid, uuid_p, uuid_len);

    ftp_pull_ind->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_PULL_IND, ftp_pull_ind , sizeof(ftps_pull_ind_struct) );

}

/*****************************************************************************
* FUNCTION
*  ftps_send_pull_ind_msg
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
void ftps_send_pullfile_ind_msg(U8 goep_conn_id,
                            U8* obj_mime_type, U8* obj_name, U16 obj_name_len,
                            U8 srm_enable)
{
    bt_ftps_pullfile_ind_struct* ind = NULL;

    ind = (bt_ftps_pullfile_ind_struct*)
        construct_local_para(sizeof(bt_ftps_pullfile_ind_struct), TD_CTRL);
    ASSERT(ind != NULL);

    ind->goep_conn_id = goep_conn_id;
    ind->srm_enable = srm_enable;
    if(obj_mime_type != NULL)
        bt_strncpy((char*)ind->obj_mime_type, (char*)obj_mime_type, GOEP_MAX_MIME_TYPE);
    else
        ind->obj_mime_type[0] = 0;

    btmtk_os_memset(ind->obj_name, 0, sizeof(ind->obj_name));

    if(obj_name!= NULL)
    {
        if(obj_name_len >= GOEP_MAX_OBJ_NAME-1)
        {
            btmtk_os_memcpy(ind->obj_name, obj_name, (GOEP_MAX_OBJ_NAME-2));
            ind->obj_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ind->obj_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ind->obj_name, obj_name, obj_name_len);
            ind->obj_name[obj_name_len] = 0;
            ind->obj_name[obj_name_len+1] = 0;
        }
    }

    btmtk_os_memset(ind->uuid, 0, sizeof(ind->uuid));

//    if((uuid_p != NULL)&&(uuid_len != 0))
//        btmtk_os_memcpy(ind->uuid, uuid_p, uuid_len);

    //ind->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_PULLFILE_IND, ind , sizeof(bt_ftps_pullfile_ind_struct) );

}

/*****************************************************************************
* FUNCTION
*  ftps_send_pushfile_rsp_ind_msg
* DESCRIPTION
*  This function is to send push indication message of final result
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code         IN   the final result
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_send_pushfile_rsp_ind_msg(U8 goep_conn_id, U8 rsp_code, const U8 *tmppath, const U8 *finalpath , U32 totalsize){
    bt_ftps_pushfile_rsp_ind_struct* ind = NULL;
	S32 result;

    ind = (bt_ftps_pushfile_rsp_ind_struct*)
        construct_local_para(sizeof(bt_ftps_pushfile_rsp_ind_struct), TD_CTRL);
    ASSERT(ind != NULL);
    ind->goep_conn_id = goep_conn_id;
	ind->rsp_code = rsp_code;
    ind->total_obj_len = totalsize;

	// rename the file when receiveing pushfile_rsp_msg

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_PUSHFILE_RSP_IND, ind , sizeof(bt_ftps_pushfile_rsp_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_send_pullfile_rsp_ind_msg
* DESCRIPTION
*  This function is to send pull indication message of final result
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code         IN   the final result
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_send_pullfile_rsp_ind_msg(U8 goep_conn_id, U8 rsp_code ){
    bt_ftps_pullfile_rsp_ind_struct* ind = NULL;

    ind = (bt_ftps_pullfile_rsp_ind_struct*)
        construct_local_para(sizeof(bt_ftps_pullfile_rsp_ind_struct), TD_CTRL);
    ASSERT(ind != NULL);
    ind->goep_conn_id = goep_conn_id;
	ind->rsp_code = rsp_code;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_PULLFILE_RSP_IND, ind , sizeof(bt_ftps_pullfile_rsp_ind_struct) );
}


/*****************************************************************************
* FUNCTION
*  ftps_send_setfolder_ind_msg
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
void ftps_send_set_folder_ind_msg(U8 goep_conn_id, goep_set_folder_type_enum setpath_flag,
                            U8* folder_name, U16 folder_name_len, U8* uuid_p, U8 uuid_len)
{
    ftps_set_folder_ind_struct* ftp_set_folder_ind = NULL;

    ftp_set_folder_ind = (ftps_set_folder_ind_struct*)
        construct_local_para(sizeof(ftps_set_folder_ind_struct), TD_CTRL);
    ASSERT(ftp_set_folder_ind != NULL);

    ftp_set_folder_ind->goep_conn_id = goep_conn_id;
    ftp_set_folder_ind->setpath_flag = setpath_flag;

    btmtk_os_memset(ftp_set_folder_ind->folder_name, 0, sizeof(ftp_set_folder_ind->folder_name));

    if(folder_name!= NULL)
    {
        if(folder_name_len >= GOEP_MAX_OBJ_NAME-1)
        {
            btmtk_os_memcpy(ftp_set_folder_ind->folder_name, folder_name, (GOEP_MAX_OBJ_NAME-2));
            ftp_set_folder_ind->folder_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ftp_set_folder_ind->folder_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ftp_set_folder_ind->folder_name, folder_name, folder_name_len);
            ftp_set_folder_ind->folder_name[folder_name_len] = 0;
            ftp_set_folder_ind->folder_name[folder_name_len+1] = 0;
        }
    }

    btmtk_os_memset(ftp_set_folder_ind->uuid, 0, sizeof(ftp_set_folder_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_set_folder_ind->uuid, uuid_p, uuid_len);

    ftp_set_folder_ind->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_SET_FOLDER_IND, ftp_set_folder_ind , sizeof(ftps_set_folder_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_send_action_ind_msg
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
void ftps_send_action_ind_msg(U8 goep_conn_id, U8 action, U8 permission, 
                            U8* folder_name, U16 folder_name_len, U8* dst_name, U16 dst_name_len, U8* uuid_p, U8 uuid_len)
{
    ftps_action_ind_struct* ftps_action_ind = NULL;

    ftps_action_ind = (ftps_action_ind_struct*)
        construct_local_para(sizeof(ftps_action_ind_struct), TD_CTRL);
    ASSERT(ftps_action_ind != NULL);

    ftps_action_ind->goep_conn_id = goep_conn_id;
	ftps_action_ind->action_id = action;
	ftps_action_ind->permission = permission; 
    //ftps_action_ind->setpath_flag = setpath_flag;

    btmtk_os_memset(ftps_action_ind->folder_name, 0, sizeof(ftps_action_ind->folder_name));

    if(folder_name!= NULL)
    {
        if(folder_name_len >= GOEP_MAX_OBJ_NAME-1)
        {
            btmtk_os_memcpy(ftps_action_ind->folder_name, folder_name, (GOEP_MAX_OBJ_NAME-2));
            ftps_action_ind->folder_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ftps_action_ind->folder_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ftps_action_ind->folder_name, folder_name, folder_name_len);
            ftps_action_ind->folder_name[folder_name_len] = 0;
            ftps_action_ind->folder_name[folder_name_len+1] = 0;
        }
    }
    if(dst_name!= NULL)
    {
        if(dst_name_len >= (GOEP_MAX_OBJ_NAME-1))
        {
            btmtk_os_memcpy(ftps_action_ind->dst_name, dst_name, (GOEP_MAX_OBJ_NAME-2));
            ftps_action_ind->dst_name[GOEP_MAX_OBJ_NAME-2] = 0;
            ftps_action_ind->dst_name[GOEP_MAX_OBJ_NAME-1] = 0;
        }    
        else
        {
            btmtk_os_memcpy(ftps_action_ind->dst_name, dst_name, dst_name_len);
            ftps_action_ind->dst_name[dst_name_len] = 0;
            ftps_action_ind->dst_name[dst_name_len+1] = 0;
        }
    }    

    btmtk_os_memset(ftps_action_ind->uuid, 0, sizeof(ftps_action_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftps_action_ind->uuid, uuid_p, uuid_len);

    ftps_action_ind->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_ACTION_IND, ftps_action_ind , sizeof(ftps_action_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_abort_ind_msg
* DESCRIPTION
*  This function is to send abort indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_send_abort_ind_msg(U8 goep_conn_id, U8* uuid_p, U8 uuid_len)
{
    ftps_abort_ind_struct* ftps_abort_ind = NULL;

    ftps_abort_ind = (ftps_abort_ind_struct*)
        construct_local_para(sizeof(ftps_abort_ind_struct), TD_CTRL);
    ASSERT(ftps_abort_ind != NULL);

    ftps_abort_ind->goep_conn_id = goep_conn_id;

    btmtk_os_memset(ftps_abort_ind->uuid, 0, sizeof(ftps_abort_ind->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftps_abort_ind->uuid, uuid_p, uuid_len);

    ftps_abort_ind->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_ABORT_IND, ftps_abort_ind , sizeof(ftps_abort_ind_struct) );
}


/*****************************************************************************
* FUNCTION
*  ftps_send_register_cnf_msg
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
void ftps_send_register_cnf_msg(U32 cnf_msg_id, U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len, U8 req_id)
{
    ftps_register_server_cnf_struct* register_server_cnf = NULL;

    register_server_cnf = (ftps_register_server_cnf_struct*)
        construct_local_para(sizeof(ftps_register_server_cnf_struct), TD_CTRL);
    ASSERT(register_server_cnf != NULL);

    btmtk_os_memset(register_server_cnf->uuid, 0, sizeof(register_server_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(register_server_cnf->uuid, uuid_p, uuid_len);

    register_server_cnf->uuid_len = uuid_len;
    register_server_cnf->req_id = req_id;
    register_server_cnf->goep_conn_id = goep_conn_id;
    register_server_cnf->rsp_code = (U8)rsp_code;

    FTP_APPROFILE_DISPATCH( cnf_msg_id, register_server_cnf , sizeof(ftps_register_server_cnf_struct) );

}

/*****************************************************************************
* FUNCTION
*  ftpc_send_connect_cnf_msg
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
void ftpc_send_connect_cnf_msg(U8 req_id, U8 goep_conn_id, ObStatus rsp_code, U16 peer_mru, U8 tptype, U8 obexver)
{
    ftpc_connect_cnf_struct* ftpc_connect_cnf = NULL;
    GoepClientApp* client;
    ObexClientApp* ObexApp;
#if BT_GOEP_V2 == XA_ENABLED
	U8 idx;
#endif

    ftpc_connect_cnf = (ftpc_connect_cnf_struct*)
        construct_local_para(sizeof(ftpc_connect_cnf_struct), TD_CTRL);
    ASSERT(ftpc_connect_cnf != NULL);

    ftpc_connect_cnf->req_id = req_id;

    btmtk_os_memset(ftpc_connect_cnf->uuid, 0, sizeof(ftpc_connect_cnf->uuid));

    //if((uuid_p != NULL)&&(uuid_len != 0))
    //    btmtk_os_memcpy(ftpc_connect_cnf->uuid, uuid_p, uuid_len);

    //ftpc_connect_cnf->uuid_len = uuid_len;
    ftpc_connect_cnf->rsp_code = (U8)rsp_code;
    ftpc_connect_cnf->tptype = (U8)tptype;
    ftpc_connect_cnf->obexver = (U8)obexver;

    kal_trace(BT_TRACE_G6_OBEX,GOEP_SEND_CONNECT_RSP, rsp_code);

    if((rsp_code == OB_STATUS_SUCCESS) || (rsp_code == OB_STATUS_PENDING))
    {
        client = (GoepClientApp *)ftp_find_client_with_goep_conn_id(goep_conn_id);
        ASSERT(client != NULL);
        ObexApp = &(GOEC(clients)[client->connId].obc);
        ftpc_connect_cnf->goep_conn_id = goep_conn_id;
        client->obex_conn_is_connected = KAL_TRUE;
        client->cm_conn_id = (U32) BTBMProfileAllocateConnIndex(ObexApp->trans.ObexClientBtTrans.target_uuid, ObexApp->trans.ObexClientBtTrans.remoteDevice->bdAddr);
        ftpc_connect_cnf->cm_conn_id = client->cm_conn_id;
        ftpc_connect_cnf->peer_mru = peer_mru;
        kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_ALLOC_CONNID, client->cm_conn_id);
    }
    else
    {
        client = (GoepClientApp *)ftp_find_client_with_req_id(req_id);
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
        GOEP_DeregisterClient(client);
        if(client->connect_req.target)
            free_ctrl_buffer(client->connect_req.target);
        
#if BT_GOEP_V2 == XA_ENABLED
        if( NULL != client->pl2capRawData[0] && 0 != client->l2capRawDataNum ){
            for( idx = 0; idx < client->l2capRawDataNum; idx++){
                ASSERT( client->pl2capRawData[i] );
                free_ctrl_buffer(client->pl2capRawData[idx]);
                client->pl2capRawData[idx] = NULL;
            }
            client->l2capRawDataNum = 0;
        }
#endif
        free_clientapp_ctrl_buffer(client);
        }
    }

SND_MSG:
    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_CONNECT_CNF, ftpc_connect_cnf , sizeof(ftpc_connect_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_push_cnf_msg
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
void ftpc_send_push_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len)
{
    ftpc_push_cnf_struct* ftp_push_cnf = NULL;

    ftp_push_cnf = (ftpc_push_cnf_struct*)
        construct_local_para(sizeof(ftpc_push_cnf_struct), TD_CTRL);
    ASSERT(ftp_push_cnf != NULL);

    ftp_push_cnf->goep_conn_id = goep_conn_id;
    ftp_push_cnf->rsp_code = (U8)rsp_code;
    ftp_push_cnf->reason = reason;

    btmtk_os_memset(ftp_push_cnf->uuid, 0, sizeof(ftp_push_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_push_cnf->uuid, uuid_p, uuid_len);

    ftp_push_cnf->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_PUSH_CNF, ftp_push_cnf , sizeof(ftpc_push_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_pushfile_cnf_msg
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
void ftpc_send_pushfile_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len)
{
    bt_ftpc_pushfile_cnf_struct* ftp_pushfile_cnf = NULL;
    GoepClientApp *Client = ftp_find_client_with_goep_conn_id(goep_conn_id);	

    ftp_pushfile_cnf = (bt_ftpc_pushfile_cnf_struct*)
        construct_local_para(sizeof(bt_ftpc_pushfile_cnf_struct), TD_CTRL);
    ASSERT(ftp_pushfile_cnf != NULL);

    ftp_pushfile_cnf->goep_conn_id = goep_conn_id;
    ftp_pushfile_cnf->rsp_code = (U8)rsp_code;
    ftp_pushfile_cnf->reason = reason;

	if(Client != NULL){
		ftpc_stop_aborttimer(Client);
	}	

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_PUSHFILE_CNF, ftp_pushfile_cnf , sizeof(bt_ftpc_pushfile_cnf_struct) );
}


/*****************************************************************************
* FUNCTION
*  ftpc_send_file_progress_ind_msg
* DESCRIPTION
*  This function is to send push/pull progress indication
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_send_file_progress_ind_msg(U8 goep_conn_id, U8 operation, U32 curr_length, U32 total_length)
{
    bt_ftpc_file_progress_ind_struct* ind = NULL;

    ind = (bt_ftpc_file_progress_ind_struct*)
        construct_local_para(sizeof(bt_ftpc_file_progress_ind_struct), TD_CTRL);
    ASSERT(ind != NULL);

    ind->goep_conn_id = goep_conn_id;
    ind->operation = (U8)operation;
    ind->total_length = total_length;
    ind->curr_length = curr_length;    

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_FILE_PROGRESS_IND, ind , sizeof(bt_ftpc_file_progress_ind_struct) );
}


/*****************************************************************************
* FUNCTION
*  ftps_send_file_progress_ind_msg
* DESCRIPTION
*  This function is to send push/pull progress indication
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code     IN   The response code from the OBEX server
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_send_file_progress_ind_msg(U8 goep_conn_id, U8 operation, U32 curr_length, U32 total_length)
{
    bt_ftps_file_progress_ind_struct* ind = NULL;

    ind = (bt_ftps_file_progress_ind_struct*)
        construct_local_para(sizeof(bt_ftps_file_progress_ind_struct), TD_CTRL);
    ASSERT(ind != NULL);

    ind->goep_conn_id = goep_conn_id;
    ind->operation = (U8)operation;
    ind->total_length = total_length;
    ind->curr_length = curr_length;    

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_FILE_PROGRESS_IND, ind , sizeof(bt_ftps_file_progress_ind_struct) );
}


/*****************************************************************************
* FUNCTION
*  ftpc_send_pull_cnf_msg
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
void ftpc_send_pull_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, goep_pkt_type_enum pkt_type, 
                            U8* obj_mime_type, U32 total_obj_len, U8* frag_ptr, 
                            U16 frag_len, U8* uuid_p, U8 uuid_len)
{
    ftpc_pull_cnf_struct* ftp_pull_cnf = NULL;

    ftp_pull_cnf = (ftpc_pull_cnf_struct*)
        construct_local_para(sizeof(ftpc_pull_cnf_struct), TD_CTRL);
    ASSERT(ftp_pull_cnf != NULL);

    ftp_pull_cnf->rsp_code = (U8)rsp_code;
    ftp_pull_cnf->reason = reason;
    
    btmtk_os_memset(ftp_pull_cnf->uuid, 0, sizeof(ftp_pull_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_pull_cnf->uuid, uuid_p, uuid_len);

    ftp_pull_cnf->uuid_len = uuid_len;
    ftp_pull_cnf->goep_conn_id = goep_conn_id;
    ftp_pull_cnf->pkt_type = pkt_type;


    if((pkt_type == GOEP_FIRST_PKT)||(pkt_type == GOEP_SINGLE_PKT))
    {
        ftp_pull_cnf->total_obj_len = total_obj_len;
    }
    ftp_pull_cnf->frag_ptr = frag_ptr;
    ftp_pull_cnf->frag_len = frag_len;

    /* added for continue parsing large folder content */
    ftp_pull_cnf->to_self = FALSE;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_PULL_CNF, ftp_pull_cnf , sizeof(ftpc_pull_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_pullfile_cnf_msg
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
void ftpc_send_pullfile_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason,
                            U8* obj_mime_type, U32 total_obj_len, U8* uuid_p, U8 uuid_len)
{
    bt_ftpc_pullfile_cnf_struct* cnf = NULL;
    GoepClientApp *Client = ftp_find_client_with_goep_conn_id(goep_conn_id);

    cnf = (bt_ftpc_pullfile_cnf_struct *)
        construct_local_para(sizeof(bt_ftpc_pullfile_cnf_struct), TD_CTRL);
    ASSERT(ftp_pull_cnf != NULL);

    cnf->rsp_code = (U8)rsp_code;
    cnf->reason = reason;
    
    cnf->goep_conn_id = goep_conn_id;
    cnf->total_obj_len = total_obj_len;

	if(Client != NULL){
		ftpc_stop_aborttimer(Client);
	}

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_PULLFILE_CNF, cnf , sizeof(bt_ftpc_pullfile_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_abortfile_cnf_msg
* DESCRIPTION
*  This function is to send OBEX fail response to remote
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code         IN   Accept the abort or not
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_send_abortfile_cnf_msg(U8 goep_conn_id, U8 rsp_code)
{
    bt_ftpc_abortfile_cnf_struct* cnf = NULL;
    GoepClientApp *Client = ftp_find_client_with_goep_conn_id(goep_conn_id);

    ASSERT(Client != NULL);

    cnf = (bt_ftpc_abortfile_cnf_struct*)
        construct_local_para(sizeof(bt_ftpc_abortfile_cnf_struct), TD_CTRL);
    ASSERT(cnf != NULL);
    cnf->goep_conn_id = goep_conn_id;
    if( rsp_code == 0 ){
    	rsp_code = GOEP_USER_CANCEL;
    }
    cnf->rsp_code = rsp_code;

	if(Client != NULL){
		ftpc_stop_aborttimer(Client);
	}	

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_ABORTFILE_CNF, cnf , sizeof(bt_ftpc_abortfile_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_set_folder_cnf_msg
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
void ftpc_send_set_folder_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len)
{
    ftpc_set_folder_cnf_struct* ftp_setfolder_cnf = NULL;

    ftp_setfolder_cnf = (ftpc_set_folder_cnf_struct*)
        construct_local_para(sizeof(ftpc_set_folder_cnf_struct), TD_CTRL);
    ASSERT(ftp_setfolder_cnf != NULL);

    ftp_setfolder_cnf->goep_conn_id = goep_conn_id;
    ftp_setfolder_cnf->rsp_code = (U8)rsp_code;
    ftp_setfolder_cnf->reason = reason;

    btmtk_os_memset(ftp_setfolder_cnf->uuid, 0, sizeof(ftp_setfolder_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_setfolder_cnf->uuid, uuid_p, uuid_len);

    ftp_setfolder_cnf->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_SET_FOLDER_CNF, ftp_setfolder_cnf , sizeof(ftpc_set_folder_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_set_folder_cnf_msg
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
void ftpc_send_action_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len)
{
    ftpc_action_cnf_struct* ftp_action_cnf = NULL;

    ftp_action_cnf = (ftpc_action_cnf_struct*)
        construct_local_para(sizeof(ftpc_action_cnf_struct), TD_CTRL);
    ASSERT(ftp_action_cnf != NULL);

    ftp_action_cnf->goep_conn_id = goep_conn_id;
    ftp_action_cnf->rsp_code = (U8)rsp_code;
    ftp_action_cnf->reason = reason;

    btmtk_os_memset(ftp_action_cnf->uuid, 0, sizeof(ftp_action_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_action_cnf->uuid, uuid_p, uuid_len);

    ftp_action_cnf->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_ACTION_CNF, ftp_action_cnf , sizeof(ftpc_action_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftpc_send_abort_cnf_msg
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
void ftpc_send_abort_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len)
{
    ftpc_abort_cnf_struct* ftp_abort_cnf = NULL;

    ftp_abort_cnf = (ftpc_abort_cnf_struct*)
        construct_local_para(sizeof(ftpc_abort_cnf_struct), TD_CTRL);
    ASSERT(ftp_abort_cnf != NULL);

    ftp_abort_cnf->goep_conn_id = goep_conn_id;
    ftp_abort_cnf->rsp_code = (U8)rsp_code;

    btmtk_os_memset(ftp_abort_cnf->uuid, 0, sizeof(ftp_abort_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_abort_cnf->uuid, uuid_p, uuid_len);

    ftp_abort_cnf->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH ( MSG_ID_BT_FTPC_ABORT_CNF, ftp_abort_cnf , sizeof(ftpc_abort_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_send_auth_cnf_msg
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
void ftpc_send_auth_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len)
{
    ftps_auth_cnf_struct* ftp_auth_cnf = NULL;

    ftp_auth_cnf = (ftps_auth_cnf_struct*)
        construct_local_para(sizeof(ftps_auth_cnf_struct), TD_CTRL);
    ASSERT(ftp_auth_cnf != NULL);

    ftp_auth_cnf->goep_conn_id = goep_conn_id;
    ftp_auth_cnf->rsp_code = (U8)rsp_code;

    btmtk_os_memset(ftp_auth_cnf->uuid, 0, sizeof(ftp_auth_cnf->uuid));

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_auth_cnf->uuid, uuid_p, uuid_len);

    ftp_auth_cnf->uuid_len = uuid_len;

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_AUTH_CNF, ftp_auth_cnf , sizeof(ftps_auth_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_send_auth_ind_msg
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
void ftpc_send_auth_ind_msg(U8 goep_conn_id, U8* realm, U8 realm_len, U8* uuid_p, U8 uuid_len)
{
    ftps_auth_ind_struct* ftp_auth_ind = NULL;

    ObexTpConnInfo      tpInfo;
    kal_bool success;
    ObexAppHandle* obexapphdl_p;
    char* dev_name = 0;
    GoepServerApp *Service = NULL;
	GoepClientApp *Client = NULL;
    goep_bd_addr_struct ftp_bd_addr;

	Service = ftp_find_server_with_goep_conn_id(goep_conn_id);
    if (Service == NULL)
	{
		Client = ftp_find_client_with_goep_conn_id(goep_conn_id);

		ASSERT(Client != NULL);
	}

    ftp_auth_ind = (ftps_auth_ind_struct*)
        construct_local_para(sizeof(ftps_auth_ind_struct), TD_CTRL);
    ASSERT(ftp_auth_ind != NULL);

    ftp_auth_ind->goep_conn_id = goep_conn_id;

    btmtk_os_memset(ftp_auth_ind->realm, 0, sizeof(ftp_auth_ind->realm));

    if((realm != NULL)&&(realm_len != 0))
        btmtk_os_memcpy(ftp_auth_ind->realm, realm, realm_len);

    ftp_auth_ind->realm_len = realm_len;

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_auth_ind->uuid, uuid_p, uuid_len);

    ftp_auth_ind->uuid_len = uuid_len;


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

    ftp_bd_addr_array2struct(&(tpInfo.remDev->bdAddr), &ftp_bd_addr);

    dev_name = (char *)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr)) ;

    btmtk_os_memset(ftp_auth_ind->dev_name, 0, sizeof(ftp_auth_ind->dev_name));

    if(dev_name != NULL)
        bt_strncpy((char*)ftp_auth_ind->dev_name, (char*)dev_name, GOEP_MAX_DEV_NAME);
	
	if( NULL != Service  ){
    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_AUTH_IND, ftp_auth_ind , sizeof(ftps_auth_ind_struct) );
	}else if( NULL != Client ){
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_AUTH_IND, ftp_auth_ind , sizeof(ftps_auth_ind_struct) );
	}else{
 		Assert(0);
	}
}

/*****************************************************************************
* FUNCTION
*  ftps_send_auth_ind_msg
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
void ftps_send_auth_ind_msg(U8 goep_conn_id, U8* realm, U8 realm_len, U8* uuid_p, U8 uuid_len)
{
    ftps_auth_ind_struct* ftp_auth_ind = NULL;

    ObexTpConnInfo      tpInfo;
    kal_bool success;
    ObexAppHandle* obexapphdl_p;
    char* dev_name = 0;
    GoepServerApp *Service = NULL;
	GoepClientApp *Client = NULL;
    goep_bd_addr_struct ftp_bd_addr;

	Service = ftp_find_server_with_goep_conn_id(goep_conn_id);
    if (Service == NULL)
	{
		Client = ftp_find_client_with_goep_conn_id(goep_conn_id);

		ASSERT(Client != NULL);
	}

    ftp_auth_ind = (ftps_auth_ind_struct*)
        construct_local_para(sizeof(ftps_auth_ind_struct), TD_CTRL);
    ASSERT(ftp_auth_ind != NULL);

    ftp_auth_ind->goep_conn_id = goep_conn_id;

    btmtk_os_memset(ftp_auth_ind->realm, 0, sizeof(ftp_auth_ind->realm));

    if((realm != NULL)&&(realm_len != 0))
        btmtk_os_memcpy(ftp_auth_ind->realm, realm, realm_len);

    ftp_auth_ind->realm_len = realm_len;

    if((uuid_p != NULL)&&(uuid_len != 0))
        btmtk_os_memcpy(ftp_auth_ind->uuid, uuid_p, uuid_len);

    ftp_auth_ind->uuid_len = uuid_len;


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

    ftp_bd_addr_array2struct(&(tpInfo.remDev->bdAddr), &ftp_bd_addr);

    dev_name = (char *)BTBMGetLocalStoredName((U8*)&(tpInfo.remDev->bdAddr)) ;

    btmtk_os_memset(ftp_auth_ind->dev_name, 0, sizeof(ftp_auth_ind->dev_name));

    if(dev_name != NULL)
        bt_strncpy((char*)ftp_auth_ind->dev_name, (char*)dev_name, GOEP_MAX_DEV_NAME);
	
	if( NULL != Service  ){
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_AUTH_IND, ftp_auth_ind , sizeof(ftps_auth_ind_struct) );
	}else if( NULL != Client ){
		FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_AUTH_IND, ftp_auth_ind , sizeof(ftps_auth_ind_struct) );
	}else{
 		Assert(0);
	}
}

/*****************************************************************************
* FUNCTION
*  ftps_send_authorize_ind_msg
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
void ftps_send_authorize_ind_msg(U8 goep_conn_id, goep_bd_addr_struct* bd_addr_p, U8* dev_name)
{
    ftps_authorize_ind_struct* ftp_authorize_ind = NULL;
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(goep_conn_id);

    ASSERT(Service != NULL);

    ftp_authorize_ind = (ftps_authorize_ind_struct*)
        construct_local_para(sizeof(ftps_authorize_ind_struct), TD_CTRL);
    ASSERT(ftp_authorize_ind != NULL);

    if(Service->target[0])
    {
        ftp_authorize_ind->uuid_len = Service->target[0]->targetLen;
    
        btmtk_os_memset(ftp_authorize_ind->uuid, 0, sizeof(ftp_authorize_ind->uuid));
    
        if((Service->target[0]->target != NULL)&&(ftp_authorize_ind->uuid_len != 0))
            btmtk_os_memcpy(ftp_authorize_ind->uuid, Service->target[0]->target, Service->target[0]->targetLen);

        ftp_authorize_ind->uuid_len = Service->target[0]->targetLen;
    }
    else
    {
        ftp_authorize_ind->uuid_len = 0;
        ftp_authorize_ind->uuid[0] = 0;
    }

    ftp_authorize_ind->goep_conn_id = goep_conn_id;
    ftp_authorize_ind->bd_addr = *bd_addr_p;

    btmtk_os_memset(ftp_authorize_ind->dev_name, 0, sizeof(ftp_authorize_ind->dev_name));
    
    if(dev_name != NULL)
        bt_strncpy((char*)ftp_authorize_ind->dev_name, (char*)dev_name, sizeof(ftp_authorize_ind->dev_name) );

    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_AUTHORIZE_IND, ftp_authorize_ind , sizeof(ftps_authorize_ind_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftps_send_abortfile_cnf_msg
* DESCRIPTION
*  This function is to send OBEX fail response to remote
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
*  rsp_code         IN   Accept the abort or not
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_send_abortfile_cnf_msg(U8 goep_conn_id, U8 rsp_code)
{
    bt_ftps_abortfile_cnf_struct* cnf = NULL;
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(goep_conn_id);

    ASSERT(Service != NULL);

    cnf = (bt_ftps_abortfile_cnf_struct*)
        construct_local_para(sizeof(bt_ftps_abortfile_cnf_struct), TD_CTRL);
    ASSERT(cnf != NULL);


    FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_ABORTFILE_CNF, cnf , sizeof(bt_ftps_abortfile_cnf_struct) );
}

/*****************************************************************************
* FUNCTION
*  ftp_send_opp_supported_formats_ind_msg
* DESCRIPTION
*  This function is to send OPP client OPP server supported format list of SDP result indication message to the application
* PARAMETERS
*  goep_conn_id     IN   The GOEP connection id
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftp_send_opp_supported_formats_ind_msg(U8 goep_conn_id, U8 supported_list)
{

}



/*****************************************************************************
* FUNCTION
*  ftps_register_server_req_hdlr 
* DESCRIPTION
*  This function is to handle the OBEX server registration
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_register_server_req_hdlr(ftps_register_server_req_struct* register_server_req)
{
    //GoepServerApp *Service = get_ctrl_buffer(sizeof(GoepServerApp));
    GoepServerApp *Service =  (GoepServerApp *)get_ctrl_buffer(sizeof(GoepServerApp));
    ObStatus rsp_code;
    ObexAppHandle* obexapphdlr_p;
//    GoepTpType tptype = GOEP_TP_RFCOMM_ONLY;
#if BT_GOEP_V2 == XA_ENABLED
	U8 idx;
#endif

    ASSERT(Service != NULL);

    btmtk_os_memset( (U8 *) Service, 0, sizeof(GoepServerApp));

#if BT_GOEP_V2 == XA_ENABLED
    /* range check */
    if(!btGetCustvalue(CUSTID_HIGH_SPEED_SUPPORT)){
        register_server_req->tptype = OBEX_TP_RFCOMM_ONLY;
    }
    if( register_server_req->tptype > 3 ){
        register_server_req->tptype = OBEX_TP_BOTH;
    }
//    GOEP_Report("[ERR] ftps_register_server_req_hdlr change to 2 only");
//    register_server_req->tptype = OBEX_TP_L2CAP_ONLY;
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

        ObexSdpDB_RegistrationEx(SC_OBEX_FILE_TRANSFER, register_server_req->tptype); //will casue connect fail
    }
    else
    {
        Service->numTargets = 0;
        Service->target[0] = NULL;
        Service->bt_service = GOEP_PROFILE_OPUSH;
        Service->type = GOEP_PROFILE_OPUSH; /* We allow only one service to be registered in one OBEX channel */

        ObexSdpDB_RegistrationEx(SC_OBEX_OBJECT_PUSH, register_server_req->tptype); //will casue connect fail
    }

    /* We do not allow existing connection to be re-used */
    /* the exact connection id will be assigned in GOEP_RegisterServer() */
    Service->connFlags = GOEP_NEW_CONN;
    Service->connId = 0;
    Service->callback = NULL;
    Service->cm_conn_id = GOEP_INVALID_CMID;
    Service->callback = FtpGoepServerCallback;

#if BT_GOEP_V2 == XA_ENABLED
    /* enable the GOEP/L2cap */
	OS_Report("[FTP][SERV] prepare the l2data buffer");
    Service->l2data.bSRM = FALSE;
	Service->l2data.l2capRawDataNum = 0;
    Service->l2data.tpType = register_server_req->tptype;
	if( register_server_req->tptype == OBEX_TP_BOTH || register_server_req->tptype == OBEX_TP_L2CAP_ONLY ){
		Service->l2data.bSRM = TRUE;
    Service->l2data.l2capRawDataNum = GOEPL2_MAX_RETRANS_NUM;
	if( 0 != Service->l2data.l2capRawDataNum ){
		for( idx = 0; idx < Service->l2data.l2capRawDataNum; idx++ ){
			Service->l2data.pl2capRawData[idx] = get_ctrl_buffer(L2CAP_MAXIMUM_MTU+20);
			ASSERT( NULL != Service->pl2capRawData[idx] );
		}
	}
	}
#endif

    rsp_code = GOEP_RegisterServer(Service, &FtpStoreFuncTable);    /* service connection id must be assigned after here */

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
        /// send register result back
        ftps_send_register_cnf_msg( MSG_ID_BT_FTPS_REGISTER_SERVER_CNF, 0, rsp_code, register_server_req->uuid, register_server_req->uuid_len, register_server_req->req_id);
        return;
    }
    obexapphdlr_p = &(GOES(servers)[Service->connId].obs.handle);
    if( NULL == Service->callback ){
        // Use old style. Use buffer to accept data
        GOES(servers)[Service->connId].obs.goep_buff_ptr = register_server_req->buf_ptr;
    }
    obexapphdlr_p->mru = register_server_req->buf_size;
    if(register_server_req->need_auth)
    {
#if BT_SECURITY == XA_ENABLED
        GOEP_RegisterServerSecurityRecord(Service, BSL_AUTHENTICATION_IN);
#endif
    }

    ftps_save_goep_conn_id(GOES(servers)[Service->connId].obs.goep_conn_id);
    /// send register result back
    ftps_send_register_cnf_msg( MSG_ID_BT_FTPS_REGISTER_SERVER_CNF, GOES(servers)[Service->connId].obs.goep_conn_id, OB_STATUS_SUCCESS, register_server_req->uuid, register_server_req->uuid_len, register_server_req->req_id);
}

/*****************************************************************************
* FUNCTION
*  ftps_deregister_server_req_hdlr 
* DESCRIPTION
*  This function is to deregister the OBEX server application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_deregister_server_req_hdlr(ftps_deregister_server_req_struct* deregister_server_req) 
{
    ObStatus rsp_code = OB_STATUS_SUCCESS;
    GoepServerApp *Service = NULL;
    S32 idx = 0;

    GOEP_Report("[FTP]ftps_deregister_server_req_hdlr, goep_conn_id:0x%x", deregister_server_req->goep_conn_id);	
    if (deregister_server_req->goep_conn_id != 0xFE)
    {
        Service = ftp_find_server_with_goep_conn_id(deregister_server_req->goep_conn_id);

        if(Service == NULL)
        {
            ASSERT(0);
            return;
        }

    	ObexSdpDB_DeRegistration(SC_OBEX_FILE_TRANSFER);
    	rsp_code = GOEP_DeregisterServer(Service);
    	if(rsp_code != OB_STATUS_SUCCESS)
    	{
    	    /* The failed result is not allowed */
    	    ASSERT(0);
    	}
    	else
    	{
            if(Service->target[0]){
    	       /// send deregister result back
    	       ftp_send_cnf_msg( MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF, deregister_server_req->goep_conn_id, rsp_code, Service->target[0]->target, Service->target[0]->targetLen);
            }else{
               /// send deregister result back
               ftp_send_cnf_msg( MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF, deregister_server_req->goep_conn_id, rsp_code, NULL, 0);
            }

            if(Service)
            {
    			if( Service->l2data.l2capRawDataNum > 0 ){
    				for( idx = 0; idx < Service->l2data.l2capRawDataNum; idx++ ){
    					free_ctrl_buffer( Service->l2data.pl2capRawData[idx] );
    					Service->l2data.pl2capRawData[idx] = 0;
    				}
    			}
#if BT_SECURITY == XA_ENABLED
                GOEP_UnregisterServerSecurityRecord(Service);
#endif
                if(Service->target[0])
                {
                    if(Service->target[0]->target)
                        free_ctrl_buffer(Service->target[0]->target);
                    free_ctrl_buffer(Service->target[0]);
                }
                free_ctrl_buffer(Service);
            }
    	}

        ftps_delete_goep_conn_id(deregister_server_req->goep_conn_id);
    }
    else    /* deregister_server_req->goep_conn_id == 0xFE means deregister all using ftp server */
    {
        for (idx = 0; idx < GOEP_NUM_OBEX_SERVER_CONS; idx++)
        {
            if (ftps_goep_conn_id[idx] != 0xFE)
            {
                GOEP_Report("[FTP]ftps deregister all server, goep_conn_id:0x%x", ftps_goep_conn_id[idx]);
                
                Service = ftp_find_server_with_goep_conn_id(ftps_goep_conn_id[idx]);

                if(Service == NULL)
                {
                    ASSERT(0);
                    return;
                }

                ObexSdpDB_DeRegistration(SC_OBEX_FILE_TRANSFER);
            	rsp_code = GOEP_DeregisterServer(Service);
            	if(rsp_code != OB_STATUS_SUCCESS)
            	{
            	    /* The failed result is not allowed */
            	    ASSERT(0);
            	}
                else
                {
                    if(Service)
                    {
            			if( Service->l2data.l2capRawDataNum > 0 ){
            				for( idx = 0; idx < Service->l2data.l2capRawDataNum; idx++ ){
            					free_ctrl_buffer( Service->l2data.pl2capRawData[idx] );
            					Service->l2data.pl2capRawData[idx] = 0;
            				}
            			}
#if BT_SECURITY == XA_ENABLED
                        GOEP_UnregisterServerSecurityRecord(Service);
#endif
                        if(Service->target[0])
                        {
                            if(Service->target[0]->target)
                                free_ctrl_buffer(Service->target[0]->target);
                            free_ctrl_buffer(Service->target[0]);
                        }
                        free_ctrl_buffer(Service);
                    }
                }

                ftps_goep_conn_id[idx] = 0xFE;
            }
        }
        
        ftp_send_cnf_msg( MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF, deregister_server_req->goep_conn_id, rsp_code, NULL, 0);
    }
   
}
/*****************************************************************************
* FUNCTION
*  ftps_connect_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the connect response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_connect_rsp_hdlr(ftps_connect_rsp_struct* ftp_connect_rsp)
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_connect_rsp->goep_conn_id);
    ObStatus rsp_code;
#if GOEP_AUTH == XA_ENABLED
    ObexServerApp* ObexAppHandle;
    GoepServerObexCons *server;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
    ObexAuthResponse* Response;
    ObexAuthChallenge* Challenge;
    

    Challenge = NULL;
    Response = NULL;
    ObexAppHandle = &(GOES(servers)[Service->connId].obs);
#endif

    rsp_code = OB_STATUS_SUCCESS;

    if(Service != NULL)
    {
        /* Note that lack of MRU interface */
        if(ftp_connect_rsp->rsp_code == GOEP_STATUS_SUCCESS)
        {
            //GOEP_ServerAccept(Service, NULL);

#if GOEP_AUTH == XA_ENABLED
		server = ftp_find_ServerObexCons_with_goep_conn_id(ftp_connect_rsp->goep_conn_id);		
                if(Service->target[0])
                {
                    uuid_len = Service->target[0]->targetLen;
                    btmtk_os_memcpy(uuid, Service->target[0]->target, uuid_len);
                }
                else
                {
                    uuid[0]=0;
                    uuid_len =0;
					ftp_connect_rsp->client_pwd_len = ftp_connect_rsp->server_pwd_len = 0;
                }

		if( 0 != ftp_connect_rsp->server_pwd_len ){
			if(!(server->flags & GOEF_RESPONSE )){
				// dont received a response of server-challenge. make one to verify it
				if( ftp_connect_rsp->server_pwd_len >=  GOEP_MAX_PASSWD_SIZE){
					ftp_connect_rsp->server_pwd_len = GOEP_MAX_PASSWD_SIZE -1;
				}
				Challenge = get_ctrl_buffer(sizeof(ObexAuthChallenge));
				btmtk_os_memset( (U8 *) Challenge, 0, sizeof(ObexAuthChallenge));
				Challenge->realm = get_ctrl_buffer(GOEP_MAX_REALM_SIZE);
				if ( NULL != Service->password ){
					free_ctrl_buffer(Service->password);
				}
				Service->password = get_ctrl_buffer(GOEP_MAX_PASSWD_SIZE);
				btmtk_os_memset( (U8 *) Challenge->realm, 0, sizeof(GOEP_MAX_REALM_SIZE));
				btmtk_os_memset( (U8 *) Service->password, 0, sizeof(GOEP_MAX_PASSWD_SIZE));

				Challenge->options = 0;
				Challenge->realmLen = ftp_connect_rsp->server_realm_len;
				Service->passwordLen = ftp_connect_rsp->server_pwd_len;
				if((ftp_connect_rsp->server_pwd[0] != 0) && (ftp_connect_rsp->server_pwd != 0))
				    btmtk_os_memcpy( Service->password, ftp_connect_rsp->server_pwd, ftp_connect_rsp->server_pwd_len);

				if((ftp_connect_rsp->server_realm[0] != 0) && (ftp_connect_rsp->server_realm_len != 0))
				    btmtk_os_memcpy( Challenge->realm, ftp_connect_rsp->server_realm, GOEP_MAX_REALM_SIZE);
				Service->challenge = Challenge;
			}else{
				// a response of server-challenge has received
				if(GOEP_ServerVerifyAuthResponse(Service, ftp_connect_rsp->server_pwd, ftp_connect_rsp->server_pwd_len )){
					// verify with server_pwd successfully
					ftp_connect_rsp->rsp_code = GOEP_STATUS_SUCCESS;
					ftps_send_connect_ind_msg(ftp_connect_rsp->goep_conn_id, (U16)(server->obs.handle.parser.maxTxPacket-6), uuid, uuid_len, FALSE, TRUE, GOEP_ServerGetTpType(Service), GOEP_ServerGetRemoteVersion(Service));
				}else{
					// verify with server_pwd fail
					ftp_connect_rsp->rsp_code = GOEP_UNAUTHORIZED;
					
				}

			}
		}

		if( 0 != ftp_connect_rsp->client_pwd_len ){
			if( !(server->flags & GOEF_CHALLENGE ) ){
				// dont' make one
			}else{
				// make a response of client-challenge
				if( ftp_connect_rsp->client_pwd_len >=  GOEP_MAX_PASSWD_SIZE){
					ftp_connect_rsp->client_pwd_len = GOEP_MAX_PASSWD_SIZE -1;
				}
				Response = get_ctrl_buffer(sizeof(ObexAuthResponse));
				btmtk_os_memset( (U8 *)Response, 0, sizeof(ObexAuthResponse));

				Response->password = get_ctrl_buffer(GOEP_MAX_PASSWD_SIZE);
				btmtk_os_memcpy(Response->password, ftp_connect_rsp->client_pwd, ftp_connect_rsp->client_pwd_len);
				Response->passwordLen = ftp_connect_rsp->client_pwd_len;

				if( ftp_connect_rsp->client_userid_len >= GOEP_MAX_USERID_SIZE ){
					ftp_connect_rsp->client_userid_len = GOEP_MAX_USERID_SIZE -1;
				}
				Response->userId = get_ctrl_buffer(GOEP_MAX_USERID_SIZE);
				btmtk_os_memset( (U8 *)Response->userId, 0, GOEP_MAX_USERID_SIZE);
				Response->userIdLen = ftp_connect_rsp->client_userid_len;
				btmtk_os_memcpy(Response->userId, ftp_connect_rsp->client_userid, Response->userIdLen);
				Service->response = Response;
			}
		}

		rsp_code = OB_STATUS_SUCCESS;
		if( NULL != Challenge || NULL != Response ){
			rsp_code = GOEP_ServerAuthenticate(Service, Service->response, Service->challenge);
		}
		if( NULL != Challenge ){
			// Service->password will be free automatically
			free_ctrl_buffer(Challenge->realm);
			free_ctrl_buffer(Challenge);
			Service->challenge = NULL;
		}
		if( NULL != Response ){
			free_ctrl_buffer(Response->password);
			free_ctrl_buffer(Response->userId);
			free_ctrl_buffer(Response);
			Service->response = NULL;
			//if( OB_STATUS_SUCCESS == rsp_code ){
				// verity the server-challenge pass
			//	ftp_send_connect_ind_msg(ftp_connect_rsp->goep_conn_id, (U16)(server->obs.handle.parser.maxTxPacket-6), uuid, uuid_len, FALSE, TRUE);
			//}
            }

#endif
		if( OB_STATUS_SUCCESS == rsp_code && GOEP_STATUS_SUCCESS == ftp_connect_rsp->rsp_code){
			GOEP_ServerAccept(Service, NULL);
		}else{
			GOEP_ServerAbort(Service, ftp_connect_rsp->rsp_code);
		}

        }
        else
            GOEP_ServerAbort(Service, ftp_connect_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  ftps_authorize_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the authorization response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_authorize_rsp_hdlr(ftps_authorize_rsp_struct* ftp_authorize_rsp ) 
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_authorize_rsp->goep_conn_id);

    if(Service != NULL)
    {
        GOEP_ServerAuthorizeResponse(Service, ftp_authorize_rsp->rsp_code);
    }
}

/*****************************************************************************
* FUNCTION
*  ftps_push_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the push response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_push_rsp_hdlr(ftps_push_rsp_struct* ftp_push_rsp )
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_push_rsp->goep_conn_id);
    ObexServerApp* ObexAppHandle;
    
    if(Service != NULL)
    {
        ObexAppHandle = &(GOES(servers)[Service->connId].obs);
        ObexAppHandle->goep_buff_wrote_len = 0;

        if(ftp_push_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, ftp_push_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  ftps_pull_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the pull response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_pull_rsp_hdlr(ftps_pull_rsp_struct* ftp_pull_rsp) 
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_pull_rsp->goep_conn_id);
    ObexServerApp* ObexAppHandle;

    if(Service != NULL)
    {
        ObexAppHandle = &(GOES(servers)[Service->connId].obs);
        ObexAppHandle->goep_buff_wrote_len = 0;
        ObexAppHandle->goep_buff_size = ftp_pull_rsp->frag_len;

        if(ftp_pull_rsp->pkt_type == GOEP_FIRST_PKT || ftp_pull_rsp->pkt_type == GOEP_SINGLE_PKT)
        {
			GoepServerObexCons *server = &GOES(servers)[Service->connId];

            ObexAppHandle->server.objLen = ftp_pull_rsp->total_obj_len;
			server->currOp.info.pushpull.objectLen = ObexAppHandle->server.objLen;

            if(ftp_pull_rsp->rsp_code == GOEP_STATUS_SUCCESS)
                GOEP_ServerAccept(Service, NULL); 

        }

        if(ftp_pull_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, ftp_pull_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}

/*****************************************************************************
* FUNCTION
*  ftps_set_folder_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the set folder response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_set_folder_rsp_hdlr(ftps_set_folder_rsp_struct* ftp_set_folder_rsp) 
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_set_folder_rsp->goep_conn_id);

    if(Service != NULL)
    {
        if(ftp_set_folder_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, ftp_set_folder_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}


/*****************************************************************************
* FUNCTION
*  ftps_action_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the set folder response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_action_rsp_hdlr(ftps_action_rsp_struct* ftp_action_rsp)
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_action_rsp->goep_conn_id);

    if(Service != NULL)
    {
        if(ftp_action_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, ftp_action_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}


/*****************************************************************************
* FUNCTION
*  ftps_abort_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the abort response from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_abort_rsp_hdlr(ftps_abort_rsp_struct* ftp_abort_rsp) 
{
    GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_abort_rsp->goep_conn_id);

    if(Service != NULL)
    {
        if(ftp_abort_rsp->rsp_code != GOEP_STATUS_SUCCESS)
            GOEP_ServerAbort(Service, ftp_abort_rsp->rsp_code);

        GOEP_ServerContinue(Service);
    }
}



/*****************************************************************************
* FUNCTION
*  ftpc_auth_req_hdlr 
* DESCRIPTION
*  This function is to handle the authentication request from the OBEX server applciation
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_auth_req_hdlr(ftps_auth_req_struct* ftp_auth_req )
{
	ASSERT(0); // Useless, auth should use in connection
	
//	
//    ObexAuthChallenge* Challenge = get_ctrl_buffer(sizeof(ObexAuthChallenge));
//    U8 *auth_passwd = get_ctrl_buffer(GOEP_MAX_PASSWD_SIZE);
//    U8 *auth_realm = get_ctrl_buffer(GOEP_MAX_REALM_SIZE);
//    ObStatus rsp_code;
//
//    ASSERT((auth_passwd != NULL)&&(auth_realm != NULL)&&(Challenge != NULL));
//
//    /* Initialize the dynamically allocated memory */
//    btmtk_os_memset( (U8 *) Challenge, 0, sizeof(ObexAuthChallenge));
//    btmtk_os_memset( (U8 *) auth_passwd, 0, GOEP_MAX_PASSWD_SIZE);
//    btmtk_os_memset( (U8 *) auth_realm, 0, GOEP_MAX_REALM_SIZE);
//
//    if((ftp_auth_req->passwd[0] != 0) && (ftp_auth_req->passwd_len != 0))
//        btmtk_os_memcpy( auth_passwd, ftp_auth_req->passwd, GOEP_MAX_PASSWD_SIZE);
//
//    if((ftp_auth_req->realm_str[0] != 0) && (ftp_auth_req->realm_len != 0))
//        btmtk_os_memcpy( auth_realm, ftp_auth_req->realm_str, GOEP_MAX_REALM_SIZE);
//
//    Challenge->options = 0;
//    Challenge->realm = auth_realm;
//    Challenge->realmLen = ftp_auth_req->realm_len;
//
//    if(ftp_auth_req->ftp_role == GOEP_CLIENT_ROLE)
//    {
//        GoepClientApp *Client = ftp_find_client_with_goep_conn_id(ftp_auth_req->goep_conn_id);
//        if(Client != NULL)
//        {
//            Client->connect_req.challenge = Challenge;
//            Client->connect_req.password = auth_passwd;
//            Client->connect_req.passwordLen = ftp_auth_req->passwd_len;
//            rsp_code = GOEP_Connect(Client, &(Client->connect_req));
//            Client->connect_req.challenge = NULL;
//        }
//    }
//    else
//    {
//        GoepServerApp *Service = ftp_find_server_with_goep_conn_id(ftp_auth_req->goep_conn_id);
//
//        if(Service != NULL)
//        {
//            Service->challenge = Challenge;
//            Service->password = auth_passwd;
//            Service->passwordLen = ftp_auth_req->passwd_len;
//            rsp_code = GOEP_ServerAuthenticate(Service, NULL, Challenge);
//            //GOEP_Report(("ftpc_auth_req_hdlr: GOEP_ServerAuthenticate status %d\n", rsp_code));
//            rsp_code = GOEP_ServerContinue(Service);
//            //GOEP_Report(("ftpc_auth_req_hdlr: GOEP_ServerContinue status %d\n", rsp_code));
//            Service->challenge = NULL;
//        }
//        else
//        {
//            /* return failed result */
//            ftp_send_auth_cnf_msg(ftp_auth_req->goep_conn_id, OB_STATUS_FAILED, ftp_auth_req->uuid, ftp_auth_req->uuid_len);
//        }
//     }
//     /* free memory */
//     free_ctrl_buffer(auth_realm);
//     free_ctrl_buffer(Challenge);
}

/*****************************************************************************
* FUNCTION
*  ftpc_connect_req_hdlr 
* DESCRIPTION
*  This function is to handle the connect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_connect_req_hdlr(ftpc_connect_req_struct* ftpc_connect_req)
{
    GoepClientApp *Client = get_ctrl_buffer(sizeof(GoepClientApp));
    ObexClientApp* ObexApp;
    ObexTpAddr  obtpaddr;
    ObStatus rsp_code;
    int result;
#if OBEX_AUTHENTICATION == XA_ENABLED
	ObexAuthChallenge* Challenge;
	U8 *auth_passwd;
	U8 *auth_realm;
#endif
#if BT_GOEP_V2 == XA_ENABLED 
    U8 i;
    U8 j;
#endif 
    ASSERT(Client != NULL);
    
    btmtk_os_memset( (U8 *) Client, 0, sizeof(GoepClientApp));

    if(ftpc_connect_req->tp_type == GOEP_TP_BT)
    {
#if BT_GOEP_V2 == XA_ENABLED
        /* Init the L2cap enhance buffer */
        if( !btGetCustvalue(CUSTID_HIGH_SPEED_SUPPORT)){
            ftpc_connect_req->tptype = GOEP_TP_RFCOMM_ONLY;
            OS_Report("HighSpeed is switched off. change tptype to rfcomm only");
        }
        Client->target_type = ftpc_connect_req->tptype;
		
        switch( Client->target_type ){
            case GOEP_TP_L2CAP_ONLY:
            case GOEP_TP_BOTH:
                {
                    // construct l2cap enhance buffer
                    for(i = 0; i< GOEPL2_MAX_RETRANS_NUM; i++){
                        Client->pl2capRawData[i] = get_ctrl_buffer(L2CAP_MAXIMUM_MTU+20);
                        ASSERT( Client->pl2capRawData[i] != NULL );
			kal_trace(BT_TRACE_G6_OBEX, FTP_CLIENT_CREATE_BUFFER, &Client, Client->pl2capRawData[i]);
                        if( NULL == Client->pl2capRawData[i] ){
                            GOEP_Report("[GOEP][ERR] out of memory. error handle i:%d", i); 
                            for(j = 0; j < i; j++){
                                if( NULL != Client->pl2capRawData[j] ){
                                    free_ctrl_buffer(Client->pl2capRawData[j]);
                                    Client->pl2capRawData[j] = NULL;
                                }
                            }
                            ftpc_connect_req->tptype = GOEP_TP_RFCOMM_ONLY; 
                            Client->target_type = GOEP_TP_RFCOMM_ONLY; 
                            Client->l2capRawDataNum = 0;
                            break;
                        }                        
                    }
                    if( NULL != Client->pl2capRawData[0] ){
                    Client->l2capRawDataNum = GOEPL2_MAX_RETRANS_NUM;
                    }
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

        if(ftpc_connect_req->uuid_len != 0)
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
		Client->callback = FtpGoepClientCallback;
        //rsp_code = GOEP_RegisterClient(Client, NULL);
		rsp_code = GOEP_RegisterClient(Client, &FtpStoreFuncTable);

        if(rsp_code != OB_STATUS_SUCCESS)
        {
            goto RETURN_ERROR_CODE;
        }

        /* Client Connect */

        obtpaddr.type = OBEX_TP_BLUETOOTH;
        result = ftp_find_bt_uuid_with_obex_uuid( &(obtpaddr.proto.bt.uuid), ftpc_connect_req->uuid);
        if(result < 0)
        {
            rsp_code = XA_STATUS_INVALID_PARM;
            goto RETURN_ERROR_CODE;
        }
        ftp_bd_addr_stuct2array(&(ftpc_connect_req->bd_addr), &(obtpaddr.proto.bt.addr));

        if(ftpc_connect_req->uuid_len != 0)
        {
            Client->connect_req.target = get_ctrl_buffer(ftpc_connect_req->uuid_len + 1);
            btmtk_os_memcpy(Client->connect_req.target, ftpc_connect_req->uuid, ftpc_connect_req->uuid_len);
            Client->connect_req.target[ftpc_connect_req->uuid_len] = 0; /* NULL terminated */
            Client->connect_req.targetLen = ftpc_connect_req->uuid_len;
        }
        else
        {
            Client->connect_req.target = NULL;
            Client->connect_req.targetLen = 0;
        }



        ObexApp = &(GOEC(clients)[Client->connId].obc);
		if( NULL == Client->callback ){
			ObexApp->goep_buff_ptr = ftpc_connect_req->buf_ptr;
		}
        ObexApp->req_id = ftpc_connect_req->req_id;
        ObexApp->goep_auth_use = ftpc_connect_req->auth_use;
        Client->connect_req.mru = ftpc_connect_req->buf_size;
#if OBEX_AUTHENTICATION == XA_ENABLED
		if( 0 != ftpc_connect_req->client_pwd_len ){
			Challenge = get_ctrl_buffer(sizeof(ObexAuthChallenge));
			auth_passwd = get_ctrl_buffer(GOEP_MAX_PASSWD_SIZE);
			auth_realm = get_ctrl_buffer(GOEP_MAX_REALM_SIZE);    
			ASSERT((auth_passwd != NULL)&&(auth_realm != NULL)&&(Challenge != NULL));

			/* Initialize the dynamically allocated memory */
			btmtk_os_memset( (U8 *) Challenge, 0, sizeof(ObexAuthChallenge));
			btmtk_os_memset( (U8 *) auth_passwd, 0, GOEP_MAX_PASSWD_SIZE);
			btmtk_os_memset( (U8 *) auth_realm, 0, GOEP_MAX_REALM_SIZE);
			if( ftpc_connect_req->client_pwd_len > GOEP_MAX_PASSWD_SIZE ){
				ftpc_connect_req->client_pwd_len = GOEP_MAX_PASSWD_SIZE -1;
			}
			if((ftpc_connect_req->client_pwd[0] != 0) && (ftpc_connect_req->client_pwd_len != 0)){
				btmtk_os_memcpy( auth_passwd, ftpc_connect_req->client_pwd, GOEP_MAX_PASSWD_SIZE);
			}
			if( ftpc_connect_req->client_realm_len > GOEP_MAX_REALM_SIZE ){
				ftpc_connect_req->client_realm_len = GOEP_MAX_REALM_SIZE -1;
			}
			if((ftpc_connect_req->client_realm[0] != 0) && (ftpc_connect_req->client_realm_len != 0)){
				btmtk_os_memcpy( auth_realm, ftpc_connect_req->client_realm, GOEP_MAX_REALM_SIZE);
			}

			Challenge->options = 0;
			Challenge->realm = auth_realm;
			Challenge->realmLen = ftpc_connect_req->client_realm_len;

			// build up a client-challenge
			if( NULL != Client->connect_req.password ){
				free_ctrl_buffer(Client->connect_req.password);
			}
			Client->connect_req.challenge = Challenge;
			Client->connect_req.password = auth_passwd;
			Client->connect_req.passwordLen = ftpc_connect_req->client_pwd_len;
		}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */


        /* Connect to transport layer first */
        rsp_code = GOEP_TpConnect(Client, &obtpaddr);
        
        if(rsp_code == OB_STATUS_SUCCESS)
        {
            /* Connect to OBEX layer */
#if OBEX_AUTHENTICATION == XA_ENABLED
            if(ftpc_connect_req->auth_use)
            {
                /* Since the client needs authentication, 
                wait for auth request to give authentication information */
                ftpc_send_connect_cnf_msg(ObexApp->req_id, ObexApp->goep_conn_id, OB_STATUS_PENDING, 0, 0 , 0);
                //GOEP_Report(("ftpc_connect_req_hdlr: Pending to wait for authenticate request\n"));
                return;
            }
#endif

            rsp_code = GOEP_Connect(Client, &(Client->connect_req));

#if OBEX_AUTHENTICATION == XA_ENABLED
			if( NULL != Client->connect_req.challenge ){
				free_ctrl_buffer(Client->connect_req.challenge->realm);
				free_ctrl_buffer(Client->connect_req.challenge);
				Client->connect_req.challenge = NULL;
			}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

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

    ftpc_send_connect_cnf_msg(ftpc_connect_req->req_id, 0xFF, rsp_code, 0, 0 , 0);
    return;
}

/*****************************************************************************
* FUNCTION
*  ftpc_push_req_hdlr 
* DESCRIPTION
*  This function is to handle the push request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_push_req_hdlr(ftpc_push_req_struct* ftpc_push_req ) 
{
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(ftpc_push_req->goep_conn_id);
    ObexClientApp* ObexApp;
    GoepObjectReq object;
    ObStatus rsp_code;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;

    if(client != NULL)
    {
        ObexApp = &(GOEC(clients)[client->connId].obc);
        btmtk_os_memset( (U8 *) &object, 0, sizeof(GoepObjectReq));

        if(ftpc_push_req->pkt_type == GOEP_FIRST_PKT || GOEP_SINGLE_PKT == ftpc_push_req->pkt_type)
        {
            if(ftpc_push_req->put_type == GOEP_PUT_DELETE || GOEP_SINGLE_PKT == ftpc_push_req->pkt_type )
            {
                object.object = NULL;
            }
            else
            {
                object.object = (void*)1;
            }

            if((ftpc_push_req->obj_name[0] != 0)||(ftpc_push_req->obj_name[1] != 0))
            {
                /* This packet is the firsk packet of the  */
                /* Transfer to utf-8 first */
                object.name = get_ctrl_buffer(GOEP_MAX_OBJ_NAME);
                bt_ucs2ncpy( (S8 *)object.name, (const S8*)ftpc_push_req->obj_name, GOEP_MAX_OBJ_NAME/sizeof(U16));
            }
            else
            {
                object.name = NULL;
            }

            if(ftpc_push_req->obj_mime_type[0] != 0)
            {
                object.type = get_ctrl_buffer(GOEP_MAX_MIME_TYPE);
                bt_strncpy((S8 *)object.type, (const S8 *)ftpc_push_req->obj_mime_type, GOEP_MAX_MIME_TYPE);
            }
            else
                object.type = NULL;

            if(ftpc_push_req->total_obj_len)
            {
                object.objectLen = ftpc_push_req->total_obj_len;
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
                ftp_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
                ftpc_send_push_cnf_msg(ftpc_push_req->goep_conn_id, rsp_code, (U8)rsp_code, uuid, uuid_len);
                return;
            }
        }
        else
        {
            ObexApp->goep_buff_ptr = ftpc_push_req->frag_ptr;
            ObexApp->goep_buff_size = ftpc_push_req->frag_len;
            ObexApp->goep_buff_wrote_len = 0;
            GOEP_ClientContinue(client);
        }
    }
}



/*****************************************************************************
* FUNCTION
*  ftp_pull_req_hdlr 
* DESCRIPTION
*  This function is to handle the pull request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_pull_req_hdlr(ftpc_pull_req_struct* ftp_pull_req) 
{
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(ftp_pull_req->goep_conn_id);
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
        if(ftp_pull_req->pkt_type == GOEP_FIRST_PKT)
        {       
            /* This packet is the firsk packet of the  */
            if((ftp_pull_req->obj_name[0] != 0)||(ftp_pull_req->obj_name[1] != 0))
            {
                /* Transfer to utf-8 first */
                object.name = get_ctrl_buffer(GOEP_MAX_OBJ_NAME);
                bt_ucs2ncpy( (S8*) object.name, (const S8*)ftp_pull_req->obj_name, GOEP_MAX_OBJ_NAME/sizeof(U16));
            }
            else
                object.name = NULL;

            if(ftp_pull_req->obj_mime_type[0] != 0)
            {
                object.type = get_ctrl_buffer(GOEP_MAX_MIME_TYPE);
                bt_strncpy((char*)object.type, (char*)ftp_pull_req->obj_mime_type, GOEP_MAX_MIME_TYPE);
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
                ftp_find_obex_uuid_with_bt_uuid(&(ObexApp->trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);
                ftpc_send_pull_cnf_msg(ftp_pull_req->goep_conn_id, rsp_code, (U8)rsp_code, 0, NULL, 0, NULL, 0, uuid, uuid_len);
                return;
            }
        }
        else
        {
            GOEP_ClientContinue(client);
        }
    }else{
        // pull fail cannot find the connection
        ftpc_send_pull_cnf_msg(ftp_pull_req->goep_conn_id, OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, 0, NULL, 0, NULL, 0, ftp_pull_req->uuid, ftp_pull_req->uuid_len);
    }
}



/*****************************************************************************
* FUNCTION
*  ftp_set_folder_req_hdlr 
* DESCRIPTION
*  This function is to handle the set folder request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_set_folder_req_hdlr(ftpc_set_folder_req_struct* ftp_set_folder_req) 
{
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(ftp_set_folder_req->goep_conn_id);
    GoepFolderReq   folder;
    ObStatus rsp_code = OB_STATUS_INVALID_PARM;

    if(client != NULL)
    {
        switch(ftp_set_folder_req->setpath_flag)
        {
            case GOEP_FORWARD_FOLDER:
            folder.name = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
            ASSERT(folder.name != NULL);
            bt_ucs2ncpy( (S8*)folder.name, (const S8*)ftp_set_folder_req->folder_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
            folder.reset = FALSE;
            folder.flags = OSPF_DONT_CREATE;
            break;

            case GOEP_CREATE_FOLDER:
            folder.name = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
            ASSERT(folder.name != NULL);
            bt_ucs2ncpy( (S8*)folder.name, (const S8*)ftp_set_folder_req->folder_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
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
            ftpc_send_set_folder_cnf_msg(ftp_set_folder_req->goep_conn_id, rsp_code, (U8)rsp_code, (U8*)ftp_set_folder_req->uuid, ftp_set_folder_req->uuid_len);
            return;
        }
    }else{
        // set path but cannot find the connection
        //TODO log
        ftpc_send_set_folder_cnf_msg(ftp_set_folder_req->goep_conn_id, (U8)OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, (U8*)ftp_set_folder_req->uuid , ftp_set_folder_req->uuid_len);
    }

}

/*****************************************************************************
* FUNCTION
*  ftpc_action_req_hdlr 
* DESCRIPTION
*  This function is to handle the set folder request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_action_req_hdlr(ftpc_action_req_struct* ftpc_action_req) 
{
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(ftpc_action_req->goep_conn_id);
    GoepActionReq  action;
    ObStatus rsp_code = OB_STATUS_INVALID_PARM;

    if(client != NULL)
    {
        action.dstname = NULL;            
        action.name = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
        ASSERT(action.name != NULL);
        action.id = ftpc_action_req->action_id;
        action.permission = ftpc_action_req->permission;
        bt_ucs2ncpy( (S8*)action.name, (const S8*)ftpc_action_req->folder_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
        
        if( OBAP_SETPARAMISSION != action.id ){
            action.dstname = get_ctrl_buffer(GOEP_MAX_FOLDER_NAME + 2);
            bt_ucs2ncpy( (S8*)action.dstname, (const S8*)ftpc_action_req->dst_name, GOEP_MAX_FOLDER_NAME /sizeof(U16));
        }

        rsp_code = GOEP_PerformAction(client, &action);    

        if(action.name)
            free_ctrl_buffer(action.name);
        if(action.dstname)
            free_ctrl_buffer(action.dstname);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            ftpc_send_action_cnf_msg(ftpc_action_req->goep_conn_id, rsp_code, (U8)rsp_code, (U8*)ftpc_action_req->uuid, ftpc_action_req->uuid_len);
            return;
        }
    }else{
        // set path but cannot find the connection
        //TODO log
        ftpc_send_action_cnf_msg(ftpc_action_req->goep_conn_id, (U8)OB_STATUS_FAILED, (U8)OBRC_INTERNAL_SERVER_ERR, (U8*)ftpc_action_req->uuid , ftpc_action_req->uuid_len);
    }

}

/*****************************************************************************
* FUNCTION
*  ftp_abort_req_hdlr 
* DESCRIPTION
*  This function is to handle the abort request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_abort_req_hdlr(ftpc_abort_req_struct* ftp_abort_req )
{
    GoepClientApp* client = ftp_find_client_with_goep_conn_id(ftp_abort_req->goep_conn_id);
    ObStatus rsp_code;

    if(client != NULL)
    {
        rsp_code = GOEP_ClientAbort(client);

        if((rsp_code != OB_STATUS_SUCCESS) && (rsp_code != OB_STATUS_PENDING))
        {
            kal_trace(BT_TRACE_G6_OBEX,GOEP_ABORTREQFAIL_SEND_ABORT_RSP);
            ftpc_send_abort_cnf_msg(ftp_abort_req->goep_conn_id, rsp_code, client->connect_req.target, client->connect_req.targetLen);
            return;
        }
    }else{
        ftpc_send_abort_cnf_msg(ftp_abort_req->goep_conn_id, OBRC_INTERNAL_SERVER_ERR, ftp_abort_req->uuid, ftp_abort_req->uuid_len);
    }
}

/*****************************************************************************
* FUNCTION
*  ftp_disconnect_req_hdlr 
* DESCRIPTION
*  This function is to handle the disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_disconnect_req_hdlr(ftpc_disconnect_req_struct* ftp_disconnect_req)
{
    GoepServerApp *Service;
    GoepClientApp* client;
    ObStatus rsp_code;
    ObStatus status = BT_STATUS_FAILED;
	
    if(ftp_disconnect_req->goep_conn_id == 0xFF)
    {
        client = ftp_find_client_with_req_id(ftp_disconnect_req->req_id);
        if(client == NULL){
        	GOEP_Report("[GOEP] ftp_disconnect_req_hdlr fail to find GoepclientApp goep_conn_id:%d", ftp_disconnect_req->goep_conn_id );
	    	ftpc_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, ftp_disconnect_req->uuid, ftp_disconnect_req->uuid_len, OBRC_INTERNAL_SERVER_ERR); //TODO NO SUCH CONNECTION
            return;
		}
		GOEP_Report("[GOEP] before GOEP API connState:%d", client->connState);
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
			GOEP_Report("[FTP] GOEP_Disconnect rsp:%d abort:%d", rsp_code, client->abortTimerStatus);
		   	// lauch a abort timerout
			if( client->connState != CS_DISCONNECTING && client->connState != CS_DISCONNECTED ){
				if( FALSE == client->abortTimerStatus ){
					client->abortTimerStatus = TRUE;
					
					OS_Report("[FTP] start a abort timer to disconnect");
					client->abortTimer.func = ftpc_abortfile_timeout;
					client->abortTimer.context = client;	
					EVM_StartTimer(&client->abortTimer, FTPS_ABORT_TIMEOUT);
				}
			} 			
			
        }
        GOEP_Report("[GOEP] after GOEP API connState:%d", client->connState);

        if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
        {
            /* if disconnect request fails,  call RF_CloseChannel directly -- added by yfchu */
            GoepClientObexCons *clientObex = &GOEC(clients)[client->connId];
            ObBtClientTransport *btxp = &(clientObex->obc.trans.ObexClientBtTrans);

            kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_RFCLOSECHANNEL);

            kal_trace(BT_TRACE_G6_OBEX,GOEP_CALL_RFCLOSECHANNEL_FAILED, status);
            ftpc_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, client->connect_req.target, client->connect_req.targetLen, (U8) rsp_code);
            return;
        }
    }
    else
    {
        
        client = ftp_find_client_with_goep_conn_id(ftp_disconnect_req->goep_conn_id);
        if(client != NULL)
        {
			/* try to do a graceful OBEX disconnect */
            rsp_code = GOEP_Disconnect(client);
			/* try to do a tpDisconnect directly */
			if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING)){
				GOEP_Report("[GOEP] Server Disconnect fail status:%d", rsp_code);
				rsp_code = GOEP_TpDisconnect(client);
			}
			/* report to MMI that fail to disconnect */
            if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
            {
                GoepClientObexCons *clientObex = &GOEC(clients)[client->connId];
                ObBtClientTransport *btxp = &(clientObex->obc.trans.ObexClientBtTrans);
				ASSERT(FALSE); // TODO: test this
				GOEP_Report("[GOEP] Server TpDisconnect fail status:%d", rsp_code);
                
                ftpc_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, client->connect_req.target, client->connect_req.targetLen, (U8)rsp_code);
                return;
            }

			GOEP_Report("[FTP] GOEP_Disconnect rsp:%d abort:%d", rsp_code, client->abortTimerStatus);
			// lauch a abort timerout
			if( client->connState != CS_DISCONNECTING && client->connState != CS_DISCONNECTED ){
				if( FALSE == client->abortTimerStatus ){
					client->abortTimerStatus = TRUE;
					
					OS_Report("[FTP] start a abort timer to disconnect");
					client->abortTimer.func = ftpc_abortfile_timeout;
					client->abortTimer.context = client;	
					EVM_StartTimer(&client->abortTimer, FTPS_ABORT_TIMEOUT);
				}
			}			
        }
        else
        {
            /* The disconnecting connection does not exist */
            /* It may be race condition, thus ignore it. */
            kal_trace(BT_TRACE_G6_OBEX,GOEP_MAIN_DISCONECT_AN_INEXISTENT_GOEP_CONN_ID_xD , ftp_disconnect_req->goep_conn_id);
            ftpc_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, ftp_disconnect_req->uuid, ftp_disconnect_req->uuid_len, OBRC_INTERNAL_SERVER_ERR); //TODO NO SUCH CONNECTION
            return;
        }

    }
}

/*****************************************************************************
* FUNCTION
*  ftpc_client_internal_rw_hdlr 
* DESCRIPTION
*  This function is to handle the disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_client_internal_rw_hdlr(bt_ftpc_client_internal_rw_ind_struct* req)
{
    ObStatus rsp_code;
    ObStatus status = BT_STATUS_FAILED;
	GoepClientApp *goepApp = ftp_find_client_with_goep_conn_id(req->goep_conn_id);
	ObexRespCode rsp1, rsp2;	

    if(goepApp != NULL && goepApp->object != NULL )
    {
		rsp1 = FTPOBS_Flush( goepApp->object );
		rsp2 = FTPOBS_PreReadMemory( goepApp->object );
		if(rsp1 == OBRC_SUCCESS || rsp2 == OBRC_SUCCESS){
			((FtpcObStoreHandle)goepApp->object)->selfmsg = FALSE;
		}else{
			OS_Report("[GOEP][WRN] s internal stop rsp1:%d rsp2:%d", rsp1, rsp2);
		}
    }
    else
    {
		OS_Report("[GOEP][ERR] Cannot find service %d", req->goep_conn_id);            

        return;
    }
}


/*****************************************************************************
* FUNCTION
*  ftp_disconnect_req_hdlr 
* DESCRIPTION
*  This function is to handle the disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_disconnect_req_hdlr(ftps_disconnect_req_struct* ftp_disconnect_req)
{
    GoepServerApp *Service;
    ObStatus rsp_code;
    ObStatus status = BT_STATUS_FAILED;
	// only take care the ftps disconnect request
    {
        Service = ftp_find_server_with_goep_conn_id(ftp_disconnect_req->goep_conn_id);
        if(Service != NULL)
        {
            rsp_code = GOEP_ServerTpDisconnect(Service);

            if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
            {
                if (Service->target[0] != NULL) /* OPP Server's target is NULL */
                {
                    ftps_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, Service->target[0]->target, Service->target[0]->targetLen, (U8)rsp_code);
                }
                else
                {
                    ftps_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, NULL, 0, (U8)rsp_code);
                }
                return;
            }
        }
        else
        {
            
            /* The disconnecting connection does not exist */
            /* It may be race condition, thus ignore it. */
            kal_trace(BT_TRACE_G6_OBEX,GOEP_MAIN_DISCONECT_AN_INEXISTENT_GOEP_CONN_ID_xD , ftp_disconnect_req->goep_conn_id);
            ftps_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, ftp_disconnect_req->uuid, ftp_disconnect_req->uuid_len, OBRC_INTERNAL_SERVER_ERR); //TODO NO SUCH CONNECTION
            return;
        }
    }
}



/*****************************************************************************
* FUNCTION
*  ftps_server_internal_rw_hdlr 
* DESCRIPTION
*  This function is to handle the disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_server_internal_rw_hdlr(bt_ftps_server_internal_rw_ind_struct* req)
{
    ObStatus rsp_code;
    ObStatus status = BT_STATUS_FAILED;
	GoepServerApp *goepApp = ftp_find_server_with_goep_conn_id(req->goep_conn_id);
	ObexRespCode rsp1, rsp2;
	
    if(goepApp != NULL && goepApp->object != NULL )
    {
		rsp1 = FTPOBS_Flush( goepApp->object );
		rsp2 = FTPOBS_PreReadMemory( goepApp->object );	
		if(rsp1 == OBRC_SUCCESS || rsp2 == OBRC_SUCCESS){
			((FtpcObStoreHandle)goepApp->object)->selfmsg = FALSE;
		}else{
			OS_Report("[GOEP][WRN] s internal stop rsp1:%d rsp2:%d", rsp1, rsp2);
		}
    }
    else
    {
		OS_Report("[GOEP][ERR] Cannot find service %d", req->goep_conn_id);            

        return;
    }
}



/*****************************************************************************
* FUNCTION
*  ftp_auth_rsp_hdlr 
* DESCRIPTION
*  This function is to handle the authentication response from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_auth_rsp_hdlr(ftps_auth_rsp_struct* ftp_auth_rsp) 
{
    ObexAuthResponse* Response = get_ctrl_buffer(sizeof(ObexAuthResponse));
    GoepServerApp *Service;
    GoepClientApp *client;
    ObStatus rsp_code;

    btmtk_os_memset( (U8 *)Response, 0, sizeof(ObexAuthResponse));

    if((ftp_auth_rsp->passwd[0] != 0) && (ftp_auth_rsp->passwd_len != 0))
        {
        Response->password = get_ctrl_buffer(sizeof(ftp_auth_rsp->passwd));
        ASSERT(Response->password != NULL);
        btmtk_os_memcpy(Response->password, ftp_auth_rsp->passwd, ftp_auth_rsp->passwd_len);
        Response->passwordLen = ftp_auth_rsp->passwd_len;
        }

    if((ftp_auth_rsp->userid[0] != 0) && (ftp_auth_rsp->userid_len != 0))
        {
        Response->userId = get_ctrl_buffer(sizeof(ftp_auth_rsp->userid));
        ASSERT(Response->userId != NULL);
        btmtk_os_memcpy(Response->userId, ftp_auth_rsp->userid, ftp_auth_rsp->userid_len);
        Response->userIdLen = ftp_auth_rsp->userid_len;
    }
    
    Service = ftp_find_server_with_goep_conn_id(ftp_auth_rsp->goep_conn_id);
    if(Service != NULL)
    {
        rsp_code = GOEP_ServerAuthenticate(Service, Response, NULL);
        //GOEP_Report(("ftp_auth_rsp_hdlr: GOEP_ServerAuthenticate status %d\n", rsp_code));
        rsp_code = GOEP_ServerContinue(Service);
        //GOEP_Report(("ftp_auth_rsp_hdlr: GOEP_ServerContinue status %d\n", rsp_code));
    }
    else
    {
        client = ftp_find_client_with_goep_conn_id(ftp_auth_rsp->goep_conn_id);
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
*  ftp_TpDisconnect_req_hdlr 
* DESCRIPTION
*  This function is to handle the TP disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftpc_TpDisconnect_req_hdlr(ftpc_disconnect_req_struct* ftp_disconnect_req) 
{
    GoepClientApp* client;
    ObStatus rsp_code;

    client = ftp_find_client_with_req_id(ftp_disconnect_req->req_id);
    if(client == NULL){
		ASSERT(0);		
        return;
	}

	if (client->connState == CS_DISCONNECTING)
	{
	    GOEP_Report(("[GOEP] client is already CS_DISCONNECTING!"));
	    return;
	}

    rsp_code = GOEP_TpDisconnect(client);

    if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
    {
        ftpc_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, client->connect_req.target, client->connect_req.targetLen, (U8)rsp_code);
    }
}


/*****************************************************************************
* FUNCTION
*  ftp_TpDisconnect_req_hdlr 
* DESCRIPTION
*  This function is to handle the TP disconnect request from the OBEX client application
* PARAMETERS
*  ilm_ptr     IN       The primitives
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftps_TpDisconnect_req_hdlr(bt_ftps_disconnect_req_struct* ftp_disconnect_req) 
{
	/*
    GoepServerApp* server;
    ObStatus rsp_code;

	GoepServerApp *goepApp = ftp_find_server_with_goep_conn_id(ftp_disconnect_req->goep_conn_id);
    if(server == NULL){
		ASSERT(0);		
        return;
	}

    rsp_code = GOEP_TpDisconnect(server);

    if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
    {
        ftps_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, OBEX_FTP_UUID, 16, (U8)rsp_code);
    }
	*/
	GoepServerApp *Service;
	ObStatus rsp_code = GOEP_NOT_FOUND;

	Service = goep_find_server_with_goep_conn_id(ftp_disconnect_req->goep_conn_id);
	if(Service != NULL)
	{
		rsp_code = GOEP_ServerTpDisconnect(Service);

		if((rsp_code != OB_STATUS_SUCCESS)&&(rsp_code != OB_STATUS_PENDING))
		{
			ftps_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, Service->target[0]->target, Service->target[0]->targetLen, (U8)rsp_code);
			return;
		}
	}else{
		GOEP_Report("[FTP][WRN] cannot find server with goep_conn_id:%d", ftp_disconnect_req->goep_conn_id);
		ftps_send_disconnect_ind_msg(ftp_disconnect_req->req_id, ftp_disconnect_req->goep_conn_id, Service->target[0]->target, Service->target[0]->targetLen, (U8)rsp_code);

	}
}


/*****************************************************************************
* FUNCTION
*  ftp_deinit 
* DESCRIPTION
*  This function is to handle the panic from the lower layer to deinit GOEP context
* PARAMETERS
*  none
* RETURNS
*  none
* GLOBALS AFFECTED
*  None
*****************************************************************************/
void ftp_deinit(void)
{
    GoepServerApp *Service;
    GoepClientApp *client;
    ObStatus rsp_code;
    int i;

    /* here only FTP & OPP server/client need to be deregisted */

    /* Server Deinit */
    for (i = 0; i < GOEP_NUM_OBEX_SERVER_CONS; i++)
    {
        Service = ftp_find_server_with_goep_conn_id(i);
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
                if(Service->target[0])
                {
                    if(Service->target[0]->target)
                        free_ctrl_buffer(Service->target[0]->target);
                    free_ctrl_buffer(Service->target[0]);
                }
                free_ctrl_buffer(Service);
            }
        }
    }
    /* Client Deinit */
    for (i = 0; i < GOEP_NUM_OBEX_CLIENT_CONS; i++)
    {
        client = ftp_find_client_with_goep_conn_id(i);
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
                if(client->connect_req.target)
                    free_ctrl_buffer(client->connect_req.target);
                free_clientapp_ctrl_buffer(client);
	        }
        }
    }
}

void ftpc_send_internal_rw_ind_msg(U8 goep_conn_id, U8 param)
{
     bt_ftpc_client_internal_rw_ind_struct* ind = NULL;
	
	 ind = (bt_ftpc_client_internal_rw_ind_struct*)
		 construct_local_para(sizeof(bt_ftpc_client_internal_rw_ind_struct), TD_CTRL);
	 ASSERT(ind != NULL);
	
	 ind->goep_conn_id = goep_conn_id;
	 ind->param = param;
	
	 //FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, ind , sizeof(bt_ftpc_client_internal_rw_ind_struct) );
	 BT_SendMessage( (msg_type) MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW, MOD_BT, ind , sizeof(bt_ftpc_client_internal_rw_ind_struct) );
}

void ftps_send_internal_rw_ind_msg(U8 goep_conn_id, U8 param)
{
    bt_ftps_server_internal_rw_ind_struct* ind = NULL;

    ind = (bt_ftps_server_internal_rw_ind_struct*)
        construct_local_para(sizeof(bt_ftps_server_internal_rw_ind_struct), TD_CTRL);
    ASSERT(ind != NULL);

	ind->goep_conn_id = goep_conn_id;
	ind->param = param;

    //FTP_APPROFILE_DISPATCH( MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, ind , sizeof(bt_ftps_server_internal_rw_ind_struct) );
	BT_SendMessage( (msg_type) MSG_ID_BT_FTPS_SERVER_INTERNAL_RW, MOD_BT, ind , sizeof(bt_ftps_server_internal_rw_ind_struct) );
}

#endif
#endif /* defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__) */
