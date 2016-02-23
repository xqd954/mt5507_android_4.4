package com.mediatek.ui.menu.commonview;

import android.app.Dialog;
import android.content.Context;
import android.inputmethodservice.Keyboard;
import android.os.Bundle;
import android.text.InputFilter;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.EditText;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.MTKKeyboardView.OnKeyboardActionListener;
import com.mediatek.ui.menu.util.DisplayUtil;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.ScreenConstant;

/**
 * channel edit name
 * 
 * @author hs_haosun
 * 
 */
public class KeyboardDialog extends Dialog {
	private EditText vInput;
	private Button button_ok, button_cancel;
	private int focusedButton = 0;

	public KeyboardDialog(Context context, boolean cancelable,
			OnCancelListener cancelListener) {
		super(context, cancelable, cancelListener);
	}

	public KeyboardDialog(Context context, int theme) {
		super(context, theme);
	}

	public KeyboardDialog(Context context) {
		this(context, R.style.dialog);
	}

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.menu_keyboard_dialog);
		button_ok = (Button) findViewById(R.id.mmp_ok);
		button_ok.setOnFocusChangeListener(focusChangeListener);
		button_ok.setFocusable(true);
		button_ok.setOnTouchListener(onTouchListener);
		button_cancel = (Button) findViewById(R.id.mmp_cancel);
		button_cancel.setOnFocusChangeListener(focusChangeListener);
		button_cancel.setFocusable(true);
		button_cancel.setOnTouchListener(onTouchListener);

		WindowManager.LayoutParams lp = getWindow().getAttributes();
		Context context = getContext();

		lp.width = DisplayUtil.getWidthPixels(context, 0.6f);
		lp.height = DisplayUtil.getHeightPixels(context, 0.6f);

		getWindow().setAttributes(lp);

		MTKKeyboard keyboard = new MTKKeyboard(getContext(),
				R.xml.menu_keyboard, lp.width, lp.height);

		final EditText vEditText = (EditText) findViewById(R.id.mmp_input);
		vEditText.setText("_");

		final MTKKeyboardView keyboardView = (MTKKeyboardView) findViewById(R.id.keyboard);
		keyboardView.setKeyboard(keyboard);
		keyboardView.setShifted(true);
		keyboardView.setFocusable(true);
		keyboardView.requestFocus();
		keyboardView
				.setOnKeyboardActionListener(new OnKeyboardActionListener() {
					public void onText(CharSequence text) {
					}

					public void onRelease(int primaryCode) {
					}

					public void onPress(int primaryCode) {
						requestButtonFocus();
					}

					public void onKey(int primaryCode, int[] keyCodes) {
						if (primaryCode == Keyboard.KEYCODE_DELETE) {
							int currentLength = vEditText.getText().length();
							if (currentLength > 1) {
								CharSequence deleted = vEditText.getText()
										.subSequence(0, currentLength - 2)
										+ "_";
								vEditText.setText(deleted);
								vEditText.setSelection(vEditText.getText()
										.length() - 1);
							}
						} else if (primaryCode == Keyboard.KEYCODE_SHIFT) {
							keyboardView.setShifted(!keyboardView.isShifted());
						} else {
							String key = String.valueOf((char) primaryCode);
							if (keyboardView.isShifted()) {
								CharSequence text = vEditText.getText().insert(
										vEditText.getText().length() - 1,
										key.toUpperCase());
								vEditText.setText(text);
							} else {
								CharSequence text = vEditText.getText().insert(
										vEditText.getText().length() - 1, key);
								vEditText.setText(text);
							}
							vEditText
									.setSelection(vEditText.getText().length() - 1);
						}
					}
				});

		vInput = (EditText) findViewById(R.id.mmp_input);

	}

	public void requestButtonFocus() {
		if (focusedButton == 0) {
			button_ok.requestFocusFromTouch();
		} else if (focusedButton == 1) {
			button_cancel.requestFocusFromTouch();
		}
	}
	
	public void setEditMaxLength(int len){
		vInput.setFilters(new InputFilter[]{
				new InputFilter.LengthFilter(len)
		});
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		requestButtonFocus();
		return super.onTouchEvent(event);
	}

	OnFocusChangeListener focusChangeListener = new OnFocusChangeListener() {

		public void onFocusChange(View v, boolean hasFocus) {
			// TODO Auto-generated method stub
			if (hasFocus) {
				switch (v.getId()) {
				case R.id.mmp_ok:
					focusedButton = 0;
					break;
				case R.id.mmp_cancel:
					focusedButton = 1;
					break;
				}
			}
		}
	};

	OnTouchListener onTouchListener = new OnTouchListener() {

		public boolean onTouch(View v, MotionEvent event) {
			// TODO Auto-generated method stub
			switch (v.getId()) {
			case R.id.mmp_ok:
				focusedButton = 0;
				break;
			case R.id.mmp_cancel:
				focusedButton = 1;
				break;
			}
			requestButtonFocus();
			return false;
		}
	};

	public String getInput() {
		String str = "";
		str = vInput.getText().toString().trim();
		String input = "";
		if (str.contains("_")) {
			input = str.replace("_", "");
		}
		return input;
	}

	/**
	 * set the dialog`s position relative to (0,0)
	 */
	public void setPositon(int xoff, int yoff) {
		Window window = KeyboardDialog.this.getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
		lp.x = xoff;
		lp.y = yoff;
		window.setAttributes(lp);
	}

	public void setSize(float height, float width) {
		Window window = KeyboardDialog.this.getWindow();
		WindowManager.LayoutParams p = window.getAttributes();
		p.height = (int) (ScreenConstant.SCREEN_HEIGHT * height);
		p.width = (int) (ScreenConstant.SCREEN_WIDTH * width);
		window.setAttributes(p);
	}

	public EditText getvInput() {
		return vInput;
	}

	public void setvInput(EditText vInput) {
		this.vInput = vInput;
	}

	public Button getButton_ok() {
		return button_ok;
	}

	public void setButton_ok(Button button_ok) {
		this.button_ok = button_ok;
	}

	public Button getButton_cancel() {
		return button_cancel;
	}

	public void setButton_cancel(Button button_cancel) {
		this.button_cancel = button_cancel;
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
