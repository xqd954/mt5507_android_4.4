/************************************************************************************
 *
 *  Filename:      btif_hf.c
 *
 *  Description:   Handsfree Profile Bluetooth Interface
 *
 *
 ***********************************************************************************/

#include <hardware/bluetooth.h>
#include <hardware/bt_hf.h>
#include "btif_common.h"
#include "bt_ext_apis.h"
#include "bt_ext_debug.h"
#include "bt_ext_common.h"
#include "bt_utils.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_hfg_struct.h"
#include "bt_hfg_struct.h"
#include "bt_hfg_api.h"
#include "bt_a2dp_api.h"
#include "bt_profile_manager.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "BTIF_HF"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
#define CASE_RETURN_STR(const) case const: return #const;
#define RING_INTERVAL     (3 * SECTICKS)
#define WAIT_TIME_SECONDS  2
#define HFG_API_SOCK_ADDR "bthfg"
    
#define HSP_CONTEXT       0
#define HFP_CONTEXT       1
#define NUM_OF_CONTEXT    2

/* IOT Device List */
#define IOT_DEV_BLUECHIP "BLUECHIP"

/************************************************************************************
**  Local type definitions
************************************************************************************/

/************************************************************************************
**  Static variables
************************************************************************************/       
static bthf_callbacks_t *bt_hf_callbacks = NULL;

#define CHECK_BTHF_INIT() if (bt_hf_callbacks == NULL)\
    {\
        ALOGE("%s: BTHF not initialized", __FUNCTION__);\
        return BT_STATUS_NOT_READY;\
    }\
    else\
    {\
        ALOGI("%s", __FUNCTION__);\
    }

#define CHECK_BTHF_SLC_CONNECTED() if (bt_hf_callbacks == NULL)\
    {\
        ALOGE("%s: BTHF not initialized", __FUNCTION__);\
        return BT_STATUS_NOT_READY;\
    }\
    else if (btif_hf_cb.state != BTHF_CONNECTION_STATE_SLC_CONNECTED)\
    {\
        ALOGE("%s: SLC connection not up. state=%s", __FUNCTION__, dump_conn_state(btif_hf_cb.state));\
        return BT_STATUS_NOT_READY;\
    }\
    else\
    {\
        ALOGI("%s", __FUNCTION__);\
    }

typedef char bdstr_t[18];    

/* BTIF-HF control block to map bdaddr to BTA handle */
typedef struct _btif_hf_cb
{
    bt_bdaddr_t             connected_bda;
    bthf_connection_state_t state;
    HfgHandsFreeFeatures    peer_feat;
    int                     num_active;
    int                     num_held;  
    bthf_call_state_t       call_setup_state;
    pthread_cond_t          disable_cond;  
    pthread_cond_t          av_suspend_cond;
    pthread_mutex_t         disable_mutex;      
    pthread_mutex_t         av_suspend_mutex;    
    BT_BOOL                 disabled;    
    BT_BOOL                 sco_on_desired;
    BT_BOOL                 sco_in_progress;
    char                    name[BTBM_ADP_MAX_NAME_LEN + 1];
} btif_hf_cb_t;

static btif_hf_cb_t btif_hf_cb;
static HfgChannelContext *g_pContext[NUM_OF_CONTEXT];
static HfgChannelContext *g_pActiveContext = NULL;   
static int g_hfgsock = 0;   

/************************************************************************************
**  Static functions
************************************************************************************/

/************************************************************************************
**  Externs
************************************************************************************/
extern int btmtk_a2dp_suspend(void);
extern void btmtk_a2dp_resume(void);
extern void btmtk_a2dp_app_registerHfgCallback(bt_suspend_callback callback);

/************************************************************************************
**  Functions
************************************************************************************/
const char* dump_msg(unsigned long msg_id)
{
    switch (msg_id)
    {
    CASE_RETURN_STR(MSG_ID_BT_HFG_ACTIVATE_CNF)
    CASE_RETURN_STR(MSG_ID_BT_HFG_DEACTIVATE_CNF)
    CASE_RETURN_STR(MSG_ID_BT_HFG_CONNECT_CNF)
    CASE_RETURN_STR(MSG_ID_BT_HFG_DISCONNECT_CNF)
    CASE_RETURN_STR(MSG_ID_BT_HFG_SCO_CONNECT_CNF)
    CASE_RETURN_STR(MSG_ID_BT_HFG_SCO_DISCONNECT_CNF)        
    CASE_RETURN_STR(MSG_ID_BT_HFG_RFCOMM_CONNECTED_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_CONNECTED_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_DISCONNECTED_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_SCO_CONNECTED_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_SCO_DISCONNECTED_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_HANDSFREE_FEATURES_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_QUERY_INDICATOR_VALUE_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_ENABLE_EXTENDED_ERRORS_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_ANSWER_CALL_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_DIAL_NUMBER_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_MEMORY_DIAL_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_REDIAL_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_CALL_HOLD_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_QUERY_RESPONSE_HOLD_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_SET_RESPONSE_HOLD_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_HANGUP_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_GENERATE_DTMF_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_GET_LAST_VOICE_TAG_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_LIST_CURRENT_CALLS_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_QUERY_SUBSCRIBER_NUMBER_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_QUERY_NETWORK_OPERATOR_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_REPORT_MIC_VOLUME_IND)
    CASE_RETURN_STR(MSG_ID_BT_HFG_REPORT_SPK_VOLUME_IND)        
    CASE_RETURN_STR(MSG_ID_BT_HFG_ENABLE_CALLER_ID_IND)     
    CASE_RETURN_STR(MSG_ID_BT_HFG_ENABLE_CALL_WAITING_IND)     
    CASE_RETURN_STR(MSG_ID_BT_HFG_DISABLE_NREC_IND)     
    CASE_RETURN_STR(MSG_ID_BT_HFG_ENABLE_VOICE_RECOGNITION_IND)     
    CASE_RETURN_STR(MSG_ID_BT_HFG_KEYPAD_CONTROL_IND)     
    CASE_RETURN_STR(MSG_ID_BT_HFG_AT_COMMAND_DATA_IND)         
    default:
        return "UNKNOWN MSG ID";
    }
}

const char* dump_disc_reason(uint8_t reason)
{
    switch (reason)
    {
    CASE_RETURN_STR(HFG_DISCREASON_NORMAL)
    CASE_RETURN_STR(HFG_DISCREASON_ACL_CONN)
    CASE_RETURN_STR(HFG_DISCREASON_ACL_DISC)
    CASE_RETURN_STR(HFG_DISCREASON_SDP)
    CASE_RETURN_STR(HFG_DISCREASON_RFCOMM)
    default:
        return "UNKNOWN DISC REASON";
    }
}

const char* dump_conn_state(uint16_t conn_state)
{
    switch (conn_state)
    {
    CASE_RETURN_STR(BTHF_CONNECTION_STATE_DISCONNECTED)
    CASE_RETURN_STR(BTHF_CONNECTION_STATE_CONNECTING)
    CASE_RETURN_STR(BTHF_CONNECTION_STATE_CONNECTED)
    CASE_RETURN_STR(BTHF_CONNECTION_STATE_SLC_CONNECTED)
    CASE_RETURN_STR(BTHF_CONNECTION_STATE_DISCONNECTING)
    default:
        return "UNKNOWN CONN STATE";
    }
}

const char* dump_call_state(bthf_call_state_t call_state)
{
    switch(call_state)
    {
    CASE_RETURN_STR(BTHF_CALL_STATE_IDLE)
    CASE_RETURN_STR(BTHF_CALL_STATE_HELD)
    CASE_RETURN_STR(BTHF_CALL_STATE_DIALING)
    CASE_RETURN_STR(BTHF_CALL_STATE_ALERTING)
    CASE_RETURN_STR(BTHF_CALL_STATE_INCOMING)
    CASE_RETURN_STR(BTHF_CALL_STATE_WAITING)
    CASE_RETURN_STR(BTHF_CALL_STATE_ACTIVE)
    default:
        return "UNKNOWN CALL STATE";
    }
}

