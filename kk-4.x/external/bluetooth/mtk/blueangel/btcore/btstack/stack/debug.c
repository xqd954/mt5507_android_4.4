/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/***************************************************************************
 *
 * File:
 *     $Workfile:debug.c$ for iAnywhere Blue SDK, Version 2.1.1
 *     $Revision: #1 $
 *
 * Description:
 *     This file contains code for debugging messages.
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions, 
 * Inc.  This work contains confidential and proprietary information of 
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "bttypes.h"
#include "hcitrans.h"
#include "sys/mei.h"
#include "l2cap.h"
#include "sys/l2cap_i.h"
#include "sys/debug.h"

#if (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED)
/*
 *---------------------------------------------------------------------------
 *            General Purpose Debug functions
 *---------------------------------------------------------------------------
 */

#if 0   /* Disable all debug print functions */
const char *pBT_Status(I8 Status)
{
    switch (Status) {
    case BT_STATUS_SUCCESS:
        return "Success";
    case BT_STATUS_FAILED:
        return "Failed";
    case BT_STATUS_PENDING:
        return "Pending";
    case BT_STATUS_BUSY:
        return "Busy";
    case BT_STATUS_NO_RESOURCES:
        return "No Resources";
    case BT_STATUS_NOT_FOUND:
        return "Not Found";
    case BT_STATUS_DEVICE_NOT_FOUND:
        return "Device Not Found";
    case BT_STATUS_CONNECTION_FAILED:
        return "Connection Failed";
    case BT_STATUS_TIMEOUT:
        return "Timeout";
    case BT_STATUS_NO_CONNECTION:
        return "No Connection";
    case BT_STATUS_INVALID_PARM:
        return "Invalid parameter";
    case BT_STATUS_IN_PROGRESS:
        return "In Progress";
    case BT_STATUS_RESTRICTED:
        return "Restricted";
    case BT_STATUS_INVALID_TYPE:
        return "Invalid Type";
    case BT_STATUS_HCI_INIT_ERR:
        return "HCI Initialization Error";
    case BT_STATUS_NOT_SUPPORTED:
        return "Not Supported";
    case BT_STATUS_IN_USE:
        return "In Use";
    case BT_STATUS_SDP_CONT_STATE:
        return "SDP Continue";
    case BT_STATUS_CANCELLED:
        return "Cancelled";
    }
    return "UNKNOWN";
}


/*
 *---------------------------------------------------------------------------
 *            ME Debug functions
 *---------------------------------------------------------------------------
 */
const char *pME_Event(I8 event)
{
    switch (event) 
    {

    case BTEVENT_INQUIRY_RESULT:      return "INQUIRY_RESULT" ;   
    case BTEVENT_INQUIRY_COMPLETE:    return "INQUIRY_COMPLETE" ;
    case BTEVENT_INQUIRY_CANCELED:    return "INQUIRY_CANCELED" ;
    case BTEVENT_LINK_CONNECT_IND:    return "LINK_CONNECT_IN " ;
    case BTEVENT_SCO_CONNECT_IND:     return "SCO_CONNECT_IN" ;
    case BTEVENT_LINK_DISCONNECT:     return "LINK_DISCONNECT" ;
    case BTEVENT_LINK_CONNECT_CNF:    return "LINK_CONNECT_CNF" ;
    case BTEVENT_LINK_CON_RESTRICT:   return "LINK_CON_RESTRICT" ;
    case BTEVENT_MODE_CHANGE:         return "MODE_CHANGE";
    case BTEVENT_ACCESSIBLE_CHANGE:   return "ACCESSIBLE_CHANGE" ;
    case BTEVENT_AUTHENTICATED:       return "AUTHENTICATED" ;
    case BTEVENT_ENCRYPTION_CHANGE:   return "ENCRYPTION_CHANGE" ;
    case BTEVENT_SECURITY_CHANGE:     return "SECURITY_CHANGE" ;
    case BTEVENT_ROLE_CHANGE:         return "ROLE_CHANGE" ;
    case BTEVENT_SCO_DISCONNECT:      return "SCO_DISCONNECT" ;
    case BTEVENT_SCO_CONNECT_CNF:     return "SCO_CONNECT_CNF" ;
    case BTEVENT_ACL_DATA_NOT_ACTIVE: return "ACL_DATA_NOT_ACTIV" ;
    case BTEVENT_HCI_INIT_ERROR:      return "HCI_INIT_ERROR " ;
    case BTEVENT_HCI_INITIALIZED:     return "HCI_INITIALIZED " ;
    case BTEVENT_HCI_FATAL_ERROR:     return "HCI_FATAL_ERROR  " ;
    case BTEVENT_HCI_DEINITIALIZED:   return "HCI_DEINITIALIZED" ;
    case BTEVENT_HCI_FAILED:          return "HCI_FAILED  " ;
    case BTEVENT_HCI_COMMAND_SENT:    return "HCI_COMMAND_SENT" ;
    case BTEVENT_LINK_CREATE_CANCEL:  return "LINK_CREATE_CANCEL" ;
    case BTEVENT_NAME_RESULT:         return "NAME_RESULT " ;   
    case BTEVENT_SCO_DATA_IND:        return "SCO_DATA_IND " ;
    case BTEVENT_SCO_DATA_CNF:        return "SCO_DATA_CNF " ;
    case BTEVENT_LINK_CONNECT_REQ:    return "LINK_CONNECT_REQ " ;
    case BTEVENT_LINK_ACCEPT_RSP:     return "LINK_ACCEPT_RSP" ;
    case BTEVENT_LINK_REJECT_RSP:     return "LINK_REJECT_RSP " ;
    case BTEVENT_COMMAND_COMPLETE:    return "COMMAND_COMPLETE " ;
    case BTEVENT_SCO_CONNECT_REQ:     return "SCO_CONNECT_REQ" ;
    case BTEVENT_SCO_VSET_COMPLETE:   return "SCO_VSET_COMPLETE" ;
    case BTEVENT_SCO_STARTED:         return "SCO_STARTED " ; 
    case BTEVENT_DEVICE_SELECTED:     return "DEVICE_SELECTED " ;
    case BTEVENT_SCO_CONN_CHNG:       return "SCO_CONN_CHNG " ;
    case BTEVENT_ACCESS_APPROVED:     return "ACCESS_APPROVED" ;
    case BTEVENT_ACCESS_DENIED:       return "ACCESS_DENIED " ;
    case BTEVENT_AUTHORIZATION_REQ:   return "AUTHORIZATION_REQ" ;
    case BTEVENT_PIN_REQ:             return "PIN_REQ" ;        
    case BTEVENT_PAIRING_COMPLETE:    return "PAIRING_COMPLETE" ; 
    case BTEVENT_AUTHENTICATE_CNF:    return "AUTHENTICATE_CNF  " ;
    case BTEVENT_ENCRYPT_COMPLETE:    return "ENCRYPT_COMPLETE" ;
    case BTEVENT_SECURITY3_COMPLETE:  return "SECURITY3_COMPLETE" ;
    case BTEVENT_RETURN_LINK_KEYS:    return "RETURN_LINK_KEYS " ;
    case SDEVENT_QUERY_RSP:           return "QUERY_RSP " ; 
    case SDEVENT_QUERY_ERR:           return "QUERY_ERR" ;      
    case SDEVENT_QUERY_FAILED:        return "QUERY_FAILED  " ;  
    case BTEVENT_SELECT_DEVICE_REQ:   return "SELECT_DEVICE_REQ" ;
    case BTEVENT_DEVICE_ADDED:        return "DEVICE_ADDED" ;
    case BTEVENT_DEVICE_DELETED:      return "DEVICE_DELETED" ;
    }
    return "UNKNOWN";
}
    
