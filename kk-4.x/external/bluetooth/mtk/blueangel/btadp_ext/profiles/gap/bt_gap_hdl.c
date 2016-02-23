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
 * Bt_gap_hdl.c
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

#include "utils/Log.h"

#include <stdlib.h>
#include <hardware/bluetooth.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>

#include "bt_utils.h"
#include "bt_ext_timer.h"
#include "btif_common.h"
#include "bt_ext_common.h"
#include "bt_gap_api.h"
#include "bt_gap_struct.h"
#include "bt_profile_manager.h"
#ifdef __BT_USE_CUST_DATA__
#include "bt_cust_api.h"
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "bt_gap_hdl.c"

#define COD_UNCLASSIFIED ((0x1F) << 8)
#define COD_HID_KEYBOARD       0x0540
#define COD_HID_POINTING       0x0580
#define COD_HID_COMBO          0x05C0
#define COD_AV_HEADSETS        0x0404
#define COD_AV_HANDSFREE       0x0408
#define COD_AV_HEADPHONES      0x0418
#define COD_AV_PORTABLE_AUDIO  0x041C
#define COD_AV_HIFI_AUDIO      0x0428

unsigned char SDP_INSECURE[16] = 
	{0x30, 0x1c, 0x21, 0x4f, 0x91, 0xa2, 0x43, 0xbf, 0xa7, 0x95, 0x09, 0xd1, 0x19, 0x8a, 0x81, 0xa7};
unsigned char SDP_SECURE[16] = 
	{0x85, 0x91, 0xD7, 0x57, 0x18, 0xEE, 0x45, 0xE1, 0x9B, 0x12, 0x92, 0x87, 0x5D, 0x06, 0xBA, 0x23};

extern uint32_t btif_get_enabled_services_mask(void);
extern bt_status_t btif_hf_execute_service(BT_BOOL b_enable);
extern bt_status_t btif_hh_execute_service(BT_BOOL b_enable);
extern bt_status_t btif_av_execute_service(bool b_enable);
extern bt_status_t btif_pan_execute_service(BT_BOOL b_enable);

kal_bool isAutoPairDevice(kal_uint32 cod)
{
	if(((cod & 0x7ff) == COD_AV_HEADSETS) ||
	   ((cod & 0x7ff) == COD_AV_HANDSFREE) ||
	   ((cod & 0x7ff) == COD_AV_HEADPHONES) ||
	   ((cod & 0x7ff) == COD_AV_PORTABLE_AUDIO) ||
	   ((cod & 0x7ff) == COD_AV_HIFI_AUDIO) ||
	   ((cod & 0x7ff) == COD_HID_POINTING))
	{
		return KAL_TRUE;
	}
	else
	{
		return KAL_FALSE;
	}
	
}

int btmtk_convert_status(btbm_gap_result status)
{
    int ret = BT_STATUS_SUCCESS;
	
	switch(status)
	{
	case BTBM_ADP_SUCCESS:
		ret = BT_STATUS_SUCCESS;
		break;
    case BTBM_ADP_FAILED:
		ret = BT_STATUS_FAIL;
		break;
    case BTBM_ADP_FAIL_BUSY:
		ret = BT_STATUS_BUSY;
		break;
    case BTBM_ADP_FAIL_NOT_ALLOW:
		ret = BT_STATUS_UNSUPPORTED;
		break;
    case BTBM_ADP_FAIL_TIMEOUT:
		ret = BT_STATUS_BUSY;
		break;
	case BTBM_ADP_FAIL_NO_RESOURCE:
    case BTBM_ADP_FAIL_KEYERR:
    case BTBM_ADP_FAIL_REMOTE_REJECT:
    case BTBM_ADP_FAIL_LINK_KEY_DISMISSED:
		ret = BT_STATUS_FAIL;
		break;
	}
	return ret;
}

bt_scan_mode_t btmtk_convert_scanmode(int mode)
{
	kal_bool discoverable = KAL_FALSE;;
	kal_bool connectable = KAL_FALSE;
	
	if((mode & BTBM_ADP_P_OFF_I_ON) > 0)
		discoverable = KAL_TRUE;

	if((mode & BTBM_ADP_P_ON_I_OFF) > 0)
		connectable = KAL_TRUE;

	if((discoverable == KAL_TRUE)
		&& (connectable == KAL_TRUE))
		return BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE;
	else if(connectable == KAL_TRUE)
		return BT_SCAN_MODE_CONNECTABLE;
	else
		return BT_SCAN_MODE_NONE;
		
}

bt_device_type_t btmtk_convert_devtype(int devicetype)
{
	bt_device_type_t type = BT_DEVICE_DEVTYPE_BREDR;
	
	switch(devicetype)
	{
	case BTBM_DEVICE_TYPE_LE:
		type = BT_DEVICE_DEVTYPE_BLE;
		break;
	case BTBM_DEVICE_TYPE_BR_EDR:
		type = BT_DEVICE_DEVTYPE_BREDR;
		break;
	case BTBM_DEVICE_TYPE_BR_EDR_LE:
		type = BT_DEVICE_DEVTYPE_DUAL;
		break;
	default:
		type = BT_DEVICE_DEVTYPE_BREDR;
		break;
	}
	return type;
}                                   


int btmtk_get_local_feature_mask(bt_gap_context_t *pContext)
{
	if(pContext == NULL)
		return 0;
	
	return pContext->read_local_feature_flag;
}

void btmtk_set_local_feature_mask(bt_gap_context_t *pContext, int flag, kal_bool enable)
{
	if(pContext == NULL)
		return;

	if(enable)
		pContext->read_local_feature_flag |= (1<<flag);
	else
		pContext->read_local_feature_flag &= ~(1<<flag);
}

void btmtk_clear_local_feature_mask(bt_gap_context_t *pContext)
{
	if(pContext == NULL)
		return;

	pContext->read_local_feature_flag = 0;
}

void btmtk_get_adapter_property(bt_gap_context_t *pContext, 
	                                 bt_property_t *property)
{
	int i = 0;
    bt_bdaddr_t *addr;
    bt_bdname_t *name;
    bt_scan_mode_t *mode;
    kal_uint32 *disc_timeout;
    bt_bdaddr_t *bonded_devices;
	unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	int num_uuids = 0;
    bt_uuid_t *local_uuids;

	ALOGI("[GAP] btmtk_get_adapter_property type:[%d]!\n", property->type);
	if(property->type == BT_PROPERTY_BDADDR)
	{
		/* BD_ADDR */
		addr = (bt_bdaddr_t *)property->val;
		if(!btmtk_check_bdaddr(&pContext->local_bd_addr))
			btmtk_util_convert_bdaddr2array(addr, &(pContext->host_cache.local_addr));
		else
			btmtk_util_convert_bdaddr2array(addr, &(pContext->local_bd_addr));
		property->len = sizeof(bt_bdaddr_t);
		ALOGI("[GAP] btmtk_get_adapter_property addr:[0x%X:0x%X:0x%X:0x%X:0x%X:0x%X]!\n",
			  addr->address[0], addr->address[1], addr->address[2],
		      addr->address[3], addr->address[4], addr->address[5]);
	}
	else if(property->type == BT_PROPERTY_BDNAME)
	{
	    /* BD_NAME */
		name = (bt_bdname_t*)property->val;
		strncpy((char *)name->name, pContext->host_cache.name, 248);
		property->len = strlen((char *)name->name);
		ALOGI("[GAP] btmtk_get_adapter_property name:[%s]!\n",
			  name->name);
	}
	else if(property->type == BT_PROPERTY_ADAPTER_SCAN_MODE)
	{
		/* SCAN_MODE */
		mode = (bt_scan_mode_t *)property->val;
		*mode = btmtk_convert_scanmode((int)pContext->host_cache.scan_mode);
		property->len = sizeof(bt_scan_mode_t);
		ALOGI("[GAP] btmtk_get_adapter_property scan_mode:[%d]!\n",
			  *mode);
	}
	else if(property->type == BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT)
	{
	    /* DISC_TIMEOUT */
		disc_timeout = (kal_uint32 *)property->val;
		*disc_timeout = pContext->host_cache.scan_mode_timeout;
		property->len = sizeof(kal_uint32);
		ALOGI("[GAP] btmtk_get_adapter_property scan_mode_timeout:[%ld]!\n",
			  *disc_timeout);
	}
	else if(property->type == BT_PROPERTY_ADAPTER_BONDED_DEVICES)
	{
		/* BONDED_DEVICES */
		bonded_devices = (bt_bdaddr_t *)property->val;
		for(i = 0; i < pContext->paired_cache_no; i ++)
		{
			btmtk_util_convert_bdaddr2array(&bonded_devices[i], &(pContext->paired_dev_cache[i].addr));
			ALOGI("[GAP] btmtk_get_adapter_property bonded_devices:[0x%X:0x%X:0x%X:0x%X:0x%X:0x%X]!\n",
				  bonded_devices[i].address[0], bonded_devices[i].address[1], bonded_devices[i].address[2],
			      bonded_devices[i].address[3], bonded_devices[i].address[4], bonded_devices[i].address[5]);
		}
		property->len = pContext->paired_cache_no*sizeof(bt_bdaddr_t);
	}
    else if(property->type == BT_PROPERTY_UUIDS)
    {
	    /* LOCAL UUIDs */
        local_uuids = (bt_uuid_t*)property->val;
        kal_uint32 service_mask = btif_get_enabled_services_mask();
        for (i=0; i < BTMTK_MAX_SERVICE_ID; i++)
        {
            /* This should eventually become a function when more services are enabled */
            if (service_mask
                &(kal_uint32)(1 << i))
            {
                switch (i)
                {
                    case BTMTK_HFP_SERVICE_ID:
                        {
                            kal_uint32 custValue = 0x03;                            
#ifdef __BT_USE_CUST_DATA__
                            initCust(NULL);      
                            getCustValue(getCustID("HFPSupportMask"), &custValue);
                            if (custValue > 0x03)
                            {
                                custValue = 0x03;
                            }
#endif                            
                            if (custValue & 0x01)
                            {
                                btmtk_util_uuid16_to_uuid128(BTMTK_SDP_UUID_HEADSET_AUDIO_GATEWAY,
                                                  local_uuids+num_uuids);
                                num_uuids++;
                            }
                            if (custValue & 0x02)
                            {
                                btmtk_util_uuid16_to_uuid128(BTMTK_SDP_UUID_AG_HANDSFREE,
                                                  local_uuids+num_uuids);
                                num_uuids++;
                            }
                        }break;
                    case BTMTK_A2DP_SERVICE_ID:
                        {
                            btmtk_util_uuid16_to_uuid128(BTMTK_SDP_UUID_AUDIO_SOURCE,
                                              local_uuids+num_uuids);
                            num_uuids++;
                        }break;
                }
            }
        }
        property->len = (num_uuids)*sizeof(bt_uuid_t);
    }
}

void btmtk_get_remote_device_property(bt_gap_context_t *pContext, 
										     bt_addr_struct *bd_addr,
	                                         bt_property_t *property)
{
	btmtk_device_entry_struct *paired_entry = NULL;
    bt_bdname_t *name;
	bt_bdname_t *friendlyname;
    kal_uint32 *cod;
	kal_uint32 *devicetype;
	bt_uuid_t  *remote_uuid;
	unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	int i = 0;

	paired_entry = btmtk_paired_dev_cache_find(bd_addr);
	if(paired_entry == NULL)
		return;

	ALOGI("[GAP] btmtk_get_remote_device_property addr:[0x%lX:0x%X:0x%X],type:[%d]!\n",
		  bd_addr->lap, bd_addr->uap, bd_addr->nap, property->type);
	if(property->type == BT_PROPERTY_BDNAME)
	{
	    /* BD_NAME */
		name = (bt_bdname_t*)property->val;
		strncpy((char *)name->name, paired_entry->name, 248);
		property->len = strlen((char *)name->name);	
		ALOGI("[GAP] btmtk_get_remote_device_property name:[%s]!\n",
		       name->name);
	}
	else if(property->type == BT_PROPERTY_REMOTE_FRIENDLY_NAME)
	{
	    /* BD_FRIENDLYNAME */
		friendlyname = (bt_bdname_t*)property->val;
		strncpy((char *)friendlyname->name, paired_entry->nickname, 248);
		property->len = strlen((char *)friendlyname->name);
		ALOGI("[GAP] btmtk_get_remote_device_property friendly_name:[%s]!\n",
		       friendlyname->name);
	}
	else if(property->type == BT_PROPERTY_CLASS_OF_DEVICE)
	{
		/* COD */
		cod = (kal_uint32 *)property->val;
		*cod = paired_entry->cod;
		property->len = sizeof(kal_uint32);
		ALOGI("[GAP] btmtk_get_remote_device_property cod:[%lx]!\n",
		      *cod);
	}
	else if(property->type == BT_PROPERTY_TYPE_OF_DEVICE)
	{
	    /* DEVICE_OF_TYPE */
		devicetype= (kal_uint32 *)property->val;
		*devicetype = btmtk_convert_devtype(paired_entry->device_type);
		property->len = sizeof(kal_uint32);
		ALOGI("[GAP] btmtk_get_remote_device_property devtype:[%ld]!\n",
		      *devicetype);
	}
    else if(property->type == BT_PROPERTY_UUIDS)
    {
	    /* LOCAL UUIDs */
		remote_uuid = (bt_uuid_t *)property->val;
		for(i = 0; i < paired_entry->sdp.uuid_no; i ++)
		{
			btmtk_util_uuid16_to_uuid128(paired_entry->sdp.uuid[i], &remote_uuid[i]);
		}
		property->len = paired_entry->sdp.uuid_no*sizeof(bt_uuid_t);
    }
}

