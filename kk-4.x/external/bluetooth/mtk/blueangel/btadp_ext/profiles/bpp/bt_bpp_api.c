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
 * Bt_bpp_api.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Tina Shen
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
#include "string.h"
#include "bt_message.h"
#include "bt_mmi.h"
#include "bluetooth_bpp_struct.h"
#include "MBTType.h"
#include "MBTSdc.h"
#include "bt_common.h"
#include "bt_mmi_msg.h"
#include "osapi.h"
#include "bt_ext_utility.h"
#include "ext_ucs2.h"
#include "bluetooth_gap_struct.h"

#include "bt_bpp_api.h"

#include "mbt_debugmsg.h"
#include "mbt_enum2string.h"

typedef enum
{
	MTKBPP_STATE_DISABLED = 0,	
	MTKBPP_STATE_ENABLED,		
	MTKBPP_STATE_CONNECTING,
	MTKBPP_STATE_CONNECTED,
	MTKBPP_STATE_CANCELING,
	MTKBPP_STATE_GETTING_ATTR,
	MTKBPP_STATE_PRINTING,
	MTKBPP_STATE_DISCONNECTING,
	MTKBPP_STATE_DISABLING,	

} BTMTK_BPP_STATE;

typedef enum
{
	MTKBPP_OPER_NONE = 0,
	MTKBPP_OPER_GETATTR,
	MTKBPP_OPER_PRINT,
	MTKBPP_OPER_DISCONNECT
	
}BTMTK_BPP_OPERATION;

typedef enum
{
	BTMTK_BPP_CNF_SUCCESS = BPP_SUCCESS,
	BTMTK_BPP_CNF_ERR_STATE,
	BTMTK_BPP_CNF_ERR_RESP,		
	BTMTK_BPP_CNF_CONN_FAILED,
	BTMTK_BPP_CNF_DISCONN_IND
}BTMTK_BPP_CNF_CODE;

/* authentication flag */
#define MTKBPP_AUTH_NONE  		0x00
#define MTKBPP_AUTH_RECV_CHAL  	0x01
#define MTKBPP_AUTH_SENT_RESP  	0x02

typedef struct
{
	BTMTK_BPP_STATE 	state;			 /* current state */
	S32          		hprinter; 		 /* printer handle */
	U8           		printer_addr[6]; /* LSB */
	U8					auth_flag; 		 /* flag: BTMTK_BPP_AUTH_xxx */
	
	U32                 file_size;       /* original file size:
											if the file is composed into xhtml,
											then the data size will be larger.
											So record the original file size for 
											use in progress indication. */
	
	BTMTK_BPP_OPERATION  oper_type;		 /* operation type */
	union
	{
		U32  			 bitmask_attr;   /* for GetPrinterAttributes */
		T_MBT_BPP_OBJECT print_object;   /* for print file */
	}oper;
	
}bt_bpp_cntx_struct;


static void bt_bpp_init(void);
static void bt_bpp_deinit(void);
static void bt_bpp_reset_cntx(void);
static void bt_bpp_reset_cntx_to_enabled(void);

static void bt_bpp_connect(U8 *btAddr);
static void bt_bpp_getprinterattribute(U32 bitmask_attr);
static S16 bt_bpp_print(T_MBT_BPP_OBJECT *MBTObject);

static void bt_bpp_send_disconnect_req(void);

void bt_bpp_mbt_postevent(T_MBTEVT Evt, MBT_SHORT conn_idx);


void bt_bpp_enable_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
void bt_bpp_disable_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
void bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_CODE cnf_code, bt_bpp_printer_attributes * p_attr);
void bt_bpp_print_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
void bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
void bt_bpp_connect_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
void bt_bpp_cancel_req_cb(BTMTK_BPP_CNF_CODE cnf_code);

void bt_bpp_update_sdc_state(T_MBT_BPP_STATE new_state);
void bt_bpp_update_sdc_state_addr(T_MBT_BPP_STATE state, T_MBT_BDADDR addr);
void bt_bpp_update_sdc_state_addr_printobj(T_MBT_BPP_STATE state, T_MBT_BDADDR addr, T_MBT_BPP_OBJECT *MBTObject);


extern MBT_VOID* mbt_sdc_getrecord(T_MBT_SDC_RECID MBTSDCRecID);
extern MBT_BOOL mbt_postevent(T_MBTEVT Evt, MBT_SHORT conn_idx);

extern void btmtk_util_convert_mbtarray2mtkarray(U8 *dest, U8 *source);

#if defined (BTMTK_ON_WISE)

S32 ext_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size,const U8 *src);

extern int wsConvertFileNameToNativeFormat(unsigned short *targetBuff_p, U8* fileName);

#endif


#ifdef BTMTK_WISE_MBT_LOG

#define BPP_MBT_LOG(x)   			MBT_LOG(x)
#define BPP_MBT_LOG1(x, p1)  		MBT_LOG1(x, p1)
#define BPP_MBT_LOG2(x, p1, p2)  	MBT_LOG2(x, p1, p2)
#define BPP_MBT_LOG3(x, p1, p2, p3)	MBT_LOG3(x, p1, p2, p3)

#else /* BTMTK_WISE_MBT_LOG */

#define BPP_MBT_LOG(x)
#define BPP_MBT_LOG1(x, p1)
#define BPP_MBT_LOG2(x, p1, p2)
#define BPP_MBT_LOG3(x, p1, p2, p3)

#endif /* BTMTK_WISE_MBT_LOG */


#define UPDATE_SDCBPP_STATE(sdcBppStatus, new_state) \
if (sdcBppStatus->State != new_state)\
{\
	MBT_LOG2("[BTMTK][BPP]Update sdcBppStatus->State: from (%s) to (%s)", T_MBT_BPP_STATE_tostring(sdcBppStatus->State), T_MBT_BPP_STATE_tostring(new_state));\
    sdcBppStatus->State = new_state;\
}


/* in bpp_data.c */
extern const kal_char* bpp_media_size_self_desc_name[];
extern bpp_mime_id_str_struct bpp_mime_type_table[];


/* global context */
static bt_bpp_cntx_struct g_bt_bpp_cntx;

/* The bpp connection handle seed. 
   Used to generate the new bpp connection handle.
 */
static int bpp_printer_handle_seed = 0;

#define BPP_API_FOR_APP

MBT_VOID btmtk_bpp_enable(MBT_VOID)
{
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_ENABLE, g_bt_bpp_cntx.state);

    if(g_bt_bpp_cntx.state != MTKBPP_STATE_DISABLED)
    {
        bt_bpp_enable_req_cb(BTMTK_BPP_CNF_SUCCESS);
        return;
    }

    bt_bpp_init();

    g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;

    bt_bpp_enable_req_cb(BTMTK_BPP_CNF_SUCCESS);
}

