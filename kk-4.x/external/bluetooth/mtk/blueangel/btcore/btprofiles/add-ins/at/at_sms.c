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
 *     $Workfile:at_sms.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description: This file contains an SMS AT Command Processor.
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
#include "atp.h"
#include "sys/atpi.h"
#include "osapi.h"

#if AT_SMS == XA_ENABLED

const char *AT_SmsCmds[] = {
    /* AT capabilities from GSM 07.07 and V.25ter */   
    "+CSMS", /* Select messaging service. */
    "+CPMS", /* Select memory storages <mem1>,<mem2>,<mem3> used for reading, writing, etc. */
    "+CMGF", /* Select input and output format of messages. */
    "+CSCA", /* Set Short Message Service Centre Address [SMSC], through which mobile originated SMs are transmitted. */
    "+CSMP", /* Select values for additional parameters needed when SM is sent to the network or placed in storage. */
    "+CSCB", /* Select which types of Cell Broadcast Message are to be received by the ME. */
    "+CNMI", /* Select the procedure, how receiving of new messages from the network is indicated to the TE. */
    "+CMTI", /* Response used to notify the TE whenever SMSDELIVER is stored into ME. */
    "+CBMI", /* Response used to notify the TE whenever Cell Broadcast Message has arrived in ME. */
    "+CDS",  /* Unsolicited response code used to notify the SMSSTATUS-REPORTS to TE. */
    "+CMGL", /* Return messages with status value from message storage <mem1> to the TE. */
    "+CMGR", /* Read message from location value <index> in the message storage <mem1> of the ME. */
    "+CMGS", /* Send message from a TE to the network (SMS-SUBMIT). */
    "+CMSS", /* Send message from a specified location <index> in the message storage <mem2> of the ME to the network (SMS-SUBMIT or SMS-COMMAND). */
    "+CMGW", /* Store message (either SMS-DELIVER or SMS-SUBMIT) to the memory storage <mem2> of the ME. */
    "+CMGD", /* Delete message from preferred message storage <mem1> location <index> of the ME. */
    "+CSDH", /* Select values for additional parameters needed when SM is sent to the network or placed in storage. */    
    "+CMS ERROR",	/* Message Service Failure Result Code "+CMS ERROR" */
    0
};

static const char *SmsStorageTypes[] = {
    "BM", "ME", "MT", "SM", "TA", "SR", 0
};

static const U8 nSmsStorageTypes = (sizeof(SmsStorageTypes)/sizeof(SmsStorageTypes[0])) - 1;

static const char *SmsStatus[] = {
    "REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL", 0
};

static const U8 nSmsStatus = (sizeof(SmsStatus)/sizeof(SmsStatus[0])) - 1;

AtSmsStatus convertSMSStatus(const char *strStatus, U8 len);
void InsertString(XaBufferDesc *buf, const char *str, BOOL bBrace, BOOL bComma);
void InsertInt(XaBufferDesc *buf, U32 data, BOOL bComma);


#if AT_ROLE_TERMINAL == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Sms
 * 
 *     Encodes TE originated AT commands. 
 */ 
AtStatus TE_Encode_Sms(AtContext *Atc, U16 Type, const AtSmsCmd *Command, XaBufferDesc *Output)
{
    /*
    U16     avail = (Output->buffSize - Output->writeOffset) - 1;
    */

    switch (Type) {
    default:
        return AT_STATUS_NOT_SUPPORTED;
    }
    return AT_STATUS_OK;
}

/*--------------------------------------------------------------------------- 
 * AT_Decode_Sms
 * 
 *     Decodes AT results received by the Common Unit (TE).
 */
