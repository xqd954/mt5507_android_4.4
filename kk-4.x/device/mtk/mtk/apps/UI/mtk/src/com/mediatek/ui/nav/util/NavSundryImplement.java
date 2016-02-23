package com.mediatek.ui.nav.util;

import java.util.Hashtable;

import android.content.Context;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcm.TVStorage;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcm.TVTimerManager.PowerOffTimer;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.tvcommon.TVOutputCommon.AudioUpdated;
import com.mediatek.tvcommon.TVOutputCommon.MTSOption;
import com.mediatek.ui.R;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenModeConfig;

/**
 * 
 * @author MTK40530
 * 
 */

public class NavSundryImplement {
	private static final String TAG = "NavSundryImplement";

	private static NavSundryImplement instance;
	private static TVContent tv;
	private static TVConfigurer cf;
	private ScreenModeConfig mScreenModeConfig;

	private static Context mContext;

	private int curMtsIndex = 1;
	private String[] languageTvcmName;
	private String[] languageModeName;
	private PippopUiLogic mLogic;
	private boolean avoidChangeScreenMode = false;//exit pop mode if  change screen mode or not

	private NavSundryImplement(Context context) {
		tv = TVContent.getInstance(context);
		cf = tv.getConfigurer();
		mLogic = PippopUiLogic.getInstance(context);
		mContext = context;
		mScreenModeConfig = ScreenModeConfig.getInstance(context);
		languageTvcmName = mContext.getResources().getStringArray(
				R.array.nav_audio_tvcm_strings);		
	}

	public static synchronized NavSundryImplement getInsNavSundryImplement(
			Context context) {
		if (instance == null) {
			instance = new NavSundryImplement(context);
		}
		return instance;
	}

	/**
	 * Use to change picture and sound effect.
	 * 
	 * @param option
	 * @param offsetEffect
	 *            is offset of the current effect.
	 */
	private void changeEffect(TVOptionRange<Integer> option, int offsetEffect) {
		if(null != option){
		int current = option.get();
		int max = option.getMax();
		int min = option.getMin();

		if (current + offsetEffect > max) {
			option.set(min);
		 } else if (current + offsetEffect < min) {
			option.set(max);
		 } else {
			option.set(current + offsetEffect);
		 }
		}
	}

	@SuppressWarnings("unchecked")
	public void setNextConfigEffect(String configType) {
		TVOptionRange<Integer> Option = (TVOptionRange<Integer>) cf
				.getOption(configType);
		changeEffect(Option, 1);
	}

	@SuppressWarnings("unchecked")
	public int getCurrentConfigEffect(String configType) {
		TVOptionRange<Integer> mOption = (TVOptionRange<Integer>) cf
				.getOption(configType);
				if(null != mOption){
				  return mOption.get();
				}
		   return 0;
	}

	@SuppressWarnings("unchecked")
	public void setNextAspectEffect() {
		TVOptionRange<Integer> aspOption = (TVOptionRange<Integer>) cf
				.getOption(ConfigType.CFG_SCREEN_MODE);

		int next = mScreenModeConfig.getNextScreen();
		if ((mLogic.isPopState() || (mLogic.isPipState() && ("sub")
				.equals(mLogic.getOutputFocus())))
				&& ConfigType.SCREEN_MODE_DOT_BY_DOT == next) {
			next += 1;
		}
		if (null!=aspOption && next <= aspOption.getMax()) {
			aspOption.set(next);
		} else if(null!=aspOption){
			aspOption.set(aspOption.getMin());
		}
	}
	
	//set screen mode for pop
	public void setScreenModeForPOP() {
		TVOptionRange<Integer> aspOption = (TVOptionRange<Integer>) cf
				.getOption(ConfigType.CFG_SCREEN_MODE);
		int mode = mScreenModeConfig.getCurrentScreen();
			if (ConfigType.SCREEN_MODE_DOT_BY_DOT == mode) {
				//aspOption.set(ConfigType.SCREEN_MODE_CUSTOM_DEF_0);
				avoidChangeScreenMode = true;
			}
	}
	//set screen mode for exit pop
	public void setScreenModeExitPOP(){
		TVOptionRange<Integer> aspOption = (TVOptionRange<Integer>) cf
				.getOption(ConfigType.CFG_SCREEN_MODE);
		int prev = mScreenModeConfig.getPreScreen();
		if (ConfigType.SCREEN_MODE_DOT_BY_DOT == prev && avoidChangeScreenMode == true) {
			//aspOption.set(ConfigType.SCREEN_MODE_DOT_BY_DOT);
			avoidChangeScreenMode = false;
		}
	}

