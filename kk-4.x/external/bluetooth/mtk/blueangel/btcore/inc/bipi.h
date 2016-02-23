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

#ifndef __BIPI_H
#define __BIPI_H
/****************************************************************************
 *
 * File:          bipi.h
 *     $Workfile:bipi.h$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:   This file specifies defines and function prototypes for the
 *                BIP application.
 * 
 * Created:       August 6, 2002
 *
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2002-2004 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems,
 * Inc.  This work contains confidential and proprietary information of
 * Extended Systems, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Apr 11 2008 mbj06032
 * [MAUI_00653579] [BT BIP ADP] add $Log: $ to bip adp files
 * 
 * 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 *
 ****************************************************************************/
#include "obex.h"
#include "bip.h"

/****************************************************************************
 *
 * BIP Internal typedefs and defines
 *
 ***************************************************************************/
#define BIP_NO_SERVICE_FOUND 0x80
#define BIP_SCO_REJECT 0x81
#define BIP_BTCHIP_REJECT 0x82
#define BipIsNum(n)         (n >= '0' && n <= '9')
#define BipIsWhiteSpace(ch) (ch == ' ' || ch == '\t')
#define BIPOP_PULL_MASK \
    (BIPOP_GET_CAPABILITIES | BIPOP_GET_IMAGES_LIST | \
     BIPOP_GET_IMAGE_PROPERTIES | BIPOP_GET_IMAGE | \
     BIPOP_GET_LINKED_THUMBNAIL | BIPOP_GET_MONITORING_IMAGE)

#define BIP_CONTEXT_INIT    0x600D

/* 64-bit */
#ifndef SDP_UINT_64BIT
#define SDP_UINT_64BIT(uint) /* uint must be an 8 byte array */ \
            DETD_UINT + DESD_8BYTES,            /* Type & size index 0x0B */ \
            uint
#endif

/* 128-bit */
#ifndef SDP_UINT_128BIT
#define SDP_UINT_128BIT(uint) /* uint must be a 16 byte array */ \
            DETD_UINT + DESD_16BYTES,           /* Type & size index 0x0C */ \
            uint
#endif

/*---------------------------------------------------------------------------
 * BipChanState type
 *
 *      BIP channel state used internaly
 */

#define BIPCHS_UNKNOWN     0x00
#define BIPCHS_INITIALIZED 0x01
#define BIPCHS_CONNECTING  0x02
#define BIPCHS_CONNECTED   0x03
#define BIPCHS_JOBSTARTED  0x04
#define BIPCHS_PRINTING    0x05
#define BIPCHS_WAITING     0x06
#define BIPCHS_STOPPED     0x07
#define BIPCHS_COMPLETED   0x08
#define BIPCHS_ABORTED     0x09
#define BIPCHS_CANCELED    0x0a
#define BIPCHS_AUTHENTICATING 0x0B /* be challenge in TP connected status */
/* End of BipChanState */

/*---------------------------------------------------------------------------
 * BipType type
 */
typedef U16 BipType;

#define BIPTYPE_NONE            0x0000
#define BIPTYPE_CAPABILITIES    0x0001  /* x-bt/img-capabilities */
#define BIPTYPE_LISTING         0x0002  /* x-bt/img-listing */
#define BIPTYPE_PROPERTIES      0x0004  /* x-bt/img-properties */
#define BIPTYPE_IMG             0x0008  /* x-bt/img-img */
#define BIPTYPE_THM             0x0010  /* x-bt/img-thm */
#define BIPTYPE_ATTACHMENT      0x0020  /* x-bt/img-attachment */
#define BIPTYPE_PARTIAL         0x0040  /* x-bt/img-partial */
#define BIPTYPE_MONITORING      0x0080  /* x-bt/img-monitoring */
#define BIPTYPE_STATUS          0x0100  /* x-bt/img-status */
#define BIPTYPE_DISPLAY         0x0200  /* x-bt/img-display */
#define BIPTYPE_PRINT           0x0400  /* x-bt/img-print */
#define BIPTYPE_ARCHIVE         0x0800  /* x-bt/img-archive */

#define BIPTYPE_XML             0x8000
#define BIPTYPE_PULL            BIPTYPE_CAPABILITIES | BIPTYPE_LISTING | \
                                BIPTYPE_PROPERTIES | BIPTYPE_IMG | BIPTYPE_THM | \
                                BIPTYPE_ATTACHMENT | BIPTYPE_PARTIAL | \
                                BIPTYPE_MONITORING | BIPTYPE_STATUS      

