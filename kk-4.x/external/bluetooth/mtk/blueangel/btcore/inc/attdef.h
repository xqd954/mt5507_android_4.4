/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * attdef.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 * Attribute protocol definition
 *
 * Author:
 * -------
 * Dlight Ting (mtk01239)
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
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
 * 08 01 2011 autumn.li
 * [ALPS00064244] [BT] [BLE] ATT/GATT revision
 * ATT/GATT revision
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef __ATTDEF_H
#define __ATTDEF_H

#include "me_adp.h"

#define ATT_PERMISSIONS_READ_ONLY    (0x00)
#define ATT_PERMISSIONS_WRITE_ONLY   (0x01)
#define ATT_PERMISSIONS_READ_WRITE   (0x02)
#define ATT_PERMISSIONS_NO_READ_WRITE                   (0x03)
#define ATT_PERMISSIONS_GATT_DEFINED                    (0x04)  /* defined in Charateristic Properties */
#define ATT_PERMISSIONS_READ_ONLY_BLE_ONLY              (0x05)
#define ATT_PERMISSIONS_WRITE_ONLY_BLE_ONLY             (0x06)
#define ATT_PERMISSIONS_READ_WRITE_BLE_ONLY             (0x07)
#define ATT_PERMISSIONS_NO_READ_WRITE_BLE_ONLY          (0x08)
#define ATT_PERMISSIONS_GATT_DEFINED_BLE_ONLY           (0x09)  /* defined in Charateristic Properties */



#define ATT_SECURITY_READ_NOT_REQUIRED          (0x00)
#define ATT_SECURITY_READ_AUTHENTICATION        (0x01)
#define ATT_SECURITY_READ_ENCRYPTION            (0x02)
#define ATT_SECURITY_READ_AUTHORIZE             (0x03)

#define ATT_SECURITY_WRITE_NOT_REQUIRED          (0x00)
#define ATT_SECURITY_WRITE_AUTHENTICATION        (0x01)
#define ATT_SECURITY_WRITE_ENCRYPTION            (0x02)
#define ATT_SECURITY_WRITE_AUTHORIZE             (0x03)


/* To modified the code, need to change the ATTDB_NotifyBondStatus function also */
#define ATT_SEC_DEVICE_BOND               (0x01)
#define ATT_SEC_DEVICE_UNBOND             (0x02)
#define ATT_SEC_DEVICE_ALL_UNBOND         (0x03)


#define BT_UUID_GATT_SERVICE_GAP           0x1800
#define BT_UUID_GATT_SERVICE_GATT          0x1801
#define BT_UUID_GATT_SERVICE_ALERT         0x1802
#define BT_UUID_GATT_SERVICE_LINKLOSS      0x1803
#define BT_UUID_GATT_SERVICE_TXPOWER       0x1804
#define BT_UUID_GATT_SERVICE_TIME          0x1805
#define BT_UUID_GATT_SERVICE_TIMEUPDATE    0x1806
#define BT_UUID_GATT_SERVICE_DST           0x1807
#define BT_UUID_GATT_SERVICE_HEALTH        0x1809
#define BT_UUID_GATT_SERVICE_DEVICE        0x180A
#define BT_UUID_GATT_SERVICE_NETWORK       0x180B
#define BT_UUID_GATT_SERVICE_WATCHDOG      0x180C
#define BT_UUID_GATT_SERVICE_HEART         0x180D
#define BT_UUID_GATT_SERVICE_PHONEALERT    0x180E
#define BT_UUID_GATT_SERVICE_BATTERY       0x180F
#define BT_UUID_GATT_SERVICE_BLOODPRESSURE 0x1810
#define BT_UUID_GATT_SERVICE_ALERTNOTIFY    0x1811
#define BT_UUID_GATT_SERVICE_HID                0x1812
#define BT_UUID_GATT_SERVICE_SCANPARAM      0x1813

#define BT_UUID_GATT_TYPE_PRIMARY_SERVICE   0x2800
#define BT_UUID_GATT_TYPE_SECONDARY_SERVICE   0x2801
#define BT_UUID_GATT_TYPE_INCLUDE               0x2802
#define BT_UUID_GATT_TYPE_CHARACTERISTIC        0x2803

