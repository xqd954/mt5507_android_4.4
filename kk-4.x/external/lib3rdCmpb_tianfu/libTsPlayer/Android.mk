LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#$(warning VM_LINUX_ROOT=$(VM_LINUX_ROOT))
#$(warning pwd=$(shell pwd))


VM_LINUX_TOP ?= $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
#ANDROID_ROOT ?= $(VM_LINUX_TOP)/android

LOCAL_SRC_FILES:= TsPlayer.cpp ipc_msgq.c listop.c block_cond.c IPTVMediaPlayerStub.cpp MTKIPTVMediaPlayer.cpp 

LOCAL_C_INCLUDES := \
    $(VM_LINUX_TOP)/project_x/middleware/cmpb

LOCAL_PRELINK_MODULE := false    


ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl -lpthread
endif

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

#LOCAL_SHARED_LIBRARIES :=     \
	libcutils             			\
	libutils              			\
	libbinder             			\
	libvorbisidec         			\
	libsonivox            			\
	libmedia              			\
	libandroid_runtime    			\
	libstagefright        			\
	libstagefright_omx    			\
	libsurfaceflinger_client        \
	libdtv_getline    
	
LOCAL_SHARED_LIBRARIES :=     \
     libcutils                \
     libutils                 \
     libbinder                \
     libvorbisidec            \
     libsonivox               \
     libmedia                 \
     libandroid_runtime       \
     libstagefright           \
     libstagefright_omx       \
     libgui                   \
     libdtv_getline
	
ifeq "$(ANDROID_VERSION)" "ics-4.x"
LOCAL_SHARED_LIBRARIES += libsurfaceflinger_client 
endif 

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_SHARED_LIBRARIES += libdl
endif

ifndef RLS_CUSTOM_BUILD
	export RLS_CUSTOM_BUILD=$(shell if [ -d $(LINUX_ROOT)/project_x/mtk_obj ]; then echo 'true'; else echo 'false'; fi )
endif
ifneq "$(RLS_CUSTOM_BUILD)" "true"
	LOCAL_SHARED_LIBRARIES += libstlport  libhttplive  librtspplayer  libcrypto
endif

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -g -Wall -O0 -mapcs-frame -DANDROID -pipe -fPIC -fsigned-char -Werror -Wno-multichar -D_CPU_LITTLE_ENDIAN_ -DUSE_SLIM_LIB -DHAVE_DTCP -DSLIM_BUILDING_LIBRARIES \
-DHAVE_WMDRM -DTARGET=10 -DUSE_SLIM_LIB -D_ANSI -D_CPU_LITTLE_ENDIAN_ -D_USE_ANDROID -D_USE_LINUX -D__arm -D_FILE_OFFSET_BITS=64 -DLOG_FOR_LOGCAT

LOCAL_CXXFLAGS := -g -Wall -O0 -DANDROID -pipe -fPIC -fsigned-char -Werror -Wno-multichar -D_CPU_LITTLE_ENDIAN_ -mapcs-frame -DUSE_SLIM_LIB -DHAVE_DTCP -DSLIM_BUILDING_LIBRARIES \
-DHAVE_WMDRM -DTARGET=10 -DUSE_SLIM_LIB -D_ANSI -D_CPU_LITTLE_ENDIAN_ -D_USE_ANDROID -D_USE_LINUX -D__arm -D_FILE_OFFSET_BITS=64 -DLOG_FOR_LOGCAT
#-DHAVE_SYS_UIO_H had defined in system/core/include/arch/linux-arm/AndroidConfig.h:107

LOCAL_LDFLAGS += -L$(DEVICE_STRING)/DTV_OUT/shares_libraries \
 -lcmpb \
 -lmtal \
 -ldtv_osai \
 -ldtv_common \
 -lhandle_app \
 -lapp_if_rpc


LOCAL_MODULE_TAGS := eng
LOCAL_MODULE:= libTsPlayer_tianfu

include $(BUILD_SHARED_LIBRARY)

#include $(call all-makefiles-under,$(LOCAL_PATH))
