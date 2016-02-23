package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import android.text.format.Time;

import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;

public class ListenTime implements Runnable {
	private Handler mHandler;
	private Context mContext;
	private SaveValue saveV;
	private boolean flag = false;

	public ListenTime(Context mContext) {
		// TODO Auto-generated constructor stub
		this.mContext = mContext;
		saveV = SaveValue.getInstance(mContext);
		new Thread(this).start();
	}

	public void shutDown() {
		mHandler = new Handler() {
			public void handleMessage(Message msg) {
				switch (msg.what) {
				case 1:
					if (saveV.readValue(MenuConfigManager.POWER_OFF_TIMER) == 1)
						saveV.saveValue(MenuConfigManager.POWER_OFF_TIMER, 0);
					// PowerOffManager powerOff = PowerManager
					// powerOff.timePowerOff("time Power Off");
				}
			}
		};
	}

	private Time time = new Time();
	private static Time timeModify;
	private int hour = 0;
	private int minute = 0;
	private int second = 0;

	public synchronized Time getTime() {
		time.setToNow();
		return time;
	}

	public synchronized void getDetailTime() {
		timeModify = getTime();
		hour = timeModify.hour;
		minute = timeModify.minute;
		second = timeModify.second;
	}

	public void run() {
		// TODO Auto-generated method stub

		SaveValue saveValue = SaveValue.getInstance(mContext);
		try {
			do {

				getDetailTime();
				// MtkLog.v("test", "time--------" + time);
				String hourStr = hour + "";
				String minuteStr = minute + "";
				String secondStr = second + "";
				if (hour < 10) {
					hourStr = "0" + hourStr;
				}
				if (minute < 10) {
					minuteStr = "0" + minuteStr;
				}
				if (second < 10) {
					secondStr = "0" + secondStr;
				}
				Thread.sleep(1000);
				Message m = new Message();
				flag = saveV.readStrValue(MenuConfigManager.TIMER2).equals(
						hourStr + ":" + minuteStr + ":" + secondStr);
				if (flag) {
					m.what = 1;
					ListenTime.this.mHandler.sendMessage(m);
				}
			} while (!flag);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
