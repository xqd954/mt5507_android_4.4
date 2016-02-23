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

/* 
 * bt_spp_api.c
 * 
 * This file is the External Adaptation API of SPP profile used by Application.
 * Flow direction: APP --> external ADP API
 */

#include "string.h"
#ifdef BTMTK_ON_WIN32
#include "malloc.h"
#include "stdio.h"
#endif	/* BTMTK_ON_WIN32 */

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
//#include "bluetooth_struct.h"  //for btmtk_gap_read_rssi_request
#include "bluetooth_spp_message.h"
#include "bluetooth_spp_struct.h"
#include "bt_spp_api.h"
#include "bluetooth_gap_struct.h"
//#include "ring.h"

//#include "btconfig.h"
//#include "bt_adp_spp.h"
#ifndef BTMTK_ON_LINUX
#include "bt_mmi_msg.h"
#endif
#include "memory.h"

#include "bt_adp_log.h"

#ifdef BTMTK_ON_LINUX
#include "bt_ext_debug.h"
#include <errno.h>
#endif

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Local function prototypes
 *
 ****************************************************************************/

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/

/* Define global data for MMI SPP SCO Server testing usage */
kal_uint8 MMI_SPP_SCO_SDP_PROCESS_HDLER_SET;
btbm_bd_addr_t MMI_SPP_SCO_CON_REQ_ADDR;
kal_uint8 MMI_SPP_SDP_ATD_TYPE;
kal_uint8 MMI_SPP_CHNL_ALWAYS_DISC_FIRST;
kal_uint8 MMI_DUN_CLIENT_TEST = FALSE;  /* TRUE for DUN_CLIENT test*/
U8  DUN_CLIENT_PORT;    /* for DUN_CLIENT test*/


//extern kal_uint8 *BtGet_SPPClientFirstConnDataPattern(kal_uint8 *len);

BTMTK_EventCallBack g_SPP_MMI_Callback = NULL;

#ifdef __SPP_SHARED_MEMORY__
SPPSharedMem g_PutDataBuffer[SPP_PORT_NUM];		// 3 SPP_Server; 1 DUN_SERVER;  3 SPP_CLIENT;
SPPSharedMem g_GetDataBuffer[SPP_PORT_NUM];	// 3 SPP_Server; 1 DUN_SERVER;  3 SPP_CLIENT;
#ifndef __SPP_DYNAMIC_MEMORY__
static U8 g_PutBuffer[SPP_PORT_NUM][SPP_PUT_DATA_SHARED_BUFFER_SIZE];
static U8 g_GetBuffer[SPP_PORT_NUM][SPP_GET_DATA_SHARED_BUFFER_SIZE];
#endif
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef BTMTK_ON_LINUX
int g_sppSockfd;

static void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size);
#endif

#ifdef __SPP_SHARED_MEMORY__
void btmtk_shared_buffer_init(void)
{
	U16 count;
	SPPSharedMem *sppMem = NULL;
	
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_PutDataBuffer[count];
		sppMem->inUse = FALSE;
		sppMem->port = 0;
		sppMem->buffer = NULL;

		sppMem = &g_GetDataBuffer[count];
		sppMem->inUse = FALSE;
		sppMem->port = 0;
		sppMem->buffer = NULL;
	}
}


void btmtk_shared_buffer_deinit(void)
{
	U16 count;
	SPPSharedMem *sppMem = NULL;
	
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_PutDataBuffer[count];
		if (TRUE == sppMem->inUse)
		{
			sppMem->inUse = FALSE;
			sppMem->port = 0;
#ifdef __SPP_DYNAMIC_MEMORY__
			free(sppMem->buffer);
#endif
			sppMem->buffer = NULL;
		}

		sppMem = &g_GetDataBuffer[count];
		if (TRUE == sppMem->inUse)
		{
			sppMem->inUse = FALSE;
			sppMem->port = 0;
#ifdef __SPP_DYNAMIC_MEMORY__
			free(sppMem->buffer);
#endif
			sppMem->buffer = NULL;
		}
	}
}


U8 *btmtk_get_shared_put_buffer_pointer(U8 port)
{
	U16 count;
	SPPSharedMem *sppMem = NULL;
	
	// Check whether a buffer is allocated to this port
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_PutDataBuffer[count];
		if ((port == sppMem->port) && (TRUE == sppMem->inUse))
		{
			return sppMem->buffer;
		}
	}

	// Allocate a buffer to this port
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_PutDataBuffer[count];
		if (TRUE != sppMem->inUse)
		{
#ifdef __SPP_DYNAMIC_MEMORY__
			sppMem->buffer = (U8 *) malloc(SPP_PUT_DATA_SHARED_BUFFER_SIZE);
#else
			sppMem->buffer = (U8 *) &g_PutBuffer[count];
#endif
			if (NULL == sppMem->buffer)
			{
				// Insufficient memory space
#ifdef BTMTK_ON_WIN32
				printf("[SPP] Insufficient memory space for data transmission!!\n");
#endif	/* BTMTK_ON_WIN32 */
				return NULL;
			}
			else
			{
				sppMem->inUse = TRUE;
				sppMem->port = port;
				return sppMem->buffer;
			}
		}
	}
	
	return NULL;
}


