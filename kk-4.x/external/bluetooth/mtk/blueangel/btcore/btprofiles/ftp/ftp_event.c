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
/*******************************************************************************
 *
 * Filename:
 * ---------
 *   ftp_event.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains goep event handler of Message Access Profile (MAP).
 *
 * Author:
 * -------
 *   Autumn Li
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#if defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__)

#include "bt_common.h"
#include "bluetooth_struct.h"
#include "sdp_adp.h"
#include "sdpdb.h"
#include "goep.h"

#include "goepext.h"
#include "ftp.h"
#include "bt_os_api.h"
#include "ftpobs.h"
#include "bt_ucs2.h"
#include "goep_main.h"

// Remove trace for tuning performance of GOEPv2
#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#ifdef __BT_GOEP_V2__
/* from goep.c */
extern void AssociateServer(ObexServerApp *serverApp);
extern U8 OBSTACK_IsTxBufferAvailable(void);
extern void ftpc_send_auth_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8* uuid_p, U8 uuid_len);
extern void ftpc_send_auth_ind_msg(U8 goep_conn_id, U8* realm, U8 realm_len, U8* uuid_p, U8 uuid_len);
extern void ftpc_send_action_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);
extern void ftpc_send_connect_cnf_msg(U8 req_id, U8 goep_conn_id, ObStatus rsp_code, U16 peer_mru, U8 tptype, U8 obexver);
extern void ftpc_send_disconnect_ind_msg(U8 req_id, U8 goep_conn_id, U8* uuid_p, U8 uuid_len, U8 reason);
extern void ftpc_send_push_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);
extern void ftpc_send_pull_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, goep_pkt_type_enum pkt_type, U8* obj_mime_type, U32 total_obj_len, U8* frag_ptr, U16 frag_len, U8* uuid_p, U8 uuid_len);
extern void ftpc_send_set_folder_cnf_msg(U8 goep_conn_id, ObStatus rsp_code, U8 reason, U8* uuid_p, U8 uuid_len);
extern void ftps_send_pullfile_rsp_ind_msg(U8 goep_conn_id, U8 rsp_code );
extern void ftps_send_pushfile_rsp_ind_msg(U8 goep_conn_id, U8 rsp_code, const U8 *tmppath, const U8 *finalpath , U32 totalsize);
extern void ftps_send_abort_ind_msg(U8 goep_conn_id, U8* uuid_p, U8 uuid_len);
extern void ftps_send_disconnect_ind_msg(U8 req_id, U8 goep_conn_id, U8* uuid_p, U8 uuid_len, U8 reason);
extern void ftps_send_auth_ind_msg(U8 goep_conn_id, U8* realm, U8 realm_len, U8* uuid_p, U8 uuid_len);
extern void ftps_send_connect_ind_msg(U8 goep_conn_id, U16 peer_mru, U8 challenge, U8 response, U8 tptype, U8 obexver);
extern void ftps_send_set_folder_ind_msg(U8 goep_conn_id, goep_set_folder_type_enum setpath_flag,
                            U8* folder_name, U16 folder_name_len, U8* uuid_p, U8 uuid_len);

static BOOL ftp_util_check_progress(U32 curr, U32 total, U32 *plastupdate, U8 gap){
    BOOL ret = FALSE;
    /* check the update gap */
    if( gap >= 100 || gap < 3 ){
        gap = 3;
    }

	if( total == 0 ){
                // remote doesn't send filesize, the totoal is 0 !
                if( (curr - *plastupdate) > (FTP_UPDATE_PROGRESS_GAP*10*1024) ){
                // update event (10K*GAP(1~100)))
                ret = TRUE;
                }
		return ret;
        }

	if( curr > total && 0 != total ){
        /* remote device may send a wrong length header (total) */
		GOEP_Report("[FTP][ERR] ftp_util_check_progress wrong curr:%d total:%d", curr, total);
		return ret;
	}

	kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_CHECK_PROGRESS, curr, total, *plastupdate);
	if( 0 == total ){
        // avoid segmentation fault. 0 should be handle above code and retrun
        total = 1;
	}

	if( curr > *plastupdate && (S32)((curr-*plastupdate)*100/total) >= gap ){
        *plastupdate = curr;
        ret = TRUE;
    }else if( *plastupdate == 0 && curr > 0 ){
        *plastupdate = curr;
        ret = TRUE;
    }else if( curr >= total  && *plastupdate != curr ){
        *plastupdate = curr;
        ret = TRUE;
    }else if( ((total & 0xfff80000)!=0) && ( (U8)((curr*100)/total) > (U8)((*plastupdate*100)/total) ) ){
        // if file is large then 512KB, update the progress when percentage is large)
        //*plastupdate = curr;
        //ret = TRUE;
    }

    if( ret ){
        kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_CHECK_PROGRESS_RET, curr, total, *plastupdate, ret);
    }
    
    return ret;
}


/*
 * Copy the callback from NotifyCurrServer
 * Connect: GOEP_EVENT_START -> GOEP_EVENT_HEADER_RX* -> GOEP_EVENT_CONTINUE -> GOEP_EVENT_COMPLETE
 * Set folder: GOEP_EVENT_START -> GOEP_EVENT_CONTINUE -> GOEP_EVENT_PRECOMPLETE -> GOEP_EVENT_COMPLETE
 * Pull (Get): GOEP_EVENT_START -> GOEP_EVENT_HEADER_RX* -> GOEP_EVENT_PROVIDE_OBJECT -> GOEP_EVENT_CONTINUE* -> GOEP_EVENT_PRECOMPLETE -> GOEP_EVENT_COMPLETE
 */
