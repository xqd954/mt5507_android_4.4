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

/****************************************************************************
 *
 * File:
 *     $Workfile:pbap.h$ for iAnywhere Blue SDK, PBAP Version 1.1.2
 *     $Revision: #1 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the Bluetooth
 *     Phonebook Access Profile program interface.
 * 
 * $Project:XTNDAccess Blue SDK$
 *
 * Copyright 2005-2006 Extended Systems, Inc.
 * Portions copyright 2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************
*============================================================================
*             HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Log: $
*
* 
*
* 
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*============================================================================
****************************************************************************/
#ifndef __PBAP_H
#define __PBAP_H
 
#ifdef __BT_PBAP_PROFILE__

#include <assert.h>
#include "config.h"
#include <obex.h>
#include <obexauth.h>
#include "goep.h"
#include "pbapobs.h"
#include "bluetooth_pbap_struct.h"


//#define ASSERT assert

/*---------------------------------------------------------------------------
 * Phonebook Access Profile (PBAP) Layer 
 *
 *     The PBAP layer provides functions for building and sending Phonebook 
 *     Access compliant messages.  These messages are based on the OBEX 
 *     protocol and are written directly on top of our GOEP profile layer.
 */

/****************************************************************************
 *
 * Section: Default general constants that can be overridden in overide.h
 *
 ***************************************************************************/


/*---------------------------------------------------------------------------
 * PBAP_NUM_SERVERS Constant
 *
 *  Defines the number of concurrent servers supported.
 *
 *  The default value is 1. Set to 0 to disable server support.
 */
#ifndef PBAP_NUM_SERVERS
#define PBAP_NUM_SERVERS            1
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_PASSWORD_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication password.
 *
 *  The default value is 20.
 */
#ifndef PBAP_MAX_PASSWORD_LEN
#define PBAP_MAX_PASSWORD_LEN       20
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_USERID_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication username.
 *
 *  The default value is 20.
 */
#ifndef PBAP_MAX_USERID_LEN
#define PBAP_MAX_USERID_LEN         20
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_REALM_LEN Constant
 *
 *  Defines the maximum length of the OBEX authentication realm.
 *
 *  The default value is 20.
 */
#ifndef PBAP_MAX_REALM_LEN
#define PBAP_MAX_REALM_LEN          20
#endif

/*---------------------------------------------------------------------------
 * PBAP_MAX_APP_PARMS_LEN Constant
 *
 *  Defines the maximum length of the application parameter header information
 *  used within PBAP.
 *
 *  The default value is 30.
 */
#ifndef PBAP_MAX_APP_PARMS_LEN
#define PBAP_MAX_APP_PARMS_LEN      30
#endif


/*---------------------------------------------------------------------------
 * PBAP_LOCAL_PHONEBOOK_SUPPORTED constant
 * 
 *  Indicates whether local phonebook support is available on the PBAP server. 
 *  This value is stored in the SDP entry.
 */
#ifndef PBAP_LOCAL_PHONEBOOK_SUPPORTED
#define PBAP_LOCAL_PHONEBOOK_SUPPORTED      XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * PBAP_SIM_PHONEBOOK_SUPPORTED constant
 * 
 *  Indicates whether SIM card phonebook support is available on the PBAP 
 *  server.  This value is stored in the SDP entry.
 */
#ifndef PBAP_SIM_PHONEBOOK_SUPPORTED
#define PBAP_SIM_PHONEBOOK_SUPPORTED        XA_ENABLED
#endif

#if PBAP_NUM_SERVERS > 0
#if OBEX_ROLE_SERVER == XA_DISABLED
#error "OBEX_ROLE_SERVER must be enabled!"
#endif
#endif

#if GOEP_MAX_UNICODE_LEN == 0
#error "GOEP_MAX_UNICODE_LEN must be greater than zero!"
#endif

/* csj */
/*
 *#if GOEP_DOES_UNICODE_CONVERSIONS == XA_DISABLED
 *#error "GOEP_DOES_UNICODE_CONVERSIONS Must be enabled."
 *#endif
 */
 
/* Provide enough room for the vCard listing type header - 'x-bt/vcard-listing' */
#if GOEP_MAX_TYPE_LEN < 19
#error "GOEP_MAX_TYPE_LEN must be 19 or greater!"
#endif

