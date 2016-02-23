/*****************************************************************************
 *
 * Filename:
 * ---------
 * hid_blescan.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth HID Profile for BLE scan adv related handling.
 *
 * Author:
 * -------
 * Ting Zheng & Dlight
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 07 31 2013 dlight.ting
 * [ALPS00913678] [BLE SQC][BLE_HID]BT will auto power off while search and connect with mouse
 * Rollback //ALPS_SW/FPB/ALPS.JB2.FPB/alps/mediatek/protect/external/bluetooth/blueangel/btcore/btprofiles/hid/hid_blescan.c to revision 2
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
#include "bt_feature.h"
#include "utils.h"
#include "hidi.h"

#ifdef __BT_4_0_BLE__
#include "me_adp.h"
#include "BtbmLeAdv.h"
#include "hid_blescan.h"

#include "bt_adp_fs.h"

/* set this value according to DDB_MAX_ENTRIES */
#define MAX_HID_BLE_WHITE_LIST_NO (40)

#define HID_BLE_DEVICE_WHITE_LIST       "/data/@btmtk/hidWL"  


typedef struct _HidScanAdvCntx_T
{
    U8 whiteList[MAX_HID_BLE_WHITE_LIST_NO][6];
    U8 whiteListNo;
    U8 enabledState;
    BtAdvEventHandler hidBleScanAdvHandler;
    U32 hidBleScanIndex;
    BOOL scanNeeded[MAX_HID_BLE_WHITE_LIST_NO];
    U8 scanNeededNo;     
} HidScanAdvCntx_T;

HidScanAdvCntx_T HidScanAdvCtnx;

#define HidLeScanAdv(f)  (HidScanAdvCtnx.f)

extern U32 BtbmAdvLeGetAdvIndex(void);


void HidScanDeviceCallback(const BtEvent *Event)
{
    BtInquiryResult *inqResult = (BtInquiryResult *)&Event->p.inqResult;
    HidChannel *channel;
    BtStatus status;

    Report(("[HID][ADV] HidScanDeviceCallback"));
    {
        U8 *addr = inqResult->bdAddr.addr;
        int i;
        Report(("[HID][ADV] HidScanDeviceCallback Address %02x,%02x,%02x,%02x,%02x,%02x,",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]));

        for(i=0; i<HidLeScanAdv(whiteListNo); i++)
        {
            if(OS_MemCmp(HidLeScanAdv(whiteList)[i], 6, addr, 6))
            {
                Report(("[HID][ADV] HidScanDeviceCallback The same address in table"));
                goto Find;    
             }
        }
        return;
    }
Find:
    status = HID_HostQueryDevice(&channel, &inqResult->bdAddr);
    Report(("[HID][ADV] HidScanDeviceCallback, query service status: %d", status));
}


