/*****************************************************************************
 *
 * Filename:
 * ---------
 * HtsService.h
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Time Profile
 *
 * Author:
 * -------
 * Ting Zheng & Dlight Ting
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 * 
 * 10 15 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 * 11 04 2011 dlight.ting
 * NULL
 * .
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_TIMES_PROFILE__

typedef struct _BtCtsServiceCbHandler
{
    ListEntry node;         	/* For internal stack use only. */
    AttdbCallback callback;    /* Pointer to callback function */
} BtCtsServiceCbHandler;

typedef struct _BleCts_T
{
    U8 registerCount;
    BtCtsServiceCbHandler CtsCttimeList;
    BtCtsServiceCbHandler CtsCttimeConfigList;
    BtCtsServiceCbHandler CtsLocaltimeInfoList;
    BtCtsServiceCbHandler CtsReftimeInfoList;    
} BleCts_T;


U8 CtsActivateService(void);
void CtsDeactivateService(void);
void CtsRegisterAttdbCallback(U16 handle, BtCtsServiceCbHandler *handler);

#endif
