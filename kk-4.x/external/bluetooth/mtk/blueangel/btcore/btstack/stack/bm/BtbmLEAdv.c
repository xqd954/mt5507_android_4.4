#include "bt_common.h"
#include "bt_feature.h"
#include "bluetooth_gap_struct.h"
#include "ddb.h"
#include "bttypes.h"
#include "btalloc.h"
#include "btconfig.h"
#include "sec.h"
#include "sdp.h"
#include "btproperty.h"
#include "btbm_adp.h"
#include "btbm.h"
#include "sdap.h"
#include "btaccctrl.h"
#include "btutils.h"
#include "osapi.h"
#include "btbm_int.h"


#include "BtbmLeAdv.h"

BMLEADVCONTENT_T BM_LE_ADV_CTX;

#define LeAdv(f)  (BM_LE_ADV_CTX.f)


void BtbmLeAdvInit(void)
{
    OS_MemSet((U8 *)&BM_LE_ADV_CTX, 0, sizeof(BMLEADVCONTENT_T));
    InitializeListHead(&(LeAdv(regList)));
	
}

void BtbmLeAddCallback(BtAdvEventHandler *hdl)
{
    if(!IsNodeOnList(&(LeAdv(regList)), &(hdl->node)))
    {
        InitializeListEntry(&hdl->node);
        InsertTailList(&(LeAdv(regList)), &(hdl->node));
    }

}


void BtbmLeRemoveCallback(BtAdvEventHandler *hdl)
{
    if(IsNodeOnList(&(LeAdv(regList)), &(hdl->node)))
    {
        RemoveEntryList(&hdl->node);
    }
}


void BtbmHandlerBLEAdvEvent(const BtEvent *Event)
{
    ListEntry *entry;
    BtAdvEventHandler *handler;
	BOOL inWhiteList = FALSE;
	BOOL inBlackList = FALSE;	
	U8 index =0;
    U8 find_flag = 0;
    BtInquiryResult *inqResult = (BtInquiryResult *)&Event->p.inqResult;

	Report(("BtbmHandlerBLEAdvEvent"));
    entry = LeAdv(regList).Flink;
    Report(("Address %02x,%02x,%02x,%02x,%02x,%02x,",
        inqResult->bdAddr.addr[0],inqResult->bdAddr.addr[1],
        inqResult->bdAddr.addr[2],inqResult->bdAddr.addr[3],
        inqResult->bdAddr.addr[4],inqResult->bdAddr.addr[5]));

    while (entry && entry != &LeAdv(regList))
    {
		handler = ((BtAdvEventHandler *)entry);
       
		if(handler->whiteListNo != NULL)
		{
		    Report(("WhiteList no:%d", handler->whiteListNo[0]));
			inWhiteList = FALSE;
			for(index =0; index< handler->whiteListNo[0]; index++)
			{
				if (OS_MemCmp((U8 *)inqResult->bdAddr.addr, 6, 
								handler->whiteList+6*index, 6))
				{
					inWhiteList = TRUE;
				}
			}
		}
		else
		{
			inWhiteList = TRUE;
		}
       
		if(handler->blackListNo != NULL )
		{
                Report(("blackListNo no:%d", handler->blackListNo[0]));

			inBlackList = FALSE;
			for(index =0; index< handler->blackListNo[0]; index++)
			{
				if (OS_MemCmp((U8 *)inqResult->bdAddr.addr, 6, handler->blackList+6*index, 6))
				{
					inBlackList = TRUE;
				}
			}
		}

        entry = entry->Flink;
		Report(("Call callback inWhiteList:%d, inBlackList:%d", inWhiteList, inBlackList));
		/* in WhiteList and not inBlackList */
       
		if((inWhiteList) && (!inBlackList))
		{
		    find_flag = 1;
	        handler->callback(Event);
		}
    }
	Report(("BtbmHandlerBLEAdvEvent end"));
    if(find_flag ==0)
    {
        BtDeviceContext *bdc;

        if (0 != (bdc = DS_FindDevice(&inqResult->bdAddr.addr)))
        {
            if ((bdc->link == 0) && (bdc->under_used == BDS_NONE))
            {
                DS_DeleteDevice((U8 *)inqResult->bdAddr.addr);
            }

        }
    }

}



U32 BtbmAdvLeGetAdvIndex(void)
{
	U8	index = 0;
	U32 enabled = 0x01;
	
	for(index = 0; index < 32; index++)
	{
		enabled = 0x01 << index;
		if((LeAdv(allocateIndex) & enabled) == 0x00)
		{
			LeAdv(allocateIndex) |= enabled;
			break;
		}
	}
	if(index == 0x32)
		Assert(0);
	Report(("BtbmAdvLeGetAdvIndex %d", index));
	return index;
}
void BtBmLeAdvEnabled(U8 index ,BtAdvEventHandler *hdl)
{
	U32 enabled;

	enabled = 0x01 << index;
	Report(("BtBmLeAdvEnabled index:%d", index));
	Report(("BtBmLeAdvEnabled enabled:%d", enabled));
	Report(("BtBmLeAdvEnabled scanOngoing:%d", LeAdv(scanOngoing)));
	
	BtbmLeAddCallback(hdl); 

	if(LeAdv(scanOngoing))
	{
		/* Cancel first and trigger the enabled procedure again. */
	    ME_LE_CancelInquiry();
   }
	ME_LE_Inquiry();
	LeAdv(scanOngoing) = TRUE;
 	LeAdv(enabled) |= enabled;
}

void BtBmLeAdvDisabled(U8 index, BtAdvEventHandler *hdl)
{
	U32 enabled;

	enabled = 0x01 << index;

	Report(("BtBmLeAdvDisabled index:%d", index));
	LeAdv(enabled) &= ~enabled;
	Report(("BtBmLeAdvDisabled LeAdv(enabled):%d", LeAdv(enabled)));
	BtbmLeRemoveCallback(hdl);
	
	if((LeAdv(enabled)== 0x00) &&
           (LeAdv(scanOngoing) == TRUE))
	{
            Report(("Make command to cancel inquiry"));
            ME_LE_CancelInquiry();
            LeAdv(scanOngoing) = FALSE;
	}
	else
	{
		Report(("Other function need Adv, not stop"));
	}

}

BtAdvEventHandler testHandler;

void btbmTestAdvResult(const BtEvent *Event)
{
	Report(("btbmTestAdvResult"));
}

void BtbmLeTestCode(void)
{
	U32 index;

	index = BtbmAdvLeGetAdvIndex();
	testHandler.callback = btbmTestAdvResult;
	testHandler.whiteList = (U8 *)get_ctrl_buffer(6*2);
	testHandler.whiteList[0] = 0x1b;
	testHandler.whiteList[1] = 0x00;
	testHandler.whiteList[2] = 0x42;
	testHandler.whiteList[3] = 0xdc;
	testHandler.whiteList[4] = 0x23;
	testHandler.whiteList[5] = 0x09;

	testHandler.whiteList[6] = 0x01;
	testHandler.whiteList[7] = 0x01;
	testHandler.whiteList[8] = 0x01;
	testHandler.whiteList[9] = 0x01;
	testHandler.whiteList[10] = 0x01;
	testHandler.whiteList[11] = 0x01;	
	testHandler.whiteListNo = 2;
	testHandler.blackListNo = 0;
	testHandler.blackList = NULL;		
	BtBmLeAdvEnabled(index, &testHandler);
}
