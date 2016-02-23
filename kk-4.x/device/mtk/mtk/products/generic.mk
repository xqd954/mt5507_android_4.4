#
# Copyright (C) 2011 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#
# This file is the build configuration for a full Android
# build for generic mtk hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#

ifndef IC_SETTING
$(error IC_SETTING is undefined)
endif

DEVICE_STRING = device/$(BRAND)/$(ODM)/$(IC_SETTING)

include $(DEVICE_STRING)/system.mk

TARGET_PROVIDES_INIT_RC := true
PREBUIT_PATH := ../mediatek/device/prebuilt
DTV_OUT_PATH := $(DEVICE_STRING)/DTV_OUT

PRODUCT_AAPT_CONFIG += large

ifeq "ROM2EMMC" "$(BOOT_TYPE)"
PRODUCT_COPY_FILES := $(DEVICE_STRING)/configs/init.rc:root/init.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.mtk_common.rc:root/init.mtk_common.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.environ.rc.in:root/init.environ.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.usb.rc:root/init.usb.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.build_cfg.$(BUILD_CFG).rc:root/init.build_cfg.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.hybrid_cfg.$(HYBRID_CFG).rc:root/init.hybrid_cfg.rc

ifeq ($(FORCE_COREDUMP),true)
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.coredump.rc:root/init.coredump.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.coredump_folder.rc:root/init.coredump_folder.rc
endif

ifeq ($(SWITCH_COREDUMP),true)
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.coredump.rc:root/init.coredump.rc
endif

ifeq ($(AUTO_TEST),true)
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/script/autotest.sh:system/etc/script/autotest.sh
export PRODUCT_PACKAGES := $(filter-out adb, $(PRODUCT_PACKAGES))
export PRODUCT_PACKAGES := $(filter-out adbd, $(PRODUCT_PACKAGES))
PRODUCT_COPY_FILES += $(PREBUIT_PATH)/adb_jb-4.x:system/bin/adb
PRODUCT_COPY_FILES += $(PREBUIT_PATH)/adbd_jb-4.x:root/sbin/adbd
endif

ifeq ($(DM_VERITY),true)
PRODUCT_COPY_FILES += $(DTV_OUT_PATH)/emmc_dm_verity_mount.rc:root/init.device.rc
else
PRODUCT_COPY_FILES += $(DTV_OUT_PATH)/emmc_mount.rc:root/init.device.rc
endif

PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/exmount_emmc.cfg:root/exmount.cfg
else
ifneq ($(NAND_BOOT),false)
PRODUCT_COPY_FILES := $(DEVICE_STRING)/configs/init.rc:root/init.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/nand_mount.rc:root/init.device.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/exmount.cfg:root/exmount.cfg
else
PRODUCT_COPY_FILES := $(DEVICE_STRING)/configs/init_for_usb_disk.rc:root/init.rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/exmount.cfg:root/exmount.cfg
endif
endif

PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.recovery.$(ANDROID_IC_SETTING).rc:root/init.recovery.$(ANDROID_IC_SETTING).rc
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.build_type.$(TARGET_BUILD_VARIANT).rc:root/init.build_type.rc

#use customized target ueventd.rc to replace original one
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/ueventd.rc:root/ueventd.rc

ifeq ($(ANDROID_TWO_WORLDS),true)
PRODUCT_COPY_FILES += $(ANDROID_TOP)/$(DEVICE_STRING)/DTV_OUT/dev_linux.txt:system/etc/dev_linux.txt
ifeq ($(DM_VERITY),true)
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.android_two_worlds_dm_verity.rc:root/init.android_two_worlds.rc
else
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.android_two_worlds.rc:root/init.android_two_worlds.rc
endif
endif

#Target init scripts
ifeq ($(TCL_PRODUCT_BRANCH),CN_C1)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.$(ANDROID_IC_SETTING).c1.rc:root/init.$(ANDROID_IC_SETTING).rc \
    $(DEVICE_STRING)/configs/init.trace.rc:root/init.trace.rc \
    $(DEVICE_STRING)/configs/script/mount_cache.sh:system/etc/script/mount_cache.sh \
    $(DEVICE_STRING)/configs/script/adb_net.sh:system/etc/script/adb_net.sh \
    $(DEVICE_STRING)/configs/script/insmod.sh:system/etc/script/insmod.sh \
    $(DEVICE_STRING)/configs/script/flashClone.sh:system/etc/script/flashClone.sh \
    $(DEVICE_STRING)/configs/script/set_eth0_mac.sh:system/etc/script/set_eth0_mac.sh \
    $(DEVICE_STRING)/configs/script/disable_mediascanner.sh:system/etc/script/disable_mediascanner.sh \
    $(DEVICE_STRING)/configs/dev.txt:system/etc/dev.txt \
    $(DEVICE_STRING)/configs/service/post-init.sh:system/etc/service/post-init.sh \
    $(DEVICE_STRING)/configs/service/config_ttyMT0.sh:system/etc/service/config_ttyMT0.sh \
    $(DEVICE_STRING)/configs/service/pre-init.sh:system/etc/service/pre-init.sh \
    $(DEVICE_STRING)/configs/mtk_bugreport.sh:system/bin/mtk_bugreport.sh \
    $(DEVICE_STRING)/configs/script/unmount.sh:root/unmount.sh \
    $(DEVICE_STRING)/configs/script/mapsanalysis.sh:system/bin/mapsanalysis.sh \
    $(DEVICE_STRING)/configs/script/mtk-memreport.sh:system/bin/mtk-memreport.sh
else 
ifeq ($(TCL_PRODUCT_BRANCH),CN_P1)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.$(ANDROID_IC_SETTING).c1.rc:root/init.$(ANDROID_IC_SETTING).rc \
    $(DEVICE_STRING)/configs/init.trace.rc:root/init.trace.rc \
    $(DEVICE_STRING)/configs/script/mount_cache.sh:system/etc/script/mount_cache.sh \
    $(DEVICE_STRING)/configs/script/adb_net.sh:system/etc/script/adb_net.sh \
    $(DEVICE_STRING)/configs/script/insmod.sh:system/etc/script/insmod.sh \
    $(DEVICE_STRING)/configs/script/flashClone.sh:system/etc/script/flashClone.sh \
    $(DEVICE_STRING)/configs/script/set_eth0_mac.sh:system/etc/script/set_eth0_mac.sh \
    $(DEVICE_STRING)/configs/script/disable_mediascanner.sh:system/etc/script/disable_mediascanner.sh \
    $(DEVICE_STRING)/configs/dev.txt:system/etc/dev.txt \
    $(DEVICE_STRING)/configs/service/post-init.sh:system/etc/service/post-init.sh \
    $(DEVICE_STRING)/configs/service/config_ttyMT0.sh:system/etc/service/config_ttyMT0.sh \
    $(DEVICE_STRING)/configs/service/pre-init.sh:system/etc/service/pre-init.sh \
    $(DEVICE_STRING)/configs/mtk_bugreport.sh:system/bin/mtk_bugreport.sh \
    $(DEVICE_STRING)/configs/script/unmount.sh:root/unmount.sh \
    $(DEVICE_STRING)/configs/script/mapsanalysis.sh:system/bin/mapsanalysis.sh \
    $(DEVICE_STRING)/configs/script/mtk-memreport.sh:system/bin/mtk-memreport.sh
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_F3800_CMP)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.$(ANDROID_IC_SETTING).c1.rc:root/init.$(ANDROID_IC_SETTING).rc \
    $(DEVICE_STRING)/configs/init.trace.rc:root/init.trace.rc \
    $(DEVICE_STRING)/configs/script/mount_cache.sh:system/etc/script/mount_cache.sh \
    $(DEVICE_STRING)/configs/script/adb_net.sh:system/etc/script/adb_net.sh \
    $(DEVICE_STRING)/configs/script/insmod.sh:system/etc/script/insmod.sh \
    $(DEVICE_STRING)/configs/script/flashClone.sh:system/etc/script/flashClone.sh \
    $(DEVICE_STRING)/configs/script/set_eth0_mac.sh:system/etc/script/set_eth0_mac.sh \
    $(DEVICE_STRING)/configs/script/disable_mediascanner.sh:system/etc/script/disable_mediascanner.sh \
    $(DEVICE_STRING)/configs/dev.txt:system/etc/dev.txt \
    $(DEVICE_STRING)/configs/service/post-init.sh:system/etc/service/post-init.sh \
    $(DEVICE_STRING)/configs/service/config_ttyMT0.sh:system/etc/service/config_ttyMT0.sh \
    $(DEVICE_STRING)/configs/service/pre-init.sh:system/etc/service/pre-init.sh \
    $(DEVICE_STRING)/configs/mtk_bugreport.sh:system/bin/mtk_bugreport.sh \
    $(DEVICE_STRING)/configs/script/unmount.sh:root/unmount.sh \
    $(DEVICE_STRING)/configs/script/mapsanalysis.sh:system/bin/mapsanalysis.sh \
    $(DEVICE_STRING)/configs/script/mtk-memreport.sh:system/bin/mtk-memreport.sh
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_H8800_CMP)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.$(ANDROID_IC_SETTING).c1.rc:root/init.$(ANDROID_IC_SETTING).rc \
    $(DEVICE_STRING)/configs/init.trace.rc:root/init.trace.rc \
    $(DEVICE_STRING)/configs/script/mount_cache.sh:system/etc/script/mount_cache.sh \
    $(DEVICE_STRING)/configs/script/adb_net.sh:system/etc/script/adb_net.sh \
    $(DEVICE_STRING)/configs/script/insmod.sh:system/etc/script/insmod.sh \
    $(DEVICE_STRING)/configs/script/flashClone.sh:system/etc/script/flashClone.sh \
    $(DEVICE_STRING)/configs/script/set_eth0_mac.sh:system/etc/script/set_eth0_mac.sh \
    $(DEVICE_STRING)/configs/script/disable_mediascanner.sh:system/etc/script/disable_mediascanner.sh \
    $(DEVICE_STRING)/configs/dev.txt:system/etc/dev.txt \
    $(DEVICE_STRING)/configs/service/post-init.sh:system/etc/service/post-init.sh \
    $(DEVICE_STRING)/configs/service/config_ttyMT0.sh:system/etc/service/config_ttyMT0.sh \
    $(DEVICE_STRING)/configs/service/pre-init.sh:system/etc/service/pre-init.sh \
    $(DEVICE_STRING)/configs/mtk_bugreport.sh:system/bin/mtk_bugreport.sh \
    $(DEVICE_STRING)/configs/script/unmount.sh:root/unmount.sh \
    $(DEVICE_STRING)/configs/script/mapsanalysis.sh:system/bin/mapsanalysis.sh \
    $(DEVICE_STRING)/configs/script/mtk-memreport.sh:system/bin/mtk-memreport.sh
