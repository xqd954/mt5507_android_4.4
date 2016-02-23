/*
**
** Copyright 2012, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define LOG_TAG "MediaPlayerFactory"
#include <utils/Log.h>

#include <cutils/properties.h>
#include <media/IMediaPlayer.h>
#include <media/stagefright/foundation/ADebug.h>
#include <utils/Errors.h>
#include <utils/misc.h>

#include "MediaPlayerFactory.h"

#include "MidiFile.h"
#include "TestPlayerStub.h"
#include "StagefrightPlayer.h"
#include "nuplayer/NuPlayerDriver.h"
#include <CmpbPlayer.h>
#ifdef SUPPORT_MTK_STREAM_PLAYER
#include "MtkStreamPlayer.h"
#endif
namespace android {

Mutex MediaPlayerFactory::sLock;
MediaPlayerFactory::tFactoryMap MediaPlayerFactory::sFactoryMap;
bool MediaPlayerFactory::sInitComplete = false;

status_t MediaPlayerFactory::registerFactory_l(IFactory* factory,
                                               player_type type) {
    if (NULL == factory) {
        ALOGE("Failed to register MediaPlayerFactory of type %d, factory is"
              " NULL.", type);
        return BAD_VALUE;
    }

    if (sFactoryMap.indexOfKey(type) >= 0) {
        ALOGE("Failed to register MediaPlayerFactory of type %d, type is"
              " already registered.", type);
        return ALREADY_EXISTS;
    }

    if (sFactoryMap.add(type, factory) < 0) {
        ALOGE("Failed to register MediaPlayerFactory of type %d, failed to add"
              " to map.", type);
        return UNKNOWN_ERROR;
    }

    return OK;
}

player_type MediaPlayerFactory::getDefaultPlayerType() {
    ALOGE("enter MediaPlayerFactory::getDefaultPlayerType()\n");

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
    char value[PROPERTY_VALUE_MAX];
    if (property_get("media.stagefright.use-nuplayer", value, NULL)
            && (!strcmp("1", value) || !strcasecmp("true", value))) {
        return NU_PLAYER;
    }

    return STAGEFRIGHT_PLAYER;
#else
    return CMPB_PLAYER;
#endif
}

status_t MediaPlayerFactory::registerFactory(IFactory* factory,
                                             player_type type) {
    Mutex::Autolock lock_(&sLock);
    return registerFactory_l(factory, type);
}

void MediaPlayerFactory::unregisterFactory(player_type type) {
    Mutex::Autolock lock_(&sLock);
    sFactoryMap.removeItem(type);
}

#define GET_PLAYER_TYPE_IMPL(a...)                      \
    Mutex::Autolock lock_(&sLock);                      \
                                                        \
    player_type ret = STAGEFRIGHT_PLAYER;               \
    float bestScore = 0.0;                              \
                                                        \
    for (size_t i = 0; i < sFactoryMap.size(); ++i) {   \
                                                        \
        IFactory* v = sFactoryMap.valueAt(i);           \
        float thisScore;                                \
        CHECK(v != NULL);                               \
        thisScore = v->scoreFactory(a, bestScore);      \
        if (thisScore > bestScore) {                    \
            ret = sFactoryMap.keyAt(i);                 \
            bestScore = thisScore;                      \
        }                                               \
    }                                                   \
                                                        \
    if (0.0 == bestScore) {                             \
        ret = getDefaultPlayerType();                   \
    }                                                   \
                                                        \
    return ret;

player_type MediaPlayerFactory::getPlayerType(const sp<IMediaPlayer>& client,
                                              const char* url) {
    GET_PLAYER_TYPE_IMPL(client, url);
}

player_type MediaPlayerFactory::getPlayerType(const sp<IMediaPlayer>& client,
                                              int fd,
                                              int64_t offset,
                                              int64_t length) {
    GET_PLAYER_TYPE_IMPL(client, fd, offset, length);
}

player_type MediaPlayerFactory::getPlayerType(const sp<IMediaPlayer>& client,
                                              const sp<IStreamSource> &source) {
    GET_PLAYER_TYPE_IMPL(client, source);
}

#undef GET_PLAYER_TYPE_IMPL

sp<MediaPlayerBase> MediaPlayerFactory::createPlayer(
        player_type playerType,
        void* cookie,
        notify_callback_f notifyFunc) {
    sp<MediaPlayerBase> p;
    IFactory* factory;
    status_t init_result;
    Mutex::Autolock lock_(&sLock);

    if (sFactoryMap.indexOfKey(playerType) < 0) {
        ALOGE("Failed to create player object of type %d, no registered"
              " factory", playerType);
        return p;
    }

    factory = sFactoryMap.valueFor(playerType);
    CHECK(NULL != factory);
    p = factory->createPlayer();

    if (p == NULL) {
        ALOGE("Failed to create player object of type %d, create failed",
               playerType);
        return p;
    }

    init_result = p->initCheck();
    if (init_result == NO_ERROR) {
        p->setNotifyCallback(cookie, notifyFunc);
    } else {
        ALOGE("Failed to create player object of type %d, initCheck failed"
              " (res = %d)", playerType, init_result);
        p.clear();
    }

    return p;
}

//MTK Begin PIC:Guanghui.wang
#define OGG_MAX_HEAD_LEN                300
#define OGG_PAGE_SEGMENT_MAX            0x100
#define dwBigFourCC(a,b,c,d)            ((((uint32_t)a)<<24)|((b)<<16)|((c)<<8)|(d))
#define OGG_HEADER_ID                     dwBigFourCC('O', 'g', 'g', 'S')
#define BYTE0(arg)        (* (char *)&(arg))
#define LOAD_BYTE(arg1, arg2)  BYTE0(arg2) = *((char *)(arg1)); 

typedef struct _OGG_MFMTRECG_HEADER_INFO_T
{
    char*        ui1_buf;
    char        ui1_header_type;
    char        ui1_page_segment;
    char        segment_table[OGG_PAGE_SEGMENT_MAX];
}OGG_MFMTRECG_HEADER_INFO_T;

bool IsOggFile(const char* url, int fd, int64_t offset)
{
    unsigned char        ac_tmp[15];
    uint32_t            ui4_id3v2_offset = 0;    
    uint32_t            ui4_id3_tag_offset = 0;

    if(url)
    {
        int32_t  sz_strlen = strlen(url);
        if(sz_strlen > 4)
        {
            char* ps_surfix = (char*)url + sz_strlen - 4;
            if(strcasecmp(ps_surfix, ".ogg"))   //retriver mp3 use cmpb player
            {
                return false;
            }
            else
            {
                if(0 == strncasecmp(url,"/mnt/", 5) ||
                   0 == strncasecmp(url,"/storage/sd", 11))
                {
                    LOGI("It's local ogg, use cmpb player!!!");
                    return false;
                }
                LOGI("this file is ogg file, android default player !!\n");
                return true;
            }
        }
    }    
    
    return false;
}

static uint32_t FindMp3HeaderOffset(int fd, int64_t offset)
{
	unsigned char        ac_tmp[15];
	uint32_t             ui4_header_offset = 0;
	
	lseek(fd, offset, SEEK_SET);
	while (true)
	{
		memset(ac_tmp, 0, sizeof(ac_tmp));
		if (ui4_header_offset >= 1024 * 2 || read(fd, ac_tmp, 10) == 0) // read 2KB or whole file
		{
			break;
		}
		
		for (int i = 0; i < 10; i++)
		{
			if (ac_tmp[i] == 0xFF)
			{
				LOGI("FindMp3HeaderOffset Find in offset: %d \n",ui4_header_offset);
				return ui4_header_offset;
			}
			else
			{
				//LOGI("0x%x!!", ac_tmp[i]);
				ui4_header_offset++;
			}
		}
	}
	return 0;
}


/////////////just for mp3 check/////////////////////////////
#define MFMTRRECG_MP3_MAX_PARSING_LEN                       (1024*5)
#define MP3_MIDXBULD_MAX_BITRATE_IDX                        (uint8_t)(14)

/* VERSION_1 */
static uint16_t const MPEG_V1_BITRATE_TAB[3][16] =
{
    {0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0}, /* Layer III */
    {0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0}, /* Layer II */
    {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0}  /* Layer I */
};

