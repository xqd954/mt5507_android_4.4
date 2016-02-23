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

#define LOG_TAG "BT SIMAP_RILADP"

#include "utils/Log.h"
#include "utils/misc.h"

#include <cutils/sockets.h>


extern "C"
{

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_simap_struct.h"
#include "bt_simap_api.h"
#include "bt_simap_mmi_event.h"
#include "bt_simap_porting.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

}

#include "BtSimapRILAdapter.h"


#include "ril_pupack.h"
#include "ril.h"


#define SIM_HEXSTR_BUF_LEN  528


struct bt_simap_ril_atcmd_struct{
	const char *atcmd;
}bt_simap_ril_atcmd_array[SIMAP_RIL_REQ_LAST] = {
	{"AT+EBTSAP=0"},    /* connect */
	{"AT+EBTSAP=1"},	  /* disconnect */
	{"AT+EBTSAP=2,0"},  /* power on */
	{"AT+EBTSAP=3"},    /* power off */
	{"AT+EBTSAP=4,0"},  /* reset */
	{"AT+EBTSAP=5,0,"}   /* transfport APDU */
};

#define BTSAP_RILRSP_PREFIX "+EBTSAP:"

int g_ril_sock_fd = -1;

bool g_processing_ril_cmd = false;

static int bt_simap_send_cmd_to_ril(char * cmd_str, const char *prefix);
static bt_simap_sim_rsp_code_enum bt_simap_recv_rsp_from_ril(char *buffer, int buf_len);


int bt_simap_init_ril_sock(int sim_index)
{
		ALOGI("[BTSIMAP RILADP] bt_simap_init_ril_sock ...sim_index=%d", sim_index);

	if (g_ril_sock_fd >= 0)
	{
		ALOGI("[BTSIMAP RILADP] g_ril_sock_fd already open, just return");
		return g_ril_sock_fd;
	}
	
#ifdef MTK_DUAL_SIM_SUPPORT
	ALOGI("[BTSIMAP RILADP] === MTK_DUAL_SIM_SUPPORT === is DEFINED!");

	if (sim_index == 1)
#endif		
	{
		g_ril_sock_fd = socket_local_client(SOCKET_NAME_MTK_RILD,
						 ANDROID_SOCKET_NAMESPACE_RESERVED,
						 SOCK_STREAM);
	}
#ifdef MTK_DUAL_SIM_SUPPORT
	
	else
	{
		g_ril_sock_fd = socket_local_client(SOCKET_NAME_MTK_RILD_2,
						 ANDROID_SOCKET_NAMESPACE_RESERVED,
						 SOCK_STREAM);		
	}
	
#endif		
		
	if (g_ril_sock_fd < 0)
		{
		ALOGE("socket_local_client() failed: errno=%d, strerr=%s",  errno, strerror(errno));

		ALOGE("[BTSIMAP RILADP]ERROR: bt_simap_init_ril_sock: g_ril_sock_fd=%d", g_ril_sock_fd);
		
		}
	else	
		ALOGI("[BTSIMAP RILADP]bt_simap_init_ril_sock: g_ril_sock_fd=%d", g_ril_sock_fd);
	
	ALOGI("[BTSIMAP RILADP]bt_simap_init_ril_sock: set g_processing_ril_cmd = false");
	g_processing_ril_cmd = false;

	return g_ril_sock_fd;
}

void bt_simap_deinit_ril_sock()
{
	ALOGI("[BTSIMAP RILADP]bt_simap_deinit_ril_sock:");

	if (g_ril_sock_fd >= 0)
	{
	    close(g_ril_sock_fd);
		g_ril_sock_fd = -1;
	}
}


