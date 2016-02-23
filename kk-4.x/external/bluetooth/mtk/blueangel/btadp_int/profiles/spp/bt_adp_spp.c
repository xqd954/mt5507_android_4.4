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
 * bt_adp_spp.c
 * 
 * This file is the Internal Adaptation of SPP profile that receives message from external ADP and use profile API.
 * Flow direction: external ADP API --> message queue --> internal ADP --> profile API
 */

#include "spp.h"
#include "bt_adp_spp.h"
#include "osapi.h"
#include "bluetooth_spp_struct.h"
#include "bt_adp_spp_event.h"

#ifdef BTMTK_ON_LINUX
#include "bt_adp_spp_ashm.h"
#endif
#ifdef __SPP_THROUGHPUT_TEST__
#include "time.h"
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
#ifdef __SPP_THROUGHPUT_TEST__
#define SPP_THROUGHPUT_TEST_BUFFER_LENGTH 256

U32 g_sppThroughputTotalSentLength;
U32 g_sppThroughputPeriodicSentLength;
U32 g_sppThroughputTotalGotLength;
U8 g_sppThroughputTestServerPort;
U8 g_sppThroughputTestClientPort;
time_t g_sppThroughputTestTimeSendStart;
time_t g_sppThroughputTestTimeSendStop;
time_t g_sppThroughputTestPeriodicTimeSendStop;
time_t g_sppThroughputTestTimeGetStart;
time_t g_sppThroughputTestTimeGetStop;
#endif


void spp_adp_init(void)
{
	SPP_Register(btmtk_adp_spp_int_event_callback);
	SPPDRV_Init();
#ifdef BTMTK_ON_LINUX
	btmtk_adp_spp_malloc_ashm_buffer();
#endif
}


#ifdef __SPP_THROUGHPUT_TEST__
/* 
 * 1. Register one Server port & one client port
 * 2. Handle received data
 * 3. CONNECT_REQ & auto accept CONNECT_IND_REQ
 * 4. CONNECT_CNF & CONNECT_IND ==> count total amout & time
 * 5. Save Port number for data transmission
 * 6. Open UART port
 * 7. Assign buffer (NOT necessary)
 * 8. Send data start
 * 9. Disconnect after a specific quantum of data is sent
 * 10. DISCONNECT_IND ==> Calculate throughput
 */
void spp_throughput_test_start(BOOL bServer)
{
		printf("spp_throughput_test_start(%d)\n", bServer);
	g_sppThroughputTotalSentLength = 0;
	g_sppThroughputPeriodicSentLength = 0;
	g_sppThroughputTotalGotLength = 0;
	g_sppThroughputTestServerPort = 0;
	g_sppThroughputTestClientPort = 0;
	g_sppThroughputTestTimeSendStart = 0;
	g_sppThroughputTestTimeSendStop = 0;
	g_sppThroughputTestPeriodicTimeSendStop = 0;

        if(bServer)
        {
	    SPPHandleActivateReq(SPP_SERVER_PORT, NULL, 0);
        }
        else
        {
	    SPPHandleActivateReq(SPP_CLIENT_PORT, NULL, 0);
	    SPPHandleConnectReq(
               #if 1
               /* 363 */
	    	0x6611A0/* lap (U32) */, 
		0x46/* uap (U8) */, 
		0xDEF5/* nap (U16) */,                
		#endif
               #if 0
               /* 361 */
	    	0x66112F/* lap (U32) */, 
		0x46/* uap (U8) */, 
		0xFE83/* nap (U16) */, 
               /* 336 */
	    	0x6611B6/* lap (U32) */, 
		0x46/* uap (U8) */, 
		0xB6A4/* nap (U16) */,
		#endif
		0, 
		0);
        }
}


