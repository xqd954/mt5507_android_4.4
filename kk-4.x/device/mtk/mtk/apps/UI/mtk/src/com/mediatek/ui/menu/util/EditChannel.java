package com.mediatek.ui.menu.util;

import java.util.List;

import android.app.AlarmManager;
import android.content.Context;
import android.os.Handler;
import android.os.RemoteException;

import com.mediatek.tvcm.TVChannelManager;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVMethods;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.commonview.MTKPowerManager;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

/**
 * Methods about channel edit
 * 
 * @author MTK40405
 * 
 */
public class EditChannel {

	/* the position of data Item about channel edit */
	public static final int FREQUENCY = 2;
	public static final int COLOR = 3;
	public static final int SOUND = 4;
	public static final int AFT = 5;
	public static final int SKIP = 7;
	private static final String TAG = "EditChannel";

	private Context mContext;
	private TVContent mTVContent;
	private TVChannelManager mTVManager;
	private TVChannelSelector mChannelSelector;
	private TVInputManager mInputManager;
	private TVOutput mOutput;
	private TVOutputCommon mOutputCommon;
	private static EditChannel mEditChannel = null;
	private final float mFineTuneStep = 0.065f;
	private float mRestoreHz = 0;

	private MenuConfigManager mcf;
	private NavIntegration mNavIntegration;

	/* store original information about color system and sound system */
	private int mOriginalColorSystem;
	private int mOriginalSoundSystem;

	/* store original information about frequency */
	private float mOriginalFrequency;

	/* the flag to judge whether channel data is stored */
	private boolean isStored = true;

	private SaveValue sv;

	/**
	 * Construct method
	 * 
	 * @param context
	 */
	private EditChannel(Context context) {
		mContext = context;
		mTVContent = TVContent.getInstance(mContext);
		mTVManager = mTVContent.getChannelManager();
		mChannelSelector = mTVContent.getChannelSelector();
		mChannelSelector
				.setDefaultFinetuneStep((int)(mFineTuneStep * 1000000));
		mInputManager = mTVContent.getInputManager();
		mOutput = mInputManager.getOutput("main");
		mOutputCommon = TVOutputCommon.getInstance(mContext);
		sv = SaveValue.getInstance(context);

		mcf = MenuConfigManager.getInstance(mContext);
		mNavIntegration = NavIntegration.getInstance(mContext);
	}

	/**
	 * get an instance of EditChannel
	 * 
	 * @param context
	 * @return
	 */
	public static synchronized EditChannel getInstance(Context context) {
		if (mEditChannel == null) {
			mEditChannel = new EditChannel(context);
		}
		return mEditChannel;
	}

	/**
	 * get restore Frequency
	 * 
	 * @return
	 */
	public float getRestoreHZ() {
		return mRestoreHz;
	}

	/**
	 * set restore Frequency
	 * 
	 * @param restoreHZ
	 */
	public void setRestoreHZ(float restoreHZ) {
		this.mRestoreHz = restoreHZ;
	}

	/**
	 * get stored flag
	 * 
	 * @return
	 */
	public boolean getStoredFlag() {
		return isStored;
	}

	/**
	 * set colorSystem and soundSystem
	 * 
	 * @param colorSystem
	 * @param soundSystem
	 */
	public void setOriginalTVSystem(int colorSystem, int soundSystem) {
		this.mOriginalColorSystem = colorSystem;
		this.mOriginalSoundSystem = soundSystem;
	}

	/**
	 * set original frequency
	 * 
	 * @param frequency
	 */
	public void setOriginalFrequency(float frequency) {
		this.mOriginalFrequency = frequency;
	}

	/**
	 * restore original sound system and color system
	 */
	public void restoreOrignalTVSystem() {
		if (!isStored) {
			TVChannel ch = mChannelSelector.getCurrentChannel();
			if(null != ch){
			ch.getColorSystemOption().set(mOriginalColorSystem);
			ch.getTvSystemOption().set(
					mOriginalSoundSystem + +ch.getTvSystemOption().getMin());
			ch.setFreq((int) (mOriginalFrequency * 1000000));
			}
			exitFinetune();
		}
	}

	/**
	 * set stored flag
	 * 
	 * @param isStored
	 */
	public void setStoredFlag(boolean isStored) {
		this.isStored = isStored;
	}

	/**
	 * Swap two channels
	 * 
	 * @param from
	 * @param to
	 */
	public void swapChannel(int from, int to) {
		List<TVChannel> list = mTVManager.getChannels();
		if(null != list){
		mTVManager.swapChannel(list.get(to), list.get(from));
		mChannelSelector.select(list.get(to));
		}
		mTVManager.flush();
	}

