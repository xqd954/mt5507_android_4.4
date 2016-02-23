/*****************************************************************************
 *
 * Filename:
 * ---------
 * htpattdb.c
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
 * 11 02 2011 dlight.ting
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

#include "htsservice.h"
#include "disservice.h"

#include "htp.h"

extern U8 HtptTemperatureClientConfigCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
//extern U8 HtptTemperatureTypeCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 HtptImtermediateTemperatureClientConfigCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 HtptIntervalValueCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
extern U8 HtptIntervalClientConfigCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);
//extern U8 HtptIntervalRangeCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);

BtHtsServiceCbHandler HtptTemperatureClientConfig;
//BtHtsServiceCbHandler HtptTemperatureType;
BtHtsServiceCbHandler HtptImtermediateTemperatureClientConfig;
BtHtsServiceCbHandler HtptIntervalValue;
BtHtsServiceCbHandler HtptIntervalClientConfig;
//BtHtsServiceCbHandler HtptIntervalRange;


U8 HtpAttDB_Registration(HtpThermometerParm *params)
{
    BtStatus status;
    
    status = HtsActivateService();

    if(status != BT_STATUS_SUCCESS)
    {
        goto exit;
    }
    status = DisActivateService();	
    if (status != BT_STATUS_SUCCESS)
    {
        DisDeactivateService();
        goto exit;
    }
    InitializeListEntry(&(HtptTemperatureClientConfig.node));
    //InitializeListEntry(&(HtptTemperatureType.node));
    InitializeListEntry(&(HtptImtermediateTemperatureClientConfig.node));
    InitializeListEntry(&(HtptIntervalValue.node));
    InitializeListEntry(&(HtptIntervalClientConfig.node));
    //InitializeListEntry(&(HtptIntervalRange.node));   

    HtptTemperatureClientConfig.callback = HtptTemperatureClientConfigCb;
    HtsRegisterAttdbCallback(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_TP_MEASURE,
                    &(HtptTemperatureClientConfig));
/*
    HtptTemperatureType.callback = HtptTemperatureTypeCb;    
    HtsRegisterAttdbCallback(ATT_HANDLE_HTS_CHARACTERISTIC_TP_TYPE_VALUE,
                    &(HtptTemperatureType));
*/
    HtptImtermediateTemperatureClientConfig.callback = HtptImtermediateTemperatureClientConfigCb;    
    HtsRegisterAttdbCallback(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_INTERMEDIATE_TP,
                    &(HtptImtermediateTemperatureClientConfig));

    HtptIntervalValue.callback = HtptIntervalValueCb;    
    HtsRegisterAttdbCallback(ATT_HANDLE_HTS_CHARACTERISTIC_MEASURE_INTERVAL_VALUE,
                    &(HtptIntervalValue));
    
    HtptIntervalClientConfig.callback = HtptIntervalClientConfigCb;    
    HtsRegisterAttdbCallback(ATT_HANDLE_HTS_DESCRIPTOR_CLIENT_CHAR_CONFIG_MEASURE_INTERVAL,
                    &(HtptIntervalClientConfig));
/*
    HtptIntervalRange.callback = HtptIntervalRangeCb;    
    HtsRegisterAttdbCallback(ATT_HANDLE_HTS_DESCRIPTOR_VALID_RANGE,
                    &(HtptIntervalRange));
*/    

    HtsSetValidRange(params->interval_range_min, params->interval_range_max);
    HtsSetMeasurementInterval(params->interval);
    HtsSetTemperatureType(params->temperature_type);

exit:

    return (U8)status;
}

U8 HtpAttDB_Deregistration(void)
{
    /* No sequance */
    HtsDeactivateService();
    DisDeactivateService();
    HtsDeregisterAttdbCallback(&(HtptTemperatureClientConfig));
    //HtsDeregisterAttdbCallback(&(HtptTemperatureType));
    HtsDeregisterAttdbCallback(&(HtptImtermediateTemperatureClientConfig));
    HtsDeregisterAttdbCallback(&(HtptIntervalValue));
    HtsDeregisterAttdbCallback(&(HtptIntervalClientConfig));
    //HtsDeregisterAttdbCallback(&(HtptIntervalRange));
    return (U8)BT_STATUS_SUCCESS;
}

#endif
