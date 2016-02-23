1、将本地码流和bin档copy到U盘同一级目录
2、 ./cmpb_playback.bin test.ts speed2x_test.ts 2 
test.ts代表正常播放码流  speed2x_test.ts代表快进码流  2代表参数2 快进2倍
3、
f  -----切换到快进，在快进过程中，会有video pts和audio pts的log打出来
s  -----退出快进恢复正常播放
q  -----退出播放