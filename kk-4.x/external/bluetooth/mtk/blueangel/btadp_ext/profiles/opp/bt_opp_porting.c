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
 * bt_goep_fs.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   This file is used to write and read file for OPP Client or Server
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
 * 12 13 2012 dexiang.jiang
 * [ALPS00417075] [Critical][NE][MT6589TD][CMCC][Gemini][CTS 4.1_r1]  "com.mediatek.bluetooth" pops up when running CTS.
 * Patch for OPP  share meory <SP> refBase issue.
 * 
 * 08 13 2012 dexiang.jiang
 * [ALPS00334687] [Need Patch] [Volunteer Patch] Correct select API usage
 * .
 *
 * 09 03 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integration BT solution.
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifdef BTMTK_ON_WIN32
#include "windows.h"
#endif

#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/sockets.h>
#include <limits.h>


#include "bt_goep_porting.h"

#include "ext_osal.h"
#include "ext_nbuffer.h"
#include "bt_os_api.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_opp_comm.h"
#include "bt_goep_fs.h"
#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_hdl.h"
#include "bt_opps_hdl.h"
#include "bt_oppc_struct.h"
#include "bt_opps_struct.h"
#include "bt_oppc_msg.h"

#include "ext_ucs2.h"
#include "bt_ext_debug.h"


extern BTMTK_OPP_CALLBACK g_oppc_ui_callback;

#ifdef BTMTK_ON_LINUX
extern int socket_make_sockaddr_un(const char *name, int namespaceId, struct sockaddr_un *p_addr, socklen_t *alen);
#endif


void btmtk_opps_init_cs(void){
     ///< Init a Critical Section for OPP Server
}

void btmtk_opps_enter_cs(void){
     ///< Enter the OPP Server Critical Section
     printf("[opps] protect\n");
}

void btmtk_opps_leave_cs(void){
     ///<  Leave the OPP Server Critical Section
     printf("[opps] unprotect\n");
}

void btmtk_oppc_init_cs(void){
     ///< Init a Critical Section for OPP Client
}

void btmtk_oppc_enter_cs(void){
     ///< Enter the OPP Client Critical Section
}

void btmtk_oppc_leave_cs(void){
     ///< Leave the OPP Client Critical Section
}

#ifdef BTMTK_ON_LINUX
// shared memory
#else
U8 g_oppc[OPPC_MAX_OBEX_PACKET_LENGTH]; // 14K
U8 g_opps[OPPS_MAX_OBEX_PACKET_LENGTH]; // 14K
#endif

U8 *btmtk_oppc_get_profile_buffer(U32 u4Size){

#ifdef BTMTK_ON_LINUX
	return (U8*)btmtk_oppc_get_ashm_buffer();
#else
	return g_oppc;
#endif
}

U32 btmtk_oppc_get_profile_buffer_size(void){

#ifdef BTMTK_ON_LINUX
	return OPPC_MAX_OBEX_PACKET_LENGTH;
#else
    return sizeof(g_oppc);
#endif
}

void btmtk_oppc_return_buffer(U8 *mem){

#ifdef BTMTK_ON_LINUX
	//btmtk_oppc_return_ashm_buffer();
#else

#endif
}

U8 *btmtk_opps_get_profile_buffer(U32 u4Size){

#ifdef BTMTK_ON_LINUX
	return (U8*)btmtk_opps_get_ashm_buffer();
#else
	return g_opps;
#endif
}

U32 btmtk_opps_get_profile_buffer_size(void){

#ifdef BTMTK_ON_LINUX
	return OPPS_MAX_OBEX_PACKET_LENGTH;
#else
	return sizeof(g_opps);
#endif
}

void btmtk_opps_return_buffer(U8 *mem){

#ifdef BTMTK_ON_LINUX
	//btmtk_opps_return_ashm_buffer();
#else

#endif
}

bt_opp_context_struct g_bt_opp_cntx;
bt_opp_context_struct *const g_bt_opp_cntx_p = &g_bt_opp_cntx;


/*****************************************************************************
* Internal Functions
*****************************************************************************/

/**
* @brief little endian to big endian
*/
U16 bt_goep_ntohs(U16 s)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (((s       & 0xFF) << 8) | \
           (((s >> 8) & 0xFF)));
}


/********************************************** OPP DEBUG *********************************************/

