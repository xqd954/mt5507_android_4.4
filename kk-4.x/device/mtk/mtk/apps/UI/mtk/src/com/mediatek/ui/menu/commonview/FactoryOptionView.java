package com.mediatek.ui.menu.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.ui.R;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;

/**
 * FactoryOptionView is used in factory. This view includes a text view (the
 * name) and the second text view(the current value), it is one item of list
 * view items. you can press LEFT or RIGHT key to change the value
 * 
 * @author hs_yaoxiao
 */

public class FactoryOptionView extends ListViewItemView implements
		RespondedKeyEvent {

	// the item name and value of list view item
	private TextView mNameTextView;
	private TextView mShowView;
	private OnValueChangedListener mValueChangedListener;

	public FactoryOptionView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		this.context = context;
		init();
	}

	public FactoryOptionView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		init();
	}

	public FactoryOptionView(Context context) {
		super(context);
		this.context = context;
		init();
	}

	public int getValue() {
		return this.mDataItem.getmInitValue();
	}

	public void setValue(int mPositon) {
		this.mDataItem.setmInitValue(mPositon);
		mShowView.setText(this.mDataItem.getmOptionValue()[mPositon]);
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mPositon);
		}
	}

	public TextView getNameView() {
		return mNameTextView;
	}

	public TextView getmShowView() {
		return mShowView;
	}

	private void init() {
//		this.addView(inflate(context, R.layout.menu_factory_option_view, null));
		
		LinearLayout lv = (LinearLayout) inflate(mContext,
				R.layout.menu_factory_option_view, null);
		LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
		addView(lv, params);

		mNameTextView = (TextView) findViewById(R.id.factory_option_tv_itemname);
		mShowView = (TextView) findViewById(R.id.factory_option_tv_itemshow);
	}

	public void setValueChangedListener(
			OnValueChangedListener mValueChangedListener) {
		this.mValueChangedListener = mValueChangedListener;
	}

	public void setAdapter(SetConfigListViewAdapter.DataItem mDataItem) {
		this.mDataItem = mDataItem;
		mId = mDataItem.mItemID;
		setViewName(this.mDataItem.getmName());
		mShowView
				.setText(this.mDataItem.getmOptionValue()[this.mDataItem.mInitValue]);
	}

	private void setViewName(String viewName) {
		mNameTextView.setText(viewName);
	}

	protected void switchValueNext() {
		if (mDataItem.mInitValue != mDataItem.mOptionValue.length - 1) {
			mShowView.setText(mDataItem.mOptionValue[++mDataItem.mInitValue]);
		} else {
			mDataItem.mInitValue = 0;
			mShowView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
		}
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);
		}
	}

	protected void switchValuePrevious() {
		if (mDataItem.mInitValue != 0) {
			mShowView.setText(mDataItem.mOptionValue[--mDataItem.mInitValue]);
		} else {
			mDataItem.mInitValue = mDataItem.mOptionValue.length - 1;
			mShowView.setText(mDataItem.mOptionValue[mDataItem.mInitValue]);
		}
		if (mValueChangedListener != null) {
			mValueChangedListener.onValueChanged(this, mDataItem.mInitValue);
		}
	}

	// press ENTER
	public void onKeyEnter() {
	}

	// press LEFT
	public void onKeyLeft() {
		switchValuePrevious();
	}

	// press RIGHT
	public void onKeyRight() {
		switchValueNext();
	}

	/**
	 * item show the value
	 */
	public void showValue(int value) {
		if (value < 0 || value > mDataItem.mOptionValue.length - 1) {
			throw new IllegalArgumentException("value is Illegal value");
		}
		mDataItem.mInitValue = value;
		mShowView.setText(mDataItem.mOptionValue[value]);
	}

}
