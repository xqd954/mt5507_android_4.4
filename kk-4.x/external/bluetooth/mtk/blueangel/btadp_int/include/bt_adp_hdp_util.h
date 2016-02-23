#ifndef __BT_ADP_HDP_UTIL_H__
#define __BT_ADP_HDP_UTIL_H__

#include "bt_adp_hdp.h"
#include "bluetooth_hdp_struct.h"
hdp_adp_instance_struct *hdp_util_find_available_instance();
hdp_adp_instance_struct *hdp_util_find_instance(U8 mdepId);
BOOL hdp_util_instance_exist(U8 mdepId);
hdp_adp_channel_struct *hdp_util_find_available_channel() ;
hdp_adp_channel_struct *hdp_util_find_channel_by_mdl_id(bt_addr_struct *addr, U16 mdlId);
hdp_adp_channel_struct *hdp_util_find_idle_channel_by_mdl_id(bt_addr_struct *addr, U16 mdlId);
hdp_adp_channel_struct *hdp_util_find_channel_by_index(bt_addr_struct *addr, U16 index);
hdp_adp_channel_struct *hdp_util_find_channel_by_state(bt_addr_struct *addr, U8 state);
hdp_adp_channel_struct *hdp_util_find_channel_by_l2cap_Id(U16 l2capId) ;
hdp_adp_channel_struct *hdp_util_find_channel_by_water_mark(U16 watermark);
hdp_adp_channel_struct *hdp_util_find_channel_by_session_id(U32 session_id);
hdp_adp_channel_struct *hdp_util_find_main_channel(bt_addr_struct *addr);
void hdp_util_add_channel(hdp_adp_channel_struct * channel);
void hdp_util_remove_channel(hdp_adp_channel_struct * channel);
U16 hdp_util_get_channel_water_mark();
bt_hdp_status hdp_util_convert_status(BtStatus status);
#endif
