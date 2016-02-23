package com.mediatek.ui.pippop;

import android.content.Context;
import android.os.Handler;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.widget.ImageView;

import com.mediatek.ui.R;

public class FocusLabel extends ImageView {
	static final String TAG = "FocusLabel";

	private LayoutParams wmParams;
	private WindowManager windowManager;

	private int defaultWidth = 60;
	private int defaultHeight = 60;
	private int defaultOffsetX = 0;
	private int defaultOffsetY = 0;

	private int delayTime = 50 * 1000;

	private Handler mHandler = new Handler();
	private Runnable timerTask = new Runnable() {
		@Override
		public void run() {
			FocusLabel.this.setVisibility(View.INVISIBLE);
		}
	};

	/**
	 * show a pic on current focus window.
	 * 
	 * @param context
	 */
	public FocusLabel(Context context) {
		super(context);
		windowManager = (WindowManager) context.getApplicationContext()
				.getSystemService(Context.WINDOW_SERVICE);

		wmParams = new LayoutParams();
		wmParams.type = LayoutParams.TYPE_PHONE;
		wmParams.flags |= LayoutParams.FLAG_NOT_FOCUSABLE;
		wmParams.gravity = Gravity.LEFT | Gravity.TOP;
		// wmParams.gravity = Gravity.CENTER;

		wmParams.format = android.graphics.PixelFormat.RGBX_8888;
//		 setBackgroundResource(R.drawable.focus_tv);
		setBackgroundResource(R.drawable.nav_pip_focus_icon);

		windowManager.addView(this, wmParams);
		setDefaultLocation();

	}

	public void setSize(int width, int height) {
		wmParams.width = width;
		wmParams.height = height;

		windowManager.updateViewLayout(this, wmParams);
	}

	public void setPadding(int x, int y) {
		wmParams.x = x;
		wmParams.y = y;

		windowManager.updateViewLayout(this, wmParams);
	}

	public void setDefaultLocation() {
		wmParams.x = defaultOffsetX;
		wmParams.y = defaultOffsetY;

		wmParams.width = defaultWidth;
		wmParams.height = defaultHeight;

		windowManager.updateViewLayout(this, wmParams);
	}

	public void show() {
		wmParams.format = android.graphics.PixelFormat.TRANSPARENT;
		windowManager.updateViewLayout(this, wmParams);
		setDefaultPic();
		setVisibility(View.VISIBLE);

		Util.showDLog("wmParams.x: "+wmParams.x+" wmParams.y:"+ wmParams.y);
		
		startTimerTask();
	}

	public void release() {
		wmParams.format = android.graphics.PixelFormat.TRANSPARENT;
		setVisibility(View.GONE);

		windowManager.removeView(this);

		mHandler.removeCallbacks(timerTask);
	}

	public void setDefaultPic() {
//		 setBackgroundResource(R.drawable.focus_tv);
		setBackgroundResource(R.drawable.nav_pip_focus_icon);
	}

	private void startTimerTask() {
		mHandler.removeCallbacks(timerTask);

		mHandler.postDelayed(timerTask, delayTime);
	}

	public void hiddenFocus(boolean hidden) {
		if (hidden) {
			mHandler.removeCallbacks(timerTask);
		} else {
			startTimerTask();
		}
	}

}
