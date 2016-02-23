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
 * bt_ucs2.c
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
#include "bt_mmi.h"
#include "bt_ucs2.h"

/**
* @brief Concate a ucs2 string
* @param size Characters, Not in bytes
* @return
*/
S8 *bt_ucs2ncat(S8 *strDestination, const S8 *strSource, U32 size){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    S8 *dest = strDestination;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    dest = dest + bt_ucs2strlen(strDestination) * ENCODING_LENGTH;

    bt_ucs2ncpy(dest, strSource, ( size - bt_ucs2strlen(strDestination) ) );
    return strDestination;
}

/**
* @brief Copy ucs2 string
* @param size Characters, Not in bytes
* @return
*/
S8 *bt_ucs2ncpy(S8 *strDestination, const S8 *strSource, U32 size){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 count;
    S8 *temp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    count = 0;
    temp = strDestination;
    size = size * 2;
    if (strSource == NULL)
    {
        if (strDestination)
        {
            *(strDestination + count) = '\0';
            *(strDestination + count + 1 ) = '\0';
        }
        return temp;

    }

    if (strDestination == NULL || strSource == NULL)
    {
        return NULL;
    }
    while ( !((*(strSource + count) == 0) && (*(strSource + count + 1) == 0)) )
    {
        *(strDestination + count) = *(strSource + count);
		*(strDestination + count + 1) = *(strSource + count + 1);

        count += 2;
        size -= 2;
        if(size <= 2){
            break;
        }
    }

    *(strDestination + count) = '\0';
    *(strDestination + count + 1 ) = '\0';

    return temp;
}


/**
* @brief Scan a UCS2 encoded string for the last occurrence of a character.
*/
S8 *bt_ucs2rchr(const S8 * string, U16 ch){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *cursor;
    U16 ch2;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/  
    cursor = (S8 *)string;  
    while (*cursor || *(cursor+1))              /* find end of string */
    {
        cursor += 2;
    }
    
    /* search towards front */
    while ((S8 *)string != cursor && 
           (*(cursor+1)<<8|(U8)(*cursor)) != ch)
    {
        cursor -= 2;
    }
    ch2 = (*(cursor+1)<<8|(U8)(*cursor));
    if ((*(cursor+1)<<8|(U8)(*cursor)) == ch)          /* found ?*/
    {
        return cursor;
    }
    
    return NULL;
}

