/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *  Portions of file: Copyright (C) 2013, Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      usb.c
 *
 *  Description:   Contains open/read/write/close functions on usb
 *
 ******************************************************************************/

#define LOG_TAG "bt_usb"

#include <utils/Log.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
//#include "bt_hci_bdroid.h"
#include "usb.h"
#include "utils.h"
//#include "bt_vendor_lib.h"
#include <sys/prctl.h>
#include "libusb.h"

/******************************************************************************
**  Constants & Macros
******************************************************************************/
/* if defined, use pipe for RX instead of buffer queues  */


#define USB_ALLOC(s) malloc(s)
#define USB_FREE(s) free(s)

#ifndef USB_DBG
#define USB_DBG TRUE
#endif

#if (USB_DBG == TRUE)
#define USBDBG ALOGD
#define USBERR ALOGE
#else
#define USBDBG
#define USBERR
#endif

/*
 * Bit masks : To check the transfer status
 */
#define XMITTED                 1
#define RX_DEAD                 2
#define RX_FAILED               4
#define XMIT_FAILED             8

/*
 * Field index values
 */
#define EV_LEN_FIELD        1
#define BLK_LEN_LO          2
#define BLK_LEN_HI          3
#define SCO_LEN_FIELD       2

#define BT_CTRL_EP      0x0
#define BT_INT_EP       0x81
#define BT_BULK_IN      0x82
#define BT_BULK_OUT     0x02
#define BT_ISO_IN       0x83
#define BT_ISO_OUT      0x03


#define BT_HCI_MAX_FRAME_SIZE      1028
#define BT_HCI_MAX_EVENT_SIZE      256
#define BT_MAX_ISO_FRAMES   10

#define H4_TYPE_COMMAND         1
#define H4_TYPE_ACL_DATA        2
#define H4_TYPE_SCO_DATA        3
#define H4_TYPE_EVENT           4

/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_REQ                 32

/* Preamble length for HCI Commands:
**      2-bytes for opcode and 1 byte for length
*/
#define HCI_CMD_PREAMBLE_SIZE   3

/* Preamble length for HCI Events:
**      1-byte for opcode and 1 byte for length
*/
#define HCI_EVT_PREAMBLE_SIZE   2

/* Preamble length for SCO Data:
**      2-byte for Handle and 1 byte for length
*/
#define HCI_SCO_PREAMBLE_SIZE   3

/* Preamble length for ACL Data:
**      2-byte for Handle and 2 byte for length
*/
#define HCI_ACL_PREAMBLE_SIZE   4
#if !defined(USB_RX_PIPE)
#define RX_NEW_PKT              1
#define RECEIVING_PKT           2
#endif

#if !defined(USB_RX_PIPE)
#define CONTAINER_RX_HDR(ptr) \
      (RX_HDR *)((char *)(ptr) - offsetof(RX_HDR, data))

#define CONTAINER_ISO_HDR(ptr) \
      (ISO_HDR *)((char *)(ptr) - offsetof(ISO_HDR, data))
#endif
#define CONTAINER_CMD_HDR(ptr) \
      (CMD_HDR *)((char *)(ptr) - offsetof(CMD_HDR, data))

/******************************************************************************
**  Local type definitions
******************************************************************************/
/*
The mutex is protecting send_rx_event and rxed_xfer.

rxed_xfer     : Accounting the packet received at recv_xfer_cb() and processed
                at usb_read().
send_rx_event : usb_read() signals recv_xfer_cb() to signal  the
                Host/Controller lib thread about new packet arrival.

usb_read() belongs to Host/Controller lib thread.
recv_xfer_cb() belongs to USB read thread
*/

typedef struct
{
    libusb_device_handle      *handle;
    pthread_t                 read_thread;
#ifdef USB_RX_PIPE
    int                             rx_pipe[2]; // rx_pipe[0] for read, rx_pipe[1] for write
    struct libusb_transfer  *pending_event_pkt;
    struct libusb_transfer  *pending_bulk_pkt;
    struct libusb_transfer  *pending_iso_pkt;
    /* Indicate current frame type. 0 means none frame ongoing */
    /* H4_TYPE_EVENT, H4_TYPE_ACL_DATA, H4_TYPE_SCO_DATA */
    uint8_t                      frame_type;
    uint16_t                    frame_remain_len;
    uint8_t                      *p_bulk_buf;
    uint8_t                      *p_int_buf;
    uint8_t                      *p_iso_buf;
#else        
    pthread_mutex_t           mutex;
    pthread_cond_t            cond;
    int                       rxed_xfer;
    uint8_t                   send_rx_event;
    BUFFER_Q                  rx_eventq;
    BUFFER_Q                  rx_bulkq;
    BUFFER_Q                  rx_isoq;
    int16_t                   rx_pkt_len;
    uint8_t                   rx_status;
    int                       iso_frame_ndx;
#endif
    struct libusb_transfer    *failed_tx_xfer;
} tUSB_CB;

/******************************************************************************
**  Static variables
******************************************************************************/
/* The list will grow and will be updated from btusb.c in kernel */
static struct bt_usb_device btusb_table[] = {
    /* Generic Bluetooth USB device */
    { BT_USB_DEVICE_INFO(0xe0, 0x01, 0x01) },
    { }     /* Terminating entry */
};

static struct bt_usb_device btusb_table_t[] = {
    /* MediaTek MT76x0E */
    { USB_DEVICE(0x0e8d, 0x763f) },
    { }     /* Terminating entry */
};

#if !defined(USB_RX_PIPE)
typedef struct
{
    uint16_t          event;
    uint16_t          len;
    uint16_t          offset;
    unsigned char     data[0];
} RX_HDR;

struct iso_frames
{
    int               actual_length;
    int               length;
};

typedef struct
{
    uint16_t           event;
    uint16_t           len;
    uint16_t           offset;
    struct iso_frames  frames[BT_MAX_ISO_FRAMES];
    unsigned char      data[0];
} ISO_HDR;
#endif

typedef struct
{
    uint8_t                     event;
    struct libusb_control_setup setup;
    unsigned char               data[0];
} CMD_HDR;

static tUSB_CB usb;
static int usb_xfer_status, usb_running;
static int intr_pkt_size, iso_pkt_size, bulk_pkt_size;
#if !defined(USB_RX_PIPE)
static int intr_pkt_size_wh, iso_pkt_size_wh, bulk_pkt_size_wh;
#endif
static struct libusb_transfer *data_rx_xfer, *event_rx_xfer, *iso_rx_xfer,
                              *xmit_transfer;
static int xmited_len;
#if !defined(USB_RX_PIPE)
RX_HDR *p_rx_hdr = NULL;
#endif
/******************************************************************************
**  Static functions
******************************************************************************/
void utils_delay (uint32_t timeout)
{
    struct timespec delay;
    int err;

    delay.tv_sec = timeout / 1000;
    delay.tv_nsec = 1000 * 1000 * (timeout%1000);

    /* [u]sleep can't be used because it uses SIGALRM */
    do {
        err = nanosleep(&delay, &delay);
    } while (err < 0 && errno ==EINTR);
}