AtStatus TE_Decode_Sms(AtContext *Atc, U16 *Type, AtSmsResult *Result, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) {
    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_TERMINAL == XA_ENABLED */


#if AT_ROLE_MOBILE == XA_ENABLED
/*--------------------------------------------------------------------------- 
 * AT_Encode_Sms
 * 
 *     Encodes Audio Gateway (ME) originated AT results.
 */ 
AtStatus ME_Encode_Sms(AtContext *Atc, U16 Type, const AtSmsResult *Result, XaBufferDesc *Output)
{
    U16     avail = (Output->buffSize - Output->writeOffset) - 2;
    U16    len,val;
    U16    p,i;
    U16    used, total;
	
    switch (Type) 
    {
    case AT_SELECT_SMS_SERVICE:
        /* Syntax: +CSMS: <mt>,<mo>,<bm> */
        if (avail < 5) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->buff[Output->writeOffset++] = Result->service_result.mt ? '1' : '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->service_result.mo ? '1' : '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->service_result.bm ? '1' : '0';
        break;
    case (AT_SELECT_SMS_SERVICE | AT_READ):
        /* Syntax: +CSMS: <service><mt>,<mo>,<bm> */
        if (avail < 16) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->service.service);
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->service.mt ? '1' : '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->service.mo ? '1' : '0';
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = Result->service.bm ? '1' : '0';        
        break;
		
    case (AT_SELECT_SMS_SERVICE | AT_TEST):
        /* Syntax: +CSMS: (list of supported <service>s) */
        len = OS_StrLen(Result->supportedServices);
        if (avail < len) {
            return AT_STATUS_NO_RESOURCES;
        }
        //Output->buff[Output->writeOffset++] = '(';
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->supportedServices), len);
        Output->writeOffset += len;
        //Output->buff[Output->writeOffset++] = ')';
        break;

    case AT_PREFERRED_SMS_STORAGE:
        /* Syntax: +CPMS: <used1>,<total1>,<used2>,<total2>,<used3>,<total3> */
        if (avail < 35/* 6*5+5 */) {
            return AT_STATUS_NO_RESOURCES;
        }
	 if(Result->preferred_result.count > 0)
	 {
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->preferred_result.readUsed);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->preferred_result.readTotal);
	 }
	 if(Result->preferred_result.count > 1)
	 {	 
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->preferred_result.writeUsed);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->preferred_result.writeTotal);
	 }
	 if(Result->preferred_result.count > 2)
	 {	 
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->preferred_result.recvUsed);
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->preferred_result.recvTotal);		
 	 }
        break;

    case (AT_PREFERRED_SMS_STORAGE|AT_READ):
        /* Syntax: +CPMS: <mem1>,<used1>,<total1>,<mem2>,<used2>,<total2>,<mem3>,<used3>,<total3> */

        for(p = 0;p < 3;p++)
        {
            if(p == 0)
            {
	         val = Result->preferred_read.read;
	         used = Result->preferred_read.readUsed;
	         total = Result->preferred_read.readTotal;
            }
	     else if(p == 1)
	     {
		  val = Result->preferred_read.write;
	         used = Result->preferred_read.writeUsed;
	         total = Result->preferred_read.writeTotal;
	     }
	     else
	     {
		  val = Result->preferred_read.recv;
	         used = Result->preferred_read.recvUsed;
	         total = Result->preferred_read.recvTotal;
	     }
            for (i = 0; SmsStorageTypes[i]; i++) 
	     {
                if (val == (1 << i)) 
		  {
                    break;
                }
            }
			
	     if(i == nSmsStorageTypes)
	     {
	     	   /* Not found keep it empty */
                Output->buff[Output->writeOffset++] = ',';
                Output->buff[Output->writeOffset++] = ',';
	     }
	     else
	     {
                Output->buff[Output->writeOffset++] = '\"';
	         Output->buff[Output->writeOffset++] = SmsStorageTypes[i][0];
	         Output->buff[Output->writeOffset++] = SmsStorageTypes[i][1];
	         Output->buff[Output->writeOffset++] = '\"';
	         Output->buff[Output->writeOffset++] = ',';

	         Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, used);
	         Output->buff[Output->writeOffset++] = ',';
	         Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, total);
	     }
            if(p < 2)
	         Output->buff[Output->writeOffset++] = ',';
        }
        break;

    case (AT_PREFERRED_SMS_STORAGE|AT_TEST):
        /* Syntax: +CPMS: (list of supported <mem1>s),(list of supported <mem2>s),(list of supported <mem3>s) */
        for(p = 0;p < 3;p++)
      	 {
            if(p == 0)
            {
	         val = Result->preferred_test.read;
            }
	     else if(p == 1)
	     {
		  val = Result->preferred_test.write;
	     }
	     else
	     {
		  val = Result->preferred_test.recv;
	     }

	     Output->buff[Output->writeOffset++] = '(';
	     for (i = 0; SmsStorageTypes[i]; i++) 
	     {
                if (val & (1 << i)) 
		  {
                    Output->buff[Output->writeOffset++] = '\"';
	             Output->buff[Output->writeOffset++] = SmsStorageTypes[i][0];
	             Output->buff[Output->writeOffset++] = SmsStorageTypes[i][1];
	             Output->buff[Output->writeOffset++] = '\"';
		      Output->buff[Output->writeOffset++] = ',';
                }
            }
	     if(Output->buff[Output->writeOffset-1] == ',')
	         Output->writeOffset--;
           Output->buff[Output->writeOffset++] = ')';
	     if(p < 2)
	         Output->buff[Output->writeOffset++] = ',';
      	 }
        break;
    case (AT_SMS_MESSAGE_FORMAT|AT_READ):
        /* Syntax: +CMGF: <mode> */
	  if(Result->msgFormat != 0 && Result->msgFormat != 1)
	  	return AT_STATUS_INVALID_PARM;
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->msgFormat);
        break;

    case (AT_SMS_MESSAGE_FORMAT|AT_TEST):
	  /* Syntax: +CMGF: (list of supported <mode>s) */
        len = OS_StrLen(Result->supportedMsgFormat);
        //Output->buff[Output->writeOffset++] = '(';
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->supportedMsgFormat), len);
        Output->writeOffset += len;
        //Output->buff[Output->writeOffset++] = ')';
        break;

    case (AT_SMS_SERVICE_CENTER|AT_READ):
         /* Syntax: +CSCA: <sca>,<tosca> */
	  len = OS_StrLen(Result->SCA.sca);
        if(len == 0)
            return AT_STATUS_NOT_SUPPORTED;
        Output->buff[Output->writeOffset++] = '\"';			
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->SCA.sca), len);
        Output->writeOffset += len;
        Output->buff[Output->writeOffset++] = '\"';
	 Output->buff[Output->writeOffset++] = ',';
	 Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->SCA.tosca);
        break;

    case (AT_SET_TEXT_MODE_PARMS|AT_READ):
	/* Syntax: +CSMP: <fo>,<vp>,<pid>,<dcs> */
	Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->textModeParams.fo);
	Output->buff[Output->writeOffset++] = ',';
	if( Result->textModeParams.strVp && (len = OS_StrLen(Result->textModeParams.strVp)) > 0 )
	{
		Output->buff[Output->writeOffset++] = '\"';
		OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->textModeParams.strVp), len);
		Output->writeOffset += len;
		Output->buff[Output->writeOffset++] = '\"';
	}
	else
	{
		Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->textModeParams.intVp);
	}
	Output->buff[Output->writeOffset++] = ',';
	Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->textModeParams.pid);
	Output->buff[Output->writeOffset++] = ',';
	Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->textModeParams.dcs);
        break;

    case (AT_SET_CBM_TYPES|AT_READ):
        /* Syntax: +CSCB: <mode>,<mids>,<dcss> */
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->CBMTypes.mode);
        Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '\"';	
        len = OS_StrLen(Result->CBMTypes.mids);
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->CBMTypes.mids), len);
        Output->writeOffset += len;
        Output->buff[Output->writeOffset++] = '\"';
	 Output->buff[Output->writeOffset++] = ',';
        Output->buff[Output->writeOffset++] = '\"';
        len = OS_StrLen(Result->CBMTypes.dcss);
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->CBMTypes.dcss), len);
        Output->writeOffset += len;
        Output->buff[Output->writeOffset++] = '\"';
        break;

    case (AT_SET_CBM_TYPES|AT_TEST):
        /* Syntax: +CSCB: (list of supported <mode>s) */
        len = OS_StrLen(Result->supportedCBMMode);
        Output->buff[Output->writeOffset++] = '(';
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->supportedCBMMode), len);
        Output->writeOffset += len;
        Output->buff[Output->writeOffset++] = ')';        
        break;

    case (AT_NEW_MESSAGE_INDICATION|AT_READ):
        /* Syntax: +CNMI: <mode>,<mt>,<bm>,<ds>,<bfr> */
        for(p = 0;p < 5;p++)
        {
            Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, *((U8*)&(Result->newMsgIndSetting.mode)+p));
            if(p < 4)
                Output->buff[Output->writeOffset++] = ',';
        }
        break;

    case (AT_NEW_MESSAGE_INDICATION|AT_TEST):
        /* Syntax: +CNMI: (list of supported <mode>s),(list of supported <mt>s),(list of supported <bm>s),
                                   (list of supported <ds>s),(list of supported <bfr>s) */
        #if 1
        if(Result->newMsgInd_test)
    	 {
            len = OS_StrLen(Result->newMsgInd_test);
	     if(len)
     	     {
	         OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->newMsgInd_test), len);
		   Output->writeOffset += len;
	     }
	     else
     	     {
     	         return AT_STATUS_INVALID_PARM;
     	     }
      	 } 
	 else
	 {
		return AT_STATUS_INVALID_PARM;
	 }
	 #else
        for(p = 0;p < 5;p++)
        {
            len = OS_StrLen(*((const char**)&(Result->supportedMsgIndSetting)+p));
            Output->buff[Output->writeOffset++] = '(';
            OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(*((const char**)&(Result->supportedMsgIndSetting)+p)), len);
            Output->writeOffset += len;
            Output->buff[Output->writeOffset++] = ')';
            if(p < 4)
                Output->buff[Output->writeOffset++] = ',';
        }
	 #endif
        break;

    case AT_SMS_DELIVER_INDICATION:
    case AT_CELL_BROADCAST_MSG_ARRIVE:
        /* Syntax: +CMTI: <mem>,<index> */
        val = Result->newMsgIndication.mem;
        for (i = 0; SmsStorageTypes[i]; i++) 
	 {
            if (val == (1 << i)) 
            {
                    break;
            }
        }
        if(i == nSmsStorageTypes)
	 {
            return AT_STATUS_NOT_SUPPORTED;
	 }
	 else
	 {
            Output->buff[Output->writeOffset++] = '\"';
	     Output->buff[Output->writeOffset++] = SmsStorageTypes[i][0];
	     Output->buff[Output->writeOffset++] = SmsStorageTypes[i][1];
	     Output->buff[Output->writeOffset++] = '\"';
        }
        Output->buff[Output->writeOffset++] = ',';
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->newMsgIndication.index);
        break;

    case AT_SMS_STATUS_REPORT:
        /* Syntax: +CDS: <length><CR><LF><pdu> */
        //Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->length);
        /* ADP shall use RAW mode to send PDU to HF */
        break;
        
    case AT_LIST_MESSAGES:
        /* Syntax(SM storage): +CMGL: <index>,<stat>,[<alpha>],<length><CR><LF><pdu> */
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->listMsg.index);	/* <index> */
        Output->buff[Output->writeOffset++] = ',';
	 if(Result->listMsg.type != AT_SMS_TYPE_PDU)
 	 {	/* status */
 	       Assert(Result->listMsg.stat < NUM_OF_AT_SMS_STATUS);
		InsertString(Output, SmsStatus[Result->listMsg.stat], FALSE, TRUE);
		/*
        	len = OS_StrLen(SmsStatus[Result->listMsg.stat]);
            Output->buff[Output->writeOffset++] = '\"';
        	OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(SmsStatus[Result->listMsg.stat]), len);
            Output->writeOffset += len;
        	*/
 	 }
	 else
	 {
	 	/* Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->listMsg.stat);*/
		InsertInt(Output, Result->listMsg.stat, TRUE);
        }
	 //Output->buff[Output->writeOffset++] = ',';
	switch(Result->listMsg.type)
	{
	case AT_SMS_TYPE_DELIVER:
	case AT_SMS_TYPE_SUBMIT:
		InsertString(Output, Result->listMsg.msg.deliver.addr, FALSE, TRUE);		/* <oa/da> */
		if(Result->listMsg.msg.deliver.mask & AT_SMS_MASK_ALPHA)
			InsertString(Output, Result->listMsg.msg.deliver.alpha, FALSE, TRUE);		/* <oa/da> */
		else
        Output->buff[Output->writeOffset++] = ',';
		if(Result->listMsg.msg.deliver.mask & AT_SMS_MASK_SCTS)
			InsertString(Output, Result->listMsg.msg.deliver.SCTimeStamp, FALSE, FALSE);		/* <scts> */
		if(Result->listMsg.msg.deliver.mask & AT_SMS_MASK_TOADDR)
		{
			Output->buff[Output->writeOffset++] = ',';
			InsertInt(Output, Result->listMsg.msg.deliver.addrType, TRUE);		/* <tooa/toda> */
			InsertInt(Output, Result->listMsg.msg.deliver.length, FALSE);		/* <length> */
		}
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->listMsg.msg.deliver.data, TRUE, FALSE);		/* <data> */
		break;
	case AT_SMS_TYPE_STATUS:
		InsertInt(Output, Result->listMsg.msg.status.fo, TRUE);		/* <fo> */
		InsertInt(Output, Result->listMsg.msg.status.mr, TRUE);		/* <mr> */
		if(Result->listMsg.msg.status.mask & AT_SMS_MASK_RA)
			InsertString(Output, Result->listMsg.msg.status.ra, FALSE, TRUE);		/* <ra> */
		else
			Output->buff[Output->writeOffset++] = ',';
		if(Result->listMsg.msg.status.mask & AT_SMS_MASK_TORA)
			InsertInt(Output, Result->listMsg.msg.status.addrType, TRUE);		/* <tora> */
		else
			Output->buff[Output->writeOffset++] = ',';
		InsertString(Output, Result->listMsg.msg.status.SCTimeStamp, FALSE, TRUE);		/* <scts> */
		InsertString(Output, Result->listMsg.msg.status.dt, FALSE, TRUE);				/* <dt> */
		InsertInt(Output, Result->listMsg.msg.status.st, FALSE);						/* <st> */
		break;
	case AT_SMS_TYPE_COMMAND:
		InsertInt(Output, Result->listMsg.msg.cmd.fo, TRUE);		/* <fo> */
		InsertInt(Output, Result->listMsg.msg.cmd.ct, FALSE);		/* <ct> */
		break;
	case AT_SMS_TYPE_CBM:
		InsertInt(Output, Result->listMsg.msg.cbm.sn, TRUE);			/* <sn> */
		InsertInt(Output, Result->listMsg.msg.cbm.mid, TRUE);		/* <mid> */
		InsertInt(Output, Result->listMsg.msg.cbm.page&0xF, TRUE);			/* <page> */
		InsertInt(Output, ((Result->listMsg.msg.cbm.page>>4)&0xF), FALSE);			/* <pages> */
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->listMsg.msg.cbm.data, TRUE, FALSE);
		break;
	case AT_SMS_TYPE_PDU:
		if(Result->listMsg.msg.pdu.mask&AT_SMS_MASK_ALPHA)
			InsertString(Output, Result->listMsg.msg.pdu.alpha, FALSE, TRUE);	/* <alpha> */
		else
			Output->buff[Output->writeOffset++] = ',';
		InsertInt(Output, Result->listMsg.msg.pdu.length, FALSE);						/* <length> */
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->listMsg.msg.pdu.PDU, TRUE, FALSE);		/* <data> */
		break;
	default:
		break;
	}
        break;

    case (AT_LIST_MESSAGES|AT_TEST):
        /* Syntax: +CMGL: (list of supported <stat>s) */
         len = OS_StrLen(Result->supportedStatus);
        /* Output->buff[Output->writeOffset++] = '('; */
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->supportedStatus), len);
        Output->writeOffset += len;
        /* Output->buff[Output->writeOffset++] = ')'; */
        break;
        
    case AT_READ_MESSAGE:
        /* Syntax: +CMGR: <stat>,[<alpha>],<length><CR><LF><pdu> */
	 if(Result->readMsg.type != AT_SMS_TYPE_PDU)
	 {	/* status */
		   Assert(Result->readMsg.stat < NUM_OF_AT_SMS_STATUS);
		   InsertString(Output, SmsStatus[Result->readMsg.stat], FALSE, TRUE);	/* <stat> */
        }
	 else
        {
	 	InsertInt(Output, Result->readMsg.stat, TRUE);	/* <stat> */
        }
	switch(Result->readMsg.type)
	{
	case AT_SMS_TYPE_DELIVER:
		InsertString(Output, Result->readMsg.msg.deliver.addr, FALSE, TRUE);		/* <oa> */
		if(Result->readMsg.msg.deliver.mask & AT_SMS_MASK_ALPHA)
			InsertString(Output, Result->readMsg.msg.deliver.alpha, FALSE, TRUE);		/* <alpha> */
		else
        Output->buff[Output->writeOffset++] = ',';
		InsertString(Output, Result->readMsg.msg.deliver.SCTimeStamp, FALSE, FALSE);		/* <scts> */
		if(Result->readMsg.msg.deliver.mask & AT_SMS_MASK_TOADDR)
		{
			Output->buff[Output->writeOffset++] = ',';
			InsertInt(Output, Result->readMsg.msg.deliver.addrType, TRUE);		/* <tooa> */
			InsertInt(Output, Result->readMsg.msg.deliver.fo, TRUE);			/* <fo> */
			InsertInt(Output, Result->readMsg.msg.deliver.pid, TRUE);			/* <pid> */
			InsertInt(Output, Result->readMsg.msg.deliver.dcs, TRUE);			/* <dcs> */
			InsertString(Output, Result->readMsg.msg.deliver.SCAddr, FALSE, TRUE);	/* <sca> */
			InsertInt(Output, Result->readMsg.msg.deliver.SCAddrType, TRUE);			/* <tosca> */
			InsertInt(Output, Result->readMsg.msg.deliver.length, FALSE);		/* <length> */
		}
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->readMsg.msg.deliver.data, TRUE, FALSE);		/* <data> */
        break;
	case AT_SMS_TYPE_SUBMIT:
		InsertString(Output, Result->readMsg.msg.submit.addr, FALSE, TRUE);		/* <oa> */
		if(Result->readMsg.msg.submit.mask & AT_SMS_MASK_ALPHA)
			InsertString(Output, Result->readMsg.msg.submit.alpha, FALSE, FALSE);		/* <alpha> */
		if(Result->readMsg.msg.submit.mask & AT_SMS_MASK_TOADDR)
		{
			Output->buff[Output->writeOffset++] = ',';
			InsertInt(Output, Result->readMsg.msg.submit.addrType, TRUE);		/* <toda> */
			InsertInt(Output, Result->readMsg.msg.submit.fo, TRUE);			/* <fo> */
			InsertInt(Output, Result->readMsg.msg.submit.pid, TRUE);			/* <pid> */
			InsertInt(Output, Result->readMsg.msg.submit.dcs, TRUE);			/* <dcs> */
			if(Result->readMsg.msg.submit.mask & AT_SMS_MASK_VP)
			{
				if(Result->readMsg.msg.submit.strVp != NULL)
					InsertString(Output, Result->readMsg.msg.submit.strVp, FALSE, TRUE);	/* <vp> */
				else
					InsertInt(Output, Result->readMsg.msg.submit.intVp, TRUE);			/* <vp> */
        }
			else
			{
				Output->buff[Output->writeOffset++] = ',';
			}
			InsertString(Output, Result->readMsg.msg.submit.SCAddr, FALSE, TRUE);	/* <sca> */
			InsertInt(Output, Result->readMsg.msg.submit.SCAddrType, TRUE);		/* <tosca> */
			InsertInt(Output, Result->readMsg.msg.submit.length, FALSE);			/* <length> */
		}
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->readMsg.msg.submit.data, TRUE, FALSE);		/* <data> */		
        break;
	case AT_SMS_TYPE_STATUS:
		InsertInt(Output, Result->readMsg.msg.status.fo, TRUE); 	/* <fo> */
		InsertInt(Output, Result->readMsg.msg.status.mr, TRUE); 	/* <mr> */
		if(Result->readMsg.msg.status.mask & AT_SMS_MASK_RA)
			InsertString(Output, Result->readMsg.msg.status.ra, FALSE, TRUE);		/* <ra> */
		else
			Output->buff[Output->writeOffset++] = ',';
		if(Result->readMsg.msg.status.mask & AT_SMS_MASK_TORA)
			InsertInt(Output, Result->readMsg.msg.status.addrType, TRUE);		/* <tora> */
		else
			Output->buff[Output->writeOffset++] = ',';
		InsertString(Output, Result->readMsg.msg.status.SCTimeStamp, FALSE, TRUE);		/* <scts> */
		InsertString(Output, Result->readMsg.msg.status.dt, FALSE, TRUE);				/* <dt> */
		InsertInt(Output, Result->readMsg.msg.status.st, FALSE);						/* <st> */
		break;
	case AT_SMS_TYPE_COMMAND:
		InsertInt(Output, Result->readMsg.msg.cmd.fo, TRUE);		/* <fo> */
		InsertInt(Output, Result->readMsg.msg.cmd.ct, FALSE);	/* <ct> */
		if(Result->readMsg.msg.cmd.mask & AT_SMS_MASK_PID)
		{
			Output->buff[Output->writeOffset++] = ',';
			InsertInt(Output, Result->readMsg.msg.cmd.pid, TRUE);	/* <pid> */
			if(Result->readMsg.msg.cmd.mask & AT_SMS_MASK_MN)
				InsertInt(Output, Result->readMsg.msg.cmd.mn, TRUE);	/* <mn> */
			else
				Output->buff[Output->writeOffset++] = ',';
			if(Result->readMsg.msg.cmd.mask & AT_SMS_MASK_ADDR)
				InsertString(Output, Result->readMsg.msg.cmd.destAddr, FALSE, TRUE);	/* <da> */
			else
				Output->buff[Output->writeOffset++] = ',';
			if(Result->readMsg.msg.cmd.mask & AT_SMS_MASK_TOADDR)
				InsertInt(Output, Result->readMsg.msg.cmd.addrType, TRUE);	/* <toda> */
			else
				Output->buff[Output->writeOffset++] = ',';
			InsertInt(Output, Result->readMsg.msg.cmd.length, FALSE);	/* <length> */
			Output->buff[Output->writeOffset++] = '\r';
			Output->buff[Output->writeOffset++] = '\n';			
			InsertString(Output, Result->readMsg.msg.cmd.cmdData, TRUE, TRUE);	/* <cdata> */
        }
        break;
	case AT_SMS_TYPE_CBM:
		InsertInt(Output, Result->readMsg.msg.cbm.sn, TRUE);			/* <sn> */
		InsertInt(Output, Result->readMsg.msg.cbm.mid, TRUE);		/* <mid> */
		InsertInt(Output, Result->readMsg.msg.cbm.dcs, TRUE);		/* <dcs> */
		InsertInt(Output, Result->readMsg.msg.cbm.page&0xF, TRUE);			/* <page> */
		InsertInt(Output, ((Result->readMsg.msg.cbm.page>>4)&0xF), FALSE);			/* <pages> */
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->readMsg.msg.cbm.data, TRUE, FALSE);
        break;
	case AT_SMS_TYPE_PDU:
		if(Result->readMsg.msg.pdu.mask&AT_SMS_MASK_ALPHA)
			InsertString(Output, Result->readMsg.msg.pdu.alpha, FALSE, TRUE);	/* <alpha> */
		else
			Output->buff[Output->writeOffset++] = ',';
		InsertInt(Output, Result->readMsg.msg.pdu.length, FALSE);				/* <length> */
		Output->buff[Output->writeOffset++] = '\r';
		Output->buff[Output->writeOffset++] = '\n';
		InsertString(Output, Result->readMsg.msg.pdu.PDU, TRUE, FALSE);		/* <data> */
        break;
	default:
		break;
	}

        break;

    case AT_STORE_MESSAGE:
        /* Syntax: +CMGW: <index> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->index);
        break;
        
	case AT_SEND_STORED_MESSAGE:
        /* Syntax: TEXT Mode : +CMSS: <mr>[,<scts>] */
        /* Syntax: PDU Mode : +CMSS: <mr>[,<ackpdu>] */
    case AT_SEND_MESSAGE:
        /* Syntax: TEXT Mode : +CMGS: <mr>[,<scts>] */
        /* Syntax: PDU Mode : +CMGS: <mr>[,<ackpdu>] */
        if (avail < 5) {
            return AT_STATUS_NO_RESOURCES;
        }
       InsertInt(Output, Result->sendMsg_rsp.mr, FALSE);		/* <mr> */
       if(Result->sendMsg_rsp.mask&AT_SMS_MASK_SCTS && Result->sendMsg_rsp.scts)
	{
		Output->buff[Output->writeOffset++] = ',';
		InsertString(Output, Result->sendMsg_rsp.scts, FALSE, FALSE);
	}
	else if(Result->sendMsg_rsp.mask&AT_SMS_MASK_ACKPDU && Result->sendMsg_rsp.ackpdu)
	{
		Output->buff[Output->writeOffset++] = ',';
		InsertString(Output, Result->sendMsg_rsp.ackpdu, FALSE, FALSE);
	}
        break;

    case (AT_SMS_SHOW_TEXT_MODE|AT_READ):
         /* Syntax: +CSDH: <show> */
         if (avail < 1) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->show);
        break;

    case (AT_SMS_SHOW_TEXT_MODE|AT_TEST):
         /* Syntax: +CSDH: (list of supported <show>s) */
         if (avail < 5) {
            return AT_STATUS_NO_RESOURCES;
        }
        len = OS_StrLen(Result->supportedCBMMode);
        /* Output->buff[Output->writeOffset++] = '('; */
        OS_MemCopy(Output->buff + Output->writeOffset, (const U8 *)(Result->showModes), len);
        Output->writeOffset += len;
        /* Output->buff[Output->writeOffset++] = ')';  */
        break;

    case AT_SMS_FAILURE_RESULT:
        /* Syntax: +CMS ERROR: <err> */
        if (avail < 3) {
            return AT_STATUS_NO_RESOURCES;
        }
        Output->writeOffset += AtItoA(Output->buff + Output->writeOffset, Result->error.type);
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
        