MBT_VOID btmtk_bpp_disable (MBT_VOID)
{
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISABLE, g_bt_bpp_cntx.state);

    if(g_bt_bpp_cntx.state == MTKBPP_STATE_DISABLED)
    {
        bt_bpp_disable_req_cb(BTMTK_BPP_CNF_SUCCESS);
        return;
    }

    if(g_bt_bpp_cntx.state == MTKBPP_STATE_ENABLED)
    {
		bt_bpp_deinit();
	    g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLED;
		
	    bt_bpp_disable_req_cb(BTMTK_BPP_CNF_SUCCESS);
        return;
    }

	if (g_bt_bpp_cntx.state == MTKBPP_STATE_DISABLING)
	{
		/* just wait */
		return;
	}

	/* connecting, connected, printing, disconnecting */
	if (g_bt_bpp_cntx.state == MTKBPP_STATE_DISCONNECTING 
		|| g_bt_bpp_cntx.state == MTKBPP_STATE_CANCELING)
	{
		/* wait disconnect cnf */
		g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLING;
	}
	else
	{
		/* send disconnect req and wait disconnect cnf */
		bt_bpp_send_disconnect_req();
		g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLING;
	}
	
}

MBT_VOID btmtk_bpp_auth_response(T_MBT_OBEX_AUTH *auth_reply)
{
    U8 copy_len_userid;
    U8 copy_len_pwd;
    bt_bpp_auth_rsp_struct msg;

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_AUTH_RSP, g_bt_bpp_cntx.state, g_bt_bpp_cntx.auth_flag, g_bt_bpp_cntx.hprinter, auth_reply->bAuth);

	BPP_MBT_LOG3("[BTMTK][BPP]btmtk_bpp_auth_response: bAuth=%d, Passwd=%s, UserID=%s", auth_reply->bAuth, auth_reply->Passwd, auth_reply->UserId);

    if(!(g_bt_bpp_cntx.auth_flag & MTKBPP_AUTH_RECV_CHAL))
    {
    	//bpp did not get the authentication indication
        return;
    }

	memset (&msg.auth_resp, 0, sizeof(msg.auth_resp));
	
    msg.hprinter = g_bt_bpp_cntx.hprinter;
	
    msg.auth_resp.cancel = auth_reply->bAuth;

	copy_len_userid =  (MBT_OBEX_AUTH_USERID_LEN < BTMTK_OBEX_AUTH_USERID_LEN) ? MBT_OBEX_AUTH_USERID_LEN : BTMTK_OBEX_AUTH_USERID_LEN;
    memcpy(msg.auth_resp.userid, auth_reply->UserId, copy_len_userid);
    msg.auth_resp.userid_len = strlen(msg.auth_resp.userid);

	copy_len_pwd =  (MBT_OBEX_AUTH_PASSWD_LEN < BTMTK_OBEX_AUTH_PASSWD_LEN) ? MBT_OBEX_AUTH_PASSWD_LEN : BTMTK_OBEX_AUTH_PASSWD_LEN;
    memcpy(msg.auth_resp.passwd, auth_reply->Passwd, copy_len_pwd);
    msg.auth_resp.passwd_len = strlen(msg.auth_resp.passwd);
	
    BTCMD_SendMessage(MSG_ID_BT_BPP_AUTH_RSP, MOD_BT, &msg, sizeof(bt_bpp_auth_rsp_struct));

	g_bt_bpp_cntx.auth_flag |= MTKBPP_AUTH_SENT_RESP;

}

MBT_VOID btmtk_bpp_getprinterattribute(T_MBT_BDADDR BdAddr, MBT_INT bitmask_attr)
{

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_GET_ATT, g_bt_bpp_cntx.state, BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5], 
				bitmask_attr, g_bt_bpp_cntx.oper_type);

    BPP_MBT_LOG1("[BTMTK][BPP]btmtk_bpp_getprinterattribute: bitmask_attr=0x%x", bitmask_attr);

    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_ENABLED:
			// bpp state is enable, create bpp connection first
      
            //store the value of btmask attr
			g_bt_bpp_cntx.oper_type = MTKBPP_OPER_GETATTR;
			g_bt_bpp_cntx.oper.bitmask_attr = bitmask_attr;
        
		    //update the BDAddr/State value of the BPP Status in the SDC
			bt_bpp_update_sdc_state_addr(MBT_BPP_STATE_CONNECTING, BdAddr);

            //create bpp connection first
            bt_bpp_connect(BdAddr);

            break;
        
        case MTKBPP_STATE_CONNECTING:
            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) == 0)
            {
            	/*connecting to the same printer...
                  store the value of btmask attr
                 */ 
				g_bt_bpp_cntx.oper_type = MTKBPP_OPER_GETATTR;
				g_bt_bpp_cntx.oper.bitmask_attr = bitmask_attr;
				
			    //update the BDAddr/State value of the BPP Status in the SDC
				bt_bpp_update_sdc_state_addr(MBT_BPP_STATE_CONNECTING, BdAddr);
				
            }
            else
            {
            	//request to the different printer...
                bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_ERR_STATE, NULL);
            }
            break;

        case MTKBPP_STATE_CONNECTED:
        case MTKBPP_STATE_PRINTING:
            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) == 0)
            {
			    //update the BDAddr/State value of the BPP Status in the SDC
				bt_bpp_update_sdc_state_addr(MBT_BPP_STATE_GETATTRIBUTE, BdAddr);

            	//connecting to the same pritner...
                //if Bpp connection exists, send GET PRINT ATTR REQ
                bt_bpp_getprinterattribute(bitmask_attr);
            }
            else
            {
            	//request to the different printer...
                bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_ERR_STATE, NULL);
            }
            break;

        case MTKBPP_STATE_GETTING_ATTR:
            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) != 0)
            {
            	//request to the different printer...
                bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_ERR_STATE, NULL);
            }
			// else, just waiting
            break;

		default:
            bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_ERR_STATE, NULL);
            break;
    }
}

