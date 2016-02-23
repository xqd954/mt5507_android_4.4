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
 * bt_goep_fs.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK bluetooth MMI OPP client/server
 *
 * Author:
 * -------
 * Daylong Chen
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

#if  (BTMTK_ON_WISESDK || BTMTK_ON_WIN32)
#define BTMTK_GOEP_USE_WIN32_FS 1
#define UNICODE
#include "windows.h"
#endif

#include "bt_types.h"
#include "bt_mmi.h"

#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"
#include "ext_osal.h"
#include "ext_nbuffer.h"

#include "bt_goep_fs.h"
#include "bt_goep_porting.h"
#include "bt_opp_comm.h"
#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_hdl.h"
#include "bt_opps_hdl.h"
#include "bt_goep_fs.h"
#include "ext_ucs2.h"

#ifdef BTMTK_GOEP_USE_WIN32_FS
#include "io.h" /// _findfile
#include "wchar.h"
#endif

#ifdef BTMTK_ON_WISE
#include "fs_type.h"
#include "fs_func.h"
#endif

#ifdef BTMTK_ON_LINUX
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <wchar.h>

#include "bt_ext_debug.h"

/* Variables */
static char g_folder_path[512];
static DIR* g_opened_dir;
#endif

/* Variable */
#ifdef BTMTK_GOEP_USE_WIN32_FS    
char g_oldcwd[512];
#endif

U16 g_opps_tmp_buff[OPPS_TMP_BUFF_SIZE];

/// Internal
/* 
 * OPP Server receives a file
 */
 U16 * bt_goep_make_tmpfile_prefix(U16 *name, U16 size, U16 *old_name, U16 num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    static U16 prefix[10];
    U16 l1, l2;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //TODO ascii to unicode
    sprintf( (S8 *)prefix, (const S8 *)"Dup(%d)", num);
    
    l1 = ext_ucs2strlen((const S8*)prefix);
    l2 = ext_ucs2strlen((const S8*)old_name);

    size = size -1;

    if (l1 + l2 > size)
    {
        return NULL;
    }

    ext_ucs2ncpy((S8 *)name, (S8 *)prefix, OPPS_TMP_BUFF_SIZE);
    ext_ucs2ncat((S8 *)name, (S8 *)old_name, OPPS_TMP_BUFF_SIZE);

    return name;
}


/*****************************************************************************
 * FUNCTION
 *  bt_goep_gen_folder_recursive
 * DESCRIPTION
 *  This function is to generate receive folder
 * PARAMETERS
 * fldr_path   [IN]
 * RETURNS
 *  S32
 *****************************************************************************/
static S32 bt_goep_gen_folder_recursive(U16 *fldr_path)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 retval = EXT_FS_OK;
    U16 *p1 = NULL, *p2 = NULL, t;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //Assert(fldr_path);

    for (p1 = (U16 *)ext_ucs2rchr( (const S8 *)fldr_path, (U16)L'\\'); p1; p1 = (U16 *)ext_ucs2rchr( (const S8 *)p2, (U16)L'\\'))
    {
        p2 = p1 + 1;
        t = *p2;
        *p2 = L'\0';
        
        retval = btmtk_goep_create_folder( (const U16*) fldr_path); 
        *p2 = t;
        if ( EXT_FS_OK != retval )
        {
            return retval;
        }
    }

    return retval;
}

/*****************************************************************************
 * FUNCTION
 *  bt_goep_make_folderpath
 * DESCRIPTION
 *  This function is to make full path of receive file
 * PARAMETERS
 * path     [OUT]
 * drive    [IN]
 * folder   [IN]
 * file     [IN]
 * RETURNS
 *  S32
 *****************************************************************************/
