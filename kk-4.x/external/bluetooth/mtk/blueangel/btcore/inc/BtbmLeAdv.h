#ifndef __BTBMLEADV_H__
#define __BTBMLEADV_H__
typedef void (*BtAdvEventCallBack) (const BtEvent *);


typedef struct _BtAdvEventHandler
{
    ListEntry node;         /* For internal stack use only. */
    BtAdvEventCallBack callback;    /* Pointer to callback function */
	U8 *whiteList;
	U8 *whiteListNo;
	U8 *blackList;
	U8 *blackListNo;
	
} BtAdvEventHandler;


typedef struct _BMLEADVContent_T
{
	U32 enabled;
	U32 allocateIndex;
	ListEntry regList;
	BOOL scanOngoing;
} BMLEADVCONTENT_T;

void BtbmLeAdvInit(void);
void BtbmHandlerBLEAdvEvent(const BtEvent *Event);
void BtBmLeAdvEnabled(U8 index ,BtAdvEventHandler *hdl);
void BtBmLeAdvDisabled(U8 index, BtAdvEventHandler *hdl);

#endif
