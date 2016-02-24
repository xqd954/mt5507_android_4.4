#ifndef COMMON_H
#define COMMON_H

#include <utils/Log.h>

#define SQL_DB_TCL_DEVICEINFO_PATH         "/system/etc/deviceinfo/DeviceInfo.db"
#define SQL_DB_TCL_DEVICEINFO_BACKUP_PATH  "/system/etc/deviceinfo/DeviceInfo.db"
#define CHECK_TABLE_DB_DEVICEINFO          "select * from tbl_DeviceInfo"
#define TABLE_DeviceInfo                   "tbl_DeviceInfo"

#define SQL_DB_PERMISSION       0666
#define BUFFERSIZE              4096
#define QUERY_STRING_LENGTH     128
#define QUERY_CONDITION_LENGTH  60

#define DEBUG  false

#define ASSERT(_x_) \
    do  { \
        if ( ! ( _x_ ) ) \
        { \
            if(DEBUG) ALOGD("ASSERT FAIL: %s %s %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__); \
        } \
	} while (0)

#define PTH_RET_CHK(_pf_) \
    ({ \
        int r = _pf_; \
        if ((r != 0) && (r != ETIMEDOUT)) \
            if(DEBUG) ALOGD("[PTHREAD] %s: %d: %s: %s\n", __FILE__, __LINE__, #_pf_, strerror(r)); \
        r; \
    })
	
#endif