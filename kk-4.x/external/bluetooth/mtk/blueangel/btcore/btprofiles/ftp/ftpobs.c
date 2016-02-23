/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#if defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__)

#include "ftp.h"
#include "ftpobs.h"

#include "bt_adp_fs.h" /* use adp_fs as os fs api */
#include "bt_adp_fs_ucs2.h" /* use adp_fs as os fs api */

#include "bt_os_api.h" /* os api */
#include "bt_adp_system.h" /* os api */

#include "bt_ucs2.h" /* os api */

#ifdef __BT_OBEX_TUNING__
// Remove trace for tuning performance of GOEPv2
#ifdef kal_trace
#undef kal_trace
#define kal_trace
#endif
#endif

#define BT_FTP_SUPPORT_INTERNAL_RW 1
#define FTP_GUIDE_BYTE 1
//#define _FTPOBS_DEBUG_ 1

ObexRespCode FTPOBS_ReadToBuffer(void *obs, U16 len);
ObexRespCode FTPOBS_Flush(FtpcObStoreHandle obsh);
extern void ftps_send_internal_rw_ind_msg(U8 goep_conn_id, U8 param);
extern void ftpc_send_internal_rw_ind_msg(U8 goep_conn_id, U8 param);
#ifdef __BT_GOEP_V2__
// DEBUG: for testing sending data without actual reading files
//#define FTP_FAKE_READ 1
// DEBUG: for testing write data without actual write to file
//#define FTP_FAKE_WRITE 1
static int g_fake_read_index = 1;

/* obs object store functions */
#ifndef FTPAssert
void FTPAssert(BOOL istrue){
	if( TRUE != istrue ){
		*(int *)1 = 0;
	}
}
#endif

BOOL ftpobs_util_compareFileContent(U32 *filepath, U8 *buff, U32 len){
	// read file from filepath and compare the data with buff & len
	BOOL ret = FALSE;
	FHANDLE fh;
	S32 read;
	U8 data[2];
	U32 pos;
	S32 filesize;
	filesize = btmtk_fs_get_filesize((const U8*)filepath);
	if( len != (U32)filesize ){
		FTPAssert(0);
	}
	
	fh = btmtk_fs_open_ucs2((const U16*)filepath, BTMTK_FS_READ_ONLY);
	if( fh <= 0 ){
		FTPAssert(0);
	}
	for( pos = 0; pos < len; pos++ ){
		if( btmtk_fs_read_ucs2(fh, data, 1, &read) ){
			FTPAssert(0);
		}
		if( buff[pos] != data[0] ){
			FTPAssert(0);
		}
	}
	btmtk_fs_close_ucs2(fh);
	ret = TRUE;
	
	return ret;
}


ObexRespCode FTPOBS_TestMemBuffer_Read(){
	ObexRespCode        ret = OBRC_FORBIDDEN;
	U8 *buff;
	U8 *buff_file;
	U32 i = 0 , pos, len1, len2, len3;
	U32 blocksize;
	U32 testsize;
	U8 filepath[128];
	FtpcObStoreHandle handler;
	BOOL l_useMem = TRUE;

	//U8 *l_pfilepath = L"/mnt/sdcard/test_one_write.txt";
	U8 *l_pfilepath = (U8*)L"c:\\temp\\1\\1k.txt";
	U32 MAXSIZE = FTP_INTERNAL_RW_BUFFER_SIZE ;

	buff = (U8 *) malloc( (MAXSIZE*3+10) );
	buff_file = (U8 *) malloc( (MAXSIZE*3+10) );

	if( buff == NULL || buff_file == NULL ){
		FTPAssert(0);
		return ret;
	}

	// prepare
	handler = FTPOBS_New(NULL, TRUE, l_useMem, 0);
	if( handler == NULL ){
		FTPAssert(0);
	}
	FTPOBS_AppendFilePath( handler, l_pfilepath, bt_ucs2strlen((const S8*)l_pfilepath)*2 );
	if( OBRC_SUCCESS != FTPOBS_Open(&handler, FALSE) ){
		FTPAssert(0);
	}
	if( OBRC_SUCCESS != FTPOBS_Read( handler, buff, MAXSIZE* 2) ){
		FTPAssert(0);
	}
	FTPOBS_Close( (FtpcObStoreHandle *) &handler, FALSE); // not to delete it

#if 1
	// start test write one time
	for(pos = 0; pos < MAXSIZE; pos++)
	{
		// prepare
		for( len1 = 1; len1 < (MAXSIZE); len1++){
				for( blocksize = 1; blocksize < (MAXSIZE+5) ; blocksize++ ){
				handler = FTPOBS_New(NULL, TRUE, l_useMem, 0);
				if( handler == NULL ){
					FTPAssert(0);
				}
				FTPOBS_AppendFilePath( handler, l_pfilepath, bt_ucs2strlen((const S8*)l_pfilepath)*2 );
				if( OBRC_SUCCESS != FTPOBS_Open(&handler, FALSE) ){
					FTPAssert(0);
				}
				handler->membuffer_start = (MAXSIZE-pos-1); // move to position X


				if( OBRC_SUCCESS != FTPOBS_ReadToBuffer( handler, len1 ) ){
					FTPAssert(0);
				}
				// check point 1
				if( handler->membuffer_len != len1 || handler->membuffer_start != (MAXSIZE-pos-1)){
					FTPAssert(0);
				}

				// read data into local buffer2
				if( OBRC_SUCCESS != FTPOBS_Read( handler, buff_file, blocksize ) ){
					FTPAssert(0);
				}


				FTPOBS_Flush( handler );
				FTPOBS_Close( (FtpcObStoreHandle *) &handler, FALSE); // not to delete it

				// compare . read all data to buffer2
				if( 0 != memcmp( buff, buff_file, blocksize) ){
					FTPAssert(0);
				}
			}			
		}
	}
#endif

	free(buff_file);
	free(buff);
	return ret;
}