void btmtk_release_shared_put_buffer(U8 port)
{
	U16 count;
	SPPSharedMem *sppMem = NULL;
	
	// Check whether a buffer is allocated to this port
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_PutDataBuffer[count];
		if ((port == sppMem->port) && (TRUE == sppMem->inUse))
		{
			sppMem->inUse = FALSE;
			sppMem->port = 0;
#ifdef __SPP_DYNAMIC_MEMORY__
			free(sppMem->buffer);
#endif
			sppMem->buffer = NULL;
			return;
		}
	}
}


U8 *btmtk_get_shared_get_buffer_pointer(U8 port)
{
	U16 count;
	SPPSharedMem *sppMem = NULL;
	
	// Check whether a buffer is allocated to this port
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_GetDataBuffer[count];
		if ((port == sppMem->port) && (TRUE == sppMem->inUse))
		{
			return sppMem->buffer;
		}
	}

	// Allocate a buffer to this port
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_GetDataBuffer[count];
		if (TRUE != sppMem->inUse)
		{
#ifdef __SPP_DYNAMIC_MEMORY__
			sppMem->buffer = (U8 *) malloc(SPP_GET_DATA_SHARED_BUFFER_SIZE);
#else
			sppMem->buffer = (U8 *) &g_GetBuffer[count];
#endif
			if (NULL == sppMem->buffer)
			{
				// Insufficient memory space
#ifdef BTMTK_ON_WIN32
				printf("[SPP] Insufficient memory space for data transmission!!\n");
#endif	/* BTMTK_ON_WIN32 */
				return NULL;
			}
			else
			{
				sppMem->inUse = TRUE;
				sppMem->port = port;
				return sppMem->buffer;
			}
		}
	}
	
	return NULL;
}


void btmtk_release_shared_get_buffer(U8 port)
{
	U16 count;
	SPPSharedMem *sppMem = NULL;
	
	// Check whether a buffer is allocated to this port
	for (count = 0; count < SPP_PORT_NUM; count++)
	{
		sppMem = &g_GetDataBuffer[count];
		if ((port == sppMem->port) && (TRUE == sppMem->inUse))
		{
			sppMem->inUse = FALSE;
			sppMem->port = 0;
#ifdef __SPP_DYNAMIC_MEMORY__
			free(sppMem->buffer);
#endif
			sppMem->buffer = NULL;
			return;
		}
	}
}


#endif	/* __SPP_SHARED_MEMORY__ */


