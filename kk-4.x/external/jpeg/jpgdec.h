
#ifndef EXTERN
    #ifdef __cplusplus
        #define EXTERN          extern "C"
    #else
        #define EXTERN          extern
    #endif
#endif  // EXTERN

EXTERN void setMtal_Init_Flag(BOOL mflag);
EXTERN BOOL getMtal_Init_Flag();

//JEXTERN(void) setMtal_Init_Flag JPP((boolean mflag));
//JEXTERN(boolean) getMtal_Init_Flag JPP((void));