#include "session.h" 
#include "session_master.h" 


/**********************************/
/****** public client interface    *********/
/**********************************/

static Session_Manager session_manager = {.init = FALSE};
static SESSION_RESULT add_group(char* name, int namesize, Session_Group **group);

SESSION_RESULT remove_group(Session_Group *group);
static SESSION_RESULT find_group(char*	name, int	namesize, Session_Group **group);





//TODO: add lock for this interface
static void session_manager_init()
{
	SESSION_RESULT sret = SR_NOERROR;
	if(session_manager.init)
	{
		return;
	}
	memset(&session_manager, 0x0, sizeof(Session_Manager));
	
	sret = session_mutex_create(&session_manager.mutex);
	if (SR_NOERROR != sret || NULL == session_manager.mutex)
	{
		return;
	}
	session_mutex_lock(session_manager.mutex);

	//double check
	
	session_manager.init = TRUE;
	session_mutex_unlock(session_manager.mutex);
}

/**********************************/
/****** public client interface    *********/
/**********************************/


//TODO: make relationship between twin sessions,
//		when one is destroyed, destroy the other
SESSION_RESULT make_internal_session(char *name, int namesize, U16 ids[2], U16 subsessionids[2])
{
	Session_Context 		*session = NULL;
	Session_Group 			*group = NULL;
	char 					sessionname[SESSION_NAME_MAX_LENGTH];
	char 					peername[SESSION_NAME_MAX_LENGTH];
	int 					fds[2];
	int						index = 0;
	U16						id = 0;
	U16 					subid = 0;
	SESSION_RESULT 			sret = SR_NOERROR;

	session_log("[SESSION]make_internal_session, group name[ %s]", name);
	session_manager_init();
	
	session_mutex_lock(session_manager.mutex);

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		sret = add_group(name, namesize, &group);
		if (sret != SR_NOERROR)
		{
			goto exit;
		}
	}

	sret = group->stream->create_int_stream(fds);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]create server session name failed %d", sret);
		goto exit;
	}

	for (index = 0; index < 2; index ++)
	{
		memset(sessionname, 0x0, SESSION_NAME_MAX_LENGTH);
		memset(peername, 0x0, SESSION_NAME_MAX_LENGTH);

		session = NULL;		

		sret = get_default_subsession_id(group, &subid);
		if (SR_NOERROR != sret)
		{
			session_log("[Session]fail to get default subsession id[%d]", sret);
			break;
		}
		
		sret = get_reserved_session_id(group, &id);
		if (SR_NOERROR != sret)
		{
			session_log("[Session]fail to get reserved session id[%d]", sret);
			break;
		}
		
		sret = allocate_session(&session);
		if (SR_NOERROR != sret)
		{
			session_log("[Session]no available resource for new session");
			break;
		}

		sret = compose_client_session_name(name, 
										   namesize,
										   id, 
										   sessionname, 
										   SESSION_NAME_MAX_LENGTH,
										   subid);
		if (SR_NOERROR != sret)
		{
			session_log("[Session]no available resource for new session");
			break;
		}
		
		sret = initialize_session(session, 
							  sessionname, 
							  SESSION_NAME_MAX_LENGTH,
							  id,
							  subid,
							  fds[index]);
		if (SR_NOERROR != sret)
		{
			session_log("[Session]fail to init session context");
			break;
		}
		session_mutex_lock(group->mutex);
		sret = insert_session(group, session);
		session_mutex_unlock(group->mutex);
		if (SR_NOERROR != sret)
		{
			recycle_session(session);
			session_log("[Session]fail to insert session context %d", sret);
			break;
		}

		ids[index] = id;
		subsessionids[index] = subid;
	}	
	
	sret = start_monitoring_thread(group);
	if (SR_NOERROR != sret && SR_E_INUSE != sret)
	{
		session_log("[Session]start_monitoring_thread failed %d", sret);
		session_mutex_lock(group->mutex);
		remove_session(group, session);
		session_mutex_unlock(group->mutex);
		goto exit;
	}
	else
	{
		sret = SR_NOERROR;
	}
	