#if OBEX_SERVER_CONS_SIZE < 1
#error "OBEX_SERVER_CONS_SIZE must be 1 or greater!"
#endif

/* csj */
//#if OBEX_BODYLESS_GET != XA_ENABLED
//#error "OBEX_BODYLESS_GET must be enabled!"
//#endif

#if OBEX_ALLOW_SERVER_TP_DISCONNECT != XA_ENABLED
#error "OBEX_ALLOW_SERVER_TP_DISCONNECT must be enabled!"
#endif

#if SDP_SERVER_SUPPORT != XA_ENABLED
#error "SDP_SERVER_SUPPORT must be enabled!"
#endif


/*----------------------------------------------------------------------------
 * PULL_PHONEBOOK_TYPE constant
 *
 *     The PULL_PHONEBOOK_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting a phonebook
 *     object.
 */
#define PULL_PHONEBOOK_TYPE            "x-bt/phonebook"

 /*---------------------------------------------------------------------------
 * VCARD_LISTING_OBJECT_TYPE constant
 *
 *     The VCARD_LISTING_OBJECT_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting the vCard listing
 *     object.
 */
#define VCARD_LISTING_OBJECT_TYPE      "x-bt/vcard-listing"

/*----------------------------------------------------------------------------
 * VCARD_OBJECT_TYPE constant
 *
 *     The VCARD_OBJECT_TYPE specifies the value that is sent by
 *     the client in an OBEX Type header when requesting a vCard
 *     object.
 */
#define VCARD_OBJECT_TYPE              "x-bt/vcard"

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_UNKNOWN_OBJECT_LENGTH constant
 *
 *     This constant is returned by PBAPOBS_GetObjectLen() when a phonebook
 *     object length is unknown. It is commonly used with objects that are 
 *     generated dynamically during the exchange. In exchanges where this 
 *     value is returned, the protocol calls the PBAPOBS_ReadFlex() read 
 *     function.
 *
 * Requires:
 *     OBEX_DYNAMIC_OBJECT_SUPPORT set to XA_ENABLED.
 */
#define PBAP_UNKNOWN_OBJECT_LENGTH      0xFFFFFFFF
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAP_FILTER_SIZE constant
 *
 *     This constant defines the size (in bytes) of the vCard filter used 
 *     in the Pull Phonebook and Pull Vcard Entry operations.
 */
//#define PBAP_FILTER_SIZE                8

/****************************************************************************
 *
 * Section: Types
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 * PbapEvent type
 *
 *     Indicates the current event type. All event indications and 
 *     confirmations are passed to a callback function of type 
 *     PbapClientCallback or PbapServerCallback. The "PbapCallbackParms.event" 
 *     field indicates one of the event types below.  The 
 *     "PbapCallbackParms.oper" field indicates the applicable event 
 *     operation.
 */
typedef U8 PbapEvent;

/* A transport layer connection has been established. There is no
 * operation associated with this event.
 */
#define PBAP_EVENT_TP_CONNECTED         0x01

/* The transport layer connection has been disconnected. There is no
 * operation associated with this event.
 */
#define PBAP_EVENT_TP_DISCONNECTED      0x02

/* Indicates that a phonebook operation (see PbapOp) has 
 * completed successfully.
 */
#define PBAP_EVENT_COMPLETE             0x03

/* Indicates that the current operation has failed or was rejected
 * by the remote device.
 */
#define PBAP_EVENT_ABORTED              0x04

/* Delivered to the application when it is time to issue
 * another request or response. The application must call either
 * PBAP_ServerContinue() or PBAP_ClientContinue().
 */
#define PBAP_EVENT_CONTINUE             0x05

/* Delivered to the application when the Application Parameters
 * header information has been fully parsed.
 */
#define PBAP_EVENT_PARAMS_RX            0x06

#if PBAP_NUM_SERVERS > 0
/* Delivered to server application before the last response is going
 * to be sent. It provides the server with an opportunity to complete 
 * the requested operation (such as a SetPhonebook request) and if necessary
 * set an abort response code before the final response is sent (Server Only).
 */
#define PBAP_EVENT_PRECOMPLETE          0x07

