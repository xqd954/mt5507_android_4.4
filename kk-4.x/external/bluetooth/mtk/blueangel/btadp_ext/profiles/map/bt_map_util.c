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
 * Bt_map_util.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file contains MAP utility functions.
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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_ext_utility.h"
//#include "bt_common.h" //temp for trace
#include "ext_osal.h"

#include "bluetooth_map_struct.h"
#include "bt_map_api.h"
#include "bt_ext_debug.h"

#include "bt_map_util.h"

extern map_ext_cntx_struct g_pi_map_cntx;

void map_postevent(U32 event, void* arg)
{

	if (g_pi_map_cntx.MMI_callback != NULL) {
		g_pi_map_cntx.MMI_callback(event, (S8*)arg);
	}
}
bt_map_result_enum btmtk_map_util_translate_result_mbt2bt(MBT_AUTHRES from)
{
    switch(from)
    {
	case MBT_ALLOW_RES:
        return BT_MAP_SUCCESS;
    case MBT_FORBID_RES:
	case MBT_FORBIDDEN_RES:
        return BT_MAP_FAIL_FORBIDDEN;
	case MBT_UNAUTHORIZED_RES:
        return BT_MAP_FAIL_UNAUTHORIZED;
	case MBT_NOT_FOUND_RES:
        return BT_MAP_FAIL_NOT_FOUND;
	case MBT_DATABASE_FULL_RES:
        return BT_MAP_FAIL_STORAGE_FULL;
	case MBT_BAD_REQUEST_RES:
        return BT_MAP_FAIL_BAD_FORMAT;
	case MBT_PRECONDITION_FAILED_RES:
        return BT_MAP_FAIL_INVALID_PARAMETER;
	case MBT_NOT_IMPLEMENTED_RES:
        return BT_MAP_FAIL_NOT_SUPPORT;
	case MBT_SERVICE_UNAVAILABLE_RES:
        return BT_MAP_FAIL_NO_RESOURCE;
	case MBT_NOT_ACCEPTABLE_RES:
	case MBT_UNSUPPORTED_MEDIA_TYPE_RES:
	case MBT_INTERNAL_SERVER_ERROR_RES:
	case MBT_UNSUPPORTED_DEVICE_TYPE_RES:
	case MBT_ERROR_RES:
    default:
        return BT_MAP_FAIL;
    }
}
bt_map_msg_enum btmtk_map_util_translate_msg_type_mbt2bt(MBT_MAP_MSG_TYPE from)
{
    bt_map_msg_enum to = BT_MAP_MSG_NONE;

    if (from & MBT_MAP_MSG_TYPE_SMS_GSM)
    {
        to |= BT_MAP_MSG_SMS_GSM;
    }

    if (from & MBT_MAP_MSG_TYPE_SMS_CDMA)
    {
        to |= BT_MAP_MSG_SMS_CDMA;
    }

    if (from & MBT_MAP_MSG_TYPE_EMAIL)
    {
        to |= BT_MAP_MSG_EMAIL;
    }

    if (from & MBT_MAP_MSG_TYPE_MMS)
    {
        to |= BT_MAP_MSG_MMS;
    }

    return to;
}
MBT_MAP_MSG_TYPE btmtk_map_util_translate_msg_type_bt2mbt(bt_map_msg_enum from)
{
    MBT_MAP_MSG_TYPE to = 0;

    if (from & BT_MAP_MSG_SMS_GSM)
    {
        to |= MBT_MAP_MSG_TYPE_SMS_GSM;
    }

    if (from & BT_MAP_MSG_SMS_CDMA)
    {
        to |= MBT_MAP_MSG_TYPE_SMS_CDMA;
    }

    if (from & BT_MAP_MSG_EMAIL)
    {
        to |= MBT_MAP_MSG_TYPE_EMAIL;
    }

    if (from & BT_MAP_MSG_MMS)
    {
        to |= MBT_MAP_MSG_TYPE_MMS;
    }

    return to;
}



MBT_MAP_MSG_READ_STATUS btmtk_map_util_translate_filter_msg_type_bt2mbt(bt_map_filter_msg_enum from)
{
    MBT_MAP_MSG_TYPE_MASK to = 0;

    if (from & BT_MAP_FILTER_MSG_SMS_GSM)
    {
        to |= MBT_MAP_MSG_TYPE_MASK_SMS_GSM;
    }

    if (from & BT_MAP_FILTER_MSG_SMS_CDMA)
    {
        to |= MBT_MAP_MSG_TYPE_MASK_SMS_CDMA;
    }

    if (from & BT_MAP_FILTER_MSG_EMAIL)
    {
        to |= MBT_MAP_MSG_TYPE_MASK_EMAIL;
    }

    if (from & BT_MAP_FILTER_MSG_MMS)
    {
        to |= MBT_MAP_MSG_TYPE_MASK_MMS;
    }

    return to;
}

