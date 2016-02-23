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

#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#include "windows.h"
#include "stdio.h"
#endif

#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
//#include "mbt_ag.h"
#include "mbt_gap.h"
#include "mbt_sdc.h"
#include "mbt_evhandler.h"
#elif defined(BTMTK_ON_WISE)
#include "MBTType.h"
#include "MBTSdc.h"
#endif


#include "bt_mmi.h"
#include "bt_hfg_struct.h"
#include "bt_hfg_api.h"
#include "bt_debug.h"
#include "bluetooth_trc.h"
#include "ps_trace.h"
#include "bt_gap_api.h"
#include "bt_ext_utility.h"
/********************************************************************************
*   Constant definition
********************************************************************************/
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#define AG_USE_CALL_OPTION2
#elif defined(BTMTK_ON_WISE)
#define CUSTOM_FOR_WISE
#ifdef CUSTOM_FOR_WISE
#define AG_CALL_SIMULATION
#define DELAY_CALL_STATE_CHANGE
#endif /* CUSTOM_FOR_WISE */
#endif

#define HSP_CONTEXT		0
#define HFP_CONTEXT			1
#define NUM_OF_CONTEXT		2

#define ID_RING_TIMER	3
#define AG_RING_TIMER_PERIOD    3000

typedef enum _ProfileState
{
	PROFILE_STATE_IDLE,
	PROFILE_STATE_ACTIVATING,
	PROFILE_STATE_DEACTIVATING,
	PROFILE_STATE_ACTIVATED,
	PROFILE_STATE_AUTO_REJECTING,
	// Below is active profile state
	PROFILE_STATE_AUTHENTICATE,
	PROFILE_STATE_ACCEPTING,
	PROFILE_STATE_REJECTING,
	PROFILE_STATE_CONNECTING,
	PROFILE_STATE_DISCONNECTING,
	// Below is connected profile state
	PROFILE_STATE_CONNECTED,
	PROFILE_STATE_AUDIO_CONNECTING,
	PROFILE_STATE_AUDIO_DISCONNECTING,
	PROFILE_STATE_AUDIO_CONNECTED,
}ProfileState;

// OS API macro
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#define ADP_OS_MemAlloc(n)			malloc(n)
#define ADP_OS_MemFree(p)			free(p)
#define ADP_OS_MemSet(dest, c, n)	memset(dest, c, n)
#define ADP_OS_MemCopy(dest, src, n)	memcpy(dest, src, n)
#define ADP_OS_MemCmp(p1, p2, n)	memcmp(p1, p2, n)
#define ADP_OS_StrLen(s)				strlen(s)
#define ADP_OS_StrCpy(dest, src)		strcpy(dest, src)
#define ADP_OS_Sprintf				sprintf
#elif defined(BTMTK_ON_WISE)
#define ADP_OS_MemAlloc(n)			malloc(n)//get_ctrl_buffer(n)
#define ADP_OS_MemFree(p)			free(p)//free_ctrl_buffer(p)
#define ADP_OS_MemSet(dest, c, n)	memset(dest, c, n)
#define ADP_OS_MemCopy(dest, src, n)	memcpy(dest, src, n)
#define ADP_OS_MemCmp(p1, p2, n)	memcmp(p1, p2, n)
#define ADP_OS_StrLen(s)				strlen(s)
#define ADP_OS_StrCpy(dest, src)		strcpy(dest, src)
#define ADP_OS_Sprintf				sprintf
#endif



#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
#define MBT_AG_INITLOCK()			if(!g_AgContext.bCr) InitializeCriticalSection(&g_AgContext.cr); g_AgContext.bCr = MBT_TRUE
#define MBT_AG_DEINITLOCK()		//if(g_AgContext.bCr) DeleteCriticalSection(&g_AgContext.cr); g_AgContext.bCr = MBT_FALSE
#define MBT_AG_LOCK()				if(g_AgContext.bCr) EnterCriticalSection(&g_AgContext.cr)
#define MBT_AG_UNLOCK()			if(g_AgContext.bCr) LeaveCriticalSection(&g_AgContext.cr)
#else
#define MBT_AG_INITLOCK()
#define MBT_AG_DEINITLOCK()
#define MBT_AG_LOCK()
#define MBT_AG_UNLOCK()
#endif
#define AG_ASSERT(cond)

// Debug macro
#define AG_LOG_GROUP_API		TRACE_FUNC
#define AG_LOG_GROUP_CBK		TRACE_FUNC
#define AG_LOG_GROUP_EVT		TRACE_FUNC
#define AG_LOG_GROUP_INFO		TRACE_FUNC
#define AG_LOG_GROUP_WARN	TRACE_FUNC
#define AG_LOG_GROUP_ERR		TRACE_FUNC

#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
void bt_prompt_trace(module_type mod_id, const char *fmt,...);
#endif

#ifdef BTMTK_ON_WISESDK
#define AG_LOG_API(s)		bt_trace s
#define AG_LOG_CBK(s)       bt_trace s
#define AG_LOG_EVT(s)       bt_trace s
#define AG_LOG_INFO(s)      bt_trace s
#define AG_LOG_WARN(s)      bt_trace s
#define AG_LOG_ERR(s)        bt_trace s
#define AG_LOG_MSG(s)       bt_prompt_trace s
#else
#define AG_LOG_API(s)		bt_trace s
#define AG_LOG_CBK(s)       bt_trace s
#define AG_LOG_EVT(s)       bt_trace s
#define AG_LOG_INFO(s)      bt_trace s
#define AG_LOG_WARN(s)      bt_trace s
#define AG_LOG_ERR(s)        bt_trace s
#define AG_LOG_MSG(s)       bt_prompt_trace s
#endif

MBT_VOID btmtk_ag_ringstart(MBT_VOID);
MBT_VOID btmtk_ag_ringstop(MBT_VOID);
void StartRingTimer(void);
void StopRingTimer(void);

#if defined(AG_CALL_SIMULATION)
MBT_VOID resetAllCallState(MBT_VOID);
MBT_VOID setCallIdx(MBT_BYTE idx);
MBT_VOID changeCallState(T_MBT_AG_PHONE_CALLSTATE newState);
MBT_BYTE getCallCount(T_MBT_AG_PHONE_CALLSTATE state);
T_MBT_AG_PHONE_CALLSTATE getSelCallState(MBT_VOID);
#endif


void btmtk_gap_handle_authorize_req(MBT_SERVICE_ID MBTSvcID, T_MBT_BDADDR RemoteBDAddr, MBT_CHAR* Name);

MBT_VOID btmtk_ag_connect(T_MBT_BDADDR RemoteBDAddr, MBT_SERVICE_ID MBtSvc);
void btmtk_hfag_authorize_res(MBT_BYTE AuthMode);
/********************************************************************************
*   Type definition
********************************************************************************/
typedef struct _AgContext
{
	MBT_BOOL		bEnabled;
	HfgChannelContext *pHfgContext[NUM_OF_CONTEXT];	// 0: headset context. 1: Handsfree context
	U8				activeProfile;	// 0xFF no active context
	HfgChannelContext *activeContext;
	ProfileState		state[NUM_OF_CONTEXT];
	// For defered connect request
	MBT_BOOL		bDeferedConnReq;
	T_MBT_BDADDR	deferedAddr;
	MBT_SERVICE_ID	deferedSvcID;
	// Session state variable
	MBT_BOOL		bVR;
	/* SH 11/11: MMIdoes not send response after send btrh result. So we have to remember 
	it is a solicited or unsolicited code. Reset this flag after send btrh result or "ERROR" 
	response is sent. When connection is disconnected also set this to FALSE. */
	MBT_BOOL		bBtrh;
	// Ring timer
	MBT_BOOL		bRingStarted;
	#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
	UINT_PTR 		timerId;
	// Synchronize object
	MBT_BOOL		bCr;
	CRITICAL_SECTION	cr;
	#endif
       U8                       prefMemStorageMask;
}AgContext;

#define NULL_CONTEXT	0xFF

/********************************************************************************
*   Global variable
********************************************************************************/
static AgContext g_AgContext = 
{
	MBT_FALSE, 
	{NULL, NULL}, 
	NULL_CONTEXT,
	NULL,
	{PROFILE_STATE_IDLE, PROFILE_STATE_IDLE},
	// For defered connect request
	MBT_FALSE,		// bDeferedConnReq
	{0},				// Address of defered conn req
	0,				// deferedSvcID
	MBT_FALSE,		// bVR
	MBT_FALSE,		// bBtrh
	MBT_FALSE,		// bRingStarted
	#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)	
	0,				// timerId
	MBT_FALSE		//bCr
	#endif
};

#ifdef DELAY_CALL_STATE_CHANGE
MBT_BOOL g_bDelayCallStatusSent = MBT_FALSE;
MBT_BOOL g_bHeldChanged = MBT_FALSE;
#endif
/********************************************************************************
*   Util functions
********************************************************************************/
const char *g_SMSStatus[] = 
{
	"REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL"
};

/* T_MBT_AG_STATE */
static const char* g_strAGState[] =
{
	"MBT_AG_STATE_IDLE",
	"MBT_AG_STATE_CONNECTING",
	"MBT_AG_STATE_DISCONNECTING",
	"MBT_AG_STATE_CONNECTED",
	"MBT_AG_STATE_AUDIO_CONNECTING",
	"MBT_AG_STATE_AUDIO_DISCONNECTING",
	"MBT_AG_STATE_AUDIO_CONNECTED",
};
/* T_MBT_AG_DIAL */
static const char* g_strDialType[] =
{
	"MBT_AG_CALL_NORMALDIAL",
	"MBT_AG_CALL_REDIAL",
	"MBT_AG_CALL_MEMDIAL",
};
/* T_MBT_AG_BTRH */
static const char* g_strBtrh[] =
{
	"MBT_AG_RH_HOLD_INCOMING_CALL",
	"MBT_AG_RH_ACCEPT_HELD_CALL",
	"MBT_AG_RH_REJECT_HELD_CALL",
	"MBT_AG_RH_NO_RESPONSE",
};
/* T_MBT_AG_PB_DEV */
static const char* g_strPbType[] =
{
	"MBT_AG_PB_DEV_ME",	            // ME phonebook list
	"MBT_AG_PB_DEV_DC",			// ME Dialed calls list
	"MBT_AG_PB_DEV_MC",			// ME missed calls list
	"MBT_AG_PB_DEV_RC",			// ME received calls list
	"MBT_AG_PB_DEV_SM",			// SIM phonebook list
	"MBT_AG_PB_DEV_FD",			// SIM fix-dialing phonebook list
	"MBT_AG_PB_DEV_LD",			// SIM last-dialing phonebook list
	"MBT_AG_PB_DEV_MT",			// combined ME and SIM phonebook list
	"MBT_AG_PB_DEV_MAX",
};
/* T_MBT_AG_SM_STORAGE */
static const char* g_strSMStorage[] =
{
	"MBT_AG_SM_MEM_BM",	// broadcast message storage
	"MBT_AG_SM_MEM_ME",		// ME message stroage
	"MBT_AG_SM_MEM_MT",		// any of the strorages associated with ME
	"MBT_AG_SM_MEM_SM",		// SIM message storage
	"MBT_AG_SM_MEM_TA",		// TA message stroage
	"MBT_AG_SM_MEM_SR",		// status report storage
	"MBT_AG_SM_MEM_MAX",
};

/* T_MBT_AG_SM_MGF */
static const char* g_strSMFormat[] =
{
	"MBT_AG_SM_MGF_PDU",		// PDU message format (default when implemented)
	"MBT_AG_SM_MGF_TEXT",			// text message format
};

/* T_MBT_AG_SM_SHOW */
static const char* g_strSMShow[] =
{
	"MBT_AG_SM_SHOW_OFF",	// do not show header values in +CMGL, +CMGR
	"MBT_AG_SM_SHOW_ON",			// show the header values in +CMGL, +CMGR
};

/* T_MBT_AG_SM_MODE */
static const char* g_strSMMode[] =
{
	"MBT_AG_SM_MODE_BUFFER",	 	// Buffer indications. If full, buffered other place or the oldest is discard
	"MBT_AG_SM_MODE_DISCARD_FWD",	// Discard indication and reject new when reserved, otherwise forward
	"MBT_AG_SM_MODE_BUFFER_FLUSH",	// Buffer indications when reserved, and then flush
	"MBT_AG_SM_MODE_FORWARD",		// Forward indications to HF
	"MBT_AG_SM_MODE_MAX",
};

/* T_MBT_AG_SM_MT */
#if 0
static const char* g_strSMMode[] =
{
	"MBT_AG_SM_MT_NO_DELIVER",	            // No SMS-DELIVER indications are routed to the HF
	"MBT_AG_SM_MT_CMTI", 				// SMS-DELIVERs are routed by +CMTI
	"MBT_AG_SM_MT_CMT",				// SMS-DELIVERs are routed by +CMT
	"MBT_AG_SM_MT_CMT_CMTI",			// SMS-DELIVERs are routed by +CMT(class3) and +CMTI(other class)
	"MBT_AG_SM_MT_CMT_MAX",
};
#endif
const char* statusToString(T_MBT_AG_SM_STATUS status)
{
	if(status < (sizeof(g_SMSStatus)/sizeof(g_SMSStatus[0])))
		return g_SMSStatus[status];
	else
		return NULL;
}

HfgPbStorageType mapMBTPbToHfgPb(T_MBT_AG_PB_DEV pb)
{
	static const HfgPbStorageType hfgPbTbl[] = 
	{
		HFG_PBS_ME_PHONEBOOK,		// MBT_AG_PB_DEV_ME
		HFG_PBS_DIALED_CALLS,		// MBT_AG_PB_DEV_DC
		HFG_PBS_MISSED_CALLS,		// MBT_AG_PB_DEV_MC
		HFG_PBS_RECEIVED_CALLS,		// MBT_AG_PB_DEV_RC
		HFG_PBS_SIM_PHONEBOOK,		// MBT_AG_PB_DEV_SM
		HFG_PBS_FIXED_DIAL,			// MBT_AG_PB_DEV_FD
		HFG_PBS_LAST_DIAL,			// MBT_AG_PB_DEV_LD
		HFG_PBS_ME_SIM_COMBINED,	//MBT_AG_PB_DEV_MT
	};
	const MBT_UINT count = (sizeof(hfgPbTbl)/sizeof(hfgPbTbl[0]));
	if( (MBT_UINT)pb >= count )
	{
		//MBT_ERR1("[AG][ERR] Invalid T_MBT_AG_PB_DEV value : %d", pb);
		pb = MBT_AG_PB_DEV_ME;
	}
	return hfgPbTbl[pb];
}

#ifdef BTMTK_WISE_MBT_LOG
static void LogBDAddress(T_MBT_BDADDR addr)
{
    char bd_addr[60];
    sprintf(bd_addr, "[BTMTK][AG] BDAddr=0x%02X%02X%02X%02X%02X%02X", 
        addr[0],
        addr[1],
        addr[2],
        addr[3],
        addr[4],
        addr[5]);
    MBT_LOG(bd_addr);
}
#endif

void PduToString(MBT_CHAR *pdu, MBT_CHAR *strPdu, U16 size)
{
	U16 i = 0;
	for(i = 0;i < size;i++)
	{
		ADP_OS_Sprintf(strPdu, "%02X", *((unsigned char*)pdu));
		pdu++;
		strPdu += 2;
	}
}

void StringToPdu(const MBT_CHAR *strPdu, MBT_CHAR *pdu, U16 size)
{
	U16 i;
	MBT_CHAR tmp[3];

	tmp[2] = 0;
	for(i = 0;i < size;i++)
	{
		tmp[0] = *(strPdu++);
		tmp[1] = *(strPdu++);
		*pdu = (MBT_CHAR)strtol(tmp, NULL, 16);
		pdu++;
	}
}

void ag_ring(void)
{
	MBT_BOOL bHeadset;
	HfgChannelContext		*pContext;
	HfgCallerIdParms		callerid;
	T_MBT_AG_DIALINFO	*pAGDialInfo;

	MBT_AG_LOCK();
    AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_RING_TIMER_TRIGGERED));
	bHeadset = (g_AgContext.activeProfile == HSP_CONTEXT);
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
 	    g_AgContext.bRingStarted)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
		callerid.number = pAGDialInfo->Digit;
		if(pAGDialInfo->Digit[0] == '+')
			callerid.type = 145;
		else
			callerid.type = 129;
		btmtk_hfg_send_ring(pContext);
		if(!bHeadset)
		btmtk_hfg_send_caller_id(pContext, &callerid);
	}
	MBT_AG_UNLOCK();
}
#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
void RingTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
	KillTimer(NULL, g_AgContext.timerId);
	#endif
	ag_ring();
	#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
	g_AgContext.timerId = SetTimer(NULL, ID_RING_TIMER, AG_RING_TIMER_PERIOD, (TIMERPROC) RingTimerProc);
	#endif
}
#elif defined(BTMTK_ON_WISE)
void RingTimerProc(void *pArg)
{
    ag_ring();
    StartRingTimer();
}
#endif
void StartRingTimer(void)
{
	#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
	g_AgContext.timerId = SetTimer(NULL, ID_RING_TIMER, AG_RING_TIMER_PERIOD, (TIMERPROC) RingTimerProc);
    #elif defined(BTMTK_ON_WISE)
    extern void ws_mm_start_timer(kal_uint8 timer_id, kal_uint32 period, kal_timer_func_ptr timer_expiry, void *arg);
    ws_mm_start_timer(3, AG_RING_TIMER_PERIOD, RingTimerProc, NULL);
    AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_RING_TIMER_START));
	#endif
}

void StopRingTimer(void)
{
	#if defined(BTMTK_ON_WIN32) || defined(BTMTK_ON_WISESDK)
	KillTimer(NULL, g_AgContext.timerId);
    #elif defined(BTMTK_ON_WISE)
    extern void ws_mm_stop_timer(kal_uint8 timer_id);
    ws_mm_stop_timer(3);
    AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_RING_TIMER_STOP));
	#endif
}

#ifdef BTMTK_ON_WISESDK
#define ag_postevent(a,b) { MBT_LOG1("[BTMTK][AG][EVT] %s",#a); _ag_postevent(a,b);}
MBT_BOOL _ag_postevent(T_MBTEVT Evt, MBT_SHORT conn_idx)
{
	AG_LOG_EVT((AG_LOG_GROUP_EVT, BT_HFG_WISE_EVT, Evt, conn_idx));
	return mbt_postevent(Evt, conn_idx);
}
#else
MBT_BOOL ag_postevent(T_MBTEVT Evt, MBT_SHORT conn_idx)
{
	AG_LOG_EVT((AG_LOG_GROUP_EVT, BT_HFG_WISE_EVT, Evt, conn_idx));
	return mbt_postevent(Evt, conn_idx);
}
#endif

void changeProfileState(U8 profile, ProfileState newState)
{
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_PROFILE_STATE_CHANGE, profile, g_AgContext.state[profile], newState));

	if(profile == g_AgContext.activeProfile && newState == PROFILE_STATE_ACTIVATED)
	{
		// Active profile back to activated state
		// CLear session state
		g_AgContext.bBtrh = MBT_FALSE;
		g_AgContext.bVR = MBT_FALSE;
        if(g_AgContext.bRingStarted)
            btmtk_ag_ringstop();
	}
	
	g_AgContext.state[profile] = newState;	
	g_AgContext.activeProfile = NULL_CONTEXT;
	g_AgContext.activeContext = NULL;	
	for( profile = 0; profile < NUM_OF_CONTEXT;profile++ )
	{
		if( g_AgContext.state[profile] > PROFILE_STATE_AUTO_REJECTING )
		{
			g_AgContext.activeProfile = profile;
			g_AgContext.activeContext = g_AgContext.pHfgContext[profile];	
			break;
		}
	}

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_SET_ACTIVE_PROFILE, g_AgContext.activeProfile, g_AgContext.activeContext));
}
/************************************************************************
* SDC update function
************************************************************************/
static MBT_CHAR* convert_bool_to_string(MBT_BOOL bTrue)
{
    if(bTrue)
        return "MBT_TRUE";
    else
        return "MBT_FALSE";
}

static void sdc_update_remote_address(T_MBT_BDADDR addr)
{
	T_MBT_AG_STATUS *pAgStatus;

       pAgStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
       {
            char msg[100];
            sprintf(msg, "[BTMTK][AG][SDC] sdcAGStatus->BdAddr : 0x%02X%02X%02X%02X%02X%02X => 0x%02X%02X%02X%02X%02X%02X",
                                pAgStatus->BdAddr[0],
                                pAgStatus->BdAddr[1],
                                pAgStatus->BdAddr[2],
                                pAgStatus->BdAddr[3],
                                pAgStatus->BdAddr[4],
                                pAgStatus->BdAddr[5],
                                addr[0],
                                addr[1],
                                addr[2],
                                addr[3],
                                addr[4],
                                addr[5]);
            MBT_LOG(msg);
       }
	ADP_OS_MemCopy( pAgStatus->BdAddr, addr, sizeof(T_MBT_BDADDR));
}

static void sdc_clear_remote_address(void)
{
	T_MBT_AG_STATUS *pAgStatus;

       pAgStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
       {
            char msg[100];
            sprintf(msg, "[BTMTK][AG][SDC] sdcAGStatus->BdAddr : 0x%02X%02X%02X%02X%02X%02X => 0x00",
                                pAgStatus->BdAddr[0],
                                pAgStatus->BdAddr[1],
                                pAgStatus->BdAddr[2],
                                pAgStatus->BdAddr[3],
                                pAgStatus->BdAddr[4],
                                pAgStatus->BdAddr[5]);
            MBT_LOG(msg);
       }
	ADP_OS_MemSet( pAgStatus->BdAddr, 0, sizeof(T_MBT_BDADDR));
}

static void sdc_change_AG_state(T_MBT_AG_STATE newState)
{
    	T_MBT_AG_STATUS *pAGStatus;
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);

       MBT_LOG2("[BTMTK][AG][SDC] sdcAGStatus->AgState : %s => %s",
                        T_MBT_AG_STATE_tostring(pAGStatus->AgState),
                        T_MBT_AG_STATE_tostring(newState));

	pAGStatus->AgState = newState;
}
static void sdc_AG_enable(MBT_BOOL bEnable)
{
    	T_MBT_AG_STATUS *pAGStatus;
        
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);

       MBT_LOG2("[BTMTK][AG][SDC] sdcAGStatus->bEnable : %s => %s",
                                convert_bool_to_string(pAGStatus->bEnabled),
                                convert_bool_to_string(bEnable));

       pAGStatus->bEnabled = bEnable;
}
static void sdc_update_dial_info(const char *digit, T_MBT_AG_DIAL type)
{
	T_MBT_AG_DIALINFO *pAGDialInfo;
       
	pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
       MBT_LOG2("[BTMTK][AG][SDC] sdcDialInfo->DialType : %s => %s",
                        T_MBT_AG_DIAL_tostring(pAGDialInfo->DialType),
                        T_MBT_AG_DIAL_tostring(type));
	pAGDialInfo->DialType = type;
       if(type != MBT_AG_CALL_REDIAL)
       {
           U8 len;
      	    len = ADP_OS_StrLen(digit);
           MBT_LOG2("[BTMTK][AG][SDC] sdcDialInfo->DialNum : %d => %d",
                            pAGDialInfo->DialNum,
                            len);
           pAGDialInfo->Digit[pAGDialInfo->DialNum] = 0;
           MBT_LOG2("[BTMTK][AG][SDC] sdcDialInfo->Digit : \"%s\" => \"%s\"",
                            pAGDialInfo->Digit,
                            digit);
           pAGDialInfo->DialNum = len;
           ADP_OS_StrCpy(pAGDialInfo->Digit, digit);
       }
}
static void sdc_update_phone_call_state(T_MBT_AG_PHONE_CALLSTATE newState)
{
    T_MBT_AG_STATUS *pAGStatus;
    pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGStatus->PhoneCallState : %s => %s",
                T_MBT_AG_PHONE_CALLSTATE_tostring(pAGStatus->PhoneCallState),
                T_MBT_AG_PHONE_CALLSTATE_tostring(newState));
    pAGStatus->PhoneCallState = newState;
}
static void sdc_update_call_state(T_MBT_AG_CALLSTATUS newState)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bCallStatus : %s => %s",
                T_MBT_AG_CALLSTATUS_tostring(pAGIndication->bCallStatus),
                T_MBT_AG_CALLSTATUS_tostring(newState));
    pAGIndication->bCallStatus = newState;
}
static void sdc_update_callsetup_state(T_MBT_AG_CALLSETUP newState)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bCallSetupStatus : %s => %s",
                T_MBT_AG_CALLSETUP_tostring(pAGIndication->bCallSetupStatus),
                T_MBT_AG_CALLSETUP_tostring(newState));
    pAGIndication->bCallSetupStatus = newState;
}
static void sdc_update_callheld_state(T_MBT_AG_CALL_HELD newState)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bHeldStatus : %s => %s",
                T_MBT_AG_CALL_HELD_tostring(pAGIndication->bHeldStatus),
                T_MBT_AG_CALL_HELD_tostring(newState));
    pAGIndication->bHeldStatus = newState;
}

