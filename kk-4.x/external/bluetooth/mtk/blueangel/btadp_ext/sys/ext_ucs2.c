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
 * ext_ucs2.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   UCS2 manipulation
 *
 * Author:
 * -------
 *  Daylong Chen
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "bt_types.h"
#include "bt_ucs2.h"

/**
* @brief Concate a ucs2 string
* @param size Characters, Not in bytes
* @return
*/
S8 *ext_ucs2ncat(S8 *strDestination, const S8 *strSource, U32 size){
    return bt_ucs2ncat(strDestination, strSource, size);
}

/**
* @brief Copy ucs2 string
* @param size Characters, Not in bytes
* @return
*/
S8 *ext_ucs2ncpy(S8 *strDestination, const S8 *strSource, U32 size){
	return bt_ucs2ncpy(strDestination, strSource, size);
}


/**
* @brief Scan a UCS2 encoded string for the last occurrence of a character.
*/
S8 *ext_ucs2rchr(const S8 * string, U16 ch){
	return bt_ucs2rchr(string, ch);
}

/**
* @brief Count the ucs2 string's length in character, not in bytes
* @return 
*/
S32 ext_ucs2strlen(const S8 *arrOut){
	return bt_ucs2strlen(arrOut);
}

/**
* @brief convert ASCII string to UCS2 string
*  The function is used for convert the characters of ANSII string to UCS2
*  string. The caller need to make sure the pOutBuffer size is greater than 
*  len and the function does add the terminated character at the end of 
*  the pOutBuffer array. The byte order of UCS2 character(output param) is
*  little endian.
* @param len the max length of pOutBuffer in Bytes
* @return The UCS2 Length in bytes
*/
U16 ext_asc_str_n_to_ucs2_str(S8 *pOutBuffer, S8 *pInBuffer, U32 len){
	return bt_asc_str_n_to_ucs2_str(pOutBuffer, pInBuffer, len);
}

/**
* @brief Count the length of a ASCII string
* @return the Length of string in bytes
*/
U32 ext_strlen(const S8 *string){
	return bt_strlen(string);
}

/**
* @brief copy a ascii string
* @return the Length of final strDest
*/
U32 ext_strncpy(S8 *strDest, const S8 *strSrc, U16 u2MaxLen){
	return bt_strncpy(strDest, strSrc, u2MaxLen);
}

/**
* @brief Cat a string 
* @return the Length of final strDest
*/
U32 ext_strncat(S8 *strDest, S8 *strSrc, U16 u2MaxLen){
	return bt_strncat(strDest, strSrc, u2MaxLen);
}


/**
* @brief Compare two ascii string
*
* @return 0 is the same 
*/
S32 ext_strncmp(const S8 *strDest, const S8 *strSrc, U16 u2Size){
	return bt_strncmp(strDest, strSrc, u2Size);
}

/*****************************************************************************
 * FUNCTION
 *  ext_ucs2swap_endian
 * DESCRIPTION
 *  Changes big endian to be little endian and vice versa
 * PARAMETERS
 *  dstBuffer       [IN]        Destination buffer
 *  srcBuffer       [IN]        Source buffer
 * RETURNS
 *  result string
 *****************************************************************************/
U8 *ext_ucs2swap_endian(U8 *dstBuffer, U8 *srcBuffer){
    return bt_ucs2swap_endian(dstBuffer, srcBuffer);
}

/**
* @brief Compare two UCS2
*
* @return 0 is the same 
*/
U32 ext_ucs2cmp(const U8 *string1, const U8 *string2)
{
    return bt_ucs2cmp(string1, string2);
}


/**
* @brief Return the first non-space position in src
*
* @return 
*/
U8 *ext_ucs2skipspace(U8 *line){
    return bt_ucs2skipspace(line);
}

/**
* @brief convert UCS2 to ASCII string 
*  The function is used for convert the characters of UCS2 string to ASCII
*  string. The caller need to make sure the pOutBuffer size is greater than 
*  len and the function does add the terminated character at the end of 
*  the pOutBuffer array. The byte order of UCS2 character(input param) is
*  little endian.
* @param len the max length of pOutBuffer in Bytes
* @return The ASC Length in bytes
*/
U32 ext_ucs2_str_n_to_asc_str(S8 *pOutBuffer, S8 *pInBuffer, U32 len){
    return bt_ucs2_str_n_to_asc_str(pOutBuffer, pInBuffer, len);
}



/******************************** utf8 vs ucs2 ***************************/
S32 ext_chset_utf8_to_ucs2_string_ex(
            U8 *dest,
            S32 dest_size,
            const U8 *src,
            U32 *src_end_pos)
{
    return bt_chset_utf8_to_ucs2_string_ex(dest, dest_size, src, src_end_pos);
}

S32 ext_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size,const U8 *src)
{
    return bt_chset_utf8_to_ucs2_string(dest, dest_size, src);
}

/*****************************************************************************
 * FUNCTION
 *  ext_chset_utf8_to_ucs2
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [OUT]       
 *  utf8        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 ext_chset_utf8_to_ucs2(U8 *dest, const U8 *utf8){
    return bt_chset_utf8_to_ucs2(dest, utf8);
}

/*****************************************************************************
 * FUNCTION
 *  ext_chset_ucs2_to_utf8
 * DESCRIPTION
 *  
 * PARAMETERS
 *  utf8        [OUT]       
 *  ucs2        [IN][OUT]        
 * RETURNS
 *  
 *****************************************************************************/
S32 ext_chset_ucs2_to_utf8(U8 *utf8, U16 ucs2){
    return bt_chset_ucs2_to_utf8(utf8, ucs2);
}

/*****************************************************************************
 * FUNCTION
 *  ext_chset_ucs2_to_utf8_string_ex
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest            [OUT]       
 *  dest_size       [IN]        
 *  src             [IN]        
 *  src_end_pos     [OUT]       
 * RETURNS
 *  
 *****************************************************************************/
S32 ext_chset_ucs2_to_utf8_string_ex(
            U8 *dest,
            S32 dest_size,
            const U8 *src,
            U32 *src_end_pos)
{
    return bt_chset_ucs2_to_utf8_string_ex(dest, dest_size, src, src_end_pos);
}


/*****************************************************************************
 * FUNCTION
 *  ext_chset_ucs2_to_utf8_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest            [OUT]       
 *  dest_size       [IN]        
 *  src             [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 ext_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size,const U8 *src)
{
    return bt_chset_ucs2_to_utf8_string(dest, dest_size, src);
}

/******************************** End of utf8 vs ucs2 ***************************/