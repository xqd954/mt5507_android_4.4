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


#include "HDCP2xDef.h"
#include "HDCPTxModule.h"

namespace android {

#define LIB_VERSION4(lev0, lev1, lev2, lev3) ((lev0<<24 & 0xff000000) | \
                                              (lev1<<16 & 0x00ff0000) | \
                                              (lev2<<8  & 0x0000ff00) | \
                                              (lev3 & 0x000000ff))

#define HDCP_TX_LIB_VL0 0       // Tee config
#define HDCP_TX_LIB_VL1 0       // key config
#define HDCP_TX_LIB_VL2 0       
#define HDCP_TX_LIB_VL3 1       // release version

HDCPTxModule::~HDCPTxModule()
{
    if (mHdcpTx != NULL) 
    {
        mHdcpTx.clear();
    }
}


HDCPTxModule::HDCPTxModule(void *cookie, HDCPModule::ObserverFunc observerNotify):HDCPModule(cookie, observerNotify)
{
    mHdcpTx = new WFDHDCPTx(cookie, observerNotify);

    mCookie = cookie;
    mObserverNotify = observerNotify;

#ifdef HDCP2_1_TX_SUPPORT    
    HDCP2X_DBG(HDCP2XDBG_VERB, "MTK HDCP_TX 2.1 Ver = 0x%x", LIB_VERSION4(HDCP_TX_LIB_VL0, HDCP_TX_LIB_VL1, HDCP_TX_LIB_VL2,HDCP_TX_LIB_VL3));
#else
    HDCP2X_DBG(HDCP2XDBG_VERB, "MTK HDCP_TX 2.0 Ver = 0x%x", LIB_VERSION4(HDCP_TX_LIB_VL0, HDCP_TX_LIB_VL1, HDCP_TX_LIB_VL2,HDCP_TX_LIB_VL3));
#endif
}



status_t HDCPTxModule::initAsync(const char *host, unsigned port)
{
    status_t err;
    
    HDCP2X_FUNC_LOG();

    err = mHdcpTx->startAuthentication(host, port);
    if (err != HDCP2X_OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "startAuthentication fail = %d", err);
        return (err);
    }
    else
    {
        HDCP2X_DBG(HDCP2XDBG_VERB, "startAuthentication success");
    }

    HDCP2X_FUNC_EXIT_LOG();

	return (HDCP2X_OK);
}

// Request to shutdown the active HDCP session.
status_t HDCPTxModule::shutdownAsync()
{
    status_t err;

    HDCP2X_FUNC_LOG();

    err = mHdcpTx->shutdownAsync();
    if (err != HDCP2X_OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "shutdownAsync fail = %d", err);
        return (err);
    }

    HDCP2X_FUNC_EXIT_LOG();

	return (HDCP2X_OK);
}


// Encrypt a data according to the HDCP spec. The data is to be
// encrypted in-place, only size bytes of data should be read/write,
// even if the size is not a multiple of 128 bit (16 bytes).
// This operation is to be synchronous, i.e. this call does not return
// until outData contains size bytes of encrypted data.
// streamCTR will be assigned by the caller (to 0 for the first PES stream,
// 1 for the second and so on)
// inputCTR will be maintained by the callee for each PES stream.
status_t HDCPTxModule::encrypt(
            const void *inData, size_t size, uint32_t streamCTR,
            uint64_t *outInputCTR, void *outData)
{
    status_t err;

    //HDCP2X_FUNC_LOG();

    err = mHdcpTx->encrypt(inData, size, streamCTR, outInputCTR, outData);
    if (err != HDCP2X_OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "encrypt fail = %d", err);
        return (err);
    }

    //HDCP2X_FUNC_EXIT_LOG();

	return 0;
}

}

extern "C" android::HDCPModule *createHDCPModule(void *cookie, android::HDCPModule::ObserverFunc Func)
{
    HDCP2X_FUNC_LOG();

    // create component instance
    android::HDCPModule *pHdcpModule = new android::HDCPTxModule(cookie, Func);
    if (NULL == pHdcpModule)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "createHDCPModule out of memory!!!");
        return NULL;
    }

	return (pHdcpModule);
}