/*
 *---------------------------------------------------------------------------
 *            L2CAP Debug functions
 *---------------------------------------------------------------------------
 */

const char *pL2CAP_Event(I8 Event)
{
    switch (Event) {
    case L2EVENT_CONNECT_IND:   return("Connect Ind");
    case L2EVENT_CONNECTED:     return("Connected");
    case L2EVENT_DISCON_IND:    return("Discon Ind");
    case L2EVENT_DISCONNECTED:  return("Disconnected");
    case L2EVENT_DATA_IND:      return("Data Ind");
    case L2EVENT_PACKET_HANDLED: return("Packet Handled");
    case L2EVENT_COMPLETE:      return("Complete");
    case L2EVENT_QOS_VIOLATION: return("QoS Violation");
    case L2EVENT_CONFIGURING:   return("Configuring");
    case L2EVENT_RECONFIGURED:  return("Reconfigured");

#if L2CAP_FLEXIBLE_CONFIG == XA_ENABLED
    case L2EVENT_CONFIG_IND:    return("Configuration Ind");
    case L2EVENT_CONFIG_CNF:    return("Configuration Cnf");
#endif
    }
    return "UNKNOWN";
}


const char *pL2CAP_DiscReason(U16 Reason)
{
    switch (Reason) {
    case L2DISC_REASON_UNKNOWN:
        return "Reason Unknown";
    case L2DISC_USER_REQUEST:
        return "User Request";
    case L2DISC_REQUEST_TIMEDOUT:
        return "Request Timedout";
    case L2DISC_LINK_DISCON:
        return "Link Discon";
    case L2DISC_QOS_VIOLATION:
        return "Qos Violation";
    case L2DISC_SECURITY_BLOCK:
        return "Local Security Block";
    case L2DISC_CONN_PSM_UNSUPP:
        return "Psm Unsupp";
    case L2DISC_CONN_SECURITY_BLOCK:
        return "Security Block";
    case L2DISC_CONN_NO_RESOURCES:
        return "No Resources";
    case L2DISC_CONFIG_UNACCEPT:
        return "Config Unaccept";
    case L2DISC_CONFIG_REJECT:
        return "Config Reject";
    case L2DISC_CONFIG_OPTIONS:
        return "Config Options";
    case L2DISC_MAX_RETRANS:
        return "Maximum Retransmissions Exceeded";
    case L2DISC_PROTOCOL_ERROR:
        return "Protocol Error";
    }
    return "UNKNOWN";
}

const char *pL2CAP_ConnStatus(U16 Status)
{
    switch (Status) {
    case L2CONN_ACCEPTED:
        return "Accepted";

    case L2CONN_PENDING:
        return "Pending";

    case L2CONN_REJECT_PSM_NOT_SUPPORTED:
        return "Rejected Psm Not Supported";

    case L2CONN_REJECT_SECURITY_BLOCK:
        return "Rejected Security Block";

    case L2CONN_REJECT_NO_RESOURCES:
        return "Rejected No Resources";
    }
    return "UNKNOWN";
}

const char *pL2CAP_SignalOpcode(U8 Opcode)
{
    switch (Opcode) {
    case LLC_COMMAND_REJ:
        return "Command Reject";
    case LLC_CONN_REQ:
        return "Connect Request";
    case LLC_CONN_RSP:
        return "Connect Response";
    case LLC_CONFIG_REQ:
        return "Config Request";
    case LLC_CONFIG_RSP:
        return "Config Response";
    case LLC_DISC_REQ:
        return "Disconnect Request";
    case LLC_DISC_RSP:
        return "Disconnect Response";
    case LLC_ECHO_REQ:
        return "Echo Request";
    case LLC_ECHO_RSP:
        return "Echo Response";
    case LLC_INFO_REQ:
        return "Info Request";
    case LLC_INFO_RSP:
        return "Info Response";
    }
    return "UNKNOWN Signal Opcode";
}

const char *pLLC_Event(I8 Event)
{
    switch (Event) {
    case LLC_COMMAND_REJ:   return "LLC_COMMAND_REJ";
    case LLC_CONN_REQ:      return "LLC_CONN_REQ";
    case LLC_CONN_RSP:      return "LLC_CONN_RSP";
    case LLC_CONFIG_REQ:    return "LLC_CONFIG_REQ";
    case LLC_CONFIG_RSP:    return "LLC_CONFIG_RSP";
    case LLC_DISC_REQ:      return "LLC_DISC_REQ";
    case LLC_DISC_RSP:      return "LLC_DISC_RSP";
    case LLC_ECHO_REQ:      return "LLC_ECHO_REQ";
    case LLC_ECHO_RSP:      return "LLC_ECHO_RSP";
    case LLC_INFO_REQ:      return "LLC_INFO_REQ";
    case LLC_INFO_RSP:      return "LLC_INFO_RSP";
    case LLC_RTX_EXPIRED:   return "LLC_RTX_EXPIRED";
    case LLC_ERTX_EXPIRED:  return "LLC_ERTX_EXPIRED";
    case LLC_TX_DONE:       return "LLC_TX_DONE";
    case LLC_ACCESS_RSP:    return "LLC_ACCESS_RSP";
    case LLC_ACCESS_REJ:    return "LLC_ACCESS_REJ";
    case LLC_DISCONNECT_IND:return "LM_DISC_IND";
    case LLC_QOS_VIOL:      return "LM_QOS_VIOL";
    case LLC_SEND_PACKET:   return "LLC_SEND_PACKET";
    }
    return "UNKNOWN";
}

