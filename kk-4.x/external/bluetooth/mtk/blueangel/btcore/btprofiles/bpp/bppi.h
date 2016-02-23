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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *  bppi.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for BPP profile internal header file.
 *
 * Author:
 * -------
 *  Dexiong Xu, revised by Yufeng Chu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/* for debug */
#define __BPP_DEBUG__

#define BPPSVC_DPS              SC_DIRECT_PRINTING
#define BPPSVC_STS              SC_PRINTING_STATUS

/* Target UUID length */
#define TARGET_UUID_LEN              16

/* BPP mru */
#define BPP_MRU  2048

#define  BPP_MAX_PRINTER_NUM  0x02

#define BPP_FLAGS_AUTHCHAL_RCVD 0x01

/*---------------------------------------------------------------------------
 * BppAppParmTag type
 */
#define BPPAPTAG_OFFSET     0x01
#define BPPAPTAG_COUNT      0x02
#define BPPAPTAG_JOBID      0x03
#define BPPAPTAG_FILESIZE   0x04
/* End of BppAppParmTag */

/* buffer size */
#define BPP_MAX_PULL_BUFF_LEN    1024
#define BPP_MAX_PUSH_BUFF_LEN    (BPP_MAX_PULL_BUFF_LEN)

/*  */
#define BPP_XHTML_TAG_PDU_FREE_HEADER 32
#define BPP_XHTML_TAG_MAX_ATTR  12
#define BPP_MAX_XHTML_TAG_SIZE  (1024-BPP_XHTML_TAG_PDU_FREE_HEADER)

#define BPP_TOKEN_BUF           480

#define BPP_FIRST_PKT_BIT       0x01
#define BPP_FINAL_PKT_BIT       0x02

#define BPP_PDU_FREE_HEADER     256
#define BPP_PDU_FREE_TAIL       32


#define BPP_MAX_MIME_TYPE_LEN  64
#define BPP_CONTENT_HEADER_LEN (BPP_MAX_MIME_TYPE_LEN+16)


#define BPP_USER_CANCEL_FLAG      0x01
#define BPP_USER_DISCONNECT_FLAG  0x02


typedef enum
{
    BPP_JOB_CHANNEL = 1,
    BPP_STATUS_CHANNEL
    
} bpp_channel_num;


typedef enum
{
    BPP_STATE_IDLE = 0,
    BPP_STATE_TP_CONNECTING,
    BPP_STATE_OBEX_CONNECTING,
    BPP_STATE_CONNECTED,
    BPP_STATE_GETING_ATTRIBUTE,
    BPP_STATE_SENDING_DOC,   /* pushing the doc: SimplePush or Job-based */
    BPP_STATE_CREATEING_JOB,
    BPP_STATE_PRINTING_JOB,  /* the doc has been sent out, waiting the job status */
    BPP_STATE_CANCEL_JOB,
    BPP_STATE_USER_CANCELLING,
    BPP_STATE_DISCONNECTING,
    BPP_STATE_MAX
} bt_bpp_print_state;


typedef enum
{
    BPP_STS_STATE_INIT,
    BPP_STS_STATE_TP_CONNECTING,
    BPP_STS_STATE_OBEX_CONNECTING,
    BPP_STS_STATE_CONNECTED,
    BPP_STS_STATE_GETING_EVENT,
    BPP_STS_STATE_ABORT,
    BPP_STS_STATE_DISCONNECTING,
    BPP_STS_STATE_MAX
} bpp_status_channel_state;


typedef enum
{
    BPP_XHTML_NONE,
    BPP_XHTML_STAG,
    BPP_XHTML_ETAG,
    BPP_XHTML_SETAG
} bpp_xhtml_tag_enum;

typedef enum
{
    BPP_XHTML_START,
    BPP_XHTML_TAG_START,
    BPP_XHTML_STAG_START,
    BPP_XHTML_HUNT_ETAG,
    BPP_XHTML_BODY_START,
    BPP_XHTML_ETAG_START,
    BPP_XHTML_TAG_MATCH
} bpp_get_tag_state_enum;

typedef enum
{
    BPP_TOKEN_UNKNOW,
    BPP_TOKEN_STAG_START,
    BPP_TOKEN_ETAG_START,
    BPP_TOKEN_TAG_END,
    BPP_TOKEN_ESTAG_END,
    BPP_TOKEN_EQUAL,
    BPP_TOKEN_VAL,
    BPP_TOKEN_ID
} bpp_token_enum;

