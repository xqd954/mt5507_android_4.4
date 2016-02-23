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

#if defined __BT_BIPI_PORFILE || defined __BT_BIPR_PROFILE__

//#include "mbt_sdc.h"
//#include "mbt_sdc_id.h"
//#include "mbt_bip.h"

#include "string.h"
#include "stdio.h"

#include "bt_mmi.h"
#include "bt_osal.h"

#include "bt_bip_comm.h"
#include "bt_bipr_struct.h"
#include "bt_bipr_api.h"
#include "bt_bipi_struct.h"
#include "bt_bipi_api.h"

#ifdef BTMTK_ON_WISE
#include "bt_debug.h"
#include "ws_sys_api.h"
#else
#include "bt_message.h"
#include "bt_adp_log.h"
#endif

#ifdef BTMTK_WISE_MBT_LOG
#define MTK_LOG(a)              MBT_LOG(a)
#define MTK_LOG1(a, b)          MBT_LOG1(a, b)
#define MTK_LOG2(a, b, c)       MBT_LOG2(a, b, c)
#define MTK_LOG3(a, b, c, d)    MBT_LOG(a, b, c, d)
#else
#define MTK_LOG(a)
#define MTK_LOG1(a, b)
#define MTK_LOG2(a, b, c)
#define MTK_LOG3(a, b, c, d)
#endif

#define SDC_LOG(a)              MBT_LOG(a)
#define SDC_LOG1(a, b)          MBT_LOG1(a, b)
#define SDC_LOG2(a, b, c)       MBT_LOG2(a, b, c)
#define SDC_LOG3(a, b, c, d)    MBT_LOG3(a, b, c, d)

//extern void btmtk_gap_handle_authorize_req(MBT_SERVICE_ID MBTSvcID, T_MBT_BDADDR RemoteBDAddr, MBT_CHAR* Name);

enum WISE_BIP_OP_ENUM
{
    WISE_BIP_OP_NONE,
    WISE_BIP_OP_ACTIVATE,
    WISE_BIP_OP_DEACTIVATE,
    WISE_BIP_OP_GET_CAPA,
    WISE_BIP_OP_PUSH_IMG,
    WISE_BIP_OP_PUSH_THUM,
    WISE_BIP_OP_DISCONNECT
};

typedef struct
{
    const char * s;
    U32 v;
} bip_encoding_map;


static U8 g_img_handle[8];

static HMTKBIPI g_i_handle;
static U8 g_i_opcode;
static U8 g_i_bchal;

static HMTKBIPR g_r_handle;
static U8 g_r_opcode;
static U8 g_r_bchal;

static const char* const status_str[] =
    {
        "",
            "Indicating",
            "Responding",
            "Completed",
            "Requesting",
            "Confirming"
    };

static const bip_encoding_map map[]=
{
    {"JPEG", BT_BIP_TYPE_JPEG},
    {"BMP", BT_BIP_TYPE_BMP},
    {"GIF", BT_BIP_TYPE_GIF},
    {"WBMP", BT_BIP_TYPE_WBMP},
    {"PNG", BT_BIP_TYPE_PNG},
    {"", 0}
};

static const char* bip_get_encoding_str(U32 v)
{
    U32 i = 0;

    while(map[i].s[0])
    {
        if (map[i].v == v)
            break;
        i++;
    }
    return map[i].s;
}
static U32 bip_get_encoding_val(const char* s)
{
    U32 i = 0;

    while(map[i].s[0])
    {
        if (!strcmp(map[i].s, s))
            break;
        i++;
    }
    return map[i].v;
}
#if 0
static T_MBT_BIP_TRANS bip_get_mbt_trans_val(U32  v)
{
    switch (v)
    {
    case BT_BIP_IMG_TRANS_CROP:
        return MBT_BIP_TRANS_CROP;

    case BT_BIP_IMG_TRANS_FILL:
        return MBT_BIP_TRANS_FILL;

    case BT_BIP_IMG_TRANS_STRECH:
        return MBT_BIP_TRANS_STRETCH;

    default:
        break;
    }
    return MBT_BIP_TRANS_NONE;
}

static U32 bip_get_mtk_trans_val(T_MBT_BIP_TRANS v)
{
    switch (v)
    {
    case MBT_BIP_TRANS_CROP:
        return BT_BIP_IMG_TRANS_CROP;

    case MBT_BIP_TRANS_FILL:
        return BT_BIP_IMG_TRANS_FILL;

    case MBT_BIP_TRANS_STRETCH:
        return BT_BIP_IMG_TRANS_STRECH;

    default:
        break;
    }
    return BT_BIP_IMG_TRANS_NONE;

}
#endif

static void bipi_report_enable(S32 result)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (result == BT_BIP_ERR_OK)
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bInitiatorEnabled : (%d) -> (%d)", sdc->bInitiatorEnabled, MBT_TRUE);
//        sdc->bInitiatorEnabled = MBT_TRUE;
//        if (sdc->State == MBT_BIP_STATE_DISABLED)
//        {
//            /* here we only set state in DISABLED case */
//            SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_ENABLED);
//
//            sdc->State = MBT_BIP_STATE_ENABLED;
//        }
        mbt_postevent(MBTEVT_BIP_INITIATOR_ENABLE_SUCCESS, 0);
    }
    else
    {
//        SDC_LOG2("[BTMTK][BIP] sdc->bInitiatorEnabled : (%d) -> (%d)", sdc->bInitiatorEnabled, MBT_FALSE);
//        sdc->bInitiatorEnabled = MBT_FALSE;
//        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISABLED);
//        sdc->State = MBT_BIP_STATE_DISABLED;

        mbt_postevent(MBTEVT_BIP_INITIATOR_ENABLE_FAIL, 0);
    }
}

static void bipi_report_disable(S32 result)
{
//    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (result == BT_BIP_ERR_OK)
    {
//        SDC_LOG2("[BTMTK][BIP] sdc->bInitiatorEnabled : (%d) -> (%d)", sdc->bInitiatorEnabled, MBT_FALSE);
//        sdc->bInitiatorEnabled = MBT_FALSE;
//        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISABLED);
//        sdc->State = MBT_BIP_STATE_DISABLED;
        mbt_postevent(MBTEVT_BIP_INITIATOR_DISABLE_SUCCESS, 0);
    }
    else
    {
        mbt_postevent(MBTEVT_BIP_INITIATOR_DISABLE_FAIL, 0);
    }
}

static void bipi_report_auth_req(bt_bipi_para* para)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (para->cnf.obauth_ind.option)
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bIsObexUserID : (%d) -> (%d)", sdc->bIsObexUserID, MBT_TRUE);
        sdc->bIsObexUserID = MBT_TRUE;
    }
    else
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bIsObexUserID : (%d) -> (%d)", sdc->bIsObexUserID, MBT_FALSE);
        sdc->bIsObexUserID = MBT_FALSE;
    }

    MTK_LOG1("[BTMTK][BIP] <-- Client : [OBEX_AUTH], option = %d.", sdc->bIsObexUserID);

    g_i_bchal = 1;
    mbt_postevent(MBTEVT_BIP_INITIATOR_OBEX_AUTHREQ, 0);
}

