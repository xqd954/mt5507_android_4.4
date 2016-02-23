#!/sbin/sh
 
/system/bin/k2logcat &

ADB_CHK_COUNT=3
for ((i=1;i<=$ADB_CHK_COUNT;i++))
do
    echo "-- auto_test: adb chk: $i: start"
    
    adb kill-server
	stop adbd
    adb start-server
	start adbd
    sleep 1
    
    break_loop=0
    adb connect localhost
    if [ $? -eq 0 ]; then
        adb devices | grep -e "localhost" | grep -e "offline"
        if [ $? -eq 0 ]; then 
            echo "!!! auto_test: adb FAILED !!!"
            echo "!!! auto_test: adb FAILED !!!"
        else
            echo "!!! auto_test: adb OK !!!" 
            adb disconnect localhost
            break_loop=1
        fi
    else
        echo "\t auto_test: adb connect FAILED"
    fi
    
    echo "-- auto_test: adb chk: $i: end"
    if [ $break_loop -ne 0 ]; then
        break
    fi

done
