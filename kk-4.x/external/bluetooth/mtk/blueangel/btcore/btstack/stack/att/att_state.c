#include "bt_common.h"
#include "btalloc.h"
#include "bt_feature.h"
#include "att.h"
#include "utils.h"

#define __BT_ATT_INITIATOR__

#if ATT_PROTOCOL == XA_ENABLED
BtHandler attAuthenHandler;
BtHandler attGlobalHandler;

#define SEC_ATT_ID (BtProtocolId)AttSecCallback

void AttSecCallback(const BtEvent *Event);

extern BtAttConn *BT_ATT_GetConCtx(BtRemoteDevice *link);
extern void L2CAP_LinkConnectedByCid(BtRemoteDevice *Link, U16 cid);


void BT_ATT_RUN_Dlight_Test(BtRemoteDevice *link);

void AttMeCallback(const BtEvent *Event)
{
	BtAttConn *conn;

    switch (Event->eType)
    {
        case BTEVENT_AUTHENTICATE_CNF:
	    case BTEVENT_AUTHENTICATED:
			conn = BT_ATT_GetConCtx(Event->p.remDev);
			if(conn !=NULL)
				BT_ATT_ReportEvent(BT_ATT_EVENT_CONNECTED_SECURITY_CHANGE, conn, NULL);
            break;
        default:
            break;
    }
}

void AttMeGlobalCallback(const BtEvent *Event)
{
	BtAttConn *conn;

    switch (Event->eType)
    {
        case BTEVENT_AUTHENTICATE_CNF:
	    case BTEVENT_AUTHENTICATED:
			conn = BT_ATT_GetConCtx(Event->p.remDev);
			if(conn !=NULL)
				BT_ATT_ReportEvent(BT_ATT_EVENT_CONNECTED_SECURITY_CHANGE, conn, NULL);
            break;
        default:
            break;
    }
}

void AttStateInit(void)
{
    ME_InitHandler(&attAuthenHandler);
    attAuthenHandler.callback = AttMeCallback;
    ME_InitHandler(&attGlobalHandler);
    attGlobalHandler.callback = AttMeGlobalCallback;
	ME_RegisterGlobalHandler(&attGlobalHandler);

}

void AttStartAuthentication(BtRemoteDevice *link)
{
#if 0 // QDAY
    if(link->authState == BAS_NOT_AUTHENTICATED)
	{
        SEC_AuthenticateLink(&attAuthenHandler, link);
	}
#endif
}

/****************************************************************************
 *
 * Internal Function prototypes
 *
 ****************************************************************************/

void BT_ATT_HandleConnected(BtRemoteDevice *link)
{
	U8 i=0;
	BtATTExchangeMTURequestStruct ptr;
    
    i = BT_ATT_FindAndAssignEmptyConn(link);
	AttStartAuthentication(link);
	BT_ATT_ReportEvent(BT_ATT_EVENT_CONNECTED, &ATTC(conn)[i], NULL);

}

void BT_ATT_HandleDisconnected(BtRemoteDevice *link)
{
	U8 i=0;

	i = BT_ATT_FindAndAssignEmptyConn(link);
	BT_ATT_ReportEvent(BT_ATT_EVENT_DISCONNECTED, &ATTC(conn)[i], NULL);
    BT_ATT_FreeConn(link);
}

