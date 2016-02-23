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

#ifndef _BT_MAUI_UTILITY_C
#define _BT_MAUI_UTILITY_C

#include "bt_common.h"

#include "bluetooth_trc.h"
#include "bluetooth_struct.h"
#ifdef __BT_BEFORE_PARTITION__
#include "bluetooth_bm_struct.h"
#else
#include "bluetooth_gap_struct.h"
#endif
#include "bt_jsr82_setting.h"
#include "bt.h"
#include "config.h"
#include "btconfig.h"
#include "ddb.h"
#include "bt_maui_utility.h"
#include "bt_adp_spp.h"
//#include "bt_spp_api.h"
//#include "tst_sap.h"
#ifdef __BT_BEFORE_PARTITION__
/* for mmi_bth_send_msg() */
#include "MMIDataType.h"
#include "BTMMICm.h" 
#endif /* __BT_BEFORE_PARTITION__ */ 
/***************************************************************************** 
* Global Variable
*****************************************************************************/
extern kal_uint8 sppAutoTxFlag;
#ifdef MTK_UPF_TESTING
extern U8 UPF_BD_ADDR[6];
U8 UPF_SETADDR = 0;
#endif /* MTK_UPF_TESTING */ 


#if (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__)
extern kal_uint8 jsr82_ut_session_con_addr[6];
#endif /* (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__) */

#ifdef __SIMAP_UT_
extern void simaptester_handler_inject_msg(char *string);
#endif 

#ifdef __GEMINI__
extern module_type Get_DualSim_Uart_Default_Owner(void);
#endif 

/***************************************************************************** 
* Global Function
*****************************************************************************/
#ifdef __MTK_TARGET__
extern kal_bool BT_PcmLoopbackTest(void);
#endif 

//extern void sppa_handler_inject_msg(char *string, kal_uint8 index);
extern void hfga_handler_inject_msg(char *string);
extern void bma_handler_inject_msg(char *string);
extern void hsga_handler_inject_msg(char *string);
extern void sdpdba_handler_inject_msg(char *string);

#ifdef __BQE_TESTER_
extern void HciTransportReset(void);
//extern void bma_panic(void);
#endif 

#ifdef __SDP_TESTER_
extern void sdp_tester_handler_inject_msg(char *string);
#endif 

#ifdef __L2CAP_TESTER_
extern void l2cap_handler_inject_msg(char *string);
#endif 

#ifdef __GAP_TESTER_
extern void gap_tester_handler_inject_msg(char *string);
#endif 

#ifdef __RFCOMM_TESTER_
//extern void rfcomm_tester_handler_inject_msg(char *string);
#endif 

#ifdef __OPP_TESTER_
extern void oppc_tester_handler_inject_msg(char *string);
#endif 

#ifdef __FTP_TESTER_
extern void ftpc_tester_handler_inject_msg(char *string);
#endif 

#ifdef MTK_BB_TESTING
void BTBBHandleTestCases(U8 *string);
#endif 

#if BT_JSR82_ENABLED
extern void BTJSR82_DeInitAll(void);
extern void JSR82_sendMsg(
                msg_type msg,
                module_type dstMod,
                local_para_struct *local_para,
                peer_buff_struct *peer_buff);
#endif /* BT_JSR82_ENABLED */ 

//extern void avrcp_handler_inject_msg(char *string);

extern void BtRadio_EMEnabledTxRxTest(
                                    U8 pattern,
                                    U8 channel_hopping,
                                    U8 tx_freq,
                                    U8 rx_freq,
                                    U8 poll_period,
                                    U8 packet_type,
                                    U16 packet_length);

extern char *BtRadio_EM_Get_Chip_Version(void);
extern char *BtRadio_EM_Get_Lmp_Version(void);
extern char *BtRadio_EM_Get_Patch_Version(void);

extern U8 getMECbdaddrInx(U8 inx);

#ifndef __BT_MP_RELEASE__
extern void BT_EnabledLocalLog(void);
#endif

#ifdef MTK_UPF_TESTING


