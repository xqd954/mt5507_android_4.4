package com.mediatek.ui.nav.util;

import android.content.Context;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.ui.util.MtkLog;

public class VolumeAdjustImp {

	private static final String TAG = "VolumeAdjustImp";

	private static VolumeAdjustImp volImp;
	private Context mContext;
	TVConfigurer mConfig;

	private TVOptionRange<Integer>[] volOption = new TVOptionRange[7];
	private static final int VOlUME_TYPE_MIN = 0;
	private static final int VOlUME_TYPE_MAX = 6;
	private static int current_vol_option = VOlUME_TYPE_MIN;

	public static VolumeAdjustImp getInstance(Context context) {
		if (volImp == null) {
			volImp = new VolumeAdjustImp(context);
		}
		return volImp;
	}

	private VolumeAdjustImp(Context context) {
		this.mContext = context;

		mConfig = TVContent.getInstance(context).getConfigurer();

		volOption[0] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME);
		volOption[1] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME_SUB_WOOFER);
		volOption[2] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME_CENTER);
		volOption[3] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME_REAR_RIGHT);
		volOption[4] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME_REAR_LEFT);
		volOption[5] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME_FRONT_RIGHT);
		volOption[6] = (TVOptionRange<Integer>) mConfig
				.getOption(ConfigType.CFG_VOLUME_FRONT_LEFT);
	}

	private TVOptionRange<Integer> getCurrentOption() {
		if (current_vol_option > VOlUME_TYPE_MAX
				|| current_vol_option < VOlUME_TYPE_MIN) {
			current_vol_option = VOlUME_TYPE_MIN;
		}
		MtkLog.d(TAG, "current option value:" + current_vol_option
				+ " [option]" + volOption[current_vol_option]);
		return volOption[current_vol_option];

	}

	/**
	 * volume up/down
	 */
	public int getMax() {
		return getCurrentOption().getMax();
	}

	public int getMin() {
		return getCurrentOption().getMin();
	}

	public int get() {
		return getCurrentOption().get();
	}

	public void initVolume() {
		current_vol_option = VOlUME_TYPE_MIN;
	}

	public void setVolume(int value) {
		int max = getCurrentOption().getMax();
		int min = getCurrentOption().getMin();

		if (value >= min && value <= max) {
			getCurrentOption().set(value);
		} else if (value > max) {
			getCurrentOption().set(max);
		} else {
			getCurrentOption().set(min);
		}
	}

	/**
	 * change volume option
	 * 
	 * @param value
	 *            1:-1, 1 means next, -1 means prev value
	 */
	public void setVolumeOption(int option) {
		current_vol_option = option;

		if (current_vol_option > VOlUME_TYPE_MAX) {
			current_vol_option = VOlUME_TYPE_MIN;
		} else if (current_vol_option < VOlUME_TYPE_MIN) {
			current_vol_option = VOlUME_TYPE_MAX;
		}
	}
	
	public int getVolumeOption(){
		return current_vol_option;
	}
}