static void sdc_update_Btrh(T_MBT_AG_BTRH val)
{
    T_MBT_AG_BTRH_VAL *pAGBtrhVal;
    pAGBtrhVal = (T_MBT_AG_BTRH_VAL*)mbt_sdc_getrecord(MBTSDC_REC_AG_BTRH);
   MBT_LOG2("[BTMTK][AG][SDC] sdcAGbtrh->BTRHValue : %s => %s",
                    T_MBT_AG_BTRH_tostring(pAGBtrhVal->BTRHValue),
                    T_MBT_AG_BTRH_tostring(val));
    pAGBtrhVal->BTRHValue = val;
}

static void sdc_update_spk_vol_level(MBT_BYTE level)
{
	T_MBT_AG_SPKGAIN *pAGSpkGain;

	pAGSpkGain = (T_MBT_AG_SPKGAIN*)mbt_sdc_getrecord(MBTSDC_REC_AG_SPKGAIN);
       MBT_LOG2("[BTMTK][AG][SDC] T_MBT_AG_SPKGAIN.VolumeLevel : %d => %d",
                    pAGSpkGain->VolumeLevel,
                    level);   
	pAGSpkGain->VolumeLevel = level;
}

static void sdc_update_net_status(T_MBT_AG_NETSTATE newState)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bNetStatus : %s => %s",
                T_MBT_AG_NETSTATE_tostring(pAGIndication->bNetStatus),
                T_MBT_AG_NETSTATE_tostring(newState));
    pAGIndication->bNetStatus = newState;
}
static void sdc_update_signal_level(MBT_BYTE level)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bSignalLevel : %d => %d",
                pAGIndication->bSignalLevel,
                level);
    pAGIndication->bSignalLevel = level;
}
static void sdc_update_roaming_status(MBT_BYTE newState)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bRoamingStatus : %d => %d",
                pAGIndication->bRoamingStatus,
                newState);
    pAGIndication->bRoamingStatus = newState;
}
static void sdc_update_battery_level(MBT_BYTE level)
{
    T_MBT_AG_INDICATION *pAGIndication;
    pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_LOG2("[BTMTK][AG][SDC] sdcAGIndication->bBatteryLevel : %d => %d",
                pAGIndication->bBatteryLevel,
                level);
    pAGIndication->bBatteryLevel = level;
}
/**********************************************************************
*  Callback events handler
***********************************************************************/
void ag_send_auth_event(T_MBT_BDADDR addr, MBT_CHAR *name, MBT_UINT length)
{
	MBT_CHAR *devName;
	MBT_SERVICE_ID svcId = (g_AgContext.activeProfile == HSP_CONTEXT) ? MBT_SVCUUID_HEADSET : MBT_SVCUUID_HF_HANDSFREE;
	//T_MBT_AG_STATUS *pAgStatus;

       // Record the bd address first. We may use the address later.
	//pAgStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	//ADP_OS_MemCopy( pAgStatus->BdAddr, addr, sizeof(T_MBT_BDADDR));
       sdc_update_remote_address(addr);
       
       #ifdef CUSTOM_FOR_WISE
       if(btmtk_gap_is_authorized(addr))
       {
            btmtk_hfag_authorize_res(MBT_TRUE);
            return;
       }
       #endif
       
	devName = (MBT_CHAR*)ADP_OS_MemAlloc(MBT_MAX_NAME_LEN+1);
	if(devName)
		btmtk_gap_get_device_name(addr, devName, MBT_MAX_NAME_LEN+1);
	btmtk_gap_handle_authorize_req(svcId, addr, devName?devName:"");
	if(devName)
		ADP_OS_MemFree(devName);

	ag_postevent(MBTEVT_AG_AUTHREQ, 0);
}

/* Response for update SDC and post event when disconnecting, disconnect success or connect fail */
void ag_send_disconnect_event(T_MBTEVT event)
{
	//T_MBT_AG_STATUS *pAGStatus;
	//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	//pAGStatus->AgState = MBT_AG_STATE_IDLE;
	sdc_change_AG_state(MBT_AG_STATE_IDLE);
	//ADP_OS_MemSet(pAGStatus->BdAddr, 0, sizeof(pAGStatus->BdAddr));
	sdc_clear_remote_address();
	ag_postevent(event, 0);
}
/********************************************************************************
*   Events callback function
********************************************************************************/
void ag_handle_activate_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_ACTIVATE_CNF, index, pParams->status));
	if(pParams->status == BT_STATUS_SUCCESS)
	{
		g_AgContext.state[index] = PROFILE_STATE_ACTIVATED;
		if(g_AgContext.state[index^1] == PROFILE_STATE_IDLE)
		{
			btmtk_hfg_deregister(pContext);
			changeProfileState(index, PROFILE_STATE_DEACTIVATING);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_ACTIVATE_FAIL, index));
		g_AgContext.state[index] = PROFILE_STATE_IDLE;
		ADP_OS_MemFree(g_AgContext.pHfgContext[index]);
		g_AgContext.pHfgContext[index] = NULL;
		if(g_AgContext.state[index^1] == PROFILE_STATE_ACTIVATED)
		{
			btmtk_hfg_deregister(g_AgContext.pHfgContext[index^1]);
			changeProfileState((U8)(index^1), PROFILE_STATE_DEACTIVATING);
		}
	}
	/* Judge if enable is success */
	if(g_AgContext.state[0] == PROFILE_STATE_ACTIVATED &&
	    g_AgContext.state[1] == PROFILE_STATE_ACTIVATED)
	{	/* enable success */
		//T_MBT_AG_STATUS *pAGStatus;
		//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
		//pAGStatus->bEnabled = TRUE;
		sdc_AG_enable(MBT_TRUE);
		//pAGStatus->AgState = MBT_AG_STATE_IDLE;
		sdc_change_AG_state(MBT_AG_STATE_IDLE);
		ag_postevent(MBTEVT_AG_ENABLE_SUCCESS, 0);
		g_AgContext.bEnabled = TRUE;
	}
	if(g_AgContext.state[0] == PROFILE_STATE_IDLE &&
	    g_AgContext.state[1] == PROFILE_STATE_IDLE)
	{	/* Enable failed */
		ag_postevent(MBTEVT_AG_ENABLE_FAIL, 0);
	}
}

void ag_handle_deactivate_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_ACTIVATE_CNF, index));

	ADP_OS_MemFree(g_AgContext.pHfgContext[index]);
	g_AgContext.pHfgContext[index] = NULL;
	changeProfileState(index, PROFILE_STATE_IDLE);
	
	if(g_AgContext.bEnabled)
	{
		if(g_AgContext.state[0] == PROFILE_STATE_IDLE &&
		    g_AgContext.state[1] == PROFILE_STATE_IDLE)
		{
			//T_MBT_AG_STATUS *pAGStatus;
			//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
			//pAGStatus->bEnabled = FALSE;
			sdc_AG_enable(MBT_FALSE);
			ag_postevent(MBTEVT_AG_DISABLE_SUCCESS, 0);
			g_AgContext.bEnabled = FALSE;
		}
	}
	else
	{
		/* when enable failed, btmtk_deregister might be called */
		ag_postevent(MBTEVT_AG_ENABLE_FAIL, 0);
	}
}

void ag_handle_connect_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_CONNECT_CNF, index, pParams->status));

	state = g_AgContext.state[index];
	if(state == PROFILE_STATE_CONNECTING)
	{
		if(pParams->status != BT_STATUS_PENDING)
		{
			T_MBT_AG_STATUS *pAGStatus;
			pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
			if(pAGStatus->AgState == MBT_AG_STATE_CONNECTING)
			{
				/* Update AG state to idle */
				//ADP_OS_MemSet(pAGStatus->BdAddr, 0, sizeof(pAGStatus->BdAddr));
				sdc_clear_remote_address();
				//pAGStatus->AgState = MBT_AG_STATE_IDLE;
				sdc_change_AG_state(MBT_AG_STATE_IDLE);
				ag_postevent(MBTEVT_AG_CONNECT_FAIL, 0);
				/* Change profile state to activated */
				changeProfileState(index, PROFILE_STATE_ACTIVATED);
			}
		else
		{
				AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_AG_STATE, pAGStatus->AgState, MBT_AG_STATE_CONNECTING));
			}
			AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_CONFIRM_FAIL, EVENT_HFG_CONNECT_CONFIRM, pParams->status));
		}
	}
	else
	{
		/* The state may be changed by auth ind */
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, index, state));
	}
}

void ag_handle_accept_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_ACCEPT_CNF, index, pParams->status));
	
	state = g_AgContext.state[index];
	if(state == PROFILE_STATE_ACCEPTING)
	{
		if(pParams->status != BT_STATUS_PENDING)
		{
			T_MBT_AG_STATUS *pAGStatus;
			pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
			if(pAGStatus->AgState == MBT_AG_STATE_IDLE)
			{
				/* Authentication */
				ag_postevent(MBTEVT_AG_CONNECT_FAIL, 0);
			}
			else if(pAGStatus->AgState == MBT_AG_STATE_CONNECTING)
			{
				/* auth ind received just after btmtk_ag_connect with the same address */
				/* Update AG state to idle */
				//ADP_OS_MemSet(pAGStatus->BdAddr, 0, sizeof(pAGStatus->BdAddr));
				sdc_clear_remote_address();
				//pAGStatus->AgState = MBT_AG_STATE_IDLE;
				sdc_change_AG_state(MBT_AG_STATE_IDLE);
				ag_postevent(MBTEVT_AG_CONNECT_FAIL, 0);
			}
		else
		{
				AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_AG_STATE, pAGStatus->AgState, MBT_AG_STATE_IDLE));
			}
			/* Change profile state to activated */
			changeProfileState(index, PROFILE_STATE_ACTIVATED);
			AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_CONFIRM_FAIL, EVENT_HFG_ACCEPT_CHANNEL_CONFIRM, pParams->status));
		}
	}
	else
	{
		/* The state may be changed by auth ind */
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, index, state));
	}
}

void ag_handle_service_connected_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;
       T_MBT_BDADDR newAddr;
	//T_MBT_AG_STATUS *pAGStatus;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_CONNECTED_IND, index));

	state = g_AgContext.state[index];
	if( state != PROFILE_STATE_CONNECTING && state != PROFILE_STATE_ACCEPTING )
	{
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, index, state));
	}
	/* Update  */
	//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	btmtk_util_convert_mbtarray2mtkarray(newAddr, pParams->p.addr->addr);
       sdc_update_remote_address(newAddr);
	//pAGStatus->AgState = MBT_AG_STATE_CONNECTED;
	sdc_change_AG_state(MBT_AG_STATE_CONNECTED);
	ag_postevent(MBTEVT_AG_CONNECT_SUCCESS, 0);
	changeProfileState(index, PROFILE_STATE_CONNECTED);
}

void ag_handle_disconnect_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	/* do nothing */
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_DISCONNECT_CNF, index));
}

void ag_handle_reject_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	/* do nothing */
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_REJECT_CNF, index));
}

void ag_handle_service_disconnected_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;
	//T_MBT_AG_STATUS *pAGStatus;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_DISCONNECTED_IND, index));
	
        //pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);

	//ADP_OS_MemSet(pAGStatus->BdAddr, 0, sizeof(pAGStatus->BdAddr));
	sdc_clear_remote_address();
	state = g_AgContext.state[index];

	/* Final, change profile state to activated */
	changeProfileState(index, PROFILE_STATE_ACTIVATED);

	if(g_AgContext.bDeferedConnReq)
	{
		//pAGStatus->AgState = MBT_AG_STATE_IDLE;
		sdc_change_AG_state(MBT_AG_STATE_IDLE);
		g_AgContext.bDeferedConnReq = MBT_FALSE;
		btmtk_ag_connect(g_AgContext.deferedAddr, g_AgContext.deferedSvcID);
	}
	else
	{
        	switch(state)
        	{
        	case PROFILE_STATE_DISCONNECTING:	/* result of btmtk_ag_disconnect */
        	case PROFILE_STATE_CONNECTED:		/* Remote disconnect the connection */
        	case PROFILE_STATE_DEACTIVATING:		/* Deactivate before connection is closed */	
        		//pAGStatus->AgState = MBT_AG_STATE_IDLE;
        		sdc_change_AG_state(MBT_AG_STATE_IDLE);
        		ag_postevent(MBTEVT_AG_DISCONNECT_SUCCESS, 0);
        		break;
        	case PROFILE_STATE_REJECTING:		/* TODO: don't know we have to response with MBTEVT_AG_CONNECT_FAIL or MBTEVT_AG_DISCONNECT_SUCCESS */		
        	case PROFILE_STATE_CONNECTING:		/* Conencting fail */
        	case PROFILE_STATE_ACCEPTING:		/* This is not documented in help file  */
        	case PROFILE_STATE_AUTHENTICATE:		/* Remote disconnect before user response */		
        		//pAGStatus->AgState = MBT_AG_STATE_IDLE;
        		sdc_change_AG_state(MBT_AG_STATE_IDLE);
        		ag_postevent(MBTEVT_AG_CONNECT_FAIL, 0);
        		break;
        	default:
        		break;
        	}
	}
}

void ag_handle_auth_req_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;
	
	state = g_AgContext.state[index];

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_AUTH_IND, 
										index,
										pParams->p.addr->addr[0],
										pParams->p.addr->addr[1],
										pParams->p.addr->addr[2],
										pParams->p.addr->addr[3],
										pParams->p.addr->addr[4],
										pParams->p.addr->addr[5]));

	if(g_AgContext.state[index^1] != PROFILE_STATE_ACTIVATED)
	{
		AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_AUTO_REJECT, index));
		btmtk_hfg_reject_connect(pContext);
		changeProfileState(index, PROFILE_STATE_AUTO_REJECTING);
	}
	else
	{
		U8 addr[6];
		btmtk_util_convert_mbtarray2mtkarray(addr, pParams->p.addr->addr);

		if(state == PROFILE_STATE_ACTIVATED)
		{
			/* authentication request */
			changeProfileState(index, PROFILE_STATE_AUTHENTICATE);
			ag_send_auth_event(addr, NULL, 0);
		}
		else if(state == PROFILE_STATE_CONNECTING)
		{
			
			/* very little possible that auth ind comes after connect request sent to stack */
			T_MBT_AG_STATUS *pAGStatus;
			pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
			if ( ADP_OS_MemCmp(pAGStatus->BdAddr, addr, sizeof(pAGStatus->BdAddr)) == 0 )
			{	/* Durectly accept the request */
				btmtk_hfg_accept_connect(pContext);
				changeProfileState(index, PROFILE_STATE_ACCEPTING);
			}
			else
			{
				/* response connect fail to btmtk_ag_connect, then report the auth req */
				ag_send_disconnect_event(MBTEVT_AG_CONNECT_FAIL);
				changeProfileState(index, PROFILE_STATE_AUTHENTICATE);
				ag_send_auth_event(addr, NULL, 0);
			}
		}
		else
		{
			AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, index, state));
		}
	}
}

void ag_handle_read_indicators_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* When negotiation, HFP need to know current indicator values */
	T_MBT_AG_INDICATION *pAGIndication;
	HfgReadIndicatorParms indicator;
	
	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
	indicator.ind[HFG_IND_SERVICE].value = pAGIndication->bNetStatus;
	indicator.ind[HFG_IND_CALL].value = pAGIndication->bCallStatus;
	indicator.ind[HFG_IND_CALL_SETUP].value = pAGIndication->bCallSetupStatus;
	indicator.ind[HFG_IND_CALL_HELD].value = pAGIndication->bHeldStatus;
	indicator.ind[HFG_IND_BATTERY].value = pAGIndication->bBatteryLevel;
	indicator.ind[HFG_IND_SIGNAL].value = pAGIndication->bSignalLevel;
	indicator.ind[HFG_IND_ROAMING].value = pAGIndication->bRoamingStatus;
	indicator.num = NUM_OF_HFG_IND;
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_READ_INDICATOR_IND,
										indicator.ind[HFG_IND_SERVICE].value,
										indicator.ind[HFG_IND_CALL].value,
										indicator.ind[HFG_IND_CALL_SETUP].value,
										indicator.ind[HFG_IND_CALL_HELD].value,
										indicator.ind[HFG_IND_BATTERY].value,
										indicator.ind[HFG_IND_SIGNAL].value,
										indicator.ind[HFG_IND_ROAMING].value));
	
	btmtk_hfg_read_indicator(pContext, &indicator);
}

void ag_handle_handsfree_features_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_STATUS *pAGStatus;
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_HF_FEATURE_IND, pParams->p.features));
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGStatus->bRemoteFeature : 0x%04X => 0x%04X", 
                        pAGStatus->bRemoteFeature,
                        (MBT_SHORT)pParams->p.features);
	pAGStatus->bRemoteFeature = (MBT_SHORT)pParams->p.features;
}

void ag_handle_audio_connected_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams);

void ag_handle_sco_connect_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	/* do nothing */
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_SCO_CONNECT_CNF, index, pParams->status));	
	/* Directly call connected ind handler */
        /* If bt stack directly returns fail or success when create sco link, in this case, 
        no connected indication will be received. So we have handle error case here. */
	if(pParams->status != BT_STATUS_PENDING)
		ag_handle_audio_connected_ind(pContext, pParams);
}

void ag_handle_sco_disconnect_cnf(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	/* do nothing */
	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_SCO_DISCONNECT_CNF, index));
}

void ag_handle_audio_connected_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;
	T_MBT_AG_STATUS *pAGStatus;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_AUDIO_CONNECTED_IND, index, pParams->status));

	state = g_AgContext.state[index];
	if(state != PROFILE_STATE_AUDIO_CONNECTING &&
	    state != PROFILE_STATE_CONNECTED )
	{
		/* Unexpected state */
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, index, state));
	}
	if(pParams->status == BT_STATUS_SUCCESS)
	changeProfileState(index, PROFILE_STATE_AUDIO_CONNECTED);
	else
	    changeProfileState(index, PROFILE_STATE_CONNECTED);
		
	/* Update SDC and post event to AP */
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(pAGStatus->AgState == MBT_AG_STATE_AUDIO_CONNECTING || pAGStatus->AgState == MBT_AG_STATE_CONNECTED)
	{
		if(pParams->status == BT_STATUS_SUCCESS)
		{
	              //pAGStatus->AgState = MBT_AG_STATE_AUDIO_CONNECTED;
	              sdc_change_AG_state(MBT_AG_STATE_AUDIO_CONNECTED);
		   	ag_postevent(MBTEVT_AG_AUDIO_CONNECT_SUCCESS,0);
		}
		else
		{
			//pAGStatus->AgState = MBT_AG_STATE_CONNECTED;
			sdc_change_AG_state(MBT_AG_STATE_CONNECTED);
			ag_postevent(MBTEVT_AG_AUDIO_CONNECT_FAIL,0);
		}
	}
}

void ag_handle_audio_disconnected_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	U8 index = (pContext ==g_AgContext.pHfgContext[HSP_CONTEXT]) ? HSP_CONTEXT : HFP_CONTEXT;
	ProfileState state;
	T_MBT_AG_STATUS *pAGStatus;

	AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_HDL_AUDIO_DISCONNECTED_IND, index));	

	state = g_AgContext.state[index];
	if(state != PROFILE_STATE_AUDIO_DISCONNECTING &&
	    state != PROFILE_STATE_AUDIO_CONNECTED &&
	    /* Maybe AP will disconnecting or deactivating before close audio connection */
	    state != PROFILE_STATE_DISCONNECTING &&
	    state != PROFILE_STATE_DEACTIVATING)
	{
		/* Unexpected state */
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, index, state));
	}
	changeProfileState(index, PROFILE_STATE_CONNECTED);
	/* Update SDC and post event to AP */
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
       if(pAGStatus->AgState == MBT_AG_STATE_AUDIO_DISCONNECTING ||
	        pAGStatus->AgState == MBT_AG_STATE_AUDIO_CONNECTED)
       {
               /* change state before post event to make the log in trace tool more readable */
               //pAGStatus->AgState = MBT_AG_STATE_CONNECTED;
               sdc_change_AG_state(MBT_AG_STATE_CONNECTED);
	        ag_postevent(MBTEVT_AG_AUDIO_DISCONNECT_SUCCESS,0);
       }
       else if(pAGStatus->AgState != MBT_AG_STATE_DISCONNECTING)
       {
               //pAGStatus->AgState = MBT_AG_STATE_CONNECTED;
               sdc_change_AG_state(MBT_AG_STATE_CONNECTED);
       }
}

void ag_handle_answer_call_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	btmtk_hfg_send_ok(pContext);
	ag_postevent(MBTEVT_AG_CALL_ANSWER, 0);
}

void ag_handle_dial_number_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	//T_MBT_AG_DIALINFO *pAGDialInfo;
	
	//pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
	//pAGDialInfo->DialType = MBT_AG_CALL_NORMALDIAL;
	//pAGDialInfo->DialNum = ADP_OS_StrLen(pParams->p.phoneNumber);
	//ADP_OS_StrCpy(pAGDialInfo->Digit, pParams->p.phoneNumber);
	sdc_update_dial_info(pParams->p.phoneNumber, MBT_AG_CALL_NORMALDIAL);
    #if !defined(BTMTK_ON_WISE)
	btmtk_hfg_send_ok(pContext);
    #endif
	ag_postevent(MBTEVT_AG_CALL_DIAL, 0);
}

void ag_handle_memory_dial_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
       char strIndex[5];
	//T_MBT_AG_DIALINFO *pAGDialInfo;
	
	//pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
	//pAGDialInfo->DialType = MBT_AG_CALL_MEMDIAL;
	ADP_OS_Sprintf(strIndex, "%d", pParams->p.index);
	//pAGDialInfo->DialNum = ADP_OS_StrLen(pAGDialInfo->Digit);
	sdc_update_dial_info(strIndex, MBT_AG_CALL_MEMDIAL);
    #if !defined(BTMTK_ON_WISE)
	btmtk_hfg_send_ok(pContext);
    #endif
	ag_postevent(MBTEVT_AG_CALL_MEMDIAL, 0);
}

void ag_handle_redial_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	//T_MBT_AG_DIALINFO *pAGDialInfo;
	
	//pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
	//pAGDialInfo->DialType = MBT_AG_CALL_REDIAL;
       sdc_update_dial_info(NULL, MBT_AG_CALL_REDIAL);
    #if !defined(BTMTK_ON_WISE)
	btmtk_hfg_send_ok(pContext);
    #endif
	ag_postevent(MBTEVT_AG_CALL_REDIAL, 0);
}

void ag_handle_call_hold_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_3WAY *pAG3Way;
       MBT_BOOL bIsExtended;

	pAG3Way = (T_MBT_AG_3WAY*)mbt_sdc_getrecord(MBTSDC_REC_AG_3WAYCALL_TYPE);
       MBT_LOG2("[BTMTK][AG][SDC] sdc3WayType->CHLDValue : %d => %d",
                                pAG3Way->CHLDValue,
                                pParams->p.hold->action);
	pAG3Way->CHLDValue = pParams->p.hold->action;
	if(pParams->p.hold->index)
	{
		bIsExtended = MBT_TRUE;
              MBT_LOG2("[BTMTK][AG][SDC] sdc3WayType->CallIndex : %d => %d",
                                pAG3Way->CallIndex,
                                pParams->p.hold->index);
		pAG3Way->CallIndex = pParams->p.hold->index;
	}
	else
	{
		bIsExtended = MBT_FALSE;
	}
       MBT_LOG2("[BTMTK][AG][SDC] sdc3WayType->bIsExtended : %s => %s",
                            convert_bool_to_string(pAG3Way->bIsExtended),
                            convert_bool_to_string(bIsExtended));
       pAG3Way->bIsExtended = bIsExtended;
       
	ag_postevent(MBTEVT_AG_CALL_3WAY, 0);
	#ifdef CUSTOM_FOR_WISE
	// WISE AP seems not send response 
	if(!pAG3Way->bIsExtended)
		btmtk_hfg_send_ok(pContext);
	#endif
}

