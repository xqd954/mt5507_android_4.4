/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmp.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile external used header file.
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
#ifndef __FMP_H__
#define __FMP_H__

#include <bttypes.h>
#include <conmgr.h>
#include "gatt.h"


/*---------------------------------------------------------------------------
 * FmpEvent type
 *
 *     All event indications and confirmations are passed to a callback
 *     function of type FmpCallback. The "FmpCallbackParms.event" field will
 *     contain one of the event types below.
 */
typedef U8 FmpEvent;

/** A connection is open.  This event can be received as the result of a call to 
 *  FMP_Open or GATT connected by remote device.  When this event has been 
 *  received, the channel is available.  When this event is received in the callback, 
 *  the "ptrs.remDev" field contains a pointer to the remote device.
 */
#define FMPEVENT_OPEN	0x00		

/** The connection is closed.  This can come as a result of calling 
 *  FMP_Close or if the remote device has closed the connection, 
 *  When this event is received, the "ptrs.remDev" field contains 
 *  a pointer to the remote device.
 */
#define FMPEVENT_CLOSED	0x01		

/** It is for Locator only.
 *  This event can be received as the result of a call to FMPL_QueryGattService. 
 *  No "ptrs" field is included.
 */
#define FMPEVENT_GATT_QUERY_CNF	0x02		

/** It is for Locator only.
 *  This event can be received as the result of a call to FMPL_SetAlertLevel. 
 *  "ptrs.level" field is included.
 */
#define FMPEVENT_SET_ALERT_LEVEL_CNF    0x03

/** It is for Target only.
 *  This event can be received if receive write alert level request. 
 *  "ptrs.level" field is included.
 */
#define FMPEVENT_SET_ALERT_LEVEL_REQ    0x04


typedef U8 FmpServiceTypeFlag;

#define FMP_SERVICE_TYPE_LOCATOR_FLAG    0x01
#define FMP_SERVICE_TYPE_TARGET_FLAG    0x02



/* Foreward References */
typedef struct _FmpCallbackParms FmpCallbackParms;
typedef struct _FmpChannel FmpChannel;

/*---------------------------------------------------------------------------
 * FmpCallback type
 *
 *     Represents a callback function called by FMP to indicate events and 
 *     data to the application. The event is targeted to the FMP connection
 *     identified by the "Channel" parameter. Information about the 
 *     event is contained in the "Parms" parameter.
 */
typedef void (*FmpCallback)(FmpChannel *Channel, FmpCallbackParms *Parms);


/** FmpAlertLevel
*  The level of an alert a device to sound.
*   Value 0, meaning "No Alert"
*   Value 1, meaning "Mild Alert"
*   Value 2, meaning "High Alert"
*/
typedef U8 FmpAlertLevel;

#define FMP_ALERT_LEVEL_NONE    0
#define FMP_ALERT_LEVEL_MILD     1
#define FMP_ALERT_LEVEL_HIGH    2


/* * FmpCallbackParms structure
 *
 *     Describes a callback event and any data that relates to the event.
 *     Various fields in the structure may be valid or not, depending
 *     on the "event" field.
 */
struct _FmpCallbackParms
{
    FmpEvent	event;	
    U32 		status;	
    U16 		len; 

    /* FmpEvent tells which characteristic it belongs to */
    union 
    {
        BtRemoteDevice	*rem_dev; 
        FmpAlertLevel   level;	
    } parm;
};


BtStatus FMP_Register(FmpCallback callback, FmpServiceTypeFlag service_flag);

BtStatus FMP_Deregister(FmpServiceTypeFlag service_flag);

BtStatus FMP_Open(FmpChannel **channel, BD_ADDR *addr);

BtStatus FMP_Close(FmpChannel *channel);

void FMPL_QueryGattService(FmpChannel *channel);

BtStatus FMPL_SetAlertLevel(FmpChannel *channel, FmpAlertLevel level);

#endif