#define BT_UUID_GATT_CHAR_DESC_CHAR_EXTEND_PROPERTY          0x2900
#define BT_UUID_GATT_CHAR_DESC_CHAR_USER_DESC                0x2901
#define BT_UUID_GATT_CHAR_DESC_CLIENT_CHAR_CONFIG            0x2902
#define BT_UUID_GATT_CHAR_DESC_SERVER_CHAR_CONFIG            0x2903
#define BT_UUID_GATT_CHAR_DESC_CHAR_FORMAT                  0x2904
#define BT_UUID_GATT_CHAR_DESC_CHAR_AGGREGATE_FORMAT        0x2905

/* Additional characteristic descriptors defined by higher layer profiles */
#define BT_UUID_HTS_CHAR_DESC_VALID_RANGE			0x2906

#define BT_UUID_GATT_CHAR_TYPE_DEVICE_NAME                  0x2A00
#define BT_UUID_GATT_CHAR_TYPE_APPEARANCE                   0x2A01
#define BT_UUID_GATT_CHAR_TYPE_PERIPHERAL_PRIVACY_FLAG      0x2A02
#define BT_UUID_GATT_CHAR_TYPE_RECONNECTION_ADDRESS         0x2A03
#define BT_UUID_GATT_CHAR_TYPE_PERIPHERAL_PREFER_CONNECT_PARM        0x2A04
#define BT_UUID_GATT_CHAR_TYPE_SERIVCE_CHANGED              0x2A05


#define BT_UUID_PROXIMITY_ALERT_LEVEL                       0x2A06 /* todo */
#define BT_UUID_PROXIMITY_TXPOWER_LEVEL                     0x2A07 /* todo */


/* For characteristics of Time profile */
#define BT_UUID_CT_TIME								0x2A2B
#define BT_UUID_LOCAL_TIME_INFO						0x2A0F
#define BT_UUID_REF_TIME_INFO						0x2A14
#define BT_UUID_TIME_WITH_DST						0x2A11
#define BT_UUID_TIME_UPDATE_CONTROL_POINT			0x2A16
#define BT_UUID_TIME_UPDATE_STATE					0x2A17

/* For characteristics defined by Health Thermometer Service */
#define BT_UUID_HTS_CHAR_TEMPERATURE_MEASUREMENT	0x2A1C			
#define BT_UUID_HTS_CHAR_TEMPERATURE_TYPE			0x2A1D
#define BT_UUID_HTS_CHAR_INTERMEDIATE_TEMPERATURE	0x2A1E
#define BT_UUID_HTS_CHAR_MEASUREMENT_INTERVAL		0x2A21

/* For characteristics defined by Device Information Service */
#define BT_UUID_DIS_CHAR_MANUFACTURER_NAME			0x2A29
#define BT_UUID_DIS_CHAR_MODEL_NUM					0x2A24
#define BT_UUID_DIS_CHAR_SERIAL_NUM					0x2A25
#define BT_UUID_DIS_CHAR_HW_REV						0x2A27
#define BT_UUID_DIS_CHAR_FM_REV						0x2A26
#define BT_UUID_DIS_CHAR_SW_REV						0x2A28
#define BT_UUID_DIS_CHAR_SYSTEM_ID					0x2A23
#define BT_UUID_DIS_CHAR_IEEE_DATA_LIST				0x2A2A
#define BT_UUID_DIS_CHAR_PNP_ID                                                 0x2A50

/* For characteristics defined by Heart Rate Service */
#define BT_UUID_HRS_CHAR_HEARTRATE_MEASUREMENT              0x2A37
#define BT_UUID_HRS_CHAR_BODY_SENSOR_LOCATION                   0x2A38
#define BT_UUID_HRS_CHAR_HEARTRATE_CONTROL_POINT            0x2A39

/* For characteristics defined by Human Interface Device Service */
#define BT_UUID_HID_CHAR_PROTOCOL_MODE                  0x2A4E
#define BT_UUID_HID_CHAR_REPORT                                 0x2A4D
#define BT_UUID_HID_CHAR_REPORT_MAP                         0x2A4B
#define BT_UUID_HID_CHAR_BOOT_KEYBOARD_INPUT      0x2A22
#define BT_UUID_HID_CHAR_BOOT_KEYBOARD_OUTPUT     0x2A32
#define BT_UUID_HID_CHAR_BOOT_MOUSE_INPUT               0x2A33
#define BT_UUID_HID_CHAR_INFORMATION                        0x2A4A
#define BT_UUID_HID_CHAR_CONTROL_POINT                  0x2A4C