/*
 *---------------------------------------------------------------------------
 *            HCI Debug functions
 *---------------------------------------------------------------------------
 */
const char *pHciCommand(U16 cmd)
{
    switch (cmd) {
    case HCC_INQUIRY:                
        return "INQUIRY";
    case HCC_INQUIRY_CANCEL:         
        return "INQUIRY_CANCEL";         
    case HCC_START_PERIODIC_INQ_MODE:
        return "START_PERIODIC_INQ_MODE";
    case HCC_EXIT_PERIODIC_INQ_MODE: 
        return "EXIT_PERIODIC_INQ_MODE"; 
    case HCC_CREATE_CONNECTION:      
        return "CREATE_CONNECTION";      
    case HCC_DISCONNECT:             
        return "DISCONNECT";             
    case HCC_ADD_SCO_CONNECTION:     
        return "ADD_SCO_CONNECTION";     
    case HCC_ACCEPT_CON_REQ:         
        return "ACCEPT_CON_REQ";         
    case HCC_REJECT_CON_REQ:         
        return "REJECT_CON_REQ";         
    case HCC_LINK_KEY_REQ_REPL:      
        return "LINK_KEY_REQ_REPL";      
    case HCC_LINK_KEY_REQ_NEG_REPL:  
        return "LINK_KEY_REQ_NEG_REPL";  
    case HCC_PIN_CODE_REQ_REPL:      
        return "PIN_CODE_REQ_REPL";      
    case HCC_PIN_CODE_REQ_NEG_REPL:  
        return "PIN_CODE_REQ_NEG_REPL";  
    case HCC_CHNG_CONN_PACKET_TYPE:  
        return "CHNG_CONN_PACKET_TYPE";  
    case HCC_AUTH_REQ:               
        return "AUTH_REQ";               
    case HCC_SET_CONN_ENCRYPT:       
        return "SET_CONN_ENCRYPT";       
    case HCC_CHNG_CONN_LINK_KEY:     
        return "CHNG_CONN_LINK_KEY";     
    case HCC_MASTER_LINK_KEY:        
        return "MASTER_LINK_KEY";        
    case HCC_REM_NAME_REQ:           
        return "REM_NAME_REQ";           
    case HCC_READ_REMOTE_FEATURES:   
        return "READ_REMOTE_FEATURES";   
    case HCC_READ_REMOTE_VERSION:    
        return "READ_REMOTE_VERSION";    
    case HCC_READ_CLOCK_OFFSET:      
        return "READ_CLOCK_OFFSET";      
    case HCC_HOLD_MODE:        
        return "HOLD_MODE";
    case HCC_SNIFF_MODE:       
        return "SNIFF_MODE";       
    case HCC_EXIT_SNIFF_MODE:  
        return "EXIT_SNIFF_MODE";  
    case HCC_PARK_MODE:        
        return "PARK_MODE";
    case HCC_EXIT_PARK_MODE:   
        return "EXIT_PARK_MODE";   
    case HCC_QOS_SETUP:        
        return "QOS_SETUP";        
    case HCC_ROLE_DISCOVERY:   
        return "ROLE_DISCOVERY";   
    case HCC_SWITCH_ROLE:      
        return "SWITCH_ROLE";      
    case HCC_READ_LINK_POLICY: 
        return "READ_LINK_POLICY"; 
    case HCC_WRITE_LINK_POLICY:
        return "WRITE_LINK_POLICY";
    case HCC_SET_EVENT_MASK:             
        return "SET_EVENT_MASK";             
    case HCC_RESET:                      
        return "RESET";                      
    case HCC_EVENT_FILTER:               
        return "EVENT_FILTER";               
    case HCC_FLUSH:                      
        return "FLUSH";                      
    case HCC_READ_PIN_TYPE:              
        return "READ_PIN_TYPE";              
    case HCC_WRITE_PIN_TYPE:             
        return "WRITE_PIN_TYPE";             
    case HCC_CREATE_NEW_UNIT_KEY:        
        return "CREATE_NEW_UNIT_KEY";        
    case HCC_READ_STORED_LINK_KEY:       
        return "READ_STORED_LINK_KEY";       
    case HCC_WRITE_STORED_LINK_KEY:      
        return "WRITE_STORED_LINK_KEY";      
    case HCC_DEL_STORED_LINK_KEY:        
        return "DEL_STORED_LINK_KEY";        
    case HCC_CHNG_LOCAL_NAME:            
        return "CHNG_LOCAL_NAME";            
    case HCC_READ_LOCAL_NAME:            
        return "READ_LOCAL_NAME";            
    case HCC_READ_CONN_ACCEPT_TIMEOUT:   
        return "READ_CONN_ACCEPT_TIMEOUT";   
    case HCC_WRITE_CONN_ACCEPT_TIMEOUT:  
        return "WRITE_CONN_ACCEPT_TIMEOUT";  
    case HCC_READ_PAGE_TIMEOUT:          
        return "READ_PAGE_TIMEOUT";          
    case HCC_WRITE_PAGE_TIMEOUT:         
        return "WRITE_PAGE_TIMEOUT";         
    case HCC_READ_SCAN_ENABLE:           
        return "READ_SCAN_ENABLE";           
    case HCC_WRITE_SCAN_ENABLE:          
        return "WRITE_SCAN_ENABLE";          
    case HCC_READ_PAGE_SCAN_ACTIVITY:    
        return "READ_PAGE_SCAN_ACTIVITY";    
    case HCC_WRITE_PAGE_SCAN_ACTIVITY:   
        return "WRITE_PAGE_SCAN_ACTIVITY";   
    case HCC_READ_INQ_SCAN_ACTIVITY:     
        return "READ_INQ_SCAN_ACTIVITY";     
    case HCC_WRITE_INQ_SCAN_ACTIVITY:    
        return "WRITE_INQ_SCAN_ACTIVITY";    
    case HCC_READ_AUTH_ENABLE:           
        return "READ_AUTH_ENABLE";           
    case HCC_WRITE_AUTH_ENABLE:          
        return "WRITE_AUTH_ENABLE";          
    case HCC_READ_ENCRYPT_MODE:          
        return "READ_ENCRYPT_MODE";          
    case HCC_WRITE_ENCRYPT_MODE:         
        return "WRITE_ENCRYPT_MODE";         
    case HCC_READ_CLASS_OF_DEVICE:       
        return "READ_CLASS_OF_DEVICE";       
    case HCC_WRITE_CLASS_OF_DEVICE:      
        return "WRITE_CLASS_OF_DEVICE";      
    case HCC_READ_VOICE_SETTING:         
        return "READ_VOICE_SETTING";         
    case HCC_WRITE_VOICE_SETTING:        
        return "WRITE_VOICE_SETTING";        
    case HCC_READ_AUTO_FLUSH_TIMEOUT:    
        return "READ_AUTO_FLUSH_TIMEOUT";    
    case HCC_WRITE_AUTO_FLUSH_TIMEOUT:   
        return "WRITE_AUTO_FLUSH_TIMEOUT";   
    case HCC_READ_NUM_BC_RETRANSMIT:     
        return "READ_NUM_BC_RETRANSMIT";     
    case HCC_WRITE_NUM_BC_RETRANSMIT:    
        return "WRITE_NUM_BC_RETRANSMIT";    
    case HCC_READ_HOLD_MODE_ACTIVITY:    
        return "READ_HOLD_MODE_ACTIVITY";    
    case HCC_WRITE_HOLD_MODE_ACTIVITY:   
        return "WRITE_HOLD_MODE_ACTIVITY";   
    case HCC_READ_XMIT_POWER_LEVEL:      
        return "READ_XMIT_POWER_LEVEL";      
    case HCC_READ_SCO_FC_ENABLE:         
        return "READ_SCO_FC_ENABLE";         
    case HCC_WRITE_SCO_FC_ENABLE:        
        return "WRITE_SCO_FC_ENABLE";        
    case HCC_SET_CTRLR_TO_HOST_FLOW_CTRL:
        return "SET_CTRLR_TO_HOST_FLOW_CTRL";
    case HCC_HOST_BUFFER_SIZE:           
        return "HOST_BUFFER_SIZE";           
    case HCC_HOST_NUM_COMPLETED_PACKETS: 
        return "HOST_NUM_COMPLETED_PACKETS"; 
    case HCC_READ_LINK_SUPERV_TIMEOUT:   
        return "READ_LINK_SUPERV_TIMEOUT";   
    case HCC_WRITE_LINK_SUPERV_TIMEOUT:  
        return "WRITE_LINK_SUPERV_TIMEOUT";  
    case HCC_READ_NUM_IAC:               
        return "READ_NUM_IAC";               
    case HCC_READ_CURRENT_IAC_LAP:       
        return "READ_CURRENT_IAC_LAP";       
    case HCC_WRITE_CURRENT_IAC_LAP:      
        return "WRITE_CURRENT_IAC_LAP";      
    case HCC_READ_PAGE_SCAN_PERIOD_MODE: 
        return "READ_PAGE_SCAN_PERIOD_MODE"; 
    case HCC_WRITE_PAGE_SCAN_PERIOD_MODE:
        return "WRITE_PAGE_SCAN_PERIOD_MODE";
    case HCC_READ_PAGE_SCAN_MODE:        
        return "READ_PAGE_SCAN_MODE";        
    case HCC_WRITE_PAGE_SCAN_MODE:       
        return "WRITE_PAGE_SCAN_MODE";       
    case HCC_READ_LOCAL_VERSION: 
        return "READ_LOCAL_VERSION"; 
    case HCC_READ_LOCAL_FEATURES:
        return "READ_LOCAL_FEATURES";
    case HCC_READ_BUFFER_SIZE:   
        return "READ_BUFFER_SIZE";   
    case HCC_READ_COUNTRY_CODE:  
        return "READ_COUNTRY_CODE";  
    case HCC_READ_BD_ADDR:       
        return "READ_BD_ADDR";       
    case HCC_READ_FAILED_CONTACT_COUNT: 
        return "READ_FAILED_CONTACT_COUNT"; 
    case HCC_RESET_FAILED_CONTACT_COUNT:
        return "RESET_FAILED_CONTACT_COUNT";
    case HCC_GET_LINK_QUALITY:          
        return "GET_LINK_QUALITY";          
    case HCC_READ_RSSI:                 
        return "READ_RSSI";                 
    case HCC_READ_LOOPBACK_MODE: 
        return "READ_LOOPBACK_MODE"; 
    case HCC_WRITE_LOOPBACK_MODE:
        return "WRITE_LOOPBACK_MODE";
    case HCC_ENABLE_DUT:         
        return "ENABLE_DUT";         
    case HCC_CREATE_CONNECTION_CANCEL:
        return "CREATE_CONNECTION_CANCEL";         
    case HCC_REM_NAME_REQ_CANCEL:
        return "REM_NAME_REQ_CANCEL";         
    case HCC_READ_REMOTE_EXT_FEATURES:
        return "READ_REMOTE_EXT_FEATURES";         
    case HCC_READ_LMP_HANDLE:
        return "READ_LMP_HANDLE";         
    case HCC_EXCHANGE_FIXED_INFO:
        return "EXCHANGE_FIXED_INFO";         
    case HCC_EXCHANGE_ALIAS_INFO:
        return "EXCHANGE_ALIAS_INFO";         
    case HCC_PRIVATE_PAIRING_REQ_REPL:
        return "PRIVATE_PAIRING_REQ_REPL";         
    case HCC_PRIVATE_PAIRING_REQ_NEG_REPL:
        return "PRIVATE_PAIRING_REQ_NEG_REPL";         
    case HCC_GENERATED_ALIAS:
        return "GENERATED_ALIAS";         
    case HCC_ALIAS_ADDRESS_REQ_REPL:
        return "ALIAS_ADDRESS_REQ_REPL";         
    case HCC_ALIAS_ADDRESS_REQ_NEG_REPL:
        return "ALIAS_ADDRESS_REQ_NEG_REPL";         
    case HCC_SETUP_SYNC_CONNECTION:
        return "SETUP_SYNC_CONNECTION";         
    case HCC_ACCEPT_SYNC_CON_REQ:
        return "ACCEPT_SYNC_CON_REQ";         
    case HCC_REJECT_SYNC_CON_REQ:
        return "REJECT_SYNC_CON_REQ";         
    case HCC_READ_DEFAULT_LINK_POLICY:
        return "READ_DEFAULT_LINK_POLICY";         
    case HCC_WRITE_DEFAULT_LINK_POLICY:
        return "WRITE_DEFAULT_LINK_POLICY";         
    case HCC_FLOW_SPECIFICATION:
        return "FLOW_SPECIFICATION";         
    case HCC_SET_AFH_HOST_CHAN_CLASS:
        return "SET_AFH_HOST_CHAN_CLASS";         
    case HCC_READ_INQ_SCAN_TYPE:
        return "READ_INQ_SCAN_TYPE";         
    case HCC_WRITE_INQ_SCAN_TYPE:
        return "WRITE_INQ_SCAN_TYPE";         
    case HCC_READ_INQ_MODE:
        return "READ_INQ_MODE";         
    case HCC_WRITE_INQ_MODE:
        return "WRITE_INQ_MODE";         
    case HCC_READ_PAGE_SCAN_TYPE:
        return "READ_PAGE_SCAN_TYPE";         
    case HCC_WRITE_PAGE_SCAN_TYPE:
        return "WRITE_PAGE_SCAN_TYPE";         
    case HCC_READ_AFH_CHAN_ASSESS_MODE:
        return "READ_AFH_CHAN_ASSESS_MODE";         
    case HCC_WRITE_AFH_CHAN_ASSESS_MODE:
        return "WRITE_AFH_CHAN_ASSESS_MODE";         
    case HCC_READ_ANONYMITY_MODE:
        return "READ_ANONYMITY_MODE";         
    case HCC_WRITE_ANONYMITY_MODE:
        return "WRITE_ANONYMITY_MODE";         
    case HCC_READ_ALIAS_AUTH_ENABLE:
        return "READ_ALIAS_AUTH_ENABLE";         
    case HCC_WRITE_ALIAS_AUTH_ENABLE:
        return "WRITE_ALIAS_AUTH_ENABLE";         
    case HCC_READ_ANON_ADDR_CHNG_PARM:
        return "READ_ANON_ADDR_CHNG_PARM";         
    case HCC_WRITE_ANON_ADDR_CHNG_PARM:
        return "WRITE_ANON_ADDR_CHNG_PARM";         
    case HCC_RESET_FIXED_ADDR_COUNTER:
        return "RESET_FIXED_ADDR_COUNTER";         
    case HCC_READ_LOCAL_COMMANDS:
        return "READ_LOCAL_COMMANDS";         
    case HCC_READ_LOCAL_EXT_FEATURES:
        return "READ_LOCAL_EXT_FEATURES";         
    case HCC_READ_AFH_CHANNEL_MAP:
        return "READ_AFH_CHANNEL_MAP";         
    case HCC_READ_CLOCK:
        return "READ_CLOCK";         
    }

    if ((cmd >> 8) & 0xFC) {
        return "Vendor Specific";
    }

    return "UNKNOWN Command";
}

