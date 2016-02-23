package com.mediatek.ui.menu.util;

import java.util.ArrayList;
import java.util.Date;

import android.R.integer;
import android.content.Context;
import android.content.ContentResolver;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemClock;
import android.util.Log;
import android.view.Gravity;
import android.widget.TextView;
import android.widget.Toast;
import android.provider.Settings;
import android.os.IPowerManager;
import android.os.RemoteException;
import android.util.Log;
import android.net.Uri;
import android.net.wifi.WifiManager;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tv.TVManager;
import com.mediatek.tv.common.ChannelCommon;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.common.ConfigValue;
import com.mediatek.tv.common.TVMException;
import com.mediatek.tv.service.ConfigService;
import com.mediatek.tv.service.TVNative;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVEventManager;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.commonview.ListenTime;
import com.mediatek.ui.menu.commonview.MTKPowerManager;
import com.mediatek.ui.menu.commonview.SleepTimerOff;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.DateFormatUtil;
import com.mediatek.ui.util.MtkLog;

/**
 * Define Constant and get value of Configure Item
 * 
 * @author MTK40405 P:\Pub\F.Li\5395 dtmb
 */
public class MenuConfigManager {
	private static MenuConfigManager mConfigManager;

	TVContent tv;
	TVConfigurer cfg;
//	MTKTVConfigurer mtkCfg;
	TVScanner sc;
	TVOptionRange<Integer> opt;
	TVOptionRange<Integer> tv_opt;
	OSDLanguage osdLanguage; // set language
	SaveValue save; // save some UI value
	TVCommonInterface comm;

	// update by jungu 2011.8.1
	// private NetworkImpl netImpl;
	ListenTime listenTime;
	SleepTimerOff sleepTimerOff;

	int minValue = 0;
	int maxValue = 0;
	int defaultValue = 0;
	int tvOptionValue = 0;
	Context mContext;
	TVEventManager tvEventManager;

	private MenuConfigManager(Context context) {
		mContext = context;
		tv = TVContent.getInstance(context);
		cfg = tv.getConfigurer();
//		mtkCfg = (MTKTVConfigurer) tv.getConfigurer();
		sc = tv.getScanner();
		comm = TVCommonInterface.getInstance(context);
		save = SaveValue.getInstance(context);
		tvEventManager=TVContent.getInstance(context).getEventManager();
	}

	public static MenuConfigManager getInstance(Context context) {
		if (mConfigManager == null) {
			mConfigManager = new MenuConfigManager(context);
		}
		return mConfigManager;
	}

	/** Video */
	public static final String PICTURE_MODE = ConfigType.CFG_PICTURE_MODE;
	public static final String BACKLIGHT = ConfigType.CFG_BACKLIGHT;
	public static final String BRIGHTNESS = ConfigType.CFG_BRIGHTNESS;
	public static final String CONTRAST = ConfigType.CFG_CONTRAST;
	public static final String SATURATION = ConfigType.CFG_SATURATION;
	public static final String HUE = ConfigType.CFG_HUE;
	public static final String SHARPNESS = ConfigType.CFG_SHARPNESS;
	public static final String GAMMA = ConfigType.CFG_GAMMA;
	public static final String COLOR_TEMPERATURE = ConfigType.CFG_TEMPERATURE;

	public static final String COLOR_G_R = ConfigType.CFG_COLOR_GAIN_R;
	public static final String COLOR_G_G = ConfigType.CFG_COLOR_GAIN_G;
	public static final String COLOR_G_B = ConfigType.CFG_COLOR_GAIN_B;
	// public static final String DNR = ConfigType.CFG_NR;
	public static final String DNR = TVConfigurer.ALL_NR_OPTION;
	public static final String MPEG_NR = ConfigType.CFG_MPEG_NR;
	public static final String ADAPTIVE_LUMA_CONTROL = ConfigType.CFG_LUMA;
	public static final String FLESH_TONE = ConfigType.CFG_FLESH_TONE;
	public static final String DI_FILM_MODE = ConfigType.CFG_DI_FILM_MODE;
	public static final String BLUE_STRETCH = ConfigType.CFG_BLUE_STRETCH;
	public static final String GAME_MODE = ConfigType.CFG_GAME_MODE;
	public static final String PQ_SPLIT_SCREEN_DEMO_MODE = ConfigType.CFG_VIDEO_PQ_DEMO;
	// Config for vga input source
	public static final String VGA_MODE = ConfigType.CFG_VGAMODE;

	// HDMI mode
	public static final String HDMI_MODE = ConfigType.CFG_HDMI_MODE;

	// Configs for VGA mode
	public static final String HPOSITION = ConfigType.CFG_VGA_POS_H;
	public static final String VPOSITION = ConfigType.CFG_VGA_POS_V;
	public static final String PHASE = ConfigType.CFG_VGA_PHASE;
	public static final String CLOCK = ConfigType.CFG_VGA_CLOCK;
	// VGA
	public static final String VGA = "SUB_VGA";
	// auto adjust
	public static final String AUTO_ADJUST = "SUB_AUTO_ADJUST";

	public static final String VIDEO_3D = "video_3d";
	public static final String VIDEO_3D_MODE = ConfigType.CFG_3D_MODE;
	public static final String VIDEO_3D_NAV = ConfigType.CFG_3D_NAV_AUTO;
	public static final String VIDEO_3D_3T2 = ConfigType.CFG_3D_TO_2D;
	public static final String VIDEO_3D_FIELD = ConfigType.CFG_3D_DEPTH_OF_FIELD;
	public static final String VIDEO_3D_PROTRUDE = ConfigType.CFG_3D_VIEW_POINT;
	public static final String VIDEO_3D_DISTANCE = ConfigType.CFG_3D_DISTANCE;
	public static final String VIDEO_3D_IMG_SFTY = ConfigType.CFG_IMG_SFTY;
	public static final String VIDEO_3D_LF = ConfigType.CFG_3D_LR_SWITCH;
	// public static final String VIDEO_3D_OSD_DEPTH = ConfigType.CFG_3D_
    /**
     * CECFUN 
     */
	public static final String CEC_CEC_FUN = ConfigType.CEC_CEC_FUNC;
    public static final int CEC_FUNTION_ON = ConfigType.CEC_FUNC_ON;
    public static final int CEC_FUNTION_OFF = ConfigType.CEC_FUNC_OFF;
    public static final String CEC_SAC_OFUN = ConfigType.CEC_SAC_FUNC;
    public static final String CEC_AUTO_ON = ConfigType.CEC_AUTO_ON;
    public static final String CEC_AUTO_OFF = ConfigType.CEC_AUTO_OFF;
    public static final String CEC_DEVICE_DISCOVERY = "cec_device";

