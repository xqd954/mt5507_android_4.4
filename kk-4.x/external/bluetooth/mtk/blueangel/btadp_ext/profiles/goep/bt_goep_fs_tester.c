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
 * bt_goep_fs_tester.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   Test the opp client, server, fs export function
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
#ifdef BTMTK_ON_WIN32
#ifdef GOEP_TEST
#include "windows.h"

//#include "xatypes.h"
#include "bt_goep_porting.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_struct.h"
#include "bt_os_api.h"
#include "ext_osal.h"
#include "bt_nbuffer.h"
//#include "goep.h"

#include "bt_oppc_api.h"
#include "bt_opps_api.h"
#include "bt_oppc_hdl.h"
#include "bt_opps_hdl.h"
#include "bt_goep_fs.h"
#include "bt_opp_comm.h"

#include "stdio.h"
#include "io.h"

#include "bt_goep_fs.h"

#define OPPS_TESTER_WRITE_FILENAME1  L"TEST1.TXT"
#define OPPS_TESTER_WRITE_FILENAME2  L"TEST2.TXT"
#define OPPS_TESTER_WRITE_FILEPATH1  L"d:\\est.txt"

extern FS_STATUS bt_goep_clean_tempfile(FHANDLE fh ,U8 *FilePath, BT_BOOL close_only);

///// Export for writing. High Level API

void myASSERT(BT_BOOL b){
    if( TRUE != b){
        b = TRUE;
    }
}

/**
* @brief Test the open vcard and read vcard
*/
static BT_BOOL opps_vcard_tester(){
    FHANDLE fh;
    U8 buffer[128];
    S32 len = 0;
    
    fh = btmtk_opp_open_vcardstream();
    if( FHANDLE_INVALID_VALUE == fh){
        GOEP_ASSERT(0);
    }else{
        if( EXT_FS_OK != btmtk_opp_read_vcard(fh, buffer, 128, &len) ){
            GOEP_ASSERT(0);
        }

        if( 0 == btmtk_opp_get_vcardsize() ){
            GOEP_ASSERT(0);
        }        
        
        if( EXT_FS_OK != btmtk_opp_close_vcard(fh) ){
            GOEP_ASSERT(0);
        }
    }
    return TRUE;
}

/**
* @brief Test to write two file
*/
static BT_BOOL opps_writefile_tester(){
    FHANDLE fh1, fh2;
   
    U8 buffer[128];
    S32 len = 0;
    U8 count = 0;
    
    fh1 = btmtk_goep_open_wstream( OPPS_TESTER_WRITE_FILENAME1, EXT_FS_CREATE_ALWAYS);
    if( FHANDLE_INVALID_VALUE == fh1 ){
        GOEP_ASSERT(0);
    }
    fh2 = btmtk_goep_open_wstream( OPPS_TESTER_WRITE_FILENAME2, EXT_FS_CREATE_ALWAYS);
    if( FHANDLE_INVALID_VALUE == fh2 ){
        GOEP_ASSERT(0);
        if( FALSE == btmtk_goep_close_rstream(fh1) ){
            GOEP_ASSERT(0);
        }        
    }
    
    if( fh1 == fh2 ){
        GOEP_ASSERT(0);
    }
    
    // write test
    GOEP_MEMSET(buffer, 'a', 128);
    for(count = 1; count < 128; count++){
        if( EXT_FS_OK != btmtk_goep_write_wstream(fh1, buffer, count, &len) ){
            GOEP_ASSERT(0);
        }else{
            if( count != len ){
                GOEP_ASSERT(0);
            }
        }
    }
    for(count = 1; count < 128; count++){
        if( EXT_FS_OK != btmtk_goep_write_wstream(fh2, buffer, count, &len) ){
            GOEP_ASSERT(0);
        }else{
            if( count != len ){
                GOEP_ASSERT(0);
            }
        }
    }
    

    if( FALSE == btmtk_goep_close_rstream(fh1) ){
        GOEP_ASSERT(0);
    }
    if( FALSE == btmtk_goep_close_rstream(fh2) ){
        GOEP_ASSERT(0);
    }
    
}

