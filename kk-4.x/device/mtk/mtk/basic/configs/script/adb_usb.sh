#!/sbin/sh

if [ -f /system/lib/modules/libcomposite.ko ]; then
	insmod /system/lib/modules/libcomposite.ko
fi
if [ -f /system/lib/modules/android.ko ]; then
	insmod /system/lib/modules/android.ko
fi

setprop service.adb.tcp.port -1
setprop persist.sys.usb.config adb
stop adbd
start adbd