static int is_usb_match_idtable (struct bt_usb_device *id, struct libusb_device_descriptor *desc)
{
    int ret = TRUE;

    ret = ((id->bDevClass != libusb_le16_to_cpu(desc->bDeviceClass)) ? FALSE :
           (id->bDevSubClass != libusb_le16_to_cpu(desc->bDeviceSubClass)) ? FALSE :
           (id->bDevProtocol != libusb_le16_to_cpu(desc->bDeviceProtocol)) ? FALSE : TRUE);

    return ret;
}

static int is_usb_match_idtable_t (struct bt_usb_device *id, struct libusb_device_descriptor *desc)
{
    int ret = TRUE;
    ALOGE("dean TAG USB  device test2 :%d , %d\n", 
    id->idVendor == libusb_le16_to_cpu(desc->idVendor),
    id->idProduct == libusb_le16_to_cpu(desc->idProduct));
    
    ret = ((id->idVendor != libusb_le16_to_cpu(desc->idVendor)) ? FALSE :
           (id->idProduct != libusb_le16_to_cpu(desc->idProduct)) ? FALSE : TRUE);

    return ret;
}

static int check_bt_usb_endpoints (struct bt_usb_device *id, struct libusb_config_descriptor *cfg_desc)
{
    const struct libusb_interface_descriptor *idesc;
    const struct libusb_endpoint_descriptor *endpoint;
    int i, num_altsetting;

    endpoint =  cfg_desc->interface[0].altsetting[0].endpoint;
    for(i = 0; i < cfg_desc->interface[0].altsetting[0].bNumEndpoints; i++)
    {
        if(!(endpoint[i].bEndpointAddress == BT_CTRL_EP || \
              endpoint[i].bEndpointAddress == BT_INT_EP || \
              endpoint[i].bEndpointAddress == BT_BULK_IN || \
              endpoint[i].bEndpointAddress == BT_BULK_OUT))
            return FALSE;
    }

    num_altsetting =  cfg_desc->interface[1].num_altsetting;
    endpoint =  cfg_desc->interface[1].altsetting[num_altsetting-1].endpoint;
    for(i = 0; i < cfg_desc->interface[1].altsetting[num_altsetting-1]. \
          bNumEndpoints; i++)
    {
        if(!(endpoint[i].bEndpointAddress == BT_ISO_IN || \
              endpoint[i].bEndpointAddress == BT_ISO_OUT))
            return FALSE;
    }
    for(i = 0; i < cfg_desc->interface[1]. \
                  altsetting[num_altsetting-1].bNumEndpoints; i++)
    {
        if(endpoint[i].bEndpointAddress == BT_ISO_IN)
        {
            iso_pkt_size =  libusb_le16_to_cpu(endpoint[i].wMaxPacketSize);
            USBDBG("iso pkt size is %d", iso_pkt_size);
            #if !defined(USB_RX_PIPE)
            iso_pkt_size_wh = iso_pkt_size * BT_MAX_ISO_FRAMES + \
                  sizeof(ISO_HDR);
            USBDBG("iso pkt size wh %d", iso_pkt_size_wh);
            #endif
        }
   }
    return TRUE;
}

static int is_btusb_device (struct libusb_device *dev)
{
    struct bt_usb_device *id;
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *cfg_desc;
    int    r, match, num_altsetting = 0;

    r = libusb_get_device_descriptor(dev, &desc);
    ALOGE("dean TAG USB  device test desc :vid %x ,pid %x\n", desc.idVendor, desc.idProduct);
    if (r < 0)
        return FALSE;

    match = 0;

    for (id = btusb_table; id->bDevClass; id++)
    {
        if (is_usb_match_idtable (id, &desc) == TRUE)
        {
                match = 1;
                break;
        }
    }

    for (id = btusb_table_t; id->idVendor; id++)
    {
    		ALOGE("dean TAG check USB device id: pid%x ,vid %x\n", id->idVendor, id->idProduct);
        if (is_usb_match_idtable_t (id, &desc) == TRUE)
        {
                match = 1;
                ALOGE("dean TAG match : %d\n", match);
                break;
        }
    }

    if (!match)
    {
        return FALSE;
    }

    r = libusb_get_config_descriptor(dev, 0, &cfg_desc);
    ALOGE("dean TAG libusb_get_config_descriptor result : %d\n", r);
    if (r < 0)
    {
        USBERR("libusb_get_config_descriptor  %x:%x failed ....%d\n", \
               desc.idVendor, desc.idProduct, r);
        return FALSE;
    }

    r = check_bt_usb_endpoints(id, cfg_desc);
    libusb_free_config_descriptor(cfg_desc);
		ALOGE("dean TAG check_bt_usb_endpoints result : %d\n", r);
    return r;
}

/*******************************************************************************
**
** Function        libusb_open_bt_device
**
** Description     Scan the system USB devices. If match is found on
**                 btusb_table ensure that  it is a bluetooth device by
**                 checking Interface endpoint addresses.
**
** Returns         NULL: termination
**                 !NULL : pointer to the libusb_device_handle
**
*******************************************************************************/
static libusb_device_handle *libusb_open_bt_device()
{
    struct libusb_device **devs;
    struct libusb_device *found = NULL;
    struct libusb_device *dev;
    struct libusb_device_handle *handle = NULL;
    int    r, i;
    int count = 3;

    while(count--){
    if (libusb_get_device_list(NULL, &devs) < 0)
    {
        return NULL;
    }
    for (i = 0; (dev = devs[i]) != NULL; i++)
    {
        if (is_btusb_device (dev) == TRUE)
            break;
    }
        if (!dev){
            libusb_free_device_list(devs, 1);
            ALOGE("Can not find BT device. Retry : count=%d", count);
            usleep(500000);
        }else{
            break;
        }
    }
    if (dev)
    {
        r = libusb_open(dev, &handle);
        if (r < 0)
        {
            ALOGE("found USB BT device failed to open ..... %d\n", r);
            return NULL;
        }
    }
    else
    {
        ALOGE("No matching USB BT device found .....\n");
        return NULL;
    }

    libusb_free_device_list(devs, 1);
    libusb_detach_kernel_driver(handle, 0);

    sleep(1);
    r = libusb_claim_interface(handle, 0);
    if (r < 0)
    {
        ALOGE("usb_claim_interface 0 error %d\n", r);
        return NULL;
    }

    intr_pkt_size = libusb_get_max_packet_size(dev, BT_INT_EP);
    USBDBG("Interrupt pkt size is %d", intr_pkt_size);
    #if !defined(USB_RX_PIPE)
    intr_pkt_size_wh =  intr_pkt_size + sizeof(RX_HDR);
    #endif
    r = libusb_claim_interface(handle, 1);
    if (r < 0)
    {
        ALOGE("usb_claim_interface 1 error %d\n", r);
    }
    ALOGE("libusb_open_bt_device finish\n");
    return handle;
}

#if !defined(USB_RX_PIPE)
static void usb_rx_signal_event()
{
    USBDBG("usb_rx_signal_event : usb.rxed_xfer=%d", usb.rxed_xfer);
    pthread_mutex_lock(&usb.mutex);
    usb.rxed_xfer++;
    pthread_cond_signal(&usb.cond);
    if (usb.send_rx_event == TRUE)
    {
       //bthc_signal_event(HC_EVENT_RX);
       ALOGD("send user sig to indicate data indication");
       //kill(0, SIGUSR1);
       mtk_rx_callback();
       usb.send_rx_event = FALSE;
    }
    pthread_mutex_unlock(&usb.mutex);
    USBDBG("usb_rx_signal_event leave");
}
#endif