const char* dump_network_state(bthf_network_state_t ntk_state)
{
    switch(ntk_state)
    {
    CASE_RETURN_STR(BTHF_NETWORK_STATE_NOT_AVAILABLE)
    CASE_RETURN_STR(BTHF_NETWORK_STATE_AVAILABLE)
    default:
        return "UNKNOWN NETWORK STATE";
    }
}

const char* dump_service_type(bthf_service_type_t svc_type)
{
    switch(svc_type)
    {
    CASE_RETURN_STR(BTHF_SERVICE_TYPE_HOME)
    CASE_RETURN_STR(BTHF_SERVICE_TYPE_ROAMING)
    default:
        return "UNKNOWN SERVICE STATE";
    }
}

const char* dump_av_state(btif_av_state_t state)
{
    switch(state)
    {
    CASE_RETURN_STR(BTIF_AV_STATE_IDLE)
    CASE_RETURN_STR(BTIF_AV_STATE_OPENING)
    CASE_RETURN_STR(BTIF_AV_STATE_OPENED)
    CASE_RETURN_STR(BTIF_AV_STATE_STARTED)
    CASE_RETURN_STR(BTIF_AV_STATE_CLOSING)
    default:
        return "UNKNOWN AV STATE";
    }
}

static void set_connection_state(bthf_connection_state_t state)
{
    bt_addr_struct converted_addr;
    ProfileStatus target = PROFILE_STATE_DISCONNECTED;
    
    btif_hf_cb.state = state;
    
    switch (state)
    {
    case BTHF_CONNECTION_STATE_DISCONNECTED:
        target = PROFILE_STATE_DISCONNECTED;
        break;
    case BTHF_CONNECTION_STATE_CONNECTING:
        target = PROFILE_STATE_CONNECTING;        
        break;
    case BTHF_CONNECTION_STATE_SLC_CONNECTED:
        target = PROFILE_STATE_CONNECTED;        
        break;
    case BTHF_CONNECTION_STATE_DISCONNECTING:
        target = PROFILE_STATE_DISCONNECTING;        
        break;   
    default:
        return;
    }
    
    btmtk_util_convert_array2bdaddr(&converted_addr, &btif_hf_cb.connected_bda);      
    notifyProfileStateChanged(&converted_addr, PROFILE_HFP, target);    
}

static void suspend_av()
{
    int err = 0;
    int state = 0;
    struct timespec ts;

    state = btmtk_a2dp_suspend();
    if (state != BT_A2dp_PENDING)
    {
        ALOGI("%s: btmtk_a2dp_suspend returns %d", __FUNCTION__, state);    
        return;
    }

    ALOGI("%s: btmtk_a2dp_suspend returns BT_A2dp_PENDING, so wait its complete", 
        __FUNCTION__);
    pthread_mutex_lock(&btif_hf_cb.av_suspend_mutex);   
    ts.tv_sec = time(NULL) + WAIT_TIME_SECONDS;
    ts.tv_nsec = 0;
    err = pthread_cond_timedwait(&btif_hf_cb.av_suspend_cond, 
        &btif_hf_cb.av_suspend_mutex, &ts);
    if (err == ETIMEDOUT)
    {
        ALOGE("%s: wait a2dp suspended timeout, WAIT_TIME_SECONDS=%d", 
            __FUNCTION__, WAIT_TIME_SECONDS);
    }   
    pthread_mutex_unlock(&btif_hf_cb.av_suspend_mutex);    
}

static void resume_av()
{
    ALOGI("%s", __FUNCTION__);  
    btmtk_a2dp_resume();  
}

static void clear_av_suspend()
{ 
    if (bt_hf_callbacks == NULL)
    {
        ALOGE("%s: BTHF not initialized", __FUNCTION__);   
        return;
    }
    else
    {
        ALOGI("%s", __FUNCTION__); 
    }
     
    pthread_mutex_lock(&btif_hf_cb.av_suspend_mutex);     
    pthread_cond_signal(&btif_hf_cb.av_suspend_cond);  
    pthread_mutex_unlock(&btif_hf_cb.av_suspend_mutex);   
}

static void on_av_state_change(btif_av_state_t state)
{
    if (bt_hf_callbacks == NULL)
    {
        ALOGE("%s: BTHF not initialized", __FUNCTION__);   
        return;
    }
    else
    {
        ALOGI("%s: state=%s", __FUNCTION__, dump_av_state(state)); 
    }
    
    pthread_mutex_lock(&btif_hf_cb.av_suspend_mutex);     
    if (state != BTIF_AV_STATE_STARTED)
    {
        pthread_cond_signal(&btif_hf_cb.av_suspend_cond);           
    }
    pthread_mutex_unlock(&btif_hf_cb.av_suspend_mutex);     
}

static void ring_timeout(void *pData)
{
    HfgChannelContext *pContext = (HfgChannelContext*)pData;
    HfgCallerIdParms callId;
    
    if (pContext)
    {
        callId.number = pContext->callIdNum;
        callId.type = pContext->callType;
        btmtk_hfg_send_ring(pContext);
        btmtk_hfg_send_caller_id(pContext, &callId);
        timer_Stop(&pContext->ringTimer);
        pContext->ringTimer.load = RING_INTERVAL;
        pContext->ringTimer.func = ring_timeout;
        pContext->ringTimer.name = "ring";
        pContext->ringTimer.arg = pContext;
        timer_Start(&pContext->ringTimer);        
    }
}

/*******************************************************************************
**
** Function         is_connected
**
** Description      Internal function to check if HF is connected
**
** Returns          TRUE if connected
**
*******************************************************************************/
static BT_BOOL is_connected(bt_bdaddr_t *bd_addr)
{
    if (g_pActiveContext && 
        ((btif_hf_cb.state == BTHF_CONNECTION_STATE_CONNECTED) || 
         (btif_hf_cb.state == BTHF_CONNECTION_STATE_SLC_CONNECTED))&&
        ((bd_addr == NULL) || 
         (0 == memcmp(bd_addr->address, btif_hf_cb.connected_bda.address, 6))))
            return TRUE;
        else
            return FALSE;
}

/*******************************************************************************
**
** Function         callstate_to_callsetup
**
** Description      Converts HAL call state to BTA call setup indicator value
**
** Returns          BTA call indicator value
**
*******************************************************************************/
static uint8_t callstate_to_callsetup(bthf_call_state_t call_state)
{
    uint8_t call_setup = 0;
    if (call_state == BTHF_CALL_STATE_INCOMING)
        call_setup = 1;
    if (call_state == BTHF_CALL_STATE_DIALING)
        call_setup = 2;
    if (call_state == BTHF_CALL_STATE_ALERTING)
        call_setup = 3;

    return call_setup;
}

static void clear_phone_state()
{ 
    btif_hf_cb.call_setup_state = BTHF_CALL_STATE_IDLE;
    btif_hf_cb.num_active = btif_hf_cb.num_held = 0;
}

