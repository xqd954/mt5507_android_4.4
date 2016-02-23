#ifndef __BT_ADP_JSR82_SESSION_H__
#define __BT_ADP_JSR82_SESSION_H__

#include "bt_mmi.h"
#include "bt_session.h"
#include "bt_adp_jsr82.h"

BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_id(U32 id);
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_l2cap(U16 l2cap);
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_identify(U32 identify);
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_index(U8 session_index);
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_role_identify(BT_ADP_JSR82_SESSION_ROLE role, U32 identify);
BT_ADP_JSR82_SESSION *btadp_jsr82_find_session_by_role_index(BT_ADP_JSR82_SESSION_ROLE role, U8 session_index);
BT_ADP_JSR82_SESSION *btadp_jsr82_find_available_session();
void btadp_jsr82_session_enable_service(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_disable_service(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_connect(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_connect_rsp(BT_ADP_JSR82_SESSION *session, kal_uint8 result);
void btadp_jsr82_session_disconnect(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_turn_on_service(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_turn_off_service(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_send(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_receive(BT_ADP_JSR82_SESSION *session);
int btadp_jsr82_session_init(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_session_deinit(BT_ADP_JSR82_SESSION *session);
void btadp_jsr82_channel_connected(BT_ADP_JSR82_SESSION *session, void *ctl, int size);
void btadp_jsr82_session_disconnected(BT_ADP_JSR82_SESSION *session);
SESSION_RESULT jsr82_session_monitor(Session_Event_PARM *parm);


#endif
