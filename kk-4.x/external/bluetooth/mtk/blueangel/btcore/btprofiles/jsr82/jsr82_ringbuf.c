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
*  permission of MediaTek Inc. (C) 2007
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 * jsr82_ringbuf.c
 *
 * Project:
 * --------
 * 
 *
 * Description:
 * ------------
 * 
 *
 * Author:
 * -------
 * Bingyi Chen
 *
 *============================================================================
 */

#include "bt_jsr82_setting.h"
#include "bt_common.h"
#include "bluetooth_trc.h"
#include "ring.h"
#include "jsr82_session.h"
//#include "bt_jsr82_api.h"
#include "bt_adp_mem.h"
#include "bt_os_api.h"


extern JSR82Callback JSR82CB;

kal_int8 jsr82_session_allocate_TxPktPool(U8 conn_index);
kal_int8 jsr82_session_free_TxPktPool(U8 index);
kal_int8 jsr82_session_fetchTxPacket(U8 index, U8 **buff, U16 *len);
void jsr82_session_returnTxPacket(U8 index, U8 *buff, U16 len);





/*****************************************************************************
 * FUNCTION
 *  jsr82_session_RxBytes
 * DESCRIPTION
 *  This function is called to calculate credits information
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_RxBytes(kal_uint8 session_type, kal_uint8 session_inx, kal_uint16 subsession_id)
{
	return RING_DataLen(&(BTJ82SEAP(session)[session_inx].ReadRingBuf));
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_RxFree
 * DESCRIPTION
 *  
 * PARAMETERS
 *  search_type         [IN]        
 *  session_type        [IN]        
 *  subsession_id       [IN]        
 *  value               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_RxFree(kal_uint8 search_type, kal_uint8 session_type, kal_uint16 subsession_id, kal_uint32 value)
{
	U8 session_inx;

	session_inx = bt_jsr82_SearchL2capContext(subsession_id);
	if (session_inx >= JSR82_MAX_SESSION_NO)
	{
		return 0;
	} 
	else
	{
		return RING_FreeSpace(&(BTJ82SEAP(session)[session_inx].ReadRingBuf));
	}
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_DevRX
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 *  buf                 [IN]        
 *  len                 [OUT]       
 * RETURNS
 *  
 *****************************************************************************/
