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
 * bt_ftp_porting.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for porting FTP Client and Server
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
#include "ext_osal.h"
#include "bt_mmi.h"
#include "ext_nbuffer.h"
#include "bluetooth_ftps_struct.h"
#include "bluetooth_ftpc_struct.h"
#include "bt_ftpc_event.h"
#include "bt_ftps_event.h"
#include "bt_ftpc_struct.h"
#include "bt_ftps_struct.h"
#include "bt_ftpc_hdl.h"
#include "bt_ftps_hdl.h"
#include "bt_goep_fs.h"
#include "bt_ftpc_api.h"
#include "bt_ftps_api.h"

#include "ext_ucs2.h"
#include "bt_ftp_porting.h"
#include "bt_ext_debug.h"

#include "stdio.h"
#ifdef BTMTK_ON_WIN32
#include "io.h"
//#include "windows.h"
#endif

#ifdef BTMTK_ON_LINUX
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/statfs.h>
#include <unistd.h>
#endif

/* Android socket api */
#include "cutils/sockets.h"

static int g_ext_ftp_sockfd;
static struct sockaddr_un g_ext_ftp_sockaddr;
static socklen_t g_ext_ftp_sockaddr_len;

static struct sockaddr_un g_int_adp_sockaddr;
static socklen_t g_int_adp_sockaddr_len;

extern int socket_make_sockaddr_un(const char *name, int ns_id,
		struct sockaddr_un *p_addr, socklen_t *alen);

extern bt_ftpc_cntx_struct *const ftpc_cntx_p;
extern bt_ftpc_conn_cntx_struct *act_client_cntx_p;
extern bt_ftps_cntx_struct *const ftps_cntx_p;
extern bt_ftps_conn_cntx_struct *act_server_cntx_p;
extern BT_BOOL ftps_cntx_init(void);


extern BTMTK_FTPS_CALLBACK g_ftps_ui_callback;
extern BTMTK_FTPC_CALLBACK g_ftpc_ui_callback;

BT_BOOL g_ftps_read_only = FALSE; ///< allow the 


U8 g_ftps_root_folder[FMGR_MAX_PATH_LEN];

void btmtk_ftpc_init_cs()
{
}

void btmtk_ftpc_enter_cs()
{
}

void btmtk_ftpc_leave_cs()
{
}

void btmtk_ftps_init_cs()
{
}

void btmtk_ftps_enter_cs()
{
}

void btmtk_ftps_leave_cs()
{
}

BT_BOOL btmtk_receiving_disabled(){
    return FALSE;
}

// U8 g_ftps_buffer[FTPC_MEM_BUFF_SIZE];
U8 *btmtk_ftps_get_profile_shared_int_buffer(U32 u4Size) {
	if( u4Size <= FTPC_MEM_BUFF_SIZE) {
		// return g_ftps_buffer;
		return btmtk_ftps_get_ashm_buffer(); 
	}
	return NULL;
}

BT_BOOL btmtk_ftps_return_profile_shared_int_buffer(U8 *ucBuffer){
	btmtk_ftps_return_ashm_buffer();
	return TRUE;
}

// U8 g_ftpc_buffer[FTPC_MEM_BUFF_SIZE];
U8 *btmtk_ftpc_get_profile_shared_int_buffer(U32 u4Size) {
	if( u4Size <= FTPC_MEM_BUFF_SIZE) {
		// return g_ftpc_buffer;
		return btmtk_ftpc_get_ashm_buffer();
	}
	return NULL;
}

BT_BOOL btmtk_ftpc_return_profile_shared_int_buffer(U8 *ucBuffer) {
	btmtk_ftpc_return_ashm_buffer();
	return TRUE;
}

BT_BOOL btmtk_get_default_root_folder_path(U8 *path, U32 u4MaxLen){
	/// copy g_ftps_root_folder to path
	ext_ucs2ncpy((S8 *)path, (const S8 *) g_ftps_root_folder, u4MaxLen);
	return TRUE;
}

void btmtk_ftps_create_tmp_objname(U8 *name, U32 u4Random ){
	U8 buffer[20];
	U8 ten = '9';
	U8 num = '9';
	ten = (U8)(u4Random/10);
	num = (U8)(u4Random - ten*10);
	buffer[0] = 'f';
	buffer[1] = 'i';
	buffer[2] = 'l';
	buffer[3] = 'e';
	if (ten <=9) {
		buffer[4] = '0' + ten ;
	} else {
		buffer[4] = 'a';
	}
	if (ten <=9) {
		buffer[5] = '0' + num ;
	} else {
		buffer[5] = 'b';
	}
	buffer[6] = '.';
	buffer[7] = 't';
	buffer[8] = 'x';
	buffer[9] = 't';
	buffer[10] = 't';

	ext_asc_str_n_to_ucs2_str((S8 *) name, (S8 *) buffer, 30); //TODO change this !
}


