package com.mediatek.ui.menu.util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import android.content.Context;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.*;
import com.mediatek.tvcommon.IntegerOption;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVOption;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.CrossRoad;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem.DataType;
import com.mediatek.ui.menu.commonview.DateTimeView;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.R;
import android.os.SystemProperties;
import android.net.wifi.WifiManager;
/**
 * initialize data item of ListView
 * 
 * @author MTK40405
 * 
 */
public class MenuDataContainer {

	// Simulation data,MINVALUE,MAXVALUE,Initial value
	private static final int MINVALUE = 0;
	private static final int MAXVALUE = 100;
	private int DEFAULTVALUE = 0;
	private static final String TAG = "MenuDataContainer";
	// current source
	public static String mCurrentSource;
	private TVContent mTV;
	private Context mContext;
	private static List<DataItem> mVideoDataGroup, mAudioDataGroup,
			mTVDataGroup, mSetUpDataGroup, mParentalDataGroup,
			mFactoryDataGroup, mAdjustDataGroup;
	private List<DataItem> mAnalogScanSubChildGroup;
	private DataItem tvChannel;
	private static List<DataItem> mRFScanDataGroup;
	private MenuConfigManager mConfigManager;
	private String ItemName;
	private static MenuDataContainer mMenuDataContainer;
	private SaveValue saveV;
	// channel information list,as child of channel edit and clear
	public ArrayList<DataItem> mChanelsInfoGroup;

	private TVChannel tvCh;
	private TVChannelSelector chSel;


	public boolean is3DPhoto =false;
	private boolean haveScreenMode = false;
	
	private static final String OSD_LANGUAGE = "SETUP_osd_language";
	
	private int OSD_LANGUAGE_CHINESE = 1;
	private int THRDPHOTO_AUTO_MODE= 1;
	// public void clearAllData() {
	// mVideoDataGroup = null;
	// mAudioDataGroup = null;
	// mTVDataGroup = null;
	// mSetUpDataGroup = null;
	// mParentalDataGroup = null;
	// mFactoryDataGroup = null;
	// }

	public List<DataItem> getVideoDataGroup() {
		return mVideoDataGroup;
	}

	public void setVideoDataGroup(List<DataItem> mVideoDataGroup) {
		this.mVideoDataGroup = mVideoDataGroup;
	}

	public List<DataItem> getAudioDataGroup() {
		return mAudioDataGroup;
	}

	public void setAudioDataGroup(List<DataItem> mAudioDataGroup) {
		this.mAudioDataGroup = mAudioDataGroup;
	}

	public List<DataItem> getTVDataGroup() {
		return mTVDataGroup;
	}

	public void setTVDataGroup(List<DataItem> mTVDataGroup) {
		this.mTVDataGroup = mTVDataGroup;
	}

	public List<DataItem> getSetUpDataGroup() {
		return mSetUpDataGroup;
	}

	public void setSetUpDataGroup(List<DataItem> mSetUpDataGroup) {
		this.mSetUpDataGroup = mSetUpDataGroup;
	}

	public List<DataItem> getmSetUpDataGroup() {
		return mSetUpDataGroup;
	}

	public void setmSetUpDataGroup(List<DataItem> mSetUpDataGroup) {
		this.mSetUpDataGroup = mSetUpDataGroup;
	}

	public List<DataItem> getmParentalDataGroup() {
		return mParentalDataGroup;
	}

	public void setmParentalDataGroup(List<DataItem> mParentalDataGroup) {
		this.mParentalDataGroup = mParentalDataGroup;
	}

	public List<DataItem> getmFactoryDataGroup() {
		return mFactoryDataGroup;
	}

	public void setmFactoryDataGroup(List<DataItem> mFactoryDataGroup) {
		this.mFactoryDataGroup = mFactoryDataGroup;
	}

	private MenuDataContainer(Context context) {
		this.mContext = context;
		mConfigManager = MenuConfigManager.getInstance(context);
		saveV = SaveValue.getInstance(context);
		mTV = TVContent.getInstance(mContext);
		chSel = TVContent.getInstance(mContext).getChannelSelector();
	}

	/**
	 * string to show in UI for video related options
	 */
	public static MenuDataContainer getInstance(Context context) {
		if (mMenuDataContainer == null) {
			mMenuDataContainer = new MenuDataContainer(context);
		}
		return mMenuDataContainer;
	}

	/** string to show in UI for Video related options */
	private String[] pictureMode;
	private String[] gamma;
	private String[] colorTemperature2;
	private String[] dnr;
	private String[] videoMpegNr;
	private String[] luma;
	private String[] fleshTone;
	private String[] videoDiFilm;
	private String[] blueStretch;
	private String[] gameMode;
	private String[] pqsplitdemoMode;
	private String[] vgaMode;
	private String[] hdmiMode;
	private DataItem mColorTemperature;
	private DataItem mAdvancedVideo;
	private DataItem mPictureMode;
	private String[] m3DModeStrArr;

	/** string to show in UI for tv related options */
	private String[] mts;
	private String[] audioLanuage;
	private String[] audioLanuage2nd;
	private String[] tvSystem;
	private String[] colorSystem;
	private String[] poweronChannel;

	/** strings to show in UI for setup related options */
	private String[] OSDLanguage;
	private String[] DPMS;
	private String[] blueMute;
	private String[] MSI;
	// effect
	private String[] videoEffect;
	// mjc mode
	private String[] videoMjcMode;
	private String[] fastBoot;
	
	
	
	private String[] dTLanguage;
	private String[] dPLanguage;
	private String[] tPLevel;

	// 3d
	String[] m3DModeArr;
	// 3d navigation
	String[] m3DNavArr;
	String[] m3D2DArr;
	String[] m3DImgSafetyArr;
	String[] m3DLrSwitchArr;

	private DataItem setupTime;
	private DataItem network;
	private DataItem teletext;
	private DataItem versionInfo;
	private DataItem resetDefault;
	private DataItem m3DItem;

	private DataItem mColorTemperature2;
	private DataItem powerOnChannel;
	private DataItem powerOnChannelMode;
	private DataItem powerOnChannelModeInvalid;
	private DataItem parentalPwd;
	private DataItem channelBlock;
	private DataItem inputBlock;
	private DataItem fvTunerDiagnostic;
	private DataItem factoyTV;
	private DataItem timeSet;
	private String blockSourceName;
	private String blockSourceName3D;
	private DataItem application;

	private List<DataItem> channelDataGroup = new ArrayList<DataItem>();
	private List<DataItem> channelPowerOnDataGroup = new ArrayList<DataItem>();
	private List<DataItem> channelBlockOnDataGroup = new ArrayList<DataItem>();
	private List<DataItem> inputBlockOnDataGroup = new ArrayList<DataItem>();
	private List<String[]> channelInfo = new ArrayList<String[]>();
	
	private List<DataItem>channelScanDataGroup = new ArrayList<DataItem>();
	private int chNum = 0;
	private List<TVChannel> ch_list = null;
	private DataItem channelEdit;

	public List<String[]> getChannelInfo() {
		return channelInfo;
	}

	public List<DataItem> getChannelScan() {
		return channelScanDataGroup;
	}

	public void setChannelInfo(List<String[]> channelInfo) {
		this.channelInfo = channelInfo;
	}

	public int getChNum() {
		return chNum;
	}

	public void setChNum(int chNum) {
		this.chNum = chNum;
	}

	public List<TVChannel> getCh_list() {
		return ch_list;
	}

	public void setCh_list(List<TVChannel> ch_list) {
		this.ch_list = ch_list;
	}

	public List<DataItem> getChannelDataGroup() {
		return channelDataGroup;
	}

	public List<DataItem> getVGADataGroup() {
		return mAdjustDataGroup;
	}

	public void setChannelDataGroup(List<DataItem> channelDataGroup) {
		this.channelDataGroup = channelDataGroup;
	}

	public List<DataItem> getChannelPowerOnDataGroup() {
		return channelPowerOnDataGroup;
	}

	public void setChannelPowerOnDataGroup(
			List<DataItem> channelPowerOnDataGroup) {
		this.channelPowerOnDataGroup = channelPowerOnDataGroup;
	}

	public List<DataItem> getChannelBlockOnDataGroup() {
		return channelBlockOnDataGroup;
	}

	public void setChannelBlockOnDataGroup(
			List<DataItem> channelBlockOnDataGroup) {
		this.channelBlockOnDataGroup = channelBlockOnDataGroup;
	}

