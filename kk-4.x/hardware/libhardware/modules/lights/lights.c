#define LOG_TAG "lights"

#include <cutils/log.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#include "mtcommon.h"
#include "mtvdo.h"
#if SUPPORT_DCR
#include "mtdrvcust.h"
#include "mtperipheral.h"
#endif

#define LIGHTS_DBG_ON

#ifndef TRUE
#define TURE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static int isBacklightInitialized = FALSE;

/******************************************************************************/

static int rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

/**
 * device methods
 */

static int set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int bri = 0;    
    bri = rgb_to_brightness(state);

    BOOL isDynamicBackLight = FALSE;
    if (state->brightnessMode == BRIGHTNESS_MODE_SENSOR)
    {
        isDynamicBackLight = TRUE;
    }
    
#if SUPPORT_DCR   
    UINT32 u4Gpio = 0;
    if (MTDRVCUST_OptQuery(ePanelDCRGpio, &u4Gpio) == -1)
    {
#endif
        ALOGD(" isDynamicBackLight = %d",isDynamicBackLight);
        MTVDO_SetArg(MTVDO_MAIN, MTVDO_ARG_ADAPTIVE_BACK_LIGHT, (INT16)isDynamicBackLight);
#if SUPPORT_DCR
    }
    else
    {
        UINT32 u4Value = MTDRVCUST_OptGet(ePanelDCROnVal);
        if (!isDynamicBackLight)
        {
            u4Value = !u4Value;
        }
        MTGPIO_Output((INT32)u4Gpio, (INT32*)&u4Value);
    }
#endif

    if (!isDynamicBackLight)
    {
        ALOGD(" brightness = %d",bri);
        bri = (bri*100)/255; //mtal range 0 - 100, android range 0 - 255
        MTVDO_SetArg(MTVDO_MAIN, MTVDO_ARG_BACK_LIGHT_LVL, (INT16)bri);
    }

    return 0;
}

static int set_light_keyboard(struct light_device_t* dev,
        struct light_state_t const* state)
{
    return 0;
}

static int set_light_buttons(struct light_device_t* dev,
        struct light_state_t const* state)
{
    return 0;
}

static int set_light_battery(struct light_device_t* dev,
        struct light_state_t const* state)
{
    return 0;
}

static int set_light_notifications(struct light_device_t* dev,
        struct light_state_t const* state)
{
    return 0;
}

static int set_light_attention(struct light_device_t* dev,
        struct light_state_t const* state)
{
    return 0;
}


/** Close the lights device */
static int close_lights(struct light_device_t *dev)
{
    if (dev) {
        free(dev);
    }
    return 0;
}


/******************************************************************************/

/**
 * module methods
 */

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);

    if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
        set_light = set_light_backlight;

        if (!isBacklightInitialized)
        {
            ALOGD("MTAL_Init");
            
            MTAL_Init();
            MTVDO_Init();
#ifdef SUPPORT_DCR
            MTGPIO_Init();
#endif
            isBacklightInitialized = TRUE;
        }
    }
    else if (0 == strcmp(LIGHT_ID_KEYBOARD, name)) {
        set_light = set_light_keyboard;
    }
    else if (0 == strcmp(LIGHT_ID_BUTTONS, name)) {
        set_light = set_light_buttons;
    }
    else if (0 == strcmp(LIGHT_ID_BATTERY, name)) {
        set_light = set_light_battery;
    }
    else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name)) {
        set_light = set_light_notifications;
    }
    else if (0 == strcmp(LIGHT_ID_ATTENTION, name)) {
        set_light = set_light_attention;
    }
    else {
        return -EINVAL;
    }

    struct light_device_t *dev = malloc(sizeof(struct light_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;
    return 0;
}


static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

/*
 * The lights Module
 */
struct lights_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = LIGHTS_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = LIGHTS_HARDWARE_MODULE_ID,
        .name = "MTK lights HAL",
        .author = "Mediatek Inc.",
        .methods = &lights_module_methods,
    }
};

