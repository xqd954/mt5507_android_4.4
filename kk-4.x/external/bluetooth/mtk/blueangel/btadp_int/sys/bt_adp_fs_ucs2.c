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
 * Bt_adp_fs_ucs2.c
 *
 * Project:
 * --------
 *
 *
 * Description:
 * ------------
 *   This file is used to read/write file as filestream.
 *
 * Author:
 * -------
 * Daylong
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
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#define USE_WIN32_FS 1
#endif

/* Porting to USE_WIN32_FS */
#ifdef USE_WIN32_FS
#define _WIN32_WINNT 0x0500
#define UNICODE
#include "windows.h"
#endif

#ifdef BTMTK_ON_WISE
#include "fs_type.h"
#include "fs_errcode.h"
#include "fs_func.h"
#endif

#ifdef BTMTK_ON_LINUX
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "osapi.h"
#include "bt_os_api.h"     // for mem
#include "stdio.h"
#include "bt_ucs2.h"

#include "bt_adp_fs.h"
#include "bt_adp_fs_ucs2.h"

/* Change by External Platform Setting */
#define BTMTK_FS_MAX_FILEPATH 640

#ifdef USE_WIN32_FS
static S32 translateWinFileError(S32 err)
{
    S32 ret_err = BTMTK_FS_ERR;

    switch (err)
    {
    case ERROR_INVALID_NAME:
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_BAD_PATHNAME:
        ret_err = BTMTK_FS_ERR_PATH;
        break;

    case ERROR_SHARING_VIOLATION:
    case ERROR_ACCESS_DENIED:
        ret_err = BTMTK_FS_ERR_ACCESS;
        break;
    case ERROR_HANDLE_EOF:
        ret_err = BTMTK_FS_ERR_EOF;
        break;
    default:
        ret_err = BTMTK_FS_ERR;
        break;
    }

    return ret_err;
}

static S32 translateWinSeekMode(btmtk_seek_mode_enum mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (mode)
    {
    case BTMTK_FS_SEEK_BEGIN:
        return FILE_BEGIN;
    case BTMTK_FS_SEEK_CURRENT:
        return FILE_CURRENT;
    case BTMTK_FS_SEEK_END:
        return FILE_END;
    }
    return FILE_BEGIN;
}
#endif

/* 1 : exist, 0 : not exist */
static U8 IsFileExist(const char *path){
#ifdef BTMTK_ON_LINUX
    struct stat file_stat;
    if ( stat(path, &file_stat) == 0 )
        return 1;
#endif
    return 0;
}

#ifdef BTMTK_ON_LINUX
static int getFilePermission(const char *path, mode_t *mode){
    int ret;
    struct stat file_stat;
    ret = stat(path, &file_stat);
    if ( ret == 0 )
        *mode = file_stat.st_mode;
    else
        OS_Report("[ERR] getFilePermission(%s) failed : errno=%u, %s", path, errno, strerror(errno));
    return ret;
}
#endif

