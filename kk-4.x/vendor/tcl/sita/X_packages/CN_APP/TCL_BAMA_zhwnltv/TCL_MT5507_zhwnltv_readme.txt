*****************************************************************
1.APK名称：        TCL_MT5507_zhwnltv.apk
2.APK英文名称：     ECalendar
3.APK中文名称：     中华万年历TV
	3.1APK由几个应用组成：
	 APK数目：1个。
	 是否依赖系统：是。
	
	3.2依赖的应用
	 无。

4.APK包名:              cn.etouch.ecalendarTv
5.APK主入口:            cn.etouch.ecalendarTv.StartActivity     
6.厂商定义版本号：1.1.0
6.1: 适合的android版本：例如：android1.6以上版本适应。
7.安装集成说明：
 7.1 预制到系统的方法：将TCL_MT5507_zhwnltv.apk放置到 /system/app 。

 7.2 U盘升级方法，如果之前系统已经预置了apk，需到/system/app删除原有的apk，以及/data/data目录下的包名，方法如下：
  1).如果是只读文件系统，先remount分区，命令：mount -o remount ,rw /system
  2).删除apk，rm /system/app/TCL_MT5507_zhwnltv.apk
  3).删除data数据包， rm -rf /data/data/cn.etouch.ecalendarTv
  4).安装pm install /mnt/usb/XXXX/TCL_MT5507_zhwnltv.apk
  5).重启
  备注：如果不是预置到system/app下的apk，
  可以直接覆盖安装pm install -r /mnt/usb/XXXX/TCL_MT5507_zhwnltv.apk


8 功能简介或说明:
   例如： 
   8.1 支持农历、黄历、天气查询。

*****************************************************************
  