void BT_ATT_HandleTxDone(BtRemoteDevice *link, BtPacket *pkt)
{
    U8 con_id;
    U8 wait_response = 0;
    
    con_id = BT_ATT_GetConId(link);
    if(con_id >= MAX_ATT_CON_NO)
    {
        return;
    }
    switch(pkt->data[0])
    {
    case BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST:
    case BT_ATT_OPCODE_FIND_INFO_REQUEST:        
    case BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST: 
    case BT_ATT_OPCODE_READ_BY_TYPE_REQUEST: 
    case BT_ATT_OPCODE_READ_REQUEST: 
    case BT_ATT_OPCODE_READ_BLOB_REQUEST: 
    case BT_ATT_OPCODE_READ_MULTIPLE_REQUEST: 
    case BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST: 
    case BT_ATT_OPCODE_WRITE_REQUEST: 
    case BT_ATT_OPCODE_PREPARE_WRITE_REQUEST: 
    case BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST:
        wait_response = 1;
        break;
    default:
        BT_ATT_ReportEvent(BT_ATT_EVENT_NOTIFIED_PACKET_HANDLED, (BtAttConn *)&ATTC(conn)[con_id],(void *) &pkt->llpContext);
        break;
    }
    if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
    {
        Report(("Packet Handled BLE"));
        if(wait_response == 1)
        {
            ATTC(conn)[con_id].bufferPkt = pkt;
        }
        else
        {
            //if(ATTC(conn)[con_id].bufferPkt !=0)
            //{
                //U8 tempIndex;
                
                //for(tempIndex =0; tempIndex<20;tempIndex++)
                    //Report(("ATT bufferPkt not empty, error"));

            //}
            //ATTC(conn)[con_id].bufferPkt = 0;
            InsertTailList(&ATTC(conn)[con_id].freePacketList,&(pkt->node));
        }
    }
    else
    {
        Report(("Packet Handled BREDR"));

        ATTC(conn)[con_id].bufferPkt = 0;
        InsertTailList(&ATTC(conn)[con_id].freePacketList,&(pkt->node));
    }
}


/*****************************************************************************
 * FUNCTION
 *  RfL2Callback
 * DESCRIPTION
 *  Callback routine for L2CAP connections.
 * PARAMETERS
 *  L2ChannelId     [IN]
 *  info            [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BT_ATT_L2Callback(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{
    Report(("ATT CALLBACK EVENT:%d",info->event));
    switch (info->event)
    {
		case L2EVENT_CONNECTED:
            BT_ATT_HandleConnected(info->aclLink);
            break;
        case L2EVENT_DISCONNECTED:
            BT_ATT_HandleDisconnected(info->aclLink);
            break;
        case L2EVENT_PACKET_HANDLED:
            BT_ATT_HandleTxDone(info->aclLink, info->ptrs.packet);
            break;
        case L2EVENT_DATA_IND:
			if(info->aclLink->authState == BAS_NOT_AUTHENTICATED)
			{
                BtAttConn *conn;
                
                conn = BT_ATT_GetConCtx(info->aclLink); 
                if(conn !=NULL)
                    conn->securityCheckCount = 0;
			}
            BT_ATT_HandleRxReady(info->aclLink, info->ptrs.data, info->dataLen);
            break;

    }
}


void BtAttBREDRL2CallBack(L2capChannelId L2ChannelId, L2capCallbackParms *info)
{
    BtStatus status;
    U8 i=0;
    
    switch(info->event){
        case L2EVENT_CONNECT_IND:
            if(BT_ATT_GetConId(info->aclLink) == MAX_ATT_CON_NO)
            {
                /* No Att connection before */
                i = BT_ATT_FindAndAssignEmptyConn(info->aclLink);
                L2CAP_ConnectRsp(L2ChannelId, L2CONN_ACCEPTED, 0);
                ATTC(conn)[i].l2cap_id = L2ChannelId;
            }
            else
            {
                L2CAP_ConnectRsp(L2ChannelId, L2CONN_REJECT_NO_RESOURCES, 0);
            }
            break;
        case L2EVENT_CONNECTED:
            BT_ATT_HandleConnected(info->aclLink);
            break;
        case L2EVENT_DISCONNECTED:
            BT_ATT_HandleDisconnected(info->aclLink);
            break;
        case L2EVENT_PACKET_HANDLED:
            BT_ATT_HandleTxDone(info->aclLink, info->ptrs.packet);
            break;
        case L2EVENT_DATA_IND:
            if(info->aclLink->authState == BAS_NOT_AUTHENTICATED)
            {
    //              SEC_AuthenticateLink(&attAuthenHandler, info->aclLink);
            }
            BT_ATT_HandleRxReady(info->aclLink, info->ptrs.data, info->dataLen);
            break;

        default:
            break;
    }

}


