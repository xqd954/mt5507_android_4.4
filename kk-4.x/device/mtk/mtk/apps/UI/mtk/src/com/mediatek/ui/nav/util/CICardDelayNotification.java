package com.mediatek.ui.nav.util;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.ui.util.MtkLog;

import android.os.Handler;
import android.util.Log;

public class CICardDelayNotification {
	private static final String TAG = "CICardDelayNotification";
	private List<Runnable> mDelayNotificationList = new ArrayList<Runnable>();
	private Handler mHandler;
	private long delayMillis = 2000;
	private boolean mPushToQueue = false;

	public boolean isPushToQueue() {
		return mPushToQueue;
	}

	public synchronized void  setPushToQueue(boolean mPushToQueue) {
		MtkLog.d(TAG,"camSelectorListener-----------setPushToQueue------>"+mPushToQueue);
		this.mPushToQueue = mPushToQueue;
	}

	public  synchronized void setDelayMillis(long delayMillis) {
		this.delayMillis = delayMillis;
	}

	private static CICardDelayNotification mCICardDelayNotification;

	private CICardDelayNotification() {
		mHandler = new Handler();
	}

	public static synchronized CICardDelayNotification getInstance(
			) {
		if (mCICardDelayNotification == null) {
			mCICardDelayNotification = new CICardDelayNotification();
		}
		return mCICardDelayNotification;
	}

	public void runDelayNotifications() {
		for (Runnable run : mDelayNotificationList) {
			if ("com.mediatek.ui.menu.commonview.CIMainView.DelayMMIMenuReceived".equals(run.getClass().getName())) {
				mHandler.postDelayed(run, delayMillis);
			} else {
				mHandler.post(run);
			}
		}
		mDelayNotificationList.clear();
	}
	public synchronized void  clearDelayNotifications(){
		mDelayNotificationList.clear();
	}
	public synchronized void addDelayNotifications(Runnable runnable){
		MtkLog.d(TAG,"addDelayNotifications--------------------->"+runnable.getClass().getName());
		mDelayNotificationList.add(runnable);
	}
}