	/**
	 * Insert channel
	 * 
	 * @param from
	 * @param to
	 */
	public void insertChannel(int from, int to) {
		List<TVChannel> list = mTVManager.getChannels();
		if(null != list){
		if (from < to) {
			try {
					ITVCommon mItvCommon = TVCommonNative.getDefault(mContext);
					if (null != mItvCommon) {
						mItvCommon.insertChannel(list.get(from), list.get(to));
					}
			} catch (RemoteException e) {
				e.printStackTrace();
			}
		} else if (from > to) {
			mTVManager.insertChannel(list.get(from), list.get(to));
		}
		// mTVManager.insertChannel(list.get(from), list.get(to));
		mChannelSelector.select(list.get(to));
		mTVManager.flush();
		}
	}

	/**
	 * Delete a channel
	 * 
	 * @param deleteId
	 *            the position of channel located in channel list
	 * @return the channel list after finish deleting
	 */
	public List<TVChannel> deleteChannel(int deleteId) {
		List<TVChannel> list = mTVManager.getChannels();
		mTVContent.manualStop();
		if(null != list){
		mTVManager.deleteChannel(list.get(deleteId));
		}
		list = mTVManager.getChannels();
		if(null != list){
		if (list.isEmpty()) {
			// stop TV
		} else {
			mChannelSelector.select(list.get(0));
			}
		}
		mTVManager.flush();
		return list;
	}

	/**
	 * Edit TV sound system and color system for the current channel
	 * 
	 * @param selId
	 * @param value
	 */

	public void editTVEditSystem(String selId, int value) {
		TVChannel ch = mChannelSelector.getCurrentChannel();
		if(null != ch){
		if (selId.equals(MenuConfigManager.TV_CHANNEL_COLOR_SYSTEM)) {
			MtkLog.d(TAG, "Option : " + selId + "  [set value]"
					+ (value + ch.getColorSystemOption().getMin()));
			ch.getColorSystemOption().set(
					value + ch.getColorSystemOption().getMin());
			isStored = false;
		} else if (selId.equals(MenuConfigManager.TV_SOUND_SYSTEM)) {
			MtkLog.d(TAG, "min value " + ch.getTvSystemOption().getMin());
			MtkLog.d(TAG, "Option : " + selId + "  [set value]"
					+ (value + ch.getTvSystemOption().getMin()));
			ch.getTvSystemOption().set(value + ch.getTvSystemOption().getMin());
			isStored = false;
		}
	}
	}

	/**
	 * restore Fine tune
	 */
	public void restoreFineTune() {
		TVChannel ch = mChannelSelector.getCurrentChannel();
		if(null != ch){
		ch.setFreq((int)(mRestoreHz * 1000000));
		}
		mChannelSelector.finetune(0);
	}

	/**
	 * get the current channel number
	 * 
	 * @return
	 */
	public int getCurrentChannelNumber() {
		int channelNum = 0;
		TVChannel currentChannel = mChannelSelector.getCurrentChannel();
		if(null != currentChannel){
			channelNum = currentChannel.getChannelNum();
		}
		return channelNum;
	}

	/**
	 * store channel data
	 * 
	 * @param channelNumber
	 *            channel number
	 * @param channelName
	 *            channel name
	 * @param channelFrequency
	 *            channel frequency
	 * @param colorSystem
	 *            color system
	 * @param soundSystem
	 *            sound system
	 * @param aft
	 *            auto fine tune
	 * @param skip
	 *            skip flag
	 */
	public void storeChannel(String channelNumber, String channelName,
			String channelFrequency, int colorSystem, int soundSystem,
			int autoFineTune, int skip) {
		TVChannel currentChannel = mChannelSelector.getCurrentChannel();
		if (null != currentChannel) {
		currentChannel.setChannelNum(Short.valueOf(channelNumber));
		currentChannel.setChannelName(channelName);
		if (currentChannel.getTvSystemOption() != null) {
			currentChannel
					.setFreq((int)(Float.parseFloat(channelFrequency) * 1000000));
//			mChannelSelector.finetune(0);
			if(!mNavIntegration.isCurrentChannelBlocking()){
				mChannelSelector.finetune(0);
			}
			currentChannel.getColorSystemOption().set(colorSystem);
			currentChannel.getTvSystemOption().set(
					soundSystem + currentChannel.getTvSystemOption().getMin());
			try {
				currentChannel.autoFinetune(autoFineTune != 0 ? true : false);
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			}
		}

		currentChannel.setSkip(skip != 0 ? true : false);
		mTVManager.flush();
		isStored = true;
		mOriginalColorSystem = colorSystem;
		mOriginalSoundSystem = soundSystem;
		}
	}