MBT_VOID btmtk_bpp_print(T_MBT_BDADDR BdAddr, T_MBT_BPP_OBJECT *MBTObject)
{

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_PRINT, g_bt_bpp_cntx.state, BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5], g_bt_bpp_cntx.oper_type);

    BPP_MBT_LOG3("[BTMTK][BPP]btmtk_bpp_print: object name(%s), size(%d), bJobBase(%d)", MBTObject->FileName, MBTObject->ObjectSize, MBTObject->bJobBasedPrinting);

    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_ENABLED:
            // bpp state is enable, create bpp connection first
    
            //store the value of MBTObject
   			g_bt_bpp_cntx.oper_type = MTKBPP_OPER_PRINT;
			memcpy(&(g_bt_bpp_cntx.oper.print_object), MBTObject, sizeof(T_MBT_BPP_OBJECT));
			g_bt_bpp_cntx.file_size = (U32)MBTObject->ObjectSize;

		    //update the BDAddr/State value of the BPP Status in the SDC
			bt_bpp_update_sdc_state_addr_printobj(MBT_BPP_STATE_CONNECTING, BdAddr, MBTObject);
            
            //if no BPP connection, create bpp connection first
            bt_bpp_connect(BdAddr);

            break;
        
        case MTKBPP_STATE_CONNECTING:

            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) == 0)
            {
                /*connecting to the same pritner...
                  store the value of MBTObject
                 */
	   			g_bt_bpp_cntx.oper_type = MTKBPP_OPER_PRINT;
				memcpy(&(g_bt_bpp_cntx.oper.print_object), MBTObject, sizeof(T_MBT_BPP_OBJECT));
			    g_bt_bpp_cntx.file_size = (U32)MBTObject->ObjectSize;

				bt_bpp_update_sdc_state_addr_printobj(MBT_BPP_STATE_CONNECTING, BdAddr, MBTObject);
            }
            else
            {
                //request to the different printer...
				bt_bpp_print_req_cb(BTMTK_BPP_CNF_ERR_STATE);
            }
            break;
        
        case MTKBPP_STATE_CONNECTED:
        //case MTKBPP_STATE_PRINTING:
            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) == 0)
            {
				S16 ret;

				bt_bpp_update_sdc_state_addr_printobj(MBT_BPP_STATE_PRINTING, BdAddr, MBTObject);

				g_bt_bpp_cntx.file_size = (U32)MBTObject->ObjectSize;

                //connecting to the same pritner...
                //if Bpp connection exists, send PRINT REQ
                ret = bt_bpp_print(MBTObject);

				if (ret < 0)
				{
					/* FAILED */
					g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
					
					bt_bpp_print_req_cb(BTMTK_BPP_CNF_ERR_RESP);
				}
            }
            else
            {
                //request to the different printer...
				bt_bpp_print_req_cb(BTMTK_BPP_CNF_ERR_STATE);
            }
            break;

		default:
			bt_bpp_print_req_cb(BTMTK_BPP_CNF_ERR_STATE);
			break;
			
    }
}

MBT_VOID btmtk_bpp_disconnect(MBT_VOID)
{
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISC, g_bt_bpp_cntx.state);
    //mbt call bpp disconnect command
    //we can reset the bpp_request_getprinterattribute and bpp_request_print flag
    //because we will read the flag value when receiving disconnect_cnf
    
	BPP_MBT_LOG1("[BTMTK][BPP]btmtk_bpp_disconnect, g_bt_bpp_cntx.state=%d", g_bt_bpp_cntx.state);    

    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_DISCONNECTING:
        case MTKBPP_STATE_CANCELING:
            break;

        case MTKBPP_STATE_CONNECTING:

            //send the disconnect request directly
			bt_bpp_send_disconnect_req();

			/* connect req for GetPrinterAttribute or Print */
            g_bt_bpp_cntx.state = MTKBPP_STATE_CANCELING;    

            break;

        case MTKBPP_STATE_CONNECTED:
            //send the disconnect request directly
			bt_bpp_send_disconnect_req();
        
            break;
        
        case MTKBPP_STATE_PRINTING:
        case MTKBPP_STATE_GETTING_ATTR:
        
            //send the disconnect request directly
			bt_bpp_send_disconnect_req();

            //mark state in MTKBPP_STATE_CANCELING
            //post cancel event when receiving DISC cnf
            g_bt_bpp_cntx.state = MTKBPP_STATE_CANCELING;    
            break;

		default:
			bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_SUCCESS);
			break;
			
    }
}

MBT_BOOL btmtk_bpp_is_dev_connected(T_MBT_BDADDR rem_bdaddr)
{
	MBT_BOOL bpp_connected;

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_IS_DEV_CONNECTED, g_bt_bpp_cntx.state);

	if ((g_bt_bpp_cntx.state > MTKBPP_STATE_CONNECTING && g_bt_bpp_cntx.state < MTKBPP_STATE_DISCONNECTING)
		&& (memcmp(rem_bdaddr, g_bt_bpp_cntx.printer_addr, sizeof(T_MBT_BDADDR)) == 0))
	{
		bpp_connected = MBT_TRUE;
	}
	else
	{
		bpp_connected = MBT_FALSE;
	}

	BPP_MBT_LOG1("[BTMTK][BPP]btmtk_bpp_is_dev_connected return (%d)", bpp_connected);    

	return bpp_connected;
}
	
MBT_BOOL btmtk_bpp_is_connected(void)
{
	MBT_BOOL bpp_connected;

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_IS_CONNECTED, g_bt_bpp_cntx.state);

	BPP_MBT_LOG1("[BTMTK][BPP]btmtk_bpp_is_connected, g_bt_bpp_cntx.state=%d", g_bt_bpp_cntx.state);    

	if (g_bt_bpp_cntx.state > MTKBPP_STATE_CONNECTING && g_bt_bpp_cntx.state < MTKBPP_STATE_DISCONNECTING)
	{
		bpp_connected = MBT_TRUE;
	}
	else
	{
		bpp_connected = MBT_FALSE;
	}

	return bpp_connected;
}
	
#define BPP_INTERNAL_FUNC

void bt_bpp_connect_cnf(BPP_ERROR_CODE bpp_conn_result)
{
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_CONN_CNF, g_bt_bpp_cntx.state, bpp_conn_result);

    if(bpp_conn_result != BPP_SUCCESS)
    {
        //callback
    	bt_bpp_connect_req_cb(BTMTK_BPP_CNF_ERR_RESP);

        if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_GETATTR)
        {
           /* the connection is for get printer attribute.
              need reset flag and post fail event when connection fail.
            */
            bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_CONN_FAILED, NULL);
        }
        if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_PRINT)
        {
        	/* the connection is for print object
          	   need reset flag and post fail event when connection fail
          	*/
            bt_bpp_print_req_cb(BTMTK_BPP_CNF_CONN_FAILED);
        }

		/* reset cntx to enabled state */
        bt_bpp_reset_cntx_to_enabled();
    }
	else
	{
	    /* CONNECT success: 
	       update bpp state
	     */
	    g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;

	    //update bpp state
		bt_bpp_connect_req_cb(BTMTK_BPP_CNF_SUCCESS);

		/* execute next operation */
	    if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_GETATTR)
	    {
	        bt_bpp_getprinterattribute(g_bt_bpp_cntx.oper.bitmask_attr);
	    }
	    else if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_PRINT)
	    {
	        S16 ret = bt_bpp_print(&g_bt_bpp_cntx.oper.print_object);
			if (ret < 0)
			{
				/* FAILED */
				bt_bpp_print_req_cb(BTMTK_BPP_CNF_ERR_RESP);
			}
	    }
		
		/* clear the flag */
		g_bt_bpp_cntx.oper_type = MTKBPP_OPER_NONE;
	}
}

