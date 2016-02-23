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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include "bt_drv.h"
#include "os_dep.h"
#include "usb.h"
#include "upio.h"
#include <assert.h>


/* For BT_HOT_OP_SET_FWASSERT */
#define COMBO_IOC_MAGIC              0xb0
#define COMBO_IOCTL_FW_ASSERT        _IOWR(COMBO_IOC_MAGIC, 0, void*)

/* For BT_AUDIO_OP_GET_CONFIG */
struct audio_t {
  int chip_id;
  AUDIO_CONFIG audio_conf;
};

struct uart_t {
  char *type;
  int  m_id;
  int  p_id;
  int  proto;
  int  init_speed;
  int  speed;
  int  flags;
  int  pm;
  char *bdaddr;
  int  (*init) (int fd, struct uart_t *u, struct termios *ti);
  int  (*post) (int fd, struct uart_t *u, struct termios *ti);
};

//===============        Global Variables         =======================

// mtk bt library
static void *glib_handle = NULL;
typedef int (*INIT)(int fd, struct uart_t *u, struct termios *ti);
typedef int (*UNINIT)(int fd);
typedef int (*WRITE)(int fd, unsigned char *buffer, unsigned long len);
typedef int (*READ)(int fd, unsigned char *buffer, unsigned long len);
typedef int (*NVRAM)(unsigned char *ucNvRamData);
typedef int (*GETID)(int *pChipId);


INIT    mtk = NULL;
UNINIT  bt_restore = NULL;
WRITE   write_comm_port = NULL;
READ    read_comm_port = NULL;
WRITE   bt_send_data = NULL;
READ    bt_receive_data = NULL;
NVRAM   bt_read_nvram = NULL;
GETID   bt_get_combo_id = NULL;
#ifndef MTK_PLATFORM
#if 0
static void fwcfg_cb(bt_vendor_op_result_t result){
    LOG_DBG("fwcfg_cb : result=%d", result);
}

static void scocfg_cb(bt_vendor_op_result_t result){
    LOG_DBG("scocfg_cb : result=%d", result);
}

static void lpm_cb(bt_vendor_op_result_t result){
    LOG_DBG("lpm_cb : result=%d", result);
}

#define BUFFER_HDR_SIZE 12

static char* alloc(int size)
{
    char *ptr;
    LOG_DBG("alloc : size=%d", size);
    ptr = malloc(size+BUFFER_HDR_SIZE);
    return ptr+BUFFER_HDR_SIZE;
}

static int dealloc(char *p_buf)
{
    char *ptr = p_buf;
    LOG_DBG("dealloc : pBuf=%p", p_buf);
    ptr -= BUFFER_HDR_SIZE;
    free(ptr);
    return 0;
}

static uint8_t xmit_cb(uint16_t opcode, void *p_buf, tINT_CMD_CBACK p_cback)
{
    LOG_DBG("xmit_cb : opcode=%u, p_buf=%p", opcode, p_buf);
    p_cback(NULL);
    return 0;
}

static void epilog_cb(bt_vendor_op_result_t result)
{
    LOG_DBG("epilog_cb : result=%d", result);
}

bt_vendor_callbacks_t g_vendor_cb = 
{
    /** set to sizeof(bt_vendor_callbacks_t) */
    sizeof(bt_vendor_callbacks_t),

    /*
     * Callback and callout functions have implemented in HCI libray
     * (libbt-hci.so).
     */

    /* notifies caller result of firmware configuration request */
    fwcfg_cb,

    /* notifies caller result of sco configuration request */
    scocfg_cb,

    /* notifies caller result of lpm enable/disable */
    lpm_cb,

    /* buffer allocation request */
    alloc,

    /* buffer deallocation request */
    dealloc,

    /* hci command packet transmit request */
    xmit_cb,

    /* notifies caller completion of epilog process */
    epilog_cb,

};

bt_vendor_interface_t *g_vendor_if = NULL;
#endif

#if !defined (USB_RX_PIPE)
bt_hc_callbacks_t mtk_hc_cbacks = 
{
    /** set to sizeof(bt_hc_callbacks_t) */
    sizeof(bt_hc_callbacks_t),

    /* notifies caller result of preload request */
    NULL,

    /* notifies caller result of postload request */
    NULL,

    /* notifies caller result of lpm enable/disable */
    NULL,

    /* notifies hardware on host wake state */
    NULL,

    /* buffer allocation request */
    alloc,

    /* buffer deallocation request */
    dealloc,

    /* notifies stack data is available */
    NULL,

    /* notifies caller when a buffer is transmitted (or failed) */
    NULL,
};

bt_hc_callbacks_t *bt_hc_cbacks = &mtk_hc_cbacks;
#endif
#endif

