集成方式：
1.把所有APK放到system/app目录下
2.把所有.so文件放到system/lib目录下
3.把xiriservice_tcl放到system/bin目录下，并给予755权限
4.把addtails.pcm放到/system/media目录下，并给予755权限
5.将tclxirihelp.dat放到/system/media目录下，并给予755权限
6.需要把xiriservice_tcl加入到init.rc中，保证其开机在后台运行。
