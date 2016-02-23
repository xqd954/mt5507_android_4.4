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

#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/MemoryHeapBase.h>
#include <binder/IServiceManager.h>

#include "bt_adp_ashm_impl.h"

namespace android {

enum
{
	GET_BUFFER = IBinder::FIRST_CALL_TRANSACTION,
	NEW_BUFFER,
	DEL_BUFFER,
};

/**
 *  Client side - Proxy
 */
class BpShBuffer: public BpInterface<IShBuffer> {
public:
	BpShBuffer(const sp<IBinder>& impl) : BpInterface<IShBuffer>(impl)
	{
	}

	sp<IMemoryHeap> getBuffer(const char *module_id)
	{
		Parcel data, reply;
		sp<IMemoryHeap> memHeap = NULL;
		data.writeInterfaceToken(IShBuffer::getInterfaceDescriptor());
		data.writeCString(module_id);
		remote()->transact(GET_BUFFER, data, &reply);
		memHeap = interface_cast<IMemoryHeap> (reply.readStrongBinder());
		return memHeap;
	}

	int newBuffer(const char *module_id, uint32_t memSize)
	{
		Parcel data, reply;

		data.writeInterfaceToken(IShBuffer::getInterfaceDescriptor());
		data.writeCString(module_id);
		data.writeInt32(memSize);
		remote()->transact(NEW_BUFFER, data, &reply);
		
		return reply.readInt32();;
	}

	void delBuffer(const char *module_id)
	{
		Parcel data, reply;

		data.writeInterfaceToken(IShBuffer::getInterfaceDescriptor());
		data.writeCString(module_id);
		remote()->transact(DEL_BUFFER, data, &reply);
		return ;
	}
};

IMPLEMENT_META_INTERFACE(ShBuffer, "vendor.mtk.bt.adp.ashm");

/**
 * Server side implementtation
 */
status_t BnShBuffer::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	switch (code)
	{
		case GET_BUFFER:
		{
			CHECK_INTERFACE(IShBuffer, data, reply);
			const char* module_id = data.readCString();
			
			sp<IMemoryHeap> Data = getBuffer(module_id);
			if (Data != NULL)
			{
				reply->writeStrongBinder(Data->asBinder());
			}
			return NO_ERROR;
			break;
		}
		case NEW_BUFFER:
		{
			CHECK_INTERFACE(IShBuffer, data, reply);
			const char* module_id = data.readCString();
			uint32_t size = data.readInt32();
			
			return (status_t)newBuffer(module_id, size);;
			break;
		}
		case DEL_BUFFER:
		{
			CHECK_INTERFACE(IShBuffer, data, reply);
			const char* module_id = data.readCString();
			
			delBuffer(module_id);
			return NO_ERROR;
			break;
		}
		default:
			return BBinder::onTransact(code, data, reply, flags);
	}
}

/*****************************************************************************
 * FUNCTION
 *  _InsertTailList
 * DESCRIPTION
 *  Insert an entry at the tail of the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::_InsertTailList(ListEntry *head, ListEntry *entry)
{
    entry->Flink = head;
    entry->Blink = head->Blink;
    head->Blink->Flink = entry;
    head->Blink = entry;
    ASSERT(IsNodeConnected(entry));

}


/*****************************************************************************
 * FUNCTION
 *  InsertTailList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::InsertTailList(ListEntry *head, ListEntry *entry)
{
    ASSERT(IsListCircular(head));
    _InsertTailList(head, entry);
    ASSERT(IsListCircular(head));
}


/*****************************************************************************
 * FUNCTION
 *  _InsertHeadList
 * DESCRIPTION
 *  Insert an entry at the head of the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::_InsertHeadList(ListEntry *head, ListEntry *entry)
{
    entry->Flink = head->Flink;
    entry->Blink = head;
    head->Flink->Blink = entry;
    head->Flink = entry;
    ASSERT(IsNodeConnected(entry));

}


/*****************************************************************************
 * FUNCTION
 *  _RemoveHeadList
 * DESCRIPTION
 *  Remove the first entry on the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 * RETURNS
 *  
 *****************************************************************************/
