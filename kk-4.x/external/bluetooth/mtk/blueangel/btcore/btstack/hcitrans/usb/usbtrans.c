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

/***************************************************************************
 *
 * File:
 *     $Workfile:usbtrans.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *      This file lies between the HCI Driver and a host with
 *      an USB interface. It contains the logical USB interface.
 *      Hardware- and OS- specific code for this interface is in 
 *      USB.C.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#if (!defined(__MTK_TARGET__)) && defined(__BT_USB_DRV__)

#include "btalloc.h"
#include "sys/usbtrans.h"

#ifdef DEMO
    #undef Report
    #define Report(S)
#endif

/* Error types */
/* reserved                 (U8)0x00 */
#define ERR_OVR             (U8)0x01
#define ERR_PARITY          0x02
/* reserved                 (U8)0x03 */
#define ERR_FRAMING         0x04
/* reserved                 (U8)0x05-0x07 */
#define ERR_CRC             0x08
#define ERR_SEQN            0x09
/* reserved                 (U8)0x0a-0x80 */
#define ERR_MISSING_RETRANS 0x81
/* reserved                 (U8)0x82-0xff */

/* Transmit States */
#define TXS_IDLE          1
#define TXS_TYPE          2
#define TXS_HEADER        3
#define TXS_DATA          4

#if HCI_USB_TX_BUFFER_SIZE <= 4
    #error HCI_USB_TX_BUFFER_SIZE must be > 4 (see btconfig.h)
#endif

/* For CRC calculation */
#define PPPINITFCS      0xffff  /* Initial FCS value */
U16 Newpppfcs(U16 fcs, U8 *cp, U16 len);


/* Local Prototypes */
static HciBuffer *getRxBuffer(U8 pktType);
static void usbTransmitData(void);
static BOOL notifyCommand(void);
static BOOL notifyEvent(void);
static BOOL notifyReadAcl(void);
static BOOL notifyWriteAcl(void);
#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
static BOOL notifyReadSco(void);
static BOOL notifyWriteSco(void);
#endif
static void USBTRAN_SendData(HciPacket *hciPacket);
static void USBTRAN_BufferAvailable(HciBufFlags buffFlag);
static void USBTRAN_SetSpeed(U16 speed);



/*****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/

#if (XA_CONTEXT_PTR == XA_ENABLED)

/* If context pointers are used create a pointer called usxprt 
 */
USTRAN_Context usxprtTemp;
USTRAN_Context *usxprt = &usxprtTemp;
#elif (XA_CONTEXT_PTR == XA_DISABLED)

/* If context pointers are NOT used create a static object.
 */
USTRAN_Context usxprt;

#endif

/*---------------------------------------------------------------------------
 * USBTRAN_Init() 
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
BtStatus USBTRAN_Init(TranCallback tranCallback)
{
    int i;
    
    //UNUSED_PARAMETER(tranCallback);
    
    /* Clear the transport's context memory if necessary */
#if (XA_CONTEXT_PTR == XA_ENABLED)
    OS_MemSet((U8 *)usxprt, 0, sizeof(USTRAN_Context));

#elif (XA_CONTEXT_PTR == XA_DISABLED)
    OS_MemSet((U8 *)&usxprt, 0, sizeof(USTRAN_Context));
#endif

    /* Initialize RX/TX queues */
    InitializeListHead(&USTRAN(txQueue));

    /* State variable is local to rxDataEvent */
    for (i = 0; i < NUM_RX_STATES; i++) {
        USTRAN(rxContext[i].state) = USB_RXS_GET_INIT;
    }

    /* Init the USB interface */
    if (! USB_Init()) {
        return BT_STATUS_FAILED;
    }

    /* Initialize tx state */
    USTRAN(txPacket) = (void *)0;

    USB_RegisterReadInterruptCallback(notifyEvent);
    USB_RegisterWriteControlCallback(notifyCommand);
    USB_RegisterReadBulkCallback(notifyReadAcl);
    USB_RegisterWriteBulkCallback(notifyWriteAcl);
