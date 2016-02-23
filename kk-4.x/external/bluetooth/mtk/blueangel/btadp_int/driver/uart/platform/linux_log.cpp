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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * win_log.cpp (Ref. tst\test\sim_uart.cpp)
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file provides hardware depend interfaces for unit-test environment.
 *
 * Author:
 * -------
 * Spancer 	(mtk00264)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime:   May 16 2005 23:22:02  $
 * $Log:   //mtkvs01/vmdata/Maui_sw/archives/mcu/tst/test/sim_uart.cpp-arc  $
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)

//#ifdef UNIT_TEST

#define _SIM_UART_CPP

/* 
	Spancer: to aviod the redefinition of ASSERT and VERIFY
	The KAL and STDAFX.h had defined the same simples
*/
#undef ASSERT
#undef VERIFY


#include "stdio.h"
//#include "stdafx.h"
#include "cshare.h"
extern "C"
{
#include "bt_types.h"
#include "bt_adp_fs.h"
}

#ifdef FILE_OUTPUT
FILE *log_file;
#else
CShare	readShareMemory, writeShareMemory;
#endif

extern "C" void translateFilePath(const U8 *refPath, U8 *absPathBuf);
/*************************************************************************
* FUNCTION                                                            
*	UART_Open
*
* DESCRIPTION                                                           
*	This function implements the virtual UART_Open function for unit-test
*	environment.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*************************************************************************/
extern "C" void tst_UART_Open(unsigned char port)
{
#ifdef FILE_OUTPUT
	unsigned char path[256];
  	if( FALSE == btmtk_fs_check_dir_exist((const U8 *)"\\@btmtk\\debug") ){
		btmtk_fs_create_dir((const U8 *)"\\@btmtk");
		btmtk_fs_create_dir((const U8 *)"\\@btmtk\\debug");
	}
	translateFilePath((const U8 *)"\\@btmtk\\debug\\log.db", path);
	if((log_file = fopen((const char*)path, "w+b"))==NULL)
		printf("log.db open fail");
#else
	/* Transmit via shared memory */

	readShareMemory.CreateShare("CATCHER_UT", 64*1024);
	writeShareMemory.CreateShare("UT_CATCHER", 64*1024);

#endif
}



/*************************************************************************
* FUNCTION                                                            
*	UART_PutBytes
*
* DESCRIPTION                                                           
*	This function implements the virtual UART_PutBytes function for 
*	unit-test environment.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*************************************************************************/
extern "C" unsigned short	tst_UART_PutBytes(	unsigned char	port, 
								unsigned char	*Buffaddr, 
								unsigned short	Length,
								unsigned char  owner)
{
#ifdef FILE_OUTPUT
	if(log_file)
	{
	fwrite(Buffaddr, 
				sizeof(char), 
				Length,
				log_file);

	fflush(log_file);
	}
	return Length;
#else
	/* Put bytes via shared memory */
	return writeShareMemory.Write(Buffaddr, (const int &)Length);

#endif
}


/*************************************************************************
* FUNCTION                                                            
*	UART_GetBytes
*
* DESCRIPTION                                                           
*	This function implements the virtual UART_GetBytes function for 
*	unit-test environment.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*************************************************************************/
extern "C" unsigned short tst_UART_GetBytes(	unsigned char	port, 
								unsigned char	*Buffaddr, 
								unsigned short	Length, 
								unsigned char	*EscDect,
								unsigned char  owner)
{
#ifdef FILE_OUTPUT
	return Length;
#else
	/* Get data from the shared memory */
	return (unsigned short)readShareMemory.Read(Buffaddr, (const int)Length);
#endif
}



/*************************************************************************
* FUNCTION                                                            
*	UART_GetBytes
*
* DESCRIPTION                                                           
*	This function is used to release the shared memory resource.
*
* PARAMETERS
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*************************************************************************/
extern "C" void tst_stop_shared_memory(void)
{
#ifndef FILE_OUTPUT
	readShareMemory.Stop();
	writeShareMemory.Stop();
	readShareMemory.DestroyShare();
	writeShareMemory.DestroyShare();
#endif
}
#endif
//#endif
