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
 * bt_simap_porting.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is the header file of Porting layer API of SIMAP profile used by MMI.
 *   Flow direction: APP --> Porting layer API
 *
 * Author:
 * -------
 * 
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "bt_struct.h"
#include "bt_types.h"
#include "bluetooth_simap_struct.h"
#include "bluetooth_gap_struct.h"
#include "bt_simap_api.h"
#include "bt_simap_porting.h"
#include "bt_gap_api.h"
#include "bluetooth_trc.h"
#include "ps_trace.h"
#include "bt_debug.h"
#include "memory.h"


#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "bt_ext_debug.h"

/* Android socket api */
#include "cutils/sockets.h"

#undef  LOG_TAG
#define LOG_TAG "BTSIMAP_PORTING"

static int g_ext_simap_sockfd = -1;
static struct sockaddr_un g_ext_simap_sockaddr;
static socklen_t g_ext_simap_sockaddr_len;

static struct sockaddr_un g_int_adp_sockaddr;
static socklen_t g_int_adp_sockaddr_len;


extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);


/*****************************************************************************
 * FUNCTION
 *  bt_simap_init_socket
 * DESCRIPTION
 *  This function initializes the socket for receiving ilm from bt task and
 *  stores the address of sending socket.
 * PARAMETERS
 *  none.        
 * RETURNS
 *  Created socket file descriptor or -1 (socket-creation failed)
 *****************************************************************************/
int bt_simap_init_socket()
{
    char buff[20];
    int err, sock_fl;

    err = socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_SIMAP,
				  ANDROID_SOCKET_NAMESPACE_ABSTRACT,
				  &g_ext_simap_sockaddr, &g_ext_simap_sockaddr_len);

    g_ext_simap_sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (g_ext_simap_sockfd < 0) {
		bt_ext_err("Create server socket failed.");
		return -1;
    }
    if (bind(g_ext_simap_sockfd, (struct sockaddr*) &g_ext_simap_sockaddr,
		 		g_ext_simap_sockaddr_len) < 0) {
		bt_ext_err("Bind server socket failed.");
		return -1;
    }

    g_int_adp_sockaddr.sun_family = AF_UNIX;
    strcpy(g_int_adp_sockaddr.sun_path, BT_SOCK_NAME_INT_ADP);
    g_int_adp_sockaddr_len = (
					offsetof(struct sockaddr_un, sun_path) + 
					strlen(g_int_adp_sockaddr.sun_path) + 1);

    return g_ext_simap_sockfd;
}

/** 
* @brief Clear data before read from the socket
*/
int bt_simap_clear_socket_data() {
    char buff[20];
    int err, sock_fl;

    /* Get current socket flags */
    sock_fl = fcntl(g_ext_simap_sockfd, F_GETFL);
    if (sock_fl == -1) {
	err = errno;
	bt_ext_err("Get socket flags failed. Reason: [%d]%s", err, strerror(err));
	return -1;
    }

    /* Set ftp socket as non-block */
    if (fcntl(g_ext_simap_sockfd, F_SETFL, sock_fl | O_NONBLOCK) == -1) {
	err = errno;
	bt_ext_err("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
	return -1;
    }

    /* Read data from socket till empty */
    while(0 < recvfrom(g_ext_simap_sockfd, (void*) buff, sizeof(buff), 0, NULL, NULL)) {
	bt_ext_log("Clearing socket...");
    }
    bt_ext_log("Socket clear.");

    /* Set back original flags */
    if (fcntl(g_ext_simap_sockfd, F_SETFL, sock_fl) == -1) {
	err = errno;
	bt_ext_err("Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
	return -1;
    }

    return 0;
}

void bt_simap_deinit_socket()
{
	bt_ext_log(" bt_simap_deinit_socket...");

	if (g_ext_simap_sockfd >= 0)
	{
		close(g_ext_simap_sockfd);
		g_ext_simap_sockfd = -1;
	}
}


/**
* @brief Send ilm via socket
*/
static void bt_simap_send_msg_socket(msg_type msg_id, module_type dest_mod,
				   void *ptr, U16 size) {
    int ret = -1;
    struct sockaddr_un* addr;
    socklen_t* addr_len;
    ilm_struct ilm;
    U16 *field;

    bt_ext_log("bt_simap_send_msg_socket, msg:[%d] mod:[%d]", msg_id, dest_mod);

    memset(&ilm, 0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.src_mod_id = MOD_EXTERNAL;
    ilm.dest_mod_id = dest_mod;

    //assert (size <= MAX_ILM_BUFFER_SIZE);

    if(ptr != NULL)
		memcpy(ilm.ilm_data, ptr, size);

    // LOCAL_PARA_HDR
    field = (U16 *)ilm.ilm_data;
    *field = 1;
    field = (U16 *)&ilm.ilm_data[2];
    *field = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

    if (g_ext_simap_sockfd) {
		if (dest_mod == MOD_EXTERNAL) {
		    addr = &g_ext_simap_sockaddr;
		    addr_len = &g_ext_simap_sockaddr_len;
		} else {
		    addr = &g_int_adp_sockaddr;
		    addr_len = &g_int_adp_sockaddr_len;
		}

		ret = sendto(g_ext_simap_sockfd, (void *)&ilm, size, 0,
			     		(struct sockaddr*) addr, *addr_len);
		if (ret < 0) {
		    bt_ext_err("Sending msg failed: %s, %d", strerror(errno), errno);
		} else {
		     //bt_ext_log("Sending msg succeeded: %d", ret);
		}

    } else {
		bt_ext_err("Socket is not yet initialized.");
    }
}



/*****************************************************************************
 * FUNCTION
 *  bt_simap_send_msg
 * DESCRIPTION
 *  This function is to send msg to BT task
 * PARAMETERS
 *  msg_id              [IN]        
 *  p_local_para               
 * RETURNS
 *  void
 *****************************************************************************/
void bt_simap_send_msg(U32 msg_id, void *p_local_para, U32 u4Size)
{
    //btmtk_goep_show_id( msg_id, TRUE, 2); //MOD_MMI

    bt_simap_send_msg_socket(msg_id, MOD_BT, p_local_para, (U16) u4Size);

	/*
    //  NOTE: free the p_local_para
    if (NULL != p_local_para ) {
        FREE_LOCAL_PARA((void*)p_local_para);
    }
    */
}

void bt_simap_send_msg_toself(U32 msg_id, void *p_local_para, U32 u4Size)
{
    // NOTE: send msg
    //btmtk_goep_show_id( msg_id, TRUE, 2); //MOD_MMI

    bt_simap_send_msg_socket(msg_id, MOD_EXTERNAL, p_local_para, (U16) u4Size);

	/*
    //  NOTE: free the p_local_para
    if (NULL != p_local_para ) {
        FREE_LOCAL_PARA((void*)p_local_para);
    }
    */
}






