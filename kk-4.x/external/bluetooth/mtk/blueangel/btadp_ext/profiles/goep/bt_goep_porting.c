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
 * bt_goep_porting.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   Compose filepath / folder path for OPP & FTP profile
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
 
#include "bt_goep_porting.h"
 
#include "bt_types.h"
#include "ext_ucs2.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"

#ifdef BTMTK_ON_WIN32
#include "io.h"
#include "windows.h"
#include "stdio.h"
#endif

#include "bt_goep_fs.h"
#include "bt_os_api.h"
#include "ext_osal.h" 
#include "bt_ext_debug.h"

#define GOEP_Report bt_ext_log

extern BT_BOOL bt_goep_ucs2_to_utf8(const U8 *ucs2, U8 *utf8, U32 u4utf8Size);

/************************** GOEP FILE READ/WRITE function *********************************/

/* 
 * OPP Client read a file for pushing
 */
FHANDLE btmtk_goep_open_wstream(const U16 *pwsFilename, U32 i4Attr){
    return (FHANDLE) ext_fs_open_ucs2( (const U16 *)pwsFilename, EXT_FS_READ_WRITE | EXT_FS_CREATE_ALWAYS | i4Attr);
}


FS_STATUS btmtk_goep_write_wstream(FHANDLE hFile, void *pData, S32 i4Length, S32 *pi4Written){
    return (FS_STATUS) ext_fs_write_ucs2( hFile, pData, i4Length, pi4Written);
}


FHANDLE btmtk_goep_open_rstream(const U16 *pwsFilename, U32 i4Attr){
    return (FHANDLE) ext_fs_open_ucs2( (const U16 *)pwsFilename, EXT_FS_READ_ONLY | i4Attr);
}


BT_BOOL btmtk_goep_is_valid_handle(FHANDLE hFile){
    return ext_fs_is_valid_handle_ucs2(hFile);
}


BT_BOOL btmtk_goep_get_filesize(FHANDLE hFile, U8 *filepath, U32 *pu4Len){
    BT_BOOL ret = EXT_FS_OK;
    
    if (hFile == FHANDLE_INVALID_VALUE)
    {
	    hFile = ext_fs_open_ucs2( (const U16 *)filepath, EXT_FS_READ_ONLY);
	    if (hFile <= 0)
        {   
	        return 0;
        }

		ret = (ext_fs_get_filesize_ucs2(hFile, pu4Len) == EXT_FS_OK);
		ext_fs_close_ucs2(hFile);
    }
    else
    {
		ret = (ext_fs_get_filesize_ucs2(hFile, pu4Len) == EXT_FS_OK);
    }
    return ret;
}


BT_BOOL btmtk_goep_close_rstream(FHANDLE hFile){
    return ext_fs_close_ucs2(hFile);
}

BT_BOOL btmtk_goep_close_wstream(FHANDLE hFile){
    return ext_fs_close_ucs2(hFile);
}


S32 btmtk_goep_read_rstream(FHANDLE hFile, void *pData, S32 i4Length, S32 *pi4Read){
    return ext_fs_read_ucs2(hFile, pData,  i4Length, pi4Read);
}


BT_BOOL btmtk_goep_isvalid_read(FS_STATUS status){
    return TRUE;
}


FS_STATUS btmtk_goep_get_file_basename(const U8 *filepath, U8 *basename, U32 u4MaxSize){
    S8 *pDelimitor;
    if( NULL == filepath ||  0 == ext_ucs2strlen((const S8*)filepath) ){
        return EXT_FS_ERR;
    }
    pDelimitor = ext_ucs2rchr((const S8 *)filepath, (U16)0x005c); // "\\" 0x5c
    if( NULL != pDelimitor ){
        ext_ucs2ncpy((S8 *)basename, (const S8 *)pDelimitor+2, u4MaxSize); // skip the delimitor
    }else{
		pDelimitor = ext_ucs2rchr((const S8 *)filepath, (U16) 0x002f); // "\\" 0x5c
		if( NULL != pDelimitor ){
			ext_ucs2ncpy((S8 *)basename, (const S8 *)pDelimitor+2, u4MaxSize); // skip the delimitor
		}else{
			GOEP_Report("[goep][warn] func:get_file_basename cannot find basename\n");
	        return EXT_FS_ERR;
		}
    }
    return     EXT_FS_OK;
}

FS_STATUS btmtk_goep_create_folder(const U16 *pwsFilePath){ ///< FS_CreateDir()
    return ext_fs_create_dir_ucs2(pwsFilePath);
}


FS_STATUS btmtk_goep_rename_file(const U16 *pwsOldFilename, const U16 *pwsNewFilename){
    return ext_fs_rename_file_ucs2(pwsOldFilename, pwsNewFilename);
}


FS_STATUS btmtk_goep_delete_file(const U16 *pwsFilename){
    return ext_fs_delete_ucs2(pwsFilename);
}

FS_STATUS btmtk_goep_delete_folder(const U16 *pwsFilename){
    return ext_fs_delete_dir_ucs2(pwsFilename);
}

