#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "btconfig_le.h"
#include "att_i.h"
#include "att.h"
#include "attdbmain.h"

#if ATT_PROTOCOL == XA_ENABLED
extern void AttSecCallback1(const BtEvent *Event);

#define SEC_ATT_ID1 (BtProtocolId)AttSecCallback1

extern U16 ATTDb_CheckHandleSize(U16 recordHandle);
extern BtStatus SM_Bond(BtDeviceContext *bdc, SmPairAttribute *attr);


U8 BT_ATT_HandleSecurityVerify(BtAttConn *conn, U8 *data, U16 length);
void BT_ATT_HandleReturnSentPkt(BtAttConn *conn);


/****************************************************************************
 *
 * Internal Function prototypes
 *
 *
 */
void BT_ATT_HandleRxReady(BtRemoteDevice *link, U8 *data, U16 length)
{
    U8 opcode;
    BtAttConn *conn;
    U8 *payload;
    U8 payload_len;
    U8 errcode;
    BtStatus status;
    
    conn = BT_ATT_GetConCtx(link);
    opcode = data[0];
    payload = data+1;
    payload_len = length -1;
    
    OS_Report("BT_ATT_HandleRxReady(): opcode=%d", opcode);
    if (conn == NULL)
    {
        OS_Report("BT_ATT_HandleRxReady(): can not find any devices");    
        return;
    }
    switch (opcode)
    {
        case BT_ATT_OPCODE_ERROR_RESPONSE:
            errcode = payload[3];    
            OS_Report("BT_ATT_HandleRxReady(): error code =%d", errcode);
            if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
            {
                if((errcode == BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION) ||
                    (errcode == BT_ATT_ERROR_CODE_INSUFFICIENT_ENCRYPTION) ||
                    (errcode == BT_ATT_ERROR_CODE_INSUFFICIENT_ENCRY_KEY_SIZE))
                {
                    /* Shall restart encryption again */
                    SmPairAttribute attr = {FALSE, SM_DEFAULT_OOB_FLAG, SM_DEFAULT_BOND_FLAG, NULL, 0};
                    Report(("SM bond starts"));
                    OS_MemCopy(conn->rxTempBuffer, data, length); /* Include the opcode */
                    conn->rxTempLength = length; /* Include the opcode */                
                    status = SM_Bond(DS_FindDevice(&link->bdAddr), &attr);
                    Report(("SM status:%02x",status));
                    Report(("BT ATT conn packet %08x", conn->pkt));
                    if( status != BT_STATUS_PENDING)
                    {
                        Report(("BT ATT return conn packet 0 "));
                        BT_ATT_HandleReturnSentPkt(conn);
                        BT_ATT_HandleErrorResponse(conn, payload, payload_len);
                    }
                }
                else
                {
                    Report(("BT ATT return conn packet 1"));
                    BT_ATT_HandleReturnSentPkt(conn);
                    BT_ATT_HandleErrorResponse(conn, payload, payload_len);
                }
            }
            else
            {
                Report(("BT ATT return conn packet 2"));
                BT_ATT_HandleReturnSentPkt(conn);
                BT_ATT_HandleErrorResponse(conn, payload, payload_len);
            }
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST:
            BT_ATT_HandleExchangeMTURequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_EXCHANGE_MTU_RESPONSE:
            BT_ATT_HandleExchangeMTUResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_FIND_INFO_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleFindInfoRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_FIND_INFO_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleFindInfoResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleFindByTypeValueRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleFindByTypeValueResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_BY_TYPE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleReadByTypeRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_BY_TYPE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleReadByTypeResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleReadRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleReadResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_BLOB_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleReadBlobRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_BLOB_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleReadBlobResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_MULTIPLE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleReadMultipleRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_MULTIPLE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleReadMultipleResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleReadByGroupTypeRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_READ_BY_GROUP_TYPE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleReadByGroupTypeResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_WRITE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleWriteRequest(conn, payload, payload_len);
            break;  
        case BT_ATT_OPCODE_WRITE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleWriteResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_WRITE_COMMAND:
            BT_ATT_HandleWriteCommand(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_PREPARE_WRITE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandlePrepareWriteRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_PREPARE_WRITE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandlePrepareWriteResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST:
            conn->trans_id++;
            BT_ATT_HandleExecuteWriteRequest(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_EXECUTE_WRITE_RESPONSE:
            BT_ATT_HandleReturnSentPkt(conn);
            BT_ATT_HandleExecuteWriteResponse(conn, payload, payload_len);
            //BT_ATT_RUN_Dlight_Test(link);
            break;
        case BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION:
            BT_ATT_HandleHandleValueNotification(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_HANDLE_VALUE_INDICATION:
            BT_ATT_HandleHandleValueIndication(conn, payload, payload_len);
            break;
        case BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION:
            BT_ATT_HandleHandleValueConfirmation(conn, payload, payload_len);
            break;  
        case BT_ATT_OPCODE_SIGNED_WRITE_COMMAND:
            payload = data;
            payload_len = length;
            BT_ATT_HandleHandleSignedWriteCommand(conn, payload, payload_len);
            break;  
        default:
            OS_Report("[ATT] BT_ATT_HandleRxReady : Invalid opcode %u", opcode);
            break;
    }
    
}


void BT_ATT_SecurityCheckDone(BtRemoteDevice *link)
{
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
    Report(("BT_ATT_SecurityCheckDone conn:%d", conn));
    if(conn != NULL)
        BT_ATT_HandleRxReady(link, conn->rxTempBuffer, conn->rxTempLength);

}

void BT_ATT_ReportEvent(U8 event_code, BtAttConn *conn, void *parm)
{
    OS_Report("BT_ATT_ReportEvent(): event_code=%d", event_code);
    ATTC(att_event).eType = event_code;
    ATTC(att_event).link = conn->link;
    ATTC(att_event).parms = (void *)parm;
    if(ATTC(callback) !=0)
        ATTC(callback)(&ATTC(att_event));
}


void BT_ATT_HandleErrorResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTErrorResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.opcode = data[0];
    ptr.attributeHandle = LEtoHost16(data+1);
    ptr.errorCode = data[3];
    OS_Report("BT_ATT_HandleErrorResponse: opcode=0x%x, errCode=0x%x",ptr.opcode, ptr.errorCode);
    BT_ATT_ReportEvent(BT_ATT_EVENT_ERROR_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleExchangeMTURequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTExchangeMTURequestStruct ptr;
    BtATTExchangeMTUResponseStruct rsp;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.clientRxMtu = LEtoHost16(data);
    if(ptr.clientRxMtu < conn->local_desired_mtu)
    {
        conn->mtu = ptr.clientRxMtu;
    }
    else
    {
        conn->mtu = conn->local_desired_mtu;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXCHANGE_MTU_REQUEST, conn, &ptr);
    rsp.serverRxMtu = conn->mtu;
    BT_ATT_SendExchangeMTUResponse(&rsp,conn->link);
}

void BT_ATT_HandleExchangeMTUResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTExchangeMTUResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.serverRxMtu = LEtoHost16(data);
    if(ptr.serverRxMtu < conn->mtu)
        conn->mtu = ptr.serverRxMtu;
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXCHANGE_MTU_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleFindInfoRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindInfoRequestStruct ptr;
    BtATTFindInfoResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);    
    if((ptr.startingHandle > ptr.endingHandle) ||
       (ptr.startingHandle == 0x00))
    {
        Report(("Start > End, Error"));
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_INFO_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
        return;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_INFO_REQUEST, conn, &ptr);
    size =  ATTDb_HandleGetInfoReq(ATTC(tempBuffer), conn->mtu - 2, &rsp.format, &ptr);
    if(size >0)
    {
        rsp.infoData = ATTC(tempBuffer);
        rsp.infoDataLen = size;
        BT_ATT_SendFindInfoResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_INFO_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }

}

void BT_ATT_HandleFindInfoResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindInfoResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.format = data[0];
    ptr.infoDataLen = length -1;
    ptr.infoData = data+1;
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_INFO_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleFindByTypeValueRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindByTypeValueRequestStruct ptr;
    BtATTFindByTypeValueResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);
    ptr.attributeType = LEtoHost16(data+4);
    ptr.attributeValueLen = length -6 ;
    ptr.attributeValue = data+6;    

    OS_Report("BT_ATT_HandleFindByTypeValueRequest(): 0x%x~0x%x, 0x%x, %d, 0x%x, 0x%x", 
        ptr.startingHandle, ptr.endingHandle, ptr.attributeType, ptr.attributeValueLen, ptr.attributeValue[0], ptr.attributeValue[1]);

    if((ptr.startingHandle > ptr.endingHandle) ||
       (ptr.startingHandle == 0x00))
    {
        Report(("Start > End, Error"));
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
        return;
    }
    
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_BY_TYPE_VALUE_REQUEST, conn, &ptr);
    size =  ATTDb_HandleGetFindByTypeValueReq(ATTC(tempBuffer), conn->mtu-1, &ptr, conn->link);
    if(size >0)
    {
        rsp.handleInfoList = ATTC(tempBuffer);
        rsp.handleInfoListLen = size;
        BT_ATT_SendFindByTypeValueResponse(&rsp, conn->link);
    }
    else
    {
        rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_ATTRIBUTE_NOT_FOUND;
        rsp_err.opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleFindByTypeValueResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTFindByTypeValueResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.handleInfoListLen = length;
    ptr.handleInfoList = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_FIND_BY_TYPE_VALUE_RESPONSE, conn, &ptr);
}

