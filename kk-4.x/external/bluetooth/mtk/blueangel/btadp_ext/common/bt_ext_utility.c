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
 * bt_ext_utility.c
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
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <ctype.h>

//#include "MBTType.h"

#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bt_ext_debug.h"

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_convert_array2bdaddrst
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  src         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_util_convert_array2bdaddrst(bt_addr_struct *dest, U8 *src)
{
    dest->nap = (((U16) src[1])) | ((U16) src[0] << 8);
    dest->uap = (U8) src[2];
    dest->lap = (((U32) src[3]) << 16) | ((U32) src[4] << 8) | ((U32) src[5]);
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_convert_bdaddrst2array
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  source         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_util_convert_bdaddrst2array(U8 *dest, bt_addr_struct *source)
{
    dest[5] = (U8) (source->lap & 0x000000FF);
    dest[4] = (U8) ((source->lap & 0x0000FF00) >> 8);
    dest[3] = (U8) ((source->lap & 0x00FF0000) >> 16);
    dest[2] = source->uap;
    dest[1] = (U8) (source->nap & 0x00FF);
    dest[0] = (U8) ((source->nap & 0xFF00) >> 8);
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_convert_mbtarray2mtkarray
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  source         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void btmtk_util_convert_mbtarray2mtkarray(U8 *dest, U8 *source)
{
    dest[5] = source[0];
    dest[4] = source[1];
    dest[3] = source[2];
    dest[2] = source[3];
    dest[1] = source[4];
    dest[0] = source[5];
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_stricmp
 * DESCRIPTION
 *  Case-insensitive comparison
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
U8 btmtk_util_stricmp(const char *str1, const char *str2)
{
    for (; tolower(*str1) == tolower(*str2); str1++, str2++)
    {
        /* Return zero on success, just like the ANSI strcmp() */
        if (*str1 == '\0')
            return 0;
    }
    return 1;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_strnicmp
 * DESCRIPTION
 *  Case-insensitive comparison
 * PARAMETERS
 *  Str1        [IN]
 *  Str2        [IN]
 * RETURNS
 *
 *****************************************************************************/
U8 btmtk_util_strnicmp(const char *str1, const char *str2, U32 size)
{
    for (; --size > 0 && tolower(*str1) == tolower(*str2); str1++, str2++)
    {
        /* Return zero on success, just like the ANSI strcmp() */
        if (*str1 == '\0')
            return 0;
    }

    /* Return zero on success, just like the ANSI strcmp() */
    if (*str1 == *str2)
    {
        return 0;
    }

    return 1;
}

void btmtk_util_convert_bdaddr2string(char *dest, bt_addr_struct *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
            (U8) ((source->nap & 0xFF00) >> 8),
            (U8) (source->nap & 0x00FF),
            source->uap,
            (U8) ((source->lap & 0x00FF0000) >> 16),
            (U8) ((source->lap & 0x0000FF00) >> 8),
            (U8) (source->lap & 0x000000FF));
}

void btmtk_util_convert_string2bdaddr(const char *source, bt_addr_struct *dest)
{
    unsigned char addr[6];
    int i;
    char *ptr = (char*)source;

    //assert(dest != NULL && source != NULL);
    for (i = 5; i >= 0; i--)
    {
        addr[i] = strtoul(ptr, &ptr, 16);
        ptr++;
    }
    dest->lap = addr[0] + (addr[1] << 8) + (addr[2] << 16);
    dest->uap = addr[3];
    dest->nap = addr[4] + (addr[5] << 8);
}


#if 0

void btmtk_util_get_addr_str(char *buf, T_MBT_BDADDR BDAddr)
{
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", BDAddr[0], BDAddr[1], BDAddr[2], BDAddr[3], BDAddr[4], BDAddr[5]);
}

void btmtk_util_get_link_key_str(char *buf, T_MBT_LINKKEY key)
{
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
        key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7], key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15]);
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_gap_translate_error
 * DESCRIPTION
 *
 * PARAMETERS
 *  reason         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
T_MBT_GAP_PAIR_ERR_REASON btmtk_util_gap_translate_pair_error(U8 reason)
{
    switch(reason)
    {
    case BTBM_ADP_FAIL_TIMEOUT:
        return MBT_GAP_PAIRERR_TIMEOUT;
    case BTBM_ADP_FAIL_KEYERR:
        return MBT_GAP_PAIRERR_KEYERR;
    case BTBM_ADP_FAIL_REMOTE_REJECT:
        return MBT_GAP_PAIRERR_REJECT;
    default:
        return MBT_GAP_PAIRERR_UNKNOWN;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_util_gap_translate_linkkey_type
 * DESCRIPTION
 *
 * PARAMETERS
 *  reason         [IN]
 * RETURNS
 *  void
 *****************************************************************************/

T_MBT_GAP_LINKKEY_TYPE btmtk_util_gap_translate_linkkey_type(btbm_link_key_type type)
{
    switch(type)
    {
    case BTBM_LINK_KEY_TYPE_COMBINATION:
        return MBT_GAP_LINKKEY_COMBINATION;
    case BTBM_LINK_KEY_TYPE_LOCAL_UNIT:
        return MBT_GAP_LINKKEY_LOCALUNIT;
    case BTBM_LINK_KEY_TYPE_REMOTE_UNIT:
        return MBT_GAP_LINKKEY_REMOTEUNIT;
    case BTBM_LINK_KEY_TYPE_DEBUG_COMBINATION:
        return MBT_GAP_LINKKEY_DEBUG;
    case BTBM_LINK_KEY_TYPE_UNAUTHENTICATED:
        return MBT_GAP_LINKKEY_UNAUTHENTICATED;
    case BTBM_LINK_KEY_TYPE_AUTHENTICATED:
        return MBT_GAP_LINKKEY_AUTHENTICATED;
    case BTBM_LINK_KEY_TYPE_CHANGED_COMBINATION:
        return MBT_GAP_LINKKEY_CHANGED;
    default:
        //ASSERT(0);
		return MBT_GAP_LINKKEY_COMBINATION;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_util_gap_lookup_search_list
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  source         [IN]
 * RETURNS
 *  index: (-1 not found)
 *****************************************************************************/

S32 btmtk_util_gap_lookup_search_list(T_MBT_SEARCHED_DEV_LIST *list, T_MBT_BDADDR addr)
{
    S32 i;
    for (i = 0; i < list->SearchCount; i++)
    {
        if (memcmp(&list->SearchList[i].BdAddr, addr, sizeof(T_MBT_BDADDR)) == 0)
        {
            return i;
        }
    }
    return -1;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_gap_lookup_paired_list
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  source         [IN]
 * RETURNS
 *  index: (-1 not found)
 *****************************************************************************/
S32 btmtk_util_gap_lookup_paired_list(T_MBT_PAIRED_DEV_LIST *list, T_MBT_BDADDR addr)
{
    S32 i;
    for (i = 0; i < list->PairedCount; i++)
    {
        if (memcmp(list->PairedList[i].BdAddr, addr, sizeof(T_MBT_BDADDR)) == 0)
        {
            return i;
        }
    }
    return -1;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_gap_lookup_service_list
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  source         [IN]
 * RETURNS
 *  index: (-1 not found)
 *****************************************************************************/
S32 btmtk_util_gap_lookup_service_list(T_MBT_SERVICE_LIST *list, MBT_SERVICE_ID uuid)
{
    S32 i;
    for (i = 0; i < list->nServiceCount; i++)
    {
        if (list->Service[i] == uuid)
        {
            return i;
        }
    }
    return -1;
}

/*****************************************************************************
 * FUNCTION
 *  btmtk_util_gap_lookup_blocked_list
 * DESCRIPTION
 *
 * PARAMETERS
 *  dest        [OUT]
 *  source         [IN]
 * RETURNS
 *  index: (-1 not found)
 *****************************************************************************/
int btmtk_util_gap_lookup_blocked_list(T_MBT_BLOCKED_DEV_LIST *list, T_MBT_BDADDR BdAddr)
{
    S32 i;
    for (i = 0; i < list->BlockedCount; i++)
    {
        if (memcmp(list->BlockDevList[i], BdAddr, sizeof(T_MBT_BDADDR)) == 0)
        {
            return i;
        }
    }
    return -1;
}
#endif
