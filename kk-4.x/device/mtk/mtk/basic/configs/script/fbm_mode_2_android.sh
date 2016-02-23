#!/sbin/sh

echo "Turn on ZRAM in FBM..."

# switch to Android mode
#switch_fbm 2

# ZRAM in FBM memory
FBM_ADDR=`switch_fbm 0 | awk '{print $1}'`
FBM_SIZE=`switch_fbm 0 | awk '{print $2}'`


DISK_SIZE=$((${FBM_SIZE}*3))

if [ ${FBM_SIZE} != "0x0" ] ; then
	# Enable MID to allow only CPU access
	cli_shell "mid.rr 4" > /dev/null
	cli_shell "mid.sr $FBM_ADDR $FBM_SIZE none 4 0x40" > /dev/null
	cli_shell "mid.ie 1" > /dev/null
	cli_shell "mid.h on" > /dev/null
	cli_shell "mid.e 4" > /dev/null
	cli_shell "mid.rl" > /dev/null

	echo $FBM_ADDR > /sys/block/zram1/phyaddr
   	echo $FBM_SIZE > /sys/block/zram1/physize
   	echo $DISK_SIZE > /sys/block/zram1/disksize
   	mkswap /dev/block/zram1
   	swapon /dev/block/zram1 -p 0
fi
