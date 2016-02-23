#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <semaphore.h>

//----
#include <utils/Log.h>
#include <stdio.h>
#include "jni.h"
#define  LOG_TAG    "video_test"

#include <GLES/gl.h>
#include <GLES/glext.h>
//---

#define MALI_IMPORT // XXX resolve compile error

#include <mtal.h>
#include <mtvss.h>

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480
#define WINDOW_BPP     16

#define RED_SIZE   8
#define GREEN_SIZE 8
#define BLUE_SIZE  8

//#define USE_NORMAL
#define N 10 // flag tesselation N*N

//typedef _Bool bool;

typedef struct{
    int tex_width, tex_height;
    pthread_mutex_t mutex;
}shared_data;
MT_RESULT_T r;
// XXX should not defined here, just for pass build
#if 0
void MTVSS_PRINT(const char *s){
}
#endif


unsigned char *gpbuf  = NULL;

unsigned int ListTextures;

shared_data gData;

#define Safe_Free(p) if((p)) {free((p)); (p) = NULL;}
	
	
pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;
pthread_t threadId;
sem_t Framesem;
bool gThradExit = false;

void* GetFrameBufferThead(void* pData)
{
		int width = gData.tex_width;
  	int height = gData.tex_height;
  	LOGI("====>GetFrameBufferThead MTVSS_GetFrameBuffer size:%d,%d\n",width,height);
		while(!gThradExit)
		{
			LOGI("start MTVSS_GetFrameBuffer \n");
			pthread_mutex_lock(&mp);
			MTVSS_GetFrameBuffer(0, 0, width, height, gpbuf);
			pthread_mutex_unlock(&mp);
    	LOGI("ends MTVSS_GetFrameBuffer \n");
    	
    	sem_wait(&Framesem);
			//usleep(10);
		}
	
		return NULL;
}

void init_thread()
{
	
		sem_init(&Framesem,0,0);
		int ret = pthread_mutex_init(&mp,NULL);	
		pthread_create(&threadId,NULL,GetFrameBufferThead,(void*)NULL);
}
	

void MTVSS_GetFrameBufferXXX(int x, int y, int width, int height, unsigned char *buf){
    static int frame = 0;
    static unsigned char *buf0 = NULL, *buf1 = NULL;
#if 1
    unsigned int col;
    int i, j;
    if(++frame % 2)
        col = 0xff0000ff;
    else
        col = 0xff00ff00;
    for(i = y; i < height; i++)
        for(j = x; j < width; j++){
            *((int*)buf) = col;
            buf += 4;
        }

#endif
}

