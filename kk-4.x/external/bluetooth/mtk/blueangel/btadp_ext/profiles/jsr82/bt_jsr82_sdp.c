#include "bt_jsr82_sdp.h"
#include "bt_jsr82_api.h"

#include "bt_ext_debug.h"
#include "bt_message.h"

#include <netinet/in.h>

#define SDP_PBAP_VERSION 								0x0101
#define SDP_OPP_VERSION 								0x0100
#define SDP_MAP_VERSION 								0x0101

#define SDP_ATTR_SERVICE_RECORD_STATE                0x0002
#define SDP_ATTR_SERVICE_ID                          0x0003
#define SDP_ATTR_BROWSE_GROUP_LIST                   0x0005
#define SDP_ATTR_LANG_BASE_ID_LIST                   0x0006
#define SDP_ATTR_SERVICE_INFO_TIME_TO_LIVE           0x0007
#define SDP_ATTR_SERVICE_AVAILABILITY                0x0008
#define SDP_ATTR_PROFILE_DESC_LIST                	0x0009
#define SDP_ATTR_ADDITIONAL_PROT_DESC_LISTS          0x000d

/* Object Push Profile */
#define SDP_ATTR_SUPPORTED_FORMATS_LIST              0x0303
/* Phonebook Access Profile */
#define SDP_ATTR_SUPPORTED_FEATURES		             0x0311
/* Phonebook Access Profile */
#define SDP_ATTR_SUPPORTED_REPOSITORIES              0x0314  /* Awaiting for an official value from CAR WG */


#define SDP_ATTR_MAS_INSTANCE_ID                	0x0315 /* MAP profile */
#define SDP_ATTR_SUPPORTED_MSG_TYPE             	0x0316 /* MAP profile */




/* OBEX Object Push service and Object Push profile. */
#define SC_OBEX_OBJECT_PUSH							0x1105
/* Phonebook Access Server */
#define SC_PBAP_SERVER                          	0x112F
/* Phonebook Access */
#define SC_PBAP                                 	0x1130
/* Message Access Server */
#define SC_MAP_SERVER                           	0x1132

/* Message Notification Server */
#define SC_MAP_MNS_SERVER                       	0x1133

/* Message Access Profile */
#define SC_MAP                                  	0x1134
/* SPP. */
#define SC_SPP										0x1101




#define SDP_ATTR_SIZE_UUID_128 				(2 + 2 + 1 + BTMTK_SDP_UUID_128_BIT_SIZE)
#define SDP_ATTR_SIZE_UUID_16 				(2 + 2 + 1 + BTMTK_SDP_UUID_16_BIT_SIZE)
#define SDP_ATTR_SIZE_PROT 					(4 + 2 + 3 + 2 + 3 + 2 )	
#define SDP_ATTR_SIZE_PROT_OPP_PBAP			(4 + 2 + 3 + 2 + 3 + 2 + 3 + 2)	
#define SDP_ATTR_SIZE_BROWSE_GROUP 			9
#define SDP_ATTR_SIZE_SUPPORT_FORMAT		8
#define SDP_ATTR_SIZE_PROFILE_VERSION	 	12
#define SDP_ATTR_SIZE_PBAP_REPOSITORIES		4
#define SDP_ATTR_SIZE_PBAP_FEATURES			7
#define SDP_ATTR_SIZE_MAP_INSTANCE			4
#define SDP_ATTR_SIZE_MAP_SUPPORT_TYPE		4


//PBAP features 
#define SDP_PBAP_SUPF_DOWNLOAD     	0x0001
#define SDP_PBAP_SUPF_BROWSE     	0x0002
//PBAP repositories
#define SDP_PBAP_REPOSIT_LOCAL    	0x0001
#define SDP_PBAP_REPOSIT_SIM    	0x0002

//MAP supported message types
#define SDP_MAP_SUPMT_EMAIL   		0x01
#define SDP_MAP_SUPMT_SMS_CDMA 		0x02
#define SDP_MAP_SUPMT_SMS_GSM  		0x04
#define SDP_MAP_SUPMT_MMS   		0x08

typedef struct _SDP_RECORD 
{
	char *buffer;
	int size;
	int pos;
} SDP_Record;

#define MIN(a,b) ((a < b) ? a : b)

static JSR82_SDP_CONTEXT sdp_cntx = {.init = FALSE};

static char bluetooth_base_uuid[BTMTK_SDP_UUID_128_BIT_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};


extern BtStatus JSR82_SendMessage(msg_type msg_id, module_type mod_id, void *buffer, U16 size);

int btmtk_jsr82_get_handler_request(int identify);
BT_BOOL btmtk_jsr82_service_attrib_search_request(JSR82_SDP_QUERY *sdpquery);
bt_sdpdb_result btmtk_jsr82_create_record_request(char **rec, int rec_len, kal_uint32 handle, int identify);

void jsr82_sdp_node_init(JSR82_SDP_NODE *node)
{
	if (NULL == node)
	{
		return;
	}
	node->last = node;
	node->next = node;
}