/*******************************************************************************
**
** Function         handle_message
**
** Description      Handle hfp ag message from internal adp
**
** Returns          void
**
*******************************************************************************/
static void handle_message(void *pContext, ilm_struct *pIlm)
{
    char bdstr[18];

    if (bt_hf_callbacks == NULL)
    {
        ALOGE("%s: BTHF not initialized", __FUNCTION__);    
        return;
    }

    if (pIlm == NULL)
    {
        return;
    }

    ALOGI("%s: msg_id=%s", __FUNCTION__, dump_msg(pIlm->msg_id));

    switch (pIlm->msg_id)
    {
        case MSG_ID_BT_HFG_ACTIVATE_CNF:
        case MSG_ID_BT_HFG_DEACTIVATE_CNF:
        {
            break;
        }            

        case MSG_ID_BT_HFG_CONNECT_CNF:
        {
            bt_hfg_connect_cnf_struct *pCnf = (bt_hfg_connect_cnf_struct *)pIlm->ilm_data;
            if (pCnf->result != BT_STATUS_PENDING)
            {
                btmtk_util_convert_array2string(bdstr, &btif_hf_cb.connected_bda);            
                if (btif_hf_cb.state == BTHF_CONNECTION_STATE_CONNECTING)
                {
                    ALOGE("%s: AG open failed. status=%d device=%s", 
                        __FUNCTION__, pCnf->result, bdstr);                 
                    set_connection_state(BTHF_CONNECTION_STATE_DISCONNECTED);
                }
                else
                {
                    ALOGI("%s: AG open failed, but another device connected. status=%d state=%d connected device=%s",
                            __FUNCTION__, pCnf->result, btif_hf_cb.state, bdstr);
                    break;
                }               
                HAL_CBACK(bt_hf_callbacks, connection_state_cb, 
                          btif_hf_cb.state, &btif_hf_cb.connected_bda); 
                if (btif_hf_cb.state == BTHF_CONNECTION_STATE_DISCONNECTED)
                    memset(&btif_hf_cb.connected_bda, 0, sizeof(bt_bdaddr_t));                
            }    
            else
            {
                g_pActiveContext = pCnf->req_context;
            }
            break;
        }

        case MSG_ID_BT_HFG_ACCEPT_CHANNEL_CNF:
        {
            bt_hfg_accept_channel_cnf_struct *pCnf = 
                (bt_hfg_accept_channel_cnf_struct *)pIlm->ilm_data;
            if (pCnf->result != BT_STATUS_PENDING)
            {
                btmtk_util_convert_array2string(bdstr, &btif_hf_cb.connected_bda);             
                if (btif_hf_cb.state == BTHF_CONNECTION_STATE_CONNECTING)
                {
                    ALOGE("%s: AG accept failed. status=%d device=%s", 
                        __FUNCTION__, pCnf->result, bdstr);                  
                    set_connection_state(BTHF_CONNECTION_STATE_DISCONNECTED);                    
                }
                else
                {               
                    ALOGI("%s: AG accept failed, but another device connected. status=%d state=%d connected device=%s",
                            __FUNCTION__, pCnf->result, btif_hf_cb.state, bdstr);
                    break;
                }
                HAL_CBACK(bt_hf_callbacks, connection_state_cb, 
                          btif_hf_cb.state, &btif_hf_cb.connected_bda); 
                if (btif_hf_cb.state == BTHF_CONNECTION_STATE_DISCONNECTED)
                    memset(&btif_hf_cb.connected_bda, 0, sizeof(bt_bdaddr_t));      
                g_pActiveContext = NULL;
            }                
            break;
        }

        case MSG_ID_BT_HFG_DISCONNECT_CNF:
        {
            bt_hfg_disconnect_cnf_struct *pCnf = 
                (bt_hfg_disconnect_cnf_struct *)pIlm->ilm_data; 
            if (pCnf->result != BT_STATUS_PENDING)
            {
                g_pActiveContext = NULL;
            }
            break;
        }

        case MSG_ID_BT_HFG_SCO_CONNECT_CNF:
        {
            bt_hfg_sco_connect_cnf_struct *pCnf = 
                (bt_hfg_sco_connect_cnf_struct *)pIlm->ilm_data;
            btmtk_util_convert_array2string(bdstr, &btif_hf_cb.connected_bda);              
            btif_hf_cb.sco_on_desired = TRUE;
            ALOGI("%s: AG open audio result=%d device=%s", __FUNCTION__, pCnf->result, bdstr);
            if (pCnf->result == BT_STATUS_IN_PROGRESS)
            { 
                btif_hf_cb.sco_in_progress = TRUE;
            }
            else if ((pCnf->result != BT_STATUS_SUCCESS) && (pCnf->result != BT_STATUS_PENDING))
            {            
                HAL_CBACK(bt_hf_callbacks, audio_state_cb, 
                          BTHF_AUDIO_STATE_DISCONNECTED, &btif_hf_cb.connected_bda);
            }
            break;
        }

        case MSG_ID_BT_HFG_SCO_DISCONNECT_CNF:
        {
            bt_hfg_sco_disconnect_cnf_struct *pCnf = 
                (bt_hfg_sco_disconnect_cnf_struct *)pIlm->ilm_data;
            btmtk_util_convert_array2string(bdstr, &btif_hf_cb.connected_bda);               
            btif_hf_cb.sco_on_desired = FALSE; 
            ALOGI("%s: AG close audio result=%d device=%s", __FUNCTION__, pCnf->result, bdstr);                      
            if (pCnf->result == BT_STATUS_IN_PROGRESS)
            {  
                btif_hf_cb.sco_in_progress = TRUE;
            }            
            else if ((pCnf->result != BT_STATUS_SUCCESS) && (pCnf->result != BT_STATUS_PENDING))
            {                        
                HAL_CBACK(bt_hf_callbacks, audio_state_cb, 
                          BTHF_AUDIO_STATE_DISCONNECTED, &btif_hf_cb.connected_bda);
            }
            break;
        }        

        case MSG_ID_BT_HFG_CONNECT_REQ_IND:
        {
            bt_hfg_connect_req_ind_struct *pInd = 
                (bt_hfg_connect_req_ind_struct *)pIlm->ilm_data;
            g_pActiveContext = pInd->user_context;
            btmtk_hfg_accept_connect(g_pActiveContext);
            break;
        }
            
        case MSG_ID_BT_HFG_RFCOMM_CONNECTED_IND:   
        {
            bt_hfg_rfcomm_connected_ind_struct *pInd = 
                (bt_hfg_rfcomm_connected_ind_struct *)pIlm->ilm_data;
            g_pActiveContext = (HfgChannelContext*)pInd->user_context;
            btif_hf_cb.peer_feat = 0;            
            btmtk_util_reverse_array(btif_hf_cb.connected_bda.address, pInd->bt_addr.addr);                       
            set_connection_state(BTHF_CONNECTION_STATE_CONNECTED);               
            clear_phone_state();  
            btmtk_a2dp_app_registerHfgCallback(on_av_state_change);
            HAL_CBACK(bt_hf_callbacks, connection_state_cb, 
                      btif_hf_cb.state, &btif_hf_cb.connected_bda);
            break;
        }

        case MSG_ID_BT_HFG_DISCONNECTED_IND:
        {
            BtStatus status = BT_STATUS_FAILED;
            bt_hfg_disconnected_ind_struct *pInd = 
                (bt_hfg_disconnected_ind_struct *)pIlm->ilm_data;
            btmtk_util_convert_array2string(bdstr, &btif_hf_cb.connected_bda);  
            ALOGI("%s: reason=%s", __FUNCTION__, dump_disc_reason(pInd->reason));
            if ((pInd->user_context == g_pContext[HFP_CONTEXT]) && 
                (pInd->reason == HFG_DISCREASON_SDP))
            {
                ALOGI("%s: Remote doesn't support HFP, try to connect HSP. device=%s", 
                    __FUNCTION__, bdstr);              
                status = btmtk_hfg_create_service_link(g_pContext[HSP_CONTEXT], &pInd->bt_addr);
                if (status != BT_STATUS_SUCCESS)
                {
                    ALOGE("%s: btmtk_hfg_create_service_link failed.", __FUNCTION__);   
                }
                else
                {
                    break;
                }
            }

            if (g_pActiveContext)
            {
                timer_Stop(&g_pActiveContext->ringTimer);  
            }
            set_connection_state(BTHF_CONNECTION_STATE_DISCONNECTED);            
            HAL_CBACK(bt_hf_callbacks, connection_state_cb, 
                      btif_hf_cb.state, &btif_hf_cb.connected_bda);
            memset(&btif_hf_cb.connected_bda, 0, sizeof(bt_bdaddr_t)); 
            memset(btif_hf_cb.name, 0, sizeof(btif_hf_cb.name));
            btif_hf_cb.peer_feat = 0;
            btif_hf_cb.sco_on_desired = FALSE;
            btif_hf_cb.sco_in_progress = FALSE;
            clear_av_suspend();
            clear_phone_state();
            resume_av();
            g_pActiveContext = NULL;
            break;
        }

        case MSG_ID_BT_HFG_CONNECTED_IND:
        {          
            bt_addr_struct bdaddr;
            btmtk_util_convert_array2string(bdstr, &btif_hf_cb.connected_bda);            
            btmtk_util_convert_array2bdaddr(&bdaddr, &btif_hf_cb.connected_bda);             
            btmtk_get_remote_device_name(&bdaddr, btif_hf_cb.name, BTBM_ADP_MAX_NAME_LEN);
            ALOGI("%s: SLC Connected. device=%s(%s)", __FUNCTION__, bdstr, btif_hf_cb.name);               
            set_connection_state(BTHF_CONNECTION_STATE_SLC_CONNECTED);             
            HAL_CBACK(bt_hf_callbacks, connection_state_cb, 
                      btif_hf_cb.state, &btif_hf_cb.connected_bda);
            break;
        }            

        case MSG_ID_BT_HFG_SCO_CONNECTED_IND:
        {
            HAL_CBACK(bt_hf_callbacks, audio_state_cb, 
                      BTHF_AUDIO_STATE_CONNECTED, &btif_hf_cb.connected_bda);
            if (btif_hf_cb.sco_in_progress == TRUE)
            {
                btif_hf_cb.sco_in_progress = FALSE;
                if (btif_hf_cb.sco_on_desired == FALSE)
                {
                    btmtk_hfg_disconnect_audio_link(g_pActiveContext);
                }
            }
            break;
        }            

        case MSG_ID_BT_HFG_SCO_DISCONNECTED_IND:
        {
            HAL_CBACK(bt_hf_callbacks, audio_state_cb, 
                      BTHF_AUDIO_STATE_DISCONNECTED, &btif_hf_cb.connected_bda);
            if (btif_hf_cb.sco_in_progress == TRUE)
            {
                btif_hf_cb.sco_in_progress = FALSE;
                if (btif_hf_cb.sco_on_desired == TRUE)
                {
                    btmtk_hfg_create_audio_link(g_pActiveContext);
                }
            }            
            break;
        }            

        case MSG_ID_BT_HFG_HANDSFREE_FEATURES_IND:
        {
            bt_hfg_handsfree_features_ind_struct *pInd = 
                (bt_hfg_handsfree_features_ind_struct *)pIlm->ilm_data;
            btif_hf_cb.peer_feat = pInd->hfFeatures;
            ALOGI("%s: peer_feat=%lu.", __FUNCTION__, btif_hf_cb.peer_feat);              
            break;
        }

        /* mtkbt auto-responds, silently discard */
        case MSG_ID_BT_HFG_REPORT_SPK_VOLUME_IND:
        {
            bt_hfg_report_speaker_volume_ind_struct *pInd = 
                (bt_hfg_report_speaker_volume_ind_struct *)pIlm->ilm_data;
            ALOGI("%s: spk_vol=%d.", __FUNCTION__, pInd->vol);                
            HAL_CBACK(bt_hf_callbacks, volume_cmd_cb, 
                      BTHF_VOLUME_TYPE_SPK, pInd->vol); 
            break;
        }
        
        /* mtkbt auto-responds, silently discard */        
        case MSG_ID_BT_HFG_REPORT_MIC_VOLUME_IND:
        {
            bt_hfg_report_mic_volume_ind_struct *pInd = 
                (bt_hfg_report_mic_volume_ind_struct *)pIlm->ilm_data;
            ALOGI("%s: mic_vol=%d.", __FUNCTION__, pInd->vol);             
            HAL_CBACK(bt_hf_callbacks, volume_cmd_cb, BTHF_VOLUME_TYPE_MIC, pInd->vol); 
            break;
        }

        /* btif responds ok */
        case MSG_ID_BT_HFG_ANSWER_CALL_IND:
        {
            if (g_pActiveContext)
            {
                timer_Stop(&g_pActiveContext->ringTimer);   
                btmtk_hfg_send_ok(g_pActiveContext);
            }
            HAL_CBACK(bt_hf_callbacks, answer_call_cmd_cb);
            break;
        }            

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_REDIAL_IND:
        {
            HAL_CBACK(bt_hf_callbacks, dial_call_cmd_cb, NULL);   
            break;
        }            
            
        /* Java needs to send OK/ERROR */            
        case MSG_ID_BT_HFG_DIAL_NUMBER_IND:
        {
            bt_hfg_dial_number_ind_struct *pInd = 
                (bt_hfg_dial_number_ind_struct *)pIlm->ilm_data;
            ALOGI("%s: dial number=%s", __FUNCTION__, pInd->number);
            HAL_CBACK(bt_hf_callbacks, dial_call_cmd_cb, pInd->number);             
            break;
        }
        
        /* Java needs to send OK/ERROR */        
        case MSG_ID_BT_HFG_MEMORY_DIAL_IND:
        {
            bt_hfg_memory_dial_ind_struct *pInd = 
                (bt_hfg_memory_dial_ind_struct *)pIlm->ilm_data;
            char number[MAX_PHONE_NUMBER + 1] = {0};
            sprintf(number, ">%d", pInd->index);
            ALOGI("%s: memory dial number=%s", __FUNCTION__, number);            
            HAL_CBACK(bt_hf_callbacks, dial_call_cmd_cb, number);  
            break;
        }

        /* btif responds ok */
        case MSG_ID_BT_HFG_HANGUP_IND:
        {
            if (g_pActiveContext)
            {
                timer_Stop(&g_pActiveContext->ringTimer);  
                btmtk_hfg_send_ok(g_pActiveContext);  
            }          
            HAL_CBACK(bt_hf_callbacks, hangup_call_cmd_cb);
            break;
        }            

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_QUERY_INDICATOR_VALUE_IND:
        {
            HAL_CBACK(bt_hf_callbacks, cind_cmd_cb);
            break;
        }            

        /* mtkbt auto-responds, silently discard */
        case MSG_ID_BT_HFG_GENERATE_DTMF_IND:
        {
            bt_hfg_generate_dtmf_ind_struct *pInd = 
                (bt_hfg_generate_dtmf_ind_struct *)pIlm->ilm_data;
            ALOGI("%s: dtmf=%d.", __FUNCTION__, pInd->dtmf);             
            HAL_CBACK(bt_hf_callbacks, dtmf_cmd_cb, pInd->dtmf);
            break;            
        }

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_ENABLE_VOICE_RECOGNITION_IND:
        {
            bt_hfg_enable_voice_recognition_ind_struct *pInd = 
                (bt_hfg_enable_voice_recognition_ind_struct *)pIlm->ilm_data;
            ALOGI("%s: vr_enable=%d.", __FUNCTION__, pInd->enable);             
            HAL_CBACK(bt_hf_callbacks, vr_cmd_cb,
                (pInd->enable == KAL_TRUE) ? BTHF_VR_STATE_STARTED : BTHF_VR_STATE_STOPPED);
            break;
        }

        /* mtkbt auto-responds, silently discard */
        case MSG_ID_BT_HFG_DISABLE_NREC_IND:
        {
            HAL_CBACK(bt_hf_callbacks, nrec_cmd_cb, BTHF_NREC_STOP);
            break;
        }

        /* btif responds ok */
        case MSG_ID_BT_HFG_KEYPAD_CONTROL_IND:
        {
            if (g_pActiveContext)
            {
                timer_Stop(&g_pActiveContext->ringTimer);   
                btmtk_hfg_send_ok(g_pActiveContext);
            }            
            HAL_CBACK(bt_hf_callbacks, key_pressed_cmd_cb);
            break;
        }

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_CALL_HOLD_IND:
        {
            bt_hfg_call_hold_ind_struct *pInd = 
                (bt_hfg_call_hold_ind_struct *)pIlm->ilm_data;  
            ALOGI("%s: action=%d, index=%d.", 
                __FUNCTION__, pInd->hold.action, pInd->hold.index);    
            if (pInd->hold.index == 0)
            {
                HAL_CBACK(bt_hf_callbacks, chld_cmd_cb, pInd->hold.action);
            }
            else
            {
                HAL_CBACK(bt_hf_callbacks, chld_cmd_cb, 
                    pInd->hold.action * 10 + pInd->hold.index);
            }
            break;
        }

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_LIST_CURRENT_CALLS_IND:
        {
            HAL_CBACK(bt_hf_callbacks, clcc_cmd_cb);
            break;
        }

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_QUERY_NETWORK_OPERATOR_IND:
        {
            HAL_CBACK(bt_hf_callbacks, cops_cmd_cb);
            break;
        }

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_AT_COMMAND_DATA_IND:
        {
            bt_hfg_at_command_data_ind_struct *pInd = 
                (bt_hfg_at_command_data_ind_struct *)pIlm->ilm_data;
            ALOGI("%s: atcmd=%s.", __FUNCTION__, pInd->atcmd + 2);              
            HAL_CBACK(bt_hf_callbacks, unknown_at_cmd_cb, pInd->atcmd + 2);
            break;
        }

        /* Java needs to send OK/ERROR */
        case MSG_ID_BT_HFG_QUERY_SUBSCRIBER_NUMBER_IND:
        {
            HAL_CBACK(bt_hf_callbacks, cnum_cmd_cb);
            break;
        }

        default:
        {
            ALOGE("%s: Unhandled msg: %lu", __FUNCTION__, pIlm->msg_id);
            break;
        }            
    }
}

