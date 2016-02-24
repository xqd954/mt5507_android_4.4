
*****************************************************************
1.APK名称：         TCL_All_GoLiveCinema.apk
2.APK英文名称：     GoLiveCinema
3.APK中文名称：     全球播单院线
	3.1APK由几个应用组成：
	 APK数目：1个。
	 是否依赖系统：是（是/否）。
	
	3.2依赖的应用
	（与其他APK交互的有那些应用？有依赖的请将XX部分填写自己apk依赖的应用名称）：
	
	 3.2.1：应用1：TCL应用商店

4.APK包名:        com.golive.cinema
5.APK主入口:      com.golive.cinema.MainActivity     
6.厂商定义版本号：2.2.22.02
  6.1: 适合的android版本：android4.0以上版本适应。
7.安装集成说明：
  7.1 预制到系统的方法：
	将TCL_All_GoLiveCinema.apk放置到/system/app；
	将libs目录下的库文件存放至/system/lib目录下：（备注：不同平台的kdm的so库放置在kdm文件夹，请集成时也放在/system/lib目录下）
		
		libGoliveHelper.so，
		libMD5.so，
		

 7.2 U盘升级方法，如果之前系统已经预置了apk，需到/system/app删除原有的apk，以及/data/data目录下的包名，方法如下：
  1).如果是只读文件系统，先remount分区，命令：mount -o remount ,rw /system
  2).删除apk，rm /system/app/TCL_All_GoLiveCinema.apk
  3).删除data数据包， rm -rf /data/data/TCL_All_GoLiveCinema.apk
  4).安装pm install /mnt/usb/XXXX/TCL_All_GoLiveCinema.apk
  5).重启
  备注：如果不是预置到system/app下的apk，
  可以直接覆盖安装pm install -r /mnt/usb/XXXX/TCL_All_GoLiveCinema.apk


8 功能简介或说明: 
   8.1 全球播同步院线，可与电影院同步观看正在热映的电影。

*****************************************************************
9.版本更改记录：
	9.1.1 更新日期：20150709
	9.1.2 版本名称：V2.2.22.02
	9.1.3 修改日志：
	GoLive测试组：
1)	首发影院、同步院线、CIBN资源统一在一个APK，默认是首发影院
2)	修复BUG:首发影院左上方“golive”图片没有左对齐
3)	修复BUG:影片详情的左上角文字一直显示“首发影院”
4)	修复BUG:不能进入广告
5)	修改APK ICON、启动页图片
6)	修复BUG：CMS改变KDM后，退出APK快速重新进入，偶尔不刷新界面（首发影院、同步院线）
7)	修复广告图片错误
8)	APK启动后，KDM启动后，先升级（如果需要升级）后init
9)	在登录接口、获取推荐影片、获取影片详情接口中增加下面一个属性： branchtype 版本分支类型；31-首发院线；32-同步院线；默认为31；
修复BUG：APK偶尔错误判断KDM没有初始化

	



	     ...
  