else
ifeq ($(TCL_PRODUCT_BRANCH),CN_H5800_CMP)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.$(ANDROID_IC_SETTING).c1.rc:root/init.$(ANDROID_IC_SETTING).rc \
    $(DEVICE_STRING)/configs/init.trace.rc:root/init.trace.rc \
    $(DEVICE_STRING)/configs/script/mount_cache.sh:system/etc/script/mount_cache.sh \
    $(DEVICE_STRING)/configs/script/adb_net.sh:system/etc/script/adb_net.sh \
    $(DEVICE_STRING)/configs/script/insmod.sh:system/etc/script/insmod.sh \
    $(DEVICE_STRING)/configs/script/flashClone.sh:system/etc/script/flashClone.sh \
    $(DEVICE_STRING)/configs/script/set_eth0_mac.sh:system/etc/script/set_eth0_mac.sh \
    $(DEVICE_STRING)/configs/script/disable_mediascanner.sh:system/etc/script/disable_mediascanner.sh \
    $(DEVICE_STRING)/configs/dev.txt:system/etc/dev.txt \
    $(DEVICE_STRING)/configs/service/post-init.sh:system/etc/service/post-init.sh \
    $(DEVICE_STRING)/configs/service/config_ttyMT0.sh:system/etc/service/config_ttyMT0.sh \
    $(DEVICE_STRING)/configs/service/pre-init.sh:system/etc/service/pre-init.sh \
    $(DEVICE_STRING)/configs/mtk_bugreport.sh:system/bin/mtk_bugreport.sh \
    $(DEVICE_STRING)/configs/script/unmount.sh:root/unmount.sh \
    $(DEVICE_STRING)/configs/script/mapsanalysis.sh:system/bin/mapsanalysis.sh \
    $(DEVICE_STRING)/configs/script/mtk-memreport.sh:system/bin/mtk-memreport.sh
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.$(ANDROID_IC_SETTING).rc:root/init.$(ANDROID_IC_SETTING).rc \
    $(DEVICE_STRING)/configs/init.trace.rc:root/init.trace.rc \
    $(DEVICE_STRING)/configs/script/mount_cache.sh:system/etc/script/mount_cache.sh \
    $(DEVICE_STRING)/configs/script/adb_net.sh:system/etc/script/adb_net.sh \
    $(DEVICE_STRING)/configs/script/insmod.sh:system/etc/script/insmod.sh \
    $(DEVICE_STRING)/configs/script/flashClone.sh:system/etc/script/flashClone.sh \
    $(DEVICE_STRING)/configs/script/set_eth0_mac.sh:system/etc/script/set_eth0_mac.sh \
    $(DEVICE_STRING)/configs/script/disable_mediascanner.sh:system/etc/script/disable_mediascanner.sh \
    $(DEVICE_STRING)/configs/dev.txt:system/etc/dev.txt \
    $(DEVICE_STRING)/configs/service/post-init.sh:system/etc/service/post-init.sh \
    $(DEVICE_STRING)/configs/service/config_ttyMT0.sh:system/etc/service/config_ttyMT0.sh \
    $(DEVICE_STRING)/configs/service/pre-init.sh:system/etc/service/pre-init.sh \
    $(DEVICE_STRING)/configs/mtk_bugreport.sh:system/bin/mtk_bugreport.sh \
    $(DEVICE_STRING)/configs/script/unmount.sh:root/unmount.sh \
    $(DEVICE_STRING)/configs/script/mapsanalysis.sh:system/bin/mapsanalysis.sh \
    $(DEVICE_STRING)/configs/script/mtk-memreport.sh:system/bin/mtk-memreport.sh
endif
endif
endif
endif
endif

ifeq ($(ANDROID_TWO_WORLDS),true)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/pms_suspend_done_two_worlds.cfg:system/etc/pms_suspend_done.cfg \
    $(DEVICE_STRING)/configs/pms_whitelist_two_worlds.txt:system/etc/pms_whitelist.txt \
    $(DEVICE_STRING)/configs/service/dtv_svc_glibc.sh:system/etc/service/dtv_svc.sh
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/pms_suspend_done.cfg:system/etc/pms_suspend_done.cfg \
    $(DEVICE_STRING)/configs/pms_whitelist.txt:system/etc/pms_whitelist.txt \
    $(DEVICE_STRING)/configs/service/dtv_svc.sh:system/etc/service/dtv_svc.sh
endif
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/service/mount_data.sh:system/etc/service/mount_data.sh

ifeq ($(BOOTUP_CHK), true)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/service/bt_chk.sh:root/bt_chk.sh
endif

ifeq ($(DTVSVC_CHK), true)
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/configs/service/dtv_svc_chk.sh:system/etc/service/dtv_svc_chk.sh
endif

ifeq ($(LOCAL_BOOTUP_STRESS), true)
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/configs/service/local_bt_stress.sh:system/etc/service/local_bt_stress.sh
endif


ifeq ($(ENABLE_NET_ADB),true)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/boot_net_adb.conf:system/etc/boot.conf
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/boot.conf:system/etc/boot.conf
endif

# quicklaunch
ifeq ($(BLCR),true)
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/configs/zygote-blcr.rc:root/zygote.rc \
	$(DEVICE_STRING)/configs/quicklaunch.sh:system/bin/quicklaunch.sh \
	$(DEVICE_STRING)/DTV_OUT/blcr_imports.ko:root/blcr_imports.ko \
	$(DEVICE_STRING)/DTV_OUT/blcr.ko:root/blcr.ko

ifeq ($(RLS_CUSTOM_BUILD),true)
PRODUCT_COPY_FILES += \
	$(PREBUIT_PATH)/libcr.so:system/lib/libcr.so \
	$(PREBUIT_PATH)/libcr_omit.so:system/lib/libcr_omit.so \
	$(PREBUIT_PATH)/libcr_run.so:system/lib/libcr_run.so \
	$(PREBUIT_PATH)/cr_restart:system/bin/cr_restart \
	$(PREBUIT_PATH)/cr_checkpoint:system/bin/cr_checkpoint \
	$(PREBUIT_PATH)/camera.$(ANDROID_IC_SETTING).so:system/lib/hw/camera.$(ANDROID_IC_SETTING).so \
	$(PREBUIT_PATH)/gralloc.$(ANDROID_IC_SETTING).so:system/lib/hw/gralloc.$(ANDROID_IC_SETTING).so \
	$(PREBUIT_PATH)/audio.primary.$(ANDROID_IC_SETTING).so:system/lib/hw/audio.primary.$(ANDROID_IC_SETTING).so \
	$(PREBUIT_PATH)/hwcomposer.$(ANDROID_IC_SETTING).so:system/lib/hw/hwcomposer.$(ANDROID_IC_SETTING).so \
	$(PREBUIT_PATH)/libfr.ko:system/lib/libfr.ko \
	$(PREBUIT_PATH)/libnetmagic.so:system/lib/libnetmagic.so