void bt_bpp_getprinterattribute_cnf(BPP_ERROR_CODE bpp_getatt_result, bt_bpp_printer_attributes* p_attr)
{
	BTMTK_BPP_CNF_CODE cnf_code;
		
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_GET_ATT_CNF, g_bt_bpp_cntx.state, bpp_getatt_result);	

	if (bpp_getatt_result == BPP_SUCCESS)
	{
		cnf_code = BTMTK_BPP_CNF_SUCCESS;
	}
	else
	{
		cnf_code = BTMTK_BPP_CNF_ERR_RESP;
	}

	/* callback */
	bt_bpp_getprinterattr_req_cb(cnf_code, p_attr);


	/* update state */
    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_GETTING_ATTR:
            //update bpp state
            g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
            break;

		default:
			break;
    }
}

void bt_bpp_print_cnf(BPP_ERROR_CODE bpp_print_result)
{
	BTMTK_BPP_CNF_CODE cnf_code;

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_PRINT_CNF, g_bt_bpp_cntx.state, bpp_print_result);

	if (bpp_print_result == BPP_SUCCESS)
	{
		cnf_code = BTMTK_BPP_CNF_SUCCESS;
	}
	else
	{
		cnf_code = BTMTK_BPP_CNF_ERR_RESP;
	}

	/* callback */
	bt_bpp_print_req_cb(cnf_code);

	/* update state */
    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_PRINTING:
            //update bpp state
            g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
            break;
		
		default:
			break;
    }
}

void bt_bpp_progress_rsp(void)
{
    bt_bpp_progress_rsp_struct msg;

    msg.hprinter = g_bt_bpp_cntx.hprinter;
    
    BTCMD_SendMessage(MSG_ID_BT_BPP_PROGRESS_RSP, MOD_BT, &msg, sizeof(bt_bpp_progress_rsp_struct));
}

void bt_bpp_disconnect_cnf(BPP_ERROR_CODE bpp_disc_result)
{
	BTMTK_BPP_CNF_CODE cnf_code;

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISC_CNF, g_bt_bpp_cntx.state, bpp_disc_result);

	if (bpp_disc_result == BPP_SUCCESS)
	{
		cnf_code = BTMTK_BPP_CNF_SUCCESS;
	}
	else
	{
		cnf_code = BTMTK_BPP_CNF_ERR_RESP;
	}

	if (g_bt_bpp_cntx.state == MTKBPP_STATE_DISABLING)
	{
		/* disabling */
		
		if (bpp_disc_result == BPP_SUCCESS)
		{
		    bt_bpp_reset_cntx();
		}
		else
		{
		    g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;
		}

		bt_bpp_disable_req_cb(cnf_code);

		return;
	}

    /* callback : CANCEL & DISCONNECT event */
    if(g_bt_bpp_cntx.state == MTKBPP_STATE_CANCELING)
    {
        bt_bpp_cancel_req_cb(cnf_code);
    }
	
    bt_bpp_disconnect_req_cb(cnf_code);

	/* reset cntx to enabled state */
    bt_bpp_reset_cntx_to_enabled();
}

void bt_bpp_disconnect_ind(void)
{
    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISC_IND, g_bt_bpp_cntx.state);

    switch(g_bt_bpp_cntx.state)
    {
    	case MTKBPP_STATE_DISABLING:
			bt_bpp_disable_req_cb(BTMTK_BPP_CNF_SUCCESS);
		    bt_bpp_reset_cntx();

			return;
			
        case MTKBPP_STATE_CANCELING:
	        bt_bpp_cancel_req_cb(BTMTK_BPP_CNF_SUCCESS);
            break;

        case MTKBPP_STATE_GETTING_ATTR:
			bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_DISCONN_IND, NULL);
			break;
			
        case MTKBPP_STATE_PRINTING:
			bt_bpp_print_req_cb(BTMTK_BPP_CNF_DISCONN_IND);
            break;
        
		default:
			break;
    }

    /* callback */
    bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_SUCCESS);

	/* reset cntx to enabled state */
    bt_bpp_reset_cntx_to_enabled();

}

void bt_bpp_mbt_postevent(T_MBTEVT Evt, MBT_SHORT conn_idx)
{
    OS_Report("[BPP EXT]BPP mbt post event:%d, conn_idx:%d", Evt, conn_idx);
	BPP_MBT_LOG2("[BTMTK][BPP]BPP mbt post event: event=%d, conn_idx=%d", Evt, conn_idx);
	
    mbt_postevent(Evt, conn_idx);
}

static void bt_bpp_init(void)
{
    bpp_printer_handle_seed = 0;
	
    bt_bpp_reset_cntx();
}

static void bt_bpp_deinit(void)
{
    bt_bpp_init();
}

static void bt_bpp_reset_cntx(void)
{
	memset (&g_bt_bpp_cntx, 0, sizeof(g_bt_bpp_cntx));
}

static void bt_bpp_reset_cntx_to_enabled(void)
{
	memset (&g_bt_bpp_cntx, 0, sizeof(g_bt_bpp_cntx));

	g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;
}

static void bt_bpp_connect(U8 *btAddr)
{
    bt_bpp_connect_req_struct msg;

	BPP_MBT_LOG("[BTMTK][BPP]send CONNECT request ");

    g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTING;

    //backup the printer bd addr
    memcpy(g_bt_bpp_cntx.printer_addr, btAddr, 6);

    //transfrom LSB to MSB
    btmtk_util_convert_mbtarray2mtkarray(msg.addr.addr, btAddr);

    //get the printer handler
    g_bt_bpp_cntx.hprinter = msg.hprinter = bpp_printer_handle_seed++;
    BTCMD_SendMessage(MSG_ID_BT_BPP_CONNECT_REQ, MOD_BT, &msg, sizeof(bt_bpp_connect_req_struct));
}

static void bt_bpp_getprinterattribute(U32 bitmask_attr)
{
    bt_bpp_get_printer_attr_req_struct msg;

	BPP_MBT_LOG1("[BTMTK][BPP]send GetPrinterAttributes request, bitmast_attr=0x%x", bitmask_attr);

    g_bt_bpp_cntx.state = MTKBPP_STATE_GETTING_ATTR;

	bt_bpp_update_sdc_state(MBT_BPP_STATE_GETATTRIBUTE);

    msg.attr_bitmask = bitmask_attr;
    msg.hprinter = g_bt_bpp_cntx.hprinter;
        
    BTCMD_SendMessage(MSG_ID_BT_BPP_GET_PRINTER_ATTR_REQ, MOD_BT, &msg, sizeof(bt_bpp_get_printer_attr_req_struct));
}