	// MJC
	public static final String MJC = "UNDEFINE_MJC";
	// EFFECT
	public static final String EFFECT = ConfigType.CFG_MJC_EFFECT;
	public static final String MENU_MJC_MODE = TVConfigurer.MJC_MODE_OPTION;
	public static final String DEMO = "UNDEFINE_DEMO";
	// DEMO PARTITION
	public static final String DEMO_PARTITION = ConfigType.CFG_MJC_DEMO;

	/** factory */
	/*
	 * factory_video
	 */
	// AUTO_COLOR
	public static final String FV_AUTOCOLOR = "SUB_FV_AUTOCOLOR";
	// COLOR_TEMPERATURE
	public static final String FV_COLORTEMPERATURE = "SUB_FV_COLORTEMPERATURE";
	// COLOR_TEMPERATURE_CHILD
	public static final String FV_COLORTEMPERATURECHILD = ConfigType.CFG_TEMPERATURE;
	// H.Position
	public static final String FV_HPOSITION = ConfigType.CFG_H_POSITION;
	// V.Position
	public static final String FV_VPOSITION = ConfigType.CFG_V_POSITION;
	// AUTO PHASE
	public static final String FV_AUTOPHASE = "SUB_FV_AUTOPHASE";
	// PHASE
	public static final String FV_PHASE = ConfigType.CFG_YPBPR_PHASE;
	// DIMA
	public static final String FV_DIMA = ConfigType.CFG_DI_MA;
	// DIEDGE
	public static final String FV_DIEDGE = ConfigType.CFG_DI_EDGE;
	// WCG
	public static final String FV_WCG = ConfigType.CFG_WCG;
	// FLIP
	public static final String FV_FLIP = ConfigType.CFG_VIDEO_FLIP;
	// MIRROR
	public static final String FV_MIRROR = ConfigType.CFG_VIDEO_MIRROR;
	// factory_video_COLOR TEMPERATURE
	// r gain
	public static final String FV_COLOR_G_R = ConfigType.CFG_COLOR_GAIN_R;
	// g gain
	public static final String FV_COLOR_G_G = ConfigType.CFG_COLOR_GAIN_G;
	// b gain
	public static final String FV_COLOR_G_B = ConfigType.CFG_COLOR_GAIN_B;
	// r offset
	public static final String FV_COLOR_O_R = ConfigType.CFG_COLOR_OFFSET_R;
	// g offset
	public static final String FV_COLOR_O_G = ConfigType.CFG_COLOR_OFFSET_G;
	// b offset
	public static final String FV_COLOR_O_B = ConfigType.CFG_COLOR_OFFSET_B;

	/*
	 * factory_audio
	 */
	// DOLBY BANNER
	public static final String FA_DOLBYBANNER = ConfigType.CFG_DOLBY_BANNER;
	// COMPRESSION
	public static final String FA_COMPRESSION = ConfigType.CFG_DOLBY_CMPR_MODE;
	// COMPRESSION FACTOR
	public static final String FA_COMPRESSIONFACTOR = ConfigType.CFG_DOLBY_CMPR_FACTOR;
	// A2 SYSTEM
	public static final String FA_A2SYSTEM = "SUB_FA_A2SYSTEM";
	// PAL SYSTEM
	public static final String FA_PALSYSTEM = "SUB_FA_PALSYSTEM";
	// EU SYSTEM
	public static final String FA_EUSYSTEM = "SUB_FA_EUSYSTEM";
	// LATENCY
	public static final String FA_LATENCY = ConfigType.CFG_AUDIO_DELAY;

	/*
	 * factory_audio_A2_system
	 */
	// NUMBERS OF CHECK
	public static final String FAA2_NUMBERSOFCHECK = "UNDEFINE_FAA2_NUMBERSOFCHECK";
	// NUMBERS OF DOUBLE
	public static final String FAA2_NUMBERSOFDOUBLE = "UNDEFINE_FAA2_NUMBERSOFDOUBLE";
	// MONO WEIGHT
	public static final String FAA2_MONOWEIGHT = "UNDEFINE_FAA2_MONOWEIGHT";
	// STEREO WEIGHT
	public static final String FAA2_STEREOWEIGHT = "UNDEFINE_FAA2_STEREOWEIGHT";
	// DUAL WEIGHT
	public static final String FAA2_DUALWEIGHT = "UNDEFINE_FAA2_DUALWEIGHT";
	// HIGH DEVIATION MODE
	public static final String FAA2_HIGHDEVIATIONMODE = "UNDEFINE_FAA2_HIGHDEVIATIONMODE";
	// CARRIER SHIFT FUNCTION
	public static final String FAA2_CARRIERSHIFTFUNCTION = "UNDEFINE_FAA2_CARRIERSHIFTFUNCTION";
	// FM CARRIER MUTE MODE
	public static final String FAA2_FMCARRIERMUTEMODE = "UNDEFINE_FAA2_FMCARRIERMUTEMODE";
	// FM CARRIER MUTE THRESHOLD HIGH
	public static final String FAA2_FMCARRIERMUTETHRESHOLDHIGH = "UNDEFINE_FAA2_FMCARRIERMUTETHRESHOLDHIGH";
	// FM CARRIER MUTE THRESHOLD LOW
	public static final String FAA2_FMCARRIERMUTETHRESHOLDLOW = "UNDEFINE_FAA2_FMCARRIERMUTETHRESHOLDLOW";
	// FINE TUNE VOLUME
	public static final String FAA2_FINETUNEVOLUME = "UNDEFINE_FAA2_FINETUNEVOLUME";

