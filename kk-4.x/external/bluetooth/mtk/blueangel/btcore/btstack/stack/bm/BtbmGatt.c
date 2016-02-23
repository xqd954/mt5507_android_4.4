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
#ifdef __BT_4_0_BLE__
#include "bt_common.h"
#include "btconfig.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "btutils.h"
#include "btconfig_le.h"
#include "btbm.h"
#include "btbm_int.h"
#include "me_adp.h"
#include "le_adp.h"
#include "att.h"
#include "attdef.h"
#include "gattcb.h"

#define BTBM_GATT_OP_NUM 10
#define BTBM_GATT_SEARCH_SERVICE_NUM 1

typedef enum
{
    BOP_GATT_NOP,
    BOP_GATT_WRITE_PRIVACY,
    BOP_GATT_WRITE_RECONNECT_ADDR,
    BOP_GATT_SERVICE_SEARCH_ALL
    /* reserved: BOP_REM_NAME_REQ = 0x11,*/
} gattOpType;

extern U8 GapAttDB_Registration(void);

typedef struct
{
    MeCommandToken *token;

    /* Below are used by GATT module */
    BtGattOp gattOp; /* gattOp.parms.p pointers to the field below */
    union 
    {
        BtGattDiscoveryAllPrimaryService discService;
        BtGattFindIncludeServices findService;
        BtGattDiscoveryAllCharacteristicDescriptors discCharDesc;
        BtGattDiscoveryPrimaryServiceByUUID discServiceUuid;
        BtGattReadCharacteristicUUID readCharUuid;
        BtGattReadCharacteristicValue readCharValue;
        BtGattWriteCharValueRequest writeCharValue;
        BtGattSignedWriteWithoutResponse signedWriteCharValue;
        BtGattReadByType readByType;
    } gattParm;
} btbmGattOp;

typedef struct
{
    U8 used;
    BtRemoteDevice *remDev; 
    BtCallBack callback;
} btbmBleNameOp;

typedef struct
{
    /* Below are used by GATT module */
    BtGattOp gattOp; /* gattOp.parms.p pointers to the field below */
    union 
    {
        BtGattDiscoveryAllPrimaryService discService;
        BtGattFindIncludeServices findService;
        BtGattDiscoveryAllCharacteristicDescriptors discCharDesc;
        BtGattDiscoveryPrimaryServiceByUUID discServiceUuid;
        BtGattReadCharacteristicUUID readCharUuid;
        BtGattReadCharacteristicValue readCharValue;
        BtGattWriteCharValueRequest writeCharValue;
        BtGattSignedWriteWithoutResponse signedWriteCharValue;
        BtGattReadByType readByType;
    } gattParm;

    U8 used;
    BtRemoteDevice *remDev; 
    U8 service_length;
    U32 service_uuid[BTBM_ADP_MAX_SDAP_UUID_NO];
	U8 service_find_no;
	EvmTimer disconnectTimer;
} btbmBleSearchServiceOp;



extern BMCONTENT_T BMController;

btbmGattOp btbmGattOps[BTBM_GATT_OP_NUM];
btbmBleNameOp btbmGattNameOps[BTBM_GATT_OP_NUM];

MeCommandToken btbmGattTokens[BTBM_GATT_OP_NUM];


btbmBleSearchServiceOp btbmGattServiceOps[BTBM_GATT_SEARCH_SERVICE_NUM];	

BtHandler btbmGattHandler;
extern BtStatus BT_GATT_DiscoveryAllPrimaryService(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
BtStatus BT_GATT_SignedWriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op);

extern U8 btbmBLEUtilCheckDeviceNameAvailable(BD_ADDR *bdAddr);
extern void btbmUpdateBleDeviceNameForConnectedDevice(BtRemoteDevice *link, U8 *name, U16 length);
extern void BTBMAdpServiceSearchComplete(kal_uint8 result, btbm_bd_addr_t addr, kal_uint8 no);

#ifdef __INDEPENDENT_INQUIRY_FLOW__
U8 btbmIsInquiryOngoing(void);
#endif
extern BTBMInquiredDevice *btbmUtilGetInquiredDevice(BD_ADDR *bdAddr);
extern void BTBMAdpServiceSearchResult(kal_uint32 uuid, btbm_bd_addr_t addr);


BtStatus BTBMGATT_BQEMakeCommandTestCode(btbmGattOp *op);

btbmBleNameOp *btbmGattGetFreeNameOp(void)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if(btbmGattNameOps[i].used == 0)
            return &btbmGattNameOps[i];
    }
    return 0;
}

void btbmGattFreeNameOp(btbmBleNameOp *op)
{
    op->used = 0;
    OS_MemSet((U8 *)op, 0, sizeof(btbmBleNameOp));
}

btbmBleSearchServiceOp *btbmGattGetFreeServiceOp(void)
{
    int i;
    for (i = 0; i < BTBM_GATT_SEARCH_SERVICE_NUM; i++)
    {
        if(btbmGattServiceOps[i].used == 0)
            return (btbmBleSearchServiceOp *)&btbmGattServiceOps[i];
    }
    return 0;
}


void btbmGattFreeServiceOp(btbmBleSearchServiceOp *op)
{
    op->used = 0;
    OS_MemSet((U8 *)op, 0, sizeof(btbmBleSearchServiceOp));
}



btbmGattOp *btbmGattGetServiceOp(btbmBleSearchServiceOp *gattOp)
{
    int i;
    for (i = 0; i < BTBM_GATT_SEARCH_SERVICE_NUM; i++)
    {
        if (gattOp == (btbmBleSearchServiceOp *)&btbmGattServiceOps[i].gattOp)
        {
            return (btbmGattOp *)&btbmGattServiceOps[i];
        }
    }

    return NULL;
}


btbmGattOp *btbmGattGetFreeOp(void)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (btbmGattOps[i].token == NULL)
        {
            OS_Report("btbmGattOps[%d]",i );
            return (btbmGattOp *)&btbmGattOps[i];
        }
    }

    Assert(0);
    return NULL;
}

MeCommandToken *btbmGattGetFreeToken(void)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (btbmGattTokens[i].op.opType == BOP_NOP)
        {
            OS_Report("btbmGattTokens[%d]",i );
            return &btbmGattTokens[i];
        }
    }

    Assert(0);
    return NULL;
}

