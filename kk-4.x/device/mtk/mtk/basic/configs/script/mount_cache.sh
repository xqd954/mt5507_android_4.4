#!/sbin/sh

# check the partition type: mtd or mmc
if [ -e /proc/mtd ] ; then
echo "*** Mount NAND ubifs cache..."
ubiattach /dev/ubi_ctrl -m 7 -d 3
mount -t ubifs /dev/ubi3_0 /cache -o rw,nosuid,nodev,no_chk_data_crc,bulk_read
else
echo "*** Mount EMMC ext4 cache..."
# /sbin/e2fsck -p /dev/block/mmcblk0p8
# EXIT_CODE=$?
# echo "e2fsck exit code $EXIT_CODE "
# case $EXIT_CODE in
# 0|1) echo "e2fsck: run ok!"
# ;;
# 2) echo "e2fsck: need reoot!"; reboot
# ;;
# 4) echo "e2fsck: all say yes!"; /sbin/e2fsck -y /dev/block/mmcblk0p8
# ;;
# *) echo "e2fsck: need use backup superblock but we don't have!";
# ;;
# esac

# STATUS=$?
# echo "e2fsck exit code $STATUS "
# if [ $STATUS != 0 ] && [ $STATUS != 1 ]
# then
#    echo "e2fsck: need more check and repair!"
#    reboot
# fi

mount -t ext4 -o nosuid,nodev,noauto_da_alloc /dev/block/mmcblk0p16 /cache     
# /system/bin/rm -r /cache/lost+found/*        
fi
             
toolbox chown system.cache /cache 
toolbox chown system.cache /cache/recovery
chmod 0770 /cache
chmod 0770 /cache/recovery
