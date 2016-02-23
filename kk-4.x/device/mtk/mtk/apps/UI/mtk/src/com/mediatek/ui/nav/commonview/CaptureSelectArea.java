package com.mediatek.ui.nav.commonview;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;

import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.nav.CaptureLogoActivity;
import com.mediatek.ui.nav.CaptureLogoActivity.DialogId;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

/**
 * In capture logo screen, when special area option is selected, the screen will
 * show a rectangle area which can move to select a special area as power on
 * picture.
 * 
 * @author MTK40462
 * 
 */
public class CaptureSelectArea extends View implements View.OnKeyListener {

	private static final String TAG = "CaptureSelectArea";

	private CaptureLogoActivity capActivity;

	private static final int moveStep = 5;

	private int rWidth = (int)(0.176*ScreenConstant.SCREEN_WIDTH);
	private int rHeight = (int)(0.18*ScreenConstant.SCREEN_HEIGHT);

	private static final int minSize = 50;

	private Handler mHandler;

	// if adjustSizeMode is true, the select area size can be adjusted by
	// pressing direction key
	private boolean adjustSizeMode = false;

	private Rect area = new Rect(0, 0, rWidth, rHeight);
	private int mWindowWidth;
	private int mWindowHeight;

	public CaptureSelectArea(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		MtkLog.e("tag", "rWidth:    "+rWidth+"  rHeight:"+rHeight);
		// TODO Auto-generated constructor stub
		WindowManager wm = (WindowManager) context
				.getSystemService(Context.WINDOW_SERVICE);
		mWindowWidth = ScreenConstant.SCREEN_WIDTH;
		mWindowHeight = ScreenConstant.SCREEN_HEIGHT;
		setOnKeyListener(this);
	}

	public CaptureSelectArea(Context context, AttributeSet attrs) {
		super(context, attrs);
		mWindowWidth = ScreenConstant.SCREEN_WIDTH;
		mWindowHeight = ScreenConstant.SCREEN_HEIGHT;
		// TODO Auto-generated constructor stub
		setOnKeyListener(this);
	}

	public CaptureSelectArea(Context context) {
		super(context);
		mWindowWidth = ScreenConstant.SCREEN_WIDTH;
		mWindowHeight = ScreenConstant.SCREEN_HEIGHT;
		setOnKeyListener(this);
		capActivity = (CaptureLogoActivity) context;
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		Paint paint = new Paint();
		paint.setColor(0xa0ff8040);
		canvas.drawRect(area, paint);
	}

	public Rect getCaptureArea() {
		return area;
	}

	public boolean onKey(View v, int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub

		mHandler.removeMessages(CaptureLogoActivity.MESSAGE_DISMISS);
		mHandler.sendEmptyMessageDelayed(CaptureLogoActivity.MESSAGE_DISMISS,
				CaptureLogoActivity.AUTO_DISMISS_TIME);
		if (event.getAction() == KeyEvent.ACTION_DOWN) {
			switch (keyCode) {

			case KeyEvent.KEYCODE_DPAD_DOWN:
				if (area.bottom < mWindowHeight) {
					if (adjustSizeMode) {
						area.bottom += moveStep;
						rHeight = area.height();
					} else {
						area.top += moveStep;
						area.bottom = area.top + rHeight;
					}
					invalidate();
				} else {
					area.bottom = mWindowHeight;
				}
				break;
			case KeyEvent.KEYCODE_DPAD_UP:
				if (area.top >= 0) {
					if (adjustSizeMode) {
						if (area.height() > minSize) {
						area.bottom -= moveStep;
						rHeight = area.height();
						}
					} else {
						if (area.top > 0) {
						area.top -= moveStep;
						area.bottom = area.top + rHeight;
					}
					}
					invalidate();
				} else {
					area.top = 0;
				}

				break;
			case KeyEvent.KEYCODE_DPAD_LEFT:

				if (area.left >= 0) {
					if (adjustSizeMode) {
						if (area.width() > minSize) {
							area.right -= moveStep;
							rWidth = area.width();
						}
					} else {
						if (area.left > 0) {
						area.left -= moveStep;
						area.right = area.left + rWidth;
					}
					}
					invalidate();
				} else {
					area.left = 0;
				}
				break;
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				if (area.right < mWindowWidth) {
					if (adjustSizeMode) {
						area.right += moveStep;
						rWidth = area.width();
					} else {
						area.left += moveStep;
						area.right = area.left + rWidth;
					}
					invalidate();
				} else {
					area.right = mWindowWidth;
				}
				break;
			case KeyEvent.KEYCODE_DPAD_CENTER:
				adjustSizeMode = !adjustSizeMode;
				MtkLog.d(TAG, "ajust size mode: " + adjustSizeMode);
				if (this != null && adjustSizeMode == false) {
					setVisibility(View.GONE);
					invalidate();
					capActivity.createDialog(DialogId.ADJUST_POSITION);
					CaptureLogoActivity.captureMain.setVisibility(View.VISIBLE);

				}

				break;
			default:
				break;
			}
		}
		return super.onKeyDown(keyCode, event);
	}

	public void setHandler(Handler handler) {
		this.mHandler = handler;
	}
}
