package com.mediatek.mmpcm.videoimpl;
import com.mediatek.mmpcm.fileimpl.FileConst;


public class VideoConst {
	/* Play Status */
	public static final int PLAY_STATUS_ERROR = -1;
	public static final int PLAY_STATUS_INITED = 0;
	public static final int PLAY_STATUS_PREPAREING = 1;
	public static final int PLAY_STATUS_PREPARED = 2;
	public static final int PLAY_STATUS_STARTED = 3;
	public static final int PLAY_STATUS_PAUSED = 4;
	public static final int PLAY_STATUS_FF = 5;
	public static final int PLAY_STATUS_FR = 6;
	public static final int PLAY_STATUS_SF = 7;
	public static final int PLAY_STATUS_SEEKING = 8;
	public static final int PLAY_STATUS_STEP = 9;
	public static final int PLAY_STATUS_STOPPED = 10;
	//public static final int PLAY_STATUS_END = PLAY_STATUS_UNKNOW;

	/* Message Type */
	public static final int MSG_RESET_VIDEO_ZOOM_MODE = 8;
	public static final int MSG_SEEK_DONE = 7;
	public static final int MSG_STEP_DONE = 6;
	public static final int MSG_END_OF_PLAYLIST = 5;
	public static final int MSG_AUTO_NEXT = 4;
	public static final int MSG_SEEKING = 3;
	public static final int MSG_PLAY_START = 2;
	public static final int MSG_SOURCE_PREPAREING = 1;
	public static final int MSG_PLAYER_NOT_PREPARED = -1;
	public static final int MSG_SOURCE_NOT_PREPARED = -2;
	public static final int MSG_SET_SOURCE_FAIL = -3;
	public static final int MSG_IS_PLAYING = -4;
	public static final int MSG_SET_DATA_SOURCE_FAIL = -5;
	public static final int MSG_SEEK_NOT_SUPPORT = -6;
	public static final int MSG_PLAY_START_FAIL = -7;
	public static final int MSG_STEP_NOT_SUPPORT = -8;
	public static final int MSG_NOT_SUPPORT = -9;
	public static final int MSG_AUDIO_NOT_SUPPORT = -10;
	public static final int MSG_VIDEO_NOT_SUPPORT = -11;
	public static final int MSG_FILE_NOT_SUPPORT = -12;
	public static final int MSG_INVALID_STATE = -13;
	public static final int MSG_INPUT_STREAM_FAIL = -14;
	public static final int MSG_AV_NOT_SUPPORT = -15;
	public static final int MSG_FILE_CORRUPT = -16;
	public static final int MSG_POSITION_UPDATE = -17;
	public static final int MSG_FILE_AUDIOONLY = -18;
	/* Player Mode */
	public static final int PLAYER_MODE_MMP = FileConst.SRC_USB;
	public static final int PLAYER_MODE_SAMBA = FileConst.SRC_SMB;
	public static final int PLAYER_MODE_DLNA = FileConst.SRC_DLNA;
	public static final int PLAYER_MODE_HTTP = FileConst.SRC_HTTP;

	/* Error msg */
	public static final String MSG_ERR_CANNOTPAUSE = "CANNOTPAUSE";
	public static final String MSG_ERR_PAUSEEXCEPTION = "PAUSEEXCEPTION";
	/* Video Media Type */
	public static final int VIDEO_TYPE_UNKNOWN = 0;
	public static final int VIDEO_TYPE_AVI = 1;
	public static final int VIDEO_TYPE_MPEG2_PS = 2;
	public static final int VIDEO_TYPE_MPEG2_TS = 3;
	public static final int VIDEO_TYPE_ASF = 4;
	public static final int VIDEO_TYPE_MKV = 5;
	public static final int VIDEO_TYPE_OGG = 6;
	public static final int VIDEO_TYPE_FLAC = 7;
	public static final int VIDEO_TYPE_APE = 8;
	public static final int VIDEO_TYPE_VIDEO_ES = 9;
	public static final int VIDEO_TYPE_AUDIO_ES = 10;
	public static final int VIDEO_TYPE_MP4 = 11;
	public static final int VIDEO_TYPE_WAV = 12;
	public static final int VIDEO_TYPE_RM = 13;
	public static final int VIDEO_TYPE_MTK_P0 = 14;

	/* Video Codec */
	public static final int VIDEO_CODEC_UNKNOWN = 0;
	public static final int VIDEO_CODEC_MPEG1_2 = 1;
	public static final int VIDEO_CODEC_MPEG4 = 2;
	public static final int VIDEO_CODEC_H264 = 3;
	public static final int VIDEO_CODEC_H263 = 4;
	public static final int VIDEO_CODEC_VC1 = 5;
	public static final int VIDEO_CODEC_WMV1 = 6;
	public static final int VIDEO_CODEC_WMV2 = 7;
	public static final int VIDEO_CODEC_WMV3 = 8;
	public static final int VIDEO_CODEC_DIVX311 = 9;
	public static final int VIDEO_CODEC_RV8 = 10;
	public static final int VIDEO_CODEC_RV9_10 = 11;
	public static final int VIDEO_CODEC_MJPEG = 12;
	public static final int VIDEO_CODEC_SORENSON_SPARK = 13;