	/**
	 * set picture freeze.
	 * 
	 * @param isFreeze
	 */
	public void setFreeze(boolean isFreeze) {
		TVInputManager mInputManager = tv.getInputManager();
		TVOutput mOutput = mInputManager.getDefaultOutput();
		mOutput.setFreeze(isFreeze);
	}

	public boolean isFreeze() {
		TVInputManager mInputManager = tv.getInputManager();
		TVOutput mOutput = mInputManager.getDefaultOutput();
		return mOutput.isFreeze();
	}
	
	public boolean isEnableFreeze(){
		TVInputManager mInputManager = tv.getInputManager();
		TVOutput mOutput = mInputManager.getDefaultOutput();
		return mOutput.isEnableFreeze();
	}
	
	/**
	 * for sleep
	 */
	private static final int SLEEP_MIN = 0;
	private static final int SLEEP_MAX = 8;
	private static final String SLEEPTIMER = "timetosleep";

	public int toSleepInMinutes() {
		int time = 0;
		TVTimerManager mTvTimerManager = tv.getTimerManager();
		PowerOffTimer mPowerOffTimer = mTvTimerManager
				.getPowerOffTimer(SLEEPTIMER);
     if(null != mPowerOffTimer){
     	time = getCurrentSleepGrade(mPowerOffTimer);
    }
		MtkLog.i(TAG, "~~~~~~~~~~~~~time grade:"+time);
		if (time < SLEEP_MAX) {
			time++;
		} else {
			time = 0;
		}

		switch (time) {
		case SLEEP_MIN:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			time = time * 10;
			break;
		case 7:
			time = 90;
			break;
		case SLEEP_MAX:
			time = 120;
			break;
		default:
			time = 120;
			break;
		}
		
  if(null != mPowerOffTimer){
		mPowerOffTimer.cancel();
  
  
		if (time != 0) {
			mPowerOffTimer.setTimer(time * 60000);// minutes to milliseconds
			mPowerOffTimer.start();
		}
   }
		return time;
	}

	private int sleepMinutesLeft(PowerOffTimer poTimer) {
		long time = 0;

		if (poTimer == null) {
			return 0;
		}

		time = poTimer.getTimeLeft();
		
		MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~~~~~~~left time:"+time);
		if (time % 60000 > 0) {
			time = time / 60000 + 1;// milliseconds to minutes
		} else {
			time = time / 60000;
		}
		
		return (int) time;
	}

	/**
	 * 
	 * @param poTimer
	 * @return time in grade 0 ~ 8
	 */
	private int getCurrentSleepGrade(PowerOffTimer poTimer) {
		int currentTime = 0;

		if (poTimer == null) {
			return 0;
		}

		currentTime = sleepMinutesLeft(poTimer) / 10;
		
		MtkLog.i(TAG,"~~~~~~~~~~~~~~~~~~~~~current time:"+currentTime);

		if (currentTime > 7 && currentTime <= 9) {
			currentTime = 7;
		} else if (currentTime > 9) {
			currentTime = 8;
		}

		return currentTime;
	}

	public int getSleepTimeLeftInMunites() {
		TVTimerManager mTvTimerManager = tv.getTimerManager();
		PowerOffTimer mPowerOffTimer = mTvTimerManager
				.getPowerOffTimer(SLEEPTIMER);
    if(null == mPowerOffTimer){
      return 0;
    }
		return sleepMinutesLeft(mPowerOffTimer);
	}
	
	public boolean isCurrentChannelAnalog() {
		TVChannel ch = tv.getCurrentChannel();
		
		if(ch==null){
			return false;
		}
		
		return ch.isAnalog();
	}
	
	/**
	 * mts/audio
	 */
	public int getCurrentMtsEffect() {
		TVInputManager im = tv.getInputManager();
		MTSOption mtsOption = im.getDefaultOutput().getMTSOption();
		return mtsOption.get();
	}

	/**
	 * mts/audio
	 */
	public int getCurrentMtsEffectTotalNum() {
		TVInputManager im = tv.getInputManager();
		MTSOption mtsOption = im.getDefaultOutput().getMTSOption();
		return mtsOption.getTotalNum();
	}

