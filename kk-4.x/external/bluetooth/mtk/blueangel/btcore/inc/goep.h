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

#ifndef __GOEP_H
#define __GOEP_H

/****************************************************************************
 *
 * File:
 *     $Workfile$ for XTNDAccess Blue SDK, Version 1.3
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Generic Object Exchange Profile program interface.
 * 
 * Copyright 2000-2005 Extended Systems, Inc.  ALL RIGHTS RESERVED.
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
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * Nov 27 2008 mbj06038
 * [MAUI_01342148] [New feature][SyncML] Device sync support
 * 
 *
 * Nov 26 2008 mbj06038
 * [MAUI_01342148] [New feature][SyncML] Device sync support
 * 
 *
 * Jul 18 2008 mbj06032
 * [MAUI_00792520] BT-device power off but can connectPBAP
 * notify server when  RFCOMM Open Ind to let user confirm:
 * define "GOEP_EVENT_AUTHORIZE_IND"; implement "GOEP_ServerAuthorizeResponse"
 *
 * Jul 2 2008 mbj06032
 * [MAUI_01171562] MTK:Bluetooth OBEX_it can't connect FTP succeed
 * accept OBEX authentication challenge
 *
 * May 16 2008 mbj06032
 * [MAUI_00976686] MTK:Bluetooth BIP_[1] Fatal Error (207, 26f3990) - BT
 * to reduce system stack size, use dynamic memory, not array
 *
 * Apr 18 2008 mbj06032
 * [MAUI_00241227] MTK:Bluetooth FTP_the file name display incompletely and can't get
 * 
 *
 * Mar 13 2008 mbj06032
 * [MAUI_00281507] MTK OPP: it is abanormal when receive a OPP file
 * clear server.pullpush.name info only after using it
 *
 * Jan 23 2008 mbj06032
 * [MAUI_00258835] MTK BT profile supported on MT6611_ [1] Assert fail: 0 bt_adp_bip.c 820 - BT
 * modify GOEP_NUM_OBEX_CONS from 3 to 4
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 *
 ****************************************************************************/

#include <obex.h>
#include <obexauth.h>

/*---------------------------------------------------------------------------
 * Generic Object Exchange Profile (GOEP) Layer 
 *
 *     The GOEP layer manages all interactions for applications and profiles
 *     layered above OBEX that desire conformance with the Generic Object
 *     Exchange profile. The GOEP layer provides structures for applications
 *     and profiles to use which simplify the sending and receiving of
 *     protocol compliant information via OBEX. In addition, the GOEP
 *     provides a multiplexing layer which provides support for running
 *     multiple GOEP based profiles simultaneously.
 *
 *     The following profiles are based on the GOEP:
 *       - The Object Push Profile (OPUSH)
 *       - The File Transfer Profile (FTP)
 *       - The Synchronization Profile (SYNC)
 */

/****************************************************************************
 *
 * Section: Constants
 *
 ****************************************************************************/

/* ADDED BY yfchu */
#if OBEX_AUTHENTICATION == XA_ENABLED
#define GOEP_AUTH     XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * GOEP_NUM_OBEX_CONS constant
 *
 *     Defines the number of OBEX connections used in GOEP.  One GOEP 
 *     multiplexer is allocated for each OBEX connection.
 */

enum {
#ifdef __BT_OPP_PROFILE__
    GOEP_OBEX_CLIENT_OPP,
#endif
#ifdef __BT_FTC_PROFILE__
    GOEP_OBEX_CLIENT_FTP,
#endif
#ifdef __BT_BPP_PROFILE__
    GOEP_OBEX_CLIENT_BPP,
#endif
#ifdef __BT_BIP_PROFILE__
    GOEP_OBEX_CLIENT_BIP,
#endif
#ifdef __BT_SUPPORT_SYNCML__
    GOEP_OBEX_CLIENT_SYNCML,
#endif
#ifdef __BT_MAPC_PROFILE__
    GOEP_OBEX_CLIENT_MAPC,
#endif
#ifdef __BT_MAPS_PROFILE__
    GOEP_OBEX_CLIENT_MAPS_MNS1,
    GOEP_OBEX_CLIENT_MAPS_MNS2,
#endif

#if !defined(__BT_OPP_PROFILE__) && \
    !defined(__BT_FTC_PROFILE__) && \
    !defined(__BT_BPP_PROFILE__) && \
    !defined(__BT_BIP_PROFILE__) && \
    !defined(__BT_SUPPORT_SYNCML__)
    GOEP_NUM_OBEX_CLIENT_CONS = 1
#else
    GOEP_NUM_OBEX_CLIENT_CONS
#endif
};


enum {
#ifdef __BT_OPP_PROFILE__
    GOEP_OBEX_SERVER_OPP,
#endif
#ifdef __BT_FTS_PROFILE__
    GOEP_OBEX_SERVER_FTP,
#endif
#ifdef __BT_BIP_PROFILE__
    GOEP_OBEX_SERVER_BIP,
#endif
#ifdef __BT_PBAP_PROFILE__
    GOEP_OBEX_SERVER_PBAP,
#endif
#ifdef __BT_SUPPORT_SYNCML__
    GOEP_OBEX_SERVER_SYNCML,
#endif
#ifdef __BT_MAPS_PROFILE__
    GOEP_OBEX_SERVER_MAPS1_1,
    GOEP_OBEX_SERVER_MAPS1_2,
    GOEP_OBEX_SERVER_MAPS2_1,
    GOEP_OBEX_SERVER_MAPS2_2,
    GOEP_OBEX_SERVER_MAPS3_1,
    GOEP_OBEX_SERVER_MAPS3_2,
    GOEP_OBEX_SERVER_MAPS4_1,
    GOEP_OBEX_SERVER_MAPS4_2,
#endif
#if !defined(__BT_OPP_PROFILE__) && \
    !defined(__BT_FTS_PROFILE__) && \
    !defined(__BT_BIP_PROFILE__) && \
    !defined(__BT_PBAP_PROFILE__) && \
    !defined(__BT_SUPPORT_SYNCML__)
    GOEP_NUM_OBEX_SERVER_CONS = 1
#else    
    GOEP_NUM_OBEX_SERVER_CONS
#endif
};

#if 0
#ifndef GOEP_NUM_OBEX_CONS
#define GOEP_NUM_OBEX_CONS   4
#endif

#if GOEP_NUM_OBEX_CONS < 1
#error GOEP_NUM_OBEX_CONS must be greater than 0
#endif
#endif

/*---------------------------------------------------------------------------
 * GOEP_MAX_UNICODE_LEN constant
 *
 *     Defines the maximum number of characters possible in an object, 
 *     folder name, or queued unicode header (including the null-terminator). 
 *     The maximum value is 32,767 (or 0x7FFF). This value must be greater 
 *     than zero, however, it will in all likelihood be larger than one, 
 *     since most filenames exceed one byte in length.
 */
#ifndef GOEP_MAX_UNICODE_LEN
#define GOEP_MAX_UNICODE_LEN   256 /*128*/
#endif

#if GOEP_MAX_UNICODE_LEN < 1 || GOEP_MAX_UNICODE_LEN > 0x7FFF
#error GOEP_MAX_UNICODE_LEN must be between 1 and 32,767
#endif

/*---------------------------------------------------------------------------
 * GOEP_MAX_TYPE_LEN constant
 *
 *     Defines the maximum size, in characters of an object type (including
 *     the null-terminator). Default max type is 'text/x-vcard'. If set to
 *     zero, TYPE headers are not stored. The maximum value is 255.
 */
#ifndef GOEP_MAX_TYPE_LEN
#define GOEP_MAX_TYPE_LEN   80
#endif

/*---------------------------------------------------------------------------
 * GOEP_MAX_WHO_LEN constant
 *
 *     Defines the maximum size, in characters of a WHO header. This size
 *     must be large enough to receive the largest WHO header which we are
 *     interested in. The WHO header is expected when we send a TARGET 
 *     header in the GoepConnectReq and the server has a matching service.
 *     If set to zero, WHO headers are not stored. The maximum value is 255.
 */
#ifndef GOEP_MAX_WHO_LEN 
#define GOEP_MAX_WHO_LEN    0
#endif

/*---------------------------------------------------------------------------
 * GOEP_DOES_UNICODE_CONVERSIONS constant
 *
 *     Defines the format of UNICODE data passed between the GOEP API and the
 *     layered application or profile. If enabled, the GOEP layer expects
 *     all Name and Description data pointers (such as in GoepPushReq) as 
 *     well as all queued unicode header data pointers (such as a unicode 
 *     header queued via GOEP_ClientQueueHeader or GOEP_ServerQueueHeader) to 
 *     point to an ASCII string. In this case the GOEP layer will convert the 
 *     string to UNICODE before transmission. Similarly all received UNICODE 
 *     strings are converted to ASCII. If disabled, all UNICODE headers are 
 *     expected to be provided in UNICODE format.
 */
