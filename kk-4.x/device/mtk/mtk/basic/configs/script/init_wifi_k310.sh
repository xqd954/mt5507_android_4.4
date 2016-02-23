#!/sbin/sh

if [ ! -s /data/misc/wifi/cfg80211.ko ] ; then
    cp -a /system/lib/modules/cfg80211.ko /data/misc/wifi/
fi

if [ ! -s /data/misc/wifi/mac80211.ko ] ; then
    cp -a /system/lib/modules/mac80211.ko /data/misc/wifi/
fi

if [ ! -s /data/misc/wifi/wpa_supplicant.conf ] ; then
    cp -a /system/etc/wifi/wpa_supplicant.conf /data/misc/wifi/
    chmod 777 /data/misc/wifi/wpa_supplicant.conf
fi

if [ ! -s /data/misc/wifi/wpa.conf ] ; then
    cp -a /system/etc/wifi/wpa_supplicant.conf /data/misc/wifi/wpa.conf
    chmod 777 /data/misc/wifi/wpa.conf
fi

if [ ! -s /data/misc/wifi/p2p.conf ] ; then
    cp -a /system/etc/wifi/p2p_supplicant.conf /data/misc/wifi/p2p.conf
    chmod 777 /data/misc/wifi/p2p.conf
fi

if [ ! -s /data/misc/wifi/rt5572sta.ko ] ; then
    cp -a /system/lib/modules/rt5572sta.ko /data/misc/wifi/rt5572sta.ko
fi

if [ ! -s /data/misc/wifi/mt7662u_sta.ko ] ; then
    cp -a /system/lib/modules/mt7662u_sta.ko /data/misc/wifi/mt7662u_sta.ko
fi

if [ ! -s /data/misc/wifi/mt7603u_sta.ko ] ; then
    cp -a /system/lib/modules/mt7603u_sta.ko /data/misc/wifi/mt7603u_sta.ko
fi

if [ ! -s /data/misc/wifi/rtutil5572ap.ko ] ; then
    cp -a /system/lib/modules/rtutil5572ap.ko /data/misc/wifi/rtutil5572ap.ko
fi

if [ ! -s /data/misc/wifi/rt5572ap.ko ] ; then
    cp -a /system/lib/modules/rt5572ap.ko /data/misc/wifi/rt5572ap.ko
fi

if [ ! -s /data/misc/wifi/rtnet5572ap.ko ] ; then
    cp -a /system/lib/modules/rtnet5572ap.ko /data/misc/wifi/rtnet5572ap.ko
fi
if [ ! -s /data/misc/wifi/mt7601Usta.ko ] ; then
    cp -a /system/lib/modules/mt7601Usta.ko /data/misc/wifi/mt7601Usta.ko
fi
if [ ! -s /data/misc/wifi/RT2870AP.dat ] ; then
    cp -a system/etc/Wireless/RT2870AP/RT2870AP.dat /data/misc/wifi/RT2870AP.dat
fi
if [ ! -s /data/misc/wifi/RT2870STA.dat ] ; then
    cp -a system/etc/Wireless/RT2870STA/RT2870STA.dat /data/misc/wifi/RT2870STA.dat
fi

if [ ! -s /data/misc/wifi/RT2870STA_7603.dat ] ; then
    cp -a system/etc/Wireless/RT2870STA/RT2870STA_7603.dat /data/misc/wifi/RT2870STA_7603.dat
fi

if [ ! -s /data/misc/wifi/RT2870STA_76x2.dat] ; then
    cp -a system/etc/Wireless/RT2870STA/RT2870STA_76x2.dat /data/misc/wifi/RT2870STA_76x2.dat
fi

if [ ! -s /data/misc/wifi/RT2870STA_7601.dat] ; then
    cp -a system/etc/Wireless/RT2870STA/RT2870STA_7601.dat /data/misc/wifi/RT2870STA_7601.dat
fi                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            