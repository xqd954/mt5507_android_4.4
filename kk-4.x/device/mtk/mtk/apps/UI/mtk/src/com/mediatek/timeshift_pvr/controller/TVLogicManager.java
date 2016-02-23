package com.mediatek.timeshift_pvr.controller;

import android.content.Context;
import android.graphics.Rect;

import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.util.NavIntegrationZoom;
import com.mediatek.ui.nav.util.NewPipLogic;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.ScreenModeConfig;

import java.util.List;
import java.util.Locale;

/**
 * 
 */
public class TVLogicManager {
	private static TVLogicManager instance;

	// PIP/POP mode
	public static final int MODE_NORMAL = 0;
	public static final int MODE_PIP = 1;
	public static final int MODE_POP = 2;

	private TVCommonManager mTVCMManager;
	private TVOutputCommon mTVOutPut;
	private TVContent mTVcontent;

	private TimeShiftManager mTopManager;
	private VideoManager playBack;

	private Context mContext;

	public TVLogicManager(Context context, TimeShiftManager topManager) {

		mTopManager = topManager;

		mTVCMManager = TVCommonManager.getInstance(context);
		mTVOutPut = TVOutputCommon.getInstance(context);

		mTVcontent = TVContent.getInstance(context);
		mContext = context;
	}

	public static TVLogicManager getInstance(Context context,
			TimeShiftManager topManager) {
		if (instance == null) {
			instance = new TVLogicManager(context, topManager);
		}
		return instance;
	}

	public static TVLogicManager getInstance() {
		if (instance != null) {
			return instance;
		}
		return null;
	}
	public int getCurrentMode() {
		// return mTVCMManager.getCurrentOutputMode();
		return MODE_NORMAL;
	}

	public boolean isScanning() {
		return MenuMain.mScanningStatus;
	}

	public boolean isPipPopMode() {
		if (getCurrentMode() == MODE_NORMAL) {
			return true;
		} else {
			return false;
		}
	}

	public boolean isDTV() {
		return mTVCMManager.getCurrentInputSource().equalsIgnoreCase("DTV");
	}

	public boolean dtvNotReadyForRecord() {

		boolean notReady = !isDTV() || !hasVideo() || (!dtvHasSignal())
				|| (!hasDTVSignal()) || (dtvIsScrambled())
				|| (dtvIsScrambled());

		return notReady;
	}

	public List<TVChannel> getChannels() {
		return mTVcontent.getChannelManager().getChannels();
	}

	public String getChannelName() {

		return mTVcontent.getChannelSelector().getCurrentChannel()
				.getChannelName();
	}

	public TVChannel getCurrentChannel() {
		
		return mTVcontent.getChannelSelector().getCurrentChannel();
	}
	
	public int getChannelNumInt() {

		return mTVcontent.getChannelSelector().getCurrentChannel()
				.getChannelNum();
	}

	public String getChannelNumStr() {

		return "CH"
				+ mTVcontent.getChannelSelector().getCurrentChannel()
						.getChannelNum();
	}

	private boolean dtvIsScrambled() {

		boolean hasSignal = false;
		hasSignal = isAudioScrambled() || isVideoScrambled();
		Util.showDLog("dtvIsScrambled(): " + hasSignal);

		return hasSignal;
	}

	private boolean dtvHasSignal() {

		boolean hasSignal = false;
		hasSignal = mTVcontent.getChannelSelector().hasSignal();
		Util.showDLog("dtvHasSignal(): " + hasSignal);
		return hasSignal;
	}

	private String getScrambleState() {
		return mTVcontent.getChannelSelector().getScrambleState();
	}

	private boolean isAudioScrambled() {
		String state = getScrambleState();
		if (state.equals(NavIntegration.STATE_AUDIO_AND_VIDEO_SCRAMBLED)
				|| state.equals(NavIntegration.STATE_AUDIO_CLEAR_VIDEO_SCRAMBLED)) {
			return true;
		}
		return false;
	}

	private boolean isVideoScrambled() {
		String state = getScrambleState();
		if (state.equals(NavIntegration.STATE_AUDIO_AND_VIDEO_SCRAMBLED)
				|| state.equals(NavIntegration.STATE_VIDEO_CLEAR_AUDIO_SCRAMBLED)) {
			return true;
		}
		return false;
	}