U16 * bt_goep_make_folderpath(U16 *path, U8 drive, U16 *folder, U16 *file)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 l1 = 0, l2 = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    BT_OPP_ASSERT(path);

    path[l1] = L'\0';
    
    if (drive)
    {
        sprintf( (S8*) path, (const S8 *) "%c:\\", drive);
        l1 = ext_ucs2strlen((const S8*)path);
        path[l1] = L'\0';
    }

    if (folder)
    {
        //TODO check path length
        ext_ucs2ncat((S8 *)path, (S8 *)folder, OPPS_TMP_BUFF_SIZE);

        l2 = ext_ucs2strlen((const S8*)folder);
        l1 += l2;
        if (folder[l2-1] != L'\\' && file)
        {
            path[l1] = L'\\';
            l1++;
        }

        path[l1] = L'\0';
    }

    if (file)
    {
        ext_ucs2ncat((S8 *)path, (S8 *)file, OPPS_TMP_BUFF_SIZE);
    }

    return path;
}


/*****************************************************************************
 * FUNCTION
 *  bt_goep_rename_temp_obj
 * DESCRIPTION
 *  This function is to rename temp object.
 * PARAMETERS:
 *  void    
 * RETURNS:
 *  S32
 *****************************************************************************/
static S32 bt_goep_rename_temp_obj(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 new_name[OPP_MAX_PATH_LENGTH+1] ;
    U16 *old_name = GOPP(recv_path);
    //U16 *tname = g_opps_tmp_buff;
    U16 tname[OPPS_TMP_BUFF_SIZE];
    S32 retval = EXT_FS_OK;
    U16 cnt = 0;
    S8 *file_name;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //new_name = kal_memalloc(sizeof(GOPP(recv_path)));
    
    /*check name whether include "/" */
    file_name = bt_goep_get_file_name_ext((S8 *)GOPP(ops_obj_name), OPP_FILE_GET_NAME_VALID);
    
    bt_goep_make_folderpath(new_name, 0, NULL, old_name);
    //bt_opps_update_path(new_name, 0, (U16 *)file_name);
    //TODO BT_OPP_LOG_PATH(old_name);
    //TODO BT_OPP_LOG_PATH(new_name);

    while (cnt++ < BT_OBEX_DUP_FILENAME_MAX_NUM)
    {
        retval = btmtk_goep_rename_file(old_name, new_name); //X: retval = FS_Rename(old_name, new_name);
        
        //TODO BT_OPP_LOG_PATH(new_name);
        //TODO BT_OPP_LOG1(BT_OPP_OPPS_RENAME_TEMP_OBJ,retval);
        
        if ( EXT_FS_OK != retval )
        {
            break;
        }
        if (bt_goep_make_tmpfile_prefix(
                    tname, 
                    OPPS_TMP_BUFF_SIZE,
                    (U16 *)file_name,
                     cnt))
        {
            //bt_opps_update_path(new_name, 0, tname);
        }
        else
        {
            break;
        }
    }

    if ( EXT_FS_OK != retval  )
    {
        btmtk_goep_delete_file(old_name); //X : FS_Delete(old_name);
    }
    else
    {
        ext_ucs2ncpy((S8 *)old_name, (S8 *)new_name, OPPS_TMP_BUFF_SIZE);
    }
    // kal_memfree(new_name);

    return retval;
}


/* 
 * FS operation functions
 */
/*****************************************************************************
 * FUNCTION
 *  bt_goep_clean_tempfile
 * DESCRIPTION
 *  This function is to clean unfinished receive file
 * PARAMETERS
 *  close_only       [IN]             
 * RETURNS
 *  S32   error code
 *****************************************************************************/
FS_STATUS bt_goep_clean_tempfile(FHANDLE fh ,U8 *FilePath, BT_BOOL close_only)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 retval;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_OPP_LOG2(BT_OPP_OPPS_CLEAN_TEMP_OBJ_1, close_only, GOPP(h_recv));

    retval = EXT_FS_OK;
    if( FHANDLE_INVALID_VALUE != fh ){
        btmtk_goep_close_rstream(fh);
        if( FALSE == close_only && 0 < ext_ucs2strlen((const S8*)FilePath)){
            btmtk_goep_delete_file( (const U16*) FilePath);
        }
    }
    
    //BT_OPP_LOG2(BT_OPP_OPPS_CLEAN_TEMP_OBJ, close_only, retval);
    return retval;
}