/* For characteristic descriptors defined by Human Interface Device Service */
#define BT_UUID_HID_CHAR_DESC_REPORT_REF          0x2908
#define BT_UUID_HID_CHAR_DESC_EXT_REPORT_REF          0X2907


/* For characteristics defined by Battery Service */
#define BT_UUID_BATTERY_CHAR_LEVEL                          0x2A19
#define BT_UUID_BATTERY_CHAR_POWER_STATE            0x2A1A

/* For characteristics defined by Scan Parameter Service */
#define BT_UUID_SCAN_CHAR_INTERVAL_WINDOW       0x2A4F
#define BT_UUID_SCAN_CHAR_REFRESH                           0x2A31



#define BT_ATT_CONST_VARIABLE               0x00
#define BT_ATT_FIX_VARIABLE                 0x01
#define BT_ATT_NON_FIX_VARIABLE             0x02
#define BT_ATT_FIX_VARIABLE_IN_UPPER_AP     0x03

typedef enum
{
    ATT_OP_BROADCAST,
    ATT_OP_READ,
    ATT_OP_READ_LONG,
    ATT_OP_READ_MULTIPLE,
    ATT_OP_READ_BY_TYPE,
    ATT_OP_WRITE_WO_RESPONSE,
    ATT_OP_WRITE,
    ATT_OP_PREPARE_WRITE,
    ATT_OP_EXECUTE_WRITE,
    ATT_OP_EXECUTE_WRITE_CANCEL,
    ATT_OP_NOTIFY,
    ATT_OP_INDICATE,
    ATT_OP_SIGNED_WRITE,
    ATT_OP_UNKNOWN,
} bt_att_op;

typedef enum
{
    GATT_CHAR_PROP_BROADCAST = 0x01,
    GATT_CHAR_PROP_READ = 0x02,
    GATT_CHAR_PROP_WRITE_WO_RESPONSE = 0x04,
    GATT_CHAR_PROP_WRITE = 0x08,
    GATT_CHAR_PROP_NOTIFY = 0x10,
    GATT_CHAR_PROP_INDICATE = 0x20,
    GATT_CHAR_PROP_SIGNED_WRITE = 0x40,
    GATT_EXT_PROP = 0x80
} bt_gatt_char_properties;


#define BT_UUID16_SIZE  (0x02)
#define BT_UUID128_SIZE (0x10)

