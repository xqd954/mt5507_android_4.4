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

#include "bt_common.h"
#include "btalloc.h"
#include "sys/hci.h"
#include "utils.h"
#include "btconfig.h"
#include "sys/l2capxp.h"
#include "sys/sdpi.h"
#include "bt_feature.h"
#include "x_file.h"
#include "bt_osal.h"
#include "me_adp.h"

BtOperation fmOverSBCEnableOp;      /* Write Change Uart Speed operation */
BtOperation fmOverSBCDisableOp;

#ifdef __WIFI_SUPPORT__
extern void wndrv_PTA_isOFF(void);
extern void wndrv_PTA_isON(void);
#endif /* __WIFI_SUPPORT__ */ 

extern void BTChangeUartRxSpeedDown(void);

kal_uint8 wifi_channel_table[][10] =
{
#if 1
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F},
#else
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F},/*0: all channel used*/        
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0x7F},/*1: For WiFi channel 1~4, use high 20 channels*/
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0x7F},/*2: For WiFi channel 5,    use 0,    60~78*/
    {0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x7F},/*3: For WiFi channel 6,    use 0~5,  65~78*/
    {0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7F},/*4: For WiFi channel 7,    use 0~10, 70~78*/
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78},/*5: For WiFi channel 8,    use 0~15, 75~78*/
    {0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} /*6: For WiFi channel 9~14, use 0~15, 75~78*/
#endif
};


void MeRadioHandleErrorTaskEnd(void);

/*****************************************************************************
 * FUNCTION
 *  MeWriteAFH
 * DESCRIPTION
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
U8 MeWriteAFH(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(afhOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(afhOp).node)))
        {
            Assert(0);
        }
        MEC(afhOp).opType = BOP_WRITE_AFH;
        InsertTailList(&MEC(opList), &(MEC(afhOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
        return TRUE;
    }
    return FALSE;    
}

#if defined (__BT_MT66XX_HCI_RETRANSMISSION__)
/*****************************************************************************
 * FUNCTION
 *  MeEnabledMT66xxFlowControlOn
 * DESCRIPTION
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void MeEnabledMT66xxFlowControlOn(void)
{
    if(!IsEntryAvailable(&(MEC(enabledMT66xxFlowControl).node)))
    {
        Assert(0);
    }
    MEC(enabledMT66xxFlowControl).opType = BOP_WRITE_MT66XX_FLOW_CONTROL_ENABLED;
    InsertTailList(&MEC(opList), &(MEC(enabledMT66xxFlowControl).node));
    /* Attempt to start the operation, it may not be in progress. */
    MeStartOperation();
}
#endif