/* VERSION_2, VERSION_2_5 */
static uint16_t const MPEG_V2_BITRATE_TAB[2][16] =
{
    {0,  8, 16, 24, 32, 40, 48,  56,  64,  80,  96, 112, 128, 144, 160, 0},    /* Layer II & III */
    {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0}     /* Layer I */
};

static uint16_t const MPEG_SAMPLERATE_TAB[3] = {44100, 48000, 32000};

#define IS_MP3_HEADER(ac_arry)         \
    ((ac_arry[0] == 0xFF)          &&  \
     ((ac_arry[1] & 0xE0) == 0xE0) &&  \
     ((ac_arry[2] >> 4) != 0)      &&  \
     ((ac_arry[2] >> 4) != 15)         \
    )

#define MP3_GET_BITRATE(ui4_bitrate, ui1_version, ui1_layer, ui1_rate_index)   \
{                                                                              \
    switch (ui1_version)                                                       \
    {                                                                          \
        case 3:  /* Version 1 */  /* kbps */                                      \
            if( (ui1_layer<= 3) && (ui1_layer > 0) && (ui1_rate_index<=MP3_MIDXBULD_MAX_BITRATE_IDX) )  \
            {\
                ui4_bitrate = MPEG_V1_BITRATE_TAB[ui1_layer - 1][ui1_rate_index];  \
            }\
            break;                                                             \
        case 2:  /* Version 2 */                                                 \
        case 0:  /* Version 2.5 */  /* kbps */                                    \
            if( (((ui1_layer - 1) >> 1)<2) && (((ui1_layer - 1) >> 1) >= 0) && (ui1_rate_index<=MP3_MIDXBULD_MAX_BITRATE_IDX) )  \
            {\
                ui4_bitrate = MPEG_V2_BITRATE_TAB[(ui1_layer - 1) >> 1][ui1_rate_index];  \
            }\
            break;                                                             \
        default:                                                               \
            ui4_bitrate = 0;                                                   \
            break;                                                             \
    }                                                                          \
    ui4_bitrate *= 1000;    /* bps */                                           \
}