kal_int8 jsr82_session_DevRX(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            char buf[],
            U16 *len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 rc = 0;    /* 0: Means succseesful */
    S16 actual_write_len = 0, actual_write_len_2 = 0, wanted_to_write_len = *len;
    JSR82CallbackParms parms;

    actual_write_len = RING_WriteData(&(BTJ82SEAP(session)[session_inx].ReadRingBuf), buf, *len);

    /*
     * Add by mtk01411: 2007-1015 
     * Because the creadit is computed as Ring's size - total_data_len:
     * If this RFCOMM's pkt is received, it means that the data contained in this pkt can be put to Ring Buf!
     * It is necessary to check if there is still free space: If it has, it means the pWrite pointer is wrapped to start position
     * It must copy the left data in the pkt to Ring Buf continusly
     */
     if (RING_FreeSpace(&(BTJ82SEAP(session)[session_inx].ReadRingBuf)))
    {
        /*
         * In this case, because pWrite pointer is wrapped to start poisiton of buffer at last RING_WirteData()
         * So if there is still free space and the wanted_to_write_len is larger than actual_write_len of last RING_WirteData(): 
         */
        if (actual_write_len < wanted_to_write_len)
        {
            actual_write_len_2 = RING_WriteData(
                &(BTJ82SEAP(session)[session_inx].ReadRingBuf),
                buf + actual_write_len,
                (S16) (wanted_to_write_len - actual_write_len));
        }
    }
     if ((actual_write_len + actual_write_len_2) < wanted_to_write_len)
     {
     	  kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DEVRX_DATA_LOST_DUE_TO_RING_BUFFER_FULL, (wanted_to_write_len - actual_write_len - actual_write_len_2));
     }

    jsr82_session_SendDataAvailableInd(session_inx);

    if (KAL_TRUE == BTJ82SEAP(session)[session_inx].readyToRead)
    {
        kal_int16 total_data_len = 0;

        /* Report(("In jbt_spp_DevRx(): Set readyToRead FALSE")); */
        kal_trace(BT_TRACE_JSR82_GROUP, FUNC_JSR82_RING_BUFFER_SET_READY_TO_READ_AS_FALSE);
        total_data_len = RING_DataLen(&(BTJ82SEAP(session)[session_inx].ReadRingBuf));
        BTJ82SEAP(session)[session_inx].readyToRead = KAL_FALSE;
        /* BT stack must notify the JBT that the event READY_TO_READ_IND */
	 parms.p.rx_ready_ind.session_inx = session_inx;
  //      parms.p.rx_ready_ind.l2cap_id = subsession_id;
        parms.p.rx_ready_ind.length = total_data_len;
	 parms.event = JSR82_EVENT_RX_READY_IND;
	 parms.dstMod = MOD_MMI;
	 JSR82CB(&parms);
    }
    
    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_CheckReadyToWrite
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 *  one_session         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jsr82_session_CheckReadyToWrite(U8 session_type, U8 session_inx, kal_uint16 subsession_id, void *one_session)
{
    JSR82CallbackParms parms;
    
    if (KAL_TRUE == BTJ82SEAP(session)[session_inx].notifyReadyToWrite)
    {
        BTJ82SEAP(session)[session_inx].notifyReadyToWrite = KAL_FALSE;
        /* BT stack must notify JBT that it's time to write data continusly: READY_TO_WRITE_IND */
	 parms.p.tx_ready_ind.session_inx = session_inx;
        parms.p.tx_ready_ind.l2cap_id = subsession_id;
        parms.p.tx_ready_ind.ps_type = session_type;
		parms.p.tx_ready_ind.tx_empty = BTJ82SEAP(session)[session_inx].is_tx_empty;
	 parms.event = JSR82_EVENT_TX_READY_IND;
	 parms.dstMod = MOD_MMI;
	 JSR82CB(&parms);
     }
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_ReturnBuf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 *  buf                 [IN]        
 *  len                 [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jsr82_session_ReturnBuf(kal_uint8 session_type, kal_uint8 session_inx, kal_uint16 subsession_id, char *buf, U16 len)
{
 //   RING_BufDelete(&(BTJ82SEAP(session)[session_inx].WriteRingBuf), len);
 	jsr82_session_returnTxPacket(session_inx, (U8 *)buf, len);

    if (BTJ82SEAP(session)[session_inx].WriteRingBuf.pRead == BTJ82SEAP(session)[session_inx].WriteRingBuf.pWrite)
    {
        BTJ82SEAP(session)[session_inx].is_tx_empty = KAL_TRUE;
    }

  //	if (RING_DataLen(&(BTJ82SEAP(session)[session_inx].WriteRingBuf)) == 0)
 // 	if (jsr82_session_BufFreeSpace(session_type, session_inx, subsession_id, JBT_SESSION_TX_BUF_TYPE) > 0)
 	if (RING_DataLen(&(BTJ82SEAP(session)[session_inx].WriteRingBuf)) == 0)
    {
        BTJ82SEAP(session)[session_inx].notifyReadyToWrite = KAL_TRUE;
    }

    jsr82_session_CheckReadyToWrite(session_type, session_inx, subsession_id, NULL);
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_DevTX
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 *  buf                 [IN]        
 *  len                 [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int8 jsr82_session_DevTX(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            char **buf,
            U16 *len)
{
	kal_int8 rc = 0;    /* Means successful */
	U16 bufSize = 0;

//	RING_GetDataPtr(&(BTJ82SEAP(session)[session_inx].WriteRingBuf), buf, (kal_int16*) len);

	rc = jsr82_session_fetchTxPacket(session_inx, (U8 **)buf, &bufSize);
	if ((signed char) rc < 0)
	{
		return rc;
	}

	*len = min (*len, bufSize);
	*len = RING_ReadData(&(BTJ82SEAP(session)[session_inx].WriteRingBuf), (char*)(*buf), *len);
	return rc;
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_DevTxEmpty
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void jsr82_session_DevTxEmpty(kal_uint8 session_type, kal_uint8 session_inx, kal_uint16 subsession_id)
{
	BTJ82SEAP(session)[session_inx].is_tx_empty = KAL_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_allocate_RWRingBuf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type            [IN]        
 *  subsession_entry        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int8 jsr82_session_allocate_RWRingBuf(kal_uint8 session_type, U8 index)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_int8 rc = 0;    /* rc=0 : Means successful */
    kal_uint16 rx_size = 0, tx_size = 0;
    kal_uint8 rx_block_inx = 0, tx_block_inx = 0; 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (session_type == JBT_SESSION_TYPE_SPP)
    {
        rx_size = JSR82_SESSION_SPP_RX_BUFSIZE;
        tx_size = JSR82_SESSION_SPP_TX_BUFSIZE;
    }
    else if (session_type == JBT_SESSION_TYPE_L2CAP)
    {
        rx_size = JSR82_SESSION_L2CAP_TX_BUFSIZE;
        tx_size = JSR82_SESSION_L2CAP_RX_BUFSIZE;
    }

#ifdef __JSR82_DYNAMIC_MEMORY__
    BTJ82SEAP(session)[index].ReadRingBuf.pStart = (kal_uint8*) btmtk_jsr82_malloc(rx_size);
#else
    // TODO: Declare one array
#endif

    if (BTJ82SEAP(session)[index].ReadRingBuf.pStart == NULL)
    {
        rc = JBT_SESSION_FAILED_ALLOCATE_RINGBUF;   /* Failed to allocate memory space */
        kal_trace(BT_TRACE_JSR82_GROUP, FUNC_JSR82_RING_BUFFER_ALLOCATE_READRINGBUF_FAILED, rx_size);
        return rc;
    }
    else
    {
        /* Allocate memory space successfully, assign pEnd and init pRead and pWrite */
        BTJ82SEAP(session)[index].ReadRingBuf.pEnd = BTJ82SEAP(session)[index].ReadRingBuf.pStart + rx_size;
          RING_BufFlush(&(BTJ82SEAP(session)[index].ReadRingBuf));
    }

    BTJ82SEAP(session)[index].WriteRingBuf.pStart = (kal_uint8*) btmtk_jsr82_malloc(tx_size);
    
    if (BTJ82SEAP(session)[index].WriteRingBuf.pStart == NULL)
    {
        rc = JBT_SESSION_FAILED_ALLOCATE_RINGBUF;   /* Failed to allocate memory space */
        kal_trace(BT_TRACE_JSR82_GROUP, FUNC_JSR82_RING_BUFFER_ALLOCATE_WRITERINGBUF_FAILED, tx_size);
        return rc;
    }
    else
    {
        /* Allocate memory space successfully, assign pEnd and init pRead and pWrite */
        BTJ82SEAP(session)[index].WriteRingBuf.pEnd = BTJ82SEAP(session)[index].WriteRingBuf.pStart + tx_size;
        RING_BufFlush(&(BTJ82SEAP(session)[index].WriteRingBuf));
    }

	rc = jsr82_session_allocate_TxPktPool(index);

	
    return rc;

}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_free_RWRingBuf
 * DESCRIPTION
 *  
 * PARAMETERS
 *  subsession_entry        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_uint8 jsr82_session_free_RWRingBuf(U8 index)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    kal_uint8 rc = 0x00;    /* 0x00: means successful */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ( BTJ82SEAP(session)[index].ReadRingBuf.pStart != NULL)
    {
        btmtk_jsr82_free( BTJ82SEAP(session)[index].ReadRingBuf.pStart);
        BTJ82SEAP(session)[index].ReadRingBuf.pStart = NULL;
        BTJ82SEAP(session)[index].ReadRingBuf.pEnd = NULL;
        BTJ82SEAP(session)[index].ReadRingBuf.pRead = NULL;
        BTJ82SEAP(session)[index].ReadRingBuf.pWrite = NULL;
    }
    if (BTJ82SEAP(session)[index].WriteRingBuf.pStart != NULL)
    {
    	 btmtk_jsr82_free(BTJ82SEAP(session)[index].WriteRingBuf.pStart);
        BTJ82SEAP(session)[index].WriteRingBuf.pStart = NULL;
        BTJ82SEAP(session)[index].WriteRingBuf.pEnd = NULL;
        BTJ82SEAP(session)[index].WriteRingBuf.pRead = NULL;
        BTJ82SEAP(session)[index].WriteRingBuf.pWrite = NULL;
    }
	
	rc = jsr82_session_free_TxPktPool(index);
	
    return rc;
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_PutBytes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]	==> l2cap_id
 *  Buffaddr            [IN]        
 *  Length              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_PutBytes(bt_jsr82_put_bytes_req_struct *ptr)
{
    kal_int16 writeLength = 0;
    U8 *pPrevWrite = NULL;
    kal_bool app_regards_buf_empty = FALSE;
    JSR82CallbackParms parms;
    kal_int16 freespace_dataLen_in_TXBuf = 0;

	OS_Report("[JSR82]jsr82_session_PutBytes");

     if (BT_JSR82_SESSION_IDLE == BTJ82SEAP(session)[ptr->index].status)
     {
	      kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_PUTBYTES_INVALID_INDEX, ptr->index);
		  return 0;
     }
     
     if ((BT_JSR82_CHANNEL_STATE_OPEN != BTJ82SEAP(session)[ptr->index].channel_state) && 
     	   (BT_JSR82_CHANNEL_STATE_OPEN != ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[ptr->index].session_buffer))->l2capCtx.l2cap_con_state))
     {
     	  kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_PUTBYTES_INVALID_STATE, ptr->index);
		  return 0;
     }

    /* Buffer is empty, sets app_regards_buf_empty to trigger to send data request to BT task:2007-1015 */
    if (BTJ82SEAP(session)[ptr->index].WriteRingBuf.pRead == BTJ82SEAP(session)[ptr->index].WriteRingBuf.pWrite)
    {
        app_regards_buf_empty = TRUE;
    }

    pPrevWrite = BTJ82SEAP(session)[ptr->index].WriteRingBuf.pWrite;

    writeLength = RING_WriteData(&(BTJ82SEAP(session)[ptr->index].WriteRingBuf), (char*)ptr->buffAddr, ptr->length);

     freespace_dataLen_in_TXBuf = jsr82_session_BufFreeSpace(ptr->ps_type, ptr->index, ptr->l2cap_id, JBT_SESSION_TX_BUF_TYPE);
   if (writeLength < ptr->length)
    {
        BTJ82SEAP(session)[ptr->index].notifyReadyToWrite = KAL_TRUE;
    }
    else
    {
        //kal_int16 freespace_dataLen_in_TXBuf = jsr82_session_BufFreeSpace(ptr->ps_type, ptr->index, ptr->l2cap_id, JBT_SESSION_TX_BUF_TYPE);
        if (freespace_dataLen_in_TXBuf == 0)
        {
            BTJ82SEAP(session)[ptr->index].notifyReadyToWrite = KAL_TRUE;
        }
    }

    /*
     * Because is_tx_empty is TRUE, JBT can receive READY_TO_WRITE_IND 
     * Application can write data to buffer again
     * Because it is not possible to transmit data rely on receiving PKT_Handled event, it must send SEND_DATA_REQ to trigger transmissino action 
     */
    if (app_regards_buf_empty || (pPrevWrite == BTJ82SEAP(session)[ptr->index].WriteRingBuf.pRead) || BTJ82SEAP(session)[ptr->index].is_tx_empty)
    {
  /*      BTJ82SEAP(session)[ptr->index].is_tx_empty = KAL_FALSE;
	    parms.p.send_data_req.session_inx = ptr->index;
        parms.p.send_data_req.l2cap_id = ptr->l2cap_id;
		 parms.event = JSR82_EVENT_SEND_DATA_REQ;
		 parms.dstMod = MOD_BT;
		 JSR82CB(&parms);
		 */

		 bt_jsr82_send(ptr->index, ptr->l2cap_id);
    }
    
    return writeLength;

}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_GetBytes
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]	==> l2cap_id 
 *  Buffaddr            [IN]        
 *  Length              [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_GetBytes(bt_jsr82_get_bytes_req_struct *ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S16 DataRead = 0;
    JSR82CallbackParms parms;
     
    /* Note: Using session_inx to find its corresponding SPP entry index */
    /* Get data from ReadRingBuf */
    DataRead = RING_ReadData(&(BTJ82SEAP(session)[ptr->index].ReadRingBuf), (char*)ptr->buffAddr, ptr->length);

    /*
     * Because it may context switch to BT task to put more incoming data into ReadRingBuf,
     * It will check the data length to determine if it is necessary to read 
     */

    /* Report(("In jbt_spp_read(): DataRead=%d,Length=%d",DataRead,Length)); */
    kal_trace(TRACE_INFO, FUNC_JBT_READ, DataRead, ptr->length);

    if (DataRead < ptr->length)
    {
        kal_int16 ReadMore;

        BTJ82SEAP(session)[ptr->index].readyToRead = TRUE;
        ReadMore = RING_DataLen(&(BTJ82SEAP(session)[ptr->index].ReadRingBuf));

        /* Report(("In jbt_spp_read():case1 Set readyToRead to TRUE and ReadMore=%d",ReadMore)); */
        kal_trace(BT_TRACE_JSR82_GROUP, FUNC_JSR82_RING_BUFFER_READ_01, ReadMore);
        if (ReadMore > 0)
        {
            if ((DataRead + ReadMore) < ptr->length)
            {
                RING_ReadData(&(BTJ82SEAP(session)[ptr->index].ReadRingBuf), (char*)(ptr->buffAddr + DataRead), ReadMore);
                DataRead += ReadMore;
            }
            else
            {

                kal_int16 left_dataLen_in_RxBuf = 0;

		  RING_ReadData(&(BTJ82SEAP(session)[ptr->index].ReadRingBuf), (char*)(ptr->buffAddr + DataRead), (S16) (ptr->length - DataRead));
                DataRead = ptr->length;
                BTJ82SEAP(session)[ptr->index].readyToRead = FALSE;
                /* Report(("In jbt_spp_read():case2 Set readyToRead to FALSE")); */
                kal_trace(BT_TRACE_JSR82_GROUP, FUNC_JBT_READ_02);

                /* Check the RXRingBuf's readable_len */
                left_dataLen_in_RxBuf = jsr82_session_BufAvailableDataLen(ptr->ps_type, ptr->index, ptr->l2cap_id, JBT_SESSION_RX_BUF_TYPE);
                if (left_dataLen_in_RxBuf == 0)
                {
                    BTJ82SEAP(session)[ptr->index].readyToRead = TRUE;
                }
            }
        }
    }
    else
    {
        /* Check the RXRingBuf's readable_len */
        kal_int16 left_dataLen_in_RxBuf = jsr82_session_BufAvailableDataLen(ptr->ps_type, ptr->index, ptr->l2cap_id, JBT_SESSION_RX_BUF_TYPE);
        if (left_dataLen_in_RxBuf == 0)
        {
            BTJ82SEAP(session)[ptr->index].readyToRead = TRUE;
        }
    }

    if (ptr->ps_type == JBT_SESSION_TYPE_SPP)
    {
        /*
         * Because the data stored in ReadRingBuf is read from the upper application, it's time to update credit information
         * It will send MSG_ID_BT_JSR_SPP_GET_DATA_REQ to ask BT stack to check credit's information (compute credit value)
         */
        U8 devCredit = bt_jsr82_getCurrentCredit(ptr->index);

        if (devCredit < JSR82_SPP_RX_CREDIT / 2)
        {
	     parms.p.get_data_req.session_inx = ptr->index;
	     parms.event = JSR82_EVENT_GET_DATA_REQ;
	     parms.dstMod = MOD_BT;
	     JSR82CB(&parms);
        }
    }
    else if (ptr->ps_type == JBT_SESSION_TYPE_L2CAP)
    {
        /* No flow control in L2CAP, it is not like RFCOMM to update credit's information: 2007-1108 */
    }
    
        
    return DataRead;
}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_BufFreeSpace
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 *  buf_type            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_BufFreeSpace(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            kal_uint8 buf_type)
{
    if (buf_type == JBT_SESSION_RX_BUF_TYPE)
    {
        /* RX Buffer */
        return RING_FreeSpace(&(BTJ82SEAP(session)[session_inx].ReadRingBuf));
    }
    else if (buf_type == JBT_SESSION_TX_BUF_TYPE)
    {
        /* TX Buffer */
        return RING_FreeSpace(&(BTJ82SEAP(session)[session_inx].WriteRingBuf));
    }
    else
    {
        return JBT_SESSION_INVALID_BUFF_TYPE;
    }

}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_BufAvailableDataLen
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 *  session_inx         [IN]        
 *  subsession_id       [IN]        
 *  buf_type            [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_BufAvailableDataLen(
            kal_uint8 session_type,
            kal_uint8 session_inx,
            kal_uint16 subsession_id,
            kal_uint8 buf_type)
{
    I16 length = 0;
    
    if (buf_type == JBT_SESSION_RX_BUF_TYPE)
    {
        /* RX Buffer */
		return RING_DataLen(&(BTJ82SEAP(session)[session_inx].ReadRingBuf));
    }
    else if (buf_type == JBT_SESSION_TX_BUF_TYPE)
    {
        /* TX Buffer */
	 	return RING_DataLen(&(BTJ82SEAP(session)[session_inx].WriteRingBuf));
    }
    else
    {
        return JBT_SESSION_INVALID_BUFF_TYPE;
    }

}


/*****************************************************************************
 * FUNCTION
 *  jsr82_session_BufSize
 * DESCRIPTION
 *  
 * PARAMETERS
 *  session_type        [IN]        
 * RETURNS
 *  
 *****************************************************************************/
kal_int16 jsr82_session_BufSize(kal_uint8 session_type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (session_type == JBT_SESSION_TYPE_SPP)
    {
        return JSR82_SESSION_SPP_RX_BUFSIZE;
    }
    else if (session_type == JBT_SESSION_TYPE_L2CAP)
    {
        return JSR82_SESSION_L2CAP_RX_BUFSIZE;
    }
    else
    {
        return -1;
    }
}


void jsr82_session_SendDataAvailableInd(U8 index)
{
	JSR82CallbackParms parms;  
	U16 lengthWritten = 0;
	U16 lengthToWrite = 0;
	U16 lengthTotal = 0;
	U16 maxBufferLength = 0;
	BT_JSR82_SESSION_AP_Record_Context_struct_t *ptr = NULL;

	 if (BT_JSR82_SESSION_IDLE == BTJ82SEAP(session)[index].status)
	 {
		kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DATA_AVAILABLE_IND_INVALID_INDEX, index);
		return ;
	 }
	 if ((BT_JSR82_CHANNEL_STATE_OPEN != BTJ82SEAP(session)[index].channel_state) && 
	 	(BT_JSR82_CHANNEL_STATE_OPEN != ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[index].session_buffer))->l2capCtx.l2cap_con_state))
	 {
		kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DATA_AVAILABLE_IND_INVALID_STATE, index);
		return ;
	 }

	// Check if waiting for Data_Available_IND_RSP
	if (0 < (BTJ82SEAP(session)[index].wait_data_available_ind_rsp_length))
	{
		kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DATA_AVAILABLE_IND_WAIT_IND_RSP, index);
		return;
	}

	ptr = &BTJ82SEAP(session)[index];

