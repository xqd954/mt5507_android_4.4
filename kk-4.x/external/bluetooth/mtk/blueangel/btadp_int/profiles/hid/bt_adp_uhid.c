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

#ifdef __BT_HIDH_PROFILE__

/* copy from bt_adp_hidh.c */
//#include "stdio.h"              /* Basic C I/O functions */
#ifndef __LINUX_SUPPRESS_ERROR__
#ifndef BTMTK_ON_WISE
#include <Windows.h>
#endif
#endif /* __LINUX_SUPPRESS_ERROR__ */
//#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "bt.h"
#include "bt_os_api.h"
#include "osapi.h"
#include "hid.h"

#include "eventmgr.h"
//#include "kal_trace.h"  /* Trace Header Files */
//#include "bluetooth_trc.h"      /* bt trace definition */

#include "bluetooth_struct.h"
#include "btbm_adp.h"
#include "sdp_adp.h"
#include "l2cap_adp.h"
#include "bluetooth_hid_struct.h"

#include "bt_adp_hidh.h"
#include "sdap_devdb.h"
#include "me.h"
#include "ddb.h"


/* copy from btif_hh_co.c */
#include <ctype.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <linux/uhid.h>
#include <linux/input.h>


#include "bt_adp_uhid.h"

const char *dev_path = "/dev/uhid";

int uhid_device_count = 0;

/*Internal function to perform UHID write and error checking*/
int hidha_uhid_write(int fd, const struct uhid_event *ev)
{
    ssize_t ret;
    ret = write(fd, ev, sizeof(*ev));
    if (ret < 0){
        int rtn = -errno;
        bt_prompt_trace(MOD_BT, "[HID]uhid_write: Cannot write to uhid:%s", strerror(errno));
        return rtn;
        
    } else if (ret != sizeof(*ev)) {
        bt_prompt_trace(MOD_BT, "[HID]uhid_write: Cannot write to uhid:%s", strerror(errno));
        return -EFAULT;
    } else {
        return 0;
    }
}


/*******************************************************************************
**
** Function         hidha_uhid_create
**
** Description      This function is called in btif_hh.c to process DSCP received.
**
** Parameters       dev_handle  - device handle
**                  dscp_len    - report descriptor length
**                  *p_dscp     - report descriptor
**
** Returns          void
*******************************************************************************/
void hidha_uhid_create(int fd, char *dev_name, U16 vendor_id, U16 product_id,
                           U16 version, U8 ctry_code, U16 dscp_len, U8 *p_dscp)
{
    int result;
    struct uhid_event ev;

    if (fd < 0) {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create: Error: fd = %d", fd);
        return;
    }
    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create: fd = %d, name = [%s], dscp_len = %d", fd, dev_name, dscp_len);    
    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create: vendor_id = 0x%04x, product_id = 0x%04x, version= 0x%04x",
                            vendor_id, product_id, version, ctry_code);

    //Create and send hid descriptor to kernel
    memset(&ev, 0, sizeof(ev));
    ev.type = UHID_CREATE;
    strncpy((char*)ev.u.create.name, dev_name, sizeof(ev.u.create.name) - 1);
    ev.u.create.rd_size = dscp_len;
    ev.u.create.rd_data = p_dscp;
    ev.u.create.bus = BUS_BLUETOOTH;
    ev.u.create.vendor = vendor_id;
    ev.u.create.product = product_id;
    ev.u.create.version = version;
    ev.u.create.country = ctry_code;
    result = hidha_uhid_write(fd, &ev);

    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create: fd = %d, dscp_len = %d, result = %d", fd, dscp_len, result);

    if (result) 
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create: Error: failed to send DSCP, result = %d", result);

        /* The HID report descriptor is corrupted. Close the driver. */
        close(fd);
    }
    else
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create: success !");
    }
    
}

void hidha_uhid_destroy(int fd)
{
    struct uhid_event ev;
    int result;
    
    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_destroy: entry");
    
    memset(&ev, 0, sizeof(ev));
    ev.type = UHID_DESTROY;
    
    result = hidha_uhid_write(fd, &ev);

    if (result) 
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_destroy: fail !");
    }
    else
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_destroy: success !");
    }    

}

