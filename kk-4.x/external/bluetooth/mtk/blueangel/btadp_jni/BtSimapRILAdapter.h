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



/* 256 bytes for data + 2 bytes status word SW1 and SW2 */
#define SIM_RSP_BYTEARRAY_BUF_LEN  258


typedef enum
{
	SIMAP_CONNECT_SIM_REQ    = 0,
	SIMAP_DISCONNECT_SIM_REQ = 1,
	SIMAP_POWER_SIM_ON_REQ   = 2,
	SIMAP_POWER_SIM_OFF_REQ  = 3,
	SIMAP_RESET_SIM_REQ      = 4,
	SIMAP_TRANSFER_APDU_REQ  = 5,
	
	SIMAP_TRANSFER_ATR_REQ                = 6,
	SIMAP_TRANSFER_CARD_READER_STATUS_REQ = 7,
	SIMAP_SET_TRANSPORT_PROTOCOL_REQ      = 8,

	SIMAP_RIL_REQ_LAST
}bt_simap_sim_req_enum;

typedef enum
{
	SIMAP_RSP_SUCCESS = 0,
	SIMAP_RSP_ERR_OTHER = 1,
	SIMAP_RSP_ERR_CARD_NOT_ACCESSIBLE = 2,
	SIMAP_RSP_ERR_CARD_REMOVED = 4,

	SIMAP_RSP_ERR_BUSY = 99,
	
	SIMAP_RSP_ERR_LAST

}bt_simap_sim_rsp_code_enum;

int bt_simap_init_ril_sock(int sim_index);
void bt_simap_deinit_ril_sock();

int bt_simap_process_connect_sim_request(int* rsp_curTPtype, int* rsp_suppTPtype, int* rsp_atr_len, char* rst_atr);
int bt_simap_process_client_simple_request(int request);
int bt_simap_process_apdu_request(char *req_data, int req_data_len, char *out_data, int* out_data_len);
int bt_simap_process_reset_sim_request(int* rsp_curTPtype, int* rsp_atr_len, char* rst_atr);
int bt_simap_process_poweron_sim_request(int* rsp_curTPtype, int* rsp_atr_len, char* rst_atr);