#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
    USB_RegisterReadIsoCallback(notifyReadSco);
    USB_RegisterWriteIsoCallback(notifyWriteSco);
#endif

    USTRAN(tranEntry).sendData = USBTRAN_SendData;
    USTRAN(tranEntry).buffAvail = USBTRAN_BufferAvailable;
    USTRAN(tranEntry).setSpeed = USBTRAN_SetSpeed;
    HCI_RegisterTransport(&USTRAN(tranEntry));

    return BT_STATUS_SUCCESS;

} /* USBTRAN_Init() */


/*---------------------------------------------------------------------------
 * USBTRAN_Shutdown() 
 *      
 * Requires:
 *     
 * Parameters:
 *    
 * Returns:
 */
BtStatus USBTRAN_Shutdown(void)
{
    bt_trace(TRACE_GROUP_1,TRANS_SHUTDOWN);

    //USB_Deinit();

    return BT_STATUS_SUCCESS;
} /* USBTRAN_Shutdown() */



/*---------------------------------------------------------------------------
 * USBTRAN_SendData() 
 *      Send a packet to the BT host via an USB interface.
 *      This function is called from HCI_Process, part of the EVM_Process or
 *      stack thread.
 *      
 * Requires:
 *     
 * Parameters:
 *        hciPacket - ptr to HciPacket
 * Returns:
 *      BT_STATUS_PENDING
 *      BT_STATUS_FAILED
 */
void USBTRAN_SendData(HciPacket *hciPacket)
{
    OS_StopHardware();

    InsertTailList(&USTRAN(txQueue), (ListEntry *)&hciPacket->node);

    if (! USTRAN(txPacket)) {
        /* A packet is not being transmitted so send one now. */
        usbTransmitData();
    }

    OS_ResumeHardware();
}


/*---------------------------------------------------------------------------
 * USBTRAN_BufferAvailable() 
 *
 */
static void USBTRAN_BufferAvailable(HciBufFlags buffFlag)
{
    switch (buffFlag & 0x0E) {
    case HCI_BUFTYPE_EVENT:
        /* Point to our state context */
        if (USTRAN(rxContext[EVENT_STATE].state) == USB_RXS_GET_BUFFER) {
            /* We needed a buffer, one is now free */
            bt_trace(TRACE_GROUP_1,USBTRAN_EVENT_BUFFER_NOW_AVAILABLE);
            notifyEvent();
        }
        break;
    case HCI_BUFTYPE_ACL_DATA:
        /* Point to our state context */
        if (USTRAN(rxContext[ACL_STATE]).state == USB_RXS_GET_BUFFER) {
            /* We needed a buffer, one is now free */
            bt_trace(TRACE_GROUP_1,USBTRAN_ACL_BUFFER_NOW_AVAILABLE);
            notifyReadAcl();
        }
        break;
#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
    case HCI_BUFTYPE_SCO_DATA:
        /* Point to our state context */
        if (USTRAN(rxContext[SCO_STATE]).state == USB_RXS_GET_BUFFER) {
            /* We needed a buffer, one is now free */
            bt_trace(TRACE_GROUP_1,USBTRAN_SCO_BUFFER_NOW_AVAILABLE);
            notifyReadSco();
        }
        break;
#endif
    }
}

/*---------------------------------------------------------------------------
 * USBTRAN_SetSpeed() 
 *
 */
static void USBTRAN_SetSpeed(U16 speed)
{
    //UNUSED_PARAMETER(speed);
}


/*---------------------------------------------------------------------------
 * usbTransmitData() 
 *      Function to transmit data. 
 *      This function may be called when an HCI packet is sent down. It is also
 *      called when the controller is ready to transmit more data. Unlike other
 *      transports, each packet must be a single IO request to USB so we must
 *      reassemble the header and data fragments.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 *      
 */