/* Audio interface & codec information mapping */
struct audio_t audio_conf_map[] = {
  { 0x6620,    { PCM,              SYNC_8K,  SHORT_FRAME,  0 } },

#if defined(MTK_MERGE_INTERFACE_SUPPORT)
  { 0x6628,    { MERGE_INTERFACE,  SYNC_8K,  SHORT_FRAME,  0 } },
#else
  { 0x6628,    { PCM,              SYNC_8K,  SHORT_FRAME,  0 } },
#endif

  { 0x6572,    { CVSD_REMOVAL,     SYNC_8K,  SHORT_FRAME,  0 } },

  { 0x6582,    { CVSD_REMOVAL,     SYNC_8K,  SHORT_FRAME,  0 } },

#if defined(MTK_MERGE_INTERFACE_SUPPORT)
  { 0x6630,    { MERGE_INTERFACE,  SYNC_8K,  SHORT_FRAME,  0 } },
#else
  { 0x6630,    { PCM,              SYNC_8K,  SHORT_FRAME,  0 } },
#endif

  { 0,         { 0 } }
};

//===============        F U N C T I O N S      =======================

static void wait_whole_chip_reset_complete(int bt_fd)
{
    UCHAR temp;
    int   res;
    
    do {
        res = read(bt_fd, &temp, 1);
        if (res < 0){
            if (errno == 88)
                usleep(200000);
            else if (errno == 99)
                break;
            else if (errno != EINTR && errno != EAGAIN)
                break;
        }
        else{
            break; // impossible case
        }
	} while(1);
}

static int setUartBlockMode(int handle, int block) 
{
    int val;

    /* uart handle is opened as blocking, change it to non-blocking */
    if( (val = fcntl(handle, F_GETFL, 0)) < 0) 
    {
        LOG_ERR("[UART][ERR] get handle flag failed");
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
        val = fcntl(handle, F_SETFL, val);
    }
    LOG_DBG("setUartBlockMode(%d,%d)=%d", handle, block, val);
    return val;
}

#define MT76XX_SEPCIAL_CMD

#ifdef MT76XX_SEPCIAL_CMD

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
    if(nRead != (int)pktLen){
        LOG_ERR("Read packet failed");
        return -1;
    }
    total_len += nRead;
    
    LOG_DBG("BT_ReadPacket : return total len 1 = %u", total_len);
    return total_len;
}

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
                    usleep(10000);
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
#endif

static unsigned char s_rx_buffer[2048];
static int s_rx_idx;
static int s_rx_pkt_size;

static rx_callback g_rx_callback = NULL;

void mtk_rx_callback(void){
    if(g_rx_callback)
        g_rx_callback();
}

int mtk_bt_enable(int flag, rx_callback func_cb)
{
#ifdef MTK_PLATFORM
    const char *errstr;
    struct uart_t u;
    int bt_fd = -1;
    
    LOG_TRC();
    
    glib_handle = dlopen("libbluetooth_mtk.so", RTLD_LAZY);
    if (!glib_handle){
        LOG_ERR("%s\n", dlerror());
        goto error;
    }
    
    dlerror(); /* Clear any existing error */
    
    mtk = dlsym(glib_handle, "mtk");
    bt_restore = dlsym(glib_handle, "bt_restore");
    write_comm_port = dlsym(glib_handle, "write_comm_port");
    read_comm_port = dlsym(glib_handle, "read_comm_port");
    bt_send_data = dlsym(glib_handle, "bt_send_data");
    bt_receive_data = dlsym(glib_handle, "bt_receive_data");

    if ((errstr = dlerror()) != NULL){
        LOG_ERR("Can't find function symbols %s\n", errstr);
        goto error;
    }
    
    bt_fd = mtk(-1, &u, NULL);
    if (bt_fd < 0)
        goto error;

    LOG_DBG("BT is enabled success\n");
    
    return bt_fd;

error:
    if (glib_handle){
        dlclose(glib_handle);
        glib_handle = NULL;
    }
#else
    int ret = 0;
    int pwr_state;
    //int fd_array[CH_MAX];
    unsigned char local_bdaddr[6] = {0xFE, 0XCD, 0x82, 0x66, 0x30, 0x01};

    s_rx_idx = 0;
    s_rx_pkt_size = 0;
    g_rx_callback = func_cb;
    
    /* Calling vendor-specific part */
#if 0
    pwr_state = BT_VND_PWR_ON;

    glib_handle = dlopen("libbt-vendor.so", RTLD_LAZY);
    if (!glib_handle){
        LOG_ERR("%s\n", dlerror());
        goto error;
    }

    g_vendor_if = (bt_vendor_interface_t *) dlsym(glib_handle, "BLUETOOTH_VENDOR_LIB_INTERFACE");
    LOG_DBG("local_addr addr is %p", local_bdaddr);
    ret = g_vendor_if->init(&g_vendor_cb, local_bdaddr);
    LOG_DBG("init return %d", ret);
    ret = g_vendor_if->op(BT_VND_OP_POWER_CTRL, &pwr_state);
    LOG_DBG("BT_VND_OP_POWER_CTRL %d", ret);
#else
    upio_init();
    upio_set_bluetooth_power(UPIO_BT_POWER_ON);
#endif
    //utils_init();
    LOG_DBG("usb_init\n");
    ret = usb_init();
    LOG_DBG("usb_init return %d\n", ret);
    LOG_DBG("usb_open\n");
    ret = usb_open(0);
    LOG_DBG("usb_open return %d\n", ret);
    if(ret != 0){
        ret = usb_getRXfd();
        //ret = g_vendor_if->op(BT_VND_OP_USERIAL_OPEN, &fd_array);
        //LOG_DBG("BT_VND_OP_USERIAL_OPEN %d", ret);
        //LOG_DBG("userial is %d", fd_array[0]);
        //setUartBlockMode(fd_array[0], 0);
        #ifdef MT76XX_SEPCIAL_CMD
        enableMt76xx();
        #endif
    }else{
        ret = -1;
    }
    return ret;

#endif
error:
#if 0
    if (glib_handle){
        dlclose(glib_handle);
        glib_handle = NULL;
    }
#endif
    return -1;
}


