/*****************************************************************************
 *
 * Filename:
 * ---------
 * ble_utils.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK BLE Utility Definition Header File.
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef __BLE_UTILS_H__
#define __BLE_UTILS_H__

#include <bttypes.h>
#include <me_adp.h>

#define BLE_UTILS_MALLOC(size)		get_ctrl_buffer(size)
#define BLE_UTILS_FREE(p)			do{if(p){free_ctrl_buffer(p); p = NULL;}}while(0)		

#define BLE_DEV_ADDR_IS_EQUAL(dev1, dev2)     (OS_MemCmp(\
                                                                                                         (U8 *)(dev1),  sizeof(BD_ADDR), (U8 *)(dev2), \
                                                                                                         sizeof(BD_ADDR)) ? TRUE : FALSE)


/*---------------------------------------------------------------------------
 * BleQueryType enum
 *
 *     Define the GATT sub-procedure used for Service Definition query
 *      BLE_QUERY_FULL: for unbonded device
 *                                      Use full GATT sub-procedures, including Find primary service, 
 *                                      Find include service, Find all characteristics, Read (long) characteristic value, 
 *                                      Find all descriptors, Read (long) characteristic descriptor value
 *      BLE_QUERY_PARTIAL: for bonded device
 *                                      Use partial GATT sub-procedures, only including Read (long) characteristic value,
 *                                      Read (long) characteristic descriptor value
 */
/*typedef enum
{
    BLE_QUERY_FULL = 0,
    BLE_QUERY_PARTIAL
} BleQueryType;
*/

/*---------------------------------------------------------------------------
 * BleQueryServiceDefinitionCb
 *
 *     If BLE_QueryServiceDefinitionByUuid function returns BT_STATUS_PENDING, the query
 *     result shall be returned via the callback function.
 *     
 *      void *: user context pointer
 *      BtStatus: query service result 
 */
typedef void (*BleQueryServiceDefinitionCb) (void *, BtStatus);


typedef struct _BleService
{
    U16 start_handle;			
    U16 end_handle;			
    U8 uuid_size;	// unit: byte		
    union
    {
        U8 uuid16[2];			
        U8 uuid128[16];		
    } uuid;
} BleService;


typedef struct _BleIncludeService
{
    U16	include_service_handle;			
    U16	end_group_handle;			
    U8	uuid_size;	// unit: byte		
    union
    {
        U8 uuid16[2];           
        U8 uuid128[16];     
    } uuid;
    U16    handle;
} BleIncludeService;

typedef struct _BleChar
{
    U16 handle;          
    U8  properties;             // bt_gatt_char_properties: attdef.h
    U16 value_handle;            
    U8 value_uuid_size;    // unit: byte     
    union
    {
        U8  uuid16[2];         
        U8  uuid128[16];           
    } value_uuid;
    U8  value_size;          
    U8  *value;          
} BleChar;

typedef struct _BleCharDesc
{
    U16	handle;			
    U8	desc_uuid_size;	// unit: byte		
    union
    {
        U8  uuid16[2];         
        U8  uuid128[16];           
    } desc_uuid;
    U8	desc_size;			
    U8	*desc;			
} BleCharDesc;


typedef struct _BleServiceDB
{
    ListEntry	node;			
    BD_ADDR	bt_addr;			
    ListEntry	service_list;	// BleServiceNode is defined for each node		
} BleServiceDB;


typedef struct _BleServiceNode
{
    ListEntry	node;			
    BleService	service;			
    ListEntry	include_list;	// BleIncludeServiceNode is defined for each node		
    ListEntry	char_list;	        // BleCharNode is defined for each node		
} BleServiceNode;

typedef struct _BleIncludeServiceNode
{
    ListEntry   node;            
    BleIncludeService   include_service;            
} BleIncludeServiceNode;


typedef struct _BleCharNode
{
    ListEntry   node;            
    BleChar characteristic;          
    ListEntry   char_desc_list;     // BleCharDescNode is defined for each node        
} BleCharNode;


typedef struct _BleCharDescNode
{
    ListEntry   node;            
    BleCharDesc descriptor;          
} BleCharDescNode;


typedef struct _BlePacket
{
    U8  hd;
    void    *context;       // user context point      
    BleQueryServiceDefinitionCb cb;          
    BtRemoteDevice  *link;   
    BT_BOOL   bCancelled;
    ListEntry    service_list;         // there might be multiple instance for the same service uuid
    U8            explore_only;
    BtGattOp    gatt_op;         
    union
    {
        BtGattDiscoveryPrimaryServiceByUUID primaryServiceByUUID;
        BtGattFindIncludeServices findIncludeServices;
        BtGattDiscoveryAllCharacteristicOfService allCharOfServices;
        BtGattDiscoveryAllCharacteristicDescriptors allCharDescriptors;
        BtGattReadCharacteristicValue readCharValue;
        BtGattReadLongCharacteristicValue readLongCharValue;
        BtGattReadCharacteristicDescriptors readCharDescriptors;
        BtGattReadLongCharacteristicDescriptors readLongCharDescriptors;
    } parm;
} BlePacket;