/**
* @brief Test to use temp file for avoiding dumplicat file
*/
static BT_BOOL opps_tempfile_tester(){
    FHANDLE fh1, fh2;
    U8 buffer[128], outfile1[128], outfile2[128];
    S32 len = 0;
    U8 count = 0;
    
    if( FALSE  == DeleteFileA( "d:\\temp\\opp\\test1.txt" ) ){
        printf( "[fs] err \n");
    }


    fh1 = bt_opps_util_gen_temp_obj( OPPS_TESTER_WRITE_FILENAME1, outfile1, 128);
    if( FHANDLE_INVALID_VALUE == fh1 ){
        GOEP_ASSERT(0);
    }else{
        // write test
        GOEP_MEMSET(buffer, 'a', 128);
        for(count = 1 ; count < 128; count++){
            if( EXT_FS_OK != btmtk_goep_write_wstream(fh1, buffer, count, &len) ){
                GOEP_ASSERT(0);
            }else{
                if( count != len ){
                    GOEP_ASSERT(0);
                }
            }
        }
        
        if( EXT_FS_OK != bt_goep_clean_tempfile(fh1 , outfile1, TRUE) ){
            GOEP_ASSERT(0);
        } // fh1 is close
        fh1 = FHANDLE_INVALID_VALUE;
    }
    
    fh2 = bt_opps_util_gen_temp_obj( OPPS_TESTER_WRITE_FILENAME1 , outfile2, 128);
    for(count = 1; count < 128 && fh2 != FHANDLE_INVALID_VALUE; count++){ 
        GOEP_MEMSET(buffer, 'b', 128);
        if( EXT_FS_OK != btmtk_goep_write_wstream(fh2, buffer, count, &len) ){
            GOEP_ASSERT(0);
        }else{
            if( count != len ){
                GOEP_ASSERT(0);
                break;
            }
        }
    }
    
    // has been clean tmp obj
    //if( FHANDLE_INVALID_VALUE != fh1 && FALSE == btmtk_goep_close_rstream(fh1) ){
    //    GOEP_ASSERT(0);
    //}
    if( FHANDLE_INVALID_VALUE != fh2 && FALSE == btmtk_goep_close_rstream(fh2) ){
        GOEP_ASSERT(0);
    }

    // clean up
    if( btmtk_goep_is_file_exist(outfile1) ){
        if( EXT_FS_OK != btmtk_goep_delete_file(outfile1) ){
            GOEP_ASSERT(0);
        }else{
            if( btmtk_goep_is_file_exist(outfile1) ){
                GOEP_ASSERT(0);
            }
        }
    }
    if( btmtk_goep_is_file_exist(outfile2) ){
        if( EXT_FS_OK != btmtk_goep_delete_file(outfile2) ){
            GOEP_ASSERT(0);
        }else{
            if( btmtk_goep_is_file_exist(outfile2) ){
                GOEP_ASSERT(0);
            }
        }
    }}

void oppc_read_file_tester(){
    FHANDLE fh;
    U8 buffer[128];
    S32 len = 0;
    
    fh = btmtk_goep_open_rstream( L"c:\\autoexec.bat", 0);
    if( FHANDLE_INVALID_VALUE == fh){
        GOEP_ASSERT(0);
    }else{
        if( EXT_FS_OK != btmtk_goep_read_rstream(fh, buffer, 128, &len) ){
            GOEP_ASSERT(0);
        }
        
        if( FALSE == btmtk_goep_close_rstream(fh) ){
            GOEP_ASSERT(0);
        }
    }
    return TRUE;    
}

void oppc_save_file_tester(){
    //GOEP_ASSERT(0);
}

void goep_compose_path_tester(){
    U8 ucFolderpath[128];
    U8 ucFilepath[128];
        
    if( FALSE == btmtk_goep_compose_folderpath(L"", L"abc", ucFolderpath, 128) ){
         GOEP_ASSERT(0);
    }
    if( 0 != ext_ucs2cmp("\\abc", ucFolderpath ) ){
        
    }
    if( FALSE == btmtk_goep_compose_filepath(ucFolderpath, "abc.txt", ucFilepath, 128 )){
         GOEP_ASSERT(0);
    }
    if( 0 != ext_ucs2cmp("\\abc\\abc.txt", ucFilepath ) ){
        
    }    


    if( FALSE == btmtk_goep_compose_folderpath(L"\\abc", L"\\def", ucFolderpath, 128) ){
         GOEP_ASSERT(0);
    }
    if( 0 != ext_ucs2cmp(L"\\abc\\def", ucFolderpath ) ){
         GOEP_ASSERT(0);
    }
    if( FALSE == btmtk_goep_compose_folderpath(L"\\abc\\", L"\\def", ucFolderpath, 128) ){
         GOEP_ASSERT(0);
    }
    if( 0 != ext_ucs2cmp(L"\\abc\\def", ucFolderpath ) ){
         GOEP_ASSERT(0);
    }
    if( FALSE == btmtk_goep_compose_folderpath(L"\\abc\\", L"def", ucFolderpath, 128) ){
         GOEP_ASSERT(0);
    }
    
    if( 0 != ext_ucs2cmp(L"\\abc\\def", ucFolderpath ) ){
         GOEP_ASSERT(0);
    }
    
    if( FALSE == btmtk_goep_compose_filepath(ucFolderpath, L"abc.txt", ucFilepath, 128 )){
         GOEP_ASSERT(0);
    }
    if( 0 != ext_ucs2cmp(L"\\abc\\def\\abc.txt", ucFilepath ) ){
         GOEP_ASSERT(0);
    }    
     
}