#ifdef USB_RX_PIPE
/* handler of int packet received from USB */
/* Return : 0 if completed, otherwise the packet is pending */
static int handle_int_pkt(struct libusb_transfer *transfer){
    //unsigned char *buf = NULL;
    int num_writed = 0, write_len = 0;

    USBDBG("+handle_int_pkt");
    /* Error checking */
    if(usb.frame_type == 0){
        /* Minus 1 to contain the frame type */
        usb.frame_type = H4_TYPE_EVENT;
        usb.frame_remain_len = transfer->buffer[EV_LEN_FIELD] + HCI_EVT_PREAMBLE_SIZE;
    }else if(usb.frame_type != H4_TYPE_EVENT){
        /* other frame ongoing. Need to pending. */
        if(usb.pending_event_pkt){
            USBERR("Receive event pkt when there is pending event pkt");
        }
        usb.pending_event_pkt = transfer;
        return 1;
    }

    usb.frame_remain_len -= transfer->actual_length;
    transfer->buffer += transfer->actual_length;
    if(usb.frame_remain_len == 0){
        usb.frame_type = 0;
        write_len =  (transfer->buffer - usb.p_int_buf);
        do{
            num_writed = write(usb.rx_pipe[1], usb.p_int_buf, write_len);
            USBDBG("write(%d, %p, %d)=%d, errno=%d", usb.rx_pipe[1], usb.p_int_buf, write_len, num_writed, errno);
        }while(num_writed < 0 && errno == EINTR);
        if(num_writed != write_len){
            USBERR("num_writed(%d) != (transfer->actual_length(%d)+1)", num_writed, write_len);
        }
        transfer->buffer = (usb.p_int_buf+1);
    }else{
        USBDBG("int remain len = %u", usb.frame_remain_len);
    }
    USBDBG("-handle_int_pkt");
    return 0;
}

/* handler of bulk packet received from USB */
/* Return : 0 if completed, otherwise the packet is pending */
static int handle_bulk_pkt(struct libusb_transfer *transfer){
    unsigned char *buf = NULL;
    int num_writed = 0, write_len = 0;

    USBDBG("+handle_bulk_pkt");
    
    /* Error checking */
    if(usb.frame_type == 0){
        /* Minus 1 to contain the frame type */
        usb.frame_type = H4_TYPE_ACL_DATA;
        usb.frame_remain_len = ((uint16_t)transfer->buffer[BLK_LEN_LO] | (uint16_t)transfer->buffer[BLK_LEN_HI] << 8) + HCI_ACL_PREAMBLE_SIZE;
    }else if(usb.frame_type != H4_TYPE_ACL_DATA){
        /* other frame ongoing. Need to pending. */
        if(usb.pending_bulk_pkt){
            USBERR("Receive bulk pkt when there is pending bulk pkt");
        }
        usb.pending_bulk_pkt = transfer;
        return 1;
    }

    usb.frame_remain_len -= transfer->actual_length;
    transfer->buffer += transfer->actual_length;
    if(usb.frame_remain_len == 0){
        usb.frame_type = 0;
        write_len =  (transfer->buffer - usb.p_bulk_buf);
        do{
            num_writed = write(usb.rx_pipe[1], usb.p_bulk_buf, write_len);
            USBDBG("write(%d, %p, %d)=%d, errno=%d", usb.rx_pipe[1], usb.p_bulk_buf, write_len, num_writed, errno);
        }while(num_writed < 0 && errno == EINTR);
        if(num_writed != write_len){
            USBERR("num_writed(%d) != (transfer->actual_length(%d)+1)", num_writed, write_len);
        }
        transfer->buffer = (usb.p_bulk_buf+1);
    }else{
        USBDBG("int remain len = %u", usb.frame_remain_len);
    }
    USBDBG("-handle_bulk_pkt");
    return 0;
}

/* handler of iso packet received from USB */
/* Return : 0 if completed, otherwise the packet is pending */
static int handle_iso_pkt(struct libusb_transfer *transfer){
    struct libusb_iso_packet_descriptor frame;
    unsigned char *buf = NULL;
    int num_writed = 0;
    int i;

    USBDBG("+handle_iso_pkt");
    
    /* Error checking */
    if(usb.frame_type == 0){
        /* Minus 1 to contain the frame type */
        buf = transfer->buffer - 1;
        usb.frame_type = H4_TYPE_SCO_DATA;
        //usb.frame_remain_len = 1 + ((uint16_t)transfer->buffer[BLK_LEN_LO] | (uint16_t)transfer->buffer[BLK_LEN_HI] << 8) + HCI_ACL_PREAMBLE_SIZE;
    }else if(usb.frame_type == H4_TYPE_SCO_DATA){
        buf = transfer->buffer;
    }else{
        /* other frame ongoing. Need to pending. */
        if(usb.pending_iso_pkt){
            USBERR("Receive event pkt when there is pending event pkt");
        }
        usb.pending_iso_pkt = transfer;
        return 1;
    }

    buf = transfer->buffer-1;
    for(i = 0; i < transfer->num_iso_packets; i++)
    {
        frame.length = transfer->iso_packet_desc[i].length;
        frame.actual_length = transfer->iso_packet_desc[i].actual_length;
        USBDBG("frames->length=%u, frames->actual_length=%u",frame.length, frame.actual_length);
        *buf = H4_TYPE_SCO_DATA;
        do{
            num_writed = write(usb.rx_pipe[1], buf, frame.actual_length+1);
        }while(num_writed < 0 && errno == EINTR);
        if(num_writed <= 0){
            USBERR("write failed : ret=%d", num_writed);
            break;
        }else{
            if(num_writed != (int)(frame.actual_length+1)){
                USBERR("num_writed(%d) != (transfer->actual_length(%u)+1)", num_writed, frame.actual_length);
                break;
            }else{
            }
        }
        *buf += frame.length;
    }
    usb.frame_type = 0;
    USBDBG("-handle_iso_pkt");
    return 0;
}
#endif

