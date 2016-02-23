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
/*******************************************************************************
*
* Filename:
* ---------
* bt_pbap_api.c
*
* Project:
* --------
*   BT Project
*
* Description:
* ------------
*   This file is used to
*
* Author:
* -------
* Xueling Li
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
#define LOG_TAG "BTEXTADP_PBAP"

#include "string.h"
#include "bt_types.h"
#include "bt_os_api.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bt_mmi_msg.h"
#include "bluetooth_pbap_struct.h"
#include "bluetooth_pbap_message.h"
#include "bt_pbap_api.h"
#include "bt_ext_debug.h"
#include "bt_config.h"
#include <errno.h>
#include <sys/socket.h>

/************************************************************************
*   Transport function
************************************************************************/
#define RESPONSE_TIMEOUT 3000 /* ms */
int BT_SendMessage(int sockfd, msg_type msg_id, void *ptr, U16 size)
{
    int ret;
    ilm_struct *ilm;
    U16 *field;
	
    bt_ext_log("msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ptr, size);

    ret = -1;
    ilm = (ilm_struct*)ptr;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = msg_id;

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    field = (U16 *)&ilm->ilm_data[2];
    *field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd)
    {
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0)
        {
            bt_ext_err("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            bt_ext_log("send msg success : %d", ret);
        }
    }
    else
    {
        bt_ext_err("socket uninitialized");
    }
    return (ret >= 0) ? 0 : -1;
}

int BT_WaitResponse(int sockfd, msg_type resp_id, ilm_struct *ilm, U16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;

    Timeout.tv_usec = (RESPONSE_TIMEOUT%1000)*1000;
    Timeout.tv_sec  = RESPONSE_TIMEOUT/1000;
    FD_ZERO(&readfs);
    if(sockfd)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        bt_ext_err("[JNI][ERR] nat->servsock == 0. exit");
    }
    res = TEMP_FAILURE_RETRY(select(sockfd+1, &readfs, NULL, NULL, &Timeout));
    if(res > 0)
    {
        res = recvfrom(sockfd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
        //bt_ext_log("[JNI] recv %d bytes resp", res);
        if(res < 0)
        {
            bt_ext_err("[JNI] recvfrom failed : %s, %d", strerror(errno), errno);
        }
        else
        {
            if( ilm->msg_id != resp_id)
            {
                res = -1;
                bt_ext_err("[JNI] recv resp=%u is mismatch, expected : %u", (unsigned int)ilm->msg_id, (unsigned int)resp_id);
            }
            else
            {
                *size = res;            
                bt_ext_log("[JNI] recv resp=%u, bytes=%d", (unsigned int)ilm->msg_id, res);
            }
        }
    }
    else if(res == 0)
    {
        bt_ext_err("[JNI] timeout waiting response, in %d milliseconds", RESPONSE_TIMEOUT);
    }
    else
    {
        bt_ext_err("[JNI] wait select failed : %s, %d", strerror(errno), errno);
    }
    return (res>0) ? 0 : -1;
}


kal_bool btmtk_pbap_send_active_req(int sockfd, U8 security_level, U8 support_repos, struct sockaddr_un *name, socklen_t namelen)
{
    ilm_struct ilm;
    bt_pbap_register_req_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_register_req_struct*)ilm.ilm_data;
    msg_p->security_level = security_level;
    msg_p->support_repositories = support_repos;
    msg_p->addr = *name;
    msg_p->addrlen = namelen;
    bt_ext_log("btmtk_pbap_send_active_req");
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_REGISTER_REQ, &ilm, sizeof(bt_pbap_register_req_struct));
    if( ret >= 0)
    {
        U16 size = 0;
        ret = BT_WaitResponse(sockfd, MSG_ID_BT_PBAP_REGISTER_CNF, &ilm, &size);
        if(ret >= 0)
            return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_active_req failed");
    return KAL_FALSE;
}

kal_bool btmtk_pbap_send_deactive_req(int sockfd)
{
    ilm_struct ilm;
    bt_pbap_deregister_req_struct *msg_p;
    int ret;
    bt_ext_log("btmtk_pbap_send_deactive_req");
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_DEREGISTER_REQ, &ilm, sizeof(bt_pbap_deregister_req_struct));
    if( ret >= 0)
    {
        U16 size = 0;
        ret = BT_WaitResponse(sockfd, MSG_ID_BT_PBAP_DEREGISTER_CNF, &ilm, &size);
        if(ret >= 0)
            return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_deactive_req failed");
    return KAL_FALSE;
}