static void bipi_report_capabilities(S32 result, bt_bip_capabilities_cnf* cap, S32 b_state)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
    U32 i;
    S32 ret;

    // call continue to complete current operation
    if (result == BT_BIP_ERR_INDICATING)
    {
        MTK_LOG("[BTMTK][BIP] <-- Client : [GET_CAPABILITY], progress indication!");

        ret = btmtk_bipi_continue(g_i_handle, 1);
        MTK_LOG1("[BTMTK][BIP]--> BIP_I : bipi_continue() ret = %d", ret);
        return;
    }

    if (result == BT_BIP_ERR_OK)
    {
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Version : -> (%s)", "1.0");
        strcpy(sdc->ImagingCapability.PreferFormat.Version, "1.0");

        strcpy(sdc->ImagingCapability.PreferFormat.Encoding,
            bip_get_encoding_str(cap->preferred_format.encoding));
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Encoding : -> (%s)",
            sdc->ImagingCapability.PreferFormat.Encoding);


        sdc->ImagingCapability.PreferFormat.Transform = bip_get_mbt_trans_val(cap->preferred_format.transformation);
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Transform : -> (%d)",
            sdc->ImagingCapability.PreferFormat.Transform);

        sdc->ImagingCapability.PreferFormat.Size = cap->preferred_format.size;
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Size : -> (%d)",
            sdc->ImagingCapability.PreferFormat.Size);

        sdc->ImagingCapability.PreferFormat.Height = cap->preferred_format.pixel.height;
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Height : -> (%d)",
            sdc->ImagingCapability.PreferFormat.Height);

        sdc->ImagingCapability.PreferFormat.Width= cap->preferred_format.pixel.width;
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Width : -> (%d)",
            sdc->ImagingCapability.PreferFormat.Width);

        sdc->ImagingCapability.PreferFormat.Height2 = cap->preferred_format.pixel.height2;
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Height2 : -> (%d)",
            sdc->ImagingCapability.PreferFormat.Height2);

        sdc->ImagingCapability.PreferFormat.Width2 = cap->preferred_format.pixel.width2;
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Width2 : -> (%d)",
            sdc->ImagingCapability.PreferFormat.Width2);

        sdc->ImagingCapability.NumImageFormats = 0;
        SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.NumImageFormats : -> (%d)",
            sdc->ImagingCapability.NumImageFormats);

        for (i = 0; i < BT_BIP_MAX_IMAGE_FORMATES && sdc->ImagingCapability.NumImageFormats < MBT_MAX_NUM_OF_NESTED_ATTR; i++)
        {
            if (cap->supported_formats[i].encoding )
            {


                strcpy(sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Encoding,
                    bip_get_encoding_str(cap->supported_formats[i].encoding));

                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Encoding : -> (%s)",
                        sdc->ImagingCapability.NumImageFormats,
                        sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Encoding);

                sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Height = cap->supported_formats[i].pixel.height;
                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Height : -> (%d)",
                        sdc->ImagingCapability.NumImageFormats,
                        sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Height);

                sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Width= cap->supported_formats[i].pixel.width;
                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Width : -> (%d)",
                        sdc->ImagingCapability.NumImageFormats,
                        sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Width);

                sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Height2 = cap->supported_formats[i].pixel.height2;
                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Height2 : -> (%d)",
                        sdc->ImagingCapability.NumImageFormats,
                        sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Height2);

                sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Width2 = cap->supported_formats[i].pixel.width2;
                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Width2 : -> (%d)",
                        sdc->ImagingCapability.NumImageFormats,
                        sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Width2);

                sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Size = cap->supported_formats[i].size;
                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Size : -> (%d)",
                        sdc->ImagingCapability.NumImageFormats,
                        sdc->ImagingCapability.ImageFormats[sdc->ImagingCapability.NumImageFormats].Size);

                sdc->ImagingCapability.NumImageFormats++;
                SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.NumImageFormats : (%d) -> (%d)",
                    sdc->ImagingCapability.NumImageFormats - 1,
                    sdc->ImagingCapability.NumImageFormats);
            }
            else
            {
                break;
            }
        }

        if (g_i_opcode == WISE_BIP_OP_GET_CAPA)
        {
            SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
            sdc->State = MBT_BIP_STATE_CONNECTED;

            g_i_opcode = 0;

            mbt_postevent(MBTEVT_BIP_INITIATOR_GET_CAPABILITY_SUCCESS, 0);

            MTK_LOG("[BTMTK][BIP] --> MBT post success event : MBTEVT_BIP_INITIATOR_GET_CAPABILITY_SUCCESS");
        }
        else
        {
            MBT_ERR1("<-- BIP_I: get capa success in error opcode = %d", g_i_opcode);
        }
    }
    /*
    else if (result == BT_BIP_ERR_USER_ABORT)
    {
        sdc->State = MBT_BIP_STATE_DISCONNECTING;
        mbt_postevent(MBTEVT_BIP_INITIATOR_CANCEL_SUCCESS, 0);
        MTK_LOG1("[BTMTK][BIP] --> MBT post success event : MBTEVT_BIP_INITIATOR_CANCEL_SUCCESS, in case of get-cap abort");

        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        MTK_LOG2("--> BIP_I : disconnect force in case of get-capa abort, ret = %d, g_i_opcode ",
            ret, g_i_opcode);
    }
    */
    else
    {
        if (b_state)
        {
            SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, b_state);
            sdc->State = b_state;

            g_i_opcode = 0;
        }

        SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
        sdc->FailReason = MBT_ERROR_RES;

        mbt_postevent(MBTEVT_BIP_INITIATOR_GET_CAPABILITY_FAIL, 0);
    }
}

static void bipi_report_progress(bt_bip_continue_ind* ind, T_MBT_BIP_STATUS* sdc)
{
    S32 ret;

    SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_SENDING);
    sdc->State = MBT_BIP_STATE_SENDING;

    SDC_LOG2("[BTMTK][BIP] sdc->TxProgress : (%d) -> (%d)", sdc->TxProgress, sdc->TxProgress + ind->data_len);
    sdc->TxProgress += ind->data_len;

    if (ind->obj_len)
        MTK_LOG2("[BTMTK][BIP]<-- BIP_I : [CONTINUE_IND], sent %d%c", (U32)((sdc->TxProgress * 100)/ind->obj_len), '%');
    else
        MTK_LOG1("[BTMTK][BIP]<-- BIP_I : [CONTINUE_IND], sent : %d bytes", sdc->TxProgress);

    mbt_postevent(MBTEVT_BIP_INITIATOR_PROGRESS, 0);

    ret = btmtk_bipi_continue(g_i_handle, 1);
    MTK_LOG1("[BTMTK][BIP]--> BIP_I : bipi_continue() ret = %d", ret);
}

static void bipi_report_push_img(S32 result, S32 b_state, bt_bipi_para* para)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);;

    if (result == BT_BIP_ERR_INDICATING)
    {
        MTK_LOG2("[BTMTK][BIP] <-- Client : [PUT_IMAGE], progress indication, obj = %d, data = %d.",
            para->cnf.ind.obj_len, para->cnf.ind.data_len);

        bipi_report_progress(&para->cnf.ind, sdc);
        return;
    }

    /* other result meaning completion of the transaction */
    if (result == BT_BIP_ERR_THUMBNAIL_NEEDED || result == BT_BIP_ERR_OK)
    {
        MTK_LOG("[BTMTK][BIP] <-- BIP_I : [PUSH_IMAGE] success!");

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;
        mbt_postevent(MBTEVT_BIP_INITIATOR_IMAGE_PUSH_SUCCESS, 0);

        g_i_opcode = 0;
        strcpy(g_img_handle, para->cnf.put_img.handle);

        if (result == BT_BIP_ERR_THUMBNAIL_NEEDED)
        {
            MTK_LOG("[BTMTK][BIP] <-- BIP_I : [PUSH_IMAGE] success, and thumbnail is needed!");
            mbt_postevent(MBTEVT_BIP_INITIATOR_THUMBNAIL_REQ, 0);
        }
    }
    else
    {
        MTK_LOG("[BTMTK][BIP] <-- BIP_I : [PUSH_IMAGE] failed !");

        if (b_state)
        {
            SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, b_state);
            sdc->State = b_state;
            g_i_opcode = 0;
        }

        SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
        sdc->FailReason = MBT_ERROR_RES;
        mbt_postevent(MBTEVT_BIP_INITIATOR_IMAGE_PUSH_FAIL, 0);
    }
}

static void bipi_report_push_thumbnail(S32 result, S32 b_state, bt_bipi_para* para)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
;

    if (result == BT_BIP_ERR_INDICATING)
    {
        MTK_LOG2("[BTMTK][BIP]<-- Client : [PUT_IMAGE], progress indication, obj = %d, data = %d.",
            para->cnf.ind.obj_len, para->cnf.ind.data_len);

        bipi_report_progress(&para->cnf.ind, sdc);
        return;
    }

    g_i_opcode = 0;
    /* other result meaning completion of the transaction */
    if (result == BT_BIP_ERR_OK)
    {
        MTK_LOG("[BTMTK][BIP] <-- BIP_I : [PUSH_THUMBNAIL] success!");

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;
        mbt_postevent(MBTEVT_BIP_INITIATOR_THUMBNAIL_PUSH_SUCCESS, 0);
    }
    else
    {
        MTK_LOG("[BTMTK][BIP] <-- BIP_I : [PUSH_THUMBNAIL] failed !");

        if (b_state)
        {
            SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, b_state);
            sdc->State = b_state;
        }

        SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
        sdc->FailReason = MBT_ERROR_RES;
        mbt_postevent(MBTEVT_BIP_INITIATOR_THUMBNAIL_PUSH_FAIL, 0);
    }
}

static void bipi_report_connect_failure(void)
{
    T_MBT_BIP_STATUS* sdc;

    switch(g_i_opcode)
    {
    case WISE_BIP_OP_GET_CAPA:
        bipi_report_capabilities(BT_BIP_ERR_FAILED, NULL, MBT_BIP_STATE_ENABLED);
        break;

    case WISE_BIP_OP_PUSH_IMG:
        bipi_report_push_img(BT_BIP_ERR_FAILED, MBT_BIP_STATE_ENABLED, NULL);
        break;

    case WISE_BIP_OP_PUSH_THUM:
        bipi_report_push_thumbnail(BT_BIP_ERR_FAILED, MBT_BIP_STATE_ENABLED, NULL);
        break;

    case WISE_BIP_OP_DISCONNECT:
        g_i_opcode = 0;
        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_ENABLED);
        sdc->State = MBT_BIP_STATE_ENABLED;

        mbt_postevent(MBTEVT_BIP_INITIATOR_DISCONNECT_SUCCESS, 0);
        break;

    default:
        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_ENABLED);
        sdc->State = MBT_BIP_STATE_ENABLED;
        break;

    }
}

static void bipi_report_disconnect(S32 result)
{
    bipi_report_connect_failure();
    g_i_bchal = 0;
}

static S32 bipi_send_connect_cmd(T_MBT_BDADDR BdAddr)
{
    bt_bip_connect_req req = {0};

    req.service = BT_BIP_SRV_IMG_PUSH;
    req.addr.lap = BdAddr[3]<< 16 | BdAddr[4] << 8 | BdAddr[5];
    req.addr.uap = BdAddr[2];
    req.addr.nap = BdAddr[0] << 8 | BdAddr[1];
    return btmtk_bipi_connect(g_i_handle, &req);
}