const S8 *btmtk_oppc_get_event_name(U32 event){
    switch(event){
        case BT_OPPCUI_IDLE:
            return "Idle";
            break;
        case BT_OPPCUI_CONNECTING: // param:dev
            return "Connecting";
            break;
        case BT_OPPCUI_CONNECTED: // param:dev
            return "Connected";
            break;
        case BT_OPPCUI_PUSHING:  // param: filename
            return "Pushing";
            break;
        case BT_OPPCUI_PUSHED: // param: rsp_code. Finish a push whether success or fail
            return "Pushed";
            break;
        case BT_OPPCUI_PULLING:  // param: filename
            return "Pulling";
            break;
        case BT_OPPCUI_PULLED: // param: rsp_code . Finish a pull whether success or fail
            return "Pulled";
            break;
        case BT_OPPCUI_DISCONNECTING: // param: none
            return "Disconnecting";
            break;
        case BT_OPPCUI_DISCONNECTED: //param: rsp_code
            return "Disconnected";
            break;
        case BT_OPPCUI_ABORTING: // param: rsp_code
            return "Aborting";
            break;
        case BT_OPPCUI_SHUTDOWNING: // param: none
            return "Shutdowning";
            break;
        case BT_OPPCUI_ERROR: //param: ASCII error msg
            return "Error !";
            break;
        default:
            return "(unknow)";
            break;
    }
}

const S8 *getOPPSState(U32 state){
    switch(state){
    case BT_OPS_IDLE:               /* Initial state */
        return "ops_idle";
        break;
    case BT_OPS_OBEX_REGISTERING:   /*Registering OBEX instance */
        return "ops_obex_registering";
        break;
    case BT_OPS_SDP_REGISTERING:    /* Registering OPP SDP */
        return "ops_sdp_registering";
    case BT_OPS_ACTIVE:             /* OPS service is active */
        return "ops_active";
        break;
    case BT_OPS_CONNECTED:          /* Bearer connection established */
        return "ops_connected";
        break;
    case BT_OPS_ACCEPT_WAIT:          /* Received put indication */
        return "ops_accepting";
        break;
    case BT_OPS_PREPARE_WAIT:           /* Sent successful response */
        return "ops_accepted";
        break;
    case BT_OPS_RECEIVING:          /* Receiving put indication */
        return "ops_receiving";
        break;
    case BT_OPS_ABORTING:           /* Aborting receiving put indication */
        return "ops_aborting";
        break;
    case BT_OPS_DISCONNECTING:      /* Disconnecting current session */
        return "ops_disconnting";
        break;
    case BT_OPS_DEACTIVATING:        /* Send deactivate request to deactivate OPS service */
        return "ops_deactivating";
        break;
    default:
        return "(ops_unknow_state)";
        break;
    }
}

const S8 *getOPPCState(U32 state){
    switch(state){
    case BT_OPC_IDLE:           /* Initial state */
        return "opc_idle";
        break;
    case BT_OPC_CONNECTING:     /* Send connect request to OPC service */
        return "opc_connecting";
        break;
    case BT_OPC_CONNECTED:      /* the result of Connect rsp is success*/
        return "opc_connected";
        break;
    case BT_OPC_SENDING:        /* Send put request to BCHS */
        return "opc_sending";
        break;
    case BT_OPC_RECEIVING:      /* QQQ: Send pull request to remote */
        return "opc_receiving";
        break;
    case BT_OPC_ABORTING:       /* Abort current sending request */
        return "opc_abortting";
        break;
    case BT_OPC_DISCONNECTING:   /* Received failed response from peer */
        return "opc_disconnting";
        break;
    default:
        return "(opc_unknow_state)";
        break;
    }
}

const S8 *getOPPFeature(U32 feature){
    switch(feature){
    case BT_OPP_READY_FEATURE:
        return "ready_feature";
        break;
    case BT_OPP_PULL_FEATURE:
        return "pull_feature";
        break;
    case BT_OPP_PUSH_FEATURE:
        return "push_feature";
        break;
    case BT_OPP_EXCHANGE_FEATURE:
        return "exchange_feature";
        break;
    default:
        return "(opc_unknow_state)";
        break;
    }
}

S8 *btmtk_opp_util_copy_filepath(S8 *pDstFilePath, const S8 *pSrcFilePath, U32 u4MaxSize){

    if( NULL == pSrcFilePath ){
        return NULL;
    }
    if( (U32)ext_ucs2strlen(pSrcFilePath) > ((u4MaxSize/2) -1)   ){
        ext_ucs2ncpy(pDstFilePath, pSrcFilePath, u4MaxSize);
        pDstFilePath[u4MaxSize -1 ] = pDstFilePath[u4MaxSize - 2] = 0;
        return pDstFilePath;
    }
    return NULL;
}

const S8 *btmtk_opps_get_event_name(U32 event){

    switch(event){
    case BT_OPPSUI_SHUTDOWNED: //
    return "BT_OPPSUI_SHUTDOWNED";
     case BT_OPPSUI_SDP_REGISTERED: // param: rsp_code
    return "BT_OPPSUI_SDP_REGISTERED";
     case BT_OPPSUI_AUTHORIZING: // param: dev
    return "BT_OPPSUI_AUTHORIZING";
     case BT_OPPSUI_CONNECTED: // param:dev
    return "BT_OPPSUI_CONNECTED";
     case BT_OPPSUI_PUSH_START:  // param: filename
    return "BT_OPPSUI_PUSH_START";
     case BT_OPPSUI_PUSHING: // param: percentage
    return "BT_OPPSUI_PUSHING";
     case BT_OPPSUI_PUSHED: // param: rsp_code. Finish a push whether success or fail
    return "BT_OPPSUI_PUSHED";
     case BT_OPPSUI_PULL_START: // param: filename
    return "BT_OPPSUI_PULL_START";
     case BT_OPPSUI_PULLING:  // param: percentage
    return "BT_OPPSUI_PULLING";
     case BT_OPPSUI_PULLED: // param: rsp_code . Finish a pull whether success or fail
    return "BT_OPPSUI_PULLED";
     case BT_OPPSUI_DISCONNECTING: // param: none
    return "BT_OPPSUI_DISCONNECTING";
     case BT_OPPSUI_DISCONNECTED: //param: rsp_code
    return "BT_OPPSUI_DISCONNECTED";
     case BT_OPPSUI_ABORTING: // param: rsp_code
    return "BT_OPPSUI_ABORTING";
     case BT_OPPSUI_SHUTDOWNING: // param: none
    return "BT_OPPSUI_SHUTDOWNING";
     case BT_OPPSUI_ERROR: //param: ASCII error msg
    return "BT_OPPSUI_ERROR";
     case BT_OPPSUI_MAX:
     return "BT_OPPSUI_MAX";
     default:
        return "<unknow event";
    }
}

void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
	int ret = -1;
	ilm_struct ilm;
	U16 *field;

	BT_OPP_LOGD("[EXT][BTCMD_SendMessage][+]: msg:[%d]", msg_id);

	GOEP_MEMSET(&ilm, 0, sizeof(ilm_struct));
	ilm.msg_id = msg_id;
	ilm.src_mod_id = MOD_EXTERNAL;
	ilm.dest_mod_id = dest_mod;

	assert (size <= MAX_ILM_BUFFER_SIZE);

	if(ptr != NULL)
		memcpy(ilm.ilm_data, ptr, size);

	/* LOCAL_PARA_HDR */
	field = (U16 *)ilm.ilm_data;
	*field = 1;
	field = (U16 *)&ilm.ilm_data[2];
	*field = size;

	// cal whole ilm size
	size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

	if( GOPP(apisock) >= 0 )
	{
		ret = send(GOPP(apisock), (void *)&ilm, size, 0);
		if(ret < 0)
		{
			bt_ext_err("[EXT][BTCMD_SendMessage]: send fail: %s, %d", strerror(errno), errno);
		}
		else
		{
			//BT_OPP_LOGD("send msg success : %d", ret);
		}
	}
	else
	{
		bt_ext_err("[EXT][BTCMD_SendMessage]: socket not initialized");
	}
}

void stopListenMessage()
{
	int servsock;
	struct sockaddr_un name;
	socklen_t namelen;
	int ret = -1;
	ilm_struct ilm;
	U16 size;

	BT_OPP_LOGD( "[EXT][stopListenMessage][+]: servsock:[%d]", GOPP(servsock) );

	if ( GOPP(servsock) >= 0 )
	{
		// create socket
		servsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
		if (servsock < 0)
		{
			bt_ext_err("[EXT][stopListenMessage]: create socket failed - %s(errno=%d)", strerror(errno), errno);
			return;
		}
		// create socket
		if( socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_OPP, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&name, &namelen) < 0)
		{
			bt_ext_err("[EXT][stopListenMessage]: create socket failed - %s(errno=%d)", strerror(errno), errno);
		}
		if ( connect(servsock, (const struct sockaddr*)&name, namelen) < 0 )
		{
			bt_ext_err("[EXT][stopListenMessage]: connect to %s (bttask) failed - %s(errno=%d)", name.sun_path, strerror(errno), errno);
			close(servsock);
			servsock = -1;
			return;
		}

		// construct terminate request
		GOEP_MEMSET(&ilm, 0, sizeof(ilm_struct));
		ilm.msg_id = ULONG_MAX;	// terminate current connection
		ilm.src_mod_id = MOD_EXTERNAL;
		ilm.dest_mod_id = MOD_BT;
		size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE;

		// send request
		ret = send( servsock, (void *)&ilm, size, 0 );
		if( ret >= 0 )
		{
			BT_OPP_LOGD( "[EXT][stopListenMessage]: send shutdown request." );
		}
		else
		{
			bt_ext_err("[EXT][stopListenMessage]: send shutdown request failed: %s(%d)", strerror(errno), errno);
		}
		close(servsock);
		servsock = -1;
	}
}