BtStatus AttClientRequest(BtRemoteDevice *link)
{
    BtStatus status;
    U16 local_id;
    U8 i=0;

    if(BT_ATT_GetConId(link) == MAX_ATT_CON_NO)
    {
        /* No Att connection before */
        i = BT_ATT_FindAndAssignEmptyConn(link);

        ATTC(conn)[i].secRecord.id =  SEC_ATT_ID;
        ATTC(conn)[i].secRecord.channel = (U32) &ATTC(conn)[i];
        if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
            ATTC(conn)[i].secRecord.level = BSL_NO_SECURITY;
        else
        {
            ATTC(conn)[i].secRecord.level = BSL_AUTHORIZATION_IN | BSL_ENCRYPTION_IN | BSL_ENCRYPTION_OUT | BT_SEC_LOW;
            ATTC(conn)[i].secRecord.level_type = BT_SEC_MED;
        }
            
        SEC_RegisterRecord(&ATTC(conn)[i].secRecord);
        
        ATTC(conn)[i].secToken.id = SEC_ATT_ID;
        ATTC(conn)[i].secToken.remDev = link;
        ATTC(conn)[i].secToken.channel = (U32) &ATTC(conn)[i];
        ATTC(conn)[i].secToken.incoming = FALSE;
        status = SEC_AccessRequest(&(ATTC(conn)[i].secToken));
        Report(("BT Att Client request status:%d", status));
    }
    else
    {
        return BT_STATUS_SUCCESS;
    }

    if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
    {
        if(status == BT_STATUS_SUCCESS)
        {
            L2CAP_LinkConnectedByCid(link, 0x0004);
            return BT_STATUS_SUCCESS;
        }
        else
        {
            return BT_STATUS_PENDING;
        }
    }
    else
    {
        if(status == BT_STATUS_SUCCESS)
        {
            status = L2CAP_ConnectReq(&(ATTC(psm)), BT_PSM_ATT, link, 0, &local_id);
            Report(("ATT Client request"));
            if(status == BT_STATUS_PENDING)
            {
                ATTC(conn)[i].l2cap_id = local_id;
                return BT_STATUS_PENDING;
            }
        }
        else if(status == BT_STATUS_PENDING)
        {
            return BT_STATUS_PENDING;
        }
        else
        {
            return BT_STATUS_FAILED;
        }
    }
    return BT_STATUS_SUCCESS;

}


BtStatus AttDisconnectRequest(BtRemoteDevice *link)
{
    BtStatus status = BT_STATUS_FAILED;
    U16 con_id;

    if(ME_GetDeviceType(&link->bdAddr) == BT_DEV_TYPE_LE)
    {
        status = BT_STATUS_SUCCESS;
    }
    else
    {
        con_id = BT_ATT_GetConId(link);
        if(con_id != MAX_ATT_CON_NO)
        {
            status = L2CAP_DisconnectReq(ATTC(conn)[con_id].l2cap_id );
            Report(("status:%d",status));
        }
    }
    Report(("AttDisconnectRequest status:%d", status));

    return status;

}

void AttSecCallback(const BtEvent *Event)
{
    BtAttConn *channel = ContainingRecord(Event->p.secToken, BtAttConn, secToken);
    BtStatus status;
    U16 local_id;
    U8 i=0;

    if (Event->p.secToken->incoming == FALSE)
    {
        if (Event->eType == BTEVENT_ACCESS_APPROVED)
        {
            if(ME_GetDeviceType(&Event->p.secToken->remDev->bdAddr) == BT_DEV_TYPE_LE)
            {
                L2CAP_LinkConnectedByCid(Event->p.secToken->remDev, 0x0004);
                return;
            }
            else
            {
                status = L2CAP_ConnectReq(&(ATTC(psm)), BT_PSM_ATT, Event->p.secToken->remDev, 0, &local_id);
                Report(("ATT Client request"));
                if(status == BT_STATUS_PENDING)
                {
                    i = BT_ATT_GetConId(Event->p.secToken->remDev);
                    if(i < MAX_ATT_CON_NO)
                    {
                        ATTC(conn)[i].l2cap_id = local_id;
                    }
                }
            }
        }
        else
        {
            BT_ATT_HandleDisconnected(Event->p.secToken->remDev);
        }
    }

}

