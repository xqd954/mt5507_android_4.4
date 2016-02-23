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
 * bt_adp_em.c
 *
 * Project:
 * --------
 * MAUI
 *
 * Description:
 * ------------
 * This file implements MTK BT tasks engineer mode related function
 *
 * Author:
 * -------
 * Brad Chang
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime$
 * $Log$
 *
 * 12 06 2010 autumn.li
 * [ALPS00025445] [BT] SSP Debug mode
 * SSP debug mode
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/ 
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_msg.h"
#include "bluetooth_struct.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "stdlib.h"

static void bt_em_ssp_debug_mode_req_hdlr(ilm_struct *ilm_ptr)
{
#ifdef __BT_2_1_SIMPLE_PAIRING__
    bt_ssp_debug_mode_req_struct *req_ptr = (bt_ssp_debug_mode_req_struct *)ilm_ptr->local_para_ptr;
    bt_ssp_debug_mode_cnf_struct *cnf_ptr;
    if (req_ptr->on)
    {
        MEC(sspDebugMode) = 0x1;
    }
    else
    {
        MEC(sspDebugMode) = 0x0;
    }
    MeWriteSSPDebug();

    cnf_ptr =
        (bt_ssp_debug_mode_cnf_struct*) construct_local_para(
                                                (kal_uint16) sizeof(bt_ssp_debug_mode_cnf_struct),
                                                TD_CTRL);
    cnf_ptr->result = KAL_TRUE;
    BT_SendMessage(MSG_ID_BT_SSP_DEBUG_MODE_CNF, MOD_MMI, (local_para_struct*) cnf_ptr, sizeof(bt_ssp_debug_mode_cnf_struct));
#endif
}

void bt_em_handler(ilm_struct *ilm_ptr)
{
    if (ilm_ptr == NULL)
    {
        Assert(0);
    }

    switch (ilm_ptr->msg_id)
    {
    case MSG_ID_BT_SSP_DEBUG_MODE_REQ:
        bt_em_ssp_debug_mode_req_hdlr(ilm_ptr);
        break;
    default:
        break;
    }
}

void bt_em_test_cmd_handler(ilm_struct *ilm_ptr){
    U8 *string;
    if (ilm_ptr == NULL)
    {
        Assert(0);
    } 

    switch (ilm_ptr->msg_id)
    {
    case MSG_ID_BT_TEST_CMD_REQ:
        {
        U16 ret = 1; /* 0 for success */
        bt_test_cmd_cnf_struct *cnf_p = NULL;
        bt_test_cmd_req_struct *req_ptr = (bt_test_cmd_req_struct *)ilm_ptr->local_para_ptr;
        OS_Report("[TEST] len:(%d) (%s)", strlen((const char*)req_ptr->test_cmd_content), req_ptr->test_cmd_content);
        string = req_ptr->test_cmd_content;
        req_ptr->test_cmd_content[sizeof(req_ptr->test_cmd_content)-1] = 0; //null terminated
        
        if(strncmp((const char*)string, "l2cap looptest",14)==0)
        {
            OS_Report("[TEST] l2cap looptest");
            ret = 0;
        }else if(strncmp((const char*)string, "fwdump",6)==0){
            int len = strlen((const char*)string);
            unsigned int fwdump;
            if(len > 7){
                fwdump = strtoul((const char*)&string[7], NULL, 16);
                OS_Report("fwdump set to : %X", fwdump);
                if( OS_SetSystemPropertyUint("fwdump", fwdump, 1) > 0 ){
                    /* success */
                    ret = 0;
        }
            }else{
                OS_Report("Invalid fwdump command : %s", string);
        }
        }else{
            OS_Report("Unknown command : %s", string);
        }
        cnf_p = (bt_test_cmd_cnf_struct*) construct_local_para(
                                                    (kal_uint16) sizeof(bt_test_cmd_cnf_struct),
                                                    TD_CTRL);
        cnf_p->result = ret;
        OS_MemCopy(cnf_p->test_cmd_content, req_ptr->test_cmd_content, sizeof(cnf_p->test_cmd_content));
        BT_SendMessage(MSG_ID_BT_TEST_CMD_CNF, MOD_MMI, (local_para_struct*) cnf_p, sizeof(bt_test_cmd_cnf_struct));
        }
        break;
    default:
        break;
    }    
}