/*****************************************************************************
 * FUNCTION
 *  mmi_bt_goep_get_file_name_ext
 * DESCRIPTION
 *  This function is to abstract file name.
 * PARAMETERS
 *  name        [IN]        The input file path
 *  type        [IN]        Get file name or extension name
 * RETURNS
 *  void
 *****************************************************************************/
S8 * bt_goep_get_file_name_ext(S8 *name,U32  type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 len = (U16)ext_ucs2strlen((const S8*)name);
    U16 *tname = (U16*)name;
    U16 deter;
    BT_OPP_GET_FILE_ENUM eType = (BT_OPP_GET_FILE_ENUM) type;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (eType)
    {
        case OPP_FILE_GET_NAME:
            deter = '\\';
            break;

        case OPP_FILE_GET_EXT:
            deter = '.';
            break;
        case OPP_FILE_GET_NAME_VALID:
            deter = '/';
            break;    

        default:
            BT_OPP_ASSERT(0);
            return NULL;
    }

    while (len > 0)
    {
        len--;
        /* This is a Byte Order dependency algorithm
            need to guarantee name is Little Endian */
        if (tname[len] == deter)
        {
            break;
        }
    }

    if (len == 0)
    {
        return name;
    }

    return (S8*)(&tname[len+1]);
}


#ifdef BTMTK_ON_WISE
void static_convert_DirEntry_to_file_info( FS_DOSDirEntry *dir_entry, bt_ftp_obj_info_struct *ftp_file_info, U8 *filename){
	memset(ftp_file_info, 0, sizeof(bt_ftp_obj_info_struct) );
	ext_ucs2ncpy((S8 *) ftp_file_info->file_name, filename, (BTMTK_EP_MAX_FILENAME_LEN/2)-1 );

	ftp_file_info->year = dir_entry->DateTime.Year1980 + 1980;
	ftp_file_info->month = dir_entry->DateTime.Month;
	ftp_file_info->day = dir_entry->DateTime.Day;
	ftp_file_info->hour = dir_entry->DateTime.Hour;
	ftp_file_info->min = dir_entry->DateTime.Minute;
	ftp_file_info->sec = dir_entry->DateTime.Second2;
	ftp_file_info->file_size = dir_entry->FileSize;	
	
	if( dir_entry->Attributes & FS_ATTR_DIR ){
		ftp_file_info->isFile = FALSE;	
	}else{
		ftp_file_info->isFile = TRUE;
	}
}
#endif

#ifdef BTMTK_GOEP_USE_WIN32_FS
void static update_win32_file_time(bt_ftp_obj_info_struct *ftp_file_info){
	HANDLE hFile;
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;

    hFile = CreateFileW( (const U16 *)ftp_file_info->file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        //printf("CreateFile failed with %d\n", GetLastError());
        return;
    }
    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)){
        CloseHandle(hFile);
        return;
	}

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	ftp_file_info->year = stLocal.wYear;
	ftp_file_info->month = (U8) stLocal.wMonth;
	ftp_file_info->day = (U8) stLocal.wDay;
	ftp_file_info->hour = (U8) stLocal.wHour;
	ftp_file_info->min = (U8) stLocal.wMinute;
	ftp_file_info->sec = (U8) stLocal.wSecond;

	CloseHandle(hFile);
}    
#endif

