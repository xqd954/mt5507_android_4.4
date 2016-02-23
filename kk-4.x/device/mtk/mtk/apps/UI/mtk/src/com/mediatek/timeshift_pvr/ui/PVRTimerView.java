/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.ui;

import android.app.Activity;
import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.widget.TextView;

import com.mediatek.ui.R;

/**
 *
 */
public class PVRTimerView extends CommonInfoBar {

	private LayoutParams wmParams;

	private int defaultWidth = 280;
	private int defaultHeight = 100;
	private int defaultOffsetX = 0;
	private int defaultOffsetY = 20;

	private OnDismissListener mOnDismissListener;

	private WindowManager windowManager;
	private View mRootView;
	private TextView mInfo;

	private boolean isShowing = false;

	/**
	 * @param context
	 */
	public PVRTimerView(Activity context) {
		super(context);
		wmParams = new LayoutParams();
	}

	/**
	 * 
	 */
	protected void setLocation() {
		addToWM();
	}

	/*
	 * 
	 */
	@Override
	public void setInfo(String info) {
		if (mRootView != null) {
			if(mInfo.getVisibility()!=View.INVISIBLE){
				mInfo.setVisibility(View.VISIBLE);
			}
			mInfo.setText(info);
		}
	}
	
	public void setCurrentTime(long mills) {
		mills +=1;
	      long minute = mills / 60;
	      long hour = minute / 60;
	      long second = mills % 60;
	      minute %= 60;
	      setInfo(String.format("[%02d:%02d:%02d]", hour, minute,
	              second));
	}
	/**
	 * 
	 */
	private void addToWM() {
		windowManager = (WindowManager) mContext.getApplicationContext()
				.getSystemService(Context.WINDOW_SERVICE);

		wmParams.type = LayoutParams.TYPE_PHONE;
		wmParams.flags |= LayoutParams.FLAG_NOT_FOCUSABLE;
		wmParams.gravity = Gravity.BOTTOM | Gravity.LEFT;

		wmParams.format = android.graphics.PixelFormat.TRANSLUCENT;
		getContentView().setBackgroundResource(
				R.drawable.translucent_background);

//		getContentView().setBackgroundResource(R.drawable.nav_shorttip_bg);

		mRootView = getContentView();
		mInfo=((TextView) mRootView.findViewById(R.id.info));
		setDefaultLocation();
		windowManager.addView(mRootView, wmParams);
		setShowing(true);
	}

	@Override
	public void doSomething() {
		super.doSomething();
	}

	public void setDefaultLocation() {
		wmParams.x = defaultOffsetX;
		wmParams.y = defaultOffsetY;

		wmParams.width = defaultWidth;
		wmParams.height = defaultHeight;
	}

	/**
	 * 
	 */
	public void dismiss() {
		if(windowManager!=null&&mRootView!=null){
			try{
				windowManager.removeView(mRootView);
			}catch(Exception e){
				e.toString();
			}
		}

		if (getmOnDismissListener() != null) {
			getmOnDismissListener().onDismiss();
		}

		setShowing(false);
	}

	public OnDismissListener getmOnDismissListener() {
		return mOnDismissListener;
	}

	public void setmOnDismissListener(OnDismissListener mOnDismissListener) {
		this.mOnDismissListener = mOnDismissListener;
	}

	/**
	 * @return the isShowing
	 */
	public boolean isShowing() {
		return isShowing;
	}

	/**
	 * @param isShowing
	 *            the isShowing to set
	 */
	public void setShowing(boolean isShowing) {
		this.isShowing = isShowing;
	}
}