const char *pHciEvent(U8 Event)
{
    switch (Event) {
    case HCE_INQUIRY_COMPLETE:             
        return "INQUIRY_COMPLETE";             
    case HCE_INQUIRY_RESULT:               
        return "INQUIRY_RESULT";
    case HCE_CONNECT_COMPLETE:             
        return "CONNECT_COMPLETE";             
    case HCE_CONNECT_REQUEST:              
        return "CONNECT_REQUEST";              
    case HCE_DISCONNECT_COMPLETE:          
        return "DISCONNECT_COMPLETE";          
    case HCE_AUTH_COMPLETE:                
        return "AUTH_COMPLETE";                
    case HCE_REMOTE_NAME_REQ_COMPLETE:     
        return "REMOTE_NAME_REQ_COMPLETE";     
    case HCE_ENCRYPT_CHNG:                 
        return "ENCRYPT_CHNG";                 
    case HCE_CHNG_CONN_LINK_KEY_COMPLETE:  
        return "CHNG_CONN_LINK_KEY_COMPLETE";  
    case HCE_MASTER_LINK_KEY_COMPLETE:     
        return "MASTER_LINK_KEY_COMPLETE";     
    case HCE_READ_REMOTE_FEATURES_COMPLETE:
        return "READ_REMOTE_FEATURES_COMPLETE";
    case HCE_READ_REMOTE_VERSION_COMPLETE: 
        return "READ_REMOTE_VERSION_COMPLETE"; 
    case HCE_QOS_SETUP_COMPLETE:           
        return "QOS_SETUP_COMPLETE";           
    case HCE_COMMAND_COMPLETE:             
        return "COMMAND_COMPLETE";             
    case HCE_COMMAND_STATUS:               
        return "COMMAND_STATUS";               
    case HCE_HARDWARE_ERROR:               
        return "HARDWARE_ERROR";               
    case HCE_FLUSH_OCCURRED:                
        return "FLUSH_OCCURRED";                
    case HCE_ROLE_CHANGE:                  
        return "ROLE_CHANGE";                  
    case HCE_NUM_COMPLETED_PACKETS:        
        return "NUM_COMPLETED_PACKETS";        
    case HCE_MODE_CHNG:                    
        return "MODE_CHNG";                    
    case HCE_RETURN_LINK_KEYS:             
        return "RETURN_LINK_KEYS";             
    case HCE_PIN_CODE_REQ:                 
        return "PIN_CODE_REQ";                 
    case HCE_LINK_KEY_REQ:                
        return "LINK_KEY_REQ";                
    case HCE_LINK_KEY_NOTIFY:              
        return "LINK_KEY_NOTIFY";              
    case HCE_LOOPBACK_COMMAND:             
        return "LOOPBACK_COMMAND";             
    case HCE_DATA_BUFFER_OVERFLOW:         
        return "DATA_BUFFER_OVERFLOW";         
    case HCE_MAX_SLOTS_CHNG:               
        return "MAX_SLOTS_CHNG";               
    case HCE_READ_CLOCK_OFFSET_COMPLETE:   
        return "READ_CLOCK_OFFSET_COMPLETE";   
    case HCE_CONN_PACKET_TYPE_CHNG:        
        return "CONN_PACKET_TYPE_CHNG";        
    case HCE_QOS_VIOLATION:                
        return "QOS_VIOLATION";                
    case HCE_PAGE_SCAN_MODE_CHANGE:        
        return "PAGE_SCAN_MODE_CHANGE";        
    case HCE_PAGE_SCAN_REPETITION_MODE:
        return "PAGE_SCAN_REPETITION_MODE";
    case HCE_VENDOR_SPECIFIC:
        return "VENDOR SPECIFIC";
    case HCE_BLUETOOTH_LOGO:
        return "LOGO TESTING";
    case HCE_FLOW_SPECIFICATION_COMPLETE:
        return "FLOW_SPECIFICATION_COMPLETE";
    case HCE_INQUIRY_RESULT_WITH_RSSI:
        return "INQUIRY_RESULT_WITH_RSSI";
    case HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
        return "READ_REMOTE_EXT_FEAT_COMPLETE";
    case HCE_FIXED_ADDRESS:
        return "FIXED_ADDRESS";
    case HCE_ALIAS_ADDRESS:
        return "ALIAS_ADDRESS";
    case HCE_GENERATE_ALIAS_REQ:
        return "GENERATE_ALIAS_REQ";
    case HCE_ACTIVE_ADDRESS:
        return "ACTIVE_ADDRESS";
    case HCE_ALLOW_PRIVATE_PAIRING:
        return "ALLOW_PRIVATE_PAIRING";
    case HCE_ALIAS_ADDRESS_REQ:
        return "ALIAS_ADDRESS_REQ";
    case HCE_ALIAS_NOT_RECOGNIZED:
        return "ALIAS_NOT_RECOGNIZED";
    case HCE_FIXED_ADDRESS_ATTEMPT:
        return "FIXED_ADDRESS_ATTEMPT";
    case HCE_SYNC_CONNECT_COMPLETE:
        return "SYNC_CONNECT_COMPLETE";
    case HCE_SYNC_CONN_CHANGED:
        return "SYNC_CONN_CHANGED";
    }
    return "UNKNOWN";
}

