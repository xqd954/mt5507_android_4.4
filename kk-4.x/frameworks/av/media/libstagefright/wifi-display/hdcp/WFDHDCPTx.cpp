
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/foundation/hexdump.h>

#include <arpa/inet.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <cutils/properties.h>

#include "WFDHDCPTx.h"
#include "HDCP2xDef.h"


extern "C" {
#include <mtcommon.h>
#include <mtdrm.h>
#include <mtperipheral.h>

}


/*-----------------------------
    Macro Defines
------------------------------*/

#define WFD_HDCP2X_PORT 4990
#define WFD_HDCP2X_RX_IP "192.168.100.11"

namespace android {



#define DEFAULT_TX_RETRY_SEND_COUNT     5

#define DEFAULT_TX_LC_RETRY_SEND_COUNT     (1024)
#define DEFAULT_TX_AKE_RETRY_SEND_COUNT     5

//static int mLC_Try_Count = DEFAULT_TX_RETRY_SEND_COUNT;
#define __KAL_ATTRIB_PACKED__	__attribute__((__packed__))

#define hdcp2x_cmmGetRandom()   rand()

static const char *hdcp2x_tx_str[] =
{
    /*[HDCP2X_TX_STATE_DETACHED] = */            "Detached",
    /*[HDCP2X_TX_STATE_WAIT_ATTACHED] = */       "Wait_Attached",
    /*[HDCP2X_TX_STATE_AKE_INIT] = */            "Ake_Init",
    /*[HDCP2X_TX_STATE_WAIT_CERT]=*/             "Wait_Cert",
    /*[HDCP2X_TX_STATE_WAIT_RRX] =*/             "Wait_Rrx",
    /*[HDCP2X_TX_STATE_WAIT_H_PRIME] =*/         "Wait_H_Prime",
    /*[HDCP2X_TX_STATE_WAIT_PAIRING_INFO] =*/    "Wait_Pairing_Info",
    /*[HDCP2X_TX_STATE_WAIT_L_PRIME] =*/         "Wait_L_Prime",
    /*[HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST] =*/ "Wait_ReceiverID_List",
    /*[HDCP2X_TX_STATE_WAIT_STREAM_READY] =*/   "Wait_Stream_Ready",    
    /*[HDCP2X_TX_STATE_WAIT_RECEIVER_INFO] = */  "Wait_Receiver_Info",
    /*[HDCP2X_TX_STATE_WAIT_RTT_READY] = */      "Wait_RTT_Ready",
    /*[HDCP2X_TX_STATE_AUTHENTICATED] =*/        "Authenticated",
    /*[HDCP2X_TX_STATE_AUTHENTICATE_FAIL] = */   "Authenticate_Fail"
};

static const char *hdcp2x_event_str[] =
{
    /*[HDCP2X_TX_TIMEOUT_WAIT_RX_CONNECTION] = */ "Wait_RX_Connection",
    /*[HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_INFO] = */ "Wait_Receiver_Info",
    /*[HDCP2X_TX_TIMEOUT_WAIT_H_PRIME] = */       "Wait_H_Prime",
    /*[HDCP2X_TX_TIMEOUT_WAIT_PAIRING_INFO] = */  "Wait_Pairing_Info",
    /*[HDCP2X_TX_TIMEOUT_WAIT_L_PRIME]=*/         "Wait_L_Prime",
    /*[HDCP2X_TX_TIMEOUT_WAIT_RTT_READY]=*/       "Wait_RTT_Ready",
    /*[HDCP2X_TX_TIMEOUT_WAIT_SHUTDOWN]=*/        "Wait_Shutdown",
    /*[HDCP2X_TX_TIMEOUT_WAIT_AUTH_FAIL]=*/       "Wait_AuthFail",
    /*[HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID]=*/     "Wait_Receiver_Id",
    /*[HDCP2X_TX_TIMEOUT_WAIT_STREAM_READY]=*/   "Wait_Stream_Ready"    
};



unsigned char dcp_key_pub[384 + 1] = {
	
		//PublicModulus n:
		0xB0, 0xE9, 0xAA, 0x45, 0xF1, 0x29, 0xBA, 0x0A, 0x1C, 0xBE, 0x17, 0x57, 0x28, 0xEB, 0x2B, 0x4E,
		0x8F, 0xD0, 0xC0, 0x6A, 0xAD, 0x79, 0x98, 0x0F, 0x8D, 0x43, 0x8D, 0x47, 0x04, 0xB8, 0x2B, 0xF4,
		0x15, 0x21, 0x56, 0x19, 0x01, 0x40, 0x01, 0x3B, 0xD0, 0x91, 0x90, 0x62, 0x9E, 0x89, 0xC2, 0x27,
		0x8E, 0xCF, 0xB6, 0xDB, 0xCE, 0x3F, 0x72, 0x10, 0x50, 0x93, 0x8C, 0x23, 0x29, 0x83, 0x7B, 0x80,
		0x64, 0xA7, 0x59, 0xE8, 0x61, 0x67, 0x4C, 0xBC, 0xD8, 0x58, 0xB8, 0xF1, 0xD4, 0xF8, 0x2C, 0x37,
		0x98, 0x16, 0x26, 0x0E, 0x4E, 0xF9, 0x4E, 0xEE, 0x24, 0xDE, 0xCC, 0xD1, 0x4B, 0x4B, 0xC5, 0x06,
		0x7A, 0xFB, 0x49, 0x65, 0xE6, 0xC0, 0x00, 0x83, 0x48, 0x1E, 0x8E, 0x42, 0x2A, 0x53, 0xA0, 0xF5,
		0x37, 0x29, 0x2B, 0x5A, 0xF9, 0x73, 0xC5, 0x9A, 0xA1, 0xB5, 0xB5, 0x74, 0x7C, 0x06, 0xDC, 0x7B,
		0x7C, 0xDC, 0x6C, 0x6E, 0x82, 0x6B, 0x49, 0x88, 0xD4, 0x1B, 0x25, 0xE0, 0xEE, 0xD1, 0x79, 0xBD,
		0x39, 0x85, 0xFA, 0x4F, 0x25, 0xEC, 0x70, 0x19, 0x23, 0xC1, 0xB9, 0xA6, 0xD9, 0x7E, 0x3E, 0xDA,
		0x48, 0xA9, 0x58, 0xE3, 0x18, 0x14, 0x1E, 0x9F, 0x30, 0x7F, 0x4C, 0xA8, 0xAE, 0x53, 0x22, 0x66,
		0x2B, 0xBE, 0x24, 0xCB, 0x47, 0x66, 0xFC, 0x83, 0xCF, 0x5C, 0x2D, 0x1E, 0x3A, 0xAB, 0xAB, 0x06,
		0xBE, 0x05, 0xAA, 0x1A, 0x9B, 0x2D, 0xB7, 0xA6, 0x54, 0xF3, 0x63, 0x2B, 0x97, 0xBF, 0x93, 0xBE,
		0xC1, 0xAF, 0x21, 0x39, 0x49, 0x0C, 0xE9, 0x31, 0x90, 0xCC, 0xC2, 0xBB, 0x3C, 0x02, 0xC4, 0xE2,
		0xBD, 0xBD, 0x2F, 0x84, 0x63, 0x9B, 0xD2, 0xDD, 0x78, 0x3E, 0x90, 0xC6, 0xC5, 0xAC, 0x16, 0x77,
		0x2E, 0x69, 0x6C, 0x77, 0xFD, 0xED, 0x8A, 0x4D, 0x6A, 0x8C, 0xA3, 0xA9, 0x25, 0x6C, 0x21, 0xFD,
		0xB2, 0x94, 0x0C, 0x84, 0xAA, 0x07, 0x29, 0x26, 0x46, 0xF7, 0x9B, 0x3A, 0x19, 0x87, 0xE0, 0x9F,
		0xEB, 0x30, 0xA8, 0xF5, 0x64, 0xEB, 0x07, 0xF1, 0xE9, 0xDB, 0xF9, 0xAF, 0x2C, 0x8B, 0x69, 0x7E,
		0x2E, 0x67, 0x39, 0x3F, 0xF3, 0xA6, 0xE5, 0xCD, 0xDA, 0x24, 0x9B, 0xA2, 0x78, 0x72, 0xF0, 0xA2,
		0x27, 0xC3, 0xE0, 0x25, 0xB4, 0xA1, 0x04, 0x6A, 0x59, 0x80, 0x27, 0xB5, 0xDA, 0xB4, 0xB4, 0x53,
		0x97, 0x3B, 0x28, 0x99, 0xAC, 0xF4, 0x96, 0x27, 0x0F, 0x7F, 0x30, 0x0C, 0x4A, 0xAF, 0xCB, 0x9E,
		0xD8, 0x71, 0x28, 0x24, 0x3E, 0xBC, 0x35, 0x15, 0xBE, 0x13, 0xEB, 0xAF, 0x43, 0x01, 0xBD, 0x61,
		0x24, 0x54, 0x34, 0x9F, 0x73, 0x3E, 0xB5, 0x10, 0x9F, 0xC9, 0xFC, 0x80, 0xE8, 0x4D, 0xE3, 0x32,
		0x96, 0x8F, 0x88, 0x10, 0x23, 0x25, 0xF3, 0xD3, 0x3E, 0x6E, 0x6D, 0xBB, 0xDC, 0x29, 0x66, 0xEB,

		//Public Exponent e:
		0x03
};








static int crypt_xor( const uint8_t *s1, const uint8_t *s2, uint8_t *d, int len );

struct HDCPTxEvent : public TimedEventQueue::Event {
    HDCPTxEvent(
            TxTimeOutType eType,
            WFDHDCPTx *player,
            void (WFDHDCPTx::*method)(TxTimeOutType))
        : mType(eType),
          mPlayer(player),
          mMethod(method) {
    }

protected:
    virtual ~HDCPTxEvent() {}

    virtual void fire(TimedEventQueue *queue, int64_t /* now_us */) {
        (mPlayer->*mMethod)(mType);
    }

private:
    TxTimeOutType mType;
    WFDHDCPTx *mPlayer;
    void (WFDHDCPTx::*mMethod)(TxTimeOutType);

    HDCPTxEvent(const HDCPTxEvent &);
    HDCPTxEvent &operator=(const HDCPTxEvent &);
};

WFDHDCPTx::WFDHDCPTx(void *cookie, HDCPModule::ObserverFunc observerNotify)
{
    int i;
    
    mCookie = cookie;
    mObserverNotify = observerNotify;
    mQueueStarted = false;
    mRxPrecomputeLSupport = false;

	/* default: set HDCP2_0 */
    //mVersion = HDCP2_0;	

    hdcp2x_tx_info = NULL;

	/* create time out event*/
    for (i = 0; i < HDCP2X_TX_TIMEOUT_MAX; i++)
    {
        mTimeOutEvent[i] = new HDCPTxEvent((TxTimeOutType)i, this, &WFDHDCPTx::onTimeOutEvent);
        mTimeOutEventPending[i] = false;
        mTimeOutEventHappend[i] = false;
    }

	/* resoure init and state set */
    hdcp2x_tx_init();
    reset();
	
	
}

WFDHDCPTx::~WFDHDCPTx() 
{	
    HDCP2X_FUNC_LOG();

    if (mQueueStarted) {
        mQueue.stop();
        cancelAllTimeOutEvent();
    }

    if (mTxSession != NULL)
        mTxSession.clear();
    if (mTxLooper != NULL)
        mTxLooper.clear();

    reset();
    
    if (hdcp2x_tx_info)
    {
        free(hdcp2x_tx_info);
        hdcp2x_tx_info = NULL;
    }
}

void WFDHDCPTx::onTimeOutEvent(TxTimeOutType eType)
{
    status_t err;
    HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	
    HDCP2X_DBG(HDCP2XDBG_VERB, "<-- onTimeOutEvent [%s] ", hdcp2x_event_str[eType]);

    if (!mTimeOutEventPending[eType]) 
    {
        return;
    }
    
    mTimeOutEventPending[eType] = false;
    mTimeOutEventHappend[eType] = true;


	switch(eType)
	{
		case HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_INFO:
			// do not receive AKE_Receiver_Info within 100ms, treat RX as HDCP 2.0-compliant Device
			/* hdcp 2.0 */
	    	/* no pre-compute */
	        mRxPrecomputeLSupport = false;
			txInfo_p->run_in_hdcp = HDCP2_0;
			
	        HDCP2X_DBG(HDCP2XDBG_VERB, "do not receive AKE_Receiver_Info within 100ms, treate RX as HDCP 2.0-compliant Device");
	        err = hdcp2x_tx_sendKm();
	        if (err != HDCP2X_OK)
	        {
	        	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
	        }
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_SHUTDOWN:
			shutdownComplete();
			break;

			
#if HDCP2X_TIME_OUT_CHECK_ENABLE

		case HDCP2X_TX_TIMEOUT_WAIT_RX_CONNECTION:
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_H_PRIME:
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_PAIRING_INFO:
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_L_PRIME:
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_RTT_READY:
			HDCP2X_DBG(HDCP2XDBG_VERB, "TIMEOUT_WAIT_RTT_READY");
			while(mLC_Try_Count--)
			{
				if((err = hdcp2x_tx_sendLc()) != HDCP2X_OK)
				{
					HDCP2X_DBG(HDCP2XDBG_ERR, "send LC failed err = %d !\n", err);
					continue;
				}
				else
				{
					HDCP2X_DBG(HDCP2XDBG_ERR, "send LC successful!\n");
					return (HDCP2X_OK);
				}	
			}

			HDCP2X_DBG(HDCP2XDBG_ERR, "LC init failed!\n");
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_AUTH_FAIL:
			break;

		case HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID:
			break;
		case HDCP2X_TX_TIMEOUT_WAIT_STREAM_READY:
			break;
#endif

		default:
			hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
			break;
			

	}












	
}

void WFDHDCPTx::postTimeOutEvent_l(TxTimeOutType eType, int64_t delayUs) 
{
    HDCP2X_DBG(HDCP2XDBG_VERB, "postTimeOutEvent [%s] delay = %d ms", hdcp2x_event_str[eType], (int)(delayUs/1000));

    if (mTimeOutEventPending[eType]) 
    {
        return;
    }

    mTimeOutEventPending[eType] = true;
    mQueue.postEventWithDelay(mTimeOutEvent[eType], delayUs);
}

void WFDHDCPTx::cancelTimeOutEvent(TxTimeOutType eType)
{
    if (mTimeOutEventPending[eType]) 
    {
        HDCP2X_DBG(HDCP2XDBG_VERB, "cancelTimeOutEvent [%s] id = %d ", hdcp2x_event_str[eType], mTimeOutEvent[eType]->eventID());
        mQueue.cancelEvent(mTimeOutEvent[eType]->eventID());
        mTimeOutEventPending[eType] = false;
    }
    
    mTimeOutEventHappend[eType] = false;
}

void WFDHDCPTx::cancelAllTimeOutEvent()
{
    int i;
    
    HDCP2X_DBG(HDCP2XDBG_VERB, "cancelAllTimeOutEvent ");

    for (i = 0; i < HDCP2X_TX_TIMEOUT_MAX; i++)
    {
        mQueue.cancelEvent(mTimeOutEvent[i]->eventID());
        mTimeOutEventPending[i] = false;
        mTimeOutEventHappend[i] = false;
    }        
}


void WFDHDCPTx::onMessageReceived(const sp<AMessage> &msg) 
{
    int32_t reason;

    CHECK(msg->findInt32("reason", &reason));

    HDCP2X_DBG(HDCP2XDBG_VERB, "[%s] -> onMessageReceived = %d reason = %d", hdcp2x_tx_str[mState], msg->what(), reason);

    if ((mState == HDCP2X_TX_STATE_AUTHENTICATE_FAIL) ||
        (mState == HDCP2X_TX_STATE_DETACHED))
    {
        return;
    }

    switch (msg->what()) 
    {
        case kWhatANSNotify : 
        {
            switch (reason)
            {

				/* handle hdcp rx authentication */
                case ANetworkSession::kWhatBinaryData:
                {
                    sp<ABuffer> data;
                    CHECK(msg->findBuffer("data", &data));
                    HDCP2X_DBG(HDCP2XDBG_DBG, " <-- ANetworkSession::kWhatBinaryData\n");

                    // if data len = 0, just skip this packet and do not process it
                    if (data->size() != 0)
                    {
                        hdcp2x_tx_processData(data->data(), data->size());
                    }                        
                    break;
                }

				/* handle hdcp rx connected event */
                case ANetworkSession::kWhatConnected:

                    HDCP2X_DBG(HDCP2XDBG_DBG, " <-- ANetworkSession::kWhatConnected\n");
                
                    CHECK(mState == HDCP2X_TX_STATE_WAIT_ATTACHED);

                    hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AKE_INIT, NULL);
                    
                    cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_RX_CONNECTION);



					/*
						Initiate authentication:AKE_Init
					*/
					reason = hdcp2x_tx_sendAkeInit();
					if(reason != HDCP2X_OK)
                    {
                        HDCP2X_DBG(HDCP2XDBG_ERR, "ake_init error [%x]!\n", reason);
                        hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
						break;
                    }
					HDCP2X_DBG(HDCP2XDBG_DBG, "Initiate authentication:AKE_Init OK\n");


					
                    /*
						send Transmiterr Info
					*/
					reason = hdcp2x_tx_sendAkeTransmitterInfo();
					if( reason != HDCP2X_OK)
					{
						HDCP2X_DBG(HDCP2XDBG_ERR, "send tx info err [%x]\n", reason);
                        hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
						break;							
					}
					HDCP2X_DBG(HDCP2XDBG_DBG, "send Transmiterr Info OK\n");


					/*
						set the timer in 100ms (must receive the rx cert and rx info within 100ms after tx info was sent)
					*/
					postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_INFO, HDCP2X_TX_WAIT_RECEIVER_INFO_US);
					hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_CERT, NULL);
					HDCP2X_DBG(HDCP2XDBG_DBG, "AKE_Receiver_Info should be received within 100ms if Receiver is HDCP2.1 or HDCP2.2 \n");

										  
                    break;