#ifdef BTMTK_ON_LINUX
static S32 static_linux_fill_file_info(bt_ftp_obj_info_struct* ftp_file_info) {

    int ret;
    int err_num;
    time_t m_time;
    char file_path[512];
    struct dirent* dir_ent;
    struct stat* ent_stat = NULL;
    struct tm* time_struct;

    dir_ent = readdir(g_opened_dir);
    if (dir_ent == NULL) {
	bt_ext_log("[GOEP_FS] readdir() reaches EOF");
	goto fill_info_failed;
    }

    bt_ext_log("[GOEP_FS] entry name: %s", dir_ent->d_name);
    ret = snprintf(file_path, 512, "%s/%s", g_folder_path, dir_ent->d_name);
    if (ret < 0) {
	err_num = errno;
	bt_ext_err("[FS_ERR] sprintf() failed. %s", (char*) strerror(err_num));
	goto fill_info_failed;
    }

    /* Start to fill file info */
    ext_chset_utf8_to_ucs2_string((U8*) ftp_file_info->file_name,
   	    sizeof(ftp_file_info->file_name),
	    (const U8*) dir_ent->d_name);

    ent_stat = (struct stat*) malloc(sizeof(struct stat));
    if (ent_stat == NULL) {
	goto fill_info_failed;
    }

    ret = stat(file_path, ent_stat);
    if (ret == -1) {
	err_num = errno;
	bt_ext_err("[FS_ERR] stat() failed. %s", (char*) strerror(err_num));
	goto fill_info_failed;
    }

    m_time = ent_stat->st_mtime;
    time_struct = localtime( (const time_t *)&m_time);
    if (time_struct == NULL) {
	bt_ext_err("[FS_ERR] localtime() failed.");
	goto fill_info_failed;
    }
    
    ftp_file_info->isFile = S_ISDIR(ent_stat->st_mode)? FALSE : TRUE;
    ftp_file_info->file_size = ent_stat->st_size;
    ftp_file_info->year = time_struct->tm_year + 1900;
    ftp_file_info->month = time_struct->tm_mon + 1;
    ftp_file_info->day = time_struct->tm_mday;
    ftp_file_info->hour = time_struct->tm_hour;
    ftp_file_info->min = time_struct->tm_min;
    ftp_file_info->sec = time_struct->tm_sec;

    // bt_ext_log("[GOEP_FS] %s is a %s", ftp_file_info->file_name,
    //	    ftp_file_info->isFile ? "FILE" : "FOLDER");

    if (ent_stat != NULL) {
	free(ent_stat);
    }
    return EXT_FS_OK;

fill_info_failed:
    if (ent_stat != NULL) {
	free(ent_stat);
    }

    return EXT_FS_ERR;

}
#endif