/*****************************************************************************
 * FUNCTION
 *  MeWritePTA
 * DESCRIPTION
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
U8 MeWritePTA(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(pta1Op).opType == BOP_NOP) &&
        (MEC(pta2Op).opType == BOP_NOP)
        )
    {
        if(!IsEntryAvailable(&(MEC(pta1Op).node)))
        {
            Assert(0);
        }
        MEC(pta1Op).opType = BOP_WRITE_PTA1;
        InsertTailList(&MEC(opList), &(MEC(pta1Op).node));
#if defined (__BT_CHIP_VERSION_1__)
        MEC(pta2Op).opType = BOP_WRITE_PTA2;
        if(!IsEntryAvailable(&(MEC(pta2Op).node)))
        {
            Assert(0);
        }
        InsertTailList(&MEC(opList), &(MEC(pta2Op).node));

#endif

        MeStartOperation();
        return TRUE;
    }
    return FALSE;    
}

void MeWritePTAInfo(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(ptaInfoOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(ptaInfoOp).node)))
        {
            Assert(0);
        }
        MEC(ptaInfoOp).opType = BOP_WRITE_PTAINFO;
        InsertTailList(&MEC(opList), &(MEC(ptaInfoOp).node));
        MeStartOperation();
    }
}

void MeWriteChangeUartSendSpeed(U8 speed)
{
    MEC(changedUartSpeed) = speed;
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (MEC(changeUartSpeedOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(changeUartSpeedOp).node)))
        {
            Assert(0);
        }
        MEC(changeUartSpeedOp).opType = BOP_WRITE_UART_SEND_SPEED;
        InsertTailList(&MEC(opList), &(MEC(changeUartSpeedOp).node));
        MeStartOperation();
    }


}

/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteAFH
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWriteAFH(BtOpEvent event)
{
	U8 wifiCh_table_index, byte_count;

    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            #ifdef __DIRECT_CHNL_MAP__
            OS_MemCopy((U8*)(MEC(hciGenCmd).parms), (const U8*)BTWIFIC(chnl_map), 10);
            #else
        	if (BTWIFIC(freq)==0)
        	{
        		wifiCh_table_index = 0;
        	}
        	else if ((BTWIFIC(freq) >0) && (BTWIFIC(freq)<5))
        	{
        		wifiCh_table_index = 1;
        	}
        	else if (BTWIFIC(freq)>8)
        	{
        		wifiCh_table_index = 6;
        	}
        	else
        	{
        		wifiCh_table_index = (U8)(BTWIFIC(freq) - 3);
        	}
            {
                extern const U8 wifi_channel_table[7][10];
            	for (byte_count = 0; byte_count <10; byte_count++)
            	{
            		MEC(hciGenCmd).parms[byte_count] = 
                            wifi_channel_table[wifiCh_table_index][byte_count];
            	}

            }
            #endif /* __DIRECT_CHNL_MAP__ */
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 10);
            MeSendHciCommand(0x0C3F, 10);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(afhOp).opType = BOP_NOP;
            bt_trace(TRACE_GROUP_1, ME_WRITE_AFH_COMPLETED_ERROR__xX, MEC(btEvent).errCode);
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTCOMMAND_COMPLETE_OF_AFH);
#if defined (WIFI_BB_MT5921) && (defined (__BT_CHIP_VERSION_1__))
            bt_trace(BT_TRACE_G1_PROTOCOL, BTLOG_BTLOG_PTA_CHANNEL_IS_CALLED);
            MeWritePTAInfo();
//            BtRadio_PTA_ChannelInfo(BTWIFIC(freq), BTWIFIC(hb));
            break;
#else
            BTWIFIC(afh_cmd_made) = 0;
#endif
            
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}



/*****************************************************************************
 * FUNCTION
 *  MeHandleWritePTA1
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWritePTA1(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            BTWIFIC(current_pta_mode)= BTWIFIC(pta_action);
#if defined (__BT_CHIP_VERSION_0__)
        	if (BTWIFIC(pta_action) != 0)
        	{
        		MEC(hciGenCmd).parms[0] = 0x50;
        		MEC(hciGenCmd).parms[1] = 0x01;
        		MEC(hciGenCmd).parms[2] = 0x05;
        		MEC(hciGenCmd).parms[3] = 0x80;
        		MEC(hciGenCmd).parms[4] = 0x00;
        		MEC(hciGenCmd).parms[5] = 0x55;
        		MEC(hciGenCmd).parms[6] = 0x00;
        		MEC(hciGenCmd).parms[7] = 0x00;
        	}
        	else
        	{
        		MEC(hciGenCmd).parms[0] = 0x50;
        		MEC(hciGenCmd).parms[1] = 0x01;
        		MEC(hciGenCmd).parms[2] = 0x05;
        		MEC(hciGenCmd).parms[3] = 0x80;
        		MEC(hciGenCmd).parms[4] = 0x00;
        		MEC(hciGenCmd).parms[5] = 0x00;
        		MEC(hciGenCmd).parms[6] = 0x00;
        		MEC(hciGenCmd).parms[7] = 0x00;
        	}
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 10);
            MeSendHciCommand(0xFCD0, 8);

#else
#if defined (__BT_CHIP_VERSION_1__)
            if (BTWIFIC(pta_action) != 0)
            {
                MEC(hciGenCmd).parms[0] = 0x01;
            }
            else
            {
                MEC(hciGenCmd).parms[0] = 0x00;
            }
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
            MeSendHciCommand(0xFCD2, 1);
#endif
#endif
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(pta1Op).opType = BOP_NOP;
            Report(("PTA Task1 end"));
#if defined (__BT_CHIP_VERSION_0__)
#ifdef __WIFI_SUPPORT__
            if (BTWIFIC(current_pta_mode) == 1)
            {
                wndrv_PTA_isON();
            }
            else
            {
                wndrv_PTA_isOFF();
            }
#endif /* __WIFI_SUPPORT__ */ 
            BTWIFIC(pta_cmd_made) = 0;
#endif
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}


