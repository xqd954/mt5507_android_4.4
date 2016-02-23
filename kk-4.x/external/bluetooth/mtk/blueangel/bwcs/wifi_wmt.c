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


#define LOG_TAG "wifi_wmt.c"

#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_bwcs.h"
#include "wmt_struct.h"
#include "cutils/sockets.h"
#include "utils/Log.h"

static int g_bwcsfd = -1;

#define ILM_HEADER_SIZE (sizeof(ilm_struct)-MAX_ILM_BUFFER_SIZE)
#define CAL_ILM_SIZE(type) (ILM_HEADER_SIZE+sizeof(type))

static int createConnection(void) {
    if(g_bwcsfd < 0) {
        g_bwcsfd = socket_local_server(WIFI_SOCK_NAME,
                                                            ANDROID_SOCKET_NAMESPACE_ABSTRACT,
                                                            SOCK_DGRAM);
        if(g_bwcsfd >= 0) {
            struct sockaddr_un btname;
            socklen_t   btnamelen; 
            btname.sun_family = AF_UNIX;
            strcpy (btname.sun_path, BT_SOCK_NAME_INT_ADP);
            btnamelen = (offsetof (struct sockaddr_un, sun_path) + strlen(btname.sun_path) + 1);
            if ( connect(g_bwcsfd, (const struct sockaddr*)&btname, btnamelen) < 0)
            {
                ALOGE("connect to BT task failed, %s, %d", strerror(errno), errno);
                close(g_bwcsfd);
                g_bwcsfd = -1;
            }
        }else {
            ALOGE("createConnection failed, %s, %d", strerror(errno), errno);
        }
    }
    return g_bwcsfd;
}

static int sendCmd(unsigned int cmd, unsigned int resp, ilm_struct *ilm, unsigned int *size) {
    //ilm_struct *ilm;
    unsigned short *field;    
    int ret = -1;
    int fd = createConnection();
    if(fd < 0)
        return fd;
	
    /* Set src_mod to MOD_MMI */
    ilm->src_mod_id = MOD_BWCS;
    ilm->dest_mod_id = MOD_BT;
    ilm->sap_id = 0;
    ilm->msg_id = cmd;

    /* LOCAL_PARA_HDR */
    field = (unsigned short*)ilm->ilm_data;
    *field = 1;
    *(++field) = (unsigned short)*size;

    // cal whole ilm size
    *size = sizeof(ilm_struct) - MAX_ILM_BUFFER_SIZE + *size;
    ALOGI("sendCmd cmd=%lu, size=%u", ilm->msg_id, *size);
    do{
        ret = send(fd, (void*)ilm, *size, 0);
    }while( ret < 0 && errno == EINTR);
    if(ret >= 0) {
        if (resp > 0 ) {
            fd_set readfs;
            struct timeval Timeout;

            Timeout.tv_usec = 0;
            Timeout.tv_sec  = 3;
            FD_ZERO(&readfs);
            FD_SET(fd, &readfs);
            do{
                ret = select(fd+1, &readfs, NULL, NULL, &Timeout);
            }while( ret < 0 && errno == EINTR);
            if(ret > 0)
            {
                ret = recvfrom(fd, (void*)ilm, sizeof(ilm_struct), 0, NULL, NULL);
                if( ilm->msg_id != resp)
                {
                    ALOGE("unexpected resp=%lu, expected=%u", ilm->msg_id, resp);
                    ret = -1;
                }
                else
                {
                    *size = ret;
                    ALOGI("Recv resp, size=%d", ret);
                }
            }else {
                ALOGE("select failed, ret=%d, %s, %d", ret, strerror(errno), errno);
                ret = -1;
            }
        }else {
            ALOGI("No response for this command");
        }
    }else {
        ALOGE("send failed, %s, %d", strerror(errno), errno);
    }
    return ret;
}

int bwcs_query_bt_state(unsigned char *state) {
    unsigned int size;
    int result;
    ilm_struct ilm;
    wmt_bt_query_state_req_struct *req = (wmt_bt_query_state_req_struct*)ilm.ilm_data;
    wmt_bt_query_state_cnf_struct *cnf = NULL;

    if(!state) {
        ALOGE("Invalid parameter : %p", state);
        return -1;
    }

    ALOGI("bwcs_query_bt_state(%p)", state);
    size = sizeof(wmt_bt_query_state_req_struct);
    result = sendCmd(MSG_ID_WMT_BT_QUERY_STATE_REQ, 
                                 MSG_ID_WMT_BT_QUERY_STATE_CNF, 
                                 &ilm, 
                                 &size);
    if(result >= 0 && result == CAL_ILM_SIZE(wmt_bt_query_state_cnf_struct)) {
        cnf = (wmt_bt_query_state_cnf_struct*)ilm.ilm_data;
        *state = cnf->state;
        ALOGI("bt state = %d", *state);
    } else {
        ALOGE("bwcs_query_bt_state failed. result=%d, expected size=%d", result, sizeof(wmt_bt_query_state_cnf_struct));
    }
    return result;
}

