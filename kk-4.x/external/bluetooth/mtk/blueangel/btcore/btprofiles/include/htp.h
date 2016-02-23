/*****************************************************************************
 *
 * Filename:
 * ---------
 * htp.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Health Thermometer Profile external used header file.
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
#ifndef __HTP_H__
#define __HTP_H__

#include <bttypes.h>
#include <conmgr.h>
#include "gatt.h"


/*******************************************
*
*  HTP Characteristic Feature Configuration Definition 
*
********************************************/

/*---------------------------------------------------------------------------
 * HTP_TEMPERATURE_TYPE constant
 *     HTP Temperature Type Characteristic is included
 *     if HTP_TEMPERATURE_TYPE is defined as XA_ENABLED.
 */
#ifndef HTP_TEMPERATURE_TYPE
#define HTP_TEMPERATURE_TYPE XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * HTP_INTERMEDIATE_TEMPERATURE constant
 *     HTP Intermediate Temperature Characteristic is included
 *     if HTP_INTERMEDIATE_TEMPERATURE is defined as XA_ENABLED.
 */
#ifndef HTP_INTERMEDIATE_TEMPERATURE
#define HTP_INTERMEDIATE_TEMPERATURE XA_ENABLED
#endif

/*---------------------------------------------------------------------------
 * HTP_MEASUREMENT_INTERVAL constant
 *     HTP Measurement Interval Characteristic is included
 *     if HTP_MEASUREMENT_INTERVAL is defined as XA_ENABLED.
 */
#ifndef HTP_MEASUREMENT_INTERVAL
#define HTP_MEASUREMENT_INTERVAL XA_ENABLED
#endif



/*---------------------------------------------------------------------------
 * HtpEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type HtpCallback. The "HtpCallbackParms.event" field will
 *     contain one of the event types below.
 */
typedef U8 HtpEvent;

/** A connection is open.  This event can be received as the result of a call to 
 *  HTP_Open or GATT connected by remote device.  When this event has been 
 *  received, the channel is available.  When this event is received in the callback, 
 *  the "ptrs.remDev" field contains a pointer to the remote device.
 */
#define HTPEVENT_OPEN	0x00		

/** The connection is closed.  This can come as a result of calling 
 *  HTP_Close or if the remote device has closed the connection, 
 *  When this event is received, the "ptrs.remDev" field contains 
 *  a pointer to the remote device.
 */
#define HTPEVENT_CLOSED	0x01		

/** It is for Collector only.
 *  This event can be received as the result of a call to HTPC_QueryGattService. 
 *  No "ptrs" field is included.
 */
#define HTPEVENT_GATT_QUERY_CNF	0x02		


/** It is for Collector only.
 *  This event can be received if receive write response of Client Characteristic
 *  Configuration Descriptor. 
 *  No "ptrs" field is included.
 */
#define HTPEVENT_SET_TEMPERATURE_CONFIG_CNF	0x03		

/** It is for Collector only.
 *  This event can be received if receive write response of Client Characteristic
 *  Configuration Descriptor. 
 *  No "ptrs" field is included.
 */
#define HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_CNF	0x04		

/** It is for Collector only.
 *  This event can be received if receive write response of Client Characteristic
 *  Configuration Descriptor. 
 *  No "ptrs" field is included.
 */
#define HTPEVENT_SET_INTERVAL_CONFIG_CNF	0x05		

/** It is for Collector only.
 *  This event can be received if receive write response of characteristic. 
 *  No "ptrs" field is included.
 */
#define HTPEVENT_SET_INTERVAL_CNF           0x06	

/** It is for Collector only.
 *  This event can be received if receive Temperature Measurement indication. 
 *  When this event is received, the "ptrs.temperature" field contains 
 *  a pointer to the temperature measurement data.
 */
#define HTPEVENT_MEASUREMENT_IND		0x07	

