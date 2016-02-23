package com.mediatek.ui.mmp.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.mmp.util.DisplayUtil;
import com.mediatek.ui.util.KeyMap;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.TextView;

public class AlertDialog extends Dialog {
	protected TextView vTitle;
	protected TextView vMessage;
	protected CharSequence mMessage;
	protected CharSequence mTitle;

	public AlertDialog(Context context, int theme) {
		super(context, theme);
	}

	public AlertDialog(Context context) {
		this(context, R.style.dialog);
	}

	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.mmp_alert_dialog);

		setLayoutParams();

		vTitle = (TextView) findViewById(R.id.mmp_title);
		vMessage = (TextView) findViewById(R.id.mmp_message);

		if (mTitle != null) {
			setTitle(mTitle);
		}
		if (mMessage != null) {
			setMessage(mMessage);
		}

		addContent();
	}

	public boolean dispatchKeyEvent(KeyEvent event) {
		int keyCode = event.getKeyCode();
		switch (keyCode) {
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
			return true;
		default:
			break;
		}

		return super.dispatchKeyEvent(event);
	}

	private void addContent() {
		ViewGroup container = (ViewGroup) findViewById(R.id.mmp_dialog_container);
		LayoutInflater.from(getContext()).inflate(getContentResId(), container,
				true);
	}

	private void setLayoutParams() {
		WindowManager.LayoutParams lp = getWindow().getAttributes();
		Context context = getContext();

		lp.width = DisplayUtil.getWidthPixels(context, 0.35f);
		lp.height = DisplayUtil.getHeightPixels(context, 0.35f);

		getWindow().setAttributes(lp);
	}

	public void setTitle(CharSequence title) {
		if (vTitle != null) {
			vTitle.setText(title);
		} else {
			mTitle = title;
		}
	}

	public void setMessage(int resId) {
		setMessage(getContext().getString(resId));
	}

	public void setMessage(CharSequence message) {
		if (vMessage != null) {
			vMessage.setText(message);
		} else {
			mMessage = message;
		}
	}

	protected int getContentResId() {
		return 0;
	}
}
