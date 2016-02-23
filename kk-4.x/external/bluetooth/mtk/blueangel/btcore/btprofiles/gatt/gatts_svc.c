/*****************************************************************************
 *
 * Filename:
 * ---------
 * gatts_svc.c
 *
 * Project:
 * --------
 *   
 *
 * Description:
 * ------------
 *   This file is for MTK Bluetooth GATTS Profile.
 *
 * Author:
 * -------
 * SH Lai
 *
 ****************************************************************************/
#if defined(__BT_GATTS_PROFILE__)

#include "sdp.h"
#include "attdb.h"
#include "attdbmain.h"
#include "gattci.h"
#include "gattsi.h"
#include "gattcb.h"
#include "gatt_util.h"
#include "ble_utils.h"
//#define MIN_GATTS_HANDLE    256
//#define MAX_GATTS_HANDLE    1024


/* Upper layer attribute permissions */
#define GATT_PERM_READ              (1 << 0) /* bit 0 */
#define GATT_PERM_READ_ENCRYPTED    (1 << 1) /* bit 1 */
#define GATT_PERM_READ_ENC_MITM     (1 << 2) /* bit 2 */
#define GATT_PERM_WRITE             (1 << 4) /* bit 4 */
#define GATT_PERM_WRITE_ENCRYPTED   (1 << 5) /* bit 5 */
#define GATT_PERM_WRITE_ENC_MITM    (1 << 6) /* bit 6 */
#define GATT_PERM_WRITE_SIGNED      (1 << 7) /* bit 7 */
#define GATT_PERM_WRITE_SIGNED_MITM (1 << 8) /* bit 8 */
//typedef U16 tGATT_PERM;

#define GATT_ENCRYPT_KEY_SIZE_MASK  (0xF000) /* the MS nibble of tGATT_PERM; key size 7=0; size 16=9 */

#define GATT_READ_ALLOWED           (GATT_PERM_READ | GATT_PERM_READ_ENCRYPTED | GATT_PERM_READ_ENC_MITM)
#define GATT_READ_AUTH_REQUIRED     (GATT_PERM_READ_ENCRYPTED)
#define GATT_READ_MITM_REQUIRED     (GATT_PERM_READ_ENC_MITM)
#define GATT_READ_ENCRYPTED_REQUIRED   (GATT_PERM_READ_ENCRYPTED | GATT_PERM_READ_ENC_MITM)


#define GATT_WRITE_ALLOWED          (GATT_PERM_WRITE | GATT_PERM_WRITE_ENCRYPTED | GATT_PERM_WRITE_ENC_MITM | \
                                     GATT_PERM_WRITE_SIGNED | GATT_PERM_WRITE_SIGNED_MITM)

#define GATT_WRITE_AUTH_REQUIRED    (GATT_PERM_WRITE_ENCRYPTED | GATT_PERM_WRITE_SIGNED)

#define GATT_WRITE_MITM_REQUIRED    (GATT_PERM_WRITE_ENC_MITM | GATT_PERM_WRITE_SIGNED_MITM)

#define GATT_WRITE_ENCRYPTED_PERM   (GATT_PERM_WRITE_ENCRYPTED | GATT_PERM_WRITE_ENC_MITM)

#define GATT_WRITE_SIGNED_PERM      (GATT_PERM_WRITE_SIGNED | GATT_PERM_WRITE_SIGNED_MITM)



static U8 GattdbCallback(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw);

/************************************************
*   Global variables
*************************************************/
const BD_ADDR gatts_fake_addr = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
extern ListEntry g_BleServiceDBTable;

/************************************************
*   Structures
*************************************************/

/************************************************
*   static utilities
*************************************************/
static BleLocalServiceNode *allocLocalSvcNode(void){
    BleLocalServiceNode *node = (BleLocalServiceNode*)gatt_malloc(sizeof(BleLocalServiceNode));
    if(node){
        InitializeListHead(&node->svc_node.include_list);
        InitializeListHead(&node->svc_node.char_list);
    }
    return node;
}

static BleLocalCharNode *allocLocalCharNode(void){
    BleLocalCharNode *node = (BleLocalCharNode*)gatt_malloc(sizeof(BleLocalCharNode));
    if(node) InitializeListHead(&node->char_node.char_desc_list);
    return node;
}

static BleLocalServiceNode *findServiceNodeByHandle(BleServiceDB *db, ATT_HANDLE handle){
    BleLocalServiceNode *svc = (BleLocalServiceNode*)&db->service_list;
    while((svc = (BleLocalServiceNode*)GetNextNode(&svc->svc_node.node)) != (BleLocalServiceNode*)&db->service_list){
        if(svc->svc_node.service.start_handle == handle)
            return svc;
        if(handle <= svc->svc_node.service.end_handle)
            return NULL;
    }
    return NULL;
}

static BleLocalServiceNode *findContainedServiceNode(BleServiceDB *db, ATT_HANDLE handle){
    OS_Report("findContainedServiceNode(db=%p, handle=%u)", db, handle);
    if(db){    
        BleLocalServiceNode *svc = (BleLocalServiceNode*)&db->service_list;
        while((svc = (BleLocalServiceNode*)GetNextNode(&svc->svc_node.node)) != (BleLocalServiceNode*)&db->service_list){
            OS_Report("svc=%p, start=%u, end=%u", svc, svc->svc_node.service.start_handle, svc->svc_node.service.end_handle);
            if(handle >= svc->svc_node.service.start_handle){
                if(handle <= svc->svc_node.service.end_handle){
                    return svc;
                }
            }
        }
    }
    return NULL;
}

