package com.mediatek.ui.pippop;

import com.mediatek.ui.util.ScreenConstant;
import android.content.Context;
import android.graphics.Point;
import android.os.Build;
import android.view.Gravity;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;

public class TVWindowView {
	static final String TAG = "TvView";

	private LayoutParams wmParams = new LayoutParams();
	private WindowManager windowManager;

	private int defaultWidth = 0;
	private int defaultHeight = 0;
	private int defaultOffsetX = 0;
	private int defaultOffsetY = 0;

	private final int mScreenWidth;
	private final int mScreenHeight;

	private float mTVLocation[] = new float[2];

	public TVWindowView(Context context) {

		setDefaultLocation();
		int sdkVersion = Build.VERSION.SDK_INT;
		windowManager = (WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE);

//		if (sdkVersion >= 13) {
//			Util.showDLog("come in sdkVersion >= 13");
//			Point point = new Point();
//			windowManager.getDefaultDisplay().getSize(point);
//
//			mScreenHeight = point.y;
//			mScreenWidth = point.x;
//
//		} else {
			Util.showDLog("come in sdkVersion < 13");
			mScreenWidth = ScreenConstant.SCREEN_WIDTH;
			mScreenHeight = ScreenConstant.SCREEN_HEIGHT;
			Util.showDLog("X: " + mScreenWidth + " Y: " + mScreenHeight);
//		}

		defaultWidth = mScreenWidth;
		defaultHeight = mScreenHeight;
	}

	private void setDefaultLocation() {
		wmParams.x = defaultOffsetX;
		wmParams.y = defaultOffsetY;

		wmParams.width = defaultWidth;
		wmParams.height = defaultHeight;
	}

	public void setWindowSize(int width, int height) {
		wmParams.width = width;
		wmParams.height = height;
	}

	public void setWindowPadding(int x, int y) {
		wmParams.x = x;
		wmParams.y = y;
	}

	public void setWindowGravity(int gravity) {
		wmParams.gravity = gravity;
	}

	public void setFullScreen() {
		wmParams.gravity = Gravity.LEFT | Gravity.TOP;

		setWindowSize(mScreenWidth, mScreenHeight); // debugging -100
		this.setWindowPadding(0, 100);

		setTVLocation(new float[] { 0, 0 });
	}

//	public void setAsPIPSubWindow() {
//		wmParams.gravity = Gravity.CENTER;
//		int w = mScreenWidth / 4;
//		int h = mScreenHeight / 4;
//
//		setWindowSize(mScreenWidth / 4, mScreenHeight / 4);
//
//		int offsetX = (int) (mScreenWidth / 4);
//		int offsetY = (int) (-mScreenHeight / 4);
//
//		setWindowPadding(offsetX, offsetY);
//
//		setTVLocation(new float[] { mScreenWidth / 2 + offsetX - w / 2,
//				mScreenHeight / 2 + offsetY - h / 2 });
//	}

	public void setAsPOPLeftWindow() {
		wmParams.gravity = Gravity.CENTER_VERTICAL | Gravity.LEFT;

		wmParams.x = 0;
		wmParams.y = 0;

		int w = mScreenWidth / 2;
		int h = (int) ((float) mScreenHeight / mScreenWidth * mScreenWidth / 2);
		setWindowSize(w, h);

		setTVLocation(new float[] { 0, mScreenHeight / 2 - h / 2 });
	}

	public void setAsPOPRightWindow() {
		wmParams.gravity = Gravity.CENTER_VERTICAL | Gravity.RIGHT;

		wmParams.x = 0;
		wmParams.y = 0;

		int w = mScreenWidth / 2;
		int h = (int) ((float) mScreenHeight / mScreenWidth * mScreenWidth / 2);
		setWindowSize(w, h);

		setTVLocation(new float[] { mScreenWidth / 2, mScreenHeight / 2 - h / 2 });
	}

	/**
	 * 
	 */
	public void showDebugInfo() {
		Util.showDLog("TVLocation(): " + this.getTVLocation()[0] + "---"
				+ this.getTVLocation()[1]);
		Util.showDLog("TVSize(): " + this.getTVSize()[0] + "---"
				+ this.getTVSize()[1]);

	}

	public float[] getTVLocation() {
		return mTVLocation;
	}

	public void setTVLocation(float[] mTVLocation) {
		this.mTVLocation = mTVLocation;
	}

	public float[] getTVSize() {
		android.view.ViewGroup.LayoutParams parmas = this.getWmParams();

		return new float[] { parmas.width, parmas.height };
	}

	public int getmScreenWidth() {
		return mScreenWidth;
	}

	public int getmScreenHeight() {
		return mScreenHeight;
	}

	public LayoutParams getWmParams() {
		return wmParams;
	}

}
