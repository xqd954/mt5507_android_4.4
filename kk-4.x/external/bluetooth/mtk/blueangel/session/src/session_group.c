#include "session.h" 
#include "session_master.h"

extern SESSION_RESULT remove_group(Session_Group *group);

SESSION_RESULT group_init(Session_Group *group, char*	name, int namesize)
{
	SESSION_RESULT sret;
	if (NULL == group)
	{
		return SR_E_INVALID_PARAM;
	}
	memset(group, 0x0, sizeof(Session_Group));
	group->state = GROUP_STATE_ACTIVE;
	strncpy((char *)group->group_name, name, min(namesize, SESSION_NAME_MAX_LENGTH));

	sret = session_mutex_create(&group->mutex);
	if (SR_NOERROR != sret)
	{
		return sret;
	}

	InitializeListHead(&group->session_list);
	InitializeListHead(&group->handler_list);
	
	group->stream = get_stream(STREAM_TYPE_SOCKET);
	if (group->stream == NULL)
	{
		return SR_E_INVALID_PARAM;
	}
	return SR_NOERROR;
}

void group_deinit(Session_Group *group)
{
	Session_Context *siterator = NULL;

	session_log("group_deinit");

	while(!IsListEmpty(&group->session_list))
	{
		siterator = (Session_Context *)RemoveHeadList(&group->session_list);
		recycle_session(siterator);
	}

	//TODO: handler list

	stop_monitoring_thread(group);
	
	stop_waiting_thread(group);
	
	session_mutex_destroy(group->mutex);

	group->mutex = NULL;

	group->state = GROUP_STATE_IDLE;
}



//TODO: risk to compare string using strncmp
SESSION_RESULT match_group(Session_Group *group, char* name, int namesize)
{
	int ret;
	if (strlen(name) > SESSION_NAME_MAX_LENGTH)
	{
		return SR_E_GENERAL;
	}
	ret = strncmp ((char *)group->group_name, name, namesize);
	return (ret == 0) ? SR_NOERROR : SR_E_GENERAL;
}


static SESSION_RESULT on_event_happend(Session_Handler *handler, SESSION_EVENT event, U16 sessionid, U16 subsessionid)
{	
	if (NULL == handler ||
		NULL == handler->callback)
	{
		return SR_E_INVALID_PARAM;
	}
	Session_Event_PARM 	parm;
	parm.event = event;
	parm.sessionid = sessionid;
	parm.subsessionid = subsessionid;	
	parm.index = (sessionid << 16 | subsessionid);
	handler->callback(&parm);
	return SR_NOERROR;
}



//TODO: 
//2. add mutex protection 
SESSION_RESULT on_session_connected(Session_Group *group, char *name, int size, int fd)
{
	Session_Context 		*session = NULL;
	Session_Handler			*handler = NULL;
	char 					groupname[SESSION_NAME_MAX_LENGTH];
	U16 					sessionid = 0;
	U16 					subsessionid = 0;
	SESSION_RESULT 			sret = SR_NOERROR;

	session_log("[session]on_session_connected: group [%s], fd[%d]", name, fd);

	if (NULL == name || 0 >= size || fd < 0)
	{
		sret = SR_E_INVALID_PARAM;
		goto exit;
	}
	
	memset(groupname, 0x0, SESSION_NAME_MAX_LENGTH);
	
	sret = parse_session_name(name,
								size,
								groupname,
								SESSION_NAME_MAX_LENGTH,
								&sessionid,
								&subsessionid);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]fail to parse remote connection name");
		goto exit;
	}	

	sret = match_group(group, groupname, SESSION_NAME_MAX_LENGTH);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session][%s]does not match group name[%s]", groupname, group->group_name);
		goto exit;
	}	

	sret = find_handler(group, sessionid, &handler);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]no handler is found for session[%d]", sessionid);
		goto exit;
	}

	sret = allocate_session(&session);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]no available resource for new session");
		goto exit;
	}
	
