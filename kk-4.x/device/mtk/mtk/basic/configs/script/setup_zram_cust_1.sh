#!/sbin/sh

# ZRAM in Linux free memory
ZRAM_SIZE=100  # unit: MB , 0 = disable
if [ ${ZRAM_SIZE} -ne 0 ] ; then
    echo $((${ZRAM_SIZE}*1024*1024)) > /sys/block/zram0/disksize
    mkswap /dev/block/zram0
    # Set higher priority than swam in FBM
    swapon /dev/block/zram0 -p 0
fi

