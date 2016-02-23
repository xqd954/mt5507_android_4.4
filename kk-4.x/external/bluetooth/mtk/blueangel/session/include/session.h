#ifndef __SESSION_H__
#define __SESSION_H__

#include "bttypes.h"
#include "utils.h"

#include "session_api.h"
#include "session_stream.h" 
#include "session_os.h"

#define MAX_SUPPORTED_CLIENT_NUM 10
#define MAX_SUPPORTED_SERVER_NUM 10
#define MAX_SUPPORTED_SESSION_NUM (MAX_SUPPORTED_CLIENT_NUM+MAX_SUPPORTED_SERVER_NUM)

#define MAX_GROUP_COUNT			4

/*session name format: */
/* client: service group name + "\\"  + sessionid +"\\client\\"+ subsessionid*/
/* server: service group name + "\\"  + sessionid +"\\server"*/
#define SESSION_NAME_CLIENT_PREFIX			"bt.adp.client"
#define SESSION_NAME_SERVER_PREFIX			"bt.adp.server"
#define SESSION_NAME_SEPERATOR 				"\\\\"
//#define SESSION_NAME_SEPERATOR 				"//"
#define SESSION_NAME_SUFFIX_CLIENT 			".client"
#define SESSION_NAME_SUFFIX_SERVER 			".server"
#define SESSION_NAME_FORMAT_CLIENT 			"%s"SESSION_NAME_SEPERATOR"%d"SESSION_NAME_SEPERATOR"%d"SESSION_NAME_SEPERATOR"%d"
#define SESSION_NAME_FORMAT_SERVER 			"%s"


#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

typedef enum _SESSION_THREAD_STATE
{
	SESSION_THREAD_STATE_IDLE,
	SESSION_THREAD_STATE_ACTIVE,
} SESSION_THREAD_STATE;


typedef enum _SESSION_STATE
{
	SESSION_STATE_NONE,		
	SESSION_STATE_ACTIVE,
	SESSION_STATE_MONITOR,
	SESSION_STATE_ZOMBIE, // abnormal state
} SESSION_STATE;

typedef enum _GROUP_STATE
{
	GROUP_STATE_IDLE,
	GROUP_STATE_ACTIVE,
} GROUP_STATE;


typedef enum _SESSION_ROLE
{
	SESSION_ROLE_CLIENT,
	SESSION_ROLE_SERVER,
} SESSION_ROLE;





/*client/server mode*/
/*4. 	about state: when the session is in monitor, if other module wants to destroy this session, */
/*		just set state to zombie. And process it later*/
typedef struct _Session_Context
{
	ListEntry		node;
	SESSION_STATE 	state;
	
	/*for client side: specified by app*/
	/*for server sied: retrieved from socket info*/
	U8				session_name[SESSION_NAME_MAX_LENGTH];
	
	U16 			sessionId;	
	
	/*subsession id is valid in session client*/
	/*that is several cients bind to server.*/
	/*in server side, the field handler is leaved to implements*/
	U16 			subsessionId;

	//attribute of all sessions in a manager, that is session manager's attribute
//	SESSION_ROLE	role;
	
	int 			refCount;
	int 			fd;		

	SESSION_EVENT	event;
	BOOL			locked; //when locked, no any interest on all events

	Session_Mutex	*mutex;
		
} Session_Context;

typedef struct _Session_Master
{
	BOOL			working;
	int 			ext_fd; //used to triger control flow: add/remove session
	int				int_fd; //used to listen the control event
} Session_Master;

/*Notes: 														*/ 
/*1. 	all session in a list have the same session name and the same stream	*/
/*2. 	session is connection-oriented, so there is a server socket to wait for remote connection*/
/*3.		there is a thread to monitor all session in this list, excluding server socket*/
typedef struct _Session_Group
{
	GROUP_STATE		state;
	U8				group_name[SESSION_NAME_MAX_LENGTH];
	Session_Stream	*stream;
	
	int				session_count;
	ListEntry		session_list;

	//the waiting thread is waiting for remote session connection
//	int				waiting_thread_id;
	SESSION_THREAD_STATE waiting_thread_state;
	int				waiting_fd;

	Session_Master	master;

	//the moniter thread poll all active session connection
	SESSION_THREAD_STATE			monitor_thread_state;

	ListEntry		handler_list;
	
	Session_Mutex	*mutex;
} Session_Group;