const char *pHC_Status(U8 Status)
{
    const char *hcStatus[] = {
        "Success",
        "Unknown HCI Cmd",
        "No Connection",
        "Hardware Failure",
        "Page Timeout",
        "Auth Failure",
        "Key Missing",
        "Memory Full",
        "Conn Timeout",
        "Max Num Conns",
        "Max SCO Conns",
        "Max ACL Conns",
        "Cmd Disallowed",
        "Host Rej No Resources",
        "Host Rej Security",
        "Host Rej Personal Dev",
        "Host Timeout",
        "Unsupp Feature/Parm Val",
        "Invalid HCI Parm Val",
        "Conn Term User Req",
        "Conn Term Low Resources",
        "Conn Term Power Off",
        "Conn Term Local Host",
        "Repeated Attempts",
        "Pairing Disallowed",
        "Unknown LMP PDU",
        "Unsupp Remote Feature",
        "SCO Offset Rejected",
        "SCO Interval Rejected",
        "SCO Air Mode Rejected",
        "Invalid LMP Parm",
        "Unspecified Error",
        "Unsupp LMP Parm",
        "Role Change Disallowed",
        "Lmp Response Timedout",
        "Lmp Err Transaction Coll",
        "Lmp PDU Disallowed",
        "Encryption Mode Unacceptable",
        "Unit Key Used",
        "QoS Not Supported",
        "Instant Passed",
        "Pairing w/Unit Key Unsupp"
    };

    if (Status > HC_STATUS_PAIRING_W_UNIT_KEY_UNSUPP) {
        /* Handle out of sequence BtErrorCode values */
        switch (Status) {
        case BEC_STORE_LINK_KEY_ERR:
            return "Store Link Key Error";
        case BEC_REQUEST_CANCELLED:
            return "Request cancelled";
        case BEC_NOT_FOUND:
            return "Not Found";
        default:
            return "UNKNOWN";
        }
    }

    return hcStatus[Status];
}