ListEntry *BtAdpAshmService::_RemoveHeadList(ListEntry *head)
{
    ListEntry *first;

    first = head->Flink;
    first->Flink->Blink = head;
    head->Flink = first->Flink;
    ASSERT(IsListCircular(head));
    return (first);

}


/*****************************************************************************
 * FUNCTION
 *  RemoveHeadList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  head        [OUT]     
 * RETURNS
 *  
 *****************************************************************************/
ListEntry *BtAdpAshmService::RemoveHeadList(ListEntry *head)
{
    ASSERT(IsListCircular(head));
    return (_RemoveHeadList(head));
}


/*****************************************************************************
 * FUNCTION
 *  RemoveEntryList
 * DESCRIPTION
 *  Remove the given entry from the list.
 * PARAMETERS
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::RemoveEntryList(ListEntry *entry)
{
    ASSERT(IsListCircular(entry));
    entry->Blink->Flink = entry->Flink;
    entry->Flink->Blink = entry->Blink;
    ASSERT(IsListCircular(entry->Blink));
    InitializeListEntry(entry);

}


/*****************************************************************************
 * FUNCTION
 *  IsNodeOnList
 * DESCRIPTION
 *  Determine if an entry is on the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  node        [OUT]     
 * RETURNS
 *  TRUE - the entry is on the list.
 *  FALSE - the entry is not on the list.
 *****************************************************************************/
BOOL BtAdpAshmService::IsNodeOnList(ListEntry *head, ListEntry *node)
{
    ListEntry *tmpNode;

    ASSERT(IsListCircular(head));
    tmpNode = GetHeadList(head);

    while (tmpNode != head)
    {
        if (tmpNode == node)
        {
            return (TRUE);
        }

        tmpNode = tmpNode->Flink;
    }
    return (FALSE);

}


/*****************************************************************************
 * FUNCTION
 *  MoveList
 * DESCRIPTION
 *  Moves a list to a new list head.  The source list is left empty.
 * PARAMETERS
 *  dest        [OUT]     
 *  src         [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::MoveList(ListEntry *dest, ListEntry *src)
{
    ASSERT(IsListCircular(src));
    if(IsListEmpty(src)){
        InitializeListHead(dest);
    }else{
        dest->Flink = src->Flink;
        dest->Blink = src->Blink;
        src->Flink->Blink = dest;
        src->Blink->Flink = dest;
        InitializeListHead(src);
    }
}

/*****************************************************************************
 * FUNCTION
 *  InsertList
 * DESCRIPTION
 *  Insert a list to another list.  The source list is left empty.
 * PARAMETERS
 *  dest        [OUT]
 *  src         [OUT]
 *  front       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::InsertList(ListEntry *dest, ListEntry *src, U8 front)
{
    ASSERT(IsListCircular(src));
    ASSERT(IsListCircular(dest));
    if(!IsListEmpty(src)){
        
    }else{
        if(front){
            dest->Flink->Blink = src->Blink;
            src->Blink->Flink = dest->Flink;
            dest->Flink = src->Flink;
            src->Flink->Blink = dest;
        }else{
            
            dest->Blink->Flink = src->Flink;
            src->Flink->Blink = dest->Blink;
            dest->Blink = src->Blink;
            src->Blink->Flink = dest; 
        }
         InitializeListHead(src);
    }
}


/*****************************************************************************
 * FUNCTION
 *  IsListCircular
 * DESCRIPTION
 *  Determine if the list is circular. This is only used for
 *  debugging purposes.
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  TRUE - the list is circular
 *  FALSE - the list is not circular.
 *****************************************************************************/
BOOL BtAdpAshmService::IsListCircular(ListEntry *list)
{

    ListEntry *tmp = list;

    if (!IsNodeConnected(list))
    {
        return (FALSE);
    }

    for (tmp = tmp->Flink; tmp != list; tmp = tmp->Flink)
    { 
        if (!IsNodeConnected(tmp))
        {
            return (FALSE);
        }
    }
    return (TRUE);

}


