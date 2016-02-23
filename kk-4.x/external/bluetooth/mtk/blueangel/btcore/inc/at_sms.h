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

#ifndef __AT_SMS_H
#define __AT_SMS_H
/****************************************************************************
 *
 * File:
 *     $Workfile:at_sms.h$ for iAnywhere Blue SDK, Version 2.1.1
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
#include "atconfig.h"

#include "bluetooth_hfg_common.h"

#if AT_SMS == XA_ENABLED

/*---------------------------------------------------------------------------
 * AT SMS Command Processor API layer
 *
 * The AT Command Processor API provides functions for both parsing and 
 * encoding AT commands and responses.  It is modularized into components
 * that support Headset, Hands-free, and DUN profiles.  It also provides
 * the ability to parse and encode Phonebook commands and responses, based
 * on the GSM specifications.  Support for SMS messaging commands will
 * be added in the future.
 *
 * The SMS Command Processor module provides the commands necessary
 * for handilng SMS messages on the Audio Gateway.  This module is currently
 * not complete, but will be added in a future release.
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
/** This command selects the type messaging service. It returns the types of
 *  messages supported by the ME for mobile terminated messages, mobile 
 *  originated messages, and broadcast type messages.
 */
 /* +CSMS */
#define AT_SELECT_SMS_SERVICE       0x0400

/** This command selects memory storages to be used for reading, writing, 
 *  etc. The read command returns information about each memory storage.
 */
 /* +CPMS */
#define AT_PREFERRED_SMS_STORAGE    0x0401

/** +CMGF */
#define AT_SMS_MESSAGE_FORMAT       0x0402

/** +CSCA */
#define AT_SMS_SERVICE_CENTER       0x0403

/** +CSMP */
#define AT_SET_TEXT_MODE_PARMS      0x0404

/** +CSCB */
#define AT_SET_CBM_TYPES            0x0405

/** +CNMI */
#define AT_NEW_MESSAGE_INDICATION   0x0406

/** +CMTI */
#define AT_SMS_DELIVER_INDICATION   0x0407

/** +CBMI */
#define AT_CELL_BROADCAST_MSG_ARRIVE 0x0408

/** +CDS */
#define AT_SMS_STATUS_REPORT        0x0409

/** +CMGL */
#define AT_LIST_MESSAGES            0x040A

/** +CMGR */
#define AT_READ_MESSAGE             0x040B

/** +CMGS */
#define AT_SEND_MESSAGE             0x040C

/** +CMSS */
#define AT_SEND_STORED_MESSAGE      0x040D

/** +CMGW */
#define AT_STORE_MESSAGE            0x040E

/** +CMGD */
#define AT_DELETE_MESSAGE           0x040F

/** +CSDH */
#define AT_SMS_SHOW_TEXT_MODE 	0x0410

/** +CMS ERROR */
#define AT_SMS_FAILURE_RESULT		0x0411

/* End of AtCommand */

/*--------------------------------------------------------------------------
 * AtError type
 *
 *     Error types used with AT_SMS_ERROR result.
 */
#ifndef __LINUX_SUPPRESS_ERROR__
typedef U8 AtError;
#endif

#define ATCMS_ME_FAILURE                    300 
#define ATCMS_SMS_SERVICE_OF_ME_RESERVED    301 
#define ATCMS_OPERATION_NOT_ALLOWED         302 
#define ATCMS_OPERATION_NOT_SUPPORTED       303 
#define ATCMS_INVALID_PDU_MODE_PARAMETER    304 
#define ATCMS_INVALID_TEXT_MODE_PARAMETER   305 
#define ATCMS_SIM_NOT_INSERTED              310 
#define ATCMS_SIM_PIN_REQUIRED              311 
#define ATCMS_PH_SIM_PIN_REQUIRED           312 
#define ATCMS_SIM_FAILURE                   313 
#define ATCMS_SIM_BUSY                      314 
#define ATCMS_SIM_WRONG                     315 
#define ATCMS_SIM_PUK_REQUIRED              316 
#define ATCMS_SIM_PIN2_REQUIRED             317
#define ATCMS_SIM_PUK2_REQUIRED             318
#define ATCMS_MEMORY_FAILURE                320 
#define ATCMS_INVALID_MEMORY_INDEX          321 
#define ATCMS_MEMORY_FULL                   322
#define ATCMS_SMSC_ADDRESS_UNKNOWN          330
#define ATCMS_NO_NETWORK_SERVICE            331
#define ATCMS_NETWORK_TIMEOUT               332
#define ATCMS_NO_CNMA_ACK_EXPECTED          340 
#define ATCMS_ERROR                         500