/* Indicates the start of a new operation. In the case of an Abort
 * operation, this event may occur during another pending operation 
 * (e.g. PullPhonebook).  In the case where PBAP_ServerContinue() 
 * has not been called during this operation, perhaps awaiting Server 
 * object creation or additional buffer space, it should be called now 
 * or the Abort operation will not complete (Server Only).
 */
#define PBAP_EVENT_START                0x08
    
/* Indicates a request for the phonebook parameters needed for the next
 * response. The application should provide these parameters through the 
 * Phonebook Access server callback parameters. This situation occurs only 
 * during Pull Phonebook and Pull VCard Listing operations.  The phonebook
 * size parameter is only needed when a maxListCount of zero is 
 * received from the client device during the PBAP_EVENT_PARAMS_RX event
 * (Server only).
 */
#define PBAP_EVENT_PROVIDE_PARAMS       0x09

/* Indicates a request for the phonebook object data length. The 
 * application should provide the length of the object being 
 * requested through the Phonebook Access server callback parameters
 * Failure to do so will cause the operation to not send an object. 
 * (Server Only)
 */
#define PBAP_EVENT_OBJECT_LENGTH_REQ    0x0a

 #define PBAP_EVENT_PRE_DATA_REQ             0x0b
 
/* Indicates a request for data. The application should provide the
 * data length requested, and provide a valid buffer pointer through the
 * Phonebook Access server callback parameters. Different callback 
 * parameters exist based on whether PBAPOBS_Read or PBAPOBS_ReadFlex 
 * have triggered this event (Server only).
 */
#define PBAP_EVENT_DATA_REQ             0x0c
#endif /* PBAP_NUM_SERVERS > 0 */

#if OBEX_AUTHENTICATION == XA_ENABLED
/* Informs the client/server application of the result from the OBEX
 * Authentication operation performed during the OBEX Connect. This event
 * is indicated only to the initiator of the OBEX Authentication (the device
 * that issued the OBEX Authentication Challenge header), as this device
 * is the one requiring authentication to proceed.  If a failure occurs
 * during PBAP Client authentication, the client will not be able to 
 * proceed with any operations until authentication is retried successfully.
 * If a failure occurs during PBAP Server authentication, the server will 
 * reject the OBEX Connect with the Unauthorized response code and the
 * PBAP Client will be forced to retry the connect operation again, before
 * the server will accept any operations.
 */
#define PBAP_EVENT_AUTH_RESULT          0x0d
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#define PBAP_EVENT_AUTHRIZE_IND         0x0e
/* End of PbapEvent */

/*---------------------------------------------------------------------------
 * PbapOp type
 *
 *     Indicates the operation type of the current event. Each event 
 *     indication has an associated operation passed to a callback function 
 *     of type PbapClientCallback or PbapServerCallback. The 
 *     "PbapCallbackParms.oper" field will indicate one of the operation 
 *     types below.  Since the Pull Phonebook, Pull Vcard Listing, and Pull
 *     Vcard Entry operations cannot be known until the actual OBEX headers
 *     are processed, the initial operation start event will indicate merely
 *     that a generic Pull operation is occurring, until further information
 *     arrives.  The exact operation will be known either during the 
 *     PBAP_EVENT_PARAMS_RX or PBAP_EVENT_OBJECT_LENGTH_REQ events
 */
typedef U32 PbapOp;

#define PBAPOP_NONE                0x0000 /* No current operation */
#define PBAPOP_CONNECT             0x0001 /* Connect operation */
#define PBAPOP_DISCONNECT          0x0002 /* Disconnect operation */
#define PBAPOP_SET_PHONEBOOK       0x0004 /* Set Phonebook operation */
#define PBAPOP_PULL                0x0008 /* Generic Pull operation  */
#define PBAPOP_PULL_PHONEBOOK      0x0010 /* Pull Phonebook operation */
#define PBAPOP_PULL_VCARD_LISTING  0x0020 /* Pull vCard listing operation */
#define PBAPOP_PULL_VCARD_ENTRY    0x0040 /* Pull vCard entry operation */

/* End of PbapOp */

/*---------------------------------------------------------------------------
 * PbapAppParmsTag type
 *
 *     Describes the tag values used in the Application Parameters OBEX header
 *     that are used on both the Phonebook Access client and server side.
 */
typedef U8 PbapAppParmsTag;

