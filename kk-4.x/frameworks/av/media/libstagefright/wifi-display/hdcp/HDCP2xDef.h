/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * Description: header file for hdcp2.x
 *
 *---------------------------------------------------------------------------*/

#ifndef _HDCP2X_H_
#define _HDCP2X_H_

#include <cutils/log.h>
#include <jni.h>

/*-----------------------------
    Header files
------------------------------*/

/*-----------------------------
    Macro Defines
------------------------------*/
#ifndef HDCP2X_DBGLVL_DEFAULT
#define HDCP2X_DBGLVL_DEFAULT   HDCP2XDBG_MAX
#endif

#define HDCP2X_DBG(x,y...)  \
do {\
        /*fprintf(stderr, "[HDCP2.X] " y);*/ \
        LOGD("WFDHDCPTx " y); \
}while(0)



#define HDCP2X_FUNC_LOG()       HDCP2X_DBG(HDCP2XDBG_VERB, "Enter %s\n", __FUNCTION__)
#define HDCP2X_FUNC_EXIT_LOG()  HDCP2X_DBG(HDCP2XDBG_VERB, "Exit %s\n", __FUNCTION__)



/* HDCP2.0/2.1 MSG ID List */
#define HDCP2X_MSGID_NULL_MESSAGE               1
#define HDCP2X_MSGID_AKE_INIT                   2
#define HDCP2X_MSGID_AKE_SEND_CERT              3
#define HDCP2X_MSGID_AKE_NO_STORED_KM           4
#define HDCP2X_MSGID_AKE_STORED_KM              5
#define HDCP2X_MSGID_AKE_SEND_RRX               6
#define HDCP2X_MSGID_AKE_SEND_H_PRIME           7
#define HDCP2X_MSGID_AKE_SEND_PAIRING_INFO      8
#define HDCP2X_MSGID_LC_INIT                    9
#define HDCP2X_MSGID_LC_SEND_L_PRIME            10
#define HDCP2X_MSGID_SKE_SEND_EKS               11
#define HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST  12
#define HDCP2X_MSGID_RTT_READY                  13
#define HDCP2X_MSGID_RTT_CHALLENGE              14
#define HDCP2X_MSGID_REPEATERAUTH_SEND_ACK      15
#define HDCP2X_MSGID_REPEATERAUTH_STREAM_MANAGE 16
#define HDCP2X_MSGID_REPEATERAUTH_STREAM_READY  17
#define HDCP2X_MSGID_RECEIVER_AUTHSTATUS        18
#define HDCP2X_MSGID_AKE_TRANSMITTER_INFO       19
#define HDCP2X_MSGID_AKE_RECEIVER_INFO          20

/* HDCP2.x message length */
#define HDCP2X_MSGLEN_AKE_INIT                  9
#define HDCP2X_MSGLEN_AKE_SEND_CERT             524
#define HDCP2X_MSGLEN_AKE_NO_STORED_KM          129
#define HDCP2X_MSGLEN_AKE_STORED_KM             33
#define HDCP2X_MSGLEN_AKE_SEND_H_PRIME          33
#define HDCP2X_MSGLEN_AKE_SEND_PAIRING_INFO		17
#define HDCP2X_MSGLEN_AKE_TRANSMITTER_INFO      6
#define HDCP2X_MSGLEN_AKE_RECEIVER_INFO         6
#define HDCP2X_MSGLEN_AKE_SEND_RRX              9
#define HDCP2X_MSGLEN_LC_INIT                   9
#define HDCP2X_MSGLEN_LC_SEND_L_PRIME           33
#define HDCP2X_MSGLEN_LC_SEND_L_PRECOMPUTE_PRIME          17

#define HDCP2X_MSGLEN_RTT_READY                 1
#define HDCP2X_MSGLEN_RTT_CHALLENGE             17
#define HDCP2X_MSGLEN_SKE_SEND_EKS              25
#define HDCP2X_MSGLEN_REPEATERAUTH_SEND_RECEIVERID_LIST   192
#define HDCP2X_MSGLEN_REPEATERAUTH_STREAM_READY	33


