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
*  permission of MediaTek Inc. (C) 2005
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
 * BtMainThread.c
 *
 * Project:
 * --------
 *   BT Project
 *
 * Description:
 * ------------
 *   This file is used to
 *
 * Author:
 * -------
 * Dlight Ting
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: 
 * $Modtime:
 * $Log: 
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#include <stdio.h>
#if !defined(BTMTK_ON_LINUX)
#include <windows.h> /* For the CreateThread prototype */
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/poll.h>
#include <linux/msg.h>
#include <signal.h>
#include <linux/futex.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif
#include "bt_types.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bt_adp_msg.h"
#include "bt_adp_system.h"
#include "bluetooth_gap_struct.h"
#include "bt_feature.h"

#include "tst_ringbuffer.h"

#if defined(__ENABLE_BTNET_DEV__)
#include "btnet.h"
#endif

#if defined(__ENABLE_SLEEP_MODE__)
#include "eint.h"
#endif
#ifndef __MTK_BT_EXTERNAL_PLATFORM__
#include "bt_drv.h"
#endif


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif 

//#define __BT_PAL__ 0

#define USE_SELECT

extern void BTUartTriggerReadUart(void);

extern int g_serverSocket;
extern int g_a2dpstreamSocket;
extern int g_clientSocket;

extern int startTst();
extern int tst_flush(int milisec, int force);

void timerSignalHandler(int data)
{
    ilm_struct ilm;
    ilm.msg_id = MSG_ID_TIMER_EXPIRY;
    ilm.src_mod_id = MOD_BT;
    ilm.dest_mod_id = MOD_BT;
    ilm.sap_id= INVALID_SAP;
    send (g_clientSocket, &ilm, (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE), MSG_DONTWAIT);
}

/*
void ExceptionHandler(int signum)
{
    printf("+Exception handler\n");
    tst_flush(3000, 1);
    printf("-Exception handler\n");
    signal(signum, SIG_DFL);
    kill(getpid, signum);
}
*/

extern void dumpTstStatus(void);
void userSigProcess(int data)
{
    int i;
    dumpTstStatus();
}
int BTMTK_Main( void ); 

#if defined(__SPP_THROUGHPUT_TEST__)
int g_bSppServer = KAL_FALSE;
#endif

int main (int argc, char **argv)
{
    bt_prompt_trace(MOD_BT, "__FUNCTION__ = %s\n", __FUNCTION__);
    bt_prompt_trace(MOD_BT, "[BT] main\n");

#if defined(__SPP_THROUGHPUT_TEST__)
    if(argc >= 2)
   	{
   		printf("argv[1] = %s\n", argv[1]);
   		if(atoi(argv[1]) > 0)
   			g_bSppServer = KAL_TRUE;
   	}
   	printf("g_bSppServer is %d\n", g_bSppServer?1:0);	   	
#endif
    bt_messagehdl_init();
    signal(SIGALRM, timerSignalHandler);
    signal(SIGPIPE, SIG_IGN);
    //signal(SIGUSR1, userSigProcess);
    //signal(SIGSEGV, ExceptionHandler);
    //signal(SIGBUS, ExceptionHandler);
    BTMTK_Main();
    return 0;
}

extern int commPort;
/* Start Tst thread */

