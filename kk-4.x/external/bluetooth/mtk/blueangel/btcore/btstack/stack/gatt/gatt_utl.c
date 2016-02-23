#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "utils.h"
#include "gattcb.h"


#if ATT_PROTOCOL == XA_ENABLED


extern BtStatus BT_ATT_ForGattExchangeMTURequest(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus Bt_GattSetDisconnectTimeOut(BtRemoteDevice *link, U32 value);
extern BtStatus AttDisconnectRequest(BtRemoteDevice *link);
extern BtStatus AttDisconnectRequest(BtRemoteDevice *link);
extern BtStatus AttClientRequest(BtRemoteDevice *link);

extern U16 BT_ATT_GetNegotiationMTU(BtRemoteDevice *link);

BtGattConn *BT_Gatt_GetConCtx(BtRemoteDevice *link);
void GATT_HandleOperation(BtRemoteDevice *link);
void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb)
{
	InitializeListEntry(&hdl->node);
	hdl->callback = cb;
    InsertTailList(&(GATTC(regList)), &(hdl->node));

}

void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl)
{
    if(IsNodeOnList(&(GATTC(regList)), &(hdl->node))){
    RemoveEntryList(&(hdl->node));
    }else{
        OS_Report("BT_GattUnregisterConnectedEvent : hdl(0x%x) is not registered", hdl);
}
}


BtStatus GattClientConnect(BtRemoteDevice *link)
{
    BtStatus status;
    status = AttClientRequest(link);
    return status;
}

BtStatus GattDisconnect(BtRemoteDevice *link)
{
    BtStatus status;
    
    status = AttDisconnectRequest(link);
    return status;
}

void BT_GattReportLinkCallback(U8 event_code, BtRemoteDevice *link, void *parms)
{
    ListEntry *entry;
    BtGattHandler *handler;
	BtGattEvent event;

	event.eType = event_code;
	event.link = link;
    event.parms = parms;
    
    entry = GATTC(regList).Flink;
    while (entry && entry != &GATTC(regList))
    {
		handler = ((BtGattHandler *)entry);
        entry = entry->Flink;
        handler->callback(handler, &event);
    }

}




BtStatus BT_GattCommonAPI(BtRemoteDevice *link, BtOperation *op, U8 opcode)
{
    BtGattConn *conn;

    OS_Report("BT_GattCommonAPI: state=%d, opcode=0x%x", link->state, opcode);
    if(link->state != BDS_CONNECTED)
        return BT_STATUS_FAILED;

    conn = BT_Gatt_GetConCtx(link);

    if(op->opType != BOP_NOP)
    {
        Assert(0);
    }
    if(!IsEntryAvailable(&(op->node)))
    {
        Assert(0);
    }
    if( conn == NULL ){
        OS_Report( "[ATT][ERR] BT_GattCommonAPI conn is NULL!");
        return BT_STATUS_FAILED;
    }
    op->opType = opcode;
    InsertTailList(&conn->gattRequestOpList, &(op->node));
    GATT_HandleOperation(link);
    return BT_STATUS_PENDING;
}

BtStatus BT_GattNotificationAPI(BtRemoteDevice *link, BtOperation *op, U8 opcode)
{
    BtGattConn *conn;

    if(link->state != BDS_CONNECTED)
        return BT_STATUS_FAILED;

    conn = BT_Gatt_GetConCtx(link);

    if(op->opType != BOP_NOP)
    {
        Assert(0);
    }
    if(!IsEntryAvailable(&(op->node)))
    {
        Assert(0);
    }
    op->opType = opcode;
    InsertTailList(&conn->gattNotificationOpList , &(op->node));
    GATT_HandleOperation(link);
    return BT_STATUS_PENDING;
}