/*--------------------------------------------------------------------------- 
 * AT_Decode_Sms
 * 
 *     Decodes AT commands received by the Gateway (ME).
 */
AtStatus ME_Decode_Sms(AtContext *Atc, U16 *Type, AtSmsCmd *Command, XaBufferDesc *Input)
{
    U8             *ptr = Input->buff + Input->readOffset;
    AtParameter     parms[AT_MAX_PARMS];
    U16         len;
    U16		p,i;
    char*       pStr;
    BOOL		bTextMode = FALSE;

    AtParseParameters(ptr, (U16)(Input->writeOffset - Input->readOffset), parms);

    switch (*Type) 
    {
    case AT_SELECT_SMS_SERVICE:
        /* Syntax: +CSMS=<service> */
        if (parms[0].len > 0)
            Command->service = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        break;

    case AT_PREFERRED_SMS_STORAGE:
        /* Syntax: +CPMS=<mem1>[,<mem2>[,<mem3>]] */
        Command->preferred.read= AT_SMS_STORE_UNKNOWN;
        Command->preferred.write= AT_SMS_STORE_UNKNOWN;
        Command->preferred.recv= AT_SMS_STORE_UNKNOWN;

        for(p = 0; p < 3;p++)
        {
            if (parms[p].len > 1) 
            {
                if (ptr[parms[p].offset] == '\"' && (parms[p].len > 2))
                    parms[p].offset++;  /* Strip quote from storage type */
		   else
		   	return AT_STATUS_NOT_SUPPORTED;

                for (i = 0; SmsStorageTypes[i]; i++) 
                {
                    if ((SmsStorageTypes[i][0] == ToUpper(ptr[parms[p].offset])) &&
                    (SmsStorageTypes[i][1] == ToUpper(ptr[parms[p].offset+1]))) 
                    {
                        /* Found the matching storage type. */
			    if(p == 0)
                            Command->preferred.read = (1 << i);
			    else if(p ==1)
			        Command->preferred.write = (1 << i);
			    else
			        Command->preferred.recv = (1 << i);
			    break;
                    }
                }
            }
            else
            {
                break;
            }
        }
        if(Command->preferred.read == AT_SMS_STORE_UNKNOWN)
            return AT_STATUS_NOT_SUPPORTED;
        break;

    case AT_SMS_MESSAGE_FORMAT:
        /* Syntax: +CMGF=[<mode>] */
	 Command->format = 0;	/* default value : 0 */
        if (parms[0].len > 0)
            Command->format = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
	 if( Command->format != 0 && Command->format != 1)
	 	return AT_STATUS_NOT_SUPPORTED;
        break;

    case AT_SMS_SERVICE_CENTER:
         /* Syntax: +CSCA=<sca>[,<tosca>] */
	  Command->SCA.mask = 0;
	  if (parms[0].len > 0)
            Command->SCA.sca = AtMakeString(ptr + parms[0].offset, parms[0].len);
	  else
	  	return AT_STATUS_NOT_SUPPORTED;
	  if (parms[1].len > 0)
        {
           Command->SCA.tosca = (U8)AtAtoI(ptr + parms[1].offset, parms[1].len);
	    Command->SCA.mask |= AT_SMS_MASK_TOSCA;
        }
        break;

    case AT_SET_TEXT_MODE_PARMS:
        /* Syntax: +CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]] */
	 Command->textModeParam.mask = 0;
	 if (parms[0].len > 0)
 	 {
	 	Command->textModeParam.fo = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
		Command->textModeParam.mask |= AT_SMS_MASK_FO;
 	 }
	 else
 	 {
	 	break;
 	 }
	 if(parms[1].len > 0)
 	 {
 	 	if(ptr[parms[1].offset] == '\"')
			Command->textModeParam.strVp = AtMakeString(ptr + parms[1].offset, parms[1].len);
		else
			Command->textModeParam.intVp = (U8)AtAtoI(ptr + parms[1].offset, parms[1].len);
		Command->textModeParam.mask |= AT_SMS_MASK_VP;
 	 }
	 else
 	 {
	 	break;
 	 }
	 if(parms[2].len > 0)
 	 {
		Command->textModeParam.pid = (U8)AtAtoI(ptr + parms[2].offset, parms[2].len);
		Command->textModeParam.mask |= AT_SMS_MASK_PID;
 	 }
	 else
 	 {
	 	break;
 	 }
	 if(parms[3].len > 0)
 	 {
		Command->textModeParam.dcs = (U8)AtAtoI(ptr + parms[3].offset, parms[3].len);
		Command->textModeParam.mask |= AT_SMS_MASK_DCS;
 	 }
        break;

    case AT_SET_CBM_TYPES:
        /* Syntax: +CSCB=[<mode>[,<mids>[,<dcss>]]] */
        Command->CBMTypes.mode = 0;
        Command->CBMTypes.mids = NULL;
        Command->CBMTypes.dcss = NULL;	
        if (parms[0].len > 0)
        {
            Command->CBMTypes.mode = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
            if(parms[1].len > 0)
            {
                Command->CBMTypes.mids = AtMakeString(ptr + parms[1].offset, parms[1].len);
                if(parms[2].len > 0)
                {
                    Command->CBMTypes.mids = AtMakeString(ptr + parms[2].offset, parms[2].len);
                }
            }
        }
        break;

    case AT_NEW_MESSAGE_INDICATION:
        /* Syntax: +CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]] */
        for(p = 0;p < 5;p++)
        {
            *((U8*)&(Command->newMsgIndSetting.mode)+p) = (parms[p].len > 0) ? (U8)AtAtoI(ptr + parms[p].offset, parms[p].len) : 0;
        }		
        break;

    case AT_LIST_MESSAGES:
        /* Syntax: +CMGL[=<stat>] */
        Command->status = AT_SMS_STATUS_UNREAD;	/* default value : 0 */
	  if(parms[0].len > 0)
            {
  	      if( ptr[parms[0].offset] == '\"' )
	  	   bTextMode = TRUE;
	      if(bTextMode)
                {
			  pStr = AtMakeString(ptr + parms[0].offset, parms[0].len);
			  len = OS_StrLen(pStr);
			  p = convertSMSStatus(pStr, (U8)len);
			  if (p < nSmsStatus)
				  Command->status = (U8)p;
			  else
			  	return AT_STATUS_INVALID_PARM;
                }
	      else
	      {
	      	    Command->status = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
            }
        }
	  else
  	  {
  	  	Command->status = AT_SMS_STATUS_UNKNOWN;
  	  }
        break;

    case AT_READ_MESSAGE:
        /* Syntax: +CMGR=<index> */
        if (parms[0].len > 0)
            Command->readMsgIndex = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);
        else
            return AT_STATUS_NOT_SUPPORTED;
        break;

    case AT_SEND_MESSAGE:
        /* Syntax(PDU mode): +CMGS=<length><CR>PDU is given<ctrl-Z/ESC> */
        /* Syntax(TEXT mode): +CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> */
        Command->sendMsg.mask = 0;
        if (parms[0].len > 0)
        {
        	if(ptr[parms[0].offset] == '\"')	/* TEXT mode */
      		{
			Command->sendMsg.destAddr = AtMakeString(ptr + parms[0].offset, parms[0].len);
			if(parms[1].len > 0)
			{
				Command->sendMsg.addrType = (U8)AtAtoI(ptr + parms[1].offset, parms[1].len);	
				Command->sendMsg.mask |= AT_SMS_MASK_TOADDR;
			}
		}
		else
		{
			Command->sendMsg.pduLen = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);
			Command->sendMsg.mask |= AT_SMS_MASK_LENGTH;
		}
		return AT_STATUS_CONTINUE;
        }
        else
        {
            return AT_STATUS_NOT_SUPPORTED;
        }
        break;

    case AT_STORE_MESSAGE:
        /* Syntax(PDU mode): +CMGW=<oa/da>[,<tooa/toda>,[,<stat>]]<CR>PDU is given<ctrl-Z/ESC> */
        /* Syntax(TEXT mode): +CMGW=<length>[,<stat>]<CR>PDU is given<ctrl-Z/ESC> */
	 i = 0;
	 Command->writeMsg.mask = 0;
        if (parms[i].len > 0)
        {
		if(ptr[parms[i].offset] == '\"')	/* TEXT mode */
		{
			bTextMode = TRUE;
			Command->writeMsg.mask |= AT_SMS_MASK_ADDR;
			Command->writeMsg.addr = AtMakeString(ptr + parms[i].offset, parms[i].len);
			i++;
			if(parms[i].len > 0)
            {
				Command->writeMsg.mask |= AT_SMS_MASK_TOADDR;
				Command->writeMsg.addrType = (U8)AtAtoI(ptr + parms[i].offset, parms[i].len);			
				i++;
			}
		}
		else
		{
			Command->writeMsg.mask |= AT_SMS_MASK_LENGTH;
			Command->writeMsg.pduLen = (U16)AtAtoI(ptr + parms[i].offset, parms[i].len);
			i++;
		}
            
            if (parms[i].len > 0)
            {
            		if(bTextMode)
      			{
	      			pStr = AtMakeString(ptr + parms[i].offset, parms[i].len);
				Command->writeMsg.stat = convertSMSStatus(pStr, (U8)(OS_StrLen(pStr)));
      			}
			else
			{
	                	Command->writeMsg.stat = (U8)AtAtoI(ptr + parms[i].offset, parms[i].len);
			}
            }
            else
            {
                /* Default value : 2 "STO UNSENT" */
                Command->writeMsg.stat = AT_SMS_STATUS_UNSENT;
            }
            /* The next packet will be unknown type and sent to ADP as RAW AT CMD data */
		return AT_STATUS_CONTINUE;
        }
        else
        {
		/* Default value : 2 "STO UNSENT" */
		Command->writeMsg.stat = AT_SMS_STATUS_UNSENT;
	 	 return AT_STATUS_CONTINUE;
        }
        break;
        
    case AT_SEND_STORED_MESSAGE:
        /* Syntax: +CMSS=<index>[,<da>[,<toda>]] */
        if (parms[0].len > 0)
        {
            Command->storedMsg.index = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);            
            if (parms[1].len > 0)
            {
				Command->storedMsg.mask |= AT_SMS_MASK_ADDR;
                Command->storedMsg.destAddr = AtMakeString(ptr + parms[1].offset, parms[1].len);
                if (parms[2].len > 0)
                {
					Command->storedMsg.mask |= AT_SMS_MASK_TOADDR;
                   Command->storedMsg.addrType = (U8)AtAtoI(ptr + parms[2].offset, parms[2].len);
                }
            }
        }
        break;
       
    case AT_DELETE_MESSAGE:
        /* Syntax: +CMGD=<index> */
        if(parms[0].len > 0)
            Command->delMsgIndex = (U16)AtAtoI(ptr + parms[0].offset, parms[0].len);
        else
            return AT_STATUS_NOT_SUPPORTED;
        break;

    case AT_SMS_SHOW_TEXT_MODE:
        /* Syntax: +CSDH=[<show>] */
        if(parms[0].len > 0)
            Command->show = (U8)AtAtoI(ptr + parms[0].offset, parms[0].len);
        else
            Command->show = 0; /* Default value */
        break;

    default:
        return AT_STATUS_NOT_SUPPORTED;
    }

    return AT_STATUS_OK;
}
#endif /* AT_ROLE_MOBILE == XA_ENABLED */

