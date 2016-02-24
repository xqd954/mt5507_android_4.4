#!/sbin/sh
echo "run tcl mw process start"
export F1_CONFIG=/midbase/base/run_config
chmod 777 /midbase/base/sitatvservice
chmod 777 /database


# create EPG database in RAM
mkdir /database/epg 0777 root system
mount -t tmpfs -o size=4m,mode=0777 tmpfs /database/epg
touch /database/epg/EpgData.db
chmod 0666 /database/epg/EpgData.db

/system/bin/logwrapper /midbase/base/sitatvservice &
echo "run tcl mw process end"



