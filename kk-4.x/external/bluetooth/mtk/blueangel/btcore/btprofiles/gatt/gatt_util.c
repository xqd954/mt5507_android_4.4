/*****************************************************************************
 *
 * Filename:
 * ---------
 * gatts_svc.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATTS Profile.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#if defined(__BT_GATTC_PROFILE__) || defined(__BT_GATTS_PROFILE__)

#include "attdb.h"
#include "attdbmain.h"
#include "gattci.h"
#include "gattsi.h"

void dumpBtUUID(BT_UUID *bt_uuid){
    int i;
    U8 len = bt_uuid->len <= 16 ? bt_uuid->len : 16;
    char *ptr, buf[3*16+1];
    for(i = 0, ptr = buf;i < len;i++, ptr+=3){
        sprintf(ptr, "%02X:", bt_uuid->uuid[i]);
    }
    if(ptr != buf) ptr--;
    *ptr = 0;
    OS_Report("uuid len=%u, uuid=%s", bt_uuid->len, buf);
}

void dumpAttUUID(ATT_UUID *att_uuid){
    OS_Report("inst : %u", att_uuid->inst);
    dumpBtUUID(&att_uuid->uuid);
}

void dumpSvcUUID(GATT_SVC_UUID *svc_uuid){
    OS_Report("is_primary : %u", svc_uuid->is_primary);
    dumpAttUUID(&svc_uuid->uuid);
}

#endif