static void usbTransmitData(void)
{
    /* USTRAN(txBufp) could be on the stack in this function. It was placed
     * in the context structure because of the size.
     */
    U8            *txBufp = USTRAN(txBuf);
    UsbEndPoint    epType;         /* USB endpoint type */
    int            bytesWritten;
    U16            len;
    int            i;


    if (USTRAN(txPacket)) {
        /* We received an event indicating the packet was sent so notify
         * the upper layer
         */
        HCI_PacketSent(USTRAN(txPacket));
    }


    /* Get a packet off the transmit queue */
    if (IsListEmpty(&USTRAN(txQueue))) {
        USTRAN(txPacket) = 0;
        return;
    }

    USTRAN(txPacket) = (HciPacket *)RemoveHeadList(&USTRAN(txQueue));

    /*
     * Based upon the type, we will write 'bulk' or 'control' or 'isochronous'.
     * SCO is the only type for isochronous
     */
    if (USTRAN(txPacket)->flags & HCI_BUFTYPE_COMMAND) {
        /* COMMAND Data type - use control pipe */
        epType = USB_CONTROL_WRITE;
        len    = 3;           /* header len = 3 */
    } else if (USTRAN(txPacket)->flags & HCI_BUFTYPE_ACL_DATA) {
        /* ACL Data type - use bulk pipe */
        epType = USB_BULK_WRITE;
        len    = 4;           /* header len = 4 */

#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
    } else if (USTRAN(txPacket)->flags & HCI_BUFTYPE_SCO_DATA) {
        /* SCO Data type - use isochronous pipe */
        epType = USB_ISO_WRITE;
        len    = 3;           /* header len = 3 */
#endif

    } else {
        Assert(0);
        return;
    }

    /* Copy header to our transmit buffer */
    OS_MemCopy((U8 *)txBufp, (U8 *)USTRAN(txPacket)->header, len);
    txBufp += len;

    /* Append each fragment to our buffer */
    for (i = 0; i < USTRAN(txPacket)->fCount; i++) {
        len = USTRAN(txPacket)->f[i].len;
        Assert(len <= (USTRAN(txBuf) + HCI_USB_TX_BUFFER_SIZE) - txBufp);
        OS_MemCopy(txBufp, USTRAN(txPacket)->f[i].buffer, len);
        txBufp += len;
    }

    /* Write outstanding bytes to the USB driver */
    bytesWritten = USB_Write(epType, USTRAN(txBuf), 
                             txBufp - USTRAN(txBuf), TRUE);

    /* If no bytes were written, there was some kind of error */
    if (bytesWritten <= 0) {
        bt_trace(TRACE_GROUP_1,TRANS_USB_COULD_NOT_WRITE_DATA);
        HCI_TransportError();
    }
}


/*---------------------------------------------------------------------------
 * notifyEvent() 
 *      Called from lower layer when an Event packet is ready to read.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static BOOL notifyEvent(void)
{
    BtStatus          status;
    UsbRxStateContext    *st;             /* event state */
    U16               cntRead=1, len;

    /* Point to our state context */
    st = &USTRAN(rxContext[EVENT_STATE]);

    do {
        if (st->state == USB_RXS_GET_INIT) {
            st->state = USB_RXS_GET_HEADER;
            st->headerLen = 2;
            st->ptr = st->header;
            st->maxReadLen = st->headerLen;
        }

        if (st->state != USB_RXS_GET_BUFFER) {

            /* Read available data */
            cntRead = (U16)USB_ReadEvent(st->ptr, st->maxReadLen);
            Assert(cntRead >= 0);

            st->ptr += cntRead;

            /* See if enough data is available */
            if (cntRead != st->maxReadLen) {
                st->maxReadLen = (U16)(st->maxReadLen - cntRead);
                return TRUE;
            }
        }

        /* All the data for the previous state has been read, now progress
         * to the next state.
         */
        switch (st->state) {
        
        case USB_RXS_GET_HEADER:
            /* Read header above so move to next state. */
            st->state = USB_RXS_GET_BUFFER;
            /* Drop to thru to next case */

        case USB_RXS_GET_BUFFER:
            if (st->header[1]) {
                /* There are parameters */
                if (0 != (len = (U16)(st->header[1] + st->headerLen))) {
                    status = HCI_GetEventBuffer(&st->rxBuffHandle, len);
                    if (status == BT_STATUS_SUCCESS) {
                        /* Got an Event buffer */
                        st->ptr = HCI_GetRxPtr(st->rxBuffHandle);
                        st->maxReadLen = st->header[1];
                    } else {
                        /* No receive buffer available */
                        bt_trace(TRACE_GROUP_1,TRANS_NO_EVENT_BUFFERS);
                        return FALSE;
                    }
                    Assert(status == BT_STATUS_SUCCESS);
                }
            }

            OS_MemCopy(st->ptr, st->header, st->headerLen);

            /* Set up for next state */
            st->ptr += st->headerLen;
            st->state = USB_RXS_GET_DATA;
            break;

        case USB_RXS_GET_DATA:
            /* Consider all commands ack'ed */
            USTRAN(unackedCommand) = FALSE;  

            HCI_RxBufferInd(st->rxBuffHandle, 
                            BT_STATUS_SUCCESS);

            /* Reset to first state */
            st->state = USB_RXS_GET_INIT;

        }

    } while (cntRead);

    return TRUE;
}