static void recv_xfer_cb(struct libusb_transfer *transfer)
{
#ifdef USB_RX_PIPE
    int pending = 0;
    int r;
    enum libusb_transfer_status status;

    status = transfer->status;
    USBDBG("[RX] recv_xfer_cb : status=%d, endpoint=%u", status, transfer->endpoint);
    if (status == LIBUSB_TRANSFER_COMPLETED)
    {
        if (transfer->actual_length == 0){
            USBDBG("*****Rxed zero length packet from usb ....");
            r = libusb_submit_transfer(transfer);
            if(r < 0){
                USBERR("submit transfer failed.");
                usb_xfer_status |= RX_FAILED;
            }
        }else{
            switch (transfer->endpoint){
            case BT_INT_EP:
                pending = handle_int_pkt(transfer);
                break;
            case BT_BULK_IN:
                pending = handle_bulk_pkt(transfer);
                break;
            case BT_ISO_IN:
                pending = handle_iso_pkt(transfer);
                break;
            default:
                USBERR("Invalid USB endpoint : typ=%u", transfer->endpoint);
                break;
            }
            if( !pending ){
                r = libusb_submit_transfer(transfer);
                if(r < 0){
                    USBERR("submit transfer failed.");
                    usb_xfer_status |= RX_FAILED;
                }
            }
            /* if completed, process pending pkt */
            if(!pending && usb.pending_event_pkt){
                pending = handle_int_pkt(usb.pending_event_pkt);
                if(!pending){
                    r = libusb_submit_transfer(usb.pending_event_pkt );
                    if(r < 0){
                        USBERR("submit event transfer failed.");
                        usb_xfer_status |= RX_FAILED;
                    }
                    USBDBG("submit event transfer. endpoint=%u, ret=%d", usb.pending_event_pkt ->endpoint, r);
                    usb.pending_event_pkt = NULL;
                }
            }
            if(!pending && usb.pending_iso_pkt){
                pending = handle_iso_pkt(usb.pending_iso_pkt);
                if(!pending){
                    r = libusb_submit_transfer(usb.pending_iso_pkt);
                    if(r < 0){
                        USBERR("submit iso transfer failed.");
                        usb_xfer_status |= RX_FAILED;
                    }
                    USBDBG("submit iso transfer. endpoint=%u, ret=%d", usb.pending_iso_pkt ->endpoint, r);
                    usb.pending_iso_pkt = NULL;
                }
            }
            if(!pending && usb.pending_bulk_pkt){
                pending = handle_bulk_pkt(usb.pending_bulk_pkt);
                if(!pending){
                    r = libusb_submit_transfer(usb.pending_bulk_pkt);
                    if(r < 0){
                        USBERR("submit bulk transfer failed.");
                        usb_xfer_status |= RX_FAILED;
                    }
                    USBDBG("submit bulk transfer. endpoint=%u, ret=%d", usb.pending_bulk_pkt ->endpoint, r);
                    usb.pending_bulk_pkt = NULL;
                }
            }
        }
    }else
    {
        USBERR("******Transfer to BT Device failed- %d *****", status);
        usb_xfer_status |= RX_DEAD;
        return;
    }
#else
#ifdef USB_RX_PIPE
    int pkt_pending = 0;
#else
    RX_HDR *p_rx = NULL;
    ISO_HDR *p_iso = NULL;
#endif
    int r, i, offset = 0, len = 0, skip = 0;
    enum libusb_transfer_status status;

    status = transfer->status;
    USBDBG("recv_xfer_cb : status=%d", status);
    if (status == LIBUSB_TRANSFER_COMPLETED)
    {
        switch (transfer->endpoint)
        {
            #ifdef USB_RX_PIPE
            struct iso_frames frame;
            #else
            struct iso_frames *frames;
            #endif
            case BT_INT_EP:
                if (transfer->actual_length == 0)
                {
                    USBDBG("*****Rxed zero length packet from usb ....");
                    skip = 1;
                    break;
                }
                #ifdef USB_RX_PIPE
                pkt_pending = handle_int_pkt(transfer);
                #else
                p_rx = CONTAINER_RX_HDR(transfer->buffer);
                p_rx->event = H4_TYPE_EVENT;
                p_rx->len = (uint16_t)transfer->actual_length;
                utils_enqueue(&(usb.rx_eventq), p_rx);
                p_rx =  (RX_HDR *) bt_hc_cbacks->alloc(intr_pkt_size_wh);
                transfer->buffer = p_rx->data;
                #endif
                transfer->length = intr_pkt_size;
                break;

            case BT_BULK_IN:
                if (transfer->actual_length == 0)
                {
                    USBDBG("*******Rxed zero length packet from usb ....");
                    skip = 1;
                    break;
                }
                #ifdef USB_RX_PIPE
                pkt_pending = handle_bulk_pkt(transfer);
                #else
                p_rx = CONTAINER_RX_HDR(transfer->buffer);
                p_rx->event = H4_TYPE_ACL_DATA;
                p_rx->len = (uint16_t)transfer->actual_length;
                utils_enqueue(&(usb.rx_bulkq), p_rx);
                p_rx =  (RX_HDR *) bt_hc_cbacks->alloc(bulk_pkt_size_wh);
                transfer->buffer = p_rx->data;
                #endif
                transfer->length = bulk_pkt_size;
                break;

            case BT_ISO_IN:
                if (transfer->actual_length == 0)
                {
                    USBDBG("*******Rxed zero length packet from usb ....");
                    skip = 1;
                    break;
                }
                #ifdef USB_RX_PIPE
                pkt_pending = handle_iso_pkt(transfer);
                #else
                p_iso = CONTAINER_ISO_HDR(transfer->buffer);
                frames = p_iso->frames;
                memset(frames, 0, sizeof(struct iso_frames) * BT_MAX_ISO_FRAMES);
                p_iso->event = H4_TYPE_SCO_DATA;
                for(i = 0; i < transfer->num_iso_packets; i++, frames++)
                {
                    frames->length = transfer->iso_packet_desc[i].length;
                    frames->actual_length = \
                          transfer->iso_packet_desc[i].actual_length;
                    len += frames->actual_length;
                }
                p_iso->len = (uint16_t)len;
                utils_enqueue(&(usb.rx_isoq), p_iso);
                p_iso =  (ISO_HDR *) bt_hc_cbacks->alloc(iso_pkt_size_wh);
                transfer->buffer = p_iso->data;
                #endif
                for(i = 0; i < BT_MAX_ISO_FRAMES; i++)
                {
                    transfer->iso_packet_desc[i].length = iso_pkt_size;
                }
                break;

            default:
                USBERR("Unexpeted endpoint rx %d\n", transfer->endpoint);
                return;
        }
#if !defined(USB_RX_PIPE)        
        if (!skip)
            usb_rx_signal_event();
#endif
    }
    else
    {
        USBERR("******Transfer to BT Device failed- %d *****", status);
        usb_xfer_status |= RX_DEAD;
        return;
    }

#ifdef USB_RX_PIPE
    if(pkt_pending){
        USBDBG("pkt is pending. wait for rxing frame completed");
        return;
    }
#endif
    
    ALOGD("+Enter libusb_submit_transfer : transfer=%p", transfer);
    r = libusb_submit_transfer(transfer);
    ALOGD("-Enter libusb_submit_transfer : r=%d", r);
    if (r < 0)
    {
        #ifdef USB_RX_PIPE
        //buf = transfer->buffer-1;
        USB_FREE((transfer->buffer-1));
        #else
        if (transfer->endpoint == BT_ISO_IN)
        {
            p_rx = (RX_HDR *)CONTAINER_ISO_HDR(transfer->buffer);
        }
        else
        {
            p_rx = CONTAINER_RX_HDR(transfer->buffer);
        }
        bt_hc_cbacks->dealloc((TRANSAC)p_rx, (char *)(p_rx + 1));
        #endif
        transfer->buffer = NULL;
        USBERR("libusb_submit_transfer : %d : %d : failed", \
               transfer->endpoint, transfer->status);
        usb_xfer_status |= RX_FAILED;
    }
    USBDBG("recv_xfer_cb");
#endif
}

