/*****************************************************************************
 *
 * Filename:
 * ---------
 * DisService.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Device Info Service
 *
 * Author:
 * -------
 * Ting Zheng & Dlight Ting
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
 * 11 04 2011 dlight.ting
 * NULL
 * .
 *
 * 11 02 2011 dlight.ting
 * NULL
 * .
 *
 * 11 02 2011 dlight.ting
 * NULL
 * .
 *
 * 11 01 2011 dlight.ting
 * NULL
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_HTPT_PROFILE__
#include "attdb.h"
#include "attdef.h"
#include "attdbmain.h"
#include "disservice.h"

extern U16 DissGetHtsRecord(U8 *ptr);

BleDis_T BleDisServiceCtx ;

#define BleDisCtx(f)  (BleDisServiceCtx.f)



/*---------------------------------------------------------------------------
 *  DIS_MANUFACTURER_NAME constant
 *      Specify the manufacturer name.
 */
#define DIS_MANUFACTURER_NAME  'M','e','d','i','a','t','e','k'

/*---------------------------------------------------------------------------
 *  DIS_MODEL_NUM constant
 * 	 Specify the model number.
 */
#define DIS_MODEL_NUM  'M','T','6','6','2','0'

/*---------------------------------------------------------------------------
 *  DIS_SYSTEM_ID constant
 *	  Specify the system ID.
 */
#define DIS_SYSTEM_ID  0x00236cfffeafbdf4


 static const U8 DisManufacturerNameSize[] = {
	 DIS_MANUFACTURER_NAME
 };

 static const U8 DisModelNumSize[] = {
	 DIS_MODEL_NUM
 };


 const U8 attDisRecord[] = 
{
    ATT_HANDLE16(ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO, BT_UUID_GATT_TYPE_PRIMARY_SERVICE, BT_ATT_CONST_VARIABLE, 0x02),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETUUID16(BT_UUID_GATT_SERVICE_DEVICE),

    /* Manufacturer Name characteristic */
    ATT_HANDLE16(ATT_HANDLE_DIS_CHARACTERISTIC_MANUFACTURER_NAME, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ, ATT_HANDLE_DIS_CHARACTERISTIC_MANUFACTURER_NAME_VALUE, BT_UUID_DIS_CHAR_MANUFACTURER_NAME),

    ATT_HANDLE16(ATT_HANDLE_DIS_CHARACTERISTIC_MANUFACTURER_NAME_VALUE, BT_UUID_DIS_CHAR_MANUFACTURER_NAME, BT_ATT_CONST_VARIABLE, sizeof(DisManufacturerNameSize)),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        DIS_MANUFACTURER_NAME,

    /* Model Number characteristic */
    ATT_HANDLE16(ATT_HANDLE_DIS_CHARACTERISTIC_MODEL_NUM, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ, ATT_HANDLE_DIS_CHARACTERISTIC_MODEL_NUM_VALUE, BT_UUID_DIS_CHAR_MODEL_NUM),

    ATT_HANDLE16(ATT_HANDLE_DIS_CHARACTERISTIC_MODEL_NUM_VALUE, BT_UUID_DIS_CHAR_MODEL_NUM, BT_ATT_CONST_VARIABLE, sizeof(DisModelNumSize)),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        DIS_MODEL_NUM,

    /* System ID characteristic */
    ATT_HANDLE16(ATT_HANDLE_DIS_CHARACTERISTIC_SYSTEM_ID, BT_UUID_GATT_TYPE_CHARACTERISTIC, BT_ATT_CONST_VARIABLE, 0x05),
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        ATT_SETCHARACTERISTIC(GATT_CHAR_PROP_READ, ATT_HANDLE_DIS_CHARACTERISTIC_SYSTEM_ID_VALUE, BT_UUID_DIS_CHAR_SYSTEM_ID),

    ATT_HANDLE16(ATT_HANDLE_DIS_CHARACTERISTIC_SYSTEM_ID_VALUE, BT_UUID_DIS_CHAR_SYSTEM_ID, BT_ATT_CONST_VARIABLE, 0x08),
        ATT_PERMINSIONS(ATT_PERMISSIONS_GATT_DEFINED, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0), 
        DIS_SYSTEM_ID,
};


void DissInitialize(void)
{
    OS_MemSet((U8*)&BleDisServiceCtx, 0, sizeof(BleDisServiceCtx));
  
}

U8 DisActivateService(void)
{
    BtStatus status = BT_STATUS_SUCCESS; 
    Report(("BleDisCtx(registerCount) in DisActivateService:%d",BleDisCtx(registerCount)));

    if(BleDisCtx(registerCount) == 0)
    {
        status = ATTDB_AddRecord(attDisRecord, sizeof(attDisRecord));    
        if (status != BT_STATUS_SUCCESS)
        {
            return BT_STATUS_FAILED;
        }
        ATTDB_AddGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO,
            ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO_END,
            BT_UUID_GATT_TYPE_PRIMARY_SERVICE);

    }
    BleDisCtx(registerCount)++;
    return status;
}

void DisDeactivateService(void)
{
    Report(("BleDisCtx(registerCount) in DisDeactivateService:%d",BleDisCtx(registerCount)));

    if(BleDisCtx(registerCount) == 1)
    {
        ATTDB_RemoveRecord(ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO,ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO_END);
        ATTDB_RemoveGroupRecord(ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO,ATT_HANDLE_PRIMARY_SERVICE_DEVICE_INFO_END);
    }
    if(BleDisCtx(registerCount) == 0)
    {
        Assert(0);
    }
    BleDisCtx(registerCount)--;    
}


#endif