/*****************************************************************************
 * FUNCTION
 *  BTUPFEnabled
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTUPFEnabled(void)
{
    UPF_SETADDR = 1;
}


/*****************************************************************************
 * FUNCTION
 *  BTUPFDisable
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTUPFDisable(void)
{
    UPF_SETADDR = 0;
}


/*****************************************************************************
 * FUNCTION
 *  BTUPFHandle
 * DESCRIPTION
 *  
 * PARAMETERS
 *  string      [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BTUPFHandle(U8 *string)
{
    U8 bd_addr[6];
    U8 name[] = "UPF30 IUT";
    btbm_bd_addr_t my_addr;
    bt_bm_pin_code_ind_struct *btbm_adp_msg1;
    bt_bm_bonding_result_ind_struct *btbm_adp_msg;

    {
        UPF_SETADDR = 1;
        bd_addr[0] = (string[0]) << 4 | (string[1]);
        bd_addr[1] = (string[2]) << 4 | (string[3]);
        bd_addr[2] = (string[4]) << 4 | (string[5]);
        bd_addr[3] = (string[6]) << 4 | (string[7]);
        bd_addr[4] = (string[8]) << 4 | (string[9]);
        bd_addr[5] = (string[10]) << 4 | (string[11]);
        UPF_BD_ADDR[0] = bd_addr[5];
        UPF_BD_ADDR[1] = bd_addr[4];
        UPF_BD_ADDR[2] = bd_addr[3];
        UPF_BD_ADDR[3] = bd_addr[2];
        UPF_BD_ADDR[4] = bd_addr[1];
        UPF_BD_ADDR[5] = bd_addr[0];

        my_addr.lap =
            (((kal_uint32) ((kal_uint8*) bd_addr)[3]) << 16) | ((kal_uint32) ((kal_uint8*) bd_addr)[4] << 8) |
            ((kal_uint32) ((kal_uint8*) bd_addr)[5]);
        my_addr.nap = (((kal_uint16) ((kal_uint8*) bd_addr)[1])) | ((kal_uint16) ((kal_uint8*) bd_addr)[0] << 8);
        my_addr.uap = bd_addr[2];

        btbm_adp_msg1 = (bt_bm_pin_code_ind_struct*) construct_local_para(sizeof(bt_bm_pin_code_ind_struct), TD_UL);

        btbm_adp_msg1->enable_16digits_pin_code = 0x00;
        btbm_adp_msg1->bd_addr = my_addr;
        btbm_adp_msg1->name_len = strlen((char*)name);
        OS_MemSet(btbm_adp_msg1->name, 0, sizeof(btbm_adp_msg1->name));
        OS_MemCopy(btbm_adp_msg1->name, (U8*) name, strlen((char*)name));
        bma_sendMsg(MSG_ID_BT_BM_PIN_CODE_IND, MOD_MMI, BT_APP_SAP, (local_para_struct*) btbm_adp_msg1, NULL);

        btbm_adp_msg =
            (bt_bm_bonding_result_ind_struct*) construct_local_para(sizeof(bt_bm_bonding_result_ind_struct), TD_UL);

        btbm_adp_msg->result = 0x00;
        btbm_adp_msg->bd_addr = my_addr;
        btbm_adp_msg->cod = 0x00;
        bma_sendMsg(MSG_ID_BT_BM_BONDING_RESULT_IND, MOD_MMI, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }
}

#endif /* MTK_UPF_TESTING */ 



/*****************************************************************************
 * FUNCTION
 *  BTCoreSpeechTest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTCoreSpeechTest(void)
{
    kal_uint8 result = FALSE;

#ifdef __MTK_TARGET__
    result = BT_PcmLoopbackTest();
#endif 
    bt_l4c_at_general_cnf_handler(result);
}


/*****************************************************************************
 * FUNCTION
 *  bt_send_msg
 * DESCRIPTION
 *  This is bt_send_msg function of BT module.
 *  send msg to other module
 * PARAMETERS
 *  msg             [IN]        To be sent
 *  dstMod          [IN]        
 *  sap             [IN]        
 *  local_para      [IN]         
 *  peer_buff       [IN]         
 * RETURNS
 *  void
 *****************************************************************************/
U8 bt_send_msg(
        msg_type msg,
        module_type dstMod,
        sap_type sap,
        local_para_struct *local_para,
        peer_buff_struct *peer_buff)
{

    ilm_struct *ilmPtr;

    if (!bt_p->is_factory_mode)
    {
        ilmPtr = allocate_ilm(MOD_BT);
        ilmPtr->msg_id = msg;
        ilmPtr->local_para_ptr = local_para;
        ilmPtr->peer_buff_ptr = peer_buff;
        ilmPtr->dest_mod_id = dstMod;
        ilmPtr->src_mod_id = MOD_BT;
        ilmPtr->sap_id = sap;
        msg_send_ext_queue(ilmPtr);
    }

    return;
}


/*****************************************************************************
 * FUNCTION
 *  BTPowerOn
 * DESCRIPTION
 *  
 * PARAMETERS
 *  uart_setting        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTPowerOn(kal_uint8 uart_setting)
{
    /* bt_send_msg(MSG_ID_BT_POWERON_REQ, MOD_BT, BT_APP_SAP, NULL, NULL); */
    ilm_struct *ilmPtr;