#ifndef GOEP_DOES_UNICODE_CONVERSIONS
#define GOEP_DOES_UNICODE_CONVERSIONS   XA_DISABLED
#endif

/*---------------------------------------------------------------------------
 * GOEP_ADDITIONAL_HEADERS constant
 *
 *     Defines the number of additional headers allowed per GOEP connection. 
 *     Additional headers are sent when a GOEP operation is sent or responded 
 *     to.
 *
 *     NOTE: Larger numbers of additional headers are allowed, but caution 
 *           should be done in allowing this, since some devices may support
 *           only the minimum OBEX packet size (255 bytes).
 *           
 */
#ifndef GOEP_ADDITIONAL_HEADERS
#if BT_GOEP_V2 == XA_ENABLED
/* for SRM & SRM_PARAM header */
#define GOEP_ADDITIONAL_HEADERS     5
#else
#define GOEP_ADDITIONAL_HEADERS     3
#endif
#endif

#if GOEP_ADDITIONAL_HEADERS > 255
#error GOEP_ADDITIONAL_HEADERS must be less than 256
#endif

/*---------------------------------------------------------------------------
 * GOEP_CLIENT_HB_SIZE constant
 * 
 *     Transmit Header block construction buffer size. The upper limit on the
 *     header block size is the OBEX packet size. For RAM efficiency this
 *     value should be calculated as follows. The most header data the GOEP
 *     will ever send is in a client PUT which sends a Name header and a
 *     Length header. The max name length GOEP_MAXIMUM_NAME_LEN times 2 for
 *     UNICODE + 3 bytes for the OBEX Name header. Add in a length header
 *     (5 bytes) and you get the total. 
 *     Now when authentication is enabled there is the issue of space for
 *     the Authentication Response and/or Challenge header(s). There is
 *     likely enough space already allocated by for the NAME & TYPE headers
 *     that so long as they aren't all in the same packet, the is no need
 *     to allocate additional space.
 */
#ifndef GOEP_CLIENT_HB_SIZE
#define GOEP_CLIENT_HB_SIZE   ((GOEP_MAX_UNICODE_LEN*2)+3+5+GOEP_MAX_TYPE_LEN)
#endif

#if GOEP_CLIENT_HB_SIZE > 0xFFFF
#error GOEP_CLIENT_HB_SIZE must be less than 65,535
#endif

#if GOEP_CLIENT_HB_SIZE == 0
#error GOEP_CLIENT_HB_SIZE must be greater than 0
#endif

/*---------------------------------------------------------------------------
 * GOEP_SERVER_HB_SIZE constant
 * 
 *     The server transmit header block must be large enough to send an
 *     OBEX length header (5 bytes). If Authentication is enabled, it must
 *     be large enough to hold an AuthResponse (18+18+22) or an AuthChallenge
 *     (18+3+MaxRealmLength). If we make the Header block the size of the
 *     AuthResponse then that leaves 35 bytes for MaxRealmLength, which seems
 *     like a comfortable value. So that's what is done here. (The server
 *     never sends both AuthResponse & AuthChallenge in one packet).
 */
#ifndef GOEP_SERVER_HB_SIZE
#if OBEX_AUTHENTICATION == XA_ENABLED
#define GOEP_SERVER_HB_SIZE   (5 + (18+18+22))
#else
#define GOEP_SERVER_HB_SIZE   (5)
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
#endif

#if GOEP_SERVER_HB_SIZE > 0xFFFF
#error GOEP_SERVER_HB_SIZE must be less than 65,535
#endif

#if GOEP_SERVER_HB_SIZE == 0
#error GOEP_SERVER_HB_SIZE must be greater than 0
#endif


#if GOEP_AUTH == XA_ENABLED

#define MAX_AUTHCHAL_PROFILES_NUM   2

#endif

/****************************************************************************
 *
 * Section: Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GoepOperation type
 *
 *     The GoepOperation type defines operations based on the Generic
 *     Object Exchange Profile's operations.
 */
typedef U8 GoepOperation;

/* This value is used when indicating events which do not occur during
 * an operation, such as TP Connect and TP Disconnect.
 */
#define GOEP_OPER_NONE              0

/* A Connect operation is in progress. The info.connect field in the
 * event structure is valid.
 */
#define GOEP_OPER_CONNECT           1

/* A Disconnect operation is in progress. */
#define GOEP_OPER_DISCONNECT        2

/* A Push operation is in progress. The info.pushpull or info.push field
 * in the event structure is valid.
 */
#define GOEP_OPER_PUSH              3

/* A Delete operation is in progress. The info.pushpull field in the 
 * event structure is valid.
 */
#define GOEP_OPER_DELETE            4

/* A Push operation is in progress. The info.pushpull field in the
 * event structure is valid.
 */
#define GOEP_OPER_PULL              5

/* A Set folder operation is in progress. The info.setfolder field in the
 * event structure is valid.
 */
#define GOEP_OPER_SETFOLDER         6

/* An abort operation is in progress. This event will be delivered while
 * another operation is in progress, if the ABORT is sent to stop that
 * operation.
 */
#define GOEP_OPER_ABORT             7

/* A Create-empty PUT is in progress. The info.pushpull field in the 
 * event structure is valid.
 */
#define GOEP_OPER_CREATEEMPTY       8

/* A Set folder operation is in progress. The info.setAction field in the
 * event structure is valid.
 */
#define GOEP_OPER_ACTION            9

/* End of GoepOperation */

/*---------------------------------------------------------------------------
 * GoepEventType type
 *
 *     The GoepEventType type defines the events that may be indicated to
 *     the Client and Server applications.
 */
typedef U8 GoepEventType;

/* Reserved event type, unused by the GOEP protocol. 
 */
#define GOEP_EVENT_NONE             0

/* Indicates the start of a new operation (Server Only).  In the case of an Abort
 * operation, this event may occur during another pending operation (Put or Get).
 * In the case where GOEP_ServerContinue() has not been called during a Put or 
 * Get, perhaps awaiting Server object creation or additional buffer space, it 
 * should be called now or the Abort operation will not complete.
 */
#define GOEP_EVENT_START            1

/* Delivered to the application when it is time to issue
 * another request or response. The application must call either
 * GOEP_ServerContinue() or GOEP_ClientContinue().
 */
#define GOEP_EVENT_CONTINUE         2

#if OBEX_AUTHENTICATION == XA_ENABLED
/* Indicates that an authentication challenge request has
 * been received and is available in the event challenge field. If
 * the application intends on responding to this challenge it should do
 * so during this event notification. This is because there is a risk
 * that the peer may send more than one challenge and the GOEP layer
 * only tracks the last challenge received. However, it is not required
 * that the application respond during the event, as long as the
 * application always responds to the last challenge received.
 */
#define GOEP_EVENT_AUTH_CHALLENGE   3

/* Indicates that an authentication response has been
 * received and is available in the event response field.
 */
#define GOEP_EVENT_AUTH_RESPONSE    4
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/* An OBEX header was received that was not handled internally in GOEP.
 */
#define GOEP_EVENT_HEADER_RX        5

/* Delivered to server applications before the last response is going
 * to be sent. It provides the server with an opportunity to complete 
 * the requested operation (such as a SetFolder request) and if necessary
 * set the response code before the final response is sent (Server Only).
 */
#define GOEP_EVENT_PRECOMPLETE      6

/* Instructs the server to provide an object store handle to the protocol.
 * This event is generated during the processing of Put and Get operations.
 * The server calls GOEP_ServerAccept() to provide the object. Failure to 
 * do so will cause the operation to be rejected by the protocol. (Server Only)
 */
#define GOEP_EVENT_PROVIDE_OBJECT   7

/* Instructs the server to delete an object. This event is generated 
 * during the processing of a Put-Delete operation. The delete is assumed
 * to be successful, so the server must call GOEP_ServerAbort() if the 
 * object could not be deleted. 
 */
#define GOEP_EVENT_DELETE_OBJECT    8

/* Indicates that the operation has completed successfully.
 */
#define GOEP_EVENT_COMPLETE         9

/* Indicates that the current operation has failed.
 */
#define GOEP_EVENT_ABORTED          10

/* A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define GOEP_EVENT_TP_CONNECTED     11

/* The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define GOEP_EVENT_TP_DISCONNECTED  12

/* The transport layer discovery process has failed. (Client only)
 */