/*****************************************************************************
 * FUNCTION
 *  InitializeListHead
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::InitializeListHead(ListEntry *list)
{
    list->Flink = list;
    list->Blink = list;
}


/*****************************************************************************
 * FUNCTION
 *  InitializeListEntry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void BtAdpAshmService::InitializeListEntry(ListEntry *list)
{
    list->Flink = 0;
    list->Blink = 0;
}


/*****************************************************************************
 * FUNCTION
 *  IsListEmpty
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  
 *****************************************************************************/
U8 BtAdpAshmService::IsListEmpty(ListEntry *list)
{
    return ((list)->Flink == (list));
}

/**
 * Service implementation
 */
sp<IMemoryHeap> BtAdpAshmService::getBuffer(const char *module_id)
{
	BtAshmNode* bufNode = findNode(module_id);

	if(bufNode == NULL)
		return NULL;
	
	return bufNode->buffer.memHeap;
}

status_t BtAdpAshmService::instantiate()
{
	status_t status;
	sp<IBinder> service;
//	char serviceName[50];
//	strcpy(serviceName, BT_ADP_ASHM_SERVICE_NAME_PREFIX);
//	strncat(serviceName, name, 49-strlen(name));
	service = defaultServiceManager()->checkService( String16(BTMTK_ASHM_SERVICE_NAME) );
	if ( service == NULL ){

		status = defaultServiceManager()->addService(String16(BTMTK_ASHM_SERVICE_NAME), new BtAdpAshmService());
		return NO_ERROR;
	}
	else {
		// already exist
		return 1;
	}
}

BtAshmNode *BtAdpAshmService::findNode(const char *module_id)
{
	BtAshmNode* bufNode = (BtAshmNode *)GetHeadList(&mMemList);
	
    while (bufNode != (BtAshmNode*)&mMemList)
    {	
    	if(strcmp(bufNode->buffer.module_id, module_id) == 0)
    	{
			return bufNode;
    	}
		
        bufNode = (BtAshmNode*)GetNextNode(&bufNode->node); 
    }

	ALOGE("[bt_ashm] Doesn't find [%s] Node!\n.", module_id);
	return NULL;
}

int BtAdpAshmService::newBuffer(const char *module_id, uint32_t memSize)
{
	BtAshmNode* bufNode = NULL;
	int    len = strlen(module_id) >= (BTMTK_ASHM_MAX_NAME_LEN-1) ? (BTMTK_ASHM_MAX_NAME_LEN - 1) : strlen(module_id);

	bufNode = findNode(module_id);
	if(bufNode != NULL)
	{
		ALOGD("[bt_ashm] Has malloc %s buffer!\n.", module_id);
		return NO_ERROR;
	}
	
	bufNode = (BtAshmNode*)malloc(sizeof(BtAshmNode));
    memset(bufNode, 0, sizeof(BtAshmNode));

	if(bufNode == NULL)
		return -1;

	strncpy(bufNode->buffer.module_id, module_id, len);
	bufNode->buffer.module_id[len] = '\0';
	bufNode->buffer.memSize = memSize;
	bufNode->buffer.memHeap = new MemoryHeapBase(memSize, 0, module_id);

	InsertTailList(&mMemList, &bufNode->node);
	return NO_ERROR;
}

void BtAdpAshmService::delBuffer(const char *module_id)
{
	BtAshmNode* bufNode = findNode(module_id);

	if(bufNode == NULL)
	{
		ALOGD("[bt_ashm] Has free %s buffer!\n.", module_id);
		return;
	}
	
	bufNode->buffer.memHeap = NULL;
	RemoveEntryList(&bufNode->node);
	free(bufNode);
	return;
}

BtAdpAshmService::BtAdpAshmService()
{
	// MemoryHeapBase will use ashmem
	InitializeListHead(&mMemList);
}

BtAdpAshmService::~BtAdpAshmService()
{
	BtAshmNode* bufNode;

	while (!IsListEmpty(&mMemList))
    {             
        bufNode = (BtAshmNode*)RemoveHeadList(&mMemList);
		bufNode->buffer.memHeap = NULL;
  		free(bufNode);
    }
}

}; // namespace android
