package com.mediatek.ui.util;

import java.util.Date;

import android.content.Context;

import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.ui.menu.commonview.MTKPowerManager;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;

public class POSTManager {
	private static final String TAG = "POSTManager";
	private static POSTManager instance = new POSTManager();
	private static SaveValue mSaveValue;
	private static Context mContext;

	private POSTManager() {

	}

	private POSTManager(Context context) {
		this();
	}

	public static POSTManager getInstance(Context context) {
		mContext = context;
		mSaveValue = SaveValue.getInstance(context);
		return instance;
	}

	public void checkPowOffTimer() {
		int value = mSaveValue.readValue(MenuConfigManager.POWER_OFF_TIMER);
		MtkLog.d(TAG, "checkPowOffTimer----value---->" + value);

		if (value == 1) {
			String powOffTime = mSaveValue
					.readStrValue(MenuConfigManager.TIMER2);

			if (DateFormatUtil.checkPowOffTimerInvalid(powOffTime)) {
				MtkLog.d(TAG, "poweroff-timer--is invalid->");
				mSaveValue.saveValue(MenuConfigManager.POWER_OFF_TIMER, 0);
				mSaveValue.saveStrValue(MenuConfigManager.TIMER2, "00:00:00");
			} else {
				MtkLog.d(TAG, "poweroff-timer--is once->");
				Date temp = DateFormatUtil.getDate(powOffTime);
				MTKPowerManager.getInstance(mContext).timePowerOff(
						MenuConfigManager.POWER_OFF_TIMER, temp);
			}
			return;
		}
		if (value == 2) {
			MtkLog.d(TAG, "poweroff-timer--on->");
			String powOffTime = mSaveValue
					.readStrValue(MenuConfigManager.TIMER2);
			Date temp = DateFormatUtil.getDate(powOffTime);
			MTKPowerManager.getInstance(mContext).timePowerOff(
					MenuConfigManager.POWER_OFF_TIMER, temp);
		}
	}

	public void checkPowOnTimer() {
		int value = mSaveValue.readValue(MenuConfigManager.POWER_ON_TIMER);
		MtkLog.d(TAG, "checkPowOnTimer----value---->" + value);
		if (value == 1) {
			String powOnTime = mSaveValue
					.readStrValue(MenuConfigManager.TIMER1);

			if (DateFormatUtil.checkPowOnTimerInvalid(powOnTime)) {
				MtkLog.d(TAG, "powOnTime---is invalid->");
				playPowerOnChannel();
				mSaveValue.saveValue(MenuConfigManager.POWER_ON_TIMER, 0);
				mSaveValue.saveStrValue(MenuConfigManager.TIMER1, "00:00:00");
			} else {
				MtkLog.d(TAG, "powOnTime---is once->");
				MTKPowerManager.getInstance(mContext).updatePowerOn(powOnTime);
			}
		}
		if (value == 2) {
			String powOnTime = mSaveValue
					.readStrValue(MenuConfigManager.TIMER1);
			if (DateFormatUtil.checkPowOnTimerInvalid(powOnTime)) {
				playPowerOnChannel();
				MTKPowerManager.getInstance(mContext).updatePowerOn(powOnTime);
			}
		}
	}

	public void checkSleepTime() {
		int value = mSaveValue.readValue(MenuConfigManager.SLEEP_TIMER);
		if (value != 0) {
			mSaveValue.saveValue(MenuConfigManager.SLEEP_TIMER, 0);
		}
	}

	public void checkAutoSleep() {
		int value = mSaveValue.readValue(MenuConfigManager.AUTO_SLEEP);
		if (value != 0) {
			mSaveValue.saveValue(MenuConfigManager.AUTO_SLEEP, 0);
		}
	}

	/*
	 * User-defined boot channel playback
	 */
	public void playPowerOnChannel() {
		TVTimerManager tvTimerManager = TVContent.getInstance(mContext)
				.getTimerManager();
		short mPowerChannelNum = tvTimerManager.getPowerOnChannel();
		TVChannelSelector tvChannelSelector = TVContent.getInstance(mContext)
				.getChannelSelector();
		if (SaveValue.getInstance(mContext).readValue(
				MenuConfigManager.POWER_ON_CH_MODE) == 0) {
			if(tvChannelSelector.getCurrentChannel() != null){
				tvChannelSelector.select(tvChannelSelector.getCurrentChannel());
			}
		} else {
			tvChannelSelector.select(mPowerChannelNum);
		}

	}

}