static bt_simap_sim_rsp_code_enum bt_simap_process_ril_cmd(char * cmd_str, const char *prefix, char *buffer, int buf_len)
{
	int send_rst;
	bt_simap_sim_rsp_code_enum proc_rst = SIMAP_RSP_ERR_OTHER;
	
	ALOGI("[BTSIMAP RILADP]bt_simap_process_ril_cmd [+]");

	//set the flag
	if (g_processing_ril_cmd)
	{
		ALOGE("[BTSIMAP RILADP]bt_simap_process_ril_cmd: g_processing_ril_cmd == TRUE, BUSY!");
		return SIMAP_RSP_ERR_BUSY;
	}
	
	g_processing_ril_cmd = true;

	send_rst = bt_simap_send_cmd_to_ril(cmd_str, prefix);
	if (send_rst < 0)
	{
		ALOGE("[BTSIMAP RILADP]bt_simap_process_ril_cmd: bt_simap_send_cmd_to_ril return error");
		goto PROC_EXIT;
	}

	proc_rst = bt_simap_recv_rsp_from_ril(buffer, buf_len);
	
PROC_EXIT:
	//clear the flag
	g_processing_ril_cmd = false;
	ALOGI("[BTSIMAP RILADP]bt_simap_process_ril_cmd [-]");
	return proc_rst;
}

static void printRawData(const void *data, size_t len)
{
    unsigned int i;
    char *pBuf = (char *) data;
    for (i=0;i <= (len - 1)/16; i++)
    {
        ALOGI("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                pBuf[16*i],pBuf[16*i+1],pBuf[16*i+2],pBuf[16*i+3],pBuf[16*i+4],pBuf[16*i+5],pBuf[16*i+6],pBuf[16*i+7],
                pBuf[16*i+8],pBuf[16*i+9],pBuf[16*i+10],pBuf[16*i+11],pBuf[16*i+12],pBuf[16*i+13],pBuf[16*i+14],pBuf[16*i+15]);
    }
}

static int bt_simap_send_cmd_to_ril(char * cmd_str, const char *prefix)
{
	char *ril_cmd = NULL;
	int ret;
	size_t len;
	size_t cur;
	unsigned char data_len[4];
	int written;

	ALOGI("bt_simap_send_cmd_to_ril call packRILCommand with cmd_str=%s", cmd_str);

	if (g_ril_sock_fd < 0)
	{
		ALOGE("[SAP_RIL]ERROR: g_ril_sock_fd < 0");		
		ret = -1;
		goto EXIT_SEND;
	}

	len = android::packRILCommand(cmd_str, (char*)prefix, &ril_cmd);
	if (ril_cmd == NULL)
	{
		ALOGE("[SAP_RIL]ERROR: packRILCommand return: cmd_len = %d, ril_cmd=NULL", len);
		ret = -1;
		goto EXIT_SEND;
	}

	//ALOGI("[SAP_RIL]packRILCommand return: cmd_len = %d, ril_cmd=%s", len, ril_cmd);
	
	//parcel length in big endian
	data_len[0] = 0;
	data_len[1] = 0;
	data_len[2] = (len >> 8) & 0xff;
	data_len[3] = len & 0xff;
	
	// send data length
	do {
		written = send(g_ril_sock_fd, data_len, sizeof(data_len), 0);
		ALOGI("[SAP_RIL]send ril_sock data_len return: %d", written);
	} while ((written < 0 && errno == EINTR) || (written == 0));

	if (written < 0) {
		ALOGE("[SAP_RIL]ERROR: send data_len failed: %d", written);
		ret = -1;
		goto EXIT_SEND;
	}

	// send raw data
	cur = 0;
	while (cur < len) {
		do {
			written = send(g_ril_sock_fd, ril_cmd + cur, len - cur, 0);
			ALOGI("[SAP_RIL]send ril_sock raw data return: %d", written);
		} while (written < 0 && errno == EINTR);

		if (written < 0) {
			ALOGE("[SAP_RIL]ERROR: send data failed: %d", written);
			ret = -1;
			goto EXIT_SEND;
		}

		cur += written;
	}
	ret = cur;
	
EXIT_SEND:
	
	if (ril_cmd)
		free(ril_cmd);
	
	return ret;
}