void handle_usb_events ()
{
#ifdef USB_RX_PIPE
    unsigned char *buf;
#else
    RX_HDR  *rx_buf;
    ISO_HDR *iso_buf;
#endif
    int  r, i, iso_xfer;
    struct libusb_transfer *transfer;
    struct timeval timeout = { 1, 0 };

    usb_xfer_status &= ~RX_DEAD;
    while (!(usb_xfer_status & RX_DEAD))
    {
        // This polling introduces two problems:
        //  1) /1s device wakeups when BT is on
        //  2) ~0.5s response to user's shutdown request
        libusb_handle_events_timeout(0, &timeout);
        transfer = NULL;
        iso_xfer = 0;
        USBDBG("return rom libusb_handle_events_timeout : usb_xfer_status=%d", usb_xfer_status);
        if (usb_xfer_status & RX_FAILED)
        {
            if (data_rx_xfer->buffer == NULL)
            {
                transfer = data_rx_xfer;
                #ifdef USB_RX_PIPE
                buf = USB_ALLOC(1+bulk_pkt_size);
                *buf = H4_TYPE_ACL_DATA;
                if (buf == NULL)
                {
                    USBERR("%s : Allocation failed", __FUNCTION__);
                    transfer = NULL;
                }
                else
                {
                    transfer->buffer = buf+1;
                    transfer->length = bulk_pkt_size;
                }
                #else
                rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(bulk_pkt_size_wh);
                if (rx_buf == NULL)
                {
                    USBERR("%s : Allocation failed", __FUNCTION__);
                    transfer = NULL;
                }
                else
                {
                    transfer->buffer = rx_buf->data;
                    transfer->length = bulk_pkt_size;
                }
                #endif
            }
            else if (event_rx_xfer->buffer == NULL)
            {
                transfer = event_rx_xfer;
                #ifdef USB_RX_PIPE
                buf = USB_ALLOC(1+intr_pkt_size);
                *buf = H4_TYPE_EVENT;
                if (buf == NULL)
                {
                    USBERR("%s : Allocation failed", __FUNCTION__);
                    transfer = NULL;
                }
                else
                {
                    transfer->buffer = buf+1;
                    transfer->length = intr_pkt_size;
                }
                #else
                rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(intr_pkt_size_wh);
                if (rx_buf == NULL)
                {
                    USBERR("%s : Allocation failed", __FUNCTION__);
                    transfer = NULL;
                }
                else
                {
                    transfer->buffer = rx_buf->data;
                    transfer->length = intr_pkt_size;
                }
                #endif
            }
            else if (iso_rx_xfer->buffer == NULL)
            {
                transfer = iso_rx_xfer;
                #ifdef USB_RX_PIPE
                buf = USB_ALLOC(1+BT_MAX_ISO_FRAMES*iso_pkt_size);
                *buf = H4_TYPE_SCO_DATA;
                if (buf == NULL)
                {
                    USBERR("%s : Allocation failed", __FUNCTION__);
                    transfer = NULL;
                }
                else
                {
                    transfer->buffer = buf+1;
                    transfer->length = BT_MAX_ISO_FRAMES * iso_pkt_size;
                    for(i = 0; i < transfer->num_iso_packets; i++)
                    {
                        transfer->iso_packet_desc[i].length = iso_pkt_size;
                    }
                    iso_xfer = 1;
                }
                #else
                iso_buf = (ISO_HDR *) bt_hc_cbacks->alloc(iso_pkt_size_wh);
                if (iso_buf == NULL)
                {
                    USBERR("%s : Allocation failed", __FUNCTION__);
                    transfer = NULL;
                }
                else
                {
                    transfer->buffer = iso_buf->data;
                    transfer->length = BT_MAX_ISO_FRAMES * iso_pkt_size;
                    for(i = 0; i < transfer->num_iso_packets; i++)
                    {
                        transfer->iso_packet_desc[i].length = iso_pkt_size;
                    }
                    iso_xfer = 1;
                }
                #endif
            }
            if (transfer != NULL)
            {
                usb_xfer_status &= ~(RX_FAILED);
                r = libusb_submit_transfer(transfer);
                if (r < 0)
                {
                    USBERR("libusb_submit_transfer : data_rx_xfer failed");
                    #ifdef USB_RX_PIPE
                    buf = transfer->buffer - 1;
                    USB_FREE(buf);
                    #else
                    if (iso_xfer)
                    {
                        bt_hc_cbacks->dealloc((TRANSAC) iso_buf, \
                              (char *)(iso_buf + 1));
                    }
                    else
                    {
                        bt_hc_cbacks->dealloc((TRANSAC) rx_buf, \
                              (char *)(rx_buf + 1));
                    }
                    #endif
                    transfer->buffer = NULL;
                }
            }
         }
         else if (usb_xfer_status & XMIT_FAILED)
         {
            transfer = usb.failed_tx_xfer;
            USBDBG("Retransmitting xmit packet %d", *(transfer->buffer - 1));
            xmited_len = transfer->length;
            usb_xfer_status &= ~(XMIT_FAILED);
            if (libusb_submit_transfer(transfer) < 0)
            {
                USBERR("libusb_submit_transfer : %d : failed", *(transfer->buffer - 1));
                /* SH : If failed to submit transferm, set xmited_len to -1 so that usb_write can return */
                xmited_len = -1;
                usb_xfer_status  |= XMITTED;
                /* SH : If failed to submit transferm, set xmited_len to -1 so that usb_write can return */
            }
         }
      }
     usb_running = 0;
}

