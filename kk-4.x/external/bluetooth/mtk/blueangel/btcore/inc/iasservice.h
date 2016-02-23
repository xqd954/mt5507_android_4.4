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
//#ifdef __BT_TIMES_PROFILE__

typedef struct _BtIasServiceCbHandler
{
    ListEntry node;         	/* For internal stack use only. */
    AttdbCallback callback;    /* Pointer to callback function */
} BtIasServiceCbHandler;


typedef struct _BleIas_T
{
    U8 registerCount;
    BtIasServiceCbHandler AlertLevelList;
} BleIas_T;


U8 IasActivateService(void);
void IasDeactivateService(void);
void IasRegisterAttdbCallback(U16 handle, BtIasServiceCbHandler *handler);
void IasDeregisterAttdbCallback(BtIasServiceCbHandler *handler);

