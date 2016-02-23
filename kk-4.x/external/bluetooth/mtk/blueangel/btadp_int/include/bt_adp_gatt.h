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
 * Bt_adp_bip.h
 *
 * Project:
 * --------
 *   BT GATT
 *
 * Description:
 * ------------
 *   This file is for BT GATT ADP service.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/

#ifndef BT_ADP_GATT_H
#define BT_ADP_GATT_H

#include "utils.h"
#include "bt_adp_system.h"

/***************************************************************************** 
 * Macro
 *****************************************************************************/ 
#define GATT_DBG_LEVEL_ERROR 1
#define GATT_DBG_LEVEL_WARN  2
#define GATT_DBG_LEVEL_DBG     3

#define GATT_DBG_LEVEL  GATT_DBG_LEVEL_DBG

#if defined(GATT_DBG_LEVEL) && (GATT_DBG_LEVEL >= GATT_DBG_LEVEL_DBG)
#define GATTC_DBG(fmt, ...)         OS_Report("[GATTC] " fmt, ## __VA_ARGS__)
#define GATTS_DBG(fmt, ...)       OS_Report("[GATTS] " fmt, ## __VA_ARGS__)
#else
#define GATTC_DBG(fmt, ...)       (void)0
#define GATTS_DBG(fmt, ...)       (void)0
#endif

#if defined(GATT_DBG_LEVEL) && (GATT_DBG_LEVEL >= GATT_DBG_LEVEL_WARN)
#define GATTC_WARN(fmt, ...)      OS_Report("[GATTC][WARN] %s#%d : " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define GATTS_WARN(fmt, ...)      OS_Report("[GATTS][WARN] %s#%d : " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#define GATTC_WARN(fmt, ...)       (void)0
#define GATTS_WARN(fmt, ...)       (void)0
#endif

#if defined(GATT_DBG_LEVEL) && (GATT_DBG_LEVEL >= GATT_DBG_LEVEL_ERROR)
#define GATTC_ERR(fmt, ...)         OS_Report("[GATTC][ERR] %s#%d : " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define GATTS_ERR(fmt, ...)         OS_Report("[GATTS][ERR] %s#%d : " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#else
#define GATTC_ERR(fmt, ...)       (void)0
#define GATTS_ERR(fmt, ...)       (void)0
#endif


//#define GATT_ASSERT(exp) (((exp) != 0 || BTCoreIsEngLoad() == 0) ? (void)0 : (void)bt_assert(#exp, __FILE__, __LINE__))
#define GATT_ASSERT(exp) (void)0


#define GATT_MALLOC(size)             get_ctrl_buffer(size)
#define GATT_free(p)                       do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)

/***************************************************************************** 
 * Definations
 ****************************************************************************/
/* Context of registered GATTC AP */
typedef struct{
    
}gattc_adp_conn_context_struct;

/* Context of registered GATTC AP */
typedef struct{
    
}gattc_adp_registered_context_struct;

/* Bluetooth GATTC context */
typedef struct{
    /* list of registered client (type is gattc_adp_registered_context_struct) */
    ListEntry clientList;
}gattc_adp_context_struct;

typedef struct{

}gatts_adp_context_struct;

/***************************************************************************** 
 * Enum
 *****************************************************************************/


/***************************************************************************** 
 * Structure
 *****************************************************************************/

/***************************************************************************** 
 * Extern Global Variable
 *****************************************************************************/

/***************************************************************************** 
 * Extern Global Function
 *****************************************************************************/
void gattc_adp_init(void);
void gattc_adp_deinit(void);
void gatts_adp_init(void);
void gatts_adp_deinit(void);

/* callback functions */
void btmtk_adp_gattc_handle_message(ilm_struct *ilm_ptr);
void btmtk_adp_gatts_handle_message(ilm_struct *ilm_ptr);

#endif /* BT_ADP_BIP_H */ 

