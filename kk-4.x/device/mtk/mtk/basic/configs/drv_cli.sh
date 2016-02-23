#!/sbin/bin/sh

echo "7 4 1 7" > /proc/sys/kernel/printk

mknod /dev/cli    c 241 4
/system/bin/cli


