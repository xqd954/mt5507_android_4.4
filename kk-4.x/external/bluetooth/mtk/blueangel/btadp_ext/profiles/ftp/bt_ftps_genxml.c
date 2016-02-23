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
 * bt_ftps_xml.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   
 *
 * Author:
 * -------
 * Will Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/


#include "bt_goep_porting.h"

#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"
#include "ext_osal.h"
#include "ext_nbuffer.h"
#include "bluetooth_ftps_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_ftps_hdl.h"
#include "bt_ftps_struct.h"
#include "bt_ftps_event.h"
#include "bt_ftps_api.h"

#include "ext_ucs2.h"
#include "bt_goep_fs.h"
#include "bt_ftp_porting.h"
#include "bt_ext_debug.h"

#define GOEP_Report bt_ext_log

#include "stdio.h"
#ifdef BTMTK_ON_WIN32
#include "io.h"
#endif

#ifdef BTMTK_ON_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

extern bt_ftps_cntx_struct *const ftps_cntx_p;
extern bt_ftps_conn_cntx_struct *act_server_cntx_p;

/***** Internal functions *****/
S32 bt_ftps_writer_put_str(bt_ftp_file_writer_struct *composer, U8* data);
static S32 bt_ftps_compose_write_ucs2_name(bt_ftp_file_writer_struct *composer, const U8* name);
S32 bt_ftps_compose_folder_element(
            bt_ftp_file_writer_struct *composer,
            bt_ftp_obj_info_struct *file_info);

S32 bt_ftps_compose_file_element(bt_ftp_file_writer_struct *composer, bt_ftp_obj_info_struct *file_info);
static S32 bt_ftps_writer_put_data(bt_ftp_file_writer_struct *composer, U8* data, U32 len);
S32 bt_ftps_writer_flush(bt_ftp_file_writer_struct *composer, U8* data);
FS_STATUS bt_ftps_flush_composer(bt_ftp_file_writer_struct *composer, FHANDLE fh);
S32 bt_ftps_compose_end_element(bt_ftp_file_writer_struct *composer);
BT_BOOL bt_ftps_init_file_info(bt_ftp_obj_info_struct *info);