static void AttHandleReadByTypeRequest(BtAttConn *conn){
    BtATTReadByTypeRequestStruct *ptr = &conn->req_status.read_by_type;
    BtATTReadByTypeResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U8 error_code = 0x00;
    U16 error_handle;
    
    conn->rspLength +=  ATTDb_HandleReadByTypeReq(&conn->rspBuffer[conn->rspLength], 
                                                                                     conn->mtu - conn->rspLength - 1,
                                                                                     ptr, 
                                                                                     conn->link, 
                                                                                     &error_code, 
                                                                                     &error_handle);

    if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
    {
        //BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
        conn->waitApplicationResponse = 1;
        Report(("Need application response"));
        return;
    }else if(conn->rspLength > 0){
        rsp.length = ptr->attrPairLen;
        rsp.handleDataList = conn->rspBuffer;
        rsp.handleDataListLen = conn->rspLength;
        BT_ATT_SendReadByTypeResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, conn->rxTempBuffer+1, conn->rxTempLength-1) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }
        }     
        
        rsp_err.attributeHandle = error_handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleReadByTypeRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByTypeRequestStruct *ptr;
    BtATTReadByTypeResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code = 0x00;
    U16 error_handle;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);

    if((data-1) != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
        conn->rxTempLength = length+1;
    }
    conn->rspLength = 0;
    ptr = &conn->req_status.read_by_type;
    ptr->startingHandle = LEtoHost16(data);
    ptr->endingHandle = LEtoHost16(data+2);
    ptr->currHandle = 0;//ptr->startingHandle;
    ptr->attrPairLen = 0;
    OS_Report("BT_ATT_HandleReadByTypeRequest(): 0x%x~0x%x", ptr->startingHandle, ptr->endingHandle);

    if((length - 4) == 2)
    {
        ptr->type = 2;
        ptr->attributeType2 = LEtoHost16(data+4);
    }
    else
    {
        ptr->type = 16;
        ptr->attributeType = data+4;
    }
    if(ptr->startingHandle > ptr->endingHandle)
    {
        Report(("Start > End, Error"));
        rsp_err.attributeHandle = ptr->startingHandle;
        rsp_err.errorCode = BT_ATT_ERROR_CODE_INVALID_HANDLE;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
        return;
    }    
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_TYPE_REQUEST, conn, ptr);

    AttHandleReadByTypeRequest(conn);
