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
#include <stdlib.h>

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_opps_struct.h"
#include "bluetooth_oppc_struct.h"

#include "bt_opp_comm.h"
#include "bt_opp_porting.h"
#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_struct.h"
#include "bt_opps_struct.h"

#include "ext_fs.h"
#include "ext_ucs2.h"
#include "bt_goep_fs.h"

#include "bt_ext_debug.h"

#ifndef OPP_PI_ASSERT
#define OPP_PI_ASSERT(x)
#endif

#define MAX_DIGIT_SIZE	15

extern void btmtk_util_convert_bdaddr2string(char *dest, bt_addr_struct *source);
extern S32 ext_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size, const U8 *src);

void opp_client_pull_finish(U8 ok, U32 failreason);
void opp_client_push_finish( U8 ok, U32 failreason );
void opp_server_push_finish( U8 ok, U32 reason, U32 size );

BT_BOOL g_user_disc = FALSE; /// opp server user disconnect req
BT_BOOL g_oppc_user_disconnect = FALSE; ///opp client disconnect req
BT_BOOL g_oppc_user_abort = FALSE;
//BT_BOOL g_oppc_connect_has_rsp = FALSE; ///opp client has received the connect response
//BT_BOOL g_oppc_remote_disconnect = FALSE;

BTMTK_OPP_JNI_CALLBACK g_opp_jni_callback = NULL;

#define MBT_OPPC_INITLOCK()
#define MBT_OPPC_DEINITLOCK()
#define MBT_OPPC_LOCK()
#define MBT_OPPC_UNLOCK()
#define MBT_OPPS_INITLOCK()
#define MBT_OPPS_DEINITLOCK()
#define MBT_OPPS_LOCK()
#define MBT_OPPS_UNLOCK()

void mbt_opp_tool_memset(void *mem, char x, U32 size){
	memset(mem, x, size);
}

/********************************************************************************
 * pass event to JNI layer (then to MMI)
 ********************************************************************************/

/**
 * register callback function ( jni should use it to register callback and handle ui event )
 */
BT_BOOL btmtk_opp_set_jni_callback( BTMTK_OPP_JNI_CALLBACK pCallback )
{
	g_opp_jni_callback = pCallback;
	return TRUE;
}

/**
 * post event to JNI
 * TODO change parameters to S* parameters[] and cancel "count"
 */
void opp_postevent(U8 event, S8* parameters[], U8 count)
{
	if( NULL !=  g_opp_jni_callback )
	{
		g_opp_jni_callback(event, parameters, count);
	}
}

/*********************** OPP Client *********************/

/**
 * return: ENABLE_SUCCESS / ENABLE_FAIL
 */
void opp_client_enable_result( U8 ok )
{
	BT_OPP_LOGD("[EXT][opp_client_enable_result][+]: %u", ok);

	opp_postevent((TRUE == ok) ? BT_OPPC_ENABLE_SUCCESS : BT_OPPC_ENABLE_FAIL, NULL, 0);
}

/**
 * return: DISABLE_SUCCESS / DISABLE_FAIL
 */
void opp_client_disable_result( U8 ok )
{
	BT_OPP_LOGD("[EXT][opp_client_disable_result][+]: %u", ok);

	opp_postevent((TRUE == ok) ? BT_OPPC_DISABLE_SUCCESS : BT_OPPC_DISABLE_FAIL, NULL, 0);
}

/**
 * return: DISABLE_SUCCESS / DISABLE_FAIL
 */
void opp_client_connect_result( U8 ok )
{
    BT_OPP_LOGD("[EXT][opp_client_connect_result][+]: %u", ok);
    opp_postevent((TRUE == ok) ? BT_OPPC_CONNECTED : BT_OPPC_DISCONNECT, NULL, 0);
}

/**
 * return: PUSH_START + totalbytes
 */
void opp_client_push_start(U8 ok, U32 failreason, U32 totalbytes){

	S8* para[1];
	S8 tbs[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_push_start][+]: %u, %u", ok, totalbytes);

	// post start event to mmi
	sprintf(tbs, "%lu", totalbytes);
	para[0] = tbs;
	opp_postevent(BT_OPPC_PUSH_START, para, 1);

	// handle fail condition
	if( FALSE == ok )
	{
	    BT_OPP_LOGI("[EXT][INT][opp_client_push_start] - fail: reason[%lu]", failreason);
		opp_client_push_finish(FALSE, failreason);
	}
}

/**
 * return:	PROGRESSIVE_UPDATE,
 * 			PUSH_FAIL: sentbytes / totalbytes
 */
void opp_client_push_continue( U8 ok, U32 sentbytes, U32 totalbytes )
{
	S8* para[2];
	S8 para1[MAX_DIGIT_SIZE];
	S8 para2[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_push_continue][+]: %u %u %u", ok, sentbytes, totalbytes);

	if( TRUE == ok )
	{
		sprintf(para1, "%lu", sentbytes);
		sprintf(para2, "%lu", totalbytes);
		para[0] = para1;
		para[1] = para2;
		opp_postevent(BT_OPPC_PROGRESS_UPDATE, para, 2);
	}
	else
	{
	    // TODO reason code from caller
	    BT_OPP_LOGI("[EXT][INT][opp_client_push_continue] - fail");
		opp_client_push_finish(FALSE, GOEP_STATUS_FAILED);
	}
}

/**
 * return:	PUSH_SUCCESS
 * 			PUSH_FAIL + GOEP status code
 */
void opp_client_push_finish( U8 ok, U32 failreason ){

	S8* para[1];
	S8 para1[4] = {0};  // para1 size = 3 is not safe to use sprintf 

	BT_OPP_LOGD("[EXT][opp_client_push_finish][+]: %u, %u", ok, failreason);

	if( TRUE == ok )
	{
		opp_postevent(BT_OPPC_PUSH_SUCCESS , NULL, 0);
	}
	else
	{
	    BT_OPP_LOGI("[EXT][INT][opp_client_push_finish] - fail: reason[%lu]", failreason);

		sprintf(para1, "%lu", failreason);
		para[0] = para1;
		opp_postevent(BT_OPPC_PUSH_FAIL, para, 1);
	}

//	g_oppc_connect_has_rsp = TRUE;
}

/**
 * return: PULL_START
 */
void opp_client_pull_start( U8 ok )
{
	BT_OPP_LOGD("[EXT][opp_client_pull_start][+]: %u", ok);

	// post start event to mmi
	opp_postevent(BT_OPPC_PULL_START, NULL, 0);

	// handle fail condition
	if( FALSE == ok )
	{
		opp_client_pull_finish(FALSE, GOEP_STATUS_FAILED);
	}
}

/**
 * return:	PROGRESSIVE_UPDATE + sentbytes
 * 			PULL_FAIL + / failReason
 */
void opp_client_pull_continue( U8 ok, U32 sentbytes )
{
	S8* para[1];
	S8 para1[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_pull_continue][+]: %u %u", ok, sentbytes);

	if( TRUE == ok )
	{
		sprintf(para1, "%lu", sentbytes);
		para[0] = para1;
		opp_postevent(BT_OPPC_PROGRESS_UPDATE, para, 1);
	}
	else
	{
		sprintf(para1, "%u", GOEP_DATABASE_FULL);
		para[0] = para1;
		opp_postevent(BT_OPPC_PULL_FAIL, para, 1);
	}
}

/**
 * return:	PULL_SUCCESS
 * 			PULL_FAIL + failreason (goep status code)
 */
void opp_client_pull_finish( U8 ok, U32 failreason )
{
	S8* para[1];
	S8 para1[4] = {0};

	BT_OPP_LOGD("[EXT][opp_client_pull_finish][+]: %u, %u", ok, failreason);

	if( TRUE == ok )
	{
		opp_postevent(BT_OPPC_PULL_SUCCESS , NULL, 0);
	}
	else
	{
		sprintf(para1, "%lu", failreason);
		para[0] = para1;
		opp_postevent(BT_OPPC_PULL_FAIL, para, 1);
	}

//	g_oppc_connect_has_rsp = TRUE;
}

/**
 * return: EXCH_START
 */