FS_STATUS btmtk_goep_move_file(const U16* SrcFullPath, const U16* DstFullPath, U16 Flag){
    //printf(("[fs] move file '%s' to '%s' \n", (const S8 *)SrcFullPath, (const S8 *)DstFullPath); // cannot print ucs2 directly
    return EXT_FS_OK;
}

FS_STATUS btmtk_goep_hide_file(const U16 *pwsFilename){
    return ext_fs_hide_file_ucs2(pwsFilename);
}


FS_STATUS btmtk_goep_unhide_file(const U16 *pwsFilename){
    return ext_fs_unhide_file_ucs2(pwsFilename);
}

BT_BOOL btmtk_goep_is_file_exist(const U16 *pwsFilename){
    return ext_fs_is_file_exist_ucs2(pwsFilename);
}

BT_BOOL btmtk_goep_is_folder_exist(const U16 *pwsFilename){
    return ext_fs_is_dir_exist_ucs2(pwsFilename);
}




/**
* @brief This function is to truncate object name if exceeds max buffer size
* PARAMETERS
* new_name    [OUT]
* old_name    [IN]
* max_len     [IN]
**/
void bt_goep_truncate_ntoh_filename(U16 *dst_name, const U16 *src_netname, U16 max_dst_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 src_name_len;
    U16 *p_ext, *p1;
    U32 ext_len;
    U16 copy_len;
    S8 l_wave[8];
    if( 0 == max_dst_len ){
        GOEP_ASSERT(0);
    }
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    src_name_len = ext_ucs2strlen((const S8*)src_netname);

    // max_dst_len /= ENCODING_LENGTH;
    max_dst_len /= 2; //TODO unicode is 2 byte
     
    if (src_name_len < max_dst_len)
    {
        /* need not truncate */
        ext_ucs2ncpy((S8 *)dst_name, (S8 *) src_netname, max_dst_len);

        p1 = dst_name;
        for(; *p1; p1++)
        {
            *p1 = bt_goep_ntohs(*p1);
        }
        *p1 = (U16)'\0';

    }
    else
    {
        /* need truncate */
        GOEP_MEMSET( (U8*) dst_name, 0, max_dst_len * 2 ); //* ENCODING_LENGTH);
        
        /* find filename ext */
        p_ext = (U16 *)ext_ucs2rchr((const S8 *)(S8 *)src_netname, (U16)(L'.'));
        
        if(p_ext)
        {
            ext_len = ext_ucs2strlen((const S8*)p_ext);
        }
        else
        {
            ext_len = 0;
        }
        
        /* 1 char for '~'; 1 char for \0 */
        copy_len = (U16) (max_dst_len - ext_len - 1 - 1 );
        ext_ucs2ncpy((S8 *) l_wave, (const S8*) L"-", 8);
        p1 = (U16 *)l_wave;
        for(; *p1; p1++)
        {
            *p1 = bt_goep_ntohs(*p1);
        }
        
        
        ext_ucs2ncpy((S8 *)dst_name, (const S8 *)src_netname, copy_len);
        ext_ucs2ncat((S8 *)dst_name, (const S8 *)l_wave, max_dst_len);
        if (p_ext)
        {
            ext_ucs2ncat((S8 *)dst_name, (const S8 *)p_ext, max_dst_len);
        }
        
        p1 = dst_name;
        for(; *p1; p1++)
        {
            *p1 = bt_goep_ntohs(*p1);
        }
        *p1 = (U16)'\0';
        
    }
    
}

/**
* @brief Refer OrigName and create a NewName
* @param OrigName ucs2 filename for reference ex. "Test1.txt"
* @param NewName Output ucs2 filename "Test1
* @param u4MaxSize NewName size in bytes
*/
U32 btmtk_goep_gen_temp_filename(U32 count,const U8 *OrigName, U8 *NewName, U32 u4MaxSize){
    U8 d1, d2, d3;
    U8 buffer[16];
        
    if( 0 == count ){
        ext_ucs2ncpy((S8 *)NewName, (const S8 *)OrigName, u4MaxSize);
        return ext_ucs2strlen((const S8*) NewName );
    }
    
    /// find the "." in Origname and append it with number
    /// ex. Test1.txt ot Test1(dup-xxx).txt

    count = count % 1000; // only support 3 digital
    d1 =  (U8)(count/100);
    d2 =  (U8)( (count - (d1 * 100) )/10);
    d3 =  (U8)(count - (d1 * 100) - ( d2 * 10));
    if( d3 != (count%10) ){
        //ERROR!
        d1 = d2 = d3 = 0;
    }
    buffer[0] = '0';
    buffer[1] = buffer[3] = buffer[5] = buffer[7] = buffer[8]= buffer[9]= 0;
    buffer[2] = '0' + d1;
    buffer[4] = '0' + d2;
    buffer[6] = '0' + d3;  
    ext_ucs2ncpy((S8 *)NewName, "D u p ( ", u4MaxSize);
    ext_ucs2ncat((S8 *)NewName, (const S8 *)buffer, u4MaxSize);
    ext_ucs2ncat((S8 *)NewName, (const S8 *)") ", u4MaxSize);    
    ext_ucs2ncat((S8 *)NewName, (const S8 *)OrigName, u4MaxSize);
    return ext_ucs2strlen((const S8*)NewName);
}