MBT_MAP_MSG_READ_STATUS btmtk_map_util_translate_filter_status_bt2mbt(bt_map_filter_status_enum from)
{
    MBT_MAP_MSG_READ_STATUS to = 0;

    if (from == BT_MAP_FILTER_STATUS_ALL)
    {
        return MBT_MAP_MSG_STATUS_NO_FILTERING;
    }

    if (from & BT_MAP_FILTER_STATUS_UNREAD)
    {
        to |= MBT_MAP_MSG_STATUS_UNREAD;
    }

    if (from & BT_MAP_FILTER_STATUS_READ)
    {
        to |= MBT_MAP_MSG_STATUS_READ;
    }

    return to;
}

MBT_MAP_PRI_STATUS btmtk_map_util_translate_filter_priority_bt2mbt(bt_map_filter_prio_enum from)
{
    MBT_MAP_PRI_STATUS to = 0;

    if (from == BT_MAP_FILTER_PRIO_ALL)
    {
        return MBT_MAP_PRI_STATUS_NO_FILTERING;
    }

    if (from & BT_MAP_FILTER_PRIO_HIGH)
    {
        to |= MBT_MAP_PRI_STATUS_HIGH;
    }

    if (from & BT_MAP_FILTER_PRIO_NOT_HIGH)
    {
        to |= MBT_MAP_PRI_STATUS_NON_HIGH;
    }

    return to;
}

MBT_MAP_MSG_CHARSET btmtk_map_util_translate_charset_bt2mbt(bt_map_charset_enum from)
{
    MBT_MAP_MSG_CHARSET to = 0;

    if (from == BT_MAP_CHARSET_NATIVE)
    {
        return MBT_MAP_MSG_CHARSET_NATIVE;
    }

    if (from & BT_MAP_CHARSET_UTF8)
    {
        to |= MBT_MAP_MSG_CHARSET_UTF8;
    }

    return to;
}

MBT_MAP_FRAC_REQ btmtk_map_util_translate_fraction_req_bt2mbt(bt_map_fraction_req_enum from)
{
    switch (from)
    {
    case BT_MAP_FRACTION_REQ_FIRST:
        return MBT_MAP_FRAC_REQ_FIRST;
    case BT_MAP_FRACTION_REQ_NEXT:
        return MBT_MAP_FRAC_REQ_NEXT;
    default:
        return MBT_MAP_FRAC_REQ_NO;
    }
}

bt_map_fraction_rsp_enum btmtk_map_util_translate_fraction_rsp_mbt2bt(MBT_MAP_FRAC_DELIVER from)
{
    switch (from)
    {
    case MBT_MAP_FRAC_DELIVER_MORE:
        return BT_MAP_FRACTION_RSP_MORE;
    case MBT_MAP_FRAC_DELIVER_LAST:
        return BT_MAP_FRACTION_RSP_LAST;
    default:
        return BT_MAP_FRACTION_RSP_NO;
    }
}

MBT_MAP_TRANSP_TYPE btmtk_map_util_translate_transparent_bt2mbt(bt_map_sent_op_enum from)
{
    switch (from)
    {
    case BT_MAP_SAVE_AND_SENT:
        return MBT_MAP_TRANSP_TYPE_OFF;
    case BT_MAP_SENT:
        return MBT_MAP_TRANSP_TYPE_ON;
    default:
        return MBT_MAP_TRANSP_TYPE_OFF;
    }
}

MBT_MAP_MAS_STATUS *btmtk_map_util_search_mas_instance(MBT_BYTE instance)
{
	U8 i;
	for(i = 0; i < MBT_MAP_MAX_MAS_INST_NUM; i++) 
	{
		if((g_pi_map_cntx.masStatus[i].registered == KAL_TRUE) && 
			(g_pi_map_cntx.masStatus[i].masInfo.MasInstId == instance))
		{
			return &g_pi_map_cntx.masStatus[i];
		}
	}
	return NULL;

}

S8 btmtk_map_util_search_mas_instance_index(MBT_BYTE instance)
{
	U32 index;
	for (index = 0; index < MBT_MAP_MAX_MAS_INST_NUM; index++)
	{
		if (instance ==	g_pi_map_cntx.masStatus[index].masInfo.MasInstId) 
		{
			return index;
		}
	}
	return -1;
}

