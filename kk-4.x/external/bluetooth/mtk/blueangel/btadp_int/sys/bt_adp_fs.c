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
 * Bt_adp_fs.c
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
 * Dlight Ting
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
#ifdef BTMTK_ON_WISE
#include "fs_type.h"
#include "fs_errcode.h"
#include "fs_func.h"
#include "bluetooth_trc.h"

#include "bt_ucs2.h"
#else /* BTMTK_ON_WISE */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BTMTK_ON_LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#else /* BTMTK_ON_LINUX */ 
#include <direct.h>
#include "windows.h"
#endif /* BTMTK_ON_LINUX */ 
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif /* BTMTK_ON_WISE */ 

/* basic system service headers */
#include "bt_common.h"
#include "bttypes.h"
#include "bt_adp_fs.h"


/***************************************************************************** 
* Define
*****************************************************************************/

#define BT_MAX_NUM_FILES (10)
typedef struct
{
    kal_bool used;
    int handle; /* file handle */
} file_info_struct;

static file_info_struct bt_file_desc[BT_MAX_NUM_FILES] = {{0,0}};

#if defined(BTMTK_ON_WISE)
#elif defined(BTMTK_ON_LINUX)
U8 g_bt_dir_buf[FS_MAX_PATH_LEN] = {0};
#else
U8 g_bt_dir_buf[FS_MAX_PATH_LEN] = {0};
#endif
U16 pathBuf[FS_MAX_PATH_LEN] = {0};

/***************************************************************************** 
* Function
*****************************************************************************/
/*****************************************************************************
 * FUNCTION
 *  translateFileModeC
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
const S8 *translateFileModeC(U32 mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((mode & BTMTK_FS_READ_ONLY))
    {
        return FS_C_READ_MODE;
    }

    if ((mode & BTMTK_FS_READ_WRITE) && ((mode & BTMTK_FS_CREATE) == 0))
    {
        return FS_C_READ_WRITE_MODE;
    }

    if ((mode & BTMTK_FS_APPEND))
    {
        return FS_C_APPEND_MODE;
    }

    if ((mode & BTMTK_FS_CREATE))
    {
        return FS_C_CREATE_READ_WRITE_MODE;
    }
    return FS_C_READ_MODE;
}


/*****************************************************************************
 * FUNCTION
 *  translateSeekModeC
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 translateSeekModeC(btmtk_seek_mode_enum mode)
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
            return SEEK_SET;
        case BTMTK_FS_SEEK_CURRENT:
            return SEEK_CUR;
        case BTMTK_FS_SEEK_END:
            return SEEK_END;
    }
    return SEEK_SET;
}



#ifdef BTMTK_ON_WISE


/*****************************************************************************
 * FUNCTION
 *  translateFileMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U32 translateFileMode(U32 mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 m = 0;   /* reset */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((mode & BTMTK_FS_READ_ONLY))
    {
        m |= FS_READ_ONLY;
    }

    if (mode & BTMTK_FS_READ_WRITE)
    {
        m |= FS_READ_WRITE;
    }

    if ((mode & BTMTK_FS_CREATE))
    {
        m |= FS_CREATE; /* not always */
    }

    if (mode & BTMTK_FS_CREATE_ALWAYS)
    {
        m |= FS_CREATE_ALWAYS;
    }

    if (mode & BTMTK_FS_APPEND)
    {
        m |= FS_READ_WRITE;
        m |= FS_CREATE;
    }

    if (mode & BTMTK_FS_OPEN_DIR)
    {
        m |= FS_OPEN_DIR;
    }
    return m;

}


/*****************************************************************************
 * FUNCTION
 *  translateMoveMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U32 translateMoveMode(btmtk_move_mode_enum mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 m = 0;  /* reset */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((mode & BTMTK_FS_MOVE_COPY))
    {
        m |= FS_MOVE_COPY;
    }

    if (mode & BTMTK_FS_MOVE_KILL)
    {
        m |= FS_MOVE_KILL;
    }

    if ((mode & BTMTK_FS_MOVE_OVERWRITE))
    {
        m |= FS_MOVE_OVERWRITE;
    }

    return m;
}


