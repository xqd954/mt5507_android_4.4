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
 * bt_hfg_struct.h
 *
 * Project:
 * --------
 *   Maui
 *
 * Description:
 * ------------
 *   struct of local parameter for hfg adp sap
 *
 * Author:
 * -------
 * Elvis Lin
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 08 08 2013 liang.cheng
 * [ALPS00931922] [MR2][BT]Source code check in
 * Easy Migration default profile check in.
 *
 * 08 08 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 08 07 2013 liang.cheng
 * [ALPS00932896] [DailyBuildError][CodePromotion] ALPS.JB2.MR2.DEV mt6582_phone_v1_2, eng
 * Easy Migration default profile Check in.
 *
 * 09 22 2010 sh.lai
 * [ALPS00003522] [BLUETOOTH] Android 2.2 BLUETOOTH porting
 * Integrate bluetooth code from //ALPS_SW_PERSONAL/sh.lai/10YW1040OF_CB/ into //ALPS_SW/TRUNK/ALPS/.
 *
 * 09 10 2010 sh.lai
 * NULL
 * 1. Fix CR ALPS00125222 : [MTK BT]when dial out a invalidable number via Bluetooth headset,phone audio connot be connect automatically
 * Cause : Original HFG code that will create SCO when call state becomes alerting. In HFG SPEC, it create SCO when call state becomes DIALING.
 * Solution : Create SCO when call state becomes DIALING.
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __BT_HFG_STRUCT_H__
#define __BT_HFG_STRUCT_H__

#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include <windows.h>
#endif  /* #if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK) */
#include "bt_struct.h"
#include "bluetooth_hfg_common.h"
#include "bluetooth_hfg_struct.h"
#include "bt_ext_timer.h"

/*******************************************************************************
*   Due to current there is no BT common headser files, I define some common BT
*   used types here
********************************************************************************/
typedef I8 BtStatus;

#define BT_STATUS_SUCCESS 0
#define BT_STATUS_FAILED  1
#define BT_STATUS_PENDING 2
#define BT_STATUS_IN_PROGRESS 19
/*******************************************************************************
*   End of BT common definition
********************************************************************************/

/***************************************************************************** 
* Definations
*****************************************************************************/

/***************************************************************************** 
* Typedef 
*****************************************************************************/

/*---------------------------------------------------------------------------
 * HfgCallbackParms structure
 *
 * This structure is sent to the application's callback to notify it of
 * any state changes.
 */
/* status        : Status of the callback event             */ 
/* errCode       : Error code (reason) on disconnect events */
/* respHold      : Only valid if HFG_USE_RESP_HOLD is set to XA_ENABLED.*/
/* pbStorage     : HFG_EVENT_SELECT_PHONEBOOK */
/* pbRead        : HFG_EVENT_READ_PBENTRY */
/* pbFind        : HFG_EVENT_FIND_PBENTRY */
/* pbWrite       : HFG_EVENT_WRITE_PBENTRY */ 
/* service       : HFG_EVENT_SELECT_SMS_SERVICE */
/* prefStorage   : HFG_EVENT_QUERY_SELECTED_PREF_MSG_STORAGE    */
/* format        : HFG_EVENT_SELECT_MSG_FORMAT(0: PDU mode, 1: Text mode) */
/* serviceCentre : HFG_EVENT_SET_SERVICE_CENTRE */
/* textParams    : HFG_EVENT_SET_TEXT_MODE_PARAMS */
/* show          : HFG_EVENT_SET_SHOW_PARAMS */
/* newMsgInd     : HFG_EVENT_SET_NEW_MSG_INDICATION */
/* stat          : HFG_EVENT_LIST_MSG */
/* readMsgIndex  : HFG_EVENT_READ_MSG */
/* delMsgIndex   : HFG_EVENT_DELETE_MSG */
/* sendMsg       : HFG_EVENT_SEND_MSG */
/* storedMsg     : HFG_EVENT_SEND_STORED_MSG */
/* writeMsg      : HFG_EVENT_WRITE_MSG */
typedef struct _HfgCallbackParameters HfgCallbackParameters;
struct _HfgCallbackParameters 
{
    I8          status;
    U8          errCode;

    /* For certain events, a single member of this union will be valid.
     * See HfgEvent documentation for more information.
     */
    union {
        void                           *ptr;
        void                           *context;
        HfgHandsFreeFeatures            features;
        kal_bool                        enabled;
        const char                     *phoneNumber;
        const char                     *memory;        
        U16                             index;
        U8                              dtmf;
        U8                              gain;
        U8                              button; 
        HfgHold                        *hold;
        HfgHandsFreeVersion             version;
        void                           *response;
        MTK_BD_ADDR                    *addr;
        HfgResponseHold                 respHold;    

#if defined(BT_SCO_HCI_DATA) && BT_SCO_HCI_DATA == XA_ENABLED
        HfgAudioData                   *audioData;   
        BtPacket                       *audioPacket; 
#endif

        HfgAtData                      *data;
        const char                     *charset;       

        U16                             pbStorage;      
        HfgPbRead                      *pbRead;            
        HfgPbFind                      *pbFind;                
        HfgPbWrite                     *pbWrite;           

        HfgSMSService_cmd               service;            
        HfgSMSPrefStorage_cmd          *prefStorage;   
        HfgSMSFormat_cmd                format;                                                         
        HfgSMSSrviceCentre_cmd         *serviceCentre;     
        HfgSMSTextModeParam_cmd        *textParams;    
        HfgSMSShowParams_cmd            show;           
        HfgSMSIndSetting_cmd           *newMsgInd;     
        HfgSMSList_cmd                  stat;           
        HfgSMSRead_cmd                  readMsgIndex;   
        HfgSMSDelete_cmd                delMsgIndex;    
        HfgSMSSend_cmd                 *sendMsg;       
        HfgSMSSendStored_cmd           *storedMsg;         
        HfgSMSWrite_cmd                *writeMsg;      
    } p;
};

/* status : Used by message handler to return result of request */
typedef struct _HfgChannelContext
{
    void*                      hfgContext;
    kal_bool                   bHeadset;
    BTMTK_EventCallBack        callback;
    I8                         status;        
#if defined( BTMTK_ON_LINUX )
    int                        sockfd;
#endif
    char                       callIdNum[MAX_PHONE_NUMBER + 1];
    U8                         callType;
    struct btTimer             ringTimer;
}HfgChannelContext;

#endif//__BT_HFG_STRUCT_H__


