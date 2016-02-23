/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/************************************************************************************
 *
 *  Filename:      btif_core.c
 *
 *  Description:   Contains core functionality related to interfacing between
 *                 Bluetooth HAL and BTE core stack.
 *
 ***********************************************************************************/	  
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
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "cutils/sockets.h"

#include "bt_cust_api.h"
#include "bt_utils.h"
#include "btif_common.h"
#include "btif_sock.h"
#include "bt_ext_common.h"
#include "bt_gap_api.h"
#include "bt_mmi.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "BTIF_CORE"
#include "btif_api.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
typedef enum {
	BTIF_CORE_STATE_DISABLED = 0,
	BTIF_CORE_STATE_ENABLING,
	BTIF_CORE_STATE_ENABLED,
	BTIF_CORE_STATE_DISABLING
} btif_core_state_t;

/* holds main adapter state */
static btif_core_state_t btif_core_state = BTIF_CORE_STATE_DISABLED;

static int btif_shutdown_pending = 0;
static uint32_t btif_enabled_services = 0;

/*
* This variable should be set to 1, if the Bluedroid+BTIF libraries are to
* function in DUT mode.
*
* To set this, the btif_init_bluetooth needs to be called with argument as 1
*/
static uint8_t btif_dut_mode = 0;

/************************************************************************************
**  external
************************************************************************************/
extern bt_status_t btif_associate_evt(void);
extern bt_status_t btif_disassociate_evt(void);

/*******************************************************************************
**
** Function         btif_is_dut_mode
**
** Description      checks if BTIF is currently in DUT mode
**
** Returns          1 if test mode, otherwize 0
**
*******************************************************************************/
uint8_t btif_is_dut_mode(void)
{
    return (btif_dut_mode == 1);
}


/*******************************************************************************
**
** Function         btif_is_enabled
**
** Description      checks if main adapter is fully enabled
**
** Returns          1 if fully enabled, otherwize 0
**
*******************************************************************************/
int btif_is_enabled(void)
{
    return ((!btif_is_dut_mode()) && (btif_core_state == BTIF_CORE_STATE_ENABLED));
}