BtStatus HidAddWhiteList(U8 *addr)
{
    U8 i = 0;
    U8 *wlAddr = NULL;    
    S32 fd;

    if (addr == NULL)
    {
        return BT_STATUS_FAILED;
    }
    
    OS_Report("[HID][ADV] HidAddWhiteList whiteListNo:%d", HidLeScanAdv(whiteListNo));
    OS_Report("[HID][ADV] HidAddWhiteList Address %02x,%02x,%02x,%02x,%02x,%02x",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    for (i = 0; i < HidLeScanAdv(whiteListNo); i++)
    {
        wlAddr = HidLeScanAdv(whiteList)[i];
        OS_Report("[HID][ADV] HidAddWhiteList Address in WL %02x,%02x,%02x,%02x,%02x,%02x",
            wlAddr[0], wlAddr[1], wlAddr[2], wlAddr[3], wlAddr[4], wlAddr[5]);

        if (OS_MemCmp(wlAddr, 6, addr, 6))
        {
            OS_Report("[HID][ADV] HidAddWhiteList The same address in table");
            return BT_STATUS_SUCCESS;    
         }
    }
    
    if (HidLeScanAdv(whiteListNo) == MAX_HID_BLE_WHITE_LIST_NO)
    {
        return BT_STATUS_FAILED;
    }

    OS_MemCopy((U8 *)HidLeScanAdv(whiteList)[HidLeScanAdv(whiteListNo)], addr, 6);
    HidLeScanAdv(scanNeeded)[HidLeScanAdv(whiteListNo)] = FALSE;
    HidLeScanAdv(whiteListNo)++;

    fd = btmtk_fs_open((const U8*)HID_BLE_DEVICE_WHITE_LIST, BTMTK_FS_READ_WRITE);
    if (fd < 0)
    {
        fd = btmtk_fs_open((const U8*)HID_BLE_DEVICE_WHITE_LIST, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
        if (fd < 0)
        {
            return BT_STATUS_SUCCESS;
        }
    }

    btmtk_fs_write(fd, &HidLeScanAdv(whiteListNo), sizeof(HidLeScanAdv(whiteListNo)));
    btmtk_fs_write(fd, HidLeScanAdv(whiteList), sizeof(HidLeScanAdv(whiteList)));
    btmtk_fs_close(fd);

    return BT_STATUS_SUCCESS;    
}

BtStatus HidRemoveWhiteList(U8 *addr)
{
    U8 i = 0;
    U8 idx = 0;
    U8 *wlAddr = NULL;
    S32 fd;

    if ((addr == NULL) || (HidLeScanAdv(whiteListNo) == 0))
    {
        return BT_STATUS_FAILED;
    }    

    OS_Report("[HID][ADV] HidRemoveWhiteList whiteListNo:%d", HidLeScanAdv(whiteListNo));
    OS_Report("[HID][ADV] HidRemoveWhiteList Address %02x,%02x,%02x,%02x,%02x,%02x",
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);    

    for (idx = 0; idx < MAX_HID_BLE_WHITE_LIST_NO; idx++)
    {
        wlAddr = HidLeScanAdv(whiteList)[idx];
        OS_Report("[HID][ADV] HidRemoveWhiteList Address in WL %02x,%02x,%02x,%02x,%02x,%02x",
            wlAddr[0], wlAddr[1], wlAddr[2], wlAddr[3], wlAddr[4], wlAddr[5]);

        if (OS_MemCmp(wlAddr, 6, addr, 6))
        {
            OS_MemSet(HidLeScanAdv(whiteList)[idx], 0x0, 6);
            HidLeScanAdv(scanNeeded)[idx] = FALSE;
            for (i = idx + 1; i < HidLeScanAdv(whiteListNo); i++)
            {
                OS_MemCopy(HidLeScanAdv(whiteList)[i - 1], HidLeScanAdv(whiteList)[i], 6);
                HidLeScanAdv(scanNeeded)[i - 1] = HidLeScanAdv(scanNeeded)[i];
            }
            OS_MemSet(HidLeScanAdv(whiteList)[HidLeScanAdv(whiteListNo) - 1], 0x0, 6);
            HidLeScanAdv(scanNeeded)[HidLeScanAdv(whiteListNo) - 1] = FALSE;            
            HidLeScanAdv(whiteListNo)--;
            break;
        }
    }

     fd = btmtk_fs_open((const U8*)HID_BLE_DEVICE_WHITE_LIST, BTMTK_FS_READ_WRITE);
     if (fd < 0)
     {
         fd = btmtk_fs_open((const U8*)HID_BLE_DEVICE_WHITE_LIST, BTMTK_FS_READ_WRITE | BTMTK_FS_CREATE);
         if (fd < 0)
         {
             return BT_STATUS_SUCCESS;
         }
     }
     
     btmtk_fs_write(fd, &HidLeScanAdv(whiteListNo), sizeof(HidLeScanAdv(whiteListNo)));
     btmtk_fs_write(fd, HidLeScanAdv(whiteList), sizeof(HidLeScanAdv(whiteList)));
     btmtk_fs_close(fd);

     return BT_STATUS_SUCCESS;    
}

void HidScanAdvInit(void)
{
    U8 i = 0;
    S32 fd;
    
    OS_MemSet((U8 *)&HidScanAdvCtnx, 0, sizeof(HidScanAdvCntx_T));
    HidLeScanAdv(hidBleScanIndex) = BtbmAdvLeGetAdvIndex();

    fd = btmtk_fs_open((const U8*)HID_BLE_DEVICE_WHITE_LIST, BTMTK_FS_READ_ONLY);
    if (fd < 0)
    {
        // hid device white list is not exist
        return;
    }

    btmtk_fs_read(fd, &HidLeScanAdv(whiteListNo), sizeof(HidLeScanAdv(whiteListNo)));
    btmtk_fs_read(fd, HidLeScanAdv(whiteList), sizeof(HidLeScanAdv(whiteList)));
    btmtk_fs_close(fd);

    OS_Report("[HID][ADV] HidScanAdvInit, size: %d", HidLeScanAdv(whiteListNo));

    HidLeScanAdv(scanNeededNo) = HidLeScanAdv(whiteListNo);
    for (i = 0; i < HidLeScanAdv(scanNeededNo); i++)
    {
        HidLeScanAdv(scanNeeded)[i] = TRUE;
    }    
    if (HidLeScanAdv(scanNeededNo) > 0)
    {
        HidScanAdvEnable();
    }
}

/* The function shall be called only after the whiteListNo has been updated. */
void HidScanAdvEnable(void)
{
    Report(("[HID][ADV] Set HidScanAdvEnable"));
    HidLeScanAdv(hidBleScanAdvHandler).callback = HidScanDeviceCallback;
    HidLeScanAdv(hidBleScanAdvHandler).whiteList = HidLeScanAdv(whiteList);
    HidLeScanAdv(hidBleScanAdvHandler).whiteListNo = &HidLeScanAdv(whiteListNo);
    HidLeScanAdv(hidBleScanAdvHandler).blackListNo = NULL;
    HidLeScanAdv(hidBleScanAdvHandler).blackList = NULL;		
    BtBmLeAdvEnabled(HidLeScanAdv(hidBleScanIndex), &HidLeScanAdv(hidBleScanAdvHandler));
}

void HidScanAdvDisable(void)
{
    Report(("[HID][ADV] Set HidScanAdvDisable"));

    BtBmLeAdvDisabled(HidLeScanAdv(hidBleScanIndex), &HidLeScanAdv(hidBleScanAdvHandler));
}

void HidScanAdvUpdate(U8 *addr, BOOL scanNeeded)
{
    U8 i = 0;
    U8 *wlAddr = NULL;    

    if ((addr == NULL) || (HidLeScanAdv(whiteListNo) == 0))
    {
        return;
    }

    OS_Report("[HID][ADV] HidScanAdvUpdate whiteListNo:%d scanNeededNo:%d", 
        HidLeScanAdv(whiteListNo), HidLeScanAdv(scanNeededNo));
    OS_Report("[HID][ADV] HidScanAdvUpdate Address %02x,%02x,%02x,%02x,%02x,%02x scanNeeded %d", 
        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], scanNeeded);

    for (i = 0; i < HidLeScanAdv(whiteListNo); i++)
    {
        wlAddr = HidLeScanAdv(whiteList)[i];
        OS_Report("[HID][ADV] HidScanAdvUpdate Address in WL %02x,%02x,%02x,%02x,%02x,%02x scanNeeded %d",
            wlAddr[0], wlAddr[1], wlAddr[2], wlAddr[3], wlAddr[4], wlAddr[5], HidLeScanAdv(scanNeeded)[i]);
        
        if (OS_MemCmp(wlAddr, 6, addr, 6))
        {
            OS_Report("[HID][ADV] The same address in table");
            if (HidLeScanAdv(scanNeeded)[i] != scanNeeded)
            {
                HidLeScanAdv(scanNeeded)[i] = scanNeeded;
                if (scanNeeded)
                {
                    HidLeScanAdv(scanNeededNo)++;
                }                
                else
                {
                    HidLeScanAdv(scanNeededNo)--;
                }
                break;
            }
            else
            {
                return;
            }
         }
    }    

    if (HidLeScanAdv(whiteListNo) == MAX_HID_BLE_WHITE_LIST_NO)
    {
        return;
    }

    if (HidLeScanAdv(scanNeededNo) > 0)
    {
        HidScanAdvEnable();
    }
    else
    {
        HidScanAdvDisable();
    }
}

#endif
