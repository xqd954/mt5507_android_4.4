/**   
 * @Description: TODO()
 * 
 * @date 2012-10-16 ����5:51:28  
 */
package com.mediatek.ui.pippop;

import android.content.Context;
import android.graphics.RectF;
import android.util.Log;
import android.view.View;

/**
 * 
 */
public class TVController {

	public Context mContext;

	private TVWindowView mMainOutPutWindow;
	private TVWindowView mSubOutPutWindow;

	private FocusLabel mFocusLable;

	private final String mainSource = Core.MAIN;
	private final String subSource = Core.SUB;

	private PippopUiLogic mLogic;

	int tempTimes = 0;

	public TVController(Context context) {
		super();
		// TODO Auto-generated constructor stub
		this.mContext = context;
		mLogic = PippopUiLogic.getInstance(mContext);
	}

	/**
	 * 
	 */
	public void pipAction() {
		// TODO Auto-generated method stub
		showMainSource();
		showSubSource();
	}

	public void showMainSource() {
		mMainOutPutWindow = new TVWindowView(mContext);
		mMainOutPutWindow.setAsPOPLeftWindow();
	}

	private void showSubSource() {
		mSubOutPutWindow = new TVWindowView(mContext);
		mSubOutPutWindow.setAsPOPRightWindow();
	}

	private void showFocus() {
		if (mFocusLable != null)
			mFocusLable.show();
	}

	/**
	 * 
	 */
	public void hiddenAllTVViews() {
		if (mMainOutPutWindow != null) {
			mMainOutPutWindow = null;
		}

		if (mSubOutPutWindow != null) {
			mSubOutPutWindow = null;
		}
	}

	public TVWindowView getMainOutPutWindow() {
		if (mMainOutPutWindow == null) {
			mMainOutPutWindow = new TVWindowView(mContext);
		}

		return mMainOutPutWindow;
	}

	public void setMainOutPutWindow(TVWindowView mMainOutPutWindow) {

		this.mMainOutPutWindow = mMainOutPutWindow;
	}

	public TVWindowView getSubOutPutWindow() {
		if (mSubOutPutWindow == null) {
			mSubOutPutWindow = new TVWindowView(mContext);
		}

		return mSubOutPutWindow;
	}

	public void setSubOutPutWindow(TVWindowView mSubOutPutWindow) {
		this.mSubOutPutWindow = mSubOutPutWindow;
	}

	/**
	 * @param subIsFocused
	 */
	public void setSubHasFocus(boolean subIsFocused) {
		if (mFocusLable == null || mFocusLable.getVisibility() != View.VISIBLE) {
			mFocusLable = new FocusLabel(mContext);
		}

		if (subIsFocused) {
			int offsetX = (int) mSubOutPutWindow.getTVLocation()[0]
					+ (int) (mSubOutPutWindow.getTVSize()[0]) / 2 - 30;// focuslabel's
																		// width
			int offsetY = (int) mSubOutPutWindow.getTVLocation()[1];

			mFocusLable.setPadding(offsetX, offsetY);
		} else {
			int offsetX = (int) mMainOutPutWindow.getTVLocation()[0]
					+ (int) (mMainOutPutWindow.getTVSize()[0]) / 2 - 30;// focuslabel's
																		// width
			int offsetY = (int) mMainOutPutWindow.getTVLocation()[1];

			mFocusLable.setPadding(offsetX, offsetY);
		}
		showFocus();
	}

	/**
	 */
	public void releaseFocus() {
		if (mFocusLable != null)
			mFocusLable.release();

		mFocusLable = null;
	}
	
	public boolean reDrawFocus() {
		if (mFocusLable != null && mFocusLable.getVisibility() != View.VISIBLE) {
			mFocusLable.show();
			return false;
		}
		return true;
	}
	
	public FocusLabel getmFocusLabel(){
		return mFocusLable;
	}

	public void hiddenFocus(boolean hidden) {
		if (mFocusLable != null) {
			if (hidden) {
//				mFocusLable.setVisibility(View.INVISIBLE);
				mFocusLable.setVisibility(View.INVISIBLE);
			} else {
//				mFocusLable.setVisibility(View.VISIBLE);
				mFocusLable.show();
			}
		}
	}

	/**
	 * change PIP mode.
	 * 
	 * @return
	 */
	public void changePIPMode(int mode) {
		mLogic.changeOutputMode();
	}

	public String getMainSource() {
		return mainSource;
	}

	public String getSubSource() {
		return subSource;
	}

	public void modifySubOutput() {
		// TODO: measure the output window,
		float leftTopX;
		float leftTopY;

		float tvWidth;
		float tvHeight;

		float LCDWidth = getSubOutPutWindow().getmScreenWidth();
		float LCDHeight = getSubOutPutWindow().getmScreenHeight();

		leftTopX = getSubOutPutWindow().getTVLocation()[0];
		leftTopY = getSubOutPutWindow().getTVLocation()[1];

		tvWidth = getSubOutPutWindow().getTVSize()[0];
		tvHeight = getSubOutPutWindow().getTVSize()[1];

		float x0 = countFloat(leftTopX / LCDWidth);
		float y0 = countFloat(leftTopY / LCDHeight);
		float x1 = countFloat(leftTopX / LCDWidth + tvWidth / LCDWidth);
		float y1 = countFloat(leftTopY / LCDHeight + tvHeight / LCDHeight);

		mLogic.setOutputPosition(getSubSource(), new RectF(x0, y0, x1, y1));

		Util.showDLog(getSubSource() + "***TV Rect*********************");
		Util.showDLog("x0:-->" + x0 + "y0:-->" + y0 + "x1:-->" + x1 + "y1:-->"
				+ y1);
	}

	public void modifyMainOutput() {
		float leftTopX;
		float leftTopY;

		float tvWidth;
		float tvHeight;

		float LCDWidth = getMainOutPutWindow().getmScreenWidth();
		float LCDHeight = getMainOutPutWindow().getmScreenHeight();

		leftTopX = getMainOutPutWindow().getTVLocation()[0];
		leftTopY = getMainOutPutWindow().getTVLocation()[1];

		tvWidth = getMainOutPutWindow().getTVSize()[0];
		tvHeight = getMainOutPutWindow().getTVSize()[1];

		float x0 = countFloat(leftTopX / LCDWidth);
		float y0 = countFloat(leftTopY / LCDHeight);
		float x1 = countFloat(leftTopX / LCDWidth + tvWidth / LCDWidth);
		float y1 = countFloat(leftTopY / LCDHeight + tvHeight / LCDHeight);

		mLogic.setOutputPosition(getMainSource(), new RectF(x0, y0, x1, y1));

		Util.showDLog(getMainSource() + "***TV Rect*********************");
		Util.showDLog("x0:-->" + x0 + "y0:-->" + y0 + "x1:-->" + x1 + "y1:-->"
				+ y1);

	}

	/**
	 * 
	 */
	private float countFloat(float f) {
		// TODO Auto-generated method stub
		// return (float)(Math.round(f*100))/100;
		return f;
	}

	public PippopUiLogic getmLogic() {
		return mLogic;
	}

}
