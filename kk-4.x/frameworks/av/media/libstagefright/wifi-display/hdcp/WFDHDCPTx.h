
#ifndef _WFD_HDCP_TX_HH
#define _WFD_HDCP_TX_HH

#include "TimedEventQueue.h"
#include "HDCPAPI.h"
#include "ANetworkSession.h"
#include <media/stagefright/foundation/AHandler.h>
#include "HDCP2xDef.h"

//	#include <tomcrypt.h>

#define WFD_HDCP_TX_BUFFER_SIZE 4000
#define UINT_8	char
#define UINT_16	short
#define UINT_32	int

#define HDCP2X_TX_MAX_BUFFSIZE 2048
#define HDCP2X_TX_SELECT_TIMEOUT_SEC 5
#define	HDCP2X_TX_SELECT_TIMEOUT_USEC 0

#define HDCP2X_RXID_LEN 5
#define HDCP2X_PUBLIC_LEN 131
#define HDCP2X_RESERVED_LEN 2
#define HDCP2X_ROOTSIGN_LEN 384

#define HDCP2X_MAX_RX 31

#define HDCP2_2_SUPPORT (1)

#define HDCP2X_TX_MAX_STREAM_NUM (2)

/*-----------------------------
    Externed functions & variables
------------------------------*/


/*-----------------------------
    Structures
------------------------------*/

typedef struct
{
    unsigned char ID[HDCP2X_RXID_LEN];
    unsigned char Public[HDCP2X_PUBLIC_LEN];
	unsigned char ProtocolDescripter;	
    unsigned char Reserved;
    unsigned char RootSign[HDCP2X_ROOTSIGN_LEN];
}HDCP2X_RXCERT_T;






typedef struct HDCP2X_TX_INFO
{
    int state;
    //int port;

	//int confd;

	HDCP2X_RXCERT_T cert;

	unsigned char kdcp[HDCP2X_KPUB_DCP_LEN];

    /* AKE keys */
    unsigned char km[HDCP2X_KM_LEN];
    unsigned char rtx[HDCP2X_RTX_LEN];
    unsigned char rrx[HDCP2X_RRX_LEN];
    unsigned char h_prime[HDCP2X_H_PRIME_LEN];
    unsigned char ekh_km[HDCP2X_EKH_KM_LEN];
	unsigned char m[HDCP2X_M_LEN];
    /* Locality check */
    unsigned char rn[HDCP2X_RN_LEN];
    unsigned char L_prime[HDCP2X_L_PRIME_LEN];
    /* EKS */
    unsigned char lc128[HDCP2X_LC128_LEN];
    unsigned char riv[HDCP2X_RIV_LEN];
    unsigned char ks[HDCP2X_KS_LEN];
    unsigned char edkey_ks[HDCP2X_EDKEY_KS_LEN];



	
	/*RepeaterID List*/
	unsigned char repeater;
	unsigned char v_prime[HDCP2X_V_PRIME_LEN];
	struct Receiver ReceiverID[HDCP2X_MAX_RECEIVER];
	int seq_num_M;
	int stream_type;

	
    /* AKE_Receiver_Info */
    unsigned char rx_cap_mask[HDCP2X_LEN_CAP_MASK];
	unsigned char tx_cap_mask[HDCP2X_LEN_CAP_MASK];


	/* hdcp version mode */
	int max_support_hdcp_version;
	int run_in_hdcp;
	

	int rx_protol_descriptor;



} HDCP2X_TX_INFO_T;
    
