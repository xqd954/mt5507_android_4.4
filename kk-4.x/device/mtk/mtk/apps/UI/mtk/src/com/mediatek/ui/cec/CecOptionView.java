package com.mediatek.ui.cec;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.OnValueChangedListener;
import com.mediatek.ui.menu.commonview.RespondedKeyEvent;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.MtkLog;
import android.os.SystemProperties;

/**
 * This view includes a text view (the name),the second text view(the current
 * value) and a ImageView(selected or not), it is one item of list view items.
 * you can press LEFT or RIGHT key to change value of the second text view
 * 
 * @author hs_yaoxiao
 */

public class CecOptionView extends CecListItemView implements RespondedKeyEvent {

	private static final String TAG = "OptionView";
	private TextView mNameView;
	private TextView mValueView;
	private OnValueChangedListener mValueChangedListener;
	private MenuConfigManager mConfig;
	private SaveValue saveV;

	public CecOptionView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public CecOptionView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public CecOptionView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public int getValue() {
		return this.mDataItem.getInitValue();
	}

	public void setValue(int mPositon) {
		this.mDataItem.setInitValue(mPositon);
		mValueView.setText(this.mDataItem.getmOptionValue()[mPositon]);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mPositon);
		}
	}

	private void setViewName(String viewName) {
		mNameView.setText(viewName);
	}

	public TextView getNameView() {
		return mNameView;
	}

	public TextView getValueView() {
		return mValueView;
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	private void init() {
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_option_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);
		mNameView = (TextView) findViewById(R.id.common_tv_itemname);
		mValueView = (TextView) findViewById(R.id.common_tv_itemshow);
		mConfig = MenuConfigManager.getInstance(context);
		saveV = SaveValue.getInstance(context);
	}

	public void setAdapter(CecListviewAdapter.CecDataItem mDataItem) {
		this.mDataItem = mDataItem;
		mId = mDataItem.mItemID;
		setViewName(this.mDataItem.getName());
			mValueView
					.setText(this.mDataItem.mOptionValue[this.mDataItem.mInitValue]);

	}

	protected void switchValueNext() {
			if (mDataItem.mInitValue != mDataItem.mOptionValue.length - 1) {
				mValueView
						.setText(mDataItem.mOptionValue[++mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = 0;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
			if(mConfig.FAST_BOOT.equals(mDataItem.mItemID)){
				saveV.saveValue(mConfig.FAST_BOOT,mDataItem.mInitValue);
				SystemProperties.set("sys.pm.warmboot",mDataItem.mInitValue+"");
			}
		// if (mValueChangedListener != null
		// && !mId.equals(MenuConfigManager.NETWORK_INTERFACE)
		// && !mId.equals(MenuConfigManager.NETWORK_CONNECTION)) {
		mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);

		MtkLog.v(TAG, "" + mDataItem.mInitValue);
	}

	protected void switchValuePrevious() {
			if (mDataItem.mInitValue != 0) {
				mValueView
						.setText(mDataItem.mOptionValue[--mDataItem.mInitValue]);
			} else {
				mDataItem.mInitValue = mDataItem.mOptionValue.length - 1;
				mValueView
						.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
			}
			if(mConfig.FAST_BOOT.equals(mDataItem.mItemID)){
				saveV.saveValue(mConfig.FAST_BOOT,mDataItem.mInitValue);
				SystemProperties.set("sys.pm.warmboot",mDataItem.mInitValue+"");
			}
		// if (mValueChangedListener != null
		// && !mId.equals(MenuConfigManager.NETWORK_INTERFACE)) {
		mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);

	}

	public void onKeyEnter() {
	}

	public void onKeyLeft() {
	    Log.e("cec", "onkeyleft");
//		if (mDataItem.isEnable) {
			switchValuePrevious();
//		}
	}

	public void onKeyRight() {
	    Log.e("cec", "onkeyright");
//		if (mDataItem.isEnable) {
			switchValueNext();
//		}
	}

	public void showValue(int value) {
		if (value < 0 || value > mDataItem.mOptionValue.length - 1) {
			throw new IllegalArgumentException("value is Illegal value");
		}
		mDataItem.mInitValue = value;
		mValueView.setText(mDataItem.mOptionValue[value]);
	}
}
