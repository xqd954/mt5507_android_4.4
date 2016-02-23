#if 0
/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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


/*****************************************************************************
 *
 *  Filename:      btif_rc.c
 *
 *  Description:   Bluetooth AVRC implementation
 *
 *****************************************************************************/
#include <hardware/bluetooth.h>
#include "hardware/bt_av.h"
#include "hardware/bt_rc.h"
#include <fcntl.h>
#include "bta_api.h"
#include "bta_av_api.h"
#include "avrc_defs.h"
#include "bd.h"
#include "gki.h"

#define LOG_TAG "BTIF_RC"
#include "btif_common.h"


/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTIF_RC_USE_UINPUT TRUE
#include "uinput.h"

/* cod value for Headsets */
#define COD_AV_HEADSETS        0x0404


/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    BOOLEAN                     rc_connected;
    UINT8                       rc_handle;
    BD_ADDR                     rc_addr;
    UINT16                      rc_pending_play;
} btif_rc_cb_t;

#ifdef BTIF_RC_USE_UINPUT
#define MAX_UINPUT_PATHS 3
static const char* uinput_dev_path[] =
                       {"/dev/uinput", "/dev/input/uinput", "/dev/misc/uinput" };
static int uinput_fd = -1;

static int  send_event (int fd, uint16_t type, uint16_t code, int32_t value);
static void send_key (int fd, uint16_t key, int pressed);
static int  uinput_driver_check();
static int  uinput_create(char *name);
static int  init_uinput (void);
static void close_uinput (void);

static struct {
    const char *name;
    uint8_t avrcp;
    uint16_t mapped_id;
    uint8_t release_quirk;
} key_map[] = {
    { "PLAY",         AVRC_ID_PLAY,     KEY_PLAYCD,       1 },
    { "STOP",         AVRC_ID_STOP,     KEY_STOPCD,       0 },
    { "PAUSE",        AVRC_ID_PAUSE,    KEY_PAUSECD,      1 },
    { "FORWARD",      AVRC_ID_FORWARD,  KEY_NEXTSONG,     0 },
    { "BACKWARD",     AVRC_ID_BACKWARD, KEY_PREVIOUSSONG, 0 },
    { "REWIND",       AVRC_ID_REWIND,   KEY_REWIND,       0 },
    { "FAST FORWARD", AVRC_ID_FAST_FOR, KEY_FORWARD,      0 },
    { NULL,           0,                0,                0 }
};
#endif /* BTIF_RC_USE_UINPUT */


/*****************************************************************************
**  Static variables
******************************************************************************/
static btif_rc_cb_t btif_rc_cb;

/*****************************************************************************
**  Static functions
******************************************************************************/

/*****************************************************************************
**  Externs
******************************************************************************/
extern BOOLEAN btif_hf_call_terminated_recently();
extern BOOLEAN check_cod(const bt_bdaddr_t *remote_bdaddr, uint32_t cod);
extern BOOLEAN btif_av_is_connected(void);

/*****************************************************************************
**  Functions
******************************************************************************/


#ifdef BTIF_RC_USE_UINPUT
/*****************************************************************************
**   Local uinput helper functions
******************************************************************************/
int send_event (int fd, uint16_t type, uint16_t code, int32_t value)
{
    struct uinput_event event;

    memset(&event, 0, sizeof(event));
    event.type  = type;
    event.code  = code;
    event.value = value;

    return write(fd, &event, sizeof(event));
}

void send_key (int fd, uint16_t key, int pressed)
{
    if (fd < 0) {
        return;
    }

    BTIF_TRACE_DEBUG3("AVRCP: Send key %d (%d) fd=%d", key, pressed, fd);
    send_event(fd, EV_KEY, key, pressed);
    send_event(fd, EV_SYN, SYN_REPORT, 0);
}

/************** uinput related functions **************/
int uinput_driver_check()
 {
    uint32_t i;
    for (i=0; i < MAX_UINPUT_PATHS; i++)
    {
        if (access(uinput_dev_path[i], O_RDWR) == 0) {
           return 0;
        }
    }
    BTIF_TRACE_ERROR1("%s ERROR: uinput device is not in the system", __FUNCTION__);
    return -1;
}