static S16 bt_bpp_print(T_MBT_BPP_OBJECT *MBTObject)
{
    I8 i;
	U8   filename[BTMTK_MAX_FILE_NAME_LEN];
#if defined (BTMTK_ON_WISE)
    //MBT target case
    U16 *ucs2filename = NULL;
    U16 ucs2name_len;
#endif
    bt_bpp_print_doc_req_struct msg;
	S32 dirlen, namelen;

	BPP_MBT_LOG("[BTMTK][BPP]send PRINT DOCUMENT request: ");
	BPP_MBT_LOG2("    DirName=%s, FileName=%s", MBTObject->DirName, MBTObject->FileName);
	BPP_MBT_LOG3("    MIME=%s, ObjectSize=%d, bJobBasePrinting=%d", T_MBT_MIME_MEDIA_tostring(MBTObject->MimeType), MBTObject->ObjectSize, MBTObject->bJobBasedPrinting);
	BPP_MBT_LOG3("    Copies=%d, NumberUp=%d, Sides=%d", MBTObject->PrintingAttribute.Copies, MBTObject->PrintingAttribute.NumberUp, MBTObject->PrintingAttribute.Sides);
	BPP_MBT_LOG3("    Orient=%d, Quality=%d, DocFmt=%s", MBTObject->PrintingAttribute.Orient, MBTObject->PrintingAttribute.Quality, MBTObject->PrintingAttribute.DocFmt);
	BPP_MBT_LOG2("    MeidaSize=%s, MediaType=%s", MBTObject->PrintingAttribute.MediaSize, T_MBT_BPP_MEDIA_tostring(MBTObject->PrintingAttribute.MediaType));
	

    g_bt_bpp_cntx.state = MTKBPP_STATE_PRINTING;

    //update the State value of the BPP Status in the SDC
	bt_bpp_update_sdc_state(MBT_BPP_STATE_PRINTING);
	
	/* set msg structure */
    memset(&msg, 0, sizeof(bt_bpp_print_doc_req_struct));
    memset(filename, 0, BTMTK_MAX_FILE_NAME_LEN);
    msg.hprinter = g_bt_bpp_cntx.hprinter;
	
    //Transform T_MBT_BPP_OBJECT to bt_bpp_object
    dirlen = strlen(MBTObject->DirName);
	namelen = strlen(MBTObject->FileName);
	if(BTMTK_MAX_FILE_NAME_LEN < dirlen)
	{
	    return -1;
	}
    memcpy(filename, MBTObject->DirName, dirlen);
    if((BTMTK_MAX_FILE_NAME_LEN - dirlen) < namelen)
	{
	    return -1;
	}
	
#if defined (BTMTK_ON_WISE)

	ucs2name_len = BTMTK_MAX_FILE_NAME_LEN * 2;
	ucs2filename = get_ctrl_buffer(ucs2name_len);

    //MBT target case
    memcpy(filename + dirlen, MBTObject->FileName, namelen);
    ext_chset_utf8_to_ucs2_string((U8 *)ucs2filename, ucs2name_len, filename);
    wsConvertFileNameToNativeFormat((U16*)msg.print_object.file_name, (U8*)ucs2filename);

	free_ctrl_buffer(ucs2filename);

#else

    *(filename + dirlen) = '/';
    memcpy(filename + dirlen + 1, MBTObject->FileName, namelen);
    ext_chset_utf8_to_ucs2_string((U8*)msg.print_object.file_name, sizeof(msg.print_object.file_name), filename);

#endif

    OS_Report("[BPP_EXT]bt_bpp_print, folder(%s) file(%s) fullpath(%s)", MBTObject->DirName, MBTObject->FileName, filename);

    msg.print_object.object_size = (U32)MBTObject->ObjectSize;
    msg.print_object.mime_type = (bt_bpp_mime_type)(MBTObject->MimeType + 1); /* map */

    msg.print_object.print_model = (MBTObject->bJobBasedPrinting) ? BPP_MODEL_JOB_BASE : BPP_MODEL_SIMPLE_PUSH;
	
    memset(msg.print_object.job_name, 0, sizeof(msg.print_object.job_name));
    memset(msg.print_object.originating_user_name, 0, sizeof(msg.print_object.originating_user_name));
    msg.print_object.job_config.copies = MBTObject->PrintingAttribute.Copies;
    msg.print_object.job_config.numberup = (bt_bpp_numberup_enum)(MBTObject->PrintingAttribute.NumberUp);
    msg.print_object.job_config.sides = (bt_bpp_sided_enum)(MBTObject->PrintingAttribute.Sides);
    msg.print_object.job_config.orient = (bt_bpp_orient_enum)(MBTObject->PrintingAttribute.Orient);
    msg.print_object.job_config.quality = (bt_bpp_quality_enum)(MBTObject->PrintingAttribute.Quality);
    //find the media size enum value
    if(strcmp("na_index_4x6in", (MBTObject->PrintingAttribute).MediaSize) == 0)
    {
        msg.print_object.job_config.media_size = BPP_MEDIA_SIZE_4X6_POSTCARD;
    }
    else
    {
        for(i = 0; i < BPP_MEDIA_SIZE_MAX_ENUM; i++)
        {
            if (strcmp((MBTObject->PrintingAttribute).MediaSize, bpp_media_size_self_desc_name[i]) == 0)
            {
                msg.print_object.job_config.media_size = (bt_bpp_media_size_enum)i;
                break;
            }
        }
    }
    msg.print_object.job_config.media_type = (bt_bpp_media_type_enum)(MBTObject->PrintingAttribute.MediaType);

    BTCMD_SendMessage(MSG_ID_BT_BPP_PRINT_DOC_REQ, MOD_BT, &msg, sizeof(bt_bpp_print_doc_req_struct));

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_SEND_PRINT_REQ, msg.print_object.print_model, msg.print_object.mime_type, msg.print_object.object_size);

	return 0;
}

void bt_bpp_send_disconnect_req(void)
{
    bt_bpp_disconnect_req_struct msg;

    g_bt_bpp_cntx.state = MTKBPP_STATE_DISCONNECTING;    
        
    //update the State value of the BPP Status in the SDC.
	bt_bpp_update_sdc_state(MBT_BPP_STATE_DISCONNECTING);

	BPP_MBT_LOG("[BTMTK][BPP]send DISCONNECT request");

    //send the disconnect request directly
    msg.hprinter = g_bt_bpp_cntx.hprinter;
    BTCMD_SendMessage(MSG_ID_BT_BPP_DISCONNECT_REQ, MOD_BT, &msg, sizeof(bt_bpp_disconnect_req_struct));
}

#define BPP_APP_REQ_CALLBACK