BT_BOOL jsr82_sdp_entry_insert(JSR82_SDP_NODE *head, Session_Mutex *mutex, void *content, int contentsize)
{
	JSR82_SDP_ENTRY *entry = NULL;
	JSR82_SDP_NODE *node = NULL;

	if (NULL == mutex)
	{
		return FALSE;
	}

	entry = (JSR82_SDP_ENTRY *)malloc(sizeof(JSR82_SDP_ENTRY));
	if (NULL == entry)
	{
		return FALSE;
	}

	session_mutex_lock(mutex);

	jsr82_sdp_node_init(&entry->node);
//	memcpy(&entry->content, content, contentsize);
	entry->content = content;
	entry->contentsize = contentsize;
	
	//insert to tail
	node = &entry->node;
	node->last = head->last;
	node->next = head;

	head->last->next = node;
	head->last = node;
	session_mutex_unlock(mutex);
	return TRUE;
}


BT_BOOL jsr82_sdp_entry_remove(JSR82_SDP_NODE *head, Session_Mutex *mutex)
{
	JSR82_SDP_ENTRY *entry = NULL;
	JSR82_SDP_NODE *node = NULL;

	if(NULL == head ||
		head->next == head)
	{
		return FALSE;
	}
	session_mutex_lock(mutex);
	node = head->next;
	entry = (JSR82_SDP_ENTRY *)node;

	//remove entry from head
	node->last->next = node->next;
	node->next->last = node->last;

	free(entry);
	session_mutex_unlock(mutex);
	return TRUE;
}

//get the first one entry
BT_BOOL jsr82_sdp_entry_get(JSR82_SDP_NODE *head, JSR82_SDP_ENTRY **entry)
{	
	JSR82_SDP_ENTRY *next = NULL; 
	if(NULL == head ||
		head->next == head ||
		entry == NULL)
	{
		return FALSE;
	}

	*entry = (JSR82_SDP_ENTRY *)head->next;
	return TRUE;
}

BT_BOOL jsr82_sdp_get_info(JSR82_SDP_REGISTRATION **info)
{
	BT_BOOL ret = FALSE;
	JSR82_SDP_ENTRY *entry = NULL;
	ret = jsr82_sdp_entry_get(&sdp_cntx.info, &entry);
	if (ret == TRUE)
	{
		*info = (JSR82_SDP_REGISTRATION *)entry->content;
	}
	return ret;
}

BT_BOOL jsr82_sdp_get_query(JSR82_SDP_QUERY **query)
{
	BT_BOOL ret = FALSE;
	JSR82_SDP_ENTRY *entry = NULL;
	ret = jsr82_sdp_entry_get(&sdp_cntx.query, &entry);
	if (ret == TRUE)
	{
		*query = (JSR82_SDP_QUERY *)entry->content;
	}
	return ret;
}

BT_BOOL convert_result(bt_sdpdb_result code)
{
	switch(code)
	{
	case BTSDPDBAPP_SUCCESS:
		return TRUE;

	default:
		return FALSE;
	}
}

void btmtk_jsr82_internal_register()
{	
	JSR82_SDP_REGISTRATION *info = NULL;
	BT_BOOL ret = jsr82_sdp_get_info(&info);
	if (ret)
	{
		btmtk_jsr82_get_handler_request(info->identify);
	}
}
void btmtk_jsr82_internal_query()
{
	JSR82_SDP_QUERY *query = NULL;
	BT_BOOL ret = jsr82_sdp_get_query(&query);
	if (ret)
	{
		btmtk_jsr82_service_attrib_search_request(query);
	}
}

unsigned short pntohs(char *arr)
{
    bt_ext_log("pntohs(0x%02X%02X) : addr=%p", arr[0], arr[1], arr);
    if( (((U32)arr) & 0x1) )
    {
        unsigned short tmp;
        bt_ext_log("pntohs : use tmp");
        memcpy(&tmp, arr, sizeof(unsigned short));
        return ntohs(tmp);
    }
    else
    {
        return ntohs(*(unsigned short*)arr);
    }
}


BT_BOOL btmtk_util_is_assigned_uuid(char *uuid128)
{
    return (memcmp(&bluetooth_base_uuid[4], &uuid128[4], 12) == 0);
}

unsigned short btmtk_util_convert_uuid128_2_uuid16(char *uuid128)
{
    unsigned short *data0 = (unsigned short *)&uuid128[2], *data1 = (unsigned short *)&bluetooth_base_uuid[2];
    bt_ext_log("btmtk_util_convert_uuid128_2_uuid16: 0x%X, 0x%X", *data0, *data1);
    return (pntohs(&uuid128[2]) - pntohs(&bluetooth_base_uuid[2]));
}

BT_BOOL btmtk_util_is_desired_uuid(char *uuid128, short target_uuid)
{
	if (uuid128 == NULL)
	{
		return FALSE;
	}
	if (FALSE == btmtk_util_is_assigned_uuid(uuid128))
	{
		return FALSE;
	}
	return (target_uuid == btmtk_util_convert_uuid128_2_uuid16(uuid128));
}

