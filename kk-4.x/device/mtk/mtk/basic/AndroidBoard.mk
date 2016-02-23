#
# Copyright (C) 2011 The Android Open Source Project
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

LOCAL_PATH := $(call my-dir)

ifeq "OPENBOX" "$(TARGET_TOOLBOX)"

SYSTEMBINSH := $(PRODUCT_OUT)/root/sbin/sh
LOCAL_MODULE := systemsh
OPEN_TARGET_SHELL := bash
$(SYSTEMBINSH): | $(OPEN_TARGET_SHELL)
$(SYSTEMBINSH): LOCAL_MODULE := $(LOCAL_MODULE)
$(SYSTEMBINSH):
	@echo "Symlink: $@ -> $(OPEN_TARGET_SHELL)"
	@rm -rf $@
	$(hide) ln -sf $(OPEN_TARGET_SHELL) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYSTEMBINSH)
ALL_MODULES.$(LOCAL_MODULE).INSTALLED += $(SYSTEMBINSH)  
else
busybox_name :=  \
    [ [[ ar arp arping ash awk basename blkid bunzip2 bzcat \
    bzip2 cal cat catv chgrp chmod chown chroot cksum clear cmp \
    cp cpio cut date dc dd depmod devmem df dhcprelay diff \
    dirname dmesg dnsdomainname dos2unix du dumpleases echo egrep \
    env expr false fbset fdisk fgrep find findfs free freeramdisk \
    fsync ftpget ftpput fuser getopt grep gunzip gzip hd \
    hdparm head hexdump hostid hostname hwclock id ifconfig insmod \
    install ionice iostat ip ipaddr ipcalc ipcrm ipcs iplink    \
    iproute iprule iptunnel kill killall killall5 length less ln \
    losetup ls lsmod lspci lsusb lzcat lzma lzop lzopcat makedevs \
    md5sum mdev mkdir mkdosfs mkfifo mkfs.vfat mknod mkswap mktemp \
    modinfo modprobe more mount mpstat mv nc netstat nice nmeter \
    nohup nslookup ntpd od pgrep pidof ping pipe_progress pkill \
    pmap printenv printf ps pwd rdate readahead readlink \
    renice reset rm rmdir rmmod route rx script \
    scriptreplay sed seq setconsole setsid sh sha1sum showkey \
    sleep sort split stat strings stty sum swapoff swapon sync \
    sysctl tac tail tar taskset tee telnet test time timeout top \
    touch traceroute true udhcpc udhcpd umount uname uniq unix2dos \
    unlzma unlzop unzip uptime uudecode uuencode vi watch wc wget \
    which xargs yes zcat

#SYMLINKS := $(PRODUCT_OUT)/system/bin/sh
SYMLINKS := $(foreach sym,$(busybox_name),$(PRODUCT_OUT)/root/sbin/$(sym))

BUSYBOX := busybox
$(SYMLINKS): $(PRODUCT_OUT)/root/sbin/$(BUSYBOX)
	@echo "Symlink: $@ -> $(BUSYBOX)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf $(BUSYBOX) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(BUSYBOX).INSTALLED := \
    $(ALL_MODULES.$(BUSYBOX).INSTALLED) $(SYMLINKS) 
endif