#ifdef __JSR82_SHARED_MEMORY__
	 //maxBufferLength = JSR82_DELIVER_GET_DATA_SHARED_BUFFER_SIZE;
	 // TODO: 
	 maxBufferLength = BTJ82SEAP(session)[index].deliverBufSize;
#ifndef BTMTK_ON_LINUX
	 parms.p.data_available_ind.buffer = BTJ82SEAP(session)[index].deliverBufPtr;
#endif // BTMTK_ON_LINUX
#else
	 // TODO: Transmission with message buffer (array copy)
#endif	/* __SPP_SHARED_MEMORY__ */
	
	lengthTotal = RING_DataLen(&(BTJ82SEAP(session)[index].ReadRingBuf));
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
		BTJ82SEAP(session)[index].wait_data_available_ind_rsp_length = lengthToWrite;

		
	//	RING_ReadDataNoDelete(&(BTJ82SEAP(session)[index].ReadRingBuf), (char*)BTJ82SEAP(session)[index].deliverBufPtr, lengthToWrite);
		
		parms.p.data_available_ind.length = lengthToWrite;
		parms.event = JSR82_EVENT_DATA_AVAILABLE_IND;
		parms.p.data_available_ind.index = index;
		parms.p.data_available_ind.ps_type = BTJ82SEAP(session)[index].ps_type;
		parms.p.data_available_ind.l2cap_id = ((BT_JSR82_SESSION_Buffer_struct_t*) ptr->session_buffer)->l2capCtx.l2capLocalCid;
		JSR82CB(&parms);
		lengthWritten += lengthToWrite;
	}
}