#define GOEP_EVENT_DISCOVERY_FAILED 13

/* The transport layer OBEX service cannot be found. (Client only)
 */
#define GOEP_EVENT_NO_SERVICE_FOUND 14

/* The connect request is rejected for hardware limitation (SCO is running)
 */
#define GOEP_EVENT_SCO_REJECT 15

/* Instructs the server to create an empty object. This event is generated 
 * during the processing of a Put-Delete operation. 
 */
#define GOEP_EVENT_CREATE_EMPTY_OBJECT    16

/* The connect request is rejected for hardware limitation (SCO is running)
 */
#define GOEP_EVENT_BTCHIP_REJECT 17


/* Authorize indication
 */
#define GOEP_EVENT_AUTHORIZE_IND  18


/* End of GoepEventType */


typedef U8 GoepTpType; /* transparent type */
#define GOEP_TP_RFCOMM_ONLY  OBEX_TP_RFCOMM_ONLY
#define GOEP_TP_L2CAP_ONLY   OBEX_TP_L2CAP_ONLY
#define GOEP_TP_BOTH         OBEX_TP_BOTH


/*---------------------------------------------------------------------------
 * GoepUniType type
 *
 *     This type is used for parameters that are encoded in UNICODE when
 *     sent over OBEX. The format of these parameters depends on whether
 *     the GOEP layer or the application is handling the UNICODE formatting.
 */
#if GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED
typedef U8      GoepUniType;
#else
typedef U16     GoepUniType;
#endif /* GOEP_DOES_UNICODE_CONVERSIONS == XA_ENABLED */

/* End of GoepUniType */

/*---------------------------------------------------------------------------
 * GoepProfile type
 *
 *     The GOEP Profile types are used in the 'type' parameter of the client
 *     and server registration structures.
 */
typedef U8 GoepProfile;

#define GOEP_PROFILE_OPUSH          0   /* Object Push profile */
#define GOEP_PROFILE_FTP            1   /* File Transfer profile */
#define GOEP_PROFILE_SYNC           2   /* Sync profile */
#define GOEP_PROFILE_BPP            3   /* Basic Printing profile */
#define GOEP_PROFILE_BPP_PBR        4   /* Basic Printing profile PBR */
#define GOEP_PROFILE_BIP            5   /* Basic Imaging profile */
#define GOEP_PROFILE_PBAP           6   /* Phonebook Access profile */
#define GOEP_PROFILE_SYNCML_CLIENT  7   /* SyncML client */
#define GOEP_PROFILE_SYNCML_SERVER  8   /* SyncML server */
#define GOEP_PROFILE_MAP_CLIENT     9   /* Map client */
#define GOEP_PROFILE_MAP_SERVER1    10  /* Map server */
#define GOEP_PROFILE_MAP_SERVER2    11  /* Map server */
#define GOEP_PROFILE_MAP_SERVER3    12  /* Map server */
#define GOEP_PROFILE_MAP_SERVER4    13  /* Map server */
#define GOEP_PROFILE_MNS_CLIENT1    14  /* Map client */
#define GOEP_PROFILE_MNS_CLIENT2    15  /* Map client */
#define GOEP_MAX_PROFILES           16  /* Number of supported profiles */
//#define GOEP_MAX_PROFILES       1   /* To avoid the incoming request with different profile using same OBEX channel, we limit it to 1 */

/* End of GoepProfile */

/*---------------------------------------------------------------------------
 * GoepConnTypes
 *
 *     Generic Object Exchange connection types for connection management 
 *     (for Client and Server)
 */
typedef U8 GoepConnTypes;

#define GOEP_MUX_CONN       0x01    /* Multiplex an existing OBEX connection */
#define GOEP_NEW_CONN       0x02    /* Start a new OBEX connection, don't multiplex */

/* End of GoepConnTypes */

/*---------------------------------------------------------------------------
 * GoepRole
 *
 *     Generic Object Exchange Roles
 */
typedef U8 GoepRole;

#define GOEP_SERVER         0x01
#define GOEP_CLIENT         0x02
 
/* End of GoepRole */

/* Forward declaration of the callback parameters */
#if OBEX_ROLE_SERVER == XA_ENABLED
typedef struct _GoepServerEvent GoepServerEvent;

/*---------------------------------------------------------------------------
 * GoepServerCallback type
 *
 *     A function of this type is called to indicate GOEP events to the
 *     GOEP Server application. When the GOEP layer calls this function
 *     it provides a server event structure which contains information
 *     about the event. The EventType, Operation and ServerApp fields
 *     are always valid. Based on the value of 'oper' the corresponding
 *     'info' subfield can also be interpreted. The values in the 'info'
 *     subfield are valid for the duration of the operation (from GOEP_EVENT
 *     START to GOEP_EVENT_COMPLETE (or GOEP_EVENT_ABORTED).
 */
typedef void (*GoepServerCallback)(GoepServerEvent *Event);
/* End of GoepServerCallback type */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
typedef struct _GoepClientEvent GoepClientEvent;

/*---------------------------------------------------------------------------
 * GoepClientCallback type
 *
 *     A function of this type is called to indicate GOEP events to the
 *     GOEP Client application. When the GOEP layer calls this function
 *     it provides a client event structure which contains information
 *     about the event. The EventType, Operation and ClientApp fields
 *     are always valid. Based on the value of 'oper' the corresponding
 *     'info' subfield can also be interpreted. The values in the 'info'
 *     subfield are valid for the duration of the operation (from GOEP_EVENT
 *     START to GOEP_EVENT_COMPLETE (or GOEP_EVENT_ABORTED).
 */
typedef void (*GoepClientCallback)(GoepClientEvent *Event);
/* End of GoepClientCallback type */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * SEC_GOEP_ID
 *
 * Security ID for GOEP access.
 */
#define SEC_GOEP_ID (BtProtocolId)GoepSecCallback
void GoepSecCallback(const BtEvent *Event);
/* End of GoepSecCallback */
#endif /* BT_SECURITY == XA_ENABLED */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GoepHeaderInd structure
 * 
 *     This structure is used by the during Header indications.
 */
typedef struct _GoepHeaderInd {
    ObexHeaderType type;            /* OBEX Header Type */

    /* Group: These fields are valid for 1-byte and 4-byte headers only */
    U32            value;           /* 1-byte or 4-byte header value */

    /* Group: These fields are valid for Unicode and Byte-Sequence headers only */
    U8            *buffer;          /* Header buffer */
    U16            len;             /* Length of the header buffer */

    /* Group: These fields are valid for Byte-Sequence headers only */
    U16            totalLen;        /* Total header length */
    U16            remainLen;       /* Remaining header length */

    /* Group: These fields are valid for Unicode headers only */

#ifdef __GOEP_USE_ARRAY_HEADER_BUFFER__
    /* The format of the unicode buffer (ASCII/UNICODE) is determined by the 
     * GOEP_DOES_UNICODE_CONVERSIONS option. 
     */
    GoepUniType    unicodeBuffer[GOEP_MAX_UNICODE_LEN]; /* "buffer" points to this data */
#endif

} GoepHeaderInd;

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * GoepServerApp structure
 *
 *     This structure is used by the Server Application (or Profile) to
 *     register with the GOEP Server multiplexer. If target information
 *     is provided, the GOEP Server takes care of registering it. 
 *
 *     If registering directly, using the GOEP_RegisterServer() function all
 *     fields must be filled in prior to registration.
 *
 *     If registering via a profile manager, such as GOEP_RegisterFtpServer()
 *     only the 'callback' field should be filled in.
 */
typedef struct _GoepServerApp {
    /* Group: These fields must be filled in prior to registration */

    GoepServerCallback      callback;
    
    /* Group: These fields must be filled in only if using GOEP_RegisterServer() */
    GoepProfile             type;       /* GOEP_PROFILE_xxx define */
    U8             bt_service;
    /* This callback is handled by MMI message handler */

#if OBEX_SERVER_CONS_SIZE > 0
    ObexConnection         *target[OBEX_SERVER_CONS_SIZE];
    U32                     numTargets; /* Must be set to zero if unused */
#endif /* OBEX_SERVER_CONS_SIZE > 0 */
    GoepConnTypes           connFlags;  /* Type of connection - new or multiplexed */
    /* This field must be filled in if connFlags is GOEP_MUX_CONN.  It will be
     * filled in automatically otherwise. 
     */
    U8                      connId;     /* Id of the Obex connection - existing or new */
    /* === Internal use only === */
#if BT_SECURITY == XA_ENABLED
    BtSecurityRecord        secRecord;
    BtSecurityToken         secToken;
#endif /* BT_SECURITY == XA_ENABLED */
    U32                     cm_conn_id;      /* cm connection id that is used for CM */
#if OBEX_AUTHENTICATION == XA_ENABLED
    /* If enabled, these two fields are used to challenge the client and
     * to respond to a received challenge request.
     */
    ObexAuthChallenge   *challenge;
    ObexAuthResponse    *response;
    U8          *password;
    U8           passwordLen;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if GOEP_AUTH == XA_ENABLED
    BOOL   bAuthChallenged;
#endif

    ObexL2BufferData l2data;

    BOOL                  abortFlag;
    U8                    abortReason;
    void                  *object;
    U32                    lastprogress;  /* Avoid too many transmission progress update message*/    
} GoepServerApp;

