hardware_modules := gralloc hwcomposer audio nfc nfc-nci local_time \
	power usbaudio audio_remote_submix camera consumerir

ifeq "$(SETTING_BACKLIGHT_SUPPORT)" "true"
    hardware_modules += lights
endif

include $(call all-named-subdir-makefiles,$(hardware_modules))
