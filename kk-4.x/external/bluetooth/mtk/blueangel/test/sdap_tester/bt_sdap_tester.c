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

#ifdef __SDAP_TESTER__

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "string.h"

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_sdap_tester_struct.h"

void printUsage() {
	printf("Usage: sdap_tester [address] [cmd]\n\n");
	printf("  address: Bluetooth device address in aa:bb:cc:dd:ee:ff format\n\n");
	printf("  cmd: (SA: SDAP_ServiceSearchAll, SM: SDAP_ServiceSearchMultipleAttribute)\n");
	printf("      1. SA + SA to Request Queue\n");
	printf("      2. Fill Request Queue with SA\n");
	printf("      3. SM + SM to Request Queue\n");
	printf("      4. Fill Request Queue with SM\n");
	printf("      5. SA + SM to Request Queue\n");
	printf("      6. Fill Request Queue with SA and SM\n");
	printf("      7. Add a SA to a full queue\n");
	printf("      8. Add a SM to a full queue\n");
}

int parseAddr(char *c_btaddr, U8 *btaddr) {
	char *token;
	int i;

	if (c_btaddr == NULL || btaddr == NULL) {
		printf("Error: c_btaddr or btaddr is null.\n");
		return -1;
	}

	if (strlen(c_btaddr) != 17) {
		printf("Error: invalid address length");
		return -1;
	}

	for (i = 0; i < 6; i++) {
		if (i == 0) {
			token = strtok(c_btaddr, ":");
		} else {
			token = strtok(NULL, ":");
		}

		if (token == NULL) {
			printf("Error: invalid address format, i=%d", i);
			return -1;
		}
		
		btaddr[i] = strtoul(token, &token, 16);
		printf("Parsing addr[%d]: 0x%x\n", i, btaddr[i]);
	}
	return 0;
}

int openSocket() {
	int fd = -1;
	struct sockaddr_un addr;
	socklen_t   addrlen;
	struct sockaddr_un btname;
	socklen_t   btnamelen;
  
	/* Set bt server address */
	btname.sun_family = AF_UNIX;
	strcpy (btname.sun_path, BT_SOCK_NAME_INT_ADP);
	btnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (btname.sun_path) + 1);

	fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
	if(fd < 0) {
		printf("create socket failed\n");
		return -1;
	}

	if(connect(fd, (const struct sockaddr*)&btname, btnamelen) < 0) {
		printf("Error: connect to /data/btserv failed : %s, errno=%d\n", strerror(errno), errno);
		close(fd);
		return -1;
	}
	return fd;
}

int sendMessage(int sockfd, msg_type msg_id, void *ptr, kal_uint16 size) {
	int ret = -1;
	ilm_struct *ilm;
	U16 *field;

	/* Set src_mod to MOD_MMI */	
	ilm = (ilm_struct*) ptr;
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

	if (sockfd) {
		ret = send(sockfd, ptr, size, 0);
		if (ret < 0) {
		    printf("Error: send msg failed. %s, %d\r", strerror(errno), errno);
		} else {
		    printf("send msg success : %d\r", ret);
		}
	} else {
	    printf("Error: socket uninitialized\r");
	}
	return ret;
}

int sendCommand(int sockfd, U8 *addr, U32 cmd_code) {
	ilm_struct ilm;

	printf("Send command to %2x:%2x:%2x:%2x:%2x:%2x, cmd: %d\n",
		addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], cmd_code);

	bt_sdap_tester_cmd_struct *cmd = (bt_sdap_tester_cmd_struct *) ilm.ilm_data;
	cmd->cmd = cmd_code;
	cmd->addr[0] = addr[0];
	cmd->addr[1] = addr[1];
	cmd->addr[2] = addr[2];
	cmd->addr[3] = addr[3];
	cmd->addr[4] = addr[4];
	cmd->addr[5] = addr[5];

	return sendMessage(sockfd, MSG_ID_BT_SDAP_TESTER_CMD, (void *) &ilm, sizeof(bt_sdap_tester_cmd_struct));
}

int main(int argc, char** argv) {
	int fd = -1;
	int cmd = -1;
	U8 btaddr[6];

	if (argc < 3) {
		printUsage();
		return 0;
	}

	if (parseAddr(argv[1], btaddr) != 0) {
		return -1;
	}

	cmd = atol(argv[2]);
	fd = openSocket();
	if (fd < 0) {
		return -1;
	}

	sendCommand(fd, btaddr, cmd);

	return 0;
}

#endif
