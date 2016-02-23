/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * $RCSfile: push_play.c,v $
 * $Revision: #2 $
 *---------------------------------------------------------------------------*/
/** @file MMPlay.c
 *  This file contains sample codes for file-based MultiMedia Playback.
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtmplayer.h"
#include "mtimage.h"
#include "drv_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "jpeglib.h"
#include <pthread.h>
#include "jpgdec.h" 
#include <errno.h>

#ifdef ANDROID_LIBJPEG_RZ_IOMMU
#include "mtgfx.h"
#endif

#ifdef CC_SUPPORT_MTK_ANDROID_RM
#include "MtkResourceManager.h"
#include "include/RMClientPriority.h"
#endif


//#define JPEG_MTAL_DEBUG_OFF
#ifdef JPEG_MTAL_DEBUG_OFF
#define printf
#endif

//#define JPEG_DISPLAY_WIDTH          1920
//#define JPEG_DISPLAY_HEIGHT         1080
//#define BUF_SIZE                           0x1000//(4k)
//#define CHECK_JPGRAW
//-----------------------------------------------------------------------------
// Static variable
//-----------------------------------------------------------------------------

//static MTMPLAYER_IMGROTATE_T eRotation = MT_MPLAYER_IMGROTATE_0;
static volatile BOOL fgMmplayEOF = FALSE;
static volatile BOOL fgImgRetErr=FALSE;
//static pthread_mutex_t JpgDecMutex ;

#ifdef ANDROID_LIBJPEG_RZ_IOMMU
MTIOMMU_PIN_RANGE_T pin_range;
MTIOMMU_DMA_RANGE_T dma_range;
#endif

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static UINT8 _u1ImgPlayStatus = 0;
static VOID _CallBack(
    MTIMAGE_CALLBACK_TYPE_T eCallbackType,
    UINT32 u4ErrorType,UINT32 u4Param)
{
    UINT8 u4msg = 0;
    switch (eCallbackType)
    {
    case MT_IMAGE_CALLBACK_ERROR:      // notification of play error
        u4msg = 2;
        fgImgRetErr = TRUE;
        SkJpegDebugf("***ERROR.\n");
        break;
    case MT_IMAGE_CALLBACK_FINISH:
        u4msg = 1;
        SkJpegDebugf("***_CallBack EOF.\n");
        break;
    case MT_IMAGE_CALLBACK_NEEDDATA:
        SkJpegDebugf("***Need data\n");
        return;
    case MT_IMAGE_CALLBACK_STOPDONE:
        u4msg = 3;
        SkJpegDebugf("***STOP DONE.\n");
        break;
    case MT_IMAGE_CALLBACK_ALREADYSTOPED:
        u4msg = 4;
        SkJpegDebugf("***ALREADY STOPEED.\n");
        break;
    default:
        _u1ImgPlayStatus = (UINT8)5;
        SkJpegDebugf("***callback error\n");
        return;
    }
    _u1ImgPlayStatus = (UINT8)u4msg;
}

j_decompress_ptr cinfoex = NULL;
UINT32 u4FileSize = 0;
#if 0
UINT32 jpeg_read_ex(UINT32 u4Addr,UINT32 u4Len)
{
    j_decompress_ptr cinfo = cinfoex;
    cinfo->src->bytes_in_buffer = 0;
    UINT32 i,readtime =   u4Len/1024 ;
    UINT32 u4TempAddr = u4Addr;
    UINT32 retLen = 0;
    cinfo->src->bytes_in_buffer = 0;
    //SkJpegDebugf("************jpeg_read_ex************\n");
    for (i = 0; i < readtime;i++)
    {
        jpeg_read(cinfo);
        retLen += cinfo->src->bytes_in_buffer;
        memcpy((void*)u4TempAddr,cinfo->src->next_input_byte,cinfo->src->bytes_in_buffer);
        u4TempAddr+=cinfo->src->bytes_in_buffer;
        if (cinfo->src->bytes_in_buffer == 1024)
        {
            cinfo->src->bytes_in_buffer = 0;
        }
        else
        {
            break;
        }
    }
    return retLen;
}
#else
static BOOL fgStop = FALSE;
static BOOL _bWaitingStop = FALSE;
UINT32 jpeg_read_ex(UINT32 u4Addr,UINT32 u4Len)
{
    j_decompress_ptr cinfo = cinfoex;
    cinfo->fghwinforead = 1;
    cinfo->src->bytes_in_buffer = u4Len;
    cinfo->src->next_input_byte = (const JOCTET*)u4Addr;
    UINT32 retLen = 0;
    SkJpegDebugf("************jpeg_read_ex************ cinfoex = 0x%08x[%x][%x]\n",cinfoex, u4Addr, u4Len);
    if (fgStop)
    {
        SkJpegDebugf("stop return\n");
        retLen = 0;
    }
    else
    {
        jpeg_read(cinfo);
        retLen = cinfo->src->bytes_in_buffer;
        SkJpegDebugf("************jpeg_read_ex************ cinfo->src->bytes_in_buffer = %x\n",cinfo->src->bytes_in_buffer);        
        SkJpegDebugf("************jpeg_read_ex************ cinfo->src->next_input_byte = %x\n",cinfo->src->next_input_byte);
    }
    cinfo->fghwinforead = 0;
    return retLen;
}

#endif

UINT32 jpeg_get_size(void)
{
    return u4FileSize;
}
static BOOL fgInited = FALSE;
#ifdef MTM_HWDEC
extern pthread_cond_t JpgStopCond ;
extern pthread_mutex_t JpgStopMutex ;
#endif
#ifdef MTM_HWDEC
boolean fgJpegIsFree(void)
{
    MTIMAGE_STATUS_T rStatus;
    if (!fgInited)
    {
        MTAL_Init();
        MTVDO_Init();
        fgInited =TRUE;
    }
    if (MTIMAGE_GetInfo(HW_JPG_DEC, NULL_MTIMAGE_HANDLE,&rStatus) == MTR_OK)
    {
        return TRUE;
    }
    return FALSE;
}
#endif
void stop_jpeg_dec(void)
{
#ifdef MTM_HWDEC
    MT_RESULT_T eImgRet = MTR_OK;
    j_decompress_ptr cinfo = NULL;
    usleep(500);
    cinfo = cinfoex;
    if (cinfo!= NULL)
    {
        if((!fgJpegIsFree()) && (!fgStop))
        {
            fgStop = TRUE;
            MTIMAGE_SetStopFlag(HW_JPG_DEC, fgStop);
            MTIMAGE_Stop(HW_JPG_DEC, NULL_MTIMAGE_HANDLE);
            SkJpegDebugf("***wait stop jpeg finish...cinfoex 0x%08x\n",cinfoex);
            
            pthread_mutex_lock(&JpgStopMutex);
            _bWaitingStop = TRUE;
            pthread_cond_wait(&JpgStopCond, &JpgStopMutex);     //signal when stop finish
            _bWaitingStop = FALSE;
            fgStop = FALSE;         
            MTIMAGE_SetStopFlag(HW_JPG_DEC,fgStop);
            pthread_mutex_unlock(&JpgStopMutex);
        }
        SkJpegDebugf("***stop finish..\n");
    }
#endif
    return ;
}

#ifdef MTIMAGE_READ_HEADER
void set_cinfo_imageRes(UINT32 u4Width,UINT32 u4Height)
{
    j_decompress_ptr cinfo = cinfoex;
    cinfo->image_width = (JDIMENSION)u4Width;
    cinfo->image_height = (JDIMENSION)u4Height;
}
#endif
#ifdef MTM_HWDEC

#ifdef CC_SUPPORT_MTK_ANDROID_RM

static IResourceComponent* HwJpgComp = NULL;

static void RmJpgClose()
{
    // release component & resource when exit.
    if (HwJpgComp)
    {
        HwJpgComp->ReleaseAllResource(HwJpgComp);
        RMDestoryComponent(HwJpgComp);
        HwJpgComp = NULL;
    }
}

BOOL RmJpgRelease(void* ctx, void* pv_data)
{
    SkJpegDebugf("RmJpgRelease is called 111.\r\n");
    stop_jpeg_dec();
    SkJpegDebugf("RmJpgRelease is called 222.\r\n");
    if (HwJpgComp)
    {
        HwJpgComp->ReleaseAllResource(HwJpgComp);
    }
    SkJpegDebugf("RmJpgRelease is called 333.\r\n");
    return TRUE;
}

static BOOL RmJpgOpen()
{
    if(HwJpgComp != NULL)
    {
        SkJpegDebugf("component != NULL, already alloc before \r\n");
        return TRUE;
    }
        
    Resource res;
    int         ret;
    res.e_Type = RESOURCE_TYPE_NUMBERNIC;
    res.resourceName = HWJPEGDECODER;
    res.u.NumbernicType.value = 0;
    // check resource either regist. if resource not regist, return error.
    // create component;
    ret = RMCreateComponent("JpegDecoder", RmJpgRelease, (void*)&HwJpgComp, &HwJpgComp );
    if (ret != 0)
    {
        SkJpegDebugf("RMCreateComponent fail, ret=%d.\n", ret);
        return FALSE;
    }

    ret = RMSetComponentPriority(HwJpgComp, RMClientPriority_Photo);
    if (ret != 0)
    {
        SkJpegDebugf("RMSetComponentPriority fail, ret=%d.\n", ret);
        return FALSE;
    }
    
    // check resource either regist. if resource not regist, return error.
    if (RMCheckResource(HWJPEGDECODER))
    {
        if (HwJpgComp->AddResource(HwJpgComp, &res))
        {
            if (HwJpgComp->AllocResource(HwJpgComp))
            {
                return TRUE;   
            }
        }
    }
    
    RmJpgClose();   ////alloc resource failed, free all alloced resource by myself.
    
    return FALSE;
}

#endif

boolean mtimage_jpg_dec_close(void)
{
    MTIMAGE_Close(HW_JPG_DEC, NULL_MTIMAGE_HANDLE);
    SkJpegDebugf("***mtimage_jpg_dec_close...fgStop %d\n",fgStop);
    //SkJpegDebugf("Getpid %d GetTid %d\n",getpid(),gettid());
    cinfoex = NULL;
    if((fgStop) || (_u1ImgPlayStatus == 3) || (_u1ImgPlayStatus == 4))
    {
        SkJpegDebugf("***mtimage_jpg_dec_close stop lock\n");
        pthread_mutex_lock(&JpgStopMutex);
        while(!_bWaitingStop)
        {
            SkJpegDebugf("***mtimage_jpg_dec_close while loop.....\n");
            pthread_mutex_unlock(&JpgStopMutex);
            usleep(200);            
            pthread_mutex_lock(&JpgStopMutex);
        }
        pthread_cond_signal(&JpgStopCond);      
        pthread_mutex_unlock(&JpgStopMutex);
        SkJpegDebugf("***mtimage_jpg_dec_close stop unlock\n");
    }

    MTIMAGE_Connect(HW_JPG_DEC, FALSE, MT_IMAGE_COLOR_FORMAT_ARGB8888);
#ifdef ANDROID_LIBJPEG_RZ_IOMMU
    if(pin_range.start)
    {
        free(pin_range.start);
        pin_range.start = NULL;
    }
#endif

#ifdef CC_SUPPORT_MTK_ANDROID_RM
    RmJpgClose();
#endif

    return TRUE;
}

BOOL getMtal_Init_Flag()
{
    return fgInited;
}

void setMtal_Init_Flag(BOOL mflag)
{
    fgInited = mflag;
}
BOOL mtmplay_jpg_dec(j_decompress_ptr cinfo)
{
    MTIMAGE_HANDLE hMtimgHandle = NULL;
    MTIMAGE_SETTING_T rMtimgSetting;
    MTIMAGE_STATUS_T rMtimgStatus;
    UINT32 i, u4Cnt = 0;
    BOOL    fgFileEnd =0;
    MT_RESULT_T eImgRet = MTR_OK;
    MTIMAGE_HANDLE pImageItem = NULL;
    UINT32 u4TgtWidth,u4TgtHeight,u4TgtAddr;
    UINT8 *pTgtAddr;
	char *msg ;
	unsigned long mmuTable = 0;
    struct jpeg_source_mgr * datasrc = cinfo->src;

    if (!fgInited)
    {
        if (!DRV_Init())
        {
            printf("Driver init fail\n");
            return 0;
        }

        MTAL_Init();
        MTVDO_Init();
        fgInited =TRUE;
    }

#ifdef CC_SUPPORT_MTK_ANDROID_RM
    if (!RmJpgOpen())
    {
        SkJpegDebugf("RmJpgOpen return fail.\n");
        return 0;
    }
#endif
    
    SkJpegDebugf("***MTM_JPG_DEC 4.2 cinfo = 0x%08x \n",cinfo);
    //SkJpegDebugf("Cinfo src buf addr= 0x%x: 0x%x,0x%x, 0x%x,0x%x \n", datasrc->next_input_byte, *datasrc->next_input_byte, *(datasrc->next_input_byte + 1), *(datasrc->next_input_byte + 2),*(datasrc->next_input_byte + 3));
    printf("Cinfo out_color_space= %d \n", (J_COLOR_SPACE)cinfo->out_color_space);

    memset(&rMtimgSetting, 0, sizeof(MTIMAGE_SETTING_T));
    memset(&rMtimgStatus, 0, sizeof(MTIMAGE_STATUS_T));

    fgImgRetErr = 0;

    switch (cinfo->out_color_space)
    {
    case JCS_RGB:
    case JCS_RGBA_8888:
        rMtimgSetting.eColorFormat = MT_IMAGE_COLOR_FORMAT_ARGB8888; //GPU_PIXEL_FORMAT_ARGB
        SkJpegDebugf("***mtmplay_jpg_dec JCS_RGBA_8888\n");
        break;
    case JCS_RGB_565:
        rMtimgSetting.eColorFormat = MT_IMAGE_COLOR_FORMAT_RGB565;//GPU_PIXEL_FORMAT_RGB16
        SkJpegDebugf("***mtmplay_jpg_dec JCS_RGB_565\n");
        break;
    default:
        printf("Strange format for decoder\n");
        return 0;
    }
#ifndef MTIMAGE_READ_HEADER
    jpeg_restart_sync(cinfo);
    u4TgtWidth= (cinfo->output_width+0xf)&~0xf;
    u4TgtHeight= cinfo->output_height;
#endif

#ifndef JPGMEM_IMPROVE
    u4TgtAddr= cinfo->input_iMCU_row;//input buffer address
#endif

    cinfoex = cinfo;
    fgStop = FALSE;
    MTIMAGE_SetStopFlag(HW_JPG_DEC, fgStop);
    if (MTIMAGE_Connect(HW_JPG_DEC, TRUE, rMtimgSetting.eColorFormat) != 0)
    {
        SkJpegDebugf("***mtmplay_jpg_dec MTIMAGE_Connect fail\n");
        cinfoex = NULL;
        return 0;
    }

    SkJpegDebugf("***mtmplay_jpg_dec MTIMAGE_DataFromBuffer\n");
    eImgRet = MTIMAGE_DataFromBuffer(HW_JPG_DEC, &pImageItem, NULL_MTIMAGE_HANDLE, datasrc->bytes_in_buffer, (UINT32)0);

    if (eImgRet != MTR_OK)
    {
        SkJpegDebugf("***MM Start play from buffer 1[%x]\n", datasrc->next_input_byte);
        mtimage_jpg_dec_close();
        cinfoex = NULL;
        return 0;
    }

    MTIMAGE_RegCb(HW_JPG_DEC, pImageItem, _CallBack,0);
    MTIMAGE_RegReadFuc(HW_JPG_DEC, pImageItem, jpeg_read_ex,set_cinfo_imageRes);
    //play
    _u1ImgPlayStatus = 0;
#ifndef MTIMAGE_READ_HEADER
    MTIMAGE_SetDecResolution(HW_JPG_DEC, NULL, u4TgtWidth, u4TgtHeight, 0);
#else
    SkJpegDebugf("***Set DecRation %d\n",cinfo->scale_denom);

    MTIMAGE_SetDecRatio(HW_JPG_DEC, NULL, (UINT16)cinfo->scale_denom);
#endif
#ifndef JPGMEM_IMPROVE
    MTIMAGE_SetDecAddr(HW_JPG_DEC, NULL, (cinfo->input_iMCU_row));
#endif

#ifdef ANDROID_LIBJPEG_RZ_IOMMU
    //u4TgtAddr = memalign(16, ((rImageUsrInfo->u4DecWidth+0xf)&(~0xf)) * rImageUsrInfo->u4DecHeight * 4 * sizeof(UINT8));   // *4 for ARGB8888 buffer
    u4TgtAddr = memalign(16, 1920* 1088 * 4 * sizeof(UINT8));   // *4 for ARGB8888 buffer
    
    if(u4TgtAddr==NULL)
    {
        SkJpegDebugf("***u4TgtAddr malloc fail fail-----\n");
    }
   // pTgtAddr = (UINT8 *)u4TgtAddr;



   MTIMAGE_SetDecAddr(HW_JPG_DEC, NULL, u4TgtAddr);    

    SkJpegDebugf("ANDROID_LIBJPEG_RZ_IOMMU Enable* jinsong modify,target address is 0x%x,mmu table addr is 0x%x...\n**********",u4TgtAddr,mmuTable);
    
    pin_range.size = 1920* 1088 * 4 * sizeof(UINT8);
    pin_range.start = (UINT32)u4TgtAddr;

	//to touch target memory, make sure it is avaliable
    u4Cnt = (1920* 1088 * 4)/4096;
    pTgtAddr = (UINT8 *)u4TgtAddr;
 
    for(i = 0; i < u4Cnt; i++)
    {
        *(pTgtAddr + i*4096) = 0xff;
    }
//	 for(i=0; i<(1920* 1080 * 4); i=i+1)		
   // for(i=0; i<(1920* 1080 * 4); i=i+1)
  //  {
  //      *(pTgtAddr+i) =0x0;
  //  }
  //  memset((void *)u4TgtAddr,0xff,1920*1080*4);
	 
	// SkJpegDebugf("mlock return 0x%x......\n",mlock(pin_range.start, pin_range.size));
	// memset((void *)u4TgtAddr,0xff,1920*1080*4);

	if(mlock(pin_range.start, pin_range.size) != 0)
	{
		 SkJpegDebugf("mlock() errno = %d==\n",errno);
		 msg = strerror(errno);
		 SkJpegDebugf("error msg: %s\n",msg);
		 mtimage_jpg_dec_close();
		 cinfoex = NULL;		 
		 return FALSE;		 
	}
	else
	{
		SkJpegDebugf("mlock() success.\n");	   		
	}

	if(MTMMU_Pin_UserMem(&pin_range) != MTR_OK)
	{
		SkJpegDebugf("***MTMMU_Pin_UserMem Fail!\n");
		munlock(pin_range.start, pin_range.size);
        mtimage_jpg_dec_close();
        cinfoex = NULL;		
		return FALSE;
	}	   
	else
	{
		SkJpegDebugf("MTMMU_Pin_UserMem success.\n");
	}
	   
#endif

    SkJpegDebugf("***MTIMAGE_Start-----\n");
    if (MTIMAGE_Start(HW_JPG_DEC, pImageItem) != 0)
    {
        SkJpegDebugf("***MM Start play failed\n");
#ifdef ANDROID_LIBJPEG_RZ_IOMMU
        MTMMU_UnPin_UserMem(&pin_range);
		munlock(pin_range.start, pin_range.size);
#endif
        //MTIMAGE_Stop(pImageItem);
        mtimage_jpg_dec_close();
        cinfoex = NULL;
        return 0;
    }
    // wait decode done
    MTIMAGE_Filldata_Libjpeg(HW_JPG_DEC);
	SkJpegDebugf("***MTIMAGE_Filldata_Libjpeg done.\n");

    if (_u1ImgPlayStatus != 1)
    {
        SkJpegDebugf("***MM Emp play status %d, return\n", _u1ImgPlayStatus);
#ifdef ANDROID_LIBJPEG_RZ_IOMMU
        MTMMU_UnPin_UserMem(&pin_range);
		munlock(pin_range.start, pin_range.size);
#endif
        mtimage_jpg_dec_close();
        cinfoex = NULL;
        return 0;
    }
    else
    {
        SkJpegDebugf("-------------MTIMAGE_decode finish-----\n");
    }

    if (!fgImgRetErr)
    {
        UINT32 u4Buf = 0, u4PhyBuf = 0,u4CbCrOffset = 0;
        MTVDO_REGION_T rRegion;
        MTIMAGE_GetImg(HW_JPG_DEC, hMtimgHandle, &u4Buf, &u4PhyBuf,&u4CbCrOffset,&rRegion, JPG_GETIMG_OSDMODE_ARGB8888,FALSE);

        cinfo->output_width = rRegion.u4Width;
        cinfo->output_height = rRegion.u4Height;

        SkJpegDebugf("***0. output_width = %d, output_height = %d, outoutput buffer=%x\n", cinfo->output_width,  cinfo->output_height, u4TgtAddr);
#ifdef JPGMEM_IMPROVE
        cinfo->input_iMCU_row =     (JDIMENSION)u4Buf ;
#endif

#ifdef ANDROID_LIBJPEG_RZ_IOMMU
        cinfo->input_iMCU_row =     (JDIMENSION)u4TgtAddr ;
#endif
    }

#ifdef ANDROID_LIBJPEG_RZ_IOMMU
    MTMMU_UnPin_UserMem(&pin_range);
	munlock(pin_range.start, pin_range.size);
#endif
    
    if (fgImgRetErr || !cinfo->input_iMCU_row)
    {
        SkJpegDebugf("***->MTM_JPG_DEC-FAILE\n");
        mtimage_jpg_dec_close();
        fgImgRetErr =0;
        cinfoex = NULL;
        return 0;
    }
    else
    {
        SkJpegDebugf("***->MTM_JPG_DEC-DONE\n");
        cinfoex = NULL;
        return 1;
    }
}

#endif