#ifdef __SPP_TESTER_
/*****************************************************************************
 * FUNCTION
 *  spp_get_bdaddr_from_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr     [IN]     
 *  string      [IN]     
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL spp_get_bdaddr_from_string(U8 *bd_addr, char *string)
{
    U8 i = 0;

    for (i = 0; i < 12; i++)
    {
        if ((string[i] >= '0') && (string[i] <= '9'))
        {
            string[i] -= '0';
        }
        else if ((string[i] <= 'f') && (string[i] >= 'a'))
        {
            string[i] = string[i] - 'a' + 10;
        }
        else if ((string[i] <= 'F') && (string[i] >= 'A'))
        {
            string[i] = string[i] - 'A' + 10;
        }
        else
        {
            return FALSE;
        }
    }
    bd_addr[5] = (string[0]) << 4 | (string[1]);
    bd_addr[4] = (string[2]) << 4 | (string[3]);
    bd_addr[3] = (string[4]) << 4 | (string[5]);
    bd_addr[2] = (string[6]) << 4 | (string[7]);
    bd_addr[1] = (string[8]) << 4 | (string[9]);
    bd_addr[0] = (string[10]) << 4 | (string[11]);
    return TRUE;
}


void spp_CONVERT_ARRAY2BDADDR(btbm_bd_addr_t *dest, U8 *src)
{
    dest->nap = (((U16) src[4])) | ((U16) src[5] << 8);
    dest->uap = (U8) src[3];
    dest->lap = (((U32) src[2]) << 16) | ((U32) src[1] << 8) | ((U32) src[0]);
}


/*****************************************************************************
 * FUNCTION
 *  sppa_handler_inject_msg
 * DESCRIPTION
 *  
 * PARAMETERS
 *  string      [IN]        
 *  index       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void sppa_handler_inject_msg(char *string, kal_uint8 index)
{
    if (strcmp(string, "sppenable") == 0)
    {
    	 //bt_trace(TRACE_FUNC, BT_SPP_INJECT_ENABLE, string);
        btmtk_spp_enable_req();
    }
    if (strcmp(string, "sppdisable") == 0)
    {
    	 //bt_trace(TRACE_FUNC, BT_SPP_INJECT_DISABLE, string);
        btmtk_spp_disable_req();
    }
    if (strncmp(string, "sppactivate", 11) == 0)
    {
    	 /* We have to inject DECIMAL UUID number (sppactivatexxxxx, at most 65535 <0xFFFF>) here */
    	 U16 uuid = 0;

	 //bt_trace(TRACE_FUNC, BT_SPP_INJECT_ACTIVATE, string);
	 
    	 string += 11;
    	 uuid = (string[0] - '0') * 10000 + (string[1] - '0') * 1000 + (string[2] - '0') * 100 + (string[3] - '0') * 10 + (string[4] - '0');
        btmtk_spp_activate_req(NULL, uuid);
    }
    if (strncmp(string, "sppdeactivate", 13) == 0)
    {
    	 /* sppdeactivatexx */
        U8 portNum = 0;

	 //bt_trace(TRACE_FUNC, BT_SPP_INJECT_DEACTIVATE, string);
	 
        string += 13;
        portNum = (string[0] - '0') * 10 + (string[1] - '0');
        btmtk_spp_deactivate_req(portNum);
    }
    if (strncmp(string, "sppdisconn", 10) == 0)
    {
    	 /* sppdisconn10 */
	 U8 portNum = 0;

    	 //bt_trace(TRACE_FUNC, BT_SPP_INJECT_DISCONNECT, string);
	 
        string += 10;
        portNum = (string[0] - '0') * 10 + (string[1] - '0');
        btmtk_spp_disconnect_req(portNum);
    }
    if (strncmp(string, "sppcon", 6) == 0)
    {
	/*
	 * for inject "SPP_CON_REQ"
	 * the format: sppconXXXXXaabbccddeeff
	 * XXXXX: UUID, at most 65535 <0xFFFF>
	 * aabbccddeeff: This is bd_address of peer's device
	 */
        kal_uint8 addr[BD_ADDR_LENGTH];
        btbm_bd_addr_t addr_t;
        U16 uuid = 0;

        //bt_trace(TRACE_FUNC, BT_SPP_INJECT_CONNECT, string);

        string += 6;
        uuid = (string[0] - '0') * 10000 + (string[1] - '0') * 1000 + (string[2] - '0') * 100 + (string[3] - '0') * 10 + (string[4] - '0');

        memset(addr, 0, BD_ADDR_LENGTH);
        string += 5;
        //Assert(spp_get_bdaddr_from_string(addr, string) == KAL_TRUE);
        spp_get_bdaddr_from_string(addr, string);
        spp_CONVERT_ARRAY2BDADDR(&addr_t, addr);

        btmtk_spp_connect_req(addr_t.lap, addr_t.uap, addr_t.nap, 0, uuid);
    }
    if (strncmp(string, "sppdata", 7) == 0)
    {
	/*
	 * Simulate SPP sends data
	 * Format: sppdataXXyyy
	 * XX: port number
	 * yyy: data to be transmitted
	 */
        U8 WRingBuf[51];
        U8 portNum = 0;

        //bt_trace(TRACE_FUNC, BT_SPP_INJECT_SEND_DATA, string);

        string += 7;
        portNum = (string[0] - '0') * 10 + (string[1] - '0');

        string += 2;
        //Assert(strlen(string) < 51);
        
        memset(WRingBuf, 0, 51);
        memcpy(WRingBuf, (const U8*)string, strlen(string));
        btmtk_spp_uart_put_bytes_req(portNum, WRingBuf, (U16) strlen(string));
    }

