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
 *    bpp_util.c
 *
 * Project:
 * --------
 *  
 *
 * Description:
 * ------------
 *   This file is for BPP utility.
 *
 * Author:
 * -------
 *   Dexiong Xu, revised by Yufeng Chu
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
#include "osapi.h"


#ifdef __ON_MAUI__
#include "fat_fs.h"     /* FS */
#else
#include "bt_adp_fs.h"  /* FS APIs */
#include "bt_adp_fs_ucs2.h"
#include "bt_osal.h"  /* str APIs */

#define  FHANDLE kal_int32 /* used in xml_def.h */
#endif

#include "xml_def.h"    /* XML_PARSER */

#ifdef    __DRM_SUPPORT__  
#include "drm_def.h"
#include "Drm_gprot.h"
#endif

#include "goep.h"

#include "bluetooth_bpp_common.h"
#include "bpp.h"
#include "bppi.h"

#ifdef __BPP_DEBUG__

//#define BPP_APP_STORE_DRV           MMI_PUBLIC_DRV
#define BPP_DEBUG_MAX_MEM_INFO      128
#define BPP_DEBUG_MAX_FH_INFO       32
#define BPP_DEBUG_MAX_FILE_NAME     32
#define BPP_DEBUG_FREE_MEM_NODE     (bpp_debug_x_mem_struct*)0xFFFFFFFF
#define BPP_DEBUG_FREE_FH_NODE      (bpp_debug_x_fh_struct*)0xFFFFFFFF

typedef struct _debug_x_mem
{
    struct _debug_x_mem *pnext;
    S8 file[BPP_DEBUG_MAX_FILE_NAME];
    U16 line;
    U16 ref;
    U32 time;
    U32 mem_size;
    S8 *pbuf;
} bpp_debug_x_mem_struct;

typedef struct _debug_x_fh
{
    struct _debug_x_fh *pnext;
    S8 file[BPP_DEBUG_MAX_FILE_NAME];
    U16 line;
    BTMTK_FS_HANDLE fh;
} bpp_debug_x_fh_struct;

typedef struct
{
    bpp_debug_x_mem_struct *pdebug_mem;
    bpp_debug_x_fh_struct *pdebug_fh;
    kal_bool init;
    bpp_debug_x_mem_struct debug_mem_info[BPP_DEBUG_MAX_MEM_INFO];
    bpp_debug_x_fh_struct debug_fh_info[BPP_DEBUG_MAX_FH_INFO];
    S8 db_file[64];
} bpp_debug_x_info_struct;


static bpp_debug_x_info_struct bpp_debug_x_info;

U16 bpp_private_path[BPP_MAX_FILE_NAME_LEN];

/*
#ifdef BTMTK_ON_LINUX
#define BPP_BTMTK_ROOT_PATH      L"@btmtk"
#define BPP_BTMTK_PROFILE_PATH   L"/profile"
#else
#define BPP_BTMTK_ROOT_PATH      L"\\@btmtk"
#define BPP_BTMTK_PROFILE_PATH   L"\\profile"
#endif

#ifdef BTMTK_ON_WISE

#define BPP_BTMTK_PARENT_PATH  L"c:"

#else 

#endif 

#define BPP_GET_FULL_PATH(fullpath,filename) \
do{                                              \
    bpp_get_private_folder((S8*)fullpath);        \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)L"\\");      \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)filename);   \
}while(0)

*/
//mtk03036 20100512
//move to bppi.h
/*
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
  #define BPP_BTMTK_PARENT_PATH  L"/data/data/com.mediatek.bluetooth"
  #define BPP_BTMTK_ROOT_PATH    L"/btmtk"
  #define BPP_BTMTK_PROFILE_PATH L"/profile"
  #define BPP_GET_FULL_PATH(fullpath,filename) \
do{                                              \
    bpp_get_private_folder((S8*)fullpath);        \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)L"/");      \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)filename);   \
}while(0)

#else 
  #define BPP_BTMTK_ROOT_PATH      L"\\@btmtk"
  #define BPP_BTMTK_PROFILE_PATH   L"\\profile"
  #define BPP_GET_FULL_PATH(fullpath,filename) \
do{                                              \
    bpp_get_private_folder((S8*)fullpath);        \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)L"\\");      \
    btmtk_wstrcat((U16 *)fullpath, (U16 *)filename);   \
}while(0)
#endif
*/
//mtk03036 20100512 end