exit:
	session_mutex_unlock(session_manager.mutex);
	
	if (SR_NOERROR != sret &&
		NULL != group)
	{
		remove_group(group);
	}
	return sret;


}


/**for client side*/
SESSION_RESULT session_connect(char *name, int namesize, U16 id, U16 *subsessionid)
{
	Session_Context 		*session = NULL;
	Session_Group 			*group = NULL;
	char 					sessionname[SESSION_NAME_MAX_LENGTH];
	char 					peername[SESSION_NAME_MAX_LENGTH];
	int 					fd;
	SESSION_RESULT 			sret = SR_NOERROR;

	session_log("[SESSION]session_connect, group name[ %s],id[%d]", name, id);
	session_manager_init();
	
	session_mutex_lock(session_manager.mutex);

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		sret = add_group(name, namesize, &group);
		if (sret != SR_NOERROR)
		{
			goto exit;
		}
	}
	
	memset(sessionname, 0x0, SESSION_NAME_MAX_LENGTH);
	memset(peername, 0x0, SESSION_NAME_MAX_LENGTH);

	sret = get_default_subsession_id(group, subsessionid);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]fail to get default subsession id[%d]", sret);
		goto exit;
	}
	
	sret = check_session_id_conflict(group, id, *subsessionid);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session] session id[%4x] conflict", id);
		goto exit;
	}

	sret = allocate_session(&session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log("[Session]no available resource for new session");
		goto exit;
	}

	sret = compose_client_session_name(name, 
											namesize,
											id, 
											sessionname, 
											SESSION_NAME_MAX_LENGTH,
											*subsessionid);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]compose client session name failed %d", sret);		
		goto exit;
	}
	sret = compose_server_session_name(name, 
											namesize,
											peername, 
											SESSION_NAME_MAX_LENGTH);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]compose server session name failed %d", sret);
		goto exit;
	}
	
	fd = group->stream->create_stream(sessionname, SESSION_NAME_MAX_LENGTH);
	if (fd < 0)
	{
		session_log("[Session]create server session name failed %d", sret);
		goto exit;
	}
	
    if (group->stream->connect_stream(fd, peername, SESSION_NAME_MAX_LENGTH) < 0)
    {
		group->stream->close_stream(fd);
        goto exit;
    }
	sret = initialize_session(session, 
							  sessionname, 
							  SESSION_NAME_MAX_LENGTH,
							  id,
							  *subsessionid,
							  fd);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]initialize server session name failed %d", sret);
		goto exit;
	}
	session_mutex_lock(group->mutex);
	sret = insert_session(group, session);
	session_mutex_unlock(group->mutex);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]insert server session name failed %d", sret);
		goto exit;
	}

	sret = start_monitoring_thread(group);
	if (SR_NOERROR != sret && SR_E_INUSE != sret)
	{
		session_log("[Session]start_monitoring_thread failed %d", sret);
		session_mutex_lock(group->mutex);
		remove_session(group, session);
		session_mutex_unlock(group->mutex);
		goto exit;
	}
	else
	{
		sret = SR_NOERROR;
	}
	
exit:
	session_mutex_unlock(session_manager.mutex);
	if (SR_NOERROR != sret &&
		SR_PENDING != sret &&
		NULL != session)
	{
		recycle_session(session);
	}

	if (SR_NOERROR != sret &&
		NULL != group)
	{
		remove_group(group);
	}
	return sret;
}

