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

/***************************************************************************
 *
 * File:
 *     $Workfile:parseopts.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains the code which parses and exports the command
 *     line options for the stack.
 *
 * Created:
 *     October 7, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
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

#include "bt_common.h"
#include "bt_feature.h"
#include "osapi.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parseopts.h"

#if BT_STACK == XA_ENABLED
#include "prescan.h"
#endif


/*---------------------------------------------------------------------------
 *
 * Global variables
 */
#if BT_STACK == XA_ENABLED
#if !defined(__MTK_TARGET__)
	#if defined(__BT_USB_DRV__)
static UserOpts opts = { 1, 115, RADIO_TYPE_CSR_USB}; /* BT-INET Emulator, COM1 @ 115 baud */
#else
	static UserOpts opts = { 1, 115, RADIO_TYPE_MTKBT_UART, 0, FALSE}; /* BT-INET Emulator, COM1 @ 115 baud */
	#endif
#else
#if defined  (__BT_CHIP_VERSION_1__)
static UserOpts opts = { 1, 32500, RADIO_TYPE_MTKBT_UART}; /* BT-INET Emulator, COM1 @ 115 baud */
#else
static UserOpts opts = { 1, 9216, RADIO_TYPE_MTKBT_UART}; /* BT-INET Emulator, COM1 @ 115 baud */
#endif
#endif
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
static IrUserOpts iropts = { 1, ESI_9680_7401_ADAPTER, FRM_TYPE_COM }; /* COM1 ESI Jeteye dongle */
#endif /* IRDA_STACK == XA_ENABLED */

static char     errMessage[256];


/*---------------------------------------------------------------------------
 * ParseStandardOptions()
 *
 *     Parses option pointed to by 'opt' parameter. If the parameter is
 * recognized, the 'opt' pointer is advanced past the option.
 */
BOOL ParseStandardOptions(char **opt)
{
    return KAL_TRUE;
#if 0
#if BT_STACK == XA_ENABLED
    int         speed;
    I8          i, j;
#endif /* BT_STACK == XA_ENABLED */
    char       *cmd = *opt;
    BOOL        done = FALSE;

    *errMessage = 0;

    switch (*cmd++) {
    case '/':
    case '-':
        /* Look for a normal switch */
        switch (*cmd++) {
#if BT_STACK == XA_ENABLED
        case 'p':
            opts.portNum = atoi(cmd);

            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;

        case 's':
            if (strncmp(cmd, "niff", 4) == 0) {
                /* It's -sniff not a speed setting. */
                opts.startSniffer = TRUE;
                while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
                break;
            }

            i = 0;
            while (!done) {
                if (!((cmd[i] >= '0') && (cmd[i] <= '9'))) {
                    if (cmd[i] == '.') {
                        /* Decimal point */
                        j = i + 1;
                        while ((cmd[j] >= '0') && (cmd[j] <= '9')) {
                            if (j == i + 1) {
                                /* Only one digit after decimal */
                                cmd[j - 1] = cmd[j];
                            }
                            cmd[j] = ' ';
                            j++;
                        }
                    } else if (cmd[i] && (cmd[i] != ' ')) {
                        sprintf(errMessage, "Invalid speed '%s'",cmd);
                        *opt = cmd;
                        return FALSE;
                    }
                    done = TRUE;
                } 
                i++;
            }

            speed = atoi(cmd);
            opts.speed = speed;
            /* Advance past the integer value */
            while ((*cmd >= '0') && (*cmd <= '9')) cmd++;
            break;
            
        case 't':
            if (strncmp(cmd, "ERI_UART", 3) == 0) {
                opts.radioType = RADIO_TYPE_ERICSSON_UART;
            } else if (strncmp(cmd, "ST_UART", 3) == 0) {
                opts.radioType = RADIO_TYPE_ST_MICRO_UART;
            } else if (strncmp(cmd, "SIW_UART", 6) == 0) {
                opts.radioType = RADIO_TYPE_SIW_UART;
            } else if (strncmp(cmd, "SIW_USB", 6) == 0) {
                opts.radioType = RADIO_TYPE_SIW_USB;
            } else if (strncmp(cmd, "TI_UART", 2) == 0) {
                opts.radioType = RADIO_TYPE_TI_UART;
            } else if (strncmp(cmd, "INF_UART", 3) == 0) {
                opts.radioType = RADIO_TYPE_INFINEON_UART;
            } else if (strncmp(cmd, "CSR_UART", 6) == 0) {
                opts.radioType = RADIO_TYPE_CSR_UART;
            } else if (strncmp(cmd, "CSR_USB", 6) == 0) {
                opts.radioType = RADIO_TYPE_CSR_USB;
            } else if (strncmp(cmd, "CSR_BCSP", 6) == 0) {
                opts.radioType = RADIO_TYPE_CSR_BCSP;
            } else if (strncmp(cmd, "MOT_UART", 3) == 0) {
                opts.radioType = RADIO_TYPE_MOTOROLA_UART;
            } else if (strncmp(cmd, "ESI_INET", 3) == 0) {
                opts.radioType = RADIO_TYPE_IA_INET;
                /* "iAnywhere INET" is the same as ESI_INET. */
            } else if (strncmp(cmd, "IA_INET", 3) == 0) {
                opts.radioType = RADIO_TYPE_IA_INET;
            } else if (strncmp(cmd, "BC_UART", 3) == 0) {
                opts.radioType = RADIO_TYPE_BROADCOM_UART;
            } else if (strncmp(cmd, "UART", 3) == 0) {
                opts.radioType = RADIO_TYPE_GENERIC_UART;
            } else {
                sprintf(errMessage, "Invalid radio type '%s'",cmd);
                *opt = cmd;
                return FALSE;
            }
            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;

#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
        case 'i':
            switch (*cmd++) {
            case 'p':
                // Handle up to two digit port numbers
                iropts.portNum = (*cmd++) - '0';
                if (*cmd >= '0' && *cmd <= '9')
                {
                    iropts.portNum *= 10;
                    iropts.portNum += (*cmd++) - '0';
                }

                if ((iropts.portNum < 1) || (iropts.portNum > 100)) {
                    sprintf(errMessage,"'p' switch was specified without a valid port number.");
                    *opt = cmd;
                    return FALSE;
                }
                break;

            case 't':
                if (strncmp(cmd, "ESI", 3) == 0) {
                    iropts.adapterType = ESI_9680_7401_ADAPTER;
                } else {
                    sprintf(errMessage, "Invalid dongle type '%s'",cmd);
                    *opt = cmd;
                    return FALSE;
                }
                break;
            }
            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;

        case 'f':
            switch (*cmd++) {
            case 'i':
                iropts.framerType = FRM_TYPE_INET;
                break;
            default:
            case 'c':
                iropts.framerType = FRM_TYPE_COM;
                break;
            }
            while ((*cmd!=' ') && (*cmd!='\t') && (*cmd!=0)) cmd++;
            break;                    
#endif
            /*
             * The following options are reserved by other applications.
             * -d
             * -r
             */

        default:
            /* Unrecognized - or / switch */
            sprintf(errMessage,"Unrecognized command line switch '%c'",*(cmd-1));
            return FALSE;
        }
        break;
        
    default:
        /* Unrecognized option */
        sprintf(errMessage,"Unrecognized command line option %s", cmd-1);
        return FALSE;
    }

    /* We survived the parse, return success */
    *opt = cmd;
    return TRUE;
    #endif
}

