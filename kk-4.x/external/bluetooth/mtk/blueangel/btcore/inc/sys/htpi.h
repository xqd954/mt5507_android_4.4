/*****************************************************************************
 *
 * Filename:
 * ---------
 * htpi.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile internal used header file.
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
#ifndef __HTPI_H__
#define __HTPI_H__

#include "htp.h"



#define HTP_IDLE_TIMEOUT_THERMOMETER			5000		// millisecond
#define HTP_IDLE_TIMEOUT_COLLECTOR			10000		// millisecond
#define HTP_TEMPERATURE_INVALID_VALUE	0x007FFFFF	// Special float value NaN (Not a Number)

#define HTP_NUM_SERVICE_DATABASE		5
#define HTP_NUM_TX_PACKETS	5

#define HTP_ATT_ERROR_CODE_OUT_OF_RANGE         0x80


typedef U8 HtpState;

#define HTP_STATE_CLOSED			0x00
#define HTP_STATE_OPENING			0x01
#define HTP_STATE_OPEN			0x02
#define HTP_STATE_DISC_PENDING	0x03		// when terminate connection timeout is set
#define HTP_STATE_CLOSING			0x04


#define htp_malloc(size)		get_ctrl_buffer(size)
#define htp_free(p)			do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		

#define htp_dev_addr_equal(dev1, dev2)     (OS_MemCmp(\
                                                                                                         (U8 *)(dev1),  sizeof(BD_ADDR), (U8 *)(dev2), \
                                                                                                         sizeof(BD_ADDR)) ? TRUE : FALSE)


/************************************************************************************
 *   Write File:  Each Service Database Format
 *
 *     Service database for the indicated device shall be updated in file when
 *     1. Query done with success status
 *     2. Measurement Interval is changed
 *     3. Client Characteristic Configuration is changed only if the device is in bonded state
 *     4. Device is removed from APP and service database shall be remove as well
 *
 *               servicedb 
 *               {               
 *                   size                           (2 bytes, value does not include this parameter itself)
 *                   bt_address               (6 bytes)
 *                   features                     (1 byte)
 *                   service 
 *                   {
 *                       start_handle         (2 bytes)
 *                       end_handle           (2 bytes)
 *                       characteristic 
 *                       {
 *                           char_size            (1 byte, value does not include this parameter itself)
 *                           handle                   (2 bytes)
 *                           properties            (1 byte)
 *                           value_handle     (2 bytes)
 *                           value_uuid           (2 bytes)
 *                           value_size           (1 bytes, value does not include this parameter itself)
 *                           value[]
 *                           descscriptor (Optional)
 *                           {
 *                               handle               (2 bytes)
 *                               desc_uuid        (2 bytes)
 *                               desc_size        (1 bytes, value does not include this parameter itself)
 *                               desc[]
 *                           }
 *                       }
 *                    }
 *                }         
 *
 ************************************************************************************/
#if defined(BTMTK_ON_LINUX)
#define HTP_SERVICE_DATABASE_FILE       "/data/@btmtk/htpdb"  
#define HTP_SERVICE_DATABASE_TEMP_FILE       "/data/@btmtk/htpdb_temp"  
#endif
#define HTP_SERVICE_SIZE                    5       // start handle + end handle + size
#define HTP_CHARACTERISTIC_SIZE     8
#define HTP_CHARACTERISTIC_DESCRIPTOR_SIZE     4


/************************************************************************************
*    HtpServiceDB -> HTS/DIS service -> characteristic 1 -> handle, properties, value handle, value uuid
*                                                                                                            characteristic descriptor 1
*                                                                                                            characteristic descriptor 2
*                                                                                                            ...
*                                                                   -> characteristic 2 -> ...     
*                                                                         ... 
*
*                                                                   -> characteristic n -> ... 
*
************************************************************************************/

typedef struct _HtpService
{
    U8 size;
    U16 start_handle;	
    U16 end_handle;	
    ListEntry	char_list;			//List node is HtpCharListNode
} HtpService;

typedef struct _HtpCharListNode
{
    ListEntry node;

    U8 size;
    U16 handle;
    U8  properties;
    U16 value_handle;
    U16 value_uuid;
    U8 value_size;
    U8 *value;		// the sizeof each characteristic value may be variable for DIS, so use pointer here
    ListEntry	char_desc_list;	//List node is HtpCharDescListNode
} HtpCharListNode;

typedef struct _HtpCharDescListNode
{
    ListEntry node;

    U16 handle;
    U16 desc_uuid;
    U8 desc_size;
    U8	*desc;		// the size of each descriptor is fixed, 2 bytes or 4 bytes
} HtpCharDescListNode;


/**  HtpServiceDB structure definition
 * 
 * It is the service database cache for the thermometer device. It shall be 
 * deleted if the device is removed from trust device list. And it shall be 
 * updated if any attribute indication or service changed indication is received.
 */
typedef struct _HtpServiceDB
{
    BD_ADDR bt_addr;
    HtpCharFeature features;
	
    HtpService	hts_service;
    HtpService	dis_service;
} HtpServiceDB;