#if 0
    size =  ATTDb_HandleReadByTypeReq(ATTC(tempBuffer), conn->mtu-2, &ptr, &rsp.length, conn->link, &error_code, &error_handle);
    if(size >0)
    {
        rsp.handleDataList = ATTC(tempBuffer);
        rsp.handleDataListLen = size;
        BT_ATT_SendReadByTypeResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }
        }else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            Report(("Need application response"));
            return;
        }        
        
        rsp_err.attributeHandle = error_handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }        
#endif
}

void BT_ATT_HandleReadByTypeResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByTypeResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.length = data[0];
    ptr.handleDataListLen = length -1;
    ptr.handleDataList = data+1;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_TYPE_RESPONSE, conn, &ptr);
}

static void AttHandleReadRequest(BtAttConn *conn){
    BtATTReadRequestStruct *ptr = &conn->req_status.read;
    BtATTReadResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U8 error_code;

    conn->rspLength +=  ATTDb_HandleReadReq(&conn->rspBuffer[conn->rspLength], 
                                                                          conn->mtu - conn->rspLength - 1,
                                                                          ptr, &error_code, conn->link);
    

    if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE){
        //BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
        conn->waitApplicationResponse = 1;
        Report(("Need application response"));
        return;
    }else if(error_code == BT_ATT_ERROR_CODE_NO_ERROR){
        rsp.attributeValue = conn->rspBuffer;
        rsp.attributeValueLen= conn->rspLength;
        BT_ATT_SendReadResponse(&rsp, conn->link);
    }else{
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE){
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, conn->rxTempBuffer+1, conn->rxTempLength-1) == 1){
                return;
            }else{
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }
        }  
        rsp_err.attributeHandle = ptr->handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }    
}

void BT_ATT_HandleReadRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadRequestStruct *ptr;
    BtATTReadResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);

    if((data-1) != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
        conn->rxTempLength = length+1;
    }
    conn->rspLength = 0;
    ptr = &conn->req_status.read;
    
    ptr->handle = LEtoHost16(data);
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_REQUEST, conn, ptr);

    AttHandleReadRequest(conn);
#if 0    
    size =  ATTDb_HandleReadReq(ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, conn->link);
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.attributeValue = ATTC(tempBuffer);
        rsp.attributeValueLen= size;
        BT_ATT_SendReadResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }  
        else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            Report(("Need application response"));
            return;
        }        
        
        rsp_err.attributeHandle = ptr.handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
#endif
}

void BT_ATT_HandleReadResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.attributeValueLen = length;
    ptr.attributeValue = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_RESPONSE, conn, &ptr);
}

static void AttHandleReadBlobRequest(BtAttConn *conn){
    BtATTReadBlobRequestStruct *ptr;
    BtATTReadBlobResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    //U16 size;
    U8 error_code;

    ptr = &conn->req_status.read_blob;
    conn->rspLength +=  ATTDb_HandleReadBlobReq(&conn->rspBuffer[conn->rspLength], 
                                                                                  conn->mtu - conn->rspLength - 1,
                                                                                  ptr, &error_code, conn->link);

    if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
    {
        //BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
        conn->waitApplicationResponse = 1;
        Report(("Need application response"));
        return;
    }else if((conn->rspLength > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR)){
        rsp.partAttributeValue = conn->rspBuffer;
        rsp.partAttributeValueLen = conn->rspLength;
        BT_ATT_SendReadBlobResponse(&rsp, conn->link);
    }else{
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, conn->rxTempBuffer+1, conn->rxTempLength-1) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }

        rsp_err.attributeHandle = ptr->attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BLOB_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    } 
}

void BT_ATT_HandleReadBlobRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadBlobRequestStruct *ptr;
    BtATTReadBlobResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);

    if((data-1) != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
        conn->rxTempLength = length+1;
    }
    conn->rspLength = 0;
    ptr = &conn->req_status.read_blob;
    
    ptr->attributeHandle = LEtoHost16(data);
    ptr->valueOffset = LEtoHost16(data+2);   
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BLOB_REQUEST, conn, ptr);

    AttHandleReadBlobRequest(conn);

#if 0    
    size =  ATTDb_HandleReadBlobReq(ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, conn->link);
    if((size > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        rsp.partAttributeValue = (U8 *)ATTC(tempBuffer);
        rsp.partAttributeValueLen = size;
        BT_ATT_SendReadBlobResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }    
        else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            Report(("Need application response"));
            return;
        }        

        rsp_err.attributeHandle = ptr.attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BLOB_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }            
#endif
}

void BT_ATT_HandleReadBlobResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadBlobResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.partAttributeValueLen = length;
    ptr.partAttributeValue = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BLOB_RESPONSE, conn, &ptr);
}

