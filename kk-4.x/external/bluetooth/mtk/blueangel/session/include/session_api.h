#ifndef __SESSION_API_H__
#define __SESSION_API_H__

#include "bt_mmi.h"
#if 1//def  __MTK_BT_EXTERNAL_PLATFORM__
#include "utils/Log.h"

#define XLOGV ALOGV
#define XLOGI ALOGI
#define XLOGD ALOGD
#define XLOGW ALOGW
#define XLOGE ALOGE
#endif

#define DEFAULT_SESSION_NAME			"bt.session.default"
//#define DEFAULT_SESSION_NAME			"/data/@btmtk/bt.session.default"
#define SESSION_NAME_MAX_LENGTH 		100
#define MAX_SESSION_COUNT 64


typedef enum _SESSION_RESULT
{
	SR_NOERROR,	
	SR_PENDING,
	SR_E_GENERAL, 				//genernal error
	SR_E_NO_CONNECTION, 		//no session connection yet
	SR_E_INVALID_PARAM, 		//invalid parameter
	SR_E_EXCEED_LIMITATION,		//
	SR_E_INTR,					//operation is interrupt	
	SR_E_NOT_SUPPORT,			//operation is interrupt	
	SR_E_CONFLICT,			//
	SR_E_FORBIDEN,
	SR_E_INUSE,
	SR_E_RESOURCE,
	SR_E_NO_ENTRY,
	SR_E_ALREADY_COMPLETE,	
	SR_E_IN_PROGRESS
} SESSION_RESULT;


enum
{
	SESSION_EVENT_CREATED			= 0x0001,
	SESSION_EVENT_DESTROYED			= 0x0002, //equal to exception	
	SESSION_EVENT_DATA_INCOMING		= 0x0004,
	SESSION_EVENT_BUFFER_WRITABLE	= 0x0008,

	SESSION_EVENT_INVALID			= 0xFFFF,
};
typedef unsigned short SESSION_EVENT ;

#define SESSION_ID_RANGE 100 

typedef enum _SESSION_PROFILE
{
	PROFILE_RESERVED = NUM_OF_PROFILE,

	PROFILE_CONTROL 			= PROFILE_RESERVED + 1, // control id for each session group

	PROFILE_JSR82_DATA 			= PROFILE_CONTROL + 1,
	PROFILE_JSR82_DATA_RANGE 	= PROFILE_JSR82_DATA + SESSION_ID_RANGE,

	PROFILE_HDP_DATA 			= PROFILE_JSR82_DATA_RANGE + 1,
	PROFILE_HDP_DATA_RANGE 		= PROFILE_HDP_DATA + SESSION_ID_RANGE,

	INTERNAL_RESERVED			= 0xF000,
	INTERNAL_RESERVED_END		= INTERNAL_RESERVED + SESSION_ID_RANGE,
	
} SESSION_PROFILE;


typedef enum _SESSION_DIRCETION
{
	DIRECTION_INPUT = 0x01,
	DIRECTION_OUTPUT = 0x02,
} SESSION_DIRECTION;

typedef struct _Session_Event_PARM
{
	SESSION_EVENT 	event;
	U16				sessionid;
	U16				subsessionid;
	U32				index;
} Session_Event_PARM;


typedef SESSION_RESULT (*EventCallback) (Session_Event_PARM *event);

SESSION_RESULT make_internal_session(char *name, int namesize, U16 ids[2], U16 subsessionids[2]);
SESSION_RESULT session_connect(char *name, int namesize, U16 id, U16 *subsessionid);
SESSION_RESULT session_setup(char *name, int namesize);
void session_destroy(char *name, int namesize, U16 sessionid, U16 subsessionId);
SESSION_RESULT session_register_handler(char *name, 
											  int namesize, 
											  int start_id, 
											  int end_id, 
											  EventCallback 	callback,
											  U16 *hId);
SESSION_RESULT session_unregister_handler(char *name, 
											  int namesize, 
											  U16 hId);
int session_write(char *name, int namesize, U16 sessionid, U16 subsessionid, void *buffer, int size);
int session_write_control(char *name, int namesize, U16 sessionid, U16 subsessionid, void *ctl, int ctllen, void *data, int datalen);
int session_read(char *name, int namesize, U16 sessionid, U16 subsessionid, void *buffer, int size);

SESSION_RESULT session_avaliable(char *name, int namesize, U16 id, U16 subsessionid, SESSION_DIRECTION direction, int *size);

SESSION_RESULT session_get_fd(char *name, int namesize, U16 sessionid, U16 subsessionid, int* fd);
SESSION_RESULT session_lock(char *name, int namesize, U16 sessionid, U16 subsessionid, BT_BOOL lock);

#endif