int uinput_create(char *name)
{
    struct uinput_dev dev;
    int fd, err, x = 0;

    for(x=0; x < MAX_UINPUT_PATHS; x++)
    {
        fd = open(uinput_dev_path[x], O_RDWR);
        if (fd < 0)
            continue;
        break;
    }
    if (x == MAX_UINPUT_PATHS) {
        BTIF_TRACE_ERROR1("%s ERROR: uinput device open failed", __FUNCTION__);
        return -1;
    }
    memset(&dev, 0, sizeof(dev));
    if (name)
        strncpy(dev.name, name, UINPUT_MAX_NAME_SIZE);

    dev.id.bustype = BUS_BLUETOOTH;
    dev.id.vendor  = 0x0000;
    dev.id.product = 0x0000;
    dev.id.version = 0x0000;

    if (write(fd, &dev, sizeof(dev)) < 0) {
        BTIF_TRACE_ERROR1("%s Unable to write device information", __FUNCTION__);
        close(fd);
        return -1;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    for (x = 0; key_map[x].name != NULL; x++)
        ioctl(fd, UI_SET_KEYBIT, key_map[x].mapped_id);

    for(x = 0; x < KEY_MAX; x++)
        ioctl(fd, UI_SET_KEYBIT, x);

    if (ioctl(fd, UI_DEV_CREATE, NULL) < 0) {
        BTIF_TRACE_ERROR1("%s Unable to create uinput device", __FUNCTION__);
        close(fd);
        return -1;
    }
    return fd;
}

int init_uinput (void)
{
    char *name = "AVRCP";

    BTIF_TRACE_DEBUG1("%s", __FUNCTION__);
    uinput_fd = uinput_create(name);
    if (uinput_fd < 0) {
        BTIF_TRACE_ERROR3("%s AVRCP: Failed to initialize uinput for %s (%d)",
                          __FUNCTION__, name, uinput_fd);
    } else {
        BTIF_TRACE_DEBUG3("%s AVRCP: Initialized uinput for %s (fd=%d)",
                          __FUNCTION__, name, uinput_fd);
    }
    return uinput_fd;
}

void close_uinput (void)
{
    BTIF_TRACE_DEBUG1("%s", __FUNCTION__);
    if (uinput_fd > 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);

        close(uinput_fd);
        uinput_fd = -1;
    }
}
#endif // BTA_AVRCP_FORCE_USE_UINPUT

const char *dump_rc_event_name(tBTA_AV_EVT event)
{
   switch(event) {
        case BTA_AV_RC_OPEN_EVT: return "BTA_AV_RC_OPEN_EVT";
        case BTA_AV_RC_CLOSE_EVT: return "BTA_AV_RC_CLOSE_EVT";
        case BTA_AV_REMOTE_CMD_EVT: return "BTA_AV_REMOTE_CMD_EVT";
        case BTA_AV_REMOTE_RSP_EVT: return "BTA_AV_REMOTE_RSP_EVT";
        case BTA_AV_VENDOR_CMD_EVT: return "BTA_AV_VENDOR_CMD_EVT";
        case BTA_AV_VENDOR_RSP_EVT: return "BTA_AV_VENDOR_RSP_EVT";
        default: return "UNKNOWN_EVENT";
   }
}

/***************************************************************************
 *  Function       handle_rc_connect
 *
 *  - Argument:    tBTA_AV_RC_OPEN 	RC open data structure
 *
 *  - Description: RC connection event handler
 *
 ***************************************************************************/
void handle_rc_connect (tBTA_AV_RC_OPEN *p_rc_open)
{
    BTIF_TRACE_DEBUG2("%s: rc_handle: %d", __FUNCTION__, p_rc_open->rc_handle);

#ifdef BTIF_RC_USE_UINPUT
    init_uinput();
#endif

    memcpy(btif_rc_cb.rc_addr, p_rc_open->peer_addr, sizeof(BD_ADDR));
    btif_rc_cb.rc_connected = TRUE;
    btif_rc_cb.rc_handle = p_rc_open->rc_handle;
}

/***************************************************************************
 *  Function       handle_rc_disconnect
 *
 *  - Argument:    tBTA_AV_RC_CLOSE 	RC close data structure
 *
 *  - Description: RC disconnection event handler
 *
 ***************************************************************************/
void handle_rc_disconnect (tBTA_AV_RC_CLOSE *p_rc_close)
{
    BTIF_TRACE_DEBUG2("%s: rc_handle: %d", __FUNCTION__, p_rc_close->rc_handle);

    btif_rc_cb.rc_handle = 0;
    btif_rc_cb.rc_connected = FALSE;
    memset(btif_rc_cb.rc_addr, 0, sizeof(BD_ADDR));
#ifdef BTIF_RC_USE_UINPUT
    close_uinput();
#endif /* BTIF_RC_USE_UINPUT */
}

/***************************************************************************
 *  Function       handle_rc_passthrough_cmd
 *
 *  - Argument:    tBTA_AV_RC rc_id   remote control command ID
 *                 tBTA_AV_STATE key_state status of key press
 *
 *  - Description: Remote control command handler
 *
 ***************************************************************************/