/*****************************************************************************
 * FUNCTION
 *  MeHandleWritePTA2
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void MeHandleWritePTA2(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            if (BTWIFIC(current_pta_mode) != 0)
            {
                MEC(hciGenCmd).parms[0] = 0x01;
            }
            else
            {
                MEC(hciGenCmd).parms[0] = 0x00;
            }
            if (BTWIFIC(current_pta_mode) != 0)
            {
                MEC(hciGenCmd).parms[0] = 0x40;
                MEC(hciGenCmd).parms[1] = 0x00;
                MEC(hciGenCmd).parms[2] = 0x05;
                MEC(hciGenCmd).parms[3] = 0x80;
                MEC(hciGenCmd).parms[4] = 0x2F;
                MEC(hciGenCmd).parms[5] = 0xFF;
                MEC(hciGenCmd).parms[6] = 0x00;
                MEC(hciGenCmd).parms[7] = 0x00;
            }
            else
            {
                MEC(hciGenCmd).parms[0] = 0x40;
                MEC(hciGenCmd).parms[1] = 0x00;
                MEC(hciGenCmd).parms[2] = 0x05;
                MEC(hciGenCmd).parms[3] = 0x80;
                MEC(hciGenCmd).parms[4] = 0xFF;
                MEC(hciGenCmd).parms[5] = 0xFF;
                MEC(hciGenCmd).parms[6] = 0x00;
                MEC(hciGenCmd).parms[7] = 0x00;
            }
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 8);
            MeSendHciCommand(0xFCD0, 8);
            return;

        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(pta2Op).opType = BOP_NOP;
            Report(("PTA Task2 end"));
#if defined (__BT_CHIP_VERSION_1__)
#ifdef __WIFI_SUPPORT__
            if (BTWIFIC(current_pta_mode) == 1)
            {
                wndrv_PTA_isON();
            }
            else
            {
                wndrv_PTA_isOFF();
            }
#endif /* __WIFI_SUPPORT__ */ 
            BTWIFIC(pta_cmd_made) = 0;
#endif            
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleWritePTAInfo(BtOpEvent event)
{
#if defined (WIFI_BB_MT5921) && (defined (__BT_CHIP_VERSION_1__))
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            ASSERT( BTWIFIC(freq) < 15) ;
            ASSERT( BTWIFIC(hb) < 79) ;
            MEC(hciGenCmd).parms[0] = BTWIFIC(freq);
            MEC(hciGenCmd).parms[1] = BTWIFIC(hb);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(0xFCD3, 2);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(ptaInfoOp).opType = BOP_NOP;
            Report(("PTA Info end"));
            BTWIFIC(afh_cmd_made) = 0;
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
#else
    Assert(0);
#endif

}

void MeHandleWriteSendSpeed(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
#if defined (__BT_CHIP_VERSION_1__)
            Report(("Change UART speed:%d",MEC(changedUartSpeed)));
            if(MEC(changedUartSpeed) == 0x01)
                MEC(hciGenCmd).parms[0] = 0xFE;
            else
                MEC(hciGenCmd).parms[0] = 0x00;
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
            MeSendHciCommand(0xFCBF, 1);
            return;
#endif
#if defined (__BT_CHIP_VERSION_0__)
            MEC(hciGenCmd).parms[1] = 0x00;
            MEC(hciGenCmd).parms[2] = 0x00;
            MEC(hciGenCmd).parms[3] = 0x00;

            if(MEC(changedUartSpeed) == 0x01)
                MEC(hciGenCmd).parms[0] = 0x80;
            else
                MEC(hciGenCmd).parms[0] = 0x00;
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 4);
            MeSendHciCommand(0xFC34, 4);
            return;
#endif
            Assert(0);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(changeUartSpeedOp).opType = BOP_NOP;
            Report(("Send Change UART speed"));
#if defined (__BT_MAUI_PLATFORM__)
#if defined (__BT_CHIP_VERSION_1__)
            Report(("0xFCBF completed"));
            BTChangeUartRxSpeedDown();
#endif
#if defined (__BT_CHIP_VERSION_0__)
            Report(("0xFCD0 completed"));
            BTChangeUartRxSpeedDown();
#endif                        
#endif
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeRadioHandleErrorTaskEnd(void)
{
    Assert(0);
}