BT_BOOL bt_ftps_util_convert_record_to_fileinfo(bt_ftps_filter_record *record, bt_ftp_obj_info_struct *info){
	if( NULL == record || NULL == info ){
		return FALSE;
	}

	info->isFile = (!record->ucFolder);
	ext_ucs2ncpy( (S8 *) info->file_name, (S8 *) record->ucObjName, sizeof(info->file_name));
	info->name_len = ext_ucs2strlen( (const S8 *) info->file_name);
	if( info->name_len > (sizeof(info->file_name)/2) ){
		OS_Report( "[FTPS] util_convert_record_to_fileinfo() name_len:%d max_len:%d", info->name_len, sizeof(info->file_name)/2 );
	}
	return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *   bt_ftps_util_find_poststring
 * DESCRIPTION
 *   find the post-substring. find ".xml" in filename.
 * PARAMETERS
 * RETURNS
 *  BT_BOOL - TRUE: add to xml file.  FALSE: NOT to add to xml file
 *****************************************************************************/
BT_BOOL bt_ftps_util_find_poststring(const U16 *u2String, const U16 *u2SubString ){
	const U16 *p1, *p2;
	U8 stirng_len, substring_len;
	p1 = u2String;

	stirng_len = ext_ucs2strlen( (const S8 *) u2String);
	substring_len = ext_ucs2strlen( (const S8 *) u2SubString);
	if( 0 == substring_len ){
		return FALSE;
	}
	if( stirng_len < substring_len ){
		return FALSE;
	}
	
	p1 = u2String + (stirng_len-substring_len) ;
	p2 = u2SubString;     		
	if( (U16) *p1 ==  (U16) *p2 ){
		while( *p1 != 0 && *p2 != 0 ){
			p1++;
			p2++;
			if( (U16) *p1 !=  (U16) *p2 ){
				break;
			}
		}
		if( *p2 == 0 && *p1 == 0){
			return TRUE;
		}
	}

	return FALSE;
}

/*****************************************************************************
 * FUNCTION
 *   bt_ftps_util_filter_check
 * DESCRIPTION
 *   Test the obj is visible or not
 * PARAMETERS
 *  bt_ftp_obj_info_struct      [IN]   current file/folder information
 *  bt_ftps_folder_filter       [IN]   filter
 * RETURNS
 *  BT_BOOL - TRUE: add to xml file.  FALSE: NOT to add to xml file
 *****************************************************************************/
BT_BOOL bt_ftps_util_filter_check(bt_ftp_obj_info_struct *info, bt_ftps_folder_filter *filter){
	U32 index;
	BT_BOOL l_ret;
	U16 *l_u2CompareString;
	l_ret = TRUE; // allow to add by default

	
	if( NULL !=  filter ){
		for( index = 0; index < filter->u4NumFilter; index++ ){
			if( FALSE == filter->records[index].ucAdd ){
				if( info->isFile != filter->records[index].ucFolder &&  0 == ext_ucs2cmp( (const U8 *) info->file_name , (const U8 *) filter->records[index].ucObjName )  ) {
					// find the remove match
					return FALSE;
				}
				
				if( info->isFile != filter->records[index].ucFolder && NULL != ext_ucs2rchr( (const S8 *) filter->records[index].ucObjName, (U16)L'*') ){
					// compare the *.jpg *.gif. find the remove match
					if( filter->records[index].ucObjName == (U8 *) ext_ucs2rchr( (const S8 *) filter->records[index].ucObjName, (U16)L'*') ){
						l_u2CompareString = ( U16 *) filter->records[index].ucObjName;
						l_u2CompareString++;
						if( TRUE == bt_ftps_util_find_poststring( (U16 *)info->file_name , (U16 *) l_u2CompareString ) ){
							return FALSE;
						}
					}
				}

			}else{
				// mark the found one
				if( 0 == filter->records[index].ucMark){
					if( info->isFile != filter->records[index].ucFolder &&  0 == ext_ucs2cmp( (const U8 *) info->file_name , (const U8 *) filter->records[index].ucObjName )  ) {
						// find the addition match. Mark it as added one to avoid add twice
						filter->records[index].ucMark = 0x1; // Mark as tested one.
					}
				}
			}
		}
	}
	return l_ret;
}

/**
* @brief This function is to generate the folder content as xml file and invoke xmlfile ready
*  bt_ftps_gen_folder_content
* Save a folder's context to a xml file and invoke btmtk_ftps_folder_xml_ready
* 
* @param ucfolderpath the path of open folder
* @param index           [IN]
* @see btmtk_ftps_folder_xml_ready
**/
S32 bt_ftps_gen_folder_content(bt_ftp_file_writer_struct *composer ,
	BT_BOOL isRoot, U8 *ucfolderpath,
	const U8 *ucXmlFilepath,
	U32 u4MaxRecord, bt_ftps_folder_filter *filter) {

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 result;
    //  bt_ftp_file_writer_struct *composer = &((act_server_cntx_p + 0)->xml_composer);
    bt_ftp_obj_info_struct ftp_file_info;
	    
    const U8 * file_name = ucXmlFilepath;  // output filepath
    S32 i;

    GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_GEN_FOLDER_PARAM, isRoot, u4MaxRecord) );
    // GOEP_TRACE_PATH( BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FOLDERPATH, ucfolderpath );
    // GOEP_TRACE_PATH( BT_FTP_TRC_CLASS, BT_FTPC_TARGET_FILEPATH, ucXmlFilepath);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //FTP_DEBUG(kal_print("FTPS GEN FOLDER CONTENT\n");)
    
    /* continue generating folder content */
    if (0 == ucfolderpath  || 0 == ucXmlFilepath) {
	GOEP_TRACE((BT_FTP_TRC_CLASS,BT_FTPS_GEN_FOLDER_PARAM_CHECK_FAIL,
		ucfolderpath,
		ucXmlFilepath));
        OS_Report("[FTPS][ERR] gen_folder folderpath is 0(%d) xmlFilepath is 0(%d)",
		ucfolderpath,
		ucXmlFilepath);
        i = 0;
        //fs_file_info.Attributes = FS_ATTR_SYSTEM;
        return EXT_FS_ERR;
    }

    if (NULL != filter ) {
	for (i = 0; i < (S32)filter->u4NumFilter; i++) {
	    filter->records[i].ucMark = 0;
	}
    }

    if (0 == ext_ucs2strlen((const S8 *) ucXmlFilepath)) {
        return EXT_FS_ERR;
    }

    /* Composer initialization */
    GOEP_MEMSET((U8 *) composer, 0, sizeof(bt_ftp_file_writer_struct));
    composer->delimitor = 0x5a;

    composer->buflen += sprintf((S8 *) (composer->buf + composer->buflen), 
		    "<?xml version=\"1.0\"?>\n");
    composer->buflen += sprintf((S8 *) (composer->buf + composer->buflen),
		    "<!DOCTYPE folder-listing SYSTEM \"obex-folder-listing.dtd\">\n");
    composer->buflen += sprintf((S8 *) (composer->buf + composer->buflen),
		    "<folder-listing version=\"1.0\">\n");

    /* If the selecting folder is not root folder, represent the parent folder */
    if (FALSE == isRoot) {
        composer->buflen += sprintf((S8 *) (composer->buf + composer->buflen),
			"<parent-folder />\n");
    }

    /* open the output file */
    composer->fd = btmtk_goep_open_wstream((const U16*) ucXmlFilepath,
	    EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS);
    
    if (FALSE == btmtk_goep_is_valid_handle(composer->fd)) {
	GOEP_Report("[ftp][ERR] gen_folder fail to open_wstream:(%d) ", (S32) composer->fd);
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_UTIL_OPEN_FILE_FAIL, composer->fd));
	return EXT_FS_ERR;
    }

    bt_ftps_init_file_info(&ftp_file_info);

    if (EXT_FS_OK != btmtk_goep_fs_findfirst(ucfolderpath, &(composer->finddata), &ftp_file_info)) {
	GOEP_Report("[FTP][ERR] gen_folder fail to browse folder ");
	GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_GEN_FOLDER_FINDFIRST_FAIL)); 

	if (FALSE != btmtk_goep_is_valid_handle(composer->fd)) {
	    btmtk_goep_close_wstream(composer->fd);
	    composer->fd = FHANDLE_INVALID_VALUE;
	}

	return EXT_FS_ERR;
    }
	
    btmtk_goep_get_fileinfo( &ftp_file_info );
    if (TRUE  == bt_ftps_util_filter_check(&ftp_file_info, filter)) {
		if (FALSE != ftp_file_info.isFile) {
		    // file
		    if (isRoot != TRUE)	{
		    	result = bt_ftps_compose_file_element(composer, &ftp_file_info);
		    }
		} else {
			if (isRoot == TRUE) {
				if (btmtk_ftps_is_sdcard((U16 *) ftp_file_info.file_name) == TRUE &&
					btmtk_ftps_is_available_sdcard((const U16*)ucfolderpath, (const U16*)ftp_file_info.file_name) == TRUE) {
					result = bt_ftps_compose_folder_element(composer, &ftp_file_info);
				}
			} else {
				result = bt_ftps_compose_folder_element(composer, &ftp_file_info);
			}
		}
    }

    while (EXT_FS_OK == btmtk_goep_fs_findnext(composer->finddata, &ftp_file_info)) {
		btmtk_goep_get_fileinfo(&ftp_file_info);

		//save to xml file
		if (TRUE  == bt_ftps_util_filter_check(&ftp_file_info, filter)) {
		    if (FALSE != ftp_file_info.isFile) {
				// file
				if (isRoot != TRUE) {
					result = bt_ftps_compose_file_element(composer, &ftp_file_info);
				}
		    } else {
				if (isRoot == TRUE) {
					if (btmtk_ftps_is_sdcard((U16 *) ftp_file_info.file_name) == TRUE &&
						btmtk_ftps_is_available_sdcard((const U16*)ucfolderpath, (const U16*)ftp_file_info.file_name) == TRUE) {
						result = bt_ftps_compose_folder_element(composer, &ftp_file_info);
					}
				} else {
					result = bt_ftps_compose_folder_element(composer, &ftp_file_info);
				}

	 		}
		}
    }

    GOEP_Report("[FTPS] gen_folder browse finish ");
    btmtk_goep_fs_findend( composer->finddata );
    composer->finddata = 0;

    GOEP_Report("[FTPS] gen_folder browse add filter data ");
    // before ending compose the add one in filter
    if (NULL != filter) {
	GOEP_TRACE((BT_FTP_TRC_CLASS, BT_FTPS_GEN_FOLDER_ADD_EXTRA_OBJECT, filter->u4NumFilter));

	for (i = 0; i< (S32)filter->u4NumFilter; i++) {
	    if (0 == filter->records[i].ucMark && TRUE == filter->records[i].ucAdd) {
		/// not tested records.
		bt_ftps_init_file_info( &ftp_file_info );
		// filter_record to file_info
		bt_ftps_util_convert_record_to_fileinfo( &filter->records[i] , &ftp_file_info);

		if (FALSE != ftp_file_info.isFile) {
		    // file
		    result = bt_ftps_compose_file_element(composer, &ftp_file_info);
		} else {
		    result = bt_ftps_compose_folder_element(composer, &ftp_file_info);
		}
	    }
	}
    }
    
    result = bt_ftps_compose_end_element(composer);   
    bt_ftps_writer_flush(composer, NULL);
    btmtk_goep_close_wstream(composer->fd);
    composer->fd = FHANDLE_INVALID_VALUE;

    return EXT_FS_OK;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_compose_end_element
 * DESCRIPTION
 *  This function is to compose the end tag
 * PARAMETERS
 *  composer        [?]     [?]
 *  file_info       [?]     [?]
 *  a(?)            [IN/OUT]        First variable, used as returns(?)
 *  b(?)            [IN]            Second variable(?)
 * RETURNS
 *  The result of composing the end tag
 *****************************************************************************/
