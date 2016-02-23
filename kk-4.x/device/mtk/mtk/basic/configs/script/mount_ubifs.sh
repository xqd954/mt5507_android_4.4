#!/sbin/sh
ubiattach /dev/ubi_ctrl -m 7 -d 3
mount -t ubifs /dev/ubi3_0 /cache -o rw,nosuid,nodev,no_chk_data_crc,bulk_read
#chown system.cache /cache                           
chown 1000.2001 /cache
chmod 0770 /cache  
