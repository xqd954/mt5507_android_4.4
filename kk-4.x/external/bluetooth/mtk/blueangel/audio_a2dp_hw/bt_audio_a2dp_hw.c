/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *  Filename:      bt_audio_a2dp_hw.c
 *
 *  Description:   Implements hal for bluedroid a2dp audio device
 *
 *****************************************************************************/

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/str_parms.h>
#include <cutils/sockets.h>

#include <system/audio.h>
#include <hardware/audio.h>

#include <hardware/hardware.h>
#include "bt_audio_a2dp_hw.h"

#define LOG_TAG "bt_audio_a2dp_hw"
/* #define LOG_NDEBUG 0 */
#include <cutils/log.h>

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

#define CTRL_CHAN_RETRY_COUNT 3
#define USEC_PER_SEC 1000000L

#define CASE_RETURN_STR(const) case const: return #const;

#define FNLOG()             ALOGV("%s", __FUNCTION__);
#define DEBUG(fmt, ...)     ALOGV("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define INFO(fmt, ...)      ALOGI("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define ERROR(fmt, ...)     ALOGE("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)

#define ASSERTC(cond, msg, val) if (!(cond)) {ERROR("### ASSERT : %s line %d %s (%d) ###", __FILE__, __LINE__, msg, val);}

/*****************************************************************************
**  Local type definitions
******************************************************************************/

typedef enum {
    AUDIO_A2DP_STATE_STARTING,
    AUDIO_A2DP_STATE_STARTED,
    AUDIO_A2DP_STATE_STOPPING,
    AUDIO_A2DP_STATE_STOPPED,
    AUDIO_A2DP_STATE_SUSPENDED, /* need explicit set param call to resume (suspend=false) */
    AUDIO_A2DP_STATE_STANDBY    /* allows write to autoresume */
} a2dp_state_t;

struct a2dp_stream_out;

struct a2dp_audio_device {
    struct audio_hw_device device;
    struct a2dp_stream_out *output;
};

struct a2dp_config {
    uint32_t                rate;
    uint32_t                channel_flags;
    int                     format;
};

/* move ctrl_fd outside output stream and keep open until HAL unloaded ? */

struct a2dp_stream_out {
    struct audio_stream_out stream;
    //pthread_mutex_t         lock;
    //int                     ctrl_fd;
    //int                     audio_fd;
    size_t                  buffer_sz;
    a2dp_state_t            state;
    struct a2dp_config      cfg;
	void                    *ctx;
};

struct a2dp_stream_in {
    struct audio_stream_in stream;
};

/*****************************************************************************
**  Static variables
******************************************************************************/
#define BT_STATUS_SUCCESS 0
#define BT_STATUS_FAILED  1

/*****************************************************************************
**  Globle variables
******************************************************************************/
uint32_t last = 0;
uint32_t now = 0;

/*****************************************************************************
**  Static functions
******************************************************************************/
static uint32_t out_get_latency(const struct audio_stream_out *stream);
static size_t out_get_buffer_size(const struct audio_stream *stream);

/*****************************************************************************
**  Externs
******************************************************************************/
extern int a2dp_init(int rate, int channels, void **ptr);
extern void a2dp_set_sink(void *ptr, const char* address);
extern int a2dp_write(void *ptr, const void *buffer, int count);
extern int a2dp_stop(void *ptr);
extern void a2dp_cleanup(void *ptr);

/*****************************************************************************
**  Functions
******************************************************************************/

/*****************************************************************************
**   Miscellaneous helper functions
******************************************************************************/

/* logs timestamp with microsec precision
   pprev is optional in case a dedicated diff is required */
