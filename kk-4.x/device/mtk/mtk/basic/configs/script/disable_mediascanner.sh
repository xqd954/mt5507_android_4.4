#!/sbin/sh

until ps|grep android.process.media > /dev/null ; do
    sleep 1
done

echo "DISABLE MediaScanner"
sh /system/bin/pm disable com.android.providers.media/com.android.providers.media.MediaScannerReceiver