/** It is for Collector only.
 *  This event can be received if receive Intermediate Temperature notification. 
 *  When this event is received, the "ptrs.temperature" field contains 
 *  a pointer to the intermediate temperature data.
 */
#define HTPEVENT_MEASUREMENT_NOTIFY	0x08	

/** It is for Collector only.
 *  This event can be received if receive Measurement Interval value 
 *  changed indication. When this event is received, the "ptrs.interval" 
 *  field contains the interval value.
 */
#define HTPEVENT_INTERVAL_VALUE_IND	0x09	

/** It is for Thermometer only.
 *  This event can be received if receive write request of Client Characteristic 
 *  Configuration Descriptor. When this event is received, the "ptrs.flag" 
 *  field contains the configuration flag.
 */
#define HTPEVENT_SET_TEMPERATURE_CONFIG_REQ	0x0a	

/** It is for Thermometer only.
 *  This event can be received if receive write request of Client Characteristic 
 *  Configuration Descriptor. When this event is received, the "ptrs.flag" 
 *  field contains the configuration flag.
 */
#define HTPEVENT_SET_INTERMEDIATE_TEMPERATURE_CONFIG_REQ	0x0b	

/** It is for Thermometer only.
 *  This event can be received if receive write request of Client Characteristic 
 *  Configuration Descriptor. When this event is received, the "ptrs.flag" 
 *  field contains the configuration flag.
 */
#define HTPEVENT_SET_INTERVAL_CONFIG_REQ	0x0c	

/** It is for Thermometer only.
 *  This event can be received if receive write request of write request of characteristic. 
 *  When this event is received, the "ptrs.interval" field contains the interval value.
 */
#define HTPEVENT_SET_INTERVAL_REQ		0x0d	

/** It is for Thermometer only.
 *  This event can be received if receive confirmation of Temperature Measurement indication.
 *  No "ptrs" field is included.
 */
#define HTPEVENT_MEASUREMENT_IND_CNF		0x0e	

/** It is for Thermometer only.
 *  This event can be received if notify upper layer that notification of Intermediate
 *  Measurement packet is handled.
 *  No "ptrs" field is included.
 */
#define HTPEVENT_MEASUREMENT_NOTIFY_CNF	0x0f	

/** It is for Thermometer only.
 *  This event can be received if receive confirmation of Measurement Interval value indication.
 *  No "ptrs" field is included.
 */
#define HTPEVENT_INTERVAL_VALUE_IND_CNF	0x10	



/** It is the definition of operation flags
 */
/*typedef U8 HtpOpFlag;

#define HTP_OP_FLAG_QUERY_THERMOMETER_SERVICE	0x01
#define HTP_OP_FLAG_QUERY_DEVICEINFO_SERVICE
*/


/*
*   Those are optional characteristic and properties
*/
typedef U8 HtpCharFeature;

#define HTP_CHAR_FEATURE_TEMPERATURE_TYPE_READABLE				0x01
#define HTP_CHAR_FEATURE_INTERMEDIATE_MEASUREMENT_NOTIFIABLE		0x02
#define HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_READABLE	0x04
#define HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_WRITABLE	0x08
#define HTP_CHAR_FEATURE_MEASUREMENT_INTERVAL_INDICATABLE	0x10



/** It is the flag field in HtpTemperature.
 *  Bit0: 0 Celsius; 1 Fahrenheit
 *  Bit1: 0 Time stamp shall not be included; 1 Time stamp is supported
 *  Bit2: 0 Temperature type shall not be included; 1 Temperature type is supported and is non-static 
 *  Reserved for Future Use (RFU) bits: 0
 */
typedef U8 HtpTemperatureFlag;

#define HTP_TEMPERATURE_FLAG_FAHRENHEIT	0x01
#define HTP_TEMPERATURE_FLAG_TIMESTAMP	0x02
#define HTP_TEMPERATURE_FLAG_NONSTATIC	0x04


