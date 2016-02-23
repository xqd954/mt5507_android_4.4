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
 * Bt_bpp_portng.c
 *
 * Project:
 * --------
 * YuSu
 *
 * Description:
 * ------------
 * This file is used to
 *
 * Author:
 * -------
 * Paul Chuang
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
#define LOG_TAG "BTEXTADP_BPP"

#include "bt_message.h"
#include "bt_mmi.h"

#include "bluetooth_bpp_struct.h"
#include "bluetooth_bpp_message.h"

#include "bt_bpp_porting.h"

#ifdef  __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_ext_debug.h"
#else
#include <cutils/xlog.h>
#endif
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>


#define BPP_PORTING_LAYER_DEBUG 1

#ifdef BPP_PORTING_LAYER_DEBUG
#define BPP_DBG(fmt, ...) XLOGD("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)
#else
#define BPP_DBG(fmt, ...) {}
#endif
#define BPP_ERR(fmt, ...) XLOGE("[%s] %s(%d):" fmt, __FILE__, __FUNCTION__, __LINE__,  ## __VA_ARGS__)


/********************************************************************************
*
********************************************************************************/
static int g_apisock;
BTMTK_BPP_JNI_CALLBACK g_bpp_jni_callback = NULL;


BT_BOOL btmtk_bpp_set_jni_callback(BTMTK_BPP_JNI_CALLBACK pfCallback, int apisock)
{
    g_bpp_jni_callback = pfCallback;
    g_apisock = apisock;
    return TRUE;
}


void bpp_postevent(U8 event, void* parameters, U8 count)
{
    if( NULL !=  g_bpp_jni_callback)
    {
        g_bpp_jni_callback(event, parameters, count);
    }
}


void BTCMD_SendMessage(msg_type msg_id, module_type dest_mod, void *ptr, U16 size)
{
    int ret = -1;
    int sockfd = -1;

    BPP_DBG("BTCMD_SendMessage msg=%u, ptr=0x%X, size=%d", msg_id, (unsigned int)ptr, size);


    U16 *field;

    ilm_struct ilm;

    memset(&ilm, 0x0, sizeof(ilm_struct));
    ilm.msg_id = msg_id;
    ilm.sap_id = 0;
    ilm.src_mod_id = MOD_MMI;
    ilm.dest_mod_id = MOD_BT;

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

    sockfd = g_apisock;
    //if(GOPP(apisock))

    //mtk03036
    //if(sockfd )
    if(sockfd >= 0 )
    {
        //ret = send(GOPP(apisock), (void *)&ilm, size, 0);
        //ret = send(sockfd, ptr, size, 0);
        ret = send(sockfd, (void*)&ilm, size, 0);
        if(ret < 0)
        {
            BPP_ERR("send msg fail : %s, %d", strerror(errno), errno);
        }
        else
        {
            BPP_DBG("send msg success : %d", ret);
        }
    }
    else
    {
        BPP_ERR("socket uninitialized");
    }
}
/********************************************************************************
*
********************************************************************************/

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
#define MTKBPP_AUTH_NONE        0x00
#define MTKBPP_AUTH_RECV_CHAL  	0x01
#define MTKBPP_AUTH_SENT_RESP  	0x02

typedef struct
{
    BTMTK_BPP_STATE state; /* current state */
    S32 hprinter;          /* printer handle */
    U8 printer_addr[6];    /* LSB */
    U8 auth_flag;          /* flag: BTMTK_BPP_AUTH_xxx */
    U32 file_size;         /* original file size:
                           if the file is composed into xhtml, then the data size will be larger.
                           So record the original file size for use in progress indication. */
    BTMTK_BPP_OPERATION  oper_type;  /* operation type */
    union
    {
        U32  bitmask_attr;           /* for GetPrinterAttributes */
        T_BPP_OBJECT print_object;   /* for print file */
    }oper;
}bt_bpp_cntx_struct;

static void bt_bpp_connect(U8 *btAddr);
static void bt_bpp_getprinterattribute(U32 bitmask_attr);
static S16 bt_bpp_print(T_BPP_OBJECT *BipObject);
static void bt_bpp_send_disconnect_req(void);


static void bt_bpp_enable_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
static void bt_bpp_disable_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
static void bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_CODE cnf_code, bt_bpp_printer_attributes * p_attr);
static void bt_bpp_print_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
static void bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
static void bt_bpp_connect_req_cb(BTMTK_BPP_CNF_CODE cnf_code);
static void bt_bpp_cancel_req_cb(BTMTK_BPP_CNF_CODE cnf_code);

#if defined (BTMTK_ON_WISE)
S32 ext_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size,const U8 *src);
extern int wsConvertFileNameToNativeFormat(unsigned short *targetBuff_p, U8* fileName);
#endif

// in bpp_data.c
//extern const kal_char* bpp_media_size_self_desc_name[];
//extern bpp_mime_id_str_struct bpp_mime_type_table[];
const kal_char* bpp_media_size_self_desc_name[BPP_MEDIA_SIZE_MAX_ENUM]=
{
  "iso_a10_26x37mm",
  "iso_a9_37x52mm",
  "iso_a8_52x74mm",
  "iso_a7_74x105mm",
  "iso_a6_105x148mm",
  "iso_a5_148x210mm",
  "iso_a5-extra_174x235mm",
  "iso_a4_210x297mm",
  "iso_a4-tab_225x297mm",
  "iso_a4-extra_235.5x322.3mm",
  "iso_a3_297x420mm",
  "iso_a2_420x594mm",
  "iso_a1_594x841mm",
  "iso_a0_841x1189mm",
  "iso_2a0_1189x1682mm",
  "iso_b10_31x44mm",
  "iso_b9_44x62mm",
  "iso_b8_62x88mm",
  "iso_b7_88x125mm",
  "iso_b6_125x176mm",
  "iso_b6c4_125x324mm",
  "iso_b5_176x250mm",
  "iso_b5-extra_201x276mm",
  "iso_b4_250x353mm",
  "iso_b3_353x500mm",
  "iso_b2_500x707mm",
  "iso_b1_707x1000mm",
  "iso_b0_1000x1414mm",
  "iso_c10_28x40mm",
  "iso_c9_40x57mm",
  "iso_c8_57x81mm",
  "iso_c7_81x114mm",
  "iso_c7c6_81x162mm",
  "iso_c6_114x162mm",
  "iso_c6c5_114x229mm",
  "iso_c5_162x229mm",
  "iso_c4_229x324mm",
  "iso_c3_324x458mm",
  "iso_c2_458x648mm",
  "iso_c1_648x917mm",
  "iso_c0_917x1297mm",
  "na_index-4x6_4x6in",
  "na_letter_8.5x11in"
};

