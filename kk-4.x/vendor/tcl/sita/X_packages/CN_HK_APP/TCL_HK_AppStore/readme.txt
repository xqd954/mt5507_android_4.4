版本号：
versionCode="003002"
versionName="3.1.002"

应用名称：
应用程序商店3.1

责任人：
莫子坤

包名：
com.tcl.appmarket2

依赖性：
安装apk(PackageInstallerHIS2.apk)
appconfig.xml
AppStoreUpdate.apk

使用说明：
1.AppStore3.1：应用商店3.1，adb push AppStore3.1 system/app

2.PackageInstallerHIS2.apk安装方法:
adb push PackageInstallerHIS2.apk system/app

3.首页3D动画库文件libtclappstoredata.so,libtclappstore.so
推送到系统目录：adb push libtclappstoredata.so system/lib  adb push libtclappstore.so system/lib

4.预装应用列表appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)配置方法：
由每个项目产品经理导出，集成人员将appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)拷贝到指定目录,赋予执行权限
导出方法：
http://manage.appstore.huan.tv/search/login.jsp
用户名和密码：TCL001
选择：商品管理->应用树查询；输入机型；货架名称选择：预置安装；国际化语言选择：中文或英语；选择搜索；选择导出预置安装XML；复制xml数据到appconfig_ClientType.xml文件(如：appconfig_TCL-AU-NT667K-S1.xml)。
appconfig_ClientType.xml需保存为UTF-8编码格式。

默认配置路径：将appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)文件拷贝到/system/etc/appinfo/appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)，appinfo目录和appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)文件赋予全部用户可读权限。
备用配置路径：如默认路径无法配置（如：MStar平台）可配置到目录:/config/appinfo/appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)，appinfo目录和appconfig_ClientType.xml(如：appconfig_TCL-AU-NT667K-S1.xml)文件赋予全部用户可读权限。


5.AppStoreUpdate.apk:
AppStoreUpdate.apk安装到system/app目录

更新记录：
2014-09-26 上传AppStore3.1 apk AppStoreUpdate.apk