static void AttHandleReadMultipleRequest(BtAttConn *conn){
    U16 size;
    U8 error_code;
    U16 error_handle;
    BtATTReadMultipleResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    BtATTReadMultipleRequestStruct *req = &conn->req_status.read_multiple;
    
    conn->rspLength +=  ATTDb_HandleReadMultipleReq(&conn->rspBuffer[conn->rspLength], 
                                                                                         conn->mtu - conn->rspLength - 1, 
                                                                                         req, 
                                                                                         &error_code, 
                                                                                         &error_handle, 
                                                                                         conn->link);

    if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
    {
        //BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
        conn->waitApplicationResponse = 1;
        Report(("Need application response"));
        return;
    }
    else if((conn->rspLength > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        rsp.setOfValues = conn->rspBuffer;
        rsp.setOfValuesLen = conn->rspLength;
        BT_ATT_SendReadMultipleResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, conn->rxTempBuffer+1, conn->rxTempLength-1) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }
        rsp_err.attributeHandle = error_handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_MULTIPLE_REQUEST; 
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleReadMultipleRequest(BtAttConn *conn, U8 *data, U16 length)
{
    //BtATTReadMultipleRequestStruct *ptr;
    BtATTReadMultipleResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;
    U16 error_handle;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    
    /* GATT migration */
    // keep the request raw data
    if((data-1) != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
        conn->rxTempLength = length+1;
    }
    conn->req_status.read_multiple.setOfHandlesLen = length;
    conn->req_status.read_multiple.setOfHandles = conn->rxTempBuffer;
    conn->req_status.read_multiple.currHandle = conn->req_status.read_multiple.setOfHandles;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_MULTIPLE_REQUEST, conn, &conn->req_status.read_multiple);

    // initialize rsp length
    conn->rspLength = 0;

    AttHandleReadMultipleRequest(conn);
#if 0
    //size =  ATTDb_HandleReadMultipleReq(ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, &error_handle, conn->link);
    conn->rspLength =  ATTDb_HandleReadMultipleReq(conn->rspBuffer, conn->mtu-1, &conn->req_status.read_multiple, &error_code, &error_handle, conn->link);

    if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
    {
        rsp.setOfValues = (U8*)ATTC(tempBuffer);
        rsp.setOfValuesLen = size;
        BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
        Report(("Need application response"));
        return;
    }
    else if((size > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        //rsp.setOfValues = &ATTC(tempBuffer);
        //rsp.setOfValuesLen = size;
        rsp.setOfValues = &conn->rspBuffer;
        rsp.setOfValuesLen = conn->rspLength;
        BT_ATT_SendReadMultipleResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }
        rsp_err.attributeHandle = error_handle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_MULTIPLE_REQUEST; 
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
#endif
}

void BT_ATT_HandleReadMultipleResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadMultipleResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.setOfValuesLen = length;
    ptr.setOfValues = data;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_MULTIPLE_RESPONSE, conn, &ptr);
}


void BT_ATT_HandleReadByGroupTypeRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByGroupTypeRequestStruct ptr;
    BtATTReadByGroupTypeResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U16 size;
    U8 error_code;
    U16 error_handler;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.startingHandle = LEtoHost16(data);
    ptr.endingHandle = LEtoHost16(data+2);
    if((length -4) == 2)
    {
        ptr.type = 2;
        ptr.attributeType2 = LEtoHost16(data+4);
    }
    else
    {
        ptr.type = 16;
        ptr.attributeType = data+4;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_GROUP_TYPE_REQUEST, conn, &ptr);
    
    size =  ATTDb_HandleReadGroupReq(ATTC(tempBuffer), conn->mtu-1, &ptr, &error_code, &error_handler, conn->link);
    if((size > 0) || (error_code == BT_ATT_ERROR_CODE_NO_ERROR))
    {
        rsp.length = ATTC(tempBuffer)[0];
        rsp.attributeDataList = &ATTC(tempBuffer)[1];
        rsp.attributeDataListLen = size-1;
        BT_ATT_SendReadByGroupTypeResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }    
        else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            Report(("Need application response"));
            return;
        }         
        if(error_code == BT_ATT_ERROR_CODE_READ_NOT_PERMITTED)
            rsp_err.attributeHandle = error_handler;
        else
            rsp_err.attributeHandle = ptr.startingHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleReadByGroupTypeResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTReadByGroupTypeResponseStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.length = data[0];
    ptr.attributeDataListLen = length -1;
    ptr.attributeDataList = data+1;
    BT_ATT_ReportEvent(BT_ATT_EVENT_READ_BY_GROUP_TYPE_RESPONSE, conn, &ptr);
}

void BT_ATT_HandleWriteRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTWriteRequestStruct *ptr;
    BtATTErrorResponseStruct rsp_err;
    U8 error_code;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);

    // keep the request raw data
    if(data != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
        conn->rxTempLength = length+1;
    }
    ptr = &conn->req_status.write;
    conn->rspLength = 0;
    
    ptr->attributeHandle = LEtoHost16(data);
    ptr->attributeValue = conn->rxTempBuffer+3;
    ptr->attributeValueLen = length -2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_WRITE_REQUEST, conn, &ptr);
    error_code = ATTDb_HandleWriteRequest(ptr, conn->link, TRUE);
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        BT_ATT_SendWriteResponse(conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }  
        else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            //BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            conn->waitApplicationResponse = 1;
            Report(("Need application response"));
            return;
        }
        rsp_err.attributeHandle = ptr->attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
    
}

void BT_ATT_HandleWriteResponse(BtAttConn *conn, U8 *data, U16 length)
{
    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    BT_ATT_ReportEvent(BT_ATT_EVENT_WRITE_RESPONSE, conn, NULL);
}

void BT_ATT_HandleWriteCommand(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTWriteRequestStruct ptr;

    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValueLen = length-2;
    ptr.attributeValue = data+2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_WRITE_COMMAND, conn, &ptr);
    ATTDb_HandleWriteRequest(&ptr, conn->link, FALSE);
    /* Not required to send response */
}

