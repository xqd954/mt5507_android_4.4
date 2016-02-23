/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

#ifndef BT_UTIL_H
#define BT_UTIL_H

#include <stdlib.h>
#include <hardware/bluetooth.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>

#include "bt_ext_common.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"

kal_bool btmtk_util_equal_bdaddr(bt_addr_struct *addr1, bt_addr_struct *addr2);

kal_bool btmtk_check_bdaddr(bt_addr_struct *addr);

void btmtk_util_reverse_array(U8 *dest, const U8 *src);

void btmtk_util_convert_bdaddr2array(bt_bdaddr_t *dest, bt_addr_struct *source);

void btmtk_util_convert_array2bdaddr(bt_addr_struct *dest, bt_bdaddr_t *source);

void btmtk_util_convert_bdaddr2string(char *dest, bt_addr_struct *source);

void btmtk_util_convert_string2bdaddr(const char *source, bt_addr_struct *dest);

void btmtk_util_convert_array2string(char *dest, bt_bdaddr_t *source);

void btmtk_util_uuid16_to_uuid128(kal_uint16 uuid16, bt_uuid_t* uuid128);

/*
UTF-8 valid format list:
0xxxxxxx
110xxxxx 10xxxxxx
1110xxxx 10xxxxxx 10xxxxxx
11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
int btmtk_util_utf8_endpos(char *str, int len);

int btmtk_util_convert_uuidlist_2_uuid16( unsigned long service_list1,  /* 0x1100 ~ 0x111F */
    										   unsigned long  service_list2,  /* 0x1120 ~ 0x113F */
    										   unsigned long  service_list3,  /* 0x1200 ~ 0x121F */
    										   unsigned long  service_list4,  /* 0x1300~ */
    										   unsigned long  service_list5,
    										   unsigned long  service_list6,
    										   unsigned short uuid[]);

void btmtk_util_list_append(btmtk_list_header_struct **list, btmtk_list_header_struct *node);

void btmtk_util_list_remove(btmtk_list_header_struct **list, btmtk_list_header_struct *node);

#endif
