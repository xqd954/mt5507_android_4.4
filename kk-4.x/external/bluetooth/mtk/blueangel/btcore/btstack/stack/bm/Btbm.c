/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2006
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * btbm.c
 *
 * Project:
 * --------
 *   MAUI ESI Bluetooth
 *
 * Description:
 * ------------
 *   This file contains functions provide the service to MMI or JAVA to 
 *   make the operation of command and event
 *
 * Author:
 * -------
 * Dlight Ting(mtk01239)
 *      Create 2006/2/9
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 *
 * 09 21 2011 sh.lai
 * [ALPS00074975] [Need Patch] [Volunteer Patch] Update BT lib
 * .
 *
 * May 12 2009 mtk02126
 * [MAUI_01744570] MTK:Bluetooth_it the power off bt need long time
 * 
 *
 * Jan 22 2009 mtk02126
 * [MAUI_01320514] check in __BTMODULE_MT6612__ compile option
 * 
 *
 * Nov 26 2008 mtk01239
 * [MAUI_01284669] [Bluetooth] New arch for BT
 * 
 *
 * Aug 5 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jul 11 2008 mtk01239
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * Jun 26 2008 MTK02126
 * [MAUI_00790838] [Bluetooth] Revise BT code and format it.
 * 
 *
 * May 26 2008 mtk01239
 * [MAUI_00777619] [Bluetooth] Revise the BTBM code
 * 
 *
 * May 16 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * May 14 2008 mtk01239
 * [MAUI_00771864] [Bluetooth] BT 2.1 new feature TW side
 * 
 *
 * May 5 2008 mtk01411
 * [MAUI_00972483] MTK:Bluetooth_when handset with earphone connected ,the name show empty
 * 
 *
 * Apr 6 2008 mtk01411
 * [MAUI_00744804] [Bluetooth] Fix SPP Compile Warnings
 * 
 *
 * Apr 6 2008 mtk01411
 * [MAUI_00740378] JSR82_MS display exchange passcode screen in a jiffy
 * 
 *
 * Mar 31 2008 mtk01411
 * [MAUI_00745048] [Bluetooth] Modify ask name mechanism in BTStack
 * 
 *
 * Mar 31 2008 mtk01411
 * [MAUI_00745048] [Bluetooth] Modify ask name mechanism in BTStack
 * 
 *
 * Mar 31 2008 mtk01411
 * [MAUI_00745048] [Bluetooth] Modify ask name mechanism in BTStack
 * 
 *
 * Mar 31 2008 mtk01411
 * [MAUI_00289659] [BT] can't search any more
 * 
 *
 * Mar 27 2008 mtk01411
 * [MAUI_00644078] [JSR82][1] Assert Fail: 0 btmmicm.c 1570 - MMI (src_module is set as MMI abnormally)
 * 
 *
 * Mar 24 2008 mtk01411
 * [MAUI_00286133] [JSR82] Screen stay on the "Please Wait" (No local name cnf is reported)
 * 
 *
 * Mar 14 2008 mtk01411
 * [MAUI_00731923] JSR82 _it can't load image
 * 
 *
 * Feb 19 2008 mtk01411
 * [MAUI_00621026] [Bluetooth] Check in JSR82 codes
 * 
 *
 * Feb 3 2008 mtk01411
 * [MAUI_00614891] [Bluetooth] Check in SIMAP PIN Code Length feature
 * 
 *
 * Feb 1 2008 mtk01411
 * [MAUI_00614891] [Bluetooth] Check in SIMAP PIN Code Length feature
 * 
 *
 * Feb 1 2008 mtk01411
 * [MAUI_00614891] [Bluetooth] Check in SIMAP PIN Code Length feature
 * 
 *
 * Jan 28 2008 mtk01239
 * [MAUI_00608294] BT-SPP connect failed
 * 
 *
 * Jan 27 2008 mtk01411
 * [MAUI_00612730] [Bluetooth] Check in MTK_BB_LOOPBACK_MODE compile option dependency for testing
 * 
 *
 * Jan 23 2008 mtk01239
 * [MAUI_00257776] Bluetooth_after delete the connected device,connect it again not pop up input passco
 * 
 *
 * Dec 27 2007 mtk01239
 * [MAUI_00596633] [Bluetooth] Checkin MT6611 code
 * 
 *
 * Dec 24 2007 mtk01239
 * [MAUI_00594662] [Bluetooth] patched code error
 * 
 *
 * Nov 26 2007 mtk01239
 * [MAUI_00581868] [Bluetooth] Create SCO not do the role switch operation
 * 
 *
 * Nov 16 2007 mtk01239
 * [MAUI_00576928] [1]Assert fail:0 btmmicm.c 1497-MMI
 * 
 *
 * Nov 5 2007 mtk01239
 * [MAUI_00239618] [BT] printer bt device discovred when inquiry audio device
 * 
 *
 * Oct 22 2007 mtk01239
 * [MAUI_00563448] [Bluetooth] patch BT code.
 * 
 *
 * Oct 8 2007 mtk01239
 * [MAUI_00556508] [BT]Shrinking BT code size
 * 
 *
 * Sep 12 2007 mtk01239
 * [MAUI_00546740] [Bluetooth] Checkin JSR82 code
 * 
 *
 * Sep 3 2007 mtk01239
 * [MAUI_00541673] [Bluetooth] Update BT codes.
 * 
 *
 * Aug 20 2007 mtk01239
 * [MAUI_00535716] [Bluetooth] fix complier warming.
 * 
 *
 * Aug 20 2007 mtk01239
 * [MAUI_00237089] MTK:Bluetooth HFP_when inquiry new device during handset popup "Handfree profile dis
 * 
 *
 * Aug 12 2007 mtk01239
 * [MAUI_00533095] BT-it's hard to search device
 * 
 *
 * Aug 9 2007 mtk01239
 * [MAUI_00236423] MTK:Bluetooth HFP_when discovered device hfp auto  disconnect
 * 
 *
 * Aug 6 2007 mtk01239
 * [MAUI_00236423] MTK:Bluetooth HFP_when discovered device hfp auto  disconnect
 * 
 *
 * Aug 4 2007 mtk01239
 * [MAUI_00526809] BT-HID can't work
 * 
 *
 * Jul 30 2007 mtk01239
 * [MAUI_00527738] [Bluetooth] Add ESO, fix pending con problem.
 * 
 *
 * Jul 30 2007 mtk01239
 * [MAUI_00453663] [BT][1]Assert fail: MEC (pendCons) > 0 mecon.c 168 -BT
 * 
 *
 * Jul 30 2007 mtk01239
 * [MAUI_00236364] MTK:Bluetooth BPP_[1] Assert fail: 0 btbm.c 1064 - BT
 * 
 *
 * Jul 29 2007 mtk01239
 * [MAUI_00453770] [1] Assert Fail: 0 btbm.c 1089 -BT
 * 
 *
 * Jul 16 2007 mtk01411
 * [MAUI_00417564] [Bluetooth] Gap tester code modification for PTW BT2.0 test case
 * 
 *
 * Jul 16 2007 mtk01239
 * [MAUI_00417454] [Bluetooth][MT6601] Check in BT2.0 patch
 * 
 *
 * Jul 13 2007 mtk01239
 * [MAUI_00416031] BT_It can't be delete and inquiry new device will pop up Empty
 * 
 *
 * Jul 11 2007 mtk01411
 * [MAUI_00445971] [BT]The icon of BT disappear.
 * 
 *
 * Jul 7 2007 mtk01239
 * [MAUI_00412062] BT_After Pairing screen, when input the wrong passcode, it should pop up "Pairing Fa
 * 
 *
 * Jun 29 2007 mtk01239
 * [MAUI_00410410] BT_It's difficult to find our handset when inauiry BT device
 * 
 *
 * Jun 20 2007 mtk01239
 * [MAUI_00407195] UPF_We are failed to bond with openinterface
 * 
 *
 * Jun 18 2007 mtk01239
 * [MAUI_00406886] [Bluetooth UPF27] Assert BTMMICm.c
 * 
 *
 * Jun 18 2007 mtk01239
 * [MAUI_00406886] [Bluetooth UPF27] Assert BTMMICm.c
 * 
 *
 * Jun 6 2007 mtk01239
 * [MAUI_00401290] BT-FTP auto disconnect
 * 
 *
 * Jun 5 2007 mtk01239
 * [MAUI_00401290] BT-FTP auto disconnect
 * 
 *
 * Jun 1 2007 mtk01239
 * [MAUI_00399654] [Bluetooth] Checkin UPF Test code
 * 
 *
 * Jun 1 2007 mtk01239
 * [MAUI_00399654] [Bluetooth] Checkin UPF Test code
 * 
 *
 * May 21 2007 mtk01239
 * [MAUI_00395001] [Bluetooth] MT6601 Host test code
 * 
 *
 * May 7 2007 mtk01239
 * [MAUI_00389758] [Bluetooth] Checkin JSR82 code of BTStack
 * 
 *
 * Mar 29 2007 mtk01239
 * [MAUI_00377236] [Bluetooth] Security level 2
 * 
 *
 * Mar 26 2007 mtk01411
 * [MAUI_00375903] [Bluetooth SIMAP] Replace report function to bt_trace
 * 
 *
 * Mar 26 2007 mtk01239
 * [MAUI_00231225] bt_hid disconnected indication pop up, but it return to disconnecting screen
 * 
 *
 * Mar 12 2007 mtk01239
 * [MAUI_00230600] bt_it can not be connected any more
 * 
 *
 * Mar 9 2007 mtk01239
 * [MAUI_00357653] [BT&WLAN Coexist] Can't receive VoIP MT call when BT inquiry audio device
 * 
 *
 * Mar 8 2007 mtk01239
 * [MAUI_00230591] bt_after remote sim mode activating canceled, the mode is still on
 * 
 *
 * Mar 7 2007 mtk01239
 * [MAUI_00357653] [BT&WLAN Coexist] Can't receive VoIP MT call when BT inquiry audio device
 * 
 *
 * Mar 5 2007 mtk01239
 * [MAUI_00357653] [BT&WLAN Coexist] Can't receive VoIP MT call when BT inquiry audio device
 * 
 *
 * Jan 29 2007 mtk01239
 * [MAUI_00321407] [BT]update Bluetooth code
 * 
 *
 * Jan 25 2007 mtk01239
 * [MAUI_00361128] [Bluetooth]Change search name request timeout to 2seconds
 * 
 *
 * Jan 24 2007 mtk01239
 * [MAUI_00361128] [Bluetooth]Change search name request timeout to 2seconds
 * 
 *
 * Jan 22 2007 mtk01239
 * [MAUI_00342967] [Bluetooth]Change packet type for different BT Chip
 * 
 *
 * Jan 22 2007 mtk01239
 * [MAUI_00342967] [Bluetooth]Change packet type for different BT Chip
 * 
 *
 * Jan 22 2007 mtk01239
 * [MAUI_00342967] [Bluetooth]Change packet type for different BT Chip
 * 
 *
 * Jan 19 2007 mtk01239
 * [MAUI_00359106] BT_A2DP/AVRCP_When press stop key and play, the audio will output via MS
 * 
 *
 * Jan 16 2007 mtk01239
 * [MAUI_00359227] [Bluetooth] Inquiry result will report the wrong number
 * 
 *
 * Jan 9 2007 mtk01411
 * [MAUI_00357493] [Bluetooth] After turning on Remote SIM mode, additional AUTH_CNF send to MMI
 * Modify btbmHandlerSecurityChange()
 *
 * Jan 9 2007 mtk01239
 * [MAUI_00357401] [Bluetooth] The name request of inquiry something has no responses
 * 
 *
 * Jan 8 2007 mtk01239
 * [MAUI_00356108] Bluetooth-Connect "Handsfree Profile" and "Connect Failed" will pop up, "Cancel" wil
 * 
 *
 * Jan 2 2007 mtk01411
 * [MAUI_00355519] [New Feature] Bluetooth SIM Access Profile w0701 check in
 * Add two functions
 *
 * Dec 27 2006 mtk01239
 * [MAUI_00354340] BT-superman29 always pop up "unfinished"
 * 
 *
 * Dec 27 2006 mtk01239
 * [MAUI_00353354] [1] Assert fail: 0 bt_adp_bm.c 1720 - BT
 * 
 *
 * Dec 26 2006 mtk01239
 * [MAUI_00353354] [1] Assert fail: 0 bt_adp_bm.c 1720 - BT
 * 
 *
 * Dec 25 2006 mtk01239
 * [MAUI_00353354] [1] Assert fail: 0 bt_adp_bm.c 1720 - BT
 * 
 *
 * Dec 25 2006 mtk01239
 * [MAUI_00353354] [1] Assert fail: 0 bt_adp_bm.c 1720 - BT
 * 
 *
 * Dec 18 2006 mtk01239
 * [MAUI_00351753] [Bluetooth] Turn on the BT panic to reboot BT when assert
 * 
 *
 * Dec 18 2006 mtk01239
 * [MAUI_00348589] Bluetooth-Connect the BT device "Stereo Headset" and play audio, it will not
 * 
 *
 * Dec 17 2006 mtk01239
 * [MAUI_00351601] [Bluetooth] Pairing IVT will wait for a long time and no service discovery
 * 
 *
 * Dec 8 2006 mtk01239
 * [MAUI_00349631] Bluetooth-Inquiry Audio Device repeat few times then "Cancelling Inquiry, Please Wai
 * 
 *
 * Dec 5 2006 mtk01239
 * [MAUI_00348578] [1]Assert fail:MTKBMC(bm_opactivity) = = BM_OP_NONE btbm.c 653-BT
 * 
 *
 * Dec 3 2006 mtk01239
 * [MAUI_00347951] [Bluetooth] Figure out what BT chip is used
 * 
 *
 * Nov 27 2006 mtk01239
 * [MAUI_00346134] [1] Assert fail: 0 bt_adp_bm.c 1708 - BT
 * 
 *
 * Nov 27 2006 mtk01239
 * [MAUI_00346132] [1] Assert fail: 0 BTMMICm.c 1264 - MMI
 * 
 *
 * Nov 24 2006 mtk01239
 * [MAUI_00340384] Bluetooth_SE T630 cannot send VCalendar to Superman29 successfully
 * 
 *
 * Nov 22 2006 mtk01239
 * [MAUI_00342697] BT-Bond failed and Entry passcode pop up at the same time
 * 
 *
 * Nov 16 2006 mtk01239
 * [MAUI_00343837] [Bluetooth][MT6601] Inquiry procedure can be performed when slave device of sniffer
 * 
 *
 * Nov 10 2006 mtk01239
 * [MAUI_00341550] BT_Connect A2DP then the handsfree profile will disconnect
 * 
 *
 * Nov 10 2006 mtk01239
 * [MAUI_00341453] [1] Assert fail: 0 bt_adp_bm.c 1668 - BT
 * 
 *
 * Nov 9 2006 mtk01239
 * [MAUI_00226520] Assert fail: 0 bt_adp_bm.c 1668 - BT
 * 
 *
 * Nov 9 2006 mtk01239
 * [MAUI_00226520] Assert fail: 0 bt_adp_bm.c 1668 - BT
 * 
 *
 * Nov 6 2006 mtk01239
 * [MAUI_00341019] [Bluetooth]Handle remote name len 0
 * 
 *
 * Nov 6 2006 mtk01239
 * [MAUI_00340829] Bluetooth, Report connection number to MMI
 * 
 *
 * Nov 4 2006 mtk01239
 * [MAUI_00340620] [Bluetooth] patch for Wifi coexistence turn AFH and PTA
 * 
 *
 * Nov 2 2006 mtk01239
 * [MAUI_00340120] [1] Fatal Error (4): Data Abort  lr= 0x34f9d8 - BT
 * 
 *
 * Oct 31 2006 mtk01239
 * [MAUI_00338167] [1] Assert fail: 0 BTMMIA2DP.c 400 - MMI
 * 
 *
 * Oct 30 2006 mtk01239
 * [MAUI_00337958] Bluetooth_ The screen will pop up Initializing Bluetooth now, wait a moment! whe
 * 
 *
 * Oct 30 2006 mtk01239
 * [MAUI_00336162] [1] Assert fail: g_mmi_bth_cntx.paired_dev BTMMICm.c 3304 - MMI
 * 
 *
 * Oct 30 2006 mtk01239
 * [MAUI_00336162] [1] Assert fail: g_mmi_bth_cntx.paired_dev BTMMICm.c 3304 - MMI
 * 
 *
 * Oct 25 2006 mtk01239
 * [MAUI_00337378] Audio Player-The BT device "BenQ_BHM-100" should not be selected (the device will no
 * 
 *
 * Oct 19 2006 mtk01239
 * [MAUI_00337212] [1] Assert fail: IsListCircular(&(remDev->handlers)) mecon.c 2350 - BT
 * 
 *
 * Oct 16 2006 mtk01239
 * [MAUI_00336126] [1] Assert fail: 0 BTMMICm.c 1188 - MMI
 * 
 *
 * Oct 16 2006 mtk01239
 * [MAUI_00335615] Bluetooth-Select "Delete All" then "Pairing Failed" will always pop up on deleting s
 * 
 *
 * Oct 13 2006 mtk01239
 * [MAUI_00334978] Bluetooth_ The screen will pop up Empty when inquiry new device
 * 
 *
 * Oct 12 2006 mtk01239
 * [MAUI_00335450] [1] Assert fail: IsListCircular(head) utils.c 250 - BT
 * 
 *
 * Oct 12 2006 mtk01239
 * [MAUI_00334978] Bluetooth_ The screen will pop up Empty when inquiry new device
 * 
 *
 * Oct 11 2006 mtk01239
 * [MAUI_00334978] Bluetooth_ The screen will pop up Empty when inquiry new device
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334713] [Bluetooth]Remove Report to bt_trace
 * 
 *
 * Oct 4 2006 mtk01239
 * [MAUI_00334643] [Bluetooth] fix complier warning
 * 
 *
 * Sep 28 2006 mtk01239
 * [MAUI_00333036] [1]Assert Fail:lsListCircular(entry) untils.c 229-BT
 * 
 *
 * Sep 27 2006 mtk01239
 * [MAUI_00332777] [Bluetooth] Assert failed in Meacc 226, MeSendHciCommand
 * 
 *
 * Sep 27 2006 mtk01239
 * [MAUI_00332777] [Bluetooth] Assert failed in Meacc 226, MeSendHciCommand
 * 
 *
 * Sep 27 2006 mtk01239
 * [MAUI_00332777] [Bluetooth] Assert failed in Meacc 226, MeSendHciCommand
 * 
 *
 * Sep 26 2006 mtk01239
 * [MAUI_00332604] [1]Assert fail:lsListCircular(&(bdc->link->handlers))btbm.c 1307-BT
 * 
 *
 * Sep 19 2006 mtk01239
 * [MAUI_00330851] [BT] Device name will be changed.
 * 
 *
 * Sep 15 2006 mtk01239
 * [MAUI_00329807] [1]Assert Fail:0 bt_adp_bm.c 1629-BT
 * 
 *
 * Sep 11 2006 MTK01411
 * [MAUI_00328462] [1] Assert fail: 0 me.c 506 - BT
 * Modify send role switch operation in btbmHandlerConnectInd
 *
 * Sep 8 2006 mtk01239
 * [MAUI_00328121] [1] Assert fail: remDev->encryptState == BES_END_ENCRYPT mesec.c 1014 - BT
 * 
 *
 * Sep 8 2006 mtk01239
 * [MAUI_00328121] [1] Assert fail: remDev->encryptState == BES_END_ENCRYPT mesec.c 1014 - BT
 * 
 *
 * Sep 7 2006 mtk01239
 * [MAUI_00327775] [1] Assert fail: L2Cap_GetSysPkt(TRUE) l2cap_utl.c 118 - BT
 * 
 *
 * Sep 7 2006 mtk01239
 * [MAUI_00327638] [1] Assert fail: 0 BTMMICm.c 1146 - MMI
 * 
 *
 * Sep 6 2006 mtk01239
 * [MAUI_00327638] [1] Assert fail: 0 BTMMICm.c 1146 - MMI
 * 
 *
 * Sep 5 2006 mtk01239
 * [MAUI_00327246] [BT] A2DP and AVDTP message missing
 * 
 *
 * Sep 4 2006 mtk01239
 * [MAUI_00326554] [1] Assert fail: remDev->state == BDS_DISCONNECTED me.c 2420 - BT
 * 
 *
 * Aug 21 2006 mtk01239
 * [MAUI_00322918] [BT] Interaction between BT-earphone and earphone, Assert Fail: meif.c 1051 -BT occu
 * 
 *
 * Aug 16 2006 mtk01239
 * [MAUI_00322313] [BT] Need to keyin passcode everytime.
 * 
 *
 * Aug 14 2006 mtk01239
 * [MAUI_00321407] [BT]update Bluetooth code
 * 
 *
 * Aug 8 2006 mtk01239
 * [MAUI_00215966] BT-Assert Fail: 0 bt_adp_bm.c 1679 -BT
 * 
 *
 * Aug 7 2006 mtk01239
 * [MAUI_00215286] [BT] Can't disconnect HF800 HF profile.
 * 
 *
 * Aug 7 2006 mtk01239
 * [MAUI_00215286] [BT] Can't disconnect HF800 HF profile.
 * 
 *
 * Aug 7 2006 mtk01239
 * [MAUI_00215228] [BT] update Bluetooth code
 * 
 *
 * Aug 4 2006 mtk01239
 * [MAUI_00215228] [BT] update Bluetooth code
 * 
 *
 * Jul 31 2006 mtk01239
 * [MAUI_00214015] [BT] update BT code and add OPP, FTP SDP record
 * 
 *
 * Jul 26 2006 mtk01239
 * [MAUI_00212407] [BT] Assert Fail: status ==BT_STATUS_SUCCESS bt_adp_avrcp.c 212 - BT
 * 
 *
 * Jul 19 2006 mtk01239
 * [MAUI_00211879] [Bluetooth][ESI]update Bluetooth codes for IOT with HS820
 * 
 *
 * Jul 16 2006 mtk01239
 * [MAUI_00210782] [Bluetooth][ESI]update Bluetooth codes
 * 
 *
 * Jun 12 2006 mtk01239
 * [MAUI_00201555] [Bluetooth][ESI] update bt code
 * 
 *
 * Jun 5 2006 mtk01239
 * [MAUI_00199721] [Bluetooth][ESI] update bt code update
 * 
 *
 * May 22 2006 mtk01239
 * [MAUI_00195772] [Bluetooth] ESI Bluetooth path for A2DP and AVDTP
 * 
 *
 * May 8 2006 mtk01239
 * [MAUI_00192291] [Bluetooth][ESI]Update CM and BM code
 * 
 *
 * Apr 24 2006 mtk01239
 * [MAUI_00189553] ESI Bluetooth project update
 * update
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!! 
 *==============================================================================
 *******************************************************************************/
