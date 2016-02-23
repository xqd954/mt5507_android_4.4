/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*******************************************************************************
 *
 * Filename:
 * ---------
 * Bt_debug.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Autumn Li
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:
 * $Modtime:
 * $Log:
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#define LOG_TAG "MTKBT"

#ifndef BTMTK_ON_WISE
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <assert.h>
#include "bt_common.h"
#include "osapi.h"
#ifndef __LINUX_SUPPRESS_ERROR__
#include "win_service.h"
#endif /* __LINUX_SUPPRESS_ERROR__ */

//#define PRINT_TYPE_DEBUG
//#ifndef __USE_CATCHER_LOG__
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#if defined(BTMTK_ON_LINUX)
#include <cutils/log.h>
#endif
//#endif
#include "tst_trace.h"
#include "tst_ringbuffer.h"
#include "bt_adp_system.h"

#define STX_OCTET                   0x55
#define LOGGED_PRIMITIVE_TYPE       0x60

extern void HciTransportReset(void);

#ifndef __USE_CATCHER_LOG__
static char msg_buffer[500] = "[MTKBT]";   /* Output buffer */
#endif

/* For dynamic change log settings */
static kal_uint32 g_tracemask = 0x0;	/* 1 : mask, 0 : unmask */
static kal_uint8	g_primitivemask = 0x0; /* 0 : unmask, others : mask */
static kal_uint8	g_promptmask = 0x0; /* 0 : unmask, others : mask */
extern unsigned int tst_full;