void btbmGattFreeOp(btbmGattOp *op)
{
    if (op->token)
    {
        OS_MemSet((U8 *)op->token, 0x0, sizeof(MeCommandToken));
    }
    OS_MemSet((U8 *)op, 0x0, sizeof(btbmGattOp));
}

btbmGattOp *btbmGattGetOp(BtGattOp *gattOp)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (gattOp == &btbmGattOps[i].gattOp)
        {
            return &btbmGattOps[i];
        }
    }

    return NULL;
}

btbmGattOp *btbmGattGetOpByToken(MeCommandToken *tok)
{
    int i;
    for (i = 0; i < BTBM_GATT_OP_NUM; i++)
    {
        if (tok == btbmGattOps[i].token)
        {
            return &btbmGattOps[i];
        }
    }

    return NULL;
}

static void btbmGattRemoveLink(BtRemoteDevice *link)
{
    OS_Report("btbmGattRemoveLink link %d",link);
    if(link == 0)
        return;
    OS_Report("btbmGattRemoveLink state %d",link->state);
    OS_Report("address %02x,%02x,%02x,",link->bdAddr.addr[0],link->bdAddr.addr[1],link->bdAddr.addr[2]);    
    OS_Report("address %02x,%02x,%02x,",link->bdAddr.addr[3],link->bdAddr.addr[4],link->bdAddr.addr[5]);        
    if(link->state == BDS_DISCONNECTED)
        return;
    if (IsNodeOnList(&link->handlers, &btbmGattHandler.node))
    {
        ME_LE_DiscLink(&btbmGattHandler, &link->bdAddr, BEC_USER_TERMINATED);
    }
    else
    {
        Report(("The link handler is not insert"));
    }
}


static void btbmGattForceRemoveLink(BtRemoteDevice *link)
{
    OS_Report("btbmGattForceRemoveLink link %d",link);
    if(link == 0)
        return;
    OS_Report("btbmGattRemoveLink state %d",link->state);
    OS_Report("address %02x,%02x,%02x,",link->bdAddr.addr[0],link->bdAddr.addr[1],link->bdAddr.addr[2]);    
    OS_Report("address %02x,%02x,%02x,",link->bdAddr.addr[3],link->bdAddr.addr[4],link->bdAddr.addr[5]);        
    if(link->state == BDS_DISCONNECTED)
        return;
    if (IsNodeOnList(&link->handlers, &btbmGattHandler.node))
    {
        ME_LE_ForceDiscLink(&btbmGattHandler, &link->bdAddr, BEC_USER_TERMINATED);
    }
}

static void btbmGattNameCallback(const BtEvent *Event)
{
    BtDeviceContext *bdc;
    int i;
    U8 error_code;
    btbmBleNameOp *op;
    BtEvent nameCbEvent;
    MeCommandToken token;
    BtRemoteDevice *link = 0;
    
    OS_Report("btbmGattNameCallback event %d", Event->eType);
    switch (Event->eType)
    {
        case BTEVENT_LINK_CONNECT_CNF:
            Report(("BLE connected confirmed, trigger another BLE search again"));
            for(i=0;i< BTBM_GATT_OP_NUM;i++)
            {
                op = &btbmGattNameOps[i];
                if ((op->used == 1)  &&
                    (AreBdAddrsEqual(&Event->p.remDev->bdAddr, 
                                    &op->remDev->bdAddr)))

                {
                    link = op->remDev;
                    break;
                }
            }
            if(link == 0)
            {
                Report(("No name token"));
                return;

            }
            if (Event->errCode != BEC_NO_ERROR)
            {
                EVM_ResetTimer(&link->bleNameReqTimer);
                link->bleNameReqTimer.func = 0;
                OS_MemSet((U8*) & nameCbEvent, 0, sizeof(nameCbEvent));
                nameCbEvent.p.meToken = &token;
                token.remDev = link;
                token.callback = op->callback;
				nameCbEvent.eType = BTEVENT_NAME_RESULT; 

                nameCbEvent.errCode = BEC_NO_CONNECTION;
                token.p.name.io.out.len = 0;
                token.p.name.io.out.name = NULL;
                op->callback(&nameCbEvent);
                btbmGattFreeNameOp(op);
            }

            break;

    }
}

static void btbmGattDummyCallback(const BtEvent *Event)
{
}

