#!/sbin/sh
#
# Low priority service
# Only run after UI shown

echo "post-init: start"

source /system/etc/boot.conf

#until [ x`getprop dev.bootcomplete` == x"1" ] ; do
#    sleep 1
#done
#echo ""
#echo "==> Boot Complete"
#cat /proc/uptime
#echo ""

#until [ x`getprop init.svc.bootanim` == x"stopped" ] ; do
#    sleep 1
#done
#echo ""
#echo "==> Boot Animation Stopped"
#cat /proc/uptime
#echo ""

if [ -f /etc/service/config_ttyMT0.sh ]; then
    /sbin/sh /system/etc/service/config_ttyMT0.sh &
else
     echo "--- config_ttyMT0.sh: script not found, no need to run --- "
fi

until ps|grep android.process.media > /dev/null ; do
   sleep 1
done
echo "==> android.process.media start"
cat /proc/uptime
echo ""
 
if $ENABLE_BOOT_MET; then
echo "Boot analysis with MET END"
echo "	--out in /data/trace.log"
/sbin/sh /system/etc/service/_stop2.sh
fi 
 
#sleep 2

# enable drmserver
start drm

# disable mediascanner
# /sbin/sh /system/etc/script/disable_mediascanner.sh &
    
# flash init
#/sbin/sh /system/etc/script/install_flash.sh &

# usb driver. make sure it's before other modules which use usb.
insmod /system/lib/modules/musb_hdrc.ko

# Enable ADB
if $ENABLE_ADB_NET ; then
    /sbin/sh /etc/script/adb_net.sh
	echo "===> after adb init: adb_net"
else
    /sbin/sh /etc/script/adb_usb.sh
	echo "===> after adb init: adb_usb"
fi

# Enable Crash handler
if $ENABLE_ANDROID_DEV; then
   setprop mtk.failed_reboot 0
   if [ x`getprop mtk.cts_test` == x'1' ];then
	logcat -v time -f /data/log.txt -r 153600 -n 1 &
   else
    if [ x`getprop mtk.stress_test` == x'1' ];then      
	 logcat -v threadtime -f /data/log.txt -r 153600 -n 1 &
    else
 	 logcat -v time -f /data/log.txt -r 8192 -n 1 &
    fi
   fi
else
   setprop mtk.failed_reboot 1
fi

# Mount /cache
/sbin/sh /etc/script/mount_cache.sh

# Start Monkey Service
if $ENABLE_MONKEY_SERVICE; then
    /sbin/sh /system/bin/monkey --port 1080 &
fi

# clean memory
rm /*.ko

# for QA's information                                          
BOOT_COMPLETE_PROP_CHK_COUNT=10
for ((i=1;i<=$BOOT_COMPLETE_PROP_CHK_COUNT;i++))
do
if [ x`getprop sys.boot_completed` == x'1' ];then               
    echo "===> Good! Boot completed."                          
    touch /tmp/android_boot_complete 
    echo 7 > /proc/sys/kernel/printk
    break
fi 
  echo "post-init: sys.boot_complete chk: $i"
  sleep 3
done

# for auto test
if [ x`getprop mtk.auto_test` == x'1' ];then
    /sbin/sh /system/etc/script/autotest.sh
fi

# restart adbd: to avoid adbd not started normally last time
if $ENABLE_ADB_NET ; then
    if [ x`getprop service.adb.tcp.port` == x'-1' ];then
        echo " ===> post-init: restart adb: start..."
    		/sbin/sh /etc/script/adb_net.sh
        echo " ===> post-init: restart adb: end ..."
    else
        echo " ===> post-init: no need to restart adb"
    fi
fi

# dtv_svc_chk bg service
if [ -f /etc/service/dtv_svc_chk.sh ]; then
    /sbin/sh /etc/service/dtv_svc_chk.sh &
else
    echo "--- dtv_svc_chk: script not found, no need to run --- "
fi

#moved from init.rc to keep rootfs read-only for CTS passing
mount -o ro -o remount rootfs /

echo "post-init: end"
if [ x`getprop mtk.tms_support` == x'1' ];then
echo "post-init: end" > /proc/boottime
fi

chmod 777 /system/bin/avmute.bin

chmod 755 /mnt/usb
chown root:system /mnt/usb
echo "enable drop cache"
/sbin/sh /system/etc/script/drop_caches.sh &
         
#add by gaodw.
echo "set secure anr_show_background 1"
settings put secure anr_show_background 1
#end