FHANDLE btmtk_fs_open_ucs2(const U16 *FileName, btmtk_file_mode_enum Flag)
{
	S32 fh;
#if !defined(BTMTK_ON_WISE) && !defined(BTMTK_ON_LINUX)
    DWORD access, create;
#endif
    U8 fileExist = 0;

    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    bt_chset_ucs2_to_utf8_string(buffer, BTMTK_FS_MAX_FILEPATH, (const U8*)FileName);

    OS_Report("[fs]btmtk_fs_open_ucs2 %s", buffer);
#ifdef BTMTK_ON_WISE
    if((fh = FS_Open( FileName, translateFileMode(Flag))) < FS_NO_ERROR)
    {
    	OS_Report("[fs] open rstream fail err:(%d)", fh);
        return translateFileError(fh);
    }
#elif defined(BTMTK_ON_LINUX)
    fileExist = IsFileExist((const char*)buffer);
    /* set CreateFile() access mode */
    OS_Report("btmtk_fs_open_ucs2: flag[%d], fileExist=%u", Flag, fileExist);
    if (Flag & BTMTK_FS_READ_ONLY){
    }else{
    	/* FS_READ_WRITE is the default setting */
    	Flag |= BTMTK_FS_READ_WRITE;
    }

    if((fh = open((const char*)buffer, translateFileMode(Flag), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1){
        OS_Report("[FS][ERR] open_stream err [%s][%d]", strerror(errno), errno);
        return translateFileError(errno);
    }
    if(!fileExist){
        int ret;
        OS_Report("Change file permission");
        ret = fchmod(fh, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(ret < 0)
            OS_Report("Change file permission failed : errno=%u, %s", errno, strerror(errno));
    }
#else

    /* set CreateFile() access mode */
    if (Flag & BTMTK_FS_READ_ONLY)
    {
        access = GENERIC_READ;
    }
    else
    {
        /* FS_READ_WRITE is the default setting */
        access = GENERIC_READ | GENERIC_WRITE;
    }

    /* set CreateFile() create policy */
    if (Flag & BTMTK_FS_CREATE_ALWAYS)
    {
        create = CREATE_ALWAYS;
    }
    else
    {
        create = OPEN_EXISTING;
    }

    if((fh = CreateFile(FileName, access, FILE_SHARE_READ, NULL, create, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE )
    {
		OS_Report("[FS][ERR] open_rstream err:%d", GetLastError());
        return translateWinFileError(GetLastError());
    }
#endif
    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_open_ucs2, FileHandle=%d", fh);
    return (FHANDLE) fh;
}

S32 btmtk_fs_read_ucs2(FHANDLE FileHandle, void *DataPtr, S32 Length, S32 *Read)
{
#ifdef BTMTK_ON_WISE
    S32 ret;
    ret = FS_Read(FileHandle, DataPtr, Length, Read);
    if( ret < FS_NO_ERROR )
    {
        return translateFileError(ret);
    }
#elif defined(BTMTK_ON_LINUX)
    *Read = read((int)FileHandle, DataPtr, Length);
    if( *Read == -1 )
    {
    	return BTMTK_FS_ERR;
    }
#else
    if(ReadFile((HANDLE)FileHandle, DataPtr, Length, Read, NULL) == FALSE)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif

    return BTMTK_FS_OK;
}

S32 btmtk_fs_write_ucs2(FHANDLE FileHandle, void *DataPtr, S32 Length, S32 *Written)
{
#ifdef BTMTK_ON_WISE
    S32 ret;
    if((ret = FS_Write(FileHandle, DataPtr, Length, Written)) != FS_NO_ERROR)
    {
        return translateFileError(ret);
    }
#elif defined(BTMTK_ON_LINUX)
    *Written = write((int)FileHandle, DataPtr, Length);
    if(*Written == -1)
    {
        return translateFileError((S32)errno);
    }
#else
    if(WriteFile((HANDLE) FileHandle, DataPtr, Length, Written, NULL) == FALSE)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif
    return BTMTK_FS_OK;
}

S32 btmtk_fs_close_ucs2(FHANDLE FileHandle)
{
    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_close_ucs2, FileHandle=%d", FileHandle);
#ifdef BTMTK_ON_WISE
    S32 ret;
    if((ret = FS_Close(FileHandle)) != FS_NO_ERROR)
    {
        return translateFileError(ret);
    }
#elif defined(BTMTK_ON_LINUX)
    if ( 0 == FileHandle )
    {
        OS_Report("[FS][WRN] file handle is 0");
    }
    else 
    {
        if (close(FileHandle) == -1)
        {
            return translateFileError((S32)errno);
        }
    }  
#else
    if (CloseHandle((HANDLE) FileHandle) == FALSE)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif
    return BTMTK_FS_OK;
}

S32 btmtk_fs_seek_ucs2(FHANDLE FileHandle, S32 offset, btmtk_seek_mode_enum seek_mode)
{
    S32 pos;
#ifdef BTMTK_ON_WISE
    if ((pos = FS_Seek((FS_HANDLE) FileHandle, offset, translateSeekMode(seek_mode))) != FS_NO_ERROR)
    {
        return translateFileError(pos);
    }
#elif defined(BTMTK_ON_LINUX)
	if((pos = lseek((int) FileHandle, offset, translateSeekMode(seek_mode))) == -1)
    {
        return translateFileError((S32)errno);
    }
#else
	if((pos = SetFilePointer( (HANDLE) FileHandle, offset,0, translateWinSeekMode(seek_mode))) < 0)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif
	return pos;
}

S32 btmtk_fs_create_dir_ucs2(const U16 *dirName)
{
	S32 fh;
	S32 ret;
#ifdef BTMTK_ON_WISE
    
    if((ret = FS_CreateDir(dirName)) < FS_NO_ERROR)
    {
        OS_Report("[goep][fs] create_folder ret:(%d)", ret);
        return translateFileError(ret);
    }
#elif defined(BTMTK_ON_LINUX)
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    bt_chset_ucs2_to_utf8_string(buffer, BTMTK_FS_MAX_FILEPATH, (const U8*)dirName);
    OS_Report("[fs]btmtk_fs_create_dir_ucs2 %s", buffer);
    if(mkdir((const char*)buffer, S_IRWXU | S_IRWXG/* | S_IRWXO*/) != 0)
    {
		OS_Report("[FS][ERR] create_folder err:%d", errno);
        return translateFileError((S32)errno);
    }
	if((fh = open((const char*)buffer, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
   	{
   		OS_Report("[FS][ERR] open_stream err [%s][%d]", strerror(errno), errno);
   		return translateFileError(errno);
   	}
	OS_Report("Change file permission");
        ret = fchmod(fh, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
        if(ret < 0)
            OS_Report("Change file permission failed : errno=%u, %s", errno, strerror(errno));
    	close(fh);

#else
    if(CreateDirectory(dirName, NULL) == 0)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif
    return BTMTK_FS_OK;
}

S32 btmtk_fs_delete_ucs2(const U16 *fileName)
{
#ifdef BTMTK_ON_WISE
    S32 ret;
    if( FALSE != btmtk_fs_is_file_exist_ucs2( fileName ) )
    {
        if((ret = FS_Delete(fileName)) < FS_NO_ERROR)
        {
            OS_Report("[fs] del file fail err:(%d)", ret);
            return translateFileError(ret);
        }
    }
#elif defined(BTMTK_ON_LINUX)
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    bt_chset_ucs2_to_utf8_string(buffer, BTMTK_FS_MAX_FILEPATH, (const U8*)fileName);
    if(remove((const char*)buffer) == -1)
    {
        OS_Report("[FS][ERR] delete_file err:%d", errno);
        return translateFileError((S32)errno);
    }
#else
    if(DeleteFile(fileName) == FALSE)
    {
        OS_Report("[FS][ERR] delete_file err:%d", GetLastError());
        return translateWinFileError((S32)GetLastError());
    }
#endif
    return BTMTK_FS_OK;
}

S32 btmtk_fs_get_current_dir_ucs2(U32 buf_len, U16* path)
{
#ifdef BTMTK_ON_WISE
    /* N/A */
#elif defined(BTMTK_ON_LINUX)
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    bt_asc_str_n_to_ucs2_str((S8 *)buffer, (S8 *)path, BTMTK_FS_MAX_FILEPATH);
    if(getcwd((char*)buffer, buf_len) == NULL)
    {
        return translateFileError(errno);
    }
#else
    if (GetCurrentDirectory(buf_len, path) == 0)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif
    return BTMTK_FS_OK;
}


S32 btmtk_fs_get_filesize_ucs2(FHANDLE FileHandle, U32 *Size)
{
#ifdef BTMTK_ON_WISE
    S32 ret;
    if ((ret = FS_GetFileSize(FileHandle, Size)) != FS_NO_ERROR)
    {
        return translateFileError(ret);
    }
#elif defined(BTMTK_ON_LINUX)
    struct stat buf;
    if(fstat(FileHandle, &buf) != 0)
    {
        return translateFileError((S32)errno);
    }
    *Size = buf.st_size;
#else
    DWORD size_high;

    *Size = GetFileSize((HANDLE)FileHandle, &size_high);
    if (*Size == INVALID_FILE_SIZE)
    {
        return translateWinFileError((S32)GetLastError());
    }
#endif
    return BTMTK_FS_OK;
}

S32 btmtk_fs_move_file_ucs2(const U16* SrcFullPath, const U16* DstFullPath, btmtk_move_mode_enum Flag)
{
#ifdef BTMTK_ON_WISE
	S32 ret = 0;
	if ((ret = FS_Move(SrcFullPath, DstFullPath, translateMoveMode(Flag), NULL, NULL, 0)) < FS_NO_ERROR)
    {
        return translateFileError(ret);
    }
#elif defined(BTMTK_ON_LINUX)
    S32 l_ret = 0;
    struct stat buf;
    U8 src[BTMTK_FS_MAX_FILEPATH];
    U8 dst[BTMTK_FS_MAX_FILEPATH];

    bt_ucs2_str_n_to_asc_str((S8 *)src, (S8 *)SrcFullPath, BTMTK_FS_MAX_FILEPATH);
    bt_ucs2_str_n_to_asc_str((S8 *)dst, (S8 *)DstFullPath, BTMTK_FS_MAX_FILEPATH);

    if(stat((const char*)src,&buf) == 0)
    {
        if(stat((const char*)dst,&buf) == 0)
        {
            if(Flag & BTMTK_FS_MOVE_OVERWRITE)
            {
                l_ret =  remove((const char*)dst);
                if ( l_ret != 0 )
                {
                    l_ret = errno;
                    OS_Report("[FS][ERR] move err, remove");
                }
            }
        }
	else
	{
		OS_Report("[FS][ERR] stat dst err [%s][%d]", strerror(errno), errno);
	}
//20101004 mtk03036
//rename(2) does not work across different mount points, even if the same filesystem is mounted on both.
/*
        if (Flag & BTMTK_FS_MOVE_KILL)
        {
            l_ret = rename(src,dst);
            if ( l_ret != 0 )
            {
                l_ret = errno;
                OS_Report("[FS][ERR] move err, rename");
            }
        }
        else
*/
//20101004 mtk03036 end
        {
            int fd_src, fd_dst;
            char buf[100], *ptr;
            int num_read, num_write;
            mode_t mode = S_IRUSR | S_IWUSR;
            //if( (fd_src = open((const char*)src,O_RDONLY)) != -1 )
		if((fd_src = btmtk_fs_open_ucs2((WCHAR*)SrcFullPath, BTMTK_FS_READ_WRITE)) >= 0)
            {
                if(getFilePermission((const char*)src, &mode) < 0){
                    mode = S_IRUSR | S_IWUSR;
                }
                //mtk03036 20101004
                //fd_dst = open(dst,O_WRONLY);
                //fd_dst = open((const char*)dst,O_WRONLY|O_CREAT);
			fd_dst = btmtk_fs_open_ucs2((WCHAR*)DstFullPath, (BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE_ALWAYS));
                //mtk03036 20101004 end

                if(fd_dst >= 0)
                {
                    if( fchmod(fd_dst, mode) < 0 )
                        OS_Report("[ERR] fchmod(%u) failed : errno=%u, %s", mode, errno, strerror(errno));
                    while((num_read = read(fd_src, buf, 100)) > 0)
                    {
                        ptr = buf;
                        while(num_read)
                        {
                            num_write = write(fd_dst, ptr, num_read);
                            if(num_write != -1)
                            {
                                num_read -= num_write;
                                ptr += num_write;
                            }
                            else
                            {
                                l_ret = errno;
					OS_Report("[FS][ERR] write err [%s][%d]", strerror(errno), errno);
                                btmtk_fs_close_ucs2(fd_src);
                                btmtk_fs_close_ucs2(fd_dst);
                                remove((const char*)dst);
                                return translateFileError((S32)errno);
                            }
                        }
                    }
			if(num_read == -1)
			{
				l_ret = errno;
				OS_Report("[FS][ERR] read err [%s][%d]", strerror(errno), errno);
				return translateFileError((S32)errno);
			}
			btmtk_fs_close_ucs2(fd_src);
			btmtk_fs_close_ucs2(fd_dst);
                }
                else
                {
                    l_ret = errno;
			 OS_Report("[FS][ERR] open dst err [%s][%d]", strerror(errno), errno);
                    btmtk_fs_close_ucs2(fd_src);
                }
            }
		else
		{
			OS_Report("[FS][ERR] open src err [%s][%d]", strerror(errno), errno);
		}
        }
    }
	else
	{
		OS_Report("[FS][ERR] stat src err [%s][%d]", strerror(errno), errno);
	}
    if(l_ret != 0)
        return translateFileError((S32)l_ret);
#else
    DWORD attributes, error;

    if (Flag & BTMTK_FS_MOVE_KILL)
    {
        /* move operation */
        if (MoveFileWithProgress(DstFullPath, DstFullPath, NULL, NULL, MOVEFILE_REPLACE_EXISTING) == 0)
        {
            error = GetLastError();
        }
        else
        {
            error = ERROR_SUCCESS;
        }
    }
    else
    {
        /* copy operation */
        attributes = GetFileAttributes(SrcFullPath);
        if (attributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            /* do not support folder move */
            return BTMTK_FS_ERR;

            /* copy a directory */
            /* CopyFileEx cannot copy a directory */
            /* do all stuffs by myself */
            /* create the directory in the destination */
            if (!CreateDirectory(DstFullPath, NULL))
            {
                if (GetLastError() != ERROR_ALREADY_EXISTS) {

                }
            }
        }
        else
        {
            /* copy a file */
            /* invoke CopyFileEx directly */
            if (CopyFileEx(SrcFullPath, DstFullPath, NULL, NULL, NULL, 0) == 0)
            {
                error = GetLastError();
            }
            else
            {
                error = ERROR_SUCCESS;
            }
        }
    }

    if (error != ERROR_SUCCESS)
        return translateWinFileError((S32)error);

#endif
    return BTMTK_FS_OK;
}

BT_BOOL btmtk_fs_is_valid_handle_ucs2(FHANDLE hFile){
#ifdef USE_WIN32_FS
    if( hFile == FHANDLE_INVALID_VALUE ){
        return FALSE;
    }
    if( hFile == 0 ){
		return FALSE;
    }
    return TRUE;
#endif

#ifdef BTMTK_ON_WISE
    if( hFile == FHANDLE_INVALID_VALUE ){
        return FALSE;;
    }
    if( hFile <= 0 ){
    	return FALSE;
    }
    return TRUE;
#endif

#ifdef BTMTK_ON_LINUX
    struct stat buf;

    if (hFile == FHANDLE_INVALID_VALUE || hFile < 0) {
        return FALSE;;
    }

    return (fstat(hFile, &buf) == 0);
#endif
}


FS_STATUS btmtk_fs_rename_file_ucs2(const U16 *pwsOldFilename,const U16 *pwsNewFilename){

#ifdef USE_WIN32_FS
    U8 buffer1[BTMTK_FS_MAX_FILEPATH];
	U8 buffer2[BTMTK_FS_MAX_FILEPATH];
    buffer1[0] = 0;
	buffer2[0] = 0;
    bt_ucs2_str_n_to_asc_str( (S8 *)buffer1, (S8 *)pwsOldFilename, BTMTK_FS_MAX_FILEPATH);
	bt_ucs2_str_n_to_asc_str( (S8 *)buffer2, (S8 *)pwsNewFilename, BTMTK_FS_MAX_FILEPATH);
	if( NULL == pwsOldFilename ||  NULL == pwsNewFilename || bt_ucs2strlen( (const U8*) pwsOldFilename) ==0 || bt_ucs2strlen( (const U8*)pwsNewFilename) == 0){
		OS_Report("[FS][ERR] rename_file param error");
		return BTMTK_FS_ERR;
	}
	if( FALSE != MoveFileA(buffer1, buffer2) ){
	    printf("[fs] rename '%s' to '%s' ok\n", buffer1, buffer2);
		OS_Report("[FS][ERR] create_folder err:%d", GetLastError());
	    return BTMTK_FS_OK;
	}
	printf("[fs] rename '%s' to '%s' fail err=%d\n", buffer1, buffer2, GetLastError());
	return GetLastError();
#endif

#ifdef BTMTK_ON_WISE
		S32 l_ret;
		l_ret = FS_Rename(pwsOldFilename, pwsNewFilename);
		if( l_ret < FS_NO_ERROR  ){
			return l_ret;
		}
    return BTMTK_FS_OK;
#endif

#ifdef BTMTK_ON_LINUX
		S32 l_ret;
            U8 buffer1[BTMTK_FS_MAX_FILEPATH];
            U8 buffer2[BTMTK_FS_MAX_FILEPATH];
            bt_ucs2_str_n_to_asc_str( (S8 *)buffer1, (S8 *)pwsOldFilename, BTMTK_FS_MAX_FILEPATH);
            bt_ucs2_str_n_to_asc_str( (S8 *)buffer2, (S8 *)pwsNewFilename, BTMTK_FS_MAX_FILEPATH);
            l_ret = rename((const char*)buffer1, (const char*)buffer2);
		if( l_ret != 0  ){
			return l_ret;
		}
    return BTMTK_FS_OK;
#endif
}

FS_STATUS btmtk_fs_delete_dir_ucs2(const U16 *pwsFilename){
#ifdef USE_WIN32_FS
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    buffer[0] = 0;
    bt_ucs2_str_n_to_asc_str( (S8 *)buffer, (S8 *)pwsFilename, BTMTK_FS_MAX_FILEPATH);


	if( FALSE == RemoveDirectoryW( pwsFilename ) ){
		printf("[fs] delete folder '%s' fail=%d\n", buffer, GetLastError());
		return GetLastError();
	}
	printf("[fs] delete folder '%s' ok\n", buffer);
#endif

#ifdef BTMTK_ON_WISE
	S32 l_ret;
	l_ret = FS_RemoveDir(pwsFilename);
	if( l_ret < FS_NO_ERROR ){
		OS_Report("[fs] del folder fail err:(%d)", l_ret);
		return BTMTK_FS_ERR;
	}
	OS_Report("[fs] del folder ok ret:(%d)", l_ret);
#endif

#ifdef BTMTK_ON_LINUX
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    buffer[0] = 0;
    int errnum;

    bt_chset_ucs2_to_utf8_string( buffer, BTMTK_FS_MAX_FILEPATH, (const U8*)pwsFilename);

    if (-1 == remove((const char*)buffer)) {
	errnum = errno;
	OS_Report("[fs] delete folder '%s' failed. Reason: %s[%d]",
		buffer, strerror(errnum), errnum);
	return errnum;
    }
    OS_Report("[fs] delete folder '%s' ok\n", buffer);
#endif

    return BTMTK_FS_OK;
}


FS_STATUS btmtk_fs_hide_file_ucs2(const U16 *pwsFilename){
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    buffer[0] = 0;
    bt_ucs2_str_n_to_asc_str( (S8 *)buffer, (S8 *)pwsFilename, BTMTK_FS_MAX_FILEPATH);

    printf("[fs] hide file '%s'\n", buffer);
    return BTMTK_FS_OK;
}


FS_STATUS btmtk_fs_unhide_file_ucs2(const U16 *pwsFilename){
	U8 buffer[BTMTK_FS_MAX_FILEPATH];
    buffer[0] = 0;
    bt_ucs2_str_n_to_asc_str( (S8 *)buffer, (S8 *)pwsFilename, BTMTK_FS_MAX_FILEPATH);

    printf("[fs] unhide file '%s'\n", buffer);
    return BTMTK_FS_OK;
}

BT_BOOL btmtk_fs_is_file_exist_ucs2(const U16 *pwsFilename){
#ifdef USE_WIN32_FS
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    HANDLE h;
    BT_BOOL l_ret = FALSE;

    buffer[0] = 0;

    bt_ucs2_str_n_to_asc_str( (S8 *)buffer, (S8 *) pwsFilename, BTMTK_FS_MAX_FILEPATH);

    if( INVALID_HANDLE_VALUE == (h = CreateFile(pwsFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL , NULL) ) ){
    	l_ret = FALSE;
    }else{
	    CloseHandle(h);
    	l_ret = TRUE;
    }
    printf("[fs] is fileexit '%s' ret=%d\n", buffer, l_ret);
    return l_ret;
#endif

#ifdef BTMTK_ON_WISE
	S32 retval;
	U8 buffer[BTMTK_FS_MAX_FILEPATH];
	S32 index;
	U16 *ptr1;
	char ucfile[BTMTK_FS_MAX_FILEPATH];


	btmtk_os_memset( (void *) buffer, 0, BTMTK_FS_MAX_FILEPATH);
	bt_ucs2ncpy( buffer, (U8 *) pwsFilename , (BTMTK_FS_MAX_FILEPATH/2) -1 );
	ptr1 = (U16 *)buffer;
	for( index = 0; index < bt_ucs2strlen(buffer) ; index++ ){
		ptr1++;
		if( (U16) *ptr1 == (U16) 0x002F )
			*ptr1 = (U16) 0x005C;
	}

	memset(ucfile, 0, sizeof(ucfile));
	bt_chset_ucs2_to_utf8_string( (U8 *) ucfile, sizeof(ucfile), (const U8 *) buffer);

	retval = FS_GetAttributes( (U16 *)buffer);
	OS_Report("[goep][fs] is_file_exist (%s) ret:%d", ucfile, retval);

	if (retval < FS_NO_ERROR)
	{
	    /* Consider no such file exist */
	    return FALSE;
	}

    return TRUE;
#endif

#ifdef BTMTK_ON_LINUX
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    int fd;
    BT_BOOL l_ret = FALSE;

    buffer[0] = 0;

    bt_chset_ucs2_to_utf8_string( buffer, BTMTK_FS_MAX_FILEPATH, (const U8*)pwsFilename);

    if( -1 == (fd = open((const char*)buffer, O_RDONLY) ) ){
    	l_ret = FALSE;
    }else{
	    close(fd);
    	l_ret = TRUE;
    }
    printf("[fs] is fileexit '%s' ret=%d\n", buffer, l_ret);
    return l_ret;
#endif
}


BT_BOOL btmtk_fs_is_dir_exist_ucs2(const U16 *pwsFilename){
#ifdef USE_WIN32_FS
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    BT_BOOL                        fOk;
	WIN32_FILE_ATTRIBUTE_DATA   fileInfo;
    BT_BOOL l_ret = FALSE;

    buffer[0] = 0;
    bt_ucs2_str_n_to_asc_str( (S8 *)buffer, (S8 *)pwsFilename, BTMTK_FS_MAX_FILEPATH);


    if (NULL == pwsFilename){
        return FALSE;
    }

    fOk = GetFileAttributesEx(pwsFilename, GetFileExInfoStandard, (void*)&fileInfo);
    if (!fOk){
		l_ret = FALSE;
		printf("[fs] is folder exit check fail l_ret=%d !\n", GetLastError());
		return FALSE;
   	}else{

	    if( FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & fileInfo.dwFileAttributes) )
		{
			l_ret = TRUE;
		}
    }
	printf("[fs] is folder exit '%s' l_ret=%d\n", buffer, l_ret );
	return l_ret;
#endif

#ifdef BTMTK_ON_WISE
	S32 retval;
	U8 buffer[BTMTK_FS_MAX_FILEPATH];
	S32 index;
	U16 *ptr1;
	char ucfile[BTMTK_FS_MAX_FILEPATH];


	btmtk_os_memset( (void *) buffer, 0, BTMTK_FS_MAX_FILEPATH);
	bt_ucs2ncpy( buffer, (U8 *) pwsFilename , (BTMTK_FS_MAX_FILEPATH/2) -1 );
	ptr1 = (U16 *)buffer;
	for( index = 0; index < bt_ucs2strlen(buffer) ; index++ ){
		ptr1++;
		if( (U16) *ptr1 == (U16) 0x002F )
			*ptr1 = (U16) 0x005C;
	}

	memset(ucfile, 0, sizeof(ucfile));
	bt_chset_ucs2_to_utf8_string( (U8 *) ucfile, sizeof(ucfile), (const U8 *) buffer);

	retval = FS_GetAttributes( (U16 *)buffer);
	OS_Report("[goep][fs] is_folder_exist (%s) ret:%d", ucfile, retval);

	if (retval < FS_NO_ERROR)
	{
	    /* Consider no such file exist */
	    return FALSE;
	}

    return TRUE;
#endif

#ifdef BTMTK_ON_LINUX
    U8 buffer[BTMTK_FS_MAX_FILEPATH];
    DIR *dir;

    buffer[0] = 0;
    bt_chset_ucs2_to_utf8_string( (U8*)buffer, BTMTK_FS_MAX_FILEPATH, (const U8*)pwsFilename);

    if (NULL == pwsFilename) {
        return FALSE;
    }

    dir = opendir((const char*)buffer);
    if (!dir) {
	printf("[fs] is folder exit check fail!\n");
	return FALSE;
    }
    printf("[fs] is folder exit '%s'", buffer);
    closedir(dir);
    return TRUE;
#endif
}

S32 btmtk_fs_get_position_ucs2(FHANDLE hFile, U32 *pos){
#ifdef USE_WIN32_FS
    *pos =   SetFilePointer( (HANDLE) hFile, 0,0, FILE_CURRENT);
#endif

#ifdef BTMTK_ON_WISE
    *pos = FS_Seek( (FS_HANDLE) hFile, 0, FS_FILE_CURRENT);;
#endif

#if defined(BTMTK_ON_LINUX)
    S32 tmpPos = lseek((int) hFile, 0, SEEK_CUR);
    if(tmpPos == -1)
    {
        return translateFileError(errno);
    }else{
        *pos = (U32)tmpPos;
    }
#endif
	return *pos;

}

BT_BOOL btmtk_goep_get_parent_folder_ucs2(const U8 *ucFolderpath, U8 *ucParent, U32 maxSize)
{
    S32 index;
    bt_ucs2ncpy((S8*)ucParent, (const S8*)ucFolderpath, maxSize/2); // use word not byte

    if( ucFolderpath == 0 || ucParent == 0 || maxSize <=4 )
        return FALSE;

    index = bt_ucs2strlen((const S8*)ucParent); //3 01,23,45,6
    if( 0 == index ){
        return FALSE;
    }

    while( index > 0 ){
        index--;
        if( ucParent[index*2] == '\\' && ucParent[index*2+1] == 0 ){
            ucParent[index*2] = ucParent[index*2+1] = 0;
            break;
        }
        if( ucParent[index*2] == '/' && ucParent[index*2+1] == 0 ){
            ucParent[index*2] = ucParent[index*2+1] = 0;
            break;
        }

        ucParent[index*2] = ucParent[index*2+1] = 0;
    }
    return TRUE;
}
