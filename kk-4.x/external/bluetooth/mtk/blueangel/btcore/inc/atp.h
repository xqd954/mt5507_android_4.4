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

#ifndef __ATP_H
#define __ATP_H
/****************************************************************************
 *
 * File:
 *     $Workfile:atp.h$ for iAnywhere Blue SDK, Version 2.1.1
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

#include "atconfig.h"
#include "xatypes.h"
#include "xastatus.h"
#include "utils.h"

/*---------------------------------------------------------------------------
 * AT Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 */


/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
/* Or'd with AT_??? command to create "Read" form of command/result ("?"). 
 */
#define AT_READ             0x1000

/* Or'd with AT_??? command to create "Test" form of command/result ("=?"). 
 */
#define AT_TEST             0x2000

/* Number of AT command groups supported by the processor. */
#if defined(USE_EXT_AT_PARSER)
//#define AT_SKIP_GROUP         0xFFFFFFFF
#define AT_NUM_GROUPS       3
#else
#define AT_NUM_GROUPS       6
#endif


#define MAX_NUM_CHARSET 7


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 */
typedef U16 AtCommand;

/* It is CMD "AT" used to check if remote is alive */
#define AT_CHECK_ALIVE		0x00FE

/** This result acknowledges successful acceptance of a command. There are no
 *  parameters with this command.
 */
#define AT_OK                   0x0000

/** This result indicates that the received command was not accepted. There 
 *  are no parameters with this command.
 */
#define AT_ERROR                0x0001

/** This unsolicited result indicates an incoming call signal from network. 
 *  There are no parameters with this command.
 */
#define AT_RING                 0x0002

/** This result indicates that the received command was not accepted. The
 *  "error" parameter is used with this command.
 */
#define AT_EXTENDED_ERROR       0x0003

/** This command is used to enable or disable extended error reporting. The
 * "error" parameter is used with this command.
 */
#define AT_SET_ERROR_MODE       0x0004

#if (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) || (AT_SMS == XA_ENABLED)
/** This command informs the mobile equipment of the character set used by
 *  the terminal equipment. The "charSet" parameter is used with this command.
 */
#define AT_SELECT_CHARACTER_SET 0x0005
#endif

#if (AT_HANDSFREE == XA_ENABLED) || (AT_DUN == XA_ENABLED)

/** This result indicates that the call could not be established or the
 *  connection was terminated. There are no parameters with this command.
 */
#define AT_NO_CARRIER           0x0006

/** This result indicates that the called party is busy. There are no 
 *  parameters with this command. 
 */
#define AT_BUSY                 0x0007

/** This result indicates connection completion on timeout. There are no
 *  parameters with this command. 
 */
#define AT_NO_ANSWER            0x0008

/** This result indicates that calls to the number are temporarilly forbidden.
 *  There are no parameters with this command.
 */
#define AT_DELAYED              0x0009

/** This result indicates that calls to the number are forbidden until a
 *  manual reset. There are no parameters with this command.
 */
#define AT_BLACKLISTED          0x000A

/** This command request manufacture ID of ME. The response contains single 
 *    or multiple lines of information
 */
#define AT_MANUFACTURE_ID		0x000B

/** This command request model ID of ME. The response contains single 
 *    or multiple lines of information
 */
#define AT_MODEL_ID			0x000C

/** This indicates that the daata will be sent as 'raw' AT data.  The
 *  "data" parameter is used with this response.
 */
#define AT_RAW                  0x000D

#endif /* (AT_HANDSFREE == XA_ENABLED) */

/* End of AtCommand */


/*---------------------------------------------------------------------------
 * AtResultFlag type
 *
 */
typedef U8 AtResultFlag;

/** This flag set inticates the result is sent but not completed yet. */
#define AT_RESULT_FLAG_ONGOING  0x01

/** This flag set inticates HFG to sent OK response after the completion of sending result.
 *   This bit will be reset after the OK response is sent.
 */
#define AT_RESULT_FLAG_AUTO_OK  0x02

/** This flag set inticates the result is responsed by HFG itself not by ADP */
#define AT_RESULT_FLAG_SELF_HANDLED  0x04
/* End of AtResultFlag */

/*---------------------------------------------------------------------------
 * AtStatus type
 *
 */
typedef U8 AtStatus;