/**
* @brief Return the temporary xml file path
* @param i index of FTP server
* @param ucXmlFilepath unicode filepath of xml file
* @param u4Maxsize the ucXmlFilepath max size
*/
BT_BOOL btmtk_ftps_get_xml_filepath(U32 index, U8 *ucXmlFilepath, U32 u4Maxsize){
	if (u4Maxsize < 32)
		return FALSE;
	// ext_ucs2ncpy(ucXmlFilepath, (const S8 *) L"d:\\temp\\ftp\\tmpxml.xml", u4Maxsize);
	// ext_ucs2ncpy((S8 *) ucXmlFilepath, (const S8 *) L"/data/data/com.mediatek.bluetooth/ftp_tmpxml.xml", u4Maxsize);
	ext_ucs2ncpy((S8 *) ucXmlFilepath, (const S8 *) L"/data/@btmtk/ftp_tmpxml.xml", u4Maxsize);
	return TRUE;    
}



/**
* @brief Get the parent path of a folder path
* @param ucFolderpath ucs2 path
* @param ucParent ucs2 path
*/
BT_BOOL btmtk_ftp_util_get_parent_folder(const U8 *ucFolderpath, U8 *ucParent, U32 maxSize) {

	S32 index;
	ext_ucs2ncpy((S8 *)ucParent, (const S8 *)ucFolderpath, maxSize);

	if (ucFolderpath == 0 || ucParent == 0 || maxSize <=4)
		return FALSE;

	index = ext_ucs2strlen((const S8 *) ucFolderpath); //3 01,23,45,6
	if (0 == index) {
		return FALSE;
	}

	while (index > 0) {
		index--;

		if ((ucParent[index*2] == '\\') && (ucParent[index*2+1] == 0)) {
			ucParent[index*2] = ucParent[index*2+1] = 0;
			break;
		} else if ( ucParent[index*2] == '/' && ucParent[index*2+1] == 0) {
			ucParent[index*2] = ucParent[index*2+1] = 0;
			break;
		}

		ucParent[index*2] = ucParent[index*2+1] = 0;    
	}
	return TRUE;
}

/**
* @brief Check the permission of creating a new folder is allowed or not
* @param ucfolderPath the folder path to be created
* @return TRUE allow
*/
BT_BOOL btmtk_ftps_is_allow_create_folder(const U16 *ucFolderPath) {

	if (FALSE != g_ftps_read_only) {
		return FALSE;
	}

	// PTS TC_SERVER_FBR_BV_08_I
	if (0 == ext_ucs2cmp((const U8*) ucFolderPath,
			(const U8*) L"d:\\temp\\ftp\\sub1")) {
		return FALSE;
	}

	return TRUE;
}

/**
* @brief check the read only permission when client connects to server.
* When a FTP connects to server, this function will be invoke to check the read only attribute of FTP server.
* @return TRUE - reset successfully
*/
BT_BOOL btmtk_ftps_reset_read_only_permission(){
	g_ftps_read_only = FALSE;
	return TRUE;
}

/**
* @brief Why to create folder/add file/delete file will check this value for permission
* @return TRUE - Not Allow to create folder
*/
BT_BOOL btmtk_ftps_is_read_only(){
	return g_ftps_read_only;
}

/**
* @brief Set the FTP server to as read_only or not
*/
BT_BOOL btmtk_ftps_setup_read_only(BT_BOOL read_only, BT_BOOL *result){
	g_ftps_read_only = read_only;
	*result = g_ftps_read_only;
	return TRUE;
}

/**
* @brief WARNING: If this function is called, there may be someting bad happened.
*/
BT_BOOL btmtk_ftps_force_clear() {
	if (act_server_cntx_p == NULL) {
		return FALSE;
	}

	// close the opened file descriptors
	if (FALSE != ext_fs_is_valid_handle_ucs2(act_server_cntx_p->fh_ftps_send)) {
		btmtk_goep_close_rstream(act_server_cntx_p->fh_ftps_send);
		act_server_cntx_p->fh_ftps_send = FHANDLE_INVALID_VALUE;
	}

	if (FALSE != ext_fs_is_valid_handle_ucs2(act_server_cntx_p->fh_ftps_recv)) {
		btmtk_goep_close_wstream(act_server_cntx_p->fh_ftps_recv);
		act_server_cntx_p->fh_ftps_recv = FHANDLE_INVALID_VALUE;
	}

	act_server_cntx_p = NULL;
	return TRUE;
}