bpp_mime_id_str_struct bpp_mime_type_table[BPP_MIME_TYPE_MAX_ENUM + 1] =
{
{BPP_MIME_TYPE_UNKNOWN, ""},
{BPP_MIME_APPLICATION_XHTML_PRINT_095,"application/vnd.pwg-xhtml-print+xml:0.95"},
{BPP_MIME_APPLICATION_XHTML_PRINT_100,"application/vnd.pwg-xhtml-print+xml:1.0"},
{BPP_MIME_APPLICATION_MULTIPLEXED,    "application/vnd.pwg-multiplexed"},
{BPP_MIME_TEXT_PLAIN,      "text/plain"},
{BPP_MIME_TEXT_VCARD,      "text/x-vcard:2.1"},
{BPP_MIME_TEXT_VCARD30,    "text/x-vcard:3.0"},
{BPP_MIME_TEXT_VCALENDAR,  "text/x-vcalendar:1.0"},
{BPP_MIME_TEXT_ICALENDAR20,"text/calendar:2.0"},
{BPP_MIME_TEXT_VMESSAGE,   "text/x-vmessage:1.1"},
{BPP_MIME_TEXT_VNOTE,      "text/x-vnote:1.1"},
{BPP_MIME_IMAGE_JPEG,"image/jpeg"},
{BPP_MIME_IMAGE_GIF, "image/gif"},
{BPP_MIME_IMAGE_BMP, "image/bmp"},
{BPP_MIME_IMAGE_WBMP,"image/vnd.wap.wbmp"},
{BPP_MIME_IMAGE_PNG, "image/png"},
{BPP_MIME_IMAGE_SVG, "image/svg+xml"},
{BPP_MIME_TYPE_MAX_ENUM, NULL}
};


// global context 
static bt_bpp_cntx_struct g_bt_bpp_cntx;

// The bpp connection handle seed. Used to generate the new bpp connection handle.
static int bpp_printer_handle_seed = 0;


//AUX function
//-------------------------------------------------------------------------------
static U32 bpp_utf8_to_ucs2_char(U16 *ucs2, const U8 * utf8)
{
    U8 c = utf8[0];

    if (c < 0x80)
    {
        *ucs2 = c;
        return 1;
    }
    else if (c < 0xe0)
    {
        *ucs2 = ((U16) (c & 0x1f) << 6) | (U16) (utf8[1] ^ 0x80);
        return 2;
    }
    else
    {
        *ucs2 = ((U16) (c & 0x0f) << 12) | ((U16) (utf8[1] ^ 0x80) << 6) | (U16) (utf8[2] ^ 0x80);
        return 3;
    }
}


U32 bpp_utf8_to_ucs2_string(U8 *dest, U32 dest_size, const U8 * src)
{
    U32 pos = 0;

    while (*src)
    {
        src += bpp_utf8_to_ucs2_char((U16*) (dest + pos), src);
        pos += 2;
        if (pos >= dest_size - 2)
        {
            break;
        }
    }
    dest[pos] = 0;
    dest[pos + 1] = 0;
    return pos + 2;
}
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//BPP Sender INTERNAL START -----------------------------------------------------
//#define BPP_INTERNAL_FUNC------------------------------------------------------
//-------------------------------------------------------------------------------
static void bt_bpp_reset_cntx(void)
{
    BPP_DBG("+");

    memset (&g_bt_bpp_cntx, 0, sizeof(g_bt_bpp_cntx));

    BPP_DBG("-");
}

static void bt_bpp_init(void)
{
    BPP_DBG("+");

    bpp_printer_handle_seed = 0;
    bt_bpp_reset_cntx();

    BPP_DBG("-");
}

static void bt_bpp_deinit(void)
{
    BPP_DBG("+");

    bt_bpp_init();

    BPP_DBG("-");
}


static void bt_bpp_reset_cntx_to_enabled(void)
{
    BPP_DBG("+");

    memset (&g_bt_bpp_cntx, 0, sizeof(g_bt_bpp_cntx));
    g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;

    BPP_DBG("-");
}

//-------------------------------------------------------------------------------
//BPP Sender INTERNAL(CNF, RSP, IND) START --------------------------------------
//-------------------------------------------------------------------------------

void bt_bpp_connect_cnf(BPP_ERROR_CODE bpp_conn_result)
{
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_CONN_CNF, g_bt_bpp_cntx.state, bpp_conn_result);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] bpp_conn_result(%d)", g_bt_bpp_cntx.state, bpp_conn_result);

    if(bpp_conn_result != BPP_SUCCESS)
    {
        //callback
    	bt_bpp_connect_req_cb(BTMTK_BPP_CNF_ERR_RESP);

        if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_GETATTR)
        {
            // the connection is for get printer attribute.
            // need reset flag and post fail event when connection fail.
            bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_CONN_FAILED, NULL);
        }
        if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_PRINT)
        {
            //the connection is for print object
            //need reset flag and post fail event when connection fail
            bt_bpp_print_req_cb(BTMTK_BPP_CNF_CONN_FAILED);
        }

	//reset cntx to enabled state
        bt_bpp_reset_cntx_to_enabled();
    }
    else
    {
        // CONNECT success:  update bpp state
        g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;

        //update bpp state
        bt_bpp_connect_req_cb(BTMTK_BPP_CNF_SUCCESS);

        // execute next operation
        if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_GETATTR)
	{
            bt_bpp_getprinterattribute(g_bt_bpp_cntx.oper.bitmask_attr);
        }
        else if(g_bt_bpp_cntx.oper_type == MTKBPP_OPER_PRINT)
        {
            S16 ret = bt_bpp_print(&g_bt_bpp_cntx.oper.print_object);
            if (ret < 0)
            {
                // FAILED
                bt_bpp_print_req_cb(BTMTK_BPP_CNF_ERR_RESP);
            }
        }

        // clear the flag
        g_bt_bpp_cntx.oper_type = MTKBPP_OPER_NONE;
    }
    BPP_DBG("-");
}


void bt_bpp_getprinterattribute_cnf(BPP_ERROR_CODE bpp_getatt_result, bt_bpp_printer_attributes* p_attr)
{
    BTMTK_BPP_CNF_CODE cnf_code;

    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_GET_ATT_CNF, g_bt_bpp_cntx.state, bpp_getatt_result);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] bpp_getatt_result(%d)", g_bt_bpp_cntx.state, bpp_getatt_result);
    
    if (bpp_getatt_result == BPP_SUCCESS)
    {
        cnf_code = BTMTK_BPP_CNF_SUCCESS;
    }
    else
    {
        cnf_code = BTMTK_BPP_CNF_ERR_RESP;
    }

    // callback
    bt_bpp_getprinterattr_req_cb(cnf_code, p_attr);

    // update state
    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_GETTING_ATTR:
            //update bpp state
            g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
            break;
 
        default:
            break;
    }
    BPP_DBG("-");
}


