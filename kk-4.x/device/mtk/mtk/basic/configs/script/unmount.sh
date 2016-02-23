#!/sbin/sh
sync
cp /system/bin/ubidetach /
umount /system -l -f
#umount /data -l -f
/ubidetach /dev/ubi_ctrl -d 1 -s
#/ubidetach /dev/ubi_ctrl -d 2 -s
touch done