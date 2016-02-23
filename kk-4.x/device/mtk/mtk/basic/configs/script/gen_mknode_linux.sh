
#OUTPUT: android/jb-4.x/device/mtk/mt5396/DTV_OUT/dev_linux.txt
#INPUT1:  android/jb-4.x/device/mtk/mt5396/configs/script/mknode.sh
#INPUT2:  chiling/rootfs/common/misc/dev4_2.txt
#INPUT3:  android/jb-4.x/device/mtk/mt5396/configs/script/mknode_linux_exception_list.txt

function usage ()
{
  echo "$0 mknode_linux.sh mknode.sh  chiling/rootfs/common/misc/dev4_2.txt"
  echo "1nd is our output file: <dev_linux.txt>"
  echo "2rd is our input file:  <dev.tx>"
  echo "3th is our input file:  <dev4_2.txt>"
  echo "4th is our input file:  <mknode_linux_exception_list.txt>"
}

#example:
#sh -x ./android/jb-4.x/device/mtk/mt5396/configs/script/gen_mknode_linux.sh
#~/0801_build/DTV/DEV_BR/e_IDTV0801_LINUX_002218_2_ANDROID_WWTV_35_BR/vm_linux/android/jb-4.x/device/mtk/mt5396/configs/script/mknode_linux.sh
#~/0801_build/DTV/DEV_BR/e_IDTV0801_LINUX_002218_2_ANDROID_WWTV_35_BR/vm_linux/android/jb-4.x/device/mtk/mt5396/configs/script/mknode.sh
#~/0801_build/DTV/DEV_BR/e_IDTV0801_LINUX_002218_2_ANDROID_WWTV_35_BR/vm_linux/chiling/rootfs/common/misc/dev4_2.txt
#~/0801_build/DTV/DEV_BR/e_IDTV0801_LINUX_002218_2_ANDROID_WWTV_35_BR/vm_linux/android/jb-4.x/device/mtk/mt5396/configs/script/mknode_linux_exception_list.txt 2>&1|tee log.txt

if [ -z $1 ]; then
  usage;
  exit;
fi

if [ -z $2 ]; then
  usage;
  exit;
fi

if [ -z $3 ]; then
  usage;
  exit;
fi

if [ -z $4 ]; then
  usage;
  exit;
fi

rm -rf $1

while read line
do
  if [ "`echo $line|grep -c ^#`" == 1 ];then
    continue;
  fi

  #echo $line
  DEVICE_NAME=`echo $line |awk '{print $1}'`
  TYPE_VALUE=`echo $line |awk '{print $2}'`
  MODE_VALUE=`echo $line |awk '{print $3}'`
  UID_VALUE=`echo $line |awk '{print $4}'`
  GID_VALUE=`echo $line |awk '{print $5}'`
  MAJOR_VALUE=`echo $line |awk '{print $6}'`
  MINOR_VALUE=`echo $line |awk '{print $7}'`

  EXCEPTION_MATCHED=`grep -w -c "/$DEVICE_NAME$" $4`

  if [ "$EXCEPTION_MATCHED" == '0' ]; then
    if [ "$TYPE_VALUE" == 'd' ]; then
      if [ "`grep $DEVICE_NAME -c $2`" == 0 ]; then
        echo "$DEVICE_NAME $TYPE_VALUE $MODE_VALUE $UID_VALUE $GID_VALUE $MAJOR_VALUE $MINOR_VALUE" >> $1
      fi
    elif [ "$TYPE_VALUE" == 'c' ]; then
      if [ "`grep -w $DEVICE_NAME -c $2`" == 0 ]; then
        # for CTS, /dev/port must be 600
        if [ "$DEVICE_NAME" == 'dev/port' ]; then
          echo "$DEVICE_NAME $TYPE_VALUE 600 $UID_VALUE $GID_VALUE $MAJOR_VALUE $MINOR_VALUE" >> $1
        else
          echo "$DEVICE_NAME $TYPE_VALUE $MODE_VALUE $UID_VALUE $GID_VALUE $MAJOR_VALUE $MINOR_VALUE" >> $1
        fi
      fi
    elif [ "$TYPE_VALUE" == 'b' ]; then
      if [ "`grep -w $DEVICE_NAME -c $2`" == 0 ]; then
        # for CTS, all block device nodes must be 600
        echo "$DEVICE_NAME $TYPE_VALUE 600 $UID_VALUE $GID_VALUE $MAJOR_VALUE $MINOR_VALUE" >> $1
      fi
    fi
  fi
done < $3