BleIncludeServiceNode *findInclServiceNodeByHandle(BleServiceDB *db, ATT_HANDLE handle){
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);
    if(svc){
        BleIncludeServiceNode *incl_svc = (BleIncludeServiceNode*)&svc->svc_node.include_list;
        while((incl_svc = (BleIncludeServiceNode*)GetNextNode(&incl_svc->node)) != (BleIncludeServiceNode*)&svc->svc_node.include_list){
            if(incl_svc->include_service.handle == handle) return incl_svc;
            if(handle < incl_svc->include_service.handle) return NULL;
        }
    }
    return NULL;
}

BleCharNode *findCharNodeByHandle(BleServiceDB *db, ATT_HANDLE handle){
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);
    if(svc){
        BleCharNode *char_node = (BleCharNode*)&svc->svc_node.char_list;
        while((char_node = (BleCharNode*)GetNextNode(&char_node->node)) != (BleCharNode*)&svc->svc_node.char_list){
            if(char_node->characteristic.handle == handle) return char_node;
            if(handle < char_node->characteristic.handle) return NULL;
        }
    }
    return NULL;
}

static BleCharDescNode *findDescrNode(BleCharNode *char_node, ATT_HANDLE descr_handle){
    BleCharDescNode *descr = (BleCharDescNode*)&char_node->char_desc_list;
    OS_Report("findDescrNode(char=%p, handle=%u)", char_node, descr_handle);
    while((descr = (BleCharDescNode*)GetPrevNode(&descr->node)) != (BleCharDescNode*)&char_node->char_desc_list){
        OS_Report("descr=%p, descr handle=%u", descr, descr->descriptor.handle);
        if(descr_handle == descr->descriptor.handle) return descr;
        else if(descr_handle > descr->descriptor.handle) return NULL;
    }
    return NULL;
}

BleCharDescNode *findDescrNodeByHandle(BleServiceDB *db, ATT_HANDLE handle){
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);
    if(svc){
        BleCharNode *char_node = (BleCharNode*)GetNextNode(&svc->svc_node.char_list), *next_char = NULL;
        while(char_node != (BleCharNode*)&svc->svc_node.char_list){
            if( handle < char_node->characteristic.handle){
                return NULL;
            }else{
                next_char = (BleCharNode*)GetNextNode(&char_node->node);
                if(next_char == (BleCharNode*)&svc->svc_node.char_list 
                   || handle < next_char->characteristic.handle){
                   return findDescrNode(char_node, handle);
                }
                char_node = next_char;
            }
        }
    }
    return NULL;
}

/* return 0 : not found, 1 : char value, 2 descr  */
U8 getValueNodeByHandle(BleServiceDB *db, ATT_HANDLE handle, ListEntry **node){
    //BleServiceDB *db = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleServiceNode *svc = (BleServiceNode*)findContainedServiceNode(db, handle);
    OS_Report("getValueNodeByHandle : handle=%u", handle);
    if(svc){
        BleCharNode *char_node = (BleCharNode*)&svc->char_list;
        while((char_node = (BleCharNode*)GetNextNode(&char_node->node)) != (BleCharNode*)&svc->char_list){
            OS_Report("char_node = %p, handle=%u, value handle=%u", char_node, char_node->characteristic.handle, char_node->characteristic.value_handle);
            if(char_node->characteristic.value_handle == handle){
                *node = &char_node->node;
                return 1;
            }else if(handle > char_node->characteristic.value_handle){
                BleCharDescNode *descr_node = findDescrNode(char_node, handle);
                if(descr_node){
                    *node = &descr_node->node;
                    return 2;
                }
            }else{
                break;
            }
        }
    }
    return 0;
}

static void converPermission(U32 perm, U8 *data){
    OS_MemSet(data, 0, 3);
    // Set read / write permission
    OS_Report("converPermission(perm=0x%X)", perm);
    if(perm & GATT_READ_ALLOWED){
        if(perm & GATT_WRITE_ALLOWED){
            OS_Report("Read/Write");
            *data = ATT_PERMISSIONS_READ_WRITE;
        }else{
            OS_Report("Read Only");
            *data = ATT_PERMISSIONS_READ_ONLY;
        }
    }else{
        if(perm & GATT_WRITE_ALLOWED){
            OS_Report("Write Only");
            *data = ATT_PERMISSIONS_WRITE_ONLY;
        }else{
            OS_Report("No Read/Write");
            *data = ATT_PERMISSIONS_NO_READ_WRITE;
        }
    }
    data++;
    // Set read security
    if(perm & GATT_READ_ENCRYPTED_REQUIRED){
        OS_Report("Read Encrypt required");
        *data = ATT_SECURITY_READ_ENCRYPTION;
    }else if(perm & GATT_READ_AUTH_REQUIRED){
        OS_Report("Read auth required");
        *data = ATT_SECURITY_READ_AUTHENTICATION;
    }
    if(((perm & GATT_ENCRYPT_KEY_SIZE_MASK) >> 12)){
        *data |= 6;
        OS_Report("Read encryp key size %u required", *data);
    }
    data++;
    // Set write security
    // TODO: Need to add signed permission
    if(perm & GATT_WRITE_ENCRYPTED_PERM || perm & GATT_WRITE_SIGNED_PERM){
        OS_Report("Write Encrypt required");
        *data = ATT_SECURITY_WRITE_ENCRYPTION;
    }else if(perm & GATT_WRITE_AUTH_REQUIRED){
        OS_Report("Write auth required");
        *data = ATT_SECURITY_WRITE_AUTHENTICATION;
    }
    if(((perm & GATT_ENCRYPT_KEY_SIZE_MASK) >> 12)){
        *data |= 6;
        OS_Report("Write encryp key size %u required", *data);
    }    
}

