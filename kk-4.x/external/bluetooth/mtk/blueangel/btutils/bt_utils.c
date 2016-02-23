

#define LOG_TAG "BT_UTILS"

#include "utils/Log.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <hardware/bluetooth.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cutils/sockets.h"

#include "bt_ext_common.h"
#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_struct.h"
#include "bluetooth_gap_struct.h"

extern int socket_make_sockaddr_un(const char *name, int namespaceId, 
        struct sockaddr_un *p_addr, socklen_t *alen);
        
static const kal_uint8  sdp_base_uuid[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                                       0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
                                       
kal_bool btmtk_util_equal_bdaddr(bt_addr_struct *addr1, bt_addr_struct *addr2)
{
    return ((addr1->lap == addr2->lap) && (addr1->uap == addr2->uap) && (addr1->nap == addr2->nap));
}

kal_bool btmtk_check_bdaddr(bt_addr_struct *addr)
{
	bt_addr_struct invalid_addr;

	memset(&invalid_addr, 0, sizeof(bt_addr_struct));
	if(addr == NULL) 
	{
		ALOGI("%s:address is Null point!\n", __FUNCTION__);
		return KAL_FALSE;
	}

	if(btmtk_util_equal_bdaddr(addr, &invalid_addr))
	{
		ALOGI("%s:address is Invaild value!\n", __FUNCTION__);
		return KAL_FALSE;
	}

	return KAL_TRUE;
}

void btmtk_util_reverse_array(U8 *dest, const U8 *src)
{
    int i = 0;
    int j = 0;

    for (i = 6; i > 0;)    
    {        
        dest[--i] = src[j++];        
    }  
	
    ALOGI("%s: dest addr=0x%02X:%02X:%02X:%02X:%02X:%02X", __FUNCTION__, 
        dest[0], dest[1], dest[2], dest[3], dest[4], dest[5]);
}

void btmtk_util_convert_bdaddr2array(bt_bdaddr_t *dest, bt_addr_struct *source)
{
	char *addr = (char *)dest->address;

	ALOGI("btmtk_util_convert_bdaddr2array: addr=0x%lX:0x%X:0x%X", source->lap, source->uap, source->nap);
	addr[0] = (U8) ((source->nap & 0xFF00) >> 8);
	addr[1] = (U8) (source->nap & 0x00FF);
	addr[2] = (U8) source->uap;
	addr[3] = (U8) ((source->lap & 0x00FF0000) >> 16);
	addr[4] = (U8) ((source->lap & 0x0000FF00) >> 8);
	addr[5] = (U8) (source->lap & 0x000000FF);
	ALOGI("btmtk_util_convert_bdaddr2array: addr=0x%X:0x%X:0x%X:0x%X:0x%X:0x%X", 
		   dest->address[0], dest->address[1], dest->address[2],
		   dest->address[3], dest->address[4], dest->address[5]);
}

void btmtk_util_convert_array2bdaddr(bt_addr_struct *dest, bt_bdaddr_t *source)
{
	char *addr = (char *)source->address;

	ALOGI("btmtk_util_convert_array2bdaddr: addr=0x%X:0x%X:0x%X:0x%X:0x%X:0x%X", 
		   source->address[0], source->address[1], source->address[2],
		   source->address[3], source->address[4], source->address[5]);
	dest->lap = (((kal_uint32)addr[5]) & 0x000000FF) 
		         | ((((kal_uint32)addr[4]) & 0x000000FF) << 8)
		         | ((((kal_uint32)addr[3]) & 0x000000FF) << 16);
	dest->uap = (kal_uint8)addr[2];
	dest->nap = (((kal_uint16)addr[1]) & 0x000000FF) 
		         | ((((kal_uint16)addr[0]) & 0x000000FF) << 8);
	ALOGI("btmtk_util_convert_array2bdaddr: addr=0x%lX:0x%X:0x%X", dest->lap, dest->uap, dest->nap);
}	

void btmtk_util_convert_bdaddr2string(char *dest, bt_addr_struct *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
            (U8) ((source->nap & 0xFF00) >> 8),
            (U8) (source->nap & 0x00FF),
            source->uap,
            (U8) ((source->lap & 0x00FF0000) >> 16),
            (U8) ((source->lap & 0x0000FF00) >> 8),
            (U8) (source->lap & 0x000000FF));
}

void btmtk_util_convert_string2bdaddr(const char *source, bt_addr_struct *dest)
{
    unsigned char addr[6];
    int i;
    char *ptr = (char*)source;
    
    //assert(dest != NULL && source != NULL);
    ALOGI("convert from %s", ptr);
    for (i = 5; i >= 0; i--)
    {
        addr[i] = strtoul(ptr, &ptr, 16);
        ptr++;
        ALOGI("convert to addr[%d]=0x%X", i, addr[i]);
    }
    dest->lap = addr[0] + (addr[1] << 8) + (addr[2] << 16);
    dest->uap = addr[3];
    dest->nap = addr[4] + (addr[5] << 8);
    ALOGI("[API] btmtk_util_convert_string2bdaddr: %s => (0x%06lX, 0x%02X, 0x%04X)", source, dest->lap, dest->uap, dest->nap);
}

void btmtk_util_convert_array2string(char *dest, bt_bdaddr_t *source)
{
    sprintf(dest, "%02X:%02X:%02X:%02X:%02X:%02X",
            source->address[0],
            source->address[1],
            source->address[2],
            source->address[3],
            source->address[4],
            source->address[5]);

}

