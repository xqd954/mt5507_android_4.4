
#include <hardware/bluetooth.h>
#include <hardware/bt_sock.h>

#define LOG_TAG "BTIF_SOCK"
#include "btif_common.h"

#include "bt_ext_apis.h"
#include "bt_jsr82_api.h"
#include "bt_jsr82_hdl.h"
#include "bluetooth_jsr82_struct.h"
#include "bt_ext_debug.h"

static bt_status_t btsock_listen(btsock_type_t type, const char* service_name,
                                const uint8_t* uuid, int channel, int* sock_fd, int flags);
static bt_status_t btsock_connect(const bt_bdaddr_t *bd_addr, btsock_type_t type,
                                  const uint8_t* uuid, int channel, int* sock_fd, int flags);





/*******************************************************************************
**
** Function         btsock_ini
**
** Description     initializes the bt socket interface
**
** Returns         bt_status_t
**
*******************************************************************************/
static btsock_interface_t sock_if = {
                sizeof(sock_if),
                btsock_listen,
                btsock_connect
       };
btsock_interface_t *btif_sock_get_interface()
{
	bt_ext_log("[BTSOCK]btif_sock_get_interface");
    return &sock_if;
}
bt_status_t btif_sock_init()
{
    static volatile int binit;
    btmtk_profile_register(PROFILE_JSR82, btmtk_jsr82_handle_message, NULL);
    btmtk_jbt_init();
    return BT_STATUS_SUCCESS;
}
void btif_sock_cleanup()
{
     btmtk_jbt_deinit();
	 btmtk_profile_unregister(PROFILE_JSR82);
}

static kal_uint8 convert_socket_type(btsock_type_t type)
{
	switch(type)
    {
    case BTSOCK_RFCOMM:
		return JSR82_SESSION_PS_RFCOMM;

	case BTSOCK_L2CAP:
		return JSR82_SESSION_PS_L2CAP;

	default:
		return 0xFF;
			
	}
}
static kal_uint8 convert_security(int flags)
{
	kal_uint8 security = 0; 
	if(flags & BTSOCK_FLAG_ENCRYPT)
        security = 1;
    if(flags & BTSOCK_FLAG_AUTH)
        security = 1;

	return security;
}


static bt_status_t btsock_listen(btsock_type_t type, const char* service_name,
        const uint8_t* service_uuid, int channel, int* sock_fd, int flags)
{
	kal_uint8 jbt_type;
	BT_BOOL ret;
	kal_uint8 *name = (kal_uint8 *)service_name;
	kal_uint8 *uuid_128 = (kal_uint8 *)service_uuid;
	kal_uint16 mtu = JSR82_SESSION_PS_RFCOMM_MTU;
    bt_status_t status = BT_STATUS_FAIL;
	
	if((service_uuid == NULL && channel <= 0) || sock_fd == NULL)
    {
    	bt_ext_log("[BTSOCK]btsock_listen invalid param");
        return BT_STATUS_PARM_INVALID;
    }
    *sock_fd = -1;
	ret = btmtk_jsr82_session_service_registration(
										            convert_socket_type(type),
										            uuid_128,
										            name,            
										            0,
										            JSR82_SESSION_PS_RFCOMM_MTU,
										            convert_security(flags),
										            0,
										            (kal_int32 *)sock_fd);
	if (ret)
	{
		status = BT_STATUS_SUCCESS;
	}
    return status;
}
static bt_status_t btsock_connect(const bt_bdaddr_t *bd_addr, btsock_type_t type,
        const uint8_t* uuid, int channel, int* sock_fd, int flags)
{
    if((uuid == NULL && channel <= 0) || bd_addr == NULL || sock_fd == NULL)
    {
    	bt_ext_log("[BTSOCK]btsock_listen invalid param");
        return BT_STATUS_PARM_INVALID;
    }
    *sock_fd = -1;
    bt_status_t status = BT_STATUS_FAIL;
    BT_BOOL ret;
	kal_uint8 *addr = (kal_uint8 *)bd_addr;
	kal_uint8 *uuid_128 = (kal_uint8 *)uuid;
	ret = btmtk_jsr82_session_connect_req(
								            0,
								            addr,
								            uuid_128,
								            convert_socket_type(type),
								            channel,
								            JSR82_SESSION_PS_RFCOMM_MTU,
								            convert_security(flags),
								            (kal_int32 *)sock_fd);
	if (ret)
	{
		status = BT_STATUS_SUCCESS;
	}
	bt_ext_log("[BTSOCK]btsock_connect ret[%d], fd[%d]", ret, *sock_fd);
    return status;
}