static bt_simap_sim_rsp_code_enum bt_simap_recv_rsp_from_ril(char *buffer, int buf_len)
{
	int count_read;
	int offset;
	int remaining;
	int msg_len;
	char *atRsp = NULL;
	char *recv_buf = NULL;
	int rsp_len = 0;
	int ril_errno;
	bt_simap_sim_rsp_code_enum rsp_code; 

	//ALOGI("bt_simap_recv_rsp_from_ril call recv...");
	
	// First, read in the length of the message
	offset = 0;
	remaining = 4;
	do {
		count_read = recv(g_ril_sock_fd, buffer, remaining, 0);
		ALOGI("[SAP_RIL]recv ril_sock data_len return: %d", count_read);
		
		if (count_read < 0) {
			if (errno == EINTR)
				count_read = 0;
			else
				return SIMAP_RSP_ERR_OTHER;
		}
		offset += count_read;
		remaining -= count_read;			   
	} while (remaining > 0);
	
	msg_len = ((buffer[0] & 0xff) << 24)
				| ((buffer[1] & 0xff) << 16)
				| ((buffer[2] & 0xff) << 8)
				| (buffer[3] & 0xff);

	ALOGI("[SAP_RIL]recv msg_len  msg_len=%d", msg_len);

	recv_buf = (char*)malloc(msg_len);
	memset(recv_buf, 0, msg_len);

	
	// Then, read in the message itself
	offset = 0;
	remaining = msg_len;
	do {
		count_read = recv(g_ril_sock_fd, recv_buf + offset, remaining, 0);
		ALOGI("[SAP_RIL]recv ril_sock data return: %d", count_read);

		if (count_read < 0) {
			if (errno == EINTR)
				count_read = 0;
			else {
				msg_len = -1;
				rsp_code = SIMAP_RSP_ERR_OTHER;
				goto RET;
			}
		}
		offset += count_read;
		remaining -= count_read;
	} while (remaining > 0);  
	
	ALOGI("[SAP_RIL]unpackRILResponseWithResult msg_len=%d", msg_len);
	atRsp = android::unpackRILResponseWithResult(recv_buf, msg_len, &ril_errno);
	//assert(msg_len <= buf_len);
	ALOGI("[SAP_RIL]unpackRILResponseWithResult: ril_errno=%d", ril_errno);

	if (ril_errno != RIL_E_SUCCESS)
	{
		if (ril_errno == RIL_E_BT_SAP_NOT_ACCESSIBLE)
			rsp_code = SIMAP_RSP_ERR_CARD_NOT_ACCESSIBLE;
		else if (ril_errno == RIL_E_BT_SAP_CARD_REMOVED)
			rsp_code = SIMAP_RSP_ERR_CARD_REMOVED;
		else	
			rsp_code = SIMAP_RSP_ERR_OTHER;
			
			goto RET;
	}
	
	rsp_code = SIMAP_RSP_SUCCESS;
	
	if (atRsp != NULL)
	{
		rsp_len = strlen(atRsp);
		ALOGI("[SAP_RIL]unpackRILResponse return: rsplen=%d, buf_len=%d, atRsp=\'%s\'", rsp_len, buf_len, atRsp);

		//printRawData((const char *)atRsp, strlen(atRsp));
		if (rsp_len >= buf_len)
		{
			ALOGE("[SAP_RIL]ERROR: recv msg_len overflow: msg_len=%d, buf_len=%d", msg_len, buf_len);
			free(atRsp);
			msg_len =  -1;
			goto RET;
		}
		
		strcpy(buffer, atRsp);
		
		//ALOGI("free atRsp: %x",atRsp);
		free(atRsp);
	}
	else
	{
		ALOGI("[SAP_RIL]ERROR: unpackRILResponse return NULL!");
		
		strcpy(buffer, "NULL");
	}

RET:
	if (recv_buf != NULL)
		free(recv_buf);
	
	ALOGI("bt_simap_recv_rsp_from_ril return: %d", rsp_code);
	return rsp_code;
}


static int get_out_rildata(char *in_buf, char* out_data)
{
	int len = 0;
	char *pstart, *pend;

	pstart = strchr(in_buf, '\"');
	if (pstart != NULL)
	{
		/* ATR data */
		pstart++;
		pend = strchr(pstart, '\"');
		if (pend != NULL)
		{
			len = pend-pstart;
			strncpy(out_data, pstart, len);
			out_data[len] = 0;

			//ALOGI("get_out_rildata: out_data=%s",out_data);
		}
	}
	return len;
}

