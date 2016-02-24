支持机型：MT5507/MS918/MS828/MS628/G9

1.将libtsplayer.so存放于/system/lib/目录下；
2.将kdmServer和kdmStart.sh存放于/system/bin/目录下；
3.将kdmStart.sh加入开机启动项：【修改init.rc文件：(在文件最后追加以下内容)】
#permissions for kdmServer
    chmod 0755 /system/bin/kdmStart.sh
    chmod 0755 /system/bin/kdmServer

#deamon kdmServer
service kdmStart /system/bin/kdmStart.sh
    user root
    class main
    oneshot


开机检验：
    通过串口执行：ps | grep kdm命令，能看到/data/kdmServer进程则开机启动成功，否则请检查相关文件的位置和权限，以及开机启动项；