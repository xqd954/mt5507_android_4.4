LOCAL_PATH:= $(call my-dir)


ifeq "$(BLUEANGEL_SUPPORT)" "true"
include $(call all-makefiles-under,$(LOCAL_PATH))
endif