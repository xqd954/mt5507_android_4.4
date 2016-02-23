package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.os.Handler;
import android.util.AttributeSet;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;

public class PasswordView extends ListViewItemView implements RespondedKeyEvent {

	private TextView mNameView;
	private EditText mShowView;
	private String passwd = "";
	private PasswordView passwordNew = null;
	private Handler handler;
	private boolean mInputNum = false;
	private static PasswordView self;
	
	public static PasswordView getInstance(){
	    if (self!=null) {
            return self;
        }else {
            return null;
        }
	}

	public PasswordView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public PasswordView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public PasswordView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	public EditText getmShowView() {
		return mShowView;
	}

	public int getValue() {
		return 0;
	}

	public void setValue(int mPositon) {
	}

	private void init() {
	    self = this;
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_password_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);
		mNameView = (TextView) findViewById(R.id.common_password_itemname);
		mShowView = (EditText) findViewById(R.id.common_password_content);
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		setViewName(this.mDataItem.getmName());
	}

	public void inputPassword(String pwd) {
		if (pwd.length() == 0) {
			passwd = "";
		} else {
			passwd = passwd + pwd;
		}
		mShowView.setText(passwd);
	}

	public void setInputNum(boolean mFlag) {
		mInputNum = mFlag;
	}

	public boolean dealPassword(int mExitLevel, String _mStr,
			final MenuSetUpListView mMenuSetUpListView) {

		if (mExitLevel == 1) {
			inputPassword(_mStr);
			if (mShowView.getText().toString().length() == 4) {
				String pwd = SaveValue.getInstance(context).readStrValue(
						"password");
				if (mShowView.getText().toString().equals(pwd)) {
					if (!mInputNum) {
						mInputNum = true;
						return false;
					}
				} else {
					showErrorInfo(context
							.getString(R.string.menu_parental_password_incorrect));
				}
			}
			return true;
		} else if (mExitLevel == 3) {
			inputPassword(_mStr);
			if (mId.equals(MenuConfigManager.PARENTAL_PASSWORD_NEW)) {
				if (mShowView.getText().toString().length() == 4) {
					mMenuSetUpListView.setSelection(1);
				}
				return true;
			} else {
				if (mShowView.getText().toString().length() == 4) {
					if (mShowView
							.getText()
							.toString()
							.equals(
									((EditText) mMenuSetUpListView
											.getChildAt(0)
											.findViewById(
													R.id.common_password_content))
											.getText().toString())) {

						SaveValue.getInstance(context).saveStrValue("password",
								mShowView.getText().toString());
						return false;
					} else {
						handler = new Handler();
						handler.postDelayed(new Runnable() {

							public void run() {
								// TODO Auto-generated method stub
								((PasswordView) mMenuSetUpListView
										.getChildAt(0)).inputPassword("");
								mMenuSetUpListView.setSelection(0);
							}
						}, 900);
						showErrorInfo(context
								.getString(R.string.menu_parental_password_matchinfo));
					}
				}
				return true;
			}
		}
		return false;

	}

	public void onKeyEnter() {
	}

	public void onKeyLeft() {
	}

	public void onKeyRight() {
	}

	public void showValue(int value) {
	}

	private void showErrorInfo(String info) {
		handler = new Handler();
		// inputPassword("");
		ToastDialog toast = new ToastDialog(context);
		// textView.setWidth(200);
		toast.setLayoutParams(new LayoutParams(
				LinearLayout.LayoutParams.WRAP_CONTENT,
				LinearLayout.LayoutParams.WRAP_CONTENT));
		toast.setText(info);
		toast.setPositon(120, 40);// set the location of display
		handler.post(toast);
		handler.postDelayed(new Runnable() {
			public void run() {
				inputPassword("");
			}
		}, 900);
	}

}
