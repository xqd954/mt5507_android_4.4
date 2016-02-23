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

typedef struct _BtLlsServiceCbHandler
{
    ListEntry node;         	/* For internal stack use only. */
    AttdbCallback callback;    /* Pointer to callback function */
} BtLlsServiceCbHandler;


typedef struct _BleLls_T
{
    U8 registerCount;
    BtLlsServiceCbHandler LinkLostAlertList;
} BleLls_T;


U8 LlssActivateService(void);
void LlsDeactivateService(void);
void LlsRegisterAttdbCallback(U16 handle, BtLlsServiceCbHandler *handler);
void LlsDeregisterAttdbCallback(BtLlsServiceCbHandler *handler);
U8 LlsActivateService(void);