void bt_bpp_print_cnf(BPP_ERROR_CODE bpp_print_result)
{
    BTMTK_BPP_CNF_CODE cnf_code;

    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] bpp_print_result(%d)", g_bt_bpp_cntx.state, bpp_print_result);

    if (bpp_print_result == BPP_SUCCESS)
    {
        cnf_code = BTMTK_BPP_CNF_SUCCESS;
    }
    else
    {
        cnf_code = BTMTK_BPP_CNF_ERR_RESP;
    }

    // callback
    bt_bpp_print_req_cb(cnf_code);

    // update state 
    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_PRINTING:
            //update bpp state
            g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
            break;

        default:
            break;
    }
    BPP_DBG("-");
}


void bt_bpp_disconnect_cnf(BPP_ERROR_CODE bpp_disc_result)
{
    BTMTK_BPP_CNF_CODE cnf_code;

    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] bpp_disc_result(%d)", g_bt_bpp_cntx.state, bpp_disc_result);

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
        // disabling
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

    // callback : CANCEL & DISCONNECT event
    if(g_bt_bpp_cntx.state == MTKBPP_STATE_CANCELING)
    {
        bt_bpp_cancel_req_cb(cnf_code);
    }

    // reset cntx to enabled state
    bt_bpp_reset_cntx_to_enabled();

    bt_bpp_disconnect_req_cb(cnf_code);

    
    //bt_bpp_reset_cntx_to_enabled();
    BPP_DBG("-");
}


void bt_bpp_progress_rsp(void)
{
    BPP_DBG("+");
    bt_bpp_progress_rsp_struct msg;

    msg.hprinter = g_bt_bpp_cntx.hprinter;
    
    BTCMD_SendMessage(MSG_ID_BT_BPP_PROGRESS_RSP, MOD_BT, &msg, sizeof(bt_bpp_progress_rsp_struct));
    BPP_DBG("-");
}


void bt_bpp_disconnect_ind(void)
{
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISC_IND, g_bt_bpp_cntx.state);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)]", g_bt_bpp_cntx.state);

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

    // callback
    bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_SUCCESS);

    //reset cntx to enabled state
    bt_bpp_reset_cntx_to_enabled();
    BPP_DBG("-");
}


void bt_bpp_authentication_ind(bt_bpp_obex_auth_chal_info *chal_info)
{
    BPP_DBG("+");
    //T_MBT_BPP_STATUS *sdcBppStatus = (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    g_bt_bpp_cntx.auth_flag |= MTKBPP_AUTH_RECV_CHAL;

    // update the bIsObexUserID value of the BPP Status in the SDC 
    //according as the User ID is required or not
    S8* para[1];
    S8 para1[1];

    if (chal_info->options & BPP_AUTH_OPTION_FLAG_USERID_REQ)
    {
        //sdcBppStatus->bIsObexUserID = MBT_TRUE;
        *para1 = 1;
    }
    else
    {
        //sdcBppStatus->bIsObexUserID = MBT_FALSE;
        *para1 = 0;
    }
    para[0] = para1;

    //MBT_LOG1("[BTMTK][BPP]OBEX Authentication request: bIsObexUserID = %d", sdcBppStatus->bIsObexUserID);
    //post the MBTEVT_BPP_OBEX_AUTHREQ event to Application
    bpp_postevent(BPP_OBEX_AUTHREQ, para, 1);
    BPP_DBG("-");
}


void bt_bpp_print_progress_ind(U32 sent_len, U32 total_len)
{
    U32 tx_size;
    //T_MBT_BPP_STATUS *sdcBppStatus = (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    //BPP_MBT_LOG2("[BTMTK][BPP]Progress Indication: sent_len=%d, total_len=%d", sent_len, total_len);
    BPP_DBG("+");
    BPP_DBG("sent_len(%lu), total_len(%lu)", sent_len, total_len);

    if ((total_len > 0) && (sent_len < total_len))
    {
        tx_size = (U32)((sent_len*g_bt_bpp_cntx.file_size)/total_len);
    }
    else
    {
        tx_size = g_bt_bpp_cntx.file_size;
    }

    //U32 old_size = (U32)sdcBppStatus->TxProgress;
    //MBT_LOG2("[BTMTK][BPP]Update sdcBppStatus->TxProgress: from (%d) to (%d)", old_size, tx_size);
    //BPP_DBG("TxProgress: from (%d) to (%d)", old_size, tx_size);

    //update the TxProgress/State value of the BPP Status in the SDC
    //sdcBppStatus->TxProgress = tx_size;
    //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_PRINTING);
    g_bt_bpp_cntx.state = MTKBPP_STATE_PRINTING;

    //post the MBTEVT_BPP_PROGRESS event to Application
    S8* para[2];
    S8 sent[MAX_DIGIT_SIZE];
    S8 total[MAX_DIGIT_SIZE];

    sprintf(sent, "%lu", sent_len );
    BPP_DBG("sent_len: %s", sent);
    sprintf(total, "%lu", total_len);
    BPP_DBG("total_len: %s", total);

    para[0] = sent;
    para[1] = total;


    bpp_postevent(BPP_PROGRESS, para, 2);

    //send the progress rsp to internal adaptor
    bt_bpp_progress_rsp();
    BPP_DBG("-");
}


void bt_bpp_job_status_ind(bt_bpp_job_status * job_sts)
{
    //T_MBT_BPP_STATUS *sdcBppStatus = (T_MBT_BPP_STATUS  *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    //update the JobStatus value of the BPP Status in the SDC
    //sdcBppStatus->PrintingFile.JobStatus.PrintJobState = (T_MBT_BPP_PRINT_JOB_STATE)job_sts->job_state;
    //sdcBppStatus->PrintingFile.JobStatus.PrinterState = (T_MBT_BPP_PRINTER_STATE)job_sts->printer_state;
    //sdcBppStatus->PrintingFile.JobStatus.PrinterStateReasons = (T_MBT_BPP_PRINTER_STATE_REASON)job_sts->state_reason;

    BPP_DBG("+");

    //MBT_LOG3("[BTMTK][BPP]Job Status indication: PrintJobState=%s, PrinterState=%s, PrinterStateReasons=%s",
    //    T_MBT_BPP_PRINT_JOB_STATE_tostring(sdcBppStatus->PrintingFile.JobStatus.PrintJobState),
    //    T_MBT_BPP_PRINTER_STATE_tostring(sdcBppStatus->PrintingFile.JobStatus.PrinterState),
    //    T_MBT_BPP_PRINTER_STATE_REASON_tostring(sdcBppStatus->PrintingFile.JobStatus.PrinterStateReasons));

    //post the MBTEVT_BPP_PRINT_STATUS event to Application
    S8* para[3];
    S8 jobState[MAX_DIGIT_SIZE];
    S8 printerState[MAX_DIGIT_SIZE];
    S8 printerStateReasons[MAX_DIGIT_SIZE];

    sprintf(jobState, "%d", (T_BPP_PRINT_JOB_STATE)job_sts->job_state );
    BPP_DBG("jobState: %s", jobState);
    sprintf(printerState, "%d",(T_BPP_PRINTER_STATE)job_sts->printer_state);
    BPP_DBG("printerState: %s", printerState);
    sprintf(printerStateReasons, "%d",(T_BPP_PRINTER_STATE_REASON)job_sts->state_reason);
    BPP_DBG("printerStateReason: %s", printerStateReasons);

    para[0] = jobState;
    para[1] = printerState;
    para[2] = printerStateReasons;


    bpp_postevent(BPP_PRINT_STATUS, para, 3);
    BPP_DBG("-");
}