endif

else
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/configs/zygote-normal.rc:root/zygote.rc
endif

# zram: ch 1031491: start
ifeq ($(ANDROID_SWITCH_FBM), true)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/setup_zram.sh:system/etc/script/setup_zram.sh \
	$(DEVICE_STRING)/configs/script/fbm_mode_1_tvmm.sh:system/etc/script/fbm_mode_1_tvmm.sh \
    $(DEVICE_STRING)/configs/script/fbm_mode_2_android.sh:system/etc/script/fbm_mode_2_android.sh
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/setup_zram_cust_1.sh:system/etc/script/setup_zram.sh
endif
# zram: ch 1031491: end

ifeq ($(USB_ADB),true)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/adb_usb.sh:system/etc/script/adb_usb.sh
# usb modules
ifneq ($(KERNEL_VER),linux-3.10)
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/DTV_OUT/musb_hdrc.ko:system/lib/modules/musb_hdrc.ko
else
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/DTV_OUT/libcomposite.ko:system/lib/modules/libcomposite.ko \
	$(DEVICE_STRING)/DTV_OUT/musb_hdrc.ko:system/lib/modules/musb_hdrc.ko
endif
endif

ifeq ($(ENABLE_BUSYBOX_INSTALL),true)
PRODUCT_COPY_FILES += $(PREBUIT_PATH)/busybox:root/sbin/busybox
endif

ifeq ($(DM_VERITY),true)
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/dmsetup:root/sbin/dmsetup
endif

# mount external storage and busybox
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/fstab.$(IC_SETTING):system/etc/fstab.$(IC_SETTING) \
    $(PREBUIT_PATH)/mount.exfat:root/sbin/mount.exfat
    
ifeq "OPENBOX" "$(TARGET_TOOLBOX)"
#PRODUCT_COPY_FILES += \
#    $(PREBUIT_PATH)/toybox:system/sbin/toybox
else
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/busybox:root/sbin/busybox
endif

# e2fsck
PRODUCT_COPY_FILES += \
	$(PREBUIT_PATH)/e2fsck_v1.42.5:root/sbin/e2fsck

# misc scripts
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/get_rootfs.sh:system/bin/get_rootfs.sh \
    $(PREBUIT_PATH)/update_rootfs.sh:system/bin/update_rootfs.sh \
    $(PREBUIT_PATH)/logv:system/bin/logv \
    $(PREBUIT_PATH)/logd:system/bin/logd \
    $(PREBUIT_PATH)/logi:system/bin/logi \
    $(PREBUIT_PATH)/logw:system/bin/logw \
    $(PREBUIT_PATH)/loge:system/bin/loge

# key input
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/qwerty.kl:system/usr/keylayout/qwerty.kl \
    $(DEVICE_STRING)/configs/mtkinp_events.kl:system/usr/keylayout/mtkinp_events.kl

ifneq ($(ANDROID_TWO_WORLDS),true)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/ttxkeymap.ini:system/usr/keylayout/ttxkeymap.ini
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/ttxkeymap_two_worlds.ini:system/usr/keylayout/ttxkeymap.ini
endif

ifneq ($(ANDROID_TWO_WORLDS),true)
# dev_svc
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/system/bin/dtv_svc:system/bin/dtv_svc \
    $(DTV_OUT_PATH)/symbols/system/bin/dtv_svc:symbols/system/bin/dtv_svc
endif

#dtv share lib
PRODUCT_COPY_FILES += \
	$(DTV_OUT_PATH)/shares_libraries/libapp_if_rpc.so:system/lib/libapp_if_rpc.so \
	$(DTV_OUT_PATH)/shares_libraries/libapp_if.so:system/lib/libapp_if.so \
	$(DTV_OUT_PATH)/shares_libraries/libcmpb.so:system/lib/libcmpb.so \
	$(DTV_OUT_PATH)/shares_libraries/libdtv_common.so:system/lib/libdtv_common.so \
	$(DTV_OUT_PATH)/shares_libraries/libdtv_osai.so:system/lib/libdtv_osai.so \
	$(DTV_OUT_PATH)/shares_libraries/libhandle_app.so:system/lib/libhandle_app.so \
	$(DTV_OUT_PATH)/shares_libraries/libmtal.so:system/lib/libmtal.so \
	$(DTV_OUT_PATH)/shares_libraries/libmw_if2.so:system/lib/libmw_if2.so \
	$(DTV_OUT_PATH)/shares_libraries/librpc_ipc.so:system/lib/librpc_ipc.so

ifeq ($(ANDROID_TWO_WORLDS),true)
ifeq ($(MTK_STREAM_RPC_SUPPORT),true)
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/shares_libraries/libmtk_stream_rpc.so:system/lib/libmtk_stream_rpc.so
endif
endif

#ifeq "$(BUILD_PHASE)" "PHASE2.0"
#PRODUCT_COPY_FILES += \
#	$(DTV_OUT_PATH)/shares_libraries/libconfig_parser.so:system/lib/libconfig_parser.so
#endif


ifeq ($(INSTALL_SYSTEM_DTV_KO),true)
    ifeq "1080p" "$(OSD_RESOLUTION)"
    PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.dtvko.1080p.rc:root/init.dtvko.rc
    else
    PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.dtvko.720p.rc:root/init.dtvko.rc
    endif
PRODUCT_COPY_FILES +=  $(DTV_OUT_PATH)/dtv_driver.ko:system/lib/modules/dtv_driver.ko
else
PRODUCT_COPY_FILES += $(DTV_OUT_PATH)/dtv_driver.ko:root/dtv_driver.ko
endif

# DTV driver ko
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/snd-mtk.ko:root/snd-mtk.ko \
    $(DTV_OUT_PATH)/mtk_mmap.ko:system/lib/modules/mtk_mmap.ko \
    $(DTV_OUT_PATH)/fuse.ko:system/lib/modules/fuse.ko \
    $(DTV_OUT_PATH)/ntfs.ko:system/lib/modules/ntfs.ko \
    $(DTV_OUT_PATH)/ntfs.ko:root/ntfs.ko

#cli
ifeq "$(NEED_CLI_MODULE)" "true"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/driver_cli.ko:system/lib/modules/driver_cli.ko
endif

# android adb support
ifeq ($(USB_ADB),true)
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/android.ko:system/lib/modules/android.ko
endif

#v4l library file
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/v4l2-common.ko:system/lib/modules/v4l2-common.ko \
    $(DTV_OUT_PATH)/v4l2-int-device.ko:system/lib/modules/v4l2-int-device.ko \
    $(DTV_OUT_PATH)/videodev.ko:system/lib/modules/videodev.ko \
    $(DTV_OUT_PATH)/uvcvideo.ko:system/lib/modules/uvcvideo.ko \
    $(DTV_OUT_PATH)/videobuf2-core.ko:system/lib/modules/videobuf2-core.ko \
    $(DTV_OUT_PATH)/videobuf2-memops.ko:system/lib/modules/videobuf2-memops.ko \
    $(DTV_OUT_PATH)/videobuf2-vmalloc.ko:system/lib/modules/videobuf2-vmalloc.ko
#$(DTV_OUT_PATH)/v4l1-compat.ko:system/lib/modules/v4l1-compat.ko

#add drv_cli for debug
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/drv_cli.sh:system/etc/drv_cli.sh

ifeq "true" "$(ANDROID_SW_RENDERING)"
else
# 3D library file
ifeq "${GPU_SETTING_VER}" "MALI600"
ifeq "debug" "$(BUILD_CFG)"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/mali_kbase.ko:system/lib/modules/mali_kbase.ko \
    $(PREBUIT_PATH)/mali-t624-debug/system/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
    $(PREBUIT_PATH)/mali-t624/system/lib/egl/libGLES_mali_KK.so:system/lib/egl/libGLES_mali.so 
else
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/mali-t624/system/lib/egl/egl.cfg:system/lib/egl/egl.cfg \
    $(PREBUIT_PATH)/mali-t624/system/lib/egl/libGLES_mali_KK.so:system/lib/egl/libGLES_mali.so 
ifneq "$(RLS_CUSTOM_BUILD)" "true"
PRODUCT_COPY_FILES += \
    $(DTV_OUT_PATH)/mali_kbase.ko:system/lib/modules/mali_kbase.ko
else
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/mali_kbase.ko:system/lib/modules/mali_kbase.ko
endif
endif
else
PRODUCT_COPY_FILES += \
		$(PREBUIT_PATH)/mali/lib/libGLES_mali_kk.so:system/lib/egl/libGLES_mali.so \
		$(PREBUIT_PATH)/mali/lib/egl.cfg:system/lib/egl/egl.cfg 
ifneq "$(RLS_CUSTOM_BUILD)" "true"
PRODUCT_COPY_FILES += \
	$(DTV_OUT_PATH)/mali.ko:system/lib/modules/mali.ko 
