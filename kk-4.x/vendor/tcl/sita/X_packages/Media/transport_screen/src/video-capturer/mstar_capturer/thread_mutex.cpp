#include "thread_mutex.h"


thread_mutex::thread_mutex()
{
  pthread_mutexattr_t mattr;

  pthread_mutexattr_init( &mattr );
  pthread_mutex_init( &m_mutex, &mattr );
}

thread_mutex::~thread_mutex()
{
  pthread_mutex_lock( &m_mutex );
  pthread_mutex_unlock( &m_mutex );
  pthread_mutex_destroy( &m_mutex );
}

void thread_mutex::lock()
{
  pthread_mutex_lock( &m_mutex );
}

void thread_mutex::unlock()
{
  pthread_mutex_unlock( &m_mutex );
}

