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

#ifndef __ATPI_H
#define __ATPI_H
/****************************************************************************
 *
 * File:
 *     $Workfile:atpi.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an AT Command Processor.
 *             
 * Copyright 2002-2005 Extended Systems, Inc.

 * Portions copyright 2005 iAnywhere Solutions, Inc.

 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,  
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
#define AT_GROUP_COMMON         0x0000
#define AT_GROUP_HEADSET        0x0100
#define AT_GROUP_HANDSFREE      0x0200
#define AT_GROUP_PHONEBOOK      0x0300
#define AT_GROUP_SMS            0x0400
#define AT_GROUP_DUN            0x0500

/* Maximum result "type" string size. Based on "NO DIALTONE\0". */
#define RESULT_STAGE_LEN        12

/* Maximum command "type" string size. Based on "+CIND\0". */
#define COMMAND_STAGE_LEN       6

/* Maximum supported indicator string size. Based on "call_setup\0" */
#define INDICATOR_STAGE_LEN     11

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
/* Internal parsing structures */                                 
typedef struct _AtParameter {
    U16     offset;
    U8      len;
} AtParameter;

/****************************************************************************
 *
 * Function Prototypes
 *
 ****************************************************************************/
#if AT_HEADSET == XA_ENABLED
/*
 * Headset Group definitions
 */
extern const char *AT_HeadsetCmds[];

AtStatus TE_Encode_Headset(AtContext *, U16, const AtHeadsetCmd *, XaBufferDesc *);
AtStatus TE_Decode_Headset(AtContext *, U16 *, AtHeadsetResult *, XaBufferDesc *);
AtStatus ME_Encode_Headset(AtContext *, U16, const AtHeadsetResult *, XaBufferDesc *);
AtStatus ME_Decode_Headset(AtContext *, U16 *, AtHeadsetCmd *, XaBufferDesc *);
#else /* AT_HEADSET == XA_ENABLED */
#define AT_HeadsetCmds          0
#endif /* AT_HEADSET == XA_ENABLED */

#if AT_HANDSFREE == XA_ENABLED
/*
 * Handsfree Group definitions
 */
extern const char *AT_HandsfreeCmds[];

AtStatus TE_Encode_Handsfree(AtContext *, U16, const AtHandsfreeCmd *, XaBufferDesc *);
AtStatus TE_Decode_Handsfree(AtContext *, U16 *, AtHandsfreeResult *, XaBufferDesc *);
AtStatus ME_Encode_Handsfree(AtContext *, U16, const AtHandsfreeResult *, XaBufferDesc *);
AtStatus ME_Decode_Handsfree(AtContext *, U16 *, AtHandsfreeCmd *, XaBufferDesc *);
#else /* AT_HANDSFREE == XA_ENABLED */
#define AT_HandsfreeCmds        0
#endif /* AT_HANDSFREE == XA_ENABLED */

#if AT_PHONEBOOK == XA_ENABLED
/*
 * Phonebook Group definitions
 */
extern const char *AT_PhonebookCmds[];

AtStatus TE_Encode_Phonebook(AtContext *, U16, const AtPhonebookCmd *, XaBufferDesc *);
AtStatus TE_Decode_Phonebook(AtContext *, U16 *, AtPhonebookResult *, XaBufferDesc *);
AtStatus ME_Encode_Phonebook(AtContext *, U16, const AtPhonebookResult *, XaBufferDesc *);
AtStatus ME_Decode_Phonebook(AtContext *, U16 *, AtPhonebookCmd *, XaBufferDesc *);
#else /* AT_PHONEBOOK == XA_ENABLED */
#define AT_PhonebookCmds        0
#endif /* AT_PHONEBOOK == XA_ENABLED */

#if AT_SMS == XA_ENABLED
/*
 * Short Message Service Group definitions
 */
extern const char *AT_SmsCmds[];

AtStatus TE_Encode_Sms(AtContext *, U16, const AtSmsCmd *, XaBufferDesc *);
AtStatus TE_Decode_Sms(AtContext *, U16 *, AtSmsResult *, XaBufferDesc *);
AtStatus ME_Encode_Sms(AtContext *, U16, const AtSmsResult *, XaBufferDesc *);
AtStatus ME_Decode_Sms(AtContext *, U16 *, AtSmsCmd *, XaBufferDesc *);
#else /* AT_SMS == XA_ENABLED */
#define AT_SmsCmds              0
#endif /* AT_SMS == XA_ENABLED */

#if AT_DUN == XA_ENABLED
/*
 * Dialup Networking Group definitions
 */
extern const char *AT_DunCmds[];

AtStatus TE_Encode_Dun(AtContext *, U16, const AtDunCmd *, XaBufferDesc *);
AtStatus TE_Decode_Dun(AtContext *, U16 *, AtDunResult *, XaBufferDesc *);
AtStatus ME_Encode_Dun(AtContext *, U16, const AtDunResult *, XaBufferDesc *);
AtStatus ME_Decode_Dun(AtContext *, U16 *, AtDunCmd *, XaBufferDesc *);
#else /* AT_DUN == XA_ENABLED */
#define AT_DunCmds              0
#endif /* AT_DUN == XA_ENABLED */

/*
 * Common Group definitions
 */
extern const char *AT_CommonCmds[];

AtStatus TE_Encode_Common(AtContext *, U16, const AtCommands *, XaBufferDesc *);
AtStatus TE_Decode_Common(AtContext *, U16 *, AtResults *, XaBufferDesc *);
AtStatus ME_Encode_Common(AtContext *, U16, const AtResults *, XaBufferDesc *);
AtStatus ME_Decode_Common(AtContext *, U16 *, AtCommands *, XaBufferDesc *);


U8   AtItoA(U8 *buff, U32 val);
U32  AtAtoI(const U8 *a, U8 len);
U8   AtParseParameters(U8 *input, U16 inputLen, AtParameter parms[AT_MAX_PARMS]);
void AtRemoveWhiteSpace(XaBufferDesc *Input);
void AtDebugOut(XaBufferDesc *Input);
char *AtMakeString(U8 *String, U16 Length);

#if XA_DEBUG_PRINT != XA_ENABLED
#define AtDebugOut(A)       (void)0
#endif

#endif /* __ATPI_H */