#if 0    
    //Simulate SPP Client sends data to SPP Server
    //format: sppc0dataxxx or sppc1dataxxx: c0: first SPP Client, c1: 2nd SPP Client, xxx: data sent to server 
    if (strncmp(string, "sppc", 4) == 0)
    {

        UART_PORT client_port = BT_START_OF_VIRTUAL_PORT + NUM_SPP_SRVS + NUM_DUN_SRVS + NUM_SPP_SCO_SRVS + NUM_FAX_SRVS;
        kal_uint16 returnWriteLen = 0;
        kal_uint8 WRingBuf[51];

        if (strncmp(&string[5], "data", 4) == 0)
        {
            client_port += (string[4] - '0');
            string += 9;
            /* KAL_NOT_SUPPORT(TRACE_GROUP_1, Client sends data=%s,len=%d, at port=%d, ,string,strlen(string),client_port); */
            memset(WRingBuf, 0, 51);
            Assert(strlen(string) < 51);
            memcpy(WRingBuf, (const U8*)string, strlen(string));
            returnWriteLen = UART_PutBytes(client_port, (kal_uint8*) WRingBuf, strlen(string), MOD_MMI);
        }

    }
    //Simulate SPP Server sends data to SPP Client
    //Format: sppsxdatayyy ; x=0~(NUM_SPP_SRVS-1) and yyy=means that string sent to server
    //x is the index of SPPOSDev[] 
    if (strncmp(string, "spps", 4) == 0)
    {
        if (strncmp(&string[5], "data", 4) == 0)
        {
            UART_PORT server_port = BT_START_OF_VIRTUAL_PORT;
            kal_uint16 returnWriteLen = 0;
            kal_uint8 WRingBuf[51];

            server_port += (string[4] - '0');
            Assert(server_port < (BT_START_OF_VIRTUAL_PORT + NUM_SPP_SRVS));

            string += 9;
            Assert(strlen(string) < 51);
            /* KAL_NOT_SUPPORT(TRACE_GROUP_1, Server sends data=%s,len=%d, at port=%d, ,string,strlen(string),server_port); */
            memset(WRingBuf, 0, 51);
            memcpy(WRingBuf, (const U8*)string, strlen(string));
            returnWriteLen = UART_PutBytes(server_port, (kal_uint8*) WRingBuf, strlen(string), MOD_MMI);
        }

    }

    if (strncmp(string, "sppsdpmode", 10) == 0)
    {
        if (string[10] == '0')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, USE_MMI_SPP_SCO_SDP_PROCESS_HDLER_SETS_0_FOR_SPP_CLIENT_APPLICATION);
        #endif 
            MMI_SPP_SCO_SDP_PROCESS_HDLER_SET = 0;
        }
        else if (string[10] == '1')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, USE_MMI_SPP_SCO_SDP_PROCESS_HDLER_SETS_1_FOR_TESTING_MODE);
        #endif 
            MMI_SPP_SCO_SDP_PROCESS_HDLER_SET = 1;
        }
    }

    if (strncmp(string, "sppsetscoaddr", 13) == 0)
    {
        kal_uint8 addr[BD_ADDR_LENGTH];

        string += 13;
        Assert(spp_get_bdaddr_from_string(addr, string) == KAL_TRUE);
        MMI_SPP_SCO_CON_REQ_ADDR.lap = 0;
        MMI_SPP_SCO_CON_REQ_ADDR.uap = 0;
        MMI_SPP_SCO_CON_REQ_ADDR.nap = 0;
        spp_CONVERT_ARRAY2BDADDR(&MMI_SPP_SCO_CON_REQ_ADDR, addr);
    #ifdef MTK_BT_DEBUG_TRACE
        kal_trace(
            TRACE_GROUP_1,
            SET_SPP_SCO_CON_REQ_BD_ADDR_WITH_LAP_BT_DEBUGx04X_UAPBT_DEBUGx04XNAPBT_DEBUGx04X,
            MMI_SPP_SCO_CON_REQ_ADDR.lap,
            MMI_SPP_SCO_CON_REQ_ADDR.uap,
            MMI_SPP_SCO_CON_REQ_ADDR.nap);
    #endif /* MTK_BT_DEBUG_TRACE */ 

    }

    if (strncmp(string, "sppaidtype", 10) == 0)
    {
        if (string[10] == '0')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, MMI_SPP_SDP_WITH_ATTRIBUTEID0X0004__PROTOCOLDESCRIPTORLIST);
        #endif 
            MMI_SPP_SDP_ATD_TYPE = 0;
        }
        else if (string[10] == '1')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, MMI_SPP_SDP_WITH_ATTRIBUTEID0X0310__SUPPORT_CAPABILITIES);
        #endif 
            MMI_SPP_SDP_ATD_TYPE = 1;
        }
    }
#if DUN_CLIENT == XA_ENABLED    
    if (strncmp(string, "sppdunclient", 12) == 0)   /* DUN_CLIENT */
    {
        if (string[12] == '0')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            Report(("DUN client off"));
        #endif 
            MMI_DUN_CLIENT_TEST = FALSE;
        }
        else if (string[12] == '1')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            Report(("DUN client on"));
        #endif 
            MMI_DUN_CLIENT_TEST = TRUE;
        }
    }    
    if (strncmp(string, "sppdiscdunclient", 16) == 0)   /* DUN_CLIENT */
    {
        bt_dun_disconnect_req_struct *msg_ptr;
        msg_ptr = (bt_dun_disconnect_req_struct*) construct_local_para(sizeof(bt_dun_disconnect_req_struct), TD_UL);
    
        Report(("1Disconnect DUN client in port %d", DUN_CLIENT_PORT));
        if (strlen(string) == 16)
        {
	     msg_ptr->port = DUN_CLIENT_PORT;
	     SPPOS_sendMsg(MSG_ID_BT_DUN_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) msg_ptr, NULL);
        }
        else
        {
            U32 port = (string[16] - 48)*10 + (string[17] - 48);
            Report(("2Disconnect DUN client in port %d", DUN_CLIENT_PORT));
	     msg_ptr->port = port;
	     SPPOS_sendMsg(MSG_ID_BT_DUN_DISCONNECT_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) msg_ptr, NULL);
        }
        
    }    
#endif /* #if DUN_CLIENT == XA_ENABLED */
    
    
    if (strncmp(string, "sppdiscmode", 11) == 0)
    {
        if (string[11] == '1')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, DISC_SPP_CHNL_FIRST_EVEN_THE_AUDIO_LINK_IS_CONNECTED);
        #endif 
            MMI_SPP_CHNL_ALWAYS_DISC_FIRST = 1;

        }
        else if (string[11] == '0')
        {
        #ifdef MTK_BT_DEBUG_TRACE
            kal_trace(TRACE_GROUP_1, DISC_SPP_AUDIO_LINK_FIRST_IF_AUDIO_LINK_IS_CONNECTED);
        #endif 
            MMI_SPP_CHNL_ALWAYS_DISC_FIRST = 0;
        }
    }
