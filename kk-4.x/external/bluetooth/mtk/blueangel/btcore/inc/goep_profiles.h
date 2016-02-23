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

#ifndef __GOEPPROFILES_H
#define __GOEPPROFILES_H
/****************************************************************************
 *
 * File:          goepProfiles.h
 *
 * Description:   This file specifies defines and function prototypes for the
 *                GOEP Profiles sample application.
 * 
 * Created:       July 25, 2002
 *
 * $Project:XTNDAccess IrDA SDK$
 *
 * Copyright 1999-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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

/****************************************************************************
 *
 * GOEP Profile Exported Function prototypes
 *
 ***************************************************************************/
extern BOOL GPA_Init(U8 Inst);
extern BOOL GPA_Deinit(void);
extern void GPA_Connect(ObexTpAddr *Target);
extern void GPA_Disconnect(void);
extern void GPA_AbortClient(void);
extern void GPA_AbortServer(void);
extern void GPA_Push(const char *ObName);
extern void GPA_Pull(const char *ObName, const char *ObType);
extern void GPA_PullFolder(const char *ObName);
extern void GPA_SetFolder(const char *PathName, BOOL Backup, BOOL NoCreate, BOOL Reset);
extern void GPA_ObjectPush(const char *ObName, ObexTpAddr *Target);
extern void GPA_BizExchange(const char *ObName, ObexTpAddr *Target);
extern void GPA_SetPassword(void);
extern void GPA_SetUserId(void);

#define AUTH_RECEIVED_CHAL  0x01
#define AUTH_AUTHENTICATED  0x02

#define GPA_IDLE             0x00   /* OBEX and Transport disconnected */   
#define GPA_CONNECTING       0x01   /* OBEX connecting */
#define GPA_TP_CONNECTED     0x02   /* Transport connected */
#define GPA_CONNECTED        0x03   /* OBEX connected */
#define GPA_DISCONNECTING    0x04   /* OBEX disconnecting */   
#define GPA_TP_DISCONNECTING 0x05   /* Transport disconnecting */

#endif /* __GOEPPROFILES_H */