void ag_handle_query_response_hold_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_STATUS *pAGStatus;
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(pAGStatus->bLocalFeature&MBT_AG_RESPOND_HOLD)
	{
		g_AgContext.bBtrh = MBT_TRUE;
		ag_postevent(MBTEVT_AG_BTRH_READ, 0);
	}
	else
	{
		btmtk_hfg_send_error(pContext, MBT_AG_CME_ERR_OPERATION_NOT_SUPPORTED);
	}
}

void ag_handle_response_hold_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_STATUS *pAGStatus;
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(pAGStatus->bLocalFeature&MBT_AG_RESPOND_HOLD)
	{
		//T_MBT_AG_BTRH_VAL *pAGBtrhVal;
		//pAGBtrhVal = (T_MBT_AG_BTRH_VAL*)mbt_sdc_getrecord(MBTSDC_REC_AG_BTRH);
		//pAGBtrhVal->BTRHValue = pParams->p.respHold;
		sdc_update_Btrh(pParams->p.respHold);
		ag_postevent(MBTEVT_AG_BTRH_SET, 0);
	}
	else
	{
		btmtk_hfg_send_error(pContext, MBT_AG_CME_ERR_OPERATION_NOT_SUPPORTED);
	}
}

void ag_handle_hangup_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_INDICATION *pAGIndication;
	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
	if(pAGIndication->bCallSetupStatus == MBT_AG_CALLSETUP_INCOMING)
	{
		ag_postevent(MBTEVT_AG_CALL_REJECT, 0);
	}
	else
	{
		ag_postevent(MBTEVT_AG_CALL_END, 0);
}
	btmtk_hfg_send_ok(pContext);
}

void ag_handle_list_current_calls_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_CALLLIST, 0);
}

void ag_handle_enable_caller_id_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* discard this, HFG profile will handle this */
	/* If callerid is not enabled, CLIP request will be rejected by profile */
	btmtk_hfg_send_ok(pContext);
}

void ag_handle_enable_call_waiting_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* discard this, HFG profile will handle this */
	/* If call waiting notify is not enabled, call waiting notify request will be rejected by profile */
	btmtk_hfg_send_ok(pContext);
}

void ag_handle_generate_dtmf_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_DTMF *pAGDtmf;
	pAGDtmf = (T_MBT_AG_DTMF*)mbt_sdc_getrecord(MBTSDC_REC_AG_DTMF_TYPE);
       MBT_LOG1("[BTMTK][AG][SDC] sdcDTMFType->Digit : %c",
                //pAGDtmf->Digit,
                pParams->p.dtmf);
	pAGDtmf->Digit = pParams->p.dtmf;
	btmtk_hfg_send_ok(pContext);
	ag_postevent(MBTEVT_AG_CALL_DTMF, 0);
}

void ag_handle_get_last_voice_tag_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* WISE is not support this */
	btmtk_hfg_send_error(pContext, MBT_AG_CME_ERR_OPERATION_NOT_SUPPORTED);
}

void ag_handle_enable_voice_recognition_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	if(pParams->p.enabled)
	{
		g_AgContext.bVR = TRUE;
	ag_postevent(MBTEVT_AG_VR_ACT, 0);
	}
	else
	{
		g_AgContext.bVR = FALSE;
		ag_postevent(MBTEVT_AG_VR_DEACT, 0);
	}
	btmtk_hfg_send_ok(pContext);
}

void ag_handle_disable_nrec_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	btmtk_hfg_send_error(pContext, 0);
	/* ag_postevent(MBTEVT_AG_VR_DEACT, 0); */
}

void ag_handle_report_mic_volume_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* NOt supported by WISE */
	//btmtk_hfg_send_error(pContext, 0);
}

void ag_handle_report_spk_volume_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	//T_MBT_AG_SPKGAIN *pAGSpkGain;

	//pAGSpkGain = (T_MBT_AG_SPKGAIN*)mbt_sdc_getrecord(MBTSDC_REC_AG_SPKGAIN);
	//pAGSpkGain->VolumeLevel = pParams->p.gain;
	sdc_update_spk_vol_level(pParams->p.gain);
	ag_postevent(MBTEVT_AG_SPK, 0);
}

void ag_handle_query_network_operator_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_OPERATOR, 0);
}

void ag_handle_query_subscriber_number_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_SUBSCRIBER, 0);
}

void ag_handle_enable_extended_error_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	btmtk_hfg_send_ok(pContext);
}

void ag_handle_keypad_control_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_STATUS *pAGStatus;
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(pAGStatus->bNonDefaultMultiFuncKey)
	{
		ag_postevent(MBTEVT_AG_HS_BUTTON_PRESSED, 0);
	}
	else
	{	/* MBT default scenario */
		T_MBT_AG_INDICATION *pAGIndication;
		pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
		if(pAGIndication->bCallSetupStatus == MBT_AG_CALLSETUP_INCOMING)
		{
			if(pAGIndication->bCallStatus != MBT_AG_CALLSTATUS_ACTIVE)
			{	/* incoming call */
				ag_postevent(MBTEVT_AG_CALL_ANSWER, 0);
			}
		}
		else
		{
			if(pAGIndication->bCallSetupStatus == MBT_AG_CALLSETUP_ORIGINATING ||
			    pAGIndication->bCallSetupStatus == MBT_AG_CALLSETUP_ALERTING ||
			    (pAGIndication->bCallStatus == MBT_AG_CALLSTATUS_ACTIVE && pAGIndication->bHeldStatus != MBT_AG_HELD_NO_ACTIVE_CALL)
			    )
			{
				if(g_AgContext.state[g_AgContext.activeProfile] == PROFILE_STATE_AUDIO_CONNECTED)
				{
					ag_postevent(MBTEVT_AG_CALL_END, 0);
				}
				else if(g_AgContext.state[g_AgContext.activeProfile] == PROFILE_STATE_CONNECTED)
				{
					btmtk_hfg_create_audio_link(pContext);
				}
			}
			else if(pAGIndication->bCallStatus == MBT_AG_CALLSTATUS_END && 
				pAGIndication->bCallSetupStatus == MBT_AG_CALLSETUP_IDLE)
			{
				if(g_AgContext.bVR)
				{
					g_AgContext.bVR = FALSE;
					ag_postevent(MBTEVT_AG_VR_DEACT, 0);
				}
				else
				{
					g_AgContext.bVR = TRUE;
					ag_postevent(MBTEVT_AG_VR_ACT, 0);
				}
			}
		}
	}
}

void ag_handle_query_supported_charset_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_CSCS *pAGCscs;
	MBT_CHAR *pCscs, *pTmp;
	MBT_BYTE i, len;

	pAGCscs = (T_MBT_AG_CSCS*)mbt_sdc_getrecord(MBTSDC_REC_AG_CSCS);
	pCscs = (MBT_CHAR*)ADP_OS_MemAlloc(MBT_AG_CSCS_NUM*MBT_AG_CSCS_MAXLEN);
	if(pCscs)
	{
		pTmp = pCscs;
		*(pTmp++) = '(';
		for(i = 0;i < MBT_AG_CSCS_NUM; i++)
		{
			len = ADP_OS_StrLen(pAGCscs->AGCharSetList[i]);
			if(len > 0)
			{
				if(i > 0)
					*(pTmp++) = ',';
				if(pAGCscs->AGCharSetList[i][0] != '\"')
				*(pTmp++) = '\"';
				ADP_OS_StrCpy(pTmp, pAGCscs->AGCharSetList[i]);
				pTmp += len;
				if(pAGCscs->AGCharSetList[i][len-1] != '\"')
				*(pTmp++) = '\"';
			}
		}
		*(pTmp++) = ')';
		*pTmp = 0;
		btmtk_hfg_supported_charset_response(pContext, pCscs);
		ADP_OS_MemFree(pCscs);	
		btmtk_hfg_send_ok(pContext);
	}
	else
	{
		btmtk_hfg_send_error(pContext, 0);
	}
}

void ag_handle_query_selected_charset_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_CSCS *pAGCscs;

	pAGCscs = (T_MBT_AG_CSCS*)mbt_sdc_getrecord(MBTSDC_REC_AG_CSCS);
	btmtk_hfg_selected_charset_response(pContext, pAGCscs->CurrentCharSet);
	btmtk_hfg_send_ok(pContext);
}

void ag_handle_select_charset_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	#ifdef CUSTOM_FOR_WISE
	MBT_BYTE i, csLen, len, cmp=0xFF;
	#endif
	T_MBT_AG_CSCS *pAGCscs;

	pAGCscs = (T_MBT_AG_CSCS*)mbt_sdc_getrecord(MBTSDC_REC_AG_CSCS);
	#ifdef CUSTOM_FOR_WISE
	// Check supported list first
	csLen = ADP_OS_StrLen(pParams->p.charset);
	if(csLen <= MBT_AG_CSCS_MAXLEN)
	{
		for(i = 0;i < MBT_AG_CSCS_NUM; i++)
		{
			len = ADP_OS_StrLen(pAGCscs->AGCharSetList[i]);
			if(len > 0)
			{	
				if(pAGCscs->AGCharSetList[i][0] != '\"')
					cmp = ADP_OS_MemCmp(pParams->p.charset, pAGCscs->AGCharSetList[i], csLen);
				else
					cmp = ADP_OS_MemCmp(pParams->p.charset,&pAGCscs->AGCharSetList[i][1], csLen);
			}
			if(len ==0 || cmp == 0)
				break;
		}
	}
	if(cmp != 0)
	{
		btmtk_hfg_send_error(pContext, 0);
		return;
	}
	#endif
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGcscs->HFCharSet : %s => %s",
                    pAGCscs->HFCharSet,
                    pParams->p.charset);  
	ADP_OS_StrCpy(pAGCscs->HFCharSet, pParams->p.charset);
	ag_postevent(MBTEVT_AG_CSCS_SET, 0);
}

void ag_handle_query_model_id_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_CGM_ID *pAGCgmId;

	pAGCgmId = (T_MBT_AG_CGM_ID*)mbt_sdc_getrecord(MBTSDC_REC_AG_CGM);
    if(pAGCgmId->modelID[0])
	btmtk_hfg_model_id(pContext, pAGCgmId->modelID);
    else
        btmtk_hfg_send_ok(pContext);
}

void ag_handle_query_manufacture_id_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_CGM_ID *pAGCgmId;

	pAGCgmId = (T_MBT_AG_CGM_ID*)mbt_sdc_getrecord(MBTSDC_REC_AG_CGM);
    if(pAGCgmId->manufacturerID[0])
	btmtk_hfg_manufacture_id(pContext, pAGCgmId->manufacturerID);
    else
        btmtk_hfg_send_ok(pContext);
}

void ag_handle_query_supported_phonebook_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_PB_CPBS_TEST, 0);
}

void ag_handle_select_phonebook_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_PB_CPBS *pAGPbCpbs;
	T_MBT_AG_PB_DEV pbType;
	
	pAGPbCpbs = (T_MBT_AG_PB_CPBS*)mbt_sdc_getrecord(MBTSDC_REC_AG_RXCPBS);
	switch(pParams->p.pbStorage)
	{
	case HFG_PBS_ME_PHONEBOOK:
		pbType = MBT_AG_PB_DEV_ME;
		break;
	case HFG_PBS_DIALED_CALLS:
		pbType = MBT_AG_PB_DEV_DC;
		break;
	case HFG_PBS_MISSED_CALLS:
		pbType = MBT_AG_PB_DEV_MC;
		break;
	case HFG_PBS_RECEIVED_CALLS:
		pbType = MBT_AG_PB_DEV_RC;
		break;
	case HFG_PBS_SIM_PHONEBOOK:
		pbType = MBT_AG_PB_DEV_SM;
		break;
	case HFG_PBS_FIXED_DIAL:
		pbType = MBT_AG_PB_DEV_FD;
		break;
	case HFG_PBS_LAST_DIAL:
		pbType = MBT_AG_PB_DEV_LD;
		break;
	case HFG_PBS_ME_SIM_COMBINED:
		pbType = MBT_AG_PB_DEV_MT;
		break;
	default:
		btmtk_hfg_send_error(pContext, 0);
		return;
	}

	#ifdef CUSTOM_FOR_WISE
	// Check if the selected type is supported for WISE MMI
	#if 0
	{
		MBT_BYTE i;
		for(i = 0;i < pAGPbCpbs->bSupportedNum;i++)
		{
			if(pbType == pAGPbCpbs->pbType[i])
				break;
		}
	}
	if(i == pAGPbCpbs->bSupportedNum)
	{
		btmtk_hfg_send_error(pContext, 0);
		return;
	}
	#endif
	#endif
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbs->selectedPbType : %s => %s",
            T_MBT_AG_PB_DEV_tostring(pAGPbCpbs->selectedPbType),
            T_MBT_AG_PB_DEV_tostring(pbType));
	pAGPbCpbs->selectedPbType = pbType;
	ag_postevent(MBTEVT_AG_PB_CPBS_SET, 0);
}

void ag_handle_query_selected_phonebook_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_PB_CPBS_READ, 0);
}

void ag_handle_query_read_phonebook_info_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_PB_CPBR_TEST, 0);
}

void ag_handle_read_phonebook_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_PB_CPBR *pAGCpbr;
	pAGCpbr = (T_MBT_AG_PB_CPBR*)mbt_sdc_getrecord(MBTSDC_REC_AG_RXCPBR);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbr->bStartIdx : %d => %d",
            pAGCpbr->bStartIdx,
            pParams->p.pbRead->first);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbr->bEndIdx : %d => %d",
            pAGCpbr->bEndIdx,
            pParams->p.pbRead->last);
	pAGCpbr->bStartIdx = pParams->p.pbRead->first;
	pAGCpbr->bEndIdx = pParams->p.pbRead->last;
	ag_postevent(MBTEVT_AG_PB_CPBR_SET, 0);
}

void ag_handle_query_find_phonebook_info_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_PB_CPBF_TEST, 0);
}

void ag_handle_find_phonebook_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_PB_CPBF *pAGCpbf;
	pAGCpbf = (T_MBT_AG_PB_CPBF*)mbt_sdc_getrecord(MBTSDC_REC_AG_RXCPBF);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbf ->findText : \"%s\" => \"%s\"",
            pAGCpbf->findText,
            pParams->p.pbFind->text);
	ADP_OS_StrCpy(pAGCpbf->findText, pParams->p.pbFind->text);
	ag_postevent(MBTEVT_AG_PB_CPBF_SET, 0);
}

void ag_handle_query_write_phonebook_info_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_PB_CPBW_TEST, 0);
}

void ag_handle_write_phonebook_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_PB_CPBW *pAGCpbw;
       MBT_BOOL erase;
	pAGCpbw = (T_MBT_AG_PB_CPBW*)mbt_sdc_getrecord(MBTSDC_REC_AG_RXCPBW);
	if(pParams->p.pbWrite->number == NULL)
	{
		erase = MBT_TRUE;
	}
	else
	{
		erase = MBT_FALSE;
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbw->bType : %d => %d",
                    pAGCpbw->bType,
                    pParams->p.pbWrite->type);
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbw->index : %d => %d",
                    pAGCpbw->index,
                    pParams->p.pbWrite->index);
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbw->phoneNum : %s => %s",
                    pAGCpbw->phoneNum,
                    pParams->p.pbWrite->number);
		pAGCpbw->bType = pParams->p.pbWrite->type;
		pAGCpbw->index = pParams->p.pbWrite->index;
		ADP_OS_StrCpy(pAGCpbw->phoneNum, pParams->p.pbWrite->number);
		if(pParams->p.pbWrite->text)
             {      
                     MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbw->text : %s => %s",
                        pAGCpbw->text,
                        pParams->p.pbWrite->text);
			ADP_OS_StrCpy(pAGCpbw->text, pParams->p.pbWrite->text);
             }
	}
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGPBRXcpbw->erase : %s => %s",
            convert_bool_to_string(pAGCpbw->erase),
            convert_bool_to_string(erase));
       pAGCpbw->erase = erase;
	ag_postevent(MBTEVT_AG_PB_CPBW_SET, 0);
}

void ag_handle_query_supported_sms_service_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	MBT_CHAR pSvc[6];
	MBT_BYTE i = 0;
	
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	pSvc[i++] = '(';
	if(pAGSmsStatus->CSMS.MsgSvc & 0x0F)
	{
		pSvc[i++] = '0';
		if(pAGSmsStatus->CSMS.MsgSvc & 0xF0)
		{
			pSvc[i++] = ',';
			pSvc[i++] = '1';
		}
	}
	else if(pAGSmsStatus->CSMS.MsgSvc & 0xF0)
	{
		pSvc[i++] = '1';
	}
	pSvc[i++] = ')';
	pSvc[i++] = 0;
	btmtk_hfg_send_sms_supported_service_rsp(pContext, pSvc);
}

void ag_handle_query_selected_sms_service_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	HfgSMSService_read read;
	T_MBT_AG_SM_SVC svc;
		
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	svc = pAGSmsStatus->CSMS.CurrentMsgSvc;
	read.service = (svc & MBT_AG_SM_SVC_0) ? 0 : 1;
	svc = pAGSmsStatus->CSMS.MsgSvc;
	if(read.service)
		svc = (svc>>4);
	read.mt = (svc & MBT_AG_SM_SVC_0_MT) ? 1 : 0; 
	read.mo = (svc & MBT_AG_SM_SVC_0_MO) ? 1 : 0; 
	read.bm = (svc & MBT_AG_SM_SVC_0_BM) ? 1 : 0; 
	btmtk_hfg_send_sms_selected_service_rsp(pContext, &read);
}

void ag_handle_select_sms_service_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);

	if(pParams->p.service == 0 && (pAGSmsStatus->CSMS.MsgSvc & MBT_AG_SM_SVC_0))
	{
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMS.SelectedMsgSvc : 0x%02X => 0x%02X",
                    pAGSmsStatus->CSMS.SelectedMsgSvc,
                    MBT_AG_SM_SVC_0);
		pAGSmsStatus->CSMS.SelectedMsgSvc = MBT_AG_SM_SVC_0;
	}
	else if(pParams->p.service == 1 && (pAGSmsStatus->CSMS.MsgSvc & MBT_AG_SM_SVC_1))
	{
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMS.SelectedMsgSvc : 0x%02X => 0x%02X",
                    pAGSmsStatus->CSMS.SelectedMsgSvc,
                    MBT_AG_SM_SVC_1);
		pAGSmsStatus->CSMS.SelectedMsgSvc = MBT_AG_SM_SVC_1;
	}
	else
	{
		btmtk_hfg_send_error(pContext, 0);
		return;
	}
	ag_postevent(MBTEVT_AG_SM_CSMS_SET, 0);
}

void ag_handle_query_supported_pref_msg_storage_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	HfgSMSPrefStorage_test test;
	HfgSMSStorage *pSto;
	MBT_BYTE i,j;
	
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	pSto = &test.read;
	for(i = 0;i < 3;i++)
	{
		*pSto = 0;
		for(j = 0;j < pAGSmsStatus->CPMS.SupportedNum[i];j++)
		{
			*pSto |= (1<<pAGSmsStatus->CPMS.MemType[i][j]);
		}
		pSto += 1;
	}
	btmtk_hfg_send_sms_supported_pref_storage_rsp(pContext, &test);
}

void ag_handle_query_selected_pref_msg_storage_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_SM_CPMS_READ, 0);
}

static MBT_BOOL isSupportedStorageType(U8 idx, T_MBT_AG_SM_STORAGE type)
{
    U8 i;
    T_MBT_AG_SMS_STATUS *pAGSmsStatus;
    pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
    for(i = 0;i < pAGSmsStatus->CPMS.SupportedNum[idx];i++)
    {
        if(type == pAGSmsStatus->CPMS.MemType[idx][i])
            return MBT_TRUE;
    }
    return MBT_FALSE;
}

void ag_handle_select_pref_msg_storage_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       T_MBT_AG_SM_STORAGE oldStorage;
	HfgSMSStorage *pSto;
	MBT_BYTE i;
	
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	pSto = &pParams->p.prefStorage->read;
       /* Customize for LG request */
       g_AgContext.prefMemStorageMask = (READ_MEM_MASK|WRITE_MEM_MASK|RECV_MEM_MASK);
	for(i = 0;i < 3;i++)
	{
              oldStorage = pAGSmsStatus->CPMS.SelectedMemType[i];
		switch(*pSto)
		{
		case HFG_SMS_STORAGE_BM:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_BM;
			break;
		case HFG_SMS_STORAGE_ME:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_ME;
			break;
		case HFG_SMS_STORAGE_MT:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_MT;
			break;
		case HFG_SMS_STORAGE_SM:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_SM;
			break;
		case HFG_SMS_STORAGE_TA:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_TA;
			break;
		case HFG_SMS_STORAGE_SR:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_SR;
			break;
		case HFG_SMS_STORAGE_UNKNOWN:
		default:
			pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_MAX;
                     /* Customize for LG request */
                     g_AgContext.prefMemStorageMask &= ~(1<<i);
			break;
		}
              /* If not supported type. Filter out in PI layer. */
              if(pAGSmsStatus->CPMS.SelectedMemType[i] != MBT_AG_SM_MEM_MAX &&
                !isSupportedStorageType(i, pAGSmsStatus->CPMS.SelectedMemType[i]))
              {
                    pAGSmsStatus->CPMS.SelectedMemType[i] = MBT_AG_SM_MEM_MAX;
                     /* Customize for LG request */
                     g_AgContext.prefMemStorageMask &= ~(1<<i);
              }
               MBT_LOG3("[BTMTK][AG][SDC] sdcAGSMS->CPMS.SelectedMemType[%d] : %s => %s",
                    i,
                    g_strSMStorage[oldStorage],
                    g_strSMStorage[pAGSmsStatus->CPMS.SelectedMemType[i]]);
              
		pSto++;
	}
	ag_postevent(MBTEVT_AG_SM_CPMS_SET, 0);
}

void ag_handle_query_supported_msg_format_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	MBT_CHAR pFormat[6];
	MBT_BYTE i,j;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	i = 0;
	pFormat[i++] = '(';
	for(j = 0;j < MBT_AG_SM_CMGF_NUM;j++)
	{
		if(j > 0)
			pFormat[i++] = ',';
		pFormat[i++] = pAGSmsStatus->CMGF.MsgFmtList[j]+'0';
	}
	pFormat[i++] = ')';
	pFormat[i++] = 0;
	btmtk_hfg_send_sms_supported_format_rsp(pContext, pFormat);
}

void ag_handle_query_selected_msg_format_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	btmtk_hfg_send_sms_selected_format_rsp(pContext, (U8)pAGSmsStatus->CMGF.CurrentMode);
}

void ag_handle_select_msg_format_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGF.SelectedMode : %s => %s",
                    g_strSMFormat[pAGSmsStatus->CMGF.SelectedMode],
                    g_strSMFormat[pParams->p.format]);
	pAGSmsStatus->CMGF.SelectedMode = pParams->p.format;
    
	ag_postevent(MBTEVT_AG_SM_CMGF_SET, 0);
}

void ag_handle_query_service_centre_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_SM_CSCA_READ, 0);
}

void ag_handle_set_service_centre_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       MBT_SHORT oldOptVal;
       MBT_BYTE addrLen;
       
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       addrLen = ADP_OS_StrLen(pParams->p.serviceCentre->sca);
       // log SDC update
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSCA.AddrLen : %d => %d",
            pAGSmsStatus->CSCA.AddrLen,
            addrLen);
       pAGSmsStatus->CSCA.SMSCAddr[pAGSmsStatus->CSCA.AddrLen] = 0;
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSCA.SMSCAddr : \"%s\" => \"%s\"",
            pAGSmsStatus->CSCA.SMSCAddr,
            pParams->p.serviceCentre->sca);
       pAGSmsStatus->CSCA.AddrLen = addrLen;
	ADP_OS_MemCopy(pAGSmsStatus->CSCA.SMSCAddr, pParams->p.serviceCentre->sca, pAGSmsStatus->CSCA.AddrLen);
	pAGSmsStatus->CSCA.SMSCAddr[pAGSmsStatus->CSCA.AddrLen] = 0;
       oldOptVal = pAGSmsStatus->CSCA.OptValue;
	if(pParams->p.serviceCentre->mask & HFG_SMS_MASK_TOSCA)
	{
		pAGSmsStatus->CSCA.OptValue = MBT_AG_SM_OPT_TOSCA;
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSCA.AddrType : %d => %d",
                pAGSmsStatus->CSCA.AddrType,
                pParams->p.serviceCentre->tosca);        
		pAGSmsStatus->CSCA.AddrType = pParams->p.serviceCentre->tosca;
	}
	else
	{
		pAGSmsStatus->CSCA.OptValue = 0;
	}
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSCA.OptValue : 0x%04X => 0x%04X",
                oldOptVal,
                pAGSmsStatus->CSCA.OptValue);   
	ag_postevent(MBTEVT_AG_SM_CSCA_SET, 0);
}

void ag_handle_query_text_mode_params_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_SM_CSMP_READ, 0);
}

void ag_handle_set_text_mode_params_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       MBT_SHORT oldOptVal;
        
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       oldOptVal = pAGSmsStatus->CSMP.OptValue;
	pAGSmsStatus->CSMP.OptValue = 0;
	if(pParams->p.textParams->mask & HFG_SMS_MASK_FO)
	{
		pAGSmsStatus->CSMP.OptValue |= MBT_AG_SM_OPT_FO;
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMP.FirstOctet : %d => %d",
                pAGSmsStatus->CSMP.FirstOctet,
                pParams->p.textParams->fo);        
		pAGSmsStatus->CSMP.FirstOctet = pParams->p.textParams->fo;
	}
	if(pParams->p.textParams->mask & HFG_SMS_MASK_VP)
	{
		pAGSmsStatus->CSMP.OptValue |= MBT_AG_SM_OPT_VP;
		if(pParams->p.textParams->strVp)
              {     
                     MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMP.TsVP : \"%s\" => \"%s\"",
                        pAGSmsStatus->CSMP.TsVP,
                        pParams->p.textParams->strVp);
			ADP_OS_StrCpy(pAGSmsStatus->CSMP.TsVP, pParams->p.textParams->strVp);
              }
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMP.IntVP : %d => %d",
                pAGSmsStatus->CSMP.IntVP,
                pParams->p.textParams->intVp);
		pAGSmsStatus->CSMP.IntVP= pParams->p.textParams->intVp;
	}
	if(pParams->p.textParams->mask & HFG_SMS_MASK_PID)
	{
		pAGSmsStatus->CSMP.OptValue |= MBT_AG_SM_OPT_PID;
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMP.PID : %d => %d",
                pAGSmsStatus->CSMP.PID,
                pParams->p.textParams->pid);
		pAGSmsStatus->CSMP.PID = pParams->p.textParams->pid;
	}
	if(pParams->p.textParams->mask & HFG_SMS_MASK_DCS)
	{
		pAGSmsStatus->CSMP.OptValue |= MBT_AG_SM_OPT_DCS;
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMP.DCS : %d => %d",
                pAGSmsStatus->CSMP.DCS,
                pParams->p.textParams->dcs);
		pAGSmsStatus->CSMP.DCS = pParams->p.textParams->dcs;
	}
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMP.OptValue : 0x%04X => 0x%04X",
                oldOptVal,
                pAGSmsStatus->CSMP.OptValue);
	ag_postevent(MBTEVT_AG_SM_CSMP_SET, 0);
}

void ag_handle_query_supported_show_params_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	MBT_CHAR pShow[6];
	MBT_BYTE i,j;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	i = 0;
	pShow[i++] = '(';
	for(j = 0;j < MBT_AG_SM_CSDH_NUM;j++)
	{
		if(j > 0)
			pShow[i++] = ',';
		pShow[i++] = pAGSmsStatus->CSDH.ShowList[j]+'0';
	}
	pShow[i++] = ')';
	pShow[i++] = 0;
	btmtk_hfg_send_sms_supported_show_params_rsp(pContext, pShow);
}

void ag_handle_query_selected_show_params_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	btmtk_hfg_send_sms_selected_show_params_rsp(pContext, (U8)pAGSmsStatus->CSDH.CurrentShow);
}

void ag_handle_set_show_params_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSDH.SelectedShow : %s => %s",
                g_strSMShow[pAGSmsStatus->CSDH.SelectedShow],
                g_strSMShow[pParams->p.show]);
	pAGSmsStatus->CSDH.SelectedShow = pParams->p.show;
	ag_postevent(MBTEVT_AG_SM_CSDH_SET, 0);
}

void ag_handle_query_supported_new_msg_indication_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);	
	btmtk_hfg_send_sms_supported_new_msg_indication_rsp(pContext, pAGSmsStatus->CNMI.Setting);
}

void ag_handle_query_selected_new_msg_indication_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	HfgSMSIndSetting_read read;
	
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);	
	read.mode = pAGSmsStatus->CNMI.CurrentSet.Mode;
	read.mt = pAGSmsStatus->CNMI.CurrentSet.Mt;
	read.bm = pAGSmsStatus->CNMI.CurrentSet.Bm;
	read.ds = pAGSmsStatus->CNMI.CurrentSet.Ds;
	read.bfr = pAGSmsStatus->CNMI.CurrentSet.Bfr;
	btmtk_hfg_send_sms_selected_new_msg_indication_rsp(pContext, &read);
}

void ag_handle_set_new_msg_indication_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);	
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.SelectedSet.Mode : %d => %d",
            pAGSmsStatus->CNMI.SelectedSet.Mode,
            pParams->p.newMsgInd->mode);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.SelectedSet.Mt : %d => %d",
            pAGSmsStatus->CNMI.SelectedSet.Mt,
            pParams->p.newMsgInd->mt);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.SelectedSet.Bm : %d => %d",
            pAGSmsStatus->CNMI.SelectedSet.Bm,
            pParams->p.newMsgInd->bm);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.SelectedSet.Ds : %d => %d",
            pAGSmsStatus->CNMI.SelectedSet.Ds,
            pParams->p.newMsgInd->ds);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.SelectedSet.Bfr : %d => %d",
            pAGSmsStatus->CNMI.SelectedSet.Bfr,
            pParams->p.newMsgInd->bfr);
	pAGSmsStatus->CNMI.SelectedSet.Mode = pParams->p.newMsgInd->mode;
	pAGSmsStatus->CNMI.SelectedSet.Mt = pParams->p.newMsgInd->mt;
	pAGSmsStatus->CNMI.SelectedSet.Bm = pParams->p.newMsgInd->bm;
	pAGSmsStatus->CNMI.SelectedSet.Ds = pParams->p.newMsgInd->ds;
	pAGSmsStatus->CNMI.SelectedSet.Bfr = pParams->p.newMsgInd->bfr;
	ag_postevent(MBTEVT_AG_SM_CNMI_SET, 0);
}

void ag_handle_query_supported_list_status_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	ag_postevent(MBTEVT_AG_SM_CMGL_TEST, 0);
}

void ag_handle_list_msg_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);	
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGLStatus : %d => %d",
            pAGSmsStatus->CMGLStatus,
            pParams->p.stat);       
	pAGSmsStatus->CMGLStatus = pParams->p.stat;
	ag_postevent(MBTEVT_AG_SM_CMGL_SET, 0);
}

void ag_handle_read_msg_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);	
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGRIdx : %d => %d",
            pAGSmsStatus->CMGRIdx,
            pParams->p.readMsgIndex);
	pAGSmsStatus->CMGRIdx = pParams->p.readMsgIndex;
	ag_postevent(MBTEVT_AG_SM_CMGR_SET, 0);
}

void ag_handle_send_msg_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       T_MBT_AG_CMS_ERR err = 0;
       MBT_SHORT oldOptVal;
       MBT_SHORT oldMsgLen;

	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);	
       oldMsgLen = pAGSmsStatus->CMGS.MsgLen;
       oldOptVal = pAGSmsStatus->CMGS.OptValue;
       pAGSmsStatus->CMGS.MsgLen = ADP_OS_StrLen(pParams->p.sendMsg->msg);
       pAGSmsStatus->CMGS.OptValue = 0;
       if(pAGSmsStatus->CMGF.CurrentMode == MBT_AG_SM_MGF_PDU)
       {
              /* PDU mode */
        	if(pParams->p.sendMsg->mask & HFG_SMS_MASK_LENGTH)
        	{
        		/* PDU mode */
                     MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGS.PduLen : %d => %d",
                            pAGSmsStatus->CMGS.PduLen,
                            pParams->p.sendMsg->pduLen);
        		pAGSmsStatus->CMGS.PduLen = pParams->p.sendMsg->pduLen;
                      pAGSmsStatus->CMGS.MsgLen /= 2;
                	StringToPdu(pParams->p.sendMsg->msg, pAGSmsStatus->CMGS.Msg, pAGSmsStatus->CMGS.MsgLen);
              }
              else
              {
                    err = MBT_AG_CMS_ERR_INVALID_PDU_PARAM;
              }
	}
	else
	{
		/* Text mode */
              // Copy destination address
              if(pParams->p.sendMsg->destAddr)
              {
                     MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGS.DestAddr : \"%s\" => \"%s\"",
                            pAGSmsStatus->CMGS.DestAddr,
                            pParams->p.sendMsg->destAddr);
                     ADP_OS_StrCpy(pAGSmsStatus->CMGS.DestAddr, pParams->p.sendMsg->destAddr);
        		if(pParams->p.sendMsg->mask & HFG_SMS_MASK_TOADDR)
        		{
        			pAGSmsStatus->CMGS.OptValue = MBT_AG_SM_OPT_TOADDR;
                           MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGS.AddrType : %d => %d",
                                pAGSmsStatus->CMGS.AddrType,
                                pParams->p.sendMsg->addrType);                    
        			pAGSmsStatus->CMGS.AddrType = pParams->p.sendMsg->addrType;
        		}
                          ADP_OS_StrCpy(pAGSmsStatus->CMGS.Msg, pParams->p.sendMsg->msg);                  
        	}
              else
              {
                  err = MBT_AG_CMS_ERR_INVALID_TEXT_PARAM;
              }
	}

       if(!err)
	{
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGS.MsgLen : %d => %d",
                    oldMsgLen,
                    pAGSmsStatus->CMGS.MsgLen);
              MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGS.OptValue : 0x%04X => 0x%04X",
                    oldOptVal,
                    pAGSmsStatus->CMGS.OptValue);
		ag_postevent(MBTEVT_AG_SM_CMGS_SET, 0);
	}
       else
	{
	    pAGSmsStatus->CMGS.MsgLen = oldMsgLen;
           pAGSmsStatus->CMGS.OptValue = oldOptVal;
           btmtk_hfg_send_sms_error(pContext, (U16)err);
	}
}

void ag_handle_send_stored_msg_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* Not supported */
	btmtk_hfg_send_sms_error(pContext, MBT_AG_CMS_ERR_OP_NOT_SUPPORTED);
}

void ag_handle_write_msg_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* Not supported by WISE */
	btmtk_hfg_send_sms_error(pContext, MBT_AG_CMS_ERR_OP_NOT_SUPPORTED);
}

void ag_handle_delete_msg_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* Not supported by WISE */
	btmtk_hfg_send_sms_error(pContext, MBT_AG_CMS_ERR_OP_NOT_SUPPORTED);
}

void ag_handle_response_complete_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	/* Do nothing */
}

#ifdef DELAY_CALL_STATE_CHANGE
void ag_handle_delay_call_state_change_ind(HfgChannelContext *pContext, HfgCallbackParameters* pParams)
{
	MBT_BYTE callStatus;
	T_MBT_AG_CALLSETUP callsetup;
	MBT_BYTE callheld;
	T_MBT_AG_INDICATION *pAGIndication;

	AG_LOG_MSG((MOD_BT, "+ag_handle_delay_call_state_change_ind"));

	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);

	AG_LOG_MSG((MOD_BT, "sdc : call=%d, callsetup=%d, callheld=%d",
		pAGIndication->bCallStatus,
		pAGIndication->bCallSetupStatus,
		pAGIndication->bHeldStatus));

	/* Check call status */
	callStatus = ((getCallCount(MBT_AG_PHONE_CALL_ACTIVE)+getCallCount(MBT_AG_PHONE_CALL_HELD)) > 0) ? 1 : 0;
	if(callStatus != (MBT_BYTE)pAGIndication->bCallStatus)
	{
		AG_LOG_MSG((MOD_BT, "callstatus change to %d", callStatus));
              sdc_update_call_state(callStatus ? MBT_AG_CALLSTATUS_ACTIVE : MBT_AG_CALLSTATUS_END);
		//pAGIndication->bCallStatus = callStatus ? MBT_AG_CALLSTATUS_ACTIVE : MBT_AG_CALLSTATUS_END;
		if(pContext)
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL, (U8)callStatus);
	}
	/* Check callsetup status */
	if (getCallCount(MBT_AG_PHONE_CALL_INCOMING))
	{
		callsetup = MBT_AG_PHONE_CALL_INCOMING;
	}
	else if (getCallCount(MBT_AG_PHONE_CALL_ORIGINATING))
	{
		callsetup = MBT_AG_PHONE_CALL_ORIGINATING;
	}
	else if (getCallCount(MBT_AG_PHONE_CALL_ALERTING))
	{
		callsetup = MBT_AG_PHONE_CALL_ALERTING;
	}
	else
	{
		callsetup = MBT_AG_PHONE_CALL_NONE;
	}
	if(callsetup != pAGIndication->bCallSetupStatus)
	{
		AG_LOG_MSG((MOD_BT, "callsetup change to %d", callsetup));
              sdc_update_callsetup_state(callsetup);
		//pAGIndication->bCallSetupStatus = callsetup;
		if(pContext)
		{
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, (U8)callsetup);
			if(callsetup == MBT_AG_PHONE_CALL_ORIGINATING)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 3);
		}
	}
	/* Check callheld status */
	if(getCallCount(MBT_AG_PHONE_CALL_HELD) == 0)
	{
		callheld = 0;
	}
	else if(getCallCount(MBT_AG_PHONE_CALL_ACTIVE) > 0)
	{
		callheld = 1;
	}
	else
	{
		callheld = 2;
	}
	if(callheld != (MBT_BYTE)pAGIndication->bHeldStatus ||
	   (callheld == MBT_AG_HELD_RETRIEVE_OTHER_CALL && g_bHeldChanged != MBT_FALSE))
	{
		AG_LOG_MSG((MOD_BT, "callheld change to %d", callheld));
              sdc_update_callheld_state(callheld);
		//pAGIndication->bHeldStatus = callheld;
		if(pContext)
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_HELD, (U8)callheld);
			
	}
	g_bDelayCallStatusSent = MBT_FALSE;
	g_bHeldChanged = MBT_FALSE;
	AG_LOG_MSG((MOD_BT, "-ag_handle_delay_call_state_change_ind"));
}
#endif