	/**
	 * exit fine tune mode when fine tune is completed
	 */
	public void exitFinetune() {
		MtkLog.d(TAG, "++++++++ Call exit fine tune ++++++++");
		mChannelSelector.exitFinetune();
	}

	/**
	 * fine tune
	 * 
	 * @param originalMHZ
	 *            original million HZ
	 * @param isUp
	 *            judge the action of user
	 * @return million HZ after fine tune
	 */
	public float fineTune(float originalMHZ, int keyCode) {
		float original = 0;
		TVChannel channel = mNavIntegration.iGetCurrentChannel();
		if(null != channel){
			original = channel.getOriginalFreq();
		}
		if (keyCode == KeyMap.KEYCODE_DPAD_RIGHT) {
			if(originalMHZ-original/1000000>=(1.5-0.065)){
				return originalMHZ;
			}
			return fineTuneUp(originalMHZ);
		} else {
			if(original/1000000-originalMHZ>=(1.5-0.065)){
				return originalMHZ;
			}
			return fineTuneDown(originalMHZ);
		}
	}

	private float fineTuneUp(float originalMHZ) {
		float retMHZ = 0;
		retMHZ = originalMHZ + mFineTuneStep;
		mChannelSelector.finetuneHigher();
		return retMHZ;
	}

	private float fineTuneDown(float originalMHZ) {
		float retMHZ = 0;
		if (originalMHZ > mFineTuneStep) {
			retMHZ = originalMHZ - mFineTuneStep;
			mChannelSelector.finetuneLower();
		}
		return retMHZ;
	}

	/**
	 * clean channel list
	 */
	public void cleanChannelList() {
		mTVManager.clear();
	}

	/**
	 * Process KEYCODE_DPAD_DOWN/KEYCODE_DPAD_UP when focus is on channel list
	 * 
	 * @param keyCode
	 */
	public void channelUpAndDown(int keyCode) {
		ITVCommon tv = TVCommonNative.getDefault(mContext);
		if(null != tv){
		if (keyCode == KeyMap.KEYCODE_DPAD_DOWN) {
			try {
				tv.selectUp();
			} catch (RemoteException e) {
				e.printStackTrace();
			}
			// mChannelSelector.channelUp();
		} else {
			try {
				tv.selectDown();
			} catch (RemoteException e) {
				e.printStackTrace();
			}
			// mChannelSelector.channelDown();
			}
		}
		// mChannelSelector.select(nextChannel);
	}

	public void selectChannel(short index) {
		mChannelSelector.select(index);
	}
	
	public void selectChannel(TVChannel ch) {
		mChannelSelector.select(ch);
	}

	/**
	 * block selected channel
	 * 
	 * @param ch_num
	 *            the selected channel num
	 * @param blocked
	 *            true-block selected channel false-unblock
	 */
	public void blockChannel(short ch_num, boolean blocked) {
		TVChannel selChannel = mTVManager.findChannelByNumber(ch_num);
		if(null != selChannel){
		selChannel.setBlocked(blocked);
		flush();
		}
	}

	/**
	 * 
	 * @param input
	 *            the selected inpput source
	 * @param blocked
	 */
	public void blockInput(String input, boolean blocked) {
		mInputManager.block(input, blocked);
		TVMethods.getInstance(mContext).flushMedia();
	}

	public boolean isInputBlock(String input) {
		if (input == null) {
			return false;
		}
		return mInputManager.isBlock(input);
	}

	public boolean isChannelBlock(short ch_num) {
		boolean channelBlock = false;
		TVChannel selChannel = mTVManager.findChannelByNumber(ch_num);
		if(null != selChannel){
			channelBlock = selChannel.isPhysicalBlocked();
		}
		return channelBlock;
	}

	public String getCurrentInput() {
		return mInputManager.getCurrInputSource("main");
	}

	// ADD Start
//	public boolean isCurrentSourceUserBlocked() {
//		String ins = getCurrentInput();
//		if (ins != null) {
//			TVInput in = mInputManager.getInput(ins);
//			if (in != null) {
//				return in.isUsrUnblocked();
//			}
//		}
//
//		return false;
//	}

	public boolean isCurrentSourceTv() {
		String ins = mInputManager.getTypeFromInputSource(getCurrentInput());
		return TVInputManager.INPUT_TYPE_TV.equals(ins);
	}

	public boolean isCurrentSourceBlocking() {
		TVCommonManager cmManager = TVCommonManager.getInstance(mContext);
		boolean ret = cmManager.isInputSourcePhysicalBlocked(cmManager
				.getCurrentInputSource());
		TVChannel ch = cmManager.getCurrentChannel();
		if (ch != null) {
			ret = ret || ch.isBlocked();
		}
		return ret;
	}