/** It is the temperature type field in HtpTemperature, or
 * Temperature Type characteristic value.
 */
typedef U8 HtpTemperatureType;

#define HTP_TEMPERATURE_TYPE_ARMPIT	0x01		//Armpit
#define HTP_TEMPERATURE_TYPE_BODY	0x02		//General
#define HTP_TEMPERATURE_TYPE_EAR		0x03		//Ear (usually ear lobe)
#define HTP_TEMPERATURE_TYPE_FINGER	0x04		//Finger
#define HTP_TEMPERATURE_TYPE_GIT		0x05		//Gastro-intestinal Tract
#define HTP_TEMPERATURE_TYPE_MOUTH	0x06		//Mouth
#define HTP_TEMPERATURE_TYPE_RECT	0x07		//Rectum
#define HTP_TEMPERATURE_TYPE_TOE		0x08		//Toe
#define HTP_TEMPERATURE_TYPE_TYMP	0x09		//Tympanum (ear drum)

/* Foreward References */
typedef struct _HtpCallbackParms HtpCallbackParms;
typedef struct _HtpChannel HtpChannel;
//typedef struct _HtpPacketParm HtpPacketParm;


/*---------------------------------------------------------------------------
 * HtpCallback type
 *
 *     Represents a callback function called by HTP to indicate events and 
 *     data to the application. The event is targeted to the HTP connection
 *     identified by the "Channel" parameter. Information about the 
 *     event is contained in the "Parms" parameter.
 */
typedef void (*HtpCallback)(HtpChannel *Channel, HtpCallbackParms *Parms);

typedef U8 HtpServiceTypeFlag;

#define HTP_SERVICE_TYPE_COLLECTOR_FLAG 		0x01
#define HTP_SERVICE_TYPE_THERMOMETER_FLAG	0x02


/*typedef enum _HtpGattService
{
    HTP_GATT_HEALTH_THERMOMETER_SERVICE,
    HTP_GATT_DEVICE_INFO_SERVICE		
} HtpGattService;*/

typedef enum _HtpRole
{
    HTP_ROLE_UNKNOWN = 0,
    HTP_ROLE_THERMOMETER,
    HTP_ROLE_COLLECTOR		
} HtpRole;

typedef struct _HtpTemperatureValue
{
    U8	  mantissa[3];  /* It shall be transmitted in little endian */
    U8  exponent;
} HtpTemperatureValue;

typedef struct _HtpTimestamp
{
    U8  year[2];
    U8  month;
    U8	  day;
    U8	  hour;
    U8  minute;
    U8	  second;
} HtpTimestamp;

typedef struct _HtpTemperature
{
    HtpTemperatureFlag	flags;
    HtpTemperatureValue temperature_value;
    HtpTimestamp	time_stamp;
    HtpTemperatureType	temperature_type;
} HtpTemperature;

/** Unit is in seconds
 */
typedef U16 HtpMeasurementInterval;

typedef U16 HtpClientCharConfig;

typedef struct _HtpThermometerParm
{
    // it is available only if measurement interval characteristic is supported
    U16 interval;

    // those two are available only if measurement interval characteristic is supported and writable
    U16 interval_range_min;
    U16 interval_range_max;

    // it is available only if temperature type characteristic is existed
    HtpTemperatureType temperature_type;
} HtpThermometerParm;

typedef struct _HtpServiceParm
{
    HtpServiceTypeFlag service_flag;
    HtpCallback callback;

    /* Only for Thermometer */
    HtpThermometerParm server_parms;
} HtpServiceParm;


/** HtpPacket structure
 * It is packet type used when adp layer transmits packet to profile
 */
/*typedef struct _HtpPacketParm
{
    union
    {
        HtpClientCharConfig 	flag;		//U16: 0x0001, 0x0002
        HtpMeasurementInterval	interval;	//U16
        HtpTemperature	*temperature;	
    } parm;
};*/