S32 btmtk_goep_fs_findfirst(U8 *ucFolderPath, bt_ftp_find_struct **findstruct, bt_ftp_obj_info_struct *ftp_file_info){
#ifdef BTMTK_GOEP_USE_WIN32_FS    
    char sdir[256];
    char filter[64];
    U32 test_arrtibe;
    struct _wfinddata_t c_file;
    long hFile;
    bt_ftp_find_struct *pfind;

    GOEP_MEMSET((U8 *)ftp_file_info, 0, sizeof(bt_ftp_obj_info_struct) );
    
    ext_ucs2_str_n_to_asc_str(sdir, ucFolderPath, sizeof(sdir));

	// keep the previous folder path
	g_oldcwd[0] = 0;
	if ( NULL == _wgetcwd( (U16 *)g_oldcwd, sizeof(g_oldcwd)/2 ) ){
		/// cannot keep the current folder
		GOEP_Report("[FS][ERR] fail to get cwd bufsize:(%d) err:%d!", sizeof(g_oldcwd)/2, GetLastError());
		return EXT_FS_ERR;
	}
    
    printf( "[fs] getcwd is '%s'\n", g_oldcwd);
    printf( "[fs] fs findfirst '%s' \n", sdir );
    _wchdir( (U16 *) ucFolderPath );
    
     
    ext_strncpy (filter, "*.*", 64);
    hFile = _wfindfirst(L"*.*", &c_file);
    
    if( -1 == hFile ){
        return EXT_FS_ERR;
    }else{
        //plong = (long * )malloc(sizeof(long));
        pfind = (bt_ftp_find_struct *) malloc( sizeof(bt_ftp_find_struct) );
        *findstruct = pfind;

        if( *findstruct == NULL ){
            return EXT_FS_ERR;
        }
        ((bt_ftp_find_struct *)*findstruct)->hFile = hFile;

		ext_ucs2ncpy( (S8 *) ftp_file_info->file_name, (const S8 *) c_file.name, sizeof(ftp_file_info->file_name)/2);
		update_win32_file_time(ftp_file_info);
        test_arrtibe = (_A_SUBDIR & c_file.attrib);
        if( _A_SUBDIR == test_arrtibe){
            ftp_file_info->isFile = FALSE;
        }else{
            ftp_file_info->isFile = TRUE;
        }
        return EXT_FS_OK;
    }
#endif

#ifdef BTMTK_ON_WISE
    U8 *filter;
    S32 hFile;
    S32 len;
    bt_ftp_find_struct *pfind;
    FS_DOSDirEntry dir_entry;
    U8 filename[ BTMTK_EP_MAX_FILENAME_LEN ]; /// output
     
    //ext_ucs2ncpy (filter, (const U8 *)L"\\*.*", sizeof(filter)/2);
    len = ext_ucs2strlen((const S8*)ucFolderPath)*2 + 10;
    filter = (U8 *) get_ctrl_buffer( len ); // plus L"\\*.*"
    if( NULL == filter ){
    	return EXT_FS_ERR;
    }
    btmtk_os_memset( filter, 0, len);
    ext_ucs2ncpy( (U8 *) filter,  (const U8 *) ucFolderPath , len-2);
    ext_ucs2ncat( (U8 *) filter, (const U8 *) L"\\*.*", len-2);
    
    ext_ucs2ncpy((S8 *)filename, (const U8 *)L"", (sizeof(filename)/2)-1);
        
	hFile = (S32) FS_FindFirst(
                    (const WCHAR*)filter,
                    0,
                    0,
                    &dir_entry,
                    (U16*) filename,
                    sizeof(filename) );
    if( NULL == filter ){
    	free_ctrl_buffer(filter);
    	filter = NULL;
    }
    if ( hFile >= 0){
    	GOEP_MEMSET((U8 *)ftp_file_info, 0, sizeof(bt_ftp_obj_info_struct) );
    	pfind = (bt_ftp_find_struct *) get_ctrl_buffer( sizeof(bt_ftp_find_struct) );
        *findstruct = pfind;
        if( *findstruct == NULL ){
        	FS_FindClose( (FS_HANDLE) hFile );
            return EXT_FS_ERR;
        }
        ((bt_ftp_find_struct *)*findstruct)->hFile = hFile;
        
        static_convert_DirEntry_to_file_info( &dir_entry, ftp_file_info , filename);
        
		return EXT_FS_OK; 	
    }else{
    	return EXT_FS_ERR;
    }
    
#endif    

#ifdef BTMTK_ON_LINUX

    int ret;
    int err_num;

    bt_ext_log("[GOEP_FS] btmtk_goep_fs_findfirst(): Linux Version.");

    ext_chset_ucs2_to_utf8_string((U8 *) g_folder_path, sizeof(g_folder_path)-1, ucFolderPath);
    // strncpy(g_folder_path, ucFolderPath, sizeof(g_folder_path));
    bt_ext_log("[GOEP_FS] g_folder_path: %s", g_folder_path);
    
    g_opened_dir = opendir( g_folder_path);
    if (g_opened_dir == NULL) {
	err_num = errno;
	bt_ext_err("[FS_ERR] opendir() failed. %s", (char*) strerror(err_num));
	return EXT_FS_ERR;
    }

    return static_linux_fill_file_info(ftp_file_info);

#endif

    return EXT_FS_ERR;
}

