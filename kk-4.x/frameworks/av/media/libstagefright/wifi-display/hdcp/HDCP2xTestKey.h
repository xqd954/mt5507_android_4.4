
#ifndef _HDCP2X_TESTKEY_H__
#define _HDCP2X_TESTKEY_H_

#ifdef __cplusplus
extern "C" {
#endif


/*test vector*/
#define KEYLEN_ENCDCP (862)
#define KEYLEN_DCP (858)
#define KEYLEN_LC128 (16)
#define KEYLEN_CERTRX (522)
#define KEYLEN_P (64)
#define KEYLEN_Q (64)
#define KEYLEN_DP (64)
#define KEYLEN_DQ (64)
#define KEYLEN_QINV (64)

#define KEYLEN_RXID (5)                  // Receiver ID 40 bit Low Secure Storage
#define KEYLEN_PUBLIC (128 + 3)     // Device Public Modulus Key 1024bit Low Secure Storage
#define KEYLEN_PUBMN (128)     // Device Public Modulus Key 1024bit Low Secure Storage
#define KEYLEN_PUBEE (3)     // Device Public Exponent E Key 24 bit Low Secure Storage
#define KEYLEN_RESERVED (2)          // RESERVED
#define KEYLEN_ROOTSIGN (384)      // Root Signature 3072 bit Low Secure Storage
#define OK 0
#define NOT_OK -1
#define DTV_STATUS_T int
#define ENCRYPTION_DATA_LENGTH 862
#define ENCRYPTION_BLOCKS (ENCRYPTION_DATA_LENGTH / 16)
#define ENCRYPTION_SIZE (ENCRYPTION_BLOCKS * 16)
#define PRIV_KEY_SIZE (16)

#define PEKPUB_KM_SIZE 128
#define PRTX_SIZE 8
#define KM_SIZE 16
#define DKEY_SIZE 16
#define KD_SIZE 32
#define PRN_SIZE 8
#define PRRX_SIZE 8
#define KH_SIZE 16
#define KPRIVRX_SIZE 320
#define KEY_P_SIZE 64
#define KEY_Q_SIZE 64
#define KEY_DP_SIZE 64
#define KEY_DQ_SIZE 64
#define KEY_QINV_SIZE 64
#define SHA256_OUTPUT_SIZE 32
#define PH_PRIME_SIZE 32
#define PL_PRIME_SIZE 32
#define IV_SIZE 16
#define KS_SIZE 16
#define KS_XOR_LC128_SIZE 16
#define LC128_SIZE 16
#define ENCKMKH_SIZE 16
#define DECKMKH_SIZE 16

extern unsigned char au1DdiDcp2[864];

extern unsigned char TestkeyDecDcp[KEYLEN_ENCDCP];

extern unsigned char Testkeylc128[KEYLEN_LC128];

extern unsigned char Testkey_p[KEYLEN_P];

extern unsigned char Testkey_q[KEYLEN_Q];

extern unsigned char Testkey_dP[KEYLEN_DP];

extern unsigned char Testkey_dQ[KEYLEN_DQ];

extern unsigned char Testkey_qInv[KEYLEN_QINV];

extern unsigned char TestkeyKm[KM_SIZE];

extern unsigned char TestkeyKd1[DKEY_SIZE];

extern unsigned char TestkeyKd2[DKEY_SIZE];

extern unsigned char TestkeyKd[KD_SIZE];

extern unsigned char TestkeyHprime[PH_PRIME_SIZE];

extern unsigned char TestkeyLprime[PL_PRIME_SIZE];

extern unsigned char Testkeykh[KH_SIZE];

extern unsigned char TestkeyEnckmKh[ENCKMKH_SIZE];

extern unsigned char TestkeyDecKmkh[DECKMKH_SIZE];

extern unsigned char TestkeyKs[KS_SIZE];

extern unsigned char TestkeyKsXorLc128[KS_XOR_LC128_SIZE];

extern unsigned char key_dcp[KEYLEN_DCP];

extern unsigned char Testkey_Ekpub[128];

extern unsigned char key_N[128];

extern unsigned char key_E[3];

extern unsigned char key_p[KEYLEN_P];

extern unsigned char key_q[KEYLEN_Q];

extern unsigned char key_dP[KEYLEN_DP];

extern unsigned char key_dQ[KEYLEN_DQ];

extern unsigned char key_qInv[KEYLEN_QINV];

// INPUTS
/* For DDI_HDCP2_KdKeyDerivation */
extern unsigned char TestkeypRtx[8];
/* DDI_HDCP2_ComputeLprime  */
extern unsigned char TestkeypRn[8];
extern unsigned char TestkeypRrx[8];
/* For DDI_HDCP2_EncryptKmUsingKh , DDI_HDCP2_DecryptKmUsingKh */
extern unsigned char TestkeypM[16];
/* For DDI_HDCP2_DecryptEKs */
extern unsigned char TestkeypEks[16];




/*******************************************************************************


        The following are the Facsimile Keys from spec: HDCP 2_0_IIA_Errata.pdf


********************************************************************************/
extern unsigned char *facskey_certrx;
extern unsigned char *facskey_n;
extern unsigned char *facskey_e;
extern unsigned char *facskey_p;
extern unsigned char *facskey_q;
extern unsigned char *facskey_dmod_p1;
extern unsigned char *facskey_dmod_q1;
extern unsigned char *facskey_q1mod_p;
extern unsigned char *facskey_rrx;
extern unsigned char *facskey_rtx;
extern unsigned char *facskey_Ekpub_km;
extern unsigned char *facskey_km;
extern unsigned char *facskey_hprime;
extern unsigned char *facskey_rn;
extern unsigned char *facskey_lprime;
extern unsigned char *facskey_riv;
extern unsigned char *facskey_ks;
extern unsigned char *facskey_eks;
extern unsigned char *facskey_kh;
extern unsigned char *facskey_ekh_km;
extern unsigned char *facskey_m;

extern unsigned char facskey_lc128[16];
extern unsigned char facskey_kpubdcp[384];
extern unsigned char facskey_certrx_r1[522];
extern unsigned char facskey_certrx_r2[522];
extern unsigned char facskey_p_r1[64];
extern unsigned char facskey_q_r1[64];
extern unsigned char facskey_dmod_p1_r1[64];
extern unsigned char facskey_dmod_q1_r1[64];
extern unsigned char facskey_q1mod_p_r1[64];
extern unsigned char facskey_p_r2[64];
extern unsigned char facskey_q_r2[64];
extern unsigned char facskey_dmod_p1_r2[64];
extern unsigned char facskey_dmod_q1_r2[64];
extern unsigned char facskey_q1mod_p_r2[64];
extern unsigned char facskey_km_r1[16];
extern unsigned char facskey_km_r2[16];
extern unsigned char facskey_rtx_r1[8];
extern unsigned char facskey_rtx_r2[8];
extern unsigned char facskey_rrx_r1[8];
extern unsigned char facskey_rrx_r2[8];
extern unsigned char facskey_kpubrx_n_r1[128];
extern unsigned char facskey_kpubrx_e_r1[3];
extern unsigned char facskey_kpubrx_n_r2[128];
extern unsigned char facskey_kpubrx_e_r2[3];
extern unsigned char facskey_Ekpub_km_r1[128];
extern unsigned char facskey_Ekpub_km_r2[128];
extern unsigned char facskey_hprime_r1[32];
extern unsigned char facskey_hprime_r2[32];
extern unsigned char facskey_kh_r1[16];
extern unsigned char facskey_kh_r2[16];
extern unsigned char facskey_ekh_km_r1[16];
extern unsigned char facskey_ekh_km_r2[16];
extern unsigned char facskey_m_r1[16];
extern unsigned char facskey_m_r2[16];


extern unsigned char dectest_ks[16];
extern unsigned char dectest_lc128[16];
extern unsigned char dectest_riv[8];


#ifdef __cplusplus
   }
#endif

#endif /* _HDCP2X_TESTKEY_H_ */