else
PRODUCT_COPY_FILES += \
	$(PREBUIT_PATH)/mali.ko:system/lib/modules/mali.ko
endif
endif
endif

# Wifi driver and configuration file
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    $(PREBUIT_PATH)/wifi/p2p_supplicant.conf:system/etc/wifi/p2p_supplicant.conf \
    $(PREBUIT_PATH)/wifi/rt2870.bin:system/etc/firmware/rt2870.bin \
    $(PREBUIT_PATH)/wifi/wifi_set.sh:system/bin/wifi_set.sh \
    $(PREBUIT_PATH)/wifi/iw:system/bin/iw \
    $(PREBUIT_PATH)/wifi/wpa_cli:system/bin/wpa_cli \
    $(PREBUIT_PATH)/wifi/iwpriv:system/bin/iwpriv

  
#Ralink RT5572  
ifeq "$(BUILD_CFG)" "debug"

ifeq ($(KERNEL_VER),linux-3.10)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/init_wifi_k310.sh:system/etc/script/init_wifi.sh \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/mt7662u_sta.ko:system/lib/modules/mt7662u_sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/mt7603u_sta.ko:system/lib/modules/mt7603u_sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rt5572sta.ko:system/lib/modules/rt5572sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rt5572ap.ko:system/lib/modules/rt5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rtnet5572ap.ko:system/lib/modules/rtnet5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rtutil5572ap.ko:system/lib/modules/rtutil5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/mt7601Usta.ko:system/lib/modules/mt7601Usta.ko \
    $(PREBUIT_PATH)/wifi/RT2870STA.dat:system/etc/Wireless/RT2870STA/RT2870STA.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7603.dat:system/etc/Wireless/RT2870STA/RT2870STA_7603.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_76x2.dat:system/etc/Wireless/RT2870STA/RT2870STA_76x2.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7601.dat:system/etc/Wireless/RT2870STA/RT2870STA_7601.dat \
    $(PREBUIT_PATH)/wifi/RT2870AP.dat:system/etc/Wireless/RT2870AP/RT2870AP.dat \
    $(PREBUIT_PATH)/wifi/SingleSKU.dat:system/etc/Wireless/RT2870STA/SingleSKU.dat
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/init_wifi.sh:system/etc/script/init_wifi.sh \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rt5572sta.ko:system/lib/modules/rt5572sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/mt7662u_sta.ko:system/lib/modules/mt7662u_sta.ko \
    $(PREBUIT_PATH)/wifi/RT2870STA.dat:system/etc/Wireless/RT2870STA/RT2870STA.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7603.dat:system/etc/Wireless/RT2870STA/RT2870STA_7603.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_76x2.dat:system/etc/Wireless/RT2870STA/RT2870STA_76x2.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7601.dat:system/etc/Wireless/RT2870STA/RT2870STA_7601.dat \
    $(PREBUIT_PATH)/wifi/SingleSKU.dat:system/etc/Wireless/RT2870STA/SingleSKU.dat \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rt5572ap.ko:system/lib/modules/rt5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rtnet5572ap.ko:system/lib/modules/rtnet5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/rtutil5572ap.ko:system/lib/modules/rtutil5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/mt7601Uap.ko:system/lib/modules/mt7601Uap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/dbg/mt7601Usta.ko:system/lib/modules/mt7601Usta.ko \
    $(PREBUIT_PATH)/wifi/RT2870AP.dat:system/etc/Wireless/RT2870AP/RT2870AP.dat
endif

else #ifeq "$(BUILD_CFG)" "debug"

ifeq ($(KERNEL_VER),linux-3.10)
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/init_wifi_k310.sh:system/etc/script/init_wifi.sh \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/mt7662u_sta.ko:system/lib/modules/mt7662u_sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/mt7603u_sta.ko:system/lib/modules/mt7603u_sta.ko \
    $(PREBUIT_PATH)/wifi/RT2870STA.dat:system/etc/Wireless/RT2870STA/RT2870STA.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7603.dat:system/etc/Wireless/RT2870STA/RT2870STA_7603.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_76x2.dat:system/etc/Wireless/RT2870STA/RT2870STA_76x2.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7601.dat:system/etc/Wireless/RT2870STA/RT2870STA_7601.dat \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rt5572sta.ko:system/lib/modules/rt5572sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rt5572ap.ko:system/lib/modules/rt5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rtnet5572ap.ko:system/lib/modules/rtnet5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rtutil5572ap.ko:system/lib/modules/rtutil5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/mt7601Usta.ko:system/lib/modules/mt7601Usta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/8188eu.ko:system/lib/modules/8188eu.ko \
    $(PREBUIT_PATH)/wifi/RT2870AP.dat:system/etc/Wireless/RT2870AP/RT2870AP.dat \
    $(PREBUIT_PATH)/wifi/SingleSKU.dat:system/etc/Wireless/RT2870STA/SingleSKU.dat
else
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/script/init_wifi.sh:system/etc/script/init_wifi.sh \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rt5572sta.ko:system/lib/modules/rt5572sta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/mt7662u_sta.ko:system/lib/modules/mt7662u_sta.ko \
    $(PREBUIT_PATH)/wifi/SingleSKU.dat:system/etc/Wireless/RT2870STA/SingleSKU.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA.dat:system/etc/Wireless/RT2870STA/RT2870STA.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7603.dat:system/etc/Wireless/RT2870STA/RT2870STA_7603.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_76x2.dat:system/etc/Wireless/RT2870STA/RT2870STA_76x2.dat \
    $(PREBUIT_PATH)/wifi/RT2870STA_7601.dat:system/etc/Wireless/RT2870STA/RT2870STA_7601.dat \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rt5572ap.ko:system/lib/modules/rt5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rtnet5572ap.ko:system/lib/modules/rtnet5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/rtutil5572ap.ko:system/lib/modules/rtutil5572ap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/mt7601Uap.ko:system/lib/modules/mt7601Uap.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/mt7601Usta.ko:system/lib/modules/mt7601Usta.ko \
    $(PREBUIT_PATH)/wifi/$(ANDROID_IC_SETTING)/8188eu.ko:system/lib/modules/8188eu.ko \
    $(PREBUIT_PATH)/wifi/RT2870AP.dat:system/etc/Wireless/RT2870AP/RT2870AP.dat
endif

endif #ifeq "$(BUILD_CFG)" "debug"

PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0




# Flash ko file, JB does not support flash, remove it from target
#PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/f11_plugin/libflashplayer.so:/system/lib/plugins/com.adobe.flashplayer/libflashplayer.so \
    $(PREBUIT_PATH)/f11_plugin/libstagefright_froyo.so:/system/lib/plugins/com.adobe.flashplayer/libstagefright_froyo.so \
    $(PREBUIT_PATH)/f11_plugin/libstagefright_honeycomb.so:/system/lib/plugins/com.adobe.flashplayer/libstagefright_honeycomb.so \
    $(PREBUIT_PATH)/f11_plugin/libysshared.so:/system/lib/plugins/com.adobe.flashplayer/libysshared.so

#3D wallpapers
ifneq ($(TARGET_BUILD_PDK), true)
PRODUCT_COPY_FILES += \
 packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml
endif

# Add hardware additional xml
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.flash-autofocus.xml:system/etc/permissions/android.hardware.camera.flash-autofocus.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.faketouch.xml:system/etc/permissions/android.hardware.faketouch.xml

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
# media codec config xml file
PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/media_codecs.xml:system/etc/media_codecs.xml \
    $(DEVICE_STRING)/configs/media_profiles.xml:system/etc/media_profiles.xml
endif

# Camera FR
#PRODUCT_COPY_FILES += \
    out/target/product/$(ANDROID_IC_SETTING)/system/lib/libfr.so:/system/lib/libfr.ko

ifneq ($(TARGET_BUILD_PDK), true)
#capture screen
PRODUCT_COPY_FILES += \
	frameworks/base/cmds/capture/capture:/system/bin/capture
endif

# aac enc library
ifeq "$(AUD_SUPPORT_ENC)" "true"
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/aacenc/libaacenc.so:system/lib/libaacenc.so
endif

# List of apps and optional libraries (Java and native) to put in the add-on system image.
ifeq ($(TCL_PRODUCT_BRANCH),EM)
PRODUCT_PACKAGES := \
    com.mediatek.common.capture \
    com.mediatek.common.PhotoPlayer \
    com.mediatek.mmp \
    com.mediatek.wfdsink \
    libwfdclientjni \
    libmtkcapture \
    libMtkPhotoPlayer \
    libmediaplayer_mtk \
    capture \
    WFDSinkTest \
    MMPOnly
else
PRODUCT_PACKAGES := \
    com.mediatek.common.capture \
    com.mediatek.common.PhotoPlayer \
    com.mediatek.mmp \
    com.mediatek.wfdsink \
    libwfdclientjni \
    libmtkcapture \
    libMtkPhotoPlayer \
    libmediaplayer_mtk \
    capture \
    MMPOnly    