static bool mp3_check_format(int fd, int64_t offset)
{
    uint8_t     ac_tag[3];

    uint8_t     *ac_buff;
    uint8_t     ac_tmp[10];
    uint32_t    ui4_actual_read;
    uint32_t    ui4_idx;
    uint32_t    ui4_frame_length;
    uint8_t     ui1_rate_index;
    uint32_t    ui4_bitrate = 0;
    uint32_t    ui4_sample_rate = 0;
    uint8_t     ui1_slot_bytes = 0;
    uint32_t    ui4_frame_slots = 0;
    uint8_t     ui1_padding = 0;

    uint8_t     ui1_layer, ui1_version;

    uint32_t    ui4_id3v2_offset = 0;
    int32_t     i4_ret = 0;
    

    /* check ID3v2 tag is exist or NOT */

    do
    {
        uint32_t ui4_id3_tag_offset;
        lseek(fd, offset + ui4_id3v2_offset, SEEK_SET);
        if((i4_ret = read(fd,
                          ac_tmp,
                          10)) <= 0)
        {
            LOGI("mp3_check_format read fail line:%d i4_ret=%d", __LINE__, i4_ret);
            return false;
        }
        if (memcmp((void*)"ID3", ac_tmp, 3) == 0)
        {
                 /* calculate ID3v2 tag offset */
            ui4_id3_tag_offset = (((uint32_t)(ac_tmp[6])) << 21)
                                 | (((uint32_t)(ac_tmp[7])) << 14)
                                 | (((uint32_t)(ac_tmp[8])) << 7)
                                 | ((uint32_t)(ac_tmp[9]));
                 
            ui4_id3v2_offset += ui4_id3_tag_offset;
            ui4_id3v2_offset += 10; /*header of id3 tag v2*/
        }
        else
        {
            break;
        }
    }while(1);

    /* check MP3 frame header */
    /*
     *  syncword             11 bits
     *  version              2 bits
     *  layer                2 bits
     *
     */

    ac_buff = (uint8_t*)malloc(sizeof(uint8_t) * MFMTRRECG_MP3_MAX_PARSING_LEN);
    if (ac_buff == NULL)
    {
        LOGI("mp3_check_format alloc fail line:%d ", __LINE__);
        return false;
    }
    
    lseek(fd, offset + ui4_id3v2_offset, SEEK_SET);
    if((i4_ret = read(fd,
                      ac_buff,
                      MFMTRRECG_MP3_MAX_PARSING_LEN)) <= 0)
    {
        if (ac_buff != NULL)
        {
            free(ac_buff);
        }
        LOGI("mp3_check_format read fail line:%d i4_ret=%d", __LINE__, i4_ret);
        return false;
    }

    ui4_actual_read = i4_ret;
    ui4_idx = 0;
    /* find a possible audio frame header */
    while (ui4_idx < ui4_actual_read)
    {
        if ((ac_buff[ui4_idx] != 0xFF)
            || ((ac_buff[ui4_idx + 1] & 0xE0) != 0xE0)
            || ((ac_buff[ui4_idx + 1] & 0x18) >> 3 == 1)      /* version description is set to reserved */
            || ((ac_buff[ui4_idx + 1] & 0x06) >> 1 == 0)      /* layer description is set to reserved */
            || ((ac_buff[ui4_idx + 2] >> 4) == 0)
            || ((ac_buff[ui4_idx + 2] >> 4) == 15))
        {
            ui4_idx++;
        }
        else
        {
            memcpy(ac_tag, ac_buff + ui4_idx, 3);
            ui1_version = ((ac_tag[1] & 0x18) >> 3);
            ui1_layer = ((ac_tag[1] & 0x6) >> 1);

            /* bitrate index */
            ui1_rate_index = ac_tag[2] >> 4;
            MP3_GET_BITRATE(ui4_bitrate, ui1_version, ui1_layer, ui1_rate_index);            

            /* sampling frequency index */
            ui1_rate_index = ((ac_tag[2] & 0xc) >> 2);
            ui4_sample_rate = 0;
            if (ui1_rate_index != 3)    /* not reserved */
            {
                ui4_sample_rate = MPEG_SAMPLERATE_TAB[ui1_rate_index];
            }
            switch (ui1_version)
            {
                case 2: /* Version 2 */
                    ui4_sample_rate >>= 1;
                    break;
                case 0: /* Version 2.5 */
                    ui4_sample_rate >>= 2;
                    break;
            }
            if (ui4_sample_rate == 0)
            {
                /* read next 1 byte */
                ui4_idx++;
                continue;
            }
            /* padding bit */
            ui1_padding = ((ac_tag[2] & 0x2)>>1);

            /* calculate frame length */
            ui4_frame_length = 0;
            if (ui4_bitrate != 0)
            {
                ui1_slot_bytes = 1;
                if (ui1_layer == 3)         /* Layer 1 */
                {
                    ui4_frame_slots = 12;
                    ui1_slot_bytes = 4;
                }
                else if ((ui1_layer == 1)   /* Layer 3 */
                    /* Version 2 or Version 2.5 */
                    && (ui1_version == 2 || ui1_version == 0))
                {
                    ui4_frame_slots = 72;
                }
                else
                {
                    ui4_frame_slots = 144;
                }
                if (ui4_sample_rate != 0)
                {
                    ui4_frame_length = (ui4_frame_slots * ui4_bitrate / ui4_sample_rate + ui1_padding) * ui1_slot_bytes;
                }
            }
            
            /* jump to check next frame header file is valid or invalid */
            if (ui4_idx + ui4_frame_length < ui4_actual_read)
            {
                memcpy(ac_tmp, (ac_buff + ui4_idx + ui4_frame_length), 3);
            }
            else
            {
                lseek(fd, offset + ui4_id3v2_offset + ui4_idx + ui4_frame_length, SEEK_SET);
                if((i4_ret = read(fd,
                                  ac_tmp,
                                  3)) <= 0)
                {
                    if (ac_buff != NULL)
                    {
                        free(ac_buff);
                    }
                    LOGI("mp3_check_format read fail line:%d i4_ret=%d", __LINE__, i4_ret);
                    return false;
                }
            }
            
            /* next frame header is also valid */
            if (IS_MP3_HEADER(ac_tmp))
            {
                /*read one more frame to avoid error recgnize*/
                if (ui4_idx + ui4_frame_length < ui4_actual_read)
                {
                    ui4_idx += ui4_frame_length;
                }
                else /*already read for the 2nd frame*/
                {
                    ui4_id3v2_offset += ui4_idx;
                    ui4_idx = 0;
                    ui4_actual_read = i4_ret;
                }

                /* bitrate index */
                ui1_rate_index = ac_tmp[2] >> 4;

                MP3_GET_BITRATE(ui4_bitrate, ui1_version, ui1_layer, ui1_rate_index);              

                /* padding bit */
                ui1_padding = ((ac_tmp[2] & 0x2)>>1);

                if(0 != ui4_bitrate)
                {
                    ui4_frame_length = 
                        (ui4_frame_slots * ui4_bitrate / ui4_sample_rate + ui1_padding) * 
                          ui1_slot_bytes;
                }
                else
                {   /*error */
                    if (ac_buff != NULL)
                    {
                        free(ac_buff);
                    }
                    LOGI("mp3_check_format error param 0 == ui4_bitrate line:%d ", __LINE__);
                    return false;
                }
                /* jump to check next frame header file is valid or invalid */
                if (ui4_idx + ui4_frame_length < ui4_actual_read)
                {
                    memcpy(ac_tmp, (ac_buff + ui4_idx + ui4_frame_length), 3);
                }
                else
                {
                    lseek(fd, offset + ui4_id3v2_offset + ui4_idx + ui4_frame_length, SEEK_SET);
                    if((i4_ret = read(fd,
                                      ac_tmp,
                                      3)) <= 0)
                    {
                        if (ac_buff != NULL)
                        {
                            free(ac_buff);
                        }
                        LOGI("mp3_check_format read fail line:%d i4_ret=%d", __LINE__, i4_ret);
                        return false;
                    }
                }

                /* next frame header is also valid */
                if (IS_MP3_HEADER(ac_tmp))
                {
                    /* then the media type & subtype can be set correctly */
                    if ((ui1_version == 0)
                        || (ui1_version == 2)
                        || (ui1_version == 3))
                    {
                        if ((ui1_layer == 3)        /* Layer 1 */
                            || (ui1_layer == 2))    /* Layer 2 */
                        {
                            LOGI("mp3_check_format ok line:%d ", __LINE__);
                            if (ac_buff != NULL)
                            {
                                free(ac_buff);
                            }
                            return true;
                        }
                        else if (ui1_layer == 1)    /* Layer 3 */
                        {
                            LOGI("mp3_check_format ok line:%d ", __LINE__);
                            if (ac_buff != NULL)
                            {
                                free(ac_buff);
                            }
                            return true;
                        }
                        else
                        {
                            if (ac_buff)
                            {
                                free(ac_buff);
                            }
                            LOGI("mp3_check_format fail line:%d ", __LINE__);
                            return false;
                        }
                    }
                    if (ac_buff)
                    {
                        free(ac_buff);
                    }
                    LOGI("mp3_check_format fail line:%d ", __LINE__);
                    return true;
                }
            }                

            /* read next 1 byte */
            ui4_idx++;
        }
    }
    if (ac_buff)
    {
        free(ac_buff);
    }
    LOGI("mp3_check_format fail line:%d ", __LINE__);
    return false;
}