#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
/*---------------------------------------------------------------------------
 * notifyReadSco() 
 *      Called from lower layer when SCO data is available for reading.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static BOOL notifyReadSco(void)

{
    UsbRxStateContext    *st;      /* SCO state */
    U16 cntRead=1;                /* Length of data we most recently read from USB */
    BtStatus status;
    HciHandle hciConnHandle;
    U16 len;

    /* Point to our state context */
    st = &USTRAN(rxContext[SCO_STATE]);


    /* Loop while there is data to be read */
    do {
        if (st->state == USB_RXS_GET_INIT) {
            st->headerLen = 3;

            /* Set up for the next state */
            st->ptr = st->header;
            st->maxReadLen = st->headerLen;
            st->ptr = st->header;
            st->maxReadLen = st->headerLen;
            st->state = USB_RXS_GET_HEADER;
        }

        if (st->state != USB_RXS_GET_BUFFER) {

            /* Read available data */
            cntRead = (U16)USB_ReadIso(st->ptr, st->maxReadLen);
            Assert(cntRead >= 0);

            st->ptr += cntRead;

            /* See if enough data is available */
            if (cntRead != st->maxReadLen) {
                st->maxReadLen = (U16)(st->maxReadLen - cntRead);
                return TRUE;
            }
        }



        /* All the data for the previous state has been read, now progress
         * to the next state.
         */
        switch (st->state) {
        
        case USB_RXS_GET_HEADER:
            /* Header was read above. Go to next state. */
            st->state = USB_RXS_GET_BUFFER;
            /* drop through to next state */

        case USB_RXS_GET_BUFFER:
            len = (U16)(st->header[2] + st->headerLen);
            hciConnHandle = LEtoHost16(&(st->header[0]));
            status = HCI_GetScoBuffer(&st->rxBuffHandle, 
                                      hciConnHandle, len);
            if (status == BT_STATUS_SUCCESS) {
                /* Got an SCO buffer */
                st->ptr = HCI_GetRxPtr(st->rxBuffHandle);
                st->maxReadLen = st->header[2];

            } else if (status == BT_STATUS_NO_RESOURCES) {
                /* No receive buffer available */
                bt_trace(TRACE_GROUP_1,TRANS_NO_SCO_BUFFERS);
                return FALSE;
            }

            Assert(status != BT_STATUS_INVALID_PARM);

            OS_MemCopy(st->ptr, st->header, st->headerLen);

            /* Set up for next state */
            st->ptr += st->headerLen;
            st->state = USB_RXS_GET_DATA;
            break;

        case USB_RXS_GET_DATA:
            /* Pass it up */
            HCI_RxBufferInd(st->rxBuffHandle, 
                            BT_STATUS_SUCCESS);

            /* Reset to first state */
            st->state = USB_RXS_GET_INIT;
            break;

        }

    } while (cntRead);

    return TRUE;
}
#endif /* (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0) */