#define PBAP_TAG_ORDER              0x01        /* 1-byte, 0x00 (indexed), 0x01 (alpha), or 0x02 (phonetic) */
#define PBAP_TAG_SEARCH_VALUE       0x02        /* Variable length text string */
#define PBAP_TAG_SEARCH_ATTRIB      0x03        /* 1-byte, 0x00 (Name), 0x01 (Number), or 0x02 (Sound) */
#define PBAP_TAG_MAX_LIST_COUNT     0x04        /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_LIST_OFFSET        0x05        /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_FILTER             0x06        /* 8-bytes, 64 bit mask */
#define PBAP_TAG_FORMAT             0x07        /* 1-byte, 0x00 = 2.1, 0x01 = 3.0 */
#define PBAP_TAG_PHONEBOOK_SIZE     0x08        /* 2-bytes, 0x0000 to 0xFFFF */
#define PBAP_TAG_MISSED_CALLS       0x09        /* 1-byte, 0x00 to 0xFF */

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapDataReqType type
 *
 *     Describes the type of data request associated with the 
 *     PBAP_EVENT_DATA_REQ event. This type will dicate which callback 
 *     parameters are valid during a data request as PBAPOBS_Read has
 *     different parameters from PBAPOBS_ReadFlex.
 */
typedef U8 PbapDataReqType;

/* Data Request is using the normal PBAPOBS_Read routine */
#define PBAP_DATAREQ_READ      0x01
/* Data Request is using the flexible PBAPOBS_ReadFlex routine */
#define PBAP_DATAREQ_READ_FLEX 0x02
/* End of PbapDataReqType */
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapAuthFailReason type
 *
 *  Determines the reason for the PBAP Authentication failure associated
 *  with the PBAP_EVENT_AUTH_RESULT event.
 */
typedef U8 PbapAuthFailReason;

#define PBAP_AUTH_NO_FAILURE            0
#define PBAP_AUTH_NO_RCVD_AUTH_RESP     1
#define PBAP_AUTH_INVALID_CREDENTIALS   2
/* End of PbapAuthFailReason */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if PBAP_NUM_SERVERS > 0

/* Forward reference to PbapServerCallbackParms defined below */
typedef struct _PbapServerCallbackParms PbapServerCallbackParms;

/*---------------------------------------------------------------------------
 * PbapServerCallback type
 *
 *  Phonebook Access Server events along with data requests are 
 *  passed to the application through a callback function of this type 
 *  defined by the application.
 */
typedef void (*PbapServerCallback)(PbapServerCallbackParms *parms);

/* End of PbapServerCallback */
#endif /* PBAP_NUM_SERVERS > 0 */

typedef U8 PbapAuthFlags;   /* Used Internally */

/****************************************************************************
 *
 * Section: Data Structures
 *
 ****************************************************************************/


//f PBAP_NUM_SERVERS > 0

/*----------------------------------------------------------------------
 * PbapServerSession structure
 *
 *  Maintains Phonebook Access server info and status during an 
 *  application session.
 */
typedef struct _PbapServerSession 
{
#if BT_SECURITY == XA_ENABLED
    /* Bluetooth security level - set by the application 
     * prior to PBAP_RegisterServer.  An application must have a
     * pairing handler registered in order to receive PIN requests.
     * This can be done through the BlueMgr API or done directly
     * by the application.
     */
    BtSecurityLevel     secLevel;
#endif /* BT_SECURITY == XA_ENABLED */

    /* GOEP Server instance */
    GoepServerApp       sApp;

    /* === Internal use only === */   
    PbapOp              currOp;
    PbapAuthFlags       flags;
    U8                  appParms[PBAP_MAX_APP_PARMS_LEN];
   // SdpRecord           record;
  //  SdpAttribute        attributes[7];
    U8                 suppored_repositories;
    /* PBAP object store handle */
    PbapObStoreHandle   object;
    /* Registration status for this PBAP Server instance */
    kal_bool                registered;
    /* GOEP Connect information */
    ObexConnection      obexConn;       

#if OBEX_AUTHENTICATION == XA_ENABLED
    /* OBEX Authentication password */
    U8                  password[PBAP_MAX_PASSWORD_LEN];
    /* OBEX Authentication userId */
    U8                  userId[PBAP_MAX_USERID_LEN];
    /* OBEX Authentication realm */
    U8                  realm[PBAP_MAX_REALM_LEN];
    ObexAuthChallenge   chal;
    ObexAuthResponse    resp;
    ObexAuthChallengeInfo challengeInfo;
     ObexAuthResponseInfo ResponseInfo;
    U8                  chalStr[25];
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

} PbapServerSession;
//endif /* PBAP_NUM_SERVERS > 0 */

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapAuthInfo structure
 *
 *  Describes the private information exchanged during OBEX authentication.
 *  These parameters must be filled in by the client/server application prior
 *  to calling PBAP_ClientSetAuthInfo or PBAP_ServerSetAuthInfo.
 */
