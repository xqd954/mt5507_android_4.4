package com.mediatek.ui.menu.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.setup.Loading;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.ScreenConstant;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

public class CiDialog extends Dialog {
	private TextView titleView;
	private TextView textView;
	private String title;
	private String message;
	private int xOff;
	private int yOff;

	public CiDialog(Context context) {
		super(context, R.style.Theme_TurnkeyCommDialog);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(getLayoutInflater().inflate(R.layout.menu_factory_setup_ci_dialog, null), 
				new LayoutParams(ScreenConstant.SCREEN_WIDTH*11/20, ScreenConstant.SCREEN_HEIGHT*9/40));
		titleView = (TextView) findViewById(R.id.menu_factory_setup_ci_query);
		textView = (TextView) findViewById(R.id.menu_factory_setup_ci_num);
		titleView.setText(title);
		textView.setText(message);
	}

	public void setMessage(String message) {
		this.message = message;
	}

	public String getMessage() {
		return this.message;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getTitle() {
		return this.title;
	}

	public void setPosition(int xoff, int yoff) {
		Window window = CiDialog.this.getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = xoff;
		lp.y = yoff;
		this.xOff = xoff;
		this.yOff = xoff;
		window.setAttributes(lp);
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
