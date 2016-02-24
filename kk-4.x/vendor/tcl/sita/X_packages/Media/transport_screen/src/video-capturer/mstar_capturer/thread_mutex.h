/* @All Right Reserved (C), 2009-2109, BIAOQI CO*/

/**
 * @file        thread_mutex.h
 * @author      jiangjq
 * @date        2010/04/17
 * @version     ver 1.0
 * @brief       定义thread_mutex类
 * @attention   
 * @section     History     
    jiangjq     2010/04/17      ver1.0      Created
*/

#ifndef __BIAOQI_THREAD_MUTEX_H_INCLUDED_
#define __BIAOQI_THREAD_MUTEX_H_INCLUDED_

#include <pthread.h>
typedef pthread_t thread_t;
typedef void* ( * thread_function )( void* );



inline int thread_create( thread_t* thread_id, thread_function start_route, void* lpData )
{
  return pthread_create( thread_id, NULL, start_route, lpData );
}

inline int thread_join( thread_t thread_id )
{
  return pthread_join( thread_id, NULL );
}
/**
 * @class       thread_mutex
 * @author      Jiangjq 
 * @date        2010/04/17
 * @version     ver 1.0
 * @brief       封装window和linux平台的mutex操作      
 */
class thread_mutex
{
  public:
    thread_mutex();
    ~thread_mutex();
  public:
    void lock();
    void unlock();
  private:

    pthread_mutex_t m_mutex;
};

#endif
