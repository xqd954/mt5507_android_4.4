TV_ADDON_TOP := $(word 1, $(subst /vm_linux/android/,/vm_linux/android /, $(shell pwd)))
TV_ADDON_TOP := $(TV_ADDON_TOP)/dtv-android/tv_addon
VM_LINUX_ROOT := $(TV_ADDON_TOP)/../../../../vm_linux
TV_ADDON_TOP_EXIST := $(shell test -d $(TV_ADDON_TOP) && echo "true" || echo "false")