int hidha_uhid_input(int fd, U8* rpt, U16 len)
{
    struct uhid_event ev;
    int result;

    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_input: fd = %d", fd);

    memset(&ev, 0, sizeof(ev));
    ev.type = UHID_INPUT;
    ev.u.input.size = len;
    if(len > sizeof(ev.u.input.data))
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_input: fd = %d", fd);

        return -1;
    }
    memcpy(ev.u.input.data, rpt, len);

    result = hidha_uhid_write(fd, &ev);

    if (result) 
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_input: fail !");
    }
    else
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_input: success !");
    }

    
    return result;

}

/* Internal function to parse the events received from UHID driver*/
static int hidha_uhid_event(hidha_context_struct *cntx)
{
    struct uhid_event ev;
    ssize_t ret;
    
    memset(&ev, 0, sizeof(ev));
    if(!cntx)
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: Device not found !");
    }

    ret = read(cntx->fd, &ev, sizeof(ev));
    if (ret == 0) {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: Read HUP on uhid-cdev %s", strerror(errno));
        return -EFAULT;
    } else if (ret < 0) {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: Cannot read uhid-cdev:%s", strerror(errno));
        return -errno;
    } else if (ret != sizeof(ev)) {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: Invalid size read from uhid-dev: %ld != %lu", ret, sizeof(ev));
        return -EFAULT;
    }

    switch (ev.type) 
    {
        case UHID_START:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_START from uhid-dev !");
            break;
        case UHID_STOP:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_STOP from uhid-dev !");
            break;
        case UHID_OPEN:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_OPEN from uhid-dev !");
            break;
        case UHID_CLOSE:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_CLOSE from uhid-dev !");
            break;
        case UHID_OUTPUT:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_OUTPUT from uhid-dev !");
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: report type = %d, report size = %d",
                            ev.u.output.rtype, ev.u.output.size);            
            //Send SET_REPORT with feature report if the report type in output event is FEATURE
            /*
                    if(ev.u.output.rtype == UHID_FEATURE_REPORT)
                        btif_hh_setreport(p_dev,BTHH_FEATURE_REPORT,ev.u.output.size,ev.u.output.data);
                    else if(ev.u.output.rtype == UHID_OUTPUT_REPORT)
                        btif_hh_setreport(p_dev,BTHH_OUTPUT_REPORT,ev.u.output.size,ev.u.output.data);
                    else
                        btif_hh_setreport(p_dev,BTHH_INPUT_REPORT,ev.u.output.size,ev.u.output.data);
                    */
            break;
        case UHID_OUTPUT_EV:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_OUTPUT_EV from uhid-dev !");
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: report type = %d, report code = %d, report value = %d",
                            ev.u.output_ev.type, ev.u.output_ev.code, ev.u.output_ev.value);
            switch (ev.u.output_ev.type) 
            {
                case EV_LED:
                    hidha_event_leds(cntx, ev.u.output_ev.code);                    
                    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_FEATURE_ANSWER from uhid-dev !");
                    break;
                
                default:
                    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_OUTPUT_EV todo !");
            }
            
            break;
        case UHID_FEATURE:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_FEATURE from uhid-dev !");
            break;
        case UHID_FEATURE_ANSWER:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: UHID_FEATURE_ANSWER from uhid-dev !");
            break;

        default:
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_event: Invalid event from uhid-dev !");
        }

    return 0;
    
}

/*******************************************************************************
**
** Function hidha_uhid_create_thread
**
** Description creat a select loop
**
** Returns pthread_t
**
*******************************************************************************/
static inline pthread_t hidha_uhid_create_thread(void *(*start_routine)(void *), void * arg)
{
    pthread_attr_t thread_attr;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    pthread_t thread_id = -1;
    if ( pthread_create(&thread_id, &thread_attr, start_routine, arg)!=0 )
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create_thread : %s", strerror(errno));
        return -1;
    }
    
    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_create_thread: thread created successfully");
    
    return thread_id;
}

static inline void hidha_uhid_close_poll_thread(hidha_context_struct *cntx)
{
    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_close_poll_thread enter");
    
    cntx->uhid_keep_polling = 0;
    if(cntx->uhid_poll_thread_id > 0)
        pthread_join(cntx->uhid_poll_thread_id,NULL);

    return;
}


