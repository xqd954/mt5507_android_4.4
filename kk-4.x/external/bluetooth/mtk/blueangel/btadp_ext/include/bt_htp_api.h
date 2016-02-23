/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_htp_api.h
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is external adaptation layer header file for htp profie to send request
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
#ifndef __BT_HTP_API_H__
#define __BT_HTP_API_H__


#ifdef __BT_HTPC_PROFILE__
void btmtk_htpc_activate_req(void);
void btmtk_htpc_deactivate_req(void);
void btmtk_htpc_connect_req(U8 *device_addr);
void btmtk_htpc_disconnect_req(U8 *device_addr);
void btmtk_htpc_set_config_req(U8 *device_addr, bt_htp_desc_config_struct *config);
void btmtk_htpc_set_interval_value_req(U8 *device_addr, U16 interval);
void btmtk_htpc_remove_device_req(U8 *device_addr);
#endif

#ifdef __BT_HTPT_PROFILE__
void btmtk_htpt_activate_req(bt_htp_server_parm_struct *parms);
void btmtk_htpt_deactivate_req(void);
void btmtk_htpt_disconnect_req(U8 *device_addr);
void btmtk_htpt_send_temperature(U8 *device_addr, bt_htp_temperature_struct *temperature);
void btmtk_htpt_send_intermediate_temperature(U8 *device_addr, bt_htp_temperature_struct *temperature);
void btmtk_htpt_set_interval_value_req(U8 *device_addr, U16 interval);
#endif

void btmtk_htp_register_mmi_callback(BTMTK_EventCallBack callback);

#ifdef BTMTK_ON_LINUX
void btmtk_htp_set_sockfd(int sockfd);
#endif

#endif