typedef struct _HtpPacket
{
    ListEntry node;

    BtGattOp gatt_op;
    union 
    {
        BtGattDiscoveryPrimaryServiceByUUID     primaryServiceByUUID;
        BtGattDiscoveryAllCharacteristicOfService	allChars;
        BtGattDiscoveryAllCharacteristicDescriptors	allCharDescriptors;	
        BtGattReadCharacteristicValue	readCharValue;
        BtGattWriteCharValueRequest	writeRequest;
        BtGattValueNotifications		valueNotifications;
        BtGattValueIndications		valueIndications;
        BtGattReadCharacteristicDescriptors	readCharDescriptors;
        BtGattWriteDescriptors	writeDescriptors;
    } parm;
} HtpPacket;


struct _HtpChannel
{
    ListEntry         node;


    /* === Internal use only === */
    HtpState	state;
    /* Thermometer is GAP Peripheral role, and Collector is GAP Central role.
    *  So it is obvious that the connection initiator is always Collector, and 
    *  the connection acceptor is Thermometer. */
    HtpRole	role;
    BtRemoteDevice *link;
    CmgrHandler cmgr_handler;
    /* Packet Pool */
    HtpPacket        packets[HTP_NUM_TX_PACKETS];
    ListEntry       packet_pool;
    HtpServiceDB  *servicedb;
    EvmTimer       readCharTimer;
    U8             readCharRetryCount;
};


typedef struct _HtpContext
{
    ListEntry	dev_list;	// Collector or Thermometer connected device list 
    HtpServiceTypeFlag	service_flag;	
    BtGattHandler gatt_handler;
    BtBleBondCbHandler  bond_handler;

    EvmTimer	idle_timer;

    /* for Collector only */
    HtpCallback htpc_callback;	

    /* for Thermometer only */
    HtpCallback htpt_callback;	
    //HtpThermometerParm server_parms;
} HtpContext;


#if XA_CONTEXT_PTR == XA_ENABLED
extern HtpContext *htpcx;
#define HTP(s)  (htpcx->s)
#else /* XA_CONTEXT_PTR == XA_ENABLED */
extern HtpContext htpcx;
#define HTP(s)  (htpcx.s)
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/* gatt_utl.c */
extern BtStatus GattClientConnect(BtRemoteDevice *link);
extern BtStatus GattDisconnect(BtRemoteDevice *link);
extern void BT_GattRegisterConnectedEvent(BtGattHandler *hdl, GattAPCallBack cb);
extern void BT_GattUnregisterConnectedEvent(BtGattHandler *hdl);
extern BtStatus BT_GATT_DiscoveryPrimaryServiceByUUID(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryAllCharacteristicOfService(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_DiscoveryAllCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_ReadCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);

extern BtStatus BT_GATT_WriteCharacteristicDescriptors(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_WriteCharacteristicValue(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_Indications(BtRemoteDevice *link, BtGattOp *op);
extern BtStatus BT_GATT_Notifications(BtRemoteDevice *link, BtGattOp *op);



void HtpCmgrCallback(CmgrHandler *Handler, CmgrEvent Event, BtStatus Status); 
void HtpGattEventCallback(void *handler, BtGattEvent *event);
void HtpBondingCallback(U8 status, U8 *bd_addr);


HtpChannel *HtpNewChannel(void);
void HtpFreeChannel(HtpChannel *channel);
HtpChannel *HtpFindChannelByLink(BtRemoteDevice *remote_dev);
HtpChannel *HtpFindChannelByAddr(BD_ADDR *bd_addr);
HtpService *HtpFindServicebyHandleRange(HtpServiceDB *servicedb, U16 handle);
HtpCharListNode *HtpFindCharbyHandleRange(HtpService *service, U16 handle);
HtpCharListNode *HtpFindCharbyHandle(HtpService *service, U16 value_handle);
HtpCharListNode *HtpFindCharByUuid(HtpService *service, U16 char_uuid);
HtpCharDescListNode * HtpFindCharDescByUuid(HtpService *service, U16 char_uuid, U16 desc_uuid);
HtpServiceDB *HtpNewServiceDB(BD_ADDR *addr);
void HtpFreeServiceDB(HtpServiceDB *servicedb);
HtpPacket *HtpGetPacket(HtpChannel *channel);
void HtpReleasePacket(HtpChannel *channel, HtpPacket *packet);
void HtpAppCallback(HtpChannel *channel, HtpEvent event, U32 status, void *ptr, U16 len);
void HtpClearConnection(HtpChannel *channel, BtStatus status);
void HtpWriteServiceDB(HtpServiceDB *servicedb);
BtStatus HtpReadServiceDB(BD_ADDR *bd_addr, HtpServiceDB **servicedb);
void HtpRemoveServiceDB(BD_ADDR *bd_addr);
void HtpIdleConnectionStartTimer(HtpChannel *channel, U16 timer_ms);
void HtpIdleConnectionStopTimer(void);

BtStatus HtpcFindPrimaryServicesByUuid(HtpChannel *channel, U16 uuid, U16 start_handle, U16 end_handle);
void HtpcReadCharacteristicEx(HtpChannel *channel, HtpCharListNode *char_node, HtpService *service, HtpServiceDB *servicedb);
BtStatus HtpcWriteCharacteristic(HtpChannel *channel, U16 handle, U8 *data, U16 len);
BtStatus HtpcWriteCharacteristicDescriptor(HtpChannel *channel, U16 handle, U8 *data, U16 len);
BtStatus HtptSendIndication(HtpChannel *channel, U16 handle, U8 *data, U16 len);
BtStatus HtptSendNotification(HtpChannel *channel, U16 handle, U8 *data, U16 len);
void HtpGattOpCallback(void *userdata, BtGattEvent *event);

#endif
