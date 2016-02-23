/******************************************************************************
FileName: ipc_msgq.h
Copyright (c) 2011, MediaTek(ShenZhen)Inc. All Rights Reserved.

Version: Ver 1.0    
Author: mawei.ma (mawei.ma@mediatek.com Dept:DTV1)

Description:
		ipc msgq for android

Note:
		none
History:
Version      Date                  Author       Modification
-------    ----------    -------    ------------
1.0              2011-12-28    mawei.ma        Create
*******************************************************************************/
#ifndef __IPC_MSGQ_H__
#define __IPC_MSGQ_H__

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "listop.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/
#define MAX_MSGQ_NUMS         (8)  


typedef struct _my_msgbuf {
    struct list_head  list;
    void*                 pv_data_buf;
    int                     i4_data_size;
} MSG_BUF_T;


/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

/**
 * msgq_create.
 * create a message queue, you can create less than MAX_MSGQ_NUMS message queue in a process
 *
 * @param[out]    pi4_msgq_handle    a handle used by other function in this model
 *
 * @return
 * @retval 0       success
 * @retval -1      fail
 *
 * @authors    mawei.ma
 * @date         2011/12/29
 */
int msgq_create(int* pi4_msgq_handle);

/**
 * msgq_delete.
 * delete a created message queue by msgq_create()
 *
 * @param[in]    i4_msgq_handle   the handle return by msgq_create()
 *
 * @return
 * @retval 0       success
 * @retval -1      fail
 *
 * @authors    mawei.ma
 * @date         2011/12/29
 */
int msgq_delete(int i4_msgq_handle);

/**
 * msgq_send.
 * send a message data to a message queue
 *
 * @param[in]    i4_msgq_handle   the handle return by msgq_create()
 * @param[in]    pv_msgq_buf       message data buffer
 * @param[in]    i4_msgq_size       message data length
 *
 * @return
 * @retval 0       success
 * @retval -1      fail
 *
 * @see msgq_recv
 * @authors    mawei.ma
 * @date         2011/12/29
 */
int msgq_send(int i4_msgq_handle, void* pv_msgq_buf,  int i4_msgq_size);

/**
 * msgq_recv.
 * receive a message from a message queue
 *
 * @param[in]          i4_msgq_handle   the handle return by msgq_create()
 * @param[out]        pv_msgq_buf       message data buffer which is stored received message
 * @param[in/out]    pi4_msgq_size     message data length, indicate the length of pv_msgq_buf,out for real message length
 * @param[in]          ui4_timeout_ms   block time in millisecond
 *                                                0 --------------- not block
 *                                               (unsigned int)(-1) -- block until receive a message
 *                                                other value ------- block time at most, in other words, once a message had send by msgq_send,
 *                                                                              this function will return with a received message immediately
 *
 * @return
 * @retval 0       success
 * @retval -1      fail
 *
 * @note if i4_msgq_size is not large enough to store a sent message, some data will be dropped.
 * @see  msgq_send
 * @authors    mawei.ma
 * @date         2011/12/29
 */
int msgq_recv(int i4_msgq_handle, void* pv_msgq_buf,  int* pi4_msgq_size, unsigned int ui4_timeout_ms);


#ifdef __cplusplus
}
#endif


#endif  /* __IPC_MSGQ_H__ */

/* EOF */