#if GOEP_MAX_WHO_LEN > 0
/*---------------------------------------------------------------------------
 * GoepConnectInd structure
 * 
 *     This structure is used by the GOEP Server during OBEX Connect event
 *     indications.
 */
typedef struct _GoepConnectInd {
    /* If the client specified a WHO header in the OBEX Connect request,
     * this field contains that information. 'whoLen' specifies its length.
     */
    U8                  who[GOEP_MAX_WHO_LEN];
    U16                 whoLen;
} GoepConnectInd;
#endif /* GOEP_MAX_WHO_LEN > 0 */

/*---------------------------------------------------------------------------
 * GoepObjectInd structure
 *
 *     Provides information to a GOEP Server about a received PUSH,
 *     PULL or DELETE indication.
 */
typedef struct _GoepObjectInd {
    /* Name of the object, null-terminated. The format (ASCII/UNICODE)
     * is determined by the GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType     name[GOEP_MAX_UNICODE_LEN];

    /* Length (in bytes) of the 'name' field. */
    U16             nameLen;

#if GOEP_MAX_TYPE_LEN > 0
    /* Type of the object, null-terminated, in ASCII format.
     */
    U8              type[GOEP_MAX_TYPE_LEN];

    /* Length (in bytes) of the 'type' field. */
    U16             typeLen;
#endif /* GOEP_MAX_TYPE_LEN > 0 */

    /* In a PUSH indication, this field indicates the length of the 
     * incoming object. In a PULL indication, this field should be set
     * by the user during the PROVIDE_OBJECT event, to the length of the object.
     * If it's zero, either the object has a zero length or the length
     * information was not available. This field is not used by DELETE.
     */
    U32             objectLen;
    BOOL            finalBit;
} GoepObjectInd;

/*---------------------------------------------------------------------------
 * GoepSetFolderInd structure
 * 
 *     This structure is used by the Server during SetFolder indications.
 */