void handle_rc_passthrough_cmd ( tBTA_AV_REMOTE_CMD *p_remote_cmd)
{
    const char *status;
    int pressed, i;

    btif_rc_cb.rc_handle = p_remote_cmd->rc_handle;

    /* If AVRC is open and peer sends PLAY but there is no AVDT, then we queue-up this PLAY */
    if (p_remote_cmd)
    {
        /* queue AVRC PLAY if GAVDTP Open notification to app is pending (2 second timer) */
        if ((p_remote_cmd->rc_id == BTA_AV_RC_PLAY) && (!btif_av_is_connected()))
        {
            if (p_remote_cmd->key_state == AVRC_STATE_PRESS)
            {
                APPL_TRACE_WARNING1("%s: AVDT not open, queuing the PLAY command", __FUNCTION__);
                btif_rc_cb.rc_pending_play = TRUE;
            }
            return;
        }

        if ((p_remote_cmd->rc_id == BTA_AV_RC_PAUSE) && (btif_rc_cb.rc_pending_play))
        {
            APPL_TRACE_WARNING1("%s: Clear the pending PLAY on PAUSE received", __FUNCTION__);
            btif_rc_cb.rc_pending_play = FALSE;
            return;
        }
    }
    if (p_remote_cmd->key_state == AVRC_STATE_RELEASE) {
        status = "released";
        pressed = 0;
    } else {
        status = "pressed";
        pressed = 1;
    }

    /* If this is Play/Pause command (press or release)  before processing, check the following
     * a voice call has ended recently
     * the remote device is not of type headset
     * If the above conditions meet, drop the Play/Pause command
     * This fix is to interop with certain carkits which sends an automatic  PLAY  or PAUSE
     * commands right after call ends
     */
    if((p_remote_cmd->rc_id == BTA_AV_RC_PLAY || p_remote_cmd->rc_id == BTA_AV_RC_PAUSE)&&
       (btif_hf_call_terminated_recently() == TRUE) &&
       (check_cod( (const bt_bdaddr_t*)&(btif_rc_cb.rc_addr), COD_AV_HEADSETS) != TRUE))
    {
        BTIF_TRACE_DEBUG2("%s:Dropping the play/Pause command received right after call end cmd:%d",
                           __FUNCTION__,p_remote_cmd->rc_id);
        return;
    }

    for (i = 0; key_map[i].name != NULL; i++) {
        if (p_remote_cmd->rc_id == key_map[i].avrcp) {
            BTIF_TRACE_DEBUG3("%s: %s %s", __FUNCTION__, key_map[i].name, status);

           /* MusicPlayer uses a long_press_timeout of 1 second for PLAYPAUSE button
            * and maps that to autoshuffle. So if for some reason release for PLAY/PAUSE
            * comes 1 second after the press, the MediaPlayer UI goes into a bad state.
            * The reason for the delay could be sniff mode exit or some AVDTP procedure etc.
            * The fix is to generate a release right after the press and drown the 'actual'
            * release.
            */
            if ((key_map[i].release_quirk == 1) && (pressed == 0))
            {
                BTIF_TRACE_DEBUG2("%s: AVRC %s Release Faked earlier, drowned now",
                                  __FUNCTION__, key_map[i].name);
                return;
            }
#ifdef BTIF_RC_USE_UINPUT
            send_key(uinput_fd, key_map[i].mapped_id, pressed);
#endif
            if ((key_map[i].release_quirk == 1) && (pressed == 1))
            {
                GKI_delay(30); // 30ms
                BTIF_TRACE_DEBUG2("%s: AVRC %s Release quirk enabled, send release now",
                                  __FUNCTION__, key_map[i].name);
#ifdef BTIF_RC_USE_UINPUT
                send_key(uinput_fd, key_map[i].mapped_id, 0);
#endif
            }
            break;
        }
    }

    if (key_map[i].name == NULL)
        BTIF_TRACE_ERROR3("%s AVRCP: unknown button 0x%02X %s", __FUNCTION__,
                        p_remote_cmd->rc_id, status);
}

/*****************************************************************************
**
** Function        btif_rc_init
**
** Description     Initialize RC
**
** Returns         Returns 0 on success, -1 otherwise
**
*******************************************************************************/
int btif_rc_init()
{
    BTIF_TRACE_DEBUG1("%s", __FUNCTION__);
    memset (&btif_rc_cb, 0, sizeof(btif_rc_cb));

#ifdef BTIF_RC_USE_UINPUT
    return uinput_driver_check();
#endif /* BTIF_RC_USE_UINPUT */
}

/***************************************************************************
 **
 ** Function       btif_rc_handler
 **
 ** Description    RC event handler
 **
 ***************************************************************************/
void btif_rc_handler(tBTA_AV_EVT event, tBTA_AV *p_data)
{
    BTIF_TRACE_DEBUG2 ("%s event:%s", __FUNCTION__, dump_rc_event_name(event));
    switch (event)
    {
        case BTA_AV_RC_OPEN_EVT:
        {
            BTIF_TRACE_DEBUG1("Peer_features:%x", p_data->rc_open.peer_features);
            handle_rc_connect( &(p_data->rc_open) );
        }break;

        case BTA_AV_RC_CLOSE_EVT:
        {
            handle_rc_disconnect( &(p_data->rc_close) );
        }break;

        case BTA_AV_REMOTE_CMD_EVT:
        {
            BTIF_TRACE_DEBUG2("rc_id:0x%x key_state:%d", p_data->remote_cmd.rc_id,
                               p_data->remote_cmd.key_state);
            handle_rc_passthrough_cmd( (&p_data->remote_cmd) );
        }break;
        default:
            BTIF_TRACE_DEBUG0("Unhandled RC event");
    }
}

/***************************************************************************
 **
 ** Function       btif_rc_get_connected_peer
 **
 ** Description    Fetches the connected headset's BD_ADDR if any
 **
 ***************************************************************************/
BOOLEAN btif_rc_get_connected_peer(BD_ADDR peer_addr)
{
    if (btif_rc_cb.rc_connected == TRUE) {
        bdcpy(peer_addr, btif_rc_cb.rc_addr);
        return TRUE;
    }
    return FALSE;
}

/***************************************************************************
 **
 ** Function       btif_rc_check_handle_pending_play
 **
 ** Description    Clears the queued PLAY command. if bSend is TRUE, forwards to app
 **
 ***************************************************************************/

