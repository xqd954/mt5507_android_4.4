版本号：
versionCode="303011"
versionName="3.3.011.3.002"

应用名称：
应用程序商店3.3

责任人：
莫子坤

包名：
com.tcl.appmarket2

依赖性：
安装apk(PackageInstallerHIS2.apk)
AppStoreUpdate.apk

使用说明：
1.AppStore3.3安装到system/app目录，需要系统权限签名。

2.PackageInstallerHIS2.apk安装到system/app目录，需要系统权限签名。

3.集成黑名单文件到系统如下目录：
将blacklist_ClientType.xml(如：blacklist_TCL-AU-NT667K-S1.xml)文件拷贝到/system/etc/appinfo/blacklist_ClientType.xml(如：blacklist_TCL-AU-NT667K-S1.xml)，appinfo目录和blacklist_ClientType.xml(如：blacklist_TCL-AU-NT667K-S1.xml文件赋予全部用户可读权限。
备用配置路径：如默认路径无法配置（如：MStar平台）可配置到目录:/config/appinfo/blacklist_ClientType.xml(如：blacklist_TCL-AU-NT667K-S1.xml)，appinfo目录和blacklist_ClientType.xml(如：blacklist_TCL-AU-NT667K-S1.xml)文件赋予全部用户可读权限。

4.集成海外配置参数文件：
appstore_ClientType.json文件根据每个机型不同进行配置，将appstore_ClientType.json文件放到黑名单相同目录，修改可读权限。
"dataurl": 请求数据服务器地址
"adurl": 广告位服务器地址
"homeads":广告位ID
"region": 请求国家
"timezone": 请求时区
"language": 请求语言
