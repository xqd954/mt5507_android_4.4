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

#include "bt_types.h"      	/* Basic data type */
#include "bt_common.h"
#include "hci.h"
#include "parseopts.h"
#include "hcitrans.h"
#include "btalloc.h"
#include "sys/debug.h"
#include "osapi.h"
#include "hci.h"
#include "uart.h"
#include "btalloc.h"
#include "bt_uartif.h"
#include "x_file.h"

#if defined(BTMTK_ON_LINUX)
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#if !defined(__BT_TRANSPORT_DRV__) 
#include <linux/serial.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#include "uart_linux.h"
#include "eint.h"
#endif

#define RADIO_STATE_DOWN      0
#define RADIO_STATE_READY     1
#define RADIO_STATE_INIT      3
#define RADIO_STATE_SHUTDOWN  4

#define MTK_BT_CHIP_VERSION_DEFAULT         0xFF
#define STR_FOUNDRY_VERSION_POSITION        0x07
#define STR_HW_ECO_VERSION_POSITION         0x08

static RadioCallback  radioCallback;  /* Pointer to the callback function that
                                       * must be called when initialization or
                                       * deinit happen asynchronously.
                                       */

static U8             radioState;     /* Current state of initialization or
                                       * shutdown.
                                       */
#if defined(BTMTK_ON_LINUX)
static BtStatus GORM_Init(void);
static BtStatus GORM_Shutdown(void);
#endif

extern BtStatus HciTransmitCommand(U16 opCode, U8 parmLen, HciCommand *cmd, BOOL internal, BOOL insertTail);

static BtStatus RADIO_Init_Default(void)
{
    return HCI_Reset();
}

BtStatus RADIO_Init(RadioCallback Callback)
{
    BtStatus status;

    radioCallback = Callback;
    radioState = RADIO_STATE_INIT;

#if defined(BTMTK_ON_WISE)
    switch(getRadioType())
    {
#if defined (__BTMODULE_MT6601__) || defined (__BTMODULE_MT6611__) || defined (__BTMODULE_MT6612__) || defined (__BTMODULE_MT6616__) || defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__) || defined (__BT_BTWIFI_COMBO_CHIP__) 
        case RADIO_TYPE_MTKBT_UART:
            status = GORM_Init();
            break;
#endif
        default:
            status = RADIO_Init_Default();
            break;
    }
#elif defined(BTMTK_ON_LINUX)
    #ifdef __BT_TRANSPORT_DRV__
    #else /* __BT_TRANSPORT_DRV__ */
    switch(getRadioType())
    {
    #if defined (__BTMODULE_MT6601__) || defined (__BTMODULE_MT6611__) || defined (__BTMODULE_MT6612__) || defined (__BTMODULE_MT6616__) || defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__) || defined (__BT_BTWIFI_COMBO_CHIP__) 
        case RADIO_TYPE_MTKBT_UART:
        status = GORM_Init();
        if(status != BT_STATUS_SUCCESS)
            return status;
        break;
    #endif
    default:
        break;
    }
    #endif /* __BT_TRANSPORT_DRV__ */
    status = RADIO_Init_Default();
#else
    status = RADIO_Init_Default();
#endif  /* BTMTK_ON_WISE */
    return status;
}

BtStatus RADIO_Shutdown(void)
{
	BtStatus status = BT_STATUS_PENDING;

	if (radioState == RADIO_STATE_READY)
	{
		radioState = RADIO_STATE_SHUTDOWN;
#if defined(BTMTK_ON_WISE)
		switch (getRadioType()) {
    #if defined (__BTMODULE_MT6601__) || defined (__BTMODULE_MT6611__) || defined (__BTMODULE_MT6612__) || defined (__BTMODULE_MT6616__) || defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__) || defined (__BT_BTWIFI_COMBO_CHIP__) 
        case RADIO_TYPE_MTKBT_UART:
		status = GORM_Shutdown();
		break;
    #endif
		default:
		status = BT_STATUS_SUCCESS;
		break;
		}
#elif defined(BTMTK_ON_LINUX)
    #ifdef __BT_TRANSPORT_DRV__
    status = BT_STATUS_SUCCESS;
    #else /* __BT_TRANSPORT_DRV__ */
    switch (getRadioType()) {
    #if defined (__BTMODULE_MT6601__) || defined (__BTMODULE_MT6611__) || defined (__BTMODULE_MT6612__) || defined (__BTMODULE_MT6616__) || defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__) || defined (__BT_BTWIFI_COMBO_CHIP__) 
    case RADIO_TYPE_MTKBT_UART:
        status = GORM_Shutdown();
        break;
    #endif
    default:
        status = BT_STATUS_SUCCESS;
        break;
    }
    #endif /* __BT_TRANSPORT_DRV__ */
#else
    status = BT_STATUS_SUCCESS;
#endif  /* BTMTK_ON_WISE */
	}
	return status;
}

