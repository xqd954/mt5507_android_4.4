/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include "string.h"
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_mmi_msg.h"
#include "bluetooth_struct.h"
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <unistd.h>
#include "cutils/sockets.h"
#include "utils/String8.h"
#include <AudioSystem.h>

using  namespace android;

extern "C"{
int enableTestMode(int enable, unsigned char *addr);
int queryTestMode();
int writeBTAddr(unsigned char *addr);
int readBTAddr(unsigned char *addr);
}

int setAudioParameter(String8& param){
    return AudioSystem::setParameters(0, param);
}

int main (int argc, char **argv)
{
    int cmd, param;
    int ret = -1;
    if(argc < 3) {
        printf("No command is specified\n");
        return -1;
    }else {
        cmd = atol(argv[1]);
        param = atol(argv[2]);
    }
    printf("command=%d, parameter=%d\n", cmd, param);
    
    switch(cmd)
    {
    case 1:
    {
        unsigned char addr[6] = {8,7,6,5,4,3};
        if(param == 0){
            /* Read BD address */
            ret = readBTAddr(addr);
            if(ret < 0){
                printf("Read BD address failed\n");
            }else{
                printf("Read BD address success. BD addr=%02X%02X%02X%02X%02X%02X\n",
                        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
            }
        }else{
            /* Write BD address */
            ret = writeBTAddr(addr);
            if(ret < 0){
                printf("Write BD address failed 1\n");
            }else{
                ret = readBTAddr(addr);
                if(ret < 0){
                    printf("Write BD address failed 2\n");
                }else{
                    printf("Write BD address success. BD addr=%02X%02X%02X%02X%02X%02X\n",
                        addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
                }
            }
        }
    }
        break;
    case 2:
    {
        /* Test mode */
        if(param == 0){
            /* Leave test mode */
            ret = enableTestMode(0, NULL);
            if(ret >= 0){
                //ret = setAudioParameter(String8("SET_LOOPBACK_TYPE=1"));
                ret = AudioSystem::setParameters(0, String8("SET_LOOPBACK_TYPE=0"));
                printf("setAudioParameter(String8(\"SET_LOOPBACK_TYPE=0\"))=%d", ret);       
            }
        }else if(param == 1){
            /* Enter test mode */
            unsigned char addr[6] = {0x00, 0x05, 0xC9,0x00, 0x00, 0x00};
            ret = enableTestMode(1, addr);
            if(ret >= 0){
                //ret = setAudioParameter(String8("SET_LOOPBACK_TYPE=1"));
                ret = AudioSystem::setParameters(0, String8("SET_LOOPBACK_TYPE=31"));
                printf("setAudioParameter(String8(\"SET_LOOPBACK_TYPE=31\"))=%d", ret);
            }
        }else{
            printf("query test mode: mode=%d", queryTestMode());
            return 1;
        }
        if(ret < 0){
            printf("Enter test mode failed : mode=%d\n", param);
        }
    }
        break;
    default:
        printf("Wrong cpommand : %d\n", cmd);
        break;
    }
    return 0;
}

