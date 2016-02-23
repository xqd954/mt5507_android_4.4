#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "att_i.h"
#include "att.h"
#include "gattcb.h"

#if ATT_PROTOCOL == XA_ENABLED


void BT_Gatt_Att_Callback(BtAttEvent *attEvent);
void GATT_HandleOperation(BtRemoteDevice *link);
void GATT_HandleOperationLinkDisconnected(BtRemoteDevice *link);



extern void GattHandleExchangeMTUOp(U8 event, BtRemoteDevice *link);
extern void GattHandleDiscoveryAllPrimaryServiceOp(U8 event, BtRemoteDevice *link);
extern void GattHandleDiscoveryPrimaryServiceByUUIDOp(U8 event, BtRemoteDevice *link);
extern void GattHandleFindIncludeServiceOp(U8 event, BtRemoteDevice *link);
extern void GattHandleDiscoveryAllCharOfServiceOp(U8 event, BtRemoteDevice *link);
extern void GattHandleDiscoveryCharByUUIDOp(U8 event, BtRemoteDevice *link);
extern void GattHandleDiscoveryAllCharDescriptorOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadCharValueOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadUsingCharUUIDOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadLongCharValueOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadMultipleCharValueOp(U8 event, BtRemoteDevice *link);
extern void GattHandleWriteCharValueOp(U8 event, BtRemoteDevice *link);
extern void GattHandleWriteLongCharValueOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReliableWriteOp(U8 event, BtRemoteDevice *link);
extern void GattHandleExecWriteOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadCharDescriptorOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadLongCharDescriptorOp(U8 event, BtRemoteDevice *link);
extern void GattHandleWriteCharDescriptorOp(U8 event, BtRemoteDevice *link);
extern void GattHandleWriteLongCharDescriptorOp(U8 event, BtRemoteDevice *link);
extern void GattHandleNotificationOp(U8 event, BtRemoteDevice *link);
extern void GattHandleWriteWithoutResponseOp(U8 event, BtRemoteDevice *link);
extern void GattHandleSignedWriteWithoutResponseOp(U8 event, BtRemoteDevice *link);
extern void GattHandleIndicationsOp(U8 event, BtRemoteDevice *link);
extern void GattHandleWriteLongCharDescriptorOp(U8 event, BtRemoteDevice *link);
extern void GattHandleReadByTypeOp(U8 event, BtRemoteDevice *link);


extern void btbmHandlerGattReadyToAskDeviceName(BtRemoteDevice *link);
extern void BT_GattReportLinkCallback(U8 event_code, BtRemoteDevice *link, void *parms);
extern void BT_ATT_GattRegisterHandler(GattCallBack callback);
extern BtGattConn *BT_Gatt_GetConCtx(BtRemoteDevice *link);




void GATT_Init()
{
    U8 i,j=0;

    Report(("GATT init"));
    InitializeListHead(&(GATTC(regList)));
    for(i=0; i< MAX_GATT_CON_NO; i++)
    {
        GATTC(conn)[i].state = GATT_CONN_STATE_IDLE;
        GATTC(conn)[i].link = 0;
        InitializeListHead(&GATTC(conn)[i].gattRequestOpList);
        InitializeListHead(&GATTC(conn)[i].gattNotificationOpList);
        InitializeListHead(&GATTC(conn)[i].gattIndicationOpList);
        GATTC(conn)[i].disconnectTimerValue = MAX_GATT_DISCONNECT_TIMEOUT_VALUE;
        EVM_ResetTimer(&GATTC(conn)[i].disconnectTimer);
    }
    BT_ATT_GattRegisterHandler(BT_Gatt_Att_Callback);
}

BtGattConn *BT_Gatt_GetConCtx(BtRemoteDevice *link)
{
    U8 i=0;
    
    for(i=0; i< MAX_GATT_CON_NO; i++)
    {
        if ((GATTC(conn)[i].state == GATT_CONN_STATE_CONNECTED) &&
            (GATTC(conn)[i].link == link))
        {
            return &GATTC(conn)[i];
        }
    }
    return NULL;

}

