#!/sbin/sh

#zengyongying: load ethernet driver first
if [ -f /system/lib/modules/star_mac.ko ]; then
        insmod /system/lib/modules/star_mac.ko
fi

insmod /snd-mtk.ko

until ps|grep android.process.media > /dev/null ; do
    sleep 1
done

insmod /system/lib/modules/v4l2-int-device.ko
insmod /system/lib/modules/videodev.ko
insmod /system/lib/modules/videobuf2-core.ko
insmod /system/lib/modules/videobuf2-memops.ko
insmod /system/lib/modules/videobuf2-vmalloc.ko
insmod /system/lib/modules/uvcvideo.ko
insmod /system/lib/modules/v4l2-common.ko

insmod /system/lib/modules/fuse.ko
insmod /system/lib/modules/ntfs.ko


if [ -f /system/lib/modules/mtk-hcd.ko ]; then
	insmod /system/lib/modules/mtk-hcd.ko
fi
if [ -f /system/lib/modules/usb-storage.ko ]; then
	insmod /system/lib/modules/usb-storage.ko
fi

/sbin/sh /system/etc/script/init_wifi.sh
                                               
if [ -f /system/lib/modules/xhci-hcd.ko ]; then
	insmod /system/lib/modules/xhci-hcd.ko 
fi                                             

#if [ -f /system/lib/modules/star_mac.ko ]; then
#	insmod /system/lib/modules/star_mac.ko
#fi

if [ -f /system/lib/modules/driver_cli.ko ]; then
	insmod /system/lib/modules/driver_cli.ko
fi

if [ -f /system/lib/modules/btusb.ko ]; then
	insmod /system/lib/modules/btusb.ko
	mknod /dev/btusb0 c 180 194
	chmod 666 dev/btusb0
fi