/*---------------------------------------------------------------------------
 *            RADIO_EventHandler_Default()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Handles HCI events for radios.
 *
 * Returns:   <See RADIO_EventHandler in hcitrans.h>
 */
void RADIO_EventHandler_Default(U8 Event, HciCallbackParms *Parms)
{
    BtStatus   status;
    RadioEvent radioEvent;

    if ( (Event == HCI_CONTROLLER_EVENT) &&
        #if defined(BTMTK_ON_LINUX)
#ifdef __MTK_BT_EXTERNAL_PLATFORM__
         (LEtoHost16(&(Parms->ptr.hciEvent->parms[1])) == HCC_RESET) 
#else
         (LEtoHost16(&(Parms->ptr.hciEvent->parms[1])) == HCC_READ_LOCAL_VERSION) 
#endif         
        #else
         (LEtoHost16(&(Parms->ptr.hciEvent->parms[1])) == HCC_RESET) 
        #endif
         ){

        radioEvent = RADIO_INIT_STATUS;
        if(Parms->status == BT_STATUS_SUCCESS) {
            radioState = RADIO_STATE_READY;
            status = BT_STATUS_SUCCESS;
        } else {
            radioState = RADIO_STATE_INIT;
            status = BT_STATUS_FAILED;
        }

        /* Deliver event to Radio Manager */
        radioCallback(radioEvent, status);
        return;
    }
}

void RADIO_EventHandler(HciEventType Event, HciCallbackParms *Parms)
{
#ifdef BTMTK_ON_WISE
    switch (getRadioType()) {
#if defined (__BTMODULE_MT6601__) || defined (__BTMODULE_MT6611__) || defined (__BTMODULE_MT6612__) || defined (__BTMODULE_MT6616__) || defined (__BTMODULE_MT6236__) || defined (__BTMODULE_MT6255__) || defined (__BTMODULE_MT6276__) || defined (__BT_BTWIFI_COMBO_CHIP__) 
        case RADIO_TYPE_MTKBT_UART:
            GORM_InitEventHandler(Event, Parms);
            break;
#endif
        default:
            RADIO_EventHandler_Default(Event, Parms);
    }
#else   /* BTMTK_ON_WISE */
    RADIO_EventHandler_Default(Event, Parms);
#endif  /* BTMTK_ON_WISE */
}

#if defined(BTMTK_ON_LINUX) && !defined(__BT_TRANSPORT_DRV__)
#if !defined(__ANDROID_EMULATOR__)
extern int commPort;
struct uart_t {
	char *type;
	int  m_id;
	int  p_id;
	int  proto;
	int  init_speed;
	int  speed;
	int  flags;
	char *bdaddr;
	int  (*init) (int fd, struct uart_t *u, struct termios *ti);
	int  (*post) (int fd, struct uart_t *u, struct termios *ti);
};

typedef int (*SETUP_UART_PARAM)(unsigned long hComPort, int iBaudrate, int iFlowControl);
typedef void (*SET_UART_SETUP)(SETUP_UART_PARAM setup);
typedef int (*INIT)(int fd, struct uart_t *u, struct termios *ti);
typedef int (*RESTORE)(int fd);

#define FLOW_CTL	  0x0001
#define FLOW_CTL_HW	  0x0001
#define FLOW_CTL_SW	  0x0002
#define FLOW_CTL_NONE 0x0000
#define FLOW_CTL_MASK 0x0003

static int get_uart_speed(int baud)
{
	switch (baud) {
	case 115200:
		return B115200;
	default:
		return B57600;
	}
}

