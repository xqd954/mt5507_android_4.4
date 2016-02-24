
*****************************************************************
1.APK名称：          Settings_DTV.apk
2.APK英文名称：      Settings_DTV
3.APK中文名称：      系统设置


4.APK包名:             
Settings_DTV(系统设置）: com.tcl.settings

4.1.使用TVApp注意事项：
  4.1.1 U盘升级方法:
   如果之前系统已经预置了apk，需到/system/app删除原有的apk，以及/data/data目录下的包名，方法如下：
   1).如果是只读文件系统，先remount分区，命令：mount -o remount ,rw /system
  2).删除apk，rm /system/app/Settings_DTV.apk
  3).删除data数据包， rm -rf /data/data/com.tcl.settings
  4).安装pm install /mnt/usb/XXXX/Settings_DTV.apk
  5).重启
  备注：如果不是预置到system/app下的apk，
  可以直接覆盖安装pm install -r /mnt/usb/XXXX/Settings_DTV.apk
  
  4.1.2 备注：
			Settings_DTV.apk:为重构版本的系统Settings,可在任意地方调出。

  4.1.3对应代码路径和版本号：
  Settings_DTV.apk:        http://svn-server3.tclking.com/svn/applications/trunk/java/apps/TclSystemSettings


  4.1.4 apk路径及版本号：
  apk统一路径：http://svn-server3.tclking.com/svn/applications/trunk/java/apps/TclSystemSettings/output/MS918_update
  

  
  4.1.5 ini 配置项、配置文件路径：
   	http://svn-server3.tclking.com/svn/applications/trunk/java/apps/TclSystemSettings/output
*****************************************************************
5.版本更改记录：
   5.1 第一版提交记录，重构设置，所有页面以Dialog形式展示：20150916 18:00
   

  

      
	 	 