static char hex_char_to_digit(char ch)
{
	char dgt = 0;
	
	if (ch >= '0' && ch <= '9')
		dgt = ch - '0';
	else if (ch >= 'A' && ch <= 'Z')
		dgt = (ch - 'A') + 10;
	else if (ch >= 'a' && ch <= 'z')
		dgt = (ch - 'a') + 10;

	return dgt;
}

static int convert_str_to_bytearry(char *src, unsigned char *buf)
{
	int src_len = strlen(src);
	int len = 0;

	for (int i = 0; i < src_len; i+=2)
	{
		buf[len] = hex_char_to_digit(src[i]) * 16 + hex_char_to_digit(src[i+1]);
		len++;
	}
	//ALOGI("convert_str_to_bytearry, src=%s", src);
	//ALOGI("convert_str_to_bytearry, byte array len=%d, data:", len);
	//printRawData((const char *) buf, len);

	return len;
}

static bt_simap_sim_rsp_code_enum bt_simap_parse_ril_connect_rsp(char* rsp_buf, int* curTPtype, int* suppTPtype, char * atr_buf, int *atr_len)
{
	bt_simap_sim_rsp_code_enum rspcode = SIMAP_RSP_ERR_OTHER;
	char atr_str[SIM_HEXSTR_BUF_LEN];

	ALOGI("bt_simap_parse_ril_connect_rsp: %s ", rsp_buf);
	
	{
		/* command success, get the rsp data from RIL  */
		char *ptmp1, *ptmp2;
		int ret; 

		ptmp1 = strchr(rsp_buf, ':');
		if (ptmp1 == NULL)
		{
			ALOGI("No : in the rsp_buf");
			return rspcode;
		}
		ptmp1++;
		*curTPtype = atoi(ptmp1);

		ptmp2 = strchr(ptmp1, ',');		
		if (ptmp2 == NULL)
		{
			ALOGI("No , in the rsp_buf");
			return rspcode;
		}
		ptmp2++;
		*suppTPtype = atoi(ptmp2);
			
		ret = get_out_rildata(rsp_buf, atr_str);

		*atr_len = convert_str_to_bytearry(atr_str, (unsigned char*)atr_buf);

		rspcode = SIMAP_RSP_SUCCESS;

		ALOGI("curTPtype=%d, suppTPtype=%d, atr_len=%d, atr_buf=%s", *curTPtype, *suppTPtype, *atr_len, atr_buf);
	}

	return rspcode;
}

static bt_simap_sim_rsp_code_enum bt_simap_parse_ril_curtype_atr_rsp(char* rsp_buf, int* curTPtype, char * atr_buf, int *atr_len)
{
	bt_simap_sim_rsp_code_enum rspcode = SIMAP_RSP_ERR_OTHER;
	char atr_str[SIM_HEXSTR_BUF_LEN];

	ALOGI("bt_simap_parse_ril_curtype_atr_rsp: %s ", rsp_buf);
	
	{
		/* command success, get the rsp data from RIL  */
		char *ptmp1;
		int ret; 

		ptmp1 = strchr(rsp_buf, ':');
		if (ptmp1 == NULL)
		{
			ALOGI("No : in the rsp_buf");
			return rspcode;
		}
		ptmp1++;
		*curTPtype = atoi(ptmp1);
			
		ret = get_out_rildata(rsp_buf, atr_str);

		*atr_len = convert_str_to_bytearry(atr_str, (unsigned char*)atr_buf);

		rspcode = SIMAP_RSP_SUCCESS;

		ALOGI("curTPtype=%d, atr_len=%d, atr_buf=%s", *curTPtype, *atr_len, atr_buf);
	}

	return rspcode;
}