BtStatus Bt_GattSetDisconnectTimeOut(BtRemoteDevice *link, U32 value)
{
    BtGattConn *conn;

    conn = BT_Gatt_GetConCtx(link);
    if(conn != NULL)
    {
        conn->disconnectTimerValue = value;
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAILED;
}

void BT_GATT_DisconnectTimeoutHandler(EvmTimer* timer)
{
    Report(("BT_GATT_DisconnectTimeoutHandler marked by yi. "));
#if 0
    BtGattConn *conn;
    BtDeviceContext *bdc;

	conn = (BtGattConn *)timer->context;
    
    bdc = DS_FindDevice((const BD_ADDR *)&conn->link->bdAddr);
    if (bdc)
    {
        if(bdc->devType != BT_DEV_TYPE_LE) 
        {
            Report(("Not BLE type, return directly"));
            return;
        }
    }
    Report(("BT_GATT_DisconnectTimeoutHandler"));
    MeDisconnectLink(conn->link,25);
#endif
}


void BT_Gatt_ResetDisconnectTimer(BtGattConn *conn)
{
#if 0
    BtDeviceContext *bdc;
    
    Report(("BT_Gatt_ResetDisconnectTimer value:%d", conn->disconnectTimerValue));
    bdc = DS_FindDevice((const BD_ADDR *)&conn->link->bdAddr);
    if (bdc)
    {
        if(bdc->devType != BT_DEV_TYPE_LE) 
        {
            Report(("Not BLE type, return directly"));
            return;
        }
    }
    conn->disconnectTimer.func = BT_GATT_DisconnectTimeoutHandler;
    conn->disconnectTimer.context = conn;
    EVM_StartTimer(&conn->disconnectTimer, conn->disconnectTimerValue);
#endif
}

void BT_Gatt_HandleConnected(BtRemoteDevice *link)
{
    U8 i=0;
    
    for(i=0; i< MAX_GATT_CON_NO; i++)
    {
        if ((GATTC(conn)[i].state == GATT_CONN_STATE_CONNECTED) &&
            (GATTC(conn)[i].link == link))
        {
            return;
        }
    }
    for(i=0; i< MAX_GATT_CON_NO; i++)
    {
        if (GATTC(conn)[i].state == GATT_CONN_STATE_IDLE)
        {
            OS_MemSet( (U8 *)&GATTC(conn)[i], 0, sizeof(GATTC(conn)[i]) );
            GATTC(conn)[i].state = GATT_CONN_STATE_CONNECTED;
            GATTC(conn)[i].link = link;
            InitializeListEntry(&GATTC(conn)[i].mtuOp.op.node);
            InitializeListHead(&GATTC(conn)[i].gattRequestOpList);
            InitializeListHead(&GATTC(conn)[i].gattNotificationOpList);
            InitializeListHead(&GATTC(conn)[i].gattIndicationOpList);
            if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
                GATTC(conn)[i].att_mtu = MAX_ATT_RX_MTU_BLE;
            else
                GATTC(conn)[i].att_mtu = MAX_ATT_RX_MTU_BREDR;            
            GATTC(conn)[i].mtuOp.op.opType = BOP_NOP;
            GATTC(conn)[i].disconnectTimerValue = MAX_GATT_DISCONNECT_TIMEOUT_VALUE;
            BT_Gatt_ResetDisconnectTimer(&GATTC(conn)[i]);
            return;
        }
    }

}

void BT_Gatt_HandleDisconnected(BtRemoteDevice *link)
{
    BtGattConn *conn;
    conn = BT_Gatt_GetConCtx(link);
    GATT_HandleOperationLinkDisconnected(link);
    conn->state = GATT_CONN_STATE_IDLE;
    conn->link = 0;
	InitializeListHead(&conn->gattRequestOpList);
	InitializeListHead(&conn->gattNotificationOpList);
	InitializeListHead(&conn->gattIndicationOpList);
    EVM_ResetTimer(&conn->disconnectTimer);

}


void BT_Gatt_Att_Callback(BtAttEvent *attEvent)
{
    BtRemoteDevice *link;
    BtGattConn *conn;

    link = attEvent->link;
	if(attEvent->eType == BT_ATT_EVENT_CONNECTED)
	{
		BT_Gatt_HandleConnected(link);
	}
    conn = BT_Gatt_GetConCtx(link);
    if(conn == 0)
    {
        return;
    }
    conn->attEvent = attEvent;
    BT_Gatt_ResetDisconnectTimer(conn);
    OS_Report("BT_Gatt_Att_Callback(): event=%d", attEvent->eType);
    switch (attEvent->eType)
    {
	    case BT_ATT_EVENT_CONNECTED:
//			BT_GATT_ExchangeMTU(link, &conn->mtuOp);
            /* Not to request the MTU for IOT reason. */
            btbmHandlerGattReadyToAskDeviceName(link);
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_CONNECTED, link, NULL);

			break;
		case BT_ATT_EVENT_DISCONNECTED:
			BT_Gatt_HandleDisconnected(link);
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_DISCONNECTED, link, NULL);
			break;
		case BT_ATT_EVENT_CONNECTED_SECURITY_CHANGE:
			GATT_HandleOperation(link);
			break;
        case BT_ATT_EVENT_ERROR_RESPONSE:
            if(conn->gattRequestHandler != 0)
                conn->gattRequestHandler(BT_GATT_OP_ERROR, link);
            break;
        case BT_ATT_EVENT_NOTIFIED_PACKET_HANDLED:
            if(conn->gattNotificationHandler != 0)
                conn->gattNotificationHandler(BT_GATT_OP_END, link);
            break;
        case BT_ATT_EVENT_FIND_BY_TYPE_VALUE_RESPONSE:
        case BT_ATT_EVENT_FIND_INFO_RESPONSE:
        case BT_ATT_EVENT_EXCHANGE_MTU_RESPONSE:
        case BT_ATT_EVENT_READ_BY_TYPE_RESPONSE:
        case BT_ATT_EVENT_READ_RESPONSE:
        case BT_ATT_EVENT_READ_BLOB_RESPONSE:
        case BT_ATT_EVENT_READ_MULTIPLE_RESPONSE:
        case BT_ATT_EVENT_READ_BY_GROUP_TYPE_RESPONSE:
        case BT_ATT_EVENT_WRITE_RESPONSE:
        case BT_ATT_EVENT_PREPARE_WRITE_RESPONSE:
        case BT_ATT_EVENT_EXECUTE_WRITE_RESPONSE:
            if(conn->gattRequestHandler != 0)            
                conn->gattRequestHandler(BT_GATT_OP_END, link);
            break;
        case BT_ATT_EVENT_VALUE_CONFIRMATION:
            if(conn->gattIndicationHandler!= 0)
                conn->gattIndicationHandler(BT_GATT_OP_END, link);
            break;
        case BT_ATT_EVENT_VALUE_NOTIFICATION:
            /* Shall report to upper layer */
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_VALUE_NOTIFICATION, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_VALUE_INDICATION:
            /* Shall report to upper layer */
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_VALUE_INDICATION, link, attEvent->parms);
            break;

        case BT_ATT_EVENT_EXCHANGE_MTU_REQUEST:
            break;
        case BT_ATT_EVENT_FIND_INFO_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_FIND_BY_TYPE_VALUE_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_READ_BY_TYPE_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_READ_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_READ_BLOB_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_READ_MULTIPLE_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_READ_BY_GROUP_TYPE_REQUEST:
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_WRITE_REQUEST:
            /* Shall report to upper layer */
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_WRITE_REQUEST, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_WRITE_COMMAND:
            /* Shall report to upper layer */
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_WRITE_COMMAND, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_PREPARE_WRITE_REQUEST:
            /* Shall report to upper layer */
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_PREPARE_WRITE_REQUEST, link, attEvent->parms);
            break;
        case BT_ATT_EVENT_EXECUTE_WRITE_REQUEST:
            /* Shall report to upper layer */
            BT_GattReportLinkCallback(BT_GATT_APP_EVENT_SERVER, link, attEvent->parms);
			BT_GattReportLinkCallback(BT_GATT_APP_EVENT_EXECUTE_WRITE_REQUEST, link, attEvent->parms);
            break;
    }
}




