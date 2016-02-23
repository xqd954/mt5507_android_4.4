#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>
#include <time.h>
#include <include/core/SkBitmap.h>

#include <JNIHelp.h>

#include <android/log.h>
#include "mtimage.h"
#include "jpgdec.h"
#include "drv_common.h"

#include <utils/Log.h>
#include <stdio.h>
#include "jni.h"
#define  LOG_TAG    "pic_test"

#include <mtal.h>
#include "pictureapi.h"

/////add for resource manager////////
#ifdef CC_SUPPORT_MTK_ANDROID_RM
#include "MtkResourceManager.h"
#endif


#ifdef ALL_PIC_VIDEOPATH
int fg_MTPHOTO4K_init = 0;
bool fg_videopath = false;
#endif

typedef struct{
    int tex_width, tex_height;
    pthread_mutex_t mutex;
}shared_data;
MT_RESULT_T r;

unsigned char *gpbuf  = NULL;
#define Safe_Free(p) if((p)) {free((p)); (p) = NULL;}






//-----------------------jni--------------------------


#ifdef ALL_PIC_VIDEOPATH

#ifdef CC_SUPPORT_MTK_ANDROID_RM

static IResourceComponent* VdpComp = NULL;

void RmClose()
{
    printf ("resource alloced failed and exit play");

    // release component & resource when exit.
    if (VdpComp)
    {
        VdpComp->ReleaseAllResource(VdpComp);
	    RMDestoryComponent(VdpComp);
	    VdpComp = NULL;
    }
}


BOOL RmRelease(void* ctx, void* pv_data)
{
    printf("release\r\n");
	if(fg_MTPHOTO4K_init)
	{
		__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### MTPHOTO4K_DeInit !!!");
		MTPHOTO4K_DeInit();
		fg_MTPHOTO4K_init = 0;	
	}
    RmClose();
    return TRUE;
}

BOOL RmOpen()
{
    if(VdpComp != NULL)
    {
        printf("component != NULL, already alloc before \r\n");
		return TRUE;
    }
        
    Resource res;
	int			ret;
    res.e_Type = RESOURCE_TYPE_NUMBERNIC;
    res.resourceName = VIDEOPATH;
    res.u.NumbernicType.value = 0;
    // check resource either regist. if resource not regist, return error.
    // create component;
    ret = RMCreateComponent("mtk4k2kphotoplayer", RmRelease, (void*)&VdpComp, &VdpComp );
	if (ret != 0)
	{
		printf("RMCreateComponent fail, ret=%d.\n", ret);
		return FALSE;
	}
	
    // check resource either regist. if resource not regist, return error.
    if (RMCheckResource(VIDEOPATH))
    {
    	if (VdpComp->AddResource(VdpComp, &res))
	   	{
	   		if (VdpComp->AllocResource(VdpComp))
		    {
		        return TRUE;   
		    }
	    }
    }
    
    RmClose();   ////alloc resource failed, free all alloced resource by myself.
    
    return FALSE;
}

#endif


//char* buf = NULL;

bool get_fgVideoPath()
{
	return fg_videopath;
}



#endif




