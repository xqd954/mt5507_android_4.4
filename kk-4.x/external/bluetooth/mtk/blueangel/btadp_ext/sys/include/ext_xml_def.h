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
 * Filename:
 * ---------
 * xml_def.h
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   This file is intends for ¡K.
 *
 * Author:
 * -------
 * Amanda Gau
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime:   Jul 19 2005 10:32:58  $
 * $Log:   //mtkvs01/vmdata/Maui_sw/archives/mcu/xmlp/include/xml_def.h-arc  $
 *
 * Dec 16 2008 mbj08033
 * [MAUI_01352582] [UDX]Should not remind "Invalid format"
 * 
 *
 * Nov 28 2008 mbj08033
 * [MAUI_01428678] [UDX support]parase udx file faild
 * 
 *
 * Nov 24 2008 mbj08033
 * [MAUI_01427906] [Shenzhen FT] Fatal Error (804, 40) - RR
 * 
 *
 * Nov 5 2008 mbj08033
 * [MAUI_01332585] XML optimization
 * 
 *
 * Nov 29 2007 mbj06066
 * [MAUI_00494762] XML PARSER modify for cdata
 * add to parser struct: 
 *     kal_int32 cdata_start_pos;
 *     kal_int32 cdata_len;
 *
 * Apr 25 2007 MBJ06066
 * [MAUI_00425513] extern xml_configure_memory in xmlp
 * extern xml_configure_memory in xmlp
 *
 * Apr 23 2007 MBJ06066
 * [MAUI_00425257] xmlp enhancement
 * xmlp enhancement
 *
 * Oct 23 2006 mtk00418
 * [MAUI_00336114] [1] Fatal Error (305): msg_send_ext_queue() failed - SYSTEM H
 * 
 *
 * Jun 12 2006 mtk00418
 * [MAUI_00126877] [RemoveWarning] remove compile warning
 * 
 *
 * Mar 17 2006 mtk00418
 * [MAUI_00220064] Assert fail: pool_info[pool_indx].pool_id romsa_ctrl_buff_pool.c 98 - MMI
 * 
 *
 * Mar 6 2006 mtk00418
 * [MAUI_00177191] [DLT] PMT Patch 2006/02/27~
 * 
 *
 * Jan 23 2006 mtk00418
 * [MAUI_00169847] [NewFeature][XMLP] support new syntax
 * 
 *
 * Nov 29 2005 mtk00418
 * [MAUI_00158179] [AddAPI][XMLP] add named character mapping API
 * 
 *
 * Nov 28 2005 mtk00418
 * [MAUI_00158179] [AddAPI][XMLP] add named character mapping API
 * 
 *
 * Nov 26 2005 mtk00418
 * [MAUI_00158179] [AddAPI][XMLP] add named character mapping API
 * 
 * 
 *    Rev 1.5   Jul 19 2005 10:37:40   mtk00563
 * Add for DLT (DownLoadable Theme)
 * Resolution for 11995: [MMI][GUI] Add for DLT
 * 
 *    Rev 1.4   May 17 2005 03:08:14   BM_Trunk
 * Karlos:
 * add copyright and disclaimer statement
 * 
 *    Rev 1.3   Mar 27 2005 11:59:38   mtk00418
 * support special characters
 * Resolution for 10406: [NewFeature][XMLP] support special character pattern
 * 
 *    Rev 1.2   Jan 24 2005 10:30:40   mtk00418
 * add CDATA parsing rule
 * Resolution for 9589: [BugFix][XMLP] add CDATA parsing rule
 * 
 *    Rev 1.1   Jan 18 2005 02:21:26   BM
 * append new line in W05.04
 * 
 *    Rev 1.0   Jan 16 2005 23:31:20   BM
 * Initial revision.
 * 
 * Initial revision.
 *******************************************************************************/

#ifndef __XML_DEF_H__
#define __XML_DEF_H__

/********************************************************************/
/* declaration */
/********************************************************************/

#define XML_TAG_PROLOG_START        "<?xml"
#define XML_TAG_PROLOG_END          "?>"
#define XML_TAG_DOCTYPE_START       "<!DOCTYPE"
#define XML_TAG_ELEMENTTYPE_START   "<!ELEMENT"
#define XML_TAG_ATTLISTTYPE_START   "<!ATTLIST"
#define XML_TAG_ENTITYTYPE_START    "<!ENTITY"
#define XML_TAG_NOTATIONTYPE_START  "<!NOTATION"
#define XML_TAG_COMMENT_START       "<!--"
#define XML_TAG_COMMENT_END         "-->"
#define XML_TAG_CDATA_START         "<![CDATA["
#define XML_TAG_CDATA_END           "]]>"
#define XML_TAG_PI_START            "<?"
#define XML_TAG_ENCODING            "encoding"
#define XML_TAG_ENCODING_UTF8       "utf-8"
#define XML_TAG_ENCODING_UTF16      "utf-16"
#define XML_TAG_ENCODING_UCS2       "iso-10646-ucs-2"
#define XML_TAG_ENCODING_UCS4       "iso-10646-ucs-4"
#define XML_TAG_ENCODING_ASCII      "iso-8859-1"

