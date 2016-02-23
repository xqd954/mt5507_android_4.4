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
*  permission of MediaTek Inc. (C) 2006
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
 * SdpDb.h
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions prototype of each profile to get SDP Data base by service.
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Jul 2 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Apr 23 2007 mtk01411
 * [MAUI_00384849] [Bluetooth SPP Client] SPP Client and number of virtual ports modification
 * 
 *
 * Feb 16 2007 mtk01239
 * [MAUI_00367691] [BT][HID] HID new feature check-in
 * 
 *
 * Jan 2 2007 mtk00560
 * [MAUI_00355463] [Bluetooth] to restructure bluetooth folders
 * created by clearfsimport
 *
 * Jul 31 2006 mtk01239
 * [MAUI_00214015] [BT] update BT code and add OPP, FTP SDP record
 * 
 *
 * Apr 3 2006 mtk00758
 * [MAUI_00184485] [New feature] Add task to simulate UART driver for unitest
 * 
 *
 * Mar 11 2006 MTK00303
 * [MAUI_00178684] [Bluetooth]ESI bluetooth protocol stack
 * add bluetooth files
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/

/***************************
 * Exported Function Prototypes 
 ****************************/

extern void SppSdpDB_InitRecordsArray(void);

/*************************************************************************
* FUNCTION                                                            
*  SDPDB_GetLocalServiceName
* DESCRIPTION                                                           
*   This function is used to get the service provider name and name length
* PARAMETERS
*  service_type  : The kind of service such as SC_DIALUP_NETWORKING,
*                               SC_FAX, SC_LAN_ACCESS_PPP, SC_SERIAL_PORT
*  service_name  : The pointer points to the service record name.
* RETURNS
*  U8 : The length of service name
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 SDPDB_GetLocalServiceName(U16 service_type, const U8 **service_name);

/*************************************************************************
* FUNCTION                                                            
*  SDPDB_GetLocalServiceAttribute
* DESCRIPTION                                                           
*   This function is used to get the service provider name and name length
* PARAMETERS
*  service_type  : The kind of service such as SC_DIALUP_NETWORKING,
*                               SC_FAX, SC_LAN_ACCESS_PPP, SC_SERIAL_PORT
*  service_name  : The pointer points to the service record name.
* RETURNS
*  U8 : The length of service name
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 SDPDB_GetLocalServiceAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn);

extern U8 *SppSdpDB_GetServiceName(int i);

/*************************************************************************
* FUNCTION                                                            
*  SppSdpDB_GetAttribute
* DESCRIPTION                                                           
*   This function is used to get the service record attributes pointer and attribute number of 
*   SPP, LAP, DUN, FAX Service
* PARAMETERS
*  service_type  : The kind of service such as SC_DIALUP_NETWORKING,
*                               SC_FAX, SC_LAN_ACCESS_PPP, SC_SERIAL_PORT
*  SdpAttribute   : The pointer points to the reutrn record attribute.
*      *srv_chn          : the RFCOMM service channel of the service 
* RETURNS
*  U8 : The number of return attribute number
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 SppSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn);

extern U8 SppSdpDB_SetNGetCustomServiceName(int i, U8 **service_name, char *svcCustomName);
extern U8 SppSdpDB_SetNGetCustomAttribute(I16 i, SdpAttribute **attribute, U8 *srv_chn, U16 customUUID);
	
/*************************************************************************
* FUNCTION                                                            
*  HfpagSdpDB_GetAttribute
* DESCRIPTION                                                           
*   This function is used to get the Handsfree Audio Gateway service record attributes pointer,
*   attribute number and the service channel in the service record
* PARAMETERS
*  service_type  : The kind of service such as SC_HANDSFREE_AUDIO_GATEWAY.
*  SdpAttribute   : The pointer points to the reutrn record attribute.
*      *srv_chn          : the RFCOMM service channel of the service 
* RETURNS
*  U8 : The number of return attribute number
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 HfpagSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn);

/*************************************************************************
* FUNCTION                                                            
*  HspSdpDB_GetAttribute
* DESCRIPTION                                                           
*   This function is used to get the Headset Audio Gateway service record attributes pointer,
*   attribute number and the service channel in the service record
* PARAMETERS
*  service_type  : The kind of service such as SC_HEADSET_AUDIO_GATEWAY.
*  SdpAttribute   : The pointer points to the reutrn record attribute.
*      *srv_chn          : the RFCOMM service channel of the service 
* RETURNS
*  U8 : The number of return attribute number
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 HspSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *srv_chn);

/*************************************************************************
* FUNCTION                                                            
*  A2dpSdpDB_GetAttribute
* DESCRIPTION                                                           
*   This function is used to get the service record attributes pointer and attribute number of A2DP
* PARAMETERS
*  service_type  : The kind of service such as A2DP SOURCE and SINK.
*  SdpAttribute   : The pointer points to the reutrn record attribute.
* RETURNS
*  U8 : The number of return attribute number
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 A2dpSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute);

/*************************************************************************
* FUNCTION                                                            
*  AvrcpSdpDB_GetAttribute
* DESCRIPTION                                                           
*   This function is used to get the service record attributes pointer and attribute number of AVRCP
* PARAMETERS
*  service_type  : The kind of service such as AVRCP Controller or AVRCP Target.
*  SdpAttribute   : The pointer points to the reutrn record attribute.
* RETURNS
*  U8 : The number of return attribute number
* GLOBALS AFFECTED
*   
*************************************************************************/
extern U8 AvrcpSdpDB_GetAttribute(U16 service_type, SdpAttribute **attribute, U8 *version);

extern U8 ObexSdpDB_GetServChn(U32 service_type, U8 instance);
extern U8 ObexSdpDB_Registration(U32 service_type);
U8 ObexSdpDB_RegistrationEx(U32 service_type, U8 tp_type);
extern U8 ObexSdpDB_DeRegistration(U32 service_type);
extern U8 HidDeviceSdpDB_Registration(void);
extern U8 HidDeviceSdpDB_DeRegistration(void);
extern void HidDeviceSdpDB_ReplaceRecord(U16 uuid, U8 *attribute, U8 size);
extern U8 PanSdpDB_AddRecord(U16 service_type);
extern U8 PanSdpDB_RemoveRecord(U16 service_type);

extern U8 PrxSdpDB_AddRecord(U8 index, U8 capability);
extern U8 PrxSdpDB_RemoveRecord(U8 index, U8 capability);


BtStatus HdpSdpDB_DeRegistration(void);
BtStatus HdpSdpDB_AddFeature(U16 mdepId, U8 role, U16 dataType, char *descriotion);
BtStatus HdpSdpDB_RemoveFeature(U8 id);
BtStatus HdpSdpDB_ReplacePsm(U16 controlPsm, U16 dataPsm);
