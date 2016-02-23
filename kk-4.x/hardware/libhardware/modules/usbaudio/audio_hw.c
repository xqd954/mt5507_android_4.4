/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "usb_audio_hw"
/*#define LOG_NDEBUG 0*/

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>

#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <cutils/properties.h>

#include <hardware/hardware.h>
#include <system/audio.h>
#include <hardware/audio.h>

#include <tinyalsa/asoundlib.h>

struct pcm_config usb_pcm_config = {
    .channels = 2,
#ifndef ANDROID_DEFAULT_AUDIO_CODE
    .rate = 48000, //DANIEL_DEBUG
#else
    .rate = 44100,
#endif
    .period_size = 1024,
    .period_count = 4,
    .format = PCM_FORMAT_S16_LE,
#ifndef ANDROID_DEFAULT_AUDIO_CODE    
    //DANIEL_DEBUG
    .start_threshold = 0,
    .avail_min = 0,
    //DANIEL_DEBUG
#endif    
};

struct audio_device {
    struct audio_hw_device hw_device;

    pthread_mutex_t lock; /* see note below on mutex acquisition order */
    int card;
    int device;
    bool standby;
};

struct stream_out {
    struct audio_stream_out stream;

    pthread_mutex_t lock; /* see note below on mutex acquisition order */
    struct pcm *pcm;
    bool standby;

    struct audio_device *dev;
};

#ifndef ANDROID_DEFAULT_AUDIO_CODE
enum SND_CARD_TYPE {
	SND_CARD_ALL	  =	0x00000000,
	SND_CARD_PLAYBACK =	0x00000001,
	SND_CARD_CAPTURE  =	0x00000010,
	SND_CARD_BOTH	  =	SND_CARD_CAPTURE | SND_CARD_PLAYBACK
};

#define AUDIO_OUTPUT_USB_NAME_EXPECT	NULL
#define AUDIO_OUTPUT_USB_NAME_EXCLUDE	"mtk - mtk"

bool check_sound_card_capability(char card_num, uint32_t expect_capability)  
{
	// currently ,we'll only check pcmCxD0y, that is, only check device 0 of the card x
	char buffer[128];
	if (expect_capability & SND_CARD_PLAYBACK) {
		sprintf(buffer, "/dev/snd/pcmC%cD0p", card_num);		
		FILE *fp = fopen(buffer, "w");
		if (fp == NULL) {
			LOGD("Check %s for PLAYBACK fail.", buffer);
			return false;
		}
		fclose(fp);
		LOGD("Check PLAYBACK for %s OK.", buffer);
	}
	if (expect_capability & SND_CARD_CAPTURE) {
		sprintf(buffer, "/dev/snd/pcmC%cD0c", card_num);		
		FILE *fp = fopen(buffer, "r");
		if (fp == NULL) {
			LOGD("Check %s for CAPTURE fail.", buffer);
			return false;
		}
		fclose(fp);
		LOGD("Check CAPTURE for %s OK.", buffer);
	}
	return true;
}

int get_sound_device_by_card(const char *name_expect, const char *name_exclude, uint32_t expect_capability)
{
	FILE* pFile;
    char buffer[128];
	pFile = fopen("/proc/asound/cards", "r");
	if (!pFile) {
		LOGE("Open /proc/asound/cards fail!");
		return -1;
	}

	while (fgets(buffer, 128, pFile)) 
	{
		// There is a space ' ' before index, we just simply hard-code it here
		if ((buffer[1] <= '9') && (buffer[1] >= '0')) 
		{
			if (!name_expect || ((name_expect) && strstr(buffer, name_expect))) 
			{
				if (!name_exclude || ((name_exclude) && (!strstr(buffer, name_exclude))))			
				{
					// OK, we have got a card, is it exactly what we want ?
					if (check_sound_card_capability(buffer[1], expect_capability)) {

						LOGD("Get a card with (%s, %s, %x) = %s .", 
							name_expect ? name_expect : "NULL",
							name_exclude ? name_exclude : "NULL",
							expect_capability, 
							buffer);						
						
						fclose(pFile);
						return buffer[1] - '0' ;				
					}
				}
			}
		}
	}

	LOGD("Cann't find a card with (%s, %s, %x).", 
			name_expect ? name_expect : "NULL",
			name_exclude ? name_exclude : "NULL",
			expect_capability);
	
	fclose(pFile);
	return -1;
}
#endif

/**
 * NOTE: when multiple mutexes have to be acquired, always respect the
 * following order: hw device > out stream
 */

/* Helper functions */

/* must be called with hw device and output stream mutexes locked */
static int start_output_stream(struct stream_out *out)
{
    struct audio_device *adev = out->dev;
    int i;

    if ((adev->card < 0) || (adev->device < 0))
        return -EINVAL;

    ALOGI("start_output_stream(%d,%d)", adev->card, adev->device);
    out->pcm = pcm_open(adev->card, adev->device, PCM_OUT, &usb_pcm_config);

    if (out->pcm && !pcm_is_ready(out->pcm)) {
        ALOGE("pcm_open() failed: %s", pcm_get_error(out->pcm));
        pcm_close(out->pcm);
        return -ENOMEM;
    }

    return 0;
}

/* API functions */

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    return usb_pcm_config.rate;
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    return 0;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    return usb_pcm_config.period_size *
           audio_stream_frame_size((struct audio_stream *)stream);
}

static uint32_t out_get_channels(const struct audio_stream *stream)
{
    return AUDIO_CHANNEL_OUT_STEREO;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
    return 0;
}