void GATT_HandleOperation(BtRemoteDevice *link)
{
    BtGattConn *conn;

#if 0
    // QDAY: remove this for testing
	if(link->authState != BAS_AUTHENTICATED)
	{
		Report(("ATT link wait for authentication"));
		AttStartAuthentication(link);
		return;
	}
#endif    
    
    conn = BT_Gatt_GetConCtx(link);
    BT_Gatt_ResetDisconnectTimer(conn);

    OS_Report("GATT_HandleOperation(): gattRequestCurOp=0x%x", conn->gattRequestCurOp);
    if (conn->gattRequestCurOp)
        OS_Report("GATT_HandleOperation(): old 0x%x", conn->gattRequestCurOp->opType);
    if ((conn->gattRequestCurOp == 0) && (!IsListEmpty(&conn->gattRequestOpList)))
    {
        conn->gattRequestCurOp = (BtOperation*) RemoveHeadList(&conn->gattRequestOpList);
        InitializeListEntry(&(conn->gattRequestCurOp->node));
    
        OS_Report("GATT_HandleOperation(): 0x%x", conn->gattRequestCurOp->opType);
        switch (conn->gattRequestCurOp->opType)
        {
            case BT_GATT_OPERATOR_EXCHANGE_MTU:
                conn->gattRequestHandler = GattHandleExchangeMTUOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE:
                conn->gattRequestHandler = GattHandleDiscoveryAllPrimaryServiceOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
                conn->gattRequestHandler = GattHandleDiscoveryPrimaryServiceByUUIDOp;
                break;
            case BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE:
                conn->gattRequestHandler = GattHandleFindIncludeServiceOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE:
                conn->gattRequestHandler = GattHandleDiscoveryAllCharOfServiceOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
                conn->gattRequestHandler = GattHandleDiscoveryCharByUUIDOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR:
                conn->gattRequestHandler = GattHandleDiscoveryAllCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_READ_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleReadCharValueOp;
                break;
			case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
			    conn->gattRequestHandler = GattHandleReadUsingCharUUIDOp;
				break;
            case BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleReadLongCharValueOp;
                break;
            case BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleReadMultipleCharValueOp;
                break;
            case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleWriteCharValueOp;
                break;
            case BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleWriteLongCharValueOp;
                break;
            case BT_GATT_OPERATOR_RELIABLEWRITES:
                conn->gattRequestHandler = GattHandleReliableWriteOp;
                break;
            case BT_GATT_OPERATOR_EXEC_WRITES:
                conn->gattRequestHandler = GattHandleExecWriteOp;
                break;
            case BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleReadCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleReadLongCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleWriteCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleWriteLongCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_READ_BY_TYPE:
                conn->gattRequestHandler = GattHandleReadByTypeOp;
                break;
                
        }
        conn->gattRequestHandler(BT_GATT_OP_START, link);
    }
    if ((conn->gattNotificationCurOp == 0) && (!IsListEmpty(&conn->gattNotificationOpList)))
    {
        conn->gattNotificationCurOp = (BtOperation*) RemoveHeadList(&conn->gattNotificationOpList);
        InitializeListEntry(&(conn->gattNotificationCurOp->node));
    
        switch (conn->gattNotificationCurOp->opType)
        {
            case BT_GATT_OPERATOR_NOTIFICATIONS:
                conn->gattNotificationHandler = GattHandleNotificationOp;
                break;
            case BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE:
                conn->gattNotificationHandler = GattHandleWriteWithoutResponseOp;
                break;
            case BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE:
                conn->gattNotificationHandler = GattHandleSignedWriteWithoutResponseOp;
                break;                
                
        }
        conn->gattNotificationHandler(BT_GATT_OP_START, link);

    }
    if ((conn->gattIndicationCurOp == 0) && (!IsListEmpty(&conn->gattIndicationOpList)))
    {
        conn->gattIndicationCurOp = (BtOperation*) RemoveHeadList(&conn->gattIndicationOpList);
        InitializeListEntry(&(conn->gattIndicationCurOp->node));
    
        switch (conn->gattIndicationCurOp->opType)
        {
            case BT_GATT_OPERATOR_INDICATIONS:
                conn->gattIndicationHandler = GattHandleIndicationsOp;
                break;
        }
        conn->gattIndicationHandler(BT_GATT_OP_START, link);
    }
    
}