typedef enum
{
    BPP_XHTML_INIT,
    BPP_XHTML_BEGIN_VAL,
    BPP_XHTML_BEGIN_ID
} bpp_get_token_state_enum;

typedef enum
{
    SOAP_INIT,
    SOAP_HEADER_START,
    SOAP_HUNT_BODY_START,
    SOAP_BODY_START
}bpp_soap_parse_state;



typedef enum
{
    BPP_DISC_OBEX,
    BPP_DISC_TP
} bpp_disc_type_enum;

typedef enum
{
	BPP_GETATTR_INVOKER_APP = 1,
	BPP_GETATTR_INVOKER_OWN
		
}bpp_get_printer_attr_invoker;

typedef enum
{
    BPP_ATTR_PRINTERNAME,
    BPP_ATTR_PRINTERLOCATION,
    BPP_ATTR_PRINTERSTATE,
    BPP_ATTR_PRINTERSTATEREASONS,
    BPP_ATTR_DOCUMENTFORMATSSUPPORTED,
    BPP_ATTR_COLORSUPPORTED,
    BPP_ATTR_MAXCOPIESSUPPORTED,
    BPP_ATTR_SIDESSUPPORTED,
    BPP_ATTR_NUMBERUPSUPPORTED,
    BPP_ATTR_ORIENTATIONSSUPPORTED,
    BPP_ATTR_MEDIASIZESSUPPORTED,
    BPP_ATTR_MEDIATYPESSUPPORTED,
    BPP_ATTR_MEDIALOADED,
    BPP_ATTR_PRINTQUALITYSUPPORTED,
    BPP_ATTR_QUEUEDJOBCOUNT,
    BPP_ATTR_IMAGEFORMATSSUPPORTED,
	BPP_ATTR_BASICTEXTPAGEWIDTH,
	BPP_ATTR_BASICTEXTPAGEHEIGHT,
    BPP_ATTR_PRINTERGENERALCURRENTOPERATOR,

    BPP_ATTR_OPERATIONSTATUS,

    BPP_ATTR_MAX_ATTRIBUTE
} bpp_printer_attribute_enum;


/* data structure */
typedef struct
{
   kal_uint16	pdu_len;
   kal_uint8	ref_count;
   kal_uint16	free_header_space;
   kal_uint16	free_tail_space;
}bpp_head_buff_struct;

typedef struct
{
    kal_int32 job_id;
    kal_int32   status_idx;
} bt_bpp_create_job_rst;

/* ------------------------ */
struct _bpp_send_obj;

typedef bpp_head_buff_struct *(*pfnread) (struct _bpp_send_obj *psend_obj, void *param);


typedef struct
{
    kal_bool btemp_file;
    WCHAR file_name[BPP_MAX_FILE_NAME_LEN];
    BTMTK_FS_HANDLE fh;
    kal_uint32 file_rsize;
	
} bpp_file_obj_struct;

typedef struct
{
    const kal_char *pbuf;
    kal_uint32      buf_size;
    kal_uint32      offset;
}bpp_buf_obj_struct;

typedef struct _bpp_simple_data
{
	U8 * buf;
	U16  buf_size;
	U16  data_len;
	
}bpp_simple_data_struct;

typedef enum
{
	BPP_SIMPLE_DATA = 1,
	BPP_XHTML_DATA	
}bpp_print_data_type_enum;

typedef enum
{
	BPP_FILE_FLAG_ORIG = 0x00, /* original file */
	BPP_FILE_FLAG_TEMP = 0x01  /* temp file */
}bpp_print_file_flag;

typedef struct _bpp_send_obj
{
    bpp_file_obj_struct *pfile_obj;
	
	bt_bpp_mime_type mime_id;
    kal_char *mime_str;

	bpp_print_data_type_enum  data_type; /* Simple or xHtml */
	
	bpp_simple_data_struct simple_data;
					
    kal_uint8 pkt_type;

    kal_int32 total_size_sent;

	bpp_buf_obj_struct	*pbuf_obj;
	struct _bpp_send_obj	*pinline_obj;

	bpp_head_buff_struct *pdu_in;

	kal_uint16 pdu_len; /* data len in pdu_in */

	kal_uint16 chk_id;

	kal_uint16 total_chk_id;

	void *p_pending_tag;

} bpp_send_obj_struct;