int btmtk_util_get_raw_record_size(char *uuid128, const char *name)
{
	int base_size = SDP_ATTR_SIZE_BROWSE_GROUP;	

    /* Name attr size */
    if (name && strlen(name) > 0)
    {
        base_size += strlen(name) + 1 + 3;
    }

	if (btmtk_util_is_desired_uuid(uuid128, SC_OBEX_OBJECT_PUSH))
	{
		base_size += SDP_ATTR_SIZE_UUID_16 + 
						SDP_ATTR_SIZE_PROT_OPP_PBAP + 
						SDP_ATTR_SIZE_PROFILE_VERSION + 
						SDP_ATTR_SIZE_SUPPORT_FORMAT;
	}
	else if (btmtk_util_is_desired_uuid(uuid128, SC_PBAP_SERVER))
	{
		base_size += SDP_ATTR_SIZE_UUID_16 + 
						SDP_ATTR_SIZE_PROT_OPP_PBAP + 
						SDP_ATTR_SIZE_PROFILE_VERSION + 
						SDP_ATTR_SIZE_PBAP_REPOSITORIES;
	}
	else if (btmtk_util_is_desired_uuid(uuid128, SC_MAP_SERVER))
	{
		base_size += SDP_ATTR_SIZE_UUID_16 + 
						SDP_ATTR_SIZE_PROT_OPP_PBAP + 
						SDP_ATTR_SIZE_PROFILE_VERSION +
						SDP_ATTR_SIZE_MAP_INSTANCE + 
						SDP_ATTR_SIZE_MAP_SUPPORT_TYPE;
	}
	else
	{
		base_size += SDP_ATTR_SIZE_UUID_128 + SDP_ATTR_SIZE_PROT;
	}
	
	return base_size;
}

int btmtk_jsr82_int_get_data_element_header(btmtk_sdp_element_type type, int size)
{
    int size_desc;
    switch (type)
    {
    case BTMTK_SDP_ELEM_BOOL:
        return (BTMTK_SDP_DESC_BOOL | BTMTK_SDP_DESC_SIZE_1_B);
    case BTMTK_SDP_ELEM_SIGNED_INT:
        switch (size)
        {
        case 1:
            return (BTMTK_SDP_DESC_SIGNED_INT | BTMTK_SDP_DESC_SIZE_1_B);
        case 2:
            return (BTMTK_SDP_DESC_SIGNED_INT | BTMTK_SDP_DESC_SIZE_2_B);
        case 4:
            return (BTMTK_SDP_DESC_SIGNED_INT | BTMTK_SDP_DESC_SIZE_4_B);
        case 8:
            return (BTMTK_SDP_DESC_SIGNED_INT | BTMTK_SDP_DESC_SIZE_8_B);
        case 16:
            return (BTMTK_SDP_DESC_SIGNED_INT | BTMTK_SDP_DESC_SIZE_16_B);
        default:
            return 0;
        }
    case BTMTK_SDP_ELEM_UNSIGNED_INT:
        switch (size)
        {
        case 1:
            return (BTMTK_SDP_DESC_UNSIGNED_INT | BTMTK_SDP_DESC_SIZE_1_B);
        case 2:
            return (BTMTK_SDP_DESC_UNSIGNED_INT | BTMTK_SDP_DESC_SIZE_2_B);
        case 4:
            return (BTMTK_SDP_DESC_UNSIGNED_INT | BTMTK_SDP_DESC_SIZE_4_B);
        case 8:
            return (BTMTK_SDP_DESC_UNSIGNED_INT | BTMTK_SDP_DESC_SIZE_8_B);
        case 16:
            return (BTMTK_SDP_DESC_UNSIGNED_INT | BTMTK_SDP_DESC_SIZE_16_B);
        default:
            return 0;
        }
    case BTMTK_SDP_ELEM_UUID:
        switch (size)
        {
        case 2:
            return (BTMTK_SDP_DESC_UUID | BTMTK_SDP_DESC_SIZE_2_B);
        case 4:
            return (BTMTK_SDP_DESC_UUID | BTMTK_SDP_DESC_SIZE_4_B);
        case 16:
            return (BTMTK_SDP_DESC_UUID | BTMTK_SDP_DESC_SIZE_16_B);
        default:
            return 0;
        }
    default:
        if (size < 0xFF)
        {
            size_desc = BTMTK_SDP_DESC_SIZE_IN_NEXT_B;
        }
        else if (size < 0xFFFF)
        {
            size_desc = BTMTK_SDP_DESC_SIZE_IN_NEXT_2B;
        }
        else
        {
            size_desc = BTMTK_SDP_DESC_SIZE_IN_NEXT_4B;
        }
    }

    switch (type)
    {
    case BTMTK_SDP_ELEM_TEXT:
        return (BTMTK_SDP_DESC_TEXT | size_desc);
    case BTMTK_SDP_ELEM_URL:
        return (BTMTK_SDP_DESC_URL | size_desc);
    case BTMTK_SDP_ELEM_SEQUENCE:
        return (BTMTK_SDP_DESC_SEQUENCE | size_desc);
    case BTMTK_SDP_ELEM_ALTERNATIVE:
        return (BTMTK_SDP_DESC_ALTERNATIVE | size_desc);
    default:
            return 0;
    }
    return 0;
}

int btmtk_jsr82_int_write_size_bytes(char *buf, int size)
{
    int idx = 0;
    if (size < 0xFF)
    {
        SDP_WRITE_8BIT(buf, idx, size);
    }
    else if (size < 0xFFFF)
    {
        SDP_WRITE_16BIT(buf, idx, size);
    }
    else
    {
        SDP_WRITE_32BIT(buf, idx, size);
    }
    return idx;
}
BT_BOOL btmtk_sdp_util_check_record(SDP_Record *sdp_record, int min_size)
{
	if (NULL == sdp_record || 
		sdp_record->size < sdp_record->pos ||
		(sdp_record->size - sdp_record->pos) < min_size)
	{		
		return FALSE;
	}
	return TRUE;
}


