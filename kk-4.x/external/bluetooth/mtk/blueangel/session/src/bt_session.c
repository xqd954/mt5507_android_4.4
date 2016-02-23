#include "bt_session.h"

#define SESSIONID_MASK 		0xFFFF0000
#define SUBSESSIONID_MASK 	0x0000FFFF
#define SESSION_ID_SHIFT	16
#define SUBSESSION_ID_SHIFT	0



static U16 get_session_id(U32 identify)
{
	return (identify & SESSIONID_MASK)>>SESSION_ID_SHIFT;
}

static U16 get_subsession_id(U32 identify)
{
	return (U16)((identify & SUBSESSIONID_MASK)>>SUBSESSION_ID_SHIFT);
}

static U32 get_session_index(U16 id, U16 subsessionid)
{
	return (id << SESSION_ID_SHIFT) | subsessionid;
}

SESSION_RESULT bt_session_setup()
{
	return session_setup(DEFAULT_SESSION_NAME, sizeof(DEFAULT_SESSION_NAME));
}

SESSION_RESULT bt_make_internal_session(U32 int_ids[2])
{
	U16 ids[2];
	U16 subids[2];
	SESSION_RESULT sret;
	sret = make_internal_session(DEFAULT_SESSION_NAME, sizeof(DEFAULT_SESSION_NAME), ids, subids);
	if (SR_NOERROR == sret)
	{
		int index;
		for(index = 0; index < 2; index ++)
		{
			int_ids[index] = get_session_index(ids[index], subids[index]);
		}
	}
	return sret;
}
SESSION_RESULT bt_session_connect(U16 sessionid, U32 *id)
{
	SESSION_RESULT sret;
	U16 subsessionid = 0;
	sret = session_connect(DEFAULT_SESSION_NAME, sizeof(DEFAULT_SESSION_NAME), sessionid, &subsessionid);
	if (sret == SR_NOERROR)
	{
		*id = get_session_index(sessionid, subsessionid);
	}
	return sret;
}

void bt_session_destroy(U32 id)
{
	session_destroy(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id));
}
SESSION_RESULT bt_session_register_handler(int start_id, 
											  int end_id, 
											  EventCallback 	callback,
											  U16 *hId)
{
	return session_register_handler(DEFAULT_SESSION_NAME, 
									sizeof(DEFAULT_SESSION_NAME), 
									start_id, 
									end_id, 
									callback,
									hId);
}
SESSION_RESULT bt_session_unregister_handler(U16 id)
{
	return session_unregister_handler(DEFAULT_SESSION_NAME, 
									sizeof(DEFAULT_SESSION_NAME), 
									id);
}
int bt_session_read(U32 id,  void *buffer, int size)
{
	return session_read(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id),
								buffer, 
								size);
}
int bt_session_write(U32 id, void *buffer, int size)
{
	return session_write(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id),
								buffer, 
								size);
}

int bt_session_write_control(U32 id, void *ctl, int ctllen, void *data, int datalen)
{
	return session_write_control(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id),
								ctl, 
								ctllen, 
								data, 
								datalen);
}

int bt_session_avaliable(U32 id, SESSION_DIRECTION direction, int *size)
{
	return session_avaliable(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id),
								direction, 
								size);
}

SESSION_RESULT bt_session_get_fd(U32 id, int *fd)
{
	return session_get_fd(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id),
								fd);
}
SESSION_RESULT bt_session_lock(U32 id, BT_BOOL lock)
{
	return session_lock(DEFAULT_SESSION_NAME, 
								sizeof(DEFAULT_SESSION_NAME),
								get_session_id(id),
								get_subsession_id(id),
								lock);
}