#if defined (__BT_MT66XX_HCI_RETRANSMISSION__)
void MeHandleWriteMT66xxFlowControlEnabled(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Report(("MeHandleWriteMT66xxFlowControlEnabled"));
            MEC(hciGenCmd).parms[0] = 0x01;
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 1);
            MeSendHciCommand(HCC_MT66XX_Enabled_FLOW_CONTROL, 1);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(enabledMT66xxFlowControl).opType = BOP_NOP;
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}
#endif


U16 btFMOverSBCStarthciHandler = 0;
U16 btFMOverSBCL2CAPmaxLength = 0;
U16 btFMOverSBCAVDTP_Sig_SID = 0;
U16 btFMOverSBCAVDTP_Sig_CID = 0;
U16 btFMOverSBCL2CAPCid;
U8 btFMOverSBCCapability[4];
U16 btFMOverSBCFeatures = 0;
U16 btFMOverStopSBChciHandler = 0;

void MeRadioHostInit(void)
{

}

void MeWriteEnableFMoverSBC(BtRemoteDevice *link,U16 max_l2cap_size, U16 AVDTP_Sig_SID, U16 AVDTP_Sig_CID, U16 l2cap_cid, U8 *sbc_capability, U16 features)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        (fmOverSBCEnableOp.opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(fmOverSBCEnableOp.node)))
        {
            Assert(0);
        }
        if(link->state != BDS_CONNECTED)
        {
            return;
        }
        btFMOverSBCStarthciHandler = link->hciHandle;
        btFMOverSBCL2CAPmaxLength = max_l2cap_size;
        btFMOverSBCAVDTP_Sig_SID = AVDTP_Sig_SID;
        btFMOverSBCAVDTP_Sig_CID = AVDTP_Sig_CID;
        btFMOverSBCL2CAPCid = l2cap_cid;
        fmOverSBCEnableOp.opType = BOP_WRITE_ENABLE_FM_OVER_SBC;
        memcpy((U8 *)btFMOverSBCCapability, sbc_capability, 4);
        btFMOverSBCFeatures = features;
        InsertTailList(&MEC(opList), &(fmOverSBCEnableOp.node));
        MeStartOperation();
    }
}


void MeWriteDisableFMoverSBC(BtRemoteDevice *link)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && 
        ((fmOverSBCDisableOp.opType == BOP_NOP)))
    {
        if(!IsEntryAvailable(&(fmOverSBCDisableOp.node)))
        {
            Assert(0);
        }
        btFMOverStopSBChciHandler = link->hciHandle;
        fmOverSBCDisableOp.opType = BOP_WRITE_DISABLE_FM_OVER_SBC;
        InsertTailList(&MEC(opList), &(fmOverSBCDisableOp.node));
        MeStartOperation();
    }
}

void MeHandleWriteEnableFMOverSBC(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            StoreLE16(MEC(hciGenCmd).parms, btFMOverSBCStarthciHandler);
            StoreLE16(MEC(hciGenCmd).parms + 2, btFMOverSBCAVDTP_Sig_SID);
            StoreLE16(MEC(hciGenCmd).parms + 4, btFMOverSBCAVDTP_Sig_CID);
            StoreLE16(MEC(hciGenCmd).parms + 6, btFMOverSBCL2CAPCid);
            StoreLE16(MEC(hciGenCmd).parms + 8, btFMOverSBCL2CAPmaxLength);
            memcpy(MEC(hciGenCmd).parms+10, btFMOverSBCCapability, 4);
            StoreLE16(MEC(hciGenCmd).parms + 14, btFMOverSBCFeatures);
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 16);
            Report(("Write FM over SBC start"));
            Report(("command raw data:%02x,%02x,%02x,%02x,%02x,%02x",
                MEC(hciGenCmd).parms[0],MEC(hciGenCmd).parms[1],
                MEC(hciGenCmd).parms[2],MEC(hciGenCmd).parms[3],
                MEC(hciGenCmd).parms[4],MEC(hciGenCmd).parms[5]));
            Report(("command raw data:%02x,%02x,%02x,%02x,%02x,%02x",
                MEC(hciGenCmd).parms[6],MEC(hciGenCmd).parms[7],
                MEC(hciGenCmd).parms[8],MEC(hciGenCmd).parms[9],
                MEC(hciGenCmd).parms[10],MEC(hciGenCmd).parms[11]));
            Report(("command raw data:%02x,%02x,%02x,%02x,%02x,%02x",
                MEC(hciGenCmd).parms[12],MEC(hciGenCmd).parms[13],
                MEC(hciGenCmd).parms[14],MEC(hciGenCmd).parms[15],
                0,0/*MEC(hciGenCmd).parms[14],MEC(hciGenCmd).parms[15]*/));
            MeSendHciCommand(0xFC83, 16);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            fmOverSBCEnableOp.opType = BOP_NOP;
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleWriteDisableFMOverSBC(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Assert(IsEntryAvailable(&(MEC(hciGenCmd).node)));
            StoreLE16(MEC(hciGenCmd).parms, btFMOverStopSBChciHandler);
            Report(("Write FM over SBC end"));
            Report(("command raw data:%02x,%02x"));

            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, 2);
            MeSendHciCommand(0xFC84, 2);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            fmOverSBCDisableOp.opType = BOP_NOP;
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();

}