/**
* @brief Count the ucs2 string's length in character, not in bytes
* @return 
*/
S32 bt_ucs2strlen(const S8 *arrOut){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/      
    U32 nCount;
    U32 nLength;
    

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Check for NULL character only at the odd no. of bytes 
       assuming forst byte start from zero */
    nCount = 0;
    nLength = 0;
    if( NULL == arrOut ){
        return 0;
    }
    // Avoid using U16*. Arm data pointer (4 byte aligment) may cause data abort exception    
    for ( nCount = 0; (0 != *arrOut || 0 != *(arrOut+1) ) ; nCount++){
        arrOut++;arrOut++;
    }
    return nCount; /* One is added to count 0th byte */
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
U16 bt_asc_str_n_to_ucs2_str(S8 *pOutBuffer, S8 *pInBuffer, U32 len){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/      
    U16 nLength;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    nLength = 0;
    if( NULL == pOutBuffer ){
        return 0;
    }

    for( nLength = 0; *pInBuffer; nLength++ ){
        *pOutBuffer = *pInBuffer;
        pOutBuffer++;
        *pOutBuffer = 0;
        pOutBuffer++;
        
        pInBuffer++;

        if( (nLength*2) >= (U16)(len -2 )){ //1 wchar is 2 bytes
            break;
        }
    }

    *pOutBuffer = 0;
    pOutBuffer++;
    *pOutBuffer = 0;
    pOutBuffer++;

    return nLength;
}

/**
* @brief Count the length of a ASCII string
* @return the Length of string in bytes
*/
U32 bt_strlen(const S8 *string){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    U32 u4Length;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    for(u4Length = 0; *string; u4Length++){
        string++;
    }
    return u4Length;
}

/**
* @brief copy a ascii string
* @return the Length of final strDest
*/
U32 bt_strncpy(S8 *strDest, const S8 *strSrc, U16 u2MaxLen){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 nLength, nAppendLen;
       
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/         
    nLength = 0;
    nAppendLen = 0;
    if( NULL == strDest ){
        return 0;
    }

    for( nAppendLen = 0; *strSrc && nAppendLen < (U32)(u2MaxLen-1); nAppendLen++){
        *strDest = *strSrc;
        *strDest++;
        *strSrc++;
        nLength++;
    }
    
    *strDest = 0;
    
    return nLength;
}

/**
* @brief Cat a string 
* @return the Length of final strDest
*/
U32 bt_strncat(S8 *strDest, S8 *strSrc, U16 u2MaxLen){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    U32 nLength, nAppendLen;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    nLength = 0;
    nAppendLen = 0;
    if( NULL == strDest ){
        return 0;
    }

    for( nLength = 0; *strDest; nLength++ ){
        strDest++;
    }

    for( nAppendLen = 0; *strSrc && nAppendLen < (U32)(u2MaxLen-1); nAppendLen++){
        *strDest = *strSrc;
        *strDest++;
        *strSrc++;
        nLength++;
    }

    *strDest = 0;
    
    return nLength;
}


/**
* @brief Compare two ascii string
*
* @return 0 is the same 
*/
S32 bt_strncmp(const S8 *strDest, const S8 *strSrc, U16 u2Size){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if( strDest == strSrc )
        return 0;
    if( NULL == strDest || NULL == strSrc)
        return 1;

    while ( (*strDest == *strSrc) && ( 0 != *strDest ) && (0 != *strSrc) && u2Size > 0)
    {
        strDest++;
        strSrc++;
        u2Size--;

    }   /* End of while */

    /* FIX: only compare n size */
    if( 0 == u2Size ){
    	return 0;
    }

    return (S32) (*strDest - *strSrc);
}

/*****************************************************************************
 * FUNCTION
 *  bt_ucs2swap_endian
 * DESCRIPTION
 *  Changes big endian to be little endian and vice versa
 * PARAMETERS
 *  dstBuffer       [IN]        Destination buffer
 *  srcBuffer       [IN]        Source buffer
 * RETURNS
 *  result string
 *****************************************************************************/
U8 *bt_ucs2swap_endian(U8 *dstBuffer, U8 *srcBuffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 i = 0;
    kal_uint8 len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    len = bt_ucs2strlen((kal_int8*)srcBuffer) * 2;
	
	if (len == 0)
	{
		return NULL;
	}

    while (i != len)
    {
        dstBuffer[i] = srcBuffer[i + 1];
        dstBuffer[i + 1] = srcBuffer[i];
        i += 2;
    }
    dstBuffer[i] = '\0';
    dstBuffer[i + 1] = '\0';

    return dstBuffer;
}

/**
* @brief Compare two UCS2
*
* @return 0 is the same 
*/
U32 bt_ucs2cmp(const U8 *string1, const U8 *string2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U16 nStr1;
    U16 nStr2;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while ((*string1 == *string2) && (*(string1 + 1) == *(string2 + 1)))
    {

        if ((*string1 == 0) && (*(string1 + 1) == 0))
        {
            return 0;
        }

        string1 += 2;
        string2 += 2;

    }   /* End of while */

    nStr1 = (string1[1] << 8) | (U8)string1[0];
    nStr2 = (string2[1] << 8) | (U8)string2[0];

    return (S32) (nStr1 - nStr2);
}


/**
* @brief Return the first non-space position in src
*
* @return 
*/
U8 *bt_ucs2skipspace(U8 *line){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S8 *ptr;
    S32 i = 0, len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ptr = (S8*)line;

    len = (bt_ucs2strlen(ptr)) << 1;

    while (ptr[i] == ' ' && ptr[i + 1] == 0)
    {
        ptr += 2;
        if (i >= len)
        {
            break;
        }
    }
    return (U8*)ptr;    
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
U32 bt_ucs2_str_n_to_asc_str(S8 *pOutBuffer, S8 *pInBuffer, U32 len){
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    S8 *pString1, *pStringIn;
    U32 length;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    length = 0;
    pString1 = pOutBuffer;
    pStringIn = pInBuffer;
    if( NULL == pOutBuffer || NULL == pInBuffer){
        return 0;
    }
    while( length < (len-1) && 0 != pStringIn[0]){
        *pString1 = pStringIn[0];
        pString1++;
        pStringIn++;pStringIn++; // jump a UCS2
        length++;
    }
    *pString1 = 0;
    return length;
}



/******************************** utf8 vs ucs2 ***************************/
#ifndef SET_CHR_AT
#define SET_CHR_AT(RAW, n, value)   do {                          \
                              U8 *_p = STR_AT(RAW, n);     \
                              U16 v= (U16) (value); \
                              _p[0] = (U8) (v & 0xff);     \
                              _p[1] = (U8) (v >> 8);       \
                           } while (0)
#endif
static const U8 g_cheset_utf8_bytes_per_char[16] = 
{
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 2, 2, 3, 4  /* we don't support UCS4 */
};
#define CAST_PU8(RAW)   ((U8*)(RAW))
#define CAST_CPU8(RAW)  ((const U8*)(RAW))
#define GET_U8_BYTE_TO_U16(RAW, i)  ((U16)(CAST_CPU8(RAW)[i]))
#define STR_SIZE(len) ((len)<<1)
#define STR_AT(RAW, n) ((void*)(CAST_PU8(RAW)+STR_SIZE(n)))
#define CHR_AT(RAW, n) ((U16)(GET_U8_BYTE_TO_U16(RAW, STR_SIZE(n))+(GET_U8_BYTE_TO_U16(RAW, STR_SIZE(n)+1)<<8)))

S32 bt_chset_utf8_to_ucs2_string_ex(
            U8 *dest,
            S32 dest_size,
            const U8 *src,
            U32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int pos = 0;
    int cnt;
    int src_len = bt_strlen((const char*)src);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    //MMI_CHSET_DBG_ASSERT((dest_size % 2) == 0);
    dest_size -= (dest_size % 2);
    *src_end_pos = (U32) src; /* set src_end_pos first */

    if (dest_size < 2)
    {
        //Assert(0);    /* dest wont be NULL-terminated */
        return 0;
    }

    while (*src && pos < dest_size - 2)
    {
        cnt = bt_chset_utf8_to_ucs2(dest + pos, src);
         if (((U32)src - (*src_end_pos)) >= (U32)(src_len - cnt) &&
             (*(dest + pos) == 0xFF && *(dest + pos + 1) == 0xFF) &&
            !(*src == 0xEF && *(src+1) == 0xBF && *(src+2) == 0xBF))
        {
            /* 
             * If src isn't 0xEF, 0xBF, 0xBF and its remain length is not enough but dest is 0xFFFF, we will abort the process.
             * dest data is invalid character because src data is not enough to convert 
             */
            break;
        }
        if (cnt == 0)   /* abnormal case */
        {
        #ifdef MMI_CHSET_UTF8_FAULT_TOLERANT
            src++;
        #else 
            break;
        #endif 
        }        
        else    /* normal case */
        {
            src += cnt;
            pos += 2;
        }
    }
    *src_end_pos = (U32) src;
    dest[pos] = 0;
    dest[pos + 1] = 0;
    return pos + 2;
}

S32 bt_chset_utf8_to_ucs2_string(U8 *dest, S32 dest_size,const U8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 src_end_pos = (U32) src;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if( NULL == src || NULL == dest ){
    	return 0;
    }
    return bt_chset_utf8_to_ucs2_string_ex(dest, dest_size, src, &src_end_pos);
}

/*****************************************************************************
 * FUNCTION
 *  bt_chset_utf8_to_ucs2
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [OUT]       
 *  utf8        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 bt_chset_utf8_to_ucs2(U8 *dest, const U8 *utf8)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 c = utf8[0];
    int count = g_cheset_utf8_bytes_per_char[c >> 4];
    U16 ucs2 = 0xFFFF;   /* invalid character */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (count)
    {
        case 1:
            ucs2 = (U16) c;
            break;
        case 2:
            if (utf8[1])
            {
                ucs2 = ((U16) (c & 0x1f) << 6) | (U16) (utf8[1] ^ 0x80);
            }
            break;
        case 3:
            if (utf8[1] && utf8[2])
            {
                ucs2 = ((U16) (c & 0x0f) << 12)
                    | ((U16) (utf8[1] ^ 0x80) << 6) | (U16) (utf8[2] ^ 0x80);
            }
            break;
        case 4:
            break;
        default:
            count = 1;   /* the character cannot be converted, return 1 means always consume 1 byte */
            break;
    }

    SET_CHR_AT(dest, 0, ucs2);

    return count;
}