static jboolean nativeShowPhotoOnVdp(JNIEnv * env, jobject obj,jint   sktype,  jint  skdata, jint  skdatasize, jint  u4x, jint u4y
	, jint  u4Width, jint  u4Height)
{
	#ifdef ALL_PIC_VIDEOPATH

	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### do_MTPHOTO4K_Show ...start...! %ld",(clock()/1000));

	MTPHOTO4K_DISPLAY_INFO_T pic_info;
	if(fg_videopath)
	{

		SkBitmap* bitmap = (SkBitmap*)skdata;
		
		LOGE(".....native EEEEEE do_MTPHOTO4K_Show  ....begin setdata....%ld ",(clock()/1000));
		pic_info.eType = (MTPHOTO4K_RGB_TYPE_T)sktype;
		bitmap->lockPixels();
		pic_info.u4DataAddr = (UINT32)bitmap->getAddr(0,0);
		pic_info.u4DataSize = skdatasize;
		pic_info.rDispRegion.u4x = u4x;
		pic_info.rDispRegion.u4y = u4y;
		pic_info.rDispRegion.u4Width = u4Width;
		pic_info.rDispRegion.u4Height = u4Height;



		if(pic_info.u4DataAddr == 0)
		{
			bitmap->unlockPixels();
			LOGE(".....native EEEEEE do_MTPHOTO4K_Show  .....pic_info.u4DataAddr 0.... ");
			return false;
		}
		/*if(buf == NULL)
		{
			if((buf = (char*)malloc(pic_info.u4DataSize))== NULL)
			{
				LOGE(".....native EEEEEE do_MTPHOTO4K_Show  .....malloc failed..... ");
				return false;
			}
			
		}

		memset(buf,0x0,pic_info.u4DataSize);
		memcpy(buf,(char*)pic_info.u4DataAddr,pic_info.u4DataSize);*/

		LOGE(".....native EEEEEE do_MTPHOTO4K_Show:..... eType= %d ,u4DataAddr = 0x%x ,u4DataSize = %d  ",pic_info.eType,pic_info.u4DataAddr,pic_info.u4DataSize);
		LOGE(".....native EEEEEE do_MTPHOTO4K_Show:..... u4x= %d ,u4y = %d ,u4Width = %d ,u4Height = %d ",pic_info.rDispRegion.u4x,pic_info.rDispRegion.u4y,pic_info.rDispRegion.u4Width,pic_info.rDispRegion.u4Height);

		/*LOGE(".....native EEEEEE do_PHOTO4K_Show:..... buf= 0x%x ",(unsigned int)buf);
		if((skdataaddr == 0) || ((unsigned int)buf == 0))
		{
			LOGE(".....native EEEEEE do_MTPHOTO4K_Show  ....skdataaddr == 0 || buf == 0..... ");
			return false;
		}
		pic_info.u4DataAddr = (unsigned int)buf;*/

		LOGE(".....native EEEEEE do_MTPHOTO4K_Show  ....begin MTPHOTO4K_SetDisplayInfo....%ld ",(clock()/1000));
		if(MTR_OK != MTPHOTO4K_SetDisplayInfo(pic_info))
		{
			bitmap->unlockPixels();
			__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### MTPHOTO4K_SetDisplayInfo error! %ld",(clock()/1000));
			return false;
		}

		LOGE(".....native EEEEEE do_MTPHOTO4K_Show  ....begin MTPHOTO4K_Show....%ld ",(clock()/1000));
		if(MTR_OK != MTPHOTO4K_Show(1,TRUE))
		{
			bitmap->unlockPixels();
			__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### MTPHOTO4K_Show error! %ld",(clock()/1000));
			return false;
		}
		bitmap->unlockPixels();
		LOGE(".....native EEEEEE do_MTPHOTO4K_Show  .....true....end %ld ",(clock()/1000));
		return true;
	}	

	LOGE(".....native EEEEEE do_MTPHOTO4K_Show  .....false.... ");
	
	#endif
	return false;
}



static jint nativeInitPhotoPlay(JNIEnv * env, jobject obj,jboolean flag)
{
	LOGE(".....native EEEEEE nativeExitFromPhotoPlay:..... flag= %d,index = %d ",flag);
	MT_RESULT_T     e_return = MTR_OK;
	
	MTAL_InitExCb(FALSE);

	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### nativeExitFromPhotoPlay is called");

#ifdef ALL_PIC_VIDEOPATH	
	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### ALL_PIC_VIDEOPATH is ok");
#else
	//e_return = MTIMAGE_GotoOrExitFromPhotoPlay(flag,(MTIMAGE_FILE_FORMAT)index);
#endif
	
	if (e_return != MTR_OK)
	{
		__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### nativeExitFromPhotoPlay is Failed!!!");
		//jniThrowException(env,"com/mediatek/jni/picjni/NotSupportException","MTIMAGE_GotoOrExitFromPhotoPlay Failed!");
	}
	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### nativeExitFromPhotoPlay is ok");


#ifdef ALL_PIC_VIDEOPATH
#ifdef CC_SUPPORT_MTK_ANDROID_RM
	if (flag)//connect
	{
		if (!RmOpen())
		{
			__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### RmOpen error!");
			return MTR_NOT_OK;
		}
	}
	else//disconnect
	{
		RmClose();
	}
#endif

	fg_videopath = flag;	
	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### pictureapi fg_videopath = %d ",fg_videopath);
	if(flag && !fg_MTPHOTO4K_init)
	{
		if(MTR_OK != MTPHOTO4K_Init())
		{
			__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### MTPHOTO4K_Init error!");
			return MTR_NOT_OK;
		}
		else
		{
			__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### MTPHOTO4K_Init ok!");
			fg_MTPHOTO4K_init = 1;
		}
	}
	
	__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### pictureapi fg_MTPHOTO4K_init = %d ",fg_MTPHOTO4K_init);
	
	if(!flag && fg_MTPHOTO4K_init)
	{
		__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "#### MTPHOTO4K_DeInit !!!");
		MTPHOTO4K_DeInit();
		fg_MTPHOTO4K_init = 0;	
		/*if(buf != NULL)
		{
			free((void *)buf);
		}
		buf = NULL;*/
	}
#endif
	
	return e_return;
}

static const char *classPathName = "com/mediatek/jni/picjni";
static JNINativeMethod methods[] = {
  {"nativeInitPhotoPlay","(Z)I",(void*)nativeInitPhotoPlay},
  {"nativeShowPhotoOnVdp","(IIIIIII)Z",(void*)nativeShowPhotoOnVdp},
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}

// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */
 
typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;
    
    LOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        LOGE("ERROR: registerNatives failed");
        goto bail;
    }
    
    result = JNI_VERSION_1_4;
    
bail:
    return result;
}

