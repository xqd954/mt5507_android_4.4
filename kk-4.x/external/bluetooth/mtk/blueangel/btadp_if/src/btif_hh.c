#include <hardware/bluetooth.h>
#include <hardware/bt_hh.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define LOG_TAG "BTIF_HH"

#include "btif_common.h"
#include "btif_hh.h"

#include "bt_mmi.h"
#include "bt_message.h"
#include "bluetooth_gap_struct.h"
#include "bluetooth_hid_struct.h"

#include "bt_hid_hdl.h"
#include "bt_hid_api.h"
#include "bt_ext_apis.h"
#include "bt_ext_common.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/




/************************************************************************************
**  Local type definitions
************************************************************************************/



/************************************************************************************
**  Static variables
************************************************************************************/

static bthh_callbacks_t *bt_hh_callbacks = NULL;

static int servsock;


/************************************************************************************
**  Static functions
************************************************************************************/

/************************************************************************************
**  Externs
************************************************************************************/


/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/



/************************************************************************************
**  Functions
************************************************************************************/

void convert_bt_addr(U8* dest, U8 *src)
{
    int i, j = 0;     

    for(i = 6;i > 0;)    
    {        
        dest[--i] = src[j++];        
    }  
	
    ALOGI("addr=0x%02X%02X%02X%02X%02X%02X",              
		        dest[0],    
		        dest[1],    
		        dest[2],    
		        dest[3],    
		        dest[4],    
		        dest[5]);
}