endif
    
# ifeq "$(TMS_SUPPORT)" "true"
PRODUCT_PACKAGES += \
    com.mediatek.util
# endif

PRODUCT_PACKAGES += \
	com.tcl.factory \
        pre_factory \
	com.tcl.factory.xml \
	com.tcl.tvmanager \
	pre_tvmanager \
	com.tcl.tvmanager.xml \
	com.tcl.snapshot \
	pre_snapshot \
	com.tcl.snapshot.xml

ifeq "$(ANDROID_TWO_WORLDS)" "true"
PRODUCT_PACKAGES += \
    libdevice_manager \
    com.mediatek.twoworlds.tv \
    libcom_mediatek_twoworlds_tv_jni \
    mtktvapi_highlevel \
    mtktvapi_agent \
	com.mediatek.network \
	libcom_mediatek_net_jni	
endif

PRODUCT_PACKAGES += mknod_util
PRODUCT_PACKAGES += limited_wait

PRODUCT_PACKAGES += \
    com.mediatek.common.audiocapture \
    com.mediatek.dm \
    AudioCaptureDemo
    
PRODUCT_PACKAGES += \
    com.mediatek.common.powercontrol \
    libmtkpowercontrol

ifeq "${SUPPORT_MTK_ANDROID_RM}" "true"
PRODUCT_PACKAGES += \
    MtkRmServer \
    libMtkRmClient
PRODUCT_COPY_FILES += \
	$(DEVICE_STRING)/configs/init.mtk_rm.rc:root/init.mtk_rm.rc \
    $(DEVICE_STRING)/configs/service/mtk_rm.sh:system/etc/service/mtk_rm.sh
endif

PRODUCT_COPY_FILES += \
    $(DEVICE_STRING)/configs/init.sdcard.rc:root/init.sdcard.rc
    
PRODUCT_PACKAGES += \
    com.extra.eventflow \
    com.mediatek.eventflow

ifneq "$(HYBRID_MODE)" "true"
ifneq "$(ANDROID_TWO_WORLDS)" "true"
#PRODUCT_PACKAGES += \
#	com.mediatek.tv.custom \
#	com.mediatek.tv \
#	com.mediatek.tvcm \
#	libcom_mediatek_tv_jni \
#	com.mediatek.record \
#	librecord_manager
endif
endif

# force su to install to debug
ifeq ($(ENABLE_SU_INSTALL),true)
PRODUCT_PACKAGES += \
    su
endif

# enable wifi tool
ifeq ($(WIFI_TOOL_NEEDED), true)
PRODUCT_PACKAGES += \
	wpa_cli
endif

# force procrank to install to do memory check
ifeq ($(ENABLE_PROCRANK_INSTALL),true)
PRODUCT_PACKAGES += \
    procrank
endif

ifeq ($(AUTO_TEST),true)
PRODUCT_PACKAGES   += k2logcat
endif

# name of the add-on
#PRODUCT_SDK_ADDON_NAME := mtk_tv_addon

# Copy the manifest and hardware files for the SDK add-on.
#PRODUCT_SDK_ADDON_COPY_FILES := \
    $(DEVICE_STRING)/tv_addon/sdk_addon/manifest.ini:manifest.ini

# Copy the jar files for the optional libraries that are exposed as APIs.
#PRODUCT_SDK_ADDON_COPY_MODULES := \
    com.mediatek.tv.custom:libs/com.mediatek.tv.custom.jar \
    com.mediatek.tv:libs/com.mediatek.tv.jar \
    com.mediatek.common.audiocapture:libs/com.mediatek.common.audiocapture.jar \
    com.mediatek.record:libs/com.mediatek.record.jar \

# Name of the doc to generate and put in the add-on. This must match the name defined
# in the optional library with the tag
#    LOCAL_MODULE:= platform_library
# in the documentation section.
PRODUCT_SDK_ADDON_DOC_MODULES := tv
$(call inherit-product, $(SRC_TARGET_DIR)/product/full.mk)
#PRODUCT_PACKAGES += TVService

PRODUCT_PACKAGES += \
        ubinize \
        ubiattach \
        ubidetach \
        mkfs.ubifs

PRODUCT_PACKAGES += \
        audio.primary.$(ANDROID_IC_SETTING) \
        audio.usb.default \
        camera.$(ANDROID_IC_SETTING) \
        cli \
        cli_shell \
        libdtv_getline

ifeq "true" "$(SYS_DRIVER_ONLY)"
PRODUCT_PACKAGES += drv_init
endif

ifeq "true" "$(ANDROID_SW_RENDERING)"
PRODUCT_PACKAGES += \
        gralloc.default
else
PRODUCT_PACKAGES += \
        gralloc.$(ANDROID_IC_SETTING)
endif

# audio_hal load setting
PRODUCT_COPY_FILES += $(TOP)/../mediatek/device/audio/audio_policy.conf:system/etc/audio_policy.conf

# jcifs
PRODUCT_COPY_FILES += $(TOP)/../mediatek/external/jcifs/jcifs.xml:system/etc/permissions/jcifs.xml

PRODUCT_PACKAGES += \
        hwcomposer.$(ANDROID_IC_SETTING)

PRODUCT_PACKAGES += \
        setmac

PRODUCT_PACKAGES += \
        ttyman

ifeq "$(MSI)" "true"
PRODUCT_PACKAGES += \
        multi_screen_inter
endif

PRODUCT_PACKAGES += \
        libvsstexch \
		libcapture \
		libpicapi

PRODUCT_PACKAGES += \
        PinyinIME \
        libjni_pinyinime

PRODUCT_PACKAGES += \
        mkbootimg \
        mkbootfs \
        split_bootimg

PRODUCT_PACKAGES += \
        avmute.bin

PRODUCT_PACKAGES += \
        switch_fbm

#JB does not support flash, remove it from target
#PRODUCT_PACKAGES += \
        oem_install_flash_player_ics

PRODUCT_PACKAGES += \
        pppoe \
        pppoedial

ifneq "$(HYBRID_MODE)" "true"
PRODUCT_PACKAGES += \
        com.mediatek.dlna \
        libdlnadmp
endif

PRODUCT_PACKAGES += \
        jcifs

PRODUCT_PACKAGES += \
        libmtkh264dec.plugin \
        libmtkh264enc.plugin

PRODUCT_PACKAGES += \
        android.policy_tv

PRODUCT_PACKAGES += \
        ntfs-3g \
        libntfs-3g \
        libfuse

SECURITY_DIR := ../../dtv_linux/mtk_util/source/securestorage/tee/
ifneq ($(shell find $(SECURITY_DIR) -name Android.mk),)
PRODUCT_PACKAGES += \
        libsecurestorage
endif

PRODUCT_PACKAGES += \
        libmms

#early TV playback
PRODUCT_PACKAGES += \
    selectchannel \
    dtv_bootanim


# MTK_TEST: this should be unmarked later
#PRODUCT_PACKAGES += \
#	libnetmagic

#host tool
PRODUCT_PACKAGES += \
        lzop \
        fixup_fs_config_util

PRODUCT_PACKAGES += libmtkaudiocapture

ifeq "OPENBOX" "$(TARGET_TOOLBOX)"
PRODUCT_PACKAGES += \
        awk \
        grep \
        modprobe \
        blkid \
        mount \
        umount \
        losetup
endif

ifeq ($(BLCR),true)
PRODUCT_PACKAGES += \
        libcr_omit \
        libcr_run \
        libcr \
        cr_checkpoint \
        cr_restart
endif

ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
PRODUCT_PACKAGES += \
	libvdecomx \
	libvdpomx.$(ANDROID_IC_SETTING) \
	libadecomx.$(ANDROID_IC_SETTING) \
	libMtkOmxAacDec \
	libMtkOmxAacEnc \
	libMtkOmxAMRNBDec \
	libMtkOmxAMRWBDec \
	libMtkOmxAmrEnc  \
	libMtkOmxApeDec \
	libMtkOmxAwbEnc \
	libMtkOmxCookDec \
	libMtkOmxFlacDec \
	libMtkOmxG711Dec \
	libMtkOmxMp3Dec \
	libMtkOmxVorbisDec \
	libMtkOmxVorbisEnc \
	libMtkOmxWmaDec \
	libMtkOmxCore \
	libMtkOmxVdec \
	libMtkOmxVenc \
	libasfextractor \
	libflvextractor \
	libstagefrighthw \
	libMtkOmxAdpcmDec \
	libMtkOmxDspAACDec \
	libMtkOmxDspMp3Dec \
	libMtkOmxDspAc3Dec \
	libMtkOmxDspWmaDec \
	libMtkOmxDspMp1Dec \
	libMtkOmxDspDtsDec \
	libMtkOmxDspFlacDec \
	libMtkOmxDspLpcmDec \
	libMtkOmxDspAdpcmDec \
	libMtkOmxDspVorbisDec \
	libMtkOmxDspPcmDec \
	libMtkOmxDspAmrNBDec \
	libMtkOmxDspAmrWBDec
