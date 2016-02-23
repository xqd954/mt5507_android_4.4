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

void BT_XFile_Init(void);
void BT_XFile_Deinit(void);
U8 BT_XFileGetKey0(void);
U8 BT_XFileGetKey1(void);
U8 BT_XFileGetKey2(void);
U8 BT_XFileGetKey3(void);
void ME_AuthenticationChip(void);
void BT_XfileGenAuthenticationInitKeyStep0(void);
void BT_XfileGenAuthenticationInitKeyStep1(void);
void BT_XfileGenAuthenticationInitKeyStep2(void);
void BT_XfileGenAuthenticationInitKeyStep3(void);
void BT_XfileCalculateSRES0(void);
void BT_XfileCalculateSRES1(void);
void BT_XfileCalculateSRES2(void);
void BT_XfileCalculateSRES3(void);
void BT_XfileGenNewKey0(void);
void BT_XfileGenNewKey1(void);
void BT_XfileGenNewKey2(void);
void BT_XfileGenNewKey3(void);
void BT_XFile_EncryptionCommand(U8 *data, U16 length);
U16 BT_XFile_EncryptionCommandOpcode(U16 opcode);
void BT_XFile_DeEncryptionEvent(U8 *data, U16 length);
U16 BT_XFile_DeOpcodeOpcode(U8 data0, U8 data1);
void BT_XFileHandleGetNewKey(U8 event, U8 len, U8 *parm);
void BT_XFileHandleAuthenticationKey(U8 event, U8 len, U8 *parm);
void BT_Xfile_PrintOpcode(U16 opcode);
U8 BT_CheckSendHostNumberOfCompletedPacket(void);

