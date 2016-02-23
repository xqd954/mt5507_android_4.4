#ifndef __BT_HDP_API_H__
#define __BT_HDP_API_H__

#include "bluetooth_hdp_struct.h"

#ifndef BOOL
typedef unsigned char BOOL;
#endif

typedef enum _BTMTK_HDP_EVENT
{
	BTMTK_HDP_EVENT_REGISTERED,
	BTMTK_HDP_EVENT_DEREGISTERED,
	BTMTK_HDP_EVENT_CONNECT_CNF,
	BTMTK_HDP_EVENT_OPEN_IND,
	BTMTK_HDP_EVENT_DISCONNECT_CNF,
	BTMTK_HDP_EVENT_CLOSE_IND,
}BTMTK_HDP_EVENT;

typedef enum _BTMTK_HDP_RESULT
{
	BTMTK_HDP_RESULT_SUCCES,
	BTMTK_HDP_RESULT_FAIL,
}BTMTK_HDP_RESULT;


typedef struct _btmtk_hdp_cb_param
{
	BTMTK_HDP_EVENT 	event;
	BTMTK_HDP_RESULT 	result;
	int					app_id;

	U8					bd_addr[6];
	int 				mdep_cfg_index;
    int 				channel_id;
	int 				fd;
} btmtk_hdp_cb_param;

typedef void (*btmtk_hdp_cb)(btmtk_hdp_cb_param *);

BOOL btmtk_hdp_register_instance(bt_hdp_role role, 
										bt_hdp_channel_type channelType, 
										U16 dataType,
										const char *description,
										int *app_id);
BOOL btmtk_hdp_deregister_instance(int app_id);
BOOL btmtk_hdp_connect(U8 *addr, int app_id, U8 config, int *index);
BOOL btmtk_hdp_disconnect(U32 index);
void btmtk_hdp_init(btmtk_hdp_cb cb);
void btmtk_hdp_deinit();


#endif

