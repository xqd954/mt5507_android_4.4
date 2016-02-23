#ifndef __BT_SESSION_H__
#define __BT_SESSION_H__

#include "session_api.h"
SESSION_RESULT bt_make_internal_session(U32 int_ids[2]);
SESSION_RESULT bt_session_setup();
SESSION_RESULT bt_session_connect(U16 sessionid, U32 *id);

void bt_session_destroy(U32 id);

SESSION_RESULT bt_session_register_handler(int start_id, 
												  int end_id, 
												  EventCallback 	callback,
												  U16 *hId);

SESSION_RESULT bt_session_unregister_handler(U16 id);
int bt_session_read(U32 id, void *buffer, int size);

int bt_session_write(U32 id, void *buffer, int size);
int bt_session_write_control(U32 id, void *ctl, int ctllen, void *data, int datalen);

int bt_session_avaliable(U32 id, SESSION_DIRECTION direction, int *size);
SESSION_RESULT bt_session_get_fd(U32 id, int *fd);
SESSION_RESULT bt_session_lock(U32 id, BT_BOOL lock);
#endif

