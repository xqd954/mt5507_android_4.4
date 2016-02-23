#include "session_master.h"
#include <sys/socket.h>
#include <sys/types.h>

SESSION_RESULT master_start_work(Session_Group *group)
{
	int fds[2];
	SESSION_RESULT sret = SR_NOERROR;
	Session_Master *master = NULL;

	if (TRUE == group->master.working)
	{
		session_log_err("[session]master_start_work, master already started");
		return SR_E_ALREADY_COMPLETE;
	}

	if(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
	{
		sret = SR_E_GENERAL;
		session_log_err("[session]fail to create socketpair");
		goto exit;
	}

	master = &group->master;
	if (master->working)
	{
		sret = SR_E_ALREADY_COMPLETE;
	}
	master->ext_fd = fds[0];
	master->int_fd = fds[1];
	master->working = TRUE;

exit:
	return sret;
}

SESSION_RESULT master_stop_work(Session_Group *group)
{
	if (FALSE == group->master.working)
	{		
		return SR_E_ALREADY_COMPLETE;
	}
	if (group->master.ext_fd > 0)
	{		
		group->stream->close_stream(group->master.ext_fd);
		group->master.ext_fd = -1;
	}
	if (group->master.int_fd > 0)
	{		
		group->stream->close_stream(group->master.int_fd);
		group->master.int_fd = -1;
	}
	group->master.working = FALSE;
	return SR_NOERROR;
}
SESSION_RESULT master_triger_event(Session_Group *group)
{
	int on = 1;
	int length = 0;
	session_log("[session]master_triger_event");
	if (FALSE == group->master.working)
	{
		session_log_err("[session]master not start");
		return SR_E_FORBIDEN;
	}
	length = group->stream->write(group->master.ext_fd, &on, sizeof(on));
	return SR_NOERROR;
}

SESSION_RESULT master_monitor_event(Session_Group *group, Session_Poll_Fd* pfds)
{
	if (FALSE == group->master.working)
	{
		session_log_err("[session]master_monitor_event,master no ready");
		return SR_E_FORBIDEN;
	}
	if(group->master.int_fd <= 0)
	{
		session_log_err("[session]master_monitor_event, int fd is invalid");
		return SR_E_NO_CONNECTION;
	}
	pfds->interested_evts = SESSION_EVENT_DESTROYED	| SESSION_EVENT_DATA_INCOMING;
	pfds->fd = group->master.int_fd;
	return SR_NOERROR;
}


SESSION_RESULT master_handle_event(Session_Group *group, Session_Poll_Fd* pfds)
{
	U8 data[50];
	if (FALSE == group->master.working)
	{
		session_log_err("[session]master_handle_event,master no ready");
		return SR_E_FORBIDEN;
	}
	if(group->master.int_fd == pfds->fd)
	{
		//the data has to be removed from session buffer
		while(group->stream->read(group->master.int_fd, data, 50) <= 0);
		return SR_NOERROR;
	}
	else
	{
		return SR_E_INTR;
	}
}