typedef struct _PbapAuthInfo
{
    /* OBEX Authentication Password */
    const U8           *password;
    /* OBEX Authentication Password Length */
    U8                 passwordLen;
    /* OBEX Authentication Username */
    const U8           *userId;
    /* OBEX Authentication Username Length */
    U8                 userIdLen;
    /* OBEX Authentication Realm - describes the type of username used */
    const U8           *realm;
    /* OBEX Authentication Realm length */
    U16                 realmLen;
} PbapAuthInfo;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PbapVcardFilter structure
 * 
 *  Describes the 64-bit filter value sent in the Application Parameters
 *  OBEX header from the Phonebook Access client to the server to dictate
 *  which fields it wishes to be returned for each vCard object.  This
 *  filter is used for both the Pull Phonebook and Pull Phonebook Entry 
 *  operations.
 */
//typedef struct _PbapVcardFilter 
//{
//    /* Array of 8 bytes for this 64-bit filter value */
//    U8                  byte[PBAP_FILTER_SIZE];
//} PbapVcardFilter;


#if PBAP_NUM_SERVERS > 0
/*---------------------------------------------------------------------------
 * PbapPullInd structure
 *
 *     Provides information to a PBAP Server about a received 
 *     PULL indication.
 */
typedef struct _PbapPullInd {
    /* OBEX final bit */
    kal_bool            finalBit;
    
    /* Name of the folder, null-terminated, in ASCII format. */
    const char     *name;

    /* Type of the object, null-terminated, in ASCII format. */
    const char     *type;
} PbapPullInd;

/*---------------------------------------------------------------------------
 * PbapSetPbInd structure
 * 
 *     This structure is used by the Server during SetFolder indications.
 */
typedef struct _PbapSetPbInd {
    /* The client requested a reset to the root folder. */
    BOOL               reset;

    /* Set Phonebook operation options. */
    ObexSetPathFlags      flags;

    /* Name of the folder, null-terminated. */
    const U8         *name;
} PbapSetPbInd;

/*---------------------------------------------------------------------------
 * PbapServerCallbackParms structure
 *
 *  Describes a callback event and any data that relates to the event. These
 *  callback parameters are used for all Phonebook Access Servers.  
 *  Determination of the valid fields can be done by evaluating which event 
 *  is being indicated as well as which server it is intended for.
 */
struct _PbapServerCallbackParms
{
    PbapEvent               event;          /* PBAP event */
    PbapOp                  oper;           /* PBAP operation */
    PbapServerSession      *server;         /* PBAP server */

    union {
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
        /* Group: Valid during PBAP_EVENT_TP_CONNECTED event, contains 
         * the SDP parsed information 
         */
        struct {
            U16                     profileVersion; /* PBAP profile version */
        } connect;
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

        /* Group: Valid during PBAP_EVENT_TP_DISCONNECTED event */
        PbapRespCode        discReason;     /* PBAP disconnect reason code */

#if OBEX_AUTHENTICATION == XA_ENABLED
        /* Group: Valid during PBAP_EVENT_AUTH_RESULT event */
        struct {
            BOOL                    result;         /* Result of the PBAP Authentication attempt */
            PbapAuthFailReason      reason;         /* PBAP Authentication failure reason */
        } auth;
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

        /* Group: Valid during PBAP_EVENT_PROVIDE_PARAMS event - requests 
         * the application to provide Application Parameter header information 
         * for the next response packet. Valid for Pull Phonebook and Pull 
         * Vcard Listing operations only.
         */
        struct {
            /* Number of new missed calls - to be set by the application */
            U16             newMissedCalls;
            /* Size of the requested phonebook. The server should provide 
             * this value if the received client maxListCount was zero 
             */
            U16             phonebookSize;
        } provideParams;