static bt_simap_sim_rsp_code_enum bt_simap_parse_ril_apdu_rsp(char* rsp_buf, char * apdu_buf, int *apdu_len)
{
	bt_simap_sim_rsp_code_enum rspcode = SIMAP_RSP_ERR_OTHER;
	char apdu_str[SIM_HEXSTR_BUF_LEN];

	ALOGI("bt_simap_parse_ril_apdu_rsp: %s ", rsp_buf);
	
	{
		/* command success, get the rsp data from RIL  */
		int ret; 
			
		ret = get_out_rildata(rsp_buf, apdu_str);

		*apdu_len = convert_str_to_bytearry(apdu_str, (unsigned char*)apdu_buf);

		rspcode = SIMAP_RSP_SUCCESS;

		//ALOGI("apdu response len=%d", *apdu_len);
		//ALOGI("apdu response raw data:");
		//printRawData((const char *) apdu_buf, *apdu_len);
	}

	return rspcode;
}

int bt_simap_process_connect_sim_request(int* rsp_curTPtype, int* rsp_suppTPtype, int* rsp_atr_len, char* rst_atr)
{
	ALOGI("bt_simap_process_connect_sim_request, g_ril_sock_fd=%d", g_ril_sock_fd);

	char * cmd_str = (char*)bt_simap_ril_atcmd_array[SIMAP_CONNECT_SIM_REQ].atcmd;
	int buf_len = SIM_HEXSTR_BUF_LEN;
	char rsp_buf[buf_len];
	bt_simap_sim_rsp_code_enum rsp_code = SIMAP_RSP_ERR_OTHER;

	rsp_code = bt_simap_process_ril_cmd(cmd_str, BTSAP_RILRSP_PREFIX, rsp_buf, buf_len);

	if (rsp_code == SIMAP_RSP_SUCCESS)
	{
		rsp_code = bt_simap_parse_ril_connect_rsp(rsp_buf, rsp_curTPtype, rsp_suppTPtype, rst_atr, rsp_atr_len);
	}

	if (rsp_code == SIMAP_RSP_ERR_BUSY)
	{
		*rsp_atr_len = 0;
		rsp_code = SIMAP_RSP_ERR_OTHER;
	}
	
	return rsp_code;

}


int bt_simap_process_poweron_sim_request(int* rsp_curTPtype, int* rsp_atr_len, char* rst_atr)
{
	ALOGI("bt_simap_process_poweron_sim_request, g_ril_sock_fd=%d", g_ril_sock_fd);

	char * cmd_str = (char*)bt_simap_ril_atcmd_array[SIMAP_POWER_SIM_ON_REQ].atcmd;
	int buf_len = SIM_HEXSTR_BUF_LEN;
	char rsp_buf[buf_len];
	bt_simap_sim_rsp_code_enum rsp_code = SIMAP_RSP_ERR_OTHER;

	rsp_code = bt_simap_process_ril_cmd(cmd_str, BTSAP_RILRSP_PREFIX, rsp_buf, buf_len);

	if (rsp_code == SIMAP_RSP_SUCCESS)
	{
		rsp_code = bt_simap_parse_ril_curtype_atr_rsp(rsp_buf, rsp_curTPtype, rst_atr, rsp_atr_len);
	}

	if (rsp_code == SIMAP_RSP_ERR_BUSY)
	{
		*rsp_atr_len = 0;
		rsp_code = SIMAP_RSP_ERR_OTHER;
	}

	return rsp_code;

}

int bt_simap_process_reset_sim_request(int* rsp_curTPtype, int* rsp_atr_len, char* rst_atr)
{
	ALOGI("bt_simap_process_reset_sim_request, g_ril_sock_fd=%d", g_ril_sock_fd);

	char * cmd_str = (char*)bt_simap_ril_atcmd_array[SIMAP_RESET_SIM_REQ].atcmd;
	int buf_len = SIM_HEXSTR_BUF_LEN;
	char rsp_buf[buf_len];
	int ret = -1;
	bt_simap_sim_rsp_code_enum rsp_code = SIMAP_RSP_ERR_OTHER;

	rsp_code = bt_simap_process_ril_cmd(cmd_str, BTSAP_RILRSP_PREFIX, rsp_buf, buf_len);

	if (rsp_code == SIMAP_RSP_SUCCESS)
	{
		rsp_code = bt_simap_parse_ril_curtype_atr_rsp(rsp_buf, rsp_curTPtype, rst_atr, rsp_atr_len);
	}

	if (rsp_code == SIMAP_RSP_ERR_BUSY)
	{
		*rsp_atr_len = 0;
		rsp_code = SIMAP_RSP_ERR_OTHER;
	}

	return rsp_code;
}

