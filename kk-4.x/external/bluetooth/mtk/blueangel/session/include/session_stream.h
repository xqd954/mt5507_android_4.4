#ifndef __SESSION_STREAM_H__
#define __SESSION_STREAM_H__

#include "session_api.h"
#include "session_os.h"

typedef enum _SESSION_STREAM_TYPE
{
	STREAM_TYPE_SOCKET,
} SESSION_STREAM_TYPE;
typedef struct _Session_Poll_Fd
{
	int 			fd;
	unsigned short 	interested_evts; //insterested event
	unsigned short	actual_evts; //returned event
} Session_Poll_Fd;

typedef struct _Session_Stream
{
	char 	*stream_name;
	int (*create_stream)(char *name, int size);
	SESSION_RESULT (*create_int_stream)(int fds[2]);
	int (*connect_stream)(int fd, char *name, int size);
	int (*listen_stream)(int fd, int limit);
	int (*accept_stream)(int fd, char *dest, int destsize);
	void (*close_stream)(int fd);
	int (*read)(int fd, void *buffer, int size);
	int (*write)(int fd, void *buffer, int size);
	int (*write_control)(int fd, void *ctl, int ctllen, void *data, int datalen);
	int (*poll)(Session_Poll_Fd *fds, int count, int timeout);
	SESSION_RESULT (*get_stream_size)(int fd, SESSION_DIRECTION direction, int *size);
	SESSION_RESULT (*set_stream_size)(int fd, SESSION_DIRECTION direction, int size);
	SESSION_RESULT (*get_stream_length)(int fd, SESSION_DIRECTION direction, int *size);
}Session_Stream;
Session_Stream *get_stream(SESSION_STREAM_TYPE type);
#endif