ObexRespCode FTPOBS_TestMemBuffer(){
	ObexRespCode        ret = OBRC_FORBIDDEN;
	U8 *buff;
	U32 i = 0 , pos, len1, len2, len3;
	U32 testsize;
	U8 filepath[128];
	BOOL l_useMem = TRUE;
	FtpcObStoreHandle handler;
	//U8 *l_pfilepath = L"/mnt/sdcard/test_one_write.txt";
	U8 *l_pfilepath = (U8*)L"c:\\temp\\1\\1.txt";
	U32 MAXSIZE = FTP_INTERNAL_RW_BUFFER_SIZE ;

	buff = (U8 *) malloc( (MAXSIZE*3+10) );
	if( buff == NULL ){
		return ret;
	}

	// preapare data
	for( pos = 0; pos < (MAXSIZE*3+10); pos++){
		buff[pos] = (U8) (pos & 0xff);
	}
#if 0
	// start test write one time
	for(pos = 0; pos < MAXSIZE; pos++)
	{
		// prepare
		for( len1 = 1; len1 < (MAXSIZE+2); len1++){
			handler = FTPOBS_New(NULL, TRUE, l_useMem, 0);
			if( handler == NULL ){
				FTPAssert(0);
			}
			FTPOBS_AppendFilePath( handler, l_pfilepath, bt_ucs2strlen(l_pfilepath)*2 );
			if( OBRC_SUCCESS != FTPOBS_Open(&handler, TRUE) ){
				FTPAssert(0);
			}
			handler->membuffer_start = (MAXSIZE-pos-1); // move to position X

			if( OBRC_SUCCESS != FTPOBS_Write(handler, buff, len1) ){
				FTPAssert(0);
			}
			
			FTPOBS_Flush( handler );
			FTPOBS_Close( (FtpcObStoreHandle *) &handler, FALSE); // not to delete it

			// compare
			if( TRUE != ftpobs_util_compareFileContent(l_pfilepath, buff, len1) ){
				FTPAssert(0);
			}else{
				printf("[testbuffer] pass pos:%d len:%d\n", pos, len1);
			}
		}
	}
#endif
#if 1
	// start write twice
	for(pos = 0; pos < MAXSIZE; pos++)
	{
		// prepare
		for( len1 = 1; len1 < (MAXSIZE+2); len1++){
			for( len2 = 1; len2 < (MAXSIZE+2); len2++){
			// del the file
			btmtk_fs_delete_ucs2((const U16*)l_pfilepath);
			handler = FTPOBS_New(NULL, TRUE, l_useMem, 0);
			if( handler == NULL ){
				FTPAssert(0);
			}
			FTPOBS_AppendFilePath( handler, l_pfilepath, bt_ucs2strlen((const S8*)l_pfilepath)*2 );
			if( OBRC_SUCCESS != FTPOBS_Open(&handler, TRUE) ){
				FTPAssert(0);
			}
			handler->membuffer_start = (MAXSIZE-pos-1); // move to position X

			if( OBRC_SUCCESS != FTPOBS_Write(handler, &buff[0], len1) ){
				FTPAssert(0);
			}
			if( OBRC_SUCCESS != FTPOBS_Write(handler, &buff[len1], len2) ){
				FTPAssert(0);
			}			

			FTPOBS_Flush( handler );
			FTPOBS_Close( (FtpcObStoreHandle *) &handler, FALSE); // not to delete it

			// compare
			if( TRUE != ftpobs_util_compareFileContent((U32*)l_pfilepath, buff, len1+len2) ){
				FTPAssert(0);
			}else{
				printf("[testbuffer] pass pos:%u len:%u\n", (unsigned int)pos, (unsigned int)len1);
			}
			}
		}
	}
#endif


// start write twice
#if 1
	for(pos = 0; pos < MAXSIZE; pos++)
	{
		// prepare
		for( len1 = 1; len1 < (MAXSIZE+2); len1++){
			for( len2 = 1; len2 < (MAXSIZE+2); len2++){
				for( len3 = 1; len3 < (MAXSIZE+2); len3++){
			handler = FTPOBS_New(NULL, TRUE, l_useMem, 0);
			if( handler == NULL ){
				FTPAssert(0);
			}
			FTPOBS_AppendFilePath( handler, l_pfilepath, bt_ucs2strlen((const S8*)l_pfilepath)*2 );
			if( OBRC_SUCCESS != FTPOBS_Open(&handler, TRUE) ){
				FTPAssert(0);
			}
			handler->membuffer_start = (MAXSIZE-pos-1); // move to position X

			if( OBRC_SUCCESS != FTPOBS_Write(handler, &buff[0], len1) ){
				FTPAssert(0);
			}
			if( OBRC_SUCCESS != FTPOBS_Write(handler, &buff[len1], len2) ){
				FTPAssert(0);
			}			

			if( OBRC_SUCCESS != FTPOBS_Write(handler, &buff[len1+len2], len3) ){
				FTPAssert(0);
			}			

			FTPOBS_Flush( handler );
			FTPOBS_Close( (FtpcObStoreHandle *) &handler, FALSE); // not to delete it

			// compare
			if( TRUE != ftpobs_util_compareFileContent((U32*)l_pfilepath, buff, len1+len2+len3) ){
				FTPAssert(0);
			}else{
				printf("[testbuffer] pass pos:%u len:%u len2:%u len3:%u\n", (unsigned int)pos, (unsigned int)len1, (unsigned int)len2, (unsigned int)len3);
			}
				}
			}
		}
	}
#endif

	free(buff);
	return ret;
}


BOOL FTPOBS_Init(){
    U8 i;
	GOEP_FLOW("[FTPCFLOW] FTPOBS_Init ");

	/* init gftpobs context to initial */
	/* initalize the Entry Lst and put it to avaiable storeobject list */

    /* gFtp.obcList for client */
	InitializeListHead( &FTP(obcList) );
    for( i =0; i < BT_FTP_MAX_OBS_NUMBER ; i++){ // porting: defined in bluetooth_struc.h
        OS_MemSet( (U8 *) &FTP(obcEntries)[i] , 0, sizeof(FtpcObStoreEntry));
		InsertTailList( &FTP(obcList), &FTP(obcEntries[i].node));
		GOEP_Report("[FTPOBS] insert list-item:0x%x obs:0x%x", &FTP(obcEntries[i].node), &FTP(obcEntries[i]) );
		
    }


	return TRUE;
}

/*---------------------------------------------------------------------------
 *            FTPOBS_New()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Obtain an object store item.
 *
 * Return:    Handle of object store or 0 if no more objects store entries.
 *
 */
FtpcObStoreHandle FTPOBS_New(void *app, BOOL client, BOOL useMem, U8 conn_id)
{
    FtpcObStoreHandle obs;
	static U32 start_test_offset = 0;	
	
    
	/* Select a objectstore handle from avaiable list */
    if (IsListEmpty(&FTP(obcList)) == FALSE) {
        obs = (FtpcObStoreHandle)RemoveHeadList(&FTP(obcList));
        obs->app = app;
        obs->ocx = 0;
        obs->objLen = 0;
        obs->offset = 0;
        obs->nameLen = 0;
        obs->name[0] = 0;
        obs->typeLen = 0;
        obs->type[0] = 0;
        obs->code = 0;
		obs->path = 0;
		obs->ftpOp = GOEP_OPER_PULL;
		obs->isClient = client;
		obs->membuffer = 0;
		obs->membuffer_len = 0;
		obs->membuffer_size= 0;
		obs->conn_id = conn_id;
		obs->selfmsg = FALSE;

#ifdef BT_FTP_SUPPORT_INTERNAL_RW
		if( TRUE == useMem ){
			obs->membuffer = get_ctrl_buffer(FTP_INTERNAL_RW_BUFFER_SIZE+1);
			OS_Report("[FTP][M] Use mem:%d size:%d buff:0x%x", useMem, FTP_INTERNAL_RW_BUFFER_SIZE, obs->membuffer);
		}
		if( NULL != obs->membuffer ){
			obs->membuffer_size = FTP_INTERNAL_RW_BUFFER_SIZE;	
		}
		start_test_offset = (start_test_offset+((FTP_INTERNAL_RW_BUFFER_SIZE*9)/10)) % FTP_INTERNAL_RW_BUFFER_SIZE;
#ifdef FTP_GUIDE_BYTE
		if( NULL != obs->membuffer ){
			obs->membuffer[FTP_INTERNAL_RW_BUFFER_SIZE] = 0x5a;
		}
#endif
#endif
    }
    else {
        Report(("[FTPC OBS]FTPOBS_New: Out of entries\n"));
		return NULL;
    }
	GOEP_FLOW("[FTPCFLOW] FTPOBS_New obsh:0x%x", obs);
    return obs;
}