//	strncpy((char *)session->session_name, name, min(SESSION_NAME_MAX_LENGTH, size));

	sret = check_session_id_conflict(group, sessionid, subsessionid);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session] session id[%d] conflict", sessionid);
		goto exit;
	}
	
	
	sret = initialize_session(session, 
							  name,
							  SESSION_NAME_MAX_LENGTH, 
							  sessionid,
							  subsessionid,
							  fd);
	if (SR_NOERROR != sret || NULL == session)
	{
		session_log_err("[Session]initialize session failed");
		goto exit;
	}
	session_mutex_lock(group->mutex);
	sret = insert_session(group, session);
	session_mutex_unlock(group->mutex);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]insert session failed %d", sret);
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

	sret = on_event_happend(handler, SESSION_EVENT_CREATED, sessionid, subsessionid);
	if ((SR_NOERROR != sret) )
	{
		session_log_err("[Session]fail to notify incoming connection %d", sret);
		session_mutex_lock(group->mutex);
		sret = remove_session(group, session);
		session_mutex_unlock(group->mutex);
		goto exit;
	}

	
	
exit:
	if (SR_NOERROR != sret &&
		SR_PENDING != sret &&
		NULL != session)
	{
		recycle_session(session);
	}
	return sret;
}


static void *waiting_session_routine(void *ptr)
{
	Session_Group *group = (Session_Group *)ptr;
	int fd = -1;
	char remote_addr[SESSION_NAME_MAX_LENGTH];
	SESSION_RESULT sret = SR_NOERROR;

	session_log("[Session]waiting_session_routine start");

	while (TRUE)
	{
		memset(remote_addr, 0x0, SESSION_NAME_MAX_LENGTH);
		fd = group->stream->accept_stream(group->waiting_fd, remote_addr, SESSION_NAME_MAX_LENGTH);
		if (fd < 0)
		{
			session_log("[Session]error happenes when accept stream: %d", group->waiting_fd);
			break;
		}
		if (GROUP_STATE_IDLE == group->state)
		{
			session_log("[Session]the group has been disabled");
			break;
		}
		sret = on_session_connected(group, remote_addr, SESSION_NAME_MAX_LENGTH, fd);
		if (SR_NOERROR != sret)
		{
			session_log_err("[session]fail to process the remote session connection");
			group->stream->close_stream(fd);
		}
	}
	
	session_mutex_lock(group->mutex);
	group->waiting_thread_state = SESSION_THREAD_STATE_IDLE; 
	session_mutex_unlock(group->mutex);
	
	session_log("[Session]waiting_session_routine end");
	return NULL;
}

static void reorganize_sessions(Session_Group *group)
{
	Session_Context *cursor = NULL;
	Session_Context *session = NULL;
	SESSION_RESULT sret = SR_NOERROR;

	session_log("[session]reorganize_sessions");
	
	session_mutex_lock(group->mutex);
	
	sret = find_first_session(group, &cursor);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]no first session in list");
		goto exit;
	}	
	
	do
	{
		session = cursor;
		sret = find_next_session(group, &cursor);
		
		if (is_garbage_session(session))
		{
			session_log("[session]garbage session[%d][%d]", session->sessionId, session->subsessionId);
			if (FALSE == signal_is_locked(session))
			{
				group->stream->close_stream(session->fd);
				session_log("[session]close fd[%d]", session->fd);
			}
			RemoveEntryList((ListEntry *)session);
			InitializeListEntry((ListEntry *)session);
			group->session_count --;
			recycle_session(session);
		}		
	} while (SR_NOERROR == sret);
exit:	
	session_mutex_unlock(group->mutex);	
}


//TODO: 
//1. when retriving fds, the new session is added, the new fd can not be monitored until next time
//	if the timeout to poll is set to ever, the new fd maybe has no chance to be monitored
//2. improve effectivity to retrieve fd list
static int retrieve_fds_monitored(Session_Group *group, Session_Poll_Fd* pfds, int size)
{
    int index = 0;
	int count = 0;
	Session_Context *cursor = NULL;
	int max_count = 0;
	Session_Context *siterator = NULL;
	SESSION_RESULT sret = SR_NOERROR;
	
	session_log("[Session]retrieve_fds_monitored %d", size);

	if (NULL == group ||
		NULL == pfds ||
		size <= 0)
	{
		sret = SR_E_INVALID_PARAM;
		return sret;
	}
	
    memset(pfds, 0, sizeof(pfds[0])*size);

	//mutex is neccessary for find_first_session and find_next_session	
	session_mutex_lock(group->mutex);

	//add master first
	sret = master_monitor_event(group, pfds);
	if(SR_NOERROR == sret)
	{
		count ++;
	}
	
 	max_count = min(size, (group->session_count + 1));
	if (max_count <= 0)
	{
		session_log_err("[Session]no any session in list, count[%d]", group->session_count);		
	}
	
	sret = find_first_session(group, &cursor);
	if (SR_NOERROR != sret)
	{
		session_log_err("[Session]no first session in list");
		goto exit;
	}	
	while(count < max_count)
	{
		sret = prepare_monitor_session(cursor);
		if(SR_NOERROR == sret)
		{
			pfds[count].fd = cursor->fd;
			pfds[count].interested_evts = cursor->event;
			session_log("[session]retrieve_fds_monitored: session event[%x], fd[%d]", cursor->event, cursor->fd);
            count++;
		}	
		else
		{
			session_log_err("[Session]the session[%d][%d] is not ready to monitor",
							cursor->sessionId, cursor->subsessionId);
		}
		sret = find_next_session(group, &cursor);
		if (SR_NOERROR != sret)
		{
			session_log_err("[Session]no any more session is in list");
			break;
		}
	}
	
exit:
	session_mutex_unlock(group->mutex);
	return count;
}