/*******************************************************************************
**
** Function         btif_task
**
** Description      BTIF task handler managing all messages from stack&btif.
**
** Returns          void
**
*******************************************************************************/
void *btif_task(void *arg)
{
	bt_gap_context_t *pContext = (bt_gap_context_t *)arg;
	int bExit = 0;
	int i = 0;
	
	ALOGI("+++[btif_task]+++!\n");
    btif_associate_evt();
#if 1
	while (1) {
		ALOGI("[btif_task]:Start retrieve data pollMemberCount=%d!\n", pContext->pollMemberCount);
		for (i = 0; i < pContext->pollMemberCount; i++) {
			if (!pContext->pollData[i].revents) {
				continue;
			}
			
			ALOGI("[btif_task]:fd %d data ready!\n", i);
			{
				ilm_struct ilm;
				int ret;

				ALOGI("[btif_task]:pollData[i].fd data ready : revents = 0x%X!\n", pContext->pollData[i].revents);
				while ((ret = recv(pContext->pollData[i].fd, (void*)&ilm, sizeof(ilm_struct), MSG_DONTWAIT)) != -1) 
				{
					btmtk_ext_msg_handler_struct *ptr = pContext->pHandler;
					pthread_mutex_lock(&(pContext->thread_mutex));
					while(ptr)
					{
						ALOGI("in profile[%d] handler!", ptr->profile_id);
						if(pContext->pollData[i].fd == ptr->socket)
						{
							if((ptr->profile_id == PROFILE_GAP) && (ilm.msg_id == EV_BT_CB_TASK_EXIT))
							{
								pthread_mutex_unlock(&(pContext->thread_mutex));
								goto BTIF_TASK_EXIT;
							}
							
							if(ptr->handler != NULL)
							{
								ptr->handler(ptr->pContext, &ilm);
								ALOGI("out profile[%d] handler!", ptr->profile_id);
							}
							break;
						}
						ALOGI("out profile[%d] handler!", ptr->profile_id);
						ptr = ptr->next;
					}
					pthread_mutex_unlock(&(pContext->thread_mutex));
				}
			}
		}
		ALOGI("[btif_task]:Start polling!\n");
		poll(pContext->pollData, pContext->pollMemberCount, -1);
		ALOGI("[btif_task]:Polling returned!\n");
	}
#else
	
	while(1)
	{
		ilm_struct ilm;
		int ret = 0;
		fd_set fd;
		
		FD_ZERO(&fd);
		for (i = 0; i < pContext->pollMemberCount; i++)
		{
			FD_SET(pContext->pollData[i].fd, &fd);
			ret = select(pContext->pollData[i].fd + 1, &fd, NULL, NULL, NULL);
			if(ret < 0)
			{
				ALOGI("[btif_task]:select failed : %s, errno=%d", strerror(errno), errno);
            	continue;
			}
			else if(ret == 0)
			{
				ALOGI("[btif_task]:wait timeout");
			}
			else
			{
				if(pContext->pollData[i].fd >= 0 && FD_ISSET(pContext->pollData[i].fd, &fs))
				{
					ALOGI("+++[btif_task]:select success!\n+++");
					ret = recvfrom(pContext->pollData[i].fd, ilm, sizeof(ilm_struct), 0, NULL, NULL);
					if(ret > 0)
					{
						btmtk_ext_msg_handler_struct *ptr = pContext->pHandler;
						while(ptr)
						{
							if(pContext->pollData[i].fd == ptr->socket)
							{
								if(ptr->profile_id == PROFILE_GAP && ilm.msg_id == EV_BT_CB_TASK_EXIT)
								{
									goto BTIF_TASK_EXIT;
								}
								ptr->handler(ptr->pContext, &ilm);
							}
							ptr = ptr->next;
						}
					}
					ALOGI("---[btif_task]:select success!\n---");
				}
			}
		}
	}
#endif
BTIF_TASK_EXIT:
	btmtk_clear_handlers();
	btif_disassociate_evt();
    ALOGI("---[btif_task]---!\n");
	return 0;
}

/*******************************************************************************
**
** Function 		btif_init_bluetooth
**
** Description		Creates BTIF task and prepares BT scheduler for startup
**
** Returns			bt_status_t
**
*******************************************************************************/
bt_status_t btif_init_bluetooth()
{
	ALOGI("+++[btif_init_bluetooth]+++!\n");
	if(!btmtk_gap_init())
	{
		ALOGE("[btmtk_gap_init] failed!\n");
		return BT_STATUS_FAIL;
	}
		
	ALOGI("---[btif_init_bluetooth]---!\n");
	return BT_STATUS_SUCCESS;
}


