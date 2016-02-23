package com.mediatek.ui.mmp.commonview;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnShowListener;
import android.inputmethodservice.Keyboard;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

import com.mediatek.ui.R;
import com.mediatek.ui.mmp.commonview.MTKKeyboardView.OnKeyboardActionListener;
import com.mediatek.ui.mmp.util.DisplayUtil;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

public class KeyboardDialog extends Dialog implements OnClickListener,
		OnShowListener {
	private static final String TAG = "KeyboardDialog";
	private static final String SUFFIX = "_";
	private static final String PWD_CHAR = "*";
	private EditText vInput;
	private Button vPosition;
	private Button vNegative;
	private MTKKeyboardView vKeyboardView;

	private OnPressedListener mListener;

	private boolean mPasswordMode;
	private String mContent;

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
		setContentView(R.layout.mmp_keyboard_dialog);

		WindowManager.LayoutParams lp = getWindow().getAttributes();
		Context context = getContext();

		lp.width = DisplayUtil.getWidthPixels(context, 0.30f);
		lp.height = DisplayUtil.getHeightPixels(context, 0.5f);

		getWindow().setAttributes(lp);

		MTKKeyboard keyboard = new MTKKeyboard(getContext(),
				R.xml.mmp_keyboard, lp.width, lp.height);

		final EditText vEditText = (EditText) findViewById(R.id.mmp_input);
		vEditText.setText("_");

		vKeyboardView = (MTKKeyboardView) findViewById(R.id.keyboard);
		vKeyboardView.setKeyboard(keyboard);
		vKeyboardView.setShifted(true);
		vKeyboardView.setFocusable(true);
		vKeyboardView.requestFocus();
		vKeyboardView
				.setOnKeyboardActionListener(new OnKeyboardActionListener() {
					public void onText(CharSequence text) {
					}

					public void onRelease(int primaryCode) {
					}

					public void onPress(int primaryCode) {
					}

					public void onKey(int primaryCode, int[] keyCodes) {
						String input = null;
						if (primaryCode == Keyboard.KEYCODE_SHIFT) {
							vKeyboardView.setShifted(!vKeyboardView.isShifted());
						} else if (primaryCode == Keyboard.KEYCODE_DELETE) {
							int currentLength = mContent.length();
							if (currentLength > 0) {
								mContent = mContent.substring(0,
										currentLength - 1);
								if (mPasswordMode) {
									input = vEditText.getText().toString()
											.substring(1);
								} else {
									input = mContent + SUFFIX;
								}
							}
						} else {
							String key = String.valueOf((char) primaryCode);
							if (vKeyboardView.isShifted()) {
								key = key.toUpperCase();
							}
							mContent += key;

							if (mPasswordMode) {
								input = PWD_CHAR
										+ vEditText.getText().toString();
							} else {
								input = mContent + SUFFIX;
							}
						}

						if (input != null) {
							vEditText.setText(input);
							vEditText.setSelection(input.length() - 1);
						}
					}
				});

		vInput = (EditText) findViewById(R.id.mmp_input);
		vPosition = (Button) findViewById(R.id.mmp_ok);
		vNegative = (Button) findViewById(R.id.mmp_cancel);

		vPosition.setOnClickListener(this);
		vNegative.setOnClickListener(this);

		setOnShowListener(this);
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

	public void onClick(View v) {
		int id = v.getId();
		switch (id) {
		case R.id.mmp_ok:
			if (mListener != null) {
				mListener.onPositivePressed(getInput());
			}
			break;
		case R.id.mmp_cancel:
			if (mListener != null) {
				mListener.onNegativePressed();
			}
			break;
		default:
			break;
		}
	}

	public void setPrefill(String prefill) {
		mContent = prefill;
		MtkLog.d(TAG, "Set Content : " + mContent);
	}

	private String getInput() {
		MtkLog.d(TAG, "Get Content : " + mContent);
		return mContent;
	}

	public void isPassword(boolean isPassword) {
		mPasswordMode = isPassword;
	}

	public void onShow(DialogInterface dialog) {
		if (mContent != null) {
		//	String input = "";
       StringBuilder inputBuilder = new StringBuilder("");
			if (mPasswordMode) {
				for (int i = 0; i < mContent.length(); i++) {
					inputBuilder.append(PWD_CHAR);
					//input += PWD_CHAR;
				}
			} 
			//else {
			//	input = mContent;
			//}

			vInput.setText(inputBuilder.toString() + SUFFIX);
			vInput.setSelection(inputBuilder.length() + 1);
		}

		vKeyboardView.init();
		vKeyboardView.requestFocus();
	}

	public void setOnPressedListener(OnPressedListener listener) {
		mListener = listener;
	}

	public interface OnPressedListener {
		void onPositivePressed(String input);

		void onNegativePressed();
	}
}