BT_BOOL btmtk_sdp_util_add_8bit_attribute(SDP_Record *sdp_record, short attr,char value)
{
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, 4))
	{
		return FALSE;
	}
	//Instance ID
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, attr);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UNSIGNED_INT, 1);
	/* 1 */ SDP_WRITE_8BIT(sdp_record->buffer, sdp_record->pos, value); 
	return TRUE;
}


BT_BOOL btmtk_sdp_util_add_assigned_service_class(SDP_Record *sdp_record, short uuid_16)
{		
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_UUID_16))
	{
		return FALSE;
	}
	/* UUID */
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_SERVICE_CLASS_ID_LIST);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, BTMTK_SDP_UUID_16_BIT_SIZE +1); 
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, BTMTK_SDP_UUID_16_BIT_SIZE + 1);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_16_BIT_SIZE);
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, uuid_16);
	
	return TRUE;
}

BT_BOOL btmtk_sdp_util_add_common_service_class(SDP_Record *sdp_record, char *uuid_128)
{		
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_UUID_128))
	{
		return FALSE;
	}
	/* UUID */
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_SERVICE_CLASS_ID_LIST);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, BTMTK_SDP_UUID_128_BIT_SIZE +1); 
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, BTMTK_SDP_UUID_128_BIT_SIZE + 1);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_128_BIT_SIZE);
    /* 16 */ memcpy(sdp_record->buffer + sdp_record->pos, uuid_128, BTMTK_SDP_UUID_128_BIT_SIZE);
    sdp_record->pos += BTMTK_SDP_UUID_128_BIT_SIZE;
	return TRUE;
}

BT_BOOL btmtk_sdp_util_add_common_protocol(SDP_Record *sdp_record, short channel)
{
	int size = 0;
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_PROT))
	{
		return FALSE;
	}

	/* PROTOCOL DESCRIPTION LIST*/
	size = (2 + 3) + (2 + 3 + 2);
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_PROTOCOL_DESC_LIST);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);

    /* PROTOCOL -> L2CAP */
    size = 3;
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_16_BIT_SIZE);
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_PROT_L2CAP);
    /* PROTOCOL -> RFCOMM + channel */
    size = 3 + 2;
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_16_BIT_SIZE);
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_PROT_RFCOMM);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UNSIGNED_INT, 1);
    /* 1 */ SDP_WRITE_8BIT(sdp_record->buffer, sdp_record->pos, channel & 0xFF);	

	return TRUE;
}

BT_BOOL btmtk_sdp_util_add_obex_protocol(SDP_Record *sdp_record, short channel)
{
	int original_idx = 0, size = 0;
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_PROT_OPP_PBAP))
	{
		return FALSE;
	}

	original_idx = sdp_record->pos;

	if (FALSE == btmtk_sdp_util_add_common_protocol(sdp_record, channel))
	{
		return FALSE;
	}
	
	//modify the attribute size firstly	
	size = 2 + 3 + 2 + 3 + 2 + 3 + 2;
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, original_idx, SDP_ATTR_PROTOCOL_DESC_LIST);
    /* 1 */ sdp_record->buffer[original_idx++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
    /* 1 */ btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + original_idx, size);

	/* PROTOCOL -> OBEX */
    size = 3;
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_16_BIT_SIZE);
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_PROT_OBEX);

	return TRUE;
}

BT_BOOL btmtk_sdp_util_add_public_browse_group(SDP_Record *sdp_record)
{
	int size = 0;
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_BROWSE_GROUP))
	{
		return FALSE;
	}

	//Public Browse Group Service
	size = 3;
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_BROWSE_GROUP_LIST);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
	/* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_16_BIT_SIZE);
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_PUBLIC_BROWSE_GROUP);
	return TRUE;
}
BT_BOOL btmtk_sdp_util_add_profile_descriptor(SDP_Record *sdp_record, short uuid_16, short version)
{
	int size = 0;
	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_PROFILE_VERSION))
	{
		return FALSE;
	}
	//PROFILE version
	size = 8;
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_PROFILE_DESC_LIST);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
	/* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
	size = 6;
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
	/* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UUID, BTMTK_SDP_UUID_16_BIT_SIZE);
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, uuid_16);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UNSIGNED_INT, BTMTK_SDP_UUID_16_BIT_SIZE);
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, version);
	return TRUE;	
}

BT_BOOL btmtk_sdp_util_add_service_name(SDP_Record *sdp_record, const char *name)
{
	if (NULL == name || 
 		0 == strlen(name)||
 		FALSE == btmtk_sdp_util_check_record(sdp_record, (strlen(name) + 1) ))
	{
		return FALSE;
	}
	
    /* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_SERVICE_NAME);
    /* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_TEXT, strlen(name) + 1);
    /* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, strlen(name) + 1);
    /* namelen */ strncpy(sdp_record->buffer + sdp_record->pos, name, strlen(name));
    sdp_record->pos += (strlen(name) + 1);	
    return TRUE;
}

