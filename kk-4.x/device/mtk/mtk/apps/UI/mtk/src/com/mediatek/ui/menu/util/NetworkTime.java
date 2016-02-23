package com.mediatek.ui.menu.util;

import java.io.IOException;
import java.io.InputStream;
import java.util.Calendar;
import java.util.Date;

import com.mediatek.ui.util.MtkLog;

import android.content.Context;
import android.net.SntpClient;
import android.os.AsyncTask;
import android.os.SystemClock;

public class NetworkTime {

	private static final String TAG = "DateTimeSettings.Sntp";
	private static NetworkTime netTime;
	private Context mContext;
	SntpTask task;

	public static NetworkTime getInstance(Context context) {
		if (netTime == null) {
			netTime = new NetworkTime(context);
		}
		return netTime;
	}

	private NetworkTime(Context context) {
		mContext = context;
	}

	/**
	 * get time from network
	 * 
	 * @return the network time
	 */
	public long getNetTime() {

		task = new SntpTask();
		task.execute();
		Date mDate = new Date(System.currentTimeMillis());
		MtkLog.v(TAG, "current time:" + mDate.toString());
		return System.currentTimeMillis();
	}

	/**
	 * cancel the task to get time from network
	 */
	public void cancelNetTimeSync() {
		if (task != null) {
			task.cancel(true);
		}

	}

	class SntpTask extends AsyncTask<Void, Integer, Long> {

		private static final int TIME_OUT = 10 * 1000;

		@Override
		protected Long doInBackground(Void... args) {
			MtkLog.v(TAG, "doInBackground");
			String[] urls = new String[] { "time.nist.gov",
					"3.cn.pool.ntp.org", "2.asia.pool.ntp.org", "nw.nist.gov" };
			long ntp = System.currentTimeMillis();
			SntpClient client = new SntpClient();
			for (String url : urls) {
				MtkLog.v(TAG, "get ntp from " + url);
				if (url == null) {
					continue;
				}
				if (client.requestTime(url, TIME_OUT)) {
					ntp = client.getNtpTime();
					break;
				}
			}
			return ntp;
		}

		@Override
		protected void onPreExecute() {
			super.onPreExecute();
		}

		@Override
		protected void onPostExecute(Long aLong) {
			super.onPostExecute(aLong);
			MtkLog.v(TAG, "onPostExecute time" + aLong);
			onSet(aLong);
		}

		@Override
		protected void onProgressUpdate(Integer... values) {
			super.onProgressUpdate(values);
		}

		@Override
		protected void onCancelled() {
			super.onCancelled();
		}
	}

	private void onSet(long time) {
		Calendar c = Calendar.getInstance();
		c.setTimeInMillis(time);

//		SystemClock.setCurrentTimeMillis(time);
		updateTimeAndDateDisplay();
	}

	private void updateTimeAndDateDisplay() {
		Date now = Calendar.getInstance().getTime();
		MtkLog.v(TAG, "Now time:" + now.toString());
	}
}