/**
* @brief WARNING: If this function is called, there may be someting bad happened.
*/
BT_BOOL btmtk_ftpc_force_clear() {
	if (act_client_cntx_p == NULL) {
		return FALSE;
	}

	if (FALSE != ext_fs_is_valid_handle_ucs2(act_client_cntx_p->fh_ftpc_push)) {
		btmtk_goep_close_rstream(act_client_cntx_p->fh_ftpc_push);
		act_client_cntx_p->fh_ftpc_push = FHANDLE_INVALID_VALUE;
	}

	if (FALSE != ext_fs_is_valid_handle_ucs2(act_client_cntx_p->fh_ftpc_recv)) {
		// close it and delete it
		btmtk_goep_close_wstream(act_client_cntx_p->fh_ftpc_recv);
		act_client_cntx_p->fh_ftpc_recv = FHANDLE_INVALID_VALUE;

		// delete it by the name in 
		btmtk_goep_delete_file((const U16*) act_client_cntx_p->ftpc_filepath);
		btmtk_goep_clear_filepath((U8 *) act_client_cntx_p->ftpc_filepath);
	}

	return TRUE;
}

BT_BOOL btmtk_ftps_is_sdcard(const U16 *ucPath) {
	U8 tmp_str[50];
	ext_chset_ucs2_to_utf8_string(tmp_str, sizeof(tmp_str), (U8 *) ucPath);

	if (ext_strncmp((const S8*)tmp_str, FTP_SDCARD, sizeof(FTP_SDCARD) - 1) == 0) {
		return TRUE;
	}

	if (ext_strncmp((const S8*)tmp_str, FTP_EXTERNAL_SD, sizeof(FTP_EXTERNAL_SD) - 1) == 0) {
		return TRUE;
	}

	return FALSE;
}