	/*
	 * factory_audio_pal_system
	 */
	// CORRECT THRESHOLD
	public static final String FAPAL_CORRECTTHRESHOLD = "UNDEFINE_FAPAL_CORRECTTHRESHOLD";
	// TOTAL SYNC LOOP
	public static final String FAPAL_TOTALSYNCLOOP = "UNDEFINE_FAPAL_TOTALSYNCLOOP";
	// ERROR THRESHOLD
	public static final String FAPAL_ERRORTHRESHOLD = "UNDEFINE_FAPAL_ERRORTHRESHOLD";
	// PARITY ERROR THRESHOLD
	public static final String FAPAL_PARITYERRORTHRESHOLD = "UNDEFINE_FAPAL_PARITYERRORTHRESHOLD";
	// EVERY NUMBER FRAMES
	public static final String FAPAL_EVERYNUMBERFRAMES = "UNDEFINE_FAPAL_EVERYNUMBERFRAMES";
	// HIGH DEVIATION MODE
	public static final String FAPAL_HIGHDEVIATIONMODE = "UNDEFINE_FAPAL_HIGHDEVIATIONMODE";
	// AM CARRIER MUTE MODE
	public static final String FAPAL_AMCARRIERMUTEMODE = "UNDEFINE_FAPAL_AMCARRIERMUTEMODE";
	// AM CARRIER MUTE THRESHOLD HIGH
	public static final String FAPAL_AMCARRIERMUTETHRESHOLDHIGH = "UNDEFINE_FAPAL_AMCARRIERMUTETHRESHOLDHIGH ";
	// AM CARRIER MUTE THRESHOLD LOW
	public static final String FAPAL_AMCARRIERMUTETHRESHOLDLOW = "UNDEFINE_FAPAL_AMCARRIERMUTETHRESHOLDLOW";
	// CARRIER SHIFT FUNCTION
	public static final String FAPAL_CARRIERSHIFTFUNCTION = "UNDEFINE_FAPAL_CARRIERSHIFTFUNCTION";
	// FM CARRIER MUTE MODE
	public static final String FAPAL_FMCARRIERMUTEMODE = "UNDEFINE_FAPAL_FMCARRIERMUTEMODE";
	// FM CARRIER MUTE THRESHOLD HIGH
	public static final String FAPAL_FMCARRIERMUTETHRESHOLDHIGH = "UNDEFINE_FAPAL_FMCARRIERMUTETHRESHOLDHIGH";
	// FM CARRIER MUTE THRESHOLD LOW
	public static final String FAPAL_FMCARRIERMUTETHRESHOLDLOW = "UNDEFINE_FAPAL_FMCARRIERMUTETHRESHOLDLOW";
	// PAL FINE TUNE VOLUME
	public static final String FAPAL_PALFINETUNEVOLUME = "UNDEFINE_FAPAL_PALFINETUNEVOLUME";
	// AM FINE TUNE VOLUME
	public static final String FAPAL_AMFINETUNEVOLUME = "UNDEFINE_FAPAL_AMFINETUNEVOLUME";
	// NICAM FINE TUNE VOLUME
	public static final String FAPAL_NICAMFINETUNEVOLUME = "UNDEFINE_FAPAL_NICAMFINETUNEVOLUME";

	/*
	 * factory_audio_EU_system
	 */
	// EU FM Saturation Mute
	public static final String FAEU_FM = "UNDEFINE_FAEU_FM";
	// EU FM EU NON EU SYSTEM
	public static final String FAEU_EU_NON = "UNDEFINE_FAEU_EU_NON";

	/** Audio */
	public static final String BALANCE = ConfigType.CFG_BALANCE;
	public static final String BASS = ConfigType.CFG_AUD_BASS;
	public static final String SRS_MODE = ConfigType.CFG_SRS_MODE;
	public static final String EQUALIZE = ConfigType.CFG_EQUALIZE;
	public static final String SPEAKER_MODE = ConfigType.CFG_SPEAKER;
	public static final String SPDIF_MODE = ConfigType.CFG_SPDIF_MODE;
	public static final String SPDIF_DELAY = ConfigType.CFG_SPDIF_DELAY;
	public static final String AVCMODE = ConfigType.CFG_AVCMODE;
	public static final String TYPE = ConfigType.CFG_AUDIO_TYPE;
	public static final String VISUALLY_VOLUME = ConfigType.CFG_AD_VOLUME;

	// Treble high-pitched voice
	public static final String TREBLE = ConfigType.CFG_AUD_TREBLE;
	// Speaker volume
	public static final String VISUALLY_SPEAKER = ConfigType.CFG_AUD_AD_SPEAKER;
	// Headphone volume
	public static final String VISUALLY_HEADPHONE = ConfigType.CFG_AUD_AD_HDPHONE;

	// Visually Impaired
	public static final String VISUALLY_IMPAIRED = "SUB_VISUALLYIMPAIRED";

	/** TV */
	public static final String TUNER_MODE = ConfigType.CFG_BS_SRC;
	public static final String TV_MTS_MODE = TVConfigurer.MENU_MTS_OPTION;
	public static final String TV_AUDIO_LANGUAGE = ConfigType.MENU_AUD_LANG;
	public static final String TV_AUDIO_LANGUAGE_2 = ConfigType.MENU_AUD_LANG_2ND;
	public static final String TV_SYSTEM = TVScanner.SCAN_OPTION_TV_SYSTEM;
	public static final String COLOR_SYSTEM = TVScanner.SCAN_OPTION_COLOR_SYSTEM;
	public static final String SCAN_MODE = TVScanner.SCAN_OPTION_SCAN_EMOD;
	public static final String SYM_RATE = TVScanner.SCAN_OPTION_SYM_RATE;
	public static final String TV_FREEZE_CHANNEL = ConfigType.CFG_CH_FRZ_CHG;
	public static final String DTV_TSHIFT_OPTION = "DTV_TSHIFT_OPTION";
	public static final String DTV_DEVICE_INFO = "DTV_DEVICE_INFO";

