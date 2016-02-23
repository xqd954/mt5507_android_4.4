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

#ifndef __OBEXERRH
#define __OBEXERRH

/****************************************************************************
 *
 * File:          obexerr.h
 *
 * Description:   This file specifies the OBEX Protocol error codes
 * 
 * Created:       August 14, 1997
 *
 * Version:       MTObex 3.4
 *
 * Copyright 1997-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
#include <xatypes.h>

/*---------------------------------------------------------------------------
 * OBEX Errors layer
 *
 *     This section defines error codes used by OBEX.
 */

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------
 * ObexRespCode type
 *
 *     Response codes used to signify the status of an OBEX operation. They
 *     are not standard function return codes, but rather are returned by
 *     ObStore functions to then be included within OBEX Response packets,
 *     which are sent out to other devices.
 *
 *     The first two response codes listed below are success responses;
 *     the remainder are considered failures. The failure codes are
 *     frequently associated with aborted operations.
 */
typedef U8 ObexRespCode;

/* Group: Successful response codes */

#define OBRC_CONTINUE             0x10 /* Continue */
#define OBRC_SUCCESS              0x20 /* OK, Success */

/* Group: Failure response codes */

#define OBRC_CREATED              0x21 /* Created */
#define OBRC_ACCEPTED             0x22 /* Accepted */
#define OBRC_NON_AUTHOR_INFO      0x23 /* Non-Authoritative Information */
#define OBRC_NO_CONTENT           0x24 /* No Content */
#define OBRC_RESET_CONTENT        0x25 /* Reset Content */
#define OBRC_PARTIAL_CONTENT      0x26 /* Partial Content */

#define OBRC_MULTIPLE_CHOICES     0x30 /* Multiple Choices */
#define OBRC_MOVED_PERMANENT      0x31 /* Moved Permanently */
#define OBRC_MOVED_TEMPORARY      0x32 /* Moved Temporarily */
#define OBRC_SEE_OTHER            0x33 /* See Other */
#define OBRC_NOT_MODIFIED         0x34 /* Not Modified */
#define OBRC_USE_PROXY            0x35 /* Use Proxy */

#define OBRC_BAD_REQUEST          0x40 /* Bad Request */
#define OBRC_UNAUTHORIZED         0x41 /* Unauthorized */
#define OBRC_PAYMENT_REQUIRED     0x42 /* Payment Required */
#define OBRC_FORBIDDEN            0x43 /* Forbidden - operation is understood
                                        * but refused */
#define OBRC_NOT_FOUND            0x44 /* Not Found */
#define OBRC_METHOD_NOT_ALLOWED   0x45 /* Method Not Allowed */
#define OBRC_NOT_ACCEPTABLE       0x46 /* Not Acceptable */
#define OBRC_PROXY_AUTHEN_REQ     0x47 /* Proxy Authentication Required */
#define OBRC_REQUEST_TIME_OUT     0x48 /* Request Timed Out */
#define OBRC_CONFLICT             0x49 /* Conflict */

#define OBRC_GONE                 0x4a /* Gone */
#define OBRC_LENGTH_REQUIRED      0x4b /* Length Required */
#define OBRC_PRECONDITION_FAILED  0x4c /* Precondition Failed */
#define OBRC_REQ_ENTITY_TOO_LARGE 0x4d /* Requested entity is too large */
#define OBRC_REQ_URL_TOO_LARGE    0x4e /* Requested URL is too large */
#define OBRC_UNSUPPORT_MEDIA_TYPE 0x4f /* Unsupported Media Type */

#define OBRC_INTERNAL_SERVER_ERR  0x50 /* Internal Server Error */
#define OBRC_NOT_IMPLEMENTED      0x51 /* Not Implemented */
#define OBRC_BAD_GATEWAY          0x52 /* Bad Gateway */
#define OBRC_SERVICE_UNAVAILABLE  0x53 /* Service Unavailable */
#define OBRC_GATEWAY_TIMEOUT      0x54 /* Gateway Timeout */
#define OBRC_HTTP_VER_NO_SUPPORT  0x55 /* HTTP version not supported */

#define OBRC_DATABASE_FULL        0x60 /* Database Full */
#define OBRC_DATABASE_LOCKED      0x61 /* Database Locked */

/* End of ObexRespCode */

#endif /* __OBEXERRH */