static void process_session_event(Session_Group *group, Session_Poll_Fd* pfds, int size)
{
	Session_Context *session = NULL;
	Session_Handler *handler = NULL;
	int eventcount = 0;
	int count = 0;
	int index = 0;
	SESSION_EVENT event = 0;
	SESSION_RESULT sret = SR_NOERROR;
	
	session_log("[Session]process_session_event %d", size);
	
//	session_mutex_lock(group->mutex);		
	while(count < size)
	{
		//
		//session_log("[Session]current count %d", count);
		
		index = count;
		count ++;
		if (pfds[index].actual_evts)
		{
			if (SESSION_EVENT_INVALID == pfds[index].actual_evts)
			{
				session_log_err("[Session]invalid events");
				continue;
			}

			sret = master_handle_event(group, &pfds[index]);
			if (SR_NOERROR == sret)
			{
				//the event has been processed by master
				continue;
			}
			
			handler = NULL;
			session = NULL;
			
			session_mutex_lock(group->mutex);
			sret = find_session_by_fd(group, pfds[index].fd, &session);
			session_mutex_unlock(group->mutex);
			if (SR_NOERROR != sret || session == NULL)
			{
				session_log_err("[Session]no session matching with the fd[%d]", pfds[index].fd);
				continue;
			}	

			sret = find_handler(group, session->sessionId, &handler);
			if (SR_NOERROR != sret)
			{
				session_log_err("[Session]no handler for this sessionid[%x]", session->sessionId);
				continue;
			}
			
			event = update_event(session, pfds[index].actual_evts);
			if (event)
			{
				on_event_happend(handler, pfds[index].actual_evts, session->sessionId, session->subsessionId);
			}			
		}
		else
		{
		//	session_log_err("[Session]error:the polled event is %d", pfds[index].actual_evts);				
		}
	}
	session_log("[Session]process_session_event end");
//	session_mutex_unlock(group->mutex);	
}


static void *monitor_session_routine(void *ptr)
{
	Session_Group *group = (Session_Group *)ptr;
	Session_Poll_Fd pfds[MAX_SESSION_COUNT];
	int count = 0;
	int ret = 0;
	
	session_log("[Session]monitor_session_routine start");
	
    memset(pfds, 0, sizeof(pfds[0])*MAX_SESSION_COUNT);

	if (NULL == group ||
		GROUP_STATE_IDLE == group)
	{
		return NULL;
	}
	
    while(TRUE)
    {
        count = retrieve_fds_monitored(group, pfds, MAX_SESSION_COUNT);
		if (count >= 0)
		{
			ret = group->stream->poll(pfds, count, -1);
			if (ret < 0 || group->state != GROUP_STATE_ACTIVE)
			{
				break;
			}
			process_session_event(group, pfds, count);
		}
		else
		{
			//no session in monitoring
			session_log("[Session]no session is in monitoring state");
			break;
		}		

		reorganize_sessions(group);
    }
	
	reorganize_sessions(group);
	
	session_mutex_lock(group->mutex);
	group->monitor_thread_state = SESSION_THREAD_STATE_IDLE;
	session_mutex_unlock(group->mutex);
	if (group->state != GROUP_STATE_ACTIVE)
	{
		remove_group(group);
	}
	session_log("[Session]monitor_session_routine end");
	return NULL;
}