/*---------------------------------------------------------------------------
 *            FTPOBS_Close()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the object store item and return the handle to the pool.
 *
 * Return:    void 
 *
 */
ObexRespCode FTPOBS_Close(FtpcObStoreHandle *obshPtr, BT_BOOL bDel)
{
    ObexRespCode  rcode = OBRC_SUCCESS;
    FtpcObStoreHandle obsh = *obshPtr;
    GOEP_FLOW("[FTPCFLOW] FTPOBS_Close obsh:0x%x bDel:%d ", obsh, bDel);
    Assert(obsh);

	/* Close the OS dependence handler first */
    if (obsh->ocx != 0) {
		/// check variable
		GOEP_FLOW("[FTPCFLOW] FTPOBS_Close isClient:%d writeable:%d", obsh->isClient, obsh->writeable);
		
		if( NULL != obsh->membuffer && obsh->ocx > 0){
			// flush all the 
			FTPOBS_Flush(obsh);
		}

		if( obsh->ocx > 0 ){
			GOEP_Report("[FTP] OBS_Close ocx:0x%x", obsh->ocx);
			btmtk_fs_close_ucs2( obsh->ocx );
		    obsh->ocx = -1;
		}else{
			GOEP_Report("[FTP][WRN] OBS_Close handle is not avaiable:0x%x", obsh->ocx);
		}

        if( bDel ){
            GOEP_Report("[FTP] FTPOBS_Close delete file !");
            if( NULL != obsh->path && BTMTK_FS_OK == btmtk_fs_delete_ucs2( (const U16*)obsh->path ) ){
                rcode = OBRC_SUCCESS;
            }else{
                rcode = OBRC_FORBIDDEN;
            }            
        }
    }

		
	if( NULL != obsh->membuffer ){
		free_ctrl_buffer(obsh->membuffer);
		obsh->membuffer = NULL;
	}
	obsh->membuffer_size = 0;
	obsh->membuffer_len = 0;	
    
	if( obsh->path ){
		free_ctrl_buffer(obsh->path);
		obsh->path = NULL;
	}
    
    /* Return the FtpcObStoreHandle to the pool */
    InsertTailList(&FTP(obcList), &(obsh->node));

	/* release the reference for obsh user */
    *obshPtr = 0;

    return rcode;
}

ObexRespCode FTPOBS_CreateMemBuffer(FtpcObStoreHandle obsh, S32 filesize)
{
	ObexRespCode  rcode = OBRC_NOT_ACCEPTABLE;

#if defined( FTP_FAKE_WRITE ) || defined( FTP_FAKE_READ )
	OS_Report("[FTP][M] CreateMemBuffer use fake. just return");
	return rcode;
#endif


	if( NULL != obsh->membuffer ){
		OS_Report("[FTP][M] CreateMemBuffer already has");
		rcode = OBRC_SUCCESS;
		return rcode;
	}

	if( filesize < FTP_LARGE_FILE_SIZE ){
		OS_Report("[FTP][M] CreateMemBuffer filesize:%d too small. ignore it", filesize );
		rcode = OBRC_SUCCESS;
		return rcode;
	}
	
#ifdef BT_FTP_SUPPORT_INTERNAL_RW
	if( NULL == obsh->membuffer ){
		obsh->membuffer = get_ctrl_buffer(FTP_INTERNAL_RW_BUFFER_SIZE+1);
		OS_Report("[FTP][M] CreateMemBuffer filesize:%d size:%d buff:0x%x", filesize, FTP_INTERNAL_RW_BUFFER_SIZE, obsh->membuffer);
	}
	if( NULL != obsh->membuffer ){
		obsh->membuffer_size = FTP_INTERNAL_RW_BUFFER_SIZE;	
		rcode = OBRC_SUCCESS;
	}
#ifdef FTP_GUIDE_BYTE
	if( NULL != obsh->membuffer ){
		obsh->membuffer[FTP_INTERNAL_RW_BUFFER_SIZE] = 0x5a;
	}
#endif
#endif	
	return rcode;
}
/*---------------------------------------------------------------------------
 *            FTPOBS_AppendName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open the store object (file)
 *            Open the file based on the filepath by FTPOBS_AppendFilePath
 * Return:    OBRC_NOT_FOUND or OBRC_SUCCESS
 *
 */
ObexRespCode FTPOBS_Open(FtpcObStoreHandle *obshPtr, BT_BOOL writeable)
{
	ObexRespCode  rcode = OBRC_NOT_FOUND;
	FtpcObStoreHandle obsh = *obshPtr;
    int ret;
    U32 Size;

    GOEP_FLOW("[FTPCFLOW] FTPOBS_Open obsh:0x%x", obsh);
    if ( obsh == 0 || obsh->path == NULL ) {
    	GOEP_Report("[FTP][OBS][ERR] obsh:0x%x filepath is NULL !", obsh);
        return rcode;
    }

	GOEP_Report("[FTP] FTPOBS_Open ftpOp:%d writeable:%d", obsh->ftpOp, writeable );

	/* open file and get the fs handle */
	obsh->writeable = writeable;
	if( writeable ){
		obsh->ocx = btmtk_fs_open_ucs2((const U16*)obsh->path, BTMTK_FS_CREATE |BTMTK_FS_CREATE_ALWAYS );
	}else{
		obsh->ocx = btmtk_fs_open_ucs2((const U16*)obsh->path, BTMTK_FS_READ_ONLY);
	}

	if( obsh->ocx >= 0 && TRUE != writeable ){
		Size = 0;
		ret = btmtk_fs_get_filesize_ucs2(obsh->ocx, &Size);
		kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_GET_FILESIZE, ret, Size);
		obsh->objLen = Size;
    }

	/* check fs handle */
	/* Update the obj len */
	if( obsh->ocx <= 0 ){
		// fail
		GOEP_Report("[FTP][FTPCFLOW][ERR] FTPOBS_Open fail:%d", obsh->ocx);
		obsh->ocx = 0;
		rcode = OBRC_NOT_FOUND;
	}else{
#if 1		
		{
			U8 filepath[128];
			memset(filepath, 0, sizeof(filepath) );
			//bt_chset_ucs2_to_utf8_string(filepath, sizeof(filepath), obsh->path);		
			GOEP_FLOW("[FTPCFLOW] FTPOBS_Open path:'%s'", filepath);
		}
#endif
		GOEP_Report("[FTP][FTPCFLOW] FTPOBS_Open ok obsh:0x%x obsh->ocx:0x%x", obsh, obsh->ocx);
		rcode = OBRC_SUCCESS;
		
	}

    obsh->flags = 0;

