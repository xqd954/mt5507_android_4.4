
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


#include "assert.h"
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>


//#include "bt_drv.h"
typedef unsigned char U8;
typedef unsigned short U16;

//#include "system/bluetooth/bluez-clean-headers/hci.h"
#define HCISETRAW    _IOW('H', 220, int)
static int commPort =-1 ;
#define MOD_BT 0

//static pthread_t monitorThread;
//static int monitoring = 0;


#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif 


#define Assert(exp)

#include <cutils/log.h>

#define bt_prompt_trace(mod, fmt, ...)    ALOGD("%s : " fmt, __FUNCTION__, ## __VA_ARGS__)

#define LOG_ERR(f, ...)       ALOGE("%s: " f, __FUNCTION__, ##__VA_ARGS__)
#define LOG_WAN(f, ...)       ALOGW("%s: " f, __FUNCTION__, ##__VA_ARGS__)

#define LOG_DBG(f, ...)       ALOGD("%s: " f,  __FUNCTION__, ##__VA_ARGS__)

typedef void (*UartCallback) (unsigned char event);

extern unsigned char BTCoreVerifySysInitState(void);

typedef struct {
    UartCallback cb_func;
}bt_adp_uart;

bt_adp_uart BTAdpUartCtx;

/* HCI dev events */
#define HCI_DEV_REG	1
#define HCI_DEV_UNREG	2
#define HCI_DEV_UP	3
#define HCI_DEV_DOWN	4
#define HCI_DEV_SUSPEND	5
#define HCI_DEV_RESUME	6

#define HCI_COMMAND_PKT		0x01
#define HCI_ACLDATA_PKT		0x02
#define HCI_SCODATA_PKT		0x03
#define HCI_EVENT_PKT		0x04
#define HCI_VENDOR_PKT		0xff
#define HCI_MAX_ACL_SIZE	1024
#define HCI_MAX_SCO_SIZE	255
#define HCI_MAX_EVENT_SIZE	260
#define HCI_MAX_FRAME_SIZE	(HCI_MAX_ACL_SIZE + 4)

#define SOL_HCI		0
#define HCI_FILTER	2
#define HCI_DEV_NONE	0xffff

#define HCI_MAX_DEV	1
#define BTPROTO_HCI	1

#define HCI_FLT_TYPE_BITS	31
#define HCI_FLT_EVENT_BITS	63

#define HCIDEVUP	_IOW('H', 201, int)
#define HCIDEVDOWN	_IOW('H', 202, int)
#define HCIDEVRESET	_IOW('H', 203, int)
#define HCIDEVRESTAT	_IOW('H', 204, int)
#define HCIGETDEVLIST	_IOR('H', 210, int)

#define EVT_INQUIRY_COMPLETE		0x01
#define EVT_INQUIRY_RESULT		0x02
#define EVT_CONN_COMPLETE		0x03
#define EVT_CONN_REQUEST		0x04
#define EVT_DISCONN_COMPLETE		0x05
#define EVT_CMD_STATUS 			0x0F
#define EVT_AUTH_COMPLETE		0x06
#define EVT_REMOTE_NAME_REQ_COMPLETE	0x07
#define EVT_READ_REMOTE_FEATURES_COMPLETE	0x0B
#define EVT_READ_REMOTE_VERSION_COMPLETE	0x0C
#define EVT_CMD_COMPLETE 		0x0E
#define EVT_RETURN_LINK_KEYS		0x15
#define EVT_PIN_CODE_REQ		0x16
#define EVT_LINK_KEY_REQ		0x17
#define EVT_LINK_KEY_NOTIFY		0x18
#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
#define EVT_READ_REMOTE_EXT_FEATURES_COMPLETE	0x23
#define EVT_EXTENDED_INQUIRY_RESULT	0x2F
#define EVT_IO_CAPABILITY_REQUEST	0x31
#define EVT_IO_CAPABILITY_RESPONSE	0x32
#define EVT_USER_CONFIRM_REQUEST	0x33
#define EVT_USER_PASSKEY_REQUEST	0x34
#define EVT_REMOTE_OOB_DATA_REQUEST	0x35
#define EVT_SIMPLE_PAIRING_COMPLETE	0x36
#define EVT_USER_PASSKEY_NOTIFY		0x3B
#define EVT_KEYPRESS_NOTIFY		0x3C
#define EVT_REMOTE_HOST_FEATURES_NOTIFY	0x3D
#define EVT_LE_META_EVENT	0x3E/* HCI device flags */