int set_speed(int fd, struct termios *ti, int speed)
{
    int ret;
    struct serial_struct ss;
    int baudenum = get_uart_speed(speed);

    if((ioctl(fd,TIOCGSERIAL,&ss))<0){  
        bt_prompt_trace(MOD_BT, "%s: BAUD: error to get the serial_struct info:%s",__func__, strerror(errno));  
        return -1;  
    }

    if ((baudenum == B57600) && (speed != 57600)) {
        /* struct serial_struct ss;  */
        bt_prompt_trace(MOD_BT, "%s: non-standard baudrate: %d", __func__, speed);
        //cfsetispeed(ti,B38400);  
        //cfsetospeed(ti,B38400);  
        tcflush(fd,TCIFLUSH);/*handle unrecevie char*/  
        //tcsetattr(fd,TCSANOW,ti);  
        /*
        if((ioctl(fd,TIOCGSERIAL,&ss))<0){  
            bt_prompt_trace(MOD_BT, "%s: BAUD: error to get the serial_struct info:%s",__func__, strerror(errno));  
            return -1;  
        }*/
        if(!(ss.flags & ASYNC_SPD_CUST)) {
            bt_prompt_trace(MOD_BT, "ASYNC_SPD_CUST is not set. Set ASYNC_SPD_CUST");
            ss.flags |= ASYNC_SPD_CUST;  
            ss.custom_divisor = ss.baud_base / speed;  
            if(ss.custom_divisor*speed != ss.baud_base )
                ss.custom_divisor = speed | (1<<31);
            if((ioctl(fd,TIOCSSERIAL,&ss))<0){  
                bt_prompt_trace(MOD_BT, "BAUD: error to set serial_struct:%s",strerror(errno));  
                return -2;  
            }
        }
        return 0;
    } else {
        bt_prompt_trace(MOD_BT, "%s: standard baudrate: %d -> %d", __func__, speed, baudenum);
        if(ss.flags & ASYNC_SPD_CUST) {
            bt_prompt_trace(MOD_BT, "ASYNC_SPD_CUST is set. Clear ASYNC_SPD_CUST");
            ss.flags &= ~ASYNC_SPD_CUST;
            ss.custom_divisor = 1;
            if((ioctl(fd,TIOCSSERIAL,&ss))<0){
                bt_prompt_trace(MOD_BT, "BAUD: error to set serial_struct:%s",strerror(errno));
                return -2;  
            }            
        }
        cfsetospeed(ti, baudenum);
        cfsetispeed(ti, baudenum);
        tcsetattr(fd, TCSANOW, ti);        
        return 0;
    }
}

int setup_uart_param(
    unsigned long hComPort, 
    int iBaudrate, 
    int iFlowControl)
{
	struct termios ti;
	int  fd;

        bt_prompt_trace(MOD_BT, "[UART] setup_uart_param %d %d\n", iBaudrate, iFlowControl);

	fd = (int)hComPort;
	if (fd < 0) {
		bt_prompt_trace(MOD_BT, "[UART][ERR] Invalid serial port");
		return -1;
	}

	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &ti) < 0) {
		bt_prompt_trace(MOD_BT, "[UART][ERR] Can't get port settings");
		return -1;
	}

	cfmakeraw(&ti);

	ti.c_cflag |= CLOCAL;
    
    ti.c_cflag &= ~CRTSCTS;
    ti.c_iflag &= ~(IXON | IXOFF | IXANY | 0x80000000);

    /* HW flow control */
	if (iFlowControl == 1){
		ti.c_cflag |= CRTSCTS;
        bt_prompt_trace(MOD_BT, "[UART] HW flow control\n");
	}
	else if (iFlowControl == 2){ //MTK HW Xon/Xoff
        /* SW flow control */
		//ti.c_iflag |= (IXON | IXOFF | IXANY);
		ti.c_iflag |= 0x80000000;
            //ti.c_cflag |= CRTSCTS;
        bt_prompt_trace(MOD_BT, "[UART] SW flow control\n");
	}

	if (tcsetattr(fd, TCSANOW, &ti) < 0) {
		bt_prompt_trace(MOD_BT, "[UART][ERR] Can't set port settings");
		return -1;
	}

	/* Set baudrate */
	if (set_speed(fd, &ti, iBaudrate) < 0) {
		bt_prompt_trace(MOD_BT, "[UART][ERR] Can't set initial baud rate");
		return -1;
	}

	tcflush(fd, TCIOFLUSH);
       bt_prompt_trace(MOD_BT, "[UART] setup_uart_param success\n");
	return fd;
}
#endif /* !defined(__ANDROID_EMULATOR__) */