#endif // 0
}
#endif /* __SPP_TESTER_ */



#ifdef BTMTK_ON_WISE
#else
kal_bool CheckCustomSPPFlowControlSetting(module_type owner)
{
    /* If the customer change BT SPP Srv or Client Applications's Port owner from MOD_MMI to other MODs 
      * to handle TX/RX data based on UART-related messages, once this spp connection is closed:
      * The port owner module must be return a MSG_ID_BT_SPP_UART_PLUGOUT_CNF backs to MOD_BT
      * -> Before BT revcives this msg, it will not reset the spp port owner to default module,
      *      It also implies that if this msg is not received, any incoming spp connection request after that will be rejected
      *      For MOD BT, if this msg is not received, it will think that org port owner is not finished yet!
      * Please fill the module of the SPP port in this function!
      * If the customer does not change the SPP Srv or SPP Client Port owner, it's owner will be MOD_MMI
      * Please see BTMMISPP.c file
      */
    if ((owner == MOD_MMI))
    {
        return KAL_TRUE;
    }
    return KAL_FALSE;
}
#endif


//#if BT_SPP_AUTHORIZE_BY_MMI
void btmtk_spp_connect_ind_rsp(U8 port, U32 lap, U8 uap, U16 nap, U8 result)
{
	btmtk_spp_connect_ind_rsp_struct msg;

	msg.port = port;
	msg.lap = lap;
	msg.uap = uap;
	msg.nap = nap;
	msg.result = result;

	BTCMD_SendMessage(MSG_ID_BT_SPP_CONNECT_IND_RSP, MOD_BT, &msg, sizeof(btmtk_spp_connect_ind_rsp_struct));
}


//#else /* BT_SPP_AUTHORIZE_BY_MMI */ 
void btmtk_spp_auth_rsp(U8 port, U8 result)
{
	btmtk_spp_auth_rsp_struct msg;

	msg.port = port;
	msg.result = result;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_AUTH_RSP, MOD_BT, &msg, sizeof(btmtk_spp_auth_rsp_struct));
}
//#endif /* BT_SPP_AUTHORIZE_BY_MMI */


void btmtk_spp_uart_owner_cnf(U8 port)
{
	btmtk_spp_uart_owner_cnf_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_OWNER_CNF, MOD_BT, &msg, sizeof(btmtk_spp_uart_owner_cnf_struct));
}


void btmtk_spp_uart_plugout_cnf(U8 port)
{
	btmtk_spp_uart_plugout_cnf_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_PLUGOUT_CNF, MOD_BT, &msg, sizeof(btmtk_spp_uart_plugout_cnf_struct));
}


#if SPP_CLIENT == XA_ENABLED
void btmtk_spp_connect_req(U32 lap, U8 uap, U16 nap, U8 server_chnl_num, U16 uuid)
{
	btmtk_spp_connect_req_struct msg;

	msg.lap = lap;
	msg.uap = uap;
	msg.nap = nap;
	msg.server_chnl_num = server_chnl_num;
	msg.uuid = uuid;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_CONNECT_REQ, MOD_BT, &msg, sizeof(btmtk_spp_connect_req_struct));
}
#endif /* SPP_CLIENT == XA_ENABLED */


void btmtk_spp_activate_req(char *svcName, U16 svcUUID)
{
	btmtk_spp_activate_req_struct msg;
	I16 length = 0;

	memset(msg.svcName, 0, sizeof(msg.svcName));
	msg.svcUUID = svcUUID;
	if (NULL != svcName)
	{
		length = (strlen(svcName) < SPP_MAX_SERVICE_NAME_LENGTH)?strlen(svcName):SPP_MAX_SERVICE_NAME_LENGTH;
		memcpy(msg.svcName, svcName, length);
	}

	BTCMD_SendMessage(MSG_ID_BT_SPP_ACTIVATE_REQ, MOD_BT, &msg, sizeof(btmtk_spp_activate_req_struct));
}


#if 1
void btmtk_spp_send_data_req(U8 port)
{
	btmtk_spp_send_data_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_SEND_DATA_REQ, MOD_BT, &msg, sizeof(btmtk_spp_send_data_req_struct));
}


void btmtk_spp_get_data_req(U8 port)
{
	btmtk_spp_get_data_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_GET_DATA_REQ, MOD_BT, &msg, sizeof(btmtk_spp_get_data_req_struct));
}
#endif


void btmtk_spp_deactivate_req(U8 port)
{
	btmtk_spp_deactivate_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_DEACTIVATE_REQ, MOD_BT, &msg, sizeof(btmtk_spp_deactivate_req_struct));
}


void btmtk_spp_disconnect_req(U8 port)
{
	btmtk_spp_disconnect_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_DISCONNECT_REQ, MOD_BT, &msg, sizeof(btmtk_spp_disconnect_req_struct));
}


void btmtk_spp_initialize_req(void)
{
	BTCMD_SendMessage(MSG_ID_BT_SPP_INITIALIZE_REQ, MOD_BT, NULL, 0);
}


