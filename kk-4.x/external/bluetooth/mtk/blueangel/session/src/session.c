#include "session.h" 
#include "assert.h"
//debug
#define ASSERT(a) assert(a)


SESSION_RESULT compose_client_session_name(char*	name, 
													 int	namesize,
													 U16 	id, 
													 char*	clientname, 
													 int 	clientnamesize,
													 U16	subsessionid)
{
	char *localbase = NULL;
	SESSION_RESULT sr_result = SR_NOERROR;
	if (clientname == NULL ||
		clientnamesize < SESSION_NAME_MAX_LENGTH )
	{
		return SR_E_INVALID_PARAM;
	}

	if (name == NULL)
	{
		localbase = SESSION_NAME_CLIENT_PREFIX;
	}
	else
	{
		localbase = name;
	}
	sprintf(clientname, SESSION_NAME_FORMAT_CLIENT, localbase, id, subsessionid, session_rand());
	return SR_NOERROR;
}

SESSION_RESULT compose_server_session_name(char*	name, 
													  int	namesize,
													 char*	servername, 
													 int 	servernamesize)
{
	char *peerbase = NULL;
	SESSION_RESULT sr_result = SR_NOERROR;

	if (servername == NULL ||
		servernamesize < SESSION_NAME_MAX_LENGTH )
	{
		return SR_E_INVALID_PARAM;
	}

	if (name == NULL)
	{
		peerbase = SESSION_NAME_SERVER_PREFIX;	
	}
	else
	{
		peerbase = name;
	}
	sprintf(servername, SESSION_NAME_FORMAT_SERVER, peerbase);
	return SR_NOERROR;
}

SESSION_RESULT parse_session_name(char *name, 
										int	namesize,
										char *servicename,
										int servicenamesize,
										U16 *sessionId, 
										U16 *subsessionid)
{
	//find available session context
//	strncpy(session->session_name, name, SESSION_NAME_MAX_LENGTH);

	if (NULL == name)
	{
		session_log("[Session]parse_session_name fails with invalid param");
		return SR_E_INVALID_PARAM;		
	}

	char *channelDup = strdup(name);
	char *p = NULL;

	//session prefix
	p = strtok(channelDup, SESSION_NAME_SEPERATOR);	
	strcpy(servicename, p);

	//session id
	p = strtok(NULL, SESSION_NAME_SEPERATOR); 
	*sessionId = atoi(p);	

	//subsession id
	p = strtok(NULL, SESSION_NAME_SEPERATOR); 
	if (p != NULL)
	{
		*subsessionid = atoi(p);
	}
	free(channelDup);
	return SR_NOERROR;
}

BOOL signal_is_locked(Session_Context *session)
{
	return session && session->locked;
}


SESSION_RESULT allocate_session(Session_Context **session)
{
	SESSION_RESULT sret = SR_NOERROR;
	*session = malloc(sizeof(Session_Context));
	if (NULL == session )
	{
		sret = SR_E_RESOURCE;
		goto exit;
	}
	memset(*session, 0x0, sizeof(Session_Context)); 
exit:
	return sret;
}


SESSION_RESULT recycle_session(Session_Context *session)
{
	if (NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}

	if (session->mutex)
		session_mutex_destroy(session->mutex);

	session->mutex = NULL;
	
	//free memory
	free(session);
	return SR_NOERROR;
}

//when sesssion is locked, no interest on any stream event: data incoming/exception
SESSION_RESULT lock_session_signal(Session_Context *session, BT_BOOL lock)
{
	if (NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}
	session_mutex_lock(session->mutex);
	session->locked = lock;

	//force to change monitor state 
//	if (SESSION_STATE_MONITOR == session->state)
//	{
//		session->state = SESSION_STATE_ACTIVE;
//	}
	session_mutex_unlock(session->mutex);
	return SR_NOERROR;
}

SESSION_RESULT initialize_session(Session_Context *session, 
									char			*name,
									int				size,
									U16				sessionid,
									U16				subsessionid,
									int 			fd)
{
	if (NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}
	strncpy((char *)session->session_name, name, min(size, SESSION_NAME_MAX_LENGTH));
	session->fd = fd;
	session->locked = FALSE;
	session->sessionId = sessionid;
	session->subsessionId = subsessionid;

	//when the session just is initialized, no need to monitor writable event
	session->event = SESSION_EVENT_DATA_INCOMING |
					/*	SESSION_EVENT_BUFFER_WRITABLE | */
						SESSION_EVENT_DESTROYED;

	session_mutex_create(&session->mutex);
	InitializeListEntry(&session->node);
	return SR_NOERROR;
}