/*****************************************************************************
 * FUNCTION
 *  translateSeekMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 translateSeekMode(btmtk_seek_mode_enum mode)
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
            return FS_FILE_BEGIN;
        case BTMTK_FS_SEEK_CURRENT:
            return FS_FILE_CURRENT;
        case BTMTK_FS_SEEK_END:
            return FS_FILE_END;
    }
}


/*****************************************************************************
 * FUNCTION
 *  translateFileError
 * DESCRIPTION
 *  
 * PARAMETERS
 *  err     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
int translateFileError(int err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (err >= 0)
    {
        return err;
    }

    switch (err)
    {
        case FS_PATH_NOT_FOUND:
        case FS_FILE_NOT_FOUND:
        case FS_INVALID_FILENAME:
            return BTMTK_FS_ERR_PATH;

        case FS_READ_ONLY_ERROR:
        case FS_ACCESS_DENIED:
        case FS_DEVICE_EXPORTED_ERROR:
        case FS_MEDIA_CHANGED:
        case FS_TOO_MANY_FILES:
            return BTMTK_FS_ERR_ACCESS;

        case FS_DISK_FULL:
        case FS_APP_QUOTA_FULL:
            return BTMTK_FS_ERR_FULL;

        case FS_FILE_EXISTS:
            return BTMTK_FS_ERR_EXIST;

        case FS_FLASH_ERASE_BUSY:
        case FS_LOCK_MUTEX_FAIL:
            return BTMTK_FS_ERR_DELAYED;

        default:
            return BTMTK_FS_ERR;
    }
}

#elif defined(BTMTK_ON_LINUX)

/*****************************************************************************
 * FUNCTION
 *  translateFileMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U32 translateFileMode(U32 mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 m = 0;  /* reset */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((mode & BTMTK_FS_READ_ONLY))
    {
        m |= O_RDONLY;
    }

    if (mode & BTMTK_FS_READ_WRITE)
    {
        m |= O_RDWR;
    }

    if ((mode & BTMTK_FS_CREATE))
    {
        m |= O_CREAT;	// create or overwrite
    }

    if ((mode & BTMTK_FS_CREATE_ALWAYS))
	{
    	m |= O_TRUNC;	// delete existing file
    	m |= O_CREAT;	// create
	}

    if (mode & BTMTK_FS_APPEND)
    {
        m |= O_APPEND;
    }

    return m;
}