int bwcs_set_afh(unsigned int freq, unsigned char *chnl_map, unsigned char hb) {
    unsigned int size;
    ilm_struct ilm;
    bt_wifi_set_chnl_req_struct *req = (bt_wifi_set_chnl_req_struct*)ilm.ilm_data;

    ALOGI("bwcs_set_afh(%p, %d)", chnl_map, hb);
    if(chnl_map == NULL) {
        ALOGE("[ERR] chnl_map is NULL");
        return -1;
    }
    req->afh = KAL_TRUE;
    req->pta = KAL_FALSE;
    memcpy(req->chnl_map, chnl_map, sizeof(req->chnl_map));
    req->freq = freq;
    req->hb = hb;
    size = sizeof(bt_wifi_set_chnl_req_struct);
    return sendCmd(MSG_ID_WMT_BT_SET_BWCS_REQ, 
                                 0, 
                                 &ilm, 
                                 &size);
}

int bwcs_set_pta(unsigned char action) {
    unsigned int size;
    ilm_struct ilm;
    bt_wifi_set_chnl_req_struct *req = (bt_wifi_set_chnl_req_struct*)ilm.ilm_data;

    ALOGI("bwcs_set_pta(action=%d)", action);
    req->afh = KAL_FALSE;
    req->pta = KAL_TRUE;
    req->pta_action = action;
    size = sizeof(bt_wifi_set_chnl_req_struct);
    return sendCmd(MSG_ID_WMT_BT_SET_BWCS_REQ, 
                                 0, 
                                 &ilm, 
                                 &size);
}

/* query rssi */
/* return value : 0 means success, < 0 means failed */
int bwcs_query_rssi(unsigned int conn_handle, int *rssi) {
    unsigned int size;
    int result;
    ilm_struct ilm;
    wmt_bt_query_rssi_req_struct *req = (wmt_bt_query_rssi_req_struct*)ilm.ilm_data;
    wmt_bt_query_rssi_ind_struct *ind = NULL;

    if(!rssi) {
        ALOGE("Invalid parameter : rssi=null");
        return -1;
    }

    ALOGI("bwcs_query_rssi(conn_handle=%d)", conn_handle);
    req->bt_conn_handle = conn_handle;
    size = sizeof(wmt_bt_query_rssi_req_struct);
    result = sendCmd(MSG_ID_WMT_BT_QUERY_RSSI_REQ, 
                                 MSG_ID_WMT_BT_QUERY_RSSI_IND, 
                                 &ilm, 
                                 &size);
    if(result >= 0 && result == CAL_ILM_SIZE(wmt_bt_query_rssi_ind_struct)) {
        ind = (wmt_bt_query_rssi_ind_struct*)ilm.ilm_data;
        if(ind->is_valid == KAL_TRUE) {
            *rssi = ind->bt_rssi;
            if((*rssi) & 0x80) {
                (*rssi) -= 256;
            }
            ALOGI("rssi=%d", *rssi);
        }else {
            result = -1;
            ALOGW("[WARN] is_valid is false");
        }
    }else {
        ALOGE("[ERR] bwcs_query_rssi failed. result=%d, expected size=%d", result, sizeof(wmt_bt_query_rssi_ind_struct));
    }
    return result;
}

int bwcs_set_rx_range(unsigned char lower_boundary, unsigned char higher_boundary) {
    unsigned int size;
    int result;
    ilm_struct ilm;

    ALOGI("bwcs_set_rx_range(lower=%d, higher=%d)", lower_boundary, higher_boundary);
    wmt_bt_set_rx_range_req_struct *req = (wmt_bt_set_rx_range_req_struct*)ilm.ilm_data;
    req->lower_boundary = lower_boundary;
    req->higher_boundary = higher_boundary;
    size = sizeof(wmt_bt_set_rx_range_req_struct);
    result = sendCmd(MSG_ID_WMT_BT_SET_RX_RANGE_REQ, 
                                 MSG_ID_WMT_BT_SET_RX_RANGE_CNF, 
                                 &ilm, 
                                 &size);
    return result;
}

int bwcs_set_default_tx_power(unsigned char power) {
    unsigned int size;
    int result;
    ilm_struct ilm;

    ALOGI("bwcs_set_default_tx_power(power=%d)", power);
    wmt_bt_set_default_tx_power_req_struct *req = (wmt_bt_set_default_tx_power_req_struct*)ilm.ilm_data;    
    req->ucPower = power;
    size = sizeof(wmt_bt_set_default_tx_power_req_struct);
    result = sendCmd(MSG_ID_WMT_BT_SET_DEFAULT_TX_POWER_REQ, 
                                 MSG_ID_WMT_BT_SET_DEFAULT_TX_POWER_CNF, 
                                 &ilm, 
                                 &size);
    return result;
}

int bwcs_update_conn_tx_power(unsigned int conn_handle, unsigned char power) {
    unsigned int size;
    int result;
    ilm_struct ilm;
    ALOGI("bwcs_update_conn_tx_power(conn_handle=%d, power=%d)", conn_handle, power);
    wmt_bt_update_conn_tx_power_req_struct *req = (wmt_bt_update_conn_tx_power_req_struct*)ilm.ilm_data;
    req->conn_handle = conn_handle;
    req->ucPower = power;
    size = sizeof(wmt_bt_update_conn_tx_power_req_struct);
    result = sendCmd(MSG_ID_WMT_BT_UPDATE_CONN_TX_POWER_REQ, 
                                 MSG_ID_WMT_BT_UPDATE_CONN_TX_POWER_CNF, 
                                 &ilm, 
                                 &size);
    return result;
}