/** The function completed successfully. */
#define AT_STATUS_OK                    XA_STATUS_SUCCESS

/** A function parameter was invalid. */
#define AT_STATUS_INVALID_PARM          XA_STATUS_INVALID_PARM

/** The buffer is insufficient to encode the command. */
#define AT_STATUS_NO_RESOURCES          XA_STATUS_NO_RESOURCES

/** The command is recognized but the format is not supported. For example
 *  decoding the test result for a command that we only support reading would
 *  return this error code.
 */
#define AT_STATUS_NOT_SUPPORTED         XA_STATUS_NOT_SUPPORTED

/** The command is not recognized. */
#define AT_STATUS_NOT_FOUND             XA_STATUS_NOT_FOUND

/* For SMS send and write command. it still have to get data after command
is completed */
#define AT_STATUS_CONTINUE			XA_STATUS_CONTINUE

/* End of AtStatus */

/*--------------------------------------------------------------------------
 * AtError type
 *
 *     Error types used with AT_EXTENEDED_ERROR result.
 */
typedef U8 AtError;

#define ATCME_NO_CONNECTION             1
#define ATCME_OP_NOT_ALLOWED            3
#define ATCME_OP_NOT_SUPPORTED          4
#define ATCME_PH_SIM_PIN_REQUIRED       5
#define ATCME_SIM_NOT_INSERTED          10
#define ATCME_SIM_PIN_REQUIRED          11
#define ATCME_SIM_PUK_REQUIRED          12
#define ATCME_SIM_FAILURE               13
#define ATCME_SIM_BUSY                  14  
#define ATCME_INCORRECT_PASSWORD        16
#define ATCME_SIM_PIN2_REQUIRED         17
#define ATCME_SIM_PUK2_REQUIRED         18
#define ATCME_MEMORY_FULL               20
#define ATCME_INVALID_INDEX             21
#define ATCME_MEMORY_FAILURE            23
#define ATCME_TEXT_STRING_TOO_LONG      24
#define ATCME_INVALID_CHARS_IN_TEXT_STRING  25
#define ATCME_DIAL_STRING_TOO_LONG      26
#define ATCME_INVALID_CHARS_IN_DIAL_STRING 27
#define ATCME_NO_NETWORK_SERVICE        30
#define ATCME_NETWORK_NOT_ALLOWED       32
#define ATCME_UNKNOWN                   100

/* End of AtError */


/*---------------------------------------------------------------------------
 * AtNumberFormat type
 *
 *     This type is used to convey the format of a phone number. The format
 *     is created by or'ing together an ATNUM_TYPE value with an ATNUM_PLAN.
 *     For more information refer to GSM 04.08 section 10.5.4.7.
 */
typedef U8 AtNumberFormat;

#define ATNUM_TYPE_UNKNOWN          0x80
#define ATNUM_TYPE_INTERNATIONAL    0x90
#define ATNUM_TYPE_NATIONAL         0xA0
#define ATNUM_TYPE_NETWORK_SPEC     0xB0
#define ATNUM_TYPE_DEDICATED_ACC    0xC0

#define ATNUM_PLAN_UNKNOWN          0x00
#define ATNUM_PLAN_ISDN_TELEPHONY   0x01
#define ATNUM_PLAN_DATA_NUMBERING   0x03
#define ATNUM_PLAN_TELEX_NUMBERING  0x04
#define ATNUM_PLAN_NATIONAL         0x08
#define ATNUM_PLAN_PRIVATE          0x09

/* Commonly used formats */
#define ATNUM_INTERNATIONAL_ISDN    (ATNUM_TYPE_INTERNATIONAL|ATNUM_PLAN_ISDN_TELEPHONY)
#define ATNUM_ISDN_TELEPHONY        (ATNUM_TYPE_UNKNOWN|ATNUM_PLAN_ISDN_TELEPHONY)
#define ATNUM_NATIONAL_ISDN         (ATNUM_TYPE_NATIONAL|ATNUM_PLAN_ISDN_TELEPHONY)

/* End of AtNumberFormat */

/* Pull in definitions for command group specific structures. */
#include "at_hs.h"
#include "at_hf.h"
#include "at_pb.h"
#include "at_sms.h"
#include "at_dun.h"

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtContext 
 *
 *     Private information necessary for the encoding and decoding of AT
 *     commands and results.
 */