#define XML_TAG_ELEMENT_START       "<"
#define XML_TAG_ELEMENT_END         "</"
#define XML_TAG_SINGLE_END          "/>"
#define XML_TAG_GENERAL_END         ">"
#define XML_SPACE                   " "
#define XML_QUOTE                   "'"
#define XML_DOUBLE_QUOTE            "\""
#define XML_SQUARE_LEFT_BRAKET      "["
#define XML_SQUARE_RIGHT_BRAKET     "]"

#define CONST_XML_FOLDER L"d:\\temp"

/* Leo add for DLT */
/*****************************************
Note: DLT needs more buffer length.
If anyone needs to use these two macro,
   please include "MMI_features.h" before
   xml_def.h.
*****************************************/
#if defined (__MMI_DOWNLOADABLE_THEMES_SUPPORT__) || defined (SVG_SUPPORT)
#define XML_BUFFER_LENGTH        512
#define XML_MAX_ATTR_COUNT        100
#else /* defined (__MMI_DOWNLOADABLE_THEMES_SUPPORT__) || defined (__SVG_SUPPORT__) */ 
#define XML_BUFFER_LENGTH        128
#define XML_MAX_ATTR_COUNT     30
#endif /* defined (__MMI_DOWNLOADABLE_THEMES_SUPPORT__) || defined (__SVG_SUPPORT__) */ 

#define MAX_CTRL_BUFFER_SIZE  2048
#define XML_MAX_ENTITY_COUNT  10
#define XML_MAX_ELEM_DEPTH  20
#define XML_CHAR_ENTITY_LENGTH  15


/* Leo end */
typedef void (*XML_start_elem_hdlr) (void *data, const kal_char *el, const kal_char **attr, kal_int32 error);
typedef void (*XML_end_elem_hdlr) (void *data, const kal_char *el, kal_int32 error);
typedef void (*XML_start_doctype_hdlr) (void *data,
                                        const kal_char *doctypeName,
                                        const kal_char **sysid, 
                                        const kal_char **pubid, kal_int32 internal_subset, kal_int32 error);
typedef void (*XML_end_doctype_hdlr) (void *data, kal_int32 error);
typedef void (*XML_data_hdlr) (void *resv, const kal_char *el, const kal_char *data, kal_int32 len, kal_int32 error);

typedef void *(*XML_malloc_buffer_ptr) (kal_int32);
typedef void (*XML_free_buffer_ptr) (void *);

#define MAX_FILEBUF_NUM  2
#define FILEBUF_SIZE   5*1024
#define FILEBUF_BACKSIZE 2*1024
typedef struct 
{
	kal_char  valid;
	kal_char  *bufptr;
	kal_char  isfull;
	kal_int32 virfilelen;
	kal_int32 virfileseek;
    kal_int32 bufsize;
	FHANDLE fp;
	kal_int32 realfilebeginpos;
	kal_int32 realfileseekpos;
   
} virtual_file_buf_struct;


#define XML_VIRFILE_MIN   4*1024
typedef enum
{
    XML_RESULT_FILE_NOT_FOUND = -4,
    XML_RESULT_OUT_OF_MEMORY = -3,
    XML_RESULT_ENCODING_ERROR = -2,
    XML_RESULT_FAIL = -1,
    XML_RESULT_OK = 0
} XML_RESULT_ENUM;


typedef enum
{
    XML_NO_ERROR,
    XML_ERROR_MISMATCH_TAG,                     /* error string: end tag name */
    XML_ERROR_MISMATCH_DOCTYPENAME_ROOTNAME,    /* error string : root element name */
    XML_ERROR_INVALID_ATTRIBUTE,                /* error string : attribute name */
    XML_ERROR_DUPLICATE_ATTRIBUTE,              /* error string : attribute name */
    XML_ERROR_ENTITY_DEFINITION,                /* error string: entity name */
    XML_ERROR_UNDEFINED_ENTITY                  /* error string : entity name */
}xml_error_enum;

typedef enum
{
    XML_PENDING_NONE,
    XML_PENDING_PROLOG,
    XML_PENDING_DOCTYPE,
    XML_PENDING_DOCTYPE_MARKUP,
    XML_PENDING_COMMENT,
    XML_PENDING_PI,
    XML_PENDING_CDATA,
    XML_PENDING_START_ELEMENT,
    XML_PENDING_END_ELEMENT
} XML_PENDING_ENTITY_TYPE_ENUM;

