#ifdef __BT_HDP_PROFILE__

#include "bt_adp_hdp.h"
#include "bluetooth_hdp_struct.h"

typedef BOOL (*cmp)(hdp_adp_channel_struct *,...);
typedef void *(*iterator)();
static U16 currentMark = 0;
extern hdp_adp_context_struct hdp_adp_context;

#define list_iterator(type, header, cmp_func, ...) 								\
			{																	\
				ListEntry *next = (ListEntry *)header;							\
				if(IsListEmpty(header))											\
				{																\
					return NULL;												\
				}																\
				do {															\
					next = GetNextNode(next);									\
					if (cmp_func(type(next), __VA_ARGS__))							\
					{															\
						return type(next);											\
					}															\
				} while (next != header);										\
				return NULL;													\
			}	
#define for_iterator(type, start_ptr, size, cmp_func, ...) 							\
			U8 index = 0;														\
			for (index = 0; index < size; index++)								\
			{																	\
				if (cmp_func(type(start_ptr + index), __VA_ARGS__))					\
				{																\
					return (start_ptr + index);									\
				}																\
			}																	\
			return NULL;


BOOL hdp_util_cmp_device_addr(bt_addr_struct *bt_addr1, bt_addr_struct *bt_addr2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL result;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    

    if( (bt_addr1->lap & 0x00FFFFFF) == (bt_addr2->lap & 0x00FFFFFF) &&
        bt_addr1->uap == bt_addr2->uap && bt_addr1->nap == bt_addr2->nap)
    {
        result = TRUE;
    }
    else
    {
        result = FALSE;
    }

    return result;
}

static BOOL cmp_instance_by_available(hdp_adp_instance_struct *instance, BOOL available)
{
	return available == instance->single;
}
static BOOL cmp_instance_by_id(hdp_adp_instance_struct *instance, U8 mdepId)
{
	return TRUE == instance->single && 
			mdepId == instance->mdepId;
}
static BOOL cmp_channel_by_available(hdp_adp_channel_struct *channel, BOOL available)
{
	return available == channel->inUse;
}

static BOOL cmp_channel_by_mdl_id(hdp_adp_channel_struct *channel, bt_addr_struct *addr, U16 mdlId)
{
	return (hdp_util_cmp_device_addr(&channel->bdaddr, addr)  &&
			channel->mdlId == mdlId &&
			(channel->state == HDP_ADP_CHANNEL_CONNECTING ||
			channel->state == HDP_ADP_CHANNEL_DISCONNECTING ||
			channel->state == HDP_ADP_CHANNEL_CONNECTED ));
}

static BOOL cmp_idle_channel_by_mdl_id(hdp_adp_channel_struct *channel, bt_addr_struct *addr, U16 mdlId)
{
	return (hdp_util_cmp_device_addr(&channel->bdaddr, addr)  &&
			channel->mdlId == mdlId &&
			(channel->state == HDP_ADP_CHANNEL_IDLE));
}

static BOOL cmp_channel_by_index(hdp_adp_channel_struct *channel, bt_addr_struct *addr, U16 index)
{
	return (hdp_util_cmp_device_addr(&channel->bdaddr, addr)  &&
			channel->index == index);
}
static BOOL cmp_channel_by_state(hdp_adp_channel_struct *channel, bt_addr_struct *addr,U8 state)
{
	return channel->state != HDP_ADP_CHANNEL_IDLE &&
			hdp_util_cmp_device_addr(&channel->bdaddr, addr)  &&
			channel->state == state;
}
static BOOL cmp_channel_by_l2cap_id(hdp_adp_channel_struct *channel, U16 l2capId)
{
	return channel->state != HDP_ADP_CHANNEL_IDLE &&
			channel->l2capChnId == l2capId;
}
static BOOL cmp_channel_by_water_mark(hdp_adp_channel_struct *channel, U16 watermark)
{
	return channel->state != HDP_ADP_CHANNEL_IDLE &&
			channel->waterMark == watermark;
}
static BOOL cmp_channel_by_session_id(hdp_adp_channel_struct *channel, U32 session_id)
{
	return channel->state != HDP_ADP_CHANNEL_IDLE &&
				channel->sessionId == session_id;
}
static BOOL cmp_channel_by_main(hdp_adp_channel_struct* channel, bt_addr_struct* addr)
{
	return (hdp_util_cmp_device_addr(&channel->bdaddr, addr) && 
			channel->mainChn == TRUE);
}

																
hdp_adp_instance_struct *hdp_util_find_available_instance() 
{
	for_iterator((hdp_adp_instance_struct *), hdp_adp_context.instance, HDP_ADP_MAX_INSTANCE_NUM, cmp_instance_by_available, FALSE)
}



