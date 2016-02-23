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
 *     xa_abcsp.h
 *
 * Description:
 *      HCI Transport - ABCSP integration definitions for inclusion by
 *      ABCSP library. This file must be added to the ASBCP library
 *      includes list, "src\abcsp_private.h" is the recommended file.
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

/*----------------------------------------------------------------------------
 * BcspChannel type
 *
 *     Channel types defined by BCSP specification.
 */
#define BCSP_CHAN_ACK   0       /* for ACKs only */
#define BCSP_CHAN_LE    1       /* (unreliable) is for link establishment */
#define BCSP_CHAN_HCI   5       /* (reliable) is for HCI commands and events */
#define BCSP_CHAN_ACL   6       /* (reliable) is for ACL data */
#define BCSP_CHAN_SCO   7       /* (unreliable and reliable) is for SCO data */

extern ABCSP_RXMSG *xa_rxmsg_create(unsigned chan, unsigned len);
extern char *xa_rxmsg_getbuf(ABCSP_RXMSG *m, unsigned *n);
extern void xa_rxmsg_write(ABCSP_RXMSG *m, char *buf, unsigned n);
extern void xa_rxmsg_complete(ABCSP_RXMSG *m);
extern void xa_delivermsg(ABCSP_RXMSG *m, unsigned chan, unsigned rel);
extern void xa_rxmsg_destroy(ABCSP_RXMSG *m);
extern void xa_cancel_bcsp_timer(void);
extern void xa_cancel_tshy_timer(void);
extern void xa_cancel_tconf_timer(void);
extern void xa_start_bcsp_timer(void);
extern void xa_start_tshy_timer(void);
extern void xa_start_tconf_timer(void);
extern void xa_txmsg_init_read(ABCSP_TXMSG *msg);
extern unsigned xa_txmsg_length(ABCSP_TXMSG *msg);
extern const char *xa_txmsg_getbuf(ABCSP_TXMSG *msg, unsigned *buflen);
extern void xa_txmsg_taken(ABCSP_TXMSG *msg, unsigned ntaken);
extern void xa_txmsg_done(ABCSP_TXMSG *msg);
extern char *xa_uart_gettxbuf(unsigned *bufsiz);
extern void xa_uart_sendbytes(char *buf, unsigned n);
extern void xa_panic(unsigned int e);
extern void xa_free(void *m);
extern void *xa_malloc(unsigned n);
extern void *xa_zmalloc(unsigned n);
extern void xa_event(unsigned e);
extern void xa_req_pumptxmsgs(void);

/*
 * From config_txmsg.h
 */
/* Obtain buffer for uart output */
#define  ABCSP_UART_GETTXBUF    xa_uart_gettxbuf

/* Pass a block of bytes to the output uart */
#define  ABCSP_UART_SENDBYTES   xa_uart_sendbytes

/* Initialise reading a bcsp transmit message */
#define  ABCSP_TXMSG_INIT_READ  xa_txmsg_init_read

/* How long is a transmit message */
#define  ABCSP_TXMSG_LENGTH     xa_txmsg_length

/* Access raw message bytes in a message */
#define  ABCSP_TXMSG_GETBUF     xa_txmsg_getbuf

/* Tell message how many bytes have been read */
#define  ABCSP_TXMSG_TAKEN      xa_txmsg_taken

/* Signal that message has been delivered */
#define  ABCSP_TXMSG_DONE       xa_txmsg_done

/*
 * From config_event.h 
 */
/* Report an event from the abcsp code */
#define ABCSP_EVENT             xa_event

/* Request external code call abcsp_pumptxmsgs */
#define ABCSP_REQ_PUMPTXMSGS    xa_req_pumptxmsgs

/*
 * From config_rxmsg.h 
 */
/* Pass abcsp message to higher level code */
#define ABCSP_DELIVERMSG        xa_delivermsg

/* Create a abcsp receive message */
#define ABCSP_RXMSG_CREATE      xa_rxmsg_create

/* Obtain storage to write to a message */
#define ABCSP_RXMSG_GETBUF      xa_rxmsg_getbuf

/* Write to a higher-level message */
#define ABCSP_RXMSG_WRITE       xa_rxmsg_write

/* Finish a higher-level message */
#define ABCSP_RXMSG_COMPLETE    xa_rxmsg_complete

/* Destroy a abcsp receive message */
#define ABCSP_RXMSG_DESTROY     xa_rxmsg_destroy

/*
 * From config_timer.h 
 */
/* Start the bcsp ack timeout timer */
#define ABCSP_START_BCSP_TIMER      xa_start_bcsp_timer

/* Start the bcsp-le tshy timer */
#define ABCSP_START_TSHY_TIMER      xa_start_tshy_timer

/* Start the bcsp-le tconf timer */
#define ABCSP_START_TCONF_TIMER     xa_start_tconf_timer

/* Cancel the bcsp ack timeout timer */
#define ABCSP_CANCEL_BCSP_TIMER     xa_cancel_bcsp_timer

/* Cancel the bcsp-le tshy timer */
#define ABCSP_CANCEL_TSHY_TIMER     xa_cancel_tshy_timer

/* Cancel the bcsp-le tconf timer */
#define ABCSP_CANCEL_TCONF_TIMER    xa_cancel_tconf_timer

/*
 * From config_malloc.h
 */
/* Allocate a block of memory */
#define ABCSP_MALLOC        xa_malloc

/* Allocate a block of zeroed memory */
#define ABCSP_ZMALLOC       xa_zmalloc

/* Liberate a block of memory */
#define ABCSP_FREE          xa_free

/*
 * From config_panic.h
 */
/* Report a panic from the abcsp code */
#define ABCSP_PANIC         xa_panic