void GATT_HandleOperationLinkDisconnected(BtRemoteDevice *link)
{
    BtGattConn *conn;

    conn = BT_Gatt_GetConCtx(link);
    if(conn->gattRequestCurOp !=0)
    {
        conn->gattRequestHandler(3, link);
        conn->gattRequestHandler = 0;
        conn->gattRequestCurOp = 0;
    }
    while(!IsListEmpty(&conn->gattRequestOpList))
    {
        conn->gattRequestCurOp = (BtOperation*) RemoveHeadList(&conn->gattRequestOpList);
        InitializeListEntry(&(conn->gattRequestCurOp->node));
    
        switch (conn->gattRequestCurOp->opType)
        {
            case BT_GATT_OPERATOR_EXCHANGE_MTU:
                conn->gattRequestHandler = GattHandleExchangeMTUOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE:
                conn->gattRequestHandler = GattHandleDiscoveryAllPrimaryServiceOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID:
                conn->gattRequestHandler = GattHandleDiscoveryPrimaryServiceByUUIDOp;
                break;
            case BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE:
                conn->gattRequestHandler = GattHandleFindIncludeServiceOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE:
                conn->gattRequestHandler = GattHandleDiscoveryAllCharOfServiceOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID:
                conn->gattRequestHandler = GattHandleDiscoveryCharByUUIDOp;
                break;
            case BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR:
                conn->gattRequestHandler = GattHandleDiscoveryAllCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_READ_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleReadCharValueOp;
                break;
			case BT_GATT_OPERATOR_READ_USING_CHAR_UUID:
			    conn->gattRequestHandler = GattHandleReadUsingCharUUIDOp;
				break;				
            case BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleReadLongCharValueOp;
                break;
            case BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleReadMultipleCharValueOp;
                break;
            case BT_GATT_OPERATOR_WRITE_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleWriteCharValueOp;
                break;
            case BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE:
                conn->gattRequestHandler = GattHandleWriteLongCharValueOp;
                break;
            case BT_GATT_OPERATOR_RELIABLEWRITES:
                conn->gattRequestHandler = GattHandleReliableWriteOp;
                break;
            case BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleReadCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleReadLongCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleWriteCharDescriptorOp;
                break;
            case BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS:
                conn->gattRequestHandler = GattHandleWriteLongCharDescriptorOp;
                break;
                
        }
        conn->gattRequestHandler(BT_GATT_OP_LINK_DISCONNECTED, link);
    }
    if(conn->gattNotificationCurOp !=0)
    {
        conn->gattNotificationHandler(3, link);
        conn->gattNotificationHandler = 0;
        conn->gattNotificationCurOp = 0;
    }

    while(!IsListEmpty(&conn->gattNotificationOpList))
    {
        conn->gattNotificationCurOp = (BtOperation*) RemoveHeadList(&conn->gattNotificationOpList);
        InitializeListEntry(&(conn->gattNotificationCurOp->node));
    
        switch (conn->gattNotificationCurOp->opType)
        {
            case BT_GATT_OPERATOR_NOTIFICATIONS:
                conn->gattNotificationHandler = GattHandleNotificationOp;
                break;
            case BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE:
                conn->gattNotificationHandler = GattHandleWriteWithoutResponseOp;
                break;
            case BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE:
                conn->gattNotificationHandler = GattHandleSignedWriteWithoutResponseOp;
                break;                
                
        }
        conn->gattNotificationHandler(BT_GATT_OP_LINK_DISCONNECTED, link);
    }
    if(conn->gattIndicationCurOp !=0)
    {
        conn->gattIndicationHandler(3, link);
        conn->gattIndicationHandler = 0;
        conn->gattIndicationCurOp = 0;
    }

    while(!IsListEmpty(&conn->gattIndicationOpList))
    {
        conn->gattIndicationCurOp = (BtOperation*) RemoveHeadList(&conn->gattIndicationOpList);
        InitializeListEntry(&(conn->gattIndicationCurOp->node));
    
        switch (conn->gattIndicationCurOp->opType)
        {
            case BT_GATT_OPERATOR_INDICATIONS:
                conn->gattIndicationHandler = GattHandleIndicationsOp;
                break;
        }
        conn->gattIndicationHandler(BT_GATT_OP_LINK_DISCONNECTED, link);
    }
    
}

#endif