static U16 addDescrRecord(BleLocalCharDescNode *descr_node, U8 *data){
    U16 hdr_size = 11;
    BleCharDesc *attr = &descr_node->desc_node.descriptor;
    U8 perm[3] = 
    {
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0)
    };
    if(attr->desc_uuid_size != BT_UUID16_SIZE) hdr_size += 14;    
    if(data){
        StoreBE16(data, attr->handle);
        data[2] = (attr->desc_uuid_size == BT_UUID16_SIZE) ? BT_UUID16_SIZE : BT_UUID128_SIZE;
        data[3] = BT_ATT_FIX_VARIABLE_IN_UPPER_AP;
        StoreBE16(data+4, 0);
        converPermission(descr_node->permission, perm);
        if(attr->desc_uuid_size == BT_UUID16_SIZE){
            OS_MemCopy(data+6, attr->desc_uuid.uuid16, 2);
            OS_MemCopy(data+8, perm, 3);
        }else{
            OS_MemCopy(data+6, attr->desc_uuid.uuid128, 16);
            OS_MemCopy(data+22, perm, 3);
        }
    }
    OS_Report("addDescrRecord(%p, data=%p) : size=%u", descr_node, data, hdr_size);
    return hdr_size;
}

static U16 addCharRecord(BleLocalCharNode *char_node, U8 *data){
    BleChar *attr = &char_node->char_node.characteristic;    
    U16 hdr_size = 11, val_hdr_size = (attr->value_uuid_size == BT_UUID16_SIZE) ? 11 : 25;
    U16 char_size = (attr->value_uuid_size == BT_UUID16_SIZE) ? 5 : 19;
    U8 perm[3] = 
    {
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0)
    };
    if(data){
        // Add char declaration
        StoreBE16(data, attr->handle);
        data[2] = BT_UUID16_SIZE;
        data[3] = BT_ATT_CONST_VARIABLE;
        StoreBE16(data+4, char_size);
        StoreBE16(data+6, BT_UUID_GATT_TYPE_CHARACTERISTIC);
        OS_MemCopy(data+8, perm, 3);
        data[11] = attr->properties;
        StoreLE16(data+12, attr->value_handle);
        if(attr->value_uuid_size == BT_UUID16_SIZE){
            OS_MemCopy(data+14, attr->value_uuid.uuid16, 2);
        }else{
            OS_MemCopy(data+14, attr->value_uuid.uuid128, 16);
        }
        // Add char value
        data += hdr_size+char_size;
        //if(attr->value_uuid_size != BT_UUID16_SIZE) hdr_size += 14;
        StoreBE16(data, attr->value_handle);
        data[2] = (attr->value_uuid_size == BT_UUID16_SIZE) ? BT_UUID16_SIZE : BT_UUID128_SIZE;
        data[3] = BT_ATT_FIX_VARIABLE_IN_UPPER_AP;
        StoreBE16(data+4, 0);
        converPermission(char_node->permission, perm);
        if(attr->value_uuid_size == BT_UUID16_SIZE){
            OS_MemCopy(data+6, attr->value_uuid.uuid16, 2);
            OS_MemCopy(data+8, perm, 3);
        }else{
            OS_MemCopy(data+6, attr->value_uuid.uuid128, 16);
            OS_MemCopy(data+22, perm, 3);
        }
    }
    OS_Report("addCharRecord(%p, data=%p) : size=%u", char_node, data, (hdr_size + char_size + val_hdr_size));
    return (hdr_size + char_size + val_hdr_size);
}

static U16 addInclServiceRecord(BleIncludeServiceNode *incl_svc, U8 *data){
    U16 hdr_size = 11;
    BleIncludeService *attr = &incl_svc->include_service;
    U16 record_size = (attr->uuid_size == BT_UUID16_SIZE) ? 6 : 20;
    U8 perm[3] = 
    {
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0)
    };
    if(data){
        StoreBE16(data, attr->handle);
        data[2] = BT_UUID16_SIZE;
        data[3] = BT_ATT_CONST_VARIABLE;
        StoreBE16(data+4, record_size);
        StoreBE16(data+6, BT_UUID_GATT_TYPE_INCLUDE);
        // Set 3 bytes permission
        OS_MemCopy(data+8, perm, 3);
        StoreLE16(data+11, attr->include_service_handle);
        StoreLE16(data+13, attr->end_group_handle);
        if(attr->uuid_size == BT_UUID16_SIZE){
            OS_MemCopy(data+15, attr->uuid.uuid16, 2);
        }else{
            OS_MemCopy(data+15, attr->uuid.uuid128, 16);
        }
    }
    OS_Report("addInclServiceRecord(%p,data=%p) : size=%u", incl_svc, data, (hdr_size+record_size));
    return (hdr_size+record_size);
}

static U16 addServiceRecord(BleLocalServiceNode *svc, U8 *data){
    U16 hdr_size = 11;
    BleService *attr = &svc->svc_node.service;
    U16 record_size = (attr->uuid_size==BT_UUID16_SIZE) ? 2 : 16;
    U8 perm[3] = 
    {
        ATT_PERMINSIONS(ATT_PERMISSIONS_READ_ONLY, ATT_SECURITY_READ_NOT_REQUIRED, 0, ATT_SECURITY_WRITE_NOT_REQUIRED, 0)
    };
    if(data){
        StoreBE16(data, attr->start_handle);
        data[2] = BT_UUID16_SIZE;
        data[3] = BT_ATT_CONST_VARIABLE;
        StoreBE16(data+4, record_size);
        StoreBE16(data+6, BT_UUID_GATT_TYPE_PRIMARY_SERVICE);
        // Set 3 bytes permission
        OS_MemCopy(data+8, perm, 3);
        if(attr->uuid_size == BT_UUID16_SIZE){
            OS_MemCopy(data+11, attr->uuid.uuid16, 2);
        }else{
            OS_MemCopy(data+11, attr->uuid.uuid128, 16);
        }
    }
    OS_Report("addServiceRecord(%p, data=%p) : size=%u", svc, data, (hdr_size+record_size));
    return (hdr_size+record_size);
}