typedef struct
{
    XML_PARSER_STRUCT *xml_parser_p;
    void*              parser_output;
	
}bt_bpp_xml_parser_parm_struct;

typedef struct
{
    bpp_file_obj_struct *pfile_obj;
    bpp_head_buff_struct *pdu_in;
    kal_char mime_type[BPP_MAX_MIME_TYPE_LEN];
    bt_bpp_xml_parser_parm_struct *parser_param;
	
} bpp_get_obj_struct;


typedef struct
{
    bpp_oper_enum current_req;
    bpp_head_buff_struct *pdu_out;
    kal_uint32 data_len;    /* just use for obex length header */
    union
    {
        bpp_send_obj_struct send_obj;
        bpp_get_obj_struct get_obj;
    } object;
	
    struct bpp_printer *printer;

} bpp_oper_context_struct;

typedef struct
{
    GoepClientApp channel;

	bpp_status_channel_state state;

	struct bpp_printer *printer;

	bpp_oper_context_struct *get_event_oper;
	
} bpp_status_channel_struct;

typedef struct bpp_printer
{
    struct bpp_printer *pnext;

    PRINTER_HANDLE handle;
    BD_ADDR   bt_addr;
	
    bt_bpp_print_state state;
	
    GoepClientApp job_channel;	
    kal_uint32 printer_mru;

	U8   flags; /* if authentication challenge is received */ 
	bt_bpp_obex_auth_chal_info chal_info; /* challenge info */

	/* DocumentFormatsSupported */
	U8 			docfmt_num;
    bt_bpp_mime_type	doc_format_supported[BPP_MAX_NESTED_ATTR_NUM];

	bpp_get_printer_attr_invoker getattr_invoker;

	/* print object */
    bt_bpp_object  print_object;
	bpp_print_file_flag  printfile_flag; /* in print object, the file is origin or temp */
	
	/* operation context */
    bpp_oper_context_struct *current_oper;
	
	/* next oper: used when create job to record the send_doc context */
    bpp_oper_context_struct *next_oper;

	/* for job-based print */
    kal_int32 job_id;
	
	/* status channel for job-base print model */
    bpp_status_channel_struct *status_channel;
	
	/* if can continue to send progress ind to app */
    kal_bool cont_prog_ind; 

	/* if user abort the current operation */
	U8 user_abort_flag;

} bpp_printer_struct;


typedef struct
{
    kal_char *pbuf;
    kal_uint16 size;
    kal_uint16 look_forward;
    kal_uint16 pos;
    bpp_token_enum look_head;
    kal_char symbol[BPP_TOKEN_BUF];
    kal_char *psymbol;
} bpp_token_parser_struct;

typedef struct _bpp_xhtml_attr
{
    kal_char *pid;
    kal_char *pval;
} bpp_xhtml_attr_struct;

typedef struct
{
    bpp_xhtml_tag_enum tag_type;
    kal_char *item_name;
    bpp_xhtml_attr_struct attr[BPP_XHTML_TAG_MAX_ATTR];
    kal_uint16 attr_num;
} bpp_xhtml_tag_struct;


typedef struct
{
    bpp_printer_struct *printer_list;

	BppCallback app_callback;
	
}bpp_session_context;

typedef struct
{
	bt_bpp_printer_attributes *p_attr;
	U32 status_idx;
}bpp_get_printer_attr_resp;

typedef struct
{
	bt_bpp_job_status *p_jobstatus;
	U32 status_idx;
}bpp_get_job_status_resp;


/*-------------- bpp.c --------------*/
bpp_oper_context_struct *bpp_find_oper_context(XML_PARSER_STRUCT *xml_parser_p ,bpp_oper_enum current_req);

void BppAppCallback(BppCallbackParms *parms);

BT_BOOL bpp_printer_support_doctype(bpp_printer_struct *printer, bt_bpp_mime_type mime_type);


/*-------------- bpp_data.c --------------*/

kal_bool bpp_init_get_attr_context(
                    bpp_oper_context_struct *get_attr_context,
                    U32 attr_id_bitmask);

kal_bool bpp_init_send_doc_context(
                    bpp_oper_context_struct *pcontext,
                    bt_bpp_object *print_object,
                    bpp_print_file_flag file_flag);

