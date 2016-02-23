#!/sbin/sh

source /etc/boot.conf

chmod -R 777  /impdat
chmod -R 777  /tclconfig
chmod -R 777  /midbase
chmod -R 777  /database

export PATH=/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin

mkdir -p /tmp

# for cts -- start
chmod 771 /tmp
# chgrp 1000 /tmp
chown root.system /tmp
# for cts -- end

# Factory mode via ttyMT3
stty -F /dev/ttyMT3 115200

if $ENABLE_MW_CLI; then
    /system/bin/dtv_svc -input_fifo &
else
    /system/bin/dtv_svc -input_fifo > /dev/null &
fi

echo "==> dtv_svc: started"

/sbin/sh /etc/script/setup_zram.sh
#echo 10240 > /proc/sys/vm/min_free_kbytes


Memory_LIMIT_512=536870912
nTotalMem=`free | grep Mem: | awk '{print $2}'`
echo "nTotalMem: $nTotalMem,  Memory_LIMIT_512: $Memory_LIMIT_512 "
if [ $nTotalMem -le $Memory_LIMIT_512 ]
then
    /sbin/sh /etc/script/setup_zram.sh
    echo 10240 > /proc/sys/vm/min_free_kbytes
else 
    echo 20480 > /proc/sys/vm/min_free_kbytes
fi
                                                 



#sleep 60
#wait for dtv_svc_pre_select_started, timeout is 5000ms
#/system/bin/limited_wait /tmp/dtv_svc_pre_select_started 5000

if [ x`getprop mtk.tms_support` == x'1' ];then
echo "dtv_svc.sh: end" > /proc/boottime
fi

