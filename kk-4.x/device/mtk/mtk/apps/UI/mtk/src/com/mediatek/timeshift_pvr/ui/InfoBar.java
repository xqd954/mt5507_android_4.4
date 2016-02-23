/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.widget.LinearLayout.LayoutParams;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.ui.R;

import java.util.Timer;
import java.util.TimerTask;

/**
 *
 */
public class InfoBar extends PopupWindow {

	private int mDuration = 5;

	private TextView mInfo;
	private Activity mContext;

	private final int mMsgWhat = 0;
	private String mStrInfo = "";

	private Timer timer;

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub

			switch (msg.what) {
			default:
				mInfo.setText("mDuration:" + mDuration);
				break;
			}
			super.handleMessage(msg);
		}
	};

	public InfoBar(Activity context) {
		super(context.getLayoutInflater().inflate(R.layout.pvr_timeshfit_nf,
				null), LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);

		this.mContext = context;
	}

	public InfoBar(Activity context, int layoutID, int duration, String strInfo) {
		super(context.getLayoutInflater().inflate(layoutID, null),
				LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);

		this.mContext = context;
		this.mDuration = duration;

		this.mStrInfo = strInfo;
	}

	public void setInfo(String info) {
		mInfo.setText(info);
	}

	public void setDuration(int duration) {
		this.mDuration = duration;
	}

	public void show() {
		if (isShowing()) {
			timer.cancel();
		}

		showAtLocation(mContext.findViewById(Core.ROOT_VIEW),
				Gravity.CENTER, 20, 20);

		mInfo = (TextView) getContentView().findViewById(R.id.info);
		mInfo.setText(mStrInfo);

		startTimeTask();
	}

	public void dimissInfobar() {
		stopTImerTask();
		super.dismiss();
	}

	private void startTimeTask() {
		timer = new Timer();

		TimerTask refreshTask = new TimerTask() {
			public void run() {
				// refresh the word
				mDuration--;
				mHandler.sendEmptyMessage(mMsgWhat);
			}
		};

		TimerTask task = new TimerTask() {
			public void run() {

				timer.cancel();
				InfoBar.this.dismiss();
			}
		};

		timer.schedule(refreshTask, 1 * 1000, 1 * 1000);
		timer.schedule(task, mDuration * 1000);
	}

	private void stopTImerTask() {
		mHandler.removeMessages(mMsgWhat);
		timer.cancel();
	}
}