#ifdef __GEMINI__
    bt_poweron_req_struct *msg_p;

    msg_p = (bt_poweron_req_struct*) construct_local_para(sizeof(bt_poweron_req_struct), TD_CTRL);
    msg_p->dual_sim_uart_setting = uart_setting;
#endif /* __GEMINI__ */ 

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_POWERON_REQ;
#ifdef __GEMINI__
    ilmPtr->local_para_ptr = (local_para_struct*) msg_p;
#else 
    ilmPtr->local_para_ptr = NULL;
#endif 
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_BT;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}

#ifdef __BT_BEFORE_PARTITION__
/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SdpGetRecordHandler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SdpGetRecordHandler()
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD1_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SdpDeregisterRecord
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SdpDeregisterRecord(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD2_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SPPConReq_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SPPConReq_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD6_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_L2CAPConReq_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_L2CAPConReq_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD7_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SPPDisconReq_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SPPDisconReq_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD8_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_L2CAPDisconReq_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_L2CAPDisconReq_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD9_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}

#if 1


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SPPDeregistrationService_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SPPDeregistrationService_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD10_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_L2CAPDeregistrationService_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_L2CAPDeregistrationService_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD11_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_SetACLSecurityTest
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_SetACLSecurityTest(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD12_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_JBT_POWEROFF
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_JBT_POWEROFF(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD13_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_JBT_GetRemoteName
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_JBT_GetRemoteName(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD14_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);

}

#endif /* 1 */ 


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_Test
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_Test(void)
{
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD5_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);
}


/*****************************************************************************
 * FUNCTION
 *  BTJSR82_Test1
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BTJSR82_Test1(void)
{
#if (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__)
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);
    ilmPtr->msg_id = MSG_ID_BT_JSR_TEST_CMD2_REQ;
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_JASYN;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);
#endif /* (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__) */ 
}


#if BT_JSR82_ENABLED


/*****************************************************************************
 * FUNCTION
 *  BT_JSR82QueryPowerStatus
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
U8 BT_JSR82QueryPowerStatus(void)
{
    if (bt_p->power_status == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
#endif /* BT_JSR82_ENABLED */ 
#endif /* __BT_BEFORE_PARTITION__ */ 

/*****************************************************************************
 * FUNCTION
 *  BTGetBTVersionResult
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTGetBTVersionResult(kal_bool result)
{
    bt_get_bt_version_cnf_struct *msg;

    msg =
        (bt_get_bt_version_cnf_struct*) construct_local_para(
                                            (kal_uint16) sizeof(bt_get_bt_version_cnf_struct),
                                            TD_CTRL | TD_RESET);
    msg->result = result;
    if (result == KAL_TRUE)
    {
        strncpy(msg->chip_version, BtRadio_EM_Get_Chip_Version(), BT_MAX_VERSION_STR_SIZE - 1);
        strncpy(msg->lmp_version, BtRadio_EM_Get_Lmp_Version(), BT_MAX_VERSION_STR_SIZE - 1);
        strncpy(msg->patch_version, BtRadio_EM_Get_Patch_Version(), BT_MAX_VERSION_STR_SIZE - 1);
    }
    else
    {
        msg->chip_version[0] = '\0';
        msg->lmp_version[0] = '\0';
        msg->patch_version[0] = '\0';
    }
    bt_send_msg(MSG_ID_BT_GET_BT_VERSION_CNF, MOD_MMI, BT_APP_SAP, (local_para_struct*) msg, NULL);

}

#ifdef __BQE_TESTER_


/*****************************************************************************
 * FUNCTION
 *  btTester_get_bdaddr_from_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  bd_addr     [OUT]    
 *  string      [IN]     
 * RETURNS
 *  
 *****************************************************************************/