//-------------------------------------------------------------------------------
//BPP Sender INTERNAL(CNF, RSP, IND) END-----------------------------------------
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//BPP Sender INTERNAL(REQ) START-------------------------------------------------
//-------------------------------------------------------------------------------

static void bt_bpp_connect(U8 *btAddr)
{
    bt_bpp_connect_req_struct msg;

    //BPP_MBT_LOG("[BTMTK][BPP]send CONNECT request ");
    BPP_DBG("+ send CONNECT request");
    BPP_DBG("BtAddr(0x%x 0x%x 0x%x 0x%x 0x%x 0x%x)", btAddr[0],btAddr[1], btAddr[2], btAddr[3], btAddr[4], btAddr[5]);

    g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTING;

    //backup the printer bd addr
    memcpy(g_bt_bpp_cntx.printer_addr, btAddr, 6);

    //transfrom LSB to MSB
    //btmtk_util_convert_mbtarray2mtkarray(msg.addr.addr, btAddr);
    memcpy(msg.addr.addr, btAddr, 6);

    //get the printer handler
    g_bt_bpp_cntx.hprinter = msg.hprinter = bpp_printer_handle_seed++;
    BTCMD_SendMessage(MSG_ID_BT_BPP_CONNECT_REQ, MOD_BT, &msg, sizeof(bt_bpp_connect_req_struct));

    BPP_DBG("-");
}


static void bt_bpp_getprinterattribute(U32 bitmask_attr)
{
    bt_bpp_get_printer_attr_req_struct msg;

    //BPP_MBT_LOG1("[BTMTK][BPP]send GetPrinterAttributes request, bitmast_attr=0x%x", bitmask_attr);
    BPP_DBG("+ send GetPrinterAttributes request");
    BPP_DBG("bitmast_attr=0x%lu", bitmask_attr);

    g_bt_bpp_cntx.state = MTKBPP_STATE_GETTING_ATTR;


    //bt_bpp_update_sdc_state(MBT_BPP_STATE_GETATTRIBUTE);

    msg.attr_bitmask = bitmask_attr;
    msg.hprinter = g_bt_bpp_cntx.hprinter;
        
    BTCMD_SendMessage(MSG_ID_BT_BPP_GET_PRINTER_ATTR_REQ, MOD_BT, &msg, sizeof(bt_bpp_get_printer_attr_req_struct));
    BPP_DBG("-");
}