void AG_EventCallBack(void* handle, BT_CALLBACK_EVENT event, void *ptr, unsigned short datasize)
{
	HfgChannelContext *pContext = (HfgChannelContext*)handle;
	HfgCallbackParameters *pParams = (HfgCallbackParameters*)ptr;	

	MBT_AG_LOCK();
	AG_LOG_CBK((AG_LOG_GROUP_CBK, BT_HFG_WISE_CBK, handle, event));
	switch(event)
	{
	case EVENT_HFG_ACTIVATE_CONFIRM:
		ag_handle_activate_cnf(pContext, pParams);
		break;
	case EVENT_HFG_DEACTIVATE_CONFIRM:
		ag_handle_deactivate_cnf(pContext, pParams);
		break;	//Prevent try to leave deleted criticalsection
	case EVENT_HFG_CONNECT_CONFIRM:
		ag_handle_connect_cnf(pContext, pParams);
		break;
	case EVENT_HFG_ACCEPT_CHANNEL_CONFIRM:
		ag_handle_accept_cnf(pContext, pParams);
		break;
	case EVENT_HFG_SERVICE_CONNECTED:
		ag_handle_service_connected_ind(pContext, pParams);
		break;
	case EVENT_HFG_DISCONNECT_CONFIRM:
		ag_handle_disconnect_cnf(pContext, pParams);
		break;
	case EVENT_HFG_REJECT_CHANNEL_CONFIRM:
		ag_handle_reject_cnf(pContext, pParams);
		break;
	case EVENT_HFG_SERVICE_DISCONNECTED:
		ag_handle_service_disconnected_ind(pContext, pParams);
		break;
	case EVENT_HFG_AUTH_REQ:
		ag_handle_auth_req_ind(pContext, pParams);
		break;
	case EVENT_HFG_READ_INDICATORS:
		ag_handle_read_indicators_ind(pContext, pParams);
		break;
	case EVENT_HFG_HANDSFREE_FEATURES:
		ag_handle_handsfree_features_ind(pContext, pParams);
		break;
	case EVENT_HFG_SCO_CONNECT_CONFIRM:
		ag_handle_sco_connect_cnf(pContext, pParams);
		break;
	case EVENT_HFG_AUDIO_CONNECTED:
		ag_handle_audio_connected_ind(pContext, pParams);
		break;
	case EVENT_HFG_SCO_DISCONNECT_CONFIRM:
		ag_handle_sco_disconnect_cnf(pContext, pParams);
		break;
	case EVENT_HFG_AUDIO_DISCONNECTED:
		ag_handle_audio_disconnected_ind(pContext, pParams);
		break;
	case EVENT_HFG_ANSWER_CALL:
		ag_handle_answer_call_ind(pContext, pParams);
		break;
	case EVENT_HFG_DIAL_NUMBER:
		ag_handle_dial_number_ind(pContext, pParams);
		break;
	case EVENT_HFG_MEMORY_DIAL:
		ag_handle_memory_dial_ind(pContext, pParams);
		break;
	case EVENT_HFG_REDIAL:
		ag_handle_redial_ind(pContext, pParams);
		break;
	case EVENT_HFG_CALL_HOLD:
		ag_handle_call_hold_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_RESPONSE_HOLD:
		ag_handle_query_response_hold_ind(pContext, pParams);
		break;
	case EVENT_HFG_RESPONSE_HOLD:
		ag_handle_response_hold_ind(pContext, pParams);
		break;
	case EVENT_HFG_HANGUP:
		ag_handle_hangup_ind(pContext, pParams);
		break;
	case EVENT_HFG_LIST_CURRENT_CALLS:
		ag_handle_list_current_calls_ind(pContext, pParams);
		break;
	case EVENT_HFG_ENABLE_CALLER_ID:
		ag_handle_enable_caller_id_ind(pContext, pParams);
		break;
	case EVENT_HFG_ENABLE_CALL_WAITING:
		ag_handle_enable_call_waiting_ind(pContext, pParams);
		break;
	case EVENT_HFG_GENERATE_DTMF:
		ag_handle_generate_dtmf_ind(pContext, pParams);
		break;
	case EVENT_HFG_GET_LAST_VOICE_TAG:
		ag_handle_get_last_voice_tag_ind(pContext, pParams);
		break;
	case EVENT_HFG_ENABLE_VOICE_RECOGNITION:
		ag_handle_enable_voice_recognition_ind(pContext, pParams);
		break;
	case EVENT_HFG_DISABLE_NREC:
		ag_handle_disable_nrec_ind(pContext, pParams);
		break;
	case EVENT_HFG_REPORT_MIC_VOLUME:
		ag_handle_report_mic_volume_ind(pContext, pParams);
		break;
	case EVENT_HFG_REPORT_SPK_VOLUME:
		ag_handle_report_spk_volume_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_NETWORK_OPERATOR:
		ag_handle_query_network_operator_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUBSCRIBER_NUMBER:
		ag_handle_query_subscriber_number_ind(pContext, pParams);
		break;
	case EVENT_HFG_ENABLE_EXTENDED_ERRORS:
		ag_handle_enable_extended_error_ind(pContext, pParams);
		break;
	/* Headset event */
	case EVENT_HFG_KEYPAD_CONTROL:
		ag_handle_keypad_control_ind(pContext, pParams);
		break;
	/* Phonebook events */
	case EVENT_HFG_QUERY_SUPPORTED_CHARSET:
		ag_handle_query_supported_charset_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_CHARSET:
		ag_handle_query_selected_charset_ind(pContext, pParams);
		break;
	case EVENT_HFG_SELECT_CHARSET:
		ag_handle_select_charset_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_MODEL_ID:
		ag_handle_query_model_id_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_MANUFACTURE_ID:
		ag_handle_query_manufacture_id_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUPPORTED_PHONEBOOK:
		ag_handle_query_supported_phonebook_ind(pContext, pParams);
		break;
	case EVENT_HFG_SELECT_PHONEBOOK:
		ag_handle_select_phonebook_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_PHONEBOOK:
		ag_handle_query_selected_phonebook_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_READ_PBENTRY_INFO:
		ag_handle_query_read_phonebook_info_ind(pContext, pParams);
		break;
	case EVENT_HFG_READ_PBENTRY:
		ag_handle_read_phonebook_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_FIND_PBENTRY_INFO:
		ag_handle_query_find_phonebook_info_ind(pContext, pParams);
		break;
	case EVENT_HFG_FIND_PBENTRY:
		ag_handle_find_phonebook_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_WRITE_PBENTRY_INFO:
		ag_handle_query_write_phonebook_info_ind(pContext, pParams);
		break;
	case EVENT_HFG_WRITE_PBENTRY:
		ag_handle_write_phonebook_ind(pContext, pParams);
		break;
	/* SMS events */
	case EVENT_HFG_QUERY_SUPPORTED_SMS_SERVICE:
		ag_handle_query_supported_sms_service_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_SMS_SERVICE:
		ag_handle_query_selected_sms_service_ind(pContext, pParams);
		break;
	case EVENT_HFG_SELECT_SMS_SERVICE:
		ag_handle_select_sms_service_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUPPORTED_PREF_MSG_STORAGE:
		ag_handle_query_supported_pref_msg_storage_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_PREF_MSG_STORAGE:
		ag_handle_query_selected_pref_msg_storage_ind(pContext, pParams);
		break;
	case EVENT_HFG_SELECT_PREF_MSG_STORAGE:
		ag_handle_select_pref_msg_storage_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUPPORTED_MSG_FORMAT:
		ag_handle_query_supported_msg_format_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_MSG_FORMAT:
		ag_handle_query_selected_msg_format_ind(pContext, pParams);
		break;
	case EVENT_HFG_SELECT_MSG_FORMAT:
		ag_handle_select_msg_format_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SERVICE_CENTRE:
		ag_handle_query_service_centre_ind(pContext, pParams);
		break;
	case EVENT_HFG_SET_SERVICE_CENTRE:
		ag_handle_set_service_centre_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_TEXT_MODE_PARAMS:
		ag_handle_query_text_mode_params_ind(pContext, pParams);
		break;
	case EVENT_HFG_SET_TEXT_MODE_PARAMS:
		ag_handle_set_text_mode_params_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUPPORTED_SHOW_PARAMS:
		ag_handle_query_supported_show_params_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_SHOW_PARAMS:
		ag_handle_query_selected_show_params_ind(pContext, pParams);
		break;
	case EVENT_HFG_SET_SHOW_PARAMS:
		ag_handle_set_show_params_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUPPORTED_NEW_MSG_INDICATION:
		ag_handle_query_supported_new_msg_indication_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SELECTED_NEW_MSG_INDICATION:
		ag_handle_query_selected_new_msg_indication_ind(pContext, pParams);
		break;
	case EVENT_HFG_SET_NEW_MSG_INDICATION:
		ag_handle_set_new_msg_indication_ind(pContext, pParams);
		break;
	case EVENT_HFG_QUERY_SUPPORTED_LIST_STATUS:
		ag_handle_query_supported_list_status_ind(pContext, pParams);
		break;
	case EVENT_HFG_LIST_MSG:
		ag_handle_list_msg_ind(pContext, pParams);
		break;
	case EVENT_HFG_READ_MSG:
		ag_handle_read_msg_ind(pContext, pParams);
		break;
	case EVENT_HFG_SEND_MSG:
		ag_handle_send_msg_ind(pContext, pParams);
		break;
	case EVENT_HFG_SEND_STORED_MSG:
		ag_handle_send_stored_msg_ind(pContext, pParams);
		break;
	case EVENT_HFG_WRITE_MSG:
		ag_handle_write_msg_ind(pContext, pParams);
		break;
	case EVENT_HFG_DELETE_MSG:
		ag_handle_delete_msg_ind(pContext, pParams);
		break;		
	case EVENT_HFG_RESPONSE_COMPLETE:
		ag_handle_response_complete_ind(pContext, pParams);
		break;
	case EVENT_HFG_RFCOMM_CONNECTED:
		break;
	case EVENT_HFG_AT_COMMAND_DATA:
		break;
	#ifdef DELAY_CALL_STATE_CHANGE
	case EVENT_HFG_DELAY_CALL_STATE_CHANGE:
		ag_handle_delay_call_state_change_ind(pContext,pParams);
		break;
	#endif
	default:
		break;
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   MBT exported interface
********************************************************************************/
/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_enable(MBT_VOID)
{
	MBT_AG_INITLOCK();
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_ENABLE));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_enable");
       #endif
	if(!g_AgContext.bEnabled)
	{
		#if defined(AG_CALL_SIMULATION)
		resetAllCallState();
		#endif
		g_AgContext.bDeferedConnReq = MBT_FALSE;
		g_AgContext.pHfgContext[HSP_CONTEXT] = (HfgChannelContext*)ADP_OS_MemAlloc(sizeof(HfgChannelContext));
		g_AgContext.pHfgContext[HFP_CONTEXT] = (HfgChannelContext*)ADP_OS_MemAlloc(sizeof(HfgChannelContext));
		if(g_AgContext.pHfgContext[HSP_CONTEXT] != NULL || g_AgContext.pHfgContext[HFP_CONTEXT] != NULL)
		{
			ADP_OS_MemSet(g_AgContext.pHfgContext[HSP_CONTEXT], 0, sizeof(HfgChannelContext));
			ADP_OS_MemSet(g_AgContext.pHfgContext[HFP_CONTEXT], 0, sizeof(HfgChannelContext));
			btmtk_hfg_register(g_AgContext.pHfgContext[HSP_CONTEXT], AG_EventCallBack, TRUE);
			btmtk_hfg_register(g_AgContext.pHfgContext[HFP_CONTEXT], AG_EventCallBack, FALSE);
			changeProfileState(HSP_CONTEXT, PROFILE_STATE_ACTIVATING);
			changeProfileState(HFP_CONTEXT, PROFILE_STATE_ACTIVATING);
		}
		else
		{
			/* Allocate context memory failed */
			AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_ALLOC_MEM_FAIL));
			if(g_AgContext.pHfgContext[HSP_CONTEXT])
				ADP_OS_MemFree(g_AgContext.pHfgContext[HSP_CONTEXT]);
			if(g_AgContext.pHfgContext[HFP_CONTEXT])
				ADP_OS_MemFree(g_AgContext.pHfgContext[HFP_CONTEXT]);
			ag_postevent(MBTEVT_AG_ENABLE_FAIL, 0);
		}
	}
	else
	{
		AG_LOG_WARN((AG_LOG_GROUP_WARN, BT_HFG_WISE_WARN_ALREADY_ENABLED));
		ag_postevent(MBTEVT_AG_ENABLE_SUCCESS, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_disable(MBT_VOID)
{
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_DISABLE));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_disable");
       #endif
	if(g_AgContext.bEnabled)
	{
		if(	g_AgContext.state[HSP_CONTEXT] == PROFILE_STATE_ACTIVATED &&
			g_AgContext.state[HFP_CONTEXT] == PROFILE_STATE_ACTIVATED)
		{
		g_AgContext.bDeferedConnReq = MBT_FALSE;
			btmtk_hfg_deregister(g_AgContext.pHfgContext[HSP_CONTEXT]);
			btmtk_hfg_deregister(g_AgContext.pHfgContext[HFP_CONTEXT]);
		changeProfileState(0, PROFILE_STATE_DEACTIVATING);
		changeProfileState(1, PROFILE_STATE_DEACTIVATING);
	}
	else
	{
			/* Not in idle state */
			ag_postevent(MBTEVT_AG_DISABLE_FAIL, 0);
		}
	}
	else
	{
		AG_LOG_WARN((AG_LOG_GROUP_WARN, BT_HFG_WISE_WARN_ALREADY_DISABLED));
		ag_postevent(MBTEVT_AG_DISABLE_SUCCESS, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_connect(T_MBT_BDADDR RemoteBDAddr, MBT_SERVICE_ID MBtSvc)
{
	MBT_BYTE index = (MBtSvc==MBT_SVCUUID_HEADSET) ? 0 : 1;
	T_MBT_AG_STATUS *pAGStatus;
	U8		bdAddr[6];

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_CONNECT, 
		RemoteBDAddr[0], 
		RemoteBDAddr[1],
		RemoteBDAddr[2],
		RemoteBDAddr[3],
		RemoteBDAddr[4],
		RemoteBDAddr[5],
		MBtSvc));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_connect");
       LogBDAddress(RemoteBDAddr);
       MBT_LOG1("[BTMTK][AG] MBtSvc=0x04X", MBtSvc);
       #endif
	if(g_AgContext.bEnabled)
	{
		pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);	
		index = (MBtSvc==MBT_SVCUUID_HEADSET) ? 0 : 1;
		if( g_AgContext.activeProfile == NULL_CONTEXT &&
	   g_AgContext.state[index] == PROFILE_STATE_ACTIVATED)
	{
		/* Update SDC */
              sdc_update_remote_address(RemoteBDAddr);
		//ADP_OS_MemCopy(pAGStatus->BdAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR));	
		sdc_change_AG_state(MBT_AG_STATE_CONNECTING);
		//pAGStatus->AgState = MBT_AG_STATE_CONNECTING;
		/* Call AG connect function */	
		btmtk_util_convert_mbtarray2mtkarray(bdAddr, RemoteBDAddr);
		btmtk_hfg_create_service_link(g_AgContext.pHfgContext[index], (MTK_BD_ADDR*)bdAddr);
		changeProfileState(index, PROFILE_STATE_CONNECTING);
	}
		else if( g_AgContext.activeProfile != NULL_CONTEXT &&
			(index != g_AgContext.activeProfile ||
			pAGStatus->AgState != MBT_AG_STATE_CONNECTING ||
			0 != ADP_OS_MemCmp(pAGStatus->BdAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR)))
			)
	{
			// Let User can connect any time even in connecting state
			g_AgContext.bDeferedConnReq = MBT_TRUE;
			ADP_OS_MemCopy(g_AgContext.deferedAddr, RemoteBDAddr, /* sizeof(RemoteBDAddr) */ 6);
			g_AgContext.deferedSvcID = MBtSvc;
			if(g_AgContext.state[g_AgContext.activeProfile] != PROFILE_STATE_DISCONNECTING)
			{
			btmtk_hfg_disconnect_service_link(g_AgContext.pHfgContext[g_AgContext.activeProfile]);
				changeProfileState(g_AgContext.activeProfile, PROFILE_STATE_DISCONNECTING);
			}
		}
		else
		{
			/* Do nothing */
		}
	}
	else
	{
		AG_LOG_WARN((AG_LOG_GROUP_WARN, BT_HFG_WISE_WARN_NOT_IN_ACTIVATED, index, g_AgContext.state[index]));
		ag_postevent(MBTEVT_AG_CONNECT_FAIL, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_disconnect(T_MBT_BDADDR RemoteBDAddr, MBT_SERVICE_ID MBtSvc)
{
	MBT_BYTE index = g_AgContext.activeProfile;//(MBtSvc==MBT_SVCUUID_HEADSET) ? 0 : 1;
	T_MBT_AG_STATUS *pAGStatus;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_DISCONNECT, 
		RemoteBDAddr[0], 
		RemoteBDAddr[1],
		RemoteBDAddr[2],
		RemoteBDAddr[3],
		RemoteBDAddr[4],
		RemoteBDAddr[5],
		MBtSvc));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_disconnect");
       LogBDAddress(RemoteBDAddr);
       MBT_LOG1("[BTMTK][AG] MBtSvc=0x%04X", MBtSvc);
       #endif    
    pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(index != NULL_CONTEXT &&
          0 == ADP_OS_MemCmp(pAGStatus->BdAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR)) &&
	   (g_AgContext.state[index] >= PROFILE_STATE_CONNECTED ||
	   g_AgContext.state[index] == PROFILE_STATE_CONNECTING ||
	   g_AgContext.state[index] == PROFILE_STATE_ACCEPTING)
	   )
	{
		/* Let user can disconnect at most condition and can be 
		considered as an cancel operation  */
		//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
		sdc_clear_remote_address();
		//ADP_OS_MemSet(pAGStatus->BdAddr, 0, sizeof(pAGStatus->BdAddr));
		sdc_change_AG_state(MBT_AG_STATE_DISCONNECTING);
		//pAGStatus->AgState = MBT_AG_STATE_DISCONNECTING;
		btmtk_hfg_disconnect_service_link(g_AgContext.pHfgContext[index]);
		changeProfileState(index, PROFILE_STATE_DISCONNECTING);
	}
	else if(index == NULL_CONTEXT || g_AgContext.state[index] != PROFILE_STATE_DISCONNECTING)
	{
		/* If no active context or state is not in disconnecting state,
                directly response FALSE */
		AG_LOG_WARN((AG_LOG_GROUP_WARN, BT_HFG_WISE_WARN_NOT_CONNECTED, index, g_AgContext.state[index]));
		ag_postevent(MBTEVT_AG_DISCONNECT_FAIL, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_audioconnect(MBT_VOID)
{
	//T_MBT_AG_STATUS 		*pAGStatus;
	HfgChannelContext		*pContext;
	ProfileState			state;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_AUDIO_CONNECT));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_audioconnect");
       #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT )
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		state = g_AgContext.state[g_AgContext.activeProfile];
		//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
		if(state == PROFILE_STATE_CONNECTED)
		{
		       sdc_change_AG_state(MBT_AG_STATE_AUDIO_CONNECTING);
			//pAGStatus->AgState = MBT_AG_STATE_AUDIO_CONNECTING;
			btmtk_hfg_create_audio_link(pContext);
			changeProfileState(g_AgContext.activeProfile, PROFILE_STATE_AUDIO_CONNECTING);
		}
		else if(state == PROFILE_STATE_AUDIO_CONNECTING)
		{
		       sdc_change_AG_state(MBT_AG_STATE_AUDIO_CONNECTING);
			//pAGStatus->AgState = MBT_AG_STATE_AUDIO_CONNECTING;
		}
		else
		{
			AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, g_AgContext.activeProfile, state));
			ag_postevent(MBTEVT_AG_AUDIO_CONNECT_FAIL, 0);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_NO_CONNECTION));
		ag_postevent(MBTEVT_AG_AUDIO_CONNECT_FAIL, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_audiodisconnect(MBT_VOID)
{
	//MBT_BYTE index;
	//T_MBT_AG_STATUS 		*pAGStatus;
	HfgChannelContext		*pContext;
	ProfileState			state;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_AUDIO_DISCONNECT));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_audiodisconnect");
       #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT )
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		state = g_AgContext.state[g_AgContext.activeProfile];
		//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
		if(state == PROFILE_STATE_AUDIO_CONNECTED)
		{
		       sdc_change_AG_state(MBT_AG_STATE_AUDIO_DISCONNECTING);
			//pAGStatus->AgState = MBT_AG_STATE_AUDIO_DISCONNECTING;
			btmtk_hfg_disconnect_audio_link(pContext);
			changeProfileState(g_AgContext.activeProfile, PROFILE_STATE_AUDIO_DISCONNECTING);
		}
		else if(state == PROFILE_STATE_AUDIO_DISCONNECTING)
		{
		       sdc_change_AG_state(MBT_AG_STATE_AUDIO_DISCONNECTING);
			//pAGStatus->AgState = MBT_AG_STATE_AUDIO_DISCONNECTING;
		}
		else
		{
			AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_UNEXPECTED_STATE, g_AgContext.activeProfile, state));
			ag_postevent(MBTEVT_AG_AUDIO_DISCONNECT_FAIL, 0);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_ERR, BT_HFG_WISE_ERR_NO_CONNECTION));
		ag_postevent(MBTEVT_AG_AUDIO_DISCONNECT_FAIL, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_BOOL btmtk_ag_getconstatus(MBT_VOID)
{
	T_MBT_AG_STATUS 	*pAGStatus;
	MBT_BOOL			ret = MBT_FALSE;

	MBT_AG_LOCK();
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(pAGStatus->AgState >= MBT_AG_STATE_CONNECTED)
		ret = MBT_TRUE;
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_GET_CON_STATUS, ret ? 1 : 0));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_getconstatus");
       MBT_LOG1("[BTMTK][AG] return %s", ret ? "true" : "false");
       #endif
	MBT_AG_UNLOCK();
	return ret;
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_BOOL btmtk_ag_getaudiostatus(MBT_VOID)
{
	T_MBT_AG_STATUS 	*pAGStatus;
	MBT_BOOL			ret = MBT_FALSE;

	MBT_AG_LOCK();
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	if(pAGStatus->AgState == MBT_AG_STATE_AUDIO_CONNECTED)
		ret = MBT_TRUE;
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_GET_AUDIO_CON_STATUS, ret ? 1 : 0));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_getaudiostatus");
       MBT_LOG1("[BTMTK][AG] return %s", ret ? "true" : "false");
       #endif
	MBT_AG_UNLOCK();
	return ret;
}


/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_setconnectable(MBT_BOOL connectable)
{
	//AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CONNECTABLE, connectable ? 1 : 0));
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_setaudiopath(MBT_BOOL audiopath)
{
	//AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CONNECTABLE, audiopath ? 1 : 0));
}

/********************************************************************************
*   Description :
********************************************************************************/
MBT_VOID btmtk_ag_setspkvolume(MBT_BYTE Level)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_SPK_VOLUME, Level));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setspkvolume");
       MBT_LOG1("[BTMTK][AG] Level=%d", Level);
       #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT && 
	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		//T_MBT_AG_SPKGAIN *pAGSpkGain;

		//pAGSpkGain = (T_MBT_AG_SPKGAIN*)mbt_sdc_getrecord(MBTSDC_REC_AG_SPKGAIN);
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		btmtk_hfg_send_speaker_volume(pContext, Level);
              sdc_update_spk_vol_level(Level);
		//pAGSpkGain->VolumeLevel = Level;
	}
	MBT_AG_UNLOCK();
}


#ifdef BTMTK_WISE_MBT_LOG
static const char* g_strPhoneCallState[] = 
{
   "MBT_AG_PHONE_CALL_NONE",           // idle
   "MBT_AG_PHONE_CALL_INCOMING",     	 // incoming call process ongoing
   "MBT_AG_PHONE_CALL_ORIGINATING", 	 // outgoing call process ongoing
   "MBT_AG_PHONE_CALL_ALERTING",       // remote party being alerted in an outgoing call
   "MBT_AG_PHONE_CALL_ACTIVE",         // active call exist
   "MBT_AG_PHONE_CALL_HELD",
};
static const char* g_strNetState[] = 
{
   "MBT_AG_NET_OFF",
   "MBT_AG_NET_ON",
};
static const char* g_strCallStatus[] = 
{
	"MBT_AG_CALLSTATUS_END",			// no active call [call indicator, 1]
	"MBT_AG_CALLSTATUS_ACTIVE",		// active call [call indicator, 0]
};
/* T_MBT_AG_CALLSETUP */
static const char* g_strCallSetupStatus[] = 
{
	"MBT_AG_CALLSETUP_IDLE",
	"MBT_AG_CALLSETUP_INCOMING",
	"MBT_AG_CALLSETUP_ORIGINATING",
	"MBT_AG_CALLSETUP_ALERTING",
} ;
static const char* g_strHeldState[] = 
{
    "MBT_AG_NO_CALL_HELD",
    "MBT_AG_HELD_RETRIEVE_OTHER_CALL",
    "MBT_AG_HELD_NO_ACTIVE_CALL",
};

static const char* g_strNetMode[] = {
	"MBT_AG_NM_AUTOMATIC",
	"MBT_AG_NM_MANUAL",
	"MBT_AG_NM_DEREGISTER_FROM_NETWORK",
	"MBT_AG_NM_NO_CHANGE",
	"MBT_AG_NM_MANUAL_AUTOMATIC",
	"MBT_AG_NM_NO_OPERATOR_SELECTED",
};
#endif
/********************************************************************************
*   Description :
********************************************************************************/
#if defined(AG_CALL_SIMULATION)
MBT_VOID btmtk_ag_callstatechange(T_MBT_AG_PHONE_CALLSTATE NewState )
{
	HfgChannelContext		*pContext = NULL;
	T_MBT_AG_INDICATION *pAGIndication;
	T_MBT_AG_STATUS	*pAGStatus;
	T_MBT_AG_PHONE_CALLSTATE state;
	T_MBT_AG_CALL_HELD	callheld;
	MBT_BYTE callCount;
	MBT_BOOL bHeadset;
	MBT_BOOL bConnected = MBT_FALSE;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_CALL_STATE_CHANGE, NewState));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_callstatechange");
       MBT_LOG1("[BTMTK][AG] NewState=%s", g_strPhoneCallState[NewState]);
       #endif
	// Get selected call state first
	state = getSelCallState();
	callCount = getCallCount(MBT_AG_PHONE_CALL_ACTIVE) + getCallCount(MBT_AG_PHONE_CALL_HELD);

	if (g_AgContext.activeProfile != NULL_CONTEXT && 
	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		bConnected = MBT_TRUE;
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
	}
	
    	// Stop ring timer
    	if(g_AgContext.bRingStarted)
        	btmtk_ag_ringstop();

	bHeadset = (g_AgContext.activeProfile == HSP_CONTEXT);
	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);

	// Start ring or set call waiting
	if(NewState == MBT_AG_PHONE_CALL_INCOMING &&
	   callCount > 0)
	{
		if(bConnected && !bHeadset)
		{
			/* Waiting call */
			T_MBT_AG_DIALINFO *pAGDialInfo;
			HfgCallWaitParms 		wait;
			
			pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
			wait.number = pAGDialInfo->Digit;
			wait.classmap = 1;
			wait.type = 129;
			if(pAGDialInfo->Digit[0] == '+')
				wait.type = 145;
			btmtk_hfg_call_waiting(pContext, &wait);
		}
	}

	// Update call simulation
	changeCallState(NewState);

	#ifdef DELAY_CALL_STATE_CHANGE
	if(bConnected)
	{
		if(g_bDelayCallStatusSent == MBT_FALSE)
		{

			ilm_struct *int_send_ilm;
			bt_hfg_activate_req_struct *msg_p;
			int_send_ilm = allocate_ilm(MOD_WISE);
			int_send_ilm->msg_id = MSG_ID_BT_HFG_GROUP_START;
			msg_p = (bt_hfg_activate_req_struct*)construct_local_para(sizeof(bt_hfg_activate_req_struct),TD_UL);
			msg_p->req_context = (void*)pContext;
			int_send_ilm->local_para_ptr = (local_para_struct*)msg_p;
			int_send_ilm->local_para_ptr->ref_count = 1;
			int_send_ilm->peer_buff_ptr = NULL;
			int_send_ilm->dest_mod_id = MOD_WISE;
			int_send_ilm->src_mod_id = MOD_WISE;
			int_send_ilm->sap_id = NULL;

			msg_send_int_queue(int_send_ilm);
			g_bDelayCallStatusSent = MBT_TRUE;
			AG_LOG_MSG((MOD_BT, "call state change delay msg is sent"));
		}
	}
	else
	{
		ag_handle_delay_call_state_change_ind(NULL,NULL);
	}
	
	if( g_bDelayCallStatusSent &&
	    (state == MBT_AG_PHONE_CALL_ACTIVE ||
	     state == MBT_AG_PHONE_CALL_HELD ||
	     NewState == MBT_AG_PHONE_CALL_ACTIVE ||
	     NewState == MBT_AG_PHONE_CALL_HELD))
	{
		AG_LOG_MSG((MOD_BT, "g_bHeldChanged set to TRUE"));
		g_bHeldChanged = MBT_TRUE;
	}
	#else
	//update callheld status
	if(getCallCount(MBT_AG_PHONE_CALL_HELD) == 0)
	{
		callheld = MBT_AG_NO_CALL_HELD;
	}
	else
	{
		if(getCallCount(MBT_AG_PHONE_CALL_ACTIVE))
			callheld = MBT_AG_HELD_RETRIEVE_OTHER_CALL;
		else
			callheld = MBT_AG_HELD_NO_ACTIVE_CALL;
	}
	// Check if indicator is needed
	if(pAGIndication->bHeldStatus != callheld)
	{
		if(bConnected && !bHeadset)
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_HELD, (MBT_BYTE)callheld);
	}
	//Store new callheld status
	sdc_update_callheld_state(callheld);
	//pAGIndication->bHeldStatus = callheld;

	// Update call status
	if( (getCallCount(MBT_AG_PHONE_CALL_ACTIVE) + getCallCount(MBT_AG_PHONE_CALL_HELD))  > 0)
	{
	       sdc_update_call_state(MBT_AG_CALLSTATUS_ACTIVE);
		//pAGIndication->bCallStatus = MBT_AG_CALLSTATUS_ACTIVE;
		if(callCount == 0)
		{
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL, 1);	
		}
	}
	else
	{
        	sdc_update_call_state(MBT_AG_CALLSTATUS_END);
		//pAGIndication->bCallStatus = MBT_AG_CALLSTATUS_END;
		if(callCount > 0)
		{
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL, 0);	
		}
	}

	// Update callsetup status
	if( (state >= MBT_AG_PHONE_CALL_INCOMING && state <= MBT_AG_PHONE_CALL_ALERTING) ||
		(NewState >= MBT_AG_PHONE_CALL_INCOMING && NewState <= MBT_AG_PHONE_CALL_ALERTING) )
	{
		if(NewState >= MBT_AG_PHONE_CALL_INCOMING && NewState <= MBT_AG_PHONE_CALL_ALERTING)
		{
		       sdc_update_callsetup_state((T_MBT_AG_CALLSETUP)NewState);
			//pAGIndication->bCallSetupStatus = (T_MBT_AG_CALLSETUP)NewState;
			if(bConnected && !bHeadset)
			{
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, (MBT_BYTE)NewState);
				#ifdef CUSTOM_FOR_WISE
				if(NewState == MBT_AG_PHONE_CALL_ORIGINATING)
					btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 3);
				#endif
			}
		}
		else
		{
		       sdc_update_callsetup_state(MBT_AG_CALLSETUP_IDLE);
			//pAGIndication->bCallSetupStatus = MBT_AG_CALLSETUP_IDLE;
			if(bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 0);
		}
	}
	#endif /* DELAY_CALL_STATE_CHANGE */

	if(NewState == MBT_AG_PHONE_CALL_INCOMING &&
	   callCount == 0)
	{	// Incoming call
		btmtk_ag_ringstart();
	}
	
	// Update PhoneCallState
	if(NewState >= MBT_AG_PHONE_CALL_NONE && NewState <= MBT_AG_PHONE_CALL_HELD)
	{
		//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
		//pAGStatus->PhoneCallState = NewState;
	       sdc_update_phone_call_state(NewState);
	}
	
	MBT_AG_UNLOCK();
}