void jsr82_session_DataAvailableIndRsp(bt_jsr82_data_available_ind_rsp_struct *ptr)
{
	JSR82CallbackParms parms;

	 if (BT_JSR82_SESSION_IDLE == BTJ82SEAP(session)[ptr->index].status)
	 {
		kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DATA_AVAILABLE_IND_RSP_INVALID_INDEX, ptr->index);
		return ;
	 }
	 if ((BT_JSR82_CHANNEL_STATE_OPEN != BTJ82SEAP(session)[ptr->index].channel_state) && 
	 	(BT_JSR82_CHANNEL_STATE_OPEN != ((BT_JSR82_SESSION_Buffer_struct_t*) (BTJ82SEAP(session)[ptr->index].session_buffer))->l2capCtx.l2cap_con_state))
	 {
		kal_trace(BT_TRACE_JSR82_GROUP, BT_JSR82_DATA_AVAILABLE_IND_RSP_INVALID_STATE, ptr->index);
		return ;
	 }

	if (0 < ptr->length)
	{
		RING_BufDelete(&(BTJ82SEAP(session)[ptr->index].ReadRingBuf), ptr->length);
	}
	BTJ82SEAP(session)[ptr->index].wait_data_available_ind_rsp_length = 0;


	jsr82_session_SendDataAvailableInd(ptr->index);

	if (JBT_SESSION_TYPE_SPP == BTJ82SEAP(session)[ptr->index].ps_type)
	{
	    /*
	     * Because the data stored in ReadRingBuf is read from the upper application, it's time to update credit information
	     * It will send MSG_ID_BT_JSR_SPP_GET_DATA_REQ to ask BT stack to check credit's information (compute credit value)
	     */
	    U8 devCredit = bt_jsr82_getCurrentCredit(ptr->index);

	    if (devCredit < JSR82_SPP_RX_CREDIT / 2)
	    {
		parms.p.get_data_req.session_inx = ptr->index;
		parms.event = JSR82_EVENT_GET_DATA_REQ;
		parms.dstMod = MOD_BT;
		JSR82CB(&parms);
	    }
	}
}


