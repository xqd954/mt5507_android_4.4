#ifndef __HDP_UTIL_H__
#define __HDP_UTIL_H__

#include "hdp.h"

/* HDP util functions */
BtStatus hdp_pool_init(void);
BtStatus hdp_pool_deinit(void);
BtStatus hdp_data_packat_list_int(void);
BtStatus hdp_data_packat_list_deint(void);
BtStatus hdp_data_packat_list_add(HdpDataPacket * pkt);
HdpDataPacket *hdp_data_packat_new(BD_ADDR *addr, U16 mdlid,void* value, U16 len);
BtStatus hdp_pool_pkt_add(HdpPkt* pkt);
BtStatus hdp_data_packat_delete(HdpDataPacket *pkt, BOOL forceDeleteData);
HdpPkt* hdp_pool_pkt_remove(void);
void hdp_data_packat_list_remove_by_channel(BD_ADDR *addr, U16 mdlid);
HdpChannel *hdp_find_channel_by_l2capId(U16 l2capId);
HdpChannel *hdp_find_channel_by_mdlId(BD_ADDR *bdaddr, U16 mdlId);
HdpChannel *hdp_find_channel_by_state(BD_ADDR *bdaddr, HdpChannelState state);
HdpChannel *hdp_find_channel_by_mdep_id(U8 mdepid);
HdpInstance* hdp_find_instance(MdepId id);
HdpInstance* hdp_find_echo_instance(HdpRole role);
HdpInstance* hdp_find_echo_instance_by_remote_config(HdpChannelType remotetype);
HdpDataPacket *hdp_data_packat_list_remove(void);
BOOL hdp_util_role_valid(HdpRole role);
BOOL hdp_channel_check_active(HdpChannel *channel);
MdepId hdp_assign_mdep_id(void);
HdpInstance* hdp_find_available_instance(void);
U8 hdp_util_convert_type2mode(HdpChannelType type);
U16 hdp_get_target_uuid(HdpRole role);
void *hdp_util_malloc(U32 size);
void hdp_util_free(void *value);
HdpMcl *hdp_mcl_new(BD_ADDR *bdaddr);
void hdp_mcl_delete(HdpMcl *mcl);
HdpMcl* hdp_find_mcl(BD_ADDR *bdaddr);
U16 hdp_util_min(U16 value1, U16 value2);
void hdp_set_channel_state(HdpChannel *channel, HdpChannelState state);
HdpInstance* hdp_find_registered_instance(HdpConfig *config);
BOOL hdp_check_outgoing_config(HdpRole localRole, HdpChannelType localSetting, HdpChannelType remoteSetting);
HdpChannelType hdp_convert_config_response(HdpRole localRole, HdpChannelType localSetting, HdpChannelType remoteSetting);
BOOL hdp_check_incoming_config(HdpRole localRole, HdpChannelType localSetting, HdpChannelType remoteSetting);
HdpChannelType hdp_get_local_config(HdpInstance *instance, BD_ADDR *bdaddr);
BOOL hdp_check_config_valid(HdpRole localRole, HdpChannelType localSetting);
void show_address(BD_ADDR *bdaddr);


#endif
