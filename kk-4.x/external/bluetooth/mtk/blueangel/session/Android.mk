################################################
# build BT session
################################################
LOCAL_PATH = $(CUR_PATH)
#BT_PATH := external/bluetooth/mtk/blueangel
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := libcutils libutils libextsys

LOCAL_SRC_FILES := \
	session/src/session.c \
	session/src/bt_session.c \
	session/src/session_api.c \
	session/src/session_group.c \
	session/src/session_handler.c \
	session/src/session_os.c \
	session/src/session_stream.c \
	session/src/session_master.c \

LOCAL_CFLAGS += -DBTMTK_ON_LINUX

LOCAL_C_INCLUDES := \
	$(BT_PATH)/session/include \
	$(BT_PATH)/btcore/inc \
	$(BT_PATH)/btcore/inc/sys \
	$(BT_PATH)/btadp_int/include \
	$(BT_PATH)/include/common/default \
	$(BT_PATH)/include/common/ \
	$(BT_PATH)/include/ \
	$(BT_PATH)/include/profiles \
	$(BT_PATH)/include/pal \
	

LOCAL_MODULE := libbtsession

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)