                case ANetworkSession::kWhatError:
                    int32_t err;
                    CHECK(msg->findInt32("err", &err));
                    HDCP2X_DBG(HDCP2XDBG_DBG, " <-- ANetworkSession::kWhatError = %d\n", err);

                    // If authentication already finished, just skip this error message, let WifiDisplaySource handle connection error
                    if (mState != HDCP2X_TX_STATE_AUTHENTICATED)
                    {
                        hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
                    }                        
                    break;
                    
                          
                default:
                    break;
            }                    

        }
            break;

        default:
            break;
        
        
    }
}


status_t WFDHDCPTx::startAuthentication(const char *host, unsigned port)
{
    status_t err = HDCP2X_OK;

	UINT32 u4Data;
	unsigned char key_set[880] = {0};

	HDCP2X_DBG(HDCP2XDBG_ERR, " >> %s:\n", __FUNCTION__);

	/* init mtal & key set */
	if (err = MTAL_Init())
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s: Fail to init MTAL\n", __FUNCTION__);
	}

	
    if (err != MTR_OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "MTDRM_HDCP2_Init fail = 0x%x", err);

        // use time out event to callback HDCP_INITIALIZATION_FAILED asynchously
        postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_AUTH_FAIL, HDCP2X_TX_WAIT_AUTH_FAIL_US);
        
        return (err);
    }


	
	//FIX: ADD API TO SET THE LC128 INTO TEE
	
	/* read enc 128 (key set) */
	/* maybe need some fix : now use rx key */
	err = MTEEPROM_GetHdcp2xOffset(&u4Data);
	if (err)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "get hdcp2x offset failed\n");
		u4Data = (UINT32)2672;
	}
	
	err = MTEEPROM_Read((UINT32)u4Data, (UINT8*)key_set, 880);
    if (err)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "read eeprom fail.\n");
        return -1;
    }
	DumpHex("key_set:", key_set, 880);


	/* set enc 128 */
	err = MTDRM_HDCP2_SetEncLc128Key((MTDRM_HDCP2_ENCLC128_T *)key_set);
	if(0 == err)
	{
		HDCP2X_DBG(HDCP2XDBG_VERB, "<-- MTDRM_HDCP2_SetEncLc128Key ok\n");
	
	}
	else
	{
		HDCP2X_DBG(HDCP2XDBG_VERB, "<-- MTDRM_HDCP2_SetEncLc128Key failure\n");
		return -1;
	}





    hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_ATTACHED, NULL);

    cancelAllTimeOutEvent();

    err = openConnection(host, port);

    if (err != HDCP2X_OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "openConnection fail = %d", err);

        // use time out event to callback HDCP_INITIALIZATION_FAILED asynchously
        postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_AUTH_FAIL, HDCP2X_TX_WAIT_AUTH_FAIL_US);
        
        return (err);
    }
    else
    {
        HDCP2X_DBG(HDCP2XDBG_VERB, "openConnection success");
    }

    // then wait ANetworkSeesion:kWhatConnected notify

    return (HDCP2X_OK);

}


/*
	connect to HDCP RX Server
*/
status_t WFDHDCPTx::openConnection(const char *host, unsigned port) 
{
    status_t ret;
 
    HDCP2X_FUNC_LOG();

    mTxLooper = new ALooper;
    mTxLooper->setName("hdcptx_looper");

    mTxSession = new ANetworkSession;
    ret = mTxSession->start();
    if (OK != ret)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "ANetworkSession start err = %d\n", ret);
        return (ret);
    }

    mTxLooper->registerHandler(this);
    mTxLooper->start(
             false /* runOnCallingThread */,
             false /* canCallJava */,
             PRIORITY_AUDIO);

    sp<AMessage> rAnsNotify = new AMessage(kWhatANSNotify, id());

    // active mode
    ret = mTxSession->createTCPBinaryDataSessionActive(0, host, port, rAnsNotify, &mSessionId);

    if (OK != ret)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "createTCPBinaryDatagramSessionActive err = %d\n", ret);
        return (ret);
    }
    else
    {
        HDCP2X_DBG(HDCP2XDBG_DBG, "createTCPBinaryDatagramSessionActive session_id = %d\n", mSessionId);
    }

    postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_RX_CONNECTION, HDCP2X_TX_WAIT_RX_CONNECTION_US);

    return (HDCP2X_OK);

}

status_t WFDHDCPTx::shutdownAsync()
{
    status_t ret;

    if (mTxSession != NULL)
    {
        if (mSessionId != -1)
        {
            // close connection
            ret = mTxSession->destroySession(mSessionId);
            if (OK != ret)
            {
                HDCP2X_DBG(HDCP2XDBG_DBG, "destroySession session_id = %d fail\n", mSessionId);
            }        
        }
        
        ret = mTxSession->stop();
        if (OK != ret)
        {
            HDCP2X_DBG(HDCP2XDBG_ERR, "ANetworkSession stop err = %d\n", ret);
        }
    }        

    // use time out event to callback HDCP_SHUTDOWN_COMPLETE asynchously
    postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_SHUTDOWN, HDCP2X_TX_WAIT_SHUTDOWN_US);

    return (HDCP2X_OK);
}

status_t WFDHDCPTx::shutdownComplete()
{
    status_t ret;

    HDCP2X_FUNC_LOG();

    if (mTxLooper != NULL)
    {
        mTxLooper->stop();
        mTxLooper->unregisterHandler(id());
    }
    
    reset();
    hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_DETACHED, NULL);


    return (HDCP2X_OK);
}

void WFDHDCPTx::reset() 
{
    timeout = 0;
    for (int i = 0; i < HDCP2X_TX_MAX_STREAM_NUM; i++)
    {
        mEncryptCnt[i] = 0;
        mInputCtr[i] = 0;
    }        
    no_stored = 1;
    mSessionId = -1;
    mLC_Try_Count = DEFAULT_TX_RETRY_SEND_COUNT;
    mRxPrecomputeLSupport = false;
}

status_t WFDHDCPTx::sendRequest(unsigned char *buf, UINT_16 len) 
{
    status_t err;

    HDCP2X_FUNC_LOG();

    err = mTxSession->sendRequest(mSessionId, (const void *)(buf), (ssize_t)(len));

    if (err != OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "sendRequest err = %d\n", err);
        return (err);
    }

	return (HDCP2X_OK);
}

inline uint64_t GETU64BE(const void *p) {
     const uint8_t *ptr = (const uint8_t *) p;
     return (((uint64_t ) ptr[0] << 56) 
             | ((uint64_t ) ptr[1] << 48)
             | ((uint64_t ) ptr[2] << 40)
             | ((uint64_t ) ptr[3] << 32) 
             | ((uint64_t ) ptr[4] << 24)
             | ((uint64_t ) ptr[5] << 16)
             | ((uint64_t ) ptr[6] << 8) 
             | ptr[7]);
}



#define HDCP2_TX_ENCRYPT_PROFILING


status_t WFDHDCPTx::encrypt(
           const void *inData, size_t size, uint32_t streamCTR,
           uint64_t *outInputCTR, void *outData)
{
	int ret = -1;
	unsigned char counter[16] = {0}; 
	unsigned char riv[8] = {0};
	/* set up IV*/
    unsigned char rx_p[HDCP2X_LC128_LEN]={0xff};

	
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	memcpy(riv, txInfo_p->riv, HDCP2X_RIV_LEN);



    union
    {
        unsigned long a[2];
        unsigned long long i;
    } u_streamCtr, u_inputCtr;

	u_streamCtr.a[0] = htonl(0);
	u_streamCtr.a[1] = htonl(streamCTR);

	/* Deal with inputCtr */
	u_inputCtr.a[0] = htonl(((unsigned long*)outInputCTR)[1]);
	u_inputCtr.a[1] = htonl(((unsigned long*)outInputCTR)[0]);

	memcpy(rx_p, riv, HDCP2X_RIV_LEN);

	/* xor stream counter */
	crypt_xor( riv, (unsigned char*)&u_streamCtr.i, rx_p , HDCP2X_RIV_LEN);

	/* or input counter */
	//memcpy(rx_p+RIV_SIZE, &llTempCounter, RIV_SIZE);
	memcpy(rx_p+HDCP2X_RIV_LEN, &u_inputCtr.i, HDCP2X_RIV_LEN);



    if (MTR_OK != MTDRM_HDCP2_DataEncrypt((unsigned char*)outData, (unsigned char*)inData, size, rx_p))
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "[%s]encryption error\n", __FUNCTION__);
        ret = -1;
    }

    return (HDCP2X_OK);

}





int WFDHDCPTx::hdcp2x_tx_genRtx()
{
	HDCP2X_TX_INFO_T *info_p = hdcp2x_tx_info;


    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
#if defined (WFD_USE_HDCP_FACSIMILE_KEY) 
    /* use facsimile key */
    memcpy(info_p->rtx, facskey_rtx, HDCP2X_RTX_LEN);

#elif defined (WFD_HDCP_USE_TEST_VECTOR)
    /* use MTK test vectors */
    memcpy(info_p->rtx, TestkeypRtx, HDCP2X_RTX_LEN);

#else
    /* use real random number */


	int rand1;
	int rand2;
    static int seeded = 0;
    struct timeval tv;
    
    if (!seeded)
    {
        gettimeofday(&tv, NULL);
        srand((unsigned)(tv.tv_sec ^ tv.tv_usec ^ (int)hdcp2x_tx_info));
        seeded = 1;
    }

	rand1 = hdcp2x_cmmGetRandom();
	rand2 = hdcp2x_cmmGetRandom();	
	memcpy(info_p->rtx, &rand1, sizeof(int));
	memcpy(info_p->rtx+4, &rand2, sizeof(int));
#endif

    DumpHex("rtx:", info_p->rtx, HDCP2X_RTX_LEN);

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

    return 0;
}

