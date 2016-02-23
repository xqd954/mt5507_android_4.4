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

#include <stdio.h>
#include "string.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_mmi_msg.h"
#include "bluetooth_struct.h"
//#include "bt_ext_system.h"
#include <errno.h>
#include <sys/socket.h>
//#include "bt_ext_debug.h"
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include "cutils/sockets.h"

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);

int sendMessage(int sockfd, msg_type msg_id, void *ptr, kal_uint16 size)
{
    int ret = -1;
    ilm_struct *ilm;
    U16 *field;
	
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
    *(++field) = size;

    // cal whole ilm size
    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd)
    {
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0)
        {
            printf("send msg fail : %s, %d\r", strerror(errno), errno);
        }
        else
        {
            printf("send msg success : %d\r", ret);
        }
    }
    else
    {
        printf("socket uninitialized\r");
    }
    return ret;
}

int waitResponse(int sockfd, msg_type resp_id, ilm_struct *ilm, kal_uint16 *size)
{
    int res = 0;
    fd_set readfs;
    struct timeval Timeout;

    Timeout.tv_usec = 0;
    Timeout.tv_sec  = 5;
    FD_ZERO(&readfs);
    if(sockfd)
    {
        FD_SET(sockfd, &readfs);
    }
    else
    {
        printf("sockfd == 0. exit\n");
    }
    res = select(sockfd+1, &readfs, NULL, NULL, &Timeout);
    if(res > 0)
    {
        res = recvfrom(sockfd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
        if(res < 0)
        {
            printf("[JNI] recvfrom failed : %s, %d\n", strerror(errno), errno);
        }
        else
        {
            if( ilm->msg_id != resp_id)
            {
                res = -1;
                printf("[JNI] recv resp=%u is mismatch, expected : %u\n", (unsigned int)ilm->msg_id, (unsigned int)resp_id);
            }
            else
            {
                *size = res;            
                printf("[JNI] recv resp=%u, bytes=%d\n", (unsigned int)ilm->msg_id, res);
            }
        }
    }
    else if(res == 0)
    {
        printf("[JNI] timeout waiting response, in 5 milliseconds\n");
    }
    else
    {
        printf("[JNI] wait select failed : %s, %d\n", strerror(errno), errno);
    }
    return res;
}

int openSocket()
{
	int fd = -1;
  struct sockaddr_un addr;
  socklen_t   addrlen;
  struct sockaddr_un btname;
  socklen_t   btnamelen;
  
  /* Set bt server address */
  btname.sun_family = AF_UNIX;
  strcpy (btname.sun_path, BT_SOCK_NAME_INT_ADP);
  btnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (btname.sun_path) + 1);
  /* set socket name */
  socket_make_sockaddr_un("btlogmask", ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&addr, &addrlen);

	fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if(fd < 0)
	{
		printf("create socket failed\n");
		return -1;
	}	
  if(bind (fd, (struct sockaddr *)&addr, addrlen) < 0)
  {
      printf("[ERR] bind hfg api socket failed : %s, errno=%d\n", strerror(errno), errno);
      close(fd);
      return -1;
  }
  if(connect(fd, (const struct sockaddr*)&btname, btnamelen) < 0)
  {
      printf("[ERR] connect to /data/btserv failed : %s, errno=%d\n", strerror(errno), errno);
      close(fd);
      return -1;
  }
  return fd;
}

int setLogMask(int sockfd, kal_uint32 trace, kal_uint8 primitive, kal_uint8 prompt)
{
	ilm_struct ilm;
	kal_uint16 size = sizeof(bt_engineer_mode_log_mask_cnf_struct);
	
	bt_engineer_mode_log_mask_req_struct *req_p = (bt_engineer_mode_log_mask_req_struct*)ilm.ilm_data;
	printf("set log mask to : trace=0x%lx, primitive=%u, prompt=%u\n", trace, primitive, prompt);
	req_p->trace = trace;
	req_p->primitive = primitive;
	req_p->prompt = prompt;
	if(sendMessage(sockfd, MSG_ID_BT_ENGINEER_MODE_LOG_MASK_REQ, (void*)&ilm, sizeof(bt_engineer_mode_log_mask_req_struct)) < 0 || 
		 waitResponse(sockfd, MSG_ID_BT_ENGINEER_MODE_LOG_MASK_CNF, &ilm, &size) < 0)
	{		 
		printf("set log mask failed\n");
		return -1;
	}
	else
	{
		bt_engineer_mode_log_mask_cnf_struct *cnf_p = (bt_engineer_mode_log_mask_cnf_struct*)ilm.ilm_data;
		if(cnf_p->result == 0)
		{
			printf("set log mask failed\n");
			return -1;
		}
	}
	printf("set log mask success\n");
	return 0;	
}

int main (int argc, char **argv)
{
	int fd = -1;
	kal_uint32 trace = 0;
	kal_uint8 primitive = 0;
	kal_uint8 prompt = 0;
	if(argc < 4)
	{
		printf("Usage : logmask trace primitive prompt\n");
		printf("trace : bit mask of trace group\n");
		printf("primitive : set other than 0 to mask primitive log\n");
		printf("prompt : set other than 0 to mask prompt log\n");
		return 0;
	}
	else
	{
		trace = strtoul(argv[1], NULL, 16);
		primitive = atol(argv[2]);
		prompt = atol(argv[3]);
	}
	
	fd = openSocket();
	if(fd < 0)
		return -1;
	setLogMask(fd, trace, primitive, prompt);
	return 0;
}