/*******************************************************************************
**
** Function         btif_associate_evt
**
** Description      Event indicating btif_task is up
**                  Attach btif_task to JVM
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_associate_evt(void)
{
    ALOGI("[btif_associate_evt]: notify ASSOCIATE_JVM!\n");
    HAL_CBACK(bt_hal_cbacks, thread_evt_cb, ASSOCIATE_JVM);

    return BT_STATUS_SUCCESS;
}


/*******************************************************************************
**
** Function         btif_enable_bluetooth
**
** Description      Performs chip power on and kickstarts OS scheduler
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_enable_bluetooth(void)
{
	ALOGI("+++[btif_enable_bluetooth] state:[%d]+++!\n", btif_core_state);
	
	if (btif_core_state != BTIF_CORE_STATE_DISABLED)
	{
		if (!btmtk_gap_status_ind(btif_core_state))
	    {
	    	ALOGE("[btmtk_gap_status_ind] failed!\n");
			btif_core_state = BTIF_CORE_STATE_DISABLED;
	        return BT_STATUS_FAIL;	
	    }
		return BT_STATUS_SUCCESS;
	}

	btif_core_state = BTIF_CORE_STATE_ENABLING;
	if (!btmtk_gap_power_on())
    {
    	ALOGE("[btmtk_gap_power_on] failed!\n");
		btif_core_state = BTIF_CORE_STATE_DISABLED;
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_enable_bluetooth]---!\n");
	return BT_STATUS_SUCCESS;
}

void btif_enable_bluetooth_evt(uint8_t status)
{
	ALOGI("+++[btif_enable_bluetooth_evt]+++!\n");
	if(status == BT_STATUS_SUCCESS)
	{
		//init profile
		/* init rfcomm & l2cap api */
        btif_sock_init();
		
		btif_core_state = BTIF_CORE_STATE_ENABLED;
		HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_ON);
	}
	else
	{
		//clean profile
		/* cleanup rfcomm & l2cap api */
        btif_sock_cleanup();
		
		btif_core_state = BTIF_CORE_STATE_DISABLED;
		HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_OFF);
	}
	ALOGI("---[btif_enable_bluetooth_evt]---!\n");
}

/*******************************************************************************
**
** Function         btif_disable_bluetooth
**
** Description      Inititates shutdown of Bluetooth system.
**                  Any active links will be dropped and device entering
**                  non connectable/discoverable mode
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_disable_bluetooth(void)
{
	ALOGI("+++[btif_disable_bluetooth]+++!\n");
	if (!btif_is_enabled())
    {
        ALOGE("btif_disable_bluetooth : not yet enabled");
        return BT_STATUS_NOT_READY;
    }

	btif_core_state = BTIF_CORE_STATE_DISABLING;

	/* cleanup rfcomm & l2cap api */
    btif_sock_cleanup();
	
	if (!btmtk_gap_disable())
    {
    	ALOGE("[btmtk_gap_power_off] failed!\n");
		btif_core_state = BTIF_CORE_STATE_ENABLED;
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_disable_bluetooth]---!\n");
	return BT_STATUS_SUCCESS;
}

void btif_disable_bluetooth_evt(void)
{
	ilm_struct ilm;
	int result;
	
	ALOGI("+++[btif_disable_bluetooth_evt]+++!\n");
	/* update local state */
    btif_core_state = BTIF_CORE_STATE_DISABLED;

    /* callback to HAL */
    HAL_CBACK(bt_hal_cbacks, adapter_state_changed_cb, BT_STATE_OFF);
	
	if (btif_shutdown_pending)
	{
		ALOGI("[btif_disable_bluetooth_evt]:btif_shutdown_bluetooth!\n");
		btif_shutdown_bluetooth();
	}
	ALOGI("---[btif_disable_bluetooth_evt]---!\n");
}