static void freeService(BleServiceNode *service_node)
{
    BleIncludeServiceNode *include_node;
    BleCharNode *char_node; 
    BleCharDescNode *char_desc_node;

    while (!IsListEmpty(&service_node->include_list))
    {
        include_node = (BleIncludeServiceNode *)RemoveHeadList(&service_node->include_list);
        gatt_free(include_node);
    }

    while (!IsListEmpty(&service_node->char_list))
    {
        char_node = (BleCharNode *)RemoveHeadList(&service_node->char_list);
        while (!IsListEmpty(&char_node->char_desc_list))
        {
            char_desc_node = (BleCharDescNode *)RemoveHeadList(&char_node->char_desc_list);
            gatt_free(char_desc_node->descriptor.desc);
            gatt_free(char_desc_node);
        }
        gatt_free(char_node->characteristic.value);         
        gatt_free(char_node);
    }
    RemoveEntryList(&service_node->node);
    gatt_free(service_node);
}



/* return size of the record */
static U16 composeAttRecord(BleLocalServiceNode *svc_node){
    U8 loop = 0;
    BleServiceNode *svc = &svc_node->svc_node;
    ListEntry *entry, *entry2, *desc_list;
    U8 *ptr = NULL;
    U16 record_size = 0, size;
    OS_Report("composeAttRecord(%p)", svc_node);
    /* Calculate size */
    while(loop < 2){
        if(loop){
            svc_node->pSvcRecord = ptr = gatt_malloc(record_size);
            if(ptr == NULL){
                svc_node->svcRecordSize = 0;
                break;
            }
            svc_node->svcRecordSize = record_size;
        }
        record_size = 0;
        // Service declaration
        size = addServiceRecord((BleLocalServiceNode*)svc, ptr);
        record_size += size;
        if(ptr) ptr += size;
        // Include list
        entry = &svc->include_list;
        while((entry = GetNextNode(entry)) != &svc->include_list){
            size = addInclServiceRecord((BleIncludeServiceNode*)entry, ptr);
            record_size += size;
            if(ptr) ptr += size;
        }
        // Add char declaration
        entry = &svc->char_list;
        while((entry = GetNextNode(entry)) != &svc->char_list){
            size = addCharRecord((BleLocalCharNode*)entry, ptr);
            record_size += size;
            if(ptr) ptr += size;
            desc_list = &((BleCharNode*)entry)->char_desc_list;
            entry2 = desc_list;
            while((entry2 = GetNextNode(entry2)) != desc_list){
                size = addDescrRecord((BleLocalCharDescNode*)entry2, ptr);
                record_size += size;
                if(ptr) ptr += size;
            }
        }
        loop++;
    }
    svc_node->svcRecordSize = record_size;
    return record_size;
}

static U16 mountAttRecordCallback(BleServiceNode *svc_node, U8 mount){
    ListEntry *charEntry, *descrEntry, *charList, *descList;
    U16 count = 0;
    charEntry = charList = &svc_node->char_list;
    OS_Report("mountAttRecordCallback(svc=%p, mount=%u)", svc_node, mount);
    while((charEntry = GetNextNode(charEntry)) != charList){
        OS_Report("mount/unmount char %p, handle %u", charEntry, ((BleCharNode*)charEntry)->characteristic.value_handle);
        if(mount) ATTDB_AddRecordCB(((BleCharNode*)charEntry)->characteristic.value_handle, GattdbCallback);
        else ATTDB_RemoveRecordCB(((BleCharNode*)charEntry)->characteristic.value_handle);
        count++;
        descrEntry = descList = &((BleCharNode*)charEntry)->char_desc_list;
        while((descrEntry = GetNextNode(descrEntry)) != descList){
            OS_Report("mount/unmount descr %p, handle %u", descrEntry, ((BleCharDescNode*)descrEntry)->descriptor.handle);
            if(mount) ATTDB_AddRecordCB(((BleCharDescNode*)descrEntry)->descriptor.handle, GattdbCallback);
            else ATTDB_RemoveRecordCB(((BleCharDescNode*)descrEntry)->descriptor.handle);
            count++;
        }
    }
    return count;
}


static U8 attSrcClassId16[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),                  /* Data Element Sequence, 6 bytes */
    SDP_UUID_16BIT(0)   /* Human Interface Device */
};


#define EMPTY_128_UUID 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

static U8 attSrcClassId128[] = 
{
    SDP_ATTRIB_HEADER_8BIT(17),                  /* Data Element Sequence, 6 bytes */
    SDP_UUID_128BIT(EMPTY_128_UUID)   /* Human Interface Device */
};

static U8 attProtoDescList[] = 
{
    SDP_ATTRIB_HEADER_8BIT(19), /* Data element sequence, 13 bytes */

    /*
     * Each element of the list is a Protocol descriptor which is a
     * data element sequence. The first element is L2CAP which only
     * has a UUID element.
     */
    SDP_ATTRIB_HEADER_8BIT(6),          /* Data element sequence for L2CAP, 6 bytes */
    SDP_UUID_16BIT(PROT_L2CAP),         /* L2CAP UUID */
    SDP_UINT_16BIT(BT_PSM_ATT),         /* ATT PSM */

    /*
     * Next protocol descriptor in the list is ATT.
     */
    SDP_ATTRIB_HEADER_8BIT(9),  /* Data element sequence for ATT, 2 bytes */
    SDP_UUID_16BIT(PROT_ATT),    /* ATT UUID */
    SDP_UINT_16BIT(0),        /* start attribute handle */
    SDP_UINT_16BIT(0)    /* end attribute handle */
};

