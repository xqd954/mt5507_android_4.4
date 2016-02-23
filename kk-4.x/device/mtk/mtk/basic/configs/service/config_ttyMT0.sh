#!/sbin/sh
echo "==> config ttyMT0 star"
su

CONFIG_STTY_PROPERTY_RUN_COUNT=5
for ((i=1;i<=$CONFIG_STTY_PROPERTY_RUN_COUNT;i++))
do
#/sbin/busybox stty -F /dev/ttyMT0 -a
/sbin/busybox stty -F /dev/ttyMT0 icrnl
/sbin/busybox stty -F /dev/ttyMT0 echo
/sbin/busybox stty -F /dev/ttyMT0 isig
/sbin/busybox stty -F /dev/ttyMT0 icanon
sleep 5
done
echo "==> config ttyMT0 end"

echo 0 >/proc/sys/vm/page-cluster 

#sleep 20
if [ -f /etc/script/fbm_mode_2_android.sh ]; then
    /sbin/busybox sh /etc/script/fbm_mode_2_android.sh
else
     echo "--- fbm_mode_2_android.sh: script not found, no need to run --- "
fi
