#include "sdap.h"
#include "sdp.h"
#include "jsr82_session.h"


static U16 jsr82_attribute_id_list[] = 
{
    AID_PROTOCOL_DESC_LIST,	
};

static void Jsr82SdpQueryCallback(SdpQueryToken *sqt, U8 result, U8 attr_idx, U8 *attr_val)
{
	BtStatus ret_status = BT_STATUS_FAILED; 
	BT_JSR82_SESSION_Buffer_struct_t *session_buffer = NULL;
	BT_JSR82_SESSION_AP_Record_Context_struct_t *session = NULL;
	U16 chnport = 0;
	BT_BOOL notify = FALSE;
	
	OS_Report("[JSR82]Jsr82SdpQueryCallback result[%d], attr id[%d]", 
				result, attr_idx);

	session_buffer = ContainingRecord(sqt, BT_JSR82_SESSION_Buffer_struct_t, sdpQueryToken);
	if (session_buffer == NULL)
	{
		OS_Report("[JSR82][SDP] no channel context for this query callback");
		return;
	}

	session = (BT_JSR82_SESSION_AP_Record_Context_struct_t *)session_buffer->mainRecord;
	session->channel = 0;

	switch (result) 
    {
        case BT_STATUS_SUCCESS:  

			switch( jsr82_attribute_id_list[attr_idx] )
			{
	        	case AID_PROTOCOL_DESC_LIST:
	        		session->channel = *attr_val;	
					break;
					
				default:
					OS_Report("[JSR82][SDP]invalid attribute");
			}
			notify = TRUE;
			break;
			
        case BT_STATUS_SDP_CONT_STATE:
			break;
			
        case BT_STATUS_FAILED:
        case BT_STATUS_NOSERVICES:			
        default:  
			notify = TRUE;
            break;            
	}

	if (notify)
	{
		bt_jsr82_rfcomm_connect(session);
	}
	
}

BtStatus bt_jsr82_sdp_query(BT_JSR82_SESSION_Buffer_struct_t *session_buffer, U8 *uuid_128)
{
	sdap_service_search_multi_attribute_struct search_pattern;
	BtStatus ret = BT_STATUS_FAILED;
	
	OS_Report("[JSR82]jsr82_sdp");
	
	memset(&search_pattern, 0x0, sizeof(sdap_service_search_multi_attribute_struct));
	memset(&session_buffer->sdpQueryToken, 0x0, sizeof(SdpQueryToken));

	 // search for SINK service
    search_pattern.rm = session_buffer->cmgrHandler.remDev;
    search_pattern.sqt = &session_buffer->sdpQueryToken;
    search_pattern.uuid = SC_JSR82;
    search_pattern.callback = Jsr82SdpQueryCallback;
    search_pattern.attribute_num = sizeof(jsr82_attribute_id_list)/sizeof(jsr82_attribute_id_list[0]);
    search_pattern.attribute_id = jsr82_attribute_id_list;
	memcpy(search_pattern.uuid128_array, uuid_128, 16);
	ret = SDAP_ServiceSearchMultipleAttribute(search_pattern);

	if (ret == BT_STATUS_FAILED)
	{
		OS_Report("[JSR82][SDP] failed %d", ret);
	}

	return ret;
}

