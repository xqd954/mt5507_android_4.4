#!/sbin/sh

#/bin/sh
#use example
#

# flashClone 2 : clone partition 2 to /mnt/usbdisk
# flashClone  clone all partitions to /mnt/usbdisk/upgrade.pkg

outputFile=android.bin
if [ $# == 1 ]
then
    echo "1 argument"
fi

if [ ! -z $1 ]
then
    outputFile=$1
    echo "User have give output file name "$outputFile
fi

if [ ! -e /mnt/usbdisk ] 
then
    echo "There isn't any USB/SDCARD device inserted"
    exit
elif [ ! -e /mnt/usbdisk/image/ ]
    then
    mkdir /mnt/usbdisk/image/
fi

#remove all existing old files
rm -rf /mnt/usbdisk/image/*

#arg 1: index of partition
#arg 2: name of partition
copyPartition()
{
    echo "Begin to copy partition $1 to $1_$2.img"
    dd if=/dev/block/mmcblk0p$1 of=/mnt/usbdisk/image/$1_$2.img
}

cd /mnt/usbdisk/image/

awk '/^ *part/ {    
    sub(/part/, "", $1);
    sub(/^0/, "", $1);
    sub(/:.*/, "", $1);
    if(gsub(/"/, "", $4) == 0) print "/proc/partinfo is bad formated"
    printf $1" "$4 "\n";
 }' /proc/partinfo > flashClone.tmp

if [ -e /mnt/usbdisk/image/$outputFile ]
then
    rm /mnt/usbdisk/image/$outputFile
fi

#create a empty file for output
touch $outputFile

while read myline
do
    echo "Input : "$myline
    #cut command maybe not available
    #index=`echo $myline | cut -d ' ' -f1`
    index=`echo $myline | awk '{print $1}'`
    name=`echo $myline | awk '{print $2}'`
    copyPartition $index $name
    cat ${index}_${name}.img >> $outputFile
done < flashClone.tmp

rm flashClone.tmp

ls /mnt/usbdisk/image/*.img |sort
echo
echo "all above images have been generated separately and also concatenated to:"
echo ${outputFile}
echo ========== all done ==========
echo