BT_BOOL btmtk_sdp_util_add_supported_repositories(SDP_Record *sdp_record, char value)
{
	return btmtk_sdp_util_add_8bit_attribute(sdp_record, SDP_ATTR_SUPPORTED_REPOSITORIES, value);
}

BT_BOOL btmtk_sdp_util_add_supported_formats(SDP_Record *sdp_record, char value_1, char value_2)
{
	int size = 0;

	if (FALSE == btmtk_sdp_util_check_record(sdp_record, SDP_ATTR_SIZE_SUPPORT_FORMAT))
	{
		return FALSE;
	}
	//SUPPORTED formats
	size = 4;
	/* 2 */ SDP_WRITE_16BIT(sdp_record->buffer, sdp_record->pos, SDP_ATTR_SUPPORTED_FORMATS_LIST);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_SEQUENCE, size); 
	/* 1 */ sdp_record->pos += btmtk_jsr82_int_write_size_bytes(sdp_record->buffer + sdp_record->pos, size);
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UNSIGNED_INT, 1);
	/* 1 */ SDP_WRITE_8BIT(sdp_record->buffer, sdp_record->pos, value_1); 
	/* 1 */ sdp_record->buffer[sdp_record->pos++] = btmtk_jsr82_int_get_data_element_header(BTMTK_SDP_ELEM_UNSIGNED_INT, 1);
	/* 1 */ SDP_WRITE_8BIT(sdp_record->buffer, sdp_record->pos, value_2); 
	return TRUE;

}
BT_BOOL btmtk_sdp_util_add_map_instance_id(SDP_Record *sdp_record, char value)
{
	return btmtk_sdp_util_add_8bit_attribute(sdp_record, SDP_ATTR_MAS_INSTANCE_ID, value);
}



BT_BOOL btmtk_sdp_util_add_map_supported_msg_type(SDP_Record *sdp_record, char value)
{	
	return btmtk_sdp_util_add_8bit_attribute(sdp_record, SDP_ATTR_SUPPORTED_MSG_TYPE, value);
}

BT_BOOL btmtk_jsr82_int_compose_opp_record(SDP_Record *sdp_record, const char *name, char *uuid, short channel)
{
	bt_ext_log("btmtk_jsr82_int_compose_opp_record");

	if (FALSE == btmtk_sdp_util_add_assigned_service_class(sdp_record, SC_OBEX_OBJECT_PUSH))
	{
		bt_ext_log("[JSR82][SDP] error in service class");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_obex_protocol(sdp_record, channel))
	{
		bt_ext_log("[JSR82][SDP] error in protocol");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_public_browse_group(sdp_record))
	{
		bt_ext_log("[JSR82][SDP] error in browse group");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_profile_descriptor(sdp_record, SC_OBEX_OBJECT_PUSH, SDP_OPP_VERSION))
	{
		bt_ext_log("[JSR82][SDP] error in profile descriptor");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_service_name(sdp_record, name))
	{
		bt_ext_log("[JSR82][SDP] error in service name");
		return FALSE;
	}
	
	//0x01: vCard 2.1
	//0xFF:all type of aobject 
	if (FALSE == btmtk_sdp_util_add_supported_formats(sdp_record, 0x01, 0xFF))
	{		
		bt_ext_log("[JSR82][SDP] error in suppoorted formats");
		return FALSE;
	}

    if (sdp_record->pos > sdp_record->size)
    {
        bt_ext_err("error!!!! btmtk_jsr82_int_compose_opp_record: size=%d >= malloc size=%d", sdp_record->pos, sdp_record->size);
    }
    bt_ext_log("btmtk_jsr82_int_compose_opp_record: size=%d ", sdp_record->pos);
    return TRUE;
}

BT_BOOL btmtk_jsr82_int_compose_pbap_record(SDP_Record *sdp_record, const char *name, char *uuid, short channel)
{
	bt_ext_log("btmtk_jsr82_int_compose_pbap_record");

	if (FALSE == btmtk_sdp_util_add_assigned_service_class(sdp_record, SC_PBAP_SERVER))
	{
		bt_ext_log("[JSR82][SDP] error in service class");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_obex_protocol(sdp_record, channel))
	{
		bt_ext_log("[JSR82][SDP] error in protocol");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_public_browse_group(sdp_record))
	{
		bt_ext_log("[JSR82][SDP] error in browse group");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_profile_descriptor(sdp_record, 
														SC_PBAP, 
														SDP_PBAP_VERSION))
	{
		bt_ext_log("[JSR82][SDP] error in profile descriptor");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_service_name(sdp_record, name))
	{
		bt_ext_log("[JSR82][SDP] error in service name");
		return FALSE;
	}
	
	if (FALSE == btmtk_sdp_util_add_supported_repositories(sdp_record,  
															SDP_PBAP_REPOSIT_LOCAL))
	{		
		bt_ext_log("[JSR82][SDP] error in suppoorted repositories");
		return FALSE;
	}
	
    if (sdp_record->pos > sdp_record->size)
    {
        bt_ext_err("error!!!! btmtk_jsr82_int_compose_pbap_record: size=%d >= malloc size=%d", sdp_record->pos, sdp_record->size);
    }
    bt_ext_log("btmtk_jsr82_int_compose_pbap_record: size=%d ", sdp_record->pos);
    return TRUE;	
}