/**for server side*/
/*TODO: add protection for this session, that is, forbid to read or write on this session*/
SESSION_RESULT session_setup(char *name, int namesize)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	char 				localname[SESSION_NAME_MAX_LENGTH];
	int 				fd;
	int					thread_id = -1;
	SESSION_RESULT 		sret = SR_NOERROR;

	session_log("[Session]session_setup");
	
	memset(localname, 0x0, sizeof(localname));

	session_manager_init();
	
	session_mutex_lock(session_manager.mutex);

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		sret = add_group(name, namesize, &group);
		if (sret != SR_NOERROR)
		{
			session_mutex_unlock(session_manager.mutex);
			return sret;
		}
	}

	session_mutex_lock(group->mutex);

	if (SESSION_THREAD_STATE_ACTIVE == group->waiting_thread_state )
	{
		sret = SR_E_INUSE;
		goto exit;
	}
	
	sret = allocate_session(&session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log("[Session]no available resource for new session");
		goto exit;
	}
	
	
	sret = compose_server_session_name(name, 
										namesize,
										localname, 
										SESSION_NAME_MAX_LENGTH);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]create server name failed with %d", sret);
		goto exit;
	}
	
	fd = group->stream->create_stream(localname, SESSION_NAME_MAX_LENGTH);
	if (fd < 0)
	{
		session_log("[Session]fail to create stream %d", fd);
		goto exit;
	}

	if (group->stream->listen_stream(fd, 5) < 0)
	{
		session_log("[Session]fail to listen stream %d", fd);
		group->stream->close_stream(fd);
		goto exit;
	}

	group->waiting_fd = fd;	
		
	sret = start_waiting_thread(group);
	if (SR_NOERROR != sret && SR_E_INUSE != sret)
	{
		session_log("[Session]start_waiting_thread failed with %d", sret);
		group->stream->close_stream(fd);
		goto exit;
	}
	else
	{
		sret = SR_NOERROR;
	}

exit:
	session_mutex_unlock(group->mutex);
	
	session_mutex_unlock(session_manager.mutex);

	if (SR_NOERROR != sret &&
		SR_PENDING != sret &&
		NULL != session)
	{
		recycle_session(session);
	}
	if (SR_NOERROR != sret &&
		NULL != group)
	{
		remove_group(group);
	}
	return sret;
	
}

void session_destroy(char *name, int namesize, U16 sessionid, U16 subsessionId)
{
	Session_Context *session = NULL;
	Session_Group	*group = NULL;
	SESSION_RESULT 	sret = SR_NOERROR;
	
	session_log("[Session]session_destroy: name [%s], id[%d], subid[%d]",name, sessionid, subsessionId);

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]no group to destroy");
		return sret;
	}
	
	session_mutex_lock(group->mutex);
	sret = find_session_by_id(group, sessionid, subsessionId, &session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log_err("[Session]find no session to destroy");
		goto exit;
	}
	sret = remove_session(group, session);
	if (SR_PENDING != sret)
	{
		if (FALSE == signal_is_locked(session))
		{
			session_log_err("[Session] fd[%d] to be destroyed", session->fd);
			group->stream->close_stream(session->fd);
		}
		recycle_session(session);
	}
	else
	{
		//TODO: should we triger master to recycle this session?
		master_triger_event(group);
	}
exit:
	session_mutex_unlock(group->mutex);
	remove_group(group);
	return;
}

//TODO:
/*handler id(hId) is used to unregister handler*/
SESSION_RESULT session_register_handler(char *name, 
											  int namesize, 
											  int start_id, 
											  int end_id, 
											  EventCallback 	callback,
											  U16 *hId)
{
	Session_Group	*group = NULL;
	Session_Handler *handler = NULL;
	SESSION_RESULT	sret = SR_NOERROR;
	
	session_log("[Session]session_register_handler: name [%s], startid[%d], endid[%d]",
					name, start_id, end_id);

	//all public API should call session manager init before further operation
	session_manager_init();
	session_mutex_lock(session_manager.mutex);
	
	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		sret = add_group(name, namesize, &group);
		if (sret != SR_NOERROR)
		{
			session_log_err("[Session][E]fail to add group:%d", sret);
			goto exit;
		}
	}

	sret = allocate_handler(start_id, end_id, callback, &handler);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session][E]fail to allocate handler context:%d", sret);
		goto exit;
	}

	sret = insert_handler(group, handler);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session][E]fail to insert handler:%d", sret);
		recycle_handler(handler);
		goto exit;
	}
	*hId = handler->identify;
exit:
	session_mutex_unlock(session_manager.mutex);
	return sret;
}