	// ADD End
	public void setTVInput(String output) {
		String[] inputSources = mInputManager.getInputSourceArray();
		if (null != inputSources && inputSources.length > 0) {
			String input = inputSources[0];
		MtkLog.d(TAG, "Input: " + input + "   Output: " + output);
		mInputManager.changeInputSource(output, input);
		}
	}

	public void resetParental(final Context context, final Runnable runnable) {
		final String[] inputSources = mInputManager.getInputSourceArray();
		new Thread(new Runnable() {

			public void run() {
				// TODO Auto-generated method stub
				for (int i = 0; i < inputSources.length; i++) {
					blockInput(inputSources[i], false);
				}
				sv.saveStrValue("password", "1234");
				Handler handler = new Handler(context.getMainLooper());
				handler.post(runnable);
			}

		}).start();

	}

	public List<TVChannel> getChannelList() {
		return mTVManager.getChannels();
	}

	public void flush() {
		mTVManager.flush();
	}
	
	public void changeFreqById() {
	    String outputName = TVCommonManager.getInstance(mContext).getCurrentOutput();
        int winID = 0;
        if (outputName.equalsIgnoreCase("sub")) {
            winID = 1;
        }
        int turnmode = MenuConfigManager.getInstance(mContext).getDefault(MenuConfigManager.TUNER_MODE);
        String rfChannel = MenuMain.getRfchanel()+"";
        int scanmode = MenuConfigManager.getInstance(mContext).getScanDefault(MenuConfigManager.SCAN_MODE);
        MtkLog.d(TAG, "outputName " + outputName+"turnmode:"+turnmode+"rfChannel:"+rfChannel+"MenuMain.getRfchanel():"+MenuMain.getRfchanel()+"scanmode:"+scanmode+"MenuMain.getSysrate():"+MenuMain.getSysrate());
        TVMethods.getInstance(mContext).changeFreqById(winID,turnmode,rfChannel,MenuMain.getRfchanel(),scanmode,MenuMain.getSysrate());
    }

	public int getSignalLevel() {
		if(mOutputCommon != null){
		    String outputName = TVCommonManager.getInstance(mContext).getCurrentOutput();
			return mOutputCommon.getSignalLevel(outputName);
		}
		return 0;
	}
	public int getSignalQuality(){
		int ber = 0;
		if(mOutputCommon != null){
		    String outputName = TVCommonManager.getInstance(mContext).getCurrentOutput();
			ber =mOutputCommon.getSignalBer(outputName);
			MtkLog.d(TAG, "signal outputName ber"+ber+"outputName" + outputName);
		}
		final int quality;
		if(ber>=0 && ber<=20){
			quality =2;
		}else if(ber> 20 && ber <=380){
			quality =1;
		}else{
			quality =0;
		}
		MtkLog.d(TAG, "signal quality ber"+ber+"quality" + quality);
		return quality;
	}

	public void resetDefAfterClean() {
		SaveValue saveV = SaveValue.getInstance(mContext);

		mcf.setScanValue(MenuConfigManager.COLOR_SYSTEM, 0);
		mcf.setScanValue(MenuConfigManager.TV_SYSTEM, 0);

		TVConfigurer tvcfg = mTVContent.getConfigurer();
		tvcfg.resetUser();
		mNavIntegration.iSetStorageZero();
		setTVInput("main");

		saveV.saveValue(MenuConfigManager.CAPTURE_LOGO_SELECT, 3);
		saveV.saveValue(MenuConfigManager.AUTO_SLEEP, 0);
		saveV.saveValue(MenuConfigManager.SLEEP_TIMER, 0);
		MTKPowerManager powerManager = MTKPowerManager.getInstance(mContext);
		powerManager.cancelPowOffTimer("timetosleep");

		saveV.saveValue(MenuConfigManager.POWER_ON_TIMER, 0);
		saveV.saveValue(MenuConfigManager.POWER_OFF_TIMER, 0);
		saveV.saveValue(MenuConfigManager.AUTO_SYNC, 1);
		AlarmManager alarm = (AlarmManager)mContext
				.getSystemService(Context.ALARM_SERVICE);
		alarm.setTimeZone("Asia/Shanghai");
		saveV.saveBooleanValue("Zone_time", false);

		saveV.saveStrValue(MenuConfigManager.TIMER2, "00:00:00");
		saveV.saveStrValue(MenuConfigManager.TIMER1, "00:00:00");

		//		saveV.saveValue(MenuConfigManager.NETWORK_CONNECTION, 0);
	}
}