#if defined( FTP_FAKE_WRITE ) || defined( FTP_FAKE_READ )
    // ftpc /data/data/com.mediatek.bluetooth/ftpc_folder_obj.xml
    // ftps /data/data/com.mediatek.bluetooth/ftp_tmpxml.xml
    if( 0 == bt_ucs2cmp(obsh->path, L"/data/data/com.mediatek.bluetooth/ftp_tmpxml.xml") ){        
        // ftp server's file. read and send it out
    }else if( 0 == bt_ucs2cmp(obsh->path, L"/data/@btmtk/ftpc_folder_obj.xml") ){
        // ftp client's file. write it down.
    }else if( 0 == bt_ucs2cmp(obsh->path, L"/data/@btmtk/ftp_tmpxml.xml") ){
        // ftp client's file. write it down.
    }else{

#ifdef FTP_FAKE_WRITE
        if( writeable ){
            obsh->flags |= 0x40; // fake write
            GOEP_Report("[FT][OBS][WRN][ERR] FAKE write 5!");
        }
#endif
#ifdef FTP_FAKE_READ
        if( !writeable ){
            obsh->flags |= 0x10; // fake read
            g_fake_read_index = 0;
            GOEP_Report("[FT][OBS][WRN][ERR] FAKE read 4!");
        }
#endif

    }
#endif
	OS_Report("[FT][OBS] OPEN flag:%d!", obsh->flags);
	return rcode;
  
}


/*---------------------------------------------------------------------------
 *            FTPOBS_AppendName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the name. 
 *
 * Return:    void
 *
 */
void FTPOBS_AppendName(FtpcObStoreHandle obsh, const U8 *name, U16 len)
{
    U16 finalLen;
    GOEP_FLOW("[FTPCFLOW] FTPOBS_AppendName len:%d", len);
    if ((obsh == 0) || (len == 0)) {
        return;
    }
	
	finalLen = sizeof(obsh->name) -1 ;
	if( len <= finalLen ){
		finalLen = len;
	}
	
	btmtk_os_memcpy( obsh->name, name, finalLen);
	obsh->nameLen = finalLen;
	obsh->name[finalLen] = 0;

}

/*---------------------------------------------------------------------------
 *            FTPOBS_AppendName
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the name. 
 *
 * Return:    void
 *
 */
void FTPOBS_AppendFilePath(FtpcObStoreHandle obsh, const U8 *path, U16 len)
{
    U16 finalLen;
    GOEP_FLOW("[FTPCFLOW] FTPOBS_AppendFilePath obsh:0x%x len:%d", obsh, len);
    if ((obsh == 0) || (len == 0)) {
        return;
    }
	
	if( obsh->path != NULL ){
		free_ctrl_buffer(obsh->path);
		obsh->path =  NULL;
	}
	FTPAssert(len!=0);
	obsh->path = get_ctrl_buffer(len+2);
	FTPAssert( obsh->path!= NULL );
	btmtk_os_memcpy( obsh->path, path, len);
	obsh->path[len] = 0;
	obsh->path[len+1] = 0;

#if 1
	{
		U8 filepath[128];
		memset(filepath, 0, sizeof(filepath) );
		if( len != 0  && bt_ucs2strlen((const S8*)obsh->path) == 0 ){
			GOEP_Report("[FTP][OBS] wrong order");
		}else{
			bt_chset_ucs2_to_utf8_string(filepath, sizeof(filepath)-2, (const U8 *)obsh->path);
			GOEP_Report("[FTPCFLOW] FTPOBS_AppendFilePath len:%d path:'%s'", len, filepath);
		}
	}
#endif		
}

/*---------------------------------------------------------------------------
 *            FTPOBS_AppendType()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Append the ASCII string to the object type field.
 *
 * Return:    void
 *
 */
void FTPOBS_AppendType(FtpcObStoreHandle obsh, const U8 *type, U16 len)
{
    U16 copyLen;
    GOEP_FLOW("[FTPCFLOW] FTPOBS_AppendType");
    if ((obsh == 0) || (len == 0)) {
        return;
    }

	copyLen = sizeof(obsh->type) -1 ;
	if( len <= copyLen ){
		copyLen = len;
	}
	
	btmtk_os_memcpy( obsh->type, type, copyLen);
	obsh->typeLen = copyLen;
	obsh->type[copyLen] = 0;  
}

/*---------------------------------------------------------------------------
 *            FTPOBS_GetObjectLen()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Get the length of an object store item.
 *
 * Return:    The length of the object in bytes.
 *
 */
U32 FTPOBS_GetObjectLen(void *obs)
{
    FtpcObStoreHandle obsh = (FtpcObStoreHandle)obs;
#ifdef _FTPOBS_DEBUG_
    GOEP_FLOW("[FTPCFLOW] FTPOBS_GetObjectLen %d", (obsh ? obsh->objLen : 0) );
#endif
    
	/* the objlen will be update when OBS_Open or OBS_New or OBS_Write */

    return obsh ? obsh->objLen : 0;
}


/* 
 * @brief for testing
 */
ObexRespCode FTPOBS_ReadToBuffer(void *obs, U16 len){
    FtpcObStoreHandle    obsh = (FtpcObStoreHandle)obs;
    ObexRespCode        ret = OBRC_NOT_FOUND;
    S32 result = 0;
	S32 space, copy_start, copy_len, read;

    GOEP_FLOW("[FTPCFLOW] FTPOBS_ReadToBuffer obsh:0x%x len:%d, buffer_len:%d", obsh, len, obsh->membuffer_len);
    if ( obsh == 0 || obsh->ocx <= 0)  {
    	GOEP_Report("[FTP][OBS][ERR] obsh or ocx is NULL");
        return ret;
    }

	// check left space
	if( len > (obsh->membuffer_size-obsh->membuffer_len) ){
		GOEP_FLOW("[FTPCFLOW] FTPOBS_ReadToBuffer not enought space");
		FTPAssert(0);
		return ret;
	}

	if( NULL == obsh->membuffer ){
		GOEP_FLOW("[FTPCFLOW] FTPOBS_ReadToBuffer no buffer");
		FTPAssert(0);
		return ret;
	}

	// check part1 space
	if( obsh->membuffer_size > (obsh->membuffer_start+obsh->membuffer_len) ){
		space = obsh->membuffer_size - (obsh->membuffer_start+obsh->membuffer_len);
	}else{
		space = obsh->membuffer_size - obsh->membuffer_len;
	}

	if( space > len ){
		copy_len = len;
	}else{
		copy_len = space;
	}

	copy_start = (obsh->membuffer_start+obsh->membuffer_len)%obsh->membuffer_size;

	//read from file to buffer	
	result = btmtk_fs_read_ucs2(obsh->ocx, &obsh->membuffer[copy_start], copy_len, &read );
    if( result < 0 ){
		FTPAssert(0);
	}
	if( read > len ){
		FTPAssert(0);
	}
	
	if( result >= 0 ){
		obsh->membuffer_len += read;
		len -= read;
		ret = OBRC_SUCCESS;
	}else{
		ret = OBRC_NOT_FOUND;
	}

	if( len > 0 ){
		// copy twice
		copy_start = (obsh->membuffer_start+obsh->membuffer_len)%obsh->membuffer_size;
		if( obsh->membuffer_size > (obsh->membuffer_start+obsh->membuffer_len) ){
			space = obsh->membuffer_size - (obsh->membuffer_start+obsh->membuffer_len);
		}else{
			space = obsh->membuffer_size - obsh->membuffer_len;
		}

		if( space > len ){
			copy_len = len;
		}else{
			copy_len = space;
		}

		result = btmtk_fs_read_ucs2(obsh->ocx, &obsh->membuffer[copy_start], copy_len, &read );
		if( result < 0 || read == 0 ){
			FTPAssert(0);
		}
		if( result >= 0 ){
			obsh->membuffer_len += read;
			len -= read;
			ret = OBRC_SUCCESS;
		}else{
			ret = OBRC_NOT_FOUND;
		}

		if( len != 0 ){
			FTPAssert(0);
		}
	}

	return ret;
}

