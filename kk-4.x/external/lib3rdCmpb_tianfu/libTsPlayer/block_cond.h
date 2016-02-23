/******************************************************************************
FileName: block_cond.h
Copyright (c) 2012, org. All Rights Reserved.

Version: Ver 1.0    
Author: deeve (deeve@dioux.org Dept:freedom)

Description:
		block condition operation

Note:
		none
History:
Version      Date                  Author       Modification
-------    ----------    -------    ------------
1.0              2012-3-10    deeve        Create
*******************************************************************************/
#ifndef __BLOCK_COND_H__
#define __BLOCK_COND_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

/******************************************************************************
 *                                 Declar Functions                           *
 ******************************************************************************/

int block_cond_init(int i4_init_value);
int block_cond_destroy(void);

int trigger_block_entry(int i4_waiting_value);

int trigger_block_exit(int i4_waiting_value);

int wait_block_out(int i4_waiting_value);

int get_block_value(void);


#ifdef __cplusplus
}
#endif

#endif  /* __BLOCK_COND_H__ */

/* EOF */