endif

ifeq ($(ENABLE_VALGRIND),true)
PRODUCT_PACKAGES += \
        cachegrind-arm-linux \
        callgrind-arm-linux \
        default.supp \
        drd-arm-linux \
        helgrind-arm-linux \
        massif-arm-linux \
        memcheck-arm-linux \
        none-arm-linux \
        valgrind \
        vgpreload_core-arm-linux \
        vgpreload_drd-arm-linux \
        vgpreload_helgrind-arm-linux \
        vgpreload_massif-arm-linux \
        vgpreload_memcheck-arm-linux
endif

PRODUCT_PACKAGES += \
		libion
# Discard inherited values and use our own instead.
PRODUCT_MANUFACTURER := MTK
PRODUCT_NAME := $(BRAND)_$(ANDROID_IC_SETTING)
PRODUCT_DEVICE := $(ANDROID_IC_SETTING)
PRODUCT_MODEL := Generic Android on $(ANDROID_IC_SETTING)
PRODUCT_POLICY := android.policy_tv
TARGET_BOOTLOADER_BOARD_NAME := $(ANDROID_IC_SETTING)
PRODUCT_BRAND := Android

# opengl version info
ifeq "$(ANDROID_IC_SETTING)" "mt5890"
	PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version=196608
else
	PRODUCT_PROPERTY_OVERRIDES += ro.opengles.version=131072
endif

ifeq "1080p" "$(OSD_RESOLUTION)"
# lcd density
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=240
ifeq "true" "$(SUPPORT_4K2K)"
	ifeq "true" "$(CTS_TEST)"
	PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=128m
	else
	PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=192m
	endif
else
	PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=128m
endif
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.dtv_driver.1080p.rc:root/init.dtv_driver.rc
else
# lcd density default is 160
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=160
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.heapsize=48m
PRODUCT_COPY_FILES += $(DEVICE_STRING)/configs/init.dtv_driver.720p.rc:root/init.dtv_driver.rc
endif

ifeq "$(RLS_CUSTOM_BUILD)" "true"
   
    PRODUCT_COPY_FILES += \
	$(PREBUIT_PATH)/libliveMedia.so:system/lib/libliveMedia.so \
	$(PREBUIT_PATH)/libvospplayer.so:system/lib/libvospplayer.so \
$(PREBUIT_PATH)/camera.${ANDROID_IC_SETTING}.so:system/lib/hw/camera.${ANDROID_IC_SETTING}.so \
$(PREBUIT_PATH)/gralloc.${ANDROID_IC_SETTING}.so:system/lib/hw/gralloc.${ANDROID_IC_SETTING}.so \
$(PREBUIT_PATH)/libMtkOmxDspAc3Dec.so:system/lib/libMtkOmxDspAc3Dec.so \
$(PREBUIT_PATH)/libMtkOmxDspFlacDec.so:system/lib/libMtkOmxDspFlacDec.so \
$(PREBUIT_PATH)/libMtkOmxDspAmrNBDec.so:system/lib/libMtkOmxDspAmrNBDec.so \
$(PREBUIT_PATH)/libadecomx.${ANDROID_IC_SETTING}.so:system/lib/libadecomx.${ANDROID_IC_SETTING}.so \
$(PREBUIT_PATH)/libdrmdecrypt.so:system/lib/libdrmdecrypt.so \
$(PREBUIT_PATH)/libbinder.hwcomposer.${ANDROID_IC_SETTING}.so:system/lib/libbinder.hwcomposer.${ANDROID_IC_SETTING}.so \
$(PREBUIT_PATH)/libMtkOmxDspVorbisDec.so:system/lib/libMtkOmxDspVorbisDec.so \
$(PREBUIT_PATH)/libMtkOmxDspMp1Dec.so:system/lib/libMtkOmxDspMp1Dec.so \
$(PREBUIT_PATH)/libMtkOmxDspAmrWBDec.so:system/lib/libMtkOmxDspAmrWBDec.so \
$(PREBUIT_PATH)/libMtkOmxDspMp3Dec.so:system/lib/libMtkOmxDspMp3Dec.so \
$(PREBUIT_PATH)/libMtkOmxDspAdpcmDec.so:system/lib/libMtkOmxDspAdpcmDec.so \
$(PREBUIT_PATH)/libMtkOmxDspPcmDec.so:system/lib/libMtkOmxDspPcmDec.so \
$(PREBUIT_PATH)/libMtkOmxDspDtsDec.so:system/lib/libMtkOmxDspDtsDec.so \
$(PREBUIT_PATH)/libMtkOmxDspLpcmDec.so:system/lib/libMtkOmxDspLpcmDec.so \
$(PREBUIT_PATH)/libMtkOmxDspWmaDec.so:system/lib/libMtkOmxDspWmaDec.so \
$(PREBUIT_PATH)/com.google.widevine.software.drm.xml:system/etc/permissions/com.google.widevine.software.drm.xml \
$(PREBUIT_PATH)/limited_wait:system/bin/limited_wait \
$(PREBUIT_PATH)/com.google.widevine.software.drm.odex:system/framework/com.google.widevine.software.drm.odex \
$(PREBUIT_PATH)/com.mediatek.util.odex:system/framework/com.mediatek.util.odex \
$(PREBUIT_PATH)/libwvm.so:system/vendor/lib/libwvm.so \
$(PREBUIT_PATH)/libwvdrmengine.so:system/vendor/lib/mediadrm/libwvdrmengine.so \
$(PREBUIT_PATH)/libdrmwvmplugin.so:system/vendor/lib/drm/libdrmwvmplugin.so \
$(PREBUIT_PATH)/libWVStreamControlAPI_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL).so:system/vendor/lib/libWVStreamControlAPI_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL).so \
$(PREBUIT_PATH)/libwvdrm_L3.so:system/vendor/lib/libwvdrm_L3.so
	
ifeq "$(SUPPORT_TV_STAGEFRIGHT)" "true"
	PRODUCT_COPY_FILES += \
	$(PREBUIT_PATH)/mtk_omx_core.cfg:system/etc/mtk_omx_core.cfg \
  $(PREBUIT_PATH)/libcok.so:system/lib/libcok.so \
	$(PREBUIT_PATH)/libvo_aacenc.so:system/lib/libvo_aacenc.so \
	$(PREBUIT_PATH)/libvencomx.${ANDROID_IC_SETTING}.so:system/lib/libvencomx.${ANDROID_IC_SETTING}.so \
  $(PREBUIT_PATH)/libMtkOmxVorbisEnc.so:system/lib/libMtkOmxVorbisEnc.so \
  $(PREBUIT_PATH)/libMtkOmxAMRWBDec.so:system/lib/libMtkOmxAMRWBDec.so \
  $(PREBUIT_PATH)/libMtkOmxAacDec.so:system/lib/libMtkOmxAacDec.so \
  $(PREBUIT_PATH)/libMtkOmxAmrEnc.so:system/lib/libMtkOmxAmrEnc.so \
  $(PREBUIT_PATH)/libMtkOmxApeDec.so:system/lib/libMtkOmxApeDec.so \
  $(PREBUIT_PATH)/libMtkOmxAwbEnc.so:system/lib/libMtkOmxAwbEnc.so \
  $(PREBUIT_PATH)/libMtkOmxCookDec.so:system/lib/libMtkOmxCookDec.so \
  $(PREBUIT_PATH)/libMtkOmxCore.so:system/lib/libMtkOmxCore.so \
  $(PREBUIT_PATH)/libMtkOmxFlacDec.so:system/lib/libMtkOmxFlacDec.so \
  $(PREBUIT_PATH)/libMtkOmxG711Dec.so:system/lib/libMtkOmxG711Dec.so \
  $(PREBUIT_PATH)/libMtkOmxAMRNBDec.so:system/lib/libMtkOmxAMRNBDec.so \
  $(PREBUIT_PATH)/libMtkOmxVdec.so:system/lib/libMtkOmxVdec.so \
  $(PREBUIT_PATH)/libMtkOmxMp3Dec.so:system/lib/libMtkOmxMp3Dec.so \
  $(PREBUIT_PATH)/libMtkOmxWmaDec.so:system/lib/libMtkOmxWmaDec.so \
  $(PREBUIT_PATH)/libMtkOmxVorbisDec.so:system/lib/libMtkOmxVorbisDec.so \
  $(PREBUIT_PATH)/libMtkOmxAacEnc.so:system/lib/libMtkOmxAacEnc.so \
  $(PREBUIT_PATH)/libMtkOmxVenc.so:system/lib/libMtkOmxVenc.so \
  $(PREBUIT_PATH)/libMtkOmxDspAACDec.so:system/lib/libMtkOmxDspAACDec.so \
  $(PREBUIT_PATH)/libstagefrighthw.so:system/lib/libstagefrighthw.so \
  $(PREBUIT_PATH)/libflvextractor.so:system/lib/libflvextractor.so \
  $(PREBUIT_PATH)/libflv.so:system/lib/libflv.so \
  $(PREBUIT_PATH)/libasf.so:system/lib/libasf.so \
  $(PREBUIT_PATH)/libasfextractor.so:system/lib/libasfextractor.so
