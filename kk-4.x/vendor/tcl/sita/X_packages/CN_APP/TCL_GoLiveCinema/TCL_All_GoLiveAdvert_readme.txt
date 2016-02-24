
*****************************************************************
1.APK名称：          TCL_All_GoLiveAdvert.apk
2.APK英文名称：     GoLiveAdvert
3.APK中文名称：     零花钱
	3.1APK由几个应用组成：
	 APK数目：1个。
	是否依赖系统：是（是/否）。
	
	3.2依赖的应用
	（与其他APK交互的有那些应用？有依赖的请将XX部分填写自己apk依赖的应用名称）：
	
	 3.2.1：应用1：TCL应用商店

4.APK包名:              com.golive.advert
5.APK主入口:            com.golive.advertlib.AdvertActivity     
6.厂商定义版本号：2.2.1.12
6.1: 适合的android版本：android4.0以上版本适应。
7.安装集成说明：
 7.1 预制到系统的方法：将TCL_All_GoLiveAdvert.apk放置到 /system/app ;

 7.2 U盘升级方法，如果之前系统已经预置了apk，需到/system/app删除原有的apk，以及/data/data目录下的包名，方法如下：
  1).如果是只读文件系统，先remount分区，命令：mount -o remount ,rw /system
  2).删除apk，rm /system/app/TCL_All_GoLiveAdvert.apk
  3).删除data数据包， rm -rf /data/data/TCL_All_GoLiveAdvert.apk
  4).安装pm install /mnt/usb/XXXX/TCL_All_GoLiveAdvert.apk
  5).重启
  备注：如果不是预置到system/app下的apk，
  可以直接覆盖安装pm install -r /mnt/usb/XXXX/TCL_All_GoLiveAdvert.apk


8 功能简介或说明: 
   8.1 略

*****************************************************************
9.版本更改记录：
	9.1.1 更新日期：20150422
	9.1.2 版本名称：V2.2.1.12
	9.1.3 修改日志：
	GoLive测试组：

2.主要修改：




	PVCS:
	a.退出时全屏播放器；
b.添加检测视频结束机制，防止有些视频停留在最后一帧；
c.首页改版；
d.界面抬头信息改版；
e.界面抬头添加客服信息；
f.视频标题抬头添加显示奖励数额；
g.首页视频在自动播放第二次或以上时的奖励数额是0元；
h.首页视频也显示视频标题抬头；
i.以前通知栏只在首页显示，现在可以在很多地方显示；
j.首页添加院线入口；
k.解决砸金库在视频结束前按Home键跳转电视首页后再回来时视频与砸箱子重叠的问题；
l.添加“找帮助”功能，里面有五大块内容，分别是：客服信息、关于零花钱、使用指南、意见反馈、用户协议；
m.去掉商城里面的客服电话；
n.就算余额不足也可以把商品加入购物车；
o.用户中心使用手机号码登录前，如果手机号可以登录，则显示一个“勾”；
p.用户中心的编辑用户资料界面是禁止焦点移向右上角的客服信息；
q.解决801机型上由零花钱进入院线再返回时，操作零花钱其他界面引起电视重启的问题；
r.解决首次用手机登录后立刻到商城兑换时不自动填充手机号码的问题；
s.解决801机型上由零花钱进入院线再返回时，操作零花钱其他界面引起电视重启的问题；
t.解决801机型由首页进入“看视频”或“答问题”时残留首页视频小窗口绿色背影的问题；
u.解决801机型在“找帮助”界面按Home键跳转到电视首页时出现蓝色色块的问题；

3.接口改动：
v.新增获取反馈问题getFeedBack接口(g_ad_feedback);
w.新增保存用户反馈saveFeedBack接口(g_ad_user_feedback)
x.登录接口新增mall_new，agreement_new，userguide_new，about_new，customerservice_new响应参数



	     ...
  