bool IsMp3File(const char* url, int fd, int64_t offset)
{
    bool b_ret= false;
    /* check ID3v2 tag is exist or NOT */
    if(url)
    {
        int32_t  sz_strlen = strlen(url);
        if(sz_strlen > 4)
        {
            char* ps_surfix = (char*)url + sz_strlen - 4;
            if(strcasecmp(ps_surfix, ".mp3"))   //retriver mp3 use cmpb player
            {
                return false;
            }
        }
    }
    else
    {
        b_ret = mp3_check_format(fd, offset);

        if(b_ret)
        {
            ///check is camera video or not
            char aPathIn[64]={0};
            char aFilePath[64]={0};
            int  ret;
            
            sprintf(aPathIn, "/proc/self/fd/%d", fd);
            memset(aFilePath, 0, sizeof(aFilePath));
            ret = readlink(aPathIn, aFilePath, 64);
            aFilePath[63] = 0;
            if(ret > 0)
            {
                if(0 == strncasecmp(aFilePath,"/mnt/", 5) ||
                   0 == strncasecmp(aFilePath,"/storage/sd", 11))
                {
                    LOGI("It's local mp3, use cmpb player!!!");
                    b_ret = false;
                }
            }
        }        

        lseek(fd, offset, SEEK_SET);
    }
    //if(b_ret)
    {
        LOGI("this file is mp3 or not %d !!\n", b_ret);
    }
    return b_ret;
}

bool IsMidiFile(const char* url, int fd, int64_t offset)
{
    unsigned char        ac_tmp[15];
    unsigned char        ac_mid_tag[8] = {0X4D,0X54,0X68,0X64,0X00,0X00,0X00,0X06}; //"Mthd", size=0x06

    if(url)
    {
        int32_t  sz_strlen = strlen(url);
        if(sz_strlen > 4)
        {
            char* ps_surfix = (char*)url + sz_strlen - 4;
            if(strcasecmp(ps_surfix, ".mid"))   //retriver mp3 use cmpb player
            {
                return false;
            }
        }
    }
    else
    {
        lseek(fd, offset, SEEK_SET);
        memset(ac_tmp, 0, sizeof(ac_tmp));
        do
        {
            read(fd, ac_tmp, 10);
            lseek(fd, offset, SEEK_SET);  ////restore postion
            if (memcmp(ac_mid_tag, ac_tmp, 8) != 0)
            {
                return false;
            }
            else
            {
                break;
            }
        }while(0);
    }
    
    LOGI("it's mid file, use android default player!!!");
    return true;
}