#ifdef __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__

extern void BTSetDualPCMSettingCnf(void);
extern void BTSetDualPCMSwitchCnf(void);

void MeWriteDualPCMSetValue(U8 *value, U16 len)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(dualPCMSetOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(dualPCMSetOp).node)))
        {
            Assert(0);
        }
        if(value && (len > 0 && len <= 32)){
          MEC(dualPCMSetOp).opType = BOP_SET_DUAL_PCM_SETTING;
          OS_MemCopy(MEC(dualPCMSetValue), value, len);
          MEC(dualPCMSetValue_len) = len;
          InsertTailList(&MEC(opList), &(MEC(dualPCMSetOp).node));
  
          /* Attempt to start the operation, it may not be in progress. */
          MeStartOperation();
        }else{
            bt_prompt_trace(MOD_BT, "MeWriteDualPCMSetValue faile : val=%x, len=%d", value, len);
            BTSetDualPCMSettingCnf();
        }
    }
}

void MeWriteDualPCMSwitch(U8 *value, U16 len)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(dualPCMSwitchOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(dualPCMSwitchOp).node)))
        {
            Assert(0);
        }
        if(value && (len > 0 && len <= 32)){
          MEC(dualPCMSwitchOp).opType = BOP_SET_DUAL_PCM_SWITCH;
          OS_MemCopy(MEC(dualPCMSwitchValue), value, len);
          MEC(dualPCMSwitchValue_len) = len;
          InsertTailList(&MEC(opList), &(MEC(dualPCMSwitchOp).node));
  
          /* Attempt to start the operation, it may not be in progress. */
          MeStartOperation();
        }else{
            bt_prompt_trace(MOD_BT, "MeWriteDualPCMSwitch faile : val=%x, len=%d", value, len);
            BTSetDualPCMSwitchCnf();
        }
    }
}

void MeHandleWritePCMSetValue(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Report(("MeHandleWritePCMSetValue(%d, %d)", MEC(dualPCMSetValue)[0], MEC(dualPCMSetValue_len)));
            OS_MemCopy(MEC(hciGenCmd).parms, MEC(dualPCMSetValue), MEC(dualPCMSetValue_len));
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms,  MEC(dualPCMSetValue_len));
            MeSendHciCommand(0xFC72, MEC(dualPCMSetValue_len));
            //MeSendHciCommand(HCC_DUAL_PCM_SET_VALUE, 4/*MEC(dualPCMSetValue_len)*/);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(dualPCMSetOp).opType = BOP_NOP;
            BTSetDualPCMSettingCnf();
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}

void MeHandleWritePCMSwitchValue(BtOpEvent event)
{
    switch (event)
    {
        case BOE_START:
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            Report(("MeHandleWritePCMSwitchValue(%d)", MEC(dualPCMSwitchValue)));
            Report(("MeHandleWritePCMSwitchValue(%x, %d)", MEC(dualPCMSwitchValue)[0], MEC(dualPCMSwitchValue_len)));
            OS_MemCopy(MEC(hciGenCmd).parms, MEC(dualPCMSwitchValue), MEC(dualPCMSwitchValue_len));
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, MEC(dualPCMSwitchValue_len));
            MeSendHciCommand(0xFC90, 2);
            //MeSendHciCommand(HCC_DUAL_PCM_SWITCH, 4/*MEC(dualPCMSwitchValue_len)*/);
            return;
        case BOE_TASK_END:
        case BOE_TASK_ERR:
            MEC(dualPCMSwitchOp).opType = BOP_NOP;
            BTSetDualPCMSwitchCnf();
            break;
        default:
            MeRadioHandleErrorTaskEnd();
    }
    /* Start another operation if one exists */
    MeOperationEnd();
}
#endif /* __MTK_BT_DUAL_PCM_SWITCH_SUPPORT__ */