/************************** GOEP Folder/FILENAME  function *********************************/
/**
* @brief Compose Virtual folder. use / as delimitor
* @param Folder1 ucs2 folder path ex. L"\\test"
* @param Folder1 ucs2 filename ex. L"abc\\def"
* @param Filepath Output filepath ex. L"\\test\\abc\\def"
* @param u4MaxSize the size of FilePath in word(character)
*/
U32 btmtk_goep_compose_folderpath(const U8 *Folder1,const U8 *Folder2, U8 *FilePath, U32 u4MaxSize){
    S32 index;
    ext_ucs2ncpy((S8 *)FilePath, (const S8 *)Folder1, u4MaxSize);

    index = ext_ucs2strlen((const S8*) FilePath );
    if( index > 0){
        index--;
        if( (FilePath[index*2] == '\\' ||FilePath[index*2] == '/' ) && FilePath[index*2+1] == 0   ){
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }else if( (Folder1[index*2] == '\\' ||Folder1[index*2] == '/' ) && Folder1[index*2+1] == 0 ) {
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }
    }
    
    ext_ucs2ncat((S8 *)FilePath, (const S8*) L"/", u4MaxSize); // FS_FOLDER_DELIMITOR
    
    index = ext_ucs2strlen((const S8*)Folder2 );
    if( index > 0){
        if( (Folder2[0] == '\\' || Folder2[0] == '/') && Folder2[1] == 0 ){
            Folder2 +=2;
        }else if(  (Folder2[1] == '\\'  || Folder2[1] == '/' )&& Folder2[0] == 0 ){
            Folder2 +=2;
        }else{
        }

        ext_ucs2ncat((S8 *)FilePath, (const S8 *)Folder2, u4MaxSize);
    }

    index = ext_ucs2strlen((const S8*) FilePath );
    if( index >= 0){
        index--;
        if( (FilePath[index*2] == '\\' ||FilePath[index*2] == '/' ) && FilePath[index*2+1] == 0   ){
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }else if( (FilePath[index*2] == '\\' ||FilePath[index*2] == '/' ) && FilePath[index*2+1] == 0 ) {
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }
    }

    return ext_ucs2strlen((const S8*)FilePath);
}



/**
* @brief Compose Folder and Filename to Filepath
* @param Folder ucs2 folder path ex. L"\\est"
* @param Filename ucs2 filename ex. L"Text1.txt"
* @param Filepath Output filepath ex. L"\\est\\Text1.txt"
* @param u4MaxSize the size of FilePath in bytes
* 
*/
U32 btmtk_goep_compose_filepath(const U8 *Folder1, const U8 *Folder2, U8 *FilePath, U32 u4MaxSize){
    S32 index;
    ext_ucs2ncpy((S8 *)FilePath, (const S8 *)Folder1, u4MaxSize);

    index = ext_ucs2strlen((const S8*) FilePath );
    if( index > 0){
        index--;
        if( (FilePath[index*2] == '\\' ||FilePath[index*2] == '/' ) && FilePath[index*2+1] == 0   ){
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }else if( (Folder1[index*2] == '\\' ||Folder1[index*2] == '/' ) && Folder1[index*2+1] == 0 ) {
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }
    }
    
    ext_ucs2ncat( (S8 *)FilePath, (const S8*) "/", u4MaxSize); // FS_FOLDER_DELIMITOR
    
    index = ext_ucs2strlen((const S8*) Folder2 );
    if( index > 0){
        if( (Folder2[0] == '\\' || Folder2[0] == '/') && Folder2[1] == 0 ){
            Folder2 +=2;
        }else if(  (Folder2[1] == '\\'  || Folder2[1] == '/' )&& Folder2[0] == 0 ){
            Folder2 +=2;
        }else{
        }

        ext_ucs2ncat((S8 *)FilePath, (const S8 *)Folder2, u4MaxSize);
    }

    index = ext_ucs2strlen((const S8*) FilePath );
    if( index >= 0){
        index--;
        if( (FilePath[index*2] == '\\' ||FilePath[index*2] == '/' ) && FilePath[index*2+1] == 0   ){
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }else if( (FilePath[index*2] == '\\' ||FilePath[index*2] == '/' ) && FilePath[index*2+1] == 0 ) {
            FilePath[index*2] = FilePath[index*2+1] = 0;
        }
    }

    return ext_ucs2strlen((const S8*)FilePath);
}


/**
* @brief Clean the filepath to empty string
*/
BT_BOOL btmtk_goep_clear_filepath(U8 *filepath){
    if( NULL == filepath  ){
        return FALSE;
    }
    filepath[0] = filepath[1] = 0;
    return TRUE;
}


 
/************************** GOEP DEBUG *********************************/
void GOEP_PORTING_ASSERT(BT_BOOL b){
    U8 i =0;
    if( FALSE == b){
       i =1;
    }
}