typedef enum
{
    XML_ENCODING_UTF8,
    XML_ENCODING_UTF16,
    XML_ENCODING_UCS2,
    XML_ENCODING_UCS4,
    XML_ENCODING_UNKNOWN
} XML_ENCODING_TYPE_ENUM;


typedef struct
{
    const kal_char named_ch;
    const kal_char *named_string;
} named_char_entry_struct;
typedef struct
{
    XML_malloc_buffer_ptr get_buffer_ptr;
    XML_free_buffer_ptr free_buffer_ptr;
    XML_start_doctype_hdlr start_doctype_hdlr;
    XML_end_doctype_hdlr end_doctype_hdlr;
    XML_start_elem_hdlr start_elem_hdlr;
    XML_end_elem_hdlr end_elem_hdlr;
    XML_data_hdlr data_hdlr;
    FHANDLE source;
    kal_uint8 isext;
    virtual_file_buf_struct virfileptr;
    const kal_char *xml_text;
    kal_char *elem[XML_MAX_ELEM_DEPTH];
    kal_char *ge_data[XML_MAX_ENTITY_COUNT << 1];
    kal_char *pe_data[XML_MAX_ENTITY_COUNT << 1]; /* parameter entity data, added by liza */
    kal_char *doct_name; /* this name is saved to match with root element name */
    kal_char *token_ptr;
    kal_char *ap_temp;
    kal_char *err_str;
    void *root;
    kal_int32 xml_len; /* the length of xml_text */
    kal_int32 curr_depth;
    kal_int32 error;
    kal_int32 prolog_depth;
    kal_int32 encoding;
    kal_int32 prolog_done;
    kal_int32 pos;
    kal_int32 pending_start_pos;
    kal_int32 cdata_start_pos;
    kal_int32 cdata_len;
    kal_int32 data_start_pos;
    kal_int32 need_end_tag;
    kal_int32 pending_type;
    kal_int32 stop;
    kal_int32 pause;
    kal_uint8 pe_count;
    kal_uint8 ge_count;
} XML_PARSER_STRUCT;


extern void xml_elem_start_hdlr(void *data, const kal_char *el, const kal_char **attr, kal_int32 error);
extern void xml_elem_end_hdlr(void *data, const kal_char *el, kal_int32 error);
extern void xml_doctype_start_hdlr(
                void *data,
                const kal_char *doctypeName,
                const kal_char **sysid,
                const kal_char **pubid,
                kal_int32 internal_subset,
                kal_int32 error);
extern void xml_doctype_end_hdlr(void *data, kal_int32 error);
extern void xml_general_data_hdlr(void *resv, const kal_char *el, const kal_char *data, kal_int32 len, kal_int32 error);

/* external API */
extern kal_int32 xml_new_parser(XML_PARSER_STRUCT *parser);
extern void xml_close_parser(XML_PARSER_STRUCT *parser);

/* Leo add for DLT */
extern void xml_close_parser_offset(XML_PARSER_STRUCT *parser);
extern kal_int32 xml_parse_file_from_offset(XML_PARSER_STRUCT *parser, kal_int32 file_handle);

/* Leo end */

extern void xml_register_doctype_handler(
                XML_PARSER_STRUCT *parser,
                XML_start_doctype_hdlr start_hdlr,
                XML_end_doctype_hdlr end_hdlr);
extern void xml_register_element_handler(
                XML_PARSER_STRUCT *parser,
                XML_start_elem_hdlr start_hdlr,
                XML_end_elem_hdlr end_hdlr);
extern void xml_register_data_handler(XML_PARSER_STRUCT *parser, XML_data_hdlr data_hdlr);

extern kal_int32 xml_resume_parse(XML_PARSER_STRUCT *parser);
extern void xml_pause_parse(XML_PARSER_STRUCT *parser);
extern void xml_stop_parse(XML_PARSER_STRUCT *parser);
extern void xml_free_parse(XML_PARSER_STRUCT *parser);
extern kal_char *xml_get_err_string(XML_PARSER_STRUCT *parser);

extern kal_int32 xml_parse(XML_PARSER_STRUCT *parser, U8 *file_name);
extern kal_int32 xml_parse_buffer(XML_PARSER_STRUCT *parser, const kal_char *xmltext, kal_int32 length);


extern void xml_configure_memory(
                XML_PARSER_STRUCT *parser,
                XML_malloc_buffer_ptr get_ptr,
                XML_free_buffer_ptr free_ptr);
extern kal_int32 xml_new_parser_ext(XML_PARSER_STRUCT *parser, kal_char* buffer, kal_int32 size);

#endif /* __XML_DEF_H__ */


