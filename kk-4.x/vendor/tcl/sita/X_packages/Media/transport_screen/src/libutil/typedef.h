/*
 * typedef.h
 *
 *  Created on: 2012-4-17
 *      Author: zhangsh
 */

#ifndef TYPEDEF_H_
#define TYPEDEF_H_

typedef enum _Ret
{
	RET_INVALID_PARAMS,
	RET_OK,
	RET_FAIL
}Ret;

#define return_if_fail(p) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n", \
		__func__, __LINE__); return;}
#define return_val_if_fail(p, ret) if(!(p)) \
	{printf("%s:%d Warning: "#p" failed.\n",\
	__func__, __LINE__); return (ret);}


#endif /* TYPEDEF_H_ */
