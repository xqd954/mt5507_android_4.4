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
/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_gap_api.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to expose common api for profile&gap
 *
 * Author:
 * -------
 * Liang Cheng
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/

#ifndef _BT_GAP_API_H_
#define _BT_GAP_API_H_

#include <stdlib.h>
#include <hardware/bluetooth.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <errno.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/un.h>
#include "cutils/sockets.h"

#include "bt_ext_event.h"
#include "bt_gap_struct.h"
#include "bt_ext_common.h"
#include "bt_ext_apis.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"


/*******************************************************************************
**
** Function         btmtk_gap_init
**
** Description     init gap context and create btif_task
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_init();

/*******************************************************************************
**
** Function         btmtk_gap_power_on
**
** Description     Send MSG_ID_BT_POWERON_REQ to mtkbt, power on bt
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_power_on();

/*******************************************************************************
**
** Function         btmtk_gap_status_ind
**
** Description     Send EV_BT_CB_BT_STATUS_IND to btif_task, notify current bt status
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_status_ind(int status);


/*******************************************************************************
**
** Function         btmtk_gap_disable
**
** Description     Send EV_BT_CB_DISABLE to btif_task,ask for disc acl or power off bt
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_disable();

/*******************************************************************************
**
** Function         btmtk_gap_power_off
**
** Description     Send MSG_ID_BT_POWERON_REQ to mtkbt, ask for power off bt
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_power_off();

/*******************************************************************************
**
** Function         btmtk_gap_acl_disconnect
**
** Description     Send MSG_ID_BT_BM_LINK_DISALLOW_REQ to mtkbt, ask for disc acl
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_acl_disconnect();

/*******************************************************************************
**
** Function         btmtk_gap_cleanup
**
** Description     clean up gap context and exit btif_task
**
** Returns          NULL
**
*******************************************************************************/
void btmtk_gap_cleanup(int force);

/*******************************************************************************
**
** Function         btmtk_gap_get_adapter_properties
**
** Description     Send EV_BT_CB_READ_ADAPTER_PROPERTY_ALL to btif_task, ask for read adapter all properties
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_get_adapter_properties();

/*******************************************************************************
**
** Function         btmtk_gap_get_adapter_property
**
** Description     Send EV_BT_CB_READ_ADAPTER_PROPERTY to btif_task, ask for read adapter one property
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_get_adapter_property(bt_property_type_t type);

/*******************************************************************************
**
** Function         btmtk_gap_set_local_name
**
** Description     modify local adapter name
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_set_local_name(const char *name, int name_len);

kal_bool btmtk_gap_set_local_cod(kal_uint32 cod);

/*******************************************************************************
**
** Function         btmtk_gap_set_scan_mode
**
** Description     modify local adapter scan mode
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_set_scan_mode(btbm_scan_enable_type scan_mode);

/*******************************************************************************
**
** Function         btmtk_gap_set_scan_mode_timeout
**
** Description     save adapter scan mode timeout value to host_cache file
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_set_scan_mode_timeout(kal_uint32 scan_mode_timeout);


/*******************************************************************************
**
** Function         btmtk_gap_set_adapter_property
**
** Description     modify local adapter property
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_set_adapter_property(const bt_property_t *prop);

/*******************************************************************************
**
** Function         btmtk_gap_get_remote_device_properties
**
** Description     Send propertyEV_BT_CB_READ_REMOTE_PROPERTY_ALL to btif_task, ask for read remote properties
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_get_remote_device_properties(bt_addr_struct *remote_addr);

/*******************************************************************************
**
** Function         btmtk_gap_get_remote_device_properties
**
** Description     Send propertyEV_BT_CB_READ_REMOTE_PROPERTY to btif_task, ask for read remote one property
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_get_remote_device_property(bt_addr_struct *remote_addr,
	                                                  bt_property_type_t type);

/*******************************************************************************
**
** Function         btmtk_gap_set_remote_device_property
**
** Description     modify remote device friendly name only
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_set_remote_device_property(bt_addr_struct *remote_addr,
                                                      const bt_property_t *prop);

/*******************************************************************************
**
** Function         btmtk_gap_get_remote_service_record
**
** Description     get remote device service record
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_get_remote_service_record(bt_addr_struct *remote_addr,
                                                     bt_uuid_t *uuid);

/*******************************************************************************
**
** Function         btmtk_gap_get_remote_services
**
** Description     Send MSG_ID_BT_BM_SERVICE_SEARCH_REQ to mtkbt, to do sdp
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_get_remote_services(bt_addr_struct *remote_addr);

/*******************************************************************************
**
** Function         btmtk_gap_ev_discovery_started
**
** Description     Send EV_BT_CB_DISCOVERY_STARTED to btif_task,  notify discovery started
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_ev_discovery_started();

/*******************************************************************************
**
** Function         btmtk_gap_ev_discovery_stopped
**
** Description     Send EV_BT_CB_DISCOVERY_STOPPED to btif_task,  notify discovery stopped
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_ev_discovery_stopped();

/*******************************************************************************
**
** Function         btmtk_gap_start_discovery
**
** Description     Send MSG_ID_BT_BM_DISCOVERY_REQ to mtkbt,  start inquiry
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_start_discovery(btbm_discovery_type mode);

/*******************************************************************************
**
** Function         btmtk_gap_cancel_discovery
**
** Description     Send MSG_ID_BT_BM_DISCOVERY_CANCEL_REQ to mtkbt,  cancel inquiry
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_cancel_discovery();

/*******************************************************************************
**
** Function         btmtk_gap_create_bond
**
** Description     Send EV_BT_CB_CREATE_BOND to btif_task,  ask for create bond device
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_create_bond(const bt_addr_struct *remote_addr);

kal_bool btmtk_gap_bond_create(const bt_addr_struct *remote_addr);

void btmtk_gap_rebond(bt_addr_struct *remote_addr);

void btmtk_gap_cancel_rebond();

/*******************************************************************************
**
** Function         btmtk_gap_cancel_bond
**
** Description     Send EV_BT_CB_CANCEL_BOND to btif_task,  ask for cancel bond
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_cancel_bond(const bt_addr_struct *remote_addr);

kal_bool btmtk_gap_bond_cancel(const bt_addr_struct *remote_addr);


/*******************************************************************************
**
** Function         btmtk_gap_remove_bond
**
** Description     Send EV_BT_CB_REMOVE_BOND to btif_task,  ask for remove bond device
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_remove_bond(bt_addr_struct *remote_addr);

kal_bool btmtk_gap_delete_trust(const bt_addr_struct *remote_addr);


/*******************************************************************************
**
** Function         btmtk_gap_pin_reply
**
** Description     Send MSG_ID_BT_BM_PIN_CODE_RSP to mtkbt,  response pin code request
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_pin_reply(const bt_addr_struct *remote_addr,
	                     	     kal_uint8  accept,
                         	     kal_uint8  pin_len,
                                 kal_uint8 *pin_code);

/*******************************************************************************
**
** Function         btmtk_gap_ssp_reply
**
** Description     response ssp request
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_gap_ssp_reply(const bt_addr_struct *remote_addr,
	                             kal_uint8  auth_type,
	                     	     kal_uint8  accept,
                                 kal_uint32 passkey);

/*******************************************************************************
**
** Function         btmtk_get_local_addr
**
** Description     Send MSG_ID_BT_BM_READ_LOCAL_ADDR_REQ to mtkbt,  get local addr
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_get_local_addr();


/*******************************************************************************
**
** Function         btmtk_enable_service
**
** Description     Send EV_BT_CB_ENABLE_SERVICE to btif_task,  ask for enable profile service
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
void btmtk_enable_service(uint8_t service_id);

/*******************************************************************************
**
** Function         btmtk_disable_service
**
** Description     Send EV_BT_CB_DISABLE_SERVICE to btif_task,  ask for enable profile service
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
void btmtk_disable_service(uint8_t service_id);

/*******************************************************************************
**
** Function         btmtk_enable_advance_service
**
** Description     Send EV_BT_CB_ENABLE_ADVANCE_SERVICE to advance service,  ask for enable advance profile service
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_enable_advance_service();

/*******************************************************************************
**
** Function         btmtk_disable_advance_service
**
** Description     Send EV_BT_CB_DISABLE_ADVANCE_SERVICE to advance service,  ask for disable advance profile service
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_disable_advance_service();

/*******************************************************************************
**
** Function         btmtk_config_hci_logging
**
** Description     Send MSG_ID_BT_HCISNOOP_CFG_REQ to mtkbt,  ask for disable/enable hci log
**
** Returns          KAL_TRUE or KAL_FALSE
**
*******************************************************************************/
kal_bool btmtk_config_hci_logging(uint8_t enable);