        /* Group: Valid during PBAP_EVENT_PARAMS_RX event - provides Application
         * Parameter header information.
         */
        struct {
            U8                 *name; /* Object name, null terminated ASCII */
            U8                 *type; /* Object type, null terminated ASCII */

            /* Group: Valid during Pull Phonebook and Pull VCard Entry operations */
            PbapVcardFilter     filter;             /* Filter of the requested vCard fields */
            PbapVcardFormat     format;             /* Format of vCard (version 2.1 or version 3.0) */

            /* Group: Valid during Pull Phonebook and Pull VCard Listing operations */
            U16                 maxListCount;       /* Maximum of vCard entries supported */
            U16                 listStartOffset;    /* Offset of the first vCard entry */

            /* Group: Valid during Pull VCard Listing operation */
            U8          order;          /* Sorting method: indexed/alphabetical/phonetical */
            U8    searchAttrib;   /* Search attribute: Name (0x00) Number (0x01), or Sound (0x02) */
            U8                         *searchValue;    /* Text value to search the attribute for */
            U8                          searchValueLen; /* Length of search value */
        } paramsRx;

        /* Group: Valid during PBAP_EVENT_OBJECT_LENGTH_REQ event */
        struct {
            /* Object name (null-terminated, ASCII) */
            U8             *name;
            /* Object Length - to be set by the application */
            U32             objectLen;
        } objectReq;

        /* Group: Valid during PBAP_EVENT_DATA_REQ event */
        struct {
#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            /* Type of data request */
            PbapDataReqType type;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */

            /* Object name (null-terminated, ASCII) */
            U8             *name;

            /* Data buffer pointer - to be assigned by the application */
            U8             *buffer;     

            /* Group: Valid for PBAP_DATAREQ_READ type only */
            struct {
                /* Length of the requested data */
                U16         len;
            } req;

#if OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED
            /* Group: Valid for PBAP_DATAREQ_READ_FLEX only. */
            struct {
                /* Length available - the length used is set by application */
                U16        *len;
                /* Dictates whether more data exists to be sent - This value 
                 * is set by the application.
                 */
                BT_BOOL       *more;
            } flex;
#endif /* OBEX_DYNAMIC_OBJECT_SUPPORT == XA_ENABLED */
        } dataReq;

        /* Group: Valid during PBAP_EVENT_PRECOMPLETE and PBAP_EVENT_COMPLETE events */
        union {
            PbapPullInd     pull;           /* Valid during Pull operations */
            PbapSetPbInd    setPb;          /* Valid during Set Phonebook operation */
        } info;
    } u;
};
/* End of PbapServerCallbackParms */
#endif /* PBAP_NUM_SERVERS > 0 */


/****************************************************************************
 *
 * Internal Data Types
 *
 ***************************************************************************/

#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapAuthFlags type
 *
 *      Internal PBAP authentication flags used to tracking the status of 
 *      any OBEX authentication operations.
 */

#define PBAP_FLAGS_NO_AUTHENTICATION  0x01   /* No OBEX Authentication has occurred */
#define PBAP_FLAGS_AUTHENTICATION_REQ 0x02   /* OBEX Authentication is required */
#define PBAP_FLAGS_AUTHENTICATED      0x04   /* OBEX Authentication complete */
#define PBAP_FLAGS_CHAL_REQ           0x08   /* OBEX Auth Challenge requested */
#define PBAP_FLAGS_CHAL_RCVD          0x10   /* OBEX Auth Challenge received */
#define PBAP_FLAGS_RESP_RCVD          0x20   /* OBEX Auth Response received */

/* End of PbapAuthFlags */
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
/*---------------------------------------------------------------------------
 * PbapSdpFlags type
 *
 *      Internal PBAP authentication flags used to tracking the status of 
 *      any SDP parsing information.
 */
typedef U8 PbapSdpFlags;

#define PBAP_SDP_FLAGS_BT_PROFILE_DESC_LIST   0x01   /* Parsed Bluetooth Profile Descriptor List */
#define PBAP_SDP_FLAGS_SUPPORTED_REPOSITORIES 0x02   /* Parsed Supported Repositories */