	public static final String SCHEDULE_PVR_SRCTYPE = "SCHEDULE_PVR_SRCTYPE";
	public static final String SCHEDULE_PVR_CHANNELLIST = "SCHEDULE_PVR_CHANNELLIST";
	public static final String SCHEDULE_PVR_REMINDER_TYPE = "SCHEDULE_PVR_REMINDER_TYPE";
	public static final String SCHEDULE_PVR_REPEAT_TYPE = "SCHEDULE_PVR_REPEAT_TYPE";
	/** setup */
	public static final String OSD_LANGUAGE = "SETUP_osd_language";
	public static final String SCREEN_MODE = ConfigType.CFG_SCREEN_MODE;
	public static final String DPMS = ConfigType.CFG_DPMS;
	public static final String WAKEUP_VGA = ConfigType.CFG_WAKEUP_VGA_SETUP;
	public static final String BLUE_MUTE = ConfigType.CFG_BLUE_SCREEN;
	public static final String POWER_ON_MUSIC = ConfigType.POWER_ON_MUSIC;
	public static final String POWER_OFF_MUSIC = ConfigType.POWER_OFF_MUSIC;
	public static final String CAPTURE_LOGO_SELECT = "SETUP_capture_logo";
	public static final String FAST_BOOT = "SETUP_fast_boot";
	public static final String WAKEUPREASON = ConfigType.CFG_WAKE_UP_REASON;

	public static final String SETUP_TIME_ZONE = "SETUP_time_zone";
	public static final String SETUP_TIME_SET = "SETUP_time_set";
	public static final String AUTO_SYNC = "SETUP_auto_syn";
	public static final String POWER_ON_TIMER = "SETUP_power_on_timer";
	public static final String POWER_OFF_TIMER = "SETUP_power_off_timer";
	public static final String POWER_ON_CH_MODE = "SETUP_power_on_ch_mode";
	public static final String SLEEP_TIMER = "SETUP_sleep_timer";
	public static final String AUTO_SLEEP = "SETUP_auto_sleep";

	public static final String TIME_DATE = "SETUP_date";
	public static final String TIME_TIME = "SETUP_time";
	public static final String TIMER1 = "SETUP_timer1";
	public static final String TIMER2 = "SETUP_timer2";

	public static final String ADDRESS_TYPE = "UNDEFINE_address_type";
	public static final String DLNA = "SETUP_dlna";
	public static final String MY_NET_PLACE = "SETUP_net_place";

	public static final String COMMON_INTERFACE = "SETUP_common_interface";
	public static final String SETUP_TIME_SETUP = "SETUP_time_setup";
	public static final String SETUP_TELETEXT="SETUP_teletext";
	public static final String SETUP_NETWORK = "SETUP_network";
	public static final String SETUP_POWER_ONCHANNEL_LIST = "SETUP_power_onchannel";

	
	public static final String SETUP_DIGITAL_TELETEXT_LANGUAGE=ConfigType.CFG_TTX_DIGITAL_LANG;
	public static final String SETUP_DECODING_PAGE_LANGUAGE=ConfigType.CFG_TTX_DECODE_PAGE;
	public static final String SETUP_TTX_PRESENTATION_LEVEL=ConfigType.CFG_TTX_PRESENTATION_LVL;
	
	public static final String DTV_SCHEDULE_LIST = "DTV_SCHEDULE_LIST";
	public static final String VERSION_INFO = "SETUP_version_info";
	public static final String SETUP_MSI = ConfigType.CFG_VIDEO_MSI;
	public static final String RESET_DEFAULT = ConfigType.RESET_USER;


	/** having sub page items are defined here */
	public static final String VIDEO_COLOR_TEMPERATURE = "colorTemprature";
	public static final String VIDEO_ADVANCED_VIDEO = "advancedVideo";
	public static final String TV_CHANNEL = "tv_channel";
	public static final String TV_CHANNEL_SCAN = "channel_scan";
	public static final String TV_UPDATE_SCAN = "update_scan";
	public static final String TV_ANALOG_SCAN = "analog_scan";
	public static final String TV_SINGLE_RF_SCAN = "single_rf_scan";
	public static final String TV_CHANNEL_EDIT = "channel_edit";
	public static final String TV_CHANNEL_CLEAR = "channel_clean";
	public static final String SETUP_APPLICATION = "application";

	public static final String TV_CHANNEL_START_FREQUENCY = "UNDEFINE_channel_start_frequency";
	public static final String TV_CHANNEL_FREQUENCY = "UNDEFINE_channel_frequency";
	public static final String TV_CHANNEL_END_FREQUENCY = "UNDEFINE_channel_end_frequency";
	public static final String TV_CHANNEL_STARTSCAN = "start_scan";
	public static final String TV_CHANNEL_EDIT_LIST = "UNDEFINE_channel_edit_list";
	public static final String TV_CHANNEL_NO = "UNDEFINE_channel_edit_no";
	public static final String TV_CHANNEL_NAME = "UNDEFINE_channel_edit_name";
	public static final String TV_FREQ = "UNDEFINE_channel_edit_frequency";
	public static final String TV_CHANNEL_COLOR_SYSTEM = "CHANNELEDIT_color_system";
	public static final String TV_SOUND_SYSTEM = "CHANNELEDIT_sound_system";
	public static final String TV_AUTO_FINETUNE = "UNDEFINE_channel_edit_aft";
	public static final String TV_FINETUNE = "channel_edit_finetune";
	public static final String TV_SKIP = "UNDEFINE_channel_edit_skip";
	public static final String TV_STORE = "channel_edit_store";
	/**DTMB Single RF Channel*/
	public static final String TV_SINGLE_SCAN_RF_CHANNEL ="UNDEFINE_tv_single_rf_channel";
	public static final String TV_SINGLE_SCAN_SIGNAL_LEVEL ="UNDEFINE_tv_single_scan_signal_level";
	public static final String TV_SINGLE_SCAN_SIGNAL_QUALITY ="UNDEFINE_tv_singl_scan_signal_quality";
	/** Parental Part */
	public static final String PARENTAL_PASSWORD = "parental_password";
	public static final String PARENTAL_CHANNEL_BLOCK = "parental_channel_block";
	public static final String PARENTAL_INPUT_BLOCK = "parental_input_block";
	public static final String PARENTAL_INPUT_BLOCK_SOURCE = "UNDEFINE_parental_input_block_source";
	public static final String PARENTAL_CHANGE_PASSWORD = "parental_change_password";
	public static final String PARENTAL_CLEAN_ALL = "parental_clean_all";
	public static final String PARENTAL_PASSWORD_NEW = "parental_password_new";
	public static final String PARENTAL_PASSWORD_NEW_RE = "parental_password_new_re";
	public static final String PARENTAL_CHANNEL_BLOCK_CHANNELLIST = "parental_block_channellist";

