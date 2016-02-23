#include "session_stream.h" 

#include <sys/socket.h>
#include <sys/ioctl.h>
#include "cutils/sockets.h"
#include <stdlib.h>
#include <errno.h>
#include <poll.h>

static int create_socket(char *name, int size);
static int connect_socket(int fd, char *destaddr, int size);
static int listen_socket(int fd, int queuelimit);
static int accept_socket(int fd, char *dest, int destsize);
static void close_socket(int fd);
static int read_socket(int fd, void *buffer, int size);
static int write_socket(int fd, void *buffer, int size);
static int write_control_socket(int fd, void *ctl, int ctllen, void *data, int datalen);
static int poll_socket(Session_Poll_Fd *fds, int count, int timeout);
static SESSION_RESULT get_buffer_size(int fd, SESSION_DIRECTION direction, int *size);
static SESSION_RESULT get_data_length(int fd, SESSION_DIRECTION direction, int *size);
static SESSION_RESULT create_socket_pair(int fds[]);

static Session_Stream session_stream = {
	"socket",
	create_socket,
	create_socket_pair,
	connect_socket,
	listen_socket,
	accept_socket,
	close_socket,
	read_socket,
	write_socket,
	write_control_socket,
	poll_socket,
	get_buffer_size,
	0,
	get_data_length,
};

//revert socket event to session event
static SESSION_EVENT revert_socket_event(unsigned int event)
{
	SESSION_EVENT sevent = 0;

	if (event & POLLIN)
	{
		sevent |= SESSION_EVENT_DATA_INCOMING;
	}

	if (event & POLLOUT)
	{
		sevent |= SESSION_EVENT_BUFFER_WRITABLE;
	}

	if (event & (POLLHUP | POLLRDHUP | POLLERR | POLLNVAL))
	{
		session_log("[session]POLLHUP: %2x, POLLRDHUP: %2x, POLLERR: %2x, POLLNVAL: %2x", 
								POLLHUP,
								POLLRDHUP,
								POLLERR,
								POLLNVAL);

		session_log_err("[session]revert_socket_event: %2x", event);
		sevent |= SESSION_EVENT_DESTROYED;
	}
	return sevent;
}

//convert session event to socket event
static unsigned int convert_socket_event(SESSION_EVENT event)
{
	unsigned int revent = 0;

	if (event & SESSION_EVENT_DATA_INCOMING)
	{
		revent |= POLLIN;
	}

	if (event & SESSION_EVENT_BUFFER_WRITABLE)
	{
		revent |= POLLOUT;
	}

	if (event & SESSION_EVENT_DESTROYED)
	{
		revent |= (POLLHUP | POLLRDHUP | POLLERR | POLLNVAL);
	}
	return revent;
}


static int convert_direction(SESSION_DIRECTION direction)
{
	switch (direction)
	{
	case DIRECTION_INPUT:
		return FIONREAD;

	case DIRECTION_OUTPUT:
//		return FIONWRITE;

	default:
		return -1;
	}
}



Session_Stream *get_stream(SESSION_STREAM_TYPE type)
{
	switch(type)
	{
	case STREAM_TYPE_SOCKET:
		return &session_stream;

	default:
		return NULL;
	}
}



static int create_socket(char *name, int size)
{
	struct sockaddr_un sockname;
    socklen_t   socknamelen;
	int fd;


	
	fd = socket(AF_UNIX, SOCK_STREAM, 0);


	if (fd < 0)
    {
        goto exit;
    }
	memset(&sockname, 0x0, sizeof(struct sockaddr_un));
	sockname.sun_family = AF_UNIX;  
	
    sockname.sun_path[0] = 0;
    strcpy(sockname.sun_path + 1, name);
    socknamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (sockname.sun_path + 1) + 1);
  		
    if (bind(fd, (struct sockaddr *) &sockname, socknamelen) < 0)
		
#if 0
    sockname.sun_family = AF_UNIX;
    strcpy (sockname.sun_path, name);
    socknamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (sockname.sun_path) + 1);   
    fd = socket_local_server(name, 
							ANDROID_SOCKET_NAMESPACE_ABSTRACT, 
							SOCK_STREAM);
	if (fd < 0)	
