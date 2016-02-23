

LOCAL_PATH := $(call my-dir)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

VM_LINUX_ROOT ?= $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
ANDROID_ROOT ?= $(VM_LINUX_ROOT)/android

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES :=  \
    common/rpc/rpc_init.c \
    common/playback/playback.c  \
    main.c

LOCAL_C_INCLUDES := \
    $(VM_LINUX_ROOT)/project_x/middleware/cmpb  \
    $(ANDROID_ROOT)/kk-4.x/external/lib3rdCmpb_tianfu/push_mode/common/include

LOCAL_PRELINK_MODULE := false    


ifeq ($(TARGET_OS)-$(TARGET_SIMULATOR),linux-true)
LOCAL_LDLIBS += -ldl -lpthread
endif

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

LOCAL_SHARED_LIBRARIES += libstlport  libhttplive  librtspplayer  libvospplayer  libcrypto libcurl

DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)
ifeq "$(ANDROID_VERSION)" "jb-4.x"
LOCAL_LDFLAGS += -L$(DEVICE_STRING)/DTV_OUT/shares_libraries \
    -lmtal
endif	

ifeq "$(ANDROID_VERSION)" "kk-4.x"
LOCAL_LDFLAGS += -L$(DEVICE_STRING)/DTV_OUT/shares_libraries \
    -lmtal
endif	

LOCAL_LDFLAGS += -L$(DEVICE_STRING)/DTV_OUT/shares_libraries \
 -lcmpb \
 -lmtal \
 -ldtv_osai \
 -ldtv_common \
 -lhandle_app \
 -lapp_if_rpc
 

LOCAL_SHARED_LIBRARIES += libstlport  libhttplive  librtspplayer  libcrypto

LOCAL_PRELINK_MODULE := false

LOCAL_CFLAGS := -g -Wall -O0 -mapcs-frame -DANDROID -pipe -fPIC -fsigned-char -Werror -Wno-multichar -D_CPU_LITTLE_ENDIAN_ -DUSE_SLIM_LIB -DHAVE_DTCP -DSLIM_BUILDING_LIBRARIES \
-DHAVE_WMDRM -DTARGET=10 -DUSE_SLIM_LIB -D_ANSI -D_CPU_LITTLE_ENDIAN_ -D_USE_ANDROID -D_USE_LINUX -D__arm -D_FILE_OFFSET_BITS=64

LOCAL_CXXFLAGS := -g -Wall -O0 -DANDROID -pipe -fPIC -fsigned-char -Werror -Wno-multichar -D_CPU_LITTLE_ENDIAN_ -mapcs-frame -DUSE_SLIM_LIB -DHAVE_DTCP -DSLIM_BUILDING_LIBRARIES \
-DHAVE_WMDRM -DTARGET=10 -DUSE_SLIM_LIB -D_ANSI -D_CPU_LITTLE_ENDIAN_ -D_USE_ANDROID -D_USE_LINUX -D__arm -D_FILE_OFFSET_BITS=64
#-DANDROID_LOG
#-DHAVE_SYS_UIO_H had defined in system/core/include/arch/linux-arm/AndroidConfig.h:107

LOCAL_MODULE:= cmpb_push.bin

include $(BUILD_EXECUTABLE)