/*---------------------------------------------------------------------------
 * notifyReadAcl() 
 *      Called from lower layer when ACL data is available for reading.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static BOOL notifyReadAcl(void)
{
    UsbRxStateContext  *st;
    U16 cntRead=1, len;
    BtStatus status;
    HciHandle hciConnHandle;

    /* Point to our state context */
    st = &USTRAN(rxContext[ACL_STATE]);

    /* Loop while there is data to be read */
    do {
        if (st->state == USB_RXS_GET_INIT) {
            st->headerLen = 4;

            /* Set up for the next state */
            st->ptr = st->header;
            st->maxReadLen = st->headerLen;
            st->ptr = st->header;
            st->maxReadLen = st->headerLen;
            st->state = USB_RXS_GET_HEADER;
        }

        if (st->state != USB_RXS_GET_BUFFER) {

            /* Read available data */
            cntRead = (U16)USB_ReadBulk(st->ptr, st->maxReadLen);
            Assert(cntRead >= 0);

            st->ptr += cntRead;

            /* See if enough data is available */
            if (cntRead != st->maxReadLen) {
                st->maxReadLen = (U16)(st->maxReadLen - cntRead);
                return TRUE;
            }
        }



        /* All the data for the previous state has been read, now progress
         * to the next state.
         */
        switch (st->state) {
        
        case USB_RXS_GET_HEADER:
            /* Header was read above. Go to next state. */
            st->state = USB_RXS_GET_BUFFER;
            /* drop through to next state */


        case USB_RXS_GET_BUFFER:
            len = (U16)(LEtoHost16(&(st->header[2])) + st->headerLen);
            hciConnHandle = LEtoHost16(&(st->header[0]));
            status = HCI_GetAclBuffer(&st->rxBuffHandle, 
                                      hciConnHandle, len);
            if (status == BT_STATUS_SUCCESS) {
                /* Got an ACL buffer */
                st->ptr = HCI_GetRxPtr(st->rxBuffHandle);
                st->maxReadLen = LEtoHost16(&(st->header[2]));

            } else if (status == BT_STATUS_NO_RESOURCES) {
                /* No receive buffer available */
                bt_trace(TRACE_GROUP_1,TRANS_NO_ACL_BUFFERS);
                return FALSE;
            }
            Assert(status != BT_STATUS_INVALID_PARM);

            OS_MemCopy(st->ptr, st->header, st->headerLen);

            /* Set up for next state */
            st->ptr += st->headerLen;
            st->state = USB_RXS_GET_DATA;
            break;

        case USB_RXS_GET_DATA:
            /* Pass it up */

            HCI_RxBufferInd(st->rxBuffHandle, 
                            BT_STATUS_SUCCESS);

            /* Reset to first state */
            st->state = USB_RXS_GET_INIT;
            break;

        }

    } while (cntRead);

    return TRUE;
}



/*---------------------------------------------------------------------------
 * notifyCommand() 
 *      Called from lower layer when a Command packet has been written.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static BOOL notifyCommand(void)
{
    usbTransmitData();
    return TRUE;
}


/*---------------------------------------------------------------------------
 * notifyWriteAcl() 
 *      Called from lower layer when a packet has been completely sent.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static BOOL notifyWriteAcl(void)
{
    usbTransmitData();
    return TRUE;
}



#if (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0)
/*---------------------------------------------------------------------------
 * notifyWriteIso() 
 *      Called from lower layer when a packet has been completely sent.
 *      
 * Requires:
 *     
 * Parameters:
 *
 * Returns:
 */
static BOOL notifyWriteSco(void)
{
    usbTransmitData();
    return TRUE;
}
#endif /* (BT_SCO_HCI_DATA == XA_ENABLED) && (NUM_SCO_CONNS > 0) */


#else
#include "btalloc.h"
#include "sys/usbtrans.h"
#endif