/* clear the queued PLAY command. if bSend is TRUE, forward to app */
void btif_rc_check_handle_pending_play (BD_ADDR peer_addr, BOOLEAN bSendToApp)
{
    ALOGV("btapp_rc_check_handle_pending_play: bSendToApp=%d", bSendToApp);
    if (btif_rc_cb.rc_pending_play)
    {
        if (bSendToApp)
        {
            tBTA_AV_REMOTE_CMD remote_cmd;
            APPL_TRACE_DEBUG1("%s: Sending queued PLAYED event to app", __FUNCTION__);

            memset (&remote_cmd, 0, sizeof(tBTA_AV_REMOTE_CMD));
            remote_cmd.rc_handle  = btif_rc_cb.rc_handle;
            remote_cmd.rc_id      = AVRC_ID_PLAY;
            remote_cmd.hdr.ctype  = AVRC_CMD_CTRL;
            remote_cmd.hdr.opcode = AVRC_OP_PASS_THRU;

            /* delay sending to app, else there is a timing issue in the framework,
             ** which causes the audio to be on th device's speaker. Delay between
             ** OPEN & RC_PLAYs
            */
            GKI_delay (200);
            /* send to app - both PRESSED & RELEASED */
            remote_cmd.key_state  = AVRC_STATE_PRESS;
            handle_rc_passthrough_cmd( &remote_cmd );

            GKI_delay (100);

            remote_cmd.key_state  = AVRC_STATE_RELEASE;
            handle_rc_passthrough_cmd( &remote_cmd );
        }
        btif_rc_cb.rc_pending_play = FALSE;
    }
}

#endif
///////////////////////////////////////////////////////////////////////////////

#include <hardware/bluetooth.h>
#include "hardware/bt_av.h"
#include "hardware/bt_rc.h"
#include "bt_ext_debug.h"

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_avrcp_struct.h"
#include "bt_avrcp_api.h"
#include "bt_ext_debug.h"

#include "bluetooth_struct.h"

// extern function
U8 getSavedSeqId(int msg_id);
U8 getSavedRegEventSeqId(U8 event_id);

extern S32 g_avrcp_internal_get_playerstatus;
extern U8 g_avrcp_player_play_status;
bt_status_t set_volume(uint8_t volume);
U8 g_avrcp_remote_volume = 0;
U16 g_avrcp_rc_version = 0;

U32 g_avrcp_fake_uid_upper = 0;
U32 g_avrcp_fake_uid_lower = 1;

/*****************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
    int                     rc_connected;
    int                       rc_handle;
    char                     rc_addr[6];
    int                      rc_pending_play;
} btif_rc_cb_t;

/*****************************************************************************
**  Static variables
******************************************************************************/
static btif_rc_cb_t btif_rc_cb;
static btrc_callbacks_t* bt_rc_callbacks = 0;

/*****************************************************************************
**  Static functions
******************************************************************************/

/***************************************************************************
**
** Function         get_play_status_rsp
**
** Description      Returns the current play status.
**                      This method is called in response to
**                      GetPlayStatus request.
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t get_play_status_rsp(btrc_play_status_t play_status, uint32_t song_len,
    uint32_t song_pos)
{
	  bt_ext_log("rc: get_play_status_rsp play_status:%d song_len:%d song_pos:%d", play_status, song_len, song_pos);
#if 0	
    tAVRC_RESPONSE avrc_rsp;
    UINT32 i;
    CHECK_RC_CONNECTED
    memset(&(avrc_rsp.get_play_status), 0, sizeof(tAVRC_GET_PLAY_STATUS_RSP));
    avrc_rsp.get_play_status.song_len = song_len;
    avrc_rsp.get_play_status.song_pos = song_pos;
    avrc_rsp.get_play_status.play_status = play_status;

    avrc_rsp.get_play_status.pdu = AVRC_PDU_GET_PLAY_STATUS;
    avrc_rsp.get_play_status.opcode = opcode_from_pdu(AVRC_PDU_GET_PLAY_STATUS);
    avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;
    /* Send the response */
    SEND_METAMSG_RSP(IDX_GET_PLAY_STATUS_RSP, &avrc_rsp);
#else

    // update the status
    g_avrcp_player_play_status = play_status;
    if( BTRC_PLAYSTATE_PAUSED == g_avrcp_player_play_status && song_pos > song_len ){
        g_avrcp_player_play_status = BTRC_PLAYSTATE_STOPPED;
    }
    // song_pos & song_len maybe -1 (0xffffffff) as unsupported in spec.
    
    if( g_avrcp_internal_get_playerstatus > 0){
        g_avrcp_internal_get_playerstatus--;
        bt_ext_log("rc: get_play_status_rsp updateinternal play_stauts:%d count:%d", g_avrcp_player_play_status, g_avrcp_internal_get_playerstatus);
    }else{
	    //void btmtk_avrcp_send_get_playstatus_rsp(AvrcpChannelContext *cntx, U8 error, U32 song_length, U32 song_position, U8 status){    
	    btmtk_avrcp_send_get_playstatus_rsp(0, 0, song_len, song_pos, play_status);
	}
#endif    
    return BT_STATUS_SUCCESS;
}

