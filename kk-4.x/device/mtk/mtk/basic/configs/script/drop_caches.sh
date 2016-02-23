#!/sbin/sh

Memory_LIMIT_512=536870912  
nTotalMem=`free | grep Mem: | awk '{print $2}'`
echo "nTotalMem:  $nTotalMem,  Memory_LIMIT_512:  $Memory_LIMIT_512 "
if [[ $nTotalMem -le $Memory_LIMIT_512 ]]
then
    echo "system Mem is 512MB" 
    #70M
    FREE_LIMIT_1=73400320 
    #60M
    FREE_LIMIT_2=62914560  
    #50M
    FREE_LIMIT_3=52428800 #51200
else
    echo "system Mem is 768MB" 
    #100M
    FREE_LIMIT_1=104857600 #102400
    #80M
    FREE_LIMIT_2=83886080  #81920
    #60M
    FREE_LIMIT_3=62914560  
    #50M
    #FREE_LIMIT_3=52428800 #51200
fi
        

#100M
#FREE_LIMIT_1=104857600 #102400
#80M
#FREE_LIMIT_2=83886080  #81920
#60M
#FREE_LIMIT_2=62914560  
#50M
#FREE_LIMIT_3=52428800 #51200

#sleep time
SLEEP_TIME=30


while true
do
	nFree=`free | grep Mem: | awk '{print $4}'`
	if [[ $nFree -le $FREE_LIMIT_3 ]]
	then
		echo "Free is : $nFree, LIMIT_3: $FREE_LIMIT_3,  drop caches"
		sync
		echo 3 > /proc/sys/vm/drop_caches
		
	elif [[ $nFree -le $FREE_LIMIT_2 ]]
	then
		echo "Free is : $nFree, LINIT_2: $FREE_LIMIT_2, drop caches"
		sync
		echo 2 > /proc/sys/vm/drop_caches
		
	elif [[ $nFree -le $FREE_LIMIT_1 ]]
	then
		echo "Free is : $nFree, LIMIT_1: $FREE_LIMIT_1, drop caches"
		sync
		echo 1 > /proc/sys/vm/drop_caches
	fi
	
	sleep $SLEEP_TIME
done