BT_BOOL ftps_file_tester()
{
    U8 ucFolderpath[128];
    U8 ucFilepath[128];
    U32 iWritten;
    FHANDLE fh;

    fh = btmtk_goep_open_wstream( OPPS_TESTER_WRITE_FILEPATH1 , 0);
    if( fh !=  FHANDLE_INVALID_VALUE ){
        btmtk_goep_write_wstream( fh, "123", 2, &iWritten);
        btmtk_goep_close_rstream(fh);
        if( EXT_FS_OK != btmtk_goep_delete_file( OPPS_TESTER_WRITE_FILEPATH1 ) ){
            GOEP_ASSERT(0);
        }
    }else{
        GOEP_ASSERT(0);
    }

    fh = btmtk_goep_open_wstream( OPPS_TESTER_WRITE_FILEPATH1 , 0);
    if( fh !=  FHANDLE_INVALID_VALUE ){
        btmtk_goep_write_wstream( fh, "123", 2, &iWritten);
        btmtk_goep_close_rstream(fh);
        if( EXT_FS_OK != btmtk_goep_delete_file( OPPS_TESTER_WRITE_FILEPATH1 ) ){
            GOEP_ASSERT(0);
        }
    }else{
        GOEP_ASSERT(0);
    }


    ucFilepath[0] = ucFilepath[1] = 0;
    if( 0 != ext_ucs2strlen((const S8*)ucFilepath)){
        GOEP_ASSERT(0);
    }    
    if( FALSE ==  btmtk_ftps_get_xml_filepath(0, ucFilepath, 128) ){
         GOEP_ASSERT(0);
    }
    if( 0 == ext_ucs2strlen((const S8*)ucFilepath)){
        GOEP_ASSERT(0);
    }

    ucFolderpath[0] = ucFolderpath[1] = 0;
    if( 0 != ext_ucs2strlen((const S8*)ucFolderpath)){
        GOEP_ASSERT(0);
    }    
    if( FALSE ==  btmtk_get_default_root_folder_path(ucFolderpath, 128) ){
         GOEP_ASSERT(0);
    }
    if( 0 == ext_ucs2strlen((const S8*)ucFolderpath)){
        GOEP_ASSERT(0);
    }

    if( FALSE == btmtk_goep_is_folder_exist( ucFolderpath ) ){
        GOEP_ASSERT(0);
    }
    
}


BT_BOOL ftps_xml_tester(){
    U8 ucFolderpath[128]; // ftp root folderpath
    U8 ucFilepath[128]; // xml filepath
    FHANDLE fh;
    U32 i4Read, i4Total;
    U8 Buffer[128];

    if( FALSE ==  btmtk_ftps_get_xml_filepath(0, ucFilepath, 128) ){
         GOEP_ASSERT(0);
    }

    if( FALSE ==  btmtk_get_default_root_folder_path(ucFolderpath, 128) ){
         GOEP_ASSERT(0);
    }
    
    if( EXT_FS_ERR != bt_ftps_gen_folder_content(0, TRUE, L"" , L"") ){
        GOEP_ASSERT(0);
    }

    ext_ucs2ncpy((S8 *) ucFolderpath, L"d:\\temp", sizeof(ucFolderpath) );
    if( EXT_FS_OK != bt_ftps_gen_folder_content(0, TRUE, ucFolderpath, ucFilepath ) ){
        GOEP_ASSERT(0);
    }

    if( EXT_FS_OK != bt_ftps_gen_folder_content(0, TRUE, ucFolderpath, ucFilepath ) ){
        GOEP_ASSERT(0);
    }else{
    
        fh = btmtk_goep_open_rstream(ucFilepath, 0);
        if( FALSE == btmtk_goep_is_valid_handle(fh ) ){
            GOEP_ASSERT(0);
        }else{

            if( FALSE == btmtk_goep_get_filesize(fh, ucFilepath, &i4Total) || 0 == i4Total){
                GOEP_ASSERT(0);
            }
            for( ; i4Total > 0; ){
            
                if( EXT_FS_OK != btmtk_goep_read_rstream(fh,Buffer, sizeof(Buffer), &i4Read) ){
                    GOEP_ASSERT(0);
                    break;
                }

                i4Total -= i4Read;
                /// print out the 
                ucFolderpath[127] = 0;
                //printf("[%d] %s\n", i4Read, ucFolderpath);/// read ascii file ?!
            }
    
    
            btmtk_goep_close_rstream(fh);
        }

    //TODO parse the xml file and delete it !
    }


}