void spp_throughput_test_send(BOOL bServer)
{
	U8 sendBuf[SPP_THROUGHPUT_TEST_BUFFER_LENGTH];
	U16 u2Count = 0;
	U16 u2WriteLengthOnce = 0;
	U16 u2PeriodCount = 0;
//printf("spp_throughput_test_send(%d)\n", bServer?1:0);
	for (u2Count = 0; u2Count < SPP_THROUGHPUT_TEST_BUFFER_LENGTH; u2Count++)
	{
		sendBuf[u2Count] = u2Count;
	}

	for (u2Count = 0; u2Count < 8; u2Count++)
	{
		if(bServer)
		{
			SPPUartPutBytes(g_sppThroughputTestServerPort,
							sendBuf,
							SPP_THROUGHPUT_TEST_BUFFER_LENGTH,
							MOD_EXTERNAL
							);
		}
		else
		{
			/*
			SPPUartPutBytes(g_sppThroughputTestClientPort,
							sendBuf,
							SPP_THROUGHPUT_TEST_BUFFER_LENGTH,
							MOD_EXTERNAL
							);
							*/
		}
	}
	if(bServer)
  {		
              //printf("SPP server start to send data\n");
		time(&g_sppThroughputTestTimeSendStart);
                g_sppThroughputTestPeriodicTimeSendStop = g_sppThroughputTestTimeSendStart;
		SPPHandleSendDataReq(g_sppThroughputTestServerPort);
  }
  else
 	{
		/*time(&g_sppThroughputTestTimeSendStart);
		SPPHandleSendDataReq(g_sppThroughputTestClientPort);*/
 	}
}
#endif


