
# libfuse lite
LOCAL_PATH := $(call my-dir)

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
		libfuse-lite/fuse.c libfuse-lite/fusermount.c libfuse-lite/fuse_kern_chan.c \
		libfuse-lite/fuse_loop.c libfuse-lite/fuse_lowlevel.c \
		libfuse-lite/fuse_opt.c libfuse-lite/fuse_session.c libfuse-lite/fuse_signals.c\
		libfuse-lite/helper.c libfuse-lite/mount.c libfuse-lite/mount_util.c androidglue/statvfs.c
	LOCAL_C_INCLUDES :=$(LOCAL_PATH)/include/fuse-lite $(LOCAL_PATH)/androidglue/include
	LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64  -DHAVE_CONFIG_H
	LOCAL_MODULE := libfuse-lite
	LOCAL_MODULE_TAGS := eng
	LOCAL_SYSTEM_SHARED_LIBRARIES := libc libcutils
	include $(BUILD_STATIC_LIBRARY)

	# libntfs-3g
	include $(CLEAR_VARS)
	LOCAL_SRC_FILES :=libntfs-3g/acls.c libntfs-3g/attrib.c libntfs-3g/attrlist.c \
		libntfs-3g/bitmap.c libntfs-3g/bootsect.c libntfs-3g/cache.c libntfs-3g/collate.c \
		libntfs-3g/compat.c libntfs-3g/compress.c libntfs-3g/debug.c libntfs-3g/device.c \
		libntfs-3g/dir.c libntfs-3g/efs.c libntfs-3g/index.c libntfs-3g/inode.c \
		libntfs-3g/lcnalloc.c libntfs-3g/logfile.c libntfs-3g/logging.c libntfs-3g/mft.c \
		libntfs-3g/misc.c libntfs-3g/mst.c libntfs-3g/object_id.c libntfs-3g/reparse.c \
		libntfs-3g/runlist.c libntfs-3g/security.c libntfs-3g/unistr.c\
		libntfs-3g/unix_io.c libntfs-3g/volume.c libntfs-3g/realpath.c
	LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/fuse-lite  $(LOCAL_PATH)/include/ntfs-3g
	LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64  -DHAVE_CONFIG_H
	LOCAL_MODULE := libntfs-3g
	LOCAL_MODULE_TAGS := eng
	LOCAL_SYSTEM_SHARED_LIBRARIES := libc libcutils
	include $(BUILD_STATIC_LIBRARY)

	#ntfs-3g
	include $(CLEAR_VARS)
	LOCAL_SRC_FILES := src/ntfs-3g.c src/ntfs-3g_common.c

	LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/fuse-lite $(LOCAL_PATH)/include/ntfs-3g \
				$(LOCAL_PATH)/androidglue/include $(LOCAL_PATH)/src
	LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_CONFIG_H
	LOCAL_MODULE := ntfs-3g
	LOCAL_MODULE_TAGS := eng
	LOCAL_SYSTEM_SHARED_LIBRARIES := libc
	LOCAL_STATIC_LIBRARIES := libfuse-lite libntfs-3g
	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/ntfs-3g
all_modules: $(LOCAL_PATH)/ntfs-3g
$(LOCAL_PATH)/ntfs-3g: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
	# ntfsprogs - ntfsfix
	include $(CLEAR_VARS)
	LOCAL_SRC_FILES := ntfsprogs/ntfsfix.c ntfsprogs/utils.c

	LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/fuse-lite $(LOCAL_PATH)/include/ntfs-3g \
				$(LOCAL_PATH)/androidglue/include $(LOCAL_PATH)/ntfsprogs/
	LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_CONFIG_H
	LOCAL_MODULE := ntfsfix
	LOCAL_MODULE_TAGS := eng
	LOCAL_SHARED_LIBRARIES := libext2_uuid
	LOCAL_SYSTEM_SHARED_LIBRARIES := libc
	LOCAL_STATIC_LIBRARIES := libfuse-lite libntfs-3g
	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/ntfsfix
all_modules: $(LOCAL_PATH)/ntfsfix
$(LOCAL_PATH)/ntfsfix: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
	# ntfsprogs - mkntfs
	include $(CLEAR_VARS)
	LOCAL_SRC_FILES := ntfsprogs/attrdef.c ntfsprogs/boot.c ntfsprogs/sd.c ntfsprogs/mkntfs.c ntfsprogs/utils.c

	LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/fuse-lite $(LOCAL_PATH)/include/ntfs-3g \
				$(LOCAL_PATH)/androidglue/include $(LOCAL_PATH)/ntfsprogs/ \
				external/e2fsprogs/lib
	LOCAL_CFLAGS := -O2 -g -W -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -DHAVE_CONFIG_H
	LOCAL_MODULE := mkntfs
	LOCAL_MODULE_TAGS := eng
	LOCAL_SYSTEM_SHARED_LIBRARIES := libc
	LOCAL_STATIC_LIBRARIES := libfuse-lite libntfs-3g
	include $(BUILD_EXECUTABLE)

##{{cp binary
$(DEFAULT_GOAL): $(LOCAL_PATH)/mkntfs
all_modules: $(LOCAL_PATH)/mkntfs
$(LOCAL_PATH)/mkntfs: $(LOCAL_BUILT_MODULE)
	$(copy-file-to-target-with-cp)	
empty_dummy:
## cp binary}}
	
else
	# use binary files.
	# ============================================================
	include $(CLEAR_VARS)
	LOCAL_MODULE_TAGS := optional
	LOCAL_PREBUILT_EXECUTABLES := \
		mkntfs \
		ntfsfix \
		ntfs-3g
		
	include $(BUILD_MULTI_PREBUILT)
endif