static int out_standby(struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    pthread_mutex_lock(&out->dev->lock);
    pthread_mutex_lock(&out->lock);

    if (!out->standby) {
        pcm_close(out->pcm);
        out->pcm = NULL;
        out->standby = true;
    }

    pthread_mutex_unlock(&out->lock);
    pthread_mutex_unlock(&out->dev->lock);

    return 0;
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    return 0;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct stream_out *out = (struct stream_out *)stream;
    struct audio_device *adev = out->dev;
    struct str_parms *parms;
    char value[32];
    int ret;
    int routing = 0;

    ALOGI("out_set_parameters");

    parms = str_parms_create_str(kvpairs);
    pthread_mutex_lock(&adev->lock);

    ret = str_parms_get_str(parms, "card", value, sizeof(value));
    if (ret >= 0)
    {
        adev->card = atoi(value);
        ALOGI("set card=%d", adev->card);        
    }
    else
    {
        ALOGI("no card information");
    }

    ret = str_parms_get_str(parms, "device", value, sizeof(value));
    if (ret >= 0)
    {
        adev->device = atoi(value);
        ALOGI("set device=%d", adev->device);
    }
    else
    {
        ALOGI("no device information");
    }

#ifndef ANDROID_DEFAULT_AUDIO_CODE
    if (adev->card == -1)
    {
        //Do auto-search
        int usb_card = get_sound_device_by_card(AUDIO_OUTPUT_USB_NAME_EXPECT, AUDIO_OUTPUT_USB_NAME_EXCLUDE, SND_CARD_PLAYBACK);
        ALOGI("get_sound_device_by_card() return %d", usb_card);
        adev->card = usb_card;
        adev->device = 0;
    }
#endif

    pthread_mutex_unlock(&adev->lock);
    str_parms_destroy(parms);

    return 0;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    return (usb_pcm_config.period_size * usb_pcm_config.period_count * 1000) /
            out_get_sample_rate(&stream->common);
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    return -ENOSYS;
}

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    int ret;
    struct stream_out *out = (struct stream_out *)stream;

    pthread_mutex_lock(&out->dev->lock);
    pthread_mutex_lock(&out->lock);
    if (out->standby) {
        ret = start_output_stream(out);
        if (ret != 0) {
            goto err;
        }
        out->standby = false;
    }

    pcm_write(out->pcm, (void *)buffer, bytes);

    pthread_mutex_unlock(&out->lock);
    pthread_mutex_unlock(&out->dev->lock);

    return bytes;

err:
    pthread_mutex_unlock(&out->lock);
    pthread_mutex_unlock(&out->dev->lock);
    if (ret != 0) {
        usleep(bytes * 1000000 / audio_stream_frame_size(&stream->common) /
               out_get_sample_rate(&stream->common));
    }

    return bytes;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int out_get_next_write_timestamp(const struct audio_stream_out *stream,
                                        int64_t *timestamp)
{
    return -EINVAL;
}

static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out)
{
    struct audio_device *adev = (struct audio_device *)dev;
    struct stream_out *out;
    int ret;

    out = (struct stream_out *)calloc(1, sizeof(struct stream_out));
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
    out->stream.get_next_write_timestamp = out_get_next_write_timestamp;

    out->dev = adev;

    config->format = out_get_format(&out->stream.common);
    config->channel_mask = out_get_channels(&out->stream.common);
    config->sample_rate = out_get_sample_rate(&out->stream.common);

    out->standby = true;

    adev->card = -1;
    adev->device = -1;

#if 0 //ndef ANDROID_DEFAULT_AUDIO_CODE
    //Do auto-search
    int usb_card = get_sound_device_by_card(AUDIO_OUTPUT_USB_NAME_EXPECT, AUDIO_OUTPUT_USB_NAME_EXCLUDE, SND_CARD_PLAYBACK);
    ALOGI("get_sound_device_by_card() return %d", usb_card);
    if (usb_card == -1)
    {
        ret = -ENOMEM;
        goto err_open;
    }
    adev->card = usb_card;
    adev->device = 0;
#endif

    *stream_out = &out->stream;
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    out_standby(&stream->common);
    free(stream);
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    return 0;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    return strdup("");
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    return 0;
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, audio_mode_t mode)
{
    return 0;
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    return -ENOSYS;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                         const struct audio_config *config)
{
    return 0;
}

static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in)
{
    return -ENOSYS;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *stream)
{
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    return 0;
}

static int adev_close(hw_device_t *device)
{
    struct audio_device *adev = (struct audio_device *)device;

    free(device);
    return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct audio_device *adev;
    int ret;

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    adev = calloc(1, sizeof(struct audio_device));
    if (!adev)
        return -ENOMEM;

    adev->hw_device.common.tag = HARDWARE_DEVICE_TAG;
    adev->hw_device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->hw_device.common.module = (struct hw_module_t *) module;
    adev->hw_device.common.close = adev_close;

    adev->hw_device.init_check = adev_init_check;
    adev->hw_device.set_voice_volume = adev_set_voice_volume;
    adev->hw_device.set_master_volume = adev_set_master_volume;
    adev->hw_device.set_mode = adev_set_mode;
    adev->hw_device.set_mic_mute = adev_set_mic_mute;
    adev->hw_device.get_mic_mute = adev_get_mic_mute;
    adev->hw_device.set_parameters = adev_set_parameters;
    adev->hw_device.get_parameters = adev_get_parameters;
    adev->hw_device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->hw_device.open_output_stream = adev_open_output_stream;
    adev->hw_device.close_output_stream = adev_close_output_stream;
    adev->hw_device.open_input_stream = adev_open_input_stream;
    adev->hw_device.close_input_stream = adev_close_input_stream;
    adev->hw_device.dump = adev_dump;

    *device = &adev->hw_device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "USB audio HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_module_methods,
    },
};