int WFDHDCPTx::hdcp2x_tx_sendAkeInit()
{
	int ret;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
    
	
	//generation rtx
	if(hdcp2x_tx_genRtx() < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "\t Generate Rtx failed!\n");
		return (HDCP2X_ERR_KEY_GEN_RTX);
	}
	
	MSG_AKE_INIT_T *ake_init = NULL;
	ake_init = (MSG_AKE_INIT_T *)malloc(HDCP2X_MSGLEN_AKE_INIT);
	if(!ake_init)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "\t ake_init allocation failed!\n");
		return (HDCP2X_ERR_OUT_OF_MEM);
	}
	memset(ake_init, 0, HDCP2X_MSGLEN_AKE_INIT);
	ake_init->msg_id = HDCP2X_MSGID_AKE_INIT;
	memcpy(ake_init->rtx, txInfo_p->rtx, HDCP2X_RTX_LEN);

	//ret = send(txInfo_p->confd, (MSG_AKE_INIT_T *)ake_init, HDCP2X_MSGLEN_AKE_INIT, 0);
	ret = sendRequest ((unsigned char *)ake_init, HDCP2X_MSGLEN_AKE_INIT);

	if(ake_init)
	{
		free(ake_init);
		ake_init = NULL;
	}
	if(ret <= 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "\t send ake_init failed!\n");
		return (HDCP2X_ERR_SEND_AKE_INIT);
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);	
	return HDCP2X_OK;
}




int WFDHDCPTx::hdcp2x_tx_sendAkeTransmitterInfo()
{
    int err;
    HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
    MSG_AKE_TRANSMITTER_INFO_T *ake_tx_info = NULL;
    
    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
    
    ake_tx_info = (MSG_AKE_TRANSMITTER_INFO_T *)malloc(HDCP2X_MSGLEN_AKE_TRANSMITTER_INFO);
    if(!ake_tx_info)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "\t AKE_Transmitter_Info allocation failed!\n");
        return (HDCP2X_ERR_OUT_OF_MEM);
    }

    memset(ake_tx_info, 0, HDCP2X_MSGLEN_AKE_TRANSMITTER_INFO);
    ake_tx_info->msg_id = HDCP2X_MSGID_AKE_TRANSMITTER_INFO;
    ake_tx_info->length[1] = HDCP2X_MSGLEN_AKE_TRANSMITTER_INFO;

	
    ake_tx_info->version = (unsigned char)(txInfo_p->max_support_hdcp_version); 

	
    ake_tx_info->transmitter_capability_mask[1] = 1; // precompute support

	/*
		save tx cap mask 
	*/
	memcpy(txInfo_p->tx_cap_mask, ake_tx_info->transmitter_capability_mask, HDCP2X_LEN_CAP_MASK);

	
  	HDCP2X_DBG(HDCP2XDBG_ERR, "\t set transmitter_capability_mask[1] 1 to enable precompute\n");
    
    err = sendRequest ((unsigned char *)ake_tx_info, HDCP2X_MSGLEN_AKE_TRANSMITTER_INFO);

    if(ake_tx_info)
    {
        free(ake_tx_info);
        ake_tx_info = NULL;
    }
	
    if(err <= 0)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "\t send AKE_Transmitter_Info failed err = %d!\n", err);
        return (HDCP2X_ERR_SEND_AKE_TX_INFO);
    }

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);
    return (HDCP2X_OK);
}



int WFDHDCPTx::hdcp2x_tx_verifyCert(unsigned char *cert)
{
	

	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);

	DumpHex("cert:", (unsigned char *)cert, HDCP2X_CERTRX_LEN);
	
	memcpy(&(txInfo_p->cert), (HDCP2X_RXCERT_T *)cert, sizeof(HDCP2X_RXCERT_T));

	//HDCP2X_DBG(HDCP2XDBG_ERR, "VerifySignature not needed \n");

	txInfo_p->rx_protol_descriptor = (txInfo_p->cert.ProtocolDescripter >> 4) & 0x0f;

	
	HDCP2X_DBG(HDCP2XDBG_ERR, "RX Cert ProtocolDescripter[%02x]\n",  txInfo_p->rx_protol_descriptor);

#if 0
	if(MTDRM_HDCP2_VerifySignature(dcp_key_pub, txInfo_p->cert.RootSign) < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Invalid rx signature!\n");
		return -1;
	}
#endif

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);


	return 0;
}



int WFDHDCPTx::hdcp2x_tx_fsmWaitCert(unsigned char *data, int length)
{
	MSG_AKE_SEND_CERT_T *rxCert = NULL;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	int ret = -1;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	if(!data || !length)
	{
        HDCP2X_DBG(HDCP2XDBG_ERR, "%s : incorrect message (data = %p length = %d)\n",
        __FUNCTION__, data, length);
		
		return (HDCP2X_ERR_INV_RX_MSG);
	}
	
	rxCert = (MSG_AKE_SEND_CERT_T *)data;
    if(rxCert->msg_id != HDCP2X_MSGID_AKE_SEND_CERT)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "%s : incorrect message (expect = %d recv = %d)\n",
        __FUNCTION__, HDCP2X_MSGID_AKE_SEND_CERT, rxCert->msg_id);
		
        return (HDCP2X_ERR_INV_RX_MSG);
    }


	/* msg invalid ? */
	if (length < HDCP2X_MSGLEN_AKE_SEND_CERT)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) :Error incorrect length (expected = %d recv=%d)\n",
					  __FUNCTION__, HDCP2X_MSGLEN_AKE_SEND_CERT, length);
		
		return (HDCP2X_ERR_INV_RX_MSG);
	}


	/*rx is repeater ? */
    txInfo_p->repeater = rxCert->repeater;
	HDCP2X_DBG(HDCP2XDBG_ERR, "RX REPEATER[%d]\n", txInfo_p->repeater);

	
	//verify the signature of cert with kpub
	if(hdcp2x_tx_verifyCert((unsigned char *)rxCert->certrx) != 0)
	{
		//HDCP2X_DBG(HDCP2XDBG_ERR, "Verify Rx Cert failed!\n");
		return (HDCP2X_ERR_INVALID_CERT);
	}

	//HDCP2X_DBG(HDCP2XDBG_ERR, "Verify Rx Cert OK \n");


	/*msg type */
	if (length < HDCP2X_MSGLEN_AKE_SEND_CERT + HDCP2X_MSGLEN_AKE_RECEIVER_INFO)
	{
		/*rx cert was received */
        hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_RECEIVER_INFO, NULL);
        ret = (HDCP2X_OK);		

	}
	else if (length >= HDCP2X_MSGLEN_AKE_SEND_CERT + HDCP2X_MSGLEN_AKE_RECEIVER_INFO)
	{
		/* rx cert and rx info was received at the same time */
		HDCP2X_DBG(HDCP2XDBG_ERR, "rx_cert & rx_info received combined \n");

		ret = (hdcp2x_tx_fsmHandleRxInfo(data+HDCP2X_MSGLEN_AKE_SEND_CERT, HDCP2X_MSGLEN_AKE_RECEIVER_INFO));		
	}


	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s (%d) \n",__FUNCTION__, ret);


	return ret;

	
}









int WFDHDCPTx::hdcp2x_tx_fsmWaitRrx(unsigned char *data, int length)
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	
	MSG_AKE_SEND_RRX_T *Rrx = NULL;
	unsigned char *pH_Prime = NULL;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	
	Rrx = (MSG_AKE_SEND_RRX_T *)data;
	if(Rrx->msg_id != HDCP2X_MSGID_AKE_SEND_RRX)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s): incorrect message (expected = %d recv = %d)\n",
			__FUNCTION__, HDCP2X_MSGID_AKE_SEND_RRX, Rrx->msg_id);
		return (HDCP2X_ERR_INV_RX_MSG);
	}


    if (length != HDCP2X_MSGLEN_AKE_SEND_RRX)
    {
        if (length < HDCP2X_MSGLEN_AKE_SEND_RRX)
        {
            HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) :Error incorrect length (expected = %d recv=%d)\n",
                            __FUNCTION__, HDCP2X_MSGLEN_AKE_SEND_RRX, length);
			
            return (HDCP2X_ERR_INV_RX_MSG);
        }
        else
        {
            /* Note:
                  It could happen that AKE_Send_rrx and AKE_Send_H_prime comes in the same TCP packet
                  if Rx sends too fast. We need to handle them separately.
            */
            HDCP2X_DBG(HDCP2XDBG_DBG, "Received 7.AKE_Send_H_prime combined\n");
            pH_Prime = data + HDCP2X_MSGLEN_AKE_SEND_RRX;
        }
    }

    DumpHex("rrx:", Rrx->rrx, HDCP2X_RRX_LEN);


    memcpy(txInfo_p->rrx, Rrx->rrx, HDCP2X_RRX_LEN);

    if (length == HDCP2X_MSGLEN_AKE_SEND_RRX)
    {
    	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_H_PRIME, NULL);
    }   
    else
    {
        hdcp2x_tx_fsmWaitHPrime(pH_Prime, length - HDCP2X_MSGLEN_AKE_SEND_RRX);
    }

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

    return (HDCP2X_OK);

	
}

int WFDHDCPTx::hdcp2x_tx_sendStoredKm()
{
	int ret;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);

#if 0
	//need to do : retrieve the ekh and m
 	// GET THE PAIRING INFO AND SET THE KM INTO TEE
 	//        AND SET The associated KM IN TEE ACTIVE
	if(MTDRM_HDCP2_RetrvEkh_km(txInfo_p->ekh_km, txInfo_p->m, txInfo_p->cert.ID) != 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Retrieve ekh and m failed!\n");
		return -1;
	}
#endif
	MSG_AKE_STORED_KM_T *stored_km;
	stored_km = (MSG_AKE_STORED_KM_T *)malloc(HDCP2X_MSGLEN_AKE_STORED_KM);
	if(!stored_km)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "stored_km malloc failed!\n");
		return -1;
	}
	memset(stored_km, 0, HDCP2X_MSGLEN_AKE_STORED_KM);
	stored_km->msg_id = HDCP2X_MSGID_AKE_STORED_KM;
	memcpy(stored_km->Ekh_km, txInfo_p->ekh_km, HDCP2X_EKH_KM_LEN);
	memcpy(stored_km->m, txInfo_p->m, HDCP2X_M_LEN);

	//ret = send(txInfo_p->confd, (MSG_AKE_STORED_KM_T *)stored_km, HDCP2X_MSGLEN_AKE_STORED_KM, 0);
	ret = sendRequest( (unsigned char *)stored_km, HDCP2X_MSGLEN_AKE_STORED_KM);
	if(stored_km)
	{
		free(stored_km);
		stored_km = NULL;
	}

	if(ret <= 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "send stored_km failed!\n");
		return -1;
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "send stored_km ok\n");

	return 0;
	
}

int WFDHDCPTx::hdcp2x_tx_genKm()
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
#if defined (WFD_USE_HDCP_FACSIMILE_KEY) 

    memcpy(txInfo_p->km, facskey_km, sizeof(txInfo_p->km));

#elif defined (WFD_HDCP_USE_TEST_VECTOR)
  
    memcpy(txInfo_p->km, TestkeyKm, HDCP2X_KM_LEN);

#else

	int rand1,rand2,rand3,rand4;

	rand1 = hdcp2x_cmmGetRandom();
	rand2 = hdcp2x_cmmGetRandom();
	rand3 = hdcp2x_cmmGetRandom();
	rand4 = hdcp2x_cmmGetRandom();

	memcpy(txInfo_p->km, &rand1, sizeof(int));
	memcpy(txInfo_p->km+4, &rand2, sizeof(int));
	memcpy(txInfo_p->km+8, &rand3, sizeof(int));
	memcpy(txInfo_p->km+12, &rand4, sizeof(int));
#endif
	//HDCP2X_DBG(HDCP2XDBG_VERB, "Dumping km\n");
    DumpHex("km:", txInfo_p->km, sizeof(txInfo_p->km));

    return 0;
}

int WFDHDCPTx::hdcp2x_tx_sendNoStoredKm()
{
	int ret;
	HDCP2X_TX_INFO_T *txInfo_p = NULL;


    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);

	txInfo_p = hdcp2x_tx_info;
		
	MSG_AKE_NO_STORED_KM_T *no_stored_km;
	no_stored_km = (MSG_AKE_NO_STORED_KM_T *)malloc(HDCP2X_MSGLEN_AKE_NO_STORED_KM);
	if(!no_stored_km)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "no_stored_km malloc failed!\n");
		return -1;
	}
	memset(no_stored_km, 0, HDCP2X_MSGLEN_AKE_NO_STORED_KM);
	no_stored_km->msg_id = HDCP2X_MSGID_AKE_NO_STORED_KM;

#if defined (WFD_USE_HDCP_FACSIMILE_KEY)  
    memcpy(no_stored_km->Ekpub_km, facskey_Ekpub_km, HDCP2X_EKPUB_KM_LEN);
#elif defined (WFD_HDCP_USE_TEST_VECTOR)
    memcpy(no_stored_km->Ekpub_km, Testkey_Ekpub, HDCP2X_EKPUB_KM_LEN);
#else
#if 0
	//encrypt km
	if(MTDRM_HDCP2_GenerateKmForTx(no_stored_km->Ekpub_km) < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Encrypt km failed!\n");
		return -1;
	}
#endif
#endif

	//ret = send(txInfo_p->confd, (MSG_AKE_NO_STORED_KM_T *)no_stored_km, HDCP2X_MSGLEN_AKE_NO_STORED_KM, 0);
	ret = sendRequest( (unsigned char *)no_stored_km, HDCP2X_MSGLEN_AKE_NO_STORED_KM);

	if(no_stored_km)
	{
		free(no_stored_km);
		no_stored_km = NULL;
	}
	
	if(ret <= 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "send no_stored_km failed!\n");
		return -1;
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "send no_stored_km ok\n");

	return 0;
		
}


int WFDHDCPTx::hdcp2x_tx_genRn()
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

#if defined (WFD_USE_HDCP_FACSIMILE_KEY) 
    memcpy(txInfo_p->rn, facskey_rn, HDCP2X_RN_LEN);
#elif defined (WFD_HDCP_USE_TEST_VECTOR)
    memcpy(txInfo_p->rn, TestkeypRn, HDCP2X_RN_LEN);
#else
	int rand1, rand2;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	rand1 = hdcp2x_cmmGetRandom();
	rand2 = hdcp2x_cmmGetRandom();

	memcpy(txInfo_p->rn, &rand1, sizeof(int));
	memcpy(txInfo_p->rn+4, &rand2, sizeof(int));

	HDCP2X_DBG(HDCP2XDBG_VERB, "rn=[%02x %02x %02x %02x %02x %02x %02x %02x]\n",
		txInfo_p->rn[0], txInfo_p->rn[1], txInfo_p->rn[2], txInfo_p->rn[3], 
        txInfo_p->rn[4], txInfo_p->rn[5], txInfo_p->rn[6], txInfo_p->rn[7]);
