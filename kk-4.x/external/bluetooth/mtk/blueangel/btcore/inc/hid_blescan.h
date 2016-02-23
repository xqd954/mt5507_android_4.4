#ifndef __HID_BLESCAN_H__
#define __HID_BLESCAN_H__

BtStatus HidAddWhiteList(U8 *addr);
BtStatus HidRemoveWhiteList(U8 *addr);
void HidScanAdvInit(void);
void HidScanAdvEnable(void);
void HidScanAdvDisable(void);
void HidScanAdvUpdate(U8 *addr, BOOL scanNeeded);

#endif
