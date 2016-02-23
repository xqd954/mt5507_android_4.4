/**
 * use for connect user interface and common logic
 */
package com.mediatek.ui.nav;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Rect;
import android.os.SystemClock;
import android.view.View;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVChannelManager;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInput;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcm.TVStorage;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcm.TVTimerManager.PowerOffTimer;
import com.mediatek.tvcommon.ITVCommon.ChannelFilter;
import com.mediatek.tvcommon.ITVCommon.ChannelsChangedListener;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.tvcommon.TVOutputCommon.MTSOption;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.MtkLog;

/**
 * @author MTK40530
 * 
 */
@SuppressWarnings("unchecked")
public class NavIntegration {

	private static final String TAG = "NavIntegration";

	private static NavIntegration instance;
	private static TVContent mTv;
	private TVInputManager mTVInputManager;
	public TVOptionRange<Integer> dpmsOption;
	private TVConfigurer mConfig;
	private TVChannelManager mTvChannelManager;
	private TVChannelSelector mTvChannelSelector;
	private SharedPreferences sp;
	private TVTimerManager mTvTimerManager;
	private List<String> mSourceList = new ArrayList<String>();

	private TVOutput mTvOutput;

	public static final int CHANNEL_UP = 0;
	public static final int CHANNEL_DOWN = 1;
	public static final int CHANNEL_PRE = 2;
    public static final int SUBTITTLE_LEN = 3;
	public static int subTittleTag = 0;
	public static int subRepeat = 0;
	private static IChannelListChangedListener mClcListener;
	private static List<IChannelSelectorListener> mSelListeners = new ArrayList<IChannelSelectorListener>();
	private static List<IInputSourceChangeListener> mIISCListeners = new ArrayList<IInputSourceChangeListener>();

	public final static String STATE_AUDIO_AND_VIDEO_SCRAMBLED = "videoAndAudioScrambled";
	public final static String STATE_AUDIO_CLEAR_VIDEO_SCRAMBLED = "audioScrambled";
	public final static String STATE_AUDIO_NO_VIDEO_SCRAMBLED = "audioScrambledAndNoVideo";
	public final static String STATE_VIDEO_CLEAR_AUDIO_SCRAMBLED = "videoScrambled";
	public final static String STATE_VIDEO_NO_AUDIO_SCRAMBLED = "videoScrambledAndNoAudio";
	public final static String STATE_UNSCRAMBLED = "unscrambled";

	private MenuConfigManager mConfigManager;
	private MTSOption mMtsOption;

	private long startShiftChannelTime = 0, endShiftChannelTime = 0;
	private Context mContext;
	private String[] audioTvcmArray;
	private String[] audioArray;
	private String[] subtitle;
	
	private boolean isTKMainActivity = false;
	

	public boolean isTKMainActivity() {
		return isTKMainActivity;
	}

	public void setIsTKMainActivity(boolean isTKMainActivity) {
		this.isTKMainActivity = isTKMainActivity;
	}

	private NavIntegration(Context context) {
		mTv = TVContent.getInstance(context);
		mTVInputManager = mTv.getInputManager();
		mTvChannelManager = mTv.getChannelManager();
		mTvChannelSelector = mTv.getChannelSelector();
		mContext = context;

		sp = mContext.getSharedPreferences("com.mediatek.ui.setupwziard",
				Context.MODE_PRIVATE);

		mConfig = mTv.getConfigurer();

		mTvTimerManager = mTv.getTimerManager();
		mTvOutput = mTVInputManager.getDefaultOutput();

		mSourceList = Arrays.asList(mTVInputManager.getInputSourceArray());
		// mChannelList = mTvChannelManager.getChannels();
		mTvChannelManager.registerListListener(mChannelListChangedListener);
		mTvChannelSelector.addSelectorListener(mTvSeclectorListener);

		mTVInputManager.registerSourceListener(mInputSourceListener);

		dpmsOption = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_DPMS);

		mConfigManager = MenuConfigManager.getInstance(context);

		mMtsOption = mTvOutput.getMTSOption();