void startListenMessage()
{
	int res;
	fd_set readfs;
	int sockfd;
	ilm_struct ilm;

	// get servsock ( initialized by enable process )
	sockfd = GOPP(servsock);

	BT_OPP_LOGD("[EXT][startListenMessage][+]: GOPP(servsock)=%d", sockfd);

	while(1)
	{
		FD_ZERO(&readfs);
		if( sockfd >= 0 )
		{
			FD_SET(sockfd, &readfs);
		}
		else
		{
			bt_ext_err("[EXT][startListenMessage]: GOPP(servsock) is NULL. exit");
		}

		res = select(sockfd+1, &readfs, NULL, NULL, NULL);

		if( res > 0 )
		{
			res = recvfrom(sockfd, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
			BT_OPP_LOGD("[EXT][startListenMessage]: recv msg(%lu) from bttask.", ilm.msg_id);

			if(res < 0)
			{
				if(errno == EAGAIN)
				{
					res = 0;
				}
				else
				{
					bt_ext_err("[EXT][startListenMessage]: recvfrom failed : %s(%d)", strerror(errno), errno);
				}
			}
			else
			{
				if( ( (MSG_ID_BT_GOEPC_GROUP_START <= ilm.msg_id) && (ilm.msg_id <= MSG_ID_BT_GOEPC_GROUP_END) ) ||
					( (MSG_ID_BT_OPPC_GROUP_START  <= ilm.msg_id) && (ilm.msg_id <= MSG_ID_BT_OPPC_GROUP_END ) ) )
				{
					// translate share memory
					if (ilm.msg_id == MSG_ID_BT_OPPC_PULL_CNF)
					{
						((goep_pull_cnf_struct*)(ilm.ilm_data))->frag_ptr = GOPC(opc_push_buff);
					}
					btmtk_oppc_handle_message(&ilm);
				}
				else if( ( (MSG_ID_BT_GOEPS_GROUP_START <= ilm.msg_id) && (ilm.msg_id <= MSG_ID_BT_GOEPS_GROUP_END) ) ||
						 ( (MSG_ID_BT_OPPS_GROUP_START  <= ilm.msg_id) && (ilm.msg_id <= MSG_ID_BT_OPPS_GROUP_END ) ) )
				{
					// translate share memory
					if (ilm.msg_id == MSG_ID_BT_OPPS_PUSH_IND)
					{
#ifdef __BT_GOEPS_AUTO_ACK__
					    goep_push_ind_struct *ind = ((goep_push_ind_struct*)(ilm.ilm_data));
						ind->frag_ptr = GOPS(ops_push_buff) + (OPPS_MAX_OBEX_PACKET_LENGTH * ind->buff_index);
#else
						((goep_push_ind_struct*)(ilm.ilm_data))->frag_ptr = GOPS(ops_push_buff);
#endif
					}
					btmtk_opps_handle_message(&ilm);
				}
				else if( ilm.msg_id == ULONG_MAX )
				{
					// terminate request
					BT_OPP_LOGD("[EXT][startListenMessage]: server shutdown request received.");
					break;
				}
				else
				{
					bt_ext_err("[EXT][startListenMessage]: Unexpected message: %lu", ilm.msg_id);
				}
			}
		}
		else
		{
			bt_ext_err("[EXT][startListenMessage]: select failed: %s(%d)", strerror(errno), errno);
			if( errno != EINTR )
			{
				break;
			}  	
		}
	}

	// close socket after exit loop
	if( GOPP(servsock) >= 0 )
	{
		BT_OPP_LOGD("[EXT][startListenMessage]: closing opp server socket(GOPP(servsock))...");
		close(GOPP(servsock));
		GOPP(servsock) = -1;
	}
}