/* End of AtError */

/*---------------------------------------------------------------------------
 * AtSmsStorage
 *
 *     Memory storage location definitions.
 */
typedef U8 AtSmsStorage;

/** Broadcast Message storage. */
#define AT_SMS_STORE_CBM            	HFG_SMS_STORAGE_BM

/** ME message storage. */
#define AT_SMS_STORE_ME             	HFG_SMS_STORAGE_ME

/** Any of the storages associated with ME. */
#define AT_SMS_STORE_ANY            	HFG_SMS_STORAGE_MT

/** SIM message storage. */
#define AT_SMS_STORE_SIM            	HFG_SMS_STORAGE_SM

/** Status report storage. */
#define AT_SMS_STORE_STATUS         	HFG_SMS_STORAGE_TA

/** Indicate the storage is not indicated  */
#define AT_SMS_STORE_UNKNOWN	HFG_SMS_STORAGE_UNKNOWN

/* End of AtSmsStorage */

/*---------------------------------------------------------------------------
 * AtSmsStorage
 *
 *     Memory storage location definitions.
 */
typedef U8 AtSmsStatus;

/** received unread message (i.e. new message). */
#define AT_SMS_STATUS_UNREAD    	HFG_SMS_STAT_REC_UNREAD

/** received read message. */
#define AT_SMS_STATUS_READ        	HFG_SMS_STAT_REC_READ

/** stored unsent message (only applicable to SMs). */
#define AT_SMS_STATUS_UNSENT    	HFG_SMS_STAT_STO_UNSENT

/** stored sent message (only applicable to SMs). */
#define AT_SMS_STATUS_SENT          	HFG_SMS_STAT_STO_SENT

/** all messages (only applicable to +CMGL command). */
#define AT_SMS_STATUS_ALL           	HFG_SMS_STAT_ALL

#define NUM_OF_AT_SMS_STATUS		NUM_OF_HFG_SMS_STAT

#define AT_SMS_STATUS_UNKNOWN	HFG_SMS_STAT_UNKNOWN

/* End of AtSmsStatus */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/
typedef U8 AtSMSType;
#define AT_SMS_TYPE_DELIVER		HFG_SMS_TYPE_DELIVER
#define AT_SMS_TYPE_SUBMIT		HFG_SMS_TYPE_SUBMIT
#define AT_SMS_TYPE_STATUS		HFG_SMS_TYPE_STATUS
#define AT_SMS_TYPE_COMMAND		HFG_SMS_TYPE_COMMAND
#define AT_SMS_TYPE_CBM			HFG_SMS_TYPE_CBM
#define AT_SMS_TYPE_PDU			HFG_SMS_TYPE_PDU
#define NUM_OF_AT_SMS_TYPE		NUM_OF_HFG_SMS_TYPE


// Optional Parameter for SMS transmission
#define AT_SMS_MASK_ALPHA		HFG_SMS_MASK_ALPHA	// string type alphanumeric representation of address (related +CSCS)
#define AT_SMS_MASK_SCTS		HFG_SMS_MASK_SCTS	// Service Centre Time Stamp in time-string format
#define AT_SMS_MASK_ADDR		HFG_SMS_MASK_ADDR	// address (destination address in +CGMR SMS-COMMAND
#define AT_SMS_MASK_TOADDR	HFG_SMS_MASK_TOADDR	// type of address (originating address and destination address, 129, 145)
#define AT_SMS_MASK_LENGTH	HFG_SMS_MASK_LENGTH	// length of the message body <data> in integer or actual TP data unit in octets
#define AT_SMS_MASK_RA		HFG_SMS_MASK_RA	// recipient address in string format
#define AT_SMS_MASK_TORA		HFG_SMS_MASK_TORA	// type of ra
#define AT_SMS_MASK_FO		HFG_SMS_MASK_FO	// first octect of SMS-DELIVER, SMS-SUBMIT, SMS-STATUS-REPORT, SMS-COMMAND
#define AT_SMS_MASK_PID		HFG_SMS_MASK_PID	// Protocol Identifier in integer format
#define AT_SMS_MASK_DCS		HFG_SMS_MASK_DCS	// SMS Data Coding Scheme
#define AT_SMS_MASK_SCA		HFG_SMS_MASK_SCA	// RP SC address in string format
#define AT_SMS_MASK_TOSCA		HFG_SMS_MASK_TOSCA	// type of sca in integer format
#define AT_SMS_MASK_VP		HFG_SMS_MASK_VP	// Validity Period (integer or time-string format)
#define AT_SMS_MASK_MN		HFG_SMS_MASK_MN	// Message Number in integer format
#define AT_SMS_MASK_CDATA		HFG_SMS_MASK_CDATA	// Command data in text mode response
#define AT_SMS_MASK_ACKPDU	HFG_SMS_MASK_ACKPDU	// User Data element of RP-ACK PDU (+CMGS in PDU mode)


