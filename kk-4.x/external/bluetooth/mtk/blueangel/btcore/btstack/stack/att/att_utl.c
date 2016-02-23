#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "att.h"
#include "att_i.h"
#include "utils.h"

#if ATT_PROTOCOL == XA_ENABLED
/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/
BtStatus BT_ATT_Init(void);
void BT_ATT_Deinit(void);
BtStatus BT_ATT_SendExchangeMTURequest(BtATTExchangeMTURequestStruct *ptr, BtRemoteDevice *link);

/****************************************************************************
 *
 * external Functions
 *
 ****************************************************************************/
extern void BT_ATT_L2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info);
extern void BtAttBREDRL2CallBack(L2capChannelId L2ChannelId, L2capCallbackParms *info);
extern BtStatus L2Cap_FixCidSendSigned(U8 cid, BtRemoteDevice *link, BtPacket *Packet, U16 PacketBufLen);
extern BtStatus L2Cap_FixCidSend(U8 cid, BtRemoteDevice *link, BtPacket *Packet);
#ifndef __BT_LE_STANDALONE__
extern  U8 AttDeviceSdpDB_DeRegistration(void);
extern U8 AttDeviceSdpDB_Registration(void);
#endif
extern void AttStateInit(void);



BtStatus BT_ATT_Init(void)
{
    U8 i,j=0;

    ATTC(callback) = 0;
    for(i=0; i< MAX_ATT_CON_NO; i++)
    {
        ATTC(conn)[i].state = ATT_CONN_DISCONNECTED;
        ATTC(conn)[i].link = 0;
        ATTC(conn)[i].bufferPkt = NULL;      
        InitializeListHead(&ATTC(conn)[i].freePacketList);
        for(j=0;j<MAX_ATT_TX_PACKET_NO;j++)
        {
            ATTC(conn)[i].pkt[j].data = (U8 *)ATTC(conn)[i].txBuffer[j];
            InsertTailList(&ATTC(conn)[i].freePacketList,&(ATTC(conn)[i].pkt[j].node));
        }
    }
    L2CAP_RegisterFixCid(BT_ATT_L2Callback, 0x0004);
    AttStateInit();
    ATTC(psm).callback = BtAttBREDRL2CallBack;
    ATTC(psm).psm = BT_PSM_ATT;
    ATTC(psm).localMtu = 48;
    ATTC(psm).minRemoteMtu = 48;
    ATTC(psm).authorizedSetting = FALSE;    
#ifdef __BT_3_0_HS__    
    ATTC(psm).op_state = L2OPERATORSTATE_STATE0;
    
    ATTC(psm).inLinkMode = ATTC(psm).outLinkMode = L2MODE_BASIC;
    ATTC(psm).lockStepNeeded = 0;
    ATTC(psm).lockStepBRNeeded = 0;
    
    ATTC(psm).highSpeedRequired = 0;
#endif
    if (L2CAP_RegisterPsm(&(ATTC(psm))) != BT_STATUS_SUCCESS) {
        /* Unable to register */
        bt_trace(BT_TRACE_G1_PROTOCOL, BT_L2CAPTS_UNABLEREG);
    }
    else
    {
#ifndef __BT_LE_STANDALONE__
        if (AttDeviceSdpDB_Registration() != BT_STATUS_SUCCESS) 
        {
            Report(("[ATT] SDP register status != BT_STATUS_SUCCESS"));
        }
#endif
    }
    return BT_STATUS_SUCCESS;
}