void opp_client_exchange_start( U8 ok )
{
	BT_OPP_LOGD("[EXT][opp_client_exchange_start][+]: %u", ok);

	opp_postevent(BT_OPPC_EXCH_START, NULL, 0);
}

/**
 * return: PROGRESS_UPDATE + sentbytes
 */
void opp_client_exchange_push_continue( U8 ok, U32 sentbytes )
{
	S8* para[1];
	S8 para1[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_exchange_push_continue][+]: %u %u", ok, sentbytes);

	// no 0 value PROGRESS_UPDATE
	if( 0 != sentbytes )
	{
		sprintf(para1, "%lu", sentbytes);
		para[0] = para1;
		opp_postevent(BT_OPPC_PROGRESS_UPDATE, para, 1);
    }

	// no failure case
}

/**
 * return: failreason ( TODO no postevent ? need to verfiy with MMI )
 */
void opp_client_exchange_push_finish( U8 ok, U32 sentbytes, U32 failreason )
{
	S8* para[2];
	S8 para1[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_exchange_push_finish][+]: %u %u %u", ok, sentbytes, failreason);

	if( TRUE != ok )
	{
		sprintf(para1, "%lu", failreason);
		para[0] = para1;
		// no post event ?
		// opp_postevent(?, para, 1);
	}

	// pull is not affected by push result
	// post start pulling / finish push event
	// opp_postevent(?, para, 1);
}

/**
 * return: PROGRESS_UPDATE + sentBytes
 */
void opp_client_exchange_pull_continue( U8 ok, U32 sentbytes )
{
	S8* para[1];
	S8 para1[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_exchange_pull_continue][+]: %u %u", ok, sentbytes);

	// no 0 value PROGRESS_UPDATE
	if( 0 != sentbytes )
	{
		sprintf(para1, "%lu", sentbytes);
		para[0] = para1;
		opp_postevent(BT_OPPC_PROGRESS_UPDATE, para, 1);
    }
}

/**
 * return: 	EXCH_SUCCESS
 * 			EXCH_FAIL + failreason
 */
