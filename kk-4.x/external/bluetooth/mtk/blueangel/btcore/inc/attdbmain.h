/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
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
 * attdbmain.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Low Energy Attribute Protocol database function prototype header
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef __ATTDBMAIN_H
#define __ATTDBMAIN_H

#include "att.h"
#include "att_i.h"
#include "attdef.h"
#include "attdb.h"

void ATTDB_Init(void);

void ATTDB_RegisterBondStatus(BtBleBondCbHandler *cb_func);
void ATTDB_UnRegisterBondStatus(BtBleBondCbHandler *cb_func);


BtStatus ATTDB_AddRecord(const U8 *data, U16 size);
void ATTDB_AddRecordCB(U16 handle, AttdbCallback cb);
BtStatus ATTDB_AddGroupRecord(U16 start_hdl, U16 end_hdl, U16 group_uuid);

BtStatus ATTDB_RemoveRecord(U16 start_hdl, U16 end_hdl);
void ATTDB_RemoveRecordCB(U16 handle);

BtStatus ATTDB_RemoveGroupRecord(U16 start_hdl, U16 end_hdl);

U16 ATTDb_HandleGetInfoReq(U8 *data, U16 mtu, U8 *format, BtATTFindInfoRequestStruct *ptr);
U16 ATTDb_HandleGetFindByTypeValueReq(U8 *data, U16 mtu, BtATTFindByTypeValueRequestStruct *ptr, BtRemoteDevice *link);
U16 ATTDb_HandleReadByTypeReq(U8 *data, U16 mtu, BtATTReadByTypeRequestStruct *ptr, /*U8 *attribute_pair_len, */BtRemoteDevice *link, U8 *error_code, U16 *error_hdl);
U16 ATTDb_HandleReadReq(U8 *data, U16 mtu, BtATTReadRequestStruct *ptr, U8 *error_code, BtRemoteDevice *link);
U16 ATTDb_HandleReadBlobReq(U8 *data, U16 mtu, BtATTReadBlobRequestStruct *ptr, U8 *error_code, BtRemoteDevice *link);
U16 ATTDb_HandleReadMultipleReq(U8 *data, U16 mtu, BtATTReadMultipleRequestStruct *ptr, U8 *error_code, U16 *error_hdl, BtRemoteDevice *link);
U16 ATTDb_HandleReadGroupReq(U8 *data, U16 mtu, BtATTReadByGroupTypeRequestStruct *ptr, U8 *error_code, U16 *error_hdl, BtRemoteDevice *link);
U8 ATTDb_HandleWriteRequest(BtATTWriteRequestStruct *ptr, BtRemoteDevice *link, BOOL need_rsp);
void ATTDb_HandleSignedWriteCommand(U8 *data, BtATTSignedWriteCommandStruct *ptr, BtRemoteDevice *link);
U8 ATTDb_HandlePrepareWriteRequest(BtATTPrepareWriteRequestStruct *ptr, BtRemoteDevice *link);
U8 ATTDb_CheckHandlePermission(BtRemoteDevice *link, U16 recordHandle, bt_att_op op);
U8 ATTDB_HandleExecuteWriteRequest(BtAttPrepareWrite *ptr, BtRemoteDevice *link, U8 cancel);

void ATTDB_NotifyBondStatus(const U8 *bd_addr, U8 status);

#endif

