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

/* 
 * bt_spp_os.c
 * 
 * This file is the OS dependent API of SPP profile used by profile & ADP.
 * Flow direction: profile --> OS dependent API
 * 			     ADP --> OS dependent API
 */


#include "spp.h"
#include "bt_os_api.h"
#include "osapi.h"
#include "bluetooth_spp_struct.h"

/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Local function prototypes
 *
 ****************************************************************************/
#ifdef __LINUX_SUPPRESS_ERROR__
kal_bool CheckCustomSPPFlowControlSetting(module_type owner)
{
	return KAL_FALSE;
}
#else
extern kal_bool CheckCustomSPPFlowControlSetting(module_type owner);
#endif
/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
//Maxium length for this pattern is 20
//For example: this pattern could be MTKGameStartReq
//kal_uint8 SPPClientFirstConnDataPattern[20]={"MTKSPPForMMI"};
//This len is actually length of this pattern
//kal_uint8 SPPClientFirstConnDataPatternLen = 12;

extern SPPCallback SPPCB;
#ifdef __SPP_THROUGHPUT_TEST__
extern U32 g_sppThroughputTotalGotLength;
#endif

/****************************************************************************
 *
 * The following functions are APIs that the OS layer (this layer) must
 * provide to the portable SPP.
 *
 ***************************************************************************/

/*---------------------------------------------------------------------------
 */

#if (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED)


/******************************************************************************************
 * FUNCTION
 *  CheckSPPFlowControlSetting
 * DESCRIPTION
 *  According the the module name to check if this module is necessary to enable SPP Flow Control Mechanism
 * PARAMETERS
 *  owner             [IN]          
 * RETURNS
 *  BOOL
 ******************************************************************************************/
kal_bool CheckSPPFlowControlSetting(module_type owner)
{
    /* Tasks which needs to enable SPP Flow Control Mechanism can be added here */
    /* If the customer wants to develop their SPP-based application and change uart data flow to other module: */
    /* It can invoke the function defined in customer folder and configured by customer */
    if ((owner == MOD_MMI)
#ifdef __DT_SUPPORT__        
        ||(owner == MOD_DT)
#endif        
        )
    {
        kal_trace(TRACE_GROUP_1,BT_SPP_Check_FLOW_CTRL_SETTING_CASE,0);
        return KAL_TRUE;
    }
    else if (CheckCustomSPPFlowControlSetting(owner))
    {
        /* For SPP Srv or Client Applications development by customer, the module must be filled in CheckCustomSPPFlowControlSetting() */
        kal_trace(TRACE_GROUP_1,BT_SPP_Check_FLOW_CTRL_SETTING_CASE,1);
        return KAL_TRUE;
    }
    else
    {
        return KAL_FALSE;
    }
}
    

/*****************************************************************************
 * FUNCTION
 *  SPPOS_SendUartReadyToReadInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_SendUartReadyToReadInd(UART_PORT port, module_type owner)
{
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;  
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)  /* SPP_PORTING */
    SPP_DEV_OS *vp_ptr = NULL;
   
    vp_ptr = getDev(port);
#endif    
    parms.event = UART_EVENT_READY_TO_READ_IND;
    parms.dstMod = owner;
    parms.p.uart_ready_to_read.port = port;
#if defined (BTMTK_ON_WIN32) || defined (BTMTK_ON_WISESDK)    /* SPP_PORTING */
    parms.p.uart_ready_to_read.profile = vp_ptr->profile;
#endif
    SPPCB(&parms);