	/**
	 * load data for video
	 */
	public void loadDataVideo() {
		// if (mVideoDataGroup != null && mVideoDataGroup.size() > 0) {
		// return;
		// } else {
		pictureMode = mContext.getResources().getStringArray(
				R.array.picture_effect_array);
		gamma = mContext.getResources().getStringArray(
				R.array.menu_video_gamma_array);
		colorTemperature2 = mContext.getResources().getStringArray(
				R.array.menu_video_color_temperature2_array);
		dnr = mContext.getResources().getStringArray(
				R.array.menu_video_dnr_array);
		videoMpegNr = mContext.getResources().getStringArray(
				R.array.menu_video_mpeg_nr_array);
		videoDiFilm = mContext.getResources().getStringArray(
				R.array.menu_video_di_film_mode_array);
		luma = mContext.getResources().getStringArray(
				R.array.menu_video_luma_array);
		fleshTone = mContext.getResources().getStringArray(
				R.array.menu_video_flesh_tone_array);
		blueStretch = mContext.getResources().getStringArray(
				R.array.menu_video_blue_stretch_array);
		gameMode = mContext.getResources().getStringArray(
				R.array.menu_video_game_mode_array);
		
		pqsplitdemoMode = mContext.getResources().getStringArray(
                R.array.menu_video_pq_split_mode_array);
		vgaMode = mContext.getResources().getStringArray(
				R.array.menu_video_vga_mode_array);
		hdmiMode = mContext.getResources().getStringArray(
				R.array.menu_video_hdmi_mode_array);
		// effect
		videoEffect = mContext.getResources().getStringArray(
				R.array.menu_video_mjc_effect_array);
		// demo partition
		/*
		 * videoDemoPa = mContext.getResources().getStringArray(
		 * R.array.menu_video_mjc_demo_partition_array);
		 */
		// Mjc Mode
		videoMjcMode = mContext.getResources().getStringArray(
				R.array.menu_video_mjc_mode);
		// 3d
		m3DModeArr = mContext.getResources().getStringArray(
				R.array.menu_video_3d_mode_array);
		// 3d navigation
		m3DNavArr = mContext.getResources().getStringArray(
				R.array.menu_video_3d_nav_array);
		m3D2DArr = mContext.getResources().getStringArray(
				R.array.menu_video_3d_3t2switch_array);
		m3DImgSafetyArr = mContext.getResources().getStringArray(
				R.array.menu_video_3d_image_safety_array);
		m3DLrSwitchArr = mContext.getResources().getStringArray(
				R.array.menu_video_3d_lrswitch_array);

		mVideoDataGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.PICTURE_MODE;
		mPictureMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_picture_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), pictureMode,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.EFFECTOPTIONVIEW);

		mPictureMode.mEffectGroup = new ArrayList<DataItem>();
		mVideoDataGroup.add(mPictureMode);
		// mPictureMode.mNoEffectIndex = 0;
		mPictureMode.setUserDefined(0);

		ItemName = MenuConfigManager.BACKLIGHT;
		DataItem mBackLight = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_backlight), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mVideoDataGroup.add(mBackLight);

		ItemName = MenuConfigManager.BRIGHTNESS;
		DataItem mBrightness = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_brighttness), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mBrightness.mParent = mPictureMode;
		mVideoDataGroup.add(mBrightness);

		ItemName = MenuConfigManager.CONTRAST;
		DataItem mContrast = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_contrast), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mContrast.mParent = mPictureMode;
		mVideoDataGroup.add(mContrast);

		ItemName = MenuConfigManager.SATURATION;
		DataItem mSaturation = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_saturation), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mSaturation.mParent = mPictureMode;
		mVideoDataGroup.add(mSaturation);

		ItemName = MenuConfigManager.HUE;
		DataItem mHue = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_hue), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.POSITIONVIEW);
		mHue.mParent = mPictureMode;
		mVideoDataGroup.add(mHue);

		// when not VGA,show definition,when VGA,not show
		if (!TVInputManager.INPUT_TYPE_VGA.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))) {
			ItemName = MenuConfigManager.SHARPNESS;
			DataItem mSharpness = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_sharpness), mConfigManager
					.getMin(ItemName), mConfigManager.getMax(ItemName),
					mConfigManager.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
			mSharpness.mParent = mPictureMode;
			mVideoDataGroup.add(mSharpness);
			for (int i = 2; i <= 6; i++) {
				mPictureMode.mEffectGroup.add(mVideoDataGroup.get(i));
			}
		} else {
			for (int i = 2; i <= 5; i++) {
				mPictureMode.mEffectGroup.add(mVideoDataGroup.get(i));
			}
		}

		ItemName = MenuConfigManager.GAMMA;
		mVideoDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_video_gamma),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), gamma,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		mColorTemperature = new DataItem(
				MenuConfigManager.VIDEO_COLOR_TEMPERATURE, mContext
						.getString(R.string.menu_video_color_temperature),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		mColorTemperature.mSubChildGroup = new ArrayList<DataItem>();

		ItemName = MenuConfigManager.COLOR_TEMPERATURE;
		mColorTemperature2 = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_color_temperature),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), colorTemperature2,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.EFFECTOPTIONVIEW);
		mColorTemperature.mSubChildGroup.add(mColorTemperature2);

		
		TVOption<Integer> focusOption = (TVOption<Integer>) mConfigManager.getTvConfigurer()
                .getOption(IntegerOption.CFG_TV_FOCUS_WIN);
        if(focusOption.get() == ConfigType.TV_FOCUS_WIN_SUB){
            mColorTemperature.setEnable(false);
        }else {
            mColorTemperature.setEnable(true);
        }
		
		ItemName = MenuConfigManager.COLOR_G_R;
		DataItem COLOR_G_R = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_color_g_red), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);

		ItemName = MenuConfigManager.COLOR_G_G;
		DataItem COLOR_G_G = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_color_g_green), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);

		ItemName = MenuConfigManager.COLOR_G_B;
		DataItem COLOR_G_B = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_color_g_blue), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mColorTemperature.mSubChildGroup.add(COLOR_G_R);
		mColorTemperature.mSubChildGroup.add(COLOR_G_G);
		mColorTemperature.mSubChildGroup.add(COLOR_G_B);

		mVideoDataGroup.add(mColorTemperature);
		mColorTemperature.setmParentGroup(mVideoDataGroup);

		// mColorTemperature2.mNoEffectIndex = 0;
		// when change value of down,model change as user
		mColorTemperature2.setUserDefined(0);
		mColorTemperature2.mEffectGroup = new ArrayList<DataItem>();

		COLOR_G_R.mParentGroup=mColorTemperature2.mEffectGroup;
		COLOR_G_G.mParentGroup=mColorTemperature2.mEffectGroup;
		COLOR_G_B.mParentGroup=mColorTemperature2.mEffectGroup;

		mColorTemperature2.mEffectGroup.add(COLOR_G_R);
		mColorTemperature2.mEffectGroup.add(COLOR_G_G);
		mColorTemperature2.mEffectGroup.add(COLOR_G_B);

		COLOR_G_R.mParent = mColorTemperature2;
		COLOR_G_G.mParent = mColorTemperature2;
		COLOR_G_B.mParent = mColorTemperature2;

		mAdvancedVideo = new DataItem(MenuConfigManager.VIDEO_ADVANCED_VIDEO,
				mContext.getString(R.string.menu_video_advancedvideo),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);

		loadAdvancedData();

		mVideoDataGroup.add(mAdvancedVideo);
		mAdvancedVideo.setmParentGroup(mVideoDataGroup);

		// only VGA have
		if (TVInputManager.INPUT_TYPE_VGA.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))
				&& NavIntegration.getInstance(mContext)
						.iCurrentInputSourceHasSignal()) {
			mAdjustDataGroup = new ArrayList<DataItem>();
			// VGA
			ItemName = MenuConfigManager.VGA;
			DataItem mVideoVga = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_vga),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			mVideoVga.mSubChildGroup = mAdjustDataGroup;

			// AUTO ADJUST
			ItemName = MenuConfigManager.AUTO_ADJUST;
			DataItem mVideoAutoAdjust = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_auto_adjust),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			mAdjustDataGroup.add(mVideoAutoAdjust);
			mVideoAutoAdjust.mSubChildGroup = new ArrayList<DataItem>();
			mVideoAutoAdjust.setmParentGroup(mVideoVga.mSubChildGroup);

			// H POSITION
			ItemName = MenuConfigManager.HPOSITION;
			DataItem mVideoAutoHPosition = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_hposition), mConfigManager
					.getMin(ItemName), mConfigManager.getMax(ItemName),
					mConfigManager.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
			mAdjustDataGroup.add(mVideoAutoHPosition);
			mVideoAutoHPosition.setmParentGroup(mVideoVga.mSubChildGroup);

			// V POSITION
			ItemName = MenuConfigManager.VPOSITION;
			DataItem mVideoAutoVPosition = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_vposition), mConfigManager
					.getMin(ItemName), mConfigManager.getMax(ItemName),
					mConfigManager.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
			mAdjustDataGroup.add(mVideoAutoVPosition);
			mVideoAutoVPosition.setmParentGroup(mVideoVga.mSubChildGroup);

			// PHASE
			ItemName = MenuConfigManager.PHASE;
			DataItem mVideoAutoVPhase = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_phase), mConfigManager
					.getMin(ItemName), mConfigManager.getMax(ItemName),
					mConfigManager.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
			mAdjustDataGroup.add(mVideoAutoVPhase);
			mVideoAutoVPhase.setmParentGroup(mVideoVga.mSubChildGroup);

			// CLOCK
			ItemName = MenuConfigManager.CLOCK;
			DataItem mVideoAutoClock = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_clock), mConfigManager
					.getMin(ItemName), mConfigManager.getMax(ItemName),
					mConfigManager.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
			mAdjustDataGroup.add(mVideoAutoClock);
			mVideoAutoClock.setmParentGroup(mVideoVga.mSubChildGroup);

			mVideoDataGroup.add(mVideoVga);
			mVideoVga.setmParentGroup(mVideoDataGroup);
		}
		// to do 3D
		m3DItem = new DataItem(MenuConfigManager.VIDEO_3D, mContext
				.getString(R.string.menu_video_3d),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		m3DItem.mSubChildGroup = new ArrayList<DataItem>();
		mVideoDataGroup.add(m3DItem);
		m3DItem.setmParentGroup(mVideoDataGroup);

	}

	public DataItem getAdvancedData() {
		return mAdvancedVideo;
	}

	/**
	 * Load Advanced Data
	 */
	public void loadAdvancedData() {

		NavIntegration mNavIntegration = NavIntegration.getInstance(mContext);
		mAdvancedVideo.mSubChildGroup = new ArrayList<DataItem>();

		// when VGA,Advanced video only have VGA model,when other source,not
		// show this item
		if (TVInputManager.INPUT_TYPE_VGA.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))) {
			ItemName = MenuConfigManager.VGA_MODE;
			DataItem mVideoVgaMode = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_vga_mode),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), vgaMode,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
			mAdvancedVideo.mSubChildGroup.add(mVideoVgaMode);
			mVideoVgaMode.setmParentGroup(mAdvancedVideo.mSubChildGroup);
		} else {
			ItemName = MenuConfigManager.DNR;
			DataItem dnrItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_dnr),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), dnr,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			ItemName = MenuConfigManager.MPEG_NR;
			DataItem mpegItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_mpeg_nr),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), videoMpegNr,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			ItemName = MenuConfigManager.ADAPTIVE_LUMA_CONTROL;
			DataItem adaptLumaItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_luma),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), luma,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			ItemName = MenuConfigManager.FLESH_TONE;
			DataItem fleshToneItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_flesh_tone),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), fleshTone,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			ItemName = MenuConfigManager.DI_FILM_MODE;
			DataItem diFilmItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_di_film_mode),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), videoDiFilm,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			ItemName = MenuConfigManager.BLUE_STRETCH;
			DataItem blueStrechItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_blue_stretch),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), blueStretch,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			ItemName = MenuConfigManager.GAME_MODE;
			DataItem mVideoGameMode = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_wme_mode),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), gameMode,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.SWICHOPTIONVIEW);





			// only when TV model,game model can use
			if (TVInputManager.INPUT_TYPE_TV.equals(mTV.getInputManager()
					.getTypeFromInputSource(mCurrentSource))) {
				mVideoGameMode.isEnable = true;
			} else {
				mVideoGameMode.isEnable = false;
			}


			ItemName = MenuConfigManager.PQ_SPLIT_SCREEN_DEMO_MODE;
			DataItem pqsplitdemo = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_pq_split_mode),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), pqsplitdemoMode,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			
			// MJC
			ItemName = MenuConfigManager.MJC;
			DataItem mVideoMjc = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_mjc),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			mVideoMjc.mSubChildGroup = new ArrayList<DataItem>();
			mVideoMjc.setmParentGroup(mAdvancedVideo.mSubChildGroup);

			// effect
			ItemName = MenuConfigManager.EFFECT;
			DataItem VideoEffect = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_mjc_effect),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), videoEffect,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.SWICHOPTIONVIEW);

			mVideoMjc.mSubChildGroup.add(VideoEffect);
			VideoEffect.setmParent(mVideoMjc);

			/*
			 * //demo partition ItemName = MenuConfigManager.DEMO_PARTITION;
			 * DataItem VideoDemoPa = new DataItem(ItemName, mContext
			 * .getString(R.string.menu_video_mjc_demo_partition),
			 * MenuConfigManager.INVALID_VALUE,
			 * MenuConfigManager.INVALID_VALUE,mConfigManager
			 * .getDefault(ItemName) - mConfigManager.getMin(ItemName),
			 * videoDemoPa, MenuConfigManager.STEP_VALUE,
			 * DataItem.DataType.OPTIONVIEW);
			 * mVideoMjc.mSubChildGroup.add(VideoDemoPa);
			 * VideoDemoPa.setmParent(mVideoMjc);
			 */

			// mode
			ItemName = MenuConfigManager.MENU_MJC_MODE;
			DataItem VideoMjcMode = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_mjc_mode),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), videoMjcMode,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
			mVideoMjc.mSubChildGroup.add(VideoMjcMode);
			VideoMjcMode.setmParent(mVideoMjc);

			// demo
			ItemName = MenuConfigManager.DEMO;
			DataItem VideoDemo = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_mjc_demo),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			VideoDemo.mSubChildGroup = new ArrayList<DataItem>();
			VideoDemo.setmParent(mVideoMjc);
			mVideoMjc.mSubChildGroup.add(VideoDemo);

			VideoEffect.mEffectGroup = new ArrayList<DataItem>();
			VideoEffect.mEffectGroup.add(VideoMjcMode);
			VideoEffect.mEffectGroup.add(VideoDemo);
			VideoEffect.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
			// off,not available
			VideoEffect.mSwitchHashMap.put(0, new Boolean[] { false, false });
			// low,available
			VideoEffect.mSwitchHashMap.put(1, new Boolean[] { true, true });
			// middle,available
			VideoEffect.mSwitchHashMap.put(2, new Boolean[] { true, true });
			// high,available
			VideoEffect.mSwitchHashMap.put(3, new Boolean[] { true, true });

			// off,not available
			if (VideoEffect.mInitValue == 0) {
				VideoMjcMode.setEnable(false);
				VideoDemo.setEnable(false);
			} else {
				VideoMjcMode.setEnable(true);
				VideoDemo.setEnable(true);
			}

			// HDMI Mode
			ItemName = MenuConfigManager.HDMI_MODE;
			DataItem hdmiItem = new DataItem(ItemName, mContext
					.getString(R.string.menu_video_hdmi_mode),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), hdmiMode,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.SWICHOPTIONVIEW);

			mVideoGameMode.mEffectGroup = new ArrayList<DataItem>();
			mVideoGameMode.mEffectGroup.add(diFilmItem);
			mVideoGameMode.mEffectGroup.add(mVideoMjc);
			mVideoGameMode.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
			mVideoGameMode.mSwitchHashMap.put(0, new Boolean[] { true, true });
			mVideoGameMode.mSwitchHashMap
					.put(1, new Boolean[] { false, false });

			hdmiItem.mEffectGroup = new ArrayList<DataItem>();
			hdmiItem.mEffectGroup.add(dnrItem);
			hdmiItem.mEffectGroup.add(mpegItem);
			hdmiItem.mEffectGroup.add(adaptLumaItem);
			hdmiItem.mEffectGroup.add(fleshToneItem);
			hdmiItem.mEffectGroup.add(diFilmItem);
			hdmiItem.mEffectGroup.add(blueStrechItem);
			hdmiItem.mEffectGroup.add(mVideoGameMode);
			hdmiItem.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
			hdmiItem.mSwitchHashMap.put(0, new Boolean[] { true, true, true,
					true, false, true, true });
			hdmiItem.mSwitchHashMap.put(1, new Boolean[] { false, false, false,
					false, false, false, false });
			hdmiItem.mSwitchHashMap.put(2, new Boolean[] { true, true, true,
					true, false, true, true });

			mAdvancedVideo.mSubChildGroup.add(dnrItem);
			mAdvancedVideo.mSubChildGroup.add(mpegItem);
			mAdvancedVideo.mSubChildGroup.add(adaptLumaItem);
			mAdvancedVideo.mSubChildGroup.add(fleshToneItem);
			mAdvancedVideo.mSubChildGroup.add(diFilmItem);
			mAdvancedVideo.mSubChildGroup.add(blueStrechItem);
			mAdvancedVideo.mSubChildGroup.add(mVideoGameMode);
			mAdvancedVideo.mSubChildGroup.add(pqsplitdemo);
			mAdvancedVideo.mSubChildGroup.add(mVideoMjc);

			if (mNavIntegration.isCurrentSourceHDMI()) {
				diFilmItem.setEnable(false);
				if (mNavIntegration.iCurrentInputSourceHasSignal()) {
					mAdvancedVideo.mSubChildGroup.add(hdmiItem);
					boolean temp = (hdmiItem.mInitValue != 1);
					dnrItem.setEnable(temp);
					mpegItem.setEnable(temp);
					adaptLumaItem.setEnable(temp);
					fleshToneItem.setEnable(temp);
					blueStrechItem.setEnable(temp);
					mVideoGameMode.setEnable(temp);
				} else {
					mVideoGameMode.setEnable(false);
				}
			} else {
				if (mNavIntegration.isCurrentSourceDTV()
						&& mNavIntegration.iGetChannelLength() > 0
						&& mNavIntegration.iCurrentChannelHasSignal()) {
					mVideoGameMode.setEnable(true);
				} else {
					mVideoGameMode.setEnable(false);
				}
			}

			if (mVideoGameMode.mInitValue == 0) {
				if (!mNavIntegration.isCurrentSourceHDMI()) {
					diFilmItem.setEnable(true);
				}else{
					diFilmItem.setEnable(false);
				}
				mVideoMjc.setEnable(true);
			} else {
				diFilmItem.setEnable(false);
				mVideoMjc.setEnable(false);
			}
		}
	}

	/**
	 * Load 3D Data
	 */
	public void load3DData() {
		if (m3DItem.mSubChildGroup != null && m3DItem.mSubChildGroup.size() > 0) {
			m3DItem.mSubChildGroup.clear();
		}

		if (m3DItem.mSubChildGroup != null) {
		// 3d mode
		ItemName = MenuConfigManager.VIDEO_3D_MODE;
		DataItem m3DMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), m3DModeArr,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		m3DItem.mSubChildGroup.add(m3DMode);
		m3DMode.mParent = m3DItem;
		/*if(is3DPhoto){
			mConfigManager.setValue(ItemName, THRDPHOTO_AUTO_MODE);
		}else*/{
			mConfigManager.setValue(ItemName, mConfigManager.getDefault(ItemName)
					- mConfigManager.getMin(ItemName));
		}


		// 3d navigation
		ItemName = MenuConfigManager.VIDEO_3D_NAV;
		DataItem m3DNav = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_nav),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), m3DNavArr,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		m3DItem.mSubChildGroup.add(m3DNav);
		m3DNav.mParent = m3DItem;
		mConfigManager.setValue(ItemName, mConfigManager.getDefault(ItemName)
				- mConfigManager.getMin(ItemName));

		// 3d-2d
		ItemName = MenuConfigManager.VIDEO_3D_3T2;
		DataItem m3D2D = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_3t2),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), m3D2DArr,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		m3DItem.mSubChildGroup.add(m3D2D);
		m3D2D.mParent = m3DItem;
		mConfigManager.setValue(ItemName, mConfigManager.getDefault(ItemName)
				- mConfigManager.getMin(ItemName));

		// depth of field
		ItemName = MenuConfigManager.VIDEO_3D_FIELD;
		DataItem m3DDepthField = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_depth_field), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		m3DItem.mSubChildGroup.add(m3DDepthField);
		m3DDepthField.mParent = m3DItem;

		// protrude
		ItemName = MenuConfigManager.VIDEO_3D_PROTRUDE;
		DataItem m3DProtrude = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_protrude), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		m3DItem.mSubChildGroup.add(m3DProtrude);
		m3DProtrude.mParent = m3DItem;

		// distance to TV
		ItemName = MenuConfigManager.VIDEO_3D_DISTANCE;
		DataItem m3DDistance = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_distance), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		m3DItem.mSubChildGroup.add(m3DDistance);
		m3DDistance.mParent = m3DItem;

		// image safety
		ItemName = MenuConfigManager.VIDEO_3D_IMG_SFTY;
		DataItem m3DImgSafety = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_image_safety),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), m3DImgSafetyArr,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		m3DItem.mSubChildGroup.add(m3DImgSafety);
		m3DImgSafety.mParent = m3DItem;
		mConfigManager.setValue(ItemName, mConfigManager.getDefault(ItemName)
				- mConfigManager.getMin(ItemName));

		// L-R switch
		ItemName = MenuConfigManager.VIDEO_3D_LF;
		DataItem m3DLrSwitch = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_3d_leftright),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), m3DLrSwitchArr,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		m3DItem.mSubChildGroup.add(m3DLrSwitch);
		m3DLrSwitch.mParent = m3DItem;
		mConfigManager.setValue(ItemName, mConfigManager.getDefault(ItemName)
				- mConfigManager.getMin(ItemName));

		// // OSD Depth
		// ItemName = MenuConfigManager.VIDEO_3D_OSD_DEPTH;
		// DataItem m3DOsdDepth = new DataItem(ItemName, mContext
		// .getString(R.string.menu_video_3d_osd), mConfigManager
		// .getMin(ItemName), mConfigManager.getMax(ItemName),
		// mConfigManager.getDefault(ItemName), null,
		// MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		// m3DItem.mSubChildGroup.add(m3DOsdDepth);
		// m3DOsdDepth.mParent = m3DItem;

		ArrayList<Boolean> m3DConfigList = mConfigManager.get3DConfig();
		boolean m3DModeFlag = m3DConfigList.get(0);
		boolean m3DNavFlag = m3DConfigList.get(1);
		boolean m3D2DFlag = m3DConfigList.get(2);
		boolean m3DDepthFieldFlag = m3DConfigList.get(3);
		boolean m3DProtrudeFlag = m3DConfigList.get(4);
		boolean m3DDistanceFlag = m3DConfigList.get(5);
		boolean m3DImgSafetyFlag = m3DConfigList.get(6);
		boolean m3DLrSwitchFlag = m3DConfigList.get(7);
		// boolean m3DOsdDepthFlag = m3DConfigList.get(8);

		m3DMode.setEnable(m3DModeFlag);
		m3DNav.setEnable(m3DNavFlag);
		m3D2D.setEnable(m3D2DFlag);
		m3DDepthField.setEnable(m3DDepthFieldFlag);
		m3DProtrude.setEnable(m3DProtrudeFlag);
		m3DDistance.setEnable(m3DDistanceFlag);
		m3DImgSafety.setEnable(m3DImgSafetyFlag);
		m3DLrSwitch.setEnable(m3DLrSwitchFlag);
		// m3DOsdDepth.setEnable(m3DOsdDepthFlag);
		}
	}

	/**
	 * get the value of data item which is marked by mId
	 * 
	 * @param mId
	 *            the flag of data item
	 * @return the value
	 */
	public int getValueById(String mId) {
		if (mId != null && mId.trim().length() > 0) {
			return mConfigManager.getDefault(mId) - mConfigManager.getMin(mId);
		} else {
			throw new IllegalArgumentException(
					"mId must be an contain elements of a character string");
		}
	}

	/**
	 * get positions of data item which are effected by parent data item
	 * 
	 * @return the position of data item
	 */
	public int[] getEffectGroupInitValues(DataItem mEffectParentDataItem) {
		int mEffectGroupInitValues[];
		if (mEffectParentDataItem != null
				&& mEffectParentDataItem.getDataType() == DataType.EFFECTOPTIONVIEW) {
			mEffectGroupInitValues = new int[mEffectParentDataItem
					.getmEffectGroup().size()];
			int i = 0;
			for (DataItem childDataItem : mEffectParentDataItem
					.getmEffectGroup()) {
				mEffectGroupInitValues[i] = mConfigManager
						.getDefault(childDataItem.getmItemID());
				i++;
			}
			return mEffectGroupInitValues;
		} else {
			throw new IllegalArgumentException(
					"type of mEffectParentDataItem is not DataType.EFFECTOPTIONVIEW or mEffectParentDataItem is null");
		}
	}

	/**
	 * load data for Audio
	 */
	public void loadDataAudio() {
		// if (mAudioDataGroup != null && mAudioDataGroup.size() > 0) {
		// return;
		// } else {
		String[] SRS_Mode = mContext.getResources().getStringArray(
				R.array.menu_audio_srs_mode_array);
		String[] equalizer = mContext.getResources().getStringArray(
				R.array.menu_audio_equalizer_array);
		String[] Speaker_Mode = mContext.getResources().getStringArray(
				R.array.menu_audio_speaker_mode_array);
		String[] SPDIF_Mode = mContext.getResources().getStringArray(
				R.array.menu_audio_spdif_mode_array);
		String[] AVC_Mode = mContext.getResources().getStringArray(
				R.array.menu_audio_avc_mode_array);
		String[] TYPE_Mode = mContext.getResources().getStringArray(
				R.array.menu_audio_type_array);
		String[] mVisuallySpeaker = mContext.getResources().getStringArray(
				R.array.menu_audio_visually_speaker_array);
		String[] mVisuallyHeadphone = mContext.getResources().getStringArray(
				R.array.menu_audio_visually_headphone_array);

		mAudioDataGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.BALANCE;
		DataItem audioBalance = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_balance), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.POSITIONVIEW);

		ItemName = MenuConfigManager.BASS;
		DataItem audioBass = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_bass), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);

		ItemName = MenuConfigManager.TREBLE;
		DataItem audioTreble = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_treble), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);

		ItemName = MenuConfigManager.SRS_MODE;
		DataItem audioSoundSur = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_srs_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), SRS_Mode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		ItemName = MenuConfigManager.EQUALIZE;
		DataItem audioEqualizer = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_equalize),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), equalizer,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		ItemName = MenuConfigManager.SPEAKER_MODE;
		DataItem audioSpeaker = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_speaker_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), Speaker_Mode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		ItemName = MenuConfigManager.SPDIF_MODE;
		DataItem audioSpdifType = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_spdif_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), SPDIF_Mode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.SWICHOPTIONVIEW);

		ItemName = MenuConfigManager.SPDIF_DELAY;
		DataItem audioSpdifDelay = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_spdif_delay), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);

		audioSpdifType.mEffectGroup = new ArrayList<DataItem>();
		audioSpdifType.mEffectGroup.add(audioSpdifDelay);
		audioSpdifType.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
		audioSpdifType.mSwitchHashMap.put(0, new Boolean[] { false });
		audioSpdifType.mSwitchHashMap.put(1, new Boolean[] { true });
		audioSpdifType.mSwitchHashMap.put(2, new Boolean[] { true });
		audioSpdifType.mSwitchHashMap.put(3, new Boolean[] { true });
		if (mConfigManager.getDefault(MenuConfigManager.SPDIF_MODE) == 0) {
			audioSpdifDelay.setEnable(false);
		} else {
			audioSpdifDelay.setEnable(true);
		}

		ItemName = MenuConfigManager.TYPE;
		DataItem audioType = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_type),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), TYPE_Mode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.SWICHOPTIONVIEW);

		ItemName = MenuConfigManager.VISUALLY_IMPAIRED;
		DataItem audioVisuallyImpaired = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_visually_impaired),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);

		audioType.mEffectGroup = new ArrayList<DataItem>();
		audioType.mEffectGroup.add(audioVisuallyImpaired);
		audioType.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
		// normal can not use
		audioType.mSwitchHashMap.put(0, new Boolean[] { false });
		// hearing impaired can not use
		audioType.mSwitchHashMap.put(1, new Boolean[] { false });
		// visually impaired available
		audioType.mSwitchHashMap.put(2, new Boolean[] { true });

		// when parent initial data is visually,available of down
		if (mConfigManager.getDefault(MenuConfigManager.TYPE) == 2) {
			audioVisuallyImpaired.setEnable(true);
		} else {
			audioVisuallyImpaired.setEnable(false);
		}
		audioVisuallyImpaired.mSubChildGroup = new ArrayList<DataItem>();

		// VisuallySpeaker
		ItemName = MenuConfigManager.VISUALLY_SPEAKER;
		audioVisuallyImpaired.mSubChildGroup.add(new DataItem(ItemName,
				mContext.getString(R.string.menu_audio_visually_speaker),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mVisuallySpeaker,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		// VisuallyHeadphone
		ItemName = MenuConfigManager.VISUALLY_HEADPHONE;
		audioVisuallyImpaired.mSubChildGroup.add(new DataItem(ItemName,
				mContext.getString(R.string.menu_audio_visually_headphone),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mVisuallyHeadphone,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		// VisuallyVolume
		ItemName = MenuConfigManager.VISUALLY_VOLUME;
		audioVisuallyImpaired.mSubChildGroup.add(new DataItem(ItemName,
				mContext.getString(R.string.menu_audio_visually_volume),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR));
		audioVisuallyImpaired.setmParentGroup(mAudioDataGroup);

		ItemName = MenuConfigManager.AVCMODE;
		DataItem audioAutoVoice = new DataItem(ItemName, mContext
				.getString(R.string.menu_audio_avc_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), AVC_Mode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		mAudioDataGroup.add(audioBalance);
		mAudioDataGroup.add(audioBass);
		mAudioDataGroup.add(audioTreble);
		mAudioDataGroup.add(audioSoundSur);
		mAudioDataGroup.add(audioEqualizer);
		mAudioDataGroup.add(audioSpeaker);
		mAudioDataGroup.add(audioSpdifType);
		mAudioDataGroup.add(audioSpdifDelay);
		mAudioDataGroup.add(audioAutoVoice);
		mAudioDataGroup.add(audioType);
		mAudioDataGroup.add(audioVisuallyImpaired);
		// }

	}

	public void loadDataChannelScan(){
		// tv system 3
		ItemName = MenuConfigManager.TV_SYSTEM;
		channelScanDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_tv_system),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getScanDefault(ItemName), tvSystem,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.SCANCHANNELSOPTIONVIEW));
		// color tv system 3
		ItemName = MenuConfigManager.COLOR_SYSTEM;
		channelScanDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_color_system),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getScanDefault(ItemName), colorSystem,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.SCANCHANNELSOPTIONVIEW));
	}

	/**
	 * load data for TV
	 */
	public void loadDataTV() {
		// if (mTVDataGroup != null && mTVDataGroup.size() > 0) {
		// return;
		// } else {
		NavIntegration mNavIntegration = NavIntegration.getInstance(mContext);
		mts = mContext.getResources().getStringArray(
				R.array.menu_tv_audio_channel_array);
		tvSystem = mContext.getResources().getStringArray(
				R.array.menu_tv_tv_system_array);
		colorSystem = mContext.getResources().getStringArray(
				R.array.menu_tv_color_system_array);
		audioLanuage = mContext.getResources().getStringArray(
				R.array.menu_tv_audio_language_array);
		audioLanuage2nd = mContext.getResources().getStringArray(
				R.array.menu_tv_audio_language_array);

		String[] Freeze_Change_Ch = mContext.getResources().getStringArray(
				R.array.menu_tv_freeze_channel_array);
		String[] Tshift_Option = mContext.getResources().getStringArray(
                R.array.menu_tv_tshift_config_array);
		String[] scanMode = mContext.getResources().getStringArray(
				R.array.menu_tv_scan_mode_array);
		String[] tunerModeStr = mContext.getResources().getStringArray(
				R.array.menu_tv_tuner_mode_array);

		mTVDataGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.TUNER_MODE;
		DataItem tvTunerMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_tuner_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,mConfigManager.getDefault(MenuConfigManager.TUNER_MODE), tunerModeStr,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		// audio channel 1
		ItemName = MenuConfigManager.TV_MTS_MODE;
		DataItem tvFirstVoice = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_mts),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mts,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		// audio language
		ItemName = MenuConfigManager.TV_AUDIO_LANGUAGE;
		DataItem tvFirstLanguage = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_audio_language),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), audioLanuage,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		// audio language 2nd
		ItemName = MenuConfigManager.TV_AUDIO_LANGUAGE_2;
		DataItem tvSecondLanguage = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_audio_language2nd),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), audioLanuage2nd,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		// channel 1
		ItemName = MenuConfigManager.TV_CHANNEL;
		tvChannel = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_channels),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		tvChannel.mSubChildGroup = new ArrayList<DataItem>();

		// channel scan 2
		DataItem channelScan = new DataItem(MenuConfigManager.TV_CHANNEL_SCAN,
				mContext.getString(R.string.menu_tv_channel_scan),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		channelScan.mSubChildGroup = new ArrayList<DataItem>();
		channelScan.setmParentGroup(tvChannel.mSubChildGroup);
		channelScan.setmSubChildGroup(channelScanDataGroup);

		// update scan 2
		ItemName = MenuConfigManager.TV_UPDATE_SCAN;
		DataItem updateScan = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_update_scan),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		updateScan.mSubChildGroup = new ArrayList<DataItem>();

		updateScan.setmParentGroup(tvChannel.mSubChildGroup);
		updateScan.setmSubChildGroup(channelScanDataGroup);
		// analog manual scan 2
		ItemName = MenuConfigManager.TV_ANALOG_SCAN;
		DataItem analogScan = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_analog_manual_scan),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