kal_bool bpp_init_create_job_context(
                    bpp_oper_context_struct *create_job_oper_context,
                    bt_bpp_object *print_object);

kal_bool bpp_init_get_event_context(
					bpp_oper_context_struct *get_event_context, 
					kal_uint32 job_id);

kal_bool bpp_init_cancel_job_context(
					bpp_oper_context_struct *cancel_job_context, 
					kal_uint32 job_id);

kal_bool bpp_init_soap_req_with_only_jobid(
                    bpp_oper_context_struct *get_context,
                    const kal_char *pcmdstr,
                    kal_int32 job_id);

void bpp_destory_oper_context(bpp_oper_context_struct *pcontext);

/* continue to send data */
kal_bool bpp_continue_read_xhtml_data(bpp_printer_struct *printer);

/* obs func */
ObexRespCode bpp_obs_read_doc(void *obs, kal_uint8 *pbuf, kal_uint16 len);

ObexRespCode bpp_obs_write_soaprsp(void *obs, kal_uint8 *pbuf, kal_uint16 len);

kal_uint32 bpp_obs_get_objlen(void *obs);

/* SOAP parse function */
kal_bool bpp_parse_get_attr_rsp_soap(
                    bpp_get_obj_struct *pobj,
                    bpp_get_printer_attr_resp *get_attr_cnf);

kal_bool bpp_parse_create_job_rsp_soap(
                    bpp_get_obj_struct *pobj,
                    bt_bpp_create_job_rst *creat_job_rst);

kal_bool bpp_parse_get_event_rsp_soap(
					bpp_get_obj_struct *pobj, 
					bpp_get_job_status_resp *status_ind);


kal_bool bpp_is_soap_complete(bpp_oper_context_struct *oper_context);

void bpp_deinit_send_obj(bpp_send_obj_struct *psend_obj);

void bpp_destory_file_obj(bpp_file_obj_struct *pfile_obj);

void bpp_progress_ind(bpp_printer_struct *printer);

kal_char *bpp_get_doc_format(bt_bpp_mime_type type);
U16 bpp_get_doc_name(WCHAR *pathname, kal_char *name_buf, U16 buf_size);


/* xhtml composing functions */
S32 bpp_pre_process_object_data(bpp_printer_struct *printer, bt_bpp_object *print_req);

S32 bpp_embed_object_into_xhtml(BTMTK_FS_HANDLE fh, U16 *obj_name, bt_bpp_mime_type obj_type);

S32 bpp_compose_object_into_xhtml(U16 * dst_xhtml_file, U16 *obj_name, bt_bpp_mime_type obj_type);

U32 bpp_compose_vcard_to_xhtml(BTMTK_FS_HANDLE fh, U16* vcard_file);

U8* bpp_get_xhtml_prologue(void);

/* text encoding functions */
BT_BOOL bpp_is_file_utf8_encoding(U16* filename);
S32 bpp_convert_filecontent_to_utf8(U16* src_file, U16* dst_file);



/*-------------- goep.c --------------*/

kal_uint32  Goep_get_mru(GoepClientApp *Client);

ObStatus Goep_init_send_buf(GoepClientApp *Client,kal_char* pbuf, kal_uint32 size);


/*-------------- bpp_util.c --------------*/
kal_bool bpp_pdu_add_header(bpp_head_buff_struct *pdu, const kal_char *pdata, kal_uint32 data_len);
kal_bool bpp_pdu_append_tail(bpp_head_buff_struct *pdu, const kal_char *pdata, kal_uint16 data_len);

bpp_head_buff_struct *bpp_read_from_pdu(bpp_send_obj_struct *psend_obj, kal_uint16 size);
void bpp_update_pdu(bpp_send_obj_struct *psend_obj);

void* bpp_get_pdu_ptr(bpp_head_buff_struct *buff_ptr, kal_uint16 *length_ptr);
void bpp_clear_pdu_data(bpp_head_buff_struct *pdu);

void bpp_free_head_buff(bpp_head_buff_struct *pdu_ptr);

kal_int32 bpp_utf8_to_ucs2_string(kal_uint8 *dest, kal_int32 dest_size, kal_uint8 *src);

void bpp_get_private_folder(S8 *pathbuf);

void bpp_create_private_folder(void);



typedef int BPP_FS_HANDLE;