BleServiceDB *BleFindServiceDBbyAddr(const BD_ADDR *bd_addr);
BleServiceDB *BleNewServiceDB(const BD_ADDR *bd_addr);

/*****************************************************************************
* FUNCTION
*  BLE_QueryServiceDefinitionByUuid
*
* DESCRIPTION
*   Use GATT procedure to find all information in indicated service definition. It shall contain 
*   service declaration and may contain include definitions and characteristic definitions 
*   which shall contain characteristic declaration, characteristic value declaration and 
*   may contain characteristic descriptor declarations.
*
*   It is an asynchronous procedure with message confirmation in callback function.
*
*****************************************************************************/
BtStatus BLE_QueryServiceDefinitionByUuid(void *context,
             BtRemoteDevice *link,
             BleQueryServiceDefinitionCb cb,
             U8 uuid_size,
             U8 *uuid,
             U8 **handle_out);


BtStatus BLE_QueryServiceDefinition(void *context,
             BtRemoteDevice *link,
             BleQueryServiceDefinitionCb cb,
             U8 uuid_size,
             U8 *uuid,
             U16 startHandle,
             U16 endHandle,
             U8 isPrimary,
             U8 **handle_out);

/*****************************************************************************
 * FUNCTION
 *  BLE_FindServiceByUuid
 *
 * DESCRIPTION
 *  1. New BlePacket
 *  2. New BleServiceDB, and insert into BleServiceDBTable.
 *
 *   It is synchronous call, and returns the data structure BleService.
 *
 *****************************************************************************/
BleService* BLE_FindServiceByUuid(const BD_ADDR *bt_addr,
             U8 uuid_size,
             U8 *uuid);
 
/*****************************************************************************
* FUNCTION
 *  BLE_FindNextServiceByUuid
 *
 * DESCRIPTION
 *
 *   It is used for multiple instance service to search more other service node with the 
 * same service UUID. BLE_FindServiceByUuid returns the first service that matches the 
 * specified service UUID.
 *
 *   It is synchronous call, and returns the data structure BleService.
 *
 *****************************************************************************/
BleService* BLE_FindNextServiceByUuid(const BD_ADDR *bt_addr,
                                                                             BleService* prev_service,
                                                                             U8 uuid_size,
                                                                             U8 *uuid);
 
/*****************************************************************************
* FUNCTION
*  BLE_FindIncludeServiceByUuid
*
* DESCRIPTION
*
*   It is synchronous call, and returns the data structure BleIncludeService.
*
*****************************************************************************/
BleIncludeService* BLE_FindIncludeServiceByUuid(BleService *in_service,
                                                                     U8 uuid_size,
                                                                     U8 *uuid);

BleIncludeService* BLE_FindNextIncludeServiceByUuid(BleService *in_service,
                                                                    BleIncludeService* prev_incl_service,
                                                                     U8 uuid_size,
                                                                     U8 *uuid);

/*****************************************************************************
* FUNCTION
*  BLE_FindCharByUuid
*
* DESCRIPTION
*
*   It is synchronous call, and returns the data structure BleChar.
*
*****************************************************************************/
BleChar* BLE_FindCharByUuid(BleService *in_service,
                                                                     U8 char_uuid_size,
                                                                     U8 *char_uuid);



BleChar* BLE_FindNextCharByUuid(BleService *in_service,
                                                                     BleChar *prev_char,                                                                
                                                                     U8 char_uuid_size,
                                                                     U8 *char_uuid);


/*****************************************************************************
* FUNCTION
*  BLE_FindCharDescByUuid
*
* DESCRIPTION
*
*   It is synchronous call, and returns the data structure BleCharDesc.
*
*****************************************************************************/
BleCharDesc* BLE_FindCharDescByUuid(BleChar *in_char,
                                                                                     U8 desc_uuid_size,
                                                                                     U8 *desc_uuid);

BleCharDesc* BLE_FindNextCharDescByUuid(BleChar *in_char,
                                                                                    BleCharDesc *prev_desc,
                                                                                     U8 desc_uuid_size,
                                                                                     U8 *desc_uuid);

/*****************************************************************************
* FUNCTION
*  BLE_UpdateServiceDatabase
*
* DESCRIPTION
*
*   Update service database to file if there is any characteristic value or descriptor value is changed.
*
*****************************************************************************/
void BLE_UpdateServiceDatabase(BD_ADDR *bt_addr);


/*****************************************************************************
* FUNCTION
*  BLE_StopQueryServiceDefinition
*
* DESCRIPTION
*
*   Stop query procedure.
*
*****************************************************************************/
void BLE_StopQueryServiceDefinition(U8 *handle);

BleServiceNode *BleFindServicebyHandleRange(ListEntry *service_list, U16 handle);

U8 BLE_CompareUuid(const U8 *uuid1, U8 len1, const U8 *uuid2, U8 len2);

U32 BLE_VerifyDB(BD_ADDR *bd_addr);

#endif