void bt_bpp_enable_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    T_MBT_BPP_STATUS *sdcBppStatus;
	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_ENABLE_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	

	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
	    BPP_MBT_LOG("[BTMTK][BPP]Enable success: ");

		//update the bEnabled value of the BPP Status in the SDC
		MBT_LOG1("[BTMTK][BPP]Update sdcBppStatus->bEnabled: from (%d) to (MBT_TRUE)", sdcBppStatus->bEnabled);
	    sdcBppStatus->bEnabled = MBT_TRUE;
		
		//update the State value of the BPP Status in the SDC
		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);

	    //Post the MBTEVT_BPP_ENABLE_SUCCESS event to Application in case of success.
        bt_bpp_mbt_postevent(MBTEVT_BPP_ENABLE_SUCCESS, 0);
    }
	else
	{
	    BPP_MBT_LOG("[BTMTK][BPP]Enable failed:"); 

		//update the bEnabled value of the BPP Status in the SDC
		MBT_LOG1("[BTMTK][BPP]Update sdcBppStatus->bEnabled: from (%d) to (MBT_FALSE)", sdcBppStatus->bEnabled);
	    sdcBppStatus->bEnabled = MBT_FALSE;

		//update the State value of the BPP Status in the SDC
		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_DISABLED);

	    //Post the MBTEVT_BPP_ENABLE_FAIL event to Application in case of failed.
        bt_bpp_mbt_postevent(MBTEVT_BPP_ENABLE_FAIL, 0);
	}
}

void bt_bpp_disable_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    T_MBT_BPP_STATUS *sdcBppStatus;
	
	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISABLE_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	
	
	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
	    //update the bEnabled value of the BPP Status in the SDC
		MBT_LOG1("[BTMTK][BPP]Update sdcBppStatus->bEnabled: from (%d) to (MBT_FALSE)", sdcBppStatus->bEnabled);
	    sdcBppStatus->bEnabled = MBT_FALSE;

		//update the State value of the BPP Status in the SDC
		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_DISABLED);

	    //Post the MBTEVT_BPP_DISABLE_SUCCESS event to Application in case of success.
	    bt_bpp_mbt_postevent(MBTEVT_BPP_DISABLE_SUCCESS, 0);
	}
	else
	{
	    bt_bpp_mbt_postevent(MBTEVT_BPP_DISABLE_FAIL, 0);
	}
}

void bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_CODE cnf_code, bt_bpp_printer_attributes * p_attr)
{
    T_MBT_BPP_STATUS *sdcBppStatus;
    U8 i, copy_len, data_count;

	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_GETATTR_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	

	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
	    MBT_LOG("[BTMTK][BPP]GetPrinterAttributes response : ");
	
        //clear all printer capability value
        memset(&sdcBppStatus->PrinterCapability, 0, sizeof(T_MBT_BPP_CAPABILITY));
        
        copy_len = (MBT_MAX_ATTR_VAL_LEN < BTMTK_BPP_MAX_PRINTER_NAME_LEN) ? MBT_MAX_ATTR_VAL_LEN : BTMTK_BPP_MAX_PRINTER_NAME_LEN;
        memcpy(sdcBppStatus->PrinterCapability.PrinterName, p_attr->capability.printer_name, copy_len);
	    MBT_LOG1("  PrinterName=%s", sdcBppStatus->PrinterCapability.PrinterName);
		
        copy_len = (MBT_MAX_ATTR_VAL_LEN < BTMTK_BPP_MAX_PRINTER_LOCT_LEN) ? MBT_MAX_ATTR_VAL_LEN : BTMTK_BPP_MAX_PRINTER_LOCT_LEN;
        memcpy(sdcBppStatus->PrinterCapability.PrinterLocation, p_attr->capability.printer_location, copy_len);
	    MBT_LOG1("  PrinterLocation=%s", sdcBppStatus->PrinterCapability.PrinterLocation);

        sdcBppStatus->PrinterCapability.PrinterState = (T_MBT_BPP_PRINTER_STATE)p_attr->printer_state; //mapping from MTK definition to MBT
        sdcBppStatus->PrinterCapability.PrinterStateReasons = (T_MBT_BPP_PRINTER_STATE_REASON)p_attr->state_reason;
	    MBT_LOG1("  PrinterState=%s", T_MBT_BPP_PRINTER_STATE_tostring(sdcBppStatus->PrinterCapability.PrinterState));
	    MBT_LOG1("  PrinterStateReason=%s", T_MBT_BPP_PRINTER_STATE_REASON_tostring(sdcBppStatus->PrinterCapability.PrinterStateReasons));

       //Document Formats Supported
        data_count = sdcBppStatus->PrinterCapability.NumDocFmtSupported = p_attr->capability.docfmt_num;
	    MBT_LOG1("  NumDocFmtSupported=%d", sdcBppStatus->PrinterCapability.NumDocFmtSupported);
        for(i=0; i<data_count; i++)
        {
            U8 mime_idx;
            mime_idx = p_attr->capability.doc_format_supported[i];
            memcpy(sdcBppStatus->PrinterCapability.DocFmtSupported[i], bpp_mime_type_table[mime_idx].type_str, strlen((char*)bpp_mime_type_table[mime_idx].type_str));
	    	MBT_LOG1("       [DocFmt]%s", sdcBppStatus->PrinterCapability.DocFmtSupported[i]);
        }

        sdcBppStatus->PrinterCapability.bColorSupported = p_attr->capability.color_supported;
	    MBT_LOG1("  bColorSupported=%d", sdcBppStatus->PrinterCapability.bColorSupported);
        sdcBppStatus->PrinterCapability.SidesSupported = p_attr->capability.sides_supported;
	    MBT_LOG1("  SidesSupported=0x%x", sdcBppStatus->PrinterCapability.SidesSupported);
        sdcBppStatus->PrinterCapability.MaxCopiesSupported = (MBT_SHORT)p_attr->capability.max_copies_supported;
	    MBT_LOG1("  MaxCopiesSupported=%d", sdcBppStatus->PrinterCapability.MaxCopiesSupported);
        sdcBppStatus->PrinterCapability.MaxNumberUp = (MBT_SHORT)p_attr->capability.max_numberup_supported;
	    MBT_LOG1("  MaxNumberUp=%d", sdcBppStatus->PrinterCapability.MaxNumberUp);
        sdcBppStatus->PrinterCapability.OrientationsSupported = p_attr->capability.orientations_supported;
	    MBT_LOG1("  OrientationsSupported=0x%x", sdcBppStatus->PrinterCapability.OrientationsSupported);
        //Media Sizes attribute
        data_count = sdcBppStatus->PrinterCapability.NumMediaSizesSupported = p_attr->capability.mediasize_num;
	    MBT_LOG1("  NumMediaSizesSupported=%d", sdcBppStatus->PrinterCapability.NumMediaSizesSupported);
        for(i=0; i<data_count; i++)
        {
            U8 media_size_idx;
            media_size_idx = p_attr->capability.media_size_supported[i];
            memcpy(sdcBppStatus->PrinterCapability.MediaSizesSupported[i], bpp_media_size_self_desc_name[media_size_idx], strlen((char*)bpp_media_size_self_desc_name[media_size_idx]));
	    	MBT_LOG1("       [MediaSize]%s", sdcBppStatus->PrinterCapability.MediaSizesSupported[i]);
        }
        //Media Types attribute
        data_count = sdcBppStatus->PrinterCapability.NumMediaTypesSupported = p_attr->capability.mediatype_num;
	    MBT_LOG1("  NumMediaTypesSupported=%d", sdcBppStatus->PrinterCapability.NumMediaTypesSupported);
        for(i=0; i<data_count; i++)
        {
            sdcBppStatus->PrinterCapability.MediaTypesSupported[i] = (T_MBT_BPP_MEDIA)p_attr->capability.media_type_supported[i];
	    	MBT_LOG1("       [MediaType]%s", T_MBT_BPP_MEDIA_tostring(sdcBppStatus->PrinterCapability.MediaTypesSupported[i]));
        }
        //Loaded Media Type attribute
        data_count = sdcBppStatus->PrinterCapability.NumLoadedMedia = p_attr->capability.loaded_mediatype_num;
	    MBT_LOG1("  NumLoadedMedia=%d", sdcBppStatus->PrinterCapability.NumLoadedMedia);
        for(i=0; i<data_count; i++)
        {
            sdcBppStatus->PrinterCapability.LoadedMediaType[i] = (T_MBT_BPP_MEDIA)p_attr->capability.loaded_media_type[i];
	    	MBT_LOG1("       [LoadedMediaType]%d", sdcBppStatus->PrinterCapability.LoadedMediaType[i]);
        }

        //Print Quality Supported attribute
        sdcBppStatus->PrinterCapability.PrintQualitySupported = p_attr->capability.print_quality_supported;
	    MBT_LOG1("  PrintQualitySupported=0x%x", sdcBppStatus->PrinterCapability.PrintQualitySupported);

       //Image Formats Supported
        data_count = sdcBppStatus->PrinterCapability.NumImageFmtSupported = p_attr->capability.imgfmt_num;
	    MBT_LOG1("  NumImageFmtSupported=%d", sdcBppStatus->PrinterCapability.NumImageFmtSupported);
        for(i=0; i<data_count; i++)
        {
            U8 mime_idx;
            mime_idx = p_attr->capability.image_format_supported[i];
            memcpy(sdcBppStatus->PrinterCapability.ImageFmtSupported[i], bpp_mime_type_table[mime_idx].type_str, strlen((char*)bpp_mime_type_table[mime_idx].type_str));
	    	MBT_LOG1("       [ImageFmt]%s", sdcBppStatus->PrinterCapability.ImageFmtSupported[i]);
        }
        
        //Queue Job Count attribute
        sdcBppStatus->PrinterCapability.QueuedJobCount = p_attr->capability.queued_job_count;
	    MBT_LOG1("  QueuedJobCount=%d", sdcBppStatus->PrinterCapability.QueuedJobCount);
        //Basic Text Page Width attribute
        sdcBppStatus->PrinterCapability.BasicTextPageWidth = (MBT_SHORT)p_attr->capability.basic_text_page_width;
	    MBT_LOG1("  BasicTextPageWidth=%d", sdcBppStatus->PrinterCapability.BasicTextPageWidth);
        //Basic Text Page Heigh attribute
        sdcBppStatus->PrinterCapability.BasicTextPageHeight = (MBT_SHORT)p_attr->capability.basic_text_page_height;
	    MBT_LOG1("  BasicTextPageHeight=%d", sdcBppStatus->PrinterCapability.BasicTextPageHeight);
        //Operation Status attribute
        sdcBppStatus->PrinterCapability.OperationStatus = p_attr->operation_status;
	    MBT_LOG1("  OperationStatus=%d", sdcBppStatus->PrinterCapability.OperationStatus);
        //Printer General Current Operator attribute
        copy_len = (MBT_MAX_ATTR_VAL_LEN < BPP_MAX_USER_NAME_LEN) ? MBT_MAX_ATTR_VAL_LEN : BPP_MAX_USER_NAME_LEN;
        memcpy(sdcBppStatus->PrinterCapability.PrinterGeneralCurrentOperator, p_attr->capability.printer_general_current_operator, copy_len);
	    MBT_LOG1("  PrinterGeneralCurrentOperator=%s", sdcBppStatus->PrinterCapability.PrinterGeneralCurrentOperator);

		/* update sdc state */
		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
		
        bt_bpp_mbt_postevent(MBTEVT_BPP_GET_PRINT_ATTR_SUCCESS, 0);

		return;
	}
	else
	{
		BPP_MBT_LOG("[BTMTK][BPP] GetPrinterAttributes failed:");
		
		switch (cnf_code)
		{
			case BTMTK_BPP_CNF_ERR_RESP:
				UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
				break;
				
			case BTMTK_BPP_CNF_CONN_FAILED:
			case BTMTK_BPP_CNF_DISCONN_IND:
				UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
				break;
				
			default:
				break;
		}
		
		MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
        sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bt_bpp_mbt_postevent(MBTEVT_BPP_GET_PRINT_ATTR_FAIL, 0);
	}
}

void bt_bpp_print_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    T_MBT_BPP_STATUS *sdcBppStatus;

	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_PRINT_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	

	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
		BPP_MBT_LOG("[BTMTK][BPP]Print success:");

		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);

        bt_bpp_mbt_postevent(MBTEVT_BPP_PRINT_COMPLETE_SUCCESS, 0);
	}
	else
	{
		BPP_MBT_LOG("[BTMTK][BPP]Print failed:");

		switch (cnf_code)
		{
			case BTMTK_BPP_CNF_ERR_RESP:
				UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
				break;

			case BTMTK_BPP_CNF_CONN_FAILED:
			case BTMTK_BPP_CNF_DISCONN_IND:
				UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
				break;

			default:
				break;
		}
	
		MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
        sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bt_bpp_mbt_postevent(MBTEVT_BPP_PRINT_COMPLETE_FAIL, 0);
	}
}

void bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    T_MBT_BPP_STATUS *sdcBppStatus;

	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISCONNECT_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	

	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
		BPP_MBT_LOG("[BTMTK][BPP] DISCONNECT success:");

		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);

        bt_bpp_mbt_postevent(MBTEVT_BPP_DISCONNECT_SUCCESS, 0);
	}
	else
	{
		BPP_MBT_LOG("[BTMTK][BPP] DISCONNECT failed:");
		
		if (cnf_code == BTMTK_BPP_CNF_ERR_RESP)
		{
			UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
		}
		
		MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
		sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bt_bpp_mbt_postevent(MBTEVT_BPP_DISCONNECT_FAIL, 0);
	}
}

