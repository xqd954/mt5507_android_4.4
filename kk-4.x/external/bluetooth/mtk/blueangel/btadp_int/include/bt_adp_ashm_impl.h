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

#ifndef __BT_ADP_ASHM_IMPL_H__
#define __BT_ADP_ASHM_IMPL_H__

#include <utils/RefBase.h>
#include <utils/Timers.h>
#include <binder/IInterface.h>
#include <binder/IMemory.h>
#include <binder/Parcel.h>

#include "bt_common.h"
#include "assert.h"
//debug
#define ASSERT(a) assert(a)


/*---------------------------------------------------------------------------
 *
 * ListEntry is for creating doubly linked circular lists. They can be used as 
 * either a list head, or as part of another structure to allow that 
 * structure to be a link node. 
 */

namespace android {

#define  BTMTK_ASHM_SERVICE_NAME "bluetooth"
#define  BTMTK_ASHM_MAX_NAME_LEN  260

typedef struct _ListEntry
{
    struct _ListEntry *Flink;
    struct _ListEntry *Blink;

} ListEntry;

#define GetHeadList(ListHead) (ListHead)->Flink
#define GetTailList(ListHead) (ListHead)->Blink
#define GetNextNode(Node)     (Node)->Flink
#define GetPrevNode(Node)     (Node)->Blink
#define IsNodeConnected(n) (((n)->Blink->Flink == (n)) && ((n)->Flink->Blink == (n)))

typedef struct _BtAshareBuffer
{
	char             module_id[BTMTK_ASHM_MAX_NAME_LEN];
    uint32_t         memSize;
    sp<IMemoryHeap>  memHeap;
}BtAshareBuffer;

typedef struct _BtAshmNode
{
  ListEntry node;
  BtAshareBuffer buffer;
} BtAshmNode;

/**
 * Shared object interface
 */
class IShBuffer: public IInterface
{
public:
	DECLARE_META_INTERFACE(ShBuffer);
	virtual sp<IMemoryHeap> getBuffer(const char *module_id) = 0;
	virtual int newBuffer(const char *module_id, uint32_t memSize) = 0;
	virtual void delBuffer(const char *module_id) = 0;
};

/**
 * Server Side - Native
 */
class BnShBuffer: public BnInterface<IShBuffer> {
public:
	virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
};

class BtAdpAshmService : public BnShBuffer {
public:
	static status_t instantiate();
	BtAdpAshmService();
	virtual ~BtAdpAshmService();
	virtual sp<IMemoryHeap> getBuffer(const char *module_id);
	virtual BtAshmNode *findNode(const char *module_id);
	virtual int newBuffer(const char *module_id, uint32_t memSize);
	virtual void delBuffer(const char *module_id);
	void _InsertTailList(ListEntry *head, ListEntry *entry);
	void InsertTailList(ListEntry *head, ListEntry *entry);
	void _InsertHeadList(ListEntry *head, ListEntry *entry);
	ListEntry *_RemoveHeadList(ListEntry *head);
	ListEntry *RemoveHeadList(ListEntry *head);
	void RemoveEntryList(ListEntry *entry);
	BOOL IsNodeOnList(ListEntry *head, ListEntry *node);
	void MoveList(ListEntry *dest, ListEntry *src);
	void InsertList(ListEntry *dest, ListEntry *src, U8 front);
	BOOL IsListCircular(ListEntry *list);
	void InitializeListHead(ListEntry *list);
	void InitializeListEntry(ListEntry *list);
	U8 IsListEmpty(ListEntry *list);
private:
	ListEntry mMemList;
};

}; // namespace android

#endif // __BT_ADP_ASHM_IMPL_H__