/*typedef struct _HtpQueryInfo
{
    HtpMeasurementInterval	interval;
    HtpMeasurementInterval	interval_range_min;
    HtpMeasurementInterval	interval_range_max;
    HtpTemperatureType	temperature_type;	
} HtpQueryInfo;*/

/* * HtpCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Various fields in the structure may be valid or not, depending
 *     on the "event" field.
 */
struct _HtpCallbackParms
{
    HtpEvent	event;	
    U32 		status;	
    U16 		len; 

    /* HtpEvent tells which characteristic it belongs to */
    union 
    {
        BtRemoteDevice	*rem_dev; 
        HtpCharFeature	rem_features;		// read attribute according to feature
        HtpClientCharConfig 	flag;		// U16
        HtpMeasurementInterval	interval;	// U16
        HtpTemperature	*temperature;	
        U8 	*pkt_handled;		// it is pointed to the handled packet
    } parm;
};


/*****************************************************************************
 * FUNCTION
 *  HTP_Register
 *
 * DESCRIPTION
 *  Collector: 
 *        Register application callback function.
 *  Thermometer: 
 *        Register GATT service record and callback function for both GATT and application.
 *
 *****************************************************************************/
BtStatus HTP_Register(HtpServiceParm *service);

/*****************************************************************************
 * FUNCTION
 *  HTP_Deregister
 *
 * DESCRIPTION
 *  Collector: 
 *        Deregister application callback function.
 *  Thermometer: 
 *        Deregister GATT service record and callback function.
 *
 *****************************************************************************/
BtStatus HTP_Deregister(HtpServiceTypeFlag service_flag);

/*****************************************************************************
 * FUNCTION
 *  HTP_Open
 *
 * DESCRIPTION
 *  Connect ATT bearer. Since Collector acts as GAP Central and Thermometer acts as
 *  Peripherial role, it is actually only available for Collector.
 *
 *****************************************************************************/
BtStatus HTP_Open(HtpChannel **channel, BD_ADDR *addr);

/*****************************************************************************
 * FUNCTION
 *  HTP_Close
 *
 * DESCRIPTION
 *  Disconnect ATT bearer for both Collector and Thermometer.
 *
 *****************************************************************************/
BtStatus HTP_Close(HtpChannel *channel);

/*****************************************************************************
 * FUNCTION
 *  HTPC_QueryGattService
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  After ATT bearer is established, try to initiate primary service and 
 *  characteristic discovery procedure and read procedure. Cache the 
 *  query result for the latest device.
 *
 *****************************************************************************/
void HTPC_QueryGattService(HtpChannel *channel);

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetTemperatureType
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetTemperatureType(HtpChannel *channel, HtpTemperatureType *type);

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetCharClientConfigDesciptor
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetCharClientConfigDesciptor(HtpChannel *channel, U16 char_uuid, HtpClientCharConfig *flag);

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetMeasurementIntervalValue
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetMeasurementIntervalValue(HtpChannel *channel, HtpMeasurementInterval *interval);

/*****************************************************************************
 * FUNCTION
 *  HTPC_GetMeasurementIntervalValue
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *
 *****************************************************************************/
BtStatus HTPC_GetMeasurementIntervalRange(HtpChannel *channel, HtpMeasurementInterval *interval_min, HtpMeasurementInterval *interval_max);

/*****************************************************************************
 * FUNCTION
 *  HTPC_SetCharClientConfigDesciptor
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write indication flag (0 or 1) in Client Characteristic Configuration
 *  Descriptor for Temperature Measurement.
 *
 *****************************************************************************/
BtStatus HTPC_SetCharClientConfigDesciptor(HtpChannel *channel, U16 char_uuid, U8 *flag);


/*****************************************************************************
 * FUNCTION
 *  HTPC_SetTemperatureIndication
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write indication flag (0 or 1) in Client Characteristic Configuration
 *  Descriptor for Temperature Measurement.
 *
 *****************************************************************************/
