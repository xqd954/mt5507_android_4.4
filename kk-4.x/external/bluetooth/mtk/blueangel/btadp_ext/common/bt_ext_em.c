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
 * Bt_ext_apis.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to expose common api for profile&gap
 *
 * Author:
 * -------
 * Liang Cheng
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

#include "utils/Log.h"

#include <stdlib.h>
#include <hardware/bluetooth.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "cutils/sockets.h"

#include "bt_gap_struct.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"

bool bt_sendmsg(int sock, void* ptr, int size)
{
    int ret = -1;
    ilm_struct *ilm = (ilm_struct*)ptr;
    U16 *field;
    struct sockaddr_un servname;
    socklen_t servnamelen;    

    /* LOCAL_PARA_HDR */
    field = (U16 *)ilm->ilm_data;
    *field = 1;
    *(++field) = size;

    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ALOGI("[JNI] bt_sendmsg(cmd=%lu, ptr=%p, len=%d)", ilm->msg_id, ptr, size);
    servname.sun_family = AF_UNIX;
    strcpy (servname.sun_path, BT_SOCK_NAME_INT_ADP);
    servnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (servname.sun_path) + 1);

    if(sock >= 0)
    {
        ret = sendto(sock, ptr, size, 0, (const struct sockaddr*)&servname, servnamelen);
        if(ret < 0)
        {
            ALOGE("[JNI] send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            ALOGI("[JNI] send msg success : %d", ret);
        }
    }
    else
    {
        ALOGE("[JNI] server socket uninitialized");
    }
    return (ret >= 0) ? true : false;
}

/* -1 : fail, 0 : timeout, 1: success */
int wait_response(int sock, unsigned long msg_id, ilm_struct* ilm, int timeout /* millisecond */)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;
    struct timeval orignalTime;
    struct timeval currentTime;
    struct timezone zone;
    int ret;
    int currentTimeout = timeout;
    
    FD_ZERO(&readfs);
    if(sock >= 0)
    {
        FD_SET(sock, &readfs);
    }
    else
    {
        ALOGE("[JNI][ERR] sock == 0. exit");
    }
    
    ret = gettimeofday(&orignalTime, &zone);
    if (0 != ret)
    {
         ALOGE("[JNI] gettimeofday failed : %s, %d", strerror(errno), errno);
         memset(&currentTime, 0x0, sizeof(struct timeval));
    }
    while (TRUE)
    {
        Timeout.tv_usec = (currentTimeout%1000)*1000;
        Timeout.tv_sec  = currentTimeout/1000;
        res = TEMP_FAILURE_RETRY(select(sock+1, &readfs, NULL, NULL, &Timeout));
        if(res > 0)
        {
            res = recvfrom(sock, ilm, sizeof(ilm_struct), 0, NULL, NULL);
            if(res < 0)
            {
                ALOGE("[JNI] recvfrom failed : %s, %d", strerror(errno), errno);
                break;
            }
            else
            {
                if( ilm->msg_id != msg_id)
                {
                    ALOGE("[JNI] recv res bytes=%d, resp=%lu is mismatch, expected : %lu", res, ilm->msg_id, msg_id);
                }
                else
                {
                    break;
                }
            }
        }
        else if(res == 0)
        {
            ALOGW("[JNI] timeout waiting response, in %d milliseconds", currentTimeout);
            break;
        }
        else
        {
            ALOGE("[JNI] select failed: %s, %d", strerror(errno), errno);
            break;
        }
        
        ret = gettimeofday(&currentTime, &zone);
        if (0 != ret)
        {
            ALOGE("[JNI] gettimeofday failed : %s, %d", strerror(errno), errno);
            break;;
        }
        currentTimeout = (currentTime.tv_sec - orignalTime.tv_sec) * 1000 + (currentTime.tv_usec/1000 - orignalTime.tv_usec/1000);
        if (currentTimeout < timeout)
        {
            currentTimeout = timeout - currentTimeout;
            ALOGW("[JNI] continue to wait response for %d(us)", currentTimeout);
        }
        else
        {
            ALOGW("[JNI] timeout waiting response, in %d milliseconds", timeout);
            res = -1;
            break;
        }
    }
    return res;
}

static int g_EmServSock = -1;
#define BT_SOCK_NAME_EXT_ADP_EM "bt.ext.adp.em"

bool bt_set_ssp_debug_mode(bool on)
{
    bool ret = false;
    ilm_struct ilm;
    bt_ssp_debug_mode_req_struct *msg_p;
  
	if(g_EmServSock < 0)
	{
        g_EmServSock = socket_local_server(BT_SOCK_NAME_EXT_ADP_EM, 
                                           ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
                                           SOCK_DGRAM);
    }
    ALOGI("[GAP] btmtk_gap_set_ssp_debug_mode: on = 0x%X, em socket = %d\n", on, g_EmServSock);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    msg_p = (bt_ssp_debug_mode_req_struct*)ilm.ilm_data;
    ilm.msg_id = MSG_ID_BT_SSP_DEBUG_MODE_REQ;
    msg_p->on = on;
    
    if(bt_sendmsg(g_EmServSock, (void*)&ilm, sizeof(bt_ssp_debug_mode_req_struct)) &&
       wait_response(g_EmServSock, MSG_ID_BT_SSP_DEBUG_MODE_CNF, &ilm, BTMTK_MAX_STACK_TIMEOUT)  > 0)
    {
        bt_ssp_debug_mode_cnf_struct *cnf_p = (bt_ssp_debug_mode_cnf_struct*)ilm.ilm_data;;

        ALOGI("[GAP] MSG_ID_BT_SSP_DEBUG_MODE_CNF: result=%d", cnf_p->result);
        ret = cnf_p->result;
    }
    
    if(!ret)
    {
        ALOGE("[GAP] btmtk_gap_set_ssp_debug_mode failed");
    }
    return ret;
}