void btmtk_util_uuid16_to_uuid128(kal_uint16 uuid16, bt_uuid_t* uuid128)
{
    uint16_t uuid16_bo;
    memset(uuid128, 0, sizeof(bt_uuid_t));

    memcpy(uuid128->uu, sdp_base_uuid, 16);
    uuid16_bo = ntohs(uuid16);
    memcpy(uuid128->uu + 2, &uuid16_bo, sizeof(uint16_t));
}

/*
UTF-8 valid format list:
0xxxxxxx
110xxxxx 10xxxxxx
1110xxxx 10xxxxxx 10xxxxxx
11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

int btmtk_util_check_utf8(char chr)
{
	int nByte = 1;

	if (chr< 0x80)
		nByte = 1;
	else if ((chr & 0xE0) == 0xC0)  //110xxxxx
		nByte = 2;
	else if ((chr & 0xF0) == 0xE0) //1110xxxx
		nByte = 3;
	else if ((chr & 0xF8) == 0xF0) //11110xxx
		nByte = 4;
	else if ((chr & 0xFC) == 0xF8) //111110xx
		nByte = 5;
	else if ((chr & 0xFE) == 0xFC) //1111110x
		nByte = 6;

	return nByte;
}

int btmtk_util_utf8_endpos(char *str, int len)
{
	int nBytes = 0;

	if(str == NULL)
		return len;
	
	nBytes = btmtk_util_check_utf8(*str);
	//ALOGI("[GAP] btmtk_util_utf8_endpos chr:[%x], nBytes:[%d], len:[%d]\n", *str, nBytes, len);
	if(nBytes < len)
	{
		return btmtk_util_utf8_endpos((str+nBytes), (len-nBytes));
	}
	else
	{
		return len;
	}
}

int btmtk_util_convert_uuidlist_2_uuid16( unsigned long service_list1,  /* 0x1100 ~ 0x111F */
    										   unsigned long  service_list2,  /* 0x1120 ~ 0x113F */
    										   unsigned long  service_list3,  /* 0x1200 ~ 0x121F */
    										   unsigned long  service_list4,  /* 0x1300~ */
    										   unsigned long  service_list5,
    										   unsigned long  service_list6,
    										   unsigned short uuid[])
{
	U8 i, idx = 0;
	if (uuid == NULL) {
		return 0;
	}
	
	for (i = 0; i < BTBM_ADP_MAX_SDAP_UUID_NO && idx < 32 * 6; idx++)
	{
		if (idx < 32)
		{
			if ((service_list1 >> idx) & 0x1)
			{
				uuid[i] = 0x1100 | idx;
				ALOGI("[GAP] uuid[%d] = 0x%x", i, uuid[i]);
				i++;
			}
		}
		else if (idx < 64)
		{
			int bit = idx - 32;
			if ((service_list2 >> bit) & 0x1)
			{
				uuid[i] = 0x1120 | bit;
				ALOGI("[GAP] uuid[%d] = 0x%x", i, uuid[i]);
				i++;
			}
		}
		else if (idx < 96)
		{
			int bit = idx - 64;
			if ((service_list3 >> bit) & 0x1)
			{
				uuid[i] = 0x1200 | bit;
				ALOGI("[GAP] uuid[%d] = 0x%x", i, uuid[i]);
				i++;
			}
		}
		else if (idx < 128)
		{
			int bit = idx - 96;
			if ((service_list4 >> bit) & 0x1)
			{
				uuid[i] = 0x1300 | bit;
				ALOGI("[GAP] uuid[%d] = 0x%x", i, uuid[i]);
				i++;
			}
		}
		else if (idx < 160)
		{
			int bit = idx - 128;
			if ((service_list5 >> bit) & 0x1)
			{
				uuid[i] = 0x1400 | bit;
				ALOGI("[GAP] entry.uuid[%d] = 0x%x", i, uuid[i]);
				i++;
			}
		}
                else if (idx < 192)
                {
                         int bit = idx - 160;
                         if ((service_list6 >> bit) & 0x1)
                         {
                              uuid[i] = 0x1800 | bit;
                              ALOGI("[GAP] uuid[%d] = 0x%x", i, uuid[i]);
                              i++;
                           }
                 }
	}
	return i;
}

void btmtk_util_list_append(btmtk_list_header_struct **list, btmtk_list_header_struct *node)
{
    btmtk_list_header_struct *ptr = *list;

    if (*list == NULL)
    {
        *list = node;
    }
    else
    {
        while (ptr->next)
        {
            ptr = ptr->next;
        }
        ptr->next = (struct _btmtk_list_header_struct *)node;
    }
}

void btmtk_util_list_remove(btmtk_list_header_struct **list, btmtk_list_header_struct *node)
{
    btmtk_list_header_struct *ptr = *list;

    if (node == NULL)
    {
        return;
    }
    
    if (list == NULL || *list == NULL)
    {
    }
    else if (*list == node)
    {
        *list = (btmtk_list_header_struct *)ptr->next;
    }
    else
    {
        while (ptr->next)
        {
            if (ptr->next == (struct _btmtk_list_header_struct *)node)
            {
                ptr->next = ptr->next->next;
                break;
            }
            ptr = ptr->next;
        }
    }
    free(node);
}