hdp_adp_instance_struct *hdp_util_find_instance(U8 mdepId) 
{
	for_iterator((hdp_adp_instance_struct *), hdp_adp_context.instance, HDP_ADP_MAX_INSTANCE_NUM, cmp_instance_by_id, mdepId)
}

BOOL hdp_util_instance_exist(U8 mdepId) 
{
	return (NULL != hdp_util_find_instance(mdepId));
}
hdp_adp_channel_struct *hdp_util_find_available_channel() 
{
	for_iterator((hdp_adp_channel_struct *), hdp_adp_context.channels, HDP_ADP_MAX_CHANNEL_NUM, cmp_channel_by_available, FALSE)
}


hdp_adp_channel_struct *hdp_util_find_channel_by_mdl_id(bt_addr_struct *addr, U16 mdlId) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_mdl_id, addr, mdlId);
}

hdp_adp_channel_struct *hdp_util_find_idle_channel_by_mdl_id(bt_addr_struct *addr, U16 mdlId) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_idle_channel_by_mdl_id, addr, mdlId);
}


hdp_adp_channel_struct *hdp_util_find_channel_by_index(bt_addr_struct *addr, U16 index) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_index, addr, index);
}

hdp_adp_channel_struct *hdp_util_find_channel_by_state(bt_addr_struct *addr, U8 state) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_state, addr, state);
}

hdp_adp_channel_struct *hdp_util_find_channel_by_l2cap_Id(U16 l2capId) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_l2cap_id, l2capId);
}

hdp_adp_channel_struct *hdp_util_find_channel_by_water_mark(U16 watermark) 
{	
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_session_id, watermark);
}

hdp_adp_channel_struct *hdp_util_find_channel_by_session_id(U32 session_id) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_session_id, session_id);
}

hdp_adp_channel_struct *hdp_util_find_main_channel(bt_addr_struct *addr) 
{
	list_iterator((hdp_adp_channel_struct *), &hdp_adp_context.active_channel, cmp_channel_by_main, addr);
}


void hdp_util_add_channel(hdp_adp_channel_struct * channel) 
{
	if (!IsNodeOnList(&hdp_adp_context.active_channel, (ListEntry *)channel)) 
	{
		InsertTailList(&hdp_adp_context.active_channel, (ListEntry *)channel);
		channel->inUse = TRUE;
    }
}
void hdp_util_remove_channel(hdp_adp_channel_struct * channel) 
{
	if (IsNodeOnList(&hdp_adp_context.active_channel, (ListEntry *)channel))
	{
		RemoveEntryList((ListEntry *)channel);
	}
	channel->inUse = FALSE;
	
}

U16 hdp_util_get_channel_water_mark()
{
	U8 index = 0;
	BOOL inUse;
	do {
		currentMark++;
		inUse = FALSE;

		/*Notes: some inactive channels that may be reconnected have to considered*/
		for (index = 0; index < HDP_ADP_MAX_CHANNEL_NUM; index ++)
		{
			if(hdp_adp_context.channels[index].waterMark == currentMark)	
			{
				inUse = TRUE;
				break;
			}
		}
	} while (inUse);
	return currentMark;
}


bt_hdp_status hdp_util_convert_status(BtStatus status)
{
    switch (status)
    {
    case BT_STATUS_SUCCESS:
        return BT_HDP_SUCCESS;
		
    case BT_STATUS_BUSY:
    case BT_STATUS_IN_USE:
    case BT_STATUS_IN_PROGRESS:
    case BT_STATUS_PENDING:
        return BT_HDP_FAIL_BUSY;
		
    case BT_STATUS_TIMEOUT:
        return BT_HDP_FAIL_TIMEOUT;
		
    case BT_STATUS_RESTRICTED:
    case BT_STATUS_BLOCK_LIST:
    case BT_STATUS_CHIP_REASON:
        return BT_HDP_FAIL_NOT_ALLOW;
		
    case BT_STATUS_REMOTE_REJECT:
        return BT_HDP_FAIL_REMOTE_REJECT;
		
    default:
        return BT_HDP_FAIL;
    }        
}

#endif /* __BT_HDP_PROFILE__ */
