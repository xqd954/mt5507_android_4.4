ifneq "$(TCL_PRODUCT_BRANCH)" "EM"
LOCAL_PATH:= $(call my-dir)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif