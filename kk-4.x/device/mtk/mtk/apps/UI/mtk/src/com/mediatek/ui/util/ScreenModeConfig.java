package com.mediatek.ui.util;

import android.content.Context;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;

public class ScreenModeConfig {

	private static final String TAG = "ScreenModeConfig";

	private static ScreenModeConfig screenMode;
	private Context mContext;

	TVContent tv;
	TVConfigurer mtkCfg;
	TVOptionRange<Integer> opt;
	private int value = 0;

	public static final String scrModeNext = ConfigType.CFG_GET_NEXT_SCREEN_MODE;
	public static final String scrModePrev = ConfigType.CFG_GET_PREV_SCREEN_MODE;
	public static final String scrModeCurrent = ConfigType.CFG_GET_THIS_SCREEN_MODE;
	public static final String scrModeFirst = ConfigType.CFG_GET_FIRST_SCREEN_MODE;
	public static final String scrModeLast = ConfigType.CFG_GET_LAST_SCREEN_MODE;

	private ScreenModeConfig(Context context) {
		mContext = context;
		tv = TVContent.getInstance(context);
		mtkCfg = tv.getConfigurer();
	}

	public static synchronized ScreenModeConfig getInstance(Context context) {
		if (screenMode == null) {
			screenMode = new ScreenModeConfig(context);
		}
		return screenMode;
	}

	@SuppressWarnings("unchecked")
	public int getFirstScreen() {
		opt = (TVOptionRange<Integer>) mtkCfg.getOption(scrModeFirst);
		if(opt != null){
			value = opt.get();
		}
		MtkLog.d(TAG, "[First screen mode value]---- " + value);
		return value;
	}

	@SuppressWarnings("unchecked")
	public int getLastScreen() {
		opt = (TVOptionRange<Integer>) mtkCfg.getOption(scrModeLast);
		if(opt != null){
			value = opt.get();
		}
		MtkLog.d(TAG, "[Last screen mode value]---- " + value);
		return value;
	}

	@SuppressWarnings("unchecked")
	public int getCurrentScreen() {
		opt = (TVOptionRange<Integer>) mtkCfg.getOption(scrModeCurrent);
		if(opt != null){
			value = opt.get();
		}
		MtkLog.d(TAG, "[Current screen mode value]---- " + value);
		return value;
	}

	@SuppressWarnings("unchecked")
	public int getNextScreen() {
		opt = (TVOptionRange<Integer>) mtkCfg.getOption(scrModeNext);
		if(opt != null){
			value = opt.get();
		}
		MtkLog.d(TAG, "[Next screen mode value]---- " + value);
		if (value < 0 || value > 18) {
			MtkLog.e(TAG, "++++++++++ [" + scrModeNext + "] get wrong value: "
					+ value + "++++++++++++++");
			MtkLog.e(TAG, "++++++++++ [" + scrModeNext
					+ "] force changing value to 0  ++++++++++++++");
			return 0;
		}
		return value;
	}

	@SuppressWarnings("unchecked")
	public int getPreScreen() {
		opt = (TVOptionRange<Integer>) mtkCfg.getOption(scrModePrev);
		if(opt != null){
			value = opt.get();
		}
		if (value < 0 || value > 18) {
			MtkLog.e(TAG, "++++++++++ [" + scrModePrev + "] get wrong value: "
					+ value + "++++++++++++++");
			MtkLog.e(TAG, "++++++++++ [" + scrModePrev
					+ "] force changing value to 0  ++++++++++++++");
			return 0;
		}
		MtkLog.d(TAG, "[Previous screen mode value]---- " + value);
		return value;
	}

}