typedef struct _Session_Handler
{
	ListEntry		node;
	U16				identify;
	U8				session_name[SESSION_NAME_MAX_LENGTH];
	U16				start_session_Id;
	U16				end_session_Id;
	EventCallback 	callback;
} Session_Handler;


typedef struct _Session_Manager
{
	BOOL			init;
	SESSION_ROLE	role;

	//the total_count can not be larger than MAX_SESSION_COUNT to reduce system overhead
	int				total_count;
	Session_Group	group[MAX_GROUP_COUNT];

	Session_Mutex	*mutex;
} Session_Manager;



/***********************************************/
/*******************     session API	****************/
/***********************************************/
SESSION_RESULT compose_client_session_name(char*	name, 
													int	namesize,
													U16 	id, 
													char*	clientname, 
													int 	clientnamesize,
													U16	subsessionid);
SESSION_RESULT compose_server_session_name(char*	name, 
													  int	namesize,
													  char*	servername, 
													  int 	servernamesize);

SESSION_RESULT parse_session_name(char *name, 
										int	namesize,
										char *servicename,
										int servicenamesize,
										U16 *sessionId, 
										U16 *subsessionid);

SESSION_RESULT allocate_session(Session_Context **session);
SESSION_RESULT recycle_session(Session_Context *session);
SESSION_RESULT lock_session_signal(Session_Context *session, BT_BOOL lock);
SESSION_RESULT initialize_session(Session_Context *session, 
									char			*name,
									int				size,
									U16				sessionid,
									U16				subsessionid,
									int 			fd);
SESSION_RESULT prepare_recycle_session(Session_Context *session);
SESSION_RESULT prepare_monitor_session(Session_Context *session);
BOOL is_garbage_session(Session_Context *session);
SESSION_RESULT start_monitor_readable(Session_Context *session);
SESSION_RESULT start_monitor_writable(Session_Context *session);
BOOL stop_monitor_event(Session_Context *session);
SESSION_EVENT update_event(Session_Context *session, SESSION_EVENT event);
BOOL signal_is_locked(Session_Context *session);

/***********************************************/
/*******************     handler API	****************/
/***********************************************/

SESSION_RESULT allocate_handler(U16 startid, U16 endid, EventCallback callback, Session_Handler **handler);
SESSION_RESULT recycle_handler(Session_Handler *handler);

/***********************************************/
/*******************     group API	****************/
/***********************************************/

SESSION_RESULT group_init(Session_Group *group, char*	name, int namesize);
void group_deinit(Session_Group *group);
SESSION_RESULT match_group(Session_Group *group, char* name, int namesize);
SESSION_RESULT on_session_connected(Session_Group *group, char *name, int size, int fd);
SESSION_RESULT start_waiting_thread(Session_Group *group);
static SESSION_RESULT stop_waiting_thread(Session_Group *group);
SESSION_RESULT start_monitoring_thread(Session_Group *group);
SESSION_RESULT stop_monitoring_thread(Session_Group *group);
SESSION_RESULT insert_handler(Session_Group *group, Session_Handler *handler);
SESSION_RESULT remove_handler(Session_Group *group, Session_Handler *hanlder);
SESSION_RESULT find_handler(Session_Group *group, U16 sessionid, Session_Handler **handler);
SESSION_RESULT find_handler_by_identify(Session_Group *group, U16 identify, Session_Handler **handler);
SESSION_RESULT insert_session(Session_Group *group, Session_Context *session);
SESSION_RESULT remove_session(Session_Group *group, Session_Context *session);
SESSION_RESULT find_first_session(Session_Group *group, Session_Context **cursor);
SESSION_RESULT find_next_session(Session_Group *group, Session_Context **cursor);
SESSION_RESULT find_session_by_fd(Session_Group *group, int fd, Session_Context **session);
SESSION_RESULT find_session_by_id(Session_Group *group, U16 sessionid, U16 subsessionid, Session_Context **session);
SESSION_RESULT check_session_id_conflict(Session_Group *group, U16 sessionid, U16 subsessionid);
SESSION_RESULT get_default_subsession_id(Session_Group *group, U16 *subsessionid);
SESSION_RESULT get_reserved_session_id(Session_Group *group, U16 *sessionid);

#endif