#define EVT_ENCRYPT_CHANGE  0x08
#define EVT_HARDWARE_ERROR  0x10
#define EVT_FLUSH_OCCURRED  0x11
#define EVT_ROLE_CHANGE     0x12
#define EVT_NUM_COMP_PKTS   0x13
#define EVT_MODE_CHANGE     0x14
#define EVT_SYNC_CONN_COMPLETE  0x2C
#define EVT_SYNC_CONN_CHANGED   0x2D
#define EVT_LINK_SUPERVISION_TIMEOUT_CHANGED  0x38

enum {
	HCI_UP,
	HCI_INIT,
	HCI_RUNNING,

	HCI_PSCAN,
	HCI_ISCAN,
	HCI_AUTH,
	HCI_ENCRYPT,
	HCI_INQUIRY,

	HCI_RAW,

	HCI_SETUP,
	HCI_AUTO_OFF,
	HCI_MGMT,
	HCI_PAIRABLE,
	HCI_SERVICE_CACHE,
	HCI_LINK_KEYS,
	HCI_DEBUG_KEYS,

	HCI_RESET,
};

U8 sock_rx_buf[HCI_MAX_FRAME_SIZE];
U16 sock_rx_buf_start = 0, sock_rx_buf_end = 0;

#define DBG(fmt, arg...)  bt_prompt_trace(MOD_BT, "DEBUG: %s: " fmt "\n" , __FUNCTION__ , ## arg)

static struct dev_info {
	int id;
	int sk;

	int already_up;

} devs;

struct hci_filter {
	uint32_t type_mask;
	uint32_t event_mask[2];
	uint16_t opcode;
};

struct sockaddr_hci {
	sa_family_t	hci_family;
	unsigned short	hci_dev;
	unsigned short  hci_channel;
};

struct hci_dev_req {
	uint16_t dev_id;
	uint32_t dev_opt;
};

struct hci_dev_list_req {
	uint16_t dev_num;
	struct hci_dev_req dev_req[0];	/* hci_dev_req structures */
};
struct hci_dev_stats {
	__u32 err_rx;
	__u32 err_tx;
	__u32 cmd_tx;
	__u32 evt_rx;
	__u32 acl_tx;
	__u32 acl_rx;
	__u32 sco_tx;
	__u32 sco_rx;
	__u32 byte_rx;
	__u32 byte_tx;
};

/* BD Address */
typedef struct {
	__u8 b[6];
} __packed bdaddr_t;

struct hci_dev_info {
	__u16 dev_id;
	char  name[8];

	bdaddr_t bdaddr;

	__u32 flags;
	__u8  type;

	__u8  features[8];

	__u32 pkt_type;
	__u32 link_policy;
	__u32 link_mode;

	__u16 acl_mtu;
	__u16 acl_pkts;
	__u16 sco_mtu;
	__u16 sco_pkts;

	struct hci_dev_stats stat;
};


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

static inline void hci_filter_all_events(struct hci_filter *f)
{
	memset((void *) f->event_mask, 0xff, sizeof(f->event_mask));
}

static inline void hci_filter_set_event(int e, struct hci_filter *f)
{
	hci_set_bit((e & HCI_FLT_EVENT_BITS), &f->event_mask);
}

static struct dev_info *init_dev_info(int index, int sk, int already_up)
{
	struct dev_info *dev = &devs;

	memset(dev, 0, sizeof(*dev));

	dev->id = index;
	dev->sk = sk;
    commPort = sk;
	bt_prompt_trace(MOD_BT, "init_dev_info: commPort = %d", commPort);
	dev->already_up = already_up;
	return dev;
}


