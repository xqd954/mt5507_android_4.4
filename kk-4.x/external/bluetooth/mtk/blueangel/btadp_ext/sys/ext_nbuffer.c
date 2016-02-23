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
 * bt_adp_nbuffer.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   
 *
 * Author:
 * -------
 * Daylong
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
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#define USE_WIN32_FS 1
#endif

#include "bt_types.h"
#include "bt_mmi.h"
#include "ext_fs.h"
#include "bt_nbuffer.h"
#include "osapi.h"
#include "bt_os_api.h"

#ifdef USE_WIN32_FS
#include "stdio.h"
#else
#include "stdio.h"
#endif

#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#define NBUFFER_MEMALLOC(n)			malloc(n)
#define NBUFFER_MEMFREE(p)			free(p)
#define NBUFFER_MEMSET(dest, c, n) 	       memset(dest, c, n)
#define NBUFFER_MEMCPY(dest, src, n)	memcpy(dest, src, n)
#define NBUFFER_MEMCMP(p1, n, p2, n2)	       memcmp(p1, p2, n)
#else //j:
#define NBUFFER_MEMALLOC(n)			malloc(n)
#define NBUFFER_MEMFREE(p)			free(p)
#define NBUFFER_MEMSET(dest, c, n) 	       memset(dest, c, n)
#define NBUFFER_MEMCPY(dest, src, n)	memcpy(dest, src, n)
#define NBUFFER_MEMCMP(p1, n, p2, n2)	       memcmp(p1, p2, n)
#endif

BT_BOOL ext_nbuffer_isInit(NBUFFER_MEM_POLL *nbuffer){
    // check all value in nbuffer
    U8 ucIndex;
    
    if( NULL == nbuffer || 0 == nbuffer->ucDataMemSize || TRUE != nbuffer->init ){
        return FALSE;
    }
    if( NBUFFER_NUM_OF_MEM <= nbuffer->ucStartOffset || NBUFFER_NUM_OF_MEM < nbuffer->ucOccupySize){
        return FALSE;
    }
    if( 0 == nbuffer->ucDataMemSize || 0xcc == nbuffer->ucDataMemSize){
        return FALSE;
    }
        
    nbuffer->init = FALSE;    
    for( ucIndex = 0; ucIndex< NBUFFER_NUM_OF_MEM; ucIndex++){
        if( NULL == nbuffer->ucDataMem[ucIndex] ){
            return FALSE;
        }
    }
        
    return TRUE;
}

BT_BOOL ext_nbuffer_deinit(NBUFFER_MEM_POLL *nbuffer){
    U8 ucIndex;
    if( NULL == nbuffer || TRUE != nbuffer->init ){
        return FALSE;
    }

	nbuffer->init = FALSE;

    for( ucIndex = 0; ucIndex < NBUFFER_NUM_OF_MEM; ucIndex++){
        if( NULL != nbuffer->ucDataMem[ucIndex]){
            NBUFFER_MEMFREE( (void *)nbuffer->ucDataMem[ucIndex]);
            nbuffer->ucDataMem[ucIndex] = NULL;
        }        
    }    
    return TRUE;
}

BT_BOOL ext_nbuffer_init(NBUFFER_MEM_POLL *nbuffer, U32 u4DataMemSize){
    // clean up the nbuffer memory
    U8 ucIndex;
    if( NULL == nbuffer || FALSE != nbuffer->init)
        return FALSE;
    nbuffer->ucOccupySize = 0;
    nbuffer->ucStartOffset = 0;
    nbuffer->init = TRUE;
    nbuffer->ucDataMemSize = u4DataMemSize;
    for( ucIndex = 0; ucIndex < NBUFFER_NUM_OF_MEM; ucIndex++){
        //nbuffer->ucDataMem[ucIndex] = (U8 *) construct_local_para( u4DataMemSize , TD_CTRL);; // new a
#ifdef USE_WIN32_FS
	nbuffer->ucDataMem[ucIndex] = (U8 *) bt_win_malloc( u4DataMemSize ); // new a
#else
	nbuffer->ucDataMem[ucIndex] = NULL;
#endif
        nbuffer->ucDataLength[ucIndex] = 0;
        if( NULL == nbuffer->ucDataMem[ucIndex]){
           ext_nbuffer_deinit(nbuffer);
           return FALSE;
        }
    }
#ifdef USE_WIN32_FS
    printf("nbufer init ok! size=%d", u4DataMemSize);
    return TRUE;
#else
    return FALSE;
#endif
}


BT_BOOL ext_nbuffer_isFull(NBUFFER_MEM_POLL *nbuffer){
    if (NBUFFER_NUM_OF_MEM == nbuffer->ucOccupySize)
		return TRUE;
	return FALSE;
}

BT_BOOL ext_nbuffer_isEmpty(NBUFFER_MEM_POLL *nbuffer){
	if( 0 == nbuffer->ucOccupySize )
		return TRUE;
    return FALSE;
}

/**
* @brief Clean up the data in nbuffer. If there is not any data, the clean up is still success
* @param nbuffer the nbuffer pointer
* @return TRUE success
*/
BT_BOOL ext_nbuffer_cleanup(NBUFFER_MEM_POLL *nbuffer){
    U8 ucIndex;
    if( NULL == nbuffer || TRUE != nbuffer->init ){
        return FALSE;
    }
    nbuffer->ucOccupySize = nbuffer->ucStartOffset = 0;
    for( ucIndex = 0; ucIndex < NBUFFER_NUM_OF_MEM; ucIndex++){
        nbuffer->ucDataLength[ucIndex] = 0;
    }
    return TRUE;    
}

