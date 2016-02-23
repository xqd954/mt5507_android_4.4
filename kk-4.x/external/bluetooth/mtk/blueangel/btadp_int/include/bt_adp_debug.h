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
#ifndef __BT_ADP_DEBUG_H__
#define __BT_ADP_DEBUG_H__

#include "bt_common.h"
#include "bt_feature.h"
#include "sys/btstats.h"

#include "sys/mei.h"
#include "sys/host.h"
#include "sys/rxbuff.h"
#include "sys/radiomgi.h"
#include "sys/l2cap_i.h"
#if RFCOMM_PROTOCOL == XA_ENABLED
#include "sys/rfc.h"
#endif /* RFCOMM_PROTOCOL == XA_ENABLED */
#include "sys/sdpi.h"
#include "sys/a2mp.h"
#include "sys/att_i.h"
#include "sys/gatt_i.h"

#if defined(__MTK_BT_OVER_WIFI_DIRECT__)
#include "sys/btwdtrans.h"
#include "sys/btwddev.h"
#endif

#define BT_CONTEXT_HCI              (1<<0)
#define BT_CONTEXT_ME               (1<<1)
#define BT_CONTEXT_RMG             (1<<2)
#define BT_CONTEXT_L2CAP          (1<<3)
#define BT_CONTEXT_RXBUF          (1<<4)
#define BT_CONTEXT_SDPS            (1<<5)
#define BT_CONTEXT_SDPC            (1<<6)
#define BT_CONTEXT_RFC               (1<<7)
#define BT_CONTEXT_A2MP            (1<<8)
#define BT_CONTEXT_ATT               (1<<9)
#define BT_CONTEXT_GATT             (1<<10)
#define BT_CONTEXT_WDDEV          (1<<11)
#define BT_CONTEXT_WDTRANS     (1<<12)
#define NUM_OF_BT_CONTEXT       13


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

typedef struct{
    LOCAL_PARA_HDR
    U8 context[1];
}bt_dbg_context_ind_struct;

/* MSG_ID_BT_DBG_HCI_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtHciContext hci;
}bt_dbg_hci_context_ind_struct;

/* MSG_ID_BT_DBG_ME_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtMeContext me;
}bt_dbg_me_context_ind_struct;

/* MSG_ID_BT_DBG_RMG_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtRmgContext rmg;
}bt_dbg_rmg_context_ind_struct;

/* MSG_ID_BT_DBG_L2CAP_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtL2CapContext l2cap;
}bt_dbg_l2cap_context_ind_struct;

/* MSG_ID_BT_DBG_RXBUF_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtRxbuffContext rxbuf;
}bt_dbg_rxbuf_context_ind_struct;

/* MSG_ID_BT_DBG_SDPS_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtSdpServContext sdps;
}bt_dbg_sdps_context_ind_struct;

/* MSG_ID_BT_DBG_SDPC_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtSdpClientContext sdpc;
}bt_dbg_sdpc_context_ind_struct;

/* MSG_ID_BT_DBG_RFC_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtRfcContext rfc;
}bt_dbg_rfc_context_ind_struct;

#if A2MP_PROTOCOL == XA_ENABLED
/* MSG_ID_BT_DBG_A2MP_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtA2mpContext a2mp;
}bt_dbg_a2mp_context_ind_struct;
#endif


/* MSG_ID_BT_DBG_ATT_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtAttContext att;
}bt_dbg_att_context_ind_struct;

/* MSG_ID_BT_DBG_GATT_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    BtGattContext gatt;
}bt_dbg_gatt_context_ind_struct;

#if defined(__MTK_BT_OVER_WIFI_DIRECT__)
/* MSG_ID_BT_DBG_WDDEV_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    bt_wd_record wdrec;
}bt_dbg_wddev_context_ind_struct;

/* MSG_ID_BT_DBG_WDTRANS_CONTEXT_IND */
typedef struct{
    LOCAL_PARA_HDR
    WDTranContext wdtran;
}bt_dbg_wdtrans_context_ind_struct;
#endif /* __MTK_BT_OVER_WIFI_DIRECT__ */

/* MSG_ID_BT_DBG_MANUAL_DUMP_REQ */
typedef struct{
    LOCAL_PARA_HDR
}bt_dbg_manual_dump_req_struct;


#endif