static const U8 publicBrowseGroup[] = 
{
    SDP_ATTRIB_HEADER_8BIT(3),              /* 3 bytes */
    SDP_UUID_16BIT(SC_PUBLIC_BROWSE_GROUP)  /* Public Browse Group */
};

static SdpAttribute attSdpAttributes[] = 
{
    /* Mandatory SDP Attributes */
    SDP_ATTRIBUTE(AID_SERVICE_CLASS_ID_LIST, attSrcClassId16),
    /* HID protocol descriptor list attribute */
    SDP_ATTRIBUTE(AID_PROTOCOL_DESC_LIST, attProtoDescList),
    /* Public Browse Group Service */
    SDP_ATTRIBUTE(AID_BROWSE_GROUP_LIST, publicBrowseGroup),
};

/*
static U8 getServiceInst(const BD_ADDR *bdaddr, BleService *pServ){
    U8 inst = 0;
    BleService *pServInst = BLE_FindServiceByUuid(bdaddr, pServ->uuid_size, pServ->uuid.uuid128);
    while(pServ != pServInst){
        pServInst = BLE_FindNextServiceByUuid(bdaddr, pServInst, pServ->uuid_size, pServ->uuid.uuid128);
        inst++;
    }
    return inst;
}

static U8 getCharInst(BleService *pServ, BleChar *pChar){
    U8 inst = 0;
    BleChar *pCharInst = BLE_FindCharByUuid(pServ, pChar->value_uuid_size, pChar->value_uuid.uuid128);
    while(pChar != pCharInst){
        pCharInst = BLE_FindNextCharByUuid(pServ, pCharInst, pChar->value_uuid_size, pChar->value_uuid.uuid128);
        inst++;
    }
    return inst;
}

static U8 getDescrInst(BleChar *pChar, BleCharDesc *pDescr){
    U8 inst = 0;
    BleCharDesc *pDescrInst = BLE_FindCharDescByUuid(pChar, pDescr->desc_uuid_size, pDescr->desc_uuid.uuid128);
    while(pDescr != pDescrInst){
        pDescrInst = BLE_FindNextCharDescByUuid(pChar, pDescrInst, pDescr->desc_uuid_size, pDescr->desc_uuid.uuid128);
        inst++;
    }
    return inst;
}
*/

static void reverseUUID(U8 *dest, U8 *src, U8 size){
    src += size;
    while(size--){
        *(dest++) = *(--src);
    }
}

static U16 composeSDPRecord(BleLocalServiceNode *svc_node){   
    SdpAttribute *attrib = NULL;
    U8 *ptr = NULL, *svcuuid;
    U32 svcuuid_size;
    U32 size = sizeof(attSdpAttributes) + sizeof(attProtoDescList);

    OS_Report("composeSDPRecord(svc=%p)", svc_node);
    // need to add {Service Class ID List(svc UUID), Protocol (L2CAP, ATT), Service Class Public Browse Group}
    if(svc_node->svc_node.service.uuid_size == 2){
        OS_Report("uuid16");
        svcuuid_size = sizeof(attSrcClassId16);
        svcuuid = attSrcClassId16;
        OS_MemCopy(&attSrcClassId16[3], svc_node->svc_node.service.uuid.uuid16, 2);
    }else if(svc_node->svc_node.service.uuid_size == 16){
        OS_Report("uuid128");
        svcuuid_size = sizeof(attSrcClassId128);
        svcuuid = attSrcClassId128;
        //OS_MemCopy(&attSrcClassId128[3], svc_node->svc_node.service.uuid.uuid128, 16);
        reverseUUID(&attSrcClassId128[3], svc_node->svc_node.service.uuid.uuid128, 16);
    }else{
        /* Invalid uuid size */
        OS_Report("Invalid uuid size. Just return");
        return 0;
    }
    size += svcuuid_size;    
    svc_node->svcSDPRecordSize = size;
    svc_node->pSvcSDPRecord = gatt_malloc(size+4);

    if(!svc_node->pSvcSDPRecord) return 0;

    attrib = (SdpAttribute*)svc_node->pSvcSDPRecord;
    OS_MemCopy(attrib, attSdpAttributes, sizeof(attSdpAttributes));
    ptr = (U8*)&attrib[sizeof(attSdpAttributes)/sizeof(attSdpAttributes[0])];
    /* Fill UUID */
    attrib->value = ptr;
    attrib->len = svcuuid_size;
    OS_MemCopy(ptr, svcuuid, svcuuid_size);
    ptr += svcuuid_size;
    attrib++;
    /* Fill start and end handle */
    attrib->value = ptr;
    OS_MemCopy(ptr, attProtoDescList, sizeof(attProtoDescList));
    ptr += sizeof(attProtoDescList);
    StoreBE16((ptr-2), svc_node->svc_node.service.end_handle);
    StoreBE16((ptr-5), svc_node->svc_node.service.start_handle);
    /* Register record */
    OS_MemSet(&svc_node->sdpRecord, 0, sizeof(svc_node->sdpRecord));
    svc_node->sdpRecord.num = sizeof(attSdpAttributes)/sizeof(attSdpAttributes[0]);
    svc_node->sdpRecord.attribs = (SdpAttribute*)svc_node->pSvcSDPRecord;    
    return svc_node->svcSDPRecordSize;
}