/*******************************************************************************
**
** Function        usb_read_thread
**
** Description
**
** Returns         void *
**
*******************************************************************************/
static void *usb_read_thread(void *arg)
{
#if !defined(USB_RX_PIPE)
    RX_HDR  *rx_buf;
    ISO_HDR *iso_buf;
#endif
    int size, size_wh, r, i, iso_xfer;
    struct libusb_transfer *transfer;
    unsigned char *buf;

    USBDBG("Entering usb_read_thread()");
    prctl(PR_SET_NAME, (unsigned long)"usb_read", 0, 0, 0);

#ifdef USB_RX_PIPE
    //buf = USB_ALLOC(1 + bulk_pkt_size);
    buf = usb.p_bulk_buf;
    *buf = H4_TYPE_ACL_DATA;
    buf++;
#else
    rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(bulk_pkt_size_wh);
    buf =  rx_buf->data;
#endif
    libusb_fill_bulk_transfer(data_rx_xfer, usb.handle, BT_BULK_IN, buf, bulk_pkt_size, recv_xfer_cb, NULL, 0);
    r = libusb_submit_transfer(data_rx_xfer);
    if (r < 0)
    {
        USBERR("libusb_submit_transfer : data_rx_xfer : failed");
        goto out;
    }

#ifdef USB_RX_PIPE
    //buf = USB_ALLOC(1 + intr_pkt_size);
    buf = usb.p_int_buf;
    *buf = H4_TYPE_EVENT;
    buf++;
#else
    rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(intr_pkt_size_wh);
    buf = rx_buf->data;
#endif
    libusb_fill_interrupt_transfer(event_rx_xfer, usb.handle, BT_INT_EP, buf, intr_pkt_size, recv_xfer_cb, NULL, 0);
    r = libusb_submit_transfer(event_rx_xfer);
    if (r < 0)
    {
        USBERR("libusb_submit_transfer : event_rx_xfer : failed");
        goto out;
    }

#ifdef USB_RX_PIPE
    //buf = USB_ALLOC(1 + iso_pkt_size);
    buf = usb.p_iso_buf;
    *buf = H4_TYPE_SCO_DATA;
    buf++;
#else
    iso_buf =  (ISO_HDR *) bt_hc_cbacks->alloc(iso_pkt_size_wh);
    buf = iso_buf->data;
#endif
    libusb_fill_iso_transfer(iso_rx_xfer, usb.handle, BT_ISO_IN, buf, \
          iso_pkt_size * BT_MAX_ISO_FRAMES, BT_MAX_ISO_FRAMES, recv_xfer_cb, \
          NULL, 0);
    libusb_set_iso_packet_lengths (iso_rx_xfer, iso_pkt_size);

    usb_running = 1;
    handle_usb_events();
out:
    USBDBG("Leaving usb_read_thread()");
    if (data_rx_xfer != NULL)
    {
        #ifdef USB_RX_PIPE
        //buf = data_rx_xfer->buffer-1;
        //USB_FREE(buf);
        #else
        rx_buf = CONTAINER_RX_HDR(data_rx_xfer->buffer);
        bt_hc_cbacks->dealloc((TRANSAC) rx_buf, (char *)(rx_buf+1));
        #endif
        libusb_free_transfer(data_rx_xfer);
    }
    if (event_rx_xfer != NULL)
    {
        #ifdef USB_RX_PIPE
        //buf = event_rx_xfer->buffer-1;
        //USB_FREE(buf);
        #else
        rx_buf = CONTAINER_RX_HDR(event_rx_xfer->buffer);
        bt_hc_cbacks->dealloc((TRANSAC) rx_buf, (char *)(rx_buf+1));
        #endif
        libusb_free_transfer(event_rx_xfer);
    }
    if(iso_rx_xfer != NULL)
    {
        #ifdef USB_RX_PIPE
        //buf = iso_rx_xfer->buffer-1;
        //USB_FREE(buf);
        #else
        iso_buf = CONTAINER_ISO_HDR(iso_rx_xfer->buffer);
        bt_hc_cbacks->dealloc((TRANSAC) iso_buf, (char *)(iso_buf+1));
        #endif
        libusb_free_transfer(iso_rx_xfer);
    }

    pthread_exit(NULL);

    return NULL;
}


/*****************************************************************************
**   USB API Functions
*****************************************************************************/

int usb_getRXfd(void){
    return usb.rx_pipe[0];
}

/*******************************************************************************
**
** Function        usb_init
**
** Description     Initializes the serial driver for usb
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t usb_init(void)
{
    USBDBG("usb_init");
    memset(&usb, 0, sizeof(tUSB_CB));
    usb.handle = NULL;
#ifdef USB_RX_PIPE
    usb.rx_pipe[0] = -1;
    usb.rx_pipe[1] = -1;
#else
    utils_queue_init(&(usb.rx_eventq));
    utils_queue_init(&(usb.rx_bulkq));
    utils_queue_init(&(usb.rx_isoq));
    pthread_mutex_init(&usb.mutex, NULL);
    pthread_cond_init(&usb.cond, NULL);
#endif
    data_rx_xfer = event_rx_xfer = iso_rx_xfer = NULL;
#if !defined(USB_RX_PIPE)
    usb.send_rx_event = TRUE;
    usb.rx_status = RX_NEW_PKT;
#endif

    return TRUE;
}

static int setUartBlockMode(int fd, int block) 
{
    int val;

    /* uart handle is opened as blocking, change it to non-blocking */
    if( (val = fcntl(fd, F_GETFL, 0)) < 0) 
    {
        USBDBG("[ERR] get handle flag failed");
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
        val = fcntl(fd, F_SETFL, val);
    }
    USBDBG("setUartBlockMode(%d,%d)=%d", fd, block, val);
    return val;
}

/*******************************************************************************
**
** Function        usb_open
**
** Description     Open Bluetooth device with the port ID
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t usb_open(uint8_t port)
{

    USBDBG("usb_open(port:%d)", port);

    if (usb_running)
    {
        /* Userial is open; close it first */
        usb_close();
        utils_delay(50);
    }
    if (libusb_init(NULL) < 0)
    {
        ALOGE("libusb_init : failed");
        return FALSE;
    }

    usb.handle = libusb_open_bt_device();
    #if !defined(USB_RX_PIPE)
    bulk_pkt_size_wh = BT_HCI_MAX_FRAME_SIZE + sizeof(RX_HDR);
    #endif
    bulk_pkt_size = BT_HCI_MAX_FRAME_SIZE;
    if (usb.handle == NULL)
    {
        ALOGE("usb_open: HCI USB failed to open");
        goto out;
    }
    ALOGE("dean TAG usb_open: HCI USB open OK");
    
    data_rx_xfer = libusb_alloc_transfer(0);
    if (!data_rx_xfer)
    {
        ALOGE("Failed alloc data_rx_xfer");
        goto out;
    }

    event_rx_xfer  = libusb_alloc_transfer(0);
    if (!event_rx_xfer)
    {
        ALOGE("Failed alloc event_rx_xfer");
        goto out;
    }


    iso_rx_xfer =  libusb_alloc_transfer(BT_MAX_ISO_FRAMES);
    if (!iso_rx_xfer)
    {
        ALOGE("Failed alloc iso_rx_xfer");
        goto out;
    }

#ifdef USB_RX_PIPE
    if( pipe(usb.rx_pipe) < 0 )
    {
        ALOGE("Can no open RX pipe.");
        goto out;
    }
    setUartBlockMode(usb.rx_pipe[0], 0);
    usb.pending_event_pkt = NULL;
    usb.pending_bulk_pkt = NULL;
    usb.pending_iso_pkt = NULL;
    usb.frame_type = 0;
    usb.p_int_buf = USB_ALLOC(1 + BT_HCI_MAX_EVENT_SIZE);
    usb.p_bulk_buf = USB_ALLOC(1 + BT_HCI_MAX_FRAME_SIZE);
    usb.p_iso_buf = USB_ALLOC(1 + BT_MAX_ISO_FRAMES*iso_pkt_size);
#endif

    ALOGE("dean TAG usb_read_thread is created ...");
    USBDBG("usb_read_thread is created ....");
    if (pthread_create(&(usb.read_thread), NULL, \
                       usb_read_thread, NULL) != 0 )
    {
    		ALOGE("dean TAG usb_open: pthread_create failed!");
        USBERR("pthread_create failed!");
        goto out;
    }

    return TRUE;
out :
    if (usb.handle != NULL)
    {
        if (data_rx_xfer != NULL)
            libusb_free_transfer(data_rx_xfer);
        if (event_rx_xfer != NULL)
            libusb_free_transfer(event_rx_xfer);
        if (iso_rx_xfer != NULL)
            libusb_free_transfer(iso_rx_xfer);
        libusb_release_interface(usb.handle, 1);
        libusb_release_interface(usb.handle, 0);
        libusb_close(usb.handle);
        libusb_exit(NULL);
    }
    return FALSE;
}