/*---------------------------------------------------------------------------
 *            FTPOBS_Read()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Read an image file.
 *
 * Return:    OBRC_SUCCESS -      Buffer was read successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not read.
 *
 */
ObexRespCode FTPOBS_Read(void *obs, U8* buff, U16 len)
{
    FtpcObStoreHandle    obsh = (FtpcObStoreHandle)obs;
    ObexRespCode        ret = OBRC_NOT_FOUND;
    S32 result  = 0;
	U32                 part1_len, copy_len, len_pos = 0;

	S32                 read;
	S32 result2 = 0;

    kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_OBSREAD_PARAM, obsh, obsh->ocx, len);
    if ( obsh == 0 || obsh->ocx <= 0)  {
    	GOEP_Report("[FTP][OBS][ERR] obsh or ocx is NULL");
        return ret;
    }

	if( len == 0 ){
		return OBRC_SUCCESS;
	}

	ret = OBRC_SUCCESS;
    if( obsh->flags == 0 ){
        Assert(obsh->app != 0);
        read = 0;
		
		if( obsh->membuffer && obsh->membuffer_len > 0 ){
#ifdef _FTPOBS_DEBUG_
			GOEP_FLOW("[FTP][M] Read input-len:%d start:0x%x len:0x%x size:%d", len, obsh->membuffer_start, obsh->membuffer_len, obsh->membuffer_size);			
#endif
			// read from buffer first. if it is no enought, read from file
			if( (obsh->membuffer_start+obsh->membuffer_len) >obsh->membuffer_size ){
				// round a circle
				part1_len = obsh->membuffer_size-obsh->membuffer_start;
				// only provide the available length
				if( part1_len > obsh->membuffer_len ){
					part1_len = obsh->membuffer_len;
				}
			}else{
				// no circle
				part1_len = obsh->membuffer_len;
			}

			if( part1_len > len ){
				copy_len = len;
			}else{
				copy_len = part1_len;
			}

			// copy from buffer
			if( copy_len != 0 ){
				btmtk_os_memcpy( buff, &obsh->membuffer[obsh->membuffer_start], copy_len);
			}else{
				FTPAssert(0);
			}
			obsh->membuffer_start = ( obsh->membuffer_start + copy_len )%obsh->membuffer_size;
			obsh->membuffer_len   -= copy_len;
			len_pos = copy_len;
			read += copy_len;

			// copy the second part if necessary
			if( len > len_pos && obsh->membuffer_len > 0 ){
				if( obsh->membuffer_len > (len-len_pos) ){
					copy_len = len-len_pos;
				}else{
					// not enought buffered data
					copy_len = obsh->membuffer_len;
				}

				btmtk_os_memcpy( buff+len_pos, &obsh->membuffer[obsh->membuffer_start], copy_len);
				obsh->membuffer_start = ( obsh->membuffer_start + copy_len )%obsh->membuffer_size;
				obsh->membuffer_len   -= copy_len;
				len_pos += copy_len;
				read += copy_len;
			}

			if( obsh->membuffer_len > 0  && len > len_pos ){
				FTPAssert(0);
			}

			if( len_pos < len  ){
		    	result = btmtk_fs_read_ucs2(obsh->ocx, buff+len_pos, (len-len_pos), &read );
                GOEP_FLOW("[FTP][M] Read - act-read result:%d read:%d", result, read); 
				// check read result
				if( result >= 0 ){
					// in the end of this api, we will check read
					read += len_pos;  // append the previous read data
				}

			}else if( copy_len == len ){
				// no need to read from file
			}else{
				ASSERT(0);
			}

			// send the message
			if( FALSE == obsh->selfmsg ){
				if( obsh->membuffer_len < (obsh->membuffer_size >> 2) ){
					// send a message to itself
					if( obsh->isClient ){
						ftpc_send_internal_rw_ind_msg(obsh->conn_id, obsh->writeable);
					}else{
						ftps_send_internal_rw_ind_msg(obsh->conn_id, obsh->writeable);
					}
					obsh->selfmsg = TRUE;
				}
			}

				
		}else{
	    	result = btmtk_fs_read_ucs2(obsh->ocx, buff, len, &read );
			GOEP_FLOW("[FTP][M] Read - act-read result:%d read:%d", result, read);

			if( obsh->membuffer && 0 == obsh->membuffer_len ){
				// read from buffer
				result2 = btmtk_fs_read_ucs2(obsh->ocx, obsh->membuffer, obsh->membuffer_size, (S32*)&obsh->membuffer_len);
				obsh->membuffer_start = 0;
				kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_READ_RESULT, result2, obsh->membuffer_len, obsh->membuffer_start);
				if( result2 < 0 || obsh->membuffer_len > obsh->membuffer_size ){
					obsh->membuffer_len = 0;
				}
			}
		}
    }else{
        // read
        Assert(obsh->app != 0);
        read = 0;
#ifdef FTP_FAKE_READ
        // first 3 byte is the index 1 -> 0x00 0x00 0x01
        if( len < 3 ){
            // fill up with 0x00 
            for( read = 0; read < len; read++){
                buff[read] = 0;
            }
        }else{
            buff[0] = (U8)((g_fake_read_index & 0xff0000) >> 16);
            buff[1] = (U8)((g_fake_read_index & 0xff00) >> 8);
            buff[2] = (U8)((g_fake_read_index & 0xff));
            result = 1;
            for( read = 3; read < len; read++){
                buff[read] = (U8)(result++);
                if( result > 0xff ){
                    result = 0x01;
                }
            }
        }
        GOEP_Report("[FAKEDATA] tx-index:%d bufflen:%d", g_fake_read_index, len);
        g_fake_read_index++;        
#endif
        read = len;
        result = 0;
    }
    
    if( len != (U16)read ) {
        GOEP_Report("[FTP][ERR] read fail handle:0x%x len:%d read:%d", obsh->ocx, len, read);
        ret = OBRC_UNAUTHORIZED;
    }
    if( result < 0  ){
        GOEP_Report("[FTP][ERR] read fail result:%d", result);
        ret = OBRC_UNAUTHORIZED;
    }


    obsh->offset += read;

    // test 
    FTPOBS_PreReadMemory(obsh);    
 
    return ret;
}