static S16 bt_bpp_print(T_BPP_OBJECT *BppObject)
{
    I8 i;
    U8 filename[BTMTK_MAX_FILE_NAME_LEN];
#if defined (BTMTK_ON_WISE)
    //MBT target case
    U16 *ucs2filename = NULL;
    U16 ucs2name_len;
#endif
    bt_bpp_print_doc_req_struct msg;
    S32 dirlen, namelen;


    // BPP_MBT_LOG("[BTMTK][BPP]send PRINT DOCUMENT request: ");
    // BPP_MBT_LOG2("    DirName=%s, FileName=%s", BppObject->DirName, BppObject->FileName);
    // BPP_MBT_LOG3("    MIME=%s, ObjectSize=%d, bJobBasePrinting=%d",
    //              T_MBT_MIME_MEDIA_tostring(BppObject->MimeType), BppObject->ObjectSize, BppObject->bJobBasedPrinting);
    // BPP_MBT_LOG3("    Copies=%d, NumberUp=%d, Sides=%d",
    //              BppObject->PrintingAttribute.Copies, BppObject->PrintingAttribute.NumberUp, BppObject->PrintingAttribute.Sides);
    // BPP_MBT_LOG3("    Orient=%d, Quality=%d, DocFmt=%s",
    //              BppObject->PrintingAttribute.Orient, BppObject->PrintingAttribute.Quality, BppObject->PrintingAttribute.DocFmt);
    // BPP_MBT_LOG2("    MeidaSize=%s, MediaType=%s",
    //              BppObject->PrintingAttribute.MediaSize, T_MBT_BPP_MEDIA_tostring(BppObject->PrintingAttribute.MediaType));
    BPP_DBG("+ send PRINT DOCUMENT request: ");
    BPP_DBG("    DirName=%s, FileName=%s", BppObject->DirName, BppObject->FileName);
    BPP_DBG("    ObjectSize=%lu, bJobBasePrinting=%d", BppObject->ObjectSize, BppObject->bJobBasedPrinting);
    BPP_DBG("    Copies=%d, NumberUp=%d, Sides=%d", 
                  BppObject->PrintingAttribute.Copies, BppObject->PrintingAttribute.NumberUp, BppObject->PrintingAttribute.Sides);
    BPP_DBG("    Orient=%d, Quality=%d, DocFmt=%s",
                  BppObject->PrintingAttribute.Orient, BppObject->PrintingAttribute.Quality, BppObject->PrintingAttribute.DocFmt);
    BPP_DBG("    MeidaSize=%s", BppObject->PrintingAttribute.MediaSize);
	

    g_bt_bpp_cntx.state = MTKBPP_STATE_PRINTING;

    //update the State value of the BPP Status in the SDC
    //abt_bpp_update_sdc_state(MBT_BPP_STATE_PRINTING);
	
    // set msg structure 
    memset(&msg, 0, sizeof(bt_bpp_print_doc_req_struct));
    memset(filename, 0, BTMTK_MAX_FILE_NAME_LEN);
    msg.hprinter = g_bt_bpp_cntx.hprinter;
	
    //Transform T_MBT_BPP_OBJECT to bt_bpp_object
    dirlen = strlen(BppObject->DirName);
    namelen = strlen(BppObject->FileName);
    if(BTMTK_MAX_FILE_NAME_LEN < dirlen)
    {
        return -1;
    }
    memcpy(filename, BppObject->DirName, dirlen);
    if((BTMTK_MAX_FILE_NAME_LEN - dirlen) < namelen)
    {
        return -1;
    }
	
#if defined (BTMTK_ON_WISE)
    ucs2name_len = BTMTK_MAX_FILE_NAME_LEN * 2;
    ucs2filename = get_ctrl_buffer(ucs2name_len);

    //MBT target case
    memcpy(filename + dirlen, BppObject->FileName, namelen);
    //ext_chset_utf8_to_ucs2_string((U8 *)ucs2filename, ucs2name_len, filename);
    bpp_utf8_to_ucs2_string((U8*)msg.print_object.file_name, sizeof(msg.print_object.file_name), filename);
    wsConvertFileNameToNativeFormat((U16*)msg.print_object.file_name, (U8*)ucs2filename);
    free_ctrl_buffer(ucs2filename);
#else
    *(filename + dirlen) = '/';
    memcpy(filename + dirlen + 1, BppObject->FileName, namelen);
    //ext_chset_utf8_to_ucs2_string((U8*)msg.print_object.file_name, sizeof(msg.print_object.file_name), filename);
    bpp_utf8_to_ucs2_string((U8*)msg.print_object.file_name, sizeof(msg.print_object.file_name), filename);
#endif

    //OS_Report("[BPP_EXT]bt_bpp_print, folder(%s) file(%s) fullpath(%s)", BppObject->DirName, BppObject->FileName, filename);
    BPP_DBG("bt_bpp_print, folder(%s) file(%s) fullpath(%s)", BppObject->DirName, BppObject->FileName, filename);

    msg.print_object.object_size = (U32)BppObject->ObjectSize;
    msg.print_object.mime_type = (bt_bpp_mime_type)(BppObject->MimeType + 1); /* map */
    msg.print_object.print_model = (BppObject->bJobBasedPrinting) ? BPP_MODEL_JOB_BASE : BPP_MODEL_SIMPLE_PUSH;
	
    memset(msg.print_object.job_name, 0, sizeof(msg.print_object.job_name));
    memset(msg.print_object.originating_user_name, 0, sizeof(msg.print_object.originating_user_name));
    msg.print_object.job_config.copies = BppObject->PrintingAttribute.Copies;
    msg.print_object.job_config.numberup = (bt_bpp_numberup_enum)(BppObject->PrintingAttribute.NumberUp);
    msg.print_object.job_config.sides = (bt_bpp_sided_enum)(BppObject->PrintingAttribute.Sides);
    msg.print_object.job_config.orient = (bt_bpp_orient_enum)(BppObject->PrintingAttribute.Orient);
    msg.print_object.job_config.quality = (bt_bpp_quality_enum)(BppObject->PrintingAttribute.Quality);

    //find the media size enum value
    if(strcmp("na_index_4x6in", (BppObject->PrintingAttribute).MediaSize) == 0)
    {
        msg.print_object.job_config.media_size = BPP_MEDIA_SIZE_4X6_POSTCARD;
    }
    else
    {
        for(i = 0; i < BPP_MEDIA_SIZE_MAX_ENUM; i++)
        {
            if (strcmp((BppObject->PrintingAttribute).MediaSize, bpp_media_size_self_desc_name[i]) == 0)
            {
                msg.print_object.job_config.media_size = (bt_bpp_media_size_enum)i;
                break;
            }
        }
    }
    msg.print_object.job_config.media_type = (bt_bpp_media_type_enum)(BppObject->PrintingAttribute.MediaType);

    BTCMD_SendMessage(MSG_ID_BT_BPP_PRINT_DOC_REQ, MOD_BT, &msg, sizeof(bt_bpp_print_doc_req_struct));

    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_SEND_PRINT_REQ, msg.print_object.print_model, msg.print_object.mime_type, msg.print_object.object_size);
    BPP_DBG("- print_model(%d), mime_type(%d), object_size(%lu)",
             msg.print_object.print_model, msg.print_object.mime_type, msg.print_object.object_size);

    return 0;
}


void bt_bpp_send_disconnect_req(void)
{
    bt_bpp_disconnect_req_struct msg;
    g_bt_bpp_cntx.state = MTKBPP_STATE_DISCONNECTING;    
        
    //update the State value of the BPP Status in the SDC.
    //bt_bpp_update_sdc_state(MBT_BPP_STATE_DISCONNECTING);
    //BPP_MBT_LOG("[BTMTK][BPP]send DISCONNECT request");
    BPP_DBG("+ send DISCONNECT request");

    //send the disconnect request directly
    msg.hprinter = g_bt_bpp_cntx.hprinter;
    BTCMD_SendMessage(MSG_ID_BT_BPP_DISCONNECT_REQ, MOD_BT, &msg, sizeof(bt_bpp_disconnect_req_struct));

    BPP_DBG("-");
}

//-------------------------------------------------------------------------------
//BPP Sender INTERNAL(REQ) END---------------------------------------------------
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
//BPP Sender INTERNAL(BPP_APP_REQ_CALLBACK) START--------------------------------
//#define BPP_APP_REQ_CALLBACK---------------------------------------------------
//-------------------------------------------------------------------------------

void bt_bpp_enable_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_ENABLE_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);

    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        //BPP_MBT_LOG("[BTMTK][BPP] Enable success: ");
        BPP_DBG("Enable success: ");

        //update the bEnabled value of the BPP Status in the SDC
        //MBT_LOG1("[BTMTK][BPP]Update sdcBppStatus->bEnabled: from (%d) to (MBT_TRUE)", sdcBppStatus->bEnabled);
        //sdcBppStatus->bEnabled = MBT_TRUE;

        //update the State value of the BPP Status in the SDC
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
        g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;

        //Post the MBTEVT_BPP_ENABLE_SUCCESS event to Application in case of success.
        bpp_postevent(BPP_ENABLE_SUCCESS, NULL, 0);
    }
    else
    {
        //BPP_MBT_LOG("[BTMTK][BPP]Enable failed:"); 
        BPP_DBG("Enable failed:");

        //update the bEnabled value of the BPP Status in the SDC
        //MBT_LOG1("[BTMTK][BPP]Update sdcBppStatus->bEnabled: from (%d) to (MBT_FALSE)", sdcBppStatus->bEnabled);
        //sdcBppStatus->bEnabled = MBT_FALSE;

        //update the State value of the BPP Status in the SDC
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_DISABLED);
        g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLED;

        //Post the MBTEVT_BPP_ENABLE_FAIL event to Application in case of failed.
        bpp_postevent(BPP_ENABLE_FAIL, NULL, 0);
     }
     BPP_DBG("-");
}