#endif
    {
    	session_log("[Session][stream]bind socket failed : local [%s], error info[%s], errno=[%d]", name, strerror(errno), errno);
     	close(fd);
		fd = -1;
        goto exit;
    }
    else
    {
        socknamelen = sizeof(sockname.sun_path);
        sockname.sun_path[0] = '\0';
        if (getsockname(fd, (struct sockaddr*)&sockname, &socknamelen) < 0)
        {
            session_log("[Session][stream]getsockname failed : %s, errno=%d", strerror(errno), errno);
			close(fd);
			goto exit;
        }
        else
        {
            session_log("[Session][stream]Auto bind server : len=%d, addr=%s", socknamelen, &sockname.sun_path[1]);
        }

		int on = 1;

		//trun on credentials passing
	/*	if((setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on))) < 0)
		{
			session_log_err("[ERR] set socket as credentials passing fail: %s errno=%d", strerror(errno), errno);
		}
		*/
    }
exit:
	return fd;
}
static int listen_socket(int fd, int queuelimit)
{
	int ret;
	int on = 1;

	ret = listen(fd, queuelimit);	
	if(ret < 0)
	{
		session_log_err("[Session][stream]listen failed : error info[%s], errno=[%d]", strerror(errno), errno);
	}

	//trun on credentials passing
/*	if((setsockopt(fd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on))) < 0)
	{
		session_log_err("[ERR] set socket as credentials passing fail: %s errno=%d", strerror(errno), errno);
	}
	*/
	
	return ret;
}


static int connect_socket(int fd, char *destaddr, int size)
{
	struct sockaddr_un sockname;
	socklen_t	socknamelen;
	int iResult = 0;

	//server socket
	memset(&sockname, 0x0, sizeof(struct sockaddr_un));
	sockname.sun_family = AF_UNIX;
	sockname.sun_path[0] = '\0';
    strncpy (sockname.sun_path + 1, destaddr, size);
    socknamelen = (offsetof (struct sockaddr_un, sun_path) + strlen (sockname.sun_path + 1) + 1); 
	iResult = connect(fd, (struct sockaddr *)&sockname, socknamelen);
	if(iResult < 0)
    {
    	session_log("[Session][stream]connect failed : remote [%s], error info[%s], errno=[%d]", destaddr, strerror(errno), errno);
	}

	return iResult;	
}
static int accept_socket(int fd, char *dest, int destsize)
{
	struct sockaddr_un addr;
	socklen_t addrlen;
	int sockfd;

	memset(&addr, 0x0, sizeof(struct sockaddr_un));
	addrlen = sizeof(struct sockaddr_un);
	sockfd = accept(fd, (struct sockaddr *)&addr, &addrlen);	
	if(sockfd < 0)
	{
		session_log("[Session][stream]accept failed : error info[%s], errno=[%d]", strerror(errno), errno);
	}
	else
	{
		strncpy(dest, &addr.sun_path[1], addrlen);	
	}
	return sockfd;
}

static void close_socket(int fd)
{
	shutdown(fd, SHUT_RDWR);
	close(fd);
}

static int read_socket(int fd, void *buffer, int size)
{
	int flag = 0;
	int ret = 0;

	//for common usage, no need to set this condition
//	flag |= MSG_WAITALL;
	flag |= MSG_DONTWAIT;
	ret = recv(fd, buffer, size, flag);
	if (ret < 0)
	{
		if(errno == EAGAIN)
		{
			ret = 0;
		}
		else
		{
			session_log_err("[Session][stream]read_socket error %s, %d", strerror(errno), errno);
		}
	}
	return ret;
}
static int write_control_socket(int fd, void *ctl, int ctllen, void *data, int datalen)
{
    ssize_t ret;
    struct msghdr msg;
    unsigned char *buffer = (unsigned char *)data;
    memset(&msg, 0, sizeof(msg));

    struct cmsghdr *cmsg;
    char msgbuf[CMSG_SPACE(1)];
    if(fd == -1)
        return -1;

	session_log("[session][stream]write_control_socket, fd %d, ctl %d, len %d, datalen %d, CMSG_SPACE(1)[%d]",
					fd, *((int *)ctl), ctllen, datalen, CMSG_SPACE(1));
	//test
    msg.msg_control = msgbuf;
    msg.msg_controllen = sizeof(msgbuf);
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(ctllen);
 	memcpy(CMSG_DATA(cmsg), ctl, ctllen);

/*	int newfd = *((int *)ctl);
	newfd = 65500;
   	cmsg->cmsg_len = CMSG_LEN(ctllen);
	memcpy(CMSG_DATA(cmsg), &newfd, ctllen);
	*/
	
    // We only write our msg_control during the first write
    int ret_len = datalen;
    while (datalen > 0) {
        struct iovec iv;
        memset(&iv, 0, sizeof(iv));

        iv.iov_base = buffer;
        iv.iov_len = datalen;

        msg.msg_iov = &iv;
        msg.msg_iovlen = 1;

        do {
            ret = sendmsg(fd, &msg, MSG_NOSIGNAL);
        } while (ret < 0 && errno == EINTR);

        if (ret < 0) {
            session_log_err("[session]fd:%d, sendmsg ret:%d, errno:%d, %s",
                              fd, (int)ret, errno, strerror(errno));
            ret_len = -1;
            break;
        }

        buffer += ret;
        datalen -= ret;

        // Wipes out any msg_control too
        memset(&msg, 0, sizeof(msg));
    }
    return ret_len;
}

