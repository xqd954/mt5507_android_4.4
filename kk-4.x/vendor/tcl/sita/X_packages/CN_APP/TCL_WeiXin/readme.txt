微信是跨平台通用版本，根据本机设备内存信息匹配功能。

在集成的时候注意，要集成微信应用需要将以下所有的apk拷贝到/system/app下，所有的so放到/system/lib下：

默认是全功能版本，很多低配置系统才加一个简化版本的配置项
内存512M：
1、微信视频点播是优酷的视频源，需要优酷的播放器和so文件，youku_tv.apk,libalgms.so,libluajava.so
2、微信apk文件TCL_WeiXin.apk
4、libKeyAndMouse.so，模拟遥控按键，同多屏互动的so完全一样，集成了多屏互动的so，就不需要重复集成。

内存大于512M：
1、需要台标讯飞识别，包括：IflytekTlrService.apk，libTLR15.so
2、微信视频点播是优酷的视频源，需要优酷的播放器和so文件，youku_tv.apk,libalgms.so,libluajava.so
3、微信apk文件TCL_WeiXin.apk
4、libKeyAndMouse.so，模拟遥控按键，同多屏互动的so完全一样，集成了多屏互动的so，就不需要重复集成。


版本说明：
versionCode 1,versionname: 20140408
为国庆前的版本，包括M90升级版本，M6C的309,312,315版本

versionCode 1,versionname: 1.1
为测试版本包括NTK667CN，M90后续版本，A71S-UD升级版本
修改内容：
采用smack源码包，可以打印所有收包和发包消息，另外写了一个数据库共享二维码路径，二维码只下载一次
可能会存在注册多次，导致用户号和二维码不一致的现象  

versionCode 1,versionname:1.2
 版本修改内容：家庭信箱必须要安装1.2版本以上的微信
（1）修改连接超时时间为3分钟
（2)限定注册次数
(3)共享数据库,包括二维码，绑定用户，分享记录
(4)登录成功后每次都获取二维码地址，更新数据库，避免二维码和用户号不一致的现象
(5)由家庭信箱修改安装包下图片和视频的权限
（6）数据库，分享记录列表，content节点用于存储图片地址

versionCode 3,versionname:1.3
versionCode 4,versionname:1.4
应用程序商店升级测试


versionCode 2,versionname:1.3
登录服务器异常放到循环里面，这样能尝试五次连接不同的机器同一频道弹幕，DTV返回的是中央一台，atv hdmi返回cctv1，要做模糊匹配，采用欢网的接口获取channelcode，判断是否一致。

versionCode 2,versionname:1.4
支持文本表情，设置-帮助-主页-返回退不出应用的问题

versionCode 2,versionname:1.5
1、文件删除：（1）、浏览用户分享记录，删除某一个文件   （2）、解绑用户，删除该用户缓存的所有文件

2、SD卡满了，将文件存储到flash中，并提示用户   
3、将视频存储路径写入数据库filename节点中，方便删除。

4、解决遥控器不在一个局域网（遥控页面收不到按键反馈就会堵塞，check时就会失败）

5、服务器修改协议后，删除用户会返回openid，通过openid查询删除本地数据库用户列表

6、bug:下载二维码为空，字节大小为0

7、添加表情改动出现的bug
8.（家庭信箱和开机向导）加上接收其他应用启动微信服务参数判断，收到其他应用退出二维码页面的广播时，判断是否有用户再确定是否退出进程。

versionCode 2,versionname:1.6（svn 19681)
1、简化版和全功能兼容
2、支持英文版
3、上传优酷播放记录
versionCode 2,versionname:1.7
（svn 19927)
1.注册失败尝试5次  2.识别失败就截图  3.视频存储空间为400M，图片为800M

4.K歌是DV信源，截图处理

svn 20134
修改换台流程：插了doggle,没有设置过机顶盒就dtv换台，不弹出机顶盒设置框

versionCode 2,versionname:1.8（svn 22127)
1.8版本：针对江苏南京移动登陆不了服务器的问题，登陆80端口尝试两次都失败则登陆6178。。超时设为10s*3=30s。。。

versionCode 2,versionname:1.9（apk svn 23162)用于测试MS918   6800机型
1.9版本：本地生成带有开机次数等设备信息的二维码，无网络且没有获取到网络二维码显示公众号。


恢复到1.8版本。



versionCode 2,versionname:1.8.1（svn 23666）
1.8.1版本：
1.addon和中间件截屏接口兼容，源码编译兼容
2.截屏返回图片超过960*640，进行缩放

versionCode 2,versionname:1.9（svn 26207）
1.9版本：
1.恢复到激活方案版本，终端生成二维码；
2.添加uuid的上报；


versionCode 8,versionname:2.0（svn 27888）
2.0版本：
1.增加应用的安装卸载和打开功能；
2.增加亲子教育模块；
3.增加终端自升级功能；
4.修改首页UI
5.添加注册成功时上报终端微信版本

versionCode 10,versionname:2.3.1（svn 29139）
2.3.1版本
1.6月UI修改需求，替换对话框和背景样式统一修改；
2.修改数据库升级方法，保证版本升级后数据的持久化管理；

versionCode 11,versionname:2.3.2（svn 31591）
2.3.2版本
1.新增腾讯视频功能；
2.新增未绑定提醒功能；

versionCode 12,versionname:2.3.3（svn ）
2.3.3版本
1.和腾讯视频APK匹配，腾讯视频不再是播放器，修改为一个有推荐页面的应用。修改微信启动腾讯播放器的方式
2、后续所有的APK都用TCL新系统签名后再上传

versionCode 13,versionname:2.3.4（svn ）
Description:防止签名混乱，TCL新系统签名更新为2.3.4.vercode为13，有更新就直接用2.3.6，vercode为15.。。
Cause:因为2.3.5已经被RT95签名，用于商店升级占用了。
Solution:后续版本全部用TCL新系统签名
Reviewed:liyulin

versionCode 18,versionname:2.4.0（svn ）
统一为TCL新签名，去掉接收商店新应用的广播