void jsr82_session_AssignDeliverBufferReq(bt_jsr82_assign_buffer_req_struct *ptr)
{
	JSR82CallbackParms parms;

	BTJ82SEAP(session)[ptr->index].deliverBufPtr = (U8*)jsr82_core_malloc_ashm(ptr->memNameGet, ptr->index, JSR82_ASHM_TYPE_DELIVER_GET);
	BTJ82SEAP(session)[ptr->index].deliverBufSize = ptr->deliverBufSize;
	BTJ82SEAP(session)[ptr->index].deliverPutBufPtr = (U8*)jsr82_core_malloc_ashm(ptr->memNamePut, ptr->index, JSR82_ASHM_TYPE_DELIVER_PUT);
	bt_prompt_trace(MOD_BT, "[JSR82] DeliverPutName(%s), Ptr(%x)", ptr->memNamePut, BTJ82SEAP(session)[ptr->index].deliverPutBufPtr);
	bt_prompt_trace(MOD_BT, "[JSR82] DeliverGetName(%s), Ptr(%x)", ptr->memNameGet, BTJ82SEAP(session)[ptr->index].deliverBufPtr);

	parms.p.assign_buffer_cnf.index = ptr->index;
	parms.p.assign_buffer_cnf.l2cap_id = ptr->l2cap_id;
	parms.event = JSR82_EVENT_ASSIGN_BUFFER_CNF;
	parms.dstMod = MOD_BT;
	JSR82CB(&parms);
}