BT_BOOL btmtk_jsr82_int_compose_map_record(SDP_Record *sdp_record, const char *name, char *uuid, short channel)
{
	bt_ext_log("btmtk_jsr82_int_compose_map_record");

	if (FALSE == btmtk_sdp_util_add_assigned_service_class(sdp_record, SC_MAP_SERVER))
	{
		bt_ext_log("[JSR82][SDP] error in service class");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_obex_protocol(sdp_record, channel))
	{
		bt_ext_log("[JSR82][SDP] error in protocol");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_public_browse_group(sdp_record))
	{
		bt_ext_log("[JSR82][SDP] error in browse group");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_service_name(sdp_record, name))
	{
		bt_ext_log("[JSR82][SDP] error in service name");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_profile_descriptor(sdp_record, 
														SC_MAP, 
														SDP_MAP_VERSION))
	{
		bt_ext_log("[JSR82][SDP] error in profile descriptor");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_map_instance_id(sdp_record, 0x0))
	{		
		bt_ext_log("[JSR82][SDP] error in instance id");
		return FALSE;
	}
	
	if (FALSE == btmtk_sdp_util_add_map_supported_msg_type(sdp_record, 
												(SDP_MAP_SUPMT_SMS_GSM | SDP_MAP_SUPMT_SMS_CDMA | SDP_MAP_SUPMT_MMS)))
	{		
		bt_ext_log("[JSR82][SDP] error in suppoorted message type");
		return FALSE;
	}	

    if (sdp_record->pos > sdp_record->size)
    {
        bt_ext_err("error!!!! btmtk_jsr82_int_compose_map_record: size=%d >= malloc size=%d", sdp_record->pos, sdp_record->size);
    }
    bt_ext_log("btmtk_jsr82_int_compose_map_record: size=%d ", sdp_record->pos);
    return TRUE;   
}



/*****************************************************************************
 * FUNCTION
 *  btmtk_jsr82_int_compose_record
 * DESCRIPTION
 *  Compose record from CreateInfo
 * PARAMETERS
 *  rec_buf           [OUT]        
 *  CreateInfo        [IN]        
 *  rec_buf_size      [IN]        
 * RETURNS
 *  Size in bytes
 *****************************************************************************/
BT_BOOL btmtk_jsr82_int_compose_common_record(SDP_Record * sdp_record, const char *name, char *uuid, short channel)
{
	
	bt_ext_log("btmtk_jsr82_int_compose_common_record");

	if (FALSE == btmtk_sdp_util_add_common_service_class(sdp_record, uuid))
	{
		bt_ext_log("[JSR82][SDP] error in service class");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_common_protocol(sdp_record, channel))
	{
		bt_ext_log("[JSR82][SDP] error in protocol");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_public_browse_group(sdp_record))
	{
		bt_ext_log("[JSR82][SDP] error in browse group");
		return FALSE;
	}

	if (FALSE == btmtk_sdp_util_add_service_name(sdp_record, name))
	{
		bt_ext_log("[JSR82][SDP] error in service name");
		return FALSE;
	}

    if (sdp_record->pos > sdp_record->size)
    {
        bt_ext_err("error!!!! btmtk_jsr82_int_compose_common_record: size=%d >= malloc size=%d", sdp_record->pos, sdp_record->size);
    }
    bt_ext_log("btmtk_jsr82_int_compose_common_record: size=%d ", sdp_record->pos);
    return TRUE;     
 
}


/*****************************************************************************
 * FUNCTION
 *  btmtk_jsr82_int_compose_record
 * DESCRIPTION
 *  Compose record from CreateInfo
 * PARAMETERS
 *  rec_buf           [OUT]        
 *  CreateInfo        [IN]        
 *  rec_buf_size      [IN]        
 * RETURNS
 *  Size in bytes
 *****************************************************************************/
int btmtk_jsr82_int_compose_record(char **buf, const char *name, char *uuid, short channel)
{
	SDP_Record record;
	BT_BOOL bRet = FALSE;
	
    bt_ext_log("btmtk_jsr82_int_compose_record: name=%s, channel=%d ", name, channel);

	//allocate memory
	memset (&record, 0x0, sizeof(SDP_Record));
	
	record.size = btmtk_util_get_raw_record_size(uuid, name) + 50;  
    if ((*buf = (char*)malloc(record.size)) == NULL)
    {
        return 0;
    }
    bt_ext_log("btmtk_jsr82_int_compose_record: calculated size=%d ", record.size);

	memset(*buf, 0x0, record.size);
	
	record.buffer = *buf;
	record.pos = 0;
	
	if (btmtk_util_is_desired_uuid(uuid, SC_OBEX_OBJECT_PUSH))
	{
		bRet = btmtk_jsr82_int_compose_opp_record(&record, name, uuid, channel);
	}
	else if (btmtk_util_is_desired_uuid(uuid, SC_PBAP_SERVER))
	{
		bRet = btmtk_jsr82_int_compose_pbap_record(&record, name, uuid, channel);
	}
	else if (btmtk_util_is_desired_uuid(uuid, SC_MAP_SERVER))
	{
		bRet = btmtk_jsr82_int_compose_map_record(&record, name, uuid, channel);
	}
	else
	{
		bRet = btmtk_jsr82_int_compose_common_record(&record, name, uuid, channel);		
	}

	if (bRet)
	{
		return record.pos;
	}
	else
	{
		free(*buf);
		*buf = NULL;
		return 0;
	}
}