SESSION_RESULT prepare_recycle_session(Session_Context *session)
{
	SESSION_RESULT sret = SR_NOERROR;
	if (NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}
	session_mutex_lock(session->mutex);
	if (session->state == SESSION_STATE_MONITOR)
	{
		session->state = SESSION_STATE_ZOMBIE;
	}
	if (SESSION_STATE_ZOMBIE == session->state)
	{		
		sret = SR_PENDING;
	}
	session_mutex_unlock(session->mutex);
	return sret;
}

SESSION_RESULT prepare_monitor_session(Session_Context *session)
{
	SESSION_RESULT sret = SR_NOERROR;
	session_mutex_lock(session->mutex);
	if(signal_is_locked(session) == FALSE &&
		SESSION_STATE_ZOMBIE != session->state &&
		session->event > 0)
	{
		if (SESSION_STATE_MONITOR != session->state)
		{
			session->state = SESSION_STATE_MONITOR;
		}
		sret = SR_NOERROR;
	}
	else
	{
		sret = SR_E_FORBIDEN;
	}
	session_mutex_unlock(session->mutex);
	return sret;
}

BOOL is_garbage_session(Session_Context *session)
{
	BOOL sret = FALSE;
	if (NULL == session)
	{
		return FALSE;
	}
	session_mutex_lock(session->mutex);
	if (SESSION_STATE_ZOMBIE == session->state)
	{
		sret = TRUE;
	}
	session_mutex_unlock(session->mutex);
	return sret;
}
BOOL start_monitor_event(Session_Context *session, SESSION_EVENT event)
{
	session_mutex_lock(session->mutex);
	session->event |= event;
	session->locked = FALSE;
	session_mutex_unlock(session->mutex);
	return TRUE;
}

BOOL stop_monitor_event(Session_Context *session)
{
	session_mutex_lock(session->mutex);
	session->locked = TRUE;
	session_mutex_unlock(session->mutex);
	return TRUE;
}


/*signal interface*/
//readable 
SESSION_RESULT start_monitor_readable(Session_Context *session)
{
	int size;
	SESSION_RESULT sret = SR_NOERROR;
	int length = 0;
	BOOL bRet = FALSE;
	
	session_log("[session] start_monitor_readable");
	if (NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}

	sret = start_monitor_event(session, SESSION_EVENT_DATA_INCOMING);
exit:
	return sret;
}

/*signal interface*/
//writable
SESSION_RESULT start_monitor_writable(Session_Context *session)
{
	int size;
	int length;
	SESSION_RESULT sret = SR_NOERROR;

	if (NULL == session)
	{
		return SR_E_INVALID_PARAM;
	}
	session_log("[session]start_monitor_writable [%d][%d]", session->sessionId, session->subsessionId);

	start_monitor_event(session, SESSION_EVENT_BUFFER_WRITABLE);
exit:
	return sret;
}




/*return parameter:*/
/* true: update and notify*/
/* FALSE: do nothing*/
SESSION_EVENT update_event(Session_Context *session, SESSION_EVENT event)
{	
	BOOL valid = FALSE;
	SESSION_EVENT actual_evt = 0;
	
	session_log("[session]update_event: session id[%d], event[0x%4x]", session->sessionId, event);
		
	session_mutex_lock(session->mutex);
	if (SESSION_STATE_MONITOR != session->state)
	{
		session_log_err("[Session]the session is not in monitoring state[%d]", session->state);
		goto exit;
	}
	if (signal_is_locked(session))
	{
		session_log_err("[session]the session is locked[%d][%d]", session->sessionId, session->subsessionId);
		goto exit;
	}
	
	//filter unexpected events
	actual_evt = event & session->event;

	//valid event in the range of monitoring
	if (actual_evt <= 0)
	{
		session_log_err("[session]the events are not intereted: our event[%4x], event[%4x]", session->event, event);
		goto exit;
	}

	//the happened event should not be monitored until external module set it manually
	session->event &= ~actual_evt; 
	if (event & SESSION_EVENT_DESTROYED)
	{
		session->state = SESSION_STATE_ZOMBIE;
	}
	session_log("[session]update_event: session event[%x]", session->event);
exit:
	session_mutex_unlock(session->mutex);
	return actual_evt; 
}