/*---------------------------------------------------------------------------
 * 
 *
 */
typedef union _AtSmsCmd 
{
	/* AT_SELECT_SMS_SERVICE: AT+CSMS=<service>							*/
	HfgSMSService_cmd 		service;

    	/* AT_PREFERRED_SMS_STORAGE: AT+CPMS=<mem1>[,<mem2>[,<mem3>]]	*/
	HfgSMSPrefStorage_cmd		preferred;

	/* AT_SMS_MESSAGE_FORMAT: AT+CMGF=<mode>							*/
	HfgSMSFormat_cmd		format;		/* Message format 0=PDU mode, 1=text mode. */
        
	/* AT_SMS_SERVICE_CENTER: AT+CSCA=<sca>[,<tosca>]					*/
	HfgSMSSrviceCentre_cmd	SCA;

	/* AT_SET_TEXT_MODE_PARMS: AT+CSMP=[<fo>[,<vp>[,<pid>[,<dcs>]]]]		*/
	HfgSMSTextModeParam_cmd	textModeParam;

    	/* AT_SMS_SHOW_TEXT_MODE: AT+CSDH=[<show>]						*/
    	HfgSMSShowParams_cmd	show;		/* If this field is missed, set to default value 0. */

	/* AT_NEW_MESSAGE_INDICATION: AT+CNMI=[<mode>[,<mt>[,<bm>[,<ds>[,<bfr>]]]]]		*/
	HfgSMSIndSetting_cmd 		newMsgIndSetting;

	/* AT_LIST_MESSAGES: AT+CMGL[=<stat>]								*/
	HfgSMSList_cmd			status;

	/* AT_READ_MESSAGE: AT+CMGR=<index>								*/
	HfgSMSRead_cmd			readMsgIndex;

	/* AT_SEND_MESSAGE: 												*/
	/* TEXT MODE : AT+CMGS=<da>[,<toda>]<CR>text is entered<ctrl-Z/ESC> 	*/
	/* PDU MODE : AT+CMGS=<length><CR>PDU is given<ctrl-Z/ESC> 			*/
	HfgSMSSend_cmd			sendMsg;

	/* AT_SEND_STORED_MESSAGE:	AT+CMSS=<index>[,<da>[,<toda>]]			*/
	HfgSMSSendStored_cmd		storedMsg;

	/* AT_STORE_MESSAGE:															*/
	/* TEXT MODE : AT+CMGW=<oa/da>[,<toda/toda>[,<stat>]]<CR>text is entered<ctrl-Z/ESC>	*/
	/* PDU MODE : AT+CMGW=<length>[,<stat>]<CR>PDU is given<ctrl-Z/ESC> 				*/
	HfgSMSWrite_cmd			writeMsg;

	/* AT_DELETE_MESSAGE: AT+CMGD=<index>								*/
	HfgSMSDelete_cmd		delMsgIndex;

    struct {
        U8 mode;
        const char *mids;
        const char *dcss;
    }CBMTypes;
} AtSmsCmd;

/*---------------------------------------------------------------------------
 * AtSmsResult
 *
 */
#if 0
/* ****************************************
*	+CMGL
******************************************/
typedef struct _AtCMGLDeliver
{
	U16				mask;			// bitwise value for optional values (alpha, scts, tooa/toda, length)
	const char		*addr; 			// originated address, destination address (oa/da)
	U8				addrType;		// type of address (tooa/toda)
	const char		*alpha; 			// string type of address (optional, alpha, ex.UTF-8)
	const char		*SCTimeStamp;	// time stamp "yy/MM/dd,hh:mm:ss+(-)zz" (optional, scts)
	const char		*data;			// data
	U16				length;			// length of data(optional)
}AtCMGLDeliver, AtCMGLSubmit;