void opp_client_exchange_pull_finish( U8 ok, U32 sentbytes, U32 failreason )
{
	S8* para[1];
	S8 para1[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[EXT][opp_client_exchange_pull_finish][+]: %u, %u, %u", ok, sentbytes, failreason);

	if( TRUE == ok )
	{
		opp_postevent(BT_OPPC_EXCH_SUCCESS, NULL, 0);
	}
	else
	{
		// TODO how to unify push_finish / pull_finish / exch_finish ( 3 finish event to mmi ? )
		sprintf(para1, "%lu", failreason);
		para[0] = para1;
		opp_postevent(BT_OPPC_EXCH_FAIL, para, 1);
	}
}

/**
 * return:	EXCH_SUCCESS
 * 			EXCH_FAIL + failreason
 */
void opp_client_exchange_finish( U8 ok, U8 failreason )
{
	S8* para[1];
	S8 para1[4] = {0};

	BT_OPP_LOGD("[EXT][opp_client_exchange_finish][+]: %u, %u", ok, failreason);

	if( TRUE == ok )
	{
		opp_postevent(BT_OPPC_EXCH_SUCCESS, NULL, 0);
	}
	else
	{
		sprintf(para1, "%u", failreason);
		para[0] = para1;
		opp_postevent(BT_OPPC_EXCH_FAIL, para, 1);
	}

//	g_oppc_connect_has_rsp = TRUE;
}

/**
 * @brief Set the disconnect sdc
 *
 * return: CANCEL / DISCONNECT / PUSH_FAIL / PULL_FAIL / EXCH_FAIL
 */
void opp_client_disconnect_result(U8 user_disc, U32 failreason)
{
	BT_OPP_LOGD("[EXT][opp_client_disconnect_result][+]: %u %u", user_disc, failreason);
	opp_postevent(BT_OPPC_DISCONNECT, NULL, 0);
}

/*********************** OPP Server *********************/

/**
 * return: ENABLE_SUCCESS / ENABLE_FAIL
 */
void opp_server_enable_result(U8 ok)
{
	BT_OPP_LOGD("[EXT][opp_server_enable_result][+]: [%u]", ok);
	opp_postevent((TRUE == ok) ? BT_OPPS_ENABLE_SUCCESS : BT_OPPS_ENABLE_FAIL, NULL, 0);
}

/**
 * return: DISABLE_SUCCESS / DISABLE_FAIL
 */
void opp_server_disable_result(U8 ok)
{
	BT_OPP_LOGD("[EXT][opp_server_disable_result][+]: [%u]", ok);
	opp_postevent((TRUE == ok) ? BT_OPPS_DISABLE_SUCCESS : BT_OPPS_DISABLE_FAIL, NULL, 0);
    g_opp_jni_callback = NULL;
}

/**
 * return: ACCESS_REQUEST, bdaddr / filename / mime / size
 */
void opp_server_push_access_request(btbm_bd_addr_t *BDAddr, U8 *ucUtfObjname, U8 *format, U32 u4Size)
{
	S8* para[4];
	S8 para0[18];	// bdaddr
	S8 para3[MAX_DIGIT_SIZE];	// size

	BT_OPP_LOGD("[EXT][opp_server_push_access_request][+]: [%s][%s][%u]", ucUtfObjname, format, u4Size);

	// convert parameters
	btmtk_util_convert_bdaddr2string(para0, BDAddr);
	sprintf(para3, "%lu", u4Size);

	para[0] = para0;				// bdaddr
	para[1] = (S8*)ucUtfObjname;	// object-name
	para[2] = (S8*)format;			// format
	para[3] = para3;				// size

	opp_postevent(BT_OPPS_PUSH_ACCESS_REQUEST, para, 4);
}

/**
 * return: ACCESS_REQUEST, bdaddr / filename / mimetype
 */
void opp_server_pull_access_request(btbm_bd_addr_t *BDAddr, U8 *ucUtfObjname, U8 *format)
{
	S8* para[3];
	S8 para0[18];	// bdaddr

	BT_OPP_LOGD("[EXT][opp_server_pull_access_request][+]: [%s][%s]", ucUtfObjname, format);

	// convert parameters
	btmtk_util_convert_bdaddr2string(para0, BDAddr);

	para[0] = para0;				// bdaddr
	para[1] = (S8*)ucUtfObjname;	// object-name (should be null)
	para[2] = (S8*)format;			// format (should be vCard)

	opp_postevent(BT_OPPS_PULL_ACCESS_REQUEST, para, 3);
}

/**
 * return: PUSH_START
 */
void opp_server_push_start( U8 ok, U32 reason )
{
	BT_OPP_LOGD("[EXT][opp_server_push_start][+]: [%u][%u])", ok, reason);

	opp_postevent(BT_OPPS_PUSH_START, NULL, 0);

	if( TRUE != ok )
	{
	    BT_OPP_LOGI("[EXT][opp_server_push_start] - fail: reason[%lu])", reason);
		opp_server_push_finish(FALSE, reason, 0);
	}
}

/**
 * return: PROGRESSIVE_UPDATE, sentSize
 */
void opp_server_push_continue( U8 ok, U32 i4SendSize )
{
	S8* para[1];
	S8 para0[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[BTMTK][OPPS] opp_server_push_continue(status[%u], size[%u])", ok, i4SendSize);

	if( TRUE == ok )
	{
		// No 0 value PROGRESS_UPDATE
		if( 0 != i4SendSize )
		{
			sprintf(para0, "%lu", i4SendSize);
			para[0] = para0;
			opp_postevent(BT_OPPS_PROGRESS_UPDATE, para, 1);
		}
	}
	else
	{
		// push fail is handled in push_finished
	}
}

/**
 * return: PUSH_SUCCESS / PUSH_FAIL, size / failreason
 */
void opp_server_push_finish( U8 ok, U32 reason, U32 size )
{
	S8* para[1];
	S8 para0[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[BTMTK][OPPS] opp_server_push_finish(status[%u], reason[%u], size[%u])", ok, reason, size);

	if( TRUE == ok )
	{
		sprintf(para0, "%lu", size);
		para[0] = para0;
		opp_postevent(BT_OPPS_PUSH_SUCCESS, para, 1);
	}
	else
	{
		if( FALSE == g_user_disc )
		{
			sprintf(para0, "%lu", reason);
			para[0] = para0;
			opp_postevent(BT_OPPS_PUSH_FAIL, para, 1);
		}
		else
		{
			// wait for disconnect ind to handle
		}
	}
}

/**
 * return: PULL_START, totalSize
 */
void opp_server_pull_start( U8 ok, U32 reason, U32 total )
{
	S8* para[1];
	S8 para0[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[BTMTK][OPPS] opp_server_pull_start(status[%u], reason[%u], total[%u])", ok, reason, total);

	sprintf(para0, "%lu", total);
	para[0] = para0;
	opp_postevent(BT_OPPS_PULL_START, para, 1);

	if( TRUE == ok )
	{
		//sdcOppStatus->TxProgress = 0;
		//sdcOppStatus->OppState = MBT_OPP_STATE_SENDING;
		//sdcOppStatus->SendingFile.ObjectSize = total;
		////sdcOppStatus->SendingFile.AcceptableFileSize = 0xFFFFFFFF; // field by AP
	}
	else
	{
		opp_server_push_finish(FALSE, reason, 0);
	}
}

/**
 * return: PROGRESSIVE_UPDATE / PUSH_FAIL, sentSize / totalSize
 */
void opp_server_pull_continue( U8 ok, U32 i4SendSize, U32 i4Total )
{
	S8* para[1];
	S8 para0[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[BTMTK][OPPS] opp_server_pull_continue(status[%u], send[%u], total[%u])", ok, i4SendSize, i4Total);

	if( TRUE == ok )
	{
		if( i4SendSize != 0 )
		{
			sprintf(para0, "%lu", i4SendSize);
			para[0] = para0;
			opp_postevent(BT_OPPS_PROGRESS_UPDATE, para, 1);
		}
	}
	else
	{
		sprintf(para0, "%u", 0);	// TODO reason_code
		para[0] = para0;
		opp_postevent(BT_OPPS_PULL_FAIL, para, 1);
	}
}

/**
 * return: PULL_SUCCESS / PULL_FAIL, failreason
 */
void opp_server_pull_finish( U8 ok, U32 reason )
{
	S8* para[1];
	S8 para0[MAX_DIGIT_SIZE];

	BT_OPP_LOGD("[BTMTK][OPPS] opp_server_pull_finish(status[%u], reason[%u])", ok, reason);

	if( TRUE == ok )
	{
		opp_postevent(BT_OPPS_PULL_SUCCESS , NULL, 0);
	}
	else
	{
		if( FALSE == g_user_disc )
		{
			sprintf(para0, "%lu", reason);
			para[0] = para0;
			opp_postevent(BT_OPPS_PULL_FAIL, para, 1);
		}
		else
		{
			// wait for disconnect ind
		}
	}
}

/**
 * @brief Send the disconnect sdc
 *
 * return: DISCONNECT / CANCEL,
 */
void opp_server_disconnect_result(U8 ok, U8 abort)
{
	BT_OPP_LOGD("[EXT][INT][opp_server_disconnect_result][+]: ok[%u], abort[%u]", ok, abort);
	opp_postevent(BT_OPPS_DISCONNECT, NULL, 0);
}

void my_oppc_ui_cb(U32 u4OPPUIEvent, U8* ucdata){

	static int g_oppc_push_start = FALSE;
	static int g_oppc_pull_start = FALSE;
	static int g_oppc_exchange_start = FALSE;
	static int g_connected_record = FALSE;
	//static int g_connected_done_record = FALSE;
	static int success;

	bt_oppc_status_struct c;
	c.size = sizeof(bt_oppc_status_struct);

	MBT_OPPC_LOCK();

	switch(u4OPPUIEvent){
		case BT_OPPCUI_CONNECTED:
			g_connected_record = TRUE;
			//g_connected_done_record = FALSE; // not send finish yet
			success = 0;
			opp_client_connect_result( TRUE );
			//btmtk_oppc_get_status(&c);
			// mbt_tools_bmaddr_to_MBTAddr(&bdaddr, &c.remote_dev_addr); // struct to byte array
			// btmtk_gap_get_device_name(bdaddr, sdcOppStatus->RemoteDevName, sizeof(sdcOppStatus->RemoteDevName) );
			break;
		case BT_OPPCUI_PUSHED:
			success += (int)ucdata;
			btmtk_oppc_get_status(&c);
			//sdcOppStatus->TxProgress = c.totalsize;
			if( BT_OPP_PUSH_FEATURE == c.feature || BT_OPP_READY_FEATURE == c.feature )
			{
				if( 0 == (U32)ucdata )
				{
					opp_client_push_finish(TRUE, 0);
					//g_connected_done_record = TRUE;
				}
				else
				{
					if( GOEP_STATUS_DISCONNECT != (U32)ucdata ){ // fail because disconnect
						//if( g_connected_done_record ){
						//	/// has report the MBT cancel
						//}
						//else
						//{
						    BT_OPP_LOGI("[EXT][INT][my_oppc_ui_cb] - fail: event[BT_OPPCUI_PUSHED], reason[%lu]", (U32)ucdata);
							opp_client_push_finish(FALSE, (U32)ucdata);
						//	g_connected_done_record = TRUE;
						//}
					}
					else
					{
						// wait for post mbt disconnect event
						//g_oppc_remote_disconnect = TRUE;
					}
				}
			}
			else if( BT_OPP_EXCHANGE_FEATURE == c.feature )
			{
				opp_client_exchange_push_finish(TRUE, c.totalsize, (U32)ucdata);
			}
			g_oppc_push_start = FALSE;
			break;
		case BT_OPPCUI_PUSHING: // receiving a push data's cnf from remote device
			btmtk_oppc_get_status(&c);
			if( BT_OPP_PUSH_FEATURE == c.feature || BT_OPP_READY_FEATURE == c.feature )
			{
				if( TRUE == g_oppc_push_start )
				{
					if( 0 != (U32)ucdata ) // cannot send the second 0 value
					{
						opp_client_push_continue(TRUE, (U32)ucdata, c.totalsize);
					}
				}
				else
				{
					g_oppc_push_start = TRUE;
					opp_client_push_start(TRUE, 0, c.totalsize);
				}
			}
			else if( BT_OPP_EXCHANGE_FEATURE == c.feature )
			{
				if( FALSE != g_oppc_push_start )
				{
					if( 0 != (U32)ucdata ) // cannot send the second 0 value
					{
						opp_client_exchange_push_continue(TRUE, (U32)ucdata); // update the push status
					}
				}
				else
				{
					g_oppc_push_start = TRUE;
					opp_client_exchange_start(TRUE);
					opp_client_exchange_push_continue(TRUE, (U32)ucdata);
				}
			}
			break;
		case BT_OPPCUI_PULLED:
			btmtk_oppc_get_status(&c);
			success += (int) ucdata;
			//sdcOppStatus->RxProgress = c.totalsize;
			if( BT_OPP_PULL_FEATURE == c.feature )
			{
				// success
				if( 0 == (U32)ucdata )
				{
					// check memory-method
					//if( MBT_MEMORY_BUFFER == sdcOppStatus->ReceivingFile.StorageType )
					//{
					//	if( c.totalsize > sdcOppStatus->ReceivingFile.AllocBufferSize )
					//	{
					//		sdcOppStatus->ReceivingFile.StorageType = MBT_INTERNAL_FS;
					//		BT_OPP_LOGD("[BTMTK][OPPC] Update  ReceivingFile.StorageType:%d", sdcOppStatus->ReceivingFile.StorageType);
					//	}
					//}
					opp_client_pull_finish(TRUE, (U32)ucdata);
					//g_connected_done_record = TRUE;
				}
				// fail
				else
				{
					if( GOEP_STATUS_DISCONNECT != (U32)ucdata ){
						//if( g_connected_done_record ){
						//	/// has report the MBT cancel
						//}
						//else
						//{
							opp_client_pull_finish(FALSE, (U32)ucdata);
						//	g_connected_done_record = TRUE;
						//}
					}
					else
					{
						// wait for post mbt disconnect event
						//g_oppc_remote_disconnect = TRUE;
					}
				}
			}
			else if( BT_OPP_EXCHANGE_FEATURE == c.feature )
			{
				if( 0 == (U32) ucdata ) // pulled result is Success(0)
				{
					// check memory-method
					//if( MBT_MEMORY_BUFFER == sdcOppStatus->ReceivingFile.StorageType )
					//{
					//	if( c.totalsize > sdcOppStatus->ReceivingFile.AllocBufferSize )
					//	{
					//		sdcOppStatus->ReceivingFile.StorageType = MBT_INTERNAL_FS;
					//		BT_OPP_LOGD("[BTMTK][OPPC] Update  ReceivingFile.StorageType:%d", sdcOppStatus->ReceivingFile.StorageType);
					//	}
					//}
					opp_client_exchange_pull_finish(TRUE, c.totalsize, (U32)ucdata);
				}
				else
				{
					//if( c.totalsize > sdcOppStatus->ReceivingFile.AcceptableFileSize )
					//{
					//	sdcOppStatus->FailReason = MBT_DATABASE_FULL_RES;
					//	BT_OPP_LOGD("[BTMTK][OPPC] Update FailReason:%d", sdcOppStatus->FailReason);
					//}
					opp_client_exchange_pull_finish(FALSE, c.totalsize, (U32)ucdata);
				}
			}
			g_oppc_pull_start = FALSE;
			//g_connected_done_record = TRUE;
			break;
		case BT_OPPCUI_PULLING: // receiving a pull data's cnf from remote device
			btmtk_oppc_get_status(&c);
			if( BT_OPP_PULL_FEATURE ==  c.feature )
			{
				if( FALSE != g_oppc_pull_start)
				{
					if( 0 != (U32)ucdata ){ // cannot send the second 0 value
						opp_client_pull_continue(TRUE, (U32)ucdata );
					}
				}
				else
				{
					g_oppc_pull_start = TRUE;
					opp_client_pull_start(TRUE);
				}
			}
			else if( BT_OPP_EXCHANGE_FEATURE ==  c.feature )
			{
				if( 0 == (U32)ucdata && TRUE == g_oppc_pull_start )
				{
					// forbidden to send twice 0 value
				}
				else
				{
					opp_client_exchange_pull_continue(TRUE, (U32)ucdata);
					g_oppc_pull_start = TRUE;
				}
			}
			break;
		case BT_OPPCUI_ABORTING:
			g_oppc_user_abort = TRUE;
			break;
		case BT_OPPCUI_DISCONNECTED:
			/*
			btmtk_oppc_get_status(&c);
			if( FALSE == g_connected_record )
			{
				// cannot connect with remote device
				switch( c.feature  ){
					case BT_OPP_PULL_FEATURE:
						if( FALSE == g_oppc_user_disconnect )
						{
							//sdcOppStatus->FailReason = MBT_ERROR_RES;
							//BT_OPP_LOGD("[BTMTK][OPPC] Update FailReason:%d", sdcOppStatus->FailReason);
							opp_postevent(BT_OPPC_PULL_FAIL, 1);
							//g_connected_done_record = TRUE;
						}
						else
						{
							//sdcOppStatus->OppState = MBT_OPP_STATE_IDLE;
							//BT_OPP_LOGD("[BTMTK][OPPC] Update OppState:%d", sdcOppStatus->OppState);
							opp_postevent(BT_OPPC_DISCONNECT , 0); //not in pushing state
						}
						break;
					case BT_OPP_PUSH_FEATURE:
						if( FALSE == g_oppc_user_disconnect )
						{
							opp_client_push_finish(FALSE, (U32)ucdata);
							//g_connected_done_record = TRUE;
						}
						else
						{
							//sdcOppStatus->OppState = MBT_OPP_STATE_IDLE;
							//BT_OPP_LOGD("[BTMTK][OPPC] Update OppState:%d", sdcOppStatus->OppState);
							opp_postevent(BT_OPPC_DISCONNECT , 0); // not in pushing state
						}
						break;
					case BT_OPP_EXCHANGE_FEATURE:
						if( FALSE == g_oppc_user_disconnect )
						{
							//sdcOppStatus->OppState = MBT_OPP_STATE_IDLE;
							//BT_OPP_LOGD("[BTMTK][OPPC] Update OppState:%d", sdcOppStatus->OppState);
							opp_postevent(BT_OPPC_EXCH_FAIL , 0);
						}
						else
						{
							//sdcOppStatus->OppState = MBT_OPP_STATE_IDLE;
							//BT_OPP_LOGD("[BTMTK][OPPC] Update OppState:%d", sdcOppStatus->OppState);
							opp_postevent(BT_OPPC_DISCONNECT , 0); //not in pushing state
						}
						//g_connected_done_record = TRUE;
						break;
				}
			}
			else	// g_connected_record == TRUE
			{
				// has connection with remote device
				BT_OPP_LOGD("[BTMTK][OPPC] Client disconnect connected:%d connected_done:%d user_dis:%d ", g_connected_record, g_connected_done_record, g_oppc_user_disconnect);

				// connected but disconnect before accept
				if( MBT_OPP_STATE_IDLE == sdcOppStatus->OppState && g_connected_done_record == FALSE )
				{
					if( g_oppc_push_start == FALSE && g_oppc_push_start == FALSE ){
						sdcOppStatus->OppState = MBT_OPP_STATE_IDLE;
						BT_OPP_LOGD("[BTMTK][OPPC] Update OppState:%d", sdcOppStatus->OppState);
						opp_postevent(BT_OPPC_DISCONNECT, 0);
					}
				}

				// move all cases to disconnect_result to handle
				opp_client_disconnect_result(FALSE, MBT_ALLOW_RES);
			}

			// all cases cause the sdc status to disconnect
			if( MBT_OPP_STATE_IDLE != sdcOppStatus->OppState ){
				//sdcOppStatus->OppState = MBT_OPP_STATE_IDLE;
				//BT_OPP_LOGD("[BTMTK][OPPC] Update OppState:%d", sdcOppStatus->OppState);
				opp_postevent(BT_OPPC_DISCONNECT, 0);
			}
			*/
			// let ui handle disconnect process
			opp_client_disconnect_result((U8)g_oppc_user_disconnect, 0);

            g_oppc_user_disconnect = FALSE;
            g_oppc_user_abort = FALSE;
			//g_oppc_remote_disconnect = FALSE;
			//g_connected_done_record = FALSE;
			g_oppc_pull_start = FALSE;
			g_oppc_push_start = FALSE;
			g_oppc_exchange_start = FALSE;
			g_connected_record = FALSE;
			break;
		default:
			BT_OPP_LOGI("[EXT][INT][my_oppc_ui_cb]: unsupported event[%lu]", u4OPPUIEvent);
			break;
	}

	MBT_OPPC_UNLOCK();
}

/*
U8 opp_server_check_trust_device(T_MBT_BDADDR RemoteBDAddr){
	U8 i;
	T_MBT_PAIRED_DEV_LIST *btPairedList;

	// Workaround: this should be done at MMI (SI layer)
	btPairedList = (T_MBT_PAIRED_DEV_LIST *) mbt_sdc_getrecord(MBTSDC_REC_GAP_PAIREDLIST );
	for(i = 0; i < MBT_MAXNUM_PAIRED_DEV && i  < btPairedList->PairedCount ; i++){
		if( 0 == memcmp( (void *) btPairedList->PairedList[i].BdAddr , (void *) RemoteBDAddr, sizeof(RemoteBDAddr)) ){
			if( TRUE == btPairedList->PairedList[i].bAuthorized ){
				MBT_LOG("[BTMTK][OPPS] opp_server_check_trust_device found and trust! send authreq" );
				return TRUE;
			}else{
				MBT_LOG("[BTMTK][OPPS] opp_server_check_trust_device found and untrust! send authreq");
				return MBT_FALSE;
			}
		}
	}

	MBT_LOG("[BTMTK][OPPS] opp_server_check_trust_device not found ! send authreq");
	return MBT_FALSE;
}
*/

void my_opps_ui_cb(U32 u4OPPUIEvent, U8* ucdata){

	static int g_opps_push_start = FALSE;
	static int g_opps_pull_start = FALSE;

	bt_opps_status_struct s;
	U8 utf8Objname[BTMTK_FS_MAX_FILEPATH];

	BT_OPP_LOGD( "[BTMTK][OPPS] my_opps_ui_cb(%s)", btmtk_opps_get_event_name(u4OPPUIEvent));

	s.size = sizeof(bt_opps_status_struct);

	MBT_OPPS_LOCK();

	switch(u4OPPUIEvent){
		case BT_OPPSUI_SHUTDOWNED:
			break;
		case BT_OPPSUI_AUTHORIZING:
			bt_ext_err("[BTMTK][OPPS] Unsupported event: OBEX Authorization Indication");
			//btmtk_opps_get_status(&s);
			//mbt_tools_bmaddr_to_MBTAddr(&bdaddr, &s.remote_dev_addr); // struct to byte array
			//memset( utf8Objname, 0, sizeof( utf8Objname ));
			//btmtk_gap_get_device_name(bdaddr, utf8Objname, sizeof(utf8Objname) );
            //BT_OPP_LOGD("[BTMTK][OPPS] Update device_name:'%s'", utf8Objname);
			//opp_server_check_trust_device(bdaddr);
			//opp_server_authorize_request(bdaddr, utf8Objname );
			break;
		case BT_OPPSUI_CONNECTED:
			// MMI will get device-name by bdaddr: BluetoothAdapter.getRemoteDevice(bdaddr)
			// update the RemoteDevName ( get device-name from GAP using bdaddr )
			//btmtk_opps_get_status(&s);
			//mbt_tools_bmaddr_to_MBTAddr(&bdaddr, &s.remote_dev_addr); // struct to byte array
			//sdcOppStatus = (T_MBT_OPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_OPP_STATUS );
			//btmtk_gap_get_device_name(bdaddr, sdcOppStatus->RemoteDevName, sizeof(sdcOppStatus->RemoteDevName) );
			//BT_OPP_LOGD("[BTMTK][OPPS] Connected, device_name:[%s]", sdcOppStatus->RemoteDevName);
            break;
		case BT_OPPSUI_PUSH_START:
			btmtk_opps_get_status(&s);
			BT_OPP_LOGD("[BTMTK][OPPS] push start: name[%s], format:[%s], size:[%u]", s.obj_name, s.obj_mime, s.totalsize);
			ext_chset_ucs2_to_utf8_string( (U8 *)utf8Objname, sizeof(utf8Objname), (const U8 *)s.obj_name );
			opp_server_push_access_request(&s.remote_dev_addr, utf8Objname, (U8 *)s.obj_mime, s.totalsize);
			break;
		case BT_OPPSUI_PUSHING:
			// update the pulling status
			btmtk_opps_get_status(&s);
			if( TRUE == g_opps_push_start )
			{
				//BT_OPP_LOGD("[EXT][my_opps_ui_cb] send continue");
				// send continue
				opp_server_push_continue(TRUE, (s.totalsize - s.remainsize) );
			}
			else
			{
				//BT_OPP_LOGD("[EXT][my_opps_ui_cb] send start");
				// send start
				g_opps_push_start = TRUE;
				opp_server_push_start(TRUE, GOEP_STATUS_SUCCESS);	// MBT_OBEX_RES_OK
			}
			break;
		case BT_OPPSUI_PUSHED:
			g_opps_push_start = FALSE;
			btmtk_opps_get_status(&s);
			// push success
			if( GOEP_STATUS_SUCCESS == (U32)ucdata )
			{
				opp_server_push_continue(TRUE, s.totalsize);
				opp_server_push_finish(TRUE, GOEP_STATUS_SUCCESS, s.totalsize);	// MBT_OBEX_RES_OK
			}
			// push fail
			else
			{
				// TODO keep acceptable size (at response) for finaish check
				//if( s.totalsize > sdcOppStatus->ReceivingFile.AcceptableFileSize){
				//	u4MbtErr = MBT_DATABASE_FULL_RES;
				//}
				opp_server_push_finish(FALSE, (U32)ucdata, 0);
			}
			break;
		case BT_OPPSUI_PULL_START:
			btmtk_opps_get_status(&s);
			BT_OPP_LOGD("[BTMTK][OPPS] pull start: name[%s], format:[%s]", s.obj_name, s.obj_mime);
			ext_chset_ucs2_to_utf8_string( (U8 *)utf8Objname, sizeof(utf8Objname), (const U8 *) s.obj_name );
			opp_server_pull_access_request(&s.remote_dev_addr, (U8 *)utf8Objname, (U8 *)s.obj_mime);
			break;
		case BT_OPPSUI_PULLING:
			btmtk_opps_get_status(&s);
			if( TRUE == g_opps_pull_start )
			{
				// send continue
				opp_server_pull_continue(TRUE, (s.totalsize - s.remainsize) , s.totalsize);
			}
			else
			{
				// send start
				g_opps_pull_start = TRUE;
				opp_server_pull_start(TRUE, 0, s.totalsize);
			}
			break;
		case BT_OPPSUI_PULLED:
			g_opps_pull_start = FALSE;
			btmtk_opps_get_status(&s);
			// pull success
			if( GOEP_STATUS_SUCCESS == (U32)ucdata )
			{
				opp_server_pull_continue(TRUE, (s.totalsize - s.remainsize) , s.totalsize);
				opp_server_pull_finish(TRUE, GOEP_STATUS_SUCCESS); // MBT_OBEX_RES_OK
			}
			else
			{
				if( GOEP_USER_CANCEL != (U32)ucdata ){
					opp_server_pull_finish(FALSE, (U32)ucdata);
				}
			}
			break;
		case BT_OPPSUI_DISCONNECTING:
			break;
		case BT_OPPSUI_DISCONNECTED:
			// notify the disconnect
			if( GOEP_STATUS_NO_CONNECT != (U32) ucdata ){
				opp_server_disconnect_result(TRUE, g_user_disc);
			}
			g_user_disc = FALSE;
			break;
		case BT_OPPSUI_ABORTING:
			break;
		case BT_OPPSUI_SHUTDOWNING:
			break;
		case BT_OPPSUI_ERROR:
			break;
		case BT_OPPSUI_MAX:
			break;
		default:
			BT_OPP_LOGD("[BTMTK][OPPS] WARN: my_opps_ui_cb => no handle event: %u", u4OPPUIEvent);
		break;
	}
	MBT_OPPS_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_opp_server_enable()
{
	BT_OPP_LOGD("[OPPS] btmtk_opp_server_enable()2");

	MBT_OPPS_INITLOCK();

	// disable first for abnormal restart
	// no GOPS(ops_goep_conn_id) and can't perform deregister server or it may free other service
	//bt_opps_obex_deregister_server_req();

	// invoke the opps enable
	//if( 0 < count && TRUE == btmtk_opps_activate_server() ){
	if( TRUE == btmtk_opps_activate_server() )
	{
		btmtk_opps_set_callback( my_opps_ui_cb ) ;
		opp_server_enable_result(TRUE);
	}
	else
	{
		opp_server_enable_result(FALSE);
	}
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_opp_server_disable()
{
	BT_OPP_LOGD("[OPPS] btmtk_opp_server_disable()");

	MBT_OPPS_DEINITLOCK();

	if( TRUE == btmtk_opps_deactivate_server() )
	{
	    btmtk_opps_set_callback(NULL);
		opp_server_disable_result(TRUE);
	}
	else
	{
		opp_server_disable_result(FALSE);
	}
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_opp_server_disconnect()
{
	bt_opps_status_struct s;

	BT_OPP_LOGD("[EXT][btmtk_opp_server_disconnect][+]");

	MBT_OPPS_LOCK();

	// close file handle first and prevent SD Card eject caused process kill
    if( FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_recv)) ){
        btmtk_goep_close_wstream(GOPS(h_ops_recv));
        GOPS(h_ops_recv) = FHANDLE_INVALID_VALUE;
        btmtk_goep_delete_file( GOPS(recv_path) );
        GOPS(recv_path)[0] = GOPS(recv_path)[1] = 0;
    }
    if(  FALSE != ext_fs_is_valid_handle_ucs2(GOPS(h_ops_snd)) ){
        btmtk_goep_close_wstream(GOPS(h_ops_snd));
        GOPS(h_ops_snd) = FHANDLE_INVALID_VALUE;
    }

	s.size = sizeof(bt_opps_status_struct);
	btmtk_opps_get_status(&s);

	// In the connecting, use auth response to disconnect
	if( s.state == BT_OPS_CONNECTING )
	{
		btmtk_opps_authoriz_response(GOEP_UNAUTHORIZED);
		MBT_OPPS_UNLOCK();
		return;
	}

	// In the disconnecting, the disconnect will be done
	if( s.state == BT_OPS_DISCONNECTING )
	{
		MBT_OPPS_UNLOCK();
		return;
	}

	if( s.state <= BT_OPS_ACTIVE )
	{
		opp_server_disconnect_result(TRUE, FALSE);
		//opp_postevent(BT_OPPS_DISCONNECT, NULL, 0);
		MBT_OPPS_UNLOCK();
		return;
	}

	// mark as user disconnect
	if( s.state >= BT_OPS_CONNECTING )
	{
		g_user_disc = TRUE;
	}

	if( TRUE == btmtk_opps_disconnect() )
	{
		// check the current status: if it is in idle wait for disconnect_ind
	}
	else
	{
		opp_server_disconnect_result(TRUE, FALSE);
		g_user_disc = FALSE;
	}

	MBT_OPPS_UNLOCK();
}

/********************************************************************************
 * Description :
 * Parameters: size / filename (push+pull)
 ********************************************************************************/
void btmtk_opp_server_access_response(U8 goepStatus, const S8* parameters[]) // T_MBT_AUTHRES
{
	bt_opps_status_struct s;
	U32 acceptableFileSize;
	U8 ucs2_filename[BTMTK_FS_MAX_FILEPATH];

	BT_OPP_LOGD("[OPPS] btmtk_opp_server_access_response(%u,%u)", goepStatus, sizeof(parameters));

	MBT_OPPS_LOCK();

	s.size = sizeof(bt_opps_status_struct);
	btmtk_opps_get_status(&s);

	BT_OPP_LOGD("[OPPS] current state: [%u]", s.state);	// btmtk_opps_getOPPSState(s.state)

	// positive reply from mmi => yes
	if( GOEP_STATUS_SUCCESS == goepStatus ){

		switch( s.state ){
			case BT_OPS_CONNECTING:
				btmtk_opps_authoriz_response(GOEP_STATUS_SUCCESS);
				break;
			case BT_OPS_ACCEPT_WAIT:	// first push
				// check acceptable file size
				acceptableFileSize = atoi(parameters[0]);

				// incoming file is too large ( reject as data full ) => TODO should be detected by mmi
				if( 0 != acceptableFileSize && s.totalsize > acceptableFileSize )
				{
					BT_OPP_LOGD("[BTMTK][OPPS] incoming file is too large: size[%u], max[%u]", s.totalsize, acceptableFileSize);
					opp_server_push_start(TRUE, GOEP_STATUS_SUCCESS);	// for mmi accept ( need to call back )
					btmtk_opps_accept_push_file(GOEP_DATABASE_FULL, NULL);
					opp_server_push_finish(FALSE, GOEP_DATABASE_FULL, 0);
				}
				else
				{
					BT_OPP_LOGD("[BTMTK][OPPS] server_access_response(accept_wait): filename=(%s)", parameters[1]);
					// convert filename to ucs2
					ext_chset_utf8_to_ucs2_string(ucs2_filename, BTMTK_FS_MAX_FILEPATH, (const U8*)parameters[1]);
					btmtk_opps_accept_push_file(GOEP_STATUS_SUCCESS, (U8 *)ucs2_filename);
				}
                break;
			case BT_OPS_PREPARE_WAIT:	// first pull
				BT_OPP_LOGD("[BTMTK][OPPS] server_access_response(prepare_wait): size=(%s), filename=(%s)", parameters[0], parameters[1]);
				ext_chset_utf8_to_ucs2_string(ucs2_filename, BTMTK_FS_MAX_FILEPATH, (const U8*)parameters[1]);
				btmtk_opps_accept_pull_file(GOEP_STATUS_SUCCESS, (U8 *)ucs2_filename);
                break;
			case BT_OPS_DISCONNECTING:
				// user cancel the transmission between connecting and waiting. Do nothing
				break;
			case BT_OPS_ACTIVE:
				// connection has been lost.
				break;
			default:
				OPP_PI_ASSERT(0);
				break;
		}
	}
	else // negative answer from mmi
	{
		switch( s.state){
			case BT_OPS_CONNECTING:
				btmtk_opps_authoriz_response(goepStatus);
				break;
            case BT_OPS_ACCEPT_WAIT:	// first push
				btmtk_opps_accept_push_file(goepStatus, NULL);
                break;
            case BT_OPS_PREPARE_WAIT:	// first pull
				btmtk_opps_accept_pull_file(goepStatus, NULL);
                break;
            case BT_OPS_DISCONNECTING:
            case BT_OPS_ACTIVE:
				break;
            default:
                OPP_PI_ASSERT(0);
                break;
        }
    }

	MBT_OPPS_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_opp_client_enable()
{
	MBT_OPPC_INITLOCK();

	if( TRUE == btmtk_oppc_init() )
	{
		btmtk_oppc_set_callback( my_oppc_ui_cb ) ;
		opp_client_enable_result( TRUE );
	}
	else
	{
		opp_client_enable_result( FALSE );
	}
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_opp_client_disable()
{
    MBT_OPPC_DEINITLOCK();

    if( TRUE == btmtk_oppc_deinit() ){
		btmtk_oppc_set_callback( NULL ) ;
		opp_client_disable_result( TRUE );
	}else{
		opp_client_disable_result( FALSE );
	}
}


/********************************************************************************
*   Description : MBT OPP Client push a object
********************************************************************************/
void btmtk_opp_client_pushobject (bt_addr_struct *destAddr, const char *mimeType, const char *objectName, const char *filename)
{
	bt_oppc_object pushobject;
	U8 ucs2_filename[BTMTK_FS_MAX_FILEPATH];
	U8 ucs2_objectname[BTMTK_FS_MAX_FILEPATH];

	BT_OPP_LOGD("[EXT][btmtk_opp_client_pushobject]: mine[%s], name[%s], file[%s]", mimeType, objectName, filename);

	MBT_OPPC_LOCK();

	// update flag
//	g_oppc_connect_has_rsp = FALSE;
	g_oppc_user_disconnect = FALSE;
	//g_oppc_remote_disconnect = FALSE;

	// convert to ucs2
	ext_chset_utf8_to_ucs2_string(ucs2_filename, BTMTK_FS_MAX_FILEPATH, (const U8*)filename);
	ext_chset_utf8_to_ucs2_string(ucs2_objectname, BTMTK_FS_MAX_FILEPATH, (const U8*)objectName);

	// prepare push object ( file )
	mbt_opp_tool_memset( &pushobject, 0, sizeof(bt_oppc_object) );
	pushobject.isFileMode = TRUE;
	pushobject.ucFilePath = ucs2_filename;
	pushobject.ucObjName = ucs2_objectname;
	pushobject.ucMime = (const U8*)mimeType;

	if( FALSE == btmtk_oppc_push_ex_feature( &pushobject, (void *)destAddr) )
	{
	    BT_OPP_LOGI("[EXT][btmtk_opp_client_pushobject] - call btmtk_oppc_push_ex_feature() fail");
		opp_client_push_finish(FALSE, 0);
		opp_client_disconnect_result(FALSE, 0);
	}
	else
	{
		// move to pushing start
	}
	MBT_OPPC_UNLOCK();
}

void btmtk_opp_client_connect (bt_addr_struct *destAddr)
{
    BT_OPP_LOGD("[EXT][btmtk_opp_client_connect]: lap[%x], uap[%x], nap[%x]", destAddr->lap, destAddr->uap, destAddr->nap);

    MBT_OPPC_LOCK();

    // update flag
    g_oppc_user_disconnect = FALSE; // TODO useless code

    if( FALSE == btmtk_oppc_connect( (void *)destAddr) )
    {
        BT_OPP_LOGI("[EXT][btmtk_opp_client_connect] - call btmtk_oppc_connect() fail");
        opp_client_disconnect_result(FALSE, 0);  // connect fail => Disconnect
    }
    else
    {
        // move to pushing start
    }
    MBT_OPPC_UNLOCK();
}

// using existing connection to push object and doesn't disconnect after pushed
void btmtk_opp_client_push (const char *mimeType, const char *objectName, const char *filename)
{
    bt_oppc_object pushobject;
    U8 ucs2_filename[BTMTK_FS_MAX_FILEPATH];
    U8 ucs2_objectname[BTMTK_FS_MAX_FILEPATH];

    BT_OPP_LOGD("[EXT][btmtk_opp_client_pushobject]: mine[%s], name[%s], file[%s]", mimeType, objectName, filename);

    MBT_OPPC_LOCK();

    // update flag
    g_oppc_user_disconnect = FALSE;

    // convert to ucs2
    ext_chset_utf8_to_ucs2_string(ucs2_filename, BTMTK_FS_MAX_FILEPATH, (const U8*)filename);
    ext_chset_utf8_to_ucs2_string(ucs2_objectname, BTMTK_FS_MAX_FILEPATH, (const U8*)objectName);

    // prepare push object ( file )
    mbt_opp_tool_memset( &pushobject, 0, sizeof(bt_oppc_object) );
    pushobject.isFileMode = TRUE;
    pushobject.ucFilePath = ucs2_filename;
    pushobject.ucObjName = ucs2_objectname;
    pushobject.ucMime = (const U8*)mimeType;

    if( FALSE == btmtk_oppc_push_object( &pushobject ) )
    {
        BT_OPP_LOGI("[EXT][btmtk_opp_client_pushobject] - call btmtk_oppc_push_object() fail");
        opp_client_push_finish(FALSE, 0);
        opp_client_disconnect_result(FALSE, 0);
    }
    else
    {
        // move to pushing start
    }
    MBT_OPPC_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
/*
void btmtk_opp_client_pullobject (T_MBT_BDADDR RemoteBDAddr,T_MBT_OPP_OBJECT *MBTObject)
{
	T_MBT_OPP_STATUS *sdcOppStatus;
    bt_oppc_object pullobject;
	U16 filepath2[MBT_MAX_FILE_NAME_LEN * 2];
	U16 objname2[MBT_MAX_FILE_NAME_LEN];
	U8 mime2[80];
	btbm_bd_addr_t g_addr;

	T_MBT_OPP_OBJECT *RecvObject = MBTObject;
	sdcOppStatus = (T_MBT_OPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_OPP_STATUS );

	MBT_OPPC_LOCK();

//    g_oppc_connect_has_rsp = FALSE;
	OPP_PI_API( (TRACE_FUNC, BT_OPP_APPI_API_CLIENT_PULLOBJECT, RecvObject->StorageType) );

	mbt_opp_tool_memset( &pullobject, 0, sizeof(bt_oppc_object) );
	mbt_opp_tool_memset( filepath2, 0, 4);
	mbt_opp_tool_memset( objname2, 0, 4);
//	mbt_tools_MBTAddr_to_bmaddr( &g_addr, RemoteBDAddr);


	switch( RecvObject->StorageType ){
		case MBT_MEMORY_BUFFER:
			// memory method
			pullobject.isFileMode = FALSE;

			break;
		default:
			// file
			pullobject.isFileMode =  TRUE;
			break;
	}

	// If memory size is not engouth, save pull-data into file (limit size is
	pullobject.ucBuffer = RecvObject->PBuffer;
	pullobject.u4BufferSize = (U32) RecvObject->AllocBufferSize;
	pullobject.u4BufferLen = (U32) RecvObject->ObjectSize;
	pullobject.ucFilePath = (U8 *) filepath2;
	pullobject.u4MaxAcceptSize = (U32) RecvObject->AcceptableFileSize;
//	pullobject.ucMime = mbt_tools_objformat_to_ascii( RecvObject->ObjectType );
	BT_OPP_LOGD("[BTMTK][OPPC] pull restrict size:%u %u", RecvObject->StorageType, RecvObject->AllocBufferSize, RecvObject->AcceptableFileSize);

#if defined(BTMTK_ON_WISE)
	bt_strncpy(  (U8 *) objname2, "c:\\", 10);
#endif
	bt_strncat( (U8 *)objname2, RecvObject->DirName, sizeof(objname2) );
//	if( FALSE == mbt_tools_compose_folder( (U8 *) objname2, '\\', RecvObject->FileName, (U8 *)filepath2, MBT_MAX_FILE_NAME_LEN*2) ){
//	}

	// common atribute
	pullobject.ucObjName = NULL; // pull vcard use empty name header
	pullobject.ucMime = mime2;
//	strncpy(mime2, mbt_tools_objformat_to_ascii( RecvObject->ObjectType ), 80);

	memcpy( sdcOppStatus->BDAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR) );
//	memcpy( &g_client_pull_object, MBTObject, sizeof(T_MBT_OPP_OBJECT) );
	memcpy( &sdcOppStatus->ReceivingFile, MBTObject, sizeof(T_MBT_OPP_OBJECT) );
	sdcOppStatus->Operation = MBT_OPC_PULL;
    BT_OPP_LOGD("[BTMTK][OPPC] Update Operation:%u BDAddr and copy alll to ReceivingFile object", sdcOppStatus->Operation);

	g_oppc_user_disconnect = FALSE;
	//g_oppc_remote_disconnect = FALSE;
	if( FALSE == btmtk_oppc_pull_ex_feature( &pullobject, (void *)&g_addr) ) {
		opp_client_pull_finish(FALSE, 0);
		OPP_PI_API( (TRACE_FUNC, BT_OPP_APPI_ERR_CLIENT_PULLOBJECT) );
	}else{
		// move to connected
	}

	MBT_OPPC_UNLOCK();
}
*/

/********************************************************************************
*   Description :
********************************************************************************/
/*
void btmtk_opp_client_exchobject (T_MBT_BDADDR RemoteBDAddr,T_MBT_OPP_OBJECT *SendObject,T_MBT_OPP_OBJECT *RecvObject)
{
	T_MBT_OPP_STATUS *sdcOppStatus;
	bt_oppc_object pushobject, pullobject;
	U16 filepath1[MBT_MAX_FILE_NAME_LEN * 2];
	U16 filepath2[MBT_MAX_FILE_NAME_LEN * 2];

	U16 objname1[MBT_MAX_FILE_NAME_LEN];
	U16 objname2[MBT_MAX_FILE_NAME_LEN];
	U8 mime1[80];
	U8 mime2[80];

	btbm_bd_addr_t g_addr;

	MBT_OPPC_LOCK();

	OPP_PI_API( (TRACE_FUNC, BT_OPP_APPI_API_CLIENT_EXCHOBJECT, SendObject->StorageType, RecvObject->StorageType) );
//    g_oppc_connect_has_rsp = FALSE;

	sdcOppStatus = (T_MBT_OPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_OPP_STATUS );
	mbt_opp_tool_memset( &pushobject, 0, sizeof(bt_oppc_object) );
	mbt_opp_tool_memset( &pullobject, 0, sizeof(bt_oppc_object) );
	mbt_opp_tool_memset( filepath1, 0, 4);
	mbt_opp_tool_memset( filepath2, 0, 4);
	mbt_opp_tool_memset( objname1, 0, 4);
	mbt_opp_tool_memset( objname2, 0, 4);
//	mbt_tools_MBTAddr_to_bmaddr( &g_addr, RemoteBDAddr);

	switch( SendObject->StorageType ){
		case MBT_MEMORY_BUFFER:
			// memory method
			pushobject.isFileMode = FALSE;
			pushobject.ucBuffer = SendObject->PBuffer;
			pushobject.u4BufferSize = (U32) SendObject->AllocBufferSize;
			pushobject.u4BufferLen = (U32) SendObject->ObjectSize;
			break;
		default:
			// file method
			pushobject.isFileMode =  TRUE;
			pushobject.ucFilePath = (U8 *) filepath1;
			if( FALSE == mbt_tools_compose_folder(SendObject->DirName, '\\', SendObject->FileName, (U8 *) filepath1, MBT_MAX_FILE_NAME_LEN*2) ){
			}
			break;
	}
	// common atribute
	pushobject.ucObjName = (U8 *) objname1;
	pushobject.ucMime = mime1;
//	strncpy(mime1, mbt_tools_objformat_to_ascii( SendObject->ObjectType ), 80);
	mbt_tools_compose_folder(  SendObject->FileName, 0, NULL, (U8 *) objname1, MBT_MAX_FILE_NAME_LEN);

	switch( RecvObject->StorageType ){
		case MBT_MEMORY_BUFFER:
			pullobject.isFileMode = FALSE;
			break;
		default:
			// file
			pullobject.isFileMode =  TRUE;
			break;
	}

   	pullobject.ucBuffer = RecvObject->PBuffer;
   	pullobject.u4BufferSize = (U32) RecvObject->AllocBufferSize;
   	pullobject.u4BufferLen = (U32) RecvObject->ObjectSize;
	pullobject.ucFilePath = (U8 *) filepath2;
//	pullobject.ucMime = mbt_tools_objformat_to_ascii( RecvObject->ObjectType );
	pullobject.u4MaxAcceptSize = (U32) RecvObject->AcceptableFileSize;
    BT_OPP_LOGD("[BTMTK][OPPC] pull restrict size:%u %u", RecvObject->StorageType, RecvObject->AllocBufferSize, RecvObject->AcceptableFileSize);

	if( FALSE == mbt_tools_compose_folder(RecvObject->DirName, '\\', RecvObject->FileName, (U8 *)filepath2, MBT_MAX_FILE_NAME_LEN*2) ){
		opp_client_exchange_finish(FALSE, MBT_INTERNAL_SERVER_ERROR_RES);
		return;
	}

	// common atribute
	pullobject.ucObjName = (U8 *) NULL ;//  pull vcard use empty name header
	pullobject.ucMime = mime2;
//	strncpy(mime2, mbt_tools_objformat_to_ascii( RecvObject->ObjectType ), sizeof(mime2) );

	memcpy( sdcOppStatus->BDAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR) );
//	memcpy( &g_client_push_object, SendObject, sizeof(T_MBT_OPP_OBJECT) );
//	memcpy( &g_client_pull_object, RecvObject, sizeof(T_MBT_OPP_OBJECT) );
	memcpy( &sdcOppStatus->SendingFile, SendObject, sizeof(T_MBT_OPP_OBJECT) );
	memcpy( &sdcOppStatus->ReceivingFile, RecvObject, sizeof(T_MBT_OPP_OBJECT) );
    BT_OPP_LOGD("[BTMTK][OPPC] Update Operation:%u BDAddr and copy alll to SendObject,  ReceivingFile object", sdcOppStatus->Operation);

	g_oppc_user_disconnect = FALSE;
	//g_oppc_remote_disconnect = FALSE;
	if( FALSE == btmtk_oppc_exchange_ex_feature( &pushobject, &pullobject, (void *)&g_addr) ) {
		opp_client_exchange_finish(FALSE, MBT_INTERNAL_SERVER_ERROR_RES);
		OPP_PI_API( (TRACE_FUNC, BT_OPP_APPI_ERR_CLIENT_EXCHOBJECT) );
	}else{
		// wait for UI connected event
	}
	MBT_OPPC_UNLOCK();
}
*/

void btmtk_opp_client_abort()
{
    bt_oppc_status_struct c;

    BT_OPP_LOGD("[EXT][btmtk_opp_client_abort][+]");

    MBT_OPPC_LOCK();

    // PUSH ONLY: close file handle first and prevent SD Card eject caused process kill
    if( btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )
    {
        btmtk_goep_close_rstream(GOPC(h_opc_snd));
    }
    GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;
    if( btmtk_goep_is_valid_handle( GOPC(h_opc_recv))  ){
        btmtk_goep_close_wstream(GOPC(h_opc_recv));
    }
    GOPC(h_opc_recv) = (FHANDLE)FHANDLE_INVALID_VALUE;

    if( GOPC(isAbort) == TRUE )
    {
        // abort flag is set, waiting for result
    }
    else
    {
        BT_OPP_LOGD("[EXT][btmtk_opp_client_abort]");
        btmtk_oppc_abort(); // when aborted => BT_OPPC_CONNECTED will be received
    }
    MBT_OPPC_UNLOCK();
}
/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_opp_client_disconnect()
{
	bt_oppc_status_struct c;

	BT_OPP_LOGD("[EXT][btmtk_opp_client_disconnect][+]");

	MBT_OPPC_LOCK();

	// PUSH ONLY: close file handle first and prevent SD Card eject caused process kill
    if( btmtk_goep_is_valid_handle(GOPC(h_opc_snd)) )
    {
        btmtk_goep_close_rstream(GOPC(h_opc_snd));
    }
    GOPC(h_opc_snd) = (FHANDLE)FHANDLE_INVALID_VALUE;
    if( btmtk_goep_is_valid_handle( GOPC(h_opc_recv))  ){
        btmtk_goep_close_wstream(GOPC(h_opc_recv));
    }
    GOPC(h_opc_recv) = (FHANDLE)FHANDLE_INVALID_VALUE;

    g_oppc_user_disconnect = TRUE;
    c.size = sizeof(bt_oppc_status_struct);
    btmtk_oppc_get_status(&c);

	if( c.state == BT_OPC_DISCONNECTING )
	{
		/// In the disconnecting, the disconnect will be done
	}
	else
	{
		BT_OPP_LOGD("[EXT][btmtk_opp_client_disconnect]: current_state[%u]", c.state);
		if( btmtk_oppc_disconnect_feature() == FALSE )
		{
			opp_client_disconnect_result(TRUE, 1);
		}
		else
		{
			// Wait for ui event disconnect
		}
	}
	MBT_OPPC_UNLOCK();
}

U8 btmtk_opp_is_connected(void){
	U8 l_ret;

	bt_opps_status_struct s;
	s.size = sizeof(bt_opps_status_struct);
	btmtk_opps_get_status(&s);
	if( s.state >= BT_OPS_CONNECTED ){
		l_ret = TRUE;
	}else{
		l_ret = FALSE;
	}

	return FALSE;
}

/*
void btmtk_opp_authorize_res(MBT_BYTE AuthMode){

	OPP_PI_API( (TRACE_FUNC, BT_OPP_APPI_API_SERVER_AUTHORIZE_RES, AuthMode) );

	if( FALSE != AuthMode ){
	    btmtk_opps_authoriz_response(GOEP_STATUS_SUCCESS);
	}else{
		btmtk_opps_authoriz_response(GOEP_FORBIDDEN); //GOEP_UNAUTHORIZED is used for authentication(OBEX v1.0 spec)
	}
}
*/

/*
U8 mbt_tools_is_match_addr(btbm_bd_addr_t *bm_addr,T_MBT_BDADDR bdaddr){
	//
	T_MBT_BDADDR l_bdaddr;
	U8 i;

//	mbt_tools_bmaddr_to_MBTAddr(&l_bdaddr, bm_addr); // struct to byte array
	for(i =  0; i < sizeof(T_MBT_BDADDR); i++){
		if( l_bdaddr[i] != bdaddr[i] ){
			return FALSE;
		}
	}

	return TRUE;
}
*/

/*
U8 btmtk_opp_is_dev_connected(T_MBT_BDADDR RemoteBDAddr){
	U8 l_ret;

	bt_opps_status_struct s;
	s.size = sizeof(bt_opps_status_struct);
	btmtk_opps_get_status(&s);
	if( s.state >= BT_OPS_CONNECTED && TRUE == mbt_tools_is_match_addr(&s.remote_dev_addr, RemoteBDAddr) ){
		l_ret = TRUE;
	}else{
		l_ret = FALSE;
	}

	OPP_PI_API( (TRACE_FUNC, BT_OPP_APPI_API_OPP_IS_DEV_CONNECTED, l_ret) );

	return FALSE;
}
*/