BOOL btTester_get_bdaddr_from_string(U8 *bd_addr, char *string)
{
    U8 i = 0;

    for (i = 0; i < 12; i++)
    {
        if ((string[i] >= '0') && (string[i] <= '9'))
        {
            string[i] -= '0';
        }
        else if ((string[i] <= 'f') && (string[i] >= 'a'))
        {
            string[i] = string[i] - 'a' + 10;
        }
        else if ((string[i] <= 'F') && (string[i] >= 'A'))
        {
            string[i] = string[i] - 'A' + 10;
        }
        else
        {
            return FALSE;
        }
    }
    bd_addr[5] = (string[0]) << 4 | (string[1]);
    bd_addr[4] = (string[2]) << 4 | (string[3]);
    bd_addr[3] = (string[4]) << 4 | (string[5]);
    bd_addr[2] = (string[6]) << 4 | (string[7]);
    bd_addr[1] = (string[8]) << 4 | (string[9]);
    bd_addr[0] = (string[10]) << 4 | (string[11]);
    return TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  bt_inject_string_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  local_para_ptr      [IN]     
 *  peer_buff_ptr       [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void bt_inject_string_hdlr(local_para_struct *local_para_ptr, peer_buff_struct *peer_buff_ptr)
{
    tst_module_string_inject_struct *msg_ptr;

    msg_ptr = (tst_module_string_inject_struct*) local_para_ptr;

    if (strcmp((char*)msg_ptr->string, "btpoweron") == 0)
    {
        bt_send_msg(MSG_ID_BT_POWERON_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    else if (strcmp((char*)msg_ptr->string, "btinquiry") == 0)
    {
        bt_bm_discovery_req_struct *btbm_adp_msg;

        btbm_adp_msg = (bt_bm_discovery_req_struct*) construct_local_para(sizeof(bt_bm_discovery_req_struct), TD_UL);

        btbm_adp_msg->cod = 0xffffffff;
        btbm_adp_msg->inquiry_length = 10;
        btbm_adp_msg->inquiry_number = 10;
        btbm_adp_msg->access_mode = BTBM_GENERAL_INQUIRY;
        btbm_adp_msg->discovery_mode = BTBM_DISCOVERY_WITHOUT_NAME;
        bt_send_msg(MSG_ID_BT_BM_DISCOVERY_REQ, MOD_BT, BT_APP_SAP, (local_para_struct*) btbm_adp_msg, NULL);
    }

#if (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__)
    if (strcmp((char*)msg_ptr->string, "btjsr82test") == 0)
    {
        BTJSR82_Test();
    }
    if (strncmp((char*)msg_ptr->string, "btjsr82conaddr", 14) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 14, strlen((char*)(msg_ptr->string)) - 14);
        btTester_get_bdaddr_from_string(jsr82_ut_session_con_addr, (char*)string);

    }
    if (strncmp((char*)msg_ptr->string, "btjsr82sscon", 12) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 12, strlen((char*)(msg_ptr->string)) - 12);
        if (string[0] == '1')
        {
            BTJSR82_SPPConReq_Test();
        }
        else if (string[0] == '2')
        {
            BTJSR82_L2CAPConReq_Test();
        }
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82authlink", 15) == 0)
    {
        Report(("Inject string and try to auth link"));
        BTJSR82_SetACLSecurityTest();
    }
#endif /* (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__) */
    if (strcmp((char*)msg_ptr->string, "btpoweroff") == 0)
    {
        bt_send_msg(MSG_ID_BT_POWEROFF_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }

    if (strcmp((char*)msg_ptr->string, "btrestart") == 0)
    {
        bt_send_msg(MSG_ID_BT_RESTART_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "btreset") == 0)
    {
        bt_send_msg(MSG_ID_BT_MMI_RESET_REQ, MOD_BT, BT_APP_SAP, NULL, NULL);
    }
    if (strncmp((char*)msg_ptr->string, "btbmreset", 9) == 0)
    {
        Report(("Inject to cause bt reset"));
        HciTransportReset();
//        bma_panic();
    }
#if BT_READ_RSSI == XA_ENABLED
    if(strncmp((char*)msg_ptr->string, "btgetrssi", 9) == 0)
    {
        bt_read_rssi_req_struct* bt_read_rssi = (bt_read_rssi_req_struct*)construct_local_para((kal_uint16)sizeof(bt_read_rssi_req_struct),TD_CTRL);
        bt_read_rssi->lap = 0x0031992A;
        bt_read_rssi->uap = 0x60;
        bt_read_rssi->nap = 0x0010;
        Report(("Inject bt get rssi value"));      
        bt_send_msg(MSG_ID_BT_READ_RSSI_REQ,MOD_BT,BT_APP_SAP,(local_para_struct*)bt_read_rssi,NULL);
    }
#endif    
#if (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__)
    if (strncmp((char*)msg_ptr->string, "btjsr82poweroff", 15) == 0)
    {
        Report(("Inject to cause JSR82 BT terminate"));
        BTJSR82_JBT_POWEROFF();
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82regs", 11) == 0)
    {
        Report(("Inject to register service"));
        BTJSR82_Test();
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82regrec", 13) == 0)
    {
        Report(("Inject to register sdp record"));
        BTJSR82_SdpGetRecordHandler();
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82deregs", 13) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 13, strlen((char*)(msg_ptr->string)) - 13);
        if (string[0] == '1')
        {
            Report(("Inject to deregister SPP service"));
            BTJSR82_SPPDeregistrationService_Test();
        }
        else if (string[0] == '2')
        {
            Report(("Inject to deregister L2CAP service"));
            BTJSR82_L2CAPDeregistrationService_Test();
        }
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82deregrec", 15) == 0)
    {
        Report(("Inject to deregister sdp record"));
        BTJSR82_SdpDeregisterRecord();
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82ssdisc", 13) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 13, strlen((char*)(msg_ptr->string)) - 13);
        if (string[0] == '1')
        {
            Report(("Inject to disc SPP connection"));
            BTJSR82_SPPDisconReq_Test();
        }
        else if (string[0] == '2')
        {
            Report(("Inject to disc L2CAP connection"));
            BTJSR82_L2CAPDisconReq_Test();
        }
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82lpmode", 13) == 0)
    {
        Report(("Inject to change tx rx loopback mode setting"));
        BT_SPECIAL_TEST1();
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82tx", 9) == 0)
    {
        Report(("Inject to start jsr82 tx"));
        BT_SPECIAL_TEST();
    }

    if (strncmp((char*)msg_ptr->string, "btjsr82askname", 14) == 0)
    {
        Report(("Inject to start ask remote name"));
        BTJSR82_JBT_GetRemoteName();
    }
#endif /* (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__) */

#ifdef __SPP_TESTER_
#if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)
    if (strncmp((char*)msg_ptr->string, "spp", 3) == 0)
    {
        sppa_handler_inject_msg((char*)msg_ptr->string, msg_ptr->index);
    }
#endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */
#endif /* __SPP_TESTER_ */ 

#ifdef __BT_HFG_PROFILE__
    if (strncmp((char*)msg_ptr->string, "hf", 2) == 0)
    {
        hfga_handler_inject_msg((char*)msg_ptr->string);
    }
#endif /* __BT_HFG_PROFILE__ */ 
#if defined(__BQE_TESTER_) && defined(__BMA_TESTER_)
    if (strncmp((char*)msg_ptr->string, "bma", 3) == 0)
    {
        bma_handler_inject_msg((char*)msg_ptr->string);
    }
#endif
#ifdef __BT_HFG_PROFILE__
    if (strncmp((char*)msg_ptr->string, "hs", 2) == 0)
    {
        hsga_handler_inject_msg((char*)msg_ptr->string);
    }
#endif /* __BT_HFG_PROFILE__ */ 
    if (strncmp((char*)msg_ptr->string, "oppsdpdb", 8) == 0)
    {
        sdpdba_handler_inject_msg((char*)msg_ptr->string + 8);
    }
#ifdef __AVDTP_TESTER_
    if (strncmp((char*)msg_ptr->string, "avdtp", 5) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 5, strlen((char*)(msg_ptr->string)) - 5);
        avdtp_handler_inject_msg(string);
    }
#endif /* __AVDTP_TESTER_ */ 
#ifdef __AVCTP_TESTER_
    if (strncmp((char*)msg_ptr->string, "avctp", 5) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 5, strlen((char*)(msg_ptr->string)) - 5);
        avctp_handler_inject_msg(string);
    }
#endif /* __AVCTP_TESTER_ */ 
#ifdef __SDP_TESTER_
    if (strncmp((char*)msg_ptr->string, "sdp", 3) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 3, strlen((char*)(msg_ptr->string)) - 3);
        sdp_tester_handler_inject_msg((char*)string);
    }
#endif /* __SDP_TESTER_ */ 
#ifdef __L2CAP_TESTER_
    if (strncmp((char*)msg_ptr->string, "l2cap", 5) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 5, strlen((char*)(msg_ptr->string)) - 5);
        l2cap_handler_inject_msg((char*)string);
    }
#endif /* __L2CAP_TESTER_ */ 
#ifdef __GAP_TESTER_
    if (strncmp((char*)msg_ptr->string, "gap", 3) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 3, strlen((char*)(msg_ptr->string)) - 3);
        gap_tester_handler_inject_msg((char*)string);
    }
#endif /* __GAP_TESTER_ */ 

#ifdef __RFCOMM_TESTER_
    if (strncmp((char*)msg_ptr->string, "rfcomm", 6) == 0)
    {
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 6, strlen((char*)(msg_ptr->string)) - 6);
//        rfcomm_tester_handler_inject_msg((char*)string);
    }
#endif /* __RFCOMM_TESTER_ */ 

#ifdef __OPP_TESTER_
    if (strncmp((char*)msg_ptr->string, "oppc", 4) == 0)
    {
        kal_uint8 string[90];

        memset(string, 0, 90);
        memcpy((char*)string, msg_ptr->string + 4, min(strlen((char*)(msg_ptr->string)) - 4, sizeof(string)-1) );
        oppc_tester_handler_inject_msg((char*)string);
    }
#endif /* __OPP_TESTER_ */ 

#ifdef __FTP_TESTER_
    if (strncmp((char*)msg_ptr->string, "ftpc", 4) == 0)
    {
        kal_uint8 string[90];

        memset(string, 0, 90);
        memcpy((char*)string, msg_ptr->string + 4, min(strlen((char*)(msg_ptr->string)) - 4, sizeof(string)01) );
        ftpc_tester_handler_inject_msg((char*)string);
    }
#endif /* __OPP_TESTER_ */ 


    if (strncmp((char*)msg_ptr->string, "avrcp", 5) == 0)
    {
//        avrcp_handler_inject_msg((char*)msg_ptr->string);
    }

#if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)
    if (strcmp((char*)msg_ptr->string, "sppauto") == 0)
    {
    #ifdef __SPP_AUTO_TX__
        sppAutoTxFlag = !sppAutoTxFlag;
        kal_prompt_trace(MOD_BT, "sppAutoTxFlag %d", sppAutoTxFlag);
    #endif /* __SPP_AUTO_TX__ */ 

    }
#endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */

#ifdef __SIMAP_UT_
    if (strncmp((char*)msg_ptr->string, "simap", 5) == 0)
    {   
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 5, strlen((char*)(msg_ptr->string)) - 5);
        simaptester_handler_inject_msg((char*)string);

    }
#endif /* __SIMAP_UT_ */ 
#ifdef MTK_BB_TESTING
    if (strncmp((char*)msg_ptr->string, "btbb", 4) == 0)
    {   
        kal_uint8 string[30];

        memset(string, 0, 30);
        memcpy((char*)string, msg_ptr->string + 4, strlen((char*)(msg_ptr->string)) - 4);
        BTBBHandleTestCases(string);
    }
#endif /* MTK_BB_TESTING */ 
#ifdef __BCC_TESTER_
#if (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__)
    if (strcmp((char*)msg_ptr->string, "bccauthorize") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD1_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccisauthorized") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD2_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccistrusted") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD3_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccgetsecuritymode") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD4_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccgetpreknowncount") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD5_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccgetpreknownlist") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD6_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccpoweron") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD7_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccrestore") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD8_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccsetname") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD9_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccgetname") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD10_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccsetdiscoverable") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD11_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccgetdiscoverable") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD12_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }
    if (strcmp((char*)msg_ptr->string, "bccgetnativepwrstatus") == 0)
    {
        bt_send_msg(MSG_ID_BT_JSR_TEST_BCC_CMD13_REQ, MOD_JASYN, BT_APP_SAP, NULL, NULL);
    }

