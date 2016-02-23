package com.mediatek.ui.menu.commonview;

import java.util.Timer;
import java.util.TimerTask;

import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.nav.commonview.SnowTextView;
import com.mediatek.ui.nav.commonview.ToastInfoView;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import android.R.integer;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.ViewGroup.LayoutParams;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

public class ToastDialog extends Dialog implements Runnable {
	private Context context;
	private TextView textView;
	private Timer timer;
    private View mSnowTextView;
	// VGA has no signal dialog flag
	private boolean tkFlag = false;

	private ToastInfoView mToastView;

	public ToastDialog(Context context) {
		super(context, R.style.Theme_ToastDialog);
		this.context = context;
		textView = new TextView(context);
		textView.setBackgroundResource(R.drawable.transparent_background);
	}

	public ToastDialog(Context context, boolean flag,View mSnowTextView) {
		super(context, R.style.Theme_ToastDialog);
		this.context = context;
		this.mSnowTextView =mSnowTextView;
		textView = new TextView(context);
		textView.setBackgroundResource(R.drawable.transparent_background);
		tkFlag = true;
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(textView);
		timer = new Timer();
		mToastView = ToastInfoView.getInstance(context);
	}

	public void setTextSize(float size, int gravity) {
		textView.setTextSize(size);
		textView.setGravity(gravity);
	}

	public void setBackGround(int res) {
		textView.setBackgroundResource(res);
	}

	public void setText(String text) {
		textView.setText(text);
	}

	/**
	 * will be replaced by setSize()
	 * 
	 * @deprecated
	 * @param width
	 *            the toast dialog width
	 */
	public void setWidth(int width) {
		int mWidth = width * ScreenConstant.SCREEN_WIDTH/1280;
		textView.setWidth(mWidth);
	}

	public void setSize(int width, int height) {
		int mWidth = width * ScreenConstant.SCREEN_WIDTH/1280;
		int mHeight = height * ScreenConstant.SCREEN_WIDTH/720;
		textView.setWidth(mWidth);
		textView.setHeight(mHeight);
	}

	public void setLayoutParams(LayoutParams params) {
		textView.setLayoutParams(params);
	}

	public void run() {
		this.show();
		timer.schedule(task, 900);
	}

	public void run(int time) {
		this.show();
		timer.schedule(task, time);
	}

	private TimerTask task = new TimerTask() {
		public void run() {
			dismiss();
		}
	};

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (tkFlag && mSnowTextView != null) {
			dismiss();
			mSnowTextView.setVisibility(View.VISIBLE);
			if (null != this.context
					&& this.context instanceof TurnkeyUiMainActivity) {
				((TurnkeyUiMainActivity) this.context)
						.onKeyDown(keyCode, event);
			}
		}

		switch (keyCode) {

		case KeyEvent.KEYCODE_BACK:
			return true;
		case KeyMap.KEYCODE_MENU:
			if (textView.getText().toString().equals("Feature Not Available")) {
				if(this.context != null){
					((TurnkeyUiMainActivity) this.context)
					.onKeyDown(keyCode, event);
				}
			}
			return true;
		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_VOLUME_UP:
			return true;	
		case KeyMap.KEYCODE_MTKIR_MUTE:
			TurnkeyUiMainActivity.switchMute();
			return true;
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	public void setPositon(int xoff, int yoff) {
		Window window = ToastDialog.this.getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = xoff * ScreenConstant.SCREEN_WIDTH/1280;
		lp.y = yoff*ScreenConstant.SCREEN_HEIGHT/720;
		window.setAttributes(lp);
	}
}