/* HDCP2.x key length */
#define HDCP2X_RTX_LEN                          8
#define HDCP2X_CERTRX_LEN                       522
#define HDCP2X_EKPUB_KM_LEN                     128
#define HDCP2X_EKH_KM_LEN                       16
#define HDCP2X_RRX_LEN                          8
#define HDCP2X_M_LEN                            16
#define HDCP2X_KM_LEN                           16
#define HDCP2X_H_PRIME_LEN                      32
#define HDCP2X_RN_LEN                           8
#define HDCP2X_L_PRIME_LEN                      32
#define HDCP2X_L_PRECOMPUTE_LEN                 16
#define HDCP2X_EDKEY_KS_LEN                     16
#define HDCP2X_RIV                              8
#define HDCP2X_LC128_LEN                        16
#define HDCP2X_RIV_LEN                          8
#define HDCP2X_KS_LEN                           16

#define HDCP2X_KPUB_DCP_LEN						384

#define HDCP2X_V_PRIME_LEN						32
#define HDCP2X_RECEIVER_ID_LEN 					5
#define HDCP2X_MAX_RECEIVER						31

/* Misc filed lengths */
#define HDCP2X_LEN_CAP_MASK                  2

/* TRANSMITTER_CAPABILITY_MASK */
#define TX_CAP_MASK_TX_LOCALITY_PRECOMPUTE_SUPPORT  0x0001
/* RECEIVER_CAPABILITY_MASK */
#define RX_CAP_MASK_RX_LOCALITY_PRECOMPUTE_SUPPORT  0x0001


#define WFD_DEFAULT_PID_VIDEO                   0x1011
#define WFD_DEFAULT_PID_AUDIO                   0x1100

#define HDCP2X_MSGLEN_RepeaterAuth_Stream_Manage_T	6


/*-----------------------------
    Externed functions & variables
------------------------------*/
//extern int hdcp2x_dbg_lvl;


/*-----------------------------
    Structures
------------------------------*/
enum
{
    HDCP2XDBG_ERR = 0,
    HDCP2XDBG_WARN,
    HDCP2XDBG_DBG,
    HDCP2XDBG_VERB,
    HDCP2XDBG_MAX
};

enum HDCPVer {
    HDCP2_0 = 0,
    HDCP2_1,
    HDCP2_2
};

/* Error Definition */
#define HDCP2X_OK                              (0)
#define HDCP2X_ERR_OUT_OF_MEM                  (0x8001)
#define HDCP2X_ERR_READ_PUB_KEY                (0x8002)
#define HDCP2X_ERR_NOT_INIT                    (0x8003)
#define HDCP2X_ERR_INV_IP_ADDR                 (0x8004)
#define HDCP2X_ERR_KEY_GEN_RTX                 (0x8005)
#define HDCP2X_ERR_SEND_AKE_INIT               (0x8006)
#define HDCP2X_ERR_INV_RX_MSG                  (0x8007)
#define HDCP2X_ERR_ENCRYPT_KM                  (0x8008)                
#define HDCP2X_ERR_COMPUTE_KD                  (0x8009)                
#define HDCP2X_ERR_COMPUTE_HPRIME              (0x800A)                
#define HDCP2X_ERR_NOT_EQUAL_H                 (0x800B)                
#define HDCP2X_ERR_STORE_PAIR_INFO             (0x800C)                
#define HDCP2X_ERR_SEND_LC_INIT                (0x800D)
#define HDCP2X_ERR_ENCRYPT_KS                  (0x800E)
#define HDCP2X_ERR_SEND_EKS                    (0x800F)
#define HDCP2X_ERR_INV_PARAM                   (0x8010)
#define HDCP2X_ERR_AES_DECRYPT                 (0x8011)
#define HDCP2X_ERR_SEND_AKE_TX_INFO            (0x8012)
#define HDCP2X_ERR_SEND_RTT_CHALLENGE          (0x8013)
#define HDCP2X_ERR_COMPUTE_LPRIME              (0x8014)                
#define HDCP2X_ERR_SET_RX_VER                  (0x8015)                
#define HDCP2X_ERR_AES_INIT                    (0x8016)                
#define HDCP2X_ERR_AES_ENCRYPT                 (0x8017)                
#define HDCP2X_ERR_VERIFY_SIGNATURE            (0x8018)                
#define HDCP2X_ERR_GET_DRM_KEY                 (0x8019)                
#define HDCP2X_ERR_SET_ENC_KEY                 (0x8020)                
#define HDCP2X_ERR_COMPUTE_VPRIME              (0x8021)                
#define HDCP2X_ERR_NOT_EQUAL_V                 (0x8022)                