typedef struct _GoepSetFolderInd {
    /* The client requested a reset to the root folder. */
    BOOL                reset;

    /* OBEX SetPath operation options. See OSPF_ defines from obex.h */
    ObexSetPathFlags    flags;

    /* The name of the requested folder to change to, null-terminated. 
     * The format (ASCII/UNICODE) is determined by the 
     * GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType         name[GOEP_MAX_UNICODE_LEN];
    U16                 nameLen;
} GoepSetFolderInd;

/*---------------------------------------------------------------------------
 * GoepSetFolderInd structure
 * 
 *     This structure is used by the Server during SetFolder indications.
 */
typedef struct _GoepActionInd {
    /* OBEX Action operation options. See OSPF_ defines from obex.h */
    U8                  action_id;
    U16                 permission;

    /* The name of the requested folder to change to, null-terminated. 
     * The format (ASCII/UNICODE) is determined by the 
     * GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType         name[GOEP_MAX_UNICODE_LEN];
    U16                 nameLen;
    GoepUniType         dstname[GOEP_MAX_UNICODE_LEN];
    U16                 dstnameLen;    
} GoepActionInd;

/*---------------------------------------------------------------------------
 * GoepServerEvent structure
 * 
 *     This structure is provided to the Server application each time an
 *     event is indicated. The 'event', 'oper' and 'handler' fields are
 *     always valid. Based on the 'oper' and/or the 'event' field the
 *     server can identify which of the 'info' structures is valid.
 */ 
struct _GoepServerEvent {
    GoepEventType           event;      /* GOEP_EVENT_xxx */
    GoepOperation           oper;       /* GOEP_OPER_xxx */
    GoepServerApp          *handler;    /* App's registration structure */

    ObexRespCode            abortReason;     /* Check during GOEP_EVENT_ABORTED */
    ObexRespCode            reason;     /* Check during GOEP_EVENT_ABORTED */    

    union {
#if GOEP_MAX_WHO_LEN > 0
        GoepConnectInd      connect;    /* Use during GOEP_OPER_CONNECT */
#endif /* GOEP_MAX_WHO_LEN > 0 */
        GoepObjectInd       pushpull;   /* Use during PUSH/PULL/DELETE */
        GoepSetFolderInd    setfolder;  /* Use during SETFOLDER */
        GoepActionInd       action;     /* action operation */
    } info;

    GoepHeaderInd           header;     /* Use during GOEP_EVENT_HEADER_RX */

#if OBEX_AUTHENTICATION == XA_ENABLED
    ObexAuthChallengeInfo   challenge;  /* OBEX Challenge parameters */
    ObexAuthResponseInfo    response;   /* OBEX Auth Response parameters */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
};
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */


#if OBEX_ROLE_CLIENT == XA_ENABLED


/*---------------------------------------------------------------------------
 * GoepConnectReq structure
 *
 *     This structure is used by the Client when sending an OBEX Connect.
 *     All fields are optional.
 */
typedef struct _GoepConnectReq {

    U16                 mru;
    /* This field specifies the Target server application. */
    U8            *target;

    /* This field specifies the local client application. */
    const U8            *who;

    /* The length of the string pointed to by the 'target' field. */
    U8                   targetLen;

    /* The length of the string pointed to by the 'who' field. */
    U16                  whoLen;

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* If enabled, these two fields are used to challenge the server and
     * to respond to a received challenge request.
     */
    ObexAuthChallenge   *challenge;
    ObexAuthResponse    *response;

    U8          *password;
    U8           passwordLen;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
} GoepConnectReq;


/*---------------------------------------------------------------------------
 * GoepClientApp structure
 *
 *     This structure is used by the Client Application (or Profile) to 
 *     register with the GOEP Client multiplexer.
 *
 *     If registering directly, using the GOEP_RegisterClient() function all
 *     fields must be filled in prior to registration.
 *
 *     If registering via a profile manager, such as GOEP_RegisterFtpClient()
 *     only the 'callback' field should be filled in.
 */
typedef struct _GoepClientApp {
    /* Group: These fields must be filled in prior to registration */
    GoepClientCallback      callback;

    /* Group: These fields must be filled in only if using GOEP_RegisterClient() */
    GoepProfile             type;       /* GOEP_PROFILE_xxx define */

    GoepConnTypes           connFlags;   /* Type of connection - new or multiplexed */
    U8                      connId;      /* Id of the Obex connection - existing or new */
    GoepConnectReq          connect_req;

    /* === Internal use only === */
#if BT_SECURITY == XA_ENABLED
    BtSecurityRecord        secRecord;
    BtSecurityToken         secToken;
#endif /* BT_SECURITY == XA_ENABLED */
    U32                     obexConnId;
    U8                      connState;
    U32                     cm_conn_id;      /* cm connection id that is used for CM */
    BOOL                    obex_conn_is_connected;      /* Flag to record if we received connect response from peer */

    BOOL                  goep_auth_resp_sent;
#if GOEP_AUTH == XA_ENABLED
    BOOL                  goep_auth_chal_server;
    BOOL                  goep_auth_verify_fail;
#endif	

#if BT_GOEP_V2 == XA_ENABLED
    U8 target_type; /* ObexLvType 0x00 Rfcomm */
    U8 *pl2capRawData[GOEPL2_MAX_RETRANS_NUM];
    U8 l2capRawDataNum; /* allow goep profile to change this */
#endif

    BOOL                  abortFlag;
    U8                    abortReason;
    void                   *object;
    U32                    lastprogress;  /* Avoid too many transmission progress update message*/
    U8                     abortTimerStatus;
    EvmTimer               abortTimer;
} GoepClientApp;


#if GOEP_MAX_WHO_LEN > 0
/*---------------------------------------------------------------------------
 * GoepConnectCnf structure
 *
 *     This structure is used by the GOEP Client during OBEX Connect event
 *     confirmations.
 */
typedef struct _GoepConnectCnf {
    /* If the server responded with a WHO header in the OBEX Connect response,
     * this field contains that information. 'whoLen' specifies its length.
     */
    U8                  who[GOEP_MAX_WHO_LEN];
    U16                 whoLen;
} GoepConnectCnf;
#endif /* GOEP_MAX_WHO_LEN > 0 */

/*---------------------------------------------------------------------------
 * GoepObjectReq structure
 *
 *     Used by the Client to provide information to GOEP during a Push,
 *     Pull or Delete request.
 */
typedef struct _GoepObjectReq {
    /* The Name of the object, must be null-terminated. The proper format
     * (ASCII/UNICODE) is based on the GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType   *name;

    /* The Type of the object, null-terminated, in ASCII format.
     */
    U8            *type;

    /* The handle of the object. */
    void                *object;

    /* The transmitting buffer pointer */
    void                *buf_ptr;

    /* The transmitting buffer size */
    U32                 buf_len;

    /* In a PUSH request, the length of the object should be provided.
     * In PULL and DELETE requests this field is not used.
     */
    U32                  objectLen;

    /* Use SRM header */
    U8                  srm_flags;
} GoepObjectReq;

/*---------------------------------------------------------------------------
 * GoepSetFolderReq structure
 * 
 *     This structure is used by the Client when issuing a Set Folder
 *     request.
 */
typedef struct _GoepSetFolderReq {
    /* Name of the folder, null-terminated. The format (ASCII/UNICODE)
     * is determined by the GOEP_DOES_UNICODE_CONVERSIONS option.
     */
    GoepUniType  *name;

    /* OBEX SetPath operation options. See OSPF_ defines from obex.h */
    ObexSetPathFlags    flags;

    /* Set this flag to true to reset the path to the root folder. 
     * No path flags may be specified if this is TRUE.  In addition,
     * any name provided will be ignored if this is TRUE, since reset 
     * uses an empty name header.
     */
    BOOL                reset;

} GoepSetFolderReq, GoepFolderReq;

/**
* GOEPv2 spec. page 37
* 
*/
typedef struct _GoepActionReq {
    /* Action Identifier*/ 
    U8             id; 
    GoepUniType   *name;
    GoepUniType   *dstname;
    U32            permission;
    U8             flags;
} GoepActionReq;
// TODO: day

/*---------------------------------------------------------------------------
 * GoepPullCnf structure
 *
 *     This structure is indicated to the Client during PULL operations to
 *     provide information that may have been sent back to the client as 
 *     part of a pull operation.
 */
typedef struct _GoepPullCnf {
    /* This field indicates the length of the object being pulled. If zero,
     * then the server did not provide a value, or the object has no content.
     */
    U32             objectLen;
} GoepPullCnf;

/*---------------------------------------------------------------------------
 * GoepClientEvent structure
 * 
 *     This structure is provided to the Client application each time an
 *     event is indicated. The 'event', 'oper' and 'handler' fields are
 *     always valid. Based on the 'oper' and/or the 'event' field the
 *     client can identify which of the 'info' structures is valid.
 */
struct _GoepClientEvent {
    GoepEventType           event;      /* GOEP_EVENT_xxx */
    GoepOperation           oper;       /* GOEP_OPER_xxx */
    GoepClientApp          *handler;    /* App's registration structure */

    ObexRespCode            reason;     /* Check during GOEP_EVENT_ABORTED */

    ObexRespCode            failed_rsp_code;    /* for GET, PUT, SET-FOLDER */

    union {
#if GOEP_MAX_WHO_LEN > 0
        GoepConnectCnf      connect;    /* Use during GOEP_OPER_CONNECT */
#endif /* GOEP_MAX_WHO_LEN > 0 */
        GoepPullCnf         pull;       /* Use during GOEP_OPER_PULL */
    } info;

    GoepHeaderInd           header;     /* Use during GOEP_EVENT_HEADER_RX */

#if OBEX_AUTHENTICATION == XA_ENABLED
    ObexAuthChallengeInfo   challenge;  /* OBEX Challenge parameters */
    ObexAuthResponseInfo    response;   /* OBEX Auth Response parameters */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */
};
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * GOEP_Init()
 *
 *      Initialize the GOEP component.  This must be the first GOEP 
 *      function called by the application layer, or if multiple 
 *      GOEP applications exist, this function should be called
 *      at system startup (see XA_LOAD_LIST in config.h).  OBEX must 
 *      also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
BOOL GOEP_Init(void);

#if OBEX_ROLE_SERVER == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterServer()
 *
 *     This function is used by an application or profile to register a
 *     service with the GOEP Server multiplexor.
 *     
 * Parameters:
 *     Service - A completed GoepServerApp structure describing the
 *          service must be provided. If registration is successful,
 *          this structure becomes property of the GOEP layer until
 *          GOEP_DeregisterServer() is called.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Server is initialized and the target
 *          registered (if provided).
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Server or registering
 *          the target (if provided).
 */
ObStatus GOEP_RegisterServer(GoepServerApp *Service, const ObStoreFuncTable *obStoreFuncs);

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterServerSecurityRecord()
 *
 *     Registers a security record for the GOEP service.  This is
 *     level 2 security.  An application must have a pairing handler 
 *     registered in order to receive PIN requests.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 *     Level - Sets the security level (See BtSecurityLevel).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully registered.
 *
 *     BT_STATUS_FAILED - The security record could not be registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_RegisterServerSecurityRecord(GoepServerApp *Service, BtSecurityLevel Level);

/*---------------------------------------------------------------------------
 * GOEP_UnregisterServerSecurityRecord()
 *
 *     Unregisters a security record for the GOEP service.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully unregistered.
 *
 *     BT_STATUS_FAILED - The security record was not registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_UnregisterServerSecurityRecord(GoepServerApp *Service);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_GetObexServer()
 *
 *     Retrieves the OBEX Server pertaining to a specific GOEP Server.  This 
 *     function is valid after GOEP_Init and GOEP_RegisterServer have been called.
 *     
 * Parameters:
 *     Service - A registered GoepServerApp structure. 
 *
 * Returns: 
 *     ObexServerApp pointer
 *
 */
ObexServerApp* GOEP_GetObexServer(GoepServerApp *Service);

/*---------------------------------------------------------------------------
 * GOEP_SetPullObjectLength()
 *
 *     To set the length of the sending object for pull indication
 *     
 * Parameters:
 *     Service - A registered GoepServerApp structure. 
 *
 * Returns: 
 *     none
 *
 */
void GOEP_SetPullObjectLength(GoepServerApp *Service, U32 obj_len);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_DeregisterServer()
 *
 *     This function is used to deregister a previously registered server.
 *     
 * Parameters:
 *     Service - The application structure used to register the server.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The server was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The server is not registered.
 *
 *     OB_STATUS_BUSY - The server could not be deregistered because
 *          it is currently processing an operation.
 *
 *     OB_STATUS_FAILED - The OBEX server failed to deinitialize.
 */
ObStatus GOEP_DeregisterServer(GoepServerApp *Service);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_AssignServerConnId()
 *
 *     Assign the connection id from the source GOEP server to the 
 *     destination GOEP server.  This function is used when setting up
 *     a new GOEP server which will multiplex off of an existing server's 
 *     connId.
 *
 * Parameters: 
 *      source - Original GOEP server
 *
 *      dest - New GOEP server that inherits the connection ID from
 *          "source".
 */
void GOEP_AssignServerConnId(GoepServerApp *source, GoepServerApp *dest);

#define GOEP_AssignServerConnId(_SOURCE, _DEST) \
            (_DEST)->connId = (_SOURCE)->connId;

/*---------------------------------------------------------------------------
 * GOEP_ServerAbort()
 *
 *     Aborts the current server operation.
 *     
 * Parameters:
 *     Service - The server's registration handle. If zero, the GOEP layer
 *         automatically selects the active server.
 *
 *     Resp - The response code to send to the client expressing the reason
 *         for the abort.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The abort was successfully requested.
 *
 *     OB_STATUS_FAILED - The abort operation failed.
 *            
 *     OB_STATUS_NO_CONNECT - No transport connection exists.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameters.
 */
ObStatus GOEP_ServerAbort(GoepServerApp *Service, ObexRespCode Resp);

/*---------------------------------------------------------------------------
 * GOEP_ServerAccept()
 *
 *     This function is called to accept a Push or Pull request.
 *     It MUST be called during the GOEP_EVENT_PROVIDE_OBJECT indication for
 *     these operations. Failure to do so will abort the operation.
 *     
 * Parameters:
 *     Service - The active server's registration handle.
 *
 *     Obsh - The handle of an Object store object handle. This handle
 *          is not interpreted by the GOEP server (but it must not be zero).
 *          It is passed to the registered object store functions.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server does not have an operation in progress.
 *
 *     OB_STATUS_SUCCESS - The object was accepted.
 *     
 */
ObStatus GOEP_ServerAccept(GoepServerApp *Service, void *Obsh);

#if OBEX_BODYLESS_GET == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerAcceptNoObject()
 *
 *     Indicates the server should accept the GET without supplying an
 *     object (bodyless GET).
 *     
 * Requires:
 *     OBEX_BODYLESS_GET is set to XA_ENABLED
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server does not have an operation in progress.
 *
 *     OB_STATUS_SUCCESS - The object was accepted.
 *     
 */
ObStatus GOEP_ServerAcceptNoObject(GoepServerApp *Service);
#endif /* OBEX_BODYLESS_GET == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_ServerContinue()
 *
 *     This function is called by the server in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus GOEP_ServerContinue(GoepServerApp *Service);

BOOL GOEP_ServerInternalPending(GoepServerApp *Service);


#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerAuthenticate()
 *
 *     This function is used to either respond to an authentication challenge
 *     received from a client, or to send a challenge to the client. The
 *     server never sends both headers at the same time. If the client's
 *     request is challenged, it is automatically rejected with the reason
 *     OBRC_UNAUTHORIZED and the oper will complete with GOEP_EVENT_ABORTED.
 *
 * Parameters:
 *     Service - The server processing the Authentication.
 *
 *     Response - The authentication response to a received challenge.
 *
 *     Challenge - The authentication challenge.
 *
 * Returns:
 *     OB_STATUS_FAILED - Server is not the active server or the GOEP
 *          encountered an error building the authentication header.
 *
 *     OB_STATUS_INVALID_PARM - The supplied authentication information
 *          conflicts with what is expected or acceptable.
 *
 *     OB_STATUS_SUCCESS - The authentication information was accepted.
 */
ObStatus GOEP_ServerAuthenticate(GoepServerApp *Service, 
                                 ObexAuthResponse *Response, 
                                 ObexAuthChallenge *Challenge);

/*---------------------------------------------------------------------------
 * GOEP_ServerVerifyAuthResponse()
 *
 *     This function is used by the Server to verify an authentication
 *     response received from the client. The server should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Service - The active server's registration handle.
 *
 *     Password - The password that the client was expected to provide. This
 *         value is used in the MD5 verification digest calculation.
 *
 *     PasswordLen - The length, in bytes of the password.
 *
 * Returns:
 *     TRUE - Verification succeeded, the digest provided by the client
 *           matched the one generated locally by the server.
 *
 *     FALSE - Verification failed, the client is not authenticated.
 */
BOOL GOEP_ServerVerifyAuthResponse(GoepServerApp *Service, U8 *Password, U8 PasswordLen);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerGetTpConnInfo()
 *
 *     Retrieves OBEX transport layer connection information.  This 
 *     function can be called when a transport connection is active to 
 *     retrieve connection specific information.   
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *     
 *     tpInfo - Pointer to the structure which receives the connection
 *         information. The "size" parameter must be initialized to the
 *         sizeof() the structure.
 *
 * Returns:    
 *     TRUE - The tpInfo structure was successfully completed.
 *     FALSE - The transport is not connected (XA_ERROR_CHECK only).
 */
BOOL GOEP_ServerGetTpConnInfo(GoepServerApp *Service, 
                                   ObexTpConnInfo *tpInfo);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 * GOEP_ServerQueueHeader()
 *
 *     This function queues Byte Sequence, UNICODE, 1-byte, and 4-byte 
 *     OBEX headers for transmission by the GOEP Server.  Queued headers
 *     are transmitted when an operation is responded to with 
 *     GOEP_ServerContinue.
 *
 *     Valid Headers: 
 *          Count, Time, Description, Http, App Params,
 *          Creator ID, Object Class, Body and End Of Body.  
 *
 *     Body headers are typically formed automatically from the Object Store.  
 *     Only manually create Body headers for packets with out of the ordinary 
 *     Body header placement (e.g BPP placing a Body header in a GET request).
 *     Body headers sent in an OBEX response packet cannot span multiple OBEX
 *     packets.
 *
 *     As mentioned before, the OBEX headers are not actually built during the 
 *     processing of this function.  Therefore, it is possible that the building 
 *     of queued headers may fail when the actual OBEX packet is formed. If a
 *     header cannot be built, a failure will be returned from the function 
 *     that added the header, and the queued header that caused the error will 
 *     be purged.  At this point, a new header can be added in its place, or 
 *     the operation can be continued without the failed header.  Any additional 
 *     headers that are needed but cannot fit in the current OBEX packet should 
 *     be sent in the next packet sent by GOEP_ServerContinue during the 
 *     GOEP_EVENT_CONTINUE event.
 *
 * Parameters:
 *     Service - The active server's registration handle.
 *
 *     Type - OBEX header type to build.
 *
 *     Value - OBEX header data (including null-termination for unicode headers).
 *
 *     Len - Length of the OBEX header (including null-termination for unicode headers)
 *
 * Returns:    
 *     TRUE - OBEX Header was built successfully.
 *     FALSE - OBEX Header was not built successfully.
 */
BOOL GOEP_ServerQueueHeader(GoepServerApp *Service, ObexHeaderType Type, 
                            const U8 *Value, U16 Len);
/*---------------------------------------------------------------------------
 * GOEP_ClearServerQueuedHeaders()
 *
 *    This function clear the queued headers of the GOEP Server.
 *
 * Parameters:
 *     Server - The active server's registration handle.
 *
 * Return:    True if the queued OBEX Headers was cleard successfully.
 */
BOOL GOEP_ClearServerQueuedHeaders(GoepServerApp *Server);

#endif /* GOEP_ADDITIONAL_HEADERS > 0 */

#if OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerTpConnect()
 *
 *     Initiates an OBEX Transport Connection to the specified client.  
 *     Completion or failure of the operation is reported by a call to the 
 *     application callback with an appropriate event. The events generated 
 *     as a result of this function returning pending are either 
 *     GOEP_EVENT_TP_CONNECTED, GOEP_EVENT_TP_DISCONNECTED, 
 *     GOEP_EVENT_DISCOVERY_FAILED or GOEP_EVENT_NO_SERVICE_FOUND.
 *     
 * Parameters:
 *     Server - The registered server requesting the connection.
 *
 *     Target - This structure describes the client to which the server
 *         wishes to connect.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_SUCCESS - The server is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the server
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The server is currently executing an operation.
 *
 */
ObStatus GOEP_ServerTpConnect(GoepServerApp *Server, ObexTpAddr *Target);
#endif /* OBEX_ALLOW_SERVER_TP_CONNECT == XA_ENABLED */

#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ServerTpDisconnect()
 *
 *     Initiates the disconnection of the servers transport connection.
 *     
 * Parameters:
 *     Server - The registered server requesting the disconnect.
 *     
 * Returns:
 *     OB_STATUS_BUSY - The server is currently executing an operation.
 *
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the application
 *         callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation failed because there is
 *         no server connection.
 *
 *     OB_STATUS_FAILED - Operation failed.
 */
ObStatus GOEP_ServerTpDisconnect(GoepServerApp *Server);
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */
#endif /* OBEX_ROLE_SERVER == XA_ENABLED */

#if OBEX_ROLE_CLIENT == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterClient()
 *
 *     This function is used by an application or profile to register a
 *     client with the GOEP Client multiplexor.
 *     
 * Parameters:
 *     Client - A completed GoepClientApp structure describing the
 *          client must be provided. If registration is successful,
 *          this structure becomes property of the GOEP layer until
 *          GOEP_DeregisterClient() is called.
 *
 *     obStoreFuncs - Pointer to the Object Store Function Table provided 
 *          by the application layer.  These functions are passed down to 
 *          the OBEX protocol layer in the ObexAppHandle structure.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The OBEX Client is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the OBEX Client.
 */
ObStatus GOEP_RegisterClient(GoepClientApp *Client, const ObStoreFuncTable *obStoreFuncs);

#if BT_SECURITY == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_RegisterClientSecurityRecord()
 *
 *     Registers a security record for the GOEP client.  This is
 *     level 2 security.  An application must have a pairing handler 
 *     registered in order to receive PIN requests.
 *
 * Parameters:
 *     Client - The active client's registration handle.
 *
 *     Level - Sets the security level (See BtSecurityLevel).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully registered.
 *
 *     BT_STATUS_FAILED - The security record could not be registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_RegisterClientSecurityRecord(GoepClientApp *Client, BtSecurityLevel Level);

/*---------------------------------------------------------------------------
 * GOEP_UnregisterClientSecurityRecord()
 *
 *     Unregisters a security record for the GOEP client.
 *
 * Parameters:
 *     Client - The active client's registration handle.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The security record was successfully unregistered.
 *
 *     BT_STATUS_FAILED - The security record was not registered.
 *
 *     BT_STATUS_INVALID_PARM - Invalid parameter (XA_ERROR_CHECK only).
 */
BtStatus GOEP_UnregisterClientSecurityRecord(GoepClientApp *Client);
#endif /* BT_SECURITY == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_GetObexClient()
 *
 *     Retrieves the OBEX Client pertaining to a specific GOEP Client.  This
 *     function is valid after GOEP_Init and GOEP_RegisterClient have been called.
 *     
 * Parameters:
 *     Client - A registered GoepClientApp structure. 
 *
 * Returns: 
 *     ObexClientApp pointer
 *
 */
ObexClientApp* GOEP_GetObexClient(GoepClientApp *Client);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_DeregisterClient()
 *
 *     This function is used to deregister a previously registered client.
 *     
 * Parameters:
 *     Client - The GoepClientApp structure used to register the client.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The client was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The client is not registered.
 *
 *     OB_STATUS_BUSY - The client could not be deregistered because
 *          it is currently sending an operation.
 *
 *     OB_STATUS_FAILED - The OBEX client failed to deinitialize.
 */
ObStatus GOEP_DeregisterClient(GoepClientApp *Client);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_AssignClientConnId()
 *
 *     Assign the connection id from the source GOEP client to the 
 *     destination GOEP client. This function is used when setting up
 *     a new GOEP client which will multiplex off of an existing client's 
 *     connId.
 *
 * Parameters:
 *     source - Original GOEP client
 *
 *     dest - New GOEP client that inherits the connection ID from
 *         "source".
 *
 */
void GOEP_AssignClientConnId(GoepClientApp *source, GoepClientApp *dest);

#define GOEP_AssignClientConnId(_SOURCE, _DEST) \
            (_DEST)->connId = (_SOURCE)->connId;

/*---------------------------------------------------------------------------
 * GOEP_Connect()
 *
 *     This function is used by the client to issue an OBEX Connect Request.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 *     Connect - Optional connect request parameters. This parameter may
 *         be zero if no optional headers are necessary.
 *
 * Returns:
 *     OB_STATUS_BUSY - Operation was not started because the client is
 *         currently executing another operation.
 *
 *     OB_STATUS_INVALID_PARM - The client provided information in the
 *          Connect structure which conflicts with what the GOEP client
 *          was expecting.
 *
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 */
ObStatus GOEP_Connect(GoepClientApp *Client, GoepConnectReq *Connect);

/*---------------------------------------------------------------------------
 * GOEP_Disconnect()
 *
 *     This function is used by the client to issue an OBEX Disconnect Request.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 * Returns:
 *     
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the
 *         application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 */
ObStatus GOEP_Disconnect(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * GOEP_Push()
 *
 *     Initiates the OBEX "Put" operation to send an object to the remote
 *     server. To perform a Delete operation, provide an object store 
 *     handle of zero and make sure OBEX_PUT_DELETE support is enabled.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 *     Object - Parameters necessary to build a push request. This
 *         parameter must be provided.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_FAILED - Operation was not started because
 *         of an underlying transport problem.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 *     
 */
ObStatus GOEP_Push(GoepClientApp *Client, GoepObjectReq *Object);

/*---------------------------------------------------------------------------
 * GOEP_Pull()
 *
 *     Initiates the OBEX "Get" operation to retrieve an object from
 *     the remote server.  
 *
 *     If this operation fails due to a failure in adding queued headers
 *     (GOEP_ClientQueueHeader), it may be useful to set the "More" parameter 
 *     when reissuing this command.  Doing so will allow the GET operation to
 *     send all the headers that will fit in the existing GET request, but will 
 *     also force the GET request to not send the final bit. This allows
 *     subsequent GET request packets to send additional headers prior to a 
 *     GET response sending back the object being described in the GET request 
 *     headers.  Any additional headers that cannot fit in the initial GET 
 *     request will be sent when GOEP_ClientContinue is called during the 
 *     GOEP_EVENT_CONTINUE.  GOEP_ClientQueueHeader should be called before or 
 *     during this event to ensure they are sent in the next GET request packet.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 *     Object - Parameters necessary to build a pull request. This
 *         parameter must be provided.
 *
 *     More - More headers exist in addition to the queued headers already
 *         added via GOEP_ClientQueueHeader.  
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *     
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *     
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 *
 *     OB_STATUS_INVALID_HANDLE - The object store handle was invalid.
 */
ObStatus GOEP_Pull(GoepClientApp *Client, GoepObjectReq *Object, BOOL More);

ObStatus GOEP_PerformAction(GoepClientApp *Client, GoepActionReq *actionReq);

/*---------------------------------------------------------------------------
 * GOEP_SetFolder()
 *
 *     Performs the OBEX "SetPath" operation.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *
 *     Folder - This structure provides the parameters for the Set Folder
 *         request. It is required.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_BUSY - Operation was not started because
 *         the client is currently executing another operation.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_PACKET_TOO_SMALL - Operation was not started
 *         because the OBEX packet size was insufficient to transmit
 *         the provided headers.
 */
ObStatus GOEP_SetFolder(GoepClientApp *Client, GoepSetFolderReq *Folder);

/*---------------------------------------------------------------------------
 * GOEP_ClientAbort()
 *
 *     Aborts the current client operation. The completion event will signal
 *     the status of the operation in progress, either COMPLETE or ABORTED.
 *     
 * Parameters:
 *     Client - The registered client requesting to send the request.
 *         If zero, the GOEP layer automatically selects the active client.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - Operation was successful.
 *
 *     OB_STATUS_FAILED - Operation failed to start.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the client is not connected.
 *
 *     OB_STATUS_INVALID_PARM - Invalid parameter.
 */
ObStatus GOEP_ClientAbort(GoepClientApp *Client);

BOOL GOEP_ClientInternalPending(GoepClientApp *Client);

/*---------------------------------------------------------------------------
 * GOEP_ClientContinue()
 *
 *     This function is called by the client in response to a received
 *     GOEP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Client - The registered client who received the CONTINUE event.
 *
 * Returns:
 *     OB_STATUS_FAILED - The client is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
ObStatus GOEP_ClientContinue(GoepClientApp *Client);

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * GOEP_ClientVerifyAuthResponse()
 *
 *     This function is used by the Client to verify an authentication
 *     response received from the server. The client should have collected
 *     the response information, during an GOEP_EVENT_AUTH_RESPONSE event.
 *     
 * Parameters:
 *     Client - The registered client requesting the connection.
 *
 *     Password - The password that the server was expected to provide. This
 *         value is used in the MD5 verification digest calculation.
 *
 *     PasswordLen - The length, in bytes of the password.
 *
 * Returns:
 *     TRUE - Verification succeeded, the digest provided by the server
 *           matched the one generated locally by the client.
 *
 *     FALSE - Verification failed, the server is not authenticated.
 */
BOOL GOEP_ClientVerifyAuthResponse(GoepClientApp *Client, U8 *Password, U8 PasswordLen);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED
/*---------------------------------------------------------------------------
 *            GOEP_ClientGetTpConnInfo
 *---------------------------------------------------------------------------
 *
 *     Retrieves OBEX transport layer connection information.  This 
 *     function can be called when a transport connection is active to 
 *     retrieve connection specific information.   
 *
 * Parameters:
 *     Client - The active client's registration handle.
 *     
 *     tpInfo - Pointer to the structure which receives the connection
 *         information. The "size" parameter must be initialized to the
 *         sizeof() the structure.
 *
 * Returns:    
 *     TRUE - The tpInfo structure was successfully completed.
 *     FALSE - The transport is not connected (XA_ERROR_CHECK only).
 *
 */
BOOL GOEP_ClientGetTpConnInfo(GoepClientApp *Client, 
                                   ObexTpConnInfo *tpInfo);
#endif /* OBEX_TRANSPORT_FLOW_CTRL == XA_ENABLED */ 

/*---------------------------------------------------------------------------
 * GOEP_TpConnect()
 *
 *     Initiates an OBEX Transport Connection to the specified server.  
 *     Completion or failure of the operation is reported by a call to the 
 *     application callback with an appropriate event. The events generated 
 *     as a result of this function returning pending are either 
 *     GOEP_EVENT_TP_CONNECTED, GOEP_EVENT_TP_DISCONNECTED, 
 *     GOEP_EVENT_DISCOVERY_FAILED or GOEP_EVENT_NO_SERVICE_FOUND.
 *     
 * Parameters:
 *     Client - The registered client requesting the connection.
 *
 *     Target - This structure describes the server to which the client
 *         wishes to connect.
 *
 * Returns:
 *     OB_STATUS_PENDING - Connection was successfully started. Completion
 *         will be signaled via an event to the application callback.
 *
 *     OB_STATUS_SUCCESS - The client is now connected.
 *
 *     OB_STATUS_FAILED - Unable to start the operation because the client
 *         is in the middle of starting up a connection.
 *     
 *     OB_STATUS_BUSY - The client is currently executing an operation.
 *
 */
ObStatus GOEP_TpConnect(GoepClientApp *Client, ObexTpAddr *Target);

/*---------------------------------------------------------------------------
 * GOEP_TpDisconnect()
 *
 *     Initiates the disconnection of the clients transport connection.
 *     
 * Parameters:
 *     Client - The registered client requesting the disconnect.
 *     
 * Returns:
 *     OB_STATUS_BUSY - The client is currently executing an operation.
 *
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled via an event to the application
 *         callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation failed because there is
 *         no client connection.
 *
 *     OB_STATUS_FAILED - Operation failed.
 */
ObStatus GOEP_TpDisconnect(GoepClientApp *Client);

#if GOEP_ADDITIONAL_HEADERS > 0
/*---------------------------------------------------------------------------
 * GOEP_ClientQueueHeader()
 *
 *     This function queues Byte Sequence, UNICODE, 1-byte, and 4-byte 
 *     OBEX headers for transmission by the GOEP Client.  Queued headers
 *     are transmitted when the next GOEP operation is initiated.  
 *     All operations except GOEP_ClientAbort will queue headers.
 *
 *     Valid Headers: 
 *          Count, Time, Description, Http, App Params,
 *          Creator ID, Object Class, Body and End Of Body.  
 *
 *     Body headers are typically formed automatically from the Object Store.  
 *     However, certain applications (such as the Basic Printing Profile) may
 *     require Body headers during a GET request.  Body headers during a GET
 *     request is the only situation where a header can span multiple OBEX 
 *     packets.  For this case only, GOEP will segment the Body header into 
 *     valid OBEX packet sizes.  
 *
 *     ALL other headers must combine to fit within a single OBEX packet.
 *
 *     As mentioned before, the OBEX headers are not actually built during the 
 *     processing of this function.  Therefore, it is possible that the building 
 *     of queued headers may fail when the actual OBEX packet is formed. If a
 *     header cannot be built, a failure will be returned from the function 
 *     that added the header, and the queued header that caused the error will 
 *     be purged.  At this point, a new header can be added in its place, or 
 *     the operation can be restarted or continued without the failed header.  
 *     Any additional headers that are needed but cannot fit in the current 
 *     OBEX packet should be sent in the next packet sent by GOEP_ClientContinue 
 *     during the GOEP_EVENT_CONTINUE event.
 *
 *     When calling GOEP_Pull again in response to its failure to add a queued 
 *     header, its "More" parameter should be set if additional headers need to 
 *     be added in the next GET request, sent during the GOEP_EVENT_CONTINUE 
 *     callback.  Setting the "More" parameter will continue the GET request 
 *     portion of the GET operation, where all client headers describing the
 *     requested object are sent prior to the object being returned.
 *
 * Parameters:
 *     Client - The active client's registration handle..
 *
 *     Type - OBEX header type to build.
 *
 *     Value - OBEX header data (including null-termination for unicode headers).
 *
 *     Len - Length of the OBEX header (including null-termination for unicode headers).
 *
 * Returns:    
 *     TRUE - OBEX Header was built successfully.
 *     FALSE - OBEX Header was not built successfully.
 */
BOOL GOEP_ClientQueueHeader(GoepClientApp *Client, ObexHeaderType Type, 
                            const U8 *Value, U16 Len);

/*---------------------------------------------------------------------------
 *GOEP_ClearClientQueuedHeaders()
 *
 * This function clear the queued headers of the GOEP Client.
 *
 * Parameters:
 *     Client - The active client's registration handle..
 *
 * Return:    True if the queued OBEX Headers was cleard successfully.
 */
BOOL GOEP_ClearClientQueuedHeaders(GoepClientApp *Client);

#endif /* GOEP_ADDITIONAL_HEADERS > 0 */
#endif /* OBEX_ROLE_CLIENT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * GOEP_GetConnectionId()
 *
 *     This function call facilitates the use of the OBEX client/server
 *     multiplexing capabilities of the GOEP layer, by retrieving a 
 *     connection Id (if one exists) for an OBEX client/server connection.
 *     A connection Id will be returned if an OBEX client/server connection 
 *     exists and if no instances of the requested profile are currently
 *     registered.  This is due to the fact that only one instance of a 
 *     specific profile is allowed over the same OBEX connection.  
 *     
 * Parameters: 
 *     role - GOEP client or GOEP server
 *     profileType - Type of profile
 *     
 * Returns:
 *     Connection Id - 0-254, 255 (No connection Id exists)
 */
U8 GOEP_GetConnectionId(GoepRole role, GoepProfile profileType);

/*---------------------------------------------------------------------------
 *GOEP_ServerResetPushNameInfo()
 *
 * This function clear the pushed file name info of the GOEP Server.
 *
 * Parameters:
 *     Server - The active server's registration handle..
 *
 * Return:    void
 */
void GOEP_ServerResetPushNameInfo(GoepServerApp *Server); 
	
/*---------------------------------------------------------------------------
 *GOEP_ServerAuthorizeResponse()
 *
 * This function accept or reject incoming RFCOMM open.
 *
 * Parameters:
 *     Server - The active server's registration handle..
 *     rsp_code -
 *          GOEP_STATUS_SUCCESS: accept the incoming RFCOMM open indication
 *          GOEP_STATUS_FAILED:  reject the incoming RFCOMM open indication
 *
 * Return:    void
 */
void GOEP_ServerAuthorizeResponse(GoepServerApp *Service, kal_uint8 rsp_code);

#ifdef __BT_PBAP_PROFILE__
ObStatus GOEP_ServerInitSendBuf(GoepServerApp *Service, kal_char* pbuf, kal_uint32 size);
kal_uint32  GOEP_ServerGetRemoteMRU(GoepServerApp *Service);
#endif /* __BT_PBAP_PROFILE__ */

	
extern BOOL OBEXH_BuildEmptyEndBody(ObexAppHandle *AppHndl);
/* Compose a SRM header */
extern BOOL OBEXH_BuildSRM(ObexAppHandle *AppHndl);
/* Compose a SRM Wait header */
extern BOOL OBEXH_BuildSRMWait(ObexAppHandle *AppHndl);
/* Update the SRM Flag (client or server side) */
extern BOOL OBEX_UpdateSRMFlag(ObexAppHandle *AppHndl, U8 Role);
/* Update the SRM Wait Flag (client or server side) */
extern BOOL OBEX_UpdateSRMWaitFlag(ObexAppHandle *AppHndl, U8 Role);
/* ClearUP the SRM Flag */
extern BOOL OBEX_ClearSRMFlag(ObexAppHandle *AppHndl);
/* ClearUP the SRM Flag */
extern BOOL OBEX_ClearSRMWaitFlag(ObexAppHandle *AppHndl);

/* if both client and server aggrees to enter SRM mode, the return value is true */
extern BOOL OBEX_IsSRMMode(ObexAppHandle *AppHndl);
/* if client or server setups the waiting flag, the return value is true */
extern BOOL OBEX_IsSRMWait(ObexAppHandle *AppHndl);


#ifdef __BT_OBEX_TUNING__
#define SRM_Assert 
#define GOEP_FLOW
#else
#define SRM_Assert Assert
#define GOEP_FLOW OS_Report
#endif

BOOL GOEP_ClientDisableWait(GoepClientApp *Client);
BOOL GOEP_ServerDisableWait(GoepServerApp *Service);

U8   GOEP_ClientGetRemoteVersion(GoepClientApp *Client);
U8   GOEP_ServerGetRemoteVersion(GoepServerApp *Service);

U8   GOEP_ClientGetTpType(GoepClientApp *Client);
U8   GOEP_ServerGetTpType(GoepServerApp *Service);

BOOL GOEP_ClientSRMMode(GoepClientApp *Client);
BOOL GOEP_ServerSRMMode(GoepServerApp *Server);

#endif /* __GOEP_H */
