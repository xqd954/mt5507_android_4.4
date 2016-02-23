#!/sbin/sh

echo "==> bt_chk bg service start: $$"
FIRST_BOOT_FILE_CHK_COUNT=6
# boot complete wait: 160sec
BOOT_COMPLETE_CHK_COUNT=38
SLEEP_DURATION=5
i=0

for ((i=1;i<=$BOOT_COMPLETE_CHK_COUNT;i++)) 
do
	if [ ! -f /tmp/android_boot_complete ]; then
        echo "--- BOOTUP_CHK: $i out of  $BOOT_COMPLETE_CHK_COUNT --- "
        sleep $SLEEP_DURATION
	else
		echo "--- BOOTUP_CHK: android boot complete ---"
		break
	fi
done

if [ $i -gt $BOOT_COMPLETE_CHK_COUNT ]; then
#   echo "--- BOOTUP_CHK: boot NG, try reboot for recovery --- "

	if [ x`getprop btchk.logcat.need` == x'1' ]; then
		echo "--- BOOTUP_CHK: getprop: start !! --- "
		getprop | grep init
		getprop | grep boot_completed
		getprop | grep bootanim
		echo "--- BOOTUP_CHK: getprop: end !! --- "
  
		echo "--- BOOTUP_CHK: get log: dmesg ---"
		dmesg
		echo "--- BOOTUP_CHK: BOOT_NG, sleep 10s after dmesg --- "
		sleep 10
  
		echo "--- BOOTUP_CHK: get log: logcat ---"
		logcat &
		echo "--- BOOTUP_CHK: BOOT_NG, sleep 40s after logcat --- "
		sleep 40
	fi    
  
	echo "--- BOOTUP_CHK: boot NG NG!!! --- "
	
  	if [ x`getprop mtk.auto_test` == x'1' ]; then
		echo "--- BOOTUP: auto_test: no need to reboot ---"
	else
		echo "--- BOOTUP_CHK: going to reboot in 10s--- "
		sleep 10
	    reboot
	fi
fi

echo "--- BOOTUP_CHK: boot OK: final_count=$i !! ---"