bt_status_t btmtk_execute_service_request(kal_uint32 service_id,
										                      kal_bool bEnable)
{
	ALOGI("[GAP] btmtk_ev_execute_service_cv service_id:[%ld], bEnable:[%d]!\n", 
		   service_id, bEnable);
    switch (service_id)
    {
         case BTMTK_HFP_SERVICE_ID:
         {
              //enable/disable hfp service
              btif_hf_execute_service(bEnable);
         }
		 break;
         case BTMTK_A2DP_SERVICE_ID:
         {
              //enable/disable a2dp/avrcp service
              btif_av_execute_service(bEnable);
         }
		 break;
         case BTMTK_HID_SERVICE_ID:
         {
              //enable/disable hid service
              btif_hh_execute_service(bEnable);
         }
		 break;
         case BTMTK_PAN_SERVICE_ID:
         {
              //enable/disable hid service
              btif_pan_execute_service(bEnable);
         }
		 break;

         default:
		 	ALOGI("[GAP] btmtk_ev_execute_service_cb Unknown!\n");
            return BT_STATUS_FAIL;
    }
    return BT_STATUS_SUCCESS;
}

void btmtk_update_device_property_name(bt_gap_context_t *pContext,
                                                bt_addr_struct *addr,
                                                char *devicename,
                                                int namelen)
{
    btmtk_device_entry_struct *entry[2] = {NULL, NULL};
	int len = (namelen >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : namelen;
	char name[BTBM_ADP_MAX_NAME_LEN] = {'\0'};
	bt_property_t  prop;
	int index = 0;
	bt_bdaddr_t bdaddr;

    ALOGI("[GAP] btmtk_util_update_device_property_name: name = %s", devicename);
	if (devicename == NULL || namelen == 0)
	{
		return;
	}
	strncpy(name, devicename, len);
	name[len] = '\0';
	
	entry[0] = btmtk_inquired_dev_cache_find(addr);
	entry[1] = btmtk_paired_dev_cache_find(addr);
	for (index = 0; index < 2; index ++)
	{
		if (entry[index] == NULL || (strncmp(entry[index]->name, name, BTBM_ADP_MAX_NAME_LEN) == 0))
		{
			continue;
		}
		strcpy(entry[index]->name, name);

		if (entry[index]->paired == BTMTK_BOND_STATE_BONDED) 
		{
			btmtk_paired_dev_cache_write();
		}
		
	}
}

void btmtk_update_remote_properties(bt_addr_struct *bd_addr, char *bd_name,
                                          kal_uint32 cod, kal_uint8 devtype)
{
    int num_properties = 0;
    bt_property_t properties[3];
    bt_bdaddr_t bdaddr;
	bt_device_type_t dev_type;

	ALOGI("[GAP] btmtk_update_remote_properties: addr:[0x%lX:0x%X:0x%X], name = %s, cod:[%lx], devtype:[%d]!\n",
		  bd_addr->lap, bd_addr->uap, bd_addr->nap, bd_name, cod, devtype);
	
    btmtk_util_convert_bdaddr2array(&bdaddr, bd_addr);

    /* remote name */
    if (bd_name != NULL && strlen((const char *) bd_name))
    {
    	properties[num_properties].type = BT_PROPERTY_BDNAME;
		properties[num_properties].len = strlen((char *)bd_name);
		properties[num_properties].val = bd_name;
        num_properties++;
    }

    /* class of device */
	if(cod == 0)
	{
		cod = COD_UNCLASSIFIED;
	}
	properties[num_properties].type = BT_PROPERTY_CLASS_OF_DEVICE;
	properties[num_properties].len = sizeof(cod);
	properties[num_properties].val = &cod;
    num_properties++;

	dev_type = btmtk_convert_devtype(devtype);
	properties[num_properties].type = BT_PROPERTY_TYPE_OF_DEVICE;
	properties[num_properties].len = sizeof(dev_type);
	properties[num_properties].val = &dev_type;
    num_properties++;
    HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb,
                     BT_STATUS_SUCCESS, &bdaddr, num_properties, properties);
}

int btmtk_read_adapter_properties(bt_gap_context_t *pContext, bt_property_t *properties)
{
    int num_properties = 0;
    bt_bdaddr_t addr;
    bt_bdname_t name;
    bt_scan_mode_t mode;
    kal_uint32 disc_timeout;
    bt_bdaddr_t bonded_devices[BTBM_ADP_MAX_PAIRED_LIST_NO];
	unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	int uuid_len = 0;
    bt_uuid_t local_uuids[BTBM_ADP_MAX_SDAP_UUID_NO];

    /* BD_ADDR */
	properties[num_properties].type = BT_PROPERTY_BDADDR;
	properties[num_properties].len = sizeof(addr);
	properties[num_properties].val = &addr;
	btmtk_get_adapter_property(pContext, &properties[num_properties]);
    num_properties ++;

    /* BD_NAME */
	memset(&name, 0, sizeof(bt_bdname_t));
	properties[num_properties].type = BT_PROPERTY_BDNAME;
	properties[num_properties].len = sizeof(name);
	properties[num_properties].val = &name;
	btmtk_get_adapter_property(pContext, &properties[num_properties]);
    num_properties ++;

    /* SCAN_MODE */
	properties[num_properties].type = BT_PROPERTY_ADAPTER_SCAN_MODE;
	properties[num_properties].len = sizeof(mode);
	properties[num_properties].val = &mode;
	btmtk_get_adapter_property(pContext, &properties[num_properties]);
    num_properties ++;

    /* DISC_TIMEOUT */
	properties[num_properties].type = BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT;
	properties[num_properties].len = sizeof(disc_timeout);
	properties[num_properties].val = &disc_timeout;
	btmtk_get_adapter_property(pContext, &properties[num_properties]);
    num_properties ++;

    /* BONDED_DEVICES */
	if(pContext->paired_cache_no > 0)
	{
		properties[num_properties].type = BT_PROPERTY_ADAPTER_BONDED_DEVICES;
		properties[num_properties].len = sizeof(bonded_devices);
		properties[num_properties].val = bonded_devices;
		btmtk_get_adapter_property(pContext, &properties[num_properties]);
	    num_properties ++;
	}

    /* LOCAL UUIDs */
	properties[num_properties].type = BT_PROPERTY_UUIDS;
	properties[num_properties].len = sizeof(local_uuids);
	properties[num_properties].val = local_uuids;
	btmtk_get_adapter_property(pContext, &properties[num_properties]);
    num_properties ++;

	HAL_CBACK(bt_hal_cbacks, adapter_properties_cb,
                     BT_STATUS_SUCCESS, num_properties, properties);
    return num_properties;
}

int btmtk_read_remote_device_properties(bt_gap_context_t *pContext, 
	                                           bt_addr_struct *bdaddr,
	                                           bt_property_t *properties)
{
    int num_properties = 0;
	bt_bdaddr_t remote_addr;
    bt_bdname_t name;
	bt_bdname_t friendlyname;
    kal_uint32 cod;
	kal_uint32 devicetype;
	bt_uuid_t  remote_uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];

    /* BD_NAME */
	memset(&name, 0, sizeof(bt_bdname_t));
	properties[num_properties].type = BT_PROPERTY_BDNAME;
	properties[num_properties].len = sizeof(name);
	properties[num_properties].val = &name;
	btmtk_get_remote_device_property(pContext, bdaddr, &properties[num_properties]);
    num_properties ++;

    /* BD_FRIENDLYNAME */
	memset(&friendlyname, 0, sizeof(bt_bdname_t));
	properties[num_properties].type = BT_PROPERTY_REMOTE_FRIENDLY_NAME;
	properties[num_properties].len = sizeof(friendlyname);
	properties[num_properties].val = &friendlyname;
	btmtk_get_remote_device_property(pContext, bdaddr, &properties[num_properties]);
    num_properties ++;

    /* BD_COD */
	properties[num_properties].type = BT_PROPERTY_CLASS_OF_DEVICE;
	properties[num_properties].len = sizeof(kal_uint32);
	properties[num_properties].val = &cod;
	btmtk_get_remote_device_property(pContext, bdaddr, &properties[num_properties]);
    num_properties ++;

    /* BD_DEVTYPE */
	properties[num_properties].type = BT_PROPERTY_TYPE_OF_DEVICE;
	properties[num_properties].len = sizeof(kal_uint32);
	properties[num_properties].val = &devicetype;
	btmtk_get_remote_device_property(pContext, bdaddr, &properties[num_properties]);
    num_properties ++;

    /* BD_UUID */
	properties[num_properties].type = BT_PROPERTY_UUIDS;
	properties[num_properties].len = sizeof(remote_uuid);
	properties[num_properties].val = remote_uuid;
	btmtk_get_remote_device_property(pContext,  bdaddr, &properties[num_properties]);
    num_properties ++;
	
    btmtk_util_convert_bdaddr2array(&remote_addr, bdaddr);
    HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb,
                     BT_STATUS_SUCCESS, &remote_addr, num_properties, properties);
	return num_properties;
}

void btmtk_gap_poweron_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_poweron_cnf_struct *cnf_p = (bt_poweron_cnf_struct*)ilm->ilm_data;

	ALOGI("[GAP] MSG_ID_BT_POWERON_CNF result:[%d]!\n", cnf_p->result);
	if(cnf_p->result == TRUE)
	{
		if(pContext->bt_status == BTMTK_POWER_STATE_ON)
		{
			ALOGI("[GAP] has power on!\n");
			return;
		}
		//btmtk_gap_set_local_name(pContext->host_cache.name, strlen(pContext->host_cache.name));
		//POWER ON success
		btmtk_set_local_feature_mask(pContext, BTMTK_READ_LOCAL_ADDR_MASK, KAL_TRUE);
		
		ALOGI("[GAP] btmtk_gap_get_local_addr");
		pContext->bt_status = BTMTK_POWER_STATE_ON;
		if(!btmtk_get_local_addr())
		{
			ALOGD("[GAP] btmtk_gap_get_local_addr send msg failed!\n");
			btif_enable_bluetooth_evt(BT_STATUS_FAIL);
		}
	}
	else
	{
		pContext->bt_status = BTMTK_POWER_STATE_OFF;
		btif_enable_bluetooth_evt(BT_STATUS_FAIL);
	}
}

void btmtk_gap_read_local_addr_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_read_local_addr_cnf_struct *cnf_p = ( bt_bm_read_local_addr_cnf_struct *)ilm->ilm_data;
	bt_property_t local_properties[6];
	bt_property_t remote_properties[8];
	kal_uint32 service_mask;
	int i = 0;
	ilm_struct msg;

	ALOGI("[GAP] MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF status:[%d], result:[%d], addr:[0x%lX:0x%X:0x%X]!\n", 
		  pContext->bt_status, cnf_p->result, cnf_p->bd_addr.lap, cnf_p->bd_addr.uap, cnf_p->bd_addr.nap);
	if(pContext->bt_status == BTMTK_POWER_STATE_TURNING_OFF || pContext->bt_status == BTMTK_POWER_STATE_OFF)
 	{
 		btmtk_clear_local_feature_mask(pContext);
		return;
 	}

	btmtk_set_local_feature_mask(pContext, BTMTK_READ_LOCAL_ADDR_MASK, KAL_FALSE);
	if(cnf_p->result == BTBM_ADP_SUCCESS)
	{
		int local_feature_mask = btmtk_get_local_feature_mask(pContext);
		
		btmtk_gap_set_local_name(pContext->host_cache.name, strlen(pContext->host_cache.name));
		btmtk_gap_set_local_cod(pContext->local_cod);
		memcpy(&pContext->local_bd_addr, &cnf_p->bd_addr, sizeof(bt_addr_struct));
		memcpy(&pContext->host_cache.local_addr, &cnf_p->bd_addr, sizeof(bt_addr_struct));
		btmtk_host_cache_write();

		pContext->bt_status = BTMTK_POWER_STATE_ON;
		if(local_feature_mask == 0)
		{
			//enable profile service
			service_mask = btif_get_enabled_services_mask();
            for (i=0; i <= BTMTK_MAX_SERVICE_ID; i++)
            {
                if (service_mask & (kal_uint32)(1 <<i))
                {
                    btmtk_execute_service_request(i, KAL_TRUE);
                }
            }
			//enable advance profile service
			btmtk_enable_advance_service();
			//update adapter properties
			btmtk_read_adapter_properties(pContext, local_properties);
			//update remote device properties
			for(i = 0; i < pContext->paired_cache_no; i++)
			{
				btmtk_read_remote_device_properties(pContext, &(pContext->paired_dev_cache[i].addr), remote_properties);
			}
			btmtk_util_load_autopair_device_list();
			btmtk_util_load_dynamic_blacklist();
			
			btif_enable_bluetooth_evt(btmtk_convert_status(cnf_p->result));
		}
	}
	else
	{
	    pContext->bt_status = BTMTK_POWER_STATE_OFF;
		HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_OFF);
	}
}

void btmtk_gap_read_local_uuid_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	ALOGI("[GAP] MSG_ID_BT_BM_READ_LOCAL_UUID_CNF status:[%d]!\n", pContext->bt_status);
}