MBT_MAP_MAS_STATUS *btmtk_map_util_search_unregistered_mas_instance(){
#if defined(BTMTK_ON_LINUX)
	U8 i;
	for(i = 0; i < MBT_MAP_MAX_MAS_INST_NUM; i++) 
	{
		if(g_pi_map_cntx.masStatus[i].registered == KAL_FALSE)
		{
			return &g_pi_map_cntx.masStatus[i];
		}
	}
#endif
	return NULL;
}


MBT_MAP_MAS_STATUS *btmtk_map_util_search_mas_client(MBT_BYTE mas_instance, MBT_BDADDR addr)
{
	U8 i;
	for(i = 0; i < MBT_MAP_MAX_CONN_NUM; i++) 
	{
	/*	if (g_pi_map_cntx.masStatus[i].registered == TRUE &&
			g_pi_map_cntx.masStatus[i].masInfo.MasInstId == mas_instance &&
			memcmp(&g_pi_map_cntx.masStatus[i].BdAddr, addr, sizeof(bt_addr_struct))== 0)
		{
			return &g_pi_map_cntx.masStatus[i];
		}	*/

		if (g_pi_map_cntx.masStatus[i].registered == TRUE &&
			g_pi_map_cntx.masStatus[i].masInfo.MasInstId == mas_instance &&
			g_pi_map_cntx.masStatus[i].BdAddr.lap == addr->lap &&
			g_pi_map_cntx.masStatus[i].BdAddr.uap == addr->uap &&
			g_pi_map_cntx.masStatus[i].BdAddr.nap == addr->nap)
		{
			return &g_pi_map_cntx.masStatus[i];
		}		

		
	}
    return NULL;
}

S8 btmtk_map_util_search_mas_client_index(MBT_BYTE mas_instance, MBT_BDADDR addr)
{
	U32 i;
  //  S8 inst_index = btmtk_map_util_search_mas_instance_index(mas_instance);
    for (i = 0; i < MBT_MAP_MAX_CONN_NUM; i++)
    {
     /*   if (g_pi_map_cntx.masStatus[i].masInfo.MasInstId == mas_instance &&
			memcmp(&g_pi_map_cntx.masStatus[i].BdAddr, addr, sizeof(bt_addr_struct)) == 0)
        {
            return i;
        }*/

		 if (g_pi_map_cntx.masStatus[i].masInfo.MasInstId == mas_instance &&
		 	g_pi_map_cntx.masStatus[i].BdAddr.lap == addr->lap &&
			g_pi_map_cntx.masStatus[i].BdAddr.uap == addr->uap &&
			g_pi_map_cntx.masStatus[i].BdAddr.nap == addr->nap)
		 {
		 	return i;
        }
    }
    return -1;

}

MBT_MAP_MNS_STATUS *btmtk_map_util_search_mns(MBT_BDADDR addr)
{
	U8 i;
	for(i = 0; i < MBT_MAP_MAX_MCE_NUM; i++) 
	{
	
	/*	if(memcmp(&g_pi_map_cntx.mnsStatus[i].BdAddr , addr ,sizeof(bt_addr_struct)) == 0)
		{
			return &g_pi_map_cntx.mnsStatus[i];
		}
		*/
		if (g_pi_map_cntx.mnsStatus[i].BdAddr.lap == addr->lap &&
			g_pi_map_cntx.mnsStatus[i].BdAddr.uap == addr->uap &&
			g_pi_map_cntx.mnsStatus[i].BdAddr.nap == addr->nap)
		{
			return &g_pi_map_cntx.mnsStatus[i];
		}
			
	}
	return NULL;
}

S8 btmtk_map_util_search_mns_index(MBT_BDADDR addr)
{
	U32 i;
    for (i = 0; i < MBT_MAP_MAX_MCE_NUM ; i++)
    {
     /*   if (memcmp(&g_pi_map_cntx.mnsStatus[i].BdAddr, addr, sizeof(bt_addr_struct)) == 0)
        {
            return i;
        }
		*/
	if (g_pi_map_cntx.mnsStatus[i].BdAddr.lap == addr->lap &&
		g_pi_map_cntx.mnsStatus[i].BdAddr.uap == addr->uap &&
		g_pi_map_cntx.mnsStatus[i].BdAddr.nap == addr->nap)
        {
            return i;
        }
    }
    return -1;
}

MBT_MAP_MNS_STATUS *btmtk_map_util_new_mns(void)
{
	U8 i;
	for(i = 0; i < MBT_MAP_MAX_MCE_NUM; i++) 
	{
		if (g_pi_map_cntx.mnsStatus[i].State == MBT_MAP_STATE_NONE)
		{
			return &g_pi_map_cntx.mnsStatus[i];
		}		
	}
    return NULL;
}

