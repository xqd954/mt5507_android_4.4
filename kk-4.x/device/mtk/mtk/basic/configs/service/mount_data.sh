#!/sbin/sh

echo "mount data on mmcblk0p19"
/system/sbin/mount -t ext4 /dev/block/mmcblk0p19 /data -o noatime -o nosuid -o nodev -o noauto_da_alloc -o journal_checksum -o nodelalloc
var=$?
echo "var = ${var}" 
if  [ $var -ne "0" ] && [ $var -ne "1" ] && [ $var -ne "16" ]; then
   echo "mmcblk0p19 need to format"
   /system/bin/make_ext4fs /dev/block/mmcblk0p19
   
   /system/sbin/mount -t ext4 /dev/block/mmcblk0p19 /data
   if [ $? -ne "0" ]; then
        echo "data partition mount failed!"
    else
        echo "data partition mount success!"
    fi
else
   echo "mount data partion success!"
fi