/*---------------------------------------------------------------------------
 *            FTPOBS_Write()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Write an image file.
 *
 * obs   :    FtpcObStoreHandle
 * Return:    OBRC_SUCCESS -      Buffer was written successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not written.
 *
 */
ObexRespCode FTPOBS_Write(void *obs, U8 *buff, U16 len)
{
    FtpcObStoreHandle    obsh = (FtpcObStoreHandle)obs;
    ObexRespCode        ret = OBRC_FORBIDDEN;

    S32                written;
    S32                fs_ret;
	U32                part1_len, copy_len;
	U32                pos;
	

    kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_OBSWRITE_PARAM, obsh, obsh->ocx, len);

	ret = OBRC_SUCCESS;

    if( obsh == 0 ) {
    	GOEP_Report("[FTP][ERR] FTPOBS_Write obsh:0x%x is NULL ", obsh);
        return ret;
    }
    if( obsh->ocx <= 0){
    	GOEP_Report("[FTP][ERR] FTPOBS_Write obsh:0x%x ocx:%d ", obsh, obsh->ocx);
    	return ret;
    }
    Assert(obsh->app != 0);

#ifdef FTP_FAKE_READ
    if( len > 3 ){
        // print out the index (first 3 Byte)
        fs_ret = buff[0] << 16 + buff[1] << 8 + buff[2];
        GOEP_Report("[FAKEDATA] buf[3]:%d idx:%d len:%d", buff[3], fs_ret, len);
    }else{
        if( len > 0){
            GOEP_Report("[FAKEDATA] buf[0]:%d len:%d ", buff[0], len);
        }
    }
#endif


    if( !(obsh->flags & 0x40) ){
		if( obsh->membuffer ){
#ifdef _FTPOBS_DEBUG_			
			GOEP_FLOW("[FTP][M] Write input-len:%d start:0x%x len:0x%x size:%d", len, obsh->membuffer_start, obsh->membuffer_len, obsh->membuffer_size);
#endif			
			if( len > (obsh->membuffer_size-obsh->membuffer_len) ){
				fs_ret = 0;
				// write the buffer before write the input
				if(  obsh->membuffer_size > (obsh->membuffer_start+obsh->membuffer_len)  ){
					part1_len = obsh->membuffer_len ;
				}else{
					part1_len = obsh->membuffer_size - obsh->membuffer_start;
				}
				fs_ret = btmtk_fs_write_ucs2(obsh->ocx, &obsh->membuffer[obsh->membuffer_start], part1_len, &written);
				if( fs_ret < 0 || written != (S32)part1_len){
					//fail
				}else{
					obsh->membuffer_len -= part1_len;
					obsh->membuffer_start = 0;

					if( obsh->membuffer_len > 0 ){
						fs_ret = btmtk_fs_write_ucs2(obsh->ocx, &obsh->membuffer[obsh->membuffer_start], obsh->membuffer_len, &written);

						if( fs_ret < 0 || written != (S32)obsh->membuffer_len){
							FTPAssert(0);
						}else{
							obsh->membuffer_len = 0;
						}
					}
				}

				if( fs_ret >= 0 ){
		    		fs_ret = btmtk_fs_write_ucs2(obsh->ocx, buff, len, &written);
				}
			}else{
				// append data into buffer. 
				// if it is full, write all to file
				fs_ret = len;
				written = 0;
#ifdef _FTPOBS_DEBUG_                
				GOEP_Report("[NB] +++len:%d", len);
#endif                
				//calculate the empty space
				
				// appedn data into buffer only
				if( len > (obsh->membuffer_size - (obsh->membuffer_start+obsh->membuffer_len) ) ){
					part1_len = obsh->membuffer_size - (obsh->membuffer_start + obsh->membuffer_len) ;
				}else{
					part1_len = len;
				}
#ifdef _FTPOBS_DEBUG_
				GOEP_Report("[NB]o size:%d start:%d len:%d part1_len:%d", obsh->membuffer_size, obsh->membuffer_start, obsh->membuffer_len, part1_len);
#endif				
				
				if( 0 != part1_len ){
					pos = (obsh->membuffer_start+obsh->membuffer_len)% obsh->membuffer_size; // maybe round
					btmtk_os_memcpy( &obsh->membuffer[pos], buff, part1_len);
				}
				obsh->membuffer_len += part1_len;
				written += part1_len;

				if( part1_len < len ){
#ifdef _FTPOBS_DEBUG_
					GOEP_Report("[NB]p size:%d start:%d len:%d part1_len:%d", obsh->membuffer_size, obsh->membuffer_start, obsh->membuffer_len, part1_len);
#endif					
					if( obsh->membuffer_size != (obsh->membuffer_start+obsh->membuffer_len) ){
						ASSERT(0);
					}
					btmtk_os_memcpy( &obsh->membuffer[0], buff+part1_len, (len-part1_len));
					obsh->membuffer_len += (len-part1_len);
					written += (len-part1_len);
				}else if( part1_len == len ){
					// copy done
				}else{
					ASSERT(0);
				}
#ifdef _FTPOBS_DEBUG_
				GOEP_Report("[NB]q size:%d start:%d len:%d part1_len:%d", obsh->membuffer_size, obsh->membuffer_start, obsh->membuffer_len, part1_len);						
#endif				
			}
#ifdef _FTPOBS_DEBUG_            
			GOEP_Report("[NB] ---len:%d", len);				
#endif

#ifdef FTP_GUIDE_BYTE
			if( obsh->membuffer && obsh->membuffer[FTP_INTERNAL_RW_BUFFER_SIZE] != 0x5a ){
				FTPAssert(0);
			}
#endif

			// send the message
			if( FALSE == obsh->selfmsg ){
				if( obsh->membuffer_len > (obsh->membuffer_size >> 2) ){
					// send a message to itself
					if( obsh->isClient ){
						ftpc_send_internal_rw_ind_msg(obsh->conn_id, obsh->writeable);
					}else{
						ftps_send_internal_rw_ind_msg(obsh->conn_id, obsh->writeable);
					}
					obsh->selfmsg = TRUE;
				}
			}

		}else{
	    	fs_ret = btmtk_fs_write_ucs2(obsh->ocx, buff, len, &written);
		}
    }else{
        // fake write
        fs_ret = written = len;
    }
    
    if( fs_ret >= 0 && written == len){
        ret = OBRC_SUCCESS;
    }else{
        ret = OBRC_FORBIDDEN;
        GOEP_Report( "[FTP][WRN] write file handle:0x%x fail:%d", obsh->ocx, fs_ret);
    }

    /* update the objlen */
    if( OBRC_SUCCESS == ret ){
        obsh->objLen += len;
        obsh->offset += len;
    }

    return ret;

}