#if defined(__ANDROID_EMULATOR__) || defined(__MTK_STP_SUPPORTED__)|| defined(__BT_TRANSPORT_DRV__)
int set_bluetooth_power(int on) 
{
    // STP will do the power on
    return 0;
}
#else /* __ANDROID_EMULATOR__ */
static int rfkill_id = -1;
static char *rfkill_state_path = NULL;
static int init_rfkill() {
    char path[64];
    char buf[16];
    int fd;
    int sz;
    int id;
    bt_prompt_trace(MOD_BT, "init_rfkill");
    for (id = 0; ; id++) {
        snprintf(path, sizeof(path), "/sys/class/rfkill/rfkill%d/type", id);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            bt_prompt_trace(MOD_BT, "[WARN] open(%s) failed: %s (%d)", path, strerror(errno), errno);
            return -1;
        }
        sz = read(fd, &buf, sizeof(buf));
        close(fd);
        if (sz >= 9 && memcmp(buf, "bluetooth", 9) == 0) {
            rfkill_id = id;
            break;
        }
    }
    asprintf(&rfkill_state_path, "/sys/class/rfkill/rfkill%d/state", rfkill_id);
    bt_prompt_trace(MOD_BT, "init_rfkill : path=%s, id=%d", rfkill_state_path, rfkill_id);
    return 0;
}
int set_bluetooth_power(int on) 
{
    int sz;
    int fd = -1;
    int ret = -1;
    const char buffer = (on ? '1' : '0');

    bt_prompt_trace(MOD_BT, "set_bluetooth_power(%d)", on);
    if (rfkill_id == -1) 
    {
        if (init_rfkill()) 
            goto out;
    }    
    fd = open(rfkill_state_path, O_WRONLY);
    if (fd < 0) 
    {
        bt_prompt_trace(MOD_BT, "[ERR] open(%s) for write failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        bt_android_log("[ERR] open(%s) for write failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        goto out;    
    }
    bt_android_log("write rfkill : %c", buffer);
    sz = write(fd, &buffer, 1);
    if (sz < 0) 
    {  
        bt_prompt_trace(MOD_BT, "[ERR] write(%s) failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        bt_android_log("[ERR] write(%s) failed: %s (%d)", rfkill_state_path, strerror(errno), errno);
        goto out;
    }
    ret = 0;
out:
    if (fd >= 0) 
        close(fd);
#if defined(__ENABLE_SLEEP_MODE__)
    if(on != 0)
    {
        if(ret == 0)
            initEint();
    }
    else
    {
        closeEint();
    }
#endif
    bt_android_log("set_bluetooth_power(%d)=%d", on, ret);
    return ret;
}
#endif

#if defined(__BT_HS_30__)	
extern int pal_main_begin();
extern int pal_main_middle(int ret, fd_set *prfds);
extern int pal_main_middle_all();
extern int pal_main_end(); 

#if defined(__BT_PAL__)
extern int getPalHandleNum();
extern int getPalHandle(int index);
extern int PalHandle_init(void);
int startPal(){
	return -1;
}
#else
// OOO:
static pid_t palpid = 0;
static pthread_t palthread;
static pthread_cond_t palcond;
kal_bool g_palStarted = KAL_FALSE;

// Use another therad
void* BTMTK_Pal(void *lpParam) 
{  
#if !defined(__MTK_BT_EXTERNAL_PLATFORM__)
	pal_main_begin();
	pal_main_middle_all();	
	pal_main_end();		
#endif
	return 0;
}

int startPal(){
    int res = -1;

    bt_tst_log("[pal]  startPal");
    if( 0 != pthread_cond_init(&palcond, NULL))
    {
        res = -1;
        bt_tst_err("[pal] pthread_cond_init failed");
    }
    else
    {
        bt_tst_log("[pal] pthread_cond_init success");
        if( 0 != pthread_create( &palthread, NULL, BTMTK_Pal, NULL) )
        {
            res = -1;
            pthread_cond_destroy(&palcond);
            bt_tst_err("[pal] pthread_create failed");
        }
        else
        {
            g_palStarted = KAL_TRUE;
            bt_tst_log("[pal] pthread_create success");
        }
    }
    return res;

}


#endif
#endif

//#define DETECT_BTUSB_RESUME move to mtkbt.inc
#ifdef DETECT_BTUSB_RESUME
static pthread_t Resumethread;


#if 1//HCI status
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#define BTPROTO_HCI	1

#define HCI_EVENT_PKT		0x04

struct sockaddr_hci {
	sa_family_t	hci_family;
	unsigned short	hci_dev;
	unsigned short  hci_channel;
};

struct hci_filter {
	uint32_t type_mask;
	uint32_t event_mask[2];
	uint16_t opcode;
};

#define HCI_FLT_TYPE_BITS	31
#define HCI_FLT_EVENT_BITS	63
#define HCI_VENDOR_PKT		0xff
#define HCI_EV_STACK_INTERNAL	0xfd

#define SOL_HCI		0
#define HCI_FILTER	2
#define HCI_DEV_NONE	0xffff

static inline int hci_test_bit(int nr, void *addr)
{
	return *((uint32_t *) addr + (nr >> 5)) & (1 << (nr & 31));
}

static inline void hci_set_bit(int nr, void *addr)
{
	*((uint32_t *) addr + (nr >> 5)) |= (1 << (nr & 31));
}

static inline void hci_filter_clear(struct hci_filter *f)
{
	memset(f, 0, sizeof(*f));
}

static inline void hci_filter_set_ptype(int t, struct hci_filter *f)
{
	hci_set_bit((t == HCI_VENDOR_PKT) ? 0 : (t & HCI_FLT_TYPE_BITS), &f->type_mask);
}

static inline void hci_filter_set_event(int e, struct hci_filter *f)
{
	hci_set_bit((e & HCI_FLT_EVENT_BITS), &f->event_mask);
}

int _hci_open_dev(int dev_id)
{
    struct sockaddr_hci a;
    int sock, err;

    /* Create HCI socket */
    sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sock < 0)
    	return sock;

    /* Bind socket to the HCI device */
    memset(&a, 0, sizeof(a));
    a.hci_family = AF_BLUETOOTH;
    a.hci_dev = dev_id;
    if (bind(sock, (struct sockaddr *) &a, sizeof(a)) < 0)
    	goto failed;

    {
        struct hci_filter flt;
        hci_filter_clear(&flt);
        hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
        hci_filter_set_event(HCI_EV_STACK_INTERNAL, &flt);
        if (setsockopt(sock, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
            bt_prompt_trace(MOD_BT, "Can't set filter on hci%d: %s (%d)",
            				index, strerror(errno), errno);
            goto failed;
        }

        if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK) < 0) {
            bt_prompt_trace(MOD_BT, "Can't set non blocking mode: %s (%d)\n",
            				strerror(errno), errno);
            goto failed;
        }        
    }

    return sock;

failed:
    err = errno;
    if(sock >= 0){
        close(sock);
    }
    errno = err;
    return -1;
}