static void* getDriverHandle() {
    void *lib_handle;
    lib_handle = dlopen("/data/libhciattach_mtk.so", RTLD_LAZY);
    if (!lib_handle) 
    {
        bt_prompt_trace(MOD_BT, "%s\n", dlerror());
        /* not found use default */
        lib_handle = dlopen("libhciattach_mtk.so", RTLD_LAZY);
    }
    if (!lib_handle) 
    {
        bt_prompt_trace(MOD_BT, "[ERR] dlopen failed : %s\n", dlerror());
    }
    return lib_handle;
}

#if !defined(__ANDROID_EMULATOR__)
/* parameter block : 1 for block mode and 0 for non-block mode */
/* return value : < 0 means failed, and others means success */
static int setUartBlockMode(int handle, U8 block) 
{
    int val;

    /* uart handle is opened as blocking, change it to non-blocking */
    if( (val = fcntl(handle, F_GETFL, 0)) < 0) 
    {
        bt_prompt_trace(MOD_BT, "[UART][ERR] get handle flag failed");
    } 
    else
    {
        if(block == 0)
        {
            val |= O_NONBLOCK;
        }
        else
        {
            val &= ~O_NONBLOCK;
        }
        val = fcntl(commPort, F_SETFL, val);
    }
    bt_prompt_trace(MOD_BT, "setUartBlockMode(%d,%d)=%d", handle, block, val);
    return val;
}
#endif

static BtStatus GORM_Init(void)
{
#if !defined(__ANDROID_EMULATOR__)
    BtStatus status = BT_STATUS_FAILED;
    void *lib_handle;
    SET_UART_SETUP SetUartSetup = NULL;
    INIT mtk = NULL;

    bt_android_log("[BT] + GORM_Init");
    
    /* load BT driver */
    lib_handle = getDriverHandle();    
    if (!lib_handle) 
    {
        bt_prompt_trace(MOD_BT, "[UART][ERR] dlopen failed : %s\n", dlerror());
        return BT_STATUS_FAILED;
    }    

    mtk = dlsym(lib_handle, "mtk");
#ifdef __MTK_STP_SUPPORTED__
    if (!mtk) 
#else
    SetUartSetup = dlsym(lib_handle, "SetUartSetup");
    if ((!mtk) || (!SetUartSetup)) 
#endif
    {
        bt_prompt_trace(MOD_BT, "[UART][ERR] Can not find functions %s\n", dlerror());
        bt_android_log("[BT][ERR] GORM_Init : Can not find functions %s", dlerror());
        dlclose(lib_handle);
        return BT_STATUS_FAILED;
    }
#ifdef __MTK_STP_SUPPORTED__
    bt_android_log("[BT] + call mtk");
    if(!mtk(commPort, NULL, NULL))
    {
        bt_prompt_trace(MOD_BT, "[UART] mtk call return success\n");
        status = BT_STATUS_SUCCESS;
    }
    else
    {
        bt_prompt_trace(MOD_BT, "[UART][ERR] mtk call return failed\n");
        bt_android_log("[BT][ERR] mtk call return failed");
        status = BT_STATUS_FAILED;
    }
    bt_android_log("[BT] - call mtk : status=%d", status);
#else
    /* Start init  */
    SetUartSetup(setup_uart_param);
    if(commPort >= 0)
    {
        struct uart_t u;
        struct termios ti;
        memset(&u, 0, sizeof(u));
        u.flags &= ~FLOW_CTL_MASK;
        u.flags |=  FLOW_CTL_SW;
        u.speed = 3250000;
        //u.speed = 115200;
        
        if (tcgetattr(commPort, &ti) < 0) 
        {
            bt_prompt_trace(MOD_BT, "[UART][ERR] Can't get port settings");
        }
        else
        {
            bt_prompt_trace(MOD_BT, "[UART] Call mtk function\n");
            bt_android_log("[BT] + call mtk");
            if(!mtk(commPort, &u, &ti))
            {
                int val;
                bt_prompt_trace(MOD_BT, "[UART] mtk call return success\n");
                /* uart handle is opened as blocking, change it to non-blocking */
                if ( setUartBlockMode(commPort, 0) < 0 ) {
                    bt_android_log("[BT][ERR] setUartBlockMode return failed");
                    status = BT_STATUS_FAILED;
                }else {
                    status = BT_STATUS_SUCCESS;
                    unmaskEint();
                }
            }
            else
            {
                bt_prompt_trace(MOD_BT, "[UART][ERR] mtk call return failed\n");
                bt_android_log("[BT][ERR] mtk call return failed");
                status = BT_STATUS_FAILED;
            }
            bt_android_log("[BT] - call mtk : status=%d", status);
        }                
    }
    else
    {
        bt_prompt_trace(MOD_BT, "[UART][ERR] commPort is not opened yet\n");
    }
#endif
    /* Close lib_handler */
    dlclose(lib_handle);
    bt_android_log("[BT] - GORM_Init : ret=%d", status);
    return status;
#else /* !defined(__ANDROID_EMULATOR__) */
    return BT_STATUS_SUCCESS;
#endif
}