bool IsWaveFile(const char* url, int fd, int64_t offset)
{
    unsigned char        ac_tmp[15];

    if(url)
    {
        int32_t  sz_strlen = strlen(url);
        if(sz_strlen > 4)
        {
            char* ps_surfix = (char*)url + sz_strlen - 4;
            if(strcasecmp(ps_surfix, ".wav"))   //retriver mp3 use cmpb player
            {
                return false;
            }
        }
    }
    else
    {
        lseek(fd, offset, SEEK_SET);
        memset(ac_tmp, 0, sizeof(ac_tmp));
        do
        {
            read(fd, ac_tmp, 12);
            lseek(fd, offset, SEEK_SET);  ////restore postion
            if (memcmp("RIFF", ac_tmp, 4) != 0 ||
                memcmp("WAVE", ac_tmp+8, 4) != 0)
            {
                return false;
            }
            else
            {
            	///check is usb/sdcard local file or not
	            char aPathIn[64]={0};
	            char aFilePath[64]={0};
	            int  ret = 0;
	            
	            sprintf(aPathIn, "/proc/self/fd/%d", fd);
	            memset(aFilePath, 0, sizeof(aFilePath));
	            ret = readlink(aPathIn, aFilePath, 64);
	            aFilePath[63] = 0;
	            if(ret > 0)
	            {
	                if(0 == strncasecmp(aFilePath,"/mnt/", 5) ||
	                   0 == strncasecmp(aFilePath,"/storage/sd", 11))
	                {
	                    LOGI("It's local wav file, use cmpb player!!!");
	                    return false;
	                }
	            }
            }
        }while(0);
    }
    
    LOGI("it's WAV file, use android default player!!!");
    return true;
}

bool IsAmrFile(const char* url, int fd, int64_t offset)
{
    unsigned char        ac_tmp[15];
    unsigned char        ac_amr_tag[6] = {'#', '!', 'A', 'M', 'R', '\n'};

    if(url)
    {
        int32_t  sz_strlen = strlen(url);
        if(sz_strlen > 4)
        {
            char* ps_surfix = (char*)url + sz_strlen - 4;
            if(strcasecmp(ps_surfix, ".amr"))   
            {
                return false;
            }
        }
    }
    else
    {
        lseek(fd, offset, SEEK_SET);
        memset(ac_tmp, 0, sizeof(ac_tmp));
        do
        {
            read(fd, ac_tmp, 6);
            lseek(fd, offset, SEEK_SET);  ////restore postion
            if (memcmp(ac_amr_tag, ac_tmp, 6) != 0)
            {
                return false;
            }
            else
            {
                break;
            }
        }while(0);
    }
    
    LOGI("it's AMR file, use android default player!!!");
    return true;
}

bool ogg_has_video_confirm(int    fd, int64_t    offset)     
{
    uint32_t                            i4_ret = 0;
    uint64_t                            ui8_cur_pos = 0;
    uint32_t                            ui4_page_cnt = 0;
    uint32_t                              ui4_box_type = 0;
    uint32_t                              ui4_data = 0;
    OGG_MFMTRECG_HEADER_INFO_T            ogg_mfmtrecg_header_info;
    uint32_t                            ui4_cnt = 0;
    const char                          i1_video_header[7] = {0x01,'v','i','d','e','o','\0'};

    memset(&ogg_mfmtrecg_header_info, 0, sizeof(OGG_MFMTRECG_HEADER_INFO_T));
    ogg_mfmtrecg_header_info.ui1_buf = (char *)malloc(OGG_MAX_HEAD_LEN + 1);
    if (ogg_mfmtrecg_header_info.ui1_buf == NULL)
    {
        return true;
    }
    
    while (1)
    {
        i4_ret = lseek(fd, offset + ui8_cur_pos, SEEK_SET);
        if(i4_ret < 0)
        {
            break;
        }

        i4_ret = read(fd, (void*)&ui4_data, sizeof(ui4_data));
        if(i4_ret <= 0)
        {
            i4_ret = -1;
            break;
        }
        
        ui4_box_type = ((ui4_data&0xff000000)>>24) 
                        | ((ui4_data&0xff0000)>>8) 
                        | ((ui4_data&0xff00)<<8) 
                        | ((ui4_data&0xff)<<24);
        if(ui4_box_type == OGG_HEADER_ID)
        {
            i4_ret = lseek(fd, offset + ui8_cur_pos, SEEK_SET);
            if(i4_ret < 0)
            {
                break;
            }
            
            i4_ret = read(fd, (void*)ogg_mfmtrecg_header_info.ui1_buf, OGG_MAX_HEAD_LEN);
            if (i4_ret <= 0)
            {
                i4_ret = -1;
                break;
            }
            LOAD_BYTE(ogg_mfmtrecg_header_info.ui1_buf + 5,  ogg_mfmtrecg_header_info.ui1_header_type);
            LOAD_BYTE(ogg_mfmtrecg_header_info.ui1_buf + 26, ogg_mfmtrecg_header_info.ui1_page_segment);
            ui8_cur_pos += 27;
            
            if(ogg_mfmtrecg_header_info.ui1_page_segment <= 255)
            {
                for(ui4_cnt=0; ui4_cnt < ogg_mfmtrecg_header_info.ui1_page_segment; ui4_cnt++)
                {
                    LOAD_BYTE(ogg_mfmtrecg_header_info.ui1_buf + 27 + ui4_cnt, ogg_mfmtrecg_header_info.segment_table[ui4_cnt]);
                    ui8_cur_pos ++;
                }
            }
            if ((ogg_mfmtrecg_header_info.ui1_header_type & 0x02) >> 1)
            {
                i4_ret = lseek(fd, offset + ui8_cur_pos, SEEK_SET);
                if(i4_ret < 0)
                {
                    break;
                }
                i4_ret = read(fd, (void*)ogg_mfmtrecg_header_info.ui1_buf, 9);
                if (i4_ret <= 0)
                {
                    i4_ret = -1;
                    break;
                }
                if (!memcmp(ogg_mfmtrecg_header_info.ui1_buf, i1_video_header, 6))
                {
                    i4_ret = 0;
                    break;
                }
            }

            for (ui4_cnt = 0; ui4_cnt < ogg_mfmtrecg_header_info.ui1_page_segment; ui4_cnt++)
            {
                ui8_cur_pos += ogg_mfmtrecg_header_info.segment_table[ui4_cnt];
            }
        }
        
        ui4_page_cnt ++;
        if (ui4_page_cnt > 200)
        {
            if (ogg_mfmtrecg_header_info.ui1_buf != NULL)
            {
                free(ogg_mfmtrecg_header_info.ui1_buf);
            }
            return false;
        }
    }
    
    if (ogg_mfmtrecg_header_info.ui1_buf != NULL)
    {
        free(ogg_mfmtrecg_header_info.ui1_buf);
    }
    if (i4_ret == 0)
    {
        return true;
    }
    
    return false;
}
//MTK END. PIC:Guanghui.wang

