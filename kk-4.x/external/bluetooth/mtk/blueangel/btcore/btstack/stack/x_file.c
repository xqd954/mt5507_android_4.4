#include "bt_common.h"
#include "xatypes.h"
#include "btalloc.h"
#include "sys/hci.h"
#include "utils.h"
#include "btconfig.h"
#include "sys/l2capxp.h"
#include "sys/sdpi.h"
#include "bt_feature.h"
#include "x_file.h"


#ifndef __BT_MP_RELEASE__
U8 BT_TURN_AUTHENTICATION_CHECK_OFF = 0;
#endif

extern int rand(void);
extern BOOL BTCheckPktServicedTimeLong(U32 int_time, U32 time_value);
extern const char* BtRadio_Get_Patch_Data(void);
extern char* BtRadio_Get_Address(void);
extern int BtRadio_Get_Patch_Size(void);
extern int BtRadio_Get_Version(void);
extern void RfTimeoutHandler(EvmTimer *timer);
#ifndef __BT_MP_RELEASE__
extern void BTWriteToVirtualSniffer(U8 type, U8 *raw_data, U16 length);
extern void BTWriteToVirtualSnifferClose(void);
#endif

//static char *patch_char;
//static U16 patch_size = 0;

static U8 encryption_start = 0;
static U8 bt_encryption_change = 0;
extern BtSdpServContext btCtxsdps;
extern BtRfcContext btCtxrfc;
//static void MeHandleAuthenticationChipTimeout(EvmTimer *Timer);
//static void MeHandleEncryptionChipTimeout(EvmTimer *Timer);
//static void MeHandleWriteWrongContextTimeout(EvmTimer *Timer);


#ifndef __BT_MP_RELEASE__
extern U8 BT_VIRTUAL_SNIFFER_FLAG;
#endif



void BT_XFile_Init(void)
{
#ifndef __BT_MP_RELEASE__
    Report(("BT_XFile_Init"));
#endif
    if(MEC(BT_Key) !=0)
        Assert(0);
    if(MEC(BT_Sres1) !=0)
        Assert(0);
    if(MEC(BT_Sres2) !=0)
        Assert(0);
    if(MEC(BT_Rand) !=0)
        Assert(0);
    if(MEC(BT_Keyaddr) !=0)
        Assert(0);
    if(MEC(BT_KeyClk) !=0)
        Assert(0);
    if(MEC(BT_TempKey) !=0)
        Assert(0);

    MEC(BT_Key) = (U8 *)get_ctrl_buffer(4);
    MEC(BT_Sres1) = (U8 *)get_ctrl_buffer(4);
    MEC(BT_Sres2) = (U8 *)get_ctrl_buffer(4);
    MEC(BT_Rand) = (U8 *)get_ctrl_buffer(4);
    MEC(BT_Keyaddr) = (U8 *)get_ctrl_buffer(4);
    MEC(BT_KeyClk) = (U8 *)get_ctrl_buffer(4);
    MEC(BT_TempKey) = (U8 *)get_ctrl_buffer(4);
    OS_MemSet(MEC(BT_Key), 0, 4);
    OS_MemSet(MEC(BT_Sres1), 0, 4);
    OS_MemSet(MEC(BT_Sres2), 0, 4);
    OS_MemSet(MEC(BT_Rand), 0, 4);
    OS_MemSet(MEC(BT_Keyaddr), 0, 4);
    OS_MemSet(MEC(BT_KeyClk), 0, 4);    
    OS_MemSet(MEC(BT_TempKey), 0, 4);        
#ifndef __BT_MP_RELEASE__
    if (BT_VIRTUAL_SNIFFER_FLAG == 1)
    {
        U8 temp_buffer[4];

        OS_MemCopy(temp_buffer, MEC(BT_Key), 4);
        BTWriteToVirtualSniffer(0x02, temp_buffer, 4);
    }
#endif    
    encryption_start = 0;
    BT_TURN_AUTHENTICATION_CHECK_OFF = 1;
    bt_encryption_change = 0;
}

U16 MetaBT_MakeCommandOpcode(U16 opcode)
{
    return opcode;
}