int bt_simap_process_apdu_request(char *req_data, int req_data_len,
												char *out_data, int* out_data_len)
{
	bt_simap_sim_rsp_code_enum rsp_code = SIMAP_RSP_ERR_OTHER;

	ALOGI("bt_simap_process_apdu_request, g_ril_sock_fd=%d", 
		g_ril_sock_fd);
	
    if (g_ril_sock_fd > 0)
    {
		char req_str[SIM_HEXSTR_BUF_LEN];
		char apdu[SIM_HEXSTR_BUF_LEN];
		int len;
		int buf_len = SIM_HEXSTR_BUF_LEN;
		char rsp_buf[buf_len];

		//ALOGI("apdu request data_len=%d", req_data_len);
		//ALOGI("apdu request raw data:");
		//printRawData((const char *) req_data, req_data_len);


		strcpy(req_str, bt_simap_ril_atcmd_array[SIMAP_TRANSFER_APDU_REQ].atcmd);

		apdu[0] = '\"';
		len = 1;

		for (int i = 0; i < req_data_len; i++)
		{
			len += sprintf(apdu+len, "%02X", req_data[i]);
		}
		apdu[len] = '\"';
		apdu[len+1] = '\0';

		//ALOGI("apdu=%s, len=%d", (char*)apdu, len);

		strcat(req_str, apdu);

		ALOGI("apdu req_str=%s", (char*)req_str);
		
		rsp_code = bt_simap_process_ril_cmd(req_str, BTSAP_RILRSP_PREFIX, rsp_buf, buf_len);

		if (rsp_code == SIMAP_RSP_SUCCESS)
		{
			bt_simap_parse_ril_apdu_rsp(rsp_buf, out_data, out_data_len);
		}
    }

	if (rsp_code == SIMAP_RSP_ERR_BUSY)
	{
		ALOGI("bt_simap_process_client_request: convert SIMAP_RSP_ERR_BUSY error to SIMAP_RSP_ERR_OTHER");
		*out_data_len = 0;
		rsp_code = SIMAP_RSP_ERR_OTHER;
	}
	
	return rsp_code;
}

int bt_simap_process_client_simple_request(int request)
{
	bt_simap_sim_rsp_code_enum rsp_code = SIMAP_RSP_ERR_OTHER;

	ALOGI("bt_simap_process_client_simple_request, req=%d, g_ril_sock_fd=%d", 
		request, 
		g_ril_sock_fd);
	
   if (request >= 0 && request < SIMAP_RIL_REQ_LAST)
   {
		if (request == SIMAP_TRANSFER_ATR_REQ)
		{
		}
		else if (request == SIMAP_TRANSFER_CARD_READER_STATUS_REQ)
		{
		}
		else if (request == SIMAP_SET_TRANSPORT_PROTOCOL_REQ)
		{
		}
		else if (g_ril_sock_fd >= 0)
		{
			/* DISCONNECT and POWER_OFF SIM request */
			char * cmd_str = (char*)bt_simap_ril_atcmd_array[request].atcmd;
			int buf_len = SIM_HEXSTR_BUF_LEN;
			char rsp_buf[buf_len];
			
			rsp_code = bt_simap_process_ril_cmd(cmd_str, NULL, rsp_buf, buf_len);
				
			/* DISCONNECT and POWER_OFF has no output string */
		}
   }

	if (rsp_code == SIMAP_RSP_ERR_BUSY)
	{
		ALOGI("bt_simap_process_client_request: convert SIMAP_RSP_ERR_BUSY error to SIMAP_RSP_ERR_OTHER");
		rsp_code = SIMAP_RSP_ERR_OTHER;
	}
	
	return rsp_code;
}



