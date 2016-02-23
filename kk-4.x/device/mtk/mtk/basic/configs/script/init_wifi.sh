#!/sbin/sh

if [ ! -s /data/misc/wifi/cfg80211.ko ] ; then
    cp -a /system/lib/modules/cfg80211.ko /data/misc/wifi/
fi

if [ ! -s /data/misc/wifi/mac80211.ko ] ; then
    cp -a /system/lib/modules/mac80211.ko /data/misc/wifi/
fi

#zengyongying:	wifi.c will do this
#if [ ! -s /data/misc/wifi/wpa_suppLicant.conf ] ; then
#    cp -a /system/etc/wifi/wpa_supplicant.conf /data/misc/wifi/
#    chmod 777 /data/misc/wifi/wpa_supplicant.conf
#fi

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

if [ ! -s /data/misc/wifi/rtutil5572ap.ko ] ; then
    cp -a /system/lib/modules/rtutil5572ap.ko /data/misc/wifi/rtutil5572ap.ko
fi

if [ ! -s /data/misc/wifi/rt5572ap.ko ] ; then
    cp -a /system/lib/modules/rt5572ap.ko /data/misc/wifi/rt5572ap.ko
fi

if [ ! -s /data/misc/wifi/rtnet5572ap.ko ] ; then
    cp -a /system/lib/modules/rtnet5572ap.ko /data/misc/wifi/rtnet5572ap.ko
fi

if [ ! -s /data/misc/wifi/mt7601Uap.ko ] ; then
    cp -a /system/lib/modules/mt7601Uap.ko /data/misc/wifi/mt7601Uap.ko
fi

if [ ! -s /data/misc/wifi/mt7601Usta.ko ] ; then
    cp -a /system/lib/modules/mt7601Usta.ko /data/misc/wifi/mt7601Usta.ko
fi

if [ ! -s /data/misc/wifi/T2870AP.dat ] ; then
    cp -a system/etc/Wireless/RT2870AP/RT2870AP.dat /data/misc/wifi/RT2870AP.dat
fi

if [ ! -s /data/misc/wifi/T2870STA.dat ] ; then
    cp -a system/etc/Wireless/RT2870STA/RT2870STA.dat /data/misc/wifi/RT2870STA.dat
fi
#insmod /data/misc/wifi/rfkill.ko
#insmod /data/misc/wifi/compat.ko
#insmod /data/misc/wifi/cfg80211_ath6kl.ko
#mkdir -p /lib/modules/3.0.13

# -------Atheros AR1021 (ath6kl)-------
#insmod /data/misc/wifi/rfkill.ko
#insmod /data/misc/wifi/compat.ko
#insmod /data/misc/wifi/cfg80211_ath6kl.ko
#insmod /data/misc/wifi/ath6kl_usb.ko ath6kl_p2p=1

# -------Realtek 8192CU-------
#insmod /data/misc/wifi/rfkill.ko
#insmod /data/misc/wifi/cfg80211.ko
#insmod  /data/misc/wifi/8192cu.ko
# -------Ralink RT2X00-------
#insmod /data/misc/wifi/rfkill.ko
#insmod /data/misc/wifi/cfg80211.ko
#insmod /data/misc/wifi/mac80211.ko
#insmod /data/misc/wifi/rt2x00lib.ko
#insmod /data/misc/wifi/rt2x00usb.ko
#insmod /data/misc/wifi/rt2800lib.ko
#insmod /data/misc/wifi/rt2800usb.ko
# -------Ralink RT2X00-------
#insmod /data/misc/wifi/rtutil5572sta.ko
#insmod /data/misc/wifi/rt5572sta.ko
#insmod /data/misc/wifi/rtnet5572sta.ko