static S32 bipi_send_put_img_cmd(T_MBT_BIP_OBJECT* pobj)
{
    bt_bip_put_img_req req = {0};
    MBT_CHAR* p;

    /* send event to PI */
    mbt_postevent(MBTEVT_BIP_INITIATOR_IMAGE_PUSH_START, 0);

    /* here, i think DirName should alway be enough to content full file path */
    p = pobj->DirName + strlen(pobj->DirName);
    *p = '\\';
    strcpy(p + 1, pobj->FileName);

#if defined(BTMTK_ON_WISE)
    wsConvertFileNameToNativeFormat(req.img_path, pobj->DirName);
#else
    btmtk_chset_utf8_to_ucs2_string((U8*)req.img_path, 520, pobj->DirName);
#endif
    *p = 0;
    btmtk_chset_utf8_to_ucs2_string((U8*)req.img_name, 512, pobj->FileName);
    req.img_desc.size = (U32)pobj->ObjectSize;
    //req.img_desc.maxsize = (U32)pobj->AcceptableFileSize;
    req.img_desc.pixel.height = pobj->ImageDesc.Height;
    req.img_desc.pixel.width = pobj->ImageDesc.Width;
    req.img_desc.pixel.height2 = pobj->ImageDesc.Height2;
    req.img_desc.pixel.width2 = pobj->ImageDesc.Width2;
    req.img_desc.encoding = bip_get_encoding_val(pobj->ImageDesc.Encoding);
    req.img_desc.transformation = bip_get_mtk_trans_val(pobj->ImageDesc.Transform);

    return btmtk_bipi_put_image(g_i_handle, &req);
}

static S32 bipi_send_put_thumbnail_cmd(T_MBT_BIP_OBJECT* pobj)
{
    bt_bip_put_linked_thum_req req = {0};
    MBT_CHAR* p;

    /* send event to PI */
    mbt_postevent(MBTEVT_BIP_INITIATOR_THUMBNAIL_PUSH_START, 0);

    /* here, i think DirName should alway be enough to content full file path */
    p = pobj->DirName + strlen(pobj->DirName);
    *p = '\\';
    strcpy(p + 1, pobj->FileName);

#if defined(BTMTK_ON_WISE)
    wsConvertFileNameToNativeFormat(req.img_path, pobj->DirName);
#else
    btmtk_chset_utf8_to_ucs2_string((U8*)req.img_path, 520, pobj->DirName);
#endif
    *p = 0;
    strcpy(req.handle, g_img_handle);

    return btmtk_bipi_put_linked_thumbnail(g_i_handle, &req);
}

/**
 * will be called when connect or abort happen (continue pending operation).
 */
static void bipi_start_req(void)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
    S32 ret;

    switch(g_i_opcode)
    {
    case WISE_BIP_OP_GET_CAPA:
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;

        ret = btmtk_bipi_get_capabilities(g_i_handle);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I: start_req, bipi_get_capabilities return = %d", ret);
        break;

    case WISE_BIP_OP_PUSH_IMG:
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;

        SDC_LOG2("[BTMTK][BIP] sdc->TxProgress : (%d) -> (%d)", sdc->TxProgress, 0);
        sdc->TxProgress = 0;

        ret = bipi_send_put_img_cmd(&sdc->SendingFile);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I: start_req, bipi_put_img return = %d", ret);
        break;

    case WISE_BIP_OP_PUSH_THUM:
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;

        SDC_LOG2("[BTMTK][BIP] sdc->TxProgress : (%d) -> (%d)", sdc->TxProgress, 0);
        sdc->TxProgress = 0;
        ret = bipi_send_put_thumbnail_cmd(&sdc->SendingFile);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I: start_req, bipi_put_thumbnail() ret = %d", ret);
        break;

    case WISE_BIP_OP_DISCONNECT:
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISCONNECTING);
        sdc->State = MBT_BIP_STATE_DISCONNECTING;

        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        MTK_LOG2("[BTMTK][BIP] --> BIP_I : bipi_disconnect(1) in case of connect success, ret = %d, g_i_opcode ",
            ret, g_i_opcode);
        break;

    default:
        break;
    }
}

/**
 * this function used in request complete confirm only
 * check disconnect by mmi ?
 */
static U32 bipi_check_disconnect(S32 result)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    if (g_i_opcode == WISE_BIP_OP_DISCONNECT)
    {
        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISCONNECTING);
        sdc->State = MBT_BIP_STATE_DISCONNECTING;

        if (result == BT_BIP_ERR_OK)
        {
            SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
            sdc->FailReason = MBT_ERROR_RES;
            mbt_postevent(MBTEVT_BIP_INITIATOR_CANCEL_FAIL, 0);
        }
        else
        {
            mbt_postevent(MBTEVT_BIP_INITIATOR_CANCEL_SUCCESS, 0);
        }
        MTK_LOG1("[BTMTK][BIP] --> MBT post success event : MBTEVT_BIP_INITIATOR_CANCEL event, result = %d", result);

        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        MTK_LOG1("[BTMTK][BIP]--> BIP_I : disconnect force in case of check_disconnect(), ret = %d ", ret);
        return 1;
    }
    return 0;
}

