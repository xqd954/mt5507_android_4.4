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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h> // open / O_RDWR
#include <utils/Log.h>
#include <linux/uinput.h>
#include <linux/input.h>
#include <errno.h>

#include "extavrcpkey_jni.h"

#if 0
#include "avrcp.h"  //TODO: btstack header !
#else
#define AVRCP_POP_POWER             0x40
#define AVRCP_POP_VOLUME_UP         0x41
#define AVRCP_POP_VOLUME_DOWN       0x42
#define AVRCP_POP_MUTE              0x43
#define AVRCP_POP_PLAY              0x44
#define AVRCP_POP_STOP              0x45
#define AVRCP_POP_PAUSE             0x46
#define AVRCP_POP_RECORD            0x47
#define AVRCP_POP_REWIND            0x48
#define AVRCP_POP_FAST_FORWARD      0x49
#define AVRCP_POP_EJECT             0x4A
#define AVRCP_POP_FORWARD           0x4B
#define AVRCP_POP_BACKWARD          0x4C
#define AVRCP_POP_CHANNEL_UP        0x30
#define AVRCP_POP_CHANNEL_DOWN      0x31
#define AVRCP_POP_SELECT            0x00
#define AVRCP_POP_UP                0x01
#define AVRCP_POP_DOWN              0x02
#define AVRCP_POP_LEFT              0x03
#define AVRCP_POP_RIGHT             0x04
#define AVRCP_POP_ROOT_MENU         0x09
#define AVRCP_POP_0                 0x20
#define AVRCP_POP_1                 0x21
#define AVRCP_POP_2                 0x22
#define AVRCP_POP_3                 0x23
#define AVRCP_POP_4                 0x24
#define AVRCP_POP_5                 0x25
#define AVRCP_POP_6                 0x26
#define AVRCP_POP_7                 0x27
#define AVRCP_POP_8                 0x28
#define AVRCP_POP_9                 0x29
#endif

#include "bt_ext_debug.h"

int g_uinput_fd = -1;


#define DBG_PRINTF bt_ext_log

static int send_event(int fd, uint16_t type, uint16_t code, int32_t value)
{
	struct input_event event;
	int i;

	memset(&event, 0, sizeof(event));
	event.type	= type;
	event.code	= code;
	event.value	= value;

	i = write(fd, &event, sizeof(event));
	if( i != sizeof(event) ){
		ALOGE("[BT][AVRCP] send_key eno:%d type:%d value:%d", errno, code, value);
		ALOGE("[BT][AVRCP][ERR] failed : %s, %d", strerror(errno), errno);
	}else if( code != SYN_REPORT ){ //SYN_REPORT is used for update status
		ALOGI("[BT][AVRCP] send_key eno:%d type:%d value:%d", errno, code, value);
	}
	return i;
}

static void send_key(int fd, uint16_t key, int pressed)
{
	int ret1, ret2;
	
	if (fd < 0)
		return;

	ret1 = send_event(fd, EV_KEY, key, pressed);
	ret2 = send_event(fd, EV_SYN, SYN_REPORT, 0);
}