/*******************************************************************************
**
** Function        usb_read
**
** Description     Read data from the usb port
**
** Returns         Number of bytes actually read from the usb port and
**                 copied into p_data.  This may be less than len.
**
*******************************************************************************/
uint16_t  usb_read(uint16_t msg_id, uint8_t *p_buffer, uint16_t len)
{
#ifdef USB_RX_PIPE
    int num_read = 0;
#else
    uint16_t total_len = 0;
    uint16_t copy_len = 0;
    uint8_t *p_data = NULL, iso_idx;
    int iso_frame_len = 0;
    int different_xfer = 0;
    struct iso_frames *frames;
    int pkt_rxing = 0;
    int rem_len = 0;
    ISO_HDR *p_iso_hdr;
#endif

    if (!usb_running)
        return 0;
#ifdef USB_RX_PIPE
    do{
        num_read = read(usb.rx_pipe[0], p_buffer, len);
    }while(num_read < 0 && errno == EINTR);
    if(num_read < 0) return 0;
    if(num_read != (int)len){
        USBERR("num_read(%d) != %u", num_read, len);
    }
    return num_read;
#else
    while (total_len < len)
    {
        if (p_rx_hdr == NULL)
        {
            pthread_mutex_lock(&usb.mutex);
            if (usb.rxed_xfer < 0)
            {
                USBERR("Rx thread and usb_read out of sync %d", \
                       usb.rxed_xfer);
                usb.rxed_xfer = 0;
            }
            if (usb.rxed_xfer == 0 && usb.rx_status == RX_NEW_PKT)
            {
                usb.send_rx_event = TRUE;
                pthread_mutex_unlock(&usb.mutex);
                USBDBG("usb_read nothing to rx....");
                return 0;

            }
            while (usb.rxed_xfer == 0)
            {
               pthread_cond_wait(&usb.cond, &usb.mutex);
            }
            usb.rxed_xfer--;
            pthread_mutex_unlock(&usb.mutex);

            if (usb.rx_status == RX_NEW_PKT)
            {
                p_rx_hdr = (RX_HDR *)utils_dequeue(&(usb.rx_eventq));
                if (p_rx_hdr == NULL)
                {
                    p_rx_hdr = (RX_HDR *)utils_dequeue(&(usb.rx_isoq));
                }
                if (p_rx_hdr == NULL)
                {
                    p_rx_hdr = (RX_HDR *)utils_dequeue(&(usb.rx_bulkq));
                }
                if (p_rx_hdr == NULL)
                {
                    USBERR("rxed_xfer is %d but no packet found", usb.rxed_xfer);
                    return 0;
                }
                switch (p_rx_hdr->event)
                {
                    case H4_TYPE_EVENT:
                        p_data = p_rx_hdr->data;
                        p_rx_hdr->offset = 0;
                        usb.rx_pkt_len = p_data[EV_LEN_FIELD] + \
                              HCI_EVT_PREAMBLE_SIZE;
                        usb.rx_status = RECEIVING_PKT;
                        *p_buffer = p_rx_hdr->event;
                        total_len += 1;
                        p_buffer++;
                        break;


                    case H4_TYPE_SCO_DATA:
                        p_iso_hdr = (ISO_HDR *)p_rx_hdr;
                        p_iso_hdr->offset = 0;
                        usb.rx_pkt_len = 0;
                        usb.rx_status = RECEIVING_PKT;
                        usb.iso_frame_ndx = 0;
                        pthread_mutex_lock(&usb.mutex);
                        usb.rxed_xfer++;
                        pthread_mutex_unlock(&usb.mutex);
                        break;


                    case H4_TYPE_ACL_DATA:
                        p_data = p_rx_hdr->data;
                        p_rx_hdr->offset = 0;
                        usb.rx_pkt_len = ((uint16_t)p_data[BLK_LEN_LO] | \
                              (uint16_t)p_data[BLK_LEN_HI] << 8) + \
                               HCI_ACL_PREAMBLE_SIZE;
                        usb.rx_status = RECEIVING_PKT;
                        *p_buffer = p_rx_hdr->event;
                        total_len += 1;
                        p_buffer++;
                        break;
                }
                USBDBG("Received packet from usb of len %d of type %x", \
                      p_rx_hdr->len, p_rx_hdr->event);
            }
            else   // rx_status == RECIVING_PKT
            {
                switch (pkt_rxing)
                {
                    case H4_TYPE_EVENT:
                        p_rx_hdr = (RX_HDR *)utils_dequeue(&(usb.rx_eventq));
                        break;

                    case H4_TYPE_SCO_DATA:
                        p_rx_hdr = (RX_HDR *)utils_dequeue(&(usb.rx_isoq));
                        break;

                    case H4_TYPE_ACL_DATA:
                        p_rx_hdr = (RX_HDR *)utils_dequeue(&(usb.rx_bulkq));
                        break;
                 }
                 if (p_rx_hdr == NULL)
                 {
                     USBDBG("Rxed packet from different end_point.");
                     different_xfer++;
                 }
                 else
                 {
                     p_rx_hdr->offset = 0;
                     USBDBG("Received packet from usb of len %d of type %x",
                          p_rx_hdr->len, p_rx_hdr->event);
                 }
             }
        }
        else //if (p_rx_hdr != NULL)
        {
            if (p_rx_hdr->event  == H4_TYPE_SCO_DATA)
            {
                p_iso_hdr = (ISO_HDR *)p_rx_hdr;
                frames = p_iso_hdr->frames;
                p_data = p_iso_hdr->data;
                frames += usb.iso_frame_ndx;
                if (usb.rx_pkt_len == 0) // Start of new micro frame
                {
                    if (frames->actual_length == 0)
                    {
	                 /* Previous frame has been processed */
                        usb.iso_frame_ndx++;
                        p_iso_hdr->offset = usb.iso_frame_ndx * iso_pkt_size;
                    }
                    *p_buffer = p_iso_hdr->event;
                    total_len += 1;
                    usb.rx_pkt_len = (uint16_t)p_data[p_iso_hdr->offset + \
                          SCO_LEN_FIELD] + HCI_SCO_PREAMBLE_SIZE;
                    p_buffer++;
                    usb.rx_status = RECEIVING_PKT;
                }
                else
                {
                    frames->actual_length -= len;
                }
                if (total_len == len)
                    break;
                pkt_rxing = p_iso_hdr->event;

                if((p_iso_hdr->len) <= (len - total_len))
                    copy_len = p_iso_hdr->len;
                else
                    copy_len = (len - total_len);

                p_iso_hdr->offset += copy_len;
                p_iso_hdr->len -= copy_len;
                rem_len = p_iso_hdr->len;
            }
            else
            {
                p_data = p_rx_hdr->data + p_rx_hdr->offset;
                pkt_rxing = p_rx_hdr->event;

                if((p_rx_hdr->len) <= (len - total_len))
                    copy_len = p_rx_hdr->len;
                else
                    copy_len = (len - total_len);

                p_rx_hdr->offset += copy_len;
                p_rx_hdr->len -= copy_len;
                rem_len = p_rx_hdr->len;
            }

            memcpy((p_buffer + total_len), p_data, copy_len);
            total_len += copy_len;

            if (rem_len == 0)
            {
               bt_hc_cbacks->dealloc((TRANSAC) p_rx_hdr, (char *)(p_rx_hdr+1));
               p_rx_hdr = NULL;
            }
            usb.rx_pkt_len -= copy_len;
            if (usb.rx_pkt_len == 0)
            {
                usb.rx_status = RX_NEW_PKT;
                break;
            }
            if (usb.rx_pkt_len < 0)
            {
                USBERR("pkt len expected %d rxed len of %d", len, total_len);
                usb.rx_status = RX_NEW_PKT;
                break;
            }
        }
    }
    if (different_xfer)
    {
        pthread_mutex_lock(&usb.mutex);
        usb.rxed_xfer += different_xfer;
        pthread_mutex_unlock(&usb.mutex);
    }
    return total_len;
#endif
}
/*******************************************************************************
**
** Function        xmit_xfer_cb
**
** Description     Callback function after xmission
**
** Returns         None
**
**
*******************************************************************************/
static void xmit_xfer_cb(struct libusb_transfer *transfer)
{
    enum libusb_transfer_status status = transfer->status;
    static int xmit_acked;
    USBDBG("xmit_xfer_cb : transfer=%p", transfer);
    if(transfer->status != LIBUSB_TRANSFER_COMPLETED)
    {
        USBERR("xfer did not succeeded .....%d", transfer->status);
        usb_xfer_status |= XMIT_FAILED;
        usb.failed_tx_xfer = transfer;
        xmited_len = 0;
    } else
    {
        xmited_len = transfer->actual_length+1;
        libusb_free_transfer(transfer);
        usb_xfer_status  |= XMITTED;
        USBDBG("Xfer Succeded : count %d", ++xmit_acked);
    }
}
/*******************************************************************************
**
** Function        usb_write
**
** Description     Write data to the usb port
**
** Returns         Number of bytes actually written to the usb port. This
**                 may be less than len.
**
*******************************************************************************/
uint16_t usb_write(uint16_t msg_id, uint8_t *p_data, uint16_t len)
{
    struct timeval tv = {60, 0};
    char buffer[512], pkt_type;;
    int x, i;
    CMD_HDR *cmd_hdr;
    static int xmit_count;

    if(!(xmit_transfer = libusb_alloc_transfer(0)))
    {
        USBERR( "libusb_alloc_tranfer() failed");
        return 0;
    }

    x = (len > (sizeof(buffer)-1)/2)? ((sizeof(buffer)-1)/2) : len;

    pkt_type = *p_data;
    switch(pkt_type)
    {
        case H4_TYPE_COMMAND:
            /* Make use of BT_HDR space to populate setup */
            cmd_hdr = CONTAINER_CMD_HDR(p_data + 1);
            cmd_hdr->setup.bmRequestType = USB_TYPE_REQ;
            cmd_hdr->setup.wLength = len - 1;
            cmd_hdr->setup.wIndex = 0;
            cmd_hdr->setup.wValue = 0;
            cmd_hdr->setup.bRequest = 0;
            cmd_hdr->event = H4_TYPE_COMMAND;
            libusb_fill_control_transfer(xmit_transfer, usb.handle,
                (uint8_t *)&cmd_hdr->setup, xmit_xfer_cb, NULL, 0);
            break;

        case H4_TYPE_ACL_DATA:
            libusb_fill_bulk_transfer(xmit_transfer, usb.handle,
                BT_BULK_OUT, (p_data+1), (len-1), xmit_xfer_cb, NULL, 0);
            break;

        case H4_TYPE_SCO_DATA:
            libusb_fill_iso_transfer(xmit_transfer, usb.handle, \
                  BT_ISO_OUT, (p_data+1), (len-1), BT_MAX_ISO_FRAMES, \
                  xmit_xfer_cb, NULL, 0);
            break;

         default:
            USBERR("Unknown packet type to transmit %x", *p_data);
            return -1;
    }

    xmited_len = len;
    usb_xfer_status &= ~(XMITTED);

    if (libusb_submit_transfer(xmit_transfer) < 0)
    {
        USBERR("libusb_submit_transfer : %d : failed", *p_data);
        return -1;
    }

    while (!(usb_xfer_status & XMITTED)){
        USBDBG("call libusb_handle_events_timeout");
        libusb_handle_events_timeout(0, &tv);
        USBDBG("return from libusb_handle_events_timeout");
    }
    return (xmited_len);
}

