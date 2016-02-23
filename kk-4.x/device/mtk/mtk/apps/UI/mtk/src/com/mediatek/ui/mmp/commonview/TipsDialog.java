package com.mediatek.ui.mmp.commonview;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Display;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.widget.ImageView;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.util.ScreenConstant;

public class TipsDialog extends Dialog {
	private ImageView vImageView;
	private TextView vTextView;
	private Context mContext;

	private String mText;

	public TipsDialog(Context context) {
		this(context, R.style.dialog);
		mContext = context;
	}

	public TipsDialog(Context context, int theme) {
		super(context, theme);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.mmp_tipsdialog);
		findView();
		if (mText != null) {
			setText(mText);
		}
		setDialogParams();
	}

	public String getTitle() {
		return mText;
	}

	public void reSetTitle() {
		mText = null;
	}

	private void findView() {
		vImageView = (ImageView) findViewById(R.id.multimedia_img);
		vTextView = (TextView) findViewById(R.id.multimedia_tv);
	}

	public void setText(String text) {
		if (vTextView != null) {
			vTextView.setText(text);
		} else {
			mText = text;
		}
	}

	public void setBackground(int res) {
		findViewById(R.id.tip_dialog_bg).setBackgroundResource(res);

	}

	public void setImage(int resId) {
		vImageView.setImageResource(resId);
	}

	// set toast dialog width and height
	private void setDialogParams() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.width = (int) (ScreenConstant.SCREEN_WIDTH * 0.5);
		lp.height = (int) (ScreenConstant.SCREEN_HEIGHT * 0.1);
		lp.flags |= LayoutParams.FLAG_NOT_FOCUSABLE;
		window.setAttributes(lp);
	}

	// set position at screen
	public void setWindowPosition() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = (int) (ScreenConstant.SCREEN_WIDTH * 0.2);
		lp.y = 0;
		window.setAttributes(lp);
	}

	public void setDialogParams(int width, int height) {
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.width = width;
		lp.height = height;
		lp.flags |= LayoutParams.FLAG_NOT_FOCUSABLE;
		window.setAttributes(lp);
	}

	public void setWindowPosition(int x, int y) {
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = x;
		lp.y = y;
		window.setAttributes(lp);
	}

/*	public boolean onKeyUp(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if (null != mContext && mContext instanceof MediaPlayActivity) {
			((MediaPlayActivity) mContext).onKeyUp(keyCode, event);
		}		
		return super.onKeyUp(keyCode, event);
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (null != mContext && mContext instanceof MediaPlayActivity) {
			((MediaPlayActivity) mContext).onKeyDown(keyCode, event);
			return true;
		}
		switch (keyCode) {
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN: {
			return true;
		}
		// fix cr DTV00407026 by xiaoyao
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:{
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_MUTE:{
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_ANGLE: {
			Util.exitMmpActivity(mContext);
			return true;
		}
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}*/
}