#endif

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return 0;
}

int WFDHDCPTx::hdcp2x_tx_sendLc()
{
	int ret;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	MSG_LC_INIT_T *lc_init = NULL;
	MTDRM_HDCP2_CALLPRIME_T *L = NULL;
	lc_init = (MSG_LC_INIT_T *)malloc(HDCP2X_MSGLEN_LC_INIT);


    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	if(!lc_init)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc lc_init failed!\n");
		return (HDCP2X_ERR_OUT_OF_MEM);
	}

	//need to do : generate rn 
	if(hdcp2x_tx_genRn() < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Generate rn failed!\n");
        free(lc_init);
		return (HDCP2X_ERR_GEN_RN);
	}
	
	lc_init->msg_id = HDCP2X_MSGID_LC_INIT;
	memcpy(lc_init->rn, txInfo_p->rn, HDCP2X_RN_LEN);

	//ret = send(txInfo_p->confd, (MSG_LC_INIT_T *)lc_init, HDCP2X_MSGLEN_LC_INIT, 0);
	ret = sendRequest((unsigned char *)lc_init, HDCP2X_MSGLEN_LC_INIT);

	if(lc_init)
	{
		free(lc_init);
		lc_init = NULL;
	}
	
	if(ret <= 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "send lc_init failed!\n");
		return (HDCP2X_ERR_SEND_LC_INIT);
	}

	if (mRxPrecomputeLSupport)  // HDCP 2.1 or HDCP 2.2
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "LC Precompute Enable\n");

		//compute L = HMAC-SHA256(rn , kd XOR rrx)
		L = (MTDRM_HDCP2_CALLPRIME_T *)malloc(sizeof(MTDRM_HDCP2_CALLPRIME_T));
		if(!L)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc L failed!\n");
			return (HDCP2X_ERR_OUT_OF_MEM);
		}

		memset(L, 0, sizeof(MTDRM_HDCP2_CALLPRIME_T));
		memcpy(L->au1rRn, txInfo_p->rn, HDCP2X_RN_LEN);
		memcpy(L->au1rRx, txInfo_p->rrx, HDCP2X_RRX_LEN);


		if(HDCP2_2 == txInfo_p->run_in_hdcp)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "new LC compute \n");		
			ret = MTDRM_HDCP2_2_ComputeLprime(L);		
		}
		else
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "old LC compute \n"); 	
			ret = MTDRM_HDCP2_ComputeLprime(L);
		}


		if( ret != MTR_OK)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : Compute L failed!\n",__FUNCTION__);
			
			if(L)
			{
				free(L); 
				L = NULL;
			}	
			
			return (HDCP2X_ERR_COMPUTE_LPRIME);
		}



		memcpy(txInfo_p->L_prime, L->au1rLprime, HDCP2X_L_PRIME_LEN);
		//DumpHex("Local L:", L->au1rLprime, HDCP2X_L_PRIME_LEN);
		
		if(L)
		{
			free(L); 
			L = NULL;
		}

		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_RTT_READY, NULL);

        postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_RTT_READY, HDCP2X_TX_WAIT_RTT_READY_US);


	}
	else
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "LC Precompute Disable\n");
		
        hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_L_PRIME, NULL);
        postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_L_PRIME, HDCP2X_TX_WAIT_L_PRIME_US);

	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return (HDCP2X_OK);

}

int WFDHDCPTx::hdcp2x_tx_fsmWaitHPrime(unsigned char *data, int length)
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	MSG_AKE_SEND_H_PRIME_T *H_Prime = NULL;
    unsigned char *pairing_info_p = NULL;
	
	MTDRM_HDCP2_CALHPRIME_T *H = NULL;
	int err = -1;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	
	H_Prime = (MSG_AKE_SEND_H_PRIME_T *)data;
	if(H_Prime->msg_id != HDCP2X_MSGID_AKE_SEND_H_PRIME)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : Incorrect message (expected = %d recv = %d)\n",
			__FUNCTION__, HDCP2X_MSGID_AKE_SEND_H_PRIME, H_Prime->msg_id);
		
		return (HDCP2X_ERR_INV_RX_MSG);
	}

    if (length != HDCP2X_MSGLEN_AKE_SEND_H_PRIME)
    {
        if (length < HDCP2X_MSGLEN_AKE_SEND_H_PRIME)
        {
            HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) :Error incorrect length (expected = %d recv=%d)\n",
                       __FUNCTION__, HDCP2X_MSGLEN_AKE_SEND_H_PRIME, length);
			
            return (HDCP2X_ERR_INV_RX_MSG);
        }
        else
        {
            /*   Note:
	                    It could happen that HPrime and PairingInfo comes in the same TCP packet
	                    if Rx sends too fast. We need to handle them separately.
            		*/
            HDCP2X_DBG(HDCP2XDBG_DBG, "Received Pairing Info combined\n");
            pairing_info_p = (data + HDCP2X_MSGLEN_AKE_SEND_H_PRIME);
        }
    }


	if(mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_H_PRIME])
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "TimeOut ! => do not receive AKE_Send_H_prime in time!\n");
		if(H) {
			free(H);
			H = NULL;
		}
		mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_H_PRIME] = false;
		
		return (HDCP2X_ERR_TIMEOUT_WAIT_H_PRIME);
	}

	cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_H_PRIME);



	memcpy(txInfo_p->h_prime, H_Prime->h_prime, HDCP2X_H_PRIME_LEN);
//    HDCP2X_DBG(HDCP2XDBG_VERB, "Dumping received H'\n");
    DumpHex("Received H':", txInfo_p->h_prime, sizeof(txInfo_p->h_prime));
 
	HDCP2X_DBG(HDCP2XDBG_ERR, "[2.%d] KdKeyDerivation \n", txInfo_p->run_in_hdcp);


	
	//compute kd = dkey0 || dkey1
	if(0x0 == txInfo_p->run_in_hdcp || 0x1 ==  txInfo_p->run_in_hdcp)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "old kd compute\n");
		err = MTDRM_HDCP2_KdKeyDerivation(txInfo_p->rtx);
	}
	else
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "new kd compute:\n");
				
		HDCP2X_DBG(HDCP2XDBG_VERB, "Dumping	rtx:\n");
		DumpHex("rtx",txInfo_p->rtx, HDCP2X_RRX_LEN);
		
		
		HDCP2X_DBG(HDCP2XDBG_VERB, "Dumping	rrx:\n");
		DumpHex("rrx",txInfo_p->rrx, HDCP2X_RTX_LEN);

		err = MTDRM_HDCP2_2_KdKeyDerivation(txInfo_p->rtx, txInfo_p->rrx);

	}

		if (err != MTR_OK)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "%s: Failed to derive Kd err = %d!\n", __FUNCTION__, err);
			return (HDCP2X_ERR_COMPUTE_KD);
		}


		H = (MTDRM_HDCP2_CALHPRIME_T *)malloc(sizeof(MTDRM_HDCP2_CALHPRIME_T));
		if(!H)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc H failed!\n");
			return (HDCP2X_ERR_OUT_OF_MEM);
		}
	
		memset(H, 0, sizeof(MTDRM_HDCP2_CALHPRIME_T));
		H->u1Repeater = txInfo_p->repeater;
		memcpy(H->au1rTx, txInfo_p->rtx, HDCP2X_RTX_LEN);

		HDCP2X_DBG(HDCP2XDBG_ERR, "rx_protol_descriptor:%02x\n",	txInfo_p->rx_protol_descriptor);


	if(	0x0 != txInfo_p->run_in_hdcp &&
		0X1 != txInfo_p->run_in_hdcp &&
		0x1 == txInfo_p->rx_protol_descriptor)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "new Hprime compute\n");

		H->rx_version  =  txInfo_p->run_in_hdcp;
		memcpy(H->rx_cap_mask, txInfo_p->rx_cap_mask, HDCP2X_LEN_CAP_MASK);
		
		H->tx_version  =  txInfo_p->run_in_hdcp;
		memcpy(H->tx_cap_mask, txInfo_p->tx_cap_mask, HDCP2X_LEN_CAP_MASK);
		
		err = MTDRM_HDCP2_2_ComputeHprime(H); 
	}
	else
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "old Hprime compute\n");
		err = MTDRM_HDCP2_ComputeHprime(H);
	}


	if (err != MTR_OK)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : Compute H failed err = %d !\n",__FUNCTION__, err);
		if(H) {
			free(H);
		}
		return (HDCP2X_ERR_COMPUTE_HPRIME);
	}

#ifdef WFD_USE_HDCP_FACSIMILE_KEY
    memcpy(H->au1rHprime, facskey_hprime, sizeof(H->au1rHprime));
#endif

	//compare h and h'
	if(memcmp(H->au1rHprime, H_Prime->h_prime, HDCP2X_H_PRIME_LEN) != 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "H does not equal to H_Prime!\n");

    	//DumpHex("Local H:", H->au1rHprime, HDCP2X_H_PRIME_LEN);	
		
		if(H) 
		{
			free(H);
			H = NULL;
		}
		return -1;
	}
    else
	{
		HDCP2X_DBG(HDCP2XDBG_DBG, "H' compare OK!\n");
	}



	//need to check if stored the km
	if(H)
	{
		free(H);
		H = NULL;
	}
	
	if(no_stored)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "no_stored km need to do pairing!\n");
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_PAIRING_INFO,NULL);
        if (pairing_info_p)
            hdcp2x_tx_fsmWaitPairingInfo(pairing_info_p, (length-HDCP2X_MSGLEN_AKE_SEND_H_PRIME));
		
		return 0;
	}

    //mLC_Try_Count = DEFAULT_TX_LC_RETRY_SEND_COUNT;
	while(mLC_Try_Count--)
	{
		if(hdcp2x_tx_sendLc() < 0)
		{
			//HDCP2X_DBG(HDCP2XDBG_ERR, "send LC failed!\n");
			continue;
		}
		else
		{
			//HDCP2X_DBG(HDCP2XDBG_ERR, "send LC successful!\n");
			//hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_L_PRIME, NULL);
			return 0;
		}	
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "LC init failed!\n");
	return -1;

}

int WFDHDCPTx::hdcp2x_tx_fsmWaitPairingInfo(unsigned char *data, int length)
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	MSG_AKE_SEND_PAIRING_INFO_T *pairingInfo;

	unsigned char *m = NULL;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	pairingInfo = (MSG_AKE_SEND_PAIRING_INFO_T *)data;
	if(pairingInfo->msg_id != HDCP2X_MSGID_AKE_SEND_PAIRING_INFO || length != HDCP2X_MSGLEN_AKE_SEND_PAIRING_INFO)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect message (expected = %d recv = %d)\n",
			__FUNCTION__, HDCP2X_MSGID_AKE_SEND_PAIRING_INFO, pairingInfo->msg_id);
		return (HDCP2X_ERR_INV_RX_MSG);
	}


    if(mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_PAIRING_INFO])
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "TimeOut ! => Wait AKE_Send_Pairing_Info !");
        mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_PAIRING_INFO] = false;
        return (HDCP2X_ERR_TIMEOUT_WAIT_PAIRING_INFO);
    }

    cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_PAIRING_INFO);

	

	memcpy(txInfo_p->ekh_km, pairingInfo->Ekh_km, HDCP2X_EKH_KM_LEN);

	m = (unsigned char *)malloc(HDCP2X_M_LEN);
	if(!m)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc m failed!\n");
		return (HDCP2X_ERR_OUT_OF_MEM);
	}
	memset(m, 0, HDCP2X_M_LEN);
	memcpy(m, txInfo_p->rtx, HDCP2X_RTX_LEN);
#if 0
	//if(MTDRM_HDCP2_PairingInfo(txInfo_p->cert.ID, txInfo_p->km, txInfo_p->ekh_km, m) < 0)
	if(MTDRM_HDCP2_PairingInfo(txInfo_p->cert.ID, txInfo_p->ekh_km, m) < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Store pairing info error!\n");
		if(m) {
			free(m);
			m = NULL;
		}
		return (HDCP2X_ERR_STORE_PAIR_INFO) ;
	}
#endif
	if(m) 
	{
		free(m);
		m = NULL;
	}
	
    //mLC_Try_Count = DEFAULT_TX_LC_RETRY_SEND_COUNT;
	while(mLC_Try_Count--)
	{
		if(hdcp2x_tx_sendLc() < 0)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "send LC failed!\n");
			continue;
		}
		else
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "send LC successful!\n");
			//hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_L_PRIME, NULL);
			return (HDCP2X_OK);
		}	
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "LC init failed!\n");

	return (HDCP2X_ERR_SEND_LC_INIT);


}

int WFDHDCPTx::hdcp2x_tx_sendEks()
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	int ret;

	MSG_SKE_SEND_EKS_T *eks;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	eks = (MSG_SKE_SEND_EKS_T *)malloc(HDCP2X_MSGLEN_SKE_SEND_EKS);
	if(!eks)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc eks failed!\n");
		return -1;
	}




	//generate ks and riv
	if(hdcp2x_tx_genRiv() < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR,"Generate Riv failed!\n");
		return HDCP2X_ERR_GEN_RIV;
	}
	
	if(hdcp2x_tx_genKs() < 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Generate Ks failed!\n");
		return HDCP2X_ERR_GEN_KS;
	}

	//derivation dkey2
	//Computes 128-bit Edkey(ks) = ks XOR (dkey2 XOR rrx)
	unsigned char ks_tmp[HDCP2X_KS_LEN];
	memcpy(ks_tmp, txInfo_p->ks, HDCP2X_KS_LEN);
#if 0
	 if (MTDRM_HDCP2_EncryptKs(ks_tmp, 
                               txInfo_p->rtx, txInfo_p->rrx, 
                               txInfo_p->rn) != 0)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "%s: Failed to encrypt ks\n", __FUNCTION__);
        return (HDCP2X_ERR_ENCRYPT_KS);
    }
#endif
    memcpy(txInfo_p->edkey_ks, ks_tmp, sizeof(txInfo_p->edkey_ks));



	// init eks msg
	eks->msg_id = HDCP2X_MSGID_SKE_SEND_EKS;

