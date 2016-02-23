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

/***************************************************************************
 *
 * File:
 *     $Workfile:hid_ctrl.c$ for XTNDAccess Blue SDK, Version 2.0
 *     $Revision: #1 $
 *
 * Description:
 *     Source code for HID control channel.
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
 ****************************************************************************/
#if defined(__BT_HIDD_PROFILE__) || defined(__BT_HIDH_PROFILE__)

//#include "kal_release.h"
//#include "bluetooth_trc.h"
#include <sys/hidi.h>
#include <sys/hidalloc.h>

/*---------------------------------------------------------------------------
 *            HidControlChannel()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Parses data from the control channel.
 *
 * Return:    void
 */
void HidControlChannel(HidChannel *Channel, L2capChannelId Cid, 
                       L2capCallbackParms *Parms)
{
#if HID_HOST == XA_ENABLED
    HidTransaction *pTrans;
#endif

#if HID_DEVICE == XA_ENABLED
    HidReportReq    reportReq;
#endif

    HidTransaction  trans;
    HidReport       report;
    BtStatus        status;

#if HID_HOST == XA_ENABLED
    if (HID(hidRole) == HID_ROLE_HOST) {
        /* Host receives responses to transaction requests, except for the
         * CONTROL transaction, which can be received spontaneously.
         */
        if ((Parms->ptrs.data[0] & 0xF0) != HID_MSG_CONTROL) {
            /* Response to a request */
            if (!IsListEmpty(&Channel->transQueue)) {
                /* A Transaction is outstanding */
                pTrans =  (HidTransaction *)GetHeadList(&Channel->transQueue);
                if (pTrans->flags & TRANS_FLAG_SENT) {
                    /* The entire transaction has been sent */
                    switch (Parms->ptrs.data[0] & 0xF0) {
                    case HID_MSG_HANDSHAKE:
                        /* Complete the transaction */
                        pTrans->resultCode = Parms->ptrs.data[0] & 0x0F;
                        HidCompleteTransaction(Channel, pTrans, 
                                               BT_STATUS_SUCCESS);
                        break;
                    case HID_MSG_DATA:
                    case HID_MSG_DATC:
                        /* Response to a GET_* transaction */
                        switch (pTrans->type) {
                        case HID_TRANS_GET_REPORT:
                            report.data = Parms->ptrs.data + 1;
                            report.dataLen = Parms->dataLen - 1;
                            report.reportType = Parms->ptrs.data[0] & 0x03;
                            trans.type = HID_TRANS_GET_REPORT_RSP;
                            trans.parm.report = &report;
                            break;
                        case HID_TRANS_GET_PROTOCOL:
                            if ((Parms->ptrs.data[0] & 0xF0) == HID_MSG_DATA) {
                                trans.type = HID_TRANS_GET_PROTOCOL_RSP;
                                trans.parm.protocol = Parms->ptrs.data[1] & 0x01;
                            } else {
                                kal_trace(BT_TRACE_G2_PROFILES,HID_HOST_UNEXPECTED_DATC);
                                return;
                            }
                            break;
                        case HID_TRANS_GET_IDLE_RATE:
                            if ((Parms->ptrs.data[0] & 0xF0) == HID_MSG_DATA) {
                                trans.type = HID_TRANS_GET_IDLE_RATE_RSP;
                                trans.parm.idleRate = Parms->ptrs.data[1];
                            } else {
                                kal_trace(BT_TRACE_G2_PROFILES,HID_HOST_UNEXPECTED_DATC);
                                return;
                            }
                            break;
                        }

                        /* Set up the event for the application */
                        trans.resultCode = HID_RESULT_SUCCESS;

                        /* See if this is the last response */
                        if (Parms->dataLen == 
                                      L2CAP_GetTxMtu(Channel->ctrlCid)) {
                            status = BT_STATUS_PENDING;
                        } else {
                            status = BT_STATUS_SUCCESS;
                        }

                        /* Indicate the response data */
                        HidAppCallback(Channel, HIDEVENT_TRANSACTION_RSP, 
                                       status, &trans, 
                                       sizeof(HidTransaction));

                        if (status != BT_STATUS_PENDING) {
                            /* All data received */
                            pTrans->resultCode = HID_RESULT_SUCCESS;
                            HidCompleteTransaction(Channel, pTrans, 
                                                   BT_STATUS_SUCCESS);
                        }
                        break;
                    }
                } else {
                    kal_trace(BT_TRACE_G2_PROFILES,HID_HOST_RESPONSE_RECEIVED_BEFORE_REQUEST_SENT);
                    return;
                }
            }
        } else {
            /* Send the Control Message to the application */
            if ((Parms->ptrs.data[0] & 0x0F) == HID_CTRL_VIRTUAL_CABLE_UNPLUG) {
                trans.type = HID_TRANS_CONTROL;
                trans.resultCode = HID_RESULT_SUCCESS;
                trans.parm.control = Parms->ptrs.data[0] & 0x0F;
                HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                               BT_STATUS_SUCCESS, 
                               &trans, sizeof(HidTransaction));
            } else {
                kal_trace(BT_TRACE_G2_PROFILES,HID_HOST_INVALID_CONTROL_COMMAND);
                return;
            }
        }
    }