/*
 *---------------------------------------------------------------------------
 *            SDP Debug functions
 *---------------------------------------------------------------------------
 */
/*
 * Maps SDP Service and Protocol UUID's to strings.
 */
const char *pSdpService(U32 Service)
{
    switch (Service) {
    /* Protocol UUID's */
    case 0x0001: return "SDP";
    case 0x0002: return "UDP";
    case 0x0003: return "RFCOMM";
    case 0x0004: return "TCP";
    case 0x0005: return "TCS-BIN";
    case 0x0006: return "TCS-AT";
    case 0x0008: return "OBEX";
    case 0x0009: return "IP";
    case 0x000A: return "FTP";
    case 0x000C: return "HTTP";
    case 0x000E: return "WSP";
    case 0x000F: return "BNEP";
    case 0x0010: return "UPnP";
    case 0x0011: return "HIDP";
    case 0x0012: return "HCRP Control Channel";
    case 0x0014: return "HCRP Data Channel";
    case 0x0016: return "HCRP Notification";
    case 0x0017: return "AVCTP";
    case 0x0019: return "AVDTP";
    case 0x001B: return "CMPT";
    case 0x001D: return "UDI Control";
    case 0x0100: return "L2CAP";

    /* Service UUID's */
    case 0x1000: return "SDP Server Service Class Id";
    case 0x1001: return "Browse Group Descriptor Service Class Id";
    case 0x1002: return "Public Browse Group";
    case 0x1101: return "Serial Port";
    case 0x1102: return "LAN Access";
    case 0x1103: return "DUN";
    case 0x1104: return "IrMC Sync";
    case 0x1105: return "Object Push";
    case 0x1106: return "File Transfer";
    case 0x1107: return "IrMC Sync Command";
    case 0x1108: return "Headset";
    case 0x1109: return "Cordless Telephony";
    case 0x110A: return "Audio Source";
    case 0x110B: return "Audio Sink";
    case 0x110C: return "Audio/Video Remote Control Target";
    case 0x110D: return "Advanced Audio Distribution Profile";
    case 0x110E: return "Audio/Video Remote Control";
    case 0x110F: return "Video Conferencing Profile";
    case 0x1110: return "Intercom";
    case 0x1111: return "Fax";
    case 0x1112: return "Headset Audio Gateway";
    case 0x1113: return "WAP";
    case 0x1114: return "WAP Client";
    case 0x1115: return "PANU";
    case 0x1116: return "NAP";
    case 0x1117: return "GN";
    case 0x1118: return "Direct Printing";
    case 0x1119: return "Reference Printing";
    case 0x111A: return "Imaging";
    case 0x111B: return "Imaging Responder";
    case 0x111C: return "Imaging Automatic Archive";
    case 0x111D: return "Imaging Referencec Objects";
    case 0x111E: return "HandsFree";
    case 0x111F: return "HandsFree Audio Gateway";
    case 0x1120: return "Direct Printing Ref Objects";
    case 0x1121: return "Reflected UI";
    case 0x1122: return "Basic Printing";
    case 0x1123: return "Printing Status";
    case 0x1124: return "Human Interface Device";
    case 0x1125: return "Hardcopy Cable Replacement";
    case 0x1126: return "Hardcopy Cable Replacement Print";
    case 0x1127: return "Hardcopy Cable Replacement Scan";
    case 0x1128: return "Common ISDN Access";
    case 0x1129: return "Video Conferencing Gateway";
    case 0x112A: return "UDI Mobile Termination";
    case 0x112B: return "UDI Terminal Adaptor";
    case 0x112C: return "Audio Video";
    case 0x112D: return "SIM Access Profile";
    case 0x1200: return "PnP Information";
    case 0x1201: return "Generic Networking";
    case 0x1202: return "Generic File Transfer";
    case 0x1203: return "Generic Audio";
    case 0x1204: return "Generic Telephony";
    case 0x1205: return "UPnP Service";
    case 0x1206: return "UPnP IP Service";
    case 0x1300: return "eSDP UPnP IP PAN";
    case 0x1301: return "eSDP UPnP IP LAP";
    case 0x1302: return "eSDP UPnP L2CAP";
    }
    return "";
}