typedef struct _AtContext {
    U8      lastCmd[AT_NUM_GROUPS];
} AtContext;


/*---------------------------------------------------------------------------
 * AtCommands structure
 *
 *     This structure is used to specify the parameters associated with
 *     all supported AT commands. Commands are sent by the Terminal Entity
 *     (e.g. Handsfree unit) and received on the Mobile Entity (e.g. Gateway).
 */
typedef struct _AtCommands {

    /* This field is intended for use by the structure owner. */
    ListEntry           node;

    /* AT_??? type from any specified command group. The type may include
     * the AT_READ or AT_TEST flags. No parameters are specified when
     * reading or testing a command.
     */
    AtCommand           type;

	BOOL			bContinue;

    union {
        /* Headset AT command group parameters. */
        AtHeadsetCmd        hs;

        /* Handsfree AT command group parameters. */
        AtHandsfreeCmd      hf;

        /* Phonebook AT command group parameters. */
        AtPhonebookCmd      pb;

        /* Short Message Service AT command group parameters. */
        AtSmsCmd            sms;

        /* Dial Up Networking AT command group parameters. */
        AtDunCmd            dun;

        /* AT_SET_ERROR_MODE */
        struct {
            /* Error mode: 0=disable, 1=enable, 2=verbose */
            U8              mode;
        } error;

#if (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED)
        /* AT_SELECT_CHARACTER_SET */
        struct {
            /* Character set type (e.g. "GSM", "HEX", "UCS2"). */
            const char     *type;
        } charSet;
#endif /* (AT_HANDSFREE == XA_ENABLED) || (AT_PHONEBOOK == XA_ENABLED) */
    } p;
} AtCommands;

/*---------------------------------------------------------------------------
 * AtResults structure
 *
 *     This structure is used to specify the parameters associated with
 *     all supported AT results. Results, solicited or unsolicited are sent 
 *     by the Mobile Entity (e.g. Gateway) and received on the Terminal Entity
 *     (e.g. Handsfree unit) 
 */
typedef struct _AtResults {

    /* This field is intended for use by the structure owner. */
    ListEntry           node;

    /* AT_??? type from any specified command group. The type may include
     * the AT_READ or AT_TEST flags. 
     */
    AtCommand           type;

    AtResultFlag            flag;

    union {
        /* Headset AT result group parameters. */
        AtHeadsetResult     hs;

        /* Handsfree AT result group parameters. */
        AtHandsfreeResult   hf;

        /* Phonebook AT command group parameters. */
        AtPhonebookResult   pb;

        /* Short Message Service AT command group parameters. */
        AtSmsResult         sms;

        /* Dial Up Networking AT command group parameters. */
        AtDunResult         dun;

        /* Raw data response */
        U8                 *data;

        /* 1 byte generic parm */
        U8                  parm8;

        /* 2 byte generic parm */
        U16                 parm16;

        /* 4 byte generic parm */
        U32                 parm32;

        /* BOOL generic parm */
        BOOL                parmB;

	 /* response of +CGMI (manuafacture ID) */
	 const char         *manufactureID;

	 /* response of +CGMI (model ID) */
	 const char         *modelID;

	 /* +CSCS: (list of supported charset) */
 	 const char 		*charset_test;	 

	 /* response of +CSCS=<charset>. */
	 const char	    *charset;
	 
        /* AT_EXTENDED_ERROR */
        struct {
            AtError         type;
        } error;

    } p;

} AtResults;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AT_Init()
 *
 *     This function initializes the AT processor. It must be called before
 *     any other AT command is used.
 *
 * Parameters:
 *     Atc - Uninitialized AtContext structure pointer.
 *     
 * Returns:
 *     AT_STATUS_OK - The command was encoded successfully.
 *     
 *     AT_STATUS_INVALID_PARM - A function parameter is invalid.
 */
AtStatus AT_Init(AtContext *Atc);

#if AT_ROLE_TERMINAL == XA_ENABLED
/*---------------------------------------------------------------------------
 * TE_Encode()
 *
 *     This function encodes an AT command for transmission to the gateway (ME).
 *     
 * Requires:
 *     AT_ROLE_TERMINAL be enabled.
 *
 * Parameters:
 *     Command - The command to encode with its parameters.
 *     
 *     Output - Data buffer that receives the encoded command.
 *
 * Returns:
 *     AT_STATUS_OK - The command was encoded successfully.
 *     
 *     AT_STATUS_NOT_FOUND - The command was not recognized.
 *     
 *     AT_STATUS_NO_RESOURCES - The "Output" buffer was too small to hold the 
 *         encoded command.
 */