S32 btmtk_goep_fs_findnext(void *ptr , bt_ftp_obj_info_struct *ftp_file_info){
#ifdef BTMTK_GOEP_USE_WIN32_FS    
    long *plong; 
    U32 test_arrtibe;
    struct _wfinddata_t c_file;
    
    if( NULL == ptr || NULL == ftp_file_info){
        return EXT_FS_ERR;
    }
    
    plong = ptr;
    GOEP_MEMSET( (U8 *) ftp_file_info, 0, sizeof(bt_ftp_obj_info_struct) );
    if( 0 == _wfindnext( (long)*plong, &c_file ) ){
        //ext_asc_str_n_to_ucs2_str( ftp_file_info->file_name, c_file.name, 260/2);
		ext_ucs2ncpy( (S8 *)ftp_file_info->file_name, (const S8 *)c_file.name, sizeof(ftp_file_info->file_name)/2 );
		update_win32_file_time(ftp_file_info);
        // ok. find the next
        test_arrtibe = (_A_SUBDIR & c_file.attrib);
        ftp_file_info->file_size = c_file.size;

        if( _A_SUBDIR == test_arrtibe  ){
            ftp_file_info->isFile = FALSE;
        }else{
            ftp_file_info->isFile = TRUE;
        }
        
        return EXT_FS_OK;
    }
#endif    

#ifdef BTMTK_ON_WISE
	FS_DOSDirEntry dir_entry;
	bt_ftp_find_struct *findstruct;
	U8 filename[ BTMTK_EP_MAX_FILENAME_LEN ];
	U8 ucfilename[ BTMTK_EP_MAX_FILENAME_LEN ];
	
	if( NULL != ptr ){
		findstruct = (bt_ftp_find_struct *) ptr;
		if( 0 <= FS_FindNext( (FS_HANDLE) findstruct->hFile , &dir_entry, (U16 *) filename, sizeof(filename) ) ){
			static_convert_DirEntry_to_file_info( &dir_entry, ftp_file_info , filename);
			
			memset( ucfilename, 0, sizeof( ucfilename) );
			ext_chset_ucs2_to_utf8_string( (U8 *)ucfilename, sizeof(ucfilename) - 1, (U8 *)filename );
			GOEP_Report("[ftp][fs] findnext file:(%s)", ucfilename);
			return EXT_FS_OK;
		}
	}
#endif

#ifdef BTMTK_ON_LINUX

    return static_linux_fill_file_info(ftp_file_info);

#endif

    return EXT_FS_ERR;
}

S32 btmtk_goep_fs_findend(void *ptr){
    bt_ftp_find_struct *pfind;
    pfind = (bt_ftp_find_struct *) ptr;

#if defined(BTMTK_ON_WISE)
    if( NULL != pfind ){
	FS_FindClose( (FS_HANDLE) pfind->hFile);
	pfind->hFile = 0;
	free_ctrl_buffer( pfind );
    }
#endif
    
#ifdef BTMTK_GOEP_USE_WIN32_FS
	_findclose(pfind->hFile);
	_wchdir( (const U16 *) g_oldcwd );
        if( NULL !=  ptr )
		free( ptr );
#endif

#ifdef BTMTK_ON_LINUX
    closedir(g_opened_dir);
#endif

    return EXT_FS_OK;
}

S32 btmtk_goep_get_fileinfo( bt_ftp_obj_info_struct *info){
    return EXT_FS_OK;
}

#ifdef BTMTK_GOEP_USE_WIN32_FS
// utf8 string
extern U8 g_bt_dir_buf[];
#endif 

