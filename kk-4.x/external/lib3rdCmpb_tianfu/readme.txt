file:
	本目录包含3rd app使用的cmpb封装库以及对应的测试程序

history:
Version       Date        Author    Modification
-------    ----------    -------    ------------
1.0        2012-3-24      mawei.ma    Create



------------------------------------------------

contents:

hios_player─── 海信应用商城和渐华游戏使用
         │
         ├───test dir: test_hios_player


libTsPlayer─── 上海电信IPTV，速影apk使用
         │
         ├───test dir: test_libTsPlayer


local_test ───  测试程序的目录【默认情况该目录下的测试程序在编译整个branch的时候不会被编译】
           ───  不要打开该目录下的Android.mk中的开关


build manual:

1. build current branch
   eg:  cd vm_linux/project_x/sys_build/hisense_android/mt5396_cn_android && make -L ODB=true

2. 切换到android编译目录
   eg: cd ../../../../../vm_linux/android/froyo-2.2

3. source 导入对应的build指令
   eg: source build/envsetup.sh


4. build:  mmm  <dir>
    │
    ├─── build so:  eg:  mmm external/lib3rdCmpb/libTsPlayer
    │
    ├─── build test elf: mmm external/lib3rdCmpb/local_test/test_libTsPlayer
   

