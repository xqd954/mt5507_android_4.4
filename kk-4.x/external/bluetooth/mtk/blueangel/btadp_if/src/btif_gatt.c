/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
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


/*******************************************************************************
 *
 *  Filename:      btif_gatt.c
 *
 *  Description:   GATT Profile Bluetooth Interface
 *
 *******************************************************************************/

#include <hardware/bluetooth.h>
#include <hardware/bt_gatt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define LOG_TAG "BTIF_GATT"
#include "bt_ext_common.h"
#include "btif_api.h"
#include "btif_common.h"
#include "bt_ext_debug.h"
#include "bt_ext_apis.h"


//#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
#if defined(__BT_4_0_BLE__)
#include "btif_gatt.h"
//#include "btif_gatt_util.h"

const btgatt_callbacks_t *bt_gatt_callbacks = NULL;

extern btgatt_client_interface_t btgattClientInterface;
extern btgatt_server_interface_t btgattServerInterface;

typedef struct btif_gatt_cb{
}btif_gatt_cb_t;

extern bt_status_t btif_gattc_init( const btgatt_client_callbacks_t *callbacks );
extern void  btif_gattc_cleanup(void);
extern bt_status_t btif_gatts_init( const btgatt_server_callbacks_t *callbacks );
extern void  btif_gatts_cleanup(void);

extern void gattc_handle_message(void *pContext, ilm_struct *pIlm);
extern void gatts_handle_message(void *pContext, ilm_struct *pIlm);

/*******************************************************************************
**
** Function         gatt_handle_message
**
** Description      Handle gatt message from internal adp
**
** Returns          void
**
*******************************************************************************/
static void gatt_handle_message(void *pContext, ilm_struct *pIlm){
    GATT_IF_API();
    if(pContext == (void*)PROFILE_GATTC){
        gattc_handle_message(pContext, pIlm);
    }else{
        gatts_handle_message(pContext, pIlm);
    }
}

