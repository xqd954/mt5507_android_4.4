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

#ifndef __TEXTMENU_H
#define __TEXTMENU_H

/***************************************************************************
 *
 * File:
 *     $Workfile:textmenu.h$  for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the structure definitions and function prototypes
 *     for the TextMenu library.
 *
 * Copyright 2000-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
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
 ****************************************************************************/

#include "osapi.h"

#ifndef TEXT_MENU_ECHO
#define TEXT_MENU_ECHO  XA_ENABLED
#endif 

/*---------------------------------------------------------------------------
 * Text Menu layer
 *
 *     This set of APIs is useful when developing a command-line-only
 *     interface.
 */

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 * Key constants
 *
 *     Handy defines for Special key values.
 */
#define KEY_BELL    0x07
#define KEY_BKSP    0x08
#define KEY_LF      0x0A
#define KEY_CR      0x0D
#define KEY_ESC     0x1B

/****************************************************************************
 *
 * Types
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MnuCommand type
 *
 *     The Menu Command type defines the commands that can be delivered to a
 *     menu. This system defines a basic set of commonly used commands.
 *     Applications are expected to define their own application menu specific
 *     commands.
 *
 */
typedef U16 MnuCommand;

#define M_NOOP          1000    /** No Command specified */
#define M_QUIT          1001    /** User request applicaion exit */
#define M_EXITING       1002    /** Application is exiting */
#define M_ACTIVATE      1003    /** Making menu the active menu */
#define M_KEYSTROKE     1004    /** Process Users keystroke */
#define M_SHOW          1005    /** Your menu is being displayed */
#define M_SWITCHMENU    1006    /** Switch to menu specified in 'Arg' */

#define M_MENUBASE      1100    /** App specific menus start here */

/* End of MnuCommand type */

/*---------------------------------------------------------------------------
 * MnuArg type
 *
 *     The Menu Argument type defines the argument parameter passed into a
 *     menu handler during a command notification. Normally, the MnuArg
 *     parameter provided to the menu handler is taken from the MnuItem
 *     structure's 'arg' field. During M_KEYSTROKE events, the MnuArg is the
 *     keystroke (unmodified) which caused the event.
 */
typedef void *MnuArg;

/* End of MnuArg type */

/*---------------------------------------------------------------------------
 * MnuErr type
 *
 *     The Menu Error type defines the status codes returned by certain
 *     menu functions.
 */
typedef U8 MnuErr;

#define MRERR_SUCCESS       0   /* The operation completed successfully */
#define MRERR_ESCAPE        1   /* The escape key was pressed */
#define MRERR_RANGE_ERR     2   /* The value is out of range */

/* End of MnuErr type */

/* Forward declarations */
typedef struct _MnuTextMenu MnuTextMenu;

/*---------------------------------------------------------------------------
 * MnuCallback type
 *
 *     A callback of this type indicates commands and arguments requested
 *     by the user.
 */
typedef BOOL(*MnuCallback) (MnuTextMenu *Menu, MnuCommand Cmd, MnuArg Arg);

/* End of MnuCallback */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * MnuItem structure
 *
 *     The Menu Item structure is used to specify a menu option. The fields
 *     describe the invoking key, description text, command identifier value
 *     and an optional argument to be passed to the menu handler function.
 *     This structure is ROMable.
 */
typedef struct _MnuItem
{
    char key;
    const char *text;
    MnuCommand cmd;
    MnuArg arg;
} MnuItem;

/*---------------------------------------------------------------------------
 * MnuTextMenu structure
 *
 *     The Text Menu structure is used to specify a menu. Typically the menu is
 *     defined with a title, a menu command handler, and a pointer to an array
 *     of Menu  Items. Such menus are notified of a keystroke when it matches
 *     one of the key's provided in the items array. The items array must
 *     terminate with an all zero's menu item. Alternatively, a menu can be
 *     designated to receive only keystrokes. Such menus do not provide a
 *     MnuItem array.
 */
struct _MnuTextMenu
{
    const char *title;
    MnuCallback handler;
    const MnuItem *items;
    U32 workspace;
};

/*---------------------------------------------------------------------------
 * MnuReadInteger structure
 *
 *     The Menu Read Integer structure is used communicate the results of a
 *     call to the Mnu_ReadInteger() function. When the completion event
 *     provided to Mnu_ReadInteger() is delivered to a menu handler, the
 *     MnuArg parameter points to this structure. The menu handler should
 *     inspect the status and value fields to interpret the results of the
 *     Mnu_ReadInteger() call.
 */
typedef struct _MnuReadInteger
{
    MnuErr status;  /* Status of Menu Read function (MRERR_). */
    U32 value;      /* Integer value read, in specified base. */

    /* === Internal use only === */
    U32 minValue;
    U32 maxValue;
    MnuCommand event;
    MnuTextMenu *parent;
    U8 base;
} MnuReadInteger;