/*****************************************************************************
 * FUNCTION
 *  bt_chset_ucs2_to_utf8_len
 * DESCRIPTION
 *  
 * PARAMETERS
 *  ucs2        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static __inline int bt_chset_ucs2_to_utf8_len(U16 ucs2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ucs2 < (U16) 0x80)
    {
        return 1;
    }
    else if (ucs2 < (U16) 0x800)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_chset_ucs2_to_utf8
 * DESCRIPTION
 *  
 * PARAMETERS
 *  utf8        [OUT]       
 *  ucs2        [IN][OUT]        
 * RETURNS
 *  
 *****************************************************************************/
S32 bt_chset_ucs2_to_utf8(U8 *utf8, U16 ucs2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int count;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ucs2 < (U16) 0x80)
    {
        count = 1;
    }
    else if (ucs2 < (U16) 0x800)
    {
        count = 2;
    }
    else
    {
        count = 3;
    }
    switch (count)
    {
        case 3:
            utf8[2] = 0x80 | (ucs2 & 0x3f);
            ucs2 = ucs2 >> 6;
            ucs2 |= 0x800;
        case 2:
            utf8[1] = 0x80 | (ucs2 & 0x3f);
            ucs2 = ucs2 >> 6;
            ucs2 |= 0xc0;
        case 1:
            utf8[0] = (U8) ucs2;
    }
    return count;
}