/*****************************************************************************
 * FUNCTION
 *  translateSeekMode
 * DESCRIPTION
 *  
 * PARAMETERS
 *  mode        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 translateSeekMode(btmtk_seek_mode_enum mode)
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
            return SEEK_SET;
        case BTMTK_FS_SEEK_CURRENT:
            return SEEK_CUR;
        case BTMTK_FS_SEEK_END:
            return SEEK_END;
    }
    return SEEK_SET;
}


/*****************************************************************************
 * FUNCTION
 *  translateFileError
 * DESCRIPTION
 *  
 * PARAMETERS
 *  err     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
int translateFileError(int err)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

//20101004 mtk03036
    OS_Report("[FS][ERROR] linux errno[%d]", err);
//20101004 mtk03036 end

    switch (err)
    {
        case EFAULT:
            return BTMTK_FS_ERR_PATH;

        case EACCES:
        case EROFS:
            return BTMTK_FS_ERR_ACCESS;

        default:
            return BTMTK_FS_ERR;
    }
}


/*****************************************************************************
 * FUNCTION
 *  translateFilePath
 * DESCRIPTION
 *  
 * PARAMETERS
 *  refPath         [IN]        
 *  absPathBuf      [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void translateFilePath(const U8 *refPath, U8 *absPathBuf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    OS_StrCpy((char*)absPathBuf, (const char*)refPath);
}

#else 


/*****************************************************************************
 * FUNCTION
 *  locateBtRootDir
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void locateBtRootDir(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *buffer = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (OS_StrLen(g_bt_dir_buf) == 0 && 
#ifdef BTMTK_ON_LINUX
        (buffer = getcwd(g_bt_dir_buf, FS_MAX_PATH_LEN)) == NULL
#else
        (buffer = _getcwd(g_bt_dir_buf, FS_MAX_PATH_LEN)) != NULL &&
        (buffer = strstr(g_bt_dir_buf, MBT_FS_PATH)) != NULL
#endif
        )
    {
        g_bt_dir_buf[0] = '\0';
    }
}

#define translateFileMode translateFileModeC
#define translateSeekMode translateSeekModeC


/*****************************************************************************
 * FUNCTION
 *  translateFilePath
 * DESCRIPTION
 *  
 * PARAMETERS
 *  refPath         [IN]        
 *  absPathBuf      [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void translateFilePath(const U8 *refPath, U8 *absPathBuf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    locateBtRootDir();
    OS_StrCpy(absPathBuf, g_bt_dir_buf);
    OS_StrCat(absPathBuf, refPath);
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


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_init
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_fs_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char cwdpath[128];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_prompt_trace(MOD_BT, "[FS] btmtk_fs_init");
    OS_Report("[FS][INIT] BT_DB_PATH='%s'", BT_DB_PATH);
    OS_Report("[FS][INIT] BT_DB_FILE='%s'", BT_DB_FILE);
#ifdef BTMTK_ON_LINUX
    memset( cwdpath, 0 , sizeof(cwdpath));
    getcwd(cwdpath, sizeof(cwdpath)-1);
    OS_Report("[FS][INIT] getcwd()='%s'", cwdpath);
#else
    OS_Report("[FS][INIT] getcwd() unknow" );
#endif
    memset(bt_file_desc, 0x0, sizeof(file_info_struct) * BT_MAX_NUM_FILES);
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_open
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fileName        [IN]        
 *  mode            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_open(const U8 *fileName, U32 mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i;

#ifdef BTMTK_ON_WISE
    int err, err1;
#else
    FILE *fp;
    U8 fileExist = 0;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < BT_MAX_NUM_FILES; i++)
    {
        if (bt_file_desc[i].used == KAL_FALSE)
        {
            break;
        }
    }

    if (i >= BT_MAX_NUM_FILES)
    {
        return BTMTK_FS_ERR_FULL;
    }

#ifdef BTMTK_ON_WISE
    bt_asc_str_n_to_ucs2_str((S8 *)pathBuf, (S8 *)fileName, FS_MAX_PATH_LEN * sizeof(U16));
    err = FS_Open(pathBuf, translateFileMode(mode));
    if ((mode & BTMTK_FS_APPEND) && err >= 0)
    {
        err1 = FS_Seek(err, 0, FS_FILE_END);
        err = (err1 < 0) ? err1 : err;
    }

    if (err >= 0)
    {
        bt_file_desc[i] = err;
        bt_file_desc[i].used = KAL_TRUE;
        return i;
    }
    else
    {
        return translateFileError(err);
    }
#else /* BTMTK_ON_WISE */ 
    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_open(fileName=%s, mode=0x%X)", fileName, mode);
    translateFilePath(fileName, (U8 *)pathBuf);
    bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_open: %s", fileName);
    fileExist = IsFileExist((const char *)pathBuf);
    fp = fopen((char *)pathBuf, translateFileModeC(mode));
    if (fp == NULL)
    {
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_open failed : %s, errno=%d", strerror(errno), errno);
        return BTMTK_FS_ERR;
    }
    else
    {
        bt_file_desc[i].handle = (int)fp;
        bt_file_desc[i].used = KAL_TRUE;
        if( chmod((const char *)pathBuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0 )
            bt_prompt_trace(MOD_BT, "[ERR] chmod failed : errno=%u, %s", errno, strerror(errno));
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_open handle=0x%x", bt_file_desc[i]);
        return i;
    }
#endif /* BTMTK_ON_WISE */ 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_close
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fd      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_close(int fd)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int handle = bt_file_desc[fd].handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bt_file_desc[fd].used == KAL_FALSE)
    {
        bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_close(fd=%d) cannot found fd", fd);
        return BTMTK_FS_ERR;
    }

    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_close(fd=%d, handle=0x%X)", fd, handle);
    bt_file_desc[fd].used = KAL_FALSE;
    bt_file_desc[fd].handle = 0;
#ifdef BTMTK_ON_WISE
    return translateFileError(FS_Close(handle));
#else
    if (fclose((FILE*) handle) != 0)
    {
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_close failed : %s, errno=%d", strerror(errno), errno);
        return BTMTK_FS_ERR;
    }
    return BTMTK_FS_OK;