static void bipi_callback(void* p, bt_bipi_para* para)
{
    switch(para->opcode)
    {
    case BT_BIP_OP_ACTIVATE:
        MTK_LOG1("[BTMTK][BIP] <-- Client : [ACTIVATE] : result = %d", para->result);

        if (para->result == BT_BIP_ERR_OK)
        {
            g_i_handle = para->handle;
        }
        g_i_opcode = 0;
        bipi_report_enable(para->result);
        break;

    case BT_BIP_OP_DEACTIVATE:
        MTK_LOG1("[BTMTK][BIP] <-- Client : [DEACTIVATE] : result = %d", para->result);

        if (para->result == BT_BIP_ERR_OK)
        {
            g_i_handle = NULL;
        }
        g_i_opcode = 0;
        bipi_report_disable(para->result);
        break;

    case BT_BIP_OP_CONNECT:
        MBT_ERR1("<--- Client : [CONNECT] : result =%d", para->result);
        if (para->result == BT_BIP_ERR_OK)
        {
            bipi_start_req();
        }
        else
        {
            bipi_report_connect_failure();
        }
        break;

    case BT_BIP_OP_GET_CAPABILITIES:
        MTK_LOG1("[BTMTK][BIP] <-- Client : [GET_CAPABILITIES] : result = %d", para->result);
        /* check if disconnect... */
        if (!bipi_check_disconnect(BT_BIP_ERR_FAILED))
        {
            bipi_report_capabilities(para->result, &para->cnf.capa, MBT_BIP_STATE_CONNECTED);
        }
        break;

    case BT_BIP_OP_PUT_IMG:
        MTK_LOG1("[BTMTK][BIP] <-- Client : [PUT_IMAGE] : result = %d", para->result);
        if (!bipi_check_disconnect(para->result))
        {
            bipi_report_push_img(para->result, MBT_BIP_STATE_CONNECTED, para);
        }
        break;

    case BT_BIP_OP_PUT_LINKED_THUMBNAIL:
        MTK_LOG1("[BTMTK][BIP] <-- Client : [PUT_THUMBNAIL] : result = %d", para->result);
        if (!bipi_check_disconnect(para->result))
        {
            bipi_report_push_thumbnail(para->result, MBT_BIP_STATE_CONNECTED, para);
        }
        break;

    case BT_BIP_OP_DISCONNECT:
        MBT_ERR1("<-- Client : [DISCONNECT] : result = %d.", para->result);
        bipi_report_disconnect(para->result);
        break;

    case BT_BIP_OP_CONTINUE:
        MTK_LOG("[BTMTK][BIP] <-- Client : [CONTINUE_IND]");
        break;

    case BT_BIP_OP_OBEX_AUTH:
        {
            switch (para->result)
            {
            case BT_BIP_ERR_OBAUTH_NEEDED:
                MTK_LOG("[BTMTK][BIP] <--Client : [AUTHENTICATION] : indicating");
                bipi_report_auth_req(para);
                break;
            default:
                MTK_LOG1("[BTMTK][BIP] <--Client : [AUTHENTICATION] : result = %d", para->result);
            }
        }
        break;

    case BT_BIP_OP_ABORT:
        /* in case of abort failure */
        bipi_start_req();
        break;

    }
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_enable_i(MBT_VOID)
{
    int ret;
    bt_bipi_active_info act = {0};

    if (g_i_handle)
    {
        MBT_ERR("--> MBT : bipi_activate in error state!, return ok");
        bipi_report_enable(BT_BIP_ERR_OK);
        return;
    }

    if (g_i_opcode == WISE_BIP_OP_ACTIVATE)
    {
        MBT_ERR("--> MBT : bipi_activate in error state!, return failed");
        bipi_report_enable(BT_BIP_ERR_FAILED);
        return;
    }

    g_i_opcode = WISE_BIP_OP_ACTIVATE;

    act.callback = bipi_callback;
    ret = btmtk_bipi_activate(&act);
    MTK_LOG1("[BTMTK][BIP] --> MBT : bipi_activate, ret = %d", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_disable_i(MBT_VOID)
{
    S32 ret;

    if (!g_i_handle)
    {
        MBT_ERR("--> MBT : bipi_deactivate in error state, response OK!");
        bipi_report_disable(BT_BIP_ERR_OK);
        return;
    }
    if (g_i_opcode == WISE_BIP_OP_DEACTIVATE)
    {
        MBT_ERR("--> MBT : bipi_deactivate in error state, response failed!");
        bipi_report_disable(BT_BIP_ERR_FAILED);
        return;
    }
    g_i_opcode = WISE_BIP_OP_DEACTIVATE;
    ret = btmtk_bipi_deactivate(g_i_handle);
    MTK_LOG1("[BTMTK][BIP] --> BIP_I : bipi_deactivate(), ret = %d", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_auth_response_i(T_MBT_OBEX_AUTH *auth_reply)
{
    bt_bip_obex_auth_rsp rsp;
    S32 ret;

    if (!g_i_bchal)
    {
        MBT_ERR1("--> BIP_I : invoke mbt_bip_initiator_auth_response in error case, cur op = %d", g_i_opcode);
        return;
    }

    if (auth_reply->bAuth == MBT_TRUE)
    {
        rsp.result = BT_BIP_ERR_OK;

        strcpy(rsp.pwd, auth_reply->Passwd);
        rsp.pwd_len = strlen(rsp.pwd);
        rsp.uid_len = strlen(auth_reply->UserId);
        if (rsp.uid_len)
            strcpy(rsp.uid, auth_reply->UserId);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }

    g_i_bchal = 0;

    ret = btmtk_bipi_obex_auth_rsp(g_i_handle, &rsp);
    MTK_LOG1("[BTMTK][BIP] --> BIP_I : bipi_obex_auth_rsp() ret = %d", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_getcapability_request_i(T_MBT_BDADDR BdAddr)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    MTK_LOG1("[BTMTK][BIP] --> BIP_I : getcapabilitiy, state = %d", sdc->State);

    if (g_i_opcode || sdc->State == MBT_BIP_STATE_DISABLED)
    {
        MBT_ERR("--> BIP_I : invoke get capabilitiy in error state!");
        /* there is operation on going already */
        /* if (g_i_opcode != WISE_BIP_OP_GET_CAPA) */
        bipi_report_capabilities(BT_BIP_ERR_FAILED,NULL, 0);
        return;
    }

    /* state should alway be enabled or connected
     */
    g_i_opcode = WISE_BIP_OP_GET_CAPA;

    memcpy(sdc->BDAddr, BdAddr, 6);
    SDC_LOG3("[BTMTK][BIP] sdc->BDAddr[0 - 2] : %d : %d : %d", sdc->BDAddr[0], sdc->BDAddr[1], sdc->BDAddr[2]);
    SDC_LOG3("[BTMTK][BIP] sdc->BDAddr[3 - 5] : %d : %d : %d", sdc->BDAddr[3], sdc->BDAddr[4], sdc->BDAddr[5]);

    SDC_LOG2("[BTMTK][BIP] sdc->Operation : (%d) -> (%d)", sdc->Operation, MBT_BIP_OP_INIT_GETCAPABILITY);
    sdc->Operation = MBT_BIP_OP_INIT_GETCAPABILITY;

    if (sdc->State == MBT_BIP_STATE_CONNECTED)
    {
        ret = btmtk_bipi_get_capabilities(g_i_handle);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I : invoke get capabilitiy ret = %d", ret);
    }
    else
    {
        ret = bipi_send_connect_cmd(BdAddr);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTING);
        sdc->State = MBT_BIP_STATE_CONNECTING;

        MTK_LOG1("[BTMTK][BIP] --> BIP_I : invoke get capabilitiy, start to connect, ret = %d ", ret);
    }

}

static void mbt_bip_print_object(const char* t, T_MBT_BIP_OBJECT* obj)
{
    if (obj)
    {
        SDC_LOG2("[BTMTK][BIP] %s->AcceptableFileSize : -> (%d)", t, obj->AcceptableFileSize);
        SDC_LOG2("[BTMTK][BIP] %s->DirName : -> (%s)", t, obj->DirName);
        SDC_LOG2("[BTMTK][BIP] %s->FileName : -> (%s)", t, obj->FileName);
        SDC_LOG2("[BTMTK][BIP] %s->ObjectSize : -> (%d)", t, obj->ObjectSize);
    }
}
/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_pushimage_i(T_MBT_BDADDR BdAddr, T_MBT_BIP_OBJECT *MBTObject)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    MTK_LOG1("[BTMTK][BIP] --> BIP_I : push_image, state = %d", sdc->State);

    if (g_i_opcode || sdc->State == MBT_BIP_STATE_DISABLED)
    {
        MBT_ERR("--> BIP_I : invoke put_img in error state!");
        /* there is operation on going already */
        /* if (g_i_opcode != WISE_BIP_OP_GET_CAPA) */
        bipi_report_push_img(BT_BIP_ERR_FAILED, 0, NULL);
        return;
    }

    /* state should alway be enabled or connected
     */
    g_i_opcode = WISE_BIP_OP_PUSH_IMG;

    memcpy(sdc->BDAddr, BdAddr, 6);
    SDC_LOG3("[BTMTK][BIP] sdc->BDAddr[0 - 2] : %d : %d : %d", sdc->BDAddr[0], sdc->BDAddr[1], sdc->BDAddr[2]);
    SDC_LOG3("[BTMTK][BIP] sdc->BDAddr[3 - 5] : %d : %d : %d", sdc->BDAddr[3], sdc->BDAddr[4], sdc->BDAddr[5]);

    SDC_LOG2("[BTMTK][BIP] sdc->Operation : (%d) -> (%d)", sdc->Operation, MBT_BIP_OP_INIT_PUSHIMAGE);
    sdc->Operation = MBT_BIP_OP_INIT_PUSHIMAGE;
    sdc->SendingFile = *MBTObject;
    mbt_bip_print_object("sdc->SendingFile", &sdc->SendingFile);

    if (sdc->State == MBT_BIP_STATE_CONNECTED)
    {
        /* sdc->State = MBT_BIP_STATE_SENDING; */
        SDC_LOG2("[BTMTK][BIP] sdc->TxProgress : (%d) -> (%d)", sdc->TxProgress, 0);
        sdc->TxProgress = 0;
        ret = bipi_send_put_img_cmd(MBTObject);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I : invoke put_image ret = %d", ret);
    }
    else
    {
        ret = bipi_send_connect_cmd(BdAddr);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTING);
        sdc->State = MBT_BIP_STATE_CONNECTING;

        MTK_LOG1("[BTMTK][BIP] --> BIP_I : invoke put_image, start to connect, ret = %d ", ret);
    }
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_pushthumbnail_i(T_MBT_BDADDR BdAddr, T_MBT_BIP_OBJECT *MBTObject)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    MTK_LOG1("[BTMTK][BIP] --> BIP_I : push_thumbnail, state = %d", sdc->State);

    if (g_i_opcode || sdc->State == MBT_BIP_STATE_DISABLED)
    {
        MBT_ERR("--> BIP_I : invoke put_img in error state!");
        /* there is operation on going already */
        /* if (g_i_opcode != WISE_BIP_OP_GET_CAPA) */
        bipi_report_push_thumbnail(BT_BIP_ERR_FAILED, 0, NULL);
        return;
    }

    /* state should alway be enabled or connected
     */
    g_i_opcode = WISE_BIP_OP_PUSH_THUM;

    memcpy(sdc->BDAddr, BdAddr, 6);
    SDC_LOG3("[BTMTK][BIP] sdc->BDAddr[0 - 2] : %d : %d : %d", sdc->BDAddr[0], sdc->BDAddr[1], sdc->BDAddr[2]);
    SDC_LOG3("[BTMTK][BIP] sdc->BDAddr[3 - 5] : %d : %d : %d", sdc->BDAddr[3], sdc->BDAddr[4], sdc->BDAddr[5]);

    SDC_LOG2("[BTMTK][BIP] sdc->Operation : (%d) -> (%d)", sdc->Operation, MBT_BIP_OP_INIT_PUSHTHUMBNAIL);
    sdc->Operation = MBT_BIP_OP_INIT_PUSHTHUMBNAIL;

    sdc->SendingFile = *MBTObject;
    mbt_bip_print_object("sdc->SendingFile", &sdc->SendingFile);

    if (sdc->State == MBT_BIP_STATE_CONNECTED)
    {
        /* sdc->State = MBT_BIP_STATE_SENDING; */
        SDC_LOG2("[BTMTK][BIP] sdc->TxProgress : (%d) -> (%d)", sdc->TxProgress, 0);
        sdc->TxProgress = 0;
        ret = bipi_send_put_thumbnail_cmd(MBTObject);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I : invoke bipi_put_thumbnail ret = %d", ret);
    }
    else
    {
        ret = bipi_send_connect_cmd(BdAddr);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTING);
        sdc->State = MBT_BIP_STATE_CONNECTING;

        MTK_LOG1("[BTMTK][BIP] --> BIP_I : invoke put_thumbnail, start to connect, ret = %d ", ret);
    }
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID mbt_bip_initiator_disconnect_i(MBT_VOID)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    MTK_LOG1("[BTMTK][BIP] --> BIP_I : disconnect, state = %d", sdc->State);

    switch(sdc->State)
    {
    case MBT_BIP_STATE_DISABLED:
    case MBT_BIP_STATE_ENABLED:
        MBT_ERR("--> BIP_I : disconnect in disconnect state!");
        mbt_postevent(MBTEVT_BIP_INITIATOR_DISCONNECT_SUCCESS, 0);
        return;

    case MBT_BIP_STATE_CONNECTING:
        /* send abort */
        mbt_postevent(MBTEVT_BIP_INITIATOR_CANCEL_SUCCESS, 0);
        /* continue */
    case MBT_BIP_STATE_CONNECTED:
        ret = btmtk_bipi_disconnect(g_i_handle, 1);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I : disconnect force, ret = %d ", ret);

    case MBT_BIP_STATE_DISCONNECTING:
        g_i_opcode = WISE_BIP_OP_DISCONNECT;
        break;

    default:
        g_i_opcode = WISE_BIP_OP_DISCONNECT;
        ret = btmtk_bipi_abort(g_i_handle);
        MTK_LOG1("[BTMTK][BIP] --> BIP_I : disconnect, start bipi_abort() ret = %d", ret);
        break;
    }

    SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISCONNECTING);
    sdc->State = MBT_BIP_STATE_DISCONNECTING;
    MTK_LOG("[BTMTK][BIP] --> BIP_I : disconnect, set state == DISCONNECTING");
}


/* -------------------- responder -------------------------------------------- */

/**
 * post parameters: result (bool)
 */
static void bipr_report_enable(S32 result)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (result == BT_BIP_ERR_OK)
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bResponderEnabled : (%d) -> (%d)", sdc->bResponderEnabled, MBT_TRUE);
        sdc->bResponderEnabled = MBT_TRUE;

        if (sdc->State == MBT_BIP_STATE_DISABLED)
        {
            /* here we only set state in DISABLED case */
            SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_ENABLED);
            sdc->State = MBT_BIP_STATE_ENABLED;
        }

        mbt_postevent(MBTEVT_BIP_RESPONDER_ENABLE_SUCCESS, 0);
    }
    else
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bResponderEnabled : (%d) -> (%d)", sdc->bResponderEnabled, MBT_FALSE);
        sdc->bResponderEnabled = MBT_FALSE;

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISABLED);
        sdc->State = MBT_BIP_STATE_DISABLED;
        mbt_postevent(MBTEVT_BIP_RESPONDER_ENABLE_FAIL, 0);
    }
}