#define BPP_DEBUG_WR(str)    bpp_debug_x_output_2file((S8*)bpp_debug_x_info.db_file,str,0)


#endif

#ifndef __ON_MAUI__
extern int sprintf(char *buffer, const char *format, ... );
#endif

#define PDU_FUNC
/*****************************************************************************
 * FUNCTION
 *  bpp_read_from_pdu
 * DESCRIPTION
 *  Modify the length of data in the psend_obj->pdu_in which would be sent immediately. Save the length of data, which has read but not send immediately, in psend_obj->pdu_len
 * PARAMETERS
 *  psend_obj           Contain the data would be sent immediately.
 *  size                The length of data would be send immediately.
 * RETURNS
 *  The data would be send immediately.
 *****************************************************************************/
bpp_head_buff_struct *bpp_read_from_pdu(bpp_send_obj_struct *psend_obj, kal_uint16 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_head_buff_struct *pdu_in = NULL;
    kal_uint16 pdu_remain = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (psend_obj->pdu_len)
    {
        pdu_in = psend_obj->pdu_in;
        pdu_remain = psend_obj->pdu_len - size;
        psend_obj->pdu_len = pdu_remain;
        pdu_in->pdu_len = size;

        if (pdu_remain)
        {
            pdu_in->free_tail_space += pdu_remain;
            bpp_hold_head_buf(pdu_in);
        }
        else
        {
            psend_obj->pdu_in = NULL;
        }
    }
    return pdu_in;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_update_pdu
 * DESCRIPTION
 *  
 * PARAMETERS
 *  psend_obj      
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_update_pdu(bpp_send_obj_struct *psend_obj)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(psend_obj);
    psend_obj->pdu_in->free_header_space += psend_obj->pdu_in->pdu_len;
    psend_obj->pdu_in->pdu_len = psend_obj->pdu_len;
    psend_obj->pdu_in->free_tail_space -= psend_obj->pdu_len;
}


void* bpp_get_pdu_ptr(bpp_head_buff_struct *buff_ptr, kal_uint16 *length_ptr)
{

   *length_ptr = buff_ptr->pdu_len;

   return(void *)((kal_uint8 *)buff_ptr + sizeof(*buff_ptr) +
                  buff_ptr->free_header_space);
}


/*****************************************************************************
 * FUNCTION
 *  bpp_pdu_add_header
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdu                      
 *  pdata                   
 *  data_len                
 * RETURNS
 *  
 *****************************************************************************/
kal_bool bpp_pdu_add_header(bpp_head_buff_struct *pdu, const kal_char *pdata, kal_uint32 data_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pdata && pdu && pdu->free_header_space > data_len)
    {
        kal_uint32 buf_len = 0;
        kal_char *pbuf = bpp_get_pdu_ptr(pdu, (kal_uint16*) & buf_len);

        pbuf -= data_len;
        strncpy(pbuf, pdata, data_len);
        pdu->pdu_len += (kal_uint16)data_len;
        pdu->free_header_space -= (kal_uint16)data_len;
		
        return KAL_TRUE;
    }
    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_pdu_append_tail
 * DESCRIPTION
 *  
 * PARAMETERS
 *  pdu          
 *  pdata        
 *  data_len     
 * RETURNS
 *  
 *****************************************************************************/
kal_bool bpp_pdu_append_tail(bpp_head_buff_struct *pdu, const kal_char *pdata, kal_uint16 data_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pdata && pdu && pdu->free_tail_space >=data_len)
    {
        kal_uint16 buf_len = 0;
        kal_char *pbuf = bpp_get_pdu_ptr(pdu, &buf_len);

        pbuf += buf_len;

        strncpy(pbuf, pdata, data_len);
        pdu->pdu_len += data_len;
        pdu->free_tail_space -= data_len;
		
        return KAL_TRUE;
    }
    return KAL_FALSE;
}

