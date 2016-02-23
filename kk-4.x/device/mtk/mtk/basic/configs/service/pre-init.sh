#!/sbin/sh
#
# Running at boot begining
# Prepare for booting

source /etc/boot.conf

echo ""
echo "==> pre-init start"
cat /proc/uptime
echo ""

# clean tools (use android toolbox provided sets)
rm /sbin/ls
rm /sbin/ln
rm /sbin/dmesg

if $ENABLE_LOGCAT; then
    logcat -v time &
fi

# Change Mac address
if $ENABLE_DYNAMIC_MAC; then
/sbin/sh /system/etc/script/set_eth0_mac.sh
fi

# samba data
mkdir -p /data/misc/smb

echo "==> pre-init end"
if [ x`getprop mtk.tms_support` == x'1' ];then
echo "pre-init: end" > /proc/boottime
fi