/*
* @param ucFilename ucs2 filename
* @u4MaxLen bytes
*/
BT_BOOL btmtk_goep_get_private_filepath(U8 type, const U8 *ucFilename, U8 *ucOutFilepath, U32 u4MaxLen){
	BT_BOOL l_bret = FALSE;

#if defined(BTMTK_ON_WISESDK)
    //SDK
	U8 tempPath[512];
	U8 l_u2RootPath[512];
	
	btmtk_os_memset( l_u2RootPath, 0, sizeof(l_u2RootPath));
	ext_chset_utf8_to_ucs2_string( l_u2RootPath, sizeof(l_u2RootPath)-2, g_bt_dir_buf);
	if( FALSE == btmtk_goep_is_folder_exist( (const U16*) l_u2RootPath) ){
		GOEP_Report("[GOEP] btmtk_goep_get_private_filepath no root folder");
		return FALSE;
	}
	
	/// <sdk root path>\\xxx\\@btmtk\\ folder
	ext_ucs2ncpy( (S8*)tempPath, l_u2RootPath, sizeof(tempPath)/2);
	ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\Component\\MBT\\HWAdapt\\MTK\\LIB\\@btmtk", sizeof(tempPath)/2);
	if( 0 != ext_ucs2strlen((const S8*)tempPath) && FALSE == btmtk_goep_is_folder_exist((const U16*)tempPath) ){
		btmtk_goep_create_folder((const U16*)tempPath);
	}
	
	switch(type){
		case 0:  /// <sdk root path>\\xxx\\@btmtk\\ .
			ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\", sizeof(tempPath)/2);
			ext_ucs2ncat( (S8*)tempPath, ucFilename, sizeof(tempPath)/2);
			ext_ucs2ncpy((S8 *) ucOutFilepath, tempPath, u4MaxLen );
			if( ext_ucs2strlen((const S8*)ucOutFilepath) == ext_ucs2strlen((const S8*)tempPath) ){
				l_bret = TRUE;
			}

			break;
		case 1:  /// <sdk root path>\\xxx\\@btmtk\\profile
			ext_ucs2ncpy( (S8*)tempPath, l_u2RootPath, sizeof(tempPath)/2);
			ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\Component\\MBT\\HWAdapt\\MTK\\LIB\\@btmtk\\profile", sizeof(tempPath)/2);
			if( FALSE == btmtk_goep_is_folder_exist((const U16*)tempPath) ){
				btmtk_goep_create_folder((const U16*)tempPath);
			}
			ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\", sizeof(tempPath)/2);
			ext_ucs2ncat( (S8*)tempPath, ucFilename, sizeof(tempPath)/2);
			ext_ucs2ncpy((S8 *) ucOutFilepath, tempPath, u4MaxLen );
			if( ext_ucs2strlen((const S8*)ucOutFilepath) == ext_ucs2strlen((const S8*)tempPath) ){
				l_bret = TRUE;
			}

			break;
		case 2:  /// <sdk root path>\\xxx\\@btmtk\\data
			ext_ucs2ncpy( (S8*)tempPath, l_u2RootPath, sizeof(tempPath)/2);
			ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\Component\\MBT\\HWAdapt\\MTK\\LIB\\@btmtk\\data", sizeof(tempPath)/2);
			if( FALSE == btmtk_goep_is_folder_exist((const U16*)tempPath) ){
				btmtk_goep_create_folder((const U16*)tempPath);
			}
			ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\", sizeof(tempPath)/2);
			ext_ucs2ncat( (S8*)tempPath, ucFilename, sizeof(tempPath)/2);
			ext_ucs2ncpy((S8 *) ucOutFilepath, tempPath, u4MaxLen );
			if( ext_ucs2strlen((const S8*)ucOutFilepath) == ext_ucs2strlen((const S8*)tempPath) ){
				l_bret = TRUE;
			}
			break;
		default:
			GOEP_Report("[OBEX][Err] btmtk_goep_get_private_filepath fail param:%d");
		break;
	}

#else
	// Target
	U8 tempPath[512];
	
	switch(type){
		case 0: /// Z:\\@btmtk
			ext_ucs2ncpy( (S8 *) tempPath, (const S8*)L"Z:\\@btmtk", sizeof( tempPath ) );
			break;
		case 1: /// C:\\@btmtk\\profiles
			ext_ucs2ncpy( (S8 *) tempPath, (const S8*)L"C:\\@btmtk\\profiles", sizeof( tempPath ) );
			break;
		case 2: /// C:\\@btmtk\\data
			ext_ucs2ncpy( (S8 *) tempPath, (const S8*)L"C:\\@btmtk\\data", sizeof( tempPath ) );
			break;
		default:
		break;
	}

	if( 0 != ext_ucs2strlen((const S8*)tempPath) ){
		if (FALSE == btmtk_goep_is_folder_exist( (const U16*) tempPath) ){
			btmtk_goep_create_folder((const U16*)tempPath);
		}
		ext_ucs2ncat( (S8*)tempPath, (const S8*)L"\\", sizeof(tempPath)/2);
		ext_ucs2ncat( (S8*)tempPath, (const S8*)ucFilename, sizeof(tempPath)/2);

		ext_ucs2ncpy( (S8 *)ucOutFilepath, (const S8*)tempPath, u4MaxLen);
		if( ext_ucs2strlen((const S8*)ucOutFilepath) == ext_ucs2strlen((const S8*)tempPath) ){
			l_bret = TRUE;
		}
	}
 
#endif
	return l_bret;
}