#define BIPTYPE_PUSH            BIPTYPE_CAPABILITIES | BIPTYPE_LISTING | \
                                BIPTYPE_PROPERTIES | BIPTYPE_IMG | BIPTYPE_THM | \
                                BIPTYPE_ATTACHMENT | BIPTYPE_DISPLAY | \
                                BIPTYPE_PRINT | BIPTYPE_ARCHIVE      
/* End of BipType */

/*---------------------------------------------------------------------------
 * BipHeaderType type
 */
typedef U8 BipHeaderType;

/* Null terminated, UTF-16 encoded Unicode text, length prefixed with a
 * two-byte unsigned integer.
 */
#define BIPH_IMG_HANDLE         0x30

/* Byte sequence, length prefixed with a two-byte unsigned integer. */
#define BIPH_IMG_DESCRIPTION    0x71
/* End of BipHeaderType */

typedef U8  *BipImgHandlePtr;

/*---------------------------------------------------------------------------
 * BipAppParmTag type
 */
#define BIPAPTAG_NB_RETURNED_HANDLES        0x01
#define BIPAPTAG_LIST_START_OFFSET          0x02
#define BIPAPTAG_LATEST_CAPTURED_IMAGES     0x03
#define BIPAPTAG_PARTIAL_FILE_LENGTH        0x04
#define BIPAPTAG_PARTIAL_FILE_START_OFFSET  0x05
#define BIPAPTAG_TOTAL_FILE_SIZE            0x06
#define BIPAPTAG_END_FLAG                   0x07
#define BIPAPTAG_REMOTE_DISPLAY             0x08
#define BIPAPTAG_SERVICE_ID                 0x09
#define BIPAPTAG_STORE_FLAG                 0x0A
/* End of BipAppParmTag */

/* BIP Context */
typedef struct _BipContext
{
#if BIP_NUM_INITIATORS > 0
    BipInitiatorSession  *initiator[BIP_NUM_INITIATORS];
    BipCallback     initiatorCallback;    /* Application callback function */
#endif /* BIP_NUM_INITIATORS */

#if BIP_NUM_RESPONDERS > 0
    BipResponderSession  *responder[BIP_NUM_RESPONDERS];
    BipCallback     responderCallback;    /* Application callback function */

    /* PrintJob Queue */
    ListEntry       jobQueue;
#endif /* BIP_NUM_RESPONDERS */

    /* ObStore Entries and List */
    BipObStoreEntry obsEntries[OBS_MAX_NUM_ENTRIES];
    ListEntry       obsList;
    U32             initialized;
}   BipContext;

#if XA_CONTEXT_PTR == XA_ENABLED
extern BipContext *bip;
#define BIP(s)  (bip->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern BipContext bip;
#define BIP(s)  (bip.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

/****************************************************************************
 *
 * BIP Internal Function prototypes
 *
 ***************************************************************************/
BOOL        BIP_Init(void);
void        BIP_Deinit(void);
ObStatus    BipObexClientInit(BipObexClient *client, void *callback);
ObStatus    BipObexServerInit(BipObexServer *server, U16 uuid);
void        BipAppCallBack(BipCallbackParms *parms, U16 status, BipEvent event);
BipType     BipGetType(U8 *type, BipType bipTypeMask);
void        BipItoA(U8 *a, U32 i);
U32         BipAtoI(U8 *a, U8 len);
void       *FindBipSession(void *app);

#if BIP_NUM_INITIATORS > 0
/*---------------------------------------------------------------------------
 * BIP_InitInitiatorChannel()
 *
 *      Initialize a BIP Initiator channel and set an application specific
 *      context. A channel must be initialized before use.
 *
 * Parameters:
 *      session - Pointer to BipInitiatorSession.
 *      channel - Identifies the BipChannel.
 *
 * Returns:
 *      BT_STATUS_SUCCESS - The BIP channel was
 *      successfully initialized.
 *
 *      BT_STATUS_FAILED - Init failure.
 */
BtStatus BIP_InitInitiatorChannel(BipInitiatorSession *session, BipChannel channel);

void        *FindBipInitiator(void *app);
void         BipInitiatorCallback(GoepClientEvent *event);
ObStatus     BipBuildReq(BipObexClient *client);
#endif

#if BIP_NUM_RESPONDERS > 0
void        *FindBipResponder(void *app);
void         BipResponderCallback(GoepServerEvent *event);
BtStatus     BipQueuePrintJob(BipObStoreHandle obsh, BipData *data);
#endif

#endif /* __BIPI_H */