	public boolean hasDTVSignal() {
		boolean hasSignal = false;

		if (isDTV() && mTVOutPut.hasSignal("main")) {
			hasSignal = true;
		} else {
			hasSignal = false;
		}

		Util.showDLog("hasDTVSignal(): " + hasSignal);
		return hasSignal;
	}

	public boolean isDTVSourceBlocked(){
		boolean block=NavIntegration.getInstance(mContext).isTheSourceBlocked("dtv");
				
		Util.showDLog("TVLogic-isDTVSourceBlocked(),"+block);
		return block;
	}
	
	public boolean isCurrentChannelBlocked(){
		boolean block=NavIntegration.getInstance(mContext).isCurrentChannelBlocked();
		
		Util.showDLog("TVLogic-isCurrentChannelBlocked(),"+block);
		
		return block;
	}
	
	
	private boolean hasVideo() {
		Rect mResolutionRect;

		TVOutput mTvOutput = mTVcontent.getInputManager().getDefaultOutput();
		if (mTvOutput.getInstanceVideoResolution().getTimingType() == TVOutputCommon.TimingType.TIMING_TYPE_NOT_SUPPORT) {
			mResolutionRect = new Rect(0, 0, 0, 0);
		} else {
			mResolutionRect = mTvOutput.getInstanceVideoResolution()
					.getVideoResolution();
		}

		int width = mResolutionRect.width();
		int height = mResolutionRect.height();

		if (height > 0 && width > 0) {
			return true;
		} else {
			return false;
		}
	}

	public boolean prepareScheduleTask(ScheduleItem item) {
		if (item == null) {
			return false;
		}
		try {
			changeDTVSource(item.getSrcType().toLowerCase(Locale.US));
			Short channelNum = Short.decode(item.getChannelName());

			mTVcontent.getChannelSelector().select(channelNum);

			return true;
		} catch (Exception e) {
			return false;
		}
	}

	public String getInputSource() {
		return mTVCMManager.getCurrentInputSource();
	}
	
	private void changeDTVSource(String src) {
		mTVCMManager.changeInputSource("main", src);
	}

	/**
	 * @return the playBack
	 */
	public VideoManager getPlayBack() {
		return playBack;
	}

	/**
	 * @param playBack
	 *            the playBack to set
	 */
	public void setPlayBack(VideoManager playBack) {
		this.playBack = playBack;
	}

	/**
	 * 
	 */
	public void releasePlayer() {
		if (getPlayBack() != null) {
			getPlayBack().onRelease();
			setPlayBack(null);
		}
		// restoreToDefault();
	}

	public boolean isDotByDotScreenMode() {
		int next = ScreenModeConfig.getInstance(mContext).getNextScreen();
		return (next == ConfigType.SCREEN_MODE_DOT_BY_DOT);
	}

	@SuppressWarnings("unchecked")
	public void setDotByDotScreenMode() {
		TVConfigurer cf = mTVcontent.getConfigurer();
		TVOptionRange<Integer> aspOption = (TVOptionRange<Integer>) cf
				.getOption(ConfigType.CFG_SCREEN_MODE);

		int next = ConfigType.SCREEN_MODE_DOT_BY_DOT;
		if (null != aspOption && next <= aspOption.getMax()) {
			aspOption.set(next);
		} else if (null != aspOption) {
			aspOption.set(aspOption.getMin());
		}
	}
	
	@SuppressWarnings("unchecked")
	public void setScreenModeToNormal() {
		TVConfigurer cf = mTVcontent.getConfigurer();
		TVOptionRange<Integer> aspOption = (TVOptionRange<Integer>) cf
				.getOption(ConfigType.CFG_SCREEN_MODE);
		
		StatePVR.isDotByDotMode=false;
		aspOption.set(aspOption.getMin());
	}
	public void reSetZoomValues(Context mContext){
		NavIntegrationZoom.getInstance(mContext).setZoomMode(NavIntegrationZoom.ZOOM_1);
		removeZoomTips();
	}

	private void removeZoomTips() {
		// TODO Auto-generated method stub
		TurnkeyUiMainActivity.getInstance().hideAll();
	}

	public void selectChannel(int keycodeMtkirChup) {
		// TODO Auto-generated method stub
		TurnkeyUiMainActivity.getInstance().handleChangeChannel(keycodeMtkirChup);
	}

	public boolean isPipOrPopState() {
		// TODO Auto-generated method stub
		return PippopUiLogic.getInstance(mContext).isPipOrPopState();
	}

}