const char *pCodMajorClass(BtClassOfDevice Cod)
{
    /* Only one major device class can be specified. */
    if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_MISCELLANEOUS)
        return "Miscellaneous";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_COMPUTER)
        return "Computer";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_PHONE)
        return "Phone";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_LAN_ACCESS_POINT)
        return "Lan Access Point";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_AUDIO)
        return "Audio";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_PERIPHERAL)
        return "Peripheral";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_IMAGING)
        return "Imaging";
    else if ((Cod & COD_MAJOR_MASK) == COD_MAJOR_UNCLASSIFIED)
        return "Unclassified";

    return "";
}

/*
 * Maps Class of Device Major Service Class bits to a displayable string.
 *
 * Returns the number of bytes written.
 */
U16 BtCod2ServiceString(BtClassOfDevice Cod, char *Result, U16 Max)
{
    I16         len = 0;
    I16         len2;
    I8          i;
    const char *str;

    for (i = 0; i < 9; i++) {
        str = 0;

        switch (i) {
            /* Service Class settings are or'ed together */
        case 0:
            if ((Cod & COD_LIMITED_DISCOVERABLE_MODE) == COD_LIMITED_DISCOVERABLE_MODE)
                str = "Limited Discoverable";
            break;
        case 1:
            if ((Cod & COD_POSITIONING) == COD_POSITIONING)
                str = "Positioning";
            break;
        case 2:
            if ((Cod & COD_NETWORKING) == COD_NETWORKING)
                str = "Networking";
            break;
        case 3:
            if ((Cod & COD_RENDERING) == COD_RENDERING)
                str = "Rendering";
            break;
        case 4:
            if ((Cod & COD_CAPTURING) == COD_CAPTURING)
                str = "Capturing";
            break;
        case 5:
            if ((Cod & COD_OBJECT_TRANSFER) == COD_OBJECT_TRANSFER)
                str = "Object Transfer";
            break;
        case 6:
            if ((Cod & COD_AUDIO) == COD_AUDIO)
                str = "Audio";
            break;
        case 7:
            if ((Cod & COD_TELEPHONY) == COD_TELEPHONY)
                str = "Telephony";
            break;
        case 8:
            if ((Cod & COD_INFORMATION) == COD_INFORMATION)
                str = "Information";
            break;
        }

        if (str) {
            len2 = OS_StrLen(str);

            if ((len + len2 + 2) > Max) {
                /* An additional string won't fit. */
                if ((len + 2) < Max) {
                    if ((len > 0) && (Result[len-1] != ' ')) {
                        OS_MemCopy((U8 *)(Result + len), (U8 *)", ", 2);
                        len += 2;
                    }
                }
                /* Append and truncate the final string. */
                if ((Max - len) > 0)
                    OS_MemCopy((U8 *)Result + len, (U8 *)str, Max - len);

                OS_MemCopy((U8 *)(Result + Max - 4), (U8 *)"...", 4);
                return Max;
            }

            /* The string fits, append it. */
            if ((len > 0) && (Result[len-1] != ' ')) {
                OS_MemCopy((U8 *)(Result + len), (U8 *)", ", 2);
                len += 2;
            }
            /* Copy in, but don't count null-term. */
            OS_MemCopy((U8 *)(Result + len), (U8 *)str, len2 + 1);    
            len = (I16)(len + len2);
        }
    }
    return (U16)(len ? len + 1 : 0);    /* +1 for null terminator */
}


/*
 *---------------------------------------------------------------------------
 *            BT-SDK Statistics Debug functions
 *---------------------------------------------------------------------------
 */
#if XA_STATISTICS == XA_ENABLED
#include "sys/btstats.h"