#endif /* (BT_JSR82_ENABLED == 1) && defined(__BT_JSR82_STACK_UT__) */
#endif /* __BCC_TESTER_ */ 

}   /* end of bt_inject_string_hdlr() */
#endif /* __BQE_TESTER_ */

/*****************************************************************************
 * FUNCTION
 *  bt_get_bd_addr_req_hdlr
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_get_bd_addr_req_hdlr(BOOL result)
{
    ilm_struct *msg;
    bt_read_bd_addr_cnf_struct *bt_read_bd_addr_cnf;

    msg = allocate_ilm(MOD_BT);

    bt_read_bd_addr_cnf =
        (bt_read_bd_addr_cnf_struct*) construct_local_para(sizeof(bt_read_bd_addr_cnf_struct), TD_CTRL);

#ifdef __GEMINI__
    msg->dest_mod_id = Get_DualSim_Uart_Default_Owner();
#else 
    msg->dest_mod_id = MOD_ATCI;
#endif 
    msg->src_mod_id = MOD_BT;
    msg->msg_id = MSG_ID_BT_GET_BD_ADDR_CNF;
    if (result == TRUE)
    {
        bt_read_bd_addr_cnf->result = KAL_TRUE;
    }
    else
    {
        bt_read_bd_addr_cnf->result = KAL_FALSE;
    }
    msg->sap_id = BT_APP_SAP;

    bt_read_bd_addr_cnf->bd_addr[0] = getMECbdaddrInx(5);
    bt_read_bd_addr_cnf->bd_addr[1] = getMECbdaddrInx(4);
    bt_read_bd_addr_cnf->bd_addr[2] = getMECbdaddrInx(3);
    bt_read_bd_addr_cnf->bd_addr[3] = getMECbdaddrInx(2);
    bt_read_bd_addr_cnf->bd_addr[4] = getMECbdaddrInx(1);
    bt_read_bd_addr_cnf->bd_addr[5] = getMECbdaddrInx(0);

    msg->local_para_ptr = (local_para_struct*) bt_read_bd_addr_cnf;
    msg->peer_buff_ptr = NULL;

    msg_send_ext_queue(msg);
}

/*****************************************************************************
 * FUNCTION
 *  bt_engineer_mode_cnf_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  type        [IN]        
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_engineer_mode_cnf_handler(msg_type type, kal_bool result)
{
    ilm_struct *msg;
    bt_engineer_mode_cnf_struct *response_message;

    msg = allocate_ilm(MOD_BT);
    response_message =
        (bt_engineer_mode_cnf_struct*) construct_local_para(sizeof(bt_engineer_mode_cnf_struct), TD_CTRL);
    msg->dest_mod_id = MOD_MMI;
    msg->src_mod_id = MOD_BT;
    msg->msg_id = type;
    msg->sap_id = BT_APP_SAP;
    response_message->result = result;
    msg->local_para_ptr = (local_para_struct*) response_message;
    msg->peer_buff_ptr = NULL;

    msg->sap_id = BT_APP_SAP;
    msg_send_ext_queue(msg);
}

/*****************************************************************************
 * FUNCTION
 *  bt_l4c_at_general_cnf_handler
 * DESCRIPTION
 *  
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void bt_l4c_at_general_cnf_handler(kal_bool result)
{
    ilm_struct *msg;
    bt_l4c_at_general_cnf_struct *at_general_cnf;

    msg = allocate_ilm(MOD_BT);
    at_general_cnf =
        (bt_l4c_at_general_cnf_struct*) construct_local_para(sizeof(bt_l4c_at_general_cnf_struct), TD_CTRL);

#ifdef __GEMINI__
    msg->dest_mod_id = Get_DualSim_Uart_Default_Owner();
#else 
    msg->dest_mod_id = MOD_ATCI;
#endif 

    msg->src_mod_id = MOD_BT;
    msg->msg_id = MSG_ID_BT_L4C_AT_GENERAL_CNF;
    at_general_cnf->result = result;
    msg->local_para_ptr = (local_para_struct*) at_general_cnf;
    msg->peer_buff_ptr = NULL;

    msg_send_ext_queue(msg);

}


/*****************************************************************************
 * FUNCTION
 *  BT_PowerOffToJsr82
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BT_PowerOffToJsr82(void)
{
#if BT_JSR82_ENABLED
    BTJSR82_DeInitAll();
    JSR82_sendMsg(MSG_ID_BT_JSR_POWER_OFF_IND, MOD_MMI, (local_para_struct*) NULL, NULL);
#endif /* BT_JSR82_ENABLED */ 
}