#else
MBT_VOID btmtk_ag_callstatechange(T_MBT_AG_PHONE_CALLSTATE NewState )
{
	HfgChannelContext		*pContext;
	T_MBT_AG_INDICATION *pAGIndication;
	//T_MBT_AG_STATUS	*pAGStatus;
	MBT_BOOL bHeadset;
	MBT_BOOL bConnected = MBT_FALSE;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_CALL_STATE_CHANGE, NewState));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_callstatechange");
       MBT_LOG1("[BTMTK][AG] NewState=%s", g_strPhoneCallState[NewState]);
       #endif
	if (g_AgContext.activeProfile != NULL_CONTEXT && 
	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		bConnected = MBT_TRUE;
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
	}
	
    // Stop ring timer
    if(g_AgContext.bRingStarted)
        btmtk_ag_ringstop();

	bHeadset = (g_AgContext.activeProfile == HSP_CONTEXT);
	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
	switch(NewState)
	{
	case MBT_AG_PHONE_CALL_NONE:           		// idle
		// Held calls
		if(pAGIndication->bHeldStatus != MBT_AG_NO_CALL_HELD)
		{
		       sdc_update_callheld_state(MBT_AG_NO_CALL_HELD);
			//pAGIndication->bHeldStatus = MBT_AG_NO_CALL_HELD;
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_HELD, 0);			
		}
		// Active calls
		if(pAGIndication->bCallStatus != MBT_AG_CALLSTATUS_END)
		{
		       sdc_update_call_state(MBT_AG_CALLSTATUS_END);
			//pAGIndication->bCallStatus = MBT_AG_CALLSTATUS_END;
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL, 0);
		}
		// Call setup
		if(pAGIndication->bCallSetupStatus != MBT_AG_CALLSETUP_IDLE)
		{
		       sdc_update_callsetup_state(MBT_AG_CALLSETUP_IDLE);
			//pAGIndication->bCallSetupStatus = MBT_AG_CALLSETUP_IDLE;
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 0);
		}
		break;
   	case MBT_AG_PHONE_CALL_INCOMING:    	 	// incoming call process ongoing
   	       sdc_update_callsetup_state(MBT_AG_CALLSETUP_INCOMING);
	   	//pAGIndication->bCallSetupStatus = MBT_AG_CALLSETUP_INCOMING;
		if (bConnected)
        {      
        		if( !bHeadset )
        		{
		        	if(pAGIndication->bCallStatus == MBT_AG_CALLSTATUS_ACTIVE)
		        	{
			        	T_MBT_AG_DIALINFO *pAGDialInfo;
					HfgCallWaitParms 		wait;
					
					pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);
					wait.number = pAGDialInfo->Digit;
					wait.classmap = 1;
					wait.type = 129;
					if(pAGDialInfo->Digit[0] == '+')
						wait.type = 145;
					btmtk_hfg_call_waiting(pContext, &wait);
				}
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 1);
        		}
        		if(pAGIndication->bCallStatus == MBT_AG_CALLSTATUS_END)
            btmtk_ag_ringstart();
        }
   		break;
   	case MBT_AG_PHONE_CALL_ORIGINATING: 	 // outgoing call process ongoing
   	       sdc_update_callsetup_state(MBT_AG_CALLSETUP_ORIGINATING);
	   	//pAGIndication->bCallSetupStatus = MBT_AG_CALLSETUP_ORIGINATING;
		if (bConnected && !bHeadset)
		{
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 2);
			#ifdef CUSTOM_FOR_WISE
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 3);
			#endif
		}
   		break;
   	case MBT_AG_PHONE_CALL_ALERTING:       	// remote party being alerted in an outgoing call
   	       sdc_update_callsetup_state(MBT_AG_CALLSETUP_ALERTING);
	   	//pAGIndication->bCallSetupStatus = MBT_AG_CALLSETUP_ALERTING;
		if (bConnected && !bHeadset)
			btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 3);
   		break;
   	case MBT_AG_PHONE_CALL_ACTIVE:         		// active call exist
		if(pAGIndication->bCallStatus == MBT_AG_CALLSTATUS_END)
		{
		       sdc_update_call_state(MBT_AG_CALLSTATUS_ACTIVE);
			//pAGIndication->bCallStatus = MBT_AG_CALLSTATUS_ACTIVE;
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL, 1);
		}
		if(pAGIndication->bCallSetupStatus != MBT_AG_CALLSETUP_IDLE)
		{
		       sdc_update_callsetup_state(MBT_AG_CALLSETUP_IDLE);
			//pAGIndication->bCallSetupStatus = MBT_AG_CALLSETUP_IDLE;
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, 0);
		}
		break;   	
   	case MBT_AG_PHONE_CALL_HELD:
		if(pAGIndication->bCallStatus != MBT_AG_CALLSTATUS_END)
		{
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_HELD, 1);
		}
		else
		{
			if (bConnected && !bHeadset)
				btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_HELD, 2);			
		}
		break;
	default:
		goto exit;
		break;
	}
	// Update PhoneCallState
	//pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
	//pAGStatus->PhoneCallState = NewState;
	sdc_update_phone_call_state(NewState);
exit:
	MBT_AG_UNLOCK();
}
#endif
/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setnetworkstatus(T_MBT_AG_NETSTATE  State)
{
	MBT_BOOL bHeadset;
	//T_MBT_AG_INDICATION *pAGIndication;
	//pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_NETWORK_STATUS, State));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setnetworkstatus");
       MBT_LOG1("[BTMTK][AG] State=%s", g_strNetState[State]);
       #endif
       sdc_update_net_status(State);
     	//pAGIndication->bNetStatus = State;
	bHeadset = (g_AgContext.activeProfile==HSP_CONTEXT);
	if( g_AgContext.activeProfile != NULL_CONTEXT && 
	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
	    !bHeadset)
	{
		HfgChannelContext		*pContext;
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		btmtk_hfg_set_indicator_value(pContext, HFG_IND_SERVICE, (U8)State);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setcid(MBT_CHAR* Num, MBT_BYTE len)
{
    T_MBT_AG_DIALINFO *pAGDialInfo;

    MBT_AG_LOCK();
    AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CALLER_ID, len));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcid");
    MBT_LOG2("[BTMTK][AG] Num=%s, len=%d", Num, len);
    #endif
    pAGDialInfo = (T_MBT_AG_DIALINFO*)mbt_sdc_getrecord(MBTSDC_REC_AG_DIALINFO);

    if(Num && Num[0])
    {
        MBT_CHAR tmp[MBT_MAX_DIALNUM_LEN + 1];
       MBT_LOG2("[BTMTK][AG][SDC] sdcDialInfo->DialNum : %d => %d",
            pAGDialInfo->DialNum,
            len);
       pAGDialInfo->Digit[pAGDialInfo->DialNum] = 0;
       ADP_OS_MemCopy(tmp, Num, len);
       tmp[len] = 0;
       MBT_LOG2("[BTMTK][AG][SDC] sdcDialInfo->Digit : \"%s\" => \"%s\"",
            pAGDialInfo->Digit,
            tmp);
    	pAGDialInfo->DialNum = len;
    	ADP_OS_MemCopy(pAGDialInfo->Digit, Num, len);
    	pAGDialInfo->Digit[len]= 0;
    	AG_LOG_MSG((MOD_BT, "[AG][INFO] callerid is %s", pAGDialInfo->Digit));
    }
    else
    {
    	AG_LOG_MSG((MOD_BT, "[AG][WARN] callerid is empty"));
       MBT_LOG2("[BTMTK][AG][SDC] sdcDialInfo->DialNum : %d => %d",
            pAGDialInfo->DialNum,
            0);
       MBT_LOG1("[BTMTK][AG][SDC] sdcDialInfo->Digit : \"%s\" => \"\"",
            pAGDialInfo->Digit);        
    	pAGDialInfo->DialNum = 0;
    	pAGDialInfo->Digit[0] = 0;
    }
    MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setsignalstrength(MBT_BYTE Level)
{
    MBT_BOOL bHeadset;
    //T_MBT_AG_INDICATION *pAGIndication;

    MBT_AG_LOCK();
    AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_SIGNAL_STRENGTH, Level));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setsignalstrength");
    MBT_LOG1("[BTMTK][AG] Level=%d", Level);
    #endif    
    sdc_update_signal_level(Level);
    //pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    //pAGIndication->bSignalLevel = Level;
    bHeadset = (g_AgContext.activeProfile == HSP_CONTEXT);
    if( g_AgContext.activeProfile != NULL_CONTEXT && 
        g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
        !bHeadset)
    {
    	HfgChannelContext		*pContext;
    	pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
    	btmtk_hfg_set_indicator_value(pContext, HFG_IND_SIGNAL, Level);
    }
    MBT_AG_UNLOCK();
    return MBT_TRUE;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setroamingstatus(MBT_BYTE Level)
{
    MBT_BOOL bHeadset;
    //T_MBT_AG_INDICATION *pAGIndication;

    //pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    MBT_AG_LOCK();	
    AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_ROAMING_STATUS, Level));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setroamingstatus");
    MBT_LOG1("[BTMTK][AG] Level=%d", Level);
    #endif
    sdc_update_roaming_status(Level);
    //pAGIndication->bRoamingStatus = Level;    
    bHeadset = (g_AgContext.activeProfile==HSP_CONTEXT);
    if( g_AgContext.activeProfile != NULL_CONTEXT && 
        g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
        !bHeadset)
    {
    	HfgChannelContext		*pContext;
    	pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
    	btmtk_hfg_set_indicator_value(pContext, HFG_IND_ROAMING, Level);
    }
    MBT_AG_UNLOCK();
    return MBT_TRUE;     
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setbatterylevel(MBT_BYTE Level)
{
    MBT_BOOL bHeadset;
    //T_MBT_AG_INDICATION *pAGIndication;

    MBT_AG_LOCK();
    AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_BATTERY_LEVEL, Level));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setbatterylevel");
    MBT_LOG1("[BTMTK][AG] Level=%d", Level);
    #endif
    sdc_update_battery_level(Level);
    //pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
    //pAGIndication->bBatteryLevel = Level;
    bHeadset = (g_AgContext.activeProfile == HSP_CONTEXT);
    if( g_AgContext.activeProfile != NULL_CONTEXT && 
        g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
        !bHeadset)
    {
    	HfgChannelContext		*pContext;
    	pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
    	btmtk_hfg_set_indicator_value(pContext, HFG_IND_BATTERY, Level);
    }
    MBT_AG_UNLOCK();
    return MBT_TRUE;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setcallheldstatus(T_MBT_AG_CALL_HELD value)
{
	MBT_BOOL bHeadset;
	//T_MBT_AG_INDICATION *pAGIndication;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CALLHELD_STATUS, value));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcallheldstatus");
    MBT_LOG1("[BTMTK][AG] value=%s", g_strHeldState[value]);
    #endif
#if defined(AG_USE_CALL_OPTION2)    
	//pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
       sdc_update_callheld_state(value);
	//pAGIndication->bHeldStatus = value;
	bHeadset = (g_AgContext.activeProfile==HSP_CONTEXT);
	if( g_AgContext.activeProfile != NULL_CONTEXT && 
	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
	    !bHeadset)
	{
		HfgChannelContext		*pContext;
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_HELD, (U8)value);
	}
#endif
	MBT_AG_UNLOCK();
	return MBT_TRUE;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setoperatorselection(T_MBT_AG_NETMODE Netmode,MBT_BYTE* OpName)
{
	HfgChannelContext		*pContext;
	HfgNetworkOperator 	oper;// = {Netmode, 0, (const char*)OpName};
	MBT_BOOL			ret = MBT_FALSE;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_OPERATOR_SELECTION, Netmode));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setoperatorselection");
    MBT_LOG2("[BTMTK][AG] Netmode=%s, OpName=%s", g_strNetMode[Netmode], OpName);
    #endif
	if(OpName && OpName[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] Operator name=%s", OpName));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] Operator name is null"));
	
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
	    if(OpName != NULL && OpName[0] != 0)
	{
		oper.mode = Netmode;
		oper.format = 0;
		oper.oper = OpName;
		btmtk_hfg_send_network_operator_rsp(pContext, &oper);
        }
        else
        {
            btmtk_hfg_send_ok(pContext);
        }
		ret = TRUE;
	}

	MBT_AG_UNLOCK();
 	return ret;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setextendederror(T_MBT_AG_CME_ERR ErrorCode)
{
	HfgChannelContext		*pContext;
	MBT_BOOL			ret = MBT_FALSE;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_EXTENDED_ERROR, ErrorCode));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setextendederror");
    MBT_LOG1("[BTMTK][AG] ErrorCode=%d", ErrorCode);
    #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		/* if BTRH operation is failed. Reset this flag */
		g_AgContext.bBtrh = MBT_FALSE;
		btmtk_hfg_send_error(pContext, (U8)ErrorCode);
		ret = MBT_TRUE;
	}
	MBT_AG_UNLOCK();
	return ret;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setsubscribernumber(MBT_CHAR* Num, MBT_BYTE NumType, MBT_BYTE Len, T_MBT_AG_SERVICE Service, MBT_BOOL FinalFlag)
{
	HfgChannelContext		*pContext;
	HfgSubscriberNum 		subNum;// = {Num, NumType, Service};
	MBT_BOOL			ret = MBT_FALSE;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_SUBSCRIBER_NUMBER, Len, NumType, Service, FinalFlag?1:0));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setsubscribernumber");
    MBT_LOG3("[BTMTK][AG] Num=%s, NumType=%d, Len=%d", Num, NumType, Len);
    MBT_LOG2("[BTMTK][AG] Service=%d, FinalFlag=%s", Service, convert_bool_to_string(FinalFlag));
    #endif
	if(Num && Len > 0)
		AG_LOG_MSG((MOD_BT, "[AG][INFO] subscriber number=%s", Num));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] subscriber number is NULL"));
	
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
	    #if 1
        pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
        if(Len)
		    subNum.number = Num;
        else
            subNum.number = NULL;
		subNum.type = NumType;
		subNum.service = Service;
		btmtk_hfg_send_subscriber_number_rsp(pContext, &subNum, FinalFlag);
        #else
	 	//if(Num != NULL && Len > 0)
	 	if(Num && Num[0])
        {     
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		subNum.number = Num;
		subNum.type = NumType;
		subNum.service = Service;
		btmtk_hfg_send_subscriber_number_rsp(pContext, &subNum, FinalFlag);
        }
        else
        {
            btmtk_hfg_send_ok(pContext);
        }
        #endif
		ret = MBT_TRUE;
	}
	MBT_AG_UNLOCK();
 	return ret;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setcallwaiting(MBT_CHAR* Num, MBT_BYTE Len)
{
	MBT_BOOL bHeadset;
	HfgChannelContext		*pContext;
	HfgCallWaitParms 		wait;// = {(const char*)Num, 129, 1 /* voice */};
	MBT_BOOL			ret = MBT_FALSE;
	
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CALL_WAITING, Len));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcallwaiting");
    MBT_LOG2("[BTMTK][AG] Num=%s, Len=%d", Num, Len);
    #endif
	#if defined(AG_USE_CALL_OPTION2)
	if(Num && Len > 0)
		AG_LOG_MSG((MOD_BT, "[AG][INFO] waiting call=%s", Num));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] waiting call is NULL"));
	bHeadset = (g_AgContext.activeProfile==HSP_CONTEXT);
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
 	    !bHeadset &&
 	    Num != NULL &&
 	    Len > 0)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		wait.number = Num;
		wait.classmap = 1;
		wait.type = 129;
		if(Num[0] == '+')
			wait.type = 145;
		btmtk_hfg_call_waiting(pContext, &wait);
		ret = MBT_TRUE;
	}
	#endif
	MBT_AG_UNLOCK();
 	return ret;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendresponse(MBT_BOOL Response)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_RESPONSE, Response?1:0));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendresponse");
    MBT_LOG1("[BTMTK][AG] Response=%s", convert_bool_to_string(Response));
    #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		g_AgContext.bBtrh = MBT_FALSE;
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(Response)
			btmtk_hfg_send_ok(pContext);
		else
			btmtk_hfg_send_error(pContext, 0);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setcind(T_MBT_AG_NETSTATE Net, T_MBT_AG_CALLSTATUS CallState, T_MBT_AG_CALLSETUP SetupState, MBT_BYTE SignalLevel, MBT_BYTE RoamingStatus, MBT_BYTE BatteryLevel,T_MBT_AG_CALL_HELD value)
{
	//T_MBT_AG_INDICATION *pAGIndication;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CIND, Net, CallState, SetupState, SignalLevel, RoamingStatus, BatteryLevel, value));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcind");
    MBT_LOG3("[BTMTK][AG] Net=%s, CallState=%s, SetupState=%s", T_MBT_AG_NETSTATE_tostring(Net), T_MBT_AG_CALLSTATUS_tostring(CallState), T_MBT_AG_CALLSETUP_tostring(SetupState));
    MBT_LOG3("[BTMTK][AG] SignalLevel=%d, RoamingStatus=%d, BatteryLevel=%d", SignalLevel, RoamingStatus, BatteryLevel);
    MBT_LOG1("[BTMTK][AG] value(callheld)=%s", T_MBT_AG_CALL_HELD_tostring(value));
    #endif
	//pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
       sdc_update_net_status(Net);
       sdc_update_call_state(CallState);
       sdc_update_callsetup_state(SetupState);
       sdc_update_callheld_state(value);
       sdc_update_signal_level(SignalLevel);
       sdc_update_roaming_status(RoamingStatus);
       sdc_update_battery_level(BatteryLevel);
	//pAGIndication->bNetStatus = Net;
	//pAGIndication->bCallStatus = CallState;
	//pAGIndication->bCallSetupStatus = SetupState;
	//pAGIndication->bSignalLevel = SignalLevel;
	//pAGIndication->bRoamingStatus = RoamingStatus;
	//pAGIndication->bBatteryLevel = BatteryLevel;
	//pAGIndication->bHeldStatus = value;
	MBT_AG_UNLOCK();
 	return MBT_TRUE;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_BOOL btmtk_ag_setcurrentcalllist(MBT_BYTE Idx, 
                                   T_MBT_AG_CL_DIR Dir, 
                                   T_MBT_AG_CL_STATUS Status,
                                   T_MBT_AG_CL_MODE Mode,
                                   T_MBT_AG_CL_MPTY Mprty,
                                   MBT_CHAR* Num,
                                   MBT_BYTE NumType,
                                   MBT_BYTE Len,MBT_BOOL FinalFlag)
{
	HfgChannelContext		*pContext;
	HfgCallListParms		call;// = {Idx, Dir, Status, Mode, Mprty, (const char*)Num, NumType};
	MBT_BOOL			ret = MBT_FALSE;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CURRENT_CALL_LIST,
								Idx,
								Dir,
								Status,
								Mode,
								Mprty,
								NumType,
								Len,
								FinalFlag));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcurrentcalllist");
    MBT_LOG3("[BTMTK][AG] Idx=%d, Dir=%d, Status=%d", Idx, Dir, Status);
    MBT_LOG2("[BTMTK][AG] Mode=%d, Mprty=%d", Mode, Mprty);
    MBT_LOG3("[BTMTK][AG] Num=%s, NumType=%d, Len=%d", Num, NumType, Len);
    MBT_LOG1("[BTMTK][AG] FinalFlag=%s", convert_bool_to_string(FinalFlag));
    #endif
	if(Num && Num[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] call number=%s", Num));
	if(!Num || Len == 0)
		AG_LOG_MSG((MOD_BT, "[AG][WARN] call number is NULL"));

	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
        //if( Num != NULL && Len > 0)
        if(Num && Num[0])
        {
		call.index = Idx;
		call.dir = Dir;
		call.state = Status;
		call. mode = Mode;
		call.multiParty = Mprty;
		call.number = Num;
		call.type = NumType;
		btmtk_hfg_send_call_list_rsp(pContext, &call, FinalFlag);
        }
        else
        {
            btmtk_hfg_send_ok(pContext);
        }
		ret = MBT_TRUE;
	}
	MBT_AG_UNLOCK();
  	return ret;
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_startvr(MBT_VOID)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_START_VR));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_startvr");
    #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		g_AgContext.bVR = TRUE;
		btmtk_hfg_enable_vr(pContext, TRUE);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_stopvr(MBT_VOID)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_STOP_VR));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_stopvr");
    #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		g_AgContext.bVR = FALSE;
		btmtk_hfg_enable_vr(pContext, FALSE);
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_ringstart(MBT_VOID)
{
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_RING_START));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] btmtk_ag_ringstart is called");
    #endif
	if(!g_AgContext.bRingStarted)
	{
	StartRingTimer();
	g_AgContext.bRingStarted = MBT_TRUE;
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_ringstop(MBT_VOID)
{
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_RING_STOP));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] btmtk_ag_ringstop is called");
    #endif
    if(g_AgContext.bRingStarted)
    {
	g_AgContext.bRingStarted = MBT_FALSE;
	StopRingTimer();
    }
	MBT_AG_UNLOCK();
}

MBT_VOID btmtk_ag_sendsupportedpblist(T_MBT_AG_PB_CPBS supportedPbList)
{
	HfgChannelContext		*pContext;
	HfgPbStorageSupported	pbSupported;
	MBT_BYTE			i;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_SUPPORTED_PB_LIST, supportedPbList.bSupportedNum));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendsupportedpblist");
    MBT_LOG1("[BTMTK][AG] T_MBT_AG_PB_CPBS.bSupportedNum=%d", supportedPbList.bSupportedNum);
    #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		pbSupported.supported = 0;
		for(i = 0;i < supportedPbList.bSupportedNum;i++)
		{
			pbSupported.supported |= mapMBTPbToHfgPb(supportedPbList.pbType[i]);
                    #ifdef BTMTK_WISE_MBT_LOG
                    MBT_LOG2("[BTMTK][AG] %d : pbType=%s", i, T_MBT_AG_PB_DEV_tostring(supportedPbList.pbType[i]));
                    #endif
		}
		AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_SUPPORTED_PB_LIST, pbSupported.supported));
		btmtk_hfg_send_pb_supported_storage_rsp(pContext, &pbSupported);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendselectedpbinfo(T_MBT_AG_PB_RETURN result, MBT_SHORT usedRecs, MBT_SHORT totalRecs)
{
	HfgChannelContext		*pContext;
	HfgPbStorageSelected	pbSelected;// = {0, usedRecs, totalRecs};
	T_MBT_AG_PB_CPBS 	*pAGPbCpbs;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_SELECTED_PB_INFO, result.bResult?1:0, usedRecs, totalRecs));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendselectedpbinfo");
        MBT_LOG3("[BTMTK][AG] result=%s, usedRecs=%d, totalRecs=%d", convert_bool_to_string(result.bResult), usedRecs, totalRecs);
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		pAGPbCpbs = (T_MBT_AG_PB_CPBS*)mbt_sdc_getrecord(MBTSDC_REC_AG_RXCPBS);
		pbSelected.selected = mapMBTPbToHfgPb(pAGPbCpbs->selectedPbType);
		pbSelected.used = usedRecs;
		pbSelected.total = totalRecs;
		if(result.bResult)		
			btmtk_hfg_send_pb_selected_storage_rsp(pContext, &pbSelected);
		else
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbselectresult(T_MBT_AG_PB_RETURN result)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_SELECT_RESULT, result.bResult?1:0, result.bErrorCode));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbselectresult");
        MBT_LOG1("[BTMTK][AG] result=%s", convert_bool_to_string(result.bResult));
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult)
			btmtk_hfg_send_ok(pContext);
		else
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbentriesinfo(T_MBT_AG_PB_RETURN result, MBT_SHORT indexStart, MBT_SHORT indexEnd, MBT_SHORT maxLenNum, MBT_SHORT maxLenTxt)
{
	HfgChannelContext		*pContext;
	HfgPbReadInfo			pbEntryInfo;// = {NULL, maxLenNum, maxLenTxt};
	char					*pRange;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_ENTRIES_INFO, 
								result.bResult?1:0, 
								indexStart,
								indexEnd,
								maxLenNum,
								maxLenTxt));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbentriesinfo");
        MBT_LOG3("[BTMTK][AG] result=%s, indexStart=%d, indexEnd=%d", convert_bool_to_string(result.bResult), indexStart, indexEnd);
        MBT_LOG2("[BTMTK][AG] maxLenNum=%d, maxLenTxt=%d", maxLenNum, maxLenTxt);
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult)
		{
			pRange = (char*)ADP_OS_MemAlloc(10);
			if(pRange)
			{
				ADP_OS_Sprintf(pRange, "(%d-%d)", indexStart, indexEnd);
				pbEntryInfo.range = pRange;
				pbEntryInfo.numLength = maxLenNum;
				pbEntryInfo.textLength = maxLenTxt;
				btmtk_hfg_send_pb_read_info_rsp(pContext, &pbEntryInfo);
				ADP_OS_MemFree(pRange);
			}
		}
		else
		{
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}	
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbreadresult(T_MBT_AG_PB_RETURN result, T_MBT_AG_PB_REC* pbRec,  MBT_BOOL finalEntry)
{
	HfgChannelContext		*pContext;
	HfgPbEntry			pbEntry;// = {pbRec->bIndex, pbRec->bType, (const char*)pbRec->number, (const char*)pbRec->text};

	MBT_AG_LOCK();
	if(pbRec)
	{
		AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_READ_RESULT, 
									result.bResult ? 1 : 0, 
								pbRec->bIndex,
								pbRec->bType,
								finalEntry?1:0));
	}
	else
	{
		AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_READ_RESULT, 
									result.bResult?1:0, 
									0,
									0,
									0));
	}
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbreadresult");
        MBT_LOG2("[BTMTK][AG] result=%s, finalEntry=%s", convert_bool_to_string(result.bResult), convert_bool_to_string(finalEntry));
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult && pbRec)
		{
	if(pbRec->number[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] number=%s", pbRec->number));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] number is NULL"));

	if(pbRec->text[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] text=%s", pbRec->text));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] text is NULL"));	
	
			pbEntry.index = pbRec->bIndex;
			pbEntry.type = pbRec->bType;
			pbEntry.number = pbRec->number;
			pbEntry.text = pbRec->text;			
			btmtk_hfg_send_pb_read_rsp(pContext, &pbEntry, finalEntry);
		}
		else
		{
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}


