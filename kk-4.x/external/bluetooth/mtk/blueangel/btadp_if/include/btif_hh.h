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

#ifndef BTIF_HH_H
#define BTIF_HH_H

#include <hardware/bluetooth.h>
#include <hardware/bt_hh.h>
#include <stdint.h>

/*******************************************************************************
**  Constants & Macros
********************************************************************************/

#define BTIF_HH_MAX_HID         8
#define BTIF_HH_MAX_ADDED_DEV   32

#define BTIF_HH_MAX_KEYSTATES            3
#define BTIF_HH_KEYSTATE_MASK_NUMLOCK    0x01
#define BTIF_HH_KEYSTATE_MASK_CAPSLOCK   0x02
#define BTIF_HH_KEYSTATE_MASK_SCROLLLOCK 0x04

/*******************************************************************************
**  Type definitions and return values
********************************************************************************/

typedef enum
{
    BTIF_HH_DISABLED =   0,
    BTIF_HH_ENABLED,
    BTIF_HH_DISABLING,
    BTIF_HH_DEV_UNKNOWN,
    BTIF_HH_DEV_CONNECTING,
    BTIF_HH_DEV_CONNECTED,
    BTIF_HH_DEV_DISCONNECTED
} BTIF_HH_STATUS;




#endif