/* AVRCP_KEY: avrcp.h KEYCODE: kernel/include/input.h */
static struct key_map_struct {
	const char *name;
	uint8_t avrcp;
	uint16_t keycode;
} key_map[] = {
	{ "PLAY",		AVRCP_POP_PLAY,		    KEY_PLAYCD },	/* category 1 TG */
	{ "STOP",		AVRCP_POP_STOP,	     	KEY_STOPCD }, // alps\bionic\libc\kernel\common\linux\input.h
	{ "PAUSE",		AVRCP_POP_PAUSE,		KEY_PAUSECD },
	{ "FORWARD",	AVRCP_POP_FORWARD,		KEY_NEXTSONG },
	{ "BACKWARD",	AVRCP_POP_BACKWARD,		KEY_PREVIOUSSONG },
	{ "REWIND",		AVRCP_POP_REWIND,		KEY_REWIND },
	{ "FAST FORWARD",	AVRCP_POP_FAST_FORWARD,	KEY_FASTFORWARD },
	{ "Vol_Up",		AVRCP_POP_VOLUME_UP,	KEY_VOLUMEUP }, /* category 2 TG */
	{ "Vol_Down",	AVRCP_POP_VOLUME_DOWN,	KEY_VOLUMEDOWN },
	{ "Chnl_Up",	AVRCP_POP_CHANNEL_UP,	KEY_CHANNELUP },/* category 3 TG */
	{ "Chnl_Down",	AVRCP_POP_CHANNEL_DOWN,	KEY_CHANNELDOWN },
	{ "Select",	    AVRCP_POP_SELECT,		KEY_SELECT },/* category 4 TG */
	{ "UP",	        AVRCP_POP_UP,			KEY_UP },
	{ "DOWN",	    AVRCP_POP_DOWN,			KEY_DOWN },
	{ "LEFT",	    AVRCP_POP_LEFT,			KEY_LEFT },
	{ "RIGHT",	    AVRCP_POP_RIGHT,		KEY_RIGHT },
	{ "ROOT_MENU",	AVRCP_POP_CHANNEL_DOWN,	KEY_MENU },
	{ "0",		AVRCP_POP_0,		KEY_0 },/* optional */
	{ "1",		AVRCP_POP_1,		KEY_1 },
	{ "2",		AVRCP_POP_2,		KEY_2 },
	{ "3",		AVRCP_POP_3,		KEY_3 },
	{ "4",		AVRCP_POP_4,		KEY_4 },
	{ "5",		AVRCP_POP_5,		KEY_5 },
	{ "6",		AVRCP_POP_6,		KEY_6 },
	{ "7",		AVRCP_POP_7,		KEY_7 },
	{ "8",		AVRCP_POP_8,		KEY_8 },
	{ "9",		AVRCP_POP_9,		KEY_9 },
	{ NULL, 0, 0 }
};

/**
 * @return >0 success, <=0 fail
 */
static int myinput_create(const char *name)
{
	struct uinput_user_dev dev;
	int fd, err, i;

	err = 0;
	DBG_PRINTF("[BT][AVRCP] open input device: %s\n", name);
	fd = open("/dev/uinput", O_RDWR);
	DBG_PRINTF("[BT][AVRCP] open input device return: %d\n", fd);
	if (fd < 0) {
		DBG_PRINTF("[BT][AVRCP] try to open uinput\n");
		fd = open("/dev/input/uinput", O_RDWR);
		if (fd < 0) {
			DBG_PRINTF("[BT][AVRCP] try to open misc-uinput\n");
			fd = open("/dev/misc/uinput", O_RDWR);
			if (fd < 0) {
				DBG_PRINTF("[BT][AVRCP] try to open event0\n");
				fd = open("/devices/virtual/input/input0", O_RDWR );
				if (fd < 0) {
					err = fd ;//errno;
					DBG_PRINTF("[BT][AVRCP] cannot open input device:\n");
					return -1;
				}else{
					
				}
			}else{
			}
			DBG_PRINTF("[BT][AVRCP] open /dev/misc/uinput device return: %d\n", fd);
		}else{
			DBG_PRINTF("[BT][AVRCP] open /dev/input/uinput device return: %d\n", fd);
		}
	}else{
		DBG_PRINTF("[BT][AVRCP] open /dev/uinput device return: %d\n", fd);
	}

	memset(&dev, 0, sizeof(dev));
	if (name)
		strncpy(dev.name, name, UINPUT_MAX_NAME_SIZE - 1);

	dev.id.bustype = BUS_BLUETOOTH;
	dev.id.vendor  = 0x0000;
	dev.id.product = 0x0000;
	dev.id.version = 0x0000;

	if (write(fd, &dev, sizeof(dev)) < 0) {
		//err = errno;
		//error("Can't write device information: %s (%d)",strerror(err), err);
		close(fd);
		//errno = err;
		DBG_PRINTF("[BT][AVRCP] fail to write dev\n");
		return -err;
	}

	DBG_PRINTF("[BT][AVRCP] before ioctl\n");
	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_EVBIT, EV_REL);
	ioctl(fd, UI_SET_EVBIT, EV_REP);
	ioctl(fd, UI_SET_EVBIT, EV_SYN);
	DBG_PRINTF("[BT][AVRCP] after ioctl\n");

#if 1
	for (i = 0; key_map[i].name != NULL; i++)
		ioctl(fd, UI_SET_KEYBIT, key_map[i].keycode);
#else
	DBG_PRINTF("[BT][AVRCP] Open all ioctl\n");
	for (i = 0; i < 255; i++)
		ioctl(fd, UI_SET_KEYBIT, i);
#endif

	DBG_PRINTF("[BT][AVRCP] before create\n");
	if (ioctl(fd, UI_DEV_CREATE, NULL) < 0) {
		close(fd);
		DBG_PRINTF("[BT][AVRCP] fail to create\n");
		return -err;
	}

	return fd;
}

