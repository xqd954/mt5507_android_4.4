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

#include <utils/Log.h>
#include "bt_adp_ashm.h"

/**
 * usage example
 */
int main(int argc, char** argv)
{
	char* opp = "com.mtk.bt.profile.opp";
	char* ftp = "com.mtk.bt.profile.ftp";
	android::sp<android::IMemoryHeap> oppmem;
	android::sp<android::IMemoryHeap> ftpmem;
	unsigned char *buffer;

	if( argc == 1 ){

		// server mode
		ALOGI("[bt_ashm] start service x 2");
		btmtk_ashm_start_service();
		btmtk_ashm_malloc_memory(ftp, 204);
		for(;;){
			sleep(5);
		}
	}
	else
	{
		// client mode
		ALOGI("[bt_ashm] client access");

		// change ftp
		ftpmem = btmtk_ashm_get_memory(ftp);
		buffer = (unsigned char*)ftpmem->getBase();
		ALOGI("[bt_ashm] client access: mem[%d]", buffer);
		if (buffer != (unsigned char*)-1)
		{
			ALOGI("[bt_ashm] client access[%s] - old: ptr=%p val=0x%x\n", ftp, buffer, *buffer);
			*buffer = (*buffer)+2;
			ALOGI("[bt_ashm] client access[%s] - new: ptr=%p val=0x%x\n", ftp, buffer, *buffer);
		}
		else
		{
			ALOGE("[bt_ashm] error: shared memory not available\n");
		}
	}

	return 0;
}