void FtpGoepServerCallback(GoepServerEvent *event)
{
	ObexServerApp *serverApp;
	GoepServerApp *goepApp = event->handler;
    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len = 0;
    U8 reason;
    BOOL l_useMem = FALSE;

    GoepEventType Event;
    ObStatus    status;
    GoepServerObexCons *server;
	GoepServerApp *Service;

	serverApp = &(GOES(servers)[goepApp->connId].obs);
	server = (GoepServerObexCons*)serverApp;

	Event = event->event;

    kal_trace(BT_TRACE_G6_OBEX, FTP_GOEP_SERVER_CALLBACK, event->event, event->oper, event->handler->connId);
    
	if( Event == GOEP_EVENT_AUTHORIZE_IND )
    {
		ObexTpConnInfo tpInfo;
		goep_bd_addr_struct goep_addr;
		U8 *addr;
		GOEP_ServerGetTpConnInfo(event->handler, &tpInfo);  /* TBC */
		
		// tpInfo.remDev->bdAddr.addr 6 byte to nel address
		OS_MemSet( (U8 *) &goep_addr, 0, sizeof(goep_bd_addr_struct));
		if( tpInfo.remDev ){
			addr = tpInfo.remDev->bdAddr.addr;
			goep_addr.nap = (((U16)((U8 *) addr)[4])) | ((U16)((U8 *) addr)[5]<<8);
			goep_addr.uap = (U8)((U8 *) addr)[3];
			goep_addr.lap =  (((U32)((U8 *) addr)[2]) << 16) | ((U32)((U8 *) addr)[1] <<8)|((U32)((U8 *) addr)[0]); 
		}
#if 1		
		ftps_send_authorize_ind_msg( serverApp->goep_conn_id, &goep_addr, (U8 *)NULL);
#else
		// auto accept it
        Service = (GoepServerApp *)goep_find_server_with_goep_conn_id(serverApp->goep_conn_id);		
		GOEP_ServerAuthorizeResponse(Service, 0);
#endif
		return;
    }

    Service = (GoepServerApp *)goep_find_server_with_goep_conn_id(serverApp->goep_conn_id);

    if (server->currOp.oper == GOEP_OPER_PUSH || 
        server->currOp.oper == GOEP_OPER_PULL) {
        /* Indicate the status of the final bit in the current OBEX packet */
        server->currOp.info.pushpull.finalBit = 
            (BOOL)((serverApp->handle.parser.opcode & 0x80) ? TRUE : FALSE);
    }

	if(Event == GOEP_EVENT_TP_DISCONNECTED)
    {
		GOEP_Report("[FTP] GOEP_EVENT_TP_DISC cur goep_conn_id:%d", serverApp->goep_conn_id);
        ftps_send_disconnect_ind_msg( serverApp->goep_conn_id, serverApp->goep_conn_id, uuid, uuid_len, 0); //TODO reason
	}

    server->currOp.event = Event;

    /* If we found a server, indicate the event now. */
    if (server->currOp.handler) 
    {

 
        /* no callback, send msg. */
		
	    if(Service != 0 )
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
	    }
		
        if(server->currOp.event == GOEP_EVENT_CONTINUE)
        {
            if(server->currOp.oper == GOEP_OPER_PUSH)
            {
                if(serverApp->goep_first_packet_received == KAL_FALSE)
                {
                    if( serverApp->goep_buff_ptr ){
                        if(serverApp->server.resp == (OBRC_SUCCESS | OB_FINAL_BIT)){
                            ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_SINGLE_PKT, GOEP_PUT_NORMAL, 
                                                server->currOp.info.pushpull.objectLen, 
                                                (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                                (U8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                                serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                                uuid, uuid_len);
                        }else
                        {
                            ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FIRST_PKT, GOEP_PUT_NORMAL, 
                                                    server->currOp.info.pushpull.objectLen, 
                                                    (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                                    (U8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                                    serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                                    uuid, uuid_len);
                            serverApp->goep_first_packet_received = KAL_TRUE;
                            GOEP_Report("[FTPS] Current first_packet_received:%d line:%d", serverApp->goep_first_packet_received, __LINE__);
                        }
                    }else{
                        if( goepApp->object != NULL){
                        	/* IVT will send file data in the first obex packet. */                            
                        }
						
                        /* obstore style */
						if( server->currOp.info.pushpull.nameLen == 0 &&  server->currOp.info.pushpull.typeLen == 0){
							// Error Pull Request. Reject it
							GOEP_Report("[FTP][WRN] Reject a unknow push req ! Ignore it");
							GOEP_ServerAbort(goepApp, GOEP_FORBIDDEN);
							GOEP_ServerContinue( goepApp );
							return;
						}
						
						SRM_Report("[SRM] check server->obs.server.srm_flags:0x%x", server->obs.server.srm_flags );
                        ftps_send_pushfile_ind_msg(serverApp->goep_conn_id, 
                                                    server->currOp.info.pushpull.objectLen, 
                                                    (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                                    (U8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                                    (0 != (server->obs.server.srm_flags & OBSF_CLIENT_ENABLE)) );
			            serverApp->goep_first_packet_received = KAL_TRUE; /* TODO: check this */
			            GOEP_Report("[FTPS] Current first_packet_received:%d line:%d", serverApp->goep_first_packet_received, __LINE__);
               
                    }
                }
                else
                {
					if( serverApp->goep_buff_ptr ){
						if(serverApp->server.resp == (OBRC_SUCCESS | OB_FINAL_BIT))
						{
							ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FINAL_PKT, 
													GOEP_PUT_NORMAL, 0, NULL, 0, NULL, 0, 
													serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
													uuid, uuid_len);
							serverApp->goep_first_packet_received = KAL_FALSE;
                            GOEP_Report("[FTPS] Current first_packet_received:%d line:%d", serverApp->goep_first_packet_received, __LINE__);
                            
						}
						else
							ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_NORMAL_PKT, GOEP_PUT_NORMAL,
												0, NULL, 0, NULL, 0, 
												serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
												uuid, uuid_len);
					}else{
						/* obstore style */
						if( serverApp->goep_first_packet_received ){
							// Notify ADP
							gFtp.adpserverCb(event);

    						if( goepApp->abortFlag ){
    							GOEP_ServerAbort( goepApp, goepApp->abortReason );
								GOEP_ServerContinue( goepApp );
    						}else{
    						    /* Inside SRM-enabled mode, ignore response */
    						    if( OBSH_SRM_ENABLE == server->obs.server.srm_server ){
									//GOEP_Report("[FTP] SRM is enabled. ignore to call ServerContinue in pushing");
    						    }else{
                                    // TODO: check this !
									/* Update the sending progress */
                                    if( ftp_util_check_progress(FTPOBS_GetObjectLen(goepApp->object), server->currOp.info.pushpull.objectLen, &goepApp->lastprogress, FTP_UPDATE_PROGRESS_GAP) ){
                                        ftps_send_file_progress_ind_msg(serverApp->goep_conn_id, server->currOp.oper, FTPOBS_GetObjectLen(goepApp->object), server->currOp.info.pushpull.objectLen);
                                    }    						    
                                    
        							GOEP_ServerContinue( goepApp );
    						    }
    						}
    					}else{
    						ftps_send_pushfile_ind_msg(serverApp->goep_conn_id,  
                                                        server->currOp.info.pushpull.objectLen, 
                                                        (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                                        (U8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                                        (0 != (server->obs.server.srm_flags |= OBSF_CLIENT_ENABLE)));		
    						serverApp->goep_first_packet_received = KAL_TRUE; 
    					}

					}
                }
            }
            else if(server->currOp.oper == GOEP_OPER_PULL)
            {
                if( serverApp->goep_buff_ptr ){
                    ftps_send_pull_ind_msg(serverApp->goep_conn_id, GOEP_NORMAL_PKT, server->currOp.info.pushpull.type, (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen, uuid, uuid_len);
                }else{
					/* obstore style */
					if( serverApp->goep_first_packet_received ){

                        gFtp.adpserverCb(event);

						if( goepApp->abortFlag ){
							GOEP_ServerAbort( goepApp, goepApp->abortReason );
							GOEP_ServerContinue( goepApp );
						}else{
							/* Update the sending progress */
                            if( ftp_util_check_progress( server->obs.server.rxOffset, FTPOBS_GetObjectLen(goepApp->object), &goepApp->lastprogress, FTP_UPDATE_PROGRESS_GAP) ){
                                ftps_send_file_progress_ind_msg(serverApp->goep_conn_id, server->currOp.oper, server->obs.server.rxOffset, server->currOp.info.pushpull.objectLen);
                            }

                            status = BT_STATUS_SUCCESS;
                            while( BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status ){
                                status = GOEP_ServerContinue( goepApp );
                                GOEP_Report("[SRM] invoke Servercontinue status:%d", status);
								
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
					}else{
						if( goepApp->object != NULL){
							GOEP_Report("[SRM] previous object is not closed yet");
							FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
							goepApp->object = NULL;
						}
						
					    GOEP_Report("[SRM] pullfile_ind  srm_flags:0x%x ", server->obs.server.srm_flags);
						ftps_send_pullfile_ind_msg(serverApp->goep_conn_id, server->currOp.info.pushpull.type, 
							(U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,  (0 != (server->obs.server.srm_flags & OBSF_CLIENT_ENABLE)) );

						serverApp->goep_first_packet_received = KAL_TRUE; /* TODO: check this */
					}
                }
            }
            else if(server->currOp.oper == GOEP_OPER_CONNECT)
            {
#if GOEP_AUTH == XA_ENABLED

			
				if (!(server->flags & GOEF_CHALLENGE) ) {
					// no client-challenge
					/// hsa client-challenge rsquest, verify with previous password
					ftps_send_connect_ind_msg(serverApp->goep_conn_id, (U16)(serverApp->handle.parser.maxTxPacket-6), FALSE, FALSE, GOEP_ServerGetTpType(goepApp), GOEP_ServerGetRemoteVersion(goepApp));
				}else{
					// has client-challenge
					/// no client-challenge request 
					ftps_send_connect_ind_msg(serverApp->goep_conn_id, (U16)(serverApp->handle.parser.maxTxPacket-6), TRUE, FALSE, GOEP_ServerGetTpType(goepApp), GOEP_ServerGetRemoteVersion(goepApp));
				}
					
#else            
                if(server->flags & GOEF_CHALLENGE){
                    ftps_send_auth_ind_msg(serverApp->goep_conn_id, server->currOp.challenge.realm, server->currOp.challenge.realmLen, uuid, uuid_len);
                }else{
                    ftps_send_connect_ind_msg(serverApp->goep_conn_id, (U16)(serverApp->handle.parser.maxTxPacket-6), FALSE, FALSE, GOEP_ServerGetTpType(goepApp), GOEP_ServerGetRemoteVersion(goepApp));
		}
#endif				
            }
            else if(server->currOp.oper == GOEP_OPER_SETFOLDER)
            {
                if(server->currOp.info.setfolder.reset == KAL_FALSE)
                {
                    if(server->currOp.info.setfolder.flags & OSPF_BACKUP)
                    {                        
                        ftps_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_BACK_FOLDER, NULL, 0, uuid, uuid_len);
                    }
                    else if(server->currOp.info.setfolder.nameLen)
                    {
                        if(server->currOp.info.setfolder.flags & OSPF_DONT_CREATE)
                            ftps_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_FORWARD_FOLDER, (U8*)server->currOp.info.setfolder.name, server->currOp.info.setfolder.nameLen, uuid, uuid_len);
                        else
                            ftps_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_CREATE_FOLDER, (U8*)server->currOp.info.setfolder.name, server->currOp.info.setfolder.nameLen, uuid, uuid_len);
                    }
                    else
                    {
                        /* Error Handling */
                        goto CANCEL_OPER;
                    }
                }
                else
                {
                    ftps_send_set_folder_ind_msg(serverApp->goep_conn_id, GOEP_ROOT_FOLDER, NULL, 0, uuid, uuid_len);
                }
            }
            else if(server->currOp.oper == GOEP_OPER_ACTION)
            {
                ftps_send_action_ind_msg(serverApp->goep_conn_id, server->currOp.info.action.action_id, server->currOp.info.action.permission, (U8*)server->currOp.info.action.name, server->currOp.info.action.nameLen, (U8*)server->currOp.info.action.dstname, server->currOp.info.action.dstnameLen, uuid, uuid_len);
            }
            else if(server->currOp.oper == GOEP_OPER_DELETE)
            {
                /* server->currOp.oper == GOEP_OPER_DELETE */
                if(server->currOp.info.pushpull.objectLen == 0)
                    ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FIRST_PKT, 
                                            GOEP_PUT_DELETE, server->currOp.info.pushpull.objectLen, (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                            (U8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                            0, 0,
                                            uuid, uuid_len);
                else
                {
                     ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_FINAL_PKT, 
                                            GOEP_PUT_NORMAL, 0, NULL, 0, NULL, 0,
                                            serverApp->goep_buff_ptr, serverApp->goep_buff_wrote_len,
                                            uuid, uuid_len);
                     serverApp->goep_first_packet_received = KAL_FALSE;
                     GOEP_Report("[FTPS] Current first_packet_received:%d line:%d", serverApp->goep_first_packet_received, __LINE__);
                     
                }
            }
            else if(server->currOp.oper == GOEP_OPER_CREATEEMPTY)
			{
                ftps_send_push_ind_msg(serverApp->goep_conn_id, GOEP_SINGLE_PKT, 
                                        GOEP_CREATEEMPTY, server->currOp.info.pushpull.objectLen, (U8*)server->currOp.info.pushpull.name, server->currOp.info.pushpull.nameLen,
                                        (U8*)server->currOp.info.pushpull.type, server->currOp.info.pushpull.typeLen,
                                        0, 0,
                                        uuid, uuid_len);
			}
            else if(server->currOp.oper == GOEP_OPER_DISCONNECT)
            {
                GOEP_ServerContinue(Service);
            }
            else if(server->currOp.oper == GOEP_OPER_ABORT)
            {
				/* Client sends abort req or Tp disconnect */
                if( serverApp->goep_buff_ptr ){
                    ftps_send_abort_ind_msg(serverApp->goep_conn_id, uuid, uuid_len);
                }else{
                    // send the final abort to external
					if( 0 == goepApp->abortFlag ){
						GOEP_Report("[FTP] Receive the abort opcode~");
						goepApp->abortReason = GOEP_USER_UNKNOW;
					}

					/*
                    if(server->abortedOper == GOEP_OPER_PUSH)
                    {
                        GOEP_Report("[FTP][WRN] Remote user push-abort abortedOper:%d", server->abortedOper);                    
                        // ftps_send_pushfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_UNKNOW, NULL, NULL, 0);
                        FTPOBS_Close( &goepApp->object, TRUE );
                    }else if(server->abortedOper == GOEP_OPER_PULL)
                    {
                        GOEP_Report("[FTP][WRN] Remote user pull-abort abortedOper:%d", server->abortedOper);
                        // ftps_send_pullfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_UNKNOW);
                        FTPOBS_Close( &goepApp->object, FALSE );
                        //GOEP_ServerAbort( goepApp, 0xa0 ); 
                    }else{
						GOEP_Report("[FTP][WRN] abortedOper:%d", server->abortedOper);
						// ftps_send_pullfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_UNKNOW);
						FTPOBS_Close( &goepApp->object, FALSE );
					}
					*/
					// serverApp->goep_first_packet_received = FALSE;
                    
 					if( OBSH_SRM_ENABLE == server->obs.server.srm_server ){
						GOEP_Report("[FTP] Fail to send abort continue setup the abortFlag ");
   				        goepApp->abortFlag = TRUE;
					}
					GOEP_ServerContinue( Service );

                }
            }
            else
            {
                /* Error Handling */
                goto CANCEL_OPER;
            }
        }
        else if(server->currOp.event == GOEP_EVENT_COMPLETE)
        {
            if(server->currOp.oper == GOEP_OPER_PUSH)
            {
                serverApp->goep_buff_wrote_len = 0;
            }
	        serverApp->goep_first_packet_received = FALSE;
            GOEP_Report("[FTPS] Current first_packet_received:%d line:%d", serverApp->goep_first_packet_received, __LINE__);
    

            if( NULL == serverApp->goep_buff_ptr ){

                if(server->currOp.oper == GOEP_OPER_PUSH)
                {
                    // close file
					U16 filepath[GOEP_MAX_FOLDER_NAME];
					btmtk_os_memset( filepath, 0, sizeof(filepath)-2);
					if( goepApp->object ){
						// notify upper layer for name
						bt_ucs2ncpy((S8*)filepath, (const S8*)FTPOBS_GetFilePath(goepApp->object), sizeof(filepath)/2);
						FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
						goepApp->object = goepApp->object;
					}
					ftps_send_pushfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_STATUS_SUCCESS, (const U8*)FTPS_TEMP_PUSH_FILEPATH, gFtp.ftps_filepath, server->obs.server.rxOffset);
                }else  if(server->currOp.oper == GOEP_OPER_PULL)
                {
                    // close file
					if( goepApp->object ){
						FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
						goepApp->object = goepApp->object;
					}
					// notify upper layer for further handle
					ftps_send_pullfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_STATUS_SUCCESS);
                }
                // Successfully complete the transmission
				// GOEP_ServerContinue(Service); // NO need this
                
            }
        }
        else if(server->currOp.event == GOEP_EVENT_START)
        {
            if(server->currOp.oper == OBSE_PUT_START)
            {
                serverApp->goep_first_packet_received = KAL_FALSE;
                serverApp->goep_buff_wrote_len = 0;
                GOEP_Report("[OBEX] clearn first_packet_received:%d", serverApp->goep_first_packet_received);                            
                
            }
			if(server->currOp.oper == OBSE_GET_START)
			{
				serverApp->goep_first_packet_received = KAL_FALSE;
                GOEP_Report("[OBEX] clearn first_packet_received:%d", serverApp->goep_first_packet_received);                            
                
			}
        }
        else if(server->currOp.event == GOEP_EVENT_AUTH_RESPONSE)
        {
          
        }else if(server->currOp.event == GOEP_EVENT_AUTH_RESPONSE)
		{
#if GOEP_AUTH == XA_ENABLED
			/// receive a client-challenge header header pass it to  AP
			//goep_send_auth_ind_msg(serverApp->goep_conn_id, server->currOp.challenge.realm, server->currOp.challenge.realmLen, uuid, uuid_len);
#endif
		}else if( server->currOp.event == GOEP_EVENT_PROVIDE_OBJECT ){
			/* obstore style */
		    if(server->currOp.oper == GOEP_OPER_PUSH)
            {
				if( goepApp->object == NULL){
					// Provide Object is trigged by BODY header
					//FTPOBS_Close( &goepApp->object, FALSE );
					//goepApp->object = NULL;
					goepApp->object = FTPOBS_New(goepApp, FALSE, l_useMem, serverApp->goep_conn_id);
					if( goepApp->object == NULL){
						Assert(0);
					}
				}
				
				if( goepApp->object ){
				    FTPOBS_AppendFilePath( (FtpcObStoreHandle)goepApp->object, (const U8*)FTPS_TEMP_PUSH_FILEPATH, bt_ucs2strlen((const S8*)FTPS_TEMP_PUSH_FILEPATH) *2 ); //TODO: need to move this
				}
                if( OBRC_SUCCESS == FTPOBS_Open( (FtpcObStoreHandle*)&goepApp->object, TRUE) ){
				    GOEP_ServerAccept( goepApp, goepApp->object ); /* Must. or the operation would abort */
                }else{
                    GOEP_ServerAbort( goepApp, GOEP_FORBIDDEN );
                    GOEP_ServerContinue( goepApp );
                }
            }else if(server->currOp.oper == GOEP_OPER_PULL)
            {
				if( goepApp->object != NULL){
					FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
					goepApp->object = NULL;
					Assert(0);
				}

			}
		}else if( server->currOp.event == GOEP_EVENT_ABORTED ){
			/* Server abort the response or disconnected link */
			if( serverApp->goep_buff_ptr ){
			}else{
				if(server->currOp.oper == GOEP_OPER_PUSH)
                {
					// If remove disconnected, we report as remote cancel 
					if( server->currOp.reason == OBRC_LINK_DISCONNECT ){
						ftps_send_pushfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_UNKNOW, NULL, NULL, 0);
					}else{
						ftps_send_pushfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_CANCEL, NULL, NULL, 0);
					}
					if( goepApp->object ){
						FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, TRUE );
						goepApp->object = NULL;
					}
                }else if(server->currOp.oper == GOEP_OPER_PULL)
                {
                    ftps_send_pullfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_CANCEL);
					if( goepApp->object ){
						FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
						goepApp->object = NULL;
					}
                }else{
					GOEP_Report("[FTP][WRN] GOEP_EVENT_ABORTED abortedOper:%d", server->abortedOper);
					ftps_send_pullfile_rsp_ind_msg(serverApp->goep_conn_id, GOEP_USER_CANCEL);
					FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
				}
			}
#ifdef __BT_GOEP_V2__
	    }else if( Event == GOEP_EVENT_HEADER_RX ){
            GOEP_Report("[FTP] callback HEADER_RX type:%d len:%d", event->header.type, event->header.len);
            switch (event->header.type) {
                case OBEXH_SRM:
                    GOEP_Report("[SRM]GOEP_EVENT_HEADER_RX 0x%x", event->header.type);
		        	server->obs.server.srm_flags |= OBSF_CLIENT_ENABLE; /* client aggree to enter srm */
		        	SRM_Report("[SRM] srm_flags:0x%x", server->obs.server.srm_flags);
		        	break;
                case OBEXH_SRM_PARAM:
                    if( event->header.value == OBSH_SRM_ENABLE ){
                        server->obs.server.srm_flags |= OBSF_CLIENT_WAITING; /* client request to wait */
                    }else{
                        GOEP_Report("[FTP][WRN] Unknow HEADER_RX!");
                    }
                    break;
                default:
					break;
                }

#else
#endif
        }
        return;
    }

    /* We did not find a suitable server, handle operation rejection. */
    if (Event == GOEP_EVENT_CONTINUE) {
CANCEL_OPER:
        status = OBEX_ServerAbort(serverApp, OBRC_SERVICE_UNAVAILABLE);
        ASSERT(status == OB_STATUS_SUCCESS);
        OBEX_ServerSendResponse(&server->obs);
        return;
    }    

}

