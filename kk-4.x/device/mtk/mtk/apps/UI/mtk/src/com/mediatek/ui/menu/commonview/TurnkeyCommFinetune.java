package com.mediatek.ui.menu.commonview;

import com.mediatek.ui.R;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.ScreenConstant;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.Display;
import android.view.KeyEvent;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

/**
 * channel edit finetune
 * 
 * @author hs_haosun
 * 
 */
public class TurnkeyCommFinetune extends Dialog {
	private TextView numText, nameText, adjustText;
	private int xOff;
	private int yOff;
	public int menuWidth = 0;
	public int menuHeight = 0;
	public int count = 2;

	public TurnkeyCommFinetune(Context context) {
		super(context, R.style.Theme_TurnkeyCommDialog);

	}

	public TurnkeyCommFinetune(Context context, int buttonCount) {
		super(context, R.style.Theme_TurnkeyCommDialog);
	}

	public TurnkeyCommFinetune(Context context, String title, String info) {
		super(context, R.style.Theme_TurnkeyCommDialog);
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.menu_comm_finetune);
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		menuWidth =ScreenConstant.SCREEN_WIDTH/3;
		menuHeight =ScreenConstant.SCREEN_HEIGHT/3;
		lp.width = menuWidth;
		lp.height= menuHeight;
		window.setAttributes(lp);
		init();
	}

	private void init() {
		numText = (TextView) findViewById(R.id.comm_finetune_numr);
		nameText = (TextView) findViewById(R.id.comm_finetune_namer);
		adjustText = (TextView) findViewById(R.id.comm_finetune_frequency);
	}

	public void setNumText(String num) {
		String chNum = this.getContext().getString(R.string.menu_tv_channel_no);
		numText.setText(chNum + num);
	}

	public void setNameText(String name) {
		nameText.setText(name);
	}

	public void setAdjustText(String hz) {
		String adjustFre = this.getContext().getString(R.string.menu_tv_freq);
		adjustText.setText(adjustFre + hz);
	}

	/**
	 * Set the dialog's position relative to the (0,0)
	 */
	public void setPositon(int xoff, int yoff) {
		Window window = TurnkeyCommFinetune.this.getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = xoff;
		lp.y = yoff;
		this.xOff = xoff;
		this.yOff = yoff;
		window.setAttributes(lp);
	}

	public void setSize() {
		Window window = TurnkeyCommFinetune.this.getWindow();
		Display d = window.getWindowManager().getDefaultDisplay();
		WindowManager.LayoutParams p = window.getAttributes();
		p.height = (int) (d.getHeight());
		p.width = (int) (d.getWidth());
		window.setAttributes(p);
	}

	public TextView getNumText() {
		return numText;
	}

	public void setNumText(TextView numText) {
		this.numText = numText;
	}

	public TextView getNameText() {
		return nameText;
	}

	public void setNameText(TextView nameText) {
		this.nameText = nameText;
	}

	public TextView getAdjustText() {
		return adjustText;
	}

	public void setAdjustText(TextView adjustText) {
		this.adjustText = adjustText;
	}

	public void setxOff(int xOff) {
		this.xOff = xOff;
	}

	public void setyOff(int yOff) {
		this.yOff = yOff;
	}

	public int getxOff() {
		return xOff;
	}

	public int getyOff() {
		return yOff;
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