int btmtk_jsr82_get_handler_request(int identify)
{
	bt_app_sdpdb_get_handle_req_struct req;
	memset (&req, 0x0, sizeof(bt_app_sdpdb_get_handle_req_struct));
	bt_ext_log("[JSR82][SDP]btmtk_jsr82_get_handler_request: identify", identify);
	
	req.identify = identify;
    return JSR82_SendMessage(MSG_ID_BT_APP_SDPDB_GET_HANDLE_REQ, MOD_BT, (void*)&req, sizeof(bt_app_sdpdb_get_handle_req_struct));
}

void btmtk_jsr82_get_handler_cnf(ilm_struct *ptr)
{
	bt_app_sdpdb_get_handle_cnf_struct *cnf = (bt_app_sdpdb_get_handle_cnf_struct *)ptr->ilm_data;
	JSR82_SDP_REGISTRATION *reginfo = NULL;
	char *buf = NULL;
	int size;
	bt_sdpdb_result result = BTSDPDBAPP_SUCCESS;
	BT_BOOL ret;

	bt_ext_log("[JSR82][SDP]btmtk_jsr82_get_handler_cnf: result[%d]", cnf->result);

	ret = jsr82_sdp_get_info(&reginfo);
	if (!ret)
	{
		return;
	}

	if (cnf->result == BTSDPDBAPP_SUCCESS)
	{	
		reginfo->handle = cnf->handle;
		size = btmtk_jsr82_int_compose_record(&buf, (const char *)reginfo->name, (char *)reginfo->uuid, reginfo->channel);
		if (buf == NULL)
		{
			result = BTSDPDBAPP_FAILED;
			goto exit;
		}
	
		result = btmtk_jsr82_create_record_request(&buf, size, reginfo->handle, reginfo->identify);		
	}
	else
	{
		result = BTSDPDBAPP_FAILED;
	}
exit:
	if (BTSDPDBAPP_SUCCESS != result)
	{
		reginfo->callback(reginfo->context, convert_result(cnf->result), reginfo->identify);
		jsr82_sdp_entry_remove(&sdp_cntx.info, sdp_cntx.mutex_info);

		btmtk_jsr82_internal_register();
	}
	
}

bt_sdpdb_result btmtk_jsr82_create_record_request(char **rec, int rec_len, kal_uint32 handle, int identify)
{
    ilm_struct ilm;
	bt_sdpdb_result result = BTSDPDBAPP_SUCCESS;

	bt_app_sdpdb_register_req_struct *req_p;
	memset(&ilm, 0x0, sizeof(ilm_struct));
	ilm.msg_id = MSG_ID_BT_APP_SDPDB_REGISTER_REQ;
	req_p = (bt_app_sdpdb_register_req_struct *)ilm.ilm_data;
	req_p->type = BT_APP_REGISTER_RECORD;
	if (rec_len > BT_SDPDB_MAX_SERVICE_RECORD_SIZE)
	{
		ALOGE("[GAP] rec_len=%d > BT_SDPDB_MAX_SERVICE_RECORD_SIZE", rec_len);
		result = BTSDPDBAPP_FAILED;
		goto exit;
	}
	memcpy(req_p->record_raw, *rec, MIN(BT_SDPDB_MAX_SERVICE_RECORD_SIZE, rec_len));
	req_p->record_raw_length = rec_len;
	req_p->identify = identify;
	req_p->handle = handle;
	JSR82_SendMessage(MSG_ID_BT_APP_SDPDB_REGISTER_REQ, MOD_BT, req_p, sizeof(bt_app_sdpdb_register_req_struct));
exit:
	free(*rec);
	return result;

}
void btmtk_jsr82_create_record_cnf(ilm_struct *ptr)
{
	JSR82_SDP_REGISTRATION *reginfo = NULL;
	bt_app_sdpdb_register_cnf_struct *cnf_p = (bt_app_sdpdb_register_cnf_struct*)ptr->ilm_data;
	BT_BOOL ret;
	bt_ext_log("[GAP] btmtk_jsr82_create_record_cnf: result=%d, handle=%d", cnf_p->result, cnf_p->handle);
	
	ret = jsr82_sdp_get_info(&reginfo);
	if (!ret)
	{
		bt_ext_log("[GAP] can not find registering service");
		return;
	}
	reginfo->handle = cnf_p->handle;

	reginfo->callback(reginfo->context, convert_result(cnf_p->result), reginfo->identify);

	jsr82_sdp_entry_remove(&sdp_cntx.info, sdp_cntx.mutex_info);
	btmtk_jsr82_internal_register();
}