	public int getCurrentMtsEffecIndex() {
		TVInputManager im = tv.getInputManager();
		MTSOption mtsOption = im.getDefaultOutput().getMTSOption();		
		return mtsOption.getMtsIndex(getCurrentMtsEffect());
	}

	public void setNextMtsEffect() {
		TVInputManager im = tv.getInputManager();
		MTSOption mtsOption = im.getDefaultOutput().getMTSOption();

		int current = mtsOption.get();
		int max = mtsOption.getMax();
		int min = mtsOption.getMin();

		int previous = current;

		do {
			current++;
			if (current > max) {
				current = min;
			}
			if (mtsOption.canSet(current)) {
				mtsOption.set(current);
				break;
			}
		} while (current != previous);
	}

	public String getCurrentLanguageEffect() {
		TVInputManager im = tv.getInputManager();
		TVOutput op = im.getDefaultOutput();

		Hashtable<Integer, String> curTable = op.getCurAudLang();
		
		if(curTable == null || curTable.size()<1){
			return null;
		}		
		
		String value = curTable.elements().nextElement();
		
		if("NUL".equals(value)){
			Hashtable<Integer, String> totalTable = op.getAudLangTable();
			if(null!= totalTable && totalTable.size() > 2){
//				Hashtable<Integer, String> table = new Hashtable<Integer, String>();
				String value1 = totalTable.get(1);
				String value2 = totalTable.get(2);
				if(value1 != null && value2 != null){					
					return value1 + value2;
				}else{
					return value;
				}
			}else{
				return value;
			}
		}else{
			return value;
		}		
	}

	public String getLanguageShowString(String audioTvcmString) {
		languageModeName = mContext.getResources().getStringArray(
				R.array.nav_audio_strings);
		if (null == audioTvcmString 
				|| "NON".equals(audioTvcmString) || audioTvcmString.length() == 0) {
			return mContext.getString(R.string.nav_unknown_audio_sundry);
		}

		if (audioTvcmString.length() > 3) {
			String audioFirst = audioTvcmString.substring(0, 3);
			String audioSecond = audioTvcmString.substring(3, 6);			
			if (audioFirst == null || audioSecond == null) {
				return mContext.getString(R.string.nav_unknown_audio_sundry);
			} else {
				return getLanguageShowString(audioFirst) + " + "
						+ getLanguageShowString(audioSecond);
			}
		}

		for (int i = 0; i < languageTvcmName.length; i++) {
			if (audioTvcmString.equals(languageTvcmName[i])) {
				return languageModeName[i];
			}
		}

		return mContext.getString(R.string.nav_unknown_audio_sundry);
	}

	public int getTotalLanguageEffectNum() {
		TVInputManager im = tv.getInputManager();
		TVOutput op = im.getDefaultOutput();
		Hashtable<Integer, String> audioTable = op.getAudLangTable();
		return (audioTable == null) ? 0 : audioTable.size();
	}

	public int getCurrentLanguageEffectIndex() {		
		TVInputManager im = tv.getInputManager();
		TVOutput op = im.getDefaultOutput();
		Hashtable<Integer, String> curTable = op.getCurAudLang();		
		return (curTable==null || curTable.size() < 1) ? -1 : curTable.keys().nextElement();
	}

	public void setLanguageNextEffect() {
		TVInputManager im = tv.getInputManager();
		TVOutput op = im.getDefaultOutput();

		int total = getTotalLanguageEffectNum();
		int curIdx = getCurrentLanguageEffectIndex();		
		
		if(total <= 1 || curIdx < 1 || curIdx > total){
			return;
		}
		
		if(total == curIdx){
			curIdx = 1;
		}else{
			curIdx++;
		}
		op.setAudLangByIndex(curIdx);
	}

	public void registerAudioUpdateListener(AudioUpdated update) {
		TVInputManager im = tv.getInputManager();
		TVOutput op = im.getDefaultOutput();
		op.registerAudioUpdatedListener(update);
	}

	public void removeAudioUpdateListener(AudioUpdated update) {
		TVInputManager im = tv.getInputManager();
		TVOutput op = im.getDefaultOutput();
		op.removeAudioUpdatedListener(update);
	}
	
	public boolean isCapLogo(){
		TVStorage st = tv.getStorage();
		return st.isCaptureLogo();
	}
}