kal_int8 jsr82_session_allocate_TxPktPool(U8 conn_index)
{
	BT_JSR82_Packet_struct_t *dataPkt = NULL;
	U8 *pool = NULL;
	kal_uint8 i = 0;
	kal_int8 rc = 0;
	
	pool = (U8 *)btmtk_jsr82_malloc(sizeof(BT_JSR82_Packet_struct_t) * JSR82_SESSION_TX_PKT_QUEUE_SIZE);
	if (NULL == pool)
	{
		return JBT_SESSION_FAILED_ALLOCATE_RINGBUF;
	}

	btmtk_os_memset(pool, 0x0, sizeof(BT_JSR82_Packet_struct_t) * JSR82_SESSION_TX_PKT_QUEUE_SIZE);

	BTJ82SEAP(session)[conn_index].txPktPool = pool;
	
	for (i = 0; i < JSR82_SESSION_TX_PKT_QUEUE_SIZE; i ++)
	{
		dataPkt = (BT_JSR82_Packet_struct_t *)(pool + i * sizeof(BT_JSR82_Packet_struct_t));
		InitializeListEntry(&dataPkt->node);
		InsertTailList(&(BTJ82SEAP(session)[conn_index].ApTxPktList),&dataPkt->node);
	}
	return rc;
}