enum State{
	HDCP2X_TX_STATE_DETACHED = 0, 
	HDCP2X_TX_STATE_WAIT_ATTACHED, 
	HDCP2X_TX_STATE_AKE_INIT,				
	HDCP2X_TX_STATE_WAIT_CERT,
	HDCP2X_TX_STATE_WAIT_RRX,
	HDCP2X_TX_STATE_WAIT_H_PRIME,
	HDCP2X_TX_STATE_WAIT_PAIRING_INFO,
	HDCP2X_TX_STATE_WAIT_L_PRIME,
	HDCP2X_TX_STATE_WAIT_RECEIVERID_LIST,
	HDCP2X_TX_STATE_WAIT_STREAM_READY,	  
	HDCP2X_TX_STATE_WAIT_RECEIVER_INFO,
	HDCP2X_TX_STATE_WAIT_RTT_READY,
	HDCP2X_TX_STATE_AUTHENTICATED,
	HDCP2X_TX_STATE_AUTHENTICATE_FAIL,
	HDCP2X_TX_STATE_MAX
};


enum TxTimeOutType{
	HDCP2X_TX_TIMEOUT_WAIT_RX_CONNECTION = 0,
	HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_INFO,
	HDCP2X_TX_TIMEOUT_WAIT_H_PRIME,
	HDCP2X_TX_TIMEOUT_WAIT_PAIRING_INFO,
	HDCP2X_TX_TIMEOUT_WAIT_L_PRIME,
	HDCP2X_TX_TIMEOUT_WAIT_RTT_READY,
	HDCP2X_TX_TIMEOUT_WAIT_SHUTDOWN,
	HDCP2X_TX_TIMEOUT_WAIT_AUTH_FAIL,
	HDCP2X_TX_TIMEOUT_WAIT_RECEIVER_ID,    
	HDCP2X_TX_TIMEOUT_WAIT_STREAM_READY,		
	HDCP2X_TX_TIMEOUT_MAX
};




namespace android {

class WFDHDCPTx : public AHandler {


public:
    WFDHDCPTx(void *cookie, HDCPModule::ObserverFunc observerNotify);
    ~WFDHDCPTx();
    static WFDHDCPTx* createNew();

    status_t startAuthentication(const char *host, unsigned port);
    status_t shutdownAsync();
    
    int hdcp2x_tx_fsmStart();
    void onMessageReceived(const sp<AMessage> &msg);

    // Encrypt a data according to the HDCP spec. The data is to be
    // encrypted in-place, only size bytes of data should be read/write,
    // even if the size is not a multiple of 128 bit (16 bytes).
    // This operation is to be synchronous, i.e. this call does not return
    // until outData contains size bytes of encrypted data.
    // streamCTR will be assigned by the caller (to 0 for the first PES stream,
    // 1 for the second and so on)
    // inputCTR will be maintained by the callee for each PES stream.
    status_t encrypt(
            const void *inData, size_t size, uint32_t streamCTR,
            uint64_t *outInputCTR, void *outData);


	int setVideoStreamInfo(int videoCtr, int videoType, int videoContentStreamID);
	int setAudioStreamInfo(int audioCtr, int audioType, int audioContentStreamID);



private:   

    status_t openConnection(const char *host, unsigned port);
    status_t sendRequest(unsigned char *buf, UINT_16 len);	
    status_t shutdownComplete();

    void reset();
	
	int hdcp2x_tx_InitAuthenticaion();

    // handle RX message
    int hdcp2x_tx_processData(unsigned char *data, int length);


    int hdcp2x_tx_genRtx();
    int hdcp2x_tx_sendAkeInit();
    int hdcp2x_tx_fsmWaitRrx(unsigned char *data, int length);
    int hdcp2x_tx_sendStoredKm();
    int hdcp2x_tx_genKm();
    int hdcp2x_tx_sendNoStoredKm();
    int hdcp2x_tx_fsmWaitCert(unsigned char *data, int length);
    int hdcp2x_tx_genRn();
    int hdcp2x_tx_sendLc();
    int hdcp2x_tx_fsmWaitHPrime(unsigned char *data, int length);
    int hdcp2x_tx_fsmWaitPairingInfo(unsigned char *data, int length);
    int hdcp2x_tx_sendEks();
    int hdcp2x_tx_genRiv();
    int hdcp2x_tx_genKs();
    int hdcp2x_tx_fsmWaitLPrime(unsigned char *data, int length);
    int hdcp2x_tx_fsmWaitReceiverIDList(unsigned char *data, int length);