const U8* bt_goep_get_id_name(U32 u4MsgId);

void btmtk_goep_show_id( U32 u4EventID, BT_BOOL bSent, U8 user){
#ifdef GOEP_TEST    
    if( user == 2){
        if( bSent ){
            printf( "=>|  ID(0x%04x) Sent %s by %d\n", u4EventID, bt_goep_get_id_name(u4EventID) , user);
        }else{
            printf( "<=|  ID(0x%04x) Recv %s by %d\n", u4EventID, bt_goep_get_id_name(u4EventID) , user);
        }
    }else if ( user == 0){
        if( bSent ){
            printf( "  |<= ID(0x%04x) Sent %s by %d\n", u4EventID, bt_goep_get_id_name(u4EventID) , user);
        }else{
            printf( "  |=> ID(0x%04x) Recv %s by %d\n", u4EventID, bt_goep_get_id_name(u4EventID) , user);
        }
    }else{
        if( bSent ){
            printf( "  =>  ID(0x%04x) Sent %s by %d\n", u4EventID, bt_goep_get_id_name(u4EventID) , user);
        }else{
            printf( "  <=  ID(0x%04x) Recv %s by %d\n", u4EventID, bt_goep_get_id_name(u4EventID) , user);
        }

    }
#endif    
}

void btmtk_goep_sleep_test(U32 interval){
#ifdef BTMTK_ON_WIN32
    Sleep(interval);
#endif
}


