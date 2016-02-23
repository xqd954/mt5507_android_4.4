#ifndef FORMAT_H
#define FORMAT_H

typedef enum {
    FORMAT_UNKNOWN = -1,
    FORMAT_MPEG   = 0,
    FORMAT_PCM_S16LE = 1,
    FORMAT_AAC   = 2,
    FORMAT_AC3   = 3,
    FORMAT_ALAW = 4,
    FORMAT_MULAW = 5,
    FORMAT_DTS = 6,
    FORMAT_PCM_S16BE = 7,
    FORMAT_FLAC = 8,
    FORMAT_COOK = 9,
    FORMAT_PCM_U8 = 10,
    FORMAT_ADPCM = 11,
    FORMAT_AMR  = 12,
    FORMAT_RAAC  = 13,
    FORMAT_WMA  = 14,
    FORMAT_WMAPRO   = 15,
    FORMAT_PCM_BLURAY  = 16,
    FORMAT_ALAC  = 17,
    FORMAT_VORBIS    = 18,
    FORMAT_UNSUPPORT ,
    FORMAT_MAX    

} aformat_t;

#define AUDIO_EXTRA_DATA_SIZE   (2048)
#define IS_AFMT_VALID(afmt)	((afmt > FORMAT_UNKNOWN) && (afmt < FORMAT_MAX))
    
#define IS_AUIDO_NEED_EXT_INFO(afmt) ((afmt == FORMAT_ADPCM) \
								 ||(afmt == FORMAT_WMA) \
								 ||(afmt == FORMAT_WMAPRO) \
								 ||(afmt == FORMAT_PCM_S16BE) \
								 ||(afmt == FORMAT_PCM_S16LE) \
								 ||(afmt == FORMAT_PCM_U8) \
								 ||(afmt == FORMAT_PCM_BLURAY) \
								 ||(afmt == FORMAT_AMR)\
								 ||(afmt == FORMAT_ALAC)\
								 ||(afmt == FORMAT_AC3) \
								 ||(afmt == FORMAT_FLAC) )


#define IS_AUDIO_NOT_SUPPORT_EXCEED_2CH(afmt) ((afmt == FORMAT_RAAC) \
										||(afmt == FORMAT_COOK) \
										||(afmt == FORMAT_FLAC))

#define IS_AUIDO_NEED_PREFEED_HEADER(afmt) ((afmt == FORMAT_VORBIS) )

#endif /* FORMAT_H */