#if 0
void btmtk_spp_register_callback_req(SPPCallback callback)
{
	btmtk_spp_register_callback_req_struct msg;

	msg.callback = callback;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_REGISTER_CALLBACK_REQ, MOD_BT, &msg, sizeof(btmtk_spp_register_callback_req_struct));
}
#endif


void btmtk_spp_uart_put_bytes_req(U8 port, U8* buffer, U16 length)
{
	btmtk_spp_uart_put_bytes_req_struct msg;
	U16 lengthToSend;
	U16 lengthSent;
	U16 maxBufferLength;

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	lengthToSend = 0;
	lengthSent = 0;
	maxBufferLength = 0;
	
#ifdef __SPP_SHARED_MEMORY__
	maxBufferLength = SPP_PUT_DATA_SHARED_BUFFER_SIZE;
#else
	maxBufferLength = SPP_ADP_UART_PUT_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef __SPP_SHARED_MEMORY__
	if (0 < length)
#else
	while (0 < (length - lengthSent))
#endif	/* __SPP_SHARED_MEMORY__ */
	{
		if (maxBufferLength < (length - lengthSent))
		{
			lengthToSend = maxBufferLength;
		}
		else
		{
			lengthToSend = length - lengthSent;
		}
		msg.length = lengthToSend;
#ifdef __SPP_SHARED_MEMORY__
		//msg.buffer = btmtk_get_shared_put_buffer_pointer(msg.port);
		msg.buffer = buffer;
#endif	/* __SPP_SHARED_MEMORY__ */
#ifndef BTMTK_ON_LINUX
		memcpy(msg.buffer, (buffer + (U8) lengthSent), lengthToSend);
#endif

		BTCMD_SendMessage(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_put_bytes_req_struct));
		lengthSent += lengthToSend;
	}
}


void btmtk_spp_uart_get_bytes_req(U8 port, U16 length)
{
	btmtk_spp_uart_get_bytes_req_struct msg;
	U16 maxBufferLength;

#ifdef __SPP_SHARED_MEMORY__
	maxBufferLength = SPP_GET_DATA_SHARED_BUFFER_SIZE;
#else
	maxBufferLength = SPP_ADP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	msg.length = (length < maxBufferLength)?length:maxBufferLength;
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_GET_BYTES_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_get_bytes_req_struct));
}


void btmtk_spp_uart_open_req(U8 port)
{
	btmtk_spp_uart_open_req_struct msg;

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_OPEN_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_open_req_struct));
}


void btmtk_spp_uart_close_req(U8 port)
{
	btmtk_spp_uart_close_req_struct msg;

	msg.port = port;
	///msg.owner = MOD_EXTERNAL;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_CLOSE_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_close_req_struct));
}


void btmtk_spp_uart_set_owner_req(U8 port)
{
	btmtk_spp_uart_set_owner_req_struct msg;

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_SET_OWNER_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_set_owner_req_struct));
}


void btmtk_spp_register_mmi_callback_req(BTMTK_EventCallBack callback)
{
	g_SPP_MMI_Callback = callback;
}


/* DUN related API */
//#if BT_SPP_AUTHORIZE_BY_MMI
void btmtk_dun_connect_ind_rsp(U8 port, U32 lap, U8 uap, U16 nap, U8 result)
{
	btmtk_dun_connect_ind_rsp_struct msg;

	msg.port = port;
	msg.lap = lap;
	msg.uap = uap;
	msg.nap = nap;
	msg.result = result;

	BTCMD_SendMessage(MSG_ID_BT_DUN_CONNECT_IND_RSP, MOD_BT, &msg, sizeof(btmtk_dun_connect_ind_rsp_struct));
}


//#else /* BT_SPP_AUTHORIZE_BY_MMI */ 
void btmtk_dun_auth_rsp(U8 port, U8 result)
{
	btmtk_dun_auth_rsp_struct msg;

	msg.port = port;
	msg.result = result;
	
	BTCMD_SendMessage(MSG_ID_BT_DUN_AUTH_RSP, MOD_BT, &msg, sizeof(btmtk_dun_auth_rsp_struct));
}
//#endif /* BT_SPP_AUTHORIZE_BY_MMI */


#if DUN_CLIENT == XA_ENABLED
void btmtk_dun_connect_req(U32 lap, U8 uap, U16 nap, U8 server_chnl_num)
{
	btmtk_dun_connect_req_struct msg;

	msg.lap = lap;
	msg.uap = uap;
	msg.nap = nap;
	msg.server_chnl_num = server_chnl_num;
	
	BTCMD_SendMessage(MSG_ID_BT_DUN_CONNECT_REQ, MOD_BT, &msg, sizeof(btmtk_dun_connect_req_struct));
}
#endif /* DUN_CLIENT == XA_ENABLED */


void btmtk_dun_activate_req(void)
{
	BTCMD_SendMessage(MSG_ID_BT_DUN_ACTIVATE_REQ, MOD_BT, NULL, 0);
}


void btmtk_dun_deactivate_req(U8 port)
{
	btmtk_dun_deactivate_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_DUN_DEACTIVATE_REQ, MOD_BT, &msg, sizeof(btmtk_dun_deactivate_req_struct));
}


