/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cutils/sockets.h>

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_ext_debug.h"

#include "ext_interlayer.h"

/**
 * create local/datagram socket and connect to specified addr
 * return socket descriptor (>=0) or -1 if failed
 */
int create_client_socket(const char* name, ext_interlayer_socket_namespace_enum namespace)
{
    // create socket + connect to peer
    int fd = socket_local_client(name, namespace, SOCK_DGRAM);
    if (fd < 0)
    {
        bt_ext_err("[EXT][ext_interlayer.c][create_client_socket()]: socket_local_client() failed - %s(errno=%d)", strerror(errno), errno);
    }
    return fd;
}

int create_server_socket(const char* name, ext_interlayer_socket_namespace_enum namespace)
{
    bt_ext_log("[EXT][ext_interlayer.c][create_server_socket()][+]");

    int fd = socket_local_server(name, namespace, SOCK_DGRAM);
    if (fd < 0)
    {
        bt_ext_err("[EXT][ext_interlayer.c][create_server_socket()]: socket_local_server() failed - %s(errno=%d)", strerror(errno), errno);
    }
    return fd;
}

void close_socket(int socketfd)
{
    if (socketfd >=0 )
    {
        close(socketfd);
    }
}

int recv_message_timeout(int socketfd, ilm_struct* ilm, unsigned int timeout){
    int res = -1;
    fd_set readfs;
    struct timeval Timeout;

    // check parameter
    if (socketfd < 0){
        bt_ext_err("[EXT][ext_interlayer.c][recvMessage()]: invalid parameter - socketfd[%d]", socketfd);
        return -1;
    }

    // select and wait for message
    FD_ZERO(&readfs);
    FD_SET(socketfd, &readfs);
    
    if(!timeout){
        res = TEMP_FAILURE_RETRY(select(socketfd+1, &readfs, NULL, NULL, NULL));
    }else{
        Timeout.tv_usec = (timeout%1000)*1000;
        Timeout.tv_sec  = timeout/1000;
        res = TEMP_FAILURE_RETRY(select(socketfd+1, &readfs, NULL, NULL, &Timeout));
    }

    // receive message and check result
    if (res > 0){
        res = recvfrom(socketfd, (void*)ilm, sizeof(ilm_struct), 0, NULL, NULL);
        if (res > 0){
            //bt_ext_log("[EXT][ext_interlayer.c][recvMessage()]: received message[%lu].", ilm->msg_id);

            // success
            return 1;
        }else{
            if (errno == EAGAIN){
                // TODO maybe need to enter recvMessage again
                bt_ext_err("[EXT][ext_interlayer.c][recvMessage()]: EAGAIN - %s(errno=%d)", strerror(errno), errno);
            }else{
                bt_ext_err("[EXT][ext_interlayer.c][recvMessage()]: recvfrom() failed - %s (errno=%ul)", strerror(errno), errno);
            }
        }
    }else if(res == 0){
        bt_ext_err("[EXT][ext_interlayer.c][recvMessage()]: select() timeout");
    }else{
        bt_ext_err("[EXT][ext_interlayer.c][recvMessage()]: select() failed - %s (errno=%ul)", strerror(errno), errno);
    }
    // failure
    return -1;
}


/**
 * receive message from specific socket and fill data into ilm_struct.
 * return 1 if success and -1 if failed.
 */
int recv_message(int socketfd, ilm_struct* ilm)
{
    return recv_message_timeout(socketfd, ilm, 0);
}

int send_message( int socketfd, ilm_struct* message, int size )
{
    int res = -1;

    // check parameters
    if (socketfd < 0 || message == NULL || size < 1 )
    {
        bt_ext_err("[EXT][ext_interlayer.c][sendMessage()]: invalid parameters - socketfd[%d], message*[%p], size[%d]", socketfd, message, size);
        return -1;
    }

    res = send(socketfd, (void *)message, size, 0);
    if (res < 0)
    {
        bt_ext_err("[EXT][ext_interlayer.c][sendMessage()]: send failed: %s(errno=%d)", strerror(errno), errno);
        return -1;
    }
    else
    {
        // success
        return 1;
    }
}

void print_message_id()
{
    // bluetooth_prx_message.h
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GROUP_START = %u;", MSG_ID_BT_PRXM_GROUP_START );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_CONNECT_REQ = %u;", MSG_ID_BT_PRXM_CONNECT_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_CONNECT_CNF = %u;", MSG_ID_BT_PRXM_CONNECT_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_DISCONNECT_REQ = %u;", MSG_ID_BT_PRXM_DISCONNECT_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_DISCONNECT_IND = %u;", MSG_ID_BT_PRXM_DISCONNECT_IND );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GET_CAPABILITY_REQ = %u;", MSG_ID_BT_PRXM_GET_CAPABILITY_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GET_CAPABILITY_CNF = %u;", MSG_ID_BT_PRXM_GET_CAPABILITY_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GET_REMOTE_TXPOWER_REQ = %u;", MSG_ID_BT_PRXM_GET_REMOTE_TXPOWER_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GET_REMOTE_TXPOWER_CNF = %u;", MSG_ID_BT_PRXM_GET_REMOTE_TXPOWER_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_SET_PATHLOSS_REQ = %u;", MSG_ID_BT_PRXM_SET_PATHLOSS_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_SET_PATHLOSS_CNF = %u;", MSG_ID_BT_PRXM_SET_PATHLOSS_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_SET_LINKLOSS_REQ = %u;", MSG_ID_BT_PRXM_SET_LINKLOSS_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_SET_LINKLOSS_CNF = %u;", MSG_ID_BT_PRXM_SET_LINKLOSS_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GET_RSSI_REQ = %u;", MSG_ID_BT_PRXM_GET_RSSI_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GET_RSSI_CNF = %u;", MSG_ID_BT_PRXM_GET_RSSI_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXM_GROUP_END = %u;", MSG_ID_BT_PRXM_GROUP_END );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_GROUP_START = %u;", MSG_ID_BT_PRXR_GROUP_START );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_REGISTER_REQ = %u;", MSG_ID_BT_PRXR_REGISTER_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_REGISTER_CNF = %u;", MSG_ID_BT_PRXR_REGISTER_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_DEREGISTER_REQ = %u;", MSG_ID_BT_PRXR_DEREGISTER_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_DEREGISTER_CNF = %u;", MSG_ID_BT_PRXR_DEREGISTER_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_AUTHORIZE_IND = %u;", MSG_ID_BT_PRXR_AUTHORIZE_IND );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_AUTHORIZE_RSP = %u;", MSG_ID_BT_PRXR_AUTHORIZE_RSP );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_CONNECT_IND = %u;", MSG_ID_BT_PRXR_CONNECT_IND );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_DISCONNECT_REQ = %u;", MSG_ID_BT_PRXR_DISCONNECT_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_DISCONNECT_IND = %u;", MSG_ID_BT_PRXR_DISCONNECT_IND );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_PATHLOSS_IND = %u;", MSG_ID_BT_PRXR_PATHLOSS_IND );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_LINKLOSS_IND = %u;", MSG_ID_BT_PRXR_LINKLOSS_IND );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_UPDATE_TXPOWER_REQ = %u;", MSG_ID_BT_PRXR_UPDATE_TXPOWER_REQ );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_UPDATE_TXPOWER_CNF = %u;", MSG_ID_BT_PRXR_UPDATE_TXPOWER_CNF );
    bt_ext_log( "public static final int MSG_ID_BT_PRXR_GROUP_END = %u;", MSG_ID_BT_PRXR_GROUP_END );
}
