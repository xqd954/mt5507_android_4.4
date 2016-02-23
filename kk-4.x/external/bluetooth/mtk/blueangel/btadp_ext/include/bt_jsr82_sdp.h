#ifndef __BT_JSR82_SDP_H__
#define __BT_JSR82_SDP_H__

#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_sdp_struct.h"
#include "session_os.h"

#define SDP_REGISTRATION_NAME_MAX_LEN 100

typedef enum
{
    BTMTK_SDP_ELEM_UNSIGNED_INT,
    BTMTK_SDP_ELEM_SIGNED_INT,
    BTMTK_SDP_ELEM_UUID,
    BTMTK_SDP_ELEM_TEXT,
    BTMTK_SDP_ELEM_BOOL,
    BTMTK_SDP_ELEM_SEQUENCE,
    BTMTK_SDP_ELEM_ALTERNATIVE,
    BTMTK_SDP_ELEM_URL
} btmtk_sdp_element_type;

#define BTMTK_SDP_UUID_16_BIT_SIZE 2
#define BTMTK_SDP_UUID_32_BIT_SIZE 4
#define BTMTK_SDP_UUID_128_BIT_SIZE 16


#define BTMTK_SDP_DESC_UNSIGNED_INT 0x08  /* = 1 << 3 */
#define BTMTK_SDP_DESC_SIGNED_INT 0x10    /* = 2 << 3 */
#define BTMTK_SDP_DESC_UUID 0x18          /* = 3 << 3 */
#define BTMTK_SDP_DESC_TEXT 0x20          /* = 4 << 3 */
#define BTMTK_SDP_DESC_BOOL 0x28          /* = 5 << 3 */
#define BTMTK_SDP_DESC_SEQUENCE 0x30      /* = 6 << 3 */
#define BTMTK_SDP_DESC_ALTERNATIVE 0x38   /* = 7 << 3 */
#define BTMTK_SDP_DESC_URL 0x40           /* = 8 << 3 */ 

#define BTMTK_SDP_DESC_SIZE_1_B 0
#define BTMTK_SDP_DESC_SIZE_2_B 1
#define BTMTK_SDP_DESC_SIZE_4_B 2
#define BTMTK_SDP_DESC_SIZE_8_B 3
#define BTMTK_SDP_DESC_SIZE_16_B 4
#define BTMTK_SDP_DESC_SIZE_IN_NEXT_B 5
#define BTMTK_SDP_DESC_SIZE_IN_NEXT_2B 6
#define BTMTK_SDP_DESC_SIZE_IN_NEXT_4B 7

#define SDP_WRITE_8BIT(buf, idx, value)  {buf[idx++] = value;}
#define SDP_WRITE_16BIT(buf, idx, value) {buf[idx++] = (U8)((value & 0xff00) >> 8);  /* Bits[15:8] of size */ \
                                          buf[idx++] = (U8)(value & 0x00ff);         /* Bits[7:0] of size */}
#define SDP_WRITE_32BIT(buf, idx, value) {buf[idx++] = (U8)((value & 0xff000000) >> 24);  /* Bits[32:24] of size */\
                                          buf[idx++] = (U8)((value & 0x00ff0000) >> 16);  /* Bits[23:16] of size */\
                                          buf[idx++] = (U8)((value & 0x0000ff00) >> 8);   /* Bits[15:8] of size */\
                                          buf[idx++] = (U8)(value & 0x000000ff);          /* Bits[7:0] of size */}

#define SDP_ATTR_SERVICE_CLASS_ID_LIST 0x0001
#define SDP_ATTR_PROTOCOL_DESC_LIST    0x0004
#define SDP_ATTR_SERVICE_NAME          (0x0000+0x0100)

#define SDP_PROT_L2CAP  0x0100
#define SDP_PROT_RFCOMM 0x0003
/* OBEX Protocol */
#define SDP_PROT_OBEX	0x0008

/* Public Browse Group service. */
#define SDP_PUBLIC_BROWSE_GROUP         0x1002


typedef enum _ROLE
{
	JSR82_SDP_ROLE_CLIENT,
	JSR82_SDP_ROLE_SERVER,
} JSR82_SDP_ROLE;

typedef enum _SDP_ENTRY_STATE
{
	SDP_ENTRY_STATE_IDLE,
	SDP_ENTRY_STATE_IN_PROGRESS,
} SDP_ENTRY_STATE;

typedef void(*JSR82_SDP_CLIENT_CALLBACK)(void *context, BT_BOOL result, int identify, void *value, int valuesize);
typedef void(*JSR82_SDP_SERVER_CALLBACK)(void *context, BT_BOOL result, int identify);


typedef struct _JSR82_SDP_QUERY
{
	bt_addr_struct 				addr;
	btbm_uuid_type 				uuid_type;
	unsigned short 				uuid16;
	unsigned char  				uuid128[BTMTK_SDP_UUID_128_BIT_SIZE];
	unsigned short 				attrib;	
	

	JSR82_SDP_CLIENT_CALLBACK	callback;
	void 						*context;
	unsigned int   				identify; //valid for external module
}JSR82_SDP_QUERY;

typedef struct _JSR82_SDP_REGISTRATION
{
	unsigned char  				uuid[BTMTK_SDP_UUID_128_BIT_SIZE];
	unsigned char  				name[SDP_REGISTRATION_NAME_MAX_LEN];
	unsigned short 				channel;

	kal_uint32					handle; //valid for lower layer, Do NOT modify it

	JSR82_SDP_SERVER_CALLBACK	callback;
	void 						*context;
	int			   				identify; //valid for external module
} JSR82_SDP_REGISTRATION;

typedef struct _JSR82_SDP_NODE
{
	struct _JSR82_SDP_NODE *last;
	struct _JSR82_SDP_NODE *next;
} JSR82_SDP_NODE;

typedef struct _JSR82_SDP_ENTRY
{
	JSR82_SDP_NODE		node;	
	SDP_ENTRY_STATE		state;
		
	int					contentsize;
/*	union
	{
		JSR82_SDP_QUERY query;
		JSR82_SDP_REGISTRATION info;
	}content;*/
	void 				*content;
} JSR82_SDP_ENTRY;

typedef struct _JSR82_SDP_CONTEXT
{
	BT_BOOL init;
	
	Session_Mutex 		*mutex_query;
	JSR82_SDP_NODE 		query;
	Session_Mutex 		*mutex_info;
	JSR82_SDP_NODE 		info;
} JSR82_SDP_CONTEXT;

void btmtk_jsr82_sdp_init();
void btmtk_jsr82_sdp_deinit();
BT_BOOL btmtk_jsr82_sdp_query(JSR82_SDP_QUERY *query);
BT_BOOL btmtk_jsr82_sdp_registration(JSR82_SDP_REGISTRATION *registration);
void btmtk_jsr82_sdp_deregistration(JSR82_SDP_REGISTRATION *reginfo);

void btmtk_jsr82_create_record_cnf(ilm_struct *ptr);
void btmtk_jsr82_remove_record_cnf(ilm_struct *ptr);
void btmtk_jsr82_get_handler_cnf(ilm_struct *ptr);
void btmtk_jsr82_service_attrib_search_cnf(ilm_struct *ptr);


#endif