S32 bt_ftps_compose_end_element(bt_ftp_file_writer_struct *composer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 ret = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ret = bt_ftps_writer_put_str(composer, (U8*)"</folder-listing>");

    if (ret)
        return ret;
    return EXT_FS_OK;

    //return bt_ftps_writer_flush(composer, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_writer_put_str
 * DESCRIPTION
 *  This function is to compose 
 * PARAMETERS
 *  composer        [?]     [?]
 *  data            [?]     [?]
 * RETURNS
 *  The result of composing the folder attributes
 *****************************************************************************/
S32 bt_ftps_writer_put_str(bt_ftp_file_writer_struct *composer, U8* data)
{
    U32 len = ext_strlen((char*)data);

    return bt_ftps_writer_put_data(composer, data, len);    
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftps_writer_put_str
 * DESCRIPTION
 *  This function is to compose 
 * PARAMETERS
 *  composer        [?]     [?]
 *  data            [?]     [?]
 * RETURNS
 *  The result of composing the folder attributes
 *****************************************************************************/
static S32 bt_ftps_compose_write_ucs2_name(bt_ftp_file_writer_struct *composer, const U8* name)
{
    S32 ret = 0, i = 0;
    S32 utf8_len;
    U8 utf8_buf[6];

    while (name[i] || name[i + 1])
    {
        utf8_len = ext_chset_ucs2_to_utf8(utf8_buf, (U16)((name[i + 1]<<8) | name[i]));
        utf8_buf[utf8_len] = 0; /* utf8_len always be 1, 2, or 3 */

        if (utf8_len == 1)
        {
            switch (utf8_buf[0])
            {
            case '<':
                ret = bt_ftps_writer_put_str(composer, (U8*)"&lt;");
                break;
                
            case '>':
                ret = bt_ftps_writer_put_str(composer, (U8*)"&gt;");
                break;
                
            case '&':
                ret = bt_ftps_writer_put_str(composer, (U8*)"&amp;");
                break;
                
            case '\'':
                ret = bt_ftps_writer_put_str(composer, (U8*)"&apos;");
                break;
                
            case '\"':
                ret = bt_ftps_writer_put_str(composer, (U8*)"&quot;");
                break;

            default:
                ret = bt_ftps_writer_put_str(composer, utf8_buf);
                break;
            }            
        }
        else
        {
            ret = bt_ftps_writer_put_str(composer, utf8_buf);
        }

        if (ret)
        {
            return ret;
        }
        i += 2;
    }
    return ret;
}


/**
* @brief This function is to compose the folder attributes
* @return  The result of composing the folder attributes
**/
S32 bt_ftps_compose_folder_element(
            bt_ftp_file_writer_struct *composer,
            bt_ftp_obj_info_struct *file_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 ret = 0;
    S8 buf[100];
    
    /* not root folder and not parent folder */
    if ((ext_ucs2cmp((const U8*)file_info->file_name, (const U8*)L".") == 0) ||
        (ext_ucs2cmp((const U8*)file_info->file_name, (const U8*)L"..") == 0))
    {
        return 0;
    }

	if( 0 == ext_ucs2strlen((const S8*)file_info->file_name ) ){
		return ret;
	}

    ret = bt_ftps_writer_put_str(composer, (U8*)"    <folder name=\"");
    if (ret)
    {
        return ret;
    }

    ret = bt_ftps_compose_write_ucs2_name(composer, file_info->file_name);
    if (ret)
    {
        return ret;
    }
    
    sprintf(
        buf,
        "\" created=\"%04d%02d%02dT%02d%02d%02dZ\"/>\n",
        file_info->year,
        file_info->month,
        file_info->day,
        file_info->hour,
        file_info->min,
        file_info->sec);    
    ret = bt_ftps_writer_put_str(composer, (U8*)buf);
    
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftps_compose_file_element
 * DESCRIPTION
 *  This function is to compose the file attributes
 * PARAMETERS
 *  composer        [?]     [?]
 *  file_info       [?]     [?]
 *  a(?)            [IN/OUT]        First variable, used as returns(?)
 *  b(?)            [IN]            Second variable(?)
 * RETURNS
 *  The result of composing the file attributes
 *****************************************************************************/
S32 bt_ftps_compose_file_element(bt_ftp_file_writer_struct *composer, bt_ftp_obj_info_struct *file_info)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 ret = 0;
    S8 buf[100];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if( 0 == ext_ucs2cmp( file_info->file_name, (const U8 *)L".") || 0 == ext_ucs2cmp(file_info->file_name, (const U8 *)L"..") ){
        return ret;
    }
    
	if( 0 == ext_ucs2strlen((const S8*)file_info->file_name ) ){
		return ret;
	}
    
    ret = bt_ftps_writer_put_str(composer, (U8*)"    <file name=\"");
    if (ret)
    {
        return ret;
    }

    ret = bt_ftps_compose_write_ucs2_name(composer, file_info->file_name);
    if (ret)
    {
        return ret;
    }
    
    sprintf(
        buf,
        "\" modified=\"%04d%02d%02dT%02d%02d%02dZ\" size=\"%d\" user-perm=\"%s\"/>\n",
        file_info->year,
        file_info->month,
        file_info->day,
        file_info->hour,
        file_info->min,
        file_info->sec,
        (S32) file_info->file_size,
        ftps_cntx_p->access_right ? "R" : "RWD");
    ret = bt_ftps_writer_put_str(composer, (U8*)buf);
    
    return ret;
}



/*****************************************************************************
 * FUNCTION
 *  bt_ftps_writer_put_data
 * DESCRIPTION
 *  This function is to compose 
 * PARAMETERS
 *  composer        [?]     [?]
 *  data            [?]     [?]
 * RETURNS
 *  The result of composing the folder attributes
 *****************************************************************************/
static S32 bt_ftps_writer_put_data(bt_ftp_file_writer_struct *composer, U8* data, U32 len)
{
    S32 ret = 0;
	if( NULL == data){
		GOEP_ASSERT(0);
	}

    if (len > FTP_MAX_FILE_WRITER_BUF_SIZE)
    {
        return bt_ftps_writer_flush(composer, data);
    }
    else if (len + composer->buflen > FTP_MAX_FILE_WRITER_BUF_SIZE)
    {
        ret = bt_ftps_writer_flush(composer, NULL);
        GOEP_MEMCPY(composer->buf + composer->buflen, data, len);
        composer->buflen = len;
        return ret;
    }
    else
    {
        GOEP_MEMCPY(composer->buf + composer->buflen, data, len);
        composer->buflen += len;
    }
    return 0;
}

S32 bt_ftps_writer_flush(bt_ftp_file_writer_struct *composer, U8* data)
{
    S32 ret = 0;
	if( 0 == data ){
		//GOEP_ASSERT(0);
	}
    
    if (composer->buflen)
    {
        ret = btmtk_goep_write_wstream(composer->fd, composer->buf, composer->buflen, (S32 *) &(composer->written));
        if(ret < EXT_FS_OK)
        {
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_WRITER_FLUSH_BUFFER_FAIL, ret) );
            return ret;
        }
        
        GOEP_MEMSET(composer->buf, 0, FTP_MAX_FILE_WRITER_BUF_SIZE);
        composer->buflen = 0;
        composer->written = 0;
    }

    if (data)
    {
        ret = btmtk_goep_write_wstream(composer->fd, data, ext_strlen((const char*)data), (S32 *) &(composer->written));
        if(ret < EXT_FS_OK)
        {
            GOEP_TRACE( (BT_FTP_TRC_CLASS, BT_FTPS_WRITER_FLUSH_NEWDATA_FAIL, ret) );
            return ret;
        }
    }

    return ret;
}

/**
* @brief Write all data in composer to file handle
*/
FS_STATUS bt_ftps_flush_composer(bt_ftp_file_writer_struct *composer, FHANDLE fh){
    FS_STATUS status;
    U32 i4Total, i4Written;
    i4Total = 0;
    i4Written = 1; //init ok
    while( i4Written > 0 && i4Total < composer->buflen){
        status = btmtk_goep_write_wstream( fh, composer->buf + i4Total, composer->buflen, (S32 *) &i4Written);
        if( EXT_FS_OK != status || 0 == i4Written ) {
            composer->buflen = 0;
            return status;
        }
        i4Total += i4Written;
    }
    composer->buflen = 0;
    return EXT_FS_OK;
}

BT_BOOL bt_ftps_init_file_info(bt_ftp_obj_info_struct *info)
{
    if( 0 == info ){
        return FALSE;
    }
    info->name_len = FMGR_MAX_PATH_LEN; // depend on fs_file_info structure
    return TRUE;
}