AtStatus TE_Encode(AtContext *Atc, const AtCommands *Command, XaBufferDesc *Output);

/*---------------------------------------------------------------------------
 * TE_Decode()
 *     
 *     This function decodes an AT result received by the device (TE).
 *     
 * Requires:
 *     AT_ROLE_TERMINAL be enabled.
 *
 * Parameters:
 *     Result - A pointer to a result structure that receives the result
 *         type and parameters.
 *     
 *     Input - The data buffer that provides the result to decode.
 *
 * Returns:
 *     AT_STATUS_OK - The result was decoded successfully.
 *
 *     AT_STATUS_NOT_FOUND - The result type was not recognized.
 */
AtStatus TE_Decode(AtContext *Atc, AtResults *Result, XaBufferDesc *Input);

#endif /* AT_ROLE_TERMINAL == XA_ENABLED */

#if AT_ROLE_MOBILE == XA_ENABLED
/*---------------------------------------------------------------------------
 * ME_Encode()
 *
 *     This function encodes an AT result for transmission to the device (TE).
 *     
 * Requires:
 *     AT_ROLE_MOBILE be enabled.
 *
 * Parameters:
 *     Result - The result to encode with its parameters.
 *     
 *     Output - Data buffer that receives the encoded result.
 *
 * Returns:
 *     AT_STATUS_OK - The result was encoded successfully.
 *     
 *     AT_STATUS_NOT_FOUND - The result was not recognized.
 *     
 *     AT_STATUS_NO_RESOURCES - The "Output" buffer was too small to hold the 
 *         encoded result.
 */
AtStatus ME_Encode(AtContext *Atc, const AtResults *Result, XaBufferDesc *Output);

/*---------------------------------------------------------------------------
 * ME_Decode()
 *     
 *     This function decodes AT commands received by a gateway (ME).
 *     
 * Requires:
 *     AT_ROLE_MOBILE be enabled.
 *
 * Parameters:
 *     Result - A pointer to a command structure that receives the command
 *         type and parameters.
 *     
 *     Input - The data buffer that provides the command to decode.
 *
 * Returns:
 *     AT_STATUS_OK - The command was decoded successfully.
 *
 *     AT_STATUS_NOT_FOUND - The command type was not recognized.
 */
AtStatus ME_Decode(AtContext *Atc, AtCommands *Command, XaBufferDesc *Input);
#endif /* AT_ROLE_MOBILE == XA_ENABLED */


/*---------------------------------------------------------------------------
 * AT_Range2MinMax()
 *     
 *     Returns the first and last values found in range strings that contain
 *     a list of integers, such as "(0,1)", "(0-5)" or "(0,1,2)".
 *
 * Parameters:
 *     Range - A null-terminated string, devoid of whitespace.
 *     
 *     Min - A pointer to the variable which receives the first value in 
 *         the range. On error *Min and *Max == 0xff.
 *     
 *     Max - A pointer to the variable which receives the last value in 
 *         the range. If only one value is present Min == Max.
 */
void AT_Range2MinMax(const char *Range, U16 *Min, U16 *Max);


#if (AT_ROLE_TERMINAL == XA_ENABLED) && (AT_ROLE_MOBILE != XA_ENABLED)
#define AT_Encode(a, b, c)     TE_Encode(a, b, c)
#define AT_Decode(a, b, c)     TE_Decode(a, b, c)
#endif /* (AT_ROLE_TERMINAL == XA_ENABLED) && (AT_ROLE_MOBILE != XA_ENABLED) */

#if (AT_ROLE_TERMINAL != XA_ENABLED) && (AT_ROLE_MOBILE == XA_ENABLED)
#define AT_Encode(a, b, c)     ME_Encode(a, b, c)
#define AT_Decode(a, b, c)     ME_Decode(a, b, c)
#endif /* (AT_ROLE_TERMINAL != XA_ENABLED) && (AT_ROLE_MOBILE == XA_ENABLED) */

#endif /* __ATP_H */