kal_int8 jsr82_session_free_TxPktPool(U8 index)
{
	if (NULL == BTJ82SEAP(session)[index].txPktPool)
	{
		return -1;
	}
	while (!IsListEmpty(&(BTJ82SEAP(session)[index].ApTxPktList)))
	{
		RemoveHeadList(&(BTJ82SEAP(session)[index].ApTxPktList));
	}
	btmtk_jsr82_free(BTJ82SEAP(session)[index].txPktPool);
	BTJ82SEAP(session)[index].txPktPool = NULL;	
	return 0;
}

kal_int8 jsr82_session_fetchTxPacket(U8 index, U8 **buff, U16 *bufSize)
{
	BT_JSR82_Packet_struct_t *dataPkt = NULL;

	if (IsListEmpty(&(BTJ82SEAP(session)[index].ApTxPktList)))
	{
		*buff = NULL;
		*bufSize = 0;
		return JBT_SESSION_ERR_STATE;		
	}
	dataPkt = (BT_JSR82_Packet_struct_t *)RemoveHeadList(&(BTJ82SEAP(session)[index].ApTxPktList));
	*buff = dataPkt->data;
	*bufSize = sizeof(dataPkt->data);
	OS_Report("[JSR82] jsr82_session_fetchTxPacket %d", *buff);
	return 0;
}