void BT_XFile_Deinit(void)
{
#ifndef __BT_MP_RELEASE__
    Report(("BT_XFile_Deinit"));
#endif
    if (MEC(encryptChipTimer).func !=0)
    {
        EVM_CancelTimer(&MEC(encryptChipTimer));
    }
    if(MEC(BT_Key) !=0)
    {
        free_ctrl_buffer(MEC(BT_Key));
     }
    MEC(BT_Key) = 0;
    if(MEC(BT_Sres1) !=0)
    {
        free_ctrl_buffer(MEC(BT_Sres1));
     }
    MEC(BT_Sres1) = 0;
    if(MEC(BT_Sres2) !=0)
    {
        free_ctrl_buffer(MEC(BT_Sres2));
     }
    MEC(BT_Sres2) = 0;
    if(MEC(BT_Rand) !=0)
    {
        free_ctrl_buffer(MEC(BT_Rand));
     }
    MEC(BT_Rand) = 0;
    
    if(MEC(BT_Keyaddr) !=0)
    {
        free_ctrl_buffer(MEC(BT_Keyaddr));
     }
    MEC(BT_Keyaddr) = 0;
    
    if(MEC(BT_KeyClk) !=0)
    {
        free_ctrl_buffer(MEC(BT_KeyClk));
     }
    MEC(BT_KeyClk) = 0;
    
    if(MEC(BT_TempKey) !=0)
    {
        free_ctrl_buffer(MEC(BT_TempKey));
     }
    MEC(BT_TempKey) = 0;
    
}

U8 BT_XFileGetKey0(void)
{
    return 0;
}

U8 BT_XFileGetKey1(void)
{
    return 0;
}

U8 BT_XFileGetKey2(void)
{
    return 0;
}

U8 BT_XFileGetKey3(void)
{
    return 0;
}

void ME_AuthenticationChip(void)
{
    return;
}

//static void MeHandleAuthenticationChipTimeout(EvmTimer *Timer)
//{

    
//}

//static void MeHandleEncryptionChipTimeout(EvmTimer *Timer)
//{
//}

//static void MeHandleWriteWrongContextTimeout(EvmTimer *Timer)
//{
//   OS_MemSet((U8 *)&btCtxsdps, 0, sizeof(BtSdpServContext));
//}


void BT_XfileGenAuthenticationInitKeyStep0(void)
{
    BT_TURN_AUTHENTICATION_CHECK_OFF = 1;
}


void BT_XfileGenAuthenticationInitKeyStep1(void)
{
}

void MetaBT_MakeCommand(U8 *data, U16 length)
{
    return;

}


void BT_XfileGenAuthenticationInitKeyStep2(void)
{
  
}

void BT_XfileGenAuthenticationInitKeyStep3(void)
{
}


void BT_XfileCalculateSRES0(void)
{
}


void BT_XfileCalculateSRES1(void)
{
}

void BT_XfileCalculateSRES2(void)
{
}

void BT_XfileCalculateSRES3(void)
{
}



void BT_XfileGenNewKey0(void)
{
}

void BT_XfileGenNewKey1(void)
{

}


void BT_XfileGenNewKey2(void)
{
}

void BT_XfileGenNewKey3(void)
{
}

#ifndef __BT_MP_RELEASE__
void BT_EnabledLocalLog(void)
{
    U8 temp_buffer[4];

    OS_MemCopy(temp_buffer, MEC(BT_Key), 4);
    BTWriteToVirtualSniffer(0x02, temp_buffer, 4);
}
#endif

void BT_XFile_EncryptionCommand(U8 *data, U16 length)
{
    return;

}

U16 BT_XFile_EncryptionCommandOpcode(U16 opcode)
{
    return opcode;

}

void BT_XFile_DeEncryptionEvent(U8 *data, U16 length)
{
        return;

}

U16 BT_XFile_DeOpcodeOpcode(U8 data0, U8 data1)
{
    return ((((U16)data1) << 8) | ((U16)data0));
  
}



void Me_HandleXFileOp1(BtOpEvent event)
{
}


void Me_HandleXFileOp2(BtOpEvent event)
{
}

void BT_XFileHandleGetNewKey(U8 event, U8 len, U8 *parm)
{
}


void BT_XFileHandleAuthenticationKey(U8 event, U8 len, U8 *parm)
{

}