int mtk_bt_disable(int bt_fd)
{
    int ret;
    LOG_TRC();

#if 0
    if (!glib_handle){
        LOG_ERR("mtk bt library is unloaded!\n");
        return -1;
    }
#endif

#ifdef MTK_PLATFORM
    bt_restore(bt_fd);
#else
    int pwr_state;
    #ifdef MT76XX_SEPCIAL_CMD
    disableMt76xx();
    #endif
    
    //userial_close();
    usb_close();
    //utils_cleanup();
#if 0
    pwr_state = BT_VND_PWR_OFF;
    LOG_DBG("init return %d", ret);
    ret = g_vendor_if->op(BT_VND_OP_POWER_CTRL, &pwr_state);
    LOG_DBG("BT_VND_OP_POWER_CTRL %d", ret);

    //ret = g_vendor_if->op(BT_VND_OP_USERIAL_CLOSE, &bt_fd);
    //if ((ret != 2) && (ret != 4))
    //{
    //    g_vendor_if->op(BT_VND_OP_USERIAL_CLOSE, NULL);
    //    return FALSE;
    //}
    g_vendor_if->cleanup();
        {
            close(bt_fd);
        }
#else
    upio_set_bluetooth_power(UPIO_BT_POWER_OFF);
    upio_cleanup();
#endif
    
#endif

#if 0
    dlclose(glib_handle);
    glib_handle = NULL;
#endif

    return 0;
}

int mtk_bt_write(int bt_fd, unsigned char *buffer, unsigned short length)
{
    int ret_val;
    unsigned short len= length;
    
    LOG_DBG("buffer %x, len %d\n", (int)buffer, len);
#if 0
    if (!glib_handle){
        LOG_ERR("mtk bt library is unloaded!\n");
        return -1;
    }
#endif
#ifdef MTK_PLATFORM
    ret_val = write_comm_port(bt_fd, buffer, len);
    LOG_ERR("write complete. ret=%d\n", ret_val);

    if (ret_val < 0 && (ret_val == -88)){
        // whole chip reset, wait it complete (errno 99)
        wait_whole_chip_reset_complete(bt_fd);
        ret_val = -99;
    }
#else
    LOG_DBG("usb_write : len=%hu\n", len);
    
    ret_val = usb_write(0, buffer, len);
    LOG_DBG("usb_write return : %d\n", ret_val);
    assert(len == ret_val);
#endif
    return ret_val;
}


int mtk_bt_read(int bt_fd, unsigned char *buffer, unsigned short length)
{
    int ret_val = 0;
    unsigned short len= length;

    
    LOG_DBG("buffer %x, len %d\n", (int)buffer, len);

#if 0
    if (!glib_handle){
        LOG_ERR("mtk bt library is unloaded!\n");
        return -1;
    }
#endif

#ifdef MTK_PLATFORM
    ret_val = read_comm_port(bt_fd, buffer, len);
    if (ret_val < 0 && (ret_val == -88)){
        // whole chip reset, wait it complete (errno 99)
        wait_whole_chip_reset_complete(bt_fd);
        ret_val = -99;
    }
#else
  //  LOG_DBG("usb_read test");
    if(len){
        LOG_DBG("usb_read(buf=%p, len=%d)\n", buffer, len);
        ret_val = usb_read(0, buffer, len);
        LOG_DBG("usb_read return %d", ret_val);
    }
    
#endif
    return ret_val;	
}