/************************************************
*   GATTS SVC API
*************************************************/
ATT_HANDLE GATTS_AddService(GATTS_REG_HANDLE user, GATT_SVC_UUID *svc_uuid, U16 num_handle){
    BleServiceDB *servicedb;
    U16 start = MIN_GATTS_HANDLE;
    BleLocalServiceNode *svc;
    //U32 found = 0;

    OS_Report("GATTS_AddService : handle=%p, num_handle=%u", user, num_handle);
    dumpSvcUUID(svc_uuid);
    if ((servicedb = BleFindServiceDBbyAddr(&gatts_fake_addr)) == NULL){
        OS_Report("Can not find DB. Create new");
        if((servicedb = BleNewServiceDB(&gatts_fake_addr)) == NULL){
            return INVALID_ATT_HANDLE;
        }
        InsertTailList(&g_BleServiceDBTable, &servicedb->node);
    }
    /* Find and insert service */
    svc = (BleLocalServiceNode*)&servicedb->service_list;
    while((svc = (BleLocalServiceNode*)GetNextNode(&svc->svc_node.node)) != (BleLocalServiceNode*)&servicedb->service_list){
        if(svc->svc_node.service.start_handle - start >= num_handle){
            //found = 1;
            break;
        }
        start = svc->svc_node.service.end_handle+1;
    }
    if(/*found && */(MAX_GATTS_HANDLE - start) > num_handle){
        BleLocalServiceNode *add_svc = allocLocalSvcNode();
        OS_Report("Alloc service %p", add_svc);
        add_svc->svc_node.service.start_handle = start;
        add_svc->svc_node.service.end_handle = start+num_handle-1;
        add_svc->svc_node.service.uuid_size = svc_uuid->uuid.uuid.len;
        if(add_svc->svc_node.service.uuid_size == 2){
            OS_MemCopy(add_svc->svc_node.service.uuid.uuid16, svc_uuid->uuid.uuid.uuid, 2);
        }else{
            OS_MemCopy(add_svc->svc_node.service.uuid.uuid128, svc_uuid->uuid.uuid.uuid, 16);
        }
        add_svc->is_primary = svc_uuid->is_primary;
        add_svc->user = (GattsUserContext*)user;
        add_svc->pSvcRecord = NULL;
        add_svc->nextAddedHandle = add_svc->svc_node.service.start_handle+1;
        InsertTailList(&svc->svc_node.node, &add_svc->svc_node.node);
        //svc_uuid->uuid.inst = getServiceInst(&gatts_fake_addr, &add_svc->svc_node.service);
        return add_svc->svc_node.service.start_handle;
    }
    GATTS_WARN("Run out of available service range");
    return INVALID_ATT_HANDLE;
}

ATT_HANDLE GATTS_AddInclService(ATT_HANDLE svc_handle, ATT_HANDLE incl_svc_handle){
    BleServiceDB *servicedb = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleLocalServiceNode *svc_node;
    BleServiceNode *svc, *incl_svc;
    BleIncludeServiceNode *add_incl_svc;

    if(!servicedb){
        GATTS_ERR("Can not find service DB");
        return INVALID_ATT_HANDLE;
    }
    svc_node = findServiceNodeByHandle(servicedb, svc_handle);
    svc = &svc_node->svc_node;
    if(!svc){
        GATTS_ERR("Can not find service");
        return INVALID_ATT_HANDLE;
    }
    incl_svc = (BleServiceNode*)findServiceNodeByHandle(servicedb, incl_svc_handle);
    if(!incl_svc){
        GATTS_ERR("Can not find included service");
        return INVALID_ATT_HANDLE;
    }
    add_incl_svc = gatt_malloc(sizeof(BleIncludeServiceNode));
    if(!add_incl_svc){
        return INVALID_ATT_HANDLE;
    }
    add_incl_svc->include_service.include_service_handle = incl_svc->service.start_handle;
    add_incl_svc->include_service.end_group_handle = incl_svc->service.end_handle;
    add_incl_svc->include_service.uuid_size = incl_svc->service.uuid_size;
    OS_MemCopy(add_incl_svc->include_service.uuid.uuid128, incl_svc->service.uuid.uuid128, sizeof(add_incl_svc->include_service.uuid.uuid128));
    add_incl_svc->include_service.handle = svc_node->nextAddedHandle;
    svc_node->nextAddedHandle++;
    InsertTailList(&svc->include_list, &add_incl_svc->node);
    return add_incl_svc->include_service.handle;
}

ATT_HANDLE GATTS_AddChar(ATT_HANDLE svc_handle, ATT_UUID *uuid, U8 properties, U32 permission){
    BleServiceDB *servicedb = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleLocalServiceNode *svc;
    BleCharNode *add_char;
    BleLocalCharNode *add_local_char;

    OS_Report("GATTS_AddChar(handle=%u)", svc_handle);

    if(!servicedb){
        GATTS_ERR("Can not find service DB");
        return INVALID_ATT_HANDLE;
    }
    svc = findServiceNodeByHandle(servicedb, svc_handle);
    OS_Report("find svc %p contain handle %u", svc, svc_handle);
    if(!svc){
        GATTS_ERR("Can not find service");
        return INVALID_ATT_HANDLE;
    }
    add_local_char = allocLocalCharNode();
    if(!add_local_char){
        return INVALID_ATT_HANDLE;
    }
    add_char = &add_local_char->char_node;
    add_char->characteristic.handle = svc->nextAddedHandle;
    add_char->characteristic.value_handle = svc->nextAddedHandle+1;
    add_char->characteristic.properties = properties;
    add_char->characteristic.value_uuid_size = uuid->uuid.len;
    if(uuid->uuid.len == 2){
        OS_MemCopy(add_char->characteristic.value_uuid.uuid16, uuid->uuid.uuid, 2);
    }else{
        OS_MemCopy(add_char->characteristic.value_uuid.uuid128, uuid->uuid.uuid, 16);
    }
    add_char->characteristic.value = NULL;
    add_char->characteristic.value_size = 0;
    add_local_char->permission = permission;
    svc->nextAddedHandle += 2; /* Reserve one more for char value */
    InsertTailList(&svc->svc_node.char_list,&add_char->node);
    //uuid->inst = getCharInst(&svc->svc_node.service, &add_char->characteristic);
    return add_char->characteristic.value_handle;
}