#define HDCP2X_ERR_INVALID_CERT                (0x8023)  
#define HDCP2X_ERR_REVOCATION_VERIFY           (0x8024)  
#define HDCP2X_ERR_SEND_REPEATERAUTH_STREAM_MANAGE          (0x8025)
#define HDCP2X_ERR_GEN_RN		               (0x8026)
#define HDCP2X_ERR_GEN_RIV		               (0x8027)
#define HDCP2X_ERR_GEN_KS		               (0x8028)



#define HDCP2X_ERR_TIMEOUT_WAIT_RX_INFO        (0x8100)                
#define HDCP2X_ERR_TIMEOUT_WAIT_H_PRIME        (0x8101)                
#define HDCP2X_ERR_TIMEOUT_WAIT_PAIRING_INFO   (0x8102)
#define HDCP2X_ERR_TIMEOUT_WAIT_L_PRIME        (0x8103)
#define HDCP2X_ERR_TIMEOUT_WAIT_RTT_READY      (0x8104)
#define HDCP2X_ERR_TIMEOUT_WAIT_RECEIVER_ID    (0x8105)

#define HDCP2X_AUTHENTICATED					(0)
#define HDCP2X_RE_AKE_INIT						(-2)
#define HDCP2X_ABORT							(-3)


#define HDCP2X_TX_WAIT_RX_CONNECTION_US              (1000*1000)       // 1s
#define HDCP2X_TX_WAIT_RECEIVER_INFO_US              (100*1000)        // 100ms
#define HDCP2X_TX_WAIT_NO_STORED_H_PRIME_US          (1000*1000)       // 1 s
#define HDCP2X_TX_WAIT_STORED_H_PRIME_US             (200*1000)        // 200 ms
#define HDCP2X_TX_WAIT_PAIRING_INFO_US               (200*1000)        // 200 ms
#define HDCP2X_TX_WAIT_L_PRIME_US                    (50*1000)         // 7 ms -> 50ms 7ms is too short
#define HDCP2X_TX_WAIT_RTT_READY_US                  (200*1000)        // 200 ms
#define HDCP2X_TX_WAIT_SHUTDOWN_US                   (2*1000)          // 2 ms
#define HDCP2X_TX_WAIT_AUTH_FAIL_US                  (2*1000)          // 2 ms
#define HDCP2X_TX_WAIT_RECEIVER_ID_US                (3000*1000)       // 3s
#define HDCP2X_TX_WAIT_STREAM_READY_US               (100*1000)        // 100ms



#define HDCP_TS_VIDEO_PID				(0x1011)
#define HDCP_TS_AUDIO_PID_MIN			(0x1100)
#define HDCP_TS_AUDIO_PID_MAX			(0x111F)




typedef struct MSG_AKE_INIT
{
    unsigned char msg_id;
    unsigned char rtx[HDCP2X_RTX_LEN];
} MSG_AKE_INIT_T;

typedef struct MSG_AKE_SEND_CERT
{
    unsigned char msg_id;
    unsigned char repeater;
    unsigned char certrx[HDCP2X_CERTRX_LEN];
} MSG_AKE_SEND_CERT_T;

typedef struct MSG_AKE_NO_STORED_KM
{
    unsigned char msg_id;
    unsigned char Ekpub_km[HDCP2X_EKPUB_KM_LEN];
} MSG_AKE_NO_STORED_KM_T;

typedef struct MSG_AKE_STORED_KM
{
    unsigned char msg_id;
    unsigned char Ekh_km[HDCP2X_EKH_KM_LEN];
    unsigned char m[HDCP2X_M_LEN];
} MSG_AKE_STORED_KM_T;

typedef struct MSG_AKE_TRANSMITTER_INFO
{
    unsigned char msg_id;
    unsigned char length[2];
    unsigned char version;
    unsigned char transmitter_capability_mask[HDCP2X_LEN_CAP_MASK];
} MSG_AKE_TRANSMITTER_INFO_T;

typedef struct MSG_AKE_RECEIVER_INFO
{
    unsigned char msg_id;
    unsigned char length[2];
    unsigned char version;
    unsigned char receiver_capability_mask[HDCP2X_LEN_CAP_MASK];
} MSG_AKE_RECEIVER_INFO_T;


typedef struct MSG_AKE_SEND_RRX
{
    unsigned char msg_id;
    unsigned char rrx[HDCP2X_RRX_LEN];
} MSG_AKE_SEND_RRX_T;

typedef struct MSG_AKE_SEND_H_PRIME
{
    unsigned char msg_id;
    unsigned char h_prime[HDCP2X_H_PRIME_LEN];
} MSG_AKE_SEND_H_PRIME_T;

typedef struct MSG_AKE_SEND_PAIRING_INFO
{
    unsigned char msg_id;
    unsigned char Ekh_km[HDCP2X_EKH_KM_LEN];
} MSG_AKE_SEND_PAIRING_INFO_T;

typedef struct MSG_LC_INIT
{
    unsigned char msg_id;
    unsigned char rn[HDCP2X_RN_LEN];
} MSG_LC_INIT_T;

typedef struct MSG_LC_SEND_L_PRIME
{
    unsigned char msg_id;
    unsigned char L_prime[HDCP2X_L_PRIME_LEN];
} MSG_LC_SEND_L_PRIME_T;

typedef struct MSG_SKE_SEND_EKS
{
    unsigned char msg_id;
    unsigned char Edkey_ks[HDCP2X_EDKEY_KS_LEN];
    unsigned char riv[HDCP2X_RIV_LEN];    
} MSG_SKE_SEND_EKS_T;

struct Receiver
{
	unsigned char RxID[HDCP2X_RECEIVER_ID_LEN];
};

typedef struct MSG_REPEATERAUTH_SEND_RECEIVERID_LIST
{
	unsigned char msg_id;
	unsigned char MAX_DEVS_EXCEEDED;
	unsigned char MAX_CASCADE_EXCEEDED;
	unsigned char DEVICE_COUNT;
	unsigned char DEPTH;
	unsigned char V_prime[HDCP2X_V_PRIME_LEN];
	struct Receiver ReceiverID_List[HDCP2X_MAX_RECEIVER];
	
} MSG_REPEATERAUTH_SEND_RECEIVERID_LIST_T;



typedef struct MSG_RTT_READY
{
    unsigned char msg_id;
} MSG_RTT_READY_T;

typedef struct MSG_RTT_CHALLENGE
{
    unsigned char msg_id;
    unsigned char L[HDCP2X_L_PRECOMPUTE_LEN];
} MSG_RTT_CHALLENGE_T;

typedef struct MSG_LC_SEND_L_PRIME2  // HDCP 2.1
{
    unsigned char msg_id;
    unsigned char L_prime[HDCP2X_L_PRECOMPUTE_LEN];
} MSG_LC_SEND_L_PRIME2_T;




