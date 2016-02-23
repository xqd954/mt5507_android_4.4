#ifndef __BT_HDP_COMMON_H__
#define __BT_HDP_COMMON_H__

#include "bt_mmi.h"
#include "bluetooth_struct.h"
#include "bluetooth_hdp_struct.h"
#include "session_api.h"
#include "bt_ext_debug.h"
#include "string.h"
#include "bt_hdp_api.h"

//equal to definition in bt_adp_hdp.h
//TODO: should they be defined in bt_hdp_struct.h???
#define BTMTK_HDP_MAX_INSTANCE_NUM 	10
#define BTMTK_HDP_MAX_CHANNEL_NUM 	20


typedef struct _btmtk_hdp_config
{	
	BOOL				used;
	// exposed for external module
	int 				app_id;

	
	// used to label mdep id in peorfile layer
	U8					mdep_id;
	
	U8					mdep_cfg;
}btmtk_hdp_config;

typedef struct _btmtk_hdp_channel
{
	BOOL					used;
	
	btmtk_hdp_config		*config;
	bt_addr_struct			bdaddr;
	U8 						mdlId;

	// exposed for external module
	int						channel_id; 
	int						fd;

	U32						session_id;
} btmtk_hdp_channel;

typedef struct _btmtk_hdp_context
{
	BOOL					init;
	btmtk_hdp_config 		config[BTMTK_HDP_MAX_INSTANCE_NUM];
	btmtk_hdp_channel 		channel[BTMTK_HDP_MAX_CHANNEL_NUM];

	btmtk_hdp_cb			cb;
}btmtk_hdp_context;

int hdp_get_app_id();
int hdp_get_channel_id();
btmtk_hdp_config *hdp_ext_allocate_config();
btmtk_hdp_config *hdp_ext_find_config_by_app_id(int app_id);
btmtk_hdp_config *hdp_ext_find_config_by_mdep_id(U8 mdep_id);
btmtk_hdp_channel *hdp_ext_allocate_channel();
void hdp_ext_free_channel(btmtk_hdp_channel *channel);
btmtk_hdp_channel *hdp_ext_find_channel_by_id(int channel_id);
btmtk_hdp_channel *hdp_ext_find_channel_by_addr_and_mdl(bt_addr_struct *addr, U16 mdlid);
SESSION_RESULT hdp_ext_channel_attach(U32 *sessionid, int *sessionfd);
void hdp_ext_channel_detach(U32 sessionid);
BOOL hdp_update_channel_config(btmtk_hdp_channel *channel, U8 mdepid);
void btmtk_hdp_mmi_cb(void *context, BTMTK_HDP_EVENT event, U8 result);
void convert_bd_addr(bt_addr_struct* dest, U8 *src);
void revert_bd_addr(U8 *dest, bt_addr_struct *source);
void btmtk_hdp_handle_message(void *pContext, ilm_struct *ilm);
BOOL btmtk_hdp_connect_rsp(bt_addr_struct *addr, U16 mdlid, U32 sessionid);
#endif