/*****************************************************************************
 * FUNCTION
 *  L2CAP_Deinit
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void BT_ATT_Deinit(void)
{
#ifndef __BT_LE_STANDALONE__
    AttDeviceSdpDB_DeRegistration();
#endif
    return;
}


void DummyGattCallback(BtAttEvent *Event)
{
    
    switch (Event->eType)
    {
        case BT_ATT_EVENT_ERROR_RESPONSE:
            break;
        case BT_ATT_EVENT_EXCHANGE_MTU_REQUEST:
            break;
        case BT_ATT_EVENT_EXCHANGE_MTU_RESPONSE:
            break;
        case BT_ATT_EVENT_FIND_INFO_REQUEST:
            break;
        case BT_ATT_EVENT_FIND_INFO_RESPONSE:
            break;
        case BT_ATT_EVENT_FIND_BY_TYPE_VALUE_REQUEST:
            break;
        case BT_ATT_EVENT_FIND_BY_TYPE_VALUE_RESPONSE:
            break;
        case BT_ATT_EVENT_READ_BY_TYPE_REQUEST:
            break;
        case BT_ATT_EVENT_READ_BY_TYPE_RESPONSE:
            break;
        case BT_ATT_EVENT_READ_REQUEST:
            break;
        case BT_ATT_EVENT_READ_RESPONSE:
            break;
        case BT_ATT_EVENT_READ_BLOB_REQUEST:
            break;
        case BT_ATT_EVENT_READ_BLOB_RESPONSE:
            break;
        case BT_ATT_EVENT_READ_MULTIPLE_REQUEST:
            break;
        case BT_ATT_EVENT_READ_MULTIPLE_RESPONSE:
            break;
        case BT_ATT_EVENT_READ_BY_GROUP_TYPE_REQUEST:
            break;
        case BT_ATT_EVENT_READ_BY_GROUP_TYPE_RESPONSE:
            break;
        case BT_ATT_EVENT_WRITE_REQUEST:
            break;
        case BT_ATT_EVENT_WRITE_RESPONSE:
            break;
        case BT_ATT_EVENT_WRITE_COMMAND:
            break;
        case BT_ATT_EVENT_PREPARE_WRITE_REQUEST:
            break;
        case BT_ATT_EVENT_PREPARE_WRITE_RESPONSE:
            break;
        case BT_ATT_EVENT_EXECUTE_WRITE_REQUEST:
            break;
        case BT_ATT_EVENT_EXECUTE_WRITE_RESPONSE:
            break;
        case BT_ATT_EVENT_VALUE_NOTIFICATION:
            break;
        case BT_ATT_EVENT_VALUE_INDICATION:
            break;
        case BT_ATT_EVENT_VALUE_CONFIRMATION:
            break;
    }
}

void BT_ATT_GattRegisterHandler(GattCallBack callback)
{
    ATTC(callback) = callback;
}

BtStatus BT_ATT_GetConTransId(BtRemoteDevice *link, U16 *trans_id){
    BtAttConn *conn = BT_ATT_GetConCtx(link);
    if(conn){
        *trans_id = conn->trans_id;
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_NOT_FOUND;
}

static bt_att_op opcodeToATTOp(U8 opcode){
    bt_att_op op = ATT_OP_UNKNOWN;
    switch(opcode){
    case BT_ATT_OPCODE_READ_REQUEST:
        op = ATT_OP_READ;
        break;
    case BT_ATT_OPCODE_READ_BLOB_REQUEST:
        op = ATT_OP_READ_LONG;
        break;
    case BT_ATT_OPCODE_READ_MULTIPLE_REQUEST:
        op = ATT_OP_READ_MULTIPLE;
        break;    
    case BT_ATT_OPCODE_READ_BY_TYPE_REQUEST:
        op = ATT_OP_READ_BY_TYPE;
        break;
    case BT_ATT_OPCODE_WRITE_REQUEST:
        op = ATT_OP_WRITE;
        break;
    case BT_ATT_OPCODE_WRITE_COMMAND:
        op = ATT_OP_WRITE_WO_RESPONSE;
        break;
    case BT_ATT_OPCODE_PREPARE_WRITE_REQUEST:
        op = ATT_OP_PREPARE_WRITE;
        break;
    case BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST:
        op = ATT_OP_EXECUTE_WRITE;
        break;
    case BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION:
        op = ATT_OP_NOTIFY;
        break;
    case BT_ATT_OPCODE_HANDLE_VALUE_INDICATION:
        op = ATT_OP_INDICATE;
        break;
    case BT_ATT_OPCODE_SIGNED_WRITE_COMMAND:
        op = ATT_OP_INDICATE;
        break;
    default:
        op = ATT_OP_UNKNOWN;
        break;
    }
    return op;
}

bt_att_op BT_ATT_GetConCurrOpcode(BtRemoteDevice *link){
    BtAttConn *conn = BT_ATT_GetConCtx(link);
    if(conn && conn->rxTempLength){
        return opcodeToATTOp(conn->rxTempBuffer[0]);
    }
    return ATT_OP_UNKNOWN;
}

const void *BT_ATT_GetConCurrReq(BtRemoteDevice *link){
    BtAttConn *conn = BT_ATT_GetConCtx(link);
    if(conn && conn->rx_transaction_ongoing){
        return &conn->req_status;
    }
    return NULL;
}


U32 BT_ATT_GetConId(BtRemoteDevice *link)
{
    U8 i=0;
    
    for(i=0; i< MAX_ATT_CON_NO; i++)
    {
        if ((ATTC(conn)[i].state == ATT_CONN_CONNECTED) &&
            (ATTC(conn)[i].link == link))
        {
            return i;
        }
    }
    return MAX_ATT_CON_NO;

}

BtAttConn *BT_ATT_GetConCtx(BtRemoteDevice *link)
{
    U8 i=0;
    
    for(i=0; i< MAX_ATT_CON_NO; i++)
    {
        if ((ATTC(conn)[i].state == ATT_CONN_CONNECTED) &&
            (ATTC(conn)[i].link == link))
        {
            return &ATTC(conn)[i];
        }
    }
    return NULL;

}

BtPacket *BT_ATT_GetBuffer(BtRemoteDevice *link)
{
    U8 i=0;
    BtPacket *pkt;
    
    if(link->state == BDS_CONNECTED)
    {
        i = BT_ATT_GetConId(link);
        if (i >= MAX_ATT_CON_NO)
        {
            return NULL;
        }        
        if (IsListEmpty(&(ATTC(conn)[i].freePacketList)) ==0)
        {
            pkt = (BtPacket *)RemoveHeadList(&(ATTC(conn)[i].freePacketList));
            pkt->headerLen = 0;
            pkt->dataLen = 0;
            pkt->flags = 0;
            pkt->priority_type = 0;
            pkt->llpContext = 0;
            return pkt;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}



void BT_ATT_TimerHandler(EvmTimer* timer)
{
	BtAttConn *conn;

	conn = (BtAttConn *)timer->context;
    Report(("BT ATT con timeout"));
    if((conn->state == ATT_CONN_CONNECTED) &&
        (conn->link !=0) &&
        (conn->link->state == BDS_CONNECTED))
    {
        conn->link->disconnectACLCheckCreatACLAlwaysForce = 0;
        MeDisconnectLink(conn->link,19);
    }
}

void BT_ATT_StartTimer(BtAttConn *conn, EvmTimer *timer)
{
	EVM_ResetTimer(timer);
	timer->func = BT_ATT_TimerHandler;
	timer->context = conn;
	EVM_StartTimer(timer, MAX_ATT_TX_TIMEOUT);
}


U8 BT_ATT_FindAndAssignEmptyConn(BtRemoteDevice *link)
{
    U8 i=0;
    for(i=0; i< MAX_ATT_CON_NO; i++)
    {
        if ((ATTC(conn)[i].state == ATT_CONN_CONNECTED) &&
            (ATTC(conn)[i].link == link))
        {
            return i;
        }
    }
    for(i=0; i< MAX_ATT_CON_NO; i++)
    {
        if (ATTC(conn)[i].state == ATT_CONN_DISCONNECTED)
        {
            ATTC(conn)[i].state = ATT_CONN_CONNECTED;
            ATTC(conn)[i].link = link;
            if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
                ATTC(conn)[i].mtu = MAX_ATT_RX_MTU_BLE;
            else
                ATTC(conn)[i].mtu = MAX_ATT_RX_MTU_BREDR;
            ATTC(conn)[i].local_desired_mtu = ATTC(conn)[i].mtu;
            EVM_ResetTimer(&ATTC(conn)[i].txWaitTimer);
            EVM_ResetTimer(&ATTC(conn)[i].rxWaitTimer);            
            ATTC(conn)[i].tx_transaction_ongoing = 0;
            ATTC(conn)[i].rx_transaction_ongoing = 0;            
            return i;
        }
    }
    Assert(0);
    return MAX_ATT_CON_NO;
}


void BT_ATT_FreeConn(BtRemoteDevice *link)
{
    U8 i=0;
    for(i=0; i< MAX_ATT_CON_NO; i++)
    {
        if ((ATTC(conn)[i].state == ATT_CONN_CONNECTED) &&
            (ATTC(conn)[i].link == link))
        {
            ATTC(conn)[i].state = ATT_CONN_DISCONNECTED;
            ATTC(conn)[i].link = 0;
            if (ATTC(conn)[i].bufferPkt !=0)
                InsertTailList(&ATTC(conn)[i].freePacketList,&(ATTC(conn)[i].bufferPkt->node));
            ATTC(conn)[i].bufferPkt = 0;
            EVM_ResetTimer(&ATTC(conn)[i].txWaitTimer);
            EVM_ResetTimer(&ATTC(conn)[i].rxWaitTimer);   
            SEC_Unregister(&ATTC(conn)[i].secRecord);               
            return;
        }
    }
}

BtStatus BT_ATT_Send(BtRemoteDevice *link, BtPacket *pkt)
{
    U8 status;
    BtAttConn *conn;
    pkt->priority_type = BTP_FLAG_HIGH_PRIPORTY;

    conn = BT_ATT_GetConCtx(link);
    
#ifdef __BT_LE_STANDALONE__
    status = L2Cap_FixCidSend(0x0004, link, pkt);
#else
    if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
    {
        status = L2Cap_FixCidSend(0x0004, link, pkt);

    }
    else
        status = L2CAP_SendData(conn->l2cap_id, pkt);        
#endif
    return status;
}


BtStatus BT_ATT_SendErrorResponse(BtATTErrorResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    OS_Report("BT_ATT_SendErrorResponse(): errCode=0x%x", ptr->errorCode);
    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendErrorResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendErrorResponse(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_ERROR_RESPONSE;
    pkt->data[1] = ptr->opcode;
    StoreLE16(pkt->data+2, ptr->attributeHandle);
    pkt->data[4] = ptr->errorCode;
    pkt->dataLen = 5;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}

BtStatus BT_ATT_ForGattExchangeMTURequest(BtRemoteDevice *link)
{
    U8 i=0;
    BtATTExchangeMTURequestStruct ptr;
        
    ptr.clientRxMtu = ATTC(conn)[i].local_desired_mtu;
    BT_ATT_SendExchangeMTURequest(&ptr, link);
    return BT_STATUS_SUCCESS;
}

BtStatus BT_ATT_SendExchangeMTURequest(BtATTExchangeMTURequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendExchangeMTURequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }      
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendExchangeMTURequest(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST;
    StoreLE16(pkt->data+1, ptr->clientRxMtu);
    pkt->dataLen = 3;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendExchangeMTUResponse(BtATTExchangeMTUResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendExchangeMTUResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }      
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendExchangeMTUResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_EXCHANGE_MTU_RESPONSE;
    StoreLE16(pkt->data+1, ptr->serverRxMtu);
    pkt->dataLen = 3;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}

BtStatus BT_ATT_SendFindInfoRequest(BtATTFindInfoRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendFindInfoRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendFindInfoRequest(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_FIND_INFO_REQUEST;
    StoreLE16(pkt->data+1, ptr->startingHandle);
    StoreLE16(pkt->data+3, ptr->endingHandle);
    pkt->dataLen = 5;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendFindInfoResponse(BtATTFindInfoResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendFindInfoResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendFindInfoResponse(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_FIND_INFO_RESPONSE;
    pkt->data[1] = ptr->format;
    memcpy(pkt->data+2, ptr->infoData, ptr->infoDataLen);
    pkt->dataLen = 2 + ptr->infoDataLen;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);

    return status;
}

BtStatus BT_ATT_SendFindByTypeValueRequest(BtATTFindByTypeValueRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendFindByTypeValueRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }    
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendFindByTypeValueRequest(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
    StoreLE16(pkt->data+1, ptr->startingHandle);
    StoreLE16(pkt->data+3, ptr->endingHandle);
    StoreLE16(pkt->data+5, ptr->attributeType);    
    memcpy(pkt->data+7, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 7 + ptr->attributeValueLen;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendFindByTypeValueResponse(BtATTFindByTypeValueResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendFindByTypeValueResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }      
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendFindByTypeValueResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE;
    memcpy(pkt->data+1, ptr->handleInfoList, ptr->handleInfoListLen);
    pkt->dataLen = 1 + ptr->handleInfoListLen;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}


BtStatus BT_ATT_SendReadByTypeRequest(BtATTReadByTypeRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadByTypeRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadByTypeRequest(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
    StoreLE16(pkt->data+1, ptr->startingHandle);
    StoreLE16(pkt->data+3, ptr->endingHandle);
    if(ptr->type == 2)
    {
        StoreLE16(pkt->data+5, ptr->attributeType2);
        pkt->dataLen = 7;
    }
    else
    {
        memcpy(pkt->data+5, ptr->attributeType, 16);
        pkt->dataLen = 21;
    }
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}



BtStatus BT_ATT_SendReadByTypeResponse(BtATTReadByTypeResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadByTypeResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadByTypeResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_BY_TYPE_RESPONSE;
    pkt->data[1] = ptr->length;
    memcpy(pkt->data+2, ptr->handleDataList, ptr->handleDataListLen);
    pkt->dataLen = 2 + ptr->handleDataListLen;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}


BtStatus BT_ATT_SendReadRequest(BtATTReadRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }      
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadRequest(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_REQUEST;
    StoreLE16(pkt->data+1, ptr->handle);
    pkt->dataLen = 3;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendReadResponse(BtATTReadResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }      
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadResponse(): can not get buffer");      
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_RESPONSE;
    memcpy(pkt->data+1, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 1+ptr->attributeValueLen ;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}

BtStatus BT_ATT_SendReadBlobRequest(BtATTReadBlobRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadBlobRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }       
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadBlobRequest(): can not get buffer");       
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_BLOB_REQUEST;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    StoreLE16(pkt->data+3, ptr->valueOffset);
    pkt->dataLen = 5;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendReadBlobResponse(BtATTReadBlobResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadBlobResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }       
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadBlobResponse(): can not get buffer");      
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_BLOB_RESPONSE;
    if(ptr->partAttributeValueLen !=0)
        memcpy(pkt->data+1, ptr->partAttributeValue, ptr->partAttributeValueLen);
    pkt->dataLen = 1+ptr->partAttributeValueLen ;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}


BtStatus BT_ATT_SendReadMultipleRequest(BtATTReadMultipleRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadMultipleRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }       
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadMultipleRequest(): can not get buffer");      
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_MULTIPLE_REQUEST;
    memcpy(pkt->data+1, ptr->setOfHandles, ptr->setOfHandlesLen);
    pkt->dataLen = 1+ptr->setOfHandlesLen ;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendReadMultipleResponse(BtATTReadMultipleResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadMultipleResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadMultipleResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_MULTIPLE_RESPONSE;
    memcpy(pkt->data+1, ptr->setOfValues, ptr->setOfValuesLen);
    pkt->dataLen = 1+ptr->setOfValuesLen ;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}


BtStatus BT_ATT_SendReadByGroupTypeRequest(BtATTReadByGroupTypeRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadByGroupTypeRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }       
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadByGroupTypeRequest(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST;
    StoreLE16(pkt->data+1, ptr->startingHandle);
    StoreLE16(pkt->data+3, ptr->endingHandle);
    if(ptr->type == 2)
    {
        StoreLE16(pkt->data+5, ptr->attributeType2);
        pkt->dataLen = 7;
    }
    else
    {
        memcpy(pkt->data+5, ptr->attributeType, 16);
        pkt->dataLen = 21;
    }
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}



BtStatus BT_ATT_SendReadByGroupTypeResponse(BtATTReadByGroupTypeResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendReadByGroupTypeResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendReadByGroupTypeResponse(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_READ_BY_GROUP_TYPE_RESPONSE;
    pkt->data[1] = ptr->length;
    memcpy(pkt->data+2, ptr->attributeDataList , ptr->attributeDataListLen);
    pkt->dataLen = 2 + ptr->attributeDataListLen;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}


BtStatus BT_ATT_SendWriteRequest(BtATTWriteRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendWriteRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }    
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendWriteRequest(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_WRITE_REQUEST;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    memcpy(pkt->data+3, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 3 + ptr->attributeValueLen;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}


BtStatus BT_ATT_SendWriteResponse(BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendWriteResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendWriteResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_WRITE_RESPONSE;
    pkt->dataLen = 1;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}


BtStatus BT_ATT_SendWriteCommand(BtATTWriteCommandStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendWriteCommand(): can not get buffer");      
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_WRITE_COMMAND;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    memcpy(pkt->data+3, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 3 + ptr->attributeValueLen;
    status = BT_ATT_Send(link, pkt);
    return status;
}



BtStatus BT_ATT_SendSignedWriteCommand(BtATTSignedWriteCommandStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendSignedWriteCommand(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_SIGNED_WRITE_COMMAND;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    memcpy(pkt->data+3, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 3 + ptr->attributeValueLen;
    L2Cap_FixCidSendSigned(0x0004, link, pkt, MAX_ATT_TX_MTU);
    return status;
}



BtStatus BT_ATT_SendPrepareWriteRequest(BtATTPrepareWriteRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendPrepareWriteRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendPrepareWriteRequest(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_PREPARE_WRITE_REQUEST;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    StoreLE16(pkt->data+3, ptr->valueOffset);    
    memcpy(pkt->data+5, ptr->partAttributeValue, ptr->partAttributeValueLen);
    pkt->dataLen = 5 + ptr->partAttributeValueLen;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}


BtStatus BT_ATT_SendPrepareWriteResponse(BtATTPrepareWriteResponseStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendPrepareWriteResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }       
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendPrepareWriteResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_PREPARE_WRITE_RESPONSE;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    StoreLE16(pkt->data+3, ptr->valueOffset);    
    memcpy(pkt->data+5, ptr->partAttributeValue, ptr->partAttributeValueLen);
    pkt->dataLen = 5 + ptr->partAttributeValueLen;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}



BtStatus BT_ATT_SendExecuteWriteRequest(BtATTExecuteWriteRequestStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendExecuteWriteRequest(): can not find any devices");    
        return BT_STATUS_FAILED;
    }       
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendExecuteWriteRequest(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST;
    pkt->data[1] = ptr->flags;
    pkt->dataLen = 2;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendExecuteWriteResponse(BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendExecuteWriteResponse(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendExecuteWriteResponse(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_EXECUTE_WRITE_RESPONSE;
    pkt->dataLen = 1;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}



BtStatus BT_ATT_SendHandleValueNotification(BtATTValueNotificationStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendHandleValueNotification(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    memcpy(pkt->data+3, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 3 + ptr->attributeValueLen;
    status = BT_ATT_Send(link, pkt);
    return status;
}


BtStatus BT_ATT_SendHandleValueIndication(BtATTValueIndicationStruct *ptr, BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendHandleValueIndication(): can not find any devices");    
        return BT_STATUS_FAILED;
    }    
    /* Check the transaction ongoing or not */
    if(conn->tx_transaction_ongoing !=0)
        return BT_STATUS_FAILED;    
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendHandleValueIndication(): can not get buffer");     
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_HANDLE_VALUE_INDICATION;
    StoreLE16(pkt->data+1, ptr->attributeHandle);
    memcpy(pkt->data+3, ptr->attributeValue, ptr->attributeValueLen);
    pkt->dataLen = 3 + ptr->attributeValueLen;
    status = BT_ATT_Send(link, pkt);
    if(status == BT_STATUS_PENDING)
    {
        conn->tx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
        BT_ATT_StartTimer(conn, &conn->txWaitTimer);
    }
    return status;
}

BtStatus BT_ATT_SendHandleValueConfirmation(BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
    {
        OS_Report("BT_ATT_SendHandleValueConfirmation(): can not find any devices");    
        return BT_STATUS_FAILED;
    }     
    pkt = BT_ATT_GetBuffer(link);
    if(pkt == NULL)
    {
        OS_Report("BT_ATT_SendHandleValueConfirmation(): can not get buffer");    
        return BT_STATUS_FAILED;
    }
    pkt->data[0] = BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION;
    pkt->dataLen = 1;
    status = BT_ATT_Send(link, pkt);
    conn->rx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->rxWaitTimer);
    return status;
}

U16 BT_ATT_GetNegotiationMTU(BtRemoteDevice *link)
{
    BtPacket *pkt;
    U8 status;
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    if(conn == NULL)
        return 0;
    else
        return conn->mtu;
}

#endif
