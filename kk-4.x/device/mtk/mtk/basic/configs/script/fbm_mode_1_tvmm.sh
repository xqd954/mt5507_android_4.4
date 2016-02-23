#!/sbin/sh

echo "Turn off ZRAM in FBM..."
time swapoff /dev/block/zram1
echo 1 > /sys/block/zram1/reset

# Disable MID
cli_shell "mid.rr 4"

# Switch to TV mode
switch_fbm 1
