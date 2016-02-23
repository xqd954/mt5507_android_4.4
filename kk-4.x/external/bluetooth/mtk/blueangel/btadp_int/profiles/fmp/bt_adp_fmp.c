/*****************************************************************************
 *
 * Filename:
 * ---------
 * bt_adp_fmp.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth Find Me Profile Internal Adaptation layer.
 *
 * Author:
 * -------
 * Ting Zheng
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision: #1 $
 * $Modtime: $
 * $Log: $
 *
 * 11 21 2012 dlight.ting
 * [ALPS00399218] [Bluetooth] Check in BT proximity pairing problem
 * .
 * 
 * 10 16 2012 dlight.ting
 * [ALPS00367405] [SW.MT6577_PLATFORM]BT4.0 LE PXP patch
 * .
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#if defined(__BT_FMPL_PROFILE__) || defined(__BT_FMPT_PROFILE__)
#include "bt_os_api.h"
#include "l2cap_adp.h"

#include "fmp.h"
#include "bluetooth_fmp_message.h"
#include "bluetooth_fmp_struct.h"

typedef struct _fmp_context_struct
{
    BT_BOOL	bConnTerminator;
    BT_BOOL	bConnInitiator;
    BT_BOOL 	bActive;    // true: if connection is active
    BT_BOOL     bReconnect;
    BD_ADDR	bd_addr;
    BT_BOOL	in_use;	
    FmpChannel	 *channel;
} fmp_context_struct;

fmp_context_struct fmp_cntx[FMP_MAX_DEV_NUM];

// If only Target Role is supported on local, then it is unnecessary to query Alert Service on Remote Device
//bt_fmp_service_flag g_service_flags;  // local supported service


static fmp_context_struct *fmp_find_free_context(void)
{
    fmp_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < FMP_MAX_DEV_NUM; i++)
    {
        if (!fmp_cntx[i].in_use)
        {
            cntx = &fmp_cntx[i];
            break;
        }
    }
    return cntx;	
}

static fmp_context_struct *fmp_find_free_context_by_addr(BD_ADDR *bt_addr_p)
{
    fmp_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < FMP_MAX_DEV_NUM; i++)
    {
        if (FMP_DEVICE_ADDR_EQUAL(&fmp_cntx[i].bd_addr, bt_addr_p) && 
             !fmp_cntx[i].in_use)
        {
            cntx = &fmp_cntx[i];
            break;
        }
    }
    return cntx;	
}

static fmp_context_struct *fmp_find_context_by_addr(BD_ADDR *bt_addr_p)
{
    fmp_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < FMP_MAX_DEV_NUM; i++)
    {
        if (FMP_DEVICE_ADDR_EQUAL(&fmp_cntx[i].bd_addr, bt_addr_p) && 
             fmp_cntx[i].in_use)
        {
            cntx = &fmp_cntx[i];
            break;
        }
    }
    return cntx;	
}

static fmp_context_struct *fmp_find_context_by_channel(FmpChannel *ch)
{
    fmp_context_struct *cntx = NULL;
    U8 i = 0;
	
    for (i = 0; i < FMP_MAX_DEV_NUM; i++)
    {
        if (ch == fmp_cntx[i].channel)
        {
            cntx = &fmp_cntx[i];
            break;
        }
    }
    return cntx;	
}


void fmp_send_msg(
        msg_type msg,
        module_type dstMod,
        local_para_struct *local_para)
{
    if (local_para != NULL)
    {
        BT_SendMessage(msg, dstMod, local_para, local_para->msg_len);
    }
    else
    {
        BT_SendMessage(msg, dstMod, NULL, 0);
    }
}

static void fmp_clear_connection(fmp_context_struct *cntx)
{
    cntx->bActive = FALSE;

    // free resources
    
    btmtk_os_memset((U8 *)cntx, 0, sizeof(fmp_context_struct));

}

void fmp_adp_init(void)
{
    //g_service_flags = 0;
    btmtk_os_memset((U8 *)&fmp_cntx, 0, sizeof(fmp_cntx));
}

void fmp_handle_connected(FmpChannel *channel, BtRemoteDevice *dev, BtStatus status)
{
    fmp_context_struct *cntx = fmp_find_context_by_channel(channel);

    if (cntx == NULL)
    {
        if ((cntx = fmp_find_free_context()) == NULL)
        {
            FMP_Close(channel);
            return;
        }
    }

    cntx->bActive = TRUE;

    if (cntx->bConnInitiator)
    {
        bt_fmp_connect_cnf_struct *msg_p;
        msg_p = (bt_fmp_connect_cnf_struct*) construct_local_para(sizeof(bt_fmp_connect_cnf_struct), TD_UL);
        msg_p->bt_addr = dev->bdAddr; 
        msg_p->result = TRUE;
        fmp_send_msg(MSG_ID_BT_FMP_CONNECT_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
    }
    else
    {
        bt_fmp_connect_ind_struct *msg_p;
        msg_p = (bt_fmp_connect_ind_struct*) construct_local_para(sizeof(bt_fmp_connect_ind_struct), TD_UL);
        msg_p->bt_addr = dev->bdAddr; 
        fmp_send_msg(MSG_ID_BT_FMP_CONNECT_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);

        cntx->bd_addr = dev->bdAddr;
        cntx->channel = channel;
        cntx->in_use = TRUE;
    }

    /* only query remote gatt service if Locator role is supported locally */