BT_BOOL btmtk_ftps_is_available_sdcard(const U16 *ucPath, const U16 *ucSdcard) {
	U16 temp_path[64];
	U8 utf8_path[64];

	struct statfs file_state;

	/* For debugging
	U8 utf8_base[32], utf8_target[32];
	ext_chset_ucs2_to_utf8_string(utf8_base, sizeof(utf8_base), ucPath);
	ext_chset_ucs2_to_utf8_string(utf8_target, sizeof(utf8_target), ucSdcard);
	bt_ext_log("[BT][FTP] %s() base: %s, target: %s", __FUNCTION__, utf8_base, utf8_target);
	*/

	bt_ftp_compose_path((const U8*)ucPath, (const U8*)ucSdcard, (U8*)temp_path, sizeof(temp_path)/2);
	ext_chset_ucs2_to_utf8_string(utf8_path, sizeof(utf8_path), (const U8*)temp_path);

	if (statfs((const char*)utf8_path, &file_state) == -1) {
		bt_ext_log("[BT][FTP] Failed to get the SD card status: %s, errno: %d", utf8_path, errno);
		return FALSE;
	} else {
		if (file_state.f_blocks == 0) {
			bt_ext_log("[BT][FTP] SD card is not ready: %s", utf8_path);
			return FALSE;
		} else {
			return TRUE;
		}
	}
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftp_compose_path
 * DESCRIPTION
 *  Compose a complete file path in UCS2 format as [base/target]
 * PARAMETERS
 *  *base	: Base path in UCS2 format
 *  *target	: Target name in UCS2 format
 *  *res_path	: The composed path in UCS2 format
 *  u4MaxSize	: Max allowed length of path
 * RETURNS
 *  Length of composed path.
 *****************************************************************************/
U32 bt_ftp_compose_path(const U8 *base, const U8 *target, U8 *res_path, U32 u4MaxSize) {
	S32 idx, len;

	ext_ucs2ncpy((S8 *) res_path, (const S8 *) base, u4MaxSize);
	len = ext_ucs2strlen((const S8 *) res_path);
	if (len > 0) {
		idx = (len - 1) * 2;
		if ((res_path[idx] == '\\' || res_path[idx] == '/') && res_path[idx + 1] == 0) {
			res_path[idx] = res_path[idx + 1] = 0;
		}
	}

	ext_ucs2ncat((S8 *) res_path, (const S8 *) FTP_FS_PATH_DELIMITOR, u4MaxSize);
	len = ext_ucs2strlen((const S8 *) target);
	if (len > 0) {
		if ((target[0] == '\\' || target[0] == '/') && target[1] == 0) {
			target += 2;
		} else if ((target[1] == '\\' || target[1] == '/') && target[0] == 0) {
			target += 2;
		}
	}

	ext_ucs2ncat((S8 *) res_path, (const S8 *) target, u4MaxSize);
	len = ext_ucs2strlen((const S8 *) res_path);
	if (len > 0) {
		idx = (len - 1) * 2;
		if ((res_path[idx] == '\\' || res_path[idx] == '/') && res_path[idx + 1] == 0) {
			res_path[idx] = res_path[idx + 1] = 0;
		}
	}
	return ext_ucs2strlen((const S8 *)res_path);
}

U64 bt_ftp_get_free_bytes(const U16 *ucPath) {
	U64 ret = 0;
	U8 utf8_path[FMGR_MAX_PATH_LEN * ENCODING_LENGTH];
	struct statfs path_state;

	ext_chset_ucs2_to_utf8_string(utf8_path, sizeof(utf8_path), (U8 *) ucPath);
	if (statfs((const char*)utf8_path, &path_state) == -1) {
		bt_ext_log("[BT][FTP] %s() Failed to get path state. %s, errno: %d",
			__FUNCTION__, utf8_path, errno);

	} else {
		ret = path_state.f_bfree * path_state.f_bsize;
	}

	return ret;
}

BT_BOOL btmtk_ftp_delete_file(const U16 *pwsFilename) {
	return ext_fs_delete_ucs2(pwsFilename);
}

BT_BOOL btmtk_ftp_delete_folder(const U16 *pwsFoldername) {
	return ext_fs_delete_dir_ucs2(pwsFoldername);
}

///
/***************************************************************************** 
* CONTEXT
*****************************************************************************/


#if defined(__BT_FTS_PROFILE__) || defined(__BT_FTC_PROFILE__)

/*****************************************************************************
 * FUNCTION
 *  bt_ftp_htons
 * DESCRIPTION
 *  Convert short integer (16 bits) from host byte order to network byte order
 * PARAMETERS
 *  a       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 bt_ftp_htons(U16 a)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	return (((a & 0xFF) << 8) | (((a >> 8) & 0xFF)));
}

/**
* @brief Convert a UCS2 string from host order to network order 
* bt_ftp_ucs2_htons
* @param  h_order_name host order
* @param  n_order_name network order        
**/
void bt_ftp_ucs2_htons(U8 *h_order_name, U8 *n_order_name)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U32 i;
	U8 t;
	U32 bytes_len = ext_ucs2strlen((S8 *) n_order_name) * 2;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	//FTP_DEBUG(kal_print("FTP OBJ NAME SWAPCPY\n");)
	bt_ext_log("[FTPC][UTIL] ucs2_htons len:(%d)", (S32) bytes_len);
	for (i = 0; i < bytes_len; i += 2)    /* includes null temination */
	{
		t = n_order_name[i];
		h_order_name[i] = n_order_name[i + 1];
		h_order_name[i + 1] = t;
	}

	h_order_name[bytes_len] = 0;
	h_order_name[bytes_len + 1] = 0;
}

/**
* @brief Get the listing xml file of FTPC
* @return TRUE - auto get the xml file
*/
BT_BOOL btmtk_ftpc_auto_get_listing_xml(void){
	return FALSE;
}


/**
* @brief Setup the default FTP Client's receive filepath
*/
const U8 *btmtk_ftpc_get_default_receive_filepath(){
    return (const U8*)L"d:\\temp\\ftp\\temp.tmp";
}

S32 btmtk_ftpc_get_push_progress(long long *c, long long *t) {
	S32 ret = 0;

	*c = (long long) (act_client_cntx_p->ftpc_total_len - act_client_cntx_p->ftpc_push_remain);
	*t = (long long) act_client_cntx_p->ftpc_total_len;

	return ret;
}

S32 btmtk_ftpc_get_pull_progress(long long *c, long long *t) {
	S32 ret = 0;

	*c = (long long) act_client_cntx_p->ftpc_data_len;
	*t = (long long) act_client_cntx_p->ftpc_total_len;

	return ret;
}

/* return the last or current filesize Note: it maybe 0*/
U32 btmtk_ftpc_get_current_filesize(){
	if ( act_client_cntx_p ){
		return act_client_cntx_p->ftpc_total_len;
	}
	return 0;
}
/* return the last or current filesize */
U32 btmtk_ftps_get_current_filesize(){
	if( act_server_cntx_p ){
		return act_server_cntx_p->total_send_obj_len;
	}
	return 0;
}

S32 btmtk_ftpc_get_conntype(){
	if( act_client_cntx_p ){
		return act_client_cntx_p->ftpc_conntype;
	}		
	return 0;
}

S32 btmtk_ftpc_get_mru(){
	if( act_client_cntx_p ){
		return act_client_cntx_p->ftpc_remote_mtu;
	}
	return 0;
}

