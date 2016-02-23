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
 * Bt_adp_fs.h
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
#ifndef __BT_ADP_FS_H__
#define __BT_ADP_FS_H__

#if defined(BTMTK_ON_WISE)
#define BT_DB_FILE "Z:\\@btmtk\\devdb"
#define BT_DB_COD_FILE "Z:\\@btmtk\\cod"
#define BT_DB_PATH "Z:\\@btmtk\\"

#define FS_MAX_PATH_LEN 300
#elif defined(BTMTK_ON_LINUX)
#define BT_DB_PATH "/data/@btmtk"
#define BT_DB_FILE BT_DB_PATH"/devdb"
#define BT_DB_COD_FILE BT_DB_PATH"/cod"
#ifdef __BT_4_0_BLE__
#define BT_DB_LE_FILE BT_DB_PATH"/le"
#endif

#define FS_C_READ_MODE         "rb"
#define FS_C_READ_WRITE_MODE   "r+b"
#define FS_C_CREATE_READ_WRITE_MODE   "wb"
#define FS_C_APPEND_MODE   "a"

#define FS_MAX_PATH_LEN PATH_MAX
#else
#define MBT_FS_PATH "\\Exec\\PC_MBTSDK_ALL_ALL"
#define BT_DB_PATH BTMTK_WORKING_PATH
#define BT_DB_FILE BT_DB_PATH"\\devdb"
#define BT_DB_COD_FILE BT_DB_PATH"\\cod"
#ifdef __BT_4_0_BLE__
#define BT_DB_LE_FILE BT_DB_PATH"/le"
#endif

#define FS_C_READ_MODE         "rb"
#define FS_C_READ_WRITE_MODE   "r+b"
#define FS_C_CREATE_READ_WRITE_MODE   "wb"
#define FS_C_APPEND_MODE   "a"

#define FS_MAX_PATH_LEN _MAX_PATH
#endif


typedef enum 
{
    BTMTK_FS_READ_WRITE     = 0x000000001,
    BTMTK_FS_READ_ONLY      = 0x000000002,
    BTMTK_FS_CREATE         = 0x000000004,
    BTMTK_FS_CREATE_ALWAYS  = 0x000000008,
    BTMTK_FS_NONBLOCK       = 0x000000010,
    BTMTK_FS_APPEND         = 0x000000020,
    BTMTK_FS_OPEN_DIR       = 0x000000040,
} btmtk_file_mode_enum;

typedef enum 
{
    BTMTK_FS_MOVE_COPY  = 0x00000001,
    BTMTK_FS_MOVE_KILL  = 0x00000002,
    BTMTK_FS_MOVE_OVERWRITE = 0x00000004
} btmtk_move_mode_enum;

typedef enum
{
    BTMTK_FS_SEEK_BEGIN,
    BTMTK_FS_SEEK_CURRENT,
    BTMTK_FS_SEEK_END
} btmtk_seek_mode_enum;

typedef enum
{
    BTMTK_FS_OK = 0,
    BTMTK_FS_ERR = -1,
    BTMTK_FS_ERR_PATH = -2,
    BTMTK_FS_ERR_ACCESS = -3,
    BTMTK_FS_ERR_FULL = -4,
    BTMTK_FS_ERR_EXIST = -5,
    BTMTK_FS_ERR_DELAYED = -6,
    BTMTK_FS_ERR_DEVICE_BUSY = -7,
    BTMTK_FS_ERR_LOCK_MUTEX_FAIL = -8,
    BTMTK_FS_ERR_ROOT_DIR_FULL = -9,
    BTMTK_FS_ERR_APP_QUOTA_FULL = -10,
    BTMTK_FS_ERR_WRITE_PROTECTION = -11,
    BTMTK_FS_ERR_MEDIA_CHANGED = -12,
    BTMTK_FS_ERR_MSDC_MOUNT_ERROR = -13,
    BTMTK_FS_ERR_MSDC_NOT_PRESENT = -14,
    BTMTK_FS_ERR_EOF = -15
} btmtk_file_err_enum;

#ifdef BTMTK_ON_WISESDK
#define BTMTK_WORKING_PATH "\\Component\\MBT\\HWAdapt\\MTK\\LIB\\@btmtk"
#else
#define BTMTK_WORKING_PATH ""
#endif

void btmtk_fs_init(void);
S32 btmtk_fs_open(const U8 *fileName, U32 mode);
S32 btmtk_fs_close(int fd);
S32 btmtk_fs_read(int fd, void *buffer, U32 size);
S32 btmtk_fs_write(int fd, void *buffer, U32 size);
BT_BOOL btmtk_fs_is_dir_exist(const U8 *dirName);
S32 btmtk_fs_create_dir(const U8 *dirName);
S32 btmtk_fs_delete(const U8 *fileName);
S32 btmtk_fs_get_filesize(const U8 *fileName);
S32 btmtk_fs_seek(int fd, U32 offset, btmtk_seek_mode_enum seek_mode);
S32 btmtk_fs_rename(const U8 *srcName, const U8 *destName);

U32 translateFileMode(U32 mode);
S32 translateSeekMode(btmtk_seek_mode_enum mode);
int translateFileError(int err);

#endif /* __BT_ADP_FS_H__ */