/********************************************************************************
*   Prototype   : 
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbfindentriesinfo(T_MBT_AG_PB_RETURN result, MBT_SHORT maxLenNum, MBT_SHORT maxLenTxt)
{
	HfgChannelContext		*pContext;
	HfgPbFindInfo			pbEntryInfo;// = {maxLenNum, maxLenTxt};

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_FIND_ENTRIES_INFO, 
								result.bResult?1:0, 
								maxLenNum,
								maxLenTxt));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbfindentriesinfo");
        MBT_LOG1("[BTMTK][AG] result=%s", convert_bool_to_string(result.bResult));
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult)
		{
			pbEntryInfo.numLength = maxLenNum;
			pbEntryInfo.textLength = maxLenTxt;
			btmtk_hfg_send_pb_find_info_rsp(pContext, &pbEntryInfo);
		}
		else
		{
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbfindresult(T_MBT_AG_PB_RETURN result, T_MBT_AG_PB_REC* pbRec, MBT_BOOL finalEntry)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	if(result.bResult && pbRec)
	{
		AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_FIND_RESULT, 
									1, 
								pbRec->bIndex,
								pbRec->bType,
								finalEntry?1:0));
	}
	else
	{
		AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_FIND_RESULT, 
									result.bResult?1:0, 
									0,
									0,
									0));
	}
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbfindresult");
        MBT_LOG2("[BTMTK][AG] result=%s, finalEntry=%s", convert_bool_to_string(result.bResult), convert_bool_to_string(finalEntry));
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult && pbRec)
		{
			HfgPbEntry			pbEntry;

	if(pbRec->number[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] number=%s", pbRec->number));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] number is NULL"));

	if(pbRec->text[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] text=%s", pbRec->text));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] text is NULL"));	

			pbEntry.index = pbRec->bIndex;
			pbEntry.type = pbRec->bType;
			pbEntry.number = pbRec->number;
			pbEntry.text = pbRec->text;
			btmtk_hfg_send_pb_find_rsp(pContext, &pbEntry, finalEntry);
		}
		else
		{
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}


/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbwriteinfo(T_MBT_AG_PB_RETURN result, MBT_SHORT indexStart, MBT_SHORT indexEnd, MBT_SHORT maxlenNum, MBT_SHORT typeStart, MBT_SHORT typeEnd, MBT_SHORT maxlenTxt)
{
	HfgChannelContext		*pContext;
	HfgPbWriteInfo		pbEntryInfo;// = {NULL, maxlenNum, NULL, maxlenTxt};
	char								*range, *types;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_WRITE_INFO, 
								result.bResult?1:0, 
								indexStart,
								indexEnd,
								maxlenNum,
								typeStart,
								typeEnd,
								maxlenTxt));	
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbwriteinfo");
        MBT_LOG1("[BTMTK][AG] result=%s", convert_bool_to_string(result.bResult));
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult)
		{
			range = (char*)ADP_OS_MemAlloc(10);
			types = (char*)ADP_OS_MemAlloc(10);
			if(range && types)
			{
				ADP_OS_Sprintf(range, "(%d-%d)", indexStart, indexEnd);
				ADP_OS_Sprintf(types, "(%d-%d)", typeStart, typeEnd);
				pbEntryInfo.range = range;
				pbEntryInfo.types = types;
				pbEntryInfo.numLength = maxlenNum;
				pbEntryInfo.textLength = maxlenTxt;
				btmtk_hfg_send_pb_write_info_rsp(pContext, &pbEntryInfo);
			}
			if(pbEntryInfo.range)
				ADP_OS_MemFree((void*)pbEntryInfo.range);
			if(pbEntryInfo.types)
				ADP_OS_MemFree((void*)pbEntryInfo.types);
		}
		else
		{
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendpbwriteresult(T_MBT_AG_PB_RETURN result)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_PB_WRITE_RESULT, result.bResult?1:0));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendpbwriteresult");
        MBT_LOG1("[BTMTK][AG] result=%s", convert_bool_to_string(result.bResult));
        #endif
   	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(result.bResult)
		{
			btmtk_hfg_send_ok(pContext);
		}
		else
		{
			btmtk_hfg_send_error(pContext, (U8)result.bErrorCode);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setcgm(MBT_CHAR* manufacturerid, MBT_CHAR* modelid)
{
	T_MBT_AG_CGM_ID *pACgm;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CGM));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcgm");
       #endif
	if(manufacturerid && manufacturerid[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] manufacturerid=%s", manufacturerid));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] manufacturerid is NULL"));

	if(modelid && modelid[0])
		AG_LOG_MSG((MOD_BT, "[AG][INFO] modelid=%s", modelid));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] modelid is NULL"));
	
	pACgm = (T_MBT_AG_CGM_ID*)mbt_sdc_getrecord(MBTSDC_REC_AG_CGM);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGCgm->modelID : \"%s\" => \"%s\"",
                pACgm->modelID,
                modelid?modelid:"");
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGCgm->manufacturerID : \"%s\" => \"%s\"",
                pACgm->manufacturerID,
                manufacturerid?manufacturerid:"");
	if(modelid)
       {   
		ADP_OS_StrCpy(pACgm->modelID, modelid);
       }
	else
       {   
		pACgm->modelID[0] = 0;
       }
	if(manufacturerid)
       {   
		ADP_OS_StrCpy(pACgm->manufacturerID, manufacturerid);
       }
	else
       {   
		pACgm->manufacturerID[0] = 0;	
       }
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setcscs(T_MBT_AG_CSCS* AgCSList)
{   
	T_MBT_AG_CSCS *pAGCscs;
       U8 i;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CSCS));
       #ifdef BTMTK_WISE_MBT_LOG
       MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcscs");
       #endif    
	pAGCscs = (T_MBT_AG_CSCS*)mbt_sdc_getrecord(MBTSDC_REC_AG_CSCS);
       for(i = 0;i < MBT_AG_CSCS_NUM;i++)
       {
             MBT_LOG3("[BTMTK][AG][SDC] sdcAGcscs->AGCharSetList[%d] : %s => %s",
                i,
                pAGCscs->AGCharSetList[i],
                AgCSList->AGCharSetList[i]);
       }
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGcscs->CurrentCharSet : %s => %s",
            pAGCscs->CurrentCharSet,
            AgCSList->CurrentCharSet);
       ADP_OS_MemCopy((void*)pAGCscs->AGCharSetList, (void*)AgCSList->AGCharSetList, sizeof(pAGCscs->AGCharSetList));
       ADP_OS_MemCopy((void*)pAGCscs->CurrentCharSet, (void*)AgCSList->CurrentCharSet, sizeof(pAGCscs->CurrentCharSet));
	//ADP_OS_MemCopy((void*)pAGCscs, (void*)AgCSList, sizeof(T_MBT_AG_CSCS));
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setmultifunckey(MBT_BOOL AGNonDefault)
{
	T_MBT_AG_STATUS *pAGStatus;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_MULTI_FUNCTION_KEY, AGNonDefault?1:0));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setmultifunckey");
        MBT_LOG1("[BTMTK][AG] AGNonDefault=%s", convert_bool_to_string(AGNonDefault));
        #endif    
	pAGStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGStatus->bNonDefaultMultiFuncKey : %s => %s",
            convert_bool_to_string(pAGStatus->bNonDefaultMultiFuncKey),
            convert_bool_to_string(AGNonDefault));
	pAGStatus->bNonDefaultMultiFuncKey = AGNonDefault;
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setcallstatus(T_MBT_AG_CALLSTATUS CurrentStatus)
{
	MBT_BOOL bHeadset;
	HfgChannelContext		*pContext;
	T_MBT_AG_INDICATION *pAGIndication;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CALL_STATUS, CurrentStatus));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcallstatus");
        MBT_LOG1("[BTMTK][AG] CurrentStatus=%s", T_MBT_AG_CALLSTATUS_tostring(CurrentStatus));
        #endif   
	#if defined(AG_CALL_SIMULATION)
	setCallIdx((MBT_BYTE)CurrentStatus);
	#endif
	
#if defined(AG_USE_CALL_OPTION2)
	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
	bHeadset = (g_AgContext.activeProfile==HSP_CONTEXT);
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
 	    !bHeadset)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
        if(pAGIndication->bCallStatus != CurrentStatus)
		btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL, (U8)CurrentStatus);
	}
       sdc_update_call_state(CurrentStatus);
	//pAGIndication->bCallStatus = CurrentStatus;
#endif
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_setcallsetup(T_MBT_AG_CALLSETUP CurrentCallSetup)
{
	MBT_BOOL bHeadset;
	HfgChannelContext		*pContext;
	T_MBT_AG_INDICATION *pAGIndication;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CALLSETUP, CurrentCallSetup));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcallsetup");
        MBT_LOG1("[BTMTK][AG] CurrentCallSetup=%s", g_strCallSetupStatus[CurrentCallSetup]);
        #endif   
#if defined(AG_USE_CALL_OPTION2)
	pAGIndication = (T_MBT_AG_INDICATION*)mbt_sdc_getrecord(MBTSDC_REC_AG_INDICATION);
       sdc_update_callsetup_state(CurrentCallSetup);
	//pAGIndication->bCallSetupStatus = CurrentCallSetup;
	bHeadset = (g_AgContext.activeProfile==HSP_CONTEXT);
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED &&
 	    !bHeadset)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		btmtk_hfg_set_indicator_value(pContext, HFG_IND_CALL_SETUP, (U8)CurrentCallSetup);
		if(CurrentCallSetup == MBT_AG_CALLSETUP_INCOMING && 
		   pAGIndication->bCallStatus == MBT_AG_CALLSTATUS_END)
		{
			btmtk_ag_ringstart();
		}
	}
#endif
	MBT_AG_UNLOCK();
}

/********************************************************************************
*   Description : 
********************************************************************************/
MBT_VOID btmtk_ag_sendbtrh( T_MBT_AG_BTRH value)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_BTRH, value));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendbtrh");
        MBT_LOG1("[BTMTK][AG] value(BTRH)=%s", g_strBtrh[value]);
        #endif 
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(value != MBT_AG_RH_NO_RESPONSE)
		btmtk_hfg_send_response_hold_state(pContext, (U8)value);
		if(g_AgContext.bBtrh != MBT_FALSE)
			btmtk_hfg_send_ok(pContext);
		g_AgContext.bBtrh = MBT_FALSE;
	}
	MBT_AG_UNLOCK();
}
/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendcmserr(T_MBT_AG_CMS_ERR ErrCode)
{
	HfgChannelContext		*pContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_CMS_ERR, ErrCode));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendcmserr");
        MBT_LOG1("[BTMTK][AG] ErrCode=%d", ErrCode);
        #endif 
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		btmtk_hfg_send_sms_error(pContext, (U16)ErrCode);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}	
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_setmsgsvclist(MBT_BYTE Svc, MBT_BOOL CurrentFlag, T_MBT_AG_SM_SVC CurrentSvc)
{
	HfgChannelContext			*pContext;
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
	HfgSMSService_result		result;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_MSG_SVC_LIST, Svc, CurrentFlag?1:0, CurrentSvc));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setmsgsvclist");
        MBT_LOG1("[BTMTK][AG] CurrentFlag=%s", convert_bool_to_string(CurrentFlag));
        #endif
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);

	// Always update current setting
      MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMS.CurrentMsgSvc : 0x%02X => 0x%02X",
            pAGSmsStatus->CSMS.CurrentMsgSvc,
            CurrentSvc);
	pAGSmsStatus->CSMS.CurrentMsgSvc = CurrentSvc;
	if(!CurrentFlag)
	{
	      MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSMS.MsgSvc : 0x%02X => 0x%02X",
                pAGSmsStatus->CSMS.MsgSvc,
                Svc);
		pAGSmsStatus->CSMS.MsgSvc = Svc;
	}
	else
	{
		Svc = pAGSmsStatus->CSMS.MsgSvc;
		if( g_AgContext.activeProfile != NULL_CONTEXT &&
	 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
		{
			pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
			if(CurrentSvc & MBT_AG_SM_SVC_1)
				Svc = (Svc>>4);
			result.bm = (Svc&MBT_AG_SM_SVC_0_BM)?1:0;
			result.mt = (Svc&MBT_AG_SM_SVC_0_MT)?1:0;
			result.mo = (Svc&MBT_AG_SM_SVC_0_MO)?1:0;
			btmtk_hfg_send_sms_select_service_rsp(pContext, &result);
			btmtk_hfg_send_ok(pContext);
		}
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_setmsgstoragelist(T_MBT_AG_SM_CPMS* StorageList)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       U8 i,j;
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_MSG_STO_LIST));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setmsgstoragelist");
    #endif
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       for(i = 0;i < 3;i++)
       {    
            char msg[100];
            MBT_LOG3("[BTMTK][AG][SDC] sdcAGSMS->CPMS.SupportedNum[%d] : %d => %d",
                    i,
                    pAGSmsStatus->CPMS.SupportedNum[i],
                    StorageList->SupportedNum[i]);
            for(j = 0;j < pAGSmsStatus->CPMS.SupportedNum[i];j++)
            {
                sprintf(msg, "[BTMTK][AG][SDC] sdcAGSMS->CPMS.MemType[%d][%d] : %d => %d", i, j,
                        pAGSmsStatus->CPMS.MemType[i][j],
                        StorageList->MemType[i][j]);
                MBT_LOG(msg);
            }
            MBT_LOG3("[BTMTK][AG][SDC] sdcAGSMS->CPMS.CurrentMemType[%d] : %d => %d",
                    i,
                    pAGSmsStatus->CPMS.CurrentMemType[i],
                    StorageList->CurrentMemType[i]);            
       }
    
	ADP_OS_MemCopy((void*)&pAGSmsStatus->CPMS.MemType, StorageList->MemType, sizeof(StorageList->MemType));
	ADP_OS_MemCopy((void*)&pAGSmsStatus->CPMS.SupportedNum, StorageList->SupportedNum, sizeof(StorageList->SupportedNum));
	ADP_OS_MemCopy((void*)&pAGSmsStatus->CPMS.CurrentMemType, StorageList->CurrentMemType, sizeof(StorageList->CurrentMemType));
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsgstorageselectresult(MBT_BYTE MemNum, MBT_BYTE* UsedNum, MBT_BYTE* TotalNum)
{
	HfgChannelContext			*pContext;
	HfgSMSPrefStorage_result	result = {0};
	MBT_BYTE				i;
	U16						*pSto = (U16*)&result.readUsed;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG_STO_SELECT_RESULT, MemNum));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsgstorageselectresult");
        MBT_LOG1("[BTMTK][AG] MemNum=%d", MemNum);
        #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
	       U8 count = 0;
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
              /* Customize for LG request */
              result.count = 0;
              if(g_AgContext.prefMemStorageMask == (READ_MEM_MASK|WRITE_MEM_MASK|RECV_MEM_MASK))
              {
        		for(i = 0;i < MemNum;i++)
        		{
        			AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_MSG_STO_SELECT_RESULT, i+1, UsedNum[i], TotalNum[i]));
        			*(pSto++) = UsedNum[i];
        			*(pSto++) = TotalNum[i];
                            result.count |= (1<<i);
        		}
              }
              else
              {
                    T_MBT_AG_SMS_STATUS *pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
		for(i = 0;(i < 3) && (count < MemNum);i++)
		{
		       if(g_AgContext.prefMemStorageMask & (1<<i))
                    {      
	        		AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_MSG_STO_SELECT_RESULT, i+1, UsedNum[i], TotalNum[i]));
                                  if(pAGSmsStatus->CPMS.CurrentMemType[i] == MBT_AG_SM_MEM_MT)
                                  {
            			            *(pSto++) = 1;
                		    	     *(pSto++) = 100;
                                  }
                                  else if(pAGSmsStatus->CPMS.CurrentMemType[i] == MBT_AG_SM_MEM_ME)
                                  {
            			            *(pSto++) = 3;
                		    	     *(pSto++) = 100;
                                  }
                                  else
                                  {
            			            *(pSto++) = 1;
                		    	     *(pSto++) = 100;
                                  }
                            count++;
                            result.count |= (1<<i);
                    }
                    else
                    {
                        /* mem not specified in AT+CPMS cmd */
			   pSto += 2;
                    }
		}              
              }
              /*
		result.count = MemNum;
		for(i = 0;i < MemNum;i++)
		{
			AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_MSG_STO_SELECT_RESULT, i+1, UsedNum[i], TotalNum[i]));
			*(pSto++) = UsedNum[i];
			*(pSto++) = TotalNum[i];
		}
		*/
		btmtk_hfg_send_sms_select_pref_storage_rsp(pContext, &result);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsgstorageinfo(MBT_BYTE MemNum, T_MBT_AG_SM_STORAGE* MemType, MBT_BYTE* UsedNum, MBT_BYTE* TotalNum)
{		
	HfgChannelContext			*pContext;
	HfgSMSPrefStorage_read	read = {0};
	MBT_BYTE				i = 0;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG_STO_INFO, MemNum));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsgstorageinfo");
        MBT_LOG1("[BTMTK][AG] MemNum=%d", MemNum);
        #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(i < MemNum)
		{
			AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_MSG_STO_INFO, i+1, UsedNum[i], TotalNum[i]));
			read.read = (1<<MemType[i]);
			read.readUsed = UsedNum[i];
			read.readTotal = TotalNum[i];
			i++;
		}
		if(i < MemNum)
		{
			AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_MSG_STO_INFO, i+1, UsedNum[i], TotalNum[i]));
			read.write = (1<<MemType[i]);
			read.writeUsed = UsedNum[i];
			read.writeTotal = TotalNum[i];
			i++;
		}
		if(i < MemNum)
		{
			AG_LOG_INFO((AG_LOG_GROUP_API, BT_HFG_WISE_INFO_MSG_STO_INFO, i+1, UsedNum[i], TotalNum[i]));
			read.recv = (1<<MemType[i]);
			read.recvUsed = UsedNum[i];
			read.recvTotal = TotalNum[i];
		}
		btmtk_hfg_send_sms_selected_pref_storage_rsp(pContext, &read);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_setcmgf(T_MBT_AG_SM_CMGF* MsgFormat)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       U8 i;
       
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CMGF, MsgFormat->CurrentMode));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcmgf");
        #endif
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       for(i = 0;i < MBT_AG_SM_CMGF_NUM;i++)
       {
             MBT_LOG3("[BTMTK][AG][SDC] sdcAGSMS->CMGF.MsgFmtList[%d] : %d => %d",
                    i,
                    pAGSmsStatus->CMGF.MsgFmtList[i],
                    MsgFormat->MsgFmtList[i]);
       }
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CMGF.CurrentMode : %d => %d",
                pAGSmsStatus->CMGF.CurrentMode,
                MsgFormat->CurrentMode);       
	ADP_OS_MemCopy((void*)&pAGSmsStatus->CMGF.MsgFmtList, MsgFormat->MsgFmtList, sizeof(MsgFormat->MsgFmtList));
	pAGSmsStatus->CMGF.CurrentMode = MsgFormat->CurrentMode;
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendsca(T_MBT_AG_SM_CSCA* SMSCAddr)
{
	HfgChannelContext			*pContext;
	HfgSMSSrviceCentre_read	read; // = {SMSCAddr->SMSCAddr, SMSCAddr->AddrType};
	char tmp;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_SCA, 
								SMSCAddr->OptValue,
								SMSCAddr->AddrLen,
								SMSCAddr->AddrType));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendsca");
        #endif
	if(SMSCAddr->SMSCAddr && SMSCAddr->AddrLen > 0)
		AG_LOG_MSG((MOD_BT, "[AG][INFO] SCA=%s", SMSCAddr->SMSCAddr));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] SCA is NULL"));
	
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		read.sca = SMSCAddr->SMSCAddr;
		read.tosca = SMSCAddr->AddrType;
                // In case no terminate char
                tmp = SMSCAddr->SMSCAddr[SMSCAddr->AddrLen];
				SMSCAddr->SMSCAddr[SMSCAddr->AddrLen] = 0;
		btmtk_hfg_send_sms_service_centre_rsp(pContext, &read);
                SMSCAddr->SMSCAddr[SMSCAddr->AddrLen] = tmp;
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendtextmodeparam(T_MBT_AG_SM_CSMP* TextMode)
{
	HfgChannelContext			*pContext;
	HfgSMSTextModeParam_read	read;/* = {	TextMode->FirstOctet, 
									TextMode->IntVP,
									TextMode->TsVP[0] ? TextMode->TsVP : NULL,
									TextMode->PID,
									TextMode->DCS};*/
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_TEXT_MODE_PARAM, 
								TextMode->OptValue,
								TextMode->FirstOctet,
								TextMode->PID,
								TextMode->DCS,
								TextMode->IntVP));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendtextmodeparam");
        #endif
	if(TextMode->TsVP[0] != 0)
		AG_LOG_MSG((MOD_BT, "[AG][INFO] TsVP=%d", TextMode->TsVP));
	
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		read.fo = TextMode->FirstOctet;
		read.intVp = TextMode->IntVP;
		read.strVp = TextMode->TsVP;
		read.pid = TextMode->PID;
		read.dcs = TextMode->DCS;
		btmtk_hfg_send_sms_text_mode_params_rsp(pContext, &read);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_setcsdh(T_MBT_AG_SM_CSDH* ShowMode)
{
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;
       U8 i;
       
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_CSDH, ShowMode->CurrentShow));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setcsdh");
        #endif
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
       for(i = 0;i < MBT_AG_SM_CMGF_NUM;i++)
       {
             MBT_LOG3("[BTMTK][AG][SDC] sdcAGSMS->CSDH.ShowList[%d] : %d => %d",
                    i,
                    pAGSmsStatus->CSDH.ShowList[i],
                    ShowMode->ShowList[i]);
       }
       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CSDH.CurrentShow : %d => %d",
                pAGSmsStatus->CSDH.CurrentShow,
                ShowMode->CurrentShow); 
	ADP_OS_MemCopy((void*)pAGSmsStatus->CSDH.ShowList, ShowMode->ShowList, sizeof(ShowMode->ShowList));
	pAGSmsStatus->CSDH.CurrentShow = ShowMode->CurrentShow;
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_setmsgsettinglist(MBT_BYTE SettingLen, MBT_CHAR* Setting, MBT_BOOL CurrentFlag, T_MBT_AG_SM_CNMI_SET* CurrentSet)
{
	HfgChannelContext			*pContext;
	T_MBT_AG_SMS_STATUS *pAGSmsStatus;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SET_MSG_SETTING_LIST,
								SettingLen,
								CurrentFlag?1:0,
								CurrentSet->Mode,
								CurrentSet->Mt,
								CurrentSet->Bm,
								CurrentSet->Ds,
								CurrentSet->Bfr));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_setmsgsettinglist");
        MBT_LOG1("[BTMTK][AG] CurrentFlag=%s", convert_bool_to_string(CurrentFlag));
        #endif

	if(Setting && SettingLen > 0)
		AG_LOG_MSG((MOD_BT, "[AG][INFO] Setting=%s", Setting));
	else
		AG_LOG_MSG((MOD_BT, "[AG][WARN] Setting is empty"));
	
	pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
	if(CurrentFlag == MBT_FALSE && Setting != MBT_NULL && SettingLen > 0)
	{
	       char msg[MBT_AG_SM_CNMI_MAXLEN+1];
              char msg1[MBT_AG_SM_CNMI_MAXLEN+1];
	       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.SettingLen : %d => %d",
                pAGSmsStatus->CNMI.SettingLen,
                SettingLen);
              if(pAGSmsStatus->CNMI.SettingLen)
                ADP_OS_MemCopy((void*)msg, pAGSmsStatus->CNMI.Setting, pAGSmsStatus->CNMI.SettingLen);
              msg[pAGSmsStatus->CNMI.SettingLen] = 0;
              if(SettingLen)
                ADP_OS_MemCopy((void*)msg1, Setting, SettingLen);
              msg1[SettingLen] = 0;
	       MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.Setting : \"%s\" => \"%s\"",
                msg,
                msg1); 
		ADP_OS_MemCopy((void*)pAGSmsStatus->CNMI.Setting, Setting, sizeof(pAGSmsStatus->CNMI.Setting));
		pAGSmsStatus->CNMI.SettingLen = SettingLen;
	}

	// UPdate current setting no matter the CurrentFlag is TRUE or FALSE
	if(CurrentSet)
       {   
             MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.CurrentSet.Mode : %d => %d",
                    pAGSmsStatus->CNMI.SelectedSet.Mode,
                    CurrentSet->Mode);
             MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.CurrentSet.Mt : %d => %d",
                    pAGSmsStatus->CNMI.SelectedSet.Mt,
                    CurrentSet->Mt);
             MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.CurrentSet.Bm : %d => %d",
                    pAGSmsStatus->CNMI.SelectedSet.Bm,
                    CurrentSet->Bm);
             MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.CurrentSet.Ds : %d => %d",
                    pAGSmsStatus->CNMI.SelectedSet.Ds,
                    CurrentSet->Ds);
             MBT_LOG2("[BTMTK][AG][SDC] sdcAGSMS->CNMI.CurrentSet.Bfr : %d => %d",
                    pAGSmsStatus->CNMI.SelectedSet.Bfr,
                    CurrentSet->Bfr);
		ADP_OS_MemCopy((void*)&pAGSmsStatus->CNMI.CurrentSet, CurrentSet, sizeof(pAGSmsStatus->CNMI.CurrentSet));
       }
	
	if(CurrentFlag)
	{
		if( g_AgContext.activeProfile != NULL_CONTEXT &&
	 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
		{
			pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
			btmtk_hfg_send_ok(pContext);
		}
	}

	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsgindication(T_MBT_AG_SM_STORAGE MemType, MBT_SHORT Index)
{
	HfgChannelContext			*pContext;
	HfgSMSNewMsgInd			ind;// = {(1<<MemType), Index};

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG_INDICATION, MemType, Index));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsgindication");
        MBT_LOG2("[BTMTK][AG] MemType=%d, Index=%d", MemType, Index);
        #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		ind.mem = (1<<MemType);
		ind.index = Index;
		btmtk_hfg_send_sms_new_msg_indication(pContext, &ind);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsgstatuslist(MBT_BYTE StatusNum, T_MBT_AG_SM_STATUS* StatusList)
{
	HfgChannelContext			*pContext;
	T_MBT_AG_SMS_STATUS 	*pAGSmsStatus;
	MBT_BYTE 				i = 0, j = 0;
	char						*pList, *ptr;
	const char				*pStatus;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG_STATUS_LIST, StatusNum));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsgstatuslist");
        MBT_LOG1("[BTMTK][AG] StatusNum=%d", StatusNum);
        #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		pList = (char*)ADP_OS_MemAlloc(40);
		ptr = pList;
		if(pList)
		{
			pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
			*(ptr++) = '(';
			if(pAGSmsStatus->CMGF.CurrentMode == MBT_AG_SM_MGF_PDU)
			{
				for(;i < StatusNum;i++)
				{
					if(i > 0)
						*(ptr++) = ',';
					*(ptr++) = StatusList[i]+'0';
				}
			}
			else
			{
				for(;i < StatusNum;i++)
				{
					if(i > 0)
						*(ptr++) = ',';	
					pStatus = statusToString(StatusList[i]);
					if(pStatus == NULL)
					{
						ADP_OS_MemFree(ptr);
						MBT_AG_UNLOCK();
						return;
					}
					*(ptr++) = '\"';
					ADP_OS_StrCpy(ptr, pStatus);
					ptr += ADP_OS_StrLen(pStatus);
					*(ptr++) = '\"';
				}				
			}
			*(ptr++) = ')';
			*(ptr++) = 0;
			//AG_LOG_INFO((AG_LOG_GROUP_INFO, BT_HFG_WISE_INFO_MSG_STATUS_LIST, pList?pList:""));
			AG_LOG_MSG((MOD_BT, "[AG][INFO] status list=%s", pList));
			btmtk_hfg_send_sms_supported_list_status_rsp(pContext, pList);
			ADP_OS_MemFree(pList);
		}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsglist(MBT_BOOL NoMsgFlag, T_MBT_AG_SM_CMGL* Item, MBT_BOOL FinalFlag)
{
	HfgChannelContext			*pContext;
	HfgSMSList_result 			result;
	T_MBT_AG_SMS_STATUS	*pAGSmsStatus;
	MBT_CHAR				*pduBuf = NULL;
	MBT_SHORT				msgLen;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG_LIST, 
								NoMsgFlag?1:0,
								Item->SMType,
								Item->Index,
								Item->Status,
								FinalFlag?1:0));
        #ifdef BTMTK_WISE_MBT_LOG
        MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsglist");
        MBT_LOG2("[BTMTK][AG] NoMsgFlag=%s, FinalFlag=%s", convert_bool_to_string(NoMsgFlag), convert_bool_to_string(FinalFlag));
        #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(NoMsgFlag || Item == NULL)
		{
			if(FinalFlag)
				btmtk_hfg_send_ok(pContext);
		}
		else
		{
		result.type = Item->SMType;
		result.index = Item->Index;
		result.stat = Item->Status;
		switch(Item->SMType)
		{
		case MBT_AG_SM_TYPE_DELIVER:
		case MBT_AG_SM_TYPE_SUBMIT:				// SMS-SUBMIT (MO)
			result.msg.deliver.mask = 0;
			result.msg.deliver.addr = Item->Type.Deliver.Addr;
			result.msg.deliver.data = Item->Type.Deliver.Data;
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_ALPAH)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_ALPHA;
				result.msg.deliver.alpha = Item->Type.Deliver.AddrStrType;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_SCTS)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_SCTS;
				result.msg.deliver.SCTimeStamp = Item->Type.Deliver.SCTimeStamp;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_TOADDR)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_TOADDR;
				result.msg.deliver.addrType = Item->Type.Deliver.AddrType;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_LENGTH)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_LENGTH;
				result.msg.deliver.length = Item->Type.Deliver.DataLen;
			}
			break;
		case MBT_AG_SM_TYPE_STATUS:				// SMS-STATUS-REPORT
			result.msg.status.mask = 0;
			result.msg.status.fo = Item->Type.Status.FirstOctet;
			result.msg.status.mr = Item->Type.Status.MsgRef;
			result.msg.status.SCTimeStamp = Item->Type.Status.SCTimeStamp;
			result.msg.status.dt = Item->Type.Status.DischargeTime;
			result.msg.status.st = Item->Type.Status.OrgStatus;
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_RA)
			{
				result.msg.status.mask |= HFG_SMS_MASK_RA;
				result.msg.status.ra = Item->Type.Status.RecipAddr;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_TORA)
			{
				result.msg.status.mask |= HFG_SMS_MASK_TORA;
				result.msg.status.addrType = Item->Type.Status.RecipAddrType;
			}
			break;
		case MBT_AG_SM_TYPE_COMMAND:			// SMS-COMMAND
			result.msg.cmd.fo = Item->Type.Cmd.FirstOctet;
			result.msg.cmd.ct = Item->Type.Cmd.Ct;
			break;
		case MBT_AG_SM_TYPE_CBM:				// SMS-CBM
			result.msg.cbm.sn = Item->Type.Cbm.SerialNum;
			result.msg.cbm.mid = Item->Type.Cbm.MessageId;
			result.msg.cbm.page = Item->Type.Cbm.Page;
			result.msg.cbm.data = Item->Type.Cbm.Data;
			break;
		case MBT_AG_SM_TYPE_PDU:
			result.msg.pdu.mask = 0;
			if(Item->Type.Pdu.PDU && Item->Type.Pdu.PDULen > 0)
			{
					pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
					msgLen = Item->Type.Pdu.PDULen;
					if(pAGSmsStatus->CPMS.CurrentMemType[0] != MBT_AG_SM_MEM_BM)
						msgLen += Item->Type.Pdu.PDU[0]+1;	// add SMSC length plus the byte that indicate SMSC length
					pduBuf = (MBT_CHAR*)ADP_OS_MemAlloc(msgLen*2+1);
				if(pduBuf)
						PduToString(Item->Type.Pdu.PDU, pduBuf, msgLen);
			}
			result.msg.pdu.PDU = pduBuf;
			result.msg.pdu.length = Item->Type.Pdu.PDULen;			
			if(Item->Type.Pdu.OptValue & MBT_AG_SM_OPT_ALPAH)
			{
				result.msg.status.mask |= HFG_SMS_MASK_ALPHA;
				result.msg.pdu.alpha = Item->Type.Pdu.AddrStrType;
			}		
			break;
		default:
			MBT_AG_UNLOCK();
			return;
		}	
		btmtk_hfg_send_sms_list_msg_rsp(pContext, &result, FinalFlag);
		if(pduBuf)
			ADP_OS_MemFree(pduBuf);
	}
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsg(T_MBT_AG_SM_CMGR* Item)
{
	HfgChannelContext			*pContext;
	HfgSMSRead_result 		result;
	T_MBT_AG_SMS_STATUS	*pAGSmsStatus;
	MBT_CHAR				*pduBuf = NULL;
	MBT_SHORT				msgLen;
	
	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG, Item->SMType, Item->Status));
    #ifdef BTMTK_WISE_MBT_LOG
    MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsglist");
    #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		result.type = Item->SMType;
		result.stat = Item->Status;
		switch(Item->SMType)
		{
		case MBT_AG_SM_TYPE_DELIVER:
			result.msg.deliver.mask = 0;
			result.msg.deliver.addr = Item->Type.Deliver.Addr;
			result.msg.deliver.SCTimeStamp = Item->Type.Deliver.SCTimeStamp;
			result.msg.deliver.data = Item->Type.Deliver.Data;
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_ALPAH)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_ALPHA;
				result.msg.deliver.alpha = Item->Type.Deliver.AddrStrType;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_TOADDR)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_TOADDR;
				result.msg.deliver.addrType = Item->Type.Deliver.AddrType;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_FO)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_FO;
				result.msg.deliver.fo = Item->Type.Deliver.FirstOctet;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_PID)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_PID;
				result.msg.deliver.pid = Item->Type.Deliver.PID;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_DCS)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_DCS;
				result.msg.deliver.dcs = Item->Type.Deliver.DCS;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_SCA)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_SCA;
				result.msg.deliver.SCAddr = Item->Type.Deliver.SCAddr;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_TOSCA)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_TOSCA;
				result.msg.deliver.SCAddrType = Item->Type.Deliver.SCAddrType;
			}
			if(Item->Type.Deliver.OptValue & MBT_AG_SM_OPT_LENGTH)
			{
				result.msg.deliver.mask |= HFG_SMS_MASK_LENGTH;
				result.msg.deliver.length = Item->Type.Deliver.DataLen;
			}
			break;
 		case MBT_AG_SM_TYPE_SUBMIT:				// SMS-SUBMIT (MO)
 			result.msg.submit.mask = 0;
			result.msg.submit.addr = Item->Type.Submit.Addr;
			result.msg.submit.data = Item->Type.Submit.Data;
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_ALPAH)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_ALPHA;
				result.msg.submit.alpha = Item->Type.Submit.AddrStrType;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_TOADDR)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_TOADDR;
				result.msg.submit.addrType = Item->Type.Submit.AddrType;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_FO)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_FO;
				result.msg.submit.fo = Item->Type.Submit.FirstOctet;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_PID)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_PID;
				result.msg.submit.pid = Item->Type.Submit.PID;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_DCS)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_DCS;
				result.msg.submit.dcs = Item->Type.Submit.DCS;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_VP)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_DCS;
				if( (Item->Type.Submit.FirstOctet &  MBT_AG_SM_FO_VP_TS) ==  MBT_AG_SM_FO_VP_INT)
				{
					result.msg.submit.intVp= Item->Type.Submit.IntVP;
				}
				else
				{
					result.msg.submit.strVp = Item->Type.Submit.TsVP;
				}
			}			
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_SCA)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_SCA;
				result.msg.submit.SCAddr = Item->Type.Submit.SCAddr;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_TOSCA)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_TOSCA;
				result.msg.submit.SCAddrType = Item->Type.Submit.SCAddrType;
			}
			if(Item->Type.Submit.OptValue & MBT_AG_SM_OPT_LENGTH)
			{
				result.msg.submit.mask |= HFG_SMS_MASK_LENGTH;
				result.msg.submit.length = Item->Type.Submit.DataLen;
			}
 			break;
		case MBT_AG_SM_TYPE_STATUS:				// SMS-STATUS-REPORT
			result.msg.status.mask = 0;
			result.msg.status.fo = Item->Type.Status.FirstOctet;
			result.msg.status.mr = Item->Type.Status.MsgRef;
			result.msg.status.SCTimeStamp = Item->Type.Status.SCTimeStamp;
			result.msg.status.dt = Item->Type.Status.DischargeTime;
			result.msg.status.st = Item->Type.Status.OrgStatus;
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_RA)
			{
				result.msg.status.mask |= HFG_SMS_MASK_RA;
				result.msg.status.ra = Item->Type.Status.RecipAddr;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_TORA)
			{
				result.msg.status.mask |= HFG_SMS_MASK_TORA;
				result.msg.status.addrType = Item->Type.Status.RecipAddrType;
			}
			break;
		case MBT_AG_SM_TYPE_COMMAND:			// SMS-COMMAND
			result.msg.cmd.mask = 0;
			result.msg.cmd.fo = Item->Type.Cmd.FirstOctet;
			result.msg.cmd.ct = Item->Type.Cmd.Ct;
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_PID)
			{
				result.msg.status.mask |= HFG_SMS_MASK_PID;
				result.msg.cmd.pid = Item->Type.Cmd.PID;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_MN)
			{
				result.msg.status.mask |= HFG_SMS_MASK_MN;
				result.msg.cmd.mn = Item->Type.Cmd.MsgNum;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_ADDR)
			{
				result.msg.status.mask |= HFG_SMS_MASK_ADDR;
				result.msg.cmd.destAddr = Item->Type.Cmd.Addr;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_TOADDR)
			{
				result.msg.status.mask |= HFG_SMS_MASK_TOADDR;
				result.msg.cmd.addrType = Item->Type.Cmd.AddrType;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_LENGTH)
			{
				result.msg.status.mask |= HFG_SMS_MASK_LENGTH;
				result.msg.cmd.length = Item->Type.Cmd.CDataLen;
			}
			if(Item->Type.Status.OptValue & MBT_AG_SM_OPT_CDATA)
			{
				result.msg.status.mask |= HFG_SMS_MASK_CDATA;
				result.msg.cmd.cmdData = Item->Type.Cmd.CData;
			}
			break;
		case MBT_AG_SM_TYPE_CBM:				// SMS-CBM
			result.msg.cbm.sn = Item->Type.Cbm.SerialNum;
			result.msg.cbm.mid = Item->Type.Cbm.MessageId;
			result.msg.cbm.dcs = Item->Type.Cbm.DCS;
			result.msg.cbm.page = Item->Type.Cbm.Page;
			result.msg.cbm.data = Item->Type.Cbm.Data;
			break;
		case MBT_AG_SM_TYPE_PDU:
			result.msg.pdu.mask = 0;
			if(Item->Type.Pdu.PDU && Item->Type.Pdu.PDULen > 0)
			{
				pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
				msgLen = Item->Type.Pdu.PDULen;
				if(pAGSmsStatus->CPMS.CurrentMemType[0] != MBT_AG_SM_MEM_BM)
					msgLen += Item->Type.Pdu.PDU[0]+1;	// add SMSC length plus the byte that indicate SMSC length
				pduBuf = (MBT_CHAR*)ADP_OS_MemAlloc(msgLen*2+1);
				if(pduBuf)
					PduToString(Item->Type.Pdu.PDU, pduBuf, msgLen);
			}
			result.msg.pdu.PDU = pduBuf;
			result.msg.pdu.length = Item->Type.Pdu.PDULen;
			if(Item->Type.Pdu.OptValue & MBT_AG_SM_OPT_ALPAH)
			{
				result.msg.status.mask |= HFG_SMS_MASK_ALPHA;
				result.msg.pdu.alpha = Item->Type.Pdu.AddrStrType;
			}		
			break;
		default:
			MBT_AG_UNLOCK();
			return;
		}
		btmtk_hfg_send_sms_read_msg_rsp(pContext, &result);
		if(pduBuf)
			ADP_OS_MemFree(pduBuf);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
