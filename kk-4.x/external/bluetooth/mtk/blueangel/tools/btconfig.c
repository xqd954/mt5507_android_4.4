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
#include "bluetooth_gap_message.h"
#include "bluetooth_gap_struct.h"
#include "bt_adp_fs.h"
#include "me_adp.h"
#include "bt_cust_api.h"
//#include "bt_adp_debug.h"
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cutils/sockets.h"

typedef enum _BT_CONFIG_CMD
{
    BT_CMD_LOG_MASK = 1,
    BT_CMD_LOG_TO_FILE = 2,
    BT_CMD_GET_CONTEXT = 3,
    BT_CMD_MANUAL_DUMP = 4,
    BT_CMD_TEST_CMD_REQ = 5,
    BT_CMD_GET_LINKKEY = 6,
    BT_CMD_DISC_ALL_REQ = 7,
    BT_CMD_ALLOW_LINK_REQ = 8,
    BT_CMD_GET_LINK_KEY_REQ = 9,
    BT_CMD_VERIFY_CUST = 10,
    BT_CMD_MMI_TEST_REQ = 11,
    BT_CMD_AVRCP_TEST_REQ = 12,
    NUM_OF_BT_CMD,
}BT_CONFIG_CMD;


/* MSG_ID_BT_DBG_QUERY_CONTEXT_REQ */
typedef struct
{
    LOCAL_PARA_HDR
    U32 filter;
} bt_dbg_query_context_req_struct;

/* MSG_ID_BT_DBG_QUERY_CONTEXT_CNF */
typedef struct
{
    LOCAL_PARA_HDR
    U32 filter;
} bt_dbg_query_context_cnf_struct;

/* MSG_ID_BT_DBG_MANUAL_DUMP_REQ */
typedef struct{
    LOCAL_PARA_HDR
}bt_dbg_manual_dump_req_struct;