/*****************************************************************************
**   Section name (Group of functions)
*****************************************************************************/

/*****************************************************************************
**
**   btif hf api functions (no context switch)
**
*****************************************************************************/

/*******************************************************************************
**
** Function         btif_hf_init
**
** Description     initializes the hf interface
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t init( bthf_callbacks_t* callbacks )
{
    struct sockaddr_un hfgname; 
    struct sockaddr_un btname;         
    socklen_t hfgnamelen = sizeof(short);
    socklen_t btnamelen;     
    int i = 0;

    bt_hf_callbacks = callbacks;    

    memset(&hfgname, 0, sizeof(struct sockaddr_un));
    hfgname.sun_family = AF_UNIX;

    memset(&btname, 0, sizeof(struct sockaddr_un));    
    btname.sun_family = AF_UNIX;
    strcpy(btname.sun_path, BT_SOCK_NAME_INT_ADP);
    btnamelen = (offsetof(struct sockaddr_un, sun_path) + strlen(btname.sun_path) + 1);      
    
    g_hfgsock = socket(PF_LOCAL, SOCK_DGRAM, 0);
    ALOGI("%s g_hfgsock=%d", __FUNCTION__, g_hfgsock);
    if (g_hfgsock < 0)
    {
        ALOGE("%s create hfg api socket failed: %s, errno=%d", __FUNCTION__, strerror(errno), errno);
        goto exit;
    }
    if (bind(g_hfgsock, (struct sockaddr *)&hfgname, hfgnamelen) < 0)
    {
        ALOGE("%s bind hfg api socket failed: %s, errno=%d", __FUNCTION__, strerror(errno), errno);
        goto exit;
    }
    else
    {
        hfgnamelen = sizeof(hfgname.sun_path);
        hfgname.sun_path[0] = '\0';
        if (getsockname(g_hfgsock, (struct sockaddr*)&hfgname, &hfgnamelen) < 0)
        {
            ALOGE("%s getsockname failed: %s, errno=%d", __FUNCTION__, strerror(errno), errno);
        }
        else
        {
            ALOGI("%s auto bind hfg api: len=%d, addr=%s", __FUNCTION__, hfgnamelen, &hfgname.sun_path[1]);
        }
    }
    if (connect(g_hfgsock, (const struct sockaddr*)&btname, btnamelen) < 0)
    {
        ALOGE("%s connect to /data/btserv failed : %s, errno=%d", __FUNCTION__, strerror(errno), errno);
        goto exit;
    }

    for (i = 0; i < NUM_OF_CONTEXT; i++)
    {
        g_pContext[i] = (HfgChannelContext *)calloc(1, sizeof(HfgChannelContext));
        if (!g_pContext[i])
        {
            ALOGE("%s out of memory for context idx %d", __FUNCTION__, i);
            goto exit;
        }
        memset((void*)g_pContext[i], 0, sizeof(g_pContext[0]));             
    }     
    g_pActiveContext = NULL;      
    memset(&btif_hf_cb, 0, sizeof(btif_hf_cb_t));
    clear_phone_state(); 
    pthread_cond_init(&btif_hf_cb.disable_cond, NULL);
    pthread_cond_init(&btif_hf_cb.av_suspend_cond, NULL);    
    pthread_mutex_init(&btif_hf_cb.disable_mutex, NULL);    
    pthread_mutex_init(&btif_hf_cb.av_suspend_mutex, NULL);
    btif_hf_cb.disabled = TRUE;
    btif_hf_cb.sco_on_desired = FALSE;
    btif_hf_cb.sco_in_progress = FALSE;
    btmtk_profile_register(PROFILE_HFP, handle_message, NULL);
    btif_enable_service(BTMTK_HFP_SERVICE_ID);      
    return BT_STATUS_SUCCESS;

exit:
    if (g_hfgsock)
    {
        close(g_hfgsock);
        g_hfgsock = 0;
    }   
    for (i = 0; i < NUM_OF_CONTEXT; i++)
    {
        if (g_pContext[i])
        {
            free(g_pContext[i]);
            g_pContext[i] = NULL;
        }
    }
    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         connect_slc
**
** Description     connect to headset
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t connect_slc( bt_bdaddr_t *bd_addr )
{
    bt_addr_struct converted_addr;
    
    CHECK_BTHF_INIT();
    if (!is_connected(bd_addr))
    {
    	btif_hf_cb.state = BTHF_CONNECTION_STATE_CONNECTING;
        if ((g_pContext[HFP_CONTEXT] && g_pContext[HFP_CONTEXT]->hfgContext) ||
            (g_pContext[HSP_CONTEXT] && g_pContext[HSP_CONTEXT]->hfgContext))
        {
            btmtk_util_convert_array2bdaddr(&converted_addr, bd_addr);        
            connect_profile(&converted_addr, PROFILE_HFP);
        }
        else
        {
            return BT_STATUS_UNSUPPORTED;
        }
        return BT_STATUS_SUCCESS;          
    }
    return BT_STATUS_BUSY;    
}

/*******************************************************************************
**
** Function         disconnect_slc
**
** Description      disconnect from headset
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t disconnect_slc( bt_bdaddr_t *bd_addr )
{ 
    bt_addr_struct converted_addr;

    CHECK_BTHF_INIT();
    if (is_connected(bd_addr))
    {
        btmtk_util_convert_array2bdaddr(&converted_addr, bd_addr);     
        disconnect_profile(&converted_addr, PROFILE_HFP);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         connect_audio
**
** Description     create an audio connection
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t connect_audio( bt_bdaddr_t *bd_addr )
{
    CHECK_BTHF_INIT();

    if (is_connected(bd_addr))
    {
        HAL_CBACK(bt_hf_callbacks, audio_state_cb, BTHF_AUDIO_STATE_CONNECTING,
                  &btif_hf_cb.connected_bda);       
        btmtk_hfg_create_audio_link(g_pActiveContext);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         disconnect_audio
**
** Description      close the audio connection
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t disconnect_audio( bt_bdaddr_t *bd_addr )
{   
    CHECK_BTHF_INIT();

    if (is_connected(bd_addr))
    {
        btmtk_hfg_disconnect_audio_link(g_pActiveContext);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         start_voice_recognition
**
** Description      start voice recognition
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t start_voice_recognition()
{
    CHECK_BTHF_INIT();
    if (is_connected(NULL))
    {
        if (btif_hf_cb.peer_feat & HFG_HANDSFREE_FEATURE_VOICE_RECOGNITION)
        {
            btmtk_hfg_enable_vr(g_pActiveContext, KAL_TRUE);
            return BT_STATUS_SUCCESS;
        }
        else
        {
            return BT_STATUS_UNSUPPORTED;
        }
    }

    return BT_STATUS_NOT_READY;
}

/*******************************************************************************
**
** Function         stop_voice_recognition
**
** Description      stop voice recognition
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t stop_voice_recognition()
{
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        if (btif_hf_cb.peer_feat & HFG_HANDSFREE_FEATURE_VOICE_RECOGNITION)
        {
            btmtk_hfg_enable_vr(g_pActiveContext, KAL_FALSE);
            return BT_STATUS_SUCCESS;
        }
        else
        {
            return BT_STATUS_UNSUPPORTED;
        }
    }

    return BT_STATUS_NOT_READY;
}

/*******************************************************************************
**
** Function         volume_control
**
** Description      volume control
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t volume_control(bthf_volume_type_t type, int volume)
{
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        if (type == BTHF_VOLUME_TYPE_SPK)
        {
            btmtk_hfg_send_speaker_volume(g_pActiveContext, volume);
        }
        else
        {
            btmtk_hfg_send_mic_volume(g_pActiveContext, volume);
        }
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         device_status_notification
**
** Description      Combined device status change notification
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t device_status_notification(bthf_network_state_t ntk_state,
                          bthf_service_type_t svc_type, int signal, int batt_chg)
{
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        ALOGI("device_status_notification: ntk_state=%s svc_type=%s signal=%d batt_chg=%d", 
              dump_network_state(ntk_state), dump_service_type(svc_type), 
              signal, batt_chg);   
    
        /* send all indicators to mtkbt.
        ** mtkbt will make sure no duplicates are sent out
        */
        btmtk_hfg_set_indicator_value(g_pActiveContext, HFG_IND_SERVICE,
            (ntk_state == BTHF_NETWORK_STATE_AVAILABLE) ? 1 : 0);
        btmtk_hfg_set_indicator_value(g_pActiveContext, HFG_IND_ROAMING,
            (svc_type == BTHF_SERVICE_TYPE_HOME) ? 0 : 1);
        /* Signal strength: if service is 0, make sure signal is also 0 */        
        if (ntk_state == BTHF_NETWORK_STATE_AVAILABLE)
        {
            btmtk_hfg_set_indicator_value(g_pActiveContext, HFG_IND_SIGNAL, signal);
        }
        else
        {
            btmtk_hfg_set_indicator_value(g_pActiveContext, HFG_IND_SIGNAL, 0);
        }
        btmtk_hfg_set_indicator_value(g_pActiveContext, HFG_IND_BATTERY, batt_chg);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         cops_response