/*****************************************************************************
 * FUNCTION
 *  sppa_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ilm_ptr     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_adp_spp_handle_message(ilm_struct *ilm_ptr)
{

    switch (ilm_ptr->msg_id)
    {

    #if BT_SPP_AUTHORIZE_BY_MMI
            /* After BT v2.1: new sap for MMI AUTHORIZE is applied */
        case MSG_ID_BT_SPP_CONNECT_IND_RSP:
        {
            btmtk_spp_connect_ind_rsp_struct *connect_ind_res = 
                (btmtk_spp_connect_ind_rsp_struct *) ilm_ptr->local_para_ptr;
            SPPHandleConnectIndRes(
                connect_ind_res->port, 
                connect_ind_res->lap, 
                connect_ind_res->uap, 
                connect_ind_res->nap, 
                connect_ind_res->result);
            break;
        }        
        case MSG_ID_BT_DUN_CONNECT_IND_RSP:
        {
            btmtk_dun_connect_ind_rsp_struct *connect_ind_res = 
                (btmtk_dun_connect_ind_rsp_struct *) ilm_ptr->local_para_ptr;
            DUNHandleConnectIndRes(
                connect_ind_res->port, 
                connect_ind_res->lap, 
                connect_ind_res->uap, 
                connect_ind_res->nap, 
                connect_ind_res->result);
            break;
}
        case MSG_ID_BT_FAX_CONNECT_IND_RSP:
        {
            btmtk_fax_connect_ind_rsp_struct *connect_ind_res = 
                (btmtk_fax_connect_ind_rsp_struct *) ilm_ptr->local_para_ptr;
            FAXHandleConnectIndRes(
                connect_ind_res->port, 
                connect_ind_res->lap, 
                connect_ind_res->uap, 
                connect_ind_res->nap, 
                connect_ind_res->result);
            break;
        }        
    #else /* BT_SPP_AUTHORIZE_BY_MMI */ 
            /* Add this sap mg processing case : 2007-0115 */
        case MSG_ID_BT_SPP_AUTH_RSP:
        {
            btmtk_spp_auth_rsp_struct *auth_rsp = 
                (btmtk_spp_auth_rsp_struct *)ilm_ptr->local_para_ptr;
            SPPHandleAuthRsp(auth_rsp->port, auth_rsp->result);
            break;
        }
        case MSG_ID_BT_DUN_AUTH_RSP:
        {
            btmtk_dun_auth_rsp_struct *auth_rsp = 
                (btmtk_dun_auth_rsp_struct *)ilm_ptr->local_para_ptr;
            SPPHandleAuthRsp(auth_rsp->port, auth_rsp->result);
            break;
        }
        case MSG_ID_BT_FAX_AUTH_RSP:
        {
            btmtk_fax_auth_rsp_struct *auth_rsp = 
                (btmtk_fax_auth_rsp_struct *)ilm_ptr->local_para_ptr;
            SPPHandleAuthRsp(auth_rsp->port, auth_rsp->result);
            break;
        }
    #endif /* BT_SPP_AUTHORIZE_BY_MMI */

            /* Add this sap msg for checking the MMI is already owner of port: 2007-0402 */

        case MSG_ID_BT_SPP_UART_OWNER_CNF:
            SPPHandleUartOwnerCnf(((btmtk_spp_uart_owner_cnf_struct *)ilm_ptr->local_para_ptr)->port);
            break;
            /*
             * Add this sap msg 2007-0409: if MMI is owner of port, it may do something after receiving UART_PLUGOUT_IND.
             * Before MMI receives UART_PLUGOUT_IND, BT should not change the owner of uart port.
             * So, BT can change owner of uart port when receive BT_SPP_UART_PLUGOUT_CNF msg.
             */
        case MSG_ID_BT_SPP_UART_PLUGOUT_CNF:
            UartHandlePlugoutCnf(((btmtk_spp_uart_plugout_cnf_struct*) ilm_ptr->local_para_ptr)->port);
            break;
    #if DUN_CLIENT == XA_ENABLED        
        case MSG_ID_BT_DUN_CONNECT_REQ:
        {
            btmtk_dun_connect_req_struct *connect_req;
            DUNHandleActivateReq(SPP_CLIENT_PORT);
            connect_req = (btmtk_dun_connect_req_struct*) ilm_ptr->local_para_ptr;
            DUNHandleConnectReq(
                connect_req->lap, 
                connect_req->uap, 
                connect_req->nap, 
                connect_req->server_chnl_num);
            break;        
        }            
    #endif /* DUN_CLIENT == XA_ENABLED */
    #if FAX_CLIENT == XA_ENABLED        
        case MSG_ID_BT_FAX_CONNECT_REQ:
        {
            btmtk_fax_connect_req_struct *connect_req;
            FAXHandleActivateReq(SPP_CLIENT_PORT);
            connect_req = (btmtk_dun_connect_req_struct*) ilm_ptr->local_para_ptr;
            FAXHandleConnectReq(
                connect_req->lap, 
                connect_req->uap, 
                connect_req->nap, 
                connect_req->server_chnl_num);
            break;        
        }            
    #endif /* FAX_CLIENT == XA_ENABLED */
    #if SPP_CLIENT == XA_ENABLED
        case MSG_ID_BT_SPP_SCO_CONNECT_REQ:
        {
            btmtk_spp_connect_req_struct *connect_req = 
                (btmtk_spp_connect_req_struct*) ilm_ptr->local_para_ptr;
            SPPSCOHandleConnectReq(
                connect_req->lap, 
                connect_req->uap, 
                connect_req->nap, 
                connect_req->server_chnl_num, 
                connect_req->uuid);
            break;        
        }            
        case MSG_ID_BT_SPP_CONNECT_REQ:
        {
            btmtk_spp_connect_req_struct *connect_req;
            SPPHandleActivateReq(SPP_CLIENT_PORT, NULL, 0);
            connect_req = (btmtk_spp_connect_req_struct*) ilm_ptr->local_para_ptr;
            SPPHandleConnectReq(
                connect_req->lap, 
                connect_req->uap, 
                connect_req->nap, 
                connect_req->server_chnl_num, 
                connect_req->uuid);
            break;        
        }
    #endif /* SPP_CLIENT == XA_ENABLED */

        case MSG_ID_BT_SPP_ACTIVATE_REQ:
            SPPHandleActivateReq(SPP_SERVER_PORT, 
            						((btmtk_spp_activate_req_struct*) (ilm_ptr->local_para_ptr))->svcName, 
            						((btmtk_spp_activate_req_struct*) (ilm_ptr->local_para_ptr))->svcUUID);
            break;
        
    #if DUN_SERVER == XA_ENABLED
        case MSG_ID_BT_DUN_ACTIVATE_REQ:
            DUNHandleActivateReq(SPP_SERVER_PORT);
            break;        
        case MSG_ID_BT_DUN_DEACTIVATE_REQ:
            DUNHandleDeactivateReq(((btmtk_dun_deactivate_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
    #endif /* DUN_SERVER == XA_ENABLED */
    #if FAX_SERVER == XA_ENABLED
        case MSG_ID_BT_FAX_ACTIVATE_REQ:
            FAXHandleActivateReq(SPP_SERVER_PORT);
            break;
        case MSG_ID_BT_FAX_DEACTIVATE_REQ:
            FAXHandleDeactivateReq(((btmtk_fax_deactivate_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
        case MSG_ID_BT_FAX_DISCONNECT_REQ:
            FAXHandleDisconnectReq(((btmtk_fax_disconnect_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
    #endif /* FAX_SERVER == XA_ENABLED */
        case MSG_ID_BT_SPP_SEND_DATA_REQ:
            SPPHandleSendDataReq(((btmtk_spp_send_data_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;        
        case MSG_ID_BT_SPP_GET_DATA_REQ:
            SPPHandleGetDataReq(((btmtk_spp_get_data_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
        case MSG_ID_BT_SPP_DEACTIVATE_REQ:
            SPPHandleDeactivateReq(((btmtk_spp_deactivate_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;        
        case MSG_ID_BT_SPP_DISCONNECT_REQ:
            SPPHandleDisconnectReq(((btmtk_spp_disconnect_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
    #if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED
        case MSG_ID_BT_DUN_DISCONNECT_REQ:
            DUNHandleDisconnectReq(((btmtk_dun_disconnect_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
    #endif /* DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED */
        case MSG_ID_BT_SPP_AUDIO_CONNECT_REQ:
            SPPHandleAudioConnectReq(((btmtk_spp_audio_connect_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;        
        case MSG_ID_BT_SPP_AUDIO_DISCONNECT_REQ:
            SPPHandleAudioDisconnectReq(((btmtk_spp_audio_disconnect_req_struct*) (ilm_ptr->local_para_ptr))->port);
            break;
	 case MSG_ID_BT_SPP_INITIALIZE_REQ:
	     SPPDRV_Init();
	     break;
#if 0	// Un-used
	 case MSG_ID_BT_SPP_REGISTER_CALLBACK_REQ:
	     SPP_Register(((btmtk_spp_register_callback_req_struct*) (ilm_ptr->local_para_ptr))->callback);
	     break;
#endif	     
	 case MSG_ID_BT_SPP_UART_PUT_BYTES_REQ:
	     SPPUartPutBytes(((btmtk_spp_uart_put_bytes_req_struct*) (ilm_ptr->local_para_ptr))->port,
		 							((btmtk_spp_uart_put_bytes_req_struct*) (ilm_ptr->local_para_ptr))->buffer,
		 							((btmtk_spp_uart_put_bytes_req_struct*) (ilm_ptr->local_para_ptr))->length,
		 							//((btmtk_spp_uart_put_bytes_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 							MOD_EXTERNAL
		 							);
	     break;
	 case MSG_ID_BT_SPP_UART_GET_BYTES_REQ:
        #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/        
	     SPPUartGetBytes(((btmtk_spp_uart_get_bytes_req_struct*) (ilm_ptr->local_para_ptr))->port,
		 							((btmtk_spp_uart_get_bytes_req_struct*) (ilm_ptr->local_para_ptr))->length,
		 							NULL,
		 							//((btmtk_spp_uart_get_bytes_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 							MOD_EXTERNAL
		 							);
        #endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
	     break;
	 case MSG_ID_BT_SPP_UART_OPEN_REQ:
	     SPPUartOpen(((btmtk_spp_uart_open_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 						//((btmtk_spp_uart_open_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 						MOD_EXTERNAL
		 						);
	     break;
	 case MSG_ID_BT_SPP_UART_CLOSE_REQ:
	     SPPUartClose(((btmtk_spp_uart_close_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 						//((btmtk_spp_uart_close_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 						MOD_EXTERNAL
		 						);
	     break;
	 case MSG_ID_BT_SPP_UART_GET_BYTES_AVAIL_REQ:
	     SPPUartGetBytesAvail(((btmtk_spp_uart_get_bytes_avail_req_struct*) (ilm_ptr->local_para_ptr))->port);
	     break;
	 case MSG_ID_BT_SPP_UART_GET_TX_ROOM_LEFT_REQ:
	     SPPUartGetTxRoomLeft(((btmtk_spp_uart_get_tx_room_left_req_struct*) (ilm_ptr->local_para_ptr))->port);
	     break;
	 case MSG_ID_BT_SPP_UART_SET_OWNER_REQ:
	     SPPUartSetOwner(((btmtk_spp_uart_set_owner_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 							 //((btmtk_spp_uart_set_owner_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 							 MOD_EXTERNAL
		 							 );
	     break;
	 case MSG_ID_BT_SPP_UART_CTRL_DCD_REQ:
	     SPPCtrlDCD(((btmtk_spp_uart_ctrl_dcd_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 						    ((btmtk_spp_uart_ctrl_dcd_req_struct*) (ilm_ptr->local_para_ptr))->sdcd, 
		 						    //((btmtk_spp_uart_ctrl_dcd_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 						    MOD_EXTERNAL
		 						    );
	     break;
	 case MSG_ID_BT_SPP_UART_CTRL_BREAK_REQ:
	     SPPUartCtrlBreak(((btmtk_spp_uart_ctrl_break_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 						    ((btmtk_spp_uart_ctrl_break_req_struct*) (ilm_ptr->local_para_ptr))->sbreak, 
		 						    //((btmtk_spp_uart_ctrl_break_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 						    MOD_EXTERNAL
		 						    );
	     break;
	 case MSG_ID_BT_SPP_UART_GET_OWNER_ID_REQ:
         #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) /*SPP_PORTING*/          
	     SPPUartGetOwnerID(((btmtk_spp_uart_get_owner_id_req_struct*) (ilm_ptr->local_para_ptr))->port);
	     break;
        #endif /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */            
	 case MSG_ID_BT_SPP_UART_CLR_RX_BUFFER_REQ:
	     SPPUartClrRxBuffer(((btmtk_spp_uart_clr_rx_buffer_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 							     //((btmtk_spp_uart_clr_rx_buffer_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 							     MOD_EXTERNAL
		 							     );
	     break;
	 case MSG_ID_BT_SPP_UART_CLR_TX_BUFFER_REQ:
	     SPPUartClrTxBuffer(((btmtk_spp_uart_clr_tx_buffer_req_struct*) (ilm_ptr->local_para_ptr))->port, 
		 							     //((btmtk_spp_uart_clr_tx_buffer_req_struct*) (ilm_ptr->local_para_ptr))->owner
		 							     MOD_EXTERNAL
		 							     );
	     break;
	 case MSG_ID_BT_SPP_ENABLE_REQ:
	     SPPEnableReq();
	     break;
	 case MSG_ID_BT_SPP_DISABLE_REQ:
	    SPPDisableReq();
	     break;
	 case MSG_ID_BT_SPP_UART_DATA_AVAILABLE_IND_RSP:
	 	SPPUartDataAvailableIndRsp(((btmtk_spp_uart_data_available_ind_rsp_struct*) (ilm_ptr->local_para_ptr))->port,
	 								((btmtk_spp_uart_data_available_ind_rsp_struct*) (ilm_ptr->local_para_ptr))->length);
	 	break;
#ifdef __SPP_SHARED_MEMORY__
	 case MSG_ID_BT_UART_ASSIGN_BUFFER_REQ:
	 	SPPAssignBufferReq(((btmtk_spp_uart_assign_buffer_req_struct*) (ilm_ptr->local_para_ptr))->port, 
	 						((btmtk_spp_uart_assign_buffer_req_struct*) (ilm_ptr->local_para_ptr))->getBufPtr, 
	 						((btmtk_spp_uart_assign_buffer_req_struct*) (ilm_ptr->local_para_ptr))->getBufSize);
	 	break;
#endif	/* __SPP_SHARED_MEMORY__ */
        default:
	     Report(("[SPP] Internal ADP: unsupported case."));
            break;

    }
}