void FtpGoepClientCallback(GoepClientEvent *event)
{

	ObexClientApp *clientApp;
	GoepClientApp *goepApp;
	GoepEventType Event;

    GoepClientEvent  tempOp;
    GoepClientObexCons  *client;
    BtStatus status = BT_STATUS_FAILED;

    U8 uuid[GOEP_MAX_UUID_SIZE];
    U8 uuid_len;
    U8 reason;
    U8 test; // for SRM test

	clientApp = &(GOEC(clients)[event->handler->connId].obc);;
	Event = event->event;
	client = (GoepClientObexCons*)clientApp;
	goepApp = event->handler;

    ASSERT(client->currOp.handler);
    kal_trace(BT_TRACE_G6_OBEX, FTP_CLIENT_GOEP_CALLBACK, event->event, event->oper, event->handler->connId, event->handler->connState);


    reason = client->obc.handle.parser.rsp;
    if(Event == GOEP_EVENT_ABORTED)
    {
        if (client->currOp.reason == OBRC_LINK_DISCONNECT)
        {
            reason = GOEP_STATUS_DISCONNECT;
        }
    }
    
    client->currOp.event = Event;
    client->currOp.failed_rsp_code = reason;
    tempOp = client->currOp;

    if (Event >= GOEP_EVENT_COMPLETE) {
        ASSERT((Event == GOEP_EVENT_TP_CONNECTED) || 
               (Event == GOEP_EVENT_TP_DISCONNECTED) || 
               (Event == GOEP_EVENT_ABORTED) || 
               (Event == GOEP_EVENT_DISCOVERY_FAILED) ||
               (Event == GOEP_EVENT_NO_SERVICE_FOUND) ||
               (Event == GOEP_EVENT_SCO_REJECT) ||
               (Event == GOEP_EVENT_COMPLETE) ||
               (Event == GOEP_EVENT_BTCHIP_REJECT));
        if ((Event == GOEP_EVENT_DISCOVERY_FAILED) || 
            (Event == GOEP_EVENT_NO_SERVICE_FOUND) ||
            (Event == GOEP_EVENT_SCO_REJECT)||
            (Event == GOEP_EVENT_BTCHIP_REJECT)) {
            /* Reset the connection state for this profile */
            if( client && client->currOp.handler){ 
                client->currOp.handler->connState = CS_DISCONNECTED; 
            }
        }
        /* Were done, remove the handler before indicating the event. */
        client->currOp.handler = 0;
    }

    /* Indicate the event */
    tempOp.event = Event;
    
    goep_find_obex_uuid_with_bt_uuid(&(client->obc.trans.ObexClientBtTrans.target_uuid), uuid, &uuid_len);


    if ((Event == GOEP_EVENT_TP_CONNECTED) && 
    #if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
                         ((goepApp->connState == CS_CONNECTING)
                         || (goepApp->connState == CS_DISCONNECTED) || (goepApp->connState == CS_CONNECTED))) {
    #else /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
                         (goepApp->connState == CS_CONNECTED)) {
    #endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */
            /* Deliver connect event */
            goepApp->connState = CS_CONNECTED;
            if(clientApp->goep_auth_use)
            {
                ftpc_send_connect_cnf_msg(clientApp->req_id, clientApp->goep_conn_id, OB_STATUS_PENDING, 0, 0, 0);
                kal_trace(BT_TRACE_G6_OBEX,GOEP_WAITFOR_AUTH_REQ);
            }
            else
            {
                GOEP_Connect(goepApp, &(goepApp->connect_req));

#if OBEX_AUTHENTICATION == XA_ENABLED
						if( NULL != goepApp->connect_req.challenge ){
							free_ctrl_buffer(goepApp->connect_req.challenge->realm);
							free_ctrl_buffer(goepApp->connect_req.challenge);
							goepApp->connect_req.challenge = NULL;
						}
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
            }
    }
    
	/* Deliver disconnect event */
	if(Event == GOEP_EVENT_TP_DISCONNECTED)
	{
		if(goepApp->connState == CS_CONNECTING)
		{
			ftpc_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, 0, 0);
		}
		else if(goepApp->connState == CS_DISCONNECTING)
		{
			ftpc_send_disconnect_ind_msg(clientApp->req_id, 0xFF, uuid, uuid_len, 0); //TODO reason
		}
		else if(goepApp->connState == CS_CONNECTED)
		{
			if(goepApp->obex_conn_is_connected)
			{
				ftpc_send_disconnect_ind_msg(clientApp->req_id, clientApp->goep_conn_id, uuid, uuid_len, 0); //TODO reason
			}
			else
			{
				ftpc_send_disconnect_ind_msg(clientApp->req_id, 0xFF, uuid, uuid_len, 0); //TODO reason
			}
		}
	}

    if(Event == GOEP_EVENT_ABORTED)
    {
        kal_trace(BT_TRACE_G6_OBEX,GOEP_CLIENT_ABORTED, reason, client->currOp.oper, clientApp->client.status);    
        
        if(client->currOp.oper == GOEP_OPER_CONNECT)
        {
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
            /* if authentication response has be sent, and the verification fails, then send CONNECT_RSP with fail */
            if(goep_client && goep_client->goep_auth_resp_sent) // remote doesn't have response
            {
#if GOEP_AUTH == XA_ENABLED
                goep_client->goep_auth_verify_fail = TRUE;
#endif				
                ftpc_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, 0, 0);

                if (client->currOp.reason != OBRC_LINK_DISCONNECT)
                {
 		        GOEP_TpDisconnect(goep_client);
                }
            }
            else
             /* this abort shall be ignored if the server challenge us */
            if ((client->flags & GOEF_CHALLENGE) == 0 && (client->currOp.reason != OBRC_LINK_DISCONNECT))
            {
                /* in case of no challenge and the abort reason is not TPDISCONNECTED */
                GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
                /* Disconnect RFCOMM */
                GOEP_TpDisconnect(goep_client);
            }
        }
        else if((client->currOp.oper == GOEP_OPER_PUSH)||(client->currOp.oper == GOEP_OPER_PULL))
        {
        	U8 reason = GOEP_STATUS_FAILED;
            if(clientApp->client.status == OBRC_USER_ABORT)
            {
            	/* we send the abort */
                reason = GOEP_USER_CANCEL;
            }
            
            if(client->currOp.oper == GOEP_OPER_PUSH)
            {
                if( clientApp->goep_buff_ptr ){
                    ftpc_send_push_cnf_msg(clientApp->goep_conn_id, reason, reason, uuid, uuid_len);
                }else{
                    ftpc_send_pushfile_cnf_msg(clientApp->goep_conn_id, reason, client->currOp.reason, uuid, uuid_len);
                }
            }
            else if(client->currOp.oper == GOEP_OPER_PULL)
            {
                if( clientApp->goep_buff_ptr ){
                    ftpc_send_pull_cnf_msg(clientApp->goep_conn_id, reason, reason, 0,
                                        NULL, 0, NULL, 0, uuid, uuid_len);
                }else{
                    ftpc_send_pullfile_cnf_msg(clientApp->goep_conn_id, reason, client->currOp.reason, 0, 0, uuid, uuid_len);
                    /* delete the failure downloaded file */
                    if( NULL != goepApp->object ){
        				FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object , TRUE);
        				goepApp->object = NULL;
                    }            
                }
            }else{
                GOEP_Report("[FTP][WRN] no handle! oper:%d status:%d", client->currOp.oper, clientApp->client.status);
            }
            
            if( NULL != goepApp->object ){
				FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object , FALSE);
				goepApp->object = NULL;
            }            
        }
        else if(client->currOp.oper == GOEP_OPER_SETFOLDER)
        {
            ftpc_send_set_folder_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_ACTION)
        {
            ftpc_send_action_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
        }        
        else if(client->currOp.oper == GOEP_OPER_DELETE)
        {
            ftpc_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, reason, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_DISCONNECT)
        {
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
            GOEP_TpDisconnect(goep_client);
        }
        else
        {
            /*error handling */
			reason = reason; // for debug
        }
    }
    else if(Event == GOEP_EVENT_CONTINUE)
    {
        if(client->currOp.oper == GOEP_OPER_PUSH)
        {
            if( clientApp->goep_buff_ptr ){
                ftpc_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
            }else{
                /* obstore style. check abort and continue */
                if( goepApp->abortFlag ){
                    GOEP_ClientAbort(goepApp);
					
					if( TRUE == goepApp->abortTimerStatus ){
						goepApp->abortTimerStatus = FALSE;
						EVM_ResetTimer(&goepApp->abortTimer);
						OS_Report("[FTP] stop abortTimer");						
					}
				
                }else{
                    /* client push file */
                    if( ftp_util_check_progress( FTPOBS_GetObjectOffset(goepApp->object), FTPOBS_GetObjectLen(goepApp->object), &goepApp->lastprogress, FTP_UPDATE_PROGRESS_GAP) ){
                        ftpc_send_file_progress_ind_msg(clientApp->goep_conn_id, client->currOp.oper, FTPOBS_GetObjectOffset(goepApp->object), FTPOBS_GetObjectLen(goepApp->object));
                    }


                    if(clientApp->goep_first_packet_received == KAL_FALSE){
                        clientApp->goep_first_packet_received = KAL_TRUE;
                        FTPOBS_CreateMemBuffer(goepApp->object, FTPOBS_GetObjectLen(goepApp->object));
                    }
					
                    status = BT_STATUS_SUCCESS;
                    while( BT_STATUS_SUCCESS == status || BT_STATUS_PENDING == status ){
                        status = GOEP_ClientContinue(goepApp);
                        if( status != BT_STATUS_PENDING){
                            kal_trace(BT_TRACE_G6_OBEX, FTP_GOEP_CLIENT_CONTINUE_RESULT, status);
                        }
                        
                        if( FALSE == GOEP_ClientSRMMode(goepApp) ){ // only loop inside SRM mode
                            break;
                        }
                        if( FALSE == OBSTACK_IsTxBufferAvailable() ){ 
                            break;
                        }
                        if( TRUE == GOEP_ClientInternalPending(goepApp) ){
                            break;
                        }
                    }
                }
            }
        }
        else if(client->currOp.oper == GOEP_OPER_PULL)
        {
            if( clientApp->goep_buff_ptr ){
                if(clientApp->goep_first_packet_received == KAL_FALSE)
                {
                    ftpc_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_FIRST_PKT, 
                                            NULL, client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                            client->obc.goep_buff_wrote_len, uuid, uuid_len);
                    clientApp->goep_first_packet_received = KAL_TRUE;
                }
                else
                {
                    ftpc_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_NORMAL_PKT, 
                                            NULL, client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                            client->obc.goep_buff_wrote_len, uuid, uuid_len);
                }
            }else{
                /* obstore style. check abort and continue */
                if( goepApp->abortFlag ){
                    GOEP_ClientAbort(goepApp);

					if( TRUE == goepApp->abortTimerStatus ){
						goepApp->abortTimerStatus = FALSE;
						EVM_ResetTimer(&goepApp->abortTimer);
						OS_Report("[FTP] stop abortTimer");
					}
					
                }else{
                    /* client pull file */
                    if( ftp_util_check_progress(FTPOBS_GetObjectLen(goepApp->object), client->currOp.info.pull.objectLen , &goepApp->lastprogress, FTP_UPDATE_PROGRESS_GAP) ){
                        ftpc_send_file_progress_ind_msg(clientApp->goep_conn_id, client->currOp.oper, FTPOBS_GetObjectLen(goepApp->object), client->currOp.info.pull.objectLen);
                    }
#ifdef _FTPOBS_DEBUG_					
					GOEP_Report("[FTP] goep_first_packet_received:%d", clientApp->goep_first_packet_received);
#endif
					if(clientApp->goep_first_packet_received == KAL_FALSE){
						clientApp->goep_first_packet_received = KAL_TRUE;
						FTPOBS_CreateMemBuffer(goepApp->object, client->currOp.info.pull.objectLen);
					}

					if(  OBSH_SRM_ENABLE == client->obc.client.srm_client ){
#ifdef _FTPOBS_DEBUG_                        
						GOEP_Report("[FTP][SRM] ignore continue event. srm_client is 0x%x", client->obc.client.srm_client);
#endif
					}else{
     					SRM_Report("[FTP][SRM] client.srm_client:0x%x ", client->obc.client.srm_client);
     					// disable the wait parameter

#if GOEP_TEST_SRM 
                        gFtp.adpclientCb(event);
#endif				
						GOEP_ClientContinue(goepApp);
					}
                }            
            }
        }
        else
        {
            /*error handling */
        }
    }
    else if(Event == GOEP_EVENT_COMPLETE)
    {
        if(client->currOp.oper == GOEP_OPER_CONNECT)
        {
#if GOEP_AUTH == XA_ENABLED
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);

            if ( TRUE == goep_client->goep_auth_chal_server && TRUE == goep_client->goep_auth_verify_fail)
            {   
                ftpc_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, 0, 0);

    		    GOEP_TpDisconnect(goep_client);
            }
    	    else