void btmtk_gap_poweroff_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_poweroff_cnf_struct *cnf_p = ( bt_poweroff_cnf_struct *)ilm->ilm_data;
	ilm_struct msg;
	kal_uint32 service_mask;
	int i = 0;

	ALOGI("[GAP] MSG_ID_BT_POWEROFF_CNF, result:[%d]!\n", cnf_p->result);
	if(cnf_p->result == TRUE)
	{
		pContext->activity = BTMTK_GAP_ACT_NONE;
		while (pContext->requests)
		{
   			btmtk_util_list_remove((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)pContext->requests);
		}

		//disable advance profile service
		btmtk_disable_advance_service();
		//disable profile service
		service_mask = btif_get_enabled_services_mask();
		for (i=0; i <= BTMTK_MAX_SERVICE_ID; i++)
		{
			if (service_mask & (kal_uint32)(1 <<i))
			{
				btmtk_execute_service_request(i, KAL_FALSE);
			}
		}
		btmtk_util_unload_dynamic_blacklist();
		btmtk_util_unload_autopair_device_list();

		//POWER Off success
		pContext->bt_status = BTMTK_POWER_STATE_OFF;
		btif_disable_bluetooth_evt();
	}
	else
	{
		pContext->bt_status = BTMTK_POWER_STATE_OFF;
		HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_ON);
	}
}

void btmtk_gap_discovery_result_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	char *name = NULL;
    bt_bm_discovery_result_ind_struct *ind_p = (bt_bm_discovery_result_ind_struct*)ilm->ilm_data;
    btmtk_device_entry_struct entry;
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
        
    ALOGI("[GAP] MSG_ID_BT_BM_DISCOVERY_RESULT_IND addr:[0x%lX:0x%X:0x%X]", ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap);
    /* Write to cache */
    memset(&entry, 0x0, sizeof(btmtk_device_entry_struct));
    memcpy(&entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
    if (ind_p->name_len != 0)
    {       
        name = (char *)ind_p->name;
   		strncpy(entry.name, name, len);
		entry.name[len] = '\0';
		btmtk_update_device_property_name(pContext, &entry.addr, entry.name, strlen(entry.name));
    }
    if(ind_p->device_type == BTBM_DEVICE_TYPE_LE)
		entry.cod = 0;
	else
    	entry.cod = ind_p->cod;
#ifndef	ANDROID
    if (ind_p->supported_servlist)
    {
        int i, idx = 0;
        for (i = 0; i < BTBM_ADP_MAX_SDAP_UUID_NO && idx < 32 * 6; idx++)
        {
            if (idx < 32)
            {
                if ((ind_p->service_list1 >> idx) & 0x1)
                {
                    entry.sdp.uuid[i] = 0x1100 | idx;
                    ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, entry.sdp.uuid[i]);
                    i++;
                }
            }
            else if (idx < 64)
            {
                int bit = idx - 32;
                if ((ind_p->service_list2 >> bit) & 0x1)
                {
                    entry.sdp.uuid[i] = 0x1120 | bit;
                    ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, entry.sdp.uuid[i]);
                    i++;
                }
            }
            else if (idx < 96)
            {
                int bit = idx - 64;
                if ((ind_p->service_list3 >> bit) & 0x1)
                {
                    entry.sdp.uuid[i] = 0x1200 | bit;
                    ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, entry.sdp.uuid[i]);
                    i++;
                }
            }
            else if (idx < 128)
            {
                int bit = idx - 96;
                if ((ind_p->service_list4 >> bit) & 0x1)
                {
                    entry.sdp.uuid[i] = 0x1300 | bit;
                    ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, entry.sdp.uuid[i]);
                    i++;
                }
            }
            else if (idx < 160)
            {
                int bit = idx - 128;
                if ((ind_p->service_list5 >> bit) & 0x1)
                {
                    entry.sdp.uuid[i] = 0x1400 | bit;
                    ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, entry.sdp.uuid[i]);
                    i++;
                }
            }
            else if (idx < 192)
            {
                int bit = idx - 160;
                if ((ind_p->service_list6 >> bit) & 0x1)
                {
                    entry.sdp.uuid[i] = 0x1800 | bit;
                    ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, entry.sdp.uuid[i]);
                    i++;
                }
            }
        }
        entry.sdp.uuid_no = i;
    }
#endif
	
    entry.legacy_pairing = (ind_p->supported_eir == KAL_FALSE);
    entry.rssi = ind_p->rssi;
    entry.device_type = ind_p->device_type;
	entry.paired = BTMTK_BOND_STATE_UNBOND;
	entry.trusted = KAL_FALSE;
    btmtk_inquired_dev_cache_add(&entry);
	
	//update remote properties
	{
		bt_property_t  prop[6];
		bt_uuid_t      uuids[BTBM_ADP_MAX_SDAP_UUID_NO];
		kal_uint8       num_uuids = 0;
		kal_uint32 num_properties = 0;
		bt_bdaddr_t bdaddr;
		kal_uint32 devtype = btmtk_convert_devtype(entry.device_type);

		//name
		if (NULL != name)
		{
			prop[num_properties].type = BT_PROPERTY_BDNAME;
			prop[num_properties].len = strlen(entry.name);
			prop[num_properties].val = entry.name;
			num_properties ++;
		}

		//addr
		btmtk_util_convert_bdaddr2array(&bdaddr, &(entry.addr)); 
		prop[num_properties].type = BT_PROPERTY_BDADDR;
		prop[num_properties].len = sizeof(bt_bdaddr_t);
		prop[num_properties].val = &bdaddr;
		num_properties ++;

		//cod
		prop[num_properties].type = BT_PROPERTY_CLASS_OF_DEVICE;
		prop[num_properties].len = sizeof(kal_uint32);
		prop[num_properties].val = &(entry.cod);
		num_properties ++;

		//devtype
		prop[num_properties].type = BT_PROPERTY_TYPE_OF_DEVICE;
		prop[num_properties].len = sizeof(kal_uint32);
		prop[num_properties].val = &(devtype);
		num_properties ++;

		//rssi
		prop[num_properties].type = BT_PROPERTY_REMOTE_RSSI;
		prop[num_properties].len = sizeof(kal_int8);
		prop[num_properties].val = &(entry.rssi);
		num_properties ++;

		//
		if (entry.sdp.uuid_no)
		{
			prop[num_properties].type = BT_PROPERTY_UUIDS;
			prop[num_properties].len = entry.sdp.uuid_no*sizeof(bt_uuid_t);
			prop[num_properties].val = uuids;
			for (num_uuids = 0; num_uuids < entry.sdp.uuid_no; num_uuids++)
			{
				btmtk_util_uuid16_to_uuid128(entry.sdp.uuid[num_uuids], &uuids[num_uuids]);
			}
			num_properties ++;
		}

		 /* Callback to notify upper layer of device */
         	HAL_CBACK(bt_hal_cbacks, device_found_cb, num_properties, prop);

		/* Added for BLE device discovery and update icon according to uuid in advertising packet */
		// update remote uuid
		if (entry.sdp.uuid_no > 0)
		{
			bt_property_t  prop;
			bt_bdaddr_t bdaddr;
			
			prop.type = BT_PROPERTY_UUIDS;
			prop.len = entry.sdp.uuid_no*sizeof(bt_uuid_t);
			prop.val = uuids;
			for (num_uuids = 0; num_uuids < entry.sdp.uuid_no; num_uuids++)
			{
				 btmtk_util_uuid16_to_uuid128(entry.sdp.uuid[num_uuids], &uuids[num_uuids]);
			}
			
			btmtk_util_convert_bdaddr2array(&bdaddr, &(ind_p->bd_addr)); 
			HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb, BT_STATUS_SUCCESS, &bdaddr, 1, &prop);
		}
	}
    
}

void btmtk_gap_discovery_update_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_discovery_update_ind_struct *ind_p = (bt_bm_discovery_update_ind_struct*)ilm->ilm_data;
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
	bt_property_t  prop;
	bt_bdaddr_t bdaddr;
	char        name[BTBM_ADP_MAX_NAME_LEN] = {'\0'};

    if(btmtk_inquired_dev_cache_find(&ind_p->bd_addr) != NULL)
    {
		ALOGI("[GAP] MSG_ID_BT_BM_DISCOVERY_UPDATE_IND addr: [0x%lX:0x%X:0x%X]",
			  ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap);
		//name
		if (ind_p->name_len != 0)
	    {       
	   		strncpy(name, ind_p->name, len);
			name[len] = '\0';
			btmtk_update_device_property_name(pContext, &(ind_p->bd_addr), name, strlen(name));

			prop.type = BT_PROPERTY_BDNAME;
			prop.len = strlen(name);
			prop.val = name;

			/* Callback to notify upper layer of device */
			btmtk_util_convert_bdaddr2array(&bdaddr, &(ind_p->bd_addr)); 
		    HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb, BT_STATUS_SUCCESS, &bdaddr, 1, &prop);
	    }
    }
	else
	{
		btmtk_gap_discovery_result_ind(pContext, ilm);
	}
}

void btmtk_gap_discovery_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_discovery_cnf_struct *cnf_p = (bt_bm_discovery_cnf_struct *)ilm->ilm_data;
	ALOGI("[GAP] btmtk_gap_discovery_cnf result:[%d] !\n", cnf_p->result);

	pContext->discovery_cancel_pending = KAL_FALSE;
    pContext->activity = (btmtk_gap_activity_enum)(pContext->activity & ~BTMTK_GAP_ACT_INQUIRY);
    HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb, BT_DISCOVERY_STOPPED);
}

void btmtk_gap_discovery_cancel_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_discovery_cancel_cnf_struct *cnf_p = (bt_bm_discovery_cancel_cnf_struct *)ilm->ilm_data;

	pContext->discovery_cancel_pending = KAL_FALSE;
	pContext->activity = (btmtk_gap_activity_enum)(pContext->activity & ~BTMTK_GAP_ACT_INQUIRY);
    HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb, BT_DISCOVERY_STOPPED);
}

void btmtk_gap_delete_trust_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
    bt_bm_delete_trust_cnf_struct *cnf_p = (bt_bm_delete_trust_cnf_struct*)ilm->ilm_data;
	bt_bdaddr_t bdaddr;
	int name_len = 0;
	
    ALOGI("[GAP] MSG_ID_BT_BM_DELETE_TRUST_CNF result: [%d], addr:[0x%lX:0x%X:0x%X]!\n", 
		  cnf_p->result, cnf_p->bd_addr.lap, cnf_p->bd_addr.uap, cnf_p->bd_addr.nap);
	
    if(cnf_p->result == BTBM_ADP_SUCCESS)
  	{
  	    /* Update cache */
  	    btmtk_device_entry_struct *inquired_entry = btmtk_inquired_dev_cache_find(&cnf_p->bd_addr);
		btmtk_device_entry_struct *paired_entry = btmtk_paired_dev_cache_find(&cnf_p->bd_addr);;
		
        if (inquired_entry)
        {
            inquired_entry->paired = BTMTK_BOND_STATE_UNBOND;
            inquired_entry->trusted = KAL_TRUE;
        }
		else if (paired_entry) 
		{
		    paired_entry->paired = BTMTK_BOND_STATE_UNBOND;
	        paired_entry->trusted = KAL_FALSE;
			inquired_entry = btmtk_inquired_dev_cache_add(paired_entry);
		}
		btmtk_paired_dev_cache_del(&cnf_p->bd_addr);
		btmtk_util_convert_bdaddr2array(&bdaddr, &cnf_p->bd_addr);
		if (inquired_entry)
		{
			name_len = strlen(inquired_entry->name);
			strncpy(inquired_entry->nickname, inquired_entry->name, name_len);
			inquired_entry->nickname[name_len] = '\0';
			
			if(inquired_entry->nickname[0] != 0)
			{
			    bt_bdname_t name;
		        bt_property_t property;
				
			    strncpy(name.name, inquired_entry->nickname, BTBM_ADP_MAX_NAME_LEN-1);
	        	property.type = BT_PROPERTY_REMOTE_FRIENDLY_NAME;
		        property.len = strlen((char *)name.name);
		        property.val = &name;

				HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb,
	                       BT_STATUS_SUCCESS, &bdaddr, 1, &property);
			}
		}
		
        HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, btmtk_convert_status(cnf_p->result), &bdaddr, BT_BOND_STATE_NONE);
    }
}

void btmtk_gap_bonding_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
    bt_bm_bonding_cnf_struct *cnf_p = (bt_bm_bonding_cnf_struct*)ilm->ilm_data;
	bt_bdaddr_t bdaddr;
	bt_bond_state_t state = BT_BOND_STATE_NONE;
	btmtk_bond_req_struct *req = btmtk_util_find_bond_request(&cnf_p->bd_addr);;
	btmtk_device_entry_struct *inquiry_entry = btmtk_inquired_dev_cache_find(&cnf_p->bd_addr);

    ALOGI("[GAP] MSG_ID_BT_BM_BONDING_CNF result:[%d]", cnf_p->result);
	btmtk_util_convert_bdaddr2array(&bdaddr, &cnf_p->bd_addr);
    if(cnf_p->result == BTBM_ADP_SUCCESS)
    {
    	state = BT_BOND_STATE_BONDED;
		if(inquiry_entry != NULL)
		{
			inquiry_entry->paired = BTMTK_BOND_STATE_BONDED;
			inquiry_entry->trusted = KAL_TRUE;
			inquiry_entry->nickname[0]='\0';
			btmtk_paired_dev_cache_add(inquiry_entry);
		}
		pContext->bond_state = BTMTK_BOND_STATE_UNBOND;
		HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, state);
    }
    else
    {
    	ALOGI("[GAP] Bonding failed");
		if(inquiry_entry != NULL)
		{
			inquiry_entry->paired = BTMTK_BOND_STATE_UNBOND;
			inquiry_entry->trusted = KAL_FALSE;
		}
		if(req != NULL && req->autopair_attempts == 2)
		{
			ALOGI("[GAP] btmtk_gap_bonding_cnf auto pair failed doesn't callback!\n");
		}
		else
		{
			if(req != NULL)
			{
				btmtk_util_list_remove((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
			}
			pContext->bond_state = BTMTK_BOND_STATE_UNBOND;
			HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_AUTH_FAILURE, &bdaddr, state);
		}
    }
	
}