/* End of PbapSdpFlags */
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

/****************************************************************************
 *
 * Internal Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PbapContext structure
 *
 *     Phonebook access context structure.
 */
typedef struct _PbapContext {
    BT_BOOL                        initialized;
/* csj */
#if 0
#if PBAP_NUM_CLIENTS > 0
    PbapClientSession          *client[PBAP_NUM_CLIENTS];
    PbapClientCallback          clientCallback;
    PbapClientCallbackParms     clientParms;
#endif /*  PBAP_NUM_CLIENTS > 0 */
#endif /* 0 */

#if PBAP_NUM_SERVERS > 0
    PbapServerSession          *server[PBAP_NUM_SERVERS];
    PbapServerCallback          serverCallback;
    PbapServerCallbackParms     serverParms;
#endif /*  PBAP_NUM_SERVERS > 0 */

#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
    PbapSdpFlags                sdpFlags;
    U16                         profileVersion;     /* Client/Server */
    PbapSupportedPhonebooks     suppPhonebooks;     /* Server only */
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

    /* ObStore Entries and List */
    PbapObStoreEntry            obsEntries[4];//[PBAP_OBS_NUM_ENTRIES];
    ListEntry                   obsList;
} PbapContext;

/*---------------------------------------------------------------------------
 * Context Pointer declarations
 */
#if XA_CONTEXT_PTR == XA_ENABLED
extern PbapContext *Pbap;
#define PBAP(f) (Pbap->f)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern PbapContext Pbap;
#define PBAP(f) (Pbap.f)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */ 

/* Function Prototypes */
#if OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED
//void PbapSdpResultsCallback(SdpQueryToken *token, U8 *customRfChannel);
#endif /* OBEX_PROVIDE_SDP_RESULTS == XA_ENABLED */

/* csj */
#if 0
#if PBAP_NUM_CLIENTS > 0
BtStatus PceAddServiceRecord(PbapClientSession *Client);
BtStatus PceRemoveServiceRecord(PbapClientSession *Client);
void PceBuildServiceQuery(const U8 **QueryPtr, U16 *QueryLen, SdpQueryType *QueryType);
PbapClientSession *FindPbapClient(GoepClientApp *app);
#endif /* PBAP_NUM_CLIENTS > 0 */
#endif /* 0 */

#if PBAP_NUM_SERVERS > 0 
//extern BtStatus PseAddServiceRecord(PbapServerSession *Server);
//extern BtStatus PseRemoveServiceRecord(PbapServerSession *Server);
//extern void PseBuildServiceQuery(const U8 **QueryPtr, U16 *QueryLen, SdpQueryType *QueryType);
extern PbapServerSession *FindPbapServer(GoepServerApp *app);
#endif /* PBAP_NUM_SERVERS > 0 */


/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * PBAP_Init()
 *
 *      Initialize the PBAP component.  This must be the first PBAP 
 *      function called by the application layer, or if multiple 
 *      PBAP applications exist, this function should be called
 *      at system startup (see XA_LOAD_LIST in config.h).  GOEP and 
 *      OBEX must also be initialized separately.
 *
 * Returns:
 *     TRUE - Initialization was successful.
 *
 *     FALSE - Initialization failed.
 */
extern BT_BOOL PBAP_Init(void);

#if PBAP_NUM_SERVERS > 0
/*---------------------------------------------------------------------------
 * PBAP_RegisterServer()
 *
 *     Registers the Phonebook Access Server with the GOEP multiplexor. This 
 *     includes registering the SDP records for the Phonebook Access Service.
 *     All of the events specified in the GOEP layer are delivered to the
 *     PBAP server.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Callback - callback function for the Phonebook Access server.
 *
 * Returns:
 *     OB_STATUS_INVALID_PARM - Invalid or in use profile type.
 *
 *     OB_STATUS_SUCCESS - The PBAP Server is initialized.
 *
 *     OB_STATUS_FAILED - Problem initializing the PBAP Server.
 *     
 *     OB_STATUS_BUSY - A PBAP server is already registered.
 *     
 */

extern ObStatus PBAP_RegisterServer(PbapServerSession *Server, 
                             PbapServerCallback Callback);