int avrcp_input_init(const char *name){
	if( g_uinput_fd > 0){
		ALOGI("[BT][AVRCP] Input already create fd:%d\n", g_uinput_fd);
		return g_uinput_fd;
	}

	g_uinput_fd = myinput_create(name);
	if( g_uinput_fd > 0){
		DBG_PRINTF("[BT][AVRCP] Input create ok fd:%d\n", g_uinput_fd);
		ALOGI("[BT][AVRCP] Input create ok fd:%d\n", g_uinput_fd);
	}else{
		DBG_PRINTF("[BT][AVRCP] Input create fail fd:%d\n", g_uinput_fd);		
		ALOGE("[BT][AVRCP] Input create fail fd:%d\n", g_uinput_fd);
		ALOGE("[BT][AVRCP][ERR] failed : %s, %d", strerror(errno), errno);
		g_uinput_fd = -1;
	}
	return g_uinput_fd;
}

int avrcp_input_deinit(){
	if( g_uinput_fd > 0 ){
		ALOGI("[BT][AVRCP] UI_DEV_DESTROY");
		ioctl(g_uinput_fd, UI_DEV_DESTROY);
		close(g_uinput_fd);
		g_uinput_fd = -1;
		return 1;
	}
	return 0;
}


int avrcp_is_ready(){
	if( g_uinput_fd >0  ){
		return 1; // success for avrcp_input_sendkey
	}
	return 0; // fail
}

/**
* @brief pass avrcpKey to send to uinput device
* Convert avrcpKey to keycode (Hardware key)
* @param key the keycode of AVRCP key
* @param pressed 1 pressed. 0 release.
*/
void avrcp_input_sendkey(uint16_t avrcpkey, int pressed){
	uint16_t keycode = 0;
	if( g_uinput_fd > 0 ){
		for(int i = 0 ; i < 255 && i < (int)( sizeof(key_map)/sizeof(key_map_struct) ); i++){
			if( key_map[i].avrcp == avrcpkey ){
				keycode = key_map[i].keycode;
				break;
			}
		}
		if( keycode > 0 ){
			send_key(g_uinput_fd, keycode, pressed==1?1:0);   // send key pad 9
			ALOGI("[BT][AVRCP] send avrcpkey:%d keycord:%d pressed:%d", avrcpkey, keycode, pressed);	
		}else{
			ALOGI("[BT][AVRCP][WRN] avrcp_input_sendkey:Cannot find keycode");
		}
	}else{
		ALOGI("[BT][AVRCP][WRN] avrcp_input_sendkey:Cannot send key event");
	}
}


#ifdef TEST_MYINPUT
/**
 * @brief avrcp sendkey tools
 * Use this to send a raw key code.
 * the rawkey keymap is /system/usr/keylayout/AVRCP.kl
 */
int main(int argc, char* argv[])
{
	uint16_t keycode = KEY_FASTFORWARD;
	uint16_t delay = 1;
	
	printf("[BT][AVRCP] Usage: avrcp_sendkey rawkey_code release_delay \n");
	
	DBG_PRINTF("[BT][AVRCP] prepare to create\n");
	
	g_uinput_fd = myinput_create("AVRCP"); // will use /system/usr/keylayout/AVRCP.kl key map
	
	if( argc > 1  ){
		keycode = atoi(argv[1]);
	}
	if( argc > 2  ){
		delay = atoi(argv[2]);
	}
	printf("[BT][AVRCP] keycode:%d(0x%x) delay:%d\n", keycode, keycode, delay );
	DBG_PRINTF("[BT][AVRCP] keycode:%d(0x%x) delay:%d", keycode, keycode, delay );
	
	if( g_uinput_fd > 0){
		DBG_PRINTF("[BT][AVRCP] create ok fd:%d\n", g_uinput_fd);
	
		printf("[BT][AVRCP] send key before\n");
		send_key(g_uinput_fd, keycode, 1);   // send key pad 9
		DBG_PRINTF("[BT][AVRCP] send key after\n");
		sleep(delay); // seconds
		send_key(g_uinput_fd, keycode, 0); // send key pad 9
		printf("[BT][AVRCP] send key after\n");
	
	}
	
	if( g_uinput_fd > 0 ){
		close(g_uinput_fd);
		g_uinput_fd = 0;
	}
	
	return 0;
}
#endif