#ifdef __BT_BEFORE_PARTITION__
void BTSlowDownUARTRxSpeed(module_type src_module)
{
#ifndef __BT_MT66XX_HCI_RETRANSMISSION__

    ilm_struct *ilmPtr;
    U32 counter=0;
    
    Report(("BTSlowDownUARTRxSpeed"));
    while(1)
    {
        kal_take_mutex(bt_p->bt_uart_rx_speed);
        if(bt_p->uart_rx_slow == 1)
        {
            kal_give_mutex(bt_p->bt_uart_rx_speed);
            break;
        }
        if(bt_p->uart_rx_slow_msg == 0)
        {
            bt_p->uart_rx_slow_msg = 1;
            ilmPtr = allocate_ilm(src_module);
            ilmPtr->msg_id = MSG_ID_BT_SET_UART_RX_SPEED_LOW_REQ;
            ilmPtr->local_para_ptr = NULL;
            ilmPtr->peer_buff_ptr = NULL;
            ilmPtr->dest_mod_id = MOD_BT;
            ilmPtr->src_mod_id = src_module;
            ilmPtr->sap_id = BT_APP_SAP;
            msg_send_ext_queue(ilmPtr);
        }
        kal_sleep_task(1);
        counter++;
        if(counter > 4500)
        {
            mmi_bth_send_msg(MOD_BT, BT_APP_SAP, MSG_ID_BT_RESTART_REQ, NULL, NULL);
//            ASSERT(0);
        }
        kal_give_mutex(bt_p->bt_uart_rx_speed);
    }
#endif
}
#endif /* __BT_BEFORE_PARTITION__ */ 

