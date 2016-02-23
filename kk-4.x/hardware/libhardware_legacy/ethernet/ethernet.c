
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>


#define LOG_TAG "EthernetHW"
#include "log/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "private/android_filesystem_config.h"

#include <linux/if.h>
#include <sys/types.h>
#include <sys/socket.h>

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
-----------------------------------------------------------------------------*/
#define  ETHERNET_DRIVER_MODULE_NAME     "star_mac"


/*----------------------------------------------------------------------------- 
                    function declarations 
----------------------------------------------------------------------------*/ 
extern int init_module(void *, unsigned long, const char *);   



/*-----------------------------------------------------------------------------
 * static  methods implementations
 *---------------------------------------------------------------------------*/
 
static int _insmod_(const char *filename, const char *args)
{
    void *module;
    unsigned int size;
    int ret;

    module = load_file(filename, &size);
    if (!module)
        return -1;
    LOGD ("insmod filename[%s]", filename);
    ret = init_module(module, size, args);

    free(module);

    return ret;
}
    
static int _insmod_driver_(const char *driver_ko, const char *args)
{
    char file_path[256] = {0};
    snprintf (file_path, sizeof (file_path), "/system/lib/modules/%s", driver_ko);
    if (_insmod_(file_path, args) != 0)
    {
        LOGD ("insmod %s failed : %s: %s", file_path, strerror(errno));
        return -1;
    }
    return 0;
}


/*-----------------------------------------------------------------------------
 * exported methods implementations
 *---------------------------------------------------------------------------*/

int ethernet_is_driver_loaded()
{
    FILE *proc;
    char line[50] = {0};
    LOGD("is_driver_loaded\n");
    if ((proc = fopen("/proc/modules", "r")) == NULL)
    {
        LOGD("Could not open %s: %s", "/proc/modules", strerror(errno));
        return -1;
    }  
    while ((fgets(line, sizeof(line), proc)) != NULL)
    {
        LOGD ("is_wifi_driver_unloaded line is [%s]\n", line);
        
        if (strncmp(line, ETHERNET_DRIVER_MODULE_NAME, strlen(ETHERNET_DRIVER_MODULE_NAME)) == 0)
        {
            fclose(proc);
            return 0;
        }
    }       
    fclose(proc);

    LOGD("is_driver_loaded end\n");
    return -1; 
}

int ethernet_load_driver()
{
    if(ethernet_is_driver_loaded() == 0)
    {
        LOGD("Driver loaded !!!!\n");
        return 0;        
    }
    
    LOGD("android_load_driver begin\n");
    int ret =  0;

    ret = _insmod_driver_("star_mac.ko", "");
    if(ret != 0)
    {
        LOGD("[%d]_insmod_driver_ failure\n", __LINE__);
        return -1;
    }
    
    int count = 100; /* wait at most 20 seconds for completion */
    while (count-- > 0) {
        if(ethernet_is_driver_loaded() == 0)
        {
            return 0;
        }
        LOGD("wait driver loaded\n");
        usleep(200000);
    }
    
    LOGD("android_load_driver failure\n");
    return -1;
}
    