#if defined (WFD_USE_HDCP_FACSIMILE_KEY)
		memcpy(eks->Edkey_ks, facskey_eks, HDCP2X_EDKEY_KS_LEN);
		memcpy(eks->riv, txInfo_p->riv, HDCP2X_RIV_LEN);
#elif defined (WFD_HDCP_USE_TEST_VECTOR)
		memcpy(eks->Edkey_ks, TestkeypEks, HDCP2X_EDKEY_KS_LEN);
		memcpy(eks->riv, TestkeypRn, HDCP2X_RIV_LEN);
#else
		memcpy(eks->Edkey_ks, txInfo_p->edkey_ks, HDCP2X_EDKEY_KS_LEN);
		memcpy(eks->riv, txInfo_p->riv, HDCP2X_RIV_LEN);
#endif
	

	ret = sendRequest((unsigned char *)eks, HDCP2X_MSGLEN_SKE_SEND_EKS);
	if(ret <= 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "send eks failed!\n");
        if (eks) 
		{
			free(eks);
			eks = NULL;
		}
		return (HDCP2X_ERR_SEND_EKS);
	}


	
    if (eks) 
	{
		eks = NULL;
		free(eks);	
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);
	return (HDCP2X_OK);
}

int WFDHDCPTx::hdcp2x_tx_genRiv()
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	int rand1, rand2;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	rand1 = hdcp2x_cmmGetRandom();
	rand2 = hdcp2x_cmmGetRandom();

	memcpy(txInfo_p->riv, &rand1, sizeof(int));
	memcpy(txInfo_p->riv+4, &rand2, sizeof(int));

#ifdef WFD_USE_HDCP_FACSIMILE_KEY
    memcpy(txInfo_p->riv, facskey_riv, sizeof(txInfo_p->riv));
#endif

    DumpHex("riv:", txInfo_p->riv, HDCP2X_RIV_LEN);


	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return 0;
}

int WFDHDCPTx::hdcp2x_tx_genKs()
{

	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	int rand1,rand2,rand3,rand4;
    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);

	rand1 = hdcp2x_cmmGetRandom();
	rand2 = hdcp2x_cmmGetRandom();
	rand3 = hdcp2x_cmmGetRandom();
	rand4 = hdcp2x_cmmGetRandom();

	memcpy(txInfo_p->ks, &rand1, sizeof(int));
	memcpy(txInfo_p->ks+4, &rand2, sizeof(int));
	memcpy(txInfo_p->ks+8, &rand3, sizeof(int));
	memcpy(txInfo_p->ks+12, &rand4, sizeof(int));

#ifdef WFD_USE_HDCP_FACSIMILE_KEY
    memcpy(txInfo_p->ks, facskey_ks, sizeof(txInfo_p->ks));
#endif

	//HDCP2X_DBG(HDCP2XDBG_VERB, "Dumping ks\n");
	DumpHex("KS:", txInfo_p->ks, sizeof(txInfo_p->ks));
	//HDCP2X_DBG(HDCP2XDBG_VERB, "ks=[%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x]\n",
        //       txInfo_p->ks[0], txInfo_p->ks[1], txInfo_p->ks[2], txInfo_p->ks[3], 
        //       txInfo_p->ks[4], txInfo_p->ks[5], txInfo_p->ks[6], txInfo_p->ks[7],
        //       txInfo_p->ks[8], txInfo_p->ks[9], txInfo_p->ks[10], txInfo_p->ks[11],
        //       txInfo_p->ks[12], txInfo_p->ks[13], txInfo_p->ks[14], txInfo_p->ks[15]);


	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);	
    return 0;
}
int WFDHDCPTx::hdcp2x_tx_fsmWaitLPrime(unsigned char *data, int length)
{

	int failed = 0;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	MSG_LC_SEND_L_PRIME_T *L_Prime;


	MTDRM_HDCP2_CALLPRIME_T *L = NULL;

	L_Prime = (MSG_LC_SEND_L_PRIME_T *)data;


    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);

	//signal(SIGALRM, SIG_IGN);
    //hdcp2x_tx_cleanTimeout();


	if(L_Prime->msg_id != HDCP2X_MSGID_LC_SEND_L_PRIME || length != HDCP2X_MSGLEN_LC_SEND_L_PRIME)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect message (expected = %d recv = %d)\n",
			__FUNCTION__, HDCP2X_MSGID_LC_INIT, L_Prime->msg_id);
		return (HDCP2X_ERR_INV_RX_MSG);
	}

	memcpy(txInfo_p->L_prime, L_Prime->L_prime, HDCP2X_L_PRIME_LEN);
//    HDCP2X_DBG(HDCP2XDBG_VERB, "Dumping received L'!\n");
    DumpHex("Received L':", txInfo_p->L_prime, HDCP2X_L_PRIME_LEN);

	//compute L = HMAC-SHA256(rn , kd XOR rrx)
	L = (MTDRM_HDCP2_CALLPRIME_T *)malloc(sizeof(MTDRM_HDCP2_CALLPRIME_T));
	if(!L)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc L failed!\n");
		return (HDCP2X_ERR_OUT_OF_MEM);
	}

	//compare L = L' ?
	memset(L, 0, sizeof(MTDRM_HDCP2_CALLPRIME_T));
	memcpy(L->au1rRn, txInfo_p->rn, HDCP2X_RN_LEN);
	memcpy(L->au1rRx, txInfo_p->rrx, HDCP2X_RRX_LEN);
	if(MTDRM_HDCP2_ComputeLprime(L) != 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : Compute L failed!\n",__FUNCTION__);
		failed = 1;
		goto _FAILED;
	}

#ifdef WFD_USE_HDCP_FACSIMILE_KEY
    memcpy(L->au1rLprime, facskey_lprime, sizeof(L->au1rLprime));
#endif

	if(memcmp(L->au1rLprime, L_Prime->L_prime, HDCP2X_L_PRIME_LEN) != 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "L does not equal to L_Prime!\n");
		goto _FAILED;
	}
    else
    {
        HDCP2X_DBG(HDCP2XDBG_DBG, "L' compare OK!\n");
    }
	
_FAILED:
    if (L)
        free(L);
	if(failed || timeout)
	{
	    //mLC_Try_Count = DEFAULT_TX_LC_RETRY_SEND_COUNT;
		while(mLC_Try_Count--)
		{
			if(hdcp2x_tx_sendLc() < 0)
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "send LC failed!\n");
				continue;
			}
			else
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "send LC successful!\n");
				//hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_L_PRIME, NULL);
				return (HDCP2X_OK);
			}	
		}
		HDCP2X_DBG(HDCP2XDBG_ERR, "LC init failed!\n");
		return (HDCP2X_ERR_SEND_LC_INIT);
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "Locality Check OK!\n");


	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return HDCP2X_OK;
}

int WFDHDCPTx::hdcp2x_tx_fsmWaitReceiverIDList(unsigned char *data, int length)
{

#if 0
	//clear the timer
    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	if(timeout)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "did no receive rx list in 3s!\n");
		return;
	}

	//signal(SIGALRM, SIG_DFL);
    hdcp2x_tx_cleanTimeout();

	//HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	MSG_REPEATERAUTH_SEND_RECEIVERID_LIST_T *rxID_List;

	rxID_List = (MSG_REPEATERAUTH_SEND_RECEIVERID_LIST_T *)data;
	if(rxID_List->msg_id != HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST 
		       || length != HDCP2X_MSGLEN_REPEATERAUTH_SEND_RECEIVERID_LIST)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect message (expected = %d recv = %d)\n",
			__FUNCTION__, HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST, rxID_List->msg_id);
		return;
	}

	if(!rxID_List->MAX_DEVS_EXCEEDED && !rxID_List->MAX_CASCADE_EXCEEDED)
	{
		//compute V and compare V and V'
		
		HDCP2X_DBG(HDCP2XDBG_ERR, "Authentication completed!\n");
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATED, NULL);
		return;
	}

#endif
	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

    return (HDCP2X_OK);
		
}




int WFDHDCPTx::hdcp2x_tx_fsmHandleRxInfo(unsigned char *data, int length)
{
	int err;

    MSG_AKE_RECEIVER_INFO *ake_rx_info;
	
    HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	
    HDCP2X_DBG(HDCP2XDBG_ERR, "Handle RxInfo:\n");
	
    ake_rx_info = (MSG_AKE_RECEIVER_INFO *)data;

	if(!data || !length)
	{
        HDCP2X_DBG(HDCP2XDBG_ERR, "%s : incorrect message (data = %p length = %d)\n",
        __FUNCTION__, data, length);		
		return (HDCP2X_ERR_INV_RX_MSG);
	}
	

    if(ake_rx_info->msg_id != HDCP2X_MSGID_AKE_RECEIVER_INFO)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect msg id(expected = %d recv = %d)\n",
            __FUNCTION__, HDCP2X_MSGID_AKE_RECEIVER_INFO, ake_rx_info->msg_id);
		
        return (HDCP2X_ERR_INV_RX_MSG);
    }



    if(length < HDCP2X_MSGLEN_AKE_RECEIVER_INFO)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect msg len(expected = %d recv = %d)\n",
            __FUNCTION__, HDCP2X_MSGLEN_AKE_RECEIVER_INFO, length);
		
        return (HDCP2X_ERR_INV_RX_MSG);
    }



/*
	only support HDCP2.1 now.

	if(HDCP2_1 == ake_rx_info->version)
	{
		txInfo_p->run_in_hdcp = HDCP2_1;
	}
	else(HDCP2_1 < ake_rx_info->version)
	{
		txInfo_p->run_in_hdcp = HDCP2_2;
	}

*/

	if(ake_rx_info->version >= txInfo_p->max_support_hdcp_version)
	{
		txInfo_p->run_in_hdcp = txInfo_p->max_support_hdcp_version;
	}
	else
	{
		txInfo_p->run_in_hdcp = ake_rx_info->version;
	}


	HDCP2X_DBG(HDCP2XDBG_ERR, "RX support [HDCP2.%d] \n", ake_rx_info->version);
	HDCP2X_DBG(HDCP2XDBG_ERR, "TX support [HDCP2.%d] \n", txInfo_p->max_support_hdcp_version);
	HDCP2X_DBG(HDCP2XDBG_ERR, "Run in [HDCP2.%d] MODE\n", txInfo_p->run_in_hdcp);

	/* cancel the timer event */
    cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_INFO);

    mRxPrecomputeLSupport = (ake_rx_info->receiver_capability_mask[1] & 1) ? true : false;

	/* save rx cap mask */
	memcpy(txInfo_p->rx_cap_mask, ake_rx_info->receiver_capability_mask, HDCP2X_LEN_CAP_MASK);
	
    HDCP2X_DBG(HDCP2XDBG_ERR, "Rx 2.%d Pre-compute L support = %d", ake_rx_info->version, mRxPrecomputeLSupport);

    
	err = hdcp2x_tx_sendKm();
	if (err != HDCP2X_OK)
	{
   	    //HDCP2X_DBG(HDCP2XDBG_ERR, "KM SEND ERR\n");
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
	}

	//HDCP2X_DBG(HDCP2XDBG_ERR, "KM SEND finished return(%d)\n", err);

	return err;

}



int WFDHDCPTx::hdcp2x_tx_sendKm()
{
    int ret;
    HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_FUNC_LOG();


	// not supported now.
    // check if stored km according to the receiver id

#if 0
    if(MTDRM_HDCP2_CheckRxID(txInfo_p->cert.ID) == MTR_OK)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "Pairing info found\n");
        no_stored = 0;
    }
    else
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "No Pairing info\n");
        no_stored = 1;
    }
#endif
    if(no_stored)
    {
        if((ret = hdcp2x_tx_sendNoStoredKm()) != HDCP2X_OK)
        {
            return (ret);
        }

        postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_H_PRIME, HDCP2X_TX_WAIT_NO_STORED_H_PRIME_US);


    }
    else
    {
        if((ret = hdcp2x_tx_sendStoredKm()) != HDCP2X_OK)
        {
            return (ret);
        }

        postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_H_PRIME, HDCP2X_TX_WAIT_STORED_H_PRIME_US);

    }

    HDCP2X_DBG(HDCP2XDBG_ERR, "send km successful!\n");
    hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_RRX, NULL);

	/* SRAM CHECK*/
	/*
		ret = hdcp2x_tx_revocation_check(txInfo_p->cert.ID);
		if(0 != ret)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "revocation check fail\n")
			return HDCP2X_ERR_REVOCATION_VERIFY;
		}

		HDCP2X_DBG(HDCP2XDBG_ERR, "revocation check ok\n")
	*/
	
    return (HDCP2X_OK);
}




int WFDHDCPTx::hdcp2x_tx_fsmHandleRxRTTReady(unsigned char *data, int length)
{
    int err;
    MSG_RTT_READY *rtt_ready;
    MSG_RTT_CHALLENGE *rtt_challenge;
    
    HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_FUNC_LOG();

    rtt_ready = (MSG_RTT_READY *)data;

    if ((rtt_ready->msg_id != HDCP2X_MSGID_RTT_READY) || length < HDCP2X_MSGLEN_RTT_READY)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : Incorrect message (expected = %d recv = %d)\n",
            __FUNCTION__, HDCP2X_MSGID_RTT_READY, rtt_ready->msg_id);
        return (HDCP2X_ERR_INV_RX_MSG);
    }


    if(mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_RTT_READY])
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "TimeOut ! => do not receive RTT_Ready in time!\n");
        mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_RTT_READY] = false;
        return (HDCP2X_ERR_TIMEOUT_WAIT_RTT_READY);
    }

    cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_RTT_READY);

    // send RTT_Challenge

    rtt_challenge = (MSG_RTT_CHALLENGE *)malloc(HDCP2X_MSGLEN_RTT_CHALLENGE);

    if(!rtt_challenge)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc rtt_challenge failed!\n");
        return (HDCP2X_ERR_OUT_OF_MEM);
    }


    rtt_challenge->msg_id = HDCP2X_MSGID_RTT_CHALLENGE;

    // send least 128 bits of L
    memcpy(rtt_challenge->L, txInfo_p->L_prime + HDCP2X_L_PRECOMPUTE_LEN, HDCP2X_L_PRECOMPUTE_LEN);


	DumpHex("send L(least 128bit:)", txInfo_p->L_prime + HDCP2X_L_PRECOMPUTE_LEN, HDCP2X_L_PRECOMPUTE_LEN);

    HDCP2X_DBG(HDCP2XDBG_VERB, " send [16.RTT_Challenge] --> ");
    err = sendRequest((unsigned char *)rtt_challenge, HDCP2X_MSGLEN_RTT_CHALLENGE);

    if(rtt_challenge)
    {
        free(rtt_challenge);
        rtt_challenge = NULL;
    }

    if(err <= 0)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "send rtt_challenge failed! %d\n", err);
        return (HDCP2X_ERR_SEND_RTT_CHALLENGE);
    }

    hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_L_PRIME, NULL);
    postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_L_PRIME, HDCP2X_TX_WAIT_L_PRIME_US);

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);	
    return (HDCP2X_OK);


}