static void AttHandlePrepareWriteRequest(BtAttConn *conn, U8 error_code){
    BtATTPrepareWriteRequestStruct *ptr;
    BtATTPrepareWriteResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U8 i=0;

    ptr = &conn->req_status.prepare_write;
    OS_Report("AttHandlePrepareWriteRequest : error_code=%u", error_code);
    switch(error_code){
    case BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE:
        conn->waitApplicationResponse = 1;
    case BT_ATT_ERROR_CODE_NO_ERROR:
        for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
        {
            if(conn->prepareWriteBuffer[i].available == 0)
            {
                OS_Report("prepareWriteBuffer[%u] is available", i);
                conn->prepareWriteBuffer[i].available = 1;
                conn->prepareWriteBuffer[i].handle = ptr->attributeHandle;
                conn->prepareWriteBuffer[i].offset = ptr->valueOffset;
                conn->prepareWriteBuffer[i].length = ptr->partAttributeValueLen;
                if(conn->prepareWriteBuffer[i].length > MAX_ATT_TX_MTU-5)
                {
                    memcpy(conn->prepareWriteBuffer[i].writeBuffer, ptr->partAttributeValue, MAX_ATT_TX_MTU-5);
                    conn->prepareWriteBuffer[i].length = MAX_ATT_TX_MTU-5;
                }
                else
                {
                    memcpy(conn->prepareWriteBuffer[i].writeBuffer, ptr->partAttributeValue, conn->prepareWriteBuffer[i].length);
                }
                break;
            }
        }
        if(i == MAX_PREPARE_WRITE_NO){
            error_code = BT_ATT_ERROR_CODE_INSUFFICIENT_RESOURCES;
            conn->waitApplicationResponse = 0;
    }
        break;
    default:
        break;
    }
    if(error_code != BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE){    
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR || 
       error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
    {
        rsp.attributeHandle = ptr->attributeHandle;
        rsp.partAttributeValue = ptr->partAttributeValue;
        rsp.partAttributeValueLen = ptr->partAttributeValueLen;
        rsp.valueOffset = ptr->valueOffset;
        BT_ATT_SendPrepareWriteResponse(&rsp, conn->link);        
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        rsp_err.attributeHandle = ptr->attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_PREPARE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
    }
}

void BT_ATT_HandlePrepareWriteRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTPrepareWriteRequestStruct *ptr;
    BtATTPrepareWriteResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    U8 error_code;
    U8 i=0;
    U16 size;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);

    // keep the request raw data
    if((data-1) != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
        conn->rxTempLength = length+1;
    }
    ptr = &conn->req_status.prepare_write;
    conn->rspLength = 0;
    
    ptr->attributeHandle = LEtoHost16(data);
    ptr->valueOffset = LEtoHost16(data+2);
    ptr->partAttributeValueLen = length-4;
    ptr->partAttributeValue = conn->rxTempBuffer+5;
    BT_ATT_ReportEvent(BT_ATT_EVENT_PREPARE_WRITE_REQUEST, conn, ptr);

    //error_code = ATTDb_CheckHandlePermission(conn->link, ptr->attributeHandle, ATT_OP_WRITE);
    error_code = ATTDb_HandlePrepareWriteRequest(ptr, conn->link);

    AttHandlePrepareWriteRequest(conn, error_code);
#if 0
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        /*
        size = ATTDb_CheckHandleSize(ptr->attributeHandle);
        if(ptr.valueOffset > size)
        {
            error_code = BT_ATT_ERROR_CODE_INVALID_OFFSET;
        }
        */
        for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
        {
            if(conn->prepareWriteBuffer[i].available == 0)
            {
                conn->prepareWriteBuffer[i].available = 1;
                conn->prepareWriteBuffer[i].handle = ptr.attributeHandle;
                conn->prepareWriteBuffer[i].offset = ptr.valueOffset;
                conn->prepareWriteBuffer[i].length = ptr.partAttributeValueLen;
                if(conn->prepareWriteBuffer[i].length > MAX_ATT_TX_MTU-5)
                {
                    memcpy(conn->prepareWriteBuffer[i].writeBuffer, ptr.partAttributeValue, MAX_ATT_TX_MTU-5);
                    conn->prepareWriteBuffer[i].length = MAX_ATT_TX_MTU-5;
                }
                else
                {
                    memcpy(conn->prepareWriteBuffer[i].writeBuffer, ptr.partAttributeValue, conn->prepareWriteBuffer[i].length);
                }
                break;
            }
        }
        if(i == MAX_PREPARE_WRITE_NO)
            error_code = BT_ATT_ERROR_CODE_INSUFFICIENT_RESOURCES;
        /* No buffer available*/
    }
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.attributeHandle = ptr->attributeHandle;
        rsp.partAttributeValue = ptr->partAttributeValue;
        rsp.partAttributeValueLen = ptr->partAttributeValueLen;
        rsp.valueOffset = ptr->valueOffset;
        BT_ATT_SendPrepareWriteResponse(&rsp, conn->link);        
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }   
        else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            Report(("Need application response"));
            return;
        }         
        rsp_err.attributeHandle = ptr.attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_PREPARE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
#endif
}

void BT_ATT_HandlePrepareWriteResponse(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTPrepareWriteRequestStruct ptr;

    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.valueOffset = LEtoHost16(data+2);
    ptr.partAttributeValueLen = length-4;
    ptr.partAttributeValue = data+4;
    BT_ATT_ReportEvent(BT_ATT_EVENT_PREPARE_WRITE_RESPONSE, conn, &ptr);
}

static void AttHandleExecuteWriteRequest(BtAttConn *conn){
    BtATTExecuteWriteRequestStruct *ptr;
    U8 i = 0;
    U8 error_code = BT_ATT_ERROR_CODE_NO_ERROR;
    BtATTErrorResponseStruct rsp_err;
    U8 cancel = 0;

    ptr = &conn->req_status.execute_write;
    if((ptr->flags & 0x1) == 0 || ptr->err != BT_ATT_ERROR_CODE_NO_ERROR){
        cancel = 1;
    }

    for(i = ptr->currIndex;i < MAX_PREPARE_WRITE_NO;i++, ptr->currIndex++)
    {
        if(conn->prepareWriteBuffer[i].available == 1)
        {
            OS_Report("prepareWriteBuffer[%u] is available, handle=0x%x", i, conn->prepareWriteBuffer[i].handle);
            error_code = ATTDB_HandleExecuteWriteRequest(&conn->prepareWriteBuffer[i], conn->link, cancel);
            ptr->currHandle = conn->prepareWriteBuffer[i].handle;
            switch(error_code){
            case BT_ATT_ERROR_CODE_NO_ERROR:
                break;
            case BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE:
                conn->waitApplicationResponse = 1;
                return;
            default:
                if(ptr->err != BT_ATT_ERROR_CODE_NO_ERROR){
                    ptr->err = error_code;
                    ptr->errHandle = ptr->currHandle;
                    cancel = 1;
                }
                break;
            }
        }
    }

    for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
    {
        conn->prepareWriteBuffer[i].available = 0;
    }
    error_code = ptr->err;
    if(error_code != BT_ATT_ERROR_CODE_NO_ERROR)
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
        }         
        rsp_err.errorCode = error_code;
        rsp_err.attributeHandle = ptr->errHandle;
        rsp_err.opcode = BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
    else
    {
        BT_ATT_SendExecuteWriteResponse(conn->link);
    }
}