#endif			
#if BT_GOEP_V2 == XA_ENABLED
            {
                ObexTpConnInfo tpInfo;
                OBEX_GetTpConnInfo(&clientApp->handle, &tpInfo);
                GOEP_Report("[OBEX] Connect transport maxPduSize is %d", tpInfo.maxPduSize);

              if( GOEP_TP_L2CAP_ONLY == GOEP_ClientGetTpType(goepApp) ){
              	if( (clientApp->handle.parser.maxTxPacket) > OBEX_TX_BUFF_SIZE ){
            		clientApp->handle.parser.maxTxPacket = OBEX_TX_BUFF_SIZE;
            		OS_Report("[OBEX][Q] Connect transport shink maxPduSize is %d --- ", clientApp->handle.parser.maxTxPacket);
              	}
              }
            }
#endif
            ftpc_send_connect_cnf_msg(clientApp->req_id, clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, (U16)(clientApp->handle.parser.maxTxPacket - 6), GOEP_ClientGetTpType(goepApp), GOEP_ClientGetRemoteVersion(goepApp));
        }
        else if(client->currOp.oper == GOEP_OPER_SETFOLDER)
        {
            ftpc_send_set_folder_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }
        else if(client->currOp.oper == GOEP_OPER_ACTION)
        {
            ftpc_send_action_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }        
        else if((client->currOp.oper == GOEP_OPER_PUSH)||(client->currOp.oper == GOEP_OPER_DELETE))
        {
            if( clientApp->goep_buff_ptr ){
                ftpc_send_push_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
            }else{
                /* obstore style */
                ftpc_send_pushfile_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, uuid, uuid_len);
                if( NULL != goepApp->object ){
    				FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
    				goepApp->object = NULL;
                }
            }
        }
        else if(client->currOp.oper == GOEP_OPER_PULL)
        {
            if( clientApp->goep_buff_ptr ){
                if(clientApp->goep_first_packet_received == KAL_FALSE)
                    ftpc_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_SINGLE_PKT, NULL, 
                                           client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                           client->obc.goep_buff_wrote_len, uuid, uuid_len);
                else
                    ftpc_send_pull_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, GOEP_FINAL_PKT, NULL, 
                                           client->currOp.info.pull.objectLen, client->obc.goep_buff_ptr, 
                                           client->obc.goep_buff_wrote_len, uuid, uuid_len);
            }else{
                /* obstore style */
                ftpc_send_pullfile_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, GOEP_STATUS_SUCCESS, 0, client->currOp.info.pull.objectLen, uuid, uuid_len);
                if( NULL != goepApp->object ){
    				FTPOBS_Close( (FtpcObStoreHandle*)&goepApp->object, FALSE );
    				goepApp->object = NULL;
                }
            }
        }
        else if(client->currOp.oper == GOEP_OPER_DISCONNECT)
        {
            GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
            GOEP_TpDisconnect(goep_client);
        }
        else
        {
            /*error handling */
        }
    }
    else if((Event == GOEP_EVENT_DISCOVERY_FAILED) || 
            (Event == GOEP_EVENT_NO_SERVICE_FOUND))
    {
        ftpc_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_FAILED, 0, 0, 0);
    }
    else if (Event == GOEP_EVENT_SCO_REJECT)
    {
        ftpc_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_SCO_REJECT, 0, 0, 0);
    }
    else if (Event == GOEP_EVENT_BTCHIP_REJECT)
    {
        ftpc_send_connect_cnf_msg(clientApp->req_id, 0xFF, GOEP_STATUS_BUSY, 0, 0, 0);
    }
    else if(Event == GOEP_EVENT_AUTH_CHALLENGE)
    {
        /* Received authentication challenge from the server */
        ftpc_send_auth_ind_msg(clientApp->goep_conn_id, client->currOp.challenge.realm, client->currOp.challenge.realmLen, uuid, uuid_len);
        kal_trace(BT_TRACE_G6_OBEX,GOEP_RECV_AUTH_CHAL);
    }
    else if(Event == GOEP_EVENT_AUTH_RESPONSE)
    {
        GoepClientApp * goep_client = goep_find_client_with_goep_conn_id(clientApp->goep_conn_id);
        if(GOEP_ClientVerifyAuthResponse(goep_client, goep_client->connect_req.password, goep_client->connect_req.passwordLen))
        {
#if GOEP_AUTH == XA_ENABLED
            if (goep_client->goep_auth_chal_server)
            {
                //don't send connect_rsp here! send it in case of EVENT_COMPLETE and oper==CONNECT
                //goep_send_connect_cnf_msg(clientApp->req_id, clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, uuid, uuid_len);
		        goep_client->goep_auth_chal_server = FALSE;		
			    goep_client->goep_auth_verify_fail = FALSE;
            }
	     else
#endif
            ftpc_send_auth_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_SUCCESS, uuid, uuid_len);
        }
        else
        {
#if GOEP_AUTH == XA_ENABLED
            if (goep_client->goep_auth_chal_server)
            {
			    goep_client->goep_auth_verify_fail = TRUE;
				goep_client->goep_auth_chal_server = TRUE;
            }
	     else
#endif
            ftpc_send_auth_cnf_msg(clientApp->goep_conn_id, GOEP_STATUS_FAILED, uuid, uuid_len);
        }
        kal_trace(BT_TRACE_G6_OBEX,GOEP_RECV_AUTH_RESP);
    }
//#ifdef GOEPV2_TEST
	else if( Event == GOEP_EVENT_HEADER_RX ){
        switch (event->header.type) {
            case OBEXH_SRM:
                if( event->header.value == 1 ){
    		        client->obc.client.srm_flags |= OBSF_SERVER_ENABLE;
                    GOEP_Report("[SRM][CLIENT] SRM Header received type:0x%x srm_flag:0x%x", event->header.type, client->obc.client.srm_flags);
                }else{
                    GOEP_Report("[SRM][WRN] SRM header unknow value:%d", event->header.value);
                }
 		        break;
		    case OBEXH_SRM_PARAM:
		        if( event->header.value == 1 ){
    		        client->obc.client.srm_flags |= OBSF_SERVER_WAITING;		        
    		        GOEP_Report("[SRM][CLIENT] SRM_WAIT Header received type:0x%x srm_flag:0x%x", event->header.type, client->obc.client.srm_flags);
		        }else{
                    GOEP_Report("[SRM][WRN] SRM_PARAM header unknow value:%d", event->header.value);		        
		        }
 		        break;
            }
	}
//#else
//#endif
    
}

#endif
#endif /* defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__) */