AtSmsStatus convertSMSStatus(const char *strStatus, U8 len)
{
	U8 p, i;
	for(p = 0; p < nSmsStatus; p++)
	{
		if(OS_StrLen(SmsStatus[p]) == len)
		{
			for(i = 0;i < len && SmsStatus[p][i] == ToUpper(strStatus[i]);i++)
			{
			}
			if(i == len)
				break;
		}
	}
	return (AtSmsStatus)p;
}

void InsertString(XaBufferDesc *buf, const char *str, BOOL bNoDoubleQuote, BOOL bComma)
{
	U16 len;

	Assert(str != NULL);
	len = OS_StrLen(str);
	/*if(bBrace) buf->buff[buf->writeOffset++] = '(';*/
	if(!bNoDoubleQuote) buf->buff[buf->writeOffset++] = '\"';
	OS_MemCopy(buf->buff + buf->writeOffset, (const U8 *)(str), len);
	buf->writeOffset += len;
	if(!bNoDoubleQuote) buf->buff[buf->writeOffset++] = '\"';
	/* if(bBrace) buf->buff[buf->writeOffset++] = ')'; */
	if(bComma) buf->buff[buf->writeOffset++] = ',';
}

void InsertInt(XaBufferDesc *buf, U32 data, BOOL bComma)
{
	buf->writeOffset += AtItoA(buf->buff + buf->writeOffset, data);
	if(bComma) buf->buff[buf->writeOffset++] = ',';
}

#endif /* AT_SMS == XA_ENABLED */