#if BT_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * getPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getPortNumOption(void)
{
    return opts.portNum;
}

/*---------------------------------------------------------------------------
 * getSpeedOption()
 *     Returns the user-selected bps rate.
 */
U16 getSpeedOption(void)
{
    return opts.speed;
}

/*---------------------------------------------------------------------------
 * getRadioType()
 *     Returns the user-selected radio type.
 */
U16 getRadioType(void)
{
    return opts.radioType;
}

/*---------------------------------------------------------------------------
 * getSnifferOption()
 *     Returns the user-selected sniffer on/off state.
 */
BOOL getSnifferOption(void)
{
    return opts.startSniffer;
}

#endif /* BT_STACK == XA_ENABLED */

#if IRDA_STACK == XA_ENABLED
/*---------------------------------------------------------------------------
 * getIrPortNumOption()
 *     Returns the user-selected port number.
 */
U16 getIrPortNumOption(void)
{
    return iropts.portNum;
}

/*---------------------------------------------------------------------------
 * getIrAdapterType()
 *     Returns the IrDA adapter type
 */
U16 getIrAdapterType(void)
{
    return iropts.adapterType;
}

/*---------------------------------------------------------------------------
 * getIrFramerType()
 *     Returns the IrDA framer type (FRM_TYPE_*)
 */
U16 getFramerType(void)
{
    return iropts.framerType;
}
#endif /* IRDA_STACK == XA_ENABLED */

/*---------------------------------------------------------------------------
 * getErrorMesage()
 *     Returns the user-selected bps rate.
 */
const char *getErrorMessage(void)
{
    return errMessage;
}

/*---------------------------------------------------------------------------
 * getStandardOptions()
 *     Returns a string which describes the standard options
 */
const char *getStandardOptions(void)
{
    return 
#if BT_STACK == XA_ENABLED
           "   -p<port num>\tBluetooth: COM port number\t\t(default=1)\n"
           "   -s<port speed>\tBluetooth: 9, 38, 57, 115, 256 <kbps>\t(default=115)\n"
           "   -t<radio type>\tBluetooth: UART, ERI_UART, SIW_UART, TI_UART\n"
           "   \t\tINF_UART, CSR_UART, MOT_UART, ST_UART, \n"
           "   \t\tSIW_USB, CSR_USB, CSR_BCSP,\n"
           "   \t\tIA_INET, BC_UART \t\t(default=IA_INET)\n"
           "   -sniff\t\tOpen sniffer at startup\n"
#endif /* BT_STACK == XA_ENABLED */
#if IRDA_STACK == XA_ENABLED
           "   -ip<port num>\tIrDA: COM port number\t\t(default=1)\n"
           "   -it<adapt type>\tIrDA: ESI 9880-7501\t\t(default=ESI)\n"
           "   -f<type>\tIrDA: Framer type: c=COM, i=TCP/IP\t(default=COM)\n"
#endif /* IRDA_STACK == XA_ENABLED */
           "";
}