/*******************************************************************************
**
** Function btif_hh_poll_event_thread
**
** Description the polling thread which polls for event from UHID driver
**
** Returns void
**
*******************************************************************************/
static void *hidha_uhid_poll_event_thread(void *arg)
{

    //btif_hh_device_t *p_dev = arg;
    hidha_context_struct *cntx = arg;
    struct pollfd pfds[1];
    int ret;

    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_poll_event_thread:%d", cntx->fd);
    
    pfds[0].fd = cntx->fd;
    pfds[0].events = POLLIN;

    while(cntx->uhid_keep_polling)
    {
        ret = poll(pfds, 1, 500);
        if (ret < 0) 
        {
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_poll_event_thread: Cannot poll for fds: %s", strerror(errno));
            break;
        }
        if (pfds[0].revents & POLLIN) 
        {
            bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_poll_event_thread: POLLIN");
            ret = hidha_uhid_event(cntx);
            if (ret)
            {
                break;
            }
        }
    }
    
    cntx->uhid_poll_thread_id = -1;

    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_poll_event_thread: fail !");

    return 0;
}

void hidha_uhid_open(hidha_context_struct *cntx)
{
    BtStatus status = BT_STATUS_FAILED;
    char *cached_name = "MTK BT HID";
    SdapDevDBRecord sdapRecord;

    bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_open: uhid_device_count=%d",uhid_device_count);
	
	if(cntx->hidChannel == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_open:cntx->hidChannel == NULL");
		return;
	}

	if(cntx->hidChannel->reportCons[0] == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_open:cntx->hidChannel->reportCons[0] == NULL");
		return;
	}

    /* open UHID */
    if(cntx->fd <= 0) 
    {
        cntx->fd = open(dev_path, O_RDWR | O_CLOEXEC);
        if (cntx->fd < 0)
        {
            bt_prompt_trace(MOD_BT,"[HID]hidha_uhid_open:open UHID fail!");
            return;
        }else
        {
            bt_prompt_trace(MOD_BT,"[HID]hidha_uhid_open:open UHID success! fd=%d", cntx->fd);

            /* get HID device infomation */
            OS_MemSet((void*) &sdapRecord, 0, sizeof(SdapDevDBRecord));
            OS_MemCopy((void*) &sdapRecord.bdaddr, (const void*) &cntx->hidha_bd_addr, BD_ADDR_SIZE);
            status = SDAP_QueryDevDBRecord(&sdapRecord);
            bt_prompt_trace(MOD_BT,"[HID]hidha_uhid_open:SDAP_QueryDevDBRecord status:%d", status);

            /* send HID device infomation to HID sub-system */
            hidha_uhid_create(
                cntx->fd, cached_name, sdapRecord.vendorID, sdapRecord.productID, 
                0, 0, cntx->descLen, cntx->descStr);

            /* create thread */
            cntx->uhid_keep_polling = 1;
            cntx->uhid_poll_thread_id = hidha_uhid_create_thread(hidha_uhid_poll_event_thread, cntx);
            bt_prompt_trace(MOD_BT,"[HID]hidha_uhid_open:cntx->uhid_poll_thread_id=%d", cntx->uhid_poll_thread_id);

            uhid_device_count++;
        }
    }
    else
    {
        bt_prompt_trace(MOD_BT,"[HID]hidha_uhid_open:open UHID fail! reason: cntx->fd > 0");
        return;
    }

}


void hidha_uhid_close(hidha_context_struct *cntx)
{
	bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_close: when close uhid_device_count=%d",uhid_device_count);
	
	if(cntx->hidChannel == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_close: cntx->hidChannel == NULL");
		return;
	}

	if(cntx->hidChannel->reportCons[0] == NULL)
	{
		bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_close: cntx->hidChannel->reportCons[0] == NULL");
		return;
	}

	if(uhid_device_count == 0)
    {
        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_close: uhid_device_count=0");
        return;
    }

    if(cntx->fd > 0)
    {
        hidha_uhid_destroy(cntx->fd);
        
        close(cntx->fd);
        cntx->fd = -1;
        
        hidha_uhid_close_poll_thread(cntx);
        cntx->uhid_poll_thread_id = -1;

        uhid_device_count--;   

        bt_prompt_trace(MOD_BT, "[HID]hidha_uhid_close:success !");
    }

}






#endif //__BT_HIDH_PROFILE__