typedef struct _bt_ddb_data {
    I16                  numDdbRecs;
    //unsigned long                 numDdbRecs;
    BtDeviceRecord       devDb[40];
} bt_ddb_data;

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
    printf("sendMessage : msg id = %u\n", msg_id);
    if(sockfd)
    {
        ret = send(sockfd, ptr, size, 0);
        if(ret < 0)
        {
            printf("send msg fail : %s, %d\n", strerror(errno), errno);
        }
        else
        {
            printf("send msg success : %d\n", ret);
        }
    }
    else
    {
        printf("socket uninitialized\n");
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
      printf("[ERR] connect to %s failed : %s, errno=%d\n", BT_SOCK_NAME_INT_ADP, strerror(errno), errno);
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

int logToFile(int sockfd, char *name) {
	ilm_struct ilm;
	kal_uint16 size = sizeof(bt_engineer_mode_log_to_file_cnf_struct);
	
	bt_engineer_mode_log_to_file_req_struct *req_p = (bt_engineer_mode_log_to_file_req_struct*)ilm.ilm_data;
	printf("logToFile(name=%s) : msg_id=%d\n", name, MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_REQ);
	memcpy(req_p->name, name, strlen(name));
	if(sendMessage(sockfd, MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_REQ, (void*)&ilm, sizeof(bt_engineer_mode_log_to_file_req_struct)) < 0 || 
		 waitResponse(sockfd, MSG_ID_BT_ENGINEER_MODE_LOG_TO_FILE_CNF, &ilm, &size) < 0)
	{		 
		printf("logToFile failed\n");
		return -1;
	}
	else
	{
		bt_engineer_mode_log_to_file_cnf_struct *cnf_p = (bt_engineer_mode_log_to_file_cnf_struct*)ilm.ilm_data;
		if(cnf_p->result != 0)
		{
			printf("logToFile failed\n");
			return -1;
		}
	}
	printf("logToFile success\n");
	return 0;
}


int inspectContext(int sockfd, U32 filter) {
	ilm_struct ilm;
	kal_uint16 size = sizeof(bt_dbg_query_context_req_struct);
	
	bt_dbg_query_context_req_struct *req_p = (bt_dbg_query_context_req_struct*)ilm.ilm_data;
	printf("inspectContext(filter=0x%04lx) : msg_id=%d\n", filter, MSG_ID_BT_DBG_QUERY_CONTEXT_REQ);
       req_p->filter = filter;
	if(sendMessage(sockfd, MSG_ID_BT_DBG_QUERY_CONTEXT_REQ, (void*)&ilm, sizeof(bt_dbg_query_context_req_struct)) < 0 || 
		 waitResponse(sockfd, MSG_ID_BT_DBG_QUERY_CONTEXT_CNF, &ilm, &size) < 0)
	{		 
		printf("inspectContext failed\n");
		return -1;
	}
	else
	{
		bt_dbg_query_context_cnf_struct *cnf_p = (bt_dbg_query_context_cnf_struct*)ilm.ilm_data;
              printf("inspectContext return filter = x%04lx\n", cnf_p->filter);
	}
	return 0;
}


int BTTool_SendMessage(int sockfd, U16 msg_id, void *ptr, U16 size){
	  // send msg to socket id
	  ilm_struct ilmdata;
    ilm_struct *ilm;
    int ret = 0;

    ilm = (ilm_struct*) &ilmdata;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = (msg_type) msg_id;
    if( size > MAX_ILM_BUFFER_SIZE ){
    	size = MAX_ILM_BUFFER_SIZE;
    }
    memcpy(ilm->ilm_data, ptr, size);

    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    if(sockfd)
    {
        //ret = send(sockfd, ptr, size, 0);
        ret = sendMessage( sockfd, msg_id, ilm, size);
		}else{
				printf("err ! \n");
		}
		return ret;
}


/**
* @brief Send to the BT stack
*/
int sendBTTestCommand(int fd,  const char *test_str, int len ){
    bt_test_cmd_req_struct reqdata;
    bt_test_cmd_req_struct *req = &reqdata;	
	
    if( strlen(test_str) >= sizeof(req->test_cmd_content) ){
    	len = sizeof(req->test_cmd_content) - 1 ;
    }    
    
    memset( req->test_cmd_content, 0, sizeof(req->test_cmd_content));
    memcpy( req->test_cmd_content, test_str, len);
    req->test_cmd_content[sizeof(req->test_cmd_content)-1] = 0;
    	
    return 	BTTool_SendMessage(fd, MSG_ID_BT_TEST_CMD_REQ, req, sizeof(bt_test_cmd_req_struct));
}



int sendtoMessageByName(int sockfd, char *name, msg_type msg_id, void *ptr, kal_uint16 size)
{
    int ret = -1;
    ilm_struct *ilm;
    U16 *field;
    struct sockaddr_un btname;
    socklen_t   btnamelen;
	
    socket_make_sockaddr_un(name, ANDROID_SOCKET_NAMESPACE_ABSTRACT, (struct sockaddr_un*)&btname, &btnamelen); 
    
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
    printf("[TEST] sendMessage : msg id = %u sockfd:%d to (%s)\n", msg_id, sockfd, name);
    if(sockfd)
    {
        // bt_ext_apis.c # ret = sendto(g_ext_gap_context.appsock, ptr, size, MSG_DONTWAIT, (const struct sockaddr*)&name, namelen);
        btname.sun_family = AF_UNIX;
        printf("[TEST] before send \n");
        ret = sendto( sockfd, ptr, size, MSG_DONTWAIT, (const struct sockaddr*)&btname, btnamelen);
        if(ret < 0)
        {
            printf("sendto msg fail : %s, %d\n", strerror(errno), errno);

            printf( "fail param btname:%d offsetof:%d strlen:%d strlenoff:%d\n", btnamelen, (offsetof (struct sockaddr_un, sun_path)), strlen (btname.sun_path), strlen (&btname.sun_path[1]) );
            ret = sendto( sockfd, ptr, size, MSG_DONTWAIT, (const struct sockaddr*)&btname, 19);            
            if(ret < 0)
            {
                printf("sendto msg fail 2: %s, %d\n", strerror(errno), errno);
            }else{
                printf("sendto msg 2 success : %d\n", ret);
            }

        }
        else
        {
            printf("sendto msg success : %d\n", ret);
        }
        printf("[TEST] before after\n");        
    }
    else
    {
        printf("socket uninitialized\n");
    }
    return ret;
}

int openExtSocket()
{
	int fd = -1;
  
  fd = socket_local_server("btlogmask2", ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_DGRAM);
	if(fd < 0)
	{
  	  printf("create btlogmask2 socket failed: %s, errno=%d!\n", strerror(errno), errno);
  }
  return fd;
}


/**
* @brief Send Test-Req structure to any profile by msg_id
*/
int sendMMITestCommand(int fd, int msg_id, const char *test_str, int len ){
    bt_test_cmd_req_struct reqdata;
    void *ptr =0;
    U16 size;
    int sockfd = fd;
    ilm_struct ilmdata;
    ilm_struct *ilm;
    char *socket_name = "";
    int ret = 0;
    
    bt_test_cmd_req_struct *req = &reqdata;	
	
    if( strlen(test_str) >= sizeof(req->test_cmd_content) ){
    	len = sizeof(req->test_cmd_content) - 1 ;
    }    
    
    memset( req->test_cmd_content, 0, sizeof(req->test_cmd_content));
    memcpy( req->test_cmd_content, test_str, len);
    req->test_cmd_content[sizeof(req->test_cmd_content)-1] = 0;

    ptr = req;
    size = sizeof(bt_test_cmd_req_struct);

    ilm = (ilm_struct*) &ilmdata;
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_MMI;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = (msg_type) msg_id;
    if( size > MAX_ILM_BUFFER_SIZE ){
    	size = MAX_ILM_BUFFER_SIZE;
    }
    memcpy(ilm->ilm_data, ptr, size);

    size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;
    
    switch( msg_id ){
        case MSG_ID_BT_AVRCP_TEST_CMD_REQ: //avrcp jni
            socket_name = BT_SOCK_NAME_EXT_ADP_AVRCP;
            break;
        case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_REQ: // gap jni
            socket_name = BT_SOCK_NAME_EXT_ADP;
            break;
        default:
            printf("err ! not support yet !\n");
            return ret;
        break;
    }
    
    if(sockfd)
    {
        ret = sendtoMessageByName( sockfd, socket_name, msg_id, ilm, size);
    }else{
	    printf("err ! \n");
    }
		return ret;    
    	

}

void manualDump(int sockfd){
    ilm_struct ilm;    
    bt_dbg_manual_dump_req_struct *req_p = (bt_dbg_manual_dump_req_struct*)ilm.ilm_data;
    if( sendMessage(sockfd, MSG_ID_BT_DBG_MANUAL_DUMP_REQ, (void*)&ilm, sizeof(bt_dbg_manual_dump_req_struct)) < 0 ){
        printf("manualDump failed\n");
    }else{
        printf("manualDump success\n");
    }
}

void discAll(int sockfd){
    ilm_struct ilm;
    kal_uint16 size = 0;
    //bt_dbg_manual_dump_req_struct *req_p = (bt_dbg_manual_dump_req_struct*)ilm.ilm_data;
    if( sendMessage(sockfd, MSG_ID_BT_BM_LINK_DISALLOW_REQ, (void*)&ilm, 0) < 0 ||
        waitResponse(sockfd, MSG_ID_BT_BM_LINK_DISALLOW_CNF, &ilm, &size) < 0){
        printf("discAll failed\n");
    }else{
        printf("discAll success\n");
    }
}

void allowLink(int sockfd){
    ilm_struct ilm;
    kal_uint16 size = 0;
    //bt_dbg_manual_dump_req_struct *req_p = (bt_dbg_manual_dump_req_struct*)ilm.ilm_data;
    if( sendMessage(sockfd, MSG_ID_BT_BM_LINK_ALLOW_REQ, (void*)&ilm, 0) < 0 ||
        waitResponse(sockfd, MSG_ID_BT_BM_LINK_ALLOW_CNF, &ilm, &size) < 0){
        printf("allowLink failed\n");
    }else{
        printf("allowLink success\n");
    }
}

void getDevLinkkey(int sockfd, btbm_bd_addr_t *bd_addr){
    ilm_struct ilm;
    kal_uint16 size = sizeof(bt_bm_get_link_key_req_struct);
    bt_bm_get_link_key_req_struct *req_p = (bt_bm_get_link_key_req_struct*)ilm.ilm_data;
    req_p->bd_addr = *bd_addr;
    if( sendMessage(sockfd, MSG_ID_BT_BM_GET_LINK_KEY_REQ, (void*)&ilm, size) < 0 ||
        waitResponse(sockfd, MSG_ID_BT_BM_GET_LINK_KEY_CNF, &ilm, &size) < 0){
        printf("getDevLinkkey failed\n");
    }else{
        bt_bm_get_link_key_cnf_struct *cnf_p = (bt_bm_get_link_key_cnf_struct*)ilm.ilm_data;
        printf("getDevLinkkey success\n");
        printf("result=%u\n", cnf_p->result);
        printf("cod=0x%lX\n", cnf_p->cod);
        printf("link_key_type=%u\n", cnf_p->link_key_type);
        printf("persist=%u\n", cnf_p->persist);
    }
}

static int readFile(int fd, U8 *ptr, int size){
    int ret = -1;
    int ori_size = size;
    for(;size;size -= ret, ptr += ret){
        ret = read(fd, ptr, size);
        if(ret == 0 || (ret < 0 && errno != EINTR))
            break;
    }
    if(ret < 0){
        printf("Error read file(%d) : errno=%d, %s\n", fd, errno, strerror(errno));
        return ret;
    }else{
        return ori_size - size;
    }
}

int getLinkkey(const char *addr, int len ){
    bt_ddb_data ddb;
    int fd = open(BT_DB_FILE, O_RDONLY);

    if(fd < 0){
        printf("No records\n");
    }else{
        I16 count;
        //BtDeviceRecord record;
        bt_ddb_data ddb;
        BtDeviceRecord *record;
        
        int flags = fcntl(fd, F_GETFL);
        flags &= ~O_NONBLOCK;
        fcntl(fd, F_SETFL, flags);

        if( readFile(fd, (U8*)&ddb, sizeof(ddb)) <= 0){
            printf("Error read ddb\n");
            return -1;
        }
        count = ddb.numDdbRecs;
        record = &ddb.devDb[0];
        while(count > 0){
            /* print linkkey */
            printf("  %02X:%02X:%02X:%02X:%02X:%02X\n", 
                record->bdAddr.addr[0], 
                record->bdAddr.addr[1], 
                record->bdAddr.addr[2], 
                record->bdAddr.addr[3],
                record->bdAddr.addr[4],
                record->bdAddr.addr[5]);
            printf("    key : %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
                record->linkKey[15],
                record->linkKey[14],
                record->linkKey[13],
                record->linkKey[12],
                record->linkKey[11],
                record->linkKey[10],
                record->linkKey[9],
                record->linkKey[8],
                record->linkKey[7],
                record->linkKey[6],
                record->linkKey[5],
                record->linkKey[4],
                record->linkKey[3],
                record->linkKey[2],
                record->linkKey[1],
                record->linkKey[0]);
            count--;
            record++;
        }
    }
#if 0
        if( readFile(fd, (U8*)&count, sizeof(I16)) <= 0){
            printf("Error read count\n");
            return -1;
        }
        printf("num of linkkey : %d\n", count);
        while(count > 0){
            if( readFile(fd, (U8*)&record, sizeof(BtDeviceRecord)) <= 0){
                printf("Error read record\n");
                return -1;
            }
            /* print linkkey */
            printf("\t%02X:%02X:%02X:%02X:%02X:%02X\n", 
                record.bdAddr.addr[0], 
                record.bdAddr.addr[1], 
                record.bdAddr.addr[2], 
                record.bdAddr.addr[3],
                record.bdAddr.addr[4],
                record.bdAddr.addr[5]);
            printf("\t\tkey : %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
                record.linkKey[0],
                record.linkKey[1],
                record.linkKey[2],
                record.linkKey[3],
                record.linkKey[4],
                record.linkKey[5],
                record.linkKey[6],
                record.linkKey[7],
                record.linkKey[8],
                record.linkKey[9],
                record.linkKey[10],
                record.linkKey[11],
                record.linkKey[12],
                record.linkKey[13],
                record.linkKey[14],
                record.linkKey[15]);
        }
        count--;
    }
#endif
    if(fd >= 0) close(fd);
    return 1;
}