/*****************************************************************************
 *                                                                           *
 *                     Built-In Factory Implementations                      *
 *                                                                           *
 *****************************************************************************/

int MediaPlayerFactory::IFactory::mPlayerType = -1; 


class CmpbPlayerFactory :
    public MediaPlayerFactory::IFactory {
  public:
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const char* url,
                               float curScore) {
        float score=2.0;
        
        ALOGI("scoreFactory (%s) Line: %d", url,__LINE__);  
        
        #ifdef CC_MTK_ANDROID_GOOGLE_TV_SUPPORT
                  score=100.0;
        #endif
        bool    bUseCmpb=false;    

        //bUseCmpb = sUseCmpb;

        if(CMPB_PLAYER==mPlayerType)
        {
            bUseCmpb=true;
        }
        else
        {
            bUseCmpb=false;
        }
//Must Keep for Support customer.        
#if 1 //if want change to cmpb, need enable this part
        //DTV00638590 http://*.mp4
        String8 mUri;
        mUri = url;
        
        ssize_t index_1 =mUri.find("http://");
        ssize_t index_m3u8 =mUri.find("m3u8");
        
        ssize_t index_mp4 = mUri.find(".mp4");

        ssize_t index_ts =mUri.find(".ts");
        ssize_t index_mkv =mUri.find(".mkv");
		ssize_t index_avi =mUri.find("avi");
		ssize_t index_live =mUri.find(".264tsuni");
        
        ALOGI("scoreFactory index_m3u8:%d index_avi:%d index_live:%d, Line: %d", index_m3u8, index_avi, index_live, __LINE__);  
        do
        {
            //not start with http://
            if (index_1<0)
            {
                break;
            }

            //m3u8 will use Nuplayer
            if (index_m3u8>0)
            {
                break;
            }
            //http:// && special case
            if (index_ts>0 ||
                index_mkv>0 ||
				index_avi>0 ||
				index_live>0)
            {
                ALOGD(" http://*.ts,*.mkv,*.avi yunnan_livetv Use CMPB Player");
                score=2.0;
                
                ALOGD("MediaPlayerFactory: scoreFactory return %f Line %d!!\n", score,__LINE__);
                
                return score;
            }
            
            break;
        }while(false);

#endif 
        LOGI("MediaPlayerFactory: scoreFactory bUseCmpb=%d!!\n", bUseCmpb);
        if ( !bUseCmpb )
        {
            return 0.0;
        }

#if 1// DTV00629857        
        if (IsMp3File(url, 0, 0)  ||
            IsWaveFile(url, 0, 0) ||
            IsAmrFile(url, 0, 0)  ||
            IsMidiFile(url, 0, 0) ||
            IsOggFile(url, 0, 0) ) //use android default player to play mp3 or mid or wave or amr files.
        {
            LOGI("this file is one of mp3/wav/amr/midi/ogg file, android default player, uri = %s !!\n",url);
            score = 0.0;
        }
#endif        
        ALOGD("MediaPlayerFactory: scoreFactory return %f Line %d!!\n", score,__LINE__);

        return score;
    }

    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const sp<IStreamSource> &source,
                               float curScore) {
        bool    bUseCmpb=false;    

        //bUseCmpb = sUseCmpb;
        
        if(CMPB_PLAYER==mPlayerType)
        {
            bUseCmpb=true;
        }
        else
        {
            bUseCmpb=false;
        }

        if ( !bUseCmpb )
        {
            return 0.0;
        }
        #ifdef CC_MTK_ANDROID_GOOGLE_TV_SUPPORT
            return 100.0;
        #else
        return 1.0;
        #endif
    }
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               int fd,
                               int64_t offset,
                               int64_t length,
                               float curScore) {
        char buf[20];
        float score=2.0;

        #ifdef CC_MTK_ANDROID_GOOGLE_TV_SUPPORT
                  score=100.0;
        #endif
        bool    bUseCmpb=false;    

        //bUseCmpb = sUseCmpb;
        
        if(CMPB_PLAYER==mPlayerType)
        {
            bUseCmpb=true;
        }
        else
        {
            bUseCmpb=false;
        }

        if ( !bUseCmpb )
        {
            return 0.0;
        }
        
        lseek(fd, offset, SEEK_SET);
        read(fd, buf, sizeof(buf));
        lseek(fd, offset, SEEK_SET);

        long ident = *((long*)buf);

        LOGI("In MediaPlayerService: call scoreFactory(1) ident = %x !\n", ident);

        // Ogg vorbis?
        if (ident == 0x5367674f) // 'OggS'
        {
            if (ogg_has_video_confirm(fd, offset))
            {
                LOGI("this file is ogm file, cmpb player !!\n");                
                lseek(fd, offset, SEEK_SET);
                #ifdef CC_MTK_ANDROID_GOOGLE_TV_SUPPORT
                    return 100.0;
                #else
                return 1.0;
                #endif
            }
            else
            {
                LOGI("this file is ogg file, android default player !!\n");
                ///check is camera video or not
                char aPathIn[64]={0};
                char aFilePath[64]={0};
                int  ret;
                
                sprintf(aPathIn, "/proc/self/fd/%d", fd);
                memset(aFilePath, 0, sizeof(aFilePath));
                ret = readlink(aPathIn, aFilePath, 64);
                aFilePath[63] = 0;
                if(ret > 0)
                {
                    if(0 == strncasecmp(aFilePath,"/mnt/", 5) ||
                       0 == strncasecmp(aFilePath,"/storage/sd", 11))
                    {
                        LOGI("It's local ogg, use cmpb player!!!");
                        return 100.0;
                    }
                }
                score = 0.0;
            }
            lseek(fd, offset, SEEK_SET);
        }
#if 1// DTV00629857        
        else if (IsMp3File(NULL, fd, offset) ||
                 IsWaveFile(NULL, fd, offset)||
                 IsAmrFile(NULL, fd, offset) ||
                 IsMidiFile(NULL, fd, offset))
        {
            score = 0.0;
        }
#endif
        return score;
    }

    virtual sp<MediaPlayerBase> createPlayer() {
        ALOGV(" create CmpbPlayer");
        return new CmpbPlayer();
    }
};