static void start_hci_dev(int index)
{
    struct dev_info *dev = &devs;
    struct hci_filter flt;

    bt_prompt_trace(MOD_BT, "Listening for HCI events on hci%d", index);

    /* Set filter */
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_set_ptype(HCI_ACLDATA_PKT, &flt);
    hci_filter_set_ptype(HCI_SCODATA_PKT, &flt);
   // hci_filter_all_events(&flt);
   // /*
    hci_filter_set_event(EVT_CMD_STATUS, &flt);
    hci_filter_set_event(EVT_CMD_COMPLETE, &flt);
    hci_filter_set_event(EVT_PIN_CODE_REQ, &flt);
    hci_filter_set_event(EVT_LINK_KEY_REQ, &flt);
    hci_filter_set_event(EVT_LINK_KEY_NOTIFY, &flt);
    hci_filter_set_event(EVT_RETURN_LINK_KEYS, &flt);
    hci_filter_set_event(EVT_IO_CAPABILITY_REQUEST, &flt);
    hci_filter_set_event(EVT_IO_CAPABILITY_RESPONSE, &flt);
    hci_filter_set_event(EVT_USER_CONFIRM_REQUEST, &flt);
    hci_filter_set_event(EVT_USER_PASSKEY_REQUEST, &flt);
    hci_filter_set_event(EVT_REMOTE_OOB_DATA_REQUEST, &flt);
    hci_filter_set_event(EVT_USER_PASSKEY_NOTIFY, &flt);
    hci_filter_set_event(EVT_KEYPRESS_NOTIFY, &flt);
    hci_filter_set_event(EVT_SIMPLE_PAIRING_COMPLETE, &flt);
    hci_filter_set_event(EVT_AUTH_COMPLETE, &flt);
    hci_filter_set_event(EVT_REMOTE_NAME_REQ_COMPLETE, &flt);
    hci_filter_set_event(EVT_READ_REMOTE_VERSION_COMPLETE, &flt);
    hci_filter_set_event(EVT_READ_REMOTE_FEATURES_COMPLETE, &flt);
    hci_filter_set_event(EVT_REMOTE_HOST_FEATURES_NOTIFY, &flt);
    hci_filter_set_event(EVT_INQUIRY_COMPLETE, &flt);
    hci_filter_set_event(EVT_INQUIRY_RESULT, &flt);
    hci_filter_set_event(EVT_INQUIRY_RESULT_WITH_RSSI, &flt);
    hci_filter_set_event(EVT_EXTENDED_INQUIRY_RESULT, &flt);
    hci_filter_set_event(EVT_READ_REMOTE_EXT_FEATURES_COMPLETE, &flt);
    hci_filter_set_event(EVT_CONN_REQUEST, &flt);
    hci_filter_set_event(EVT_CONN_COMPLETE, &flt);
    hci_filter_set_event(EVT_DISCONN_COMPLETE, &flt);
    hci_filter_set_event(EVT_ENCRYPT_CHANGE, &flt);
    hci_filter_set_event(EVT_HARDWARE_ERROR, &flt);
    hci_filter_set_event(EVT_FLUSH_OCCURRED, &flt);
    hci_filter_set_event(EVT_ROLE_CHANGE, &flt);
    hci_filter_set_event(EVT_NUM_COMP_PKTS, &flt);
    hci_filter_set_event(EVT_MODE_CHANGE, &flt);
    hci_filter_set_event(EVT_SYNC_CONN_COMPLETE, &flt);
    hci_filter_set_event(EVT_SYNC_CONN_CHANGED, &flt);
    hci_filter_set_event(EVT_LINK_SUPERVISION_TIMEOUT_CHANGED, &flt);
    hci_filter_set_event(EVT_LE_META_EVENT, &flt);
  //  */
 
    if (setsockopt(dev->sk, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        bt_prompt_trace(MOD_BT, "Can't set filter on hci%d: %s (%d)",
        				index, strerror(errno), errno);
        return;
    }

    if (fcntl(dev->sk, F_SETFL, fcntl(dev->sk, F_GETFL, 0) | O_NONBLOCK) < 0) {
        bt_prompt_trace(MOD_BT, "Can't set non blocking mode: %s (%d)\n",
        				strerror(errno), errno);
        return;
    }
}

static void stop_hci_dev(int index)
{
	struct dev_info *dev = &devs;

	if (dev->sk < 0)
		return;

	bt_prompt_trace(MOD_BT, "Stopping hci%d event socket %d", index, dev->sk);

	hci_close_dev(dev->sk);
	init_dev_info(index, -1, dev->already_up);
}


/* Open HCI device.
 * Returns device descriptor (dd). */
int hci_open_dev(int dev_id)
{
	struct sockaddr_hci a;
	int dd, err;

    DBG("");
	/* Create HCI socket */
	dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (dd < 0)
		return dd;

    DBG("sock=%x",dd);
	/* Bind socket to the HCI device */
	memset(&a, 0, sizeof(a));
	a.hci_family = AF_BLUETOOTH;
	a.hci_dev = dev_id;
	if (bind(dd, (struct sockaddr *) &a, sizeof(a)) < 0)
		goto failed;
//lawrance
    if (ioctl(dd, HCISETRAW, 1) < 0) 
    {
		bt_prompt_trace(MOD_BT, "Can't set HCI RAW: %s (%d)", strerror(errno), errno);
        goto failed;
    }

	return dd;

failed:
	err = errno;
	close(dd);
	errno = err;

	return -1;
}

int hci_close_dev(int dd)
{
	DBG("hci_close_dev %d", dd);
	return close(dd);
}


static struct dev_info *init_device(int index, int already_up)
{
	struct dev_info *dev;
	struct hci_dev_req dr;
	struct hci_dev_info di;
	int dd, opt;

	DBG("init_device hci=%d, already_up=%d", index, already_up);

	dd = hci_open_dev(index);
	if (dd < 0) {
		bt_prompt_trace(MOD_BT, "Unable to open hci%d: %s (%d)", index,
						strerror(errno), errno);
		return NULL;
	}
	


	dev = init_dev_info(index, dd, already_up);
	start_hci_dev(index);

	/* Avoid forking if nothing else has to be done */
	if (already_up)
		return dev;

	/* Do initialization in the separate process */
	memset(&dr, 0, sizeof(dr));
	dr.dev_id = index;

	/* Start HCI device */

    #if 1
    	while (ioctl(dd, HCIDEVUP, index) < 0 && errno != EALREADY) {
		bt_prompt_trace(MOD_BT, "Can't init device hci%d: %s (%d)",
					index, strerror(errno), errno);
              sleep(1);
	}
    #else
	if (ioctl(dd, HCIDEVUP, index) < 0 && errno != EALREADY) {
		bt_prompt_trace(MOD_BT, "Can't init device hci%d: %s (%d)",
					index, strerror(errno), errno);
		goto fail;
	}
    #endif
    return dev;

fail:
	hci_close_dev(dd);
    return NULL;
}


static void device_event(int event, int index)
{
	DBG("event %d, index %d", event, index);

	switch (event) {
	case HCI_DEV_REG:
		bt_prompt_trace(MOD_BT, "HCI dev %d registered", index);
		init_device(index, 0);
		break;

	case HCI_DEV_UNREG:
		bt_prompt_trace(MOD_BT, "HCI dev %d unregistered", index);
		stop_hci_dev(index);
		break;

	case HCI_DEV_UP:
		bt_prompt_trace(MOD_BT, "HCI dev %d up", index);
		break;

	case HCI_DEV_DOWN:
		bt_prompt_trace(MOD_BT, "HCI dev %d down", index);
		break;
	}
}


static int init_known_adapters(int ctl)
{
    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    int i, err;
    size_t req_size;

    DBG("");

    req_size = HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t);

    dl = malloc(req_size);
    if (!dl) {
        bt_prompt_trace(MOD_BT, "Can't allocate devlist buffer");
        return -1;
    }

    dr = dl->dev_req;

    while (1) {
        dl->dev_num = HCI_MAX_DEV;
        if (ioctl(ctl, HCIGETDEVLIST, dl) < 0) {
            err = -errno;
            bt_prompt_trace(MOD_BT, "Can't get device list: %s (%d)", strerror(-err), -err);
            free(dl);
            return -1;
        }

        if (dl->dev_num == 0) {
            bt_prompt_trace(MOD_BT, "No BT adapter. Wait....");
            usleep(300000);
        } else {
            break;
        }
    }

    bt_prompt_trace(MOD_BT, "dl->dev_num = %d", dl->dev_num);
    
    for (i = 0; i < dl->dev_num; i++, dr++) {
        struct dev_info *dev;
        int already_up;

        already_up = hci_test_bit(HCI_UP, &dr->dev_opt);
        DBG("already_up=%d", already_up);

        dev = init_device(dr->dev_id, already_up);
        if (dev == NULL)
            continue;

        if (!dev->already_up)
            continue;

        //device_event(HCI_DEV_UP, dr->dev_id);
        bt_prompt_trace(MOD_BT, "HCI dev %d up", dr->dev_id);
    }
    free(dl);
    return 0;
}