kal_bool btmtk_pbap_send_authorize_rsp(int sockfd,  U8 cnf_code)
{
    ilm_struct ilm;
    bt_pbap_authorize_rsp_struct *msg_p;
    int ret;
    
    msg_p = (bt_pbap_authorize_rsp_struct*)ilm.ilm_data;
    msg_p->cnf_code = cnf_code;
    bt_ext_log("btmtk_pbap_send_authorize_rsp");
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_AUTHORIZE_RSP, &ilm, sizeof(bt_pbap_authorize_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_authorize_rsp failed");
    return KAL_FALSE;
}

kal_bool btmtk_pbap_send_connect_rsp(int sockfd, U32 cm_conn_id,U8 cnf_code)
{
    ilm_struct ilm;
    bt_pbap_client_connect_rsp_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_client_connect_rsp_struct*)ilm.ilm_data;
    msg_p->cm_conn_id = cm_conn_id;
    msg_p->cnf_code = cnf_code;
    bt_ext_log("btmtk_pbap_send_connect_rsp");
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_CLIENT_CONNECT_RSP, &ilm, sizeof(bt_pbap_client_connect_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_connect_rsp failed");
    return KAL_FALSE;
}


kal_bool btmtk_pbap_send_disconnect_req(int sockfd,  
									U32 cm_conn_id,
									U8  disconnect_tp_directly)
{
    ilm_struct ilm;
    bt_pbap_disconnect_req_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_disconnect_req_struct*)ilm.ilm_data;
    msg_p->cm_conn_id = cm_conn_id;
    msg_p->disconnect_tp_directly =(kal_bool)disconnect_tp_directly;
    bt_ext_log("btmtk_pbap_send_disconnect_req");

    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_DISCONNECT_REQ, &ilm, sizeof(bt_pbap_disconnect_req_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_disconnect_req failed");
    return KAL_FALSE;
}

kal_bool btmtk_pbap_send_set_path_rsp(int sockfd,  U8 result)
{
    ilm_struct ilm;
    bt_pbap_set_path_rsp_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_set_path_rsp_struct*)ilm.ilm_data;
    msg_p->result = result;
    bt_ext_log("btmtk_pbap_send_set_path_rsp");

    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_SET_PATH_RSP, &ilm, sizeof(bt_pbap_set_path_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_set_path_rsp failed");
    return KAL_FALSE;    
}
kal_bool btmtk_pbap_send_read_entry_rsp(int sockfd, 
											U8 result,
											U8* file_name,
											U16 name_len,
											U8  cont)
{
    ilm_struct ilm;
    bt_pbap_read_entry_rsp_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_read_entry_rsp_struct*)ilm.ilm_data;
    msg_p->result = result;
    msg_p->cont = cont;
    memset(msg_p->pbap_data_file, 0x00, BT_PBAP_MAX_FILEPATH_NAME_LEN);
    if(file_name)
    {
        memcpy(msg_p->pbap_data_file, file_name, name_len);
    }
    bt_ext_log("btmtk_pbap_send_read_entry_rsp");

    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_READ_ENTRY_RSP, &ilm, sizeof(bt_pbap_read_entry_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_read_entry_rsp failed");
    return KAL_FALSE;
}
kal_bool btmtk_pbap_send_read_folder_rsp(int sockfd,
									 U8 result,
									 U16 phoneBookSize,
									 U16 newMissedCalls,
									 U8* file_name,
									 U16 name_len,
									 U8 cont)
{
    ilm_struct ilm;
    bt_pbap_read_folder_rsp_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_read_folder_rsp_struct*)ilm.ilm_data;
    msg_p->result = result;
    msg_p->phoneBookSize =phoneBookSize;
    msg_p->newMissedCalls = newMissedCalls;
    msg_p->cont = cont;
    memset(msg_p->pbap_data_file, 0x00, BT_PBAP_MAX_FILEPATH_NAME_LEN);
    if(file_name)
    {
        memcpy(msg_p->pbap_data_file, file_name, name_len);
    }
    bt_ext_log("btmtk_pbap_send_read_folder_rsp");
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_READ_FOLDER_RSP, &ilm, sizeof(bt_pbap_read_folder_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_read_folder_rsp failed");
    return KAL_FALSE;
}
kal_bool btmtk_pbap_send_read_list_rsp(int sockfd,
                                                                                    U8 result,
                                                                                    U16 phoneBookSize,
                                                                                    U16 newMissedCalls,
                                                                                    U8* file_name,
                                                                                    U16 name_len,
                                                                                    U8 cont)
{
    ilm_struct ilm;
    bt_pbap_read_list_rsp_struct *msg_p;
    int ret;

    msg_p = (bt_pbap_read_list_rsp_struct*)ilm.ilm_data;
    msg_p->result = result;
    msg_p->phoneBookSize =phoneBookSize;
    msg_p->newMissedCalls = newMissedCalls;
    msg_p->cont = cont;
    memset(msg_p->pbap_data_file, 0x00, BT_PBAP_MAX_FILEPATH_NAME_LEN);
    if(file_name && name_len)
    {
        memcpy(msg_p->pbap_data_file, file_name, name_len);
    }
    bt_ext_log("btmtk_pbap_send_read_list_rsp");
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_READ_LIST_RSP, &ilm, sizeof(bt_pbap_read_list_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_read_list_rsp failed");
    return KAL_FALSE;
}

kal_bool btmtk_pbap_send_obex_auth_challege_rsp(int sockfd,
                                                                                                            U8 cancel,
                                                                                                            const char* password,  U16 password_length,
                                                                                                            const char* userID, U16 userID_length)
											
{
    ilm_struct ilm;
    bt_pbap_client_auth_challenge_rsp_struct *rsp_p;
    int ret;
    U16 len;
    
    bt_ext_log("btmtk_pbap_send_obex_auth_challege_rsp(%d, %s,%d,%s,%d)", 
                        cancel, 
                        password?password:"null", 
                        password_length,
                        userID?userID:"null", 
                        userID_length);
    rsp_p = (bt_pbap_client_auth_challenge_rsp_struct*)ilm.ilm_data;
    rsp_p->cancel = cancel;
    memset(rsp_p->password, 0x00, 20);
    memset(rsp_p->userId, 0x00, 20);
    /* len = strlen((const char *)password); */
    if(password != NULL)
    {
        len = strlen((const char *)password);
        if(len < 20)
            memcpy(rsp_p->password,password, len);
    }	
    if(userID != NULL)
    {
        len = strlen((const char *)userID);
        if(len < 20)
            memcpy(rsp_p->userId,userID, len);
    }
    ret = BT_SendMessage(sockfd, MSG_ID_BT_PBAP_CLIENT_AUTH_CHALLENGE_RSP, &ilm, sizeof(bt_pbap_client_auth_challenge_rsp_struct));
    if( ret >= 0)
    {
        return KAL_TRUE;
    }
    bt_ext_err("btmtk_pbap_send_obex_auth_challege_rsp failed");
    return KAL_FALSE;
}


/********************************************************************************
*	Description	: 
********************************************************************************/
void btmtk_pbap_authorize_res(unsigned char AuthMode)
{
     bt_ext_log("btmtk_pbap_authorize_res(%d)", AuthMode);
#if defined(__SYNC_API__)
#else     
     btmtk_pbap_send_authorize_rsp(MOD_MMI, AuthMode);
#endif
}


/********************************************************************************
*	Description	: 
********************************************************************************/
#if 0
kal_bool btmtk_pbap_server_enable (void)
{
    /*default security level is 2, */
	/*MBT_PBAP_INTERNAL = MBT_TRUE is mandatory
	MBT_PBAP_SIM  = MBT_TRUE is optional
	*/
    U8  support_repos = 0x01;
    bt_ext_log("btmtk_pbap_server_enable");
    btmtk_pbap_send_active_req(MOD_MMI, 0x02, support_repos);
    return KAL_TRUE;
}
#endif
/********************************************************************************
*	Description	: 
********************************************************************************/
void btmtk_pbap_server_disable (void)
{
    bt_ext_log("btmtk_pbap_server_disable");
    btmtk_pbap_send_deactive_req(MOD_MMI); 
}


/********************************************************************************
*	Description	: 
********************************************************************************/
kal_bool btmtk_pbap_server_authenticate(char *p_password, char *p_userid)
{
	/*get length of pass word*/
    bt_ext_log("btmtk_pbap_server_authenticate(%s, %s)", p_password?p_password:"null", p_userid?p_userid:"null");
    btmtk_pbap_send_obex_auth_challege_rsp(MOD_MMI,0x00, p_password, 20,p_userid,20);
    return KAL_TRUE;
}

/********************************************************************************
*	Description	: 
********************************************************************************/
kal_bool btmtk_pbap_server_close(void)
{
    bt_ext_log("btmtk_pbap_server_close");
    btmtk_pbap_send_disconnect_req(MOD_MMI, 0x00, 0x00);
    return KAL_TRUE;
}

/********************************************************************************
*	Description	: 
********************************************************************************/