/* #else  #ifdef CUSTOM_SPP */                                     
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_SendUartReadyToWriteInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  port        [IN]        
 *  owner       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_SendUartReadyToWriteInd(UART_PORT port, module_type owner)
{
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;  
    SPP_DEV_OS *vp_ptr = NULL;
   
    vp_ptr = getDev(port);
    parms.event = UART_EVENT_READY_TO_WRITE_IND;
    parms.dstMod = owner;
    parms.p.uart_ready_to_write.port = port;
    parms.p.uart_ready_to_write.profile = vp_ptr->profile;
    SPPCB(&parms);
/* #else #ifdef CUSTOM_SPP */       
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_LockDev
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_LockDev(void *dev)
{
    return;
}

/*---------------------------------------------------------------------------
 */


/*****************************************************************************
 * FUNCTION
 *  SPPOS_UnlockDev
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_UnlockDev(void *dev)
{
    return;
}

/*---------------------------------------------------------------------------
 */


/*****************************************************************************
 * FUNCTION
 *  SPPOS_Wait
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 *  ms      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPPOS_Wait(void *dev, U16 ms)
{
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 */


/*****************************************************************************
 * FUNCTION
 *  SPPOS_Resume
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_Resume(void *dev)
{
    return;
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_Ioctl
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev             [IN]        
 *  function        [IN]        
 *  arg             [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPPOS_Ioctl(void *dev, U16 function, void *arg)
{
    BtStatus rc = BT_STATUS_SUCCESS;

    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_DevRx
 * DESCRIPTION
 *  This function is called by SPP for when characters have been received from RFCOMM.
 *  Put bytes in the device ring buffer.
 * PARAMETERS
 *  dev     [IN]     
 *  buf     [IN]     
 *  len     [IN]     
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPPOS_DevRx(void *dev, char buf[], U16 *len)
{
    BtStatus rc = BT_STATUS_SUCCESS;

    S16 actual_write_len = 0, wanted_to_write_len = *len;

    actual_write_len = RING_WriteData(((SPP_DEV_OS*) dev)->rRing, buf, *len);

    /*
     * Add by mtk01411: 2007-1015 
     * * Because the creadit is computed as Ring's size - total_data_len:
     * * If this RFCOMM's pkt is received, it means that the data contained in this pkt can be put to Ring Buf!
     * * It is necessary to check if there is still free space: If it has, it means the pWrite pointer is wrapped to start position
     * * It must copy the left data in the pkt to Ring Buf continusly
     */
    if (RING_FreeSpace(((SPP_DEV_OS*) dev)->rRing))
    {
        /*
         * In this case, because pWrite pointer is wrapped to start poisiton of buffer at last RING_WirteData()
         * * So if there is still free space and the wanted_to_write_len is larger than actual_write_len of last RING_WirteData(): 
         */
        if (actual_write_len < wanted_to_write_len)
        {
            RING_WriteData(((SPP_DEV_OS*) dev)->rRing, buf + actual_write_len, (S16) (wanted_to_write_len - actual_write_len));
        }
    }

#ifndef BTMTK_ON_WISE /* SPP_PORTING */
    SPPOS_SendUARTDataAvailableInd(((SPP_DEV_OS*) dev)->port);
#endif  /* BTMTK_ON_WISE */

#ifdef __SPP_THROUGHPUT_TEST__
    g_sppThroughputTotalGotLength += *len;
    RING_BufFlush(((SPP_DEV_OS*) dev)->rRing);
#endif

    if (((SPP_DEV_OS*) dev)->readyToRead == KAL_TRUE)
    {
        if (((SPP_DEV_OS*) dev)->set_auth_req_mode == KAL_FALSE)
        {

            kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SET_READYTOREAD_FALSE);
            ((SPP_DEV_OS*) dev)->readyToRead = KAL_FALSE;
            SPPOS_SendUartReadyToReadInd(((SPP_DEV_OS*) dev)->port, ((SPP_DEV_OS*) dev)->owner);
        }

    }
    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_DevTx
 * DESCRIPTION
 *  Return a buffer of data to be transmitted via RFCOMM
 *  Instead of allocating a buffer and copying data from the ring buffer,
 *  return a pointer into the ring buffer.
 *  This assumes that SPPOS_DevTx will not be called again without an interceding call to SPPOS_ReturnBuf.
 *  Note that buf will be returned to us via SPPOS_ReturnBuf.
 * PARAMETERS
 *  dev     [IN]        
 *  buf     [IN]        
 *  len     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