BtStatus HTPC_SetTemperatureIndication(HtpChannel *channel, U8 *flag);
#define HTPC_SetTemperatureIndication(ch, flag) HTPC_SetCharClientConfigDesciptor(ch, \
														BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT, \
														flag)


/*****************************************************************************
 * FUNCTION
 *  HTPC_SetIntervalIndication
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write indication flag (0 or 1) in Client Characteristic Configuration
 *  Descriptor for Measurement Interval.
 *
 *****************************************************************************/
BtStatus HTPC_SetIntervalIndication(HtpChannel *channel, U8 *flag);
#define HTPC_SetIntervalIndication(ch, flag) HTPC_SetCharClientConfigDesciptor(ch, \
														BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL, \
														flag)


/*****************************************************************************
 * FUNCTION
 *  HTPC_SetIntermediateTemperatureNotification
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write notification flag (0 or 1) in Client Characteristic Configuration
 *  Descriptor for Intermediate Temperature.
 *
 *****************************************************************************/
BtStatus HTPC_SetIntermediateTemperatureNotification(HtpChannel *channel, U8 *flag);
#define HTPC_SetIntermediateTemperatureNotification(ch, flag) HTPC_SetCharClientConfigDesciptor(ch, \
														BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE, \
														flag)


/*****************************************************************************
 * FUNCTION
 *  HTPC_SetMeasurementInterval
 *
 * DESCRIPTION
 *  It is for Collector only. 
 *  Write Measurement Interval value to Thermometer if the characteristic
 *  is available on Thermometer.
 *
 *****************************************************************************/
BtStatus HTPC_SetMeasurementInterval(HtpChannel *channel, U8 *interval);


/*****************************************************************************
 * FUNCTION
 *  HTPT_GetCharClientConfigDesciptor
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Since Client Configuration Descriptor is multiple instance, and it shall be saved for each
 *  bonded device. It is used to get characteristic client configuration descriptor for indicated
 *  device.
 *
 *****************************************************************************/
HtpClientCharConfig HTPT_GetCharClientConfigDesciptor(HtpChannel *channel, U16 char_uuid);

/*****************************************************************************
 * FUNCTION
 *  HTPT_SendTemperatureMeasurement
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Send Temperature Measurement indication to Collector.
 *
 *****************************************************************************/
BtStatus HTPT_SendTemperatureMeasurement(HtpChannel *channel, HtpTemperature *temperature, U16 size);

/*****************************************************************************
 * FUNCTION
 *  HTPT_SendIntermediateTemperature
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Send Intermediate Temperature notification to Collector.
 *
 *****************************************************************************/
BtStatus HTPT_SendIntermediateTemperature(HtpChannel *channel, HtpTemperature *temperature, U16 size);

/*****************************************************************************
 * FUNCTION
 *  HTPT_SetMeasurementInterval
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Thermometer may reset Measurement Interval value locally.
 *  It shall  
 *     a. save the interval value in profile layer; 
 *     b. if client characteristic configuration for that Collector is configured to indicated,
 *         thermometer shall indicate the current interval value the next time there is a connection.
 *         (Note that, since the client charatertistic configuration is configured for each bonded 
 *         device which is saved in app, the indication shall be checked and triggered by app)
 *
 *****************************************************************************/
BtStatus HTPT_SetMeasurementInterval(U16 interval);

/*****************************************************************************
 * FUNCTION
 *  HTPT_SendMeasurementIntervalIndication
 *
 * DESCRIPTION
 *  It is for Thermometer only. 
 *  Thermometer shall send Measurement Interval value indication, if interval value is
 *  reset locally and client characteristic configuration for that Collector is configured 
 *  to indicated.
 *
 *****************************************************************************/
BtStatus HTPT_SendMeasurementIntervalIndication(HtpChannel *channel);

#endif