#include "bt_common.h"
#include "bt_feature.h"
#include "bluetooth_gap_struct.h"
#include "ddb.h"
#include "bttypes.h"
#include "btalloc.h"
#include "btconfig.h"
#include "sec.h"
#include "osapi.h"
#include "sdp.h"
#include "btproperty.h"
#include "btbm_adp.h"
#include "btbm.h"
#include "sdap.h"
#include "btaccctrl.h"
#include "osapi.h"
#include "btbm_int.h"
#include "bt_adp_bm.h"
#ifdef __BT_4_0_BLE__
#include "BtbmLeAdv.h"
#endif
#if defined (__BT_2_1_BLOCK_DEVICE__)
#include "btbmblocklist.h"
#endif 

#ifdef MTK_BB_TESTING
#include "BtBBtesting.h"
#endif 

#if !defined(__MTK_TARGET__)
#define BTBM_TIMER_TICKS (50)
#else 
#define BTBM_TIMER_TICKS (4.615)
#endif 

/***************
Internal Globals 
****************/
/* MTK Bluetooth Manager Context */
BMCONTENT_T BMController = {0};
BtOperation audioSettingOp = {{NULL, NULL}, BOP_NOP};

#define MTKBMC(f)  (BMController.f)

BtHandler globalbtbmHandler;
BtHandler linkbmHandler;
U8 bTLocalNameType[4];