BT_BOOL goep_folder_test(){
    U8 ucFolderpath[128]; // ftp root folderpath
    U8 ucNewFolderpath[128]; // ftp root folderpath

    ext_ucs2ncpy((S8 *) ucFolderpath, L"\\abc", sizeof(ucFolderpath) );
    if( FALSE == btmtk_ftp_util_get_parent_folder( ucFolderpath, ucNewFolderpath, sizeof(ucFolderpath) ) ){
        GOEP_ASSERT(0);
    }
    ext_ucs2ncpy((S8 *) ucFolderpath, L"", sizeof(ucFolderpath) );
    if( 0 != ext_ucs2cmp(ucFolderpath, ucNewFolderpath) && ext_ucs2strlen((const S8*)ucFolderpath) != ext_ucs2strlen((const S8*)ucNewFolderpath) ){
        GOEP_ASSERT(0);
       }

    ext_ucs2ncpy((S8 *) ucFolderpath, L"\\abc\\def", sizeof(ucFolderpath) );
    if( FALSE == btmtk_ftp_util_get_parent_folder( ucFolderpath, ucNewFolderpath, sizeof(ucFolderpath) ) ){
        GOEP_ASSERT(0);
    }
    ext_ucs2ncpy((S8 *) ucFolderpath, L"\\abc", sizeof(ucFolderpath) );
    if( 0 != ext_ucs2cmp(ucFolderpath, ucNewFolderpath) && ext_ucs2strlen((const S8*)ucFolderpath) != ext_ucs2strlen((const S8*)ucNewFolderpath) ){
        GOEP_ASSERT(0);
       }

    ext_ucs2ncpy((S8 *) ucFolderpath, L"\\abc\\def\\xxx", sizeof(ucFolderpath) );
    if( FALSE == btmtk_ftp_util_get_parent_folder( ucFolderpath, ucNewFolderpath, sizeof(ucFolderpath) ) ){
        GOEP_ASSERT(0);
    }
    ext_ucs2ncpy((S8 *) ucFolderpath, L"\\abc\\def", sizeof(ucFolderpath) );
    if( 0 != ext_ucs2cmp(ucFolderpath, ucNewFolderpath) && ext_ucs2strlen((const S8*)ucFolderpath) != ext_ucs2strlen((const S8*)ucNewFolderpath) ){
        GOEP_ASSERT(0);
       }    
}

BT_BOOL goep_create_folder_tester(){
    U8 ucFolderpath[128]; // ftp root folderpath
    ext_ucs2ncpy((S8 *) ucFolderpath, L"d:\\temp\\aaa" , sizeof(ucFolderpath));

    btmtk_goep_delete_folder(ucFolderpath);
    
    if( FALSE != btmtk_goep_is_folder_exist(ucFolderpath) ){
        GOEP_ASSERT(0);
    }

    btmtk_goep_create_folder(ucFolderpath);
    
    if( FALSE == btmtk_goep_is_folder_exist(ucFolderpath) ){
        GOEP_ASSERT(0);
    }
    
    btmtk_goep_delete_folder(ucFolderpath);
    
    if( FALSE != btmtk_goep_is_folder_exist(ucFolderpath) ){
        GOEP_ASSERT(0);
    }    
}

#define FILENAME_SIZE 26
void ftpc_filename_tester(){
    U8 filename[FILENAME_SIZE];
    U8 obj_name[FILENAME_SIZE];
    filename[0] = filename[1] = 0x5a;
    obj_name[0] = obj_name[1] = 0x5a;
    obj_name[FILENAME_SIZE-1] = obj_name[FILENAME_SIZE-2] = 0x5a;
    memset( filename, 0x5a, FILENAME_SIZE);
    memset( obj_name, 0x5a, FILENAME_SIZE);

    ext_ucs2ncpy((S8 *) filename+2, L"c:\\est.txt", (FILENAME_SIZE-4)/2 );
    btmtk_goep_get_file_basename(filename+2, obj_name+2, (FILENAME_SIZE-4)/2);

    ext_ucs2ncpy((S8 *) filename+2, L"c:\\windows\\est.txt", (FILENAME_SIZE-4)/2 );
    btmtk_goep_get_file_basename(filename+2, obj_name+2, (FILENAME_SIZE-4)/2);
    
    if(filename[0] != 0x5A || filename[1] != 0x5A ){
        GOEP_ASSERT(0);
    }
    
    if(filename[FILENAME_SIZE-2] != 0x5A || filename[FILENAME_SIZE-1] != 0x5A ){
        GOEP_ASSERT(0);
    }

    if(obj_name[0] != 0x5A || obj_name[1] != 0x5A ){
        GOEP_ASSERT(0);
    }    


    if(obj_name[FILENAME_SIZE-2] != 0x5A || obj_name[FILENAME_SIZE-1] != 0x5A ){
        GOEP_ASSERT(0);
    }    
}

