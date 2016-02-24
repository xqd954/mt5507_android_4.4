LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifneq ($(TARGET_SIMULATOR),true)

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
	include $(CLEAR_VARS)

	LOCAL_SRC_FILES := \
		clear.c \
		compare.c \
		copy.c \
		gen_uuid.c \
		isnull.c \
		pack.c \
		parse.c \
		unpack.c \
		unparse.c \
		uuid_time.c


	LOCAL_MODULE := libvold_uuid_tcl
	LOCAL_SYSTEM_SHARED_LIBRARIES := libc
	LOCAL_MODULE_TAGS:= eng
	LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../fstype_support/lib

	LOCAL_CFLAGS := -O2 -g -W -Wall \
		-DHAVE_INTTYPES_H \
		-DHAVE_UNISTD_H \
		-DHAVE_ERRNO_H \
		-DHAVE_NETINET_IN_H \
		-DHAVE_SYS_IOCTL_H \
		-DHAVE_SYS_MMAN_H \
		-DHAVE_SYS_MOUNT_H \
		-DHAVE_SYS_PRCTL_H \
		-DHAVE_SYS_RESOURCE_H \
		-DHAVE_SYS_SELECT_H \
		-DHAVE_SYS_STAT_H \
		-DHAVE_SYS_TYPES_H \
		-DHAVE_STDLIB_H \
		-DHAVE_STRDUP \
		-DHAVE_MMAP \
		-DHAVE_UTIME_H \
		-DHAVE_GETPAGESIZE \
		-DHAVE_LSEEK64 \
		-DHAVE_LSEEK64_PROTOTYPE \
		-DHAVE_LINUX_FD_H \
		-DHAVE_TYPE_SSIZE_T

	LOCAL_PRELINK_MODULE := false
	include $(BUILD_SHARED_LIBRARY)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/libvold_uuid_tcl.so
all_modules: $(LOCAL_PATH)/libvold_uuid_tcl.so
$(LOCAL_PATH)/libvold_uuid_tcl.so: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
else
	# use binary files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_LIBS := \
		libvold_uuid_tcl.so

	include $(BUILD_MULTI_PREBUILT)
endif
endif
