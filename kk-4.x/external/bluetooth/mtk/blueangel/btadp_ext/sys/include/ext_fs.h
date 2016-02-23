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
 * ext_fs.h
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
#ifndef __EXT_FS_H__
#define __EXT_FS_H__

#include "bt_adp_fs.h" /* temp for skip porting */
#include "bt_adp_fs_ucs2.h" /* temp for skip porting */

typedef S32 EXT_FS_HANDLE;

/* Change by External Platform Setting */
#define BTMTK_FS_MAX_FILEPATH 640

typedef enum 
{
    EXT_FS_READ_WRITE    = BTMTK_FS_READ_WRITE,
    EXT_FS_READ_ONLY     = BTMTK_FS_READ_ONLY,
    EXT_FS_CREATE        = BTMTK_FS_CREATE,
    EXT_FS_CREATE_ALWAYS = BTMTK_FS_CREATE_ALWAYS,
    EXT_FS_NONBLOCK      = BTMTK_FS_NONBLOCK,
    EXT_FS_APPEND        = BTMTK_FS_APPEND,
    EXT_FS_OPEN_DIR      = BTMTK_FS_OPEN_DIR,
} ext_fs_open_mode_enum;

typedef enum 
{
    EXT_FS_MOVE_COPY = BTMTK_FS_MOVE_COPY,
    EXT_FS_MOVE_KILL = BTMTK_FS_MOVE_KILL,
    EXT_FS_MOVE_OVERWRITE = BTMTK_FS_MOVE_OVERWRITE
} ext_fs_move_mode_enum;

typedef enum 
{
    EXT_FS_SEEK_BEGIN = BTMTK_FS_SEEK_BEGIN,
    EXT_FS_SEEK_CURRENT = BTMTK_FS_SEEK_CURRENT,
    EXT_FS_SEEK_END= BTMTK_FS_SEEK_END
} ext_fs_seek_mode_enum;

typedef enum
{
    EXT_FS_OK = BTMTK_FS_OK,
    EXT_FS_ERR = BTMTK_FS_ERR,
    EXT_FS_ERR_PATH = BTMTK_FS_ERR_PATH,
    EXT_FS_ERR_ACCESS = BTMTK_FS_ERR_ACCESS,
    EXT_FS_ERR_FULL = BTMTK_FS_ERR_FULL,
    EXT_FS_ERR_EXIST = BTMTK_FS_ERR_EXIST,
    EXT_FS_ERR_DELAYED = BTMTK_FS_ERR_DELAYED,
    EXT_FS_ERR_DEVICE_BUSY = BTMTK_FS_ERR_DEVICE_BUSY,
    EXT_FS_ERR_LOCK_MUTEX_FAIL = BTMTK_FS_ERR_LOCK_MUTEX_FAIL,
    EXT_FS_ERR_ROOT_DIR_FULL = BTMTK_FS_ERR_ROOT_DIR_FULL,
    EXT_FS_ERR_APP_QUOTA_FULL = BTMTK_FS_ERR_APP_QUOTA_FULL,
    EXT_FS_ERR_WRITE_PROTECTION = BTMTK_FS_ERR_WRITE_PROTECTION,
    EXT_FS_ERR_MEDIA_CHANGED = BTMTK_FS_ERR_MEDIA_CHANGED,
    EXT_FS_ERR_MSDC_MOUNT_ERROR = BTMTK_FS_ERR_MSDC_MOUNT_ERROR,
    EXT_FS_ERR_MSDC_NOT_PRESENT = BTMTK_FS_ERR_MSDC_NOT_PRESENT,
    EXT_FS_ERR_EOF = BTMTK_FS_ERR_EOF
} ext_file_err_enum;

#define ext_fs_open btmtk_fs_open
#define ext_fs_close btmtk_fs_close
#define ext_fs_read btmtk_fs_read
#define ext_fs_write btmtk_fs_write
#define ext_fs_seek btmtk_fs_seek
#define ext_fs_is_dir_exist btmtk_fs_is_dir_exist
#define ext_fs_create_dir btmtk_fs_create_dir
#define ext_fs_delete btmtk_fs_delete
#define ext_fs_rename btmtk_fs_rename
#define ext_fs_get_filesize btmtk_fs_get_filesize

#define ext_fs_open_ucs2 btmtk_fs_open_ucs2
#define ext_fs_read_ucs2 btmtk_fs_read_ucs2
#define ext_fs_write_ucs2 btmtk_fs_write_ucs2
#define ext_fs_close_ucs2 btmtk_fs_close_ucs2
#define ext_fs_seek_ucs2 btmtk_fs_seek_ucs2
#define ext_fs_create_dir_ucs2 btmtk_fs_create_dir_ucs2
#define ext_fs_delete_ucs2 btmtk_fs_delete_ucs2
#define ext_fs_get_current_dir_ucs2 btmtk_fs_get_current_dir_ucs2
#define ext_fs_get_filesize_ucs2 btmtk_fs_get_filesize_ucs2
#define ext_fs_move_file_ucs2 btmtk_fs_move_file_ucs2
#define ext_fs_is_valid_handle_ucs2 btmtk_fs_is_valid_handle_ucs2
#define ext_fs_rename_file_ucs2 btmtk_fs_rename_file_ucs2
#define ext_fs_delete_dir_ucs2 btmtk_fs_delete_dir_ucs2
#define ext_fs_hide_file_ucs2 btmtk_fs_hide_file_ucs2
#define ext_fs_unhide_file_ucs2 btmtk_fs_unhide_file_ucs2
#define ext_fs_is_file_exist_ucs2 btmtk_fs_is_file_exist_ucs2
#define ext_fs_is_dir_exist_ucs2 btmtk_fs_is_dir_exist_ucs2
#define ext_fs_get_position_ucs2 btmtk_fs_get_position_ucs2
#define ext_goep_get_parent_folder_ucs2 btmtk_goep_get_parent_folder_ucs2
#endif /* __EXT_FS_H__ */