#ifdef SUPPORT_MTK_STREAM_PLAYER
class MtkStreamPlayerFactory:public MediaPlayerFactory::IFactory {
  public:

	virtual float scoreFactory(const sp<IMediaPlayer>& client,
							   const sp<IStreamSource> &source,
							   float curScore) 
 	{
        float score=0.0;
        
        if(MTK_STREAM_PLAYER==mPlayerType)
        {
            score=100.0;
        }
        else
        {
            score=0.0;
        }

		LOGE("(1)return MtkStreamPlayer score = %f.\r\n", score); 
        
		return score;
	}

	virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const char* url,
                               float curScore) {
		int type;
		float score=0.0;
		MtkStreamPlayerUtils utils;
		IMTK_STREAM_PARAM_T param;

		utils.getMimeHeader(url);
		utils.parsePlayerParam(url, NULL, &param);
        type = utils.getPlayerType(url);

        if(MTK_STREAM_PLAYER==mPlayerType)
        {
            if((type>0 && type < 4) || (type == 7))
            {
                score=100.0;
            }
            else
            {
                LOGI("MtkStreamPlayer, not msss/dash/hls case, change to CmpbPlayer.");
            }
        }
		else
		{
			score=0.0;
		}

        LOGE("(2)return MtkStreamPlayer score %f.\r\n", score);      
        return score;
    }
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               int fd,
                               int64_t offset,
                               int64_t length,
                               float curScore) 
    {
    	LOGE("(3)return MtkStreamPlayer score 0.0.\r\n"); 
        
        float score=0.0;
        
        if(MTK_STREAM_PLAYER==mPlayerType)
        {
            score=100.0;
        }
        else
        {
            score=0.0;
        }
		return score;
	}
	
	virtual sp<MediaPlayerBase> createPlayer() 
	{
        ALOGV(" create MtkStreamPlayer");
        return new MtkStreamPlayer();
    }

};
#endif

class StagefrightPlayerFactory :
    public MediaPlayerFactory::IFactory {
  public:
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               int fd,
                               int64_t offset,
                               int64_t length,
                               float curScore) {
        char buf[20];
        lseek(fd, offset, SEEK_SET);
        read(fd, buf, sizeof(buf));
        lseek(fd, offset, SEEK_SET);

        long ident = *((long*)buf);

#ifdef CC_SUPPORT_TV_STAGEFRIGHT
       if(STAGEFRIGHT_PLAYER==mPlayerType)
       {
           return 1.0;
       }
#endif

        // Ogg vorbis?
        if (ident == 0x5367674f && !ogg_has_video_confirm(fd, offset)) // 'OggS'
            return 1.0;

        return 0.0;
    }

    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const char* url,
                               float curScore) {
        float kOurScore = 0.0;

        if (IsMp3File(url, 0, 0)  ||
            IsWaveFile(url, 0, 0) ||
            IsAmrFile(url, 0, 0)  ||
            IsOggFile(url, 0, 0) ) //use android default player to play mp3 or mid or wave or amr files.
        {
            LOGI("this file is one of mp3/wav/amr/ogg file, stagefright player, uri = %s !!\n",url);
            kOurScore = 1.0;
        }   

        return kOurScore;
    }

    virtual sp<MediaPlayerBase> createPlayer() {
        ALOGV(" create StagefrightPlayer");
        return new StagefrightPlayer();
    }
};