MBT_VOID btmtk_ag_sendmsgresult(MBT_BYTE MsgRef, MBT_CHAR* SCTimeStamp, MBT_CHAR* AckPdu)
{
	HfgChannelContext			*pContext;
	T_MBT_AG_SMS_STATUS 	*pAGSmsStatus;
	HfgSMSSend_result			result;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_SEND_MSG_RESULT, MsgRef));
      #ifdef BTMTK_WISE_MBT_LOG
      MBT_LOG("[BTMTK][AG] AP calls btmtk_ag_sendmsgresult");
      #endif
	if( g_AgContext.activeProfile != NULL_CONTEXT &&
 	    g_AgContext.state[g_AgContext.activeProfile] >= PROFILE_STATE_CONNECTED)
	{
		pContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		pAGSmsStatus = (T_MBT_AG_SMS_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_SMS);
		result.mr = MsgRef;
		result.mask = 0;
		if(pAGSmsStatus->CMGF.CurrentMode == MBT_AG_SM_MGF_PDU)
		{
			if(AckPdu)
			{
				result.ackpdu = AckPdu;
				result.mask = HFG_SMS_MASK_ACKPDU;
			}
		}
		else
		{
			if(AckPdu)
			{
				result.scts = SCTimeStamp;
				result.mask = HFG_SMS_MASK_SCTS;
			}
		}
		btmtk_hfg_send_sms_send_msg_rsp(pContext, &result);
	}
	else
	{
		AG_LOG_ERR((AG_LOG_GROUP_API, BT_HFG_WISE_ERR_NO_CONNECTION));
	}
	MBT_AG_UNLOCK();
}

/********************************************************************************
*	Prototype	:
*	Description	:
********************************************************************************/
static MBT_BOOL btmtk_is_dev_connected( MBT_SERVICE_ID svcID, T_MBT_BDADDR RemoteBDAddr )
{
	U8 profile = (svcID==MBT_SVCUUID_HEADSET_AUDIO_GATEWAY) ? HSP_CONTEXT : HFP_CONTEXT;
	if ( g_AgContext.state[profile] >= PROFILE_STATE_CONNECTED )
	{
		T_MBT_AG_STATUS *pAgStatus;
		pAgStatus = (T_MBT_AG_STATUS*)mbt_sdc_getrecord(MBTSDC_REC_AG_STATUS_TYPE);	
		if( pAgStatus && 0 == ADP_OS_MemCmp(pAgStatus->BdAddr, RemoteBDAddr, sizeof(T_MBT_BDADDR)) )
			return MBT_TRUE;
	}
	return MBT_FALSE;
}
MBT_BOOL btmtk_hsag_is_dev_connected(T_MBT_BDADDR RemoteBDAddr)
{
	return btmtk_is_dev_connected(MBT_SVCUUID_HEADSET_AUDIO_GATEWAY, RemoteBDAddr);
}

MBT_BOOL btmtk_hfag_is_dev_connected(T_MBT_BDADDR RemoteBDAddr)
{
	return btmtk_is_dev_connected(MBT_SVCUUID_AG_HANDSFREE, RemoteBDAddr);
}

MBT_BOOL btmtk_hsag_is_connected(void)
{
	MBT_BOOL ret= MBT_FALSE;
	
	if( g_AgContext.state[HSP_CONTEXT] >= PROFILE_STATE_CONNECTED )
	    ret = MBT_TRUE;

	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_IS_HSAG_CONNECTED, ret?1:0));
	return ret;
}


MBT_BOOL btmtk_hfag_is_connected(void)
{
	MBT_BOOL ret= MBT_FALSE;
	if( g_AgContext.state[HFP_CONTEXT] >= PROFILE_STATE_CONNECTED )
	    ret = MBT_TRUE;
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_IS_HFAG_CONNECTED, ret?1:0));	
	return ret;
}

void btmtk_hfag_authorize_res(MBT_BYTE AuthMode)
{
	HfgChannelContext *pHfgContext;

	MBT_AG_LOCK();
	AG_LOG_API((AG_LOG_GROUP_API, BT_HFG_WISE_API_HFAG_AUTHORIZE_RES, AuthMode));
	if( g_AgContext.activeProfile != NULL_CONTEXT && 
	    g_AgContext.state[g_AgContext.activeProfile] == PROFILE_STATE_AUTHENTICATE)
	{
		pHfgContext = g_AgContext.pHfgContext[g_AgContext.activeProfile];
		if(AuthMode)
		{
			btmtk_hfg_accept_connect(pHfgContext);
			changeProfileState( g_AgContext.activeProfile, PROFILE_STATE_ACCEPTING);
		}
		else
		{
			btmtk_hfg_reject_connect(pHfgContext);
			changeProfileState( g_AgContext.activeProfile, PROFILE_STATE_REJECTING);
		}
	}
	MBT_AG_UNLOCK();
}

void btmtk_hsag_authorize_res(MBT_BYTE AuthMode)
{
	//HfgChannelContext *pHfgContext;
	btmtk_hfag_authorize_res(AuthMode);
}
/***************************************************************************************
* Simulating call status
****************************************************************************************/
#if defined(AG_CALL_SIMULATION)
static T_MBT_AG_PHONE_CALLSTATE g_callStatus[20];
static MBT_BYTE g_callcount[6];
static MBT_BYTE g_callIdx;
MBT_VOID resetAllCallState(MBT_VOID)
{
	MBT_BYTE i;
	OS_Report("[AG][CALL] resetAllCallState");
	ADP_OS_MemSet((void*)g_callcount,0,sizeof(g_callcount));
	g_callcount[MBT_AG_PHONE_CALL_NONE] = sizeof(g_callStatus)/sizeof(g_callStatus[0]);
	for(i = 0;i < (sizeof(g_callStatus)/sizeof(g_callStatus[0]));i++)
		g_callStatus[i] = MBT_AG_PHONE_CALL_NONE;
}

MBT_VOID setCallIdx(MBT_BYTE idx)
{
	OS_Report("[AG][CALL] setCallIdx(%d)", idx);
	g_callIdx = idx;
}

MBT_VOID changeCallState(T_MBT_AG_PHONE_CALLSTATE newState)
{
	OS_Report("[AG][CALL] changeCallState(%d)", newState);
	g_callcount[(MBT_BYTE)g_callStatus[g_callIdx]]--;
	g_callStatus[g_callIdx] = newState;
	g_callcount[(MBT_BYTE)newState]++;
}

MBT_BYTE getCallCount(T_MBT_AG_PHONE_CALLSTATE state)
{
	return g_callcount[(MBT_BYTE)state];
}

T_MBT_AG_PHONE_CALLSTATE getSelCallState(MBT_VOID)
{
	return g_callStatus[g_callIdx];
}

#endif


