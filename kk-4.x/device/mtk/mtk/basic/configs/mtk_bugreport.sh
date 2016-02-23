#!/sbin/sh

OUT=`mount | grep usbdisk`

if [ ! $? ]; then
    echo "Please insert USB disk"
    return 1
fi

rm -rf /mnt/usbdisk/bug
mkdir -p /mnt/usbdisk/bug

#
echo "Dump Log...."
procrank > /mnt/usbdisk/bug/procrank
/system/bin/top -n 1 -d 1 -m 30 -t > /mnt/usbdisk/bug/top
dmesg > /mnt/usbdisk/bug/dmesg
service list > /mnt/usbdisk/bug/service
cp /data/log.txt* /mnt/usbdisk/bug/
cp -f /data/anr/traces.txt /mnt/usbdisk/bug/traces.txt
cp -a /data/tombstones/ /mnt/usbdisk/bug/
ls -la /mnt/usbdisk/bug
sync
sync

echo "============================"
echo "Done, please remove USB disk"
