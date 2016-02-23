#!/sbin/sh

       until ps|grep android.process.media > /dev/null ; do
            sleep 1
        done

        if  [ ! -e /data/data/com.adobe.flashplayer/lib/ ] ; then
             cp /system/apk/oem_install_flash_player.bin /system/app/oem_install_flash_player_ics.apk
             sync
             rm -f /system/app/oem_install_flash_player_ics.apk
             sync
             cp /system/apk/oem_install_flash_player.bin /system/app/oem_install_flash_player_ics.apk
             sync
        fi
        until [ -e /data/data/com.adobe.flashplayer/lib/ ] ; do
                    sleep 1
        done
