/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/


/*****************************************************************************
**
**  Name:          btif_gatt.h
**
**  Description:
**
******************************************************************************/

#ifndef BTIF_GATT_H
#define BTIF_GATT_H


//#define __REVERSE_UUID__

// GATT Error code
#define  GATT_SUCCESS                        0x0000
#define  GATT_INVALID_HANDLE                 0x0001
#define  GATT_READ_NOT_PERMIT                0x0002
#define  GATT_WRITE_NOT_PERMIT               0x0003
#define  GATT_INVALID_PDU                    0x0004
#define  GATT_INSUF_AUTHENTICATION           0x0005
#define  GATT_REQ_NOT_SUPPORTED              0x0006
#define  GATT_INVALID_OFFSET                 0x0007
#define  GATT_INSUF_AUTHORIZATION            0x0008
#define  GATT_PREPARE_Q_FULL                 0x0009
#define  GATT_NOT_FOUND                      0x000a
#define  GATT_NOT_LONG                       0x000b
#define  GATT_INSUF_KEY_SIZE                 0x000c
#define  GATT_INVALID_ATTR_LEN               0x000d
#define  GATT_ERR_UNLIKELY                   0x000e
#define  GATT_INSUF_ENCRYPTION               0x000f
#define  GATT_UNSUPPORT_GRP_TYPE             0x0010
#define  GATT_INSUF_RESOURCE                 0x0011

#define  GATT_ILLEGAL_PARAMETER              0x0087
#define  GATT_NO_RESOURCES                   0x0080
#define  GATT_INTERNAL_ERROR                 0x0081
#define  GATT_WRONG_STATE                    0x0082
#define  GATT_DB_FULL                        0x0083
#define  GATT_BUSY                           0x0084
#define  GATT_ERROR                          0x0085
#define  GATT_CMD_STARTED                    0x0086
#define  GATT_PENDING                        0x0088
#define  GATT_AUTH_FAIL                      0x0089
#define  GATT_MORE                           0x008a
#define  GATT_INVALID_CFG                    0x008b
#define  GATT_SERVICE_STARTED                0x008c
#define  GATT_ENCRYPED_MITM                  GATT_SUCCESS
#define  GATT_ENCRYPED_NO_MITM               0x008d
#define  GATT_NOT_ENCRYPTED                  0x008e


#define GATT_IF_API()   ALOGD("[GATT] %s", __FUNCTION__)
#define GATT_IF_DBG(fmt, ...)   ALOGD("[GATT] " fmt, ## __VA_ARGS__)
#define GATT_IF_WARN(fmt, ...)   ALOGW("[GATT][WARN] %s#%d : " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define GATT_IF_ERR(fmt, ...)   ALOGE("[GATT][ERR] %s#%d : " fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__)

#define GATT_MAX_CLIENT 10
#define GATT_MAX_SERVER 10

#define GET_IF_FROM_CONNID(conn_id) ((conn_id>>16) & 0xFFFF)
#define GET_CONN_FROM_CONNID(conn_id) (conn_id & 0xFFFF)
#define GET_CONNID(reg_if, conn) (((reg_if<<16) & 0xFFFF0000) | ((unsigned int)conn & 0xFFFF))

/**************************************************
*   Utility API
**************************************************/
int convertToGattErrCode(uint8_t result);
void gattDumpUUID(char *prefix, uint8_t len, uint8_t *uuid);
U8 convertBtIdToBTUUID(BT_UUID *dest, bt_uuid_t *src);
U8 convertBTUUIDToBtId(bt_uuid_t *dest, BT_UUID *src);
U8 convertCharIdToAttUUID(ATT_UUID *dest, btgatt_gatt_id_t *src);
U8 convertAttUUIDToCharId(btgatt_gatt_id_t *dest, ATT_UUID *src);
U8 convertSvcIdToSvcUUID(GATT_SVC_UUID *dest, btgatt_srvc_id_t *src);
U8 convertSvcUUIDToSvcId(btgatt_srvc_id_t *dest, GATT_SVC_UUID *src);

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

#endif