		audioTvcmArray = mContext.getResources().getStringArray(
				R.array.nav_audio_tvcm_strings);
		audioArray = mContext.getResources().getStringArray(
				R.array.nav_audio_strings);
	}

	public static synchronized NavIntegration getInstance(Context context) {
		if (instance == null) {
			instance = new NavIntegration(context);
		}
		return instance;
	}

	/**
	 * source
	 */
	public void iAddInputsourceChangeListener(
			IInputSourceChangeListener mListener) {
		if (!mIISCListeners.contains(mListener)) {
			mIISCListeners.add(mListener);
		}
	}

	public void setAutoSync() {
		TVOptionRange<Integer> option = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_TIME_SYNC);
	   if(null != option){
	    MtkLog.d(TAG, " TIME_SYNC_MODE_MANUAL: ");
		 option.set(ConfigType.TIME_SYNC_MODE_MANUAL);
	  }
		
	}


	public void setFbmMode(int mode){
		TVOptionRange<Integer> fbmOption = (TVOptionRange<Integer>) mConfig.getOption(ConfigType.CFG_FBM_MODE);
		if(null != fbmOption){
			fbmOption.set(mode);
			}
	}

	public void syncTimeIfNeed(){
        int menuSyncTimeValue = SaveValue.getInstance(mContext).readValue(MenuConfigManager.AUTO_SYNC);
        if (menuSyncTimeValue == 1){
           Long when = mTvTimerManager.getBroadcastUTC();
      			//SystemClock.setCurrentTimeMillis(when);
	 
         }//else if(menuSyncTimeValue == 2){
			 // TODO sync time form network.
		//}
	 }

	/**
	 * Record the main output has signal
	 * 
	 * @return
	 */
	public boolean iCurrentInputSourceHasSignal() {
		return mTVInputManager.getDefaultOutput().hasSignal();
	}

	public boolean isNonPIPMode() {
		if (mTVInputManager.getOuputMode() == 0) {
			return true;
		} else {
			return false;
		}
	}

	TVInputManager.InputSourceListener mInputSourceListener = new TVInputManager.InputSourceListener() {
		public void onBlocked(String input) {
			if (mIISCListeners != null && mIISCListeners.size() > 0){
				for (IInputSourceChangeListener mSelListener : mIISCListeners) {
					mSelListener.isBlocked(input);
				}
			}			
		}

		public void onInputGotSignal(String input) {
			if (mIISCListeners != null && mIISCListeners.size() > 0){
				for (IInputSourceChangeListener mSelListener : mIISCListeners) {
					mSelListener.isInputGotSignal(input);
				}
			}			
		}

		public void onOutputSignalChange(String output, boolean hasSignal) {
			if (mIISCListeners != null && mIISCListeners.size() > 0){
				for (IInputSourceChangeListener mSelListener : mIISCListeners) {
					mSelListener.isOutputSignalChange(output, hasSignal);
				}
			}			
		}

		public void onSelected(String output, String input) {
			if (mIISCListeners != null && mIISCListeners.size() > 0){
				for (IInputSourceChangeListener mSelListener : mIISCListeners) {
					mSelListener.isSelected(output, input);
				}
			}			
		}

		public void onInputSignalChange(String input, boolean hasSignal) {
			// TODO Auto-generated method stub

		}
	};

	public interface IInputSourceChangeListener {
		public void isBlocked(String input);

		public void isInputGotSignal(String input);

		public void isOutputSignalChange(String output, boolean hasSignal);

		public void isSelected(String output, String input);
	}

	public String[] iGetInputSourceList() {
		return mTVInputManager.getInputSourceArray();
	}

	public String[] iGetOutputSourceList() {
		return mTVInputManager.getOutputArray();
	}

	/*public void iSwapInputSource() {
		mTVInputManager.swapInputSource();
	}*/

	public void iSetTheSource(String output, String input) {
		mTVInputManager.changeInputSource(output, input);
	}

	public String iGetCurrentSouce() {
		return mTVInputManager.getCurrInputSource();
	}

	public void iGetNextSource(View mView, iSourceChangeListener mListener) {
		int tmp = 0;

		mTVInputManager.changeNextInputSource();
		tmp = mSourceList.indexOf(mTVInputManager.getCurrInputSource());
		if (tmp >= 0) {
			if (mListener != null) {
				mListener.sourceChanged(mView, tmp);
			}
		}
	}

	public void iSetTheSource(View mView, iSourceChangeListener mListener,
			String inputSource) {
		int tmp = 0;

		mTVInputManager.changeInputSource(inputSource);
		tmp = mSourceList.indexOf(mTVInputManager.getCurrInputSource());
		if (tmp >= 0) {
			if (mListener != null) {
				mListener.sourceChanged(mView, tmp);
			}
		}
	}

	public interface iSourceChangeListener {
		void sourceChanged(View mView, int index);
	}

	private String iFindTvSource() {
		int tmp = 0;

		for (tmp = 0; tmp < mSourceList.size(); tmp++) {
			if (null !=mTVInputManager.getTypeFromInputSource(mSourceList.get(tmp)) && TVInputManager.INPUT_TYPE_TV
					.equals(mTVInputManager.getTypeFromInputSource(mSourceList.get(tmp)))) {
				return mSourceList.get(tmp);
			}
		}
		return null;
	}

	/* set input source on TV */
	public void iSetSourcetoTv() {
		if (null!=iGetCurrentSouce() && null!=mTVInputManager.getTypeFromInputSource(iGetCurrentSouce()) && !TVInputManager.INPUT_TYPE_TV.equals(
				mTVInputManager.getTypeFromInputSource(iGetCurrentSouce()))) {
			mTVInputManager.changeInputSource(iFindTvSource());
		}
	}

	public boolean isCurrentSourceTv() {
		String ins = mTVInputManager.getTypeFromInputSource(iGetCurrentSouce());
		return TVInputManager.INPUT_TYPE_TV.equals(ins);

		// return mTVInputManager.getTypeFromInputSource(iGetCurrentSouce())
		// .equals(TVInputManager.INPUT_TYPE_TV);
	}

	public boolean isCurrentSourceDTV() {
		return (null != mTVInputManager.getCurrInputSource() && "dtv".equals(mTVInputManager.getCurrInputSource()));
	}
	
	public boolean isCurrentSourceATV(){
		return (null != mTVInputManager.getCurrInputSource() && "atv".equals(mTVInputManager.getCurrInputSource()));
	}

	public boolean isCurrentSourceVGA() {
		String ins = mTVInputManager.getTypeFromInputSource(iGetCurrentSouce());
		return (null != ins && ins.equals(TVInputManager.INPUT_TYPE_VGA));
		// return mTVInputManager.getTypeFromInputSource(iGetCurrentSouce())
		// .equals(TVInputManager.INPUT_TYPE_VGA);
	}
	
	public boolean vgaNotSupport() {
		if (isCurrentSourceVGA()
				&& mTVInputManager.getDefaultOutput()
						.getInstanceVideoResolution().getTimingType() == TVOutputCommon.TimingType.TIMING_TYPE_NOT_SUPPORT) {
			return true;
		} else {
			return false;
		}
	}
	public boolean isCurrentSourceComponent(){
		String ins = mTVInputManager.getTypeFromInputSource(iGetCurrentSouce());
		return TVInputManager.INPUT_TYPE_COMPONENT.equals(ins);
	}

	public boolean isCurrentSourceHDMI() {
		if(null !=iGetCurrentSouce()){
		   return (null != mTVInputManager.getTypeFromInputSource(iGetCurrentSouce()) && TVInputManager.INPUT_TYPE_HDMI
				.equals(mTVInputManager.getTypeFromInputSource(iGetCurrentSouce())));
		}else{
		   return false;
		}
		
	}

	public boolean isCurrentSourceAV() {
		String ins = mTVInputManager.getTypeFromInputSource(iGetCurrentSouce());
		return TVInputManager.INPUT_TYPE_AV.equals(ins);
	}

	public Rect iGetResolution() {
		mTvOutput = mTVInputManager.getDefaultOutput();	
		if (mTvOutput.getInstanceVideoResolution().getTimingType() == TVOutputCommon.TimingType.TIMING_TYPE_NOT_SUPPORT) {
			return new Rect(0,0,0,0);
		} else {
		return mTvOutput.getInstanceVideoResolution().getVideoResolution();		
	}
	}
	public boolean isTimingTypeGraphic() {
		mTvOutput = mTVInputManager.getDefaultOutput();	
		if (mTvOutput.getInstanceVideoResolution().getTimingType() == TVOutputCommon.TimingType.TIMING_TYPE_GRAPHIC) {
			return true;
		} else {
			return false;
		}
	}

	public String iGetVideoFormat() {
		mTvOutput = mTVInputManager.getDefaultOutput();
		return mTvOutput.getInstanceVideoResolution().getVideoFormat();
	}

	public int iGetVedioFrequency() { // hzy fix CR:363961
		mTvOutput = mTVInputManager.getDefaultOutput();
		return mTvOutput.getInstanceVideoResolution().getVideoFramerate();
	}

	public boolean isVideoProgressive() {
		mTvOutput = mTVInputManager.getDefaultOutput();
		return mTvOutput.getInstanceVideoResolution().isVideoProgressive();
	}

	public boolean isCurrentSourceATVPlaying(){
		
		
		if(null != mTVInputManager.getCurrInputSource() 
				&& "atv".equals(mTVInputManager.getCurrInputSource())){
			if(!mTVInputManager.isBlock(iGetCurrentSouce())){
				if(mTvChannelSelector.getCurrentChannel()!=null && iGetChannelLength()>0){
						return !isCurrentChannelBlocked();
					
				}
			}
		}
		return false;
	}
	
	/**
	 * judge current source is blocked or not.
	 * 
	 * @return
	 */
	public boolean isCurrentSourceBlocked() {
		TVCommonManager tvMgr=TVCommonManager.getInstance(mContext);
		return tvMgr.isInputSourceBlocked(iGetCurrentSouce());
	}

