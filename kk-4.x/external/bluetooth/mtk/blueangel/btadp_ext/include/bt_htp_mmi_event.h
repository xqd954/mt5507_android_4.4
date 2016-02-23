#ifndef __BT_HTP_MMI_EVENT_H__
#define __BT_HTP_MMI_EVENT_H__

/* Event sent to application layer definition */

EVENT_HTPC_MMI_ACTIVATE_CNF,
EVENT_HTPC_MMI_DEACTIVATE_CNF,
EVENT_HTPC_MMI_CONNECT_CNF,
EVENT_HTPC_MMI_SEARCH_ATTIBUTE_CNF,
EVENT_HTPC_MMI_DISCONNECT_CNF,
EVENT_HTPC_MMI_SET_CONFIG_CNF,      // client characteristic configuration write cnf
EVENT_HTPC_MMI_SET_INTERVAL_VALUE_CNF,  // measurement interval value write cnf
EVENT_HTPC_MMI_DISCONNECT_IND,
EVENT_HTPC_MMI_TEMPERATURE_IND,     // measurement temperature indication 
EVENT_HTPC_MMI_INTERMEDIATE_TEMPERATURE_IND,    // intermediate temperature notification
EVENT_HTPC_MMI_SET_INTERVAL_VALUE_IND,      // measurement interval value indication

EVENT_HTPT_MMI_ACTIVATE_CNF,
EVENT_HTPT_MMI_DEACTIVATE_CNF,
EVENT_HTPT_MMI_DISCONNECT_CNF,
EVENT_HTPT_MMI_SEND_TEMPERATURE_CNF,        // measurement temperature indication cnf
EVENT_HTPT_MMI_SEND_INTERMEDIATE_TEMPERATURE_CNF,   // intermediate temperature notification cnf
EVENT_HTPT_MMI_SET_INTERVAL_VALUE_CNF,      // measurement interval value reset cnf
EVENT_HTPT_MMI_SEND_INTERVAL_VALUE_INDICATION_CNF,      // measurement interval value indicaiton cnf
EVENT_HTPT_MMI_CONNECT_IND,
EVENT_HTPT_MMI_DISCONNECT_IND,
EVENT_HTPT_MMI_SET_CONFIG_IND,          // client characteristic configuration write request
EVENT_HTPT_MMI_SET_INTERVAL_VALUE_IND,      // measurement interval value write request


#endif