/*******************************************************************************
**
** Function        usb_close
**
** Description     Close the serial port
**
** Returns         None
**
*******************************************************************************/
void usb_close(void)
{
    int result;
    #if !defined (USB_RX_PIPE)
    TRANSAC p_buf;
    #endif
    
    USBDBG("usb_close \n");

    usb_xfer_status |= RX_DEAD;

    if ((result=pthread_join(usb.read_thread, NULL)) < 0)
        ALOGE( "pthread_join() FAILED result:%d \n", result);

    if (usb.handle) {
        libusb_release_interface(usb.handle, 1);
        libusb_release_interface(usb.handle, 0);
        libusb_close(usb.handle);
    }
    usb.handle = NULL;
    libusb_exit(NULL);
    #ifdef USB_RX_PIPE
    USBDBG("free pipe %d, %d \n", usb.rx_pipe[0], usb.rx_pipe[1]);
    if(usb.rx_pipe[0] >= 0){
        close(usb.rx_pipe[0]);
        usb.rx_pipe[0] = -1;
    }
    if(usb.rx_pipe[1] >= 0){
        close(usb.rx_pipe[1]);
        usb.rx_pipe[1] = -1;
    }
    USBDBG("free buffer int=%p, bulk=%p\n", usb.p_int_buf, usb.p_bulk_buf);
    if(usb.p_int_buf){
       USB_FREE(usb.p_int_buf);
       usb.p_int_buf = 0;
    }
    if(usb.p_bulk_buf){
       USB_FREE(usb.p_bulk_buf);
       usb.p_bulk_buf = 0;
    }
    #else
    if (bt_hc_cbacks)
    {
        while ((p_buf = utils_dequeue (&(usb.rx_eventq))) != NULL)
        {
            bt_hc_cbacks->dealloc(p_buf, (char *) ((RX_HDR *)p_buf+1));
        }
        while ((p_buf = utils_dequeue (&(usb.rx_isoq))) != NULL)
        {
            bt_hc_cbacks->dealloc(p_buf, (char *) ((RX_HDR *)p_buf+1));
        }
        while ((p_buf = utils_dequeue (&(usb.rx_bulkq))) != NULL)
        {
            bt_hc_cbacks->dealloc(p_buf, (char *) ((RX_HDR *)p_buf+1));
        }
    }
    #endif
}

/*******************************************************************************
**
** Function        usb_ioctl
**
** Description     ioctl inteface
**
** Returns         None
**
*******************************************************************************/
void usb_ioctl(usb_ioctl_op_t op, void *p_data)
{
    return;
}