**
** Description      Response for COPS command
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t cops_response(const char *cops)
{
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        HfgNetworkOperator oper;

        memset(&oper, 0, sizeof(oper));
        oper.oper = cops;
        btmtk_hfg_send_network_operator_rsp(g_pActiveContext, &oper);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         cind_response
**
** Description      Response for CIND command
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t cind_response(int svc, int num_active, int num_held,
                                     bthf_call_state_t call_setup_state,
                                     int signal, int roam, int batt_chg)
{
    CHECK_BTHF_INIT();
 
    if (is_connected(NULL))
    {
        int idx = 0;
        HfgReadIndicatorParms indicators;

        ALOGI("cind_response: svc=%d num_active=%d num_held=%d"\
              " call_setup=%s signal=%d roam=%d batt_chg=%d", 
              svc, num_active, num_held, dump_call_state(call_setup_state), 
              signal, roam, batt_chg);          

        memset(&indicators, 0, sizeof(indicators));
        /* per the errata 2043, call=1 implies atleast one call is in progress (active/held)
        ** https://www.bluetooth.org/errata/errata_view.cfm?errata_id=2043
        **/        
        indicators.ind[idx++].value = svc;                                                 /* network service */
        indicators.ind[idx++].value = (num_active + num_held) ? 1 : 0;                     /* Call state */
        indicators.ind[idx++].value = callstate_to_callsetup(call_setup_state);            /* Callsetup state */
        indicators.ind[idx++].value = ((num_held == 0) ? 0 : ((num_active == 0) ? 2 : 1)); /* Call held */    
        indicators.ind[idx++].value = batt_chg;                                            /* Battery level */
        /* Signal strength: if service is 0, make sure signal is also 0 */
        if (svc)
        {
            indicators.ind[idx++].value = signal;                                              
        }
        else
        {
            indicators.ind[idx++].value = 0; 
        }
        indicators.ind[idx++].value = roam; /* Roaming indicator */
        indicators.num = idx;    
        btmtk_hfg_read_indicator(g_pActiveContext, &indicators);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         formatted_at_response
**
** Description      Pre-formatted AT response, typically in response to unknown AT cmd
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t formatted_at_response(const char *rsp)
{
    char atStr[MAX_AT_STRING_LEN] = {0};
    
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        sprintf(atStr, "\r\n%s\r\n", rsp);
        btmtk_hfg_send_data(g_pActiveContext, atStr, strlen(atStr));
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         at_response
**
** Description      ok/error response
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t at_response(bthf_at_response_t response_code, int error_code)
{
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        if (response_code == BTHF_AT_RESPONSE_OK)
        {
            btmtk_hfg_send_ok(g_pActiveContext);
        }
        else
        {
            btmtk_hfg_send_error(g_pActiveContext, error_code);
        }
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         clcc_response
**
** Description      response for CLCC command
**                  Can be iteratively called for each call index. Call index
**                  of 0 will be treated as NULL termination (Completes response)
**
** Returns          bt_status_t
**
*******************************************************************************/
static bt_status_t clcc_response(int index, bthf_call_direction_t dir,
                                bthf_call_state_t state, bthf_call_mode_t mode,
                                bthf_call_mpty_type_t mpty, const char *number,
                                bthf_call_addrtype_t type)
{
    CHECK_BTHF_INIT();

    if (is_connected(NULL))
    {
        /* Format the response */
        if (index == 0)
        {
            btmtk_hfg_send_ok(g_pActiveContext);
        }
        else
        {
            HfgCallListParms callList;
            char formatedNum[MAX_PHONE_NUMBER + 2] = {0};
            int len = 0;
            int pos = 0;
            
            ALOGI("clcc_response: [%d] dir %d state %d mode %d number = %s type = %d",
                          index, dir, state, mode, number, type);

            memset(&callList, 0, sizeof(callList));            
            if (number)
            {
                if ((type == BTHF_CALL_ADDRTYPE_INTERNATIONAL) && (*number != '+'))
                {
                    formatedNum[pos++] = '+';
                }
                len = (strlen(number) > MAX_PHONE_NUMBER) ? MAX_PHONE_NUMBER : strlen(number);
                memcpy(&formatedNum[pos], number, len);
                pos += len;
                formatedNum[pos] = '\0';
            }

            callList.index = index;
            callList.dir = dir;
            callList.state = state;
            callList.mode = mode;
            callList.multiParty = mpty;
            callList.number = formatedNum;
            callList.type = type;      
            btmtk_hfg_send_call_list_rsp(g_pActiveContext, &callList, KAL_FALSE);
        }

        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         phone_state_change
**
** Description      notify of a call state change
**                  number & type: valid only for incoming & waiting call
**
** Returns          bt_status_t
**
*******************************************************************************/

static bt_status_t phone_state_change(int num_active, int num_held, bthf_call_state_t call_setup_state,
                                            const char *number, bthf_call_addrtype_t type)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    BT_BOOL activeCallUpdated = FALSE;

    CHECK_BTHF_SLC_CONNECTED();

    ALOGI("phone_state_change: num_active=%d [prev: %d]  num_held=%d[prev: %d]"\
               " call_setup=%s [prev: %s]", num_active, btif_hf_cb.num_active,
               num_held, btif_hf_cb.num_held,
               dump_call_state(call_setup_state), dump_call_state(btif_hf_cb.call_setup_state));

    /* if all indicators are 0, send end call and return */
    if (num_active == 0 && num_held == 0 && call_setup_state == BTHF_CALL_STATE_IDLE)
    {
        ALOGI("%s: Phone on hook", __FUNCTION__);

        btmtk_hfg_disconnect_audio_link(g_pActiveContext);      

        /* if held call was present, reset that as well */
        if (btif_hf_cb.num_held)
        {
            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                HFG_IND_CALL_HELD, HFG_HOLD_NO_HLD);
        }
        
        if (btif_hf_cb.num_active + btif_hf_cb.num_held)
        {
            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                HFG_IND_CALL, 0); 
        }

        if (btif_hf_cb.call_setup_state != BTHF_CALL_STATE_IDLE)
        {
            if (btif_hf_cb.call_setup_state == BTHF_CALL_STATE_INCOMING)
            {
                timer_Stop(&g_pActiveContext->ringTimer); 
            }         
            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE); 
        }

        resume_av();           
        goto update_call_states;
    }

    /* active state can change when:
    ** 1. an outgoing/incoming call was answered
    ** 2. an held was resumed
    ** 3. without callsetup notifications, call became active
    ** (3) can happen if call is active and a headset connects to us
    **
    ** In the case of (3), we will have to notify the stack of an active
    ** call, instead of sending an indicator update. This will also
    ** force the SCO to be setup. Handle this special case here prior to
    ** call setup handling
    */
    if ( (num_active == 1) && (btif_hf_cb.num_active == 0) && (btif_hf_cb.num_held == 0) &&
         (btif_hf_cb.call_setup_state == BTHF_CALL_STATE_IDLE) )
    {
        ALOGI("%s: Active call notification received without call setup update",
                   __FUNCTION__);

        suspend_av();
        CHECK_BTHF_SLC_CONNECTED();
        btmtk_hfg_set_indicator_value(g_pActiveContext, 
            HFG_IND_CALL, 1);     
        btmtk_hfg_create_audio_link(g_pActiveContext);
        activeCallUpdated = TRUE;
    }

    /* Ringing call changed? */
    if (call_setup_state != btif_hf_cb.call_setup_state)
    {
        ALOGI("%s: Call setup states changed. old: %s new: %s",
            __FUNCTION__, dump_call_state(btif_hf_cb.call_setup_state),
            dump_call_state(call_setup_state));

        switch (call_setup_state)
        {
            case BTHF_CALL_STATE_IDLE:
            {
                switch (btif_hf_cb.call_setup_state)
                {
                    case BTHF_CALL_STATE_INCOMING:
                        timer_Stop(&g_pActiveContext->ringTimer);                        
                        if (num_active > btif_hf_cb.num_active)
                        {    
                            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                                HFG_IND_CALL, 1);
                            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                                HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);        
                            btmtk_hfg_create_audio_link(g_pActiveContext);
                        }
                        else if (num_held > btif_hf_cb.num_held)
                        {
                            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                                HFG_IND_CALL, 1);
                            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                                HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE); 
                        }
                        else
                        {
                            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                                HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);
                        }
                        break;
                    case BTHF_CALL_STATE_DIALING:
                    case BTHF_CALL_STATE_ALERTING:
                        if (num_active > btif_hf_cb.num_active)
                        {
                            btmtk_hfg_set_indicator_value(g_pActiveContext, 
                                HFG_IND_CALL, 1);
                        }
                        btmtk_hfg_set_indicator_value(g_pActiveContext, 
                            HFG_IND_CALL_SETUP, HFG_CALL_SETUP_NONE);
                        break;
                    default:
                        ALOGE("%s: Incorrect Call setup state transition", __FUNCTION__);
                        status = BT_STATUS_PARM_INVALID;
                        break;
                }
            } break;

            case BTHF_CALL_STATE_INCOMING:
            {
                HfgCallWaitParms callWait;
                char formatedNum[MAX_PHONE_NUMBER + 2] = {0};
                int len = 0;
                int pos = 0;
                               
                if (number)
                {
                    if ((type == BTHF_CALL_ADDRTYPE_INTERNATIONAL) && (*number != '+'))
                    {
                        formatedNum[pos++] = '+';
                    }
                    len = (strlen(number) > MAX_PHONE_NUMBER) ? MAX_PHONE_NUMBER : strlen(number);
                    memcpy(&formatedNum[pos], number, len);
                    pos += len;
                    formatedNum[pos] = '\0';
                }
                if (num_active || num_held)
                {
                    memset(&callWait, 0, sizeof(callWait));
                    callWait.number = formatedNum;
                    callWait.type = type;
                    btmtk_hfg_call_waiting(g_pActiveContext, &callWait);
                    if ((btif_hf_cb.peer_feat & HFG_HANDSFREE_FEATURE_CALL_WAITING) ||
                        (0 == strcmp(btif_hf_cb.name, IOT_DEV_BLUECHIP)))
                    {
                        btmtk_hfg_set_indicator_value(g_pActiveContext, 
                            HFG_IND_CALL_SETUP, HFG_CALL_SETUP_IN); 
                    }                   
                }
                else
                {
                    suspend_av();       
                    CHECK_BTHF_SLC_CONNECTED();                    
                    btmtk_hfg_set_indicator_value(g_pActiveContext, 
                        HFG_IND_CALL_SETUP, HFG_CALL_SETUP_IN);                   
                    memcpy(g_pActiveContext->callIdNum, formatedNum, sizeof(formatedNum));
                    g_pActiveContext->callType = type;       
                    ring_timeout(g_pActiveContext);
                }              
            } break;
            
            case BTHF_CALL_STATE_DIALING:
                if (num_active || num_held)
                {
                    if (btif_hf_cb.peer_feat & HFG_HANDSFREE_FEATURE_CALL_WAITING)
                    {
                        btmtk_hfg_set_indicator_value(g_pActiveContext, 
                            HFG_IND_CALL_SETUP, HFG_CALL_SETUP_OUT);
                    }
                }
                else
                {
                    suspend_av();  
                    CHECK_BTHF_SLC_CONNECTED(); 
                    btmtk_hfg_set_indicator_value(g_pActiveContext, 
                        HFG_IND_CALL_SETUP, HFG_CALL_SETUP_OUT);   
                    btmtk_hfg_create_audio_link(g_pActiveContext);                       
                }                
                break;
            case BTHF_CALL_STATE_ALERTING:
                if (num_active || num_held)
                {
                    if (btif_hf_cb.peer_feat & HFG_HANDSFREE_FEATURE_CALL_WAITING)
                    {
                        btmtk_hfg_set_indicator_value(g_pActiveContext, 
                            HFG_IND_CALL_SETUP, HFG_CALL_SETUP_ALERT); 
                    }
                }
                else
                {
                    if (btif_hf_cb.call_setup_state == BTHF_CALL_STATE_IDLE)
                    {
                        suspend_av();  
                        CHECK_BTHF_SLC_CONNECTED();                     
                        btmtk_hfg_create_audio_link(g_pActiveContext);
                    }  
                    btmtk_hfg_set_indicator_value(g_pActiveContext, 
                        HFG_IND_CALL_SETUP, HFG_CALL_SETUP_ALERT);                    
                }                                  
                break;
            default:
                ALOGE("%s: Incorrect new ringing call state", __FUNCTION__);
                status = BT_STATUS_PARM_INVALID;
                break;
        }

        /* if call setup is idle, we have already updated call indicator, jump out */
        if (call_setup_state == BTHF_CALL_STATE_IDLE)
        {
            /* check & update callheld */
            if ((num_held > 0) && (num_active > 0) &&
                ((num_active != btif_hf_cb.num_active) || (num_held != btif_hf_cb.num_held)))
            {
                btmtk_hfg_set_indicator_value(g_pActiveContext, 
                    HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
            }
            goto update_call_states;
        }
    }

    /* per the errata 2043, call=1 implies atleast one call is in progress (active/held)
    ** https://www.bluetooth.org/errata/errata_view.cfm?errata_id=2043
    ** Handle call indicator change
    **/
    if (!activeCallUpdated && ((num_active + num_held) != (btif_hf_cb.num_active + btif_hf_cb.num_held)) )
    {
        ALOGI("%s: Active/held call states changed. old: %d new: %d", __FUNCTION__, 
            btif_hf_cb.num_active + btif_hf_cb.num_held, num_active + num_held);
        btmtk_hfg_set_indicator_value(g_pActiveContext, 
            HFG_IND_CALL, ((num_active + num_held) > 0) ? 1 : 0);
        if ((num_active == 0) && (num_held == 0) && 
            (call_setup_state != BTHF_CALL_STATE_DIALING) &&
            (call_setup_state != BTHF_CALL_STATE_ALERTING))
        {
            btmtk_hfg_disconnect_audio_link(g_pActiveContext);
        }
    }

    /* Held Changed? */
    if ((num_active != btif_hf_cb.num_active) || (num_held != btif_hf_cb.num_held))
    {
        if (num_active != btif_hf_cb.num_active)
        {
            ALOGI("%s: Active call states changed. old: %d new: %d", __FUNCTION__, 
                btif_hf_cb.num_active, num_active);
        }
        if (num_held != btif_hf_cb.num_held)
        {
            ALOGI("%s: Held call states changed. old: %d new: %d", __FUNCTION__, 
                btif_hf_cb.num_held, num_held);
        }    
        btmtk_hfg_set_indicator_value(g_pActiveContext,
            HFG_IND_CALL_HELD, 
            ((num_held == 0) ? HFG_HOLD_NO_HLD : 
            ((num_active == 0) ? HFG_HOLD_HLD_NO_ACT : HFG_HOLD_HLD_ACT)));
    }

    /* Calls Swapped? */
    if ( (call_setup_state == btif_hf_cb.call_setup_state) &&
         (num_active && num_held) &&
         (num_active == btif_hf_cb.num_active) &&
         (num_held == btif_hf_cb.num_held) )
    {
        ALOGI("%s: Calls swapped", __FUNCTION__);
        btmtk_hfg_set_indicator_value(g_pActiveContext,
            HFG_IND_CALL_HELD, HFG_HOLD_HLD_ACT);
    }