void BT_ATT_HandleExecuteWriteRequest(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTExecuteWriteRequestStruct *ptr;
    
    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);

    ptr = &conn->req_status.execute_write;
    ptr->currIndex = 0;
    ptr->currHandle = 0;
    ptr->err = BT_ATT_ERROR_CODE_NO_ERROR;
    ptr->flags = data[0];

    BT_ATT_ReportEvent(BT_ATT_EVENT_EXECUTE_WRITE_REQUEST, conn, &ptr);

    AttHandleExecuteWriteRequest(conn);
#if 0    
    //if(ptr.flags == 0x01)
    //{
        for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
        {
            if(conn->prepareWriteBuffer[i].available == 1)
            {
                error_code = ATTDB_HandleExecuteWriteRequest(&conn->prepareWriteBuffer[i], conn->link);
                if(error_code != BT_ATT_ERROR_CODE_NO_ERROR)
                {
                    rsp_err.attributeHandle = conn->prepareWriteBuffer[i].handle;
                    break;
                    /* ignore error, flush all*/
                }
            }
        }
    //}
    for(i=0;i<MAX_PREPARE_WRITE_NO;i++)
    {
        conn->prepareWriteBuffer[i].available = 0;
    }
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXECUTE_WRITE_REQUEST, conn, &ptr);
    if(error_code != BT_ATT_ERROR_CODE_NO_ERROR)
    {
        Report(("ATT event with error code:%d", error_code));
        if(error_code == BT_ATT_ERROR_CODE_WAIT_SECURITY_PROCEDURE)
        {
            Report(("Need wait security procedure"));
            if (BT_ATT_HandleSecurityVerify(conn, data, length) == 1)
                return;
            else
            {
                error_code =BT_ATT_ERROR_CODE_INSUFFICIENT_AUTNEHTICATION; 
            }

        }  
        else if(error_code == BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE)
        {
            BT_ATT_HandleApplicationPrepareResponseData(conn, data, length);
            Report(("Need application response"));
            return;
        }         
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
    else
    {
        BT_ATT_SendExecuteWriteResponse(conn->link);
    }
#endif
}

void BT_ATT_HandleExecuteWriteResponse(BtAttConn *conn, U8 *data, U16 length)
{
    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    BT_ATT_ReportEvent(BT_ATT_EVENT_EXECUTE_WRITE_RESPONSE, conn, NULL);
}

void BT_ATT_HandleHandleValueNotification(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTValueNotificationStruct ptr;
    
    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValueLen = length-2;
    ptr.attributeValue = data+2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_VALUE_NOTIFICATION, conn, &ptr);
}

void BT_ATT_HandleHandleValueIndication(BtAttConn *conn, U8 *data, U16 length)
{
    BtATTValueIndicationStruct ptr;

    conn->rx_transaction_ongoing = ATT_TRANSACTION_ONGOING;
    BT_ATT_StartTimer(conn, &conn->rxWaitTimer);
    ptr.attributeHandle = LEtoHost16(data);
    ptr.attributeValueLen = length-2;
    ptr.attributeValue = data+2;
    BT_ATT_ReportEvent(BT_ATT_EVENT_VALUE_INDICATION, conn, &ptr);
    BT_ATT_SendHandleValueConfirmation(conn->link);

}

void BT_ATT_HandleHandleValueConfirmation(BtAttConn *conn, U8 *data, U16 length)
{
    conn->tx_transaction_ongoing = ATT_TRANSACTION_NONE;
    EVM_ResetTimer(&conn->txWaitTimer);
    BT_ATT_ReportEvent(BT_ATT_EVENT_VALUE_CONFIRMATION, conn, NULL);
    
}

#ifdef __BT_4_0_BLE__
static void attSignWriteVerifyCB(const BtEvent *event)
{
    int j;
    U8 status;
    BtPacket *pkt;
    BtRemoteDevice *link = ME_FindRemoteDevice((BD_ADDR *)&(event->p.smSignData.bdAddr));
    BtAttConn *conn = BT_ATT_GetConCtx(link);
    
    if (conn == NULL)
    {
        OS_Report("attSignWriteVerifyCB(): can not find any devices");    
        return;
    }
    switch (event->eType)
    {
        case BTEVENT_LE_SIGN_VERIFY_COMPLETE:    
        if (event->errCode == BEC_NO_ERROR)
        { 
            BtATTSignedWriteCommandStruct ptr;
            ptr.attributeHandle = LEtoHost16(event->p.smSignData.data + 1);
            ptr.attributeValueLen = event->p.smSignData.dataLen - 3;
            ptr.attributeValue = event->p.smSignData.data + 3;
//            ptr.authenticationSignature = data + 2 + ptr.attributeValueLen;
            BT_ATT_ReportEvent(BT_ATT_EVENT_SIGNED_WRITE_COMMAND, conn, &ptr);
            ATTDb_HandleSignedWriteCommand(ATTC(tempBuffer),&ptr, conn->link);
        }
        break;
    }
}

BtSecurityToken attSecurityToken;      /* Passed to ME for access requests. */
BtSecurityRecord attSecurityRecord;

static void attSignWriteSecCallback(const BtEvent *Event)
{
    SEC_Unregister(&attSecurityRecord);
}
#endif

