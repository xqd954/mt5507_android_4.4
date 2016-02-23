#ifdef __cplusplus
extern "C"
{
#endif

#ifndef EXTERN
    #ifdef __cplusplus
        #define EXTERN          extern "C"
    #else
        #define EXTERN          extern
    #endif
#endif  // EXTERN

//#define ALL_PIC_VIDEOPATH 

#ifdef ALL_PIC_VIDEOPATH
#include "mtphoto4k.h"


EXTERN bool get_fgVideoPath();
/*EXTERN bool do_MTPHOTO4K_Show(unsigned int sktype, unsigned int skdataaddr, unsigned int skdatasize,  unsigned int u4x, unsigned int u4y
	, unsigned int u4Width, unsigned int u4Height);*/
#endif

#ifdef __cplusplus
}
#endif 