#if OBEX_DEINIT_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_DeregisterServer()
 *
 *     Deregisters the Phonebook Access Server from the GOEP multiplexor.
 *     This includes removing the SDP records for the Phonebook Access Service.
 *     
 * Parameters:
 *     Server - The server used in the registration call.
 *
 * Returns:
 *     OB_STATUS_SUCCESS - The server was deregistered.
 *
 *     OB_STATUS_INVALID_PARM - The server is not registered.
 *
 *     OB_STATUS_BUSY - The server could not be deregistered because
 *          it is currently processing an operation.
 *
 *     OB_STATUS_FAILED - The PBAP server failed to deinitialize.
 */
extern ObStatus PBAP_DeregisterServer(PbapServerSession *Server);
#endif /* OBEX_DEINIT_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * PBAP_ServerContinue()
 *
 *     This function is called by the server in response to a received
 *     PBAP_EVENT_CONTINUE event. It must be called once for every
 *     CONTINUE event received. It may or may not be called in the context
 *     of the callback and can be deferred for flow control purposes.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The server is not expecting a continue.
 *
 *     OB_STATUS_SUCCESS - The continue was successful.
 */
extern ObStatus PBAP_ServerContinue(PbapServerSession *Server);

/*---------------------------------------------------------------------------
 * PBAP_ServerAbort()
 *
 *     Aborts the current server operation.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
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
extern ObStatus PBAP_ServerAbort(PbapServerSession *Server, PbapRespCode Resp);
extern ObStatus PBAP_ServerAuthCheck(PbapServerSession *Server, U8 cancel);
#if OBEX_AUTHENTICATION == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_ServerAuthenticate()
 *
 *     This function is called by the server to initiate OBEX authentication
 *     on the upcoming Phonebook Access OBEX connection.  This function must
 *     be called prior to calling PBAP_ServerContinue during the Phonebook 
 *     Access OBEX connection, in order to setup the OBEX Authentication 
 *     Challenge information. In addition, the PBAP_ServerSetAuthInfo routine 
 *     should be called prior to both of these functions to ensure that the 
 *     proper username, password, and challenge realm information are being 
 *     used.
 *     
 *     The authentication information is cleared when an OBEX Disconnect or
 *     transport disconnect occurs, so this should take place each time the
 *     connection is coming up.
 *
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_FAILED - The request to authenticate failed.
 *
 *     OB_STATUS_SUCCESS - The request to authenticate was successful.
 */
extern ObStatus PBAP_ServerAuthenticate(PbapServerSession *Server);

/*---------------------------------------------------------------------------
 * PBAP_ServerSetAuthInfo()
 *
 *     This function is called by the server to set the proper username, 
 *     password, and challenge realm information for the upcoming Phonebook
 *     Access OBEX connection.  This information is used when authenticating 
 *     this OBEX connection.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 *     Info - Structure containing the authentication information.
 *
 * Returns:
 *     OB_STATUS_FAILED - Setting the authentication information failed.
 *
 *     OB_STATUS_SUCCESS - Setting the authentication information succeeded.
 */
extern ObStatus PBAP_ServerSetAuthInfo(PbapServerSession *Server, PbapAuthInfo *Info);
#endif /* OBEX_AUTHENTICATION == XA_ENABLED */

extern void PbapGoepServerCallback(GoepServerEvent *Event);

#if OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED
/*---------------------------------------------------------------------------
 * PBAP_ServerTpDisconnect()
 *
 *     This function is used by the server to issue a transport disconnection.
 *     
 * Parameters:
 *     Server - The server application's registration structure.
 *
 * Returns:
 *     OB_STATUS_PENDING - Operation was successfully started.
 *         Completion will be signaled with the PBAP_EVENT_TP_DISCONNECTED 
 *         event to the application callback.
 *
 *     OB_STATUS_NO_CONNECT - Operation was not started because
 *         the server is not connected.
 */
extern ObStatus PBAP_ServerTpDisconnect(PbapServerSession *Server);
#endif /* OBEX_ALLOW_SERVER_TP_DISCONNECT == XA_ENABLED */
#endif /* PBAP_NUM_SERVERS > 0 */


#endif /* __BT_PBAP_NEW_PROFILE__ */
#endif /* __PBAP_H */