static void ts_log(char *tag, int val, struct timespec *pprev_opt)
{
    struct timespec now;
    static struct timespec prev = {0,0};
    unsigned long long now_us;
    unsigned long long diff_us;

    clock_gettime(CLOCK_MONOTONIC, &now);

    now_us = now.tv_sec*USEC_PER_SEC + now.tv_nsec/1000;

    if (pprev_opt)
    {
        diff_us = (now.tv_sec - prev.tv_sec) * USEC_PER_SEC + (now.tv_nsec - prev.tv_nsec)/1000;
        *pprev_opt = now;
        DEBUG("[%s] ts %08lld, *diff %08lld, val %d", tag, now_us, diff_us, val);
    }
    else
    {
        diff_us = (now.tv_sec - prev.tv_sec) * USEC_PER_SEC + (now.tv_nsec - prev.tv_nsec)/1000;
        prev = now;
        DEBUG("[%s] ts %08lld, diff %08lld, val %d", tag, now_us, diff_us, val);
    }
}



/*****************************************************************************
**
** AUDIO DATA PATH
**
*****************************************************************************/

static int a2dp_stream_out_init(struct a2dp_stream_out *out)
{
    FNLOG();

    out->state = AUDIO_A2DP_STATE_STOPPED;

    out->cfg.channel_flags = AUDIO_STREAM_DEFAULT_CHANNEL_FLAG;
    out->cfg.format = AUDIO_STREAM_DEFAULT_FORMAT;
    out->cfg.rate = AUDIO_STREAM_DEFAULT_RATE;
    out->ctx = NULL;

    /* manages max capacity of socket pipe */
    out->buffer_sz = AUDIO_STREAM_OUTPUT_BUFFER_SZ;

    int ret = a2dp_init(AUDIO_STREAM_DEFAULT_RATE, 2, &out->ctx);

	return ret;
}

static uint32_t get_system_time()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec * 1000000 + now.tv_nsec / 1000);
}

/*****************************************************************************
**
**  audio output callbacks
**
*****************************************************************************/

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    int sent;
    int wait = 0;
    uint32_t latency = 0;

    sent = a2dp_write(out->ctx, buffer, bytes);

    latency = out_get_latency(stream) - 200;
    now = get_system_time();

    if ((now - last)/1000 < latency - 5) {
        wait = (int)latency * 1000 - (now - last);
        usleep(wait);
        INFO("[A2DP HW] a2dp run too fast, wait: %d", wait);
    }

    DEBUG("[A2DP] write %d bytes out of %d bytes, %d, now: %d, last: %d, %d", sent, bytes, latency, now, last, (now - last));
    
    last = now;
    return sent;
}


static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("rate %d", out->cfg.rate);

    return out->cfg.rate;
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("out_set_sample_rate : %d", rate);

    if (rate != AUDIO_STREAM_DEFAULT_RATE)
    {
        ERROR("only rate %d supported", AUDIO_STREAM_DEFAULT_RATE);
        return -1;
    }

    out->cfg.rate = rate;

    return 0;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("buffer_size : %d", out->buffer_sz);

    return out->buffer_sz;
}

static uint32_t out_get_channels(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("channels 0x%x", out->cfg.channel_flags);

    return out->cfg.channel_flags;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    DEBUG("format 0x%x", out->cfg.format);
    return out->cfg.format;
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    DEBUG("setting format not yet supported (0x%x)", format);
    return -ENOSYS;
}