int _hci_close_dev(int sock)
{	
	return close(sock);
}

#define HCI_DEV_REG			1
#define HCI_DEV_UNREG			2
#define HCI_DEV_UP			3
#define HCI_DEV_DOWN			4
#define HCI_DEV_SUSPEND			5
#define HCI_DEV_RESUME			6

#define HCI_EVENT_HDR_SIZE   2
#define HCI_ACL_HDR_SIZE     4
#define HCI_SCO_HDR_SIZE     3

#define HCI_DEV_NONE	0xffff

/* Internal events generated by BlueZ stack */
#define EVT_STACK_INTERNAL		0xFD
typedef struct {
	uint16_t	type;
	uint8_t	data[0];
} __attribute__ ((packed)) evt_stack_internal;
#define EVT_STACK_INTERNAL_SIZE 2

#define EVT_SI_DEVICE	0x01
typedef struct {
	uint16_t	event;
	uint16_t	dev_id;
} __attribute__ ((packed)) evt_si_device;
#define EVT_SI_DEVICE_SIZE 4

typedef struct {
	__u8	evt;
	__u8	plen;
} __attribute__ ((packed)) hci_event_hdr;

static void device_event(int event, int index)
{
    static int count;
    static int ready_to_reset = 0;
    bt_prompt_trace(MOD_BT, "device event %d, index %d", event, index);
    bt_android_log("device event %d, index %d", event, index);

    switch (event) {
    case HCI_DEV_REG:
    	bt_prompt_trace(MOD_BT, "HCI dev %d registered", index);
       bt_android_log("HCI dev %d registered", index);
    	break;

    case HCI_DEV_UNREG:
    	bt_prompt_trace(MOD_BT, "HCI dev %d unregistered", index);
       bt_android_log("HCI dev %d unregistered", index);
    	break;

    case HCI_DEV_UP:
    	bt_prompt_trace(MOD_BT, "HCI dev %d up", index);
       //if(count++ > 0 && commPort >= 0)
       // Send to BT task and let BT task to decide if reset is necessary
       BT_SendMessage(MSG_ID_BT_RESET_REQ_IND, MOD_BT, NULL, 0);
    	break;

    case HCI_DEV_DOWN:
        if( commPort >= 0 ){
            bt_prompt_trace(MOD_BT, "Ready to reset");
            ready_to_reset = 1;
        }
    	bt_prompt_trace(MOD_BT, "HCI dev %d down", index);
    	break;
    }
}