//		analogScan.mSubChildGroup = new ArrayList<DataItem>();
		mAnalogScanSubChildGroup = new ArrayList<DataItem>();
		analogScan.setmSubChildGroup(mAnalogScanSubChildGroup);
		analogScan.setmParentGroup(tvChannel.mSubChildGroup);

		loadAnaLogChild();
//		// start frequency 3
//		int startFrq = 0;
//		tvCh = chSel.getCurrentChannel();
//		if ((tvCh != null) /*&& (tvCh.isValid())*/) {
//			startFrq = tvCh.getFreq() / 1000000;
//		} else {
//			startFrq = 44;
//		}
//		MenuMain.setFreFrom(startFrq * 1000000);
//		ItemName = MenuConfigManager.TV_CHANNEL_START_FREQUENCY;
//		analogScan.mSubChildGroup.add(new DataItem(ItemName, mContext
//				.getString(R.string.menu_tv_start_frequency), 44, 865,
//				startFrq, null, MenuConfigManager.STEP_VALUE,
//				DataItem.DataType.NUMVIEW));
//		// end frequency 3
//		int endFrq = 865;
//		MenuMain.setFreTo(endFrq * 1000000);
//		ItemName = MenuConfigManager.TV_CHANNEL_END_FREQUENCY;
//		analogScan.mSubChildGroup.add(new DataItem(ItemName, mContext
//				.getString(R.string.menu_tv_end_frequency), 44, 865, endFrq,
//				null, MenuConfigManager.STEP_VALUE, DataItem.DataType.NUMVIEW));
//		// begin scan 3
//		ItemName = MenuConfigManager.TV_CHANNEL_STARTSCAN;
//		DataItem beginScan = new DataItem(ItemName, mContext
//				.getString(R.string.menu_tv_status_value),
//				MenuConfigManager.INVALID_VALUE,
//				MenuConfigManager.INVALID_VALUE,
//				MenuConfigManager.INVALID_VALUE, null,
//				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
//		beginScan.mSubChildGroup = new ArrayList<DataItem>();
//		analogScan.mSubChildGroup.add(beginScan);
//		beginScan.setmParentGroup(tvChannel.mSubChildGroup);

		// single RF scan
		mRFScanDataGroup  = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.TV_SINGLE_RF_SCAN;
		DataItem RFScan = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_single_rf_scan),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
			RFScan.mSubChildGroup = mRFScanDataGroup;
		RFScan.setmParentGroup(tvChannel.mSubChildGroup);
        loadDTMBSingleRF();

		// channel edit 2
		ItemName = MenuConfigManager.TV_CHANNEL_EDIT;
		channelEdit = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_channel_edit),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		channelEdit.setmParentGroup(tvChannel.mSubChildGroup);
		channelEdit.setmSubChildGroup(channelDataGroup);

		DataItem cleanList = new DataItem(MenuConfigManager.TV_CHANNEL_CLEAR,
				mContext.getString(R.string.menu_tv_clear_channel_list),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		cleanList.mSubChildGroup = new ArrayList<DataItem>();
		cleanList.setmParentGroup(tvChannel.mSubChildGroup);

		tvChannel.mSubChildGroup.add(channelScan);
		if (!NavIntegration.getInstance(mContext).isCurrentSourceDTV()) {
		        tvChannel.mSubChildGroup.add(updateScan);
			tvChannel.mSubChildGroup.add(analogScan);
		} else {
			tvChannel.mSubChildGroup.add(RFScan);
		}
		tvChannel.mSubChildGroup.add(channelEdit);
		tvChannel.mSubChildGroup.add(cleanList);
		// freeze channel 1
		ItemName = MenuConfigManager.TV_FREEZE_CHANNEL;
		DataItem freezeChannel = new DataItem(ItemName, mContext
				.getString(R.string.menu_tv_freeze_channel),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), Freeze_Change_Ch,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		if(mNavIntegration.isCurrentSourceDTV() || !CrossRoad.isOnlyCableAtv ) {
			mTVDataGroup.add(tvTunerMode);
			tvTunerMode.setmParentGroup(mTVDataGroup);
		}
		
		if (mNavIntegration.isCurrentSourceTv()) {
			if (mNavIntegration.isCurrentSourceDTV()) {
				mTVDataGroup.add(tvFirstLanguage);
				mTVDataGroup.add(tvSecondLanguage);
			} else {
				mTVDataGroup.add(tvFirstVoice);
			}
		}
		mTVDataGroup.add(tvChannel);
		tvChannel.setmParentGroup(mTVDataGroup);
		if (mNavIntegration.isCurrentSourceTv()
				&& !mNavIntegration.isCurrentSourceDTV()) {
			mTVDataGroup.add(freezeChannel);
		}
		// add for time/shift.@2013.01.10
		ItemName = MenuConfigManager.DTV_TSHIFT_OPTION;
		DataItem tshiftoption = new DataItem(ItemName,
				mContext.getString(R.string.menu_tv_tshift_config),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				mConfigManager.getValueFromPrefer(ItemName), Tshift_Option,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		ItemName = MenuConfigManager.DTV_DEVICE_INFO;
		DataItem deviceInfo = new DataItem(ItemName,
				mContext.getString(R.string.menu_tv_device_info),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);

		if (mNavIntegration.isCurrentSourceDTV()) {
			
			//Disable tshift feature on CN.
			if (TimeShiftManager.enableTshiftModule()) {
				mTVDataGroup.add(tshiftoption);
			}
			
			mTVDataGroup.add(deviceInfo);
		}
	}

	/**
	 * load data for setup
	 */
	public void loadDataSetup() {
		// if (mSetUpDataGroup != null && mSetUpDataGroup.size() > 0) {
		// return;
		// } else {
		NavIntegration mNavIntegration = NavIntegration.getInstance(mContext);
		OSDLanguage mOSDLanguage = new OSDLanguage(mContext);
		haveScreenMode = false;
		OSDLanguage = mContext.getResources().getStringArray(
				R.array.menu_setup_language_array);
		
		dTLanguage = mContext.getResources().getStringArray(
				R.array.menu_setup_digital_teletext_language_array);
		
		dPLanguage = mContext.getResources().getStringArray(
				R.array.menu_setup_decoding_page_language_array);
		
		tPLevel = mContext.getResources().getStringArray(
				R.array.menu_setup_ttx_presentation_level_array);
		
		DPMS = mContext.getResources().getStringArray(
				R.array.menu_setup_dpms_array);
		blueMute = mContext.getResources().getStringArray(
				R.array.menu_setup_blue_mute_array);
		fastBoot = mContext.getResources().getStringArray(
				R.array.menu_setup_fast_boot_array);
		MSI = mContext.getResources().getStringArray(R.array.menu_setup_msi_array);
		String[] screenMode = mContext.getResources().getStringArray(
				R.array.screen_mode_array);
		String[] poweronMusic = mContext.getResources().getStringArray(
				R.array.menu_setup_poweron_music_array);
		String[] captureLogSelect = mContext.getResources().getStringArray(
				R.array.menu_setup_capturelogo_array);
		poweronChannel = mContext.getResources().getStringArray(
				R.array.menu_setup_power_on_channel_mode_array);
		String[] optionTiemZone = mContext.getResources().getStringArray(
				R.array.menu_setup_timezone_array);
		String[] optionTiemSleep = mContext.getResources().getStringArray(
				R.array.menu_setup_sleep_timer_array);
		String[] optionAutoSleep = mContext.getResources().getStringArray(
				R.array.menu_setup_auto_sleep_array);

		mSetUpDataGroup = new ArrayList<DataItem>();

		ItemName = MenuConfigManager.OSD_LANGUAGE;
		if(mOSDLanguage.getOSDLanguageIsChinese()){
		mSetUpDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_language),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, OSD_LANGUAGE_CHINESE,
					OSDLanguage, MenuConfigManager.STEP_VALUE,
					DataItem.DataType.OPTIONVIEW));
			saveV.saveValue(OSD_LANGUAGE, OSD_LANGUAGE_CHINESE);
		}else{
			mSetUpDataGroup.add(new DataItem(ItemName, mContext
					.getString(R.string.menu_setup_language),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
				OSDLanguage, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW));
		}

		ItemName = MenuConfigManager.SCREEN_MODE;
		DataItem screenModeItem = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_screenmode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), screenMode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		if (/*
			 * !TVInputManager.INPUT_TYPE_AV.equals(TVContent
			 * .getInstance(mContext).getInputManager()
			 * .getTypeFromInputSource(mCurrentSource)) &&
			 * !TVInputManager.INPUT_TYPE_COMPONENT.equals(TVContent
			 * .getInstance(mContext).getInputManager()
			 * .getTypeFromInputSource(mCurrentSource)) &&
			 */
		!mNavIntegration.isCurrentSourceBlocking()
				&& mNavIntegration.iCurrentInputSourceHasSignal()
				&& ((mNavIntegration.isCurrentSourceTv() && chSel.hasSignal()
						&& !mNavIntegration.isCurrentChannelBlocking()
						&& (mNavIntegration.iGetChannelLength() > 0)
						&& !mNavIntegration.isCurrentChannelRadio() && !mNavIntegration
							.isVideoScrambled()) || !mNavIntegration
						.isCurrentSourceTv())) {
			mSetUpDataGroup.add(screenModeItem);
			haveScreenMode = true;
		}
		ItemName = MenuConfigManager.DPMS;
		mSetUpDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_dpms),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName), DPMS,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		ItemName = MenuConfigManager.BLUE_MUTE;
		mSetUpDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_bluemute),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName), blueMute,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		ItemName = MenuConfigManager.POWER_ON_MUSIC;
		mSetUpDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_poweron_music),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName), poweronMusic,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		ItemName = MenuConfigManager.POWER_OFF_MUSIC;
		mSetUpDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_poweroff_music),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName), poweronMusic,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));

		ItemName = MenuConfigManager.CAPTURE_LOGO_SELECT;
		mSetUpDataGroup.add(new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_capturelogo_select),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
				captureLogSelect, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW));
		// Time setup
		setupTime = new DataItem(MenuConfigManager.SETUP_TIME_SETUP, mContext
				.getString(R.string.menu_setup_time_setup),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		setupTime.mSubChildGroup = new ArrayList<DataItem>();

		DataItem timeZone = new DataItem(MenuConfigManager.SETUP_TIME_ZONE,
				mContext.getString(R.string.menu_setup_timezone),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, GetTimeZone.getInstance(
						mContext).getZoneId(), optionTiemZone,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		timeSet = new DataItem(MenuConfigManager.SETUP_TIME_SET, mContext
				.getString(R.string.menu_setup_time),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		timeSet.mSubChildGroup = new ArrayList<DataItem>();

		// sleep timer
		ItemName = MenuConfigManager.SLEEP_TIMER;
		DataItem timeSleep = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_sleep_timer),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
				optionTiemSleep, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);

		// auto sleep
		ItemName = MenuConfigManager.AUTO_SLEEP;
		DataItem timeAutoSleep = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_auto_sleep),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
				optionAutoSleep, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);

		setupTime.mSubChildGroup.add(timeZone);
		setupTime.mSubChildGroup.add(timeSet);
		setupTime.mSubChildGroup.add(timeSleep);
		setupTime.mSubChildGroup.add(timeAutoSleep);

		// common interface
		ItemName = MenuConfigManager.COMMON_INTERFACE;
		DataItem commonInterface = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_common_interface),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		commonInterface.mSubChildGroup = new ArrayList<DataItem>();

		
		
		
		// teletext
		ItemName=MenuConfigManager.SETUP_TELETEXT;
		teletext = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_teletext),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		teletext.mSubChildGroup = new ArrayList<DataItem>();
		
		
		ItemName=MenuConfigManager.SETUP_DIGITAL_TELETEXT_LANGUAGE;
		DataItem dTLanguageItem = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_digital_teletext_language),
				mConfigManager.getMin(ItemName),
				mConfigManager.getMax(ItemName), mConfigManager.getDefault(ItemName),
				dTLanguage, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		dTLanguageItem.setmParentGroup(teletext.mSubChildGroup);
		
		
		ItemName=MenuConfigManager.SETUP_DECODING_PAGE_LANGUAGE;
		DataItem dPLanguageItem = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_decoding_page_language),
				mConfigManager.getMin(ItemName),
				mConfigManager.getMax(ItemName), mConfigManager.getDefault(ItemName),
				dPLanguage, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		dPLanguageItem.setmParentGroup(teletext.mSubChildGroup);
		
		
		ItemName=MenuConfigManager.SETUP_TTX_PRESENTATION_LEVEL;
		DataItem tPLevelItem = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_ttx_presentation_level),
				mConfigManager.getMin(ItemName),
				mConfigManager.getMax(ItemName), mConfigManager.getDefault(ItemName),
				tPLevel, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		tPLevelItem.setmParentGroup(teletext.mSubChildGroup);
		
		
		
		
		
		
		
		
		
		
		// network
		network = new DataItem("Network", mContext
				.getString(R.string.menu_setup_network),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		network.mSubChildGroup = new ArrayList<DataItem>();


		DataItem networkUpdate = new DataItem("upgradeNet", mContext
				.getString(R.string.menu_setup_auto_network_upgrade),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		networkUpdate.mSubChildGroup = new ArrayList<DataItem>();
		networkUpdate.setmParentGroup(network.mSubChildGroup);

		// Application
		application = new DataItem(MenuConfigManager.SETUP_APPLICATION,
				mContext.getString(R.string.menu_setup_application),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		application.mSubChildGroup = new ArrayList<DataItem>();
		application.setmParentGroup(network.mSubChildGroup);

    String[] net_connection = mContext.getResources().getStringArray(
                R.array.menu_setup_network_connection_array);
    DataItem wow = new DataItem(MenuConfigManager.SETUP_WOW, mContext
                .getString(R.string.menu_setup_wow),
                MenuConfigManager.INVALID_VALUE,
                MenuConfigManager.INVALID_VALUE, saveV.readValue(MenuConfigManager.SETUP_WOW), net_connection,
                MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		teletext.mSubChildGroup.add(dTLanguageItem);
		teletext.mSubChildGroup.add(dPLanguageItem);
		teletext.mSubChildGroup.add(tPLevelItem);
		
//		network.mSubChildGroup.add(ConfigItem);
		network.mSubChildGroup.add(networkUpdate);
		network.mSubChildGroup.add(application);
		 WifiManager mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        if (mWifiManager != null) {
            if (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED) {
                MtkLog.d(TAG, "WIFI_STATE_ENABLED true");
                network.mSubChildGroup.add(wow);
            } else {
                MtkLog.d(TAG, "WIFI_STATE_ENABLED false");
            }
        }
		//		if("1".equals(SystemProperties.get("ro.pm.warmboot"))){
		//		ItemName = MenuConfigManager.FAST_BOOT;
		//		mSetUpDataGroup.add(new DataItem(ItemName, mContext
		//				.getString(R.string.menu_setup_fastboot),
		//				MenuConfigManager.INVALID_VALUE,
		//				MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName), fastBoot,
		//				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));
		//		}


		// version info
		versionInfo = new DataItem(MenuConfigManager.VERSION_INFO, mContext
				.getString(R.string.menu_setup_version_info),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		versionInfo.mSubChildGroup = new ArrayList<DataItem>();

		DataItem msiItem = new DataItem(MenuConfigManager.SETUP_MSI,mContext.getString(R.string.menu_setup_msi),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(MenuConfigManager.SETUP_MSI), MSI,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);


		mSetUpDataGroup.add(setupTime);
		setupTime.setmParentGroup(mSetUpDataGroup);
		mSetUpDataGroup.add(commonInterface);
		commonInterface.setmParentGroup(mSetUpDataGroup);
		mSetUpDataGroup.add(teletext);
		teletext.setmParentGroup(mSetUpDataGroup);
		mSetUpDataGroup.add(network);
		network.setmParentGroup(mSetUpDataGroup);
		mSetUpDataGroup.add(versionInfo);
		versionInfo.setmParentGroup(mSetUpDataGroup);
		if("1".equals(SystemProperties.get("mtk.multi_screen_interaction")))
		{
		 	mSetUpDataGroup.add(msiItem);
		}
		msiItem.setmParentGroup(mSetUpDataGroup);
		
		if("1".equals(SystemProperties.get("ro.pm.warmboot"))){
			ItemName = MenuConfigManager.FAST_BOOT;
			mSetUpDataGroup.add(new DataItem(ItemName, mContext
					.getString(R.string.menu_setup_fastboot),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName), fastBoot,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));
		}
		// reset default
		resetDefault = new DataItem(MenuConfigManager.RESET_DEFAULT, mContext
				.getString(R.string.menu_setup_reset_default),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		resetDefault.mSubChildGroup = new ArrayList<DataItem>();
		
		mSetUpDataGroup.add(resetDefault);
		resetDefault.setmParentGroup(mSetUpDataGroup);



		
		// }
	}

	/** load application data */
	public void loadApplicationData() {
		String[] net_connection = mContext.getResources().getStringArray(
				R.array.menu_setup_network_connection_array);
		int initDLNAValue = 0;
		int initNetValue = 0;
			initDLNAValue = saveV.readValue(MenuConfigManager.DLNA);
			initNetValue = saveV.readValue(MenuConfigManager.MY_NET_PLACE);
		DataItem DLNA = new DataItem(MenuConfigManager.DLNA, mContext
				.getString(R.string.menu_setup_dlna),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, initDLNAValue, net_connection,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

		DataItem myNetPlace = new DataItem(MenuConfigManager.MY_NET_PLACE,
				mContext.getString(R.string.menu_setup_my_net_place),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, initNetValue, net_connection,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
//		if (value == 0) {
//			DLNA.isEnable = false;
//			myNetPlace.isEnable = false;
//		} else {
			DLNA.isEnable = true;
			myNetPlace.isEnable = true;
//		}
		application.mSubChildGroup.add(DLNA);
		application.mSubChildGroup.add(myNetPlace);
	}

	/** load time data */
	public void loadTimeData() {
		String[] autosyn = mContext.getResources().getStringArray(
				R.array.menu_setup_auto_synchronization_array);// time zone time
		String[] poweronTimer = mContext.getResources().getStringArray(
				R.array.menu_setup_power_on_timer_array);//
		// time auto synchronization
		DataItem autoSynchronizaiton = new DataItem(
				MenuConfigManager.AUTO_SYNC, mContext
						.getString(R.string.menu_setup_auto_synchronization),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV
						.readValue(MenuConfigManager.AUTO_SYNC), autosyn,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.SWICHOPTIONVIEW);

		DataItem timeSet_date = new DataItem(MenuConfigManager.TIME_DATE,
				mContext.getString(R.string.menu_setup_date),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, 0, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.DATETIMEVIEW);

		timeSet_date.setmDateTimeType(DateTimeView.DATETYPE);

		timeSet_date.setmDateTimeStr("2008/01/01");

		DataItem timeSet_time = new DataItem(MenuConfigManager.TIME_TIME,
				mContext.getString(R.string.menu_setup_time),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, 0, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.DATETIMEVIEW);

		timeSet_time.setmDateTimeType(DateTimeView.TIMETYPE);

		timeSet_time.setmDateTimeStr("00:00:00");
		autoSynchronizaiton.mEffectGroup = new ArrayList<DataItem>();
		autoSynchronizaiton.mEffectGroup.add(timeSet_date);
		autoSynchronizaiton.mEffectGroup.add(timeSet_time);
		autoSynchronizaiton.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
		autoSynchronizaiton.mSwitchHashMap.put(0, new Boolean[] { true, true });
		autoSynchronizaiton.mSwitchHashMap.put(1,
				new Boolean[] { false, false });
		autoSynchronizaiton.mSwitchHashMap.put(2,
				new Boolean[] { false, false });
		if (saveV.readValue(MenuConfigManager.AUTO_SYNC) == 0) {
			timeSet_date.setEnable(true);
			timeSet_time.setEnable(true);
		} else {
			timeSet_date.setEnable(false);
			timeSet_time.setEnable(false);
		}

		// power on timer
		ItemName = MenuConfigManager.POWER_ON_TIMER;
		DataItem timeSet_powerOnTimer = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_power_on_timer),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
				poweronTimer, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.SWICHOPTIONVIEW);
		// timer 1
		ItemName = MenuConfigManager.TIMER1;
		DataItem timer1 = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_timer),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.DATETIMEVIEW);

		timer1.setmDateTimeType(DateTimeView.TIMETYPE);

		timer1.setmDateTimeStr(saveV.readStrValue(MenuConfigManager.TIMER1));
		// power on channel
		powerOnChannel = new DataItem("PowerOnCh", mContext
				.getString(R.string.menu_setup_power_on_channel),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);

		timeSet_powerOnTimer.mEffectGroup = new ArrayList<DataItem>();
		timeSet_powerOnTimer.mEffectGroup.add(timer1);
		timeSet_powerOnTimer.mEffectGroup.add(powerOnChannel);

		timeSet_powerOnTimer.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
		timeSet_powerOnTimer.mSwitchHashMap.put(0,
				new Boolean[] { false, false });
		timeSet_powerOnTimer.mSwitchHashMap
				.put(1, new Boolean[] { true, true });
		timeSet_powerOnTimer.mSwitchHashMap
				.put(2, new Boolean[] { true, true });

		if (saveV.readValue(MenuConfigManager.POWER_ON_TIMER) == 0) {
			timer1.setEnable(false);
			powerOnChannel.setEnable(false);
		} else {
			timer1.setEnable(true);
			powerOnChannel.setEnable(true);
		}

		powerOnChannel.mSubChildGroup = new ArrayList<DataItem>();
		powerOnChannel.setmParentGroup(timeSet.mSubChildGroup);

		// power off timer
		DataItem timeSet_powerOffTimer = new DataItem(
				MenuConfigManager.POWER_OFF_TIMER, mContext
						.getString(R.string.menu_setup_power_off_timer),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV
						.readValue(MenuConfigManager.POWER_OFF_TIMER),
				poweronTimer, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.SWICHOPTIONVIEW);
		// timer 2
		ItemName = MenuConfigManager.TIMER2;
		DataItem timer2 = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_timer),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.DATETIMEVIEW);

		timer2.setmDateTimeType(DateTimeView.TIMETYPE);

		timer2.setmDateTimeStr(saveV.readStrValue(MenuConfigManager.TIMER2));
		timeSet_powerOffTimer.mEffectGroup = new ArrayList<DataItem>();
		timeSet_powerOffTimer.mEffectGroup.add(timer2);

		timeSet_powerOffTimer.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
		timeSet_powerOffTimer.mSwitchHashMap.put(0, new Boolean[] { false });
		timeSet_powerOffTimer.mSwitchHashMap.put(1, new Boolean[] { true });
		timeSet_powerOffTimer.mSwitchHashMap.put(2, new Boolean[] { true });
		if (saveV.readValue(MenuConfigManager.POWER_OFF_TIMER) == 0) {
			timer2.setEnable(false);
		} else {
			timer2.setEnable(true);
		}

		/*timeSet.mSubChildGroup.add(autoSynchronizaiton);
		timeSet.mSubChildGroup.add(timeSet_date);
		timeSet.mSubChildGroup.add(timeSet_time);*/
		timeSet.mSubChildGroup.add(timeSet_powerOnTimer);
		timeSet.mSubChildGroup.add(timer1);
		timeSet.mSubChildGroup.add(powerOnChannel);
		timeSet.mSubChildGroup.add(timeSet_powerOffTimer);
		timeSet.mSubChildGroup.add(timer2);
		timeSet.setmParentGroup(setupTime.mSubChildGroup);
	}

	
	/**
	 * load channel data
	 */
	public void loadChannelData() {
		for (int i = 0; i < chNum; i++) {
			TVChannel ch = null;
			if(null != ch_list){
				ch = ch_list.get(i);
			}
			// channelList 3
			DataItem channelList = new DataItem(
					MenuConfigManager.TV_CHANNEL_EDIT_LIST, " ",
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, channelInfo.get(i),
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.CHANNELLISTVIEW);
			channelList.mSubChildGroup = new ArrayList<DataItem>();
			// channel number 4
			DataItem channelNum = new DataItem(MenuConfigManager.TV_CHANNEL_NO,
					mContext.getString(R.string.menu_tv_channel_no),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, 0, channelInfo.get(i),
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.TEXTCOMMVIEW);
			channelList.mSubChildGroup.add(channelNum);
			channelNum.mSubChildGroup = new ArrayList<DataItem>();
			channelNum.setmParentGroup(channelList.mSubChildGroup);
			// channel name 4
			DataItem channelName = new DataItem(
					MenuConfigManager.TV_CHANNEL_NAME, mContext
							.getString(R.string.menu_tv_channel_name),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, 4, channelInfo.get(i),
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.TEXTCOMMVIEW);
			channelList.mSubChildGroup.add(channelName);
			channelName.mSubChildGroup = new ArrayList<DataItem>();
			channelName.setmParentGroup(channelList.mSubChildGroup);

			// channel frequency 4
			DataItem channelFre = new DataItem(MenuConfigManager.TV_FREQ,
					mContext.getString(R.string.menu_tv_freq),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, 2, channelInfo.get(i),
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.TEXTCOMMVIEW);
			channelList.mSubChildGroup.add(channelFre);
			channelFre.mSubChildGroup = new ArrayList<DataItem>();
			channelFre.setmParentGroup(channelList.mSubChildGroup);

			// color system 4
			ItemName = MenuConfigManager.TV_CHANNEL_COLOR_SYSTEM;
			int tvColorNum = 0;
			if (null != ch && ch.getColorSystemOption() != null) {
				tvColorNum = ch.getColorSystemOption().get();
			} else {
				tvColorNum = 0;
			}
			if (tvColorNum < 0
					|| tvColorNum + 1 > mContext.getResources().getStringArray(
							R.array.menu_tv_color_system_array).length) {
				MtkLog.w("yinxin", "Warning: colorNum changed");
				tvColorNum = 0;
			}
			DataItem colorSystem = new DataItem(ItemName, mContext
					.getString(R.string.menu_tv_color_system),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, tvColorNum, mContext
							.getResources().getStringArray(
									R.array.menu_tv_color_system_array),
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			// sound system 4
			ItemName = MenuConfigManager.TV_SOUND_SYSTEM;
			int tvSysNum = 0;
			int minNum = 1;
			int maxNum = 8;
			if (null != ch && ch.getTvSystemOption() != null) {
				tvSysNum = ch.getTvSystemOption().get();
				minNum = ch.getTvSystemOption().getMin();
				maxNum = ch.getTvSystemOption().getMax();
				MtkLog.i(TAG, " [FROM TVCM] Value: " + tvSysNum + "[" + minNum
						+ "," + maxNum + "]");
			} else {
				tvSysNum = 1;
			}

			MtkLog.i(TAG, " [FROM TKUI] Sound System Num: "
					+ mContext.getResources().getStringArray(
							R.array.menu_tv_sound_system_array).length);

			if (tvSysNum < minNum || tvSysNum > maxNum) {
				tvSysNum = minNum;
			}
			DataItem soundSystem = new DataItem(ItemName, mContext
					.getString(R.string.menu_tv_sound_system),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, tvSysNum - minNum,
					mContext.getResources().getStringArray(
							R.array.menu_tv_sound_system_array),
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			// aft 4
			int aft=0;
			try {
				if(null != ch){
				aft = ch.isAutoFinetune() ? 1 : 0;
				}	
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
			ItemName = MenuConfigManager.TV_AUTO_FINETUNE;
			DataItem autoFineTune = new DataItem(ItemName, mContext
					.getString(R.string.menu_tv_auto_finetune),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, aft, mContext
							.getResources().getStringArray(
									R.array.menu_tv_fine_tune_array),
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);

			// fine tune 4
			DataItem finetuneItem = new DataItem(MenuConfigManager.TV_FINETUNE,
					mContext.getString(R.string.menu_tv_finetune),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);

			channelList.mSubChildGroup.add(colorSystem);
			channelList.mSubChildGroup.add(soundSystem);
			channelList.mSubChildGroup.add(autoFineTune);
			channelList.mSubChildGroup.add(finetuneItem);
			finetuneItem.mSubChildGroup = new ArrayList<DataItem>();
			finetuneItem.setmParentGroup(channelList.mSubChildGroup);

			// skip 4
			int skip = 0 ;
			if(null != ch){
				skip = ch.isSkip() ? 1 : 0;
			}
			if (null != ch && ch.isSkip()) {
				channelList.mBrightBackGroundFlag = true;
			} else {
				channelList.mBrightBackGroundFlag = false;
			}
			ItemName = MenuConfigManager.TV_SKIP;
			channelList.mSubChildGroup
					.add(new DataItem(ItemName, mContext
							.getString(R.string.menu_tv_skip),
							MenuConfigManager.INVALID_VALUE,
							MenuConfigManager.INVALID_VALUE, skip, mContext
									.getResources().getStringArray(
											R.array.menu_tv_skip_ch_array),
							MenuConfigManager.STEP_VALUE,
							DataItem.DataType.OPTIONVIEW));
			// store 4
			DataItem storeItem = new DataItem(MenuConfigManager.TV_STORE,
					mContext.getString(R.string.menu_tv_store),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			channelList.mSubChildGroup.add(storeItem);
			storeItem.mSubChildGroup = new ArrayList<DataItem>();
			storeItem.setmParentGroup(channelList.mSubChildGroup);

			if (null != ch && ch.getTvSystemOption() == null) {
				channelFre.setEnable(false);
				colorSystem.setEnable(false);
				soundSystem.setEnable(false);
				autoFineTune.setEnable(false);
				finetuneItem.setEnable(false);
			}
			channelDataGroup.add(channelList);
			channelList.setmParentGroup(channelDataGroup);
		}
	}

	/**
	 * Load Data For Parental Control
	 */
	public void loadParentalData() {
		// if (mParentalDataGroup != null && mParentalDataGroup.size() > 0) {
		// return;
		// } else {
		mParentalDataGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.PARENTAL_PASSWORD;
		parentalPwd = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_password),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PASSWORDVIEW);
		parentalPwd.mSubChildGroup = new ArrayList<DataItem>();
		mParentalDataGroup.add(parentalPwd);// add parentalPwd item first
		// level
		parentalPwd.setmParentGroup(mParentalDataGroup);
		ItemName = MenuConfigManager.PARENTAL_CHANNEL_BLOCK;
		channelBlock = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_channel_block),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		// 7.12
		ItemName = MenuConfigManager.PARENTAL_INPUT_BLOCK;
		inputBlock = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_input_block),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		ItemName = MenuConfigManager.PARENTAL_CHANGE_PASSWORD;
		DataItem changePassword = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_change_password),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		ItemName = MenuConfigManager.PARENTAL_CLEAN_ALL;
		DataItem cleanAll = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_clean_all),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		parentalPwd.mSubChildGroup.add(channelBlock);
		parentalPwd.mSubChildGroup.add(inputBlock);
		parentalPwd.mSubChildGroup.add(changePassword);
		parentalPwd.mSubChildGroup.add(cleanAll);
		channelBlock.setmParentGroup(parentalPwd.mSubChildGroup);
		inputBlock.setmParentGroup(parentalPwd.mSubChildGroup);
		changePassword.setmParentGroup(parentalPwd.mSubChildGroup);
		cleanAll.setmParentGroup(parentalPwd.mSubChildGroup);

		channelBlock.mSubChildGroup = new ArrayList<DataItem>();
		inputBlock.mSubChildGroup = new ArrayList<DataItem>();
		changePassword.mSubChildGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.PARENTAL_PASSWORD_NEW;
		DataItem parentPwdNew = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_password_new),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PASSWORDVIEW);
		ItemName = MenuConfigManager.PARENTAL_PASSWORD_NEW_RE;
		DataItem parentPwdNewRe = new DataItem(ItemName, mContext
				.getString(R.string.menu_parental_password_new_re),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PASSWORDVIEW);
		changePassword.mSubChildGroup.add(parentPwdNew);
		changePassword.mSubChildGroup.add(parentPwdNewRe);

		cleanAll.mSubChildGroup = new ArrayList<DataItem>();
		// }
	}

	/**
	 * Load block channel data
	 */
	public String[] loadParentalSourceData() {
		TVInputManager tvManager = mTV.getInputManager();
		String[] sourceNames = tvManager.getInputSourceArray();
		String[] sourceArray = null;
		if(null != sourceNames){
		for (int i = 0; i < sourceNames.length; i++) {
			sourceArray = new String[2];
			sourceArray[0] = i + 1 + "";
			sourceArray[1] = sourceNames[i];
			DataItem sourceList = new DataItem(
					MenuConfigManager.PARENTAL_INPUT_BLOCK_SOURCE, " ",
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, sourceArray,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.TVSOURCEVIEW);
			inputBlock.mSubChildGroup.add(sourceList);
			inputBlockOnDataGroup.add(sourceList);
			sourceList.setmParentGroup(inputBlock.mSubChildGroup);
			blockSourceName = sourceNames[0];
			blockSourceName3D = sourceNames[5];
			}
		}
		return sourceNames;
	}

	/**
	 * Load block channel data
	 */
	public void loadParentalChannelData() {
		for (int i = 0; i < chNum; i++) {
			DataItem channelList = new DataItem(
					MenuConfigManager.PARENTAL_CHANNEL_BLOCK_CHANNELLIST, " ",
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, channelInfo.get(i),
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.CHANNELPOWERONVIEW);

			channelBlock.mSubChildGroup.add(channelList);
			channelBlockOnDataGroup.add(channelList);
			channelList.setmParentGroup(channelBlock.mSubChildGroup);
		}
		if (chNum == 0) {
			channelBlock.isEnable = false;
		} else {
			channelBlock.isEnable = true;
		}

	}

	/**
	 * load data for factory menu .//press 0 four times to enter
	 */

	public void loadFactoryData() {
		// if (mFactoryDataGroup != null && mFactoryDataGroup.size() > 0) {
		// return;
		// } else {

		mContext.getResources().getStringArray(
				R.array.menu_factory_autovedio_array);
		mContext.getResources().getStringArray(
				R.array.menu_factory_autovedio_array);
		colorTemperature2 = mContext.getResources().getStringArray(
				R.array.menu_video_color_temperature2_array);
		mFactoryDataGroup = new ArrayList<DataItem>();

		String[] mFaDolbyBanner = mContext.getResources().getStringArray(
				R.array.menu_factory_audio_dolbybanner_array);

		String[] mFaCompression = mContext.getResources().getStringArray(
				R.array.menu_factory_audio_compression_array);

		String[] mFaCompressionFactor = mContext.getResources().getStringArray(
				R.array.menu_factory_audio_compressionfactor_array);
		String[] mFvEventForm = mContext.getResources().getStringArray(
				R.array.menu_factory_setup_eventform);
		String[] mFvBurningMode = mContext.getResources().getStringArray(
				R.array.menu_factory_setup_burning_mode);

		String[] mFaSystemTest = mContext.getResources().getStringArray(
				R.array.menu_factory_audio_system_test_array);

		ItemName = MenuConfigManager.FACTORY_VIDEO;
		DataItem factoyVideo = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		mFactoryDataGroup.add(factoyVideo);
		factoyVideo.setmParentGroup(mFactoryDataGroup);
		ItemName = MenuConfigManager.FACTORY_AUDIO;
		DataItem factoyAudio = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD); //
		mFactoryDataGroup.add(factoyAudio);
		factoyAudio.setmParentGroup(mFactoryDataGroup);

		// when source is tv,show tv
		// and when control of parent,show tv at have not screen
		EditChannel mEditChannel = EditChannel.getInstance(mContext);
		if (TVInputManager.INPUT_TYPE_TV.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))
				&& !mEditChannel.isInputBlock(getBlockSourceName())) {
			ItemName = MenuConfigManager.FACTORY_TV;
			factoyTV = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_TV),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			mFactoryDataGroup.add(factoyTV);
			factoyTV.mSubChildGroup = new ArrayList<DataItem>();
			// TV
			ItemName = MenuConfigManager.FACTORY_TV_TUNER_DIAGNOSTIC;
			fvTunerDiagnostic = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_TV_tunerdiag),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			factoyTV.mSubChildGroup.add(fvTunerDiagnostic);
			fvTunerDiagnostic.setmParentGroup(factoyTV.mSubChildGroup);
			fvTunerDiagnostic.mSubChildGroup = new ArrayList<DataItem>();
			factoyTV.setmParentGroup(mFactoryDataGroup);
		}

		ItemName = MenuConfigManager.FACTORY_SETUP;
		DataItem factoySetup = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		mFactoryDataGroup.add(factoySetup);
		factoySetup.setmParentGroup(mFactoryDataGroup);
		ItemName = MenuConfigManager.FACTORY_PRESET_CH;
		DataItem factoyPresetCh = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_preset),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		mFactoryDataGroup.add(factoyPresetCh);
		factoyPresetCh.setmParentGroup(mFactoryDataGroup);

		addVideoFactorySub(factoyVideo);

		/**
		 * factory_audio
		 */
		factoyAudio.mSubChildGroup = new ArrayList<DataItem>();

		// Dolby Banner
		ItemName = MenuConfigManager.FA_DOLBYBANNER;
		DataItem faDolbyBanner = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_dolby_banner),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mFaDolbyBanner,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		factoyAudio.mSubChildGroup.add(faDolbyBanner);

		// Compression
		ItemName = MenuConfigManager.FA_COMPRESSION;
		DataItem faCompression = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_compression),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mFaCompression,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		factoyAudio.mSubChildGroup.add(faCompression);

		// CompressionFactor
		ItemName = MenuConfigManager.FA_COMPRESSIONFACTOR;
		DataItem faCompressionFactor = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_compression_factor),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName),
				mFaCompressionFactor, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.OPTIONVIEW);
		factoyAudio.mSubChildGroup.add(faCompressionFactor);

		/**
		 *A2 system
		 */
		ItemName = MenuConfigManager.FA_A2SYSTEM;
		DataItem faA2System = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_system),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoyAudio.mSubChildGroup.add(faA2System);
		faA2System.setmParentGroup(factoyAudio.mSubChildGroup);
		faA2System.mSubChildGroup = new ArrayList<DataItem>();
		// set no available of temporary
		// faA2System.isEnable = false;

		// numbers of check
		ItemName = MenuConfigManager.FAA2_NUMBERSOFCHECK;
		DataItem faA2Check = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_numbersofcheck),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2Check);
		faA2Check.setmParentGroup(faA2System.mSubChildGroup);

		// numbers of double
		ItemName = MenuConfigManager.FAA2_NUMBERSOFDOUBLE;
		DataItem faA2Double = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_numbersofdouble),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2Double);
		faA2Double.setmParentGroup(faA2System.mSubChildGroup);

		// mono weight
		ItemName = MenuConfigManager.FAA2_MONOWEIGHT;
		DataItem faA2Mono = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_monoweight),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2Mono);
		faA2Mono.setmParentGroup(faA2System.mSubChildGroup);

		// stereo weight
		ItemName = MenuConfigManager.FAA2_STEREOWEIGHT;
		DataItem faA2Stereo = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_stereoweight),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2Stereo);
		faA2Stereo.setmParentGroup(faA2System.mSubChildGroup);

		// dual weight
		ItemName = MenuConfigManager.FAA2_DUALWEIGHT;
		DataItem faA2Dual = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_dualweight),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2Dual);
		faA2Dual.setmParentGroup(faA2System.mSubChildGroup);

		// HIGH DEVIATION MODE
		ItemName = MenuConfigManager.FAA2_HIGHDEVIATIONMODE;
		DataItem faA2High = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_highdeviationmode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, DEFAULTVALUE, mFaSystemTest,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYOPTIONVIEW);
		faA2System.mSubChildGroup.add(faA2High);
		faA2High.setmParentGroup(faA2System.mSubChildGroup);

		// CARRIER SHIFT FUNCTION
		ItemName = MenuConfigManager.FAA2_CARRIERSHIFTFUNCTION;
		DataItem faA2CarrierShift = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_a2_carriershiftfunction),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, DEFAULTVALUE, mFaSystemTest,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYOPTIONVIEW);
		faA2System.mSubChildGroup.add(faA2CarrierShift);
		faA2CarrierShift.setmParentGroup(faA2System.mSubChildGroup);

		// FM CARRIER MUTE MODE
		ItemName = MenuConfigManager.FAA2_FMCARRIERMUTEMODE;
		DataItem faA2FMMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_fmcarriermutemode),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2FMMode);
		faA2FMMode.setmParentGroup(faA2System.mSubChildGroup);

		// FM CARRIER MUTE THRESHOLD HIGH
		ItemName = MenuConfigManager.FAA2_FMCARRIERMUTETHRESHOLDHIGH;
		DataItem faA2FMHigh = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_a2_fmcarriermutethreshold_high),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2FMHigh);
		faA2FMHigh.setmParentGroup(faA2System.mSubChildGroup);

		// FM CARRIER MUTE THRESHOLD LOW
		ItemName = MenuConfigManager.FAA2_FMCARRIERMUTETHRESHOLDLOW;
		DataItem faA2FMLow = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_a2_fmcarriermutethreshold_low),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2FMLow);
		faA2FMLow.setmParentGroup(faA2System.mSubChildGroup);

		// FINE TUNE VOLUME
		ItemName = MenuConfigManager.FAA2_FINETUNEVOLUME;
		DataItem faA2Volume = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_a2_finetunevolume),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faA2System.mSubChildGroup.add(faA2Volume);
		faA2Volume.setmParentGroup(faA2System.mSubChildGroup);

		/**
		 *PAL system
		 */
		ItemName = MenuConfigManager.FA_PALSYSTEM;
		DataItem faPALSystem = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_system),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoyAudio.mSubChildGroup.add(faPALSystem);
		faPALSystem.setmParentGroup(factoyAudio.mSubChildGroup);
		faPALSystem.mSubChildGroup = new ArrayList<DataItem>();
		// set no available of temporary
		// faPALSystem.isEnable = false;

		// CORRECT THRESHOLD
		ItemName = MenuConfigManager.FAPAL_CORRECTTHRESHOLD;
		DataItem faPALCorrect = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_correctthreshold),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALCorrect);
		faPALCorrect.setmParentGroup(faPALSystem.mSubChildGroup);

		// TOTAL SYNC LOOP
		ItemName = MenuConfigManager.FAPAL_TOTALSYNCLOOP;
		DataItem faPALTotal = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_totalsyncloop),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALTotal);
		faPALTotal.setmParentGroup(faPALSystem.mSubChildGroup);

		// ERROR THRESHOLD
		ItemName = MenuConfigManager.FAPAL_ERRORTHRESHOLD;
		DataItem faPALError = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_errorthreshold),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALError);
		faPALError.setmParentGroup(faPALSystem.mSubChildGroup);

		// PARITY ERRORT HRESHOLD
		ItemName = MenuConfigManager.FAPAL_PARITYERRORTHRESHOLD;
		DataItem faPALParity = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_parityerrorthreshold),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALParity);
		faPALParity.setmParentGroup(faPALSystem.mSubChildGroup);

		// EVERY NUMBER FRAMES
		ItemName = MenuConfigManager.FAPAL_EVERYNUMBERFRAMES;
		DataItem faPALEvery = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_everynumberframes),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALEvery);
		faPALEvery.setmParentGroup(faPALSystem.mSubChildGroup);

		// HIGH DEVIATION MODE
		ItemName = MenuConfigManager.FAPAL_HIGHDEVIATIONMODE;
		DataItem faPALHigh = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_highdeviationmode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, DEFAULTVALUE, mFaSystemTest,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYOPTIONVIEW);
		faPALSystem.mSubChildGroup.add(faPALHigh);
		faPALHigh.setmParentGroup(faPALSystem.mSubChildGroup);

		// AM CARRIER MUTE MODE
		ItemName = MenuConfigManager.FAPAL_AMCARRIERMUTEMODE;
		DataItem faPALAmMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_amcarriermutemode),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALAmMode);
		faPALAmMode.setmParentGroup(faPALSystem.mSubChildGroup);

		// AM CARRIER MUTE THRESHOLD HIGH
		ItemName = MenuConfigManager.FAPAL_AMCARRIERMUTETHRESHOLDHIGH;
		DataItem faPALAmHigh = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_amcarriermutethresholdhigh),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALAmHigh);
		faPALAmHigh.setmParentGroup(faPALSystem.mSubChildGroup);

		// AM CARRIER MUTE THRESHOLD LOW
		ItemName = MenuConfigManager.FAPAL_AMCARRIERMUTETHRESHOLDLOW;
		DataItem faPALAmLow = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_amcarriermutethresholdlow),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALAmLow);
		faPALAmLow.setmParentGroup(faPALSystem.mSubChildGroup);

		// CARRIER SHIFT FUNCTION
		ItemName = MenuConfigManager.FAPAL_CARRIERSHIFTFUNCTION;
		DataItem faPALCarrier = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_carriershiftfunction),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, DEFAULTVALUE, mFaSystemTest,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYOPTIONVIEW);
		faPALSystem.mSubChildGroup.add(faPALCarrier);
		faPALCarrier.setmParentGroup(faPALSystem.mSubChildGroup);

		// FM CARRIER MUTE MODE
		ItemName = MenuConfigManager.FAPAL_FMCARRIERMUTEMODE;
		DataItem faPALFmMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_fmcarriermutemode),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALFmMode);
		faPALFmMode.setmParentGroup(faPALSystem.mSubChildGroup);

		// FM CARRIER MUTE THRESHOLD HIGH
		ItemName = MenuConfigManager.FAPAL_FMCARRIERMUTETHRESHOLDHIGH;
		DataItem faPALFmHigh = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_fmcarriermutethresholdhigh),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALFmHigh);
		faPALFmHigh.setmParentGroup(faPALSystem.mSubChildGroup);

		// FM CARRIER MUTE THRESHOLD LOW
		ItemName = MenuConfigManager.FAPAL_FMCARRIERMUTETHRESHOLDLOW;
		DataItem faPALFmLow = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_fmcarriermutethresholdlow),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALFmLow);
		faPALFmLow.setmParentGroup(faPALSystem.mSubChildGroup);

		// PAL FINE TUNE VOLUME
		ItemName = MenuConfigManager.FAPAL_PALFINETUNEVOLUME;
		DataItem faPALVolume = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_palfinetunevolume),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALVolume);
		faPALVolume.setmParentGroup(faPALSystem.mSubChildGroup);

		// AM FINE TUNE VOLUME
		ItemName = MenuConfigManager.FAPAL_AMFINETUNEVOLUME;
		DataItem faPALAmVolume = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_pal_amfinetunevolume),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALAmVolume);
		faPALAmVolume.setmParentGroup(faPALSystem.mSubChildGroup);

		// NICAM FINE TUNE VOLUME
		ItemName = MenuConfigManager.FAPAL_NICAMFINETUNEVOLUME;
		DataItem faPALNicamVolume = new DataItem(
				ItemName,
				mContext
						.getString(R.string.menu_factory_audio_pal_nicamfinetunevolume),
				MINVALUE, MAXVALUE, DEFAULTVALUE, null,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYPROGRESSVIEW);
		faPALSystem.mSubChildGroup.add(faPALNicamVolume);
		faPALNicamVolume.setmParentGroup(faPALSystem.mSubChildGroup);

		/**
		 *EU system
		 */
		ItemName = MenuConfigManager.FA_EUSYSTEM;
		DataItem faEUSystem = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_eu_system),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoyAudio.mSubChildGroup.add(faEUSystem);
		faEUSystem.setmParentGroup(factoyAudio.mSubChildGroup);
		faEUSystem.mSubChildGroup = new ArrayList<DataItem>();

		// FM SATURATION MUTE
		ItemName = MenuConfigManager.FAEU_FM;
		DataItem faEUSystemFM = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_eu_fmsaturationmute),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, DEFAULTVALUE, mFaSystemTest,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYOPTIONVIEW);
		faEUSystem.mSubChildGroup.add(faEUSystemFM);
		faEUSystemFM.setmParentGroup(faEUSystem.mSubChildGroup);

		// EU NON-EU SYSTEM
		ItemName = MenuConfigManager.FAEU_EU_NON;
		DataItem faEUSystemEU = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_eu_eunoneusystem),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, DEFAULTVALUE, mFaSystemTest,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.FACTORYOPTIONVIEW);
		faEUSystem.mSubChildGroup.add(faEUSystemEU);
		faEUSystemEU.setmParentGroup(faEUSystem.mSubChildGroup);

		// latency wait time
		ItemName = MenuConfigManager.FA_LATENCY;
		DataItem fvLatency = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_audio_latency), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		factoyAudio.mSubChildGroup.add(fvLatency);

		factoySetup.mSubChildGroup = new ArrayList<DataItem>();
		// setup
		ItemName = MenuConfigManager.FACTORY_SETUP_EVENT_FORM;
		DataItem fvEventForm = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup_eventform),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, 0, mFvEventForm,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		factoySetup.mSubChildGroup.add(fvEventForm);
		ItemName = MenuConfigManager.FACTORY_SETUP_BURNING_MODE;
		DataItem fvBurningMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup_burningmode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, 0, mFvBurningMode,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		factoySetup.mSubChildGroup.add(fvBurningMode);
		ItemName = MenuConfigManager.FACTORY_SETUP_CI_UPDATE;
		DataItem updateCi = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup_ci_update),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		ItemName = MenuConfigManager.FACTORY_SETUP_CI_ERASE;
		DataItem eraseCi = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup_ci_erase),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		ItemName = MenuConfigManager.FACTORY_SETUP_CI_QUERY;
		DataItem queryCi = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup_ci_query),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