/*******************************************************************************
**
** Function         btmtk_gap_exit
**
** Description     Send EV_BT_CB_TASK_EXIT to mtkbt,  ask for exit btif task
**
** Returns          void
**
*******************************************************************************/
void btmtk_gap_exit();

/*****************************************************************************
 * FUNCTION
 *  btmtk_host_cache_init
 * DESCRIPTION
 *  Read all paired device cache data from file
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_host_cache_init();

void btmtk_host_cache_write();


/*****************************************************************************
 * FUNCTION
 *  btmtk_paired_dev_cache_init
 * DESCRIPTION
 *  Read all paired device cache data from file
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_paired_dev_cache_init();

btmtk_device_entry_struct *btmtk_paired_dev_cache_find(bt_addr_struct *addr);

void btmtk_paired_dev_cache_write();

void btmtk_paired_dev_cache_del(bt_addr_struct *addr);

void btmtk_paired_dev_cache_add(btmtk_device_entry_struct *entry);

btmtk_device_entry_struct *btmtk_inquired_dev_cache_find(bt_addr_struct *addr);

btmtk_device_entry_struct *btmtk_inquired_dev_cache_add(btmtk_device_entry_struct *entry);

void btmtk_inquired_dev_cache_clear();

btmtk_bond_req_struct *btmtk_util_find_bond_request(bt_addr_struct *addr);

kal_bool btmtk_util_load_autopair_device_list();

void btmtk_util_unload_autopair_device_list();

void btmtk_util_load_dynamic_blacklist();

void btmtk_util_unload_dynamic_blacklist();

void btmtk_util_add_device_to_dynamic_blacklist(bt_addr_struct *addr);

kal_bool  btmtk_is_device_autopair_blacklisted(bt_gap_context_t * pContext, bt_addr_struct *addr);

kal_bool btmtk_is_fixed_pin_zeros_keyboard(bt_gap_context_t * pContext, bt_addr_struct *addr);


#endif
