package com.mediatek.ui.nav.commonview;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.util.CheckLockSignalChannelState;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.ScreenConstant;

public class EPGPwdDialog extends Dialog {

	private static final String TAG = "EPGPwd";

	private Context mContext; 

	private NavIntegration mNavIntegration;

	private static final int MSG_PWD_ERROR_DISMISS = 1;
	private static final int MSG_CHECK_DELAY = 100;
	public static final int PASSWORD_VIEW_PWD_INPUT = 1;
	public static final int PASSWORD_VIEW_PWD_ERROR = 2;
	public int mode = PASSWORD_VIEW_PWD_INPUT;
    private String password ="";
    private String showPasswordStr="";
    
	private LinearLayout pwdView;
	private TextView pwdError;
	private TextView pwdValue;
  
	public EPGPwdDialog(Context context, int theme) {
		super(context, theme);
		setContentView(R.layout.epg_pwd_view);
		findViews();
		setOnKeyListener(new OnKeyListener() {

			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					switch (keyCode) {
					case KeyMap.KEYCODE_DPAD_CENTER:
//						if (mode == PASSWORD_VIEW_PWD_ERROR) {
//							showPasswordView(PASSWORD_VIEW_PWD_INPUT);
//						} else if (mode == PASSWORD_VIEW_PWD_INPUT) {
//							String inputStr = getInputString();
//							if ("null".equals(inputStr) || "".equals(inputStr)) {
//								dismiss();
//							} else {
//								checkPassWord(inputStr);
//							}
//						}
						checkPassWord(password);
						return true;
					default:
						break;
					}
				}
				return false;
			}
		});
	}

	public EPGPwdDialog(Context context) {
		this(context, R.style.nav_dialog);
		Log.d(TAG, "Constructor!!");
		mContext = context;
		setContentView(R.layout.epg_pwd_view);
		password ="";
		showPasswordStr="";
		findViews();
	}

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_PWD_ERROR_DISMISS: {
				showPasswordView(PASSWORD_VIEW_PWD_INPUT);
				break;
			}
			default:
				break;
			}

		}

	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.d(TAG, "onCreate");
		mNavIntegration = NavIntegration.getInstance(mContext);
		setWindowPosition();
	}

	protected void onStart() {
		password ="";
		showPasswordStr="";
		showPasswordView(PASSWORD_VIEW_PWD_INPUT);
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {

		switch (event.getKeyCode()) {
		case KeyMap.KEYCODE_MENU:
		case KeyEvent.KEYCODE_MTKIR_YELLOW:
		case KeyEvent.KEYCODE_MTKIR_BLUE:
		case KeyEvent.KEYCODE_MTKIR_GREEN:
		case KeyEvent.KEYCODE_MTKIR_RED:
			return true;
		case KeyMap.KEYCODE_BACK:
			dismiss();
			break;
		case KeyMap.KEYCODE_0:
		case KeyMap.KEYCODE_1:
		case KeyMap.KEYCODE_2:
		case KeyMap.KEYCODE_3:
		case KeyMap.KEYCODE_4:
		case KeyMap.KEYCODE_5:
		case KeyMap.KEYCODE_6:
		case KeyMap.KEYCODE_7:
		case KeyMap.KEYCODE_8:
		case KeyMap.KEYCODE_9:
			mHandler.removeMessages(MSG_PWD_ERROR_DISMISS);
			inputChar(keyCode - 7);
			return true;

		default:
			break;
		}

//		if (null != mContext && mContext instanceof EPGActivity) {
//			((EPGActivity) mContext).onKeyDown(keyCode, event);
//		}

		if (keyCode == KeyMap.KEYCODE_VOLUME_UP
				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
			return true;
		}

		return super.onKeyDown(keyCode, event);

	};



	private void findViews() {
		pwdError = (TextView) findViewById(R.id.epg_pwd_error);
		pwdView = (LinearLayout) findViewById(R.id.epg_input_pwd_view);
		pwdValue = (TextView) findViewById(R.id.epg_pwd_value);
		pwdValue.setInputType(EditorInfo.TYPE_NULL);
		pwdValue.addTextChangedListener(passwordInputTextWatcher);
	}

	private int marginX;
	private int marginY;
	private static int menuWidth = 390;
	private static int menuHeight = 220;

	public void setWindowPosition() {
		// TODO
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();

		marginX = 500*ScreenConstant.SCREEN_WIDTH/1280;
		marginY = 100*ScreenConstant.SCREEN_HEIGHT/720;

		lp.width = menuWidth*ScreenConstant.SCREEN_WIDTH/1280;
		lp.height = menuHeight*ScreenConstant.SCREEN_HEIGHT/720;

		lp.x = ScreenConstant.SCREEN_WIDTH / 2 - menuWidth / 2 - marginX;
		lp.y = (int) (ScreenConstant.SCREEN_HEIGHT / 2) - marginY - menuHeight / 2;

		window.setAttributes(lp);
	}

	public void dismiss() {
		mHandler.removeMessages(MSG_PWD_ERROR_DISMISS);
		super.dismiss();
	}

	TextWatcher passwordInputTextWatcher = new TextWatcher() {

		public void afterTextChanged(Editable s) {
			if (s.toString().length() == 4) {
				
				mHandler.postDelayed(new Runnable() {

					public void run() {
						checkPassWord(password);
					}
				}, MSG_CHECK_DELAY);
				
			}
		}

		public void beforeTextChanged(CharSequence s, int start, int count,
				int after) {
		}

		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
		}
	};

	public void checkPassWord(String pwd) {
		mHandler.removeMessages(MSG_PWD_ERROR_DISMISS);
		if (mNavIntegration.isPasswordRight(mContext, pwd)) {
			mNavIntegration.iOpenLockedSourceAndChannel();
			dismiss();
			CheckLockSignalChannelState.getInstance(mContext)
			.checkLockedSignStateOrHasChannel(true);
			
		} else {
			showPasswordView(PASSWORD_VIEW_PWD_ERROR);
			mHandler.sendEmptyMessageDelayed(MSG_PWD_ERROR_DISMISS, 2000);
		}
	}

	public void showPasswordView(int mode) {
		pwdValue.setText(null);
		password ="";
		showPasswordStr="";
		switch (mode) {
		case PASSWORD_VIEW_PWD_INPUT:
			pwdView.setVisibility(View.VISIBLE);
			pwdError.setVisibility(View.GONE);
			this.mode = PASSWORD_VIEW_PWD_INPUT;
			break;

		case PASSWORD_VIEW_PWD_ERROR:
			pwdView.setVisibility(View.VISIBLE);
			pwdError.setVisibility(View.VISIBLE);
			this.mode = PASSWORD_VIEW_PWD_ERROR;
			break;
		}
	}

	public void inputChar(int num) {
		password=password +num;
		showPasswordStr=showPasswordStr+"*";
		pwdValue.setText(showPasswordStr);
	}

	public String getInputString() {
		if (password != null) {
			return password;
		}
		return null;
	}

	
}