typedef struct _AtCMGLStatus
{
	U16			mask;				// bitwise value for optional values (ra, tora)
	U8			fo;					// first octet (default 2)	
	U8			mr;					// Message Reference (mr)
	const char	*ra;					// Recipient  Address (ra)
	U8			addrType;			// type of address (optional, tora)	
	const char	*SCTimeStamp;		//"yy/MM/dd,hh:mm:ss+zz" (scts)	
	const char	*dt;					// Discahrge Time  "yy/MM/dd,hh:mm:ss+(-)zz"(dt)
	U8			st;					// Status of previously sent message	(st)
}AtCMGLStatus;

typedef struct _AtCMGLCommand
{
	U8				fo;				// first octet (default 2)
	U8				ct;				// Command type in integer format (default 0)
}AtCMGLCommand;

typedef struct _AtCMGLCbm
{
	U16			sn;					// serial number for particular message (sn)
	U16			mid;					// message identifier (mid)
	U8			page;				// page parameter. 0-3 bit:total number of pages, 4-7 bit: page number
	const char	*data;				// data
	U16			length;				// length of data (not transmitted)
	U8			dcs;					// data coding scheme (only for +CMGR)
}AtCMGLCbm;

typedef struct _AtCMGLPdu
{
	U16			mask;		// bitwise value for optional values (alpha)
	const char	*alpha;		// string type of address(optional, alpha, ex.UTF-8)
	const char	*PDU;		// PDU
	U16			length;		// data length of PDU
}AtCMGLPdu;

typedef struct
{
	AtSMSType		type;
    U16 index;
	AtSmsStatus		stat;
	union
	{
		AtCMGLDeliver	deliver;
		AtCMGLSubmit 	submit;		
		AtCMGLStatus 	status;
		AtCMGLCommand	cmd;
		AtCMGLCbm		cbm;
		AtCMGLPdu		pdu;
	}msg;
}AtCMGL;

/* ****************************************
*	+CMGR
******************************************/
// SMS-DELIVER for +CMGR
typedef struct
{
	U16				mask;			// bitwise value for optional values (alpha, tooa, fo, pid, dcs, sca, tosca, length)
	const char		*addr; 			// originated address
	U8				addrType;		// type of address(optional, tooa)
	const char		*alpha; 			// string type of address(optional, alpha)
	const char		*SCTimeStamp;	// "yy/MM/dd,hh:mm:ss+zz" (scts)
	U8				fo;				// first octet (default 17, optional, fo)
	U8				pid;				// Protocol Identifer (default 0, optional)
	U8				dcs;				// data coding scheme (optional)
	const char		*SCAddr; 		// Service Centre address (optional, sca)
	U8				SCAddrType;		// type of Service Centre address(optional, tosca) 
	const char		*data;			// data
	U16				length;			// length of data (optional)	
}AtCMGRDeliver;

// SMS-SUBMIT  for +CMGR
typedef struct
{
	U16				mask;			// bitwise value for optional values (alpha, toda, fo, pid, dcs, vp, sca, tosca, length)
	const char		*addr; 			// destination address (da)
	U8				addrType;		// type of address(optional, toda)
	const char		*alpha;			// string type of address(optional, alpha)
	U8				fo;				// first octet (default 17, optional)	
	U8				pid;				// Protocol Identifer (default 0, optional)
	U8				dcs;				// data coding scheme (optional)
	U8				intVp;			// Validity period in integer format(default 167, optional)
	const char		*strVp;			// Validity period in time stamp format (yy/MM/dd,hh:mm:ss+-zz)
	const char		*SCAddr; 		// Service Centre address (optional, sca)
	U8				SCAddrType;		// type of Service Centre address(optional, tosca) 
	const char		*data;			// data
	U16				length;			// length of data (optional)	
}AtCMGRSubmit;

// SMS-STATUS-REPORT  for +CMGR
typedef AtCMGLStatus AtCMGRStatus;

// SMS-COMMAND  for +CMGR
typedef struct
{
	U16				mask;		// bitwise value for optional values (pid, mn, da, toda, length, cdata)
	U8				fo;			// first octet (default 2)		
	U8				ct;			// Command type
	U8				pid;			// Protocol Identifer (default 0, optional)
	U8				mn;			// Message Number previously MR value (optional, mn) 
	const char		*destAddr; 	// destnation address (otional, da)
	U8				addrType;	// type of address(optional, toda)	
	const char		*cmdData;	// command data(optional)
	U16				length;		// length of data (optional)		
}AtCMGRCommand;

