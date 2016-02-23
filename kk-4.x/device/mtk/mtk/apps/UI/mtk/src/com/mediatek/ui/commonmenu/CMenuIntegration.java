package com.mediatek.ui.commonmenu;

import android.content.Context;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVOptionRange;

public class CMenuIntegration {
	private static final String TAG = "CMenuIntegration";

	public final static String CONFIG_VOLUME = "config_volume";
	public final static String CONFIG_PICTURE_EFFECT = ConfigType.CFG_PICTURE_MODE;
	public final static String CONFIG_SOUND_EFFECT = ConfigType.CFG_EQUALIZE;
	public final static String CONFIG_TRACK = "config_track";
	private static CMenuIntegration instance;
	private TVContent tv;
	private TVConfigurer mConfig;
	private TVOptionRange<Integer> volOption;
	//private LogicManager mLogicManager;

	@SuppressWarnings("unchecked")
	private CMenuIntegration(Context context) {
		tv = TVContent.getInstance(context);
		mConfig = tv.getConfigurer();
		volOption = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME);
	}

	synchronized static public CMenuIntegration getInstance(Context context) {
		if (instance == null) {
			instance = new CMenuIntegration(context);
		}

		return instance;
	}

	/**
	 * volume
	 */

	public  int getCurrentVolume() {
		if(null == volOption){
			return 0;
		}
		return volOption.get();
	}

	public void setVolume(int step) {
		int max = volOption.getMax();
		int min = volOption.getMin();

		int current = volOption.get() + step;

		if (current > max) {
			current = max;
		} else if (current < min) {
			current = min;
		}

		volOption.set(current);
	}

	/**
	 * p.effect
	 */

	public int getPictureEffect() {
		return getCurrentConfigEffect(ConfigType.CFG_PICTURE_MODE);
	}

	public void setNextPictureEffect() {
		setNextConfigEffect(ConfigType.CFG_PICTURE_MODE);
	}

	public void setPrePictureEffect() {
		setPreConfigEffect(ConfigType.CFG_PICTURE_MODE);
	}

	/**
	 * S.effect
	 */

	public int getSoundEffect() {
		return getCurrentConfigEffect(ConfigType.CFG_EQUALIZE);
	}

	public void setNextSoundEffect() {
		 setNextConfigEffect(ConfigType.CFG_EQUALIZE);
	}

	public void setPreSoundEffect() {
		 setPreConfigEffect(ConfigType.CFG_EQUALIZE);
	}
	
	/**
	 * Use to change picture and sound effect.
	 * 
	 * @param option
	 * @param offsetEffect
	 *            is offset of the current effect.
	 */
	private void changeEffect(TVOptionRange<Integer> option, int offsetEffect) {
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

	@SuppressWarnings("unchecked")
	private void setNextConfigEffect(String configType) {
		TVOptionRange<Integer> Option = (TVOptionRange<Integer>) mConfig
				.getOption(configType);

		if(Option != null){
				changeEffect(Option, 1);
			}
	}
	
	@SuppressWarnings("unchecked")
	private void setPreConfigEffect(String configType) {
		TVOptionRange<Integer> Option = (TVOptionRange<Integer>) mConfig
				.getOption(configType);
		if(Option != null){
				changeEffect(Option, -1);
			}
	}
	
	@SuppressWarnings("unchecked")
	private int getCurrentConfigEffect(String configType) {
		TVOptionRange<Integer> mOption = (TVOptionRange<Integer>) mConfig
				.getOption(configType);
		if(mOption != null){
				return mOption.get();
			}else{
				return 0;
			}
			
	}
}
