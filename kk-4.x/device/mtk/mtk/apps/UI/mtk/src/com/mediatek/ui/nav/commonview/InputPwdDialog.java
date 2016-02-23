package com.mediatek.ui.nav.commonview;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.util.CICardDelayNotification;
import com.mediatek.ui.nav.util.CheckLockSignalChannelState;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.ScreenConstant;

public class InputPwdDialog extends Dialog {

	private static final String TAG = "InputPwdDialog";

	private Context mContext;

	private NavIntegration mNavIntegration;

	private static final int MSG_DISMISS = 1;
	private static final int MSG_DISMISS_DELAY = 10000;
	private static final int MSG_CHECK_DELAY = 500;
	public static final int PASSWORD_VIEW_PWD_INPUT = 1;
	public static final int PASSWORD_VIEW_PWD_ERROR = 2;
	public static final int PASSWORD_VIEW_GONE = 3;
	public int mode = PASSWORD_VIEW_GONE;
    private String password ="";
    private String showPasswordStr="";
    
    private static final String PWD_CHAR = "*";
    
    private BannerView mBannerView;
    
    private PippopUiLogic mPippopUiLogic;
    
    
	public InputPwdDialog(Context context, int theme) {
		super(context, theme);
		mPippopUiLogic = PippopUiLogic.getInstance(context);
		setContentView(R.layout.nav_input_pwd_view);
		findViews();
		setOnKeyListener(new OnKeyListener() {

			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					switch (keyCode) {
					case KeyMap.KEYCODE_DPAD_CENTER:
						if (mode == PASSWORD_VIEW_PWD_ERROR) {
							mHandler.removeMessages(MSG_DISMISS);
							showPasswordView(PASSWORD_VIEW_PWD_INPUT);
							mHandler.sendEmptyMessageDelayed(MSG_DISMISS,
									MSG_DISMISS_DELAY);
						} else if (mode == PASSWORD_VIEW_PWD_INPUT) {
							String inputStr = getInputString();
							if (null == inputStr || inputStr.length() == 0) {
								dismiss();
							} else {
								checkPassWord(inputStr);
							}
						}
						return true;
					default:
						break;
					}
				}
				return false;
			}
		});
	}

	public InputPwdDialog(Context context) {
		this(context, R.style.nav_dialog);
		mPippopUiLogic = PippopUiLogic.getInstance(context);
		mContext = context;
		setContentView(R.layout.nav_input_pwd_view);
		password ="";
		showPasswordStr="";
		findViews();
	}
	
	public void setBannerView(BannerView mBannerView){
		this.mBannerView=mBannerView;
	}

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_DISMISS: {
				dismiss();
				if(!mPippopUiLogic.isPipOrPopState()){
				CheckLockSignalChannelState.getInstance(mContext)
						.checkLockedSignStateOrHasChannel(false);
				}
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
		// TODO
		mNavIntegration = NavIntegration.getInstance(mContext);

	}

	protected void onStart() {
		password ="";
		showPasswordStr="";
//		pwdValue.setFocusable(true);
//		pwdValue.requestFocus();
		showPasswordView(PASSWORD_VIEW_PWD_INPUT);
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {

		switch (event.getKeyCode()) {
		case KeyMap.KEYCODE_MENU:
		case KeyMap.KEYCODE_BACK:
		case KeyMap.KEYCODE_VOLUME_UP:
		case KeyMap.KEYCODE_VOLUME_DOWN:
			dismiss();
			if(keyCode==KeyMap.KEYCODE_BACK){
				if(mBannerView.getTotalVisibility()==View.VISIBLE){
					return true;
				}else{
					break;
				}				
			}
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
			mHandler.removeMessages(MSG_DISMISS);
			inputChar(keyCode - 7);
			mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
			return true;

		default:
			break;
		}

		if (null != mContext && mContext instanceof TurnkeyUiMainActivity) {
			((TurnkeyUiMainActivity) mContext).onKeyDown(keyCode, event);
		}

		if (keyCode == KeyMap.KEYCODE_VOLUME_UP
				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
			return true;
		}

		return super.onKeyDown(keyCode, event);

	};

	RelativeLayout pwdLayout;
	LinearLayout pwdView;
	TextView pwdError;
	TextView pwdValue;
	TextView pwdName;

	private void findViews() {
		pwdLayout = (RelativeLayout) findViewById(R.id.nav_input_password_layout);
		pwdError = (TextView) findViewById(R.id.nav_tv_pwd_error);
		pwdView = (LinearLayout) findViewById(R.id.nav_tv_pwd_view);
		pwdValue = (TextView) findViewById(R.id.nav_tv_pwd_value);
		pwdName = (TextView) findViewById(R.id.nav_tv_pwd_name);
		pwdValue.setInputType(EditorInfo.TYPE_NULL);
		pwdValue.addTextChangedListener(passwordInputTextWatcher);
	}
	
	public void updateConfigration(){
		pwdName.setText(mContext.getString(R.string.nav_parent_psw));
	}

	private int marginX;
	private int marginY;
	private static int menuWidth = 494 + 329;
	private static int menuHeight = 420;

	public void setWindowPosition() {
		// TODO
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();
        
		marginX = 240;
		marginY = 4;
		marginY = 4*ScreenConstant.SCREEN_HEIGHT / 720;
		marginX = ScreenConstant.SCREEN_WIDTH*240/1280;
		
		menuWidth = (494 + 329)*ScreenConstant.SCREEN_WIDTH /1280;
		menuHeight = 420*ScreenConstant.SCREEN_HEIGHT /720;
		lp.width = menuWidth;
		lp.height = menuHeight;

		lp.x = ScreenConstant.SCREEN_WIDTH / 2 - menuWidth / 2 - marginX;
		lp.y = (int) (ScreenConstant.SCREEN_HEIGHT / 2) - marginY - menuHeight / 2;

		window.setAttributes(lp);
	}

	@Override
	public void dismiss() {
		mHandler.removeMessages(MSG_DISMISS);
//		pwdLayout.setVisibility(View.GONE);
		pwdLayout.setVisibility(View.INVISIBLE);
		mode = PASSWORD_VIEW_GONE;
		super.dismiss();
	}

	TextWatcher passwordInputTextWatcher = new TextWatcher() {

		public void afterTextChanged(Editable s) {
			if (s.toString().length() == 4) {
				mHandler.postDelayed(new Runnable() {

					public void run() {
						// TODO Auto-generated method stub
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
		if (mNavIntegration.isPasswordRight(mContext, pwd)) {
			// TODO password true
			mNavIntegration.iOpenLockedSourceAndChannel();
			dismiss();
			CheckLockSignalChannelState.getInstance(mContext)
			.checkLockedSignStateOrHasChannel(true);
			CICardDelayNotification.getInstance().setPushToQueue(false);
			CICardDelayNotification.getInstance().runDelayNotifications();
		} else {
			// TODO password false
			mHandler.removeMessages(MSG_DISMISS);
			mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
			showPasswordView(PASSWORD_VIEW_PWD_ERROR);
		}
	}

	public void showPasswordView(int mode) {
		pwdValue.setText(null);
		password ="";
		showPasswordStr="";
		switch (mode) {
		case PASSWORD_VIEW_PWD_INPUT:
			pwdLayout.setVisibility(View.VISIBLE);
			pwdView.setVisibility(View.VISIBLE);
			pwdError.setVisibility(View.GONE);
			this.mode = PASSWORD_VIEW_PWD_INPUT;
			break;

		case PASSWORD_VIEW_PWD_ERROR:
			pwdLayout.setVisibility(View.VISIBLE);
			pwdError.setVisibility(View.VISIBLE);
			pwdView.setVisibility(View.GONE);
			this.mode = PASSWORD_VIEW_PWD_ERROR;
			break;

		}
	}

	public void inputChar(int num) {
		if(password.length()>=4){
			return;
		}
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