/**
 * post parameters: result(bool)
 */
static void bipr_report_disable(S32 result)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (result == BT_BIP_ERR_OK)
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bResponderEnabled : (%d) -> (%d)", sdc->bResponderEnabled, MBT_FALSE);
        sdc->bResponderEnabled = MBT_FALSE;

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISABLED);
        sdc->State = MBT_BIP_STATE_DISABLED;
        mbt_postevent(MBTEVT_BIP_RESPONDER_DISABLE_SUCCESS, 0);
    }
    else
    {
        mbt_postevent(MBTEVT_BIP_RESPONDER_DISABLE_FAIL, 0);
    }
}

static void bip_set_mbt_btaddr(T_MBT_BDADDR mbt, bt_bip_bd_addr* mtk)
{
    mbt[0] = (U8)(mtk->nap >> 8);
    mbt[1] = (U8)mtk->nap;
    mbt[2] = mtk->uap;
    mbt[3] = (U8)(mtk->lap >> 16);
    mbt[4] = (U8)(mtk->lap >> 8);
    mbt[5] = (U8)(mtk->lap);
}

/**
 * ?
 */
static void bipr_report_authorize_ind(bt_bipr_para* para)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
    T_MBT_AUTHREPLY* btAuthReply = (T_MBT_AUTHREPLY*)mbt_sdc_getrecord(MBTSDC_REC_GAP_AUTHREPLY);

    SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTING);
    sdc->State = MBT_BIP_STATE_CONNECTING;

    btmtk_gap_handle_authorize_req(MBT_SVCUUID_IMAGING_RESPONDER, btAuthReply->BdAddr, para->data.auth_ind.dev_name);

    if (strlen(para->data.auth_ind.dev_name) < MBT_MAX_NAME_LEN)
        strcpy(btAuthReply->Name, para->data.auth_ind.dev_name);
    else
    {
        memcpy(btAuthReply->Name, para->data.auth_ind.dev_name, MBT_MAX_NAME_LEN - 1);
        btAuthReply->Name[MBT_MAX_NAME_LEN - 1] = 0;
    }

    bip_set_mbt_btaddr(btAuthReply->BdAddr, &para->data.auth_ind.addr);

    MTK_LOG("[BTMTK][BIP] <-- BIP_R : receive authorize indication, set state to CONNECTING");
    mbt_postevent(MBTEVT_BIP_RESPONDER_AUTH_REQ, 0);
}

static void bipr_rsp_connect_ind(bt_bipr_para* para)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (para->status == BT_BIPR_OPS_INDICATING)
    {
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTING);
        sdc->State = MBT_BIP_STATE_CONNECTING;

        bip_set_mbt_btaddr(sdc->BDAddr, &para->data.conn_ind.addr);
        if (strlen(para->data.conn_ind.dev_name) < MBT_MAX_NAME_LEN)
            strcpy(sdc->RemoteDevName, para->data.conn_ind.dev_name);
        else
        {
            memcpy(sdc->RemoteDevName, para->data.conn_ind.dev_name, MBT_MAX_NAME_LEN - 1);
            sdc->RemoteDevName[MBT_MAX_NAME_LEN - 1] = 0;
        }
        SDC_LOG1("[BTMTK][BIP] sdc->RemoteDevName : (%s)", sdc->RemoteDevName);

        ret = btmtk_bipr_connect_rsp(g_r_handle, 1);
        MTK_LOG1("[BTMTK][BIP] --> BIP_R : bipr_connect_rsp(), ret = %d.", ret);
    }
    else
    {
        MTK_LOG("[BTMTK][BIP] --> BIP_R : [CONNECT_IND], connection established!");
        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;
    }
}
/**
 * post parameters: isObexUserID (bool) ?
 */
static void bipr_report_obex_auth(bt_bipr_para* para)
{
    T_MBT_BIP_STATUS* sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    if (para->data.obauth_ind.option)
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bIsObexUserID : (%d) -> (%d)", sdc->bIsObexUserID, MBT_TRUE);
        sdc->bIsObexUserID = MBT_TRUE;
    }
    else
    {
        SDC_LOG2("[BTMTK][BIP] sdc->bIsObexUserID : (%d) -> (%d)", sdc->bIsObexUserID, MBT_FALSE);
        sdc->bIsObexUserID = MBT_FALSE;
    }

    g_r_bchal = 1;

    MTK_LOG1("[BTMTK][BIP] <-- MBT_R : [OBEX_AUTH] indication, option = %d.", sdc->bIsObexUserID);

    mbt_postevent(MBTEVT_BIP_RESPONDER_OBEX_AUTHREQ, 0);
}
static void bipr_rsp_get_capa_ind(bt_bipr_para* para)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    if (para->status == BT_BIPR_OPS_INDICATING)
    {
        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

        g_r_opcode = WISE_BIP_OP_GET_CAPA;

        SDC_LOG2("[BTMTK][BIP] sdc->Operation : (%d) -> (%d)", sdc->Operation, MBT_BIP_OP_RESP_GETCAPABILITY);
        sdc->Operation = MBT_BIP_OP_RESP_GETCAPABILITY;
        mbt_postevent(MBTEVT_BIP_RESPONDER_GET_CAPABILITY_REQ, 0);
    }
    else if (para->status == BT_BIPR_OPS_COMPLETE)
    {
        /* here meaning always be success */
        mbt_postevent(MBTEVT_BIP_RESPONDER_CAPABILITY_RES_SUCCESS, 0);
        if (g_r_opcode == WISE_BIP_OP_GET_CAPA)
            g_r_opcode = 0;
        else if (g_r_opcode == WISE_BIP_OP_DISCONNECT)
        {
            ret = btmtk_bipr_disconnect(g_r_handle, 1);
            MTK_LOG1("[BTMTK][BIP] --> BIP_R : in case of GetCapa Complete, bipr_disconnect(), ret = %d", ret);
        }
    }
}

static void wchar_net_to_local(unsigned short* str)
{
    unsigned short t;

    while(*str)
    {
        t = *((unsigned char*)str) << 8 | *(((unsigned char*)str) + 1);
        *str = t;
        str++;
    }
}

/**
 * post parameters: progress (U32 or bt_bip_continue_ind)
 */
static void bipr_report_progress(bt_bip_continue_ind* ind, T_MBT_BIP_STATUS* sdc)
{
    S32 ret;

    sdc->State = MBT_BIP_STATE_RECEIVING;
    sdc->RxProgress += ind->data_len;

    if (ind->obj_len)
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [CONTINUE_IND], received %d%c", (U32)((sdc->RxProgress * 100)/ind->obj_len), '%');
    else
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [CONTINUE_IND], received : %d bytes", sdc->RxProgress);

    mbt_postevent(MBTEVT_BIP_RESPONDER_PROGRESS, 0);

    ret = btmtk_bipr_continue(g_r_handle, 1);
    MTK_LOG1("[BTMTK][BIP] --> BIP_R : bipr_continue() ret = %d", ret);
}