int WFDHDCPTx::hdcp2x_tx_fsmHandleRxPrecomputeLprime(unsigned char *data, int length)
{

	int failed = 0;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	MSG_LC_SEND_L_PRIME2_T *L_Prime = NULL;

	L_Prime = (MSG_LC_SEND_L_PRIME2_T *)data;


    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	


	
	if(L_Prime->msg_id != HDCP2X_MSGID_LC_SEND_L_PRIME || length < HDCP2X_MSGLEN_LC_SEND_L_PRECOMPUTE_PRIME)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect message (expected = %d recv = %d)\n",
			__FUNCTION__, HDCP2X_MSGID_LC_INIT, L_Prime->msg_id);
		return (HDCP2X_ERR_INV_RX_MSG);
	}

#ifdef WFD_USE_HDCP_FACSIMILE_KEY

    memcpy(L->au1rLprime, facskey_lprime, sizeof(L->au1rLprime));
#endif


	
	DumpHex("Received L(most 128bit)':", L_Prime->L_prime, HDCP2X_L_PRECOMPUTE_LEN);


	if(memcmp(txInfo_p->L_prime, L_Prime->L_prime, HDCP2X_L_PRECOMPUTE_LEN) != 0)
	{
				HDCP2X_DBG(HDCP2XDBG_ERR, "L does not equal to L_Prime!\n");



				goto _FAILED;
	}
    else
    {
        HDCP2X_DBG(HDCP2XDBG_DBG, "L' compare OK!\n");
    }
	
_FAILED:

	
	
	if(failed || timeout)
	{

			    //mLC_Try_Count = DEFAULT_TX_LC_RETRY_SEND_COUNT;
		while(mLC_Try_Count--)
		{
			if((failed = hdcp2x_tx_sendLc()) != HDCP2X_OK)
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "send LC failed!\n");
				continue;
			}
			else
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "send LC successful!\n");
				//hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_L_PRIME, NULL);
				return (HDCP2X_OK);
			}	
		}
		HDCP2X_DBG(HDCP2XDBG_ERR, "LC init failed!\n");
		return (HDCP2X_ERR_SEND_LC_INIT);
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "Locality Check OK!\n");
	
	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return 0;
}




int WFDHDCPTx::setVideoStreamInfo(int videoCtr, int videoType, int videoContentStreamID)
{
	mVideoCtr = videoCtr;
	mVideoType = videoType;
	mVideoContentStreamID = videoContentStreamID;

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return 0;
}


int WFDHDCPTx::setAudioStreamInfo(int audioCtr, int audioType, int audioContentStreamID)
{
	mAudioCtr = audioCtr;
	mAudioType = audioType;
	mAudioContentStreamID = audioContentStreamID;

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return 0;
}







int WFDHDCPTx::hdcp2x_tx_send_RepeaterAuth_Stream_Manage()
{
	int ret = -1, i = 0;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	unsigned char *stream_type = NULL;
	
	long sec = 0; 
	long usec = 0;

	
	MSG_RepeaterAuth_Stream_Manage_T *stream_manage_msg = NULL;
	stream_manage_msg = (MSG_RepeaterAuth_Stream_Manage_T *)malloc(HDCP2X_MAX_STREAM_COUNT * HDCP2X_M_STREAM_TYPE_LEN + HDCP2X_MSGLEN_RepeaterAuth_Stream_Manage_T);
	if(NULL ==  stream_manage_msg)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "stream_manage_msg allocation failed!\n");
		return -1;
	}


	memset(stream_manage_msg, 0, sizeof(MSG_RepeaterAuth_Stream_Manage_T));
	
	stream_manage_msg->msg_id = HDCP2X_MSGID_REPEATERAUTH_STREAM_MANAGE;
	stream_manage_msg->k[0] = 0;
	stream_manage_msg->k[1] = 0;

	stream_type = (unsigned char *)stream_manage_msg + HDCP2X_MSGLEN_RepeaterAuth_Stream_Manage_T;

	if(mVideoCtr != 0)
	{
		/* video stream */
		*((unsigned int *)stream_type)		  = mVideoCtr;		        //  streamCtr	
		*((unsigned short *)(stream_type+4))  = mVideoContentStreamID;  //  ContentStreamID
		*((unsigned char *)(stream_type+6))   = mVideoType; 			//  Type

		stream_manage_msg->k[1] ++;		
	}

	if(mAudioCtr != 0)
	{
		/* audio stream */
		stream_type += HDCP2X_M_STREAM_TYPE_LEN;

		*((unsigned int *)stream_type)		  = mAudioCtr;		        //  streamCtr	
		*((unsigned short *)(stream_type+4))  = mAudioContentStreamID;  //  ContentStreamID
		*((unsigned char *)(stream_type+6))   = mAudioType; 	

		stream_manage_msg->k[1] ++;	
	}

	mStreamCount = stream_manage_msg->k[1];

	if(0 == mStreamCount)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "stream count:0\n");
		HDCP2X_DBG(HDCP2XDBG_ERR, "ignore RepeaterAuth_Stream_Manage\n");
		HDCP2X_DBG(HDCP2XDBG_ERR, "[HDCP2.0] Authenticated!\n");
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATED, NULL);

			
		ret = -1;
		goto out;
	}

	ret = sendRequest((unsigned char *)stream_manage_msg, mStreamCount*HDCP2X_M_STREAM_TYPE_LEN + HDCP2X_MSGLEN_RepeaterAuth_Stream_Manage_T);
    if(ret <= 0)
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "send HDCP2X_MSGID_REPEATERAUTH_STREAM_MANAGE failed!\n");
		ret = HDCP2X_ERR_SEND_REPEATERAUTH_STREAM_MANAGE;
		goto out;
    }


	postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_STREAM_READY, HDCP2X_TX_WAIT_STREAM_READY_US);
	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_CERT, NULL);



out:

	if(NULL != stream_manage_msg)
	{
		free(stream_manage_msg);
		stream_manage_msg = NULL;
	}


	return ret;

}



int WFDHDCPTx::hdcp2x_tx_fsmWaitReceiverAuthStatus(unsigned char *data, int length)
{
	MSG_Receiver_AuthStatus_T *receiver_auth_status_msg  = NULL;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	receiver_auth_status_msg = (MSG_Receiver_AuthStatus_T *)data;
	if(NULL == receiver_auth_status_msg)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s: err: receiver_auth_status_msg NULL !\n", __FUNCTION__);
		return -1;
	}
	
	if( HDCP2X_MSGID_RECEIVER_AUTHSTATUS != receiver_auth_status_msg->msg_id ||
	     HDCP2X_RECEIVER_AUTH_STATUS_LEN  != receiver_auth_status_msg->LENGTH[1])
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s: err: invalid msg id[%d]  len[%d]  !\n", __FUNCTION__,
			receiver_auth_status_msg->msg_id, receiver_auth_status_msg->LENGTH[1]);

		return -1;

	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "Handle RX  Receiver_AuthStatus msg: REAUTH_REQ[%d]\n", receiver_auth_status_msg->REAUTH_REQ);

	
	if( 0 != receiver_auth_status_msg->REAUTH_REQ)
	{
		/* TO-DO: re ake init*/
		//HDCP2X_RX_DBG(HDCP2XDBG_ERR, "%s:REAUTH_REQ NOT 0: RE_AKE_INIT\n", __FUNCTION__);
		//hdcp2x_tx_disalbe_encryption(0);

		// to-do
		//hdcp2x_tx_ReAKEInit();
	}
	else
	{
		/*
			TO-DO:
		*/
		//HDCP2X_RX_DBG(HDCP2XDBG_ERR, "%s:!! RX REAUTH_REQ 0 !!\n", __FUNCTION__);

		HDCP2X_DBG(HDCP2XDBG_ERR, "[HDCP2.%d] prepared for M'!\n", txInfo_p->run_in_hdcp);
		/* prepared for M' */
		hdcp2x_tx_send_RepeaterAuth_Stream_Manage();
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);
	
	return 0;

}



int WFDHDCPTx::hdcp2x_tx_fsmWaitRepeaterAuthStreamReady(unsigned char *data, int length)
{
	int ret = -1, i = 0;
	
	MSG_REPEATERAUTH_Stream_Ready *rx_ake_send_m_prime = NULL;
	MTDRM_HDCP2_2_CALMPRIME_T *mprime = NULL;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	unsigned char *stream_type = NULL;
	
	/*store the received M'*/
	rx_ake_send_m_prime = (MSG_REPEATERAUTH_Stream_Ready *)data;
	if(HDCP2X_MSGID_REPEATERAUTH_STREAM_READY != rx_ake_send_m_prime->msg_id || 
		length < HDCP2X_MSGLEN_REPEATERAUTH_STREAM_READY)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s:hdcp2x_tx_fsmWaitRepeaterAuthStreamReady: invalid msg id[%d] len[%d]\n", __FUNCTION__,  rx_ake_send_m_prime->msg_id, length);
		
		return -1;
	}

	HDCP2X_DBG(HDCP2XDBG_ERR, "HDCP2.[%d] Handle RepeaterAuthStreamReady msg: streamCount:%d\n", txInfo_p->run_in_hdcp, mStreamCount);


	if(mStreamCount<0 || mStreamCount>2)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "invalid streamCount[%d]\n", mStreamCount);		

		return -1;
	}


	/*cal M' */

	/* malloc mprime */
	mprime = (MTDRM_HDCP2_2_CALMPRIME_T *)malloc(sizeof(MTDRM_HDCP2_2_CALMPRIME_T));
	
	if (!mprime)
	{
		ret = -1;
		goto mprimeout;
	}
	

	memset(mprime, 0, sizeof(MTDRM_HDCP2_2_CALMPRIME_T));

	/* init mprime */
	mprime->STREAMID_TYPE_LEN  = mStreamCount * HDCP2X_M_STREAM_TYPE_LEN;

	/* init seq_num_M */ 

	mprime->seq_num_M[0] 	   = (txInfo_p->seq_num_M>>8)&0xff;		
	mprime->seq_num_M[1] 	   = (txInfo_p->seq_num_M>>4)&0xff;
	mprime->seq_num_M[2] 	   = txInfo_p->seq_num_M&0xff;


	/*init STREAMID_TYPE */ 
	mprime->STREAMID_TYPE = (unsigned char *)malloc(mprime->STREAMID_TYPE_LEN);
	if(NULL == mprime->STREAMID_TYPE)
	{
		ret = -1;
		goto mprimeout;
		
	}

	stream_type = mprime->STREAMID_TYPE;

	if(mVideoCtr != 0)
	{
		/* video stream */
		*((unsigned int *)stream_type)		  = mVideoCtr;		        //  streamCtr	
		*((unsigned short *)(stream_type+4))  = mVideoContentStreamID;  //  ContentStreamID
		*((unsigned char *)(stream_type+6))   = mVideoType; 			//  Type

	}

	if(mAudioCtr != 0)
	{
		/* audio stream */
		stream_type += HDCP2X_M_STREAM_TYPE_LEN;

		*((unsigned int *)stream_type)		  = mAudioCtr;		        //  streamCtr	
		*((unsigned short *)(stream_type+4))  = mAudioContentStreamID;  //  ContentStreamID
		*((unsigned char *)(stream_type+6))   = mAudioType; 	

	}

#if 0
	/* cal mprime*/
	if(MTDRM_HDCP2_2_ComputeMprime(mprime) != 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s: Failed to compute M'\n", __FUNCTION__);
		ret = -1;
		goto mprimeout;
	}
	else
	{
		HDCP2X_DBG(HDCP2XDBG_DBG, "compute M OK\n");
	}
#endif

    DumpHex("recieved M':", rx_ake_send_m_prime->M_prime, HDCP2X_M_PRIME_LEN);


	/*DO COMPARE*/

	//compare M and M'
	if(memcmp(mprime->au1rMprime, rx_ake_send_m_prime->M_prime, HDCP2X_M_PRIME_LEN) != 0)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "M does not equal to M_Prime!\n");
		ret = -1;
		goto mprimeout;

	}
    else
	{
		HDCP2X_DBG(HDCP2XDBG_DBG, "M compare OK!\n");
		
		ret = 0;
    }

	txInfo_p->seq_num_M ++;

	if(HDCP2X_MAX_SEQ_NUM_M < txInfo_p->seq_num_M )
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "WARNING: seq_num_M roll-over!!\n");
		HDCP2X_DBG(HDCP2XDBG_ERR, "!!!TO-DO: RE SEND AKE_Init!!!\n");


		/* DISABLE HDCP TRANSMISION  */
		
		/* TO-DO */
		ret = -1;

		goto mprimeout;

	}


	HDCP2X_DBG(HDCP2XDBG_ERR, "Authenticated!\n");
	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATED, NULL);


	return ret;


mprimeout:

	HDCP2X_DBG(HDCP2XDBG_ERR, "Authentication failed !\n");
	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);

	if(NULL != mprime->STREAMID_TYPE)
	{
		free(mprime->STREAMID_TYPE);
		mprime->STREAMID_TYPE = NULL;
	}


	if (NULL != mprime)
	{
		free(mprime);
		mprime = NULL;
	}


	return ret;

}