//TODO:
/*following cases have to be considered:						*/
/*1, id range:												*/
/*    id range maybe lager/smaller/overlapped with existed range	*/
/*2. callback is neccassery to specify ????						*/
/*So simpilicy to process, use handler id to unregister			*/
SESSION_RESULT session_unregister_handler(char *name, 
											  int namesize,
											  U16 hId)
{
	Session_Group	*group = NULL;
	Session_Handler *handler = NULL;
	SESSION_RESULT	sret = SR_NOERROR;

	session_log("[Session]session_unregister_handler: name [%s], id[%d]", name, hId),

	//all public API should call session manager init before further operation
	session_manager_init();
	
	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session][E]find no group to unregister");
		goto exit;
	}

	sret = find_handler_by_identify(group, hId, &handler);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session][E]find no handler to unregister");
		goto exit;
	}
	else
	{
		remove_handler(group, handler);
		recycle_handler(handler);
	}
exit:
	return sret;
}

int session_read(char *name, int namesize, U16 sessionid, U16 subsessionid, void *buffer, int size)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	int 				length = 0;
	SESSION_RESULT 		sret = SR_NOERROR;
	
	session_log("[Session]session_read: name [%s], id[%d], subid[%d], size[%d]",
				name, sessionid, subsessionid, size);

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]find no group");
		return sret;
	}

	session_mutex_lock(group->mutex);

	sret = find_session_by_id(group, sessionid, subsessionid, &session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log_err("[Session]find no session to read");
		goto exit;
	}	
	length = group->stream->read(session->fd, buffer, size);

	if (length == 0 &&
		FALSE == is_garbage_session(session))
	{
		start_monitor_readable(session);
		master_triger_event(group);
	}
exit:	
	session_mutex_unlock(group->mutex);
	return length;
}

int session_write(char *name, int namesize, U16 sessionid, U16 subsessionid, void *buffer, int size)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	int 				length = 0;
	SESSION_RESULT 		sret = SR_NOERROR;

	session_log("[Session]session_write: id[%x], subid[%x], size[%d]", sessionid, subsessionid,size);

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]find no group to write");
		return sret;
	}
	
	session_mutex_lock(group->mutex);
	sret = find_session_by_id(group, sessionid, subsessionid, &session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log_err("[Session]find no session to write");
		goto exit;
	}	

	length = group->stream->write(session->fd, buffer, size);
	if (length > 0 && 
		length < size &&
		FALSE == is_garbage_session(session))
	{
		start_monitor_writable(session);
		master_triger_event(group);
	}
exit:	
	session_mutex_unlock(group->mutex);
	return length;
}

int session_write_control(char *name, int namesize, U16 sessionid, U16 subsessionid, void *ctl, int ctllen, void *data, int datalen)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	int 				length = 0;
	SESSION_RESULT 		sret = SR_NOERROR;

	session_log("[Session]session_write_control");

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]find no group to write");
		return sret;
	}

	session_mutex_lock(group->mutex);

	sret = find_session_by_id(group, sessionid, subsessionid, &session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log_err("[Session]find no session to write");
		goto exit;
	}	

	length = group->stream->write_control(session->fd, ctl, ctllen, data, datalen);
	if (length > 0 && length < datalen)
	{
		start_monitor_writable(session);
		master_triger_event(group);
	}
exit:	
	session_mutex_unlock(group->mutex);
	return length;
}



/**If both of directiuon are set, maxinum size among input and output will be returned*/
SESSION_RESULT session_avaliable(char *name, int namesize, U16 id, U16 subsessionid, SESSION_DIRECTION direction, int *size)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	SESSION_RESULT 		sret = SR_NOERROR;
	int insize, outsize = 0;

	session_log("[Session]session_avaliable: name[%s], id[%d],subis[%d]", name, id, subsessionid);

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		return sret;
	}

	session_mutex_lock(group->mutex);
	
	sret = find_session_by_id(group, id, subsessionid, &session);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log("[Session]find no session for this operation");
		goto exit;
	}	
	
	if (direction & DIRECTION_INPUT) 
	{
		group->stream->get_stream_length(session->fd, DIRECTION_INPUT, &insize);
	}
	if (direction & DIRECTION_OUTPUT) 
	{
		group->stream->get_stream_length(session->fd, DIRECTION_OUTPUT, &outsize);
	}
	*size = max(insize, outsize);