SESSION_RESULT start_waiting_thread(Session_Group *group)
{   
	SESSION_RESULT sret = SR_NOERROR;
	
	if ((NULL == group) ||
		(GROUP_STATE_IDLE == group->state) ||
		(group->waiting_fd < 0) ||
		(NULL == group->stream))
	{
		return SR_E_INVALID_PARAM;
	}

//	session_mutex_lock(group->mutex);
	if (SESSION_THREAD_STATE_ACTIVE == group->waiting_thread_state)
	{
		sret = SR_E_INUSE;
	}
	else
	{
    	if(-1 != session_thread_create(waiting_session_routine, group))
    	{
			group->waiting_thread_state = SESSION_THREAD_STATE_ACTIVE;
		}
	}
//	session_mutex_unlock(group->mutex);
	return sret;
}
//TODO:
static SESSION_RESULT stop_waiting_thread(Session_Group *group)
{
	return SR_NOERROR;;
}





SESSION_RESULT start_monitoring_thread(Session_Group *group)
{   
	SESSION_RESULT sret = SR_NOERROR;
	
	if ((NULL == group) ||
		(GROUP_STATE_IDLE == group->state) ||
		(NULL == group->stream))
	{
		return SR_E_INVALID_PARAM;
	}	
	session_log("start_monitoring_thread: state %d", group->monitor_thread_state);
	
	session_mutex_lock(group->mutex);
	if (SESSION_THREAD_STATE_ACTIVE == group->monitor_thread_state)
	{
		sret = SR_E_INUSE;
		
		//the monitoring thread has come up, just notify to monitor new-added sesssion
		master_triger_event(group);
		goto exit;
	}

	//create master firstly 
	sret = master_start_work(group);
	if (SR_NOERROR != sret && SR_E_ALREADY_COMPLETE != sret)
	{
		goto exit;
	}
	if (-1 != session_thread_create(monitor_session_routine, group))
	{
		session_log("[session]create monitoring thread in group 0x[%x]", group);
		group->monitor_thread_state = SESSION_THREAD_STATE_ACTIVE;
		sret = SR_NOERROR;
	}
	else
	{
		sret = SR_E_RESOURCE;
	}
	
exit:	
	session_mutex_unlock(group->mutex);
	return sret;
}
//TODO:
SESSION_RESULT stop_monitoring_thread(Session_Group *group)
{
	master_stop_work(group);
	return SR_NOERROR;
}



//TODO: add check whether the range of current handler conflict with existing handler 
SESSION_RESULT insert_handler(Session_Group *group, Session_Handler *handler)
{
	int index = 0;
	int reverseindex = 0;
	Session_Context *siterator= NULL;
	Session_Context *snext= NULL;
	SESSION_RESULT sret = SR_NOERROR;
	
	if (NULL == handler ||
		NULL == group)
	{
		return SR_E_INVALID_PARAM;
	}
	session_mutex_lock(group->mutex);	
	InsertTailList(&group->handler_list,(ListEntry *)handler);	
	session_mutex_unlock(group->mutex);

	return sret;
}




//when monitoring state ends, it will be processed then
SESSION_RESULT remove_handler(Session_Group *group, Session_Handler *hanlder)
{
	int index = 0;
	int reverseindex = 0;
	Session_Context *siterator= NULL;
	Session_Context *snext= NULL;
	SESSION_RESULT sret = SR_NOERROR;
	
	if (NULL == group ||
		NULL == hanlder)
	{
		return SR_E_INVALID_PARAM;
	}
	session_mutex_lock(group->mutex);

	RemoveEntryList((ListEntry *)hanlder);
	InitializeListEntry((ListEntry *)hanlder);
	session_mutex_unlock(group->mutex);
	return sret;
}

SESSION_RESULT find_handler(Session_Group *group, U16 sessionid, Session_Handler **handler)
{
	ListEntry 			*head = NULL;
	ListEntry 			*next = NULL;
	Session_Handler 	*hiterator = NULL;
	SESSION_RESULT 		sret = SR_E_NO_ENTRY;

	if(NULL == group ||
		NULL == handler)
	{
		*handler = NULL;
		return SR_E_INVALID_PARAM;
	}
	
	session_mutex_lock(group->mutex);
	
	*handler = NULL;

	if(IsListEmpty(&group->handler_list))
	{
		sret = SR_E_NO_ENTRY;
		goto exit;
	}
	head = &group->handler_list;
	next = GetNextNode(&group->handler_list);

	while(head != next)
	{
		hiterator = (Session_Handler *)next;
		if(sessionid >= hiterator->start_session_Id &&
			sessionid <= hiterator->end_session_Id)
		{
			*handler = hiterator;
			sret = SR_NOERROR;
			break;
		}
		next = GetNextNode(next);
	}
	
exit:		
	session_mutex_unlock(group->mutex);
	return sret;	
}