update_call_states:
    btif_hf_cb.num_active = num_active;
    btif_hf_cb.num_held = num_held;
    btif_hf_cb.call_setup_state = call_setup_state;

    return status;
}

/*******************************************************************************
**
** Function         cleanup
**
** Description      Closes the HF interface
**
** Returns          bt_status_t
**
*******************************************************************************/
static void  cleanup( void )
{
    int i = 0;
    int err = 0;    
    struct timespec ts;    

    ALOGI("%s", __FUNCTION__);
    
    if (!btif_hf_cb.disabled)
    {
        pthread_mutex_lock(&btif_hf_cb.disable_mutex);        
        btmtk_profile_unregister(PROFILE_HFP);   
        btif_disable_service(BTMTK_HFP_SERVICE_ID);    
        ts.tv_sec = time(NULL) + WAIT_TIME_SECONDS;
        ts.tv_nsec = 0;           
        err = pthread_cond_timedwait(&btif_hf_cb.disable_cond, 
            &btif_hf_cb.disable_mutex, &ts);
        if (err == ETIMEDOUT)
        {
            ALOGE("%s: wait deregister timeout, WAIT_TIME_SECONDS=%d", 
                __FUNCTION__, WAIT_TIME_SECONDS);
        }              
        pthread_mutex_unlock(&btif_hf_cb.disable_mutex);          
    }
            
    if (bt_hf_callbacks)
    {
        if (g_hfgsock)    
        {        
            close(g_hfgsock);        
            g_hfgsock = 0;    
        }       
        for (; i < NUM_OF_CONTEXT; i++)    
        {        
            if (g_pContext[i])        
            {            
                free(g_pContext[i]);            
                g_pContext[i] = NULL;        
            }    
        }
        bt_hf_callbacks = NULL;
    }

    pthread_cond_destroy(&btif_hf_cb.disable_cond);
    pthread_cond_destroy(&btif_hf_cb.av_suspend_cond);    
    pthread_mutex_destroy(&btif_hf_cb.disable_mutex);      
    pthread_mutex_destroy(&btif_hf_cb.av_suspend_mutex);    
}