static inline long timediff(struct timeval tv1, struct timeval tv2){
    return (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
}

int init_egl(){
   
    return 0;
}

int init_vss(shared_data *data){
    UINT16 width=1280, height=720, color=0, source=0;
		LOGE("init_vss : MTAL_Init....\n");
    r = MTAL_InitExCb(false);
    if(r != MTR_OK)
		{
			LOGE("ERROR: init_vss : MTAL_Init.... , r = %d \n",r);
		}
    assert(r == MTR_OK);
    r = MTVSS_Init();
    if(r != MTR_OK)
		{
			LOGE("ERROR: init_vss : MTVSS_Init.... , r = %d \n",r);
		}
    assert(r == MTR_OK);

    //r = MTVSS_SetColorFormat(0x00020001); // ABGR
    //r = MTVSS_SetColorFormat(0x01020001); // ABGR
    r = MTVSS_Connect(MTVDO_MAIN);
    if(r != MTR_OK)
		{
			LOGE("ERROR: init_vss : MTVSS_Connect.... , r = %d \n",r);
		}
    assert(r == MTR_OK);
    r = MTVSS_GetVideoInfo(&width, &height, &color, &source);
    if(r != MTR_OK)
		{
			LOGE("ERROR: init_vss : MTVSS_GetVideoInfo.... , r = %d \n",r);
		}
    assert(r == MTR_OK);
  

    if(width % 4){
        width = (int)(width / 4) * 4;
        LOGE("width not multiple of 4, adjust to %d\n", width);
    }
    data->tex_width = width;
    data->tex_height = height;

    return 0;
}

int init_opengl(shared_data *data){


    return 0;
}

int init_tex(shared_data *data){
  

    return 0;
}

int init_flag(shared_data *data){
  

    return 0;
}

int anim_flag(shared_data *data){
   

    return 0;
}

int draw_flag(shared_data *data){
  
    return 0;
}

void resetBufferSize()
{
	  // LOGE("Vsstex funtion getvidoinfo .....\n");
	  UINT16 width=1280, height=720, color=0, source=0;
    r = MTVSS_GetVideoInfo(&width, &height, &color, &source);
    LOGE("resetBufferSize MTVSS_GetVideoInfo remalloc buffer ok width = %d, height = %d \n",width ,height);
    assert(r == MTR_OK);
    if(r!= MTR_OK)
    {
    	
    		LOGE("Vsstex funtion getvidoinfo error. ret = \n",r);
    }
    if(width % 4){
        width = (int)(width / 4) * 4;
    }
    if(gData.tex_width != width ||gData.tex_height != height)
    {
	    gData.tex_width = width;
	    gData.tex_height = height;
      if(gpbuf != NULL)
			{
				free(gpbuf);
				gpbuf = NULL;
				gpbuf = (unsigned char *)malloc(gData.tex_width * gData.tex_height * 4);
				if(gpbuf == NULL)
				{
						LOGE("error:malloc buffer is error\n");
				}
				else
				{
						LOGE("resetBufferSize malloc buffer use memset......\n");
						memset(gpbuf,0,sizeof(gData.tex_width * gData.tex_height * 4));
				}
				LOGE("resetBufferSize  remalloc buffer ok width = %d, height = %d \n",gData.tex_width ,gData.tex_height);
			}
    }
}
int update_tex(int texId)
{
		ListTextures = texId;
		resetBufferSize();
  	int width = gData.tex_width;
  	int height = gData.tex_height;
#if 0
    MTVSS_GetFrameBufferXXX(0, 0, width, height, gpbuf);
#else
		if(gpbuf == NULL)
		{
			LOGE("erro:malloc buffer is erro\n");
			return 0;
		}
		else
		{
			memset(gpbuf,0,sizeof(gData.tex_width * gData.tex_height * 4));
   	}
   	MT_RESULT_T res = MTVSS_GetFrameBuffer(0, 0, width, height, gpbuf);
   	if(res != MTR_OK)
   	{ 		
   			LOGE("MTVSS_GetFrameBuffer ddddd res:%d,width = %d, height = %d do nothing\n",(int)res,width, height);
   			return 0;
   	}else{
   	  // 	LOGE("MTVSS_GetFrameBuffer  width = %d, height = %d \n",width, height);	
   	}    
#endif

//#if 1

        static int dump = 0;
        if(dump == 11){
        	LOGI("dump video+++data->tex_width:%d,data->tex_height:%d\n",width,height);
            FILE *fp = fopen("/data/dump", "wb");
            if(fp)
            {
            	fwrite(gpbuf, width*height*4, 1, fp);
            	fclose(fp);
          	}
          	else
          		LOGI("erro: fopen is erro\n");
        }
        dump++;

//#else

		
		//LOGI("start glTexImage2D \n");
		glBindTexture(GL_TEXTURE_2D,texId);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    //pthread_mutex_lock(&mp);
//    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
 //   glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
 //   glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,gpbuf);
		
		//pthread_mutex_unlock(&mp);
		
		//LOGI("ends glTexImage2D \n");
		
		//sem_post(&Framesem);

//#endif	

		MTVSS_SetTrigger();

    return 0;
}


int main(int argc, char** argv)
{
   
    return 0;
}

void init()
{
    //pthread_t thread1;
		LOGE("Vsstex funtion initial....\n");
    init_egl();
    init_vss(&gData);
    init_opengl(&gData);
    init_tex(&gData);
    init_flag(&gData);
    
		gpbuf = (unsigned char *)malloc(gData.tex_width * gData.tex_height * 4);
		if(gpbuf == NULL)
		{
			LOGE("erro:malloc buffer is erro\n");
		}
		else
			memset(gpbuf,0,sizeof(gData.tex_width * gData.tex_height * 4));
		
		//init_thread();
}

void Uninit()
{
		//exit thread
		gThradExit = true;
		//pthread_join(threadId,NULL);
		//sem_destroy(&Framesem);
		
		Safe_Free(gpbuf);
}


//------------temp

int gWidht = 480;
int gheight = 800;

 void prepareFrame()
{
	
    glViewport(0, 0, gWidht, gheight);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
   // gluPerspective(45, (float)width / height, 0.5f, 150);
    glOrthof(-gWidht/2,gWidht/2, -gheight/2,gheight/2, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    
}

 void drawModels()
{
	
  float widht = 200.0f;
	float height = 200.0f;
	float Ground_VS[8] = {
	    -widht, height,
            widht,height,
         
            widht,-height,
            -widht,-height
	};

	float Ground_TS[8] = 
	{
	   (float)0,(float)0,
           (float)1,(float)0,
           (float)1,(float)1,
           (float)0,(float)1
	};

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ListTextures);

    glVertexPointer(2, GL_FLOAT, 0, Ground_VS);
    glTexCoordPointer(2, GL_FLOAT, 0,Ground_TS);
   
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
   // glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}


void appRender()
{
    prepareFrame();

    drawModels();
}


//-----------------------jni--------------------------

static void nativeInit(JNIEnv * env, jobject obj)
{
		init();
}

static void nativeTimingChange(JNIEnv * env, jobject obj)
{
		UINT16 width=1280, height=720, color=0, source=0;
		LOGE("Change timing nativeInitssssssssssss....\n");
		r = MTVSS_Disconnect(MTVDO_MAIN);
		if(r != MTR_OK)
		{
			LOGE("ERROR: nativeTimingChange : MTVSS_Disconnect , r = %d \n",r);
		}
		assert(r == MTR_OK);
    r = MTVSS_Init();
    if(r != MTR_OK)
		{
			LOGE("ERROR: nativeTimingChange : MTVSS_Init , r = %d \n",r);
		}
    assert(r == MTR_OK);
    r = MTVSS_Connect(MTVDO_MAIN);
    if(r != MTR_OK)
		{
			LOGE("ERROR: nativeTimingChange : MTVSS_Connect , r = %d \n",r);
		}
    assert(r == MTR_OK);
    r = MTVSS_GetVideoInfo(&width, &height, &color, &source);
    if(r != MTR_OK)
		{
			LOGE("ERROR: nativeTimingChange : MTVSS_GetVideoInfo , r = %d \n",r);
		}
    assert(r == MTR_OK);
  

    if(width % 4){
        width = (int)(width / 4) * 4;
        LOGE("width not multiple of 4, adjust to %d\n", width);
    }
    gData.tex_width = width;
    gData.tex_height = height;
    
    if(gpbuf != NULL)
		{
					free(gpbuf);
					gpbuf = NULL;
					gpbuf = (unsigned char *)malloc(gData.tex_width * gData.tex_height * 4);
					if(gpbuf == NULL)
					{
						LOGE("erro:malloc buffer is erro\n");
					}
					else
					{
						LOGE("malloc buffer use memset...\n");
						memset(gpbuf,0,sizeof(gData.tex_width * gData.tex_height * 4));
					}
					LOGE("remalloc buffer ok width = %d, height = %d \n",gData.tex_width ,gData.tex_height);
		}
}

static void nativeUnInit(JNIEnv * env, jobject obj)
{
	 MTVSS_Disconnect(MTVDO_MAIN);
	 if(gpbuf != NULL)
	 {
	 		free(gpbuf);
	    gpbuf = NULL;
	 }

}
static jint nativeGetWidth(JNIEnv * env, jobject obj)
{
		UINT16 width=1280, height=720, color=0, source=0;
    r = MTVSS_GetVideoInfo(&width, &height, &color, &source);
    assert(r == MTR_OK);
    if(width % 4){
        width = (int)(width / 4) * 4;
    }
    LOGE("GetWidth width = %d\n",width);

 // gData.tex_width = width;
 // gData.tex_height = height;
	return width;
}
static jint nativeGetHeight(JNIEnv * env, jobject obj)
{
		UINT16 width=1280, height=720, color=0, source=0;
    r = MTVSS_GetVideoInfo(&width, &height, &color, &source);
    assert(r == MTR_OK);
    if(width % 4){
        width = (int)(width / 4) * 4;
   }
LOGE("GetHeight Height = %d\n",height);
  //gData.tex_width = width;
  //gData.tex_height = height;
	return height;
}
static void nativeDone(JNIEnv * env, jobject obj)
{
		Uninit();
}
static void nativeRender(JNIEnv * env, jobject obj)
{
}
static void nativeResize(JNIEnv *env , jobject obj,jint w, jint h) 
{
  LOGE("###resize the width and heightqqqqqqqqqqqqqqqqqq");
	resetBufferSize();
  LOGE("###resize the width and height");
}

static void nativeUpdateTexture(JNIEnv * env, jobject obj,jint texId)
{
	update_tex(texId);
}

static jint
add(JNIEnv *env, jobject thiz, jint a, jint b) {
int result = a + b;
    LOGI("%d + %d = %d", a, b, result);
    return result;
}

static const char *classPathName = "com/opengl/jni/GLJniLib";

static JNINativeMethod methods[] = {
  {"add", "(II)I", (void*)add },
  {"nativeInit","()V",(void*)nativeInit},
  {"nativeUnInit","()V",(void*)nativeUnInit},
  {"nativeTimingChange","()V",(void*)nativeTimingChange},
  {"nativeGetWidth","()I",(void*)nativeGetWidth},
  {"nativeGetHeight","()I",(void*)nativeGetHeight},
  {"nativeDone","()V",(void*)nativeDone},
	{"nativeRender","()V",(void*)nativeRender},
	{"nativeResize","(II)V",(void*)nativeResize},
	{"nativeUpdateTexture","(I)V",(void*)nativeUpdateTexture},
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
