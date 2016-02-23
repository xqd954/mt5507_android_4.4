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

#ifndef __OBXALLOC_H
#define __OBXALLOC_H
/****************************************************************************
 *
 * File:          obxalloc.h
 *
 * Description:   This file contains defines the globally accessable RAM
 *                structures and their access functions. The RAM may be
 *                allocated in any file, from the individual OBEX source
 *                code files, to a global allocation file.
 * 
 * Created:       February 12, 2000
 *
 * Version:       MTObex 3.4
 *
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of Extended Systems, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any means, 
 * or stored in a database or retrieval system, without prior written 
 * permission of Extended Systems, Inc.
 * 
 * Use of this work is governed by a license granted by Extended Systems, Inc.  
 * This work contains confidential and proprietary information of Extended 
 * Systems, Inc. which is protected by copyright, trade secret, trademark and 
 * other intellectual property rights.
 *
 ****************************************************************************/
#include <obconfig.h>
#include <sys/obstack.h>

#if OBEX_ROLE_SERVER == XA_ENABLED
extern void ProcessGeneral(ObexServerApp *ObexApp, ObexParserEvent event);
extern void ProcessPut(ObexServerApp *ObexApp, ObexParserEvent event);
extern void ProcessGet(ObexServerApp *ObexApp, ObexParserEvent event);
#if OBEX_SESSION_SUPPORT == XA_ENABLED 
extern void ProcessSession(ObexServerApp *ObexApp, ObexParserEvent event);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
extern void ProcessSimpleOpEvents(ObexClientApp *ObexApp, ObexParserEvent event);
extern void ProcessPutEvents(ObexClientApp *ObexApp, ObexParserEvent event);
extern void ProcessGetEvents(ObexClientApp *ObexApp, ObexParserEvent event);
#if OBEX_SESSION_SUPPORT == XA_ENABLED
extern void ProcessSessionEvents(ObexClientApp *ObexApp, ObexParserEvent event);
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX ROM
 */
#if OBEX_ROLE_SERVER == XA_ENABLED
extern const ServerProcessEvent ServerEventHandlerTable[];
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
extern const ClientProcessEvent ClientEventHandlerTable[];
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
#if OBEX_ROLE_CLIENT == XA_ENABLED
extern const U8 ObClntRxSessionParmsTable[];
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if OBEX_ROLE_SERVER == XA_ENABLED
extern const U8 ObServRxSessionParmsTable[];
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * OBEX RAM
 *
 * All of the OBEX components; Client, Server and Transports, are located
 * via the RAM pointer(s) or structure(s) defined in this section.
 */
#if XA_CONTEXT_PTR == XA_DISABLED
/*
 * Each OBEX modules data is stored in a global structure. This structure
 * is allocated in static RAM which is reserved in the executable.
 */
extern ObInternalData     ObexData;    /* Defined in obstack.h */

#else /* XA_CONTEXT_PTR == XA_DISABLED */
/*
 * The module data is accessed through a global pointer.
 */
extern ObInternalData     *ObexData;

#endif /* XA_CONTEXT_PTR == XA_DISABLED */

/*---------------------------------------------------------------------------
 * OBEX Context Access macros
 *
 * These macros convert to point to or dereference the actual memory.
 */
#if XA_CONTEXT_PTR == XA_DISABLED
#define OBD(f)      (ObexData.f)
#else
#define OBD(f)      (ObexData->f)
#endif /* XA_CONTEXT_PTR == XA_DISABLED */

/*---------------------------------------------------------------------------
 * OBEX Customizations
 *
 * Since this file is included by all source files after all the include
 * files, it is an ideal place to perform module wide customizations that
 * are not performed in the overide.h file.
 * 
 */

#endif /* __OBXALLOC_H */
