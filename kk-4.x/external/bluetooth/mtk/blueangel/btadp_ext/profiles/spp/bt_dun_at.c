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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*******************************************************************************
*
* Filename:
* ---------
* bt_dun_at.c
*
* Project:
* --------
*   BT Project
*
* Description:
* ------------
*   This file is used to provide DUN AT Command Parser
*
* Author:
* -------
* Ting Zheng
*
*==============================================================================
*             HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Revision: 
* $Modtime:
* $Log: 
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*==============================================================================
*******************************************************************************/
#include "bt_mmi.h"
#include "bt_ext_debug.h"
#include "bt_os_api.h" 
#include "bt_dun_at.h"

const char *AT_DunCmds[] = {
     "&C",	  // Circuit 109 (Received line signal detector) (DCD)
     "&D",	  // Circuit 108 (Data terminal ready) (DTR)
     "&F",	  // Set to Factory-defined Configuration (FDC)
     "+GCAP", // Request Complete Capabilities List (GCAP)
     "+GMI",  // Request Manufacturer Identification (GMI)
     "+GMM",  // Request Model Identification (GMM)
     "+GMR",  // Request Revision Identification (GMR)
     "A",	  // Answer (ASW)
     "D",	  // Dial (DIAL)
     "E",	  // Command Echo (CE)
     "H",	  // Hook Control (HC)
     "L",	  // Monitor Speaker Loudness (SL)
     "M",	  // Monitor Speaker Mode (SM)
     "O",	  // Return to Online Data (OD)
     "P",	  // Select Pulse Dialling (PD)
     "Q",	  // Result Code Suppression (CS)
     "S0",	  // Automatic Answer (AASW)
     "S10",   // Automatic Disconnect Delay (ADD)
     "S3",	  // Command Line Termination Character (CLTC)
     "S4",	  // Response Formatting Character (RFC)
     "S5",	  // Command Line Editing Character (CLEC)
     "S6",	  // Pause Before Blind Dialling (PBBD)
     "S7",	  // Connection Completion Timeout (may be ignored) (CCT)
     "S8",	  // Comma Dial Modifier Time (CDMT)
     "T",	  // Select Tone Dialling (STD)
     "V",	  // DCE Response Format (RF)
     "X",	  // Result Code Selection and Call Progress Monitoring Control (RCS_CPMC)
     "Z",	  // Reset To Default Configuration (RDC)
#ifdef __DUN_FOR_GSM__
     "+CGDCONT",   // Define PDP Context, used for GPRS
#endif
     0
};

const char *AT_CommonCmds[] = {
    /* AT capabilities from GSM 07.07 and V.25ter */
    "OK",          /* Acknowledgement to the execution of a command. */
    "CONNECT",    /* Connection has been established. */
    "RING", 	   /* Incoming call indication. */
    "NO CARRIER",  /* Call could not be established or connection terminated. */
    "ERROR",	   /* Error indication code. */
    "NO DIALTONE",	   /* No dial-tone detected. */
    "BUSY",        /* Called party is busy. */
    0
};

#define DUN_STRING_QUOTES	0x22

#ifdef __DUN_FOR_GSM__
/*
* Refer to "3GPP 27.007 Section 10.2.1.1"
* Dail up format: ATD(T)*99[[*call_address][*[L2P][*[cid]...]]]#
*/
#define DUN_DIALUP_STRING		"*99#"
#define DUN_DIALUP_LONG_STRING	"*99***1#"

/*
*  Specifies PDP context parameters values for a PDP context
*  identified by the (local) context identification parameter.
*  References: ETSI GSM 07.07
*  AT+CGDCONT=<cid>[,<pdp_type>[,<apn>[,<pdp_address>[,<d_comp>[,<h_comp>[,<pd1>[,<pd2>[,<pd3>[,<pd4>]]]]]]]]]
*/
#define DUN_PDP_TYPE		"IP"
#define DUN_PDP_APN_NAME		""
#else
#define DUN_DIALUP_STRING		""
#define DUN_DIALUP_LONG_STRING	""
#endif

/*
 * Macro to convert lower case ASCII characters to upper case
 */
#ifndef ToUpper
#define ToUpper(ch)  (((ch) >= 'a' && (ch) <= 'z') ? ((ch) - 'a' + 'A') : (ch))
#endif 

#define DUN_MEM_MALLOC(size)		malloc(size) //get_ctrl_buffer(size)
#define DUN_MEM_FREEIF(p)			if(p){free(p); p = NULL;} //if(p){free_ctrl_buffer(p); p = NULL;}		