void bt_bpp_disable_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISABLE_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);

    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        //update the bEnabled value of the BPP Status in the SDC
        //MBT_LOG1("[BTMTK][BPP]Update sdcBppStatus->bEnabled: from (%d) to (MBT_FALSE)", sdcBppStatus->bEnabled);
        //sdcBppStatus->bEnabled = MBT_FALSE;

        //update the State value of the BPP Status in the SDC
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_DISABLED);
        g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLED;

        //Post the MBTEVT_BPP_DISABLE_SUCCESS event to Application in case of success.
	bpp_postevent(BPP_DISABLE_SUCCESS, NULL, 0);
    }
    else
    {
        bpp_postevent(BPP_DISABLE_FAIL, NULL, 0);
    }
    BPP_DBG("-");
}

void bt_bpp_getprinterattr_req_cb(BTMTK_BPP_CNF_CODE cnf_code, bt_bpp_printer_attributes * p_attr)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_GETATTR_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);	
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);


//    public void onBppGetPrinterAttrConfirm(int cnfCode,
//                                  byte sides, byte orientations, byte qualities,
//                                  int max_numberup, int max_copies, int media_size_number, int[] media_size){


    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        int NumMediaSize = p_attr->capability.mediasize_num;
        S8* para[7 + NumMediaSize];

        int i;
        for (i = 0; i < 7 + NumMediaSize; i++)
        {
            para[i] = (S8*)malloc(sizeof(S8) * MAX_DIGIT_SIZE);
        }

        S8 sides[MAX_DIGIT_SIZE];
        S8 orientations[MAX_DIGIT_SIZE];
        S8 qualities[MAX_DIGIT_SIZE];
        S8 max_numberup[MAX_DIGIT_SIZE];
        S8 max_copies[MAX_DIGIT_SIZE];
        S8 media_size_number[MAX_DIGIT_SIZE];
	 S8 printer_type[MAX_DIGIT_SIZE];	

        sprintf(sides, "%d", p_attr->capability.sides_supported);
        BPP_DBG("sides: -> (%s)", sides);
        sprintf(orientations, "%d", p_attr->capability.orientations_supported);
        BPP_DBG("orientations: -> (%s)", orientations);
        sprintf(qualities, "%d", p_attr->capability.print_quality_supported);
        BPP_DBG("qualities: -> (%s)", qualities);
        sprintf(max_numberup, "%lu",p_attr->capability.max_numberup_supported);
        BPP_DBG("max_numberup: -> (%s)", max_numberup);
        sprintf(max_copies, "%lu", p_attr->capability.max_copies_supported);
        BPP_DBG("max_copies: -> (%s)", max_copies);
        sprintf(media_size_number, "%d",  p_attr->capability.mediasize_num);
        BPP_DBG("media_size_number: -> (%s)", media_size_number);
        if ( strcmp((char*)p_attr->capability.printer_name, "Photosmart D7200 series") == 0 )
        {
            sprintf(printer_type, "%d",  1);
            BPP_DBG("printer_type: -> Photosmart D7200 series");
	}
	else
	{
            sprintf(printer_type, "%d",  0);
            BPP_DBG("printer_type: -> Others");
	}
		

        int j;
        for(j = 0; j < MAX_DIGIT_SIZE; j++)
        {
            *(para[0] + j) = sides[j];
            *(para[1] + j) = orientations[j];
            *(para[2] + j) = qualities[j];
            *(para[3] + j) = max_numberup[j];
            *(para[4] + j) = max_copies[j];
            *(para[5] + j) = media_size_number[j];
            *(para[6] + j) = printer_type[j];			
        }

        for(i = 0; i< NumMediaSize; i++)
        {
            S8 mediasize[MAX_DIGIT_SIZE];

            sprintf( mediasize, "%d", p_attr->capability.media_size_supported[i]);
            BPP_DBG("mediasize[%d]: -> (%s)", i,mediasize );

            for(j = 0; j < MAX_DIGIT_SIZE; j++)
            {
                *(para[7 + i] + j) = mediasize[j];
            }
        }


        bpp_postevent(BPP_GET_PRINT_ATTR_SUCCESS, para, 7 + NumMediaSize);
        for (i = 0; i < 7 + NumMediaSize; i++)
        {
            free(para[i]);
        }
        BPP_DBG("-");
        return;
    }
    else
    {
        //BPP_MBT_LOG("[BTMTK][BPP] GetPrinterAttributes failed:");
        BPP_DBG("GetPrinterAttributes failed:");

        switch (cnf_code)
        {
            case BTMTK_BPP_CNF_ERR_RESP:
                 //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
                 g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
                 break;
            
            case BTMTK_BPP_CNF_CONN_FAILED:
            case BTMTK_BPP_CNF_DISCONN_IND:
                 //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
                 g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;
                 break;
 
            default:
                 break;
        }

        //MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
        //sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response  */
		
        bpp_postevent(BPP_GET_PRINT_ATTR_FAIL, NULL, 0); //add error response
    }
    BPP_DBG("-");
}


void bt_bpp_print_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_PRINT_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);

    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        //BPP_MBT_LOG("[BTMTK][BPP]Print success:");
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
        g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
    
        BPP_DBG("Print success:");
        bpp_postevent(BPP_PRINT_COMPLETE_SUCCESS, NULL, 0);
    }
    else
    {
        //BPP_MBT_LOG("[BTMTK][BPP]Print failed:");
        BPP_DBG("Print failed:");
        
        switch (cnf_code)
        {
            case BTMTK_BPP_CNF_ERR_RESP:
                //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
                g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;
                break;
 
            case BTMTK_BPP_CNF_CONN_FAILED:
            case BTMTK_BPP_CNF_DISCONN_IND:
                //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
                g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;
                break;

            default:
                break;
        }
	
        //MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
        //sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bpp_postevent(BPP_PRINT_COMPLETE_FAIL, NULL, 0); //add error response
    }
    BPP_DBG("-");
}

void bt_bpp_disconnect_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISCONNECT_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);

    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        //BPP_MBT_LOG("[BTMTK][BPP] DISCONNECT success:");
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
        g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;


        BPP_DBG("DISCONNECT success:");
        bpp_postevent(BPP_DISCONNECT_SUCCESS, NULL, 0);
    }
    else
    {
        //BPP_MBT_LOG("[BTMTK][BPP] DISCONNECT failed:");
        BPP_DBG("DISCONNECT failed:");

        if (cnf_code == BTMTK_BPP_CNF_ERR_RESP)
        {
            //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
            g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;

        }
        //MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
        //sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bpp_postevent(BPP_DISCONNECT_FAIL, NULL, 0);  //add error response
    }
    BPP_DBG("-");
}