endif

ifeq "$(HEVC_SUPPORT)" "true"
  PRODUCT_COPY_FILES += ${TOP}/out/target/product/$(IC_SETTING)/symbols/system/lib/libstagefright_soft_h265dec.so:system/lib/libstagefright_soft_h265dec.so
endif

	PRODUCT_COPY_FILES += \
  $(PREBUIT_PATH)/libnetmagic.so:system/lib/libnetmagic.so \
  $(PREBUIT_PATH)/libvdpomx.$(ANDROID_IC_SETTING).so:system/lib/libvdpomx.$(ANDROID_IC_SETTING).so \
  $(PREBUIT_PATH)/hwcomposer.$(ANDROID_IC_SETTING).so:system/lib/hw/hwcomposer.$(ANDROID_IC_SETTING).so \
  $(PREBUIT_PATH)/libvdecomx.so:system/lib/libvdecomx.so \
  $(PREBUIT_PATH)/librecord_manager.so:system/lib/librecord_manager.so \
  $(PREBUIT_PATH)/MtkRmServer:system/bin/MtkRmServer \
  $(PREBUIT_PATH)/com.mediatek.record.odex:system/framework/com.mediatek.record.odex \
  $(PREBUIT_PATH)/libdlna_dms.so:system/lib/libdlna_dms.so \
  $(PREBUIT_PATH)/dms_srs.xml:system/etc/dlna/dmscfg/xml/dms_srs.xml \
  $(PREBUIT_PATH)/dms_upload.xml:system/etc/dlna/dmscfg/xml/dms_upload.xml \
  $(PREBUIT_PATH)/dms.xml:system/etc/dlna/dmscfg/xml/dms.xml \
  $(PREBUIT_PATH)/DMS-large.jpg:system/etc/dlna/dmscfg/xml/icon/DMS-large.jpg \
  $(PREBUIT_PATH)/DMS-large.png:system/etc/dlna/dmscfg/xml/icon/DMS-large.png \
  $(PREBUIT_PATH)/DMS-small.png:system/etc/dlna/dmscfg/xml/icon/DMS-small.png \
  $(PREBUIT_PATH)/DMS-small.jpg:system/etc/dlna/dmscfg/xml/icon/DMS-small.jpg \
  $(PREBUIT_PATH)/dms_cms.xml:system/etc/dlna/dmscfg/xml/dms_cms.xml \
  $(PREBUIT_PATH)/dms_cds.xml:system/etc/dlna/dmscfg/xml/dms_cds.xml \
  $(PREBUIT_PATH)/dms_cds_upload.xml:system/etc/dlna/dmscfg/xml/dms_cds_upload.xml \
  $(PREBUIT_PATH)/camera.$(ANDROID_IC_SETTING).so:system/lib/camera.$(ANDROID_IC_SETTING).so


ifeq ($(TCL_PRODUCT_BRANCH),CN_TEN)
PRODUCT_COPY_FILES += \
	$(PREBUIT_PATH)/dtv_bootanim_new:system/bin/dtv_bootanim \
	$(PREBUIT_PATH)/mknod_util:system/bin/mknod_util
else ifeq ($(TCL_PRODUCT_BRANCH),CN_C1)
PRODUCT_COPY_FILES += \
  $(PREBUIT_PATH)/dtv_bootanim_new:system/bin/dtv_bootanim \
  $(PREBUIT_PATH)/mknod_util:system/bin/mknod_util
else
PRODUCT_COPY_FILES += \
  $(PREBUIT_PATH)/dtv_bootanim_new:system/bin/dtv_bootanim \
  $(PREBUIT_PATH)/mknod_util:system/bin/mknod_util
endif
endif

#move from android/mediatek/device/uploadservice/uploadservice/Android.mk
PRODUCT_COPY_FILES += \
    out/target/product/$(IC_SETTING)/symbols/system/bin/uploadservice:system/bin/uploadservice

#move from android/mediatek/external/jcifs/Android.mk
PRODUCT_COPY_FILES += \
    ${TOP}/../mediatek/external/jcifs/jcifs.xml:system/etc/permissions/jcifs.xml

#move from android/mediatek/$(ANDROID_VERSION)/protect-bsp/frameworks/av/media/libstagefright/stagefright_extension/flv/Android.mk
PRODUCT_PACKAGE += libflvextractor

#move from android/mediatek/$(ANDROID_VERSION)/protect-bsp/frameworks/av/media/libstagefright/stagefright_extension/asf/Android.mk
PRODUCT_PACKAGE += libasfextractor

#move from android/mediatek/$(ANDROID_VERSION)/protect-bsp/hardware/omx/core/Android.mk
PRODUCT_COPY_FILES += \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/protect-bsp/hardware/omx/core/mtk_omx_core.cfg:system/etc/mtk_omx_core.cfg
    
#move from android/dtv-android
# tv_addon/framework/record/Android.mk 
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/record/com.mediatek.record.xml:system/etc/permissions/com.mediatek.record.xml

# tv_addon/framework/dlna/Android.mk
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/dlna/com.mediatek.dlna.xml:system/etc/permissions/com.mediatek.dlna.xml

# tv_addon/framework/dm/Android.mk
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/dm/com.mediatek.dm.xml:system/etc/permissions/com.mediatek.dm.xml

# tv_addon/framework/tv/Android.mk
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/tv/com.mediatek.tv.xml:system/etc/permissions/com.mediatek.tv.xml

# tv_addon/framework/MediaPlayer/Android.mk
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/MediaPlayer/com.mediatek.mmp.xml:system/etc/permissions/com.mediatek.mmp.xml

# vm_linux/android/config/kk.mk
SECURITY_DIR := ${TOP}/../../dtv_linux/mtk_util/source/securestorage/tee/

ifeq ($(shell find $(SECURITY_DIR) -name Android.mk),)
PRODUCT_COPY_FILES += \
    $(PREBUIT_PATH)/lib/libsecurestorage.so:system/lib/libsecurestorage.so
endif


# tv_addon/framework/tvmCustom/Android.mk
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/tvmCustom/com.mediatek.tv.custom.xml:system/etc/permissions/com.mediatek.tv.custom.xml


# tv_addon/framework/common/AudioCapture/Android.mk
#PRODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/common/AudioCapture/com.mediatek.common.audiocapture.xml:system/etc/permissions/com.mediatek.common.audiocapture.xml

# tv_addon/framework/common/powercontrol/Android.mk
#RODUCT_COPY_FILES += \
    ${TOP}/../dtv-android/tv_addon/framework/common/powercontrol/com.mediatek.common.powercontrol.xml:system/etc/permissions/com.mediatek.common.powercontrol.xml

ifeq ($(ANDROID_TWO_WORLDS),true)
# android/mediatek/frameworks/twoworlds/tv/Android.mk
PRODUCT_COPY_FILES += \
    ${TOP}/../mediatek/frameworks/twoworlds/tv/com.mediatek.twoworlds.tv.xml:system/etc/permissions/com.mediatek.twoworlds.tv.xml
endif

# android/mediatek/frameworks/EventDispatch/Android.mk
PRODUCT_COPY_FILES += \
    ${TOP}/../mediatek/frameworks/EventDispatch/com.extra.eventflow.xml:system/etc/permissions/com.extra.eventflow.xml \
    ${TOP}/../mediatek/frameworks/EventDispatch/com.mediatek.eventflow.xml:system/etc/permissions/com.mediatek.eventflow.xml

ifeq "$(RLS_CUSTOM_BUILD)" "true"
	PRODUCT_COPY_FILES += \
  $(PREBUIT_PATH)/libnetmagic.so:system/lib/libnetmagic.so \
  $(PREBUIT_PATH)/libhdcp2x.so:system/lib/libhdcp2x.so \
  $(PREBUIT_PATH)/libwfd.so:system/lib/libwfd.so \
  $(PREBUIT_PATH)/libwfdclientjni.so:system/lib/libwfdclientjni.so \
  $(PREBUIT_PATH)/com.mediatek.record.xml:system/etc/permissions/com.mediatek.record.xml
endif


TOP_OUT = $(TOP)/out
ifdef OUT_DIR
TOP_OUT := $(OUT_DIR)
endif

# android/mediatek/$(ANDROID_VERSION)/protect-bsp/frameworks/av/media/libstagefright/stagefright_extension/flv/Android.mk
PRODUCT_COPY_FILES += \
	$(TOP_OUT)/target/product/$(IC_SETTING)/symbols/system/lib/libflvextractor.so:system/lib/libflvextractor.so