void bt_log_setting(kal_uint32 trace, kal_uint8 primitive, kal_uint8 prompt)
{
	g_tracemask = trace;
	g_primitivemask = primitive;
	g_promptmask = prompt;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_get_frame_number
 * DESCRIPTION
 *  Get GSM frame number
 * PARAMETERS
 *  void
 * RETURNS
 *  frame number
 *****************************************************************************/
U32 btmtk_log_get_frame_number(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static U32 frame = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return ((frame == 0xFFFFFFFF) ? 0 : ++frame);
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_get_timestamp
 * DESCRIPTION
 *  Get sec after system bootup
 * PARAMETERS
 *  void
 * RETURNS
 *  Sec after system bootup
 *****************************************************************************/
U32 btmtk_log_get_timestamp(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    time_t rawtime;
    struct tm* timeinfo; 
    struct timeval tv;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    gettimeofday(&tv, NULL);

    return (U32)(timeinfo->tm_hour*10000000 + timeinfo->tm_min*100000 + timeinfo->tm_sec*1000 + tv.tv_usec / 1000 );
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_indicate_frame_lost (ref. tst_indicate_frame_lost() in tst_create.c)
 * DESCRIPTION
 *  Encapsulate data to RS232 format
 * PARAMETERS
 * RETURNS
 *  void
 *****************************************************************************/
kal_uint8 tst_lostIndicator[64] = {0x55, 0, 1 + sizeof(tst_index_trace_header_struct) + 12, 0x61};
kal_uint16 tst_lostIndicator_len = 0;
#define LOST_WRITE_START_POS 4	

kal_uint32 btmtk_indicate_frame_lost(kal_uint32 droppedTrace, kal_uint32 droppedPrimitive, kal_uint32 others)
{
    bt_tst_dbg("btmtk_indicate_frame_lost(%lu, %lu, %lu)", droppedTrace, droppedPrimitive, others);
    if (droppedTrace > 0 || droppedPrimitive > 0 || others > 0)
    {
        kal_uint32 pos;
        kal_uint8 checksum;
        tst_index_trace_header_struct   trace_header;

        trace_header.frame_number = btmtk_log_get_frame_number();
        trace_header.time_stamp = btmtk_log_get_timestamp();

        trace_header.msg_index = TST_MSG2__enum;
        trace_header.trace_class = TRACE_WARNING;
        trace_header.buf_length = 12; //3 4-byte integer
        tstmemcpy(&tst_lostIndicator[0], LOST_WRITE_START_POS, KAL_FALSE);
        pos = LOST_WRITE_START_POS; 
        tstmemcpy((const U8*)&trace_header, sizeof(tst_index_trace_header_struct), KAL_FALSE);
        pos += sizeof(tst_index_trace_header_struct);
        tstmemcpy((const U8*)&droppedTrace, 4, KAL_FALSE);
        pos += 4;
        tstmemcpy((const U8*)&droppedPrimitive, 4, KAL_FALSE);
        pos += 4;
        tstmemcpy((const U8*)&others, 4, KAL_FALSE);
        pos += 4;
        checksum= tstChecksum();
        tstmemcpy(&checksum, 1, KAL_TRUE);
        return pos;
    }
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_rs232_frame_post_process (ref. FillAndSendFrame() in tst_trace.c)
 * DESCRIPTION
 *  Encapsulate data to RS232 format
 * PARAMETERS
 *  pArgs      [IN]        Frame data structure
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_log_frame_post_process(kal_uint8 *dest, kal_uint8 *src, kal_uint32 *buf_size)
{
    kal_uint8   check_sum = 0, curChar;
    kal_uint32  i, dest_index = 0;

    dest_index = 0;
    check_sum = 0;
    for(i = 0; i < *buf_size + 3; i++)
    {
        curChar = *(src + i);

        if( curChar == 0xA5 )
        {
            *(dest + dest_index)= 0x5A;
            *(dest + dest_index + 1) = 0x01;
            dest_index += 1;
        }
        else if( curChar == 0x5A )
        {
            *(dest + dest_index)= 0x5A;
            *(dest + dest_index + 1) = 0x5A;
            dest_index += 1;
        }
        else
        {
            *(dest + dest_index)= curChar;
        }
        dest_index += 1;
        check_sum = check_sum ^ curChar;
    }

    // don't worry about the space of this byte, we have reserved.
    if (0xA5 == check_sum)
    {
        *(dest + dest_index)= 0x5A;
        *(dest + dest_index + 1) = 0x01;
        dest_index += 1;
    }
    else if (0x5A == check_sum)
    {
        *(dest + dest_index)= 0x5A;
        *(dest + dest_index + 1) = 0x5A;
        dest_index += 1;
    }
    else
    {
        *(dest + dest_index) = check_sum;
    }
    dest_index += 1;

    //make sure this frame can be flush by NewDBG interface (flush in unit of 2)
    if (dest_index % 2)
    {
        *(dest + dest_index) = 0;
        dest_index += 1;
    }

    *buf_size = dest_index;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_fill_and_send_frame (ref. tst_task_main() in tst_create.c)
 * DESCRIPTION
 *  Encapsulate data to RS232 format
 * PARAMETERS
 *  pArgs      [IN]        Frame data structure
 * RETURNS
 *  void
 *****************************************************************************/
static void btmtk_log_fill_and_send_frame(const tst_trace_frame_arg *pArgs, kal_bool bMsg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8   buf[MAX_ILM_BUFFER_SIZE] = {0}, buf2[MAX_ILM_BUFFER_SIZE] = {0};
    kal_uint32  buf_size = 0;
    kal_uint32  back_len;
    kal_uint8 frame_header[2];
    kal_uint8 start_char = STX_OCTET;
    kal_uint8 tmp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    buf_size = 1 + pArgs->lenTraceHeader + pArgs->lenData;
    #if 0
    if(buf_size + 11 >= MAX_ILM_BUFFER_SIZE)
    {
        ALOGE("[ERR] btmtk_log_fill_and_send_frame : lenTraceHeader=%lu, lenData=%lu", pArgs->lenTraceHeader, pArgs->lenData);
        ALOGE("frameMessageType=%lu", pArgs->frameMessageType);
        //printf("[ERR] btmtk_log_fill_and_send_frame : lenTraceHeader=%d\r\n", pArgs->lenTraceHeader);
        //printf("frameMessageType=%d\r\n", pArgs->frameMessageType);
        if(pArgs->frameMessageType == PS_TRACE_INDEX_TRACE_TYPE)
        {
            tst_index_trace_header_struct *pHeader = (tst_index_trace_header_struct*)pArgs->pTraceHeader;
            ALOGE("msg_index=%lu, trace_class=%d", pHeader->msg_index, pHeader->trace_class);
            //printf("msg_index=%d, trace_class=%d\r\n", pHeader->msg_index, pHeader->trace_class);
        }
        else if(pArgs->frameMessageType == LOGGED_PRIMITIVE_TYPE)
        {
            tst_log_prim_header_struct *pHeader = (tst_log_prim_header_struct*)pArgs->pTraceHeader;
            ALOGE("msg_id=%d", pHeader->msg_id);
            //printf("msg_id=%d\r\n", pHeader->msg_id);
        }
        else
        {
            //tst_prompt_trace_header_struct *pHeader = (tst_prompt_trace_header_struct*)pArgs->pTraceHeader;
            ALOGE("len=%lu, prompt=%s", pArgs->lenData, pArgs->pData);
            //printf("len=%d, prompt=%s\r\n", pArgs->lenData, pArgs->pData);
        }

        Assert(buf_size + 11 < MAX_ILM_BUFFER_SIZE);
    }
    #endif
    //frame_header = ((buf_size / 256) << 8) | ((buf_size % 256) << 16);
    frame_header[0] = ((buf_size / 256 )&0xFF);
    frame_header[1] = ((buf_size % 256 )&0xFF);
    back_len = buf_size + 9;
    #if 1
    if( tstChecksize(buf_size+3) )
    {
        if(tst_lost_primitive || tst_lost_trace)
        {
            btmtk_indicate_frame_lost(tst_lost_primitive, tst_lost_trace, 0);
            tst_lost_primitive = tst_lost_trace = 0;
        }
        tstmemcpy(&start_char, 1, KAL_FALSE);
        tstmemcpy(frame_header, sizeof(frame_header), KAL_FALSE);
        tmp = (kal_uint8)pArgs->frameMessageType;
        tstmemcpy(&tmp, 1, KAL_FALSE);
        tstmemcpy(pArgs->pTraceHeader, pArgs->lenTraceHeader, KAL_FALSE);
        tstmemcpy(pArgs->pData, pArgs->lenData, KAL_FALSE);
        tmp = tstChecksum();
        tstmemcpy(&tmp, 1, KAL_TRUE);
    }
    else
    {
        if(bMsg)
            tst_lost_primitive++;
        else
            tst_lost_trace++;
        tst_full = 1;
    }
    #else
    OS_MemCopy(buf, (U8 *)&frame_header, 3);  /* rs232 frame header */
    buf[3] = (kal_uint8)pArgs->frameMessageType;
    OS_MemCopy(buf + 4, pArgs->pTraceHeader, pArgs->lenTraceHeader);
    OS_MemCopy(buf + 4 + pArgs->lenTraceHeader, pArgs->pData, pArgs->lenData);
    OS_MemSet(buf + buf_size + 3, 0x00, 6);  /* lost trace number */
    OS_MemCopy(buf + buf_size + 9, (U8 *)&back_len, 2);
    buf[0] = STX_OCTET;
    btmtk_log_frame_post_process(buf2, buf, &buf_size);
    //#ifdef __WIN_QUEUE__
    BTMTK_send_frame_ext(buf2, buf_size, bMsg);
    //#endif
    #endif
}

/*************************************************************************
* FUNCTION
*	convert_int_to_ascii
*
* DESCRIPTION
*		This routine converts a signed integer value to a ascii string.
*
* PARAMETERS
*		buf		buffer to save the converted ascii string
*		bindex	the index of the buffer
*		value    integer value that want to be converted
*     type     TRUE: signed type value. FALSE: unsigned type value.
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void convert_int_to_ascii(kal_uint8	*buf,
									kal_uint32	*bindex,
									kal_int32	value,
									kal_bool		type)
{
	kal_uint32	tmp_val, i;
	kal_uint8   tmp_buf[MAX_INT_BUF_LENGTH];

	if(type==KAL_TRUE && value<0)
	{
		tmp_val = (kal_uint32) (-value);
		buf[*bindex] = '-';
		*bindex = *bindex + 1;
	}
	else
	{
		tmp_val = (kal_uint32) value;
	}

	/* convert value to ascii */
	i = 0;
	do{
		tmp_buf[i] = (kal_uint8) (tmp_val %10) + 0x30 ;
		i++;
		tmp_val/=10;
	}while ( tmp_val != 0);

	if (*bindex + i > MAX_ARGUMENT_SIZE)
	{
		*bindex = MAX_ARGUMENT_SIZE;
		return;
	}

	/* get the result to buf */
	while( i!=0 )
	{
		i--;
		buf[*bindex] = tmp_buf[i];
		*bindex = *bindex +1;
	}
}

/*************************************************************************
* FUNCTION
*	convert_hex_to_ascii
*
* DESCRIPTION
*		This routine converts a hex integer value to a ascii string.
*
* PARAMETERS
*		buf		buffer to save the converted ascii string
*		bindex	the index of the buffer
*		value    integer value that want to be converted
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void convert_hex_to_ascii(kal_uint8 *buf, kal_uint32 *bindex, kal_int32 value)
{
	kal_uint32	tmp_val = value;
	kal_uint8	i, j=0;
	kal_uint8   tmp_buf[MAX_INT_BUF_LENGTH];


	/* convert hex to ascii string */
	do
	{
		if( (i =(kal_uint8) (tmp_val%16) ) < 10 )
		{
			tmp_buf[j++] = i + 0x30;		/* i + 0x30 would be 0~9 */
		}
		else
		{
			tmp_buf[j++] =	i + 0x37;	   /* i + 0x37 would be A~F */
		}

		tmp_val = tmp_val/16;
	}
	while( tmp_val!=0 );

	if (*bindex + 2 + j > MAX_ARGUMENT_SIZE)
	{
		*bindex = MAX_ARGUMENT_SIZE;
		return;
	}

	/* append '0x' characters to buf */
	buf[*bindex] = '0';
	*bindex+=1;
	buf[*bindex] = 'x';
	*bindex+=1;

	while( j!=0 )
	{
		j--;
		buf[*bindex] = tmp_buf[j];
		*bindex = *bindex +1;
	}
}


/*****************************************************************************
 * FUNCTION
 *  bt_trace  (Ref. kal_trace)
 * DESCRIPTION
 *  To print trace message to catcher using pre-defined trace message enum
 * PARAMETERS
 *  trc_class      [IN]        Trace class
 *  msg_index      [IN]        Trace message enum
 *  arg_type,...   [IN]        Provided parameter value
 * RETURNS
 *  void
 *****************************************************************************/
void bt_trace( trace_class_enum trc_class,
          kal_uint32 msg_index,
          const char *arg_type,...)
{
#if !defined(__PLATFORM_SUPPORTED_CATCHER__)

    if( tst_full != 0 || (g_tracemask & (1<<trc_class)) > 0)
    {
        /* this group is masked */
        return;
    }
#ifndef __USE_CATCHER_LOG__
    extern const char* MOD_BT_MSG_FMT[];

    if(!(msg_index > MOD_BT_empty_string && msg_index < MOD_BT__end))
        bt_prompt_trace(MOD_BT, "[BT] msg_index=%d out of range", msg_index);

    //char msg_buffer[300] = "[MTKBT]";   /* Output buffer */
    va_list args;
    msg_buffer[7] = 0;
    va_start(args, arg_type);
    vsnprintf(msg_buffer+7, 300, MOD_BT_MSG_FMT[msg_index-1], args);
    va_end(args);
    if(trc_class == TRACE_ERROR)
        ALOGE(msg_buffer);
    else if(trc_class == TRACE_WARNING)
        ALOGW(msg_buffer);
    else
        ALOGI(msg_buffer);
#else
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    va_list     argument_info;
    kal_uint8   arg_buf[MAX_ARGUMENT_SIZE];
    kal_uint32  i;
    kal_uint32  temp_int;
    kal_uint16  arg_write_index;
    kal_uint32  arg_type_length;
    tst_index_trace_header_struct   trace_header;
    tst_trace_frame_arg argFrameToSend;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    arg_write_index=0;
    if (arg_type != NULL)
    {
        arg_type_length = OS_StrLen(arg_type);
        /* Prepare argument buffer */
        va_start(argument_info, arg_type);
        for(i=0; i< arg_type_length && arg_write_index < MAX_ARGUMENT_SIZE-3; i++)
        {
            kal_uint32 j, count;

            if ((unsigned char)arg_type[i] > 127)
            {
                count = (unsigned char) arg_type[i] - 128;
                i++;
                Assert((count >= 2)&&(i< arg_type_length));
            }
            else
                count=1;

            for (j=0;j<count;j++)
            {
                switch(arg_type[i])
                {
                	case 'c':
                	  arg_buf[arg_write_index++]= (kal_uint8) va_arg(argument_info, kal_int32);
                	break;

                	case 'd':
                	  temp_int= va_arg(argument_info, kal_uint32);
                	  OS_MemCopy(&arg_buf[arg_write_index], (kal_uint8 *) &temp_int, sizeof(kal_uint32));
                	  arg_write_index+=4;
                	break;

                	case 'x':
                	  /* error: this indicates the PC side won't
                	  */
                	break;

                	default:
                	break;
                }

                if (arg_write_index >= MAX_ARGUMENT_SIZE-3)
                {  //end the loop
                  i = 0xFFFE;  //0xFFFF-1 to prevent overflow when incremented by 1
                  break;
                }
            }//for (j=0;j<count;j++)
        }
        va_end(argument_info);
    }

    /* fill trace header */
    trace_header.frame_number = btmtk_log_get_frame_number();
    trace_header.time_stamp = btmtk_log_get_timestamp();
    trace_header.msg_index = msg_index;
    trace_header.trace_class = trc_class;
    trace_header.buf_length = arg_write_index;

    OS_MemSet((U8 *)&argFrameToSend, 0x00, sizeof(argFrameToSend));

    argFrameToSend.pTraceHeader = (kal_uint8*) &trace_header;
    argFrameToSend.lenTraceHeader = sizeof(trace_header);
    argFrameToSend.pData = arg_buf; //frame content
    argFrameToSend.lenData = arg_write_index; // frame content length
    argFrameToSend.frameMessageType = PS_TRACE_INDEX_TRACE_TYPE;

    btmtk_log_fill_and_send_frame(&argFrameToSend, KAL_FALSE);
#endif
#endif /* !defined(__PLATFORM_SUPPORTED_CATCHER__) */
}

/*************************************************************************
* FUNCTION
*   bt_prompt_trace  (Ref. kal_prompt_trace)
* DESCRIPTION
*  To print trace message to catcher using printf-like function (not using pre-defined enum)
* PARAMETERS
*   mod_id   module identity
*   fmt      format control string
* RETURNS
*   void
*************************************************************************/
#if !defined(__FOOTPRINT_MEASUREMENT__)
void bt_prompt_trace(module_type mod_id, const char *fmt,...)
{
#if !defined(__PLATFORM_SUPPORTED_CATCHER__)
    if(tst_full != 0 || g_promptmask != 0)
    {
        /* this group is masked */
        return;
    }

#ifndef __USE_CATCHER_LOG__
    extern const char* MOD_BT_MSG_FMT[];
    //char buffer[300] = "[MTKBT]";   /* Output buffer */
    va_list args;
    msg_buffer[7] = 0;
    va_start(args, fmt);
    vsnprintf(msg_buffer+7, 300, fmt, args);
    va_end(args);

    ALOGI(msg_buffer);
#else
    kal_uint8	arg_buf[MAX_ARGUMENT_SIZE];
    kal_uint32  arg_index;
    va_list     argument_info;
    tst_prompt_trace_header_struct   trace_header;
    kal_uint8   *string_arg_ptr;
    kal_uint32  string_uint_val;
    kal_int32   string_int_val;
    const char        *src;
    tst_trace_frame_arg argFrameToSend;

    arg_index=0;

	/* Prepare argument buffer */
	va_start(argument_info, fmt);

	for (src=fmt; *src && arg_index < MAX_ARGUMENT_SIZE-1; src++)
    {
		if (*src != '%')
		{
			arg_buf[arg_index] = *src;
			arg_index++;
			continue;
		}

    src++;

    //escape +, -, 0-9
    if (*src >= '0' && *src <= '9')
    {
    	while ( (*src) && (arg_index < MAX_ARGUMENT_SIZE-1) )
    	{
    		src++;
    		if (*src >= '0' && *src <= '9')
    			continue;
    		else
    			break;
    	}
    }

		switch (*src)
		{
			case 'd':
			case 'i':
				string_int_val = va_arg(argument_info, kal_int32);
				convert_int_to_ascii(arg_buf, &arg_index, string_int_val, KAL_TRUE);
			break;

			case 'u':
			   string_uint_val = va_arg(argument_info, kal_uint32);
				convert_int_to_ascii(arg_buf, &arg_index, string_uint_val, KAL_FALSE);
			break;

			case 'x':
			   string_int_val = va_arg(argument_info, kal_int32);
				convert_hex_to_ascii(arg_buf, &arg_index, string_int_val);
			break;

			case 's':
				string_arg_ptr = va_arg(argument_info, kal_uint8 *);
                              if(string_arg_ptr != NULL)
                              {
				while (*string_arg_ptr && arg_index < MAX_ARGUMENT_SIZE)
				{
					arg_buf[arg_index] = *string_arg_ptr;
					arg_index++;
					string_arg_ptr++;
				}
                              }
			break;

			default :
			break;
		}
   }

	va_end(argument_info);			/* Reset argument_info */

	/* fill NULL character*/
	//arg_buf[arg_index] = '\0';
	//arg_index++;

	/* fill trace header */
	trace_header.frame_number = btmtk_log_get_frame_number();
	trace_header.time_stamp = btmtk_log_get_timestamp();
	trace_header.module_id = mod_id;
	trace_header.buf_length = (kal_uint16) arg_index;

	OS_MemSet((U8 *)&argFrameToSend, 0x00, sizeof(argFrameToSend));

	argFrameToSend.pTraceHeader = (kal_uint8*) &trace_header;
	argFrameToSend.lenTraceHeader = sizeof(trace_header);
	argFrameToSend.pData = arg_buf; //frame content
	argFrameToSend.lenData = arg_index; // frame content length
	argFrameToSend.frameMessageType = PS_PROMPT_TRACE_TYPE;

    btmtk_log_fill_and_send_frame(&argFrameToSend, KAL_FALSE);
#endif
#endif /* !defined(__PLATFORM_SUPPORTED_CATCHER__) */
}
#endif /* __FOOTPRINT_MEASUREMENT__ */
/*************************************************************************
* FUNCTION
*	bt_log_primitive  (Ref. tst_log_primitive)
*
* DESCRIPTION
*		This routine log current primitive to TST module.
*
* PARAMETERS
*	ilm		The pointer of ilm strucutre
*
* RETURNS
*
* GLOBALS AFFECTED
*
*************************************************************************/
void bt_log_primitive(ilm_struct* ilm_ptr)
{
    tst_log_prim_header_struct log_header;
    tst_trace_frame_arg argFrameToSend;

#if !defined(__PLATFORM_SUPPORTED_CATCHER__)
    if(tst_full != 0 || g_primitivemask != 0)
    {
        return;
    }

    #if !defined(__USE_CATCHER_LOG__)
    return;
    #endif

    log_header.local_len = 0;
    log_header.peer_len = 0;

 	/* Log this primitive to TST task */
 	if(*(kal_uint16 *)(ilm_ptr->ilm_data + 2) != 0)
 	{
        log_header.local_len = *(kal_uint16 *)(ilm_ptr->ilm_data + 2);
 	}

    log_header.frame_number = btmtk_log_get_frame_number();
    log_header.time_stamp = btmtk_log_get_timestamp();
    log_header.src_mod_id = ilm_ptr->src_mod_id;
    log_header.dest_mod_id = ilm_ptr->dest_mod_id;
    log_header.sap_id = INVALID_SAP;//ilm_ptr->sap_id;
    log_header.msg_id = ilm_ptr->msg_id;

    if (log_header.local_len == 0)
    {
        log_header.local_addr = 0;
    }
    else
    {
        log_header.local_addr = (kal_uint32) ilm_ptr->ilm_data;
    }
    log_header.peer_addr = 0;
	OS_MemSet((U8 *)&argFrameToSend, 0x00, sizeof(argFrameToSend));
	argFrameToSend.pTraceHeader = (kal_uint8*) &log_header;

    argFrameToSend.frameMessageType = LOGGED_PRIMITIVE_TYPE;
    argFrameToSend.lenTraceHeader = sizeof(tst_log_prim_header_struct);
    if (log_header.local_len == 0)
    {
        argFrameToSend.pData = NULL; //frame content
    }
    else
    {
        argFrameToSend.pData = (kal_uint8*) ilm_ptr->ilm_data; //frame content
    }
    argFrameToSend.pData2 = NULL; //frame content
    argFrameToSend.lenData = log_header.local_len; // frame content length
    argFrameToSend.lenData2 = log_header.peer_len; // frame content length

    btmtk_log_fill_and_send_frame(&argFrameToSend, KAL_TRUE);
#endif /* !defined(__PLATFORM_SUPPORTED_CATCHER__) */
}

void bt_fw_assert(kal_uint32 panic){
    U32 fwdump = BTCoreGetFWDump();

    OS_Report("bt_fw_assert(panic=%u), fwdump=0x%x", panic, fwdump);
    if( (fwdump & (1<<panic)) != 0){
        if( BTCoreFWDump(panic) ){
            OS_Report("FW core dump success, abort mtkbt");
            sleep(3);
            abort();
        }
    }
}

void bt_assert(char* expr, char* file, kal_uint32 line)
{
   /* chop off dirname if present */
   char *f = strrchr(file, '\\');

   if (f == NULL)
      f = file;
   else
      ++f;

    //printf("Assert fail: %s %d\n", f, line);
    //printf("Expression: %s", expr);
    OS_Report("Assert fail: %s %d\n", f, line);
    OS_Report("Expression: %s", expr);
    //assert(0);
    /* Sleep 3 seconds for trace output */

    bt_fw_assert(BT_PANIC_HOST_ASSERT);
    if( BTCoreIsInternalLoad() || BTCoreIsEngLoad() ){
    sleep(3);
    abort();
    }else{
        HciTransportReset();
    }
}

void bt_assert_reset(char* expr, char* file, kal_uint32 line)
{
   /* chop off dirname if present */
   char *f = strrchr(file, '\\');

   if (f == NULL)
      f = file;
   else
      ++f;

    //printf("Assert fail: %s %d\n", f, line);
    //printf("Expression: %s", expr);
    OS_Report("Assert fail: %s %d\n", f, line);
    OS_Report("Expression: %s", expr);

    HciTransportReset();
}

#endif  /* #ifndef BTMTK_ON_WISE */