exit:
	session_mutex_unlock(group->mutex);
	return sret;
}

/*direction can be both of input and output*/
SESSION_RESULT set_session_buffer_size(char *name, int namesize, U16 sessionid, U16 subsessionid, SESSION_DIRECTION direction, I32 size)
{
	return SR_E_NOT_SUPPORT;
}

SESSION_RESULT session_get_fd(char *name, int namesize, U16 sessionid, U16 subsessionid, int* fd)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	SESSION_RESULT 		sret = SR_NOERROR;
	int insize, outsize = 0;

	session_log("[Session]session_get_fd: name[%s], id[%d],subis[%d]", name, sessionid, subsessionid);

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]find no group to get fd");
		return sret;
	}
	
	session_mutex_lock(group->mutex);
	
	sret = find_session_by_id(group, sessionid, subsessionid, &session);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]find no session to get fd");
		goto exit;
	}	

	*fd = session->fd;

exit:
	session_mutex_unlock(group->mutex);
	return sret;
}
SESSION_RESULT session_lock(char *name, int namesize, U16 sessionid, U16 subsessionid, BT_BOOL lock)
{
	Session_Context 	*session = NULL;
	Session_Group 		*group = NULL;
	SESSION_RESULT 		sret = SR_NOERROR;
	int insize, outsize = 0;

	session_log("[Session]session_lock: name[%s], id[%d],subis[%d], lock[%d]", name, sessionid, subsessionid, lock);

	//all public API should call session manager init before further operation
	session_manager_init();

	sret = find_group(name, namesize, &group);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]find no group to get fd");
		return sret;
	}
	
	session_mutex_lock(group->mutex);
	
	sret = find_session_by_id(group, sessionid, subsessionid, &session);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]find no session to get fd");
		goto exit;
	}
	
	sret = lock_session_signal(session, lock);
	if (sret != SR_NOERROR)
	{
		session_log_err("[Session]find no group to get fd");
		goto exit;
	}
	
	master_triger_event(group);
exit:
	session_mutex_unlock(group->mutex);
	return sret;
}




static SESSION_RESULT find_group(char* name, int	namesize, Session_Group **group)
{
	int index = 0;
	Session_Group *siterator = NULL;
	
	for(index = 0; index < MAX_GROUP_COUNT;index ++)
	{	
		siterator = &session_manager.group[index];
		if((GROUP_STATE_ACTIVE == siterator->state) &&
			(strlen(name) == strlen((char *)siterator->group_name)) &&
			(strncmp((char *)siterator->group_name, name, min(namesize, SESSION_NAME_MAX_LENGTH)) == 0))
		{
			*group = siterator;
			break;
		}
	}
	if (index >= MAX_GROUP_COUNT)
	{
		return SR_E_INVALID_PARAM;
	}
	else
	{
		return SR_NOERROR;
	}
}

static SESSION_RESULT add_group(char* name, int namesize, Session_Group **group)
{
	int index = 0;
	Session_Group *siterator = NULL;
	SESSION_RESULT sret = SR_NOERROR;
	
	for(index = 0; index < MAX_GROUP_COUNT;index ++)
	{	
		siterator = &session_manager.group[index];
		if(GROUP_STATE_IDLE == siterator->state)
		{
			break;
		}
	}
	if (index >= MAX_GROUP_COUNT)
	{
		return SR_E_EXCEED_LIMITATION;
	}
	
	sret = group_init(siterator, name, namesize);
	if (SR_NOERROR == sret)
	{
		*group = siterator;
	}
	return sret;
}

SESSION_RESULT remove_group(Session_Group *group)
{
	int index = 0;
	Session_Group *siterator = NULL;
	
	for(index = 0; index < MAX_GROUP_COUNT;index ++)
	{	
		if(group == siterator)
		{
			break;
		}
	}
	if (index >= MAX_GROUP_COUNT)
	{
		return SR_E_INVALID_PARAM;
	}
	
	if(IsListEmpty(&group->session_list) &&
		IsListEmpty(&group->handler_list))
	{
		group_deinit(group);
	}
	
	return SR_NOERROR;
}