void bt_bpp_connect_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);

    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        //BPP_MBT_LOG("[BTMTK][BPP] CONNECT success:");
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_CONNECTED);
        g_bt_bpp_cntx.state = MTKBPP_STATE_CONNECTED;

        BPP_DBG("CONNECT success:");
        bpp_postevent(BPP_CONNECT_SUCCESS, NULL, 0);
    }
    else
    {
        //BPP_MBT_LOG("[BTMTK][BPP] CONNECT failed:");
        //UPDATE_SDCBPP_STATE(sdcBppStatus, MBT_BPP_STATE_ENABLED);
	//sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
        g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;


        BPP_DBG("CONNECT failed:");
        bpp_postevent(BPP_CONNECT_FAIL, NULL, 0);
    }
    BPP_DBG("-");
}

void bt_bpp_cancel_req_cb(BTMTK_BPP_CNF_CODE cnf_code)
{
    //T_MBT_BPP_STATUS *sdcBppStatus;
    //sdcBppStatus = (T_MBT_BPP_STATUS *) mbt_sdc_getrecord(MBTSDC_REC_BPP_STATUS);

    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_CANCEL_CB, cnf_code, g_bt_bpp_cntx.state, sdcBppStatus->State);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d)] cnf_code(%d)", g_bt_bpp_cntx.state, cnf_code);	

    if (cnf_code == BTMTK_BPP_CNF_SUCCESS)
    {
        //BPP_MBT_LOG("[BTMTK][BPP] CANCEL success");
        BPP_DBG("CANCEL success");
        bpp_postevent(BPP_CANCEL_SUCCESS, NULL, 0);
    }
    else
    {
        //BPP_MBT_LOG("[BTMTK][BPP] CANCEL failed");
        BPP_DBG("CANCEL failed");
        //MBT_LOG("[BTMTK][BPP]sdcBppStatus->FailedReason=MBT_ERROR_RES");
        //sdcBppStatus->FailReason = MBT_ERROR_RES; /* error response */
		
        bpp_postevent(BPP_CANCEL_FAIL, NULL, 0);
    }
    BPP_DBG("-");
}

//-------------------------------------------------------------------------------
//BPP Sender INTERNAL(BPP_APP_REQ_CALLBACK) END----------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//BPP Sender INTERNAL END--------------------------------------------------------
//-------------------------------------------------------------------------------



//-------------------------------------------------------------------------------
//BPP Sender API START-----------------------------------------------------------
//#define BPP_API_FOR_APP--------------------------------------------------------
//-------------------------------------------------------------------------------

void btmtk_bpp_enable(void)
{
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx.state: (%d)",  g_bt_bpp_cntx.state);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_ENABLE, g_bt_bpp_cntx.state);

    if(g_bt_bpp_cntx.state != MTKBPP_STATE_DISABLED)
    {
        bt_bpp_enable_req_cb(BTMTK_BPP_CNF_SUCCESS);
        return;
    }

    bt_bpp_init();

    g_bt_bpp_cntx.state = MTKBPP_STATE_ENABLED;

    bt_bpp_enable_req_cb(BTMTK_BPP_CNF_SUCCESS);

    BPP_DBG("-");
}

void btmtk_bpp_disable (void)
{
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx.state: (%d)",  g_bt_bpp_cntx.state);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISABLE, g_bt_bpp_cntx.state);

    if(g_bt_bpp_cntx.state == MTKBPP_STATE_DISABLED)
    {
        BPP_DBG("- DISABLED"); 
        bt_bpp_disable_req_cb(BTMTK_BPP_CNF_SUCCESS);
        return;
    }

    if(g_bt_bpp_cntx.state == MTKBPP_STATE_ENABLED)
    {
        BPP_DBG("- ENABLED -> DISABLED ");

        bt_bpp_deinit();
        g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLED;
        bt_bpp_disable_req_cb(BTMTK_BPP_CNF_SUCCESS);
        return;
    }

    if (g_bt_bpp_cntx.state == MTKBPP_STATE_DISABLING)
    {
        BPP_DBG("- DISABLING -> DISABLING ");

        /* just wait */
        return;
    }


    /* connecting, connected, printing, disconnecting */
    if (g_bt_bpp_cntx.state == MTKBPP_STATE_DISCONNECTING
        || g_bt_bpp_cntx.state == MTKBPP_STATE_CANCELING)
    {
        BPP_DBG("- DISCONNECTING or CANCELING -> DISABLING ");
        /* wait disconnect cnf */
	g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLING;
    }
    else
    {
       BPP_DBG("- -> DISABLING ");
       /* send disconnect req and wait disconnect cnf */
       	bt_bpp_send_disconnect_req();
        g_bt_bpp_cntx.state = MTKBPP_STATE_DISABLING;
    }

    BPP_DBG("-");
}

void btmtk_bpp_disconnect(void )
{
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx.state: (%d)",  g_bt_bpp_cntx.state);
    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_DISC, g_bt_bpp_cntx.state);
    //BPP_MBT_LOG1("[BTMTK][BPP]btmtk_bpp_disconnect, g_bt_bpp_cntx.state=%d", g_bt_bpp_cntx.state);    

    //mbt call bpp disconnect command
    //we can reset the bpp_request_getprinterattribute and bpp_request_print flag
    //because we will read the flag value when receiving disconnect_cnf
    

    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_DISCONNECTING:
        case MTKBPP_STATE_CANCELING:
            break;

        case MTKBPP_STATE_CONNECTING:
            //send the disconnect request directly
            bt_bpp_send_disconnect_req();

            // connect req for GetPrinterAttribute or Print
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
    BPP_DBG("-");
}