int WFDHDCPTx::hdcp2x_tx_fsmHandleRxReceiverIDList(unsigned char *data, int length)
{
    MTDRM_HDCP2_0_CALVPRIME_T *V;
    int err;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);

    if(mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID])
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "TimeOut ! => do not receive RepeaterAuth_Send_ReceiverID_List in %d ms!\n",  HDCP2X_TX_WAIT_RECEIVER_ID_US/1000);
        mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID] = false;
        return (HDCP2X_ERR_TIMEOUT_WAIT_RECEIVER_ID);
    }

    cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID);


	HDCP2X_DBG(HDCP2XDBG_ERR, "!! Vprime[hdcp 2.%d way] \n", txInfo_p->run_in_hdcp);
	if(0 == txInfo_p->run_in_hdcp)
	{
		/* hdcp2.0way */
		err = hdcp2x_tx_2_0_process_IDListMsg(data, length);
	}
	else
	{
		err = hdcp2x_tx_2_2_process_IDListMsg(data, length);
	}

	if(0 != err)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "Authentication failed !\n");
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);

	}


	if(HDCP2X_RE_AKE_INIT == err)
	{
		//hdcp2x_tx_disalbe_encryption(0);
		//hdcp2x_tx_ReAKEInit();
	}
	else if (HDCP2X_ABORT == err)
	{
		//hdcp2x_tx_disalbe_encryption(0);
	}

	
    HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);
    return (HDCP2X_OK);

}






int WFDHDCPTx::hdcp2x_tx_2_0_process_IDListMsg(unsigned char *data, int length)
{
    MTDRM_HDCP2_0_CALVPRIME_T *V;
    int  err;

	HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);


    
    MSG_REPEATERAUTH_2_0_SEND_RECEIVERID_LIST_T *rxID_List;

    rxID_List = (MSG_REPEATERAUTH_2_0_SEND_RECEIVERID_LIST_T *)data;
    if(rxID_List->msg_id != HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST) 
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : incorrect message (expected = %d recv = %d)\n",
        __FUNCTION__, HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST, rxID_List->msg_id);
        return (HDCP2X_ERR_INV_RX_MSG);
    }

#if 0
	if(length < HDCP2X_2_0_IDMSG_MIN_LEN)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "msg len[%d] too short \n", length);
		return (HDCP2X_ERR_INV_RX_MSG);
	}
#endif


	if(length>=3 && (1 == rxID_List->MAX_CASCADE_EXCEEDED || 1 == rxID_List->MAX_DEVS_EXCEEDED))
	{
		
		HDCP2X_DBG(HDCP2XDBG_ERR, "MAX_CASCADE_EXCEEDED OR MAX_DEVS_EXCEEDED\n");
		HDCP2X_DBG(HDCP2XDBG_ERR, "!!!TO-DO: RE SEND AKE_Init!!!\n");

		DumpHex("received id msg:", data, length);

		/* TO-DO */
		err = HDCP2X_RE_AKE_INIT;
		goto vprimeout;		
	}
	else
    {

		if(length < HDCP2X_2_0_IDMSG_MIN_LEN)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "msg len[%d] too short \n", length);
			return (HDCP2X_ERR_INV_RX_MSG);
		}

	
        V = (MTDRM_HDCP2_0_CALVPRIME_T *)malloc(sizeof(MTDRM_HDCP2_0_CALVPRIME_T));
        if(!V)
        {
            HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc V failed!\n");
            return (HDCP2X_ERR_OUT_OF_MEM);
        }
        
        memset(V, 0, sizeof(MTDRM_HDCP2_0_CALVPRIME_T));

        V->id_list               = (unsigned char *)rxID_List->ReceiverID_List;
        V->MAX_DEVS_EXCEEDED     = rxID_List->MAX_DEVS_EXCEEDED;
        V->MAX_CASCADE_EXCEEDED  = rxID_List->MAX_CASCADE_EXCEEDED;
        V->DEVICE_COUNT          = rxID_List->DEVICE_COUNT;
        V->DEPTH                 = rxID_List->DEPTH;
        #if 0
        if ((err = MTDRM_HDCP2_0_ComputeVprime(V)) != MTR_OK)
        {
            HDCP2X_DBG(HDCP2XDBG_ERR, "(%s) : Compute V failed err = %d !\n",__FUNCTION__, err);
            if (V) 
			{
                free(V);
            }                
            return (HDCP2X_ERR_COMPUTE_VPRIME);
        }            
    #endif
        //compare V and V'
        if(memcmp(rxID_List->V_prime, V->au1rVprime, HDCP2X_V_PRIME_LEN) != 0)
        {
            HDCP2X_DBG(HDCP2XDBG_ERR, "V does not equal to V_Prime!\n");
    
            DumpHex("Rx_V'", rxID_List->V_prime, HDCP2X_V_PRIME_LEN);
            DumpHex("Tx_V",  V->au1rVprime, HDCP2X_V_PRIME_LEN);
            
            if(V) 
			{
                free(V);
                V = NULL;
            }
            return (HDCP2X_ERR_NOT_EQUAL_V);
        }
        else
        {
            HDCP2X_DBG(HDCP2XDBG_DBG, "V' compare OK!\n");
        }            

	
        if(V)  {
            free(V);
        }
    }

	


	HDCP2X_DBG(HDCP2XDBG_ERR, "[HDCP2.0] Authenticated!\n");
	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATED, NULL);

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);


vprimeout:
	
    return (err);

}









int WFDHDCPTx::hdcp2x_tx_2_2_process_IDListMsg(unsigned char *data, int len)
{

	MSG_REPEATERAUTH_2_2_SEND_RECEIVERID_LIST_T *ake_rx_v_prime = NULL;
	MTDRM_HDCP2_2_CALVPRIME_T *vprime = NULL;
	int ret = 0;
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

	MSG_REPEATERAUTH_SEND_Ack_T *id_list_ack_msg = NULL;
	
	HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);


	ake_rx_v_prime = (MSG_REPEATERAUTH_2_2_SEND_RECEIVERID_LIST_T *)data;
	if (NULL == ake_rx_v_prime)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s: ake_rx_v_prime NULL\n", __FUNCTION__);
		ret = -1;
		goto vprimeout;
	}

	if( HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST != ake_rx_v_prime->msg_id)		
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "%s: invalid msg id[%d] \n", __FUNCTION__, ake_rx_v_prime->msg_id);
		return (HDCP2X_ERR_INV_RX_MSG);
	}

	
#if 0
	if(len < HDCP2X_2_2_IDMSG_MIN_LEN)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "msg len[%d] too short \n", length);
		return (HDCP2X_ERR_INV_RX_MSG);
	}
#endif



	/* compute V' */
	vprime = (MTDRM_HDCP2_2_CALVPRIME_T *)malloc(sizeof(MTDRM_HDCP2_2_CALVPRIME_T));
	if (!vprime)
	{
		ret = -1;
		goto vprimeout;
	}
	
	memset(vprime, 0, sizeof(MTDRM_HDCP2_2_CALVPRIME_T));


	/* init vprime */
	
	
	vprime->MAX_CASCADE_EXCEEDED = ake_rx_v_prime->MAX_CASCADE_EXCEEDED;
	vprime->MAX_DEVS_EXCEEDED = ake_rx_v_prime->MAX_DEVS_EXCEEDED;

	if(len>=3 && (1 == vprime->MAX_CASCADE_EXCEEDED || 1 == vprime->MAX_DEVS_EXCEEDED))
	{
		
		HDCP2X_DBG(HDCP2XDBG_ERR, "MAX_CASCADE_EXCEEDED OR MAX_DEVS_EXCEEDED\n");
		HDCP2X_DBG(HDCP2XDBG_ERR, "!!!TO-DO: RE SEND AKE_Init!!!\n");

		DumpHex("received id msg:", data, len);

		/* TO-DO */
		ret = HDCP2X_RE_AKE_INIT;
		goto vprimeout;		
	}
	else
	{
		if(len < HDCP2X_2_2_IDMSG_MIN_LEN)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "msg len[%d] too short \n", len);
			ret = HDCP2X_ERR_INV_RX_MSG;
			goto vprimeout;

		}


		vprime->DEVICE_COUNT                = ake_rx_v_prime->DEVICE_COUNT;
		vprime->DEPTH                       = ake_rx_v_prime->DEPTH;
		vprime->HDCP2_0_REPEATER_DOWNSTREAM = ake_rx_v_prime->HDCP2_0_REPEATER_DOWNSTREAM;
		vprime->HDCP1_DEVICE_DOWNSTREAM     = ake_rx_v_prime->HDCP1_DEVICE_DOWNSTREAM;


		vprime->seq_num_V = (ake_rx_v_prime->seq_num_V[0]) || (ake_rx_v_prime->seq_num_V[1]<<8) || (ake_rx_v_prime->seq_num_V[2]<<16);
		HDCP2X_DBG(HDCP2XDBG_ERR, "device count:%d seq_num_V %d\n", vprime->DEVICE_COUNT, vprime->seq_num_V);

		if(HDCP2X_MAX_SEQ_NUM_V < vprime->seq_num_V)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "WARNING: seq_num_V roll-over!!\n");
			
			ret = HDCP2X_RE_AKE_INIT;
			goto vprimeout;
		}

		
		/* init id_list */
		vprime->id_list = (unsigned char *)malloc((vprime->DEVICE_COUNT) * HDCP2X_RECEIVER_ID_LEN * sizeof(unsigned char));
		if (NULL == vprime->id_list)
		{
			ret = -1;
			goto vprimeout;
		}

		memcpy(vprime->id_list, ake_rx_v_prime->ReceiverID_List, (vprime->DEVICE_COUNT) * HDCP2X_RECEIVER_ID_LEN * sizeof(unsigned char));	

#if 0
		if(MTDRM_HDCP2_2_ComputeVprime(vprime) != 0)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "%s: Failed to compute V'\n", __FUNCTION__);
			ret = -1;
			goto vprimeout;
		}
		else
		{
			HDCP2X_DBG(HDCP2XDBG_DBG, "compute V OK\n");
			//HDCP2X_RX_DBG(HDCP2XDBG_VERB, "Dumping computed V'\n");
			//DumpHex("V:", vprime->au1rVprime, HDCP2X_V_PRIME_LEN);
		}
#endif
		//memcpy(ake_rx_v_prime->V_prime_most, (char*)(vprime->au1rVprime), HDCP2X_V_PRIME_LEN>>1);



		//HDCP2X_RX_DBG(HDCP2XDBG_VERB, "Dumping Received V' most: \n");
		//DumpHex(ake_rx_v_prime->V_prime_most, HDCP2X_V_PRIME_LEN>>1);

		/* do compare */
		if(0 != memcmp(ake_rx_v_prime->V_prime_most, (char*)(vprime->au1rVprime), (MTDRM_HDCP2_KEYLEN_VPRIME>>1)))
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "%s: err: Vprime most 128bit not match\n", __FUNCTION__);


			/* authentication faure */
			HDCP2X_DBG(HDCP2XDBG_ERR, "!! authentication faure !!\n");
			
			DumpHex("Received V' most:", ake_rx_v_prime->V_prime_most, HDCP2X_V_PRIME_LEN>>1);

			
			ret = HDCP2X_RE_AKE_INIT;
			goto vprimeout;

		}
		else
		{


		
			HDCP2X_DBG(HDCP2XDBG_ERR, "Vprime most 128bit match \n");


			/* to-do: check the id*/
			HDCP2X_DBG(HDCP2XDBG_DBG, " to-do check revocation list\n");


			//send  Vprime Ack
			id_list_ack_msg = (MSG_REPEATERAUTH_SEND_Ack_T *)malloc(HDCP2X_REPEATER_AUTH_ACK_LEN);
			if(NULL == id_list_ack_msg)
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "%s: id_list_ack_msg malloc err\n", __FUNCTION__);
				ret = -1;
				goto vprimeout;

			}
			memset(id_list_ack_msg, 0, HDCP2X_REPEATER_AUTH_ACK_LEN);


			/* INIT ack msg*/
			id_list_ack_msg->msg_id = HDCP2X_MSGID_REPEATERAUTH_SEND_ACK;

			/* send least 128 bit of Vprime*/
			memcpy(id_list_ack_msg->V_prime_least,  (char*)(vprime->au1rVprime) + (MTDRM_HDCP2_KEYLEN_VPRIME>>1), MTDRM_HDCP2_KEYLEN_VPRIME>>1);


		    ret = sendRequest ((unsigned char *)id_list_ack_msg, HDCP2X_REPEATER_AUTH_ACK_LEN);
			//ret = send(txInfo_p->confd, id_list_ack_msg, HDCP2X_REPEATER_AUTH_ACK_LEN, 0);
			if(ret <= 0)
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "send HDCP2X_MSGID_REPEATERAUTH_SEND_ACK failed!\n");
				ret = -1;
				goto vprimeout;
			}

			
			HDCP2X_DBG(HDCP2XDBG_ERR, "HDCP2X_MSGID_REPEATERAUTH_SEND_ACK Sent\n");
			//txInfo_p->IDMsg_Ack_send = 1;
			//hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_RECEIVER_AUTH_STATUS, NULL);
			
		}
		
		ret = 0;

	}

	


vprimeout:

	if(NULL != vprime&& NULL != vprime->id_list)
	{
		free(vprime->id_list);
		vprime->id_list = NULL;
		}

	if (NULL != vprime)
	{
		free(vprime);
		vprime = NULL;
	}

	if(NULL != id_list_ack_msg)
	{
		free(id_list_ack_msg);
		id_list_ack_msg = NULL;

	}



	return ret;
}