    int hdcp2x_tx_fsmTransition(void *param);
    void hdcp2x_tx_statePrint(int oldstate, int newstate);
    int hdcp2x_tx_fsmChange(int newstate,void *param);
    //int hdcp2x_tx_start();
    int hdcp2x_tx_init();
    int hdcp2x_tx_readKpub();
    void hdcp2x_tx_fsmcleanup();
    int hdcp2x_tx_verifyCert(unsigned char * cert);
	int hdcp2x_tx_sendKm();

	/* for HDCP2.1(2)*/
	int hdcp2x_tx_sendAkeTransmitterInfo();
	int hdcp2x_tx_fsmHandleRxInfo(unsigned char *data, int length);
	int hdcp2x_tx_fsmHandleRxRTTReady(unsigned char *data, int length);
	int hdcp2x_tx_fsmHandleRxPrecomputeLprime(unsigned char *data, int length);
	
	/* timer */
	//static void timeoutTask(WFDHDCPTx *WFDHDCPTxSession);
	//void postTimeOutEvent(long sec, long usec, TxTimeOutType eType);
	//void cancelTimeOutEvent(TxTimeOutType eType);


	/* converter */
	int hdcp2x_tx_revocation_check(unsigned const char *device_id); 
	int hdcp2x_tx_send_RepeaterAuth_Stream_Manage();
	int hdcp2x_tx_fsmWaitReceiverAuthStatus(unsigned char *data, int length);
	int hdcp2x_tx_fsmWaitRepeaterAuthStreamReady(unsigned char *data, int length);
    int hdcp2x_tx_fsmHandleRxReceiverIDList(unsigned char *data, int length);
	int hdcp2x_tx_2_0_process_IDListMsg(unsigned char *data, int len);
	int hdcp2x_tx_2_2_process_IDListMsg(unsigned char *data, int len);


	//int hdcp2x_tx_fsmChange(int newstate,void *param);



    void initAES(uint32_t streamCTR);
  
    void onTimeOutEvent(TxTimeOutType eType);
    void postTimeOutEvent_l(TxTimeOutType eType, int64_t delayUs);
    void cancelTimeOutEvent(TxTimeOutType eType);
    void cancelAllTimeOutEvent();

    void DumpHex(const char *pName, const unsigned char *pData, unsigned int size);


	

protected:	
    enum {
        kWhatANSNotify,      /* ANetworkSession Notify */
    };
    
private:

    HDCP2X_TX_INFO_T *hdcp2x_tx_info;
	
	int mVideoCtr;
	int mVideoType;
	int mVideoContentStreamID;

	int mAudioCtr;
	int mAudioType;
	int mAudioContentStreamID;

	int mStreamCount;
	
    int timeout;
    int no_stored;
    int mLC_Try_Count;

    sp<ANetworkSession> mTxSession;
    sp<ALooper> mTxLooper;
    int32_t mSessionId;

    TimedEventQueue mQueue;
    bool mQueueStarted;

    sp<TimedEventQueue::Event> mTimeOutEvent[HDCP2X_TX_TIMEOUT_MAX];
    bool mTimeOutEventPending[HDCP2X_TX_TIMEOUT_MAX];
    bool mTimeOutEventHappend[HDCP2X_TX_TIMEOUT_MAX];

    bool mRxPrecomputeLSupport;

    State mState;
    HDCPVer mVersion;

    void *mCookie;
    HDCPModule::ObserverFunc mObserverNotify;

    uint64_t mInputCtr[HDCP2X_TX_MAX_STREAM_NUM];
    //AES_KEY_PARAM_T mAESParam[HDCP2X_TX_MAX_STREAM_NUM];
    uint32_t mEncryptCnt[HDCP2X_TX_MAX_STREAM_NUM];
    //symmetric_CTR ctr[HDCP2X_TX_MAX_STREAM_NUM];			/*!!!!  fix !!!!!*/
    int mCipherIdx;

    DISALLOW_EVIL_CONSTRUCTORS(WFDHDCPTx);

};

}
#endif