void BT_Xfile_PrintOpcode(U16 opcode)
{
#ifndef __BT_MP_RELEASE__
    char *str;
    switch (opcode)
    {
    case HCC_INQUIRY:
        str = "HCC_INQUIRY";
        break;
    case HCC_INQUIRY_CANCEL:
        str = "HCC_INQUIRY_CANCEL";
        break;
    case HCC_START_PERIODIC_INQ_MODE:
        str = "HCC_START_PERIODIC_INQ_MODE";
        break;
    case HCC_EXIT_PERIODIC_INQ_MODE:
        str = "HCC_EXIT_PERIODIC_INQ_MODE";
        break;
    case HCC_CREATE_CONNECTION:
        str = "HCC_CREATE_CONNECTION";
        break;
    case HCC_DISCONNECT:
        str = "HCC_DISCONNECT";
        break;
    case HCC_ADD_SCO_CONNECTION:
        str = "HCC_ADD_SCO_CONNECTION";
        break;
    case HCC_CREATE_CONNECTION_CANCEL:
        str = "HCC_CREATE_CONNECTION_CANCEL";
        break;
    case HCC_ACCEPT_CON_REQ:
        str = "HCC_ACCEPT_CON_REQ";
        break;
    case HCC_REJECT_CON_REQ:
        str = "HCC_REJECT_CON_REQ";
        break;
    case HCC_LINK_KEY_REQ_REPL:
        str = "HCC_LINK_KEY_REQ_REPL";
        break;
    case HCC_LINK_KEY_REQ_NEG_REPL:
        str = "HCC_LINK_KEY_REQ_NEG_REPL";
        break;
    case HCC_PIN_CODE_REQ_REPL:
        str = "HCC_PIN_CODE_REQ_REPL";
        break;
    case HCC_PIN_CODE_REQ_NEG_REPL:
        str = "HCC_PIN_CODE_REQ_NEG_REPL";
        break;
    case HCC_CHNG_CONN_PACKET_TYPE:
        str = "HCC_CHNG_CONN_PACKET_TYPE";
        break;
    case HCC_AUTH_REQ:
        str = "HCC_AUTH_REQ";
        break;
    case HCC_SET_CONN_ENCRYPT:
        str = "HCC_SET_CONN_ENCRYPT";
        break;
    case HCC_CHNG_CONN_LINK_KEY:
        str = "HCC_CHNG_CONN_LINK_KEY";
        break;
    case HCC_MASTER_LINK_KEY:
        str = "HCC_MASTER_LINK_KEY";
        break;
    case HCC_REM_NAME_REQ:
        str = "HCC_REM_NAME_REQ";
        break;
    case HCC_REM_NAME_REQ_CANCEL:
        str = "HCC_REM_NAME_REQ_CANCEL";
        break;
    case HCC_READ_REMOTE_FEATURES:
        str = "HCC_READ_REMOTE_FEATURES";
        break;
    case HCC_READ_REMOTE_EXT_FEATURES:
        str = "HCC_READ_REMOTE_EXT_FEATURES";
        break;
    case HCC_READ_REMOTE_VERSION:
        str = "HCC_READ_REMOTE_VERSION";
        break;
    case HCC_READ_CLOCK_OFFSET:
        str = "HCC_READ_CLOCK_OFFSET";
        break;
    case HCC_READ_LMP_HANDLE:
        str = "HCC_READ_LMP_HANDLE";
        break;
    case HCC_EXCHANGE_FIXED_INFO:
        str = "HCC_EXCHANGE_FIXED_INFO";
        break;
    case HCC_EXCHANGE_ALIAS_INFO:
        str = "HCC_EXCHANGE_ALIAS_INFO";
        break;
    case HCC_PRIVATE_PAIRING_REQ_REPL:
        str = "HCC_PRIVATE_PAIRING_REQ_REPL";
        break;
    case HCC_PRIVATE_PAIRING_REQ_NEG_REPL:
        str = "HCC_PRIVATE_PAIRING_REQ_NEG_REPL";
        break;
    case HCC_GENERATED_ALIAS:
        str = "HCC_GENERATED_ALIAS";
        break;
    case HCC_ALIAS_ADDRESS_REQ_REPL:
        str = "HCC_ALIAS_ADDRESS_REQ_REPL";
        break;
    case HCC_ALIAS_ADDRESS_REQ_NEG_REPL:
        str = "HCC_ALIAS_ADDRESS_REQ_NEG_REPL";
        break;
    case HCC_SETUP_SYNC_CONNECTION:
        str = "HCC_SETUP_SYNC_CONNECTION";
        break;
    case HCC_ACCEPT_SYNC_CON_REQ:
        str = "HCC_ACCEPT_SYNC_CON_REQ";
        break;
    case HCC_REJECT_SYNC_CON_REQ:
        str = "HCC_REJECT_SYNC_CON_REQ";
        break;
    case HCC_IO_CAPABILITY_REPL:
        str = "HCC_IO_CAPABILITY_REPL";
        break;
    case HCC_USER_CONFIRM_REPL:
        str = "HCC_USER_CONFIRM_REPL";
        break;
    case HCC_USER_CONFIRM_NEG_REPL:
        str = "HCC_USER_CONFIRM_NEG_REPL";
        break;
    case HCC_IO_CAPABILITY_NEG_REPL:
        str = "HCC_IO_CAPABILITY_NEG_REPL";
        break;
    case HCC_HOLD_MODE:
        str = "HCC_HOLD_MODE";
        break;
    case HCC_SNIFF_MODE:
        str = "HCC_SNIFF_MODE";
        break;
    case HCC_EXIT_SNIFF_MODE:
        str = "HCC_EXIT_SNIFF_MODE";
        break;
    case HCC_PARK_MODE:
        str = "HCC_PARK_MODE";
        break;
    case HCC_EXIT_PARK_MODE:
        str = "HCC_EXIT_PARK_MODE";
        break;
    case HCC_QOS_SETUP:
        str = "HCC_QOS_SETUP";
        break;
    case HCC_ROLE_DISCOVERY:
        str = "HCC_ROLE_DISCOVERY";
        break;
    case HCC_SWITCH_ROLE:
        str = "HCC_SWITCH_ROLE";
        break;
    case HCC_READ_LINK_POLICY:
        str = "HCC_READ_LINK_POLICY";
        break;
    case HCC_WRITE_LINK_POLICY:
        str = "HCC_WRITE_LINK_POLICY";
        break;
    case HCC_READ_DEFAULT_LINK_POLICY:
        str = "HCC_READ_DEFAULT_LINK_POLICY";
        break;
    case HCC_WRITE_DEFAULT_LINK_POLICY:
        str = "HCC_WRITE_DEFAULT_LINK_POLICY";
        break;
    case HCC_FLOW_SPECIFICATION:
        str = "HCC_FLOW_SPECIFICATION";
        break;
    case HCC_SNIFF_SUBRATING:
        str = "HCC_SNIFF_SUBRATING";
        break;
    case HCC_SET_EVENT_MASK:
        str = "HCC_SET_EVENT_MASK";
        break;
    case HCC_RESET:
        str = "HCC_RESET";
        break;
    case HCC_EVENT_FILTER:
        str = "HCC_EVENT_FILTER";
        break;
    case HCC_FLUSH:
        str = "HCC_FLUSH";
        break;
    case HCC_READ_PIN_TYPE:
        str = "HCC_READ_PIN_TYPE";
        break;
    case HCC_WRITE_PIN_TYPE:
        str = "HCC_WRITE_PIN_TYPE";
        break;
    case HCC_CREATE_NEW_UNIT_KEY:
        str = "HCC_CREATE_NEW_UNIT_KEY";
        break;
    case HCC_READ_STORED_LINK_KEY:
        str = "HCC_READ_STORED_LINK_KEY";
        break;
    case HCC_WRITE_STORED_LINK_KEY:
        str = "HCC_WRITE_STORED_LINK_KEY";
        break;
    case HCC_DEL_STORED_LINK_KEY:
        str = "HCC_DEL_STORED_LINK_KEY";
        break;
    case HCC_CHNG_LOCAL_NAME:
        str = "HCC_CHNG_LOCAL_NAME";
        break;
    case HCC_READ_LOCAL_NAME:
        str = "HCC_READ_LOCAL_NAME";
        break;
    case HCC_READ_CONN_ACCEPT_TIMEOUT:
        str = "HCC_READ_CONN_ACCEPT_TIMEOUT";
        break;
    case HCC_WRITE_CONN_ACCEPT_TIMEOUT:
        str = "HCC_WRITE_CONN_ACCEPT_TIMEOUT";
        break;
    case HCC_READ_PAGE_TIMEOUT:
        str = "HCC_READ_PAGE_TIMEOUT";
        break;
    case HCC_WRITE_PAGE_TIMEOUT:
        str = "HCC_WRITE_PAGE_TIMEOUT";
        break;
    case HCC_READ_SCAN_ENABLE:
        str = "HCC_READ_SCAN_ENABLE";
        break;
    case HCC_WRITE_SCAN_ENABLE:
        str = "HCC_WRITE_SCAN_ENABLE";
        break;
    case HCC_READ_PAGE_SCAN_ACTIVITY:
        str = "HCC_READ_PAGE_SCAN_ACTIVITY";
        break;
    case HCC_WRITE_PAGE_SCAN_ACTIVITY:
        str = "HCC_WRITE_PAGE_SCAN_ACTIVITY";
        break;
    case HCC_READ_INQ_SCAN_ACTIVITY:
        str = "HCC_READ_INQ_SCAN_ACTIVITY";
        break;
    case HCC_WRITE_INQ_SCAN_ACTIVITY:
        str = "HCC_WRITE_INQ_SCAN_ACTIVITY";
        break;
    case HCC_READ_AUTH_ENABLE:
        str = "HCC_READ_AUTH_ENABLE";
        break;
    case HCC_WRITE_AUTH_ENABLE:
        str = "HCC_WRITE_AUTH_ENABLE";
        break;
    case HCC_READ_ENCRYPT_MODE:
        str = "HCC_READ_ENCRYPT_MODE";
        break;
    case HCC_WRITE_ENCRYPT_MODE:
        str = "HCC_WRITE_ENCRYPT_MODE";
        break;
    case HCC_READ_CLASS_OF_DEVICE:
        str = "HCC_READ_CLASS_OF_DEVICE";
        break;
    case HCC_WRITE_CLASS_OF_DEVICE:
        str = "HCC_WRITE_CLASS_OF_DEVICE";
        break;
    case HCC_READ_VOICE_SETTING:
        str = "HCC_READ_VOICE_SETTING";
        break;
    case HCC_WRITE_VOICE_SETTING:
        str = "HCC_WRITE_VOICE_SETTING";
        break;
    case HCC_READ_AUTO_FLUSH_TIMEOUT:
        str = "HCC_READ_AUTO_FLUSH_TIMEOUT";
        break;
    case HCC_WRITE_AUTO_FLUSH_TIMEOUT:
        str = "HCC_WRITE_AUTO_FLUSH_TIMEOUT";
        break;
    case HCC_READ_NUM_BC_RETRANSMIT:
        str = "HCC_READ_NUM_BC_RETRANSMIT";
        break;
    case HCC_WRITE_NUM_BC_RETRANSMIT:
        str = "HCC_WRITE_NUM_BC_RETRANSMIT";
        break;
    case HCC_READ_HOLD_MODE_ACTIVITY:
        str = "HCC_READ_HOLD_MODE_ACTIVITY";
        break;
    case HCC_WRITE_HOLD_MODE_ACTIVITY:
        str = "HCC_WRITE_HOLD_MODE_ACTIVITY";
        break;
    case HCC_READ_XMIT_POWER_LEVEL:
        str = "HCC_READ_XMIT_POWER_LEVEL";
        break;
    case HCC_READ_SCO_FC_ENABLE:
        str = "HCC_READ_SCO_FC_ENABLE";
        break;
    case HCC_WRITE_SCO_FC_ENABLE:
        str = "HCC_WRITE_SCO_FC_ENABLE";
        break;
    case HCC_SET_CTRLR_TO_HOST_FLOW_CTRL:
        str = "HCC_SET_CTRLR_TO_HOST_FLOW_CTRL";
        break;
    case HCC_HOST_BUFFER_SIZE:
        str = "HCC_HOST_BUFFER_SIZE";
        break;
    case HCC_HOST_NUM_COMPLETED_PACKETS:
        str = "HCC_HOST_NUM_COMPLETED_PACKETS";
        break;
    case HCC_READ_LINK_SUPERV_TIMEOUT:
        str = "HCC_READ_LINK_SUPERV_TIMEOUT";
        break;
    case HCC_WRITE_LINK_SUPERV_TIMEOUT:
        str = "HCC_WRITE_LINK_SUPERV_TIMEOUT";
        break;
    case HCC_READ_NUM_IAC:
        str = "HCC_READ_NUM_IAC";
        break;
    case HCC_READ_CURRENT_IAC_LAP:
        str = "HCC_READ_CURRENT_IAC_LAP";
        break;
    case HCC_WRITE_CURRENT_IAC_LAP:
        str = "HCC_WRITE_CURRENT_IAC_LAP";
        break;
    case HCC_READ_PAGE_SCAN_PERIOD_MODE:
        str = "HCC_READ_PAGE_SCAN_PERIOD_MODE";
        break;
    case HCC_WRITE_PAGE_SCAN_PERIOD_MODE:
        str = "HCC_WRITE_PAGE_SCAN_PERIOD_MODE";
        break;
    case HCC_READ_PAGE_SCAN_MODE:
        str = "HCC_READ_PAGE_SCAN_MODE";
        break;
    case HCC_WRITE_PAGE_SCAN_MODE:
        str = "HCC_WRITE_PAGE_SCAN_MODE";
        break;
    case HCC_SET_AFH_HOST_CHAN_CLASS:
        str = "HCC_SET_AFH_HOST_CHAN_CLASS";
        break;
    case HCC_READ_INQ_SCAN_TYPE:
        str = "HCC_READ_INQ_SCAN_TYPE";
        break;
    case HCC_WRITE_INQ_SCAN_TYPE:
        str = "HCC_WRITE_INQ_SCAN_TYPE";
        break;
    case HCC_READ_INQ_MODE:
        str = "HCC_READ_INQ_MODE";
        break;
    case HCC_WRITE_INQ_MODE:
        str = "HCC_WRITE_INQ_MODE";
        break;
    case HCC_READ_PAGE_SCAN_TYPE:
        str = "HCC_READ_PAGE_SCAN_TYPE";
        break;
    case HCC_WRITE_PAGE_SCAN_TYPE:
        str = "HCC_WRITE_PAGE_SCAN_TYPE";
        break;
    case HCC_READ_AFH_CHAN_ASSESS_MODE:
        str = "HCC_READ_AFH_CHAN_ASSESS_MODE";
        break;
    case HCC_WRITE_AFH_CHAN_ASSESS_MODE:
        str = "HCC_WRITE_AFH_CHAN_ASSESS_MODE";
        break;
    case HCC_READ_ANONYMITY_MODE:
        str = "HCC_READ_ANONYMITY_MODE";
        break;
    case HCC_WRITE_ANONYMITY_MODE:
        str = "HCC_WRITE_ANONYMITY_MODE";
        break;
    case HCC_READ_ALIAS_AUTH_ENABLE:
        str = "HCC_READ_ALIAS_AUTH_ENABLE";
        break;
    case HCC_WRITE_ALIAS_AUTH_ENABLE:
        str = "HCC_WRITE_ALIAS_AUTH_ENABLE";
        break;
    case HCC_READ_ANON_ADDR_CHNG_PARM:
        str = "HCC_READ_ANON_ADDR_CHNG_PARM";
        break;
    case HCC_WRITE_ANON_ADDR_CHNG_PARM:
        str = "HCC_WRITE_ANON_ADDR_CHNG_PARM";
        break;
    case HCC_RESET_FIXED_ADDR_COUNTER:
        str = "HCC_RESET_FIXED_ADDR_COUNTER";
        break;
    case HCC_WRITE_EXT_INQUIRY_RESPONSE:
        str = "HCC_WRITE_EXT_INQUIRY_RESPONSE";
        break;
    case HCC_WRITE_SIMPLE_PAIRING_MODE:
        str = "HCC_WRITE_SIMPLE_PAIRING_MODE";
        break;
    case HCC_WRITE_INQUIRY_TX_PWR:
        str = "HCC_WRITE_INQUIRY_TX_PWR";
        break;
    case HCC_READ_LOCAL_VERSION:
        str = "HCC_READ_LOCAL_VERSION";
        break;
    case HCC_READ_LOCAL_COMMANDS:
        str = "HCC_READ_LOCAL_COMMANDS";
        break;
    case HCC_READ_LOCAL_FEATURES:
        str = "HCC_READ_LOCAL_FEATURES";
        break;
    case HCC_READ_LOCAL_EXT_FEATURES:
        str = "HCC_READ_LOCAL_EXT_FEATURES";
        break;
    case HCC_READ_BUFFER_SIZE:
        str = "HCC_READ_BUFFER_SIZE";
        break;
    case HCC_READ_COUNTRY_CODE:
        str = "HCC_READ_COUNTRY_CODE";
        break;
    case HCC_READ_BD_ADDR:
        str = "HCC_READ_BD_ADDR";
        break;
    case HCC_SET_PINCODE_RSP_TIMEOUT:
        str = "HCC_SET_PINCODE_RSP_TIMEOUT";
        break;
    case HCC_READ_FAILED_CONTACT_COUNT:
        str = "HCC_READ_FAILED_CONTACT_COUNT";
        break;
    case HCC_RESET_FAILED_CONTACT_COUNT:
        str = "HCC_RESET_FAILED_CONTACT_COUNT";
        break;
    case HCC_GET_LINK_QUALITY:
        str = "HCC_GET_LINK_QUALITY";
        break;
    case HCC_READ_RSSI:
        str = "HCC_READ_RSSI";
        break;
    case HCC_READ_AFH_CHANNEL_MAP:
        str = "HCC_READ_AFH_CHANNEL_MAP";
        break;
    case HCC_READ_CLOCK:
        str = "HCC_READ_CLOCK";
        break;
    case HCC_READ_LOOPBACK_MODE:
        str = "HCC_READ_LOOPBACK_MODE";
        break;
    case HCC_WRITE_LOOPBACK_MODE:
        str = "HCC_WRITE_LOOPBACK_MODE";
        break;
    case HCC_ENABLE_DUT:
        str = "HCC_ENABLE_DUT";
        break;
    case HCC_UART_PERIPHERAL_REQ:
        str = "HCC_UART_PERIPHERAL_REQ";
        break;
    case HCC_UART_TEST_REQ:
        str = "HCC_UART_TEST_REQ";
        break;
    case HCC_DMA_TEST_REQ:
        str = "HCC_DMA_TEST_REQ";
        break;
    case HCC_GPIO_TEST_REQ:
        str = "HCC_GPIO_TEST_REQ";
        break;
    case HCC_WDT_TEST_REQ:
        str = "HCC_WDT_TEST_REQ";
        break;
    case HCC_PATCH_TEST_REQ:
        str = "HCC_PATCH_TEST_REQ";
        break;
    case HCC_MT66XX_Enabled_FLOW_CONTROL:
        str = "HCC_MT66XX_Enabled_FLOW_CONTROL";
        break;

    /*LE */
    case HCC_LE_SET_EVENT_MASK:
        str = "HCC_LE_SET_EVENT_MASK";
        break;
    case HCC_LE_READ_BUFFER_SIZE:
        str = "HCC_LE_READ_BUFFER_SIZE";
        break;
    case HCC_LE_READ_LOCAL_FEATURES:
        str = "HCC_LE_READ_LOCAL_FEATURES";
        break;
    case HCC_LE_SET_RANDOM_ADDRESS:
        str = "HCC_LE_SET_RANDOM_ADDRESS";
        break;
    case HCC_LE_SET_ADVERTISE_PARAM:
        str = "HCC_LE_SET_ADVERTISE_PARAM";
        break;
    case HCC_LE_READ_ADVERTISE_TX_POWER:
        str = "HCC_LE_READ_ADVERTISE_TX_POWER";
        break;
    case HCC_LE_SET_ADVERTISE_DATA:
        str = "HCC_LE_SET_ADVERTISE_DATA";
        break;
    case HCC_LE_SET_SCAN_RESPONSE_DATA:
        str = "HCC_LE_SET_SCAN_RESPONSE_DATA";
        break;
    case HCC_LE_SET_ADVERTISE_ENABLE:
        str = "HCC_LE_SET_ADVERTISE_ENABLE";
        break;
    case HCC_LE_SET_SCAN_PARAM:
        str = "HCC_LE_SET_SCAN_PARAM";
        break;
    case HCC_LE_SET_SCAN_ENABLE:
        str = "HCC_LE_SET_SCAN_ENABLE";
        break;
    case HCC_LE_CREATE_CONNECTION:
        str = "HCC_LE_CREATE_CONNECTION";
        break;
    case HCC_LE_CREATE_CONNECTION_CANCEL:
        str = "HCC_LE_CREATE_CONNECTION_CANCEL";
        break;
    case HCC_LE_READ_WHITE_LIST_SIZE:
        str = "HCC_LE_READ_WHITE_LIST_SIZE";
        break;
    case HCC_LE_CLEAR_WHITE_LIST:
        str = "HCC_LE_CLEAR_WHITE_LIST";
        break;
    case HCC_LE_ADD_TO_WHITE_LIST:
        str = "HCC_LE_ADD_TO_WHITE_LIST";
        break;
    case HCC_LE_REMOVE_FROM_WHITE_LIST:
        str = "HCC_LE_REMOVE_FROM_WHITE_LIST";
        break;
    case HCC_LE_CONNECT_UPDATE:
        str = "HCC_LE_CONNECT_UPDATE";
        break;
    case HCC_LE_SET_HOST_CHANNEL_CLASS:
        str = "HCC_LE_SET_HOST_CHANNEL_CLASS";
        break;
    case HCC_LE_READ_CHANNEL_MAP:
        str = "HCC_LE_READ_CHANNEL_MAP";
        break;
    case HCC_LE_READ_REMOTE_FEATURES:
        str = "HCC_LE_READ_REMOTE_FEATURES";
        break;
    case HCC_LE_ENCRYPT:
        str = "HCC_LE_ENCRYPT";
        break;
    case HCC_LE_RAND:
        str = "HCC_LE_RAND";
        break;
    case HCC_LE_START_ENCRYPT:
        str = "HCC_LE_START_ENCRYPT";
        break;
    case HCC_LE_LONG_TERM_KEY_REQ_REPL:
        str = "HCC_LE_LONG_TERM_KEY_REQ_REPL";
        break;
    case HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL:
        str = "HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL";
        break;
    case HCC_LE_READ_SUPPORT_STATES:
        str = "HCC_LE_READ_SUPPORT_STATES";
        break;
    case HCC_LE_RECEIVER_TEST:
        str = "HCC_LE_RECEIVER_TEST";
        break;
    case HCC_LE_TRANSMITTER_TEST:
        str = "HCC_LE_TRANSMITTER_TEST";
        break;
    case HCC_LE_TEST_END:
        str = "HCC_LE_TEST_END";
        break;

#if 0
    /* 3DS */
    case HCC_SET_CONNECTIONLESS_SLAVE_BROADCAST:
        str = "HCC_SET_CONNECTIONLESS_SLAVE_BROADCAST";
        break;
    case HCC_START_SYNCHRONIZATION_TRAIN:
        str = "HCC_START_SYNCHRONIZATION_TRAIN";
        break;
    case HCC_READ_INQ_RSP_XMIT_POWER_LEVEL:
        str = "HCC_READ_INQ_RSP_XMIT_POWER_LEVEL";
        break;
    case HCC_SET_EVENT_MASK_PAGE_2:
        str = "HCC_SET_EVENT_MASK_PAGE_2";
        break;
    case HCC_SET_RESERVED_LT_ADDR:
        str = "HCC_SET_RESERVED_LT_ADDR";
        break;
    case HCC_DELETE_RESERVED_LT_ADDR:
        str = "HCC_DELETE_RESERVED_LT_ADDR";
        break;
    case HCC_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA:
        str = "HCC_SET_CONNECTIONLESS_SLAVE_BROADCAST_DATA";
        break;
    case HCC_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS:
        str = "HCC_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS";
        break;
#endif
    default: 
        str = "Unknown command";
        break;
    }
    Report(("Encoded raw opcode:%04x (%s)", opcode, str));
#else
    Report(("timestamp index:%04x", ~opcode));
#endif
}

U8 BT_CheckSendHostNumberOfCompletedPacket(void)
{
    if(bt_encryption_change ==0)
        return 0;
    else 
        return 1;
}

U8 Me_WriteAudioSetting(void)
{
    return 1;
}

void Me_AudioRemoveOp(void)
{
}

void Me_AudioDeinit(void)
{
}

void Me_AudioInit(void)
{
}

void Me_ScoHandleVoiceSettings(BtOpEvent event)
{
}


