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

#ifndef _HDCP_TX_MODULE_H_
#define _HDCP_TX_MODULE_H_

#include "HDCPAPI.h"
#include "WFDHDCPTx.h"

namespace android {

class HDCPTxModule : public HDCPModule
{

public:
    ~HDCPTxModule();

public:
    // Module can call the notification function to signal completion/failure
    // of asynchronous operations (such as initialization) or out of band
    // events.
    HDCPTxModule(void *cookie, HDCPModule::ObserverFunc observerNotify);


    // Request to setup an HDCP session with the specified host listening
    // on the specified port.
    status_t initAsync(const char *host, unsigned port);

    // Request to shutdown the active HDCP session.
    status_t shutdownAsync();

    // Encrypt a data according to the HDCP spec. The data is to be
    // encrypted in-place, only size bytes of data should be read/write,
    // even if the size is not a multiple of 128 bit (16 bytes).
    // This operation is to be synchronous, i.e. this call does not return
    // until outData contains size bytes of encrypted data.
    // streamCTR will be assigned by the caller (to 0 for the first PES stream,
    // 1 for the second and so on)
    // inputCTR will be maintained by the callee for each PES stream.
    status_t encrypt(
            const void *inData, size_t size, uint32_t streamCTR,
            uint64_t *outInputCTR, void *outData);

private:
    HDCPTxModule(const HDCPTxModule &);
    HDCPTxModule &operator=(const HDCPTxModule &);

    void *mCookie;
    HDCPModule::ObserverFunc mObserverNotify;

    sp<WFDHDCPTx> mHdcpTx;
    
};

}

#endif //~MTK_OMX_AAC_DEC_H

