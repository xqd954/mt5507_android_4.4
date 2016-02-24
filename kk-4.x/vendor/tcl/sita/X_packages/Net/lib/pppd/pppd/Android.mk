#========================================
LOCAL_PATH:= $(call my-dir)

define tcl-test-java-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.java" -and -not -name ".*") )
endef

define tcl-test-cpp-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.cpp" -and -not -name ".*") )
endef

define tcl-test-c-files
$(patsubst ./%,%, $(shell cd $(LOCAL_PATH) ; find $(1) -name "*.c" -and -not -name ".*") )
endef

ifneq ($(call tcl-test-c-files),)
	# use Source files.
	# ============================================================
	####
	include $(CLEAR_VARS)
	PPPDSRCS = main.c magic.c fsm.c lcp.c ipcp.c upap.c chap-new.c md5.c ccp.c \
		   ecp.c ipxcp.c auth.c options.c sys-linux.c md4.c chap_ms.c \
		   demand.c utils.c tty.c eap.c chap-md5.c session.c  sha1.c\
		   pppcrypt.c plugins/rp-pppoe/plugin.c \
		   plugins/rp-pppoe/discovery.c \
		   plugins/rp-pppoe/if.c \
		   plugins/rp-pppoe/common.c
		   
	PPPDSRCS += tdb.c spinlock.c

	LOCAL_SRC_FILES:= $(PPPDSRCS)
	LOCAL_C_INCLUDES := \
			$(LOCAL_PATH)/ \
			$(LOCAL_PATH)/plugins/rp-pppoe/ \
			$(LOCAL_PATH)/../ \
			$(LOCAL_PATH)/../include \
			$(LOCAL_PATH)/../pppd \
			external/openssl/include/ \
			external/openssl/crypto/des/ \
			external/openssl/crypt


	LOCAL_CFLAGS += -DRP_VERSION="3.8"

	LOCAL_CFLAGS += -DCHAPMS=1 -DMSLANMAN=1 -DMPPE=1 -DDEBUGMPPEKEY=1 \
	 -DPLUGIN=1  -DDEBUG=1  -DANDROID_CHANGES=1 -DHAVE_PATHS_H=1 -DNEEDDES=1 \
	 -DDESTDIR="/usr/local"
	 
	LOCAL_SHARED_LIBRARIES := \
			 libcrypto libutils libcutils 
	LOCAL_MODULE:= pppd

	LOCAL_MODULE_TAGS := optional

	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/pppd
all_modules: $(LOCAL_PATH)/pppd
$(LOCAL_PATH)/pppd: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
else
	# use binary files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_EXECUTABLES := \
		pppd \

	include $(BUILD_MULTI_PREBUILT)
endif