/* Handle(2B) 0x02(1B) CONST_VALUE(1B) VALUESIZE(2B) UUID(2) */
#define ATT_HANDLE16(HANDLE, UUID, CONST_VALUE, VALUE_SIZE)                                              \
            (U8)(((HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
            BT_UUID16_SIZE,  /* SIZE Include Header */      \
            (U8)(CONST_VALUE), \
            (U8)(((VALUE_SIZE) & 0xff00) >> 8),        \
            (U8)((VALUE_SIZE) & 0x00ff),        \
            (U8)(((UUID) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((UUID) & 0x00ff)       /* Bits[7:0] of UUID */

/* Handle(2B) 0x10(1B) CONST_VALUE(1B) VALUESIZE(2B) UUID(2) */
#define ATT_HANDLE128(HANDLE, UUID, CONST_VALUE, VALUE_SIZE)                                              \
                (U8)(((HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
                (U8)((HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
                BT_UUID128_SIZE,        \
                (U8)(CONST_VALUE), \
                (U8)(((VALUE_SIZE) & 0xff00) >> 8),        \
                (U8)((VALUE_SIZE) & 0x00ff),            \
                UUID        /* 128-bit UUID */

#define ATT_GROUP(START_HANDLE, END_HANDLE, UUID) \
            (U8)(((START_HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((START_HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
            (U8)(((END_HANDLE) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((END_HANDLE) & 0x00ff),       /* Bits[7:0] of UUID */ \
            UUID        /* 128-bit UUID */


/* PERS(1B) */
#define ATT_PERMINSIONS(PERS, READ_SECURITY, READ_KEY_SIZE, WRITE_SECURITY, WRITE_KEY_SIZE)     \
                       (U8) (PERS), \
                       (U8) (READ_SECURITY & ((READ_KEY_SIZE) << 4)), \
                       (U8) (WRITE_SECURITY & ((WRITE_KEY_SIZE) << 4))       
                                
#define ATT_SETHANDLE(HDL)                                              \
            (U8)((HDL) & 0xff00),             /* Bits[7:0] of HDL */   \
    		(U8)(((HDL) & 0x00ff) >> 8)	 /* Bits[15:8] of HDL */	                              

#define ATT_SETUUID16(uuid)                                              \
            (U8)((uuid) & 0x00ff),             /* Bits[7:0] of UUID */   \
			(U8)(((uuid) & 0xff00) >> 8)	 /* Bits[15:8] of UUID */	  


/* PROP(1B) REC_HANDLE(2B) UUID(2B) */
#define ATT_SETCHARACTERISTIC(PROP, REC_HANDLE, UUID)                   \
                                (U8) (PROP),                            \
                                (U8)((REC_HANDLE) & 0xff),     \
                                (U8)(((REC_HANDLE) & 0xff00) >> 8),             \
                                (U8)( ((UUID) & 0xff) ),           \
                                (U8)(((UUID) & 0xff00) >> 8)



#define ATT_SETCHARACTERISTIC128(PROP, REC_HANDLE, UUID)                   \
                                                                (U8) (PROP),                            \
                                                                (U8)((REC_HANDLE) & 0xff),     \
                                                                (U8)(((REC_HANDLE) & 0xff00) >> 8),             \
                                                                UUID
                                                                
#define ATT_SETINCLUDE16(HDL_START, HDL_END, UUID)                        \
                (U8)((HDL_START) & 0x00ff),             /* Bits[7:0] of UUID */   \
                (U8)(((HDL_START) & 0xff00) >> 8),     /* Bits[15:8] of UUID */    \
                (U8)((HDL_END) & 0x00ff),             /* Bits[7:0] of UUID */   \
                (U8)(((HDL_END) & 0xff00) >> 8),     /* Bits[15:8] of UUID */     \
                (U8)((UUID) & 0x00ff),             /* Bits[7:0] of UUID */   \
                (U8)(((UUID) & 0xff00) >> 8)     /* Bits[15:8] of UUID */     
                
                                                                
#define ATT_SETINCLUDE128(HDL_START, HDL_END)                        \
                                (U8)((HDL_START) & 0x00ff),             /* Bits[7:0] of UUID */   \
                                (U8)(((HDL_START) & 0xff00) >> 8),     /* Bits[15:8] of UUID */  \
                                (U8)((HDL_END) & 0x00ff),             /* Bits[7:0] of UUID */   \
                                (U8)(((HDL_END) & 0xff00) >> 8)     /* Bits[15:8] of UUID */    
                                

#define ATT_SETEXTENDEDPROPERTIES(PROP)                        \
                    (U8)((PROP) & 0x00ff),             /* Bits[7:0] of UUID */   \
                    (U8)(((PROP) & 0xff00) >> 8)     /* Bits[15:8] of UUID */   


#define ATT_SETCHARFORMAT(FORMAT, EXP, UNIT, N_SP, DESCR)                        \
                        (U8) (FORMAT),\
                        (U8) (EXP), \
                        (U8)((UNIT) & 0x00ff),             /* Bits[7:0] of UUID */   \
                        (U8)(((UNIT) & 0xff00) >> 8),     /* Bits[15:8] of UUID */ \
                        (U8) (N_SP), \
                        (U8)((DESCR) & 0x00ff),             /* Bits[7:0] of UUID */   \
                        (U8)(((DESCR) & 0xff00) >> 8)   /* Bits[15:8] of UUID */   

typedef U8 (*AttdbCallback) (U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw);

typedef void (*AttBondCallback) (U8 type, const U8 *bd_addr);


typedef struct _BtBleBondCbHandler
{
    ListEntry node;         	/* For internal stack use only. */
    AttBondCallback callback;    /* Pointer to callback function */
} BtBleBondCbHandler;

#endif  /* __ATTDEF_H */