#endif
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_read
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fd          [IN]        
 *  buffer      [?]         
 *  size        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_read(int fd, void *buffer, U32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int handle = bt_file_desc[fd].handle;

#ifdef BTMTK_ON_WISE
    U32 byte_read = 0;
    S32 err;
#else
    size_t byte_read = 0;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bt_file_desc[fd].used == KAL_FALSE)
    {
        bt_prompt_trace(MOD_BT, "[FS] btmtk_fs_read(fd=%d) cannot found fd", fd);
        return BTMTK_FS_ERR;
    }

    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_read(handle=0x%X, buffer=0x%X, size=%d)", handle, buffer, size);
#ifdef BTMTK_ON_WISE
    if ((err = FS_Read(handle, buffer, size, &byte_read)) == FS_NO_ERROR)
    {
        return byte_read;
    }
    return translateFileError(err);
#else
    byte_read = fread(buffer, 1, size, (FILE *) handle);
    if (byte_read == size || feof((FILE *) handle))
    {
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_read : %d", byte_read);
        return byte_read;
    }
    bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_read failed : %s, errno=%d", strerror(errno), errno);
    return BTMTK_FS_ERR;
#endif /* BTMTK_ON_WISE */ 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_write
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fd          [IN]        
 *  buffer      [?]         
 *  size        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_write(int fd, void *buffer, U32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int handle = bt_file_desc[fd].handle;
#ifdef BTMTK_ON_WISE
    U32 bytes_written = 0;
    S32 err;
#else
    size_t bytes_written = 0;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bt_file_desc[fd].used == KAL_FALSE)
    {
        bt_prompt_trace(MOD_BT, "[FS] btmtk_fs_write(fd=%d) cannot found fd", fd);
        return BTMTK_FS_ERR;
    }

#ifdef BTMTK_ON_WISE
    if ((err = FS_Write(handle, buffer, size, &bytes_written)) == FS_NO_ERROR)
    {
        return bytes_written;
    }
    return translateFileError(err);
#else
    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_write(handle=0x%X, buffer=0x%X, size=%d)", handle, buffer, size);
    bytes_written = fwrite(buffer, 1, size, (FILE *) handle);
    if (bytes_written == size)
    {
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_write : %d", bytes_written);
        return bytes_written;
    }
    bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_write failed : %s, errno=%d", strerror(errno), errno);
    return BTMTK_FS_ERR;
#endif /* BTMTK_ON_WISE */ 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_is_dir_exist
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dirName     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BT_BOOL btmtk_fs_is_dir_exist(const U8 *dirName)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef BTMTK_ON_WISE
    FS_HANDLE file_handle;

    bt_asc_str_n_to_ucs2_str((S8 *)pathBuf, (S8 *)dirName, FS_MAX_PATH_LEN * sizeof(U16));
    if ((file_handle = FS_Open(pathBuf, FS_OPEN_DIR | FS_READ_ONLY)) > 0)
    {
        FS_Close(file_handle);
        return TRUE;
    }
    return FALSE;
#else /* BTMTK_ON_WISE */ 
    #ifdef BTMTK_ON_LINUX
    struct stat buf;
    BT_BOOL bRet;
    #else
    struct _stat buf;
    #endif
    translateFilePath(dirName, (U8 *)pathBuf);
    #ifdef BTMTK_ON_LINUX
    bRet = (BT_BOOL)((stat((char *)pathBuf, &buf) != 0) ? FALSE : TRUE);
    if(!bRet)
    {
        bt_prompt_trace(MOD_BT, "[FS] btmtk_fs_is_dir_exist failed : %s, errno=%d", strerror(errno), errno);
    }
    return bRet;
    #else
    return (BT_BOOL)((_stat((char *)pathBuf, &buf) != 0) ? FALSE : TRUE);
    #endif
#endif /* BTMTK_ON_WISE */ 
    bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_is_dir_exist");
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_create_dir
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dirName     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_create_dir(const U8 *dirName)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_create_dir(%s)", dirName);
#ifdef BTMTK_ON_WISE
    bt_asc_str_n_to_ucs2_str((S8 *)pathBuf, (S8 *)dirName, FS_MAX_PATH_LEN * sizeof(U16));
    return translateFileError(FS_CreateDir(pathBuf));
#else
    translateFilePath(dirName, (U8 *)pathBuf);
    #ifdef BTMTK_ON_LINUX
    if (mkdir((char *)pathBuf, 0777) != 0)
    #else
    if (_mkdir((char *)pathBuf) != 0)
    #endif
    {
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_create_dir failed : %s, errno=%d", strerror(errno), errno);
        return BTMTK_FS_ERR;
    }
    bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_create_dir");
    return BTMTK_FS_OK;