void btmtk_gap_pin_code_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
    bt_bm_pin_code_ind_struct *ind_p = (bt_bm_pin_code_ind_struct*)ilm->ilm_data;
	btmtk_bond_req_struct *req;
    btmtk_device_entry_struct *entry;
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
	bt_bdaddr_t bdaddr;
	bt_bdname_t bdname;
	kal_uint8   pin[4];

    ALOGI("[GAP] MSG_ID_BT_BM_PIN_CODE_IND addr:[0x%lX:0x%X:0x%X], cod:[%lX]", 
		  ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap, ind_p->cod);
	if ((entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr)) == NULL)
    {
		btmtk_device_entry_struct inquired_entry;
		
		ALOGI("[GAP] remote initiate pairing");
		/* Add to inquired cache so that following handling consistent with bonding */
		memset(&inquired_entry, 0x0, sizeof(btmtk_device_entry_struct));
		memcpy(&inquired_entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		if (ind_p->name_len > 0)
		{
			strncpy(inquired_entry.name, ind_p->name, len);
			inquired_entry.name[len] = '\0';
		}
		inquired_entry.legacy_pairing = KAL_TRUE;
		inquired_entry.paired = BTMTK_BOND_STATE_BONDING;
		entry = btmtk_inquired_dev_cache_add(&inquired_entry);
    }
	else
	{
		if (ind_p->name_len > 0)
		{
			strncpy(entry->name, ind_p->name, len);
			entry->name[len] = '\0';
		}
		entry->legacy_pairing = KAL_TRUE;
		entry->paired = BTMTK_BOND_STATE_BONDING;
	}
	if(ind_p->cod != 0)
		entry->cod = ind_p->cod;
	btmtk_update_remote_properties(&entry->addr, (char *)entry->name, entry->cod, BTBM_DEVICE_TYPE_BR_EDR);
		
	btmtk_util_convert_bdaddr2array(&bdaddr, &ind_p->bd_addr);
	HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_BONDING);
	
	req = btmtk_util_find_bond_request(&ind_p->bd_addr);
	if(req != NULL && req->activity == BTMTK_BOND_ACTIVITY_BONDING)
	{
		//local initial bond
		req->auth_type = BTMTK_AUTH_TYPE_PIN_CODE;
		if(((ind_p->cod & 0x7ff) == COD_AV_HEADSETS) ||
		   ((ind_p->cod & 0x7ff) == COD_AV_HANDSFREE) ||
		   ((ind_p->cod & 0x7ff) == COD_AV_HEADPHONES) ||
		   ((ind_p->cod & 0x7ff) == COD_AV_PORTABLE_AUDIO) ||
		   ((ind_p->cod & 0x7ff) == COD_AV_HIFI_AUDIO) ||
		   ((ind_p->cod & 0x7ff) == COD_HID_POINTING))
        {
            ALOGI("[GAP] MSG_ID_BT_BM_PIN_CODE_IND 1 cod matches for auto pair");
            /*  Check if this device can be auto paired  */
            if ((btmtk_is_device_autopair_blacklisted(pContext, &ind_p->bd_addr) == KAL_FALSE) &&
                (req->autopair_attempts == 0))
            {
                ALOGI("[GAP] MSG_ID_BT_BM_PIN_CODE_IND Attempting auto pair");
                pin[0] = 0x30;
                pin[1] = 0x30;
                pin[2] = 0x30;
                pin[3] = 0x30;

                req->autopair_attempts++;
				btmtk_gap_pin_reply(&ind_p->bd_addr, KAL_TRUE, 4, pin);
                return;
            }
        }
        else if(((ind_p->cod & 0x7ff) == COD_HID_KEYBOARD) ||
		   	    ((ind_p->cod & 0x7ff) == COD_HID_COMBO))
        {
        	ALOGI("[GAP] MSG_ID_BT_BM_PIN_CODE_IND 2 cod matches for auto pair");
            if(( btmtk_is_fixed_pin_zeros_keyboard(pContext, &ind_p->bd_addr) == KAL_TRUE) &&
               (req->autopair_attempts == 0))
            {
                ALOGI("[GAP] MSG_ID_BT_BM_PIN_CODE_IND Attempting auto pair");
                pin[0] = 0x30;
                pin[1] = 0x30;
                pin[2] = 0x30;
                pin[3] = 0x30;

                req->autopair_attempts++;
                btmtk_gap_pin_reply(&ind_p->bd_addr, KAL_TRUE, 4, pin);
                return;
            }
        }
		
	}
	else
	{
		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
		memcpy(&req->addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		btmtk_util_list_append((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		req->autopair_attempts = 0;
		req->activity = BTMTK_BOND_ACTIVITY_NONE;
		req->auth_type = BTMTK_AUTH_TYPE_PIN_CODE;
	}
		
	memcpy(bdname.name, entry->name, 248);
	HAL_CBACK(bt_hal_cbacks, pin_request_cb, &bdaddr, &bdname, entry->cod);
}

void btmtk_gap_security_user_confirm_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
    uint32_t number;
    bt_bm_security_user_confirm_ind_struct *ind_p = (bt_bm_security_user_confirm_ind_struct*)ilm->ilm_data;
	btmtk_bond_req_struct *req;
	btmtk_device_entry_struct *entry;
	kal_uint8 numeric[7];
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
	bt_bdaddr_t bdaddr;
	bt_bdname_t bdname;

	memset(numeric, 0x0, 7);
	memcpy(numeric, ind_p->numeric, 6);
    number = atoi((const char*)numeric);
    ALOGI("[GAP] MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND addr:[0x%lX:0x%X:0x%X], number=%u, display=%d", 
		  ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap, number, ind_p->display_numeric);
	if ((entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr)) == NULL)
    {
		btmtk_device_entry_struct inquired_entry;
		
		ALOGI("[GAP] remote initiate pairing");
		/* Add to inquired cache so that following handling consistent with bonding */
		memset(&inquired_entry, 0x0, sizeof(btmtk_device_entry_struct));
		memcpy(&inquired_entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		if (ind_p->name_len > 0)
		{
			strncpy(inquired_entry.name, ind_p->name, len);
			inquired_entry.name[len] = '\0';
		}
		inquired_entry.legacy_pairing = KAL_TRUE;
		inquired_entry.paired = BTMTK_BOND_STATE_BONDING;
		entry = btmtk_inquired_dev_cache_add(&inquired_entry);
    }
	else
	{
		if (ind_p->name_len > 0)
		{
			strncpy(entry->name, ind_p->name, len);
			entry->name[len] = '\0';
		}
		entry->legacy_pairing = KAL_TRUE;
		entry->paired = BTMTK_BOND_STATE_BONDING;
	}
	if(ind_p->cod != 0)
		entry->cod = ind_p->cod;
    btmtk_update_remote_properties(&entry->addr, (char *)entry->name, entry->cod, BTBM_DEVICE_TYPE_BR_EDR);
	
	btmtk_util_convert_bdaddr2array(&bdaddr, &ind_p->bd_addr);
	HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_BONDING);

	req = btmtk_util_find_bond_request(&ind_p->bd_addr);
	if(req != NULL && req->activity == BTMTK_BOND_ACTIVITY_BONDING)
	{
		//local initial bond
		req->auth_type = BTMTK_AUTH_TYPE_PASSKEY_CONFIRMATION;
	}
	else
	{
		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
		memcpy(&req->addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		btmtk_util_list_append((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		req->autopair_attempts = 0;
		req->activity = BTMTK_BOND_ACTIVITY_NONE;
		req->auth_type = BTMTK_AUTH_TYPE_PASSKEY_CONFIRMATION;
	}
	
	memcpy(bdname.name, entry->name, 248);
	HAL_CBACK(bt_hal_cbacks, ssp_request_cb, &bdaddr, &bdname, entry->cod,
                     (ind_p->display_numeric ? BT_SSP_VARIANT_PASSKEY_CONFIRMATION: BT_SSP_VARIANT_CONSENT ),
                     number);
}

void btmtk_gap_security_passkey_entry_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
    bt_bm_security_passkey_entry_ind_struct *ind_p = (bt_bm_security_passkey_entry_ind_struct*)ilm->ilm_data;
	btmtk_bond_req_struct *req;
	btmtk_device_entry_struct *entry;
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
	bt_bdaddr_t bdaddr;
	bt_bdname_t bdname;

    ALOGI("[GAP] MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_IND addr:[0x%lX:0x%X:0x%X]",
		  ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap);
	if ((entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr)) == NULL)
    {
		btmtk_device_entry_struct inquired_entry;
		
		ALOGI("[GAP] remote initiate pairing");
		/* Add to inquired cache so that following handling consistent with bonding */
		memset(&inquired_entry, 0x0, sizeof(btmtk_device_entry_struct));
		memcpy(&inquired_entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		if (ind_p->name_len > 0)
		{
			strncpy(inquired_entry.name, ind_p->name, len);
			inquired_entry.name[len] = '\0';
		}
		inquired_entry.legacy_pairing = KAL_TRUE;
		inquired_entry.paired = BTMTK_BOND_STATE_BONDING;
		entry = btmtk_inquired_dev_cache_add(&inquired_entry);
    }
	else
	{
		if (ind_p->name_len > 0)
		{
			strncpy(entry->name, ind_p->name, len);
			entry->name[len] = '\0';
		}
		entry->legacy_pairing = KAL_TRUE;
		entry->paired = BTMTK_BOND_STATE_BONDING;
	}
	if(ind_p->cod != 0)
		entry->cod = ind_p->cod;
	btmtk_update_remote_properties(&entry->addr, (char *)entry->name, entry->cod, BTBM_DEVICE_TYPE_BR_EDR);
	
	btmtk_util_convert_bdaddr2array(&bdaddr, &ind_p->bd_addr);
	HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_BONDING);

	req = btmtk_util_find_bond_request(&ind_p->bd_addr);
	if(req != NULL && req->activity == BTMTK_BOND_ACTIVITY_BONDING)
	{
		//local initial bond
		req->auth_type = BTMTK_AUTH_TYPE_PASSKEY_ENTRY;
	}
	else
	{
		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
		memcpy(&req->addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		btmtk_util_list_append((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		req->autopair_attempts = 0;
		req->activity = BTMTK_BOND_ACTIVITY_NONE;
		req->auth_type = BTMTK_AUTH_TYPE_PASSKEY_ENTRY;
	}
		
	memcpy(bdname.name, entry->name, 248);
	HAL_CBACK(bt_hal_cbacks, ssp_request_cb, &bdaddr, &bdname, entry->cod,
                     BT_SSP_VARIANT_PASSKEY_ENTRY,
                     0);
}

void btmtk_gap_security_passkey_notify_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
    bt_bm_security_passkey_notify_ind_struct *ind_p = (bt_bm_security_passkey_notify_ind_struct*)ilm->ilm_data;
	btmtk_bond_req_struct *req;
	btmtk_device_entry_struct *entry;
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
	bt_bdaddr_t bdaddr;
	bt_bdname_t bdname;

    ALOGI("[GAP] MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND addr:[0x%lX:0x%X:0x%X], passkey=%ld",
		  ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap, ind_p->passkey);
	if ((entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr)) == NULL)
    {
		btmtk_device_entry_struct inquired_entry;
		
		ALOGI("[GAP] remote initiate pairing");
		/* Add to inquired cache so that following handling consistent with bonding */
		memset(&inquired_entry, 0x0, sizeof(btmtk_device_entry_struct));
		memcpy(&inquired_entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		if (ind_p->name_len > 0)
		{
			strncpy(inquired_entry.name, ind_p->name, len);
			inquired_entry.name[len] = '\0';
		}
		inquired_entry.legacy_pairing = KAL_TRUE;
		inquired_entry.paired = BTMTK_BOND_STATE_BONDING;
		entry = btmtk_inquired_dev_cache_add(&inquired_entry);
    }
	else
	{
		if (ind_p->name_len > 0)
		{
			strncpy(entry->name, ind_p->name, len);
			entry->name[len] = '\0';
		}
		entry->legacy_pairing = KAL_TRUE;
		entry->paired = BTMTK_BOND_STATE_BONDING;
	}
	if(ind_p->cod != 0)
		entry->cod = ind_p->cod;
	btmtk_update_remote_properties(&entry->addr, (char *)entry->name, entry->cod, BTBM_DEVICE_TYPE_BR_EDR);
	
	btmtk_util_convert_bdaddr2array(&bdaddr, &ind_p->bd_addr);
	HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_BONDING);

	req = btmtk_util_find_bond_request(&ind_p->bd_addr);
	if(req != NULL && req->activity == BTMTK_BOND_ACTIVITY_BONDING)
	{
		//local initial bond
		req->auth_type = BTMTK_AUTH_TYPE_PASSKEY_NOTIFICATION;
	}
	else
	{
		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
		memcpy(&req->addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		btmtk_util_list_append((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		req->autopair_attempts = 0;
		req->activity = BTMTK_BOND_ACTIVITY_NONE;
		req->auth_type = BTMTK_AUTH_TYPE_PASSKEY_NOTIFICATION;
	}
	
	memcpy(bdname.name, entry->name, 248);
	HAL_CBACK(bt_hal_cbacks, ssp_request_cb, &bdaddr, &bdname, entry->cod,
                     BT_SSP_VARIANT_PASSKEY_NOTIFICATION,
                     ind_p->passkey);
}


void btmtk_gap_bonding_result_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bdaddr_t bdaddr;
    bt_bm_bonding_result_ind_struct *ind_p = (bt_bm_bonding_result_ind_struct*)ilm->ilm_data;
    btmtk_bond_req_struct *req;
    
    ALOGI("[GAP] MSG_ID_BT_BM_BONDING_RESULT_IND addr:[0x%lX:0x%X:0x%X], result=%d, cod:[%lX], devtype:[%d]", 
		  ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap, ind_p->result, ind_p->cod, ind_p->device_type);
	req = btmtk_util_find_bond_request(&ind_p->bd_addr);
	if(req == NULL)
	{
		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
		memcpy(&req->addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
		btmtk_util_list_append((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		req->autopair_attempts = 0;
		req->activity = BTMTK_BOND_ACTIVITY_NONE;
		req->auth_type = BTMTK_AUTH_TYPE_NONE;
	}
	btmtk_util_convert_bdaddr2array(&bdaddr, &ind_p->bd_addr);
    if(ind_p->result == BTBM_ADP_SUCCESS)
	{
        btmtk_device_entry_struct entry, *inquired_entry;
		
        memset(&entry, 0x0, sizeof(btmtk_device_entry_struct));
        inquired_entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr);
        if (inquired_entry)
        {
    		//inquired_entry->cod = ind_p->cod;
			if(ind_p->cod != 0)
				inquired_entry->cod = ind_p->cod;
			inquired_entry->device_type = ind_p->device_type;
        	inquired_entry->paired = BTMTK_BOND_STATE_BONDED;
			inquired_entry->trusted = KAL_TRUE;
	    }
    	else
        {
        	memcpy(&entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
			entry.cod = ind_p->cod;
			entry.device_type = ind_p->device_type;
			entry.paired = BTMTK_BOND_STATE_BONDED;
			entry.trusted = KAL_TRUE;
			inquired_entry = btmtk_inquired_dev_cache_add(&entry);
        }
		inquired_entry->nickname[0]='\0';
        btmtk_paired_dev_cache_add(inquired_entry);

		if(req->activity == BTMTK_BOND_ACTIVITY_BONDING)
		{
			/* local initial bond */
            /* wait for SDP result */
            ALOGI("[GAP] btmtk_gap_handle_bonding_result_ind wait for SDP result");
			req->auth_type = BTMTK_AUTH_TYPE_NONE;
			req->activity = BTMTK_BOND_ACTIVITY_SDP;
            inquired_entry->sdp.uuid_no = 0;
            memset(inquired_entry->sdp.uuid, 0x0, BTBM_ADP_MAX_SDAP_UUID_NO * sizeof(unsigned short));
            return;
		}
		else
		{
			ALOGI("[GAP] entry.device_type = %d!\n", inquired_entry->device_type);
			if (inquired_entry->device_type != BTBM_DEVICE_TYPE_LE)
            {
            	req->auth_type = BTMTK_AUTH_TYPE_NONE;
				req->activity = BTMTK_BOND_ACTIVITY_SDP;
				btmtk_gap_get_remote_services(&ind_p->bd_addr);
            }
			pContext->bond_state = BTMTK_BOND_STATE_UNBOND;
			HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_BONDED);
		}
    }
	else
	{
		btmtk_device_entry_struct entry, *inquired_entry;

		memset(&entry, 0x0, sizeof(btmtk_device_entry_struct));
		inquired_entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr);
        if (inquired_entry) 
	    {
	    	//inquired_entry->cod = ind_p->cod;
			//inquired_entry->device_type = ind_p->device_type;
			if(ind_p->cod != 0)
				inquired_entry->cod = ind_p->cod;
			inquired_entry->paired = BTMTK_BOND_STATE_UNBOND;
            inquired_entry->trusted = KAL_FALSE;
	    }
		else
		{
			memcpy(&entry.addr, &ind_p->bd_addr, sizeof(bt_addr_struct));
			entry.cod = ind_p->cod;
			entry.device_type = ind_p->device_type;
			entry.paired = BTMTK_BOND_STATE_UNBOND;
			entry.trusted = KAL_FALSE;
			inquired_entry = btmtk_inquired_dev_cache_add(&entry);
		}
		btmtk_paired_dev_cache_del(&ind_p->bd_addr);
		if(req->autopair_attempts == 1 && req->activity == BTMTK_BOND_ACTIVITY_BONDING)
		{
			if(((inquired_entry->cod & 0x7ff) == COD_AV_HEADSETS) ||
			   ((inquired_entry->cod & 0x7ff) == COD_AV_HANDSFREE) ||
			   ((inquired_entry->cod & 0x7ff) == COD_AV_HEADPHONES) ||
			   ((inquired_entry->cod & 0x7ff) == COD_AV_PORTABLE_AUDIO) ||
			   ((inquired_entry->cod & 0x7ff) == COD_AV_HIFI_AUDIO) ||
			   ((inquired_entry->cod & 0x7ff) == COD_HID_POINTING))
			{
				btmtk_util_add_device_to_dynamic_blacklist(&ind_p->bd_addr);
			}
			//local initial bond failed
			req->autopair_attempts ++;
			/* Create the Bond once again */
            ALOGI("btmtk_gap_handle_bonding_result_ind auto pair failed. Reinitiate Bond");
			btmtk_gap_rebond(&ind_p->bd_addr);
			return;
		}
		else
		{
			btmtk_util_list_remove((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		}
		pContext->bond_state = BTMTK_BOND_STATE_UNBOND;
		HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_AUTH_FAILURE, &bdaddr, BT_BOND_STATE_NONE);
	}
}

void btmtk_gap_write_local_name_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_write_local_name_cnf_struct *cnf_p = (bt_bm_write_local_name_cnf_struct*)ilm->ilm_data;
	bt_property_t property;

	ALOGI("[GAP] MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF result=%d", cnf_p->result);
	if(cnf_p->result == BTBM_ADP_SUCCESS)
	{ 
        btmtk_host_cache_write();

		property.type = BT_PROPERTY_BDNAME;
		property.len = strlen(pContext->host_cache.name);
		property.val = pContext->host_cache.name;

		HAL_CBACK(bt_hal_cbacks, adapter_properties_cb, BT_STATUS_SUCCESS, 1, &property);
	}
}

void btmtk_gap_write_scanenable_mode_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_write_scanenable_mode_cnf_struct *cnf_p = (bt_bm_write_scanenable_mode_cnf_struct*)ilm->ilm_data;
	bt_property_t properties[2];
	int num_properties = 0;
	
	ALOGI("[GAP] MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF result=%d", cnf_p->result);
	if(cnf_p->result == BTBM_ADP_SUCCESS)
	{ 
		bt_scan_mode_t mode = btmtk_convert_scanmode(pContext->host_cache.scan_mode);
		kal_uint32 disc_timeout = pContext->host_cache.scan_mode_timeout;
        btmtk_host_cache_write(pContext);

		properties[num_properties].type = BT_PROPERTY_ADAPTER_SCAN_MODE;
		properties[num_properties].len = sizeof(mode);
		properties[num_properties].val = &mode;
		num_properties++;
		
		properties[num_properties].type = BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT;
		properties[num_properties].len = sizeof(disc_timeout);
		properties[num_properties].val = &disc_timeout;
		num_properties++;
		
        HAL_CBACK(bt_hal_cbacks, adapter_properties_cb, BT_STATUS_SUCCESS, num_properties, properties);
	}
}

void btmtk_gap_bonding_cancel_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_bonding_cancel_cnf_struct *cnf_p = (bt_bm_bonding_cancel_cnf_struct*)ilm->ilm_data;
	btmtk_device_entry_struct entry, *inquired_entry;
	btmtk_bond_req_struct *req;
	bt_bdaddr_t bdaddr;
	
	ALOGI("[GAP] MSG_ID_BT_BM_BONDING_CANCEL_CNF result=%d", cnf_p->result);
	btmtk_util_convert_bdaddr2array(&bdaddr, &cnf_p->bd_addr);
	if(cnf_p->result == BTBM_ADP_SUCCESS)
	{	
		memset(&entry, 0x0, sizeof(btmtk_device_entry_struct));
		inquired_entry = btmtk_inquired_dev_cache_find(&cnf_p->bd_addr);
		if(inquired_entry)
		{
			inquired_entry->paired = BTMTK_BOND_STATE_UNBOND;
            inquired_entry->trusted = KAL_FALSE;
	    }
		else
		{
			memcpy(&entry.addr, &cnf_p->bd_addr, sizeof(bt_addr_struct));
			entry.paired = BTMTK_BOND_STATE_UNBOND;
			entry.trusted = KAL_FALSE;
			inquired_entry = btmtk_inquired_dev_cache_add(&entry);
		}
	    btmtk_paired_dev_cache_del(&cnf_p->bd_addr);

		req = btmtk_util_find_bond_request(&cnf_p->bd_addr);
		if(req != NULL)
		{
			btmtk_util_list_remove((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
		}
	}
	pContext->bond_state = BTMTK_BOND_STATE_UNBOND;
	HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_AUTH_FAILURE, &bdaddr, BT_BOND_STATE_NONE);
}

void btmtk_gap_service_search_result_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_service_search_result_ind_struct *ind_p = (bt_bm_service_search_result_ind_struct *)ilm->ilm_data;
    btmtk_bond_req_struct *req;
    
    ALOGI("[GAP] MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND : uuid=0x%lX", ind_p->uuid);
    req = btmtk_util_find_bond_request(&ind_p->bd_addr);
    if (req && req->uuid_no < BTBM_ADP_MAX_SDAP_UUID_NO)
    {
        req->uuid[req->uuid_no++] = (unsigned short)ind_p->uuid;
        ALOGI("[GAP] MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND : uuid_no=%d", req->uuid_no);
    }
}

void btmtk_gap_service_search_cnf(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_service_search_cnf_struct *cnf_p = (bt_bm_service_search_cnf_struct *)ilm->ilm_data;
    btmtk_bond_req_struct *req;
	bt_bdaddr_t bdaddr;
	kal_uint32 devtype;
	bt_property_t properties[8];
	int num_properties = 0;
    int i;

    ALOGI("[GAP] MSG_ID_BT_BM_SERVICE_SEARCH_CNF: addr=[0x%lX:0x%X:0x%X], result=%d, searched=%d", 
         cnf_p->bd_addr.lap, cnf_p->bd_addr.uap, cnf_p->bd_addr.nap, cnf_p->result, cnf_p->searched_number);

    btmtk_util_convert_bdaddr2array(&bdaddr, &cnf_p->bd_addr);
    req = btmtk_util_find_bond_request(&cnf_p->bd_addr);
    if (req == NULL)
    {
        ALOGE("[GAP] btmtk_gap_handle_service_search_cnf no sdp request found");
        return;
    }
    ALOGI("[GAP] btmtk_gap_handle_service_search_cnf sdp req type=%d, uuid_no=%d", req->activity, req->uuid_no);
	/* write to FS */
    if (cnf_p->result == BTBM_ADP_SUCCESS)
    {
        btmtk_device_entry_struct *p_entry = btmtk_paired_dev_cache_find(&cnf_p->bd_addr);
        btmtk_device_entry_struct *i_entry = btmtk_inquired_dev_cache_find(&cnf_p->bd_addr);
        if (p_entry)
        {
            p_entry->sdp.uuid_no = req->uuid_no;
            memcpy(p_entry->sdp.uuid, req->uuid, BTBM_ADP_MAX_SDAP_UUID_NO * sizeof(unsigned short));
            btmtk_paired_dev_cache_write();
        }
        if (i_entry)
        {
            i_entry->sdp.uuid_no = req->uuid_no;
            memcpy(i_entry->sdp.uuid, req->uuid, BTBM_ADP_MAX_SDAP_UUID_NO * sizeof(unsigned short));
			if(i_entry->name[0] != 0)
			{
				properties[num_properties].type = BT_PROPERTY_BDNAME;
				properties[num_properties].val = i_entry->name;
				properties[num_properties].len = strlen(i_entry->name);
				num_properties ++;
			}

			properties[num_properties].type = BT_PROPERTY_CLASS_OF_DEVICE;
			properties[num_properties].val = &(i_entry->cod);
			properties[num_properties].len = sizeof(kal_uint32);
			num_properties ++;

			devtype = btmtk_convert_devtype(i_entry->device_type);
			properties[num_properties].type = BT_PROPERTY_TYPE_OF_DEVICE;
			properties[num_properties].val = &devtype;
			properties[num_properties].len = sizeof(kal_uint32);
			num_properties ++;
		}

		
		if (req->uuid_no)
		{
			bt_uuid_t  remote_uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
			unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	
			for (i = 0; i < req->uuid_no; i++)
			{
				int uuid128[16/4];
				if (req->uuid[i] == 0x2001) 
				{
					memcpy(remote_uuid[i].uu, SDP_INSECURE, 16);
				} 
				else if (req->uuid[i] == 0x2002)
				{
					memcpy((char*)uuid128, SDP_SECURE, 16);
				}  
				else 
				{
					btmtk_util_uuid16_to_uuid128(req->uuid[i], &remote_uuid[i]);
				}
				ALOGI("[GAP] btmtk_gap_handle_service_search_cnf uuid[%d]=%x", i, req->uuid[i]);
			}
			properties[num_properties].type = BT_PROPERTY_UUIDS;
			properties[num_properties].val = remote_uuid;
			properties[num_properties].len = req->uuid_no*sizeof(bt_uuid_t);
			num_properties ++;
		}

		HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb,
                             BT_STATUS_SUCCESS, &bdaddr, num_properties, properties);
    }
    btmtk_util_list_remove((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
}

void btmtk_gap_remote_device_name_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	
	bt_bm_remote_name_ind_struct *ind_p = (bt_bm_remote_name_ind_struct*)ilm->ilm_data;
	int len = (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN) ? (BTBM_ADP_MAX_NAME_LEN-1) : ind_p->name_len;
	bt_property_t  prop;
	bt_bdaddr_t bdaddr;
	char        name[BTBM_ADP_MAX_NAME_LEN] = {'\0'};
	
	ALOGI("[GAP] btmtk_gap_remote_device_name_ind addr: [0x%lX:0x%X:0x%X]", ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap);
	if (ind_p->name_len != 0)
    {       
   		strncpy(name, ind_p->name, len);
		name[len] ='\0';
		btmtk_update_device_property_name(pContext, &(ind_p->bd_addr), name, strlen(name));

		prop.type = BT_PROPERTY_BDNAME;
		prop.len = strlen(name);
		prop.val = name;

		/* Callback to notify upper layer of device */
		btmtk_util_convert_bdaddr2array(&bdaddr, &(ind_p->bd_addr)); 
	    HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb, BT_STATUS_SUCCESS, &bdaddr, 1, &prop);
    }
}

static void btmtk_acl_disc_timeout_handler(void *data)
{
	bt_gap_context_t *pContext = (bt_gap_context_t *)data;

	ALOGI("+++[GAP] btmtk_acl_disc_timeout_handler+++!\n");
	timer_Stop(&pContext->disc_timer);
	pContext->disc_acl_action = KAL_FALSE;
	pContext->disc_timer.load = 0;
	btmtk_gap_power_off();
	ALOGI("---[GAP] btmtk_acl_disc_timeout_handler---!\n");
}

static void btmtk_disable_timeout_handler(void *data)
{
	bt_gap_context_t *pContext = (bt_gap_context_t *)data;

	ALOGI("+++[GAP] btmtk_disable_timeout_handler+++!\n");
	timer_Stop(&pContext->disc_timer);
	pContext->disc_timer.load = 0;
	btmtk_gap_power_off();
	ALOGI("---[GAP] btmtk_disable_timeout_handler---!\n");
}


static void btmtk_profile_disc_timeout_handler(void *data)
{
	bt_gap_context_t *pContext = (bt_gap_context_t *)data;

	ALOGI("+++[GAP] btmtk_profile_disc_timeout_handler conn_no:[%ld] +++!\n", pContext->conn_no);
	timer_Stop(&pContext->disc_timer);
	pContext->disc_acl_action = KAL_FALSE;
	if(pContext->conn_no > 0)
	{
		pContext->disc_timer.load = (5*SECTICKS)/10;
		pContext->disc_timer.func = btmtk_acl_disc_timeout_handler;
		pContext->disc_timer.name = "acl disconnect";
		pContext->disc_timer.arg = pContext;
		timer_Start(&pContext->disc_timer);
		
		btmtk_gap_acl_disconnect();
	}
	else
	{
		pContext->disc_timer.load = 0;
		btmtk_gap_power_off();
	}
	ALOGI("---[GAP] btmtk_profile_disc_timeout_handler---!\n");
}

void btmtk_gap_link_state_ind(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	bt_bm_link_state_ind_struct *ind_p = (bt_bm_link_state_ind_struct*)ilm->ilm_data;;
	bt_bdaddr_t remote_addr;
	kal_bool connected = KAL_FALSE;
	
    ALOGI("[GAP] MSG_ID_BT_BM_LINK_STATE_IND linkno=%d addr=[0x%lX:0x%X:0x%X]", 
		  ind_p->current_number, ind_p->bd_addr.lap, ind_p->bd_addr.uap, ind_p->bd_addr.nap);
	btmtk_util_convert_bdaddr2array(&remote_addr, &ind_p->bd_addr);
	if (pContext->conn_no != ind_p->current_number)
    {
        btmtk_device_entry_struct *p_entry = btmtk_paired_dev_cache_find(&ind_p->bd_addr);
        btmtk_device_entry_struct *i_entry = btmtk_inquired_dev_cache_find(&ind_p->bd_addr);

        if (pContext->conn_no > ind_p->current_number)
        {
            pContext->conn_no--;
			connected = KAL_FALSE;
        }
        else
        {
            pContext->conn_no++;
			connected = KAL_TRUE;
        }

		if (p_entry)
        {
            p_entry->connected = connected;
        }
        if (i_entry)
        {
            i_entry->connected = connected;
        }

		HAL_CBACK(bt_hal_cbacks, acl_state_changed_cb, BT_STATUS_SUCCESS,
                      &remote_addr, connected? BT_ACL_STATE_CONNECTED : BT_ACL_STATE_DISCONNECTED);
    }

	if(pContext->disc_acl_action == KAL_TRUE && pContext->conn_no == 0)
	{
		ALOGI("[GAP] start timer 1000ms wait profile callback disc event!\n");
		timer_Stop(&pContext->disc_timer);
		pContext->disc_acl_action = KAL_FALSE;
		btmtk_gap_power_off();
		#if 0
		pContext->disc_timer.load = 1*SECTICKS;
		pContext->disc_timer.func = btmtk_disable_timeout_handler;
		pContext->disc_timer.name = "disable";
		pContext->disc_timer.arg = pContext;
		timer_Start(&pContext->disc_timer);
		#endif
	}
}

#ifdef DETECT_BTUSB_RESUME
void btmtk_gap_reset_req_ind(bt_gap_context_t *pContext, 
	                              ilm_struct *ilm)
{
	ALOGI("[GAP] btmtk_gap_reset_req_ind");
	if(pContext->activity&BTMTK_GAP_ACT_INQUIRY)
    {
        pContext->discovery_cancel_pending = KAL_FALSE;
        pContext->activity = (btmtk_gap_activity_enum)(pContext->activity & ~BTMTK_GAP_ACT_INQUIRY);
        HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb, BT_DISCOVERY_STOPPED);
	}
#ifndef MTK_BSP_PACKAGE
	HAL_CBACK(bt_hal_cbacks, adapter_reset_ind_cb);
#endif
}
#endif

void btmtk_ev_disable_cb(bt_gap_context_t *pContext, 
	                         ilm_struct *ilm)
{
	ALOGI("[GAP] btmtk_ev_disable_cb conn_no:[%ld] !\n", pContext->conn_no);
	if(pContext->conn_no > 0)
	{
		timer_Stop(&pContext->disc_timer);
		pContext->disc_acl_action = KAL_TRUE;
		pContext->disc_timer.load = 2*SECTICKS;
		pContext->disc_timer.func = btmtk_profile_disc_timeout_handler;
		pContext->disc_timer.name = "profile disconnect";
		pContext->disc_timer.arg = pContext;
		timer_Start(&pContext->disc_timer);

		disconnect_all();
	}
	else
	{
		btmtk_gap_power_off();
	}
}

void btmtk_ev_read_adapter_properties_cb(bt_gap_context_t *pContext, 
	                                            ilm_struct *ilm)
{
	bt_property_t properties[6];
	
	btmtk_read_adapter_properties(pContext, properties);
}

void btmtk_ev_read_adapter_property_cb(bt_gap_context_t *pContext, 
	                                      ilm_struct *ilm)
{
	btmtk_read_adapter_property_struct *req_p = (btmtk_read_adapter_property_struct *)ilm->ilm_data;
	bt_property_t property = {0};

	int i = 0;
    bt_bdaddr_t addr;
    bt_bdname_t name;
    bt_scan_mode_t mode;
    kal_uint32 disc_timeout;
    bt_bdaddr_t bonded_devices[BTBM_ADP_MAX_PAIRED_LIST_NO];
	unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	int uuid_len = 0;
    bt_uuid_t local_uuids[BTBM_ADP_MAX_SDAP_UUID_NO];

	ALOGI("[GAP] btmtk_ev_read_adapter_property_cb type:[%d] !\n", req_p->type);
	if(req_p->type == BT_PROPERTY_BDADDR)
	{
		/* BD_ADDR */
		property.type = BT_PROPERTY_BDADDR;
		property.len = sizeof(addr);
		property.val = &addr;
		btmtk_get_adapter_property(pContext, &property);
	}
	else if(req_p->type == BT_PROPERTY_BDNAME)
	{
	    /* BD_NAME */
		memset(&name, 0, sizeof(bt_bdname_t));
		property.type = BT_PROPERTY_BDNAME;
		property.len = sizeof(name);
		property.val = &name;
		btmtk_get_adapter_property(pContext, &property);
	}
	else if(req_p->type == BT_PROPERTY_ADAPTER_SCAN_MODE)
	{
	    /* SCAN_MODE */
		property.type = BT_PROPERTY_ADAPTER_SCAN_MODE;
		property.len = sizeof(mode);
		property.val = &mode;
		btmtk_get_adapter_property(pContext, &property);
	}
	else if(req_p->type == BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT)
	{
	    /* DISC_TIMEOUT */
		property.type = BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT;
		property.len = sizeof(disc_timeout);
		property.val = &disc_timeout;
		btmtk_get_adapter_property(pContext, &property);
	}
	else if(req_p->type == BT_PROPERTY_ADAPTER_BONDED_DEVICES)
	{
	    /* BONDED_DEVICES */
		if(pContext->paired_cache_no > 0)
		{
			property.type = BT_PROPERTY_ADAPTER_BONDED_DEVICES;
			property.len = sizeof(bonded_devices);
			property.val = bonded_devices;
			btmtk_get_adapter_property(pContext, &property);
		}
	}
	else if(req_p->type == BT_PROPERTY_UUIDS)
	{
	    /* LOCAL UUIDs */
		property.type = BT_PROPERTY_UUIDS;
		property.len = sizeof(local_uuids);
		property.val = local_uuids;
		btmtk_get_adapter_property(pContext, &property);
	}

    HAL_CBACK(bt_hal_cbacks, adapter_properties_cb,
                     BT_STATUS_SUCCESS, 1, &property);
}

void btmtk_ev_write_scan_mode_timeout_cb(bt_gap_context_t *pContext, 
	                                          ilm_struct *ilm)
{
	btmtk_set_scan_mode_timeout_struct *req_p = (btmtk_set_scan_mode_timeout_struct *)ilm->ilm_data;
	bt_property_t property;
	

	/* DISC_TIMEOUT */
	pContext->host_cache.scan_mode_timeout = req_p->scan_mode_timeout;
	btmtk_host_cache_write();
	
	property.type = BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT;
	property.len = sizeof(req_p->scan_mode_timeout);
	property.val = &(req_p->scan_mode_timeout);
	btmtk_get_adapter_property(pContext, &property);
		
	HAL_CBACK(bt_hal_cbacks, adapter_properties_cb,
					 BT_STATUS_SUCCESS, 1, &property);
}

void btmtk_ev_read_remote_property_cb(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	btmtk_read_remote_property_struct *req_p = (btmtk_read_remote_property_struct *)ilm->ilm_data;
	bt_property_t property;
	bt_bdaddr_t remote_addr;
    bt_bdname_t name;
    kal_uint32 cod;
	kal_uint32 devicetype;
	bt_uuid_t  remote_uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	unsigned short uuid[BTBM_ADP_MAX_SDAP_UUID_NO];

    
	if(req_p->type == BT_PROPERTY_BDNAME)
	{
		/* BD_NAME */
		memset(&name, 0, sizeof(bt_bdname_t));
		property.type = BT_PROPERTY_BDNAME;
		property.len = sizeof(name);
		property.val = &name;
	}
	else if(req_p->type == BT_PROPERTY_REMOTE_FRIENDLY_NAME)
	{
		/* BD_FRIENDLYNAME */
		memset(&name, 0, sizeof(bt_bdname_t));
		property.type = BT_PROPERTY_REMOTE_FRIENDLY_NAME;
		property.len = sizeof(name);
		property.val = &name;
	}
    else if(req_p->type == BT_PROPERTY_CLASS_OF_DEVICE)
	{
		/* BD_COD */
		property.type = BT_PROPERTY_CLASS_OF_DEVICE;
		property.len = sizeof(kal_uint32);
		property.val = &cod;
    }
	else if(req_p->type == BT_PROPERTY_TYPE_OF_DEVICE)
	{
		/* BD_DEVTYPE */
		property.type = BT_PROPERTY_TYPE_OF_DEVICE;
		property.len = sizeof(kal_uint32);
		property.val = &devicetype;
	}
	else if(req_p->type == BT_PROPERTY_UUIDS)
	{
		/* BD_UUID */
		property.type = BT_PROPERTY_UUIDS;
		property.len = sizeof(remote_uuid);
		property.val = remote_uuid;
	}
	btmtk_get_remote_device_property(pContext, &req_p->addr, &property);
  	btmtk_util_convert_bdaddr2array(&remote_addr, &req_p->addr);
    HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb,
                     BT_STATUS_SUCCESS, &remote_addr, 1, &property);
}