int mtk_hciops_setup(void)
{
    struct sockaddr_hci addr;
    struct hci_filter flt;
    int sock, err;
    int count =10, ret=0;
    DBG("");
	
    while (count-- >0) {
        /* Create and bind HCI socket */
        sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (sock < 0) {
            err = -errno;
            bt_prompt_trace(MOD_BT, "Can't open HCI socket: %s (%d)", strerror(-err), -err);
            return err;
        }
        DBG("sock=%x",sock);
        /* Set filter */
        ret= init_known_adapters(sock);
        hci_close_dev(sock);
        if (!ret)
            break;    
        usleep(300000);                
    }
    return 0;
}

void mtk_hciops_teardown(void)
{
    stop_hci_dev(0);
}

U16 hci_sock_read(int fd, U8 *buffer, U16 length)
{
    int readByte = 0;
    int ret;
    if (fd < 0 || buffer == NULL || length == 0)
    {
        return 0;
    }

    if (sock_rx_buf_start == sock_rx_buf_end)
    {
        Assert(sock_rx_buf_start == 0);
        ret = read(fd, sock_rx_buf, HCI_MAX_FRAME_SIZE);
        if (ret < 0) {
            if (errno != EAGAIN)
            {
                bt_prompt_trace(MOD_BT, "[UART][ERR] UART_Read : %d", ret);
            }
            goto error;
        }
        else
        {
            sock_rx_buf_end = ret;
            //DisplayHex("SOCK read", sock_rx_buf, ret);
        }
    }

    //Assert(sock_rx_buf_end > sock_rx_buf_start);
    readByte = min(length, sock_rx_buf_end - sock_rx_buf_start);
    memcpy(buffer, &sock_rx_buf[sock_rx_buf_start], readByte);
    sock_rx_buf_start += readByte;
    if (sock_rx_buf_start == sock_rx_buf_end)
    {
        sock_rx_buf_start = 0;
        sock_rx_buf_end = 0;
    }

error:
    return readByte;
}

