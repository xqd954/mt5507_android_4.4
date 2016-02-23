#!/sbin/sh

source /etc/boot.conf

export PATH=/system/bin:$PATH

mkdir -p /tmp

# for cts -- start
chmod 771 /tmp
# chgrp 1000 /tmp
chown root.system /tmp
# for cts -- end

# Factory mode via ttyMT3
stty -F /dev/ttyMT3 115200

# default umask in Linux world
umask 000

# if /etc/scripts/mount_main.sh does not exist ==> BASIC_IN_ROOTFS=true
if [ ! -f /etc/scripts/mount_main.sh ];then
	echo "--- BASIC_IN_ROOTFS=true --- "
	ln -s /linux_rootfs/basic /basic
else
	echo "--- BASIC_IN_ROOTFS=false --- "	
	mkdir /basic
	sh /etc/scripts/mount_main.sh
fi

touch /tmp/basic_mounted

if [ -f /system/bin/drv_init ]; then
    echo "--- init driver in driver only mode"
    /system/bin/drv_init &
else
    # Launch linux glibc world dtv_svc
    echo "--- linux rc.local --- "
    sh /etc/rc.d/rc.local
    echo "==> linux autorun: started"
fi

/sbin/sh /etc/script/setup_zram.sh
echo 20480 > /proc/sys/vm/min_free_kbytes

echo "dtv_svc.sh:wait_pre_select_b" > /proc/boottime
#wait for dtv_svc_pre_select_started, timeout is 5000ms
/system/bin/limited_wait /tmp/dtv_svc_pre_select_started 5000
echo "dtv_svc.sh:wait_pre_select_e" > /proc/boottime