void* bpp_construct_head_buff(kal_uint16 pdu_len, kal_uint16  header_len, kal_uint16 tail_len)
{

   bpp_head_buff_struct *buff_ptr;

   //ASSERT((pdu_len + header_len + tail_len) != 0, pdu_len, header_len, tail_len);

   buff_ptr = get_ctrl_buffer(sizeof(*buff_ptr) + header_len + pdu_len + tail_len);

   buff_ptr->ref_count = 1;
   buff_ptr->free_header_space = header_len;
   buff_ptr->pdu_len = pdu_len;
   buff_ptr->free_tail_space = tail_len;

   return(void*)buff_ptr;
}

void bpp_free_head_buff(bpp_head_buff_struct *pdu_ptr)
{
   if (pdu_ptr != NULL) 
   {

      --(pdu_ptr->ref_count);

      if (pdu_ptr->ref_count == 0) 
	  {

         free_ctrl_buffer(pdu_ptr);
      }
   } 
   else 
   {

      Report(("Warning: PDU ptr is already NULL.\n"));
   }
}

kal_bool bpp_hold_head_buff(bpp_head_buff_struct *buff_ptr)
{
   if (buff_ptr != NULL) {

      buff_ptr->ref_count++;

      return KAL_TRUE;
   }

   return KAL_FALSE;
}


void bpp_clear_pdu_data(bpp_head_buff_struct *pdu)
{
    pdu->free_tail_space  += pdu->free_header_space+ pdu->pdu_len;
    pdu->free_header_space = 0;
    pdu->pdu_len = 0;
}

#define FS_API_FUNC

extern S32 bt_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size,const U8 *src);

int bpp_fs_open(const WCHAR * FileName, unsigned int Flag)
{
 	int ret;
	
	ret = btmtk_fs_open_ucs2(FileName, Flag);

#ifdef __BPP_DEBUG__
	if (ret < 0)
	{
	    U8 asc_filename[BTMTK_MAX_FILE_NAME_LEN];

		bt_chset_ucs2_to_utf8_string(asc_filename,BTMTK_MAX_FILE_NAME_LEN,(U8*)FileName);

	    OS_Report("[BPP] btmtk_fs_open(%s) return (%d) ", asc_filename, ret);
	}

#endif

	return ret;
}

int bpp_fs_close(BPP_FS_HANDLE FileHandle)
{
	int ret;
	
	ret = btmtk_fs_close_ucs2(FileHandle);

	return ret;
}

int bpp_fs_seek(BPP_FS_HANDLE FileHandle, S32 Offset, S32 Whence)
{
	int ret;
	
	ret = btmtk_fs_seek_ucs2(FileHandle, Offset, Whence);

	return ret;
}

//int bpp_fs_read(BPP_FS_HANDLE FileHandle, void * DataPtr, unsigned int Length, unsigned int * Read)
int bpp_fs_read(BPP_FS_HANDLE FileHandle, void * DataPtr, int Length, int * Read)
{
	int ret;

	ret = btmtk_fs_read_ucs2(FileHandle, DataPtr, Length, Read);

	return ret;
}

//int bpp_fs_write(BPP_FS_HANDLE FileHandle, void * DataPtr, unsigned int Length, unsigned int * Written)
int bpp_fs_write(BPP_FS_HANDLE FileHandle, void * DataPtr, int Length, int * Written)
{
	int ret;

	ret = btmtk_fs_write_ucs2(FileHandle, DataPtr, Length, Written);

	return ret;
}

int bpp_fs_createdir(const WCHAR * DirName)
{
	int ret;

	ret = btmtk_fs_create_dir_ucs2(DirName);
	
#ifdef __BPP_DEBUG__
	{
	    U8 asc_dirname[BTMTK_MAX_FILE_NAME_LEN];

		bt_chset_ucs2_to_utf8_string(asc_dirname,BTMTK_MAX_FILE_NAME_LEN,(U8*)DirName);

	    OS_Report("[BPP] bpp_fs_createdir(%s) return (%d) ", asc_dirname, ret);
	}

#endif
	
	return ret;
}

int bpp_fs_delete(const WCHAR * FileName)
{
	int ret;

	ret = btmtk_fs_delete_ucs2(FileName);

	return ret;
}

