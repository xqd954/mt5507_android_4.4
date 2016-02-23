#include "session_os.h" 

pthread_t session_thread_create(Session_Routine start_routine, void * arg)
{
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    pthread_t thread_id = -1;
	int ret = 0;
	ret = pthread_create(&thread_id, &thread_attr, start_routine, arg);
    if(ret !=0 )
    {
        session_log_err("[session]pthread_create : %s", strerror(errno));
        return -1;
    }
    return thread_id;
}
SESSION_RESULT session_thread_wait(Session_Thread *thread, int timeout)
{
	void *status = NULL;
	if (timeout <= 0)
	{
		timeout = 0;
	}
	pthread_join(*thread, &status);
	return SR_NOERROR;
}	



SESSION_RESULT session_mutex_create(Session_Mutex **mutex)
{
	if (*mutex != NULL)
	{
		return SR_E_INUSE;
	}
	*mutex = malloc(sizeof(pthread_mutex_t));
	if (NULL == *mutex)
	{
		return SR_E_RESOURCE;
	}
	memset(*mutex, 0x0, sizeof(Session_Mutex));
//	**mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init((pthread_mutex_t *)(*mutex), NULL);
	return SR_NOERROR;
}

SESSION_RESULT session_mutex_destroy(Session_Mutex *mutex)
{
	if(NULL == mutex)
	{
		return SR_E_INVALID_PARAM;
	}
	pthread_mutex_destroy((pthread_mutex_t *)mutex);
	free(mutex);
	return SR_NOERROR;
}

SESSION_RESULT session_mutex_lock(Session_Mutex *mutex)
{
	if(NULL == mutex)
	{
		return SR_E_INVALID_PARAM;
	}
	pthread_mutex_lock((pthread_mutex_t *)mutex);
	return SR_NOERROR;
}
SESSION_RESULT session_mutex_unlock(Session_Mutex *mutex)
{
	if(NULL == mutex)
	{
		return SR_E_INVALID_PARAM;
	}
	pthread_mutex_unlock((pthread_mutex_t *)mutex);
	return SR_NOERROR;
}

int session_rand()
{
	return rand();
}

SESSION_RESULT Session_Convert_ErrorCode(I32 error)
{
	switch(error)
	{
	case EACCES:
		break;
		
	case EAFNOSUPPORT:
		break;
		
	case EINVAL:
		break;
		
	case ENFILE:
		break;
		
	case ENOBUFS:
		break;
		
	case ENOMEM:
		break;
		
	case EPROTONOSUPPORT:
		break;
		
/*	case EAGAIN:
		break;
		*/
		
	case EBADF:
		break;
		
	case EWOULDBLOCK:
		break;
		
	case EFAULT:
		break;
		
	case EINTR:
		break;
		
		
	case EIO:
		break;
		
	case EISDIR:
		break;
		
	case EDESTADDRREQ:
		break;		
		
	case EFBIG:
		break;
		
	case ENOSPC:
		break;
		
	case EPIPE:
		break;

	default:
		return SR_E_GENERAL;
	}

	return SR_E_GENERAL;
}

