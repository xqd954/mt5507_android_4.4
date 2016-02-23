#ifndef __SESSION_MASTER_H__
#define __SESSION_MASTER_H__

#include "session_api.h"
#include "session.h"

SESSION_RESULT master_start_work(Session_Group *group);
SESSION_RESULT master_stop_work(Session_Group *group);
SESSION_RESULT master_triger_event(Session_Group *group);
SESSION_RESULT master_monitor_event(Session_Group *group, Session_Poll_Fd* pfds);

SESSION_RESULT master_handle_event(Session_Group *group, Session_Poll_Fd* pfds);



#endif 