int WFDHDCPTx::hdcp2x_tx_processData(unsigned char *data, int length)
{
	HDCP2X_TX_INFO_T *txInfo_p = NULL;

	int err = HDCP2X_OK;
	
	txInfo_p = hdcp2x_tx_info;
	
    HDCP2X_DBG(HDCP2XDBG_ERR, "\n\n>>> %s msg[%d] len[%d] \n",__FUNCTION__, data[0], length);

	if(!txInfo_p)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "hdcp2x_tx_info not initialized!\n");
		return -1;
	}


	/* be prepared to process the id list message request from RX */
	
	if(1 == txInfo_p->repeater &&  
	   HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST <= txInfo_p->state &&
	   HDCP2X_MSGID_REPEATERAUTH_SEND_RECEIVERID_LIST == data[0])  //verify the rx if repeater
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "msg: HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST received ! \n");
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST, NULL);		
		hdcp2x_tx_fsmHandleRxReceiverIDList(data, length);	
		return 0;		
	}


	/*
		REPEATERAUTH_STREAM_READY Received
	*/
	if(HDCP2X_MSGID_REPEATERAUTH_STREAM_READY == data[0])
	{
		hdcp2x_tx_fsmWaitRepeaterAuthStreamReady(data, length);
		return 0;
	}



	if(HDCP2X_MSGID_RECEIVER_AUTHSTATUS == data[0])
	{
		//txInfo_p->IDMsg_Ack_send = 0;
		hdcp2x_tx_fsmWaitReceiverAuthStatus(data, length);
		return 0;		
	}
	




	

	switch(txInfo_p->state)
	{
		case HDCP2X_TX_STATE_WAIT_CERT:
			err = hdcp2x_tx_fsmWaitCert(data, length);
			break;

		case HDCP2X_TX_STATE_WAIT_RECEIVER_INFO:
			hdcp2x_tx_fsmHandleRxInfo(data, length);
			break;			
			
		case HDCP2X_TX_STATE_WAIT_RRX:
			err = hdcp2x_tx_fsmWaitRrx(data, length);
			break;
			
		case HDCP2X_TX_STATE_WAIT_H_PRIME:
			err = hdcp2x_tx_fsmWaitHPrime(data, length);
			break;
			
		case HDCP2X_TX_STATE_WAIT_PAIRING_INFO:
			err = hdcp2x_tx_fsmWaitPairingInfo(data, length);
			break;

        case HDCP2X_TX_STATE_WAIT_RTT_READY:
            err = hdcp2x_tx_fsmHandleRxRTTReady(data, length);
            break;
			
        case HDCP2X_TX_STATE_WAIT_L_PRIME:

			if(mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_L_PRIME])
			{
				HDCP2X_DBG(HDCP2XDBG_ERR, "TimeOut ! => do not receive LC_Send_L_prime in %d ms!\n",  HDCP2X_TX_WAIT_L_PRIME_US/1000);
				mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_WAIT_L_PRIME] = false;
				timeout = 1;
				err = HDCP2X_ERR_TIMEOUT_WAIT_L_PRIME;
			}
			else
			{
				cancelTimeOutEvent(HDCP2X_TX_TIMEOUT_WAIT_L_PRIME);
			
				
	            if (mRxPrecomputeLSupport)
	                err = hdcp2x_tx_fsmHandleRxPrecomputeLprime(data, length);
	            else
	                err = hdcp2x_tx_fsmWaitLPrime(data, length);


				if(HDCP2X_OK != err)
				{
					/* LC err */
					break;
				}

		
				/* send eks */
				err = hdcp2x_tx_sendEks();
				
				if(HDCP2X_OK != err)
				{	
					/*send eks err*/
					break;
				}

				
				if(txInfo_p->repeater)//verify the rx if repeater
				{
					//set the timer in 3s
					long sec = 0;
					long usec = 0;
				
					HDCP2X_DBG(HDCP2XDBG_ERR, "[RX REPEATER] to receive ID list!\n");
				
					//set the timer in 3s (must receive id list msg within 3s)
					postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID, HDCP2X_TX_WAIT_RECEIVER_ID_US);
					hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST, NULL);
				
				}
				else
				{
					HDCP2X_DBG(HDCP2XDBG_ERR, "[RX NOT REPEATER] Authenticated!\n");
					hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATED, NULL);
				}

				
			}
            break;

			
		case HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST:
			//err = hdcp2x_tx_fsmWaitReceiverIDList(data, length);
			err = hdcp2x_tx_fsmHandleRxReceiverIDList(data, length);
			break;

		default:
			HDCP2X_DBG(HDCP2XDBG_ERR, "!!!WARNING: unknow state !!!\n");
			err = HDCP2X_OK;
			break;
			
	}


    if (err != HDCP2X_OK)
    {
		
		HDCP2X_DBG(HDCP2XDBG_ERR, "msg: hdcp2x_tx_processData process msg(%d) err\n\n\n", data[0]);
        hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
    }
	else
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "msg: hdcp2x_tx_processData process msg(%d) ok\n\n\n", data[0]);

	}

    return (HDCP2X_OK);
}


int WFDHDCPTx::hdcp2x_tx_fsmTransition(void *param)
{
	fd_set fds;
	int maxfd = -1;
	int nfds = -1;
	HDCP2X_TX_INFO_T *txInfo_p = NULL;

	char *recvbuf = NULL;
	int length;

	int reason = -1;
	
	//set the timer in 100ms
	long sec = 0;
	long usec = 100000;



    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	txInfo_p = (HDCP2X_TX_INFO_T *)param;
	if(!txInfo_p)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "hdcp2x_tx_info not initallized!\n");
		return -1;
	}

	/*
		Initiate authentication:AKE_Init
	*/
	reason = hdcp2x_tx_sendAkeInit();
	if(reason != HDCP2X_OK)
	{
		//HDCP2X_DBG(HDCP2XDBG_ERR, "hdcp2x_tx_sendAkeInit error [%x]!\n", reason);
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
		return -1;
	}
	//HDCP2X_DBG(HDCP2XDBG_DBG, "Initiate authentication:AKE_Init OK\n");
	

	/*
		send Transmiterr Info
	*/
	reason = hdcp2x_tx_sendAkeTransmitterInfo();
	if( reason != HDCP2X_OK)
	{
		///HDCP2X_DBG(HDCP2XDBG_ERR, "send tx info err [%x]\n", reason);
		hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AUTHENTICATE_FAIL, NULL);
		return -1;							
	}
	//HDCP2X_DBG(HDCP2XDBG_DBG, "send Transmiterr Info OK\n");


	/*
		set the timer in 100ms (must receive the rx cert and rx info within 100ms after tx info was sent)
	*/
	postTimeOutEvent_l(HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_INFO, HDCP2X_TX_WAIT_RECEIVER_INFO_US);
	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_WAIT_CERT, NULL);
	
	HDCP2X_DBG(HDCP2XDBG_DBG, "RX INFO should be received within 100ms if Rx is not HDCP2.0 \n");

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

    return 0;
}



void WFDHDCPTx::hdcp2x_tx_statePrint(int oldstate, int newstate)
{
    HDCP2X_DBG(HDCP2XDBG_ERR, "TX state change: [%s] -> [%s]\n", 
               hdcp2x_tx_str[oldstate], 
               hdcp2x_tx_str[newstate]);
}

#if 0
int hdcp2x_tx_stateGet(void)
{
    if (!hdcp2x_tx_info)
        return -1;
    else
        return hdcp2x_tx_info->state;
}
#endif


void  WFDHDCPTx::DumpHex(const char *pName, const unsigned char *pData, unsigned int size)
{
    unsigned int i;

    char v[PROPERTY_VALUE_MAX];
    if (property_get("media.stagefright_wfd.hdcp.dump", v, NULL) 
            && (!strcmp(v, "1") ))
    {
        HDCP2X_DBG(HDCP2XDBG_ERR, " --- Dump %s ---", pName);

        hexdump(pData, size);

        HDCP2X_DBG(HDCP2XDBG_ERR, "\n");
    }        
}


int WFDHDCPTx::hdcp2x_tx_fsmChange(int newstate,void *param)
{
	HDCP2X_TX_INFO_T *txInfo_p = NULL;
    unsigned int stateChanged = 0;
	unsigned char encl28[HDCP2X_LC128_LEN] = {0};

	int ret = -1;
	
	txInfo_p = hdcp2x_tx_info;
    //HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	if(!txInfo_p)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "hdcp2x_tx_info not initallized!\n");
		return -1;
	}
    hdcp2x_tx_statePrint(txInfo_p->state, newstate);

    if(txInfo_p->state != newstate) {
        stateChanged = 1;
    }

	switch(newstate)
	{
        case HDCP2X_TX_STATE_DETACHED:
            break;
        case HDCP2X_TX_STATE_WAIT_ATTACHED:
        case HDCP2X_TX_STATE_AKE_INIT:
        case HDCP2X_TX_STATE_WAIT_CERT:
        case HDCP2X_TX_STATE_WAIT_RRX:
        case HDCP2X_TX_STATE_WAIT_H_PRIME:
        case HDCP2X_TX_STATE_WAIT_PAIRING_INFO:
        case HDCP2X_TX_STATE_WAIT_L_PRIME:
        case HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST:
		case HDCP2X_TX_STATE_WAIT_STREAM_READY:
        case HDCP2X_TX_STATE_WAIT_RECEIVER_INFO:
        case HDCP2X_TX_STATE_WAIT_RTT_READY:

		{
			if(txInfo_p->state == newstate)
				return 0;
			//hdcp2x_tx_statePrint(txInfo_p->state, newstate);
			txInfo_p->state = newstate;
			break;
		}
		case HDCP2X_TX_STATE_AUTHENTICATED:

			/* set up ks */
			
			ret = MTDRM_HDCP2_KsXorLC128();
			if(0 == ret)
			{
				HDCP2X_DBG(HDCP2XDBG_VERB, "<-- MTDRM_HDCP2_KsXorLC128 ok\n");

			}
			else
			{
				HDCP2X_DBG(HDCP2XDBG_VERB, "<-- MTDRM_HDCP2_KsXorLC128 failure\n");
			}
		
			
			hdcp2x_tx_statePrint(txInfo_p->state, newstate);
			txInfo_p->state = newstate;
			break;

        case HDCP2X_TX_STATE_AUTHENTICATE_FAIL:

            txInfo_p->state = newstate;
            break;
			
		default:
			break;
	}
	
	if (stateChanged)
	{
		switch (newstate)
		{
			case HDCP2X_TX_STATE_AUTHENTICATED:
				HDCP2X_DBG(HDCP2XDBG_ERR, "-> Notify HDCP_INITIALIZATION_COMPLETE \n");
				mObserverNotify(mCookie, HDCPModule::HDCP_INITIALIZATION_COMPLETE, 0, 0);
				break;

			case HDCP2X_TX_STATE_AUTHENTICATE_FAIL:
				HDCP2X_DBG(HDCP2XDBG_ERR, "-> Notify HDCP_INITIALIZATION_FAILED \n");            
				mObserverNotify(mCookie, HDCPModule::HDCP_INITIALIZATION_FAILED, 0, 0);
				break;

			case HDCP2X_TX_STATE_DETACHED:
				HDCP2X_DBG(HDCP2XDBG_ERR, "-> Notify HDCP_SHUTDOWN_COMPLETE \n");            
				mObserverNotify(mCookie, HDCPModule::HDCP_SHUTDOWN_COMPLETE, 0, 0);
				break;

			default:
				break;

		}
	}
	return 0;
}

int WFDHDCPTx::hdcp2x_tx_InitAuthenticaion()
{
	int txSock;
	
	struct sockaddr_in rxAddr;
	HDCP2X_TX_INFO_T *txInfo_p;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	txInfo_p = hdcp2x_tx_info;
	if(!txInfo_p)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "hdcp2x_tx_info not initallized!\n");
		return -1;
	}

	hdcp2x_tx_fsmChange(HDCP2X_TX_STATE_AKE_INIT,NULL);

	hdcp2x_tx_fsmTransition(hdcp2x_tx_info);

	HDCP2X_DBG(HDCP2XDBG_ERR, "<<< %s ok \n",__FUNCTION__);

	return 0;
}

int WFDHDCPTx::hdcp2x_tx_readKpub()
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;
	extern unsigned char facskey_kpubdcp[];

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);


	//Reserved: now the keypub stored in local array


	HDCP2X_DBG(HDCP2XDBG_ERR, "Tx read kpubdcp ok!\n");

	return 0;
}
void WFDHDCPTx::hdcp2x_tx_fsmcleanup()
{
	HDCP2X_TX_INFO_T *txInfo_p = hdcp2x_tx_info;

    HDCP2X_DBG(HDCP2XDBG_ERR, ">>> %s\n",__FUNCTION__);
	if(!txInfo_p)
		return;

	//if(txInfo_p->confd > 0)
	//	close(txInfo_p->confd);

	txInfo_p->state = HDCP2X_TX_STATE_DETACHED;

	hdcp2x_tx_fsmChange(txInfo_p->state,NULL);

}
int WFDHDCPTx::hdcp2x_tx_init()
{
	int ret = -1;
	
	if (!hdcp2x_tx_info)
	{
	    hdcp2x_tx_info = (HDCP2X_TX_INFO_T *)malloc(sizeof(HDCP2X_TX_INFO_T));
		if(!hdcp2x_tx_info)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "Malloc hdcp2x_tx_info failed!\n");
			return -1;
		}
		memset(hdcp2x_tx_info, 0, sizeof(HDCP2X_TX_INFO_T));

		if(hdcp2x_tx_readKpub() != 0)
		{
			HDCP2X_DBG(HDCP2XDBG_ERR, "Read kpub failed!\n");
			return -1;
		}

		hdcp2x_tx_info->state = HDCP2X_TX_STATE_DETACHED;

		hdcp2x_tx_info->max_support_hdcp_version = HDCP2_2;


		HDCP2X_DBG(HDCP2XDBG_ERR, "HDCP[2.%d]\n", hdcp2x_tx_info->max_support_hdcp_version);

	
	}




	
	return ret;
}

 





int crypt_xor( const uint8_t *s1, const uint8_t *s2, uint8_t *d, int len )
{
   int ii;

   if ( !s1 || !s2 || !d )
   {
      return -1;
   }

   /** @todo: Optimize by using 32 bit-xors if needed. */
   for ( ii = 0 ; ii < len ; ii++ )
   {
      d[ii] = s1[ii] ^ s2[ii];
   }

   return 0;

}










int hdcp2x_tx_disalbe_encryption(int enable)
{
	if(1 == enable)
	{
		HDCP2X_DBG(HDCP2XDBG_ERR, "!!TO-DO:	@@ Enable HDCP Encryption!!!\n");
	}
	else if(0 == enable)
	{

		HDCP2X_DBG(HDCP2XDBG_ERR, "!!TO-DO:	@@ Disable HDCP Encryption!!!\n");
	}



	return 0;
}







}