# android/mediatek/$(ANDROID_VERSION)/protect-bsp/frameworks/av/media/libstagefright/stagefright_extension/asf/Android.mk
PRODUCT_COPY_FILES += \
	$(TOP_OUT)/target/product/$(IC_SETTING)/symbols/system/lib/libasfextractor.so:system/lib/libasfextractor.so

# android/mediatek/$(ANDROID_VERSION)/frameworks/common/powercontrol/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/common/powercontrol/com.mediatek.common.powercontrol.xml:system/etc/permissions/com.mediatek.common.powercontrol.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/common/AudioCapture/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/common/AudioCapture/com.mediatek.common.audiocapture.xml:system/etc/permissions/com.mediatek.common.audiocapture.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/common/photoplayer/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/common/photoplayer/com.mediatek.common.PhotoPlayer.xml:system/etc/permissions/com.mediatek.common.PhotoPlayer.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/common/capture/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/common/capture/com.mediatek.common.capture.xml:system/etc/permissions/com.mediatek.common.capture.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/tvmCustom/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/tvmCustom/com.mediatek.tv.custom.xml:system/etc/permissions/com.mediatek.tv.custom.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/MediaPlayer/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/MediaPlayer/com.mediatek.mmp.xml:system/etc/permissions/com.mediatek.mmp.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/tv/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/tv/com.mediatek.tv.xml:system/etc/permissions/com.mediatek.tv.xml

# android/$(ANDROID_VERSION)/device/dev/apps/common
PRODUCT_COPY_FILES += \
    $(TOP)/../$(ANDROID_VERSION)/device/dev/apps/common/com.mediatek.tvcm.xml:system/etc/permissions/com.mediatek.tvcm.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/dm/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/dm/com.mediatek.dm.xml:system/etc/permissions/com.mediatek.dm.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/WfdSink/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/WfdSink/com.mediatek.wfdsink.xml:system/etc/permissions/com.mediatek.wfdsink.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/dlna/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/dlna/com.mediatek.dlna.xml:system/etc/permissions/com.mediatek.dlna.xml

# android/mediatek/$(ANDROID_VERSION)/frameworks/record/Android.mk
PRODUCT_COPY_FILES += \
    $(TOP)/../mediatek/$(ANDROID_VERSION)/frameworks/record/com.mediatek.record.xml:system/etc/permissions/com.mediatek.record.xml

# android/mediatek/device/uploadservice/uploadservice/Android.mk
PRODUCT_COPY_FILES += \
	$(TOP_OUT)/target/product/$(IC_SETTING)/symbols/system/bin/uploadservice:system/bin/uploadservice
	   
# android/mediatek/device/bt_cli/Android.mk
ifneq ($(RLS_CUSTOM_BUILD),true)
PRODUCT_COPY_FILES += \
	${TOP}/out/target/product/$(IC_SETTING)/symbols/system/bin/bt_cli:system/bin/bt_cli
else
PRODUCT_COPY_FILES += \
  $(PREBUIT_PATH)/bt_cli:system/bin/bt_cli
endif

# Visual StrictMode indicator
PRODUCT_PROPERTY_OVERRIDES += persist.sys.strictmode.visual=0
PRODUCT_PROPERTY_OVERRIDES += persist.sys.strictmode.disable=1

# turn off dalvik vm warning abort in release version
ifeq "$(BUILD_CFG)" "rel"
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.jniopts=warnonly
endif

ifeq "$(SUSPEND_TO_DRAM)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.pm.warmboot=1
PRODUCT_PROPERTY_OVERRIDES += sys.pm.warmboot=1
else
PRODUCT_PROPERTY_OVERRIDES += ro.pm.warmboot=0
PRODUCT_PROPERTY_OVERRIDES += sys.pm.warmboot=0
endif

PRODUCT_PROPERTY_OVERRIDES += sys.vold.autoflag= 1

#add by zhaojun@tcl.com for mediascanner
PRODUCT_PROPERTY_OVERRIDES += media.scan.bulkinsertenable=true
PRODUCT_PROPERTY_OVERRIDES += media.scan.bulkinsertbuffersize=50
PRODUCT_PROPERTY_OVERRIDES += media.scan.bulkaudiobuffersize=5
PRODUCT_PROPERTY_OVERRIDES += media.scan.count=100000
PRODUCT_PROPERTY_OVERRIDES += media.scan.filecount=100000
#add zhaojun end

PRODUCT_PROPERTY_OVERRIDES += sys.pm.warmboot.flag=0

ifeq ($(TCL_PRODUCT_BRANCH),EM)
PRODUCT_PROPERTY_OVERRIDES += persist.sys.timezone=Australia/Sydney
PRODUCT_PROPERTY_OVERRIDES += persist.sys.language=en
PRODUCT_PROPERTY_OVERRIDES += persist.sys.country=AU
PRODUCT_PROPERTY_OVERRIDES += persist.logonum.flag=0
else
PRODUCT_PROPERTY_OVERRIDES += persist.sys.timezone=Asia/Shanghai
PRODUCT_PROPERTY_OVERRIDES += persist.sys.language=zh
PRODUCT_PROPERTY_OVERRIDES += persist.sys.country=CN
endif

ifeq "$(ANDROID_512MB)" "true"
PRODUCT_PROPERTY_OVERRIDES += ro.config.low_ram=true
PRODUCT_PROPERTY_OVERRIDES += dalvik.vm.jit.codecachesize=0
endif

PRODUCT_PROPERTY_OVERRIDES += debug.hwui.render_dirty_regions=false
PRODUCT_PROPERTY_OVERRIDES += ro.hwui.texture_cache_flushrate=0

#enable Widevine drm
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
PRODUCT_PACKAGES += com.google.widevine.software.drm.xml \
                    com.google.widevine.software.drm \
                    libdrmwvmplugin libwvm libWVStreamControlAPI libwvdrm_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL) libdrmdecrypt libwvdrmengine

# Begin add by TCL zhanghangzhi, mail: zhanghangzhi@tcl.com 
PRODUCT_PACKAGES += ttm
# End of TCL
 
# refer CL: 1359011, put TKUI build here from dtv_linux/mak/android/android.mak
#ifeq  "$(CUSTOMER_AP)" "TKUI"
#	PRODUCT_PACKAGES += $(CUSTOMER_AP)
#endif

PRODUCT_COPY_FILES += device/dev/gamekit/libogrekit.so:system/lib/libogrekit.so
PRODUCT_COPY_FILES += device/dev/gamekit/MusicBox.blend:system/resource/MusicBox.blend 
PRODUCT_COPY_FILES += device/dev/gamekit/FileBrowse.blend:system/resource/FileBrowse.blend

#ifeq "$(DLNA_DMS_SUPPORT)" "true"
#PRODUCT_PACKAGES += DmsDemo
#endif

# Include System Property config file
$(call inherit-product, $(DEVICE_STRING)/system.mk)

# locale + densities. en_US is both first and in alphabetical order to
# ensure this is the default locale.
PRODUCT_LOCALES = \
zh_CN \
en_US \
ldpi \
hdpi \
mdpi \
xhdpi \
ar_EG \
ar_IL \
bg_BG \
ca_ES \
cs_CZ \
da_DK \
de_AT \
de_CH \
de_DE \
de_LI \
el_GR \
en_AU \
en_CA \
en_GB \
en_IE \
en_IN \
en_NZ \
en_SG \
en_US \
en_ZA \
es_ES \
es_US \
fi_FI \
fr_BE \
fr_CA \
fr_CH \
fr_FR \
he_IL \
hi_IN \
hr_HR \
hu_HU \
id_ID \
it_CH \
it_IT \
ja_JP \
ko_KR \
lt_LT \
lv_LV \
nb_NO \
nl_BE \
nl_NL \
pl_PL \
pt_BR \
pt_PT \
ro_RO \
ru_RU \
sk_SK \
sl_SI \
sr_RS \
sv_SE \
th_TH \
tl_PH \
tr_TR \
uk_UA \
vi_VN \
zh_TW

#move from vm_linux\android\mediatek\$(ANDROID_VERSION)\protect-bsp\frameworks\av\media\libstagefright\stagefright_extension\asf\Android.mk

PRODUCT_COPY_FILES += \
    $(TOP_OUT)/target/product/$(IC_SETTING)/symbols/system/lib/libasfextractor.so:system/lib/libasfextractor.so
    
#move from vm_linux\android\mediatek\$(ANDROID_VERSION)\frameworks\MediaPlayer\Android.mk
PRODUCT_COPY_FILES += \
    ${TOP}/../mediatek/$(ANDROID_VERSION)/frameworks/MediaPlayer/com.mediatek.mmp.xml:system/etc/permissions/com.mediatek.mmp.xml
