/*****************************************************************************
 *
 * Filename:
 * ---------
 * HtsService.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile
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
 * 12 06 2011 dlight.ting
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
#ifndef __HTS_SERVICE_H__
#define __HTS_SERVICE_H__

typedef struct _BtHtsServiceCbHandler
{
    ListEntry node;         	/* For internal stack use only. */
    AttdbCallback callback;    /* Pointer to callback function */
} BtHtsServiceCbHandler;
/*
typedef struct _BtHtsClientConfigDescriptorNode
{
    ListEntry node;
    
    BT_BOOL bBonded;    // only bonded device shall be updated to file with those params below 
    BD_ADDR bdAddr;
    U8 temperatureClientCfgDesc[2];
    U8 intermediateTemperatureClientCfgDesc[2];
    U8 intervalClientCfgDesc[2];
} BtHtsClientConfigDescriptorNode;
*/

typedef struct _BleHts_T
{
    U8 registerCount;
    BtHtsServiceCbHandler HtsTemperatureClientConfigList;
    BtHtsServiceCbHandler HtsTemperatureTypeList;
    BtHtsServiceCbHandler HtsIntermediateTemperatureClientConfigList;
    BtHtsServiceCbHandler HtsIntervalValueList;
    BtHtsServiceCbHandler HtsIntervalClientConfigList;
    BtHtsServiceCbHandler HtsIntervalRangeList;    

    U8 interval[2];      // it is available only if measurement interval characteristic is supported
    U8 valid_range[4];     // those two are available only if measurement interval characteristic is supported and writable
    U8 temperature_type;    // it is available only if temperature type characteristic is existed
    //ListEntry HtsClientCfgDescList;
} BleHts_T;

U8 HtsActivateService(void);
void HtsDeactivateService(void);
void HtsRegisterAttdbCallback(U16 handle, BtHtsServiceCbHandler *handler);
void HtsDeregisterAttdbCallback(BtHtsServiceCbHandler *handler);

BtStatus HtsSetMeasurementInterval(U16 interval);
U8 *HtsGetMeasurementInterval(void);
void HtsSetValidRange(U16 valid_range_min, U16 valid_range_max);
void HtsSetTemperatureType(U8 type);
U8 *HtsGetClientConfigDescriptors(BD_ADDR *bd_addr, U16 char_uuid);

#endif