int BT_WritePacket(unsigned char *pkt, int len);
int BT_ReadPacket(unsigned char *pkt, int len);

static void dumpBinary(const unsigned char *raw, int len){
    char buf[2048];
    char *ptr = buf;
    while(len){
        sprintf(ptr, "%02X:", *raw);
        len--;
        raw++;
        ptr+=3;
    }
    *ptr = 0;
   LOG_DBG("DumpString : %s\n", buf);
}

void enableMt76xx()
{
    unsigned char evt[256];
    unsigned char power_on[] = 
                        {0x01, /* CMD */
                        0xC9, 0xFC, /* opcode */
                        0x02, /* cmd length */
                        0x01, 0x01};
    if(BT_WritePacket(power_on, sizeof(power_on)) < 0){
        LOG_ERR("Write power on pkt failed");
    }else{
        int nRead;
        int retry = 10;
        while(retry--){
            nRead = BT_ReadPacket(evt, sizeof(evt));
            if(nRead < 0){
                LOG_ERR("Read failed. Sleep 10 ms and retry: %d", retry);
                usleep(30000);
            }else{
                dumpBinary(evt, nRead);
                if(evt[0] == 4 && evt[1]==0xE6){
                    break;
                }                    
            }
        }
        if(nRead > 0){
            LOG_DBG("Successfully get 0xE6 event");
        }
    }
}

int mtk_bt_enable(int arg, void *func_cb)
{
   if (mtk_hciops_setup() < 0) {
	bt_prompt_trace(MOD_BT, "adapter_ops_setup failed");
        return -1;
    } else {
        BTAdpUartCtx.cb_func = func_cb;
        enableMt76xx();
        return commPort;
    }
}

void disableMt76xx()    /* Deinit 76xx chip */
{
    unsigned char evt[1024];
    {
        unsigned char reset_cmd[] = 
                            {0x01, /* ACL */
                            0x3, 0xC, /* opcode */
                            0x0, /* cmd length */};
        if(BT_WritePacket(reset_cmd, sizeof(reset_cmd)) < 0){
            LOG_ERR("Write reset command failed");
        }else{
            int nRead = 0, retry = 10;
            while(retry--){
                nRead = BT_ReadPacket(evt, sizeof(evt));
                if(nRead < 0){
                    LOG_ERR("Read failed. Sleep 10 ms and retry: %d", retry);
                    usleep(20000);
                }else{
                    dumpBinary(evt, nRead);
                    if(evt[0] == 4 && evt[1] == 0xE){
                        break;
                    }                    
                }
            }
        }
    }        
    {
        unsigned char power_off[] = 
                            {0x01, /* ACL */
                            0xC9, 0xFC, /* opcode */
                            0x02, /* cmd length */
                            0x01, 0x00 /* State is power off */};
        if(BT_WritePacket(power_off, sizeof(power_off)) < 0){
            LOG_ERR("Write power on pkt failed");
        }else{
            int nRead = 0, retry = 10;
            while(retry--){
                nRead = BT_ReadPacket(evt, sizeof(evt));
                if(nRead < 0){
                    LOG_ERR("Read failed. Sleep 10 ms and retry: %d", retry);
                    usleep(10000);
                }else{
                    dumpBinary(evt, nRead);
                    if(evt[0] == 4 && evt[1] == 0xE6){
                        break;
                    }                    
                }
            }
        }
    }
}