// move the. if we already save some data and need it to change other place.
// we use this to assign another filepath
ObexRespCode FTPOBS_MoveOpenFilePath(FtpcObStoreHandle obsh, const U8 *path, U16 len){
    int new_ocx = 0;
    S32 sRet;
    S32 read;
    S32 written;
    U8  buff[128];
	S32 total = 0;
    
    GOEP_Report("[FTP] FTPOBS_MoveOpenFilePath obsh:0x%x newpath_len:%d newpath_len:%d test5", obsh, len, bt_ucs2strlen((const S8*)path)*2 );
    
    if( obsh->ocx < 0 ){
    	GOEP_Report("[FTP][OBS] Fail to MoveOpenFilePath no opened file");
    	return OBRC_FORBIDDEN;
    }
    /* open the another file. if fail, report error */
    new_ocx = btmtk_fs_open_ucs2((const U16*)path, BTMTK_FS_CREATE|BTMTK_FS_CREATE_ALWAYS );
    if( new_ocx < 0 ){
        GOEP_Report("[FTP][OBS] Fail to MoveOpenFilePath err:%d", new_ocx);
        return OBRC_FORBIDDEN;
    }
    /* Close the previous file */
	FTPOBS_Flush(obsh); //!
    btmtk_fs_close_ucs2(obsh->ocx);
    
    /* Open the previous file */
    obsh->ocx = btmtk_fs_open_ucs2((const U16*)obsh->path, BTMTK_FS_READ_ONLY);
    
    GOEP_Report("[FTP][OBS] copy all data to ocx:0x%x", obsh->ocx);
    /* Copy all data to the new open file handler */
    /* Close prvious file */
    sRet = read = 0;
    while( (sRet = btmtk_fs_read_ucs2(obsh->ocx, buff, sizeof(buff), &read )) >= 0 && read > 0 ){
		if( read > (S32)sizeof(buff) ){
    		GOEP_Report("[FTP][OBS] Read fail err:%d read:%d", sRet, read);
			return OBRC_FORBIDDEN;
		}
    	if( (sRet = btmtk_fs_write_ucs2(new_ocx, buff, read, &written)) < 0 ){
    		GOEP_Report("[FTP][OBS] Write fail err:%d", sRet );
    		btmtk_fs_close_ucs2(new_ocx);
    		btmtk_fs_delete_ucs2( (const U16*)path );
    		return OBRC_FORBIDDEN;
    	}
		if( read != written ){
    		GOEP_Report("[FTP][OBS] Write fail err:%d read:%d written:%d", sRet, read, written );
    		btmtk_fs_close_ucs2(new_ocx);
    		btmtk_fs_delete_ucs2( (const U16*)path );
    		return OBRC_FORBIDDEN;
		}
		total += written;
    }
	GOEP_Report("[FTP][OBS] FTPOBS_MoveOpenFilePath copy %d", total);
    /* close and delete previous file */
	btmtk_fs_close_ucs2( obsh->ocx );
	obsh->ocx = 0;
	if(obsh->path){
		btmtk_fs_delete_ucs2( (const U16*)obsh->path );
	}
    
    /* replace the new filepath */
	if( obsh->path != NULL ){
		free_ctrl_buffer(obsh->path);
		obsh->path =  NULL;
	}
	obsh->path = get_ctrl_buffer(len+2);
	Assert(obsh->path);
	btmtk_os_memcpy( obsh->path, path, len);
	obsh->path[len] = 0;
	obsh->path[len+1] = 0;
    
    /* replace the new file handle */
    obsh->ocx = new_ocx;
    
    return OBRC_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            FTPOBS_Write()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Write an image file.
 *
 * Return:    OBRC_SUCCESS -      Buffer was written successfully.
 *            OBRC_UNAUTHORIZED - Buffer was not written.
 *
 */
ObexRespCode FTPOBS_Flush(FtpcObStoreHandle obsh)
{
    ObexRespCode        ret = OBRC_FORBIDDEN;

    S32                written;
    S32                fs_ret = 0;
    S32                part1_len, part2_len;

    kal_trace(BT_TRACE_G6_OBEX, FTP_UTIL_FLUSH, obsh, obsh->ocx);

	ret = OBRC_SUCCESS;

    if( obsh == 0 ) {
    	GOEP_Report("[FTP][ERR] FTPOBS_Flush obsh:0x%x is NULL ", obsh);
        return ret;
    }
    if( obsh->ocx <= 0){
    	GOEP_Report("[FTP][ERR] FTPOBS_Flush obsh:0x%x ocx:%d ", obsh, obsh->ocx);
    	return ret;
    }
	if( FALSE == obsh->writeable ){
		return ret;
	}
	
    Assert(obsh->app != 0);
    if( obsh->flags == 0 ){
	if( obsh->writeable && obsh->membuffer && 0 !=obsh->membuffer_len ){
		// write down the data
#ifdef _FTPOBS_DEBUG_		
		GOEP_Report("[FTP][M] Flush input-len:%d start:0x%x len:0x%x size:%d", 0, obsh->membuffer_start, obsh->membuffer_len, obsh->membuffer_size);
#endif

		if( (obsh->membuffer_start+obsh->membuffer_len) > obsh->membuffer_size ){
			part1_len = obsh->membuffer_size - (obsh->membuffer_start);
		}else{
			part1_len = obsh->membuffer_len;
		}
		if( 0 != part1_len ){
			fs_ret = btmtk_fs_write_ucs2(obsh->ocx, &obsh->membuffer[obsh->membuffer_start], part1_len, &written);
            if( fs_ret < 0 ){            
			GOEP_Report("[FTP][M] Flush write1 fs_ret:%d part1:%d written:%d", fs_ret, part1_len, written);
            }
		}else{
			fs_ret = written = part1_len = 0;
			GOEP_Report("[FTP][M] Flush part1_len is %d", part1_len); 
		}
		if( fs_ret >= 0 ){
			// write part1 success 
			obsh->membuffer_len -= part1_len;
			obsh->membuffer_start = 0;
		}
		if( written != part1_len || fs_ret < 0 ){
			ret = OBRC_FORBIDDEN;
		}else{
			// write part2
			if(  obsh->membuffer_len > 0){
				fs_ret = btmtk_fs_write_ucs2(obsh->ocx, &obsh->membuffer[obsh->membuffer_start], obsh->membuffer_len, &written);
				GOEP_Report("[FTP][M] Flush write2 fs_ret:%d part1:%d written:%d", fs_ret, part1_len, written);

				if( written != (S32)obsh->membuffer_len || fs_ret < 0 ){
					ret = OBRC_FORBIDDEN;
				}else{
					obsh->membuffer_len = 0;
				}
			}
		}
	}
    }
	
	// GOEP_Report("[FTP][M] Flush input end:%d start:0x%x len:0x%x size:%d", 0, obsh->membuffer_start, obsh->membuffer_len, obsh->membuffer_size);

    // base on the read/write to do the write or read
    return ret;
}

/**
 *
 */
ObexRespCode FTPOBS_PreReadMemory(FtpcObStoreHandle obsh){
    ObexRespCode        ret = OBRC_FORBIDDEN;
#ifdef _FTPOBS_DEBUG_
    GOEP_FLOW("[FTPCFLOW] PreRead obsh:0x%x ocx:0x%x ", obsh, obsh->ocx);
#endif
	ret = OBRC_SUCCESS;
	U32 pos, readsize;
	S32 read;
    S32 result = 0;

    if( obsh == 0 ) {
    	GOEP_Report("[FTP][ERR] PreRead obsh:0x%x is NULL ", obsh);
        return ret;
    }
    if( obsh->ocx <= 0 ){
    	GOEP_Report("[FTP][ERR] PreRead obsh:0x%x ocx:%d writeable:%d", obsh, obsh->ocx);
    	return ret;
    }
	if( TRUE == obsh->writeable ){
		return ret;
	}
    Assert(obsh->app != 0);
    if( obsh->flags == 0 ){
	if( !obsh->writeable && obsh->membuffer && obsh->membuffer_size != obsh->membuffer_len ){
#ifdef _FTPOBS_DEBUG_
	    GOEP_FLOW("[FTPCFLOW] PreRead begin start:%d len:%d ", obsh->membuffer_start, obsh->membuffer_len);
#endif	    
		// read more data from file
		pos = (obsh->membuffer_start + obsh->membuffer_len)%obsh->membuffer_size;
		if( pos < obsh->membuffer_start ){
			// round a circle
			readsize = obsh->membuffer_size-obsh->membuffer_len;
		}else{
			readsize = obsh->membuffer_size - pos;
		}

		if( readsize > 0 ){
			result = btmtk_fs_read_ucs2(obsh->ocx, &obsh->membuffer[pos], readsize, &read );
			if( result >= 0 && read != 0 ){
				obsh->membuffer_len += read;
			}
			if( result < 0 ){
			    GOEP_FLOW("[FTP][M] PreRead - read1 result:%d read:%d membuffer_len:%d readsize:%d", result, read, obsh->membuffer_len, readsize); 
     	    }
			// read the second part
			if( result >= 0 && read != 0 ){
				if( obsh->membuffer_len < obsh->membuffer_size ){
					pos = (obsh->membuffer_start + obsh->membuffer_len)%obsh->membuffer_size;
					if( pos < obsh->membuffer_start ){
						// round a circle
						readsize = obsh->membuffer_size-obsh->membuffer_len;
					}else{
#ifdef _FTPOBS_DEBUG_
						GOEP_FLOW("[FTP][M][ERR] PreRead pos:%d start:%d len:%d", pos, obsh->membuffer_start, obsh->membuffer_len);
#endif						
						ret = OBRC_CONFLICT;
						return ret;
					}
					result = btmtk_fs_read_ucs2(obsh->ocx, &obsh->membuffer[pos], readsize, &read );
					if( result >= 0 && read != 0 ){
						obsh->membuffer_len += read;
					}
#ifdef _FTPOBS_DEBUG_					
					GOEP_FLOW("[FTP][M] PreRead - read2 result:%d read:%d membuffer_len:%d", result, read, obsh->membuffer_len); 
#endif					
				}
								
			}
		}else{
			GOEP_FLOW("[FTP][M] PreRead - no need pos:%d membuffer_start:%d membuffer_len:%d", pos, obsh->membuffer_start, obsh->membuffer_len); 		
		}
#ifdef _FTPOBS_DEBUG_		
	    GOEP_FLOW("[FTPCFLOW] PreRead end start:%d len:%d ", obsh->membuffer_start, obsh->membuffer_len);
#endif	    
	}	
    }
    return ret;
}



void FTPOBS_TestCode(S8 *code, S8 *filepath){
	U8  *buff = NULL;
	U32 blocksize;
	U32 filesize;
	U32 pos;
	int fh = 0;
	S32 sRet, read, write;
	TimeT t1, t2;
	double speed;
	U8 i, j;
	U32 sizearray[4] = {4096, (4096*4), (4096*16), (4096*64)};
	U32 blockarray[4] = {1, 4, 8, 16};

	if( code && 0 == bt_ucs2cmp((const U8*)code, (const U8 *)L"mtkbttestsd") && 11 == bt_ucs2strlen((const S8 *)code)){
		buff = get_ctrl_buffer(32*1024); // 32K
		OS_Report("[FTP] enter TestCode");
		if( NULL == buff ){
			GOEP_Report("[FTP] no buffer for test");
			return;
		}
		
		// write file to sdcard 1k,2k,4k,8K to 1M,2M,4M,8M
		for( j = 0; j< 4; j++){
			filesize = sizearray[j];
			for( i = 0; i< 4; i++){
				blocksize = blockarray[i];
				pos = read = write = 0;
				fh = btmtk_fs_open_ucs2((const U16*)filepath, BTMTK_FS_CREATE|BTMTK_FS_CREATE_ALWAYS );
				if( fh < 0 ){
					break;
				}
				// mark time1
				t1 = BTCoreGetSystemTick1();
				while( pos < (filesize*1024) ){
					// write data
					sRet = btmtk_fs_write_ucs2(fh, buff, (blocksize*1024), &write );
					if( sRet < 0 || write == 0){
						OS_Report("[FTP] TESTCODE fail sRet:%d write:%d", sRet, write);
						break;
					}
					pos += write;
				}
				t2 = BTCoreGetSystemTick1();
				if( t2 <= t1 ){
					t2 =1;
				}else{
					t2 = t2-t1;
				}
				// search TESTCODE and uses , as delimitor to import to excel for analyzing
				OS_Report("[FTP] TESTCODE w,blocksize,%d,filesize,%d,diff,%d,end,%d,speed,%d,", blocksize, filesize, (t2), pos, (((pos)/(t2))));
				// mark time2
				if( fh >= 0 ){
					btmtk_fs_close_ucs2(fh);
				}
			}
		}

		// read file to sdcard 1k,2k,4k,8K to 1M,2M,4M,8M 
		for( j = 0; j< 4; j++){
			filesize = sizearray[j];

			for( i = 0; i< 4; i++){
				blocksize = blockarray[i];
				
				pos = read = write = 0;
				fh = btmtk_fs_open_ucs2((const U16*)filepath, BTMTK_FS_READ_ONLY);
				if( fh < 0 ){
					break;
				}
				// mark time1
				t1 = BTCoreGetSystemTick1();
				while( pos < (filesize*1024) ){
					// write data
					sRet = btmtk_fs_read_ucs2(fh, buff, (blocksize*1024), &read );
					if( sRet < 0 || read == 0){
						OS_Report("[FTP] TESTCODE fail sRet:%d read:%d", sRet, read);
						break;
					}
					pos += read;
				}
				t2 = BTCoreGetSystemTick1();
				if( t2 <= t1 ){
					t2 = 1;
				}else{
					t2 = t2-t1;
				}
				// search TESTCODE and uses , as delimitor to import to excel for analyzing
				OS_Report("[FTP] TESTCODE r,blocksize,%d,filesize,%d,diff,%d,end,%d,speed,%d,", blocksize, filesize, (t2), pos, (((pos)/(t2))));

				// mark time2
				if( fh >= 0 ){
					btmtk_fs_close_ucs2(fh);
				}
			}
		}		

		// delete the test file
		btmtk_fs_delete_ucs2((const U16*)filepath);		
	}

	if( buff ){
		free_ctrl_buffer(buff);
		buff = 0;
	}	
}


#endif
#endif /* defined(__BT_FTC_PROFILE__) || defined(__BT_FTS_PROFILE__) */
