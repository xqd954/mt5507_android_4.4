
#include <cutils/properties.h>

void main(int argc, char **argv)
{
   char b_mute;

   printf("##################av setmute/unmute!!! \n");

   if (MTAL_Init() != 0)
   {
        printf("Driver init fail\n");
        return;
   }

if(atoi(argv[1]) == 1)
	b_mute = 1;
else
	b_mute = 0;

 MTAUDDEC_SetChannelMute(0,0,b_mute);
 MTAUDDEC_SetChannelMute(0,1,b_mute);
	
return;
}