BtStatus SPPOS_DevTx(void *dev, char **buf, U16 *len)
{
    BtStatus rc = BT_STATUS_SUCCESS;

    /* modify this function to approve performance */
    RING_GetDataPtr(((SPP_DEV_OS*) dev)->wRing, buf, (S16*) len);
    /* Check if (*buf) points to a NULL Address */
    Assert((*buf));
    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_RxBytes
 * DESCRIPTION
 *  Return number of bytes received from RFCOMM that have not been read.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 SPPOS_RxBytes(void *dev)
{
    return RING_DataLen(((SPP_DEV_OS*) dev)->rRing);
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_RxFree
 * DESCRIPTION
 *  Report number of free bytes in ring buffer.
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
U16 SPPOS_RxFree(void *dev)
{
    return RING_FreeSpace(((SPP_DEV_OS*) dev)->rRing);
}

/*---------------------------------------------------------------------------
 */


/*****************************************************************************
 * FUNCTION
 *  SPPOS_FlushRx
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_FlushRx(void *dev)
{
    kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPPOS_FLUSHRX);
    RING_BufFlush(((SPP_DEV_OS*) dev)->rRing);
}

/*---------------------------------------------------------------------------
 */


/*****************************************************************************
 * FUNCTION
 *  SPPOS_FlushTx
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_FlushTx(void *dev)
{
    kal_trace(TRACE_GROUP_1,BT_RFCOMMM_SPPOS_FLUSHTX);
    RING_BufFlush(((SPP_DEV_OS*) dev)->wRing);
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_CheckReadyToWrite
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]     
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_CheckReadyToWrite(void *dev)
{
    if ((((SPP_DEV_OS*) dev)->notifyReadyToWrite))
    {
        ((SPP_DEV_OS*) dev)->notifyReadyToWrite = KAL_FALSE;
        SPPOS_SendUartReadyToWriteInd(((SPP_DEV_OS*) dev)->port, ((SPP_DEV_OS*) dev)->owner);
    }
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_ReturnBuf
 * DESCRIPTION
 *  Called when the buffer returned by SPPOS_DevTx is relinguished by the driver
 * PARAMETERS
 *  dev     [IN]         
 *  buf     [IN]         
 *  len     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_ReturnBuf(void *dev, char *buf, U16 len)
{

    RingBuf *ring = ((SPP_DEV_OS*) dev)->wRing;

    RING_BufDelete(((SPP_DEV_OS*) dev)->wRing, len);

    /* Add by mtk01411: 2007-1015 */
    if (ring->pRead == ring->pWrite)
    {
        ((SPP_DEV_OS*) dev)->is_tx_empty = KAL_TRUE;
    }

#ifdef __SPP_ERR_HANDLE_FOR_APP__
    if (RING_FreeSpace() == 0)
    {
        ((SPP_DEV_OS*) dev)->notifyReadyToWrite = KAL_TRUE;
    }
#endif /* __SPP_ERR_HANDLE_FOR_APP__ */ 
    /* Check if the sender task sets notifyReadyToWrite flag as TRUE or not: 2007-1015 by mtk01411 */
    SPPOS_CheckReadyToWrite(dev);
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_DevTxEmpty
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_DevTxEmpty(void *dev)
{
    ((SPP_DEV_OS*) dev)->is_tx_empty = TRUE;
}

#ifdef __BT_SPP_SUPPORT_IOCTRL_SETTINGS__
/*---------------------------------------------------------------------------
 * ioctlPortSettings() 
 *      This is part of ioctl(). The portsettings are handled simarily
 *      so they are segregated here to reduce code.
 *
 * Requires:
 *     
 * Parameters:
 *      dev         pointer to device structure 
 *      function    ioctl function
 *      arg         arg appropriate for 'function'
 *
 * Returns:
 *      BT_STATUS_SUCCESS
 *      BT_STATUS_FAILED
 *      SPPOS_Ioctl is allowed to return other values more meaningful
 */


/*****************************************************************************
 * FUNCTION
 *  ioctlPortSettings
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev             [IN]        
 *  function        [IN]        
 *  arg             [IN]        
 * RETURNS
 *  
 *****************************************************************************/
static BtStatus ioctlPortSettings(void *dev, U16 function, void *arg)
{
    BtStatus status = BT_STATUS_FAILED;

    return status;
}
#endif /* __BT_SPP_SUPPORT_IOCTRL_SETTINGS__ */ 


/*****************************************************************************
 * FUNCTION
 *  SPPOS_ConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 *  cid     [IN]        
 *  lap     [IN]        
 *  uap     [IN]        
 *  nap     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_ConnectInd(void *dev, U8 cid, U32 lap, U8 uap, U16 nap)
{
/*  #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;

    parms.event = SPP_EVENT_CONNECT_IND;
    parms.dstMod = ((SPP_DEV_OS*) dev)->owner;
    parms.p.spp_connect_ind.port = ((SPP_DEV_OS*) dev)->port;
    parms.p.spp_connect_ind.cid = cid;
    parms.p.spp_connect_ind.lap = lap;
    parms.p.spp_connect_ind.uap = uap;
    parms.p.spp_connect_ind.nap = nap;    
    SPPCB(&parms);
/* #else   CUSTOM_SPP */


    if (((SPP_DEV_OS*) dev)->profile == SPP_PROFILE)
    {
/* #ifdef CUSTOM_SPP */
        parms.event = SPP_EVENT_CONNECT_IND;
        parms.dstMod = MOD_MMI;
        parms.p.spp_connect_ind.port = ((SPP_DEV_OS*) dev)->port;
        parms.p.spp_connect_ind.cid = cid;
        parms.p.spp_connect_ind.lap = lap;
        parms.p.spp_connect_ind.uap = uap;
        parms.p.spp_connect_ind.nap = nap;    
        SPPCB(&parms);
/* #else   #ifdef CUSTOM_SPP */
    }

#if DUN_SERVER == XA_ENABLED
    else if (((SPP_DEV_OS*) dev)->profile == DUN_PROFILE)
    {

    /* #ifdef CUSTOM_SPP */
        SPPCallbackParms parms;   
        parms.event = DUN_EVENT_CONNECT_IND;
        parms.dstMod = MOD_MMI;
        parms.p.dun_connect_ind.port = ((SPP_DEV_OS*) dev)->port;
        parms.p.dun_connect_ind.cid = cid;
        parms.p.dun_connect_ind.lap = lap;
        parms.p.dun_connect_ind.uap = uap;
        parms.p.dun_connect_ind.nap = nap;        
        SPPCB(&parms);    
    /* #else #ifdef CUSTOM_SPP */
    }
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED
    else if (((SPP_DEV_OS*) dev)->profile == FAX_PROFILE)
    {
        SPPCallbackParms parms;   
        parms.event = FAX_EVENT_CONNECT_IND;
        parms.dstMod = MOD_MMI;
        parms.p.fax_connect_ind.port = ((SPP_DEV_OS*) dev)->port;
        parms.p.fax_connect_ind.cid = cid;
        parms.p.fax_connect_ind.lap = lap;
        parms.p.fax_connect_ind.uap = uap;
        parms.p.fax_connect_ind.nap = nap;        
        SPPCB(&parms);    
    }
#endif /* FAX_SERVER == XA_ENABLED */
}


/*****************************************************************************
 * FUNCTION
 *  SPPOS_DisConnectInd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dev     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void SPPOS_DisConnectInd(void *dev)
{
/* #ifdef CUSTOM_SPP */
    SPPCallbackParms parms;
/* #endif #ifdef CUSTOM_SPP */                                            

/* #ifdef CUSTOM_SPP */
    parms.p.uart_plugout_ind.port = ((SPP_DEV_OS*) dev)->port;
/* #else #ifdef CUSTOM_SPP */

    //2008-0930: If uart_owner is not L4C, then Set this wait_plugout_cnf_msg = KAL_TRUE and waits for MSG_ID_BT_SPP_UART_PLUGOUT_CNF from MMI later 
    //When BT receives MSG_ID_BT_SPP_UART_PLUGOUT_CNF, it can change owner of this port to MOD_ATCI
#ifndef __GEMINI__
    if (((SPP_DEV_OS*) dev)->owner != MOD_ATCI)
#else 
    if ((((SPP_DEV_OS*) dev)->owner != MOD_ATCI) && (((SPP_DEV_OS*) dev)->owner != MOD_ATCI_2))
#endif 
    {
        kal_bool Enable_SPP_FlowControl = KAL_FALSE;
        /* Check the set_auth_req_mode first */
        kal_trace(TRACE_GROUP_1,BT_SPPOS_DISCONNECT_IND_WAIT_PLUGOUT_FLAG,((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg);
        if (((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg == KAL_TRUE)
        {
            /* It means that this SPP DISCONNECT_IND is triggered due to no PLUG_OUT_CNF is sent to BT, BT rejects any incoming CON_REQ */    
            kal_trace(TRACE_GROUP_1,BT_SPPA_NOT_SEND_PLUGOUT_IND);
            return;
        }
        /* Only task priority is lower than MOD_BT, it is necessary to apply this flow control mechanism */
        Enable_SPP_FlowControl = CheckSPPFlowControlSetting(((SPP_DEV_OS*) dev)->owner);
        if (Enable_SPP_FlowControl)
        {
            kal_trace(TRACE_GROUP_1,BT_SPP_SET_WAIT_PLUGOUT_CNF_FLAG_INX,0);
            ((SPP_DEV_OS*) dev)->wait_plugout_cnf_msg = KAL_TRUE;
        }
    }
/* #ifdef CUSTOM_SPP */
    parms.event = UART_EVENT_PLUGOUT_IND;
    parms.dstMod = ((SPP_DEV_OS*) dev)->owner;
    parms.p.uart_plugout_ind.profile = ((SPP_DEV_OS*) dev)->profile;
    SPPCB(&parms);
/* #else #ifdef CUSTOM_SPP */

    if (((SPP_DEV_OS*) dev)->profile == SPP_PROFILE)
    {
/* #ifdef CUSTOM_SPP */
        parms.event = SPP_EVENT_DISCONNECT_IND;
        parms.dstMod = MOD_MMI;
        Report(("SPPOS_DisConnectInd:dstMod=%d", parms.dstMod));
        parms.p.spp_disconnect_ind.port = ((SPP_DEV_OS*) dev)->port;
        SPPCB(&parms);
/* #else #ifdef CUSTOM_SPP */
    }
#if DUN_SERVER == XA_ENABLED || DUN_CLIENT == XA_ENABLED
    else if (((SPP_DEV_OS*) dev)->profile == DUN_PROFILE)
    {

    /* #ifdef CUSTOM_SPP */
        parms.event = DUN_EVENT_DISCONNECT_IND;
        parms.dstMod = MOD_MMI;
        parms.p.dun_disconnect_ind.port = ((SPP_DEV_OS*) dev)->port;
        SPPCB(&parms);        
    /* #else #ifdef CUSTOM_SPP */
    }
#endif /* DUN_SERVER == XA_ENABLED */

#if FAX_SERVER == XA_ENABLED
    else if (((SPP_DEV_OS*) dev)->profile == FAX_PROFILE)
    {
        parms.event = FAX_EVENT_DISCONNECT_IND;
        parms.dstMod = MOD_MMI;
        parms.p.fax_disconnect_ind.port = ((SPP_DEV_OS*) dev)->port;
        SPPCB(&parms);        
    }
#endif /* FAX_SERVER == XA_ENABLED */
}

#if 0
/* Platform dependent APIs <MAUI> */
kal_uint8* BtGet_SPPClientFirstConnDataPattern(kal_uint8* len)
{
    if(SPPClientFirstConnDataPatternLen == 0)
    {
        //Using default setting; pattern = MTKSPPForMMI;
        btmtk_os_memset(SPPClientFirstConnDataPattern,0,20);
        SPPClientFirstConnDataPatternLen =  (U8) btmtk_os_strlen("MTKSPPForMMI");
        btmtk_os_memcpy(SPPClientFirstConnDataPattern,"MTKSPPForMMI",SPPClientFirstConnDataPatternLen); 
        
    }
    *len = SPPClientFirstConnDataPatternLen; 
    return SPPClientFirstConnDataPattern;
}
#endif


void SPPOS_SendUARTDataAvailableInd(UART_PORT port)
{
	SPPCallbackParms parms;  
	SPP_DEV_OS *vp_ptr = NULL;
	U16 lengthWritten = 0;
	U16 lengthToWrite = 0;
	U16 lengthTotal = 0;
	U16 maxBufferLength = 0;

	vp_ptr = getDev(port);

	// Check if waiting for UART_Data_Available_IND_CNF
	if (0 < ((SPP_DEV_OS*) vp_ptr)->wait_uart_data_available_ind_rsp_length)
	{
		return;
	}

#ifdef __SPP_SHARED_MEMORY__
	 maxBufferLength = vp_ptr->getMemSize;
	 parms.p.spp_uart_data_available_ind.buffer = vp_ptr->getMemPtr;
#else
	 maxBufferLength = SPP_UART_GET_BYTE_BUFFER_LENGTH;
#endif	/* __SPP_SHARED_MEMORY__ */
	
	lengthTotal = RING_DataLen(((SPP_DEV_OS*) vp_ptr)->rRing);
	lengthToWrite = 0;
	lengthWritten = 0;
	if (0 < (lengthTotal - lengthWritten))
	{
		if (maxBufferLength > (lengthTotal - lengthWritten))
		{
			lengthToWrite = (lengthTotal - lengthWritten);
			
		}
		else
		{
			lengthToWrite = maxBufferLength;
		}
		((SPP_DEV_OS*) vp_ptr)->wait_uart_data_available_ind_rsp_length = lengthToWrite;
		RING_ReadDataNoDelete(((SPP_DEV_OS*) vp_ptr)->rRing, (char*)parms.p.spp_uart_data_available_ind.buffer, lengthToWrite);
		
		parms.p.spp_uart_data_available_ind.length = lengthToWrite;
		parms.event = SPP_UART_EVENT_DATA_AVAILABLE_IND;
		parms.p.spp_uart_data_available_ind.port = port;
		parms.p.spp_uart_data_available_ind.profile = vp_ptr->profile;
		SPPCB(&parms);
		lengthWritten += lengthToWrite;
	}
    	

}

#ifdef BTMTK_ON_WISE  /* SPP_PORTING */
/* uart dispatch funtion table */
UartDriver_strcut sppa_uart_api = 
{
    SPPUartOpen,
    SPPUartClose,
    SPPUartGetBytes,
    SPPUartPutBytes,
    SPPUartGetBytesAvail,
    SPPUartGetTxRoomLeft,
    SppUartPutISRBytes,
    SppUartGetTxISRRoomLeft,
    SppUartPurge,
    SPPUartSetOwner,
    SppUartSetFlowCtrl,
    SppUartConfigEscape,
    SppUartSetDCBConfig,
    SPPCtrlDCD,
    SPPUartCtrlBreak,
    SPPUartClrRxBuffer,
    SPPUartClrTxBuffer,
    SppUartSetBaudRate,
    SppUartSendISRData,
    SppUartSendData,
    SPPUartGetOwnerID,
    SppUartSetAutoBaud_Div,
    SppUartRegister_TX_cb,
    SppUartRegister_RX_cb,
    SppUartGetUARTByte,
    SppUartPutUARTByte,
    SppUartPutUARTBytes
};
#endif  /* BTMTK_ON_WISE */

#endif /* (SPP_CLIENT == XA_ENABLED) || (SPP_SERVER == XA_ENABLED) */