void jsr82_session_returnTxPacket(U8 index, U8 *buff, U16 len)
{	
    BT_JSR82_Packet_struct_t *dataPkt = NULL;
	
	OS_Report("[JSR82] jsr82_session_returnTxPacket %d", buff);

	if (buff < BTJ82SEAP(session)[index].txPktPool ||
		buff > (BTJ82SEAP(session)[index].txPktPool + sizeof(BT_JSR82_Packet_struct_t) * JSR82_SESSION_TX_PKT_QUEUE_SIZE))
	{
		OS_Report("[JSR82] invalid address %d", buff);
		return;
	}

	dataPkt = ContainingRecord(buff, BT_JSR82_Packet_struct_t, data);
	if (NULL == dataPkt)
	{
		return;
	}
	InsertTailList(&(BTJ82SEAP(session)[index].ApTxPktList),&dataPkt->node);
}

BOOL jsr82_session_DataRemainInBuff(U8 session_type, U8 session_inx, kal_uint16 l2cap_id)
{
	kal_int16 tx_len = 0;
	kal_int16 rx_len = 0;
	ListEntry *pHead = &(BTJ82SEAP(session)[session_inx].ApTxPktList);
	ListEntry *pNext = pHead->Blink;
	kal_int16 freeTxPktCount = 0;

	//check the packets pending in lower layer cache
	while (pHead != pNext)
	{
		pNext = pNext->Blink;
		freeTxPktCount ++;
	}
	if (freeTxPktCount < JSR82_SESSION_TX_PKT_QUEUE_SIZE)
	{
		return TRUE;
	}

	tx_len = jsr82_session_BufAvailableDataLen(session_type, 
													session_inx,
													l2cap_id,
													JBT_SESSION_TX_BUF_TYPE);
	if (tx_len < 0)
	{
		return FALSE;
	}
	rx_len = jsr82_session_BufAvailableDataLen(session_type, 
													session_inx,
													l2cap_id,
													JBT_SESSION_RX_BUF_TYPE);
	if (rx_len < 0)
	{
		return FALSE;
	}
	
	return ((tx_len + rx_len) > 0);
	
}









