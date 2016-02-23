#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "att.h"
#include "gatt.h"
#include "gattcb.h"
#include "attdef.h"


extern BOOL AttUuidCmp(U8 *uuid1, U16 len1,U8 *uuid2, U16 len2);
extern void GATT_HandleOperation(BtRemoteDevice *link);
extern BtGattConn *BT_Gatt_GetConCtx(BtRemoteDevice *link);

void GattSetOpTypeNull(BtGattOp *op)
{
    op->op.opType = 0;
}

void GattHandleExchangeMTUOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtATTExchangeMTUResponseStruct *ptr;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);

    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0)
    {
		return;
    }

    switch (event)
    {
        case BT_GATT_OP_START:
               BT_ATT_ForGattExchangeMTURequest(link);
            return;
        case BT_GATT_OP_LINK_DISCONNECTED:
        	conn->gattRequestCurOp = 0;            
            return;
        case BT_GATT_OP_END:
        case BT_GATT_OP_ERROR:
            ptr = (BtATTExchangeMTUResponseStruct *)conn->attEvent->parms;
            conn->att_mtu = ptr->serverRxMtu;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            break;
    }
	conn->gattRequestCurOp = 0;
    conn->gattRequestHandler = 0;
    GATT_HandleOperation(link);
}

void GattHandleDiscoveryAllPrimaryServiceOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattDiscoveryAllPrimaryService *parms;
	BtATTReadByGroupTypeRequestStruct input_data;
	BtATTReadByGroupTypeResponseStruct *ptr;
	BtGattDiscoveryAllPrimaryServiceResultEvent gatt_event;
	U16 remainLength = 0;
	U16 offset = 0;
	
    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
    if (op->apCb ==0)
    {
		return;
    }
	parms = (BtGattDiscoveryAllPrimaryService *)op->parms.p.allPrimaryService;
    switch (event)
    {
        case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle;			
			input_data.attributeType2 = BT_UUID_GATT_TYPE_PRIMARY_SERVICE;
			input_data.type = 0x02;
			BT_ATT_SendReadByGroupTypeRequest(&input_data, link);
            return;
        case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);			
			conn->gattRequestCurOp = 0;
            return;
        case BT_GATT_OP_END:
			ptr = (BtATTReadByGroupTypeResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->length));
			remainLength = ptr->attributeDataListLen;

			switch (ptr->length)
			{
				case 0x06:
					if((ptr->attributeDataListLen % 0x06) !=0)
						remainLength = 0;
					break;
				case 0x14:
					if((ptr->attributeDataListLen % 0x14) !=0)
						remainLength = 0;
					break;
				default:
					remainLength = 0;
					/* Error, UUID response shall be 2 bytes or 16 bytes */
					break;
			}
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE;

			if(remainLength ==0)
			{
				gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done with error*/
                gatt_event.error_code = BT_ATT_ERROR_CODE_INVALID_PDU;
				op->apCb( op, &conn->gattEvent);
				break;
			}
			while(remainLength !=0)
			{
				gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
				gatt_event.startHandle = LEtoHost16(ptr->attributeDataList+offset);
				gatt_event.endHandle = LEtoHost16(ptr->attributeDataList+2+offset);
				if(ptr->length == 0x06)
				{
					gatt_event.type = BT_UUID16_SIZE;
					gatt_event.uuid2 = LEtoHost16(ptr->attributeDataList+4+offset);
					remainLength -= 6;
					offset+=6;
				}
				else
				{
					gatt_event.type = BT_UUID128_SIZE;
					gatt_event.uuid16 = ptr->attributeDataList+offset+4;
					remainLength -= 20;
					offset+=20;
				}
				op->apCb( op, &conn->gattEvent);
			}
			if((gatt_event.endHandle != 0xFFFF) && ((gatt_event.endHandle+1) < parms->endHandle))
			{
				
				/* Try to get the next one */
				input_data.startingHandle = gatt_event.endHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				input_data.attributeType2 = BT_UUID_GATT_TYPE_PRIMARY_SERVICE;
				input_data.type = 0x02;
				BT_ATT_SendReadByGroupTypeRequest(&input_data, link);			
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
			}
            break;
        case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_PRIMARY_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            break;
    }
    GATT_HandleOperation(link);

}

void GattHandleDiscoveryPrimaryServiceByUUIDOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattDiscoveryPrimaryServiceByUUID *parms;
	BtATTFindByTypeValueRequestStruct input_data;
	BtATTFindByTypeValueResponseStruct *ptr;
	BtGattDiscoveryPrimaryServiceByUUIDResultEvent gatt_event;
	U16 offset =0;
    
    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
    if (op->apCb ==0)
    {
		return;
    }
	parms = (BtGattDiscoveryPrimaryServiceByUUID *)op->parms.p.primaryServiceByUUID;
    switch (event)
    {
        case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle;			
			input_data.attributeType = BT_UUID_GATT_TYPE_PRIMARY_SERVICE;
			input_data.attributeValueLen = parms->type;
			if(parms->type == 0x02)
			{
				input_data.attributeValue = parms->uuid16;
			}
			else
			{
				input_data.attributeValue = parms->uuid128;
			}
			BT_ATT_SendFindByTypeValueRequest(&input_data, link);
            return;
        case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);			
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            return;
        case BT_GATT_OP_END:
			ptr = (BtATTFindByTypeValueResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->handleInfoListLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID;

            if(ptr->handleInfoListLen %4  != 0)
            {
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
                gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done with error*/
                gatt_event.error_code = BT_ATT_ERROR_CODE_INVALID_PDU;
                op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
                break;
            }

            while(offset != ptr->handleInfoListLen)
            {
                conn->gattEvent.link = link;
                conn->gattEvent.parms = &gatt_event;
                conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID;
                gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
                gatt_event.startHandle = LEtoHost16(ptr->handleInfoList+offset);
                gatt_event.endHandle = LEtoHost16(ptr->handleInfoList+2+offset);
                Report(("[GATT]gatt_event.startHandle:%d",gatt_event.startHandle));
                Report(("[GATT]gatt_event.endHandle:%d",gatt_event.endHandle));               
                op->apCb( op, &conn->gattEvent);
                offset+=4;
            }
			if((gatt_event.endHandle != 0xFFFF) && ((gatt_event.endHandle+1) < parms->endHandle))
			{
				input_data.startingHandle = gatt_event.endHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				input_data.attributeType = BT_UUID_GATT_TYPE_PRIMARY_SERVICE;
				input_data.attributeValueLen = parms->type;
				if(parms->type == 0x02)
				{
					input_data.attributeValue = parms->uuid16;
				}
				else
				{
					input_data.attributeValue = parms->uuid128;
				}
				BT_ATT_SendFindByTypeValueRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
			}
            break;
        case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_PRIMARY_SERVICE_BY_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
    }
    GATT_HandleOperation(link);


}

void GattHandleFindIncludeServiceOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattFindIncludeServices *parms;
	BtATTReadByTypeRequestStruct input_data;
	BtATTReadByTypeResponseStruct *ptr;
	BtATTReadRequestStruct input_data1;
	BtATTReadResponseStruct *ptr1 = 0;
	BtGattFindIncludeServicesResultEvent gatt_event;
	U16 remain_length;
	U16 offset =0;
	
    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
    if (op->apCb ==0)
    {
        return;
    }
    Report(("GattHandleFindIncludeServiceOp event:%d",event));
    parms = (BtGattFindIncludeServices *)op->parms.p.findIncludeServices;
    switch (event)
    {
        case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle;			
			input_data.attributeType2 = BT_UUID_GATT_TYPE_INCLUDE;
			input_data.type = 0x02;
			BT_ATT_SendReadByTypeRequest(&input_data, link);
            return;
        case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);			
			conn->gattRequestCurOp = 0;
            return;
        case BT_GATT_OP_END:
			if(conn->attEvent->eType == BT_ATT_EVENT_READ_BY_TYPE_RESPONSE)
			{
				ptr = (BtATTReadByTypeResponseStruct *)conn->attEvent->parms;
				Report(("[GATT]The remote device response length:%d",ptr->handleDataListLen));
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
                                    remain_length = ptr->handleDataListLen;

				if((ptr->length % 0x08) == 0x00)
				{
					while (remain_length !=0)
					{
						gatt_event.attributeHandle = LEtoHost16(ptr->handleDataList+offset);
						gatt_event.includeServiceHandle = LEtoHost16(ptr->handleDataList+2+offset);
						gatt_event.endGroupHandle = LEtoHost16(ptr->handleDataList+4+offset);
						gatt_event.serviceUUID = LEtoHost16(ptr->handleDataList+6+offset);
						gatt_event.type = 0x02;
						Report(("[GATT]gatt_event.attributeHandle:%d",gatt_event.attributeHandle));
						Report(("[GATT]gatt_event.includeServiceHandle:%d",gatt_event.includeServiceHandle));				
						Report(("[GATT]gatt_event.endGroupHandle:%d",gatt_event.endGroupHandle));				
						Report(("[GATT]gatt_event.serviceUUID:%d",gatt_event.serviceUUID));	
						offset+=8;
						remain_length -= 8;
						op->apCb( op, &conn->gattEvent);
					}
				}
				else
				{
					if(remain_length != 0x06)
					{
						Report(("[GATT]Possible multiple include response"));
					}
					parms->tempData1 = LEtoHost16(ptr->handleDataList+offset);
					parms->tempData2 = LEtoHost16(ptr->handleDataList+2+offset);
					parms->tempData3 = LEtoHost16(ptr->handleDataList+4+offset);
					Report(("[GATT]parms->tempData1:%d",parms->tempData1));
					Report(("[GATT]parms->tempData2:%d",parms->tempData2));
					Report(("[GATT]parms->tempData3:%d",parms->tempData3));
					input_data1.handle = parms->tempData2;
					BT_ATT_SendReadRequest(&input_data1, link);
					return;
				}
				
			}
			else 
			{
				/* Read operation done */
				ptr1 = (BtATTReadResponseStruct *)conn->attEvent->parms;
				gatt_event.attributeHandle = parms->tempData1;
				gatt_event.includeServiceHandle = parms->tempData2;
				gatt_event.endGroupHandle = parms->tempData3;
				gatt_event.type = 0x10;
				gatt_event.serviceUUID16 = ptr1->attributeValue;
				op->apCb( op, &conn->gattEvent);
			}
			if((gatt_event.attributeHandle != 0xFFFF) && ((gatt_event.attributeHandle+1) < parms->endHandle))
			{
				input_data.startingHandle = gatt_event.attributeHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				input_data.attributeType2 = BT_UUID_GATT_TYPE_INCLUDE;
				input_data.type = 0x02;
				BT_ATT_SendReadByTypeRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
			}
            break;
        case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_FIND_INCLUDE_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
    }
    GATT_HandleOperation(link);
}

void GattHandleDiscoveryAllCharOfServiceOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattDiscoveryAllCharacteristicOfService *parms;
	BtATTReadByTypeRequestStruct input_data;
	BtATTReadByTypeResponseStruct *ptr;
	BtGattDiscoveryAllCharacteristicOfServiceResultEvent gatt_event;
	U16 remain_length;
	U16 offset =0;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattDiscoveryAllCharacteristicOfService *)op->parms.p.allCharOfServices;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle; 		
			input_data.attributeType2 = BT_UUID_GATT_TYPE_CHARACTERISTIC;
			input_data.type = 0x02;
			BT_ATT_SendReadByTypeRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadByTypeResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->handleDataListLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			if(ptr->length == 0x07)
			{
				remain_length = ptr->handleDataListLen;
				while (remain_length !=0)
				{
					gatt_event.attributeHandle = LEtoHost16(ptr->handleDataList+offset);
					gatt_event.charProperty = ptr->handleDataList[offset+2];
					gatt_event.charValueAttributeHandle = LEtoHost16(ptr->handleDataList+3+offset);
					gatt_event.charUUID = LEtoHost16(ptr->handleDataList+5+offset);
					gatt_event.type = 0x02;
					Report(("[GATT]gatt_event.charProperty:%d",gatt_event.charProperty));
					Report(("[GATT]gatt_event.charValueAttributeHandle:%d",gatt_event.charValueAttributeHandle)); 			
					Report(("[GATT]gatt_event.charUUID:%d",gatt_event.charUUID)); 			
					remain_length -=7;
					offset+=7;
					op->apCb( op, &conn->gattEvent);
				}
			}
			else if(ptr->length == 0x15)
			{
				remain_length = ptr->handleDataListLen;
				while (remain_length !=0)
				{
					gatt_event.attributeHandle = LEtoHost16(ptr->handleDataList+offset);
					gatt_event.charProperty = ptr->handleDataList[offset+2];
					gatt_event.charValueAttributeHandle = LEtoHost16(ptr->handleDataList+3+offset);
					gatt_event.charUUID16 = ptr->handleDataList+5+offset;
					gatt_event.type = 0x10;
					Report(("[GATT]gatt_event.charProperty:%d",gatt_event.charProperty));
					Report(("[GATT]gatt_event.charValueAttributeHandle:%d",gatt_event.charValueAttributeHandle)); 			
					Report(("[GATT]gatt_event.charUUID:%d",gatt_event.charUUID)); 			
					remain_length -= 0x15;
					offset += 0x15;
					op->apCb( op, &conn->gattEvent);
				}

			}
			else
			{
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done with error*/
                gatt_event.error_code = BT_ATT_ERROR_CODE_INVALID_PDU;
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
				break;
			}
			if((gatt_event.attributeHandle != 0xFFFF) && ((gatt_event.attributeHandle+1) < parms->endHandle))
			{
				input_data.startingHandle = gatt_event.attributeHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				input_data.attributeType2 = BT_UUID_GATT_TYPE_CHARACTERISTIC;
				input_data.type = 0x02;
				BT_ATT_SendReadByTypeRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
			}
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_OF_SERVICE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0; 
			break;
	}
	GATT_HandleOperation(link);
}

void GattHandleDiscoveryCharByUUIDOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattDiscoveryCharacteristicByUUID *parms;
	BtATTReadByTypeRequestStruct input_data;
	BtATTReadByTypeResponseStruct *ptr;
	BtGattDiscoveryCharacteristicByUUIDResultEvent gatt_event;
	U16 remain_length;
	U16 offset =0;
	U8 *tempUUID;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattDiscoveryCharacteristicByUUID *)op->parms.p.charByUUID;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle; 		
			input_data.attributeType2 = BT_UUID_GATT_TYPE_CHARACTERISTIC;
			input_data.type = 0x02;
			BT_ATT_SendReadByTypeRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadByTypeResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->handleDataListLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			if(parms->type == 0x02)
			{
				tempUUID = parms->uuid16;
			}
			else
			{
				tempUUID = parms->uuid128;
			}
			if(ptr->length == 0x07)
			{
				remain_length = ptr->handleDataListLen;
				while (remain_length !=0)
				{
					gatt_event.attributeHandle = LEtoHost16(ptr->handleDataList+offset);
					gatt_event.charProperty = ptr->handleDataList[offset+2];
					gatt_event.charValueAttributeHandle = LEtoHost16(ptr->handleDataList+3+offset);
					gatt_event.charUUID = LEtoHost16(ptr->handleDataList+5+offset);
					gatt_event.type = 0x02;
					if(AttUuidCmp(ptr->handleDataList+5+offset, 0x02,tempUUID, parms->type) == TRUE)
					{
						Report(("[GATT]gatt_event.charProperty:%d",gatt_event.charProperty));
						Report(("[GATT]gatt_event.charValueAttributeHandle:%d",gatt_event.charValueAttributeHandle)); 			
						Report(("[GATT]gatt_event.charUUID:%d",gatt_event.charUUID)); 			
						op->apCb( op, &conn->gattEvent);
					}
					else
					{
						Report(("[GATT]UUID is not match"));
					}
					remain_length -=7;
					offset+=7;
				}
			}
			else if(ptr->length == 0x15)
			{
				remain_length = ptr->handleDataListLen;
				while (remain_length !=0)
				{
					gatt_event.attributeHandle = LEtoHost16(ptr->handleDataList+offset);
					gatt_event.charProperty = ptr->handleDataList[offset+2];
					gatt_event.charValueAttributeHandle = LEtoHost16(ptr->handleDataList+3+offset);
					gatt_event.charUUID16 = (U8 *)(ptr->handleDataList+5+offset);
					gatt_event.type = 0x10;
					if(AttUuidCmp(ptr->handleDataList+5+offset, 0x02, tempUUID, parms->type) == TRUE)
					{
						Report(("[GATT]gatt_event.charProperty:%d",gatt_event.charProperty));
						Report(("[GATT]gatt_event.charValueAttributeHandle:%d",gatt_event.charValueAttributeHandle)); 			
						Report(("[GATT]gatt_event.charUUID:%d",gatt_event.charUUID)); 			
						op->apCb( op, &conn->gattEvent);
					}
					else
					{
						Report(("[GATT]UUID is not match"));
					}
					remain_length -= 0x15;
					offset+= 0x15;
				}

			} 
			else
			{
				gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done with error*/
                gatt_event.error_code = BT_ATT_ERROR_CODE_INVALID_PDU;
				op->apCb( op, &conn->gattEvent);
				break;
			}
			if((gatt_event.attributeHandle != 0xFFFF) && ((gatt_event.attributeHandle+1) < parms->endHandle))
			{
				input_data.startingHandle = gatt_event.attributeHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				input_data.attributeType2 = BT_UUID_GATT_TYPE_CHARACTERISTIC;
				input_data.type = 0x02;
				BT_ATT_SendReadByTypeRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;         
			}
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_CHAR_BY_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			break;
	}
	GATT_HandleOperation(link);
}


void GattHandleDiscoveryAllCharDescriptorOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattDiscoveryAllCharacteristicDescriptors *parms;
	BtATTFindInfoRequestStruct input_data;
	BtATTFindInfoResponseStruct *ptr;
	BtGattDiscoveryAllCharacteristicDescriptorsResultEvent gatt_event;
	U16 remain_length;
	U16 offset =0;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattDiscoveryAllCharacteristicDescriptors *)op->parms.p.allCharDescriptors;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle; 		
			BT_ATT_SendFindInfoRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTFindInfoResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->infoDataLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR;
			gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			if(ptr->format == 0x01)
			{
				remain_length = ptr->infoDataLen;
				while (remain_length !=0)
				{
					gatt_event.attributeHandle = LEtoHost16(ptr->infoData+offset);
					gatt_event.uuid16 = LEtoHost16(ptr->infoData+offset+2);
					gatt_event.type = 0x02;
					Report(("[GATT]gatt_event.attributeHandle:%d",gatt_event.attributeHandle));
					Report(("[GATT]gatt_event.uuid16:%d",gatt_event.uuid16)); 			
					Report(("[GATT]gatt_event.type:%d",gatt_event.type)); 			
					op->apCb( op, &conn->gattEvent);
					remain_length -=4;
					offset+=4;
				}
			}
			else if(ptr->format == 0x02)
			{
				remain_length = ptr->infoDataLen;
				while (remain_length !=0)
				{
					gatt_event.attributeHandle = LEtoHost16(ptr->infoData+offset);
					gatt_event.uuid128 = ptr->infoData+offset+2;
					gatt_event.type = 0x10;
					Report(("[GATT]gatt_event.attributeHandle:%d",gatt_event.attributeHandle));
					Report(("[GATT]gatt_event.type:%d",gatt_event.type)); 			
					op->apCb( op, &conn->gattEvent);
					remain_length -= 0x12;
					offset+= 0x12;
				}
			}
			else
			{
				gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done with error*/
                gatt_event.error_code = BT_ATT_ERROR_CODE_INVALID_PDU;
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
				break;
			}
			if((gatt_event.attributeHandle != 0xFFFF) && ((gatt_event.attributeHandle+1) < parms->endHandle))
			{
				input_data.startingHandle = gatt_event.attributeHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				BT_ATT_SendFindInfoRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;
			}
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_DISCOVERY_ALL_CHAR_DESCRIPTOR;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
	}
	GATT_HandleOperation(link);
}

void GattHandleReadCharValueOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadCharacteristicValue *parms;
	BtATTReadRequestStruct input_data;
	BtATTReadResponseStruct *ptr;
	BtGattReadCharacteristicValueResultEvent gatt_event;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattReadCharacteristicValue *)op->parms.p.readCharValue;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.handle = parms->handle;
			BT_ATT_SendReadRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->attributeValueLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
			gatt_event.data = ptr->attributeValue;
			gatt_event.length = ptr->attributeValueLen;
            gatt_event.error_code = 0;
			Report(("[GATT]gatt_event.length:%d",gatt_event.length));
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			break;
	}
	GATT_HandleOperation(link);

}

void GattHandleReadUsingCharUUIDOp(U8  event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadCharacteristicUUID *parms;
	BtATTReadByTypeRequestStruct input_data;
	BtATTReadByTypeResponseStruct *ptr;
	BtGattReadCharacteristicUUIDResultEvent gatt_event;
	U16 remain_length;
	U16 offset =0;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattReadCharacteristicUUID *)op->parms.p.readCharUUID;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle; 
			if(parms->type == 0x02)
			{
				input_data.attributeType2 = parms->uuid16;
				input_data.type = 0x02;
			}
			else
			{
				input_data.attributeType = parms->uuid128;
				input_data.type = 0x10;
			}
			BT_ATT_SendReadByTypeRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_USING_CHAR_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadByTypeResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->handleDataListLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_USING_CHAR_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			remain_length = ptr->handleDataListLen;
			while (remain_length !=0)
			{
				gatt_event.handle = LEtoHost16(ptr->handleDataList+offset);
				gatt_event.data = ptr->handleDataList + offset+2;
				gatt_event.length = ptr->length-2;
				Report(("[GATT]gatt_event.handle:%d",gatt_event.handle));
				Report(("[GATT]gatt_event.length:%d",gatt_event.length)); 			
				remain_length -= ptr->length;
				offset += ptr->length;
				op->apCb( op, &conn->gattEvent);
			}
            gatt_event.data = 0;
            gatt_event.length = 0;
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_USING_CHAR_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_USING_CHAR_UUID;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));            
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
	}
	GATT_HandleOperation(link);
}


void GattHandleReadLongCharValueOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadLongCharacteristicValue *parms;
	BtATTReadBlobRequestStruct input_data;
	BtATTReadBlobResponseStruct *ptr;
	BtGattReadLongCharacteristicValueResultEvent gatt_event;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattReadLongCharacteristicValue *)op->parms.p.readLongCharValue;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.attributeHandle= parms->handle;
			input_data.valueOffset = parms->offset;
			BT_ATT_SendReadBlobRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadBlobResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->partAttributeValueLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			gatt_event.data = ptr->partAttributeValue;
			gatt_event.length = ptr->partAttributeValueLen;
			Report(("[GATT]gatt_event.length:%d",gatt_event.length));
			op->apCb( op, &conn->gattEvent);
			parms->offset += ptr->partAttributeValueLen;
			Report(("[GATT]parms->offset:%d",parms->offset));
			Report(("[GATT]conn->att_mtu:%d",conn->att_mtu));			
			if(ptr->partAttributeValueLen == (conn->att_mtu -1))
			{
				input_data.attributeHandle = parms->handle;
				input_data.valueOffset = parms->offset;
				BT_ATT_SendReadBlobRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
                gatt_event.error_code = 0;
				Report(("[GATT]gatt_event.length:%d",gatt_event.length));
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);			
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;
			}
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));            
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
	}
	GATT_HandleOperation(link);

}

void GattHandleReadMultipleCharValueOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadMultipleCharacteristicValue *parms;
	BtATTReadMultipleRequestStruct input_data;
	BtATTReadMultipleResponseStruct *ptr;
	BtGattReadMultipleCharacteristicValueResultEvent gatt_event;
	U8 tempBuffer[2*7];
	U8 i;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattReadMultipleCharacteristicValue *)op->parms.p.readMultipleCharValue;
	switch (event)
	{
		case BT_GATT_OP_START:
		    if(parms->handleSetNo > 7)
				Assert(0);
		    if(parms->handleSetNo == 1)
				Assert(0);			
			for(i=0; i< parms->handleSetNo; i++)
			{
				StoreLE16(tempBuffer+i*2, parms->handle[i]);
			}
			input_data.setOfHandlesLen = parms->handleSetNo * 2;
			input_data.setOfHandles = tempBuffer;			
			BT_ATT_SendReadMultipleRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadMultipleResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->setOfValuesLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
			gatt_event.data = ptr->setOfValues;
			gatt_event.length = ptr->setOfValuesLen;
            gatt_event.error_code = 0;
			Report(("[GATT]gatt_event.length:%d",gatt_event.length));
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_MULTIPLE_CHAR_VALUE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));              
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
	}
	GATT_HandleOperation(link);


}

void GattHandleWriteCharValueOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattWriteCharValueRequest *parms;
    BtATTWriteRequestStruct input_data;
    BtGattWriteCharValueResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattWriteCharValueRequest *)op->parms.p.writeRequest;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.attributeValueLen= parms->size;		
            input_data.attributeValue = parms->data;					
            BT_ATT_SendWriteRequest(&input_data, link);
            return;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_CHAR_VALUE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattRequestCurOp = 0;
            return;
		case BT_GATT_OP_END:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_CHAR_VALUE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
        case BT_GATT_OP_ERROR:
            /* Operation done */
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_CHAR_VALUE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));              
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            break;
        }
    GATT_HandleOperation(link);
}

void GattHandleWriteLongCharValueOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattWriteLongCharacteristicValues *parms;
    BtATTPrepareWriteRequestStruct input_data;
    BtATTPrepareWriteResponseStruct *ptr;
    BtATTExecuteWriteRequestStruct input_data1;
    BtGattWriteLongCharValueResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattWriteLongCharacteristicValues *)op->parms.p.writeLongCharValues;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.valueOffset = parms->offset;
            if(parms->size > conn->att_mtu - 5)
            {
                input_data.partAttributeValueLen = conn->att_mtu - 5;                    
            }
            else
            {
                input_data.partAttributeValueLen = parms->size;                    
            }
            input_data.partAttributeValue = parms->data + parms->offset;
            BT_ATT_SendPrepareWriteRequest(&input_data, link);
            return;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            return;
		case BT_GATT_OP_END:
            if(conn->attEvent->eType == BT_ATT_EVENT_PREPARE_WRITE_RESPONSE)
            {
                ptr = (BtATTPrepareWriteResponseStruct*)conn->attEvent->parms;
                parms->offset += ptr->partAttributeValueLen;
                /* Not necessary to check the response value data correct*/
                if(parms->offset < parms->size)
                {
                    input_data.attributeHandle = parms->handle;
                    input_data.valueOffset = parms->offset;
                    if((parms->size - parms->offset) > conn->att_mtu - 5)
                    {
                        input_data.partAttributeValueLen = conn->att_mtu - 5;                    
                    }
                    else
                    {
                        input_data.partAttributeValueLen = parms->size - parms->offset;                    
                    }
                    input_data.partAttributeValue = parms->data + parms->offset;
                    BT_ATT_SendPrepareWriteRequest(&input_data, link);
                    return;                    
                }
                else
                {
                    input_data1.flags = 0x01;
                    BT_ATT_SendExecuteWriteRequest(&input_data1, link);
                    return;                    
                }
            }
            else
            {
                conn->gattEvent.link = link;
                conn->gattEvent.parms = &gatt_event;
                conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE;
                gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
                gatt_event.error_code = 0;
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
                op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
            }
            break;
        case BT_GATT_OP_ERROR:
            /* Operation done */
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_LONG_CHAR_VALUE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));                
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            break;
        }
    GATT_HandleOperation(link);

}

void GattHandleReliableWriteOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattReliableWrite *parms;
    BtATTPrepareWriteRequestStruct input_data;
    BtATTPrepareWriteResponseStruct *ptr;
    BtATTExecuteWriteRequestStruct input_data1;
    BtGattReliableWriteResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0){
		return;
    }
    if (op->apCb ==0){
		return;
    }
    parms = (BtGattReliableWrite *)op->parms.p.reliableWrite;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.valueOffset = parms->offset;
        if(parms->size > conn->att_mtu - 5){
                input_data.partAttributeValueLen = conn->att_mtu - 5;                    
        }else{
                input_data.partAttributeValueLen = parms->size;                    
            }
        //parms->excuteFlag = 0;
        parms->canceled = 0;
        parms->curr_offset = input_data.valueOffset;
        input_data.partAttributeValue = parms->data + parms->curr_offset;
            BT_ATT_SendPrepareWriteRequest(&input_data, link);
            return;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_RELIABLEWRITES;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            return;
		case BT_GATT_OP_END:
            if(conn->attEvent->eType == BT_ATT_EVENT_PREPARE_WRITE_RESPONSE)
            {
                ptr = (BtATTPrepareWriteResponseStruct*)conn->attEvent->parms;
            if((ptr->valueOffset != parms->curr_offset) ||
                    (OS_MemCmp(ptr->partAttributeValue ,ptr->partAttributeValueLen,
                    parms->data+parms->curr_offset, ptr->partAttributeValueLen) == FALSE))
                {
                    /* Flush old data   */
                    input_data1.flags = 0x00;
                parms->canceled = 1;
                    BT_ATT_SendExecuteWriteRequest(&input_data1, link);
                    return;
                }
            parms->curr_offset += ptr->partAttributeValueLen;
            if(parms->curr_offset < parms->size){
                    input_data.attributeHandle = parms->handle;
                    input_data.valueOffset = parms->offset;
                if((parms->size - parms->curr_offset) > conn->att_mtu - 5){
                        input_data.partAttributeValueLen = conn->att_mtu - 5;                    
                }else{
                    input_data.partAttributeValueLen = parms->size - parms->curr_offset;
                    }
                input_data.partAttributeValue = parms->data + parms->curr_offset;
                    BT_ATT_SendPrepareWriteRequest(&input_data, link);
                    return;                    
            }else if(parms->excuteFlag != 0){
                /* Request require to execute */
                    input_data1.flags = 0x01;
                //parms->excuteFlag = 1;
                    BT_ATT_SendExecuteWriteRequest(&input_data1, link);
                    return;
            }else{
                /* Complete without execute */
                conn->gattEvent.link = link;
                conn->gattEvent.parms = &gatt_event;
                conn->gattEvent.eType = BT_GATT_OPERATOR_RELIABLEWRITES;
                gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
                gatt_event.executed = 0;
                gatt_event.error_code = 0;
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
                op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;
                }
        }else{
                conn->gattEvent.link = link;
                conn->gattEvent.parms = &gatt_event;
                conn->gattEvent.eType = BT_GATT_OPERATOR_RELIABLEWRITES;
            if(parms->excuteFlag == 1 && parms->canceled == 0){
                gatt_event.executed = 1;
                    gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
            }else{
                    gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event */
                }
                gatt_event.error_code = 0;
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
                op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;
            }
            break;
        case BT_GATT_OP_ERROR:
            /* Operation done */
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_RELIABLEWRITES;
            gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));                
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
        }
    GATT_HandleOperation(link);
}

void GattHandleExecWriteOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattExecWrite *parms;
    BtATTExecuteWriteRequestStruct input_data;
    BtGattExecWriteResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0){
		return;
    }
    if (op->apCb ==0){
		return;
    }
    parms = (BtGattExecWrite *)op->parms.p.execWrite;
    switch (event)
    {
    case BT_GATT_OP_START:
        input_data.flags = parms->execute;
        BT_ATT_SendExecuteWriteRequest(&input_data, link);
        return;
    case BT_GATT_OP_LINK_DISCONNECTED:
        conn->gattEvent.link = link;
        conn->gattEvent.parms = &gatt_event;
        conn->gattEvent.eType = BT_GATT_OPERATOR_EXEC_WRITES;
        gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
        GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
        op->apCb( op, &conn->gattEvent); 		
        conn->gattRequestCurOp = 0;
        return;
    case BT_GATT_OP_END:
        conn->gattEvent.link = link;
        conn->gattEvent.parms = &gatt_event;
        conn->gattEvent.eType = BT_GATT_OPERATOR_EXEC_WRITES;
        gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
        gatt_event.executed = parms->execute;
        gatt_event.error_code = 0;
        GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
        op->apCb( op, &conn->gattEvent);
        conn->gattRequestCurOp = 0;
        conn->gattRequestHandler = 0;            
        break;
    case BT_GATT_OP_ERROR:
        /* Operation done */
        conn->gattEvent.link = link;
        conn->gattEvent.parms = &gatt_event;
        conn->gattEvent.eType = BT_GATT_OPERATOR_EXEC_WRITES;
        gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
        gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
        Report(("Error response:%d",gatt_event.error_code));              
        GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
        op->apCb( op, &conn->gattEvent);
        conn->gattRequestCurOp = 0;
        conn->gattRequestHandler = 0;
        break;
    }
    GATT_HandleOperation(link);
}


void GattHandleReadCharDescriptorOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadCharacteristicDescriptors *parms;
	BtATTReadRequestStruct input_data;
	BtATTReadResponseStruct *ptr;
	BtGattReadCharacteristicDescriptorsResultEvent gatt_event;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattReadCharacteristicDescriptors *)op->parms.p.readCharDescriptors;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.handle = parms->handle;
			BT_ATT_SendReadRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->attributeValueLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
			gatt_event.data = ptr->attributeValue;
			gatt_event.length = ptr->attributeValueLen;
			Report(("[GATT]gatt_event.length:%d",gatt_event.length));
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_CHAR_DESCRIPTORS;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));             
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
	}
	GATT_HandleOperation(link);

}

void GattHandleReadLongCharDescriptorOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadLongCharacteristicValue *parms;
	BtATTReadBlobRequestStruct input_data;
	BtATTReadBlobResponseStruct *ptr;
	BtGattReadLongCharacteristicValueResultEvent gatt_event;
	
	conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
	if (op->apCb ==0)
	{
		return;
	}
	parms = (BtGattReadLongCharacteristicValue *)op->parms.p.readLongCharValue;
	switch (event)
	{
		case BT_GATT_OP_START:
			input_data.attributeHandle= parms->handle;
			input_data.valueOffset = parms->offset;
			parms->offset = 0x00;
			BT_ATT_SendReadBlobRequest(&input_data, link);
			return;
		case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent); 		
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			return;
		case BT_GATT_OP_END:
			ptr = (BtATTReadBlobResponseStruct *)conn->attEvent->parms;
			Report(("[GATT]The remote device response length:%d",ptr->partAttributeValueLen));
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS;
			gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			gatt_event.data = ptr->partAttributeValue;
			gatt_event.length = ptr->partAttributeValueLen;
			Report(("[GATT]gatt_event.length:%d",gatt_event.length));
			op->apCb( op, &conn->gattEvent);
			parms->offset += ptr->partAttributeValueLen;
			Report(("[GATT]parms->offset:%d",parms->offset));
			Report(("[GATT]conn->att_mtu:%d",conn->att_mtu));			
			if(ptr->partAttributeValueLen == (conn->att_mtu -1))
			{
				input_data.attributeHandle = parms->handle;
				input_data.valueOffset = parms->offset;
				BT_ATT_SendReadBlobRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
                gatt_event.error_code = 0;
				Report(("[GATT]gatt_event.length:%d",gatt_event.length));
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);			
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
			}
			break;
		case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_LONG_CHAR_DESCRIPTORS;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));              
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
			break;
	}
	GATT_HandleOperation(link);

}

void GattHandleWriteCharDescriptorOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattWriteDescriptors *parms;
    BtATTWriteRequestStruct input_data;
    BtGattWriteCharDescriptorResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattWriteDescriptors *)op->parms.p.writeDescriptors;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.attributeValueLen= parms->size;		
            input_data.attributeValue = parms->data;					
            BT_ATT_SendWriteRequest(&input_data, link);
            return;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            return;
		case BT_GATT_OP_END:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            break;
        case BT_GATT_OP_ERROR:
            /* Operation done */
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_CHAR_DESCRIPTORS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));              
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
        }
    GATT_HandleOperation(link);


}

void GattHandleWriteLongCharDescriptorOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattWriteLongDescriptors *parms;
    BtATTPrepareWriteRequestStruct input_data;
    BtATTPrepareWriteResponseStruct *ptr;
    BtATTExecuteWriteRequestStruct input_data1;
    BtGattWriteLongCharDescriptorResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattRequestCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattWriteLongDescriptors *)op->parms.p.writeLongDescriptors;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.valueOffset = parms->offset;
            if(parms->size > conn->att_mtu - 5)
            {
                input_data.partAttributeValueLen = conn->att_mtu - 5;                    
            }
            else
            {
                input_data.partAttributeValueLen = parms->size;                    
            }
            input_data.partAttributeValue = parms->data + parms->offset;
            BT_ATT_SendPrepareWriteRequest(&input_data, link);
            return;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            return;
		case BT_GATT_OP_END:
            if(conn->attEvent->eType == BT_ATT_EVENT_PREPARE_WRITE_RESPONSE)
            {
                ptr = (BtATTPrepareWriteResponseStruct*)conn->attEvent->parms;
                parms->offset += ptr->partAttributeValueLen;
                /* Not necessary to check the response value data correct*/
                if(parms->offset < parms->size)
                {
                    input_data.attributeHandle = parms->handle;
                    input_data.valueOffset = parms->offset;
                    if((parms->size - parms->offset) > conn->att_mtu - 5)
                    {
                        input_data.partAttributeValueLen = conn->att_mtu - 5;                    
                    }
                    else
                    {
                        input_data.partAttributeValueLen = parms->size - parms->offset;                    
                    }                    
                    input_data.partAttributeValue = parms->data + parms->offset;
                    BT_ATT_SendPrepareWriteRequest(&input_data, link);
                    return;                    
                }
                else
                {
                    input_data1.flags = 0x01;
                    BT_ATT_SendExecuteWriteRequest(&input_data1, link);
                    return;                    
                }
            }
            else
            {
                conn->gattEvent.link = link;
                conn->gattEvent.parms = &gatt_event;
                conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS;
                gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
                gatt_event.error_code = 0;
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
                op->apCb( op, &conn->gattEvent);
            }
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;
            break;
        case BT_GATT_OP_ERROR:
            /* Operation done */
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_LONG_CHAR_DESCRIPTORS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            gatt_event.error_code = ((BtATTErrorResponseStruct *)(conn->attEvent->parms))->errorCode;
            Report(("Error response:%d",gatt_event.error_code));             
            op->apCb( op, &conn->gattEvent);
            conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
        }
    GATT_HandleOperation(link);

}

void GattHandleNotificationOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattValueNotifications *parms;
    BtATTValueNotificationStruct input_data;
    BtGattValueNotificationResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattNotificationCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattValueNotifications *)op->parms.p.valueNotifications;
    switch (event)
    {
	case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.attributeValueLen= parms->size;		
            input_data.attributeValue = parms->data;					
            BT_ATT_SendHandleValueNotification(&input_data, link);
            return;
        case BT_GATT_OP_END:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_NOTIFICATIONS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event error*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattNotificationCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattNotificationCurOp = 0;
            conn->gattNotificationHandler = 0;
            break;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_NOTIFICATIONS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattNotificationCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattNotificationCurOp = 0;
            conn->gattNotificationHandler = 0;
            return;
        }
    GATT_HandleOperation(link);

}

void GattHandleWriteWithoutResponseOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattWriteWithoutResponse *parms;
    BtATTWriteCommandStruct input_data;
    BtGattWriteWithoutResponseResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattNotificationCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattWriteWithoutResponse *)op->parms.p.writeWithoutResponse;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.attributeValueLen= parms->size;		
            input_data.attributeValue = parms->data;					
            BT_ATT_SendWriteCommand(&input_data, link);
            return;
        case BT_GATT_OP_END:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event error*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattNotificationCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattNotificationCurOp = 0;
            conn->gattNotificationHandler = 0;
            break;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_WRITE_WITHOUT_RESPONSE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattNotificationCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattNotificationCurOp = 0;
            conn->gattNotificationHandler = 0;            
            return;
        }
    GATT_HandleOperation(link);


}

void GattHandleSignedWriteWithoutResponseOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattSignedWriteWithoutResponse *parms;
    BtATTSignedWriteCommandStruct input_data;
    BtGattSignedWriteWithoutResponseResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattNotificationCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattSignedWriteWithoutResponse *)op->parms.p.signedWriteWithoutResponse;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.attributeValueLen= parms->size;		
            input_data.attributeValue = parms->data;	
            input_data.authenticationSignatureExists = 1;
            BT_ATT_SendSignedWriteCommand(&input_data, link);
            return;
        case BT_GATT_OP_END:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event error*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattNotificationCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattNotificationCurOp = 0;
            conn->gattNotificationHandler = 0;            
            break;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_SIGNED_WRITE_WITHOUT_RESPONSE;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattNotificationCurOp);
            op->apCb( op, &conn->gattEvent); 		
            conn->gattNotificationCurOp = 0;
            conn->gattNotificationHandler = 0;            
            return;
        }
    GATT_HandleOperation(link);

}

void GattHandleIndicationsOp(U8 event, BtRemoteDevice *link)
{
    BtGattConn *conn;
    BtGattOp *op;
    BtGattValueIndications *parms;
    BtATTValueIndicationStruct input_data;
    BtGattValueIndicationsResultEvent gatt_event;

    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
    op = (BtGattOp*) conn->gattIndicationCurOp;	
    if(op == 0)
    {
		return;
    }
    if (op->apCb ==0)
    {
		return;
    }
    parms = (BtGattValueIndications *)op->parms.p.valueIndications;
    switch (event)
    {
		case BT_GATT_OP_START:
            input_data.attributeHandle = parms->handle;
            input_data.attributeValueLen= parms->size;		
            input_data.attributeValue = parms->data;					
            BT_ATT_SendHandleValueIndication(&input_data, link);
            return;
		case BT_GATT_OP_LINK_DISCONNECTED:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_INDICATIONS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattIndicationCurOp);
            
            op->apCb( op, &conn->gattEvent); 		
            conn->gattIndicationCurOp = 0;
            conn->gattIndicationHandler = 0;            
            return;
		case BT_GATT_OP_END:
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_INDICATIONS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event */
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattIndicationCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattIndicationCurOp = 0;
            conn->gattIndicationHandler = 0;                        
            break;
        case BT_GATT_OP_ERROR:
            /* Operation done */
            conn->gattEvent.link = link;
            conn->gattEvent.parms = &gatt_event;
            conn->gattEvent.eType = BT_GATT_OPERATOR_INDICATIONS;
            gatt_event.flag = BT_GATT_EVENT_RESULT_ERROR; /* Report event done*/
            GattSetOpTypeNull((BtGattOp *)conn->gattIndicationCurOp);
            op->apCb( op, &conn->gattEvent);
            conn->gattIndicationCurOp = 0;
            conn->gattIndicationHandler = 0;                        
            break;
        }
    GATT_HandleOperation(link);

}


void GattHandleReadByTypeOp(U8 event, BtRemoteDevice *link)
{
	BtGattConn *conn;
	BtGattOp *op;
	BtGattReadByType *parms;
	BtATTReadByTypeRequestStruct input_data;
	BtATTReadByTypeResponseStruct *ptr;
	BtGattReadByTypeResultEvent gatt_event;
	U16 remain_length;
	U16 offset =0;
	
    conn = (BtGattConn *)BT_Gatt_GetConCtx(link);
	op = (BtGattOp*) conn->gattRequestCurOp;	
	if(op == 0)
	{
		return;
	}
    if (op->apCb ==0)
    {
		return;
    }
	parms = (BtGattReadByType *)op->parms.p.readByType;
    switch (event)
    {
        case BT_GATT_OP_START:
			input_data.startingHandle = parms->startHandle;
			input_data.endingHandle = parms->endHandle;			
			input_data.attributeType2 = parms->uuid16;
			input_data.type = 0x02;
			BT_ATT_SendReadByTypeRequest(&input_data, link);
            return;
        case BT_GATT_OP_LINK_DISCONNECTED:
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_BY_TYPE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DISCONNECT; /* Report event error*/
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);			
			conn->gattRequestCurOp = 0;
            return;
        case BT_GATT_OP_END:
			if(conn->attEvent->eType == BT_ATT_EVENT_READ_BY_TYPE_RESPONSE)
			{
				ptr = (BtATTReadByTypeResponseStruct *)conn->attEvent->parms;
				Report(("[GATT]The remote device response length:%d",ptr->handleDataListLen));
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_READ_BY_TYPE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_CONTINUE_DATA; /* Report event */
			}
                            else
                            {
                                Report(("[GATT]read event type code is error"));
                                return;
                            }
			remain_length = ptr->handleDataListLen;
			while (remain_length !=0)
			{
				gatt_event.attributeHandle = LEtoHost16(ptr->handleDataList+offset);
				offset += ptr->length;
				remain_length -= ptr->length;
			}

			if(gatt_event.attributeHandle != 0xFFFF)
			{
				input_data.startingHandle = gatt_event.attributeHandle+1;
				input_data.endingHandle = parms->endHandle; 		
				input_data.attributeType2 = parms->uuid16;
				input_data.type = 0x02;
				BT_ATT_SendReadByTypeRequest(&input_data, link);
				return;
			}
			else
			{
				conn->gattEvent.link = link;
				conn->gattEvent.parms = &gatt_event;
				conn->gattEvent.eType = BT_GATT_OPERATOR_READ_BY_TYPE;
				gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
                GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
				op->apCb( op, &conn->gattEvent);
                conn->gattRequestCurOp = 0;
                conn->gattRequestHandler = 0;                
			}
            break;
        case BT_GATT_OP_ERROR:
			/* Operation done */
			conn->gattEvent.link = link;
			conn->gattEvent.parms = &gatt_event;
			conn->gattEvent.eType = BT_GATT_OPERATOR_READ_BY_TYPE;
			gatt_event.flag = BT_GATT_EVENT_RESULT_DONE_COMPLETED; /* Report event done*/
            gatt_event.error_code = 0;
            GattSetOpTypeNull((BtGattOp *)conn->gattRequestCurOp);
			op->apCb( op, &conn->gattEvent);
			conn->gattRequestCurOp = 0;
            conn->gattRequestHandler = 0;            
            break;
	}
    GATT_HandleOperation(link);
}