void bt_hid_ext_cb(U32 hid_ui_event, BD_ADDR* addr, void* para) 
{
	int		status;
	bt_bdaddr_t	bd_addr	;

	if(addr!=0)
	{
		ALOGI("device_addr=0x%02X%02X%02X%02X%02X%02X",				
					 addr->addr[0],	 
					 addr->addr[1],	 
					 addr->addr[2],	 
					 addr->addr[3],	 
					 addr->addr[4],	 
					 addr->addr[5]);
		convert_bt_addr(bd_addr.address, addr->addr);
	}

	status=hid_ui_event;

	switch(hid_ui_event)
	{
		case MBTEVT_HID_HOST_ENABLE_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_ENABLE_SUCCESS");
			break;
		case MBTEVT_HID_HOST_ENABLE_FAIL:
			ALOGI("MBTEVT_HID_HOST_ENABLE_FAIL");
			break;
		case MBTEVT_HID_HOST_DISABLE_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_DISABLE_SUCCESS");
			bt_hid_close_socket();
			break;
		case MBTEVT_HID_HOST_DISABLE_FAIL:
			ALOGI("MBTEVT_HID_HOST_DISABLE_FAIL");
			break;
		case MBTEVT_HID_HOST_CONNECT_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_CONNECT_SUCCESS");
			HAL_CBACK(bt_hh_callbacks, connection_state_cb, &bd_addr,BTHH_CONN_STATE_CONNECTED);
			break;
		case MBTEVT_HID_HOST_CONNECTING:
			ALOGI("MBTEVT_HID_HOST_CONNECTING");
			HAL_CBACK(bt_hh_callbacks, connection_state_cb, &bd_addr,BTHH_CONN_STATE_CONNECTING);
			break;
		case MBTEVT_HID_HOST_CONNECT_FAIL:
			ALOGI("MBTEVT_HID_HOST_CONNECT_FAIL");
			HAL_CBACK(bt_hh_callbacks, connection_state_cb, &bd_addr,BTHH_CONN_STATE_DISCONNECTED);
			break;
		case MBTEVT_HID_HOST_DISCONNECT_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_DISCONNECT_SUCCESS");
			HAL_CBACK(bt_hh_callbacks, connection_state_cb, &bd_addr,BTHH_CONN_STATE_DISCONNECTED);
			break;
		case MBTEVT_HID_HOST_DISCONNECTING:
			ALOGI("MBTEVT_HID_HOST_DISCONNECTING");
			HAL_CBACK(bt_hh_callbacks, connection_state_cb, &bd_addr,BTHH_CONN_STATE_DISCONNECTING);
			break;
		case MBTEVT_HID_HOST_DISCONNECT_FAIL:
			ALOGI("MBTEVT_HID_HOST_DISCONNECT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_DESC_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_GET_DESC_SUCCESS");
			break;
		case MBTEVT_HID_HOST_GET_DESC_FAIL:
			ALOGI("MBTEVT_HID_HOST_GET_DESC_FAIL");
			break;
		case MBTEVT_HID_HOST_RECEIVE_UNPLUG:
			ALOGI("MBTEVT_HID_HOST_RECEIVE_UNPLUG");
			break;
		case MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_SEND_CONTROL_SUCCESS");
                    HAL_CBACK(bt_hh_callbacks, virtual_unplug_cb,&bd_addr,BTHH_OK);
			break;
		case MBTEVT_HID_HOST_SEND_CONTROL_FAIL:
			ALOGI("MBTEVT_HID_HOST_SEND_CONTROL_FAIL");
			HAL_CBACK(bt_hh_callbacks, virtual_unplug_cb,&bd_addr,BTHH_ERR);
			break;
		case MBTEVT_HID_HOST_SEND_REPORT_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_SEND_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SEND_REPORT_FAIL:
			ALOGI("MBTEVT_HID_HOST_SEND_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_SET_REPORT_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_SET_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SET_REPORT_FAIL:
			ALOGI("MBTEVT_HID_HOST_SET_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_REPORT_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_GET_REPORT_SUCCESS");
			break;
		case MBTEVT_HID_HOST_GET_REPORT_FAIL:
			ALOGI("MBTEVT_HID_HOST_GET_REPORT_FAIL");
			break;
		case MBTEVT_HID_HOST_GET_PROTOCOL_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_GET_PROTOCOL_SUCCESS");
			HAL_CBACK(bt_hh_callbacks, protocol_mode_cb,&bd_addr, BTHH_OK,  abs(*(bthh_protocol_mode_t*)para-1));
			break;
		case MBTEVT_HID_HOST_GET_PROTOCOL_FAIL:
			ALOGI("MBTEVT_HID_HOST_GET_PROTOCOL_FAIL");
			HAL_CBACK(bt_hh_callbacks, protocol_mode_cb,&bd_addr, BTHH_ERR, BTHH_UNSUPPORTED_MODE);
			break;
		case MBTEVT_HID_HOST_SET_PROTOCOL_SUCCESS:
			ALOGI("MBTEVT_HID_HOST_SET_PROTOCOL_SUCCESS");
			break;
		case MBTEVT_HID_HOST_SET_PROTOCOL_FAIL:
			ALOGI("MBTEVT_HID_HOST_SET_PROTOCOL_FAIL");
			break;
		case MBTEVT_HID_HOST_RECEIVE_AUTHORIZE:
			ALOGI("MBTEVT_HID_HOST_RECEIVE_AUTHORIZE");
    			btmtk_hidh_authorize_rsp_ext(*addr, TRUE);
			HAL_CBACK(bt_hh_callbacks, connection_state_cb, &bd_addr,BTHH_CONN_STATE_CONNECTING);
			break;
		default:
			ALOGI("Unknown HID event");
			break;			
	}	
}


/*****************************************************************************
**   Section name (Group of functions)
*****************************************************************************/

/*****************************************************************************
**
**   btif hh api functions (no context switch)
**
*****************************************************************************/


/*******************************************************************************
**
** Function         btif_hh_init
**
** Description     initializes the hh interface
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t init( bthh_callbacks_t* callbacks )
{
    ALOGI("%s", __FUNCTION__);

    bt_hh_callbacks = callbacks;

    btmtk_profile_register(PROFILE_HID, btmtk_hid_handle_message, NULL);
	
    if(bt_hid_init_socket()==0)
    {
	ALOGI("Create sending socket failed");
	return BT_STATUS_FAIL;
    }
    btmtk_hidh_set_callback(bt_hid_ext_cb);
    btif_enable_service(BTMTK_HID_SERVICE_ID);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function        connect
**
** Description     connect to hid device
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_connect( bt_bdaddr_t *bd_addr)
{   
    ALOGI("%s", __FUNCTION__);

    bt_hidh_req_ext_struct  msg;
	
    msg.command = BT_HIDH_CONNECT_REQ_EXT;
	
    convert_bt_addr(msg.addr.addr, bd_addr->address);
    btmtk_hidh_handle_req_ext(&msg);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         disconnect
**
** Description      disconnect from hid device
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_disconnect( bt_bdaddr_t *bd_addr )
{
    ALOGI("%s", __FUNCTION__);

    bt_hidh_req_ext_struct  msg;
    msg.command = BT_HIDH_DISCONNECT_REQ_EXT;
	
    convert_bt_addr(msg.addr.addr, bd_addr->address);
    btmtk_hidh_handle_req_ext(&msg);    

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         virtual_unplug
**
** Description      Virtual UnPlug (VUP) the specified HID device.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_virtual_unplug (bt_bdaddr_t *bd_addr)
{
    ALOGI("%s", __FUNCTION__);

    BD_ADDR bdaddr;

    convert_bt_addr(bdaddr.addr, bd_addr->address);
    btmtk_hidh_send_control_req(bdaddr, hidd_ctrl_op_virtual_cable_unplug);
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         set_info
**
** Description      Set the HID device descriptor for the specified HID device.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_set_info (bt_bdaddr_t *bd_addr, bthh_hid_info_t hid_info )
{
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         get_protocol
**
** Description      Get the HID proto mode.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_get_protocol (bt_bdaddr_t *bd_addr, bthh_protocol_mode_t protocolMode)
{
    ALOGI("%s", __FUNCTION__);

    BD_ADDR bdaddr;

    convert_bt_addr(bdaddr.addr, bd_addr->address);
    btmtk_hidh_get_protocol_req(bdaddr);
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         set_protocol
**
** Description      Set the HID proto mode.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_set_protocol (bt_bdaddr_t *bd_addr, bthh_protocol_mode_t protocolMode)
{
    ALOGI("%s", __FUNCTION__);

    BD_ADDR bdaddr;

    convert_bt_addr(bdaddr.addr, bd_addr->address);
		
    if(protocolMode == BTHH_REPORT_MODE)
    	btmtk_hidh_set_protocol_req(bdaddr, FALSE);
    else
	btmtk_hidh_set_protocol_req(bdaddr, TRUE);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         get_report
**
** Description      Send a GET_REPORT to HID device.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_get_report (bt_bdaddr_t *bd_addr, bthh_report_type_t reportType, uint8_t reportId, int bufferSize)
{
    ALOGI("%s", __FUNCTION__);

    BD_ADDR bdaddr;

    convert_bt_addr(bdaddr.addr, bd_addr->address);
		
    btmtk_hidh_get_report_req(bdaddr, reportType, reportId, bufferSize);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         set_report
**
** Description      Send a SET_REPORT to HID device.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_set_report (bt_bdaddr_t *bd_addr, bthh_report_type_t reportType, char* report)
{
    ALOGI("%s", __FUNCTION__);

    BD_ADDR bdaddr;
    U8		*rpt_data;
    U16		rpt_size = strlen(report);

    rpt_data = (U8*)malloc(rpt_size* sizeof(U8));
    memcpy(rpt_data, report, rpt_size* sizeof(U8));

    convert_bt_addr(bdaddr.addr, bd_addr->address);
		
    btmtk_hidh_set_report_req(bdaddr, (bt_hidd_report_type_enum)reportType, rpt_data, rpt_size);
    free(rpt_data);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         send_data
**
** Description      Send a SEND_DATA to HID device.
**
** Returns         bt_status_t
**
*******************************************************************************/
static bt_status_t hid_send_data (bt_bdaddr_t *bd_addr, char* data)
{
    ALOGI("%s", __FUNCTION__);

    BD_ADDR bdaddr;
    U8		*rpt_data;
    U16		rpt_size = strlen(data);

    rpt_data = (U8*)malloc(rpt_size* sizeof(U8));
    memcpy(rpt_data, data, rpt_size* sizeof(U8));

    convert_bt_addr(bdaddr.addr, bd_addr->address);
		
    btmtk_hidh_send_output_report(bdaddr, hidd_report_output, rpt_data, rpt_size);
    free(rpt_data);

    return BT_STATUS_SUCCESS;
}


/*******************************************************************************
**
** Function         cleanup
**
** Description      Closes the HH interface
**
** Returns          bt_status_t
**
*******************************************************************************/
static void  cleanup( void )
{
    ALOGI("%s", __FUNCTION__);
    btif_disable_service(BTMTK_HID_SERVICE_ID);

    btmtk_profile_unregister(PROFILE_HID);
}

static const bthh_interface_t bthhInterface = {
    sizeof(bt_interface_t),
    init,
    hid_connect,
    hid_disconnect,
    hid_virtual_unplug,
    hid_set_info,
    hid_get_protocol,
    hid_set_protocol,
    hid_get_report,
    hid_set_report,
    hid_send_data,
    cleanup,
};

/*******************************************************************************
**
** Function         btif_hh_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_hh_execute_service(BT_BOOL b_enable)
{   
     bt_hidh_req_ext_struct  msg;

     if (b_enable)
     {
          /* Enable and register with BTA-HH */
    	   ALOGI("serverActivateReq");
    	   msg.command = BT_HIDH_ACTIVATE_REQ_EXT;	
     }
     else {
         /* Disable HH */
	   ALOGI("serverDeactivateReq");
	   msg.command = BT_HIDH_DEACTIVATE_REQ_EXT;
     }
     btmtk_hidh_handle_req_ext(&msg);
     return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_hh_get_interface
**
** Description      Get the hh callback interface
**
** Returns          bthh_interface_t
**
*******************************************************************************/
const bthh_interface_t *btif_hh_get_interface()
{
    ALOGI("%s", __FUNCTION__);
    return &bthhInterface;
}