void BTSTAT_Report(const BtStatistics *Stats, void (*PrintMsg)(const char *,...))
{
    Assert (Stats != 0);

    if (PrintMsg == 0)
        PrintMsg = OS_Report;

    PrintMsg("*\n");
    PrintMsg("* %-46s  %5s %10s\n", "Allocation statistics:", "Used", "Allocated");
    PrintMsg("*   HCI:\n");
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of ACL receive buffers", Stats->hci_num_acl_buffers, HCI_NUM_ACL_BUFFERS);
    PrintMsg("* %45s:  %5d\n",     "Count ACL receive buffer not available", Stats->hci_num_no_acl_rxbuf);
    PrintMsg("* %45s:  %5d %7d\n", "Peak ACL receive buffer allocation", Stats->hci_acl_buff_size, HCI_ACL_DATA_SIZE);
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of SCO receive buffers", Stats->hci_num_sco_buffers, HCI_NUM_SCO_BUFFERS);
    PrintMsg("* %45s:  %5d %7d\n", "Peak SCO receive buffer allocation", Stats->hci_sco_buff_size, HCI_SCO_DATA_SIZE);
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of Event receive buffers", Stats->hci_num_event_buffers, HCI_NUM_EVENTS);
    PrintMsg("* %45s:  %5d %7s\n", "Peak number of queued ACL BtPackets", Stats->hci_num_acl_bt_packets, "N/A");
    PrintMsg("* %45s:  %5d %7s\n", "Peak number of queued SCO BtPackets", Stats->hci_num_sco_bt_packets, "N/A");
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of queued HciPackets", Stats->hci_num_hci_packets, HCI_NUM_PACKETS);
    PrintMsg("* %45s:  %5d %7s\n", "Peak size of outgoing ACL data", Stats->hci_max_tx_packet_size, "N/A");

    PrintMsg("*\n");
    PrintMsg("*   L2CAP:\n");
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of registered protocols", Stats->l2cap_num_protocols, L2CAP_NUM_PROTOCOLS);
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of registered groups", Stats->l2cap_num_groups, L2CAP_NUM_GROUPS);
    PrintMsg("* %45s:  %5s %7s\n", "Ping service", (Stats->l2cap_ping_support?"Yes":"No"), (L2CAP_PING_SUPPORT==XA_ENABLED?"Yes":"No"));
    PrintMsg("* %45s:  %5s %7s\n", "Flexible configuration service", (Stats->l2cap_flexible_config?"Yes":"No"), (L2CAP_FLEXIBLE_CONFIG==XA_ENABLED?"Yes":"No"));
    PrintMsg("* %45s:  %5d %7s\n", "Number of times RTX timer expired", Stats->l2cap_rtx_timeout, "N/A");
    PrintMsg("* %45s:  %5d %7s\n", "Number of times E-RTX timer expired", Stats->l2cap_ertx_timeout, "N/A");
    PrintMsg("* %45s:  %5d %7d\n", "Maximum receive PDU size", Stats->l2cap_mtu, L2CAP_MTU);
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of channels", Stats->l2cap_num_channels, L2CAP_NUM_CHANNELS);
    PrintMsg("* %45s:  %5s %7s\n", "Deregister function", (Stats->l2cap_deregister_func?"Yes":"No"), (L2CAP_DEREGISTER_FUNC==XA_ENABLED?"Yes":"No"));
    PrintMsg("* %45s:  %5d %7d\n", "Peak number of signalling packets", Stats->l2cap_num_signal_packets, L2CAP_NUM_SIGNAL_PACKETS);
    PrintMsg("* %45s:  %5d %7d\n", "Number of times Retransmit timer expired", Stats->l2cap_retrans_timeout, "N/A");
    PrintMsg("* %45s:  %5d %7d\n", "Number of times Monitor timer expired", Stats->l2cap_monitor_timeout, "N/A");

    PrintMsg("*\n");
    PrintMsg("* %-46s  %5s %9s %7s\n", "Performance statistics:", "Peak", "Average", "Samples");
    PrintMsg("*   HCI:\n");

    if ((Stats->hci_tx_timer.samples > 0) &&
        (Stats->hci_tx_timer.peak_latency > 0) &&
        (Stats->hci_tx_timer.total_time >= Stats->hci_tx_timer.samples)) {
        PrintMsg("* %45s: %6d %7d %7d\n", "Data PDU Transmit latency (ms)", (Stats->hci_tx_timer.peak_latency * MS_TO_TICKS(1000))/1000, ((Stats->hci_tx_timer.total_time / Stats->hci_tx_timer.samples) * MS_TO_TICKS(1000))/1000, Stats->hci_tx_timer.samples);
        if (Stats->hci_tx_timer.total_bytes) {
            PrintMsg("* %45s: %6ld\n", "Max Theoretical TX Throughput (bytes/sec)", Stats->hci_tx_timer.total_bytes / Stats->hci_tx_timer.total_time * MS_TO_TICKS(1000));
        }
    }

    if ((Stats->hci_rx_timer.samples > 0) &&
        (Stats->hci_rx_timer.peak_latency > 0) &&
        (Stats->hci_rx_timer.total_time > Stats->hci_rx_timer.samples)) {
        PrintMsg("* %45s: %6d %7d %7d\n", "Data PDU Receive latiency (ms)", (Stats->hci_rx_timer.peak_latency * MS_TO_TICKS(1000))/1000, ((Stats->hci_rx_timer.total_time / Stats->hci_rx_timer.samples) * MS_TO_TICKS(1000))/1000, Stats->hci_rx_timer.samples);
        PrintMsg("* %45s: %6d\n", "Max Theoretical RX Throughput (bytes/sec)", Stats->hci_rx_timer.total_bytes * MS_TO_TICKS(1000) / Stats->hci_rx_timer.total_time);
    }

    PrintMsg("*\n");
    PrintMsg("*   L2CAP:\n");
    if ((Stats->l2cap_tx_timer.samples > 0) &&
        (Stats->l2cap_tx_timer.total_time > 0)) {
        PrintMsg("* %45s: %6d %7d %7d\n", "Data PDU Transmit latiency (ms)", (Stats->l2cap_tx_timer.peak_latency * MS_TO_TICKS(1000))/1000, ((Stats->l2cap_tx_timer.total_time / Stats->l2cap_tx_timer.samples) * MS_TO_TICKS(1000))/1000, Stats->l2cap_tx_timer.samples);
        PrintMsg("* %45s: %6d\n", "Max Theoretical TX Throughput (bytes/sec)", Stats->l2cap_tx_timer.total_bytes * MS_TO_TICKS(1000) / Stats->l2cap_tx_timer.total_time);
    }

    PrintMsg("*\n");
    PrintMsg("*   RFCOMM:\n");
    if (Stats->rfcomm_tx_timer.samples > 0) {
        PrintMsg("* %45s: %6d %7d %7d\n", "Data PDU Transmit latiency (ms)", (Stats->rfcomm_tx_timer.peak_latency * MS_TO_TICKS(1000))/1000, ((Stats->rfcomm_tx_timer.total_time / Stats->rfcomm_tx_timer.samples) * MS_TO_TICKS(1000))/1000, Stats->rfcomm_tx_timer.samples);
        PrintMsg("* %45s: %6d\n", "Max Theoretical TX Throughput (bytes/sec)", Stats->rfcomm_tx_timer.total_bytes * MS_TO_TICKS(1000) / Stats->rfcomm_tx_timer.total_time);
    }
    PrintMsg("\n");
}

#endif /* XA_STATISTICS == XA_ENABLED */
#endif /* 0 */ /* Disable all debug print functions */
#endif /* (XA_DEBUG == XA_ENABLED) || (XA_DEBUG_PRINT == XA_ENABLED) */

