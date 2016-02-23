#!/sbin/sh

echo "==> local_bt_stress bg service start: $$"
CHK_COUNT=10000000
SLEEP_DURATION=5
i=0
error_found=0
BOOT_COMPLETE_CHK_COUNT=16

for ((i=1;i<=$CHK_COUNT;i++))
do
	if [ ! -f /tmp/android_boot_complete ]; then
		echo "--- LOCAL_BT_CHK: $i --- "
        sleep $SLEEP_DURATION
		if [ $i -gt $BOOT_COMPLETE_CHK_COUNT ]; then 
			error_found=1
			break
		fi
	else
		echo "--- LOCAL_BT_STRESS: android boot complete ---"
		break
	fi
done

echo "--- LOCAL_BT_STRESS: boot finished: final_count=$i !! ---"

# boot stress testing
if [ $error_found -gt 0 ]; then
	echo "--- BOOTUP_CHK: get log: dmesg ---"
	dmesg
	echo "--- BOOTUP_CHK: BOOT_NG, sleep 10s after dmesg --- "
	sleep 10
	echo "--- BOOTUP_CHK: get log: logcat ---"
	logcat &
	echo "==> ***** bootstress: err found, please check logcat/dmesg"
else
	sync
	sync
	echo "==> LOCAL_BT_STRESS: start reboot after 15 second...."
	sleep 15
	reboot
fi