S32 btmtk_ftpc_get_srm(){
	if( act_client_cntx_p ){
		return act_client_cntx_p->ftpc_remote_srm;
	}	
	return 0;
}

#ifdef BTMTK_ON_LINUX
/*****************************************************************************
 * FUNCTION
 *  bt_ftp_init_socket
 * DESCRIPTION
 *  This function initializes the socket for receiving ilm from bt task and
 *  stores the address of sending socket.
 * PARAMETERS
 *  none.        
 * RETURNS
 *  Created socket file descriptor or -1 (socket-creation failed)
 *****************************************************************************/
int bt_ftp_init_socket()
{
	int err;

	err = socket_make_sockaddr_un(BT_SOCK_NAME_EXT_ADP_FTP,
			  ANDROID_SOCKET_NAMESPACE_ABSTRACT,
			  &g_ext_ftp_sockaddr, &g_ext_ftp_sockaddr_len);

	g_ext_ftp_sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (g_ext_ftp_sockfd < 0) {
		bt_ext_err("Create server socket failed.");
		return -1;
	}

	if (0 > bind(g_ext_ftp_sockfd, (struct sockaddr*) &g_ext_ftp_sockaddr,
			g_ext_ftp_sockaddr_len)) {
		bt_ext_err("Bind server socket failed.");
		return -1;
	}

	g_int_adp_sockaddr.sun_family = AF_UNIX;
	strcpy(g_int_adp_sockaddr.sun_path, BT_SOCK_NAME_INT_ADP);
	g_int_adp_sockaddr_len = (
			offsetof(struct sockaddr_un, sun_path) + 
			strlen(g_int_adp_sockaddr.sun_path) + 1);

	return g_ext_ftp_sockfd;
}

