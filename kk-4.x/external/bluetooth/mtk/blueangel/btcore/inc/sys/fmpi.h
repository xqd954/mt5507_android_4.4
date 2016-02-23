/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmpi.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile internal used header file.
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __FMPI_H__
#define __FMPI_H__

#include "fmp.h"

#define FMP_NUM_TX_PACKETS	1

#if defined(BTMTK_ON_LINUX)
#define FMP_SERVICE_DATABASE_FILE       "/data/@btmtk/fmpdb"  
#define FMP_SERVICE_DATABASE_TEMP_FILE       "/data/@btmtk/fmpdb_temp"  
#endif


typedef U8 FmpState;

#define FMP_STATE_CLOSED			0x00
#define FMP_STATE_OPENING			0x01
#define FMP_STATE_OPEN			0x02
#define FMP_STATE_DISC_PENDING	0x03		// when terminate connection timeout is set
#define FMP_STATE_CLOSING			0x04


#define fmp_malloc(size)		get_ctrl_buffer(size)
#define fmp_free(p)			do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		

#define fmp_dev_addr_equal(dev1, dev2)     (OS_MemCmp(\
                                                                                                         (U8 *)(dev1),  sizeof(BD_ADDR), (U8 *)(dev2), \
                                                                                                         sizeof(BD_ADDR)) ? TRUE : FALSE)

typedef struct _FmpServiceDB
{
    BD_ADDR bt_addr;

    U16 start_handle;	
    U16 end_handle;	
    //U16 char_handle;
    U16 char_value_handle;
} FmpServiceDB;


typedef struct _FmpPacket
{
    ListEntry node;

    BtGattOp gatt_op;
    union 
    {

        BtGattDiscoveryPrimaryServiceByUUID     primaryServiceByUUID;
        BtGattDiscoveryCharacteristicByUUID     charByUUID;
        BtGattWriteWithoutResponse      writeWithoutResponse;
    } parm;
} FmpPacket;

struct _FmpChannel
{
    ListEntry         node;

    /* === Internal use only === */
    FmpState	state;
    BtRemoteDevice *link;
    CmgrHandler cmgr_handler;
    /* Packet Pool */
    FmpPacket        packets[FMP_NUM_TX_PACKETS];
    ListEntry       packet_pool;

    /* Locator used only */
    FmpServiceDB *servicedb;
};


typedef struct _FmpContext
{
    ListEntry	dev_list;	// Collector or Thermometer connected device list 
    BtGattHandler gatt_handler;
    BtBleBondCbHandler  bond_handler;

    FmpServiceTypeFlag service_flag;
    FmpCallback fmp_callback;
} FmpContext;


#if XA_CONTEXT_PTR == XA_ENABLED
extern FmpContext *fmpcx;
#define FMP(s)  (fmpcx->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern FmpContext fmpcx;
#define FMP(s)  (fmpcx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/* gatt_utl.c */
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);
extern BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryCharacteristicByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteWithoutResponse(BtRemoteDevice *link, BtGattOp *op);

void FmpCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status); 
void FmpGattEventCallback(void *handler, BtGattEvent *event);
void FmpBondingCallback(U8 status, U8 *bd_addr);

BtStatus FmplFindPrimaryServicesByUuid(FmpChannel *channel, U16 uuid, U16 start_handle, U16 end_handle);
BtStatus FmplWriteWithoutResponse(FmpChannel *channel, U16 handle, U8 *data, U16 len);

FmpChannel *FmpNewChannel(void);
void FmpFreeChannel(FmpChannel *channel);
FmpChannel *FmpFindChannelByLink(BtRemoteDevice *remote_dev);
FmpChannel *FmpFindChannelByAddr(BD_ADDR *bd_addr);
FmpPacket *FmpGetPacket(FmpChannel *channel);
void FmpReleasePacket(FmpChannel *channel, FmpPacket *packet);
void FmpClearConnection(FmpChannel *channel, BtStatus status);
void FmpAppCallback(FmpChannel *channel, FmpEvent event, U32 status, void *ptr, U16 len);
void FmpWriteServiceDB(FmpServiceDB *servicedb);
BtStatus FmpReadServiceDB(BD_ADDR *bd_addr, FmpServiceDB **servicedb);
void FmpRemoveServiceDB(BD_ADDR *bd_addr);
void FmpGattOpCallback(void *userdata, BtGattEvent *event);

#endif