/***************************************************************************
**
** Function         get_element_attr_rsp
**
** Description      Returns the current songs' element attributes
**                      in text.
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t get_element_attr_rsp(uint8_t num_attr, btrc_element_attr_val_t *p_attrs)
{
	
#if 0	
    tAVRC_RESPONSE avrc_rsp;
    UINT32 i;
    uint8_t j;
    tAVRC_ATTR_ENTRY element_attrs[BTRC_MAX_ELEM_ATTR_SIZE];
    CHECK_RC_CONNECTED
    memset(element_attrs, 0, sizeof(tAVRC_ATTR_ENTRY) * num_attr);

    if (num_attr == 0)
    {
        avrc_rsp.get_play_status.status = AVRC_STS_BAD_PARAM;
    }
    else
    {
        for (i=0; i<num_attr; i++) {
            element_attrs[i].attr_id = p_attrs[i].attr_id;
            element_attrs[i].name.charset_id = AVRC_CHARSET_ID_UTF8;
            element_attrs[i].name.str_len = (UINT16)strlen((char *)p_attrs[i].text);
            element_attrs[i].name.p_str = p_attrs[i].text;
            BTIF_TRACE_DEBUG5("%s attr_id:0x%x, charset_id:0x%x, str_len:%d, str:%s",
                __FUNCTION__, (unsigned int)element_attrs[i].attr_id,
                element_attrs[i].name.charset_id, element_attrs[i].name.str_len,
                element_attrs[i].name.p_str);
        }
        avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;
    }
    avrc_rsp.get_elem_attrs.num_attr = num_attr;
    avrc_rsp.get_elem_attrs.p_attrs = element_attrs;
    avrc_rsp.get_elem_attrs.pdu = AVRC_PDU_GET_ELEMENT_ATTR;
    avrc_rsp.get_elem_attrs.opcode = opcode_from_pdu(AVRC_PDU_GET_ELEMENT_ATTR);
    /* Send the response */
    SEND_METAMSG_RSP(IDX_GET_ELEMENT_ATTR_RSP, &avrc_rsp);
#else

    // void btmtk_avrcp_send_get_element_attributes_rsp(AvrcpChannelContext *cntx, U8 error, U8 index, U8 total, U8 attr_id, U16 charset, U16 strlen, U8 *strText)    
    AvrcpChannelContext avrcpCntx;
    AvrcpChannelContext *cntx;
    btrc_element_attr_val_t emptyAttr;
    
    int i = 0;
    char c = 0x20;

    cntx = &avrcpCntx;
    
    bt_ext_log("rc: get_element_attr_rsp num_attr:%d", num_attr);
    cntx->seq_id = getSavedSeqId(MSG_ID_BT_AVRCP_GET_ELEMENT_ATTRIBUTES_IND);

    if( 0 == num_attr ){
        btmtk_avrcp_send_get_element_attributes_rsp( &avrcpCntx, 2, 0, 0, 0, 0, 0, (unsigned char *)0); // 2 is bad parameter
        //U8 error, U8 index, U8 total, U8 attr_id, U16 charset, U16 strlen, U8 *strText);
    }else{

        // only return attribute that has values  PTS TC_TG_MDI_BV_04_C
#if 0        
        if( num_attr > 3 ){ 
            for( i = 3; i< num_attr; i++){
                 if( p_attrs[i].text[0] == 0 ){ // 
                      num_attr = i;
                      bt_ext_log("rc: [AVRCP] error change to only %d item support because p_attrs[i] is null", i);   
                      break;
                 }else{
                      bt_ext_log("rc: [AVRCP] attrs[3].text[0] is 0x%x ", p_attrs[3].text[0]);   
                 }
            }
        }
#else
        // Avoid to return 0 attribute-count. PCM carkit will keep asking when number is 0
        if( num_attr == 0 ){
            // at least return id 0
            num_attr = 3;
            for( i = 0; i< num_attr; i++){
                emptyAttr.attr_id = i+1;
                emptyAttr.text[0] = ' ';
                emptyAttr.text[1] = 0;
                bt_ext_log("rc: [AVRCP] create emtpyAttr i%d %s", emptyAttr.attr_id, emptyAttr.text);
            }
            p_attrs = &emptyAttr;
        }
        // Avoid to return 0 attribute length
        for( i = 0; i< num_attr; i++){
            if( p_attrs[i].text[0] == 0 ){ // 
                p_attrs[i].text[0] = ' ';
                p_attrs[i].text[1] = 0;
            }
            bt_ext_log("rc: [AVRCP] change 0 attribute length to 1 space ");
        }
#endif        
    
        for( i = 0; i< num_attr; i++){ // 0x6a utf8
            if( 0 != p_attrs[i].text ){
                c = p_attrs[i].text[0];
            }
            bt_ext_log("rc: [AVRCP] get_element_attr_rsp i:%d attr_id:%d strlen:%d c:%d", i, p_attrs[i].attr_id, (unsigned short)strlen((char *)p_attrs[i].text), c);
            btmtk_avrcp_send_get_element_attributes_rsp( cntx, 0, i, num_attr, p_attrs[i].attr_id, 0x6a, (unsigned short)strlen((char *)p_attrs[i].text), (unsigned char *)p_attrs[i].text);
        }
    }

#endif
    return BT_STATUS_SUCCESS;
}