static const bthf_interface_t bthfInterface = {
    sizeof(bt_interface_t),
    init,
    connect_slc,
    disconnect_slc,
    connect_audio,
    disconnect_audio,
    start_voice_recognition,
    stop_voice_recognition,
    volume_control,
    device_status_notification,
    cops_response,
    cind_response,
    formatted_at_response,
    at_response,
    clcc_response,
    phone_state_change,
    cleanup,
};

/*******************************************************************************
**
** Function         btif_hf_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_hf_execute_service(BT_BOOL b_enable)
{
    int i = 0;

    ALOGI("%s: %s", __FUNCTION__, b_enable ? "enable" : "disable");  

    for (; i < NUM_OF_CONTEXT; i++)
    {      
        if (g_pContext[i])
        {
            if (b_enable)
            {
                btmtk_hfg_register(g_pContext[i], NULL, g_hfgsock, i == HSP_CONTEXT); 
            }
            else
            {
                btmtk_hfg_deregister(g_pContext[i]);
            }
        }
        else
        {
            ALOGE("%s: g_pContext[%d] is NULL", __FUNCTION__, i);
        }
    }      

    if (b_enable)
    {
        btif_hf_cb.disabled = FALSE;
        pthread_mutex_lock(&btif_hf_cb.disable_mutex);   
        pthread_cond_signal(&btif_hf_cb.disable_cond); 
        pthread_mutex_unlock(&btif_hf_cb.disable_mutex);  
    }
    else
    {
        btif_hf_cb.disabled = TRUE;
    }
    
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_hf_get_interface
**
** Description      Get the hf callback interface
**
** Returns          bthf_interface_t
**
*******************************************************************************/
const bthf_interface_t *btif_hf_get_interface()
{
    ALOGI("%s", __FUNCTION__);
    return &bthfInterface;
}