/*
    if (g_service_flags & BT_FMP_SERVICE_LOCATOR_FLAG)
    {
        FMPL_QueryGattService(cntx->channel);
    }
*/
}

void fmp_handle_disconnected(FmpChannel *channel, BtRemoteDevice *dev, BtStatus status)
{
    fmp_context_struct *cntx = fmp_find_context_by_channel(channel);
    L2capDiscReason discReason = status;

    if (cntx == NULL)
        return;

    /* If it is in reconnecting, it may need to try to reconnect one more time */
    if (cntx->bReconnect)
    {
        // TODO: try to reconnect again and return here
    }

    /* Check if it is not connected yet, then notify the connection failed instead of disconnection msg */
    if (cntx->bActive)
    {
        if (cntx->bConnTerminator)
        {
            bt_fmp_disconnect_cnf_struct *msg_p;
            msg_p = (bt_fmp_disconnect_cnf_struct*) construct_local_para(sizeof(bt_fmp_disconnect_cnf_struct), TD_UL);
            msg_p->bt_addr = dev->bdAddr; 
            msg_p->result = TRUE;
            fmp_send_msg(MSG_ID_BT_FMP_DISCONNECT_CNF, MOD_EXTERNAL, 
                (local_para_struct*)msg_p);
        }
        else
        {
            bt_fmp_disconnect_ind_struct *msg_p;
            msg_p = (bt_fmp_disconnect_ind_struct*) construct_local_para(sizeof(bt_fmp_disconnect_ind_struct), TD_UL);
            msg_p->bt_addr = dev->bdAddr; 
            fmp_send_msg(MSG_ID_BT_FMP_DISCONNECT_IND, MOD_EXTERNAL, 
                (local_para_struct*)msg_p);
        
            /* When a connection is terminated due to a link loss, a Connection Initiator should attempt 
            *   to reconnect. 
            */
            if (cntx->bConnInitiator && (discReason == L2DISC_LINK_DISCON))
            {
                cntx->bReconnect = TRUE;
                // TODO: set timer to reconnect
            }
        }
        
    }
    else
    {
        bt_fmp_connect_cnf_struct *msg_p;
        msg_p = (bt_fmp_connect_cnf_struct*) construct_local_para(sizeof(bt_fmp_connect_cnf_struct), TD_UL);
        msg_p->bt_addr = cntx->bd_addr; 
        msg_p->result = FALSE;
        fmp_send_msg(MSG_ID_BT_FMP_CONNECT_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
    }

    fmp_clear_connection(cntx);
}

void fmp_handle_gatt_query_cnf(FmpChannel *channel, BtStatus status)
{
    fmp_context_struct *cntx = fmp_find_context_by_channel(channel);
    bt_fmp_search_attribute_cnf_struct *msg_p;

    msg_p = (bt_fmp_search_attribute_cnf_struct*) construct_local_para(sizeof(bt_fmp_search_attribute_cnf_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    fmp_send_msg(MSG_ID_BT_FMP_SEARCH_ATTRIBUTE_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void fmp_handle_set_alert_level_cnf(FmpChannel *channel, FmpAlertLevel level, BtStatus status)
{
    fmp_context_struct *cntx = fmp_find_context_by_channel(channel);
    bt_fmp_set_alert_level_cnf_struct *msg_p;

    msg_p = (bt_fmp_set_alert_level_cnf_struct*) construct_local_para(sizeof(bt_fmp_set_alert_level_cnf_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 
    msg_p->level = (bt_fmp_alert_level_enum)level;
    msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
    fmp_send_msg(MSG_ID_BT_FMP_SET_ALERT_LEVEL_CNF, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void fmp_handle_set_alert_level_ind(FmpChannel *channel, FmpAlertLevel level)
{
    fmp_context_struct *cntx = fmp_find_context_by_channel(channel);
    bt_fmp_set_alert_level_ind_struct *msg_p;

    msg_p = (bt_fmp_set_alert_level_ind_struct*) construct_local_para(sizeof(bt_fmp_set_alert_level_ind_struct), TD_UL);
    msg_p->bt_addr = cntx->bd_addr; 
    msg_p->level = (bt_fmp_alert_level_enum)level;
    fmp_send_msg(MSG_ID_BT_FMP_SET_ALERT_LEVEL_IND, MOD_EXTERNAL, (local_para_struct*)msg_p);
}

void fmp_adp_callback(FmpChannel *channel, FmpCallbackParms *parms)
{
    kal_trace(BT_TRACE_BLE_PROFILES, FMP_INTADP_CALLBACK, parms->event, parms->status);

    switch (parms->event)
    {
        case FMPEVENT_OPEN:
            fmp_handle_connected(channel, parms->parm.rem_dev, parms->status);		
            break;			
        case FMPEVENT_CLOSED:
            fmp_handle_disconnected(channel, parms->parm.rem_dev, parms->status);			
            break;			
        case FMPEVENT_GATT_QUERY_CNF:
            fmp_handle_gatt_query_cnf(channel, parms->status);			
            break;			
        case FMPEVENT_SET_ALERT_LEVEL_CNF:
            fmp_handle_set_alert_level_cnf(channel, parms->parm.level, parms->status);          
            break;          
        case FMPEVENT_SET_ALERT_LEVEL_REQ:
            fmp_handle_set_alert_level_ind(channel, parms->parm.level);          
            break;          
        default:
            break;          
    }
}

void btmtk_adp_fmp_handle_activate_req(ilm_struct *ilm_ptr)
{
    bt_fmp_activate_req_struct *req_p = (bt_fmp_activate_req_struct*) ilm_ptr->local_para_ptr; 
    bt_fmp_activate_cnf_struct *msg_p;
    FmpServiceTypeFlag service_flags = 0;

    msg_p = (bt_fmp_activate_cnf_struct*) construct_local_para(sizeof(bt_fmp_activate_cnf_struct), TD_UL);

    if (req_p->service_flag & BT_FMP_SERVICE_LOCATOR_FLAG)
    {
        service_flags |= FMP_SERVICE_TYPE_LOCATOR_FLAG;
    }
    if (req_p->service_flag & BT_FMP_SERVICE_TARGET_FLAG)
    {
        service_flags |= FMP_SERVICE_TYPE_TARGET_FLAG;
    }

    if (FMP_Register(fmp_adp_callback, service_flags) == BT_STATUS_SUCCESS)

    {
        msg_p->result = TRUE;
        //g_service_flags |= service_flags;
    }
    else
    {
        msg_p->result = FALSE;
    }
    msg_p->service_flag = req_p->service_flag;
    fmp_send_msg(MSG_ID_BT_FMP_ACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);
}

void btmtk_adp_fmp_handle_deactivate_req(ilm_struct *ilm_ptr)
{
    bt_fmp_deactivate_req_struct *req_p = (bt_fmp_deactivate_req_struct*) ilm_ptr->local_para_ptr; 
    bt_fmp_deactivate_cnf_struct *msg_p;
    FmpServiceTypeFlag service_flags = 0;

    msg_p = (bt_fmp_deactivate_cnf_struct*) construct_local_para(sizeof(bt_fmp_deactivate_cnf_struct), TD_UL);

    if (req_p->service_flag & BT_FMP_SERVICE_LOCATOR_FLAG)
    {
        service_flags |= FMP_SERVICE_TYPE_LOCATOR_FLAG;
    }
    if (req_p->service_flag & BT_FMP_SERVICE_TARGET_FLAG)
    {
        service_flags |= FMP_SERVICE_TYPE_TARGET_FLAG;
    }

    if (FMP_Deregister(service_flags) == BT_STATUS_SUCCESS)

    {
        msg_p->result = TRUE;
        //g_service_flags &= ~service_flags;
    }
    else
    {
        msg_p->result = FALSE;
    }
    msg_p->service_flag = req_p->service_flag;
    fmp_send_msg(MSG_ID_BT_FMP_DEACTIVATE_CNF, MOD_EXTERNAL, (local_para_struct*) msg_p);

}

void btmtk_adp_fmp_handle_connect_req(ilm_struct *ilm_ptr)
{
    bt_fmp_connect_req_struct *req_p = (bt_fmp_connect_req_struct*) ilm_ptr->local_para_ptr;
    fmp_context_struct *cntx = fmp_find_free_context_by_addr(&req_p->bt_addr);    
    BtStatus status;

    
    if (cntx != NULL)
    {
        // it is in reconnecting?
    }
    else
    {
        if ((cntx = fmp_find_free_context()) == NULL)
        {
            goto exit;
        }
    }

    status = FMP_Open(&cntx->channel, &req_p->bt_addr);
    if ((status == BT_STATUS_PENDING) || (status == BT_STATUS_SUCCESS))
    {
        cntx->bd_addr = req_p->bt_addr;
        cntx->in_use = TRUE;
        cntx->bConnInitiator = TRUE;
        if (status == BT_STATUS_PENDING)
        {
            return;
        }
    }


exit:
    {
        bt_fmp_connect_cnf_struct *msg_p;
        msg_p = (bt_fmp_connect_cnf_struct*) construct_local_para(sizeof(bt_fmp_connect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = (status == BT_STATUS_SUCCESS) ? TRUE : FALSE;
        fmp_send_msg(MSG_ID_BT_FMP_CONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }   
}

void btmtk_adp_fmp_handle_search_attribute_req(ilm_struct *ilm_ptr)
{
    bt_fmp_search_attribute_req_struct *req_p = (bt_fmp_search_attribute_req_struct *) ilm_ptr->local_para_ptr;
    fmp_context_struct *cntx = fmp_find_context_by_addr(&req_p->bt_addr);    

    FMPL_QueryGattService(cntx->channel);
}

void btmtk_adp_fmp_handle_disconnect_req(ilm_struct *ilm_ptr)
{
    bt_fmp_disconnect_req_struct *req_p = (bt_fmp_disconnect_req_struct*) ilm_ptr->local_para_ptr;
    fmp_context_struct *cntx = fmp_find_context_by_addr(&req_p->bt_addr);    
    BtStatus status;

    if (cntx == NULL)
    {
        goto fail;
    }

    cntx->bConnTerminator = TRUE;
    status = FMP_Close(cntx->channel);
    if ((status == BT_STATUS_SUCCESS) || (status == BT_STATUS_PENDING) || (status == BT_STATUS_IN_PROGRESS))
    {
        return;     
    }

fail:
    {
        bt_fmp_disconnect_cnf_struct *msg_p;
        msg_p = (bt_fmp_disconnect_cnf_struct*) construct_local_para(sizeof(bt_fmp_disconnect_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        fmp_send_msg(MSG_ID_BT_FMP_DISCONNECT_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
}

void btmtk_adp_fmp_handle_set_alert_level_req(ilm_struct *ilm_ptr)
{
    bt_fmp_set_alert_level_req_struct *req_p = (bt_fmp_set_alert_level_req_struct*) ilm_ptr->local_para_ptr;
    fmp_context_struct *cntx = fmp_find_context_by_addr(&req_p->bt_addr);    
    BtStatus status;

    status = FMPL_SetAlertLevel(cntx->channel, req_p->level);
    if (status != BT_STATUS_PENDING)
    {
        bt_fmp_set_alert_level_cnf_struct *msg_p;
        msg_p = (bt_fmp_set_alert_level_cnf_struct*) construct_local_para(sizeof(bt_fmp_set_alert_level_cnf_struct), TD_UL);
        msg_p->bt_addr = req_p->bt_addr; 
        msg_p->result = FALSE;
        msg_p->level = req_p->level;
        fmp_send_msg(MSG_ID_BT_FMP_SET_ALERT_LEVEL_CNF, MOD_EXTERNAL, 
            (local_para_struct*)msg_p);
    }
}
/*
void btmtk_adp_fmp_handle_remove_dev_req(ilm_struct *ilm_ptr)
{
    bt_fmp_remove_dev_req_struct *req_p = (bt_fmp_remove_dev_req_struct*) ilm_ptr->local_para_ptr;

    FMPL_DeleteDevServiceDB(&req_p->bt_addr);
}*/

void btmtk_adp_fmp_handle_message(void *msg)
{
    ilm_struct *ilm_ptr = (ilm_struct *)msg;

    kal_trace(BT_TRACE_BLE_PROFILES, FMP_INTADP_HANDLE_MESSAGE, ilm_ptr->msg_id);
    switch (ilm_ptr->msg_id)
    {
        case MSG_ID_BT_FMP_ACTIVATE_REQ:
            btmtk_adp_fmp_handle_activate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_FMP_DEACTIVATE_REQ:
            btmtk_adp_fmp_handle_deactivate_req(ilm_ptr);			
            break;
        case MSG_ID_BT_FMP_CONNECT_REQ:
            btmtk_adp_fmp_handle_connect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_FMP_SEARCH_ATTRIBUTE_REQ:
            btmtk_adp_fmp_handle_search_attribute_req(ilm_ptr);          
            break;
        case MSG_ID_BT_FMP_DISCONNECT_REQ:
            btmtk_adp_fmp_handle_disconnect_req(ilm_ptr);			
            break;
        case MSG_ID_BT_FMP_SET_ALERT_LEVEL_REQ:
            btmtk_adp_fmp_handle_set_alert_level_req(ilm_ptr);          
            break;
        case MSG_ID_BT_FMP_REMOVE_DEV_REQ:
            //btmtk_adp_fmp_handle_remove_dev_req(ilm_ptr);			
            break;
        default:
            break;
    }
}

#endif