void BT_ATT_HandleHandleSignedWriteCommand(BtAttConn *conn, U8 *data, U16 length)
{
#ifdef __BT_4_0_BLE__
    BtDeviceRecord record;
    if (SEC_FindDeviceRecord(&conn->link->bdAddr, &record) != BT_STATUS_SUCCESS ||
        (record.smKeyType & SM_KEY_CSRK) == 0)
    {
        SEC_DeleteDeviceRecord(&conn->link->bdAddr);
        attSecurityRecord.level_type = BT_SEC_MED;
        attSecurityRecord.id = attSignWriteSecCallback;
        attSecurityRecord.channel = 0x001F;
        SEC_RegisterRecord(&attSecurityRecord);
        attSecurityRecord.level = BSL_NO_SECURITY;  //overwrite
        
        attSecurityToken.remDev = conn->link;
        attSecurityToken.id = attSignWriteSecCallback;
        attSecurityToken.channel = 0x001F;
        SEC_AccessRequest(&attSecurityToken);
        return;
    }

    if (ME_LE_VerifySignedData(attSignWriteVerifyCB, &conn->link->bdAddr, data, length) == BT_STATUS_PENDING)
    {
        return;
    }

#endif
}


void BT_ATT_SecurityCheckTimerHandler(EvmTimer* timer)
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


U8 BT_ATT_HandleSecurityVerify(BtAttConn *conn, U8 *data, U16 length)
{
    if(conn->securityCheckCount !=0)
    {
        conn->securityCheckCount = 0;
        return 0x00; /* The same op has done security check before, shall return error code */
    }
    conn->securityCheckCount++;
    EVM_ResetTimer(&conn->securityCheckTimer);
    conn->securityCheckTimer.func = BT_ATT_SecurityCheckTimerHandler;
    EVM_StartTimer(&conn->securityCheckTimer, 300000);
    /* Security Check failed, do security procedure */
    OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
    conn->rxTempLength = length+1; /* Include the opcode */
    conn->secRecord.id =  SEC_ATT_ID1;
    conn->secRecord.channel = (U32) conn;
    if(ME_GetDeviceType(&conn->link->bdAddr) == BT_DEV_TYPE_LE)
        conn->secRecord.level = BSL_AUTHENTICATION_OUT;
    else
        conn->secRecord.level = BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN | BSL_ENCRYPTION_OUT;
        
    SEC_RegisterRecord(&conn->secRecord);
    
    conn->secToken.id = SEC_ATT_ID1;
    conn->secToken.remDev = conn->link;
    conn->secToken.channel = (U32) conn;
    conn->secToken.incoming = FALSE;
    SEC_AccessRequest(&(conn->secToken));
    return 0x01;

    /* Start SM procedure */
    
}

void BT_ATT_HandleApplicationPrepareResponseData(BtAttConn *conn, U8 *data, U16 length)
{
    if((data-1) != conn->rxTempBuffer){
        OS_MemCopy(conn->rxTempBuffer, data-1, length+1); /* Include the opcode */
    }
    conn->rxTempLength = length+1; /* Include the opcode */
    conn->waitApplicationResponse = 1;
    Report(("BT Att BT_ATT_HandleApplicationResponse status"));    
    //return 0x01;
}

void BT_ATT_HandleApplicationResponseReadOperation(BtAttConn *conn, U8 error_code, U8 *rsp_data, U16 rsp_length)
{
    U16 handler;
    BtATTReadResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;
    
    handler = LEtoHost16(conn->rxTempBuffer+1);
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.attributeValue = rsp_data;
        rsp.attributeValueLen= rsp_length;
        BT_ATT_SendReadResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        rsp_err.attributeHandle = handler;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }            
}

void BT_ATT_HandleApplicationResponseReadBlobOperation(BtAttConn *conn, U8 error_code, U8 *rsp_data, U16 rsp_length)
{
    U16 handler;
    U16 valueOffset;
    
    BtATTReadBlobResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;

    
    handler = LEtoHost16(conn->rxTempBuffer+1);
    valueOffset = LEtoHost16(conn->rxTempBuffer+3);  

    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.partAttributeValue = rsp_data;
        rsp.partAttributeValueLen = rsp_length;
        BT_ATT_SendReadBlobResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));

        rsp_err.attributeHandle = handler;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BLOB_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }            
}

void BT_ATT_HandleApplicationResponseReadMultipleOperation(BtAttConn *conn, U8 error_code, U8 *rsp_data, U16 rsp_length)
{
    BtATTErrorResponseStruct rsp_err;

    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        /* Upper layer will return value in callback function */
        AttHandleReadMultipleRequest(conn);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
       
        rsp_err.attributeHandle = LEtoHost16(conn->rxTempBuffer+1);
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_MULTIPLE_REQUEST; 
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleApplicationResponseReadByTypeOperation(BtAttConn *conn, U8 error_code, U8 *rsp_data, U16 rsp_length)
{
    BtATTErrorResponseStruct rsp_err;

    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        /* Upper layer will return value in callback function */
        AttHandleReadByTypeRequest(conn);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));

        rsp_err.attributeHandle = LEtoHost16(conn->rxTempBuffer+1);
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST; 
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}