class NuPlayerFactory : public MediaPlayerFactory::IFactory {
  public:
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const char* url,
                               float curScore) {
        static const float kOurScore = 0.8;

        if (kOurScore <= curScore)
            return 0.0;

        if (!strncasecmp("http://", url, 7)
                || !strncasecmp("https://", url, 8)
                || !strncasecmp("file://", url, 7)) {
            size_t len = strlen(url);
            if (len >= 5 && !strcasecmp(".m3u8", &url[len - 5])) {
                return kOurScore;
            }

            if (strstr(url,"m3u8")) {
                return kOurScore;
            }

            if ((len >= 4 && !strcasecmp(".sdp", &url[len - 4])) || strstr(url, ".sdp?")) {
                return kOurScore;
            }
        }

        if (!strncasecmp("rtsp://", url, 7)) {
            return kOurScore;
        }
       #if 0 // Nuplayer BK. Must keep for compare awsomeplayer/Nuplayer
       //DTV00616549 http://127.0.0.1:51818/*.ts
       String8 mUri;
       mUri = url;
       ssize_t index_1 =0;
       ssize_t index_2 =0;
       index_1 = mUri.find("http://127.0.0.1:51818/");
       index_2 = mUri.find(".ts");
       
       if (index_1>=0 &&
           index_2>0)
       {
           ALOGD(" http://127.0.0.1:51818/*.ts Use NuPlayer");
           return kOurScore;
       }
       #endif
       
        return 0.0;
    }

    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const sp<IStreamSource> &source,
                               float curScore) {
        return 1.0;
    }

    virtual sp<MediaPlayerBase> createPlayer() {
        ALOGV(" create NuPlayer");
        return new NuPlayerDriver;
    }
};

class SonivoxPlayerFactory : public MediaPlayerFactory::IFactory {
  public:
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const char* url,
                               float curScore) {
        static const float kOurScore = 0.4;
        static const char* const FILE_EXTS[] = { ".mid",
                                                 ".midi",
                                                 ".smf",
                                                 ".xmf",
                                                 ".mxmf",
                                                 ".imy",
                                                 ".rtttl",
                                                 ".rtx",
                                                 ".ota" };
        if (kOurScore <= curScore)
            return 0.0;

        // use MidiFile for MIDI extensions
        int lenURL = strlen(url);
        for (int i = 0; i < NELEM(FILE_EXTS); ++i) {
            int len = strlen(FILE_EXTS[i]);
            int start = lenURL - len;
            if (start > 0) {
                if (!strncasecmp(url + start, FILE_EXTS[i], len)) {
                    return kOurScore;
                }
            }
        }

        return 0.0;
    }

    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               int fd,
                               int64_t offset,
                               int64_t length,
                               float curScore) {
        static const float kOurScore = 0.8;

        if (kOurScore <= curScore)
            return 0.0;

        // Some kind of MIDI?
        EAS_DATA_HANDLE easdata;
        if (EAS_Init(&easdata) == EAS_SUCCESS) {
            EAS_FILE locator;
            locator.path = NULL;
            locator.fd = fd;
            locator.offset = offset;
            locator.length = length;
            EAS_HANDLE  eashandle;
            if (EAS_OpenFile(easdata, &locator, &eashandle) == EAS_SUCCESS) {
                EAS_CloseFile(easdata, eashandle);
                EAS_Shutdown(easdata);
                return kOurScore;
            }
            EAS_Shutdown(easdata);
        }

        return 0.0;
    }

    virtual sp<MediaPlayerBase> createPlayer() {
        ALOGV(" create MidiFile");
        return new MidiFile();
    }
};

class TestPlayerFactory : public MediaPlayerFactory::IFactory {
  public:
    virtual float scoreFactory(const sp<IMediaPlayer>& client,
                               const char* url,
                               float curScore) {
        if (TestPlayerStub::canBeUsed(url)) {
            return 1.0;
        }

        return 0.0;
    }

    virtual sp<MediaPlayerBase> createPlayer() {
        ALOGV("Create Test Player stub");
        return new TestPlayerStub();
    }
};

void MediaPlayerFactory::registerBuiltinFactories() {
    Mutex::Autolock lock_(&sLock);

    if (sInitComplete)
        return;

    registerFactory_l(new CmpbPlayerFactory(), CMPB_PLAYER);

    registerFactory_l(new StagefrightPlayerFactory(), STAGEFRIGHT_PLAYER);
    registerFactory_l(new NuPlayerFactory(), NU_PLAYER);
    registerFactory_l(new SonivoxPlayerFactory(), SONIVOX_PLAYER);
    registerFactory_l(new TestPlayerFactory(), TEST_PLAYER);

#ifdef SUPPORT_MTK_STREAM_PLAYER
    registerFactory_l(new MtkStreamPlayerFactory(), MTK_STREAM_PLAYER);
#endif
    sInitComplete = true;
}

}  // namespace android
