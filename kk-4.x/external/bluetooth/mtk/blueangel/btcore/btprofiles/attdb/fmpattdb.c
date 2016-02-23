/*****************************************************************************
 *
 * Filename:
 * ---------
 * fmpattdb.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile GATT Service Definition
 *
 * Author:
 * -------
 * Ting Zheng
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
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifdef __BT_FMPT_PROFILE__
#include "attdb.h"
#include "attdef.h"

#include "iasservice.h"

extern U8 FmptAlertLevelCb(U8 type, BtRemoteDevice *link, U16 *len, U8 **raw);

BtIasServiceCbHandler FmptAlertLevel;


U8 FmpAttDB_Registration(void)
{
    BtStatus status;

    status = IasActivateService();
    if (status != BT_STATUS_SUCCESS)
    {
        return status;
    }

    InitializeListEntry(&(FmptAlertLevel.node));
    FmptAlertLevel.callback = FmptAlertLevelCb;
    IasRegisterAttdbCallback(ATT_HANDLE_IA_CHARACTERISTIC_ALERT_LEVEL_VALUE, &FmptAlertLevel);

    return status;
}

void FmpAttDB_Deregistration(void)
{
    IasDeactivateService();
    IasDeregisterAttdbCallback(&FmptAlertLevel);
}
#endif