void BT_ATT_HandleApplicationResponseReadGroupOperation(BtAttConn *conn, U8 error_code, U8 *rsp_data, U16 rsp_length)
{
    BtATTReadByGroupTypeResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;

    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.length = rsp_data[0];
        rsp.attributeDataList = rsp_data;
        rsp.attributeDataListLen = rsp_length -1;
        BT_ATT_SendReadByGroupTypeResponse(&rsp, conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        rsp_err.attributeHandle = LEtoHost16(conn->rxTempBuffer+1);
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleApplicationResponseWriteOperation(BtAttConn *conn, U8 error_code)
{
    
    BtATTWriteRequestStruct ptr;
    BtATTErrorResponseStruct rsp_err;


    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        BT_ATT_SendWriteResponse(conn->link);
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        rsp_err.attributeHandle = LEtoHost16(conn->rxTempBuffer+1);
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }
}

void BT_ATT_HandleApplicationResponsePrepareWriteOperation(BtAttConn *conn, U8 error_code)
{
    BtATTPrepareWriteRequestStruct ptr;
    BtATTPrepareWriteResponseStruct rsp;
    BtATTErrorResponseStruct rsp_err;

    ptr.attributeHandle = LEtoHost16(conn->rxTempBuffer+1);
    ptr.valueOffset = LEtoHost16(conn->rxTempBuffer+3);
    ptr.partAttributeValueLen = conn->rxTempLength - 5;
    ptr.partAttributeValue = (conn->rxTempBuffer+5);
    if(error_code == BT_ATT_ERROR_CODE_NO_ERROR)
    {
        rsp.attributeHandle = ptr.attributeHandle;
        rsp.partAttributeValue = ptr.partAttributeValue;
        rsp.partAttributeValueLen = ptr.partAttributeValueLen;
        rsp.valueOffset = ptr.valueOffset;
        BT_ATT_SendPrepareWriteResponse(&rsp, conn->link);        
    }
    else
    {
        Report(("ATT event with error code:%d", error_code));
        rsp_err.attributeHandle = ptr.attributeHandle;
        rsp_err.errorCode = error_code;
        rsp_err.opcode = BT_ATT_OPCODE_PREPARE_WRITE_REQUEST;
        BT_ATT_SendErrorResponse(&rsp_err, conn->link);
    }    
}

void BT_ATT_HandleApplicationResponseExecuteWriteOperation(BtAttConn *conn, U8 error_code, U16 errHandle){
    BtATTExecuteWriteRequestStruct *ptr = &conn->req_status.execute_write;
    ptr->currIndex++;
    if(ptr->err == BT_ATT_ERROR_CODE_NO_ERROR && error_code != BT_ATT_ERROR_CODE_NO_ERROR){
       ptr->err =  error_code;
       ptr->errHandle = errHandle;
    }
    AttHandleExecuteWriteRequest(conn);
}

void BT_ATTHandleApplicationResponse(BtRemoteDevice *link, U8 error_code, U8 *data, U16 length)
{
    BtAttConn *conn;
    U8 opcode;

    conn = BT_ATT_GetConCtx(link);
    Report(("BT_ATTHandleApplicationResponse"));

    if(conn == NULL)
    {
        Report(("can not find any devices"));
        return;
    }
    if(conn->waitApplicationResponse != 1)
    {
        Report(("No command waiting to response??????????"));
        return;
    }
    conn->waitApplicationResponse = 0;
    opcode = conn->rxTempBuffer[0];
    Report(("Previous command opcode:%d", opcode));    
    switch (opcode)
    {
        case BT_ATT_OPCODE_READ_REQUEST:
            BT_ATT_HandleApplicationResponseReadOperation(conn, error_code, data, length);
            break;
        case BT_ATT_OPCODE_READ_BLOB_REQUEST:
            BT_ATT_HandleApplicationResponseReadBlobOperation(conn, error_code, data, length);
            break;
        case BT_ATT_OPCODE_READ_MULTIPLE_REQUEST:
            /* if error response, the data[0] and data[1] shall be the failed handler*/
            BT_ATT_HandleApplicationResponseReadMultipleOperation(conn, error_code, data, length);
            break;
        case BT_ATT_OPCODE_READ_BY_TYPE_REQUEST:
            /* if error response, the data[0] and data[1] shall be the failed handler*/
            BT_ATT_HandleApplicationResponseReadByTypeOperation(conn, error_code, data, length);            
            break;
        case BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST:
            /* if data[0] is the element length, length is total raw data length+1, data[1]~data[length] is the data  */
            BT_ATT_HandleApplicationResponseReadGroupOperation(conn, error_code, data, length);
            break;
        case BT_ATT_OPCODE_WRITE_REQUEST:
            BT_ATT_HandleApplicationResponseWriteOperation(conn, error_code);
            break;  
        case BT_ATT_OPCODE_PREPARE_WRITE_REQUEST:
            BT_ATT_HandleApplicationResponsePrepareWriteOperation(conn, error_code);
            break;
        case BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST:
            BT_ATT_HandleApplicationResponseExecuteWriteOperation(conn, error_code, *(U16*)data);
            break;
    }
}

void BT_ATT_HandleSMCompleted(BtRemoteDevice *link)
{
    BtAttConn *conn;
    BtPacket *pkt;
    
    conn = BT_ATT_GetConCtx(link);
    Report(("BT_ATT_HandleSMCompleted:%d", conn));
    if(conn != NULL)
    {
        if(link->encryptState != BES_ENCRYPTED)
        {
            Report(("SM compled but failed."));
//            MeDisconnectLink(link, 100);
            conn = BT_ATT_GetConCtx(link);
            if(conn->bufferPkt !=0 ) /* To avioid duplicate send. */
            {
                Report(("Conn:%08x",conn));
                Report(("conn->freePacketList:%08x",conn->freePacketList));
                Report(("conn->bufferPkt:%08x",conn->bufferPkt));
                Report(("conn->bufferPkt->node:%08x",conn->bufferPkt->node));            
                InsertTailList(&conn->freePacketList,&(conn->bufferPkt->node));
                Report(("conn->bufferPkt:%08x",conn->bufferPkt));
                
                conn->bufferPkt = NULL;
                Report(("conn->bufferPkt:%08x",conn->bufferPkt));           
                Report(("BT_ATT_HandleRxReady(): opcode=%02x", conn->rxTempBuffer[0]));
                Report(("conn->bufferPkt:%08x",conn->bufferPkt));
                
                if (conn->rxTempBuffer[0] == BT_ATT_OPCODE_ERROR_RESPONSE)
                {
                    BT_ATT_HandleErrorResponse(conn, conn->rxTempBuffer+1, conn->rxTempLength-1);
                    conn->rxTempLength = 0;
                }
            }
        }
        else
        {
  
            Report(("SM Conn buffer packet:%08x",conn->bufferPkt));
            if(conn->bufferPkt !=0 )
            {
                pkt = conn->bufferPkt;
                conn->bufferPkt = NULL;
                pkt->headerLen = 0;
                pkt->tailLen = 0;
                BT_ATT_Send(link, pkt);
            }
        }
    }
}

void BT_ATT_HandleReturnSentPkt(BtAttConn *conn)
{
    if(conn->bufferPkt !=0)
        InsertTailList(&conn->freePacketList,&(conn->bufferPkt->node));
    conn->bufferPkt = NULL;
}
#endif   