//		factoySetup.mSubChildGroup.add(updateCi);
//		factoySetup.mSubChildGroup.add(eraseCi);
//		factoySetup.mSubChildGroup.add(queryCi);
		updateCi.setmParentGroup(factoySetup.mSubChildGroup);
		eraseCi.setmParentGroup(factoySetup.mSubChildGroup);
		queryCi.setmParentGroup(factoySetup.mSubChildGroup);
		updateCi.mSubChildGroup = new ArrayList<DataItem>();
		eraseCi.mSubChildGroup = new ArrayList<DataItem>();
		queryCi.mSubChildGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.FACTORY_SETUP_CLEAN_STORAGE;
		DataItem fvCleanStorage = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_setup_cleanstorage),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoySetup.mSubChildGroup.add(fvCleanStorage);
		fvCleanStorage.setmParentGroup(factoySetup.mSubChildGroup);
		fvCleanStorage.mSubChildGroup = new ArrayList<DataItem>();

		factoyPresetCh.mSubChildGroup = new ArrayList<DataItem>();
		// preset ch
		ItemName = MenuConfigManager.FACTORY_PRESET_CH_DUMP;
		DataItem fvDumpCh = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_preset_dump),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoyPresetCh.mSubChildGroup.add(fvDumpCh);
		fvDumpCh.setmParentGroup(factoyPresetCh.mSubChildGroup);
		fvDumpCh.mSubChildGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.FACTORY_PRESET_CH_PRINT;
		DataItem fvPrintCh = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_preset_print),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoyPresetCh.mSubChildGroup.add(fvPrintCh);
		fvPrintCh.setmParentGroup(factoyPresetCh.mSubChildGroup);
		fvPrintCh.mSubChildGroup = new ArrayList<DataItem>();
		ItemName = MenuConfigManager.FACTORY_PRESET_CH_RESTORE;
		DataItem fvRestoreCh = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_preset_restore),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		factoyPresetCh.mSubChildGroup.add(fvRestoreCh);
		fvRestoreCh.setmParentGroup(factoyPresetCh.mSubChildGroup);
		fvRestoreCh.mSubChildGroup = new ArrayList<DataItem>();

		// }
	}

	private void addVideoFactorySub(DataItem factoyVideo) {
		String[] mFvColor = mContext.getResources().getStringArray(
				R.array.menu_factory_video_colortemperature_array);

		String[] mFvDIMA = mContext.getResources().getStringArray(
				R.array.menu_factory_auto_dima_array);

		String[] mFvDIEdge = mContext.getResources().getStringArray(
				R.array.menu_factory_video_diedge_array);

		String[] mFvWCG = mContext.getResources().getStringArray(
				R.array.menu_factory_video_wcg_array);

		String[] mFvFlip = mContext.getResources().getStringArray(
				R.array.menu_factory_video_flip_array);

		String[] mFvMirror = mContext.getResources().getStringArray(
				R.array.menu_factory_video_mirror_array);

		factoyVideo.mSubChildGroup = new ArrayList<DataItem>();
		/**
		 * factory_video
		 */
		// auto color
		ItemName = MenuConfigManager.FV_AUTOCOLOR;
		DataItem fvAutoColor = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_auto_color),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		fvAutoColor.mSubChildGroup = new ArrayList<DataItem>();
		fvAutoColor.setmParentGroup(factoyVideo.mSubChildGroup);
		fvAutoColor.setmParent(factoyVideo);
		factoyVideo.mSubChildGroup.add(fvAutoColor);

		// when VGA,available,other,can not use
		if (TVInputManager.INPUT_TYPE_VGA.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))
				&& NavIntegration.getInstance(mContext)
						.iCurrentInputSourceHasSignal()) {
			fvAutoColor.isEnable = true;
		} else {
			fvAutoColor.isEnable = false;
		}

		// ColorTemperature
		ItemName = MenuConfigManager.FV_COLORTEMPERATURE;
		DataItem fvColorTemperature = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_temperature),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		fvColorTemperature.mSubChildGroup = new ArrayList<DataItem>();
		fvColorTemperature.setmParentGroup(factoyVideo.mSubChildGroup);
		factoyVideo.mSubChildGroup.add(fvColorTemperature);

		// ColorTemperatureChild
		ItemName = MenuConfigManager.FV_COLORTEMPERATURECHILD;
		DataItem fvColorTemperatureChild = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_temperature),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mFvColor,
				MenuConfigManager.STEP_VALUE,
				DataItem.DataType.EFFECTOPTIONVIEW);
		fvColorTemperature.mSubChildGroup.add(fvColorTemperatureChild);

		// R Gain
		ItemName = MenuConfigManager.FV_COLOR_G_R;
		DataItem fvColorGR = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_rgain),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR);
		fvColorTemperature.mSubChildGroup.add(fvColorGR);

		// G Gain
		ItemName = MenuConfigManager.FV_COLOR_G_G;
		DataItem fvColorGG = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_ggain),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR);
		fvColorTemperature.mSubChildGroup.add(fvColorGG);

		// B Gain
		ItemName = MenuConfigManager.FV_COLOR_G_B;
		DataItem fvColorGB = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_bgain),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR);
		fvColorTemperature.mSubChildGroup.add(fvColorGB);

		// R Offset
		ItemName = MenuConfigManager.FV_COLOR_O_R;
		DataItem fvColorOR = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_roffset),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR);
		fvColorTemperature.mSubChildGroup.add(fvColorOR);

		// G Offset
		ItemName = MenuConfigManager.FV_COLOR_O_G;
		DataItem fvColorOG = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_goffset),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR);
		fvColorTemperature.mSubChildGroup.add(fvColorOG);

		// B Offset
		ItemName = MenuConfigManager.FV_COLOR_O_B;
		DataItem fvColorOB = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_color_boffset),
				mConfigManager.getMin(ItemName), mConfigManager
						.getMax(ItemName), mConfigManager.getDefault(ItemName),
				null, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.PROGRESSBAR);
		fvColorTemperature.mSubChildGroup.add(fvColorOB);

		// change value of down,view model change as user
		// fvColorTemperatureChild.setUserDefined(0);
		fvColorTemperatureChild.mEffectGroup = new ArrayList<DataItem>();
		fvColorGR.mParent = fvColorTemperatureChild;
		fvColorGG.mParent = fvColorTemperatureChild;
		fvColorGB.mParent = fvColorTemperatureChild;
		fvColorOR.mParent = fvColorTemperatureChild;
		fvColorOG.mParent = fvColorTemperatureChild;
		fvColorOB.mParent = fvColorTemperatureChild;
		// add by pingping
		fvColorGR.mParentGroup=fvColorTemperatureChild.mEffectGroup;
		fvColorGG.mParentGroup=fvColorTemperatureChild.mEffectGroup;
		fvColorGB.mParentGroup=fvColorTemperatureChild.mEffectGroup;
		fvColorOR.mParentGroup=fvColorTemperatureChild.mEffectGroup;
		fvColorOG.mParentGroup=fvColorTemperatureChild.mEffectGroup;
		fvColorOB.mParentGroup=fvColorTemperatureChild.mEffectGroup;
		
		fvColorTemperatureChild.mEffectGroup.add(fvColorGR);
		fvColorTemperatureChild.mEffectGroup.add(fvColorGG);
		fvColorTemperatureChild.mEffectGroup.add(fvColorGB);
		fvColorTemperatureChild.mEffectGroup.add(fvColorOR);
		fvColorTemperatureChild.mEffectGroup.add(fvColorOG);
		fvColorTemperatureChild.mEffectGroup.add(fvColorOB);

		// AUTO PHASE only component source and has signal
		if (NavIntegration.getInstance(mContext).isCurrentSourceComponent()
				&& NavIntegration.getInstance(mContext)
						.iCurrentInputSourceHasSignal()) {
			ItemName = MenuConfigManager.FV_AUTOPHASE;
			DataItem fvAutoPhase = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_auto_phase),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.HAVESUBCHILD);
			fvAutoPhase.mSubChildGroup = new ArrayList<DataItem>();
			fvAutoPhase.setmParentGroup(factoyVideo.mSubChildGroup);
			fvAutoPhase.setmParent(factoyVideo);
			factoyVideo.mSubChildGroup.add(fvAutoPhase);

			ItemName = MenuConfigManager.FV_PHASE;
			DataItem fvPhase = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_phase),
					mConfigManager.getMin(ItemName), mConfigManager
							.getMax(ItemName), mConfigManager
							.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
			factoyVideo.mSubChildGroup.add(fvPhase);
		}

		// only TV and have channel,show H POSITION and V POSITION
		TVChannelManager mTM = mTV.getChannelManager();