int bpp_fs_open(const WCHAR * FileName, unsigned int Flag);
int bpp_fs_close(BPP_FS_HANDLE FileHandle);
//int bpp_fs_read(BPP_FS_HANDLE FileHandle, void * DataPtr, unsigned int Length, unsigned int * Read);
//int bpp_fs_write(BPP_FS_HANDLE FileHandle, void * DataPtr, unsigned int Length, unsigned int * Written);
int bpp_fs_read(BPP_FS_HANDLE FileHandle, void * DataPtr, int Length, int * Read);
int bpp_fs_write(BPP_FS_HANDLE FileHandle, void * DataPtr,int Length, int * Written);
int bpp_fs_createdir(const WCHAR * DirName);
int bpp_fs_delete(const WCHAR * FileName);
int bpp_fs_getfilesize(BPP_FS_HANDLE FileHandle, unsigned int * Size);

int bpp_fs_seek(BPP_FS_HANDLE FileHandle, S32 Offset, S32 Whence);




#ifdef __BPP_DEBUG__

void bpp_debug_x_init(void);

void bpp_debug_x_output2(U16* file_name,kal_char *str,kal_uint32 len);
void *bpp_debug_x_mem_alloc(U32 size, S8 *pfile, U16 line);
void bpp_debug_x_mem_free(S8 *pbuf, S8 *pfile, U16 line);
void bpp_debug_x_hold_head_buf(bpp_head_buff_struct *pdu, S8 *pfile, U16 line);
void *bpp_debug_x_alloc_head_buf(
        U16 pdu_len,
        kal_int16 header_len,
        kal_int16 tail_len,
        kal_int16 direction,
        S8 *pfile,
        U16 line);
void bpp_debug_x_free_head_buf(bpp_head_buff_struct *pdu, S8 *pfile, U16 line);

BTMTK_FS_HANDLE bpp_debug_x_fs_open(const WCHAR *pfilename, U32 flags, S8 *pfile, U16 line);
void bpp_debug_x_fs_close(BTMTK_FS_HANDLE fh, S8 *pfile, U16 line);

void bpp_debug_x_mem_fh_summary(void);


#define bpp_mem_alloc(size)             bpp_debug_x_mem_alloc(size,__FILE__, __LINE__)
#define bpp_mem_free(pbuf)              do{if(pbuf){bpp_debug_x_mem_free((S8*)pbuf,__FILE__, __LINE__);pbuf=NULL;}}while(0)


#define BPP_Debug_x_FS_Open(pfilename,flags)    bpp_debug_x_fs_open((WCHAR*)pfilename, flags, __FILE__, __LINE__)
#define BPP_Debug_x_FS_Close(fh)                bpp_debug_x_fs_close(fh, __FILE__, __LINE__)

void bpp_debug_x_output(S8 *str, U32 len);

void bpp_debug_x_mem_fh_summary(void);

void *bpp_debug_x_mem_alloc(U32 size, S8 *pfile, U16 line);

void bpp_debug_x_mem_free(S8 *pbuf, S8 *pfile, U16 line);

BTMTK_FS_HANDLE bpp_debug_x_fs_open(const WCHAR *pfilename, U32 flags, S8 *pfile, U16 line);

void bpp_debug_x_fs_close(BTMTK_FS_HANDLE fh, S8 *pfile, U16 line);

void *bpp_debug_x_alloc_peer_buf(
        U16 pdu_len,
        kal_int16 header_len,
        kal_int16 tail_len,
        kal_int16 direction,
        S8 *pfile,
        U16 line);

void bpp_debug_x_free_peer_buf(bpp_head_buff_struct *pdu, S8 *pfile, U16 line);

void bpp_debug_x_hold_peer_buf(bpp_head_buff_struct *pdu, S8 *pfile, U16 line);

#define bpp_free_head_buf(pdu)          do{if(pdu){bpp_debug_x_free_head_buf(pdu,__FILE__, __LINE__);pdu=NULL;}}while(0)
#define bpp_hold_head_buf(pdu)          bpp_debug_x_hold_head_buf(pdu,__FILE__, __LINE__)

#define bpp_alloc_head_buf(pdu_len, header_len, tail_len, direction)\
bpp_debug_x_alloc_head_buf(pdu_len, header_len, tail_len, direction, __FILE__, __LINE__)