void Handle_stack_event(U8 *ptr){
    evt_stack_internal *si;
    evt_si_device *sd;
    hci_event_hdr *eh;
    int type, fd;
    ssize_t len;

    type = *ptr++;

    if (type != HCI_EVENT_PKT){
    	return 0;
    }

    eh = (hci_event_hdr *) ptr;
    if (eh->evt != EVT_STACK_INTERNAL){
    	return 0;
    }

    ptr += HCI_EVENT_HDR_SIZE;

    si = (evt_stack_internal *) ptr;
    switch (si->type) {
    case EVT_SI_DEVICE:
    	sd = (void *) &si->data;
    	device_event(sd->event, sd->dev_id);
    	break;
    }    
}

void* ListenResume(void *lpParam) 
{
    fd_set readfs;
    int res = -1;
    int sock = -1;

    sock = _hci_open_dev(HCI_DEV_NONE);
    if(sock < 0){
        bt_prompt_trace_lv3(MOD_BT,"[RESUME] create monitor sockfailed : %d, %s", errno, strerror(errno));
        bt_android_log("[RESUME] create monitor sockfailed : %d, %s", errno, strerror(errno));
        return 0;
    }
    do{
        FD_ZERO(&readfs);
        FD_SET(sock, &readfs);
        res = select(sock+1, &readfs, NULL, NULL, NULL);
        if(res == 0){
            bt_prompt_trace(MOD_BT, "[BT] monitor select timeout");
        }else if(res > 0){
            if(sock >= 0 && FD_ISSET(sock, &readfs)){
                U8 buf[256];
                int len;
                bt_prompt_trace_lv3(MOD_BT,"[RESUME] monitor sock triggered");
                bt_android_log("[RESUME] monitor sock triggered");
                len = read(sock, buf, sizeof(buf));
                bt_prompt_trace_lv3(MOD_BT,"[RESUME] read ret %d, errno=%d", len, errno);
                bt_android_log("[RESUME] read ret %d, errno=%d", len, errno);
                if(len > 0){
                    Handle_stack_event(buf);
                }
            }
        }else{
            bt_prompt_trace_lv3(MOD_BT,"[RESUME] select ret %d, errno=%d", res, errno);
            bt_android_log("[RESUME] select ret %d, errno=%d", res, errno);            
        }
    }while(1);
    _hci_close_dev(sock);
    return 0;

}


#else

#if 0 //btusb hc ioctl
#define PARAM_POWER_EVENT   _IOW('H', 7630, int)
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#define BTPROTO_HCI	1

struct sockaddr_hci {
	sa_family_t	hci_family;
	unsigned short	hci_dev;
	unsigned short  hci_channel;
};