//	public boolean isCurrentSourceUserBlocked() {
////		String ins = mTVInputManager.getCurrInputSource();
////		if (ins != null) {
////			TVInput in = mTVInputManager.getInput(ins);
////			if (in != null) {
////				return in.isUsrUnblocked();
////			}
////		}
//		
//		TVCommonManager tvMgr=TVCommonManager.getInstance(mContext);
//		boolean pyBlock = isCurrentSourceBlocked();
//		boolean block = tvMgr.isInputSourceBlocked(tvMgr.getCurrentInputSource());
//		return block ? false: pyBlock;
//
//	}

	public boolean isCurrentSourceBlocking(){
//		TVCommonManager tvMgr=TVCommonManager.getInstance(mContext);
		boolean tmpSource=isCurrentSourceBlocked();
		if(isCurrentSourceTv()){
			TVChannel channel=iGetCurrentChannel();
				boolean tmpChannel = ((channel == null) || (channel != null && channel.isBlocked()? true:!channel.isPhysicalBlocked()));
			return tmpChannel && tmpSource;
		}else{
			return tmpSource;
		}
		
	}	
	
	public boolean isCurrentChannelBlocking(){
		TVChannel channel=iGetCurrentChannel();
		if(channel==null){
			return false;
		}else{
			return (channel.isBlocked() /*&& !channel.isUsrUnblocked()*/);
		}
	}

	/**
	 * judge the source is blocked or not.
	 * 
	 * @param inputsource
	 * @return
	 */
	public boolean isTheSourceBlocked(String inputsource) {
		return mTVInputManager.isBlock(inputsource);
	}

	/**
	 * power/sleep
	 */
	/**
	 * @deprecated
	 */
	public void iSetPowerOff(int time) {
		Timer mTimer = new Timer();

		mTimer.schedule(new TimerTask() {
			public void run() {
				mTv.sendPowerOff();
			}
		}, time);
	}

	/**
	 * @deprecated
	 */
	public void iSetPowerOff(View mView, iEffectChangeListener mListener,
			int time) {
		Timer mTimer = new Timer();

		if (mListener != null) {
			mListener.effectChanged(mView, time);

			mTimer.schedule(new TimerTask() {
				public void run() {
					mTv.sendPowerOff();
				}
			}, time);
		}
	}

	public void startDpmsTimer() {
		MtkLog.d(TAG, "<Start DPMS Timer>");
		mConfigManager.setValue(MenuConfigManager.WAKEUP_VGA,
				ConfigType.WAKE_UP_SETUP_VGA_VALIDE);
		PowerOffTimer dpmsTimer = mTvTimerManager.getPowerOffTimer("dpmsTimer");
		dpmsTimer.setTimer(10000);// 10 seconds.
		dpmsTimer.start();
	}

	public void cancelDpmsTimer() {
		PowerOffTimer dpmsTimer = mTvTimerManager.getPowerOffTimer("dpmsTimer");
		dpmsTimer.cancel();
	}

	/**
	 * return timer's time left in minutes. this function if write for menu
	 * timer.
	 * 
	 * @param timer_name
	 * @return
	 */
	public long iPowerOffMinutesLeft(String timer_name) {
		long time = 0;

		PowerOffTimer mPowerOffTimer = mTvTimerManager
				.getPowerOffTimer(timer_name);

		if (mPowerOffTimer != null) {
			time = mPowerOffTimer.getTimeLeft();
			if (time % 60000 > 0) {
				time = time / 60000 + 1;// milliseconds to minutes
			} else {
				time = time / 60000;
			}
		}

		return time;
	}

	/**
	 * effect(picture, audio, aspect)
	 */

	public interface iEffectChangeListener {
		void effectChanged(View mView, int value);
	}

	/**
	 * channel
	 */
	/**
	 * @deprecated
	 */
	public List<TVChannel> iGetChannelList() {
		return mTvChannelManager.getChannels();
	}

	public int iGetChannelLength() {
		return mTvChannelManager.getChannelLength();
	}

	public boolean iHasChannel() {
		if (mTvChannelManager.getChannelLength() > 0) {
			return true;
		} else {
			return false;
		}
	}

	public boolean isCurrentChannelPAL() {
		TVChannel mChannel = mTvChannelSelector.getCurrentChannel();

		if (mChannel != null) {
			return 	mChannel.isAnalog();
		}
		return false;
	}

	public boolean isCurrentChannelDTV() {
		TVChannel mChannel = mTvChannelSelector.getCurrentChannel();

		if (mChannel != null) {
			return mChannel.isDTV();
		}
		return false;
	}

	public boolean isCurrentChannelRadio() {
		TVChannel mChannel = mTvChannelSelector.getCurrentChannel();

		if (mChannel != null) {
			return mChannel.isRadio();
		}
		return false;
	}

	/**
	 * favorite channel up and down filter
	 */
	public void iSelectFavChanUp() {
		mTvChannelSelector.favoriteChannelUp();
	}

	public void iSelectFavChanDown() {
		mTvChannelSelector.favoriteChannelDown();
	}

	public boolean iSelectChannel(TVChannel ch) {
		if (ch == null) {
			return false;
		}
//		if (!ch.isValid()) {
//			return false;
//		}
		if (ch.equals(iGetCurrentChannel()) && isCurrentSourceTv()) {
			return false;
		}

		iSetSourcetoTv();
		startShiftChannelTime = System.currentTimeMillis();
		mTvChannelSelector.select(ch);
		return true;
	}

	public boolean iSelectChannel(short num) {
		TVChannel ch = null;
		if (num >= 0) {
			ch = mTvChannelManager.findChannelByNumber(num);
			return iSelectChannel(ch);
		} else {
			return false;
		}
	}
	
	public boolean hasSelectChannel(short num) {
		TVChannel ch = null;
		if (num >= 0) {
			ch = mTvChannelManager.findChannelByNumber(num);
			return ch==null?false:true;
		} else {
			return false;
		}
	}

	public TVChannel iGetCurrentChannel() {
		return mTvChannelSelector.getCurrentChannel();
	}

	/**
	 * Select channel.
	 * 
	 * @param Type
	 *            0,1,2
	 */
	public void iSetChannel(int Type) {
		iSetSourcetoTv();
		startShiftChannelTime = System.currentTimeMillis();

		switch (Type) {
		case CHANNEL_UP:
			mTvChannelSelector.channelUp();
			break;
		case CHANNEL_DOWN:
			mTvChannelSelector.channelDown();
			break;
		case CHANNEL_PRE:
			mTvChannelSelector.selectPrev();
			break;
		default:
			break;
		}

	};

	private final static int RADIO_TYPE = 1;

	/**
	 * 
	 * @param channelType
	 *            ANALOGUE_TYPE,DIGITAL_TYPE,RADIO_TYPE
	 * @return channel {@link List} List<TVChannel>
	 */
	public List<TVChannel> iGetChannelList(int channelType) {
		List<TVChannel> channelList = null;
		if (channelType == RADIO_TYPE) {
			channelList = mTvChannelManager.getChannels(0,
					new ChannelFilter() {

						public boolean filter(TVChannel channel) {
							if (channel.isRadio()) {
								return true;
							}
							return false;
						}
					});

		} else {
			channelList = mTvChannelManager.getChannels();// get all channels.
		}
		return channelList;
	}

	public List<TVChannel> iGetFavoriteList() {
		return mTvChannelManager.getChannels(0,
				ChannelFilter.TVFavoriteFilter);
	}

	/**
	 * return true: add a current TVchannel into FavoriteList, return false:
	 * remove one or didn't get current channel
	 * 
	 * @deprecated
	 */
	public boolean isSetCurrentChannelFavorite() {
		TVChannel mTVChannel = mTvChannelSelector.getCurrentChannel();
		return isSetChannelFavorite(mTVChannel);
	}

	public boolean isSetChannelFavorite(TVChannel mTvChannel) {
		if (mTvChannel == null) {
			return false;
		} else {
			if (mTvChannel.isFavorite()) {
				mTvChannel.setFavorite(false);
				mTvChannelManager.flush();
				return false;
			} else {
				mTvChannel.setFavorite(true);
				mTvChannelManager.flush();
				return true;
			}
		}
	}

	public void isSetCurrentChannelFavorite(View mView,
			FavoriteListListener mListener) {
		if (iHasChannel()) {
			TVChannel mTVChannel = mTvChannelSelector.getCurrentChannel();
			if (mTVChannel != null) {
				isSetChannelFavorite(mView, mListener, mTVChannel);
				mListener.updateFavoriteList();
			}
		}
	}

	public void isSetChannelFavorite(View mView,
			FavoriteListListener mListener, TVChannel mTvChannel) {
		if (mTvChannel != null) {
			if (mTvChannel.isFavorite()) {
				mTvChannel.setFavorite(false);
			} else {
				mTvChannel.setFavorite(true);
			}
			mTvChannelManager.flush();
			// mTvChannel.flush();

			mListener.updateFavoriteList();
		}
	}

	public interface FavoriteListListener {
		void updateFavoriteList();
	}

	ChannelsChangedListener mChannelListChangedListener = new ChannelsChangedListener() {
		public void onChanged() {
			if (mClcListener != null) {
				mClcListener.updateChannelList();
			}
		}
	};

	public interface IChannelListChangedListener {
		public void updateChannelList();
	}

	public void iRegisteClcListener(IChannelListChangedListener mListener) {
		mClcListener = mListener;
	}

	TVChannelSelector.SelectorListener mTvSeclectorListener = new TVChannelSelector.SelectorListener() {

		public void onChannelSelect(TVChannel ch) {
			// TODO Auto-generated method stub
			if (mSelListeners != null && mSelListeners.size() > 0) {
				endShiftChannelTime = System.currentTimeMillis();
				MtkLog.v(TAG, "*************start time "
						+ startShiftChannelTime);
				MtkLog.v(TAG, "*************end time " + endShiftChannelTime);
				MtkLog.v(TAG, "*************Shift channel cost "
						+ (endShiftChannelTime - startShiftChannelTime)
						+ "millsecond****************************************");
				for (IChannelSelectorListener mSelListener : mSelListeners) {
					mSelListener.updateChannel(ch);
				}
			}
		}

		public void onSignalChange(boolean hasSignal) {
			// TODO Auto-generated method stub
			if (mSelListeners != null && mSelListeners.size() > 0) {
				for (IChannelSelectorListener mSelListener : mSelListeners) {
					mSelListener.signalChange(hasSignal);
				}
			}
		}

		public void onBlock(TVChannel ch) {
			// TODO Auto-generated method stub
			if (mSelListeners != null && mSelListeners.size() > 0) {
				for (IChannelSelectorListener mSelListener : mSelListeners) {
					mSelListener.channelLocked();
				}
			}
		}

		public void onScramble(boolean hasScramble, String state) {
			// TODO Auto-generated method stub
			if (mSelListeners != null && mSelListeners.size() > 0) {
				for (IChannelSelectorListener mSelListener : mSelListeners) {
					mSelListener.channelScrambled(hasScramble, state);
				}
			}
		}
	};

	public interface IChannelSelectorListener {
		public void updateChannel(TVChannel ch);

		public void signalChange(boolean hasSignal);

		public void channelLocked();

		public void channelScrambled(boolean hasScramble, String state);
	}

	public void iAddChSelectListener(IChannelSelectorListener mListener) {
		if (!mSelListeners.contains(mListener)) {
			mSelListeners.add(mListener);
		}
	}

	public void iRemoveChSelectListener(IChannelSelectorListener mListener) {
		mSelListeners.remove(mListener);
	}
	
	public void iRemoveInputsourceChangeListener(
			IInputSourceChangeListener mListener) {
		mIISCListeners.remove(mListener);
	}

	/**
	 * freeze
	 */
	public void iSetFreeze(boolean isFreeze) {
		mTVInputManager.setFreeze(isFreeze);
	}

	/**
	 * mute
	 */
	public boolean iSetMute() {
		if (mTv.isMute()) {
			mTv.setAudioMute(false);
		} else {
			mTv.setAudioMute(true);
		}

		return mTv.isMute();
	}

	public void iSetMute(boolean isMute) {
		mTv.setAudioMute(isMute);
	}

	public boolean isMute() {
		return mTv.isMute();
	}

	/**
	 * when power on TV first time, should start setup wizard
	 */
	public boolean isStartSetupwizard() {
		return sp.getBoolean("firstTimePowerOn", true);
	}

	public void iSetStorageZero() {
		Editor mEditor = sp.edit();
		mEditor.putBoolean("firstTimePowerOn", true);
		mEditor.commit();
		mTv.flushMedia();
	}

	public void iSetStorageOne() {
		Editor mEditor = sp.edit();
		mEditor.putBoolean("firstTimePowerOn", false);
		mEditor.commit();
		mTv.flushMedia();
	}

	/**
	 * signal
	 */
	/**
	 * @deprecated
	 */
	public void iHasSignal(View mView, iHasSignalListener mListener) {
		boolean bTmp = false;
		bTmp = mTvChannelSelector.hasSignal();
		if (mListener != null) {
			mListener.signalChanged(mView, bTmp);
		}
	}

	/**
	 * replace iHasSignal(View mView, iHasSignalListener mListener)
	 * 
	 * @param mView
	 * @param mListener
	 */
	public void iCurrentChannelHasSignal(View mView,
			iHasSignalListener mListener) {
		boolean bTmp = false;
		bTmp = mTvChannelSelector.hasSignal();
		if (mListener != null) {
			mListener.signalChanged(mView, bTmp);
		}
	}

	/**
	 * @deprecated
	 */
	public boolean iHasSignal() {
		return mTvChannelSelector.hasSignal();
	}

	/**
	 * replace iHasSignal()
	 * 
	 * @return
	 */
	public boolean iCurrentChannelHasSignal() {
		return mTvChannelSelector.hasSignal();
	}

	public interface iHasSignalListener {
		void signalChanged(View mView, boolean hasSignal);
	}

	/**
	 * setup wizard call this function to set all configure items to default
	 * value, and delete all channels info.
	 */
	public void iResetDefault() {
		mTvChannelSelector.stop();
		mConfig.resetFactory();
		mTvChannelManager.clear();
	}

	/**
	 * parental control
	 */
	public boolean isPasswordRight(Context context, String mString) {
		String mPassword;

		mPassword = SaveValue.getInstance(context).readStrValue("password");
		if (mPassword.equals(mString)) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * when user input right password, this function should be called.
	 */
	public void iOpenLockedSourceAndChannel() {
		// TODO open locked source or Channel.
		mTvOutput = mTVInputManager.getDefaultOutput();
		mTv.unblock(mTvOutput.getName());
	}

	/**
	 * judge current channel is blocked or not.
	 * 
	 * @return
	 */
	public boolean isCurrentChannelBlocked() {
		TVChannel ch = mTvChannelSelector.getCurrentChannel();
		if (ch != null) {
			return ch.isBlocked();
		} else {
			return false;
		}
	}

	/**
	 * user unlocked this channel.
	 * 
	 * @return
	 */
//	public boolean isCurrentChannelUserBlocked() {
//		TVChannel ch = mTvChannelSelector.getCurrentChannel();
//		if (ch != null) {
//			return !ch.isBlocked();
//		} else {
//			return false;
//		}
//	}

	public static void setColorKey(boolean enable) {
		MtkLog.d(TAG, "------- set color key: " + enable);
		mTv.setColorKey(enable, 0xfff74dff);
	}

	public void changeDTVSource() {
		MtkLog.d(TAG, "------- change input source to dtv --------- ");
		mTVInputManager.changeInputSource("dtv");
	}

	public interface effectChangeListener {
		void effectChanged(View mView, String value);
	}

	private String getScrambleState() {
		return mTvChannelSelector.getScrambleState();
	}

	public boolean isAudioScrambled() {
		String state = getScrambleState();
		if (state.equals(STATE_AUDIO_AND_VIDEO_SCRAMBLED)
				|| state.equals(STATE_AUDIO_CLEAR_VIDEO_SCRAMBLED)) {
			return true;
		}
		return false;
	}

	public boolean isVideoScrambled() {
		String state = getScrambleState();
		if (state.equals(STATE_AUDIO_AND_VIDEO_SCRAMBLED)
				|| state.equals(STATE_VIDEO_CLEAR_AUDIO_SCRAMBLED)) {
			return true;
		}
		return false;
	}

	private boolean isSubtitleShowing = false;

	public void iSetSubtitleLang(String lang) {
		mTv.setSubtitleLang(lang);
	}

	public void resetSubtitle() {
		if(!"stop".equals(iGetCurrentSubtitleLangAddIndex())){
			NavIntegration.subRepeat = 0;
			NavIntegration.subTittleTag = 0;
			isSubtitleShowing = false;
		}
	}

	public String iGetCurrentSubtitleLangAddIndex() {
		MtkLog.d("foxy",
				"iGetCurrentSubtitleLangAddIndex mTv.getCurSubtitleLang()"
						+ mTv.getCurSubtitleLang());
		if (isSubtitleShowing) {
			String mCurSubtitleLang = mTv.getCurSubtitleLang();
			subRepeat = iGetCurrentSubtitleIndex();
			if (subRepeat != 0) {
				
				return mCurSubtitleLang + subRepeat;
			}
			return mCurSubtitleLang;
		} else {
			return "stop";
		}
	}

	public String iGetCurrentSubtitleLang() {
		if (isSubtitleShowing) {
			return mTv.getCurSubtitleLang();
		} else {
			return "stop";
		}
	}

	public void iSetNextSubtitleLang() {
		String totalSubtitleLang = mTv.getSubtitleLang();
		if (null == totalSubtitleLang  || totalSubtitleLang.length() == 0 ) {
			MtkLog.v("foxy", "null in next sub");
			return;
		}
		int str = 0;
		if (totalSubtitleLang.length() > 0) {
			subtitle = new String[totalSubtitleLang.length() / SUBTITTLE_LEN];
			while (str < totalSubtitleLang.length()) {
				subtitle[str / SUBTITTLE_LEN] = totalSubtitleLang.substring(
						str, str + SUBTITTLE_LEN);
				str += SUBTITTLE_LEN;
			}
		} 
		NavIntegration.subTittleTag++;
		if (NavIntegration.subTittleTag > subtitle.length) {
			if (isSubtitleShowing) {
				mTv.syncStopSubtitleStream();
				isSubtitleShowing = false;
			} else {
				NavIntegration.subTittleTag = 1;
				mTv.setSubtitleLang(subtitle[0]);
					isSubtitleShowing = true;
				}
			} else {
				MtkLog.d("foxy", "NavIntegration.subTittleTag :"
						+ NavIntegration.subTittleTag);
				mTv.setSubtitleLang(subtitle[NavIntegration.subTittleTag - 1]);
				isSubtitleShowing = true;
			}
		}

	public boolean isDTVHasChannels() {
		List<TVChannel> dtvChannellist = mTvChannelManager
				.getChannelList(TVChannelManager.LIST_DVB_NAME);
		if (dtvChannellist != null && dtvChannellist.size() > 0) {
			return true;
		} else {
			return false;
		}
	}

	public boolean aspectRatioCanSet(){
		mTvOutput = mTVInputManager.getDefaultOutput();
		return mTvOutput.aspectRatioCanSet();
	}
	
	public boolean isCapLogo(){
		TVStorage tvStorage = mTv.getStorage();
		return tvStorage.isCaptureLogo();
	}

	public boolean canDoCapLogo() {
		if (iCurrentInputSourceHasSignal() && !isCurrentChannelRadio()
				&& !isVideoScrambled() && isCapLogo()) {
			return   !isCurrentChannelBlocked();
		} else {
			return false;
		}
	}
	public int iGetCurrentSubtitleIndex() {
		String totalSubtitleLang = mTv.getSubtitleLang();
		String mCurSubtitleLang = mTv.getCurSubtitleLang();
		if(null == totalSubtitleLang  || totalSubtitleLang.length() == 0 
			|| null == mCurSubtitleLang || mCurSubtitleLang.length() == 0) {
			return 0;
		}
		subtitle = new String[totalSubtitleLang.length() / SUBTITTLE_LEN];
		int str = 0;
		int repeat = 0;
		int allRepeat = 0;
		if (totalSubtitleLang.length() > 0) {
			while (str < totalSubtitleLang.length()) {
				subtitle[str / SUBTITTLE_LEN] = totalSubtitleLang.substring(
						str, str + SUBTITTLE_LEN);
				str += SUBTITTLE_LEN;
			}
			MtkLog.i("foxy", "subTittleTag: " + NavIntegration.subTittleTag);
			for (int i = 0; i < subtitle.length; i++) {
				if (subtitle[i]
						.equals(subtitle[NavIntegration.subTittleTag - 1])) {
					allRepeat++;
				}
			}
			if (allRepeat > 1) {
				for (int i = 0; i < NavIntegration.subTittleTag; i++) {
					if (subtitle[i]
							.equals(subtitle[NavIntegration.subTittleTag - 1])) {
						repeat++;
					}
				}
			}
		}
		
		return repeat;
	}
}