void btmtk_dun_disconnect_req(U8 port)
{
	btmtk_dun_disconnect_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_DUN_DISCONNECT_REQ, MOD_BT, &msg, sizeof(btmtk_dun_disconnect_req_struct));
}


void btmtk_dun_uart_put_bytes_req(U8 port, U8* buffer, U16 length)
{
	btmtk_spp_uart_put_bytes_req_struct msg;
	U16 lengthToSend;
	U16 lengthSent;
	U16 maxBufferLength;

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	lengthToSend = 0;
	lengthSent = 0;
	maxBufferLength = 0;
	
#ifdef __SPP_SHARED_MEMORY__
	maxBufferLength = SPP_PUT_DATA_SHARED_BUFFER_SIZE;
#else
	maxBufferLength = SPP_ADP_UART_PUT_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef __SPP_SHARED_MEMORY__
	if (0 < length)
#else
	while (0 < (length - lengthSent))
#endif	/* __SPP_SHARED_MEMORY__ */
	{
		if (maxBufferLength < (length - lengthSent))
		{
			lengthToSend = maxBufferLength;
		}
		else
		{
			lengthToSend = length - lengthSent;
		}
		msg.length = lengthToSend;
#ifdef __SPP_SHARED_MEMORY__
		//msg.buffer = btmtk_get_shared_put_buffer_pointer(msg.port);
		msg.buffer = buffer;
#endif	/* __SPP_SHARED_MEMORY__ */
		memcpy(msg.buffer, (buffer + (U8) lengthSent), lengthToSend);

		BTCMD_SendMessage(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_put_bytes_req_struct));
		lengthSent += lengthToSend;
	}
}


void btmtk_dun_uart_get_bytes_req(U8 port, U16 length)
{
	btmtk_spp_uart_get_bytes_req_struct msg;
	U16 maxBufferLength;

#ifdef __SPP_SHARED_MEMORY__
	maxBufferLength = SPP_GET_DATA_SHARED_BUFFER_SIZE;
#else
	maxBufferLength = SPP_ADP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	msg.length = (length < maxBufferLength)?length:maxBufferLength;
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_GET_BYTES_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_get_bytes_req_struct));
}


void btmtk_dun_register_mmi_callback_req(BTMTK_EventCallBack callback)
{
	g_SPP_MMI_Callback = callback;
}


/* FAX related API */
//#if BT_SPP_AUTHORIZE_BY_MMI
void btmtk_fax_connect_ind_rsp(U8 port, U32 lap, U8 uap, U16 nap, U8 result)
{
	btmtk_spp_connect_cnf_struct msg;

	msg.port = port;
	msg.lap = lap;
	msg.uap = uap;
	msg.nap = nap;
	msg.result = result;

	BTCMD_SendMessage(MSG_ID_BT_FAX_CONNECT_IND_RSP, MOD_BT, &msg, sizeof(btmtk_spp_connect_cnf_struct));
}


//#else /* BT_SPP_AUTHORIZE_BY_MMI */ 
void btmtk_fax_auth_rsp(U8 port, U8 result)
{
	btmtk_fax_auth_rsp_struct msg;

	msg.port = port;
	msg.result = result;
	
	BTCMD_SendMessage(MSG_ID_BT_FAX_AUTH_RSP, MOD_BT, &msg, sizeof(btmtk_fax_auth_rsp_struct));
}
//#endif /* BT_SPP_AUTHORIZE_BY_MMI */


#if FAX_CLIENT == XA_ENABLED
void btmtk_fax_connect_req(U32 lap, U8 uap, U16 nap, U8 server_chnl_num)
{
	btmtk_fax_connect_req_struct msg;

	msg.lap = lap;
	msg.uap = uap;
	msg.nap = nap;
	msg.server_chnl_num = server_chnl_num;
	
	BTCMD_SendMessage(MSG_ID_BT_FAX_CONNECT_REQ, MOD_BT, &msg, sizeof(btmtk_fax_connect_req_struct));
}
#endif /* FAX_CLIENT == XA_ENABLED */


void btmtk_fax_activate_req(void)
{
	BTCMD_SendMessage(MSG_ID_BT_FAX_ACTIVATE_REQ, MOD_BT, NULL, 0);
}


void btmtk_fax_deactivate_req(U8 port)
{
	btmtk_fax_deactivate_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_FAX_DEACTIVATE_REQ, MOD_BT, &msg, sizeof(btmtk_fax_deactivate_req_struct));
}


void btmtk_fax_disconnect_req(U8 port)
{
	btmtk_fax_disconnect_req_struct msg;

	msg.port = port;
	
	BTCMD_SendMessage(MSG_ID_BT_FAX_DISCONNECT_REQ, MOD_BT, &msg, sizeof(btmtk_fax_disconnect_req_struct));
}


