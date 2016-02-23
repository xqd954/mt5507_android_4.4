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
*  permission of MediaTek Inc. (C) 2006
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
 *
 * Project:
 * --------
 *
 * Description:
 * ------------
 *
 * Author:
 * -------
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#ifndef __BT_ADP_FS_UCS2_H__
#define __BT_ADP_FS_UCS2_H__
#include "bt_types.h"

typedef S32 FHANDLE; ///< file handle is a signed int
typedef S32 BTMTK_FS_HANDLE;
typedef S32 FS_STATUS; ///< file read status

#define FHANDLE_INVALID_VALUE      0     ///< FileHanel <0 invalid. 0 is invalid. >0 is valid.

/* MMI usb context bit-wise flag defination */
#define FS_ERR_FOLDER_ERR          (0x01)   // read folder structure error 
#define FS_ERR_WRITE_FILE_FAIL     (0x04)   // write file error
#define FS_ERR_DISK_FULL           (0x08)   // 
#define FS_ERR_ROOT_DIR_FULL       (0x10)   // 
#define FS_ERR_DEVICE_EXPORTED     (0x20)   // 
#define FS_ERR_RSPERVED_5          (0x40)
#define FS_ERR_RSPERVED_6          (0x80)
/* When long fs operation on card, fs api will become Non-blockong */
#define FS_ERR_FS_DEVICE_BUSY      (0x100)
#define FS_ERR_FS_LOCK_MUTEX_FAIL  (0x200)
#define FS_ERR_FS_NO_CARD          (0x400)
#define FS_ERR_FS_WRITE_PROTEC     (0x800)

FHANDLE btmtk_fs_open_ucs2(const U16 *FileName, btmtk_file_mode_enum Flag);
S32 btmtk_fs_read_ucs2(FHANDLE FileHandle, void *DataPtr, S32 Length, S32 *Read);
S32 btmtk_fs_write_ucs2(FHANDLE FileHandle, void *DataPtr, S32 Length, S32 *Written);
S32 btmtk_fs_close_ucs2(FHANDLE FileHandle);
S32 btmtk_fs_seek_ucs2(FHANDLE FileHandle, S32 offset, btmtk_seek_mode_enum seek_mode);
S32 btmtk_fs_create_dir_ucs2(const U16 *dirName);
S32 btmtk_fs_delete_ucs2(const U16 *fileName);
S32 btmtk_fs_get_current_dir_ucs2(U32 buf_len, U16* path);
S32 btmtk_fs_get_filesize_ucs2(FHANDLE FileHandle, U32 *Size);
S32 btmtk_fs_move_file_ucs2(const U16* SrcFullPath, const U16* DstFullPath, btmtk_move_mode_enum Flag);

/// Export for folder manipulating
extern BT_BOOL btmtk_fs_is_dir_exist_ucs2(const U16 *pwsFilename); ///< check the folder is exist or not

/// Export for file manipulating
extern BT_BOOL btmtk_fs_is_file_exist_ucs2(const U16 *pwsFilename); ///< check the file is exist or not
extern FS_STATUS btmtk_fs_rename_file_ucs2(const U16 *pwsOldFilename, const U16 *pwsNewFilename); ///< FS_Rename
extern FS_STATUS btmtk_fs_move_file(const U16 *pwsFilename,const  U16 *pwsNewFilename);///<FS_MOVE
extern FS_STATUS btmtk_fs_hide_file_ucs2(const U16 *pwsFilename); ///< FS_GetAttributes and mark the hide bit
extern FS_STATUS btmtk_fs_unhide_file_ucs2(const U16 *pwsFilename); ///< FS_GetAttributes and unmakr the hide bit
extern FS_STATUS btmtk_fs_delete_dir_ucs2(const U16 *pwsFilename);

/// Export for reading. Low Level API
extern BT_BOOL btmtk_fs_is_valid_handle_ucs2(FHANDLE hFile); ///< Is a valid File Handle 

/// FILE seek
extern S32 btmtk_fs_get_position_ucs2(FHANDLE hFile, U32 *pos);
	
///
extern BT_BOOL btmtk_goep_get_parent_folder_ucs2(const U8 *ucFolderpath, U8 *ucParent, U32 maxSize);
	
#endif