	/* Audio Codec */
	public static final int AUDIO_CODEC_UNKNOWN = 0;
	public static final int AUDIO_CODEC_MPEG = 1;
	public static final int AUDIO_CODEC_MP3 = 2;
	public static final int AUDIO_CODEC_AAC = 3;
	public static final int AUDIO_CODEC_DD = 4;
	public static final int AUDIO_CODEC_TRUEHD = 5;
	public static final int AUDIO_CODEC_PCM = 6;
	public static final int AUDIO_CODEC_DTS = 7;
	public static final int AUDIO_CODEC_DTS_HD_HR = 8;
	public static final int AUDIO_CODEC_DTS_HD_MA = 9;
	public static final int AUDIO_CODEC_WMA = 10;
	public static final int AUDIO_CODEC_COOK = 11;
	public static final int AUDIO_CODEC_VORBIS = 12;
	public static final int AUDIO_CODEC_FLAC = 13;
	public static final int AUDIO_CODEC_MONKEY = 14;

	/* Audio channel number */
	public static final int AUDIO_CH_MONO = 1;
	public static final int AUDIO_CH_STEREO = 2;
	public static final int AUDIO_CH_SURROUND_TWO_CH = 3;
	public static final int AUDIO_CH_SURROUND = 4;
	public static final int AUDIO_CH_THREE_ZERO = 5;
	public static final int AUDIO_CH_FOUR_ZERO = 6;
	public static final int AUDIO_CH_FIVE_ZERO = 7;
	public static final int AUDIO_CH_FIVE_ONE = 8;
	public static final int AUDIO_CH_SEVEN_ONE = 9;

	/* audio sample Rate */
	public static final int AUDIO_SAMPLE_RATE_8K = 1;
	public static final int AUDIO_SAMPLE_RATE_16K = 2;
	public static final int AUDIO_SAMPLE_RATE_32K = 3;
	public static final int AUDIO_SAMPLE_RATE_11K = 4;
	public static final int AUDIO_SAMPLE_RATE_22K = 5;
	public static final int AUDIO_SAMPLE_RATE_44K = 6;
	public static final int AUDIO_SAMPLE_RATE_12K = 7;
	public static final int AUDIO_SAMPLE_RATE_24K = 8;
	public static final int AUDIO_SAMPLE_RATE_48K = 9;
	public static final int AUDIO_SAMPLE_RATE_96K = 10;
	public static final int AUDIO_SAMPLE_RATE_192K = 11;

	public static final int VOUT_REGION_MAX_WIDTH = 10000;
	public static final int VOUT_REGION_MAX_HEIGTH = 10000;

	public static final int PREVIEW_RECT_X = 480;
	public static final int PREVIEW_RECT_Y = 1500;
	public static final int PREVIEW_RECT_W = 3000;
	public static final int PREVIEW_RECT_H = 3000;

	public static final int TS_READ_BUFFER = 1024 * 1024;
	public static final int VOUT_ZOOM_TYPE_1X = 0;
	public static final int VOUT_ZOOM_TYPE_2X = 1;
	public static final int VOUT_ZOOM_TYPE_4X = 2;
	public static final int VOUT_ZOOM_TYPE_8X = 3;
	public static final int VOUT_ZOOM_TYPE_16X = 4;
	public static final int VOUT_ZOOM_TYPE_32X = 5;
	public static final int VOUT_ZOOM_TYPE_64X = 6;
	public static final int VOUT_ZOOM_TYPE_128X = 7;
	public static final int VOUT_ZOOM_TYPE_256X = 8;
	public static final int VOUT_ZOOM_TYPE_512X = 9;

	public static final int THUMBNAIL_WIDTH = 176;
	public static final int THUMBNAIL_HEIGTH = 144;

	/* ts stream type */
	public static final short TS_STREAM_TYPE_AUDIO_11172_3 = 3;
	public static final short TS_STREAM_TYPE_AUDIO_13818_3 = 4;
	public static final short TS_STREAM_TYPE_AUDIO_13818_7 = 15;
	public static final short TS_STREAM_TYPE_AUDIO_14496_3 = 17;
	public static final short TS_STREAM_TYPE_AUDIO_AVS = 67;
	public static final short TS_STREAM_TYPE_AUDIO_ATSC_A53 = 129;
	public static final short TS_STREAM_TYPE_AUDIO_MSRT24 = 130;
	public static final short TS_STREAM_TYPE_AUDIO_ATT_LABS_G729A = 131;
	public static final short TS_STREAM_TYPE_AUDIO_DTS_EXPT_XLL = 133;
	public static final short TS_STREAM_TYPE_AUDIO_DTS_GSM10 = 134;

	public static final String NOT_SUPPORT = "Not Support!";

	
	
	public static final int THUMANIL_STATUS_PREPARED = 2;
	public static final int THUMANIL_STATUS_STOPED = 3;
	public static final int THUMANIL_STATUS_INIT = 1;
	/* message type */
	public static final int MSG_PRAPARE = 1;

	public static final String subSuffix[] = { ".srt", ".ass", ".ssa", ".sub",
			".smi", ".txt", ".psb", ".mpl" };

	public static final short tsAudioStrmType[] = {
			TS_STREAM_TYPE_AUDIO_11172_3, TS_STREAM_TYPE_AUDIO_13818_3,
			TS_STREAM_TYPE_AUDIO_13818_7, TS_STREAM_TYPE_AUDIO_14496_3,
			TS_STREAM_TYPE_AUDIO_AVS, TS_STREAM_TYPE_AUDIO_ATSC_A53,
			TS_STREAM_TYPE_AUDIO_MSRT24, TS_STREAM_TYPE_AUDIO_ATT_LABS_G729A,
			TS_STREAM_TYPE_AUDIO_DTS_EXPT_XLL, TS_STREAM_TYPE_AUDIO_DTS_GSM10 };
}