//		MtkLog.v(TAG, "channels -------" + mTM.getChannels().size());
		List<TVChannel> myChannelList =  mTM.getChannels();
		if (TVInputManager.INPUT_TYPE_TV.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))
				&& null != myChannelList && myChannelList.size() > 0) {

			// H POSITION
			ItemName = MenuConfigManager.FV_HPOSITION;
			DataItem fvHPosition = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_h_position),
					mConfigManager.getMin(ItemName), mConfigManager
							.getMax(ItemName), mConfigManager
							.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.POSITIONVIEW);
			factoyVideo.mSubChildGroup.add(fvHPosition);

			// V POSITION
			ItemName = MenuConfigManager.FV_VPOSITION;
			DataItem fvVPosition = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_v_position),
					mConfigManager.getMin(ItemName), mConfigManager
							.getMax(ItemName), mConfigManager
							.getDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.POSITIONVIEW);
			factoyVideo.mSubChildGroup.add(fvVPosition);
		}

		// VGA have no DIMA,DI EDGE,WCG
		if (!TVInputManager.INPUT_TYPE_VGA.equals(mTV.getInputManager()
				.getTypeFromInputSource(mCurrentSource))) {
			// DIMA
			ItemName = MenuConfigManager.FV_DIMA;
			DataItem fvDIMA = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_di_ma),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), mFvDIMA,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
			factoyVideo.mSubChildGroup.add(fvDIMA);

			// DIEdge DI border
			ItemName = MenuConfigManager.FV_DIEDGE;
			DataItem fvDIEdge = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_di_edge),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), mFvDIEdge,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
			factoyVideo.mSubChildGroup.add(fvDIEdge);

			// WCG
			ItemName = MenuConfigManager.FV_WCG;
			DataItem fvWcg = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_video_wcg),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mConfigManager
							.getDefault(ItemName)
							- mConfigManager.getMin(ItemName), mFvWCG,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
			factoyVideo.mSubChildGroup.add(fvWcg);
		}

		// Flip
		ItemName = MenuConfigManager.FV_FLIP;
		DataItem fvFlip = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_flip),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mFvFlip,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		factoyVideo.mSubChildGroup.add(fvFlip);

		// Mirror
		ItemName = MenuConfigManager.FV_MIRROR;
		DataItem fvMirror = new DataItem(ItemName, mContext
				.getString(R.string.menu_factory_video_mirror),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, mConfigManager
						.getDefault(ItemName)
						- mConfigManager.getMin(ItemName), mFvMirror,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		factoyVideo.mSubChildGroup.add(fvMirror);
	}

	public DataItem refreshVideoFactorySub(DataItem dataItem) {
		if (MenuConfigManager.FACTORY_VIDEO.equals(dataItem.mItemID)) {
			addVideoFactorySub(dataItem);
			return dataItem;
		}
		return null;
	}

	/**
	 * load tune dialog info
	 * 
	 * @param flag
	 *            :load different type of dataIt
	 */
	public void loadTuneDiagInfo(boolean flag) {
		String[] nullArray = mContext.getResources().getStringArray(
				R.array.menu_setup_null_array);
		if (flag) {

		} else {
			ItemName = MenuConfigManager.FACTORY_TV_TUNER_DIAGNOSTIC_NOINFO;
			DataItem fvEventForm = new DataItem(ItemName, mContext
					.getString(R.string.menu_factory_TV_tunerdiag_noinfo),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, 0, nullArray,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.FACTORYOPTIONVIEW);
			fvTunerDiagnostic.mSubChildGroup.add(fvEventForm);
		}

	}

	/**
	 * load power on channel data
	 */
	public void loadPowerOnChannelData() {
		if (channelPowerOnDataGroup.size() > 0) {
			channelPowerOnDataGroup = new ArrayList<DataItem>();
		}
		ItemName = MenuConfigManager.POWER_ON_CH_MODE;
		int page = 0;
		int num = 0;
		for (int j = 0; j < chNum; j++) {
			if (j % 8 == 0 && j != 0) {
				page += 1;
			}
			if (saveV.readValue("Power_On_Channel") - page == j + 1)
				break;
			num++;
		}
		if (num == chNum) {
			saveV.saveValue(ItemName, 0);
		}

		powerOnChannelMode = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_power_on_channel_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
				poweronChannel, MenuConfigManager.STEP_VALUE,
				DataItem.DataType.SWICHOPTIONVIEW);
		powerOnChannelModeInvalid = new DataItem(ItemName, mContext
				.getString(R.string.menu_setup_power_on_channel_mode),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, 1, poweronChannel,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW);
		powerOnChannel.mSubChildGroup.add(powerOnChannelMode);
		channelPowerOnDataGroup.add(powerOnChannelMode);
		powerOnChannelMode.mEffectGroup = new ArrayList<DataItem>();
		powerOnChannelMode.mSwitchHashMap = new HashMap<Integer, Boolean[]>();
		if (chNum <= 0) {
			String[] tStrings = { " ", " " };
			DataItem noChannel = new DataItem("SETUP_No_channel", mContext
					.getString(R.string.menu_setup_power_on_no_channel),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, 0, tStrings,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.CHANNELPOWERNOCAHNNEL);

			powerOnChannelMode.mEffectGroup.add(noChannel);
			if (powerOnChannelMode.mInitValue == 0) {
				noChannel.setEnable(false);
			} else {
				noChannel.setEnable(true);
			}

			noChannel.setEnable(false);
			noChannel.mSubChildGroup = new ArrayList<DataItem>();
			powerOnChannel.mSubChildGroup.add(noChannel);

			powerOnChannelMode.setmParentGroup(powerOnChannel.mSubChildGroup);
			channelPowerOnDataGroup.add(noChannel);
			noChannel.setmParentGroup(powerOnChannel.mSubChildGroup);
			Boolean[] t1 = new Boolean[1];
			t1[0] = false;
			Boolean[] t2 = new Boolean[1];
			t2[0] = true;
			powerOnChannelMode.mSwitchHashMap.put(0, t1);
			powerOnChannelMode.mSwitchHashMap.put(1, t2);
		} else {
			for (int i = 0; i < chNum; i++) {

				DataItem channelList = new DataItem(
						MenuConfigManager.SETUP_POWER_ONCHANNEL_LIST, " ",
						MenuConfigManager.INVALID_VALUE,
						MenuConfigManager.INVALID_VALUE,
						MenuConfigManager.INVALID_VALUE, channelInfo.get(i),
						MenuConfigManager.STEP_VALUE,
						DataItem.DataType.CHANNELPOWERONVIEW);
				if (i % 8 == 0 && i != 0) {
					powerOnChannel.mSubChildGroup
							.add(powerOnChannelModeInvalid);
					powerOnChannelModeInvalid.setEnable(false);
					channelPowerOnDataGroup.add(powerOnChannelModeInvalid);
					powerOnChannelMode.mEffectGroup
							.add(powerOnChannelModeInvalid);
					page += 1;

				} else {
					powerOnChannelMode.mEffectGroup.add(channelList);
					if (powerOnChannelMode.mInitValue == 0) {
						channelList.setEnable(false);
					} else {
						channelList.setEnable(true);
					}
					if (saveV.readValue("Power_On_Channel") - page == i + 1) {
						channelList.mBrightBackGroundFlag = true;
					} else {
						channelList.mBrightBackGroundFlag = false;
					}

				}
				channelList.mSubChildGroup = new ArrayList<DataItem>();
				powerOnChannel.mSubChildGroup.add(channelList);

				powerOnChannelMode
						.setmParentGroup(powerOnChannel.mSubChildGroup);
				powerOnChannelModeInvalid
						.setmParentGroup(powerOnChannel.mSubChildGroup);
				channelPowerOnDataGroup.add(channelList);
				channelList.setmParentGroup(powerOnChannel.mSubChildGroup);
			}
			Boolean[] t1 = new Boolean[chNum];
			for (int i = 0; i < chNum; i++) {
				t1[i] = false;
			}
			Boolean[] t2 = new Boolean[chNum];
			for (int i = 0; i < chNum; i++) {
				if (i % 8 == 0 && i != 0) {
					t2[i] = false;
				} else {
					t2[i] = true;
				}

			}
			powerOnChannelMode.mSwitchHashMap.put(0, t1);
			powerOnChannelMode.mSwitchHashMap.put(1, t2);
		}

	}

	public DataItem getPowerOnChannelMode() {
		return powerOnChannelMode;
	}

	public void setPowerOnChannelMode(DataItem powerOnChannelMode) {
		this.powerOnChannelMode = powerOnChannelMode;
	}

	public DataItem getFvTunerDiagnostic() {
		return fvTunerDiagnostic;
	}

	public void setFvTunerDiagnostic(DataItem fvTunerDiagnostic) {
		this.fvTunerDiagnostic = fvTunerDiagnostic;
	}

	public DataItem getInputBlock() {
		return inputBlock;
	}

	public void setInputBlock(DataItem inputBlock) {
		this.inputBlock = inputBlock;
	}

	public DataItem getTimeSet() {
		return timeSet;
	}

	public void setTimeSet(DataItem timeSet) {
		this.timeSet = timeSet;
	}

	public String getBlockSourceName() {
		return blockSourceName;
	}

	public String getBlockSourceName3D() {
		return blockSourceName3D;
	}

	public void setBlockSourceName(String blockSourceName) {
		this.blockSourceName = blockSourceName;
	}

	public DataItem getApplication() {
		return application;
	}

	public void setApplication(DataItem application) {
		this.application = application;
	}

	public DataItem getM3DItem() {
		return m3DItem;
	}

	public void setM3DItem(DataItem m3dItem) {
		m3DItem = m3dItem;
	}

	public String[] getM3DModeStrArr() {
		return m3DModeStrArr;
	}

	public void setM3DModeStrArr(String[] m3dModeStrArr) {
		m3DModeStrArr = m3dModeStrArr;
	}

	public void loadTimeSetUpData() {
		Long timeLeft = NavIntegration.getInstance(mContext)
				.iPowerOffMinutesLeft("timetosleep");
		if (timeLeft > 0) {
			String[] optionTiemSleep = mContext.getResources().getStringArray(
					R.array.menu_setup_sleep_timer_array);
			ItemName = MenuConfigManager.SLEEP_TIMER;
			int mInitValue = getIndexByLeftTime(timeLeft);
			MtkLog.d(TAG, "mInitValue------------------------->" + mInitValue);
			DataItem timeSleep = new DataItem(ItemName, mContext
					.getString(R.string.menu_setup_sleep_timer),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, mInitValue,
					optionTiemSleep, MenuConfigManager.STEP_VALUE,
					DataItem.DataType.OPTIONVIEW);
			setupTime.mSubChildGroup.remove(2);
			setupTime.mSubChildGroup.add(2, timeSleep);
		} else {
			String[] optionTiemSleep = mContext.getResources().getStringArray(
					R.array.menu_setup_sleep_timer_array);
			ItemName = MenuConfigManager.SLEEP_TIMER;
			DataItem timeSleep = new DataItem(ItemName, mContext
					.getString(R.string.menu_setup_sleep_timer),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, saveV.readValue(ItemName),
					optionTiemSleep, MenuConfigManager.STEP_VALUE,
					DataItem.DataType.OPTIONVIEW);
			setupTime.mSubChildGroup.remove(2);
			setupTime.mSubChildGroup.add(2, timeSleep);
		}

	}

	private int getIndexByLeftTime(Long timeLeft) {
		if (timeLeft < 10)
			return 0;
		if (timeLeft < 20) {
			return 1;
		}
		if (timeLeft < 30)
			return 2;
		if (timeLeft < 40)
			return 3;
		if (timeLeft < 50)
			return 4;
		if (timeLeft < 60)
			return 5;
		if (timeLeft < 90)
			return 6;
		if (timeLeft < 120)
			return 7;
		return 8;
	}

	public void loadVGA() {
		if (mAdjustDataGroup != null && mAdjustDataGroup.size() > 0) {
			mAdjustDataGroup.clear();
		}
		if (mAdjustDataGroup != null) {
		ItemName = MenuConfigManager.AUTO_ADJUST;
		DataItem mVideoAutoAdjust = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_auto_adjust),
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE,
				MenuConfigManager.INVALID_VALUE, null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
		mAdjustDataGroup.add(mVideoAutoAdjust);
		mVideoAutoAdjust.mSubChildGroup = new ArrayList<DataItem>();

		// H POSITION
		ItemName = MenuConfigManager.HPOSITION;
		DataItem mVideoAutoHPosition = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_hposition), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mAdjustDataGroup.add(mVideoAutoHPosition);

		// V POSITION
		ItemName = MenuConfigManager.VPOSITION;
		DataItem mVideoAutoVPosition = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_vposition), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mAdjustDataGroup.add(mVideoAutoVPosition);

		// PHASE
		ItemName = MenuConfigManager.PHASE;
		DataItem mVideoAutoVPhase = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_phase), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mAdjustDataGroup.add(mVideoAutoVPhase);

		// CLOCK
		ItemName = MenuConfigManager.CLOCK;
		DataItem mVideoAutoClock = new DataItem(ItemName, mContext
				.getString(R.string.menu_video_clock), mConfigManager
				.getMin(ItemName), mConfigManager.getMax(ItemName),
				mConfigManager.getDefault(ItemName), null,
				MenuConfigManager.STEP_VALUE, DataItem.DataType.PROGRESSBAR);
		mAdjustDataGroup.add(mVideoAutoClock);
		}
	}
	
	public void loadAnaLogChild() {
		if (mAnalogScanSubChildGroup != null && mAnalogScanSubChildGroup.size() > 0) {
			mAnalogScanSubChildGroup.clear();
		}
		// start frequency 3
				int startFrq = 0;
				tvCh = chSel.getCurrentChannel();
				if ((tvCh != null) /*&& (tvCh.isValid())*/) {
					startFrq = tvCh.getFreq() / 1000000;
				} else {
					startFrq = 44;
				}
				MenuMain.setFreFrom(startFrq * 1000000);
				ItemName = MenuConfigManager.TV_CHANNEL_START_FREQUENCY;
				mAnalogScanSubChildGroup.add(new DataItem(ItemName, mContext
						.getString(R.string.menu_tv_start_frequency), 44, 865,
						startFrq, null, MenuConfigManager.STEP_VALUE,
						DataItem.DataType.NUMVIEW));
				// end frequency 3
				int endFrq = 865;
				MenuMain.setFreTo(endFrq * 1000000);
				ItemName = MenuConfigManager.TV_CHANNEL_END_FREQUENCY;
				mAnalogScanSubChildGroup.add(new DataItem(ItemName, mContext
						.getString(R.string.menu_tv_end_frequency), 44, 865, endFrq,
						null, MenuConfigManager.STEP_VALUE, DataItem.DataType.NUMVIEW));
				// begin scan 3
				ItemName = MenuConfigManager.TV_CHANNEL_STARTSCAN;
				DataItem beginScan = new DataItem(ItemName, mContext
						.getString(R.string.menu_tv_status_value),
						MenuConfigManager.INVALID_VALUE,
						MenuConfigManager.INVALID_VALUE,
						MenuConfigManager.INVALID_VALUE, null,
						MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
				beginScan.mSubChildGroup = new ArrayList<DataItem>();
				mAnalogScanSubChildGroup.add(beginScan);
				beginScan.setmParentGroup(tvChannel.mSubChildGroup);
	}
	
	public boolean isHaveScreenMode(){
		return haveScreenMode;
	}
	public void loadDTMBSingleRF() {

		if (mRFScanDataGroup != null && mRFScanDataGroup.size() > 0) {
			mRFScanDataGroup.clear();
		}
		// scan frequency
		String[] scanMode = mContext.getResources().getStringArray(
				R.array.menu_tv_scan_mode_array);
		TVScanner tvScanner = TVContent.getInstance(mContext).getScanner();
		int tunerMode = mConfigManager.getDefault(MenuConfigManager.TUNER_MODE);
		int scanLowFrq, scanUpperFreq;
		if (mRFScanDataGroup != null) {
		if (tunerMode == ConfigType.BS_SRC_CABLE) {
			scanLowFrq = tvScanner.getDvbcLowerFreq() / 1000000;
			scanUpperFreq = tvScanner.getDvbcUpperFreq() / 1000000;
			ItemName = MenuConfigManager.TV_CHANNEL_FREQUENCY;
			mRFScanDataGroup.add(new DataItem(ItemName, mContext
					.getString(R.string.menu_tv_rf_scan_frequency), scanLowFrq,
					scanUpperFreq, scanLowFrq, null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.NUMVIEW));
			// symbol rate
			int sym_rate = 0;
			ItemName = MenuConfigManager.SYM_RATE;
			mRFScanDataGroup.add(new DataItem(ItemName, mContext
					.getString(R.string.menu_tv_rf_sym_rate), mConfigManager
					.getScanMin(ItemName), mConfigManager.getScanMax(ItemName),
					mConfigManager.getScanDefault(ItemName), null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.NUMVIEW));
			// scan mode
			ItemName = MenuConfigManager.SCAN_MODE;
			mRFScanDataGroup.add(new DataItem(ItemName, mContext
					.getString(R.string.menu_tv_rf_scan_mode), mConfigManager
					.getScanMin(ItemName), mConfigManager.getScanMax(ItemName),
					mConfigManager.getScanDefault(ItemName)
							- mConfigManager.getScanMin(ItemName), scanMode,
					MenuConfigManager.STEP_VALUE,
					DataItem.DataType.SCANCHANNELSOPTIONVIEW));
			// begin scan
			ItemName = MenuConfigManager.TV_CHANNEL_STARTSCAN;
			DataItem startScan = new DataItem(ItemName,
					mContext.getString(R.string.menu_tv_status_value),
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE,
					MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.HAVESUBCHILD);
			startScan.mSubChildGroup = new ArrayList<DataItem>();
			mRFScanDataGroup.add(startScan);

		} else {
//			scanLowFrq = tvScanner.getDtmbLowerFreq() / 1000000;
//			scanUpperFreq = tvScanner.getDtmbUpperFreq() / 1000000;

			ItemName = MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL;
			// RFScan.mSubChildGroup.add(new DataItem(ItemName, mContext
			// .getString(R.string.menu_tv_single_rf_channel), mConfigManager
			// .getScanMin(ItemName), mConfigManager.getScanMax(ItemName),
			// mConfigManager.getScanDefault(ItemName), null,
			// MenuConfigManager.STEP_VALUE, DataItem.DataType.OPTIONVIEW));
			DataItem RFChannel = new DataItem(ItemName,
					mContext.getString(R.string.menu_tv_single_rf_channel),
					MenuConfigManager.INVALID_VALUE, MenuConfigManager.INVALID_VALUE, MenuConfigManager.INVALID_VALUE, null,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.NUMVIEW);
			mRFScanDataGroup.add(RFChannel);
			ItemName = MenuConfigManager.TV_SINGLE_SCAN_SIGNAL_LEVEL;
			int mSignalLevel = EditChannel.getInstance(mContext)
					.getSignalLevel();
			DataItem SignalLevelDateItem = new DataItem(ItemName,
					mContext.getString(R.string.menu_tv_single_signal_level),
					0, 100, mSignalLevel, null, MenuConfigManager.STEP_VALUE,
					DataItem.DataType.PROGRESSBAR);
			SignalLevelDateItem.setEnable(false);
			mRFScanDataGroup.add(SignalLevelDateItem);

			ItemName = MenuConfigManager.TV_SINGLE_SCAN_SIGNAL_QUALITY;
			int mSignalQuality = EditChannel.getInstance(mContext)
					.getSignalQuality();
			String[] mSignalQualityStr = mContext
					.getResources()
					.getStringArray(R.array.menu_setup_tv_single_signal_quality);
			DataItem SignalQuality = new DataItem(ItemName,
					mContext.getString(R.string.menu_tv_single_signal_quality),
					0, 2, mSignalQuality, mSignalQualityStr,
					MenuConfigManager.STEP_VALUE, DataItem.DataType.NUMVIEW);
			SignalQuality.setEnable(false);
			mRFScanDataGroup.add(SignalQuality);
		}
	 }
}
}