/*******************************************************************************
**
** Function         btif_gatt_init
**
** Description      Initializes the GATT interface
**
** Returns    s      bt_status_t
**
*******************************************************************************/
static bt_status_t btif_gatt_init( const btgatt_callbacks_t* callbacks )
{
    bt_status_t status;
    
    GATT_IF_API();
    bt_gatt_callbacks = callbacks;

    status = btif_gattc_init(callbacks->client);
    status = btif_gatts_init(callbacks->server);
    
    btif_enable_service(BTMTK_BLE_SERVICE_ID);
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_gatt_cleanup
**
** Description      Closes the GATT interface
**
** Returns          void
**
*******************************************************************************/
static void  btif_gatt_cleanup( void )
{
    GATT_IF_API();
    if (bt_gatt_callbacks)
        bt_gatt_callbacks = NULL;

    btif_disable_service(BTMTK_BLE_SERVICE_ID);
}

static const btgatt_interface_t btgattInterface = {
    sizeof(btgattInterface),

    btif_gatt_init,
    btif_gatt_cleanup,

    &btgattClientInterface,
    &btgattServerInterface,
};

/*******************************************************************************
**
** Function         btif_gatt_get_interface
**
** Description      Get the gatt callback interface
**
** Returns          btgatt_interface_t
**
*******************************************************************************/
const btgatt_interface_t *btif_gatt_get_interface()
{
    GATT_IF_API();
    return &btgattInterface;
}

/*******************************************************************************
*   GATT utility
*******************************************************************************/
const U8 BT_BASE_UUID[16] = 
/*
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
    0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};
*/
{
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int convertToGattErrCode(U8 result){
    int status;
    switch(result){
    case 0:
        status = GATT_SUCCESS;
        break;
    case 2:
        status = GATT_PENDING;
        break;        
    default:
        status = GATT_ERROR;
        break;
    }
    return status;
}

void gattDumpUUID(char *prefix, uint8_t len, uint8_t *uuid){
    char buf[60], *ptr;
    int i;
    if(len){
        for(i = 0, ptr = buf;i < len;i++, ptr+=3){
            sprintf(ptr, "%02X:", uuid[i]);
        }
        *(ptr-1) = 0;
        if(prefix){
            GATT_IF_DBG("%s : uuid : len(%u) : %s", prefix, len, buf);
        }else{
            GATT_IF_DBG("uuid : len(%u) : %s", len, buf);
        }
    }else{
        GATT_IF_WARN("gattDumpUUID : len == 0");
    }
}

static void convertUUID(U8 *dest, U8 *src, U8 size){
    #ifdef __REVERSE_UUID__
    src += size;
    while(size--){
        *(dest++) = *(--src);
    }
    #else
    memcpy(dest, src, size);
    #endif
}
    
/* return : 0 means success, otherwise failed */
U8 convertBtIdToBTUUID(BT_UUID *dest, bt_uuid_t *src){
    GATT_IF_API();
    if(!dest){
        GATT_IF_WARN("dest is NULL");
    }else{
        if(src){
            gattDumpUUID("src", 16, src->uu);
            dest->len = 16;
            convertUUID(dest->uuid, src->uu, 16);
            gattDumpUUID("dest", dest->len, dest->uuid);
        }else{
            GATT_IF_DBG("src is NULL");
            memset(dest, 0, sizeof(*dest));
        }
        return 0;
    }
    return 1;
}

/* return : 0 means success, otherwise failed */
U8 convertBTUUIDToBtId(bt_uuid_t *dest, BT_UUID *src){
    GATT_IF_API();
    gattDumpUUID("src", src->len, src->uuid);
    if(!dest){
        GATT_IF_WARN("dest is NULL");
    }else{
        if(src){
            if(src->len == 2){
                U8 uuid128[16];
                memcpy(uuid128, BT_BASE_UUID, 16);
                /*
                uuid128[2] = src->uuid[1];
                uuid128[3] = src->uuid[0];
                */
                uuid128[13] = src->uuid[1];
                uuid128[12] = src->uuid[0];
                convertUUID(dest->uu, uuid128, sizeof(dest->uu));
            }else{
                convertUUID(dest->uu, src->uuid, 16);
            }
        }else{
            GATT_IF_DBG("src is NULL");
            memset(dest, 0, sizeof(*dest));
        }
        gattDumpUUID("dest", 16, dest->uu);        
        return 0;
    }
    return 1;
}


/* return : 0 means success, otherwise failed */
U8 convertCharIdToAttUUID(ATT_UUID *dest, btgatt_gatt_id_t *src){
    GATT_IF_API();
    if(!dest){
        GATT_IF_WARN("dest is NULL");
    }else{
        if(src){
            GATT_IF_DBG("inst_id=%u", src->inst_id);
            dest->inst = src->inst_id;
            convertBtIdToBTUUID(&dest->uuid, &src->uuid);
        }else{
            GATT_IF_DBG("src is NULL");
            memset(dest, 0, sizeof(*dest));
        }
        return 0;
    }
    return 1;
}

/* return : 0 means success, otherwise failed */
U8 convertAttUUIDToCharId(btgatt_gatt_id_t *dest, ATT_UUID *src){
    GATT_IF_API();
    if(!dest){
        GATT_IF_WARN("dest is NULL");
    }else{
        if(src){
            GATT_IF_DBG("inst_id = %u", src->inst);
            dest->inst_id = src->inst;
            convertBTUUIDToBtId(&dest->uuid, &src->uuid);
        }else{
            GATT_IF_DBG("src is NULL");
            memset(dest, 0, sizeof(*dest));
        }
        return 0;
    }
    return 1;
}

/* return : 0 means success, otherwise failed */
U8 convertSvcIdToSvcUUID(GATT_SVC_UUID *dest, btgatt_srvc_id_t *src){
    GATT_IF_API();
    if(!dest){
        GATT_IF_WARN("dest is NULL");
    }else{
        if(src){
            GATT_IF_DBG("is_primary=%u", src->is_primary);
            dest->is_primary = src->is_primary;
            convertCharIdToAttUUID(&dest->uuid, &src->id);
        }else{
            GATT_IF_DBG("src is NULL");
            memset(dest, 0, sizeof(*dest));
        }
        return 0;
    }
    return 1;
}

/* return : 0 means success, otherwise failed */
U8 convertSvcUUIDToSvcId(btgatt_srvc_id_t *dest, GATT_SVC_UUID *src){
    GATT_IF_API();
    if(!dest){
        GATT_IF_WARN("dest is NULL");
    }else{
        if(src){
            GATT_IF_DBG("is_primary=%u", src->is_primary);
            dest->is_primary = src->is_primary;
            convertAttUUIDToCharId(&dest->id, &src->uuid);
        }else{
            GATT_IF_DBG("src is NULL");
            memset(dest, 0, sizeof(*dest));
        }
        return 0;
    }
    return 1;
}

#endif