ATT_HANDLE GATTS_AddDescr(ATT_HANDLE svc_handle, ATT_UUID *descr_uuid, U32 permission){
    BleServiceDB *servicedb = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleServiceNode *svc;
    BleLocalServiceNode *svc_node;
    BleCharNode *last_char;
    BleCharDescNode *add_desc;
    BleLocalCharDescNode *add_local_desc;

    OS_Report("GATTS_AddDescr(handle=%u, permission=%u)", svc_handle, permission);


    if(!servicedb){
        GATTS_ERR("Can not find service DB");
        return INVALID_ATT_HANDLE;
    }
    svc_node = findServiceNodeByHandle(servicedb, svc_handle);
    OS_Report("find svc %p contain handle %u", svc_node, svc_handle);
    svc = &svc_node->svc_node;
    if(!svc){
        GATTS_ERR("Can not find service");
        return INVALID_ATT_HANDLE;
    }
    if(IsListEmpty(&svc->char_list)){
        GATTS_ERR("No added char");
        return INVALID_ATT_HANDLE;
    }
    last_char = (BleCharNode*)GetTailList(&svc->char_list);
    add_local_desc = (BleLocalCharDescNode*)gatt_malloc(sizeof(BleLocalCharDescNode));
    if(!add_local_desc){
        return INVALID_ATT_HANDLE;
    }
    add_desc = &add_local_desc->desc_node;
    add_desc->descriptor.desc_uuid_size = descr_uuid->uuid.len;
    if(descr_uuid->uuid.len == 2){
        OS_MemCopy(add_desc->descriptor.desc_uuid.uuid16, descr_uuid->uuid.uuid, 2);
    }else{
        OS_MemCopy(add_desc->descriptor.desc_uuid.uuid128, descr_uuid->uuid.uuid, 16);
    }
    add_desc->descriptor.desc_size = 0;
    add_desc->descriptor.desc = NULL;
    add_local_desc->permission = permission;
    add_desc->descriptor.handle = svc_node->nextAddedHandle;    
    svc_node->nextAddedHandle++; /* Reserve one more for char value */
    InsertTailList(&last_char->char_desc_list,&add_desc->node);
    //descr_uuid->inst = getDescrInst(&last_char->characteristic, &add_desc->descriptor);
    return add_desc->descriptor.handle;
}

BtStatus GATTS_StartService(ATT_HANDLE handle, GattTransport transport){
    BtStatus status = BT_STATUS_SUCCESS;
    BleServiceDB *db = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);
    OS_Report("Find contained service %p", svc);
    if(svc){
        if(transport | GATT_TRANSPORT_LE){
            composeAttRecord(svc);
            OS_Report("GATTS_StartService : svc=%p, pSvcRecord=%p, svcRecordSize=%u", svc, svc->pSvcRecord, svc->svcRecordSize);
            if(svc->svcRecordSize){
                ATTDB_AddRecord(svc->pSvcRecord, svc->svcRecordSize);
                ATTDB_AddGroupRecord(svc->svc_node.service.start_handle,
                                                       svc->svc_node.service.end_handle,
                                                       svc->is_primary ? BT_UUID_GATT_TYPE_PRIMARY_SERVICE : BT_UUID_GATT_TYPE_SECONDARY_SERVICE);
                mountAttRecordCallback((BleServiceNode*)svc, 1);
            }else{
                status = BT_STATUS_FAILED;
            }
        }
        if(transport | GATT_TRANSPORT_BR_EDR){
            // TODO: Compose SDP record
            if( composeSDPRecord(svc) > 0 ){
                SDP_AddRecord(&svc->sdpRecord);
            }else{
                if(!(transport | GATT_TRANSPORT_LE)){
                    status = BT_STATUS_FAILED;
                }
            }
        }
    }else{
        status = BT_STATUS_NOT_FOUND;
    }
    return status;
}

BtStatus GATTS_StopService(ATT_HANDLE handle){
    BtStatus status = BT_STATUS_SUCCESS;
    BleServiceDB *db = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);
    if(svc){
        if(svc->svcRecordSize){
            ATTDB_RemoveRecord(svc->svc_node.service.start_handle, svc->svc_node.service.end_handle);
            ATTDB_RemoveGroupRecord(svc->svc_node.service.start_handle, svc->svc_node.service.end_handle);
            mountAttRecordCallback((BleServiceNode*)svc, 0);
            gatt_free(svc->pSvcRecord);
            svc->pSvcRecord = NULL;
            svc->svcRecordSize = 0;
        }
        if(svc->svcSDPRecordSize){
            SDP_RemoveRecord(&svc->sdpRecord);
            gatt_free(svc->pSvcSDPRecord);
            svc->pSvcSDPRecord = NULL;
            svc->svcSDPRecordSize = 0;
        }
    }else{
        status = BT_STATUS_NOT_FOUND;
    }
    return status;
}

BtStatus GATTS_DeleteService(ATT_HANDLE handle){
    BtStatus status = BT_STATUS_SUCCESS;
    BleServiceDB *db = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);

    if(svc){
        GATTS_StopService(handle);
        freeService((BleServiceNode*)svc);
    }else{
        status = BT_STATUS_NOT_FOUND;
    }
    return status;
}