/***************************************************************************
**
** Function         register_notification_rsp
**
** Description      Response to the register notification request.
**                      in text.
**
** Returns          bt_status_t
**
***************************************************************************/
static bt_status_t register_notification_rsp(btrc_event_id_t event_id,
    btrc_notification_type_t type, btrc_register_notification_t *p_param)
{

#if 0	
    tAVRC_RESPONSE avrc_rsp;
    CHECK_RC_CONNECTED
    BTIF_TRACE_EVENT2("## %s ## event_id:%s", __FUNCTION__, dump_rc_notification_event_id(event_id));
    memset(&(avrc_rsp.reg_notif), 0, sizeof(tAVRC_REG_NOTIF_RSP));
    avrc_rsp.reg_notif.event_id = event_id;

    switch(event_id)
    {
        case BTRC_EVT_PLAY_STATUS_CHANGED:
            avrc_rsp.reg_notif.param.play_status = p_param->play_status;
            break;
        case BTRC_EVT_TRACK_CHANGE:
            memcpy(&(avrc_rsp.reg_notif.param.track), &(p_param->track), sizeof(btrc_uid_t));
            break;
        case BTRC_EVT_PLAY_POS_CHANGED:
            avrc_rsp.reg_notif.param.play_pos = p_param->song_pos;
            break;
        default:
            BTIF_TRACE_WARNING2("%s : Unhandled event ID : 0x%x", __FUNCTION__, event_id);
            return BT_STATUS_UNHANDLED;
    }

    avrc_rsp.reg_notif.pdu = AVRC_PDU_REGISTER_NOTIFICATION;
    avrc_rsp.reg_notif.opcode = opcode_from_pdu(AVRC_PDU_REGISTER_NOTIFICATION);
    avrc_rsp.get_play_status.status = AVRC_STS_NO_ERROR;

    /* Send the response. */
    send_metamsg_rsp(btif_rc_cb.rc_handle, btif_rc_cb.rc_notif[event_id-1].label,
        ((type == BTRC_NOTIFICATION_TYPE_INTERIM)?AVRC_CMD_NOTIF:AVRC_RSP_CHANGED), &avrc_rsp);
#else

    AvrcpChannelContext avrcpCntx;
    AvrcpChannelContext *cntx = &avrcpCntx;
    U8 interim = 1;
    U8 allff[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    U8 fakeid[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    fakeid[7] = g_avrcp_fake_uid_lower++; //TC_TG_NFY_BV_02_C
    
    cntx->seq_id = getSavedRegEventSeqId(event_id);
    interim = ((type == BTRC_NOTIFICATION_TYPE_INTERIM)?1:0); //type == BTRC_NOTIFICATION_TYPE_INTERIM
        
    bt_ext_log( "rc: register_notification_rsp event_id:%d interim:%d playstatus:%d", event_id, interim, p_param->play_status );
    
    switch(event_id)
    {
        case BTRC_EVT_PLAY_STATUS_CHANGED:
            //avrc_rsp.reg_notif.param.play_status = p_param->play_status;
            //void btmtk_avrcp_send_reg_notievent_playback_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 status){
            btmtk_avrcp_send_reg_notievent_playback_rsp(cntx, 0, interim, p_param->play_status);
            break;
        case BTRC_EVT_TRACK_CHANGE:
            //memcpy(&(avrc_rsp.reg_notif.param.track), &(p_param->track), sizeof(btrc_uid_t));
            bt_ext_log( "rc: [AVRCP] track-change (g_play_status:%d param->play_status:%d)", g_avrcp_player_play_status, p_param->play_status);
            bt_ext_log( "rc: [AVRCP] id from ap lower [0]:0x%x 0x%x 0x%x 0x%x", p_param->track[0], p_param->track[1],p_param->track[2],p_param->track[3]);
            bt_ext_log( "rc: [AVRCP] id from ap upper [4]:0x%x 0x%x 0x%x 0x%x", p_param->track[0], p_param->track[1],p_param->track[2],p_param->track[3]);
            //if( g_avrcp_player_play_status == BTRC_PLAYSTATE_STOPPED ){
            //    btmtk_avrcp_send_reg_notievent_track_changed_rsp( cntx, 0, interim, allff);
            //}else{
                btmtk_avrcp_send_reg_notievent_track_changed_rsp( cntx, 0, interim, &(p_param->track));
            //}
            break;
        case BTRC_EVT_PLAY_POS_CHANGED:
            //avrc_rsp.reg_notif.param.play_pos = p_param->song_pos;
            // void btmtk_avrcp_send_reg_notievent_pos_changed_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U32 position){
            btmtk_avrcp_send_reg_notievent_pos_changed_rsp( cntx, 0, interim, p_param->song_pos);
            break;
        default:
            //BTIF_TRACE_WARNING2("%s : Unhandled event ID : 0x%x", __FUNCTION__, event_id);
            //void btmtk_avrcp_send_reg_notievent_playback_rsp(AvrcpChannelContext *cntx, U8 error, U8 interim, U8 status);
            btmtk_avrcp_send_reg_notievent_playback_rsp( cntx, 4, interim, 4);
            return BT_STATUS_UNHANDLED;
    }   

#endif        	
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         init
**
** Description      Initializes the AVRC interface
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t init(btrc_callbacks_t* callbacks )
{
    //BTIF_TRACE_EVENT1("## %s ##", __FUNCTION__);
    bt_ext_log("rc: init");
    bt_status_t result = BT_STATUS_SUCCESS;

    if (bt_rc_callbacks)
        return BT_STATUS_DONE;

    bt_rc_callbacks = callbacks;
    memset (&btif_rc_cb, 0, sizeof(btif_rc_cb));

    return result;
}

/***************************************************************************
**
** Function         cleanup
**
** Description      Closes the AVRC interface
**
** Returns          void
**
***************************************************************************/
static void cleanup()
{
	bt_ext_log("rc: cleanup");
#if 0	
    BTIF_TRACE_EVENT1("## %s ##", __FUNCTION__);

    close_uinput();
#endif    
    if (bt_rc_callbacks)
    {
        bt_rc_callbacks = NULL;
    }
    memset(&btif_rc_cb, 0, sizeof(btif_rc_cb_t));
    
}

static const btrc_interface_t bt_rc_interface = {
    sizeof(bt_rc_interface),
    init,
    get_play_status_rsp,
    NULL, /* list_player_app_attr_rsp */
    NULL, /* list_player_app_value_rsp */
    NULL, /* get_player_app_value_rsp */
    NULL, /* get_player_app_attr_text_rsp */
    NULL, /* get_player_app_value_text_rsp */
    get_element_attr_rsp,
    NULL, /* set_player_app_value_rsp */
    register_notification_rsp,
    set_volume,
    cleanup,
};

/*******************************************************************************
**
** Function         btif_rc_get_interface
**
** Description      Get the AVRCP callback interface
**
** Returns          btav_interface_t
**
*******************************************************************************/
const btrc_interface_t *btif_rc_get_interface(void)
{
    //BTIF_TRACE_EVENT1("%s", __FUNCTION__);
	  bt_ext_log("rc: btif_rc_get_interface %s", __FUNCTION__);
    return &bt_rc_interface;
}

//

int btrc_handle_get_playerstatus_ind(){
    if( 0 != bt_rc_callbacks && 0 != bt_rc_callbacks->get_play_status_cb){
        bt_rc_callbacks->get_play_status_cb();
        return 0;
    }
    return 1;
}

int btrc_handle_register_notification_ind(btrc_event_id_t event_id, uint32_t param){
    // PCM carkit: register all events without asking the supported event
    switch(event_id){
    case 0x01: // AVRCP_EVENT_PLAYBACK_STATUS_CHANGED
    case 0x02: // AVRCP_EVENT_TRACK_CHANGED
    case 0x05: // AVRCP_EVENT_PLAYBACK_POS_CHANGED
    if( 0 != bt_rc_callbacks && 0 != bt_rc_callbacks->register_notification_cb){
        bt_rc_callbacks->register_notification_cb( event_id, param);
        return 0;
    }
        break;
    default:
        bt_ext_log("rc: event_id:%d not support.", event_id); 
        return 1;
        break;
    }
    return 1;
}

int btrc_handle_get_element_attributes(uint8_t num_attr, uint32_t *p_attrs){
    btrc_media_attr_t attrArray[7];
    int count = 0;
    if( 0 != p_attrs ){
        for(count = 0; count< num_attr && count< (sizeof(attrArray)/sizeof(btrc_media_attr_t)); count++) {
            attrArray[count] = p_attrs[count];
        }
    }
    
    if( 0 != bt_rc_callbacks && 0 != bt_rc_callbacks->get_element_attr_cb ){
        bt_rc_callbacks->get_element_attr_cb( num_attr, attrArray); //uint8_t num_attr, btrc_media_attr_t *p_attrs);
        return 0;
    }    
    return 1;
}

/**
* @brief Pass remote CT role's address and sdp feature to java layer
*/
int btrc_handle_rc_features(uint8_t *addr, uint16_t rc_version, uint16_t rc_features){
	//bt_rc_callbacks, remote_features_cb, &rc_addr, rc_features
    AvrcpChannelContext *cntx = 0;
    U8 btrc_feature = 0;
    // rc_feature is the sdp record of remote device (see spec. category + browse bit)    
    // btrc_feature is BTRC_FEAT_NONE, BTRC_FEAT_METADATA, BTRC_FEAT_ABSOLUTE_VOLUME, BTRC_FEAT_BROWSE

        
	bt_ext_log("[BT][AVRCP] handle rc_feature version:0x%x feature:0x%x", rc_version, rc_features);
    if( rc_version > 0 ){
        g_avrcp_rc_version = rc_version;
        switch( g_avrcp_rc_version ){
            case 0x0104:
            case 0x0105:
            case 0x0106:
                btrc_feature |= BTRC_FEAT_METADATA;
                // move to set-volumn btmtk_avrcp_send_reg_notievent_volume_changed_req( 0);
            break;
            case 0x0:
            case 0x0103:
                btrc_feature |= BTRC_FEAT_METADATA;
                break;
            default:
                bt_ext_log("[BT][AVRCP] no handle rc_version:0x%x", rc_version);
                break;
        }
    }

    // update btrc_feature 
    if( g_avrcp_rc_version >= 0x0103 && (rc_features&0x02) ){
        // it is category2 device and it support set-volume 
        btrc_feature |= BTRC_FEAT_ABSOLUTE_VOLUME;
        bt_ext_log("[BT][AVRCP] support set-volume");
        
        btmtk_avrcp_send_reg_notievent_volume_changed_req( 0); // remote support absolute-volue
    }
    if( g_avrcp_rc_version >= 0x0103 && (rc_features&(0x1 << 6)) ){
        btrc_feature |= BTRC_FEAT_BROWSE;
        bt_ext_log("[BT][AVRCP] support browsing");
    }

    bt_ext_log("[BT][AVRCP] btrc_feature:0x%x", btrc_feature);
    if( 0 != bt_rc_callbacks && 0 != bt_rc_callbacks->remote_features_cb ){
        bt_rc_callbacks->remote_features_cb( addr, btrc_feature);
        return 0;
    }    
    return 1;	
}
/***************************************************************************
**
** Function         set_volume
**
** Description      Send current volume setting to remote side.
**                  Support limited to SetAbsoluteVolume
**                  This can be enhanced to support Relative Volume (AVRCP 1.0).
**                  With RelateVolume, we will send VOLUME_UP/VOLUME_DOWN
**                  as opposed to absolute volume level
** volume: Should be in the range 0-127. bit7 is reseved and cannot be set
** // fail: no able to send, not ready: don't know/remote don't don't support v1.4, done: volume is the same as previous
** Returns          bt_status_t
**
***************************************************************************/
bt_status_t set_volume(uint8_t volume)
{
#if 1
    bt_status_t status = BT_STATUS_FAIL;

    bt_ext_log("%s ", __FUNCTION__);

    if( g_avrcp_remote_volume == volume ){
        status=BT_STATUS_DONE;
        bt_ext_log("%s: volume value already set earlier: 0x%02x",__FUNCTION__, volume);
        return status;
    }
    
    if( g_avrcp_rc_version >= 0x0104 ){
        // has rc_feature
        if( volume >= 0 && volume <= 0x80 ){
            btmtk_avrcp_send_set_absolute_volume_req( 0, volume);
            status = BT_STATUS_SUCCESS;
        }else{
            status = BT_STATUS_FAIL;
        } 
    }else{
        // not ready
        status = BT_STATUS_NOT_READY;
    }
    bt_ext_log("%s status:%d", __FUNCTION__, status);
    return status;
#endif
}

U16 get_ctype_from_result(U16 result, U8 is_interim){
    U16 ctype = 0;
    switch( result ){
        case 0x1000:
            if( is_interim ){
                ctype = 0x0f; //AVCTP_RESPONSE_INTERIM;
            }else{
                ctype = 0x09; //AVCTP_RESPONSE_ACCEPTED;
            }
            break;
        default:
            if( result == 0x0f || result == 0x09 ){
                ctype = result;
            }else{
                ctype = 0x0a; //AVCTP_RESPONSE_REJECTED;
            }
            break;
    }
    return ctype;    
}

void handle_rc_metamsg_cmd_reg_cnf (bt_avrcp_register_notification_cnf_struct* ind){
    U16 ctype = 0;
    switch( ind->event_id ){
        case 0x0d: //AVRCP_EVENT_VOLUME_CHANGED:
            if( 0 == ind->result || 0x1000 == ind->result ){
                g_avrcp_remote_volume = ind->volume;
                bt_ext_log("[avrcp] volume update to %d ctype:%d", g_avrcp_remote_volume, ctype);
                // percentage = (g_avrcp_remote_volume *100)/ 0x7f;
                if( g_avrcp_remote_volume <= 4){ // for better user experience. MW600 maybe dones't return 0 when it is too small
                    bt_ext_log("[avrcp] volume:%d too small as 0", g_avrcp_remote_volume);
                    g_avrcp_remote_volume = 0;
                }
                
                if( 0 == ind->is_interim ){
                    // need to register volumn notification again
                    btmtk_avrcp_send_reg_notievent_volume_changed_req(0);
                }

                ctype = get_ctype_from_result(ind->result, ind->is_interim);

                if( 0 != bt_rc_callbacks && 0 != bt_rc_callbacks->volume_change_cb ){
                    bt_ext_log("[avrcp] volume update to final:%d ctype:%d interm:%d", g_avrcp_remote_volume, ctype, ind->is_interim);
                    bt_rc_callbacks->volume_change_cb( g_avrcp_remote_volume, ctype);
                }

            }else{
                bt_ext_log("%s: volume value cannot get from remote",__FUNCTION__);
            }
            break;
        default:
            bt_ext_log("%s: warning no handle notify event_id:%d",__FUNCTION__, ind->event_id);
            break;
    }
}   

void handle_rc_metamsg_cmd_set_volume_cnf (bt_avrcp_set_absolute_volume_cnf_struct* ind){
    U16 ctype = get_ctype_from_result(ind->result, FALSE);
    
    if( 0 == ind->result || 0x1000 == ind->result ){
        g_avrcp_remote_volume = ind->volume;
        bt_ext_log("[avrcp] volume update to %d ctype:%d", g_avrcp_remote_volume, ctype);
        
        if( 0 != bt_rc_callbacks && 0 != bt_rc_callbacks->volume_change_cb ){
            bt_rc_callbacks->volume_change_cb( g_avrcp_remote_volume, ctype);
        }
    }else{
        bt_ext_log("%s: volume value cannot set to remote ",__FUNCTION__);
    }
}