/*************************************
*   Typedef
**************************************/
typedef struct _bt_config_item{
    const char *name; /* config name */
    const void *value; /* shall be int or pointer to const string */
}bt_config_item;

typedef struct _bt_dev_config_item{
    const char *name; /* config name */
    const void *value; /* shall be int or pointer to const string */
    unsigned int cid;
}bt_dev_config_item;

typedef struct _bt_dev_config{
    const char *name; /* device name */
    bt_dev_config_item *config_table;
}bt_dev_config;

void test_log(const char *log){
    printf("%s\n", log);
}

extern unsigned int getCustTable(bt_config_item **table);
extern unsigned int getDevCustTable(bt_dev_config **config);

static void CONVERT_ARRAY2BDADDR(bt_addr_struct *dest, U8 *src)
{
    dest->nap = (((U16) src[4])) | ((U16) src[5] << 8);
    dest->uap = (U8) src[3];
    dest->lap = (((U32) src[2]) << 16) | ((U32) src[1] << 8) | ((U32) src[0]);
}

int main (int argc, char **argv)
{
	int fd = -1;
	int fdext = -1;
	char test_str[256];
	int i;
        BT_CONFIG_CMD cmd = -1;
        
    signal(SIGPIPE, SIG_IGN);

       if(argc < 2) {
        printf("No command is specified\n");
        return -1;
       }else {
        cmd = (BT_CONFIG_CMD)atol(argv[1]);
       }

	fd = openSocket();
	if(fd < 0 && cmd != 11){
		return -1;
       }
    printf("fd %d\n", fd);
       fdext = openExtSocket();
    printf("fdext %d\n", fdext);
    
        switch(cmd)
        {
        case BT_CMD_LOG_MASK:
        {
            kal_uint32 trace = 0;
            kal_uint8 primitive = 0;
            kal_uint8 prompt = 0;
            if(argc < 5)
            {
                printf("Usage : logmask trace primitive prompt\n");
                printf("trace : bit mask of trace group\n");
                printf("primitive : set other than 0 to mask primitive log\n");
                printf("prompt : set other than 0 to mask prompt log\n");
                return 0;
            }
            else
            {
                trace = strtoul(argv[2], NULL, 16);
                primitive = atol(argv[3]);
                prompt = atol(argv[4]);
                setLogMask(fd, trace, primitive, prompt);
            }
        }
            break;
        case BT_CMD_LOG_TO_FILE:
        {
            if(argc < 3 && strlen(argv[2]) != 0){
                printf("Usage : btconfig 2 <logfile>\n");
                printf("logfile : name of the file to store logs\n");
                return 0;
            }else {
                logToFile(fd, argv[2]);
            }            
        }
            break;
        case BT_CMD_GET_CONTEXT:
        {
            kal_uint32 filter = 0;
            if(argc < 3)
            {
                printf("Usage : btconfig 3 filter\n");
                printf("filter : bit mask of contexts to get\n");
                return 0;
            }
            else
            {
                filter = strtoul(argv[2], NULL, 16);
                inspectContext(fd, filter);
            }
        }
            break;
        case BT_CMD_MANUAL_DUMP:
            manualDump(fd);
            break;
        case BT_CMD_TEST_CMD_REQ:
        	  /* */
        	  {
        	  	printf("test cmd req");
        	  	strcpy( test_str, argv[2] );
					    printf("argv[1]=%s\n", argv[2]);
					    for (i=2; i<(argc-1); i++)
					    {
					        strcat(test_str, " ");
					        strcat(test_str, argv[i+1]);
					        printf("argv[%d]=%s\n", (i+1), argv[i+1]);
					    }
					    printf("test_str=%s\n", test_str);
					    printf("test_str len=%d\n", strlen(test_str));
					    sendBTTestCommand( fd, test_str, strlen(test_str) );
        	  }
            break;
        case BT_CMD_GET_LINKKEY:
            /* Get linkkey */
            getLinkkey(NULL, 0);
            break;
        case BT_CMD_DISC_ALL_REQ:
            discAll(fd);
            break;
        case BT_CMD_ALLOW_LINK_REQ:
            allowLink(fd);
            break;
        case BT_CMD_GET_LINK_KEY_REQ:
        {
            btbm_bd_addr_t bd_addr;
            char *addr = argv[2];
            BD_ADDR addrstr;
            U8 *paddr = addrstr.addr;
            addr = strtok(addr, ":");
            while(addr != NULL){
                *paddr = strtoul(addr, NULL, 16);
                addr = strtok(NULL, ":");
                paddr++;
            }
            CONVERT_ARRAY2BDADDR(&bd_addr, addrstr.addr);
            getDevLinkkey(fd, &bd_addr);
        }
            break;
        case BT_CMD_VERIFY_CUST:
        {
            bt_config_item *cust_table;
            bt_dev_config *dev_config;
            bt_dev_config_item *dev;
            unsigned int cust_table_size, dev_config_size;
            unsigned int i,j;
            unsigned int value;
            unsigned int cid = 0, did = 0;
            int ret = 0;
            
            cust_table_size = getCustTable(&cust_table);
            dev_config_size = getDevCustTable(&dev_config);
            if( initCust(test_log) >= 0 ){
                printf("Start verifying cust table\n");
                for(i = 0;i < cust_table_size;i++){
                    cid = getCustID(cust_table[i].name);
                    if(cid != (i+1)){
                        printf("[ERROR] retrieve cid(%u) of %s failed : return %u\n", (i+1), cust_table[i].name, cid);
                        ret++;
                    }else{
                        if( getCustValue(cid, &value) == 0){
                            printf("[ERROR] getCustValue(%u) failed\n", cid);
                            ret++;
                        }else if(value != (unsigned int)cust_table[i].value){
                            printf("[ERROR] value 0x%X returned by getCustValue(%u) incorrect. expected : %p\n", value, cid, cust_table[i].value);
                            ret++;
                        }
                    }
                }
                printf("Error testing\n");
                cid = getCustID("Test error handling");
                if(cid != BT_CUST_UNKNOWN){
                    printf("[ERROR] Error testing with invalid name failed : return cid %u\n", cid);
                }
                if( getCustValue(BT_CUST_UNKNOWN, &value) != 0){
                    printf("[ERROR] Error testing getCustValue(BT_CUST_UNKNOWN, &value) failed\n");
                }
                printf("End verifying cust table\n");
                printf("Start verifying device table\n");
                for(i = 0;i < dev_config_size;i++){
                    printf("Start verifying device %s\n", dev_config[i].name);
                    did = getDeviceID(dev_config[i].name);
                    if(did != (i+1)){
                        printf("[ERROR] retrieve did(%u) of %s failed : return %u\n", (i+1), dev_config[i].name, did);
                        ret++;
                    }else{
                        dev = dev_config[i].config_table;
                        while(dev && dev->name){
                            if( getDevCustValue(did, dev->cid, &value) == 0){
                                printf("[ERROR] getDevCustValue(%u, %u) failed\n", did, dev->cid);
                                ret++;
                            }else if(value != (unsigned int)dev->value){
                                printf("[ERROR] value 0x%X returned by getDevCustValue(%u, %u) incorrect. expected : %p\n", value, did, cid, dev->value);
                                ret++;
                            }
                            dev++;
                        }
                        /* Verifying if cus table value is returned if dev does not contain this 
                        config which is specified by cid */
                        for(j = 0;j < cust_table_size;j++){
                            cid = j+1;
                            dev = dev_config[i].config_table;
                            while(dev && dev->name){
                                if(strcmp(dev->name, cust_table[j].name) == 0){
                                    break;
                                }
                                dev++;
                            }
                            if(dev && dev->name == NULL){
                                printf("Verify getDevCustValue return value in cust table if dev did not contain name=%s, cid(%u), value=%p\n", cust_table[j].name, cid, cust_table[j].value);
                                if( getDevCustValue(did, cid, &value) == 0){
                                    printf("[ERROR] getDevCustValue(%u, %u) failed\n", did, cid);
                                    ret++;
                                }else if(value != (unsigned int)cust_table[j].value){
                                    printf("[ERROR] value 0x%X returned by getDevCustValue(%u, %u) did not equal to the value in cust_table. expected : %p\n", value, did, cid, cust_table[j].value);
                                    ret++;
                                }
                                break;
                            }
                        }
                    }
                }
                printf("Error testing\n");
                if( getDeviceID("Test error handling") != BT_DEVICE_UNKNOWN){
                    printf("[ERROR] Error testing with invalid name failed : return cid %u\n", did);
                    ret++;
                }
                if( getDevCustValue(BT_DEVICE_UNKNOWN, BT_DEVICE_UNKNOWN, &value) != 0){
                    printf("[ERROR] Error testing getCustValue(BT_DEVICE_UNKNOWN, BT_CUST_UNKNOWN, &value) failed\n");
                    ret++;
                }
                if( getDevCustValue(BT_DEVICE_UNKNOWN, 1, &value) == 0){
                    printf("[ERROR] Error testing getCustValue(BT_DEVICE_UNKNOWN, 1, &value) failed\n");
                    ret++;
                }else if(value != (unsigned int)cust_table[0].value){
                    printf("[ERROR] Error testing getCustValue(BT_DEVICE_UNKNOWN, 1, &value) failed. value=%u, correct=%p\n", value, cust_table[0].value);
                    ret++;
                }
            }else{
                printf("initCust failed\n");
            }
            printf("Test completed : error case = %d\n", ret);
            printf("Show config table +\n");
            dumpCust();
            printf("Show config table -\n");
        }
            break;
       case BT_CMD_MMI_TEST_REQ:
            if( fdext < 0){
                //printf("send no fdext error!");
                //return -1;
            }
            printf("send cmd to extnal adp layer");
            strcpy( test_str, argv[2] );
            printf("argv[1]=%s\n", argv[2]);
            for (i=2; i<(argc-1); i++)
            {
                strcat(test_str, " ");
                strcat(test_str, argv[i+1]);
                printf("argv[%d]=%s\n", (i+1), argv[i+1]);
            }
            printf("test_str=%s\n", test_str);
            printf("test_str len=%d\n", strlen(test_str));
            sendMMITestCommand( fdext, MSG_ID_BT_BM_BLOCK_LIST_UPDATE_REQ, test_str, strlen(test_str));
            
            break;
      case BT_CMD_AVRCP_TEST_REQ:
            if( fdext < 0){
                //printf("send no fdext error!");
                //return -1;
            }
            printf("send cmd to extnal adp layer");
            strcpy( test_str, argv[2] );
            printf("argv[1]=%s\n", argv[2]);
            for (i=2; i<(argc-1); i++)
            {
                strcat(test_str, " ");
                strcat(test_str, argv[i+1]);
                printf("argv[%d]=%s\n", (i+1), argv[i+1]);
            }
            printf("test_str=%s\n", test_str);
            printf("test_str len=%d\n", strlen(test_str));
            sendMMITestCommand( fdext, MSG_ID_BT_AVRCP_TEST_CMD_REQ, test_str, strlen(test_str));
            
            break;            
        default:
            printf("unknown command\n");
            break;
        }
	
	return 0;
}