#ifdef __BT_TEST_MODE_ENABLED__
extern void testModeCnf(kal_bool result, kal_bool query);
/*****************************************************************************
 * FUNCTION
 *  MeHandleWriteSniffSubratingOp
 * DESCRIPTION
 *  
 * PARAMETERS
 *  event       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void HandleTestModeEnable(BtOpEvent event)
{
    HciCommandType command_tbl[] = {HCC_SET_BD_ADDR, HCC_WRITE_AUTH_ENABLE, HCC_ENABLE_DUT, HCC_EVENT_FILTER};
    U8 cmdCount = sizeof(command_tbl)/sizeof(HciCommandType);
    HciCommandType cmd = command_tbl[MEC(bttm_seq)];
    U16 param_len = 0;
    switch (event)
    {
        case BOE_TASK_END:
            if(MEC(bttm_seq) >= cmdCount){
                Report(("HandleTestModeEnable completed"));
                break;
            }
        case BOE_START:
            cmd = command_tbl[MEC(bttm_seq)++];
            Report(("HandleTestModeEnable : cmd=%d, seq=%d", cmd, MEC(bttm_seq)));
            /* Start the task */
            MEC(taskHandler) = MeHandleCompleteTask;
            MEC(taskEvent) = HCE_COMMAND_COMPLETE;
            if(!IsEntryAvailable(&(MEC(hciGenCmd).node)))
            {
                Assert(0);
            }
            switch(cmd){
            case HCC_SET_BD_ADDR:
                param_len = 6;
                MEC(hciGenCmd).parms[0] = MEC(bttmAddr).addr[5];
                MEC(hciGenCmd).parms[1] = MEC(bttmAddr).addr[4];
                MEC(hciGenCmd).parms[2] = MEC(bttmAddr).addr[3];
                MEC(hciGenCmd).parms[3] = MEC(bttmAddr).addr[2];
                MEC(hciGenCmd).parms[4] = MEC(bttmAddr).addr[1];
                MEC(hciGenCmd).parms[5] = MEC(bttmAddr).addr[0];                
                break;
            case HCC_WRITE_AUTH_ENABLE:
                param_len = 1;
                MEC(hciGenCmd).parms[0] = 0x00;
                break;
            case HCC_ENABLE_DUT:
                param_len = 0;
                break;
            case HCC_EVENT_FILTER:
                param_len = 3;
                MEC(hciGenCmd).parms[0] = 0x02;
                MEC(hciGenCmd).parms[1] = 0x00;
                MEC(hciGenCmd).parms[2] = 0x02;
                break;
            default:
                OS_Report("unexpected HCC command : %d", cmd);
                break;
            }
            BT_XFile_EncryptionCommand(MEC(hciGenCmd).parms, param_len);
            MeSendHciCommand(cmd, param_len);
            return;
            
        case BOE_TASK_ERR:
        default:
            Report(("HandleTestModeEnable(): set %d end, seq=%d", MEC(bttmOp), MEC(bttm_seq)));
            break;
    }
    testModeCnf((MEC(bttm_seq) >= cmdCount) ? KAL_TRUE : KAL_FALSE, KAL_FALSE);
    MEC(bttmOp).opType = BOP_NOP;
    /* Start another operation if one exists */
    MeOperationEnd();
}

/*****************************************************************************
 * FUNCTION
 *  MeEnterTestMode
 * DESCRIPTION
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
U8 MeEnterTestMode(void)
{
    if ((MEC(stackState) < BTSS_INITIALIZE_ERR) && (MEC(bttmOp).opType == BOP_NOP))
    {
        if(!IsEntryAvailable(&(MEC(bttmOp).node)))
        {
            Assert(0);
        }
        MEC(bttmOp).opType = BOP_TEST_MODE;
        MEC(bttm_seq) = 0;
        InsertTailList(&MEC(opList), &(MEC(bttmOp).node));
        /* Attempt to start the operation, it may not be in progress. */
        MeStartOperation();
        return TRUE;
    }
    return FALSE;    
}
#endif /* __BT_TEST_MODE_ENABLED__ */