/*****************************************************************************
 * FUNCTION
 *  bt_chset_ucs2_to_utf8_string_ex
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
S32 bt_chset_ucs2_to_utf8_string_ex(
            U8 *dest,
            S32 dest_size,
            const U8 *src,
            U32 *src_end_pos)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int pos = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (src[0] || src[1])
    {
        if (pos + bt_chset_ucs2_to_utf8_len(CHR_AT(src, 0)) >= dest_size)      /* leave space of '\0' */
        {
            //Assert(0);
            break;
        }

        pos += bt_chset_ucs2_to_utf8(dest + pos, CHR_AT(src, 0));
        src += 2;
        if (pos >= dest_size - 1)
        {
            break;
        }
    }
    if (pos >= dest_size)
    {
        //Assert(0);
        dest[dest_size - 1] = 0;
    }
    else
    {
        dest[pos] = 0;
    }
    *src_end_pos = (U32) src;
    return pos + 1;
}


/*****************************************************************************
 * FUNCTION
 *  bt_chset_ucs2_to_utf8_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest            [OUT]       
 *  dest_size       [IN]        
 *  src             [IN]        
 * RETURNS
 *  
 *****************************************************************************/
S32 bt_chset_ucs2_to_utf8_string(U8 *dest, S32 dest_size,const U8 *src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 src_end_pos = (U32) src;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	if( NULL == src || NULL == dest ){
		return 0;
	}
    return bt_chset_ucs2_to_utf8_string_ex(dest, dest_size, src, &src_end_pos);
}

/******************************** End of utf8 vs ucs2 ***************************/