SESSION_RESULT find_handler_by_identify(Session_Group *group, U16 identify, Session_Handler **handler)
{
	ListEntry 			*head = NULL;
	ListEntry 			*next = NULL;
	Session_Handler 	*hiterator = NULL;
	SESSION_RESULT 		sret = SR_E_NO_ENTRY;

	if(NULL == group ||
		NULL == handler)
	{
		*handler = NULL;
		return SR_E_INVALID_PARAM;
	}
	
	session_mutex_lock(group->mutex);
	
	*handler = NULL;

	if(IsListEmpty(&group->handler_list))
	{
		sret = SR_E_NO_ENTRY;
		goto exit;
	}
	head = &group->handler_list;
	next = GetNextNode(&group->handler_list);

	while(head != next)
	{
		hiterator = (Session_Handler *)next;
		if(identify == hiterator->identify)
		{
			*handler = hiterator;
			sret = SR_NOERROR;
			break;
		}
		next = GetNextNode(next);
	}
	
exit:		
	session_mutex_unlock(group->mutex);
	return sret;	
}


SESSION_RESULT insert_session(Session_Group *group, Session_Context *session)
{
	int index = 0;
	int reverseindex = 0;
	Session_Context *siterator= NULL;
	Session_Context *snext= NULL;
	SESSION_RESULT sret = SR_NOERROR;
	
	if (NULL == session ||
		NULL == group)
	{
		return SR_E_INVALID_PARAM;
	}
//	session_mutex_lock(group->mutex);
	
	InsertTailList(&group->session_list,(ListEntry *) session);
	group->session_count ++;
	
//	session_mutex_unlock(group->mutex);

	return sret;
}

//when monitoring state ends, it will be processed then
SESSION_RESULT remove_session(Session_Group *group, Session_Context *session)
{
	int index = 0;
	int reverseindex = 0;
	Session_Context *siterator= NULL;
	Session_Context *snext= NULL;
	SESSION_RESULT sret = SR_NOERROR;
	
	if (NULL == group ||
		NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}
//	session_mutex_lock(group->mutex);

	sret = prepare_recycle_session(session);
	if (SR_PENDING != sret)
	{
		RemoveEntryList((ListEntry *)session);
		InitializeListEntry((ListEntry *)session);
		group->session_count --;
//		recycle_session(session);
	}
//	session_mutex_unlock(group->mutex);
	return sret;
}



SESSION_RESULT find_first_session(Session_Group *group, Session_Context **cursor)
{
	SESSION_RESULT sret = SR_NOERROR;	

	if(NULL == group)
	{
		*cursor = NULL;
		return SR_E_INVALID_PARAM;
	}
	
//	session_mutex_lock(group->mutex);

	if (!IsListEmpty(&group->session_list))
	{
		*cursor = (Session_Context *)GetHeadList((ListEntry *)&group->session_list);		
	}
	else
	{
		*cursor = NULL;
		sret = SR_E_NO_ENTRY;
	}
	
//	session_mutex_unlock(group->mutex);
	return sret;	
}

/*input parameter:									*/
/*	group: in											*/
/*	cursor: in-out										*/
/*return value:										*/
/*	SR_NOERROR: 									*/
/*	any error:    arrive end or invalid param				*/
/*Notes:												*/
/*	1. advise to lock this group when using this API			*/
/*	2. when the session is zombie, remove it and find furhter	*/
SESSION_RESULT find_next_session(Session_Group *group, Session_Context **cursor)
{
	SESSION_RESULT sret = SR_NOERROR;
	ListEntry * curr = (ListEntry *)(*cursor);
	ListEntry * next = NULL;

	if(NULL == group ||
		NULL == cursor)
	{
		*cursor = NULL;
		return SR_E_INVALID_PARAM;
	}
	
	//	session_mutex_lock(group->mutex);
	
	if (IsNodeOnList(&group->session_list, curr))
	{
		next = GetNextNode(curr);	
		if (next == &group->session_list)
		{
			sret = SR_E_NO_ENTRY;
			*cursor = NULL;
		}
		else
		{
			*cursor = (Session_Context *)next;
		}
	}
	else
	{	
		*cursor = NULL;
		sret = SR_E_NO_ENTRY;
	}

	//	session_mutex_unlock(group->mutex);	
	return sret;
}