// CBM  for +CMGR
typedef AtCMGLCbm AtCMGRCbm;
        
// PDU  for +CMGR
typedef AtCMGLPdu AtCMGRPdu;

// Read Message +CMGR
typedef struct
{
	AtSMSType		type;		// type of SMs
	AtSmsStatus		stat;		// status
	union
	{
		AtCMGRDeliver	deliver;
		AtCMGRSubmit	submit;
		AtCMGRStatus		status;
		AtCMGRCommand	cmd;
		AtCMGRCbm		cbm;
		AtCMGRPdu		pdu;
	}msg;
}AtCMGR;

/* AT_SEND_MESSAGE		 										*/
/* TEXT MODE : +CMGS:<mr>[,<scts>] 								*/
/* PDU MODE : +CMGS:<mr>[,<ackpdu>] 								*/
typedef struct
{
	U16			mask;
	U8			mr;
	const char	*scts;		// optional
	const char	*ackpdu;
}AtCMGS;
#endif

        
typedef union _AtSmsResult {
	/* AT_SELECT_SMS_SERVICE|AT_TEST		*/
	/* +CSMS:(list of supported <service>s) 		*/
	const char         			*supportedServices;

	/* AT_SELECT_SMS_SERVICE|AT_READ		*/
	/* +CSMS:<service>,<mt>,<mo>,<bm>		*/
	HfgSMSService_read		service;

	/* AT_SELECT_SMS_SERVICE 				*/
	/* +CSMS:<mt>,<mo>,<bm> 				*/
	HfgSMSService_result		service_result;
        
	/* AT_PREFERRED_SMS_STORAGE|AT_TEST								*/
	/* +CPMS:(list of supported <mem1>s),(..),(list of supported <mem3>s)		*/
	HfgSMSPrefStorage_test		preferred_test;

	/* AT_PREFERRED_SMS_STORAGE|AT_READ								*/
	/* +CPMS:<mem1>,<used1>,<total1>...,<mem3>,<used3>,<total3>			*/
	HfgSMSPrefStorage_read	preferred_read;

	/* AT_PREFERRED_SMS_STORAGE										*/
	/* +CPMS:<used1>,<total1>...,<used3>,<total3>							*/
	HfgSMSPrefStorage_result	preferred_result;

	/* AT_SMS_MESSAGE_FORMAT|AT_TEST	*/
	/* +CMGF:(list of supported <mode>s)	*/
    const char *supportedMsgFormat;

	/* AT_SMS_MESSAGE_FORMAT|AT_READ	*/
	/* +CMGF:<mode>					*/
	HfgSMSFormat_read 		msgFormat;

	/* AT_SMS_SERVICE_CENTER|AT_READ	*/
	/* +CSCA:<sca>[,<tosca>]			*/
	HfgSMSSrviceCentre_read	SCA;

	/* AT_SET_TEXT_MODE_PARMS|AT_READ */
	/* Syntax: +CSMP: <fo>,<vp>,<pid>,<dcs> */
	HfgSMSTextModeParam_read	textModeParams;

	/* AT_SMS_SHOW_TEXT_MODE|AT_TEST	*/
	/* +CSDH: (list of supported <show>s) */
	const char 				*showModes;

	/* AT_SMS_SHOW_TEXT_MODE|AT_READ	*/
	/* +CSDH:<show>					*/
	HfgSMSShowParams_read 	show;

	/* AT_NEW_MESSAGE_INDICATION|AT_TEST	*/
	/* +CNMI: (list of supported <mode>s)...(list of supported <bfr>s) */
	const char				*newMsgInd_test;

	/* AT_NEW_MESSAGE_INDICATION|AT_READ								*/
	/* +CNMI:<mode>,<mt>,<bm>,<ds>,<bfr>								*/
	HfgSMSIndSetting_read		newMsgIndSetting;

	/* AT_SMS_DELIVER_INDICATION										*/
	/* +CMTI:<mem>,<index>												*/
	HfgSMSNewMsgInd			newMsgIndication;

	/* AT_LIST_MESSAGES|AT_TEST 		*/
    /* +CMGL: (list of supported <stat>s) */
    const char *supportedStatus;

	/* AT_LIST_MESSAGES													*/
	/* TEXT MODE & SMS-SUBMITs and/or SMS-DELIVERs : 						*/
	/* 	+CMGL:<index>,<stat>,<oa/da>,[<alpha>],[<scts>][,<tooa/toda>,		*/
	/*               <length>]<CR><LF><data> 									*/
	/* TEXT MODE & SMS-STATUS-REPORTs : 									*/
	/*	+CMGL:<index>,<stat>,<sn>,<mid>,<page>,<pages><CR><LF><data> 	*/
	/* TEXT MODE & SMS-COMMANDs : 										*/
	/*	+CMGL:<index>,<stat>,<fo>,<ct><CR><LF><data> 					*/
	/* TEXT MODE & CMB storage : 											*/
	/*	+CMGL:<index>,<stat>,<sn>,<mid>,<page>,<pages><CR><LF><data> 	*/
	/* PDU MODE : 														*/
	/*	+CMGL:<index>,<stat>,[alpha],<length><CR><LF><pdu> 				*/
	HfgSMSList_result			listMsg;	

	/* AT_READ_MESSAGE													*/
	/* TEXT MODE & SMS-DELIVERs : 										*/
	/*	+CMGR:<stat>,<oa>,[<alpha>],[<scts>][,<tooa>,<fo>,<pid>,<dcs>,		*/
	/*	            <sca>,<tosca>,<length>]<CR><LF><data> 					*/
	/* TEXT MODE & SMS-SUBMIT : 											*/
	/*	+CMGR:<stat>,<da>,[<alpha>][,<toda>,<fo>,<pid>,<dcs>,[<vp>],		*/
	/*		     <sca>,<tosca>,<length>]<CR><LF><data> 					*/
	/* TEXT MODE & SMS-STATUS-REPORT : 									*/
	/*	+CMGR:<stat>,<fo>,<mr>,[<ra>],[<tora>],<scts>,<dt>,<st>			*/
	/*               <CR><LF><data> 											*/
	/* TEXT MODE & SMS-COMMANDs : 										*/
	/*	+CMGR:<stat>,<fo>,<ct>[,<pid>,[<mn>],[<da>],[<toda>],<length>		*/
	/*		     <CR><LF><cdata>] 										*/
	/* TEXT MODE & CMB storage : 											*/
	/*	+CMGR:<stat>,<sn>,<mid>,<dcs>,<page>,<pages><CR><LF><data> 	*/
	/* PDU MODE : 														*/
	/*	+CMGR:<stat>,[<alpha>],<length><CR><LF><pdu> 					*/
	HfgSMSRead_result			readMsg;
    
	/* AT_SEND_MESSAGE													*/
	/* TEXT MODE : +CMGS:<mr>[,<scts>] 									*/
	/* PDU MODE : +CMGS:<mr>[,<ackpdu>] 									*/
	HfgSMSSend_result			sendMsg_rsp;

	/* AT_SEND_STORED_MESSAGE											*/
	/* TEXT MODE : +CMSS:<mr>[,<scts>] 									*/
	/* PDU MODE : +CMSS:<mr>[,<ackpdu>] 									*/
	HfgSMSSendStored_result	sendStoredMsg_rsp;

	/* AT_STORE_MESSAGE													*/
    /* +CMGW: <index> */
	HfgSMSWrite_result		index;

	/* AT_SET_CBM_TYPES|AT_READ */
	/* +CSCB: <mode>,<mids>,<dcss> */
	struct {
	    U8 mode;
	    const char *mids;
	    const char *dcss;
	}CBMTypes;

	/* AT_SET_CBM_TYPES|AT_TEST */
	/* +CSCB: (list of supported <mode>s) */
	const char *supportedCBMMode;

	/* +CMTI: <mem><index> */
	#if 0
	struct {
	    AtSmsStorage type;
	    U16 index;
	}msgInd;
	#endif
    /* AT_SMS_FAILURE_RESULT */
    struct {
        AtError         type;
    } error;
} AtSmsResult;

#else /* AT_SMS == XA_ENABLED */

/* Stub structures to keep #defines out of AtResults/AtCommands structures. */
typedef U8 AtSmsCmd;
typedef U8 AtSmsResult;

#endif /* AT_SMS == XA_ENABLED */
#endif /* __AT_SMS_H */