void btmtk_ev_read_remote_properties_cb(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	btmtk_read_remote_property_struct *req_p = (btmtk_read_remote_property_struct *)ilm->ilm_data;
	bt_property_t properties[8];

   	btmtk_read_remote_device_properties(pContext, &(req_p->addr), properties);
}

void btmtk_ev_paired_device_rename_cb(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	btmtk_device_entry_struct *ptr = NULL; 
	btmtk_paired_device_rename_struct *ind_p = (btmtk_paired_device_rename_struct *)ilm->ilm_data;
	bt_property_t property;
	bt_bdaddr_t remote_addr;
	bt_bdname_t name;
	
	ALOGI("[GAP] btmtk_ev_paired_device_rename_cb: name=%s , len = %d",
				ind_p->name, ind_p->name_len);		

	if ((ptr = btmtk_paired_dev_cache_find(&(ind_p->addr))) != NULL)
    {   
    	memset(ptr->nickname, 0x0, BTBM_ADP_MAX_NAME_LEN);
    	if (ind_p->name_len >= BTBM_ADP_MAX_NAME_LEN)    			
    	{
    		strncpy(ptr->nickname, ind_p->name, BTBM_ADP_MAX_NAME_LEN-1);
			ptr->nickname[BTBM_ADP_MAX_NAME_LEN-1]='\0';
		} else if (ind_p->name_len > 0) {
			strncpy(ptr->nickname, ind_p->name, ind_p->name_len);
		} else {
			ptr->nickname[0]='\0';
		}
		btmtk_paired_dev_cache_write();

		/* BD_FRIENDLYNAME */
		strncpy(name.name, ptr->nickname, BTBM_ADP_MAX_NAME_LEN-1);
	}
	else
	{
		/* BD_FRIENDLYNAME */
		strncpy(name.name, ind_p->name, BTBM_ADP_MAX_NAME_LEN-1);
	}
	name.name[BTBM_ADP_MAX_NAME_LEN-1]='\0';
	property.type = BT_PROPERTY_REMOTE_FRIENDLY_NAME;
	property.len = strlen((char *)name.name);
	property.val = &name;
	
	btmtk_util_convert_bdaddr2array(&remote_addr, &ind_p->addr);
	HAL_CBACK(bt_hal_cbacks, remote_device_properties_cb,
                     BT_STATUS_SUCCESS, &remote_addr, 1, &property);
}


