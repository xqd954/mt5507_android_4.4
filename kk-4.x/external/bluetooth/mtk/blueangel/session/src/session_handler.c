#include "session.h"

/********** handler utility *****/
//TODO: it can be compeleted later to compare session id range

static U16 handler_identify = 0;


typedef int (*compare_list_func)(ListEntry *entry1, ListEntry *entry2);

U16 get_handler_identify()
{
	return handler_identify ++;
}

int compare_handler(ListEntry *entry1, ListEntry *entry2)
{
	Session_Handler *handler1 = (Session_Handler *)entry1;
	Session_Handler *handler2 = (Session_Handler *)entry2;
	if (handler1->start_session_Id > handler2->end_session_Id)
	{
		return 1;
	}
	else if (handler2->start_session_Id > handler1->end_session_Id)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SESSION_RESULT allocate_handler(U16 startid, U16 endid, EventCallback callback, Session_Handler **handler)
{
	SESSION_RESULT sret = SR_NOERROR;
	Session_Handler *shandler = NULL;	

	if (NULL == callback)
	{
		return SR_E_INVALID_PARAM;		
	}

	shandler = (Session_Handler *)malloc(sizeof(Session_Handler));
	if (NULL == shandler)
	{
		return SR_E_RESOURCE;
	}

	InitializeListEntry(&shandler->node);
	
	shandler->start_session_Id = (startid < endid) ? startid : endid;
	shandler->end_session_Id = (startid < endid) ? endid : startid;
	shandler->callback = callback;
	shandler->identify = get_handler_identify();
	*handler = shandler;
	return SR_NOERROR;
}

SESSION_RESULT recycle_handler(Session_Handler *handler)
{
	if (NULL == handler)
	{
		return SR_E_INVALID_PARAM;
	}
	free(handler);	
	return SR_NOERROR;
}