static AtResponse btmtk_dun_ParseParameter(U8 *param, U16 len, AtCommand type)
{
    U16 i;

    if (type == AT_SIGNAL_DIAL)
    {
        char dailupStr[] = DUN_DIALUP_STRING;
        char dailupLStr[] = DUN_DIALUP_LONG_STRING;
        for (i = 0; i < len; i++)
        {
            if (*(param + i) == '*')
            {
                bt_ext_log("[DUN][AT] parse param, dial string len: %d, total param len: %d, stringlen: %d", len - i, len, sizeof(dailupStr));
                if (((len - i - 1 == sizeof(dailupStr) - 1) 
		    && (memcmp(param + i, dailupStr, sizeof(dailupStr) - 1) == 0))
		    || ((len - i - 1 == sizeof(dailupLStr) - 1) 
		    && (memcmp(param + i, dailupLStr, sizeof(dailupLStr) - 1) == 0)))
                {
                    return AT_CONNECT;
                }
            }
        }
        return AT_ERROR;
    }
#ifdef __DUN_FOR_GSM__
    else if (type == AT_SIGNAL_SET_GPRS_PDP_CONTEXT)
    {
        char *gprsPdpParam[2] = {DUN_PDP_TYPE, DUN_PDP_APN_NAME};
        int index = 0;
 
        if ((*param == '=') && ((*(param + 1) >= 0x31) && (*(param + 1) <= 0x41)))
        {
            U8 *pStart = param + 2;
            U8 *pEnd = pStart;		
            BT_BOOL bPair = TRUE;			
            for (i = 0; i < len; i++)
            {
                if (*pEnd++ == DUN_STRING_QUOTES)
                {
                    if (bPair)
                    {
                        pStart = pEnd;
                        bPair = FALSE;						
                        bt_ext_log("[DUN][AT] parse param, pStart: 0x%x", *pStart);
                    }
                    else
                    {
                        if ((pEnd - pStart - 1 == (U8)strlen(gprsPdpParam[index]))
			  && (memcmp(pStart, gprsPdpParam[index], pEnd - pStart - 1) == 0))
                        {
                            index++;
                            bPair = TRUE;
                            break; // do not check apn here							
                            //continue;							
                        }
                        return AT_ERROR;						
                    }
                }
            }
            return AT_OK;
        }
        else
        {
            return AT_ERROR;
        }
    }
#endif
    else
    {
        // Ignore to check parameter of other command
        return AT_OK;
    }

}

AtResponse btmtk_dun_at_decode(U8 *command, U16 len, AtCommand *cmd_out)
{
    U8 *cmdStr = NULL;
    U16 i;
    U16 offset;
    BT_BOOL match = FALSE;
    AtResponse type = AT_ERROR;

    *cmd_out = 0xffff;

    /* Commands we process must start with "AT" and end with <cr> */
    if (((*command != 'A') && (*command != 'a')) || 
        ((*(command+1) != 'T') && (*(command+1) != 't')))  
    {
        bt_ext_log("[DUN][AT] Not start with AT");
        type = AT_ERROR;
        goto exit;
    }

    /* Only start with "AT" and end with <cr>, no more command */
    if (*(command+2) == '\r')
    {
        bt_ext_log("[DUN][AT] No command");
        type = AT_OK;
        goto exit;		
    }

    /* Skip over 'AT'<command type> */
    len = len -2;

    cmdStr = (U8 *)DUN_MEM_MALLOC(len);
    if (cmdStr == NULL)
    {
        bt_ext_log("[DUN][AT] get memory failed");
        type = AT_ERROR;
        goto exit;		
    }

    memset(cmdStr, 0, len);

    
    for (i = 0; i < len; i++)
    {
        *(cmdStr + i) = ToUpper(*(command + i + 2));  /* Skip over 'AT'<command type> */
    }

    for (i = 0; AT_DunCmds[i] != 0; i++) 
    {
        for (offset = 0; offset < len; offset++) 
        {
            if (AT_DunCmds[i][offset] == 0) 
            {
                match = TRUE;
                break;
            }
            if (*(cmdStr + offset) != AT_DunCmds[i][offset]) 
            {
                break;
            }
        }
        if (match) 
        {
            break;
        }
    }

    if (!match)
    {
        bt_ext_log("[DUN][AT] Not find matched command");
        type = AT_ERROR;
        goto exit;		
    }

    bt_ext_log("[DUN][AT] Find matched command: %d, offset: %d", i, offset);
    type = btmtk_dun_ParseParameter(cmdStr + offset, len - offset, (AtCommand) i);

    *cmd_out = i;

exit:
    DUN_MEM_FREEIF(cmdStr);
    return type;
}

BT_BOOL btmtk_dun_at_encode(AtResponse type, U8 *response, U16 *len)
{
    const char *cmdStr;
    U8 *rspStr;

    bt_ext_log("[DUN][AT] dun encode response type: %d", type);

    cmdStr = AT_CommonCmds[type];

    /* space for <cr><lf><base command><cr><lf> */
    *len = strlen(cmdStr) + 4;	

    rspStr = response;

    *rspStr++ = '\r';
    *rspStr++ = '\n';
    memcpy(rspStr, cmdStr, strlen(cmdStr));
    rspStr += strlen(cmdStr);
    *rspStr++ = '\r';
    *rspStr++ = '\n';
	
    return TRUE;	
}

/*
BT_BOOL btmtk_dun_at_process(U8 *buff_in, U16 len_in, U8 **buff_out, U16 *len_out)
{
    AtResponse type;

    type = btmtk_dun_at_decode(buff_in, len_in);
    bt_ext_log("[DUN][AT] Response command type: %d", type);

    return btmtk_dun_at_encode(type, buff_out, len_out);
}
*/