/*******************************************************************************
**
** Function 		btif_shutdown_bluetooth
**
** Description		destroy BTIF task and prepares BT scheduler for startup
**
** Returns			bt_status_t
**
*******************************************************************************/
bt_status_t btif_shutdown_bluetooth(void)
{
	ALOGI("+++[btif_shutdown_bluetooth]+++!\n");

	if (btif_is_enabled())
    {
        ALOGI("[btif_shutdown_bluetooth]:disable bt before shutdown!\n");

        /* shutdown called prior to disabling, initiate disable */
        btif_disable_bluetooth();
		//bt_hal_cbacks = NULL;
        btif_shutdown_pending = 1;
	    btmtk_gap_cleanup(0);
        goto exit;
    }

	ALOGI("[btif_shutdown_bluetooth]:btmtk_gap_cleanup entry!\n");
	if(btif_shutdown_pending == 0)
    {
    	btmtk_gap_cleanup(1);
	}
	else
	{
	 	btif_shutdown_pending = 0;
		btmtk_gap_exit();
	}
	
exit:
	btif_dut_mode = 0;
    ALOGI("---[GAP] btif_shutdown_bluetooth---!\n");
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_disassociate_evt
**
** Description      Event indicating btif_task is going down
**                  Detach btif_task to JVM
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_disassociate_evt(void)
{
    ALOGI("btif_disassociate_evt: notify DISASSOCIATE_JVM");
    HAL_CBACK(bt_hal_cbacks, thread_evt_cb, DISASSOCIATE_JVM);
    /* shutdown complete, all events notified and we reset HAL callbacks */
    bt_hal_cbacks = NULL;
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dut_mode_configure
**
** Description      Configure Test Mode - 'enable' to 1 puts the device in test mode and 0 exits
**                       test mode
**
** Returns          BT_STATUS_SUCCESS on success
**
*******************************************************************************/
bt_status_t btif_dut_mode_configure(uint8_t enable)
{
    ALOGI("btif_dut_mode_configure");

    if (btif_core_state != BTIF_CORE_STATE_ENABLED) {
        ALOGE("btif_dut_mode_configure : Bluetooth not enabled");
        return BT_STATUS_NOT_READY;
    }

    btif_dut_mode = enable;
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dut_mode_send
**
** Description     Sends a HCI Vendor specific command to the controller
**
** Returns          BT_STATUS_SUCCESS on success
**
*******************************************************************************/
bt_status_t btif_dut_mode_send(uint16_t opcode, uint8_t *buf, uint8_t len)
{
    /* TODO: Check that opcode is a vendor command group */
    ALOGI("btif_dut_mode_send");
    if (!btif_is_dut_mode()) {
         ALOGE("Bluedroid HAL needs to be init with test_mode set to 1.");
         return BT_STATUS_FAIL;
    }
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_adapter_properties
**
** Description      Fetch all available properties (local & remote)
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_get_adapter_properties(void)
{
	ALOGI("+++[btif_get_adapter_properties]+++!\n");
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
    if (!btmtk_gap_get_adapter_properties())
    {
    	ALOGE("[btmtk_gap_get_adapter_properties] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_get_adapter_properties]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_adapter_property
**
** Description      Fetches property value from local cache
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_get_adapter_property(bt_property_type_t type)
{
	ALOGI("+++[btif_get_adapter_property] type:[%d]+++!\n", type);

	/* Allow get_adapter_property only for BDADDR and BDNAME if BT is disabled */
    if (!btif_is_enabled() && (type != BT_PROPERTY_BDADDR) && (type != BT_PROPERTY_BDNAME))
        return BT_STATUS_NOT_READY;
	
	if (!btmtk_gap_get_adapter_property(type))
    {
    	ALOGE("[btmtk_gap_get_adapter_property] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_get_adapter_property]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_set_adapter_property
**
** Description      Updates core stack with property value and stores it in
**                  local cache
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_set_adapter_property(const bt_property_t *property)
{
	ALOGI("+++[btif_set_adapter_property] type:[%d], len:[%d], val:[%p]+++!\n", 
		   property->type, property->len, property->val);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	if(!btmtk_gap_set_adapter_property(property))
	{
		ALOGE("[btmtk_gap_set_adapter_property] failed!\n");
		return BT_STATUS_FAIL;
	}

	ALOGI("---[btif_set_adapter_property]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_remote_device_properties
**
** Description      Fetches all the remote device properties from NVRAM
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_get_remote_device_properties(bt_bdaddr_t *remote_addr)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_get_remote_device_properties] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, remote_addr);
	if(!btmtk_gap_get_remote_device_properties(&bdaddr))
    {
    	ALOGE("[btmtk_gap_get_remote_device_properties] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_get_remote_device_properties]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_remote_device_property
**
** Description      Fetches the remote device property from the NVRAM
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_get_remote_device_property(bt_bdaddr_t *remote_addr,
                                                 bt_property_type_t type)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_get_remote_device_property] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, remote_addr);
	if(!btmtk_gap_get_remote_device_property(&bdaddr, type))
    {
    	ALOGE("[btmtk_gap_get_remote_device_property] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_get_remote_device_property]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_set_remote_device_property
**
** Description      Writes the remote device property to NVRAM.
**                  Currently, BT_PROPERTY_REMOTE_FRIENDLY_NAME is the only
**                  remote device property that can be set
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_set_remote_device_property(bt_bdaddr_t *remote_addr,
                                                 const bt_property_t *property)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_set_remote_device_property] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	
	if (!btif_is_enabled())
			return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, remote_addr);
	if(!btmtk_gap_set_remote_device_property(&bdaddr, property))
    {
    	ALOGE("[btmtk_gap_set_remote_device_property] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_set_remote_device_property]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_remote_service_record
**
** Description      Looks up the service matching uuid on the remote device
**                  and fetches the SCN and service_name if the UUID is found
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_get_remote_service_record(bt_bdaddr_t *remote_addr,
                                               bt_uuid_t *uuid)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_get_remote_service_record] addr:[%02x:%02x:%02x:%02x:%02x:%02x], UUID:[%p]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], uuid->uu);

	if (!btif_is_enabled())
			return BT_STATUS_NOT_READY;

	ALOGI("---[btif_get_remote_service_record]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_enabled_services_mask
**
** Description      Fetches currently enabled services
**
** Returns          tBTA_SERVICE_MASK
**
*******************************************************************************/

uint32_t btif_get_enabled_services_mask(void)
{
    return btif_enabled_services;
}


/*******************************************************************************
**
** Function         btif_dm_get_remote_services
**
** Description      Start SDP to get remote services
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_get_remote_services(bt_bdaddr_t *remote_addr)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_dm_get_remote_services] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, remote_addr);
	if(!btmtk_gap_get_remote_services(&bdaddr))
    {
    	ALOGE("[btif_dm_get_remote_services] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_get_remote_services]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_start_discovery
**
** Description      Start device discovery/inquiry
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_start_discovery(void)
{
	btbm_discovery_type type;
    unsigned int ret = 0;
    BT_CUST_ID id = 0;
    unsigned int value = 0;

/*
#ifdef __BT_4_0_BLE__
	type = BTBM_DISCOVERY_DUAL;
#else
	type = BTBM_DISCOVERY_BR_EDR_ONLY;
#endif
*/

    id = getCustID("LESupport");
    ret = getCustValue(id, &value);
    type = value ? BTBM_DISCOVERY_DUAL : BTBM_DISCOVERY_BR_EDR_ONLY;

	ALOGI("+++[btif_dm_start_discovery] type:[%d]+++!\n", type);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	if(!btmtk_gap_start_discovery(type))
    {
    	ALOGE("[btmtk_gap_start_discovery] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_start_discovery]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_cancel_discovery
**
** Description      Cancels search
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_cancel_discovery(void)
{
	btbm_discovery_type type;
	
	ALOGI("+++[btif_dm_cancel_discovery]+++!\n");
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	if(!btmtk_gap_cancel_discovery())
    {
    	ALOGE("[btmtk_gap_cancel_discovery] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_cancel_discovery]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_create_bond
**
** Description      Initiate bonding with the specified device
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_create_bond(const bt_bdaddr_t *remote_addr)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_dm_create_bond] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, (bt_bdaddr_t *)remote_addr);
	if(!btmtk_gap_create_bond(&bdaddr))
    {
    	ALOGE("[btmtk_gap_create_bond] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_create_bond]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_cancel_bond
**
** Description      Initiate bonding with the specified device
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_cancel_bond(const bt_bdaddr_t *remote_addr)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_dm_cancel_bond] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, (bt_bdaddr_t *)remote_addr);
	if(!btmtk_gap_cancel_bond(&bdaddr))
    {
    	ALOGE("[btmtk_gap_cancel_bond] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_cancel_bond]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_remove_bond
**
** Description      Removes bonding with the specified device
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_remove_bond(const bt_bdaddr_t *remote_addr)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_dm_remove_bond] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, (bt_bdaddr_t *)remote_addr);
	if(!btmtk_gap_remove_bond(&bdaddr))
    {
    	ALOGE("[btmtk_gap_cancel_bond] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_remove_bond]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_pin_reply
**
** Description      BT legacy pairing - PIN code reply
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_pin_reply( const bt_bdaddr_t *remote_addr, uint8_t accept,
                                 uint8_t pin_len, bt_pin_code_t *pin_code)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_dm_pin_reply] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, (bt_bdaddr_t *)remote_addr);
	if(!btmtk_gap_pin_reply(&bdaddr, accept, pin_len, pin_code->pin))
    {
    	ALOGE("[btmtk_gap_pin_reply] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_pin_reply]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_dm_ssp_reply
**
** Description      BT SSP Reply - Just Works, Numeric Comparison & Passkey Entry
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_dm_ssp_reply(const bt_bdaddr_t *remote_addr,
                                 bt_ssp_variant_t variant, uint8_t accept,
                                 uint32_t passkey)
{
	char *addr = (char *)remote_addr->address;
	bt_addr_struct bdaddr;
	ALOGI("+++[btif_dm_ssp_reply] addr:[%02x:%02x:%02x:%02x:%02x:%02x]+++!\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	if (!btif_is_enabled())
        return BT_STATUS_NOT_READY;
	
	btmtk_util_convert_array2bdaddr(&bdaddr, (bt_bdaddr_t *)remote_addr);
	if(!btmtk_gap_ssp_reply(&bdaddr, variant, accept, passkey))
    {
    	ALOGE("[btmtk_gap_ssp_reply] failed!\n");
        return BT_STATUS_FAIL;	
    }

	ALOGI("---[btif_dm_ssp_reply]---!\n");
	return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_enable_service
**
** Description      Enables the service 'service_ID' to the service_mask.
**                  Upon BT enable, BTIF core shall invoke the BTA APIs to
**                  enable the profiles
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_enable_service(uint8_t service_id)
{
	ALOGI("+++[btif_enable_service]+++!\n");
    /* If BT is enabled, we need to switch to BTIF context and trigger the
         * enable for that profile
         *
         * Otherwise, we just set the flag. On BT_Enable, the DM will trigger
         * enable for the profiles that have been enabled */

    btif_enabled_services |= (1 << service_id);

    if (btif_is_enabled())
    {
    	btmtk_enable_service(service_id);
    }
	ALOGI("---[btif_enable_service]---!\n");
    return BT_STATUS_SUCCESS;
}
/*******************************************************************************
**
** Function         btif_disable_service
**
** Description      Disables the service 'service_ID' to the service_mask.
**                  Upon BT disable, BTIF core shall invoke the BTA APIs to
**                  disable the profiles
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_disable_service(uint8_t service_id)
{
	ALOGI("+++[btif_disable_service]+++!\n");
	 /* If BT is enabled, we need to switch to BTIF context and trigger the
     * disable for that profile so that the appropriate uuid_property_changed will
     * be triggerred. Otherwise, we just need to clear the service_id in the mask
     */

    btif_enabled_services &=  (uint32_t)(~(1<<service_id));
	if (btif_is_enabled())
    {
    	btmtk_disable_service(service_id);
	}
	ALOGI("---[btif_disable_service]---!\n");
    return BT_STATUS_SUCCESS;
}


/*******************************************************************************
**
** Function         btif_config_hci_snoop_log
**
** Description      enable or disable HCI snoop log
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_config_hci_snoop_log(uint8_t enable)
{
    btmtk_config_hci_logging(enable);
    return BT_STATUS_SUCCESS;
}