void btmtk_ev_discovery_started_cb(bt_gap_context_t *pContext,
	                                      ilm_struct *ilm)
{
    ALOGI("[GAP] btmtk_ev_discovery_started_cb!\n");
	btmtk_inquired_dev_cache_clear();

    pContext->activity = (btmtk_gap_activity_enum)(pContext->activity | BTMTK_GAP_ACT_INQUIRY);
    HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb, BT_DISCOVERY_STARTED);
}

void btmtk_ev_discovery_stopped_cb(bt_gap_context_t *pContext,
	                                      ilm_struct *ilm)
{
    ALOGI("[GAP] btmtk_ev_discovery_stopped_cb!\n");

    pContext->activity = (btmtk_gap_activity_enum)(pContext->activity & ~BTMTK_GAP_ACT_INQUIRY);
    HAL_CBACK(bt_hal_cbacks, discovery_state_changed_cb, BT_DISCOVERY_STOPPED);
}

kal_bool btmtk_ev_create_bond_cb(bt_gap_context_t *pContext,
	                               ilm_struct *ilm)
{
	kal_bool ret = KAL_FALSE;
	btmtk_bonding_req_struct *req_p = (btmtk_bonding_req_struct *)ilm->ilm_data;
	bt_addr_struct *addr = &req_p->bd_addr;
	kal_bool result = req_p->result;
	bt_bdaddr_t bdaddr;
	btmtk_device_entry_struct *inquiry_entry;
	btmtk_device_entry_struct entry;

    ALOGI("[GAP] btmtk_ev_create_bond_cb: addr=[0x%lX:0x%X:0x%X]", 
		  addr->lap, addr->uap, addr->nap);
	if(result == KAL_TRUE)
	{
		inquiry_entry = btmtk_inquired_dev_cache_find(addr);
		if(inquiry_entry == NULL)
		{
			memcpy(&entry.addr, addr, sizeof(bt_addr_struct));
			entry.paired = BTMTK_BOND_STATE_UNBOND;
			entry.trusted = KAL_FALSE;
			inquiry_entry = btmtk_inquired_dev_cache_add(&entry);
		}
		
		if(inquiry_entry->paired == BTMTK_BOND_STATE_UNBOND)
		{
			btmtk_util_convert_bdaddr2array(&bdaddr, addr);
			HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_BONDING);
			if(btmtk_gap_bond_create(addr))
		    {
		    	btmtk_bond_req_struct *req = btmtk_util_find_bond_request(addr);
				if(req == NULL)
				{
		    		req = (btmtk_bond_req_struct *) calloc(1, sizeof(btmtk_bond_req_struct));
					memcpy(&req->addr, addr, sizeof(bt_addr_struct));
					btmtk_util_list_append((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
				}
				//local initial bond flag
				req->activity = BTMTK_BOND_ACTIVITY_BONDING;
				req->auth_type = BTMTK_AUTH_TYPE_NONE;
				req->autopair_attempts = 0;
				inquiry_entry->paired = BTMTK_BOND_STATE_BONDING;
		        ret = KAL_TRUE;
		    }
		}
		else
		{
			ALOGI("[GAP] btmtk_ev_create_bond_cb is not unbond status!\n");
		}
	}
	else
	{
		btmtk_util_convert_bdaddr2array(&bdaddr, addr);
		HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_AUTH_FAILURE, &bdaddr, BT_BOND_STATE_NONE);
	}
	ALOGI("[GAP] btmtk_ev_create_bond_cb return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_ev_cancel_bond_cb(bt_gap_context_t *pContext,
	                               ilm_struct *ilm)
{
	kal_bool ret = KAL_FALSE;
	bt_bm_bonding_cancel_req_struct *req_p = (bt_bm_bonding_cancel_req_struct *)ilm->ilm_data;
	bt_addr_struct *addr = &req_p->bd_addr;
	btmtk_device_entry_struct *inquiry_entry;

    ALOGI("[GAP] btmtk_ev_cancel_bond_cb: addr=[0x%lX:0x%X:0x%X]", 
		  addr->lap, addr->uap, addr->nap);
	btmtk_gap_cancel_rebond();
	inquiry_entry = btmtk_inquired_dev_cache_find(addr);
	if(inquiry_entry != NULL && inquiry_entry->paired == BTMTK_BOND_STATE_BONDING)
	{
		btmtk_bond_req_struct *req = btmtk_util_find_bond_request(addr);
		if(req == NULL)
		{
			ALOGE("[GAP] btmtk_ev_cancel_bond_cb bond request NULL!\n");
			return KAL_FALSE;
		}

		if((req->auth_type == BTMTK_AUTH_TYPE_NONE) 
			|| (req->auth_type == BTMTK_AUTH_TYPE_PASSKEY_CONFIRMATION))
		{
			btmtk_gap_bond_cancel(addr);
		}
		else if(req->auth_type == BTMTK_AUTH_TYPE_PASSKEY_ENTRY)
		{
			btmtk_gap_ssp_reply(addr, BT_SSP_VARIANT_PASSKEY_ENTRY, KAL_FALSE, 0);
		}
		else if(req->auth_type == BTMTK_AUTH_TYPE_PASSKEY_NOTIFICATION)
		{
			btmtk_gap_ssp_reply(addr, BT_SSP_VARIANT_PASSKEY_NOTIFICATION, KAL_FALSE, 0);
		}
		else if(req->auth_type == BTMTK_AUTH_TYPE_CONSENT)
		{
			btmtk_gap_ssp_reply(addr, BT_SSP_VARIANT_CONSENT, KAL_FALSE, 0);
		}
		else if(req->auth_type == BTMTK_AUTH_TYPE_PIN_CODE)
		{
			btmtk_gap_pin_reply(addr, KAL_FALSE, 0, NULL);
		}
	}
	else
	{
		ALOGI("[GAP] btmtk_ev_cancel_bond_cb is not unbond status!\n");
	}
	ALOGI("[GAP] btmtk_ev_cancel_bond_cb return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_ev_remove_bond_cb(bt_gap_context_t *pContext,
	                                ilm_struct *ilm)
{
	kal_bool ret = KAL_FALSE;
	bt_bm_bonding_cancel_req_struct *req_p = (bt_bm_bonding_cancel_req_struct *)ilm->ilm_data;
	bt_addr_struct *addr = &req_p->bd_addr;
	btmtk_device_entry_struct *entry;

    ALOGI("[GAP] btmtk_ev_remove_bond_cb: addr=[0x%lX:0x%X:0x%X]", 
		  addr->lap, addr->uap, addr->nap);
	entry = btmtk_paired_dev_cache_find(addr);
	if(entry != NULL && entry->paired == BTMTK_BOND_STATE_BONDED)
	{
		if(btmtk_gap_delete_trust(addr))
	    {
	    	btmtk_bond_req_struct *req = btmtk_util_find_bond_request(addr);
	        if (req!= NULL)
	        {
	            btmtk_util_list_remove((btmtk_list_header_struct **)&pContext->requests, (btmtk_list_header_struct *)req);
	        }
	        ret = KAL_TRUE;
	    }
	}
	else
	{
		bt_bdaddr_t bdaddr;

		btmtk_util_convert_bdaddr2array(&bdaddr, &req_p->bd_addr);
        HAL_CBACK(bt_hal_cbacks, bond_state_changed_cb, BT_STATUS_SUCCESS, &bdaddr, BT_BOND_STATE_NONE);
	}
	ALOGI("[GAP] btmtk_ev_remove_bond_cb return:%d!\n", ret);
    return ret;
}

kal_bool btmtk_ev_enable_service_cb(bt_gap_context_t *pContext,
	                                      ilm_struct *ilm)
{
	btmtk_execute_services_struct *req_p = (btmtk_execute_services_struct *)ilm->ilm_data;

	ALOGI("[GAP] btmtk_ev_enable_service_cb: service_id: [%ld]", 
		  req_p->service_id);
	btmtk_execute_service_request(req_p->service_id, KAL_TRUE);
	return KAL_TRUE;
}

kal_bool btmtk_ev_disable_service_cb(bt_gap_context_t *pContext,
	                                  ilm_struct *ilm)
{
	btmtk_execute_services_struct *req_p = (btmtk_execute_services_struct *)ilm->ilm_data;

	ALOGI("[GAP] btmtk_ev_disable_service_cb: service_id: [%ld]", 
		  req_p->service_id);
	btmtk_execute_service_request(req_p->service_id, KAL_FALSE);
	return KAL_TRUE;
}

kal_bool btmtk_ev_status_ind_cb(bt_gap_context_t *pContext,
	                                 ilm_struct *ilm)
{
	btmtk_update_bt_status_struct *req_p = (btmtk_update_bt_status_struct *)ilm->ilm_data;

	ALOGI("[GAP] btmtk_ev_status_ind_cb: status: [%d], bt_status: [%d]", req_p->status, pContext->bt_status);
	if(req_p->status == BTMTK_POWER_STATE_ON || req_p->status == BTMTK_POWER_STATE_TURNING_ON)
	{
		//btmtk_gap_set_local_name(pContext->host_cache.name, strlen(pContext->host_cache.name));
		//POWER ON success
		btmtk_set_local_feature_mask(pContext, BTMTK_READ_LOCAL_ADDR_MASK, KAL_TRUE);
		
		ALOGI("[GAP] btmtk_gap_get_local_addr");
		pContext->bt_status = req_p->status;
		if(!btmtk_get_local_addr())
		{
			ALOGD("[GAP] btmtk_gap_get_local_addr send msg failed!\n");
			pContext->bt_status = BTMTK_POWER_STATE_OFF;
			HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_OFF);
		}
	}
	else
	{
		pContext->bt_status = req_p->status;
	}
	return KAL_TRUE;
}
char *btmtk_format_message(unsigned long id)
{
	switch (id)
    {
    case MSG_ID_BT_POWERON_CNF:
		return "MSG_ID_BT_POWERON_CNF";
	case MSG_ID_BT_POWEROFF_CNF:
		return "MSG_ID_BT_POWEROFF_CNF"; 
	case MSG_ID_BT_RESET_REQ_IND:
    	return "MSG_ID_BT_RESET_REQ_IND";
	case MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF:
		return "MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF"; 
	case MSG_ID_BT_BM_READ_LOCAL_UUID_CNF:
		return "MSG_ID_BT_BM_READ_LOCAL_UUID_CNF";
	 case MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF:
		return "MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF";
	case MSG_ID_BT_BM_READ_LOCAL_COD_CNF:
		return "MSG_ID_BT_BM_READ_LOCAL_COD_CNF"; 
	case MSG_ID_BT_BM_READ_PROPERTY_CNF:
		return "MSG_ID_BT_BM_READ_PROPERTY_CNF";
    case MSG_ID_BT_BM_DISCOVERY_RESULT_IND:
    	return "MSG_ID_BT_BM_DISCOVERY_RESULT_IND";
    case MSG_ID_BT_BM_DISCOVERY_UPDATE_IND:
    	return "MSG_ID_BT_BM_DISCOVERY_UPDATE_IND";
    case MSG_ID_BT_BM_DISCOVERY_CNF:
    	return "MSG_ID_BT_BM_DISCOVERY_CNF";
	case MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:
		return "MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF";
    case MSG_ID_BT_BM_DELETE_TRUST_CNF:
    	return "MSG_ID_BT_BM_DELETE_TRUST_CNF";
	case MSG_ID_BT_BM_BONDING_RESULT_IND:
		return "MSG_ID_BT_BM_BONDING_RESULT_IND";
    case MSG_ID_BT_BM_BONDING_CNF:
		return "MSG_ID_BT_BM_BONDING_CNF";
    case MSG_ID_BT_BM_PIN_CODE_IND:
		return "MSG_ID_BT_BM_PIN_CODE_IND";
    case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND:
    	return "MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND";
    case MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_IND:
		return "MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_IND";
    case MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND:
		return "MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND";
	case MSG_ID_BT_BM_SECURITY_OOB_DATA_IND:
		return "MSG_ID_BT_BM_SECURITY_OOB_DATA_IND";
    case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_IND:
        return "MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_IND";
    case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:
		return "MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF";
    case MSG_ID_BT_BM_READ_LOCAL_NAME_CNF:
		return "MSG_ID_BT_BM_READ_LOCAL_NAME_CNF";
    case MSG_ID_BT_BM_READ_REMOTE_NAME_CNF:
		return "MSG_ID_BT_BM_READ_REMOTE_NAME_CNF";
    case MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF:
		return "MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF";
	case MSG_ID_BT_BM_REMOTE_NAME_IND:
		return "MSG_ID_BT_BM_REMOTE_NAME_IND";
    case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:
		return "MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF";
    case MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF:
		return "MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF";
    case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF:
		return "MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF";
    case MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_CNF:
		return "MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_CNF";
    case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_CNF:
		return "MSG_ID_BT_BM_BLOCK_LIST_UPDATE_CNF";
    case MSG_ID_BT_BM_DELETE_TRUST_ALL_CNF:
		return "MSG_ID_BT_BM_DELETE_TRUST_ALL_CNF";
    case MSG_ID_BT_BM_BONDING_CANCEL_CNF:
		return "MSG_ID_BT_BM_BONDING_CANCEL_CNF";
    case MSG_ID_BT_BM_PAIRING_CANCEL_CNF:
		return "MSG_ID_BT_BM_PAIRING_CANCEL_CNF";
    case MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND:
		return "MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND";
    case MSG_ID_BT_BM_SERVICE_SEARCH_CNF:
		return "MSG_ID_BT_BM_SERVICE_SEARCH_CNF";
    case MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_CNF:
		return "MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_CNF";
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND:
		return "MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND";
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF:
		return "MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF";
    case MSG_ID_BT_BM_LINK_STATE_IND:
		return "MSG_ID_BT_BM_LINK_STATE_IND";
    case MSG_ID_BT_BM_LINK_ALLOW_CNF:
		return "MSG_ID_BT_BM_LINK_ALLOW_CNF";
    case MSG_ID_BT_BM_LINK_DISALLOW_CNF:    
		return "MSG_ID_BT_BM_LINK_DISALLOW_CNF";
    case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND:
		return "MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND";
    case MSG_ID_BT_BM_GET_LINK_STATE_CNF:
		return "MSG_ID_BT_BM_GET_LINK_STATE_CNF";
    case MSG_ID_BT_BM_GET_SCATTERNET_STATE_CNF:
		return "MSG_ID_BT_BM_GET_SCATTERNET_STATE_CNF";
    case MSG_ID_BT_BM_SEARCH_RAW_RESULT:
		return "MSG_ID_BT_BM_SEARCH_RAW_RESULT";
    case MSG_ID_BT_BM_SEARCH_RAW_CNF:
		return "MSG_ID_BT_BM_SEARCH_RAW_CNF";	
	case EV_BT_CB_ENABLE_SERVICE:
		return "EV_BT_CB_ENABLE_SERVICE";
	case EV_BT_CB_DISABLE_SERVICE:
		return "EV_BT_CB_DISABLE_SERVICE";
	case EV_BT_CB_REMOVE_BOND:
		return "EV_BT_CB_REMOVE_BOND";
    case EV_BT_CB_CANCEL_BOND:
		return "EV_BT_CB_CANCEL_BOND";
    case EV_BT_CB_CREATE_BOND:
		return "EV_BT_CB_CREATE_BOND";
    case EV_BT_CB_DISCOVERY_STOPPED:
		return "EV_BT_CB_DISCOVERY_STOPPED";
    case EV_BT_CB_DISCOVERY_STARTED:
		return "EV_BT_CB_DISCOVERY_STARTED";
    case EV_BT_CB_PAIRED_DEVICE_RENAME:
		return "EV_BT_CB_PAIRED_DEVICE_RENAME";
    case EV_BT_CB_WRITE_SCAN_MODE_TIMEOUT:
		return "EV_BT_CB_WRITE_SCAN_MODE_TIMEOUT";
    case EV_BT_CB_READ_REMOTE_PROPERTY:
		return "EV_BT_CB_READ_REMOTE_PROPERTY";
    case EV_BT_CB_READ_REMOTE_PROPERTY_ALL:
		return "EV_BT_CB_READ_REMOTE_PROPERTY_ALL";
    case EV_BT_CB_READ_ADAPTER_PROPERTY:
		return "EV_BT_CB_READ_ADAPTER_PROPERTY";
    case EV_BT_CB_READ_ADAPTER_PROPERTY_ALL:
		return "EV_BT_CB_READ_ADAPTER_PROPERTY_ALL";
    case EV_BT_CB_DISABLE:
		return "EV_BT_CB_DISABLE";
    }
    return "";
}

void btmtk_ext_gap_message_handler(bt_gap_context_t *pContext, ilm_struct *ilm)
{
	ALOGI("+++[GAP] btmtk_handle_gap_message msg:[%s]+++!\n", btmtk_format_message(ilm->msg_id));
	switch (ilm->msg_id)
    {
    case MSG_ID_BT_POWERON_CNF:
		btmtk_gap_poweron_cnf(pContext, ilm); 
		break;
	case MSG_ID_BT_BM_READ_LOCAL_ADDR_CNF:
		btmtk_gap_read_local_addr_cnf(pContext, ilm);
		break;
	case MSG_ID_BT_BM_READ_LOCAL_UUID_CNF:
		btmtk_gap_read_local_uuid_cnf(pContext, ilm);
		break;
	case MSG_ID_BT_POWEROFF_CNF:
		btmtk_gap_poweroff_cnf(pContext, ilm);
		break;
    case MSG_ID_BT_BM_DISCOVERY_RESULT_IND:
    	btmtk_gap_discovery_result_ind(pContext, ilm);
        break;
    case MSG_ID_BT_BM_DISCOVERY_UPDATE_IND:
		btmtk_gap_discovery_update_ind(pContext, ilm);
	    break;
    case MSG_ID_BT_BM_DISCOVERY_CNF:
    	btmtk_gap_discovery_cnf(pContext, ilm);
        break;
    case MSG_ID_BT_BM_DELETE_TRUST_CNF:
   		btmtk_gap_delete_trust_cnf(pContext, ilm);
        break;
    case MSG_ID_BT_BM_BONDING_CNF:
		btmtk_gap_bonding_cnf(pContext, ilm);
	    break;
    case MSG_ID_BT_BM_PIN_CODE_IND:
		btmtk_gap_pin_code_ind(pContext, ilm);
    	break;
    case MSG_ID_BT_BM_BONDING_RESULT_IND:
		btmtk_gap_bonding_result_ind(pContext, ilm);
    	break;
    case MSG_ID_BT_BM_SECURITY_USER_CONFIRM_IND:
		btmtk_gap_security_user_confirm_ind(pContext, ilm);
    	break;
    case MSG_ID_BT_BM_SECURITY_PASSKEY_ENTRY_IND:
		btmtk_gap_security_passkey_entry_ind(pContext, ilm);
    	break;
    case MSG_ID_BT_BM_SECURITY_PASSKEY_NOTIFY_IND:
		btmtk_gap_security_passkey_notify_ind(pContext, ilm);
    	break;
    case MSG_ID_BT_BM_DISCOVERY_CANCEL_CNF:
		btmtk_gap_discovery_cancel_cnf(pContext, ilm);
		break;
    case MSG_ID_BT_BM_WRITE_LOCAL_NAME_CNF:
		btmtk_gap_write_local_name_cnf(pContext, ilm);
		break;
    case MSG_ID_BT_BM_WRITE_SCANENABLE_MODE_CNF:
		btmtk_gap_write_scanenable_mode_cnf(pContext, ilm);
		break;
    case MSG_ID_BT_BM_BONDING_CANCEL_CNF:
		btmtk_gap_bonding_cancel_cnf(pContext, ilm);
		break;
    case MSG_ID_BT_BM_SERVICE_SEARCH_RESULT_IND:
		btmtk_gap_service_search_result_ind(pContext, ilm);
		break;
    case MSG_ID_BT_BM_SERVICE_SEARCH_CNF:
		btmtk_gap_service_search_cnf(pContext, ilm);
		break;
    case MSG_ID_BT_BM_LINK_STATE_IND:
		btmtk_gap_link_state_ind(pContext, ilm);
		break;
    case MSG_ID_BT_BM_SEARCH_RAW_RESULT:
		break;
    case MSG_ID_BT_BM_SEARCH_RAW_CNF:
		break;
	case EV_BT_CB_ENABLE_SERVICE:
		btmtk_ev_enable_service_cb(pContext, ilm);
		break;
	case EV_BT_CB_DISABLE_SERVICE:
		btmtk_ev_disable_service_cb(pContext, ilm);
		break;
	case EV_BT_CB_REMOVE_BOND:
		btmtk_ev_remove_bond_cb(pContext, ilm);
		break;
    case EV_BT_CB_CANCEL_BOND:
		btmtk_ev_cancel_bond_cb(pContext, ilm);
		break;
    case EV_BT_CB_CREATE_BOND:
		btmtk_ev_create_bond_cb(pContext, ilm);
		break;
    case EV_BT_CB_DISCOVERY_STOPPED:
		btmtk_ev_discovery_stopped_cb(pContext, ilm);
		break;
    case EV_BT_CB_DISCOVERY_STARTED:
		btmtk_ev_discovery_started_cb(pContext, ilm);
		break;
    case EV_BT_CB_PAIRED_DEVICE_RENAME:
		btmtk_ev_paired_device_rename_cb(pContext, ilm);
		break;
    case EV_BT_CB_WRITE_SCAN_MODE_TIMEOUT:
		btmtk_ev_write_scan_mode_timeout_cb(pContext, ilm);
		break;
    case EV_BT_CB_READ_REMOTE_PROPERTY:
		btmtk_ev_read_remote_property_cb(pContext, ilm);
		break;
    case EV_BT_CB_READ_REMOTE_PROPERTY_ALL:
		btmtk_ev_read_remote_properties_cb(pContext, ilm);
		break;
    case EV_BT_CB_READ_ADAPTER_PROPERTY:
		btmtk_ev_read_adapter_property_cb(pContext, ilm);
		break;
    case EV_BT_CB_READ_ADAPTER_PROPERTY_ALL:
		btmtk_ev_read_adapter_properties_cb(pContext, ilm);
		break;
    case EV_BT_CB_DISABLE:
		btmtk_ev_disable_cb(pContext, ilm);
		break;
	case MSG_ID_BT_BM_REMOTE_NAME_IND:
		btmtk_gap_remote_device_name_ind(pContext, ilm);
		break;
	case EV_BT_CB_BT_STATUS_IND:
		btmtk_ev_status_ind_cb(pContext, ilm);
		break;
	case MSG_ID_BT_RESET_REQ_IND:
#ifdef DETECT_BTUSB_RESUME
//liang cl:1976747
		btmtk_gap_reset_req_ind(pContext, ilm);
		break;
#endif		
	case EV_BT_CB_PROFILE_CONNECT_REQ:
		{
			btmtk_profile_connect_request_struct *conn_req_p = (btmtk_profile_connect_request_struct *)ilm->ilm_data;
			handle_profile_connect(&conn_req_p->bd_addr, conn_req_p->profile_id);
		}
		break;
	case EV_BT_CB_PROFILE_DISCONNECT_REQ:
		{
			btmtk_profile_disconnect_request_struct *disc_req_p = (btmtk_profile_disconnect_request_struct *)ilm->ilm_data;
			handle_profile_disconnect(&disc_req_p->bd_addr, disc_req_p->profile_id);
		}
		break;
	case MSG_ID_BT_BM_SECURITY_OOB_DATA_IND:
    case MSG_ID_BT_BM_SECURITY_KEYPRESS_NOTIFY_IND:
	case MSG_ID_BT_BM_READ_LOCAL_NAME_CNF:
	case MSG_ID_BT_BM_READ_REMOTE_NAME_CNF:
    case MSG_ID_BT_BM_READ_REMOTE_NAME_CANCEL_CNF:
	case MSG_ID_BT_BM_READ_SCANENABLE_MODE_CNF:
    case MSG_ID_BT_BM_READ_LOCAL_COD_CNF:
    case MSG_ID_BT_BM_WRITE_LOCAL_COD_CNF:
    case MSG_ID_BT_BM_READ_PROPERTY_CNF:
    case MSG_ID_BT_BM_WRITE_AUTHENTICATION_MODE_CNF:
    case MSG_ID_BT_BM_BLOCK_ACTIVE_LINK_CNF:
    case MSG_ID_BT_BM_BLOCK_LIST_UPDATE_CNF:
    case MSG_ID_BT_BM_DELETE_TRUST_ALL_CNF:
	case MSG_ID_BT_BM_SERVICE_SEARCH_CANCEL_CNF:
	case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_RESULT_IND:
    case MSG_ID_BT_BM_SEARCH_ATTRIBUTE_CNF:
	case MSG_ID_BT_BM_LINK_ALLOW_CNF:
	case MSG_ID_BT_BM_LINK_CONNECT_ACCEPT_IND:
    case MSG_ID_BT_BM_GET_LINK_STATE_CNF:
    case MSG_ID_BT_BM_GET_SCATTERNET_STATE_CNF:
	case MSG_ID_BT_BM_PAIRING_CANCEL_CNF:
	case MSG_ID_BT_BM_LINK_DISALLOW_CNF:
		break;
    }
	ALOGI("---[GAP] btmtk_handle_gap_message---!\n");
}