static void bipr_rsp_put_img_ind(bt_bipr_para* para)
{
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    switch (para->status)
    {
    case BT_BIPR_OPS_INDICATING:
        g_r_opcode = WISE_BIP_OP_PUSH_IMG;

        sdc->ReceivingFile.ObjectSize = para->data.put_img.img_desc.size;
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ObjectSize : -> (%d)", sdc->ReceivingFile.ObjectSize);

        strcpy(sdc->ReceivingFile.ImageDesc.Version, "1.0");
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Version : -> (%s)", "1.0");

        strcpy(sdc->ReceivingFile.ImageDesc.Encoding, bip_get_encoding_str(para->data.put_img.img_desc.encoding));
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Encoding : -> (%s)", sdc->ReceivingFile.ImageDesc.Encoding);

        sdc->ReceivingFile.ImageDesc.Size = para->data.put_img.img_desc.size;
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Size : -> (%d)", sdc->ReceivingFile.ImageDesc.Size);

        sdc->ReceivingFile.ImageDesc.Transform = bip_get_mbt_trans_val(para->data.put_img.img_desc.transformation);
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Transform : -> (%d)", sdc->ReceivingFile.ImageDesc.Transform);

        sdc->ReceivingFile.ImageDesc.Width = para->data.put_img.img_desc.pixel.width;
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Width : -> (%d)", sdc->ReceivingFile.ImageDesc.Width);

        sdc->ReceivingFile.ImageDesc.Height= para->data.put_img.img_desc.pixel.height;
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Height : -> (%d)", sdc->ReceivingFile.ImageDesc.Height);

        sdc->ReceivingFile.ImageDesc.Width2 = para->data.put_img.img_desc.pixel.width2;
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Width2 : -> (%d)", sdc->ReceivingFile.ImageDesc.Width2);

        sdc->ReceivingFile.ImageDesc.Height2 = para->data.put_img.img_desc.pixel.height2;
        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.ImageDesc.Height2 : -> (%d)", sdc->ReceivingFile.ImageDesc.Height2);

        wchar_net_to_local(para->data.put_img.img_name);
        btmtk_chset_ucs2_to_utf8_string(
            sdc->ReceivingFile.FileName,
            MBT_MAX_FILE_NAME_LEN,
            (U8*)para->data.put_img.img_name);

        SDC_LOG1("[BTMTK][BIP] sdc->ReceivingFile.FileName : -> (%s)", sdc->ReceivingFile.FileName);

        mbt_postevent(MBTEVT_BIP_RESPONDER_ACCESS_REQ, 0);
        break;

    case BT_BIPR_OPS_RESPONDING:
        MTK_LOG3("[BTMTK][BIP] <-- BIP_R : [PUT_IMG] : status = %s, image size = %d, current size = %d",
            status_str[para->status],
            para->data.cnt_ind.obj_len,
            para->data.cnt_ind.data_len);
        bipr_report_progress(&para->data.cnt_ind, sdc);
        break;

    case BT_BIPR_OPS_COMPLETE:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [PUT_IMG] : Complete, result", para->data.result);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;
        if (para->data.result == BT_BIP_ERR_OK)
            mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_SUCCESS, 0);
        else
        {
            SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
            sdc->FailReason = MBT_ERROR_RES;
            mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_FAIL, 0);
        }
        g_r_opcode = 0;
        break;
    }
}

static void bipr_rsp_put_thumbnail_ind(bt_bipr_para* para)
{
    T_MBT_BIP_STATUS* sdc;
    S32 ret;
    MBT_CHAR* p;
    bt_bip_put_linked_thum_rsp rsp;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    switch (para->status)
    {
    case BT_BIPR_OPS_INDICATING:
        g_r_opcode = WISE_BIP_OP_PUSH_THUM;

        SDC_LOG2("[BTMTK][BIP] sdc->Operation : (%d) -> (%d)", sdc->Operation, MBT_BIP_OP_RESP_PUSHTHUMBNAIL);
        sdc->Operation = MBT_BIP_OP_RESP_PUSHTHUMBNAIL;
        //sdc->ReceivingFile.ObjectSize = (U32)-1;

        mbt_postevent(MBTEVT_BIP_RESPONDER_THUMBNAIL_RECEIVE_START, 0);


        p = sdc->ReceivingFile.DirName + strlen(sdc->ReceivingFile.DirName);
        if (p > sdc->ReceivingFile.DirName)
        {
            /* maybe we should consider if path name size is larger than 260...! */
            rsp.result = BT_BIP_ERR_OK;
            *p = '\\';
            if (strlen(sdc->ReceivingFile.FileName))
            {
                strcpy(p + 1, sdc->ReceivingFile.FileName);
                sprintf(p + 1, "%s.thum.jpg", sdc->ReceivingFile.FileName);
            }
            else
            {
                strcpy(p + 1, "1234567.thum.jpg");
            }
#if defined(BTMTK_ON_WISE)
            wsConvertFileNameToNativeFormat(rsp.img_path, sdc->ReceivingFile.DirName);
#else
            btmtk_chset_utf8_to_ucs2_string((U8*)rsp.img_path, 520, sdc->ReceivingFile.DirName);
#endif
            *p = 0;
        }
        else
        {
            rsp.result = BT_BIP_ERR_FAILED;
        }
        ret = btmtk_bipr_put_linked_thumbnail(g_r_handle, &rsp);
        MTK_LOG1("[BTMTK][BIP] --> BIP_R : bipr_put_linked_thumbnail(), ret = %d.", ret);
        break;

    case BT_BIPR_OPS_RESPONDING:
        MTK_LOG3("[BTMTK][BIP] <-- BIP_R : [PUT_IMG_THUMNAIL] : status = %s, image size = %d, current size = %d",
            status_str[para->status],
            para->data.cnt_ind.obj_len,
            para->data.cnt_ind.data_len);
        bipr_report_progress(&para->data.cnt_ind, sdc);
        break;

    case BT_BIPR_OPS_COMPLETE:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [PUT_IMG_THUMBNAIL] : Complete, result", para->data.result);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_CONNECTED);
        sdc->State = MBT_BIP_STATE_CONNECTED;
        if (para->data.result == BT_BIP_ERR_OK)
            mbt_postevent(MBTEVT_BIP_RESPONDER_THUMBNAIL_RECEIVE_SUCCESS, 0);
        else
        {
            SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
            sdc->FailReason = MBT_ERROR_RES;
            mbt_postevent(MBTEVT_BIP_RESPONDER_THUMBNAIL_RECEIVE_FAIL, 0);
        }
        g_r_opcode = 0;
        break;
    }
}

/*
 * only used in callback function with OPERATION EVENT!!!
 */
U32 bipr_check_disconnect(bt_bipr_para* para, U32 opcode)
{
    T_MBT_BIP_STATUS* sdc;
    S32 ret;

    if (g_r_opcode == WISE_BIP_OP_DISCONNECT)
    {
        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_DISCONNECTING);
        sdc->State = MBT_BIP_STATE_DISCONNECTING;

        if (para->status == BT_BIPR_OPS_COMPLETE && para->data.result == BT_BIP_ERR_OK)
        {
            SDC_LOG2("[BTMTK][BIP] sdc->FailReason : (%d) -> (%d)", sdc->FailReason, MBT_ERROR_RES);
            sdc->FailReason = MBT_ERROR_RES;
            if (opcode == BT_BIP_OP_PUT_IMG)
                mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_SUCCESS, 0);
            else if (opcode == BT_BIP_OP_PUT_LINKED_THUMBNAIL)
                mbt_postevent(MBTEVT_BIP_RESPONDER_THUMBNAIL_RECEIVE_SUCCESS, 0);
            else if (opcode == BT_BIP_OP_GET_CAPABILITIES)
                mbt_postevent(MBTEVT_BIP_RESPONDER_CAPABILITY_RES_SUCCESS, 0);
        }
        else
        {
            if (opcode == BT_BIP_OP_PUT_IMG)
                mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_FAIL, 0);
            else if (opcode == BT_BIP_OP_PUT_LINKED_THUMBNAIL)
                mbt_postevent(MBTEVT_BIP_RESPONDER_THUMBNAIL_RECEIVE_FAIL, 0);
            else if (opcode == BT_BIP_OP_GET_CAPABILITIES)
                mbt_postevent(MBTEVT_BIP_RESPONDER_CAPABILITY_RES_SUCCESS, 0);
        }
        MTK_LOG("[BTMTK][BIP] --> MBT post success event : MBTEVT_BIP_RESPONDER_CANCEL event");

        ret = btmtk_bipr_disconnect(g_r_handle, 1);
        MTK_LOG1("[BTMTK][BIP] --> BIP_R : disconnect force in case of check_disconnect(), ret = %d ", ret);
        return 1;
    }
    return 0;
}

/**
 * post parameters: result (code mapping?)
 */
static void bipr_report_disconnect(void)
{
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_ENABLED);
    sdc->State = MBT_BIP_STATE_ENABLED;

    switch(g_r_opcode)
    {
    case WISE_BIP_OP_GET_CAPA:
        MBT_ERR("<-- BIP_R : [DISCONNECT_IND], in case of GET_CAPA!");
        break;

    case WISE_BIP_OP_PUSH_IMG:
        mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_FAIL, 0);
        break;

    case WISE_BIP_OP_PUSH_THUM:
        mbt_postevent(MBTEVT_BIP_RESPONDER_THUMBNAIL_RECEIVE_FAIL, 0);
        break;

    case WISE_BIP_OP_DISCONNECT:
        mbt_postevent(MBTEVT_BIP_RESPONDER_DISCONNECT_SUCCESS, 0);
        break;

    default:
        break;
    }
    g_r_opcode = 0;
    g_r_bchal = 0;
}