int write_socket(int fd, void *buffer, int size)
{
	int flag = 0;
	int ret;
	ret = send(fd, buffer, size, flag);
	if (ret < 0)
	{
		session_log_err("[Session][stream]write_socket error %s, %d", strerror(errno), errno);
	}
	return ret;
}


static int poll_socket(Session_Poll_Fd *fds, int count, int timeout)
{
	struct pollfd pollfds[MAX_SESSION_COUNT];
	int index = 0;
	int ret;

	memset(pollfds, 0x0, sizeof(struct pollfd)*MAX_SESSION_COUNT);
	
	for (index = 0; index < count ; index ++)
	{
		pollfds[index].fd = fds[index].fd;	
		pollfds[index].events = convert_socket_event(fds[index].interested_evts);
	}
	ret = poll(pollfds, count, timeout);
	if (ret < 0)
	{
		session_log("[Session][stream]poll error %s, %d", strerror(errno), errno);
		return ret;
	}
	else if (ret == 0)
	{
		session_log("[Session][stream]poll timeout: %d", timeout);
		return ret;		
	}
	for (index = 0; index < count ; index ++)
	{
		fds[index].actual_evts = revert_socket_event(pollfds[index].revents);	
	}
	return ret;
}

/*

int parse_socket_name(int fd, Session_Context *context)
{
	//find available session
	struct sockaddr addr;
	struct soclen addrlen;
	char sessionname[SESSION_NAME_MAX_LENGTH];

	getpeername(fd, &addr, &addrlen);

	strncpy(sessionname, addr.sun[0], SESSION_NAME_MAX_LENGTH);
	strncpy(context->session_name, addr.sun[0], SESSION_NAME_MAX_LENGTH);


	char *channelDup = strdup(sessionname);
	char *p = NULL;

	//session prefix
	p = strtok(channelDup, SESSION_NAME_SEPERATOR);	

	//session id
	p = strtok(NULL, SESSION_NAME_SEPERATOR); 
	context->sessionId = atoi(p);	

	//subsession id
	p = strtok(NULL, SESSION_NAME_SEPERATOR); 
	if (p != NULL)
	{
		context->sessionId = atoi(p);
	}

	context->fd = fd;
exit:
	free(channelDup);
	return TRUE;
}
*/

//TODO: this API should be caucious to use to avoid effort
//that means keep the buffer size in first time and update it if outer module wants to change it
static SESSION_RESULT get_buffer_size(int fd, SESSION_DIRECTION direction, int *size)
{
	int operation;
	int optlen = sizeof(int);
	SESSION_RESULT sret = SR_NOERROR;
	switch (direction)
	{
	case DIRECTION_INPUT:
		operation = SO_RCVBUF;
		break;

	case DIRECTION_OUTPUT:
		operation =  SO_SNDBUF;
		break;

	default:
		return SR_E_INVALID_PARAM;
	}
	
	if(getsockopt(fd, SOL_SOCKET, operation, (void*)size, &optlen) == 0)
	{
		session_log("[Session][stream]getsockopt size:%d, fd:%d", *size, fd);
    }
	else
    {
    	session_log("[Session][stream]getsockopt error, errno:[%s][%d], fd:%d", strerror(errno),errno, fd);
		sret = SR_E_GENERAL;
    }
	return sret;
}

static SESSION_RESULT get_data_length(int fd, SESSION_DIRECTION direction, int *size)
{
	int operation = convert_direction(direction);

	if (operation < 0)
	{
		return SR_E_INVALID_PARAM;
	}
	
	if(ioctl(fd, operation, size) == 0)
	{
		session_log("[Session][stream]ioctl avaiable size:%d, fd:%d", *size, fd);
    }
	else
    {
    	session_log("[Session][stream]ioctl error, errno:[%s][%d], fd:%d", strerror(errno),errno, fd);
    }
	return SR_NOERROR;
}
SESSION_RESULT create_socket_pair(int fds[])
{
	SESSION_RESULT sret = SR_NOERROR;
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0)
	{
		sret = SR_E_GENERAL;
		session_log_err("[session]fail to create socketpair, errno[%d][%s]", errno, strerror(errno));		
	}
	return sret;
}