BT_BOOL ext_nbuffer_push_data(NBUFFER_MEM_POLL *nbuffer, void *pData, S32 i4Length){
    U8 ucEmptyIndex;
    if( NULL == nbuffer || FALSE != ext_nbuffer_isFull(nbuffer) || (U32) i4Length > nbuffer->ucDataMemSize ){
        return FALSE;
    }
    ucEmptyIndex = (nbuffer->ucStartOffset + nbuffer->ucOccupySize) % NBUFFER_NUM_OF_MEM;
    NBUFFER_MEMCPY( nbuffer->ucDataMem[ucEmptyIndex], pData, i4Length);
    nbuffer->ucDataLength[ucEmptyIndex] = i4Length;
    nbuffer->ucOccupySize++;
#ifdef USE_WIN32_FS    
    printf("nbuffer_push_data index=%d size=%d\n", ucEmptyIndex, i4Length);
#endif            
    return TRUE;
}

BT_BOOL ext_nbuffer_pop_data(NBUFFER_MEM_POLL *nbuffer, void *pData, S32 i4Length, S32 *pi4Read){
    U8 ucNewIndex;
    if( NULL == nbuffer || FALSE != ext_nbuffer_isEmpty(nbuffer) || (U32) i4Length < nbuffer->ucDataMemSize  ){
        return FALSE;
    }
    ucNewIndex = (nbuffer->ucStartOffset+1)%NBUFFER_NUM_OF_MEM;
    NBUFFER_MEMCPY( pData, nbuffer->ucDataMem[nbuffer->ucStartOffset], nbuffer->ucDataLength[nbuffer->ucStartOffset]);
    *pi4Read = nbuffer->ucDataLength[nbuffer->ucStartOffset];
#ifdef USE_WIN32_FS    
    printf("nbuffer_pop_data index=%d size=%d\n", nbuffer->ucStartOffset, *pi4Read);
#endif            
    nbuffer->ucStartOffset = ucNewIndex;
    nbuffer->ucOccupySize--;

    return TRUE;
}

BT_BOOL ext_nbuffer_pop_data_to_file(NBUFFER_MEM_POLL *nbuffer, U32 fh, U32 *fs_status, U32 *pi4Written){
    U8 ucNewIndex;
    if( NULL == nbuffer || FALSE != ext_nbuffer_isEmpty(nbuffer) || FHANDLE_INVALID_VALUE == fh){
        return FALSE;
    }
    *fs_status = *pi4Written = 0;
    ucNewIndex = (nbuffer->ucStartOffset+1)%NBUFFER_NUM_OF_MEM;
    *fs_status = (U32) ext_fs_write_ucs2( (FHANDLE) fh, nbuffer->ucDataMem[nbuffer->ucStartOffset], (S32)nbuffer->ucDataLength[nbuffer->ucStartOffset], (S32 *)pi4Written);
    if( nbuffer->ucDataLength[nbuffer->ucStartOffset] != *pi4Written && (*fs_status == EXT_FS_OK) ){
        *fs_status = EXT_FS_ERR;
    }
#ifdef USE_WIN32_FS    
    printf("nbuffer_pop_data_to_file index=%d size=%d fs_status=%d\n", nbuffer->ucStartOffset, *pi4Written, *fs_status);
#endif            
    nbuffer->ucStartOffset = ucNewIndex;
    nbuffer->ucOccupySize--;

    return TRUE;
}

U8 *ext_nbuffer_get_tail_buffer(NBUFFER_MEM_POLL *nbuffer, U32 *pu4Index, S32 *pi4BuffSize){
    U8 ucEmptyIndex;
    
    if( NULL == nbuffer || FALSE != ext_nbuffer_isFull(nbuffer) ){
        return FALSE;
    }
    ucEmptyIndex = (nbuffer->ucStartOffset + nbuffer->ucOccupySize) % NBUFFER_NUM_OF_MEM;
    *pu4Index = ucEmptyIndex; 
    *pi4BuffSize = nbuffer->ucDataMemSize;
    
#ifdef USE_WIN32_FS    
    printf("nbuffer_get_tail_buffer index=%d\n", *pu4Index );
#endif    
    return (nbuffer->ucDataMem[ucEmptyIndex]);
}

BT_BOOL ext_nbuffer_return_tail_buffer(NBUFFER_MEM_POLL *nbuffer, U32 u4Index, U32 datasize){
    U8 ucEmptyIndex;
    ucEmptyIndex = (nbuffer->ucStartOffset + nbuffer->ucOccupySize) % NBUFFER_NUM_OF_MEM;
    if( ucEmptyIndex != u4Index ){
        return FALSE;
    }
    nbuffer->ucDataLength[ucEmptyIndex] = datasize;
    nbuffer->ucOccupySize++;

#ifdef USE_WIN32_FS    
    printf("nbuffer_return_tail_buffer index=%d datasize=%d\n", u4Index , nbuffer->ucDataLength[ucEmptyIndex]);
#endif        
    return TRUE;
}