static void bipr_callback(void* p, bt_bipr_para* para)
{
    switch(para->opcode)
    {
    case BT_BIP_OP_ACTIVATE:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [ACTIVATE] : result = %d", para->data.result);
        if (para->data.result == BT_BIP_ERR_OK)
        {
            g_r_handle = para->handle;
        }
        g_r_opcode = 0;
        bipr_report_enable(para->data.result);
        break;

    case BT_BIP_OP_DEACTIVATE:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [DEACTIVATE] : result = %d", para->data.result);
        if (para->data.result == BT_BIP_ERR_OK)
            g_r_handle = 0;
        g_r_opcode = 0;
        bipr_report_disable(para->data.result);
        break;

    case BT_BIP_OP_AUTHORIZE:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [AUTHORIZE] : dev name : %s",  para->data.auth_ind.dev_name);
        MTK_LOG3("[BTMTK][BIP] <-- BIP_R : [AUTHORIZE] : addr: lap = %x, uap = %x, nap = %x",
            para->data.auth_ind.addr.lap,
            para->data.auth_ind.addr.uap,
            para->data.auth_ind.addr.nap);
        bipr_report_authorize_ind(para);
        break;

    case BT_BIP_OP_CONNECT:
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [CONNECT] : status = %s, result =%d", status_str[para->status], para->data.result);
        bipr_rsp_connect_ind(para);
        break;

    case BT_BIP_OP_OBEX_AUTH:
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [OBEX_AUTH] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (para->status == BT_BIPR_OPS_INDICATING)
        {
            bipr_report_obex_auth(para);
        }
        break;

    case BT_BIP_OP_GET_CAPABILITIES:
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [GET_CAPABILITIES] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (!bipr_check_disconnect(para, BT_BIP_OP_GET_CAPABILITIES))
            bipr_rsp_get_capa_ind(para);
        break;

    case BT_BIP_OP_PUT_IMG:
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [PUT_IMAGE] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (!bipr_check_disconnect(para, BT_BIP_OP_PUT_IMG))
        {
            bipr_rsp_put_img_ind(para);
        }
        break;

    case BT_BIP_OP_PUT_LINKED_THUMBNAIL:
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [PUT_THUMBNAIL] : status = %s, result =%d", status_str[para->status], para->data.result);
        if (!bipr_check_disconnect(para, BT_BIP_OP_PUT_IMG))
        {
            bipr_rsp_put_thumbnail_ind(para);
        }
        break;

    case BT_BIP_OP_GET_IMG:
        break;

    case BT_BIP_OP_DISCONNECT:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [DISCONNECT] : status = %s.", status_str[para->status]);
        bipr_report_disconnect();
        break;

    case BT_BIP_OP_CONTINUE:
        MTK_LOG1("[BTMTK][BIP] <-- BIP_R : [CONTINUE_IND] : status = %s", status_str[para->status]);
        break;

    case BT_BIP_OP_ABORT:
        MTK_LOG2("[BTMTK][BIP] <-- BIP_R : [R_ABORT] status = %s, result = %d", status_str[para->status], para->data.result);

        break;
    }
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_BOOL btmtk_bipi_is_connected(T_MBT_BDADDR BdAddr, U8 bCheckAddr)
{
    T_MBT_BIP_STATUS* sdc;

    bt_prompt_trace(MOD_MMI, "--> [btmtk_bipi_is_connected], bCheckAddr = %d.", (U32)bCheckAddr);

    if (!g_i_handle)
    {
        MTK_LOG("[BTMTK][BIP] --> bip_i_is_connected, return FALSE");
        return MBT_FALSE;
    }

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
    if (sdc->State > MBT_BIP_STATE_CONNECTING)
    {
        if (bCheckAddr)
        {
            if (!memcmp(BdAddr, sdc->BDAddr, 6))
            {
                MTK_LOG("[BTMTK][BIP] --> bip_i_is_connected, check BDAddr, return TRUE");
                return MBT_TRUE;
            }
            else
            {
                MTK_LOG("[BTMTK][BIP] --> bip_i_is_connected, check BDAddr, return FALSE");
                return MBT_FALSE;
            }
        }
        MTK_LOG("[BTMTK][BIP] --> bip_i_is_connected, return TRUE");
        return MBT_TRUE;
    }

    MTK_LOG("[BTMTK][BIP] --> bip_i_is_connected, return FALSE");
    return MBT_FALSE;
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_BOOL btmtk_bipr_is_connected(T_MBT_BDADDR BdAddr, U8 bCheckAddr)
{
    T_MBT_BIP_STATUS* sdc;

    bt_prompt_trace(MOD_MMI, "--> [btmtk_bipr_is_connected], bCheckAddr = %d.", (U32)bCheckAddr);

    if (!g_r_handle)
    {
        MTK_LOG("[BTMTK][BIP] --> bip_r_is_connected, return FALSE");
        return MBT_FALSE;
    }

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
    if (sdc->State > MBT_BIP_STATE_CONNECTING)
    {
        if (bCheckAddr)
        {
            if (!memcmp(BdAddr, sdc->BDAddr, 6))
            {
                MTK_LOG("[BTMTK][BIP] --> bip_r_is_connected, check BDAddr, return TRUE");
                return MBT_TRUE;
            }
            else
            {
                MTK_LOG("[BTMTK][BIP] --> bip_r_is_connected, check BDAddr, return FALSE");
                return MBT_FALSE;
            }
        }
        MTK_LOG("[BTMTK][BIP] --> bip_r_is_connected, return TRUE");
        return MBT_TRUE;
    }

    MTK_LOG("[BTMTK][BIP] --> bip_r_is_connected, return FALSE");
    return MBT_FALSE;
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_bipr_authorize_res(MBT_BYTE AuthMode)
{
    S32 ret;

    bt_prompt_trace(MOD_MMI, "--> [btmtk_bipr_authorize_res]");
    MTK_LOG("[BTMTK][BIP] --> incoming btmtk_bip_authorize_res()");
    if (g_r_handle)
    {
        ret = btmtk_bipr_authorize(g_r_handle, (U32)AuthMode);
        MTK_LOG2("[BTMTK][BIP] --> bipr_authorize(), option = %d, ret", (U32)AuthMode, ret);
        return;
    }
    MBT_ERR("--> invoke btmtk_bip_authorize_res() in error state!");
}
/********************************************************************************
*   Description :
********************************************************************************/

void btmtk_bipr_enable()
MBT_VOID mbt_bip_responder_enable_i(MBT_VOID)
{
    S32 ret;
    bt_bipr_active_info inf = {0};

    if (g_r_handle)
    {
        MBT_ERR("--> MBT_R : inovke enable in error state, repose ok!");
        bipr_report_enable(BT_BIP_ERR_OK);
        return;
    }

    if (g_r_opcode == WISE_BIP_OP_ACTIVATE)
    {
        MBT_ERR("--> MBT_R : inovke enable in error state, repose failed!");
        bipr_report_enable(BT_BIP_ERR_FAILED);
        return;
    }

    g_r_opcode = WISE_BIP_OP_ACTIVATE;

    inf.callback = bipr_callback;
    inf.service = BT_BIP_SRV_IMG_PUSH | BT_BIP_SRV_IMG_PULL;
    ret = btmtk_bipr_activate(&inf);
    MTK_LOG1("[BTMTK][BIP] --> MBT_R : bipr_activate(), ret = %d", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_bipr_disable()
MBT_VOID mbt_bip_responder_disable_i(MBT_VOID)
{
    S32 ret;

    if (!g_r_handle)
    {
        MBT_ERR("--> MBT_R : inovke disenable in error state, repose ok!");
        bipr_report_disable(BT_BIP_ERR_OK);
        return;
    }

    if (g_r_opcode == WISE_BIP_OP_DEACTIVATE)
    {
        MBT_ERR("--> MBT_R : inovke disenable in error state, repose failed!");
        bipr_report_disable(BT_BIP_ERR_FAILED);
        return;
    }

    g_r_opcode = WISE_BIP_OP_DEACTIVATE;

    ret = btmtk_bipr_deactivate(g_r_handle);
    MTK_LOG1("[BTMTK][BIP] --> MBT_R : bipr_deactivate(), ret = %d", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
/**
 * bt_bip_obex_auth_rsp: auth, userId, password
 */
void btmtk_bipr_auth_response( bt_bip_obex_auth_rsp response )
MBT_VOID mbt_bip_responder_auth_response_i(T_MBT_OBEX_AUTH *auth_reply)
{
    S32 ret;
    bt_bip_obex_auth_rsp rsp = {0};

    if (!g_r_bchal)
    {
        MBT_ERR1("--> BIP_R : invoke mbt_bip_responder_auth_response() in error case!, curr op = %d", g_r_opcode);
        return;
    }
    if (auth_reply->bAuth && auth_reply->UserId[0])
    {
        rsp.result = BT_BIP_ERR_OK;

        strcpy(rsp.pwd, auth_reply->UserId);
        rsp.pwd_len = strlen(auth_reply->UserId);

        rsp.uid_len = strlen(auth_reply->Passwd);
        if (rsp.uid_len)
            strcpy(rsp.uid, auth_reply->Passwd);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }

    g_r_bchal = 0;

    ret = btmtk_bipr_obex_auth_rsp(g_r_handle, &rsp);
    MTK_LOG1("[BTMTK][BIP] --> MBT_R : bipr_obex_auth_rsp(), ret = %d", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
/**
 * push_image (bt_bip_put_img_rsp): response-code ( partial-content ), filename, handle
 */
void btmtk_bipr_access_response( bt_bip_put_img_rsp response )
MBT_VOID mbt_bip_responder_access_response_i(T_MBT_AUTHRES Reply)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;
    MBT_CHAR* p;
    T_MBT_BIP_OBJECT* pobj;
    bt_bip_put_img_rsp rsp = {0};

    if (g_r_opcode != WISE_BIP_OP_PUSH_IMG)
    {
        MBT_ERR1("--> BIP_R : invoke mbt_bip_responder_access_response() in error case, current opcode = %d", g_r_opcode);
        mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_FAIL, 0);
        return;
    }

    if (MBT_ALLOW_RES == Reply)
    {
        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
        if (sdc->bThumbReq == MBT_FALSE)
            rsp.result = BT_BIP_ERR_OK;
        else
            rsp.result = BT_BIP_ERR_PARTIAL_CONTENT;

        /* here, i think DirName should alway be enough to content full file path */
        pobj = &sdc->ReceivingFile;
        p = pobj->DirName + strlen(pobj->DirName);
        *p = '\\';
        strcpy(p + 1, pobj->FileName);
#if defined(BTMTK_ON_WISE)
        wsConvertFileNameToNativeFormat(rsp.img_path, pobj->DirName);
#else
        btmtk_chset_utf8_to_ucs2_string((U8*)rsp.img_path, 520, pobj->DirName);
#endif
        *p = 0;
        strcpy(rsp.handle, "1234567"); /* just set it */

        /* update sdc */
        SDC_LOG2("[BTMTK][BIP] sdc->Operation : (%d) -> (%d)", sdc->Operation, MBT_BIP_OP_RESP_PUSHIMAGE);
        sdc->Operation = MBT_BIP_OP_RESP_PUSHIMAGE;

        SDC_LOG2("[BTMTK][BIP] sdc->State : (%d) -> (%d)", sdc->State, MBT_BIP_STATE_RECEIVING);
        sdc->State = MBT_BIP_STATE_RECEIVING;

        SDC_LOG2("[BTMTK][BIP] sdc->RxProgress : (%d) -> (%d)", sdc->RxProgress, 0);
        sdc->RxProgress = 0;

        mbt_postevent(MBTEVT_BIP_RESPONDER_IMAGE_RECEIVE_START, 0);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }

    ret = btmtk_bipr_put_image(g_r_handle, &rsp);
    MTK_LOG2("[BTMTK][BIP] --> MBT_R : bipr_put_image(), option = %d, ret = %d", Reply, ret);
}

static S32 bip_check_pixel(U16 w, U16 h, bt_bip_pixel* pixel)
{
    if (!pixel->height2 && !pixel->width2)
    {
        if (w == pixel->height2 && h == pixel->width2)
            return 1;
        else
            return 0;
    }
    else if (pixel->height2 && pixel->width2)
    {
        if (pixel->height)
        {
            if ((pixel->width <= w && pixel->width2 >= w) &&
                (pixel->height <= h && pixel->height2 >= h))
                return 1;
            else
                return 0;
        }
        else
        {
            if ((pixel->height2 / pixel->width2) == (h/w))
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

static void mbt_bip_print_capability(const char* t, T_MBT_BIP_STATUS* sdc, T_MBT_BIP_IMAGING_CAPABILITY* cap)
{
    U32 i;

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Version : -> (%s)", "1.0");

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Encoding : -> (%s)",
        sdc->ImagingCapability.PreferFormat.Encoding);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Transform : -> (%d)",
        sdc->ImagingCapability.PreferFormat.Transform);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Size : -> (%d)",
        sdc->ImagingCapability.PreferFormat.Size);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Height : -> (%d)",
        sdc->ImagingCapability.PreferFormat.Height);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Width : -> (%d)",
        sdc->ImagingCapability.PreferFormat.Width);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Height2 : -> (%d)",
        sdc->ImagingCapability.PreferFormat.Height2);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.PreferFormat.Width2 : -> (%d)",
        sdc->ImagingCapability.PreferFormat.Width2);

    SDC_LOG1("[BTMTK][BIP] sdc->ImagingCapability.NumImageFormats : -> (%d)",
        sdc->ImagingCapability.NumImageFormats);

    for (i = 0; i < sdc->ImagingCapability.NumImageFormats; i++)
    {

        SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Encoding : -> (%s)",
            i,
            sdc->ImagingCapability.ImageFormats[i].Encoding);

        SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Height : -> (%d)",
            i,
            sdc->ImagingCapability.ImageFormats[i].Height);

        SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Width : -> (%d)",
            i,
            sdc->ImagingCapability.ImageFormats[i].Width);


        SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Height2 : -> (%d)",
            i,
            sdc->ImagingCapability.ImageFormats[i].Height2);


        SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Width2 : -> (%d)",
            i,
            sdc->ImagingCapability.ImageFormats[i].Width2);


        SDC_LOG2("[BTMTK][BIP] sdc->ImagingCapability.ImageFormats[%d].Size : -> (%d)",
            i,
            sdc->ImagingCapability.ImageFormats[i].Size);
    }
}
/********************************************************************************
*   Description :
********************************************************************************/
/**
 * bt_bip_capabilities_rsp: result, preferred_format, supported_formats x n,
 * 							num of supported_formats ?
 */
