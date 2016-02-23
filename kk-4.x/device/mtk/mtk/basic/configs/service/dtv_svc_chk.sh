#!/sbin/sh
echo "==> dtv_svc_chk bg service: start: $$"
sleep 10

ENDLESS_LOOP=1                      
for ((i=1; $ENDLESS_LOOP<=1;i++))
do    
    ps | grep dtv_svc > /dev/null
    if [ $? -eq 0 ]; then            
        sleep 10                                
    else                            
        echo "--- !!! dtv_svc process is MISSING, going to reboot !!! --- "
        echo "--- !!! dtv_svc process is MISSING, going to reboot !!! --- "
        date >> /data/dtv_svc_was_missing
        sync   
        sync            
        sleep 10                                                                                   
        reboot                                         
    fi                                                        
done                      

if [ x`getprop mtk.tms_support` == x'1' ];then
echo "dtv_svc.sh: end" > /proc/boottime
fi