/* Init 76xx chip */
int mtk_bt_disable(int bt_fd)
{
    if(commPort >= 0)
    {
        disableMt76xx();
        mtk_hciops_teardown();
        commPort = -1;
        BTAdpUartCtx.cb_func = NULL;
    }
    return 0x00;
}
    
int mtk_bt_write(int bt_fd, const unsigned char *buffer, unsigned short length)
{
    U16 bytesWritten = 0;
    struct iovec iv[3];
    int ivn;
    int ret;

    iv[0].iov_base = buffer;
    iv[0].iov_len  = length;
    ivn = 1;

    bt_prompt_trace(MOD_BT, "UART_Write : commPort=%d, len=%d", commPort, length);
    if ((ret = writev(commPort, iv, ivn)) < 0) {  //commPort = dd Device descriptor returned by hci_open_dev
        bt_prompt_trace(MOD_BT, "UART_Write error(%d): %s", errno, strerror(errno));
        return -1;
    } else {
        bytesWritten = (U16)ret;
    }
    return bytesWritten;
}
 int mtk_bt_read(int bt_fd, unsigned char *buffer, unsigned short length)
{
    U16 readByte = 0;
    if(commPort >= 0) {
        readByte = hci_sock_read(commPort, buffer, length);
    }
    return readByte;
}

//void mtk_bt_op(BT_REQ req, BT_RESULT *result)
void mtk_bt_op()
{

}

#define HCI_TYPE_COMMAND   1
#define HCI_TYPE_ACL_DATA  2
#define HCI_TYPE_SCO_DATA  3
#define HCI_TYPE_EVENT     4

int BT_WritePacket(unsigned char *pkt, int len){
    int nWrite;
    nWrite = mtk_bt_write(0, pkt, len);
    return nWrite;
}

int BT_ReadPacket(unsigned char *pkt, int len){
    int state, nRead, size;
    unsigned char type;
    unsigned int pktLen = 0;
    int total_len = 0;
    int i;
    
    /* Read type */
    unsigned char *ptr = pkt;
    nRead = mtk_bt_read(0, ptr, 1);
    LOG_DBG("Read Type=%u", *ptr);
    if(nRead != 1 || *ptr > 4 || *ptr == 0){ 
        LOG_ERR("Read Type failed");
        return -1;
    }
    type = *ptr;
    LOG_DBG("type=%u", type);
    ptr += nRead;
    total_len += nRead;

    /* Read handle or evend code */
    size = (type == HCI_TYPE_ACL_DATA) ? 2 : 1;
    nRead = mtk_bt_read(0, ptr, size);
    if(nRead != size){
        LOG_ERR("Read length failed");
        return -1;
    }
    ptr += nRead;
    total_len += nRead;

    /* Read length */
    size = (type == HCI_TYPE_ACL_DATA) ? 2 : 1;
    nRead = mtk_bt_read(0, ptr, size);
    if(nRead != size){
        LOG_ERR("Read length failed");
        return -1;
    }
    for(i = nRead;i > 0;i--){
        pktLen <<= 8;
        pktLen += ptr[i-1];
    }
    LOG_DBG("pktLen=%u", pktLen);    
    ptr += nRead;
    total_len += nRead;

    /* Read packet */
    nRead = mtk_bt_read(0, ptr, pktLen);
    if(nRead != pktLen){
        LOG_ERR("Read packet failed");
        return -1;
    }
    total_len += nRead;
    
    LOG_DBG("BT_ReadPacket : return total len 1 = %u", total_len);
    return total_len;
}
/**********************************************
*   Monitoring BT device event
**********************************************/
#if 0
void* ListenStackEvent(void *lpParam) 
{
    fd_set readfs;
    int res = -1;    
    int sock = hci_open_dev(HCI_DEV_NONE);
    if(sock >= 0){
        
    }else{
        bt_prompt_trace_lv3(MOD_BT,"[resume] pthread_create failed");
    }
    return NULL;
}

static void StartListen(void)
{
    if(monitoring){
        if( 0 != pthread_create( &monitorThread, NULL, ListenResume, NULL) ){
            bt_prompt_trace_lv3(MOD_BT,"[resume] pthread_create failed");
        }else{
            monitoring = 1;
            bt_prompt_trace_lv3(MOD_BT,"[resume] pthread_create success ");
        }
    }
}
#endif