void btmtk_fax_uart_put_bytes_req(U8 port, U8* buffer, U16 length)
{
	btmtk_spp_uart_put_bytes_req_struct msg;
	U16 lengthToSend;
	U16 lengthSent;
	U16 maxBufferLength;

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	lengthToSend = 0;
	lengthSent = 0;
	maxBufferLength = 0;
	
#ifdef __SPP_SHARED_MEMORY__
	maxBufferLength = SPP_PUT_DATA_SHARED_BUFFER_SIZE;
#else
	maxBufferLength = SPP_ADP_UART_PUT_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef __SPP_SHARED_MEMORY__
	if (0 < length)
#else
	while (0 < (length - lengthSent))
#endif	/* __SPP_SHARED_MEMORY__ */
	{
		if (maxBufferLength < (length - lengthSent))
		{
			lengthToSend = maxBufferLength;
		}
		else
		{
			lengthToSend = length - lengthSent;
		}
		msg.length = lengthToSend;
#ifdef __SPP_SHARED_MEMORY__
		//msg.buffer = btmtk_get_shared_put_buffer_pointer(msg.port);
		msg.buffer = buffer;
#endif	/* __SPP_SHARED_MEMORY__ */
		memcpy(msg.buffer, (buffer + (U8) lengthSent), lengthToSend);

		BTCMD_SendMessage(MSG_ID_BT_SPP_UART_PUT_BYTES_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_put_bytes_req_struct));
		lengthSent += lengthToSend;
	}
}


void btmtk_fax_uart_get_bytes_req(U8 port, U16 length)
{
	btmtk_spp_uart_get_bytes_req_struct msg;
	U16 maxBufferLength;

#ifdef __SPP_SHARED_MEMORY__
	maxBufferLength = SPP_GET_DATA_SHARED_BUFFER_SIZE;
#else
	maxBufferLength = SPP_ADP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */

	msg.port = port;
	//msg.owner = MOD_EXTERNAL;
	msg.length = (length < maxBufferLength)?length:maxBufferLength;
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_GET_BYTES_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_get_bytes_req_struct));
}


void btmtk_fax_register_mmi_callback_req(BTMTK_EventCallBack callback)
{
	g_SPP_MMI_Callback = callback;
}


///////////////////////////////
/* Special customization for WISE */
///////////////////////////////
void btmtk_spp_enable_req(void)
{
#ifdef __SPP_SHARED_MEMORY__
	btmtk_shared_buffer_init();
#endif	/* __SPP_SHARED_MEMORY__ */
	BTCMD_SendMessage(MSG_ID_BT_SPP_ENABLE_REQ, MOD_BT, NULL, 0);
}


void btmtk_spp_disable_req(void)
{
#ifdef __SPP_SHARED_MEMORY__
	btmtk_shared_buffer_deinit();
#endif	/* __SPP_SHARED_MEMORY__ */
	BTCMD_SendMessage(MSG_ID_BT_SPP_DISABLE_REQ, MOD_BT, NULL, 0);
}


void btmtk_spp_uart_data_available_ind_rsp(U8 port, U16 length)
{
	btmtk_spp_uart_data_available_ind_rsp_struct msg;

	msg.port = port;
	msg.length = length;
	BTCMD_SendMessage(MSG_ID_BT_SPP_UART_DATA_AVAILABLE_IND_RSP, MOD_BT, &msg, sizeof(btmtk_spp_uart_data_available_ind_rsp_struct));
}


#ifdef __SPP_SHARED_MEMORY__
void btmtk_spp_uart_assign_buffer_req(U8 port)
{
	btmtk_spp_uart_assign_buffer_req_struct msg;

	msg.port = port;
#ifdef BTMTK_ON_LINUX
         msg.getBufPtr = NULL;	// get shared memory in bt process
#else
	msg.getBufPtr = btmtk_get_shared_get_buffer_pointer(msg.port);
#endif
	msg.getBufSize = SPP_GET_DATA_SHARED_BUFFER_SIZE;

	BTCMD_SendMessage(MSG_ID_BT_UART_ASSIGN_BUFFER_REQ, MOD_BT, &msg, sizeof(btmtk_spp_uart_assign_buffer_req_struct));
}


void btmtk_spp_uart_release_buffer_req(U8 port)
{
	if (0 < port)
	{
		btmtk_release_shared_put_buffer(port);
		btmtk_release_shared_get_buffer(port);
	}
}
#endif	/* __SPP_SHARED_MEMORY__ */

#ifdef BTMTK_ON_LINUX
void btmtk_spp_set_sockfd(int sockfd)
{
    g_sppSockfd = sockfd;
}

static void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
	int ret = -1;
	ilm_struct ilm;
	U16 *field;

	bt_ext_log("[SPP][ADP] BTCMD_SendMessage: %d", msg_id);

	memset(&ilm, 0, sizeof(ilm_struct));
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

	if(g_sppSockfd > 0)
	{
		ret = send(g_sppSockfd, (void *)&ilm, size, 0);
		if(ret < 0)
		{
			bt_ext_err("[SPP][ADP] BTCMD_SendMessage: send fail: %s, %d", strerror(errno), errno);
		}
		bt_ext_log("[SPP][ADP] send done: %d", ret);
	}
	else
	{
		bt_ext_err("[SPP][ADP] BTCMD_SendMessage: socket not initialized");
	}
}
#endif