void mtk_bt_op(BT_REQ req, BT_RESULT *result)
{
#ifdef MTK_PLATFORM
    result->status = FALSE;
    
    switch(req.op)
    {
      case BT_COLD_OP_GET_ADDR:
      {
          const char *errstr;
          unsigned char nvram[sizeof(ap_nvram_btradio_mt6610_struct)];
          unsigned char ucDefaultAddr[6] = {0};
          int chipId;
          
          LOG_DBG("BT_COLD_OP_GET_ADDR\n");
          
          glib_handle = dlopen("libbluetooth_mtk.so", RTLD_LAZY);
          if (!glib_handle){
              LOG_ERR("%s\n", dlerror());
              return;
          }
          
          dlerror(); /* Clear any existing error */
          
          bt_read_nvram = dlsym(glib_handle, "bt_read_nvram");
          bt_get_combo_id = dlsym(glib_handle, "bt_get_combo_id");
          
          if ((errstr = dlerror()) != NULL){
              LOG_ERR("Can't find function symbols %s\n", errstr);
              dlclose(glib_handle);
              glib_handle = NULL;
              return;
          }
          
          if(bt_read_nvram(nvram) < 0){
              LOG_ERR("Read Nvram data fails\n");
              dlclose(glib_handle);
              glib_handle = NULL;
              return;
          }
          
          /* Get combo chip id */
          if(bt_get_combo_id(&chipId) < 0){
              LOG_ERR("Unknown combo chip id\n");
              dlclose(glib_handle);
              glib_handle = NULL;
              return;
          }
          
          if(chipId == 0x6620){
              memcpy(ucDefaultAddr, stBtDefault_6620.addr, 6);
          }
          
          if(chipId == 0x6628){
              memcpy(ucDefaultAddr, stBtDefault_6628.addr, 6);
          }
          
          if(chipId == 0x6572){
              memcpy(ucDefaultAddr, stBtDefault_6572.addr, 6);
          }
          
          if(chipId == 0x6582){
              memcpy(ucDefaultAddr, stBtDefault_6582.addr, 6);
          }
          
          if(chipId == 0x6630){
              memcpy(ucDefaultAddr, stBtDefault_6630.addr, 6);
          }
          
          result->status = TRUE;
          if (0 == memcmp(nvram, ucDefaultAddr, 6))
          {
              LOG_DBG("Nvram BD address default value\n");
              result->param.addr[0] = 0;  //default address
              memcpy(&result->param.addr[1], nvram, 6);
          }
          else {
              LOG_DBG("Nvram BD address has valid value\n");
              result->param.addr[0] = 1;  //valid address
              memcpy(&result->param.addr[1], nvram, 6);
          }
          
          dlclose(glib_handle);
          glib_handle = NULL;
          break;
      }
      case BT_HOT_OP_SET_FWASSERT:
          LOG_DBG("BT_HOT_OP_SET_FWASSERT\n");
          
          // req.param.assert.fd should be the fd returned by mtk_bt_enable
          if (req.param.assert.fd < 0){
              LOG_ERR("Invalid bt fd!\n");
              return;
          }
          
          if (ioctl(req.param.assert.fd, COMBO_IOCTL_FW_ASSERT, \
                  req.param.assert.reason) < 0)
          {
              LOG_ERR("Set COMBO FW ASSERT fails\n");
              return;
          }
          
          result->status = TRUE;
          break;
          
      case BT_AUDIO_OP_GET_CONFIG:
      {
          const char *errstr;
          int chipId, i;
          
          LOG_DBG("BT_AUDIO_OP_GET_CONFIG\n");
          
          glib_handle = dlopen("libbluetooth_mtk.so", RTLD_LAZY);
          if (!glib_handle){
              LOG_ERR("%s\n", dlerror());
              return;
          }
          
          dlerror(); /* Clear any existing error */
          
          bt_get_combo_id = dlsym(glib_handle, "bt_get_combo_id");
          
          if ((errstr = dlerror()) != NULL){
              LOG_ERR("Can't find function symbols %s\n", errstr);
              dlclose(glib_handle);
              glib_handle = NULL;
              return;
          }
          
          /* Get combo chip id */
          if(bt_get_combo_id(&chipId) < 0){
              LOG_ERR("Unknown combo chip id\n");
              dlclose(glib_handle);
              glib_handle = NULL;
              return;
          }
          
          dlclose(glib_handle);
          glib_handle = NULL;
          
          /* Return the specific audio config on current chip */
          for(i = 0; audio_conf_map[i].chip_id; i++){
              if(audio_conf_map[i].chip_id == chipId){
                  memcpy(&result->param.audio_conf, &audio_conf_map[i].audio_conf, 
                      sizeof(AUDIO_CONFIG));
                  result->status = TRUE;
                  return;
              }
          }
          
          result->status = FALSE;
          break;
      }
      default:
          LOG_DBG("Unknown operation %d\n", req.op);
          break;
    }
#endif
    return;
}