int bpp_fs_getfilesize(BPP_FS_HANDLE FileHandle, unsigned int * Size)
{
	int ret;

	ret = btmtk_fs_get_filesize_ucs2(FileHandle, (kal_uint32*)Size);
	
	return ret;
}



#define STRING_ENCODE_UTIL

/*****************************************************************************
 * FUNCTION
 *  bpp_utf8_to_ucs2
 * DESCRIPTION
 *  This function covet utf8 char to ucs2 encoding
 * PARAMETERS
 *  ucs2    
 *  utf8    
 * RETURNS
 *  void
 *****************************************************************************/
static kal_uint32 bpp_utf8_to_ucs2_char(kal_uint16 *ucs2, kal_uint8 *utf8)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 c = utf8[0];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (c < 0x80)
    {
        *ucs2 = c;
        return 1;
    }
    else if (c < 0xe0)
    {
        *ucs2 = ((kal_uint16) (c & 0x1f) << 6) | (kal_uint16) (utf8[1] ^ 0x80);
        return 2;
    }
    else
    {
        *ucs2 = ((kal_uint16) (c & 0x0f) << 12) | ((kal_uint16) (utf8[1] ^ 0x80) << 6) | (kal_uint16) (utf8[2] ^ 0x80);
        return 3;
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_utf8_to_ucs2_string
 * DESCRIPTION
 *  This function covert utf8 to ucs2 string
 * PARAMETERS
 *  dest           
 *  dest_size        
 *  src             
 * RETURNS
 *  void
 *****************************************************************************/
kal_int32 bpp_utf8_to_ucs2_string(kal_uint8 *dest, kal_int32 dest_size, kal_uint8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int32 pos = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (*src)
    {
        src += bpp_utf8_to_ucs2_char((kal_uint16*) (dest + pos), src);
        pos += 2;
        if (pos >= dest_size - 2)
        {
            break;
        }
    }
    dest[pos] = 0;
    dest[pos + 1] = 0;
    return pos + 2;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_get_private_folder
 * DESCRIPTION
 *
 * PARAMETERS
 *  pathbuf  
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_get_private_folder(S8 *pathbuf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pathbuf)
    {
        btmtk_wstrcpy((WCHAR*)pathbuf, bpp_private_path);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bpp_create_private_folder
 * DESCRIPTION
 *
 * PARAMETERS
 *  pathbuf  
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_create_private_folder(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	int fh;
	int ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

//mtk03036 20100406
#ifdef BTMTK_ON_WISE
	btmtk_wstrcpy(bpp_private_path, BPP_BTMTK_PARENT_PATH);
#elif defined(BTMTK_ON_LINUX)
	btmtk_wstrcpy(bpp_private_path, BPP_BTMTK_PARENT_PATH);
#else
	// get current path into bpp_private_folder_at_cur_path
	btmtk_fs_get_current_dir_ucs2(sizeof(bpp_private_path), bpp_private_path);
#endif
//mtk03036 20100406 end	
	
	btmtk_wstrcat((WCHAR *)bpp_private_path, BPP_BTMTK_ROOT_PATH);

	/* check "\\@btmtk" path */
	if ((fh = bpp_fs_open(bpp_private_path, BTMTK_FS_READ_ONLY)) < 0)
	{
		ret = bpp_fs_createdir(bpp_private_path);
		
		/* create "\\profile" path */	
		btmtk_wstrcat((WCHAR *)bpp_private_path, BPP_BTMTK_PROFILE_PATH);
		ret = bpp_fs_createdir(bpp_private_path);
	}
	else
	{
		bpp_fs_close(fh);

		/* check \\profile path */	
		btmtk_wstrcat((WCHAR *)bpp_private_path, BPP_BTMTK_PROFILE_PATH);
		fh = bpp_fs_open(bpp_private_path, BTMTK_FS_READ_ONLY);
		if (fh < 0)
		{
			ret = bpp_fs_createdir(bpp_private_path);
		}
		else
		{
			bpp_fs_close(fh);
		}
	}
}


#ifdef __BPP_DEBUG__

void bpp_debug_x_output2(kal_wchar* file_name,kal_char *str,kal_uint32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WCHAR db_file[128];
    BTMTK_FS_HANDLE bpp_db_fh = -1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_get_private_folder((kal_char*)db_file);
	
    btmtk_wstrcat(db_file,file_name);

    bpp_db_fh = bpp_fs_open(db_file, BTMTK_FS_READ_WRITE);
    if (len == 0 && str)
    {
        len = strlen(str);
    }
    if (str && bpp_db_fh > 0)
    {
        //U32 bpp_write_len = 0;
        int bpp_write_len = 0;
		
        bpp_fs_seek(bpp_db_fh, 0, BTMTK_FS_SEEK_END);
        bpp_fs_write(bpp_db_fh, (void*)str, len, &bpp_write_len);
    }
    if (bpp_db_fh > 0)
    {
        bpp_fs_close(bpp_db_fh);
    }
}
#endif /*__BPP_DEBUG__*/

#ifdef __BPP_DEBUG__


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_init
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i = 0;
	bpp_debug_x_info_struct *pbpp_debug_x_info = &bpp_debug_x_info;
    BTMTK_FS_HANDLE bpp_db_fh = -1;
    WCHAR db_file[BPP_MAX_FILE_NAME_LEN + 64];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_debug_x_info.init = KAL_FALSE;                         
    btmtk_wstrcpy((kal_wchar*)bpp_debug_x_info.db_file,(kal_wchar*)L"bpp_xhtml.bin");

    if (pbpp_debug_x_info->init == KAL_FALSE)
    {
        /* memset(pbpp_debug_x_info,0,sizeof(bpp_debug_x_info_struct)); */
        for (i = 0; i < BPP_DEBUG_MAX_MEM_INFO; i++)
        {
            pbpp_debug_x_info->debug_mem_info[i].pnext = BPP_DEBUG_FREE_MEM_NODE;
        }

        for (i = 0; i < BPP_DEBUG_MAX_FH_INFO; i++)
        {
            pbpp_debug_x_info->debug_fh_info[i].pnext = BPP_DEBUG_FREE_FH_NODE;
        }
        pbpp_debug_x_info->init = KAL_TRUE;
        pbpp_debug_x_info->pdebug_mem = NULL;
        pbpp_debug_x_info->pdebug_fh = NULL;

        BPP_GET_FULL_PATH(db_file, BPP_DB_SOAP_OUT_FILE);
		bpp_fs_delete(db_file);
        bpp_db_fh = bpp_fs_open(db_file, BTMTK_FS_CREATE|BTMTK_FS_CREATE_ALWAYS);
        if (bpp_db_fh > 0)
        {
            bpp_fs_close(bpp_db_fh);
        }
        
        BPP_GET_FULL_PATH(db_file, BPP_DB_SOAP_IN_FILE);
		bpp_fs_delete(db_file);
        bpp_db_fh = bpp_fs_open(db_file, BTMTK_FS_CREATE|BTMTK_FS_CREATE_ALWAYS);
        if (bpp_db_fh > 0)
        {
            bpp_fs_close(bpp_db_fh);
        }
        
        BPP_GET_FULL_PATH(db_file, pbpp_debug_x_info->db_file);
		bpp_fs_delete(db_file);
        bpp_db_fh = bpp_fs_open(db_file, BTMTK_FS_CREATE|BTMTK_FS_CREATE_ALWAYS);
        if (bpp_db_fh > 0)
        {
            bpp_fs_close(bpp_db_fh);
        }
		
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_output_2file
 * DESCRIPTION
 *
 * PARAMETERS
 *  file_name     
 *  str           
 *  len           
 * RETURNS
 *  void
 *****************************************************************************/
static void bpp_debug_x_output_2file(S8 *file_name, S8 *str, U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WCHAR db_file[BPP_MAX_FILE_NAME_LEN+64];
    BTMTK_FS_HANDLE bpp_db_fh = -1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BPP_GET_FULL_PATH(db_file, file_name);
    bpp_db_fh = bpp_fs_open(db_file, BTMTK_FS_READ_WRITE);
    if (len == 0 && str)
    {
        len = strlen(str);
    }
    if (str && bpp_db_fh > 0)
    {
        //U32 bpp_write_len = 0;
        int bpp_write_len = 0;		

        bpp_fs_seek(bpp_db_fh, 0, BTMTK_FS_SEEK_END);
        bpp_fs_write(bpp_db_fh, (void*)str, len, &bpp_write_len);
    }
    if (bpp_db_fh > 0)
    {
        bpp_fs_close(bpp_db_fh);
    }
}

#define  bpp_debug_x_free_mem_node(pmem_node)  \
do{                                          \
    pmem_node->pnext=BPP_DEBUG_FREE_MEM_NODE;\
}while(0);

#define  bpp_debug_x_free_fh_node(pfh_node)  \
do{                                        \
    pfh_node->pnext=BPP_DEBUG_FREE_FH_NODE;\
}while(0);


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_alloc_mem_node
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbpp_debug_x_info   
 * RETURNS
 *
 *****************************************************************************/
static bpp_debug_x_mem_struct *bpp_debug_x_alloc_mem_node(bpp_debug_x_info_struct *pbpp_debug_x_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (; i < BPP_DEBUG_MAX_MEM_INFO; i++)
    {
        if (pbpp_debug_x_info->debug_mem_info[i].pnext == BPP_DEBUG_FREE_MEM_NODE)
        {
            pbpp_debug_x_info->debug_mem_info[i].pnext = NULL;
            return &pbpp_debug_x_info->debug_mem_info[i];
        }
    }
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_alloc_fh_node
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbpp_debug_x_info   
 * RETURNS
 *
 *****************************************************************************/
static bpp_debug_x_fh_struct *bpp_debug_x_alloc_fh_node(bpp_debug_x_info_struct *pbpp_debug_x_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (; i < BPP_DEBUG_MAX_FH_INFO; i++)
    {
        if (pbpp_debug_x_info->debug_fh_info[i].pnext == BPP_DEBUG_FREE_FH_NODE)
        {
            pbpp_debug_x_info->debug_fh_info[i].pnext = NULL;
            return &pbpp_debug_x_info->debug_fh_info[i];
        }
    }
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_add_mem_node
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbpp_debug_x_info   
 *  pbuf              
 *  size              
 *  pfile             
 *  line              
 * RETURNS
 *
 *****************************************************************************/
static kal_bool bpp_debug_x_add_mem_node(
                    bpp_debug_x_info_struct *pbpp_debug_x_info,
                    S8 *pbuf,
                    U32 size,
                    S8 *pfile,
                    U32 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_debug_x_mem_struct *pmem_node = bpp_debug_x_alloc_mem_node(pbpp_debug_x_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pmem_node)
    {
        S32 i = strlen(pfile) - 1;
        S8 *pfilename = pfile;
        //kal_get_time(&pmem_node->time);
        pmem_node->line = (U16)line;
        pmem_node->pbuf = pbuf;
        pmem_node->mem_size = size;
        pmem_node->ref = 1;
        for (; i > 0; i--)
        {
            if (pfile[i] == '\\' || pfile[i] == '/')
            {
                pfilename = &pfile[i + 1];
                break;
            }
        }
        strcpy((S8*) pmem_node->file, pfilename);
        pmem_node->pnext = pbpp_debug_x_info->pdebug_mem;
        pbpp_debug_x_info->pdebug_mem = pmem_node;
        //printf("\r\nBPP_MEM_ALLOC: file--%s, lin--%d, pbuf--%d\r\n",pfilename,line,(int)pbuf);
        return KAL_TRUE;
    }
    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_add_fh_node
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbpp_debug_x_info   
 *  fh                  
 *  pfile               
 *  line                
 * RETURNS
 *
 *****************************************************************************/
static kal_bool bpp_debug_x_add_fh_node(bpp_debug_x_info_struct *pbpp_debug_x_info, BTMTK_FS_HANDLE fh, S8 *pfile, U32 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_debug_x_fh_struct *pfh_node = bpp_debug_x_alloc_fh_node(pbpp_debug_x_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pfh_node && fh > 0)
    {
        S32 i = strlen(pfile) - 1;
        S8 *pfilename = pfile;

        pfh_node->line = (U16)line;
        pfh_node->fh = fh;
        for (; i > 0; i--)
        {
            if (pfile[i] == '\\' || pfile[i] == '/')
            {
                pfilename = &pfile[i + 1];
                break;
            }
        }
        strcpy(pfh_node->file, pfilename);
        pfh_node->pnext = pbpp_debug_x_info->pdebug_fh;
        pbpp_debug_x_info->pdebug_fh = pfh_node;
        return KAL_TRUE;
    }
    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_remove_fh_node
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbpp_debug_x_info    
 *  fh                 
 *  pfile              
 *  line               
 * RETURNS
 *
 *****************************************************************************/
static kal_bool bpp_debug_x_remove_fh_node(bpp_debug_x_info_struct *pbpp_debug_x_info, BTMTK_FS_HANDLE fh, S8 *pfile, U32 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_debug_x_fh_struct *pfh_node = pbpp_debug_x_info->pdebug_fh;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (fh <= 0 || pfh_node == NULL)
    {
        return KAL_FALSE;
    }

    if (pfh_node->fh == fh)
    {
        pbpp_debug_x_info->pdebug_fh = pfh_node->pnext;
        bpp_debug_x_free_fh_node(pfh_node);
        return KAL_TRUE;
    }
    else
    {
        bpp_debug_x_fh_struct *pre = pfh_node;

        pfh_node = pre->pnext;

        while (pfh_node)
        {
            if (pfh_node->fh == fh)
            {
                pre->pnext = pfh_node->pnext;
                bpp_debug_x_free_fh_node(pfh_node);
                return KAL_TRUE;
            }
            pre = pfh_node;
            pfh_node = pre->pnext;
        }
    }
    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_remove_mem_node
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbpp_debug_x_info   
 *  pbuf            
 *  pfile           
 *  line            
 * RETURNS
 *
 *****************************************************************************/
static kal_bool bpp_debug_x_remove_mem_node(bpp_debug_x_info_struct *pbpp_debug_x_info, S8 *pbuf, S8 *pfile, U32 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_debug_x_mem_struct *pmem_node = pbpp_debug_x_info->pdebug_mem;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pbuf == NULL || pmem_node == NULL)
    {
        return KAL_FALSE;
    }

    if (pmem_node->pbuf == pbuf)
    {
        pbpp_debug_x_info->pdebug_mem = pmem_node->pnext;
        bpp_debug_x_free_mem_node(pmem_node);
        return KAL_TRUE;
    }
    else
    {
        bpp_debug_x_mem_struct *pre = pmem_node;

        pmem_node = pre->pnext;

        while (pmem_node)
        {
            if (pmem_node->pbuf == pbuf)
            {
                pre->pnext = pmem_node->pnext;
                bpp_debug_x_free_mem_node(pmem_node);
                return KAL_TRUE;
            }
            pre = pmem_node;
            pmem_node = pre->pnext;
        }
    }

    return KAL_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_mem_alloc
 * DESCRIPTION
 *
 * PARAMETERS
 *  size      
 *  pfile     
 *  line      
 * RETURNS
 *  void
 *****************************************************************************/
void *bpp_debug_x_mem_alloc(U32 size, S8 *pfile, U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    void *pbuf = get_ctrl_buffer(size);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pbuf)
    {
        bpp_debug_x_add_mem_node(&bpp_debug_x_info, pbuf, size, pfile, line);
    }
    return pbuf;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_mem_free
 * DESCRIPTION
 *
 * PARAMETERS
 *  pbuf     
 *  pfile    
 *  line     
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_mem_free(S8 *pbuf, S8 *pfile, U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pbuf)
    {
        free_ctrl_buffer(pbuf);

        bpp_debug_x_remove_mem_node(&bpp_debug_x_info, pbuf, pfile, line);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_fs_open
 * DESCRIPTION
 *
 * PARAMETERS
 *  pfilename     
 *  flags         
 *  pfile         
 *  line          
 * RETURNS
 *
 *****************************************************************************/
BTMTK_FS_HANDLE bpp_debug_x_fs_open(const WCHAR *pfilename, U32 flags, S8 *pfile, U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BTMTK_FS_HANDLE fh = bpp_fs_open(pfilename, flags);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (fh > 0)
    {
        bpp_debug_x_add_fh_node(&bpp_debug_x_info, fh, pfile, line);
    }
    return fh;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_fs_close
 * DESCRIPTION
 *
 * PARAMETERS
 *  fh         
 *  pfile      
 *  line       
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_fs_close(BTMTK_FS_HANDLE fh, S8 *pfile, U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (fh > 0)
    {
        bpp_fs_close(fh);
        bpp_debug_x_remove_fh_node(&bpp_debug_x_info, fh, pfile, line);
    }
}



/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_alloc_peer_buf
 * DESCRIPTION
 *
 * PARAMETERS
 *  pdu_len         [IN]
 *  header_len      [IN]
 *  tail_len        [IN]
 *  direction       [IN]
 *  pfile           [?]
 *  line            [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void *bpp_debug_x_alloc_head_buf(
        U16 pdu_len,
        kal_int16 header_len,
        kal_int16 tail_len,
        kal_int16 direction,
        S8 *pfile,
        U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bpp_head_buff_struct *pdu = bpp_construct_head_buff(pdu_len, header_len, tail_len);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pdu)
    {
        bpp_debug_x_add_mem_node(&bpp_debug_x_info, (kal_char*) pdu, pdu_len + header_len + tail_len, pfile, line);
    }
    return pdu;
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_free_peer_buf
 * DESCRIPTION
 *
 * PARAMETERS
 *  pdu        
 *  pfile      
 *  line       
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_free_head_buf(bpp_head_buff_struct *pdu, S8 *pfile, U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pdu)
    {
        bpp_free_head_buff(pdu);
        bpp_debug_x_remove_mem_node(&bpp_debug_x_info, (kal_char*) pdu, pfile, line);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_hold_peer_buf
 * DESCRIPTION
 *
 * PARAMETERS
 *  pdu       
 *  pfile     
 *  line      
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_hold_head_buf(bpp_head_buff_struct *pdu, S8 *pfile, U16 line)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (pdu)
    {
        bpp_hold_head_buff(pdu);
        bpp_debug_x_add_mem_node(
            &bpp_debug_x_info,
            (kal_char*) pdu,
            pdu->pdu_len + pdu->free_tail_space + pdu->free_header_space,
            pfile,
            line);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_output_2file
 * DESCRIPTION
 *
 * PARAMETERS
 *  str    
 *  len    
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_output(S8 *str, U32 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bpp_debug_x_output_2file(bpp_debug_x_info.db_file, str, len);
}



/*****************************************************************************
 * FUNCTION
 *  bpp_debug_x_mem_fh_summary
 * DESCRIPTION
 *
 * PARAMETERS
 *  debug_info     
 * RETURNS
 *  void
 *****************************************************************************/
void bpp_debug_x_mem_fh_summary(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	bpp_debug_x_info_struct *debug_info = &bpp_debug_x_info;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	
    if (debug_info && debug_info->init == KAL_TRUE)
    {
        S8 temp_buf[256];
        bpp_debug_x_mem_struct *pdebug_mem = debug_info->pdebug_mem;
        bpp_debug_x_fh_struct *pdebug_fh = debug_info->pdebug_fh;        

        sprintf(temp_buf,"\r\n===bpp_debug_x_mem_fh_summary START ===\r\n");
        BPP_DEBUG_WR(temp_buf);

        while(pdebug_mem)
        {
            sprintf(temp_buf,"\r\n\r\nMEMORY_LEAK: file--%s,line--%d,size--%lu,time--%lu\r\n\r\n",pdebug_mem->file,pdebug_mem->line,pdebug_mem->mem_size,pdebug_mem->time);
            BPP_DEBUG_WR(temp_buf);
            pdebug_mem = pdebug_mem->pnext;
        }
		
        while(pdebug_fh)
        {
            sprintf(temp_buf,"\r\n\r\nFILE_NOT_CLOSE: file--%s,line--%d\r\n\r\n",pdebug_fh->file,pdebug_fh->line);
            BPP_DEBUG_WR(temp_buf);
            pdebug_fh = pdebug_fh->pnext;
        }
		
        sprintf(temp_buf,"\r\n===bpp_debug_x_mem_fh_summary END ===\r\n");
        BPP_DEBUG_WR(temp_buf);
    }
        
	//bpp_fs_delete(BPP_DB_SOAP_OUT_FILE);
	//bpp_fs_delete(BPP_DB_SOAP_IN_FILE);
}

#endif /* __BPP_APP_DEBUG__ */


