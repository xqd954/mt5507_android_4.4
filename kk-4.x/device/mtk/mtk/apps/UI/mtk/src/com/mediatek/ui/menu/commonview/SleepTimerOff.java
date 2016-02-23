package com.mediatek.ui.menu.commonview;

import android.content.Context;

public class SleepTimerOff {
	private Context mContext;
	private MTKPowerManager powerOff;

	public SleepTimerOff(Context context) {
		this.mContext = context;
		this.powerOff = MTKPowerManager.getInstance(context);
	}

	public void shutDownSleep(int value) {
		String mTimer = "timetosleep"; 
		
		switch (value) {
		case 0:
			powerOff.cancelPowOffTimer(mTimer);
			break;
		case 1:
			powerOff.timePowerOff(10 * 60 * 1000, mTimer);
			break;
		case 2:
			powerOff.timePowerOff(20 * 60 * 1000, mTimer);
			break;
		case 3:
			powerOff.timePowerOff(30 * 60 * 1000, mTimer);
			break;
		case 4:
			powerOff.timePowerOff(40 * 60 * 1000, mTimer);
			break;
		case 5:
			powerOff.timePowerOff(50 * 60 * 1000, mTimer);
			break;
		case 6:
			powerOff.timePowerOff(60 * 60 * 1000, mTimer);
			break;
		case 7:
			powerOff.timePowerOff(90 * 60 * 1000, mTimer);
			break;
		case 8:
			powerOff.timePowerOff(120 * 60 * 1000, mTimer);
			break;
		default:
			break;
		}
	}

	public void shutDownAuto(int value) {
		switch (value) {
		case 0:
			break;
		case 1:
			powerOff.timePowerOff(60 * 60 * 1000, "Auto Sleep");
			break;
		case 2:
			powerOff.timePowerOff(2 * 60 * 60 * 1000, "Auto Sleep");
			break;
		case 3:
			powerOff.timePowerOff(3 * 60 * 60 * 1000, "Auto Sleep");
			break;
		default:
			break;
		}
	}

}