#ifdef __BT_BEFORE_PARTITION__
void BTNormalUARTRxSpeed(module_type src_module)
{
#ifndef __BT_MT66XX_HCI_RETRANSMISSION__
    ilm_struct *ilmPtr;
    U32 counter=0;

    Report(("BTNormalUARTTxSpeed"));
    while(1)
    {
        kal_take_mutex(bt_p->bt_uart_rx_speed);
        if(bt_p->uart_rx_slow == 0)
        {
            kal_give_mutex(bt_p->bt_uart_rx_speed);
            break;
        }
        if(bt_p->uart_rx_slow_msg == 0)
        {
            bt_p->uart_rx_slow_msg = 1;
            ilmPtr = allocate_ilm(src_module);
            ilmPtr->msg_id = MSG_ID_BT_SET_UART_RX_SPEED_HIGH_REQ;
            ilmPtr->local_para_ptr = NULL;
            ilmPtr->peer_buff_ptr = NULL;
            ilmPtr->dest_mod_id = MOD_BT;
            ilmPtr->src_mod_id = src_module;
            ilmPtr->sap_id = BT_APP_SAP;
            msg_send_ext_queue(ilmPtr);
        }
        kal_sleep_task(1);
        counter++;
        if(counter > 4500)
        {
            ASSERT(0);
        }

        kal_give_mutex(bt_p->bt_uart_rx_speed);
    }
#endif    
}
#endif /* __BT_BEFORE_PARTITION__ */ 