	/** Factory part */
	public static final String FACTORY_VIDEO = "SUB_factory_video";
	public static final String FACTORY_AUDIO = "SUB_factory_audio";
	public static final String FACTORY_TV = "SUB_factory_TV";
	public static final String FACTORY_SETUP = "SUB_factory_setup";
	public static final String FACTORY_PRESET_CH = "SUB_preset_ch";

	public static final String FACTORY_TV_TUNER_DIAGNOSTIC = "tuner_diagnostic";
	public static final String FACTORY_TV_TUNER_DIAGNOSTIC_NOINFO = "tuner_diagnostic_noinfo";
	public static final String FACTORY_TV_TUNER_DIAGNOSTIC_VERSION = "tuner_diagnostic_version";
	public static final String FACTORY_TV_TUNER_DIAGNOSTIC_RF = "tuner_diagnostic_rf";
	public static final String FACTORY_TV_TUNER_DIAGNOSTIC_LOCK = "tuner_diagnostic_lock";
	public static final String FACTORY_SETUP_EVENT_FORM = "UNDEFINE_mts_factory_setup_event_form";
	public static final String FACTORY_SETUP_BURNING_MODE = "UNDEFINE_mts_factory_setup_burning_mode";
	public static final String FACTORY_SETUP_CLEAN_STORAGE = "factory_setup_clean_storage";
	public static final String FACTORY_PRESET_CH_DUMP = "preset_ch_dump";
	public static final String FACTORY_PRESET_CH_PRINT = "preset_ch_print";
	public static final String FACTORY_PRESET_CH_RESTORE = "preset_ch_restore";
	public static final String FACTORY_SETUP_CI_UPDATE = "factory_updateCi";
	public static final String FACTORY_SETUP_CI_ERASE = "factory_eraseCi";
	public static final String FACTORY_SETUP_CI_QUERY = "factory_queryCi";

	public static final int INVALID_VALUE = 10004;
	public static final int STEP_VALUE = 1;
	public static final int STEP_BIG_VALUE = 10;
	private static final String TAG = "MenuConfigManager";

  public static final String SETUP_WOW = "WOW";
	/** wifi part */
	public static final int WIFI_COMMON_BIND = 0;
	public static final int WIFI_COMMON_NODONGLE = 1;
	public static final int WIFI_COMMON_SCAN_FAIL = 2;
	public static final int WIFI_COMMON_SCAN_INVALID = 3;
	public static final int WIFI_COMMON_SCAN_SUCCESS = 4;
	public static final int WIFI_COMMON_MANUAL_INVASSID = 6;
	public static final int WIFI_COMMON_MANUAL_INVAPASS = 7;
	public static final int WIFI_COMMON_MANUAL_SUCCESS = 8;
	public static final int WIFI_COMMON_MANUAL_FAIL = 9;
	// public static final int WIFI_COMMON_MANUAL_PASSERR = 10;
	public static final int WIFI_COMMON_PIN_FAIL = 11;
	public static final int WIFI_COMMON_PIN_SUCCESS = 12;
	public static final int WIFI_COMMON_PBC_FAIL = 13;
	public static final int WIFI_COMMON_PBC_SUCCESS = 14;
	public static final int WIFI_COMMON_PBC_HINT = 15;
	public static final int WIFI_COMMON_SCAN_TIMEOUT = 16;
	public static final int WIFI_COMMON_MANUAL_TIMEOUT = 17;
	public static final int WIFI_COMMON_NO_AP = 18;
	public static final int WIFI_COMMON_NO_WPS_AP = 19;

	public static final int WIFI_INPUT_SCAN_PASS = 0;
	public static final int WIFI_INPUT_MANUAL_SSID = 1;
	public static final int WIFI_INPUT_MANUAL_PASS = 2;

	public static final int W_CONFIRM_UNKNOWN = 0;
	public static final int W_CONFIRM_NONE = 1;
	public static final int W_CONFIRM_WEP = 2;
	public static final int W_CONFIRM_WPA_PSK_TKIP = 7;
	public static final int W_CONFIRM_WPA_PSK_AES = 4;
	public static final int W_CONFIRM_WPA2_PSK_TKIP = 5;
	public static final int W_CONFIRM_WPA2_PSK_AES = 6;
	public static final int W_CONFIRM_AUTO = 3;

	public static final int W_SECURITY_NONE = 0;
	public static final int W_SECURITY_WEP = 1;
	public static final int W_SECURITY_TKIP = 2;
	public static final int W_SECURITY_AES = 3;

	public static final int WIFI_CONNECT_WPS_SCANING = 5;
	public static final int WIFI_CONNECT_SCANING = 0;
	public static final int WIFI_CONNECT_SCAN = 1;
	public static final int WIFI_CONNECT_MANUAL = 2;
	public static final int WIFI_CONNECT_PIN_AUTO = 3;
	public static final int WIFI_CONNECT_PIN_AP = 4;
	public static final int WIFI_CONNECT_PBC = 6;

	public static final int WIFI_SCAN_NORMAL = 0;
	public static final int WIFI_SCAN_WPS = 1;

	public static final int FOCUS_OPTION_CHANGE_CHANNEL = 0;

	public TVScanner getScanner() {
		return sc;
	}

	public TVConfigurer getTvConfigurer() {
		return cfg;
	}