/*index: session index in array*/
SESSION_RESULT find_session_by_fd(Session_Group *group, int fd, Session_Context **session)
{
	ListEntry 			*head = NULL;
	ListEntry 			*next = NULL;
	Session_Context 	*siterator = NULL;
	SESSION_RESULT 		sret = SR_E_NO_ENTRY;
	
	if(NULL == group ||
		NULL == session)
	{
		*session = NULL;
		return SR_E_INVALID_PARAM;
	}
	
//	session_mutex_lock(group->mutex);
	
	*session = NULL;

	if(IsListEmpty(&group->session_list))
	{
		sret = SR_E_NO_ENTRY;
		goto exit;
	}
	head = &group->session_list;
	next = GetHeadList(&group->session_list);

	while(head != next)
	{
		siterator = (Session_Context *)next;
		if(fd == siterator->fd && (FALSE == signal_is_locked(session)))
		{
			*session = siterator;
			sret = SR_NOERROR;
			break;
		}
		next = GetNextNode(next);
	}
	
exit:		
//	session_mutex_unlock(group->mutex);
	return sret;	

}

SESSION_RESULT find_session_by_id(Session_Group *group, U16 sessionid, U16 subsessionid, Session_Context **session)
{
	ListEntry 			*head = NULL;
	ListEntry 			*next = NULL;
	Session_Context 	*siterator = NULL;
	SESSION_RESULT 		sret = SR_E_NO_ENTRY;

	if(NULL == group ||
		NULL == session)
	{
		*session = NULL;
		return SR_E_INVALID_PARAM;
	}
	
//	session_mutex_lock(group->mutex);
	
	*session = NULL;

	if(IsListEmpty((ListEntry *)&group->session_list))
	{
		sret = SR_E_NO_ENTRY;
		goto exit;
	}
	head = &group->session_list;
	next = GetHeadList(&group->session_list);

	while(head != next)
	{
		siterator = (Session_Context *)next;
		if(sessionid == siterator->sessionId &&
			subsessionid == siterator->subsessionId)
		{
			*session = siterator;
			sret = SR_NOERROR;
			break;
		}		
		next = GetNextNode(next);
	}
	
exit:		
//	session_mutex_unlock(group->mutex);
	return sret;	
}

SESSION_RESULT check_session_id_conflict(Session_Group *group, U16 sessionid, U16 subsessionid)
{
	ListEntry 			*head = NULL;
	ListEntry 			*next = NULL;
	Session_Context 	*siterator = NULL;
	SESSION_RESULT 		sret = SR_NOERROR;

	if(NULL == group)
	{
		return SR_E_INVALID_PARAM;
	}
	
	session_mutex_lock(group->mutex);

	if(IsListEmpty((ListEntry *)&group->session_list))
	{
		sret = SR_NOERROR;
		goto exit;
	}
	head = &group->session_list;
	next = GetHeadList(&group->session_list);

	while(head != next)
	{
		siterator = (Session_Context *)next;
		if(sessionid == siterator->sessionId &&
			subsessionid == siterator->subsessionId)
		{
			sret = SR_E_CONFLICT;
			break;
		}		
		next = GetNextNode(next);
	}
	
exit:		
	session_mutex_unlock(group->mutex);
	return sret;
	
}

SESSION_RESULT get_default_subsession_id(Session_Group *group, U16 *subsessionid)
{
	*subsessionid = getpid();
	return SR_NOERROR;
}

static U16 reserved_id = 0;

SESSION_RESULT get_reserved_session_id(Session_Group *group, U16 *sessionid)
{
	U16 subsessionid = 0;
	SESSION_RESULT sret = SR_NOERROR;
	U16 index = 0;
	U16 id = 0xFF;
	int range = SESSION_ID_RANGE;


	sret = get_default_subsession_id(group, &subsessionid);
	if (SR_NOERROR != sret)
	{
		session_log("[Session]fail to get default subsession id[%d]", sret);
		return sret;
	}

	for(index = 0; index < range; index ++)
	{
		id = (reserved_id++ % range) + INTERNAL_RESERVED;
		
		sret = check_session_id_conflict(group, id, subsessionid);
		if (SR_NOERROR == sret)
		{
			break;
		}
	}

	if (index < range)
	{
		sret = SR_NOERROR;
		*sessionid = id;
	}
	else
	{
		sret = SR_E_EXCEED_LIMITATION;
	}
	return sret;	
}