static void btbmGattCallback(const BtEvent *Event)
{
    btbmGattOp *op;
    BtDeviceContext *bdc;
    BtEvent cbEvent;
    int i;
    U8 error_code;
    
    OS_Report("btbmGattCallback event %d", Event->eType);
    switch (Event->eType)
    {
#if 1
        case BTEVENT_LINK_CONNECT_CNF:
            
            /* Only handle the failed cases, success case wait the BTEVENT_GATT_ATT_READY to handle that */
            for (i = 0; i < BTBM_GATT_OP_NUM; i++)
            {
                if (btbmGattOps[i].token &&
                    AreBdAddrsEqual(&Event->p.remDev->bdAddr, &btbmGattOps[i].token->remDev->bdAddr))
                {
                    op = &btbmGattOps[i];
                    switch (op->token->op.opType)
                    {
                    case BOP_GATT_WRITE_PRIVACY:
                        if (Event->errCode != BEC_NO_ERROR)
                        {
                            cbEvent.errCode = BEC_NO_CONNECTION;
                            cbEvent.eType = op->token->eType;
                            cbEvent.p.meToken = op->token;
                            op->token->callback(&cbEvent);
                            btbmGattFreeOp(op);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
#endif            
        case BTEVENT_GATT_ATT_READY:
            if (Event->errCode == BEC_NO_ERROR && (bdc = DS_FindDevice(&Event->p.remDev->bdAddr)) != 0)
            {
                bdc->link = Event->p.remDev;
            }
            Report(("bm operativity:%02x",BMController.bm_opactivity));
            
            for (i = 0; i < BTBM_GATT_OP_NUM; i++)
            {
                if (btbmGattOps[i].token &&
                    AreBdAddrsEqual(&Event->p.remDev->bdAddr, &btbmGattOps[i].token->remDev->bdAddr))
                {
                    op = &btbmGattOps[i];
                    switch (op->token->op.opType)
                    {
                    case BOP_GATT_WRITE_PRIVACY:
                        if (Event->errCode == BEC_NO_ERROR)
                        {
                            BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
                        }
                        else
                        {
                            cbEvent.errCode = BEC_NO_CONNECTION;
                            cbEvent.eType = op->token->eType;
                            cbEvent.p.meToken = op->token;
                            op->token->callback(&cbEvent);
                            btbmGattFreeOp(op);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            break;
        case BTEVENT_LINK_DISCONNECT:
                break;
    }
}


static void btbmGattSearchServiceCallback(const BtEvent *Event)
{
    btbmGattOp *op;
    BtDeviceContext *bdc;
    BtEvent cbEvent;
    int i;
    U8 error_code;
    
    OS_Report("btbmGattCallback event %d", Event->eType);
    switch (Event->eType)
    {
        case BTEVENT_LINK_CONNECT_CNF:
            
            /* Only handle the failed cases, success case wait the BTEVENT_GATT_ATT_READY to handle that */
            for (i = 0; i < BTBM_GATT_OP_NUM; i++)
            {
                if (btbmGattOps[i].token &&
                    AreBdAddrsEqual(&Event->p.remDev->bdAddr, &btbmGattOps[i].token->remDev->bdAddr))
                {
                    op = &btbmGattOps[i];
                    switch (op->token->op.opType)
                    {
                        case BOP_GATT_SERVICE_SEARCH_ALL:
                            if (Event->errCode != BEC_NO_ERROR)
                            {
                                cbEvent.errCode = BEC_NO_CONNECTION;
                                cbEvent.eType = op->token->eType;
                                cbEvent.p.meToken = op->token;                        
                                op->token->callback(&cbEvent);
                                btbmGattFreeOp(op);
                                /* If create link failed, try again. If create link success, wait GATT ready to query service */
                                Report(("Service search BLE link completed, fast search service again"));
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

            break;
        case BTEVENT_GATT_ATT_READY:
            if (Event->errCode == BEC_NO_ERROR && (bdc = DS_FindDevice(&Event->p.remDev->bdAddr)) != 0)
            {
                bdc->link = Event->p.remDev;
            }
            Report(("bm operativity:%02x",BMController.bm_opactivity));
            
            for (i = 0; i < BTBM_GATT_OP_NUM; i++)
            {
                if (btbmGattOps[i].token &&
                    AreBdAddrsEqual(&Event->p.remDev->bdAddr, &btbmGattOps[i].token->remDev->bdAddr))
                {
                    op = &btbmGattOps[i];
                    switch (op->token->op.opType)
                    {
                        case BOP_GATT_SERVICE_SEARCH_ALL:
                            if (Event->errCode != BEC_NO_ERROR)
                            {
                                cbEvent.errCode = BEC_NO_CONNECTION;
                                cbEvent.eType = op->token->eType;
                                cbEvent.p.meToken = op->token;                        
                                op->token->callback(&cbEvent);
                                btbmGattFreeOp(op);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            if (Event->errCode == BEC_NO_ERROR)
            {
                /* If create link failed, try again. If create link success, wait GATT ready to query service */
                Report(("Service search BLE link completed, fast search service again"));
                BT_GATT_DiscoveryAllPrimaryService(op->token->remDev, &op->gattOp);
                return;

            }            
            break;
        case BTEVENT_LINK_DISCONNECT:
                break;
    }
}

static void btbmGattGetRemoateDeviceNameCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattGetRemoateDeviceNameCallBack():%d", event->eType));
    
    if (op == NULL)
    {
        return;
    }
    else if(op->token == NULL)  /* already callback */
    {
        btbmGattFreeOp(op);
        return;
    }
    else
    {
        switch(event->eType)
        {
            case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
            {
                /* GAP service handle range discovered, then read value */
                BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
                
                Report(("result->flag:%d, result->length:%d", result->flag, result->length));
                EVM_CancelTimer(&op->token->tokenTimer);
                if (result->flag == BT_GATT_EVENT_RESULT_CONTINUE_DATA)
                {
                    btbmGattRemoveLink(op->token->remDev);
                    cbEvent.eType = op->token->eType;  /* BTEVENT_NAME_RESULT */
                    cbEvent.errCode = BEC_NO_ERROR;
                    op->token->p.name.io.out.len = result->length;
                    op->token->p.name.io.out.name = result->data;
                    cbEvent.p.meToken = op->token;
                    op->token->callback(&cbEvent);
                    op->token = NULL;
                }
                else
                {
                    goto error;
                }
            }
            break;
    
            default:
                break;;
        }
        return;
    }

error:
    btbmGattForceRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;  /* BTEVENT_NAME_RESULT */
    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
    op->token->p.name.io.out.len = 0;
    op->token->p.name.io.out.name = NULL;
    cbEvent.p.meToken = op->token;
    op->token->callback(&cbEvent);
    btbmGattFreeOp(op);
}


static void btbmGattServiceSearchTimeout(EvmTimer *timer)
{
    btbmBleSearchServiceOp *op = (btbmBleSearchServiceOp *)timer->context;
    BtRemoteDevice *link;
    BtRemoteDevice *temp_link;

    btbm_bd_addr_t bd_addr;
    link = op->remDev;
	
    if ((link->state == BDS_CONNECTED) && (!IsNodeOnList(&link->handlers, &btbmGattHandler.node)))
    {	
        ME_InitHandler(&btbmGattHandler);
        btbmGattHandler.callback = btbmGattDummyCallback;    
        ME_LE_CreateLink(&btbmGattHandler, &link->bdAddr, NULL, &temp_link);
    }
	btbmGattRemoveLink(link);

	/* op is not return, let disconnect event report that.*/

}


void btbmGattServiceSearchDisconnectTimout(EvmTimer *timer)
{
	BtRemoteDevice *link;
    BtRemoteDevice *temp_link;

	link = (BtRemoteDevice*)timer->context;
    if ((link->state == BDS_CONNECTED) && (!IsNodeOnList(&link->handlers, &btbmGattHandler.node)))
    {	
    	
        ME_InitHandler(&btbmGattHandler);
        btbmGattHandler.callback = btbmGattDummyCallback;    
    	Report(("Insert link handler"));
        ME_LE_CreateLink(&btbmGattHandler, &link->bdAddr, NULL, &temp_link);
	}
    if(link->state == BDS_CONNECTED)
        btbmGattRemoveLink(link);
    
}

static void btbmGattServiceSearchAllCallBack(void *userdata, BtGattEvent *event)
{
    btbmBleSearchServiceOp *op = (btbmBleSearchServiceOp *)btbmGattGetServiceOp((btbmBleSearchServiceOp *)userdata);
    BtEvent cbEvent;
    BtRemoteDevice *link;

    btbm_bd_addr_t bd_addr;

    Report(("btbmGattServiceSearchAllCallBack():%d", event->eType));
    
    if (op == NULL)
    {
        return;
    }
    else
    {
    	link = op->remDev;
        CONVERT_ARRAY2BDADDR((bt_addr_struct *)&bd_addr, (U8 *)link->bdAddr.addr);
		
        switch(event->eType)
        {
            case BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE:
            {
                /* GAP service handle range discovered, then read value */
                BtGattDiscoveryAllPrimaryServiceResultEvent *result = event->parms;
                
                Report(("result->flag:%d, handle 0x%x~0x%x, type=%d, uuid2=0x%x", 
                    result->flag, result->startHandle, result->endHandle, result->type, result->uuid2));
                if (result->flag == BT_GATT_EVENT_RESULT_CONTINUE_DATA)
                {
                    if (result->type == BT_UUID16_SIZE)
                    {
                        if (result->uuid2 >= 0x1800 && result->uuid2 < 0x1820)
                        {
                            U8 i=0;
							
                            Report(("op->uuid2:%04x", result->uuid2));

                            BTBMAdpServiceSearchResult(result->uuid2, 
                                                bd_addr);
                            op->service_find_no++;

/*
                            for (i=0; i<op->service_length; i++)
                            {
                                if(op->service_uuid[i] == result->uuid2)
                                {
                                    BTBMAdpServiceSearchResult(result->uuid2, 
                                                        bd_addr);
                                    op->service_find_no++;

                                }
                            }
*/                            
                        }
                    }
                    else if (result->type == BT_UUID128_SIZE)
                    {
                    }
                    else
                    {
                        Assert(0);
                    }
                }
                else
                {
                    Report(("Discovery all primary service done"));
                    EVM_ResetTimer(&op->disconnectTimer);
                    BTBMAdpServiceSearchComplete(BTBM_ADP_SUCCESS, 
                                            bd_addr,
                                            op->service_find_no);
                    goto search_end;
                }
                break;
            }
            default:
                break;;
        }
        return;
    }

search_end:
    btbmGattFreeServiceOp(op);
	EVM_ResetTimer(&link->bleServiceRequestDisconnectTimer);
	link->bleServiceRequestDisconnectTimer.context = link;
	link->bleServiceRequestDisconnectTimer.func = btbmGattServiceSearchDisconnectTimout;
	EVM_StartTimer(&link->bleServiceRequestDisconnectTimer,10000);

}

static void btbmGattWriteRemotePrivacyCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattWriteRemotePrivacyCallBack():%d", event->eType));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
        {
            /* value and handle is read, then write */
            BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
            
            Report(("result->flag:%d, result->length:%d", result->flag, result->length));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    if (result->length == 1 && *result->data == op->token->p.gattReq.u.privacy)
                    {
                    }
                    else
                    {
                        btbmGattOp *op2 = btbmGattGetFreeOp();
                        BtGattWriteCharValueRequest *parm = &op2->gattParm.writeCharValue;
                        
                        InitializeListEntry(&op2->gattOp.op.node);
                        op2->token = op->token;
                        op->token = NULL;  /* op2 continue token operation */
                        op2->gattOp.apCb = btbmGattGetRemoateDeviceNameCallBack;
                        parm->handle = result->handle;
                        parm->size = 0x1;
                        parm->data = &op2->token->p.gattReq.u.privacy;
                        op2->gattOp.parms.p.writeRequest = parm;
                        BT_GATT_WriteCharacteristicValue(op2->token->remDev, &op2->gattOp);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    if (op->token)  /* operation done */
                    {
                        cbEvent.errCode = BEC_NO_ERROR;
                        goto done;
                    }
                    else /* op2 operation continuing */
                    {
                         btbmGattFreeOp(op);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }
        }
        break;

        case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
        {
            /* value and handle is read, then write */
            BtGattWriteCharValueResultEvent *result = event->parms;
            
            Report(("result->flag:%d", result->flag));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    cbEvent.errCode = BEC_NO_ERROR;
                    goto done;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }

       }
        break;

        default:
            break;;
    }
    return;

done:
    btbmGattRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;
//    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
    if (op->token->callback)
    {
        op->token->callback(&cbEvent);
    }
    btbmGattFreeOp(op);
}

static void btbmGattWriteRemoteReconnectAddressCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("btbmGattWriteRemoteReconnectAddressCallBack():%d", event->eType));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
        {
            /* value and handle is read, then write */
            BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
            
            Report(("result->flag:%d, result->length:%d", result->flag, result->length));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    if (result->length == 6 && OS_MemCmp(result->data, 6, op->token->p.gattReq.u.recon.addr, 6))
                    {
                    }
                    else
                    {
                        btbmGattOp *op2 = btbmGattGetFreeOp();
                        BtGattWriteCharValueRequest *parm = &op2->gattParm.writeCharValue;
                        
                        InitializeListEntry(&op2->gattOp.op.node);
                        op2->token = op->token;
                        op->token = NULL;  /* op2 continue token operation */
                        op2->gattOp.apCb = btbmGattWriteRemoteReconnectAddressCallBack;
                        parm->handle = result->handle;
                        parm->size = BD_ADDR_SIZE;
                        parm->data = op->token->p.gattReq.u.recon.addr;
                        op2->gattOp.parms.p.writeRequest = parm;
                        BT_GATT_WriteCharacteristicValue(op2->token->remDev, &op2->gattOp);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    if (op->token)  /* operation done */
                    {
                        cbEvent.errCode = BEC_NO_ERROR;
                        goto done;
                    }
                    else /* op2 operation continuing */
                    {
                         btbmGattFreeOp(op);
                    }
                    break;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }
        }
        break;

        case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
        {
            /* value and handle is read, then write */
            BtGattWriteCharValueResultEvent *result = event->parms;
            
            Report(("result->flag:%d", result->flag));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    cbEvent.errCode = BEC_NO_ERROR;
                    goto done;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }

       }
        break;

        default:
            break;;
    }
    return;

done:
    btbmGattRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;
//    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
    if (op->token->callback)
    {
        op->token->callback(&cbEvent);
    }
    btbmGattFreeOp(op);
}

static void btbmGattHandleRemNameReqTimeout(EvmTimer *timer)
{
    BtStatus status;
    BtEvent nameCbEvent;
    btbmBleNameOp *op = timer->context;
    BtRemoteDevice *link;
    BTBMRemoteNameSetting_T *item;
    U8 *name = NULL;
    U16 length = 0xffff;
    MeCommandToken token;
    
    link = op->remDev;

    item = btbmUtilConnectedDeviceNameEntryLookUp(&link->bdAddr);
    
    if(item ==0)
    {
        EVM_StartTimer(&link->bleNameReqTimer, 500);
        Report(("Wait name request completed"));
        return;
    }
    else
    {
        Report(("name request completed"));
        name = (U8 *)item->dev_name;
        length = item->dev_name_len;
    }

    EVM_ResetTimer(timer);
    OS_MemSet((U8*) & nameCbEvent, 0, sizeof(nameCbEvent));
    if(length == 0xffff)
        nameCbEvent.errCode = BEC_HOST_TIMEOUT;
    else
        nameCbEvent.errCode = BEC_NO_ERROR;
    nameCbEvent.p.meToken = &token;
    nameCbEvent.eType = BTEVENT_NAME_RESULT; 
    token.remDev = link;
    token.callback = op->callback;
    token.p.name.io.out.name = name;
    token.p.name.io.out.len = length;
    token.p.name.bdAddr = link->bdAddr;

    op->callback(&nameCbEvent);

    btbmGattFreeNameOp(op);

}



void btbmGattHandleRemNameReqTimeoutCompleted(EvmTimer *timer)
{
    BtStatus status;
    BtEvent nameCbEvent;
    btbmBleNameOp *op;
    BtRemoteDevice *link;
    BTBMRemoteNameSetting_T *item;
    BTBMInquiredDevice *inqResult;
    U8 *name = NULL;
    U16 length = 0xffff;
    MeCommandToken token;
    
    OS_Report("btbmGattHandleRemNameReqTimeoutCompleted %08x",timer);

	if(timer->func == 0)
		return;

    op = timer->context;
    link = op->remDev;
    Report(("address:%02x,%02x,%02x,%02x,%02x,%02x",
            link->bdAddr.addr[0],
            link->bdAddr.addr[1],
            link->bdAddr.addr[2],
            link->bdAddr.addr[3],
            link->bdAddr.addr[4],
            link->bdAddr.addr[5]));
    item = btbmUtilConnectedDeviceNameEntryLookUp(&link->bdAddr);
    Report(("btbmBLEUtilCheckDeviceNameAvailable item:%08x",item));
    
    if(item !=0)
    {
        name = (U8 *)item->dev_name;
        length = item->dev_name_len;
    }

    EVM_ResetTimer(timer);
    Report(("Name request timer stop"));

    OS_MemSet((U8*) & nameCbEvent, 0, sizeof(nameCbEvent));
    if(length == 0xffff)
        nameCbEvent.errCode = BEC_HOST_TIMEOUT;
    else
        nameCbEvent.errCode = BEC_NO_ERROR;
    nameCbEvent.p.meToken = &token;
    token.remDev = link;
	nameCbEvent.eType = BTEVENT_NAME_RESULT; 
	MEC(btEvent) = nameCbEvent;
    token.callback = op->callback;
    token.p.name.io.out.name = name;
    token.p.name.io.out.len = length;
    token.p.name.bdAddr = link->bdAddr;

    op->callback(&nameCbEvent);
    btbmGattFreeNameOp(op);

}


/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_Init
 * DESCRIPTION
 *  Module init
 * PARAMETERS
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_Init(void)
{
    OS_MemSet((U8 *)btbmGattOps, 0x0, sizeof(btbmGattOp) * BTBM_GATT_OP_NUM);
    OS_MemSet((U8 *)btbmGattTokens, 0x0, sizeof(MeCommandToken) * BTBM_GATT_OP_NUM);
    OS_MemSet((U8 *)btbmGattNameOps, 0x0, sizeof(btbmBleNameOp) * BTBM_GATT_OP_NUM);
    GapAttDB_Registration();
    return BT_STATUS_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_GetRemoteDeviceName
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_GetRemoteDeviceName(BtDeviceContext *bdc, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmBleNameOp *op;
    U32 timeOutValue;
    BTBMRemoteNameSetting_T *item;
    U8 result;
    
    op = btbmGattGetFreeNameOp();
    if(op == 0)
        return BT_STATUS_FAILED;

    op->callback = callback;
    op->used = 1;
    if ((op->remDev = ME_FindRemoteDevice(&bdc->addr)) == NULL)
    {
        op->remDev->bleNameReqTimer.func = btbmGattHandleRemNameReqTimeout;
        op->remDev->bleNameReqTimer.context = op;

        ME_InitHandler(&btbmGattHandler);
        btbmGattHandler.callback = btbmGattNameCallback;
        timeOutValue = 10000; /* 10 seconds */
        status = ME_LE_CreateLink(&btbmGattHandler, &bdc->addr, NULL, &op->remDev);
        if(status != BT_STATUS_FAILED)
            EVM_StartTimer(&op->remDev->bleNameReqTimer, timeOutValue); 
    }
    else
    {
        op->remDev->bleNameReqTimer.func = btbmGattHandleRemNameReqTimeout;
        op->remDev->bleNameReqTimer.context = op;
        timeOutValue = 500;

        result = btbmBLEUtilCheckDeviceNameAvailable(&bdc->addr);
        status = BT_STATUS_PENDING;
        EVM_StartTimer(&op->remDev->bleNameReqTimer, timeOutValue); 
    }

done:
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_ServiceSearchAll
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  token       [IN]        
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_ServiceSearchAll(BtRemoteDevice *link, U8 *uuid, U8 size)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmBleSearchServiceOp *op;
    BtGattDiscoveryAllPrimaryService *parm;

    OS_Report("BTBMGATT_ServiceSearchAll");

    op = btbmGattGetFreeServiceOp();
    if(op == NULL)
        return BT_STATUS_FAILED;
    op->remDev = link;
    /* Set up callback token */
	op->service_length = size;
	OS_MemSet((U8*) op->service_uuid, 0, sizeof(U32)*BTBM_ADP_MAX_SDAP_UUID_NO);
    InitializeListEntry(&op->gattOp.op.node);
    op->gattOp.apCb = btbmGattServiceSearchAllCallBack;
    op->service_find_no = 0;
    parm = &op->gattParm.discService;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    op->gattOp.parms.p.allPrimaryService = parm;
    if (link->state == BDS_CONNECTED)
    {
        /* Discover GAP service handle range */
        BT_GATT_DiscoveryAllPrimaryService(link, &op->gattOp);
        op->disconnectTimer.func = 0;
        EVM_ResetTimer(&op->disconnectTimer);
        op->disconnectTimer.func = btbmGattServiceSearchTimeout;
        op->disconnectTimer.context = (void *)op;
        EVM_StartTimer(&op->disconnectTimer, 10000);
        status = BT_STATUS_PENDING;
    }
    else
    {
        status = BT_STATUS_FAILED;
    }
    return status;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_WriteRemotePrivacy
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  privacy       [IN]        
 *  callback      [IN]
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_WriteRemotePrivacy(BD_ADDR *bdAddr,BOOL privacy, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;
    BtGattReadCharacteristicUUID *parm;

    OS_Report("BTBMGATT_WriteRemotePrivacy: privacy=%d", privacy);
    if (callback == NULL)
    {
        OS_Report("BTBMGATT_WriteRemotePrivacy: NULL callback");
    }

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_WRITE_PRIVACY;
    op->token->eType = BTEVENT_GATT_WRITE_PRIVACY;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->p.gattReq.bdAddr = *bdAddr;
    op->token->p.gattReq.u.privacy = privacy;
    op->token->callback = callback;
    op->gattOp.apCb = btbmGattWriteRemotePrivacyCallBack;
    parm = &op->gattParm.readCharUuid;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16 = BT_UUID_GATT_CHAR_TYPE_PERIPHERAL_PRIVACY_FLAG;
    op->gattOp.parms.p.readCharUUID = parm;

    if ((op->token->remDev = ME_FindRemoteDevice(bdAddr)) == NULL)
    {
        btbmGattHandler.callback = btbmGattCallback;
        ME_LE_CreateLink(&btbmGattHandler, bdAddr, NULL, &op->token->remDev);
    }
    else
    {
        /* Discover GAP service handle range */
        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
    }
    
    return BT_STATUS_PENDING;
}

/*****************************************************************************
 * FUNCTION
 *  BTBMGATT_WriteRemoteReconnectAddress
 * DESCRIPTION
 *  Get the remote device name.
 * PARAMETERS
 *  privacy       [IN]        
 *  callback      [IN]
 * RETURNS
 *  status of the operation.
 *****************************************************************************/
BtStatus BTBMGATT_WriteRemoteReconnectAddress(BD_ADDR *bdAddr, BD_ADDR *reconAddr, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;
    BtGattReadCharacteristicUUID *parm;

    OS_Report("BTBMGATT_WriteRemoteReconnectAddress");
    BTBMDumpBdAddr(bdAddr->addr);
    BTBMDumpBdAddr(reconAddr->addr);
    if (callback == NULL)
    {
        OS_Report("BTBMGATT_WriteRemoteReconnectAddress: NULL callback");
    }

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_WRITE_RECONNECT_ADDR;
    op->token->eType = BTEVENT_GATT_WRITE_RECONNECT_ADDR;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->p.gattReq.bdAddr = *bdAddr;
    op->token->p.gattReq.u.recon = *reconAddr;
    op->token->callback = callback;
    op->gattOp.apCb = btbmGattWriteRemoteReconnectAddressCallBack;
    parm = &op->gattParm.readCharUuid;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16 = BT_UUID_GATT_CHAR_TYPE_RECONNECTION_ADDRESS;
    op->gattOp.parms.p.readCharUUID = parm;

    if ((op->token->remDev = ME_FindRemoteDevice(bdAddr)) == NULL)
    {
        btbmGattHandler.callback = btbmGattCallback;
        ME_LE_CreateLink(&btbmGattHandler, bdAddr, NULL, &op->token->remDev);
    }
    else
    {
        /* Discover GAP service handle range */
        BT_GATT_ReadUsingCharacteristicUUID(op->token->remDev, &op->gattOp);
    }
    
    return BT_STATUS_PENDING;
}

BtStatus BTBMGATT_Cancel(BtRemoteDevice *link)
{
    btbmGattForceRemoveLink(link);
    return BT_STATUS_SUCCESS;
}

static void UPF_SignedWriteCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;

    Report(("UPF_SignedWriteCallBack():%d", event->eType));
    switch(event->eType)
    {
        case BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE:
        {
            /* value and handle is read, then write */
            BtGattWriteCharValueResultEvent *result = event->parms;
            
            Report(("result->flag:%d", result->flag));
            switch(result->flag)
            {
                case BT_GATT_EVENT_RESULT_CONTINUE_DATA:
                    break;
                case BT_GATT_EVENT_RESULT_DONE_COMPLETED:
                    cbEvent.errCode = BEC_NO_ERROR;
                    goto done;
                case BT_GATT_EVENT_RESULT_ERROR:
                    cbEvent.errCode = BEC_UNSPECIFIED_ERR;
                    goto done;
                case BT_GATT_EVENT_RESULT_DISCONNECT:
                    cbEvent.errCode = BEC_NO_CONNECTION;
                    goto done;
            }

       }
        break;

        default:
            break;;
    }
    return;

done:
    btbmGattFreeOp(op);
}

void UPF_SignedWrite(btbm_bd_addr_t *addr)
{
    U16 handle = 0;
    U8 size = 0;
    U8 data[64];
    BD_ADDR bdAddr;
    BtRemoteDevice *remDev; 
    btbmGattOp *op;
    BtGattSignedWriteWithoutResponse *parm;

    OS_Report("UPF_SignedWrite");
    CONVERT_BDADDRSRC2ARRAY(bdAddr.addr, addr);
    BTBMDumpBdAddr(bdAddr.addr);

    remDev = ME_FindRemoteDevice(&bdAddr);
    if (remDev)
    {
        op = btbmGattGetFreeOp();
        parm = &op->gattParm.signedWriteCharValue;
        InitializeListEntry(&op->gattOp.op.node);
        op->token = btbmGattGetFreeToken();
        op->gattOp.apCb = UPF_SignedWriteCallBack;
//        DDB_ReadUPF_ATT(&handle, &size, data);
        parm->handle = handle;
        parm->size = size;
        parm->data = data;
        op->token->remDev = remDev;
        op->gattOp.parms.p.signedWriteWithoutResponse = parm;
        BT_GATT_SignedWriteWithoutResponse(op->token->remDev, &op->gattOp);
    }
}


U8 BtbmAttHandleDeviceName(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    U8 status = BT_STATUS_SUCCESS;
    bt_bm_write_local_name_req_struct req;

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            /* Read */
            *len = BMController.bm_setting.len;
            *raw = (U8 *)BMController.bm_setting.local_name;
			OS_Report("[GAP] read name %s", BMController.bm_setting.local_name);
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:            
            /* Write */
            OS_Report("[GAP] write name %s",  *raw);
            req.name_len = *len;
            OS_MemSet(req.name, 0, BTBM_ADP_MAX_NAME_LEN);
            OS_MemCopy(req.name, *raw, *len);
            BTBMGapWriteLocalName(&req);
            break;
    }
    return status;
}

U8 BtbmAttHandleAppearance(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    U8 status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            *len = 2;
            *raw = BMController.bm_setting.appearance;
			OS_Report("[GAP] read appearance 0x%x, 0x%x", BMController.bm_setting.appearance[0], BMController.bm_setting.appearance[1]);
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:            
            break;
    }
    return status;
}

#if defined(__BT_4_0_PRIVACY__) && defined(__BT_LE_STANDALONE__)
U8 privacy[] = {0x00, 0x00, 0x00, 0x00, 0x00,0x00};

U8 BtbmAttHandlePrivacy(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    U8 status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            *len = 1;
            *raw = privacy;
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            break;
    }
    return status;
}

U8 BtbmAttHandleReconnectAddress(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw)
{
    U8 status = BT_STATUS_SUCCESS;

    switch (type)
    {
        case ATT_OP_READ:
        case ATT_OP_READ_LONG:
        case ATT_OP_READ_MULTIPLE:
        case ATT_OP_READ_BY_TYPE:
            *len = 6;
            *raw = privacy;            
            break;
        case ATT_OP_WRITE:
        case ATT_OP_EXECUTE_WRITE:
            break;
    }
    return status;
}

#endif


static void btbmConnectedRemoteDeviceNameCallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    Report(("btbmConnectedRemoteDeviceNameCallBack():%d", event->eType));
    
    if (op == NULL)
    {
        return;
    }
    else if(op->token == NULL)  /* already callback */
    {
        btbmGattFreeOp(op);
        return;
    }
    else
    {
        switch(event->eType)
        {
            case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
            {
                /* GAP service handle range discovered, then read value */
                BtGattReadCharacteristicUUIDResultEvent *result = event->parms;
                
                Report(("result->flag:%d, result->length:%d", result->flag, result->length));
                if (result->flag == BT_GATT_EVENT_RESULT_CONTINUE_DATA)
                {
                    btbmUpdateBleDeviceNameForConnectedDevice(op->token->remDev, result->data, result->length);
                }
                else
                {
                    goto error;
                }
            }
            break;
    
            default:
                break;;
        }
        return;
    }

error:

    btbmGattFreeOp(op);
}


void btbmHandlerGattReadyToAskDeviceName(BtRemoteDevice *link)
{
    btbmGattOp *op;
    BtGattReadCharacteristicUUID *parm;

    Report(("btbmHandlerGattReadyToAskDeviceName"));
    op = btbmGattGetFreeOp();
    parm = &op->gattParm.readCharUuid;
    InitializeListEntry(&op->gattOp.op.node);
    //op->token = (MeCommandToken *)link; /* temp reuse the token */
    op->token = btbmGattGetFreeToken();
    if (op->token)
    {
        op->token->remDev = link;
    }
    op->gattOp.apCb = btbmConnectedRemoteDeviceNameCallBack;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16 = BT_UUID_GATT_CHAR_TYPE_DEVICE_NAME;
    op->gattOp.parms.p.readCharUUID = parm;
    BT_GATT_ReadUsingCharacteristicUUID(link, &op->gattOp);
}


#if 0


U8 test_count = 0;

void btbmGattBQECallBack(void *userdata, BtGattEvent *event)
{
    btbmGattOp *op = btbmGattGetOp((BtGattOp *)userdata);
    BtEvent cbEvent;
    BTBMInquiredDevice *item;
    BtDeviceContext *bdc;

    BtGattDiscoveryCharacteristicByUUID *parm2;
    BtGattReadByType *parm3;

    Report(("btbmGattBQECallBack():%d", event->eType));
    
    if (op == NULL)
    {
        return;
    }
    else if(op->token == NULL)  /* already callback */
    {
        btbmGattFreeOp(op);
        return;
    }
    else
    {
        switch(event->eType)
        {
            case BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE:
            {
                /* GAP service handle range discovered, then read value */
                BtGattFindIncludeServicesResultEvent *result = event->parms;
                
                Report(("result->flag:%d", result->flag));
                EVM_CancelTimer(&op->token->tokenTimer);
                if (result->flag == BT_GATT_EVENT_RESULT_DONE_COMPLETED)
                {
                    Report(("Get include servicedone."));
                    /* Set up callback token */
                    InitializeListEntry(&op->gattOp.op.node);
                    op->gattOp.apCb = btbmGattBQECallBack;
                    parm2 = &op->gattParm.discCharDesc;
                    parm2->startHandle = 0x0001;
                    parm2->endHandle = 0xFFFF;
					parm2->uuid16[0] = 0x2A;
					parm2->uuid16[1] = 0x04;
					parm2->type = 2;
                    op->gattOp.parms.p.charByUUID = parm2;
                    BT_GATT_DiscoveryCharacteristicByUUID(op->token->remDev, &op->gattOp);
                    return;
                }
                break;
            }        
            case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
            {
                /* GAP service handle range discovered, then read value */
                BtGattFindIncludeServicesResultEvent *result = event->parms;
                
                Report(("result->flag:%d", result->flag));
                EVM_CancelTimer(&op->token->tokenTimer);
                if (result->flag == BT_GATT_EVENT_RESULT_DONE_COMPLETED)
                {
                    Report(("discovery char service done."));
                    /* Set up callback token */
                    InitializeListEntry(&op->gattOp.op.node);
                    parm3 = &op->gattParm.readByType;
                    parm3->startHandle = 0x0001;
                    parm3->endHandle = 0xFFFF;
                    parm3->uuid16 = BT_UUID_GATT_CHAR_DESC_CHAR_EXTEND_PROPERTY;
					op->gattOp.parms.p.readByType = parm3;
					BT_GATT_ReadByType(op->token->remDev, &op->gattOp);
                    return;
                }
                break;
            }          
            case BT_GATT_OPERATOR_READ_BY_TYPE:
            {
                /* GAP service handle range discovered, then read value */
                BtGattReadByTypeResultEvent *result = event->parms;
                
                Report(("result->flag:%d", result->flag));
                EVM_CancelTimer(&op->token->tokenTimer);
                if (result->flag == BT_GATT_EVENT_RESULT_DONE_COMPLETED)
                {
                    Report(("discovery char service done."));
                    /* Set up callback token */
                    InitializeListEntry(&op->gattOp.op.node);
                    parm3 = &op->gattParm.readByType;
                    if(parm3->uuid16 == BT_UUID_GATT_CHAR_DESC_CHAR_EXTEND_PROPERTY)
                    {
                         parm3->uuid16 = BT_UUID_GATT_CHAR_DESC_CHAR_USER_DESC;                    
                    }
                    else if(parm3->uuid16 == BT_UUID_GATT_CHAR_DESC_CHAR_USER_DESC)
                    {
                         parm3->uuid16 = BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG;                    
                    }
                    else if(parm3->uuid16 == BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG)
                    {
                         parm3->uuid16 = BT_UUID_GATT_CHAR_DESC_SERVER_CHAR_CONFIG;                    
                    }
                    else if(parm3->uuid16 == BT_UUID_GATT_CHAR_DESC_SERVER_CHAR_CONFIG)
                    {
                         parm3->uuid16 = BT_UUID_GATT_TYPE_SECONDARY_SERVICE;                    
                    }   
                    else if(parm3->uuid16 == BT_UUID_GATT_TYPE_SECONDARY_SERVICE)
                    {
                         parm3->uuid16 = BT_UUID_GATT_TYPE_PRIMARY_SERVICE;                    
                    }  					
                    else
                    {
                        break;
                    }
                    parm3->startHandle = 0x0001;
                    parm3->endHandle = 0xFFFF;
                    op->gattOp.parms.p.readByType = parm3;
                    BT_GATT_ReadByType(op->token->remDev, &op->gattOp);
                    return;
                }
            } 

            default:
                break;;
        }
        return;
    }

error:
    btbmGattForceRemoveLink(op->token->remDev);
    cbEvent.eType = op->token->eType;  
    cbEvent.errCode = BEC_NO_ERROR;
    cbEvent.p.meToken = op->token;
    if(op->token->callback != NULL)
        op->token->callback(&cbEvent);
    btbmGattFreeOp(op);
}

BtStatus BTBMGATT_BQETestCode(BD_ADDR *bdAddr, BtCallBack callback)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;
    BtGattDiscoveryPrimaryServiceByUUID *parm;

    OS_Report("BTBMGATT_ServiceSearchAll");
         
    if (callback == NULL)
    {
        OS_Report("BTBMGATT_ServiceSearchAll: NULL callback");
    }

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_SERVICE_SEARCH_ALL;
    op->token->eType = SDEVENT_QUERY_RSP;
    op->token->p.gattReq.bdAddr = *bdAddr;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->callback = callback;
    op->gattOp.apCb = btbmGattBQECallBack;
    parm = &op->gattParm.discService;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16[0] = 0x0B; /* Low bytes */
    parm->uuid16[1] = 0xA0; /* Low bytes */
    
    op->gattOp.parms.p.primaryServiceByUUID = parm;

    /* Discover GAP service handle range */
    BT_GATT_DiscoveryPrimaryServiceByUUID(op->token->remDev, &op->gattOp);
    status = BT_STATUS_PENDING;
    
    return status;
}



BtStatus BTBMGATT_BQEMakeCommandTestCode(btbmGattOp *op)
{
#if 0
    BtStatus status = BT_STATUS_FAILED;
    BtGattDiscoveryPrimaryServiceByUUID *parm;

    OS_Report("BTBMGATT_ServiceSearchAll");
         
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_SERVICE_SEARCH_ALL;
    op->token->eType = SDEVENT_QUERY_RSP;
    InitializeListEntry(&op->gattOp.op.node);
    op->gattOp.apCb = btbmGattBQECallBack;
    parm = &op->gattParm.discService;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;
    parm->type = 0x02;
    parm->uuid16[0] = 0x0B; /* Low bytes */
    parm->uuid16[1] = 0x0A; /* high bytes */
    
    op->gattOp.parms.p.primaryServiceByUUID = parm;

    /* Discover GAP service handle range */
    BT_GATT_DiscoveryPrimaryServiceByUUID(op->token->remDev, &op->gattOp);
    status = BT_STATUS_PENDING;
    
    return status;
#endif
    BtStatus status = BT_STATUS_FAILED;
    BtGattFindIncludeServices *parm;

    OS_Report("BTBMGATT_ServiceSearchAll");
    	 
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_SERVICE_SEARCH_ALL;
    op->token->eType = SDEVENT_QUERY_RSP;
    InitializeListEntry(&op->gattOp.op.node);
    op->gattOp.apCb = btbmGattBQECallBack;
    parm = &op->gattParm.discService;
    parm->startHandle = 0x0001;
    parm->endHandle = 0xFFFF;

    op->gattOp.parms.p.findIncludeServices = parm;

    /* Discover GAP service handle range */
    BT_GATT_FindIncludeService(op->token->remDev, &op->gattOp);
    status = BT_STATUS_PENDING;

return status;

}


void BtBmGattBQETest(BtRemoteDevice *link)
{
    BtStatus status = BT_STATUS_FAILED;
    btbmGattOp *op;

    op = btbmGattGetFreeOp();
    op->token = btbmGattGetFreeToken();
    /* Set up callback token */
    op->token->op.opType = BOP_GATT_SERVICE_SEARCH_ALL;
    op->token->eType = SDEVENT_QUERY_RSP;
    op->token->p.gattReq.bdAddr = link->bdAddr;
    InitializeListEntry(&op->gattOp.op.node);
    op->token->callback = NULL;
    op->token->remDev = link;
    op->gattOp.apCb = btbmGattBQECallBack;
    //				  BT_GATT_DiscoveryAllPrimaryService(op->token->remDev, &op->gattOp);
    BTBMGATT_BQEMakeCommandTestCode(op);

}

#endif

#endif


