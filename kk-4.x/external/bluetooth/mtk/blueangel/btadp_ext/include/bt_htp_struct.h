/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_htp_struct.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is internal used header file for htp external adp layer
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#ifndef __BT_HTP_STRUCT_H__
#define __BT_HTP_STRUCT_H__

#include "bt_struct.h"
#include "bt_message.h"


typedef enum 
{
    BT_HTPC_OP_ACTIVATE = 0,
    BT_HTPC_OP_DEACTIVATE,
    BT_HTPC_OP_CONNECT,
    BT_HTPC_OP_DISCONNECT,
    BT_HTPC_OP_SET_CONFIG,
    BT_HTPC_OP_SET_INTERVAL_VALUE,
    BT_HTPC_OP_REMOVE_DEV,

    BT_HTPT_OP_ACTIVATE,
    BT_HTPT_OP_DEACTIVATE,
    BT_HTPT_OP_DISCONNECT,
    BT_HTPT_OP_SEND_TEMPERATURE,
    BT_HTPT_OP_SEND_INTERMEDIATE_TEMPERATURE,
    BT_HTPT_OP_SET_INTERVAL_VALUE,
    BT_HTPT_OP_SEND_INTERVAL_VALUE_INDICAITON,
} bt_htp_op_enum;

typedef enum
{
    BT_HTP_APP_STAT_NO_CONNECTION = 0,
    BT_HTP_APP_STAT_CONNECTED,	

    BT_HTP_APP_STAT_CONNECTING,	// only available for Collector
    BT_HTP_APP_STAT_DISCONNECTING,	
    BT_HTP_APP_STAT_CONN_CANCEL	// only available for Collector
} BT_HTP_APP_STATE;

typedef struct _BT_HTP_DEVICE
{
    BD_ADDR	addr;
    BT_HTP_APP_STATE	state;
} BT_HTP_DEVICE;

typedef struct _BT_HTPC_APP_CTX
{
    BT_BOOL 	enabled;
    BT_BOOL	do_disable;	
    BT_HTP_DEVICE	device[HTP_MAX_DEV_NUM];
} BT_HTPC_APP_CTX;

typedef struct _BT_HTPT_APP_CTX
{
    BT_BOOL 	enabled;
    BT_BOOL	do_disable;	
    BT_HTP_DEVICE	device;
} BT_HTPT_APP_CTX;

typedef struct
{
    LOCAL_PARA_HDR
    U8 op;
    BD_ADDR addr;
    U8 parm[0];	
} bt_htp_op_struct;


extern BTMTK_EventCallBack g_HTP_MMI_Callback;
extern BT_HTPC_APP_CTX g_htpc_cntx;
extern BT_HTPT_APP_CTX g_htpt_cntx;

void htpc_op_activate(void);
void htpc_op_deactivate(void);
void htpc_op_connect(BD_ADDR *bt_addr);
void htpc_op_disconnect(BD_ADDR *bt_addr);
void htpc_op_set_config(BD_ADDR *bt_addr, bt_htp_desc_config_struct *config);
void htpc_op_set_interval(BD_ADDR *bt_addr, U16 interval);
void htpc_op_remove_dev(BD_ADDR *bt_addr);


void htpt_op_activate(bt_htp_server_parm_struct *parm);
void htpt_op_deactivate(void);
void htpt_op_disconnect(BD_ADDR *bt_addr);
void htpt_op_send_temperature(BD_ADDR *bt_addr, bt_htp_temperature_struct *temperature);
void htpt_op_send_intermediate_temperature(BD_ADDR *bt_addr, bt_htp_temperature_struct *temperature);
void htpt_op_set_interval(U16 interval);
void htpt_op_send_interval_value_indication(BD_ADDR *bt_addr);

void htp_handle_cmd(bt_htp_op_struct *cmd, U32 size);

#endif
