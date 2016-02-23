package com.mediatek.ui.menu.commonview;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.setup.Loading;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

/**
 * dialog factory,get types of dialogs here
 * 
 * @author hs_haosun
 * 
 */

public class TurnkeyCommDialog extends Dialog implements Runnable {
	private TextView titleView, textView, waitView;
	private Loading loading;
	private Button buttonYes, buttonNo;
	// dialog title
	private String title;
	// dialog display information
	private String message;
	private String buttonYesName;
	private String buttonNoName;
	private String buttonOKName;
	private int xOff;
	private int yOff;
	private int buttonCount = 2;
	private int focusedButton = 0;
	private String TAG = "TurnkeyCommDialog";
	Window window;
	WindowManager.LayoutParams lp;
	public int width = 0;
	public int height = 0;

	public TurnkeyCommDialog(Context context) {
		super(context, R.style.Theme_TurnkeyCommDialog);
		window = getWindow();
		lp = window.getAttributes();
	}

	public TurnkeyCommDialog(Context context, int buttonCount) {
		super(context, R.style.Theme_TurnkeyCommDialog);
		this.buttonCount = buttonCount;
		window = getWindow();
		lp = window.getAttributes();
	}

	public TurnkeyCommDialog(Context context, String title, String info) {
		super(context, R.style.Theme_TurnkeyCommDialog);
		this.title = title;
		this.message = info;
		window = getWindow();
		lp = window.getAttributes();
	}

	public void requestButtonFocus() {
		if (buttonCount == 2 || buttonCount == 3) {
			if (focusedButton == 0) {
				// buttonOK.requestFocusFromTouch();
			} else if (focusedButton == 1) {
				buttonYes.requestFocusFromTouch();
			} else if (focusedButton == 2) {
				buttonNo.requestFocusFromTouch();
			}
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		requestButtonFocus();
		return super.onTouchEvent(event);
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (buttonCount == 0) {
			MtkLog.d(TAG, "+++++++++++++Button = 0+++++++++++++++");
			setContentView(R.layout.menu_dialog_no_button);
			width = (int) (ScreenConstant.SCREEN_WIDTH * 0.55);
			lp.width = width;
			height = (int) (ScreenConstant.SCREEN_HEIGHT * 0.2);
			lp.height = height;
			window.setAttributes(lp);
			initNoButton();
		} else if (buttonCount == 3) {
			MtkLog.d(TAG, "+++++++++++++Button = 3+++++++++++++++");
			setContentView(R.layout.menu_comm_long_dialog);
			width = (int) (ScreenConstant.SCREEN_WIDTH * 0.55);
			lp.width = width;
			
			height = (int) (ScreenConstant.SCREEN_HEIGHT * 0.2);
			lp.height = height;
			window.setAttributes(lp);
			init();
		} else if (buttonCount == 4) {
			MtkLog.d(TAG, "+++++++++++++Button = 4+++++++++++++++");
			setContentView(R.layout.menu_dialog_network_gider);
			width = (int) (ScreenConstant.SCREEN_WIDTH * 0.55);
			lp.width = width;
			height = (int) (ScreenConstant.SCREEN_HEIGHT * 0.83);
			lp.height = height;
			window.setAttributes(lp);
			initnetGider();
		} else if (buttonCount == 5) {
			MtkLog.d(TAG, "+++++++++++++Button = 5+++++++++++++++");
			setContentView(R.layout.menu_dialog_auto_adjust);
			width = (int) (ScreenConstant.SCREEN_WIDTH* 0.31);
			lp.width = width;
			height = (int) (ScreenConstant.SCREEN_HEIGHT * 0.25);
			lp.height = height;
			window.setAttributes(lp);
			initAutoAdjust();
		}
	}

	OnFocusChangeListener focusChangeListener = new OnFocusChangeListener() {

		public void onFocusChange(View v, boolean hasFocus) {
			// TODO Auto-generated method stub
			if (hasFocus) {
				switch (v.getId()) {
				case R.id.comm_dialog_buttonYes:
					focusedButton = 1;
					break;
				case R.id.comm_dialog_buttonNo:
					focusedButton = 2;
					break;
				}
			}
		}
	};

	/**
	 * Init Dialog
	 */
	private void init() {
		titleView = (TextView) findViewById(R.id.comm_dialog_title);
		textView = (TextView) findViewById(R.id.comm_dialog_text);
		buttonYes = (Button) findViewById(R.id.comm_dialog_buttonYes);
		buttonYes.setText(buttonYesName);
		buttonYes.setFocusable(true);
		buttonYes.requestFocus();
		buttonYes.setOnFocusChangeListener(focusChangeListener);
		buttonYes.setOnTouchListener(onTouchListener);
		buttonNo = (Button) findViewById(R.id.comm_dialog_buttonNo);
		buttonNo.setText(buttonNoName);
		buttonNo.setOnFocusChangeListener(focusChangeListener);
		buttonNo.setOnTouchListener(onTouchListener);
		titleView.setText(title);
		textView.setText(message);
	}

	OnTouchListener onTouchListener = new OnTouchListener() {

		public boolean onTouch(View v, MotionEvent event) {
			// TODO Auto-generated method stub
			switch (v.getId()) {
			case R.id.comm_dialog_buttonYes:
				focusedButton = 1;
				break;
			case R.id.comm_dialog_buttonNo:
				focusedButton = 2;
				break;
			}
			requestButtonFocus();
			return false;
		}
	};

	private void initNoButton() {
		titleView = (TextView) findViewById(R.id.comm_dialog_title);
		textView = (TextView) findViewById(R.id.comm_dialog_text);
		waitView = (TextView) findViewById(R.id.comm_dialog_wait);
		loading = (Loading) findViewById(R.id.comm_dialog_loading);
		titleView.setText(title);
		textView.setText(message);
	}

	private void initnetGider() {
		titleView = (TextView) findViewById(R.id.comm_dialog_title);
		textView = (TextView) findViewById(R.id.comm_dialog_text);
		titleView.setText(title);
		textView.setText(message);
	}

	private void initAutoAdjust() {
		loading = (Loading) findViewById(R.id.menu_dialog_auto_adjust_loading);
		textView = (TextView) findViewById(R.id.menu_dialog_auto_adjust_text);
		textView.setText(message);
		loading.drawLoading();
	}

	/**
	 * Set the dialog's position relative to the (0,0)
	 */
	public void setPositon(int xoff, int yoff) {
		Window window = TurnkeyCommDialog.this.getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = xoff;
		lp.y = yoff;
		this.xOff = xoff;
		this.yOff = yoff;
		window.setAttributes(lp);
	}

	public Button getButtonYes() {
		return buttonYes;
	}

	public Button getButtonNo() {
		return buttonNo;
	}

	public void setButtonYesName(String buttonYesName) {
		this.buttonYesName = buttonYesName;
	}

	public void setButtonNoName(String buttonNoName) {
		this.buttonNoName = buttonNoName;
	}

	public void setMessage(String info) {
		this.message = info;
	}

	public void run() {
	}

	public TextView getTextView() {
		return textView;
	}

	public TextView getWaitView() {
		return waitView;
	}

	public Loading getLoading() {
		return loading;
	}

	// for volume down/up
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		switch (keyCode) {
		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_VOLUME_UP:
			return true;

		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

}
