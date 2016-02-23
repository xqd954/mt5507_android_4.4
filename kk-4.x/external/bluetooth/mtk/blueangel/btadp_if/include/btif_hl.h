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

#ifndef BTIF_HL_H
#define BTIF_HL_H

/*******************************************************************************
**  Constants & Macros
********************************************************************************/

#define BTIF_HL_DATA_TYPE_NONE               0x0000
#define BTIF_HL_DATA_TYPE_PULSE_OXIMETER     0x1004   /* from BT assigned number */
#define BTIF_HL_DATA_TYPE_BLOOD_PRESSURE_MON 0x1007
#define BTIF_HL_DATA_TYPE_BODY_THERMOMETER   0x1008
#define BTIF_HL_DATA_TYPE_BODY_WEIGHT_SCALE  0x100F
#define BTIF_HL_DATA_TYPE_GLUCOSE_METER      0x1011
#define BTIF_HL_DATA_TYPE_STEP_COUNTER       0x1068

#define BTIF_HL_CCH_NUM_FILTER_ELEMS            3
#define BTIF_HL_APPLICATION_NAME_LEN          512

#define BTIF_HL_NV_MAX_APPS   16

#define BTMTK_HDP_MAX_NUM_MDEP	5
#define BTMTK_HDP_MAX_NUM_CHANNEL	7


/*******************************************************************************
**  Type definitions and return values
********************************************************************************/

typedef enum
{
    BTIF_HL_SOC_STATE_IDLE,
    BTIF_HL_SOC_STATE_W4_ADD,
    BTIF_HL_SOC_STATE_W4_CONN,
    BTIF_HL_SOC_STATE_W4_READ,
    BTIF_HL_SOC_STATE_W4_REL
} btif_hl_soc_state_t;

typedef enum
{
    BTIF_HL_STATE_DISABLED,
    BTIF_HL_STATE_DISABLING,
    BTIF_HL_STATE_ENABLED,
    BTIF_HL_STATE_ENABLING,
} btif_hl_state_t;

typedef enum
{
    BTIF_HL_CCH_OP_NONE,
    BTIF_HL_CCH_OP_MDEP_FILTERING,
    BTIF_HL_CCH_OP_MATCHED_CTRL_PSM,
    BTIF_HL_CCH_OP_DCH_OPEN,
    BTIF_HL_CCH_OP_DCH_RECONNECT,
    BTIF_HL_CCH_OP_DCH_ECHO_TEST
} btif_hl_cch_op_t;

typedef enum
{
    BTIF_HL_PEND_DCH_OP_NONE,
    BTIF_HL_PEND_DCH_OP_DELETE_MDL,
    BTIF_HL_PEND_DCH_OP_OPEN,
    BTIF_HL_PEND_DCH_OP_RECONNECT
} btif_hl_pend_dch_op_t;

typedef enum
{
    BTIF_HL_DCH_OP_NONE,
    BTIF_HL_DCH_OP_DISC
} btif_hl_dch_op_t;

typedef enum
{
    BTIF_HL_CHAN_CB_STATE_NONE,
    BTIF_HL_CHAN_CB_STATE_CONNECTING_PENDING,
    BTIF_HL_CHAN_CB_STATE_CONNECTED_PENDING,

    BTIF_HL_CHAN_CB_STATE_DISCONNECTING_PENDING,
    BTIF_HL_CHAN_CB_STATE_DISCONNECTED_PENDING,
    BTIF_HL_CHAN_CB_STATE_DESTROYED_PENDING,
} btif_hl_chan_cb_state_t;

enum
{
    BTIF_HL_SEND_CONNECTED_CB,
    BTIF_HL_SEND_DISCONNECTED_CB,
    BTIF_HL_REG_APP,
    BTIF_HL_UNREG_APP,
    BTIF_HL_UPDATE_MDL,
};




#endif