/** 
* @brief Clear data before read from the socket
*/
int bt_ftp_clear_socket() {
	char buff[20];
	int err, sock_fl;

	// Get current socket flags
	sock_fl = fcntl(g_ext_ftp_sockfd, F_GETFL);
	if (sock_fl == -1) {
		err = errno;
		bt_ext_err("[BT][FTP] Get socket flags failed. Reason: [%d]%s", err, strerror(err));
		return -1;
	}

	// Set ftp socket as non-block
	if (fcntl(g_ext_ftp_sockfd, F_SETFL, sock_fl | O_NONBLOCK) == -1) {
		err = errno;
		bt_ext_err("[BT][FTP] Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
		return -1;
	}

	// Read data from socket till empty
	while(0 < recvfrom(g_ext_ftp_sockfd, (void*) buff, sizeof(buff), 0, NULL, NULL)) {
		// bt_ext_log("[BT][FTP] Clearing socket...");
	}
	bt_ext_log("[BT][FTP] Socket clear.");

	// Set back original flags
	if (fcntl(g_ext_ftp_sockfd, F_SETFL, sock_fl) == -1) {
		err = errno;
		bt_ext_err("[BT][FTP] Set O_NONBLOCK flag to socket failed. Reason: [%d]%s", err, strerror(err));
		return -1;
	}

	return 0;
}

/**
* @brief Send ilm via socket
*/
static void bt_ftp_send_msg_socket(msg_type msg_id, module_type dest_mod,
				   void *ptr, U16 size) {
	int ret = -1;
	struct sockaddr_un* addr;
	socklen_t* addr_len;
	ilm_struct ilm;
	U16 *field;

	// bt_ext_log("bt_ftp_send_msg, msg:[%d] mod:[%d]", msg_id, dest_mod);

	GOEP_MEMSET(&ilm, 0, sizeof(ilm_struct));
	ilm.msg_id = msg_id;
	ilm.src_mod_id = MOD_EXTERNAL;
	ilm.dest_mod_id = dest_mod;

	assert (size <= MAX_ILM_BUFFER_SIZE);

	if(ptr != NULL)
		memcpy(ilm.ilm_data, ptr, size);

	// LOCAL_PARA_HDR
	field = (U16 *)ilm.ilm_data;
	*field = 1;
	field = (U16 *)&ilm.ilm_data[2];
	*field = size;

	// cal whole ilm size
	size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + size;

	if (g_ext_ftp_sockfd) {
		if (dest_mod == MOD_EXTERNAL) {
			addr = &g_ext_ftp_sockaddr;
			addr_len = &g_ext_ftp_sockaddr_len;
		} else {
			addr = &g_int_adp_sockaddr;
			addr_len = &g_int_adp_sockaddr_len;
		}

		ret = sendto(g_ext_ftp_sockfd, (void *)&ilm, size, 0,
				 (struct sockaddr*) addr, *addr_len);
		if (ret < 0) {
			bt_ext_err("Sending msg failed: %s, %d", strerror(errno), errno);
		} else {
			// bt_ext_log("Sending msg succeeded: %d", ret);
		}

	} else {
		bt_ext_err("[BT][FTP] Socket is not yet initialized.");
	}
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_ftp_send_msg
 * DESCRIPTION
 *  This function is to send msg to BT task
 * PARAMETERS
 *  msg_id              [IN]        
 *  p_local_para        [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void bt_ftp_send_msg(U32 msg_id, void *p_local_para, U32 u4Size)
{
	btmtk_goep_show_id( msg_id, TRUE, 2); //MOD_MMI
#ifdef BTMTK_ON_LINUX
	bt_ftp_send_msg_socket(msg_id, MOD_BT, p_local_para, (U16) u4Size);
#else
	BTCMD_SendMessage(msg_id, MOD_BT, p_local_para, (U16) u4Size );
#endif

	//  NOTE: free the p_local_para
	if (NULL != p_local_para ) {
		FREE_LOCAL_PARA((void*)p_local_para);
	}
}

void bt_ftp_send_msg_toself(U32 msg_id, void *p_local_para, U32 u4Size)
{
	// NOTE: send msg
	btmtk_goep_show_id( msg_id, TRUE, 2); //MOD_MMI
#ifdef BTMTK_ON_LINUX
	bt_ftp_send_msg_socket(msg_id, MOD_EXTERNAL, p_local_para, (U16) u4Size);
#else
	BTCMD_SendMessage( msg_id, MOD_EXTERNAL, p_local_para, (U16) u4Size ) ;
#endif

	//  NOTE: free the p_local_para
	if (NULL != p_local_para ) {
		FREE_LOCAL_PARA((void*)p_local_para);
	}
}

const char *btmtk_ftps_get_event_name(U32 u4Event) {
	switch(u4Event) {
		case BT_FTPSUI_EVENT_NONE:
			return "BT_FTPSUI_EVENT_NONE";
		case BT_FTPSUI_SHUTDOWNING: /* deinit Param: none */
			return "BT_FTPSUI_SHUTDOWNING";
		case BT_FTPSUI_READY: /* server register ok Param: none */
			return "BT_FTPSUI_READY";
		case BT_FTPSUI_AUTHORIZING: /* server receive a incoming */
			return "BT_FTPSUI_AUTHORIZING";
		case BT_FTPSUI_AUTHEN_WAIT: /* server receive a client_challenge */
			return "BT_FTPSUI_AUTHEN_WAIT";
		case BT_FTPSUI_CONNECTING:
			return "BT_FTPSUI_CONNECTING";
		case BT_FTPSUI_CONNECTED: /* Param: none */
			return "BT_FTPSUI_CONNECTED";
		case BT_FTPSUI_SET_FOLDER_START: /* Param: folder name. remote change folder */
			return "BT_FTPSUI_SET_FOLDER_START";
		case BT_FTPSUI_SET_FOLDERED: /* Param: folder name. remote change folder */
			return "BT_FTPSUI_SET_FOLDERED";
		case BT_FTPSUI_BROWSE_START: /* Param: current folder. remote request to get the xml file */
			return "BT_FTPSUI_BROWSE_START";
		case BT_FTPSUI_BROWSING: 
			return "BT_FTPSUI_BROWSING";
		case BT_FTPSUI_BROWSED:
			return "BT_FTPSUI_BROWSED";
		case BT_FTPSUI_PUSH_FILE_START: /* Param: filename in unicode */
			return "BT_FTPSUI_PUSH_FILE_START";
		case BT_FTPSUI_PUSHING: /* Param: Int Percentage */
			return "BT_FTPSUI_PUSHING";
		case BT_FTPSUI_PUSHED:
			return "BT_FTPSUI_PUSHED";
		case BT_FTPSUI_PULL_FILE_START: /* Param: filename in unicode */
			return "BT_FTPSUI_PULL_FILE_START";
		case BT_FTPSUI_PULLING: /* Param: Int Percentage */
			return "BT_FTPSUI_PULLING";
		case BT_FTPSUI_PULLED:
			return "BT_FTPSUI_PULLED";
		case BT_FTPSUI_FILE_DELETE:
			return "BT_FTPSUI_FILE_DELETE";
		case BT_FTPSUI_FOLDER_DELETE:
			return "BT_FTPSUI_FOLDER_DELETE";
		case BT_FTPSUI_FILE_CREATE:
			return "BT_FTPSUI_FILE_CREATE";
		case BT_FTPSUI_FOLDER_CREAT_START:
			return "BT_FTPSUI_FOLDER_CREAT_START";
		case BT_FTPSUI_ABORTED:
			return "BT_FTPSUI_ABORTED";
		case BT_FTPSUI_DISCONNECTED:
			return "BT_FTPSUI_DISCONNECTED";
		case BT_FTPSUI_ERROR:
			return "BT_FTPSUI_ERROR";
	}

	return "unknow";
}

void btmtk_ftps_ui_callback(U32 u4Event, U8 *ucData) {
	// printf( "[ftps] UI Event (%s)\n", btmtk_ftps_get_event_name(u4Event) );
	if (NULL != g_ftps_ui_callback) {
		g_ftps_ui_callback(u4Event, ucData);
	}
}

const char *btmtk_ftpc_get_event_name(U32 u4Event) {
	switch (u4Event) {
		case BT_FTPCUI_DEACTVE:
			return "BT_FTPCUI_DEACTVE";
		case BT_FTPCUI_ACTIVE:
			return "BT_FTPCUI_ACTIVE";
		case BT_FTPCUI_CONNECTING:
			return "BT_FTPCUI_CONNECTING";
		case BT_FTPCUI_AUTHEN_WAIT:  /* Received a server-challenge */
			return "BT_FTPCUI_AUTHEN_WAIT";            
		case BT_FTPCUI_CONNECTED:
			return "BT_FTPCUI_CONNECTED";
		case BT_FTPCUI_DISCONNECTING:
			return "BT_FTPCUI_DISCONNECTING";
		case BT_FTPCUI_PUSHING:
			return "BT_FTPCUI_PUSHING";
		case BT_FTPCUI_PUSHED:
			return "BT_FTPCUI_PUSHED";
		case BT_FTPCUI_BROWSING:
			return "BT_FTPCUI_BROWSING";
		case BT_FTPCUI_BROWSED:
			return "BT_FTPCUI_BROWSED";
		case BT_FTPCUI_PULLING:
			return "BT_FTPCUI_PULLING";
		case BT_FTPCUI_PULLED:
			return "BT_FTPCUI_PULLED";
		case BT_FTPCUI_SETPATHING:
			return "BT_FTPCUI_SETPATHING";
		case BT_FTPCUI_SETPATHED:
			return "BT_FTPCUI_SETPATHED";
		case BT_FTPCUI_ABORTING:
			return "BT_FTPCUI_ABORTING";
		case BT_FTPCUI_FILE_DELETED:
			return "BT_FTPCUI_FILE_DELETED";
		case BT_FTPCUI_FOLDER_DELETED:
			return "BT_FTPCUI_FOLDER_DELETED";
		case BT_FTPCUI_FOLDER_CREATED:
			return "BT_FTPCUI_FOLDER_CREATED";
		case BT_FTPCUI_DISCONNECTED:
			return "BT_FTPCUI_DISCONNECTED";
		case BT_FTPCUI_ERROR:
			return "BT_FTPCUI_ERROR";
		case BT_FTPCUI_MAX:
			return "BT_FTPCUI_MAX";
		default:
			return "unknow";
	}

	return "unknow";
}


/**
* @brief This function  remove the space in the font, and cut the remain name to requsted size.
* 
* All strings in this funcion must be Unicode encoded.
* @param dest_size the buffer size including the  end Zero 
* @param src  host sequence
* @param dest host sequence
**/
void bt_ftp_ucs2_cut_name_with_ext(U8 *dest, U32  dest_size, const U8 *src)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 *src_no_space;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	src_no_space = (U8*)ext_ucs2skipspace((U8 *)src);


	/* need not to cut*/
	if(ext_ucs2strlen((S8 *)src_no_space) <= (S32)(dest_size/ENCODING_LENGTH-1))
	{
		ext_ucs2ncpy((S8 *)dest, (const S8 *)src_no_space, (U32)(dest_size/ENCODING_LENGTH -1));
	}
	else
	{
		char *p_ext_name;
		U32  ext_name_len = 0;

		/* 1. calculate the extend name length
		 * 2. cut
		 * 3. cat
		 */

		p_ext_name = ext_ucs2rchr((const S8 *)src_no_space, (U16)(L'.'));
		if(p_ext_name)
		{
			ext_name_len = ext_ucs2strlen((const S8 *)p_ext_name);
		}
		else
		{
			ext_name_len = 0;
		}

		ext_ucs2ncpy((S8 *)dest, (const S8 *)src_no_space, FMGR_MAX_PATH_LEN);
		ext_ucs2ncat((S8 *)dest, (const S8 *)p_ext_name, FMGR_MAX_PATH_LEN);
	}
}

/*****************************************************************************
 * FUNCTION
 *  bt_ftp_htons
 * DESCRIPTION
 *  This function  remove the space in the font, and cut the remain name to requsted size.
 * 
 *  All strings in this funcion must be Unicode encoded.
 *  dest_size: is the buffer size including the  end Zero 
 * PARAMETERS
 *  a       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
void bt_ftp_ucs2_cut_name_without_ext(U8 *dest, U32  dest_size, const U8 *src)
{
	/*----------------------------------------------------------------*/
	/* Local Variables                                                */
	/*----------------------------------------------------------------*/
	U8 *src_no_space;

	/*----------------------------------------------------------------*/
	/* Code Body                                                      */
	/*----------------------------------------------------------------*/
	src_no_space = (U8*)ext_ucs2skipspace((U8 *) src);

	/*copy max dest_size/2 - 1 chars*/
	ext_ucs2ncpy((S8 *)dest, (const S8 *)src_no_space, (U32)(dest_size/2 -1));
    
}

/**
* @brief delete file used by ftp composed by parameters
* bt_ftp_util_delete_file
*/
void bt_ftp_util_delete_file(const U8* absolute_path, const U8* file_name)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *all_path_name_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //BT_TRACE(BT_FTP_TRC_CLASS, BT_FTP_DELETE_FILE);
    all_path_name_ptr = GET_CTRL_BUFFER(FMGR_MAX_PATH_LEN * ENCODING_LENGTH);
    BT_FTP_ASSERT(all_path_name_ptr != NULL);
    GOEP_MEMSET(all_path_name_ptr, 0, FMGR_MAX_PATH_LEN * ENCODING_LENGTH);

    /* form obj name */
    ext_ucs2ncpy((S8 *) all_path_name_ptr, (S8 *) absolute_path, FMGR_MAX_PATH_LEN-1);
    ext_ucs2ncat((S8 *) all_path_name_ptr, (S8 *) file_name, FMGR_MAX_PATH_LEN-1);

    btmtk_goep_delete_file((U16 *) all_path_name_ptr);
    // GOEP_TRACE_PATH( BT_FTP_TRC_CLASS, BT_FTP_UTIL_DELETE_FILE, all_path_name_ptr );

    if (all_path_name_ptr)
    {
        FREE_CTRL_BUFFER((void *) all_path_name_ptr);
    }
}

