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


#include "bt_types.h"
#include "btalloc.h"
#include "bt_adp_debug.h"
#include <unistd.h>


#ifdef __EXTRA_DEBUG_FEATURE__

typedef struct{
    void *ctx;
    U32 size;
    U32 msg;
}context_info;

#if defined(__MTK_BT_OVER_WIFI_DIRECT__) 
extern bt_wd_record g_bt_wd_records;
extern WDTranContext s_wdTransCtx;
#endif


static context_info s_context[] = {
    {&btCtxHci, sizeof(bt_dbg_hci_context_ind_struct), MSG_ID_BT_DBG_HCI_CONTEXT_IND},        //              (1<<0)
    {&btCtxMe, sizeof(bt_dbg_me_context_ind_struct), MSG_ID_BT_DBG_ME_CONTEXT_IND},         //                (1<<1)
    {&btCtxrmg, sizeof(bt_dbg_rmg_context_ind_struct), MSG_ID_BT_DBG_RMG_CONTEXT_IND},      //              (1<<2)
    {&btCtxl2cap, sizeof(bt_dbg_l2cap_context_ind_struct), MSG_ID_BT_DBG_L2CAP_CONTEXT_IND},  //           (1<<3)
    {&btCtxrxbf, sizeof(bt_dbg_rxbuf_context_ind_struct), MSG_ID_BT_DBG_RXBUF_CONTEXT_IND},      //           (1<<4)
#if SDP_SERVER_SUPPORT == XA_ENABLED
    {&btCtxsdps, sizeof(bt_dbg_sdps_context_ind_struct), MSG_ID_BT_DBG_SDPS_CONTEXT_IND},    //             (1<<5)
#else
    {NULL, 0, 0},
#endif
#if SDP_CLIENT_SUPPORT == XA_ENABLED
    {&btCtxsdpc, sizeof(bt_dbg_sdpc_context_ind_struct), MSG_ID_BT_DBG_SDPC_CONTEXT_IND},    //             (1<<6)
#else
    {NULL, 0, 0},
#endif
#if RFCOMM_PROTOCOL == XA_ENABLED
    {&btCtxrfc, sizeof(bt_dbg_rfc_context_ind_struct), MSG_ID_BT_DBG_RFC_CONTEXT_IND},          //                (1<<7)
#else
    {NULL, 0, 0},
#endif
#if A2MP_PROTOCOL == XA_ENABLED
    {&btCtxa2mp, sizeof(bt_dbg_a2mp_context_ind_struct), MSG_ID_BT_DBG_A2MP_CONTEXT_IND},//             (1<<8)
#else
    {NULL, 0, 0},
#endif
#if ATT_PROTOCOL == XA_ENABLED    
    {&btCtxatt, sizeof(bt_dbg_att_context_ind_struct), MSG_ID_BT_DBG_ATT_CONTEXT_IND},                              //                (1<<9)
    {&btCtxgatt, sizeof(bt_dbg_gatt_context_ind_struct), MSG_ID_BT_DBG_GATT_CONTEXT_IND},                            //              (1<<10)
#else
    {NULL, 0, 0},
    {NULL, 0, 0},
#endif
#if defined(__MTK_BT_OVER_WIFI_DIRECT__)    
    {&g_bt_wd_records, sizeof(bt_dbg_wddev_context_ind_struct), MSG_ID_BT_DBG_WDDEV_CONTEXT_IND},//           (1<<11)
    {&s_wdTransCtx, sizeof(bt_dbg_wdtrans_context_ind_struct), MSG_ID_BT_DBG_WDTRANS_CONTEXT_IND},          //      (1<<12)
#else
    {NULL, 0, 0},
    {NULL, 0, 0},
#endif
};

static void sendContextInd(context_info *pCtx){
    ilm_struct *ilm = NULL;
    U16 *field;
    local_para_struct *ind;

    ilm = (ilm_struct*)bt_win_malloc(sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE+pCtx->size);
    ind = (local_para_struct*)ilm->ilm_data;
    ind->ref_count = 1;
    ind->msg_len = pCtx->size;

    OS_MemCopy(((U8*)ind)+sizeof(local_para_struct),
                            (const U8*)pCtx->ctx,
                            pCtx->size-sizeof(local_para_struct));
    /* Init  */
    ilm->msg_id = pCtx->msg;
    ilm->src_mod_id = MOD_BT;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = INVALID_SAP;
    bt_log_primitive(ilm);
    bt_win_free(ilm);
}

void bt_dbg_query_req_hdlr(ilm_struct *ilm_ptr){
    bt_dbg_query_context_req_struct *req = ( bt_dbg_query_context_req_struct*)ilm_ptr->local_para_ptr;
    bt_dbg_query_context_cnf_struct *cnf = NULL;
    context_info *pCtx = &s_context[0];
    U32 filter = req->filter;
    U32 i;

    cnf = (bt_dbg_query_context_cnf_struct*)construct_local_para(sizeof(bt_dbg_query_context_cnf_struct),TD_UL);
    cnf->filter = 0;
    for(i = 0;i < NUM_OF_BT_CONTEXT;i++, pCtx++){
        if((filter & (1<<i)) && pCtx->ctx && pCtx->size){
            sendContextInd(pCtx);
            cnf->filter |= (1<<i);
        }
    }
    BT_SendMessage(MSG_ID_BT_DBG_QUERY_CONTEXT_CNF, MOD_TST, cnf, sizeof(bt_dbg_query_context_cnf_struct));
}

#endif /* __EXTRA_DEBUG_FEATURE__ */


U32 bt_dbg_manual_dump_req_hdlr(ilm_struct *ilm_ptr){
    U32 *ptr = (U32*)1;
    bt_prompt_trace(MOD_BT, "Request manual dump");
    bt_android_log("Make mtkbt crash to get coredump");
    usleep(200000);
    *ptr= 0;
    return *ptr;
}
