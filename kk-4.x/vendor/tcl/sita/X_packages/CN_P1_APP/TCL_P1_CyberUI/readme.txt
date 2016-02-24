1.black_list.json

  功能 ：此文件用于配置出厂预置的网格UI应用列表的黑名单；

  位置：放到system/etc目录下集成

  方法：需要隐藏的应用通过包名和activity名的方式加入到此文件中；

   
2.cyberui_config.json

  功能：此文件用于配置网格UI信源列表中支持的信源类型；

  位置：放到system/etc目录下集成

  方法：针对clienttype和projectid唯一指定一个机型，0代表不支持此信源，1代表支持此信源类型；
  
3.PackageInstallerRenew.apk

  功能：用于支持网格UI的自升级功能；
  
   位置：集成到/system/app目录下