/*---------------------------------------------------------------------------
 * MnuReadString structure
 *
 *     The Menu Read String structure is used communicate the results of a
 *     call to the Mnu_ReadString() function. When the completion event
 *     provided to Mnu_ReadString() is delivered to a menu handler, the
 *     MnuArg parameter points to this structure. The menu handler should
 *     inspect the status, buffer and length fields to interpret the results
 *     of the Mnu_ReadString() call.
 */
typedef struct _MnuReadString
{
    MnuErr status;  /* Status of Menu Read function (MRERR_). */
    char *buffer;   /* String read (null-terminated) */
    U16 length;     /* Length of string */

    /* === Internal use only === */
    MnuCommand event;
    MnuTextMenu *parent;
    U16 maxLen;
} MnuReadString;

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * Mnu_Load()
 *
 *     Loads the specified menu as the active menu.
 *     
 * Parameters:
 *     Menu - Pointer to the menu to load.
 *     Show - If true, the loader will show the menu's title and call
 *         the menu handler with the M_SHOW command.
 *
 * Returns:
 *     TRUE  - If new menu loaded successfully.
 *     FALSE - If new menu could not load.
 *
 */
BOOL Mnu_Load(MnuTextMenu *Menu, BOOL Show);

/*---------------------------------------------------------------------------
 * Mnu_ProcessKey()
 *
 *     Processes keystrokes in the context of the current menu. Delivers any
 *     valid keystrokes to the menu handler.
 *
 * Parameters:
 *     Key - The keystroke to process.
 *
 * Returns:
 *     Menu command generated as a result of the keystroke, 
 *
 */
MnuCommand Mnu_ProcessKey(char Key);

/*---------------------------------------------------------------------------
 * Mnu_Show()
 *
 *     Displays the currently active menu's options.
 *
 */
void Mnu_Show(void);

/*---------------------------------------------------------------------------
 * Mnu_ReadInteger()
 *
 *     Reads an integer for the current menu. When the read operation is 
 *     complete, the specified command is generated back to the parent
 *     with the MnuArg parameter pointing to a MnuReadInteger structure.
 *     This function returns immediately. On successful completion the
 *     status in the MnuReadInteger structure will be set to MRERR_SUCCESS.
 *
 * Parameters:
 *     Cmd - The command to indicate when the operation is complete.
 *     Min - The minimum acceptable value. If the user specified value is
 *         less than this value, the completion status is MRERR_RANGE_ERR.
 *     Max - The maximum acceptable value. If the user specified value is
 *         more than this value, the completion status is MRERR_RANGE_ERR.
 *     Base - The base (decimal or hex) of the integer to read.
 *
 */
void Mnu_ReadInteger(MnuCommand Cmd, U32 Min, U32 Max, U8 Base);

/*---------------------------------------------------------------------------
 * Mnu_ReadString()
 *
 *     Reads a string for the current menu. When the read operation is 
 *     complete, the specified command is generated back to the parent
 *     with the MnuArg parameter pointing to a MnuReadString structure.
 *     This function returns immediately. On successful completion the
 *     status in the MnuReadInteger structure will be set to MRERR_SUCCESS.
 *
 * Parameters:
 *     Cmd - The command to indicate when the operation is complete.
 *     Buffer - A pointer to the buffer to store the string in.
 *     BuffLen - The maximum length of the buffer provided.
 *
 */
void Mnu_ReadString(MnuCommand Cmd, void *Buffer, U16 BuffLen);

/*---------------------------------------------------------------------------
 * Mnu_ShowPrompt()
 *
 *      Show the prompt to the user.
 */
void Mnu_ShowPrompt(void);

/*---------------------------------------------------------------------------
 * Mnu_EchoKey()
 *
 *     Echoes a key to output display. 
 *
 * Parameters:
 *     Key - The key to echo.
 *
 */
void Mnu_EchoKey(char Key);

/*---------------------------------------------------------------------------
 * Mnu_EchoKeyNL()
 *
 *     Echoes a key to output display, moving the cursor to the next
 *     line after the echoed character.
 *
 * Parameters:
 *     Key - The key to echo.
 *
 */
void Mnu_EchoKeyNL(char Key);

#if TEXT_MENU_ECHO == XA_ENABLED
#define Mnu_EchoKey(_K)     MnuEchoKey((_K), '\r')
#define Mnu_EchoKeyNL(_K)   MnuEchoKey((_K), '\n')
#else /* TEXT_MENU_ECHO == XA_ENABLED */ 
#define Mnu_EchoKey(_K)     ((void) 0)
#define Mnu_EchoKeyNL(_K)   ((void) 0)
#endif /* TEXT_MENU_ECHO == XA_ENABLED */ 

/****************************************************************************
 *
 * Internal Functions and Macros
 *
 ****************************************************************************/
extern void MnuEchoKey(char Key, char Newline);

#endif /* __TEXTMENU_H */ 