/**
* @brief little endian to big endian
*/
U16 bt_goep_ntohs(U16 s)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (((s       & 0xFF) << 8) | \
           (((s >> 8) & 0xFF)));
}

#if 0
/**
* @brief This function  remove the space in the font, and cut the remain name to requsted size.
* 
* All strings in this funcion must be Unicode encoded.
* @param dest_size the buffer size including the  end Zero 
* @param src  host sequence
* @param dest host sequence
**/
void bt_ftp_ucs2_cut_name_with_ext(U8 *dest, U32  dest_size, const U8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *src_no_space;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    src_no_space = (U8*)ext_ucs2skipspace((S8*)src);


    /* need not to cut*/
    if(ext_ucs2strlen((S8 *)src_no_space) <= (S32)(dest_size/ENCODING_LENGTH-1))
    {
        ext_ucs2ncpy((S8 *)dest, (const S8 *)src_no_space, (U32)(dest_size/ENCODING_LENGTH -1));
    }
    else
    {
        char *p_ext_name;
        U32  ext_name_len = 0;

        /* 1. calculate the extend name length
          * 2. cut
          * 3. cat
          */

        p_ext_name = ext_ucs2rchr((const S8 *)src_no_space, (U16)(L'.'));
        if(p_ext_name)
        {
            ext_name_len = ext_ucs2strlen((const S8 *)p_ext_name);
        }
        else
        {
            ext_name_len = 0;
        }

        ext_ucs2ncpy((S8 *)dest, (const S8 *)src_no_space, FMGR_MAX_PATH_LEN);
        ext_ucs2ncat((S8 *)dest, (const S8 *)p_ext_name, FMGR_MAX_PATH_LEN);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_ftp_htons
 * DESCRIPTION
 *  This function  remove the space in the font, and cut the remain name to requsted size.
 * 
 *  All strings in this funcion must be Unicode encoded.
 *  dest_size: is the buffer size including the  end Zero 
 * PARAMETERS
 *  a       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static void bt_ftp_ucs2_cut_name_without_ext(U8 *dest, U32  dest_size, const U8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 *src_no_space;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    src_no_space = (U8*)ext_ucs2skipspace((S8*)src);

    /*copy max dest_size/2 - 1 chars*/
    ext_ucs2ncpy((S8 *)dest, (const S8 *)src_no_space, (U32)(dest_size/2 -1));

}
#endif
#endif