void AttSecCallback1(const BtEvent *Event)
{
    BtAttConn *channel = ContainingRecord(Event->p.secToken, BtAttConn, secToken);
    BtStatus status;
    U16 local_id;
    U8 i=0;

    if (Event->p.secToken->incoming == FALSE)
    {
        if (Event->eType == BTEVENT_ACCESS_APPROVED)
        {
            BT_ATT_SecurityCheckDone(channel->link);
        }
    }

}

#if 0
static U8 dlight_test =0;
void BT_ATT_RUN_Dlight_Test(BtRemoteDevice *link)
{
    BtAttConn *conn;

    conn = BT_ATT_GetConCtx(link);
#ifdef __BT_ATT_INITIATOR__
    switch (dlight_test)
    {
        case 0x00:
        {
            attTesterTestCase0(link);
            break;
        }
        case 0x01:
        {
            attTesterTestCase1(link);
            break;
        }
        case 0x02:
        {
            attTesterTestCase2(link);
            break;
        }
        case 0x03:
        {
            attTesterTestCase3(link);
            break;
        }
        case 0x04:
        {
            attTesterTestCase4(link);
            break;
        }     
        case 0x05:
        {
            attTesterTestCase5(link);
            break;
        }
        case 0x06:
        {
            attTesterTestCase6(link);
            break;
        }
        case 0x07:
        {
            attTesterTestCase7(link);
            break;
        }        
        case 0x08:
        {
            attTesterTestCase8(link);
            break;
        }   
        case 0x09:
        {
            attTesterTestCase9(link);
            break;
        }   
        case 0x0a:
        {
            attTesterTestCase0a(link);
            break;
        }
        case 0x0b:
        {
            attTesterTestCase0b(link);
            break;
        }
        case 0x0c:
        {
            attTesterTestCase0c(link);
            break;
        }
        case 0x0d:
        {
            attTesterTestCase0d(link);
            break;
        }
        case 0x0e:
        {
            attTesterTestCase0e(link);
            break;
        }
        case 0x0f:
        {
            attTesterTestCase0f(link);
            break;
        }
        case 0x10:
        {
            attTesterTestCase10(link);
            break;
        }
        case 0x11:
        { 
            attTesterTestCase11(link);
            break;
        }
        case 0x12:
        {
            attTesterTestCase12(link);
            break;
        }
        case 0x13:
        {
            attTesterTestCase13(link);
            break;
        }
        case 0x14:
        {
            attTesterTestCase14(link);
            break;
        }
        case 0x15:
        {
            attTesterTestCase15(link);
            break;
        }
        case 0x16:
        {
            attTesterTestCase16(link);
            break;
        }
        case 0x17:
        {
            attTesterTestCase17(link); 
            break;
        }
        case 0x18:
        {
            attTesterTestCase18(link);
            break;
        }
        case 0x19:
        {
            attTesterTestCase19(link);
            break;
        }
        case 0x1a:
        {
            attTesterTestCase1a(link);
            break;
        }
        case 0x1b:
        {
            attTesterTestCase1b(link);
            break;
        }        
        case 0x1c:
        {
            attTesterTestCase1c(link);
            break;
        }
        case 0x1d:
        {
            attTesterTestCase1d(link);
            break;
        }
        case 0x1e: 
        {
            attTesterTestCase1e(link);
            break;
        }

    }
#else
#if 0
    switch(dlight_test)
    {
        case 0x00:
            attTesterTestServerCase0(link);
            break;
        case 0x01:
            attTesterTestServerCase1(link);
            break;
    }
#endif    
#endif
    dlight_test++;
}
#endif

#endif