	public ArrayList<Boolean> get3DConfig() {
		ArrayList<Boolean> m3DList = new ArrayList<Boolean>();
		boolean m3DModeFlag = false;
		boolean m3DNavFlag = false;
		boolean m3D2DFlag = false;
		boolean m3DDepthFieldFlag = false;
		boolean m3DProtrudeFlag = false;
		boolean m3DDistanceFlag = false;
		boolean m3DImgSafetyFlag = false;
		boolean m3DLrSwitchFlag = false;
		// boolean m3DOsdDepthFlag = false;

		try {
			m3DModeFlag = cfg.checkType(TVConfigurer.CFG_3D_MODE);
			m3DNavFlag = cfg.checkType(TVConfigurer.CFG_3D_NAV);
			m3D2DFlag = cfg.checkType(TVConfigurer.CFG_3D_2D);
			m3DDepthFieldFlag = cfg.checkType(TVConfigurer.CFG_DEPTH_FIELD);
			m3DProtrudeFlag = cfg.checkType(TVConfigurer.CFG_PROTRUDE);
			m3DDistanceFlag = cfg.checkType(TVConfigurer.CFG_DISTANCE_TV);
			m3DImgSafetyFlag = cfg.checkType(TVConfigurer.CFG_IMG_SAFETY);
			m3DLrSwitchFlag = cfg.checkType(TVConfigurer.CFG_LR_SWITCH);
			// m3DOsdDepthFlag = cfg.checkType(TVConfigurer.CFG_OSD_DEPTH);
		} catch (TVMException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		m3DList.add(m3DModeFlag);
		m3DList.add(m3DNavFlag);
		m3DList.add(m3D2DFlag);
		m3DList.add(m3DDepthFieldFlag);
		m3DList.add(m3DProtrudeFlag);
		m3DList.add(m3DDistanceFlag);
		m3DList.add(m3DImgSafetyFlag);
		m3DList.add(m3DLrSwitchFlag);
		// m3DList.add(m3DOsdDepthFlag);
		return m3DList;
	}

	private TVOptionRange<Integer> getTVOption(String option) {
		opt = (TVOptionRange<Integer>) cfg.getOption(option);
		return opt;
	}

	private TVOptionRange<Integer> getTVScanOption(String option) {
		tv_opt = (TVOptionRange<Integer>) sc.getOption(option);
		return tv_opt;
	}

	public int getMin(String itemID) {
		int min = 0;
		opt = getTVOption(itemID);
		if(opt != null){
			min = opt.getMin();
		}
		return min;
	}

	public int getScanMin(String itemID) {
		int scanMin = 0;
		tv_opt = getTVScanOption(itemID);
		if (tv_opt != null) {
			scanMin = tv_opt.getMin();
		}
		return scanMin;
	}

	public int getMax(String itemID) {
		int max = 0;
		opt = getTVOption(itemID);
		if(opt != null){
			max = opt.getMax();
		}
		
		return max;

	}

	public int getScanMax(String itemID) {
		int scanMax = 0;
		tv_opt = getTVScanOption(itemID);
		if (tv_opt != null) {
			scanMax = tv_opt.getMax();
		}
		return scanMax;
	}

	public int getDefault(String itemID) {
		opt = getTVOption(itemID);
		int value = 0;
		if(opt != null){
			value = (int) opt.get();
		}
		MtkLog.d(TAG, "[" + itemID + "] get value: " + value + "   Min value: "
				+ getMin(itemID) + "   Max value: " + getMax(itemID));
		if (itemID.equals(MenuConfigManager.SHARPNESS)) {
			int min = getMin(itemID);
			int max = getMax(itemID);

			if (value >= getMin(itemID) && value <= getMax(itemID)) { // hzy fix
																		// CR:363304
				MtkLog.v(TAG, "Normal Case.--------------");
				return getMax(itemID)-value;
			} else if (value < min) {
				MtkLog.v(TAG, "Minimum Case.--------------");
				return min;
			} else {
				MtkLog.v(TAG, "Maximum Case.--------------");
				return max;
			}
		}
		if (value >= getMin(itemID) && value <= getMax(itemID)) {
			return value;
		} else {
			return getMin(itemID);
		}

	}

	public int getValueFromPrefer(String itemID) {
		return save.readValue(itemID);
	}

	public void setValueToPrefer(String itemID, int value) {
		save.saveValue(itemID, value);
	      if (null!=TimeShiftManager.getInstance()&value==0) {
	            TimeShiftManager.getInstance().stopAllRunning();
	        }
	}
	public int getScanDefault(String itemID) {
		tv_opt = getTVScanOption(itemID);

		int value = 0;
		if(tv_opt != null){
			value = tv_opt.get();
		}
		if (value >= getScanMin(itemID) && value <= getScanMax(itemID)) {
			return value;
		} else {
			MtkLog.d(TAG, "[" + itemID + "] get value: " + value
					+ "   Min value: " + getScanMin(itemID) + "   Max value"
					+ getScanMax(itemID));
			return getScanMin(itemID);
		}
	}

	public void setValue(String itemID, int value) {
		MtkLog.d(TAG, "set value: " + itemID + "---" + value);
		opt = getTVOption(itemID);
    if (itemID.equalsIgnoreCase(SETUP_WOW)) {
        WifiManager mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        
        mWifiManager.enableWoWL(value == 0?false:true);
        save.saveValue(SETUP_WOW, value);
        return;
    }
		if (itemID.equals(MenuConfigManager.SHARPNESS)) {
			int min = getMin(itemID);
			int max = getMax(itemID);

			if (value >= getMin(itemID) && value <= getMax(itemID)) { // hzy fix
																		// CR:363304
				MtkLog.v(TAG, "Normal Case.--------------");
				if(opt != null){
					opt.set(max - value);
				}
			} else if (value < min) {
				MtkLog.v(TAG, "Minimum Case.--------------");
				if(opt != null){
					opt.set(max);
				}
			} else {
				MtkLog.v(TAG, "Maximum Case.--------------");
				if(opt != null){
					opt.set(min);
				}
			}
			return;
		} else if (itemID.equals(TUNER_MODE)) {
//			if(opt != null){
//				opt.set(value);
//			}
			
			try {
				ITVCommon mCommon = TVCommonNative.getDefault(mContext);
				if(null != mCommon){
					mCommon.setTunnerMode(value);
				}	
			} catch (RemoteException e) {
				e.printStackTrace();
			}
			
//			if (value == ConfigType.BS_SRC_AIR) {
//				tv.getChannelManager().saveLastTunerModeChList(ChannelCommon.DB_CABEL);
//				tv.getChannelManager().update(ChannelCommon.DB_AIR);
//			} else {
//				tv.getChannelManager().saveLastTunerModeChList(ChannelCommon.DB_AIR);
//				tv.getChannelManager().update(ChannelCommon.DB_CABEL);
//			}

			String input = tv.getInputManager().getCurrInputSource();
			tv.getInputManager().changeInputSource(input);
            if (null!=TimeShiftManager.getInstance()) {
                TimeShiftManager.getInstance().stopAllRunning();
            }
		}

		if (value >= getMin(itemID) && value <= getMax(itemID)) {
			/*if(itemID.equals(MenuConfigManager.BLUE_MUTE)&&value==0){
				tv.setVideoBlueMute(false);
			}else if(itemID.equals(MenuConfigManager.BLUE_MUTE)&&value==1){
				tv.setVideoBlueMute(true);
			}*/
			if(opt != null){
				opt.set(value);
			}
			if(itemID.equals(MenuConfigManager.SETUP_MSI)){
				NavIntegration mNavIntegration = NavIntegration.getInstance(mContext);
				if(null != mNavIntegration.iGetCurrentChannel()){
					TVContent.getInstance(mContext).sendUpdate();
				}
			}
		} else {
			MtkLog.d(TAG, "[" + itemID + "] set value: " + value
					+ "   Min value: " + getMin(itemID) + "   Max value"
					+ getMax(itemID));
			if(opt != null){
				opt.set(getMin(itemID));
			}
		}
	}

	public void setScanValue(String itemID, int value) {
		tv_opt = getTVScanOption(itemID);
		if(tv_opt != null){
			tv_opt.set(value);
		}	
	}

	public void setSetup(String id, int value) {
		MtkLog.d(TAG, "set Setup: " + id + "---" + value);
		if (id.equals(OSD_LANGUAGE)) {

			osdLanguage = new OSDLanguage(mContext.getApplicationContext());
		

			if (value < 0 || value > 1) {
				value = 0;
			}
			 
			try {
				osdLanguage.setOSDLanguage(value);
				
				
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			save.saveValue(id, value);
		} else if (id.equals(POWER_OFF_TIMER)) {
			if (value < 0 || value > 2) {
				value = 0;
			}
			if (value == 0) {
				MTKPowerManager mMTKPowerManager = MTKPowerManager
						.getInstance(mContext);
				mMTKPowerManager
						.cancelPowOffTimer(MenuConfigManager.POWER_OFF_TIMER);
			} else if (value == 1) {
				MTKPowerManager mMTKPowerManager = MTKPowerManager
						.getInstance(mContext);
				String powOffTime = save.readStrValue(MenuConfigManager.TIMER2);
				Date temp = DateFormatUtil.getDate(powOffTime);
				mMTKPowerManager.timePowerOff(
						MenuConfigManager.POWER_OFF_TIMER, temp);
			} else if (value == 2) {
				MTKPowerManager mMTKPowerManager = MTKPowerManager
						.getInstance(mContext);
				String powOffTime = save.readStrValue(MenuConfigManager.TIMER2);
				Date temp = DateFormatUtil.getDate(powOffTime);
				mMTKPowerManager.timePowerOff(
						MenuConfigManager.POWER_OFF_TIMER, temp);
			}

		} else if (id.equals(POWER_ON_TIMER)) {
			if (value < 0 || value > 2) {
				value = 0;
			}
			if (value == 0) {
				MTKPowerManager mMTKPowerManager = MTKPowerManager
						.getInstance(mContext);
				mMTKPowerManager.cancelPowOnTimer();
			} else if (value == 1 || value == 2) {
				MTKPowerManager mMTKPowerManager = MTKPowerManager
						.getInstance(mContext);
				String powOnTime = save.readStrValue(MenuConfigManager.TIMER1);
				Date temp = DateFormatUtil.getDate(powOnTime);
				mMTKPowerManager.setPowerOn(temp);
			}
		} else if (id.equals(SLEEP_TIMER)) {
			sleepTimerOff = new SleepTimerOff(mContext);
			sleepTimerOff.shutDownSleep(value);
			if (value != 0) {
				return;
			}
		} else if (id.equals(AUTO_SLEEP)) {
			sleepTimerOff = new SleepTimerOff(mContext);
			sleepTimerOff.shutDownAuto(value);
		} else if (id.equals(CAPTURE_LOGO_SELECT)) {
			comm.selectCaptureLogo(value);
		} else if (id.equals(AUTO_SYNC)) {
			opt = (TVOptionRange<Integer>) cfg
					.getOption(ConfigType.CFG_TIME_SYNC);
			if (value == 1) {
				MtkLog.d(TAG, "*****Time Sync set Value: "
						+ ConfigType.TIME_SYNC_MODE_AUTO);
				if(opt != null){
					opt.set(ConfigType.TIME_SYNC_MODE_AUTO);
				}
				NetworkTime.getInstance(mContext).cancelNetTimeSync();
				TVTimerManager tvManager = tv.getTimerManager();
				Long getUTC = tvManager.getBroadcastUTC();
				// SystemClock.setCurrentTimeMillis(getUTC);
				Long mBroadcastTimeZone = tvManager.getBroadcastTZOffset();
				if (SaveValue.getInstance(mContext).readValue(
						MenuConfigManager.SETUP_TIME_ZONE) == 25) {
//					boolean t = SystemClock.setCurrentTimeMillis(getUTC
//							+ mBroadcastTimeZone);
				} else {
//					boolean clock = SystemClock.setCurrentTimeMillis(getUTC);
				}

			} else if (value == 2) {
				MtkLog.d(TAG, "*****Time Sync set Value: "
						+ ConfigType.TIME_SYNC_MODE_MANUAL);
				if(opt != null){
					opt.set(ConfigType.TIME_SYNC_MODE_MANUAL);
				}

//				NetworkTime netTime = NetworkTime.getInstance(mContext);
//				SystemClock.setCurrentTimeMillis(netTime.getNetTime());

			} else {
				MtkLog.d(TAG, "*****Time Sync set Value: "
						+ ConfigType.TIME_SYNC_MODE_MANUAL);
				if(opt != null){
					opt.set(ConfigType.TIME_SYNC_MODE_MANUAL);
					MtkLog.d(TAG, "*****auto_sync***get*********" + opt.get());
				}
				NetworkTime.getInstance(mContext).cancelNetTimeSync();
			}
			SaveValue mSaveValue = SaveValue.getInstance(mContext);
			if (mSaveValue.readValue(MenuConfigManager.POWER_ON_TIMER) != 0) {
				String poweronTime = mSaveValue
						.readStrValue(MenuConfigManager.TIMER1);
				MTKPowerManager mMtkPowerManager = MTKPowerManager
						.getInstance(mContext);
				mMtkPowerManager.updatePowerOn(poweronTime);
			}
			if (mSaveValue.readValue(MenuConfigManager.POWER_OFF_TIMER) != 0) {
				String powerOffTime = mSaveValue
						.readStrValue(MenuConfigManager.TIMER2);
				MTKPowerManager mMtkPowerManager = MTKPowerManager
						.getInstance(mContext);
				mMtkPowerManager.updatePowerOff(powerOffTime);
			}
		} else if (id.equals(SETUP_TIME_ZONE)) {
			if (value != 25) {
				SaveValue.getInstance(mContext).saveBooleanValue("Zone_time",
						false);
			} else {
				SaveValue.getInstance(mContext).saveBooleanValue("Zone_time",
						true);
			}
		}		
		save.saveValue(id, value);
	}

	public int get3DInitValue(boolean left) {
		int value = 0;
		try {
			value = cfg.check3DModeSubTpye(left);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return value;
	}


	// 3d,when 3d mode change to 2d-to-3d,toast a dialog
	public void toastWearGlass() {
		Toast toast = new Toast(mContext);
		toast.setGravity(Gravity.CENTER, 6, 20);
		toast.setDuration(Toast.LENGTH_SHORT);
		TextView view = new TextView(mContext);
		view.setBackgroundDrawable(mContext.getResources().getDrawable(
				R.drawable.mmp_infodetail_page_960));
		view.getBackground().setAlpha(100);
		view.setGravity(Gravity.CENTER);
		view.setText(mContext.getString(R.string.menu_video_3d_wear_glass));
		toast.setView(view);
		toast.show();
	}
	
	public void autoTestCheck(){
	    int address = getAddress();//0x0D37;
	    Log.e("autoTestCheck", "address:"+address);
	    byte[] getValue = eepromReadByte(address);
	    byte[] value = new byte[1];
        value[0] = 0;
	    if (getValue[0]==1) {
	        eepromWrite(address,value);
        }
	}

    public static void eepromWrite(int address, byte[] value) {
        try {
            ConfigValue configValue = new ConfigValue();
            configValue.setByteArrayValue(value);
            configValue.setIntValue(address);
            TVManager tvManager = TVManager.getInstance();
            ConfigService configService = (ConfigService)tvManager.getService(ConfigService.ConfigServiceName);
            configService.setEEPCfg(ConfigType.CFG_EEPROM_RAW_RW, configValue);
        } catch (Exception e) {
            // TODO: handle exception
        }
    }
    public static byte[] eepromReadByte(int address){
        byte[] value = new byte[1];
        try {
            ConfigValue configValue = new ConfigValue();
            TVManager tvManager = TVManager.getInstance();
            ConfigService configService = (ConfigService)tvManager.getService(ConfigService.ConfigServiceName);
            
            int[] valueArray = new int[2];
            valueArray[0] = address;
            valueArray[1] = 1;
            configValue.setIntArrayValue(valueArray);
            configValue = configService.getEEPCfg(ConfigType.CFG_EEPROM_RAW_RW,configValue);
            value = configValue.getByteArrayValue();
        } catch (Exception e) {
            // TODO: handle exception
        }
        return value;
    }

    public static int getAddress() {
        try {
            ConfigValue configOut = new ConfigValue();
            ConfigValue configIn = new ConfigValue();
            int[] mValue = new int[3];
            mValue[0] = ConfigType.D_INTERFACE_OP_GET_NORMAL;
            configIn.setIntArrayValue(mValue);
            TVManager tvManager = TVManager.getInstance();
            ConfigService configService = (ConfigService)tvManager.getService(ConfigService.ConfigServiceName);
            configOut = configService.getDrvCfg(ConfigType.D_CFG_MISC_AUTO_BOOT_EEPROM_OFFSET, configIn);
            int offset = configOut.getIntArrayValue()[0];
            return offset;

        } catch (Exception e) {
            // TODO: handle exception
        }
        return 0;
    }
		
	public void setBackLightToTV(){
	    setValue(MenuConfigManager.BACKLIGHT, getDefault(MenuConfigManager.BACKLIGHT));
	}
	
    public void setBrightness() {
        final ContentResolver resolver = mContext.getContentResolver();
        int brightness = Settings.System.getInt(resolver,Settings.System.SCREEN_BRIGHTNESS,100);
        Uri uri = Settings.System.getUriFor(Settings.System.SCREEN_BRIGHTNESS);
        final int SEEK_BAR_RANGE = 10000;
            try {
                IPowerManager power = IPowerManager.Stub.asInterface(
                        ServiceManager.getService("power"));
                Settings.System.putInt(resolver,
                        Settings.System.SCREEN_BRIGHTNESS_MODE, Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
                if (power != null) {
                    power.setTemporaryScreenBrightnessSettingOverride(brightness);
                    Settings.System.putInt(resolver,
                            Settings.System.SCREEN_BRIGHTNESS, brightness);
                    resolver.notifyChange(uri, null);
//                    Log.e("brightness", "sync:brightness:comein");
                }
//                    Log.e("brightness", "sync:brightness:"+brightness);
            } catch (RemoteException doe) {
            }
//            Toast.makeText(mContext, "value2:"+brightness, Toast.LENGTH_LONG).show();
    }
}