int _hci_open_dev(int dev_id)
{
	struct sockaddr_hci a;
	int sock, err;

	/* Create HCI socket */
	sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (sock < 0)
		return sock;

	/* Bind socket to the HCI device */
	memset(&a, 0, sizeof(a));
	a.hci_family = AF_BLUETOOTH;
	a.hci_dev = dev_id;
	if (bind(sock, (struct sockaddr *) &a, sizeof(a)) < 0)
		goto failed;

	return sock;

failed:
	err = errno;
	close(sock);
	errno = err;

	return -1;
}

int _hci_close_dev(int sock)
{	
	return close(sock);
}

void* ListenResume(void *lpParam) 
{

    int sock;
    int devid=0;
    volatile int resume_state =-1;
    volatile int Comm=-1;

        while(1)
        {
            Comm = commPort;
            
            if (commPort > 0)
            {
                 resume_state= ioctl(commPort, PARAM_POWER_EVENT, 0);
                 
                 bt_prompt_trace_lv3(MOD_BT,"resume value = %d, sock=%d\n", resume_state,commPort);
                 
                if (resume_state >0)
                 {        
                        BT_SendMessage(MSG_ID_BT_RESET_REQ_IND, MOD_MMI, NULL, 0);
                        bt_prompt_trace_lv3(MOD_BT,"BTUSB resume");
                        bt_android_log("BTUSB resume");
                        

                  }
                }
              usleep(600000);
            }
    
        return 0;

}

#else
#define MAX_PROC_SIZE 16
volatile static char proc_data[MAX_PROC_SIZE]={};

void* ListenResume(void *lpParam) 
{  
        FILE * fp = NULL;
        FILE *fp2 = NULL;
        volatile int Comm=-1;
        static int count = 0;
      	fp2 = fopen("/proc/blueangel", "w");
      	if (fp2)
      	{
      		fprintf(fp2,"0");
      		fclose(fp2);
            bt_prompt_trace_lv3(MOD_BT,"[resume] reset proc");
            bt_android_log("[resume] reset proc");
      	}
      	
        while (1)
        {
            Comm = commPort;
            
           // if (Comm > 0)
            {
                  fp = fopen("/proc/blueangel", "rw");
                if (fp)
                {
                    fscanf(fp, "%s", proc_data);
                    
                    if (!strcmp(proc_data, "2") && (count++) > 0)
                    {
                        bt_prompt_trace_lv3(MOD_BT,"BTUSB resume");
                        bt_android_log("BTUSB resume");
                        
                           	fp2 = fopen("/proc/blueangel", "w");
                      	if (fp2)
                      	{
                      		fprintf(fp2,"0");
                      		fclose(fp2);
                      	}
                      	
                         //   HciTransportReset();
                          if (commPort > 0)
                            BT_SendMessage(MSG_ID_BT_RESET_REQ_IND, MOD_MMI, NULL, 0);
                    }
                    fclose(fp);
                    
                }
                else
                {
                        bt_prompt_trace_lv3(MOD_BT,"/proc/blueangel not exist");
                        bt_android_log("/proc/blueangel not exist");
                }
            }
            usleep(600000);
            
        }
        
	return 0;
}
#endif
#endif

void StartListen()
{
    if( 0 != pthread_create( &Resumethread, NULL, ListenResume, NULL) )
    {
        bt_prompt_trace_lv3(MOD_BT,"[resume] pthread_create failed");
    }
    else
    {
        bt_prompt_trace_lv3(MOD_BT,"[resume] pthread_create success ");
    }
}
#endif