static int out_standby(struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    int retVal = 0;

    FNLOG();

    a2dp_stop(out->ctx);

    last = 0;
    now = 0;

    return retVal;
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    FNLOG();
    return 0;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    struct str_parms *parms;
    char keyval[16];
    int retval = 0;

    INFO("state %d", out->state);

    //pthread_mutex_lock(&out->lock);

    parms = str_parms_create_str(kvpairs);

    /* dump params */
    str_parms_dump(parms);

    retval = str_parms_get_str(parms, "closing", keyval, sizeof(keyval));

    if (retval >= 0)
    {
        if (strcmp(keyval, "true") == 0)
        {
            DEBUG("stream closing, disallow any writes");
            out->state = AUDIO_A2DP_STATE_STOPPING;
        }
    }

    retval = str_parms_get_str(parms, "A2dpSuspended", keyval, sizeof(keyval));

    if (retval >= 0)
    {
        if (strcmp(keyval, "true") == 0)
        {
            if (out->state == AUDIO_A2DP_STATE_STARTED)
                retval = a2dp_stop(out->ctx);//suspend_audio_datapath(out, false);
        }
        else
        {
            /* Do not start the streaming automatically. If the phone was streaming
             * prior to being suspended, the next out_write shall trigger the
             * AVDTP start procedure */
            if (out->state == AUDIO_A2DP_STATE_SUSPENDED)
                out->state = AUDIO_A2DP_STATE_STANDBY;
            /* Irrespective of the state, return 0 */
            retval = 0;
        }
    }

    //pthread_mutex_unlock(&out->lock);
    str_parms_destroy(parms);

    return retval;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    FNLOG();

    /* add populating param here */

    return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    int latency_us;

    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    FNLOG();

    latency_us = ((out->buffer_sz * 1000 ) /
                    audio_stream_frame_size(&out->stream.common) /
                    out->cfg.rate) * 1000;


    return (latency_us / 1000) + 200;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    FNLOG();

    /* volume controlled in audioflinger mixer (digital) */

    return -ENOSYS;
}



static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    FNLOG();
    return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    FNLOG();
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    FNLOG();
    return 0;
}

/*
 * AUDIO INPUT STREAM
 */

static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    FNLOG();
    return 8000;
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    FNLOG();
    return 0;
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    FNLOG();
    return 320;
}

static uint32_t in_get_channels(const struct audio_stream *stream)
{
    FNLOG();
    return AUDIO_CHANNEL_IN_MONO;
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
    FNLOG();
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int in_set_format(struct audio_stream *stream, audio_format_t format)
{
    FNLOG();
    return 0;
}

static int in_standby(struct audio_stream *stream)
{
    FNLOG();
    return 0;
}

static int in_dump(const struct audio_stream *stream, int fd)
{
    FNLOG();
    return 0;
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    FNLOG();
    return 0;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    FNLOG();
    return strdup("");
}

static int in_set_gain(struct audio_stream_in *stream, float gain)
{
    FNLOG();
    return 0;
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
                       size_t bytes)
{
    FNLOG();
    return bytes;
}

static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
    FNLOG();
    return 0;
}

static int in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    FNLOG();
    return 0;
}

static int in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    FNLOG();

    return 0;
}

/* 
    this function is used to open output stream channel 
    according to a2dp, it refer to source role
*/
static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out)

{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_out *out;
    int ret = 0;
    int i;

    INFO("opening output");

    out = (struct a2dp_stream_out *)calloc(1, sizeof(struct a2dp_stream_out));

    if (!out)
        return -ENOMEM;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;

    /* initialize a2dp specifics */
    if (a2dp_stream_out_init(out) == BT_STATUS_FAILED)
    {
        DEBUG("init streaming fail");
        return 0;
	}

   /* set output config values */
   if (config)
   {
      config->format = out_get_format((const struct audio_stream *)&out->stream);
      config->sample_rate = out_get_sample_rate((const struct audio_stream *)&out->stream);
      config->channel_mask = out_get_channels((const struct audio_stream *)&out->stream);
   }
    *stream_out = &out->stream;
    a2dp_dev->output = out;

    last = 0;
    now = 0;


    DEBUG("success");
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    ERROR("failed");
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    INFO("closing output (state %d)", out->state);

    //if ((out->state == AUDIO_A2DP_STATE_STARTED) || (out->state == AUDIO_A2DP_STATE_STOPPING))
    //    stop_audio_datapath(out);

    a2dp_cleanup(out->ctx);
	
    //skt_disconnect(out->ctrl_fd);
    free(stream);
    a2dp_dev->output = NULL;

    DEBUG("done");
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_out *out = a2dp_dev->output;
    int retval = 0;

    if (out == NULL)
        return retval;

    INFO("state %d", out->state);

    retval = out->stream.common.set_parameters((struct audio_stream *)out, kvpairs);

    return retval;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    struct str_parms *parms;

    FNLOG();

    parms = str_parms_create_str(keys);

    str_parms_dump(parms);

    str_parms_destroy(parms);

    return strdup("");
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device*)dev;

    FNLOG();

    return 0;
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    FNLOG();

    return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    FNLOG();

    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, int mode)
{
    FNLOG();

    return 0;
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    FNLOG();

    return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    FNLOG();

    return -ENOSYS;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                         const struct audio_config *config)
{
    FNLOG();

    return 320;
}