/*****************************************************************************
 * FUNCTION
 *  _InsertTailList
 * DESCRIPTION
 *  Insert an entry at the tail of the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void _InsertTailList(ListEntry *head, ListEntry *entry)
{
    entry->Flink = head;
    entry->Blink = head->Blink;
    head->Blink->Flink = entry;
    head->Blink = entry;
    ASSERT(IsNodeConnected(entry));

}


/*****************************************************************************
 * FUNCTION
 *  InsertTailList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void InsertTailList(ListEntry *head, ListEntry *entry)
{
    ASSERT(IsListCircular(head));
    _InsertTailList(head, entry);
    ASSERT(IsListCircular(head));
}


/*****************************************************************************
 * FUNCTION
 *  _InsertHeadList
 * DESCRIPTION
 *  Insert an entry at the head of the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void _InsertHeadList(ListEntry *head, ListEntry *entry)
{
    entry->Flink = head->Flink;
    entry->Blink = head;
    head->Flink->Blink = entry;
    head->Flink = entry;
    ASSERT(IsNodeConnected(entry));

}


/*****************************************************************************
 * FUNCTION
 *  _RemoveHeadList
 * DESCRIPTION
 *  Remove the first entry on the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 * RETURNS
 *  
 *****************************************************************************/
ListEntry *_RemoveHeadList(ListEntry *head)
{
    ListEntry *first;

    first = head->Flink;
    first->Flink->Blink = head;
    head->Flink = first->Flink;
    ASSERT(IsListCircular(head));
    return (first);

}


/*****************************************************************************
 * FUNCTION
 *  RemoveHeadList
 * DESCRIPTION
 *  
 * PARAMETERS
 *  head        [OUT]     
 * RETURNS
 *  
 *****************************************************************************/
ListEntry *RemoveHeadList(ListEntry *head)
{
    ASSERT(IsListCircular(head));
    return (_RemoveHeadList(head));
}


/*****************************************************************************
 * FUNCTION
 *  RemoveEntryList
 * DESCRIPTION
 *  Remove the given entry from the list.
 * PARAMETERS
 *  entry       [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void RemoveEntryList(ListEntry *entry)
{
    ASSERT(IsListCircular(entry));
    entry->Blink->Flink = entry->Flink;
    entry->Flink->Blink = entry->Blink;
    ASSERT(IsListCircular(entry->Blink));
    InitializeListEntry(entry);

}


/*****************************************************************************
 * FUNCTION
 *  IsNodeOnList
 * DESCRIPTION
 *  Determine if an entry is on the list specified by head.
 * PARAMETERS
 *  head        [OUT]     
 *  node        [OUT]     
 * RETURNS
 *  TRUE - the entry is on the list.
 *  FALSE - the entry is not on the list.
 *****************************************************************************/
BOOL IsNodeOnList(ListEntry *head, ListEntry *node)
{
    ListEntry *tmpNode;

    ASSERT(IsListCircular(head));
    tmpNode = GetHeadList(head);

    while (tmpNode != head)
    {
        if (tmpNode == node)
        {
            return (TRUE);
        }

        tmpNode = tmpNode->Flink;
    }
    return (FALSE);

}


/*****************************************************************************
 * FUNCTION
 *  MoveList
 * DESCRIPTION
 *  Moves a list to a new list head.  The source list is left empty.
 * PARAMETERS
 *  dest        [OUT]     
 *  src         [OUT]     
 * RETURNS
 *  void
 *****************************************************************************/
void MoveList(ListEntry *dest, ListEntry *src)
{
    ASSERT(IsListCircular(src));
    dest->Flink = src->Flink;
    dest->Blink = src->Blink;
    src->Flink->Blink = dest;
    src->Blink->Flink = dest;
    InitializeListHead(src);
}


/*****************************************************************************
 * FUNCTION
 *  IsListCircular
 * DESCRIPTION
 *  Determine if the list is circular. This is only used for
 *  debugging purposes.
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  TRUE - the list is circular
 *  FALSE - the list is not circular.
 *****************************************************************************/
BOOL IsListCircular(ListEntry *list)
{

    ListEntry *tmp = list;

    if (!IsNodeConnected(list))
    {
        return (FALSE);
    }

    for (tmp = tmp->Flink; tmp != list; tmp = tmp->Flink)
    { 
        if (!IsNodeConnected(tmp))
        {
            return (FALSE);
        }
    }
    return (TRUE);

}


/*****************************************************************************
 * FUNCTION
 *  InitializeListHead
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void InitializeListHead(ListEntry *list)
{
    list->Flink = list;
    list->Blink = list;
}


/*****************************************************************************
 * FUNCTION
 *  InitializeListEntry
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void InitializeListEntry(ListEntry *list)
{
    list->Flink = 0;
    list->Blink = 0;
}


/*****************************************************************************
 * FUNCTION
 *  IsListEmpty
 * DESCRIPTION
 *  
 * PARAMETERS
 *  list        [IN]     
 * RETURNS
 *  
 *****************************************************************************/
U8 IsListEmpty(ListEntry *list)
{
    return ((list)->Flink == (list));
}