/* for converter */


#define HDCP2X_MAX_DEVICE						10
#define HDCP2X_V_PRIME_LEN						32
#define HDCP2X_RECEIVER_ID_LEN 					5
#define HDCP2X_MAX_RECEIVER						31
#define HDCP2X_DEFAULT_DEPTH					1
#define HDCP2X_MAX_SEQ_NUM_V					0xffffff
#define HDCP2X_MAX_SEQ_NUM_M					0xffffff

#define HDCP2X_REPEATER_AUTH_ACK_LEN			17
#define HDCP2X_RECEIVER_AUTH_STATUS_LEN			4

#define HDCP2X_M_PRIME_LEN						32
#define	HDCP2X_M_STREAM_TYPE_LEN				7
#define HDCP2X_MAX_STREAM_COUNT					2

#define HDCP2X_2_0_IDMSG_MIN_LEN				(42-5)
#define HDCP2X_2_2_IDMSG_MIN_LEN				(31-5)



typedef struct MSG_Receiver_AuthStatus
{
	unsigned char msg_id;
	unsigned char LENGTH[2];
	unsigned char REAUTH_REQ;
	
} MSG_Receiver_AuthStatus_T;




typedef struct MSG_REPEATERAUTH_2_0_SEND_RECEIVERID_LIST
{
	unsigned char msg_id;
	unsigned char MAX_DEVS_EXCEEDED;
	unsigned char MAX_CASCADE_EXCEEDED;
	unsigned char DEVICE_COUNT;
	unsigned char DEPTH;
	unsigned char V_prime[HDCP2X_V_PRIME_LEN];
	unsigned char ReceiverID_List[31*HDCP2X_RECEIVER_ID_LEN];
	
} MSG_REPEATERAUTH_2_0_SEND_RECEIVERID_LIST_T;





typedef struct MSG_REPEATERAUTH_2_2_SEND_RECEIVERID_LIST
{
	unsigned char msg_id;
	
	unsigned char MAX_DEVS_EXCEEDED;
	unsigned char MAX_CASCADE_EXCEEDED;
	unsigned char DEVICE_COUNT;
	unsigned char DEPTH;
	unsigned char HDCP2_0_REPEATER_DOWNSTREAM;
	unsigned char HDCP1_DEVICE_DOWNSTREAM;
	unsigned char seq_num_V[3];
	unsigned char V_prime_most[HDCP2X_V_PRIME_LEN>>1];
	unsigned char ReceiverID_List[31*HDCP2X_RECEIVER_ID_LEN];
	
//	struct Receiver ReceiverID_List[HDCP2X_MAX_RECEIVER];
	
} MSG_REPEATERAUTH_2_2_SEND_RECEIVERID_LIST_T;




typedef struct MSG_REPEATERAUTH_Stream_Ready
{
	unsigned char msg_id;
	unsigned char M_prime[HDCP2X_M_PRIME_LEN];

	
} MSG_REPEATERAUTH_Stream_Ready;










typedef struct MSG_REPEATERAUTH_SEND_Ack
{
	unsigned char msg_id;
	unsigned char V_prime_least[HDCP2X_V_PRIME_LEN>>1];
	
} MSG_REPEATERAUTH_SEND_Ack_T;



typedef struct MSG_RepeaterAuth_Stream_Manage
{
	unsigned char msg_id;
	unsigned char seq_num_M[3];
	unsigned char k[2];
	unsigned char STREAM_TYPE[HDCP2X_MAX_STREAM_COUNT * HDCP2X_M_STREAM_TYPE_LEN];
	
} MSG_RepeaterAuth_Stream_Manage_T;



typedef struct Stream_Manage_Info
{
	unsigned int   streamCtr;
	unsigned short ContentStreamID;
	unsigned char  Type;
} Stream_Manage_Info_T;

















#endif /* _HDCP2X_H_ */