/* 
    this function is used to open input stream channel 
    according to a2dp, it refer to sink role
*/

static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in)
{
    struct a2dp_audio_device *ladev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_in *in;
    int ret;

    FNLOG();

    in = (struct a2dp_stream_in *)calloc(1, sizeof(struct a2dp_stream_in));

    if (!in)
        return -ENOMEM;

    in->stream.common.get_sample_rate = in_get_sample_rate;
    in->stream.common.set_sample_rate = in_set_sample_rate;
    in->stream.common.get_buffer_size = in_get_buffer_size;
    in->stream.common.get_channels = in_get_channels;
    in->stream.common.get_format = in_get_format;
    in->stream.common.set_format = in_set_format;
    in->stream.common.standby = in_standby;
    in->stream.common.dump = in_dump;
    in->stream.common.set_parameters = in_set_parameters;
    in->stream.common.get_parameters = in_get_parameters;
    in->stream.common.add_audio_effect = in_add_audio_effect;
    in->stream.common.remove_audio_effect = in_remove_audio_effect;
    in->stream.set_gain = in_set_gain;
    in->stream.read = in_read;
    in->stream.get_input_frames_lost = in_get_input_frames_lost;

    *stream_in = &in->stream;
    return 0;

err_open:
    free(in);
    *stream_in = NULL;
    return ret;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *in)
{
    FNLOG();

    return;
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    FNLOG();

    return 0;
}

static int adev_close(hw_device_t *device)
{
    FNLOG();

    free(device);
    return 0;
}

/* 
    the first function called by hardware, return device 
    we can use this function directly
*/
static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct a2dp_audio_device *adev;
    int ret;

    //bt_ext_log(" adev_open in A2dp_hw module");

    FNLOG();

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
    {
        ERROR("interface %s not matching [%s]", name, AUDIO_HARDWARE_INTERFACE);
        return -EINVAL;
    }

    adev = calloc(1, sizeof(struct a2dp_audio_device));

    if (!adev)
        return -ENOMEM;

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_CURRENT;
    adev->device.common.module = (struct hw_module_t *) module;
    adev->device.common.close = adev_close;

    adev->device.init_check = adev_init_check;
    adev->device.set_voice_volume = adev_set_voice_volume;
    adev->device.set_master_volume = adev_set_master_volume;
    adev->device.set_mode = adev_set_mode;
    adev->device.set_mic_mute = adev_set_mic_mute;
    adev->device.get_mic_mute = adev_get_mic_mute;
    adev->device.set_parameters = adev_set_parameters; // need implement
    adev->device.get_parameters = adev_get_parameters; // need implement
    adev->device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->device.open_output_stream = adev_open_output_stream; // need
    adev->device.close_output_stream = adev_close_output_stream; //
    adev->device.open_input_stream = adev_open_input_stream;
    adev->device.close_input_stream = adev_close_input_stream;
    adev->device.dump = adev_dump;

    adev->output = NULL;


    *device = &adev->device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1, // AUDIO_MODULE_API_VERSION_0_1
        .version_minor = 0, // HARDWARE_HAL_API_VERSION
        .id = "audio", // AUDIO_HARDWARE_MODULE_ID, // "libaudio"
        .name = "A2DP Audio HW HAL",
        .author = "Mediatek Android Project",
        .methods = &hal_module_methods,
    },
};