void btmtk_bpp_auth_response(T_OBEX_AUTH *auth_reply)
{
    U8 copy_len_userid;
    U8 copy_len_pwd;
    bt_bpp_auth_rsp_struct msg;

    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_AUTH_RSP, g_bt_bpp_cntx.state, g_bt_bpp_cntx.auth_flag, g_bt_bpp_cntx.hprinter, auth_reply->bAuth);
    //BPP_MBT_LOG3("[BTMTK][BPP]btmtk_bpp_auth_response: bAuth=%d, Passwd=%s, UserID=%s", auth_reply->bAuth, auth_reply->Passwd, auth_reply->UserId);
    BPP_DBG("+");
    BPP_DBG("bAuth=%d, Passwd=%s, UserID=%s", auth_reply->bAuth, auth_reply->Passwd, auth_reply->UserId);


    if(!(g_bt_bpp_cntx.auth_flag & MTKBPP_AUTH_RECV_CHAL))
    {
    	//bpp did not get the authentication indication
        return;
    }

    memset (&msg.auth_resp, 0, sizeof(msg.auth_resp));
    msg.auth_resp.cancel = auth_reply->bAuth;
    msg.auth_resp.cancel = auth_reply->bAuth;

    copy_len_userid =  (OBEX_AUTH_USERID_LEN < BTMTK_OBEX_AUTH_USERID_LEN) ? OBEX_AUTH_USERID_LEN : BTMTK_OBEX_AUTH_USERID_LEN;
    memcpy(msg.auth_resp.userid, auth_reply->UserId, copy_len_userid);
    msg.auth_resp.userid_len = strlen(msg.auth_resp.userid);

    copy_len_pwd =  (OBEX_AUTH_PASSWD_LEN < BTMTK_OBEX_AUTH_PASSWD_LEN) ?OBEX_AUTH_PASSWD_LEN : BTMTK_OBEX_AUTH_PASSWD_LEN;
    memcpy(msg.auth_resp.passwd, auth_reply->Passwd, copy_len_pwd);
    msg.auth_resp.passwd_len = strlen(msg.auth_resp.passwd);
	
    BTCMD_SendMessage(MSG_ID_BT_BPP_AUTH_RSP, MOD_BT, &msg, sizeof(bt_bpp_auth_rsp_struct));

    g_bt_bpp_cntx.auth_flag |= MTKBPP_AUTH_SENT_RESP;
}

void btmtk_bpp_getprinterattribute(T_BDADDR BdAddr, int bitmask_attr)
{

    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_GET_ATT, g_bt_bpp_cntx.state, BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5], 
    //           bitmask_attr, g_bt_bpp_cntx.oper_type);
    // BPP_MBT_LOG1("[BTMTK][BPP]btmtk_bpp_getprinterattribute: bitmask_attr=0x%x", bitmask_attr);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d) oper_type(%d)] BdAddr(%d%d%d%d%d%d)",
             g_bt_bpp_cntx.state, g_bt_bpp_cntx.oper_type, BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5]);
    BPP_DBG(" bitmask_attr=0x%x", bitmask_attr);


    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_ENABLED:
            // bpp state is enable, create bpp connection first
            //store the value of btmask attr
            g_bt_bpp_cntx.oper_type = MTKBPP_OPER_GETATTR;
            g_bt_bpp_cntx.oper.bitmask_attr = bitmask_attr;

            //update the BDAddr, State value of the BPP Status in the SDC
            //bt_bpp_update_sdc_state_addr(MBT_BPP_STATE_CONNECTING, BdAddr);

            //create bpp connection first
            bt_bpp_connect(BdAddr);

            break;

        case MTKBPP_STATE_CONNECTING:
            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) == 0)
            {
                //connecting to the same printer...  store the value of btmask attr
                g_bt_bpp_cntx.oper_type = MTKBPP_OPER_GETATTR;
                g_bt_bpp_cntx.oper.bitmask_attr = bitmask_attr;

                //update the BDAddr/State value of the BPP Status in the SDC
                //bt_bpp_update_sdc_state_addr(MBT_BPP_STATE_CONNECTING, BdAddr);
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
                //bt_bpp_update_sdc_state_addr(MBT_BPP_STATE_GETATTRIBUTE, BdAddr);

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
    BPP_DBG("-");
}

void btmtk_bpp_print(T_BDADDR BdAddr, T_BPP_OBJECT *BppObject)
{

    //bt_trace(TRACE_GROUP_9, BT_BPP_EXT_PRINT, g_bt_bpp_cntx.state, BdAddr[0],
    //          BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5], g_bt_bpp_cntx.oper_type);
    //BPP_MBT_LOG3("[BTMTK][BPP]btmtk_bpp_print: object name(%s), size(%d), bJobBase(%d)",
    //          MBTObject->FileName, MBTObject->ObjectSize, MBTObject->bJobBasedPrinting);
    BPP_DBG("+");
    BPP_DBG("g_bt_bpp_cntx[state(%d) oper_type(%d)] BdAddr(%d%d%d%d%d%d)",
             g_bt_bpp_cntx.state, g_bt_bpp_cntx.oper_type, BdAddr[0], BdAddr[1], BdAddr[2], BdAddr[3], BdAddr[4], BdAddr[5]);
    BPP_DBG("object name(%s), size(%lu), bJobBase(%d)",
             BppObject->FileName, BppObject->ObjectSize, BppObject->bJobBasedPrinting);

    switch(g_bt_bpp_cntx.state)
    {
        case MTKBPP_STATE_ENABLED:
            // bpp state is enable, create bpp connection first
            //store the value of BppObject

            g_bt_bpp_cntx.oper_type = MTKBPP_OPER_PRINT;
            memcpy(&(g_bt_bpp_cntx.oper.print_object), BppObject, sizeof(T_BPP_OBJECT));
            g_bt_bpp_cntx.file_size = (U32)BppObject->ObjectSize;

            //update the BDAddr/State value of the BPP Status in the SDC
            //bt_bpp_update_sdc_state_addr_printobj(MBT_BPP_STATE_CONNECTING, BdAddr, BppObject);
            
            //if no BPP connection, create bpp connection first
            bt_bpp_connect(BdAddr);

            break;
        
        case MTKBPP_STATE_CONNECTING:

            if(memcmp(g_bt_bpp_cntx.printer_addr, BdAddr, 6) == 0)
            {
                //connecting to the same pritner...  store the value of BppObject
                
                g_bt_bpp_cntx.oper_type = MTKBPP_OPER_PRINT;
                memcpy(&(g_bt_bpp_cntx.oper.print_object), BppObject, sizeof(T_BPP_OBJECT));
                g_bt_bpp_cntx.file_size = (U32)BppObject->ObjectSize;

                //bt_bpp_update_sdc_state_addr_printobj(MBT_BPP_STATE_CONNECTING, BdAddr, BppObject);
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
                //bt_bpp_update_sdc_state_addr_printobj(MBT_BPP_STATE_PRINTING, BdAddr, BppObject);
                g_bt_bpp_cntx.file_size = (U32)BppObject->ObjectSize;

                //connecting to the same pritner...
                //if Bpp connection exists, send PRINT REQ
                ret = bt_bpp_print(BppObject);

                if (ret < 0)
                {
                  // FAILED
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
    BPP_DBG("-");
}


/*
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
*/
//-------------------------------------------------------------------------------
//BPP Sender API END-------------------------------------------------------------
//#define BPP_API_FOR_APP--------------------------------------------------------
//-------------------------------------------------------------------------------
