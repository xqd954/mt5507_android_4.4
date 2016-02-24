1.APK名称：         TCL_MT5507_DouguoRecipes.apk
2.APK英文名称：     Douguo Recipes
3.APK中文名称：     豆果美食
	3.1APK由几个应用组成：
	 APK数目：1个。
	是否依赖系统：否。
4.APK包名:              com.douguo.recipetv
5.APK主入口:            com.douguo.recipetv.MainActivity
6.厂商定义版本号：2.2.3
6.1: 适合的android版本：android4.0.3以上版本适应。
7.安装集成说明：
 7.1 预制到系统的方法：将TCL_MT5507_DouguoRecipes.apk放置到 /system/app 。

 7.2 U盘升级方法，如果之前系统已经预置了apk，需到/system/app删除原有的apk，以及/data/data目录下的包名，方法如下：
  1).如果是只读文件系统，先remount分区，命令：mount -o remount ,rw /system
  2).删除apk，rm /system/app/TCL_MT5507_DouguoRecipes.apk
  3).删除data数据包， rm -rf /data/data/com.douguo.recipetv
  4).安装pm install /mnt/usb/XXXX/TCL_MT5507_DouguoRecipes.apk
  5).重启
  备注：如果不是预置到system/app下的apk，
  可以直接覆盖安装pm install -r /mnt/usb/XXXX/TCL_MT5507_DouguoRecipes.apk


8 功能简介或说明:
   例如： 
   8.1 支持浏览菜谱功能。
   8.2 支持按分类浏览菜谱。
   8.3 支持菜谱搜索功能。
   8.4 支持查看最近浏览功能。

*****************************************************************
9.版本更改记录：
	9.1 第一版修改记录
		9.1.1 修改问题时间：20150928
		9.1.2 第三方应用版本名称：
			豆果美食版本号：2.2.3
		9.1.2 膝盖的问题，原因和对策：
			9.1.1.2.1.d（问题描述）最近浏览中始终无浏览记录
			9.1.1.2.1.r（问题原因）调用系统Api查询挂载外设方法是用错误
			9.1.1.2.1.a（问题对策）修改调用的系统方法
			9.1.1.2.1.v（修改人名称）王国伟
			9.1.1.2.1.m（修改人邮箱）wanggguowei@douguo.com
			
			
			
			
			