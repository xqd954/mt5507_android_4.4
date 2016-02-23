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
 * prxattdb.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Proximity ATT record database
 *
 * Author:
 * -------
 * Autumn Li(mtk01174)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 12 14 2011 dlight.ting
 * NULL
 * .
 *
 * 11 04 2011 dlight.ting
 * NULL
 * .
 *
 * 10 31 2011 dlight.ting
 * NULL
 * .
 *
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "attdb.h"
#include "attdef.h"
#include "attdbmain.h"
#include "iasservice.h"
#include "tpsservice.h"
#include "llsservice.h"

#if defined(__BT_PRXR_PROFILE__) || defined(__BT_PRXM_PROFILE__)

extern U8 ProximityImmAlertLevel(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw);
extern U8 BtPrxLocalTxPowerDatabase(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw);
extern U8 ProximityLinkLossAlert(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw);




BtIasServiceCbHandler IasAlertLevelHandle;
BtTpsServiceCbHandler TpsTxPowerDatabaseHandle;
BtLlsServiceCbHandler LlsLinkLostAlertHandle;

/*********************
Function Definitions 
**********************/


/*****************************************************************************
 * FUNCTION
 *  PrxAttDB_Registration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 PrxAttDB_Registration(void)
{
    BtStatus status;

    status = IasActivateService();
    if(status == BT_STATUS_SUCCESS)
    {
        InitializeListEntry(&(IasAlertLevelHandle.node));
        IasAlertLevelHandle.callback = ProximityImmAlertLevel;
        IasRegisterAttdbCallback(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE,
                 &(IasAlertLevelHandle));
    }
    
    status = TpsActivateService();
    if(status == BT_STATUS_SUCCESS)
    {
        InitializeListEntry(&(TpsTxPowerDatabaseHandle.node));
        TpsTxPowerDatabaseHandle.callback = BtPrxLocalTxPowerDatabase;
        TpsRegisterAttdbCallback(ATT_HANDLE_TP_CHARACTERISTIC_TX_POWER_LEVEL_VALUE,
                 &(TpsTxPowerDatabaseHandle));
    }
    
    status = LlsActivateService();
    if(status == BT_STATUS_SUCCESS)
    {
        Report(("Return status 1"));

        InitializeListEntry(&(LlsLinkLostAlertHandle.node));
        Report(("Return status 2"));

        LlsLinkLostAlertHandle.callback = ProximityLinkLossAlert;
        Report(("Return status 3"));
        
        LlsRegisterAttdbCallback(ATT_HANDLE_LL_CHARACTERISTIC_ALERT_LEVEL_VALUE,
                 &(LlsLinkLostAlertHandle));
        Report(("Return status 4"));
        
    }
    Report(("Return status:%d",status));
    return (U8)status;
}

/*****************************************************************************
 * FUNCTION
 *  PrxAttDB_Deregistration
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 PrxAttDB_Deregistration(void)
{
    IasDeactivateService();
    IasDeregisterAttdbCallback(&(IasAlertLevelHandle));
    
    TpsDeactivateService();
    TpsDeregisterAttdbCallback(&(TpsTxPowerDatabaseHandle));
    
    LlsDeactivateService();
    LlsDeregisterAttdbCallback(&(LlsLinkLostAlertHandle));
    return (U8)BT_STATUS_SUCCESS;
}
#endif