void btmtk_bipr_getcapability_response( U8 , bt_bip_capabilities_rsp response )
MBT_VOID mbt_bip_responder_getcapability_response_i(T_MBT_AUTHRES Reply, T_MBT_BIP_IMAGING_CAPABILITY *ImagingCapability)
{
    bt_bip_capabilities_rsp rsp = {0};
    S32 ret;
    S32 f;
    T_MBT_BIP_STATUS* sdc;

    if (g_r_opcode != WISE_BIP_OP_GET_CAPA)
    {
        MBT_ERR1("--> BIP_R : invoke mbt_bip_responder_getcapability_response() in error case, current opcode = %d", g_r_opcode);
        return;
    }
    if (Reply == MBT_ALLOW_RES)
    {
        f = 0;

        rsp.result = BT_BIP_ERR_OK;
        rsp.preferred_format.encoding = bip_get_encoding_val(ImagingCapability->PreferFormat.Encoding);
        rsp.preferred_format.size = (U32)ImagingCapability->PreferFormat.Size;
        rsp.preferred_format.transformation = bip_get_mtk_trans_val(ImagingCapability->PreferFormat.Transform);
        rsp.preferred_format.pixel.height = ImagingCapability->PreferFormat.Height;
        rsp.preferred_format.pixel.width = ImagingCapability->PreferFormat.Width;
        rsp.preferred_format.pixel.height2 = ImagingCapability->PreferFormat.Height2;
        rsp.preferred_format.pixel.width2 = ImagingCapability->PreferFormat.Width2;

        if (rsp.preferred_format.encoding == BT_BIP_TYPE_JPEG)
        {
            f = bip_check_pixel(160, 120, &rsp.preferred_format.pixel);
        }

        for (ret = 0; ret < BT_BIP_MAX_IMAGE_FORMATES && (U32)ret < ImagingCapability->NumImageFormats; ret++)
        {
            rsp.supported_formats[ret].encoding  = bip_get_encoding_val(ImagingCapability->ImageFormats[ret].Encoding);
            rsp.supported_formats[ret].size = (U32)ImagingCapability->ImageFormats[ret].Size;
            rsp.supported_formats[ret].pixel.width = ImagingCapability->ImageFormats[ret].Width;
            rsp.supported_formats[ret].pixel.height = ImagingCapability->ImageFormats[ret].Height;
            rsp.supported_formats[ret].pixel.width2 = ImagingCapability->ImageFormats[ret].Width2;
            rsp.supported_formats[ret].pixel.height2 = ImagingCapability->ImageFormats[ret].Height2;

            if (!f && rsp.supported_formats[ret].encoding == BT_BIP_TYPE_JPEG)
                f = bip_check_pixel(160, 120, &rsp.supported_formats[ret].pixel);
        }

        if (!f)
        {
            if (ret == BT_BIP_MAX_IMAGE_FORMATES)
                ret--;

            rsp.supported_formats[ret].encoding  = BT_BIP_TYPE_JPEG;
            rsp.supported_formats[ret].size = 65535;
            rsp.supported_formats[ret].pixel.width = 160;
            rsp.supported_formats[ret].pixel.height = 120;
            rsp.supported_formats[ret].pixel.width2 = 0;
            rsp.supported_formats[ret].pixel.height2 = 0;

        }

        sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);
        sdc->ImagingCapability = *ImagingCapability;
        mbt_bip_print_capability("sdc->ImagingCapability", sdc, &sdc->ImagingCapability);
    }
    else
    {
        rsp.result = BT_BIP_ERR_FAILED;
    }
    ret = btmtk_bipr_get_capabilities(g_r_handle, &rsp);
    MTK_LOG1("[BTMTK][BIP] --> BIP_R : bipr_get_capabilities(), ret = %d.", ret);
}

/********************************************************************************
*   Description :
********************************************************************************/
void btmtk_bipr_disconnect()
MBT_VOID mbt_bip_responder_disconnect_i(MBT_VOID)
{
    S32 ret;
    T_MBT_BIP_STATUS* sdc;

    sdc = (T_MBT_BIP_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_BIP_STATUS);

    MTK_LOG1("[BTMTK][BIP] --> BIP_R : disconnect, state = %d", sdc->State);

    switch(sdc->State)
    {
    case MBT_BIP_STATE_DISABLED:
    case MBT_BIP_STATE_ENABLED:
        MBT_ERR("--> BIP_R : disconnect in disconnect state!");
        mbt_postevent(MBTEVT_BIP_RESPONDER_DISCONNECT_SUCCESS, 0);
        return;

    case MBT_BIP_STATE_CONNECTING:
    case MBT_BIP_STATE_CONNECTED:
        /* if (g_r_opcode != WISE_BIP_OP_GET_CAPA) */
        {
            ret = btmtk_bipr_disconnect(g_r_handle, 1);
            MTK_LOG1("[BTMTK][BIP] --> BIP_R : disconnect force, ret = %d ", ret);
        }

    case MBT_BIP_STATE_DISCONNECTING:
        g_r_opcode = WISE_BIP_OP_DISCONNECT;
        break;

    default:
        g_r_opcode = WISE_BIP_OP_DISCONNECT;
        ret = btmtk_bipr_abort(g_r_handle);
        MTK_LOG1("[BTMTK][BIP] --> BIP_R : disconnect, start bipi_abort() ret = %d", ret);
        break;
    }
    sdc->State = MBT_BIP_STATE_DISCONNECTING;
    MTK_LOG("[BTMTK][BIP] --> BIP_R : disconnect, set state == DISCONNECTING");
}

#endif//MBT_BIP