/***************************
Internal function prototypes
****************************/
void btbm_dummy_callback(BMCallbackParms *Parms);

/*********************
Function Definitions 
**********************/
#if (BT_READ_RSSI == XA_ENABLED)
extern void btbmReportReadRSSIResult(const BtEvent* Event);
#endif

extern void SDAP_Register(BMCallback Callback);
extern U32 BtbmAdvLeGetAdvIndex(void);


/*****************************************************************************
 * FUNCTION
 *  BTBMInit
 * DESCRIPTION
 *  This function is used to initialize the context of btbm.
 * PARAMETERS
 *  void
 * RETURNS
 *  init result
 *****************************************************************************/
BOOL BTBMInit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ME_InitHandler(&globalbtbmHandler);
    globalbtbmHandler.callback = Me2BMCallback;
    ME_InitHandler(&linkbmHandler);
    linkbmHandler.callback = DummyLinkCallback;
    OS_MemSet((U8*) & BMController, 0, sizeof(BMController));
    /* BT_State_Report2WLAN(0, BT_STATE_INQUIRY); */
    MTKBMC(bmCB) = btbm_dummy_callback;
    #ifdef __BT_USE_CUST_DATA__
    MTKBMC(inquiryShoAddress) = (U32)btGetCustvalue(CUSTID_GAP_INQUIRY_SHOW_ADDRESS);
    #else
    MTKBMC(inquiryShoAddress) = 1;
    #endif
    if (ME_RegisterGlobalHandler(&globalbtbmHandler) == BT_STATUS_SUCCESS)
    {
        if (ME_SetEventMask(&globalbtbmHandler, BEM_ALL_EVENTS) != BT_STATUS_SUCCESS)
        {
            return FALSE;
        }
        SEC_RegisterPairingHandler(&globalbtbmHandler);
        SEC_RegisterAuthorizeHandler(&globalbtbmHandler);
        ME_RegisterAcceptHandler(&linkbmHandler);
        MTKBMC(bm_opactivity) = BM_OP_NONE;
        MTKBMC(bonding_state) = BM_BONDING_NONE;
        /* We assume system initialize, the callback function type is the NORMAL type, not JSR 82 */
        MTKBMC(inquiry_with_name) = TRUE;
        MTKBMC(bm_security_mode) = BSM_SEC_LEVEL_1;
        MEC(classOfDevice) = BT_DEFAULT_COD;
        ME_SetClassOfDevice(BT_DEFAULT_COD);

        MTKBMC(bm_scan_mode) = BM_SCAN_MODE_SET_NORMAL;
        MTKBMC(bm_scan_opactivity) = BM_OP_NONE;
        MTKBMC(power_on_off_mode) = BM_POWER_ON;
        MTKBMC(link_allow_state) = BM_LINK_DISALLOW;
        MTKBMC(wait_conection_acp_rsp) = BM_LINK_ACCEPT_CMD_AUTO;
        MTKBMC(write_authentication_state) = BM_WRITE_AUTHENTICATION_NONE;
        bm_adp_event_init();
#ifdef __BT_4_0_BLE__
        BTBMGATT_Init();
        BtbmLeAdvInit();
        MTKBMC(LeAdvIndex) = BtbmAdvLeGetAdvIndex();
#endif
        MTKBMC(scoData) = 0;

        return TRUE;
    }
    else
    {
        return FALSE;
    }

}