int btmtk_jsr82_remove_record_request(int handle)
{
	ilm_struct ilm;
    bt_app_sdpdb_deregister_req_struct *req_p;

    bt_ext_log("[GAP] btmtk_jsr82_remove_record, handle=%d", handle);
    memset(&ilm, 0x0, sizeof(ilm_struct));
    req_p = (bt_app_sdpdb_deregister_req_struct*)ilm.ilm_data;
    req_p->handle = handle;	
	return JSR82_SendMessage(MSG_ID_BT_APP_SDPDB_DEREGISTER_REQ, MOD_BT, req_p, sizeof(bt_app_sdpdb_register_req_struct));
}
void btmtk_jsr82_remove_record_cnf(ilm_struct *ptr)
{
	JSR82_SDP_REGISTRATION *reginfo = NULL;
	bt_app_sdpdb_deregister_cnf_struct *cnf_p = (bt_app_sdpdb_deregister_cnf_struct*)ptr->ilm_data;
    bt_ext_log("[GAP] btmtk_jsr82_remove_record_cnf");
}


BT_BOOL btmtk_jsr82_service_attrib_search_request(JSR82_SDP_QUERY *sdpquery)
{
    BT_BOOL ret = FALSE;
    ilm_struct ilm;
    bt_bm_search_attribute_req_struct req_p;

    bt_ext_log("[GAP] btmtk_gap_service_attrib_search_request addr=%X:%X:%X, uuid_type=%d, uuid=0x%X, attrib=0x%X",
					sdpquery->addr.lap, sdpquery->addr.uap, sdpquery->addr.nap,
					sdpquery->uuid_type, sdpquery->uuid16, sdpquery->attrib);
    memset(&req_p, 0x0, sizeof(bt_bm_search_attribute_req_struct));
    memcpy(&req_p.bd_addr, &sdpquery->addr, sizeof(bt_addr_struct));
    req_p.type = sdpquery->uuid_type;
    req_p.sdap_uuid_16 = sdpquery->uuid16;
    memcpy(req_p.sdap_uuid_128, sdpquery->uuid128, BTMTK_SDP_UUID_128_BIT_SIZE);
    req_p.attribute_id = sdpquery->attrib;
	
	JSR82_SendMessage(MSG_ID_BT_BM_SEARCH_ATTRIBUTE_REQ, MOD_BT, (void *)&req_p, sizeof(bt_bm_search_attribute_req_struct));
	return TRUE;
}
void btmtk_jsr82_service_attrib_search_cnf(ilm_struct *ptr)
{	
	bt_bm_search_attribute_cnf_struct *cnf = (bt_bm_search_attribute_cnf_struct *)ptr->ilm_data;
	JSR82_SDP_QUERY *query = NULL;
	
	bt_ext_log("[JSR82][SDP]btmtk_jsr82_service_attrib_search_cnf: result[%d]", cnf->result);
	BT_BOOL ret = jsr82_sdp_get_query(&query);
	if (NULL == query)
	{
		bt_ext_err("[JSR82][SDP]no query entry is found");
		return;
	}

	query->callback(query->context, convert_result(cnf->result), query->identify, cnf->data, cnf->len); 

	jsr82_sdp_entry_remove(&sdp_cntx.query, sdp_cntx.mutex_query);
	btmtk_jsr82_internal_query();
}

void btmtk_jsr82_sdp_init()
{
	if (sdp_cntx.init)
	{
		return;
	}
	memset(&sdp_cntx, 0x0, sizeof(JSR82_SDP_CONTEXT));
	jsr82_sdp_node_init(&sdp_cntx.info);
	jsr82_sdp_node_init(&sdp_cntx.query);
	
	session_mutex_create(&sdp_cntx.mutex_info);		
	session_mutex_create(&sdp_cntx.mutex_query);	
	sdp_cntx.init = TRUE;
}
void btmtk_jsr82_sdp_deinit()
{
	while(jsr82_sdp_entry_remove(&sdp_cntx.info, sdp_cntx.mutex_info));
	while(jsr82_sdp_entry_remove(&sdp_cntx.query, sdp_cntx.mutex_query));
	session_mutex_destroy(sdp_cntx.mutex_info);		
	session_mutex_destroy(sdp_cntx.mutex_query);	

	sdp_cntx.mutex_info = NULL;
	sdp_cntx.mutex_query = NULL;
	
	sdp_cntx.init = FALSE;
}



BT_BOOL btmtk_jsr82_sdp_query(JSR82_SDP_QUERY *query)
{
	BT_BOOL ret = FALSE;
	
	bt_ext_log("[JSR82][SDP]btmtk_jsr82_sdp_query");
	ret = jsr82_sdp_entry_insert(&sdp_cntx.query, sdp_cntx.mutex_query, (void *)query, sizeof(JSR82_SDP_QUERY));
	btmtk_jsr82_internal_query();
	return ret;	
}


BT_BOOL btmtk_jsr82_sdp_registration(JSR82_SDP_REGISTRATION *registration)
{
	int result;
	BT_BOOL ret;
	
	bt_ext_log("[JSR82][SDP]btmtk_jsr82_sdp_registration");
	ret = jsr82_sdp_entry_insert(&sdp_cntx.info, sdp_cntx.mutex_info,registration, sizeof(JSR82_SDP_REGISTRATION));
	if (!ret)
	{
		return FALSE;
	}

	btmtk_jsr82_internal_register();		
	return TRUE;
}
void btmtk_jsr82_sdp_deregistration(JSR82_SDP_REGISTRATION *reginfo)
{
	int result;
	bt_ext_log("[JSR82][SDP]btmtk_jsr82_sdp_deregistration");
	result = btmtk_jsr82_remove_record_request(reginfo->handle);
}










