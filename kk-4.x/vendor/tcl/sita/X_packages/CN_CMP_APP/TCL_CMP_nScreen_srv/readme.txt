将apk放置到/system/app
将so放置到/system/lib

MS828配置项：
[NSCREEN]
TAGET=MS828;				#
STYLE=IQY;				#IQY MG OVERSEA CHINA
HasSD=YES;				#YES NO
CODE_STYLE= 0;				#0 1 2
CODE_GALLERY=1;				#0 1
CODE_MUSIC=1;				#0 1
CODE_ONLINE_SHARING=1;	                #0 1
CODE_RC=1;				#0 1
CODE_TV_BACK=0;				#0 1
CODE_SCREEN_FREEZE=1;		        #0 1
CODE_SD=1;				#0 1
AUDIO_SHARE=1;                          #0 1
AIRPLAY_MIRROR=0;                    //发送给手机的信息，手机镜像回传：0--不支持，1--支持 --9
CODE_EXT=000000;                     //发送给手机的信息，保留位

[NSCREEN]
TAGET=MT5507;                          //平台
STYLE=IQY;                            //版本系列：CHINA--国内通用,OVERSEA--海外通用,IQY--爱奇艺,MG--芒果
HasSD=NO;                             //项目有无SD卡：YES--有,NO--无
CODE_STYLE=0;                         //发送给手机的信息，版本系列：0--国内通用,1--海外通用,2--爱奇艺,3--芒果,4--OEM,5---腾讯，6---VST
CODE_GALLERY=1;                       //发送给手机的信息，图库：0--不支持,1--支持
CODE_MUSIC=1;                         //发送给手机的信息，音乐：0--不支持,1--支持
CODE_ONLINE_SHARING=1;                //发送给手机的信息，在线视频分享：0--不支持,1--支持
CODE_RC=1;                            //发送给手机的信息，遥控器：0--不支持,1--支持,2--遥控不含语音
CODE_TV_BACK=0;                       //发送给手机的信息，电视回传：0--不支持,1--支持
CODE_SCREEN_FREEZE=1;                 //发送给手机的信息，电视快照：0--不支持,1--支持
CODE_SD=0;                            //发送给手机的信息，SD卡项：0--无SD卡,1--有SD卡
AUDIO_SHARE=0;                        //发送给手机的信息，声音回传：0--不支持，1--支持
AIRPLAY_MIRROR=0;                    //发送给手机的信息，手机镜像回传：0--不支持，1--支持 --9
CODE_EXT=000000;                     //发送给手机的信息，保留位
SupportCollectInfo=1;                  //项目是否支持用户行为收集功能：0--不支持,1--支持