int BTMTK_Main( void ) 
{ 
    int i;
    int btpalfd; /* pal temp file descriptor */
    ilm_struct *ptr = 0;
#ifndef __MTK_BT_EXTERNAL_PLATFORM__
    BT_RESULT result;
#endif    
    int ret;
	
    #if defined(__ENABLE_SLEEP_MODE__)
    int eintfd = -1;
    #endif
    #if defined(__ENABLE_BTNET_DEV__)
    int btnetfd = -1;
    #endif    
    
    bt_prompt_trace_lv3(MOD_BT, "BT main\n");

#if 0
    /* check address in NVRM */
    if( mtk_bt_op(BT_COLD_OP_GET_ADDR, &result) < 0 || result.buffer[0] == 0){
        OS_Report("No nvrm address exist. enable / disable driver to make address wrote to NVRM");
        ret = mtk_bt_enable(0, NULL);
        if(ret >= 0){
            mtk_bt_disable(ret);
            ret = mtk_bt_op(BT_COLD_OP_GET_ADDR, &result);
            if(ret < 0 || result.buffer[0] == 0){
                OS_Report("mtk_bt_op return %d, result=%u", ret, result.buffer[0]);
            }else{
                BTCoreSetNVRMBDAddr(&result.buffer[1]);
            }
        }else{
            OS_Report("mtk_bt_enable failed");
        }
    }else{
        BTCoreSetNVRMBDAddr(&result.buffer[1]);
    }
#endif

    /* +VSNIFF */
    //initVirtualSniff();
    /* -VSNIFF */

    btmtk_adp_system_init();

    #if defined(__USE_CATCHER_LOG__) && !defined(__PLATFORM_SUPPORTED_CATCHER__)
    startTst();
    #endif

#if defined(__BT_HS_30__)		
#if defined(__BT_PAL__)
	startPal();
	pal_main_begin();
#endif
#endif

#ifdef DETECT_BTUSB_RESUME
    StartListen();
#endif    
    while(1)
    {
        int res = 0;
        fd_set readfs;
		int g_max_set_fd = 0;
        //struct timeval Timeout;

        //Timeout.tv_usec = 500;
        //Timeout.tv_sec  = 0;
        FD_ZERO(&readfs);
        if(commPort >= 0)
        {
            FD_SET(commPort, &readfs);
        }
        if(g_serverSocket >= 0)
        {
            FD_SET(g_serverSocket, &readfs);
			g_max_set_fd = max (g_max_set_fd, g_serverSocket);
        }
        else
        {
            bt_prompt_trace(MOD_BT, "[BT] g_serverSocket == 0. exit");
        }        
        if(g_a2dpstreamSocket >= 0)
        {
            FD_SET(g_a2dpstreamSocket, &readfs);
			g_max_set_fd = max (g_max_set_fd, g_a2dpstreamSocket);
        }
        /* Add sleep mode eint */
        #if defined(__ENABLE_SLEEP_MODE__)
        eintfd = getEintHandle();
        if(eintfd >= 0)
        {
            FD_SET(eintfd, &readfs);
			g_max_set_fd = max (g_max_set_fd, eintfd);			
        }
        #endif
        #if defined(__ENABLE_BTNET_DEV__)
        btnetfd = getBtNetHandle();
        if (btnetfd >= 0)
        {
            FD_SET(btnetfd, &readfs);
			g_max_set_fd = max (g_max_set_fd, btnetfd);			
        }
        #endif
	    #if defined(__BT_HS_30__)		
		#if defined(__BT_PAL__)	
		{
			int maxfd = 0;
			PalHandle_init();			
			maxfd = getPalHandleNum();
			/* Add the pal sockets */
			for( i = 0; i< maxfd; i++ ){
				btpalfd = getPalHandle(i);
	    	    if (btpalfd >= 0) 
	        	{
	            	FD_SET(btpalfd, &readfs);
					g_max_set_fd = max (g_max_set_fd, btpalfd);			
	    	    }else{
	    	    	// fail to get the pad fd !
	    	    	bt_prompt_trace_lv2(MOD_BT, "[BT][PAL] fail to get pal fds index:%d of total:%d", i, getPalHandleNum() );
	    	    } 
			}
		}
		#endif
		#endif

        bt_prompt_trace_lv3(MOD_BT, "[BT] start select g_serverSocket=%d, commPort=%d g_a2dpstreamSocket=%d", g_serverSocket, commPort, g_a2dpstreamSocket);

#if 0		
        #if defined(__ENABLE_SLEEP_MODE__)
        #if defined(__ENABLE_BTNET_DEV__)
        res = select(max(commPort, max(g_serverSocket,max(g_a2dpstreamSocket,max(eintfd, btnetfd))))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #else
        res = select(max(commPort, max(g_serverSocket,max(g_a2dpstreamSocket,eintfd)))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #endif
        #else
        #if defined(__ENABLE_BTNET_DEV__)
        res = select(max(commPort, max(g_serverSocket,max(g_a2dpstreamSocket, btnetfd)))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #else
        res = select(max(commPort, max(g_serverSocket,g_a2dpstreamSocket))+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
        #endif
        #endif
#else
        res = select(max(commPort, g_max_set_fd)+1, 
                            &readfs, NULL, NULL, NULL/*&Timeout*/);
#endif
		
        bt_prompt_trace_lv3(MOD_BT, "[BT] select return : %d", res);
        if(res < 0)
        {
            bt_prompt_trace(MOD_BT, "[BT] select failed : %s, errno=%d", strerror(errno), errno);
            continue;
        }

        if(res == 0)
        {
            bt_prompt_trace(MOD_BT, "[BT] wait uart timeout");
            btmtk_adp_system_main_loop();
        }
        else
        {
            #if defined(__ENABLE_SLEEP_MODE__)
            if(eintfd >= 0 && FD_ISSET(eintfd, &readfs))
            {
                ilm_struct ilm;
                ilm.msg_id = MSG_ID_BT_HOST_WAKE_UP_IND;
                bt_prompt_trace(MOD_BT, "[SM] BT Eint triggered");
                bt_main(&ilm);
            }
            #endif
            #if defined(__ENABLE_BTNET_DEV__)
            if (btnetfd >= 0 && FD_ISSET(btnetfd, &readfs))
            {
                ilm_struct ilm;
                ilm.msg_id = MSG_ID_BT_PAN_SEND_PACKET_REQ;
                bt_prompt_trace(MOD_BT, "[BT] PAN network device ready to read");
                bt_main(&ilm);
            }
            #endif
            if (commPort >= 0 && FD_ISSET(commPort, &readfs))
            {
                   bt_prompt_trace_lv3(MOD_BT, "[BT] com buffer filled");
                   BTUartTriggerReadUart();
            }
            if(g_serverSocket >= 0 && FD_ISSET(g_serverSocket, &readfs))
            {
                U16 *field;

//                bt_prompt_trace_lv3(MOD_BT, "[MSG] message received");
                ptr = (ilm_struct *)bt_deQueue_message(MOD_BT);
		if(NULL != ptr)
                {
                if(ptr->src_mod_id == MOD_PAL)
                {
                /// OOO: pal -> bt
                    field = (U16 *)ptr->ilm_data;
                    *field = 1;
                    field = (U16 *)&ptr->ilm_data[2];
                    if(*field > 100)
                        *field = 100;                
                    if(*field != 100)
                        bt_log_primitive(ptr);
                }
                else
                    bt_log_primitive(ptr);                    

                bt_main(ptr);
            }
            }
            if(g_a2dpstreamSocket >= 0 && FD_ISSET(g_a2dpstreamSocket, &readfs))
            {
                int result = 0;
                ilm_struct ilm;
                bt_prompt_trace_lv3(MOD_BT, "[MSG] message received from g_a2dpstreamSocket");
                res = recvfrom(g_a2dpstreamSocket, (void*)&ilm, sizeof(ilm_struct), 0, NULL, NULL);
                bt_log_primitive(&ilm);
                bt_main(&ilm);
            }
#if defined(__BT_HS_30__)
#if defined(__BT_PAL__)
			/// init pal check loop
			pal_main_middle(res, &readfs);
#endif
#endif
        }
    }
    bt_prompt_trace(MOD_BT, "[BT] BTMTK_Main exit!!");

#if defined(__BT_HS_30__)		
#if defined(__BT_PAL__)
	pal_main_end();
#endif
#endif

    return 0;
} 