void BTChangeUartRxSpeedDown(void)
{
#ifndef __BT_MT66XX_HCI_RETRANSMISSION__
    kal_take_mutex(bt_p->bt_uart_rx_speed);
    bt_p->uart_rx_slow = bt_p->uart_rx_current_setting;
    bt_p->uart_rx_slow_msg = 0;
    Report(("BTChangeUartRxSpeedDown, %02x", bt_p->uart_rx_slow));
    kal_give_mutex(bt_p->bt_uart_rx_speed);
#endif    
}

void BTSetUartRxSpeedDownSetting(U8 mode)
{
#ifndef __BT_MT66XX_HCI_RETRANSMISSION__
    kal_take_mutex(bt_p->bt_uart_rx_speed);
    bt_p->uart_rx_slow = mode;
    bt_p->uart_rx_slow_msg = 0;
    kal_give_mutex(bt_p->bt_uart_rx_speed);
#endif    
}
void BTPowerOnSetUartRxSpeed(void)
{
#ifndef __BT_MT66XX_HCI_RETRANSMISSION__
    ilm_struct *ilmPtr;

    ilmPtr = allocate_ilm(MOD_BT);

    kal_take_mutex(bt_p->bt_uart_rx_speed);
    if(bt_p->uart_rx_slow == 1)
    {
        ilmPtr->msg_id = MSG_ID_BT_SET_UART_RX_SPEED_LOW_REQ;
    }
    else
    {
        ilmPtr->msg_id = MSG_ID_BT_SET_UART_RX_SPEED_HIGH_REQ;
    }
    bt_p->uart_rx_slow_msg = 1;
    kal_give_mutex(bt_p->bt_uart_rx_speed);
    ilmPtr->local_para_ptr = NULL;
    ilmPtr->peer_buff_ptr = NULL;
    ilmPtr->dest_mod_id = MOD_BT;
    ilmPtr->src_mod_id = MOD_BT;
    ilmPtr->sap_id = BT_APP_SAP;
    msg_send_ext_queue(ilmPtr);
#endif    
}


#ifndef __BT_MP_RELEASE__
extern U8 BT_VIRTUAL_SNIFFER_FLAG;
extern void BTWriteToVirtualSnifferClose(void);
void BTVirtualSnifferDecoder(void);
extern BOOL BTConfigDrive(void);
#endif
void BTDlightTest(void)
{
#ifndef __BT_MP_RELEASE__
    if(BT_VIRTUAL_SNIFFER_FLAG == 0)
    {
        // We call BTConfigDrive() to init the path for virtual sniffer path.
        if (BTConfigDrive() == FALSE)
        {
            return;
        }
        Report(("Turn the Virtual Sniffer to ON"));
        BTWriteToVirtualSnifferClose();
        BT_EnabledLocalLog();
        BT_VIRTUAL_SNIFFER_FLAG = 1;
    }
    else
    {
        Report(("Turn the Virtual Sniffer to OFF"));
        BTWriteToVirtualSnifferClose();
        BT_VIRTUAL_SNIFFER_FLAG = 0;
    }
#endif
}

void BTDlightTest1(void)
{
#ifndef __BT_MP_RELEASE__
    Report(("BTDlightTest1"));
    BTVirtualSnifferDecoder();
#endif
}



#endif /* #define _BT_MAUI_UTILITY_C */