#endif /* HID_HOST == XA_ENABLED */
     
#if HID_DEVICE == XA_ENABLED
    if (HID(hidRole) == HID_ROLE_DEVICE) {
        /* Device receives transaction requests and must respond (except for
         * the CONTROL transaction.
         */
        switch (Parms->ptrs.data[0] & 0xF0) {
        case HID_MSG_CONTROL:
            /* Send the Control Message to the application */
            trans.type = HID_TRANS_CONTROL;
            trans.resultCode = HID_RESULT_SUCCESS;
            trans.parm.control = Parms->ptrs.data[0] & 0x0F;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, BT_STATUS_SUCCESS, 
                           &trans, sizeof(HidTransaction));
            break;
        case HID_MSG_GET_REPORT:
            /* Set up the event for the application */
            trans.type = HID_TRANS_GET_REPORT;
            trans.resultCode = HID_RESULT_SUCCESS;
            reportReq.reportType = Parms->ptrs.data[0] & 0x03;
            if (Parms->ptrs.data[0] & 0x08) {
                //Add by stanley: in this case bit3=1: A 2byte BufferSize follows the ReportID(1 byte)
                /* A length is added */
                kal_trace(BT_TRACE_G2_PROFILES,GET_REPORT_BIT3_IS_1);
                if (Parms->dataLen == 4) {
                    /* A report ID is added */
                    reportReq.useId = TRUE;
                    reportReq.reportId = Parms->ptrs.data[1];
                    reportReq.bufferSize = LEtoHost16(&Parms->ptrs.data[2]);
                } else {
                    /* No report ID, just length */
                    reportReq.useId = FALSE;
                    reportReq.bufferSize = LEtoHost16(&Parms->ptrs.data[1]);
                }
            } else {
                //Add by stanley: in this case bit3=0:
                kal_trace(BT_TRACE_G2_PROFILES,GET_REPORT_BIT3_IS_0);
                if (Parms->dataLen == 2) {
                    /* A report ID is added */
                    reportReq.useId = TRUE;
                    reportReq.reportId = Parms->ptrs.data[1];
                } else {
                    reportReq.useId = FALSE;
                }
            }

            trans.parm.reportReq = &reportReq;
            kal_trace(BT_TRACE_G2_PROFILES,GET_REPORT_PARMA_LENxD ,Parms->dataLen);
            //Add by stanley: 2007-0608    
            if(reportReq.useId)
            {
                kal_trace(BT_TRACE_G2_PROFILES,GET_REPORTIDxD ,reportReq.reportId);
                //Current reportId only 1 (Keyboard) or 2 (Mouse)
                if((reportReq.reportId !=1) && (reportReq.reportId !=2))
                {
                    //Reserved ReportId is used
                     /* Set up the event for the application */
                    trans.type = HID_HANDSHAKE; 
                    trans.resultCode = HID_RESULT_INVALID_REPORT_ID;
                    HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, BT_STATUS_SUCCESS, 
                           &trans, sizeof(HidTransaction));
                    return;
                }
            }
            //Modify by stanley: 2007-0608
            //if((reportReq.reportId ==1) || (reportReq.reportId ==2))
            {
            /* Make the callback */
                kal_trace(BT_TRACE_G2_PROFILES,RECV_GET_REPORT_WITH_VALID_ID);
            Channel->flags |= CHNL_FLAG_TRANS_INCOMING;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                           BT_STATUS_SUCCESS, &trans, 
                           sizeof(HidTransaction));
            }
            break;
        case HID_MSG_GET_PROTOCOL:
            /* Set up the event for the application */
            trans.type = HID_TRANS_GET_PROTOCOL;
            trans.resultCode = HID_RESULT_SUCCESS;

            /* Make the callback */
            Channel->flags |= CHNL_FLAG_TRANS_INCOMING;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                           BT_STATUS_SUCCESS, &trans, 
                           sizeof(HidTransaction));
            break;
        case HID_MSG_GET_IDLE_RATE:
            /* Set up the event for the application */
            trans.type = HID_TRANS_GET_IDLE_RATE;
            trans.resultCode = HID_RESULT_SUCCESS;

            /* Make the callback */
            Channel->flags |= CHNL_FLAG_TRANS_INCOMING;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                           BT_STATUS_SUCCESS, &trans, 
                           sizeof(HidTransaction));
            break;
        case HID_MSG_SET_PROTOCOL:
            /* Set up the event for the application */
            trans.type = HID_TRANS_SET_PROTOCOL;
            trans.resultCode = HID_RESULT_SUCCESS;
            trans.parm.protocol = Parms->ptrs.data[0] & 0x01;

            /* Make the callback */
            Channel->flags |= CHNL_FLAG_TRANS_INCOMING;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                           BT_STATUS_SUCCESS, &trans, 
                           sizeof(HidTransaction));
            break;
        case HID_MSG_SET_IDLE_RATE:
            /* Set up the event for the application */
            trans.type = HID_TRANS_SET_IDLE_RATE;
            trans.resultCode = HID_RESULT_SUCCESS;
            trans.parm.idleRate = Parms->ptrs.data[1];

            /* Make the callback */
            Channel->flags |= CHNL_FLAG_TRANS_INCOMING;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                           BT_STATUS_SUCCESS, &trans, 
                           sizeof(HidTransaction));
            break;
        case HID_MSG_SET_REPORT:
        case HID_MSG_DATC:
            /* Set up the event for the application */
            report.data = Parms->ptrs.data + 1;
            report.dataLen = Parms->dataLen - 1;
            report.reportType = Parms->ptrs.data[0] & 0x03;
            trans.type = HID_TRANS_SET_REPORT;
            trans.resultCode = HID_RESULT_SUCCESS;
            trans.parm.report = &report;

            if (Parms->dataLen == L2CAP_GetTxMtu(Channel->ctrlCid)) {
                status = BT_STATUS_PENDING;
            } else {
                Channel->flags |= CHNL_FLAG_TRANS_INCOMING;
                status = BT_STATUS_SUCCESS;
            }

            /* Make the callback */
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, 
                           status, &trans, 
                           sizeof(HidTransaction));
            break;
        //Add by stanley 2007-0607    
        case HID_UNSUPPORTED_TRANS_TYPE_20:
        case HID_UNSUPPORTED_TRANS_TYPE_30:
        case HID_UNSUPPORTED_TRANS_TYPE_C0:
        case HID_UNSUPPORTED_TRANS_TYPE_D0:
        case HID_UNSUPPORTED_TRANS_TYPE_E0:
        case HID_UNSUPPORTED_TRANS_TYPE_F0:
            /* Set up the event for the application */
            trans.type = HID_HANDSHAKE; 
            trans.resultCode = HID_RESULT_UNSUPPORTED_REQUEST;
            HidAppCallback(Channel, HIDEVENT_TRANSACTION_IND, BT_STATUS_SUCCESS, 
                           &trans, sizeof(HidTransaction));
            
        }
    }
#endif /* HID_DEVICE == XA_ENABLED */
}

#endif
