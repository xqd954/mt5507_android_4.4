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

#ifndef __AT_PB_H
#define __AT_PB_H
/****************************************************************************
 *
 * File:
 *     $Workfile:at_pb.h$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an Phonebook AT Command Processor.
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

#if AT_PHONEBOOK == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT Phonebook Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 *
 * The Phonebook Command Processor module provides the commands necessary
 * for managing a phonebook on an audio gateway.
 */


/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtCommand type
 *
 */
#ifndef __LINUX_SUPPRESS_ERROR__
typedef U16 AtCommand;
#endif
/** This command selects the phonebook memory storage, which is used by other
 *  phonebook commands. The read command returns the currently selected memory,
 *  and when supported by manufacturer, number of used locations and total
 *  number of locations in the memory. The test command returns a list of
 *  supported storages. The "storage" parameters are used with this command.
 */
#define AT_SELECT_PHONEBOOK_STORAGE     0x0300

/** This command returns phonebook entries in location number range <index1>..
 *  ..<index2> from the current phonebook memory storage. If <index2> is left
 *  out, only location <index1> is returned. For each entry the phone number,
 *  format and text associated with the number are returned. The test command
 *  returns the location range supported by the current storage and the maximum
 *  lengths of the number and text fields.
 */
#define AT_READ_PHONEBOOK_ENTRY         0x0301

/** This command returns phonebook entries from the current phonebook memory
 *  whose alphanumeric field starts with specified string. Entry fields
 *  returned are the location number, phone number and associated text. The
 * "read" parameters are used with this command. 
 */
#define AT_FIND_PHONEBOOK_ENTRY         0x0302

/** This command writes the phonebook entry to location number "index" in the
 *  current phonebook. Entry fields written are the phone number and text
 *  associated with the number. If those fields are omitted, the phonebook
 *  entry is deleted. If "index" is zero, and "number" is given, the entry
 *  is written to the first free location in the phonebook. The "write"
 *  parameters are used with this command. 
 */
#define AT_WRITE_PHONEBOOK_ENTRY        0x0303

/* End of AtCommand */

/*---------------------------------------------------------------------------
 * AtPbStorageType type
 *
 * Types of phonebook memory storage used by the Gateway.
 */
typedef U16 AtPbStorageType;

/** Gateway dialed calls list. */
#define AT_PBS_DIALED_CALLS         0x0001

/** SIM fixed-dialing-phonebook list. */
#define AT_PBS_FIXED_DIAL           0x0002

/** SIM last-dialing-phonebook list. */
#define AT_PBS_LAST_DIAL            0x0004

/** Gateway missed calls list. */
#define AT_PBS_MISSED_CALLS         0x0008

/** Gateway phonebook list. */
#define AT_PBS_ME_PHONEBOOK         0x0010

/** Combined Gateway and SIM phonebook list. */
#define AT_PBS_ME_SIM_COMBINED      0x0020

/** Gateway received calls list. */
#define AT_PBS_RECEIVED_CALLS       0x0040

/** SIM phonebook list. */
#define AT_PBS_SIM_PHONEBOOK        0x0080

/* End of AtPbStorageType */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * AtPhonebookCmd
 *
 */
typedef union _AtPhonebookCmd {

    /* AT_SELECT_PHONEBOOK_STORAGE */
    struct {
        /* Phonebook storage type to select. */
        AtPbStorageType     select;
    } storage;

    /* AT_READ_PHONEBOOK_ENTRY */
    struct {
        /* First entry to return. */
        U16                 first;

        /* Last entry to return. To return only one entry,
         * set last = first.
         */
        U16                 last;
    } read;

    /* AT_FIND_PHONEBOOK_ENTRY */
    struct {
        /* Start text to search for. */
        const char         *text;
    } find;

    /* AT_WRITE_PHONEBOOK_ENTRY */
    struct {
        /* Index of this entry. */
        U16                 index;

        /* Phone number format. */
        AtNumberFormat      type;

        /* Phone number. */
        const char         *number;

        /* Text associated with phone number. The character set used with
         * this parameter is specified by AT_SELECT_CHARACTER_SET command.
         */
        const char         *text;
    } write;

} AtPhonebookCmd;

/*---------------------------------------------------------------------------
 * AtPhonebookResult
 *
 */
typedef union _AtPhonebookResult {

    /* AT_SELECT_PHONEBOOK_STORAGE|AT_READ */
    struct {
        /* Phonebook storage currently selected. */
        AtPbStorageType     selected;

        /* Number of used locations. */
        U16                 used;

        /* Total number of locations in the memory. */
        U16                 total;

    } storage_read;

    /* AT_SELECT_PHONEBOOK_STORAGE|AT_TEST */
    struct {
        /* Bitmap of supported storage types. */
        AtPbStorageType     supported;
    } storage_test;

    /* AT_READ_PHONEBOOK_ENTRY */
    /* AT_FIND_PHONEBOOK_ENTRY */
    struct {
        /* Index of this entry. */
        U16                 index;

        /* Phone number format. */
        AtNumberFormat      type;

        /* Phone number. */
        const char         *number;

        /* Text associated with phone number. The character set used with
         * this parameter is specified by AT_SELECT_CHARACTER_SET command.
         */
        const char         *text;
    } read, find;

    /* AT_READ_PHONEBOOK_ENTRY|AT_TEST */
    struct {
        /* Location range supported by the current storage (e.g. "0-99"). 
         * Quotes must not be included in this string.
         */
        const char         *range;
        
        /* The maximum length of the phonebook "number" field. */
        U16                 numLength;

        /* The maximum length of the phonebook "text" field. */
        U16                 textLength;
    } read_test;

    /* AT_FIND_PHONEBOOK_ENTRY|AT_TEST */
    struct {
        /* The maximum length of the phonebook "number" field. */
        U16                 numLength;

        /* The maximum length of the phonebook "text" field. */
        U16                 textLength;
    } find_test;	

    /* AT_WRITE_PHONEBOOK_ENTRY|AT_TEST */
    struct {
        /* Location range supported by the current storage (e.g. "0-99"). 
         * Quotes must not be included in this string.
         */
        const char         *range;
        
        /* The maximum length of the phonebook "number" field. */
        U16                 numLength;

        /* List of supported type by the current storage (e.g. "145,129")
         * Quotes must not be included in this string.
         */	 
	 const char         *types;

        /* The maximum length of the phonebook "text" field. */
        U16                 textLength;
    } write_test;
	
} AtPhonebookResult;

#else /* AT_PHONEBOOK == XA_ENABLED */

/* Stub structures to keep #defines out of AtResults/AtCommands structures. */
typedef U8 AtPhonebookCmd;
typedef U8 AtPhonebookResult;

#endif /* AT_PHONEBOOK == XA_ENABLED */
#endif /* __AT_PB_H */