int btif_hf_connect(bt_addr_struct *bd_addr)
{
    bt_bdaddr_t addr_array;
    MTK_BD_ADDR addr_reversed_array;
    
    CHECK_BTHF_INIT();
    btmtk_util_convert_bdaddr2array(&addr_array, bd_addr);    
    if (!is_connected(&addr_array))
    {
        memcpy(&btif_hf_cb.connected_bda, &addr_array, sizeof(bt_bdaddr_t));
        btmtk_util_reverse_array(addr_reversed_array.addr, addr_array.address);          
        if (g_pContext[HFP_CONTEXT] && g_pContext[HFP_CONTEXT]->hfgContext)
        {
            btmtk_hfg_create_service_link(g_pContext[HFP_CONTEXT], &addr_reversed_array);
        }
        else if (g_pContext[HSP_CONTEXT] && g_pContext[HSP_CONTEXT]->hfgContext)
        {
            btmtk_hfg_create_service_link(g_pContext[HSP_CONTEXT], &addr_reversed_array);
        }
        else
        {
            return BT_STATUS_FAILED;
        }
        return BT_STATUS_SUCCESS;          
    }
    return BT_STATUS_BUSY;    
}

int btif_hf_disconnect(bt_addr_struct *bd_addr)
{
    bt_bdaddr_t addr_array;

    CHECK_BTHF_INIT();
    btmtk_util_convert_bdaddr2array(&addr_array, bd_addr);        
    if (is_connected(&addr_array))
    {
        btmtk_hfg_disconnect_service_link(g_pActiveContext);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAILED;
}