S8 btmtk_map_util_new_mns_index(void)
{
	U8 i;
	for(i = 0; i < MBT_MAP_MAX_MCE_NUM; i++) 
	{
		if (g_pi_map_cntx.mnsStatus[i].State == MBT_MAP_STATE_NONE)
		{
			return i;
		}		
	}
	return -1;
}

S8 btmtk_map_util_get_connection_num(MBT_BDADDR addr)
{
       U32 i;
	S8 count = 0;
	for (i = 0; i < MBT_MAP_MAX_MAS_INST_NUM; i++)
    {
        /*if (memcmp(&g_pi_map_cntx.masStatus[i].BdAddr, addr, sizeof(bt_addr_struct)) == 0)
        {
            count++;
        }*/
        if (g_pi_map_cntx.masStatus[i].BdAddr.lap == addr->lap &&
			g_pi_map_cntx.masStatus[i].BdAddr.uap == addr->uap &&
			g_pi_map_cntx.masStatus[i].BdAddr.nap == addr->nap)
        {
            count++;
        }
    }
    return count;

}

MBT_CHAR *btmtk_map_util_get_virtual_folder_path(MBT_CHAR *physical_path, MBT_CHAR *physical_root_path)
{
    if (strstr(physical_path, physical_root_path) == physical_path)
    {
        MBT_CHAR *vf = physical_path + strlen(physical_root_path);
        if (*vf == '/')
        {
            vf++;
        }
        return vf;
    }
    return NULL;
}

BT_BOOL btmtk_map_util_buffer2file(const char *path, char *buffer, U32 size)
{
    S32 fhandle, write;
    
    if ((fhandle = ext_fs_open((const U8*)path, EXT_FS_READ_WRITE | EXT_FS_CREATE)) < 0)
    {
        return FALSE;
    }

    if((write = ext_fs_write(fhandle, buffer, size)) < 0)
    {
        ext_fs_close(fhandle);
        return FALSE;
    }

    ext_fs_close(fhandle);
    return TRUE;
}

BT_BOOL btmtk_map_util_file2buffer(const char *path, char *buffer, U32 size)
{
    S32 fhandle, write;
    
    if ((fhandle = ext_fs_open((const U8*)path, EXT_FS_READ_WRITE | EXT_FS_CREATE)) < 0)
    {
        return FALSE;
    }

    if((write = ext_fs_write(fhandle, buffer, size)) < 0)
    {
        ext_fs_close(fhandle);
        return FALSE;
    }

    ext_fs_close(fhandle);
    return TRUE;
}

BT_BOOL btmtk_map_util_file_copy(const char *to, const char *from, U32 from_size)
{
    S32 rhandle, whandle, read, write, remain = from_size;

    if ((rhandle = ext_fs_open((const U8*)from, EXT_FS_READ_ONLY)) < 0)
    {
        return FALSE;
    }
    
    if ((whandle = ext_fs_open((const U8*)to, EXT_FS_READ_WRITE | EXT_FS_CREATE)) < 0)
    {
        ext_fs_close(rhandle);
        return FALSE;
    }

    while (remain)
    {
        if((read = ext_fs_read(rhandle, g_pi_map_cntx.file_buffer, MAP_ADP_BUFFER_SIZE)) < 0)
        {
            ext_fs_close(rhandle);
            ext_fs_close(whandle);
            return FALSE;
        }
        remain -= read;
		if((write = ext_fs_write(whandle, g_pi_map_cntx.file_buffer, read)) < 0)
		{
            ext_fs_close(rhandle);
            ext_fs_close(whandle);
            return FALSE;
        }
    }

    ext_fs_close(rhandle);
    ext_fs_close(whandle);
    return TRUE;
}

U64 btmtk_map_util_str2ull(char *str)
{
    U64 n = 0;
    while (*str != '\0')
    {
        n *= 10 ;
        n += *str - '0';
        str++;
    }
    return n;
}
U64 btmtk_map_util_str2xll(char *str)
{
    U64 n = 0;
	U8 c;
    while (*str != '\0')
    {
		c = *str;
		n *= 16 ;
		if (c >= 'a' && c <= 'f') 
		{
			n += 0xa + c - 'a';
		} 
		else if (c >= 'A' && c <= 'F')
		{
			n += 0xa + c - 'A';
		}
		else 
		{
			n += *str - '0';
		}
        str++;
    }
    return n;
}
BT_BOOL btmtk_map_util_bdaddr_equal(bt_addr_struct *addr1, bt_addr_struct *addr2)
{
	return addr1 && 
		addr2 && 
		(addr1->lap == addr2->lap) && 
		(addr1->lap == addr2->lap) && 
		(addr1->lap == addr2->lap);
}


