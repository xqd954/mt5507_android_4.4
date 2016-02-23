LOCAL_PATH:= $(call my-dir)

# the library
# ============================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
            $(call all-subdir-java-files) \
	    com/android/server/EventLogTags.logtags \
	    com/android/server/am/EventLogTags.logtags

LOCAL_MODULE:= services

LOCAL_JAVA_LIBRARIES := android.policy conscrypt telephony-common
#LOCAL_JAVA_LIBRARIES += com.tcl.tvos.addon
LOCAL_JAVA_LIBRARIES += com.mediatek.dm
LOCAL_JAVA_LIBRARIES += com.mediatek.wfdsink
LOCAL_JAVA_LIBRARIES += com.mediatek.common.powercontrol
LOCAL_JAVA_LIBRARIES += com.mediatek.util
LOCAL_JAVA_LIBRARIES += com.tcl.tvmanager com.tcl.factory com.tcl.snapshot
#add by gaodw.
#LOCAL_JAVA_LIBRARIES += com.tcl.deviceinfo com.tcl.devicemanager com.tcl.seeker android.tclwidget
LOCAL_JAVA_LIBRARIES += com.tcl.deviceinfo com.tcl.devicemanager com.tcl.seeker 
#end.

ifneq "$(HYBRID_MODE)" "true"
ifneq "$(ANDROID_TWO_WORLDS)" "true"
LOCAL_JAVA_LIBRARIES += com.mediatek.tv
ifneq "$(RLS_CUSTOM_BUILD)" "true"
LOCAL_JAVA_LIBRARIES += com.mediatek.record
endif
endif # endif ANDROID_TWO_WORLDS
endif # endif HYBRID_MODE

include $(BUILD_JAVA_LIBRARY)

include $(BUILD_DROIDDOC)