static BtStatus GORM_Shutdown(void)
{
#if !defined(__ANDROID_EMULATOR__)
#ifndef __MTK_STP_SUPPORTED__
    void* lib_handle;
    RESTORE bt_restore = NULL;
    SET_UART_SETUP SetUartSetup = NULL;
    /* restore bt driver */
    if(commPort >= 0) {
        lib_handle = getDriverHandle();
        if(lib_handle) {
            bt_prompt_trace(MOD_BT, "Start to restore bt driver");
            SetUartSetup = dlsym(lib_handle, "SetUartSetup");
            bt_restore = dlsym(lib_handle, "bt_restore");
            if(bt_restore && SetUartSetup) {
                setUartBlockMode(commPort, 1);
                SetUartSetup(setup_uart_param);
                bt_restore(commPort);
            } else {
                bt_prompt_trace(MOD_BT, "[ERR] can locate bt_restore function");
            }
        }
    } else {
        bt_prompt_trace(MOD_BT, "commPort is invalid when GORM_Shutdown");
    }
    //bt_prompt_trace(MOD_BT, "+GORM_Shutdown");
    BTUartLinuxShutdown();
    //bt_prompt_trace("set_bluetooth_power(0)");
    if( set_bluetooth_power(0) < 0)
    {
        //bt_prompt_trace(MOD_BT, "[ERR] failed to set bt power");
        return BT_STATUS_FAILED;
    }
    //bt_prompt_trace(MOD_BT, "-GORM_Shutdown");
#else
    BTUartLinuxShutdown();
#endif  /* __MTK_STP_SUPPORTED__ */
#endif /* !defined(__ANDROID_EMULATOR__) */
    return BT_STATUS_SUCCESS;
}

#if defined(__ENABLE_SLEEP_MODE__)
static HciCommandType wOpCode2;          /* Opcode of HCI command sent from init
                                       * script.
                                       */
static HciReducedCommand    hciHostWakeupCmd;   /* Structure for sending commands.  Only
                                       * one command can be sent at a time.
                                       */
void Radio_hostWakeup(void)
{
    HciCommandType wOpCode_enc;
    /*
    if((stack_query_boot_mode() != FACTORY_BOOT)
         &&(0==bt_MMI_RFtest_mode)
        )*/
    {
        bt_prompt_trace(MOD_BT, "[SM] Radio_hostWakeup");
        wOpCode2 = 0xFCC1;
        hciHostWakeupCmd.parms[0] = 0;
        //    Assert(IsEntryAvailable(&hciHostWakeupCmd.node));
        //Call host provided interface to send host wakeup dummy command
        if (IsEntryAvailable(&hciHostWakeupCmd.node))
        {
            bt_prompt_trace(MOD_BT, "[SM] IsEntryAvailable(&hciHostWakeupCmd.node) return true");
            //Encrypt HCI commands
            if (/*enc_state >= START_CMD_ENC*/ 1)
            {
                wOpCode_enc = BT_XFile_EncryptionCommandOpcode(wOpCode2);
            }
            else
            {
                wOpCode_enc = wOpCode2;
            }
#ifdef __ENABLE_SLEEP_MODE__ /* avoid compile error */
            HCI_HostWakeupStartTimer();
#endif /* __ENABLE_SLEEP_MODE__ */
            HciTransmitCommand(wOpCode_enc, 0, (HciCommand *)&hciHostWakeupCmd, TRUE, FALSE );
        }
    }
}

/* Handle things after controller is waked up 
(command complete of FCC1 is received) */
void Radio_hostWakedup(void)
{
    unmaskEint();
}

void Radio_hostSleep(void)
{
}
#else
/* Handle things after controller is waked up 
(command complete of FCC1 is received) */
void Radio_hostWakedup(void)
{
}
void Radio_hostWakeup(void)
{
}
#endif
#endif /* defined(BTMTK_ON_LINUX) */