/* Callback for db query */
static U8 GattdbCallback(U8 type, BtRemoteDevice *link, U16 handle, U16 *len, U8 **raw){
    BleServiceDB *db = BleFindServiceDBbyAddr(&gatts_fake_addr);
    BleLocalServiceNode *svc = findContainedServiceNode(db, handle);
    ListEntry *handleNode;
    GattsCallbackParms cbParams;    
    U16 trans_id;
    U8 valid_trans_id = 0;
    U32 connHdl = BT_ATT_GetConId(link);

    if(BT_STATUS_SUCCESS == BT_ATT_GetConTransId(link, &trans_id)){
        valid_trans_id = 1;
    }

    GATTS_DBG("GattdbCallback(type=%u, handle=%u, connHdl=%u", type, handle, connHdl);
    if(connHdl >= MAX_ATT_CON_NO){
        return BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
    }

    OS_MemSet(&cbParams, 0, sizeof(cbParams));
    cbParams.result = 0;
    cbParams.handle = svc->user;
    switch(type){
    case ATT_OP_READ_LONG:
    {
        const BtATTReadBlobRequestStruct *req = (BtATTReadBlobRequestStruct*)BT_ATT_GetConCurrReq(link);
        cbParams.param.read_req_ind.is_long = 1;
        if(!req) return BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
        cbParams.param.read_req_ind.offset = req->valueOffset;
    }
    case ATT_OP_READ:
    case ATT_OP_READ_MULTIPLE:
    case ATT_OP_READ_BY_TYPE:
        cbParams.event = GATTS_EVENT_READ_REQ_IND;
        cbParams.param.read_req_ind.bdaddr = &link->bdAddr;
        cbParams.param.read_req_ind.connHdl = (GATTS_CONN_HANDLE)connHdl;
        cbParams.param.read_req_ind.handle = handle;
        if(!valid_trans_id) return BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
        cbParams.param.read_req_ind.trans_id = trans_id;

        switch(getValueNodeByHandle(db, handle, &handleNode)){
        case 1:
            /* Char value */
            {
                BleCharNode *char_node = (BleCharNode*)handleNode;
                if(char_node->characteristic.value_size){
                    *len = char_node->characteristic.value_size;
                    *raw = char_node->characteristic.value;
                    return BT_ATT_ERROR_CODE_NO_ERROR;
                }else{
                    svc->user->callback(svc->user, &cbParams);
                    return BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE;
                }
            }
            break;
        case 2:
            /* Descr */
            {
                BleCharDescNode *desc_node = (BleCharDescNode*)handleNode;
                if(desc_node->descriptor.desc_size){
                    *len = desc_node->descriptor.desc_size;
                    *raw = desc_node->descriptor.desc;
                    return BT_ATT_ERROR_CODE_NO_ERROR;
                }else{
                    svc->user->callback(svc->user, &cbParams);
                    return BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE;
                }
            }
            break;
        case 0:
        default:
            Assert(0);
            break;
        }
        break;
    case ATT_OP_PREPARE_WRITE:
    {
        const BtATTPrepareWriteRequestStruct *req = (BtATTPrepareWriteRequestStruct*)BT_ATT_GetConCurrReq(link);
        cbParams.param.write_req_ind.is_prepare = 1;
        cbParams.param.write_req_ind.offset = req->valueOffset;
        svc->user->prepare_count[connHdl]++;
        GATTS_DBG("prepare write of conn %u : count is %u", connHdl, svc->user->prepare_count[connHdl]);
        //return BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE;
    }
    case ATT_OP_WRITE_WO_RESPONSE:
    case ATT_OP_SIGNED_WRITE:
    case ATT_OP_WRITE:
        cbParams.event = GATTS_EVENT_WRITE_REQ_IND;
        cbParams.param.write_req_ind.connHdl = (GATTS_CONN_HANDLE)connHdl;
        cbParams.param.write_req_ind.bdaddr = &link->bdAddr;
        cbParams.param.write_req_ind.handle = handle;
        cbParams.param.write_req_ind.need_rsp = (type == ATT_OP_WRITE || type == ATT_OP_PREPARE_WRITE) ? 1 : 0;
        cbParams.param.write_req_ind.value_len = *len;
        cbParams.param.write_req_ind.value = *raw;
        if(!valid_trans_id) return BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
        cbParams.param.write_req_ind.trans_id = trans_id;
        svc->user->callback(svc->user, &cbParams);
        return BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE;
        //break;
    case ATT_OP_EXECUTE_WRITE:
        cbParams.param.exec_write_req_ind.exec_write = 1;
    case ATT_OP_EXECUTE_WRITE_CANCEL:
        svc->user->prepare_count[connHdl]--;
        GATTS_DBG("execute write : count of %u conn is %u", connHdl, svc->user->prepare_count[connHdl]);
        if(svc->user->prepare_count[connHdl] == 0){
            cbParams.event = GATTS_EVENT_EXEC_WRITE_REQ_IND;
            cbParams.param.exec_write_req_ind.connHdl = (GATTS_CONN_HANDLE)connHdl;
            cbParams.param.exec_write_req_ind.bdaddr = &link->bdAddr;
            if(!valid_trans_id) return BT_ATT_ERROR_CODE_UNLIKELY_ERROR;
            cbParams.param.exec_write_req_ind.trans_id = trans_id;        
            svc->user->callback(svc->user, &cbParams);
            return BT_ATT_ERROR_CODE_WAIT_APPLICATION_RESPONSE;
        }
        break;
    default:
        GATTS_WARN("Unknown type %u", type);
        break;
    }
    return BT_ATT_ERROR_CODE_INVALID_HANDLE;
}

#endif