const U8* bt_goep_get_id_name(U32 u4MsgId){
#if 0    
   switch(u4MsgId){
        /** GOEP id **/
    /* Server */
    case MSG_ID_BT_GOEPS_REGISTER_SERVER_REQ:
        return "MSG_ID_BT_GOEPS_REGISTER_SERVER_REQ";
        break;
    case MSG_ID_BT_GOEPS_REGISTER_SERVER_CNF:
        return "MSG_ID_BT_GOEPS_REGISTER_SERVER_CNF";
    break;
    case MSG_ID_BT_GOEPS_DEREGISTER_SERVER_REQ:
        return "MSG_ID_BT_GOEPS_DEREGISTER_SERVER_REQ";
    break;
    case MSG_ID_BT_GOEPS_DEREGISTER_SERVER_CNF:
        return "MSG_ID_BT_GOEPS_DEREGISTER_SERVER_CNF";
    break;
    case MSG_ID_BT_GOEPS_CONNECT_IND:
        return "MSG_ID_BT_GOEPS_CONNECT_IND";
    break;
    case MSG_ID_BT_GOEPS_CONNECT_RSP:
        return "MSG_ID_BT_GOEPS_CONNECT_RSP";
    break;
    case MSG_ID_BT_GOEPS_PUSH_IND:
        return "MSG_ID_BT_GOEPS_PUSH_IND";
    break;
    case MSG_ID_BT_GOEPS_PUSH_RSP:
        return "MSG_ID_BT_GOEPS_PUSH_RSP";
    break;  
    case MSG_ID_BT_GOEPS_PULL_IND:
        return "MSG_ID_BT_GOEPS_PULL_IND";
    break;  
    case MSG_ID_BT_GOEPS_PULL_RSP:
        return "MSG_ID_BT_GOEPS_PULL_RSP";
    break;  
    case MSG_ID_BT_GOEPS_SET_FOLDER_IND:
        return "MSG_ID_BT_GOEPS_SET_FOLDER_IND";
    break;  
    case MSG_ID_BT_GOEPS_SET_FOLDER_RSP:
        return "MSG_ID_BT_GOEPS_SET_FOLDER_RSP";
    break;
    case MSG_ID_BT_GOEPS_ABORT_IND:
        return "MSG_ID_BT_GOEPS_ABORT_IND";
    break;  
    case MSG_ID_BT_GOEPS_ABORT_RSP:
        return "MSG_ID_BT_GOEPS_ABORT_RSP";
    break;  
    case MSG_ID_BT_GOEPS_AUTH_REQ:
        return "MSG_ID_BT_GOEPS_AUTH_REQ";
    break;  
    case MSG_ID_BT_GOEPS_AUTH_CNF:
        return "MSG_ID_BT_GOEPS_AUTH_CNF";
    break;  
//    case MSG_ID_BT_GOEPS_TPDISCONNECT_CNF:
    //    return "MSG_ID_BT_GOEPS_TPDISCONNECT_CNF";
    break;
    case MSG_ID_BT_GOEPS_DISCONNECT_IND:
        return "MSG_ID_BT_GOEPS_DISCONNECT_IND";
    break;  
    case MSG_ID_BT_GOEPS_DISCONNECT_RSP:
        return "MSG_ID_BT_GOEPS_DISCONNECT_RSP";
    break; 
    case MSG_ID_BT_GOEPS_TPDISCONNECT_REQ:
        return "MSG_ID_BT_GOEPS_TPDISCONNECT_REQ";
    break;
    case MSG_ID_BT_GOEPS_TPDISCONNECT_IND:
        return "MSG_ID_BT_GOEPS_TPDISCONNECT_REQ";
        break;
    case MSG_ID_BT_GOEPS_AUTH_RSP:
        return "MSG_ID_BT_GOEPS_AUTH_RSP";
    break;  
    case MSG_ID_BT_GOEPS_AUTH_IND:
        return "MSG_ID_BT_GOEPS_AUTH_IND";
    break;  

    /* Client */
    case MSG_ID_BT_GOEPC_REGISTER_CLIENT_REQ:
        return "MSG_ID_BT_GOEPC_REGISTER_CLIENT_REQ";
    break;  
    case MSG_ID_BT_GOEPC_REGISTER_CLIENT_CNF:
        return "MSG_ID_BT_GOEPC_REGISTER_CLIENT_CNF";
    break;  
    case MSG_ID_BT_GOEPC_DEREGISTER_CLIENT_REQ:
        return "MSG_ID_BT_GOEPC_DEREGISTER_CLIENT_REQ";
    break;  
    case MSG_ID_BT_GOEPC_DEREGISTER_CLIENT_CNF:
        return "MSG_ID_BT_GOEPC_DEREGISTER_CLIENT_CNF";
    break;  
    case MSG_ID_BT_GOEPC_CONNECT_REQ:
        return "MSG_ID_BT_GOEPC_CONNECT_REQ";
    break;  
    case MSG_ID_BT_GOEPC_CONNECT_CNF:
        return "MSG_ID_BT_GOEPC_CONNECT_CNF";
    break;  
    case MSG_ID_BT_GOEPC_PUSH_REQ:
        return "MSG_ID_BT_GOEPC_PUSH_REQ";
    break;  
    case MSG_ID_BT_GOEPC_PUSH_CNF:
        return "MSG_ID_BT_GOEPC_PUSH_CNF";
    break;  
    case MSG_ID_BT_GOEPC_PULL_REQ:
        return "MSG_ID_BT_GOEPC_PULL_REQ";
    break;  
    case MSG_ID_BT_GOEPC_PULL_CNF:
        return "MSG_ID_BT_GOEPC_PULL_CNF";
    break;  
    case MSG_ID_BT_GOEPC_SET_FOLDER_REQ:
        return "MSG_ID_BT_GOEPC_SET_FOLDER_REQ";
    break;  
    case MSG_ID_BT_GOEPC_SET_FOLDER_CNF:
        return "MSG_ID_BT_GOEPC_SET_FOLDER_CNF";
    break;  
    case MSG_ID_BT_GOEPC_ABORT_REQ:
        return "MSG_ID_BT_GOEPC_ABORT_REQ";
    break;  
    case MSG_ID_BT_GOEPC_ABORT_CNF:
        return "MSG_ID_BT_GOEPC_ABORT_CNF";
    break;  
    case MSG_ID_BT_GOEPC_DISCONNECT_REQ:
        return "MSG_ID_BT_GOEPC_DISCONNECT_REQ";
    break;  
    case MSG_ID_BT_GOEPC_DISCONNECT_CNF:
        return "MSG_ID_BT_GOEPC_DISCONNECT_CNF";
    break;  
    case MSG_ID_BT_GOEPC_TPDISCONNECT_IND:
        return "MSG_ID_BT_GOEPC_TPDISCONNECT_IND:";
    break;
        
    case MSG_ID_BT_GOEPC_AUTH_IND:
        return "MSG_ID_BT_GOEPC_AUTH_IND";
    break;  
        
    case MSG_ID_BT_GOEPC_AUTH_RSP:
        return "MSG_ID_BT_GOEPC_AUTH_RSP";
    break;  
    case MSG_ID_BT_GOEPC_SUPPORTED_FORMATS_IND:
        return "MSG_ID_BT_GOEPC_SUPPORTED_FORMATS_IND";
    break;  
    case MSG_ID_BT_GOEPS_AUTHORIZE_IND:
        return "MSG_ID_BT_GOEPS_AUTHORIZE_IND:";
    break;
    case MSG_ID_BT_GOEPS_AUTHORIZE_RSP:
        return "MSG_ID_BT_GOEPS_AUTHORIZE_RSP:";
    break;

        /** OPP id **/
    /** OPP SERVER **/
    case MSG_ID_BT_OPPS_REGISTER_SERVER_REQ:
        return "MSG_ID_BT_OPPS_REGISTER_SERVER_REQ";
        break;
    case MSG_ID_BT_OPPS_REGISTER_SERVER_CNF:
        return "MSG_ID_BT_OPPS_REGISTER_SERVER_CNF";
    break;
    case MSG_ID_BT_OPPS_DEREGISTER_SERVER_REQ:
        return "MSG_ID_BT_OPPS_DEREGISTER_SERVER_REQ";
    break;
    case MSG_ID_BT_OPPS_DEREGISTER_SERVER_CNF:
        return "MSG_ID_BT_OPPS_DEREGISTER_SERVER_CNF";
    break;
    case MSG_ID_BT_OPPS_CONNECT_IND:
        return "MSG_ID_BT_OPPS_CONNECT_IND:";
    break;
    case MSG_ID_BT_OPPS_CONNECT_RSP:
        return "MSG_ID_BT_OPPS_CONNECT_RSP";
    break;
    case MSG_ID_BT_OPPS_PUSH_IND:
        return "MSG_ID_BT_OPPS_PUSH_IND";
    break;
    case MSG_ID_BT_OPPS_PUSH_RSP:
        return "MSG_ID_BT_OPPS_PUSH_RSP";
    break;  
    case MSG_ID_BT_OPPS_PULL_IND:
        return "MSG_ID_BT_OPPS_PULL_IND";
    break;  
    case MSG_ID_BT_OPPS_PULL_RSP:
        return "MSG_ID_BT_OPPS_PULL_RSP";
    break;  
    case MSG_ID_BT_OPPS_SET_FOLDER_IND:
        return "MSG_ID_BT_OPPS_SET_FOLDER_IND";
    break;  
    case MSG_ID_BT_OPPS_SET_FOLDER_RSP:
        return "MSG_ID_BT_OPPS_SET_FOLDER_RSP";
    break;
    case MSG_ID_BT_OPPS_ABORT_IND:
        return "MSG_ID_BT_OPPS_ABORT_IND";
    break;  
    case MSG_ID_BT_OPPS_ABORT_RSP:
        return "MSG_ID_BT_OPPS_ABORT_RSP";
    break;  
    case MSG_ID_BT_OPPS_AUTH_REQ:
        return "MSG_ID_BT_OPPS_AUTH_REQ";
    break;  
    case MSG_ID_BT_OPPS_AUTH_CNF:
        return "MSG_ID_BT_OPPS_AUTH_CNF";
    break;  
    case MSG_ID_BT_OPPS_DISCONNECT_IND:
        return "MSG_ID_BT_OPPS_DISCONNECT_IND";
    break;  
    case MSG_ID_BT_OPPS_DISCONNECT_RSP:
        return "MSG_ID_BT_OPPS_DISCONNECT_RSP";
    break;  
    case MSG_ID_BT_OPPS_AUTHORIZE_IND:
        return "MSG_ID_BT_OPPS_AUTHORIZE_IND";
    break;
    case MSG_ID_BT_OPPS_TPDISCONNECT_IND:
        return "MSG_ID_BT_OPPS_TPDISCONNECT_IND";
    break;  
    case MSG_ID_BT_OPPS_AUTHORIZE_RSP:
        return "MSG_ID_BT_OPPS_AUTHORIZE_RSP";
    break;
        
    /** OPP Client **/    
    case MSG_ID_BT_OPPC_REGISTER_CLIENT_REQ:
        return "MSG_ID_BT_OPPC_REGISTER_CLIENT_REQ";
    break;  
    case MSG_ID_BT_OPPC_REGISTER_CLIENT_CNF:
        return "MSG_ID_BT_OPPC_REGISTER_CLIENT_CNF";
    break;  
    case MSG_ID_BT_OPPC_DEREGISTER_CLIENT_REQ:
        return "MSG_ID_BT_OPPC_DEREGISTER_CLIENT_REQ";
    break;  
    case MSG_ID_BT_OPPC_DEREGISTER_CLIENT_CNF:
        return "MSG_ID_BT_OPPC_DEREGISTER_CLIENT_CNF";
    break;  
    case MSG_ID_BT_OPPC_CONNECT_REQ:
        return "MSG_ID_BT_OPPC_CONNECT_REQ";
    break;  
    case MSG_ID_BT_OPPC_CONNECT_CNF:
        return "MSG_ID_BT_OPPC_CONNECT_CNF";
    break;  
    case MSG_ID_BT_OPPC_PUSH_REQ:
        return "MSG_ID_BT_OPPC_PUSH_REQ:";
    break;  
    case MSG_ID_BT_OPPC_PUSH_CNF:
        return "MSG_ID_BT_OPPC_PUSH_CNF";
    break;  
    case MSG_ID_BT_OPPC_PULL_REQ:
        return "MSG_ID_BT_OPPC_PULL_REQ";
    break;  
    case MSG_ID_BT_OPPC_PULL_CNF:
        return "MSG_ID_BT_OPPC_PULL_CNF";
    break;  
    case MSG_ID_BT_OPPC_SET_FOLDER_REQ:
        return "MSG_ID_BT_OPPC_SET_FOLDER_REQ";
    break;  
    case MSG_ID_BT_OPPC_SET_FOLDER_CNF:
        return "MSG_ID_BT_OPPC_SET_FOLDER_CNF";
    break;  
    case MSG_ID_BT_OPPC_ABORT_REQ:
        return "MSG_ID_BT_OPPC_ABORT_REQ";
    break;  
    case MSG_ID_BT_OPPC_ABORT_CNF:
        return "MSG_ID_BT_OPPC_ABORT_CNF";
    break;  
    case MSG_ID_BT_OPPC_DISCONNECT_REQ:
        return "MSG_ID_BT_OPPC_DISCONNECT_REQ";
    break;  
    case MSG_ID_BT_OPPC_DISCONNECT_CNF:
        return "MSG_ID_BT_OPPC_DISCONNECT_CNF";
    break;  
    case MSG_ID_BT_OPPC_AUTH_IND:
        return "MSG_ID_BT_OPPC_AUTH_IND";
    break;  
    case MSG_ID_BT_OPPC_AUTH_RSP:
        return "MSG_ID_BT_OPPC_AUTH_RSP";
    break;  
    case MSG_ID_BT_OPPC_OPP_SUPPORTED_FORMATS_IND:
        return "MSG_ID_BT_OPPC_OPP_SUPPORTED_FORMATS_IND";
    break;  
    case MSG_ID_BT_OPPC_TPDISCONNECT_IND:
        return "MSG_ID_BT_OPPC_TPDISCONNECT_IND";
    break;
    case MSG_ID_BT_OPPS_SERVER_INTERNAL_RW:
        return "MSG_ID_BT_OPPS_SERVER_INTERNAL_RW";
    break;
    case MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW:
        return "MSG_ID_BT_OPPC_CLIENT_INTERNAL_RW";
    break;    

   /** FTP id **/
   /** FTP Server **/
    case MSG_ID_BT_FTPS_REGISTER_SERVER_REQ:
        return "MSG_ID_BT_FTPS_REGISTER_SERVER_REQ";
        break;
    case MSG_ID_BT_FTPS_REGISTER_SERVER_CNF:
        return "MSG_ID_BT_FTPS_REGISTER_SERVER_CNF";
    break;
    case MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ:
        return "MSG_ID_BT_FTPS_DEREGISTER_SERVER_REQ";
    break;
    case MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF:
        return "MSG_ID_BT_FTPS_DEREGISTER_SERVER_CNF";
    break;
    case MSG_ID_BT_FTPS_CONNECT_IND:
        return "MSG_ID_BT_FTPS_CONNECT_IND";
    break;
    case MSG_ID_BT_FTPS_CONNECT_RSP:
        return "MSG_ID_BT_FTPS_CONNECT_RSP";
    break;
    case MSG_ID_BT_FTPS_PUSH_IND:
        return "MSG_ID_BT_FTPS_PUSH_IND";
    break;
    case MSG_ID_BT_FTPS_PUSH_RSP:
        return "MSG_ID_BT_FTPS_PUSH_RSP";
    break;  
    case MSG_ID_BT_FTPS_PULL_IND:
        return "MSG_ID_BT_FTPS_PULL_IND";
    break;  
    case MSG_ID_BT_FTPS_PULL_RSP:
        return "MSG_ID_BT_FTPS_PULL_RSP";
    break;  
    case MSG_ID_BT_FTPS_SET_FOLDER_IND:
        return "MSG_ID_BT_FTPS_SET_FOLDER_IND";
    break;  
    case MSG_ID_BT_FTPS_SET_FOLDER_RSP:
        return "MSG_ID_BT_FTPS_SET_FOLDER_RSP";
    break;
    case MSG_ID_BT_FTPS_ABORT_IND:
        return "MSG_ID_BT_FTPS_ABORT_IND";
    break;  
    case MSG_ID_BT_FTPS_ABORT_RSP:
        return "MSG_ID_BT_FTPS_ABORT_RSP";
    break;  
    case MSG_ID_BT_FTPS_AUTH_REQ:
        return "MSG_ID_BT_FTPS_AUTH_REQ";
    break;  
    case MSG_ID_BT_FTPS_AUTH_CNF:
        return "MSG_ID_BT_FTPS_AUTH_CNF";
    break;  
    case MSG_ID_BT_FTPS_DISCONNECT_IND:
        return "MSG_ID_BT_FTPS_DISCONNECT_IND";
    break;  
    case MSG_ID_BT_FTPS_DISCONNECT_RSP:
        return "MSG_ID_BT_FTPS_DISCONNECT_RSP";
    break;
    case MSG_ID_BT_FTPS_TPDISCONNECT_REQ:
        return "MSG_ID_BT_FTPS_TPDISCONNECT_REQ";
    break;
//    case MSG_ID_BT_FTPS_TPDISCONNECT_CNF:
//        return "MSG_ID_BT_FTPS_TPDISCONNECT_CNF";
    break;

    case MSG_ID_BT_FTPS_AUTHORIZE_IND:
        return "MSG_ID_BT_FTPS_AUTHORIZE_IND";
    break;
    case MSG_ID_BT_FTPS_AUTHORIZE_RSP:
        return "MSG_ID_BT_FTPS_AUTHORIZE_RSP";
    break;
    case MSG_ID_BT_FTPS_TPDISCONNECT_IND:
        return "MSG_ID_BT_FTPS_TPDISCONNECT_IND";
    break;    

    /* FTP Client */
    case MSG_ID_BT_FTPC_REGISTER_CLIENT_REQ:
        return "MSG_ID_BT_FTPC_REGISTER_CLIENT_REQ";
    break;  
    case MSG_ID_BT_FTPC_REGISTER_CLIENT_CNF:
        return "MSG_ID_BT_FTPC_REGISTER_CLIENT_CNF";
    break;  
    case MSG_ID_BT_FTPC_DEREGISTER_CLIENT_REQ:
        return "MSG_ID_BT_FTPC_DEREGISTER_CLIENT_REQ";
    break;  
    case MSG_ID_BT_FTPC_DEREGISTER_CLIENT_CNF:
        return "MSG_ID_BT_FTPC_DEREGISTER_CLIENT_CNF";
    break;  
    case MSG_ID_BT_FTPC_CONNECT_REQ:
        return "MSG_ID_BT_FTPC_CONNECT_REQ";
    break;  
    case MSG_ID_BT_FTPC_CONNECT_CNF:
        return "MSG_ID_BT_FTPC_CONNECT_CNF";
    break;  
    case MSG_ID_BT_FTPC_PUSH_REQ:
        return "MSG_ID_BT_FTPC_PUSH_REQ:";
    break;  
    case MSG_ID_BT_FTPC_PUSH_CNF:
        return "MSG_ID_BT_FTPC_PUSH_CNF";
    break;  
    case MSG_ID_BT_FTPC_PULL_REQ:
        return "MSG_ID_BT_FTPC_PULL_REQ:";
    break;  
    case MSG_ID_BT_FTPC_PULL_CNF:
        return "MSG_ID_BT_FTPC_PULL_CNF";
    break;  
    case MSG_ID_BT_FTPC_SET_FOLDER_REQ:
        return "MSG_ID_BT_FTPC_SET_FOLDER_REQ";
    break;  
    case MSG_ID_BT_FTPC_SET_FOLDER_CNF:
        return "MSG_ID_BT_FTPC_SET_FOLDER_CNF";
    break;  
    case MSG_ID_BT_FTPC_ABORT_REQ:
        return "MSG_ID_BT_FTPC_ABORT_REQ";
    break;  
    case MSG_ID_BT_FTPC_ABORT_CNF:
        return "MSG_ID_BT_FTPC_ABORT_CNF";
    break;  
    case MSG_ID_BT_FTPC_DISCONNECT_REQ:
        return "MSG_ID_BT_FTPC_DISCONNECT_REQ";
    break;  
    case MSG_ID_BT_FTPC_DISCONNECT_CNF:
        return "MSG_ID_BT_FTPC_DISCONNECT_CNF";
    break;
    case MSG_ID_BT_FTPC_AUTH_IND:
        return "MSG_ID_BT_FTPC_AUTH_IND";
    break;  
    case MSG_ID_BT_FTPC_AUTH_RSP:
        return "MSG_ID_BT_FTPC_AUTH_RSP";
    break;  
    case MSG_ID_BT_FTPC_TPDISCONNECT_IND:
        return "MSG_ID_BT_FTPC_TPDISCONNECT_IND";
        break;
    case MSG_ID_BT_FTPS_SERVER_INTERNAL_RW:
        return "MSG_ID_BT_FTPS_SERVER_INTERNAL_RW";
        break;
    case MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW:
        return "MSG_ID_BT_FTPC_CLIENT_INTERNAL_RW";
        break;

    default:
    break;
    }

#ifdef BTMTK_ON_WIN32
    printf("[goepadp] MSG_ID_BT_GOEPC_GROUP_START=%d\n", MSG_ID_BT_GOEPC_GROUP_START);
    printf("[goepadp] MSG_ID_BT_GOEPS_GROUP_START=%d\n", MSG_ID_BT_GOEPS_GROUP_START);
    printf("[oppadp] MSG_ID_BT_OPPC_GROUP_START=%d\n", MSG_ID_BT_OPPC_GROUP_START);
    printf("[oppadp] MSG_ID_BT_OPPS_GROUP_START=%d\n", MSG_ID_BT_OPPS_GROUP_START);
    printf("[ftpadp] MSG_ID_BT_FTPC_GROUP_START=%d\n", MSG_ID_BT_FTPC_GROUP_START);
    printf("[ftpadp] MSG_ID_BT_FTPS_GROUP_START=%d\n", MSG_ID_BT_FTPS_GROUP_START);    
    printf("unknow id is %d\n", u4MsgId);
    //GOEP_ASSERT(0);
#endif    
    return " <error id> ";
#endif
    return (const U8 *)" ";
}


#if 0
void GOEP_TRACE_PATH_FUNC( S32 x, S32 y, const U8 *param, const U8 *unicode_path){
	//U8 asc_path[128];
	//asc_path[128-1]=0;
	//ext_chset_ucs2_to_utf8_string((U8 *)asc_path, 128-1, (const U8 *)unicode_path);
	//kal_trace( x, y, param, asc_path );
}
#endif