/*****************************************************************************
 * FUNCTION
 *  Me2BMCallback
 * DESCRIPTION
 *  This function is register to ME.
 *  When ME has events, the Me2BMCallback will also be called
 * PARAMETERS
 *  Event       [IN]        Event of ME layer
 * RETURNS
 *  void
 *****************************************************************************/
void Me2BMCallback(const BtEvent *Event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (Event->eType)
    {
        case BTEVENT_INQUIRY_RESULT:
            btbmHandlerInquiryResult(Event);
            break;
        case BTEVENT_INQUIRY_COMPLETE:
            btbmHandlerInquiryComplete(Event);
            break;
#ifdef __BT_4_0_BLE__
        case BTEVENT_BLE_ADV_RESULT:
            BtbmHandlerBLEAdvEvent(Event);
            break;
#endif
        case BTEVENT_INQUIRY_CANCELED:
            btbmHandlerInquiryCancel(Event);
            break;
        case BTEVENT_LINK_CONNECT_IND:
            btbmHandlerConnectInd(Event);
            btbmHandleReportLinkState(Event->p.remDev);
            break;
        case BTEVENT_LINK_DISCONNECT:
#ifdef __BT_4_0_BLE__
            btbmGattHandleRemNameReqTimeoutCompleted(&Event->p.remDev->bleNameReqTimer);
			EVM_ResetTimer(&Event->p.remDev->bleServiceRequestDisconnectTimer);
#endif
            btbmHandlerDisconnectInd(Event);
            btbmHandleReportLinkState(Event->p.remDev);
            break;
        case BTEVENT_LINK_CONNECT_CNF:
            btbmHandlerConnectCfm(Event);
            btbmHandleReportLinkState(Event->p.remDev);
            break;
#ifdef __BT_4_0_BLE__
        case BTEVENT_GATT_REDAY_TO_ASK_NAME:
//            btbmHandlerGattReadyToAskDeviceName(Event->p.remDev);
            break;
#endif
        case BTEVENT_LINK_CONNECT_REQ:
            bt_trace(BT_TRACE_G1_PROTOCOL, BT_BTBM_LINK_CONN_REQ);
            btbmHandleLinkConnectReq(Event->p.remDev);
            //ME_AcceptIncomingLink(Event->p.remDev, BCR_SLAVE);
            break;
        case BTEVENT_ACCESSIBLE_CHANGE:
            btbmHandlerAccessModeChange(Event);
            break;
        case BTEVENT_LOCAL_NAME_CHANGE:
            btbmHandlerChangeLocalName(Event);
            break;
        case BTEVENT_SECURITY_CHANGE:
            btbmHandlerSecurityChange(Event);
            break;
        case BTEVENT_LINK_CREATE_CANCEL:
            btbmHandlerLinkCreateCancel(Event);
            break;
        case BTEVENT_PIN_REQ:
            btbmHandlerPinCodeRequest(Event);
            break;
        case BTEVENT_PAIRING_COMPLETE:
            btbmHandlerPairingComplete(Event);
            break;
        case BTEVENT_AUTHENTICATE_CNF:
            btbmHandlerAuthenticatedCfm(Event);
            break;
        case BTEVENT_LINK_KEY_DELETED:
            btbmHandlerLinkkeyDeleted(Event);
            break;
        case BTEVENT_MODE_CHANGE:
            btbmHandleCheckPiconetState();
            btbmHandleModeCfm(Event);
            break;
 #if (BT_READ_RSSI == XA_ENABLED)
        case BTEVENT_READ_RSSI_COMPLETED:
            btbmReportReadRSSIResult(Event);
            break;
 #endif           
    #if defined (__BT_2_1_SIMPLE_PAIRING__)
        case BTEVENT_IO_CAPABILITY_REQUEST:
            bt_trace(TRACE_GROUP_1, BTLOG_BTEVENT_IO_CAPABILITY_REQUEST);
            btbmHandleIoCapabilityRequest(Event->p.remDev);
            break;
        case BTEVENT_IO_CAPABILITY_RESPONSE:
            bt_trace(TRACE_GROUP_1, BTLOG_BTEVENT_IO_CAPABILITY_RESPONSE);
            bt_trace(
                TRACE_GROUP_1,
                BTLOG_BTEVENT_IO_CAPABILITY_RESPONSE_IOCAPABILITYxD,
                Event->p.ioCapabilityResponse.ioCapability);
            bt_trace(
                TRACE_GROUP_1,
                BTLOG_BTEVENT_IO_CAPABILITY_RESPONSE_OOBDATAPRESENTxD,
                Event->p.ioCapabilityResponse.OOBDataPresent);
            bt_trace(
                TRACE_GROUP_1,
                BTLOG_BTEVENT_IO_CAPABILITY_RESPONSE_AUTHREQUIREMENTSxD,
                Event->p.ioCapabilityResponse.authRequirements);
            btbmHandleIoCapabilityResponse(Event);
            break;
        case BTEVENT_USER_CONFIRM_REQUEST:
            bt_trace(TRACE_GROUP_1, BTLOG_BTEVENT_USER_CONFIRM_REQUEST);
            btbmHandleUserConfirmRequest(Event);
            break;
        case BTEVENT_PASSKEY_NOTIFICATION:
            btbmHandlePasskeyEntryNotification(Event);
            break;
        case BTEVENT_KEYPRESS_EVENT:
            btbmHandlePasskeyPressIndication(Event);
            break;
        case BTEVENT_OOB_DATA_REQUEST:
            btbmHandleOobDataRequest(Event);
            break;
        case BTEVENT_SIMPLE_PAIRING_COMPLTETE:
            btbmHandlerSimplePairingComplete(Event);
            break;
    #endif /* defined (__BT_2_1_SIMPLE_PAIRING__) */ 

        case BTEVENT_ENCRYPTION_CHANGE:
            btbmHandleEncryptionCfm(Event);
            break;
        case BTEVENT_ROLE_CHANGE:
            btbmHandleRoleSwitchCfm(Event);
            btbmHandleCheckPiconetState();
            break;
        case BTEVENT_CHECK_ALL_LINK_POWER_OFF:
            if (MTKBMC(power_on_off_mode) != BM_POWER_ON)
            {
                btbmHandlerCheckAllLinkPowerOffStatus();
            }            
            break;
        default:
            break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  btbm_dummy_callback
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Parms       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void btbm_dummy_callback(BMCallbackParms *Parms)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
}


/*****************************************************************************
 * FUNCTION
 *  BTBM_Register
 * DESCRIPTION
 *  
 * PARAMETERS
 *  Callback        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void BTBM_Register(BMCallback Callback)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    MTKBMC(bmCB) = Callback;
    SDAP_Register(Callback);
}