void btmtk_nbuffer_tester(){
    NBUFFER_MEM_POLL nbuffer;
    char data[100];
    char *pdata;
    U32 index, buffersize;

    if( FALSE == nbuffer_init(&nbuffer, 100) ){
        GOEP_ASSERT(0);
    }else{
        if( FALSE == nbuffer_isInit(&nbuffer) ){
            GOEP_ASSERT(0);
        }else if( FALSE == nbuffer_isEmpty(&nbuffer) ){
            GOEP_ASSERT(0);

        }else{
            if( FALSE != nbuffer_isFull(&nbuffer) ){
                GOEP_ASSERT(0);
            }

            pdata = nbuffer_get_tail_buffer(&nbuffer, &index, &buffersize);
            if( NULL == pdata || buffersize != 100 ){
                GOEP_ASSERT(0);
            }
            //memcpy( pdata, "1234567890", 10);
            memset( pdata, 'a', 100);
            nbuffer_return_tail_buffer( &nbuffer, index, 10);
            pdata = nbuffer_get_tail_buffer(&nbuffer, &index, &buffersize);
            if( NULL == pdata || buffersize != 100 ){
                GOEP_ASSERT(0);
            }

            if( FALSE != nbuffer_isFull(&nbuffer) || FALSE != nbuffer_isEmpty(&nbuffer) ){
                GOEP_ASSERT(0);
            }

            //memcpy( pdata, "1234567890", 10);
            memset( pdata, 'b', 100);
            nbuffer_return_tail_buffer( &nbuffer, index, 20);
            pdata = nbuffer_get_tail_buffer(&nbuffer, &index, &buffersize);
            if( NULL == pdata || buffersize != 100 ){
                GOEP_ASSERT(0);
            }
            //memcpy( pdata, "1234567890", 10);
            memset( pdata, 'c', 100);
            nbuffer_return_tail_buffer( &nbuffer, index, 30);

            /// read check
            memcpy( data, "0123456789", 10);
            if( FALSE == nbuffer_pop_data(&nbuffer, data, 100, &buffersize) ){
                GOEP_ASSERT(0);
            }
            if( buffersize != 10 || data[0] != 'a' || data[9] != 'a'){
                GOEP_ASSERT(0);
            }
            memcpy( data, "0123456789", 10);
            if( FALSE == nbuffer_pop_data(&nbuffer, data, 100, &buffersize) ){
                GOEP_ASSERT(0);
            }
            if( buffersize != 20 || data[0] != 'b' || data[9] != 'b'){
                GOEP_ASSERT(0);
            }
            memcpy( data, "0123456789", 10);
            if( FALSE == nbuffer_pop_data(&nbuffer, data, 100, &buffersize) ){
                GOEP_ASSERT(0);
            }
            if( buffersize != 30 || data[0] != 'c' || data[9] != 'c'){
                GOEP_ASSERT(0);
            }

            if( FALSE == nbuffer_isEmpty(&nbuffer) ){
                GOEP_ASSERT(0);
            }


            if( FALSE == nbuffer_deinit(&nbuffer) ){
                GOEP_ASSERT(0);
            }
        }
    }
}

void btmtk_oppsfs_tester(){
    ///offer a pull file
    opps_vcard_tester();
    ///save pushing file
    opps_writefile_tester();
    opps_tempfile_tester();
}



void btmtk_oppcfs_tester(){
    ///offer a push file
    oppc_read_file_tester();
    ///save pull file (vcard)
    oppc_save_file_tester(); /// should be able to rename after receive
    ftpc_filename_tester();
}


void btmtk_ftpsfs_tester(){
    goep_compose_path_tester();
    goep_folder_test();
    ftps_file_tester();
    ftps_xml_tester();
    goep_create_folder_tester();
}

void btmtk_goep_tester(){
    btmtk_oppcfs_tester();
    btmtk_oppsfs_tester();
    btmtk_ftpsfs_tester();
    btmtk_nbuffer_tester();
}
#endif
#endif