void bt_bpp_connect_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    T_MBT_BPP_STATUS *sdcBppStatus;

	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
		BPP_MBT_LOG("[BTMTK][BPP] CONNECT success:");

		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);

        //bt_bpp_mbt_postevent(MBTEVT_BPP_CONNECT_SUCCESS, 0);
	}
	else
	{
		BPP_MBT_LOG("[BTMTK][BPP] CONNECT failed:");

		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
		//sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        //bt_bpp_mbt_postevent(MBTEVT_BPP_CONNECT_FAIL, 0);
	}
}

void bt_bpp_cancel_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    T_MBT_BPP_STATUS *sdcBppStatus;

	sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    bt_trace(TRACE_GROUP_9, BT_BPP_EXT_CANCEL_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	

	if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
	{
		BPP_MBT_LOG("[BTMTK][BPP] CANCEL success");

        bt_bpp_mbt_postevent(MBTEVT_BPP_CANCEL_SUCCESS, 0);
	}
	else
	{
		BPP_MBT_LOG("[BTMTK][BPP] CANCEL failed");

		MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
		sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bt_bpp_mbt_postevent(MBTEVT_BPP_CANCEL_FAIL, 0);
	}
}

void bt_bpp_authentication_ind(bt_bpp_obex_auth_chal_info *chal_info)
{
    T_MBT_BPP_STATUS *sdcBppStatus = 
        (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

	g_bt_bpp_cntx.auth_flag |= MTKBPP_AUTH_RECV_CHAL;

    /* update the bIsObexUserID value of the BPP Status in the SDC 
    	according as the User ID is required or not
     */	
    if (chal_info->options & BPP_AUTH_OPTION_FLAG_USERID_REQ)
    {
    	sdcBppStatus->bIsObexUserID = MBT_TRUE;
    }
	else
    {
        sdcBppStatus->bIsObexUserID = MBT_FALSE;
    }

	MBT_LOG1("[BTMTK][BPP]OBEX Authentication request: bIsObexUserID = %d", sdcBppStatus->bIsObexUserID);
	
    //post the MBTEVT_BPP_OBEX_AUTHREQ event to Application
    bt_bpp_mbt_postevent(MBTEVT_BPP_OBEX_AUTHREQ, 0);
}

void bt_bpp_print_progress_ind(U32 sent_len, U32 total_len)
{
	U32 tx_size;
    T_MBT_BPP_STATUS *sdcBppStatus = 
        (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

	BPP_MBT_LOG2("[BTMTK][BPP]Progress Indication: sent_len=%d, total_len=%d", sent_len, total_len);
		
	if ((total_len > 0) && (sent_len < total_len))
	{
		tx_size = (U32)((sent_len*g_bt_bpp_cntx.file_size)/total_len);
	}
	else
	{
		tx_size = g_bt_bpp_cntx.file_size;
	}
	
	//if (tx_size > 0)
	{
		U32 old_size = (U32)sdcBppStatus->TxProgress;
		
	    //MBT_LOG("[BTMTK][BPP]Progress Indication: ");
		MBT_LOG2("[BTMTK][BPP]Update sdcBppStatus->TxProgress: from (%d) to (%d)", old_size, tx_size);

		//update the TxProgress/State value of the BPP Status in the SDC
		sdcBppStatus->TxProgress = tx_size;

		UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_PRINTING);

		//post the MBTEVT_BPP_PROGRESS event to Application
	    bt_bpp_mbt_postevent(MBTEVT_BPP_PROGRESS, 0);
	}

    //send the progress rsp to internal adaptor
    bt_bpp_progress_rsp();
}

void bt_bpp_job_status_ind(bt_bpp_job_status * job_sts)
{
    T_MBT_BPP_STATUS *sdcBppStatus = 
        (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    //update the JobStatus value of the BPP Status in the SDC
    sdcBppStatus->PrintingFile.JobStatus.PrintJobState = (T_MBT_BPP_PRINT_JOB_STATE)job_sts->job_state;
    sdcBppStatus->PrintingFile.JobStatus.PrinterState = (T_MBT_BPP_PRINTER_STATE)job_sts->printer_state;
    sdcBppStatus->PrintingFile.JobStatus.PrinterStateReasons = (T_MBT_BPP_PRINTER_STATE_REASON)job_sts->state_reason;

    MBT_LOG3("[BTMTK][BPP]Job Status indication: PrintJobState=%s, PrinterState=%s, PrinterStateReasons=%s",
						T_MBT_BPP_PRINT_JOB_STATE_tostring(sdcBppStatus->PrintingFile.JobStatus.PrintJobState),
						T_MBT_BPP_PRINTER_STATE_tostring(sdcBppStatus->PrintingFile.JobStatus.PrinterState),
						T_MBT_BPP_PRINTER_STATE_REASON_tostring(sdcBppStatus->PrintingFile.JobStatus.PrinterStateReasons));

    //post the MBTEVT_BPP_PRINT_STATUS event to Application
    bt_bpp_mbt_postevent(MBTEVT_BPP_PRINT_STATUS, 0);
}

void bt_bpp_update_sdc_state(T_MBT_BPP_STATE new_state)
{
    T_MBT_BPP_STATUS *sdcBppStatus = 
        (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

	UPDATE_SDCBPP_STATE(sdcBppStatus, new_state);
}

void bt_bpp_update_sdc_state_addr(T_MBT_BPP_STATE state, T_MBT_BDADDR addr)
{
    T_MBT_BPP_STATUS *sdcBppStatus = 
        (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

	UPDATE_SDCBPP_STATE(sdcBppStatus, state);
		
	MBT_LOG("[BTMTK][BPP]Update sdcBppStatus->BDAdd");
	memcpy(sdcBppStatus->BDAddr, addr, sizeof(T_MBT_BDADDR));
}

void bt_bpp_update_sdc_state_addr_printobj(T_MBT_BPP_STATE state, T_MBT_BDADDR addr, T_MBT_BPP_OBJECT *MBTObject)
{
    T_MBT_BPP_STATUS *sdcBppStatus = 
        (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

	UPDATE_SDCBPP_STATE(sdcBppStatus, state);
	
	MBT_LOG("[BTMTK][BPP]Update sdcBppStatus->BDAdd");
	memcpy(sdcBppStatus->BDAddr, addr, sizeof(T_MBT_BDADDR));

	MBT_LOG("[BTMTK][BPP]Update sdcBppStatus->PrintingFile");
	memcpy(&(sdcBppStatus->PrintingFile), MBTObject, sizeof(T_MBT_BPP_OBJECT));
}