//mtk03036 20100506
#ifdef BTMTK_ON_LINUX
#define BPP_DB_SOAP_OUT_FILE  L"/BppSoapO.txt"
#define BPP_DB_SOAP_IN_FILE   L"/BppSoapI.txt"
#define BPP_IN_SOAP_FILE   L"/BppSoapRsp.xml"
#else
#define BPP_DB_SOAP_OUT_FILE  L"\\BppSoapO.txt"
#define BPP_DB_SOAP_IN_FILE   L"\\BppSoapI.txt"
#define BPP_IN_SOAP_FILE   L"\\BppSoapRsp.xml"
#endif

//move from bpp_util.c
#ifdef BTMTK_ON_WISE
  #define BPP_BTMTK_PARENT_PATH  L"c:"
  #define BPP_BTMTK_ROOT_PATH      L"\\@btmtk"
  #define BPP_BTMTK_PROFILE_PATH   L"\\profile"
  #define BPP_GET_FULL_PATH(fullpath,filename) \
do{                                              \
    bpp_get_private_folder((S8*)fullpath);        \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)L"\\");      \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)filename);   \
}while(0)

#elif defined(BTMTK_ON_LINUX)
  #define BPP_BTMTK_PARENT_PATH  L"/data"
  #define BPP_BTMTK_ROOT_PATH    L"/@btmtk"
  #define BPP_BTMTK_PROFILE_PATH L"/profile"
  #define BPP_GET_FULL_PATH(fullpath,filename) \
do{                                              \
    bpp_get_private_folder((S8*)fullpath);        \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)L"/");      \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)filename);   \
}while(0)

#else /* BTMTK_ON_WISE */
  #define BPP_BTMTK_ROOT_PATH      L"\\@btmtk"
  #define BPP_BTMTK_PROFILE_PATH   L"\\profile"
  #define BPP_GET_FULL_PATH(fullpath,filename) \
do{                                              \
    bpp_get_private_folder((S8*)fullpath);        \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)L"\\");      \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)filename);   \
}while(0)
#endif
//mtk03036 20100506 end

#define BPP_DB_OUT(str,len)         bpp_debug_x_output(str,len)
#define BPP_DB_SOAP_OUT(str,len)        bpp_debug_x_output2(BPP_DB_SOAP_OUT_FILE,str,len)
#define BPP_DB_SOAP_IN(str,len)         bpp_debug_x_output2(BPP_DB_SOAP_IN_FILE,str,len)

#ifdef __ON_MAUI__
#define BPP_STORE_DRV               FS_GetDrive(FS_DRIVE_V_NORMAL, 2, FS_DRIVE_V_NORMAL | FS_DRIVE_V_REMOVABLE)/*MMI_PUBLIC_DRV*/
#else
#define BPP_STORE_DRV  'C'
#endif

#else /* __BPP_DEBUG__ */ 

#define bpp_mem_alloc(size)             get_ctrl_buffer(size)
#define bpp_mem_free(pbuf)              do{if(pbuf){free_ctrl_buffer(pbuf);pbuf=NULL;}}while(0)

#ifdef __DRM_SUPPORT__   
#define BPP_Debug_x_FS_Open(pfilename,flags)    DRM_open_file(pfilename, flags,DRM_PERMISSION_PRINT)
#define BPP_Debug_x_FS_Close(fh)                DRM_close_file(fh)
#else
#define BPP_Debug_x_FS_Open(pfilename,flags)    bpp_fs_open(pfilename, flags)
#define BPP_Debug_x_FS_Close(fh)                bpp_fs_close(fh)
#endif

#define bpp_alloc_head_buf(pdu_len, header_len, tail_len, direction)\
bpp_construct_head_buff(pdu_len, header_len, tail_len)

#define BPP_DB_INIT()
#define BPP_DB_OUT(str,len)
#define BPP_DB_SOAP_OUT(str,len)
#define BPP_DB_SOAP_IN(str,len)
#define BPP_DEBUG_PHONY()

#ifdef __ON_MAUI__
#define BPP_STORE_DRV               FS_GetDrive(FS_DRIVE_I_SYSTEM, 2, FS_ONLY_ALT_SERIAL )/*MMI_PRIVATE_DRV*/
#else
#define BPP_STORE_DRV  'C'
#endif

#endif /* __BPP_DEBUG__ */

#define BPP_ASSERT(a)   ASSERT((a))