#endif /* BTMTK_ON_WISE */ 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_delete
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fileName        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_delete(const U8 *fileName)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_prompt_trace(MOD_BT, "[FS] +btmtk_fs_delete(%s)", fileName);
#ifdef BTMTK_ON_WISE
    bt_asc_str_n_to_ucs2_str((S8 *)pathBuf, (S8 *)fileName, FS_MAX_PATH_LEN * sizeof(U16));
    return translateFileError(FS_Delete(pathBuf));
#else
    
    translateFilePath(fileName, (U8 *)pathBuf);
    if (remove((char *)pathBuf) != 0)
    {
        bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_delete failed : %s, errno=%d", strerror(errno), errno);
        return BTMTK_FS_ERR;
    }
    bt_prompt_trace(MOD_BT, "[FS] -btmtk_fs_delete");
    return BTMTK_FS_OK;
#endif /* BTMTK_ON_WISE */ 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_get_filesize
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fileName        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_get_filesize(const U8 *fileName)
{
#ifdef BTMTK_ON_WISE
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int err, handle;
    U32 size;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_asc_str_n_to_ucs2_str((S8 *)pathBuf, (S8 *)fileName, FS_MAX_PATH_LEN * sizeof(U16));
    handle = FS_Open(pathBuf, FS_READ_ONLY | FS_OPEN_SHARED);
    if (handle < 0)
    {
        return translateFileError(handle);
    }
    err = FS_GetFileSize(handle, &size);
    FS_Close(handle);
    return translateFileError(err);
#else /* BTMTK_ON_WISE */ 
    FILE *fp;
    int size;

    translateFilePath(fileName, (U8 *)pathBuf);
    fp = fopen((char *)pathBuf, translateFileModeC(BTMTK_FS_READ_ONLY));
    if (fp == NULL)
    {
        return BTMTK_FS_ERR;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
#endif /* BTMTK_ON_WISE */ 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_seek
 * DESCRIPTION
 *  
 * PARAMETERS
 *  fd              [IN]        
 *  offset          [IN]        
 *  seek_mode       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_seek(int fd, U32 offset, btmtk_seek_mode_enum seek_mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 pos;
    int handle = bt_file_desc[fd].handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bt_file_desc[fd].used == KAL_FALSE)
    {
        bt_prompt_trace(MOD_BT, "[FS] btmtk_fs_seek(fd=%d) cannot found fd", fd);
        return BTMTK_FS_ERR;
    }

#ifdef BTMTK_ON_WISE
    pos = FS_Seek(handle, offset, translateSeekMode(seek_mode));
    return translateFileError(pos);
#else
    if (fseek((FILE *)handle, offset, translateSeekMode(seek_mode)) == 0)
    {
        pos = ftell((FILE *)handle);
        return pos;
    }
    return BTMTK_FS_ERR;
#endif
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_fs_rename
 * DESCRIPTION
 *  
 * PARAMETERS
 *  srcName         [IN]        
 *  destName        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 btmtk_fs_rename(const U8 *srcName, const U8 *destName)
{
#ifdef BTMTK_ON_WISE
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 ucs2_buf[FS_MAX_PATH_LEN] = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_asc_str_n_to_ucs2_str((S8 *)pathBuf, (S8 *)srcName, FS_MAX_PATH_LEN * sizeof(U16));
    bt_asc_str_n_to_ucs2_str((S8 *)ucs2_buf, (S8 *)destName, FS_MAX_PATH_LEN * sizeof(U16));
    result = FS_Rename(srcName, destName);
    return translateFileError(result);
#else /* BTMTK_ON_WISE */ 
    U8 pathBuf2[FS_MAX_PATH_LEN] = {0};
    translateFilePath(srcName, (U8 *)pathBuf);
    translateFilePath(destName, pathBuf2);
  #if defined(BTMTK_ON_LINUX)
    if (rename((char *)pathBuf, (char*)pathBuf2) == 0)
  #else
	if (FALSE != MoveFileA((char *)pathBuf, pathBuf2))
  #endif
    {
        return BTMTK_FS_OK;
    }
    return BTMTK_FS_ERR;
#endif /* BTMTK_ON_WISE */ 
}