BtStatus BT_GattIndicationAPI(BtRemoteDevice *link, BtOperation *op, U8 opcode)
{
    BtGattConn *conn;

    if(link->state != BDS_CONNECTED)
        return BT_STATUS_FAILED;

    conn = BT_Gatt_GetConCtx(link);

    if(op->opType != BOP_NOP)
    {
        Assert(0);
    }
    if(!IsEntryAvailable(&(op->node)))
    {
        Assert(0);
    }
    op->opType = opcode;
    InsertTailList(&conn->gattIndicationOpList, &(op->node));
    GATT_HandleOperation(link);
    return BT_STATUS_PENDING;
}

BtStatus BT_GATT_ExchangeMTU(BtRemoteDevice *link, BtGattOp *op)
{

    BtStatus status;
    BtGattConn *conn;

    if(link->state != BDS_CONNECTED)
        return BT_STATUS_FAILED;

    conn = BT_Gatt_GetConCtx(link);

    if(op->op.opType != BOP_NOP)
    {
    	OS_Report("[ATT] op:0x%x opType:%d", op, op->op.opType);
        Assert(0);
    }
   
    if(!IsEntryAvailable(&(op->op.node)))
    {
        Assert(0);
    }
    op->op.opType = BT_GATT_OPERATOR_EXCHANGE_MTU;
    InsertHeadList(&conn->gattRequestOpList, &(op->op.node));
    GATT_HandleOperation(link);
    return BT_STATUS_PENDING;	
}

BtStatus BT_GATT_DiscoveryAllPrimaryService(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE);
    return status;

}

BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID);
    return status;

}

BtStatus BT_GATT_FindIncludeService(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE);
    return status;

}

BtStatus BT_GATT_DiscoveryAllCharacteristicOfService(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE);
    return status;

}

BtStatus BT_GATT_DiscoveryCharacteristicByUUID(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID);
    return status;

}

BtStatus BT_GATT_DiscoveryAllCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR);
    return status;

}

BtStatus BT_GATT_ReadCharacteristicValue(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_CHAR_VALUE);
    return status;

}

BtStatus BT_GATT_ReadUsingCharacteristicUUID(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_USING_CHAR_UUID);
    return status;

}

BtStatus BT_GATT_ReadLongCharacteristicValues(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE);
    return status;

}

BtStatus BT_GATT_WriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattNotificationAPI(link, &op->op, BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE);
    return status;

}

BtStatus BT_GATT_ReadMultipleCharacteristicValues(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE);
    return status;

}

BtStatus BT_GATT_SignedWriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattNotificationAPI(link, &op->op, BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE);
    return status;

}

BtStatus BT_GATT_WriteCharacteristicValue(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_WRITE_CHAR_VALUE);
    return status;

}

BtStatus BT_GATT_WriteLongCharacteristicValues(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE);
    return status;

}

BtStatus BT_GATT_ReliableWrites(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_RELIABLEWRITES);
    return status;

}

BtStatus BT_GATT_ExecuteWrites(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_EXEC_WRITES);
    return status;

}

BtStatus BT_GATT_Notifications(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattNotificationAPI(link, &op->op, BT_GATT_OPERATOR_NOTIFICATIONS);
    return status;

}

BtStatus BT_GATT_Indications(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattIndicationAPI(link, &op->op, BT_GATT_OPERATOR_INDICATIONS);
    return status;

}

BtStatus BT_GATT_ReadCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS);
    return status;

}

BtStatus BT_GATT_ReadLongCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS);
    return status;

}

BtStatus BT_GATT_WriteCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS);
    return status;

}

BtStatus BT_GATT_WriteLongCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS);
    return status;

}

U16 BT_GATT_GetNegotiationMTU(BtRemoteDevice *link)
{
    U16 mtu;

    mtu = BT_ATT_GetNegotiationMTU(link);
    return mtu;
}


BtStatus BT_GATT_ReadByType(BtRemoteDevice *link, BtGattOp *op)
{
    BtStatus status;

    status = BT_GattCommonAPI(link, &op->op, BT_GATT_OPERATOR_READ_BY_TYPE);
    return status;

}


BtStatus BT_GATT_SetDisconnectTimeout(BtRemoteDevice *link, U32 value)
{
    BtStatus status;
    status = Bt_GattSetDisconnectTimeOut(link, value);
    return status;
}



#endif
